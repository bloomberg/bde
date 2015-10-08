// bdlcc_multipriorityqueue.t.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlcc_multipriorityqueue.h>

#include <bslim_testutil.h>

#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_semaphore.h>
#include <bslma_testallocator.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>
#include <bslmt_threadgroup.h>

#include <bsls_atomic.h>
#include <bsls_types.h>

#include <bdlt_currenttime.h>

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
// [ 4] bdlcc::MultipriorityQueue()
// [ 4] bdlcc::MultipriorityQueue(numPriorities)
// [ 4] bdlcc::MultipriorityQueue(alloc)
// [ 4] bdlcc::MultipriorityQueue(numPriorities, alloc)
// [ 6] ~bdlcc::MultipriorityQueue()
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
//                      NON-STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

bslmt::Mutex coutMutex;

#define COUT  { coutMutex.lock(); bsl::cout
#define CERR  { coutMutex.lock(); bsl::cerr
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

// ============================================================================
//                 NON-STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
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

// ============================================================================
//                   NON-SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) COUT << #X " = " << (X) << ENDL // Print identifier and value.
#define Q(X) COUT << "<| " #X " |>" << ENDL  // Quote identifier literally.
#define P_(X) COUT << #X " = " << (X) << ", "<< FLUSH // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  COUT << '\t' << FLUSH           // Print tab w/o newline

// ============================================================================
//            GLOBAL TYPEDEFS, CONSTANTS & VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

// Struct 'Element' is used in many examples, it behaves similarly to a double,
// but it also keeps track of the number instances of it in existence, useful
// for detecting leaks.

struct Element {
    static bsls::AtomicInt s_allocCount;
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
bsls::AtomicInt Element::s_allocCount(0);

typedef bdlcc::MultipriorityQueue<Element>      Obj;
typedef bdlcc::MultipriorityQueue<double>       Dobj;
typedef bdlcc::MultipriorityQueue<int>          Iobj;
typedef bdlcc::MultipriorityQueue<bsl::string>  Sobj;

}  // close unnamed namespace

// ============================================================================
//            TYPES AND FUNCTIONS FOR TEST CASE - USAGE EXAMPLE 1
// ----------------------------------------------------------------------------

namespace MULTIPRIORITYQUEUE_TEST_USAGE_1 {

///Usage
///-----
///Example 1: Simple Thread Pool
///- - - - - - - - - - - - - - -
// This example demonstrates how we might use a 'bdlcc::MultipriorityQueue' to
// communicate between a single "producer" thread and multiple "consumer"
// threads.  The "producer" pushes work requests of varying priority onto the
// queue, and each "consumer" iteratively takes the highest priority work
// request from the queue and services it.
//
// We begin our example with some utility classes that define a simple "work
// item":
//..
    enum {
        k_MAX_CONSUMER_THREADS = 10
    };

    struct MyWorkData {
        int d_i;        // input to work to be done

        // Work data...
    };

    struct MyWorkRequest {
        enum RequestType {
            e_WORK = 1,
            e_STOP = 2
        };

        RequestType d_type;
        MyWorkData  d_data;

        // Work data...
    };
//..
// Next, we provide a simple function to service an individual work item, and a
// function to get a work item.  The details are unimportant for this example:
//..
    void myDoWork(MyWorkData& data)
    {
        // Do work...
        (void)data;
    }

    int getWorkData(MyWorkData *result)
    {
        static int count = 0;
        result->d_i = rand();   // Only one thread runs this routine, so it
                                // does not matter that 'rand()' is not
                                // thread-safe, or that 'count' is 'static'.

        return ++count >= 100;
    }
//..
// The 'myConsumer' function (below) will pop elements off the queue in
// priority order and process them.  As discussed above, note that the call to
// 'queue->popFront(&item)' will block until there is an element available on
// the queue.  This function will be executed in multiple threads, so that each
// thread waits in 'queue->popFront()'; 'bdlcc::MultipriorityQueue' guarantees
// that each thread gets a unique element from the queue:
//..
    void myConsumer(bdlcc::MultipriorityQueue<MyWorkRequest> *queue)
    {
        MyWorkRequest item;
        while (1) {

            // The 'popFront' function will wait for a 'MyWorkRequest' until
            // one is available.

            queue->popFront(&item);

            if (MyWorkRequest::e_STOP == item.d_type) {
                break;
            }

            myDoWork(item.d_data);
        }
    }
//..
// The 'myConsumerThread' function below is a callback for 'bslmt::ThreadUtil',
// which requires a "C" signature.  'bslmt::ThreadUtil::create()' expects a
// pointer to this function, and provides that function pointer to the
// newly-created thread.  The new thread then executes this function.
//
// Since 'bslmt::ThreadUtil::create()' uses the familiar "C" convention of
// passing a 'void' pointer, our function simply casts that pointer to our
// required type ('bdlcc::MultipriorityQueue<MyWorkRequest> *'), and then
// delegates to the queue-specific function 'myConsumer' (above):
//..
    extern "C" void *myConsumerThread(void *queuePtr)
    {
        myConsumer ((bdlcc::MultipriorityQueue<MyWorkRequest>*) queuePtr);
        return queuePtr;
    }
//..
// In this simple example, the 'myProducer' function (below) serves multiple
// roles: it creates the 'bdlcc::MultipriorityQueue', starts the consumer
// threads, and then produces and queues work items.  When work requests are
// exhausted, this function queues one 'e_STOP' item for each consumer thread.
//
// When each consumer thread reads a 'e_STOP', it terminates its
// thread-handling function.  Note that, although the producer cannot control
// which thread pops a particular work item, it can rely on the knowledge that
// each consumer thread will read a single 'e_STOP' item and then terminate.
//
// Finally, the 'myProducer' function "joins" each consumer thread, which
// ensures that the thread itself will terminate correctly (see the
// 'bslmt_threadutil' component-level documentation for details):
//..
    void myProducer()
    {
        enum {
            k_NUM_PRIORITIES = 8,
            k_NUM_THREADS    = 8
        };

        MyWorkRequest item;
        MyWorkData    workData;

        // Create multi-priority queue with specified number of priorities.

        bdlcc::MultipriorityQueue<MyWorkRequest> queue(k_NUM_PRIORITIES);

        // Start the specified number of threads.

        ASSERT(0 < k_NUM_THREADS
            && k_NUM_THREADS <= static_cast<int>(k_MAX_CONSUMER_THREADS));
        bslmt::ThreadUtil::Handle consumerHandles[k_MAX_CONSUMER_THREADS];

        for (int i = 0; i < k_NUM_THREADS; ++i) {
            bslmt::ThreadUtil::create(&consumerHandles[i],
                                     myConsumerThread,
                                     &queue);
        }

        // Load work data into work requests and push them onto the queue with
        // varying priority until all work data has been exhausted.

        int count = 0;                          // used to generate priorities

        while (!getWorkData(&workData)) {       // see declaration (above)
            item.d_type = MyWorkRequest::e_WORK;
            item.d_data = workData;
            queue.pushBack(item, count % k_NUM_PRIORITIES);  // mixed
                                                             // priorities
            ++count;
        }

        // Load as many stop requests as there are active consumer threads.

        for (int i = 0; i < k_NUM_THREADS; ++i) {
            item.d_type = MyWorkRequest::e_STOP;
            queue.pushBack(item, k_NUM_PRIORITIES - 1);  // lowest priority
        }

        // Join all of the consumer threads back with the main thread.

        for (int i = 0; i < k_NUM_THREADS; ++i) {
            bslmt::ThreadUtil::join(consumerHandles[i]);
        }
    }
//..

}  // close namespace MULTIPRIORITYQUEUE_TEST_USAGE_1

// ============================================================================
//            TYPES AND FUNCTIONS FOR TEST CASE - USAGE EXAMPLE 2
// ----------------------------------------------------------------------------

namespace MULTIPRIORITYQUEUE_TEST_USAGE_2 {

///Example 2: Multi-Threaded Observer
///- - - - - - - - - - - - - - - - -
// The previous example shows a simple mechanism for distributing work requests
// over multiple threads.  This approach works well for large tasks that can be
// decomposed into discrete, independent tasks that can benefit from parallel
// execution.  Note also that the various threads are synchronized only at the
// end of execution, when the producer "joins" the various consumer threads.
//
// The simple strategy used in the first example works well for tasks that
// share no state, and are completely independent of one another.  For
// instance, a web server might use a similar strategy to distribute 'http'
// requests across multiple worker threads.
//
// In more complicated examples, it is often necessary or desirable to
// synchronize the separate tasks during execution.  The second example below
// shows a single "Observer" mechanism that receives event notification from
// the various worker threads.
//
// We first create a simple 'MyEvent' data type.  Worker threads will use this
// type to report information about their work.  In our example, we will report
// the "worker Id", the event number, and some arbitrary text.
//
// As with the previous example, class 'MyEvent' also contains an 'EventType',
// an enumeration that indicates whether the worker has completed all work.
// The "Observer" will use this enumerated value to note when a worker thread
// has completed its work:
//..
    enum {
        k_MAX_CONSUMER_THREADS = 10,
        k_MAX_EVENT_TEXT       = 80
    };

    struct MyEvent {
        enum EventType {
            e_IN_PROGRESS   = 1,
            e_TASK_COMPLETE = 2
        };

        EventType d_type;
        int       d_workerId;
        int       d_eventNumber;
        char      d_eventText[k_MAX_EVENT_TEXT];
    };
//..
// As noted in the previous example, 'bslmt::ThreadUtil::create()' spawns a new
// thread, which invokes a simple "C" function taking a 'void' pointer.  In the
// previous example, we simply converted that 'void' pointer into a pointer to
// 'bdlcc::MultipriorityQueue<MyWorkRequest>'.
//
// In this example, however, we want to pass an additional data item.  Each
// worker thread is initialized with a unique integer value ("worker Id"),
// which identifies that thread.  We therefore create a simple 'struct' that
// contains both of these values:
//..
    struct MyWorkerData {
        int                               d_workerId;
        bdlcc::MultipriorityQueue<MyEvent> *d_queue;
    };
//..
// Function 'myWorker' (below) simulates a working thread by enqueuing multiple
// 'MyEvent' events during execution.  In a realistic application, each
// 'MyEvent' structure would likely contain different textual information.  For
// the sake of simplicity, however, our loop uses a constant value for the text
// field.  Note that various priorities are generated to illustrate the
// multi-priority aspect of this particular queue:
//..
    void myWorker(int workerId, bdlcc::MultipriorityQueue<MyEvent> *queue)
    {
        const int N = queue->numPriorities();
        const int NUM_EVENTS = 5;
        int eventNumber;    // used also to generate mixed priorities

        // First push 'NUM_EVENTS' events onto 'queue' with mixed priorities.

        for (eventNumber = 0; eventNumber < NUM_EVENTS; ++eventNumber) {
            MyEvent ev = {
                MyEvent::e_IN_PROGRESS,
                workerId,
                eventNumber,
                "In-Progress Event"         // constant (for simplicity)
            };
            queue->pushBack(ev, eventNumber % N);       // mixed priorities
        }

        // Now push an event to end this task.

        MyEvent ev = {
            MyEvent::e_TASK_COMPLETE,
            workerId,
            eventNumber,
            "Task Complete"
        };
        queue->pushBack(ev, N - 1);                     // lowest priority
    }
//..
// The callback function 'myWorkerThread' (below) invoked by
// 'bslmt::ThreadUtil::create' takes the traditional 'void' pointer.  The
// expected data is the composite structure 'MyWorkerData'.  The callback
// function casts the 'void' pointer to the application-specific data type and
// then uses the referenced object to construct a call to the 'myWorker'
// function:
//..
    extern "C" void *myWorkerThread(void *vWorkerPtr)
    {
        MyWorkerData *workerPtr = (MyWorkerData *)vWorkerPtr;
        myWorker(workerPtr->d_workerId, workerPtr->d_queue);
        return vWorkerPtr;
    }
//..
// For the sake of simplicity, we will implement the Observer behavior (below)
// in the main thread.  The 'void' function 'myObserver' starts multiple
// threads running the 'myWorker' function, reads 'MyEvent' values from the
// queue, and logs all messages in the order of arrival.
//
// As each 'myWorker' thread terminates, it sends a 'e_TASK_COMPLETE' event.
// Upon receiving this event, the 'myObserver' function uses the 'd_workerId'
// to find the relevant thread, and then "joins" that thread.
//
// The 'myObserver' function determines when all tasks have completed simply by
// counting the number of 'e_TASK_COMPLETE' messages received:
//..
    void myObserver()
    {
        const int k_NUM_THREADS    = 10;
        const int k_NUM_PRIORITIES = 4;

        bdlcc::MultipriorityQueue<MyEvent> queue(k_NUM_PRIORITIES);

        ASSERT(0 < k_NUM_THREADS
            && k_NUM_THREADS <= static_cast<int>(k_MAX_CONSUMER_THREADS));
        bslmt::ThreadUtil::Handle workerHandles[k_MAX_CONSUMER_THREADS];

        // Create 'k_NUM_THREADS' threads, each having a unique "worker id".

        MyWorkerData workerData[k_NUM_THREADS];
        for (int i = 0; i < k_NUM_THREADS; ++i) {
            workerData[i].d_queue = &queue;
            workerData[i].d_workerId = i;
            bslmt::ThreadUtil::create(&workerHandles[i],
                                     myWorkerThread,
                                     &workerData[i]);
        }

        // Now print out each of the 'MyEvent' values as the threads complete.
        // This function ends after a total of 'k_NUM_THREADS'
        // 'MyEvent::e_TASK_COMPLETE' events have been printed.

        int nStop = 0;
        while (nStop < k_NUM_THREADS) {
            MyEvent ev;
            queue.popFront(&ev);
            bsl::cout << "[" << ev.d_workerId << "] "
                      << ev.d_eventNumber << ". "
                      << ev.d_eventText << bsl::endl;
            if (MyEvent::e_TASK_COMPLETE == ev.d_type) {
                ++nStop;
                bslmt::ThreadUtil::join(workerHandles[ev.d_workerId]);
            }
        }
    }
//..

}  // close namespace MULTIPRIORITYQUEUE_TEST_USAGE_2

// ============================================================================
//                           TYPE FOR TEST CASE 11
// ----------------------------------------------------------------------------

namespace MULTIPRIORITYQUEUE_TEST_CASE_11 {

// The goal of this test is to make sure that pushes and pops are exception
// safe.  Push copies the object into the queue, pops assign the object from
// the queue.  So what we need is a type that we can program to throw
// exceptions either during push or pop, or in neither case.  We call this type
// 'Thrower'.

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

}  // close namespace MULTIPRIORITYQUEUE_TEST_CASE_11

// ============================================================================
//                                TEST CASE 9
// ----------------------------------------------------------------------------

namespace MULTIPRIORITYQUEUE_TEST_CASE_9 {

double testStartedTime;

struct ProducerThread {
    static int             s_numPriorities;
    static int             s_numItemsPerProducer;
    static bsls::AtomicInt  s_pushVal;
    static bslmt::Barrier  *s_barrier;
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
            double doneTime = bdlt::CurrentTime::now().totalSecondsAsDouble() -
                                                               testStartedTime;
            COUT << "Producer finishing after " << doneTime << " seconds" <<
                                                                        ENDL;
        }

        return 0;
    }
};
int             ProducerThread::s_numPriorities;
int             ProducerThread::s_numItemsPerProducer;
bsls::AtomicInt  ProducerThread::s_pushVal(0);
bslmt::Barrier  *ProducerThread::s_barrier;
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
        k_ILLEGAL_VAL = INT_MAX
    };

    static bslmt::Barrier  *s_barrier;
    static Obj             *s_queue_p;
    static OutPair         *s_outPairVec;
    static bsls::AtomicInt *s_outPairVecIdx;

    int operator()() {
        s_barrier->wait();

        while (true) {
            OutPair outPair = { k_ILLEGAL_VAL, 0 };

            Element e;
            s_queue_p->popFront(&e, &outPair.d_priority);
            outPair.d_value = static_cast<int>(e);

            if (0 > outPair.d_value) {
                break;
            }
            ASSERT(k_ILLEGAL_VAL != outPair.d_value);
            if (k_ILLEGAL_VAL == outPair.d_value) {
                break;
            }

            s_outPairVec[(*s_outPairVecIdx)++] = outPair;
        }

        if (veryVerbose) {
            double doneTime = bdlt::CurrentTime::now().totalSecondsAsDouble() -
                                                               testStartedTime;
            COUT << "Consumer done after " << doneTime << " seconds" << ENDL;
        }

        return 0;
    }
};
bslmt::Barrier  *ConsumerThread::s_barrier;
Obj             *ConsumerThread::s_queue_p;
OutPair         *ConsumerThread::s_outPairVec;
bsls::AtomicInt *ConsumerThread::s_outPairVecIdx;

}  // close namespace MULTIPRIORITYQUEUE_TEST_CASE_9

// ============================================================================
//                                TEST CASE 8
// ----------------------------------------------------------------------------

namespace MULTIPRIORITYQUEUE_TEST_CASE_8 {

double testStartedTime;

struct ProducerThread {
    static int             s_numPriorities;
    static int             s_numItemsPerProducer;
    static bsls::AtomicInt  s_pushVal;
    static bslmt::Barrier  *s_barrier;
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
            double doneTime = bdlt::CurrentTime::now().totalSecondsAsDouble() -
                                                               testStartedTime;
            COUT << "Producer finishing after " << doneTime << " seconds" <<
                                                                        ENDL;
        }

        return 0;
    }
};
int             ProducerThread::s_numPriorities;
int             ProducerThread::s_numItemsPerProducer;
bsls::AtomicInt  ProducerThread::s_pushVal(0);
bslmt::Barrier  *ProducerThread::s_barrier;
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
        k_ILLEGAL_VAL = INT_MAX
    };

    static bslmt::Barrier  *s_barrier;
    static Iobj            *s_queue_p;
    static OutPair         *s_outPairVec;
    static bsls::AtomicInt *s_outPairVecIdx;

    int operator()() {
        s_barrier->wait();

        while (true) {
            OutPair outPair = { k_ILLEGAL_VAL, 0 };

            s_queue_p->popFront(&outPair.d_value, &outPair.d_priority);
            if (0 > outPair.d_value) {
                break;
            }
            ASSERT(k_ILLEGAL_VAL != outPair.d_value);
            if (k_ILLEGAL_VAL == outPair.d_value) {
                break;
            }

            s_outPairVec[(*s_outPairVecIdx)++] = outPair;
        }

        if (veryVerbose) {
            double doneTime = bdlt::CurrentTime::now().totalSecondsAsDouble() -
                                                               testStartedTime;
            COUT << "Consumer done after " << doneTime << " seconds" << ENDL;
        }

        return 0;
    }
};
bslmt::Barrier  *ConsumerThread::s_barrier;
Iobj           *ConsumerThread::s_queue_p;
OutPair        *ConsumerThread::s_outPairVec;
bsls::AtomicInt *ConsumerThread::s_outPairVecIdx;

}  // close namespace MULTIPRIORITYQUEUE_TEST_CASE_8

// ============================================================================
//                                TEST CASE 7
// ----------------------------------------------------------------------------

namespace MULTIPRIORITYQUEUE_TEST_CASE_7 {

struct TestFunctor7 {
    enum {
        k_GARBAGE_VALUE = -178,
        k_PUSH_PRIORITY  = 4,
        k_FIRST_PUSHVAL  = 1,
        k_SECOND_PUSHVAL = 2
    };

    Obj *d_pMX;
    bslmt::Barrier *d_barrier;
    Element d_e;
    int     d_priority;

    void setGarbage() {
        d_e        = k_GARBAGE_VALUE;
        d_priority = k_GARBAGE_VALUE;
    }

    TestFunctor7(Obj *pMX, bslmt::Barrier *barrier)
    : d_pMX(pMX)
    , d_barrier(barrier)
    {
        setGarbage();
    }

    void operator()() {
        // the queue is empty, verify tryPopFront does not block
        setGarbage();
        ASSERT(0 != d_pMX->tryPopFront(&d_e));
        ASSERT(k_GARBAGE_VALUE == d_e);
        ASSERT(k_GARBAGE_VALUE == d_priority);

        setGarbage();
        ASSERT(0 != d_pMX->tryPopFront(&d_e, &d_priority));
        ASSERT(k_GARBAGE_VALUE == d_e);
        ASSERT(k_GARBAGE_VALUE == d_priority);

        d_barrier->wait();
        // the main thread now can start pushing stuff, sleeping and yielding
        // between pushes

        setGarbage();
        d_pMX->popFront(&d_e);
        ASSERT(k_FIRST_PUSHVAL == d_e);

        setGarbage();
        d_pMX->popFront(&d_e, &d_priority);
        ASSERT(k_SECOND_PUSHVAL == d_e);
        ASSERT(k_PUSH_PRIORITY == d_priority);
    }
};

}  // close namespace MULTIPRIORITYQUEUE_TEST_CASE_7

// ============================================================================
//                                TEST CASE 5
// ----------------------------------------------------------------------------

namespace MULTIPRIORITYQUEUE_TEST_CASE_5 {

struct PushPoint {
    double d_value;
    int    d_priority;
    bool operator<(const PushPoint& rhs) const {
        // for sorting by priority
        return this->d_priority < rhs.d_priority;
    }
};

}  // close namespace MULTIPRIORITYQUEUE_TEST_CASE_5

// ============================================================================
//                               MAIN PROGRAM
// ============================================================================

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator taDefault(veryVeryVeryVerbose);
    bslma::TestAllocator ta(veryVeryVeryVerbose);  // passed to
                                                  // multipriority queue

    bslma::DefaultAllocatorGuard guard(&taDefault);

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
        //   objects with a 'bslma::TestAllocator' object where applicable.
        //   Additionally, replace all calls to 'assert' in the usage example
        //   with calls to 'ASSERT'.  This now becomes the source, which is
        //   then "copied" back to the header file by reversing the above
        //   process.
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        using namespace MULTIPRIORITYQUEUE_TEST_USAGE_1;

        myProducer();
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
        //   objects with a 'bslma::TestAllocator' object where applicable.
        //   Additionally, replace all calls to 'assert' in the usage example
        //   with calls to 'ASSERT'.  This now becomes the source, which is
        //   then "copied" back to the header file by reversing the above
        //   process.
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        using namespace MULTIPRIORITYQUEUE_TEST_USAGE_2;

        myObserver();
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

            bdlcc::MultipriorityQueue<int> mpq(&ta);

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

        bdlcc::MultipriorityQueue<int> mpq(&ta);

        ASSERT(!mpq.pushBack(5, 3));

        ASSERT(1 == mpq.length());

        mpq.disable();

        ASSERT(0 != mpq.pushBack(7, 3));

        ASSERT(1 == mpq.length());

        bsls::Types::Int64 memUsed = ta.numBytesInUse();

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
        //   Standard use of the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros.
        // --------------------------------------------------------------------

        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator silentTa;   // always silent

        bsl::list<bsls::Types::Int64> numAllocList(&silentTa);

        bsls::Types::Int64 start;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
        {
            numAllocList.clear();
            start = testAllocator.numAllocations();

                                                             // numAllocations:
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
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (veryVerbose) {
            cout << "AllocLimits: ";

            for (bsl::list<bsls::Types::Int64>::iterator it
                                                        = numAllocList.begin();
                 numAllocList.end() != it;
                 ++it) {
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

        using namespace MULTIPRIORITYQUEUE_TEST_CASE_11;

        if (verbose) {
            cout << "===================================\n"
                    "Verifying push/pop exception safety\n"
                    "===================================\n";
        }

        enum {
            k_A_VAL = 38,
            k_B_VAL = 39,
            k_C_VAL = 40,
            k_D_VAL = 41,
            k_E_VAL = 42
        };

        ASSERT(0 == Element::s_allocCount);

        try {
            typedef bdlcc::MultipriorityQueue<Thrower> Tq;

            Tq *pMX = new (ta) Tq(1, &ta);
            Tq& mX = *pMX;     const Tq& X = *pMX;

            Thrower thrA(k_A_VAL, false, false);     // never throws
            Thrower thrB(k_B_VAL, true,  false);     // throws on copy
            Thrower thrC(k_C_VAL, false, true);      // throws on assignment
            Thrower thrD(k_D_VAL, false, false);     // never throws
            Thrower thrE(k_E_VAL, false, false);     // never throws

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
                ASSERT(k_A_VAL == thrD.d_value);
            }

            ASSERT(0 == X.length());
            mX.pushBack(thrC, 0);

            try {
                mX.popFront(&thrE);             // attempting to pop thrC,
                                                // should throw
                ASSERT(0);
            } catch (...) {
                ASSERT(1 == X.length());          // verify queue is unchanged
                ASSERT(k_E_VAL == thrE.d_value);  // verify thrE is unchanged

                // can't examine front of queue any more, will always throw on
                // pop.
            }

            ASSERT(1 == X.length());
            ASSERT(6 == Element::s_allocCount);     // verify no leaks
            mX.removeAll();
            ASSERT(5 == Element::s_allocCount);     // verify no leaks

            ta.deleteObjectRaw(pMX);
        } catch (...) {
            ASSERT(0);          // Should never get here, that is, we should be
                                // able to destroy the queue without throwing.
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
            k_N_PRIORITIES = 8
        };

        bslma::TestAllocator taDefault(veryVeryVeryVerbose);
        bslma::TestAllocator taString(veryVeryVeryVerbose);
        bslma::TestAllocator ta(veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard guard(&taDefault);

        LOOP_ASSERT(taDefault.numBytesMax(), 0 == taDefault.numBytesMax());

        for (int construct = 0; 2 > construct; ++construct) {
            {
                Sobj *pMX;

                if (0 == construct) {
                    pMX = new(ta) Sobj(k_N_PRIORITIES, &ta);
                }
                else {
                    pMX = new(ta) Sobj(&ta);
                }
                Sobj& mX = *pMX;   const Sobj& X = *pMX;

                LOOP_ASSERT(taDefault.numBytesInUse(),
                            0 == taDefault.numBytesInUse());

                const bsl::string woof("woof", &taString);
                const bsl::string meow("meow", &taString);

                const bsls::Types::Int64 stringMemoryUse =
                                                      taString.numBytesInUse();

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

        using namespace MULTIPRIORITYQUEUE_TEST_CASE_9;

        if (verbose) {
            cout << "=====================\n"
                    "Stress removeAll test\n"
                    "=====================\n";
        }

        enum {
            k_NUM_PRODUCERS          = 10,
            k_NUM_CONSUMERS          = 4,
            k_NUM_PRIORITIES         = 8,
            k_NUM_ITEMS_PER_PRODUCER = 8000,
            k_GARBAGE_CHAR           = 0x8f,
            k_REMOVE_MASK            = 0x7ff      // removeAll every 2048 items
        };

        Obj mX(k_NUM_PRIORITIES, &ta);     const Obj& X = mX;

        bslmt::Barrier producerBarrier(k_NUM_PRODUCERS + 1);

        ProducerThread::s_numPriorities       =  k_NUM_PRIORITIES;
        ProducerThread::s_numItemsPerProducer =  k_NUM_ITEMS_PER_PRODUCER;
        ProducerThread::s_pushVal             =  0;
        ProducerThread::s_barrier             = &producerBarrier;
        ProducerThread::s_queue_p             = &mX;
        ProducerThread::s_removeMask          =  k_REMOVE_MASK;

        bslmt::Barrier  consumerBarrier(k_NUM_CONSUMERS + 1);
        bsls::AtomicInt outPairVecIdx(0);

        OutPair *outPairVec =
                       new OutPair[k_NUM_PRODUCERS * k_NUM_ITEMS_PER_PRODUCER];

        bsl::memset(
                 outPairVec,
                 k_GARBAGE_CHAR,
                 sizeof(OutPair) * k_NUM_PRODUCERS * k_NUM_ITEMS_PER_PRODUCER);

        ConsumerThread::s_barrier       = &consumerBarrier;
        ConsumerThread::s_queue_p       = &mX;
        ConsumerThread::s_outPairVec    =  outPairVec;
        ConsumerThread::s_outPairVecIdx = &outPairVecIdx;

        ProducerThread producer;
        ConsumerThread consumer;

        ASSERT(0 == Element::s_allocCount);

        bslmt::ThreadGroup consumerGroup;
        consumerGroup.addThreads(consumer, k_NUM_CONSUMERS);

        if (veryVerbose) {
            COUT << "Consumers spawned" << ENDL;
        }

        bslmt::ThreadGroup producerGroup;
        producerGroup.addThreads(producer, k_NUM_PRODUCERS);

        if (veryVerbose) {
            COUT << "Producers spawned" << ENDL;
        }

        consumerBarrier.wait();

        // try to give consumers a change to block on pop, ready to go
        for (int i = 0; k_NUM_CONSUMERS + k_NUM_PRODUCERS > i; ++i) {
            bslmt::ThreadUtil::yield();
            bslmt::ThreadUtil::microSleep(10 * 1000);
        }

        testStartedTime = bdlt::CurrentTime::now().totalSecondsAsDouble();
        producerBarrier.wait();

        producerGroup.joinAll();

        if (veryVerbose) {
            COUT << "Producers joined after " <<
                bdlt::CurrentTime::now().totalSecondsAsDouble() -
                                             testStartedTime << " sec" << ENDL;
        }

        for (int i = 0; k_NUM_CONSUMERS > i; ++i) {
            mX.pushBack(-1, k_NUM_PRIORITIES - 1);
        }

        consumerGroup.joinAll();

        if (veryVerbose) {
            COUT << "Consumers joined after " <<
                bdlt::CurrentTime::now().totalSecondsAsDouble() -
                                             testStartedTime << " sec" << ENDL;
        }

        LOOP_ASSERT(X.length(), 0 == X.length());
        ASSERT(0 == Element::s_allocCount);

        int priorityCounts[k_NUM_PRIORITIES];
        bsl::memset(priorityCounts, 0, sizeof priorityCounts);
        for (int i = 0;  outPairVecIdx > i; ++i) {
            ++priorityCounts[outPairVec[i].d_priority];
        }
        if (veryVerbose) {
            cout << "PriorityCounts: ";
            for (int i = 0; k_NUM_PRIORITIES > i; ++i) {
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
        ASSERT(k_NUM_PRIORITIES - 1 == lastPriority);
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

        delete [] outPairVec;
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

        using namespace MULTIPRIORITYQUEUE_TEST_CASE_8;

        if (verbose) {
            cout << "=========================\n"
                    "Multithreaded stress test\n"
                    "=========================\n";
        }

        enum {
            k_NUM_PRODUCERS  = 10,
            k_NUM_CONSUMERS  = 5,
            k_NUM_PRIORITIES = 8,
            k_GARBAGE_CHAR   = 0x8f
        };

        Iobj mX(k_NUM_PRIORITIES, &ta); const Iobj& X = mX;

        int numItemsPerProducer =   4000;
        if (veryVeryVerbose) {
            numItemsPerProducer = 120000;
        }

        bslmt::Barrier producerBarrier(k_NUM_PRODUCERS + 1);

        ProducerThread::s_numPriorities       =  k_NUM_PRIORITIES;
        ProducerThread::s_numItemsPerProducer =  numItemsPerProducer;
        ProducerThread::s_pushVal             =  0;
        ProducerThread::s_barrier             = &producerBarrier;
        ProducerThread::s_queue_p             = &mX;

        bslmt::Barrier   consumerBarrier(k_NUM_CONSUMERS + 1);
        bsl::size_t      outPairVecNumBytes = sizeof(OutPair) * k_NUM_PRODUCERS
                                                         * numItemsPerProducer;
        OutPair        *outPairVec = (OutPair *)
                                               ta.allocate(outPairVecNumBytes);
        bsls::AtomicInt  outPairVecIdx(0);

        bsl::memset(outPairVec, k_GARBAGE_CHAR, outPairVecNumBytes);

        ConsumerThread::s_barrier       = &consumerBarrier;
        ConsumerThread::s_queue_p       = &mX;
        ConsumerThread::s_outPairVec    =  outPairVec;
        ConsumerThread::s_outPairVecIdx = &outPairVecIdx;

        ProducerThread producer;
        ConsumerThread consumer;

        bslmt::ThreadGroup consumerGroup;
        consumerGroup.addThreads(consumer, k_NUM_CONSUMERS);

        if (veryVerbose) {
            COUT << "Consumers spawned" << ENDL;
        }

        bslmt::ThreadGroup producerGroup;
        producerGroup.addThreads(producer, k_NUM_PRODUCERS);

        if (veryVerbose) {
            COUT << "Producers spawned" << ENDL;
        }

        consumerBarrier.wait();

        // try to give consumers a change to block on pop, ready to go
        for (int i = 0; k_NUM_CONSUMERS + k_NUM_PRODUCERS > i; ++i) {
            bslmt::ThreadUtil::yield();
            bslmt::ThreadUtil::microSleep(10 * 1000);
        }

        testStartedTime = bdlt::CurrentTime::now().totalSecondsAsDouble();
        producerBarrier.wait();

        producerGroup.joinAll();

        if (veryVerbose) {
            COUT << "Producers joined after " <<
                bdlt::CurrentTime::now().totalSecondsAsDouble() -
                                             testStartedTime << " sec" << ENDL;
        }

        for (int i = 0; k_NUM_CONSUMERS > i; ++i) {
            mX.pushBack(-1, k_NUM_PRIORITIES - 1);
        }

        consumerGroup.joinAll();

        if (veryVerbose) {
            COUT << "Consumers joined after " <<
                bdlt::CurrentTime::now().totalSecondsAsDouble() -
                    testStartedTime << " sec, " << outPairVecIdx << " items" <<
                                                                        ENDL;
        }

        ASSERT(k_NUM_PRODUCERS * numItemsPerProducer == outPairVecIdx);

        LOOP_ASSERT(X.length(), 0 == X.length());

        if (veryVerbose) {
            double averages[k_NUM_PRIORITIES];
            bsl::memset(averages, 0, sizeof(averages));
            int counts[k_NUM_PRIORITIES];
            bsl::memset(counts, 0, sizeof(counts));

            for (int i = 0; outPairVecIdx > i; ++i) {
                averages[outPairVec[i].d_priority] += i;
                ++counts[outPairVec[i].d_priority];
            }
            for (int i = 0; k_NUM_PRIORITIES > i; ++i) {
                averages[i] /= counts[i];
            }
            cout << "(count, average) for priority i:\n";
            for (int i = 0; k_NUM_PRIORITIES > i; ++i) {
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
        ASSERT(k_NUM_PRIORITIES - 1 == lastPriority);
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
        //   queue, once with each possible argument combination for
        //   'popFront'.  The spawning thread sleeps for about 200
        //   milliSeconds, then pushes data into the queue, and the spawned
        //   thread, once it pops data, verifies the value is what was
        //   expected, showing that the pops blocked as they should have until
        //   the queue contained data for them.
        //
        // Testing:
        //   blocking behavior
        // --------------------------------------------------------------------

        using namespace MULTIPRIORITYQUEUE_TEST_CASE_7;

        if (verbose) {
            cout << "=================================================\n"
                    "Testing 'popFront' and 'tryPopFront' blocking\n"
                    "=================================================\n";
        }

        Obj mX(&ta);
        bslmt::Barrier barrier(2);

        TestFunctor7 tf7(&mX, &barrier);
        bslmt::ThreadUtil::Handle handle;

        bslmt::ThreadUtil::create(&handle, tf7);

        barrier.wait(); // wait while it tries to pop from the empty queue

        for (int j = 0; 4 > j; ++j) {
            bslmt::ThreadUtil::yield();
            bslmt::ThreadUtil::microSleep(1000 * 50);
        }
        mX.pushBack(tf7.k_FIRST_PUSHVAL, tf7.k_PUSH_PRIORITY);

        for (int j = 0; 4 > j; ++j) {
            bslmt::ThreadUtil::yield();
            bslmt::ThreadUtil::microSleep(1000 * 50);
        }
        mX.pushBack(tf7.k_SECOND_PUSHVAL, tf7.k_PUSH_PRIORITY);

        bslmt::ThreadUtil::join(handle);
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
        //   ~bdlcc::MultipriorityQueue()
        // --------------------------------------------------------------------

        using namespace MULTIPRIORITYQUEUE_TEST_CASE_5;    // reusing

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
        int VPUSH_LEN = static_cast<int>(sizeof vPush / sizeof *vPush);

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

        using namespace MULTIPRIORITYQUEUE_TEST_CASE_5;

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
        const int VPUSH_LEN = static_cast<int>(sizeof vPush / sizeof *vPush);

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

        for (int i = 0; VPUSH_LEN > i; ++i) {
            double value;
            int priority;

            mX.popFront(&value, &priority);

            ASSERT(value    == expected[i].d_value);
            ASSERT(priority == expected[i].d_priority);

            if (veryVerbose) {
                P_(value);  P_(priority);  P(X.length());
            }
        }

        if (veryVerbose) {
            cout << "Next, testing tryPopFront()\n";
        }

        for (int i = 0; VPUSH_LEN > i; ++i) {
            mX.pushBack(vPush[i].d_value, vPush[i].d_priority);
        }

        for (int i = 0; VPUSH_LEN > i; ++i) {
            double value;
            int priority;

            ASSERT(0 == mX.tryPopFront(&value, &priority));

            ASSERT(value    == expected[i].d_value);
            ASSERT(priority == expected[i].d_priority);

            if (veryVerbose) {
                P_(value);  P_(priority);  P(X.length());
            }
        }
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ALL CONSTRUCTORS AND numPriorities() ACCESSOR
        //
        // Concerns:
        //   That all possible combinations of argument types to constructors
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
        //   bdlcc::MultipriorityQueue()
        //   bdlcc::MultipriorityQueue(numPriorities)
        //   bdlcc::MultipriorityQueue(alloc)
        //   bdlcc::MultipriorityQueue(numPriorities, alloc)
        //   numPriorities()
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "=====================================================\n"
                    "Testing all constructors and numPriorities() accessor\n"
                    "=====================================================\n";
        }

        enum {
            k_GARBAGE_VALUE = -2374
        };

        bslma::DefaultAllocatorGuard guard(&ta);

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
            int VDOUB_LEN = static_cast<int>(sizeof vDoub / sizeof *vDoub);

            ASSERT(VDOUB_LEN >= priorityCount);

            for (int i = 0; priorityCount > i; ++i) {
                ASSERT(X.length() == i);
                mX.pushBack(vDoub[i], i);
            }

            if (veryVerbose) {
                cout << "Popped values: ";
            }
            for (int i = 0; priorityCount > i; ++i) {
                Element e    = k_GARBAGE_VALUE;
                int priority = k_GARBAGE_VALUE;

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
            k_SINGLE_PRIORITY =   3,
            k_GARBAGE_VALUE   = -12
        };

        Obj mX(&ta);    const Obj& X = mX;

        {
            Element e;
            ASSERT(0 != mX.tryPopFront(&e));
        }

        const double vDoub[] = { 5.1, 4.2, 3.3, 2.4, 2.4, 2.4, 2.4, 7.1, 8.1 };
        int VDOUB_LEN = static_cast<int>(sizeof vDoub / sizeof *vDoub);

        ASSERT(X.isEmpty());

        if (verbose) {
            cout << "Testing popFront(&e, &pri)\n";
        }

        // first, test popFront(&e, &pri)

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == i);
            mX.pushBack(vDoub[i], k_SINGLE_PRIORITY);
            ASSERT(!X.isEmpty());
        }

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == VDOUB_LEN - i);
            ASSERT(!X.isEmpty());
            ASSERT(X.length() == Element::s_allocCount);

            Element e = k_GARBAGE_VALUE;
            int priority = k_GARBAGE_VALUE;

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
            mX.pushBack(vDoub[i], k_SINGLE_PRIORITY);
            ASSERT(!X.isEmpty());
        }

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == VDOUB_LEN - i);
            ASSERT(!X.isEmpty());
            ASSERT(X.length() == Element::s_allocCount);

            Element e = k_GARBAGE_VALUE;

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
            mX.pushBack(vDoub[i], k_SINGLE_PRIORITY);
            ASSERT(!X.isEmpty());
        }

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == VDOUB_LEN - i);
            ASSERT(!X.isEmpty());
            ASSERT(X.length() == Element::s_allocCount);

            Element e = k_GARBAGE_VALUE;
            int priority = k_GARBAGE_VALUE;

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
            mX.pushBack(vDoub[i], k_SINGLE_PRIORITY);
            ASSERT(!X.isEmpty());
        }

        for (int i = 0; VDOUB_LEN > i; ++i) {
            ASSERT(X.length() == VDOUB_LEN - i);
            ASSERT(!X.isEmpty());
            ASSERT(X.length() == Element::s_allocCount);

            Element e = k_GARBAGE_VALUE;

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
        //   * That repeated calls to 'popFront' dequeue the items in turn.
        //   * That 'tryPopFront' dequeues items in turn, returning success
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
        int VDOUB_LEN = static_cast<int>(sizeof vDoub / sizeof *vDoub);

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
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
