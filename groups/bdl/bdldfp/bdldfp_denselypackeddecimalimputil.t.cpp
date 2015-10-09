// bdldfp_denselypackeddecimalimputil.t.cpp                           -*-C++-*-
#include <bdldfp_denselypackeddecimalimputil.h>

#include <bdldfp_uint128.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_algorithm.h>
#include <bsl_cfloat.h>
#include <bsl_climits.h>
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <typeinfo>

// Even in hardware and intel modes, we need decNumber functions.

extern "C" {
#include <decSingle.h>
}

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::hex;
using bsl::atoi;
using bsl::stringstream;

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


//-----------------------------------------------------------------------------

                                // ===
                                // D32
                                // ===

class D32 {
    Util::StorageType32 d_data;

    void set(int exponent, const unsigned char *bcd, int sign)
    {
        decSingle d_data_s;
        decSingleFromBCD(&d_data_s, exponent, bcd, sign);
        bsl::memcpy(&d_data, &d_data_s, sizeof(d_data));
    }
  public:
    template <unsigned S>
    D32(int exponent, const unsigned char (&bcd)[S], int sign)
    {
        if (S >= DECSINGLE_Pmax) {
            unsigned char buff[DECSINGLE_Pmax];
            bsl::memcpy(&buff, &bcd[S - DECSINGLE_Pmax], DECSINGLE_Pmax);
            set(exponent, buff, sign<0?DECFLOAT_Sign:0);
        } else {
            unsigned char buff[DECSINGLE_Pmax];
            bsl::memset(buff, 0, DECSINGLE_Pmax);
            for (unsigned i = S, j = DECSINGLE_Pmax - 1; i > 0; --i, --j) {
                buff[j] = bcd[i - 1];
            }
            set(exponent, buff, sign < 0 ? DECFLOAT_Sign : 0);
        }
    }

    D32(Util::StorageType32 data) : d_data(data)
    {
    }

    bool isEqual(const D32& other) const
    {
        // compare the memory for now.  It is wrong in general, sufficient here
        return bsl::memcmp(&d_data, &other.d_data, sizeof(d_data)) == 0;
    }

    bsl::ostream &printHex(bsl::ostream &o) const
    {
        bsl::ostringstream oss;
        oss << bsl::hex << d_data;

        return o << oss.str();
    }
};

bool operator==(const D32& lhs, const D32& rhs)
{
    return lhs.isEqual(rhs);
}

bsl::ostream &operator<<(bsl::ostream &o, const D32& d)
{
    return d.printHex(o);
}

//bsl::ostream &operator<<(bsl::ostream &o, const Util::StorageType32& d)
//{
 //   return operator<<(o, D32(d));
//}
                                // ===
                                // D64
                                // ===

class D64 {
    Util::StorageType64 d_data;

    void set(int exponent, const unsigned char *bcd, int sign)
    {
        decDouble d_data_s;
        decDoubleFromBCD(&d_data_s, exponent, bcd, sign);
        bsl::memcpy(&d_data, &d_data_s, sizeof(d_data));
    }
  public:
    template <unsigned S>
    D64(int exponent, const unsigned char (&bcd)[S], int sign)
    {
        if (S >= DECDOUBLE_Pmax) {
            unsigned char buff[DECDOUBLE_Pmax];
            bsl::memcpy(&buff, &bcd[S - DECDOUBLE_Pmax], DECDOUBLE_Pmax);
            set(exponent, buff, sign<0?DECFLOAT_Sign:0);
        } else {
            unsigned char buff[DECDOUBLE_Pmax];
            bsl::memset(buff, 0, DECDOUBLE_Pmax);
            for (unsigned i = S, j = DECDOUBLE_Pmax - 1; i > 0; --i, --j) {
                buff[j] = bcd[i - 1];
            }
            set(exponent, buff, sign < 0 ? DECFLOAT_Sign : 0);
        }
    }

    D64(Util::StorageType64 data) : d_data(data)
    {
    }

    bool isEqual(const D64& other) const
    {
        // compare the memory for now.  It is wrong in general, sufficient here
        return bsl::memcmp(&d_data, &other.d_data, sizeof(d_data)) == 0;
    }

    bsl::ostream &printHex(bsl::ostream &o) const
    {
        bsl::ostringstream oss;
        oss << bsl::hex << d_data;

        return o << oss.str();
    }
};

bool operator==(const D64& lhs, const D64& rhs)
{
    return lhs.isEqual(rhs);
}

bsl::ostream &operator<<(bsl::ostream &o, const D64& d)
{
    return d.printHex(o);
}

//bsl::ostream &operator<<(bsl::ostream &o, const Util::StorageType64& d)
//{
    //return operator<<(o, D64(d));
//}

                                // ====
                                // D128
                                // ====

class D128 {
    Util::StorageType128 d_data;

    void set(int exponent, const unsigned char *bcd, int sign)
    {
        decQuad d_data_s;
        decQuadFromBCD(&d_data_s, exponent, bcd, sign);
        bsl::memcpy(&d_data, &d_data_s, sizeof(d_data));
    }
  public:
    template <unsigned S>
    D128(int exponent, const unsigned char (&bcd)[S], int sign)
    {
        if (S >= DECQUAD_Pmax) {
            unsigned char buff[DECQUAD_Pmax];
            bsl::memcpy(&buff, &bcd[S - DECQUAD_Pmax], DECQUAD_Pmax);
            set(exponent, buff, sign<0?DECFLOAT_Sign:0);
        } else {
            unsigned char buff[DECQUAD_Pmax];
            bsl::memset(buff, 0, DECQUAD_Pmax);
            for (unsigned i = S, j = DECQUAD_Pmax - 1; i > 0; --i, --j) {
                buff[j] = bcd[i - 1];
            }
            set(exponent, buff, sign < 0 ? DECFLOAT_Sign : 0);
        }
    }

    D128(Util::StorageType128 data) : d_data(data)
    {
    }

    bool isEqual(const D128& other) const
    {
        // compare the memory for now.  It is wrong in general, sufficient here
        return bsl::memcmp(&d_data, &other.d_data, sizeof(d_data)) == 0;
    }

    bsl::ostream &printHex(bsl::ostream &o) const
    {
        bsl::ostringstream oss;
        oss << bsl::hex << d_data.high() << ":" << d_data.low();

        return o << oss.str();
    }
};

bool operator==(const D128& lhs, const D128& rhs)
{
    return lhs.isEqual(rhs);
}

bsl::ostream &operator<<(bsl::ostream &o, const D128& d)
{
    return d.printHex(o);
}

//bsl::ostream &operator<<(bsl::ostream &o, const Util::StorageType128& d)
//{
 //   return operator<<(o, D128(d));
//}

// ----------------------------------------------------------------------------

// Parse a 'int' literal in the specified 's', returning the result in the
// address of the specified 'result', using the specified 'pa' for temporary
// memory allocation. Return true iff 's' is a valid 'int' literal.
bool parseInt(const bsl::string& s, int *result, bslma::TestAllocator *pa)
{
    if (s.empty()) return 0;                                          // RETURN

    bsl::istringstream ss(s, pa);
    long long wider;
    ss >> wider;
    if (ss.fail() || wider > INT_MAX || wider < INT_MIN) return false;
                                                                      // RETURN
    *result = static_cast<int>(wider);
    return true;
}

// Parse an 'unsigned int' literal in the specified 's', returning the result
// in the address of the specified 'result', using the specified 'pa' for
// temporary memory allocation. Return true iff 's' is a valid 'unsigned int'
// literal.
bool parseUInt(const bsl::string& s, unsigned int *result,
               bslma::TestAllocator *pa)
{
    if (s.empty()) return 0;                                          // RETURN
    if (s[0] == '-') return 0;                                        // RETURN
    bsl::istringstream ss(s, pa);
    unsigned long long wider;
    ss >> wider;
    if (ss.fail() || wider > UINT_MAX ) return false;                 // RETURN
    *result = static_cast<unsigned int>(wider);
    return true;
}

// Parse a 'long long' literal in the specified 's', returning the result in
// the address of the specified 'result', using the specified 'pa' for
// temporary memory allocation. Return true iff 's' is a valid 'long long'
// literal.
bool parseLL(const bsl::string& s, long long *result,
             bslma::TestAllocator *pa)
{
    if (s[0] == '-') {
        bsl::istringstream ss(s, pa);
        return !(ss >> *result).fail();                               // RETURN
    }

    bsl::istringstream ss(s, pa);
    unsigned long long wider;
    ss >> wider;
    if (ss.fail() || wider > static_cast<unsigned long long>(
                                      bsl::numeric_limits<long long>::max())) {
        return false;                                                 // RETURN
    }
    *result = static_cast<long long>(wider);
    return true;
}

// Parse an 'unsigned long long' literal in the specified 's', returning the
// result in the address of the specified 'result', using the specified 'pa'
// for temporary memory allocation. Return true iff 's' is a valid
// 'unsigned long long' literal.
bool parseULL(const bsl::string& s, unsigned long long *result,
              bslma::TestAllocator *pa)
{
    if (s.empty()) return 0;                                          // RETURN
    if (s[0] == '-') return 0;                                        // RETURN
    bsl::istringstream ss(s, pa);
    return !(ss >> *result).fail();
}

// Normalize the specified 'mantissa' by dividing by powers of 10 if
// divisible. Add to the specified 'exponent' the powers of 10 removed from
// the mantissa. If 'mantissa' is zero, set 'exponent' to zero, giving a
// canonical zero value.
void normalize(long long *mantissa, int *exponent)
{
    // Canonicalize zeros.
    if (!(*mantissa)) {
        *exponent = 0;
        return;                                                       // RETURN
    }

    while (!(*mantissa % 10)) {
        *mantissa/= 10;
        ++*exponent;
    }
}

// Compare two mantissa and exponent pairs, with normalization.
bool decimalNormalizedCompare(long long lhs_mantissa, int lhs_exponent,
                              long long rhs_mantissa, int rhs_exponent )
{
    normalize(&lhs_mantissa, &lhs_exponent);
    normalize(&rhs_mantissa, &rhs_exponent);

    return lhs_mantissa == rhs_mantissa && lhs_exponent == rhs_exponent;
}

//-----------------------------------------------------------------------------

unsigned encodeDeclet(unsigned value)
    // Return an 'unsigned int' representing the specified 'value' in a Densely
    // Packed Decimal format.  This is an oracle function for
    // 'DenselyPackedDecimalImpUtil::encodeDeclet'.
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
                 | (lowerDigit  << 0);                                // RETURN

        case 02:
            return 0x0A
                 | ((upperDigit  & 07) << 7)
                 | ((middleDigit & 01) << 4)
                 | ((lowerDigit  & 06) << 4)
                 | ((lowerDigit  & 01) << 0);                         // RETURN

        case 04:
            return 0x0C
                 | ((upperDigit  & 01) << 7)
                 | ((middleDigit & 07) << 4)
                 | ((lowerDigit  & 06) << 7)
                 | ((lowerDigit  & 01) << 0);                         // RETURN

        case 3:
            return 0x4E
                 | ((upperDigit  & 07) << 7)
                 | ((middleDigit & 01) << 4)
                 | ((lowerDigit  & 01) << 0);                         // RETURN

        case 5:
            return 0x2E
                 | ((upperDigit  & 01) << 7)
                 | ((middleDigit & 06) << 7)
                 | ((middleDigit & 01) << 4)
                 | ((lowerDigit  & 01) << 0);                         // RETURN

        case 6:
            return 0x0E
                 | ((upperDigit  & 01) << 7)
                 | ((middleDigit & 01) << 4)
                 | ((lowerDigit  & 06) << 7)
                 | ((lowerDigit  & 01) << 0);                         // RETURN

        case 07:
            return 0x6e
                 | ((upperDigit  & 01) << 7)
                 | ((middleDigit & 01) << 4)
                 | ((lowerDigit  & 01) << 0);                         // RETURN
    }

    return 0;

}

unsigned decodeDeclet(unsigned declet)
    // Return an 'unsigned int' representing the specified Densely Packed
    // Decimal 'declet' in a binary format.  This is an oracle function for
    // 'DenselyPackedDecimalImpUtil::decodeDeclet'.
{
    if (!(declet & 0x8)) {

        // When the "magic" bit is clear, we have three small, 3-bit encoded
        // digits.

        return     1 * ( ( declet >> 0 ) & 07 )
               +  10 * ( ( declet >> 4 ) & 07 )
               + 100 * ( ( declet >> 7 ) & 07 );                      // RETURN
    }


    // If at least one of bit one and bit two are not set, there is only one
    // small digit.  Therefore:

    switch ((declet >> 1) & 03) {
        case 0:

            // The third digit is "big"

            return         ((declet       & 01) | 010)
                   +  10 * ((declet >> 4) & 07)
                   + 100 * ((declet >> 7) & 07);                      // RETURN

        case 1:

            // The second digit is "big"

            return     1 * (((declet >> 4) & 06) | (declet & 1))
                   +  10 * (((declet >> 4) & 01) | 010)
                   + 100 * (((declet >> 7) & 07) | 000);              // RETURN

        case 2:

            // The first digit is "big"

            return     1 * (((declet >> 7) & 06) | (declet & 1))
                   +  10 * (((declet >> 4) & 07) | 000)
                   + 100 * (((declet >> 7) & 01) | 010);              // RETURN

        default:

            // If both bits are set, then there are two large digits, so we
            // handle that with a different switch block.

            break;
    }

    switch ((declet >> 5) & 03) {
        case 0:

            // The third digit is "small"

            return     1 * (((declet >> 7) & 06) | ( declet & 1 ) )
                   +  10 * (((declet >> 4) & 01) | 010 )
                   + 100 * (((declet >> 7) & 01) | 010 );             // RETURN

        case 1:

            // The second digit is "small"

            return     1 * (((declet >> 0) & 01) | 010)
                   +  10 * (((declet >> 7) & 06) | ((declet >> 4) & 01))
                   + 100 * (((declet >> 7) & 01) | 010);              // RETURN

        case 2:  // The first digit is "small"

            return     1 * (((declet >> 0) & 01) | 010)
                   +  10 * (((declet >> 4) & 01) | 010)
                   + 100 * (((declet >> 7) & 07) | 000);              // RETURN

        default:

            // If both bits in question are set, then there are three large
            // digits, so we can merely compose them

            return     1 * (((declet >> 0) & 01) | 010)
                   +  10 * (((declet >> 4) & 01) | 010)
                   + 100 * (((declet >> 7) & 01) | 010);              // RETURN
    }
}

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
//  bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator  ta(veryVeryVeryVerbose);
    bslma::TestAllocator *pa = &ta;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'makeDecimalRawXX' FUNCTIONS
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

        if (verbose) cout << endl
                          << "TESTING 'makeDecimalRawXX' FUNCTIONS" << endl
                          << "====================================" << endl;
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
            ASSERT(!bsl::memcmp(&v255, &v255x, 8));
            ASSERT(!bsl::memcmp(&v582, &v582x, 8));

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
#endif

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
               0, 0, 0, 0, 0, 0, 0},
              1},
            { L_,                    "0",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0},
              1},
            { L_,                    "0",   -10, -10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0},
              1},
            { L_,                  "100",    -2, -2,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 0, 0},
              1},
            { L_,                    "1",     2, 2,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              1},
            { L_,                    "1",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              1},
            { L_,                   "12",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 1, 2},
              1},
            { L_,                  "123",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 2, 3},
              1},
            { L_,                 "1234",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 1, 2, 3, 4},
              1},
            { L_,                "12345",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 1, 2, 3, 4, 5},
              1},
            { L_,               "123456",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 1, 2, 3, 4, 5, 6},
              1},
            { L_,              "1234567",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               1, 2, 3, 4, 5, 6, 7},
              1},
            { L_,             "12345678",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 1,
               2, 3, 4, 5, 6, 7, 8},
              1},
            { L_,            "123456789",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9},
              1},
            { L_,           "1234567890",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1, 2, 3,
               4, 5, 6, 7, 8, 9, 0},
              1},
            { L_,           "2147483647",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 2, 1, 4,
               7, 4, 8, 3, 6, 4, 7},
              1},
            { L_,           "4294967295",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 4, 2, 9,
               4, 9, 6, 7, 2, 9, 5},
              1},
            { L_,          "12345678901",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 1, 2, 3, 4,
               5, 6, 7, 8, 9, 0, 1},
              1},
            { L_,         "123456789012",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 2, 3, 4, 5,
               6, 7, 8, 9, 0, 1, 2},
              1},
            { L_,        "1234567890123",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 1, 2, 3, 4, 5, 6,
               7, 8, 9, 0, 1, 2, 3},
              1},
            { L_,       "12345678901234",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 1, 2, 3, 4, 5, 6, 7,
               8, 9, 0, 1, 2, 3, 4},
              1},
            { L_,      "123456789012345",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 1, 2, 3, 4, 5, 6, 7, 8,
               9, 0, 1, 2, 3, 4, 5},
              1},
            { L_,     "1234567890123456",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               1, 2, 3, 4, 5, 6, 7, 8, 9,
               0, 1, 2, 3, 4, 5, 6},
              1},
            { L_,    "12345678901234567",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 1,
               2, 3, 4, 5, 6, 7, 8, 9, 0,
               1, 2, 3, 4, 5, 6, 7},
              1},
            { L_,   "123456789012345678",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9, 0, 1,
               2, 3, 4, 5, 6, 7, 8},
              1},
            { L_,  "1234567890123456789",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1, 2, 3,
               4, 5, 6, 7, 8, 9, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9},
              1},
            { L_,  "9223372036854775807",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 9, 2, 2, 3, 3,
               7, 2, 0, 3, 6, 8, 5, 4, 7, 7,
               5, 8, 0, 7},
              1},
            { L_, "18446744073709551615",    10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 8, 4, 4, 6, 7,
               4, 4, 0, 7, 3, 7, 0, 9, 5, 5,
               1, 6, 1, 5},
              1},
            { L_,                    "-1",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              -1},
            { L_,                   "-12",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 1, 2},
              -1},
            { L_,                  "-123",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 2, 3},
              -1},
            { L_,                 "-1234",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 1, 2, 3, 4},
              -1},
            { L_,                "-12345",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 1, 2, 3, 4, 5},
              -1},
            { L_,               "-123456",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 1, 2, 3, 4, 5, 6},
              -1},
            { L_,              "-1234567",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               1, 2, 3, 4, 5, 6, 7},
              -1},
            { L_,             "-12345678",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 1,
               2, 3, 4, 5, 6, 7, 8},
              -1},
            { L_,            "-123456789",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9},
              -1},
            { L_,           "-1234567890",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1, 2, 3,
               4, 5, 6, 7, 8, 9, 0},
              -1},
            { L_,           "-2147483647",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 2, 1, 4,
               7, 4, 8, 3, 6, 4, 7},
              -1},
            { L_,           "-2147483648",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 2, 1, 4,
               7, 4, 8, 3, 6, 4, 8},
              -1},
            { L_,          "-12345678901",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 1, 2, 3, 4,
               5, 6, 7, 8, 9, 0, 1},
              -1},
            { L_,         "-123456789012",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 2, 3, 4, 5,
               6, 7, 8, 9, 0, 1, 2},
              -1},
            { L_,        "-1234567890123",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 1, 2, 3, 4, 5, 6,
               7, 8, 9, 0, 1, 2, 3},
              -1},
            { L_,       "-12345678901234",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 1, 2, 3, 4, 5, 6, 7,
               8, 9, 0, 1, 2, 3, 4},
              -1},
            { L_,      "-123456789012345",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 1, 2, 3, 4, 5, 6, 7, 8,
               9, 0, 1, 2, 3, 4, 5},
              -1},
            { L_,     "-1234567890123456",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               1, 2, 3, 4, 5, 6, 7, 8, 9,
               0, 1, 2, 3, 4, 5, 6},
              -1},
            { L_,    "-12345678901234567",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 1,
               2, 3, 4, 5, 6, 7, 8, 9, 0,
               1, 2, 3, 4, 5, 6, 7},
              -1},
            { L_,   "-123456789012345678",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9, 0, 1,
               2, 3, 4, 5, 6, 7, 8},
              -1},
            { L_,  "-1234567890123456789",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1, 2, 3,
               4, 5, 6, 7, 8, 9, 0, 1, 2,
               3, 4, 5, 6, 7, 8, 9},
              -1},
            { L_,  "-9223372036854775807",   10, 10,
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 9, 2, 2, 3, 3,
               7, 2, 0, 3, 6, 8, 5, 4, 7, 7,
               5, 8, 0, 7},
              -1},
            { L_,  "-9223372036854775808",   10, 10,
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 9, 2, 2, 3, 3,
                 7, 2, 0, 3, 6, 8, 5, 4, 7, 7,
                 5, 8, 0, 8},
              -1},

            { L_,                    "1",  -101, -101,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              1},
            { L_,                    "1",    90, 90,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              1},
            { L_,                   "-1",  -101, -101,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              -1},
            { L_,                   "-1",    90, 90,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              -1},

            { L_,                    "1",  -398, -398,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              1},
            { L_,                    "1",   369, 369,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              1},
            { L_,                   "-1",  -398, -398,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              -1},
            { L_,                   "-1",   369, 369,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              -1},

            { L_,                    "1", -6176, -6176,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              1},
            { L_,                    "1",  6111,  6111,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              1},
            { L_,                   "-1", -6176, -6176,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              -1},
            { L_,                   "-1",  6111,  6111,
              {0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 1},
              -1},
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
                const Util::StorageType128 ACTUAL =
                    Util::makeDecimalRaw128(mantissa_in_int, EXPONENT);
                LOOP3_ASSERT(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            }

            if (parseUInt(MANTISSA, &mantissa_in_uint, pa)) {
                const Util::StorageType128 ACTUAL =
                    Util::makeDecimalRaw128(mantissa_in_uint, EXPONENT);
                LOOP3_ASSERT(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            }

            if (parseLL(MANTISSA, &mantissa_in_ll, pa)) {
                const Util::StorageType128 ACTUAL =
                    Util::makeDecimalRaw128(mantissa_in_ll, EXPONENT);
                LOOP3_ASSERT(LINE, ACTUAL, EXPECTED, ACTUAL == EXPECTED);
            }

            if (parseULL(MANTISSA, &mantissa_in_ull, pa)) {
                const Util::StorageType128 ACTUAL =
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
                    const Util::StorageType64 ACTUAL64 =
                        Util::makeDecimalRaw64(mantissa_in_int, EXPONENT);
                    LOOP3_ASSERT(LINE, ACTUAL64, EXPECTED64,
                                 ACTUAL64 == EXPECTED64);
                }

                if (parseUInt(MANTISSA, &mantissa_in_uint, pa)) {
                    const Util::StorageType64 ACTUAL64 =
                        Util::makeDecimalRaw64(mantissa_in_uint, EXPONENT);
                    LOOP3_ASSERT(LINE, ACTUAL64, EXPECTED64,
                                 ACTUAL64 == EXPECTED64);
                }

                if (parseLL(MANTISSA, &mantissa_in_ll, pa)) {
                    const Util::StorageType64 ACTUAL64 =
                        Util::makeDecimalRaw64(mantissa_in_ll, EXPONENT);
                    LOOP3_ASSERT(LINE, ACTUAL64, EXPECTED64,
                                 ACTUAL64 == EXPECTED64);
                }

                if (parseULL(MANTISSA, &mantissa_in_ull, pa)) {
                    const Util::StorageType64 ACTUAL64 =
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
                    const Util::StorageType32 ACTUAL32 =
                        Util::makeDecimalRaw32(mantissa_in_int, EXPONENT);
                    LOOP_ASSERT(LINE, ACTUAL32 == EXPECTED32);
                }
            }
        }

        // Test that 'makeDecimalRaw32' enforces undefined behavior in the
        // right build mode
        {
            using bsls::AssertFailureHandlerGuard;
            AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw32(42, -102));
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw32(42, -101));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw32(42, 90));
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw32(42, 91));

            // Check mantissa lower bound
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw32(-10000000, 42));
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw32(- 9999999, 42));

            // Check mantissa upper bound
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw32(  9999999, 42));
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw32( 10000000, 42));
        }

        // Test that 'makeDecimalRaw64' enforces undefined behavior in the
        // right build mode
        {
            using bsls::AssertFailureHandlerGuard;
            AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw64(42, -399));
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw64(42, -398));

            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw64(42u, -399));
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw64(42u, -398));

            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw64( 42ll,
                                                                   -399));
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw64( 42ll,
                                                                   -398));

            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw64( 42ull,
                                                                   -399));
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw64( 42ull,
                                                                   -398));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw64(42, 369));
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw64(42, 370));

            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw64(42u, 369));
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw64(42u, 370));

            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw64(42ll, 369));
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw64(42ll, 370));

            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw64(4ull, 369));
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw64(4ull, 370));

            // Check mantissa lower bound
            BSLS_ASSERTTEST_ASSERT_FAIL(
                             Util::makeDecimalRaw64(-10000000000000000ll, 42));
            BSLS_ASSERTTEST_ASSERT_PASS(
                             Util::makeDecimalRaw64(- 9999999999999999ll, 42));

            // Check mantissa upper bound
            BSLS_ASSERTTEST_ASSERT_PASS(
                              Util::makeDecimalRaw64( 9999999999999999ll, 42));
            BSLS_ASSERTTEST_ASSERT_FAIL(
                              Util::makeDecimalRaw64(10000000000000000ll, 42));

            BSLS_ASSERTTEST_ASSERT_PASS(
                             Util::makeDecimalRaw64( 9999999999999999ull, 42));
            BSLS_ASSERTTEST_ASSERT_FAIL(
                             Util::makeDecimalRaw64(10000000000000000ull, 42));
        }

        // Test that 'makeDecimalRaw128' enforces undefined behavior in the
        // right build mode
        {
            using bsls::AssertFailureHandlerGuard;
            AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

            // Check exponent lower bound
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(       42,
                                                                    -6177));
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(       42,
                                                                    -6176));

            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(       42u,
                                                                    -6177));
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(       42u,
                                                                    -6176));

            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(       42ll,
                                                                    -6177));
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(       42ll,
                                                                    -6176));

            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(       42ull,
                                                                    -6177));
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(       42ull,
                                                                    -6176));

            // Check exponent upper bound
            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(      42,
                                                                    6111));
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(      42,
                                                                    6112));

            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(      42u,
                                                                    6111));
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(      42u,
                                                                    6112));

            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(      42ll,
                                                                    6111));
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(      42ll,
                                                                    6112));

            BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(      42ull,
                                                                    6111));
            BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(      42ull,
                                                                    6112));
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
        //   decodeDeclet(unsigned declet)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'decodeDeclet(unsigned declet)'"
                          << endl
                          << "============================================"
                          << endl;


        // Exhaustively test every possible declet encoded value, in the range
        // '[0, 1000)'.

        for (unsigned int i = 0; i < 1000; ++i) {
            if (veryVerbose) cout << "i = " << i << endl;
            unsigned declet   = Util::encodeDeclet(i);

            unsigned result   = Util::decodeDeclet(declet);
            unsigned expected =       decodeDeclet(declet);
            LOOP4_ASSERT(i, declet, result, expected, result == expected);
            LOOP4_ASSERT(i, declet, result, expected, result == i);
        }

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

                // All aliased multi-representation declets should pass for
                // high-zeros but have undefined behavior with aliased forms.
                // The soft undefined behavior is tested on the aliased forms
                // here.  The loop counter is unsigned, to facilitate easy
                // bitwise operations on its value.

                for (unsigned i = 0; i < 7; ++i) {
                    if (veryVerbose) cout << "i = " << i << endl;
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
        //   encodeDeclet(unsigned digits)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'encodeDeclet(unsigned digits)'"
                          << endl
                          << "============================================"
                          << endl;

        for (int i = 0; i < 1000; ++i) {
            if (veryVerbose) cout << "i = " << i << endl;
            unsigned result   = Util::encodeDeclet(i);
            unsigned expected =       encodeDeclet(i);
            LOOP3_ASSERT(i, result, expected, result == expected);
            LOOP2_ASSERT(i, result, result < 1024);
        }

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
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
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
