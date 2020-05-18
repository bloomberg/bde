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
// The component under test a version of the ratio operations introduced
// into the standard library in C++11 (and 14). We test to make sure that the
// operations exist and are give sane resuts. The tests are not exhaustive.
//-----------------------------------------------------------------------------
// Groups of operations tested:
// * SI prefixes - kilo, mega, milli, etc
// * Ratio comparisons - less/greater/equal, etc
// * Ratio operations - add/subtract/multiply/divide, etc.
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
void testSI()
{
    ASSERT(bsl::atto::num  == 1 && bsl::atto::den  == 1000000000000000000ULL);
    ASSERT(bsl::femto::num == 1 && bsl::femto::den == 1000000000000000ULL);
    ASSERT(bsl::pico::num  == 1 && bsl::pico::den  == 1000000000000ULL);
    ASSERT(bsl::nano::num  == 1 && bsl::nano::den  == 1000000000ULL);
    ASSERT(bsl::micro::num == 1 && bsl::micro::den == 1000000ULL);
    ASSERT(bsl::milli::num == 1 && bsl::milli::den == 1000ULL);
    ASSERT(bsl::centi::num == 1 && bsl::centi::den == 100ULL);
    ASSERT(bsl::deci::num  == 1 && bsl::deci::den  == 10ULL);
    ASSERT(bsl::deca::num  == 10ULL && bsl::deca::den == 1);
    ASSERT(bsl::hecto::num == 100ULL && bsl::hecto::den == 1);
    ASSERT(bsl::kilo::num  == 1000ULL && bsl::kilo::den == 1);
    ASSERT(bsl::mega::num  == 1000000ULL && bsl::mega::den == 1);
    ASSERT(bsl::giga::num  == 1000000000ULL && bsl::giga::den == 1);
    ASSERT(bsl::tera::num  == 1000000000000ULL && bsl::tera::den == 1);
    ASSERT(bsl::peta::num  == 1000000000000000ULL && bsl::peta::den == 1);
    ASSERT(bsl::exa::num   == 1000000000000000000ULL && std::exa::den == 1);

#ifdef BSL_RATIO_SUPPORTS_EXTENDED_SI_TYPEDEFS
    ASSERT(bsl::yocto::num  == 1 && 
           bsl::yocto::den  == 1000000000000000000000000ULL);
    ASSERT(bsl::zepto::num  == 1 && 
           bsl::zepto::den  == 1000000000000000000000ULL);
    ASSERT(bsl::zetta::num  == 1000000000000000000000ULL && 
           std::zetta::den == 1);
    ASSERT(bsl::yotta::num  == 1000000000000000000000000ULL &&
           std::yotta::den == 1);
#endif
}

void testRatioComparisons()
{
	typedef bsl::ratio<3, 2> three_halfs;
	typedef bsl::ratio<3, 2> six_fourths;
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

//  same tests, with using the _v suffix
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

	
void testRatioOperations()
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

//	there are no '_t' shortcuts for these operations
}

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
    	testSI();
    	testRatioComparisons();
    	testRatioOperations();
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
