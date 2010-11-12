// bcec_queue.t.cpp                                                   -*-C++-*-

#include <bcec_queue.h>

#include <bcema_testallocator.h>
#include <bcema_sharedptr.h>
#include <bcemt_barrier.h>
#include <bcemt_lockguard.h>
#include <bcemt_semaphore.h>
#include <bcemt_thread.h>
#include <bcemt_threadgroup.h>
#include <bces_atomictypes.h>

#include <bslma_defaultallocatorguard.h>
#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdetu_systemtime.h>

#include <bsl_algorithm.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides a thread-enabled proxy to the 'bdec_Queue'
// component.  The purpose of this test driver is to assert that each operation
// is properly "hooked up" to its respective 'bdec_Queue' operation, and that
// the locking mechanisms work as expected when the boundary conditions on
// length and high water mark are reached.  In addition, although all the
// memory allocated is allocated by the underlying 'bdec_Queue', we want to
// make sure that the allocator is correctly passed to it.  The component is
// tested in a single thread by the breathing test.  In the rest of the test
// cases, we use multiple threads and test the locking and concurrency
// mechanisms:
//
// [2] makes sure that push and pop functions execute properly when queue is
// modified in another thread,
//
// [3] makes sure that timed push and pop functions execute properly when queue
// is modified in another thread,
//
// [4] makes sure that the push functions block properly when high-water mark
// is reached, but are non-blocking otherwise, and that 'forcePushFront' never
// blocks,
//
// [5] makes sure that the timed push functions time out properly when high
// water mark is reached, but are non-blocking otherwise, and
//
// [6] makes sure that the condition variables work properly when modifying the
// queues directly in several threads.
//
// [10] multithreaded test of timedPopFront, timedPopBack.

//-----------------------------------------------------------------------------
// CREATORS
// [ 1] bcec_Queue(bslma_Allocator *basicAllocator = 0);
// [ 1] bcec_Queue(int highWaterMark, bslma_Allocator *basicAllocator = 0);
// [ 1] ~bcec_Queue();
//
// MANIPULATORS
// [ 2] T popBack();
// [ 2] T popFront();
// [ 4] void pushBack(const T& item);
// [ 4] void pushFront(const T& item);
// [ 3] int timedPopBack(TYPE *, const bdet_TimeInterval &);
// [10] int timedPopBack(TYPE *, const bdet_TimeInterval&);
// [ 3] int timedPopFront(TYPE *, const bdet_TimeInterval &);
// [10] int timedPopFront(TYPE *, const bdet_TimeInterval&);
// [ 5] int timedPushBack(const T& item, const bdet_TimeInterval &timeout);
// [ 5] int timedPushFront(const T& item,  const bdet_TimeInterval &timeout);
// [ 4] void forcePushFront(const T& item);
// [ 7] void removeAll(bsl::vector<T>& buffer);
// [ 9] int tryPopFront(TYPE *);
// [ 9] void tryPopFront(int, vector<TYPE> *);
// [ 9] int tryPopBack(TYPE *);
// [ 9] void tryPopBack(int, vector<TYPE> *);

// ACCESSORS
// [ 4] int highWaterMark() const;
// [ 6] bcemt_Mutex& mutex();
// [ 6] bcemt_Condition& notEmptyCondition();
// [ 6] bcemt_Condition& notFullCondition();
// [ 6] bcemt_Condition& condition();
// [ 6] bcemt_Condition& insertCondition();
// [ 6] bdec_Queue<T>& queue();
// [10] blocking on queue empty
// [11] blocking on queue full
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] Usage example 2
// [ 9] Usage example 1
// [10] Use of the 'bdec_Queue' interface example
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
namespace {

volatile int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
   if (c) {
       cout << "Error " << __FILE__ << "(" << i << "): " << s
            << "    (failed)" << endl;
       if (0 <= testStatus && testStatus <= 100) ++testStatus;
   }
}

}  // close unnamed namespace

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                      STANDARD BDE LOOP-ASSERT TEST MACRO
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
        if (!(X)) { cout << #I << ":" << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
        if (!(X)) { cout << #I << ":" << I << "\n" << #J << ":" << J << "\n"; \
                    aSsErT(1, #X, __LINE__);}}

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define TAB cout << '\t';

#define PP(X) (cout << #X " = " << (X) << endl, false) // Print identifier and
                                         // value, return false, as expression.

//=============================================================================
//                                    GLOBALS
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;
int veryVeryVerbose;
int veryVeryVeryVerbose;

//=============================================================================
//                GLOBAL TYPEDEFS/CONSTANTS/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
typedef double Element;
typedef bcec_Queue<Element> Obj;

static const double DECI_SEC      = 0.1;
                                    // 1 deci second (i.e., 1/10th of a second)

static const int MICRO_SEC_IN_SEC = 100000;
                                         // number of micro seconds in a second

//=============================================================================
//                  SUPPORT CLASSES AND FUNCTIONS USED FOR TESTING
//-----------------------------------------------------------------------------

class MyBarrier {
   // This class is a fast 2-thread barrier implemented with semaphores.  Since
   // it can only coordinate two threads, it can use much simpler primitives
   // than the real Barrier component.

   bcemt_Semaphore d_entryGate;
   bcemt_Semaphore d_exitGate;
   bces_AtomicInt  d_threadWaiting;

public:

   MyBarrier() : d_threadWaiting(0) {}

   void wait() {
      if (0 == d_threadWaiting.testAndSwap(0, 1)) {
         d_entryGate.wait();
         d_threadWaiting = 0;
         d_exitGate.post();
      }
      else {
         d_entryGate.post();
         d_exitGate.wait();
      }
   }
};

//=============================================================================
//          USAGE use of the 'bdec_Queue' interface from header
//-----------------------------------------------------------------------------
namespace BCEC_QUEUE_USE_OF_BDEC_QUEUE_INTERFACE {

struct myData
{
    // myData...
};

bcec_Queue<myData>  myWorkQueue;
bdec_Queue<myData>& rawQueue = myWorkQueue.queue();
bcemt_Mutex&        queueMutex = myWorkQueue.mutex();

myData  data1;
myData  data2;
bool pairFoundFlag = false;

void myWork()
// Take two elements from the queue atomically
{
    queueMutex.lock();
    if (2 <= rawQueue.length()) {
        data1 = rawQueue.front();
        rawQueue.popFront();
        data2 = rawQueue.front();
        rawQueue.popFront();
        pairFoundFlag = true;
    }
    queueMutex.unlock();

    if (pairFoundFlag) {
        // process the pair
    }
}

} // namespace BCEC_QUEUE_USE_OF_BDEC_QUEUE_INTERFACE
//=============================================================================
//          USAGE example 1 from header (with assert replaced with ASSERT)
//-----------------------------------------------------------------------------
namespace BCEC_QUEUE_USAGE_EXAMPLE_1 {

struct my_WorkData;

int getWorkData(my_WorkData *)
    // Dummy implementation of 'getWorkData' function required by the
    // usage example.
{
    static int i = 1;
    return !(++i%100);
}

enum {
    MAX_CONSUMER_THREADS = 10
};

struct my_WorkData{
    // Work data...
};

struct my_WorkRequest {
    enum RequestType {
        WORK = 1
      , STOP = 2
    };

    RequestType d_type;
    my_WorkData d_data;
    // Work data...
};

void myDoWork(my_WorkData&)
{
    // do some stuff...
}

void myConsumer(bcec_Queue<my_WorkRequest> *queue)
{
    while (1) {
        my_WorkRequest item = queue->popFront();
        if (item.d_type == my_WorkRequest::STOP) break;
        myDoWork(item.d_data);
    }
}

extern "C" void *myConsumerThread(void *queuePtr)
{
    myConsumer ((bcec_Queue<my_WorkRequest>*)queuePtr);
    return queuePtr;
}

void myProducer(int numThreads)
{
    my_WorkRequest item;
    my_WorkData workData;

    bcec_Queue<my_WorkRequest> queue;

    ASSERT(0 < numThreads && numThreads <= MAX_CONSUMER_THREADS);
    bcemt_ThreadUtil::Handle consumerHandles[MAX_CONSUMER_THREADS];

    for (int i = 0; i < numThreads; ++i) {
        bcemt_ThreadUtil::create(&consumerHandles[i],
                                 myConsumerThread,
                                 &queue);
    }

    while (!getWorkData(&workData)) {
        item.d_type = my_WorkRequest::WORK;
        item.d_data = workData;
        queue.pushBack(item);
    }

    for (int i = 0; i < numThreads; ++i) {
        item.d_type = my_WorkRequest::STOP;
        queue.pushBack(item);
    }

    for (int i = 0; i < numThreads; ++i) {
        bcemt_ThreadUtil::join(consumerHandles[i]);
    }
}

} // namespace BCEC_QUEUE_USAGE_EXAMPLE_1
//=============================================================================
//          USAGE example 2 from header (with assert replaced with ASSERT)
//-----------------------------------------------------------------------------
namespace BCEC_QUEUE_USAGE_EXAMPLE_2 {

enum {
    MAX_CONSUMER_THREADS=10
  , MAX_EVENT_TEXT=80
};

struct my_Event {
    enum EventType {
          IN_PROGRESS=1
        , TASK_COMPLETE=2
    };

    EventType d_type;
    int d_workerId;
    int d_eventNumber;
    char d_eventText[MAX_EVENT_TEXT];
};

struct my_WorkerData {
    int d_workerId;
    bcec_Queue<my_Event> *d_queue_p;
};

void myWorker(int workerId, bcec_Queue<my_Event> *queue)
{
    const int NEVENTS = 5;
    int evnum;

    for (evnum = 0; evnum < NEVENTS; ++evnum) {
        my_Event ev = {
            my_Event::IN_PROGRESS,
            workerId,
            evnum,
            "In-Progress Event"
        };
        queue->pushBack(ev);
    }

    my_Event ev = {
        my_Event::TASK_COMPLETE,
        workerId,
        evnum,
        "Task Complete"
    };
    queue->pushBack(ev);
}

extern "C"
void *myWorkerThread(void *v_worker_p)
{
    my_WorkerData  *worker_p = (my_WorkerData *) v_worker_p;
    myWorker(worker_p->d_workerId, worker_p->d_queue_p);
    return v_worker_p;
}

}  // namespace BCEC_QUEUE_USAGE_EXAMPLE_2

//=============================================================================
//          TEST CASE 12
//-----------------------------------------------------------------------------

namespace BCEC_QUEUE_TEST_CASE_12 {

class TestPopFront {
    Obj     *d_mX;
    int      d_maxVecSize;
    Element  d_maxVecSizeAt;

  public:
    TestPopFront(Obj *mX) {
        d_mX = mX;
        d_maxVecSize = 0;
        d_maxVecSizeAt = 0;
    }
    void operator()() {
        int expectedVal = 0;
        Element e;
        vector<Element> v;

        while (expectedVal < 50) {
            if (25 == expectedVal) {
                bcemt_ThreadUtil::microSleep(100 * 1000);
            }
            int sts = d_mX->tryPopFront(&e);
            if (!sts) {
                LOOP2_ASSERT(expectedVal, e, expectedVal++ == e && "popFront");
            }

            v.clear();
            d_mX->tryPopFront(20, &v);
            int s = v.size();
            for (int i = 0; i < s; ++i) {
                LOOP2_ASSERT(expectedVal, v[i],
                                          expectedVal++ == v[i] && "popFront");
            }
            if (s > d_maxVecSize) {
                d_maxVecSize = v.size();
                d_maxVecSizeAt = expectedVal;
            }
        }

        if (verbose) {
            cout << "PopFront: max vecsize = " << d_maxVecSize <<
                    " at " << d_maxVecSizeAt << endl;
        }
    }
};

class TestPopBack {
    Obj     *d_mX;
    int      d_maxVecSize;
    Element  d_maxVecSizeAt;

  public:
    TestPopBack(Obj *mX) {
        d_mX = mX;
        d_maxVecSize = 0;
        d_maxVecSizeAt = 0;
    }
    void operator()() {
        int expectedVal = 0;
        Element e;
        vector<Element> v;

        while (expectedVal < 50) {
            if (25 == expectedVal) {
                bcemt_ThreadUtil::microSleep(100 * 1000);
            }
            int sts = d_mX->tryPopBack(&e);
            if (!sts) {
                LOOP2_ASSERT(expectedVal, e, expectedVal++ == e && "popBack");
            }

            v.clear();
            d_mX->tryPopBack(20, &v);
            int s = v.size();
            for (int i = 0; i < s; ++i) {
                LOOP2_ASSERT(expectedVal, v[i],
                                           expectedVal++ == v[i] && "popBack");
            }
            if (s > d_maxVecSize) {
                d_maxVecSize = v.size();
                d_maxVecSizeAt = expectedVal;
            }
        }

        if (verbose) {
            cout << "PopBack: max vecsize = " << d_maxVecSize <<
                    " at " << d_maxVecSizeAt << endl;
        }
    }
};

}  //  namespace BCEC_QUEUE_TEST_CASE_12

//=============================================================================
//          TEST CASE 11
//-----------------------------------------------------------------------------

namespace BCEC_QUEUE_TEST_CASE_11 {

int exitCode1;
int exitCode2;
int exitCode3;

void *const THREAD_EXIT_1 = &exitCode1;
void *const THREAD_EXIT_2 = &exitCode2;
void *const THREAD_EXIT_3 = &exitCode3;

class TestClass13 {      // this class is a functor passed to thread::create
    Obj              *d_queue;
    bcemt_Barrier    *d_barrier;
    bdet_TimeInterval d_timeout;

  public:
    static int         s_pushCount;
    enum {
        VALID_VAL = 45,
        INVALID_VAL = 46
    };

    TestClass13(Obj *queue, bcemt_Barrier *barrier) {
        d_queue = queue;
        d_barrier = barrier;
        s_pushCount = 0;

        // have everything time out 2 seconds after thread object creation
        d_timeout = bdetu_SystemTime::now() + bdet_TimeInterval(4.0);
    }
    ~TestClass13() {
        // make sure we did not wait until timeout
        ASSERT(bdetu_SystemTime::now() < d_timeout);
    }
    void operator()() {         // thread function
        int sts;

        for (int i = 0; 6 > i; ++i) {
            bool back = !(i & 1);

            Element e = VALID_VAL;

            if (back) {
                ASSERT((sts = d_queue->timedPushBack(e, d_timeout), !sts));
            }
            else {
                ASSERT((sts = d_queue->timedPushFront(e, d_timeout), !sts));
            }
            if (sts) {
                bcemt_ThreadUtil::exit(THREAD_EXIT_1);
            }

            ++s_pushCount;

            ASSERT((sts = d_barrier->timedWait(d_timeout), !sts));
            if (sts) {
                bcemt_ThreadUtil::exit(THREAD_EXIT_2);
            }
        }
        bcemt_ThreadUtil::exit(THREAD_EXIT_3);
    }
};
int TestClass13::s_pushCount;

}  // close namespace BCEC_QUEUE_TEST_CASE_11

//=============================================================================
//          TEST CASE 10
//-----------------------------------------------------------------------------
namespace BCEC_QUEUE_TEST_CASE_10 {

class TestClass12 {      // this class is a functor passed to thread::create
    Obj              *d_queue;
    bcemt_Barrier    *d_barrier;
    bdet_TimeInterval d_timeout;

  public:
    enum {
        VALID_VAL = 45,
        TERMINATE = 46
    };
    TestClass12(Obj *queue, bcemt_Barrier *barrier) {
        d_queue = queue;
        d_barrier = barrier;

        // have everything time out 4 seconds after thread object creation
        d_timeout = bdetu_SystemTime::now() + bdet_TimeInterval(4.0);
    }
    ~TestClass12() {
        // make sure we did not wait until timeout
        ASSERT(bdetu_SystemTime::now() < d_timeout);
    }
    void operator()() {         // thread function
        Element e;
        int sts;

        for (bool back = false; true; back = !back) {
            if (back) {
                ASSERT((sts = d_queue->timedPopBack(&e, d_timeout), !sts));
            }
            else {
                ASSERT((sts = d_queue->timedPopFront(&e, d_timeout), !sts));
            }
            if (sts) {
                bcemt_ThreadUtil::exit((void *) 1);
            }

            if (TERMINATE == e) {
                bcemt_ThreadUtil::exit((void *) 0);
            }

            ASSERT(VALID_VAL == e);

            ASSERT((sts = d_barrier->timedWait(d_timeout), !sts));
            if (sts) {
                bcemt_ThreadUtil::exit((void *) 2);
            }
        }
    }
};

}  // close namespace BCEC_QUEUE_TEST_CASE_10

//=============================================================================
//          TEST CASE 6
//-----------------------------------------------------------------------------
namespace BCEC_QUEUE_TEST_CASE_6 {

class TestClass6 {

    // DATA
    bcec_Queue<Element> *d_queue_p;
    MyBarrier       *d_barrier_p;
    bces_AtomicInt       d_stage;
    Element              d_toBePopped;
    Element              d_toBePushed;

  public:
    // CREATORS
    TestClass6(bcec_Queue<Element> *queue,
               MyBarrier       *barrier,
               const Element&       value)
    : d_queue_p(queue)
    , d_barrier_p(barrier)
    , d_stage(0)
    , d_toBePopped(value)
    , d_toBePushed(value)
    {
    }

    // MANIPULATORS
    void callback()
    {
        d_barrier_p->wait();

        // Stage 1: verify that 'pushBack' blocks on mutex in main thread
        d_stage = 1;
        d_queue_p->pushBack(1.); // blocks
        ASSERT(1. == d_queue_p->popFront()); // empty queue, does not block
        d_barrier_p->wait();

        // Stage 2: verify that 'popFront' blocks on empty queue in main thread
        d_stage = 2;
        ASSERT(d_toBePopped == d_queue_p->popFront()); // blocks
        d_barrier_p->wait();

        // Stage 3: verify that 'pushBack' blocks on full queue in main thread
        d_stage = 3;
        d_barrier_p->wait(); // until the queue is filled to the high-water
                             // mark
        d_queue_p->pushBack(d_toBePushed); //blocks
        d_barrier_p->wait();
    }

    // ACCESSORS
    int stage() const { return d_stage; }
};

extern "C"
void *test6(void *arg)
{
    using namespace BCEC_QUEUE_TEST_CASE_6;
    TestClass6 *x = (TestClass6*)arg;
    x->callback();
    return 0;
}

} // namespace BCEC_QUEUE_TEST_CASE_6
//=============================================================================
//          TEST CASE 5
//-----------------------------------------------------------------------------
namespace BCEC_QUEUE_TEST_CASE_5 {

class TestClass5back {

    // DATA
    bcec_Queue<Element> *d_queue_p;
    MyBarrier       *d_barrier_p;
    bdet_TimeInterval    d_timeout;
    bces_AtomicInt       d_timeoutFlag;
    bces_AtomicInt       d_waitingFlag;
    Element              d_toBeInserted;

  public:
    // CREATORS
    TestClass5back(bcec_Queue<Element> *queue,
                   MyBarrier       *barrier,
                   bdet_TimeInterval    timeout,
                   Element              value)
    : d_queue_p(queue)
    , d_barrier_p(barrier)
    , d_timeout(timeout)
    , d_timeoutFlag(0)
    , d_waitingFlag(1)
    , d_toBeInserted(value)
    {
    }

    // MANIPULATORS
    void callback()
    {
        d_waitingFlag = 1;

        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPushBack(
                                          d_toBeInserted,
                                          bdetu_SystemTime::now() + d_timeout);
        d_waitingFlag = 0;

        d_barrier_p->wait();
        d_waitingFlag = 1;
        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPushBack(
                                          d_toBeInserted,
                                          bdetu_SystemTime::now() + d_timeout);
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int timeOutFlag() const { return d_timeoutFlag; }
    int waitingFlag() const { return d_waitingFlag; }
};

class TestClass5front {

    // DATA
    bcec_Queue<Element> *d_queue_p;
    MyBarrier       *d_barrier_p;
    bdet_TimeInterval    d_timeout;
    bces_AtomicInt       d_timeoutFlag;
    bces_AtomicInt       d_waitingFlag;
    Element              d_toBeInserted;

  public:
    // CREATORS
    TestClass5front(bcec_Queue<Element> *queue,
                    MyBarrier       *barrier,
                    bdet_TimeInterval    timeout,
                    Element              value)
    : d_queue_p(queue)
    , d_barrier_p(barrier)
    , d_timeout(timeout)
    , d_timeoutFlag(0)
    , d_waitingFlag(1)
    , d_toBeInserted(value)
    {
    }

    // MANIPULATORS
    void callback()
    {
        d_waitingFlag = 1;

        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPushFront(
                                          d_toBeInserted,
                                          bdetu_SystemTime::now() + d_timeout);
        d_waitingFlag = 0;

        d_barrier_p->wait();
        d_waitingFlag = 1;
        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPushFront(
                                          d_toBeInserted,
                                          bdetu_SystemTime::now() + d_timeout);
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int timeOutFlag() const { return d_timeoutFlag; }
    int waitingFlag() const { return d_waitingFlag; }
};

extern "C"
void *test5back(void *arg)
{
    using namespace BCEC_QUEUE_TEST_CASE_5;
    TestClass5back *x = (TestClass5back*)arg;
    x->callback();
    return 0;
}

extern "C"
void *test5front(void *arg)
{
    using namespace BCEC_QUEUE_TEST_CASE_5;
    TestClass5front *x = (TestClass5front*)arg;
    x->callback();
    return 0;
}

} // namespace BCEC_QUEUE_TEST_CASE_5
//=============================================================================
//          TEST CASE 4
//-----------------------------------------------------------------------------
namespace BCEC_QUEUE_TEST_CASE_4 {

class TestClass4back {

    // DATA
    bcec_Queue<Element> *d_queue_p;
    bces_AtomicInt       d_waitingFlag;
    Element              d_toBeInserted;

  public:
    // CREATORS
    TestClass4back(bcec_Queue<Element> *queue,
                   const Element&       value)
    : d_queue_p(queue)
    , d_waitingFlag(0)
    , d_toBeInserted(value)
    {
    }

    // MANIPULATORS
    void callback()
    {
        d_waitingFlag = 1;
        d_queue_p->pushBack(d_toBeInserted);
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int waitingFlag() {
        return d_waitingFlag;
    }
};

class TestClass4front {

    // DATA
    bcec_Queue<Element> *d_queue_p;
    bces_AtomicInt       d_waitingFlag;
    Element              d_toBeInserted;

  public:
    // CREATORS
    TestClass4front(bcec_Queue<Element> *queue,
                    const Element&       value)
    : d_queue_p(queue)
    , d_waitingFlag(0)
    , d_toBeInserted(value)
    {
    }

    // MANIPULATORS
    void callback()
    {
        d_waitingFlag = 1;
        d_queue_p->pushFront(d_toBeInserted);
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int waitingFlag()
    {
        return d_waitingFlag;
    }
};

extern "C"
void *test4back(void *arg)
{
    using namespace BCEC_QUEUE_TEST_CASE_4;
    TestClass4back *x = (TestClass4back*)arg;
    x->callback();
    return 0;
}

extern "C"
void *test4front(void *arg)
{
    using namespace BCEC_QUEUE_TEST_CASE_4;
    TestClass4front *x = (TestClass4front*)arg;
    x->callback();
    return 0;
}

} // namespace BCEC_QUEUE_TEST_CASE_4
//=============================================================================
//          TEST CASE 3
//-----------------------------------------------------------------------------
namespace BCEC_QUEUE_TEST_CASE_3 {

class TestClass3back {

    // DATA
    bcec_Queue<Element> *d_queue_p;
    MyBarrier       *d_barrier_p;
    bdet_TimeInterval    d_timeout;
    bces_AtomicInt       d_timeoutFlag;
    bces_AtomicInt       d_waitingFlag;
    Element              d_expected;

  public:
    // CREATORS
    TestClass3back(bcec_Queue<Element> *queue,
                   MyBarrier       *barrier,
                   bdet_TimeInterval    timeout,
                   Element              val)
    : d_queue_p(queue)
    , d_barrier_p(barrier)
    , d_timeout(timeout)
    , d_timeoutFlag(0)
    , d_waitingFlag(1)
    , d_expected(val)
    {
    }

    // MANIPULATORS
    void callback()
    {
        Element result;

        d_waitingFlag = 1;
        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPopBack(
                                          &result,
                                          bdetu_SystemTime::now() + d_timeout);
        d_waitingFlag = 0;

        d_barrier_p->wait();
        d_waitingFlag = 1;
        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPopBack(
                                          &result,
                                          bdetu_SystemTime::now() + d_timeout);
        d_waitingFlag = 0;
        if (0 == d_timeoutFlag) {
            ASSERT(result == d_expected);
        }
    }

    // ACCESSORS
    int  timeOutFlag() const { return d_timeoutFlag; }
    int  waitingFlag() const { return d_waitingFlag; }
};

class TestClass3front {

    // DATA
    bcec_Queue<Element> *d_queue_p;
    MyBarrier       *d_barrier_p;
    bdet_TimeInterval    d_timeout;
    bces_AtomicInt       d_timeoutFlag;
    bces_AtomicInt       d_waitingFlag;
    Element              d_expected;

  public:
    // CREATORS
    TestClass3front(bcec_Queue<Element> *queue,
                    MyBarrier       *barrier,
                    bdet_TimeInterval    timeout,
                    Element              value)
    : d_queue_p(queue)
    , d_barrier_p(barrier)
    , d_timeout(timeout)
    , d_timeoutFlag(0)
    , d_waitingFlag(1)
    , d_expected(value)
    {
    }

    // MANIPULATORS
    void callback()
    {
        Element result;

        d_waitingFlag = 1;
        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPopFront(
                                          &result,
                                          bdetu_SystemTime::now() + d_timeout);
        d_waitingFlag = 0;

        d_barrier_p->wait();
        d_waitingFlag = 1;
        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPopFront(
                                          &result,
                                          bdetu_SystemTime::now() + d_timeout);
        d_waitingFlag = 0;
        if (0 == d_timeoutFlag) {
            ASSERT(result == d_expected);
        }
    }

    // ACCESSORS
    int  timeOutFlag() const { return d_timeoutFlag; }
    int waitingFlag() const { return d_waitingFlag; }
};

// need additional tests to ensure timedPopFront & timedPopBack are waiting
// long enough to time out

struct TestStruct3 {
    bcec_Queue<Element> *d_queue_p;
    bdet_TimeInterval    d_timeout;

    void operator()() {
        Element result;

        d_queue_p->removeAll();

        bdet_TimeInterval start = bdetu_SystemTime::now();
        int sts = d_queue_p->timedPopFront(&result,
                                            start + d_timeout);
        bdet_TimeInterval end = bdetu_SystemTime::now();
        ASSERT(0 != sts);
        ASSERT(end >= start + d_timeout);

        start = bdetu_SystemTime::now();
        sts = d_queue_p->timedPopBack(&result,
                                       start + d_timeout);
        end = bdetu_SystemTime::now();
        ASSERT(0 != sts);
        ASSERT(end >= start + d_timeout);
    }
};

} // namespace BCEC_QUEUE_TEST_CASE_3

extern "C" {
    void *test3back(void *arg)
    {
        using namespace BCEC_QUEUE_TEST_CASE_3;
        TestClass3back *x = (TestClass3back*)arg;
        x->callback();
        return 0;
    }

    void *test3front(void *arg)
    {
        using namespace BCEC_QUEUE_TEST_CASE_3;
        TestClass3front *x = (TestClass3front*)arg;
        x->callback();
        return 0;
    }
} // extern "C"

//=============================================================================
//          TEST CASE 2
//-----------------------------------------------------------------------------
namespace BCEC_QUEUE_TEST_CASE_2 {

class TestClass2back {

    // DATA
    bcec_Queue<Element> *d_queue_p;
    bces_AtomicInt       d_waitingFlag;
    Element              d_expected;

  public:
    // CREATORS
    TestClass2back(bcec_Queue<Element> *queue,
                   const Element&       value)
    : d_queue_p(queue)
    , d_waitingFlag(0)
    , d_expected(value)
    {
    }

    // MANIPULATORS
    void callback()
    {
        d_waitingFlag = 1;
        ASSERT(d_expected == d_queue_p->popBack());
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int waitingFlag() { return d_waitingFlag; }
};

class TestClass2front {

    // DATA
    bcec_Queue<Element> *d_queue_p;
    bces_AtomicInt       d_waitingFlag;
    Element              d_expected;

  public:
    // CREATORS
    TestClass2front(bcec_Queue<Element> *queue,
                    const Element&       value)
    : d_queue_p(queue)
    , d_waitingFlag(0)
    , d_expected(value)
    {
    }

    // MANIPULATORS
    void callback()
    {
        d_waitingFlag = 1;
        ASSERT(d_expected == d_queue_p->popFront());
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int waitingFlag() { return d_waitingFlag; }
};

extern "C"
void *test2back(void *arg)
{
    using namespace BCEC_QUEUE_TEST_CASE_2;
    TestClass2back *x = (TestClass2back*)arg;
    x->callback();
    return 0;
}

extern "C"
void *test2front(void *arg)
{
    using namespace BCEC_QUEUE_TEST_CASE_2;
    TestClass2front *x = (TestClass2front*)arg;
    x->callback();
    return 0;
}

} // namespace BCEC_QUEUE_TEST_CASE_2

//=============================================================================
//          TEST CASE 2
//-----------------------------------------------------------------------------
namespace BCEC_QUEUE_TEST_CASE_MINUS_1 {

bcemt_Mutex outputMutex;

struct Producer {
    bcec_Queue<int>    *d_queue;
    int                 d_iterations;

    void operator()() const {
        for (int i = 0; i < d_iterations; ++i) {
            d_queue->pushBack(i);
        }

        if (verbose) {
            bcemt_LockGuard<bcemt_Mutex> guard(&outputMutex);

            cout << "Producer finishing\n";
        }

        d_queue->pushBack(-1);
    }
};

struct Consumer {
    bcec_Queue<int>    *d_queue;

    void operator()() const {
        int terminatorsFound = 0;
        int val;
        do {
            val = d_queue->popFront();
            if (-1 == val) {
                ++terminatorsFound;
            }
        } while (terminatorsFound < 2);

        if (verbose) {
            bcemt_LockGuard<bcemt_Mutex> guard(&outputMutex);

            cout << "Consumer finishing\n";
        }
    }
};

} // namespace BCEC_QUEUE_TEST_CASE_MINUS_1

//=============================================================================
//         SEQUENCE CONSTRAINT TEST
//-----------------------------------------------------------------------------
namespace seqtst {

struct Item {
    int  d_threadId;
    int  d_sequenceNum;
    bdef_Function<void(*)()> d_funct;
};

struct Control {
    bcemt_Barrier         *d_barrier;

    bcec_Queue<Item> *d_queue;

    int                    d_numExpectedPushers;
    int                    d_iterations;

    bces_AtomicInt         d_numPushers;
    bces_AtomicInt         d_numPopped;
};

void f(const bcema_SharedPtr<int>&)
{
}

void pusherThread(Control *control)
{
    bcema_SharedPtr<int> sp;
    sp.createInplace(bslma_Default::allocator(), 12345);

    bdef_Function<void(*)()> funct = bdef_BindUtil::bind(&f, sp);

    bcec_Queue<Item> *queue = control->d_queue;

    int threadId = control->d_numPushers++;

    control->d_barrier->wait();

    for (int i=0; i<control->d_iterations; i++) {
        Item item;
        item.d_threadId = threadId;
        item.d_sequenceNum = i;
        item.d_funct = funct;
        queue->pushBack(item);
    }
}

void popperThread(Control *control)
{
    bsl::vector<int> seq(control->d_numExpectedPushers, -1);

    bcec_Queue<Item> *queue = control->d_queue;

    int totalToPop = control->d_numExpectedPushers * control->d_iterations;

    control->d_barrier->wait();

    int numPopped;
    while((numPopped = control->d_numPopped++) < totalToPop) {

        Item item = queue->popFront();

        ASSERT(seq[item.d_threadId] < item.d_sequenceNum);
        seq[item.d_threadId] = item.d_sequenceNum;
    }
}

void runtest(int numIterations, int numPushers, int numPoppers)
{
    enum {
        QUEUE_SIZE = 2047
    };

    bcec_Queue<Item> queue(QUEUE_SIZE);

    bcemt_Barrier barrier(numPushers + numPoppers);

    Control control;

    control.d_numExpectedPushers = numPushers;
    control.d_iterations = numIterations;
    control.d_queue = &queue;

    control.d_numPopped = 0;
    control.d_numPushers = 0;

    control.d_barrier = &barrier;

    bcemt_ThreadGroup tg;
    tg.addThreads(bdef_BindUtil::bind(&pusherThread,&control),
            numPushers);
    tg.addThreads(bdef_BindUtil::bind(&popperThread,&control),
            numPoppers);

    tg.joinAll();
}
}

namespace seqtst2 {

struct Item {
    int  d_threadId;
    int  d_sequenceNum;
    bdef_Function<void(*)()> d_funct;
};

struct Control {
    bcemt_Barrier         *d_barrier;

    bcec_Queue<Item> *d_queue;

    int                    d_numExpectedPushers;
    int                    d_iterations;

    bces_AtomicInt         d_numPushers;
    bces_AtomicInt         d_numPopped;
};

void f(const bcema_SharedPtr<int>&)
{
}

void pusherThread(Control *control)
{
    bcema_SharedPtr<int> sp;
    sp.createInplace(bslma_Default::allocator(), 12345);

    bdef_Function<void(*)()> funct = bdef_BindUtil::bind(&f, sp);

    bcec_Queue<Item> *queue = control->d_queue;

    int threadId = control->d_numPushers++;

    control->d_barrier->wait();

    for (int i=0; i<control->d_iterations; i++) {
        Item item;
        item.d_threadId = threadId;
        item.d_sequenceNum = i;
        item.d_funct = funct;
        queue->pushBack(item);
    }
}

void popperThread(Control *control)
{
    bsl::vector<int> seq(control->d_numExpectedPushers, -1);

    bcec_Queue<Item> *queue = control->d_queue;

    int totalToPop = control->d_numExpectedPushers * control->d_iterations;

    control->d_barrier->wait();

    int numPopped;
    while((numPopped = control->d_numPopped++) < totalToPop) {

        Item item;
        queue->popFront(&item);

        ASSERT(seq[item.d_threadId] < item.d_sequenceNum);
        seq[item.d_threadId] = item.d_sequenceNum;
    }
}

void runtest(int numIterations, int numPushers, int numPoppers)
{
    enum {
        QUEUE_SIZE = 2047
    };

    bcec_Queue<Item> queue(QUEUE_SIZE);

    bcemt_Barrier barrier(numPushers + numPoppers);

    Control control;

    control.d_numExpectedPushers = numPushers;
    control.d_iterations = numIterations;
    control.d_queue = &queue;

    control.d_numPopped = 0;
    control.d_numPushers = 0;

    control.d_barrier = &barrier;

    bcemt_ThreadGroup tg;
    tg.addThreads(bdef_BindUtil::bind(&pusherThread,&control),
            numPushers);
    tg.addThreads(bdef_BindUtil::bind(&popperThread,&control),
            numPoppers);

    tg.joinAll();
}
}

namespace seqtst3 {

struct Item {
    int  d_threadId;
    int  d_sequenceNum;
    bdef_Function<void(*)()> d_funct;
};

struct Control {
    bcemt_Barrier         *d_barrier;

    bcec_Queue<Item> *d_queue;

    int                    d_numExpectedPushers;
    int                    d_iterations;

    bces_AtomicInt         d_numPushers;
    bces_AtomicInt         d_numPopped;
};

void f(const bcema_SharedPtr<int>&)
{
}

void pusherThread(Control *control)
{
    bcema_SharedPtr<int> sp;
    sp.createInplace(bslma_Default::allocator(), 12345);

    bdef_Function<void(*)()> funct = bdef_BindUtil::bind(&f, sp);

    bcec_Queue<Item> *queue = control->d_queue;

    int threadId = control->d_numPushers++;

    control->d_barrier->wait();

    for (int i=0; i<control->d_iterations; i++) {
        Item item;
        item.d_threadId = threadId;
        item.d_sequenceNum = i;
        item.d_funct = funct;
        queue->pushFront(item);
    }
}

void popperThread(Control *control)
{
    bsl::vector<int> seq(control->d_numExpectedPushers, -1);

    bcec_Queue<Item> *queue = control->d_queue;

    int totalToPop = control->d_numExpectedPushers * control->d_iterations;

    control->d_barrier->wait();

    int numPopped;
    while((numPopped = control->d_numPopped++) < totalToPop) {

        Item item;
        queue->popBack(&item);

        ASSERT(seq[item.d_threadId] < item.d_sequenceNum);
        seq[item.d_threadId] = item.d_sequenceNum;
    }
}

void runtest(int numIterations, int numPushers, int numPoppers)
{
    enum {
        QUEUE_SIZE = 2047
    };

    bcec_Queue<Item> queue(QUEUE_SIZE);

    bcemt_Barrier barrier(numPushers + numPoppers);

    Control control;

    control.d_numExpectedPushers = numPushers;
    control.d_iterations = numIterations;
    control.d_queue = &queue;

    control.d_numPopped = 0;
    control.d_numPushers = 0;

    control.d_barrier = &barrier;

    bcemt_ThreadGroup tg;
    tg.addThreads(bdef_BindUtil::bind(&pusherThread,&control),
            numPushers);
    tg.addThreads(bdef_BindUtil::bind(&popperThread,&control),
            numPoppers);

    tg.joinAll();
}
}

//=============================================================================
//         ZERO PTR TEST
//-----------------------------------------------------------------------------
namespace zerotst {

struct Control {
    bcemt_Barrier         *d_barrier;

    bcec_Queue<void *>    *d_queue;

    int                    d_numExpectedPushers;
    int                    d_iterations;

    bces_AtomicInt         d_numPushers;
    bces_AtomicInt         d_numPopped;
};

void pusherThread(Control *control)
{
    int threadId;

    bcec_Queue<void *> *queue = control->d_queue;

    threadId = control->d_numPushers++;

    control->d_barrier->wait();

    for (int i=0; i<control->d_iterations; i++) {
        queue->pushBack(0);
    }
}

void popperThread(Control *control)
{
    bcec_Queue<void *> *queue = control->d_queue;

    int totalToPop = control->d_numExpectedPushers * control->d_iterations;

    control->d_barrier->wait();

    int numPopped;
    while((numPopped = control->d_numPopped++) < totalToPop) {
        queue->popFront();
    }
}

void runtest(int numIterations, int numPushers, int numPoppers)
{
    enum {
        QUEUE_SIZE = 2047
    };

    bcec_Queue<void *> queue(QUEUE_SIZE);

    bcemt_Barrier barrier(numPushers + numPoppers);

    Control control;

    control.d_numExpectedPushers = numPushers;
    control.d_iterations = numIterations;
    control.d_queue = &queue;

    control.d_numPopped = 0;
    control.d_numPushers = 0;

    control.d_barrier = &barrier;

    bcemt_ThreadGroup tg;
    tg.addThreads(bdef_BindUtil::bind(&pusherThread,&control),
            numPushers);
    tg.addThreads(bdef_BindUtil::bind(&popperThread,&control),
            numPoppers);

    tg.joinAll();
}
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bcema_TestAllocator da(veryVeryVeryVerbose);
    bslma_DefaultAllocatorGuard defaultAllocatorGuard(&da);

    switch (test) { case 0:  // Zero is always the leading case.
      case 19: {
        // ---------------------------------------------------------
        // TESTING sequence constraints using 'backwards'
        // ---------------------------------------------------------
        if (verbose) cout << endl
                          << "sequence constraint test 'backwars'" << endl
                          << "===================================" << endl;
        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 5000
        };

        for(int numPushers=1; numPushers<=NUM_THREADS; numPushers++) {
        for(int numPoppers=1; numPoppers<=NUM_THREADS; numPoppers++) {

            seqtst3::runtest(NUM_ITERATIONS, numPushers, numPoppers);
        }
        }

      } break;

      case 18: {
        // ---------------------------------------------------------
        // TESTING sequence constraints using popFront(TYPE*)
        // ---------------------------------------------------------
        if (verbose) cout << endl
                          << "sequence constraint test using popFront(TYPE*)"
                          << endl
                          << "========================"
                          << endl;
        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 5000
        };

        for(int numPushers=1; numPushers<=NUM_THREADS; numPushers++) {
        for(int numPoppers=1; numPoppers<=NUM_THREADS; numPoppers++) {

            seqtst2::runtest(NUM_ITERATIONS, numPushers, numPoppers);
        }
        }

      } break;

      case 17: {
        // --------------------------------------------------------------------
        //  Basic test for popFront(TYPE*) and popBack(TYPE*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing popFront(TYPE*) and popBack(TYPE*)"
                          << "=========================================="
                          << endl;

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            if (verbose)
                cout << "Exercising default c'tor and basic methods" << endl;

            Obj x1(&ta);
            Element VA = 1.2;
            Element VB =- 5.7;
            Element VC = 1234.99;

            x1.pushBack(VA);
            x1.pushBack(VB);
            x1.pushBack(VC);

            Element buffer;
            x1.popFront(&buffer);
            ASSERT(VA == buffer);
            x1.popFront(&buffer);
            ASSERT(VB == buffer);
            x1.popFront(&buffer);
            ASSERT(VC == buffer);

            x1.pushFront(VA);
            x1.pushFront(VB);
            x1.pushFront(VC);

            x1.popBack(&buffer);
            ASSERT(VA == buffer);
            x1.popBack(&buffer);
            ASSERT(VB == buffer);
            x1.popBack(&buffer);
            ASSERT(VC == buffer);

        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

      } break;
      case 16: {
        // ---------------------------------------------------------
        // TESTING queue of zero ptr
        // ---------------------------------------------------------
        if (verbose) cout << endl
                          << "zero ptr test" << endl
                          << "========================" << endl;
        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 5000
        };

        for(int numPushers=1; numPushers<=NUM_THREADS; numPushers++) {
        for(int numPoppers=1; numPoppers<=NUM_THREADS; numPoppers++) {

            zerotst::runtest(NUM_ITERATIONS, numPushers, numPoppers);
        }
        }

      } break;

      case 15: {
        // ---------------------------------------------------------
        // TESTING sequence constraints
        // ---------------------------------------------------------
        if (verbose) cout << endl
                          << "sequence constraint test" << endl
                          << "========================" << endl;
        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 5000
        };

        for(int numPushers=1; numPushers<=NUM_THREADS; numPushers++) {
        for(int numPoppers=1; numPoppers<=NUM_THREADS; numPoppers++) {

            seqtst::runtest(NUM_ITERATIONS, numPushers, numPoppers);
        }
        }

      } break;

      case 14: {
        // --------------------------------------------------------------------
        // TEST USAGE EXAMPLE 1
        //   The first usage example from the header has been incorporated into
        //   this test driver.  All references to 'assert' have been replaced
        //   with 'ASSERT'.  Call the test example function and assert that it
        //   works as expected.
        //
        // Plan:
        //   Call the 'myProducer' function(from the usage example) with an
        //   arbitrary number of threads.  Assert that the function runs
        //   as expected.
        //
        // Testing:
        //   USAGE example
        // --------------------------------------------------------------------
        using namespace BCEC_QUEUE_USAGE_EXAMPLE_1;

        if (verbose) cout << endl
                          << "Testing USAGE example 1" << endl
                          << "=======================" << endl;
        {
            const int NTHREADS = 10;
            myProducer(NTHREADS);
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TEST USAGE EXAMPLE 2
        //   The second usage example from the header has been incorporated
        //   into this test driver.  All references to 'assert' have been
        //   replaced with 'ASSERT'.  Call the test example function and
        //   assert that it works as expected.
        //
        // Plan:
        //   1. Create a multi-threaded queue of 'my_Event's.
        //   2. Create a set of an arbitrary number of 'myWorker' threads,
        //      where each 'myWorker' thread simulates a single task.
        //   3. Each 'myWorker' thread generates and enqueues multiple
        //      'my_Event's.  Upon completion, each 'myWorker' thread enqueues
        //      a TASK_COMPLETE event.
        //   4. Count the TASK_COMPLETE events until all are complete; then
        //      "join" each thread.

        // Testing:
        //   USAGE example
        // --------------------------------------------------------------------
        using namespace BCEC_QUEUE_USAGE_EXAMPLE_2;

        if (verbose) cout << endl
                          << "Testing USAGE example 2" << endl
                          << "=======================" << endl;
        {
            const int NTHREADS = 10;
            bcec_Queue<my_Event> queue;

            ASSERT(0 < NTHREADS && NTHREADS <= MAX_CONSUMER_THREADS);
            bcemt_ThreadUtil::Handle workerHandles[MAX_CONSUMER_THREADS];

            my_WorkerData wdata;
            wdata.d_queue_p = &queue;
            for (int i=0; i < NTHREADS; ++i) {
                wdata.d_workerId = i;
                bcemt_ThreadUtil::create(&workerHandles[i],
                                         myWorkerThread,
                                         &wdata);
            }
            int n_Stop = 0;
            while (n_Stop < NTHREADS) {
                my_Event ev = queue.popFront();
                if (verbose) {
                    cout << "[" << ev.d_workerId << "] "
                         << ev.d_eventNumber << ". "
                         << ev.d_eventText << endl;
                }
                if (my_Event::TASK_COMPLETE == ev.d_type) {
                    ++n_Stop;
                    bcemt_ThreadUtil::join(workerHandles[ev.d_workerId]);
                }
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // multithreaded test of tryPopFront, tryPopBack
        //
        // Concern:
        //   That tryPopFront, tryPopBack will work in a multithreaded context.
        //
        // Plan:
        //   Have two functors, TestPopFront and TestPopBack.  Each functor
        //   will pop items off the queue and verify that the values are in
        //   the expected sequence.
        //
        // Testing:
        //   int tryPopFront(TYPE *);
        //   void tryPopFront(int, vector<TYPE> *);
        //   int tryPopBack(TYPE *);
        //   void tryPopBack(int, vector<TYPE> *);
        // --------------------------------------------------------------------

        using namespace BCEC_QUEUE_TEST_CASE_12;

        Element waitTable[] = { 4, 5, 6, 7, 15, 20, 30, 31, 32, 33, 34 };
        enum { waitTableLength = sizeof waitTable / sizeof *waitTable };
        const Element *waitTableBegin = waitTable;
        const Element *waitTableEnd = waitTable + waitTableLength;

        Obj mX;
        bcemt_ThreadUtil::Handle handle;

        for (int run = 0; run < 4; ++run) {
            bcemt_ThreadUtil::create(&handle, TestPopFront(&mX));
            bcemt_ThreadUtil::microSleep(10 * 1000);
            for (Element e = 0; e < 50; ++e) {
                if (waitTableEnd != bsl::find(waitTableBegin, waitTableEnd,
                                                                          e)) {
                    if (veryVerbose) {
                        cout << "PopFront: wait at " << e << endl;
                    }
                    bcemt_ThreadUtil::microSleep(10 * 1000);
                }
                mX.pushBack(e);
            }
            bcemt_ThreadUtil::join(handle);

            bcemt_ThreadUtil::create(&handle, TestPopBack(&mX));
            for (Element e = 0; e < 50; ++e) {
                if (waitTableEnd != bsl::find(waitTableBegin, waitTableEnd,
                                                                          e)) {
                    if (veryVerbose) {
                        cout << "PopBack: wait at " << e << endl;
                    }
                    bcemt_ThreadUtil::microSleep(10 * 1000);
                }
                mX.pushFront(e);
            }
            bcemt_ThreadUtil::join(handle);
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TEST BLOCKING ON EMPTY QUEUE
        //
        // Concern:
        //   That the queue blocks pushes properly when it is at the high
        //   watermark.
        //
        // Plan:
        //   Call both forms of push... with the queue in a variety of
        //   states and observe the results.
        //
        // Testing:
        //   pushFront
        //   pushBack
        // --------------------------------------------------------------------

        using namespace BCEC_QUEUE_TEST_CASE_11;

        bcema_TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(4, &ta);
        vector<Element> v;
        bcemt_ThreadUtil::Handle handle;
        bcemt_Barrier barrier(2);
        bdet_TimeInterval timeout = bdetu_SystemTime::now() +
                                                        bdet_TimeInterval(4.0);

        ASSERT(bdetu_SystemTime::now() < timeout);

        ASSERT(0 == mX.queue().length());

        TestClass13 tc13(&mX, &barrier);

        bcemt_ThreadUtil::create(&handle, tc13);

        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));

        bcemt_ThreadUtil::yield();
        bcemt_ThreadUtil::microSleep(50*1000);        // 50 mSec

        ASSERT(4 == tc13.s_pushCount);
        ASSERT(4 == mX.queue().length());  // 5th push is blocking on high
                                           // watermark

        ASSERT(TestClass13::VALID_VAL == mX.popFront());
        ASSERT(!barrier.timedWait(timeout));
        bcemt_ThreadUtil::yield();
        bcemt_ThreadUtil::microSleep(50*1000);        // 50 mSec
        ASSERT(5 == tc13.s_pushCount);
        ASSERT(4 == mX.queue().length());

        ASSERT(TestClass13::VALID_VAL == mX.popBack());
        ASSERT(!barrier.timedWait(timeout));
        bcemt_ThreadUtil::yield();
        bcemt_ThreadUtil::microSleep(50*1000);        // 50 mSec
        ASSERT(6 == tc13.s_pushCount);
        ASSERT(4 == mX.queue().length());

        for (int i = 0; 4 > i; ++i) {
            bool back = !(1 & i);
            if (back) {
                ASSERT(TestClass13::VALID_VAL == mX.popBack());
            }
            else {
                ASSERT(TestClass13::VALID_VAL == mX.popFront());
            }
            ASSERT(6 == tc13.s_pushCount);
            ASSERT(3 - i == mX.queue().length());
        }

        {
            void *sts = 0;
            bcemt_ThreadUtil::join(handle, &sts);
            LOOP_ASSERT(sts, THREAD_EXIT_3 == sts);
        }

        ASSERT(bdetu_SystemTime::now() < timeout);
      }  break;
      case 10: {
        // --------------------------------------------------------------------
        // TEST BLOCKING ON EMPTY QUEUE
        //
        // Concern:
        //   Does the queue block pops properly when it is empty.
        //
        // Plan:
        //   Call both forms of pop... with the queue in a variety of
        //   states and observe the results.
        //
        // Testing:
        //   int timedPopBack(TYPE *, const bdet_TimeInterval&);
        //   int timedPopFront(TYPE *, const bdet_TimeInterval&);
        // --------------------------------------------------------------------

        using namespace BCEC_QUEUE_TEST_CASE_10;

        bcema_TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(&ta);
        Element e = TestClass12::VALID_VAL;
        vector<Element> v;
        bcemt_ThreadUtil::Handle handle;
        bcemt_Barrier barrier(2);
        bdet_TimeInterval timeout = bdetu_SystemTime::now() +
                                                        bdet_TimeInterval(4.0);

        ASSERT(bdetu_SystemTime::now() < timeout);

        ASSERT(0 == mX.queue().length());

        TestClass12 tc12(&mX, &barrier);

        bcemt_ThreadUtil::create(&handle, tc12);

        mX.pushFront(e);
        mX.pushBack(e);

        ASSERT(1 <= mX.queue().length());
        ASSERT(2 >= mX.queue().length());

        ASSERT(!barrier.timedWait(timeout));

        ASSERT(0 <= mX.queue().length());
        ASSERT(1 >= mX.queue().length());

        mX.pushBack(e);
        mX.pushBack(e);
        mX.pushBack(e);
        mX.pushBack(e);

        ASSERT(!barrier.timedWait(timeout));

        ASSERT(3 <= mX.queue().length());
        ASSERT(4 >= mX.queue().length());

        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));

        ASSERT(0 == mX.queue().length());

        for (int i = 0; i < 5; ++i) {
            bcemt_ThreadUtil::yield();
            bcemt_ThreadUtil::microSleep(10*1000);        // 10 mSec
            mX.pushBack(e);
            ASSERT(!barrier.timedWait(timeout));
            ASSERT(0 == mX.queue().length());
        }
        for (int i = 0; i < 5; ++i) {
            bcemt_ThreadUtil::yield();
            bcemt_ThreadUtil::microSleep(10*1000);        // 10 mSec
            mX.pushFront(e);
            ASSERT(!barrier.timedWait(timeout));
            ASSERT(0 == mX.queue().length());
        }

        e = TestClass12::TERMINATE;
        mX.pushFront(e);

        {
            void *sts;
            bcemt_ThreadUtil::join(handle, &sts);
            LOOP_ASSERT((long) sts, !sts);
        }

        ASSERT(bdetu_SystemTime::now() < timeout);
      }  break;
      case 9: {
        // --------------------------------------------------------------------
        // TEST TRYPOPFRONT, TRYPOPBACK -- SINGLE THREAD TEST
        //
        // Concern:
        //   That tryPopFront and tryPopBack work as designed in a single -
        //   threaded context.
        //
        // Plan:
        //   Call both forms of tryPop... both with an empty queue and with
        //   a queue containing items, verify that return values (if any)
        //   are correct and that correct data is returned.
        //
        // Testing:
        //   int tryPopFront(TYPE *);
        //   void tryPopFront(int, vector<TYPE> *);
        //   int tryPopBack(TYPE *);
        //   void tryPopBack(int, vector<TYPE> *);
        // --------------------------------------------------------------------

        Obj mX(&da), &X = mX;
        vector<Element> v(&da);
        vector<Element> v2(&da);
        Element e;
        int sts;

        ASSERT(!X.queue().length());

        e = -7;
        sts = mX.tryPopFront(&e);
        ASSERT(0 != sts);
        ASSERT(-7 == e);
        mX.tryPopFront(100, &v);
        ASSERT(v.empty());
        mX.tryPopFront(100);

        ASSERT(!X.queue().length());

        for (int i = 0; i < 10; ++i) {
            mX.pushBack((Element) i);
        }

        mX.tryPopFront(1);
        e = -7;
        sts = mX.tryPopFront(&e);
        ASSERT(0 == sts);
        ASSERT(1 == e);

        mX.tryPopFront(4, &v);
        ASSERT(4 == v.size());
        ASSERT(2 == v.front());
        ASSERT(5 == v.back());
        v.clear();

        mX.tryPopFront(10, &v);
        ASSERT(4 == v.size());
        ASSERT(6 == v.front());
        ASSERT(9 == v.back());
        v.clear();

        mX.tryPopFront(1, &v);
        ASSERT(v.empty());

        mX.removeAll();

        e = -7;
        sts = mX.tryPopBack(&e);
        ASSERT(0 != sts);
        ASSERT(-7 == e);
        mX.tryPopBack(100);

        mX.removeAll();

        mX.tryPopBack(10, &v);
        ASSERT(v.empty());

        for (int i = 0; 10 > i; ++i) {
            mX.pushBack((Element) i);
        }

        mX.tryPopBack(1);

        e = -7;
        sts = mX.tryPopBack(&e);
        ASSERT(0 == sts);
        ASSERT(8 == e);

        mX.tryPopBack(4, &v);
        ASSERT(4 == v.size());
        ASSERT(7 == v.front());
        ASSERT(4 == v.back());

        v.clear();
        mX.tryPopBack(10, &v);
        ASSERT(4 == v.size());
        ASSERT(3 == v.front());
        ASSERT(0 == v.back());

        ASSERT(!X.queue().length());

        v.clear();
        mX.tryPopBack(1, &v);
        ASSERT(v.empty());

        for (int i = 0; i < 10; ++i) {
            mX.pushBack((Element) i);
            v.push_back(i - 10);
        }

        mX.tryPopFront(30, &v);
        ASSERT(20 == v.size());
        for (int i = 0; i < 20; ++i) {
            ASSERT(i - 10 == v[i]);
        }
        v.clear();

        for (int i = 0; i < 10; ++i) {
            mX.pushBack((Element) i);
            v.push_back(19 - i);
        }

        mX.tryPopBack(30, &v);
        ASSERT(20 == v.size());
        for (int i = 0; i < 20; ++i) {
            LOOP2_ASSERT(19 - i, v[i], 19 - i == v[i]);
        }
        v.clear();

        ASSERT(!X.queue().length());

        ASSERT(0 != mX.tryPopBack(&e));
        ASSERT(0 != mX.tryPopFront(&e));

        for (int i = 0; 10 > i; ++i) {
            mX.pushBack((Element) i);
        }

        for (int i = 9; 5 <= i; --i) {
            ASSERT(0 == mX.tryPopBack(&e));
            ASSERT(i == e);
        }
        for (int i = 0; 5 > i; ++i) {
            ASSERT(0 == mX.tryPopFront(&e));
            ASSERT(i == e);
        }

        ASSERT(!X.queue().length());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TEST USE OF THE BDEC_QUEUE INTERFACE
        //
        // Concern:
        //   That the example use of the 'bdec_Queue' interface code
        //   from the header works as expected.
        //
        // Plan:
        //   Insert to elements on a queue, run the code from the example
        //   section and assert that the two values have been removed properly.
        //
        // Testing:
        //   USAGE use of the 'bdec_Queue' interface
        // --------------------------------------------------------------------
        using namespace BCEC_QUEUE_USE_OF_BDEC_QUEUE_INTERFACE;

        {
            myWorkQueue.pushBack( myData() );
            myWorkQueue.pushBack( myData() );
            myWork();
            ASSERT(0 == rawQueue.length());
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING REMOVEALL
        //
        // Concerns:
        //   That 'removeAll' empties the queue, that it saves the elements
        //   in proper order into the specified buffer if one is provided, and
        //   destroys them properly otherwise.  Also check that it properly
        //   signals threads waiting for the 'notFullCondition'.
        //
        // Plan:
        //   Create a queue object with multiple elements enqueued.  Then
        //   invoke 'removeAll' and verify that the queue is empty and that all
        //   elements have been copied to the optionally specified buffer in
        //   the proper order.  Finally, create a queue with a high watermark,
        //   and fill it, then create a thread to push into it, verify that
        //   thread is blocked.  In the main thread, invoke 'removeAll' and
        //   verifies that pushing thread is unblocked.
        //
        // Testing:
        //    removeAll();
        //    removeAll(bsl::vector<T>& buffer);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'removeAll'" << endl
                          << "===================" << endl;

        Element VA = 1.2;
        Element VB =- 5.7;
        Element VC = 1234.99;

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            Obj mX(&ta);

            mX.pushBack(VA);
            mX.pushBack(VB);
            mX.pushBack(VC);

            mX.removeAll();
            ASSERT(0 == mX.queue().length());
        }

        {
            Obj mX(&ta);
            bsl::vector<Element> buffer(&ta);

            mX.pushBack(VA);
            mX.pushBack(VB);
            mX.pushBack(VC);

            mX.removeAll(&buffer);
            ASSERT(0 == mX.queue().length());
            ASSERT(3 == buffer.size());
            ASSERT(VA == buffer[0]);
            ASSERT(VB == buffer[1]);
            ASSERT(VC == buffer[2]);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING INTERNAL BDEC QUEUE ACCESSORS
        //
        // Concerns:
        //   That queue accessor gives internal access to queue, that mutex
        //   blocks other threads properly, and that the condition variables do
        //   signal other threads of the availability of data (for
        //   'notFullCondition') or of room beneath the highWaterMark (for
        //   'condition').
        //
        // Plan:
        //   Create a queue object with a positive high-water mark, then get
        //   an internal access to its queue and lock its mutex.  Then try to
        //   modify the queue in another thread and verify that it blocks
        //   without modifying, then unlock the mutex and verify that other
        //   thread does not block any longer.  Next, try to pop from the empty
        //   queue in second thread and verify that it blocks, then push an
        //   element onto the internal queue, verify that the second thread is
        //   still blocked, then signal the condition variable and verify that
        //   second thread is unblocked.  Finally, try to push element at the
        //   high-water mark and verify that it blocks, then pop an element
        //   from the internal queue and check that second thread is still
        //   blocked, then signal the insert condition variable and verify
        //   that second thread is unblocked.
        //
        // Testing:
        //   bcemt_Mutex& mutex();
        //   bcemt_Condition& notEmptyCondition();
        //   bcemt_Condition& notFullCondition();
        //   bdec_Queue<T>& queue();
        // --------------------------------------------------------------------
        using namespace BCEC_QUEUE_TEST_CASE_6;

        if (verbose)
            cout << endl
   // -----------^
   << "TESTING 'mutex', 'notEmptyCondition', 'notFullCondition' and 'queue'\n"
   << "====================================================================\n";
   // -----------v

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            const int HIGH_WATER_MARK = 2;
            const int T = 1 * DECI_SEC * MICRO_SEC_IN_SEC; // in microseconds
            bdet_TimeInterval T4(2 * DECI_SEC);

            Element VA = 1.2;
            Element VB = -5.7;
            Element VC = 1234.99;

            Obj x(HIGH_WATER_MARK, &ta);
            MyBarrier barrier;
            TestClass6 testObj(&x, &barrier, VA);

            bcemt_ThreadUtil::Handle thread;
            bcemt_ThreadUtil::create(&thread, test6, &testObj);

            ASSERT(0 == testObj.stage());
            {
                bcemt_LockGuard<bcemt_Mutex> lock(&x.mutex());
                barrier.wait();
                bcemt_ThreadUtil::yield();

                // This is not bullet-proof because it does not ENSURE that
                // testObj is blocking on the 'pushback', so we maximize our
                // chances by waiting more -- to prevent failure in high loads
                // should the mutex destruction below execute before stage 2 of
                // testObj (which unfortunately happens erratically).

                int iter = 1000;
                while (0 < --iter && testObj.stage() < 1) {
                    bcemt_ThreadUtil::yield();
                    bcemt_ThreadUtil::microSleep(10 * 1000);
                }
                // either testObj.stage() has changed or we have waited 10 sec
                ASSERT(1 == testObj.stage());
            }

            // LockGuard destruction unlocks x.mutex().

            barrier.wait();

            // Unfortunately, there is no way to test if the test thread is
            // waiting on the condition variable... so we just give it a chance
            // to unblock - it should continue to block.  This is not
            // bullet-proof because it does not ENSURE that testObj is blocking
            // on the popFront, so we maximize our chances by waiting some
            // more.

            int iter = 10;
            while (0 < --iter) {
                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(T);
            }

            {
                bcemt_LockGuard<bcemt_Mutex> lock(&x.mutex());
                x.queue().pushBack(VA);
                bcemt_ThreadUtil::microSleep(T);
                ASSERT(2 == testObj.stage());
            }
            bcemt_ThreadUtil::microSleep(T);
            ASSERT(2 == testObj.stage());

            // Make testObj stop waiting on 'popFront'.

            x.notEmptyCondition().signal();
            barrier.wait();

            // Fill the queue to the high-water mark.

            x.pushBack(VB);
            x.pushBack(VC);
            barrier.wait();

            ASSERT(3 == testObj.stage());

            // Again, there is no way to test if the test thread is waiting on
            // the condition variable, and the 'yield()' is not bullet-proof
            // because it does not ENSURE that testObj is blocking on the
            // 'pushBack', so we maximize our chances by waiting some more.

            iter = 10;
            while (0 < --iter) {
                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(T);
            }

            ASSERT(3 == testObj.stage());
            {
                bcemt_LockGuard<bcemt_Mutex> lock(&x.mutex());
                ASSERT(VC == x.queue().back());
                x.queue().popFront();
                bcemt_ThreadUtil::microSleep(T);
                ASSERT(3 == testObj.stage());
            }
            bcemt_ThreadUtil::microSleep(T);
            ASSERT(3 == testObj.stage());

            // Make testObj stop waiting on 'pushBack'.

            x.notFullCondition().signal();
            barrier.wait();

            ASSERT(VC == x.popFront());
            ASSERT(VA == x.popFront());

            bcemt_ThreadUtil::join(thread);
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING TIMED PUSH FUNCTIONS IN PRESENCE OF A HIGH WATER MARK
        //
        // Concerns:
        //   That the 'timedPushBack' and 'timedPushFront' functions properly
        //   time out unless the queue has fewer items than the high-water mark
        //
        // Plan:
        //   Create a queue object with a positive high-water mark, then push
        //   that many items on the queue and verify that none of the push
        //   operations block, then in a different thread add one more item and
        //   verify that it now times out.  Then if the high-water mark is
        //   positive, again add one more item in second thread, and in the
        //   main thread, after half the timeout (or less, for margin), pop one
        //   item and verify that the other thread is now unblocked and that
        //   the element has been added to the queue.
        //
        // Testing:
        //   void timedPushBack(const T& item);
        //   void timedPushFront(const T& item);
        // --------------------------------------------------------------------
        using namespace BCEC_QUEUE_TEST_CASE_5;

        if (verbose)
            cout << endl
                 << "TESTING 'timedPushBack' and 'timedPushFront'" << endl
                 << "============================================" << endl;

        static  const struct {
            int d_lineNum;
            int d_highWaterMark;
            int d_insertions;
        } VALUES[] = {
            // line     high-water mark    insertions
            // ----     ---------------    ----------
            { L_ ,                   -1,           10 },
         // { L_ ,                    0,            0 }, // undefined behavior
            { L_ ,                    1,            1 },
            { L_ ,                    3,            3 },
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        const int T = 1 * DECI_SEC * MICRO_SEC_IN_SEC; // in microseconds
        bdet_TimeInterval T4(4 * DECI_SEC);            // .4s
        bdet_TimeInterval T10(10 * DECI_SEC);          // 1s

        Element VA = 1.2;
        Element VB = -5.7;

        if (verbose) cout << "\tWith 'timedPushBack'" << endl;
        for (int i = 0; i< NUM_VALUES; ++i)
        {
            bcema_TestAllocator ta(veryVeryVeryVerbose);
            {
                const int HIGH_WATER_MARK = VALUES[i].d_highWaterMark;
                const int INSERTIONS = VALUES[i].d_insertions;

                Obj x(HIGH_WATER_MARK, &ta);
                MyBarrier barrier;
                TestClass5back testObj(&x, &barrier, T10, VB);

                LOOP_ASSERT(i, HIGH_WATER_MARK == x.highWaterMark() );
                for (int j = 0; j < INSERTIONS; ++j) {
                    x.pushBack(VA); // non-blocking
                }

                bcemt_ThreadUtil::Handle thread;
                bcemt_ThreadUtil::create(&thread, test5back, &testObj);

                barrier.wait();
                while ( 0 != testObj.waitingFlag() ) {
                    bcemt_ThreadUtil::yield();
                }
                if (-1 != HIGH_WATER_MARK) {
                    LOOP_ASSERT(i, 0 != testObj.timeOutFlag() );
                } else {
                    LOOP_ASSERT(i, 0 == testObj.timeOutFlag() );
                }

                barrier.wait();
                if (-1 != HIGH_WATER_MARK) {
                    bdet_TimeInterval now = bdetu_SystemTime::now();
                    barrier.wait();
                    bcemt_ThreadUtil::microSleep(T);

                    bdet_TimeInterval elapsed = bdetu_SystemTime::now() - now;
                    if (elapsed < T4) {
                        LOOP_ASSERT(i, 0 != testObj.waitingFlag() );
                        LOOP_ASSERT(i, VA == x.popBack() );

                        while ( 0 != testObj.waitingFlag() ) {
                            bcemt_ThreadUtil::yield();
                        }

                        LOOP_ASSERT(i, 0 == testObj.timeOutFlag() );

                        if (0 == testObj.timeOutFlag()) {
                            // Test that assert succeeded, otherwise
                            // 'x.popBack()' would time out.

                            LOOP_ASSERT(i, VB == x.popBack() );
                        }
                    } else {
                        bsl::cout << "*** Warning:  (line " << __LINE__ << ") "
                                  << "high load delays prevented test case 5 "
                                  << "to run properly ('timedPushBack')"
                                  << endl;
                    }
                }
                else {
                    barrier.wait();

                    while (0 != testObj.waitingFlag()) {
                        bcemt_ThreadUtil::yield();
                    }
                    LOOP_ASSERT(i, 0 == testObj.timeOutFlag() );
                }

                bcemt_ThreadUtil::join(thread);
            }
            LOOP_ASSERT(i, 0 == da.numAllocations());
            LOOP_ASSERT(i, 0 == ta.numBytesInUse());
            LOOP_ASSERT(i, 0 == ta.numMismatches());
            if (veryVerbose) { P(ta); }
        }

        if (verbose) cout << "\tWith 'timedPushFront'" << endl;
        for (int i = 0; i< NUM_VALUES; ++i)
        {
            bcema_TestAllocator ta(veryVeryVeryVerbose);
            {
                const int HIGH_WATER_MARK = VALUES[i].d_highWaterMark;
                const int INSERTIONS = VALUES[i].d_insertions;

                Obj x(HIGH_WATER_MARK, &ta);
                MyBarrier barrier;
                TestClass5front testObj(&x, &barrier, T10, VB);

                LOOP_ASSERT(i, HIGH_WATER_MARK == x.highWaterMark() );
                for (int j = 0; j < INSERTIONS; ++j) {
                    x.pushBack(VA); // non-blocking
                }

                bcemt_ThreadUtil::Handle thread;
                bcemt_ThreadUtil::create(&thread, test5front, &testObj);

                barrier.wait();
                while ( 0 != testObj.waitingFlag() ) {
                    bcemt_ThreadUtil::yield();
                }
                if (-1 != HIGH_WATER_MARK) {
                    LOOP_ASSERT(i, 0 != testObj.timeOutFlag() );
                } else {
                    LOOP_ASSERT(i, 0 == testObj.timeOutFlag() );
                }

                barrier.wait();
                if (-1 != HIGH_WATER_MARK) {
                    bdet_TimeInterval now = bdetu_SystemTime::now();
                    barrier.wait();
                    bcemt_ThreadUtil::microSleep(T);

                    bdet_TimeInterval elapsed = bdetu_SystemTime::now() - now;
                    if (elapsed < T4) {
                        LOOP_ASSERT(i, 0 != testObj.waitingFlag() );
                        LOOP_ASSERT(i, VA == x.popFront() );

                        while ( 0 != testObj.waitingFlag() ) {
                            bcemt_ThreadUtil::yield();
                        }
                        LOOP_ASSERT(i, 0 == testObj.timeOutFlag() );
                        if (0 == testObj.timeOutFlag()) {
                            // Otherwise 'x.popFront()' would time out.

                            LOOP_ASSERT(i, VB == x.popFront() );
                        }
                    } else {
                        bsl::cout << "*** Warning:  (line " << __LINE__ << ") "
                                  << "high load delays prevented test case 5 "
                                  << "to run properly ('timedPushFront')"
                                  << endl;
                    }
                }
                else {
                    barrier.wait();

                    while (0 != testObj.waitingFlag()) {
                        bcemt_ThreadUtil::yield();
                    }
                    LOOP_ASSERT(i, 0 == testObj.timeOutFlag() );
                }

                bcemt_ThreadUtil::join(thread);
            }
            LOOP_ASSERT(i, 0 == da.numAllocations());
            LOOP_ASSERT(i, 0 == ta.numBytesInUse());
            LOOP_ASSERT(i, 0 == ta.numMismatches());
            if (veryVerbose) { P(ta); }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PUSH FUNCTIONS IN PRESENCE OF A HIGH WATER MARK
        //
        // Concerns:
        //   That the 'pushBack' and 'pushFront' functions properly block
        //   unless the queue has fewer items than the high-water mark.
        //   That the 'forcePushFront' does not block.
        //
        // Plan:
        //   Create a queue object with a positive high-water mark, then push
        //   that many items on the queue and verify that none of the push
        //   operations block, then in a different thread add one more item and
        //   verify that it now blocks.  In the main thread, pop one item and
        //   verify that the other thread is now unblocked and that the element
        //   has been added to the queue.  When creating with negative high
        //   water mark, many insertions should never block.
        //
        // Testing:
        //   bcec_Queue(int highWaterMark,
        //              bslma_Allocator *basicAllocator = 0);
        //   int highWaterMark() const;
        //   void pushBack(const T& item);
        //   void pushFront(const T& item);
        //   void forcePushFront(const T& item);
        // --------------------------------------------------------------------
        using namespace BCEC_QUEUE_TEST_CASE_4;

        if (verbose) cout << endl
           << "TESTING 'highWaterMark' and associated c'tor along with" << endl
           << "'pushBack' and 'pushFront'" << endl
           << "======================================================="
           << "==========================" << endl;

        static  const struct {
            int d_lineNum;
            int d_highWaterMark;
        } VALUES[] = {
            // line     high-water mark
            // ----     ---------------
            { L_ ,                   -1 },
         // { L_ ,                    0 }, // undefined behavior
            { L_ ,                    1 },
            { L_ ,                    2 },
            { L_ ,                   10 },
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        const int T = 1 * DECI_SEC * MICRO_SEC_IN_SEC; // in microseconds

        const Element VA = 1.2;
        const Element VB = -5.7;

        if (verbose) cout << "\tWith 'pushBack'" << endl;
        for (int i = 0; i< NUM_VALUES; ++i)
        {
            bcema_TestAllocator ta(veryVeryVeryVerbose);
            {
                const int LINE            = VALUES[i].d_lineNum;
                const int HIGH_WATER_MARK = VALUES[i].d_highWaterMark;
                const int INSERTIONS      = (HIGH_WATER_MARK < 0)
                                          ? 10 : HIGH_WATER_MARK;
                if (veryVerbose) {
                    P_(LINE); P_(HIGH_WATER_MARK); P(INSERTIONS);
                }

                Obj x(HIGH_WATER_MARK, &ta);
                TestClass4back testObj(&x, VB);

                LOOP_ASSERT(i, HIGH_WATER_MARK == x.highWaterMark() );
                for (int j = 0; j < INSERTIONS; ++j) {
                    x.pushBack(VA); // non-blocking
                }

                bcemt_ThreadUtil::Handle thread;
                bcemt_ThreadUtil::create(&thread, test4back, &testObj);

                if (0 < HIGH_WATER_MARK) {
                    // Yielding is not bullet-proof because it does not ENSURE
                    // that testObj is blocking on the 'pushFront', so we
                    // make sure by waiting as long as necessary -- to prevent
                    // failure in high loads should the 'popBack' below
                    // execute before 'pushFront' in testObj (which actually
                    // happens erratically).

                    int iter = 100;
                    while (0 == testObj.waitingFlag() && 0 < --iter) {
                        bcemt_ThreadUtil::yield();
                        bcemt_ThreadUtil::microSleep(T);
                    }

                    LOOP_ASSERT(i, 0 != testObj.waitingFlag() );
                    LOOP_ASSERT(i, VA == x.popBack() );
                }

                while ( 0 != testObj.waitingFlag()){
                    bcemt_ThreadUtil::yield();
                }
                if (0 < HIGH_WATER_MARK) {
                    LOOP_ASSERT(i, VB == x.popBack() );
                }

                bcemt_ThreadUtil::join(thread);
            }
            LOOP_ASSERT(i, 0 == da.numAllocations());
            LOOP_ASSERT(i, 0 == ta.numBytesInUse());
            LOOP_ASSERT(i, 0 == ta.numMismatches());
            if (veryVerbose) { P(ta); }
        }

        if (verbose) cout << "\tWith 'push_front'" << endl;
        for (int i = 0; i< NUM_VALUES; ++i)
        {
            bcema_TestAllocator ta(veryVeryVeryVerbose);
            {
                const int LINE            = VALUES[i].d_lineNum;
                const int HIGH_WATER_MARK = VALUES[i].d_highWaterMark;
                const int INSERTIONS      = (HIGH_WATER_MARK < 0)
                                          ? 10 : HIGH_WATER_MARK;
                if (veryVerbose) {
                    P_(LINE); P_(HIGH_WATER_MARK); P(INSERTIONS);
                }

                Obj x(HIGH_WATER_MARK, &ta);
                TestClass4front testObj(&x, VB);

                LOOP_ASSERT(i, HIGH_WATER_MARK == x.highWaterMark() );
                for (int j = 0; j < INSERTIONS; ++j) {
                    x.pushBack(VA); // non-blocking
                }

                x.forcePushFront(VB); // should block, but force it
                LOOP_ASSERT(i, VB == x.popFront() );

                bcemt_ThreadUtil::Handle thread;
                bcemt_ThreadUtil::create(&thread, test4front, &testObj);

                if (0 < HIGH_WATER_MARK) {
                    // See comment above.

                    int iter = 100;
                    while (0 == testObj.waitingFlag() && 0 < --iter) {
                        bcemt_ThreadUtil::yield();
                        bcemt_ThreadUtil::microSleep(T);
                    }

                    LOOP_ASSERT(i, 0 != testObj.waitingFlag() );
                    LOOP_ASSERT(i, VA == x.popFront() );
                }

                while ( 0 != testObj.waitingFlag() ) {
                    bcemt_ThreadUtil::yield();
                }
                if (0 < HIGH_WATER_MARK) {
                    LOOP_ASSERT(i, VB == x.popFront() );
                }

                bcemt_ThreadUtil::join(thread);
            }
            LOOP_ASSERT(i, 0 == da.numAllocations());
            LOOP_ASSERT(i, 0 == ta.numBytesInUse());
            LOOP_ASSERT(i, 0 == ta.numMismatches());
            if (veryVerbose) { P(ta); }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TIMED POP FUNCTIONS IN MULTI-THREAD
        //
        // Concerns:
        //   That the 'timedPopBack' and 'timedPopFront' functions block
        //   properly when an item is not available, and that they time out
        //   properly.
        //
        // Plan:
        //   Create a queue object, pop within a different thread, verify that
        //   thread is waiting and times out, then signal the other thread and
        //   wait for half its timeout (or less, for margin) to push an object,
        //   go to sleep and verify upon waking up that the other thread has
        //   popped the object properly.
        //
        // Testing:
        //   int timedPopBack(T *buffer, const bdet_TimeInterval& timeout);
        //   int timedPopFront(T *buffer, const bdet_TimeInterval& timeout);
        // --------------------------------------------------------------------
        using namespace BCEC_QUEUE_TEST_CASE_3;

        if (verbose) cout
                       << endl
                       << "TESTING 'timedPopBack' and 'timedPopFront'" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\tWith 'timedPopBack'" << endl;
        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            const int T = 1 * DECI_SEC * MICRO_SEC_IN_SEC; // in microseconds
            bdet_TimeInterval T4(4 * DECI_SEC);            // .4s
            bdet_TimeInterval T10(10 * DECI_SEC);          // 1s

            MyBarrier barrier;
            Obj x(&ta);

            Element VA = 1.2;
            TestClass3back testObj(&x, &barrier, T10, VA);

            bcemt_ThreadUtil::Handle thread;
            bcemt_ThreadUtil::create(&thread, test3back, &testObj);

            barrier.wait();
            while ( 0 != testObj.waitingFlag() ) {
                bcemt_ThreadUtil::yield();
            }
            ASSERT(0 != testObj.timeOutFlag());

            barrier.wait();
            bdet_TimeInterval now = bdetu_SystemTime::now();
            barrier.wait();
            bcemt_ThreadUtil::microSleep(T);

            // Already, microSleep could oversleep and prevent to push
            // in time for pop to unblock in the other thread, so we test
            // for security.

            x.pushBack( VA ); // this should unlock the timedPopBack in testObj

            bdet_TimeInterval elapsed = bdetu_SystemTime::now() - now;
            if (elapsed < T4) {
                while ( 0 != testObj.waitingFlag() ) {
                    bcemt_ThreadUtil::yield();
                }
                ASSERT(0 == testObj.timeOutFlag());
            } else {
                bsl::cout
                    << "*** Warning:  (line " << __LINE__ << ") "
                    << "high load delays prevented test"
                    << " case 3 to run properly ('timedPopBack')" << endl;
            }

            bcemt_ThreadUtil::join(thread);
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

        if (verbose) cout << "\tWith 'timedPopFront'" << endl;
        {
            const int T = 1 * DECI_SEC * MICRO_SEC_IN_SEC; // in microseconds
            bdet_TimeInterval T4(4 * DECI_SEC);   // .4s
            bdet_TimeInterval T10(10 * DECI_SEC); // 1s

            MyBarrier barrier;
            Obj x(&ta);

            Element VA = 1.2;
            TestClass3front testObj(&x, &barrier, T10, VA);

            bcemt_ThreadUtil::Handle thread;
            bcemt_ThreadUtil::create(&thread, test3front, &testObj);

            barrier.wait();
            while ( 0 != testObj.waitingFlag() ) {
                bcemt_ThreadUtil::yield();
            }
            ASSERT(0 != testObj.timeOutFlag());

            barrier.wait();
            bdet_TimeInterval now = bdetu_SystemTime::now();
            barrier.wait();
            bcemt_ThreadUtil::microSleep(T);

            // See comment above.

            x.pushFront( VA );

            bdet_TimeInterval elapsed = bdetu_SystemTime::now() - now;
            if (elapsed < T4) {
                while ( 0 != testObj.waitingFlag() ) {
                    bcemt_ThreadUtil::yield();
                }
                ASSERT(0 == testObj.timeOutFlag());
            } else {
                bsl::cout
                    << "*** Warning: (line " << __LINE__ << ") "
                    << "high load delays prevented test"
                    << " case 3 to run properly ('timedPopFront') " << endl;
            }

            bcemt_ThreadUtil::join(thread);
        }
        if (verbose) cout << "\tEnsure wait time on timeout" << endl;
        {
            bdet_TimeInterval T1(1 * DECI_SEC);    // in seconds

            Obj x(&ta);
            TestStruct3 s3;
            s3.d_queue_p = &x;
            s3.d_timeout = T1;

            x.pushBack(1.0);

            bcemt_ThreadUtil::Handle thread;
            bcemt_ThreadUtil::create(&thread, s3);
            bcemt_ThreadUtil::join(thread);

            ASSERT(0 == x.queue().length());    // make sure thread ran
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PUSH AND POP FUNCTIONS IN MULTI-THREAD
        //
        // Concerns:
        //   That the 'popBack' and 'popFront' functions properly block until
        //   an item is available.
        //
        // Plan:
        //   Create a queue object, pop within a different thread, verify that
        //   second thread is waiting, then push an object and verify that
        //   the second thread has correctly popped the object and unblocked.
        //
        // Testing:
        //   T popBack();
        //   T popFront();
        // --------------------------------------------------------------------
        using namespace BCEC_QUEUE_TEST_CASE_2;

        if (verbose) cout << endl
                          << "TESTING 'popBack' and 'popFront'" << endl
                          << "================================" << endl;

        if (verbose) cout << "\tWith 'popBack'" << endl;
        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            const int T = 1 * DECI_SEC * MICRO_SEC_IN_SEC; // in microseconds
            Obj x(&ta);

            Element VA = 1.2;
            TestClass2back testObj(&x, VA);

            bcemt_ThreadUtil::Handle thread;
            bcemt_ThreadUtil::create(&thread, test2back, &testObj);

            // Yielding is not bullet-proof because it does not ENSURE that
            // 'testObj' is blocking on the 'popFront', so we make sure by
            // waiting as long as necessary -- to prevent failure in high loads
            // should the 'pushBack' below execute before 'popFront' in
            // 'testObj'.

            int iter = 100;
            while (0 == testObj.waitingFlag() && 0 < --iter) {
                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(T);
            }

            ASSERT(0 != testObj.waitingFlag());

            x.pushBack( VA );

            bcemt_ThreadUtil::join(thread);
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

        if (verbose) cout << "\tWith 'popFront'" << endl;
        {
            const int T = 1 * DECI_SEC * MICRO_SEC_IN_SEC; // in microseconds
            Obj x(&ta);

            Element VA = 1.2;
            TestClass2front testObj(&x, VA);

            bcemt_ThreadUtil::Handle thread;
            bcemt_ThreadUtil::create(&thread, test2front, &testObj);

            // See note above.

            int iter = 100;
            while (0 == testObj.waitingFlag() && 0 < --iter) {
                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(T);
            }

            ASSERT(0 != testObj.waitingFlag());

            x.pushFront( VA );

            bcemt_ThreadUtil::join(thread);
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise the basic functionality
        //
        // Concerns:
        //   That basic essential functionality is operational for one thread
        //
        // Plan:
        //   Create a queue object using the various constructors, 'pushBack'
        //   three elements and verify that 'popFront'ing them results in same
        //   order, then 'pushBack' three elements again and verify that
        //   'popBack'ing them results in opposite order.  Verify that
        //   'timedPopFront' will time out, then 'pushFront' another item,
        //   verify that 'timedPopFront' pops the item, then 'pushFront'
        //   another item and verify that 'timedPopFront' pops the item.
        //   Finally, verify that one can gain modifiable access to the mutex,
        //   condition variable, and queue, perform same operations and release
        //   the mutex.
        //
        // Testing:
        //   This Test Case exercises basic functionality
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bcema_TestAllocator ta(veryVeryVeryVerbose);
        {
            if (verbose)
                cout << "Exercising default c'tor and basic methods" << endl;

            Obj x1(&ta);
            Element VA = 1.2;
            Element VB =- 5.7;
            Element VC = 1234.99;

            x1.pushBack(VA);
            x1.pushBack(VB);
            x1.pushBack(VC);

            ASSERT(VA == x1.popFront());
            ASSERT(VB == x1.popFront());
            ASSERT(VC == x1.popFront());

            x1.pushFront(VA);
            x1.pushFront(VB);
            x1.pushFront(VC);

            ASSERT(VA == x1.popBack());
            ASSERT(VB == x1.popBack());
            ASSERT(VC == x1.popBack());

            Element front, back;
            int    result;

            result = x1.timedPopFront( &front,
                                 bdetu_SystemTime::now().addMilliseconds(250));
            ASSERT(0 != result);

            x1.pushBack(VA);
            result = x1.timedPopFront( &front,
                                 bdetu_SystemTime::now().addMilliseconds(250));
            ASSERT(0 == result);
            ASSERT(VA == front);

            result = x1.timedPopBack( &back,
                                 bdetu_SystemTime::now().addMilliseconds(250));
            ASSERT(0 != result);

            x1.pushBack(VB);
            result = x1.timedPopBack( &back,
                                 bdetu_SystemTime::now().addMilliseconds(250));
            ASSERT(0 == result);
            ASSERT(VB == back);
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

        {
            if (verbose) cout << "Exercising InitCapacity c'tor" << endl;

            const int INITIAL_CAPACITY = 16;
            const Obj::InitialCapacity NE(INITIAL_CAPACITY);

            Obj x1(NE, &ta);
            Element VA = 1.2;
            Element VB = -5.7;
            Element VC = 1234.99;

            x1.pushBack(VA);
            x1.pushBack(VB);
            x1.pushBack(VC);

            ASSERT(VA == x1.popFront());
            ASSERT(VB == x1.popFront());
            ASSERT(VC == x1.popFront());
        }
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

        {
            if (verbose)
                cout << "Exercising InitCapacity, highWaterMark c'tor" << endl;

            const int INITIAL_CAPACITY = 16;
            const int HIGH_WATER_MARK  = 32;
            const Obj::InitialCapacity NE(INITIAL_CAPACITY);

            Obj x1(NE, HIGH_WATER_MARK, &ta);
            Element VA = 1.2;
            Element VB = -5.7;
            Element VC = 1234.99;

            x1.pushBack(VA);
            x1.pushBack(VB);
            x1.pushBack(VC);

            ASSERT(VA == x1.popFront());
            ASSERT(VB == x1.popFront());
            ASSERT(VC == x1.popFront());
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

        {
            if (verbose)
                cout << "Exercising bdec_Queue c'tor" << endl;

            bdec_Queue<Element> q1(&ta);  const bdec_Queue<Element>& Q1 = q1;

            Obj x1(Q1, &ta);
            Element VA = 1.2;
            Element VB = -5.7;
            Element VC = 1234.99;

            x1.pushBack(VA);
            x1.pushBack(VB);
            x1.pushBack(VC);

            ASSERT(VA == x1.popFront());
            ASSERT(VB == x1.popFront());
            ASSERT(VC == x1.popFront());
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

        {
            if (verbose)
                cout << "Exercising bdec_Queue, highWaterMark c'tor" << endl;

            bdec_Queue<Element> q1(&ta);  const bdec_Queue<Element>& Q1 = q1;
            const int HIGH_WATER_MARK  = 32;

            Obj x1(Q1, HIGH_WATER_MARK, &ta);
            Element VA = 1.2;
            Element VB = -5.7;
            Element VC = 1234.99;

            x1.pushBack(VA);
            x1.pushBack(VB);
            x1.pushBack(VC);

            ASSERT(VA == x1.popFront());
            ASSERT(VB == x1.popFront());
            ASSERT(VC == x1.popFront());
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

        {
            if (verbose)
                cout << "Exercising mutex, condition, and queue access"
                     << endl;

            Element VA = 1.2;
            Element VB = -5.7;
            Element VC = 1234.99;

            bdec_Queue<Element> q1(&ta);  const bdec_Queue<Element>& Q1 = q1;
            q1.pushBack(VA);
            q1.pushBack(VB);
            q1.pushBack(VC);

            Obj x1(Q1, &ta);

            ASSERT(x1.queue() == q1);
            ASSERT(VA == x1.popFront());
            ASSERT(VB == x1.popFront());
            ASSERT(VC == x1.popFront());

            // These two condition variable functions are DEPRECATED, but we
            // still test for their presence.

            bcemt_Condition *xc_deprecated = &x1.condition();
            bcemt_Condition *xi_deprecated = &x1.insertCondition();

            // They should return the same as their replacements, so that it
            // will not be necessary to test the deprecated functions later in
            // [6].

            bcemt_Condition *xe = &x1.notEmptyCondition();
            bcemt_Condition *xf = &x1.notFullCondition();

            ASSERT(xe == xc_deprecated);
            ASSERT(xf == xi_deprecated);
            ASSERT(xe != xf);

            {   // atomic push of three values
                bcemt_LockGuard<bcemt_Mutex> lock(&x1.mutex());

                x1.queue().pushBack(VA);
                x1.queue().pushBack(VB);
                x1.queue().pushBack(VC);
            }

            ASSERT(VA == x1.popFront());
            ASSERT(VB == x1.popFront());
            ASSERT(VC == x1.popFront());
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

        {
            if (verbose)
                cout << "Exercising 'highWaterMark'" << endl;

            const int HIGH_WATER_MARK = 10;
            Obj x(&ta), x2(HIGH_WATER_MARK, &ta), x3(-HIGH_WATER_MARK, &ta);

            ASSERT(-1 == x.highWaterMark());
            ASSERT(HIGH_WATER_MARK == x2.highWaterMark());
            ASSERT(-1 == x3.highWaterMark()); // by normalization
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // STRESS TEST
        //
        // Concerns:
        //   That the queue does not hold up under heavy traffic
        //
        // Plan:
        //   Conduct a test that sends heavy traffic through a bcec_Queue,
        //   with and without a high watermark.
        // --------------------------------------------------------------------

        using namespace BCEC_QUEUE_TEST_CASE_MINUS_1;

        enum {
            CONSUMER_COUNT = 4,
            PRODUCER_COUNT = 2 * CONSUMER_COUNT,
            PRODUCER_ITERATIONS = 100 * 1000
        };

        enum {
            WATERMARK_OFF,
            WATERMARK_ON,
            DONE
        };

        for (int w = WATERMARK_OFF; DONE > w; ++w) {
            bcec_Queue<int> queue(WATERMARK_ON == w ? 1024 : -1);

            Producer prod;
            prod.d_queue = &queue;
            prod.d_iterations = PRODUCER_ITERATIONS;

            Consumer cons;
            cons.d_queue = &queue;

            double startTime = bdetu_SystemTime::now().totalSecondsAsDouble();

            bcemt_ThreadGroup tgroup;

            tgroup.addThreads(cons, CONSUMER_COUNT);
            tgroup.addThreads(prod, PRODUCER_COUNT);

            tgroup.joinAll();

            ASSERT(0 == queue.queue().length());

            if (verbose) {
                cout << "Total seconds = " <<
                               bdetu_SystemTime::now().totalSecondsAsDouble() -
                                                             startTime << endl;
            }
        } // for w
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // STRESS TEST
        //
        // Concerns:
        //   Compare performance of bcec_Queue with that of bcec_FixedQueue
        //
        // Plan:
        //   Perform a benchmark analogous to bcec_FixedQueue test case -4.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "STRESS TEST -2" << endl
                          << "==============" << endl;
        enum {
            NUM_THREADS = 6,
            NUM_ITERATIONS = 1000000
        };

        int numIterations = argc > 2 ? atoi(argv[2]) : NUM_ITERATIONS;
        int numPushers = argc > 3 ? atoi(argv[3]) : NUM_THREADS;
        int numPoppers = argc > 4 ? atoi(argv[4]) : NUM_THREADS;

        if (verbose) cout << endl
                          << "NUM ITERATIONS: " << numIterations << endl
                          << "NUM PUSHERS: " << numPushers << endl
                          << "NUM POPPERS: " << numPoppers << endl;
        seqtst::runtest(numIterations, numPushers, numPoppers);
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // STRESS TEST
        //
        // Concerns:
        //   Compare performance of bcec_Queue with that of bcec_FixedQueue
        //
        // Plan:
        //   Perform a benchmark analogous to bcec_FixedQueue test case -5.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "STRESS TEST -3" << endl
                          << "==============" << endl;
        enum {
            NUM_THREADS = 6,
            NUM_ITERATIONS = 1000000
        };

        int numIterations = argc > 2 ? atoi(argv[2]) : NUM_ITERATIONS;
        int numPushers = argc > 3 ? atoi(argv[3]) : NUM_THREADS;
        int numPoppers = argc > 4 ? atoi(argv[4]) : NUM_THREADS;

        if (verbose) cout << endl
                          << "NUM ITERATIONS: " << numIterations << endl
                          << "NUM PUSHERS: " << numPushers << endl
                          << "NUM POPPERS: " << numPoppers << endl;
        zerotst::runtest(numIterations, numPushers, numPoppers);
      } break;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
