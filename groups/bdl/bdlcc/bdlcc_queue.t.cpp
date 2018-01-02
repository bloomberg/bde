// bdlcc_queue.t.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlcc_queue.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_semaphore.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>
#include <bslmt_threadgroup.h>
#include <bsls_atomic.h>

#include <bdlf_bind.h>
#include <bdlt_currenttime.h>
#include <bdlb_random.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bsls_stopwatch.h>

#include <bsl_algorithm.h>

#include <bsl_cstdlib.h>
#include <bsl_deque.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides a thread-enabled proxy to the 'bdlc::Queue'
// component.  The purpose of this test driver is to assert that each operation
// is properly "hooked up" to its respective 'bdlc::Queue' operation, and that
// the locking mechanisms work as expected when the boundary conditions on
// length and high water mark are reached.  In addition, although all the
// memory allocated is allocated by the underlying 'bdlc::Queue', we want to
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
// [ 1] bdlcc::Queue(bslma::Allocator *basicAllocator = 0);
// [ 1] bdlcc::Queue(int highWaterMark, bslma::Allocator *basicAllocator = 0);
// [ 1] ~bdlcc::Queue();
//
// MANIPULATORS
// [ 2] T popBack();
// [ 2] T popFront();
// [ 4] void pushBack(const T& item);
// [ 4] void pushFront(const T& item);
// [ 3] int timedPopBack(TYPE *, const bsls::TimeInterval &);
// [10] int timedPopBack(TYPE *, const bsls::TimeInterval&);
// [ 3] int timedPopFront(TYPE *, const bsls::TimeInterval &);
// [10] int timedPopFront(TYPE *, const bsls::TimeInterval&);
// [ 5] int timedPushBack(const T& item, const bsls::TimeInterval &timeout);
// [ 5] int timedPushFront(const T& item,  const bsls::TimeInterval &timeout);
// [ 4] void forcePushFront(const T& item);
// [ 7] void removeAll(bsl::vector<T>& buffer);
// [ 9] int tryPopFront(TYPE *);
// [ 9] void tryPopFront(int, vector<TYPE> *);
// [ 9] int tryPopBack(TYPE *);
// [ 9] void tryPopBack(int, vector<TYPE> *);

// ACCESSORS
// [ 4] int highWaterMark() const;
// [ 6] bslmt::Mutex& mutex();
// [ 6] bslmt::Condition& notEmptyCondition();
// [ 6] bslmt::Condition& notFullCondition();
// [ 6] bslmt::Condition& condition();
// [ 6] bslmt::Condition& insertCondition();
// [ 6] bdlc::Queue<T>& queue();
// [10] blocking on queue empty
// [11] blocking on queue full
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] Usage example 2
// [ 9] Usage example 1
// [10] Use of the 'bdlc::Queue' interface example

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                             GLOBAL VARIABLES
// ----------------------------------------------------------------------------

int verbose;
int veryVerbose;
int veryVeryVerbose;
int veryVeryVeryVerbose;

// ============================================================================
//              GLOBAL TYPEDEFS/CONSTANTS/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
typedef double Element;
typedef bdlcc::Queue<Element> Obj;

static const double DECI_SEC      = 0.1;
                                     // 1 decisecond (i.e., 1/10th of a second)

static const int MICRO_SEC_IN_SEC = 100000;
                                         // number of micro seconds in a second

static const int MICRO_DECI_SEC =    10000;
                                       // number of micro seconds in .1 seconds

// ============================================================================
//              SUPPORT CLASSES AND FUNCTIONS USED FOR TESTING
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bslma {

bsl::ostream& operator<<(bsl::ostream& out, const TestAllocator& ta)
{
    ta.print();
    return out;
}
}  // close namespace bslma
}  // close enterprise namespace

Element randElement(int *seed)
{
    enum { k_DIV_BIT  = 1 << 14,
           k_DIV_MASK = k_DIV_BIT - 1 };

    const unsigned int num = bdlb::Random::generate15(seed);
    const unsigned int div = bdlb::Random::generate15(seed);
    const Element      ret = num + ((double) (div & k_DIV_MASK) / k_DIV_BIT);

    return (div & k_DIV_BIT) ? ret : -ret;
}

class MyBarrier {
    // This class is a fast 2-thread barrier implemented with semaphores.
    // Since it can only coordinate two threads, it can use much simpler
    // primitives than the real Barrier component.

    bslmt::Semaphore d_entryGate;
    bslmt::Semaphore d_exitGate;
    bsls::AtomicInt  d_threadWaiting;

  public:

    MyBarrier() : d_threadWaiting(0)
        // c'tor
    {}

    void wait()
        // wait until the other thread has reach the 'MyBarrier'
    {

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

// ============================================================================
//           USAGE use of the 'bdlc::Queue' interface from header
// ----------------------------------------------------------------------------

namespace QUEUE_USE_OF_QUEUE_INTERFACE {

struct MyData
{
    // MyData...
};

bdlcc::Queue<MyData> myWorkQueue;
bdlc::Queue<MyData>& rawQueue   = myWorkQueue.queue();
bslmt::Mutex&        queueMutex = myWorkQueue.mutex();

MyData data1;
MyData data2;
bool   pairFoundFlag = false;

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

}  // close namespace QUEUE_USE_OF_QUEUE_INTERFACE

// ============================================================================
//      USAGE example 1 from header (with assert replaced with ASSERT)
// ----------------------------------------------------------------------------

namespace QUEUE_USAGE_EXAMPLE_1 {

struct my_WorkData;

int getWorkData(my_WorkData *)
    // Dummy implementation of 'getWorkData' function required by the usage
    // example.
{
    static int i = 1;
    return !(++i%100);
}

///Usage
///-----
///Example 1: Simple Thread Pool
///- - - - - - - - - - - - - - -
// The following example demonstrates a typical usage of a 'bdlcc::Queue'.
//
// This 'bdlcc::Queue' is used to communicate between a single "producer"
// thread and multiple "consumer" threads.  The "producer" will push work
// requests onto the queue, and each "consumer" will iteratively take a work
// request from the queue and service the request.  This example shows a
// partial, simplified implementation of the 'bdlmt::ThreadPool' class.  See
// component 'bdlmt_threadpool' for more information.
//
// We begin our example with some utility classes that define a simple "work
// item":
//..
    enum {
        k_MAX_CONSUMER_THREADS = 10
    };

    struct my_WorkData {
        // Work data...
    };

    struct my_WorkRequest {
        enum RequestType {
            e_WORK = 1,
            e_STOP = 2
        };

        RequestType d_type;
        my_WorkData d_data;
        // Work data...
    };
//..
// Next, we provide a simple function to service an individual work item.  The
// details are unimportant for this example.
//..
    void myDoWork(my_WorkData& data)
    {
        // do some stuff...
        (void)data;
    }
//..
// The 'myConsumer' function will pop items off the queue and process them.  As
// discussed above, note that the call to 'queue->popFront()' will block until
// there is an item available on the queue.  This function will be executed in
// multiple threads, so that each thread waits in 'queue->popFront()', and
// 'bdlcc::Queue' guarantees that each thread gets a unique item from the
// queue.
//..
    void myConsumer(bdlcc::Queue<my_WorkRequest> *queue)
    {
        while (1) {
            // 'popFront()' will wait for a 'my_WorkRequest' until available.

            my_WorkRequest item = queue->popFront();
            if (item.d_type == my_WorkRequest::e_STOP) break;
            myDoWork(item.d_data);
        }
    }
//..
// The function below is a callback for 'bslmt::ThreadUtil', which requires a
// "C" signature.  'bslmt::ThreadUtil::create()' expects a pointer to this
// function, and provides that function pointer to the newly created thread.
// The new thread then executes this function.
//
// Since 'bslmt::ThreadUtil::create()' uses the familiar "C" convention of
// passing a 'void' pointer, our function simply casts that pointer to our
// required type ('bdlcc::Queue<my_WorkRequest*> *'), and then delegates to the
// queue-specific function 'myConsumer', above.
//..
    extern "C" void *myConsumerThread(void *queuePtr)
    {
        myConsumer ((bdlcc::Queue<my_WorkRequest> *)queuePtr);
        return queuePtr;
    }
//..
// In this simple example, the 'myProducer' function serves multiple roles: it
// creates the 'bdlcc::Queue', starts out the consumer threads, and then
// produces and queues work items.  When work requests are exhausted, this
// function queues one 'STOP' item for each consumer queue.
//
// When each Consumer thread reads a 'STOP', it terminates its thread-handling
// function.  Note that, although the producer cannot control which thread
// 'pop's a particular work item, it can rely on the knowledge that each
// Consumer thread will read a single 'STOP' item and then terminate.
//
// Finally, the 'myProducer' function "joins" each Consumer thread, which
// ensures that the thread itself will terminate correctly; see the
// 'bslmt_threadutil' component for details.
//..
    void myProducer(int numThreads)
    {
        my_WorkRequest item;
        my_WorkData    workData;

        bdlcc::Queue<my_WorkRequest> queue;

        ASSERT(0 < numThreads && numThreads <= k_MAX_CONSUMER_THREADS);
        bslmt::ThreadUtil::Handle consumerHandles[k_MAX_CONSUMER_THREADS];

        for (int i = 0; i < numThreads; ++i) {
            bslmt::ThreadUtil::create(&consumerHandles[i],
                                      myConsumerThread,
                                      &queue);
        }

        while (!getWorkData(&workData)) {
            item.d_type = my_WorkRequest::e_WORK;
            item.d_data = workData;
            queue.pushBack(item);
        }

        for (int i = 0; i < numThreads; ++i) {
            item.d_type = my_WorkRequest::e_STOP;
            queue.pushBack(item);
        }

        for (int i = 0; i < numThreads; ++i) {
            bslmt::ThreadUtil::join(consumerHandles[i]);
        }
    }
//..

}  // close namespace QUEUE_USAGE_EXAMPLE_1

// ============================================================================
//      USAGE example 2 from header (with assert replaced with ASSERT)
// ----------------------------------------------------------------------------

namespace QUEUE_USAGE_EXAMPLE_2 {

///Example 2: Multi-Threaded Observer
/// - - - - - - - - - - - - - - - - -
// The previous example shows a simple mechanism for distributing work requests
// over multiple threads.  This approach works well for large tasks that can be
// decomposed into discrete, independent tasks that can benefit from parallel
// execution.  Note also that the various threads are synchronized only at the
// end of execution, when the Producer "joins" the various consumer threads.
//
// The simple strategy used in the first example works well for tasks that
// share no state, and are completely independent of one another.  For
// instance, a web server might use a similar strategy to distribute http
// requests across multiple worker threads.
//
// In more complicated examples, it is often necessary or desirable to
// synchronize the separate tasks during execution.  The second example below
// shows a single "Observer" mechanism that receives event notification from
// the various worker threads.
//
// We first create a simple 'my_Event' data type.  Worker threads will use this
// data type to report information about their work.  In our example, we will
// report the "worker Id", the event number, and some arbitrary text.
//
// As with the previous example, class 'my_Event' also contains an 'EventType',
// which is an enumeration which that indicates whether the worker has
// completed all work.  The "Observer" will use this enumerated value to note
// when a Worker thread has completed its work.
//..
    enum {
        k_MAX_CONSUMER_THREADS = 10,
        k_MAX_EVENT_TEXT       = 80
    };

    struct my_Event {
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
// the parameterized 'bdlcc::Queue<TYPE>' object.
//
// In this example, we want to pass an additional data item.  Each worker
// thread is initialized with a unique integer value ("worker Id") that
// identifies that thread.  We create a simple data structure that contains
// both of these values:
//..
    struct my_WorkerData {
        int                     d_workerId;
        bdlcc::Queue<my_Event> *d_queue_p;
    };
//..
// Function 'myWorker' simulates a working thread by enqueuing multiple
// 'my_Event' events during execution.  In a normal application, each
// 'my_Event' structure would likely contain different textual information; for
// the sake of simplicity, our loop uses a constant value for the text field.
//..
    void myWorker(int workerId, bdlcc::Queue<my_Event> *queue)
    {
        const int NEVENTS = 5;
        int evnum;

        for (evnum = 0; evnum < NEVENTS; ++evnum) {
            my_Event ev = {
                my_Event::e_IN_PROGRESS,
                workerId,
                evnum,
                "In-Progress Event"
            };
            queue->pushBack(ev);
        }

        my_Event ev = {
            my_Event::e_TASK_COMPLETE,
            workerId,
            evnum,
            "Task Complete"
        };
        queue->pushBack(ev);
    }
//..
// The callback function invoked by 'bslmt::ThreadUtil::create()' takes the
// traditional 'void' pointer.  The expected data is the composite structure
// 'my_WorkerData'.  The callback function casts the 'void' pointer to the
// application-specific data type and then uses the referenced object to
// construct a call to the 'myWorker' function.
//..
    extern "C" void *myWorkerThread(void *v_worker_p)
    {
        my_WorkerData *worker_p = (my_WorkerData *) v_worker_p;
        myWorker(worker_p->d_workerId, worker_p->d_queue_p);
        return v_worker_p;
    }
//..
// For the sake of simplicity, we will implement the Observer behavior in the
// main thread.  The 'void' function 'myObserver' starts out multiple threads
// running the 'myWorker' function, reads 'my_Event's from the queue, and logs
// all messages in the order of arrival.
//
// As each 'myWorker' thread terminates, it sends a 'e_TASK_COMPLETE' event.
// Upon receiving this event, the 'myObserver' function uses the 'd_workerId'
// to find the relevant thread, and then "joins" that thread.
//
// The 'myObserver' function determines when all tasks have completed simply by
// counting the number of 'e_TASK_COMPLETE' messages received.
//..
    void myObserver()
    {
        const int NTHREADS = 10;
        bdlcc::Queue<my_Event> queue;

        ASSERT(NTHREADS > 0 && NTHREADS <= k_MAX_CONSUMER_THREADS);
        bslmt::ThreadUtil::Handle workerHandles[k_MAX_CONSUMER_THREADS];

        my_WorkerData workerData;
        workerData.d_queue_p = &queue;
        for (int i = 0; i < NTHREADS; ++i) {
            workerData.d_workerId = i;
            bslmt::ThreadUtil::create(&workerHandles[i],
                                      myWorkerThread,
                                      &workerData);
        }
        int nStop = 0;
        while (nStop < NTHREADS) {
            my_Event ev = queue.popFront();
            bsl::cout << "[" << ev.d_workerId    << "] "
                             << ev.d_eventNumber << ". "
                             << ev.d_eventText   << bsl::endl;
            if (my_Event::e_TASK_COMPLETE == ev.d_type) {
                ++nStop;
                bslmt::ThreadUtil::join(workerHandles[ev.d_workerId]);
            }
        }
    }
//..

}  // close namespace QUEUE_USAGE_EXAMPLE_2

// ============================================================================
//                               TEST CASE 12
// ----------------------------------------------------------------------------

namespace QUEUE_TEST_CASE_12 {

class TestPopFront {
    Obj     *d_mX;
    int      d_maxVecSize;
    Element  d_maxVecSizeAt;

  public:
    explicit
    TestPopFront(Obj *mX)
        // c'tor
    {
        d_mX = mX;
        d_maxVecSize = 0;
        d_maxVecSizeAt = 0;
    }
    void operator()()
        //
    {
        int expectedVal = 0;
        Element e;
        vector<Element> v;

        while (expectedVal < 50) {
            if (25 == expectedVal) {
                bslmt::ThreadUtil::microSleep(100 * 1000);
            }
            int sts = d_mX->tryPopFront(&e);
            if (!sts) {
                LOOP2_ASSERT(expectedVal, e, expectedVal++ == e && "popFront");
            }

            v.clear();
            d_mX->tryPopFront(20, &v);
            int s = static_cast<int>(v.size());
            for (int i = 0; i < s; ++i) {
                LOOP2_ASSERT(expectedVal, v[i],
                                          expectedVal++ == v[i] && "popFront");
            }
            if (s > d_maxVecSize) {
                d_maxVecSize = static_cast<int>(v.size());
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
    explicit
    TestPopBack(Obj *mX)
        // c'tor
    {
        d_mX = mX;
        d_maxVecSize = 0;
        d_maxVecSizeAt = 0;
    }

    void operator()()
        //
    {
        int expectedVal = 0;
        Element e;
        vector<Element> v;

        while (expectedVal < 50) {
            if (25 == expectedVal) {
                bslmt::ThreadUtil::microSleep(100 * 1000);
            }
            int sts = d_mX->tryPopBack(&e);
            if (!sts) {
                LOOP2_ASSERT(expectedVal, e, expectedVal++ == e && "popBack");
            }

            v.clear();
            d_mX->tryPopBack(20, &v);
            int s = static_cast<int>(v.size());
            for (int i = 0; i < s; ++i) {
                LOOP2_ASSERT(expectedVal, v[i],
                                           expectedVal++ == v[i] && "popBack");
            }
            if (s > d_maxVecSize) {
                d_maxVecSize = static_cast<int>(v.size());
                d_maxVecSizeAt = expectedVal;
            }
        }

        if (verbose) {
            cout << "PopBack: max vecsize = " << d_maxVecSize <<
                    " at " << d_maxVecSizeAt << endl;
        }
    }
};

}  // close namespace QUEUE_TEST_CASE_12

// ============================================================================
//                               TEST CASE 11
// ----------------------------------------------------------------------------

namespace QUEUE_TEST_CASE_11 {

int exitCode1;
int exitCode2;
int exitCode3;

void *const THREAD_EXIT_1 = &exitCode1;
void *const THREAD_EXIT_2 = &exitCode2;
void *const THREAD_EXIT_3 = &exitCode3;

class TestClass13 {      // this class is a functor passed to thread::create
    Obj                *d_queue;
    bslmt::Barrier     *d_barrier;
    bsls::TimeInterval  d_timeout;

  public:
    static int         s_pushCount;
    enum {
        k_VALID_VAL   = 45,
        k_INVALID_VAL = 46
    };

    TestClass13(Obj *queue, bslmt::Barrier *barrier)
        // c'tor
    {
        d_queue = queue;
        d_barrier = barrier;
        s_pushCount = 0;

        // have everything time out 2 seconds after thread object creation

        d_timeout = bdlt::CurrentTime::now() + bsls::TimeInterval(4.0);
    }

    ~TestClass13()
        // make sure we did not wait until timeout
    {
        ASSERT(bdlt::CurrentTime::now() < d_timeout);
    }

    void operator()()
        // thread function
    {
        int sts;

        for (int i = 0; 6 > i; ++i) {
            bool back = !(i & 1);

            Element e = k_VALID_VAL;

            if (back) {
                ASSERT((sts = d_queue->timedPushBack(e, d_timeout), !sts));
            }
            else {
                ASSERT((sts = d_queue->timedPushFront(e, d_timeout), !sts));
            }
            if (sts) {
                bslmt::ThreadUtil::exit(THREAD_EXIT_1);
            }

            ++s_pushCount;

            ASSERT((sts = d_barrier->timedWait(d_timeout), !sts));
            if (sts) {
                bslmt::ThreadUtil::exit(THREAD_EXIT_2);
            }
        }
        bslmt::ThreadUtil::exit(THREAD_EXIT_3);
    }
};
int TestClass13::s_pushCount;

}  // close namespace QUEUE_TEST_CASE_11

// ============================================================================
//                               TEST CASE 10
// ----------------------------------------------------------------------------

namespace QUEUE_TEST_CASE_10 {

class TestClass12 {      // this class is a functor passed to thread::create
    Obj              *d_queue;
    bslmt::Barrier    *d_barrier;
    bsls::TimeInterval d_timeout;

  public:
    enum {
        k_VALID_VAL = 45,
        k_TERMINATE = 46
    };
    TestClass12(Obj *queue, bslmt::Barrier *barrier)
        // c'tor
    {
        d_queue = queue;
        d_barrier = barrier;

        // have everything time out 4 seconds after thread object creation

        d_timeout = bdlt::CurrentTime::now() + bsls::TimeInterval(4.0);
    }
    ~TestClass12()
        // make sure we did not wait until timeout
    {
        ASSERT(bdlt::CurrentTime::now() < d_timeout);
    }

    void operator()()
        // thread function
    {
        Element e;
        int sts;

        for (bool back = false; true; back = !back) {
            ASSERT((sts = d_barrier->timedWait(d_timeout), !sts));
            if (sts) {
                bslmt::ThreadUtil::exit((void *) 2);
            }

            if (back) {
                ASSERT((sts = d_queue->timedPopBack(&e, d_timeout), !sts));
            }
            else {
                ASSERT((sts = d_queue->timedPopFront(&e, d_timeout), !sts));
            }
            if (sts) {
                bslmt::ThreadUtil::exit((void *) 1);
            }

            if (k_TERMINATE == e) {
                bslmt::ThreadUtil::exit((void *) 0);
            }

            ASSERT(k_VALID_VAL == e);

            sts = d_barrier->timedWait(d_timeout);
            ASSERT(!sts);
            if (sts) {
                bslmt::ThreadUtil::exit((void *) 2);
            }
        }
    }
};

}  // close namespace QUEUE_TEST_CASE_10

// ============================================================================
//                                TEST CASE 6
// ----------------------------------------------------------------------------

namespace QUEUE_TEST_CASE_6 {

class TestClass6 {

    // DATA
    bdlcc::Queue<Element> *d_queue_p;
    MyBarrier       *d_barrier_p;
    bsls::AtomicInt       d_stage;
    Element              d_toBePopped;
    Element              d_toBePushed;

  public:
    // CREATORS
    TestClass6(bdlcc::Queue<Element> *queue,
               MyBarrier             *barrier,
               const Element&         value)
    : d_queue_p(queue)
    , d_barrier_p(barrier)
    , d_stage(0)
    , d_toBePopped(value)
    , d_toBePushed(value)
        // c'tor
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
    int stage() const
        //
    { return d_stage; }
};

extern "C"
void *test6(void *arg)
{
    using namespace QUEUE_TEST_CASE_6;
    TestClass6 *x = (TestClass6*)arg;
    x->callback();
    return 0;
}

}  // close namespace QUEUE_TEST_CASE_6
// ============================================================================
//                                TEST CASE 5
// ----------------------------------------------------------------------------

namespace QUEUE_TEST_CASE_5 {

class TestClass5back {

    // DATA
    bdlcc::Queue<Element> *d_queue_p;
    MyBarrier       *d_barrier_p;
    bsls::TimeInterval    d_timeout;
    bsls::AtomicInt       d_timeoutFlag;
    bsls::AtomicInt       d_waitingFlag;
    Element              d_toBeInserted;

  public:
    // CREATORS
    TestClass5back(bdlcc::Queue<Element> *queue,
                   MyBarrier             *barrier,
                   bsls::TimeInterval     timeout,
                   Element                value)
    : d_queue_p(queue)
    , d_barrier_p(barrier)
    , d_timeout(timeout)
    , d_timeoutFlag(0)
    , d_waitingFlag(1)
    , d_toBeInserted(value)
        //
    {
    }

    // MANIPULATORS
    void callback()
        //
    {
        d_waitingFlag = 1;

        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPushBack(
                                         d_toBeInserted,
                                         bdlt::CurrentTime::now() + d_timeout);
        d_waitingFlag = 0;

        d_barrier_p->wait();
        d_waitingFlag = 1;
        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPushBack(
                                         d_toBeInserted,
                                         bdlt::CurrentTime::now() + d_timeout);
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int timeOutFlag() const
        //
    { return d_timeoutFlag; }
    int waitingFlag() const
        //
    { return d_waitingFlag; }
};

class TestClass5front {

    // DATA
    bdlcc::Queue<Element> *d_queue_p;
    MyBarrier       *d_barrier_p;
    bsls::TimeInterval    d_timeout;
    bsls::AtomicInt       d_timeoutFlag;
    bsls::AtomicInt       d_waitingFlag;
    Element              d_toBeInserted;

  public:
    // CREATORS
    TestClass5front(bdlcc::Queue<Element> *queue,
                    MyBarrier             *barrier,
                    bsls::TimeInterval     timeout,
                    Element                value)
    : d_queue_p(queue)
    , d_barrier_p(barrier)
    , d_timeout(timeout)
    , d_timeoutFlag(0)
    , d_waitingFlag(1)
    , d_toBeInserted(value)
        //
    {
    }

    // MANIPULATORS
    void callback()
        //
    {
        d_waitingFlag = 1;

        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPushFront(
                                         d_toBeInserted,
                                         bdlt::CurrentTime::now() + d_timeout);
        d_waitingFlag = 0;

        d_barrier_p->wait();
        d_waitingFlag = 1;
        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPushFront(
                                         d_toBeInserted,
                                         bdlt::CurrentTime::now() + d_timeout);
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int timeOutFlag() const { return d_timeoutFlag; }
    int waitingFlag() const { return d_waitingFlag; }
};

extern "C"
void *test5back(void *arg)
{
    using namespace QUEUE_TEST_CASE_5;
    TestClass5back *x = (TestClass5back*)arg;
    x->callback();
    return 0;
}

extern "C"
void *test5front(void *arg)
{
    using namespace QUEUE_TEST_CASE_5;
    TestClass5front *x = (TestClass5front*)arg;
    x->callback();
    return 0;
}

}  // close namespace QUEUE_TEST_CASE_5
// ============================================================================
//                                TEST CASE 4
// ----------------------------------------------------------------------------

namespace QUEUE_TEST_CASE_4 {

class TestClass4back {

    // DATA
    bdlcc::Queue<Element> *d_queue_p;
    bsls::AtomicInt       d_waitingFlag;
    Element              d_toBeInserted;

  public:
    // CREATORS
    TestClass4back(bdlcc::Queue<Element> *queue, const Element& value)
    : d_queue_p(queue)
    , d_waitingFlag(0)
    , d_toBeInserted(value)
        //
    {
    }

    // MANIPULATORS
    void callback()
        //
    {
        d_waitingFlag = 1;
        d_queue_p->pushBack(d_toBeInserted);
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int waitingFlag()
        //
    {
        return d_waitingFlag;
    }
};

class TestClass4front {

    // DATA
    bdlcc::Queue<Element> *d_queue_p;
    bsls::AtomicInt       d_waitingFlag;
    Element              d_toBeInserted;

  public:
    // CREATORS
    TestClass4front(bdlcc::Queue<Element> *queue, const Element& value)
    : d_queue_p(queue)
    , d_waitingFlag(0)
    , d_toBeInserted(value)
        // c'tor
    {
    }

    // MANIPULATORS
    void callback()
        //
    {
        d_waitingFlag = 1;
        d_queue_p->pushFront(d_toBeInserted);
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int waitingFlag()
        //
    {
        return d_waitingFlag;
    }
};

extern "C"
void *test4back(void *arg)
{
    using namespace QUEUE_TEST_CASE_4;
    TestClass4back *x = (TestClass4back*)arg;
    x->callback();
    return 0;
}

extern "C"
void *test4front(void *arg)
{
    using namespace QUEUE_TEST_CASE_4;
    TestClass4front *x = (TestClass4front*)arg;
    x->callback();
    return 0;
}

}  // close namespace QUEUE_TEST_CASE_4
// ============================================================================
//                                TEST CASE 3
// ----------------------------------------------------------------------------

namespace QUEUE_TEST_CASE_3 {

class TestClass3back {
    // DATA
    bdlcc::Queue<Element> *d_queue_p;
    MyBarrier       *d_barrier_p;
    bsls::TimeInterval    d_timeout;
    bsls::AtomicInt       d_timeoutFlag;
    bsls::AtomicInt       d_waitingFlag;
    Element              d_expected;

  public:
    // CREATORS
    TestClass3back(bdlcc::Queue<Element> *queue,
                   MyBarrier             *barrier,
                   bsls::TimeInterval     timeout,
                   Element                val)
    : d_queue_p(queue)
    , d_barrier_p(barrier)
    , d_timeout(timeout)
    , d_timeoutFlag(0)
    , d_waitingFlag(1)
    , d_expected(val)
        // c'tor
    {
    }

    // MANIPULATORS
    void callback()
        //
    {
        Element result;

        d_waitingFlag = 1;
        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPopBack(
                                         &result,
                                         bdlt::CurrentTime::now() + d_timeout);
        d_waitingFlag = 0;

        d_barrier_p->wait();
        d_waitingFlag = 1;
        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPopBack(
                                         &result,
                                         bdlt::CurrentTime::now() + d_timeout);
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
    bdlcc::Queue<Element> *d_queue_p;
    MyBarrier       *d_barrier_p;
    bsls::TimeInterval    d_timeout;
    bsls::AtomicInt       d_timeoutFlag;
    bsls::AtomicInt       d_waitingFlag;
    Element              d_expected;

  public:
    // CREATORS
    TestClass3front(bdlcc::Queue<Element> *queue,
                    MyBarrier             *barrier,
                    bsls::TimeInterval     timeout,
                    Element                value)
    : d_queue_p(queue)
    , d_barrier_p(barrier)
    , d_timeout(timeout)
    , d_timeoutFlag(0)
    , d_waitingFlag(1)
    , d_expected(value)
        //
    {
    }

    // MANIPULATORS
    void callback()
        //
    {
        Element result;

        d_waitingFlag = 1;
        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPopFront(
                                         &result,
                                         bdlt::CurrentTime::now() + d_timeout);
        d_waitingFlag = 0;

        d_barrier_p->wait();
        d_waitingFlag = 1;
        d_barrier_p->wait();
        d_timeoutFlag = d_queue_p->timedPopFront(
                                         &result,
                                         bdlt::CurrentTime::now() + d_timeout);
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
    bdlcc::Queue<Element> *d_queue_p;
    bsls::TimeInterval    d_timeout;

    void operator()()
        //
    {
        Element result;

        d_queue_p->removeAll();

        bsls::TimeInterval start = bdlt::CurrentTime::now();
        int sts = d_queue_p->timedPopFront(&result,
                                            start + d_timeout);
        bsls::TimeInterval end = bdlt::CurrentTime::now();
        ASSERT(0 != sts);
        ASSERT(end >= start + d_timeout);

        start = bdlt::CurrentTime::now();
        sts = d_queue_p->timedPopBack(&result,
                                       start + d_timeout);
        end = bdlt::CurrentTime::now();
        ASSERT(0 != sts);
        ASSERT(end >= start + d_timeout);
    }
};

}  // close namespace QUEUE_TEST_CASE_3

extern "C" {
    void *test3back(void *arg)
    {
        using namespace QUEUE_TEST_CASE_3;
        TestClass3back *x = (TestClass3back*)arg;
        x->callback();
        return 0;
    }

    void *test3front(void *arg)
    {
        using namespace QUEUE_TEST_CASE_3;
        TestClass3front *x = (TestClass3front*)arg;
        x->callback();
        return 0;
    }
} // extern "C"

// ============================================================================
//                                TEST CASE 2
// ----------------------------------------------------------------------------

namespace QUEUE_TEST_CASE_2 {

class TestClass2back {

    // DATA
    bdlcc::Queue<Element> *d_queue_p;
    bsls::AtomicInt       d_waitingFlag;
    Element              d_expected;

  public:
    // CREATORS
    TestClass2back(bdlcc::Queue<Element> *queue, const Element& value)
    : d_queue_p(queue)
    , d_waitingFlag(0)
    , d_expected(value)
        // c'tor
    {
    }

    // MANIPULATORS
    void callback()
        // manipulate the flags and pop from the queue back
    {
        d_waitingFlag = 1;
        ASSERT(d_expected == d_queue_p->popBack());
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int waitingFlag()
        // reveal waiting flag
    {
        return d_waitingFlag;
    }
};

class TestClass2front {
    // DATA
    bdlcc::Queue<Element> *d_queue_p;
    bsls::AtomicInt       d_waitingFlag;
    Element              d_expected;

  public:
    // CREATORS
    TestClass2front(bdlcc::Queue<Element> *queue, const Element& value)
    : d_queue_p(queue)
    , d_waitingFlag(0)
    , d_expected(value)
        // c'tor
    {
    }

    // MANIPULATORS
    void callback()
        // manipulate the flags and pop from the queue front
    {
        d_waitingFlag = 1;
        ASSERT(d_expected == d_queue_p->popFront());
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int waitingFlag()
        // reveal waiting flag
    {
        return d_waitingFlag;
    }
};

extern "C"
void *test2back(void *arg)
{
    using namespace QUEUE_TEST_CASE_2;
    TestClass2back *x = (TestClass2back*)arg;
    x->callback();
    return 0;
}

extern "C"
void *test2front(void *arg)
{
    using namespace QUEUE_TEST_CASE_2;
    TestClass2front *x = (TestClass2front*)arg;
    x->callback();
    return 0;
}

}  // close namespace QUEUE_TEST_CASE_2

// ============================================================================
//                                TEST CASE 2
// ----------------------------------------------------------------------------

namespace QUEUE_TEST_CASE_MINUS_1 {

bslmt::Mutex outputMutex;

struct Producer {
    bdlcc::Queue<int>    *d_queue;
    int                 d_iterations;

    void operator()() const
        //
    {
        for (int i = 0; i < d_iterations; ++i) {
            d_queue->pushBack(i);
        }

        if (verbose) {
            bslmt::LockGuard<bslmt::Mutex> guard(&outputMutex);

            cout << "Producer finishing\n";
        }

        d_queue->pushBack(-1);
    }
};

struct Consumer {
    bdlcc::Queue<int>    *d_queue;

    void operator()() const
        //
    {
        int terminatorsFound = 0;
        int val;
        do {
            val = d_queue->popFront();
            if (-1 == val) {
                ++terminatorsFound;
            }
        } while (terminatorsFound < 2);

        if (verbose) {
            bslmt::LockGuard<bslmt::Mutex> guard(&outputMutex);

            cout << "Consumer finishing\n";
        }
    }
};

}  // close namespace QUEUE_TEST_CASE_MINUS_1

// ============================================================================
//                         SEQUENCE CONSTRAINT TEST
// ----------------------------------------------------------------------------

namespace seqtst {

struct Item {
    int                   d_threadId;
    int                   d_sequenceNum;
    bsl::function<void()> d_funct;
};

struct Control {
    bslmt::Barrier     *d_barrier;

    bdlcc::Queue<Item> *d_queue;

    int                 d_numExpectedPushers;
    int                 d_iterations;

    bsls::AtomicInt     d_numPushers;
    bsls::AtomicInt     d_numPopped;
};

void f(const bsl::shared_ptr<int>&)
{
}

void pusherThread(Control *control)
{
    bsl::shared_ptr<int> sp;
    sp.createInplace(bslma::Default::allocator(), 12345);

    bsl::function<void()> funct = bdlf::BindUtil::bind(&f, sp);

    bdlcc::Queue<Item> *queue = control->d_queue;

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

    bdlcc::Queue<Item> *queue = control->d_queue;

    int totalToPop = control->d_numExpectedPushers * control->d_iterations;

    control->d_barrier->wait();

    int numPopped;
    while ((numPopped = control->d_numPopped++) < totalToPop) {

        Item item = queue->popFront();

        ASSERT(seq[item.d_threadId] < item.d_sequenceNum);
        seq[item.d_threadId] = item.d_sequenceNum;
    }
}

void runtest(int numIterations, int numPushers, int numPoppers)
{
    enum {
        k_QUEUE_SIZE = 2047
    };

    bdlcc::Queue<Item> queue(k_QUEUE_SIZE);

    bslmt::Barrier barrier(numPushers + numPoppers);

    Control control;

    control.d_numExpectedPushers = numPushers;
    control.d_iterations = numIterations;
    control.d_queue = &queue;

    control.d_numPopped = 0;
    control.d_numPushers = 0;

    control.d_barrier = &barrier;

    bslmt::ThreadGroup tg;
    tg.addThreads(bdlf::BindUtil::bind(&pusherThread,&control),
            numPushers);
    tg.addThreads(bdlf::BindUtil::bind(&popperThread,&control),
            numPoppers);

    tg.joinAll();
}
}  // close namespace seqtst

namespace seqtst2 {

struct Item {
    int                   d_threadId;
    int                   d_sequenceNum;
    bsl::function<void()> d_funct;
};

struct Control {
    bslmt::Barrier     *d_barrier;

    bdlcc::Queue<Item> *d_queue;

    int                 d_numExpectedPushers;
    int                 d_iterations;

    bsls::AtomicInt     d_numPushers;
    bsls::AtomicInt     d_numPopped;
};

void f(const bsl::shared_ptr<int>&)
{
}

void pusherThread(Control *control)
{
    bsl::shared_ptr<int> sp;
    sp.createInplace(bslma::Default::allocator(), 12345);

    bsl::function<void()> funct = bdlf::BindUtil::bind(&f, sp);

    bdlcc::Queue<Item> *queue = control->d_queue;

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

    bdlcc::Queue<Item> *queue = control->d_queue;

    int totalToPop = control->d_numExpectedPushers * control->d_iterations;

    control->d_barrier->wait();

    int numPopped;
    while ((numPopped = control->d_numPopped++) < totalToPop) {

        Item item;
        queue->popFront(&item);

        ASSERT(seq[item.d_threadId] < item.d_sequenceNum);
        seq[item.d_threadId] = item.d_sequenceNum;
    }
}

void runtest(int numIterations, int numPushers, int numPoppers)
{
    enum {
        k_QUEUE_SIZE = 2047
    };

    bdlcc::Queue<Item> queue(k_QUEUE_SIZE);

    bslmt::Barrier barrier(numPushers + numPoppers);

    Control control;

    control.d_numExpectedPushers = numPushers;
    control.d_iterations = numIterations;
    control.d_queue = &queue;

    control.d_numPopped = 0;
    control.d_numPushers = 0;

    control.d_barrier = &barrier;

    bslmt::ThreadGroup tg;
    tg.addThreads(bdlf::BindUtil::bind(&pusherThread,&control),
            numPushers);
    tg.addThreads(bdlf::BindUtil::bind(&popperThread,&control),
            numPoppers);

    tg.joinAll();
}
}  // close namespace seqtst2

namespace seqtst3 {

struct Item {
    int                   d_threadId;
    int                   d_sequenceNum;
    bsl::function<void()> d_funct;
};

struct Control {
    bslmt::Barrier     *d_barrier;

    bdlcc::Queue<Item> *d_queue;

    int                 d_numExpectedPushers;
    int                 d_iterations;

    bsls::AtomicInt     d_numPushers;
    bsls::AtomicInt     d_numPopped;
};

void f(const bsl::shared_ptr<int>&)
{
}

void pusherThread(Control *control)
{
    bsl::shared_ptr<int> sp;
    sp.createInplace(bslma::Default::allocator(), 12345);

    bsl::function<void()> funct = bdlf::BindUtil::bind(&f, sp);

    bdlcc::Queue<Item> *queue = control->d_queue;

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

    bdlcc::Queue<Item> *queue = control->d_queue;

    int totalToPop = control->d_numExpectedPushers * control->d_iterations;

    control->d_barrier->wait();

    int numPopped;
    while ((numPopped = control->d_numPopped++) < totalToPop) {

        Item item;
        queue->popBack(&item);

        ASSERT(seq[item.d_threadId] < item.d_sequenceNum);
        seq[item.d_threadId] = item.d_sequenceNum;
    }
}

void runtest(int numIterations, int numPushers, int numPoppers)
{
    enum {
        k_QUEUE_SIZE = 2047
    };

    bdlcc::Queue<Item> queue(k_QUEUE_SIZE);

    bslmt::Barrier barrier(numPushers + numPoppers);

    Control control;

    control.d_numExpectedPushers = numPushers;
    control.d_iterations = numIterations;
    control.d_queue = &queue;

    control.d_numPopped = 0;
    control.d_numPushers = 0;

    control.d_barrier = &barrier;

    bslmt::ThreadGroup tg;
    tg.addThreads(bdlf::BindUtil::bind(&pusherThread,&control),
            numPushers);
    tg.addThreads(bdlf::BindUtil::bind(&popperThread,&control),
            numPoppers);

    tg.joinAll();
}
}  // close namespace seqtst3

// ============================================================================
//                             ZERO POINTER TEST
// ----------------------------------------------------------------------------

namespace zerotst {

struct Control {
    bslmt::Barrier       *d_barrier;
    bdlcc::Queue<void *> *d_queue;
    int                   d_numExpectedPushers;
    int                   d_iterations;
    bsls::AtomicInt       d_numPushers;
    bsls::AtomicInt       d_numPopped;
};

void pusherThread(Control *control)
{
    bdlcc::Queue<void *> *queue = control->d_queue;

    control->d_numPushers++;

    control->d_barrier->wait();

    for (int i=0; i<control->d_iterations; i++) {
        queue->pushBack(0);
    }
}

void popperThread(Control *control)
{
    bdlcc::Queue<void *> *queue = control->d_queue;

    int totalToPop = control->d_numExpectedPushers * control->d_iterations;

    control->d_barrier->wait();

    int numPopped;
    while ((numPopped = control->d_numPopped++) < totalToPop) {
        queue->popFront();
    }
}

void runtest(int numIterations, int numPushers, int numPoppers)
{
    enum {
        k_QUEUE_SIZE = 2047
    };

    bdlcc::Queue<void *> queue(k_QUEUE_SIZE);

    bslmt::Barrier barrier(numPushers + numPoppers);

    Control control;

    control.d_numExpectedPushers = numPushers;
    control.d_iterations = numIterations;
    control.d_queue = &queue;

    control.d_numPopped = 0;
    control.d_numPushers = 0;

    control.d_barrier = &barrier;

    bslmt::ThreadGroup tg;
    tg.addThreads(bdlf::BindUtil::bind(&pusherThread,&control),
            numPushers);
    tg.addThreads(bdlf::BindUtil::bind(&popperThread,&control),
            numPoppers);

    tg.joinAll();
}
}  // close namespace zerotst

namespace TEST_CASE_2 {

int myLength(bdlcc::Queue<Element> *q)
{
    const bdlcc::Queue<Element>& Q = *q;

    int ret = Q.length();

    ASSERT(ret == q->queue().length());

    return ret;
}

}  // close namespace TEST_CASE_2

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&da);

    switch (test) { case 0:  // Zero is always the leading case.
      case 20: {
        // ---------------------------------------------------------
        // TESTING sequence constraints using 'backwards'
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "sequence constraint test 'backwars'" << endl
                          << "===================================" << endl;
        enum {
            k_NUM_THREADS = 4,
            k_NUM_ITERATIONS = 5000
        };

        for (int numPushers=1; numPushers<=k_NUM_THREADS; numPushers++) {
        for (int numPoppers=1; numPoppers<=k_NUM_THREADS; numPoppers++) {

            seqtst3::runtest(k_NUM_ITERATIONS, numPushers, numPoppers);
        }
        }

      } break;

      case 19: {
        // ---------------------------------------------------------
        // TESTING sequence constraints using popFront(TYPE*)
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "sequence constraint test using popFront(TYPE*)"
                          << endl
                          << "========================"
                          << endl;
        enum {
            k_NUM_THREADS = 4,
            k_NUM_ITERATIONS = 5000
        };

        for (int numPushers=1; numPushers<=k_NUM_THREADS; numPushers++) {
        for (int numPoppers=1; numPoppers<=k_NUM_THREADS; numPoppers++) {

            seqtst2::runtest(k_NUM_ITERATIONS, numPushers, numPoppers);
        }
        }

      } break;

      case 18: {
        // --------------------------------------------------------------------
        //  Basic test for popFront(TYPE*) and popBack(TYPE*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing popFront(TYPE*) and popBack(TYPE*)"
                          << "=========================================="
                          << endl;

        bslma::TestAllocator ta(veryVeryVeryVerbose);
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
      case 17: {
        // ---------------------------------------------------------
        // TESTING queue of zero pointer
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "zero ptr test" << endl
                          << "========================" << endl;
        enum {
            k_NUM_THREADS = 4,
            k_NUM_ITERATIONS = 5000
        };

        for (int numPushers=1; numPushers<=k_NUM_THREADS; numPushers++) {
        for (int numPoppers=1; numPoppers<=k_NUM_THREADS; numPoppers++) {

            zerotst::runtest(k_NUM_ITERATIONS, numPushers, numPoppers);
        }
        }

      } break;

      case 16: {
        // ---------------------------------------------------------
        // TESTING sequence constraints
        // ---------------------------------------------------------

        if (verbose) cout << endl
                          << "sequence constraint test" << endl
                          << "========================" << endl;
        enum {
            k_NUM_THREADS = 4,
            k_NUM_ITERATIONS = 5000
        };

        for (int numPushers=1; numPushers<=k_NUM_THREADS; numPushers++) {
        for (int numPoppers=1; numPoppers<=k_NUM_THREADS; numPoppers++) {

            seqtst::runtest(k_NUM_ITERATIONS, numPushers, numPoppers);
        }
        }

      } break;

      case 15: {
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

        using namespace QUEUE_USAGE_EXAMPLE_1;

        if (verbose) cout << endl
                          << "Testing USAGE example 1" << endl
                          << "=======================" << endl;
        {
            const int NTHREADS = 10;
            myProducer(NTHREADS);
        }
      } break;
      case 14: {
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

        using namespace QUEUE_USAGE_EXAMPLE_2;

        if (verbose) cout << endl
                          << "Testing USAGE example 2" << endl
                          << "=======================" << endl;
        {
            const int NTHREADS = 10;
            bdlcc::Queue<my_Event> queue;

            ASSERT(0 < NTHREADS && NTHREADS <= k_MAX_CONSUMER_THREADS);
            bslmt::ThreadUtil::Handle workerHandles[k_MAX_CONSUMER_THREADS];

            my_WorkerData wdata[NTHREADS];
            for (int i=0; i < NTHREADS; ++i) {
                wdata[i].d_workerId = i;
                wdata[i].d_queue_p = &queue;
                bslmt::ThreadUtil::create(&workerHandles[i],
                                         myWorkerThread,
                                         &wdata[i]);
            }
            int n_Stop = 0;
            while (n_Stop < NTHREADS) {
                my_Event ev = queue.popFront();
                if (verbose) {
                    cout << "[" << ev.d_workerId << "] "
                         << ev.d_eventNumber << ". "
                         << ev.d_eventText << endl;
                }
                if (my_Event::e_TASK_COMPLETE == ev.d_type) {
                    ++n_Stop;
                    bslmt::ThreadUtil::join(workerHandles[ev.d_workerId]);
                }
            }
        }
      } break;
      case 13: {
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

        using namespace QUEUE_TEST_CASE_12;

        Element waitTable[] = { 4, 5, 6, 7, 15, 20, 30, 31, 32, 33, 34 };
        int waitTableLength = static_cast<int>(sizeof waitTable
                                             / sizeof *waitTable);

        const Element *waitTableBegin = waitTable;
        const Element *waitTableEnd   = waitTable + waitTableLength;

        Obj mX;
        bslmt::ThreadUtil::Handle handle;

        for (int run = 0; run < 4; ++run) {
            bslmt::ThreadUtil::create(&handle, TestPopFront(&mX));
            bslmt::ThreadUtil::microSleep(10 * 1000);
            for (Element e = 0; e < 50; ++e) {
                if (waitTableEnd != bsl::find(waitTableBegin, waitTableEnd,
                                                                          e)) {
                    if (veryVerbose) {
                        cout << "PopFront: wait at " << e << endl;
                    }
                    bslmt::ThreadUtil::microSleep(10 * 1000);
                }
                mX.pushBack(e);
            }
            bslmt::ThreadUtil::join(handle);

            bslmt::ThreadUtil::create(&handle, TestPopBack(&mX));
            for (Element e = 0; e < 50; ++e) {
                if (waitTableEnd != bsl::find(waitTableBegin, waitTableEnd,
                                                                          e)) {
                    if (veryVerbose) {
                        cout << "PopBack: wait at " << e << endl;
                    }
                    bslmt::ThreadUtil::microSleep(10 * 1000);
                }
                mX.pushFront(e);
            }
            bslmt::ThreadUtil::join(handle);
        }
      } break;
      case 12: {
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

        using namespace QUEUE_TEST_CASE_11;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(4, &ta);
        vector<Element> v;
        bslmt::ThreadUtil::Handle handle;
        bslmt::Barrier barrier(2);
        bsls::TimeInterval timeout =
                            bdlt::CurrentTime::now() + bsls::TimeInterval(4.0);

        ASSERT(bdlt::CurrentTime::now() < timeout);

        ASSERT(0 == mX.length());

        TestClass13 tc13(&mX, &barrier);

        bslmt::ThreadUtil::create(&handle, tc13);

        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));

        bslmt::ThreadUtil::yield();
        bslmt::ThreadUtil::microSleep(50*1000);        // 50 mSec

        ASSERT(4 == tc13.s_pushCount);
        ASSERT(4 == mX.length());  // 5th push is blocking on high watermark

        ASSERT(TestClass13::k_VALID_VAL == mX.popFront());
        ASSERT(!barrier.timedWait(timeout));
        bslmt::ThreadUtil::yield();
        bslmt::ThreadUtil::microSleep(50*1000);        // 50 mSec
        ASSERT(5 == tc13.s_pushCount);
        ASSERT(4 == mX.length());

        ASSERT(TestClass13::k_VALID_VAL == mX.popBack());
        ASSERT(!barrier.timedWait(timeout));
        bslmt::ThreadUtil::yield();
        bslmt::ThreadUtil::microSleep(50*1000);        // 50 mSec
        ASSERT(6 == tc13.s_pushCount);
        ASSERT(4 == mX.length());

        for (int i = 0; 4 > i; ++i) {
            bool back = !(1 & i);
            if (back) {
                ASSERT(TestClass13::k_VALID_VAL == mX.popBack());
            }
            else {
                ASSERT(TestClass13::k_VALID_VAL == mX.popFront());
            }
            ASSERT(6 == tc13.s_pushCount);
            ASSERT(3 - i == mX.length());
        }

        {
            void *sts = 0;
            bslmt::ThreadUtil::join(handle, &sts);
            LOOP_ASSERT(sts, THREAD_EXIT_3 == sts);
        }

        ASSERT(bdlt::CurrentTime::now() < timeout);
      }  break;
      case 11: {
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
        //   int timedPopBack(TYPE *, const bsls::TimeInterval&);
        //   int timedPopFront(TYPE *, const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        using namespace QUEUE_TEST_CASE_10;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(&ta);
        Element e = TestClass12::k_VALID_VAL;
        vector<Element> v;
        bslmt::ThreadUtil::Handle handle;
        bslmt::Barrier barrier(2);

        // Note microSleeps on Solaris can arbitrarily take as long as 2 sec,
        // so have a pessimistic timeout time -- normally this will take MUCH
        // less than 9 seconds.

        bsls::TimeInterval timeout = bdlt::CurrentTime::now()
                                   + bsls::TimeInterval(9.0);

        ASSERT(bdlt::CurrentTime::now() < timeout);

        ASSERT(0 == mX.length());

        TestClass12 tc12(&mX, &barrier);

        bslmt::ThreadUtil::create(&handle, tc12);

        mX.pushFront(e);
        mX.pushBack(e);

        ASSERT(2 == mX.length());

        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));

        ASSERT(0 == mX.length());

        for (int i = 0; i < 4; ++i) {
            enum { k_SLEEP_TIME = 10 * 1000 };        // 10 mSec

            ASSERT(!barrier.timedWait(timeout));
            bslmt::ThreadUtil::yield();
            bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);
            ASSERT(0 == mX.length());

            mX.pushBack(e);
            ASSERT(1 >= mX.length());

            ASSERT(!barrier.timedWait(timeout));
            ASSERT(0 == mX.length());
        }

        e = TestClass12::k_TERMINATE;
        mX.pushFront(e);
        ASSERT(!barrier.timedWait(timeout));

        {
            void *sts;
            bslmt::ThreadUtil::join(handle, &sts);
            LOOP_ASSERT(sts, !sts);
        }

        ASSERT(bdlt::CurrentTime::now() < timeout);
      }  break;
      case 10: {
        // --------------------------------------------------------------------
        // TEST 'tryPopFront', 'tryPopBack' -- SINGLE THREAD TEST
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

        ASSERT(!X.length());

        e = -7;
        sts = mX.tryPopFront(&e);
        ASSERT(0 != sts);
        ASSERT(-7 == e);
        mX.tryPopFront(100, &v);
        ASSERT(v.empty());
        mX.tryPopFront(100);

        ASSERT(!X.length());

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

        ASSERT(!X.length());

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

        ASSERT(!X.length());

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

        ASSERT(!X.length());
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TEST USE OF THE QUEUE INTERFACE
        //
        // Concern:
        //   That the example use of the 'bdlc::Queue' interface code
        //   from the header works as expected.
        //
        // Plan:
        //   Insert to elements on a queue, run the code from the example
        //   section and assert that the two values have been removed properly.
        //
        // Testing:
        //   USAGE use of the 'bdlc::Queue' interface
        // --------------------------------------------------------------------

        using namespace QUEUE_USE_OF_QUEUE_INTERFACE;

        {
            myWorkQueue.pushBack( MyData() );
            myWorkQueue.pushBack( MyData() );
            myWork();
            ASSERT(0 == rawQueue.length());
            ASSERT(0 == myWorkQueue.length());
        }

      } break;
      case 8: {
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

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            Obj mX(&ta);

            mX.pushBack(VA);
            mX.pushBack(VB);
            mX.pushBack(VC);

            mX.removeAll();
            ASSERT(0 == mX.length());
        }

        {
            Obj mX(&ta);
            bsl::vector<Element> buffer(&ta);

            mX.pushBack(VA);
            mX.pushBack(VB);
            mX.pushBack(VC);

            mX.removeAll(&buffer);
            ASSERT(0 == mX.length());
            ASSERT(3 == buffer.size());
            ASSERT(VA == buffer[0]);
            ASSERT(VB == buffer[1]);
            ASSERT(VC == buffer[2]);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING INTERNAL QUEUE ACCESSORS
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
        //   bslmt::Mutex& mutex();
        //   bslmt::Condition& notEmptyCondition();
        //   bslmt::Condition& notFullCondition();
        //   bdlc::Queue<T>& queue();
        // --------------------------------------------------------------------

        using namespace QUEUE_TEST_CASE_6;

        if (verbose)
            cout << endl <<
              "\nTESTING 'mutex', 'notEmptyCondition', 'notFullCondition'"
                                                              " and 'queue'\n"
                "========================================================"
                                                              "============\n";

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            const int HIGH_WATER_MARK = 2;
            const int T = 1 * MICRO_DECI_SEC;
            bsls::TimeInterval T4(2 * DECI_SEC);

            Element VA = 1.2;
            Element VB = -5.7;
            Element VC = 1234.99;

            Obj x(HIGH_WATER_MARK, &ta);
            MyBarrier barrier;
            TestClass6 testObj(&x, &barrier, VA);

            bslmt::ThreadUtil::Handle thread;
            bslmt::ThreadUtil::create(&thread, test6, &testObj);

            ASSERT(0 == testObj.stage());
            {
                bslmt::LockGuard<bslmt::Mutex> lock(&x.mutex());
                barrier.wait();
                bslmt::ThreadUtil::yield();

                // This is not bullet-proof because it does not ENSURE that
                // testObj is blocking on the 'pushback', so we maximize our
                // chances by waiting more -- to prevent failure in high loads
                // should the mutex destruction below execute before stage 2 of
                // testObj (which unfortunately happens erratically).

                int iter = 1000;
                while (0 < --iter && testObj.stage() < 1) {
                    bslmt::ThreadUtil::yield();
                    bslmt::ThreadUtil::microSleep(10 * 1000);
                }

                // either testObj.stage() has changed or we have waited 10 sec

                ASSERT(1 == testObj.stage());
            }

            // LockGuard destruction unlocks x.mutex().

            barrier.wait();

            // Unfortunately, there is no way to test if the test thread is
            // waiting on the condition variable.  So we just give it a chance
            // to unblock - it should continue to block.  This is not
            // bullet-proof because it does not ENSURE that testObj is blocking
            // on the popFront, so we maximize our chances by waiting some
            // more.

            int iter = 10;
            while (0 < --iter) {
                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(T);
            }

            {
                bslmt::LockGuard<bslmt::Mutex> lock(&x.mutex());
                x.queue().pushBack(VA);
                bslmt::ThreadUtil::microSleep(T);
                ASSERT(2 == testObj.stage());
            }
            bslmt::ThreadUtil::microSleep(T);
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
                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(T);
            }

            ASSERT(3 == testObj.stage());
            {
                bslmt::LockGuard<bslmt::Mutex> lock(&x.mutex());
                ASSERT(VC == x.queue().back());
                x.queue().popFront();
                bslmt::ThreadUtil::microSleep(T);
                ASSERT(3 == testObj.stage());
            }
            bslmt::ThreadUtil::microSleep(T);
            ASSERT(3 == testObj.stage());

            // Make testObj stop waiting on 'pushBack'.

            x.notFullCondition().signal();
            barrier.wait();

            ASSERT(VC == x.popFront());
            ASSERT(VA == x.popFront());

            bslmt::ThreadUtil::join(thread);
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }
      } break;
      case 6: {
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

        using namespace QUEUE_TEST_CASE_5;

        if (verbose)
            cout << endl
                 << "TESTING 'timedPushBack' and 'timedPushFront'" << endl
                 << "============================================" << endl;

        static  const struct {
            int d_lineNum;
            int d_highWaterMark;
            int d_insertions;
        } VALUES[] = {
            // line high-water mark insertions
            // ----     ---------------    ----------
            { L_ ,                   -1,           10 },
         // { L_ , 0, 0 }, // undefined behavior
            { L_ ,                    1,            1 },
            { L_ ,                    3,            3 },
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        const int T = 1 * MICRO_DECI_SEC; // in microseconds
        bsls::TimeInterval T4(4 * DECI_SEC);            // .4s
        bsls::TimeInterval T10(10 * DECI_SEC);          // 1s

        Element VA = 1.2;
        Element VB = -5.7;

        if (verbose) cout << "\tWith 'timedPushBack'" << endl;
        for (int i = 0; i< NUM_VALUES; ++i)
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
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

                bslmt::ThreadUtil::Handle thread;
                bslmt::ThreadUtil::create(&thread, test5back, &testObj);

                barrier.wait();
                while ( 0 != testObj.waitingFlag() ) {
                    bslmt::ThreadUtil::yield();
                }
                if (-1 != HIGH_WATER_MARK) {
                    LOOP_ASSERT(i, 0 != testObj.timeOutFlag() );
                } else {
                    LOOP_ASSERT(i, 0 == testObj.timeOutFlag() );
                }

                barrier.wait();
                if (-1 != HIGH_WATER_MARK) {
                    bsls::TimeInterval now = bdlt::CurrentTime::now();
                    barrier.wait();
                    bslmt::ThreadUtil::microSleep(T);

                    bsls::TimeInterval elapsed =
                                                bdlt::CurrentTime::now() - now;
                    if (elapsed < T4) {
                        LOOP_ASSERT(i, 0 != testObj.waitingFlag() );
                        LOOP_ASSERT(i, VA == x.popBack() );

                        while ( 0 != testObj.waitingFlag() ) {
                            bslmt::ThreadUtil::yield();
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
                        bslmt::ThreadUtil::yield();
                    }
                    LOOP_ASSERT(i, 0 == testObj.timeOutFlag() );
                }

                bslmt::ThreadUtil::join(thread);
            }
            LOOP_ASSERT(i, 0 == da.numAllocations());
            LOOP_ASSERT(i, 0 == ta.numBytesInUse());
            LOOP_ASSERT(i, 0 == ta.numMismatches());
            if (veryVerbose) { P(ta); }
        }

        if (verbose) cout << "\tWith 'timedPushFront'" << endl;
        for (int i = 0; i< NUM_VALUES; ++i)
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
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

                bslmt::ThreadUtil::Handle thread;
                bslmt::ThreadUtil::create(&thread, test5front, &testObj);

                barrier.wait();
                while ( 0 != testObj.waitingFlag() ) {
                    bslmt::ThreadUtil::yield();
                }
                if (-1 != HIGH_WATER_MARK) {
                    LOOP_ASSERT(i, 0 != testObj.timeOutFlag() );
                } else {
                    LOOP_ASSERT(i, 0 == testObj.timeOutFlag() );
                }

                barrier.wait();
                if (-1 != HIGH_WATER_MARK) {
                    bsls::TimeInterval now = bdlt::CurrentTime::now();
                    barrier.wait();
                    bslmt::ThreadUtil::microSleep(T);

                    bsls::TimeInterval elapsed =
                                                bdlt::CurrentTime::now() - now;
                    if (elapsed < T4) {
                        LOOP_ASSERT(i, 0 != testObj.waitingFlag() );
                        LOOP_ASSERT(i, VA == x.popFront() );

                        while ( 0 != testObj.waitingFlag() ) {
                            bslmt::ThreadUtil::yield();
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
                        bslmt::ThreadUtil::yield();
                    }
                    LOOP_ASSERT(i, 0 == testObj.timeOutFlag() );
                }

                bslmt::ThreadUtil::join(thread);
            }
            LOOP_ASSERT(i, 0 == da.numAllocations());
            LOOP_ASSERT(i, 0 == ta.numBytesInUse());
            LOOP_ASSERT(i, 0 == ta.numMismatches());
            if (veryVerbose) { P(ta); }
        }

      } break;
      case 5: {
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
        //   bdlcc::Queue(int highWaterMark,
        //              bslma::Allocator *basicAllocator = 0);
        //   int highWaterMark() const;
        //   void pushBack(const T& item);
        //   void pushFront(const T& item);
        //   void forcePushFront(const T& item);
        // --------------------------------------------------------------------

        using namespace QUEUE_TEST_CASE_4;

        if (verbose) cout << endl
           << "TESTING 'highWaterMark' and associated c'tor along with" << endl
           << "'pushBack' and 'pushFront'" << endl
           << "======================================================="
           << "==========================" << endl;

        static  const struct {
            int d_lineNum;
            int d_highWaterMark;
        } VALUES[] = {
            // line high-water mark
            // ----     ---------------
            { L_ ,                   -1 },
         // { L_ , 0 }, // undefined behavior
            { L_ ,                    1 },
            { L_ ,                    2 },
            { L_ ,                   10 },
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        const int T = 1 * MICRO_DECI_SEC; // in microseconds

        const Element VA = 1.2;
        const Element VB = -5.7;

        if (verbose) cout << "\tWith 'pushBack'" << endl;
        for (int i = 0; i< NUM_VALUES; ++i)
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
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

                bslmt::ThreadUtil::Handle thread;
                bslmt::ThreadUtil::create(&thread, test4back, &testObj);

                if (0 < HIGH_WATER_MARK) {
                    // Yielding is not bullet-proof because it does not ENSURE
                    // that testObj is blocking on the 'pushFront', so we make
                    // sure by waiting as long as necessary -- to prevent
                    // failure in high loads should the 'popBack' below execute
                    // before 'pushFront' in testObj (which actually happens
                    // erratically).

                    int iter = 100;
                    while (0 == testObj.waitingFlag() && 0 < --iter) {
                        bslmt::ThreadUtil::yield();
                        bslmt::ThreadUtil::microSleep(T);
                    }

                    LOOP_ASSERT(i, 0 != testObj.waitingFlag() );
                    LOOP_ASSERT(i, VA == x.popBack() );
                }

                while ( 0 != testObj.waitingFlag()){
                    bslmt::ThreadUtil::yield();
                }
                if (0 < HIGH_WATER_MARK) {
                    LOOP_ASSERT(i, VB == x.popBack() );
                }

                bslmt::ThreadUtil::join(thread);
            }
            LOOP_ASSERT(i, 0 == da.numAllocations());
            LOOP_ASSERT(i, 0 == ta.numBytesInUse());
            LOOP_ASSERT(i, 0 == ta.numMismatches());
            if (veryVerbose) { P(ta); }
        }

        if (verbose) cout << "\tWith 'push_front'" << endl;
        for (int i = 0; i< NUM_VALUES; ++i)
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
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

                bslmt::ThreadUtil::Handle thread;
                bslmt::ThreadUtil::create(&thread, test4front, &testObj);

                if (0 < HIGH_WATER_MARK) {
                    // See comment above.

                    int iter = 100;
                    while (0 == testObj.waitingFlag() && 0 < --iter) {
                        bslmt::ThreadUtil::yield();
                        bslmt::ThreadUtil::microSleep(T);
                    }

                    LOOP_ASSERT(i, 0 != testObj.waitingFlag() );
                    LOOP_ASSERT(i, VA == x.popFront() );
                }

                while ( 0 != testObj.waitingFlag() ) {
                    bslmt::ThreadUtil::yield();
                }
                if (0 < HIGH_WATER_MARK) {
                    LOOP_ASSERT(i, VB == x.popFront() );
                }

                bslmt::ThreadUtil::join(thread);
            }
            LOOP_ASSERT(i, 0 == da.numAllocations());
            LOOP_ASSERT(i, 0 == ta.numBytesInUse());
            LOOP_ASSERT(i, 0 == ta.numMismatches());
            if (veryVerbose) { P(ta); }
        }

      } break;
      case 4: {
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
        //   int timedPopBack(T *buffer, const bsls::TimeInterval& timeout);
        //   int timedPopFront(T *buffer, const bsls::TimeInterval& timeout);
        // --------------------------------------------------------------------

        using namespace QUEUE_TEST_CASE_3;

        if (verbose) cout
                       << endl
                       << "TESTING 'timedPopBack' and 'timedPopFront'" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\tWith 'timedPopBack'" << endl;
        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            const int T = 1 * MICRO_DECI_SEC; // in microseconds
            bsls::TimeInterval T4(4 * DECI_SEC);            // .4s
            bsls::TimeInterval T10(10 * DECI_SEC);          // 1s

            MyBarrier barrier;
            Obj x(&ta);

            Element VA = 1.2;
            TestClass3back testObj(&x, &barrier, T10, VA);

            bslmt::ThreadUtil::Handle thread;
            bslmt::ThreadUtil::create(&thread, test3back, &testObj);

            barrier.wait();
            while ( 0 != testObj.waitingFlag() ) {
                bslmt::ThreadUtil::yield();
            }
            ASSERT(0 != testObj.timeOutFlag());

            barrier.wait();
            bsls::TimeInterval now = bdlt::CurrentTime::now();
            barrier.wait();
            bslmt::ThreadUtil::microSleep(T);

            // Already, microSleep could oversleep and prevent to push in time
            // for pop to unblock in the other thread, so we test for security.

            x.pushBack( VA ); // this should unlock the timedPopBack in testObj

            bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
            if (elapsed < T4) {
                while ( 0 != testObj.waitingFlag() ) {
                    bslmt::ThreadUtil::yield();
                }
                ASSERT(0 == testObj.timeOutFlag());
            } else {
                bsl::cout
                    << "*** Warning:  (line " << __LINE__ << ") "
                    << "high load delays prevented test"
                    << " case 3 to run properly ('timedPopBack')" << endl;
            }

            bslmt::ThreadUtil::join(thread);
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

        if (verbose) cout << "\tWith 'timedPopFront'" << endl;
        {
            const int T = 1 * MICRO_DECI_SEC; // in microseconds
            bsls::TimeInterval T4(4 * DECI_SEC);   // .4s
            bsls::TimeInterval T10(10 * DECI_SEC); // 1s

            MyBarrier barrier;
            Obj x(&ta);

            Element VA = 1.2;
            TestClass3front testObj(&x, &barrier, T10, VA);

            bslmt::ThreadUtil::Handle thread;
            bslmt::ThreadUtil::create(&thread, test3front, &testObj);

            barrier.wait();
            while ( 0 != testObj.waitingFlag() ) {
                bslmt::ThreadUtil::yield();
            }
            ASSERT(0 != testObj.timeOutFlag());

            barrier.wait();
            bsls::TimeInterval now = bdlt::CurrentTime::now();
            barrier.wait();
            bslmt::ThreadUtil::microSleep(T);

            // See comment above.

            x.pushFront( VA );

            bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
            if (elapsed < T4) {
                while ( 0 != testObj.waitingFlag() ) {
                    bslmt::ThreadUtil::yield();
                }
                ASSERT(0 == testObj.timeOutFlag());
            } else {
                bsl::cout
                    << "*** Warning: (line " << __LINE__ << ") "
                    << "high load delays prevented test"
                    << " case 3 to run properly ('timedPopFront') " << endl;
            }

            bslmt::ThreadUtil::join(thread);
        }
        if (verbose) cout << "\tEnsure wait time on timeout" << endl;
        {
            bsls::TimeInterval T1(1 * DECI_SEC);    // in seconds

            Obj x(&ta);
            TestStruct3 s3;
            s3.d_queue_p = &x;
            s3.d_timeout = T1;

            x.pushBack(1.0);

            bslmt::ThreadUtil::Handle thread;
            bslmt::ThreadUtil::create(&thread, s3);
            bslmt::ThreadUtil::join(thread);

            ASSERT(0 == x.length());    // make sure thread ran
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

      } break;
      case 3: {
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

        using namespace QUEUE_TEST_CASE_2;

        if (verbose) cout << endl
                          << "TESTING 'popBack' and 'popFront'" << endl
                          << "================================" << endl;

        if (verbose) cout << "\tWith 'popBack'" << endl;
        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            const int T = 1 * MICRO_DECI_SEC; // in microseconds
            Obj x(&ta);

            Element VA = 1.2;
            TestClass2back testObj(&x, VA);

            bslmt::ThreadUtil::Handle thread;
            bslmt::ThreadUtil::create(&thread, test2back, &testObj);

            // Yielding is not bullet-proof because it does not ENSURE that
            // 'testObj' is blocking on the 'popFront', so we make sure by
            // waiting as long as necessary -- to prevent failure in high loads
            // should the 'pushBack' below execute before 'popFront' in
            // 'testObj'.

            int iter = 100;
            while (0 == testObj.waitingFlag() && 0 < --iter) {
                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(T);
            }

            ASSERT(0 != testObj.waitingFlag());

            x.pushBack( VA );

            bslmt::ThreadUtil::join(thread);
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

        if (verbose) cout << "\tWith 'popFront'" << endl;
        {
            const int T = 1 * MICRO_DECI_SEC; // in microseconds
            Obj x(&ta);

            Element VA = 1.2;
            TestClass2front testObj(&x, VA);

            bslmt::ThreadUtil::Handle thread;
            bslmt::ThreadUtil::create(&thread, test2front, &testObj);

            // See note above.

            int iter = 100;
            while (0 == testObj.waitingFlag() && 0 < --iter) {
                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(T);
            }

            ASSERT(0 != testObj.waitingFlag());

            x.pushFront( VA );

            bslmt::ThreadUtil::join(thread);
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // SINGLE_THREADED TESTING PUSHES, POPS, AND LENGTH
        //
        // Concerns:
        //: 1 That for a 'bdlcc::Queue' 'q', 'q.length() == q.queue().length()'
        //:   always.  This is verified by the 'myLength' function.  Since
        //:   it is usually impossible to test manipulators without calling
        //:   accessors, or accessors without calling manipulators, this
        //:   testing will take place while the manipulators are being tested
        //:   rather than in a separate section.
        //:
        //: 2 That 'pushBack' has the same effect on the queue as calling
        //:   'pushBack on the underlying 'bdlc::Queue'.
        //:
        //: 3 That 'popBack' calls 'popBack' on the underlying 'bdlc::Queue',
        //:   except it also returns the popped value.
        //:
        //: 4 That 'pushFront' has the same effect on the queue as calling
        //:   'pushFront on the underlying 'bdlc::Queue'.
        //:
        //: 5 That 'popFront' calls 'popFront' on the underlying 'bdlc::Queue',
        //:   except it also returns the popped value.
        //
        // Plan:
        //: 1 Testing 'pushBack', 'popBack', 'pushFront', and 'popFront':
        //:
        //:   o Push a couple of different values into 'x', the queue, with
        //:     'pushBack', monitoring the length of the queue with 'myLength'
        //:     and monitoring the contents of the queue with
        //:     'x.queue().back()' and 'x.queue().front()'. C-2, C-1.
        //:
        //:   o Pop the two values from the 'x', the queue, using 'popBack',
        //:     observing that the correct values are returned, and monitoring
        //:     the length of the queue with 'myLength' and monitoring the
        //:     contents of the queue with 'x.queue().back()' and
        //:     'x.queue().front()'. C-3, C-1.
        //:
        //:   o Push a couple of new, different values into 'x', the queue,
        //:     with 'pushFront', monitoring the length of the queue with
        //:     'myLength' and monitoring the contents of the queue with
        //:     'x.queue().back()' and 'x.queue().front()'. C-4, C-1.
        //:
        //:   o Pop the two values from the 'x', the queue, using 'popFront',
        //:     observing that the correct values are returned, and monitoring
        //:     the length of the queue with 'myLength' and monitoring the
        //:     contents of the queue with 'x.queue().back()' and
        //:     'x.queue().front()'. C-5, C-1.
        //:
        //: 2 Iterate, randomly choosing a queue length in the range 0-7.  This
        //    test tests C-1, C-2, C-3, C-4, and C-5, just more thoroughly.
        //:
        //:   o If the chosen length is longer than the existing queue length,
        //:     grow the queue to the desired queue length by random choosing
        //:     'pushFront' or 'pushBack', and pushing random doubles into the
        //:     queue.  Simultaneously push the same value onto the same end of
        //:     a 'bsl::deque' kept in parallel.
        //;
        //:   o If the chosen length is shorter than the existing queue, shrink
        //:     the queue to the designated queue length by randomly calling
        //:     'popFront' or 'popBack'.  Simultaneously do a similar pop from
        //:     the parallel 'bsl::deque', and observe the values popped are
        //:     identical.
        //;
        //:   o Each iteration, whether growing or shrinking, frequently check
        //:     the length is as expected, via 'myLength', but also verify it
        //:     matches the length of the 'deque'. check the length of the
        //:     deque, and call '.queue().length()' on the queue and observe
        //:     that all three lengths match with the expected value.
        //
        // Testing:
        //   pushFront
        //   pushBack
        //   popFront
        //   popBack
        //   length
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING PUSH, POP, and LENGTH\n"
                             "=============================\n";

        using namespace TEST_CASE_2;

        bsls::Stopwatch sw;
        sw.start();

        int seed = 123456789;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        if (verbose) cout << "\t1. Explicit Pushes and Pops\n";
        {
            enum { k_NUM_V = 4 };
            Element v[k_NUM_V];    const Element *V = &v[0];
            for (int ti = 0; ti < k_NUM_V; ++ti) {
                v[ti] = randElement(&seed);
                for (int tj = 0; tj < ti; ++tj) {
                    ASSERT(V[tj] != V[ti]);
                }
                if (veryVerbose) { T_ T_ P_(ti); P(V[ti]); }
            }

            Obj x(&ta);    const Obj& X = x;

            if (verbose) cout << "\t\t'pushBack' && 'length'\n";
            {
                ASSERT(0 == myLength(&x));

                x.pushBack(V[0]);

                ASSERT(1 == myLength(&x));
                ASSERT(V[0] == x.queue().back());

                x.pushBack(V[1]);

                ASSERT(2 == myLength(&x));
                ASSERT(V[0] == x.queue().front());
                ASSERT(V[1] == x.queue().back());
            }

            if (verbose) cout << "\t\t'popBack' && 'length'\n";
            {
                ASSERT(2 == myLength(&x));

                ASSERT(V[1] == x.popBack());

                ASSERT(1 == myLength(&x));
                ASSERT(V[0] == x.queue().front());
                ASSERT(V[0] == x.queue().back());

                ASSERT(V[0] == x.popBack());

                ASSERT(0 == myLength(&x));
            }

            if (verbose) cout << "\t\t'pushFront' && 'length'\n";
            {
                ASSERT(0 == myLength(&x));

                x.pushFront(V[2]);

                ASSERT(1 == myLength(&x));
                ASSERT(V[2] == x.queue().back());

                x.pushFront(V[3]);

                ASSERT(2 == myLength(&x));
                ASSERT(V[3] == x.queue().front());
                ASSERT(V[2] == x.queue().back());
            }

            if (verbose) cout << "\t\t'popFront' && 'length'\n";
            {
                ASSERT(2 == myLength(&x));

                ASSERT(V[3] == x.popFront());

                ASSERT(1 == myLength(&x));
                ASSERT(V[2] == x.queue().front());
                ASSERT(V[2] == x.queue().back());

                ASSERT(V[2] == x.popFront());

                ASSERT(0 == myLength(&x));
            }
        }

        ASSERT(0 == ta.numBytesInUse());

        if (verbose) cout << "\t2. Random pushes and pops\n";
        {
            Obj x(&ta);                     const Obj& X = x;
            bsl::deque<Element> d(&ta);     const bsl::deque<Element>& D = d;

            ASSERT(0 == D.size());
            ASSERT(0 == X.length());
            ASSERT(x.queue().length() == X.length());

            int expectedLength = 0;
            const int ITERATIONS = veryVeryVerbose ? 50 : 5000;

            for (int i = 0; i < ITERATIONS; ++i) {
                int ll;
                do {
                    ll = bdlb::Random::generate15(&seed) % 8;
                } while (expectedLength == ll);
                const int LENGTH = ll;

                if (expectedLength < LENGTH) {
                    while (expectedLength < LENGTH) {
                        ASSERT(expectedLength == (int) D.size());
                        ASSERT(expectedLength == myLength(&x));

                        // Generate a fairly random double using 'generate15'.

                        const Element v = randElement(&seed);

                        if (bdlb::Random::generate15(&seed) & 0x80) {
                            x.pushBack(v);
                            d.push_back(v);
                            if (veryVerbose) {
                                T_ T_ P_(i) P_(LENGTH) P_(expectedLength);
                                cout << "\tPUB: " << v << endl;
                            }
                        }
                        else {
                            x.pushFront(v);
                            d.push_front(v);
                            if (veryVerbose) {
                                T_ T_ P_(i) P_(LENGTH) P_(expectedLength);
                                cout << "\tPUF: " << v << endl;
                            }
                        }

                        ++expectedLength;

                        ASSERT(expectedLength == (int) D.size());
                        ASSERT(expectedLength == myLength(&x));
                    }
                }
                else {
                    while (expectedLength > LENGTH) {
                        ASSERT(expectedLength == (int) D.size());
                        ASSERT(expectedLength == myLength(&x));

                        if (bdlb::Random::generate15(&seed) & 0x80) {
                            const Element popped = D.back();
                            d.pop_back();
                            ASSERT(popped == x.popBack());
                            if (veryVerbose) {
                                T_ T_ P_(i) P_(LENGTH) P_(expectedLength);
                                cout << "\tPOB: " << popped << endl;
                            }
                        }
                        else {
                            const Element popped = D.front();
                            d.pop_front();
                            ASSERT(popped == x.popFront());
                            if (veryVerbose) {
                                T_ T_ P_(i) P_(LENGTH) P_(expectedLength);
                                cout << "\tPOF: " << popped << endl;
                            }
                        }

                        --expectedLength;

                        ASSERT(expectedLength == (int) D.size());
                        ASSERT(expectedLength == myLength(&x));
                    }
                }

                ASSERT(LENGTH == expectedLength);
            }
        }
        sw.stop();

        if (verbose) P(sw.accumulatedWallTime());
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

        bslma::TestAllocator ta(veryVeryVeryVerbose);
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

            result = x1.timedPopFront(
                        &front, bdlt::CurrentTime::now().addMilliseconds(250));
            ASSERT(0 != result);

            x1.pushBack(VA);
            result = x1.timedPopFront(
                        &front, bdlt::CurrentTime::now().addMilliseconds(250));
            ASSERT(0 == result);
            ASSERT(VA == front);

            result = x1.timedPopBack(
                         &back, bdlt::CurrentTime::now().addMilliseconds(250));
            ASSERT(0 != result);

            x1.pushBack(VB);
            result = x1.timedPopBack(
                         &back, bdlt::CurrentTime::now().addMilliseconds(250));
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
                cout << "Exercising bdlc::Queue c'tor" << endl;

            bdlc::Queue<Element> q1(&ta);  const bdlc::Queue<Element>& Q1 = q1;

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
                cout << "Exercising bdlc::Queue, highWaterMark c'tor" << endl;

            bdlc::Queue<Element> q1(&ta);  const bdlc::Queue<Element>& Q1 = q1;
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

            bdlc::Queue<Element> q1(&ta);  const bdlc::Queue<Element>& Q1 = q1;
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

            bslmt::Condition *xc_deprecated = &x1.condition();
            bslmt::Condition *xi_deprecated = &x1.insertCondition();

            // They should return the same as their replacements, so that it
            // will not be necessary to test the deprecated functions later in
            // [6].

            bslmt::Condition *xe = &x1.notEmptyCondition();
            bslmt::Condition *xf = &x1.notFullCondition();

            ASSERT(xe == xc_deprecated);
            ASSERT(xf == xi_deprecated);
            ASSERT(xe != xf);

            {   // atomic push of three values
                bslmt::LockGuard<bslmt::Mutex> lock(&x1.mutex());

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
        //   Conduct a test that sends heavy traffic through a bdlcc::Queue,
        //   with and without a high watermark.
        // --------------------------------------------------------------------

        using namespace QUEUE_TEST_CASE_MINUS_1;

        enum {
            k_CONSUMER_COUNT = 4,
            k_PRODUCER_COUNT = 2 * k_CONSUMER_COUNT,
            k_PRODUCER_ITERATIONS = 100 * 1000
        };

        enum {
            k_WATERMARK_OFF,
            k_WATERMARK_ON,
            k_DONE
        };

        for (int w = k_WATERMARK_OFF; k_DONE > w; ++w) {
            bdlcc::Queue<int> queue(k_WATERMARK_ON == w ? 1024 : -1);

            Producer prod;
            prod.d_queue = &queue;
            prod.d_iterations = k_PRODUCER_ITERATIONS;

            Consumer cons;
            cons.d_queue = &queue;

            double startTime = bdlt::CurrentTime::now().totalSecondsAsDouble();

            bslmt::ThreadGroup tgroup;

            tgroup.addThreads(cons, k_CONSUMER_COUNT);
            tgroup.addThreads(prod, k_PRODUCER_COUNT);

            tgroup.joinAll();

            ASSERT(0 == queue.length());

            if (verbose) {
                cout << "Total seconds = "
                     << bdlt::CurrentTime::now().totalSecondsAsDouble() -
                                                             startTime << endl;
            }
        } // for w
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // STRESS TEST
        //
        // Concerns:
        //   Compare performance of bdlcc::Queue with that of bdlcc::FixedQueue
        //
        // Plan:
        //   Perform a benchmark analogous to bdlcc::FixedQueue test case -4.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "STRESS TEST -2" << endl
                          << "==============" << endl;
        enum {
            k_NUM_THREADS = 6,
            k_NUM_ITERATIONS = 1000000
        };

        int numIterations = argc > 2 ? atoi(argv[2]) : k_NUM_ITERATIONS;
        int numPushers = argc > 3 ? atoi(argv[3]) : k_NUM_THREADS;
        int numPoppers = argc > 4 ? atoi(argv[4]) : k_NUM_THREADS;

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
        //   Compare performance of bdlcc::Queue with that of bdlcc::FixedQueue
        //
        // Plan:
        //   Perform a benchmark analogous to bdlcc::FixedQueue test case -5.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "STRESS TEST -3" << endl
                          << "==============" << endl;
        enum {
            k_NUM_THREADS = 6,
            k_NUM_ITERATIONS = 1000000
        };

        int numIterations = argc > 2 ? atoi(argv[2]) : k_NUM_ITERATIONS;
        int numPushers = argc > 3 ? atoi(argv[3]) : k_NUM_THREADS;
        int numPoppers = argc > 4 ? atoi(argv[4]) : k_NUM_THREADS;

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
