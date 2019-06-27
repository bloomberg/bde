// bslmt_throughputbenchmark.t.cpp                                    -*-C++-*-

#include <bslmt_throughputbenchmark.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmt_condition.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bslmt_throughputbenchmark.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_atomic.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_queue.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <stddef.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a mechanism,
// 'bslmt::ThroughputBenchmark', that provides performance testing for multi-
// threaded components.  The results can be parsed with
// 'bslmt::ThroughputBenchmarkResult', which represents counts of the work done
// by each thread, thread group, and sample, divided by the number of actual
// seconds that sample has executed.  The primary manipulators are the methods
// for adding thread groups ('addThreadGroup') and executing a benchmark
// ('execute').  The provided basic accessors are the methods for obtaining the
// allocator ('allocator'), the number of thread groups added, the number of
// threads in each thread group ('numThreads'), and a convenience function with
// the total number of threads ('numThreads').  There are also utility
// functions to simulate load ('busyWorkAmount') and to estimate the load
// ('estimateBusyWorkAmount').
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
// ----------------------------------------------------------------------------
// [ 5] unsigned int antiOptimization();
// [ 5] void busyWork(Int64 busyWorkAmount);
// [ 5] Int64 estimateBusyWorkAmount(TimeInterval interval);
// [ 1] ThroughputBenchmark(bslma::Allocator *basicAllocator = 0);
// [ 2] int addThreadGroup(runF, numThreads, workAmount);
// [ 2] int addThreadGroup(runF, numThreads, workAmount, initF, cleanupF);
// [ 4] void execute(result, millis, numSamples);
// [ 4] void execute(result, millis, numSamples, initF, cleanupF);
// [ 3] int numThreads() const;
// [ 3] int numThreadGroups() const;
// [ 3] int numThreadsInGroup(int threadGroupIndex) const;
// [ 3] bslma::Allocator *allocator() const;
// ----------------------------------------------------------------------------
// [ 6] USAGE EXAMPLE
// ----------------------------------------------------------------------------

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

#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Test Performance of bsl::queue<int>
/// - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we test the throughput of a 'bsl::queue<int>' in a
// multi-threaded environment, where multiple "producer" threads are pushing
// elements, and multiple "consumer" threads are popping these elements.
//
// First, we define a global queue, a mutex to protect this queue, and a
// semaphore for a "pop" operation to block on:
//..
    bsl::queue<int>  myQueue;
    bslmt::Mutex     myMutex;
    bslmt::Semaphore mySem;
//..
// Next, we define a counter value we push in:
//..
    int              counterValue = 0;
//..
// Then, we define simple push and pop functions that manipulate this queue:
//..
    void myPush(int threadIndex)
        // Push an element into 'myQueue', using the specified 'threadIndex'.
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&myMutex);
        myQueue.push(1000000 * threadIndex + counterValue++);
        mySem.post();
    }

    void myPop(int)
        // Pop an element from 'myQueue'.
    {
        mySem.wait();
        bslmt::LockGuard<bslmt::Mutex> guard(&myMutex);
        myQueue.pop();
    }
//..
// Next, we define a thread "cleanup" function for the push thread group, which
// pushes a couple of extra elements to make sure that the pop thread group
// will not hang on an empty queue:
//..
    void myCleanup()
        // Cleanup function.
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&myMutex);
        for (int i = 0; i < 10; ++i) {
            myQueue.push(counterValue++);
            mySem.post();
        }
    }
//..

// ============================================================================
//                   GLOBAL STRUCTS/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

                             // ===============
                             // SetValueFunctor
                             // ===============

class SetValueFunctor {
    // This class provides a functor setting a global value to the constructor
    // parameter.

    // CLASS DATA
    static int s_value;  // Value getting set.

    // DATA
    int d_value;

  public:
    // CREATORS
    explicit SetValueFunctor(int value);
        // Create 'SetValueFunctor' object with the specified 'value'.

    // ~SetValueFunctor() = default;
        // Destroy this object.

    // MANIPULATORS
    void operator()(int);
        // Set static value to the constructor parameter
};

                             // ---------------
                             // SetValueFunctor
                             // ---------------

int SetValueFunctor::s_value = 0;

// CREATORS
SetValueFunctor::SetValueFunctor(int value)
: d_value(value)
{
}

// MANIPULATORS
void SetValueFunctor::operator()(int)
{
    s_value = d_value;
}

                               // ===========
                               // InitFunctor
                               // ===========

class InitFunctor {
    // This class provides a functor setting a global value to the constructor
    // parameter.

    // CLASS DATA
    static int s_value;  // Value getting set.

    // DATA
    int d_value;

  public:
    // CREATORS
    explicit InitFunctor(int value);
        // Create an 'InitiFunctor' object with the specified 'value'.

    // ~InitFunctor() = default;
        // Destroy this object.

    // MANIPULATORS
    void operator()();
        // Set static value to the constructor parameter
};

                               // -----------
                               // InitFunctor
                               // -----------

int InitFunctor::s_value = 0;

// CREATORS
InitFunctor::InitFunctor(int value)
: d_value(value)
{
}

// MANIPULATORS
void InitFunctor::operator()()
{
    s_value = d_value;
}

                            // =================
                            // CountNoParFunctor
                            // =================

class CountNoParFunctor {
    // This class counts the number of invocations of its function call
    // operator.  It uses an atomic integer addressed by the 'count' supplied
    // on construction to store the count.

  private:
    // DATA
    bsls::AtomicInt *d_count_p;
        // Counter.

  public:
    // CREATORS
    explicit CountNoParFunctor(bsls::AtomicInt *count);
        // Create a 'CountNoParFunctor' object with the specified 'count'
        // counter set to 0.

    // ~CountNoParFunctor() = default;
        // Destroy this object.

    // MANIPULATORS
    void operator()();
        // Increase count.
};

                            // -----------------
                            // CountNoParFunctor
                            // -----------------

// CREATORS
CountNoParFunctor::CountNoParFunctor(bsls::AtomicInt *count)
: d_count_p(count)
{
    *d_count_p = 0;
}

// MANIPULATORS
void CountNoParFunctor::operator()()
{
    ++(*d_count_p);
}

                         // ========================
                         // ThreadCountIntParFunctor
                         // ========================

class ThreadCountIntParFunctor {
    // This class counts the number of unique threads that invoke its function
    // call operator.  It uses an atomic integer addressed by the 'count'
    // supplied on construction to store the count.

  private:
    // DATA
    bsls::AtomicInt        *d_count_p;
        // Counter.

    bslmt::ThreadUtil::Key  d_tlsKey;
        // TLS key

  public:
    // CREATORS
    explicit ThreadCountIntParFunctor(bsls::AtomicInt *count);
        // Create a 'CountNoParFunctor' object with the specified 'count'
        // counter set to 0.

    ThreadCountIntParFunctor(const ThreadCountIntParFunctor& orig);
        // Create a 'ThreadCountIntParFunctor' object with the value of the
        // specified 'orig'.

    ~ThreadCountIntParFunctor();
        // Destroy this object.

    // MANIPULATORS
    void operator()(int);
        // Increase count.  The parameter is ignored.
};

                         // ------------------------
                         // ThreadCountIntParFunctor
                         // ------------------------

// CREATORS
ThreadCountIntParFunctor::ThreadCountIntParFunctor(bsls::AtomicInt *count)
: d_count_p(count)
{
    *d_count_p = 0;
    bslmt::ThreadUtil::createKey(&d_tlsKey, NULL);
}

ThreadCountIntParFunctor::ThreadCountIntParFunctor(
                                          const ThreadCountIntParFunctor& orig)
{
    d_count_p = orig.d_count_p;
    bslmt::ThreadUtil::createKey(&d_tlsKey, NULL);
}

ThreadCountIntParFunctor::~ThreadCountIntParFunctor()
{
    bslmt::ThreadUtil::deleteKey(d_tlsKey);
}

// MANIPULATORS
void ThreadCountIntParFunctor::operator()(int)
{
    void *value = bslmt::ThreadUtil::getSpecific(d_tlsKey);
    if (!value) {
        ++(*d_count_p);
        bslmt::ThreadUtil::setSpecific(d_tlsKey, d_count_p);
    }
}

                           // ===================
                           // CountBoolParFunctor
                           // ===================

class CountBoolParFunctor {
    // This class separately counts the number of times its function call
    // operator is invoked with the values 'true' and 'false'.

  private:
    // DATA
    bsls::AtomicInt *d_countTrue_p;
        // Counter for calls with 'true'.

    bsls::AtomicInt *d_countFalse_p;
        // Counter for calls with 'false'.

  public:
    // CREATORS
    CountBoolParFunctor(bsls::AtomicInt *countTrue,
                        bsls::AtomicInt *countFalse);
        // Create a 'CountBoolParFunctor' object with the specified counters
        // 'countTrue' and 'countFalse' set to 0.

    // ~CountBoolParFunctor() = default;
        // Destroy this object.

    // MANIPULATORS
    void operator()(bool parameter);
        // Increase count of the 'true' functor calls if the specified
        // 'parameter' is 'true', and the count of 'false' functor calls
        // otherwise.
};

                           // -------------------
                           // CountBoolParFunctor
                           // -------------------

// CREATORS
CountBoolParFunctor::CountBoolParFunctor(bsls::AtomicInt *countTrue,
                                         bsls::AtomicInt *countFalse)
: d_countTrue_p (countTrue)
, d_countFalse_p(countFalse)
{
    *d_countTrue_p  = 0;
    *d_countFalse_p = 0;
}

// MANIPULATORS
void CountBoolParFunctor::operator()(bool parameter)
{
    if (parameter) {
        ++(*d_countTrue_p);
    } else {
        ++(*d_countFalse_p);
    }
}

}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocatorRaw(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
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

// Then, we create a 'bslmt::ThroughputBenchmark' object and add push and pop
// thread groups, each with 2 threads and a work load (arithmetic operations to
// consume an amount of time) of 100:
//..
    bslmt::ThroughputBenchmark myBench;
    myBench.addThreadGroup(
                        myPush,
                        2,
                        100,
                        bslmt::ThroughputBenchmark::InitializeThreadFunction(),
                        myCleanup);
    const int consumerGroupIdx = myBench.addThreadGroup(myPop, 2, 100);
//..
// Now, we create a 'bslmt::ThroughputBenchmarkResult' object to contain the
// result, and call 'execute' to run the benchmark for 500 millseconds 10
// times:
//..
    bslmt::ThroughputBenchmarkResult myResult;
    myBench.execute(&myResult, 500, 10);
//..
// Finally, we print the median of the throughput of the consumer thread group.
//..
    double median;
    myResult.getMedian(&median, consumerGroupIdx);
    bsl::cout << "Throughput:" << median << "\n";
//..

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TEST BUSY WORK
        //   This is problematic to calculate, as it is all based on timing,
        //   which differs between hosts and between different time slots.
        //
        // Concerns:
        //: 1 A larger time interval provided to 'estimateBusyWorkAmount' will
        //:   result in a larger 'busyWorkAmount' value.
        //:
        //: 2 A larger 'busyWorkAmount' value provided to 'busyWork' will
        //:   result in a larger elapsed time.
        //:
        //: 3 The 'busyWorkAmount' value will produce a time interval in line
        //:   with the one provided to 'estimateBusyWorkAmount'.
        //:
        //: 4 'estimateBusyWorkAmount' and 'busyWork' do not allocate any
        //:   memory.
        //
        // Plan:
        //: 1 Run 'estimateBusyWorkAmount' with a significant interval (0.5sec)
        //:   and again with double that (1sec).  Verify that the resulting
        //:   'busyWorkAmount' is larger.
        //:
        //: 2 Run 'busyWork' with the outputs of #1, and verify that the
        //:   elapsed time on the longer run is bigger than the elapsed time on
        //:   the shorter run.
        //:
        //: 3 Verify that the time intervals resulting from #2 are between
        //:   50% and 200% of the original time interval entered in #1.
        //:
        //: 4 There is no memory allocated on the default or global allocators.
        //
        // Testing:
        //   void busyWork(Int64 busyWorkAmount);
        //   Int64 estimateBusyWorkAmount(TimeInterval interval);
        //   unsigned int antiOptimization();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST BUSY WORK" << endl
                          << "==============" << endl;

        typedef bslmt::ThroughputBenchmark Obj;

        bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
        (void)allocations;

        bsls::TimeInterval loTime(0.5);
        bsls::TimeInterval hiTime(1.0);

        bsls::Types::Int64 loAmount = Obj::estimateBusyWorkAmount(loTime);
        bsls::Types::Int64 hiAmount = Obj::estimateBusyWorkAmount(hiTime);

        BSLS_ASSERT(hiAmount > loAmount);

        bsls::TimeInterval startTime = bsls::SystemTime::nowMonotonicClock();
        Obj::busyWork(loAmount);
        bsls::TimeInterval endTime   = bsls::SystemTime::nowMonotonicClock();
        bsls::TimeInterval duration  = endTime - startTime;
        double             loSeconds = duration.totalSecondsAsDouble();
        (void)loSeconds;

        startTime = endTime;
        unsigned int beforeVal = Obj::antiOptimization();
        (void)beforeVal;
        Obj::busyWork(hiAmount);
        unsigned int afterVal  = Obj::antiOptimization();
        (void)afterVal;
        endTime   = bsls::SystemTime::nowMonotonicClock();
        duration  = endTime - startTime;
        double hiSeconds = duration.totalSecondsAsDouble();
        (void)hiSeconds;

        BSLS_ASSERT(hiSeconds >  loSeconds);
        BSLS_ASSERT(loSeconds >= 0.1 );
        BSLS_ASSERT(loSeconds <= 1.0 );
        BSLS_ASSERT(hiSeconds >= 0.5 );
        BSLS_ASSERT(hiSeconds <= 2.0 );
        BSLS_ASSERT(beforeVal != afterVal);

        BSLS_ASSERT(defaultAllocator.numAllocations() == allocations);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST 'execute'
        //
        // Concerns:
        //: 1 The 'result' object is created with the right size - numSamples,
        //:   numThreadGroups, and number of threads in each of the thread
        //:   groups.
        //:
        //: 2 'execute' does not allocate memory, except within the 'result'
        //:   object, and within the run, init, and cleanup functions.
        //:
        //: 3 For each previously defined thread group, run function is called
        //:   exactly 'numSamples' x <number of threads in that thread group>
        //:   times.
        //:
        //: 4 For each previously defined thread group, if init and cleanup
        //:   functions are defined, they are called exactly 'numSamples' x
        //:   <number of threads in that thread group> times.
        //:
        //: 5 If the sample level init and cleanup functions are provided (with
        //:   the 5-arg 'execute') they are called exactly 'numSamples' times.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object using a supplied test allocator.
        //:
        //: 2 Call method 3-arg 'addThreadGroup' with a counting run function
        //:   and certain number of threads and work amount twice. Then call
        //:   method 3-arg 'execute' with a 'result' object created with a
        //:   different supplied test allocator.  Verify:
        //:   1 Dimensions of 'result' object.
        //:   2 Count of times run function was called for each of the thread
        //:     groups.
        //:   3 No memory was allocated except on either of the supplied test
        //:     allocators.
        //:   4 During 'execute', only the second supplied test allocator was
        //:     used.
        //:
        //: 3 Call method 5-arg 'addThreadGroup' with a counting run function
        //:   and certain number of threads and work amount twice. Then call
        //:   method 5-arg 'execute'.  Verify:
        //:   1 Dimensions of 'result' object.
        //:   2 Count of times run function was called for each of the thread
        //:     groups.
        //:   3 Count of times thread level init and cleanup function were
        //:     called.
        //:   4 Count of times sample level init and cleanup functions were
        //:     called.
        //:   5 No memory was allocated except on either of the supplied test
        //:     allocators.
        //:   6 During 'execute', only the second supplied test allocator was
        //:     used.
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid indexes, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //
        // Testing:
        //   void execute(result, millis, numSamples);
        //   void execute(result, millis, numSamples, initF, cleanupF);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST 'execute'" << endl
                          << "==============" << endl;

        typedef bslmt::ThroughputBenchmark          Obj;
        typedef bslmt::ThroughputBenchmark_TestUtil TestUtil;

        if (verbose) cout << "\nTesting 3-arg 'execute'." << endl;
        {
            bslma::TestAllocator supplied ("supplied" , veryVeryVeryVerbose);
            bslma::TestAllocator supplied2("supplied2", veryVeryVeryVerbose);

            bsls::AtomicInt           cnt1, cnt2;
            ThreadCountIntParFunctor  cnt1Functor(&cnt1);
            ThreadCountIntParFunctor  cnt2Functor(&cnt2);

            Obj      mX(&supplied);  const Obj& X = mX; (void)X;
            TestUtil test(mX);

            int ret = mX.addThreadGroup(cnt1Functor, 10, 100);
            (void)ret;
            BSLS_ASSERT(0 == ret);
            bsls::Types::Int64 sAllocations = supplied.numAllocations();
            (void)sAllocations;
            BSLS_ASSERT(1  == X.numThreadGroups());
            BSLS_ASSERT(10 == X.numThreadsInGroup(ret));
            BSLS_ASSERT(10 == X.numThreads());

            BSLS_ASSERT(sAllocations == supplied.numAllocations());

            ret = mX.addThreadGroup(cnt2Functor, 5, 200);
            BSLS_ASSERT(1 == ret);
            sAllocations = supplied.numAllocations();
            BSLS_ASSERT(2  == X.numThreadGroups());
            BSLS_ASSERT(5  == X.numThreadsInGroup(ret));
            BSLS_ASSERT(15 == X.numThreads());

            BSLS_ASSERT(sAllocations == supplied.numAllocations());

            bslmt::ThroughputBenchmarkResult result(&supplied2);

            mX.execute(&result, 500, 10);
            BSLS_ASSERT(10 == result.numSamples());
            BSLS_ASSERT(2  == result.numThreadGroups());
            BSLS_ASSERT(10 == result.numThreads(0));
            BSLS_ASSERT(5  == result.numThreads(1));
            BSLS_ASSERT(15 == result.totalNumThreads());

            BSLS_ASSERT(sAllocations == supplied.numAllocations());

            // Verify counts
            BSLS_ASSERT(10 * 10 == cnt1.load());
            BSLS_ASSERT( 5 * 10 == cnt2.load());
        }

        if (verbose) cout << "\nTesting 5-arg 'execute'." << endl;
        {
            bslma::TestAllocator supplied ("supplied" , veryVeryVeryVerbose);
            bslma::TestAllocator supplied2("supplied2", veryVeryVeryVerbose);

            bsls::AtomicInt           cnt1, cnt2;
            ThreadCountIntParFunctor  cnt1Functor(&cnt1);
            ThreadCountIntParFunctor  cnt2Functor(&cnt2);

            bsls::AtomicInt     cntTrueInit, cntFalseInit;
            bsls::AtomicInt     cntTrueClean, cntFalseClean;
            CountBoolParFunctor initFunctor(&cntTrueInit, &cntFalseInit);
            CountBoolParFunctor cleanFunctor(&cntTrueClean, &cntFalseClean);
            bsls::AtomicInt     cntThread1Init, cntThread2Init;
            bsls::AtomicInt     cntThread1Clean, cntThread2Clean;
            CountNoParFunctor   initThread1Functor(&cntThread1Init);
            CountNoParFunctor   initThread2Functor(&cntThread2Init);
            CountNoParFunctor   cleanThread1Functor(&cntThread1Clean);
            CountNoParFunctor   cleanThread2Functor(&cntThread2Clean);

            Obj      mX(&supplied);  const Obj& X = mX; (void)X;
            TestUtil test(mX);

            int ret = mX.addThreadGroup(cnt1Functor,
                                        10,
                                        100,
                                        initThread1Functor,
                                        cleanThread1Functor);
            (void)ret;
            BSLS_ASSERT(0 == ret);
            bsls::Types::Int64 sAllocations = supplied.numAllocations();
            (void)sAllocations;
            BSLS_ASSERT(1  == X.numThreadGroups());
            BSLS_ASSERT(10 == X.numThreadsInGroup(ret));
            BSLS_ASSERT(10 == X.numThreads());

            BSLS_ASSERT(sAllocations == supplied.numAllocations());

            ret = mX.addThreadGroup(cnt2Functor,
                                    5,
                                    200,
                                    initThread2Functor,
                                    cleanThread2Functor);
            BSLS_ASSERT(1 == ret);
            sAllocations = supplied.numAllocations();
            BSLS_ASSERT(2  == X.numThreadGroups());
            BSLS_ASSERT(5  == X.numThreadsInGroup(ret));
            BSLS_ASSERT(15 == X.numThreads());

            BSLS_ASSERT(sAllocations == supplied.numAllocations());

            bslmt::ThroughputBenchmarkResult result(&supplied2);

            mX.execute(&result, 500, 10, initFunctor, cleanFunctor);
            BSLS_ASSERT(10 == result.numSamples());
            BSLS_ASSERT(2  == result.numThreadGroups());
            BSLS_ASSERT(10 == result.numThreads(0));
            BSLS_ASSERT(5  == result.numThreads(1));
            BSLS_ASSERT(15 == result.totalNumThreads());

            BSLS_ASSERT(sAllocations == supplied.numAllocations());

            // Verify counts
            if (10 * 10 != cnt1) {
                abort();
            }
            BSLS_ASSERT(10 * 10 == cnt1);
            BSLS_ASSERT( 5 * 10 == cnt2);
            BSLS_ASSERT( 1      == cntTrueInit);
            BSLS_ASSERT( 9      == cntFalseInit);
            BSLS_ASSERT( 1      == cntTrueClean);
            BSLS_ASSERT( 9      == cntFalseClean);
            BSLS_ASSERT(10 * 10 == cntThread1Init);
            BSLS_ASSERT(10 * 10 == cntThread1Clean);
            BSLS_ASSERT( 5 * 10 == cntThread2Init);
            BSLS_ASSERT( 5 * 10 == cntThread2Clean);
        }

        if (verbose) cout << "Negative Testing" << endl;
        {
            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

            bsls::AtomicInt     cntTrueInit , cntFalseInit;
            bsls::AtomicInt     cntTrueClean, cntFalseClean;
            CountBoolParFunctor initFunctor(&cntTrueInit, &cntFalseInit);
            CountBoolParFunctor cleanFunctor(&cntTrueClean, &cntFalseClean);
            SetValueFunctor     setValueFunctor(4);

            Obj mX(&supplied);

            mX.addThreadGroup(setValueFunctor, 1,  0);
            mX.addThreadGroup(setValueFunctor, 1,  0);

            bsls::AssertTestHandlerGuard hG;

            bslmt::ThroughputBenchmarkResult result;

            ASSERT_FAIL(mX.execute(0      , 1, 1));
            ASSERT_FAIL(mX.execute(&result, 0, 1));
            ASSERT_FAIL(mX.execute(&result, 1, 0));
            ASSERT_PASS(mX.execute(&result, 1, 1));

            ASSERT_FAIL(mX.execute(0      , 1, 1, initFunctor, cleanFunctor));
            ASSERT_FAIL(mX.execute(&result, 0, 1, initFunctor, cleanFunctor));
            ASSERT_FAIL(mX.execute(&result, 1, 0, initFunctor, cleanFunctor));
            ASSERT_PASS(mX.execute(&result, 1, 1, initFunctor, cleanFunctor));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute of
        //:   the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates any memory.
        //
        // Plan:
        //: 1 To test 'allocator', create object with various allocators and
        //:   ensure the returned value matches the supplied allocator.
        //:
        //: 2 Use 'addThreadGroup' to populate an object, and verify the
        //:   return value of the accessor against expected values.  (C-1)
        //:
        //: 3 The accessors will only be accessed from a 'const' reference to
        //:   the created object.  (C-2)
        //:
        //: 4 A supplied allocator will be used for all created objects
        //:   (excluding those used to test 'allocator') and the number of
        //:   allocation will be verified to ensure that no memory was
        //:   allocated during use of the accessors.  (C-3)
        //
        // Testing:
        //   bslma::Allocator *allocator() const;
        //   int numThreadGroups() const;
        //   int numThreadsInGroup(int threadGroupIndex) const;
        //   int numThreads() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        typedef bslmt::ThroughputBenchmark          Obj;
        typedef bslmt::ThroughputBenchmark_TestUtil TestUtil;

        if (verbose) cout << "\nTesting 'allocator'." << endl;
        {
            Obj mX;  const Obj& X = mX; (void)X;
            BSLS_ASSERT(&defaultAllocator == X.allocator());
        }
        {
            Obj        mX(0);
            const Obj& X = mX; (void)X;
            BSLS_ASSERT(&defaultAllocator == X.allocator());
        }
        {
            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

            Obj mX(&supplied);  const Obj& X = mX; (void)X;
            BSLS_ASSERT(&supplied == X.allocator());
        }

        if (verbose) cout << "\nTesting 'numThread*'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            (void)allocations;

            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

            Obj      mX(&supplied);  const Obj& X = mX; (void)X;
            TestUtil test(mX);

            int ret = mX.addThreadGroup(Obj::RunFunction(), 10, 1000);
            (void)ret;
            BSLS_ASSERT(0 == ret);
            bsls::Types::Int64 sAllocations = supplied.numAllocations();
            (void)sAllocations;
            BSLS_ASSERT(1  == X.numThreadGroups());
            BSLS_ASSERT(10 == X.numThreadsInGroup(ret));
            BSLS_ASSERT(10 == X.numThreads());

            BSLS_ASSERT(test.threadGroups().size() == 1);
            BSLS_ASSERT(test.threadGroups()[0].d_numThreads == 10);
            BSLS_ASSERT(allocations  == defaultAllocator.numAllocations());
            BSLS_ASSERT(sAllocations == supplied.numAllocations());

            SetValueFunctor setValueFunctor(1);

            ret = mX.addThreadGroup(setValueFunctor, 5, 200);
            BSLS_ASSERT(1 == ret);
            sAllocations = supplied.numAllocations();
            BSLS_ASSERT(2  == X.numThreadGroups());
            BSLS_ASSERT(5  == X.numThreadsInGroup(ret));
            BSLS_ASSERT(15 == X.numThreads());

            BSLS_ASSERT(test.threadGroups().size() == 2);
            BSLS_ASSERT(test.threadGroups()[1].d_numThreads == 5);
            // pre-c++11 platoforms has allocating 'bsl::function'
#if __cplusplus >= 201103L
            BSLS_ASSERT(allocations  == defaultAllocator.numAllocations());
#endif
            BSLS_ASSERT(sAllocations == supplied.numAllocations());

            ret = mX.addThreadGroup(setValueFunctor, 1, 0);
            BSLS_ASSERT(2 == ret);
            sAllocations = supplied.numAllocations();
            BSLS_ASSERT(3  == X.numThreadGroups());
            BSLS_ASSERT(1  == X.numThreadsInGroup(ret));
            BSLS_ASSERT(16 == X.numThreads());

            BSLS_ASSERT(test.threadGroups().size() == 3);
            BSLS_ASSERT(test.threadGroups()[2].d_numThreads == 1);
            // pre-c++11 platoforms has allocating 'bsl::function'
#if __cplusplus >= 201103L
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());
#endif
            BSLS_ASSERT(sAllocations == supplied.numAllocations());

            // Verify that the number of threads in the previous thread groups
            // did not change
            BSLS_ASSERT(10 == X.numThreadsInGroup(0));
            BSLS_ASSERT(5  == X.numThreadsInGroup(1));
        }

        if (verbose) cout << "Negative Testing" << endl;
        {
            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

            SetValueFunctor setValueFunctor(3);

            Obj mX(&supplied); const Obj& X = mX;

            mX.addThreadGroup(setValueFunctor, 1,  0);
            mX.addThreadGroup(setValueFunctor, 1,  0);

            bsls::AssertTestHandlerGuard hG;

            ASSERT_FAIL(X.numThreadsInGroup(-1));
            ASSERT_FAIL(X.numThreadsInGroup(2));
            ASSERT_PASS(X.numThreadsInGroup(0));
            ASSERT_PASS(X.numThreadsInGroup(1));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST 'addThreadGroup'
        //
        // Concerns:
        //: 1 The method 3-argument 'addThreadGroup' adds a thread group, has
        //:   the correct number of threads, the correct work amount, the
        //    correct run function, and that initialize and cleanup functions
        //:   are missing.
        //:
        //: 2 The method 3-argument 'addThreadGroup' does not affect allocated
        //:   memory, and is exception neutral with respect to memory
        //:   allocation.
        //:
        //: 3 The method 5-argument 'addThreadGroup' adds a thread group, has
        //:   the correct number of threads, the correct work amount, the
        //:   correct run function, and that initialize and cleanup functions
        //:   are present.
        //:
        //: 4 The method 5-argument 'addThreadGroup' does not
        //:   affect allocated memory, and is exception neutral with respect to
        //:   memory allocation.
        //:
        //: 5 Memory is not leaked by any method and the destructor properly
        //:   deallocates the residual allocated memory.
        //:
        //: 6 QoI: There is no temporary memory allocation from any allocator.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object using a supplied test allocator.
        //:
        //: 2 Call method 3-arg 'addThreadGroup' with an empty run function and
        //:   certain number of threads and work amount. Verify:
        //:   1 Number of thread groups is 1.
        //:   2 The return code is 0 (the first thread group).
        //:   3 The number of threads in total is as expected.
        //:   4 The number of threads in the first thread group is as expected.
        //:   5 There is no run function.
        //:   6 There are no init and cleanup functions.
        //:
        //: 3 Call method 3-arg 'addThreadGroup' again with a non-empty run
        //:   function, and redo the verification.
        //:
        //: 4 Call method 5-arg 'addThreadGroup' with an empty run and cleanup
        //:   functions, a certain number of threads and work amount, and a
        //:   non-empty init function.  Verify:
        //:   1 Number of thread groups is 1.
        //:   2 The return code is 0 (the first thread group).
        //:   3 The number of threads in total is as expected.
        //:   4 The number of threads in the first thread group is as expected.
        //:   5 There is no run function.
        //:   6 There is an init function.
        //:   6 There is no cleanup function.
        //:
        //: 5 Call method 5-arg 'addThreadGroup' again with a non-empty run
        //:   and init functions, and redo the verification.
        //:
        //: 6 When the object goes out of scope for both 3-arg and 5-arg
        //:   'addThreadGroup', there is no memory leak on the supplied
        //:   allocator.
        //:
        //: 7 There is no memory allocated on the default or global allocators.
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid indexes, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //
        // Testing:
        //   int addThreadGroup(runF, numThreads, workAmount);
        //   int addThreadGroup(runF, numThreads, workAmount, initF, cleanupF);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST 'addThreadGroup'" << endl
                          << "=====================" << endl;

        typedef bslmt::ThroughputBenchmark          Obj;
        typedef bslmt::ThroughputBenchmark_TestUtil TestUtil;

        if (verbose) cout << "\nTesting 3-arg 'addThreadGroup'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            (void)allocations;

            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

            Obj      mX(&supplied);  const Obj& X = mX;(void)X;
            TestUtil test(mX);

            int ret = mX.addThreadGroup(Obj::RunFunction(), 10, 1000);
            (void)ret;
            BSLS_ASSERT(0  == ret);
            BSLS_ASSERT(1  == X.numThreadGroups());
            BSLS_ASSERT(10 == X.numThreadsInGroup(ret));
            BSLS_ASSERT(10 == X.numThreads());

            BSLS_ASSERT(test.threadGroups().size() == 1);
            Obj::ThreadGroup& tg0 = test.threadGroups()[0];  (void)tg0;
            BSLS_ASSERT(tg0.d_numThreads == 10);
            BSLS_ASSERT(tg0.d_amount == 1000);
            BSLS_ASSERT(static_cast<bool>(tg0.d_initialize) == false);
            BSLS_ASSERT(static_cast<bool>(tg0.d_cleanup)    == false);
            BSLS_ASSERT(static_cast<bool>(tg0.d_func)       == false);
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());

            SetValueFunctor setValueFunctor(1);

            ret = mX.addThreadGroup(setValueFunctor, 5, 200);
            BSLS_ASSERT(1  == ret);
            BSLS_ASSERT(2  == X.numThreadGroups());
            BSLS_ASSERT(5  == X.numThreadsInGroup(ret));
            BSLS_ASSERT(15 == X.numThreads());

            BSLS_ASSERT(test.threadGroups().size() == 2);
            Obj::ThreadGroup& tg1 = test.threadGroups()[1];  (void)tg1;
            BSLS_ASSERT(tg1.d_numThreads == 5);
            BSLS_ASSERT(tg1.d_amount     == 200);
            BSLS_ASSERT(static_cast<bool>(tg1.d_initialize) == false);
            BSLS_ASSERT(static_cast<bool>(tg1.d_cleanup)    == false);
            BSLS_ASSERT(static_cast<bool>(tg1.d_func)       != false);
            // pre-c++11 platoforms has allocating 'bsl::function'
#if __cplusplus >= 201103L
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());
#endif
        }

        if (verbose) cout << "\nTesting 5-arg 'addThreadGroup'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            (void)allocations;

            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

            Obj      mX(&supplied);  const Obj& X = mX;(void)X;
            TestUtil test(mX);

            InitFunctor initFunctor(2);

            int ret = mX.addThreadGroup(Obj::RunFunction(),
                                        30,
                                        0,
                                        initFunctor,
                                        Obj::CleanupThreadFunction());
            (void)ret;
            BSLS_ASSERT(0  == ret);
            BSLS_ASSERT(1  == X.numThreadGroups());
            BSLS_ASSERT(30 == X.numThreadsInGroup(ret));
            BSLS_ASSERT(30 == X.numThreads());

            BSLS_ASSERT(test.threadGroups().size() == 1);
            Obj::ThreadGroup& tg0 = test.threadGroups()[0];  (void)tg0;
            BSLS_ASSERT(tg0.d_numThreads == 30);
            BSLS_ASSERT(tg0.d_amount     == 0);
            BSLS_ASSERT(static_cast<bool>(tg0.d_initialize) != false);
            BSLS_ASSERT(static_cast<bool>(tg0.d_cleanup)    == false);
            BSLS_ASSERT(static_cast<bool>(tg0.d_func)       == false);
            // pre-c++11 platoforms has allocating 'bsl::function'
#if __cplusplus >= 201103L
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());
#endif

            SetValueFunctor setValueFunctor(3);

            ret = mX.addThreadGroup(setValueFunctor,
                                    50,
                                    2000,
                                    initFunctor,
                                    initFunctor);
            BSLS_ASSERT(1  == ret);
            BSLS_ASSERT(2  == X.numThreadGroups());
            BSLS_ASSERT(50 == X.numThreadsInGroup(ret));
            BSLS_ASSERT(80 == X.numThreads());

            BSLS_ASSERT(test.threadGroups().size() == 2);
            Obj::ThreadGroup& tg1 = test.threadGroups()[1];  (void)tg1;
            BSLS_ASSERT(tg1.d_numThreads == 50);
            BSLS_ASSERT(tg1.d_amount == 2000);
            BSLS_ASSERT(static_cast<bool>(tg1.d_initialize) != false);
            BSLS_ASSERT(static_cast<bool>(tg1.d_cleanup)    != false);
            BSLS_ASSERT(static_cast<bool>(tg1.d_func)       != false);
            // pre-c++11 platoforms has allocating 'bsl::function'
#if __cplusplus >= 201103L
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());
#endif
        }

        if (verbose) cout << "\n'addThreadGroup' exception test." << endl;
        {
            SetValueFunctor setValueFunctor(3);
            InitFunctor     initFunctor(2);

            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            (void)allocations;

            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
                Obj mX(&supplied);  const Obj& X = mX; (void)X;
                BSLS_ASSERT(0 == X.numThreadGroups());

                for (int i = 1; i <= 10; ++i) {
                    int ret = mX.addThreadGroup(Obj::RunFunction(), 30, 0);
                    (void)ret;
                    BSLS_ASSERT(i - 1 == ret);
                    BSLS_ASSERT(i     == X.numThreadGroups());
                    if (veryVerbose) {
                        P_(i) P(ret)
                    }
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());

            // Do the allocating conversion (Sun and AIX) outside the exception
            // loop.
            bsl::function<void()> initFunc = initFunctor;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
                Obj mX(&supplied);  const Obj& X = mX; (void)X;
                BSLS_ASSERT(0 == X.numThreadGroups());

                for (int i = 1; i <= 10; ++i) {
                    int ret = mX.addThreadGroup(Obj::RunFunction(),
                                                30,
                                                0,
                                                initFunc,
                                                Obj::CleanupThreadFunction());
                    (void)ret;
                    BSLS_ASSERT(i - 1 == ret);
                    BSLS_ASSERT(i     == X.numThreadGroups());
                    if (veryVerbose) {
                        P_(i) P(ret)
                    }
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            // pre-c++11 platoforms has allocating 'bsl::function'
#if __cplusplus >= 201103L
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());
#endif
        }

        if (verbose) cout << "Negative Testing" << endl;
        {
            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

            SetValueFunctor setValueFunctor(3);
            InitFunctor     initFunctor(2);

            Obj mX(&supplied);

            bsls::AssertTestHandlerGuard hG;

            ASSERT_FAIL(mX.addThreadGroup(setValueFunctor, 0,  0));
            ASSERT_FAIL(mX.addThreadGroup(setValueFunctor, 1, -1));
            ASSERT_PASS(mX.addThreadGroup(setValueFunctor, 1,  0));

            ASSERT_FAIL(mX.addThreadGroup(setValueFunctor,
                                          0,
                                          0,
                                          initFunctor,
                                          initFunctor));
            ASSERT_FAIL(mX.addThreadGroup(setValueFunctor,
                                          1,
                                          -1,
                                          initFunctor,
                                          initFunctor));
            ASSERT_PASS(mX.addThreadGroup(setValueFunctor,
                                          1,
                                          0,
                                          initFunctor,
                                          initFunctor));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR
        //
        // Concerns:
        //: 1 The default constructor creates the correct initial value and has
        //:   the internal memory management system hooked up properly so that
        //:   *all* internally allocated memory draws from the same
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 2 The method 'addThreadGroup' uses the allocator specified with the
        //:   constructor.  Note that 'addThreadGroup' is not tested here, and
        //:   is used merely to test memory allocation.
        //:
        //: 3 Memory is not leaked by any method and the (default) destructor
        //:   properly deallocates the residual allocated memory.
        //
        // Plan:
        //: 1 Create an object using the default constructor with and without
        //:   passing in an allocator, verify the allocator is stored using the
        //:   (untested) 'allocator' accessor, and verifying all allocations
        //:   are done from the allocator in future tests.
        //:
        //: 2 Create objects using the 'bslma::TestAllocator', use the
        //:   'addThreadGroup' method with various values, and the (untested)
        //:   accessors to verify the value of the object and that allocation
        //:   occurred when expected.
        //:
        //: 3 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is returned
        //:   to the allocator.  (C-3)
        //
        // Testing:
        //   ThroughputBenchmark(bslma::Allocator *basicAllocator = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONSTRUCTOR" << endl
                          << "===========" << endl;

        typedef bslmt::ThroughputBenchmark          Obj;
        typedef bslmt::ThroughputBenchmark_TestUtil TestUtil;

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            (void)allocations;

            Obj      mX;  const Obj& X = mX; (void)X;
            TestUtil test(mX);

            BSLS_ASSERT(&defaultAllocator == X.allocator());
            BSLS_ASSERT(0 == X.numThreadGroups());
            BSLS_ASSERT(0 == X.numThreads());
            BSLS_ASSERT(0 == test.state());
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());

            int ret = mX.addThreadGroup(Obj::RunFunction(), 2, 100);
            (void)ret;
            BSLS_ASSERT(0 == ret);
            BSLS_ASSERT(1 == X.numThreadGroups());
            BSLS_ASSERT(2 == X.numThreads());
            BSLS_ASSERT(0 == test.state());
            BSLS_ASSERT(allocations + 1 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            (void)allocations;

            Obj        mX(0);
            const Obj& X = mX; (void)X;
            TestUtil   test(mX);

            BSLS_ASSERT(&defaultAllocator == X.allocator());

            BSLS_ASSERT(0           == X.numThreadGroups());
            BSLS_ASSERT(0           == X.numThreads());
            BSLS_ASSERT(0           == test.state());
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());

            int ret = mX.addThreadGroup(Obj::RunFunction(), 3, 0);
            (void)ret;
            BSLS_ASSERT(0               == ret);
            BSLS_ASSERT(1               == X.numThreadGroups());
            BSLS_ASSERT(3               == X.numThreads());
            BSLS_ASSERT(0               == test.state());
            BSLS_ASSERT(allocations + 1 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            (void)allocations;

            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

            Obj      mX(&supplied);  const Obj& X = mX; (void)X;
            TestUtil test(mX);

            BSLS_ASSERT(&supplied   == X.allocator());
            BSLS_ASSERT(0           == X.numThreadGroups());
            BSLS_ASSERT(0           == X.numThreads());
            BSLS_ASSERT(0           == test.state());
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());
            bsls::Types::Int64 sAllocations = supplied.numAllocations();
            (void)sAllocations;

            int ret = mX.addThreadGroup(Obj::RunFunction(), 10, 1000);
            (void)ret;
            BSLS_ASSERT(0           == ret);
            BSLS_ASSERT(1           == X.numThreadGroups());
            BSLS_ASSERT(10          == X.numThreads());
            BSLS_ASSERT(0           == test.state());
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());
            BSLS_ASSERT(sAllocations < supplied.numAllocations());
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    if (test != 4 && test != 6) {
        LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                    0 == globalAllocator.numBlocksTotal());
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
