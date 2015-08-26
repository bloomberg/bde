// bdlb_bitutil.t.cpp                                                 -*-C++-*-
#include <bdlb_bitutil.h>

#include <bslim_testutil.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

using bsl::int32_t;
using bsl::int64_t;
using bsl::uint32_t;
using bsl::uint64_t;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test provides static methods that perform various bit
// related computations.  The goal of this 'bdlb::BitUtil' test suite is to
// verify that the methods return the expected values.  The test techniques
// incorporated to obtain this goal are boundary value testing and depth
// enumeration testing.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] bool isBitSet(uint32_t value, int index);
// [ 2] bool isBitSet(uint64_t value, int index);
// [ 6] int log2(uint32_t value);
// [ 6] int log2(uint64_t value);
// [ 4] int numBitsSet(uint32_t value);
// [ 4] int numBitsSet(uint64_t value);
// [ 5] int numLeadingUnsetBits(uint32_t value);
// [ 5] int numLeadingUnsetBits(uint64_t value);
// [ 5] int numTrailingUnsetBits(uint32_t value);
// [ 5] int numTrailingUnsetBits(uint64_t value);
// [ 7] uint32_t roundUp(uint32_t value, uint32_t boundary);
// [ 7] uint64_t roundUp(uint64_t value, uint64_t boundary);
// [ 6] uint32_t roundUpToBinaryPower(uint32_t value);
// [ 6] uint64_t roundUpToBinaryPower(uint64_t value);
// [ 1] int sizeInBits(INTEGER value);
// [ 3] uint32_t withBitCleared(uint32_t value, int index);
// [ 3] uint64_t withBitCleared(uint64_t value, int index);
// [ 3] uint32_t withBitSet(uint32_t value, int index);
// [ 3] uint64_t withBitSet(uint64_t value, int index);
//-----------------------------------------------------------------------------
// [ 8] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlb::BitUtil       Util;

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// The following usage examples illustrate how some of the methods provided by
// this component are used.  Note that, in all of these examples, the low-order
// bit is considered bit 0 and resides on the right edge of the bit string.
//
// First, we use 'withBitSet' to demonstrate the ordering of bits:
//..
    ASSERT(static_cast<uint32_t>(0x00000001)
                   == bdlb::BitUtil::withBitSet(static_cast<uint32_t>(0),  0));
    ASSERT(static_cast<uint32_t>(0x00000008)
                   == bdlb::BitUtil::withBitSet(static_cast<uint32_t>(0),  3));
    ASSERT(static_cast<uint32_t>(0x00800000)
                   == bdlb::BitUtil::withBitSet(static_cast<uint32_t>(0), 23));
    ASSERT(static_cast<uint32_t>(0x66676666)
          == bdlb::BitUtil::withBitSet(static_cast<uint32_t>(0x66666666), 16));

    /*------------------------------------------------------------------------+
    | 'bdlb::BitUtil::withBitSet(0x66666666, 16)' in binary:                  |
    |                                                                         |
    | input in binary:                       01100110011001100110011001100110 |
    | set bit 16:                                           1                 |
    | result:                                01100110011001110110011001100110 |
    +------------------------------------------------------------------------*/
//..
// Then, we count the number of set bits in a value with 'numBitsSet':
//..
    ASSERT(0 == bdlb::BitUtil::numBitsSet(static_cast<uint32_t>(0x00000000)));
    ASSERT(2 == bdlb::BitUtil::numBitsSet(static_cast<uint32_t>(0x00101000)));
    ASSERT(8 == bdlb::BitUtil::numBitsSet(static_cast<uint32_t>(0x30071101)));

    /*------------------------------------------------------------------------+
    | 'bdlb::BitUtil::numBitsSet(0x30071101)' in binary:                      |
    |                                                                         |
    | input in binary:                       00110000000001110001000100000001 |
    | that has 8 bits set.  result: 8                                         |
    +------------------------------------------------------------------------*/
//..
// Finally, we use 'numLeadingUnsetBits' to determine the number of unset bits
// with a higher index than the first set bit:
//..
    ASSERT(32 ==
        bdlb::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x00000000)));
    ASSERT(31 ==
        bdlb::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x00000001)));
    ASSERT(7 ==
        bdlb::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x01000000)));
    ASSERT(7 ==
        bdlb::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x01620030)));

    /*------------------------------------------------------------------------+
    | 'bdlb::BitUtil::numLeadingUnsetBits(0x01620030)' in binary:             |
    |                                                                         |
    | input in binary:                       00000001011000100000000000110000 |
    | highest set bit:                              1                         |
    | number of unset bits leading this set bit == 7                          |
    +------------------------------------------------------------------------*/
//..

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'roundUp'
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 The methods correctly perform the calculations.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Verify return value for input values 2^i - 1, 2^i, 2^i + 1 and
        //:   all possible 'boundary' values where i = 1 .. 31 for 'uint32_t'
        //:   and i = 1 .. 63 for 'uint64_t'.
        //:
        //: 2 Verify return values when all bits are set and all possible
        //:   'boundary' values in the input value.  (C-1)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   uint32_t roundUp(uint32_t value, uint32_t boundary);
        //   uint64_t roundUp(uint64_t value, uint64_t boundary);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'roundUp'" << endl
                          << "=================" << endl;

        for (int b = 0; b < 32; ++b) {
            uint32_t boundary = static_cast<uint32_t>(1) << b;
            for (int i = 0; i < 32; ++i) {
                for (int d = -1; d <= 1; ++d) {
                    uint32_t value32 = (static_cast<uint32_t>(1) << i) + d;
                    // NOTE: the only possible overflow scenario in the
                    // following correctly results in 0 since 'boundary' is a
                    // power of two
                    const uint32_t EXP = (value32 % boundary
                                          ? (value32 / boundary + 1) * boundary
                                          : value32);
                    uint32_t rv = Util::roundUp(value32, boundary);
                    if (veryVerbose) {
                        T_ P_(value32); T_ P_(boundary); T_ P_(rv); T_ P(EXP);
                    }
                    LOOP3_ASSERT(b, i, d, EXP == rv);
                }
            }
        }
        for (int b = 0; b < 64; ++b) {
            uint64_t boundary = static_cast<uint64_t>(1) << b;
            for (int i = 0; i < 64; ++i) {
                for (int d = -1; d <= 1; ++d) {
                    uint64_t value64 = (static_cast<uint64_t>(1) << i) + d;
                    // NOTE: the only possible overflow scenario in the
                    // following correctly results in 0 since 'boundary' is a
                    // power of two
                    const uint64_t EXP = (value64 % boundary
                                          ? (value64 / boundary + 1) * boundary
                                          : value64);
                    uint64_t rv = Util::roundUp(value64, boundary);
                    if (veryVerbose) {
                        T_ P_(value64); T_ P_(boundary); T_ P_(rv); T_ P(EXP);
                    }
                    LOOP3_ASSERT(b, i, d, EXP == rv);
                }
            }
        }

        { // verify all bits set
            uint32_t value32 = static_cast<uint32_t>(-1);
            ASSERT(value32 == Util::roundUp(value32,
                                            static_cast<uint32_t>(1)));
            for (int b = 1; b < 32; ++b) {
                uint32_t boundary = static_cast<uint32_t>(1) << b;
                const int EXP = 0;
                uint32_t rv = Util::roundUp(value32, boundary);
                if (veryVerbose) {
                    T_ P_(value32); T_ P_(boundary); T_ P_(rv); T_ P(EXP);
                }
                LOOP_ASSERT(b, EXP == rv);
            }
            uint64_t value64 = static_cast<uint64_t>(-1);
            ASSERT(value64 == Util::roundUp(value64,
                                            static_cast<uint64_t>(1)));
            for (int b = 1; b < 64; ++b) {
                uint64_t boundary = static_cast<uint64_t>(1) << b;
                const int EXP = 0;
                uint64_t rv = Util::roundUp(value64, boundary);
                if (veryVerbose) {
                    T_ P_(value64); T_ P_(boundary); T_ P_(rv); T_ P(EXP);
                }
                LOOP_ASSERT(b, EXP == rv);
            }
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            // one bit set in 'boundary'
            for (int b = 0; b < 32; ++b) {
                uint32_t boundary = static_cast<uint32_t>(1) << b;
                if (veryVerbose) {
                    T_ P(boundary);
                }
                ASSERT_SAFE_PASS(Util::roundUp(static_cast<uint32_t>(0),
                                               boundary));
            }
            for (int b = 0; b < 64; ++b) {
                uint64_t boundary = static_cast<uint64_t>(1) << b;
                if (veryVerbose) {
                    T_ P(boundary);
                }
                ASSERT_SAFE_PASS(Util::roundUp(static_cast<uint64_t>(0),
                                               boundary));
            }

            // no bits set in 'boundary'
            ASSERT_SAFE_FAIL(Util::roundUp(static_cast<uint32_t>(0),
                                           static_cast<uint32_t>(0)));
            ASSERT_SAFE_FAIL(Util::roundUp(static_cast<uint64_t>(0),
                                           static_cast<uint64_t>(0)));

            // more than one bit set in 'boundary'
            ASSERT_SAFE_FAIL(Util::roundUp(static_cast<uint32_t>(0),
                                           static_cast<uint32_t>(3)));
            ASSERT_SAFE_FAIL(Util::roundUp(static_cast<uint64_t>(0),
                                           static_cast<uint64_t>(3)));
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'log2' & 'roundUpToBinaryPower'
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 The methods correctly perform the calculations.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Verify return value for input values 1 and 2.
        //:
        //: 2 Verify return value for input values 2^i - 1, 2^i, 2^i + 1 where
        //:   i = 2 .. 30 for 'uint32_t' and i = 2 .. 62 for 'uint64_t'.
        //:
        //: 3 For 'roundUpToBinaryPower', verify behavior at input value 0.
        //:
        //: 4 Verify return value for input values 2^x - 1, 2^x, 2^x + 1 where
        //:   x = 31 for 'uint32_t' and x = 63 for 'uint64_t'.
        //:
        //: 5 Verify return values when all bits are set in the input value.
        //:   (C-1)
        //:
        //: 6 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   int log2(uint32_t value);
        //   int log2(uint64_t value);
        //   uint32_t roundUpToBinaryPower(uint32_t value);
        //   uint64_t roundUpToBinaryPower(uint64_t value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'log2' & 'roundUpToBinaryPower'" << endl
                          << "=======================================" << endl;

        { // verify values 1 and 2
            ASSERT(0 == Util::log2(static_cast<uint32_t>(1)));
            ASSERT(1 == Util::log2((uint32_t)2));
            ASSERT(1 == Util::roundUpToBinaryPower(static_cast<uint32_t>(1)));
            ASSERT(2 == Util::roundUpToBinaryPower((uint32_t)2));
            ASSERT(0 == Util::log2(static_cast<uint64_t>(1)));
            ASSERT(1 == Util::log2((uint64_t)2));
            ASSERT(1 == Util::roundUpToBinaryPower(static_cast<uint64_t>(1)));
            ASSERT(2 == Util::roundUpToBinaryPower((uint64_t)2));
        }
        { // verify values that are >= 3 and <= (2 << (30|62)) + 1
            for (int i = 2; i < 31; ++i) {
                // NOTE: 2^2 - 1 == 3
                {
                    uint32_t value32 = (static_cast<uint32_t>(1) << i) - 1;
                    const int EXP = i;
                    int rv = Util::log2(value32);
                    if (veryVerbose) { T_ P_(value32); T_ P_(rv); T_ P(EXP); }
                    LOOP_ASSERT(i, EXP == rv);
                }
                {
                    uint32_t value32 = (static_cast<uint32_t>(1) << i);
                    const int EXP = i;
                    int rv = Util::log2(value32);
                    if (veryVerbose) { T_ P_(value32); T_ P_(rv); T_ P(EXP); }
                    LOOP_ASSERT(i, EXP == rv);
                }
                {
                    uint32_t value32 = (static_cast<uint32_t>(1) << i) + 1;
                    const int EXP = i + 1;
                    int rv = Util::log2(value32);
                    if (veryVerbose) { T_ P_(value32); T_ P_(rv); T_ P(EXP); }
                    LOOP_ASSERT(i, EXP == rv);
                }
                {
                    uint32_t value32 = (static_cast<uint32_t>(1) << i) - 1;
                    const uint32_t EXP = static_cast<uint32_t>(1) << i;
                    uint32_t rv = Util::roundUpToBinaryPower(value32);
                    if (veryVerbose) { T_ P_(value32); T_ P_(rv); T_ P(EXP); }
                    LOOP_ASSERT(i, EXP == rv);
                }
                {
                    uint32_t value32 = (static_cast<uint32_t>(1) << i);
                    const uint32_t EXP = static_cast<uint32_t>(1) << i;
                    uint32_t rv = Util::roundUpToBinaryPower(value32);
                    if (veryVerbose) { T_ P_(value32); T_ P_(rv); T_ P(EXP); }
                    LOOP_ASSERT(i, EXP == rv);
                }
                {
                    uint32_t value32 = (static_cast<uint32_t>(1) << i) + 1;
                    const uint32_t EXP = static_cast<uint32_t>(1) << (i + 1);
                    uint32_t rv = Util::roundUpToBinaryPower(value32);
                    if (veryVerbose) { T_ P_(value32); T_ P_(rv); T_ P(EXP); }
                    LOOP_ASSERT(i, EXP == rv);
                }
            }
            for (int i = 2; i < 63; ++i) {
                // NOTE: 2^2 - 1 == 3
                {
                    uint64_t value64 = (static_cast<uint64_t>(1) << i) - 1;
                    const int EXP = i;
                    int rv = Util::log2(value64);
                    if (veryVerbose) { T_ P_(value64); T_ P_(rv); T_ P(EXP); }
                    LOOP_ASSERT(i, EXP == rv);
                }
                {
                    uint64_t value64 = (static_cast<uint64_t>(1) << i);
                    const int EXP = i;
                    int rv = Util::log2(value64);
                    if (veryVerbose) { T_ P_(value64); T_ P_(rv); T_ P(EXP); }
                    LOOP_ASSERT(i, EXP == rv);
                }
                {
                    uint64_t value64 = (static_cast<uint64_t>(1) << i) + 1;
                    const int EXP = i + 1;
                    int rv = Util::log2(value64);
                    if (veryVerbose) { T_ P_(value64); T_ P_(rv); T_ P(EXP); }
                    LOOP_ASSERT(i, EXP == rv);
                }
                {
                    uint64_t value64 = (static_cast<uint64_t>(1) << i) - 1;
                    const uint64_t EXP = static_cast<uint64_t>(1) << i;
                    uint64_t rv = Util::roundUpToBinaryPower(value64);
                    if (veryVerbose) { T_ P_(value64); T_ P_(rv); T_ P(EXP); }
                    LOOP_ASSERT(i, EXP == rv);
                }
                {
                    uint64_t value64 = (static_cast<uint64_t>(1) << i);
                    const uint64_t EXP = static_cast<uint64_t>(1) << i;
                    uint64_t rv = Util::roundUpToBinaryPower(value64);
                    if (veryVerbose) { T_ P_(value64); T_ P_(rv); T_ P(EXP); }
                    LOOP_ASSERT(i, EXP == rv);
                }
                {
                    uint64_t value64 = (static_cast<uint64_t>(1) << i) + 1;
                    const uint64_t EXP = static_cast<uint64_t>(1) << (i + 1);
                    uint64_t rv = Util::roundUpToBinaryPower(value64);
                    if (veryVerbose) { T_ P_(value64); T_ P_(rv); T_ P(EXP); }
                    LOOP_ASSERT(i, EXP == rv);
                }
            }
        }

        { // verify value 0 for 'roundUpToBinaryPower'
            // NOTE: 0 is undefined for 'log2'; see negative testing
            ASSERT(0 == Util::roundUpToBinaryPower(static_cast<uint32_t>(0)));
            ASSERT(0 == Util::roundUpToBinaryPower(static_cast<uint64_t>(0)));
        }

        { // verify extreme values
            uint32_t value32 = static_cast<uint32_t>(1) << 31;
            uint64_t value64 = static_cast<uint64_t>(1) << 63;

            ASSERT(     31 == Util::log2(value32 - 1));
            ASSERT(     63 == Util::log2(value64 - 1));
            ASSERT(value32 == Util::roundUpToBinaryPower(value32 - 1));
            ASSERT(value64 == Util::roundUpToBinaryPower(value64 - 1));

            ASSERT(     31 == Util::log2(value32));
            ASSERT(     63 == Util::log2(value64));
            ASSERT(value32 == Util::roundUpToBinaryPower(value32));
            ASSERT(value64 == Util::roundUpToBinaryPower(value64));

            ASSERT(     32 == Util::log2(value32 + 1));
            ASSERT(     64 == Util::log2(value64 + 1));
            ASSERT(      0 == Util::roundUpToBinaryPower(value32 + 1));
            ASSERT(      0 == Util::roundUpToBinaryPower(value64 + 1));
        }

        { // verify all bits set
            uint32_t value32 = static_cast<uint32_t>(-1);
            uint64_t value64 = static_cast<uint64_t>(-1);
            ASSERT(32 == Util::log2(value32));
            ASSERT(64 == Util::log2(value64));
            ASSERT( 0 == Util::roundUpToBinaryPower(value32));
            ASSERT( 0 == Util::roundUpToBinaryPower(value64));
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_FAIL(Util::log2(static_cast<uint32_t>(0)));
            ASSERT_SAFE_FAIL(Util::log2(static_cast<uint64_t>(0)));

            ASSERT_SAFE_PASS(Util::log2(static_cast<uint32_t>(1)));
            ASSERT_SAFE_PASS(Util::log2(static_cast<uint64_t>(1)));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'num(*)UnsetBits'
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 Methods correctly return the number of unset bits prior to a set
        //:   bit for the specified direction.
        //
        // Plan:
        //: 1 Verify return values for depth enumerated test vectors with known
        //:   expected results.
        //:
        //: 2 Verify results for case where all bits are set.  (C-1)
        //
        // Testing:
        //   int numLeadingUnsetBits(uint32_t value);
        //   int numLeadingUnsetBits(uint64_t value);
        //   int numTrailingUnsetBits(uint32_t value);
        //   int numTrailingUnsetBits(uint64_t value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'num(*)UnsetBits'" << endl
                          << "=========================" << endl;

        if (verbose) {
            cout << "'bdlb::BitUtil::numLeadingUnsetBits(value)'"
                 << endl;
        }

        { // no bits set
            uint32_t value32 = static_cast<uint32_t>(0);
            ASSERT(32 == Util::numLeadingUnsetBits(value32));
            uint64_t value64 = static_cast<uint64_t>(0);
            ASSERT(64 == Util::numLeadingUnsetBits(value64));
        }

        { // test for 1 to 3 bits set
            for (int i = 0; i < 32; ++i) {
                for (int j = 0; j <= i; ++j) {
                    for (int k = 0; k <= j; ++k) {
                        uint32_t value32 = ((static_cast<uint32_t>(1) << i)
                                            | (static_cast<uint32_t>(1) << j)
                                            | (static_cast<uint32_t>(1) << k));
                        const int EXP = 31 - i;
                        int rv = Util::numLeadingUnsetBits(value32);
                        if (veryVerbose) {
                            T_ P_(value32); T_ P_(rv); T_ P(EXP);
                        }
                        LOOP3_ASSERT(i, j, k, EXP == rv);
                    }
                }
            }
            for (int i = 0; i < 64; ++i) {
                for (int j = 0; j <= i; ++j) {
                    for (int k = 0; k <= j; ++k) {
                        uint64_t value64 = ((static_cast<uint64_t>(1) << i)
                                            | (static_cast<uint64_t>(1) << j)
                                            | (static_cast<uint64_t>(1) << k));
                        const int EXP = 63 - i;
                        int rv = Util::numLeadingUnsetBits(value64);
                        if (veryVerbose) {
                            T_ P_(value64); T_ P_(rv); T_ P(EXP);
                        }
                        LOOP3_ASSERT(i, j, k, EXP == rv);
                    }
                }
            }
        }

        { // all bits set
            uint32_t value32 = static_cast<uint32_t>(-1);
            ASSERT(0 == Util::numLeadingUnsetBits(value32));
            uint64_t value64 = static_cast<uint64_t>(-1);
            ASSERT(0 == Util::numLeadingUnsetBits(value64));
        }

        if (verbose) {
            cout << "'bdlb::BitUtil::numTrailingUnsetBits(value)'"
                 << endl;
        }

        { // no bits set
            uint32_t value32 = static_cast<uint32_t>(0);
            ASSERT(32 == Util::numTrailingUnsetBits(value32));
            uint64_t value64 = static_cast<uint64_t>(0);
            ASSERT(64 == Util::numTrailingUnsetBits(value64));
        }

        { // test for 1 to 3 bits set
            for (int i = 0; i < 32; ++i) {
                for (int j = i; j < 32; ++j) {
                    for (int k = j; k < 32; ++k) {
                        uint32_t value32 = ((static_cast<uint32_t>(1) << i)
                                            | (static_cast<uint32_t>(1) << j)
                                            | (static_cast<uint32_t>(1) << k));
                        const int EXP = i;
                        int rv = Util::numTrailingUnsetBits(value32);
                        if (veryVerbose) {
                            T_ P_(value32); T_ P_(rv); T_ P(EXP);
                        }
                        LOOP3_ASSERT(i, j, k, EXP == rv);
                    }
                }
            }
            for (int i = 0; i < 64; ++i) {
                for (int j = i; j < 64; ++j) {
                    for (int k = j; k < 64; ++k) {
                        uint64_t value64 = ((static_cast<uint64_t>(1) << i)
                                            | (static_cast<uint64_t>(1) << j)
                                            | (static_cast<uint64_t>(1) << k));
                        const int EXP = i;
                        int rv = Util::numTrailingUnsetBits(value64);
                        if (veryVerbose) {
                            T_ P_(value64); T_ P_(rv); T_ P(EXP);
                        }
                        LOOP3_ASSERT(i, j, k, EXP == rv);
                    }
                }
            }
        }

        { // all bits set
            uint32_t value32 = static_cast<uint32_t>(-1);
            ASSERT(0 == Util::numTrailingUnsetBits(value32));
            uint64_t value64 = static_cast<uint64_t>(-1);
            ASSERT(0 == Util::numTrailingUnsetBits(value64));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'numBitsSet'
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 'numBitsSet' correctly returns the number of set bits.
        //
        // Plan:
        //: 1 Verify return values for depth enumerated test vectors with known
        //:   expected results.
        //:
        //: 2 Verify results for case where all bits are set.  (C-1)
        //
        // Testing:
        //   int numBitsSet(uint32_t value);
        //   int numBitsSet(uint64_t value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'numBitsSet'" << endl
                          << "====================" << endl;

        { // depth 0; no bits set
            uint32_t value32 = 0;
            ASSERT(0 == Util::numBitsSet(value32));
            uint64_t value64 = 0;
            ASSERT(0 == Util::numBitsSet(value64));
        }

        { // depth 1; one bit set
            for (int i = 0; i < 32; ++i) {
                uint32_t value32 = static_cast<uint32_t>(1) << i;
                const int EXP = 1;
                int rv = Util::numBitsSet(value32);
                if (veryVerbose) { T_ P_(value32); T_ P_(rv); T_ P(EXP); }
                LOOP_ASSERT(i, EXP == rv);
            }
            for (int i = 0; i < 64; ++i) {
                uint64_t value64 = static_cast<uint64_t>(1) << i;
                const int EXP = 1;
                int rv = Util::numBitsSet(value64);
                if (veryVerbose) { T_ P_(value64); T_ P_(rv); T_ P(EXP); }
                LOOP_ASSERT(i, EXP == rv);
            }
        }

        { // depth 2; two bits set
            for (int i = 0; i < 32; ++i) {
                for (int j = i + 1; j < 32; ++j) {
                    uint32_t value32 = ((static_cast<uint32_t>(1) << i)
                                        | (static_cast<uint32_t>(1) << j));
                    const int EXP = 2;
                    int rv = Util::numBitsSet(value32);
                    if (veryVerbose) { T_ P_(value32); T_ P_(rv); T_ P(EXP); }
                    LOOP2_ASSERT(i, j, EXP == rv);
                }
            }
            for (int i = 0; i < 64; ++i) {
                for (int j = i + 1; j < 64; ++j) {
                    uint64_t value64 = ((static_cast<uint64_t>(1) << i)
                                        | (static_cast<uint64_t>(1) << j));
                    const int EXP = 2;
                    int rv = Util::numBitsSet(value64);
                    if (veryVerbose) { T_ P_(value64); T_ P_(rv); T_ P(EXP); }
                    LOOP2_ASSERT(i, j, EXP == rv);
                }
            }
        }

        { // all bits set
            uint32_t value32 = static_cast<uint32_t>(-1);
            ASSERT(32 == Util::numBitsSet(value32));
            uint64_t value64 = static_cast<uint64_t>(-1);
            ASSERT(64 == Util::numBitsSet(value64));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'withBitCleared' & 'withBitSet'
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 The methods correctly adjust the bit in the specified index.
        //:
        //: 2 The methods do not adjust the bits in other index positions.
        //:
        //: 3 The methods work for all index positions.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Starting with the values 0 and all-bits-set, for every index
        //:   perform both operations and verify the result values.  (C-1..3)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   uint32_t withBitCleared(uint32_t value, int index);
        //   uint64_t withBitCleared(uint64_t value, int index);
        //   uint32_t withBitSet(uint32_t value, int index);
        //   uint64_t withBitSet(uint64_t value, int index);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'withBitCleared' & 'withBitSet'" << endl
                          << "=======================================" << endl;

        { // starting from 0
            for (int index = 0; index < 32; ++index) {
                const uint32_t value32 = 0;
                {
                    const uint32_t EXP = 0;
                    uint32_t rv = Util::withBitCleared(value32, index);
                    if (veryVerbose) {
                        T_ P_(value32); T_ P_(index); T_ P_(rv); T_ P(EXP);
                    }
                    LOOP_ASSERT(index, EXP == rv);
                }
                {
                    const uint32_t EXP = static_cast<uint32_t>(1) << index;
                    uint32_t rv = Util::withBitSet(value32, index);
                    if (veryVerbose) {
                        T_ P_(value32); T_ P_(index); T_ P_(rv); T_ P(EXP);
                    }
                    LOOP_ASSERT(index, EXP == rv);
                }
            }
            for (int index = 0; index < 64; ++index) {
                const uint64_t value64 = 0;
                {
                    const uint64_t EXP = 0;
                    uint64_t rv = Util::withBitCleared(value64, index);
                    if (veryVerbose) {
                        T_ P_(value64); T_ P_(index); T_ P_(rv); T_ P(EXP);
                    }
                    LOOP_ASSERT(index, EXP == rv);
                }
                {
                    const uint64_t EXP = static_cast<uint64_t>(1) << index;
                    uint64_t rv = Util::withBitSet(value64, index);
                    if (veryVerbose) {
                        T_ P_(value64); T_ P_(index); T_ P_(rv); T_ P(EXP);
                    }
                    LOOP_ASSERT(index, EXP == rv);
                }
            }
        }
        { // starting from all-bits-set
            for (int index = 0; index < 32; ++index) {
                const uint32_t value32 = static_cast<uint32_t>(-1);
                {
                    const uint32_t EXP = ~(static_cast<uint32_t>(1) << index);
                    uint32_t rv = Util::withBitCleared(value32, index);
                    if (veryVerbose) {
                        T_ P_(value32); T_ P_(index); T_ P_(rv); T_ P(EXP);
                    }
                    LOOP_ASSERT(index, EXP == rv);
                }
                {
                    const uint32_t EXP = value32;
                    uint32_t rv = Util::withBitSet(value32, index);
                    if (veryVerbose) {
                        T_ P_(value32); T_ P_(index); T_ P_(rv); T_ P(EXP);
                    }
                    LOOP_ASSERT(index, EXP == rv);
                }
            }
            for (int index = 0; index < 64; ++index) {
                const uint64_t value64 = static_cast<uint64_t>(-1);
                {
                    const uint64_t EXP = ~(static_cast<uint64_t>(1) << index);
                    uint64_t rv = Util::withBitCleared(value64, index);
                    if (veryVerbose) {
                        T_ P_(value64); T_ P_(index); T_ P_(rv); T_ P(EXP);
                    }
                    LOOP_ASSERT(index, EXP == rv);
                }
                {
                    const uint64_t EXP = value64;
                    uint64_t rv = Util::withBitSet(value64, index);
                    if (veryVerbose) {
                        T_ P_(value64); T_ P_(index); T_ P_(rv); T_ P(EXP);
                    }
                    LOOP_ASSERT(index, EXP == rv);
                }
            }
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            uint32_t value32 = 0;
            uint64_t value64 = 0;

            ASSERT_SAFE_FAIL(Util::withBitCleared(value32, -1));
            ASSERT_SAFE_PASS(Util::withBitCleared(value32, 0));
            ASSERT_SAFE_PASS(Util::withBitCleared(value32, 31));
            ASSERT_SAFE_FAIL(Util::withBitCleared(value32, 32));

            ASSERT_SAFE_FAIL(Util::withBitCleared(value64, -1));
            ASSERT_SAFE_PASS(Util::withBitCleared(value64, 0));
            ASSERT_SAFE_PASS(Util::withBitCleared(value64, 63));
            ASSERT_SAFE_FAIL(Util::withBitCleared(value64, 64));

            ASSERT_SAFE_FAIL(Util::withBitSet(value32, -1));
            ASSERT_SAFE_PASS(Util::withBitSet(value32, 0));
            ASSERT_SAFE_PASS(Util::withBitSet(value32, 31));
            ASSERT_SAFE_FAIL(Util::withBitSet(value32, 32));

            ASSERT_SAFE_FAIL(Util::withBitSet(value64, -1));
            ASSERT_SAFE_PASS(Util::withBitSet(value64, 0));
            ASSERT_SAFE_PASS(Util::withBitSet(value64, 63));
            ASSERT_SAFE_FAIL(Util::withBitSet(value64, 64));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'isBitSet'
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 'isBitSet' correctly returns the state of the specified bit.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Verify return values for depth enumerated test vectors with known
        //:   expected results.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   bool isBitSet(uint32_t value, int index);
        //   bool isBitSet(uint64_t value, int index);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'isBitSet'" << endl
                          << "==================" << endl;

        { // depth 0; no bits set
            uint32_t value32 = 0;
            for (int index = 0; index < 32; ++index) {
                const bool EXP = false;
                bool rv = Util::isBitSet(value32, index);
                if (veryVerbose) {
                    T_ P_(value32); T_ P_(index); T_ P_(rv); T_ P(EXP);
                }
                LOOP_ASSERT(index, EXP == rv);
            }
            uint64_t value64 = 0;
            for (int index = 0; index < 64; ++index) {
                const bool EXP = false;
                bool rv = Util::isBitSet(value64, index);
                if (veryVerbose) {
                    T_ P_(value64); T_ P_(index); T_ P_(rv); T_ P(EXP);
                }
                LOOP_ASSERT(index, EXP == rv);
            }
        }

        { // depth 1; one bit set
            for (int i = 0; i < 32; ++i) {
                uint32_t value32 = static_cast<uint32_t>(1) << i;
                for (int index = 0; index < 32; ++index) {
                    ASSERT((index == i) == Util::isBitSet(value32, index));
                    const bool EXP = index == i;
                    bool rv = Util::isBitSet(value32, index);
                    if (veryVerbose) {
                        T_ P_(value32); T_ P_(index); T_ P_(rv); T_ P(EXP);
                    }
                    LOOP2_ASSERT(i, index, EXP == rv);
                }
            }
            for (int i = 0; i < 64; ++i) {
                uint64_t value64 = static_cast<uint64_t>(1) << i;
                for (int index = 0; index < 64; ++index) {
                    ASSERT((index == i) == Util::isBitSet(value64, index));
                    const bool EXP = index == i;
                    bool rv = Util::isBitSet(value64, index);
                    if (veryVerbose) {
                        T_ P_(value64); T_ P_(index); T_ P_(rv); T_ P(EXP);
                    }
                    LOOP2_ASSERT(i, index, EXP == rv);
                }
            }
        }

        { // depth 2; two bits set
            for (int i = 0; i < 32; ++i) {
                for (int j = i + 1; j < 32; ++j) {
                    uint32_t value32 = ((static_cast<uint32_t>(1) << i)
                                        | (static_cast<uint32_t>(1) << j));
                    for (int index = 0; index < 32; ++index) {
                        const bool EXP = index == i || index == j;
                        bool rv = Util::isBitSet(value32, index);
                        if (veryVerbose) {
                            T_ P_(value32); T_ P_(index); T_ P_(rv); T_ P(EXP);
                        }
                        LOOP3_ASSERT(i, j, index, EXP == rv);
                    }
                }
            }
            for (int i = 0; i < 64; ++i) {
                for (int j = i + 1; j < 64; ++j) {
                    uint64_t value64 = ((static_cast<uint64_t>(1) << i)
                                        | (static_cast<uint64_t>(1) << j));
                    for (int index = 0; index < 64; ++index) {
                        const bool EXP = index == i || index == j;
                        bool rv = Util::isBitSet(value64, index);
                        if (veryVerbose) {
                            T_ P_(value64); T_ P_(index); T_ P_(rv); T_ P(EXP);
                        }
                        LOOP3_ASSERT(i, j, index, EXP == rv);
                    }
                }
            }
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            uint32_t value32 = 0;
            uint64_t value64 = 0;

            ASSERT_SAFE_FAIL(Util::isBitSet(value32, -1));
            ASSERT_SAFE_PASS(Util::isBitSet(value32, 0));
            ASSERT_SAFE_PASS(Util::isBitSet(value32, 31));
            ASSERT_SAFE_FAIL(Util::isBitSet(value32, 32));

            ASSERT_SAFE_FAIL(Util::isBitSet(value64, -1));
            ASSERT_SAFE_PASS(Util::isBitSet(value64, 0));
            ASSERT_SAFE_PASS(Util::isBitSet(value64, 63));
            ASSERT_SAFE_FAIL(Util::isBitSet(value64, 64));
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'sizeInBits'
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 'sizeInBits' correctly returns the size of variables in bits.
        //
        // Plan:
        //: 1 Verify results on types of various sizes.  (C-1)
        //
        // Testing:
        //   int sizeInBits(INTEGER value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'sizeInBits'" << endl
                          << "====================" << endl;

        BSLMF_ASSERT(4 == sizeof(int32_t) && 8 == sizeof(int64_t));
        BSLMF_ASSERT(1 == sizeof(char)    && 2 == sizeof(short));
        BSLMF_ASSERT(4 == sizeof(int)     && 8 == sizeof(Int64));

        char myChar = 0;
        unsigned char myUnsignedChar = 0;
        signed char mySignedChar = 0;

        short myShort = 0;
        unsigned short myUnsignedShort = 0;

        int32_t myExactInt32 = 0;
        uint32_t myExactUint32 = 0;

        int myInt = 0;
        unsigned myUnsigned = 0;

        long myLong = 0;
        unsigned long myUnsignedLong = 0;

        void *myPtr = 0;

        int64_t myExactInt64 = 0;
        uint64_t myExactUint64 = 0;

        Int64 myInt64 = 0;
        Uint64 myUint64 = 0;

        ASSERT(8 == Util::sizeInBits(myChar));
        ASSERT(8 == Util::sizeInBits(myUnsignedChar));
        ASSERT(8 == Util::sizeInBits(mySignedChar));

        ASSERT(16 == Util::sizeInBits(myShort));
        ASSERT(16 == Util::sizeInBits(myUnsignedShort));

        ASSERT(32 == Util::sizeInBits(myExactInt32));
        ASSERT(32 == Util::sizeInBits(myExactUint32));

        ASSERT(32 == Util::sizeInBits(myInt));
        ASSERT(32 == Util::sizeInBits(myUnsigned));

        ASSERT(sizeof(long) * CHAR_BIT == Util::sizeInBits(myLong));
        ASSERT(sizeof(unsigned long) * CHAR_BIT ==
                                             Util::sizeInBits(myUnsignedLong));

        ASSERT(sizeof(void *) * CHAR_BIT == Util::sizeInBits(myPtr));

        ASSERT(64 == Util::sizeInBits(myExactInt64));
        ASSERT(64 == Util::sizeInBits(myExactUint64));

        ASSERT(64 == Util::sizeInBits(myInt64));
        ASSERT(64 == Util::sizeInBits(myUint64));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
