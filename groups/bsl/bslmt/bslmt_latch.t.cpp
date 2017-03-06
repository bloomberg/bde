// bslmt_latch.t.cpp                                                  -*-C++-*-
#include <bslmt_latch.h>

#include <bslmt_barrier.h>        // for testing only
#include <bslmt_lockguard.h>
#include <bslmt_threadgroup.h>    // for testing only
#include <bslmt_threadutil.h>     // for testing only

#include <bslim_testutil.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_spinlock.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>

#include <bsl_algorithm.h>   // 'bsl::min'
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_list.h>        // for usage example
#include <bsl_vector.h>      // for usage example

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// We begin by testing that a latch can be constructed and destroyed, and that
// the non-blocking methods, 'arrive', 'countDown', and 'arriveAndWait' (on a
// 'bslmt::Latch(1)'), are indeed non-blocking.  Also, a 'wait()' is tested on
// a 'bslmt::Latch(0)'; indeed, in this case the synchronization point is
// already reached.
//
// Some "non-blocking" smoke tests are performed by multiple threads.
//
// Once we have established that non-blocking methods are indeed permitting the
// calling thread to proceed we can test three groups of threads: some acting
// as Producers, some acting as Consumers, and some acting as both Producers
// and Consumers.
//
//: o Producer Threads call:         'arrive()' or 'countDown(n)'
//:
//: o Consumer Threads call:         'wait()'
//:
//: o ProducerConsumer Threads call: 'arriveAndWait()'
//
// In order to test that not a single thread in a "consumer group" passes the
// latch before the synchronization point is reached the following strategy is
// performed:
//
// All threads will share the latch under test and an "atomic int".
//
//: o Producer threads perform 'x += 1' on the shared 'int' *before* the
//:   'arrive()' or the 'countDown(1)'.
//:
//: o Consumer threads perform 'x *= 2' on the shared 'int' *after* the
//:   'wait()'.
//:
//: o ProducerConsumer threads perform 'x += 1' on the shared 'int' *before*
//:   the 'arriveAndWait()' and 'x *= 2' *after* it.
//
// After the threads execution the expected result is:
//..
//  # producers * 2 ** # consumers)
//..
// If the latch doesn't provide the expected "isolation" and an 'x += 1' and an
// 'x *= 2' are mixed the result will be not the expected one.
//
// Note that a ProducerConsumer thread counts toward the expected result
// formula as one producer and one consumer.
//
// In order to increase the concurrency on the performed operation all threads
// block on a barrier initialized with a value that is the sum of all launched
// threads.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] Latch(int count);
// [ 1] ~Latch();
//
// MANIPULATORS (breathing tests, non-concurrent usage)
// [ 2] void arrive();
// [ 4] void arriveAndWait();
// [ 1] void countDown(int n);
// [ 5] void wait();
// [ 6] void timedWait(const bsls::TimeInterval &timeout);
//
// ACCESSORS
// [ 1] int currentCount() const;
// [ 3] bool tryWait() const;
//
// Interactions between manipulators, concurrent usage:
// [13] countDown(int n); wait(); arriveAndWait();
// [12] arrive();         wait(); arriveAndWait();
// [11] countDown(int n); wait();
// [10] arrive();         wait();
// [ 9] arriveAndWait();
// [ 8] countDown(int n);
// [ 7] arrive();
// ----------------------------------------------------------------------------
// [14] USAGE EXAMPLE

// The following table shows how the three groups of threads described are
// mixed together in each test case, where:
//
//: o A  threads issue: x += 1;  arrive();
//:
//: o CD threads issue: x += 1;  countDown(1);
//:
//: o W  threads issue:          wait();           x *= 2;
//:
//: o AW threads issue: x += 1;  arriveAndWait();  x *= 2;
//
//     | C | C | C | C | C | C | C |
//     | A | A | A | A | A | A | A |
//     | S | S | S | S | S | S | S |
//     | E | E | E | E | E | E | E |
//     |   |   |   |   |   |   |   |
//     | 6 | 7 | 8 | 9 | 10| 11| 12|
// ---------------------------------
//  A  | x | - | - | x | - | x | - |
// ---------------------------------
//  CD | - | x | - | - | x | - | x |
// ---------------------------------
//  W  | - | - | - | x | x | x | x |
// ---------------------------------
//  AW | - | - | x | - | - | x | x |
// ---------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::Latch Obj;

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

bool verbose             = false;
bool veryVerbose         = false;
bool veryVeryVerbose     = false;
bool veryVeryVeryVerbose = false;

class Thread {
    // This class facilitates the launching of a thread.  Derived classes must
    // implement the (pure) 'virtual' 'mainLoop' function.  See the contract
    // of 'mainLoop' below.

  private:
    // NOT IMPLEMENTED
    Thread(const Thread&);
    Thread& operator=(const Thread&);

  public:
    // CREATORS
    Thread()
        // Create a 'Thread' object.
    {
    }

    virtual ~Thread()
        // Destroy this object.  The behavior is undefined if the thread is
        // still running.
    {
    }

    // MANIPULATORS
    void callable()
        // Invoke the ('virtual') 'mainLoop' method repeatedly until 'mainLoop'
        // returns a non-zero value.  Note that this method serves as the entry
        // point for this 'Thread' object.  Also note that this method prints
        // the exit status of 'mainLoop' to 'bsl::cout' in 'veryVeryVerbose'
        // mode.
    {
        int rc = -1;

        while (0 == (rc = mainLoop())) {
            // empty
        }

        if (veryVeryVerbose) {
            if (rc < 0) {
                cout << "Thread terminated with error: " << rc << endl;
            }
            else {
                cout << "Thread terminated w/o error: "  << rc << endl;
            }
        }
    }

    virtual int mainLoop() = 0;
        // If this method returns a value that is:
        //:   0 - 'mainLoop' can be called again.
        //:
        //: > 0 - The function completed with no error and the exit code is the
        //:       value returned.
        //:
        //: < 0 - The function completed with an error and the error code is
        //:       the value returned.
};

class ThreadBinder {
    // This class provides an invokable that binds a 'Thread' object to a
    // 'Thread' member function.

    // DATA
    void (Thread::*d_memFunc)();  // member function to invoke (not owned)
    Thread        *d_thread_p;    // object on which to invoke it (not owned)

  public:
    // CREATORS
    ThreadBinder(void (Thread::*memberFunction)(), Thread *threadPtr)
        // Create a 'ThreadBinder' object that binds the specified
        // 'memberFunction' and 'threadPtr'.
    : d_memFunc(memberFunction)
    , d_thread_p(threadPtr)
    {
    }

    // MANIPULATORS
    void operator()()
        // Invoke the member function on the 'Thread' object that were supplied
        // at construction.
    {
        (d_thread_p->*d_memFunc)();
    }
};

class ThreadGroup {
    // This class provides a simple wrapper around 'bslmt::ThreadGroup'.

    // DATA
    bslmt::ThreadGroup d_theGroup;

  private:
    // NOT IMPLEMENTED
    ThreadGroup(const ThreadGroup&);
    ThreadGroup& operator=(const ThreadGroup&);

  public:
    // CREATORS
    ThreadGroup()
        // Create a 'ThreadGroup' object.
    : d_theGroup()
    {
    }

    ~ThreadGroup()
        // Destroy this object after first joining all threads of this
        // 'ThreadGroup' that are still active (if any).
    {
        if (d_theGroup.numThreads() > 0) {
            std::cout << "A 'ThreadGroup' is being destroyed with active "
                      << "threads.  Joining them." << std::endl;
            join();
        }
    }

    // MANIPULATORS
    void addThread(Thread& thread)
        // Add to this thread group the specified 'thread' whose entry point is
        // the 'Thread::callable' member function.
    {
        d_theGroup.addThread(ThreadBinder(&Thread::callable, &thread));
    }

    void join()
        // Join all threads of this 'ThreadGroup' that are still active (if
        // any).
    {
        d_theGroup.joinAll();
    }
};

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace BSLMT_USAGE_EXAMPLE_1 {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a Parallelizable Algorithm
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we use a 'bslmt::Latch' object to help implement an
// operation that can be parallelized across a series of sub-tasks (or "jobs").
// The "parent" operation enqueue's the jobs and blocks on a thread pool, and
// uses the latch as a signaling mechanism to indicate when all of the jobs
// have been completed and return to the caller.
//
// The use of a 'bslmt::Latch', rather than a 'bslmt::Barrier', is important to
// ensure that jobs in the thread pool do not block until the entire task is
// completed (preventing the thread pool from processing additional work).
//
// Suppose, for example, we want to provide a C++ type for computing a vector
// sum (vector in the mathematical sense).  That is, for two input vectors, 'A'
// and 'B', each of length 'N', the result is a vector, 'R', of length 'N',
// where each element at index 'i' has the value:
//..
//  R[i] = A[i] + B[i];
//..
// This function can easily be computed in parallel because the value for each
// result index only depends on the input vectors.
//
// First, assume we have a class, 'FixedThreadPool', providing the following
// public interface (for brevity, the details have been elided; see
// 'bdlmt_fixedthreadpool' or 'bdlmt_threadpool' for examples of thread pools):
//..
    class FixedThreadPool {

      public:
        // ...

        void enqueueJob(const bsl::function<void()>& job);
            // Enqueue the specified 'job' to be executed by the next available
            // thread.
    };
//..
// Next, we declare the signature for our vector sum function,
// 'parallelVectorSum':
//..
    void parallelVectorSum(double          *result,
                           const double    *inputA,
                           const double    *inputB,
                           int              numElements,
                           FixedThreadPool *threadPool,
                           int              numJobs);
        // Load the specified 'result' array with the vector sum of the
        // specified 'inputA' and 'inputB', each having at least the specified
        // 'numElements', using the specified 'threadPool' to perform the
        // operation in parallel using the specified 'numJobs' parallel jobs.
        // The behavior is undefined unless 'numElements > 0', 'numJobs > 0',
        // and 'result', 'inputA', and 'inputB' each contain at least
        // 'numElements'.
//..
// Now, we declare a helper function, 'vectorSumJob', that will be used as a
// sub-task by 'parallelVectorSum'.  'vectorSumJob' computes a single-threaded
// vector sum and uses a 'bslmt::Latch' object, 'completionSignal', to indicate
// to the parent task that the computation has been completed:
//..
    void vectorSumJob(double       *result,
                      bslmt::Latch *completionSignal,
                      const double *inputA,
                      const double *inputB,
                      int           numElements)
        // Load the specified 'result' array with the vector sum of the
        // specified 'inputA' and 'inputB', each having at least the specified
        // 'numElements', and when the operation is complete signal the
        // specified 'completionSignal'.  The behavior is undefined unless
        // 'numElements > 0' and 'result', 'inputA', and 'inputB' each contain
        // at least 'numElements'.
    {
        for (int i = 0; i < numElements; ++i) {
            result[i] = inputA[i] + inputB[i];
        }

        completionSignal->arrive();
    }
//..
// Note that 'bslmt::Latch::arrive' does not block the current thread (unlike
// 'bslmt::Barrier::wait'), and within the context of a thread pool, this job
// will complete and the thread will be returned to the pool to accept more
// work.
//
// Next, we provide a rudimentary function argument binder (specific to this
// usage example) in view of the fact that such a facility is not available at
// this level in the BDE hierarchy:
//..
    class UsageBinder {
        // This class provides an invokable that is tailored to bind the
        // 'vectorSumJob' (defined above) to its requisite five arguments.

      public:
        // TYPES
        typedef void FREE_FUNCTION(double       *,
                                   bslmt::Latch *,
                                   const double *,
                                   const double *,
                                   int           );

      private:
        // DATA
        FREE_FUNCTION *d_func_p;
        double        *d_arg1_p;
        bslmt::Latch  *d_arg2_p;
        const double  *d_arg3_p;
        const double  *d_arg4_p;
        int            d_arg5;

      public:
        // CREATORS
        UsageBinder(FREE_FUNCTION *functionPtr,
                    double        *arg1Ptr,
                    bslmt::Latch  *arg2Ptr,
                    const double  *arg3Ptr,
                    const double  *arg4Ptr,
                    int            arg5)
            // Create a 'UsageBinder' object that binds the specified
            // 'functionPtr' to the specified 'arg1Ptr', 'arg2Ptr', 'arg3Ptr',
            // 'arg4Ptr', and 'arg5' arguments.
        : d_func_p(functionPtr)
        , d_arg1_p(arg1Ptr)
        , d_arg2_p(arg2Ptr)
        , d_arg3_p(arg3Ptr)
        , d_arg4_p(arg4Ptr)
        , d_arg5(arg5)
        {
        }

        // MANIPULATORS
        void operator()()
            // Invoke the function that was supplied at construction on the
            // arguments that were supplied at construction.
        {
            (*d_func_p)(d_arg1_p, d_arg2_p, d_arg3_p, d_arg4_p, d_arg5);
        }
    };
//..
// Then, we define 'parallelVectorSum':
//..
    void parallelVectorSum(double          *result,
                           const double    *inputA,
                           const double    *inputB,
                           int              numElements,
                           FixedThreadPool *threadPool,
                           int              numJobs)
    {
        // Ensure that there is at least 1 element per job.

        if (numElements < numJobs) {
            numJobs = numElements;
        }

        const int jobSize = numElements / numJobs;
//..
// Now, we define a 'bslmt::Latch' object, 'completionSignal', that we will
// use to track the completion of this work:
//..
        bslmt::Latch completionSignal(numJobs);

        for (int i = 0; i < numJobs; ++i) {
            // If 'numJobs' doesn't evenly divide 'numElements', the last job
            // will process the remaining elements.  For simplicity, we have
            // chosen not distribute the elements between jobs as evenly as is
            // possible.

            int offset = i * jobSize;
            int size   = (i == numJobs - 1) ? jobSize + numElements % numJobs
                                            : jobSize;
            ASSERT(0 != size);

            threadPool->enqueueJob(UsageBinder(vectorSumJob,
                                               result + offset,
                                               &completionSignal,
                                               inputA + offset,
                                               inputB + offset,
                                               size));
        }
//..
// Finally, calling 'wait' on the latch will block this function from returning
// until all the queued jobs computing the vector sum have been completed:
//..
        completionSignal.wait();
    }
//..

// Implementation Note:  The following code provides a stub implementation for
// 'FixedThreadPool' sufficient for sanity testing the usage example:

void FixedThreadPool::enqueueJob(const bsl::function<void()>& job)
{
    bslmt::ThreadUtil::Handle handle;

    int rc = bslmt::ThreadUtil::create(&handle, job);

    ASSERTV(rc, 0 == rc);

    bslmt::ThreadUtil::detach(handle);
}

}  // close namespace BSLMT_USAGE_EXAMPLE_1

// ----------------------------------------------------------------------------
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

namespace groups {

class IndependentLinearValue {
    // This class is shared by threads in order to test that producers and
    // consumers do not mix 'inc()' and 'mult()' calls.

    // DATA
    bsls::SpinLock d_lock;
    int            d_value;

  private:
    // NOT IMPLEMENTED
    IndependentLinearValue(const IndependentLinearValue&);
    IndependentLinearValue& operator=(const IndependentLinearValue&);

  public:
    IndependentLinearValue()
        // Create an 'IndependentLinearValue' object having the default value.
    : d_lock(bsls::SpinLock::s_unlocked)
    , d_value(0)
    {
    }

    // MANIPULATORS
    void inc()
        // Atomically increment the value of this 'IndependentLinearValue'
        // object.
    {
        bsls::SpinLockGuard guard(&d_lock);

        ++d_value;
    }

    void mult()
        // Atomically multiply the value of this 'IndependentLinearValue'
        // object by a factor of 2.
    {
        bsls::SpinLockGuard guard(&d_lock);

        d_value *= 2;
    }

    int value() const
        // Return the current value of this 'IndependentLinearValue' object.
    {
        return d_value;
    }
};

class ThreadTest : public Thread {

    // DATA
    bslmt::Latch&           d_theLatch;
    bslmt::Barrier&         d_theBarrier;
    IndependentLinearValue& d_theValue;

    // FRIENDS
    friend class ThreadProducerArrive;
    friend class ThreadProducerCountDown;
    friend class ThreadConsumer;
    friend class ThreadProducerConsumer;

  public:
    // CREATORS
    ThreadTest(bslmt::Latch&           latch,
               bslmt::Barrier&         barrier,
               IndependentLinearValue& value)
        // Create a 'ThreadTest' object that uses the specified 'latch',
        // 'barrier', and 'value'.
    : Thread()
    , d_theLatch(latch)
    , d_theBarrier(barrier)
    , d_theValue(value)
    {
    }
};

class ThreadProducerArrive : public ThreadTest {
    // Instances of this class execute the following operations (in the order
    // shown) on the 'Latch', 'Barrier', and 'IndependentLinearValue' that are
    // supplied at construction:
    //..
    //  barrier.wait();
    //  value.inc();
    //  latch.arrive();
    //..
    // This sequence of operations is intended to be run concurrently with
    // those of other test objects.

  public:
    // CREATORS
    ThreadProducerArrive(bslmt::Latch&           latch,
                         bslmt::Barrier&         barrier,
                         IndependentLinearValue& value)
        // Create a 'ThreadProducerArrive' object that uses the specified
        // 'latch', 'barrier', and 'value'.
    : ThreadTest(latch, barrier, value)
    {
    }

    // MANIPULATORS
    virtual int mainLoop()
        // Execute the sequence of operations specific to a
        // 'ThreadProducerArrive' object, and return 1.
    {
        d_theBarrier.wait();
        d_theValue.inc();
        d_theLatch.arrive();

        return 1;
    }
};

class ThreadProducerCountDown : public ThreadTest {
    // Instances of this class execute the following operations (in the order
    // shown) on the 'Latch', 'Barrier', and 'IndependentLinearValue' that are
    // supplied at construction:
    //..
    //  barrier.wait();
    //  value.inc();
    //  latch.countDown(1);
    //..
    // This sequence of operations is intended to be run concurrently with
    // those of other test objects.

  public:
    // CREATORS
    ThreadProducerCountDown(bslmt::Latch&           latch,
                            bslmt::Barrier&         barrier,
                            IndependentLinearValue& value)
        // Create a 'ThreadProducerCountDown' object that uses the specified
        // 'latch', 'barrier', and 'value'.
    : ThreadTest(latch, barrier, value)
    {
    }

    // MANIPULATORS
    virtual int mainLoop()
        // Execute the sequence of operations specific to a
        // 'ThreadProducerCountDown' object, and return 1.
    {
        d_theBarrier.wait();
        d_theValue.inc();
        d_theLatch.countDown(1);

        return 1;
    }
};

class ThreadConsumer : public ThreadTest {
    // Instances of this class execute the following operations (in the order
    // shown) on the 'Latch', 'Barrier', and 'IndependentLinearValue' that are
    // supplied at construction:
    //..
    //  barrier.wait();
    //  latch.wait();
    //  value.mult();
    //..
    // This sequence of operations is intended to be run concurrently with
    // those of other test objects.

  public:
    // CREATORS
    ThreadConsumer(bslmt::Latch&           latch,
                   bslmt::Barrier&         barrier,
                   IndependentLinearValue& value)
        // Create a 'ThreadConsumer' object that uses the specified 'latch',
        // 'barrier', and 'value'.
    : ThreadTest(latch, barrier, value)
    {
    }

    // MANIPULATORS
    virtual int mainLoop()
        // Execute the sequence of operations specific to a 'ThreadConsumer'
        // object, and return 1.
    {
        d_theBarrier.wait();
        d_theLatch.wait();
        d_theValue.mult();

        return 1;
    }
};

class ThreadProducerConsumer : public ThreadTest {
    // Instances of this class execute the following operations (in the order
    // shown) on the 'Latch', 'Barrier', and 'IndependentLinearValue' that are
    // supplied at construction:
    //..
    //  barrier.wait();
    //  value.inc();
    //  latch.arriveAndWait();
    //  value.mult();
    //..
    // This sequence of operations is intended to be run concurrently with
    // those of other test objects.

  public:
    // CREATORS
    ThreadProducerConsumer(bslmt::Latch&           latch,
                           bslmt::Barrier&         barrier,
                           IndependentLinearValue& value)
        // Create a 'ThreadProducerConsumer' object that uses the specified
        // 'latch', 'barrier', and 'value'.
    : ThreadTest(latch, barrier, value)
    {
    }

    // MANIPULATORS
    virtual int mainLoop()
        // Execute the sequence of operations specific to a
        // 'ThreadProducerConsumer' object, and return 1.
    {
        d_theBarrier.wait();
        d_theValue.inc();
        d_theLatch.arriveAndWait();
        d_theValue.mult();

        return 1;
    }
};

template <class PRODUCER, class CONSUMER, class PRODUCERCONSUMER>
void test(int numProducers, int numConsumers, int numProducerConsumers)
    // Execute, concurrently, the specified 'numProducers' of type 'PRODUCER',
    // the specified 'numConsumers' of type 'CONSUMER', and the specified
    // 'numProducerConsumers' of type 'PRODUCERCONSUMER'.
{
    if (veryVerbose) {
        cout << "Testing: "
             << numConsumers         << " consumers vs. "
             << numProducers         << " producers vs. "
             << numProducerConsumers << " producer-consumers" << endl;
    }

    IndependentLinearValue myInt;  // This will contain the result.

    const int myProducersAmount = numProducers + numProducerConsumers;
    const int myConsumersAmount = numConsumers + numProducerConsumers;

    // Latch under test.
    bslmt::Latch myLatch(myProducersAmount);

    // This barrier enables all threads to start at the same time to maximize
    // parallelism.
    bslmt::Barrier myBarrier(numProducers
                           + numConsumers
                           + numProducerConsumers);

    bsl::vector<Thread *> myThreads;
    myThreads.reserve(numProducers + numConsumers + numProducerConsumers);

    // create producers

    for (int i = 0; i < numProducers; ++i) {
        myThreads.push_back(new PRODUCER(myLatch, myBarrier, myInt));
    }

    // create consumers

    for (int i = 0; i < numConsumers; ++i) {
        myThreads.push_back(new CONSUMER(myLatch, myBarrier, myInt));
    }

    // create producer-consumers

    for (int i = 0; i < numProducerConsumers; ++i) {
        myThreads.push_back(new PRODUCERCONSUMER(myLatch, myBarrier, myInt));
    }

    ThreadGroup myGroup;

    // Start all threads.  Note that the threads will wait on a barrier before
    // doing their job until the last one is started.

    for (size_t i = 0; i < myThreads.size(); ++i) {
        myGroup.addThread(*myThreads[i]);
    }

    myGroup.join();

    for (size_t i = 0; i < myThreads.size(); ++i) {
        delete myThreads[i];
    }

    const int result = myProducersAmount * (1 << myConsumersAmount);

    ASSERTV(result, myInt.value(), result == myInt.value());

    const int myCount = myLatch.currentCount();
    ASSERTV(0, myCount, 0 == myCount);

    const bool myTryWait = myLatch.tryWait();
    ASSERTV(true, myTryWait, true == myTryWait);
}

}  // close namespace groups

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int test          = argc > 1 ? atoi(argv[1]) : 0;
    const int threadsAmount = argc > 2 ? atoi(argv[2]) : 18;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        using namespace BSLMT_USAGE_EXAMPLE_1;

        // Perform a sanity test on 'parallelVectorSum' (defined above).

        const double inputA[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0 };
        const double inputB[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0 };

        const int NUM_ELEM = sizeof inputA / sizeof *inputA;

        double result[NUM_ELEM];

        FixedThreadPool pool;

        for (int jobs = 1; jobs < NUM_ELEM + 1; ++jobs) {
            for (int i = 0; i < NUM_ELEM; ++i) {
                result[i] = 0.0;
            }

            parallelVectorSum(result, inputA, inputB, NUM_ELEM, &pool, jobs);

            for (int i = 0; i < NUM_ELEM; ++i) {
                ASSERT(result[i] == inputA[i] + inputB[i]);
            }

            if (veryVerbose) {
                for (int i = 0; i < NUM_ELEM; ++i) {
                    cout << result[i] << " ";
                }
                cout << endl;
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // PRODUCERS(C-D), CONSUMERS, AND PRODUCERS-CONSUMERS
        //
        // Concerns:
        //: 1 The latch must be a pass-through for 'countDown(n)' callers.
        //:
        //: 2 The latch must be a barrier for 'arriveAndWait()' and 'wait()'
        //:   callers.
        //:
        //: 3 At the end of the test the latch has reached the synchronization
        //:   point.
        //
        // Plan:
        //: 1 Create three groups of threads (from 1 to 'X' concurrent threads
        //:   in each group).
        //:
        //: 2 The first group will call 'countDown(1)'.
        //:
        //: 3 The second group will call 'wait()'.
        //:
        //: 3 The third group will call 'arriveAndWait()'.
        //:
        //: 4 Verify that the operation produces the expected result (see the
        //:   test plan overview).  (C-1..3)
        //
        // Testing:
        //   countDown(int n); wait(); arriveAndWait();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PRODUCERS(C-D), CONSUMERS, AND PRODUCERS-CONSUMERS"
                 << endl
                 << "=================================================="
                 << endl;

        for (int i = 1; i <= threadsAmount / 3; ++i) {
            for (int j = 1; j <= threadsAmount / 3; ++j) {
                for (int k = 1; k <= threadsAmount / 3; ++k) {
                    groups::test<groups::ThreadProducerCountDown,
                                 groups::ThreadConsumer,
                                 groups::ThreadProducerConsumer>(i, j, k);
                }
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // PRODUCERS(ARRIVE), CONSUMERS, AND PRODUCERS-CONSUMERS
        //
        // Concerns:
        //: 1 The latch must be a pass-through for 'arrive()' callers.
        //:
        //: 2 The latch must be a barrier for 'arriveAndWait()' and 'wait()'
        //:   callers.
        //:
        //: 3 At the end of the test the latch has reached the synchronization
        //:   point.
        //
        // Plan:
        //: 1 Create three groups of threads (from 1 to 'X' concurrent threads
        //:   in each group).
        //:
        //: 2 The first group will call 'arrive()'.
        //:
        //: 3 The second group will call 'wait()'.
        //:
        //: 4 The third group will call 'arriveAndWait()'.
        //:
        //: 5 Verify that the operation produces the expected result (see the
        //:   test plan overview).  (C-1..3)
        //
        // Testing:
        //   arrive();         wait(); arriveAndWait();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PRODUCERS(ARRIVE), CONSUMERS, AND PRODUCERS-CONSUMERS"
                 << endl
                 << "====================================================="
                 << endl;

        for (int i = 1; i <= threadsAmount/3; ++i) {
            for (int j = 1; j <= threadsAmount/3; ++j) {
                for (int k = 1; k <= threadsAmount/3; ++k) {
                    groups::test<groups::ThreadProducerArrive,
                                 groups::ThreadConsumer,
                                 groups::ThreadProducerConsumer>(i, j, k);
                }
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // PRODUCERS(COUNT DOWN) AND CONSUMERS
        //
        // Concerns:
        //: 1 The latch must be a pass-through for 'countDown(n)' callers.
        //:
        //: 2 The latch must be a barrier for 'wait()' callers.
        //:
        //: 3 At the end of the test the latch has reached the synchronization
        //:   point.
        //
        // Plan:
        //: 1 Create two groups of threads (from 1 to 'X' concurrent threads in
        //:   each group).
        //:
        //: 2 The first group will call 'countDown()'.
        //:
        //: 3 The second group will call 'wait()'.
        //:
        //: 4 Verify that the operation produces the expected result (see the
        //:   test plan overview).  (C-1..3)
        //
        // Testing:
        //   countDown(int n); wait();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRODUCERS(COUNT DOWN) AND CONSUMERS" << endl
                          << "===================================" << endl;

        for (int i = 1; i <= threadsAmount/2; ++i) {
            for (int j = 1; j <= threadsAmount/2; ++j) {
                groups::test<groups::ThreadProducerCountDown,
                             groups::ThreadConsumer,
                             groups::ThreadProducerConsumer>(i, j, 0);
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // PRODUCERS(ARRIVE) AND CONSUMERS
        //
        // Concerns:
        //: 1 The latch must be a pass-through for 'arrive()' callers.
        //:
        //: 2 The latch must be a barrier for 'wait()' callers.
        //:
        //: 3 At the end of the test the latch has reached the synchronization
        //:   point.
        //
        // Plan:
        //: 1 Create two groups of threads (from 1 to 'X' concurrent threads in
        //:   each group).
        //:
        //: 2 The first group will call 'arrive()'.
        //:
        //: 3 The second group will call 'wait()'.
        //:
        //: 4 Verify that the operation produces the expected result (see the
        //:   test plan overview).  (C-1..3)
        //
        // Testing:
        //   arrive();         wait();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRODUCERS(ARRIVE) AND CONSUMERS" << endl
                          << "===============================" << endl;

        for (int i = 1; i <= threadsAmount/2; ++i) {
            for (int j = 1; j <= threadsAmount/2; ++j) {
                groups::test<groups::ThreadProducerArrive,
                             groups::ThreadConsumer,
                             groups::ThreadProducerConsumer>(i, j, 0);
            }
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // PRODUCERS-CONSUMERS
        //
        // Concerns:
        //: 1 The latch must be a barrier for 'arriveAndWait()' callers.
        //:
        //: 2 At the end of the test the latch has reached the synchronization
        //:   point.
        //
        // Plan:
        //: 1 Create one group of threads (from 1 to 'X' concurrent threads)
        //:   calling 'arriveAndWait()'.
        //:
        //: 2 Verify that the operation produces the expected result (see the
        //:   test plan overview).  (C-1..2)
        //
        // Testing:
        //   arriveAndWait();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRODUCERS-CONSUMERS" << endl
                          << "===================" << endl;

        for (int i = 1; i <= threadsAmount; ++i) {
            groups::test<groups::ThreadProducerArrive,
                         groups::ThreadConsumer,
                         groups::ThreadProducerConsumer>(0, 0, i);
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PRODUCERS(COUNT DOWN)
        //
        // Concerns:
        //: 1 The latch must be a pass-through for 'countDown(1)' callers.
        //:
        //: 2 At the end of the test the latch has reached the synchronization
        //:   point.
        //
        // Plan:
        //: 1 Create one group of threads (from 1 to 'X' concurrent threads)
        //:   calling 'countDown(1)'.
        //:
        //: 2 Verify that the operation produces the expected result (see the
        //:   test plan overview).  (C-1..2)
        //
        // Testing:
        //   countDown(int n);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRODUCERS(COUNT DOWN)" << endl
                          << "=====================" << endl;

        for (int i = 1; i <= threadsAmount; ++i) {
            groups::test<groups::ThreadProducerCountDown,
                         groups::ThreadConsumer,
                         groups::ThreadProducerConsumer>(i, 0, 0);
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // PRODUCERS(ARRIVE)
        //
        // Concerns:
        //: 1 The latch must be a pass-through for 'arrive()' callers.
        //:
        //: 2 At the end of the test the latch has reached the synchronization
        //:   point.
        //
        // Plan:
        //: 1 Create one group of threads (from 1 to 'X' concurrent threads)
        //:   calling 'arrive()'.
        //:
        //: 2 Verify that the operation produces the expected result (see the
        //:   test plan overview).  (C-1..2)
        //
        // Testing:
        //   arrive();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRODUCERS(ARRIVE)" << endl
                          << "=================" << endl;

        for (int i = 1; i <= threadsAmount; ++i) {
            groups::test<groups::ThreadProducerArrive,
                         groups::ThreadConsumer,
                         groups::ThreadProducerConsumer>(i, 0, 0);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'timedWait'
        //
        // Concerns:
        //: 1 A latch built with 0 has already reached the synchronization
        //:   point.
        //:
        //: 2 A latch built will timeout after reaching 'timeout' time.  The
        //:   initial 'count' given the constructor is unchanged.
        //
        // Plan:
        //: 1 Create a latch with an initial count of 0.
        //:
        //: 2 Verify that 'wait()' calls do not block the test execution.
        //:   (C-1)
        //:
        //: 3 Create a latch with an initial count of 5.
        //:
        //: 4 Verify that 'timedWait()' call times out, and 'currentCount'
        //    returns 5.
        //:   (C-2)
        //
        // Testing:
        //   void timedWait(const bsls::TimeInterval &timeout);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'timedWait'" << endl
                          << "===================" << endl;

        {
            bslmt::Latch myLatch(0);
            myLatch.wait();
        }

        {
            bslmt::Latch myLatch(5);

            // define a timeout of 1s
            bsls::TimeInterval timeOut = bsls::SystemTime::nowRealtimeClock();
            timeOut.addMicroseconds(1000000);

            const int rc = myLatch.timedWait(timeOut);
            ASSERT(-1 == rc);
            const int myCount = myLatch.currentCount();
            ASSERT(5 == myCount);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'wait'
        //
        // Concerns:
        //: 1 A latch built with 0 has already reached the synchronization
        //:   point.
        //
        // Plan:
        //: 1 Create a latch with an initial count of 0.
        //:
        //: 2 Verify that 'wait()' calls do not block the test execution.
        //:   (C-1)
        //
        // Testing:
        //   void wait();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'wait'" << endl
                          << "==============" << endl;

        bslmt::Latch myLatch(0);
        myLatch.wait();

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'arriveAndWait'
        //
        // Concerns:
        //: 1 A latch built with 1 will permit a single thread to not block
        //:   on an 'arriveAndWait()' call.
        //
        // Plan:
        //: 1 Create a latch with an initial count of 1 and verify that an
        //:   'arriveAndWait()' call is not a blocking call.
        //:
        //: 2 Verify that the current count is 0 following the
        //:   'arriveAndWait()' call.  (C-1)
        //
        // Testing:
        //   void arriveAndWait();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'arriveAndWait'" << endl
                          << "=======================" << endl;

        bslmt::Latch myLatch(1);

        myLatch.arriveAndWait();
        {
            const int myCount = myLatch.currentCount();
            ASSERTV(0, myCount, 0 == myCount);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'tryWait'
        //
        // Concerns:
        //: 1 'tryWait' returns 'false' if the latch has not been released.
        //:
        //: 2 'tryWait' returns 'true' if the latch has been released.
        //
        // Plan:
        //: 1 Perform a brute force test setting a latch to a variety of counts
        //:   and verifying the result of 'tryWait'.  (C-1..2)
        //
        // Testing:
        //   bool tryWait() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'tryWait'" << endl
                          << "=================" << endl;

        {
            {
                Obj mX(5);  const Obj& X = mX;

                ASSERT(false == X.tryWait());

                mX.countDown(1);
                ASSERT(false == X.tryWait());

                mX.countDown(4);
                ASSERT(true == X.tryWait());
            }
            {
                const Obj X(0);

                ASSERT(true == X.tryWait());
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'arrive'
        //
        // Concerns:
        //: 1 A latch built with 'x' will need (at least) a sequence of 'x'
        //:   'arrive()' calls in order to reach the synchronization point.
        //
        // Plan:
        //: 1 Create a latch with an initial count of 3.
        //:
        //: 2 Call 'wait' 3 times.
        //:
        //: 3 Verify that the synchronization point has been reached.  (C-1)
        //
        // Testing:
        //   void arrive();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'arrive'" << endl
                          << "================" << endl;

        bslmt::Latch myLatch(3);
        {
            const int myCount = myLatch.currentCount();
            ASSERTV(3, myCount, 3 == myCount);
            const bool myTryWait = myLatch.tryWait();
            ASSERTV(false, myTryWait, false == myTryWait);
        }
        myLatch.arrive();
        {
            const int myCount = myLatch.currentCount();
            ASSERTV(2, myCount, 2 == myCount);
            const bool myTryWait = myLatch.tryWait();
            ASSERTV(false, myTryWait, false == myTryWait);
        }
        myLatch.arrive();
        {
            const int myCount = myLatch.currentCount();
            ASSERTV(1, myCount, 1 == myCount);
            const bool myTryWait = myLatch.tryWait();
            ASSERTV(false, myTryWait, false == myTryWait);
        }
        myLatch.arrive();
        {
            const int myCount = myLatch.currentCount();
            ASSERTV(0, myCount, 0 == myCount);
            const bool myTryWait = myLatch.tryWait();
            ASSERTV(true, myTryWait, true == myTryWait);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, DTOR, AND PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 That after construction the current count of a latch is the
        //:   supplied count value.
        //:
        //: 2 That 'countDown' decrements the supplied count by the indicated
        //:   count.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a latch with a variety of initial counts and verify
        //:   'currentCount' returns the supplied count.  (C-1)
        //:
        //: 2 Perform a loop-based test, creating a latch with a variety of
        //:   initial counts, and use 'countDown' to decrement it by a variety
        //:   of step sizes, comparing the 'currentCount' to an oracle count
        //:   value.  (C-2)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-3)
        //
        // Testing:
        //   Latch(int count);
        //   ~Latch();
        //   void countDown(int n);
        //   int currentCount() const;
        // --------------------------------------------------------------------

        if (verbose) cout
             << endl
             << "DEFAULT CTOR, DTOR, AND PRIMARY MANIPULATORS" << endl
             << "============================================" << endl;

        if (verbose) cout << "\nCheck value constructor." << endl;

        for (int i = 0; i < 10; ++i) {
            if (veryVerbose) { T_ P(i); }

            const Obj X(i);

            ASSERT(i == X.currentCount());
        }

        if (verbose) cout << "\nCheck 'countDown'." << endl;

        for (int stepSize = 1; stepSize < 10; ++stepSize) {
            if (veryVerbose) { T_ P(stepSize); }

            for (int initialCount = 1; initialCount < 100; ++initialCount) {
                if (veryVeryVerbose) { T_ T_ P(initialCount); }

                Obj mX(initialCount);  const Obj &X = mX;

                int count = initialCount;
                while (count > 0) {
                    if (veryVeryVeryVerbose) { T_ T_ T_ P(count); }

                    int step = bsl::min(stepSize, count);
                    count -= step;

                    mX.countDown(step);
                    ASSERT(count == X.currentCount());
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard guard;

            if (veryVerbose) cout << "\t'CTOR'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1));
                ASSERT_SAFE_PASS(Obj( 0));
                ASSERT_SAFE_FAIL(Obj(-1));
            }

            if (veryVerbose) cout << "\t'countDown'" << endl;
            {
                Obj mX(10);

                ASSERT_PASS(mX.countDown( 1));
                ASSERT_FAIL(mX.countDown(-1));
                ASSERT_FAIL(mX.countDown(10));
            }
        }

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
