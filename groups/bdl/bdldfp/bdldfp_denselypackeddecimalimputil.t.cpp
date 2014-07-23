// bdldfp_denselypackeddecimalimputil.t.cpp                           -*-C++-*-
#include <bdldfp_denselypackeddecimalimputil.h>

#include <bdldfp_uint128.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_algorithm.h>
#include <bsl_cfloat.h>
#include <bsl_climits.h>
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>
#include <bsl_limits.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <typeinfo>


#include <cassert>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::hex;
using bsl::atoi;
using bsl::stringstream;

#if 1 || BDLDFP_DECIMALPLATFORM_DPD

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// 'bdldfp_denselypackeddecimalimputil' implements routines used in creation,
// composition, and decomposition of /Densely/ /Packed/ /Decimal/ (DPD) format
// numbers.
//
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] encodeDeclet(unsigned digits)
// [ 3] decodeDeclet(unsigned declet)
// [ 4] makeDecimalRaw32 (                   int mantissa, int exponent)
// [ 4] makeDecimalRaw64 (         long long int mantissa, int exponent)
// [ 4] makeDecimalRaw64 (unsigned long long int mantissa, int exponent)
// [ 4] makeDecimalRaw64 (                   int mantissa, int exponent)
// [ 4] makeDecimalRaw64 (unsigned           int mantissa, int exponent)
// [ 4] makeDecimalRaw128(         long long int mantissa, int exponent)
// [ 4] makeDecimalRaw128(unsigned long long int mantissa, int exponent)
// [ 4] makeDecimalRaw128(                   int mantissa, int exponent)
// [ 4] makeDecimalRaw128(unsigned           int mantissa, int exponent)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
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
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace BDEC = BloombergLP::bdldfp;

typedef BDEC::DenselyPackedDecimalImpUtil Util;

inline unsigned encodeDeclet(unsigned value)
{
    const unsigned upperDigit  = value / 100;
    const unsigned remainder   = value % 100;

    const unsigned middleDigit = remainder / 10;
    const unsigned lowerDigit  = remainder % 10;

    switch(((upperDigit  >= 8) << 2)
        |  ((middleDigit >= 8) << 1)
        |   (lowerDigit  >= 8) << 0) {
        case 00:
        case 01:
            return (upperDigit  << 7)
                 | (middleDigit << 4)
                 | (lowerDigit  << 0);

        case 02:
            return 0x0A
                 | ((upperDigit  & 07) << 7)
                 | ((middleDigit & 01) << 4)
                 | ((lowerDigit  & 06) << 4)
                 | ((lowerDigit  & 01) << 0);

        case 04:
            return 0x0C
                 | ((upperDigit  & 01) << 7)
                 | ((middleDigit & 07) << 4)
                 | ((lowerDigit  & 06) << 7)
                 | ((lowerDigit  & 01) << 0);

        case 3:
            return 0x4E
                 | ((upperDigit  & 07) << 7)
                 | ((middleDigit & 01) << 4)
                 | ((lowerDigit  & 01) << 0);

        case 5:
            return 0x2E
                 | ((upperDigit  & 01) << 7)
                 | ((middleDigit & 06) << 7)
                 | ((middleDigit & 01) << 4)
                 | ((lowerDigit  & 01) << 0);

        case 6:
            return 0x0E
                 | ((upperDigit  & 01) << 7)
                 | ((middleDigit & 01) << 4)
                 | ((lowerDigit  & 06) << 7)
                 | ((lowerDigit  & 01) << 0);

        case 07:
            return 0x6e
                 | ((upperDigit  & 01) << 7)
                 | ((middleDigit & 01) << 4)
                 | ((lowerDigit  & 01) << 0);
    }

    return 0;

}

inline unsigned decodeDeclet(unsigned declet)
{
    if (!(declet & 0x8)) {

        // When the "magic" bit is clear, we have three small, 3-bit encoded
        // digits.

        return     1 * ( ( declet >> 0 ) & 07 )
               +  10 * ( ( declet >> 4 ) & 07 )
               + 100 * ( ( declet >> 7 ) & 07 );
    }


    // If at least one of bit one and bit two are not set, there is only one
    // small digit.  Therefore:

    switch ((declet >> 1) & 03) {
        case 0:

            // The third digit is "big"

            return         ((declet       & 01) | 010)
                   +  10 * ((declet >> 4) & 07)
                   + 100 * ((declet >> 7) & 07);

        case 1:

            // The second digit is "big"

            return     1 * (((declet >> 4) & 06) | (declet & 1))
                   +  10 * (((declet >> 4) & 01) | 010)
                   + 100 * (((declet >> 7) & 07) | 000);

        case 2:

            // The first digit is "big"

            return     1 * (((declet >> 7) & 06) | (declet & 1))
                   +  10 * (((declet >> 4) & 07) | 000)
                   + 100 * (((declet >> 7) & 01) | 010);

        default:

            // If both bits are set, then there are two large digits, so we
            // handle that with a different switch
            // block.

            break;
    }

    switch ((declet >> 5) & 03) {
        case 0:

            // The third digit is "small"

            return     1 * (((declet >> 7) & 06) | ( declet & 1 ) )
                   +  10 * (((declet >> 4) & 01) | 010 )
                   + 100 * (((declet >> 7) & 01) | 010 );

        case 1:

            // The second digit is "small"

            return     1 * (((declet >> 0) & 01) | 010)
                   +  10 * (((declet >> 7) & 06) | ((declet >> 4) & 01))
                   + 100 * (((declet >> 7) & 01) | 010);

        case 2:  // The first digit is "small"

            return     1 * (((declet >> 0) & 01) | 010)
                   +  10 * (((declet >> 4) & 01) | 010)
                   + 100 * (((declet >> 7) & 07) | 000);

        default:

            // If both bits in question are set, then there are three large
            // digits, so we can merely compose them

            return     1 * (((declet >> 0) & 01) | 010)
                   +  10 * (((declet >> 4) & 01) | 010)
                   + 100 * (((declet >> 7) & 01) | 010);
    }
}

int
main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'makeDecimalRawXX' functions
        //
        // Concerns:
        //: 1 'makeDecimalRawXX' constructs a decimal floating point value
        //:   having the specified mantissa and exponent, with no rounding.
        //:
        //: 2 The correctness of the conversion of each 3-digit group (or
        //:   declet) into a 10-bit binary-encoded declet.
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
        //: 2 Test each overload of each 'makeDecimalRawXX' by representing
        //:   the mantissa in a string format and parsing in each intergral
        //:   type.
        //:
        //: 3 Test a mantissa of each length in base 10.
        //:
        //: 4 Test that 'makeDecimalRawXX' returns the same value as the
        //:   decimal created by invoking the decNumber constructors using
        //:   binary-coded decimals.
        //
        // Testing:
        //   makeDecimalRaw32 (                   int mantissa, int exponent)
        //   makeDecimalRaw64 (                   int mantissa, int exponent)
        //   makeDecimalRaw64 (unsigned           int mantissa, int exponent)
        //   makeDecimalRaw64 (         long long int mantissa, int exponent)
        //   makeDecimalRaw64 (unsigned long long int mantissa, int exponent)
        //   makeDecimalRaw128(                   int mantissa, int exponent)
        //   makeDecimalRaw128(unsigned           int mantissa, int exponent)
        //   makeDecimalRaw128(         long long int mantissa, int exponent)
        //   makeDecimalRaw128(unsigned long long int mantissa, int exponent)
        // --------------------------------------------------------------------

        // Test that 'makeDecimalRaw32' enforces undefined behavior in the
        // right build mode
        {
            using bsls::AssertFailureHandlerGuard;
            AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw32(42, -102));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw32(42, -101));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(Util::makeDecimalRaw32(42, 90));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(Util::makeDecimalRaw32(42, 91));

            // Check mantissa lower bound
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(
                                        Util::makeDecimalRaw32(-10000000, 42));
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(
                                        Util::makeDecimalRaw32(- 9999999, 42));

            // Check mantissa upper bound
            BSLS_ASSERTTEST_ASSERT_OPT_PASS(
                                        Util::makeDecimalRaw32(  9999999, 42));
            BSLS_ASSERTTEST_ASSERT_OPT_FAIL(
                                        Util::makeDecimalRaw32( 10000000, 42));
        }
        
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'decodeDeclet(unsigned declet)'
        //   Ensure that the method correctly decodes DPD declets into values.
        //
        // Concerns:
        //: 1 The method correctly converts a value in the expected range which
        //:   encodes a correct and valid declet into its respective value.
        //:
        //: 2 QoI: The method has correct soft-undefined behavior on expected
        //:   boundaries, and non-canonical declet forms.
        //
        // Plan:
        //: 1 Iterate through the entire valid declet encoding space
        //:   '[0, 1000)' and test that each value, once encoded as a declet
        //:   matches the expected value when decoding.  The alternate decoder
        //:   in this test driver shall be used as an oracle for the expected
        //:   value the decoded results.  The results of the utility decoder
        //:   function shall also be tested against the original expected
        //:   value, before encoding.
        //:
        //: 2 Validate that each value returned is in the range '[0, 1000)', as
        //:   expected for the results of a declet.
        //:
        //: 3 Test the boundary case of 1024, for BSLS_ASSERT behavior.
        //:
        //: 4 Test all non-canonical declet forms for BSLS_ASSERT behavior.
        //:   Note that non-canonical forms are those forms with three large
        //:   digits, where any of the upper two bits of the declet are set.
        //
        // Testing:
        //   static unsigned decodeDeclet(unsigned value)
        // --------------------------------------------------------------------



        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);


            if (veryVerbose) cout << "\t'Util::decodeDeclet'" << endl;
            {
                // decodeDeclet is not supposed to
                ASSERT_FAIL(Util::decodeDeclet(0x400));

                // Known highest valid declet with single representation

                ASSERT_PASS(Util::decodeDeclet(0x3FD));

                // All aliased multi-value declets should pass for high-zeros
                // but have undefined behavior with aliased forms.  The soft
                // undefined behavior is tested on the aliased forms here.
                // The loop counter is unsigned, to facilitate easy bitwise
                // operations on its value.

                for (unsigned i = 0; i < 7; ++i) {
                    unsigned value = 0x6E | ((i & 01) << 0)
                                          | ((i & 02) << 3)
                                          | ((i & 04) << 5);
                    ASSERT_PASS(Util::decodeDeclet(0x000 | value));
                    ASSERT_FAIL(Util::decodeDeclet(0x100 | value));
                    ASSERT_FAIL(Util::decodeDeclet(0x200 | value));
                    ASSERT_FAIL(Util::decodeDeclet(0x300 | value));
                }
            }
        }
        for (int i = 0; i < 1000; ++i) {
            unsigned declet   = Util::encodeDeclet(i);

            unsigned result   = Util::decodeDeclet(declet);
            unsigned expected =       decodeDeclet(declet);
            LOOP4_ASSERT(i, declet, result, expected, result == expected);
            LOOP4_ASSERT(i, declet, result, expected, result == i);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'encodeDeclet(unsigned digits)'
        //   Ensure that the method correctly encodes DPD declets
        //
        // Concerns:
        //: 1 The method correctly converts a value in the expected range into
        //:   correct and valid declet encoding.
        //:
        //: 2 QoI: The method has correct soft-undefined behavior on expected
        //:   boundaries.
        //
        // Plan:
        //: 1 Iterate through the entire valid declet encoding space
        //:   '[0,   1000)' and test that each encoded declet matches the
        //:   expected value from an alternate encoder.  (The alternate encoder
        //:   shoud handles each of the 8 cases for declet encoding,
        //:   individually).
        //:
        //: 2 Validate that each value returned is in the range '[0, 1024)', as
        //:   promised by the declet definition.
        //:
        //: 3 Test the boundary case of 1000, for BSLS_ASSERT behavior.
        //
        // Testing:
        //   static unsigned uncodeDeclet(unsigned value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);


            if (veryVerbose) cout << "\t'encodeDeclet'" << endl;
            {
                ASSERT_FAIL(Util::encodeDeclet(1000));
                ASSERT_PASS(Util::encodeDeclet(999));
            }
        }

        for (int i = 0; i < 1000; ++i) {
            unsigned result   = Util::encodeDeclet(i);
            unsigned expected =       encodeDeclet(i);
            LOOP3_ASSERT(i, result, expected, result == expected);
            LOOP2_ASSERT(i, result, result < 1024);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 TBD
        //
        // Plan:
        //: 1 TBD
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

#else

int
main()
{
    return -1;
}

#endif

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
