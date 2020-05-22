// bslstl_ratio.t.cpp                                                 -*-C++-*-
#include <bslstl_ratio.h>

#include <bslmf_issame.h>

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
// The component under test implements a version of the ratio operations
// introduced into the standard library in C++11 (and 14).  We test to make
// sure that the operations exist and give sane results.  The tests are not
// exhaustive.
//
// Groups of operations tested:
// * Basic ratio functionality - numerator/denominator/nested 'type'
// * SI prefixes - kilo, mega, milli, etc.
// * Ratio comparisons - less/greater/equal, etc.
// ** This includes the '_v' inline variables
// * Ratio operations - add/subtract/multiply/divide, etc.
// ** There are no '_t' versions of these operations.
// ----------------------------------------------------------------------------
// [ 1] '<ratio>' STL header

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

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // VERIFYING '<ratio>' HEADER
        //
        // Concerns:
        //: 1 That the operations are successfully imported into the 'bsl'
        //:   namespace from the underlying STL implementation.
        //:
        //: 2 That the short versions of the comparison operators, such as
        //:   'ratio_less_v', exist for C++14 and C++17 modes (but not C++11).
        //
        // Plan:
        //: 1 Verify that the template 'ratio' exists in the 'bsl' namespace
        //:   and that it behaves like 'std::ratio'.
        //:
        //: 2 Verify that the SI 'typedef's exist and have the correct
        //:   numerator and denominator.
        //:
        //: 3 Verify that the comparison operations exist, and give expected
        //:   results.  Check again for the '_v' versions.
        //:
        //: 4 Verify that the arithmetic operations exist, and give expected
        //:   results.
        //
        // Testing:
        //   '<ratio>' STL header
        // --------------------------------------------------------------------

        if (verbose) printf("\nVERIFYING '<ratio>' HEADER"
                            "\n==========================\n");

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        if (verbose) printf("Cannot test 'bsl::ratio' in pre-C++11 mode.\n");
#else
        if (verbose) printf("\tTest basic 'ratio' functionality.\n");
        {
            typedef bsl::ratio<0, 1> Zero;
            typedef bsl::ratio<2>    Two;
            typedef bsl::ratio<6, 4> Six4ths;

            ASSERT(0 == Zero::num);
            ASSERT(1 == Zero::den);
            ASSERT(2 == Two::num);
            ASSERT(1 == Two::den);

            // make sure that the ratio is reduced
            ASSERT(3 == Six4ths::num);
            ASSERT(2 == Six4ths::den);

            // a nested 'typedef' named 'type'
            ASSERT((bsl::is_same<Zero, Zero::type>::value));
        }

        if (verbose) printf("\tTest SI unit prefixes.\n");
        {
            ASSERT(1 == bsl::deci::num);
            ASSERT(1 == bsl::centi::num);
            ASSERT(1 == bsl::milli::num);
            ASSERT(1 == bsl::micro::num);
            ASSERT(1 == bsl::nano::num);
            ASSERT(1 == bsl::pico::num);
            ASSERT(1 == bsl::femto::num);
            ASSERT(1 == bsl::atto::num);

            ASSERT(                 10ULL == bsl::deci::den);
            ASSERT(                100ULL == bsl::centi::den);
            ASSERT(               1000ULL == bsl::milli::den);
            ASSERT(            1000000ULL == bsl::micro::den);
            ASSERT(         1000000000ULL == bsl::nano::den);
            ASSERT(      1000000000000ULL == bsl::pico::den);
            ASSERT(   1000000000000000ULL == bsl::femto::den);
            ASSERT(1000000000000000000ULL == bsl::atto::den);

            ASSERT(1 == bsl::deca::den);
            ASSERT(1 == bsl::hecto::den);
            ASSERT(1 == bsl::kilo::den);
            ASSERT(1 == bsl::mega::den);
            ASSERT(1 == bsl::giga::den);
            ASSERT(1 == bsl::tera::den);
            ASSERT(1 == bsl::peta::den);
            ASSERT(1 == bsl::exa::den);

            ASSERT(                 10ULL == bsl::deca::num);
            ASSERT(                100ULL == bsl::hecto::num);
            ASSERT(               1000ULL == bsl::kilo::num);
            ASSERT(            1000000ULL == bsl::mega::num);
            ASSERT(         1000000000ULL == bsl::giga::num);
            ASSERT(      1000000000000ULL == bsl::tera::num);
            ASSERT(   1000000000000000ULL == bsl::peta::num);
            ASSERT(1000000000000000000ULL == bsl::exa::num);

        #ifdef BSL_RATIO_SUPPORTS_EXTENDED_SI_TYPEDEFS
            ASSERT(1 == bsl::zepto::num);
            ASSERT(1 == bsl::yocto::num);
            ASSERT(   1000000000000000000000ULL == bsl::zepto::den);
            ASSERT(1000000000000000000000000ULL == bsl::yocto::den);

            ASSERT(1 == std::zetta::den);
            ASSERT(1 == std::yotta::den);
            ASSERT(   1000000000000000000000ULL == bsl::zetta::num);
            ASSERT(1000000000000000000000000ULL == bsl::yotta::num);
        #endif
        }

        if (verbose) printf("\tTest 'ratio' comparisons.\n");
        {
            typedef bsl::ratio<3, 2>  ThreeHalfs;
            typedef bsl::ratio<6, 4>  Six4ths;
            typedef bsl::ratio<0, 1>  Zero;
            typedef bsl::ratio<-2, 3> NegTwo3rds;

            ASSERT( (bsl::ratio_less<NegTwo3rds, ThreeHalfs>::value));
            ASSERT( (bsl::ratio_less<NegTwo3rds, Zero>::value));
            ASSERT(!(bsl::ratio_less<ThreeHalfs, Zero>::value));
            ASSERT(!(bsl::ratio_less<ThreeHalfs, Six4ths>::value));

            ASSERT( (bsl::ratio_less_equal<NegTwo3rds, ThreeHalfs>::value));
            ASSERT( (bsl::ratio_less_equal<NegTwo3rds, Zero>::value));
            ASSERT(!(bsl::ratio_less_equal<ThreeHalfs, Zero>::value));
            ASSERT( (bsl::ratio_less_equal<ThreeHalfs, Six4ths>::value));

            ASSERT(!(bsl::ratio_greater<NegTwo3rds, ThreeHalfs>::value));
            ASSERT(!(bsl::ratio_greater<NegTwo3rds, Zero>::value));
            ASSERT( (bsl::ratio_greater<ThreeHalfs, Zero>::value));
            ASSERT(!(bsl::ratio_greater<ThreeHalfs, Six4ths>::value));

            ASSERT(!(bsl::ratio_greater_equal<NegTwo3rds, ThreeHalfs>::value));
            ASSERT(!(bsl::ratio_greater_equal<NegTwo3rds, Zero>::value));
            ASSERT( (bsl::ratio_greater_equal<ThreeHalfs, Zero>::value));
            ASSERT( (bsl::ratio_greater_equal<ThreeHalfs, Six4ths>::value));

            ASSERT(!(bsl::ratio_equal<NegTwo3rds, ThreeHalfs>::value));
            ASSERT(!(bsl::ratio_equal<NegTwo3rds, Zero>::value));
            ASSERT(!(bsl::ratio_equal<ThreeHalfs, Zero>::value));
            ASSERT( (bsl::ratio_equal<ThreeHalfs, Six4ths>::value));

            ASSERT( (bsl::ratio_not_equal<NegTwo3rds, ThreeHalfs>::value));
            ASSERT( (bsl::ratio_not_equal<NegTwo3rds, Zero>::value));
            ASSERT( (bsl::ratio_not_equal<ThreeHalfs, Zero>::value));
            ASSERT(!(bsl::ratio_not_equal<ThreeHalfs, Six4ths>::value));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
            // same tests, with using the '_v' suffix
            ASSERT( (bsl::ratio_less_v<NegTwo3rds, ThreeHalfs>));
            ASSERT( (bsl::ratio_less_v<NegTwo3rds, Zero>));
            ASSERT(!(bsl::ratio_less_v<ThreeHalfs, Zero>));
            ASSERT(!(bsl::ratio_less_v<ThreeHalfs, Six4ths>));

            ASSERT( (bsl::ratio_less_equal_v<NegTwo3rds, ThreeHalfs>));
            ASSERT( (bsl::ratio_less_equal_v<NegTwo3rds, Zero>));
            ASSERT(!(bsl::ratio_less_equal_v<ThreeHalfs, Zero>));
            ASSERT( (bsl::ratio_less_equal_v<ThreeHalfs, Six4ths>));

            ASSERT(!(bsl::ratio_greater_v<NegTwo3rds, ThreeHalfs>));
            ASSERT(!(bsl::ratio_greater_v<NegTwo3rds, Zero>));
            ASSERT( (bsl::ratio_greater_v<ThreeHalfs, Zero>));
            ASSERT(!(bsl::ratio_greater_v<ThreeHalfs, Six4ths>));

            ASSERT(!(bsl::ratio_greater_equal_v<NegTwo3rds, ThreeHalfs>));
            ASSERT(!(bsl::ratio_greater_equal_v<NegTwo3rds, Zero>));
            ASSERT( (bsl::ratio_greater_equal_v<ThreeHalfs, Zero>));
            ASSERT( (bsl::ratio_greater_equal_v<ThreeHalfs, Six4ths>));

            ASSERT(!(bsl::ratio_equal_v<NegTwo3rds, ThreeHalfs>));
            ASSERT(!(bsl::ratio_equal_v<NegTwo3rds, Zero>));
            ASSERT(!(bsl::ratio_equal_v<ThreeHalfs, Zero>));
            ASSERT( (bsl::ratio_equal_v<ThreeHalfs, Six4ths>));

            ASSERT( (bsl::ratio_not_equal_v<NegTwo3rds, ThreeHalfs>));
            ASSERT( (bsl::ratio_not_equal_v<NegTwo3rds, Zero>));
            ASSERT( (bsl::ratio_not_equal_v<ThreeHalfs, Zero>));
            ASSERT(!(bsl::ratio_not_equal_v<ThreeHalfs, Six4ths>));
#endif
        }

        if (verbose) printf("\tTest 'ratio' arithmetic operations.\n");
        {
            typedef bsl::ratio<3, 2>  ThreeHalfs;
            typedef bsl::ratio<3, 2>  Six4ths;
            typedef bsl::ratio<0, 1>  Zero;
            typedef bsl::ratio<-2, 3> NegTwo3rds;

            ASSERT((bsl::ratio_equal<
                            bsl::ratio_add<Zero, ThreeHalfs>::type,
                            ThreeHalfs>::value));
            ASSERT((bsl::ratio_equal<
                            bsl::ratio_add<ThreeHalfs, Six4ths>::type,
                            bsl::ratio<3,1>>::value));
            ASSERT((bsl::ratio_equal<
                            bsl::ratio_add<ThreeHalfs, NegTwo3rds>::type,
                            bsl::ratio<5,6>>::value));

            ASSERT((bsl::ratio_equal<
                            bsl::ratio_subtract<Zero, ThreeHalfs>::type,
                            bsl::ratio<-3,2>>::value));
            ASSERT((bsl::ratio_equal<
                            bsl::ratio_subtract<ThreeHalfs, Six4ths>::type,
                            Zero>::value));
            ASSERT((bsl::ratio_equal<
                            bsl::ratio_subtract<ThreeHalfs, NegTwo3rds>::type,
                            bsl::ratio<13,6>>::value));

            ASSERT((bsl::ratio_equal<
                            bsl::ratio_multiply<Zero, ThreeHalfs>::type,
                            Zero>::value));
            ASSERT((bsl::ratio_equal<
                            bsl::ratio_multiply<ThreeHalfs, Six4ths>::type,
                            bsl::ratio<9, 4>>::value));
            ASSERT((bsl::ratio_equal<
                            bsl::ratio_multiply<ThreeHalfs, NegTwo3rds>::type,
                            bsl::ratio<-1,1>>::value));

            ASSERT((bsl::ratio_equal<
                            bsl::ratio_divide<Zero, ThreeHalfs>::type,
                            Zero>::value));
            ASSERT((bsl::ratio_equal<
                            bsl::ratio_divide<ThreeHalfs, Six4ths>::type,
                            bsl::ratio<1, 1>>::value));
            ASSERT((bsl::ratio_equal<
                            bsl::ratio_divide<ThreeHalfs, NegTwo3rds>::type,
                            bsl::ratio<-9,4>>::value));
        }

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
// Copyright 2020 Bloomberg Finance L.P.
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
