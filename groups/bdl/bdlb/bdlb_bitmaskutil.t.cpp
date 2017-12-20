// bdlb_bitmaskutil.t.cpp                                             -*-C++-*-

#include <bdlb_bitmaskutil.h>

#include <bslim_testutil.h>

#include <bslmf_assert.h>
#include <bsls_asserttest.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>

#include <bsl_c_limits.h>    // 'INT_MIN', 'INT_MAX'
#include <bsl_cstdlib.h>     // 'atoi'

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// Case 2 thoroughly tests all 12 one-argument functions, and case 3 thoroughly
// tests all 4 two-argument functions.  Since the valid range of any argument
// of input to the functions is very limited, it is possible to do exhaustive
// testing of all 16 functions in the component.
//-----------------------------------------------------------------------------
// [ 1] k_BITS_PER_UINT32
// [ 1] k_BITS_PER_UINT64
// [ 2] uint32_t eq(int index);
// [ 2] uint64_t eq64(int index);
// [ 2] uint32_t ge(int index);
// [ 2] uint64_t ge64(int index);
// [ 2] uint32_t gt(int index);
// [ 2] uint64_t gt64(int index);
// [ 2] uint32_t le(int index);
// [ 2] uint64_t le64(int index);
// [ 2] uint32_t lt(int index);
// [ 2] uint64_t lt64(int index);
// [ 2] uint32_t ne(int index);
// [ 2] uint64_t ne64(int index);
// [ 3] uint32_t zero(int index, int numBits);
// [ 3] uint64_t zero64(int index, int numBits);
// [ 3] uint32_t one(int index, int numBits);
// [ 3] uint64_t one64(int index, int numBits);
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlb::BitMaskUtil Util;

typedef bsl::uint32_t uint32_t;
typedef bsl::uint64_t uint64_t;

const static uint64_t int64Max  = ~0ULL / 2;
const static uint64_t int64Min  = 1ULL << 63;
const static uint64_t zero64    = 0;
const static uint64_t one64     = 1;
const static uint64_t two64     = 2;

const static uint32_t zero      = 0;
const static uint32_t one       = 1;
const static uint32_t two       = 2;

enum { BITS_PER_WORD   = 8 * sizeof(uint32_t), BPW = BITS_PER_WORD   };
enum { BITS_PER_UINT64 = 8 * sizeof(uint64_t), BPS = BITS_PER_UINT64 };

#define CAT4(A, B, C, D)  CAT(CAT(A, B), CAT(C, D))
#define CAT(X, Y) CAT_IMP(X, Y)
#define CAT_IMP(X, Y) X##Y

// Hex Bit Strings
#define x0_ "0000"
#define x5_ "0101"
#define xA_ "1010"
#define xF_ "1111"

// Longer Bit Strings
#define B8_0 x0_ x0_
#define B8_1 xF_ xF_
#define B8_01 x5_ x5_
#define B8_10 xA_ xA_
#define B12_0 B8_0 x0_
#define B12_1 B8_1 xF_
#define B16_0 B8_0 B8_0
#define B16_1 B8_1 B8_1
#define B16_01 B8_01 B8_01
#define B16_10 B8_10 B8_10
#define B32_0 B16_0 B16_0
#define B32_1 B16_1 B16_1
#define B32_01 B16_01 B16_01
#define B32_10 B16_10 B16_10

// Platform-Dependent Bit Strings
BSLMF_ASSERT(static_cast<bsls::Types::Int64>(int64Min - 1) >
                                    static_cast<bsls::Types::Int64>(int64Min));

#define EW_0 x0_
#define EW_1 xF_
#define FW_01 B32_01
#define FW_10 B32_10
#define SW_01 B32_01 B32_01
#define SW_10 B32_10 B32_10
#define MID_0   " 00000000 00000000 "
#define MID_1   " 11111111 11111111 "
#define MID_01  " 01010101 01010101 "
#define MID_10  " 10101010 10101010 "

// Word Relative Bit Strings
#define QW_0 EW_0 EW_0
#define QW_1 EW_1 EW_1
#define HW_0 QW_0 QW_0
#define HW_1 QW_1 QW_1
#define FW_0 HW_0 HW_0
#define FW_1 HW_1 HW_1
#define SW_0 FW_0 FW_0
#define SW_1 FW_1 FW_1

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
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

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creation of Simple Bit Masks
///- - - - - - - - - - - - - - - - - - - -
// The following usage examples illustrate how some of the methods provided by
// this component are used.  Note that, in all of these examples, the low-order
// bit is considered bit 0 and resides on the right edge of the bit string.
//
// First, the 'ge' function takes a single argument, 'index', and returns a bit
// mask with all bits below the specified 'index' cleared and all bits at or
// above the 'index' set:
//..
//  +-------------------------------------------------------------------------+
//  | 'bdlb::BitMaskUtil::ge(16)' in binary:                                  |
//  |                                                                         |
//  | 'index': bit 16:                                      *                 |
//  | All bits at and above bit 16 are set:  11111111111111110000000000000000 |
//  +-------------------------------------------------------------------------+
//
    const uint32_t expGe = 0xffff0000;
    ASSERT(expGe == bdlb::BitMaskUtil::ge(16));
//..
// Next, the 'lt' function returns a bit mask with all bits at or above the
// specified 'index' cleared, and all bits below 'index' set.  'lt' and 'ge'
// return the complement of each other if passed the same 'index':
//..
//  +-------------------------------------------------------------------------+
//  | 'bdlb::BitMaskUtil::lt(16)' in binary:                                  |
//  |                                                                         |
//  | 'index': bit 16:                                      *                 |
//  | All bits below bit 16 are set:         00000000000000001111111111111111 |
//  +-------------------------------------------------------------------------+
//
    const uint32_t expLt = 0x0000ffff;
    ASSERT(expLt == bdlb::BitMaskUtil::lt(16));

    ASSERT(expGe == ~expLt);
//..
// Then, the 'eq' function returns a bit mask with only the bit at the
// specified 'index' set:
//..
//  +-------------------------------------------------------------------------+
//  | 'bdlb::BitMaskUtil::eq(23)' in binary:                                  |
//  |                                                                         |
//  | 'index': bit 23:                               *                        |
//  | Only bit 23 is set:                    00000000100000000000000000000000 |
//  +-------------------------------------------------------------------------+
//
    const uint32_t expEq = 0x00800000;
    ASSERT(expEq == bdlb::BitMaskUtil::eq(23));
//..
// Now, the 'ne' function returns a bit mask with only the bit at the specified
// 'index' cleared.  'ne' and 'eq' return the complement of each other for a
// given 'index':
//..
//  +-------------------------------------------------------------------------+
//  | 'bdlb::BitMaskUtil::ne(23)' in binary:                                  |
//  |                                                                         |
//  | 'index': bit 23:                               *                        |
//  | All bits other than bit 16 are set:    11111111011111111111111111111111 |
//  +-------------------------------------------------------------------------+
//
    const uint32_t expNe = 0xff7fffff;
    ASSERT(expNe == bdlb::BitMaskUtil::ne(23));

    ASSERT(expEq == ~expNe);
//..
// Finally, 'one' and 'zero' return a bit mask with all bits within a specified
// range starting from a specified 'index' either set or cleared, respectively.
// For the same arguments, 'one' and 'zero' return the complement of each
// other:
//..
//  +-------------------------------------------------------------------------+
//  | 'bdlb::BitMaskUtil::one(16, 4)' in binary:                              |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | 4 bits starting at bit 16:                         ****                 |
//  | Result: only those bits set:           00000000000011110000000000000000 |
//  +-------------------------------------------------------------------------+
//
    const uint32_t expOne = 0x000f0000;
    ASSERT(expOne == bdlb::BitMaskUtil::one(16, 4));
//
//  +-------------------------------------------------------------------------+
//  | 'bdlb::BitMaskUtil::zero(16, 4)' in binary:                             |
//  |                                                                         |
//  | bit 16:                                               *                 |
//  | 4 bits starting at bit 16:                         ****                 |
//  | Result: only those bits cleared:       11111111111100001111111111111111 |
//  +-------------------------------------------------------------------------+
//
    const uint32_t expZero = 0xfff0ffff;
    ASSERT(expZero == bdlb::BitMaskUtil::zero(16, 4));

    ASSERT(expZero == ~expOne);
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ONE AND ZERO FUNCTIONS
        //   Ensure the methods return the expected value.
        //
        // Concerns:
        //: 1 The functions all return the correct result.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Do table-driven testing.
        //:
        //: 2 Do exhaustive testing, calculating the expected result through
        //:   different algorithms than the functions use.  (C-1)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   uint32_t one(int index, int numBits);
        //   uint64_t one64(int index, int numBits);
        //   uint32_t zero(int index, int numBits);
        //   uint64_t zero64(int index, int numBits);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ONE AND ZERO FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout << "Table-driven 32-bit testing\n";
        static const struct {
            int      d_lineNum;     // line number
            int      d_index;       // index
            int      d_numBits;     // number of bits
            uint32_t d_zero;        // expected result from 'zero' function
            uint32_t d_one;         // expected result from 'one' function
        } DATA_A[] = {
          //LINE Idx  NumBits  zero result      one result
          //---- ---  -------  -----------      ----------
          { L_,    0,    0,          ~zero,           zero }, // index 0
          { L_,    0,    1,           ~one,            one },
          { L_,    0,BPW-1, (uint32_t)INT_MIN, (uint32_t) INT_MAX },
          { L_,    0,  BPW,           zero,          ~zero },

          { L_,    1,    0,          ~zero,           zero }, // index 1
          { L_,    1,    1,           ~two,            two },
          { L_,    1,BPW-1,            one,           ~one },

          { L_,BPW-1,    0,          ~zero,           zero }, // BPW-1
          { L_,BPW-1,    1, (uint32_t) INT_MAX, (uint32_t) INT_MIN },

          { L_,  BPW,    0,          ~zero,           zero }, // BPW

          { L_,    2,    4, (uint32_t) ~(0xf << 2),
                                      (uint32_t) 0xf << 2  }, // typical case
          { L_,BPW-5,    3, (uint32_t) ~(7 << (BPW-5)),
                                   (uint32_t) 7 << (BPW-5) }
        };
        enum { NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A };

        for (int di = 0; di < NUM_DATA_A; ++di) {
            const int LINE     = DATA_A[di].d_lineNum;
            const int INDEX    = DATA_A[di].d_index;
            const int NUM_BITS = DATA_A[di].d_numBits;

            uint32_t resA = Util::zero(INDEX, NUM_BITS);
            if (veryVerbose) {
                P_(LINE);
                P_(INDEX);
                P_(NUM_BITS);
                P_(DATA_A[di].d_zero);
                P(resA);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_zero == resA);

            uint32_t resB = Util::one(INDEX, NUM_BITS);
            if (veryVerbose) {
                P_(LINE);
                P_(INDEX);
                P_(NUM_BITS);
                P_(DATA_A[di].d_one);
                P(resB);
            }
            LOOP_ASSERT(LINE, DATA_A[di].d_one == resB);

            ASSERT(~resB == resA);
        }

        if (verbose) cout << "Table-driven 64-bit testing\n";

        const uint64_t f64 = 0xf, seven64 = 7;

        static const struct {
            int      d_lineNum;     // line number
            int      d_index;       // index
            int      d_numBits;     // number of bits
            uint64_t d_zero;        // expected result from 'zero' function
            uint64_t d_one;         // expected result from 'one' function
        } DATA_B[] = {
          //LINE Idx NumBits        zero result      one result
          //---- --- -------        -----------      ----------
          { L_,    0,      0,           ~zero64,              0 }, // index 0
          { L_,    0,      1,            ~one64,              1 },
          { L_,    0,  BPS-1,          int64Min,       int64Max },
          { L_,    0,    BPS,                 0,        ~zero64 },

          { L_,    1,      0,           ~zero64,              0 }, // index 1
          { L_,    1,      1,            ~two64,              2 },
          { L_,    1,  BPS-1,                 1,         ~one64 },

          { L_,BPS-1,      0,           ~zero64,              0 }, // BPS-1
          { L_,BPS-1,      1,          int64Max,       int64Min },

          { L_,BPS,        0,           ~zero64,              0 }, // BPW

          { L_,    2,      4,       ~(f64 << 2),       f64 << 2 },
          { L_,BPS-5,      3, ~(seven64 << (BPS-5)),
                                             seven64 << (BPS-5) }
        };
        enum { NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B };

        for (int di = 0; di < NUM_DATA_B; ++di) {
            const int LINE     = DATA_B[di].d_lineNum;
            const int INDEX    = DATA_B[di].d_index;
            const int NUM_BITS = DATA_B[di].d_numBits;

            uint64_t resA = Util::zero64(INDEX, NUM_BITS);
            if (veryVerbose) {
                P_(LINE);
                P_(INDEX);
                P_(NUM_BITS);
                P_(DATA_B[di].d_zero);
                P(resA);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_zero == resA);

            uint64_t resB = Util::one64(INDEX, NUM_BITS);
            if (veryVerbose) {
                P_(LINE);
                P_(INDEX);
                P_(NUM_BITS);
                P_(DATA_B[di].d_one);
                P(resB);
            }
            LOOP_ASSERT(LINE, DATA_B[di].d_one == resB);

            ASSERT(~resB == resA);
        }

        if (verbose) cout << "Exhaustive 32-bit testing\n";
        for (int iNB = 0; iNB <= BPW; ++iNB) {
            uint32_t nbBits = Util::lt(iNB);
            for (int iIdx = 0; iIdx <= BPW - iNB; ++iIdx) {
                ASSERTV(iNB, iIdx, (nbBits << iIdx) == Util::one(iIdx, iNB));
                ASSERTV(iNB, iIdx, (nbBits << iIdx) == ~Util::zero(iIdx, iNB));
            }
        }

        if (verbose) cout << "Exhaustive 64-bit testing\n";
        for (int iNB = 0; iNB <= BPS; ++iNB) {
            uint64_t nbBits = Util::lt64(iNB);
            for (int iIdx = 0; iIdx <= BPS - iNB; ++iIdx) {
                ASSERTV(iNB, iIdx, (nbBits << iIdx) == Util::one64(iIdx, iNB));
                ASSERTV(iNB, iIdx, (nbBits << iIdx) ==
                                                     ~Util::zero64(iIdx, iNB));
            }
        }

        if (verbose) cout << "Negative testing\n";
        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_SAFE_PASS(Util::one(              0, 0));
            ASSERT_SAFE_PASS(Util::one(  BITS_PER_WORD, 0));
            ASSERT_SAFE_PASS(Util::one(              0, BITS_PER_WORD));
            ASSERT_SAFE_FAIL(Util::one(             -1, 0));
            ASSERT_SAFE_FAIL(Util::one(        INT_MIN, 0));
            ASSERT_SAFE_FAIL(Util::one(BITS_PER_WORD+1, 0));
            ASSERT_SAFE_FAIL(Util::one(              0, -1));
            ASSERT_SAFE_FAIL(Util::one(              1, BITS_PER_WORD));
            ASSERT_SAFE_FAIL(Util::one(              0, BITS_PER_WORD + 1));
            ASSERT_SAFE_FAIL(Util::one(              0, INT_MIN));

            ASSERT_SAFE_PASS(Util::zero(              0, 0));
            ASSERT_SAFE_PASS(Util::zero(  BITS_PER_WORD, 0));
            ASSERT_SAFE_PASS(Util::zero(              0, BITS_PER_WORD));
            ASSERT_SAFE_FAIL(Util::zero(             -1, 0));
            ASSERT_SAFE_FAIL(Util::zero(        INT_MIN, 0));
            ASSERT_SAFE_FAIL(Util::zero(BITS_PER_WORD+1, 0));
            ASSERT_SAFE_FAIL(Util::zero(              0, -1));
            ASSERT_SAFE_FAIL(Util::zero(              1, BITS_PER_WORD));
            ASSERT_SAFE_FAIL(Util::zero(              0, BITS_PER_WORD + 1));
            ASSERT_SAFE_FAIL(Util::zero(              0, INT_MIN));

            const int plusOne = BITS_PER_UINT64 + 1;

            ASSERT_SAFE_PASS(Util::one64(              0, 0));
            ASSERT_SAFE_PASS(Util::one64(BITS_PER_UINT64, 0));
            ASSERT_SAFE_PASS(Util::one64(              0, BITS_PER_UINT64));
            ASSERT_SAFE_FAIL(Util::one64(             -1, 0));
            ASSERT_SAFE_FAIL(Util::one64(        INT_MIN, 0));
            ASSERT_SAFE_FAIL(Util::one64(        plusOne, 0));
            ASSERT_SAFE_FAIL(Util::one64(              0, -1));
            ASSERT_SAFE_FAIL(Util::one64(              1, BITS_PER_UINT64));
            ASSERT_SAFE_FAIL(Util::one64(              0, plusOne));
            ASSERT_SAFE_FAIL(Util::one64(              0, INT_MIN));

            ASSERT_SAFE_PASS(Util::zero64(              0, 0));
            ASSERT_SAFE_PASS(Util::zero64(BITS_PER_UINT64, 0));
            ASSERT_SAFE_PASS(Util::zero64(              0, BITS_PER_UINT64));
            ASSERT_SAFE_FAIL(Util::zero64(             -1, 0));
            ASSERT_SAFE_FAIL(Util::zero64(        INT_MIN, 0));
            ASSERT_SAFE_FAIL(Util::zero64(        plusOne, 0));
            ASSERT_SAFE_FAIL(Util::zero64(              0, -1));
            ASSERT_SAFE_FAIL(Util::zero64(              1, BITS_PER_UINT64));
            ASSERT_SAFE_FAIL(Util::zero64(              0, plusOne));
            ASSERT_SAFE_FAIL(Util::zero64(              0, INT_MIN));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING ONE ARG MASK GENERATION FUNCTIONS
        //
        // Concerns:
        //: 1 All 12 single-argument mask functions return the correct result.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Do table-driven testing.  Note that since the functions come in
        //:   pairs whose results are compliments of each other, test one pair
        //:   per table.
        //:
        //: 2 Do exhaustive testing, exhausting all input in the range
        //:   '[0 .. wordSize]', comparing the result with an expected value
        //:   calculated by a means other than that employed by the function.
        //:
        //: 3 Do exhaustive testing verifying that functions expected to return
        //:   the complement of each other actually do.  (C-1)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   uint32_t eq(int index);
        //   uint64_t eq64(int index);
        //   uint32_t ne(int index);
        //   uint64_t ne64(int index);
        //   uint32_t ge(int index);
        //   uint64_t ge64(int index);
        //   uint32_t gt(int index);
        //   uint64_t gt64(int index);
        //   uint32_t le(int index);
        //   uint64_t le64(int index);
        //   uint32_t lt(int index);
        //   uint64_t lt64(int index);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ONE ARG MASK GENERATION FUNCTIONS\n"
                          << "=========================================\n";

        {
            if (verbose) cout << "Table-driven test of 'eq' and 'ne'\n";

            struct {
                int      d_line;
                int      d_index;
                uint32_t d_expectedEq;
            } DATA[] = {
                { L_, 0,     1 },
                { L_, 1,     2 },
                { L_, 2,     4 },
                { L_, 16,    CAT(0x1,    0000) },
                { L_, BPW-1, CAT(0x8000, 0000) },
                { L_, BPW,   0 },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int      LINE   = DATA[ti].d_line;
                const int      INDEX  = DATA[ti].d_index;
                const uint32_t EXP_EQ = DATA[ti].d_expectedEq;
                const uint32_t EXP_NE = ~EXP_EQ;

                const uint32_t eq = Util::eq(INDEX);
                const uint32_t ne = Util::ne(INDEX);
                ASSERTV(LINE, INDEX, eq, EXP_EQ == eq);
                ASSERTV(LINE, INDEX, ne, EXP_NE == ne);
            }
        }

        {
            if (verbose) cout << "Table-driven test of 'ge' and 'lt'\n";

            struct {
                int      d_line;
                int      d_index;
                uint32_t d_expectedGe;
            } DATA[] = {
                { L_, 0,     CAT(0xffff, ffff) },
                { L_, 1,     CAT(0xffff, fffe) },
                { L_, 2,     CAT(0xffff, fffc) },
                { L_, 16,    CAT(0xffff, 0000) },
                { L_, BPW-1, CAT(0x8000, 0000) },
                { L_, BPW,   0 },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int      LINE   = DATA[ti].d_line;
                const int      INDEX  = DATA[ti].d_index;
                const uint32_t EXP_GE = DATA[ti].d_expectedGe;
                const uint32_t EXP_LT = ~EXP_GE;

                const uint32_t ge = Util::ge(INDEX);
                const uint32_t lt = Util::lt(INDEX);
                ASSERTV(LINE, INDEX, ge, EXP_GE == ge);
                ASSERTV(LINE, INDEX, lt, EXP_LT == lt);
            }
        }

        {
            if (verbose) cout << "Table-driven test of 'gt' and 'le'\n";

            struct {
                int      d_line;
                int      d_index;
                uint32_t d_expectedGt;
            } DATA[] = {
                { L_, 0,     CAT(0xffff, fffe) },
                { L_, 1,     CAT(0xffff, fffc) },
                { L_, 2,     CAT(0xffff, fff8) },
                { L_, 16,    CAT(0xfffe, 0000) },
                { L_, BPW-1, CAT(0x0000, 0000) },
                { L_, BPW,   0 },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int      LINE   = DATA[ti].d_line;
                const int      INDEX  = DATA[ti].d_index;
                const uint32_t EXP_GT = DATA[ti].d_expectedGt;
                const uint32_t EXP_LE = ~EXP_GT;

                const uint32_t gt = Util::gt(INDEX);
                const uint32_t le = Util::le(INDEX);
                ASSERTV(LINE, INDEX, gt, EXP_GT == gt);
                ASSERTV(LINE, INDEX, le, EXP_LE == le);

                if (INDEX < BPW) {
                    ASSERTV(LINE, INDEX, le == (Util::lt(INDEX+1)));
                    if (INDEX > 0) {
                        ASSERTV(LINE, INDEX, gt == Util::ge(INDEX+1));
                    }
                }
            }
        }

        {
            if (verbose) cout << "Table-driven test of 'eq64' and 'ne64'\n";

            struct {
                int      d_line;
                int      d_index;
                uint64_t d_expectedEq;
            } DATA[] = {
                { L_, 0,     1 },
                { L_, 1,     2 },
                { L_, 2,     4 },
                { L_, 16,    CAT4(0x0,       0, 1,    0000ULL) },
                { L_, 32,    CAT4(0x0,       1, 0000, 0000ULL) },
                { L_, 48,    CAT4(0x1,    0000, 0000, 0000ULL) },
                { L_, BPS-1, CAT4(0x8000, 0000, 0000, 0000ULL) },
                { L_, BPS,   0 },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int      LINE   = DATA[ti].d_line;
                const int      INDEX  = DATA[ti].d_index;
                const uint64_t EXP_EQ = DATA[ti].d_expectedEq;
                const uint64_t EXP_NE = ~EXP_EQ;

                const uint64_t eq = Util::eq64(INDEX);
                const uint64_t ne = Util::ne64(INDEX);
                ASSERTV(LINE, INDEX, eq, EXP_EQ == eq);
                ASSERTV(LINE, INDEX, ne, EXP_NE == ne);
            }
        }

        {
            if (verbose) cout << "Table-driven test of 'ge64' and 'lt64'\n";

            struct {
                int      d_line;
                int      d_index;
                uint64_t d_expectedGe;
            } DATA[] = {
                { L_, 0,     CAT4(0xffff, ffff, ffff, ffffULL) },
                { L_, 1,     CAT4(0xffff, ffff, ffff, fffeULL) },
                { L_, 2,     CAT4(0xffff, ffff, ffff, fffcULL) },
                { L_, 16,    CAT4(0xffff, ffff, ffff, 0000ULL) },
                { L_, 32,    CAT4(0xffff, ffff, 0000, 0000ULL) },
                { L_, 48,    CAT4(0xffff, 0000, 0000, 0000ULL) },
                { L_, BPS-1, CAT4(0x8000, 0000, 0000, 0000ULL) },
                { L_, BPS,   0 },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int      LINE   = DATA[ti].d_line;
                const int      INDEX  = DATA[ti].d_index;
                const uint64_t EXP_GE = DATA[ti].d_expectedGe;
                const uint64_t EXP_LT = ~EXP_GE;

                const uint64_t ge = Util::ge64(INDEX);
                const uint64_t lt = Util::lt64(INDEX);
                ASSERTV(LINE, INDEX, ge, EXP_GE == ge);
                ASSERTV(LINE, INDEX, lt, EXP_LT == lt);
            }
        }

        {
            if (verbose) cout << "Table-driven test of 'gt64' and 'le64'\n";

            struct {
                int      d_line;
                int      d_index;
                uint64_t d_expectedGt;
            } DATA[] = {
                { L_, 0,     CAT4(0xffff, ffff, ffff, fffeULL) },
                { L_, 1,     CAT4(0xffff, ffff, ffff, fffcULL) },
                { L_, 2,     CAT4(0xffff, ffff, ffff, fff8ULL) },
                { L_, 16,    CAT4(0xffff, ffff, fffe, 0000ULL) },
                { L_, 32,    CAT4(0xffff, fffe, 0000, 0000ULL) },
                { L_, 48,    CAT4(0xfffe, 0000, 0000, 0000ULL) },
                { L_, BPS-1, CAT4(0x0000, 0000, 0000, 0000ULL) },
                { L_, BPS,   0 },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int      LINE   = DATA[ti].d_line;
                const int      INDEX  = DATA[ti].d_index;
                const uint64_t EXP_GT = DATA[ti].d_expectedGt;
                const uint64_t EXP_LE = ~EXP_GT;

                const uint64_t gt = Util::gt64(INDEX);
                const uint64_t le = Util::le64(INDEX);
                ASSERTV(LINE, INDEX, gt, EXP_GT == gt);
                ASSERTV(LINE, INDEX, le, EXP_LE == le);

                if (INDEX < BPS) {
                    ASSERTV(LINE, INDEX, le == Util::lt64(INDEX+1));
                    ASSERTV(LINE, INDEX, gt == Util::ge64(INDEX+1));
                }
            }
        }

        int i;

        if (verbose) cout << "Exhaustive Testing of 'eq'" << endl;
        for (i = 0; i < BITS_PER_WORD; ++i) {
            LOOP_ASSERT(i, one << i == Util::eq(i));
        }
        ASSERT(zero == Util::eq(BITS_PER_WORD));

        if (verbose) cout << "Exhaustive Testing of 'ne'" << endl;
        for (i = 0; i < (int) BITS_PER_WORD; ++i) {
            LOOP_ASSERT(i, ~(one << i) == Util::ne(i));
        }
        ASSERT(~zero == Util::ne(BITS_PER_WORD));

        if (verbose) cout << "Exhaustive Testing of 'ge'" << endl;
        for (i = 0; i < (int) BITS_PER_WORD; ++i) {
            LOOP_ASSERT(i, ~zero << i == Util::ge(i));
        }
        ASSERT(zero == Util::ge(BITS_PER_WORD));

        if (verbose) cout << "Exhaustive Testing of 'gt'" << endl;
        for (i = 0; i < (int) BITS_PER_WORD - 1; ++i) {
            LOOP_ASSERT(i, ~zero << (i + 1) == Util::gt(i));
        }
        ASSERT(zero == Util::gt(BITS_PER_WORD - 1));
        ASSERT(zero == Util::gt(BITS_PER_WORD));

        if (verbose) cout << "Exhaustive Testing of 'le'" << endl;
        for (i = 0; i < (int) BITS_PER_WORD - 1; ++i) {
            LOOP_ASSERT(i, (one << (i + 1)) - 1 == Util::le(i));
        }
        ASSERT(~zero == Util::le(BITS_PER_WORD - 1));
        ASSERT(~zero == Util::le(BITS_PER_WORD));

        if (verbose) cout << "Exhaustive Testing of 'lt'" << endl;
        for (i = 0; i < (int) BITS_PER_WORD; ++i) {
            LOOP_ASSERT(i, (one << i) - 1 == Util::lt(i));
            ASSERT(Util::lt(i) == ~Util::ge(i));
        }
        ASSERT(~zero == Util::lt(BITS_PER_WORD));

        if (verbose) cout << "Exhaustive Testing of 'eq64'" << endl;
        for (i = 0; i < (int) BITS_PER_UINT64; ++i) {
            LOOP_ASSERT(i, one64 << i == Util::eq64(i));
        }
        ASSERT(zero64 == Util::eq64(i));

        if (verbose) cout << "Exhaustive Testing of 'ne64'" << endl;
        for (i = 0; i < (int) BITS_PER_UINT64; ++i) {
            LOOP_ASSERT(i, ~(one64 << i) == Util::ne64(i));
        }
        ASSERT(~zero64 == Util::ne64(i));

        if (verbose) cout << "Exhaustive Testing of 'ge64'" << endl;
        for (i = 0; i < (int) BITS_PER_UINT64; ++i) {
            LOOP_ASSERT(i, ~zero64 << i == Util::ge64(i));
        }
        ASSERT(zero64 == Util::ge64(i));

        if (verbose) cout << "Exhaustive Testing of 'gt64'" << endl;
        for (i = 0; i < (int) BITS_PER_UINT64 - 1; ++i) {
            LOOP_ASSERT(i, ~zero64 << (i + 1) == Util::gt64(i));
        }
        ASSERT(zero64 == Util::gt64(BITS_PER_UINT64 - 1));
        ASSERT(zero64 == Util::gt64(BITS_PER_UINT64));

        if (verbose) cout << "Exhaustive Testing of 'le64'" << endl;
        for (i = 0; i < (int) BITS_PER_UINT64 - 1; ++i) {
            LOOP_ASSERT(i, (one64 << (i + 1)) - 1 == Util::le64(i));
        }
        ASSERT(~zero64 == Util::le64(BITS_PER_UINT64 - 1));
        ASSERT(~zero64 == Util::le64(BITS_PER_UINT64));

        if (verbose) cout << "Exhaustive Testing of 'lt64'" << endl;
        for (i = 0; i < (int) BITS_PER_UINT64; ++i) {
            LOOP_ASSERT(i, (one64 << i) - 1 == Util::lt64(i));
        }
        ASSERT(~zero64 == Util::lt64(i));

        if (verbose) cout << "Testing relationships between mask functions\n";
        for (i = 0; i <= (int) BITS_PER_WORD; ++i) {
            LOOP_ASSERT(i, ~Util::ge(i) == Util::lt(i));
            LOOP_ASSERT(i, ~Util::le(i) == Util::gt(i));
            LOOP_ASSERT(i, ~Util::ne(i) == Util::eq(i));
        }

        if (verbose) cout << "Testing relationships between mask functions\n";
        for (i = 0; i <= (int) BITS_PER_UINT64; ++i) {
            LOOP_ASSERT(i, ~Util::ge64(i) == Util::lt64(i));
            LOOP_ASSERT(i, ~Util::le64(i) == Util::gt64(i));
            LOOP_ASSERT(i, ~Util::ne64(i) == Util::eq64(i));
        }

        {
            bsls::AssertTestHandlerGuard guard;

            ASSERT_SAFE_PASS(Util::eq(0));
            ASSERT_SAFE_PASS(Util::eq(BITS_PER_WORD));
            ASSERT_SAFE_FAIL(Util::eq(-1));
            ASSERT_SAFE_FAIL(Util::eq(INT_MIN));
            ASSERT_SAFE_FAIL(Util::eq(BITS_PER_WORD + 1));

            ASSERT_SAFE_PASS(Util::ne(0));
            ASSERT_SAFE_PASS(Util::ne(BITS_PER_WORD));
            ASSERT_SAFE_FAIL(Util::ne(-1));
            ASSERT_SAFE_FAIL(Util::ne(INT_MIN));
            ASSERT_SAFE_FAIL(Util::ne(BITS_PER_WORD + 1));

            ASSERT_SAFE_PASS(Util::gt(0));
            ASSERT_SAFE_PASS(Util::gt(BITS_PER_WORD));
            ASSERT_SAFE_FAIL(Util::gt(-1));
            ASSERT_SAFE_FAIL(Util::gt(INT_MIN));
            ASSERT_SAFE_FAIL(Util::gt(BITS_PER_WORD + 1));

            ASSERT_SAFE_PASS(Util::ge(0));
            ASSERT_SAFE_PASS(Util::ge(BITS_PER_WORD));
            ASSERT_SAFE_FAIL(Util::ge(-1));
            ASSERT_SAFE_FAIL(Util::ge(INT_MIN));
            ASSERT_SAFE_FAIL(Util::ge(BITS_PER_WORD + 1));

            ASSERT_SAFE_PASS(Util::lt(0));
            ASSERT_SAFE_PASS(Util::lt(BITS_PER_WORD));
            ASSERT_SAFE_FAIL(Util::lt(-1));
            ASSERT_SAFE_FAIL(Util::lt(INT_MIN));
            ASSERT_SAFE_FAIL(Util::lt(BITS_PER_WORD + 1));

            ASSERT_SAFE_PASS(Util::le(0));
            ASSERT_SAFE_PASS(Util::le(BITS_PER_WORD));
            ASSERT_SAFE_FAIL(Util::le(-1));
            ASSERT_SAFE_FAIL(Util::le(INT_MIN));
            ASSERT_SAFE_FAIL(Util::le(BITS_PER_WORD + 1));

            ASSERT_SAFE_PASS(Util::eq64(0));
            ASSERT_SAFE_PASS(Util::eq64(BITS_PER_UINT64));
            ASSERT_SAFE_FAIL(Util::eq64(-1));
            ASSERT_SAFE_FAIL(Util::eq64(INT_MIN));
            ASSERT_SAFE_FAIL(Util::eq64(BITS_PER_UINT64 + 1));

            ASSERT_SAFE_PASS(Util::ne64(0));
            ASSERT_SAFE_PASS(Util::ne64(BITS_PER_UINT64));
            ASSERT_SAFE_FAIL(Util::ne64(-1));
            ASSERT_SAFE_FAIL(Util::ne64(INT_MIN));
            ASSERT_SAFE_FAIL(Util::ne64(BITS_PER_UINT64 + 1));

            ASSERT_SAFE_PASS(Util::gt64(0));
            ASSERT_SAFE_PASS(Util::gt64(BITS_PER_UINT64));
            ASSERT_SAFE_FAIL(Util::gt64(-1));
            ASSERT_SAFE_FAIL(Util::gt64(INT_MIN));
            ASSERT_SAFE_FAIL(Util::gt64(BITS_PER_UINT64 + 1));

            ASSERT_SAFE_PASS(Util::ge64(0));
            ASSERT_SAFE_PASS(Util::ge64(BITS_PER_UINT64));
            ASSERT_SAFE_FAIL(Util::ge64(-1));
            ASSERT_SAFE_FAIL(Util::ge64(INT_MIN));
            ASSERT_SAFE_FAIL(Util::ge64(BITS_PER_UINT64 + 1));

            ASSERT_SAFE_PASS(Util::lt64(0));
            ASSERT_SAFE_PASS(Util::lt64(BITS_PER_UINT64));
            ASSERT_SAFE_FAIL(Util::lt64(-1));
            ASSERT_SAFE_FAIL(Util::lt64(INT_MIN));
            ASSERT_SAFE_FAIL(Util::lt64(BITS_PER_UINT64 + 1));

            ASSERT_SAFE_PASS(Util::le64(0));
            ASSERT_SAFE_PASS(Util::le64(BITS_PER_UINT64));
            ASSERT_SAFE_FAIL(Util::le64(-1));
            ASSERT_SAFE_FAIL(Util::le64(INT_MIN));
            ASSERT_SAFE_FAIL(Util::le64(BITS_PER_UINT64 + 1));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING LOCAL ENUM TYPE VARIABLES
        //
        // Concerns:
        //: 1 Ensure that the 'enum' values in the 'struct' have the correct
        //:   values.
        //
        // Plan:
        //: 1 Carefully define a set of 'const' local "helper" variables
        //:   initialized to appropriate intermediate or final values.
        //:
        //: 2 Compare the "helper" variables to the actual 'enum' values in
        //:   the 'struct'.  (C-1)
        //
        // Testing:
        //   k_BITS_PER_UINT32
        //   k_BITS_PER_UINT64
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING LOCAL ENUM TYPE VARIABLES\n"
                             "=================================\n";

        const uint64_t EIGHT = 8;        // one fact in one place for testing
        const uint64_t BPW   = EIGHT * sizeof(uint32_t);
        const uint64_t BPW64 = EIGHT * sizeof(uint64_t);

        if (veryVerbose) {
            T_  P(EIGHT);
            T_  P(BPW);  cout << endl;

            T_  P(CHAR_BIT);
            T_  P(Util::k_BITS_PER_UINT32);
            T_  P(Util::k_BITS_PER_UINT64);
        }
        ASSERT(EIGHT == CHAR_BIT);
        ASSERT(BPW   == Util::k_BITS_PER_UINT32);
        ASSERT(BPW64 == Util::k_BITS_PER_UINT64);
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
// Copyright 2015 Bloomberg Finance L.P.
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
