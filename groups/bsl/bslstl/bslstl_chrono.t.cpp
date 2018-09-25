// bslstl_chrono.t.cpp                                                -*-C++-*-
#include <bslstl_chrono.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>     // 'sprintf', 'snprintf' [NOT '<cstdio>', which does not
                       // include 'snprintf']
#include <stdlib.h>    // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides a set of user-defined literal operators
// that forms a duration objects having various duration periods.  This test
// driver tests each implemented UDL operator.
//-----------------------------------------------------------------------------
// FREE OPERATORS
// [1] bsl::chrono::hours        operator "" _h  (unsigned long long);
// [1] bsl::chrono::duration     operator "" _h  (long double);
// [1] bsl::chrono::minutes      operator "" _min(unsigned long long);
// [1] bsl::chrono::duration     operator "" _min(long double);
// [1] bsl::chrono::seconds      operator "" _s  (unsigned long long);
// [1] bsl::chrono::duration     operator "" _s  (long double);
// [1] bsl::chrono::milliseconds operator "" _ms (unsigned long long);
// [1] bsl::chrono::duration     operator "" _ms (long double);
// [1] bsl::chrono::microseconds operator "" _us (unsigned long long);
// [1] bsl::chrono::duration     operator "" _us (long double);
// [1] bsl::chrono::nanoseconds  operator "" _ns (unsigned long long);
// [1] bsl::chrono::duration     operator "" _ns (long double);
// ----------------------------------------------------------------------------
// [2] USAGE EXAMPLE

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
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Usage Example"
                            "\n=====================\n");

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Basic 'bsl'-chrono's UDLs Usage
/// - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates basic use of user-defined literal operators.
//
// First, we provide an access to 'bsl'-chrono's UDLs.

        using namespace bsl::chrono_literals;

// Then, we construct two duration objects that represent a 24-hours and a half
// an hour time intervals using 'operator "" _h'.

        auto hours_in_a_day = 24_h;
        auto halfhour       = 0.5_h;

// Finally, stream the two objects to 'stdout':

        printf("one day is %ld hours\n", hours_in_a_day.count());
        printf("half an hour is %.1f hours\n",
               static_cast<double>(halfhour.count()));
#else
        if (veryVerbose) { printf("Cannot test user-defined literals "
                                  "in pre-C++11 mode or if the compiler "
                                  "does not support inline namespaces.\n"); }
#endif

      } break;
    case 1: {
      // ----------------------------------------------------------------------
      // TESTING User-defined literal operators
      //
      // Concerns:
      //: 1 That UDL operators correctly forward arguments to corresponding
      //:   constructors of 'bsl::chrono::duration' objects.
      //:
      //: 2 That an access to UDL operators can be gained using either
      //:   'bsl::literals', 'bsl::chrono_literals' or
      //:   'bsl::literals::chrono_literals' namespaces.
      //
      // Plan:
      //: 1 Verify that all UDL operators create expected chrono type objects
      //:   having the expected values.
      //
      // Testing:
      //   bsl::chrono::hours        operator "" _h  (unsigned long long);
      //   bsl::chrono::duration     operator "" _h  (long double);
      //   bsl::chrono::minutes      operator "" _min(unsigned long long);
      //   bsl::chrono::duration     operator "" _min(long double);
      //   bsl::chrono::seconds      operator "" _s  (unsigned long long);
      //   bsl::chrono::duration     operator "" _s  (long double);
      //   bsl::chrono::milliseconds operator "" _ms (unsigned long long);
      //   bsl::chrono::duration     operator "" _ms (long double);
      //   bsl::chrono::microseconds operator "" _us (unsigned long long);
      //   bsl::chrono::duration     operator "" _us (long double);
      //   bsl::chrono::nanoseconds  operator "" _ns (unsigned long long);
      //   bsl::chrono::duration     operator "" _ns (long double);
      // ----------------------------------------------------------------------

        if (verbose) printf("\nTesting User-defined literal operators"
                            "\n======================================\n");

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)


        if (verbose) printf("Testing 'operator \"\" _h'\n");
        { // C-1,2
            using namespace bsl::chrono_literals;
            bsl::chrono::hours h = 1_h;
            ASSERT(1 == h.count());
            auto duration = 1.5_h;
            ASSERT(1.5 == duration.count());
        }
        { // C-2
            using namespace bsl::literals;
            auto mX = 1_h;
            (void) mX;
            auto mY = 1.5_h;
            (void) mY;
        }
        { // C-2
            using namespace bsl::literals::chrono_literals;
            auto mX = 1_h;
            (void) mX;
            auto mY = 1.5_h;
            (void) mY;
        }

        if (verbose) printf("Testing 'operator \"\" _min'\n");
        { // C-1,2
            using namespace bsl::chrono_literals;
            bsl::chrono::minutes min = 2_min;
            ASSERT(2 == min.count());
            auto duration = 2.5_min;
            ASSERTV(2.5 == duration.count());
        }
        { // C-2
            using namespace bsl::literals;
            auto mX = 1_min;
            (void) mX;
            auto mY = 1.5_min;
            (void) mY;
        }
        { // C-2
            using namespace bsl::literals::chrono_literals;
            auto mX = 1_min;
            (void) mX;
            auto mY = 1.5_min;
            (void) mY;
        }

        if (verbose) printf("Testing 'operator \"\" _s'\n");
        { // C-1,2
            using namespace bsl::chrono_literals;
            bsl::chrono::seconds sec = 3_s;
            ASSERT(3 == sec.count());
            auto duration = 3.5_s;
            ASSERTV(3.5 == duration.count());
        }
        { // C-2
            using namespace bsl::literals;
            auto mX = 1_s;
            (void) mX;
            auto mY = 1.5_s;
            (void) mY;
        }
        { // C-2
            using namespace bsl::literals::chrono_literals;
            auto mX = 1_s;
            (void) mX;
            auto mY = 1.5_s;
            (void) mY;
        }

        if (verbose) printf("Testing 'operator \"\" _ms'\n");
        { // C-1,2
            using namespace bsl::chrono_literals;
            bsl::chrono::milliseconds ms = 4_ms;
            ASSERT(4 == ms.count());
            auto duration = 4.5_ms;
            ASSERTV(4.5 == duration.count());
        }
        { // C-2
            using namespace bsl::literals;
            auto mX = 1_ms;
            (void) mX;
            auto mY = 1.5_ms;
            (void) mY;
        }
        { // C-2
            using namespace bsl::literals::chrono_literals;
            auto mX = 1_ms;
            (void) mX;
            auto mY = 1.5_ms;
            (void) mY;
        }

        if (verbose) printf("Testing 'operator \"\" _us'\n");
        { // C-1,2
            using namespace bsl::chrono_literals;
            bsl::chrono::microseconds us = 5_us;
            ASSERT(5 == us.count());
            auto duration = 5.5_us;
            ASSERTV(5.5 == duration.count());
        }
        { // C-2
            using namespace bsl::literals;
            auto mX = 1_us;
            (void) mX;
            auto mY = 1.5_us;
            (void) mY;
        }
        { // C-2
            using namespace bsl::literals::chrono_literals;
            auto mX = 1_us;
            (void) mX;
            auto mY = 1.5_us;
            (void) mY;
        }

        if (verbose) printf("Testing 'operator \"\" _ns'\n");
        { // C-1,2
            using namespace bsl::chrono_literals;
            bsl::chrono::nanoseconds ns = 6_ns;
            ASSERT(6 == ns.count());
            auto duration = 6.5_ns;
            ASSERTV(6.5 == duration.count());
        }
        { // C-2
            using namespace bsl::literals;
            auto mX = 1_ns;
            (void) mX;
            auto mY = 1.5_ns;
            (void) mY;
        }
        { // C-2
            using namespace bsl::literals::chrono_literals;
            auto mX = 1_ns;
            (void) mX;
            auto mY = 1.5_ns;
            (void) mY;
        }
#else
        if (veryVerbose) { printf("Cannot test user-defined literals "
                                  "in pre-C++11 mode or if the compiler "
                                  "does not support inline namespaces.\n"); }

#endif  // BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

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
