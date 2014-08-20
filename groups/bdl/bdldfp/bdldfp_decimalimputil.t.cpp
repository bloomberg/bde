// bdldfp_decimalimputil.t.cpp                                        -*-C++-*-
#include <bdldfp_decimalimputil.h>

#include <bdldfp_bufferbuf.h>

#include <bdls_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_stopwatch.h>

#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>
#include <bsl_fstream.h>
#include <bsl_limits.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

#include <typeinfo>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::atoi;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// TBD:
// ----------------------------------------------------------------------------
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//
// FREE OPERATORS
//
// TRAITS
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TEST 'notEqual' FOR 'NaN' CORRECTNESS
// [ 4] USAGE EXAMPLE
// ----------------------------------------------------------------------------


// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace BDEC = BloombergLP::bdldfp;


// ============================================================================
//                      USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Example 2: Adding two Decimal Floating Point Values
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we we define the signature of a function that computes the sum of an
// array of security prices, and returns that sum as a decimal floating point
// value:

    bdldfp::DecimalImpUtil::ValueType64
    totalSecurities(bdldfp::DecimalImpUtil::ValueType64 *prices,
                    int                                  numPrices)
        // Return a Decimal Floating Point number representing the arithmetic
        // total of the values specified by 'prices' and 'numPrices'.
    {

// Then, we create a local variable to hold the intermediate sum, and set it to
// 0:

        bdldfp::DecimalImpUtil::ValueType64 total;
        total = bdldfp::DecimalImpUtil::int32ToDecimal64(0);

// Next, we loop over the array of 'prices' and add each price to the
// intermediate 'total':

        for (int i = 0; i < numPrices; ++i) {
            total = bdldfp::DecimalImpUtil::add(total, prices[i]);
        }

// Now, we return the computed total value of the securities:

        return total;
    }



//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    int                test = argc > 1 ? atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;  // always the last

    using bsls::AssertFailureHandlerGuard;

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator  ta(veryVeryVeryVerbose);
    bslma::TestAllocator *pa = &ta;

    typedef BDEC::DecimalImpUtil Util;

    cout.precision(35);


    switch (test) { case 0:
      case 4: {
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: Constructing a Representation of a Value in Decimal
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A common requirement for decimal floating point types is to be able to
// create a value from independent "coefficient" and "exponent" values, where
// the resulting decimal has the value 'coefficient * 10 ^ exponent'.  In the
// following example we use such a 'coffecicient' and 'exponent' to create
// 'Decimal32', 'Decimal64', and 'Decimal128' values.
//
// First we define values representing the 'coefficient' and 'exponent' (note
// the result should be the value 42.5):

    long long coefficient = 425; // Yet another name for significand
    int exponent          =  -1;

// Then we call 'makeDecimal32', 'makeDecimal64', and 'makeDecimal128' to
// construct a 'Decimal32', 'Decimal64', and 'Decimal128' respectively.

    bdldfp::DecimalImpUtil::ValueType32  d32 =
              bdldfp::DecimalImpUtil::makeDecimalRaw32( coefficient, exponent);
    bdldfp::DecimalImpUtil::ValueType64  d64 =
              bdldfp::DecimalImpUtil::makeDecimalRaw64( coefficient, exponent);
    bdldfp::DecimalImpUtil::ValueType128 d128 =
              bdldfp::DecimalImpUtil::makeDecimalRaw128(coefficient, exponent);
//
    ASSERT(bdldfp::DecimalImpUtil::equal(
                      bdldfp::DecimalImpUtil::binaryToDecimal32( 42.5), d32));
    ASSERT(bdldfp::DecimalImpUtil::equal(
                      bdldfp::DecimalImpUtil::binaryToDecimal64( 42.5), d64));
    ASSERT(bdldfp::DecimalImpUtil::equal(
                      bdldfp::DecimalImpUtil::binaryToDecimal128(42.5), d128));

//
///Example 2: Adding two Decimal Floating Point Values
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Decimal floating point values are frequently used in arithmetic computations
// where the precise representation of decimal values is of paramount
// importance (for example, financial calculations, as currency is typically
// denominated in base-10 decimal values).  In the following example we
// demonstrate computing the sum of a sequence of security prices, where each
// price is held in a 'DecimalImpUtil::ValueType64' value.
//

// Notice that 'add' is called as a function, and is not an operator overload
// for '+'; this is because the 'bdldfp::DecimalImpUtil' utility is intended to
// be used in the implementation of operator overloads on a more full fledged
// type.
//
// Finally, we call the function with some sample data, and check the result:

    bdldfp::DecimalImpUtil::ValueType64 data[16];
//
    for (int i = 0; i < 16; ++i) {
        data[i] = bdldfp::DecimalImpUtil::int32ToDecimal64(i + 1);
    }
//
    bdldfp::DecimalImpUtil::ValueType64 result;
    result = totalSecurities(data, 16);
//
    bdldfp::DecimalImpUtil::ValueType64 expected;
//
    expected = bdldfp::DecimalImpUtil::int32ToDecimal64(16);
//
    // Totals of values from 1 to 'x' are '(x * x + x) / 2':
//
    expected = bdldfp::DecimalImpUtil::add(
                 bdldfp::DecimalImpUtil::multiply(expected, expected),
                 expected);
    expected = bdldfp::DecimalImpUtil::divide(
                         expected,
                         bdldfp::DecimalImpUtil::int32ToDecimal64(2));
//
    ASSERT(bdldfp::DecimalImpUtil::equal(expected, result));

// Notice that arithmetic is unwieldy and hard to visualize.  This is by
// design, as the DecimalImpUtil and subordinate components are not intended
// for public consumption, or direct use in decimal arithmetic.
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING notEqual
        //
        // Concerns:
        //:  1 notEqual should return false when there are two NaN arguments.
        //
        // Plan:
        //:  1 Create 'NaN' values, and compare them as inequal.
        // --------------------------------------------------------------------
        {
            Util::ValueType32  nan32  = Util::parse32( "NaN");
            Util::ValueType64  nan64  = Util::parse64( "NaN");
            Util::ValueType128 nan128 = Util::parse128("NaN");

            ASSERT(Util::notEqual(nan32,  nan32));
            ASSERT(Util::notEqual(nan64,  nan64));
            ASSERT(Util::notEqual(nan128, nan128));
        }

        {
            Util::ValueType32  nan32  = Util::parse32( "qNaN");
            Util::ValueType64  nan64  = Util::parse64( "qNaN");
            Util::ValueType128 nan128 = Util::parse128("qNaN");

            ASSERT(Util::notEqual(nan32,  nan32));
            ASSERT(Util::notEqual(nan64,  nan64));
            ASSERT(Util::notEqual(nan128, nan128));
        }

        {
            Util::ValueType32  nan32  = Util::parse32( "sNaN");
            Util::ValueType64  nan64  = Util::parse64( "sNaN");
            Util::ValueType128 nan128 = Util::parse128("sNaN");

            ASSERT(Util::notEqual(nan32,  nan32));
            ASSERT(Util::notEqual(nan64,  nan64));
            ASSERT(Util::notEqual(nan128, nan128));
        }
      } break;
      case 2: {} break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING makeDecimalRaw
        //
        // Concerns:
        //: 1 'makeDecimalRawXX' constructs a decimal floating point value
        //:   having the specified mantissa and exponent, with no rounding.
        //:
        //: 2 The correctness of the conversion of each 3-digit group (or
        //:   declet) into a 10-bit binary-encoded declet.  Note that this is
        //:   superfluous on BID architectures, but causes no significant
        //:   slowdown.
        //:
        //: 3 The conversion of each 3-digit declet into a 10-bit
        //:   binary-encoded declet does not intefere each other.
        //:
        //: 4 The combination field is correctly computed given the leading
        //:   digit of the mantissa and the exponent.
        //:
        //: 5 'makeDecimalRawXX' asserts in the right build modes for undefined
        //:   behavior.
        //
        // Plan:
        //: 1 Test 'makeDecimalRawXX' contract using
        //:   'AssertFailureHandlerGuard' and 'BSLS_ASSERTTEST_ASSERT_OPT_FAIL'
        //:
        //: 2 Test each overload of each 'makeDecimalRawXX' by representing the
        //:   mantissa in a string format and parsing in each intergral type.
        //:
        //: 3 Test a mantissa of each length in base 10.
        //:
        //: 4 Test that 'makeDecimalRawXX' returns the same value as the
        //:   decimal created by invoking the decNumber constructors using
        //:   binary-coded decimals.
        //
        // Testing:
        //   makeDecimalRaw32 (int mantissa,                int exponent);
        //   makeDecimalRaw64 (long long mantissa,          int exponent);
        //   makeDecimalRaw64 (unsigned long long mantissa, int exponent);
        //   makeDecimalRaw64 (int mantissa,                int exponent);
        //   makeDecimalRaw64 (unsigned int mantissa,       int exponent);
        //   makeDecimalRaw128(long long mantissa,          int exponent);
        //   makeDecimalRaw128(unsigned long long mantissa, int exponent);
        //   makeDecimalRaw128(int mantissa,                int exponent);
        //   makeDecimalRaw128(unsigned int mantissa,       int exponent);
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << bsl::endl
                                << "PRIMARY MANIPULATORS"
                                << "====================" << bsl::endl;

        // Test that 'makeDecimalRaw32' enforces undefined behavior in the
        // right build mode
        {
            AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw32(42, -102));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw32(42, -101));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw32(42, 90));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw32(42, 91));

            // Check mantissa lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw32(-10000000,
                                                                          42));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw32(- 9999999,
                                                                          42));

            // Check mantissa upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw32( 9999999,
                                                                          42));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw32(10000000,
                                                                          42));
        }

        // Test that 'makeDecimalRaw64' variants enforce undefined behavior in
        // the right build mode
        {
            AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

            // Test 'makeDecimalRaw64(int, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42, -399));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42, -398));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42, 369));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42, 370));

            // 'makeDecimalRaw64(int, int)' needs no test for mantissa bounds
            // because 9,999,999,999,999,999 is unrepresentable as an int.

            // Test 'makeDecimalRaw64(unsigned int, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42u, -399));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42u, -398));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42u, 369));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42u, 370));

            // 'makeDecimalRaw64(unsigned int, int)' needs no test for mantissa
            // bounds because 9,999,999,999,999,999 is unrepresentable as an
            // int.

            // Test 'makeDecimalRaw64(long long, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42ll,
                                                                        -399));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42ll,
                                                                        -398));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42ll, 369));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42ll, 370));

            // Check mantissa lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(
                                                    -10000000000000000ll, 42));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(
                                                    - 9999999999999999ll, 42));

            // Check mantissa upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(
                                                      9999999999999999ll, 42));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(
                                                     10000000000000000ll, 42));

            // Test 'makeDecimalRaw64(unsigned long long, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42ull,
                                                                        -399));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42ull,
                                                                        -398));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(42ull,
                                                                         369));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(42ull,
                                                                         370));

            // Unsigned numbers cannot be negative, so we do not have a lower
            // bound test.

            // Check mantissa upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw64(
                                                     9999999999999999ull, 42));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw64(
                                                    10000000000000000ull, 42));
        }

        // Test that 'makeDecimalRaw128' variants enforce undefined behavior in
        // the right build mode
        {
            AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

            // Test 'makeDecimalRaw128(int, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42,
                                                                       -6177));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42,
                                                                       -6176));
            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42, 6111));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42, 6112));

            // Test 'makeDecimalRaw128(unsigned, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42u,
                                                                       -6177));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42u,
                                                                       -6176));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42u,
                                                                        6111));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42u,
                                                                        6112));

            // Test 'makeDecimalRaw128(long long, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42ll,
                                                                       -6177));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42ll,
                                                                       -6176));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42ll,
                                                                        6111));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42ll,
                                                                        6112));

            // Test 'makeDecimalRaw128(unsigned long long, int)':

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42ull,
                                                                       -6177));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42ull,
                                                                       -6176));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw128(42ull,
                                                                        6111));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw128(42ull,
                                                                        6112));
        }
      } break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: No memory came from the global or default allocator.

    LOOP2_ASSERT(test, globalAllocator.numBlocksTotal(),
                 0 == globalAllocator.numBlocksTotal());
    LOOP2_ASSERT(test, defaultAllocator.numBlocksTotal(),
                 0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
