// bslstl_concepts.t.cpp                                              -*-C++-*-
#include <bslstl_concepts.h>

#include <bslmf_assert.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

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
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Add any component-related code here.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
        BSLMF_ASSERT(( bsl::same_as<int, int >));
        BSLMF_ASSERT((!bsl::same_as<int, char>));

        {
            class B {};
            class D : public B {};
            BSLMF_ASSERT(( bsl::derived_from<D, B>));
            BSLMF_ASSERT((!bsl::derived_from<B, D>));
        }
        {
            class C {};
            BSLMF_ASSERT(( bsl::convertible_to<char, int >));
            BSLMF_ASSERT(( bsl::convertible_to<int,  char>));
            BSLMF_ASSERT(( bsl::convertible_to<int,  int >));
            BSLMF_ASSERT((!bsl::convertible_to<C,    int >));
        }
        {
            class C {};
            BSLMF_ASSERT(( bsl::common_reference_with<int,   char      >));
            BSLMF_ASSERT(( bsl::common_reference_with<int,   const int >));
            BSLMF_ASSERT((!bsl::common_reference_with<C,     int       >));
            BSLMF_ASSERT((!bsl::common_reference_with<int *, unsigned *>));
        }

        BSLMF_ASSERT(( bsl::common_with<int,   char  >));
        BSLMF_ASSERT(( bsl::common_with<int *, void *>));
        BSLMF_ASSERT((!bsl::common_with<int *, char *>));

        BSLMF_ASSERT(( bsl::integral<int   >));
        BSLMF_ASSERT(( bsl::integral<char  >));
        BSLMF_ASSERT((!bsl::integral<double>));

        BSLMF_ASSERT(( bsl::signed_integral<int     >));
        BSLMF_ASSERT((!bsl::signed_integral<unsigned>));

        BSLMF_ASSERT(( bsl::unsigned_integral<unsigned>));
        BSLMF_ASSERT((!bsl::unsigned_integral<int     >));

        BSLMF_ASSERT(( bsl::floating_point<double>));
        BSLMF_ASSERT((!bsl::floating_point<int   >));

        BSLMF_ASSERT(( bsl::assignable_from<int&, int>));
        BSLMF_ASSERT((!bsl::assignable_from<int,  int>));

        {
            struct C {
                C(const C&)            = delete;
                C &operator=(const C&) = delete;
            };
            BSLMF_ASSERT(( bsl::swappable<int>));
            BSLMF_ASSERT((!bsl::swappable<C  >));
        }

        BSLMF_ASSERT(( bsl::swappable_with<int&, int&>));
        BSLMF_ASSERT((!bsl::swappable_with<int,  int >));

        {
            class C { ~C() = delete; };
            BSLMF_ASSERT(( bsl::destructible<int>));
            BSLMF_ASSERT((!bsl::destructible<C  >));
        }

        BSLMF_ASSERT(( bsl::constructible_from<int, int >));
        BSLMF_ASSERT((!bsl::constructible_from<int, int*>));

        BSLMF_ASSERT(( bsl::default_initializable<int >));
        BSLMF_ASSERT((!bsl::default_initializable<int&>));

        {
            struct C { C(C&&) = delete; };
            BSLMF_ASSERT(( bsl::move_constructible<int>));
            BSLMF_ASSERT((!bsl::move_constructible<C  >));
        }

        {
            struct C { C(const C&) = delete; };
            BSLMF_ASSERT(( bsl::copy_constructible<int>));
            BSLMF_ASSERT((!bsl::copy_constructible<C  >));
        }

        {
            struct C {};

            BSLMF_ASSERT(( bsl::equality_comparable<int>));
            BSLMF_ASSERT((!bsl::equality_comparable<C>));

            BSLMF_ASSERT(( bsl::equality_comparable_with<int, short>));
            BSLMF_ASSERT((!bsl::equality_comparable_with<int, C>));

            BSLMF_ASSERT(( bsl::totally_ordered<int>));
            BSLMF_ASSERT((!bsl::totally_ordered<C>));

            BSLMF_ASSERT(( bsl::totally_ordered_with<int, short>));
            BSLMF_ASSERT((!bsl::totally_ordered_with<int, C>));
        }

        {
            struct C {
                C(C&&) = delete;
                C& operator=(C&&) = delete;
            };
            BSLMF_ASSERT(( bsl::movable<int>));
            BSLMF_ASSERT((!bsl::movable<C>));
        }

        {
            struct C {
                C(const C&) = delete;
                C& operator=(const C&) = delete;
            };
            BSLMF_ASSERT(( bsl::copyable<int>));
            BSLMF_ASSERT((!bsl::copyable<C>));
        }

        {
            struct C { C() = delete; };
            BSLMF_ASSERT(( bsl::semiregular<int>));
            BSLMF_ASSERT((!bsl::semiregular<C>));
        }

        {
            struct C1 { bool operator==(const C1&) const = default; };
            struct C2 {};
            BSLMF_ASSERT(( bsl::regular<C1>));
            BSLMF_ASSERT((!bsl::regular<C2>));
        }

        {
            const auto func = [](int) {};

            BSLMF_ASSERT(( bsl::invocable<decltype(func), int>));
            BSLMF_ASSERT((!bsl::invocable<decltype(func)>));

            BSLMF_ASSERT(( bsl::regular_invocable<decltype(func), int>));
            BSLMF_ASSERT((!bsl::regular_invocable<decltype(func)>));
        }

        {
            const auto pred = [](int) { return true; };
            const auto func = [](int) {};

            BSLMF_ASSERT(( bsl::predicate<decltype(pred), int>));
            BSLMF_ASSERT((!bsl::predicate<decltype(func), int>));
        }

        {
            struct C {};
            const auto rel = [](int, int) { return true; };

            BSLMF_ASSERT(( bsl::relation<decltype(rel), int, int>));
            BSLMF_ASSERT((!bsl::relation<decltype(rel), int, C>));

            BSLMF_ASSERT(( bsl::equivalence_relation<decltype(rel), int,int>));
            BSLMF_ASSERT((!bsl::equivalence_relation<decltype(rel), int, C>));

            BSLMF_ASSERT(( bsl::strict_weak_order<decltype(rel), int, int>));
            BSLMF_ASSERT((!bsl::strict_weak_order<decltype(rel), int, C>));
        }
#endif  // else BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS
        ASSERT(true);
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
// Copyright 2025 Bloomberg Finance L.P.
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
