// bslmt_throughputbenchmarkresult.cpp                                -*-C++-*-

#include <bslmt_throughputbenchmarkresult.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_bslmt_throughputbenchmarkresult_cpp,"$Id$ $CSID$")

#include <bslma_allocator.h>

#include <bslmf_assert.h>

#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_vector.h>
#include <bsl_cstddef.h>

namespace BloombergLP {
namespace bslmt {

                     // -------------------------------
                     // class ThroughputBenchmarkResult
                     // -------------------------------

// PRIVATE ACCESSORS
void ThroughputBenchmarkResult::getSortedSumThroughputs(
                                   bsl::vector<double> *throughputs,
                                   int                  threadGroupIndex) const
{
    BSLS_ASSERT(0                 <= threadGroupIndex);
    BSLS_ASSERT(numThreadGroups() >  threadGroupIndex);
    BSLS_ASSERT(throughputs);
    BSLS_ASSERT(static_cast<int>(throughputs->size()) == numSamples());

    DoubleVector& vec = *throughputs;
    int           numThreadsInTG = numThreads(threadGroupIndex);
    for (int sampleIndex = 0; sampleIndex < numSamples(); ++sampleIndex) {
        vec[sampleIndex] = 0;
        for (int threadIndex = 0; threadIndex < numThreadsInTG;
                                                               ++threadIndex) {
            double thrput = getValue(threadGroupIndex,
                                     threadIndex,
                                     sampleIndex);
            vec[sampleIndex] += thrput;
        }
    }
    bsl::sort(vec.begin(), vec.end());
}

// CREATORS
ThroughputBenchmarkResult::ThroughputBenchmarkResult(
                                     int                      numSamples,
                                     const bsl::vector<int>&  threadGroupSizes,
                                     bslma::Allocator        *basicAllocator)
: d_vecThroughputs(basicAllocator)
{
    BSLS_ASSERT(0 < numSamples);
    BSLS_ASSERT(0 < threadGroupSizes.size());

    initialize(numSamples, threadGroupSizes);
}

ThroughputBenchmarkResult::ThroughputBenchmarkResult(
                                              bslma::Allocator *basicAllocator)
: d_vecThroughputs(basicAllocator)
{
}

ThroughputBenchmarkResult::ThroughputBenchmarkResult(
                              const ThroughputBenchmarkResult&  original,
                              bslma::Allocator                 *basicAllocator)
: d_vecThroughputs(original.d_vecThroughputs, basicAllocator)
{
}

ThroughputBenchmarkResult::ThroughputBenchmarkResult(
                         bslmf::MovableRef<ThroughputBenchmarkResult> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_vecThroughputs(bslmf::MovableRefUtil::move(
                     bslmf::MovableRefUtil::access(original).d_vecThroughputs))
{
}

ThroughputBenchmarkResult::ThroughputBenchmarkResult(
                  bslmf::MovableRef<ThroughputBenchmarkResult>  original,
                  bslma::Allocator                             *basicAllocator)
: d_vecThroughputs(bslmf::MovableRefUtil::move(
     bslmf::MovableRefUtil::access(original).d_vecThroughputs), basicAllocator)
{
}

// MANIPULATORS
ThroughputBenchmarkResult& ThroughputBenchmarkResult::operator=(
                                          const ThroughputBenchmarkResult& rhs)
{
    d_vecThroughputs = rhs.d_vecThroughputs;
    return *this;
}

ThroughputBenchmarkResult& ThroughputBenchmarkResult::operator=(
                              bslmf::MovableRef<ThroughputBenchmarkResult> rhs)
{
    d_vecThroughputs = bslmf::MovableRefUtil::move(
        bslmf::MovableRefUtil::access(rhs).d_vecThroughputs);

    return *this;
}

void ThroughputBenchmarkResult::initialize(
                                      int                     numSamples,
                                      const bsl::vector<int>& threadGroupSizes)
{
    BSLS_ASSERT(0 < numSamples);
    BSLS_ASSERT(0 < threadGroupSizes.size());

    int numTG = static_cast<int>(threadGroupSizes.size());
    for (int i = 0; i < numTG; ++i) {
        BSLS_ASSERT(0 < threadGroupSizes[i]);
    }

    d_vecThroughputs.resize(numSamples);
    for (int i = 0; i < numSamples; ++i) {
        d_vecThroughputs[i].resize(numTG);
        for (int j = 0; j < numTG; ++j) {
            d_vecThroughputs[i][j].resize(threadGroupSizes[j], 0.0);
        }
    }
}

// ACCESSORS
                                  // Results
void ThroughputBenchmarkResult::getMedian(double *median,
                                          int     threadGroupIndex) const
{
    BSLS_ASSERT(0                 <= threadGroupIndex);
    BSLS_ASSERT(numThreadGroups() >  threadGroupIndex);
    BSLS_ASSERT(median);

    int          nSamples = numSamples();
    DoubleVector sortedThroughputs(nSamples,
                                   0.0,
                                   bslma::Default::defaultAllocator());
    getSortedSumThroughputs(&sortedThroughputs, threadGroupIndex);
    int    medianIdx = nSamples / 2;
    *median   = nSamples % 2 == 0 ?
        (sortedThroughputs[medianIdx] + sortedThroughputs[medianIdx-1]) / 2.0 :
         sortedThroughputs[medianIdx];
}

void ThroughputBenchmarkResult::getPercentile(double *percentile,
                                              double  percentage,
                                              int     threadGroupIndex) const
{
    BSLS_ASSERT(0                 <= threadGroupIndex);
    BSLS_ASSERT(numThreadGroups() >  threadGroupIndex);
    BSLS_ASSERT(0.0               <= percentage);
    BSLS_ASSERT(1.0               >= percentage);
    BSLS_ASSERT(percentile);

    int          nSamples = numSamples();
    DoubleVector sortedThroughputs(nSamples,
                                   0.0,
                                   bslma::Default::defaultAllocator());
    getSortedSumThroughputs(&sortedThroughputs, threadGroupIndex);
    int    statIdx = static_cast<int>(nSamples * percentage);
    if (statIdx == nSamples) --statIdx;
    *percentile = sortedThroughputs[statIdx];
}

void ThroughputBenchmarkResult::getPercentiles(
                                   bsl::vector<double> *percentiles,
                                   int                  threadGroupIndex) const
{
    BSLS_ASSERT(0                 <= threadGroupIndex);
    BSLS_ASSERT(numThreadGroups() >  threadGroupIndex);
    BSLS_ASSERT(percentiles);
    BSLS_ASSERT(2                 <= percentiles->size());

    int          nSamples = numSamples();
    DoubleVector sortedThroughputs(nSamples,
                                   0.0,
                                   bslma::Default::defaultAllocator());
    getSortedSumThroughputs(&sortedThroughputs, threadGroupIndex);
    double curPercent = 0.0;
    double percentInc = 1.0 / static_cast<double>(percentiles->size() - 1);
    for (bsl::size_t i = 0; i < percentiles->size();
                                               ++i, curPercent += percentInc) {
        int statIdx = static_cast<int>(nSamples * curPercent);
        if (statIdx == nSamples) --statIdx;
        (*percentiles)[i] = sortedThroughputs[statIdx];
    }
}

void ThroughputBenchmarkResult::getThreadPercentiles(
                     bsl::vector<bsl::vector<double> > *percentiles,
                     int                                threadGroupIndex) const
{
    BSLS_ASSERT(0                 <= threadGroupIndex);
    BSLS_ASSERT(numThreadGroups() >  threadGroupIndex);
    BSLS_ASSERT(percentiles);
    BSLS_ASSERT(2                 <= percentiles->size());

    int    nSamples = numSamples();
    int    numThreadsInTG = numThreads(threadGroupIndex);
    for (bsl::size_t i = 0; i < percentiles->size(); ++i) {
        BSLS_ASSERT(numThreadsInTG ==
                                   static_cast<int>((*percentiles)[i].size()));
    }

    DoubleVector sortedThroughputs(nSamples,
                                   bslma::Default::defaultAllocator());
    for (int threadIndex = 0; threadIndex < numThreadsInTG; ++threadIndex) {
        for (int sampleIndex = 0; sampleIndex < nSamples; ++sampleIndex) {
            sortedThroughputs[sampleIndex] =
                    getValue(threadGroupIndex, threadIndex, sampleIndex);
        }
        bsl::sort(sortedThroughputs.begin(), sortedThroughputs.end());

        double curPercent = 0.0;
        double percentInc = 1.0 / static_cast<double>(percentiles->size() - 1);
        for (bsl::size_t i = 0; i < percentiles->size();
                                               ++i, curPercent += percentInc) {
            int statIdx = static_cast<int>(nSamples * curPercent);
            if (statIdx == nSamples) --statIdx;
            (*percentiles)[i][threadIndex] = sortedThroughputs[statIdx];
        }
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
