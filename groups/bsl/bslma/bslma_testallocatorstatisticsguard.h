// bslma_testallocatorstatisticsguard.h                               -*-C++-*-
#ifndef INCLUDED_BSLMA_TESTALLOCATORSTATISTICSGUARD
#define INCLUDED_BSLMA_TESTALLOCATORSTATISTICSGUARD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide mechanism for `bslma::TestAllocator` scoped statistics.
//
//@CLASSES:
//  bslma::TestAllocatorStatiscticsGuard: `bslma::TestAllocator` scoped stats
//
//@SEE_ALSO: bslma_testallocator
//
//@DESCRIPTION: This component provides a single mechanism class,
// `bslma::TestAllocatorStatiscticsGuard`, which is used, in concert with
// `bslma::TestAllocator`, in the implementation of test drivers.  Upon its
// creation the `bslma::TestAllocatorStatiscticsGuard` stashes and resets the
// current statistics state of the specified `bslma::TestAllocator` so that
// local values (maximum etc) may be measured in thw scope of the guard.  Upon
// its destruction the guard restores the statistics values of the guarded test
// allocator to the state what it would have if the guard hasn't been there (as
// if no stashing and resetting had taken place), but combining the stashed
// values with the current statistics values of the test allocator.
//
///Statistics
///----------
// On creation the current statistics are saved to be used later in restoring,
// and then reset as follows:
//
// Statistic        | Reset value to
// ---------------- | --------------
// numAllocations   | numBlocksInUse
// numDeallocations | ZERO
// numMismatches    | ZERO
// numBoundsErrors  | ZERO
// numBlocksMax     | numBlocksInUse
// numBytesMax      | numBytesInUse
// numBlocksTotal   | numBlocksInUse
// numBytesTotal    | numBytesInUse
//
// During destruction the guard restores the state of the statistics, as if the
// reset (on construction) has never happened, in the following manner:
//
// Statistic        | Restore value as
// ---------------- | ---------------------------------------------------
// numAllocations   | saved + current - saved.numBlocksInUse
// numDeallocations | saved.numDeallocations + current.numDeallocations
// numMismatches    | saved.numMismatches    + current.numMismatches
// numBoundsErrors  | saved.numBoundsErrors  + current.numBoundsErrors
// numBlocksMax     | max(saved.numBlocksMax, current.numBlocksMax)
// numBytesMax      | max(saved.numBytesMax,  current.numBytesMax)
// numBlocksTotal   | saved + current - saved.numBlocksInUse
// numBytesTotal    | saved + current - saved.numBytesInUse
//
// See also `bslma::TestAllocator::stashStatistics` and
// `bslma::TestAllocator::restoreStatistics`.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Determine Maximums for a Scope
///- - - - - - - - - - - - - - - - - - - - -
// Suppose that, in a test driver, we would like to ensure that a certain
// operation does not use too much memory.  However, to do that operation we
// need to first do something that may or may not use more memory than what we
// allow.  In order for us to be able to measure local maximums (or any
// statistics) we need to stash and reset the statistics of the used test
// allocator, and later (at the end of our measured local scope) restore them
// as if we had never reset them.
//
// First, we define our `TestAllocator` that we will use throughout:
// ```
// bslma::TestAllocator testAllocator, *ta = &testAllocator;
// ```
// Then, we perform the test-preparation operation that may allocate and then
// release a lot of memory, therefore skewing later statistics:
// ```
// someOperation(ta);
//
// assert(ta->numBlocksMax() - ta->numBlocksInUse() > 4);
// ```
// Next, we prepare the local statistics by creating a scope and declaring a
// guard variable:
// ```
// {
//     bslma::TestAllocatorStatisticsGuard tasg(ta);
//     assert(ta->numBlocksInUse() == tasg.originalNumBlocksInUse());
// ```
// Now, we run the measured operation and verify that it has not allocated more
// than 4 blocks (in addition to what was already allocated before):
// ```
//     measuredOperation(ta);
//     assert(ta->numBlocksMax() - tasg.originalNumBlocksInUse() <= 4);
// }
// ```
// Finally, we demonstrate that the guard restores the statistics:
// ```
// assert(ta->numBlocksMax() - ta->numBlocksInUse() > 4);
// ```

#include <bslscm_version.h>

#include <bslma_testallocator.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bslma {

                    // ==================================
                    // class TestAllocatorStatisticsGuard
                    // ==================================

/// This mechanism provides the means to get localized `TestAllocator`
/// statistics in a scope since the construction of the guard, and also
/// reinstates the statistics upon its destruction as if the local reset has
/// not happened.  See the [](#Statistics) section for the statistics managed.
class TestAllocatorStatisticsGuard {

    // DATA
    TestAllocatorStashedStatistics d_stashed;  // the stashed statistics

  private:
    // NOT IMPLEMENTED
    TestAllocatorStatisticsGuard(const TestAllocatorStatisticsGuard&);
                                                                    // = delete
    TestAllocatorStatisticsGuard& operator=(
                             const TestAllocatorStatisticsGuard&);  // = delete

  public:
    // CREATORS

    /// Create a `TestAllocatorStatisticsGuard` object to stash, reset, and
    /// restore statistics of the specified `testAllocator`.
    explicit TestAllocatorStatisticsGuard(TestAllocator *testAllocator);

    /// Destroy this object and restore the statistics of the `testAllocator`
    /// supplied at construction to a state as if this guard had never existed.
    ~TestAllocatorStatisticsGuard();

    // ACCESSORS

    /// Return the stashed value of `numBlocksInUse` at construction.
    bsls::Types::Int64 originalNumBlocksInUse() const;

    /// Return the stashed value of `numBytesInUse` at construction.
    bsls::Types::Int64 originalNumBytesInUse() const;
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                    // ----------------------------------
                    // class TestAllocatorStatisticsGuard
                    // ----------------------------------

// CREATORS
inline
TestAllocatorStatisticsGuard::TestAllocatorStatisticsGuard(
                                                  TestAllocator *testAllocator)
: d_stashed(testAllocator->stashStatistics())
{
}

inline
TestAllocatorStatisticsGuard::~TestAllocatorStatisticsGuard()
{
    d_stashed.restore();
}

// ACCESSORS
inline
bsls::Types::Int64 TestAllocatorStatisticsGuard::originalNumBlocksInUse() const
{
    return d_stashed.numBlocksInUse();
}

inline
bsls::Types::Int64 TestAllocatorStatisticsGuard::originalNumBytesInUse() const
{
    return d_stashed.numBytesInUse();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
