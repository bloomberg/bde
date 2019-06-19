// bslmt_throughputbenchmarkresult.h                                  -*-C++-*-

#ifndef INCLUDED_BSLMT_THROUGHPUTBENCHMARKRESULT
#define INCLUDED_BSLMT_THROUGHPUTBENCHMARKRESULT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide result repository for throughput performance test harness.
//
//@CLASSES:
//  bslmt::ThroughputBenchmarkResult: results for multi-threaded benchmarks
//
//@SEE_ALSO: bslmt_throughputbenchmark
//
//@DESCRIPTION: This component defines a mechanism,
// 'bslmt::ThroughputBenchmarkResult', which represents counts of the work done
// by each thread, thread group, and sample, divided by the number of actual
// seconds that the sample took to execute.  Each specific result can be
// retrieved by calling 'getValue', and relevant percentiles can be retrieved
// using 'getMedian', 'getPercentile', 'getPercentiles', and
// 'getThreadPercentiles'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Calculate Median and Percentiles
///- - - - - - - - - - - - - - - - - - - - - -
// In the following example we populate a 'bslmt::ThroughputBenchmarkResult'
// object and calculate median and percentiles.
//
// First, we define a vector with thread group sizes:
//..
//  bsl::vector<int> threadGroupSizes;
//  threadGroupSizes.resize(2);
//  threadGroupSizes[0] = 3;
//  threadGroupSizes[1] = 2;
//..
// Next, we define a 'bslmt::ThroughputBenchmarkResult' with 10 samples and the
// previously defined thread group sizes:
//..
//  bslmt::ThroughputBenchmarkResult myResult(10, threadGroupSizes);
//..
// Then, we populate the object with throughputs:
//..
//  for (int tgId = 0; tgId < 2; ++tgId) {
//      for (int tId = 0; tId < myResult.numThreads(tgId); ++tId) {
//          for (int sId = 0; sId < 10; ++sId) {
//              double throughput = static_cast<double>(rand());
//              myResult.setThroughput(tgId, tId, sId, throughput);
//          }
//      }
//  }
//..
// Now, we calculate median of the first thread group and print it out:
//..
//  double median;
//  myResult.getMedian(&median, 0);
//  bsl::cout << "Median of first thread group:" << median << "\n";
//..
// Finally, we calculate percentiles 0, 0.25, 0.5, 0.75, and 1.0 of the first
// thread group and print it out:
//..
//  bsl::vector<double> percentiles(5);
//  myResult.getPercentiles(&percentiles, 0);
//  for (int i = 0; i < 5; ++i) {
//      bsl::cout << "Percentile " << 25 * i << "% is:"
//                << percentiles[i] << "\n";
//  }
//..

#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_types.h>

#include <bsl_vector.h>

namespace BloombergLP {
namespace bslmt {

class ThroughputBenchmarkResult_TestUtil;

                     // ===============================
                     // class ThroughputBenchmarkResult
                     // ===============================

class ThroughputBenchmarkResult {
    // This class provides support for output of multi-threaded performance
    // benchmark results.  The results are counts of work done during the
    // benchmark time period divided by the time period.

  public:
    // PUBLIC TYPES
    typedef bsls::Types::Int64  Int64;
    typedef bsl::vector<double> DoubleVector;

  private:
    // DATA
    bsl::vector<bsl::vector<DoubleVector> > d_vecThroughputs;
        // Count of work done, collected from the various threads, and the
        // various samples, divided by the actual time period a sample took.
        // The inner-most vectors (the 'DoubleVector') are the data for the
        // specific threads within a thread group.  The middle vector is
        // indexed over the thread groups.  The outer vector is indexed over
        // the samples.  That is, to access sample S1, thread group G1, and
        // thread index T1 within G1, we refer to
        // 'd_vecThroughputs[S1][G1][T1]'.

    // PRIVATE ACCESSORS
    void getSortedSumThroughputs(bsl::vector<double> *throughputs,
                                 int                  threadGroupIndex) const;
        // Load into the specified 'throughputs' vector a sum of the work done
        // by all the threads in the specified 'threadGroupIndex'.  The size of
        // 'throughputs' must match the number of samples.  The behavior is
        // undefined unless '0 <= threadGroupIndex < numThreadGroups' and
        // 'throughputs->size() == numSamples()'.

    // FRIENDS
    friend class ThroughputBenchmarkResult_TestUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ThroughputBenchmarkResult,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit ThroughputBenchmarkResult(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'ThroughputBenchmarkResult' object.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that this object has to be initialized before it can be
        // used.

    ThroughputBenchmarkResult(int                      numSamples,
                              const bsl::vector<int>&  threadGroupSizes,
                              bslma::Allocator        *basicAllocator = 0);
        // Create a 'ThroughputBenchmarkResult' object with the specified
        // 'numSamples' the number of samples in the benchmark, and the
        // specified 'threadGroupSizes', the number of threads in each of the
        // thread groups.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // '0 < numSamples', '0 < threadGroupSizes.size()', and
        // '0 < threadGroupSizes[N]' for all valid 'N'.

    ThroughputBenchmarkResult(
                         const ThroughputBenchmarkResult&  original,
                         bslma::Allocator                 *basicAllocator = 0);
        // Create a 'ThroughputBenchmarkResult' object having the value of the
        // specified 'original'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ThroughputBenchmarkResult(
                         bslmf::MovableRef<ThroughputBenchmarkResult> original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'ThroughputBenchmarkResult' object having the same value
        // and the same allocator as the specified 'original' object.  The
        // value of 'original' becomes unspecified but valid, and its allocator
        // remains unchanged.

    ThroughputBenchmarkResult(
                 bslmf::MovableRef<ThroughputBenchmarkResult>  original,
                 bslma::Allocator                             *basicAllocator);
        // Create a 'ThroughputBenchmarkResult' object having the same value as
        // the specified 'original' object, using the specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.  The allocator of
        // 'original' remains unchanged.  If 'original' and the newly created
        // object have the same allocator then the value of 'original' becomes
        // unspecified but valid, and no exceptions will be thrown; otherwise
        // 'original' is unchanged (and an exception may be thrown).

    // ~ThroughputBenchmarkResult() = default;
        // Destroy this object.

    // MANIPULATORS
    ThroughputBenchmarkResult& operator=(const ThroughputBenchmarkResult& rhs);
        // Assign to this object the value of the specified 'rhs' benchmark
        // result, and return a reference providing modifiable access to this
        // object.

    ThroughputBenchmarkResult& operator=(
                             bslmf::MovableRef<ThroughputBenchmarkResult> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.  The allocators of
        // this object and 'rhs' both remain unchanged.  If 'rhs' and this
        // object have the same allocator then the value of 'rhs' becomes
        // unspecified but valid, and no exceptions will be thrown; otherwise
        // 'rhs' is unchanged (and an exception may be thrown).

    void initialize(int numSamples, const bsl::vector<int>& threadGroupSizes);
        // Initialize a default constructed 'ThroughputBenchmarkResult' object
        // with the specified 'numSamples' number of samples in the benchmark,
        // and the specified 'threadGroupSizes', the number of threads in each
        // of the thread groups.  If any data was previously kept, it is lost.
        // The behavior is undefined unless '0 < numSamples',
        // '0 < threadGroupSizes.size()', and '0 < threadGroupSizes[N]' for all
        // valid N.

    void setThroughput(int    threadGroupIndex,
                       int    threadIndex,
                       int    sampleIndex,
                       double value);
        // Set the throughput related to the specified 'threadIndex' thread, in
        // the specified 'threadGroupIndex', and the specified 'sampleIndex' to
        // the specified 'value'.  The behavior is undefined unless
        // '0 <= value', '0 <= threadIndex < numThreads(threadGroupIndex)',
        // '0 <= threadGroupIndex < numThreadGroups()', and
        // '0 <= sampleIndex < numSamples()'.

    // ACCESSORS

                             // Object state

    int numSamples() const;
        // Return the number of test samples.

    int numThreadGroups() const;
        // Return the number of thread groups.

    int numThreads(int threadGroupIndex) const;
        // Return the number of threads in the specified 'threadGroupIdx'.  The
        // behavior is undefined unless
        // '0 <= threadGroupIndex < numThreadGroups()'.

    int totalNumThreads() const;
        // Return the total number of threads.

                                  // Results

    double getValue(int threadGroupIndex,
                    int threadIndex,
                    int sampleIndex) const;
        // Return the throughput of work done on the specified 'threadIndex'
        // thread, in the specified 'threadGroupIndex', and the specified
        // 'sampleIndex' sample.  The behavior is undefined unless
        // '0 <= threadIndex < numThreads(threadGroupIndex)',
        // '0 <= threadGroupIndex < numThreadGroups()', and
        // '0 <= sampleIndex < numSamples()'.

    void getMedian(double *median, int threadGroupIndex) const;
        // Load into the specified 'median' the median throughput (count /
        // second) of the work done by all the threads in the specified
        // 'threadGroupIndex'.  The behavior is undefined unless
        // '0 <= threadGroupIndex < numThreadGroups'.

    void getPercentile(double *percentile,
                       double  percentage,
                       int     threadGroupIndex) const;
        // Load into the specified 'percentile' the specified 'percentage'
        // throughput (count / second) of the work done by all the threads in
        // the specified 'threadGroupIndex'.  A 'percentage' of 0.0 is the
        // minimum, and a 'percentage' of 1.0 is the maximum.  The behavior is
        // undefined unless '0 <= threadGroupIndex < numThreadGroups' and
        // '0.0 <= percentage <= 1.0'.

    void getPercentiles(bsl::vector<double> *percentiles,
                        int                  threadGroupIndex) const;
        // Load into the specified 'percentiles' vector a uniform breakdown of
        // percentage throughput (count / second) of the work done by all the
        // threads in the specified 'threadGroupIndex'.  The size of
        // 'percentiles' controls how many percentages are provided.  For
        // example, a size of 5 will return the percentages 0, 0.25, 0.5, 0.75,
        // 1.  The behavior is undefined unless
        // '0 <= threadGroupIndex < numThreadGroups' and
        // '2 <= percentiles->size()'.

    void getThreadPercentiles(
                    bsl::vector<bsl::vector<double> > *percentiles,
                    int                                threadGroupIndex) const;
        // Load into the specified 'percentiles' vector of vectors a uniform
        // breakdown of percentage throughput (count / second) of the work done
        // on the specified 'threadGroupIndex' for each of the threads in it.
        // The size of 'percentiles' controls how many percentages are
        // provided.  For example, a size of 5 will return the percentages 0,
        // 0.25, 0.5, 0.75, 1.  The size of each of the vectors inside 'stats'
        // must be 'numThreads(threadGroupId)'.  The behavior is undefined
        // unless '0 <= threadGroupId < numThreadGroups',
        // '2 <= percentiles.size()', and
        // 'percentiles[N].size() == numThreads(threadGroupIndex)' for all
        // N.

                                  // Aspects
    bslma::Allocator *allocator() const;
        // Return the allocator used by this object.

};

                 // ========================================
                 // class ThroughputBenchmarkResult_TestUtil
                 // ========================================

class ThroughputBenchmarkResult_TestUtil {
    // This component-private class provides modifiable access to the
    // non-public attributes of a 'ThroughPutBenchmarkResult' object supplied
    // on construction, and is provided for use exclusively in the test driver
    // of this component.

    // DATA
    ThroughputBenchmarkResult& d_data;

  public:
    // CREATORS
    explicit ThroughputBenchmarkResult_TestUtil(
                                              ThroughputBenchmarkResult& data);
        // Create a 'ThroughputBenchmarkResult_TestUtil' object to test
        // contents of the specified 'data'.

    // ~ThroughputBenchmarkResult_TestUtil() = default; Destroy this object.

    // MANIPULATORS
    bsl::vector<bsl::vector<bsl::vector<double> > >& throughputs();
        // Return a reference providing modifiable access to the
        // 'd_vecThroughputs' data member of 'ThroughputBenchmarkResult'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                     // -------------------------------
                     // class ThroughputBenchmarkResult
                     // -------------------------------

// MANIPULATORS
inline
void ThroughputBenchmarkResult::setThroughput(int    threadGroupIndex,
                                              int    threadIndex,
                                              int    sampleIndex,
                                              double value)
{
    BSLS_ASSERT(0                            <= value);
    BSLS_ASSERT(0                            <= threadGroupIndex);
    BSLS_ASSERT(numThreadGroups()            >  threadGroupIndex);
    BSLS_ASSERT(0                            <= threadIndex);
    BSLS_ASSERT(numThreads(threadGroupIndex) >  threadIndex);
    BSLS_ASSERT(0                            <= sampleIndex);
    BSLS_ASSERT(numSamples()                 >  sampleIndex);

    d_vecThroughputs[sampleIndex][threadGroupIndex][threadIndex] = value;
}

// ACCESSORS
                                // Object state
inline
int ThroughputBenchmarkResult::numSamples() const
{
    return static_cast<int>(d_vecThroughputs.size());
}

inline
int ThroughputBenchmarkResult::numThreadGroups() const
{
    if (0 == numSamples()) {
        return 0;                                                     // RETURN
    }
    return static_cast<int>(d_vecThroughputs[0].size());
}

inline
int ThroughputBenchmarkResult::numThreads(int threadGroupIndex) const
{
    BSLS_ASSERT(0                 <= threadGroupIndex);
    BSLS_ASSERT(numThreadGroups() >  threadGroupIndex);

    return static_cast<int>(d_vecThroughputs[0][threadGroupIndex].size());
}

inline
int ThroughputBenchmarkResult::totalNumThreads() const
{
    if (0 == numSamples()) {
        return 0;                                                     // RETURN
    }

    int nThreadGroups = numThreadGroups();
    int nThreads = 0;
    for (int i = 0; i < nThreadGroups; ++i) {
        nThreads += numThreads(i);
    }
    return nThreads;
}

                                  // Results
inline
double ThroughputBenchmarkResult::getValue(int threadGroupIndex,
                                           int threadIndex,
                                           int sampleIndex) const
{
    BSLS_ASSERT(0                            <= threadGroupIndex);
    BSLS_ASSERT(numThreadGroups()            >  threadGroupIndex);
    BSLS_ASSERT(0                            <= threadIndex);
    BSLS_ASSERT(numThreads(threadGroupIndex) >  threadIndex);
    BSLS_ASSERT(0                            <= sampleIndex);
    BSLS_ASSERT(numSamples()                 >  sampleIndex);

    return d_vecThroughputs[sampleIndex][threadGroupIndex][threadIndex];
}

                        // Aspects
inline
bslma::Allocator* ThroughputBenchmarkResult::allocator() const
{
    return d_vecThroughputs.get_allocator().mechanism();
}

                 // ----------------------------------------
                 // class ThroughputBenchmarkResult_TestUtil
                 // ----------------------------------------

// CREATORS
inline
ThroughputBenchmarkResult_TestUtil::ThroughputBenchmarkResult_TestUtil(
                                               ThroughputBenchmarkResult& data)
: d_data(data)
{
}

// MANIPULATORS
inline
bsl::vector<bsl::vector<bsl::vector<double> > >&
                              ThroughputBenchmarkResult_TestUtil::throughputs()
{
    return d_data.d_vecThroughputs;
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
