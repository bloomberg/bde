// bslma_testallocatorstatisticsguard.t.cpp                           -*-C++-*-
#include <bslma_testallocatorstatisticsguard.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_types.h>

#include <stdio.h>      // `printf`
#include <stdlib.h>     // `atoi`
#include <string.h>     // `strlen`

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a guard that stashes and resets the
// statistics of the test allocator object supplied at construction, and on
// destruction restores the statistics as if the reset had never happened.
// The guard uses member function of the test allocator that are already tested
// so the testing concern here is only that those functions are called on
// construction and destruction, we do not have to fully test their behavior.
//
// The guard also provides access to the state of the in-use test allocator at
// the time of construction because some statistics are reset to those values
// (`originalNumBlocksInUse()`, `originalNumBytesInUse`) so they may be needed
// by client code to interpret current statistics within the examined block.
// Our concern here is that those values are actually the ones at the time of
// construction and do not change during the lifetime of the guard.
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] bslma::TestAllocatorStatiscticsGuard(bslma::TestAllocator *);
// [ 1] ~bslma::TestAllocatorStatiscticsGuard();
//
// ACCESSORS
// [ 2] bsls::Types::Int64 originalNumBlocksInUse() const;
// [ 2] bsls::Types::Int64 originalNumBytesInUse() const;
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 2] ACCESSORS
// [ 1] GUARD FUNCTIONALITY
// [ *] CONCERN: There is no memory allocation from default allocator.
// [ *] CONCERN: There is no memory allocation from global allocator.

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Example 1: Determine Maximums for a Scope
///- - - - - - - - - - - - - - - - - - - - -
// We define fake functions here that stand in for the not-measured and the
// measured operation.  Real operations would have a return value and so have
// something that remains allocated, but that would complicate this test
// driver beyond what is necessary to demonstrate the guard functionality.

void someOperation(bslma::TestAllocator *ta)
{
    void *p1 = ta->allocate(128);
    void *p2 = ta->allocate(128);
    void *p3 = ta->allocate(128);
    void *p4 = ta->allocate(128);
    void *p5 = ta->allocate(128);

    ta->deallocate(p5);
    ta->deallocate(p4);
    ta->deallocate(p3);
    ta->deallocate(p2);
    ta->deallocate(p1);
}

void measuredOperation(bslma::TestAllocator *ta)
{
    void* p1 = ta->allocate(128);
    void* p2 = ta->allocate(128);
    void* p3 = ta->allocate(128);

    ta->deallocate(p3);
    ta->deallocate(p2);
    ta->deallocate(p1);
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3; (void)veryVerbose;
    bool     veryVeryVerbose = argc > 4; (void)veryVeryVerbose;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

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
//
//```
   bslma::TestAllocator testAllocator, *ta = &testAllocator;
//```
//
// Then, we perform the test-preparation operation that may allocate and then
// release a lot of memory, therefore skewing later statistics:
//
//```
   someOperation(ta);
//
   ASSERT(ta->numBlocksMax() - ta->numBlocksInUse() > 4);
//```
//
// Next, we prepare the local statistics by creating a scope and declaring a
// guard variable:
//```
  {
      bslma::TestAllocatorStatisticsGuard tasg(ta);
      ASSERT(ta->numBlocksInUse() == tasg.originalNumBlocksInUse());
//```
// Now, we run the measured operation and verify that it has not allocated more
// than 4 blocks (in addition to what was already allocated before):
//```
      measuredOperation(ta);
      ASSERT(ta->numBlocksMax() - tasg.originalNumBlocksInUse() <= 4);
  }
//```
//
// Finally, we demonstrate that the guard restores the statistics:
//```
  ASSERT(ta->numBlocksMax() - ta->numBlocksInUse() > 4);
//```
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        // 1. The "originalInUse" accessors return the values at the time of
        //    construction and do not change during the lifetime of the guard.
        //
        // Plan:
        // 1. Create a test allocator, perform several allocations (retaining
        //    the returned memory addresses).  Save the original in-use values.
        //    Then create a test allocator statistics guard with its own scope.
        //    Immediately after creation verify that the original in-use values
        //    match those we saved from the test allocator.
        //
        // 2. Within the scope exercise the test allocator by allocating and
        //    deallocating blocks.  Verify that the reported original values do
        //    not change.
        //
        // Testing:
        //   bsls::Types::Int64 originalNumBlocksInUse() const;
        //   bsls::Types::Int64 originalNumBytesInUse() const;
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING ACCESSORS"
                          "\n=================");

        bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

        void *p1 = ta.allocate(1024);
        void *p2 = ta.allocate(1024);
        void *p3 = ta.allocate(1024);
        ta.deallocate(ta.allocate(1024));

        const bsls::Types::Int64 taOrigBlocksInUse = ta.numBlocksInUse();
        const bsls::Types::Int64 taOrigBytesInUse  = ta.numBytesInUse();
        {
            bslma::TestAllocatorStatisticsGuard guard(&ta);

            ASSERTV(guard.originalNumBlocksInUse(),   taOrigBlocksInUse,
                    guard.originalNumBlocksInUse() == taOrigBlocksInUse);
            ASSERTV(guard.originalNumBytesInUse(),    taOrigBytesInUse,
                    guard.originalNumBytesInUse()  == taOrigBytesInUse);

            void *p4 = ta.allocate(1024);
            ASSERTV(guard.originalNumBlocksInUse(),   taOrigBlocksInUse,
                    guard.originalNumBlocksInUse() == taOrigBlocksInUse);
            ASSERTV(guard.originalNumBytesInUse(),    taOrigBytesInUse,
                    guard.originalNumBytesInUse()  == taOrigBytesInUse);

            void *p5 = ta.allocate(1024);
            ASSERTV(guard.originalNumBlocksInUse(),   taOrigBlocksInUse,
                    guard.originalNumBlocksInUse() == taOrigBlocksInUse);
            ASSERTV(guard.originalNumBytesInUse(),    taOrigBytesInUse,
                    guard.originalNumBytesInUse()  == taOrigBytesInUse);
            ta.deallocate(ta.allocate(1024));
            ASSERTV(guard.originalNumBlocksInUse(),   taOrigBlocksInUse,
                    guard.originalNumBlocksInUse() == taOrigBlocksInUse);
            ASSERTV(guard.originalNumBytesInUse(),    taOrigBytesInUse,
                    guard.originalNumBytesInUse()  == taOrigBytesInUse);

            ta.deallocate(p5);
            ta.deallocate(p4);
        }
        ta.deallocate(p3);
        ta.deallocate(p2);
        ta.deallocate(p1);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING GUARD FUNCTIONALITY
        //
        // Concerns:
        // 1. The guard restores the state of the test allocator as if it
        //    itself have never existed (by calling the test allocator's stash
        //    and restore methods that are tested comprehensively elsewhere).
        //
        // Plan:
        // 1. Create a test allocator, perform several allocations (retaining
        //    the returned memory addresses).  Then create a test allocator
        //    statistics guard with its own scope.
        //
        // 2. Immediately after the creation of the guard object verify that
        //    the statistics have been reset to the expected values.
        //
        // 3. Perform further allocations and deallocations within that scope.
        //
        // 4. Right after the end of the scope verify that the test allocator
        //    statistics reflect all allocations and deallocations as if the
        //    guard have not existed (and have not reset the values).
        //
        // Testing:
        //   bslma::TestAllocatorStatiscticsGuard(bslma::TestAllocator *);
        //   ~bslma::TestAllocatorStatiscticsGuard();
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING GUARD FUNCTIONALITY"
                          "\n===========================");

        bslma::TestAllocator ta("testAllocator", veryVeryVeryVerbose);

        void *p1 = ta.allocate(1024);
        void *p2 = ta.allocate(1024);
        void *p3 = ta.allocate(1024);
        ta.deallocate(ta.allocate(1024));

        {
            bslma::TestAllocatorStatisticsGuard guard(&ta);

            // Verify that stash was done
            ASSERTV(guard.originalNumBlocksInUse(),
                    3 == guard.originalNumBlocksInUse());
            ASSERTV(guard.originalNumBytesInUse(),
                    3072 == guard.originalNumBytesInUse());

            // Verify that reset was done
            ASSERTV(ta.numAllocations(),    3 == ta.numAllocations());
            ASSERTV(ta.numBlocksInUse(),    3 == ta.numBlocksInUse());
            ASSERTV(ta.numBytesInUse(),  3072 == ta.numBytesInUse());

            ASSERTV(ta.numDeallocations(), 0 == ta.numDeallocations());

            ASSERTV(ta.numBlocksMax(),    3 == ta.numBlocksMax());
            ASSERTV(ta.numBytesMax(),  3072 == ta.numBytesMax());

            ASSERTV(ta.numBlocksTotal(),   3 == ta.numBlocksTotal());
            ASSERTV(ta.numBytesTotal(), 3072 == ta.numBytesTotal());

            ASSERTV(ta.numMismatches(),   0 == ta.numMismatches());
            ASSERTV(ta.numBoundsErrors(), 0 == ta.numBoundsErrors());

            void *p4 = ta.allocate(1024);
            void *p5 = ta.allocate(1024);
            ta.deallocate(ta.allocate(1024));
            ta.deallocate(p5);
            ta.deallocate(p4);
        }
        ASSERTV(ta.numAllocations(),    7 == ta.numAllocations());
        ASSERTV(ta.numBlocksInUse(),    3 == ta.numBlocksInUse());
        ASSERTV(ta.numBytesInUse(),  3072 == ta.numBytesInUse());

        ASSERTV(ta.numDeallocations(), 4 == ta.numDeallocations());

        ASSERTV(ta.numBlocksMax(),    6 == ta.numBlocksMax());
        ASSERTV(ta.numBytesMax(),  6144 == ta.numBytesMax());

        ASSERTV(ta.numBlocksTotal(),   7 == ta.numBlocksTotal());
        ASSERTV(ta.numBytesTotal(), 7168 == ta.numBytesTotal());

        ASSERTV(ta.numMismatches(),   0 == ta.numMismatches());
        ASSERTV(ta.numBoundsErrors(), 0 == ta.numBoundsErrors());

        ta.deallocate(p3);
        ta.deallocate(p2);
        ta.deallocate(p1);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
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
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
