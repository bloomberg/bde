// bcec_multipriorityqueue.t.cpp       -*-C++-*-

#include <bcec_multipriorityqueue.h>

#include <bcemt_barrier.h>
#include <bcemt_lockguard.h>
#include <bcemt_semaphore.h>
#include <bcema_testallocator.h>
#include <bcemt_thread.h>
#include <bcemt_threadgroup.h>
#include <bces_atomictypes.h>

#include <bdetu_systemtime.h>

#include <bslma_allocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsl_algorithm.h>
#include <bsl_list.h>
#include <bsl_string.h>

#include <bsl_cerrno.h>
#include <bsl_climits.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cin;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//
// The component under test is a thread-aware mechanism.
//-----------------------------------------------------------------------------
//
// CREATORS
// [ 4] bcec_MultipriorityQueue()
// [ 4] bcec_MultipriorityQueue(numPriorities)
// [ 4] bcec_MultipriorityQueue(alloc)
// [ 4] bcec_MultipriorityQueue(numPriorities, alloc)
// [ 6] ~bcec_MultipriorityQueue()
//
// MANIPULATORS
// [ 2] pushBack(item, priority)
// [ 2] popFront(&item)
// [ 2] tryPopFront(&item)
// [ 3] popFront(&item, &priority)
// [ 3] tryPopFront(&item, &priority)
// [ 6] removeAll()
//
// ACCESSORS
// [ 4] numPriorities()
// [ 2] length()
// [ 2] isEmpty()
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING PRIMARY MANIPULATORS (BOOTSTRAP, SINGLE THREADED):
// [ 3] MORE THOROUGH TESTING OF PRIMARY MANIPULATORS
// [ 4] TESTING ALL CONSTRUCTORS AND numPriorities() ACCESSOR
// [ 5] POPPED OBJECTS - SORTED BY PRIORITY AND FIFO WITHIN PRIORITY
// [ 7] BLOCKING BEHAVIOR
// [ 8] STRESS MULTITHREADING TEST
// [ 9] STRESS REMOVEALL TEST
// [10] TESTING USAGE OF PROPER MEMORY MEMORY ALLOCATOR
// [11] EXCEPTION SAFETY OF PUSHBACK, POPFRONT
// [12] EXCEPTION SAFETY DURING ALL ALLOCATIONS
// [13] USAGE EXAMPLE 2
// [14] USAGE EXAMPLE 1
//
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

bcemt_Mutex coutMutex;

#define COUT  { coutMutex.lock(); bsl::cout
#define ENDL  bsl::endl; coutMutex.unlock(); }
#define FLUSH bsl::flush; coutMutex.unlock(); }

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        COUT << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << ENDL;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

int verbose;
int veryVerbose;
int veryVeryVerbose;
int veryVeryVeryVerbose;

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { COUT << #I << ": " << I << ENDL; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { COUT << #I << ": " << I << "\t" << #J << ": " \
              << J << ENDL; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { COUT << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << ENDL; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { COUT << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << ENDL; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { COUT << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << ENDL; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { COUT << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << ENDL; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) COUT << #X " = " << (X) << ENDL // Print identifier and value.
#define Q(X) COUT << "<| " #X " |>" << ENDL  // Quote identifier literally.
#define P_(X) COUT << #X " = " << (X) << ", "<< FLUSH // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  COUT << '\t' << FLUSH           // Print tab w/o newline

//=============================================================================
//              GLOBAL TYPEDEFS, CONSTANTS & VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

// Struct 'Element' is used in many examples, it behaves similarly to a double,
// but it also keeps track of the number instances of it in existence, useful
// for detecting leaks.

struct Element {
    static bces_AtomicInt s_allocCount;
    double                d_data;

    Element() {
        ++s_allocCount;
    }
    Element(double e) {
        ++s_allocCount;

        d_data = e;
    }
    Element(const Element& original) {
        ++s_allocCount;

        d_data = original.d_data;
    }
    ~Element() {
        --s_allocCount;
    }

    operator double() const {
        return d_data;
    }
};
bces_AtomicInt Element::s_allocCount = 0;

typedef bcec_MultipriorityQueue<Element>      Obj;
typedef bcec_MultipriorityQueue<double>       Dobj;
typedef bcec_MultipriorityQueue<int>          Iobj;
typedef bcec_MultipriorityQueue<bsl::string>  Sobj;

}  // close unnamed namespace

//=============================================================================
//             TYPES AND FUNCTIONS FOR TEST CASE - USAGE EXAMPLE 1
//-----------------------------------------------------------------------------

namespace BCEC_MULTIPRIORITYQUEUE_TEST_USAGE_1 {

enum {
    MAX_CONSUMER_THREADS = 10
};

struct MyWorkData {
    int d_i;
};

struct MyWorkRequest {
    enum RequestType {
          WORK = 1
        , STOP = 2
    };

    RequestType d_type;
    MyWorkData  d_data;
};
void myDoWork(MyWorkData& data)
{
    int j = data.d_i * data.d_i;

    if (veryVerbose) {
        COUT << "Work: " << j << ENDL;
    }
}

int getWorkData(MyWorkData *result)
{
    static int count = 0;
    result->d_i = bsl::rand();   // Only one thread runs this routine, so
                                 // it does not matter that rand() is not
                                 // thread-safe.

    return ++count >= 100;
}

void myConsumer(bcec_MultipriorityQueue<MyWorkRequest> *queue)
{
    MyWorkRequest item;
    while (1) {

        // The 'popFront' function will wait for a 'MyWorkRequest' until one
        // is available.

        queue->popFront(&item);

        if (MyWorkRequest::STOP == item.d_type) {
            break;
        }

        myDoWork(item.d_data);
    }
}

extern "C" void *myConsumerThread(void *queuePtr)
{
    myConsumer ((bcec_MultipriorityQueue<MyWorkRequest>*) queuePtr);
    return queuePtr;
}

}  // close namespace BCEC_MULTIPRIORITYQUEUE_TEST_USAGE_1

//=============================================================================
//            TYPES AND FUNCTIONS FOR TEST CASE - USAGE EXAMPLE 2
//-----------------------------------------------------------------------------

namespace BCEC_MULTIPRIORITYQUEUE_TEST_USAGE_2 {

enum {
    MAX_CONSUMER_THREADS = 10
  , MAX_EVENT_TEXT = 80
};

struct MyEvent {
    enum EventType {
        IN_PROGRESS   = 1
      , TASK_COMPLETE = 2
    };

    EventType d_type;
    int       d_workerId;
    int       d_eventNumber;
    char      d_eventText[MAX_EVENT_TEXT];
};

struct MyWorkerData {
    int                               d_workerId;
    bcec_MultipriorityQueue<MyEvent> *d_queue;
};

void myWorker(int workerId, bcec_MultipriorityQueue<MyEvent> *queue)
{
    const int N = queue->numPriorities();
    const int NUM_EVENTS = 5;
    int eventNumber;    // used also to generate mixed priorities

    // First push 'NUM_EVENTS' in this queue with mixed priorities.

    for (eventNumber = 0; eventNumber < NUM_EVENTS; ++eventNumber) {
        MyEvent ev = {
            MyEvent::IN_PROGRESS,
            workerId,
            eventNumber,
            "In-Progress Event"         // constant (for simplicity)
        };
        queue->pushBack(ev, eventNumber % N);       // mixed priorities
    }

    // Now push an event to end this task.

    MyEvent ev = {
        MyEvent::TASK_COMPLETE,
        workerId,
        eventNumber,
        "Task Complete"
    };
    queue->pushBack(ev, N - 1);                     // lowest priority
}

extern "C" void *myWorkerThread(void *vWorkerPtr)
{
    MyWorkerData *workerPtr = (MyWorkerData *)vWorkerPtr;
    myWorker(workerPtr->d_workerId, workerPtr->d_queue);
    return vWorkerPtr;
}

}  // close namespace BCEC_MULTIPRIORITYQUEUE_TEST_USAGE_2

//=============================================================================
//                            TYPE FOR TEST CASE 11
//-----------------------------------------------------------------------------

namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_11 {

// The goal of this test is to make sure that pushes and pops are exception
// safe.  Push copies the object into the queue, pops assign the object
// from the queue.  So what we need is a type that we can program to throw
// exceptions either during push or pop, or in neither case.  We call this
// type 'Thrower'.

struct Thrower {
    Element d_value;            // use Element type to monitor for leaks.
    bool d_throwOnCopy;
    bool d_throwOnAssign;

  public:
    Thrower(double value, bool throwOnCopy, bool throwOnAssign);
    Thrower(const Thrower& original);
    Thrower& operator=(const Thrower& original);
};

Thrower::Thrower(double value, bool throwOnCopy, bool throwOnAssign) {
    d_value = value;
    d_throwOnCopy = throwOnCopy;
    d_throwOnAssign = throwOnAssign;
}

Thrower::Thrower(const Thrower& original) {
    if (original.d_throwOnCopy) {
        throw 1;
    }

    d_value         = original.d_value;
    d_throwOnCopy   = original.d_throwOnCopy;
    d_throwOnAssign = original.d_throwOnAssign;
}

Thrower& Thrower::operator=(const Thrower& original) {
    if (original.d_throwOnAssign) {
        throw 1;
    }

    d_value         = original.d_value;
    d_throwOnCopy   = original.d_throwOnCopy;
    d_throwOnAssign = original.d_throwOnAssign;

    return *this;
}

}  // close namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_11

//=============================================================================
//                                  TEST CASE 9
//-----------------------------------------------------------------------------

namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_9 {

double testStartedTime;

struct ProducerThread {
    static int             s_numPriorities;
    static int             s_numItemsPerProducer;
    static bces_AtomicInt  s_pushVal;
    static bcemt_Barrier  *s_barrier;
    static Obj            *s_queue_p;
    static int             s_removeMask;

    int operator()() {
        int pushPriority = 0;

        s_barrier->wait();

        for (int i = 0; s_numItemsPerProducer > i; ++i) {
            if (s_numPriorities <= ++pushPriority) {
                pushPriority = 0;
            }

            Element e = s_pushVal++;
            s_queue_p->pushBack(e, pushPriority);

            if ((s_removeMask & i) == s_removeMask) {
                s_queue_p->removeAll();
            }
        }

        if (veryVerbose) {
            double doneTime = bdetu_SystemTime::now().totalSecondsAsDouble() -
                                                               testStartedTime;
            COUT << "Producer finishing after " << doneTime << " seconds" <<
                                                                        ENDL;
        }

        return 0;
    }
};
int             ProducerThread::s_numPriorities;
int             ProducerThread::s_numItemsPerProducer;
bces_AtomicInt  ProducerThread::s_pushVal = 0;
bcemt_Barrier  *ProducerThread::s_barrier;
Obj            *ProducerThread::s_queue_p;
int             ProducerThread::s_removeMask;

struct OutPair {
    int d_value;
    int d_priority;
};

struct OutPairValueLess {
    bool operator()(const OutPair& lhs, const OutPair& rhs) {
        return lhs.d_value < rhs.d_value;
    }
};

struct OutPairPriorityLess {
    bool operator()(const OutPair& lhs, const OutPair& rhs) {
        return lhs.d_priority < rhs.d_priority;
    }
};

struct ConsumerThread {
    enum {
        ILLEGAL_VAL = INT_MAX
    };

    static bcemt_Barrier  *s_barrier;
    static Obj            *s_queue_p;
    static OutPair        *s_outPairVec;
    static bces_AtomicInt *s_outPairVecIdx;

    int operator()() {
        s_barrier->wait();

        while (true) {
            OutPair outPair = { ILLEGAL_VAL, 0 };

            Element e;
            s_queue_p->popFront(&e, &outPair.d_priority);
            outPair.d_value = e;

            if (0 > outPair.d_value) {
                break;
            }
            ASSERT(ILLEGAL_VAL != outPair.d_value);
            if (ILLEGAL_VAL == outPair.d_value) {
                break;
            }

            s_outPairVec[(*s_outPairVecIdx)++] = outPair;
        }

        if (veryVerbose) {
            double doneTime = bdetu_SystemTime::now().totalSecondsAsDouble() -
                                                               testStartedTime;
            COUT << "Consumer done after " << doneTime << " seconds" << ENDL;
        }

        return 0;
    }
};
bcemt_Barrier  *ConsumerThread::s_barrier;
Obj            *ConsumerThread::s_queue_p;
OutPair        *ConsumerThread::s_outPairVec;
bces_AtomicInt *ConsumerThread::s_outPairVecIdx;

}  // close namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_9

//=============================================================================
//                                  TEST CASE 8
//-----------------------------------------------------------------------------

namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_8 {

double testStartedTime;

struct ProducerThread {
    static int             s_numPriorities;
    static int             s_numItemsPerProducer;
    static bces_AtomicInt  s_pushVal;
    static bcemt_Barrier  *s_barrier;
    static Iobj           *s_queue_p;

    int operator()() {
        int pushPriority = 0;

        s_barrier->wait();

        for (int i = 0; s_numItemsPerProducer > i; ++i) {
            if (s_numPriorities <= ++pushPriority) {
                pushPriority = 0;
            }

            s_queue_p->pushBack(s_pushVal++, pushPriority);
        }

        if (veryVerbose) {
            double doneTime = bdetu_SystemTime::now().totalSecondsAsDouble() -
                                                               testStartedTime;
            COUT << "Producer finishing after " << doneTime << " seconds" <<
                                                                        ENDL;
        }

        return 0;
    }
};
int             ProducerThread::s_numPriorities;
int             ProducerThread::s_numItemsPerProducer;
bces_AtomicInt  ProducerThread::s_pushVal = 0;
bcemt_Barrier  *ProducerThread::s_barrier;
Iobj           *ProducerThread::s_queue_p;

struct OutPair {
    int d_value;
    int d_priority;
};

struct OutPairValueLess {
    bool operator()(const OutPair& lhs, const OutPair& rhs) {
        return lhs.d_value < rhs.d_value;
    }
};

struct OutPairPriorityLess {
    bool operator()(const OutPair& lhs, const OutPair& rhs) {
        return lhs.d_priority < rhs.d_priority;
    }
};

struct ConsumerThread {
    enum {
        ILLEGAL_VAL = INT_MAX
    };

    static bcemt_Barrier  *s_barrier;
    static Iobj           *s_queue_p;
    static OutPair        *s_outPairVec;
    static bces_AtomicInt *s_outPairVecIdx;

    int operator()() {
        s_barrier->wait();

        while (true) {
            OutPair outPair = { ILLEGAL_VAL, 0 };

            s_queue_p->popFront(&outPair.d_value, &outPair.d_priority);
            if (0 > outPair.d_value) {
                break;
            }
            ASSERT(ILLEGAL_VAL != outPair.d_value);
            if (ILLEGAL_VAL == outPair.d_value) {
                break;
            }

            s_outPairVec[(*s_outPairVecIdx)++] = outPair;
        }

        if (veryVerbose) {
            double doneTime = bdetu_SystemTime::now().totalSecondsAsDouble() -
                                                               testStartedTime;
            COUT << "Consumer done after " << doneTime << " seconds" << ENDL;
        }

        return 0;
    }
};
bcemt_Barrier  *ConsumerThread::s_barrier;
Iobj           *ConsumerThread::s_queue_p;
OutPair        *ConsumerThread::s_outPairVec;
bces_AtomicInt *ConsumerThread::s_outPairVecIdx;

}  // close namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_8

//=============================================================================
//                                  TEST CASE 7
//-----------------------------------------------------------------------------

namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_7 {

struct TestFunctor7 {
    enum {
        GARBAGE_VALUE = -178,
        PUSH_PRIORITY  = 4,
        FIRST_PUSHVAL  = 1,
        SECOND_PUSHVAL = 2
    };

    Obj *d_pMX;
    bcemt_Barrier *d_barrier;
    Element d_e;
    int     d_priority;

    void setGarbage() {
        d_e        = GARBAGE_VALUE;
        d_priority = GARBAGE_VALUE;
    }

    TestFunctor7(Obj *pMX, bcemt_Barrier *barrier)
    : d_pMX(pMX)
    , d_barrier(barrier)
    {
        setGarbage();
    }

    void operator()() {
        // the queue is empty, verify tryPopFront does not block
        setGarbage();
        ASSERT(0 != d_pMX->tryPopFront(&d_e));
        ASSERT(GARBAGE_VALUE == d_e);
        ASSERT(GARBAGE_VALUE == d_priority);

        setGarbage();
        ASSERT(0 != d_pMX->tryPopFront(&d_e, &d_priority));
        ASSERT(GARBAGE_VALUE == d_e);
        ASSERT(GARBAGE_VALUE == d_priority);

        d_barrier->wait();
        // the main thread now can start pushing stuff, sleeping and yielding
        // between pushes

        setGarbage();
        d_pMX->popFront(&d_e);
        ASSERT(FIRST_PUSHVAL == d_e);

        setGarbage();
        d_pMX->popFront(&d_e, &d_priority);
        ASSERT(SECOND_PUSHVAL == d_e);
        ASSERT(PUSH_PRIORITY == d_priority);
    }
};

}  // close namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_7

//=============================================================================
//                                  TEST CASE 5
//-----------------------------------------------------------------------------

namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_5 {

struct PushPoint {
    double d_value;
    int    d_priority;
    bool operator<(const PushPoint& rhs) const {
        // for sorting by priority
        return this->d_priority < rhs.d_priority;
    }
};

}  // close namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_5

//=============================================================================
//                                MAIN PROGRAM
//=============================================================================

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bcema_TestAllocator taDefault(veryVeryVeryVerbose);
    bcema_TestAllocator ta(veryVeryVeryVerbose);  // passed to
                                                  // multipriority queue

    bslma_DefaultAllocatorGuard guard(&taDefault);

    switch (test) { case 0:
      case 16: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.  Make use of existing test apparatus by instantiating
        //   objects with a 'bcema_TestAllocator' object where applicable.
        //   Additionally, replace all calls to 'assert' in the usage example
        //   with calls to 'ASSERT'.  This now becomes the source, which is
        //   then "copied" back to the header file by reversing the above
        //   process.
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        using namespace BCEC_MULTIPRIORITYQUEUE_TEST_USAGE_1;

        enum {
            NUM_PRIORITIES = 8,
            NUM_THREADS = 8
        };

        bcema_TestAllocator ta;

        MyWorkRequest item;
        MyWorkData workData;

        // Create Multi-Priority Queue with specified number of priorities.

        bcec_MultipriorityQueue<MyWorkRequest> queue(NUM_PRIORITIES, &ta);

        // Start the specified number of threads.

        ASSERT(0 < NUM_THREADS && NUM_THREADS <= MAX_CONSUMER_THREADS);
        bcemt_ThreadUtil::Handle consumerHandles[MAX_CONSUMER_THREADS];

        for (int i = 0; i < NUM_THREADS; ++i) {
            bcemt_ThreadUtil::create(&consumerHandles[i],
                                     myConsumerThread,
                                     &queue);
        }

        // Load work data into work requests and push them onto the queue
        // with varying priority until all work data has been exhausted.

        int count = 0;                      // used to generate priorities

        while (!getWorkData(&workData)) {       // see declaration (above)
            item.d_type = MyWorkRequest::WORK;
            item.d_data = workData;
            queue.pushBack(item, count % NUM_PRIORITIES);
                                                    // mixed priorities
            ++count;
        }

        // Load as many stop requests as there are active consumer threads.

        for (int i = 0; i < NUM_THREADS; ++i) {
            item.d_type = MyWorkRequest::STOP;
            queue.pushBack(item, NUM_PRIORITIES - 1);   // lowest priority
        }

        // Join all of the consumer threads back with the main thread.

        for (int i = 0; i < NUM_THREADS; ++i) {
            bcemt_ThreadUtil::join(consumerHandles[i]);
        }
      }  break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.  Make use of existing test apparatus by instantiating
        //   objects with a 'bcema_TestAllocator' object where applicable.
        //   Additionally, replace all calls to 'assert' in the usage example
        //   with calls to 'ASSERT'.  This now becomes the source, which is
        //   then "copied" back to the header file by reversing the above
        //   process.
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        using namespace BCEC_MULTIPRIORITYQUEUE_TEST_USAGE_2;

        const int NUM_THREADS = 10;
        const int NUM_PRIORITIES = 4;

        bcec_MultipriorityQueue<MyEvent> queue(NUM_PRIORITIES, &ta);

        ASSERT(0 < NUM_THREADS && NUM_THREADS <= MAX_CONSUMER_THREADS);
        bcemt_ThreadUtil::Handle workerHandles[MAX_CONSUMER_THREADS];

        // Create 'NUM_THREADS', each holding a unique value for 'd_workerId'.

        MyWorkerData workerData[NUM_THREADS];
        for (int i = 0; i < NUM_THREADS; ++i) {
            workerData[i].d_queue = &queue;
            workerData[i].d_workerId = i;
            bcemt_ThreadUtil::create(&workerHandles[i],
                                     myWorkerThread,
                                     &workerData[i]);
        }

        // Now wait and print out each of the 'd_workerId' values as the
        // threads complete.  This function ends after a total of
        // 'NUM_THREADS' 'MyEvent::TASK_COMPLETE' events have been printed.

        int nStop = 0;
        while (nStop < NUM_THREADS) {
            MyEvent ev;
            queue.popFront(&ev);
            if (veryVerbose) {
                bsl::cout << "[" << ev.d_workerId << "] "
                          << ev.d_eventNumber << ". "
                          << ev.d_eventText << bsl::endl;
            }
            if (MyEvent::TASK_COMPLETE == ev.d_type) {
                ++nStop;
                bcemt_ThreadUtil::join(workerHandles[ev.d_workerId]);
            }
        }
      }  break;
      case 14: {
        // --------------------------------------------------------------------
        // TEST MULTIPLE PUSH RAW FUNCTIONS
        //
        // Concerns:
        //   That pushMultiple{Back, Front}Raw() work
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing multiple push raw functions\n"
                             "===================================\n";

        ASSERT(0 == ta.numBytesInUse());

        for (int i = 0; i < 2; ++i) {
            bool enabled = i;

            bcec_MultipriorityQueue<int> mpq(&ta);

            mpq.pushBack(4, 3);
            mpq.pushBack(6, 4);

            if (enabled) {
                mpq.enable();
            }
            else {
                mpq.disable();
            }

            mpq.pushBackMultipleRaw(5, 3, 3);

            ASSERT(5 == mpq.length());

            int val, pri;

            mpq.popFront(&val, &pri);
            ASSERT(4 == val && 3 == pri);
            mpq.popFront(&val, &pri);
            ASSERT(5 == val && 3 == pri);
            mpq.popFront(&val, &pri);
            ASSERT(5 == val && 3 == pri);
            mpq.popFront(&val, &pri);
            ASSERT(5 == val && 3 == pri);
            mpq.popFront(&val, &pri);
            ASSERT(6 == val && 4 == pri);

            ASSERT(0 == mpq.length());

            mpq.enable();

            mpq.pushBack(7, 3);
            mpq.pushBack(9, 4);

            if (enabled) {
                mpq.enable();
            }
            else {
                mpq.disable();
            }

            mpq.pushFrontMultipleRaw(8, 4, 3);

            mpq.popFront(&val, &pri);
            ASSERT(7 == val && 3 == pri);
            mpq.popFront(&val, &pri);
            ASSERT(8 == val && 4 == pri);
            mpq.popFront(&val, &pri);
            ASSERT(8 == val && 4 == pri);
            mpq.popFront(&val, &pri);
            ASSERT(8 == val && 4 == pri);
            mpq.popFront(&val, &pri);
            ASSERT(9 == val && 4 == pri);

            ASSERT(0 == mpq.length());

            mpq.enable();

            mpq.pushBack(17, 7);

            if (enabled) {
                mpq.enable();
            }
            else {
                mpq.disable();
            }

            ASSERT(1 == mpq.length());

            mpq.pushFrontMultipleRaw(8, 7, 0);
            ASSERT(1 == mpq.length());
            mpq.pushFrontMultipleRaw(8, 6, 0);
            ASSERT(1 == mpq.length());
            mpq.pushFrontMultipleRaw(8, 8, 0);
            ASSERT(1 == mpq.length());

            mpq.pushBackMultipleRaw(8, 7, 0);
            ASSERT(1 == mpq.length());
            mpq.pushBackMultipleRaw(8, 6, 0);
            ASSERT(1 == mpq.length());
            mpq.pushBackMultipleRaw(8, 8, 0);
            ASSERT(1 == mpq.length());

            mpq.popFront(&val, &pri);
            ASSERT(17 == val && 7 == pri);

            ASSERT(0 == mpq.length());

            mpq.pushBackMultipleRaw(21, 12, 1);

            mpq.popFront(&val, &pri);
            ASSERT(21 == val && 12 == pri);

            ASSERT(0 == mpq.length());

            mpq.pushFrontMultipleRaw(21, 14, 1);

            mpq.popFront(&val, &pri);
            ASSERT(21 == val && 14 == pri);

            ASSERT(0 == mpq.length());
        } // for i
      }  break;
      case 13: {
        // --------------------------------------------------------------------
        // TEST ENABLE / DISABLE
        //
        // Concerns:
        //   That disable / enable work.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing enable / disable\n"
                             "========================\n";

        ASSERT(0 == ta.numBytesInUse());

        bcec_MultipriorityQueue<int> mpq(&ta);

        ASSERT(!mpq.pushBack(5, 3));

        ASSERT(1 == mpq.length());

        mpq.disable();

        ASSERT(0 != mpq.pushBack(7, 3));

        ASSERT(1 == mpq.length());

        int memUsed = ta.numBytesInUse();

        for (int i = 0; i < 10000; ++i) {
            ASSERT(0 != mpq.pushBack(8, 3));
            ASSERT(1 == mpq.length());
            ASSERT(ta.numBytesInUse() == memUsed);    // no memory leaked
        }

        mpq.enable();

        ASSERT(!mpq.pushBack(15, 3));

        ASSERT(2 == mpq.length());
      }  break;
      case 12: {
        // --------------------------------------------------------------------
        // EXCEPTION SAFETY DURING ALL ALLOCATIONS
        //
        // Concerns:
        //   Is the multipriority queue safe on any exceptions that may occur
        //   on any allocation?
        //
        // Plan:
        //   Standard use of the *_BSLMA_EXCEPTION_TEST macros.
        // --------------------------------------------------------------------

        bcema_TestAllocator testAllocator(veryVeryVeryVerbose);
        bcema_TestAllocator silentTa;   // always silent

        bsl::list<int> numAllocList(&silentTa);

        int start;
        BEGIN_BSLMA_EXCEPTION_TEST
        {
            numAllocList.clear();
            start = testAllocator.numAllocations();

                                                              // numAllocation:
                                                              // --------------
            numAllocList.push_back(testAllocator.numAllocations());    // 0

            Sobj mX(&testAllocator);    const Sobj& X = mX;

            numAllocList.push_back(testAllocator.numAllocations());    // 2

            bsl::string s("woof woof", &silentTa);

            numAllocList.push_back(testAllocator.numAllocations());    // 2

            mX.pushBack(s, 0);

            numAllocList.push_back(testAllocator.numAllocations());    // 4

            bsl::string t(&testAllocator);

            numAllocList.push_back(testAllocator.numAllocations());    // 5

            mX.popFront(&t);

            numAllocList.push_back(testAllocator.numAllocations());    // 6

            if (veryVerbose) {
                cout << "Made it to the end!\n" << flush;
            }
        }
        END_BSLMA_EXCEPTION_TEST

        if (veryVerbose) {
            cout << "AllocLimits: ";

            for (bsl::list<int>::iterator it = numAllocList.begin();
                                              numAllocList.end() != it; ++it) {
                cout << (it != numAllocList.begin() ? ", " : "");
                cout << *it - start;
            }
            cout << endl;
        }
      }  break;
      case 11: {
        // --------------------------------------------------------------------
        // VERIFYING PUSH/POP EXCEPTION SAFETY
        //
        // Concerns:
        //   That the data structure is preserved during exceptions thrown
        //   during pushing and popping.
        //
        // Plan:
        //   Declare a class 'Thrower' which can be programmed to throw on
        //   copy, assign, neither, or both.  Use this class to cause
        //   pushes and pops, respectively, to throw.  Verify that the queue
        //   is unchanged when these throws occur, and that no aborts or
        //   segmentation faults are caused by these throws.  Have an instance
        //   of type 'Element' within the 'Thrower' class for monitoring leaks.
        // --------------------------------------------------------------------

        using namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_11;

        if (verbose) {
            cout << "===================================\n"
                    "Verifying push/pop exception safety\n"
                    "===================================\n";
        }

        enum {
            A_VAL = 38,
            B_VAL = 39,
            C_VAL = 40,
            D_VAL = 41,
            E_VAL = 42
        };

        ASSERT(0 == Element::s_allocCount);

        try {
            typedef bcec_MultipriorityQueue<Thrower> Tq;

            Tq *pMX = new (ta) Tq(1, &ta);
            Tq& mX = *pMX;     const Tq& X = *pMX;

            Thrower thrA(A_VAL, false, false);     // never throws
            Thrower thrB(B_VAL, true,  false);     // throws on copy
            Thrower thrC(C_VAL, false, true);      // throws on assignment
            Thrower thrD(D_VAL, false, false);     // never throws
            Thrower thrE(E_VAL, false, false);     // never throws

            ASSERT(5 == Element::s_allocCount);

            mX.pushBack(thrA, 0);

            ASSERT(0 < ta.numBytesInUse());

            try {
                mX.pushBack(thrB, 0);           // should throw
                ASSERT(0);
            } catch (...) {
                ASSERT(1 == X.length());

                mX.popFront(&thrD);             // should be able to
                                                // successfully pop thrA
                ASSERT(A_VAL == thrD.d_value);
            }

            ASSERT(0 == X.length());
            mX.pushBack(thrC, 0);

            try {
                mX.popFront(&thrE);             // attempting to pop thrC,
                                                // should throw
                ASSERT(0);
            } catch (...) {
                ASSERT(1 == X.length());        // verify queue is unchanged.
                ASSERT(E_VAL == thrE.d_value); // verify thrE is unchanged.

                // can't examine front of queue any more, will always
                // throw on pop.
            }

            ASSERT(1 == X.length());
            ASSERT(6 == Element::s_allocCount);     // verify no leaks
            mX.removeAll();
            ASSERT(5 == Element::s_allocCount);     // verify no leaks

            ta.deleteObjectRaw(pMX);
        } catch (...) {
            ASSERT(0);          // Should never get here, that is, we
                                // should be able to destroy the queue
                                // without throwing.
        }

        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == Element::s_allocCount);     // verify no leaks
      }  break;
      case 10: {
        // --------------------------------------------------------------------
        //  USAGE OF PROPER MEMORY ALLOCATOR RATHER THAN DEFAULT ALLOCATOR
        //
        // Concerns:
        //   That all data members are allocated using the allocator are
        //   passed the allocator passed to the multipriority queue at
        //   construction.
        //   That all non-temporary memory managed by the multipriority
        //   queue uses the allocator passed at construction.
        //
        // Plan:
        //   Create 3 test allocators: 'taDefault', which we will make the
        //   default allocator, 'taString', which we will use to initially
        //   create strings, and 'ta', which we will use to create a
        //   multipriority queue.
        //   Push and pop some strings onto and out of the queue and call
        //   'removeAll()', closely monitoring allocator usage to verify
        //   that no persistent memory is being allocated with the default
        //   allocator.  Verify at the end that no memory is leaked from
        //   'ta'.
        //   Note 'taString' and 'ta' must be different allocators, because
        //   copying strings that share the same allocator does not
        //   necessarily allocate memory -- the two strings might both
        //   point to a single, reference-counted entity.  Hence the need
        //   for 'taString' to ensure that when strings are copied into
        //   and out of the multipriority queue, their memory allocator
        //   is used.
        //
        // Testing:
        //   Memory usage is done with the specified allocator.
        //---------------------------------------------------------------------

        if (verbose) {
            cout << "=====================================\n"
                    "Testing proper memory allocator usage\n"
                    "=====================================\n";
        }

        enum {
            N_PRIORITIES = 8
        };

        bcema_TestAllocator taDefault(veryVeryVeryVerbose);
        bcema_TestAllocator taString(veryVeryVeryVerbose);
        bcema_TestAllocator ta(veryVeryVeryVerbose);

        bslma_DefaultAllocatorGuard guard(&taDefault);

        LOOP_ASSERT(taDefault.numBytesMax(), 0 == taDefault.numBytesMax());

        for (int construct = 0; 2 > construct; ++construct) {
            {
                Sobj *pMX;

                if (0 == construct) {
                    pMX = new(ta) Sobj(N_PRIORITIES, &ta);
                }
                else {
                    pMX = new(ta) Sobj(&ta);
                }
                Sobj& mX = *pMX;   const Sobj& X = *pMX;

                LOOP_ASSERT(taDefault.numBytesInUse(),
                                            0 == taDefault.numBytesInUse());

                const bsl::string woof("woof", &taString);
                const bsl::string meow("meow", &taString);

                const int stringMemoryUse = taString.numBytesInUse();

                for (int i = 0; 10 > i; ++i) {
                    mX.pushBack(woof, 0);
                    mX.pushBack(meow, 0);
                }
                ASSERT(20 == X.length());

                ASSERT(stringMemoryUse == taString.numBytesInUse());
                ASSERT(0 < ta.numBytesInUse());
                LOOP_ASSERT(taDefault.numBytesInUse(),
                                            0 == taDefault.numBytesInUse());

                for (int i = 0; 3 > i; ++i) {
                    bsl::string s(&taString);

                    mX.popFront(&s);
                    ASSERT(woof == s);
                    mX.popFront(&s);
                    ASSERT(meow == s);

                    ASSERT(stringMemoryUse <= taString.numBytesInUse());
                }
                ASSERT(14 == X.length());

                ASSERT(stringMemoryUse == taString.numBytesInUse());
                LOOP_ASSERT(taDefault.numBytesInUse(),
                                            0 == taDefault.numBytesInUse());

                mX.removeAll();
                ASSERT(0 == X.length());

                ASSERT(stringMemoryUse == taString.numBytesInUse());
                LOOP_ASSERT(taDefault.numBytesInUse(),
                                            0 == taDefault.numBytesInUse());

                ta.deleteObjectRaw(pMX);

                ASSERT(stringMemoryUse == taString.numBytesInUse());
                LOOP_ASSERT(taDefault.numBytesInUse(),
                                            0 == taDefault.numBytesInUse());
                LOOP_ASSERT(ta.numBytesInUse(), 0 == ta.numBytesInUse());
            }

            LOOP_ASSERT(taString.numBytesInUse(),
                                                0 == taString.numBytesInUse());
        }  // for construct
      }  break;
      case 9: {
        // --------------------------------------------------------------------
        // STRESS REMOVEALL TEST
        //
        // Concerns:
        //   That there are no race or deadlock conditions, and no leaks,
        //   when calling 'removeAll' while other operations are being
        //   executed.
        //
        // Plan:
        //   We will conduct a stress test, only with threads periodically
        //   calling 'removeAll'.  We will no longer be able to predict as
        //   much about the data, but we can detect deadlocks and by using
        //   the 'Element' type we can detect leaks.
        //
        // Testing:
        //   Stress 'RemoveAll' test
        // --------------------------------------------------------------------

        using namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_9;

        if (verbose) {
            cout << "=====================\n"
                    "Stress removeAll test\n"
                    "=====================\n";
        }

        enum {
            NUM_PRODUCERS          = 10,
            NUM_CONSUMERS          = 4,
            NUM_PRIORITIES         = 8,
            NUM_ITEMS_PER_PRODUCER = 8000,
            GARBAGE_CHAR           = 0x8f,
            REMOVE_MASK            = 0x7ff      // removeAll every 2048 items
        };

        Obj mX(NUM_PRIORITIES, &ta);     const Obj& X = mX;

        bcemt_Barrier producerBarrier(NUM_PRODUCERS + 1);

        ProducerThread::s_numPriorities       =  NUM_PRIORITIES;
        ProducerThread::s_numItemsPerProducer =  NUM_ITEMS_PER_PRODUCER;
        ProducerThread::s_pushVal             =  0;
        ProducerThread::s_barrier             = &producerBarrier;
        ProducerThread::s_queue_p             = &mX;
        ProducerThread::s_removeMask          =  REMOVE_MASK;

        bcemt_Barrier  consumerBarrier(NUM_CONSUMERS + 1);
        OutPair        outPairVec[NUM_PRODUCERS * NUM_ITEMS_PER_PRODUCER];
        bces_AtomicInt outPairVecIdx = 0;

        bsl::memset(outPairVec, GARBAGE_CHAR, sizeof(outPairVec));

        ConsumerThread::s_barrier       = &consumerBarrier;
        ConsumerThread::s_queue_p       = &mX;
        ConsumerThread::s_outPairVec    =  outPairVec;
        ConsumerThread::s_outPairVecIdx = &outPairVecIdx;

        ProducerThread producer;
        ConsumerThread consumer;

        ASSERT(0 == Element::s_allocCount);

        bcemt_ThreadGroup consumerGroup;
        consumerGroup.addThreads(consumer, NUM_CONSUMERS);

        if (veryVerbose) {
            COUT << "Consumers spawned" << ENDL;
        }

        bcemt_ThreadGroup producerGroup;
        producerGroup.addThreads(producer, NUM_PRODUCERS);

        if (veryVerbose) {
            COUT << "Producers spawned" << ENDL;
        }

        consumerBarrier.wait();

        // try to give consumers a change to block on pop, ready to go
        for (int i = 0; NUM_CONSUMERS + NUM_PRODUCERS > i; ++i) {
            bcemt_ThreadUtil::yield();
            bcemt_ThreadUtil::microSleep(10 * 1000);
        }

        testStartedTime = bdetu_SystemTime::now().totalSecondsAsDouble();
        producerBarrier.wait();

        producerGroup.joinAll();

        if (veryVerbose) {
            COUT << "Producers joined after " <<
                bdetu_SystemTime::now().totalSecondsAsDouble() -
                                             testStartedTime << " sec" << ENDL;
        }

        for (int i = 0; NUM_CONSUMERS > i; ++i) {
            mX.pushBack(-1, NUM_PRIORITIES - 1);
        }

        consumerGroup.joinAll();

        if (veryVerbose) {
            COUT << "Consumers joined after " <<
                bdetu_SystemTime::now().totalSecondsAsDouble() -
                                             testStartedTime << " sec" << ENDL;
        }

        LOOP_ASSERT(X.length(), 0 == X.length());
        ASSERT(0 == Element::s_allocCount);

        int priorityCounts[NUM_PRIORITIES];
        bsl::memset(priorityCounts, 0, sizeof priorityCounts);
        for (int i = 0;  outPairVecIdx > i; ++i) {
            ++priorityCounts[outPairVec[i].d_priority];
        }
        if (veryVerbose) {
            cout << "PriorityCounts: ";
            for (int i = 0; NUM_PRIORITIES > i; ++i) {
                cout << (i ? ", " : "") << priorityCounts[i];
            }
            cout << endl;
        }

        OutPairPriorityLess priorityLess;
        bsl::stable_sort(outPairVec, outPairVec + outPairVecIdx, priorityLess);

        int lastPriority = -1;
        int lastValue = -1;
        int outOfOrder = 0;
        for (int i = 0; outPairVecIdx > i; ++i) {
            if (outPairVec[i].d_priority != lastPriority) {
                ASSERT(lastPriority == outPairVec[i].d_priority - 1);
                lastPriority = outPairVec[i].d_priority;
                lastValue = -1;
            }
            else {
                outOfOrder += (outPairVec[i].d_value <= lastValue);
            }
        }
        ASSERT(NUM_PRIORITIES - 1 == lastPriority);
        if (veryVerbose) {
            cout << "Out of order: " <<
                    outOfOrder << " out of " << outPairVecIdx << " = " <<
                                   outOfOrder * 100.0 / outPairVecIdx << "%\n";
        }

        // verify no duplicates
        OutPairValueLess valueLess;
        bsl::sort(outPairVec, outPairVec + outPairVecIdx, valueLess);
        lastValue = -1;
        for (int i = 0; outPairVecIdx > i; ++i) {
            ASSERT(outPairVec[i].d_value > lastValue);
            lastValue = outPairVec[i].d_value;
        }
      }  break;
      case 8: {
        // --------------------------------------------------------------------
        // STRESS MULTITHREADED TEST
        //
        // Concerns:
        //   That there are no race or deadlock conditions when pushing
        //   and popping elements.
        //
        // Plan:
        //   Subject our component to a complex, heavily threaded stress
        //   test case.
        //
        //   Test multipriority queue with many simultaneous threads pushing
        //   and popping with many items, to maximize the possibility of
        //   encountering race conditions or deadlocks.  Configure
        //   with the value of 'veryVeryVerbose' controlling the number of
        //   items that the test will process so the test can be run for a long
        //   time on a non-routine basis.
        //
        //   Several threads simultaneously submit jobs into the multipriority
        //   queue with different priorities.  Several threads simultaneously
        //   pop data off the queue.  The popping threads inherently cannot
        //   exactly predict what the next item they pop will be, but they
        //   can accumulate the data into a data structure which can be
        //   merged in their entirety at the end of the run, which will
        //   be precisely predictable and verifiable.
        //
        //   The items pushed into the queue will be driven by an atomicInt
        //   which increments on every push.  Thus the items pushed will be
        //   in a contiguous range of integers.  The consumer threads will
        //   push items into a vector, with the index they push to driven
        //   by another atomicInt.  In the end we can sort and examine
        //   that vector and verify properties of the data.
        //
        // Testing:
        //   Looking for race conditions, lost data, and surprise deadlocks.
        // --------------------------------------------------------------------

        using namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_8;

        if (verbose) {
            cout << "=========================\n"
                    "Multithreaded stress test\n"
                    "=========================\n";
        }

        enum {
            NUM_PRODUCERS  = 10,
            NUM_CONSUMERS  = 5,
            NUM_PRIORITIES = 8,
            GARBAGE_CHAR   = 0x8f
        };

        Iobj mX(NUM_PRIORITIES, &ta); const Iobj& X = mX;

        int numItemsPerProducer =   4000;
        if (veryVeryVerbose) {
            numItemsPerProducer = 120000;
        }

        bcemt_Barrier producerBarrier(NUM_PRODUCERS + 1);

        ProducerThread::s_numPriorities       =  NUM_PRIORITIES;
        ProducerThread::s_numItemsPerProducer =  numItemsPerProducer;
        ProducerThread::s_pushVal             =  0;
        ProducerThread::s_barrier             = &producerBarrier;
        ProducerThread::s_queue_p             = &mX;

        bcemt_Barrier   consumerBarrier(NUM_CONSUMERS + 1);
        int             outPairVecNumBytes = sizeof(OutPair) * NUM_PRODUCERS
                                                         * numItemsPerProducer;
        OutPair        *outPairVec = (OutPair *)
                                               ta.allocate(outPairVecNumBytes);
        bces_AtomicInt  outPairVecIdx = 0;

        bsl::memset(outPairVec, GARBAGE_CHAR, outPairVecNumBytes);

        ConsumerThread::s_barrier       = &consumerBarrier;
        ConsumerThread::s_queue_p       = &mX;
        ConsumerThread::s_outPairVec    =  outPairVec;
        ConsumerThread::s_outPairVecIdx = &outPairVecIdx;

        ProducerThread producer;
        ConsumerThread consumer;

        bcemt_ThreadGroup consumerGroup;
        consumerGroup.addThreads(consumer, NUM_CONSUMERS);

        if (veryVerbose) {
            COUT << "Consumers spawned" << ENDL;
        }

        bcemt_ThreadGroup producerGroup;
        producerGroup.addThreads(producer, NUM_PRODUCERS);

        if (veryVerbose) {
            COUT << "Producers spawned" << ENDL;
        }

        consumerBarrier.wait();

        // try to give consumers a change to block on pop, ready to go
        for (int i = 0; NUM_CONSUMERS + NUM_PRODUCERS > i; ++i) {
            bcemt_ThreadUtil::yield();
            bcemt_ThreadUtil::microSleep(10 * 1000);
        }

        testStartedTime = bdetu_SystemTime::now().totalSecondsAsDouble();
        producerBarrier.wait();

        producerGroup.joinAll();

        if (veryVerbose) {
            COUT << "Producers joined after " <<
                bdetu_SystemTime::now().totalSecondsAsDouble() -
                                             testStartedTime << " sec" << ENDL;
        }

        for (int i = 0; NUM_CONSUMERS > i; ++i) {
            mX.pushBack(-1, NUM_PRIORITIES - 1);
        }

        consumerGroup.joinAll();

        if (veryVerbose) {
            COUT << "Consumers joined after " <<
                bdetu_SystemTime::now().totalSecondsAsDouble() -
                    testStartedTime << " sec, " << outPairVecIdx << " items" <<
                                                                        ENDL;
        }

        ASSERT(NUM_PRODUCERS * numItemsPerProducer == outPairVecIdx);

        LOOP_ASSERT(X.length(), 0 == X.length());

        if (veryVerbose) {
            double averages[NUM_PRIORITIES];
            bsl::memset(averages, 0, sizeof(averages));
            int counts[NUM_PRIORITIES];
            bsl::memset(counts, 0, sizeof(counts));

            for (int i = 0; outPairVecIdx > i; ++i) {
                averages[outPairVec[i].d_priority] += i;
                ++counts[outPairVec[i].d_priority];
            }
            for (int i = 0; NUM_PRIORITIES > i; ++i) {
                averages[i] /= counts[i];
            }
            cout << "(count, average) for priority i:\n";
            for (int i = 0; NUM_PRIORITIES > i; ++i) {
                cout << (i % 3 ? ", " : (i ? "\n    " : "    "));
                cout << "(" << counts[i] << ", " << averages[i] << ")";
            }
            cout << endl;
        }

        OutPairPriorityLess priorityLess;
        bsl::stable_sort(outPairVec, outPairVec + outPairVecIdx, priorityLess);

        int lastPriority = -1;
        int lastValue = -1;
        int outOfOrder = 0;
        for (int i = 0; outPairVecIdx > i; ++i) {
            if (outPairVec[i].d_priority != lastPriority) {
                LOOP3_ASSERT(lastPriority, i, outPairVec[i].d_priority,
                                 lastPriority == outPairVec[i].d_priority - 1);
                lastPriority = outPairVec[i].d_priority;
                lastValue = -1;
            }
            else {
                outOfOrder += (outPairVec[i].d_value <= lastValue);
            }
        }
        ASSERT(NUM_PRIORITIES - 1 == lastPriority);
        if (veryVerbose) {
            cout << "Out of order: " <<
                    outOfOrder << " out of " << outPairVecIdx << " = " <<
                                outOfOrder * 100.0 / outPairVecIdx << "%\n";
        }

        OutPairValueLess valueLess;
        bsl::sort(outPairVec, outPairVec + outPairVecIdx, valueLess);
        lastValue = -1;
        for (int i = 0; outPairVecIdx > i; ++i) {
            LOOP3_ASSERT(i, outPairVec[i].d_value, lastValue,
                                       outPairVec[i].d_value == 1 + lastValue);
            lastValue = outPairVec[i].d_value;
        }

        ta.deallocate(outPairVec);
      }  break;
      case 7: {
        // --------------------------------------------------------------------
        // 'POPFRONT' AND 'TRYPOPFRONT' BLOCKING BEHAVIOR TEST
        //
        // Concerns:
        //   That 'popFront' blocks properly when the queue is empty.
        //   That 'tryPopFront' never blocks.
        //
        // Plan:
        //   Start a thread, which will attempt to pop data off the queue.
        //   First, the thread will verify that 'tryPopFront' with both
        //   possible argument combinations returns failure without blocking
        //   when called on an empty queue.  Then it will reach a barrier
        //   and, once past the barrier, will attempt to pop data off the
        //   queue, once with each possible arg combination for 'popFront'.
        //   The spawning thread sleeps for about 200 milliSeconds, then
        //   pushes data into the queue, and the spawned thread, once it
        //   pops data, verifies the value is what was expected, showing
        //   that the pops blocked as they should have until the queue
        //   contained data for them.
        //
        // Testing:
        //   blocking behavior
        // --------------------------------------------------------------------

        using namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_7;

        if (verbose) {
            cout << "=================================================\n"
                    "Testing 'popFront' and 'tryPopFront' blocking\n"
                    "=================================================\n";
        }

        Obj mX(&ta);
        bcemt_Barrier barrier(2);

        TestFunctor7 tf7(&mX, &barrier);
        bcemt_ThreadUtil::Handle handle;

        bcemt_ThreadUtil::create(&handle, tf7);

        barrier.wait(); // wait while it tries to pop from the empty queue

        for (int j = 0; 4 > j; ++j) {
            bcemt_ThreadUtil::yield();
            bcemt_ThreadUtil::microSleep(1000 * 50);
        }
        mX.pushBack(tf7.FIRST_PUSHVAL, tf7.PUSH_PRIORITY);

        for (int j = 0; 4 > j; ++j) {
            bcemt_ThreadUtil::yield();
            bcemt_ThreadUtil::microSleep(1000 * 50);
        }
        mX.pushBack(tf7.SECOND_PUSHVAL, tf7.PUSH_PRIORITY);

        bcemt_ThreadUtil::join(handle);
      }  break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING REMOVEALL AND DESTRUCTOR
        //
        // Concerns:
        //   That 'removeAll' and the destructor work both when the the queue
        //   contains items and when it is empty, and never leak items.
        //
        // Plan:
        //   Create multipriority queues, call 'removeAll' while empty,
        //   push items, call 'removeAll' again, destroy when empty, create
        //   again, push items, destroy when items are in queue.  All the
        //   while, monitor queue size both by calling 'length' and 'isEmpty'
        //   and by monitoring the count of 'Element's in existence.
        //
        // Testing:
        //   removeAll()
        //   ~bcec_MultipriorityQueue()
        // --------------------------------------------------------------------

        using namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_5;    // reusing

        if (verbose) {
            cout << "==================================\n"
                    "Testing removeAll and destructor\n"
                    "==================================\n";
        }

        Obj mX(&ta);   const Obj& X = mX;

        const PushPoint vPush[] = { { 11.1, 1 }, { 3.1,  3 }, { 25.1, 5 },
                                    { 15.2, 5 }, { 23.2, 3 }, { 5.3,  5 },
                                    { 12.1, 2 }, { 43.3, 3 }, { 22.2, 2 },
                                    { 13.4, 3 }, { 1.2,  1 }, { 42.3, 2 },
                                    { 10.1, 0 } };
        enum { VPUSH_LEN = sizeof vPush / sizeof *vPush };

        // testing removeAll

        for (int j = 1; VPUSH_LEN >= j; ++j) {
            for (int i = 0; j > i; ++i) {
                mX.pushBack(vPush[i].d_value, vPush[i].d_priority);
            }
            ASSERT(j == X.length());
            ASSERT(j == Element::s_allocCount);

            mX.removeAll();

            ASSERT(0 == X.length());
            ASSERT(0 == Element::s_allocCount);

            if (veryVerbose) {
                cout << "Did removeAll() on queue with " << j << " elements\n";
            }
        }

        // testing destructor

        for (int j = 1; VPUSH_LEN >= j; ++j) {
            {
                Obj mY(&ta);  const Obj& Y = mY;

                for (int i = 0; j > i; ++i) {
                    mY.pushBack(vPush[i].d_value, vPush[i].d_priority);
                }
                ASSERT(j == Y.length());
                ASSERT(j == Element::s_allocCount);
            }

            ASSERT(0 == Element::s_allocCount);

            if (veryVerbose) {
                cout << "Destructed queue with " << j << " elements\n";
            }
        }
      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // POPPED OBJECTS - SORTED BY PRIORITY AND FIFO WITHIN PRIORITY
        //
        // Concerns:
        //   That 'popFront' and 'tryPopFront' pop items according to their
        //   priority, and that they pop items of the same priority in
        //   the same order in which they were pushed.
        //
        // Plan:
        //   Have a vector of distinct value - priority pairs, all values
        //   distinct but some pairs having identical priorities, push them
        //   into the queue, then pop them all out, verifying that the
        //   value - priority pairs coming out are sorted firstly by priority,
        //   and secondly FIFO among pairs having the same priority.
        //
        // Testing:
        //   Proper ordering of popped objects.
        // --------------------------------------------------------------------

        using namespace BCEC_MULTIPRIORITYQUEUE_TEST_CASE_5;

        if (verbose) {
            cout << "========================================\n"
                    "Testing proper sorting of popped objects\n"
                    "========================================\n";
        }

        Dobj mX(&ta);   const Dobj& X = mX;

        const PushPoint vPush[] = { { 1.1, 1 }, { 1.2, 3 }, { 1.3, 5 },
                                    { 2.1, 5 }, { 2.2, 3 }, { 2.3, 5 },
                                    { 3.1, 2 }, { 3.2, 3 }, { 3.3, 2 },
                                    { 4.1, 3 }, { 4.2, 1 }, { 4.3, 2 },
                                    { 5.1, 0 } };
        enum { VPUSH_LEN = sizeof vPush / sizeof *vPush };

        if (veryVerbose) {
            cout << "First, testing popFront\n";
        }

        for (int i = 0; VPUSH_LEN > i; ++i) {
            mX.pushBack(vPush[i].d_value, vPush[i].d_priority);
        }

        // Set up 'expected[]' to contain the same pairs we pushed, only
        // ordered in the order we expect them to be popped.
        PushPoint expected[VPUSH_LEN];
        bsl::memcpy(expected, vPush, sizeof(expected));
        bsl::stable_sort(expected, expected + VPUSH_LEN);
                                                // stable sorts by priority

        int lastPriority = -1;
        for (int i = 0; VPUSH_LEN > i; ++i) {
            double value;
            int priority;

            mX.popFront(&value, &priority);

            ASSERT(value    == expected[i].d_value);
            ASSERT(priority == expected[i].d_priority);

            if (veryVerbose) {
                P_(value);  P_(priority);  P(X.length());
            }

            lastPriority = priority;
        }

        if (veryVerbose) {
            cout << "Next, testing tryPopFront()\n";
        }

        for (int i = 0; VPUSH_LEN > i; ++i) {
            mX.pushBack(vPush[i].d_value, vPush[i].d_priority);
        }

        lastPriority = -1;
        for (int i = 0; VPUSH_LEN > i; ++i) {
            double value;
            int priority;

            ASSERT(0 == mX.tryPopFront(&value, &priority));

            ASSERT(value    == expected[i].d_value);
            ASSERT(priority == expected[i].d_priority);

            if (veryVerbose) {
                P_(value);  P_(priority);  P(X.length());
            }

            lastPriority = priority;
        }
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ALL CONSTRUCTORS AND numPriorities() ACCESSOR
        //
        // Concerns:
        //   That all possible combinations of arg types to constructors
        //   work and produce multipriority queues that function on a basic
        //   level.
        //
        // Plan:
        //   Construct with default and specified number of priorities.
        //   Verify with numPriorities() that the right number of priorities
        //   were created.  In each case, push items with all valid
        //   priorities and pop them out, verifying that proper results
        //   are obtained.  Single threaded test.
        //
        // Testing:
        //   bcec_MultipriorityQueue()
        //   bcec_MultipriorityQueue(numPriorities)
        //   bcec_MultipriorityQueue(alloc)
        //   bcec_MultipriorityQueue(numPriorities, alloc)
        //   numPriorities()
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "=====================================================\n"
                    "Testing all constructors and numPriorities() accessor\n"
                    "=====================================================\n";
        }

        enum {
            GARBAGE_VALUE = -2374
        };

        bslma_DefaultAllocatorGuard guard(&ta);

        for (int construct = 0; true; ++construct) {
            Obj *pX;
            int priorityCount;
            bool exitLoop = false;

            switch (construct) {
              case 0: {
                priorityCount = sizeof(int) * 8;
                pX = new (ta) Obj(&ta);
              }  break;
              case 1: {
                priorityCount = 23;
                pX = new (ta) Obj(priorityCount, &ta);
              }  break;
              case 2: {
                priorityCount = 1;
                pX = new (ta) Obj(priorityCount, &ta);
              }  break;
              case 3: {
                priorityCount = sizeof(int) * 8;
                pX = new (ta) Obj;
              }  break;
              case 4: {
                priorityCount = 23;
                pX = new (ta) Obj(priorityCount);
              }  break;
              case 5: {
                priorityCount = 1;
                pX = new (ta) Obj(priorityCount);
              }  break;
              default: {
                exitLoop = true;
              }  break;
            }  // switch

            if (exitLoop) {
                break;
            }

            Obj& mX = *pX;   const Obj& X = *pX;

            ASSERT(X.numPriorities() == priorityCount);

            {
                Element e;

                ASSERT(0 != mX.tryPopFront(&e));
            }

            // do a little exercise to make sure it's working for all
            // priorities

            static const double vDoub[] = {
                                     1.1,  2.1, 3.1, 4.1, 5.1, 6.1, 7.1, 8.1,
                                     1.2,  2.2, 3.2, 4.2, 5.2, 6.2, 7.2, 8.2,
                                     1.3,  2.3, 3.3, 4.3, 5.3, 6.3, 7.3, 8.3,
                                     1.4,  2.4, 3.4, 4.4, 5.4, 6.4, 7.4, 8.4,
                                     1.5,  2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5 };
            enum { VDOUB_LEN = sizeof vDoub / sizeof *vDoub };

            ASSERT(VDOUB_LEN >= priorityCount);

            for (int i = 0; priorityCount > i; ++i) {
                ASSERT(X.length() == i);
                mX.pushBack(vDoub[i], i);
            }

            if (veryVerbose) {
                cout << "Popped values: ";
            }
            for (int i = 0; priorityCount > i; ++i) {
                Element e    = GARBAGE_VALUE;
                int priority = GARBAGE_VALUE;

                ASSERT(X.length() == priorityCount - i);

                ASSERT(0 == mX.tryPopFront(&e, &priority));
                ASSERT(vDoub[i] == e);
                ASSERT(i == priority);

                if (veryVerbose) {
                    cout << (i % 10 ? ", " : (i ? "\n               " : ""));
                    cout << e;
                }
            }  // for i
            if (veryVerbose) {
                cout << endl;
            }

            ASSERT(X.isEmpty());
            ASSERT(0 == Element::s_allocCount);

            ta.deleteObjectRaw(pX);
        }  // for construct
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // MORE THOROUGH TESTING OF PRIMARY MANIPULATORS
        //
        // Concerns:
        //   That all possible argument combinations passed to 'popFront' and
        //   'tryPopFront' work correctly.
        //
        // Plan:
        //   Have a vector of distinct values to be pushed one by one with a
        //   single priority and pop the values with 'popFront' and
        //   'tryPopFront', testing them all and verifying expected
        //   values are popped along with the appropriate priorities.
        //
        // Testing:
        //   popFront(&item, &priority)
        //   tryPopFront(&item, &priority)
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "=============================================\n"
                    "More thorough testing of primary manipulators\n"
                    "=============================================\n";
        }

        enum {
            SINGLE_PRIORITY = 3,
            GARBAGE_VALUE = -12
        };

        Obj mX(&ta);    const Obj& X = mX;

        {
            Element e;
            ASSERT(0 != mX.tryPopFront(&e));
        }

        const double vDoub[] = { 5.1, 4.2, 3.3, 2.4, 2.4, 2.4, 2.4, 7.1, 8.1 };
        enum { VDOUB_LEN = sizeof vDoub / sizeof *vDoub };

        ASSERT(X.isEmpty());

        if (verbose) {
            cout << "Testing popFront(&e, &pri)\n";
        }

        // first, test popFront(&e, &pri)

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == i);
            mX.pushBack(vDoub[i], SINGLE_PRIORITY);
            ASSERT(!X.isEmpty());
        }

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == VDOUB_LEN - i);
            ASSERT(!X.isEmpty());
            ASSERT(X.length() == Element::s_allocCount);

            Element e = GARBAGE_VALUE;
            int priority = GARBAGE_VALUE;

            mX.popFront(&e, &priority);

            ASSERT(vDoub[i] == e);
            ASSERT(3 == priority);

            if (veryVerbose) {
                COUT << e << ENDL;
            }
        }  // for i

        ASSERT(X.isEmpty());

        if (verbose) {
            cout << "Testing popFront(&e)\n";
        }

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == i);
            mX.pushBack(vDoub[i], SINGLE_PRIORITY);
            ASSERT(!X.isEmpty());
        }

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == VDOUB_LEN - i);
            ASSERT(!X.isEmpty());
            ASSERT(X.length() == Element::s_allocCount);

            Element e = GARBAGE_VALUE;

            mX.popFront(&e);

            ASSERT(vDoub[i] == e);

            if (veryVerbose) {
                COUT << e << ENDL;
            }
        }  // for i

        ASSERT(X.isEmpty());

        if (verbose) {
            cout << "Testing tryPopFront(&e, &pri)\n";
        }

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == i);
            mX.pushBack(vDoub[i], SINGLE_PRIORITY);
            ASSERT(!X.isEmpty());
        }

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == VDOUB_LEN - i);
            ASSERT(!X.isEmpty());
            ASSERT(X.length() == Element::s_allocCount);

            Element e = GARBAGE_VALUE;
            int priority = GARBAGE_VALUE;

            ASSERT(0 == mX.tryPopFront(&e, &priority));
            ASSERT(3 == priority);

            ASSERT(vDoub[i] == e);

            if (veryVerbose) {
                COUT << e << ENDL;
            }
        }  // for i

        ASSERT(X.isEmpty());

        if (verbose) {
            cout << "Testing tryPopFront(&e)\n";
        }

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == i);
            mX.pushBack(vDoub[i], SINGLE_PRIORITY);
            ASSERT(!X.isEmpty());
        }

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == VDOUB_LEN - i);
            ASSERT(!X.isEmpty());
            ASSERT(X.length() == Element::s_allocCount);

            Element e = GARBAGE_VALUE;

            ASSERT(0 == mX.tryPopFront(&e));

            ASSERT(vDoub[i] == e);

            if (veryVerbose) {
                COUT << e << ENDL;
            }
        }  // for i

        ASSERT(X.isEmpty());
        {
            Element e;
            ASSERT(0 != mX.tryPopFront(&e));
        }

        ASSERT(0 == Element::s_allocCount);
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP, SINGLE THREADED)
        //
        // Concerns:
        //   * That 'pushBack' enqueues multiple items of different value
        //     at the same priority.
        //   * That 'pushBack' enqueues multiple items of the same value
        //     at the same priority.
        //   * That repeated calls to 'popFront' deque the items in turn.
        //   * That 'tryPopFront' deques items in turn, returning success
        //     then returns a non-zero value when the queue is empty.
        //
        // Plan:
        //   Have a vector of distinct values to be pushed one by one with a
        //   single priority and pop the values with various incarnations
        //   of 'popFront' and 'tryPopFront', testing them all and verifying
        //   expected values are popped.  Also verify periodically that
        //   'length' and 'isEmpty' are what's expected.  Also use
        //   Element::s_allocCount to verify periodically that the expected
        //   number of Elements are in existence.
        //
        // Testing:
        //   pushBack()
        //   tryPopFront(&item)
        //   popFront(&item)
        //   length()
        //   isEmpty()
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "========================================\n"
                    "Testing Primary Manipulators (Bootstrap)\n"
                    "========================================\n";
        }

        Obj mX(&ta);   const Obj& X = mX;
        Element e;

        ASSERT(1 == e.s_allocCount);

        ASSERT(0 != mX.tryPopFront(&e));

        const double vDoub[] = { 5.1, 4.2, 3.3, 2.4, 2.4, 2.4, 2.4, 7.7, 8.8 };
        enum { VDOUB_LEN = sizeof vDoub / sizeof *vDoub };

        ASSERT(X.isEmpty());

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == i);
            e = vDoub[i];
            mX.pushBack(e, 3);
            ASSERT(!X.isEmpty());
        }

        ASSERT(0 == mX.tryPopFront(&e))
        ASSERT(vDoub[0] == e);
        ASSERT(VDOUB_LEN == e.s_allocCount);

        for (int i = 1; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == VDOUB_LEN - i);
            ASSERT(!X.isEmpty());
            ASSERT(X.length() + 1 == e.s_allocCount);

            mX.popFront(&e);
            ASSERT(vDoub[i] == e);

            if (veryVerbose) {
                COUT << e << ENDL;
            }
        }

        if (veryVerbose) {
            cout << "Done with popFront, Starting tryPopFront\n";
        }

        ASSERT(X.isEmpty());

        // now repeat the test with tryPopFront()

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == i);
            e = vDoub[i];
            mX.pushBack(e, 3);
            ASSERT(!X.isEmpty());
        }

        ASSERT(0 == mX.tryPopFront(&e))
        ASSERT(vDoub[0] == e);
        ASSERT(VDOUB_LEN == e.s_allocCount);

        for (int i = 1; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == VDOUB_LEN - i);
            ASSERT(!X.isEmpty());
            ASSERT(X.length() + 1 == e.s_allocCount);

            ASSERT(0 == mX.tryPopFront(&e));
            ASSERT(vDoub[i] == e);

            if (veryVerbose) {
                COUT << e << ENDL;
            }
        }

        ASSERT(X.isEmpty());
        ASSERT(0 != mX.tryPopFront(&e));

        ASSERT(1 == e.s_allocCount);
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise some basic functionality.
        //
        // Plan:
        //   Create a queue, call tryPopFront on it before putting anything
        //   into it to verify that it does not succeed.  Push a few different
        //   items with a single priority, then pop them out and verify they
        //   came out in the same order in which they were pushed.  Verify
        //   no memory has been leaked.
        // --------------------------------------------------------------------

        if (verbose) cout << "==============\n"
                             "BREATHING TEST\n"
                             "==============\n";

        {
            Dobj mX(5, &ta);
            double x;
            int pri;

            ASSERT(0 != mX.tryPopFront(&x));

            mX.pushBack(1.0, 0);
            mX.pushBack(4.0, 4);

            mX.popFront(&x);
            ASSERT(1.0 == x);
            mX.popFront(&x, &pri);
            ASSERT(4.0 == x);
            ASSERT(4 == pri);

            mX.pushBack(4.0, 4);
            mX.pushBack(1.0, 0);

            ASSERT(0 == mX.tryPopFront(&x));
            ASSERT(1.0 == x);
            ASSERT(0 == mX.tryPopFront(&x, &pri));
            ASSERT(4.0 == x);
            ASSERT(4 == pri);
        }

        ASSERT(0 == ta.numBytesInUse());
      }  break;
      default: {

        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
