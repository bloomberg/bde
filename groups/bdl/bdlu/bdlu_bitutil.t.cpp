// bdlu_bitutil.t.cpp                                                 -*-C++-*-
#include <bdlu_bitutil.h>

#include <bdls_testutil.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>


using namespace BloombergLP;
using namespace bsl;


//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// This component under test provides static methods that perform various bit
// related computations.  This test driver tests each implemented function.
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

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define Q   BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P   BDLS_TESTUTIL_P   // Print identifier and value.
#define P_  BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BDLS_TESTUTIL_L_  // current Line number

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

typedef bdlu::BitUtil        Util;

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
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
// The following usage examples illustrate how some of the methods are used.
// Note that, in all of these examples, the low-order bit is considered bit '0'
// and resides on the right edge of the bit string.
//
// First, use 'withBitSet' to demonstrate the bit ordering:
//..
    ASSERT(static_cast<uint32_t>(0x00000001)
                   == bdlu::BitUtil::withBitSet(static_cast<uint32_t>(0),  0));
    ASSERT(static_cast<uint32_t>(0x00000008)
                   == bdlu::BitUtil::withBitSet(static_cast<uint32_t>(0),  3));
    ASSERT(static_cast<uint32_t>(0x00800000)
                   == bdlu::BitUtil::withBitSet(static_cast<uint32_t>(0), 23));
    ASSERT(static_cast<uint32_t>(0x66676666)
          == bdlu::BitUtil::withBitSet(static_cast<uint32_t>(0x66666666), 16));

    /*------------------------------------------------------------------------+
    | 'bdlu::BitUtil::withBitSet(0x66666666, 16)' in binary:                  |
    |                                                                         |
    | srcInteger in binary:                  01100110011001100110011001100110 |
    | set bit 16:                                           1                 |
    | result:                                01100110011001110110011001100110 |
    +------------------------------------------------------------------------*/
//..
// Then, count the number of set bits in a value with 'numBitsSet':
//..
    ASSERT(0 == bdlu::BitUtil::numBitsSet(static_cast<uint32_t>(0x00000000)));
    ASSERT(2 == bdlu::BitUtil::numBitsSet(static_cast<uint32_t>(0x00101000)));
    ASSERT(8 == bdlu::BitUtil::numBitsSet(static_cast<uint32_t>(0x30071101)));

    /*------------------------------------------------------------------------+
    | 'bdlu::BitUtil::numBitsSet(0x30071101)' in binary:                      |
    |                                                                         |
    | input in binary:                       00110000000001110001000100000001 |
    | that has 8 ones set.  result: 8                                         |
    +------------------------------------------------------------------------*/
//..
// Finally, use 'numLeadingUnsetBits' to determine the number of unset bits
// with higher index than the first set bit:
//..
    ASSERT(32 ==
        bdlu::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x00000000)));
    ASSERT(31 ==
        bdlu::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x00000001)));
    ASSERT(7 ==
        bdlu::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x01000000)));
    ASSERT(7 ==
        bdlu::BitUtil::numLeadingUnsetBits(static_cast<uint32_t>(0x01620030)));

    /*------------------------------------------------------------------------+
    | 'bdlu::BitUtil::numLeadingUnsetBits(0x01620030)' in binary:             |
    |                                                                         |
    | input:                                 00000001011000100000000000110000 |
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
        //: 1 the methods correctly perform the calculations
        //: 2 QoI: asserted precondition violations are detected when enabled
        //
        // Plan:
        //: 1 verify return value for input values 2^i - 1, 2^i, 2^i + 1 and
        //:   all possible 'boundary' values where i = 1 .. 31 for 'uint32_t'
        //:   and i = 1 .. 63 for 'uint64_t'
        //:
        //: 2 verify return values when all bits are set and all possible
        //:   'boundary' values in the input value (C-1)
        //:
        //: 3 verify defensive checks are triggered for invalid attribute
        //:   values (C-2)
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
                    uint32_t rv = Util::roundUp(value32, boundary);
                    // NOTE: the only possible overflow scenario in the
                    // following correctly results in 0 since 'boundary' is a
                    // power of two
                    uint32_t RV = (value32 % boundary
                                   ? (value32 / boundary + 1) * boundary
                                   : value32);
                    LOOP3_ASSERT(b, i, d, rv == RV);
                }
            }
        }
        for (int b = 0; b < 64; ++b) {
            uint64_t boundary = static_cast<uint64_t>(1) << b;
            for (int i = 0; i < 64; ++i) {
                for (int d = -1; d <= 1; ++d) {
                    uint64_t value64 = (static_cast<uint64_t>(1) << i) + d;
                    uint64_t rv = Util::roundUp(value64, boundary);
                    // NOTE: the only possible overflow scenario in the
                    // following correctly results in 0 since 'boundary' is a
                    // power of two
                    uint64_t RV = (value64 % boundary
                                   ? (value64 / boundary + 1) * boundary
                                   : value64);
                    LOOP3_ASSERT(b, i, d, rv == RV);
                }
            }
        }

        { // verify all bits set
            uint32_t value32 = static_cast<uint32_t>(-1);
            ASSERT(value32 == Util::roundUp(value32,
                                            static_cast<uint32_t>(1)));
            for (int b = 1; b < 32; ++b) {
                uint32_t boundary = static_cast<uint32_t>(1) << b;
                LOOP_ASSERT(b, 0 == Util::roundUp(value32, boundary));
            }
            uint64_t value64 = static_cast<uint64_t>(-1);
            ASSERT(value64 == Util::roundUp(value64,
                                            static_cast<uint64_t>(1)));
            for (int b = 1; b < 64; ++b) {
                uint64_t boundary = static_cast<uint64_t>(1) << b;
                LOOP_ASSERT(b, 0 == Util::roundUp(value64, boundary));
            }
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            // '0 == boundary'
            ASSERT_SAFE_FAIL(Util::roundUp(static_cast<uint32_t>(0),
                                           static_cast<uint32_t>(0)));
            ASSERT_SAFE_FAIL(Util::roundUp(static_cast<uint64_t>(0),
                                           static_cast<uint64_t>(0)));

            // more than one bit in 'boundary'
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
        //: 1 the methods correctly perform the calculations
        //: 2 QoI: asserted precondition violations are detected when enabled
        //
        // Plan:
        //: 1 verify return value for input values 1 and 2
        //:
        //: 2 verify return value for input values 2^i - 1, 2^i, 2^i + 1 where
        //:   i = 2 .. 30 for 'uint32_t' and i = 2 .. 62 for 'uint64_t'
        //:
        //: 3 for 'roundUpToBinaryPower', verify behavior at input value 0
        //:
        //: 4 verify return value for input values 2^x - 1, 2^x, 2^x + 1 where
        //:   x = 31 for 'uint32_t' and x = 63 for 'uint64_t'
        //:
        //: 5 verify return values when all bits are set in the input value
        //:   (C-1)
        //:
        //: 6 verify defensive checks are triggered for invalid attribute
        //:   values (C-2)
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
                uint32_t value32 = static_cast<uint32_t>(1) << i;
                LOOP_ASSERT(i, i == Util::log2(value32 - 1));
                LOOP_ASSERT(i, i == Util::log2(value32));
                LOOP_ASSERT(i, i + 1 == Util::log2(value32 + 1));
                LOOP_ASSERT(i, value32
                                   == Util::roundUpToBinaryPower(value32 - 1));
                LOOP_ASSERT(i, value32 == Util::roundUpToBinaryPower(value32));
                LOOP_ASSERT(i, value32 * 2
                                   == Util::roundUpToBinaryPower(value32 + 1));
            }
            for (int i = 2; i < 63; ++i) {
                // NOTE: 2^2 - 1 == 3
                uint64_t value64 = static_cast<uint64_t>(1) << i;
                LOOP_ASSERT(i, i == Util::log2(value64 - 1));
                LOOP_ASSERT(i, i == Util::log2(value64));
                LOOP_ASSERT(i, i + 1 == Util::log2(value64 + 1));
                LOOP_ASSERT(i, value64
                                   == Util::roundUpToBinaryPower(value64 - 1));
                LOOP_ASSERT(i, value64 == Util::roundUpToBinaryPower(value64));
                LOOP_ASSERT(i, value64 * 2
                                   == Util::roundUpToBinaryPower(value64 + 1));
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

            ASSERT(31 == Util::log2(value32 - 1));
            ASSERT(63 == Util::log2(value64 - 1));
            ASSERT(value32 == Util::roundUpToBinaryPower(value32 - 1));
            ASSERT(value64 == Util::roundUpToBinaryPower(value64 - 1));

            ASSERT(31 == Util::log2(value32));
            ASSERT(63 == Util::log2(value64));
            ASSERT(value32 == Util::roundUpToBinaryPower(value32));
            ASSERT(value64 == Util::roundUpToBinaryPower(value64));

            ASSERT(32 == Util::log2(value32 + 1));
            ASSERT(64 == Util::log2(value64 + 1));
            ASSERT(0 == Util::roundUpToBinaryPower(value32 + 1));
            ASSERT(0 == Util::roundUpToBinaryPower(value64 + 1));
        }

        { // verify all bits set
            uint32_t value32 = static_cast<uint32_t>(-1);
            uint64_t value64 = static_cast<uint64_t>(-1);
            ASSERT(32 == Util::log2(value32));
            ASSERT(64 == Util::log2(value64));
            ASSERT(0 == Util::roundUpToBinaryPower(value32));
            ASSERT(0 == Util::roundUpToBinaryPower(value64));
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            uint32_t value32 = 0;  (void)value32;
            uint64_t value64 = 0;  (void)value64;

            ASSERT_SAFE_FAIL(Util::log2(value32));
            ASSERT_SAFE_FAIL(Util::log2(value64));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'num(*)UnsetBits'
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 methods correctly return the number of unset bits prior to a set
        //:   bit for the specified direction
        //
        // Plan:
        //: 1 verify return values for depth enumerated test vectors with known
        //:   expected result
        //:
        //: 2 verify result for case where all bits are set (C-1)
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
            cout << "'bdlu::numLeadingUnsetBits(value)'"
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
                        LOOP2_ASSERT(value32,
                                     Util::numLeadingUnsetBits(value32),
                                     31 - i
                                        == Util::numLeadingUnsetBits(value32));
                    }
                }
            }
            for (int i = 0; i < 64; ++i) {
                for (int j = 0; j <= i; ++j) {
                    for (int k = 0; k <= j; ++k) {
                        uint64_t value64 = ((static_cast<uint64_t>(1) << i)
                                            | (static_cast<uint64_t>(1) << j)
                                            | (static_cast<uint64_t>(1) << k));
                        LOOP2_ASSERT(value64,
                                     Util::numLeadingUnsetBits(value64),
                                     63 - i
                                        == Util::numLeadingUnsetBits(value64));
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
            cout << "'bdlu::numTrailingUnsetBits(value)'"
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
                        LOOP2_ASSERT(value32,
                                     Util::numTrailingUnsetBits(value32),
                                     i == Util::numTrailingUnsetBits(value32));
                    }
                }
            }
            for (int i = 0; i < 64; ++i) {
                for (int j = i; j < 64; ++j) {
                    for (int k = j; k < 64; ++k) {
                        uint64_t value64 = ((static_cast<uint64_t>(1) << i)
                                            | (static_cast<uint64_t>(1) << j)
                                            | (static_cast<uint64_t>(1) << k));
                        LOOP2_ASSERT(value64,
                                     Util::numTrailingUnsetBits(value64),
                                     i == Util::numTrailingUnsetBits(value64));
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
        //: 1 'numBitsSet' correctly returns the number of set bits
        //
        // Plan:
        //: 1 verify return values for depth enumerated test vectors with known
        //:   expected result
        //:
        //: 2 verify result for case where all bits are set (C-1)
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
                ASSERT(1 == Util::numBitsSet(value32));
            }
            for (int i = 0; i < 64; ++i) {
                uint64_t value64 = static_cast<uint64_t>(1) << i;
                ASSERT(1 == Util::numBitsSet(value64));
            }
        }

        { // depth 2; two bits set
            for (int i = 0; i < 32; ++i) {
                for (int j = i + 1; j < 32; ++j) {
                    uint32_t value32 = ((static_cast<uint32_t>(1) << i)
                                        | (static_cast<uint32_t>(1) << j));
                    ASSERT(2 == Util::numBitsSet(value32));
                }
            }
            for (int i = 0; i < 64; ++i) {
                for (int j = i + 1; j < 64; ++j) {
                    uint64_t value64 = ((static_cast<uint64_t>(1) << i)
                                        | (static_cast<uint64_t>(1) << j));
                    ASSERT(2 == Util::numBitsSet(value64));
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
        //: 1 the methods correctly adjust the bit in the specified index
        //: 2 the methods do not adjust the bits in other index positions
        //: 3 the methods work for all index positions
        //: 4 QoI: asserted precondition violations are detected when enabled
        //
        // Plan:
        //: 1 starting with the values 0 and all-bits-set, for every index
        //:   perform both operations and verify the result values (C-1..3)
        //:
        //: 2 verify defensive checks are triggered for invalid attribute
        //:   values (C-4)
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
                uint32_t value32 = 0;
                ASSERT(0 == Util::withBitCleared(value32, index));
                ASSERT((static_cast<uint32_t>(1) << index)
                                          == Util::withBitSet(value32, index));
            }
            for (int index = 0; index < 64; ++index) {
                uint64_t value64 = 0;
                ASSERT(0 == Util::withBitCleared(value64, index));
                ASSERT((static_cast<uint64_t>(1) << index)
                                          == Util::withBitSet(value64, index));
            }
        }
        { // starting from all-bits-set
            for (int index = 0; index < 32; ++index) {
                uint32_t value32 = static_cast<uint32_t>(-1);
                ASSERT(~(static_cast<uint32_t>(1) << index)
                                      == Util::withBitCleared(value32, index));
                ASSERT(value32 == Util::withBitSet(value32, index));
            }
            for (int index = 0; index < 64; ++index) {
                uint64_t value64 = static_cast<uint64_t>(-1);
                ASSERT(~(static_cast<uint64_t>(1) << index)
                                      == Util::withBitCleared(value64, index));
                ASSERT(value64 == Util::withBitSet(value64, index));
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
        //: 1 'isBitSet' correctly returns the state of the specified bit
        //: 2 QoI: asserted precondition violations are detected when enabled
        //
        // Plan:
        //: 1 verify return values for depth enumerated test vectors with known
        //:   expected result (C-1)
        //:
        //: 2 verify defensive checks are triggered for invalid attribute
        //:   values (C-2)
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
                ASSERT(false == Util::isBitSet(value32, index));
            }
            uint64_t value64 = 0;
            for (int index = 0; index < 64; ++index) {
                ASSERT(false == Util::isBitSet(value64, index));
            }
        }

        { // depth 1; one bit set
            for (int i = 0; i < 32; ++i) {
                uint32_t value32 = static_cast<uint32_t>(1) << i;
                for (int index = 0; index < 32; ++index) {
                    ASSERT((index == i) == Util::isBitSet(value32, index));
                }
            }
            for (int i = 0; i < 64; ++i) {
                uint64_t value64 = static_cast<uint64_t>(1) << i;
                for (int index = 0; index < 64; ++index) {
                    ASSERT((index == i) == Util::isBitSet(value64, index));
                }
            }
        }

        { // depth 2; two bits set
            for (int i = 0; i < 32; ++i) {
                for (int j = i + 1; j < 32; ++j) {
                    uint32_t value32 = ((static_cast<uint32_t>(1) << i)
                                        | (static_cast<uint32_t>(1) << j));
                    for (int index = 0; index < 32; ++index) {
                        ASSERT((index == i || index == j)
                                            == Util::isBitSet(value32, index));
                    }
                }
            }
            for (int i = 0; i < 64; ++i) {
                for (int j = i + 1; j < 64; ++j) {
                    uint64_t value64 = ((static_cast<uint64_t>(1) << i)
                                        | (static_cast<uint64_t>(1) << j));
                    for (int index = 0; index < 64; ++index) {
                        ASSERT((index == i || index == j)
                                            == Util::isBitSet(value64, index));
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
        //: 1 'sizeInBits' correctly returns the size of variables in bits
        //
        // Plan:
        //: 1 verify results on various sized types (C-1)
        //
        // Testing:
        //   int sizeInBits(INTEGER value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'sizeInBits'" << endl
                          << "====================" << endl;

        BSLMF_ASSERT(4 == sizeof(int32_t) && 8 == sizeof(int64_t));
        BSLMF_ASSERT(1 == sizeof(char) && 2 == sizeof(short));
        BSLMF_ASSERT(4 == sizeof(int) && 8 == sizeof(Int64));

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
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
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
