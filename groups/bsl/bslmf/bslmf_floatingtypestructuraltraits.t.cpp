// bslmf_floatingtypestructuraltraits.t.cpp                           -*-C++-*-

#include <bslmf_floatingtypestructuraltraits.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // `printf`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
// This component provides a metafunction with constants and conversion
// functions to be used when deconstructing or constructing floating point
// values of type `float`, or `double`.
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLES - TBD
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
        fflush(stdout);

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

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                              TEST HELPERS
// ----------------------------------------------------------------------------

template <class t_FLOATING>
void testExponentTraits(int LINE, t_FLOATING fpValue, int expectedExponent)
{
    typedef bslmf::FloatingTypeStructuralTraits<t_FLOATING> Traits;
    typedef typename Traits::UintType UintType;

    const UintType uintVal = Traits::toUintType(fpValue);
    const UintType exponent = (
        (uintVal & Traits::k_SHIFTED_EXPONENT_MASK) >> Traits::k_EXPONENT_SHIFT
                              ) - Traits::k_EXPONENT_BIAS;
    ASSERTV(LINE, exponent, expectedExponent,
            exponent == static_cast<UintType>(expectedExponent));

    const UintType firstShiftedExponent =
        ((uintVal >> Traits::k_EXPONENT_SHIFT) & Traits::k_EXPONENT_MASK)
                                                      - Traits::k_EXPONENT_BIAS;
    ASSERTV(LINE, exponent, firstShiftedExponent,
            exponent == firstShiftedExponent);
}

template <class t_FLOATING>
void testMantissaTraits(int                 LINE,
                        t_FLOATING          fpValue,
                        unsigned long long  expectedMantissa)
{
    typedef bslmf::FloatingTypeStructuralTraits<t_FLOATING> Traits;
    typedef typename Traits::UintType UintType;

    const UintType uintVal  = Traits::toUintType(fpValue);
    const UintType mantissa = uintVal & Traits::k_NORMAL_MANTISSA_MASK;
    ASSERTV(LINE, mantissa, expectedMantissa, mantissa == expectedMantissa);
}

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;  (void)veryVeryVerbose;
    bool veryVeryVeryVerbose = argc > 5;  (void)veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        // 1. That the usage examples shown in the component-level
        //    documentation compile and run as described.
        //
        // Plan:
        // 1. Copy the usage examples from the component header, changing
        //    `assert` to `ASSERT` and execute them.
        //
        // Testing:
        //     USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");
        // TBD
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. Converting to `UintType` and back works.
        // 2. Mask, shift, and combined values work.
        // 3. Both `float` and `double` work.
        //
        // Plan:
        // 1. Round-trip between `UintType` and floating types
        // 2. Verify sign, exponent, and mantissa masks with known values.
        //
        // Testing:
        //    BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============\n");

        if (veryVerbose) puts("\tConverters");
        {   // float
            typedef bslmf::FloatingTypeStructuralTraits<float> Traits;

            const float fpVal = 3.14159f;
            const Traits::UintType uintVal = Traits::toUintType(fpVal);
            const float roundTripFpVal = Traits::toFloatType(uintVal);
            ASSERTV(fpVal, roundTripFpVal, fpVal == roundTripFpVal);
        }
        {   // double
            typedef bslmf::FloatingTypeStructuralTraits<double> Traits;

            const double fpVal = 3.141592653589793;
            const Traits::UintType uintVal = Traits::toUintType(fpVal);
            const double roundTripFpVal = Traits::toFloatType(uintVal);
            ASSERTV(fpVal, roundTripFpVal, fpVal == roundTripFpVal);
        }

        if (veryVerbose) puts("\tShifted Sign Mask");
        {   // float
            typedef bslmf::FloatingTypeStructuralTraits<float> Traits;

            Traits::UintType uintVal = Traits::toUintType(3.14159f);
            ASSERTV(uintVal & Traits::k_SHIFTED_SIGN_MASK,
                    0 == (uintVal & Traits::k_SHIFTED_SIGN_MASK));

            uintVal = Traits::toUintType(-3.14159f);
            ASSERTV(uintVal & Traits::k_SHIFTED_SIGN_MASK,
                    Traits::k_SHIFTED_SIGN_MASK ==
                                      (uintVal & Traits::k_SHIFTED_SIGN_MASK));
        }
        {   // double
            typedef bslmf::FloatingTypeStructuralTraits<double> Traits;

            Traits::UintType uintVal = Traits::toUintType(3.141592653589793);
            ASSERTV(uintVal & Traits::k_SHIFTED_SIGN_MASK,
                    0 == (uintVal & Traits::k_SHIFTED_SIGN_MASK));

            uintVal = Traits::toUintType(-3.141592653589793);
            ASSERTV(uintVal & Traits::k_SHIFTED_SIGN_MASK,
                    Traits::k_SHIFTED_SIGN_MASK ==
                                      (uintVal & Traits::k_SHIFTED_SIGN_MASK));
        }

        if (veryVerbose) puts("\tExponent Mask, Shift, Shifted Mask & Bias");
        {   // float, positive exponent
            testExponentTraits(L_,  1.0f, 0);
            testExponentTraits(L_,  2.0f, 1);
            testExponentTraits(L_,  3.0f, 1);
            testExponentTraits(L_,  4.0f, 2);
            testExponentTraits(L_,  5.0f, 2);
            testExponentTraits(L_,  6.0f, 2);
            testExponentTraits(L_,  7.0f, 2);
            testExponentTraits(L_,  8.0f, 3);
            testExponentTraits(L_,  9.0f, 3);
            testExponentTraits(L_, 12.0f, 3);
            testExponentTraits(L_, 15.0f, 3);
            testExponentTraits(L_, 16.0f, 4);

            // float, negative exponent
            testExponentTraits(L_, .5f,   -1);
            testExponentTraits(L_, .25f,  -2);
            testExponentTraits(L_, .125f, -3);
        }

        {   // double, positive exponent
            testExponentTraits(L_,  1.0, 0);
            testExponentTraits(L_,  2.0, 1);
            testExponentTraits(L_,  3.0, 1);
            testExponentTraits(L_,  4.0, 2);
            testExponentTraits(L_,  5.0, 2);
            testExponentTraits(L_,  6.0, 2);
            testExponentTraits(L_,  7.0, 2);
            testExponentTraits(L_,  8.0, 3);
            testExponentTraits(L_,  9.0, 3);
            testExponentTraits(L_, 12.0, 3);
            testExponentTraits(L_, 15.0, 3);
            testExponentTraits(L_, 16.0, 4);

            // double, negative exponent
            testExponentTraits(L_, .5,   -1);
            testExponentTraits(L_, .25,  -2);
            testExponentTraits(L_, .125, -3);
        }


        if (veryVerbose) puts("\tMantissa Mask");
        {   // float, positive
            testMantissaTraits(L_,  0.0f, 0x0);
            testMantissaTraits(L_,  1.0f, 0x800000llu);
            testMantissaTraits(L_,  2.0f, 0x0);
            testMantissaTraits(L_,  3.0f, 0x400000llu);
            testMantissaTraits(L_,  4.0f, 0x800000llu);

            // float, negative
            testMantissaTraits(L_, -0.0f, 0x0);
            testMantissaTraits(L_, -1.0f, 0x800000llu);
            testMantissaTraits(L_, -2.0f, 0x0);
            testMantissaTraits(L_, -3.0f, 0x400000llu);
            testMantissaTraits(L_, -4.0f, 0x800000llu);

            // float, fractions
            testMantissaTraits(L_,  0.5f,   0x0);
            testMantissaTraits(L_,  0.25f,  0x800000llu);
            testMantissaTraits(L_,  0.125f, 0x0);
            testMantissaTraits(L_,  0.75f,  0x400000llu);
        }

        {   // double, positive
            testMantissaTraits(L_,  0.0, 0x0);
            testMantissaTraits(L_,  1.0, 0x10000000000000llu);
            testMantissaTraits(L_,  2.0, 0x0);
            testMantissaTraits(L_,  3.0, 0x08000000000000llu);
            testMantissaTraits(L_,  4.0, 0x10000000000000llu);

            // double, negative
            testMantissaTraits(L_, -0.0, 0x0);
            testMantissaTraits(L_, -1.0, 0x10000000000000llu);
            testMantissaTraits(L_, -2.0, 0x0);
            testMantissaTraits(L_, -3.0, 0x08000000000000llu);
            testMantissaTraits(L_, -4.0, 0x10000000000000llu);

            // double, fractions
            testMantissaTraits(L_,  0.5,   0x0);
            testMantissaTraits(L_,  0.25,  0x10000000000000llu);
            testMantissaTraits(L_,  0.125, 0x0);
            testMantissaTraits(L_,  0.75,  0x08000000000000llu);
        }
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
