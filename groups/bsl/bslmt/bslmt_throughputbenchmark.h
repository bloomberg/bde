// bslmt_throughputbenchmark.h                                        -*-C++-*-

#ifndef INCLUDED_BSLMT_THROUGHPUTBENCHMARK
#define INCLUDED_BSLMT_THROUGHPUTBENCHMARK

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a performance test harness for multi-threaded components.
//
//@CLASSES:
//  bslmt::ThroughputBenchmark: multi-threaded performance test harness
//
//@DESCRIPTION: This component defines a mechanism,
// 'bslmt::ThroughputBenchmark', that provides performance testing for multi-
// threaded components.  The results are loaded into a
// 'bslmt::ThroughputBenchmarkResult' object, which provides access to counts
// of the work done by each thread, thread group, and sample, divided by the
// number of actual seconds of execution.
//
///Structure of a Test
///-------------------
// A test is composed from one or more thread groups, each running one or more
// threads.  Each thread in a thread group executes a thread function, with a
// simulated work load executing between subsequent calls to the thread
// function.  To provide reliability, the test is executed multiple times.  A
// single execution of a test is referred to as a "sample execution" and its
// result referred to as a "sample".  To support fine tuning of the test, it is
// possible to provide initialize and cleanup functions for a sample and / or a
// thread.
//
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
//  bsl::queue<int>  myQueue;
//  bslmt::Mutex     myMutex;
//  bslmt::Semaphore mySem;
//..
// Next, we define a counter value we push in:
//..
//  int              counterValue = 0;
//..
// Then, we define simple push and pop functions that manipulate this queue:
//..
//  void myPush(int threadIndex)
//      // Push an element into 'myQueue', using the specified 'threadIndex'.
//  {
//      bslmt::LockGuard<bslmt::Mutex> guard(&myMutex);
//      myQueue.push(1000000 * threadIndex + counterValue++);
//      mySem.post();
//  }
//
//  void myPop(int)
//      // Pop an element from 'myQueue'.
//  {
//      mySem.wait();
//      bslmt::LockGuard<bslmt::Mutex> guard(&myMutex);
//      myQueue.pop();
//  }
//..
// Next, we define a thread "cleanup" function for the push thread group, which
// pushes a couple of extra elements to make sure that the pop thread group
// will not hang on an empty queue:
//..
//  void myCleanup()
//      // Cleanup function.
//  {
//      bslmt::LockGuard<bslmt::Mutex> guard(&myMutex);
//      for (int i = 0; i < 10; ++i) {
//          myQueue.push(counterValue++);
//          mySem.post();
//      }
//  }
//..
// Then, we create a 'bslmt::ThroughputBenchmark' object and add push and pop
// thread groups, each with 2 threads and a work load (arithmetic operations to
// consume an amount of time) of 100:
//..
//  bslmt::ThroughputBenchmark myBench;
//  myBench.addThreadGroup(
//                      myPush,
//                      2,
//                      100,
//                      bslmt::ThroughputBenchmark::InitializeThreadFunction(),
//                      myCleanup);
//  const int consumerGroupIdx = myBench.addThreadGroup(myPop, 2, 100);
//..
// Now, we create a 'bslmt::ThroughputBenchmarkResult' object to contain the
// result, and call 'execute' to run the benchmark for 500 millseconds 10
// times:
//..
//  bslmt::ThroughputBenchmarkResult myResult;
//  myBench.execute(&myResult, 500, 10);
//..
// Finally, we print the median of the throughput of the consumer thread group.
//..
//  double median;
//  myResult.getMedian(&median, consumerGroupIdx);
//  bsl::cout << "Throughput:" << median << "\n";
//..

#include <bslscm_version.h>

#include <bslmt_barrier.h>
#include <bslmt_throughputbenchmarkresult.h>

#include <bslma_allocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_functional.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace bslmt {

class ThroughputBenchmark_TestUtil;

                        // =========================
                        // class ThroughputBenchmark
                        // =========================

class ThroughputBenchmark {
    // This class is a mechanism that provides performance testing for multi-
    // threaded components.  It allows running different thread functions at
    // the same time, and simulates a work load between subsequent calls to the
    // tested thread functions.  The results are loaded into a
    // 'bslmt::ThroughputBenchmarkResult' object, which provides access to
    // counts of the work done by each thread, thread group, and sample,
    // divided by the number of actual seconds of execution.

  public:
    // PUBLIC TYPES
    typedef bsl::function<void(int)> RunFunction;
        // An alias to a function meeting the following contract:
        //..
        //  void runTest(int threadIndex);
        //      // Run the main part of the benchmark having the specified
        //      // 'threadIndex'.  The behavior is undefined unless
        //      // 'threadIndex' is in the range '[0, numThreadsInGroup)',
        //      // where 'numThreadsInGroup' is the number of threads in a
        //      // thread group for the associated throughput benchmark.
        //..

    typedef bsl::function<void(bool)> InitializeSampleFunction;
        // An alias to a function meeting the following contract:
        //..
        //  void initializeSample(bool isFirst);
        //      // Initialize the sample run.  If the specified 'isFirst' is
        //      // 'true', this is the first sample run.
        //..

    typedef bsl::function<void(bool)> ShutdownSampleFunction;
        // An alias to a function meeting the following contract:
        //..
        //  void shutdownSample(bool isLast);
        //      // Clean up at the end of the sample run, before threads have
        //      // been joined.  If the specified 'isLast' is 'true', this is
        //      // the last sample run.
        //..

    typedef bsl::function<void(bool)> CleanupSampleFunction;
        // An alias to a function meeting the following contract:
        //..
        //  void cleanupSample(bool isLast);
        //      // Clean up after the sample run.  If the specified 'isLast' is
        //      // 'true', this is the last sample run.
        //..

    typedef bsl::function<void()> InitializeThreadFunction;
        // An alias to a function meeting the following contract:
        //..
        //  void initializeThread();
        //      // Initialize each thread in a sample run.
        //..

    typedef bsl::function<void()> CleanupThreadFunction;
        // An alias to a function meeting the following contract:
        //..
        //  void cleanupThread();
        //      // Clean up after each thread in a sample run.
        //..

    struct ThreadGroup {
        // Data used by a thread group

        // PUBLIC DATA
        RunFunction               d_func;         // test function to run

        int                       d_numThreads;   // number of threads in the
                                                  // thread group

        bsls::Types::Int64        d_amount;       // amount of busy work to
                                                  // perform between calls to
                                                  // 'd_func'

        InitializeThreadFunction  d_initialize;   // initialize function per
                                                  // thread

        CleanupThreadFunction     d_cleanup;      // cleanup function per
                                                  // thread
    };

  private:
    // CLASS DATA
    static unsigned int       s_antiOptimization; // Used by 'busyWork' to
                                                  // prevent optimization.

    // DATA
    bsl::vector<ThreadGroup>  d_threadGroups;     // Data kept for each thread
                                                  // group added.

    bsls::AtomicInt           d_state;            // This is how a test thread
                                                  // knows it has to exit.  It
                                                  // starts as 0, and exits
                                                  // when is set to 1.

    // FRIENDS
    friend class ThroughputBenchmark_WorkFunction;
    friend class ThroughputBenchmark_TestUtil;

    // NOT IMPLEMENTED
    ThroughputBenchmark(const ThroughputBenchmark&);
    ThroughputBenchmark& operator=(const ThroughputBenchmark&);

    // PRIVATE ACCESSORS
    bool isRunState() const;
        // Return 'true' if the test should continue to run, and 'false'
        // otherwise.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ThroughputBenchmark,
                                   bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static unsigned int antiOptimization();
        // Return the value calculated by 'busyWork'.  Note that this method is
        // provided to prevent the compiler from optimizing the simulated
        // workload away.

    static void busyWork(bsls::Types::Int64 busyWorkAmount);
        // Perform arithmetic operations to consume an amount of time in linear
        // relation to the specified 'busyWorkAmount'.

    static bsls::Types::Int64 estimateBusyWorkAmount(
                                                  bsls::TimeInterval duration);
        // Return an estimate of the work amount so that 'busyWork' invoked
        // with the returned work amount executes, approximately, for the
        // specified 'duration'.

    // CREATORS
    explicit ThroughputBenchmark(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'ThroughputBenchmark' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // MANIPULATORS
    int addThreadGroup(const RunFunction& runFunction,
                       int                numThreads,
                       bsls::Types::Int64 busyWorkAmount);
    int addThreadGroup(const RunFunction&              runFunction,
                       int                             numThreads,
                       bsls::Types::Int64              busyWorkAmount,
                       const InitializeThreadFunction& initializeFunctor,
                       const CleanupThreadFunction&    cleanupFunctor);
        // Create a set of threads, with cardinality the specified
        // 'numThreads', that will repeatedly execute the specified
        // 'runFunction' followed by the specified 'busyWork', with the
        // specified 'busyWorkAmount' as its argument.  Return the index for
        // the thread group.  Optionally specify 'initializeFunctor', which is
        // run at the beginning of each thread of the sample and accepts a
        // boolean flag 'isFirst', that is set to 'true' on the first sample,
        // and 'false' otherwise.  Optionally specify 'cleanupFunctor', which
        // is run at the end of each thread of the sample and accepts a boolean
        // flag 'isLast', that is set to 'true' on the last sample, and 'false'
        // otherwise.  Return an id for the added thread group.  The behavior
        // is undefined unless '0 < numThreads' and '0 <= busyWorkAmount'.

    void execute(ThroughputBenchmarkResult       *result,
                 int                              millisecondsPerSample,
                 int                              numSamples);
    void execute(ThroughputBenchmarkResult       *result,
                 int                              millisecondsPerSample,
                 int                              numSamples,
                 const InitializeSampleFunction&  initializeFunctor,
                 const ShutdownSampleFunction&    shutdownFunctor,
                 const CleanupSampleFunction&     cleanupFunctor);
        // Run the tests previously added with calls to the 'addThreadGroup'
        // method.  The tests are run for the specified 'numSamples' times.
        // Each sample is run for the specified 'millisecondsPerSample'
        // duration.  The results are stored in the specified 'result' object.
        // Optionally specify 'initializeFunctor', which is run at the
        // beginning of the sample and accepts a boolean flag 'isFirst', that
        // is set to 'true' on the first sample, and 'false' otherwise.
        // Optionally specify 'shutdownFunctor', which is run at the end of
        // each sample before threads have been joined, and accepts a boolean
        // flag 'isLast', that is set to 'true' on the last sample, and 'false'
        // otherwise.  Optionally specify 'cleanupFunctor', which is run at the
        // end of each sample after threads have been joined, and accepts a
        // boolean flag 'isLast', that is set to 'true' on the last sample, and
        // 'false' otherwise.  The behavior is undefined unless
        // '0 < millisecondsPerSample', '0 < numSamples', and
        // '0 < numThreadGroups()'.  Also see {Structure of a Test}.

    // ACCESSORS
    int numThreads() const;
        // Return the total number of threads.

    int numThreadGroups() const;
        // Return the number of thread groups.

    int numThreadsInGroup(int threadGroupIndex) const;
        // Return the number of threads in the specified 'threadGroupIndex'.
        // The behavior is undefined unless
        // '0 <= threadGroupIndex < numThreadGroups()'.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object.

};

                   // ===================================
                   // struct ThroughputBenchmark_WorkData
                   // ===================================

struct ThroughputBenchmark_WorkData {
    // Data transferred to ThroughputBenchmark_WorkFunction.

    // PUBLIC DATA
    ThroughputBenchmark::RunFunction              d_func;
                                                    // test function to run

    bsls::Types::Int64                            d_amount;
                                                    // busy work amount

    ThroughputBenchmark::InitializeThreadFunction d_initialize;
                                                    // initialize function per
                                                    // thread

    ThroughputBenchmark::CleanupThreadFunction    d_cleanup;
                                                    // cleanup function per
                                                    // thread

    ThroughputBenchmark                          *d_bench_p;
                                                    // exposes the "this"
                                                    // pointer of the benchmark
                                                    // to the work thread

    int                                           d_threadIndex;
                                                    // thread index 0, 1, 2,
                                                    // ... that is provided to
                                                    // the thread to
                                                    // differentiate it if so
                                                    // desired

    bslmt::Barrier                               *d_barrier_p;
                                                    // trigger start for
                                                    // threads to start
                                                    // processing at the same
                                                    // time

    bsls::Types::Int64                            d_actualNanos;
                                                    // number of nanoseconds
                                                    // that the thread actually
                                                    // ran

    bsls::Types::Int64                            d_count;
                                                    // number of items
                                                    // processed by this thread
};

                  // ======================================
                  // class ThroughputBenchmark_WorkFunction
                  // ======================================

class ThroughputBenchmark_WorkFunction {
    // This class is the work function functor, being called for each work
    // thread.

  private:
    // DATA
    ThroughputBenchmark_WorkData& d_data;

  public:
    // CREATORS
    explicit ThroughputBenchmark_WorkFunction(
                                           ThroughputBenchmark_WorkData& data);
        // Create a 'ThroughputBenchmark_WorkFunction' object with the
        // specified 'data' argument.

    // ~ThroughputBenchmark_WorkFunction() = default;
        // Destroy this object.

    // MANIPULATORS
    void operator()();
        // Work function being run on the thread.
};

                    // ==================================
                    // class ThroughputBenchmark_TestUtil
                    // ==================================

class ThroughputBenchmark_TestUtil {
    // This class implements a test utility that gives the test driver access
    // to the unexposed data members of 'ThroughputBenchmark'.

    // DATA
    ThroughputBenchmark& d_data;

  public:
    // CREATORS
    explicit ThroughputBenchmark_TestUtil(ThroughputBenchmark& data);
        // Create a 'ThroughputBenchmark_TestUtil' object to test contents of
        // the specified 'data'.

    // ~ThroughputBenchmark_TestUtil() = default;
        // Destroy this object.

    // MANIPULATORS
    bsls::AtomicInt& state();
        // Return a reference providing modifiable access to the 'd_state' data
        // member of 'ThroughputBenchmark'.

    bsl::vector<ThroughputBenchmark::ThreadGroup>& threadGroups();
        // Return a reference providing modifiable access to the
        // 'd_threadGroups' data member of 'ThroughputBenchmark'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class ThroughputBenchmark
                        // -------------------------

// PRIVATE ACCESSORS
inline
bool ThroughputBenchmark::isRunState() const
{
    return d_state.loadAcquire() == 0;
}

// ACCESSORS
inline
int ThroughputBenchmark::numThreads() const
{
    if (0 == d_threadGroups.size()) {
        return 0;                                                     // RETURN
    }

    int numThreads = d_threadGroups[0].d_numThreads;
    for (int i = 1; i < numThreadGroups(); ++i) {
        numThreads += d_threadGroups[i].d_numThreads;
    }
    return numThreads;
}

inline
int ThroughputBenchmark::numThreadGroups() const
{
    return static_cast<int>(d_threadGroups.size());
}

inline
int ThroughputBenchmark::numThreadsInGroup(int threadGroupIndex) const
{
    BSLS_ASSERT(0                 <= threadGroupIndex);
    BSLS_ASSERT(numThreadGroups() >  threadGroupIndex);

    return d_threadGroups[threadGroupIndex].d_numThreads;
}

                                  // Aspects

inline
bslma::Allocator* ThroughputBenchmark::allocator() const
{
    return d_threadGroups.get_allocator().mechanism();
}

                  // --------------------------------------
                  // class ThroughputBenchmark_WorkFunction
                  // --------------------------------------

// CREATORS
inline
ThroughputBenchmark_WorkFunction::ThroughputBenchmark_WorkFunction(
                                            ThroughputBenchmark_WorkData& data)
: d_data(data)
{
}

                    // ----------------------------------
                    // class ThroughputBenchmark_TestUtil
                    // ----------------------------------

// CREATORS
inline
ThroughputBenchmark_TestUtil::ThroughputBenchmark_TestUtil(
                                                     ThroughputBenchmark& data)
: d_data(data)
{
}

// MANIPULATORS
inline
bsls::AtomicInt& ThroughputBenchmark_TestUtil::state()
{
    return d_data.d_state;
}

inline
bsl::vector<ThroughputBenchmark::ThreadGroup>&
                                   ThroughputBenchmark_TestUtil::threadGroups()
{
    return d_data.d_threadGroups;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
