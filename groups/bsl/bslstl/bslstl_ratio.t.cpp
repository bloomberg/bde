// bslstl_ratio.t.cpp                                                -*-C++-*-
#include <bslstl_ratio.h>

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
// introduced into the standard library in C++11 (and 14). We test to make
// sure that the operations exist and give sane results. 
// The tests are not exhaustive.
//-----------------------------------------------------------------------------
// Groups of operations tested:
// * SI prefixes - kilo, mega, milli, etc
// * Ratio comparisons - less/greater/equal, etc
// ** This includes the '_v' inline variables
// * Ratio operations - add/subtract/multiply/divide, etc.
// ** There are no '_t' versions of these operations.
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

      } break;
      case 1: {
        // ----------------------------------------------------------------------
        // TESTING the ratio operations
        //
        // Concerns:
        //: 1 That the operations are successfully imported into the 'bsl'
        //:   namespace from the underlying STL implementation.
        //: 
        //: 2 That the short versions of the comparison operators, such
        //:   as 'ratio_less_v' exist and work even if the underlying STL
        //:   does not provide them.
        //
        // Plan:
        //: 1 Verify that the SI typedefs exist and have the correct 
        //:   numerator and denominator.
        //:
        //: 2 Verify that the comparison operations exist and give 
        //:   expected results. Check again for the '_v' versions.
        //:
        //: 3 Verify that the arithmetic operations exist and give
        //:   expected results.
        //
        // ----------------------------------------------------------------------

        if (verbose) printf("\nTesting ratio operations"
                            "\n======================================\n");

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

            ASSERT(10ULL == bsl::deci::den);
            ASSERT(100ULL == bsl::centi::den);
            ASSERT(1000ULL == bsl::milli::den);
            ASSERT(1000000ULL == bsl::micro::den);
            ASSERT(1000000000ULL == bsl::nano::den);
            ASSERT(1000000000000ULL == bsl::pico::den);
            ASSERT(1000000000000000ULL == bsl::femto::den);
            ASSERT(1000000000000000000ULL == bsl::atto::den);

            ASSERT(1 == bsl::deca::den);
            ASSERT(1 == bsl::hecto::den);
            ASSERT(1 == bsl::kilo::den);
            ASSERT(1 == bsl::mega::den);
            ASSERT(1 == bsl::giga::den);
            ASSERT(1 == bsl::tera::den);
            ASSERT(1 == bsl::peta::den);
            ASSERT(1 == bsl::exa::den);

            ASSERT(10ULL == bsl::deca::num);
            ASSERT(100ULL == bsl::hecto::num);
            ASSERT(1000ULL == bsl::kilo::num);
            ASSERT(1000000ULL == bsl::mega::num);
            ASSERT(1000000000ULL == bsl::giga::num);
            ASSERT(1000000000000ULL == bsl::tera::num);
            ASSERT(1000000000000000ULL == bsl::peta::num);
            ASSERT(1000000000000000000ULL == bsl::exa::num);

        #ifdef BSL_RATIO_SUPPORTS_EXTENDED_SI_TYPEDEFS
            ASSERT(1 == bsl::zepto::num);
            ASSERT(1 == bsl::yocto::num);
            ASSERT(1000000000000000000000ULL == bsl::zepto::den);
            ASSERT(1000000000000000000000000ULL == bsl::yocto::den);

            ASSERT(1 == std::zetta::den);
            ASSERT(1 == std::yotta::den);
            ASSERT(1000000000000000000000ULL == bsl::zetta::num);
            ASSERT(1000000000000000000000000ULL == bsl::yotta::num);
        #endif
        }

        if (verbose) printf("\tTest ratio comparisons.\n");
        {
            typedef bsl::ratio<3, 2> three_halfs;
            typedef bsl::ratio<6, 4> six_fourths;
            typedef bsl::ratio<0, 1> zero;
            typedef bsl::ratio<-2, 3> neg_two_thirds;

            ASSERT( (bsl::ratio_less<neg_two_thirds, three_halfs>::value));
            ASSERT( (bsl::ratio_less<neg_two_thirds, zero>::value));
            ASSERT(!(bsl::ratio_less<three_halfs,zero>::value));
            ASSERT(!(bsl::ratio_less<three_halfs, six_fourths>::value));

            ASSERT( (bsl::ratio_less_equal<neg_two_thirds, three_halfs>::value));
            ASSERT( (bsl::ratio_less_equal<neg_two_thirds, zero>::value));
            ASSERT(!(bsl::ratio_less_equal<three_halfs,zero>::value));
            ASSERT( (bsl::ratio_less_equal<three_halfs, six_fourths>::value));

            ASSERT(!(bsl::ratio_greater<neg_two_thirds, three_halfs>::value));
            ASSERT(!(bsl::ratio_greater<neg_two_thirds, zero>::value));
            ASSERT( (bsl::ratio_greater<three_halfs,zero>::value));
            ASSERT(!(bsl::ratio_greater<three_halfs, six_fourths>::value));

            ASSERT(!(bsl::ratio_greater_equal<neg_two_thirds, three_halfs>::value));
            ASSERT(!(bsl::ratio_greater_equal<neg_two_thirds, zero>::value));
            ASSERT( (bsl::ratio_greater_equal<three_halfs,zero>::value));
            ASSERT( (bsl::ratio_greater_equal<three_halfs, six_fourths>::value));

            ASSERT(!(bsl::ratio_equal<neg_two_thirds, three_halfs>::value));
            ASSERT(!(bsl::ratio_equal<neg_two_thirds, zero>::value));
            ASSERT(!(bsl::ratio_equal<three_halfs,zero>::value));
            ASSERT( (bsl::ratio_equal<three_halfs, six_fourths>::value));

            ASSERT( (bsl::ratio_not_equal<neg_two_thirds, three_halfs>::value));
            ASSERT( (bsl::ratio_not_equal<neg_two_thirds, zero>::value));
            ASSERT( (bsl::ratio_not_equal<three_halfs,zero>::value));
            ASSERT(!(bsl::ratio_not_equal<three_halfs, six_fourths>::value));

            // same tests, with using the _v suffix
            ASSERT( (bsl::ratio_less_v<neg_two_thirds, three_halfs>));
            ASSERT( (bsl::ratio_less_v<neg_two_thirds, zero>));
            ASSERT(!(bsl::ratio_less_v<three_halfs,zero>));
            ASSERT(!(bsl::ratio_less_v<three_halfs, six_fourths>));

            ASSERT( (bsl::ratio_less_equal_v<neg_two_thirds, three_halfs>));
            ASSERT( (bsl::ratio_less_equal_v<neg_two_thirds, zero>));
            ASSERT(!(bsl::ratio_less_equal_v<three_halfs,zero>));
            ASSERT( (bsl::ratio_less_equal_v<three_halfs, six_fourths>));

            ASSERT(!(bsl::ratio_greater_v<neg_two_thirds, three_halfs>));
            ASSERT(!(bsl::ratio_greater_v<neg_two_thirds, zero>));
            ASSERT( (bsl::ratio_greater_v<three_halfs,zero>));
            ASSERT(!(bsl::ratio_greater_v<three_halfs, six_fourths>));

            ASSERT(!(bsl::ratio_greater_equal_v<neg_two_thirds, three_halfs>));
            ASSERT(!(bsl::ratio_greater_equal_v<neg_two_thirds, zero>));
            ASSERT( (bsl::ratio_greater_equal_v<three_halfs,zero>));
            ASSERT( (bsl::ratio_greater_equal_v<three_halfs, six_fourths>));

            ASSERT(!(bsl::ratio_equal_v<neg_two_thirds, three_halfs>));
            ASSERT(!(bsl::ratio_equal_v<neg_two_thirds, zero>));
            ASSERT(!(bsl::ratio_equal_v<three_halfs,zero>));
            ASSERT( (bsl::ratio_equal_v<three_halfs, six_fourths>));

            ASSERT( (bsl::ratio_not_equal_v<neg_two_thirds, three_halfs>));
            ASSERT( (bsl::ratio_not_equal_v<neg_two_thirds, zero>));
            ASSERT( (bsl::ratio_not_equal_v<three_halfs,zero>));
            ASSERT(!(bsl::ratio_not_equal_v<three_halfs, six_fourths>));
        }

        if (verbose) printf("\tTest ratio arithmetic operations.\n");
        {
            typedef bsl::ratio<3, 2> three_halfs;
            typedef bsl::ratio<3, 2> six_fourths;
            typedef bsl::ratio<0, 1> zero;
            typedef bsl::ratio<-2, 3> neg_two_thirds;

            ASSERT((bsl::ratio_equal_v<bsl::ratio_add<zero, three_halfs>::type, three_halfs>));
            ASSERT((bsl::ratio_equal_v<bsl::ratio_add<three_halfs, six_fourths>::type, bsl::ratio<3,1>>));
            ASSERT((bsl::ratio_equal_v<bsl::ratio_add<three_halfs, neg_two_thirds>::type, bsl::ratio<5,6>>));

            ASSERT((bsl::ratio_equal_v<bsl::ratio_subtract<zero, three_halfs>::type, bsl::ratio<-3,2>>));
            ASSERT((bsl::ratio_equal_v<bsl::ratio_subtract<three_halfs, six_fourths>::type, zero>));
            ASSERT((bsl::ratio_equal_v<bsl::ratio_subtract<three_halfs, neg_two_thirds>::type, bsl::ratio<13,6>>));

            ASSERT((bsl::ratio_equal_v<bsl::ratio_multiply<zero, three_halfs>::type, zero>));
            ASSERT((bsl::ratio_equal_v<bsl::ratio_multiply<three_halfs, six_fourths>::type, bsl::ratio<9, 4>>));
            ASSERT((bsl::ratio_equal_v<bsl::ratio_multiply<three_halfs, neg_two_thirds>::type, bsl::ratio<-1,1>>));

            ASSERT((bsl::ratio_equal_v<bsl::ratio_divide<zero, three_halfs>::type, zero>));
            ASSERT((bsl::ratio_equal_v<bsl::ratio_divide<three_halfs, six_fourths>::type, bsl::ratio<1, 1>>));
            ASSERT((bsl::ratio_equal_v<bsl::ratio_divide<three_halfs, neg_two_thirds>::type, bsl::ratio<-9,4>>));
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
