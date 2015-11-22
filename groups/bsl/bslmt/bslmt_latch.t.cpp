// bslmt_latch.t.cpp                                                  -*-C++-*-
#include <bslmt_latch.h>

#include <bslmt_barrier.h>        // for testing only
#include <bslmt_lockguard.h>
#include <bslmt_threadgroup.h>    // for testing only
#include <bslmt_threadutil.h>     // for testing only

#include <bdef_bind.h>       // TBD
#include <bdef_function.h>   // TBD

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_spinlock.h>
#include <bsls_timeutil.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_limits.h>
#include <bsl_iostream.h>
#include <bsl_list.h>        // for usage example
#include <bsl_vector.h>      // for usage example

#include <math.h>

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
// Producer Threads call:         'arrive()' or 'countDown(n)'
// Consumer Threads call:         'wait()'
// ProducerConsumer Threads call: 'arriveAndWait()'
//
// In order to test that not a single thread in a "consumer group" passes the
// latch before the synchronization point is reached the following strategy
// is performed:
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
//  # producers * pow(2, # consumers)
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
// MANIPULATORS
// [  ] void tryWait();
//
// ACCESSORS
// [ 1] int currentCount() const;
//
// Breathing tests and non-concurrent usage:
// MANIPULATORS - MAIN PROCESS
// [ 5] void wait();
// [ 4] void arriveAndWait();
// [ 1] void countDown(int n);
// [ 2] void arrive();
//
// Interactions between manipulators:
// MANIPULATORS - CONCURRENT USAGE
// [12] countDown(int n); wait(); arriveAndWait();
// [11] arrive();         wait(); arriveAndWait();
// [10] countDown(int n); wait();
// [ 9] arrive();         wait();
// [ 8] arriveAndWait();
// [ 7] countDown(int n);
// [ 6] arrive();
// ----------------------------------------------------------------------------

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

bool verbose             = false;
bool veryVerbose         = false;
bool veryVeryVerbose     = false;
bool veryVeryVeryVerbose = false;

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

class Thread {
    // This class is used to facilitate the launch of a thread.  Derived
    // classes must implement the 'mainLoop' 'virtual' function.  See its
    // contract below.

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
    {
        int rc = -1;

        while (0 == (rc = mainLoop())) {
            // empty
        }

        if (veryVeryVerbose) {
            if (rc < 0) {
                cout << "Thread terminated with error: " << rc << endl;
            } else {
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

class ThreadGroup {

    // DATA
    bslmt::ThreadGroup theGroup;

  public:
    // CREATORS
    ThreadGroup()
    : theGroup()
    {
    }

    ~ThreadGroup()
    {
        if (theGroup.numThreads() > 0) {
            std::cout << "Thread Group being destroyed while having active "
                      << "threads.  Joining them." << std::endl;
            join();
        }
    }

    // MANIPULATORS
    void createThread(Thread& aThread)
    {
        theGroup.addThread(bdef_BindUtil::bind(&Thread::callable, &aThread));
    }

    void join()
    {
        theGroup.joinAll();
    }
};

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace BSLMT_USAGE_EXAMPLE_1 {

///Usage
///-----
// This section illustrates intended use of this component
//
///Example 1: Implementing a Parallelizable Algorithm
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we use a 'bslmt::Latch' object to help implement an
// operation that can be parallelized across a series of sub-tasks (or "jobs").
// The "parent" operation enqueue's the jobs and blocks on a thread-pool, and
// uses the latch as a signalling mechanism to indicate when all of the jobs
// have been completed and return to the caller.
//
// The use of a 'bslmt::Latch', rather than a 'bslmt::Barrier', is important to
// ensure that jobs in the thread-pool do not block until the entire task is
// completed (preventing the thread-pool from processing additional work).
//
// Suppose, for example, we want to provide a C++ type for computing a vector
// sum (vector in the mathematical sense).  That is, for two input vectors, A,
// and B, each of length N, the result is a vector, R, of length N, where each
// element at index i has the value:
//..
//  R[i] = A[i] + B[i];
//..
// This function can easily be computed in parallel because the value for each
// result index only depends on the input vectors.
//
// First, assume we have a class 'FixedThreadPool' providing the following
// public interface (for brevity, the details have been elided; see
// 'bdlmt_fixedthreadpool' or 'bdlmt_threadpool' for examples of thread-pools):
//..
    class FixedThreadPool {

      public:
        //...

        void enqueueJob(const bdef_Function<void(*)()>& job);
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
                           const int        numElements,
                           FixedThreadPool *threadPool,
                           int              numJobs);
        // Load the specified 'result' array with the vector sum of the
        // specified 'inputA', and 'inputB', each having at least
        // 'numElements', using the specified 'threadPool' to perform the
        // operation in parallel using the specified 'numJobs' parallel jobs.
        // The behavior is undefined unless 'numJobs > 0' and 'result',
        // 'inputA', and 'inputB' each contain at least 'numElements'.
//..
// Now, we declare a helper function, 'vectorSumJob', which will be used as a
// sub-task by 'parallelVectorSum'.  'vectorSumJob' computes a single threaded
// vector sum and uses a 'bslmt::Latch' object, 'completionSignal', to indicate
// to the parent task that the computation has been completed.
//..
    void vectorSumJob(double       *result,
                      bslmt::Latch *completionSignal,
                      const double *inputA,
                      const double *inputB,
                      const int     numElements)
        // Load the specified 'result' array with the vector sum of the
        // specified 'inputA', and 'inputB', each having at least
        // 'numElements', and when the operation is complete signal the
        // specified 'completionSignal'.
    {
        for (int i = 0; i < numElements; ++i) {
            result[i] = inputA[i] + inputB[i];
        }
        completionSignal->arrive();
    }
//..
// Notice that 'bslmt::Latch::arrive' does not block the current thread (unlike
// 'bslmt::Barrier::wait', and within the context of a thread pool, this job
// will complete and the thread will be returned to the pool to accept more
// work.
//
// Then we define 'parallelVectorSum':
//..
    void parallelVectorSum(double          *result,
                           const double    *inputA,
                           const double    *inputB,
                           const int        numElements,
                           FixedThreadPool *threadPool,
                           int              numJobs)
    {
        // Ensure that there is at least 1 element per job.

        if (numElements < numJobs) {
            numJobs = numElements;
        }

        int jobSize = numElements / numJobs;
//..
// Here we define a 'bslmt::Latch' object, 'completionSignal', that we will
// use to track the completion of this work:
//..
        bslmt::Latch completionSignal(numJobs);
        for (int i = 0; i < numJobs; ++i) {
            // If 'numJobs' doesn't evenly divide 'numElements' the last job
            // will process the remaining elements.  For simplicity, we've
            // chosen not distribute the elements between jobs as evenly as is
            // possible.

            int offset = i * jobSize;
            int size   =  (i == numJobs - 1) ? jobSize + numElements % numJobs
                                             : jobSize;
            if (0 != size) {
                threadPool->enqueueJob(bdef_BindUtil::bind(vectorSumJob,
                                                           result + offset,
                                                           &completionSignal,
                                                           inputA + offset,
                                                           inputB + offset,
                                                           size));
            }
        }
//..
// Finally, calling 'wait' on the latch will block this function from returning
// until all the queued jobs computing the vector sum have been completed:
//..
        completionSignal.wait();
    }
//..

// Implementation note:  The following code provides a fake implementation for
// 'FixedThreadPool' sufficient for sanity testing this usage example:

void FixedThreadPool::enqueueJob(const bdef_Function<void(*)()>& job)
{
    bslmt::ThreadUtil::Handle handle;

    int rc = bslmt::ThreadUtil::create(&handle, job);

    ASSERTV(rc, 0 == rc);

    bslmt::ThreadUtil::detach(handle);
}

}  // namespace BSLMT_USAGE_EXAMPLE_1

// ----------------------------------------------------------------------------
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

namespace groups {

class IndependentLinearValue {
    // This class is shared by threads in order to test that producers and
    // consumers do not mix 'inc()' and 'mult()' calls.

    // DATA
    bsls::SpinLock myLock;
    int            myValue;

  public:
    IndependentLinearValue()
    : myValue(0)
    {
    }

    // MANIPULATORS
    void inc()
    {
        myLock.lock();
        ++myValue;
        myLock.unlock();
    }

    void mult()
    {
        myLock.lock();
        myValue *= 2;
        myLock.unlock();
    }

    int value() const
    {
        return myValue;
    }
};

class ThreadTest : public Thread {

  protected:
    // DATA
    bslmt::Latch&           theLatch;
    bslmt::Barrier&         theBarrier;
    IndependentLinearValue& theValue;

  public:
    // CREATORS
    ThreadTest(bslmt::Latch&           aLatch,
               bslmt::Barrier&         aBarrier,
               IndependentLinearValue& aValue)
    : Thread()
    , theLatch(aLatch)
    , theBarrier(aBarrier)
    , theValue(aValue)
    {
    }
};

class ThreadProducerArrive : public ThreadTest {

  public:
    // CREATORS
    ThreadProducerArrive(bslmt::Latch&           aLatch,
                         bslmt::Barrier&         aBarrier,
                         IndependentLinearValue& aValue)
    : ThreadTest(aLatch, aBarrier, aValue)
    {
    }

    // MANIPULATORS
    virtual int mainLoop()
    {
        theBarrier.wait();
        theValue.inc();
        theLatch.arrive();

        return 1;
    }
};

class ThreadProducerCountDown : public ThreadTest {

  public:
    // CREATORS
    ThreadProducerCountDown(bslmt::Latch&           aLatch,
                            bslmt::Barrier&         aBarrier,
                            IndependentLinearValue& aValue)
    : ThreadTest(aLatch, aBarrier, aValue)
    {
    }

    // MANIPULATORS
    virtual int mainLoop()
    {
        theBarrier.wait();
        theValue.inc();
        theLatch.countDown(1);

        return 1;
    }
};

class ThreadConsumer : public ThreadTest {

  public:
    // CREATORS
    ThreadConsumer(bslmt::Latch&           aLatch,
                   bslmt::Barrier&         aBarrier,
                   IndependentLinearValue& aValue)
    : ThreadTest(aLatch, aBarrier, aValue)
    {
    }

    // MANIPULATORS
    virtual int mainLoop()
    {
        theBarrier.wait();
        theLatch.wait();
        theValue.mult();

        return 1;
    }
};

class ThreadProducerConsumer : public ThreadTest {

  public:
    // CREATORS
    ThreadProducerConsumer(bslmt::Latch&           aLatch,
                           bslmt::Barrier&         aBarrier,
                           IndependentLinearValue& aValue)
    : ThreadTest(aLatch, aBarrier, aValue)
    {
    }

    // MANIPULATORS
    virtual int mainLoop()
    {
        theBarrier.wait();
        theValue.inc();
        theLatch.arriveAndWait();
        theValue.mult();

        return 1;
    }
};

template <class PRODUCER, class CONSUMER, class PRODUCERCONSUMER>
void test(const int aProducersNumber,
          const int aConsumersNumber,
          const int aProducerConsumerNumber)
{
    if (veryVerbose) {
        cout << "Testing: "
             << aConsumersNumber << " consumers vs. "
             << aProducersNumber << " producers vs. "
             << aProducerConsumerNumber << " producers-consumer" << endl;
    }

    IndependentLinearValue result;

    const int myProducersAmount = aProducersNumber + aProducerConsumerNumber;
    const int myConsumersAmount = aConsumersNumber + aProducerConsumerNumber;

    // Latch under test.
    bslmt::Latch myLatch(myProducersAmount);

    // This barrier enables all threads to start at the same time to maximize
    // parallelism.
    bslmt::Barrier myBarrier(aProducersNumber +
                             aConsumersNumber +
                             aProducerConsumerNumber);

    bsl::vector<Thread *> myThreads;
    myThreads.reserve(aProducersNumber +
                      aConsumersNumber +
                      aProducerConsumerNumber);

    // producers

    for (int i = 0; i < aProducersNumber; ++i) {
        myThreads.push_back(new PRODUCER(myLatch, myBarrier, result));
    }

    // consumers

    for (int i = 0; i < aConsumersNumber; ++i) {
        myThreads.push_back(new CONSUMER(myLatch, myBarrier, result));
    }

    // producer/consumers

    for (int i = 0; i < aProducerConsumerNumber; ++i) {
        myThreads.push_back(new PRODUCERCONSUMER(myLatch, myBarrier, result));
    }

    ThreadGroup myGroup;

    // Start all threads.  Note that the threads will wait on a barrier before
    // doing their job until the last one is started.

    for (size_t i = 0; i < myThreads.size(); ++i) {
        myGroup.createThread(*myThreads[i]);
    }

    myGroup.join();

    for (size_t i = 0; i < myThreads.size(); ++i) {
        delete myThreads[i];
    }

    const double result = myProducersAmount * pow(static_cast<const int>(2),
                                                  myConsumersAmount);

    ASSERTV(result, result.value(), result == result.value());

    const int myCount = myLatch.currentCount();
    ASSERTV(0, myCount, 0 == myCount);
    const bool myTryWait = myLatch.tryWait();
    ASSERTV(true, myTryWait, true == myTryWait);
}

// ============================================================================
//                         CASE 13 RELATED ENTITIES
// ----------------------------------------------------------------------------

class Job {

    // DATA
    bslmt::Latch d_latch;

  public:
    // PUBLIC DATA
    const double *const d_input_a;     // owned
    const double *const d_input_b;     // owned
    double *const       d_result;      // owned
    const int           d_input_size;

    // CREATORS
    Job(const double *a, const double *b, double *r, int size, int workers)
    : d_input_a(a)
    , d_input_b(b)
    , d_result(r)
    , d_input_size(size)
    , d_latch(workers)
    {
    }

    ~Job()
    {
        delete [] d_input_a;
        delete [] d_input_b;
        delete [] d_result;
    }

    // MANIPULATORS
    void slice_done()
    {
        d_latch.arrive();
    }

    void wait()
    {
        d_latch.wait();
    }
};

class Worker {

    // DATA
    const int        d_workerIndex;
    const int        d_workersAmount;
    bsl::list<Job *> d_jobs;
    bslmt::Condition d_condition;
    bslmt::Mutex     d_mutex;

  public:
    // CREATORS
    Worker(int workerIndex, int workersAmount)
    : d_workerIndex(workerIndex)
    , d_workersAmount(workersAmount)
    , d_jobs()
    , d_condition()
    , d_mutex()
    {
    }

    // MANIPULATORS
    void mainLoop()
    {
        while (true) {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

            while (d_jobs.empty()) {
                d_condition.wait(&d_mutex);
            }

            Job *job = d_jobs.front();
            d_jobs.pop_front();

            if (0 == job) {
                return;                                               // RETURN
            }

            const int localSize  = job->d_input_size / d_workersAmount;
            const int localIndex = localSize * d_workerIndex;

            const double *const a = job->d_input_a;
            const double *const b = job->d_input_b;
            double *const       r = job->d_result;

            for (int i = 0; i < localSize; ++i) {
                r[localIndex + i] = a[localIndex + i] + b[localIndex + i];
            }

            job->slice_done();
        }
    }

    void submit(Job *job)
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

        d_jobs.push_back(job);
        d_condition.signal();
    }

    void terminate()
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

        d_jobs.push_front(0);
        d_condition.signal();
    }
};

class WorkerPool {

    // DATA
    bsl::vector<Worker *> d_workers;
    bslmt::ThreadGroup    d_threadGroup;

  public:
    // CREATORS
    explicit WorkerPool(int numberOfWorkers)
    : d_workers()
    , d_threadGroup()
    {
        d_workers.reserve(numberOfWorkers);
        for (int i = 0; i < numberOfWorkers; ++i) {
            d_workers.push_back(new Worker(i, numberOfWorkers));
            d_threadGroup.addThread(bdef_BindUtil::bind(&Worker::mainLoop,
                                                                d_workers[i]));

        }
    }

    ~WorkerPool()
    {
        for (size_t i = 0; i < d_workers.size(); ++i) {
            d_workers[i]->terminate();
        }

        d_threadGroup.joinAll();

        for (size_t i = 0; i < d_workers.size(); ++i) {
            delete d_workers[i];
        }
    }

    // MANIPULATORS
    void submit(Job *aJob)
    {
        for (size_t i = 0; i < d_workers.size(); ++i) {
            d_workers[i]->submit(aJob);
        }
    }

    // ACCESSORS
    bsl::size_t size() const
    {
        return d_workers.size();
    }
};

}  // close namespace groups

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int test          = argc > 1 ? atoi(argv[1]) : 0;
    const int threadsAmount = argc > 2 ? atoi(argv[2]) : 24;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);

    switch (test) { case 0:
      case 13: {
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

        double inputA[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0 };
        double inputB[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0 };
        double result[sizeof inputA / sizeof *inputA];

        const int NUM_ELEM = sizeof inputA / sizeof *inputA;
        FixedThreadPool pool;

        for (int jobs = 1; jobs < NUM_ELEM + 1; ++jobs) {
            parallelVectorSum(result, inputA, inputB, NUM_ELEM, &pool, jobs);
            for (int i = 0; i < NUM_ELEM; ++i) {
                ASSERT(result[i] = inputA[i] + inputB[i]);

            }
            if (veryVerbose) {
                for (int i = 0; i < NUM_ELEM; ++i) {
                    cout << result[i] << " ";
                }
                cout << endl;
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // PRODUCERS(C-D), CONSUMERS AND PRODUCERS-CONSUMERS
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
        //: 3 The third group will call 'arriveWait()'.
        //:
        //: 4 Verify that the operation produces the expected result (see the
        //:   test plan overview).  (C-1..3)
        //
        // Testing:
        //   void countDown(int n);
        //   void wait();
        //   void arriveAndWait();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PRODUCERS(C-D), CONSUMERS AND PRODUCERS-CONSUMERS"
                 << endl
                 << "================================================="
                 << endl;

        for (int i = 1; i <= threadsAmount 3; ++i) {
            for (int j = 1; j <= threadsAmount 3; ++j) {
                for (int k = 1; k <= threadsAmount 3; ++k) {
                    groups::test<groups::ThreadProducerCountDown,
                                 groups::ThreadConsumer,
                                 groups::ThreadProducerConsumer>(i, j, k);
                }
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // PRODUCERS(ARRIVE), CONSUMERS AND PRODUCERS-CONSUMERS
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
        //: 4 The third group will call 'arriveWait()'.
        //:
        //: 5 Verify that the operation produces the expected result (see the
        //:   test plan overview).  (C-1..3)
        //
        // Testing:
        //   void arrive();
        //   void wait();
        //   void arriveAndWait();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PRODUCERS(ARRIVE), CONSUMERS AND PRODUCERS-CONSUMERS"
                 << endl
                 << "===================================================="
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
      case 10: {
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
        //   void countDown(int n);
        //   void wait();
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
      case 9: {
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
        //   void arrive();
        //   void wait();
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
      case 8: {
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
        //   void arriveAndWait();
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
      case 7: {
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
        //   void countDown(int n);
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
      case 6: {
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
        //   void arrive();
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
      case 5: {
        // --------------------------------------------------------------------
        // WAIT
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
                          << "WAIT" << endl
                          << "====" << endl;

        bslmt::Latch myLatch(0);
        myLatch.wait();

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ARRIVE AND WAIT
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
                          << "ARRIVE AND WAIT" << endl
                          << "===============" << endl;

        bslmt::Latch myLatch(1);
        myLatch.arriveAndWait();
        {
            const int myCount = myLatch.currentCount();
            ASSERTV(0, myCount, 0 == myCount);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING: 'tryWait'
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
        //   void countDown(int n);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESITNG: 'tryWait'"
                          << "==================" << endl;

        {
            {
                Obj x(5);  const Obj& X = x;

                ASSERT(false == X.tryWait());
                x.countDown(1);
                ASSERT(false == X.tryWait());
                x.countDown(4);
                ASSERT(true == X.tryWait());
            }
            {
                Obj x(0);  const Obj& X = x;

                ASSERT(true == X.tryWait());
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ARRIVE
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
                          << "ARRIVE" << endl
                          << "======" << endl;

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
        // TESTING: DEFAULT CTOR, DTOR, AND PRIMARY MANIPULATORS
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
             << "TESTING DEFAULT CTOR, DTOR, AND PRIMARY MANIPULATORS" << endl
             << "====================================================" << endl;

        if (verbose) cout << "\nCheck value constructor." << endl;

        for (int i = 0; i < 10; ++i) {
            Obj x(i);  const Obj& X = x;

            ASSERT(i == X.currentCount());
        }

        if (verbose) cout << "\nCheck 'countDown'." << endl;

        for (int stepSize = 1; stepSize < 10; ++stepSize) {
            for (int initialCount = 1; initialCount < 100; ++initialCount) {
                Obj x(initialCount);  const Obj &X = x;
                int count = initialCount;
                while (count > 0) {
                    int step = bsl::min(stepSize, count);
                    count -= step;
                    x.countDown(step);

                    ASSERT(count == X.currentCount());
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'CTOR'" << endl;
            {
                ASSERT_PASS(Obj(1));
                ASSERT_PASS(Obj(0));
                ASSERT_FAIL_RAW(Obj(-1));
            }

            if (veryVerbose) cout << "\t'countDown'" << endl;
            {
                Obj x(10);  const Obj& X = x;

                ASSERT_PASS(x.countDown(1));
                ASSERT_FAIL(x.countDown(-1));
                ASSERT_FAIL(x.countDown(10));
            }
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the default allocator.

    LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                0 == defaultAllocator.numBlocksTotal());

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
