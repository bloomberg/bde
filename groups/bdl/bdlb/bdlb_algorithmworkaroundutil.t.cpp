// bdlb_algorithmworkaroundutil.t.cpp                                 -*-C++-*-
#include <bdlb_algorithmworkaroundutil.h>

#include <bslma_default.h>
#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_functional.h>

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>      // atoi
#include <string.h>      // strlen

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// The component under test provides workarounds for incorrectly implemented
// standard library algorithms.
//
// ----------------------------------------------------------------------------
//
// [ 2] IT lower_bound(IT , IT , const TYPE& )
// [ 2] IT lower_bound(IT , IT , const TYPE&, COMPARE )
// [ 2] IT upper_bound(IT , IT , const TYPE& )
// [ 2] IT upper_bound(IT , IT , const TYPE&, COMPARE )
// ----------------------------------------------------------------------------
// [  ] BREATHING TEST
// [  ] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

// ============================================================================
//                       GLOBAL TEST ALIASES
// ----------------------------------------------------------------------------

typedef bdlb::AlgorithmWorkaroundUtil Util;

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING: lower_bound, upper_bound
        //
        // Concerns:
        //: 1 That lower_bound returns the *first* match from a sequence of
        //:   values.
        //:
        //: 2 That upper_bound returns the *last* match from a sequence of
        //:   values.
        //:
        //: 3 That both lower_bound and upper_bound both function with and
        //:   without a supplied comparator.
        //
        // Plan:
        //: 1 Create an input array of integers and and perform a table based
        //:   test ensuring the upper_bound and lower_bound functions return
        //:   the expected results.
        //
        // Testing:
        //   IT lower_bound(IT , IT , const TYPE& )
        //   IT lower_bound(IT , IT , const TYPE&, COMPARE )
        //   IT upper_bound(IT , IT , const TYPE& )
        //   IT upper_bound(IT , IT , const TYPE&, COMPARE )
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: lower_bound, upper_bound"
                            "\n=================================\n");

        if (verbose) printf("\tTable based test\n");


        {
            const int INPUT[]   = {0, 0, 1, 3, 3, 3, 5, 7 };
            const int NUM_INPUT = sizeof(INPUT) / sizeof(*INPUT);

            struct TestData {
                    int  d_searchValue;
                    int  d_lowerBoundIndex;
                    int  d_upperBoundIndex;
                    bool d_exactMatch;
            } DATA[] = {
                { -1,  0,  0, false },
                {  0,  0,  2,  true },
                {  1,  2,  3,  true },
                {  2,  3,  3, false },
                {  3,  3,  6,  true },
                {  4,  6,  6, false },
                {  5,  6,  7,  true },
                {  6,  7,  7, false },
                {  7,  7,  8,  true },
                {  8,  8,  8, false },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int  VALUE     = DATA[i].d_searchValue;
                const int  EXP_LOWER = DATA[i].d_lowerBoundIndex;
                const int  EXP_UPPER = DATA[i].d_upperBoundIndex;
                const int  EXACT     = DATA[i].d_exactMatch;
                const int *END       = INPUT + NUM_INPUT;

                // Note that the conditions for termination of 'lower_bound'
                // on, for example, cppreference.com:
                //
                // "Returns an iterator pointing to the first element in the
                // range [first, last) that is not less than (i.e. greater or
                // equal to) value.".
                //
                // Similar condition exists for 'upper_bound'.
                //
                // Below we test returned iterator ('result') meets that
                // condition (if it is not the end iterator).  Then, we test
                // the preceding position ('prevValue') does *not* meet the
                // condition (if the returned iterator does not refer to the
                // first element).  Thereby verifying the returned iterator is
                // the *first* element in the range meeting termination
                // condition for the algorithm.

                {
                    const int *result = Util::lowerBound(INPUT, END, VALUE);
                    const long offset = result - INPUT;

                    ASSERTV(EXP_LOWER, offset, EXP_LOWER == offset);
                    if (result < END) {
                        ASSERTV(*result, VALUE, !(*result < VALUE));
                    }
                    if (result != INPUT) {
                        const int prevValue = *(result - 1);
                        ASSERTV(prevValue, VALUE, !!(prevValue < VALUE));
                    }
                    if (EXACT) {
                        ASSERTV(*result, VALUE, *result == VALUE);
                    }
                }
                {
                    const int *result =
                        Util::lowerBound(INPUT, END, VALUE, bsl::less<int>());

                    const long offset = result - INPUT;

                    ASSERTV(EXP_LOWER, offset, EXP_LOWER == offset);
                    if (result < END) {
                        ASSERTV(*result, VALUE, !(*result < VALUE));
                    }
                    if (result != INPUT) {
                        const int prevValue = *(result - 1);
                        ASSERTV(prevValue, VALUE, !!(prevValue < VALUE));
                    }
                    if (EXACT) {
                        ASSERTV(*result, VALUE, *result == VALUE);
                    }
                }
                {
                    const int *result = Util::upperBound(INPUT, END, VALUE);
                    const long offset = result - INPUT;

                    ASSERTV(EXP_UPPER, offset, EXP_UPPER == offset);
                    if (result < END) {
                        ASSERTV(*result, VALUE, *result > VALUE);
                    }
                    if (result != INPUT) {
                        const int prevValue = *(result - 1);
                        ASSERTV(prevValue, VALUE, !(prevValue > VALUE));
                    }
                }
                {
                    const int *result =
                        Util::upperBound(INPUT, END, VALUE, bsl::less<int>());

                    const long offset = result - INPUT;

                    ASSERTV(EXP_UPPER, offset, EXP_UPPER == offset);
                    if (result < END) {
                        ASSERTV(*result, VALUE, *result > VALUE);
                    }
                    if (result != INPUT) {
                        const int prevValue = *(result - 1);
                        ASSERTV(prevValue, VALUE, !(prevValue > VALUE));
                    }
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Run each method with arbitrary inputs and verify the behavior is
        //:   as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
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

