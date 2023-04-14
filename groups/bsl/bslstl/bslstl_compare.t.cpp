// bslstl_compare.t.cpp                                               -*-C++-*-
#include <bslstl_compare.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides library support for three-way comparison
// operator '<=>'.
// ----------------------------------------------------------------------------
// [ 1] BASIC FUNCTIONALITY

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
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

// ============================================================================
//                         OTHER MACROS
// ----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TESTING BASIC FUNCTIONALITY
        //
        // Concerns:
        //: 1 The definitions from '<compare>' defined by the C++20 Standard
        //:   are available in C++20 mode in the 'bsl' namespace to users who
        //:   include 'bslstl_compare.h'.
        //
        // Plan:
        //: 1 For every identifier aliased from the 'std' namespace, verify
        //:   that the identifier exists and is usable with the 'bsl' namespace
        //:   prefix.
        //
        // Testing:
        //   BASIC FUNCTIONALITY
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING BASIC FUNCTIONALITY"
                            "\n===========================\n");

#ifdef BSLS_SUPPORT_THREE_WAY_COMPARISON
        BSLMF_ASSERT(__cpp_impl_three_way_comparison >= 201907L);
        // This fails on Apple Clang 14.0
        //BSLMF_ASSERT(__cpp_lib_three_way_comparison >= 201907L);

        {
            bsl::strong_ordering result = 1 <=> 0;
            ASSERT(result > 0);
        }
        {
            bsl::weak_ordering result = 1 <=> 0;
            ASSERT(result > 0);
        }
        {
            bsl::partial_ordering result = 1 <=> 0;
            ASSERT(result > 0);
        }
        BSLMF_ASSERT((bsl::three_way_comparable<int> ==
                      bsl::three_way_comparable_with<int, int>));

        BSLMF_ASSERT((bsl::is_same<
            bsl::common_comparison_category<bsl::strong_ordering,
                                            bsl::weak_ordering>::type,
            bsl::common_comparison_category_t<bsl::strong_ordering,
                                              bsl::weak_ordering>
        >::value));

        BSLMF_ASSERT((bsl::is_same<
            bsl::compare_three_way_result<int>::type,
            bsl::compare_three_way_result_t<int>
        >::value));

        ASSERT(bsl::strong_order(0, 1) < 0);
        ASSERT(bsl::weak_order(0, 1) < 0);
        ASSERT(bsl::partial_order(0, 1) < 0);

        ASSERT(bsl::compare_strong_order_fallback(0, 1) < 0);
        ASSERT(bsl::compare_weak_order_fallback(0, 1) < 0);
        ASSERT(bsl::compare_partial_order_fallback(0, 1) < 0);

        ASSERT(bsl::compare_three_way{}(0, 1) < 0);

        ASSERT(bsl::is_eq(1 <=> 1));
        ASSERT(bsl::is_neq(1 <=> 2));
        ASSERT(bsl::is_lt(1 <=> 2));
        ASSERT(bsl::is_lteq(1 <=> 2));
        ASSERT(bsl::is_gt(1 <=> 0));
        ASSERT(bsl::is_gteq(1 <=> 0));
#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-Zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
