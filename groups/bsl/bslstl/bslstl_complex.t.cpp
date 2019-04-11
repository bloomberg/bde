// bslstl_complex.t.cpp                                               -*-C++-*-
#include <bslstl_complex.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <limits>
#include <cfloat>

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
// that forms a 'bsl::complex' objects having various ranges of values.  This
// test driver tests each implemented UDL operator.
//-----------------------------------------------------------------------------
// FREE OPERATORS
// [1] bsl::complex<double>      operator "" _i (unsigned long long);
// [1] bsl::complex<double>      operator "" _i (long double);
// [1] bsl::complex<float>       operator "" _if(unsigned long long);
// [1] bsl::complex<float>       operator "" _if(long double);
// [1] bsl::complex<long double> operator "" _il(unsigned long long);
// [1] bsl::complex<long double> operator "" _il(long double);
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
///Example 1: Basic 'bsl'-complex's UDLs Usage
/// - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates basic use of user-defined literal operators.
//
// First, we provide an access to 'bsl'-complex's UDLs.

        using namespace bsl::complex_literals;

// Then, we construct a 'bsl::complex' object 'z'.

        auto z = 1.0 + 1_i;

// Finally, stream the magnitude of the complex number 'z' to 'stdout':

        printf("abs(%.1f,%.1f) = %f\n", z.real(), z.imag(), bsl::abs(z));
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
      //:   constructors of the 'bsl::complex<T>' type.
      //:
      //: 2 That UDL operators correctly cast the argument's type to the
      //:   the 'value_type' of the resultant 'bsl::complex<T>' type.
      //:
      //: 3 That an access to UDL operators can be gained using either
      //:   'bsl::literals', 'bsl::complex_literals' or
      //:   'bsl::literals::complex_literals' namespaces.
      //
      // Plan:
      //: 1 Verify that all UDL operators create expected 'bsl::complex<T>'
      //:   type objects having the expected values.
      //:
      //: 2 Using table-driven technique test that UDL operators produce the
      //:   expected 'bsl::complex<T>' objects for the various values.
      //
      // Testing:
      //   bsl::complex<double>      operator "" _i (unsigned long long);
      //   bsl::complex<double>      operator "" _i (long double);
      //   bsl::complex<float>       operator "" _if(unsigned long long);
      //   bsl::complex<float>       operator "" _if(long double);
      //   bsl::complex<long double> operator "" _il(unsigned long long);
      //   bsl::complex<long double> operator "" _il(long double);
      // ----------------------------------------------------------------------

        if (verbose) printf("\nTesting User-defined literal operators"
                             "\n======================================\n");

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)

        if (verbose) printf("Testing 'operator \"\" _i'\n");
        { // C-1,2
            using namespace bsl::complex_literals;

            using Obj = bsl::complex<double>;

            struct {
                int             d_line;
                Obj             d_value;
                Obj::value_type d_expected;
            } DATA[] = {
               //------------------------------------------------------------
               // LINE |        VALUE        |           EXPECTED
               //------------------------------------------------------------
               { L_,                      0_i,                            0. },
               { L_,             1020304050_i,                   1020304050. },
               { L_,     0xffffffffffffffff_i, static_cast<Obj::value_type>(
                                                         0xffffffffffffffff) },
               { L_,                    0.0_i,                            0. },
               { L_,                   -0.0_i,                           -0. },
               { L_,               2.3e-323_i,                      2.3e-323 },
               { L_,              -2.3e-323_i,                     -2.3e-323 },
               { L_,               1.7e+308_i,                      1.7e+308 },
               { L_,              -1.7e+308_i,                     -1.7e+308 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE     = DATA[ti].d_line;
                const Obj&             VALUE    = DATA[ti].d_value;
                const Obj::value_type& EXPECTED = DATA[ti].d_expected;

                LOOP3_ASSERT(LINE,
                             EXPECTED,
                             VALUE.imag(),
                             EXPECTED == VALUE.imag());
            }
            { // C-3
                using namespace bsl::literals;
                Obj mX = 1_i;
                (void) mX;
                Obj mY = 1.5_i;
                (void) mY;
            }
            { // C-3
                using namespace bsl::complex_literals;
                Obj mX = 1_i;
                (void) mX;
                Obj mY = 1.5_i;
                (void) mY;
            }
            { // C-3
                using namespace bsl::literals::complex_literals;
                Obj mX = 1_i;
                (void) mX;
                Obj mY = 1.5_i;
                (void) mY;
            }
        }

        if (verbose) printf("Testing 'operator \"\" _if'\n");
        { // C-1,2
            using namespace bsl::complex_literals;

            using Obj = bsl::complex<float>;

            struct {
                int             d_line;
                Obj             d_value;
                Obj::value_type d_expected;
            } DATA[] = {
              //-------------------------------------------------------------
              // LINE |        VALUE         |           EXPECTED
              //-------------------------------------------------------------
              { L_,                      0_if,                           0.f },
              { L_,               10203040_if,                    10203040.f },
              { L_,     0xffffffffffffffff_if, static_cast<Obj::value_type>(
                                                         0xffffffffffffffff) },
              { L_,                    0.0_if,                           0.f },
              { L_,                   -0.0_if,                          -0.f },
              { L_,                1.5e-45_if,                      1.5e-45f },
              { L_,               -1.5e-45_if,                     -1.5e-45f },
              { L_,                3.4e+38_if,                      3.4e+38f },
              { L_,               -3.4e+38_if,                     -3.4e+38f },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE     = DATA[ti].d_line;
                const Obj&             VALUE    = DATA[ti].d_value;
                const Obj::value_type& EXPECTED = DATA[ti].d_expected;

                LOOP3_ASSERT(LINE,
                             EXPECTED,
                             VALUE.imag(),
                             EXPECTED == VALUE.imag());
            }
            { // C-3
                using namespace bsl::literals;
                Obj mX = 1_if;
                (void) mX;
                Obj mY = 1.5_if;
                (void) mY;
            }
            { // C-3
                using namespace bsl::complex_literals;
                Obj mX = 1_if;
                (void) mX;
                Obj mY = 1.5_if;
                (void) mY;
            }
            { // C-3
                using namespace bsl::literals::complex_literals;
                Obj mX = 1_if;
                (void) mX;
                Obj mY = 1.5_if;
                (void) mY;
            }
        }

        if (verbose) printf("Testing 'operator \"\" _il'\n");
        { // C-1,2
            using namespace bsl::complex_literals;

            using Obj = bsl::complex<long double>;

            struct {
                int             d_line;
                Obj             d_value;
                Obj::value_type d_expected;
            } DATA[] = {
              //--------------------------------------------------------------
              // LINE |        VALUE         |           EXPECTED
              //--------------------------------------------------------------
              { L_,                      0_il,                           0.L },
              { L_,               10203040_il,                    10203040.L },
              { L_,     0xffffffffffffffff_il, static_cast<Obj::value_type>(
                                                         0xffffffffffffffff) },
              { L_,                    0.0_il,                           0.L },
              { L_,                   -0.0_il,                          -0.L },
              { L_,               2.3e-323_il,                     2.3e-323L },
              { L_,              -2.3e-323_il,                    -2.3e-323L },
              { L_,               1.7e+308_il,                     1.7e+308L },
              { L_,              -1.7e+308_il,                    -1.7e+308L },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);


            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE     = DATA[ti].d_line;
                const Obj&             VALUE    = DATA[ti].d_value;
                const Obj::value_type& EXPECTED = DATA[ti].d_expected;

                LOOP3_ASSERT(LINE,
                             EXPECTED,
                             VALUE.imag(),
                             EXPECTED == VALUE.imag());
            }
            { // C-3
                using namespace bsl::literals;
                Obj mX = 1_il;
                (void) mX;
                Obj mY = 1.5_il;
                (void) mY;
            }
            { // C-3
                using namespace bsl::complex_literals;
                Obj mX = 1_il;
                (void) mX;
                Obj mY = 1.5_il;
                (void) mY;
            }
            { // C-3
                using namespace bsl::literals::complex_literals;
                Obj mX = 1_il;
                (void) mX;
                Obj mY = 1.5_il;
                (void) mY;
            }
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
