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
// [  ] USAGE EXAMPLE
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
#if 0

        if (veryVerbose) bsl::cout << "Test the correctness of the declets "
                                   "table in this implementation by parsing "
                                   "each number between 0 and 999, "
                                   "inclusive." << bsl::endl;
        {
            for (int NUM = 0; NUM < 1000; ++NUM) {
                const Util::ValueType32 ACTUAL =
                    Util::makeDecimalRaw32(NUM, 0);
                const unsigned char iBCD[7] = {
                    0, 0, 0, 0, NUM / 100, (NUM / 10) % 10, NUM % 10};
                const D32 EXPECTED = D32(0, iBCD, 1);
                LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
            }

            for (int NUM = 0; NUM < 1000; ++NUM) {
                const Util::ValueType64 ACTUAL =
                    Util::makeDecimalRaw64(NUM, 0);
                const unsigned char iBCD[7] = {
                    0, 0, 0, 0, NUM / 100, (NUM / 10) % 10, NUM % 10};
                const D64 EXPECTED = D64(0, iBCD, 1);
                LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
            }

            for (int NUM = 0; NUM < 1000; ++NUM) {
                const Util::ValueType128 ACTUAL =
                    Util::makeDecimalRaw128(NUM, 0);
                const unsigned char iBCD[7] = {
                    0, 0, 0, 0, NUM / 100, (NUM / 10) % 10, NUM % 10};
                const D128 EXPECTED = D128(0, iBCD, 1);
                LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
            }
        }
        if (veryVerbose) bsl::cout << "Test that each declet can be correctly "
                                   "placed into each 10-bit shift pattern."
                                << bsl::endl;
        {
            // Test that declets can be properly shifted to each position.
            for (int thousands_pow = 1; thousands_pow <= 1; ++thousands_pow) {
                for (int raw_num = 1; raw_num < 1000; ++raw_num) {
                    // NUM = raw_num * 1000^thousands_pow.
                    unsigned long long NUM = raw_num;
                    for (int i = 0; i < thousands_pow; ++i) {
                        NUM *= 1000;
                    }

                    const Util::ValueType32 ACTUAL =
                        Util::makeDecimalRaw32(NUM, 0);
                    unsigned char iBCD[7] = {0, 0, 0, 0, 0, 0, 0};

                    iBCD[6 - 3 * thousands_pow] = raw_num % 10;
                    iBCD[5 - 3 * thousands_pow] = (raw_num / 10) % 10;
                    iBCD[4 - 3 * thousands_pow] = (raw_num / 100);

                    const D32 EXPECTED = D32(0, iBCD, 1);
                    LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
                }
            }

            for (int thousands_pow = 1; thousands_pow <= 4; ++thousands_pow) {
                for (int raw_num = 1; raw_num < 1000; ++raw_num) {
                    // NUM = raw_num * 1000^thousands_pow.
                    unsigned long long NUM = raw_num;
                    for (int i = 0; i < thousands_pow; ++i) {
                        NUM *= 1000;
                    }

                    const Util::ValueType64 ACTUAL =
                        Util::makeDecimalRaw64(NUM, 0);
                    unsigned char iBCD[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                                0, 0, 0, 0, 0};

                    iBCD[15 - 3 * thousands_pow] = raw_num % 10;
                    iBCD[14 - 3 * thousands_pow] = (raw_num / 10) % 10;
                    iBCD[13 - 3 * thousands_pow] = (raw_num / 100);

                    const D64 EXPECTED = D64(0, iBCD, 1);
                    LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
                }
            }

            for (int thousands_pow = 1; thousands_pow <= 5; ++thousands_pow) {
                for (int raw_num = 1; raw_num < 1000; ++raw_num) {
                    // NUM = raw_num * 1000^thousands_pow.
                    unsigned long long NUM = raw_num;
                    for (int i = 0; i < thousands_pow; ++i) {
                        NUM *= 1000;
                    }

                    const Util::ValueType128 ACTUAL =
                        Util::makeDecimalRaw128(NUM, 0);
                    unsigned char iBCD[34] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

                    iBCD[33 - 3 * thousands_pow] = raw_num % 10;
                    iBCD[32 - 3 * thousands_pow] = (raw_num / 10) % 10;
                    iBCD[31 - 3 * thousands_pow] = (raw_num / 100) % 10;

                    const D128 EXPECTED = D128(0, iBCD, 1);
                    LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
                }
            }
            for (int thousands_pow = 6; thousands_pow <= 6; ++thousands_pow) {
                for (int raw_num = 1; raw_num <= 18; ++raw_num) {
                    // NUM = raw_num * 1000^thousands_pow.
                    unsigned long long NUM = raw_num;
                    for (int i = 0; i < thousands_pow; ++i) {
                        NUM *= 1000;
                    }

                    const Util::ValueType128 ACTUAL =
                        Util::makeDecimalRaw128(NUM, 0);
                    unsigned char iBCD[34] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

                    iBCD[33 - 3 * thousands_pow] = raw_num % 10;
                    iBCD[32 - 3 * thousands_pow] = (raw_num / 10) % 10;
                    iBCD[31 - 3 * thousands_pow] = (raw_num / 100) % 10;

                    const D128 EXPECTED = D128(0, iBCD, 1);
                    LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
                }
            }
        }

        if (veryVerbose) bsl::cout << "Test some special cases to prove the "
                                   "correcness of declet bit arithmetic."
                                << bsl::endl;
        {
            // The bit pattern of the declet '255' is '0b0101010101' and that
            // of '582' is '0b1010101010'. If the bit arithmetic is off by a
            // character, it will cause problems in adjacent declets.
            const Util::ValueType64 v255 = Util::makeDecimal64(255, 0);
            const Util::ValueType64 v582 = Util::makeDecimal64(582, 0);
            const unsigned long long v255x = 0x2238000000000155ull;
            const unsigned long long v582x = 0x22380000000002aaull;
            ASSERT(!memcmp(&v255, &v255x, 8));
            ASSERT(!memcmp(&v582, &v582x, 8));

            for (int i = 0; i < 4; i++) {
                unsigned long long NUM =
                    (i & 1 ?    255ull :    582ull) +
                    (i & 2 ? 255000ull : 582000ull);

                const Util::ValueType32 ACTUAL =
                    Util::makeDecimalRaw32(NUM, 0);

                unsigned char iBCD[7] = {0, 0, 0, 0, 0, 0, 0};

                if (i & 1) {
                    iBCD[4] = 2;
                    iBCD[5] = 5;
                    iBCD[6] = 5;
                }
                else {
                    iBCD[4] = 5;
                    iBCD[5] = 8;
                    iBCD[6] = 2;
                }

                if (i & 2) {
                    iBCD[1] = 2;
                    iBCD[2] = 5;
                    iBCD[3] = 5;
                }
                else {
                    iBCD[1] = 5;
                    iBCD[2] = 8;
                    iBCD[3] = 2;
                }

                const D32 EXPECTED = D32(0, iBCD, 1);
                LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
            }

            for (int i = 0; i < 32; i++) {
                unsigned long long NUM =
                    (i &  1 ?             255ull :             582ull) +
                    (i &  2 ?          255000ull :          582000ull) +
                    (i &  4 ?       255000000ull :       582000000ull) +
                    (i &  8 ?    255000000000ull :    582000000000ull) +
                    (i & 16 ? 255000000000000ull : 582000000000000ull);

                const Util::ValueType64 ACTUAL =
                    Util::makeDecimalRaw64(NUM, 0);

                unsigned char iBCD[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                                   0, 0, 0, 0};

                if (i & 1) {
                    iBCD[13] = 2;
                    iBCD[14] = 5;
                    iBCD[15] = 5;
                }
                else {
                    iBCD[13] = 5;
                    iBCD[14] = 8;
                    iBCD[15] = 2;
                }
                if (i & 2) {
                    iBCD[10] = 2;
                    iBCD[11] = 5;
                    iBCD[12] = 5;
                }
                else {
                    iBCD[10] = 5;
                    iBCD[11] = 8;
                    iBCD[12] = 2;
                }
                if (i & 4) {
                    iBCD[7] = 2;
                    iBCD[8] = 5;
                    iBCD[9] = 5;
                }
                else {
                    iBCD[7] = 5;
                    iBCD[8] = 8;
                    iBCD[9] = 2;
                }
                if (i & 8) {
                    iBCD[4] = 2;
                    iBCD[5] = 5;
                    iBCD[6] = 5;
                }
                else {
                    iBCD[4] = 5;
                    iBCD[5] = 8;
                    iBCD[6] = 2;
                }
                if (i & 16) {
                    iBCD[1] = 2;
                    iBCD[2] = 5;
                    iBCD[3] = 5;
                }
                else {
                    iBCD[1] = 5;
                    iBCD[2] = 8;
                    iBCD[3] = 2;
                }

                const D64 EXPECTED = D64(0, iBCD, 1);
                LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
            }

            for (int i = 0; i < 64; i++) {
                unsigned long long NUM =
                    (i &  1 ?                255ull :                582ull) +
                    (i &  2 ?             255000ull :             582000ull) +
                    (i &  4 ?          255000000ull :          582000000ull) +
                    (i &  8 ?       255000000000ull :       582000000000ull) +
                    (i & 16 ?    255000000000000ull :    582000000000000ull) +
                    (i & 32 ? 255000000000000000ull : 582000000000000000ull);

                const Util::ValueType128 ACTUAL =
                    Util::makeDecimalRaw128(NUM, 0);

                unsigned char iBCD[34] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                if (i & 1) {
                    iBCD[31] = 2;
                    iBCD[32] = 5;
                    iBCD[33] = 5;
                }
                else {
                    iBCD[31] = 5;
                    iBCD[32] = 8;
                    iBCD[33] = 2;
                }
                if (i & 2) {
                    iBCD[28] = 2;
                    iBCD[29] = 5;
                    iBCD[30] = 5;
                }
                else {
                    iBCD[28] = 5;
                    iBCD[29] = 8;
                    iBCD[30] = 2;
                }
                if (i & 4) {
                    iBCD[25] = 2;
                    iBCD[26] = 5;
                    iBCD[27] = 5;
                }
                else {
                    iBCD[25] = 5;
                    iBCD[26] = 8;
                    iBCD[27] = 2;
                }
                if (i & 8) {
                    iBCD[22] = 2;
                    iBCD[23] = 5;
                    iBCD[24] = 5;
                }
                else {
                    iBCD[22] = 5;
                    iBCD[23] = 8;
                    iBCD[24] = 2;
                }
                if (i & 16) {
                    iBCD[19] = 2;
                    iBCD[20] = 5;
                    iBCD[21] = 5;
                }
                else {
                    iBCD[19] = 5;
                    iBCD[20] = 8;
                    iBCD[21] = 2;
                }
                if (i & 32) {
                    iBCD[16] = 2;
                    iBCD[17] = 5;
                    iBCD[18] = 5;
                }
                else {
                    iBCD[16] = 5;
                    iBCD[17] = 8;
                    iBCD[18] = 2;
                }

                const D128 EXPECTED = D128(0, iBCD, 1);
                LOOP_ASSERT(NUM, ACTUAL == EXPECTED);
            }
        }

        if (veryVerbose) bsl::cout << "Test the correctness of the "
                                   << "computation of the combination field."
                                   << bsl::endl;
        {
            // Test each of the 9 possible leading digits with each exponent
            // value.
            for (int leading_digit = 1; leading_digit <= 9; ++leading_digit) {
                for (int EXPONENT = -101; EXPONENT <= 90; ++EXPONENT) {
                    unsigned int MANTISSA = leading_digit * 1000000u + 255255u;
                    unsigned char iBCD[7] = {leading_digit, 2, 5, 5, 2, 5, 5};

                    const Util::ValueType32 ACTUAL =
                        Util::makeDecimalRaw32(MANTISSA, EXPONENT);

                    const D32 EXPECTED = D32(EXPONENT, iBCD, 1);
                    LOOP2_ASSERT(MANTISSA, EXPONENT, ACTUAL == EXPECTED);
                }
            }
            for (int leading_digit = 1; leading_digit <= 9; ++leading_digit) {
                for (int EXPONENT = -101; EXPONENT <= 90; ++EXPONENT) {
                    unsigned int MANTISSA = leading_digit * 1000000u + 582582u;
                    unsigned char iBCD[7] = {leading_digit, 5, 8, 2, 5, 8, 2};

                    const Util::ValueType32 ACTUAL =
                        Util::makeDecimalRaw32(MANTISSA, EXPONENT);

                    const D32 EXPECTED = D32(EXPONENT, iBCD, 1);
                    LOOP2_ASSERT(MANTISSA, EXPONENT, ACTUAL == EXPECTED);
                }
            }

            for (int leading_digit = 1; leading_digit <= 9; ++leading_digit) {
                for (int EXPONENT = -398; EXPONENT <= 369; ++EXPONENT) {
                    unsigned long long MANTISSA =
                      leading_digit * 1000000000000000ull + 255255255255255ull;
                    unsigned char iBCD[16] = {leading_digit, 2, 5, 5, 2, 5, 5,
                                              2, 5, 5, 2, 5, 5, 2, 5, 5};

                    const Util::ValueType64 ACTUAL =
                        Util::makeDecimalRaw64(MANTISSA, EXPONENT);

                    const D64 EXPECTED = D64(EXPONENT, iBCD, 1);
                    LOOP2_ASSERT(MANTISSA, EXPONENT, ACTUAL == EXPECTED);
                }
            }
            for (int leading_digit = 1; leading_digit <= 9; ++leading_digit) {
                for (int EXPONENT = -398; EXPONENT <= 369; ++EXPONENT) {
                    unsigned long long MANTISSA =
                      leading_digit * 1000000000000000ull + 582582582582582ull;
                    unsigned char iBCD[16] = {leading_digit, 5, 8, 2, 5, 8, 2,
                                              5, 8, 2, 5, 8, 2, 5, 8, 2};

                    const Util::ValueType64 ACTUAL =
                        Util::makeDecimalRaw64(MANTISSA, EXPONENT);

                    const D64 EXPECTED = D64(EXPONENT, iBCD, 1);
                    LOOP2_ASSERT(MANTISSA, EXPONENT, ACTUAL == EXPECTED);
                }
            }
        }

        if (veryVerbose) bsl::cout << "Test makeDecimalRaw32, "
                                   << "makeDecimalRaw64, and makeDecimal128."
                                   << bsl::endl;
        static const struct {
            int           d_lineNum;
            const char   *d_mantissa;
            int           d_exponent;
            int           d_expectedExponent;
            unsigned char d_expectedBCD[34];
            int           d_expectedSign;
        } MAKE_DECIMAL_RAW_TESTS[] = {
            { L_,                    "0",     0, 0,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0}, 1},
            { L_,                    "0",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0}, 1},
            { L_,                    "0",   -10, -10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0}, 1},
            { L_,                  "100",    -2, -2,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 0, 0}, 1},
            { L_,                    "1",     2, 2,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                    "1",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                   "12",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 1, 2}, 1},
            { L_,                  "123",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 2, 3}, 1},
            { L_,                 "1234",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 1, 2, 3, 4}, 1},
            { L_,                "12345",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 1, 2, 3, 4, 5}, 1},
            { L_,               "123456",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 1, 2, 3, 4, 5, 6}, 1},
            { L_,              "1234567",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               1, 2, 3, 4, 5, 6, 7}, 1},
            { L_,             "12345678",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 1,
               2, 3, 4, 5, 6, 7, 8}, 1},
            { L_,            "123456789",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9}, 1},
            { L_,           "1234567890",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1, 2, 3,
               4, 5, 6, 7, 8, 9, 0}, 1},
            { L_,           "2147483647",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 2, 1, 4,
               7, 4, 8, 3, 6, 4, 7}, 1},
            { L_,           "4294967295",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 4, 2, 9,
               4, 9, 6, 7, 2, 9, 5}, 1},
            { L_,          "12345678901",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 1, 2, 3, 4,
               5, 6, 7, 8, 9, 0, 1}, 1},
            { L_,         "123456789012",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 2, 3, 4, 5,
               6, 7, 8, 9, 0, 1, 2}, 1},
            { L_,        "1234567890123",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 1, 2, 3, 4, 5, 6,
               7, 8, 9, 0, 1, 2, 3}, 1},
            { L_,       "12345678901234",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 1, 2, 3, 4, 5, 6, 7,
               8, 9, 0, 1, 2, 3, 4}, 1},
            { L_,      "123456789012345",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 1, 2, 3, 4, 5, 6, 7, 8,
               9, 0, 1, 2, 3, 4, 5}, 1},
            { L_,     "1234567890123456",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               1, 2, 3, 4, 5, 6, 7, 8, 9,
               0, 1, 2, 3, 4, 5, 6}, 1},
            { L_,    "12345678901234567",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 1,
               2, 3, 4, 5, 6, 7, 8, 9, 0,
               1, 2, 3, 4, 5, 6, 7}, 1},
            { L_,   "123456789012345678",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9, 0, 1,
               2, 3, 4, 5, 6, 7, 8}, 1},
            { L_,  "1234567890123456789",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1, 2, 3,
               4, 5, 6, 7, 8, 9, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9}, 1},
            { L_,  "9223372036854775807",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 9, 2, 2, 3, 3,
               7, 2, 0, 3, 6, 8, 5, 4, 7, 7,
               5, 8, 0, 7}, 1},
            { L_, "18446744073709551615",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 8, 4, 4, 6, 7,
               4, 4, 0, 7, 3, 7, 0, 9, 5, 5,
               1, 6, 1, 5}},
            { L_,                    "-1",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},
            { L_,                   "-12",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 1, 2}, -1},
            { L_,                  "-123",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 2, 3}, -1},
            { L_,                 "-1234",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 1, 2, 3, 4}, -1},
            { L_,                "-12345",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 1, 2, 3, 4, 5}, -1},
            { L_,               "-123456",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 1, 2, 3, 4, 5, 6}, -1},
            { L_,              "-1234567",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               1, 2, 3, 4, 5, 6, 7}, -1},
            { L_,             "-12345678",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 1,
               2, 3, 4, 5, 6, 7, 8}, -1},
            { L_,            "-123456789",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9}, -1},
            { L_,           "-1234567890",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1, 2, 3,
               4, 5, 6, 7, 8, 9, 0}, -1},
            { L_,           "-2147483647",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 2, 1, 4,
               7, 4, 8, 3, 6, 4, 7}, -1},
            { L_,           "-2147483648",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 2, 1, 4,
               7, 4, 8, 3, 6, 4, 8}, -1},
            { L_,          "-12345678901",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 1, 2, 3, 4,
               5, 6, 7, 8, 9, 0, 1}, -1},
            { L_,         "-123456789012",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 2, 3, 4, 5,
               6, 7, 8, 9, 0, 1, 2}, -1},
            { L_,        "-1234567890123",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 1, 2, 3, 4, 5, 6,
               7, 8, 9, 0, 1, 2, 3}, -1},
            { L_,       "-12345678901234",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 1, 2, 3, 4, 5, 6, 7,
               8, 9, 0, 1, 2, 3, 4}, -1},
            { L_,      "-123456789012345",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 1, 2, 3, 4, 5, 6, 7, 8,
               9, 0, 1, 2, 3, 4, 5}, -1},
            { L_,     "-1234567890123456",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               1, 2, 3, 4, 5, 6, 7, 8, 9,
               0, 1, 2, 3, 4, 5, 6}, -1},
            { L_,    "-12345678901234567",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 1,
               2, 3, 4, 5, 6, 7, 8, 9, 0,
               1, 2, 3, 4, 5, 6, 7}, -1},
            { L_,   "-123456789012345678",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9, 0, 1,
               2, 3, 4, 5, 6, 7, 8}, -1},
            { L_,  "-1234567890123456789",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1, 2, 3,
               4, 5, 6, 7, 8, 9, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9}, -1},
            { L_,  "-9223372036854775807",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 9, 2, 2, 3, 3,
               7, 2, 0, 3, 6, 8, 5, 4, 7, 7,
               5, 8, 0, 7}, -1},
            { L_,  "-9223372036854775808",   10, 10,
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 9, 2, 2, 3, 3,
                 7, 2, 0, 3, 6, 8, 5, 4, 7, 7,
                 5, 8, 0, 8}, -1},

            { L_,                    "1",  -101, -101,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                    "1",    90, 90,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                   "-1",  -101, -101,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},
            { L_,                   "-1",    90, 90,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},

            { L_,                    "1",  -398, -398,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                    "1",   369, 369,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                   "-1",  -398, -398,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},
            { L_,                   "-1",   369, 369,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},

            { L_,                    "1", -6176, -6176,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                    "1",  6111,  6111,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, 1},
            { L_,                   "-1", -6176, -6176,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},
            { L_,                   "-1",  6111,  6111,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1}, -1},
        };

        const int NUM_MAKE_DECIMAL_RAW_TESTS =
            sizeof MAKE_DECIMAL_RAW_TESTS / sizeof *MAKE_DECIMAL_RAW_TESTS;

        for (int ti = 0; ti < NUM_MAKE_DECIMAL_RAW_TESTS; ++ti) {
            const int LINE = MAKE_DECIMAL_RAW_TESTS[ti].d_lineNum;

            const bsl::string MANTISSA(
                MAKE_DECIMAL_RAW_TESTS[ti].d_mantissa, pa);
            const int EXPONENT = MAKE_DECIMAL_RAW_TESTS[ti].d_exponent;

            const D128 EXPECTED =
                D128(MAKE_DECIMAL_RAW_TESTS[ti].d_expectedExponent,
                     MAKE_DECIMAL_RAW_TESTS[ti].d_expectedBCD,
                     MAKE_DECIMAL_RAW_TESTS[ti].d_expectedSign);

            int mantissa_in_int;
            unsigned int mantissa_in_uint;
            long long mantissa_in_ll;
            unsigned long long mantissa_in_ull;

            if (parseInt(MANTISSA, &mantissa_in_int, pa)) {
                const Util::ValueType128 ACTUAL =
                    Util::makeDecimalRaw128(mantissa_in_int, EXPONENT);
                LOOP3_ASSERT(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            }

            if (parseUInt(MANTISSA, &mantissa_in_uint, pa)) {
                const Util::ValueType128 ACTUAL =
                    Util::makeDecimalRaw128(mantissa_in_uint, EXPONENT);
                LOOP3_ASSERT(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            }

            if (parseLL(MANTISSA, &mantissa_in_ll, pa)) {
                const Util::ValueType128 ACTUAL =
                    Util::makeDecimalRaw128(mantissa_in_ll, EXPONENT);
                LOOP3_ASSERT(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            }

            if (parseULL(MANTISSA, &mantissa_in_ull, pa)) {
                const Util::ValueType128 ACTUAL =
                    Util::makeDecimalRaw128(mantissa_in_ull, EXPONENT);
                LOOP3_ASSERT(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            }

            // Check if within 64-bit decimal floating-point range.
            if ((parseLL(MANTISSA, &mantissa_in_ll, pa)) &&
                (-9999999999999999ll <= mantissa_in_ll) &&
                (mantissa_in_ll <= 9999999999999999ll) &&
                (-398 <= EXPONENT) && (EXPONENT <= 369)) {
                const D64 EXPECTED64 =
                    D64(MAKE_DECIMAL_RAW_TESTS[ti].d_expectedExponent,
                        MAKE_DECIMAL_RAW_TESTS[ti].d_expectedBCD,
                        MAKE_DECIMAL_RAW_TESTS[ti].d_expectedSign);

                if (parseInt(MANTISSA, &mantissa_in_int, pa)) {
                    const Util::ValueType64 ACTUAL64 =
                        Util::makeDecimalRaw64(mantissa_in_int, EXPONENT);
                    LOOP3_ASSERT(LINE, ACTUAL64, EXPECTED64,
                                 ACTUAL64 == EXPECTED64);
                }

                if (parseUInt(MANTISSA, &mantissa_in_uint, pa)) {
                    const Util::ValueType64 ACTUAL64 =
                        Util::makeDecimalRaw64(mantissa_in_uint, EXPONENT);
                    LOOP3_ASSERT(LINE, ACTUAL64, EXPECTED64,
                                 ACTUAL64 == EXPECTED64);
                }

                if (parseLL(MANTISSA, &mantissa_in_ll, pa)) {
                    const Util::ValueType64 ACTUAL64 =
                        Util::makeDecimalRaw64(mantissa_in_ll, EXPONENT);
                    LOOP3_ASSERT(LINE, ACTUAL64, EXPECTED64,
                                 ACTUAL64 == EXPECTED64);
                }

                if (parseULL(MANTISSA, &mantissa_in_ull, pa)) {
                    const Util::ValueType64 ACTUAL64 =
                        Util::makeDecimalRaw64(mantissa_in_ull, EXPONENT);
                    LOOP3_ASSERT(LINE, ACTUAL64, EXPECTED64,
                                 ACTUAL64 == EXPECTED64);
                }
            }

            // Check if within 32-bit decimal floating-point range.
            if ((parseInt(MANTISSA, &mantissa_in_int, pa)) &&
                (-9999999 <= mantissa_in_int) &&
                (mantissa_in_int <= 9999999) &&
                (-98 <= EXPONENT) && (EXPONENT <= 90)) {
                const D32 EXPECTED32 =
                    D32(MAKE_DECIMAL_RAW_TESTS[ti].d_expectedExponent,
                        MAKE_DECIMAL_RAW_TESTS[ti].d_expectedBCD,
                        MAKE_DECIMAL_RAW_TESTS[ti].d_expectedSign);

                if (parseInt(MANTISSA, &mantissa_in_int, pa)) {
                    const Util::ValueType32 ACTUAL32 =
                        Util::makeDecimalRaw32(mantissa_in_int, EXPONENT);
                    LOOP_ASSERT(LINE, ACTUAL32 == EXPECTED32);
                }
            }
        }
        #endif
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
