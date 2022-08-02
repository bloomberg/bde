// bslmt_throughputbenchmark.cpp                                      -*-C++-*-

#include <bslmt_throughputbenchmark.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_bslmt_throughputbenchmark_cpp,"$Id$ $CSID$")

#include <bslmt_threadutil.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>

#include <bsls_systemtime.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_array.h>
#include <bsl_vector.h>

#include <bslstl_sharedptr.h>

namespace BloombergLP {
namespace bslmt {

                        // -------------------------
                        // class ThroughputBenchmark
                        // -------------------------

// CLASS METHODS
unsigned int ThroughputBenchmark::antiOptimization()
{
    return s_antiOptimization;
}

void ThroughputBenchmark::busyWork(bsls::Types::Int64 busyWorkAmount)
{
    int j = 1;
    for (bsls::Types::Int64 i = 0; i < busyWorkAmount; ++i) {
        j = j * 3 % 7;
    }
    s_antiOptimization += j;
}

unsigned int ThroughputBenchmark::s_antiOptimization = 0;

bsls::Types::Int64 ThroughputBenchmark::estimateBusyWorkAmount(
                                                   bsls::TimeInterval duration)
{
    // The basic approach is to do a linear extrapolation from an observation
    // to compute the 'busyWorkAmount'.  Specifically: take a start time stamp,
    // run 'busyWork' with a reasonable busy work amount, take an end time
    // stamp, subtract the timestamps to obtain the duration, and extrapolate.
    //
    // On heavily loaded machines, the validity of using the linear
    // extrapolation degrades when the observed duration is significantly
    // different from the supplied 'duration'.  Furthermore, there are a
    // significant number of outliers in the observations.  A robust estimation
    // technique is used to mitigate the effect of outliers.  This algorithm
    // requires executing 'busyWork' multiple times.  While 'duration' is
    // expected to be short, a long 'duration' would make executing this method
    // temporally expensive.  As such, a maximum value is imposed during the
    // robust estimation steps, and then a final linear extrapolation is
    // performed, at the risk of this estimate being somewhat inaccurate for
    // long 'duration'.

    // compute target duration that ensures a tolerable execution time
    const bsls::Types::Int64 k_MAX_TARGET_DURATION_NANOS = 40 * 1000 * 1000;

    bsls::Types::Int64 targetDurationNanos = duration.totalNanoseconds();
    if (targetDurationNanos > k_MAX_TARGET_DURATION_NANOS) {
        targetDurationNanos = k_MAX_TARGET_DURATION_NANOS;
    }

    const int k_ITERATIONS = 10;  // number of estimation iterations, each
                                  // iteration adds two observations

    bsl::array<bsls::Types::Int64, 2 * k_ITERATIONS + 1> estimates;

    // initial estimate
    {
        bsls::Types::Int64 busyWorkAmount = 200000;  // initial value

        bsls::TimeInterval startTime = bsls::SystemTime::nowMonotonicClock();
        busyWork(busyWorkAmount);
        bsls::Types::Int64 nanos = (  bsls::SystemTime::nowMonotonicClock()
                                    - startTime).totalNanoseconds();

        estimates[0] = targetDurationNanos * busyWorkAmount / nanos;
    }

    for (int i = 0; i < k_ITERATIONS; ++i) {
        // add two estimates from the observed median of 'estimates'
        {
            bsls::Types::Int64 busyWorkAmount = estimates[i] * 9 / 10;

            bsls::TimeInterval startTime =
                                         bsls::SystemTime::nowMonotonicClock();
            busyWork(busyWorkAmount);
            bsls::Types::Int64 nanos = (  bsls::SystemTime::nowMonotonicClock()
                                        - startTime).totalNanoseconds();

            estimates[i*2 + 1] = targetDurationNanos * busyWorkAmount / nanos;
        }
        {
            bsls::Types::Int64 busyWorkAmount = estimates[i] * 11 / 10;

            bsls::TimeInterval startTime =
                                         bsls::SystemTime::nowMonotonicClock();
            busyWork(busyWorkAmount);
            bsls::Types::Int64 nanos = (  bsls::SystemTime::nowMonotonicClock()
                                        - startTime).totalNanoseconds();

            estimates[i*2 + 2] = targetDurationNanos * busyWorkAmount / nanos;
        }

        // sort to obtain the median
        bsl::sort(estimates.begin(), estimates.begin() + i * 2 + 3);
    }

    return duration.totalNanoseconds() * estimates[k_ITERATIONS]
                                                         / targetDurationNanos;
}

// CREATORS
ThroughputBenchmark::ThroughputBenchmark(bslma::Allocator *basicAllocator)
: d_threadGroups(basicAllocator)
{
    d_state.storeRelease(0);
}

// MANIPULATORS
int ThroughputBenchmark::addThreadGroup(const RunFunction& runFunction,
                                        int                numThreads,
                                        bsls::Types::Int64 busyWorkAmount)
{
    return addThreadGroup(runFunction,
                          numThreads,
                          busyWorkAmount,
                          InitializeThreadFunction(),
                          CleanupThreadFunction());
}

int ThroughputBenchmark::addThreadGroup(
                             const RunFunction&              runFunction,
                             int                             numThreads,
                             bsls::Types::Int64              busyWorkAmount,
                             const InitializeThreadFunction& initializeFunctor,
                             const CleanupThreadFunction&    cleanupFunctor)
{
    BSLS_ASSERT(0 <  numThreads);
    BSLS_ASSERT(0 <= busyWorkAmount);

    int         threadGroupIndex = static_cast<int>(d_threadGroups.size());
    ThreadGroup newTG;

    newTG.d_func       = runFunction;
    newTG.d_numThreads = numThreads;
    newTG.d_amount     = busyWorkAmount;
    newTG.d_initialize = initializeFunctor;
    newTG.d_cleanup    = cleanupFunctor;

    d_threadGroups.push_back(newTG);
    return threadGroupIndex;
}

void ThroughputBenchmark::execute(
                              ThroughputBenchmarkResult *result,
                              int                        millisecondsPerSample,
                              int                        numSamples)
{
    return execute(result,
                   millisecondsPerSample,
                   numSamples,
                   InitializeSampleFunction(),
                   ShutdownSampleFunction(),
                   CleanupSampleFunction());
}

void ThroughputBenchmark::execute(
                        ThroughputBenchmarkResult       *result,
                        int                              millisecondsPerSample,
                        int                              numSamples,
                        const InitializeSampleFunction&  initializeFunctor,
                        const ShutdownSampleFunction&    shutdownFunctor,
                        const CleanupSampleFunction&     cleanupFunctor)
{
    BSLS_ASSERT(0 < millisecondsPerSample);
    BSLS_ASSERT(0 < numSamples);
    BSLS_ASSERT(0 < numThreadGroups());
    BSLS_ASSERT(result);

    // At this point we know how many threads we have.
    int              nThreadGroups = numThreadGroups();
    int              nThreads      = numThreads();
    bsl::vector<int> threadGroupSizes(nThreadGroups,
                                      bslma::Default::defaultAllocator());

    for (int i = 0; i < nThreadGroups; ++i) {
        threadGroupSizes[i] = d_threadGroups[i].d_numThreads;
    }
    result->initialize(numSamples, threadGroupSizes);

    for (int sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex) {
        bool isFirst = sampleIndex == 0;
        bool isLast  = sampleIndex == numSamples - 1;
        if (initializeFunctor) {
            initializeFunctor(isFirst);
        }
        d_state.storeRelease(0);  // Reset the state.

        bslmt::Barrier                                 barrier(nThreads+1);
        bsl::vector<bslmt::ThreadUtil::Handle>         handles(nThreads);
        bsl::vector<ThroughputBenchmark_WorkData>      functionArgs(nThreads);
        bsl::vector<bsl::shared_ptr<ThroughputBenchmark_WorkFunction> >
                                                       workFunctions(nThreads);

        // Spawn work threads.
        int threadIndex = 0;
        for (int i = 0; i < nThreadGroups; ++i) {
            int numThreadsInGroup = d_threadGroups[i].d_numThreads;
            for (int j = 0; j < numThreadsInGroup; ++j, ++threadIndex) {
                functionArgs[threadIndex].d_func    = d_threadGroups[i].d_func;
                functionArgs[threadIndex].d_amount  =
                                                    d_threadGroups[i].d_amount;
                functionArgs[threadIndex].d_initialize =
                                                d_threadGroups[i].d_initialize;
                functionArgs[threadIndex].d_cleanup =
                                                   d_threadGroups[i].d_cleanup;
                functionArgs[threadIndex].d_bench_p = this;
                functionArgs[threadIndex].d_threadIndex = j;
                functionArgs[threadIndex].d_barrier_p = &barrier;

                workFunctions[threadIndex].reset(new
                  ThroughputBenchmark_WorkFunction(functionArgs[threadIndex]));
                bslmt::ThreadUtil::create(&handles[threadIndex],
                                          *workFunctions[threadIndex]);
            }
        }

        // Synchronize with the threads, after they finished initialization.
        barrier.wait();
        // Sleep for the duration, set the state to 1, and tell the tested
        // class to be ready to exit.
        static const int k_MILLIS_IN_SECOND = 1000;
        bslmt::ThreadUtil::microSleep(
                                   k_MILLIS_IN_SECOND * millisecondsPerSample);
        d_state.storeRelease(1);
        if (shutdownFunctor) {
            shutdownFunctor(isLast);
        }

        // Collect results.
        int curOffset = 0;
        for (int tgIdx = 0; tgIdx < nThreadGroups; ++tgIdx) {
            int numThreadsInGroup = d_threadGroups[tgIdx].d_numThreads;
            for (int tIdx = 0; tIdx < numThreadsInGroup; ++tIdx) {
                bslmt::ThreadUtil::join(handles[curOffset + tIdx]);
                bsls::Types::Int64 actualNanos =
                                              functionArgs[tIdx].d_actualNanos;
                bsls::Types::Int64 count = functionArgs[tIdx].d_count;

                static const double k_NANOS_IN_SECOND = 1e9;

                double throughput = static_cast<double>(count) *
                          k_NANOS_IN_SECOND / static_cast<double>(actualNanos);
                result->setThroughput(tgIdx, tIdx, sampleIndex, throughput);
            }
            curOffset += numThreadsInGroup;
        }

        if (cleanupFunctor) {
            cleanupFunctor(isLast);
        }
    } // END loop on numSamples
}

                  // --------------------------------------
                  // class ThroughputBenchmark_WorkFunction
                  // --------------------------------------

// MANIPULATORS
void ThroughputBenchmark_WorkFunction::operator()()
{
    if (d_data.d_initialize) {
        (d_data.d_initialize)();
    }
    // Wait for the other threads to get into position.
    d_data.d_barrier_p->wait();

    bsls::TimeInterval startTime = bsls::SystemTime::nowMonotonicClock();
    // Loop interspersing running the function to benchmark and wasting time.
    bsls::Types::Int64 count = 0;
    for (; d_data.d_bench_p->isRunState(); ++count) {
        d_data.d_func(d_data.d_threadIndex);
        d_data.d_bench_p->busyWork(d_data.d_amount);
    }
    bsls::TimeInterval endTime = bsls::SystemTime::nowMonotonicClock();
    bsls::TimeInterval duration = endTime - startTime;
    bsls::Types::Int64 actualNanos = duration.totalNanoseconds();
    if (actualNanos == 0) {
        actualNanos = 1; // end case due to too short an interval
    }
    d_data.d_actualNanos = actualNanos;
    d_data.d_count       = count;

    if (d_data.d_cleanup) {
        (d_data.d_cleanup)();
    }
}

}  // close package namespace
}  // close enterprise namespace

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
