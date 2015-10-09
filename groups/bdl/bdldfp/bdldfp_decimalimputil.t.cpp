// bdldfp_decimalimputil.t.cpp                                        -*-C++-*-
#include <bdldfp_decimalimputil.h>

#include <bslim_testutil.h>

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
// CLASS METHODS
// [13] makeDecimal64(                   int, int)
// [13] makeDecimal64(unsigned           int, int)
// [13] makeDecimal64(         long long int, int)
// [13] makeDecimal64(unsigned long long int, int)
// [12] makeInfinity64(bool)
// [14]  int32ToDecimal32 (                   int)
// [14] uint32ToDecimal32 (unsigned           int)
// [14]  int64ToDecimal32 (         long long int)
// [14] uint64ToDecimal32 (unsigned long long int)
// [14]  int32ToDecimal64 (                   int)
// [14] uint32ToDecimal64 (unsigned           int)
// [14]  int64ToDecimal64 (         long long int)
// [14] uint64ToDecimal64 (unsigned long long int)
// [14]  int32ToDecimal128(                   int)
// [14] uint32ToDecimal128(unsigned           int)
// [14]  int64ToDecimal128(         long long int)
// [14] uint64ToDecimal128(unsigned long long int)
// [ 5] add(ValueType64,  ValueType64)
// [ 5] add(ValueType128, ValueType128)
// [ 6] subtract(ValueType64,  ValueType64)
// [ 6] subtract(ValueType128, ValueType128)
// [ 7] multiply(ValueType64,  ValueType64)
// [ 7] multiply(ValueType128, ValueType128)
// [ 8] divide(ValueType64,  ValueType64)
// [ 8] divide(ValueType128, ValueType128)
// [ 9] negate(ValueType32)
// [ 9] negate(ValueType64)
// [ 9] negate(ValueType128)
// [16] less(ValueType32,  ValueType32)
// [16] less(ValueType64,  ValueType64)
// [16] less(ValueType128, ValueType128)
// [16] greater(ValueType32,  ValueType32)
// [16] greater(ValueType64,  ValueType64)
// [16] greater(ValueType128, ValueType128)
// [16] lessEqual(ValueType32,  ValueType32)
// [16] lessEqual(ValueType64,  ValueType64)
// [16] lessEqual(ValueType128, ValueType128)
// [16] greaterEqual(ValueType32,  ValueType32)
// [16] greaterEqual(ValueType64,  ValueType64)
// [16] greaterEqual(ValueType128, ValueType128)
// [ 3] equal(ValueType32,  ValueType32)
// [ 3] equal(ValueType64,  ValueType64)
// [ 3] equal(ValueType128, ValueType128)
// [ 4] notEqual(ValueType32,  ValueType32)
// [ 4] notEqual(ValueType64,  ValueType64)
// [ 4] notEqual(ValueType128, ValueType128)
// [10] convertToDecimal32( const ValueType64&)
// [10] convertToDecimal64( const ValueType32&)
// [10] convertToDecimal64( const ValueType128&)
// [10] convertToDecimal128(const ValueType32&)
// [10] convertToDecimal128(const ValueType64&)
// [15] binaryToDecimal32 (float)
// [15] binaryToDecimal32 (double)
// [15] binaryToDecimal64 (float)
// [15] binaryToDecimal64 (double)
// [15] binaryToDecimal128(float)
// [15] binaryToDecimal128(double)
// [ 2] makeDecimalRaw32 (                   int, int)
// [ 2] makeDecimalRaw64 (unsigned long long int, int)
// [ 2] makeDecimalRaw64 (         long long int, int)
// [ 2] makeDecimalRaw64 (unsigned           int, int)
// [ 2] makeDecimalRaw64 (                   int, int)
// [ 2] makeDecimalRaw128(unsigned long long int, int)
// [ 2] makeDecimalRaw128(         long long int, int)
// [ 2] makeDecimalRaw128(unsigned           int, int)
// [ 2] makeDecimalRaw128(                   int, int)
// [17] scaleB(ValueType32,  int)
// [17] scaleB(ValueType64,  int)
// [17] scaleB(ValueType128, int)
// [11] parse32 (const char *)
// [11] parse64 (const char *)
// [11] parse128(const char *)
// [18] format(ValueType32,  char *)
// [18] format(ValueType64,  char *)
// [18] format(ValueType128, char *)
// [ 1] checkLiteral(double)
// [20] convertFromDPD(DenselyPackedDecimalImpUtil::StorageType32)
// [20] convertFromDPD(DenselyPackedDecimalImpUtil::StorageType64)
// [20] convertFromDPD(DenselyPackedDecimalImpUtil::StorageType128)
// [19] convertToDPD(ValueType32)
// [19] convertToDPD(ValueType64)
// [19] convertToDPD(ValueType128)
// [22] convertFromBID(BinaryIntegralDecimalImpUtil::StorageType32)
// [22] convertFromBID(BinaryIntegralDecimalImpUtil::StorageType64)
// [22] convertFromBID(BinaryIntegralDecimalImpUtil::StorageType128)
// [21] convertToBID(ValueType32)
// [21] convertToBID(ValueType64)
// [21] convertToBID(ValueType128)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] TEST 'notEqual' FOR 'NaN' CORRECTNESS
// [23] USAGE EXAMPLE
// ----------------------------------------------------------------------------

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace BDEC = BloombergLP::bdldfp;
typedef BDEC::DecimalImpUtil Util;

const long long mantissas[] = {
                                               0LL,
                                               2LL,
                                               7LL,
                                              35LL,
                                              72LL,
                                             135LL,
                                             924LL,

                  // Exhaustive mantissa cases
                                               1LL,
                                               9LL,
                                              12LL,
                                              98LL,
                                             123LL,
                                             987LL,
                                            1234LL,
                                            9876LL,
                                           12345LL,
                                           98765LL,
                                          123456LL,
                                          987654LL,
                                         1234567LL,
                                         9876543LL,
                                        12345678LL,
                                        98765432LL,
                                       123456789LL,
                                       987654321LL,
                                      1234567890LL,
                                      9876543210LL,
                                     12345678901LL,
                                     98765432109LL,
                                    123456789012LL,
                                    987654321098LL,
                                   1234567890123LL,
                                   9876543210987LL,
                                  12345678901234LL,
                                  98765432109876LL,
                                 123456789012345LL,
                                 987654321098765LL,
                                1234567890123456LL,
                                9876543210987654LL,

                               -               1LL,
                               -               9LL,
                               -              12LL,
                               -              98LL,
                               -             123LL,
                               -             987LL,
                               -            1234LL,
                               -            9876LL,
                               -           12345LL,
                               -           98765LL,
                               -          123456LL,
                               -          987654LL,
                               -         1234567LL,
                               -         9876543LL,
                               -        12345678LL,
                               -        98765432LL,
                               -       123456789LL,
                               -       987654321LL,
                               -      1234567890LL,
                               -      9876543210LL,
                               -     12345678901LL,
                               -     98765432109LL,
                               -    123456789012LL,
                               -    987654321098LL,
                               -   1234567890123LL,
                               -   9876543210987LL,
                               -  12345678901234LL,
                               -  98765432109876LL,
                               - 123456789012345LL,
                               - 987654321098765LL,
                               -1234567890123456LL,
                               -9876543210987654LL
                              };
const int numMantissas = sizeof(mantissas) / sizeof(*mantissas);

const int exponents[] = {
                            0,
                            1,
                            7,
                           13,
                           64,
                          123,
                          321,

                         -  1,
                         -  7,
                         - 13,
                         - 64,
                         -123,
                         -321
                        };
const int numExponents = sizeof(exponents) / sizeof(*exponents);


static const long long TEST_NONZERO_MANTISSAS[] = {
    // These numbers will test equality of decimal floating point values of
    // different quanta.
    2LL,
    7LL,
    20LL,
    70LL,
    200LL,
    700LL,
    2000LL,
    7000LL,
    20000LL,
    70000LL,
    200000LL,
    700000LL,
    2000000LL,
    7000000LL,
    20000000LL,
    70000000LL,
    200000000LL,
    700000000LL,
    2000000000LL,
    7000000000LL,
    20000000000LL,
    70000000000LL,
    200000000000LL,
    700000000000LL,
    2000000000000LL,
    7000000000000LL,
    20000000000000LL,
    70000000000000LL,
    200000000000000LL,
    700000000000000LL,
    2000000000000000LL,
    7000000000000000LL,
    20000000000000000LL,
    70000000000000000LL,
    200000000000000000LL,
    700000000000000000LL,

    // These number ensure that we can handle digits in all positions of the
    // mantissa.
    12LL,
    123LL,
    1234LL,
    12345LL,
    123456LL,
    1234567LL,
    12345678LL,
    123456789LL,
    1234567890LL,
    12345678901LL,
    123456789012LL,
    1234567890123LL,
    12345678901234LL,
    123456789012345LL,
    1234567890123456LL,
    12345678901234567LL,
    123456789012345678LL,
    1234567890123456789LL,

    // These numbers test the behavior of rounding to 16 significant figures
    // (e.g., in makeDecimal64.)
    44444444444444444LL,
    44444444444444445LL,
    44444444444444455LL,
    444444444444444444LL,
    444444444444444445LL,
    444444444444444455LL,
    4444444444444444445LL,
    4444444444444444455LL,
    4444444444444444555LL,
    5555555555555555555LL,

    // Negative variations of these test numbers.
    -2LL,
    -7LL,
    -20LL,
    -70LL,
    -200LL,
    -700LL,
    -2000LL,
    -7000LL,
    -20000LL,
    -70000LL,
    -200000LL,
    -700000LL,
    -2000000LL,
    -7000000LL,
    -20000000LL,
    -70000000LL,
    -200000000LL,
    -700000000LL,
    -2000000000LL,
    -7000000000LL,
    -20000000000LL,
    -70000000000LL,
    -200000000000LL,
    -700000000000LL,
    -2000000000000LL,
    -7000000000000LL,
    -20000000000000LL,
    -70000000000000LL,
    -200000000000000LL,
    -700000000000000LL,
    -2000000000000000LL,
    -7000000000000000LL,
    -20000000000000000LL,
    -70000000000000000LL,
    -200000000000000000LL,
    -700000000000000000LL,
    -12LL,
    -123LL,
    -1234LL,
    -12345LL,
    -123456LL,
    -1234567LL,
    -12345678LL,
    -123456789LL,
    -1234567890LL,
    -12345678901LL,
    -123456789012LL,
    -1234567890123LL,
    -12345678901234LL,
    -123456789012345LL,
    -1234567890123456LL,
    -12345678901234567LL,
    -123456789012345678LL,
    -1234567890123456789LL,

    -44444444444444444LL,
    -44444444444444445LL,
    -44444444444444455LL,
    -444444444444444444LL,
    -444444444444444445LL,
    -444444444444444455LL,
    -4444444444444444445LL,
    -4444444444444444455LL,
    -4444444444444444555LL,
    -5555555555555555555LL,

};

const int NUM_TEST_NONZERO_MANTISSAS =
            sizeof TEST_NONZERO_MANTISSAS / sizeof *TEST_NONZERO_MANTISSAS;

static const int TEST_EXPONENTS[] = {
    -4064,
    -399,
    -398,
    -355,
    -44,
    -42,
    -13,
    -4,
    -3,
    -2,
    -1,
    0,
    1,
    2,
    3,
    4,
    13,
    42,
    44,
    355,

    // These exponents exercise the rounding behavior of makeDecimal64.
    369,
    370,
    371,
    372,
    373,
    374,
    375,
    376,
    377,
    378,
    379,
    380,
    381,
    382,
    383,
    384,
    385,

    4064
};

const int NUM_TEST_EXPONENTS = sizeof TEST_EXPONENTS / sizeof *TEST_EXPONENTS;

// ============================================================================
//                      HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

template<class INTEGER>
bsl::string makeParseString(INTEGER mantissa, int exponent)
    // Return a string representing the specified 'mantissa' and 'exponent', in
    // the format 'MeX', where 'M' is a string representation of 'mantissa',
    // and 'X' is a string representation of 'exponent'.  Note that this
    // function is intended to reduce the complexity of the 'makeDecimalRawXX'
    // test cases.
{
    bsl::ostringstream oss;
    oss << mantissa << "e" << exponent;
    return oss.str();
}

bool nanEqual(Util::ValueType64 lhs, Util::ValueType64 rhs)
    // Return true if the specified 'lhs' and 'rhs' are the same value, even in
    // the case of 'NaN'.  Two 'ValueType64' objects are considered equal if
    // either 'Util::equal' returns true, or both 'lhs' and 'rhs' are not equal
    // to themselves (implying them both to be 'NaN').
{
    return  Util::equal(lhs, rhs)
        || (Util::notEqual(lhs, lhs) && Util::notEqual(rhs, rhs));
}

bool nanEqual(Util::ValueType128 lhs, Util::ValueType128 rhs)
    // Return true if the specified 'lhs' and 'rhs' are the same value, even in
    // the case of 'NaN'.  Two 'ValueType128' objects are considered equal if
    // either 'Util::equal' returns true, or both 'lhs' and 'rhs' are not equal
    // to themselves (implying them both to be 'NaN').
{
    return  Util::equal(lhs, rhs)
        || (Util::notEqual(lhs, lhs) && Util::notEqual(rhs, rhs));
}

Util::ValueType32 alternateMakeDecimalRaw32(int mantissa, int exponent)
    // Create a 'ValueType32' object representing a decimal floating point
    // number consisting of the specified 'mantissa' and 'exponent', with the
    // sign given by 'mantissa'.  The behavior is undefined unless
    // 'abs(mantissa) <= 9,999,999' and '-101 <= exponent <= 90'.  Note that
    // this contract is identical to the one in the Utility under test.
{
    Util::ValueType32 result;

    BDEC::DenselyPackedDecimalImpUtil::StorageType32 denselyPacked =
       BDEC::DenselyPackedDecimalImpUtil::makeDecimalRaw32(mantissa, exponent);

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    BID_UINT32 temp;
    bsl::memcpy(&temp, &denselyPacked, sizeof(temp));
    BID_UINT32 converted = __bid_dpd_to_bid32(temp);

    bsl::memcpy(&denselyPacked, &converted, sizeof(denselyPacked));
#endif

    bsl::memcpy(&result, &denselyPacked, sizeof(result));

    return result;
}

Util::ValueType64 alternateMakeDecimalRaw64(int mantissa, int exponent)
    // Create a 'ValueType64' object representing a decimal floating point
    // number consisting of the specified 'mantissa' and 'exponent', with the
    // sign given by 'mantissa'.  The behavior is undefined unless
    // 'abs(mantissa) <= 9,999,999,999,999,999' and '-398 <= exponent <= 369'.
    // Note that this contract is identical to the one in the Utility under
    // test.
{
    Util::ValueType64 result;

    BDEC::DenselyPackedDecimalImpUtil::StorageType64 denselyPacked =
       BDEC::DenselyPackedDecimalImpUtil::makeDecimalRaw64(mantissa, exponent);

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    BID_UINT64 temp;
    bsl::memcpy(&temp, &denselyPacked, sizeof(temp));
    BID_UINT64 converted = __bid_dpd_to_bid64(temp);

    bsl::memcpy(&denselyPacked, &converted, sizeof(denselyPacked));
#endif

    bsl::memcpy(&result, &denselyPacked, sizeof(result));

    return result;
}

Util::ValueType64 alternateMakeDecimalRaw64(unsigned int mantissa,
                                                     int exponent)
    // Create a 'ValueType64' object representing a decimal floating point
    // number consisting of the specified 'mantissa' and 'exponent', with the
    // sign given by 'mantissa'.  The behavior is undefined unless
    // 'abs(mantissa) <= 9,999,999,999,999,999' and '-398 <= exponent <= 369'.
    // Note that this contract is identical to the one in the Utility under
    // test.
{
    Util::ValueType64 result;

    BDEC::DenselyPackedDecimalImpUtil::StorageType64 denselyPacked =
       BDEC::DenselyPackedDecimalImpUtil::makeDecimalRaw64(mantissa, exponent);

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    BID_UINT64 temp;
    bsl::memcpy(&temp, &denselyPacked, sizeof(temp));
    BID_UINT64 converted = __bid_dpd_to_bid64(temp);

    bsl::memcpy(&denselyPacked, &converted, sizeof(denselyPacked));
#endif

    bsl::memcpy(&result, &denselyPacked, sizeof(result));

    return result;
}

Util::ValueType64 alternateMakeDecimalRaw64(long long int mantissa,
                                                      int exponent)
    // Create a 'ValueType64' object representing a decimal floating point
    // number consisting of the specified 'mantissa' and 'exponent', with the
    // sign given by 'mantissa'.  The behavior is undefined unless
    // 'abs(mantissa) <= 9,999,999,999,999,999' and '-398 <= exponent <= 369'.
    // Note that this contract is identical to the one in the Utility under
    // test.
{
    Util::ValueType64 result;

    BDEC::DenselyPackedDecimalImpUtil::StorageType64 denselyPacked =
       BDEC::DenselyPackedDecimalImpUtil::makeDecimalRaw64(mantissa, exponent);

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    BID_UINT64 temp;
    bsl::memcpy(&temp, &denselyPacked, sizeof(temp));
    BID_UINT64 converted = __bid_dpd_to_bid64(temp);

    bsl::memcpy(&denselyPacked, &converted, sizeof(denselyPacked));
#endif

    bsl::memcpy(&result, &denselyPacked, sizeof(result));

    return result;
}

Util::ValueType64 alternateMakeDecimalRaw64(unsigned long long int mantissa,
                                                               int exponent)
    // Create a 'ValueType64' object representing a decimal floating point
    // number consisting of the specified 'mantissa' and 'exponent', with the
    // sign given by 'mantissa'.  The behavior is undefined unless
    // 'abs(mantissa) <= 9,999,999,999,999,999' and '-398 <= exponent <= 369'.
    // Note that this contract is identical to the one in the Utility under
    // test.
{
    Util::ValueType64 result;

    BDEC::DenselyPackedDecimalImpUtil::StorageType64 denselyPacked =
       BDEC::DenselyPackedDecimalImpUtil::makeDecimalRaw64(mantissa, exponent);

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    BID_UINT64 temp;
    bsl::memcpy(&temp, &denselyPacked, sizeof(temp));
    BID_UINT64 converted = __bid_dpd_to_bid64(temp);

    bsl::memcpy(&denselyPacked, &converted, sizeof(denselyPacked));
#endif

    bsl::memcpy(&result, &denselyPacked, sizeof(result));

    return result;
}

Util::ValueType128 alternateMakeDecimalRaw128(int mantissa, int exponent)
    // Create a 'ValueType128' object representing a decimal floating point
    // number consisting of the specified 'mantissa' and 'exponent', with the
    // sign given by 'mantissa'.  The behavior is undefined unless
    // '-6176 <= exponent <= 6111'.  Note that this contract is identical to
    // the one in the Utility under test.
{
    Util::ValueType128 result;

    BDEC::DenselyPackedDecimalImpUtil::StorageType128 denselyPacked =
      BDEC::DenselyPackedDecimalImpUtil::makeDecimalRaw128(mantissa, exponent);

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    BID_UINT128 temp;
    bsl::memcpy(&temp, &denselyPacked, sizeof(temp));
    BID_UINT128 converted = __bid_dpd_to_bid128(temp);

    bsl::memcpy(&denselyPacked, &converted, sizeof(denselyPacked));
#endif

    bsl::memcpy(&result, &denselyPacked, sizeof(result));

    return result;
}

Util::ValueType128 alternateMakeDecimalRaw128(unsigned int mantissa,
                                                       int exponent)
    // Create a 'ValueType128' object representing a decimal floating point
    // number consisting of the specified 'mantissa' and 'exponent', with the
    // sign given by 'mantissa'.  The behavior is undefined unless
    // '-6176 <= exponent <= 6111'.  Note that this contract is identical to
    // the one in the Utility under test.
{
    Util::ValueType128 result;

    BDEC::DenselyPackedDecimalImpUtil::StorageType128 denselyPacked =
      BDEC::DenselyPackedDecimalImpUtil::makeDecimalRaw128(mantissa, exponent);

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    BID_UINT128 temp;
    bsl::memcpy(&temp, &denselyPacked, sizeof(temp));
    BID_UINT128 converted = __bid_dpd_to_bid128(temp);

    bsl::memcpy(&denselyPacked, &converted, sizeof(denselyPacked));
#endif

    bsl::memcpy(&result, &denselyPacked, sizeof(result));

    return result;
}

Util::ValueType128 alternateMakeDecimalRaw128(long long int mantissa,
                                                        int exponent)
    // Create a 'ValueType128' object representing a decimal floating point
    // number consisting of the specified 'mantissa' and 'exponent', with the
    // sign given by 'mantissa'.  The behavior is undefined unless
    // '-6176 <= exponent <= 6111'.  Note that this contract is identical to
    // the one in the Utility under test.
{
    Util::ValueType128 result;

    BDEC::DenselyPackedDecimalImpUtil::StorageType128 denselyPacked =
      BDEC::DenselyPackedDecimalImpUtil::makeDecimalRaw128(mantissa, exponent);

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    BID_UINT128 temp;
    bsl::memcpy(&temp, &denselyPacked, sizeof(temp));
    BID_UINT128 converted = __bid_dpd_to_bid128(temp);

    bsl::memcpy(&denselyPacked, &converted, sizeof(denselyPacked));
#endif

    bsl::memcpy(&result, &denselyPacked, sizeof(result));

    return result;
}

Util::ValueType128 alternateMakeDecimalRaw128(unsigned long long int mantissa,
                                                                 int exponent)
    // Create a 'ValueType128' object representing a decimal floating point
    // number consisting of the specified 'mantissa' and 'exponent', with the
    // sign given by 'mantissa'.  The behavior is undefined unless
    // '-6176 <= exponent <= 6111'.  Note that this contract is identical to
    // the one in the Utility under test.
{
    Util::ValueType128 result;

    BDEC::DenselyPackedDecimalImpUtil::StorageType128 denselyPacked =
      BDEC::DenselyPackedDecimalImpUtil::makeDecimalRaw128(mantissa, exponent);

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    BID_UINT128 temp;
    bsl::memcpy(&temp, &denselyPacked, sizeof(temp));
    BID_UINT128 converted = __bid_dpd_to_bid128(temp);

    bsl::memcpy(&denselyPacked, &converted, sizeof(denselyPacked));
#endif

    bsl::memcpy(&result, &denselyPacked, sizeof(result));

    return result;
}

// ============================================================================
//                              USAGE EXAMPLE
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
//                      TEST DRIVER NAMESPACE CLASS
//-----------------------------------------------------------------------------

static int                test;
static int             verbose;
static int         veryVerbose;
static int     veryVeryVerbose;
static int veryVeryVeryVerbose;

struct TestDriver {
    typedef bsls::AssertFailureHandlerGuard AssertFailureHandlerGuard;
    static void testCase23();
    static void testCase22();
    static void testCase21();
    static void testCase20();
    static void testCase19();
    static void testCase18();
    static void testCase17();
    static void testCase16();
    static void testCase15();
    static void testCase14();
    static void testCase13();
    static void testCase12();
    static void testCase11();
    static void testCase10();
    static void testCase9();
    static void testCase8();
    static void testCase7();
    static void testCase6();
    static void testCase5();
    static void testCase4();
    static void testCase3();
    static void testCase2();
    static void testCase1();
};

void TestDriver::testCase23()
{
    // ------------------------------------------------------------------------
    // TESTING USAGE EXAMPLE
    //
    // Concerns:
    //:  1 Usage example compiles
    //
    // Plan:
    //:  1 Copy-and-paste usage example into test driver.
    //:  2 Replace 'assert' with 'ASSERT'.
    //
    // Testing:
    //   USAGE EXAMPLE
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TESTING USAGE EXAMPLE" << endl
                      << "=====================" << endl;

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

    int coefficient = 425; // Yet another name for significand
    int exponent    =  -1;

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
}

void TestDriver::testCase22()
{
    // ------------------------------------------------------------------------
    // TESTING CONVERSION FROM BINARY INTEGRAL
    //
    // Concerns:
    //:  1 Values in the implementation format are created faithfully and
    //:    correctly when converted from the BID format.
    //:
    //:  2 The value, after conversion, has the same cohort as the original
    //:    value.
    //:
    //:  3 32, 64, and 128 bit variations all work correctly.
    //:
    //:  4 Special cases, such as Infinity and NaN are represented correctly as
    //:    BID.
    //:
    //:  5 The implementation format values correctly survive round-trip
    //:    encoding.
    //
    // Plan:
    //:  1 Iterate through a set of test mantissa and convert them to BID, then
    //:    convert back to the implementation format, comparing the results.
    //:    (C-1..3)
    //:
    //:  2 Test the special case values explicitly, by converting to BID, then
    //:    using the conversion back.  Check that the properties of these
    //:    special cases hold.  (C-2..5)
    //
    // Testing
    //   convertFromBID(ValueType32)
    //   convertFromBID(ValueType64)
    //   convertFromBID(ValueType128)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TESTING CONVERSION FROM BINARY INTEGRAL" << endl
                      << "=======================================" << endl;

    typedef BloombergLP::bdldfp::BinaryIntegralDecimalImpUtil BidUtil;

    // Test that with any of a set of exponents, we can convert 32-bit
    // values correctly to BID.

    // IMPLEMENTATION NOTE: This test case currently assumes that the
    // underlying implementation of Decimal arithmetic is 'BID' not 'DPD'.
    // This test driver will need to be updated to handle 'DPD'
    // implementations.

    {
           Util::  ValueType32 testDecimal;
        BidUtil::StorageType32 witnessBID;
           Util::  ValueType32 witness;

                    // 0e0

        testDecimal = Util::makeDecimalRaw32(0, 0);
        witnessBID  = Util::convertToBID(testDecimal);
        witness     = Util::convertFromBID(witnessBID);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // 7e0

        testDecimal = Util::makeDecimalRaw32(7, 0);
        witnessBID  = Util::convertToBID(testDecimal);
        witness     = Util::convertFromBID(witnessBID);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // 52e0

        testDecimal = Util::makeDecimalRaw32(52, 0);
        witnessBID  = Util::convertToBID(testDecimal);
        witness     = Util::convertFromBID(witnessBID);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // Table driven test for conversion

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose)
                         cout << "convertFromBID, mantissa num: "
                                << t_m << ", " << mantissas[t_m] << endl;

            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose)
                         cout << "convertFromBID, exponent num: "
                                << t_e << ", "
                                << exponents[t_e] << endl;

                long long int t_mantissa = mantissas[t_m];
                          int   exponent = exponents[t_e];

                if (t_mantissa <= 9999999 && t_mantissa >= -9999999
                 &&   exponent <= 90      &&   exponent >= -101) {

                    int mantissa = static_cast<int>(t_mantissa);

                    testDecimal = Util::makeDecimalRaw32(mantissa,
                                                         exponent);

                    witnessBID = Util::convertToBID(testDecimal);
                    witness = Util::convertFromBID(witnessBID);

                    LOOP4_ASSERT(t_m, t_e, mantissa, exponent,
                                 !bsl::memcmp(&testDecimal,
                                              &witness,
                                               sizeof(witness)));
                }
            }
        }

        Util::ValueType32 specialWitness;

        testDecimal    = Util::parse32("NaN");
        witnessBID     = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(witnessBID);

        ASSERT(!Util::equal(testDecimal,    testDecimal));
        ASSERT(!Util::equal(specialWitness, specialWitness));

        testDecimal    = Util::parse32("+Inf");
        witnessBID     = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(witnessBID);

        ASSERT( Util::equal(testDecimal, specialWitness));

        testDecimal    = Util::parse32("-Inf");
        witnessBID     = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(witnessBID);

        ASSERT( Util::equal(testDecimal, specialWitness));
    }

    // Test that with any of a set of exponents, we can convert 64-bit
    // values correctly to DPD.

    {
           Util::  ValueType64 testDecimal;
        BidUtil::StorageType64 witnessBID;
           Util::  ValueType64 witness;

                    // 0e0

        testDecimal = Util::makeDecimalRaw64(0, 0);
        witnessBID  = Util::convertToBID(testDecimal);
        witness     = Util::convertFromBID(witnessBID);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // 7e0

        testDecimal = Util::makeDecimalRaw64(7, 0);
        witnessBID  = Util::convertToBID(testDecimal);
        witness     = Util::convertFromBID(witnessBID);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // 52e0

        testDecimal = Util::makeDecimalRaw64(52, 0);
        witnessBID  = Util::convertToBID(testDecimal);
        witness     = Util::convertFromBID(witnessBID);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // Table driven test for conversion

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose)
                         cout << "convertFromBID, mantissa num: "
                                << t_m << ", " << mantissas[t_m] << endl;

            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose)
                         cout << "convertFromBID, exponent num: "
                                << t_e << ", "
                                << exponents[t_e] << endl;

                long long int t_mantissa = mantissas[t_m];
                          int   exponent = exponents[t_e];

                if (t_mantissa <= 9999999 && t_mantissa >= -9999999
                 &&   exponent <= 90      &&   exponent >= -101) {

                    int mantissa = static_cast<int>(t_mantissa);

                    testDecimal = Util::makeDecimalRaw64(mantissa,
                                                         exponent);

                    witnessBID = Util::convertToBID(testDecimal);
                    witness = Util::convertFromBID(witnessBID);

                    LOOP4_ASSERT(t_m, t_e, mantissa, exponent,
                                 !bsl::memcmp(&testDecimal,
                                              &witness,
                                               sizeof(witness)));
                }
            }
        }

        Util::ValueType64 specialWitness;

        testDecimal    = Util::parse64("NaN");
        witnessBID     = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(witnessBID);

        ASSERT(!Util::equal(testDecimal,    testDecimal));
        ASSERT(!Util::equal(specialWitness, specialWitness));

        testDecimal    = Util::parse64("+Inf");
        witnessBID     = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(witnessBID);

        ASSERT( Util::equal(testDecimal, specialWitness));

        testDecimal    = Util::parse64("-Inf");
        witnessBID     = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(witnessBID);

        ASSERT( Util::equal(testDecimal, specialWitness));
    }

    // Test that with any of a set of exponents, we can convert 128-bit
    // values correctly to DPD.

    {
           Util::  ValueType128 testDecimal;
        BidUtil::StorageType128 witnessBID;
           Util::  ValueType128 witness;

                    // 0e0

        testDecimal = Util::makeDecimalRaw128(0, 0);
        witnessBID  = Util::convertToBID(testDecimal);
        witness     = Util::convertFromBID(witnessBID);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // 7e0

        testDecimal = Util::makeDecimalRaw128(7, 0);
        witnessBID  = Util::convertToBID(testDecimal);
        witness     = Util::convertFromBID(witnessBID);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // 52e0

        testDecimal = Util::makeDecimalRaw128(52, 0);
        witnessBID  = Util::convertToBID(testDecimal);
        witness     = Util::convertFromBID(witnessBID);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // Table driven test for conversion

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose)
                         cout << "convertFromBID, mantissa num: "
                                << t_m << ", " << mantissas[t_m] << endl;

            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose)
                         cout << "convertFromBID, exponent num: "
                                << t_e << ", "
                                << exponents[t_e] << endl;

                long long int t_mantissa = mantissas[t_m];
                          int   exponent = exponents[t_e];

                if (t_mantissa <= 9999999 && t_mantissa >= -9999999
                 &&   exponent <= 90      &&   exponent >= -101) {

                    int mantissa = static_cast<int>(t_mantissa);

                    testDecimal = Util::makeDecimalRaw128(mantissa,
                                                         exponent);

                    witnessBID = Util::convertToBID(testDecimal);
                    witness = Util::convertFromBID(witnessBID);

                    LOOP4_ASSERT(t_m, t_e, mantissa, exponent,
                                 !bsl::memcmp(&testDecimal,
                                              &witness,
                                               sizeof(witness)));
                }
            }
        }

        Util::ValueType128 specialWitness;

        testDecimal    = Util::parse128("NaN");
        witnessBID     = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(witnessBID);

        ASSERT(!Util::equal(testDecimal,    testDecimal));
        ASSERT(!Util::equal(specialWitness, specialWitness));

        testDecimal    = Util::parse128("+Inf");
        witnessBID     = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(witnessBID);

        ASSERT( Util::equal(testDecimal, specialWitness));

        testDecimal    = Util::parse128("-Inf");
        witnessBID     = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(witnessBID);

        ASSERT( Util::equal(testDecimal, specialWitness));
    }
}

void TestDriver::testCase21()
{
    // ------------------------------------------------------------------------
    // TESTING CONVERSION TO BINARY INTEGRAL
    //
    // Concerns:
    //:  1 Values in the implementation format are represented faithfully and
    //:    correctly in the BID format, when converted.
    //:
    //:  2 The value, after conversion, has the same cohort as the original
    //:    value.
    //:
    //:  3 32, 64, and 128 bit variations all work correctly.
    //:
    //:  4 Special cases, such as Infinity and NaN are represented correctly as
    //:    BID.
    //
    // Plan:
    //:  1 Iterate through a set of test mantissa and convert them to BID,
    //:    comparing the result to a canonically constructed value in BID.
    //:    (C-1..3)
    //:
    //:  2 Test the special case values explicitly, by converting to BID, then
    //:    using the conversion back.  Check that the properties of these
    //:    special cases hold. (C-3,4)
    //
    // Testing
    //   convertToBID(ValueType32)
    //   convertToBID(ValueType64)
    //   convertToBID(ValueType128)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TESTING CONVERSION TO BINARY INTEGRAL" << endl
                      << "=====================================" << endl;

    typedef BloombergLP::bdldfp::BinaryIntegralDecimalImpUtil BidUtil;

    // Test that with any of a set of exponents, we can convert 32-bit
    // values correctly to BID.

    // IMPLEMENTATION NOTE: This test case currently assumes that the
    // underlying implementation of Decimal arithmetic is 'BID' not 'DPD'.
    // This test driver will need to be updated to handle 'DPD'
    // implementations.

    {
           Util::  ValueType32 testDecimal;
        BidUtil::StorageType32 testConvert;
           Util::  ValueType32 witness;

                    // 0e0

        witness     =    Util::makeDecimalRaw32(0, 0);
        testDecimal =    Util::makeDecimalRaw32(0, 0);
        testConvert =    Util::convertToBID(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // 7e0

        witness     =    Util::makeDecimalRaw32(7, 0);
        testDecimal =    Util::makeDecimalRaw32(7, 0);
        testConvert =    Util::convertToBID(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // 52e0

        witness     =    Util::makeDecimalRaw32(52, 0);
        testDecimal =    Util::makeDecimalRaw32(52, 0);
        testConvert =    Util::convertToBID(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // Table driven test for conversion

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose)
                           cout << "convertToBID, mantissa num: "
                                << t_m << ", " << mantissas[t_m] << endl;

            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose)
                           cout << "convertToBID, exponent num: "
                                << t_e << ", "
                                << exponents[t_e] << endl;

                long long int t_mantissa = mantissas[t_m];
                          int   exponent = exponents[t_e];

                if (t_mantissa <= 9999999 && t_mantissa >= -9999999
                 &&   exponent <= 90      &&   exponent >= -101) {

                    int mantissa = static_cast<int>(t_mantissa);

                    witness     =    Util::makeDecimalRaw32(mantissa,
                                                            exponent);
                    testDecimal =    Util::makeDecimalRaw32(mantissa,
                                                            exponent);
                    testConvert =    Util::convertToBID(
                                                              testDecimal);

                    LOOP4_ASSERT(t_m, t_e, mantissa, exponent,
                                 !bsl::memcmp(&testConvert,
                                              &witness,
                                               sizeof(testConvert)));

                }
            }
        }

        Util::ValueType32 specialWitness;

        testDecimal    = Util::parse32("NaN");
        testConvert    = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(testConvert);

        ASSERT(!Util::equal(testDecimal,    testDecimal));
        ASSERT(!Util::equal(specialWitness, specialWitness));

        testDecimal    = Util::parse32("+Inf");
        testConvert    = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(testConvert);

        ASSERT( Util::equal(testDecimal, specialWitness));

        testDecimal    = Util::parse32("-Inf");
        testConvert    = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(testConvert);

        ASSERT( Util::equal(testDecimal, specialWitness));
    }

    // Test that with any of a set of exponents, we can convert 64-bit
    // values correctly to DPD.

    {
           Util::  ValueType64 testDecimal;
        BidUtil::StorageType64 testConvert;
           Util::  ValueType64 witness;

                    // 0e0

        witness     =    Util::makeDecimalRaw64(0, 0);
        testDecimal =    Util::makeDecimalRaw64(0, 0);
        testConvert =    Util::convertToBID(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // 7e0

        witness     =    Util::makeDecimalRaw64(7, 0);
        testDecimal =    Util::makeDecimalRaw64(7, 0);
        testConvert =    Util::convertToBID(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // 52e0

        witness     =    Util::makeDecimalRaw64(52, 0);
        testDecimal =    Util::makeDecimalRaw64(52, 0);
        testConvert =    Util::convertToBID(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // Table driven test for conversion

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose)
                           cout << "convertToBID, mantissa num: "
                                << t_m << ", " << mantissas[t_m] << endl;

            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose)
                           cout << "convertToBID, exponent num: "
                                << t_e << ", "
                                << exponents[t_e] << endl;

                long long int mantissa = mantissas[t_m];
                          int exponent = exponents[t_e];

                if (mantissa <=  9999999999999999ll &&
                    mantissa >= -9999999999999999ll &&
                    exponent <= 369 && exponent >= -398) {

                    witness     =    Util::makeDecimalRaw64(mantissa,
                                                            exponent);
                    testDecimal =    Util::makeDecimalRaw64(mantissa,
                                                            exponent);
                    testConvert =    Util::convertToBID(
                                                              testDecimal);

                    LOOP4_ASSERT(t_m, t_e, mantissa, exponent,
                                 !bsl::memcmp(&testConvert,
                                              &witness,
                                               sizeof(testConvert)));
                }
            }
        }

        Util::ValueType64 specialWitness;

        testDecimal    = Util::parse64("NaN");
        testConvert    = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(testConvert);

        ASSERT(!Util::equal(testDecimal,    testDecimal));
        ASSERT(!Util::equal(specialWitness, specialWitness));

        testDecimal    = Util::parse64("+Inf");
        testConvert    = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(testConvert);

        ASSERT( Util::equal(testDecimal, specialWitness));

        testDecimal    = Util::parse64("-Inf");
        testConvert    = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(testConvert);

        ASSERT( Util::equal(testDecimal, specialWitness));
    }

    // Test that with any of a set of exponents, we can convert 128-bit
    // values correctly to DPD.

    {
           Util::  ValueType128 testDecimal;
        BidUtil::StorageType128 testConvert;
           Util::  ValueType128 witness;

                    // 0e0

        witness     =    Util::makeDecimalRaw128(0, 0);
        testDecimal =    Util::makeDecimalRaw128(0, 0);
        testConvert =    Util::convertToBID(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // 7e0

        witness     =    Util::makeDecimalRaw128(7, 0);
        testDecimal =    Util::makeDecimalRaw128(7, 0);
        testConvert =    Util::convertToBID(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // 52e0

        witness     =    Util::makeDecimalRaw128(52, 0);
        testDecimal =    Util::makeDecimalRaw128(52, 0);
        testConvert =    Util::convertToBID(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // Table driven test for conversion

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose)
                           cout << "convertToBID, mantissa num: "
                                << t_m << ", " << mantissas[t_m] << endl;

            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose)
                           cout << "convertToBID, exponent num: "
                                << t_e << ", "
                                << exponents[t_e] << endl;

                long long int mantissa = mantissas[t_m];
                          int exponent = exponents[t_e];

                if (exponent <= 6111 && exponent >= -6176) {

                    witness     =    Util::makeDecimalRaw128(mantissa,
                                                            exponent);
                    testDecimal =    Util::makeDecimalRaw128(mantissa,
                                                            exponent);
                    testConvert =    Util::convertToBID(
                                                              testDecimal);

                    LOOP4_ASSERT(t_m, t_e, mantissa, exponent,
                                 !bsl::memcmp(&testConvert,
                                              &witness,
                                               sizeof(testConvert)));
                }
            }
        }

        Util::ValueType128 specialWitness;

        testDecimal    = Util::parse128("NaN");
        testConvert    = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(testConvert);

        ASSERT(!Util::equal(testDecimal,    testDecimal));
        ASSERT(!Util::equal(specialWitness, specialWitness));

        testDecimal    = Util::parse128("+Inf");
        testConvert    = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(testConvert);

        ASSERT( Util::equal(testDecimal, specialWitness));

        testDecimal    = Util::parse128("-Inf");
        testConvert    = Util::convertToBID(testDecimal);
        specialWitness = Util::convertFromBID(testConvert);

        ASSERT( Util::equal(testDecimal, specialWitness));
    }
}

void TestDriver::testCase20()
{
    // ------------------------------------------------------------------------
    // TESTING CONVERSION FROM DENSELY PACKED
    //
    // Concerns:
    //:  1 Values in the implementation format are created faithfully and
    //:    correctly when converted from  the DPD format.
    //:
    //:  2 The value, after conversion, has the same cohort as the original
    //:    value.
    //:
    //:  3 32, 64, and 128 bit variations all work correctly.
    //:
    //:  4 Special cases, such as Infinity and NaN are represented correctly as
    //:    DPD.
    //:
    //:  5 The implementation format values correctly survive round-trip
    //:    encoding.
    //
    // Plan:
    //:  1 Iterate through a set of test mantissa and convert them to DPD, then
    //:    convert back to the implementation format, comparing the results.
    //:    (C-1..3)
    //:
    //:  2 Test the special case values explicitly, by converting to DPD, then
    //:    using the conversion back.  Check that the properties of these
    //:    special cases hold.  (C-2..5)
    //
    // Testing
    //   convertFromDPD(ValueType32)
    //   convertFromDPD(ValueType64)
    //   convertFromDPD(ValueType128)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TESTING CONVERSION FROM DENSELY PACKED" << endl
                      << "======================================" << endl;

    typedef BloombergLP::bdldfp::DenselyPackedDecimalImpUtil DpdUtil;

    // Test that with any of a set of exponents, we can convert 32-bit
    // values correctly to DPD.

    {
           Util::  ValueType32 testDecimal;
        DpdUtil::StorageType32 witnessDPD;
           Util::  ValueType32 witness;

                    // 0e0

        testDecimal = Util::makeDecimalRaw32(0, 0);
        witnessDPD  = Util::convertToDPD(testDecimal);
        witness     = Util::convertFromDPD(witnessDPD);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // 7e0

        testDecimal = Util::makeDecimalRaw32(7, 0);
        witnessDPD  = Util::convertToDPD(testDecimal);
        witness     = Util::convertFromDPD(witnessDPD);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // 52e0

        testDecimal = Util::makeDecimalRaw32(52, 0);
        witnessDPD  = Util::convertToDPD(testDecimal);
        witness     = Util::convertFromDPD(witnessDPD);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // Table driven test for conversion

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose)
                         cout << "convertFromDPD, mantissa num: "
                                << t_m << ", " << mantissas[t_m] << endl;

            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose)
                         cout << "convertFromDPD, exponent num: "
                                << t_e << ", "
                                << exponents[t_e] << endl;

                long long int t_mantissa = mantissas[t_m];
                          int   exponent = exponents[t_e];

                if (t_mantissa <= 9999999 && t_mantissa >= -9999999
                 &&   exponent <= 90      &&   exponent >= -101) {

                    int mantissa = static_cast<int>(t_mantissa);

                    testDecimal = Util::makeDecimalRaw32(mantissa,
                                                         exponent);

                    witnessDPD = Util::convertToDPD(testDecimal);
                    witness = Util::convertFromDPD(witnessDPD);

                    LOOP4_ASSERT(t_m, t_e, mantissa, exponent,
                                 !bsl::memcmp(&testDecimal,
                                              &witness,
                                               sizeof(witness)));
                }
            }
        }

        Util::ValueType32 specialWitness;

        testDecimal    = Util::parse32("NaN");
        witnessDPD     = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(witnessDPD);

        ASSERT(!Util::equal(testDecimal,    testDecimal));
        ASSERT(!Util::equal(specialWitness, specialWitness));

        testDecimal    = Util::parse32("+Inf");
        witnessDPD     = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(witnessDPD);

        ASSERT( Util::equal(testDecimal, specialWitness));

        testDecimal    = Util::parse32("-Inf");
        witnessDPD     = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(witnessDPD);

        ASSERT( Util::equal(testDecimal, specialWitness));
    }

    // Test that with any of a set of exponents, we can convert 64-bit
    // values correctly to DPD.

    {
           Util::  ValueType64 testDecimal;
        DpdUtil::StorageType64 witnessDPD;
           Util::  ValueType64 witness;

                    // 0e0

        testDecimal = Util::makeDecimalRaw64(0, 0);
        witnessDPD  = Util::convertToDPD(testDecimal);
        witness     = Util::convertFromDPD(witnessDPD);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // 7e0

        testDecimal = Util::makeDecimalRaw64(7, 0);
        witnessDPD  = Util::convertToDPD(testDecimal);
        witness     = Util::convertFromDPD(witnessDPD);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // 52e0

        testDecimal = Util::makeDecimalRaw64(52, 0);
        witnessDPD  = Util::convertToDPD(testDecimal);
        witness     = Util::convertFromDPD(witnessDPD);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // Table driven test for conversion

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose)
                         cout << "convertFromDPD, mantissa num: "
                                << t_m << ", " << mantissas[t_m] << endl;

            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose)
                         cout << "convertFromDPD, exponent num: "
                                << t_e << ", "
                                << exponents[t_e] << endl;

                long long int t_mantissa = mantissas[t_m];
                          int   exponent = exponents[t_e];

                if (t_mantissa <= 9999999 && t_mantissa >= -9999999
                 &&   exponent <= 90      &&   exponent >= -101) {

                    int mantissa = static_cast<int>(t_mantissa);

                    testDecimal = Util::makeDecimalRaw64(mantissa,
                                                         exponent);

                    witnessDPD = Util::convertToDPD(testDecimal);
                    witness = Util::convertFromDPD(witnessDPD);

                    LOOP4_ASSERT(t_m, t_e, mantissa, exponent,
                                 !bsl::memcmp(&testDecimal,
                                              &witness,
                                               sizeof(witness)));
                }
            }
        }

        Util::ValueType64 specialWitness;

        testDecimal    = Util::parse64("NaN");
        witnessDPD     = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(witnessDPD);

        ASSERT(!Util::equal(testDecimal,    testDecimal));
        ASSERT(!Util::equal(specialWitness, specialWitness));

        testDecimal    = Util::parse64("+Inf");
        witnessDPD     = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(witnessDPD);

        ASSERT( Util::equal(testDecimal, specialWitness));

        testDecimal    = Util::parse64("-Inf");
        witnessDPD     = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(witnessDPD);

        ASSERT( Util::equal(testDecimal, specialWitness));
    }

    // Test that with any of a set of exponents, we can convert 128-bit
    // values correctly to DPD.

    {
           Util::  ValueType128 testDecimal;
        DpdUtil::StorageType128 witnessDPD;
           Util::  ValueType128 witness;

                    // 0e0

        testDecimal = Util::makeDecimalRaw128(0, 0);
        witnessDPD  = Util::convertToDPD(testDecimal);
        witness     = Util::convertFromDPD(witnessDPD);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // 7e0

        testDecimal = Util::makeDecimalRaw128(7, 0);
        witnessDPD  = Util::convertToDPD(testDecimal);
        witness     = Util::convertFromDPD(witnessDPD);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // 52e0

        testDecimal = Util::makeDecimalRaw128(52, 0);
        witnessDPD  = Util::convertToDPD(testDecimal);
        witness     = Util::convertFromDPD(witnessDPD);

        ASSERT(!bsl::memcmp(&testDecimal, &witness, sizeof(witness)));

                    // Table driven test for conversion

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose)
                         cout << "convertFromDPD, mantissa num: "
                                << t_m << ", " << mantissas[t_m] << endl;

            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose)
                         cout << "convertFromDPD, exponent num: "
                                << t_e << ", "
                                << exponents[t_e] << endl;

                long long int t_mantissa = mantissas[t_m];
                          int   exponent = exponents[t_e];

                if (t_mantissa <= 9999999 && t_mantissa >= -9999999
                 &&   exponent <= 90      &&   exponent >= -101) {

                    int mantissa = static_cast<int>(t_mantissa);

                    testDecimal = Util::makeDecimalRaw128(mantissa,
                                                         exponent);

                    witnessDPD = Util::convertToDPD(testDecimal);
                    witness = Util::convertFromDPD(witnessDPD);

                    LOOP4_ASSERT(t_m, t_e, mantissa, exponent,
                                 !bsl::memcmp(&testDecimal,
                                              &witness,
                                               sizeof(witness)));
                }
            }
        }

        Util::ValueType128 specialWitness;

        testDecimal    = Util::parse128("NaN");
        witnessDPD     = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(witnessDPD);

        ASSERT(!Util::equal(testDecimal,    testDecimal));
        ASSERT(!Util::equal(specialWitness, specialWitness));

        testDecimal    = Util::parse128("+Inf");
        witnessDPD     = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(witnessDPD);

        ASSERT( Util::equal(testDecimal, specialWitness));

        testDecimal    = Util::parse128("-Inf");
        witnessDPD     = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(witnessDPD);

        ASSERT( Util::equal(testDecimal, specialWitness));
    }
}

void TestDriver::testCase19()
{
    // ------------------------------------------------------------------------
    // TESTING CONVERSION TO DENSELY PACKED
    //
    // Concerns:
    //:  1 Values in the implementation format are represented faithfully and
    //:    correctly in the DPD format, when converted.
    //:
    //:  2 The value, after conversion, has the same cohort as the original
    //:    value.
    //:
    //:  3 32, 64, and 128 bit variations all work correctly.
    //:
    //:  4 Special cases, such as Infinity and NaN are represented correctly as
    //:    DPD.
    //
    // Plan:
    //:  1 Iterate through a set of test mantissa and convert them to DPD,
    //:    comparing the result to a canonically constructed value in DPD.
    //:    (C-1..3)
    //:
    //:  2 Test the special case values explicitly, by converting to DPD, then
    //:    using the conversion back.  Check that the properties of these
    //:    special cases hold. (C-3,4)
    //
    // Testing
    //   convertToDPD(ValueType32)
    //   convertToDPD(ValueType64)
    //   convertToDPD(ValueType128)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TESTING CONVERSION TO DENSELY PACKED" << endl
                      << "====================================" << endl;

    typedef BloombergLP::bdldfp::DenselyPackedDecimalImpUtil DpdUtil;

    // Test that with any of a set of exponents, we can convert 32-bit
    // values correctly to DPD.

    {
           Util::  ValueType32 testDecimal;
        DpdUtil::StorageType32 testConvert;
        DpdUtil::StorageType32 witness;

                    // 0e0

        witness     = DpdUtil::makeDecimalRaw32(0, 0);
        testDecimal =    Util::makeDecimalRaw32(0, 0);
        testConvert =    Util::convertToDPD(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // 7e0

        witness     = DpdUtil::makeDecimalRaw32(7, 0);
        testDecimal =    Util::makeDecimalRaw32(7, 0);
        testConvert =    Util::convertToDPD(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // 52e0

        witness     = DpdUtil::makeDecimalRaw32(52, 0);
        testDecimal =    Util::makeDecimalRaw32(52, 0);
        testConvert =    Util::convertToDPD(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // Table driven test for conversion

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose)
                           cout << "convertToDPD, mantissa num: "
                                << t_m << ", " << mantissas[t_m] << endl;

            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose)
                           cout << "convertToDPD, exponent num: "
                                << t_e << ", "
                                << exponents[t_e] << endl;

                long long int t_mantissa = mantissas[t_m];
                          int   exponent = exponents[t_e];

                if (t_mantissa <= 9999999 && t_mantissa >= -9999999
                 &&   exponent <= 90      &&   exponent >= -101) {

                    int mantissa = static_cast<int>(t_mantissa);

                    witness     = DpdUtil::makeDecimalRaw32(mantissa,
                                                            exponent);
                    testDecimal =    Util::makeDecimalRaw32(mantissa,
                                                            exponent);
                    testConvert =    Util::convertToDPD(
                                                              testDecimal);

                    LOOP4_ASSERT(t_m, t_e, mantissa, exponent,
                                 !bsl::memcmp(&testConvert,
                                              &witness,
                                               sizeof(testConvert)));

                }
            }
        }

        Util::ValueType32 specialWitness;

        testDecimal    = Util::parse32("NaN");
        testConvert    = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(testConvert);

        ASSERT(!Util::equal(testDecimal,    testDecimal));
        ASSERT(!Util::equal(specialWitness, specialWitness));

        testDecimal    = Util::parse32("+Inf");
        testConvert    = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(testConvert);

        ASSERT( Util::equal(testDecimal, specialWitness));

        testDecimal    = Util::parse32("-Inf");
        testConvert    = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(testConvert);

        ASSERT( Util::equal(testDecimal, specialWitness));
    }

    // Test that with any of a set of exponents, we can convert 64-bit
    // values correctly to DPD.

    {
           Util::  ValueType64 testDecimal;
        DpdUtil::StorageType64 testConvert;
        DpdUtil::StorageType64 witness;

                    // 0e0

        witness     = DpdUtil::makeDecimalRaw64(0, 0);
        testDecimal =    Util::makeDecimalRaw64(0, 0);
        testConvert =    Util::convertToDPD(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // 7e0

        witness     = DpdUtil::makeDecimalRaw64(7, 0);
        testDecimal =    Util::makeDecimalRaw64(7, 0);
        testConvert =    Util::convertToDPD(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // 52e0

        witness     = DpdUtil::makeDecimalRaw64(52, 0);
        testDecimal =    Util::makeDecimalRaw64(52, 0);
        testConvert =    Util::convertToDPD(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // Table driven test for conversion

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose)
                           cout << "convertToDPD, mantissa num: "
                                << t_m << ", " << mantissas[t_m] << endl;

            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose)
                           cout << "convertToDPD, exponent num: "
                                << t_e << ", "
                                << exponents[t_e] << endl;

                long long int mantissa = mantissas[t_m];
                          int exponent = exponents[t_e];

                if (mantissa <=  9999999999999999ll &&
                    mantissa >= -9999999999999999ll &&
                    exponent <= 369 && exponent >= -398) {

                    witness     = DpdUtil::makeDecimalRaw64(mantissa,
                                                            exponent);
                    testDecimal =    Util::makeDecimalRaw64(mantissa,
                                                            exponent);
                    testConvert =    Util::convertToDPD(
                                                              testDecimal);

                    LOOP4_ASSERT(t_m, t_e, mantissa, exponent,
                                 !bsl::memcmp(&testConvert,
                                              &witness,
                                               sizeof(testConvert)));
                }
            }
        }

        Util::ValueType64 specialWitness;

        testDecimal    = Util::parse64("NaN");
        testConvert    = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(testConvert);

        ASSERT(!Util::equal(testDecimal,    testDecimal));
        ASSERT(!Util::equal(specialWitness, specialWitness));

        testDecimal    = Util::parse64("+Inf");
        testConvert    = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(testConvert);

        ASSERT( Util::equal(testDecimal, specialWitness));

        testDecimal    = Util::parse64("-Inf");
        testConvert    = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(testConvert);

        ASSERT( Util::equal(testDecimal, specialWitness));
    }

    // Test that with any of a set of exponents, we can convert 128-bit
    // values correctly to DPD.

    {
           Util::  ValueType128 testDecimal;
        DpdUtil::StorageType128 testConvert;
        DpdUtil::StorageType128 witness;

                    // 0e0

        witness     = DpdUtil::makeDecimalRaw128(0, 0);
        testDecimal =    Util::makeDecimalRaw128(0, 0);
        testConvert =    Util::convertToDPD(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // 7e0

        witness     = DpdUtil::makeDecimalRaw128(7, 0);
        testDecimal =    Util::makeDecimalRaw128(7, 0);
        testConvert =    Util::convertToDPD(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // 52e0

        witness     = DpdUtil::makeDecimalRaw128(52, 0);
        testDecimal =    Util::makeDecimalRaw128(52, 0);
        testConvert =    Util::convertToDPD(testDecimal);

        ASSERT(!bsl::memcmp(&testConvert, &witness, sizeof(testConvert)));

                    // Table driven test for conversion

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose)
                           cout << "convertToDPD, mantissa num: "
                                << t_m << ", " << mantissas[t_m] << endl;

            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose)
                           cout << "convertToDPD, exponent num: "
                                << t_e << ", "
                                << exponents[t_e] << endl;

                long long int mantissa = mantissas[t_m];
                          int exponent = exponents[t_e];

                if (exponent <= 6111 && exponent >= -6176) {

                    witness     = DpdUtil::makeDecimalRaw128(mantissa,
                                                            exponent);
                    testDecimal =    Util::makeDecimalRaw128(mantissa,
                                                            exponent);
                    testConvert =    Util::convertToDPD(
                                                              testDecimal);

                    LOOP4_ASSERT(t_m, t_e, mantissa, exponent,
                                 !bsl::memcmp(&testConvert,
                                              &witness,
                                               sizeof(testConvert)));
                }
            }
        }

        Util::ValueType128 specialWitness;

        testDecimal    = Util::parse128("NaN");
        testConvert    = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(testConvert);

        ASSERT(!Util::equal(testDecimal,    testDecimal));
        ASSERT(!Util::equal(specialWitness, specialWitness));

        testDecimal    = Util::parse128("+Inf");
        testConvert    = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(testConvert);

        ASSERT( Util::equal(testDecimal, specialWitness));

        testDecimal    = Util::parse128("-Inf");
        testConvert    = Util::convertToDPD(testDecimal);
        specialWitness = Util::convertFromDPD(testConvert);

        ASSERT( Util::equal(testDecimal, specialWitness));
    }
}

void TestDriver::testCase18()
{
    // ------------------------------------------------------------------------
    // TESTING 'format'
    //
    // Concerns:
    //:  1 Formatted output accurately represents the value of a Decimal in
    //:    some human readable form.
    //:
    //:  2 Formatted output can be re-read as a Decimal with the same value.
    //
    // Plan:
    //:  1 Output will be formatted, and then reloaded
    //
    // Testing:
    //   format(ValueType32,  char *)
    //   format(ValueType64,  char *)
    //   format(ValueType128, char *)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TESTING 'format'" << endl
                      << "================" << endl;

    char buffer[BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE];

    // Testing 'format(ValueType32, char *)'

    {
        Util::ValueType32 value;
        Util::ValueType32 test;

        value = Util::makeDecimalRaw32(0, 0);
        Util::format(value, buffer);
        test  = Util::parse32(buffer);
        LOOP_ASSERT(buffer, Util::equal(value, test));

        value = Util::makeDecimalRaw32(-1, 0);
        Util::format(value, buffer);
        test  = Util::parse32(buffer);
        LOOP_ASSERT(buffer, Util::equal(value, test));

        value = Util::makeDecimalRaw32(42, 0);
        Util::format(value, buffer);
        test  = Util::parse32(buffer);
        LOOP_ASSERT(buffer, Util::equal(value, test));

        value = Util::makeDecimalRaw32(-42, 0);
        Util::format(value, buffer);
        test  = Util::parse32(buffer);
        LOOP_ASSERT(buffer, Util::equal(value, test));

        value = Util::makeDecimalRaw32(42, 5);
        Util::format(value, buffer);
        test  = Util::parse32(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw32(-42, 5);
        Util::format(value, buffer);
        test  = Util::parse32(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw32(42, -17);
        Util::format(value, buffer);
        test  = Util::parse32(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw32(-42, -17);
        Util::format(value, buffer);
        test  = Util::parse32(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::convertToDecimal32(Util::makeInfinity64(false));
        Util::format(value, buffer);
        test  = Util::parse32(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::convertToDecimal32(Util::makeInfinity64(true));
        Util::format(value, buffer);
        test  = Util::parse32(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::parse32("NaN");
        Util::format(value, buffer);
        test  = Util::parse32(buffer);
        LOOP_ASSERT(buffer,!Util::equal(test, test));
    }

    // Testing 'format(ValueType64, char *)'

    {
        Util::ValueType64 value;
        Util::ValueType64 test;

        value = Util::makeDecimalRaw64(0, 0);
        Util::format(value, buffer);
        test  = Util::parse64(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw64(-1, 0);
        Util::format(value, buffer);
        test  = Util::parse64(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw64(42, 0);
        Util::format(value, buffer);
        test  = Util::parse64(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw64(-42, 0);
        Util::format(value, buffer);
        test  = Util::parse64(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw64(42, 5);
        Util::format(value, buffer);
        test  = Util::parse64(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw64(-42, 5);
        Util::format(value, buffer);
        test  = Util::parse64(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw64(42, -17);
        Util::format(value, buffer);
        test  = Util::parse64(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw64(-42, -17);
        Util::format(value, buffer);
        test  = Util::parse64(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeInfinity64(false);
        Util::format(value, buffer);
        test  = Util::parse64(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeInfinity64(true);
        Util::format(value, buffer);
        test  = Util::parse64(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::parse64("NaN");
        Util::format(value, buffer);
        test  = Util::parse64(buffer);
        LOOP_ASSERT(buffer,!Util::equal(test, test));
    }

    // Testing 'format(ValueType128, char *)'

    {
        Util::ValueType128 value;
        Util::ValueType128 test;

        value = Util::makeDecimalRaw128(0, 0);
        Util::format(value, buffer);
        test  = Util::parse128(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw128(-1, 0);
        Util::format(value, buffer);
        test  = Util::parse128(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw128(42, 0);
        Util::format(value, buffer);
        test  = Util::parse128(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw128(-42, 0);
        Util::format(value, buffer);
        test  = Util::parse128(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw128(42, 5);
        Util::format(value, buffer);
        test  = Util::parse128(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw128(-42, 5);
        Util::format(value, buffer);
        test  = Util::parse128(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw128(42, -17);
        Util::format(value, buffer);
        test  = Util::parse128(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::makeDecimalRaw128(-42, -17);
        Util::format(value, buffer);
        test  = Util::parse128(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::convertToDecimal128(Util::makeInfinity64(false));
        Util::format(value, buffer);
        test  = Util::parse128(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::convertToDecimal128(Util::makeInfinity64(true));
        Util::format(value, buffer);
        test  = Util::parse128(buffer);
        LOOP_ASSERT(buffer,Util::equal(value, test));

        value = Util::parse128("NaN");
        Util::format(value, buffer);
        test  = Util::parse128(buffer);
        LOOP_ASSERT(buffer,!Util::equal(test, test));
    }
}

void TestDriver::testCase17()
{
    // ------------------------------------------------------------------------
    // TESTING 'scaleB'
    //
    // Concerns:
    //:  1 Values should be '/=' or '*=' by 10 times the second parameter.
    //
    // Plan:
    //:  1 Run some sample values.
    //
    // Testing:
    //   scaleB(ValueType32,  int)
    //   scaleB(ValueType64,  int)
    //   scaleB(ValueType128, int)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TESTING 'scaleB'" << endl
                      << "================" << endl;

    Util::ValueType32  value32;
    Util::ValueType32   test32;

    Util::ValueType64  value64;
    Util::ValueType64   test64;

    Util::ValueType128 value128;
    Util::ValueType128  test128;

    value32 = Util::parse32("0");
    test32  = Util::scaleB(Util::parse32("0"), 10);
    ASSERT(Util::equal(value32, test32));

    value32 = Util::parse32("0");
    test32  = Util::scaleB(Util::parse32("0"), -10);
    ASSERT(Util::equal(value32, test32));

    value32 = Util::parse32("420000000000");
    test32  = Util::scaleB(Util::parse32("42"), 10);
    ASSERT(Util::equal(value32, test32));

    value32 = Util::parse32("4.2e-9");
    test32  = Util::scaleB(Util::parse32("42"), -10);
    ASSERT(Util::equal(value32, test32));


    value64 = Util::parse64("0");
    test64  = Util::scaleB(Util::parse64("0"), 10);
    ASSERT(Util::equal(value64, test64));

    value64 = Util::parse64("0");
    test64  = Util::scaleB(Util::parse64("0"), -10);
    ASSERT(Util::equal(value64, test64));

    value64 = Util::parse64("420000000000");
    test64  = Util::scaleB(Util::parse64("42"), 10);
    ASSERT(Util::equal(value64, test64));

    value64 = Util::parse64("4.2e-9");
    test64  = Util::scaleB(Util::parse64("42"), -10);
    ASSERT(Util::equal(value64, test64));


    value128 = Util::parse128("0");
    test128  = Util::scaleB(Util::parse128("0"), 10);
    ASSERT(Util::equal(value128, test128));

    value128 = Util::parse128("0");
    test128  = Util::scaleB(Util::parse128("0"), -10);
    ASSERT(Util::equal(value128, test128));

    value128 = Util::parse128("420000000000");
    test128  = Util::scaleB(Util::parse128("42"), 10);
    ASSERT(Util::equal(value128, test128));

    value128 = Util::parse128("4.2e-9");
    test128  = Util::scaleB(Util::parse128("42"), -10);
    ASSERT(Util::equal(value128, test128));
}

void TestDriver::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING COMPARISON FUNCTIONS
    //
    // Concerns:
    //:  1 Infinities are at the ends of all ordered comparision arrangements.
    //:
    //:  2 Positive numbers are greater than negative numbers.
    //:
    //:  3 'NaN' is neither greater nor less than any value, including itself.
    //:
    //:  4 Zero compares neither less nor greater than itself.
    //:
    //:  5 Transitivity is preserved.
    //
    // Plan:
    //:  1 A set of representative values for 32, 64, and 128-bit types will be
    //:    created and each compared against each other, under assertion for
    //:    the correct result. (C-2,5)
    //:
    //:  2 Zeros of both signs will be in the data set. (C-4)
    //:
    //:  3 Infinities will be among the values (C-1)
    //:
    //:  4 A 'NaN' will be among the data. (C-3)
    //
    // Testing:
    //   less(ValueType32,  ValueType32)
    //   less(ValueType64,  ValueType64)
    //   less(ValueType128, ValueType128)
    //   greater(ValueType32,  ValueType32)
    //   greater(ValueType64,  ValueType64)
    //   greater(ValueType128, ValueType128)
    //   lessEqual(ValueType32,  ValueType32)
    //   lessEqual(ValueType64,  ValueType64)
    //   lessEqual(ValueType128, ValueType128)
    //   greaterEqual(ValueType32,  ValueType32)
    //   greaterEqual(ValueType64,  ValueType64)
    //   greaterEqual(ValueType128, ValueType128)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TESTING COMPARISON FUNCTIONS" << endl
                      << "============================" << endl;

    Util::ValueType32  nan32  = Util::parse32( "NaN");
    Util::ValueType64  nan64  = Util::parse64( "NaN");
    Util::ValueType128 nan128 = Util::parse128("NaN");

    // Check that all 'NaN' operations return false:

    ASSERT(!Util::less(nan32, nan32));
    ASSERT(!Util::greater(nan32, nan32));
    ASSERT(!Util::lessEqual(nan32, nan32));
    ASSERT(!Util::greaterEqual(nan32, nan32));

    ASSERT(!Util::less(nan64, nan64));
    ASSERT(!Util::greater(nan64, nan64));
    ASSERT(!Util::lessEqual(nan64, nan64));
    ASSERT(!Util::greaterEqual(nan64, nan64));

    ASSERT(!Util::less(nan128, nan128));
    ASSERT(!Util::greater(nan128, nan128));
    ASSERT(!Util::lessEqual(nan128, nan128));
    ASSERT(!Util::greaterEqual(nan128, nan128));

    struct {
        const char *number;
        const int order;
    } tests[] = {
        { "-Inf",   -100000 },
        { "-42e10", -   100 },
        { "-42",    -    90 },
        { "-1",     -    10 },
        { "-.1415", -     2 },
        { "-.0025", -     1 },
        { "-0",           0 },
        { "+0",           0 },
        { "+.0025", +     1 },
        { "+.1415", +     2 },
        { "+1",     +    10 },
        { "+42",    +    90 },
        { "+42e10", +   100 },
        { "+Inf",   +100000 }
    };
    const int numTests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < numTests; ++i) {
        const char *const lhsValue = tests[i].number;
        const int         lhsOrder = tests[i].order;

        if (veryVerbose) cout << endl
                              << "lhsValue = " << lhsValue << endl;

        Util::ValueType32  lhs32  = Util::parse32( lhsValue);
        Util::ValueType64  lhs64  = Util::parse64( lhsValue);
        Util::ValueType128 lhs128 = Util::parse128(lhsValue);

        // Test 'NaN' against these values...

        ASSERT(!Util::less( nan32, lhs32));
        ASSERT(!Util::less(lhs32,  nan32));
        ASSERT(!Util::greater( nan32, lhs32));
        ASSERT(!Util::greater(lhs32,  nan32));
        ASSERT(!Util::lessEqual( nan32, lhs32));
        ASSERT(!Util::lessEqual(lhs32,  nan32));
        ASSERT(!Util::greaterEqual( nan32, lhs32));
        ASSERT(!Util::greaterEqual(lhs32,  nan32));

        ASSERT(!Util::less(nan64, lhs64));
        ASSERT(!Util::less(lhs64, nan64));
        ASSERT(!Util::greater(nan64, lhs64));
        ASSERT(!Util::greater(lhs64, nan64));
        ASSERT(!Util::lessEqual(nan64, lhs64));
        ASSERT(!Util::lessEqual(lhs64, nan64));
        ASSERT(!Util::greaterEqual(nan64, lhs64));
        ASSERT(!Util::greaterEqual(lhs64, nan64));

        ASSERT(!Util::less(nan128, lhs128));
        ASSERT(!Util::less(lhs128, nan128));
        ASSERT(!Util::greater(nan128, lhs128));
        ASSERT(!Util::greater(lhs128, nan128));
        ASSERT(!Util::lessEqual(nan128, lhs128));
        ASSERT(!Util::lessEqual(lhs128, nan128));
        ASSERT(!Util::greaterEqual(nan128, lhs128));
        ASSERT(!Util::greaterEqual(lhs128, nan128));

        for (int j = 0; j < numTests; ++j) {
            const char *const rhsValue = tests[j].number;
            const int         rhsOrder = tests[j].order;
            if (veryVerbose) cout << endl
                                  << "rhsValue = " << rhsValue << endl;

            Util::ValueType32  rhs32  = Util::parse32( rhsValue);
            Util::ValueType64  rhs64  = Util::parse64( rhsValue);
            Util::ValueType128 rhs128 = Util::parse128(rhsValue);

            // Test Decimal32.
            {
                ASSERTV(
                 lhsValue, rhsValue,
                 lhsOrder, rhsOrder,
                 Util::less(lhs32, rhs32)         == (lhsOrder< rhsOrder));
                ASSERTV(
                 lhsValue, rhsValue,
                 lhsOrder, rhsOrder,
                 Util::greater(lhs32, rhs32)      == (lhsOrder> rhsOrder));
                ASSERTV(
                 lhsValue, rhsValue,
                 lhsOrder, rhsOrder,
                 Util::lessEqual(lhs32, rhs32)    == (lhsOrder<=rhsOrder));
                ASSERTV(
                 lhsValue, rhsValue,
                 lhsOrder, rhsOrder,
                 Util::greaterEqual(lhs32, rhs32) == (lhsOrder>=rhsOrder));
            }

            // Test Decimal64.
            {
                ASSERTV(
                 lhsValue, rhsValue,
                 lhsOrder, rhsOrder,
                 Util::less(lhs64, rhs64)         == (lhsOrder< rhsOrder));
                ASSERTV(
                 lhsValue, rhsValue,
                 lhsOrder, rhsOrder,
                 Util::greater(lhs64, rhs64)      == (lhsOrder> rhsOrder));
                ASSERTV(
                 lhsValue, rhsValue,
                 lhsOrder, rhsOrder,
                 Util::lessEqual(lhs64, rhs64)    == (lhsOrder<=rhsOrder));
                ASSERTV(
                 lhsValue, rhsValue,
                 lhsOrder, rhsOrder,
                 Util::greaterEqual(lhs64, rhs64) == (lhsOrder>=rhsOrder));
            }

            // Test Decimal128.
            {
                ASSERTV(
                 lhsValue, rhsValue,
                 lhsOrder, rhsOrder,
                 Util::less(lhs128, rhs128)        ==(lhsOrder< rhsOrder));
                ASSERTV(
                 lhsValue, rhsValue,
                 lhsOrder, rhsOrder,
                 Util::greater(lhs128, rhs128)     ==(lhsOrder> rhsOrder));
                ASSERTV(
                 lhsValue, rhsValue,
                 lhsOrder, rhsOrder,
                 Util::lessEqual(lhs128, rhs128)   ==(lhsOrder<=rhsOrder));
                ASSERTV(
                 lhsValue, rhsValue,
                 lhsOrder, rhsOrder,
                 Util::greaterEqual(lhs128, rhs128)==(lhsOrder>=rhsOrder));
            }
        }
    }
}

void TestDriver::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING 'binaryToDecimalXX'
    //
    // Concerns:
    //:  1 Conversion to Decimal preserves precise value, where possible.  The
    //:    bases of 10, and 2 are only able to cross convert when the actual
    //:    value is proper sum of fractions of powers-of-two.
    //
    // Plan:
    //:  1 Convert several values that are 'integral' and sums of power-of-two
    //:    fractions (which just counts as whole numbers).  (C-1)
    //
    // Testing:
    //   binaryToDecimal32 (float)
    //   binaryToDecimal32 (double)
    //   binaryToDecimal64 (float)
    //   binaryToDecimal64 (double)
    //   binaryToDecimal128(float)
    //   binaryToDecimal128(double)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TESTING 'binaryToDecimalXX'" << endl
                      << "===========================" << endl;

    // 'binaryToDecimal32(float)'

    {
        Util::ValueType32 value32;
        Util::ValueType32  test32;

        value32 = Util::parse32(         "0.0");
         test32 = Util::binaryToDecimal32(0.0f);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "1.0");
         test32 = Util::binaryToDecimal32(1.0f);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "10.");
         test32 = Util::binaryToDecimal32(10.f);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "42.");
         test32 = Util::binaryToDecimal32(42.f);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "-1.0");
         test32 = Util::binaryToDecimal32(-1.0f);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "-10.");
         test32 = Util::binaryToDecimal32(-10.f);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "-42.");
         test32 = Util::binaryToDecimal32(-42.f);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "0.5");
         test32 = Util::binaryToDecimal32(0.5f);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "0.25");
         test32 = Util::binaryToDecimal32(0.25f);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "-0.5");
         test32 = Util::binaryToDecimal32(-0.5f);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "-0.25");
         test32 = Util::binaryToDecimal32(-0.25f);
        ASSERT(Util::equal(value32, test32));
    }

    // binaryToDecimal32(double)

    {
        Util::ValueType32 value32;
        Util::ValueType32  test32;

        value32 = Util::parse32(         "0.0");
         test32 = Util::binaryToDecimal32(0.0);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "1.0");
         test32 = Util::binaryToDecimal32(1.0);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "10.");
         test32 = Util::binaryToDecimal32(10.);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "42.");
         test32 = Util::binaryToDecimal32(42.);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "-1.0");
         test32 = Util::binaryToDecimal32(-1.0);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "-10.");
         test32 = Util::binaryToDecimal32(-10.);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "-42.");
         test32 = Util::binaryToDecimal32(-42.);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "0.5");
         test32 = Util::binaryToDecimal32(0.5);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "0.25");
         test32 = Util::binaryToDecimal32(0.25);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "-0.5");
         test32 = Util::binaryToDecimal32(-0.5);
        ASSERT(Util::equal(value32, test32));

        value32 = Util::parse32(         "-0.25");
         test32 = Util::binaryToDecimal32(-0.25);
        ASSERT(Util::equal(value32, test32));
    }


    // 'binaryToDecimal64(float)'

    {
        Util::ValueType64 value64;
        Util::ValueType64  test64;

        value64 = Util::parse64(         "0.0");
         test64 = Util::binaryToDecimal64(0.0f);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "1.0");
         test64 = Util::binaryToDecimal64(1.0f);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "10.");
         test64 = Util::binaryToDecimal64(10.f);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "42.");
         test64 = Util::binaryToDecimal64(42.f);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "-1.0");
         test64 = Util::binaryToDecimal64(-1.0f);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "-10.");
         test64 = Util::binaryToDecimal64(-10.f);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "-42.");
         test64 = Util::binaryToDecimal64(-42.f);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "0.5");
         test64 = Util::binaryToDecimal64(0.5f);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "0.25");
         test64 = Util::binaryToDecimal64(0.25f);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "-0.5");
         test64 = Util::binaryToDecimal64(-0.5f);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "-0.25");
         test64 = Util::binaryToDecimal64(-0.25f);
        ASSERT(Util::equal(value64, test64));
    }

    // binaryToDecimal64(double)

    {
        Util::ValueType64 value64;
        Util::ValueType64  test64;

        value64 = Util::parse64(         "0.0");
         test64 = Util::binaryToDecimal64(0.0);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "1.0");
         test64 = Util::binaryToDecimal64(1.0);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "10.");
         test64 = Util::binaryToDecimal64(10.);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "42.");
         test64 = Util::binaryToDecimal64(42.);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "-1.0");
         test64 = Util::binaryToDecimal64(-1.0);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "-10.");
         test64 = Util::binaryToDecimal64(-10.);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "-42.");
         test64 = Util::binaryToDecimal64(-42.);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "0.5");
         test64 = Util::binaryToDecimal64(0.5);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "0.25");
         test64 = Util::binaryToDecimal64(0.25);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "-0.5");
         test64 = Util::binaryToDecimal64(-0.5);
        ASSERT(Util::equal(value64, test64));

        value64 = Util::parse64(         "-0.25");
         test64 = Util::binaryToDecimal64(-0.25);
        ASSERT(Util::equal(value64, test64));
    }

    // 'binaryToDecimal128(float)'

    {
        Util::ValueType128 value128;
        Util::ValueType128  test128;

        value128 = Util::parse128(         "0.0");
         test128 = Util::binaryToDecimal128(0.0f);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "1.0");
         test128 = Util::binaryToDecimal128(1.0f);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "10.");
         test128 = Util::binaryToDecimal128(10.f);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "42.");
         test128 = Util::binaryToDecimal128(42.f);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "-1.0");
         test128 = Util::binaryToDecimal128(-1.0f);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "-10.");
         test128 = Util::binaryToDecimal128(-10.f);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "-42.");
         test128 = Util::binaryToDecimal128(-42.f);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "0.5");
         test128 = Util::binaryToDecimal128(0.5f);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "0.25");
         test128 = Util::binaryToDecimal128(0.25f);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "-0.5");
         test128 = Util::binaryToDecimal128(-0.5f);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "-0.25");
         test128 = Util::binaryToDecimal128(-0.25f);
        ASSERT(Util::equal(value128, test128));
    }

    // binaryToDecimal128(double)

    {
        Util::ValueType128 value128;
        Util::ValueType128  test128;

        value128 = Util::parse128(         "0.0");
         test128 = Util::binaryToDecimal128(0.0);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "1.0");
         test128 = Util::binaryToDecimal128(1.0);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "10.");
         test128 = Util::binaryToDecimal128(10.);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "42.");
         test128 = Util::binaryToDecimal128(42.);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "-1.0");
         test128 = Util::binaryToDecimal128(-1.0);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "-10.");
         test128 = Util::binaryToDecimal128(-10.);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "-42.");
         test128 = Util::binaryToDecimal128(-42.);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "0.5");
         test128 = Util::binaryToDecimal128(0.5);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "0.25");
         test128 = Util::binaryToDecimal128(0.25);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "-0.5");
         test128 = Util::binaryToDecimal128(-0.5);
        ASSERT(Util::equal(value128, test128));

        value128 = Util::parse128(         "-0.25");
         test128 = Util::binaryToDecimal128(-0.25);
        ASSERT(Util::equal(value128, test128));
    }
}

void TestDriver::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING '[u]intXXToDecimalXX'
    //
    // Concerns:
    //:  1 '[u]intXXToDecimalXX' must convert the value as expected.
    //:
    //:  2 Conversion may not be by forwarding.
    //:
    //:  3 There are 4 overloads for each size, which need testing.
    //:
    //:  4 Rounding is handled appropriately.
    //
    //Plan:
    //:  1 Run numerous mantissas through the conversion functions and compare
    //:    the results with 'parse(mantissaAsString)'.  (C-1..4)
    //
    // Testing:
    //    int32ToDecimal32 (                   int)
    //   uint32ToDecimal32 (unsigned           int)
    //    int64ToDecimal32 (         long long int)
    //   uint64ToDecimal32 (unsigned long long int)
    //    int32ToDecimal64 (                   int)
    //   uint32ToDecimal64 (unsigned           int)
    //    int64ToDecimal64 (         long long int)
    //   uint64ToDecimal64 (unsigned long long int)
    //    int32ToDecimal128(                   int)
    //   uint32ToDecimal128(unsigned           int)
    //    int64ToDecimal128(         long long int)
    //   uint64ToDecimal128(unsigned long long int)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TESTING '[u]intXXToDecimalXX'" << endl
                      << "=============================" << endl;

    for (int mi = 0; mi < NUM_TEST_NONZERO_MANTISSAS; ++mi) {
        const long long MANTISSA = TEST_NONZERO_MANTISSAS[mi];

        if (veryVerbose) cout << endl
                              << "Testing mantissa index: " << mi
                              << ", MANTISSA: " << MANTISSA << endl;

        char TEST_STRING[100];
        sprintf(TEST_STRING, "%lld", MANTISSA);

        Util::ValueType32  value32 = Util::parse32(TEST_STRING);
        Util::ValueType64  value64 = Util::parse64(TEST_STRING);
        Util::ValueType128 value128 = Util::parse128(TEST_STRING);

        if (MANTISSA >= 0) {
          LOOP2_ASSERT(MANTISSA, TEST_STRING, Util::equal(value32,
                                   Util::uint64ToDecimal32(
                              static_cast<unsigned long long>(MANTISSA))));

          LOOP2_ASSERT(MANTISSA, TEST_STRING, Util::equal(value64,
                                   Util::uint64ToDecimal64(
                              static_cast<unsigned long long>(MANTISSA))));


          LOOP2_ASSERT(MANTISSA, TEST_STRING, Util::equal(value128,
                                   Util::uint64ToDecimal128(
                              static_cast<unsigned long long>(MANTISSA))));
        }

        if ((MANTISSA >= bsl::numeric_limits<int>::min()) &&
            (MANTISSA <= bsl::numeric_limits<int>::max())) {
          LOOP2_ASSERT(MANTISSA, TEST_STRING, Util::equal(value32,
                                   Util::int32ToDecimal32(
                                   static_cast<int>(MANTISSA))));

          LOOP2_ASSERT(MANTISSA, TEST_STRING, Util::equal(value64,
                                   Util::int32ToDecimal64(
                                   static_cast<int>(MANTISSA))));


          LOOP2_ASSERT(MANTISSA, TEST_STRING, Util::equal(value128,
                                   Util::int32ToDecimal128(
                                   static_cast<int>(MANTISSA))));
        }
        if ((MANTISSA >= 0 ) &&
            (MANTISSA <= bsl::numeric_limits<unsigned int>::max())) {
          LOOP2_ASSERT(MANTISSA, TEST_STRING, Util::equal(value32,
                                   Util::uint32ToDecimal32(
                                   static_cast<unsigned int>(MANTISSA))));

          LOOP2_ASSERT(MANTISSA, TEST_STRING, Util::equal(value64,
                                   Util::uint32ToDecimal64(
                                   static_cast<unsigned int>(MANTISSA))));


          LOOP2_ASSERT(MANTISSA, TEST_STRING, Util::equal(value128,
                                   Util::uint32ToDecimal128(
                                   static_cast<unsigned int>(MANTISSA))));
        }

        // For 'long long'

        {
          LOOP2_ASSERT(MANTISSA, TEST_STRING, Util::equal(value32,
                                   Util::int64ToDecimal32(MANTISSA)));

          LOOP2_ASSERT(MANTISSA, TEST_STRING, Util::equal(value64,
                                   Util::int64ToDecimal64(MANTISSA)));


          LOOP2_ASSERT(MANTISSA, TEST_STRING, Util::equal(value128,
                                   Util::int64ToDecimal128(MANTISSA)));
        }
    }
}

void TestDriver::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING 'makeDecimal64'
    //
    // Concerns:
    //: 1 'makeDecimal64' passes its arguments to 'makeDecimal64Raw' and
    //:   'makeDecimal128Raw' correctly.
    //:
    //: 2 The correct rounding behavior is performed when the specified
    //:   mantissa has a magnitude too great to be represented by a 64-bit
    //:   decimal floating point value.
    //:
    //: 3 The correct rounding behavior is performed when the magnitude of the
    //:   specified number is too great to be represented by a 64-bit decimal
    //:   floating point value.
    //:
    //: 4 The correct sign is preserved when rounding to zero or infinite
    //:   values.
    //:
    //: 5 If the mantissa and exponent are in range for 64-bit decimal floating
    //:   point numbers (i.e., 'abs(mantissa) <= 9,999,999,999,999,999' and
    //:   '-398 <= exponent <= 369'), the 64-bit decimal floating number
    //:   consisting of the mantissa and exponent is returned without rounding.
    //:
    //: 6 The four overloads of 'makeDecimal64' perform properly.
    //:
    //: 7 Note that this test is adapted from the original 'makeDecimal64' test
    //:   in the old 'bdldfp::DecimalImplUtil' component.
    //
    // Plan:
    //: 1 Test that 'makeDecimal64' returns a value that is 'equals' to the
    //:   value given by 'parse64' called on a string containing the specified
    //:   'mantissa' and 'exponent'.
    //:
    //: 2 Test 'makeDecimal64' on values for which both the mantissa and
    //:   exponent are in the range of 64-bit decimal floating point. The value
    //:   returned should be identical in bits to that returned by
    //:   'makeDecimal64Raw' called on 'mantissa' and 'exponent'.
    //:
    //: 3 For each mantissa, exponent pair, test 'makeDecimal64' for all
    //:   overloads for which the mantissa fits.
    //
    // Testing:
    //   makeDecimal64(                   int, int)
    //   makeDecimal64(unsigned           int, int)
    //   makeDecimal64(         long long int, int)
    //   makeDecimal64(unsigned long long int, int)
    // ------------------------------------------------------------------------

    if (verbose) bsl::cout << bsl::endl
                           << "TESTING 'makeDecimal64'" << bsl::endl
                           << "=======================" << bsl::endl;
    {
        for (int mi = 0; mi < NUM_TEST_NONZERO_MANTISSAS; ++mi) {
            if (veryVerbose) cout << endl
                                  << "Testing mantissa index: "
                                  << mi << endl;
            for (int ei = 0; ei < NUM_TEST_EXPONENTS; ++ei) {
                const long long MANTISSA = TEST_NONZERO_MANTISSAS[mi];
                const int EXPONENT = TEST_EXPONENTS[ei];

            if (veryVerbose) cout << endl
                                  << "Testing exponent index: " << ei
                                  << ", MANTISSA: " << MANTISSA
                                  << ", EXPONENT: " << EXPONENT << endl;

                char TEST_STRING[100];
                sprintf(TEST_STRING, "%llde%d", MANTISSA, EXPONENT);

                Util::ValueType64 EXPECTED64 =
                    Util::parse64(TEST_STRING);
                LOOP2_ASSERT(MANTISSA, EXPONENT,
                             Util::equal(Util::makeDecimal64(MANTISSA,
                                                             EXPONENT),
                                         EXPECTED64));

                if (MANTISSA >= 0) {
                    LOOP2_ASSERT(MANTISSA, EXPONENT,
                            Util::equal(Util::makeDecimal64(
                                 static_cast<unsigned long long>(MANTISSA),
                                                                 EXPONENT),
                                        EXPECTED64));
                }
                if ((MANTISSA >= bsl::numeric_limits<int>::min()) &&
                    (MANTISSA <= bsl::numeric_limits<int>::max())) {
                    LOOP2_ASSERT(MANTISSA, EXPONENT,
                                 Util::equal(Util::makeDecimal64(
                                                static_cast<int>(MANTISSA),
                                                                 EXPONENT),
                                             EXPECTED64));
                }
                if ((MANTISSA >=
                               bsl::numeric_limits<unsigned int>::min()) &&
                    (MANTISSA <=
                               bsl::numeric_limits<unsigned int>::max())) {
                    LOOP2_ASSERT(MANTISSA, EXPONENT,
                                 Util::equal(Util::makeDecimal64(
                                       static_cast<unsigned int>(MANTISSA),
                                                                 EXPONENT),
                                             EXPECTED64));
                }

                bool exponentInRange = (EXPONENT >= -398) &&
                                       (EXPONENT <=  369);
                bool mantissaInRange = (MANTISSA >= -9999999999999999LL) &&
                                       (MANTISSA <=  9999999999999999LL);

                if (exponentInRange & mantissaInRange) {
                    LOOP2_ASSERT(MANTISSA, EXPONENT, Util::equal(
                              Util::makeDecimal64(   MANTISSA, EXPONENT),
                              Util::makeDecimalRaw64(MANTISSA, EXPONENT)));
                    if (MANTISSA >= 0) {
                        LOOP2_ASSERT(MANTISSA, EXPONENT, Util::equal(
                            Util::makeDecimal64(
                               static_cast<unsigned long long>(MANTISSA),
                                                               EXPONENT),
                            Util::makeDecimalRaw64(
                               static_cast<unsigned long long>(MANTISSA),
                                                               EXPONENT)));
                    }
                    if ((MANTISSA >= bsl::numeric_limits<int>::min()) &&
                        (MANTISSA <= bsl::numeric_limits<int>::max())) {
                        LOOP2_ASSERT(MANTISSA, EXPONENT, Util::equal(
                            Util::makeDecimal64(
                                   static_cast<int>(MANTISSA), EXPONENT),
                            Util::makeDecimalRaw64(
                                   static_cast<int>(MANTISSA), EXPONENT)));
                    }
                    if ((MANTISSA >=
                         bsl::numeric_limits<unsigned int>::min()) &&
                        (MANTISSA <=
                         bsl::numeric_limits<unsigned int>::max())) {
                        LOOP2_ASSERT(MANTISSA, EXPONENT, Util::equal(
                            Util::makeDecimal64(
                                     static_cast<unsigned int>(MANTISSA),
                                                               EXPONENT),
                            Util::makeDecimalRaw64(
                                     static_cast<unsigned int>(MANTISSA),
                                                               EXPONENT)));
                    }
                }
            }
        }
    }
}

void TestDriver::testCase12()
{
    // ------------------------------------------------------------------------
    // INFINITY CREATION 'makeInfinity64(bool)'
    //
    // Concerns:
    //:  1 'makeInfinity64' should return an infinity value which is positive,
    //:    when passed no arguments.
    //:
    //:  2 'makeInfinity64' should return an infinity value which is positive,
    //:    when passed a 'false' argument.
    //:
    //:  2 'makeInfinity64' should return an infinity value which is negative,
    //:    when passed a 'true' argument.
    //
    // Plan:
    //:  1 call 'makeInfinity' with no arguments.  (C-1)
    //:
    //:  2 call 'makeInfinity' with 'false' argument.  (C-2)
    //:
    //:  3 call 'makeInfinity' with 'true' argument.  (C-3)
    //
    // Testing:
    //   makeInfinity64(bool)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "INFINITY CREATION 'makeInfinity64(bool)'" << endl
                      << "========================================"
                      << endl;

    Util::ValueType64 ninf = Util::parse64("-inf");
    Util::ValueType64 pinf = Util::parse64("+inf");

    Util::ValueType64 test;

    test = Util::makeInfinity64();
    ASSERT(Util::equal(test, pinf));

    test = Util::makeInfinity64(false);
    ASSERT(Util::equal(test, pinf));

    test = Util::makeInfinity64(true);
    ASSERT(Util::equal(test, ninf));
}

void TestDriver::testCase11()
{
    // ------------------------------------------------------------------------
    // PARSING 'parse32', 'parse64', AND 'parse128'
    //
    // Concerns:
    //:  1 Numerical values are parsed and generated correctly.
    //:
    //:  2 Non-numerical state-values are parsed and generated correctly.
    //:
    //:  3 Alternate creation strings work correctly.
    //:
    //:  4 Signs are interpretted correctly.
    //
    // Plan:
    //:  1 Test a series of strings versus their expected values.
    //:
    //:  2 Test the non-numerical state-values by alternate means of
    //:    generation, such as '1.0/0.0' and '0.0/0.0'.
    //
    // Testing:
    //   parse32 (const char *)
    //   parse64 (const char *)
    //   parse128(const char *)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "PARSING 'parse32', 'parse64', AND 'parse128'"
              << endl << "============================================"
              << endl;


    // Testing 'parse32 (const char *)'

    {
        Util::ValueType32 value32;
        Util::ValueType32 test32;

        value32 = Util::makeDecimalRaw32(0, 0);

        test32  = Util::parse32("0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("-0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("0e0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("-0e0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+0e0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("0e-0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("-0e-0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+0e-0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("0e+0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("-0e+0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+0e+0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("0e4");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("-0e4");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+0e4");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("0e+4");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("-0e+4");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+0e+4");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("0e-9");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("-0e-9");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+0e-9");
        ASSERT(Util::equal(value32, test32));

        value32 = Util::makeDecimalRaw32(1, 0);

        test32  = Util::parse32("1");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+1");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("1e0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+1e0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("01e0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+01e0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("1e-0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+1e-0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("1e+0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+1e+0");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+10e-001");
        ASSERT(Util::equal(value32, test32));

        value32 = Util::makeDecimalRaw32(8, 42);

        test32  = Util::parse32("8e42");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+8e42");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("8e+42");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("800e40");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("+800e40");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32(".00800e+45");
        ASSERT(Util::equal(value32, test32));

        value32 = Util::makeDecimalRaw32(-7, -33);

        test32  = Util::parse32("-7e-33");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("-00007e-0033");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("-70000e-37");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("-007e-33");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("-0.7e-32");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("-0.0007e-29");
        ASSERT(Util::equal(value32, test32));
        test32  = Util::parse32("-000.000000000000000000000000000000007");
        ASSERT(Util::equal(value32, test32));

        value32 = Util::convertToDecimal32(
                                         Util::divide(Util::parse64("0"),
                                                      Util::parse64("0")));

        test32  = Util::parse32("nan");
        ASSERT(!Util::equal(test32, test32));

        value32 = Util::convertToDecimal32(
                                         Util::divide(Util::parse64("1"),
                                                      Util::parse64("0")));

        test32  = Util::parse32("inf");
        ASSERT(Util::equal(test32, value32));
        test32  = Util::parse32("+inf");
        ASSERT(Util::equal(test32, value32));
        test32  = Util::parse32("inF");
        ASSERT(Util::equal(test32, value32));
        test32  = Util::parse32("+inF");
        ASSERT(Util::equal(test32, value32));
    }

    // Testing 'parse64 (const char *)'

    {
        Util::ValueType64 value64;
        Util::ValueType64 test64;

        value64 = Util::makeDecimalRaw64(0, 0);

        test64  = Util::parse64("0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("-0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("0e0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("-0e0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+0e0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("0e-0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("-0e-0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+0e-0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("0e+0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("-0e+0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+0e+0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("0e4");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("-0e4");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+0e4");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("0e+4");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("-0e+4");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+0e+4");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("0e-9");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("-0e-9");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+0e-9");
        ASSERT(Util::equal(value64, test64));

        value64 = Util::makeDecimalRaw64(1, 0);

        test64  = Util::parse64("1");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+1");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("1e0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+1e0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("01e0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+01e0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("1e-0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+1e-0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("1e+0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+1e+0");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+10e-001");
        ASSERT(Util::equal(value64, test64));

        value64 = Util::makeDecimalRaw64(8, 42);

        test64  = Util::parse64("8e42");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+8e42");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("8e+42");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("800e40");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("+800e40");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64(".00800e+45");
        ASSERT(Util::equal(value64, test64));

        value64 = Util::makeDecimalRaw64(-7, -33);

        test64  = Util::parse64("-7e-33");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("-00007e-0033");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("-70000e-37");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("-007e-33");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("-0.7e-32");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("-0.0007e-29");
        ASSERT(Util::equal(value64, test64));
        test64  = Util::parse64("-000.000000000000000000000000000000007");
        ASSERT(Util::equal(value64, test64));

        value64 = Util::divide(Util::parse64("0"), Util::parse64("0"));

        test64  = Util::parse64("nan");
        ASSERT(!Util::equal(test64, test64));

        value64 = Util::divide(Util::parse64("1"), Util::parse64("0"));

        test64  = Util::parse64("inf");
        ASSERT(Util::equal(test64, value64));
        test64  = Util::parse64("+inf");
        ASSERT(Util::equal(test64, value64));
        test64  = Util::parse64("inF");
        ASSERT(Util::equal(test64, value64));
        test64  = Util::parse64("+inF");
        ASSERT(Util::equal(test64, value64));

        value64 = Util::divide(Util::parse64("-1"), Util::parse64("0"));

        test64  = Util::parse64("-inf");
        ASSERT(Util::equal(test64, value64));
        test64  = Util::parse64("-Inf");
        ASSERT(Util::equal(test64, value64));
        test64  = Util::parse64("-InF");
        ASSERT(Util::equal(test64, value64));
        test64  = Util::parse64("-inF");
        ASSERT(Util::equal(test64, value64));
    }

    // Testing 'parse128 (const char *)'

    {
        Util::ValueType128 value128;
        Util::ValueType128 test128;

        value128 = Util::makeDecimalRaw128(0, 0);

        test128  = Util::parse128("0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("-0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("0e0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("-0e0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+0e0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("0e-0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("-0e-0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+0e-0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("0e+0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("-0e+0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+0e+0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("0e4");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("-0e4");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+0e4");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("0e+4");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("-0e+4");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+0e+4");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("0e-9");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("-0e-9");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+0e-9");
        ASSERT(Util::equal(value128, test128));

        value128 = Util::makeDecimalRaw128(1, 0);

        test128  = Util::parse128("1");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+1");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("1e0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+1e0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("01e0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+01e0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("1e-0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+1e-0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("1e+0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+1e+0");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+10e-001");
        ASSERT(Util::equal(value128, test128));

        value128 = Util::makeDecimalRaw128(8, 42);

        test128  = Util::parse128("8e42");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+8e42");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("8e+42");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("800e40");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("+800e40");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128(".00800e+45");
        ASSERT(Util::equal(value128, test128));

        value128 = Util::makeDecimalRaw128(-7, -33);

        test128  = Util::parse128("-7e-33");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("-00007e-0033");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("-70000e-37");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("-007e-33");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("-0.7e-32");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("-0.0007e-29");
        ASSERT(Util::equal(value128, test128));
        test128  = Util::parse128("-00.000000000000000000000000000000007");
        ASSERT(Util::equal(value128, test128));

        value128 = Util::divide(Util::parse128("0"), Util::parse128("0"));

        test128  = Util::parse128("nan");
        ASSERT(!Util::equal(test128, test128));

        value128 = Util::divide(Util::parse128("1"), Util::parse128("0"));

        test128  = Util::parse128("inf");
        ASSERT(Util::equal(test128, value128));
        test128  = Util::parse128("+inf");
        ASSERT(Util::equal(test128, value128));
        test128  = Util::parse128("inF");
        ASSERT(Util::equal(test128, value128));
        test128  = Util::parse128("+inF");
        ASSERT(Util::equal(test128, value128));

        value128 = Util::divide(Util::parse128("-1"), Util::parse128("0"));

        test128  = Util::parse128("-inf");
        ASSERT(Util::equal(test128, value128));
        test128  = Util::parse128("-Inf");
        ASSERT(Util::equal(test128, value128));
        test128  = Util::parse128("-InF");
        ASSERT(Util::equal(test128, value128));
        test128  = Util::parse128("-inF");
        ASSERT(Util::equal(test128, value128));
    }
}

void TestDriver::testCase10()
{
    // ------------------------------------------------------------------------
    // CROSS CONVERSION 'convertToDecimalXX'
    //
    // Concerns:
    //:  1 Post-conversion, the value should be as expected.
    //:
    //:  2 Rounded values should become infinity or 0 when truncating.
    //:
    //:  3 NaN value-states should convert correctly.
    //
    // Plan:
    //:  1 Convert some values to wider types, and confirm the value is as
    //:    expected.
    //:
    //:  2 Convert some values to narrower types, and confirm the value is
    //:    preserved or truncated as appropriate.
    //:
    //:  3 Convert NaN states using all functions
    //
    // Testing:
    //   convertToDecimal32( const ValueType64&)
    //   convertToDecimal64( const ValueType32&)
    //   convertToDecimal64( const ValueType128&)
    //   convertToDecimal128(const ValueType32&)
    //   convertToDecimal128(const ValueType64&)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "CROSS CONVERSION 'convertToDecimalXX'" << endl
                      << "=====================================" << endl;

    // Testing 'convertToDecimal32( const ValueType64&)'

    {
        Util::ValueType32  test32;
        Util::ValueType32 value32;

         test32 = Util::convertToDecimal32(
                                         Util::makeDecimalRaw64(  0,   0));
        value32 = Util::makeDecimalRaw32(                         0,   0);
        ASSERT(Util::equal(test32, value32));

         test32 = Util::convertToDecimal32(
                                         Util::makeDecimalRaw64( 42,   0));
        value32 = Util::makeDecimalRaw32(                        42,   0);
        ASSERT(Util::equal(test32, value32));

         test32 = Util::convertToDecimal32(
                                         Util::makeDecimalRaw64(-42,   0));
        value32 = Util::makeDecimalRaw32(                       -42,   0);
        ASSERT(Util::equal(test32, value32));

         test32 = Util::convertToDecimal32(
                                         Util::makeDecimalRaw64( 42,   4));
        value32 = Util::makeDecimalRaw32(                        42,   4);
        ASSERT(Util::equal(test32, value32));

         test32 = Util::convertToDecimal32(
                                         Util::makeDecimalRaw64(-42,   4));
        value32 = Util::makeDecimalRaw32(                       -42,   4);
        ASSERT(Util::equal(test32, value32));

         test32 = Util::convertToDecimal32(
                                         Util::makeDecimalRaw64( 42, -17));
        value32 = Util::makeDecimalRaw32(                        42, -17);
        ASSERT(Util::equal(test32, value32));

         test32 = Util::convertToDecimal32(
                                         Util::makeDecimalRaw64(-42, -17));
        value32 = Util::makeDecimalRaw32(                       -42, -17);
        ASSERT(Util::equal(test32, value32));

         test32 = Util::convertToDecimal32(
                                   Util::makeDecimalRaw64( 1048576,  300));
        value32 = Util::parse32("Inf");
        ASSERT(Util::equal(test32, value32));

         test32 = Util::convertToDecimal32(
                                   Util::makeDecimalRaw64(-1048576,  300));
        value32 = Util::parse32("-Inf");
        ASSERT(Util::equal(test32, value32));

         test32 = Util::convertToDecimal32(
                                   Util::makeDecimalRaw64( 1048576, -300));
        value32 = Util::parse32("0");
        ASSERT(Util::equal(test32, value32));

         test32 = Util::convertToDecimal32(
                                   Util::makeDecimalRaw64(-1048576, -300));
        value32 = Util::parse32("-0");
        ASSERT(Util::equal(test32, value32));


         test32 = Util::convertToDecimal32(Util::parse64("NaN"));
        ASSERT(!Util::equal(test32, test32));
    }

    // Testing 'convertToDecimal64( const ValueType32&)'

    {
        Util::ValueType64  test64;
        Util::ValueType64 value64;

         test64 = Util::convertToDecimal64(
                                         Util::makeDecimalRaw32(  0,   0));
        value64 = Util::makeDecimalRaw64(                         0,   0);
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                         Util::makeDecimalRaw32( 42,   0));
        value64 = Util::makeDecimalRaw64(                        42,   0);
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                         Util::makeDecimalRaw32(-42,   0));
        value64 = Util::makeDecimalRaw64(                       -42,   0);
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                         Util::makeDecimalRaw32( 42,   4));
        value64 = Util::makeDecimalRaw64(                        42,   4);
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                         Util::makeDecimalRaw32(-42,   4));
        value64 = Util::makeDecimalRaw64(                       -42,   4);
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                         Util::makeDecimalRaw32( 42, -17));
        value64 = Util::makeDecimalRaw64(                        42, -17);
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                         Util::makeDecimalRaw32(-42, -17));
        value64 = Util::makeDecimalRaw64(                       -42, -17);
        ASSERT(Util::equal(test64, value64));

                    // Infinities and 0 rounding are not possible with
                    // widening conversions, so we only test for 'NaN'
                    // preservation.

         test64 = Util::convertToDecimal64(Util::parse32("NaN"));
        ASSERT(!Util::equal(test64, test64));
    }

    // Testing 'convertToDecimal64( const ValueType128&)'

    {
        Util::ValueType64  test64;
        Util::ValueType64 value64;

         test64 = Util::convertToDecimal64(
                                        Util::makeDecimalRaw128(  0,   0));
        value64 = Util::makeDecimalRaw64(                         0,   0);
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                        Util::makeDecimalRaw128( 42,   0));
        value64 = Util::makeDecimalRaw64(                        42,   0);
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                        Util::makeDecimalRaw128(-42,   0));
        value64 = Util::makeDecimalRaw64(                       -42,   0);
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                        Util::makeDecimalRaw128( 42,   4));
        value64 = Util::makeDecimalRaw64(                        42,   4);
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                        Util::makeDecimalRaw128(-42,   4));
        value64 = Util::makeDecimalRaw64(                       -42,   4);
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                        Util::makeDecimalRaw128( 42, -17));
        value64 = Util::makeDecimalRaw64(                        42, -17);
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                        Util::makeDecimalRaw128(-42, -17));
        value64 = Util::makeDecimalRaw64(                       -42, -17);
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                 Util::makeDecimalRaw128( 1048576,  6000));
        value64 = Util::parse64("Inf");
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                 Util::makeDecimalRaw128(-1048576,  6000));
        value64 = Util::parse64("-Inf");
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                 Util::makeDecimalRaw128( 1048576, -6000));
        value64 = Util::parse64("0");
        ASSERT(Util::equal(test64, value64));

         test64 = Util::convertToDecimal64(
                                 Util::makeDecimalRaw128(-1048576, -6000));
        value64 = Util::parse64("-0");
        ASSERT(Util::equal(test64, value64));


         test64 = Util::convertToDecimal64(Util::parse128("NaN"));
        ASSERT(!Util::equal(test64, test64));
    }

    // Testing 'convertToDecimal128(const ValueType32&)'

    {
        Util::ValueType128  test128;
        Util::ValueType128 value128;

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw32(  0,   0));
        value128 = Util::makeDecimalRaw128(                       0,   0);
        ASSERT(Util::equal(test128, value128));

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw32( 42,   0));
        value128 = Util::makeDecimalRaw128(                      42,   0);
        ASSERT(Util::equal(test128, value128));

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw32(-42,   0));
        value128 = Util::makeDecimalRaw128(                     -42,   0);
        ASSERT(Util::equal(test128, value128));

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw32( 42,   4));
        value128 = Util::makeDecimalRaw128(                      42,   4);
        ASSERT(Util::equal(test128, value128));

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw32(-42,   4));
        value128 = Util::makeDecimalRaw128(                     -42,   4);
        ASSERT(Util::equal(test128, value128));

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw32( 42, -17));
        value128 = Util::makeDecimalRaw128(                      42, -17);
        ASSERT(Util::equal(test128, value128));

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw32(-42, -17));
        value128 = Util::makeDecimalRaw128(                     -42, -17);
        ASSERT(Util::equal(test128, value128));

                    // Infinities and 0 rounding are not possible with
                    // widening conversions, so we only test for 'NaN'
                    // preservation.

         test128 = Util::convertToDecimal128(Util::parse32("NaN"));
        ASSERT(!Util::equal(test128, test128));
    }

    // Testing 'convertToDecimal128(const ValueType64&)'

    {
        Util::ValueType128  test128;
        Util::ValueType128 value128;

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw64(  0,   0));
        value128 = Util::makeDecimalRaw128(                       0,   0);
        ASSERT(Util::equal(test128, value128));

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw64( 42,   0));
        value128 = Util::makeDecimalRaw128(                      42,   0);
        ASSERT(Util::equal(test128, value128));

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw64(-42,   0));
        value128 = Util::makeDecimalRaw128(                     -42,   0);
        ASSERT(Util::equal(test128, value128));

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw64( 42,   4));
        value128 = Util::makeDecimalRaw128(                      42,   4);
        ASSERT(Util::equal(test128, value128));

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw64(-42,   4));
        value128 = Util::makeDecimalRaw128(                     -42,   4);
        ASSERT(Util::equal(test128, value128));

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw64( 42, -17));
        value128 = Util::makeDecimalRaw128(                      42, -17);
        ASSERT(Util::equal(test128, value128));

         test128 = Util::convertToDecimal128(
                                         Util::makeDecimalRaw64(-42, -17));
        value128 = Util::makeDecimalRaw128(                     -42, -17);
        ASSERT(Util::equal(test128, value128));

                    // Infinities and 0 rounding are not possible with
                    // widening conversions, so we only test for 'NaN'
                    // preservation.

         test128 = Util::convertToDecimal128(Util::parse64("NaN"));
        ASSERT(!Util::equal(test128, test128));
    }
}

void TestDriver::testCase9()
{
    // ------------------------------------------------------------------------
    // ARITHMETIC FUNCTION 'negate'
    //
    // Concerns:
    //:  1 'negate' returns the correct answer.
    //:
    //:  2 'negate' inverts signed 0 values.
    //:
    //:  3 'negate' inverts 'Inf' state-values.
    //:
    //
    // Plan:
    //:  1 Test several example values for negation and equality. (C-1..3)
    //
    // Testing:
    //   negate(ValueType32)
    //   negate(ValueType64)
    //   negate(ValueType128)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "ARITHMETIC FUNCTION 'negate'" << endl
                      << "============================" << endl;

    Util::ValueType32 nZero32 = Util::parse32("-0");
    Util::ValueType32 pZero32 = Util::parse32("+0");

    Util::ValueType32 nFive32 = Util::parse32("-5");
    Util::ValueType32 pFive32 = Util::parse32("+5");

    Util::ValueType32 nSeventeen32 = Util::parse32("-17");
    Util::ValueType32 pSeventeen32 = Util::parse32("+17");

    ASSERT(Util::equal(pZero32, Util::negate(nZero32)));
    ASSERT(Util::equal(nZero32, Util::negate(pZero32)));

    // Check for zero sign correctly...

    {
        Util::ValueType32 tmp = Util::negate(pZero32);
        ASSERT(!bsl::memcmp(&nZero32, &tmp, sizeof(tmp)));

        tmp = Util::negate(nZero32);
        ASSERT(!bsl::memcmp(&pZero32, &tmp, sizeof(tmp)));
    }

    ASSERT(Util::equal(pFive32, Util::negate(nFive32)));
    ASSERT(Util::equal(nFive32, Util::negate(pFive32)));

    ASSERT(Util::equal(pSeventeen32, Util::negate(nSeventeen32)));
    ASSERT(Util::equal(nSeventeen32, Util::negate(pSeventeen32)));

    Util::ValueType64 nZero64 = Util::parse64("-0");
    Util::ValueType64 pZero64 = Util::parse64("+0");

    Util::ValueType64 nFive64 = Util::parse64("-5");
    Util::ValueType64 pFive64 = Util::parse64("+5");

    Util::ValueType64 nSeventeen64 = Util::parse64("-17");
    Util::ValueType64 pSeventeen64 = Util::parse64("+17");

    ASSERT(Util::equal(pZero64, Util::negate(nZero64)));
    ASSERT(Util::equal(nZero64, Util::negate(pZero64)));

    // Check for zero sign correctly...

    {
        Util::ValueType64 tmp = Util::negate(pZero64);
        ASSERT(!bsl::memcmp(&nZero64, &tmp, sizeof(tmp)));

        tmp = Util::negate(nZero64);
        ASSERT(!bsl::memcmp(&pZero64, &tmp, sizeof(tmp)));
    }

    ASSERT(Util::equal(pFive64, Util::negate(nFive64)));
    ASSERT(Util::equal(nFive64, Util::negate(pFive64)));

    ASSERT(Util::equal(pSeventeen64, Util::negate(nSeventeen64)));
    ASSERT(Util::equal(nSeventeen64, Util::negate(pSeventeen64)));

    Util::ValueType128 nZero128 = Util::parse128("-0");
    Util::ValueType128 pZero128 = Util::parse128("+0");

    Util::ValueType128 nFive128 = Util::parse128("-5");
    Util::ValueType128 pFive128 = Util::parse128("+5");

    Util::ValueType128 nSeventeen128 = Util::parse128("-17");
    Util::ValueType128 pSeventeen128 = Util::parse128("+17");

    ASSERT(Util::equal(pZero128, Util::negate(nZero128)));
    ASSERT(Util::equal(nZero128, Util::negate(pZero128)));

    // Check for zero sign correctly...

    {
        Util::ValueType128 tmp = Util::negate(pZero128);
        ASSERT(!bsl::memcmp(&nZero128, &tmp, sizeof(tmp)));

        tmp = Util::negate(nZero128);
        ASSERT(!bsl::memcmp(&pZero128, &tmp, sizeof(tmp)));
    }

    ASSERT(Util::equal(pFive128, Util::negate(nFive128)));
    ASSERT(Util::equal(nFive128, Util::negate(pFive128)));

    ASSERT(Util::equal(pSeventeen128, Util::negate(nSeventeen128)));
    ASSERT(Util::equal(nSeventeen128, Util::negate(pSeventeen128)));
}

void TestDriver::testCase8()
{
    // ------------------------------------------------------------------------
    // ARITHMETIC FUNCTION 'divide'
    //
    // Concerns:
    //:  1 'divide' should never (except in identity, 'NaN' and 'Inf' cases)
    //:    cases) return the same value as at least one of the operands.
    //:
    //:  2 'divide' behaves correctly with identity operand (1).
    //:
    //:  3 'divide' will return a distinct value from both of its operands,
    //:    when both are non-zero and value-states (not 'NaN' or 'Inf').
    //:
    //:  4 'divide' to 'NaN' or 'Inf' should follow the IEEE rules for the
    //:    cases listed in this chart:
    //:
    //:             L / R|-Inf|Normal|+Inf|NaN|
    //:            ------+----+------+----+---+
    //:             -Inf | NaN|+/-Inf| NaN|NaN|
    //:            ------+----+------+----+---+
    //:            Normal|+/-0|Normal|+/-0|NaN|
    //:            ------+----+------+----+---+
    //:             +Inf | NaN|+/-Inf| NaN|NaN|
    //:            ------+----+------+----+---+
    //:              NaN | NaN|  NaN | NaN|NaN|
    //:
    //:  5 'divide' behaves reasonably when working with different quanta.
    //:
    //:  6 'divide' by 0 gives a 'NaN'.
    //
    // Plan:
    //:  1 Test a handful of expected values of different exponent and
    //:    mantissa, with both possible orders. (C-1,5,6)
    //:
    //:  2 Test identity with '1' case. (C-2)
    //:
    //:  3 When not dealing with special cases (or rounding cases), make sure
    //:    the result value does not compare equal to either source.  (C-3)
    //:
    //:  4 Test all 16 special cases in the chart, which covers both orders.
    //:    (C-4,6)
    //
    // Testing:
    //   divide(ValueType64,  ValueType64)
    //   divide(ValueType128, ValueType128)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "ARITHMETIC FUNCTION 'divide'" << endl
                      << "============================" << endl;

    Util::ValueType64     lhs64;
    Util::ValueType64     rhs64;
    Util::ValueType64  result64;

    Util::ValueType128    lhs128;
    Util::ValueType128    rhs128;
    Util::ValueType128 result128;

    struct  {
        long long int lhsMantissa;
                  int lhsExponent;
        long long int rhsMantissa;
                  int rhsExponent;
        long long int resMantissa;
                  int resExponent;
    } testCases[] = {
        {  0,  0,  1,  0,  0,  0 },
        {  1,  0,  1,  0,  1,  0 },
        {  1,  0,  1,  0,  1,  0 },
        { 42,  0,  1,  0, 42,  0 },
        {  1,  0, 10,  0, 10, -2 },
        { 42,  0,  1,  1, 42, -1 },
        {  1,  1, 10,  0, 10, -1 },
        { 42,  0,  1,  2, 42, -2 },
        {  1,  2, 10,  0, 10,  0 }
    };
    const int numTestCases = sizeof(testCases) / sizeof(*testCases);

    for (int i = 0; i < numTestCases; ++i) {
        long long int lhsMantissa = testCases[ i ].lhsMantissa;
                  int lhsExponent = testCases[ i ].lhsExponent;
        long long int rhsMantissa = testCases[ i ].rhsMantissa;
                  int rhsExponent = testCases[ i ].rhsExponent;
        long long int resMantissa = testCases[ i ].resMantissa;
                  int resExponent = testCases[ i ].resExponent;

        if (veryVerbose) cout << endl
                              << "Test 'divide(ValueType64,"
                              << " ValueType64)'" << endl
                              << "and 'divide(ValueType128,"
                              << " ValueType128)' on" << endl
                              << lhsMantissa << "e" << lhsExponent
                              << " + "
                              << rhsMantissa << "e" << rhsExponent
                              << " == "
                              << resMantissa << "e" << resExponent << endl;

        Util::ValueType64 identity64 = Util::parse64("1");

           lhs64 = Util::makeDecimalRaw64(lhsMantissa, lhsExponent);
           rhs64 = Util::makeDecimalRaw64(rhsMantissa, rhsExponent);
        result64 = Util::divide(lhs64, rhs64);

        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(result64, Util::makeDecimalRaw64(resMantissa,
                                                            resExponent)));
        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(lhs64, Util::divide(lhs64, identity64)));


        Util::ValueType128 identity128 = Util::parse128("1");

           lhs128 = Util::makeDecimalRaw128(lhsMantissa, lhsExponent);
           rhs128 = Util::makeDecimalRaw128(rhsMantissa, rhsExponent);
        result128 = Util::divide(lhs128, rhs128);

        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(result128, Util::makeDecimalRaw128(resMantissa,
                                                            resExponent)));
        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(lhs128, Util::divide(lhs128,
                                                  identity128)));
    }


                    // Testing for 'NaN'/'Inf' special cases:
    {
        //:             L / R|-Inf|Normal|+Inf|NaN|
        //:            ------+----+------+----+---+
        //:             -Inf | NaN|+/-Inf| NaN|NaN|
        //:            ------+----+------+----+---+
        //:            Normal|+/-0|Normal|+/-0|NaN|
        //:            ------+----+------+----+---+
        //:             +Inf | NaN|+/-Inf| NaN|NaN|
        //:            ------+----+------+----+---+
        //:              NaN | NaN|  NaN | NaN|NaN|

        Util::ValueType64   ninf64 = Util::parse64("-Inf");
        Util::ValueType64   pinf64 = Util::parse64("+Inf");
        Util::ValueType64    nan64 = Util::parse64( "NaN");
        Util::ValueType64   zero64 = Util::parse64(   "0");
        Util::ValueType64 normal64 = Util::makeDecimalRaw64(42, 1);

        ASSERT(nanEqual( nan64, Util::divide(  ninf64,   ninf64)));
        ASSERT(nanEqual(ninf64, Util::divide(  ninf64, normal64)));
        ASSERT(nanEqual( nan64, Util::divide(  ninf64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::divide(  ninf64,    nan64)));

        ASSERT(nanEqual(zero64, Util::divide(normal64,   ninf64)));
        ASSERT(nanEqual(        Util::divide(normal64, normal64),
                        Util::makeDecimalRaw64(1, 0)));
        ASSERT(nanEqual(zero64, Util::divide(normal64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::divide(normal64,    nan64)));

        ASSERT(nanEqual( nan64, Util::divide(  pinf64,   ninf64)));
        ASSERT(nanEqual(pinf64, Util::divide(  pinf64, normal64)));
        ASSERT(nanEqual( nan64, Util::divide(  pinf64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::divide(  pinf64,    nan64)));

        ASSERT(nanEqual( nan64, Util::divide(   nan64,   ninf64)));
        ASSERT(nanEqual( nan64, Util::divide(   nan64, normal64)));
        ASSERT(nanEqual( nan64, Util::divide(   nan64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::divide(   nan64,    nan64)));

        // Zero checks:

        ASSERT(nanEqual( nan64, Util::divide(  zero64, zero64)));
        ASSERT(nanEqual(pinf64, Util::divide(normal64, zero64)));
        ASSERT(nanEqual(ninf64, Util::divide(Util::makeDecimalRaw64(-1, 0),
                                zero64)));


        Util::ValueType128   ninf128 = Util::parse128("-Inf");
        Util::ValueType128   pinf128 = Util::parse128("+Inf");
        Util::ValueType128    nan128 = Util::parse128( "NaN");
        Util::ValueType128   zero128 = Util::parse128(   "0");
        Util::ValueType128 normal128 = Util::makeDecimalRaw128(42,1);

        ASSERT(nanEqual( nan128, Util::divide(  ninf128,   ninf128)));
        ASSERT(nanEqual(ninf128, Util::divide(  ninf128, normal128)));
        ASSERT(nanEqual( nan128, Util::divide(  ninf128,   pinf128)));
        ASSERT(nanEqual( nan128, Util::divide(  ninf128,    nan128)));

        ASSERT(nanEqual(zero128, Util::divide(normal128,   ninf128)));
        ASSERT(nanEqual(         Util::divide(normal128, normal128),
                        Util::makeDecimalRaw128(1, 0)));
        ASSERT(nanEqual(zero128, Util::divide(normal128,   pinf128)));
        ASSERT(nanEqual( nan128, Util::divide(normal128,    nan128)));

        ASSERT(nanEqual( nan128, Util::divide(  pinf128,   ninf128)));
        ASSERT(nanEqual(pinf128, Util::divide(  pinf128, normal128)));
        ASSERT(nanEqual( nan128, Util::divide(  pinf128,   pinf128)));
        ASSERT(nanEqual( nan128, Util::divide(  pinf128,    nan128)));

        ASSERT(nanEqual( nan128, Util::divide(   nan128,   ninf128)));
        ASSERT(nanEqual( nan128, Util::divide(   nan128, normal128)));
        ASSERT(nanEqual( nan128, Util::divide(   nan128,   pinf128)));
        ASSERT(nanEqual( nan128, Util::divide(   nan128,    nan128)));

        // Zero checks:

        ASSERT(nanEqual( nan128, Util::divide(  zero128, zero128)));
        ASSERT(nanEqual(pinf128, Util::divide(normal128, zero128)));
        ASSERT(nanEqual(ninf128,
                   Util::divide(Util::makeDecimalRaw128(-1, 0), zero128)));
    }
}

void TestDriver::testCase7()
{
    // ------------------------------------------------------------------------
    // ARITHMETIC FUNCTION 'multiply'
    //
    // Concerns:
    //:  1 'multiply' should never (except in identity, 'NaN' and 'Inf' cases)
    //:    return the same value as at least one of the operands.
    //:
    //:  2 'multiply' behaves correctly with identity operand (1).
    //:
    //:  3 'multiply' will return a distinct value from both of its operands,
    //:    when both are non-zero and value-states (not 'NaN' or 'Inf').
    //:
    //:  4 'multiply' to 'NaN' or 'Inf' should follow the IEEE rules for the
    //:    cases listed in this chart:
    //:
    //:             L * R|-Inf|Normal|+Inf|NaN|
    //:            ------+----+------+----+---+
    //:             -Inf |+Inf|+/-Inf|+Inf|NaN|
    //:            ------+----+------+----+---+
    //:            Normal| Inf|Normal| Inf|NaN|
    //:            ------+----+------+----+---+
    //:             +Inf |-Inf|+/-Inf|+Inf|NaN|
    //:            ------+----+------+----+---+
    //:              NaN | NaN|  NaN | NaN|NaN|
    //:
    //:  5 'multiply' behaves reasonably when working with different quanta.
    //:
    //:  6 'multiply' by 0 behaves as expected.
    //
    // Plan:
    //:  1 Test a handful of expected values of different exponent and
    //:    mantissa, with both possible orders. (C-1,5,6)
    //:
    //:  2 Test identity with '1' case. (C-2)
    //:
    //:  3 When not dealing with special cases (or rounding cases), make sure
    //:    the result value does not compare equal to either source. (C-3)
    //:
    //:  4 Test all 16 special cases in the chart, which covers both
    //:    orders. (C-4,6)
    //
    // Testing:
    //   multiply(ValueType64,  ValueType64)
    //   multiply(ValueType128, ValueType128)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "ARITHMETIC FUNCTION 'multiply'" << endl
                      << "==============================" << endl;

    Util::ValueType64     lhs64;
    Util::ValueType64     rhs64;
    Util::ValueType64  result64;

    Util::ValueType128    lhs128;
    Util::ValueType128    rhs128;
    Util::ValueType128 result128;

    struct  {
        long long int lhsMantissa;
                  int lhsExponent;
        long long int rhsMantissa;
                  int rhsExponent;
        long long int resMantissa;
                  int resExponent;
    } testCases[] = {
        {  0,  0,  0,  0,  0,  0 },
        {  1,  0,  0,  0,  0,  0 },
        {  0,  0,  1,  0,  0,  0 },
        {  1,  0,  1,  0,  1,  0 },
        {  1,  0,  1,  0,  1,  0 },
        { 42,  0,  1,  0, 42,  0 },
        {  1,  0, 42,  0, 42,  0 },
        { 42,  0,  1,  1, 42,  1 },
        {  1,  1, 42,  0, 42,  1 },
        { 42,  0,  1,  2, 42,  2 },
        {  1,  2, 42,  0, 42,  2 }
    };
    const int numTestCases = sizeof(testCases) / sizeof(*testCases);

    for (int i = 0; i < numTestCases; ++i) {
        long long int lhsMantissa = testCases[ i ].lhsMantissa;
                  int lhsExponent = testCases[ i ].lhsExponent;
        long long int rhsMantissa = testCases[ i ].rhsMantissa;
                  int rhsExponent = testCases[ i ].rhsExponent;
        long long int resMantissa = testCases[ i ].resMantissa;
                  int resExponent = testCases[ i ].resExponent;

        if (veryVerbose) cout << endl
                              << "Test 'multiply(ValueType64,"
                              << " ValueType64)'" << endl
                              << "and 'multiply(ValueType128,"
                              << " ValueType128)' on" << endl
                              << lhsMantissa << "e" << lhsExponent
                              << " + "
                              << rhsMantissa << "e" << rhsExponent
                              << " == "
                              << resMantissa << "e" << resExponent << endl;

        Util::ValueType64 identity64 = Util::parse64("1");

           lhs64 = Util::makeDecimalRaw64(lhsMantissa, lhsExponent);
           rhs64 = Util::makeDecimalRaw64(rhsMantissa, rhsExponent);
        result64 = Util::multiply(lhs64, rhs64);

        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(result64, Util::makeDecimalRaw64(resMantissa,
                                                            resExponent)));
        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(lhs64, Util::multiply(lhs64, identity64)));


        Util::ValueType128 identity128 = Util::parse128("1");

           lhs128 = Util::makeDecimalRaw128(lhsMantissa, lhsExponent);
           rhs128 = Util::makeDecimalRaw128(rhsMantissa, rhsExponent);
        result128 = Util::multiply(lhs128, rhs128);

        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(result128, Util::makeDecimalRaw128(resMantissa,
                                                            resExponent)));
        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(lhs128, Util::multiply(lhs128,
                                                  identity128)));
    }


                    // Testing for 'NaN'/'Inf' special cases:
    {
        //:             L * R|-Inf|Normal|+Inf|NaN|
        //:            ------+----+------+----+---+
        //:             -Inf |+Inf|+/-Inf|+Inf|NaN|
        //:            ------+----+------+----+---+
        //:            Normal| Inf|Normal| Inf|NaN|
        //:            ------+----+------+----+---+
        //:             +Inf |-Inf|+/-Inf|+Inf|NaN|
        //:            ------+----+------+----+---+
        //:              NaN | NaN|  NaN | NaN|NaN|

        Util::ValueType64   ninf64 = Util::parse64("-Inf");
        Util::ValueType64   pinf64 = Util::parse64("+Inf");
        Util::ValueType64    nan64 = Util::parse64( "NaN");
        Util::ValueType64 normal64 = Util::makeDecimalRaw64(42,1);

        ASSERT(nanEqual(pinf64, Util::multiply(  ninf64,   ninf64)));
        ASSERT(nanEqual(ninf64, Util::multiply(  ninf64, normal64)));
        ASSERT(nanEqual(ninf64, Util::multiply(  ninf64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::multiply(  ninf64,    nan64)));

        ASSERT(nanEqual(ninf64, Util::multiply(normal64,   ninf64)));
        ASSERT(nanEqual(        Util::multiply(normal64, normal64),
                        Util::makeDecimalRaw64(42*42,2)));
        ASSERT(nanEqual(pinf64, Util::multiply(normal64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::multiply(normal64,    nan64)));

        ASSERT(nanEqual(ninf64, Util::multiply(  pinf64,   ninf64)));
        ASSERT(nanEqual(pinf64, Util::multiply(  pinf64, normal64)));
        ASSERT(nanEqual(pinf64, Util::multiply(  pinf64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::multiply(  pinf64,    nan64)));

        ASSERT(nanEqual( nan64, Util::multiply(   nan64,   ninf64)));
        ASSERT(nanEqual( nan64, Util::multiply(   nan64, normal64)));
        ASSERT(nanEqual( nan64, Util::multiply(   nan64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::multiply(   nan64,    nan64)));


        Util::ValueType128   ninf128 = Util::parse128("-Inf");
        Util::ValueType128   pinf128 = Util::parse128("+Inf");
        Util::ValueType128    nan128 = Util::parse128( "NaN");
        Util::ValueType128 normal128 = Util::makeDecimalRaw128(42,1);

        ASSERT(nanEqual(pinf128, Util::multiply(  ninf128,   ninf128)));
        ASSERT(nanEqual(ninf128, Util::multiply(  ninf128, normal128)));
        ASSERT(nanEqual(ninf128, Util::multiply(  ninf128,   pinf128)));
        ASSERT(nanEqual( nan128, Util::multiply(  ninf128,    nan128)));

        ASSERT(nanEqual(ninf128, Util::multiply(normal128,   ninf128)));
        ASSERT(nanEqual(         Util::multiply(normal128, normal128),
                        Util::makeDecimalRaw128(42*42, 2)));
        ASSERT(nanEqual(pinf128, Util::multiply(normal128,   pinf128)));
        ASSERT(nanEqual( nan128, Util::multiply(normal128,    nan128)));

        ASSERT(nanEqual(ninf128, Util::multiply(  pinf128,   ninf128)));
        ASSERT(nanEqual(pinf128, Util::multiply(  pinf128, normal128)));
        ASSERT(nanEqual(pinf128, Util::multiply(  pinf128,   pinf128)));
        ASSERT(nanEqual( nan128, Util::multiply(  pinf128,    nan128)));

        ASSERT(nanEqual( nan128, Util::multiply(   nan128,   ninf128)));
        ASSERT(nanEqual( nan128, Util::multiply(   nan128, normal128)));
        ASSERT(nanEqual( nan128, Util::multiply(   nan128,   pinf128)));
        ASSERT(nanEqual( nan128, Util::multiply(   nan128,    nan128)));
    }
}

void TestDriver::testCase6()
{
    // ------------------------------------------------------------------------
    // ARITHMETIC FUNCTION 'subtract'
    //
    // Concerns:
    //:  1 'subtract' should never (except in identity, 'NaN' and 'Inf' cases)
    //:    return the same value as at least one of the operands.
    //:
    //:  2 'subtract' behaves correctly with identity operands (0 and -0).
    //:
    //:  3 'subtract' will return a distinct value from both of its operands,
    //:    when both are non-zero and value-states (not 'NaN' or 'Inf').
    //:
    //:  4 'subtract' to 'NaN' or 'Inf' should follow the IEEE rules for the
    //:    cases listed in this chart:
    //:
    //:             L - R|-Inf|Normal|+Inf|NaN|
    //:            ------+----+------+----+---+
    //:             -Inf | NaN| -Inf |-Inf|NaN|
    //:            ------+----+------+----+---+
    //:            Normal|+Inf|Normal|-Inf|NaN|
    //:            ------+----+------+----+---+
    //:             +Inf |+Inf| +Inf | NaN|NaN|
    //:            ------+----+------+----+---+
    //:              NaN | NaN|  NaN | NaN|NaN|
    //:
    //:  5 'subtract' behaves reasonably when working with different
    //:    quanta.
    //
    // Plan:
    //:  1 Test a handful of expected values of different exponent and
    //:    mantissa, with both possible orders. (C-1,5,6)
    //:
    //:  2 Test identity with both '-0' and '0' cases. (C-2)
    //:
    //:  3 When not dealing with special cases (or rounding cases), make sure
    //:    the result value does not compare equal to either source. (C-3)
    //:
    //:  4 Test all 16 special cases in the chart, which covers both orders.
    //:    (C-4,6)
    //
    // Testing:
    //   subtract(ValueType64,  ValueType64)
    //   subtract(ValueType128, ValueType128)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "ARITHMETIC FUNCTION 'subtract'" << endl
                      << "==============================" << endl;

    Util::ValueType64     lhs64;
    Util::ValueType64     rhs64;
    Util::ValueType64  result64;

    Util::ValueType128    lhs128;
    Util::ValueType128    rhs128;
    Util::ValueType128 result128;

    struct  {
        long long int lhsMantissa;
                  int lhsExponent;
        long long int rhsMantissa;
                  int rhsExponent;
        long long int resMantissa;
                  int resExponent;
    } testCases[] = {
        {  0,  0,  0,  0,  0,  0 },
        {  1,  0,  0,  0,  1,  0 },
        {  0,  0,  1,  0, -1,  0 },
        { 42,  0,  0,  0, 42,  0 },
        {  0,  0, 42,  0,-42,  0 },
        { 42,  0,  1,  0, 41,  0 },
        {  1,  0, 42,  0,-41,  0 },
        { 42,  0,  1,  1, 32,  0 },
        {  1,  1, 42,  0,-32,  0 }
    };
    const int numTestCases = sizeof(testCases) / sizeof(*testCases);

    for (int i = 0; i < numTestCases; ++i) {
        long long int lhsMantissa = testCases[ i ].lhsMantissa;
                  int lhsExponent = testCases[ i ].lhsExponent;
        long long int rhsMantissa = testCases[ i ].rhsMantissa;
                  int rhsExponent = testCases[ i ].rhsExponent;
        long long int resMantissa = testCases[ i ].resMantissa;
                  int resExponent = testCases[ i ].resExponent;

        if (veryVerbose) cout << endl
                              << "Test 'subtract(ValueType64,"
                              << " ValueType64)'" << endl
                              << "and 'subtract(ValueType128,"
                              << " ValueType128)' on" << endl
                              << lhsMantissa << "e" << lhsExponent
                              << " + "
                              << rhsMantissa << "e" << rhsExponent
                              << " == "
                              << resMantissa << "e" << resExponent << endl;

        Util::ValueType64 negativeZero64 = Util::parse64("-0");

           lhs64 = Util::makeDecimalRaw64(lhsMantissa, lhsExponent);
           rhs64 = Util::makeDecimalRaw64(rhsMantissa, rhsExponent);
        result64 = Util::subtract(lhs64, rhs64);

        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
                     Util::equal(result64,
                                 Util::makeDecimalRaw64(resMantissa,
                                                        resExponent)));
        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(lhs64, Util::subtract(lhs64, negativeZero64)));


        Util::ValueType128 negativeZero128 = Util::parse128("-0");

           lhs128 = Util::makeDecimalRaw128(lhsMantissa, lhsExponent);
           rhs128 = Util::makeDecimalRaw128(rhsMantissa, rhsExponent);
        result128 = Util::subtract(lhs128, rhs128);

        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
                     Util::equal(result128,
                                 Util::makeDecimalRaw128(resMantissa,
                                                         resExponent)));
        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
                     Util::equal(lhs128,
                                 Util::subtract(lhs128,
                                                negativeZero128)));
    }


                    // Testing for 'NaN'/'Inf' special cases:
    {
        //:             L - R|-Inf|Normal|+Inf|NaN|
        //:            ------+----+------+----+---+
        //:             -Inf | NaN| -Inf |-Inf|NaN|
        //:            ------+----+------+----+---+
        //:            Normal|+Inf|Normal|-Inf|NaN|
        //:            ------+----+------+----+---+
        //:             +Inf |+Inf| +Inf | NaN|NaN|
        //:            ------+----+------+----+---+
        //:              NaN | NaN|  NaN | NaN|NaN|

        Util::ValueType64   ninf64 = Util::parse64("-Inf");
        Util::ValueType64   pinf64 = Util::parse64("+Inf");
        Util::ValueType64    nan64 = Util::parse64( "NaN");
        Util::ValueType64 normal64 = Util::makeDecimalRaw64(42,1);

        ASSERT(nanEqual( nan64, Util::subtract(  ninf64,   ninf64)));
        ASSERT(nanEqual(ninf64, Util::subtract(  ninf64, normal64)));
        ASSERT(nanEqual(ninf64, Util::subtract(  ninf64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::subtract(  ninf64,    nan64)));

        ASSERT(nanEqual(pinf64, Util::subtract(normal64,   ninf64)));
        ASSERT(nanEqual(        Util::subtract(normal64, normal64),
                        Util::makeDecimalRaw64( 0, 1)));
        ASSERT(nanEqual(ninf64, Util::subtract(normal64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::subtract(normal64,    nan64)));

        ASSERT(nanEqual(pinf64, Util::subtract(  pinf64,   ninf64)));
        ASSERT(nanEqual(pinf64, Util::subtract(  pinf64, normal64)));
        ASSERT(nanEqual( nan64, Util::subtract(  pinf64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::subtract(  pinf64,    nan64)));

        ASSERT(nanEqual( nan64, Util::subtract(   nan64,   ninf64)));
        ASSERT(nanEqual( nan64, Util::subtract(   nan64, normal64)));
        ASSERT(nanEqual( nan64, Util::subtract(   nan64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::subtract(   nan64,    nan64)));


        Util::ValueType128   ninf128 = Util::parse128("-Inf");
        Util::ValueType128   pinf128 = Util::parse128("+Inf");
        Util::ValueType128    nan128 = Util::parse128( "NaN");
        Util::ValueType128 normal128 = Util::makeDecimalRaw128(42,1);

        ASSERT(nanEqual( nan128, Util::subtract(  ninf128,   ninf128)));
        ASSERT(nanEqual(ninf128, Util::subtract(  ninf128, normal128)));
        ASSERT(nanEqual(ninf128, Util::subtract(  ninf128,   pinf128)));
        ASSERT(nanEqual( nan128, Util::subtract(  ninf128,    nan128)));

        ASSERT(nanEqual(pinf128, Util::subtract(normal128,   ninf128)));
        ASSERT(nanEqual(         Util::subtract(normal128, normal128),
                        Util::makeDecimalRaw128( 0, 1)));
        ASSERT(nanEqual(ninf128, Util::subtract(normal128,   pinf128)));
        ASSERT(nanEqual( nan128, Util::subtract(normal128,    nan128)));

        ASSERT(nanEqual(pinf128, Util::subtract(  pinf128,   ninf128)));
        ASSERT(nanEqual(pinf128, Util::subtract(  pinf128, normal128)));
        ASSERT(nanEqual( nan128, Util::subtract(  pinf128,   pinf128)));
        ASSERT(nanEqual( nan128, Util::subtract(  pinf128,    nan128)));

        ASSERT(nanEqual( nan128, Util::subtract(   nan128,   ninf128)));
        ASSERT(nanEqual( nan128, Util::subtract(   nan128, normal128)));
        ASSERT(nanEqual( nan128, Util::subtract(   nan128,   pinf128)));
        ASSERT(nanEqual( nan128, Util::subtract(   nan128,    nan128)));
    }
}

void TestDriver::testCase5()
{
    // ------------------------------------------------------------------------
    // ARITHMETIC FUNCTION 'add'
    //
    // Concerns:
    //:  1 'add' should never (except in 'NaN' and 'Inf' cases) return the same
    //:    value as at least one of the operands.
    //:
    //:  2 'add' behaves correctly with identity operands (0 and -0).
    //:
    //:  3 'add' will return a distinct value from both of its operands, when
    //:    both are non-zero and value-states (not 'NaN' or 'Inf').
    //:
    //:  4 'add' to 'NaN' or 'Inf' should follow the IEEE rules for the cases
    //:    listed in this chart:
    //:
    //:             L + R|-Inf|Normal|+Inf|NaN|
    //:            ------+----+------+----+---+
    //:             -Inf |-Inf| -Inf | NaN|NaN|
    //:            ------+----+------+----+---+
    //:            Normal|-Inf|Normal|+Inf|NaN|
    //:            ------+----+------+----+---+
    //:             +Inf | NaN| +Inf |+Inf|NaN|
    //:            ------+----+------+----+---+
    //:              NaN | NaN|  NaN | NaN|NaN|
    //:
    //:  5 'add' behaves reasonably when working with different quanta.
    //:
    //:  6 'add' must be a commutative operation (when avoiding corner cases).
    //
    // Plan:
    //:  1 Test a handful of expected values of different exponent and
    //:    mantissa, with both possible orders. (C-1,5,6)
    //:
    //:  2 Test identity with both '-0' and '0' cases. (C-2)
    //:
    //:  3 When not dealing with special cases (or rounding cases), make sure
    //:    the result value does not compare equal to either source.  (C-3)
    //:
    //:  4 Test all 16 special cases in the chart, which covers both orders.
    //:    (C-4,6)
    //
    // Testing:
    //   add(ValueType64,  ValueType64)
    //   add(ValueType128, ValueType128)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "ARITHMETIC FUNCTION 'add'" << endl
                      << "=========================" << endl;

    Util::ValueType64     lhs64;
    Util::ValueType64     rhs64;
    Util::ValueType64  result64;

    Util::ValueType128    lhs128;
    Util::ValueType128    rhs128;
    Util::ValueType128 result128;


    struct {
        long long int lhsMantissa;
                  int lhsExponent;
        long long int rhsMantissa;
                  int rhsExponent;
        long long int resMantissa;
                  int resExponent;
    } testCases[] = {
        {  0,  0,  0,  0,  0,  0 },
        {  1,  0,  0,  0,  1,  0 },
        {  0,  0,  1,  0,  1,  0 },
        { 42,  0,  0,  0, 42,  0 },
        {  0,  0, 42,  0, 42,  0 },
        { 42,  0,  1,  0, 43,  0 },
        {  1,  0, 42,  0, 43,  0 },
        { 42,  0,  1,  1, 52,  0 },
        {  1,  1, 42,  0, 52,  0 }
    };
    const int numTestCases = sizeof(testCases) / sizeof(*testCases);

    for (int i = 0; i < numTestCases; ++i) {
        long long int lhsMantissa = testCases[ i ].lhsMantissa;
                  int lhsExponent = testCases[ i ].lhsExponent;
        long long int rhsMantissa = testCases[ i ].rhsMantissa;
                  int rhsExponent = testCases[ i ].rhsExponent;
        long long int resMantissa = testCases[ i ].resMantissa;
                  int resExponent = testCases[ i ].resExponent;

        if (veryVerbose) cout << endl
                              << "Test 'add(ValueType64,"
                              << " ValueType64)'" << endl
                              << "and 'add(ValueType128,"
                              << " ValueType128)' on" << endl
                              << lhsMantissa << "e" << lhsExponent
                              << " + "
                              << rhsMantissa << "e" << rhsExponent
                              << " == "
                              << resMantissa << "e" << resExponent << endl;

        Util::ValueType64 negativeZero64 = Util::parse64("-0");

           lhs64 = Util::makeDecimalRaw64(lhsMantissa, lhsExponent);
           rhs64 = Util::makeDecimalRaw64(rhsMantissa, rhsExponent);
        result64 = Util::add(lhs64, rhs64);

        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(result64, Util::makeDecimalRaw64(resMantissa,
                                                            resExponent)));
        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(lhs64, Util::add(lhs64, negativeZero64)));
        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(lhs64, Util::add(negativeZero64, lhs64)));


        Util::ValueType128 negativeZero128 = Util::parse128("-0");

           lhs128 = Util::makeDecimalRaw128(lhsMantissa, lhsExponent);
           rhs128 = Util::makeDecimalRaw128(rhsMantissa, rhsExponent);
        result128 = Util::add(lhs128, rhs128);

        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(result128, Util::makeDecimalRaw128(resMantissa,
                                                            resExponent)));
        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(lhs128, Util::add(lhs128, negativeZero128)));
        LOOP6_ASSERT(lhsMantissa, lhsExponent,
                     rhsMantissa, rhsExponent,
                     resMantissa, resExponent,
               Util::equal(lhs128, Util::add(negativeZero128, lhs128)));
    }

                    // Testing for 'NaN'/'Inf' special cases:

    {
        //:             L + R|-Inf|Normal|+Inf|NaN|
        //:            ------+----+------+----+---+
        //:             -Inf |-Inf| -Inf | NaN|NaN|
        //:            ------+----+------+----+---+
        //:            Normal|-Inf|Normal|+Inf|NaN|
        //:            ------+----+------+----+---+
        //:             +Inf | NaN| +Inf |+Inf|NaN|
        //:            ------+----+------+----+---+
        //:              NaN | NaN|  NaN | NaN|NaN|

        Util::ValueType64   ninf64 = Util::parse64("-Inf");
        Util::ValueType64   pinf64 = Util::parse64("+Inf");
        Util::ValueType64    nan64 = Util::parse64( "NaN");
        ASSERT( Util::equal(pinf64, pinf64));
        ASSERT( Util::equal(ninf64, ninf64));
        ASSERT( Util::notEqual(pinf64, ninf64));
        ASSERT(!Util::equal(nan64, nan64));
        ASSERT( Util::notEqual(nan64, nan64));
        Util::ValueType64 normal64 = Util::makeDecimalRaw64(42,1);

        ASSERT(nanEqual(ninf64, Util::add(  ninf64,   ninf64)));
        ASSERT(nanEqual(ninf64, Util::add(  ninf64, normal64)));
        ASSERT(nanEqual( nan64, Util::add(  ninf64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::add(  ninf64,    nan64)));

        ASSERT(nanEqual(ninf64, Util::add(normal64,   ninf64)));
        ASSERT(nanEqual(        Util::add(normal64, normal64),
                        Util::makeDecimalRaw64(84,1)));
        ASSERT(nanEqual(pinf64, Util::add(normal64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::add(normal64,    nan64)));

        ASSERT(nanEqual( nan64, Util::add(  pinf64,   ninf64)));
        ASSERT(nanEqual(pinf64, Util::add(  pinf64, normal64)));
        ASSERT(nanEqual(pinf64, Util::add(  pinf64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::add(  pinf64,    nan64)));

        ASSERT(nanEqual( nan64, Util::add(   nan64,   ninf64)));
        ASSERT(nanEqual( nan64, Util::add(   nan64, normal64)));
        ASSERT(nanEqual( nan64, Util::add(   nan64,   pinf64)));
        ASSERT(nanEqual( nan64, Util::add(   nan64,    nan64)));


        Util::ValueType128   ninf128 = Util::parse128("-Inf");
        Util::ValueType128   pinf128 = Util::parse128("+Inf");
        Util::ValueType128    nan128 = Util::parse128( "NaN");
        Util::ValueType128 normal128 = Util::makeDecimalRaw128(42,1);

        ASSERT(nanEqual(  ninf128, Util::add(  ninf128,   ninf128)));
        ASSERT(nanEqual(  ninf128, Util::add(  ninf128, normal128)));
        ASSERT(nanEqual(   nan128, Util::add(  ninf128,   pinf128)));
        ASSERT(nanEqual(   nan128, Util::add(  ninf128,    nan128)));

        ASSERT(nanEqual(  ninf128, Util::add(normal128,   ninf128)));
        ASSERT(nanEqual(           Util::add(normal128, normal128),
                        Util::makeDecimalRaw128(84,1)));
        ASSERT(nanEqual(  pinf128, Util::add(normal128,   pinf128)));
        ASSERT(nanEqual(   nan128, Util::add(normal128,    nan128)));

        ASSERT(nanEqual(   nan128, Util::add(  pinf128,   ninf128)));
        ASSERT(nanEqual(  pinf128, Util::add(  pinf128, normal128)));
        ASSERT(nanEqual(  pinf128, Util::add(  pinf128,   pinf128)));
        ASSERT(nanEqual(   nan128, Util::add(  pinf128,    nan128)));

        ASSERT(nanEqual(   nan128, Util::add(   nan128,   ninf128)));
        ASSERT(nanEqual(   nan128, Util::add(   nan128, normal128)));
        ASSERT(nanEqual(   nan128, Util::add(   nan128,   pinf128)));
        ASSERT(nanEqual(   nan128, Util::add(   nan128,    nan128)));
    }
}

void TestDriver::testCase4()
{
    // ------------------------------------------------------------------------
    // INEQUALITY COMPARISON 'Util::notEqual'
    //
    // Concerns:
    //:  1 'notEqual' should return false if the bit patterns are the same (and
    //:    are not a bit pattern representing a 'NaN').
    //:
    //:  2 'notEqual' should return false if the bit patterns are different,
    //:    but represent the same value.  I.e. '10e0' should be equal to '1e1'.
    //:
    //:  3 Values which differ by only one of sign, exponent, or mantissa
    //:    should not be equal.
    //:
    //:  4 'NaN' values should never be equal.
    //:
    //:  5 The implementation of 'notEqual' normally would be a forwarding
    //:    function (to the underlying implementation), but some
    //:    implementations require a call to a helper function to determine
    //:    value.  This means that we don't have to aggressively test this
    //:    function, but should perform more than a cursory forwarding test.
    //:    (This will result in more than a simple forwarding test.)
    //:
    //:  6 Equality is a "Commutative" operation.
    //:       'A == B' implies 'B == A'
    //:    (Commutative property.)
    //:
    //:  7 Equality is a "Commutative" operation in the negative.
    //:       'A != B' implies 'B != A'
    //:    (Commutative property.)
    //:
    //:  8 Any two things equal to a third are equal to each other:
    //:       'A == B' and 'B == C' implies 'A == C'.
    //:    (Transitive property.)
    //:
    //:  9 Any two things equal to each other are both inequal to an inequal
    //:    third.
    //:       'A == B' and 'B != C' implies 'A != C'
    //:    (Transitive property.)
    //:
    //: 10 'NaN' and 'Inf' states are excluded from these rules and instead
    //:    have the following properties:
    //:
    //:     1  'NaN' !=  'NaN'
    //:     2  value !=  'NaN'
    //:     3  'NaN' !=  value
    //:     4 +'Inf' == +'Inf'
    //:     5 -'Inf' == -'Inf'
    //:     6 +'Inf' != -'Inf'
    //:     7 -'Inf' != +'Inf'
    //:     8  value != +'Inf'
    //:     9  value != -'Inf'
    //:    10 +'Inf' !=  value
    //:    11 -'Inf' !=  value
    //:    12  'NaN' != +'Inf'
    //:    13  'NaN' != -'Inf'
    //:    14 +'Inf' !=  'NaN'
    //:    15 -'Inf' !=  'NaN'
    //:
    //: 11 Inequality tests more than identity (same 'this' pointer).
    //:
    //: 12 'notEqual' should return false when there are two NaN arguments.
    //
    // Plan:
    //:  1 Myriad values will be tested for equality and inequality with
    //:    slightly altered values.  ('A != A + 1', and 'A + 1 != A', for
    //:    example)
    //:
    //:  2 All equality, and inequality tests will be tested with both
    //:    possible orderings to check the Commutative property.
    //:
    //:  3 Values will be tested for equality with copies of themselves
    //:
    //:  4 Create 'NaN' values, and compare them as inequal.
    //
    // Testing:
    //   notEqual(ValueType32,  ValueType32)
    //   notEqual(ValueType64,  ValueType64)
    //   notEqual(ValueType128, ValueType128)
    //   TEST 'notEqual' FOR 'NaN' CORRECTNESS
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "INEQUALITY COMPARISON 'Util::notEqual'" << endl
                      << "======================================" << endl;

    // Basic value comparison testing

    {

        Util::ValueType32  test32;
        Util::ValueType64  test64;
        Util::ValueType128 test128;

        Util::ValueType32  same32;
        Util::ValueType64  same64;
        Util::ValueType128 same128;

        Util::ValueType32  negated32;
        Util::ValueType64  negated64;
        Util::ValueType128 negated128;

        Util::ValueType32  incremented32;
        Util::ValueType64  incremented64;
        Util::ValueType128 incremented128;

        Util::ValueType32  exponentChanged32;
        Util::ValueType64  exponentChanged64;
        Util::ValueType128 exponentChanged128;

                //    0e0

                // (All 0's are special cases which only differ by mantissa
                // value not by exponent or sign.)

        test32 = Util::makeDecimalRaw32(0, 0);
        same32 = Util::makeDecimalRaw32(0, 0);
        incremented32 = Util::makeDecimalRaw32(1, 0);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));

        test64 = Util::makeDecimalRaw64(0, 0);
        same64 = Util::makeDecimalRaw64(0, 0);
        incremented64 = Util::makeDecimalRaw64(1, 0);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));

        test128 = Util::makeDecimalRaw128(0, 0);
        same128 = Util::makeDecimalRaw128(0, 0);
        incremented128 = Util::makeDecimalRaw128(1, 0);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));

                //    7e0

        test32 = Util::makeDecimalRaw32(7, 0);
        same32 = Util::makeDecimalRaw32(7, 0);
        negated32 = Util::makeDecimalRaw32(-7, 0);
        incremented32 = Util::makeDecimalRaw32(8, 0);
        exponentChanged32 = Util::makeDecimalRaw32(7, 2);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(negated32, test32));
        ASSERT( Util::notEqual(test32, negated32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));
        ASSERT( Util::notEqual(exponentChanged32, test32));
        ASSERT( Util::notEqual(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(7, 0);
        same64 = Util::makeDecimalRaw64(7, 0);
        negated64 = Util::makeDecimalRaw64(-7, 0);
        incremented64 = Util::makeDecimalRaw64(8, 0);
        exponentChanged64 = Util::makeDecimalRaw64(7, 2);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(negated64, test64));
        ASSERT( Util::notEqual(test64, negated64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));
        ASSERT( Util::notEqual(exponentChanged64, test64));
        ASSERT( Util::notEqual(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(7, 0);
        same128 = Util::makeDecimalRaw128(7, 0);
        negated128 = Util::makeDecimalRaw128(-7, 0);
        incremented128 = Util::makeDecimalRaw128(8, 0);
        exponentChanged128 = Util::makeDecimalRaw128(7, 2);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(negated128, test128));
        ASSERT( Util::notEqual(test128, negated128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));
        ASSERT( Util::notEqual(exponentChanged128, test128));
        ASSERT( Util::notEqual(test128, exponentChanged128));

                //  100e0

        test32 = Util::makeDecimalRaw32(100, 0);
        same32 = Util::makeDecimalRaw32(100, 0);
        negated32 = Util::makeDecimalRaw32(-100, 0);
        incremented32 = Util::makeDecimalRaw32(101, 0);
        exponentChanged32 = Util::makeDecimalRaw32(100, 2);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(negated32, test32));
        ASSERT( Util::notEqual(test32, negated32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));
        ASSERT( Util::notEqual(exponentChanged32, test32));
        ASSERT( Util::notEqual(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(100, 0);
        same64 = Util::makeDecimalRaw64(100, 0);
        negated64 = Util::makeDecimalRaw64(-100, 0);
        incremented64 = Util::makeDecimalRaw64(101, 0);
        exponentChanged64 = Util::makeDecimalRaw64(100, 2);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(negated64, test64));
        ASSERT( Util::notEqual(test64, negated64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));
        ASSERT( Util::notEqual(exponentChanged64, test64));
        ASSERT( Util::notEqual(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(100, 0);
        same128 = Util::makeDecimalRaw128(100, 0);
        negated128 = Util::makeDecimalRaw128(-100, 0);
        incremented128 = Util::makeDecimalRaw128(101, 0);
        exponentChanged128 = Util::makeDecimalRaw128(100, 2);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(negated128, test128));
        ASSERT( Util::notEqual(test128, negated128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));
        ASSERT( Util::notEqual(exponentChanged128, test128));
        ASSERT( Util::notEqual(test128, exponentChanged128));

                //   -7e0

        test32 = Util::makeDecimalRaw32(-7, 0);
        same32 = Util::makeDecimalRaw32(-7, 0);
        negated32 = Util::makeDecimalRaw32(7, 0);
        incremented32 = Util::makeDecimalRaw32(-6, 0);
        exponentChanged32 = Util::makeDecimalRaw32(-7, 2);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(negated32, test32));
        ASSERT( Util::notEqual(test32, negated32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));
        ASSERT( Util::notEqual(exponentChanged32, test32));
        ASSERT( Util::notEqual(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(-7, 0);
        same64 = Util::makeDecimalRaw64(-7, 0);
        negated64 = Util::makeDecimalRaw64(7, 0);
        incremented64 = Util::makeDecimalRaw64(-6, 0);
        exponentChanged64 = Util::makeDecimalRaw64(-7, 2);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(negated64, test64));
        ASSERT( Util::notEqual(test64, negated64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));
        ASSERT( Util::notEqual(exponentChanged64, test64));
        ASSERT( Util::notEqual(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(-7, 0);
        same128 = Util::makeDecimalRaw128(-7, 0);
        negated128 = Util::makeDecimalRaw128(7, 0);
        incremented128 = Util::makeDecimalRaw128(-6, 0);
        exponentChanged128 = Util::makeDecimalRaw128(-7, 2);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(negated128, test128));
        ASSERT( Util::notEqual(test128, negated128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));
        ASSERT( Util::notEqual(exponentChanged128, test128));
        ASSERT( Util::notEqual(test128, exponentChanged128));

                // -100e0

        test32 = Util::makeDecimalRaw32(-100, 0);
        same32 = Util::makeDecimalRaw32(-100, 0);
        negated32 = Util::makeDecimalRaw32(100, 0);
        incremented32 = Util::makeDecimalRaw32(-99, 0);
        exponentChanged32 = Util::makeDecimalRaw32(-100, 2);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(negated32, test32));
        ASSERT( Util::notEqual(test32, negated32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));
        ASSERT( Util::notEqual(exponentChanged32, test32));
        ASSERT( Util::notEqual(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(-100, 0);
        same64 = Util::makeDecimalRaw64(-100, 0);
        negated64 = Util::makeDecimalRaw64(100, 0);
        incremented64 = Util::makeDecimalRaw64(-99, 0);
        exponentChanged64 = Util::makeDecimalRaw64(-100, 2);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(negated64, test64));
        ASSERT( Util::notEqual(test64, negated64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));
        ASSERT( Util::notEqual(exponentChanged64, test64));
        ASSERT( Util::notEqual(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(-100, 0);
        same128 = Util::makeDecimalRaw128(-100, 0);
        negated128 = Util::makeDecimalRaw128(100, 0);
        incremented128 = Util::makeDecimalRaw128(-99, 0);
        exponentChanged128 = Util::makeDecimalRaw128(-100, 2);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(negated128, test128));
        ASSERT( Util::notEqual(test128, negated128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));
        ASSERT( Util::notEqual(exponentChanged128, test128));
        ASSERT( Util::notEqual(test128, exponentChanged128));

                //    0e3

        test32 = Util::makeDecimalRaw32(0, 3);
        same32 = Util::makeDecimalRaw32(0, 3);
        incremented32 = Util::makeDecimalRaw32(1, 3);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));

        test64 = Util::makeDecimalRaw64(0, 3);
        same64 = Util::makeDecimalRaw64(0, 3);
        incremented64 = Util::makeDecimalRaw64(1, 3);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));

        test128 = Util::makeDecimalRaw128(0, 3);
        same128 = Util::makeDecimalRaw128(0, 3);
        incremented128 = Util::makeDecimalRaw128(1, 3);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));

                //    7e3

        test32 = Util::makeDecimalRaw32(7, 3);
        same32 = Util::makeDecimalRaw32(7, 3);
        negated32 = Util::makeDecimalRaw32(-7, 3);
        incremented32 = Util::makeDecimalRaw32(8, 3);
        exponentChanged32 = Util::makeDecimalRaw32(7, 5);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(negated32, test32));
        ASSERT( Util::notEqual(test32, negated32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));
        ASSERT( Util::notEqual(exponentChanged32, test32));
        ASSERT( Util::notEqual(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(7, 3);
        same64 = Util::makeDecimalRaw64(7, 3);
        negated64 = Util::makeDecimalRaw64(-7, 3);
        incremented64 = Util::makeDecimalRaw64(8, 3);
        exponentChanged64 = Util::makeDecimalRaw64(7, 5);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(negated64, test64));
        ASSERT( Util::notEqual(test64, negated64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));
        ASSERT( Util::notEqual(exponentChanged64, test64));
        ASSERT( Util::notEqual(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(7, 3);
        same128 = Util::makeDecimalRaw128(7, 3);
        negated128 = Util::makeDecimalRaw128(-7, 3);
        incremented128 = Util::makeDecimalRaw128(8, 3);
        exponentChanged128 = Util::makeDecimalRaw128(7, 5);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(negated128, test128));
        ASSERT( Util::notEqual(test128, negated128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));
        ASSERT( Util::notEqual(exponentChanged128, test128));
        ASSERT( Util::notEqual(test128, exponentChanged128));

                //  100e3

        test32 = Util::makeDecimalRaw32(100, 3);
        same32 = Util::makeDecimalRaw32(100, 3);
        negated32 = Util::makeDecimalRaw32(-100, 3);
        incremented32 = Util::makeDecimalRaw32(101, 3);
        exponentChanged32 = Util::makeDecimalRaw32(100, 5);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(negated32, test32));
        ASSERT( Util::notEqual(test32, negated32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));
        ASSERT( Util::notEqual(exponentChanged32, test32));
        ASSERT( Util::notEqual(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(100, 3);
        same64 = Util::makeDecimalRaw64(100, 3);
        negated64 = Util::makeDecimalRaw64(-100, 3);
        incremented64 = Util::makeDecimalRaw64(101, 3);
        exponentChanged64 = Util::makeDecimalRaw64(100, 5);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(negated64, test64));
        ASSERT( Util::notEqual(test64, negated64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));
        ASSERT( Util::notEqual(exponentChanged64, test64));
        ASSERT( Util::notEqual(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(100, 3);
        same128 = Util::makeDecimalRaw128(100, 3);
        negated128 = Util::makeDecimalRaw128(-100, 3);
        incremented128 = Util::makeDecimalRaw128(101, 3);
        exponentChanged128 = Util::makeDecimalRaw128(100, 5);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(negated128, test128));
        ASSERT( Util::notEqual(test128, negated128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));
        ASSERT( Util::notEqual(exponentChanged128, test128));
        ASSERT( Util::notEqual(test128, exponentChanged128));

                //   -7e3

        test32 = Util::makeDecimalRaw32(-7, 3);
        same32 = Util::makeDecimalRaw32(-7, 3);
        negated32 = Util::makeDecimalRaw32(7, 3);
        incremented32 = Util::makeDecimalRaw32(-6, 3);
        exponentChanged32 = Util::makeDecimalRaw32(-7, 5);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(negated32, test32));
        ASSERT( Util::notEqual(test32, negated32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));
        ASSERT( Util::notEqual(exponentChanged32, test32));
        ASSERT( Util::notEqual(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(-7, 3);
        same64 = Util::makeDecimalRaw64(-7, 3);
        negated64 = Util::makeDecimalRaw64(7, 3);
        incremented64 = Util::makeDecimalRaw64(-6, 3);
        exponentChanged64 = Util::makeDecimalRaw64(-7, 5);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(negated64, test64));
        ASSERT( Util::notEqual(test64, negated64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));
        ASSERT( Util::notEqual(exponentChanged64, test64));
        ASSERT( Util::notEqual(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(-7, 3);
        same128 = Util::makeDecimalRaw128(-7, 3);
        negated128 = Util::makeDecimalRaw128(7, 3);
        incremented128 = Util::makeDecimalRaw128(-6, 3);
        exponentChanged128 = Util::makeDecimalRaw128(-7, 5);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(negated128, test128));
        ASSERT( Util::notEqual(test128, negated128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));
        ASSERT( Util::notEqual(exponentChanged128, test128));
        ASSERT( Util::notEqual(test128, exponentChanged128));

                // -100e3

        test32 = Util::makeDecimalRaw32(-100, 3);
        same32 = Util::makeDecimalRaw32(-100, 3);
        negated32 = Util::makeDecimalRaw32(100, 3);
        incremented32 = Util::makeDecimalRaw32(-99, 3);
        exponentChanged32 = Util::makeDecimalRaw32(-100, 5);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(negated32, test32));
        ASSERT( Util::notEqual(test32, negated32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));
        ASSERT( Util::notEqual(exponentChanged32, test32));
        ASSERT( Util::notEqual(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(-100, 3);
        same64 = Util::makeDecimalRaw64(-100, 3);
        negated64 = Util::makeDecimalRaw64(100, 3);
        incremented64 = Util::makeDecimalRaw64(-99, 3);
        exponentChanged64 = Util::makeDecimalRaw64(-100, 5);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(negated64, test64));
        ASSERT( Util::notEqual(test64, negated64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));
        ASSERT( Util::notEqual(exponentChanged64, test64));
        ASSERT( Util::notEqual(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(-100, 3);
        same128 = Util::makeDecimalRaw128(-100, 3);
        negated128 = Util::makeDecimalRaw128(100, 3);
        incremented128 = Util::makeDecimalRaw128(-99, 3);
        exponentChanged128 = Util::makeDecimalRaw128(-100, 5);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(negated128, test128));
        ASSERT( Util::notEqual(test128, negated128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));
        ASSERT( Util::notEqual(exponentChanged128, test128));
        ASSERT( Util::notEqual(test128, exponentChanged128));

                //    0e-5

        test32 = Util::makeDecimalRaw32(0, -5);
        same32 = Util::makeDecimalRaw32(0, -5);
        incremented32 = Util::makeDecimalRaw32(1, -5);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));

        test64 = Util::makeDecimalRaw64(0, -5);
        same64 = Util::makeDecimalRaw64(0, -5);
        incremented64 = Util::makeDecimalRaw64(1, -5);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));

        test128 = Util::makeDecimalRaw128(0, -5);
        same128 = Util::makeDecimalRaw128(0, -5);
        incremented128 = Util::makeDecimalRaw128(1, -5);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));

                //    7e-5

        test32 = Util::makeDecimalRaw32(7, -5);
        same32 = Util::makeDecimalRaw32(7, -5);
        negated32 = Util::makeDecimalRaw32(-7, -5);
        incremented32 = Util::makeDecimalRaw32(8, -5);
        exponentChanged32 = Util::makeDecimalRaw32(7, -3);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(negated32, test32));
        ASSERT( Util::notEqual(test32, negated32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));
        ASSERT( Util::notEqual(exponentChanged32, test32));
        ASSERT( Util::notEqual(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(7, -5);
        same64 = Util::makeDecimalRaw64(7, -5);
        negated64 = Util::makeDecimalRaw64(-7, -5);
        incremented64 = Util::makeDecimalRaw64(8, -5);
        exponentChanged64 = Util::makeDecimalRaw64(7, -3);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(negated64, test64));
        ASSERT( Util::notEqual(test64, negated64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));
        ASSERT( Util::notEqual(exponentChanged64, test64));
        ASSERT( Util::notEqual(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(7, -5);
        same128 = Util::makeDecimalRaw128(7, -5);
        negated128 = Util::makeDecimalRaw128(-7, -5);
        incremented128 = Util::makeDecimalRaw128(8, -5);
        exponentChanged128 = Util::makeDecimalRaw128(7, -3);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(negated128, test128));
        ASSERT( Util::notEqual(test128, negated128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));
        ASSERT( Util::notEqual(exponentChanged128, test128));
        ASSERT( Util::notEqual(test128, exponentChanged128));

                //  100e-5

        test32 = Util::makeDecimalRaw32(100, -5);
        same32 = Util::makeDecimalRaw32(100, -5);
        negated32 = Util::makeDecimalRaw32(-100, -5);
        incremented32 = Util::makeDecimalRaw32(101, -5);
        exponentChanged32 = Util::makeDecimalRaw32(100, -3);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(negated32, test32));
        ASSERT( Util::notEqual(test32, negated32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));
        ASSERT( Util::notEqual(exponentChanged32, test32));
        ASSERT( Util::notEqual(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(100, -5);
        same64 = Util::makeDecimalRaw64(100, -5);
        negated64 = Util::makeDecimalRaw64(-100, -5);
        incremented64 = Util::makeDecimalRaw64(101, -5);
        exponentChanged64 = Util::makeDecimalRaw64(100, -3);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(negated64, test64));
        ASSERT( Util::notEqual(test64, negated64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));
        ASSERT( Util::notEqual(exponentChanged64, test64));
        ASSERT( Util::notEqual(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(100, -5);
        same128 = Util::makeDecimalRaw128(100, -5);
        negated128 = Util::makeDecimalRaw128(-100, -5);
        incremented128 = Util::makeDecimalRaw128(101, -5);
        exponentChanged128 = Util::makeDecimalRaw128(100, -3);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(negated128, test128));
        ASSERT( Util::notEqual(test128, negated128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));
        ASSERT( Util::notEqual(exponentChanged128, test128));
        ASSERT( Util::notEqual(test128, exponentChanged128));

                //   -7e-5

        test32 = Util::makeDecimalRaw32(-7, -5);
        same32 = Util::makeDecimalRaw32(-7, -5);
        negated32 = Util::makeDecimalRaw32(7, -5);
        incremented32 = Util::makeDecimalRaw32(-6, -5);
        exponentChanged32 = Util::makeDecimalRaw32(-7, -3);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(negated32, test32));
        ASSERT( Util::notEqual(test32, negated32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));
        ASSERT( Util::notEqual(exponentChanged32, test32));
        ASSERT( Util::notEqual(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(-7, -5);
        same64 = Util::makeDecimalRaw64(-7, -5);
        negated64 = Util::makeDecimalRaw64(7, -5);
        incremented64 = Util::makeDecimalRaw64(-6, -5);
        exponentChanged64 = Util::makeDecimalRaw64(-7, -3);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(negated64, test64));
        ASSERT( Util::notEqual(test64, negated64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));
        ASSERT( Util::notEqual(exponentChanged64, test64));
        ASSERT( Util::notEqual(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(-7, -5);
        same128 = Util::makeDecimalRaw128(-7, -5);
        negated128 = Util::makeDecimalRaw128(7, -5);
        incremented128 = Util::makeDecimalRaw128(-6, -5);
        exponentChanged128 = Util::makeDecimalRaw128(-7, -3);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(negated128, test128));
        ASSERT( Util::notEqual(test128, negated128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));
        ASSERT( Util::notEqual(exponentChanged128, test128));
        ASSERT( Util::notEqual(test128, exponentChanged128));

                // -100e-5

        test32 = Util::makeDecimalRaw32(-100, -5);
        same32 = Util::makeDecimalRaw32(-100, -5);
        negated32 = Util::makeDecimalRaw32(100, -5);
        incremented32 = Util::makeDecimalRaw32(-99, -5);
        exponentChanged32 = Util::makeDecimalRaw32(-100, -3);
        ASSERT(!Util::notEqual(test32, test32));
        ASSERT(!Util::notEqual(same32, test32));
        ASSERT(!Util::notEqual(test32, same32));
        ASSERT( Util::notEqual(negated32, test32));
        ASSERT( Util::notEqual(test32, negated32));
        ASSERT( Util::notEqual(incremented32, test32));
        ASSERT( Util::notEqual(test32, incremented32));
        ASSERT( Util::notEqual(exponentChanged32, test32));
        ASSERT( Util::notEqual(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(-100, -5);
        same64 = Util::makeDecimalRaw64(-100, -5);
        negated64 = Util::makeDecimalRaw64(100, -5);
        incremented64 = Util::makeDecimalRaw64(-99, -5);
        exponentChanged64 = Util::makeDecimalRaw64(-100, -3);
        ASSERT(!Util::notEqual(test64, test64));
        ASSERT(!Util::notEqual(same64, test64));
        ASSERT(!Util::notEqual(test64, same64));
        ASSERT( Util::notEqual(negated64, test64));
        ASSERT( Util::notEqual(test64, negated64));
        ASSERT( Util::notEqual(incremented64, test64));
        ASSERT( Util::notEqual(test64, incremented64));
        ASSERT( Util::notEqual(exponentChanged64, test64));
        ASSERT( Util::notEqual(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(-100, -5);
        same128 = Util::makeDecimalRaw128(-100, -5);
        negated128 = Util::makeDecimalRaw128(100, -5);
        incremented128 = Util::makeDecimalRaw128(-99, -5);
        exponentChanged128 = Util::makeDecimalRaw128(-100, -3);
        ASSERT(!Util::notEqual(test128, test128));
        ASSERT(!Util::notEqual(same128, test128));
        ASSERT(!Util::notEqual(test128, same128));
        ASSERT( Util::notEqual(negated128, test128));
        ASSERT( Util::notEqual(test128, negated128));
        ASSERT( Util::notEqual(incremented128, test128));
        ASSERT( Util::notEqual(test128, incremented128));
        ASSERT( Util::notEqual(exponentChanged128, test128));
        ASSERT( Util::notEqual(test128, exponentChanged128));

    }

    // Non bitwise notEqual testing:

    {
        Util::ValueType32 test32;
        Util::ValueType32 alt32;

        Util::ValueType64 test64;
        Util::ValueType64 alt64;

        Util::ValueType128 test128;
        Util::ValueType128 alt128;

                    // 0

        test32 = Util::makeDecimalRaw32(0, 0);
        alt32  = Util::makeDecimalRaw32(0, 5);

        ASSERT(!Util::notEqual(test32,  alt32));
        ASSERT(!Util::notEqual( alt32, test32));

        test64 = Util::makeDecimalRaw64(0, 0);
        alt64  = Util::makeDecimalRaw64(0, 5);

        ASSERT(!Util::notEqual(test64,  alt64));
        ASSERT(!Util::notEqual( alt64, test64));

        test128 = Util::makeDecimalRaw128(0, 0);
        alt128  = Util::makeDecimalRaw128(0, 5);

        ASSERT(!Util::notEqual(test128,  alt128));
        ASSERT(!Util::notEqual( alt128, test128));

                    // 700 vs 7e2

        test32 = Util::makeDecimalRaw32(700, 0);
        alt32  = Util::makeDecimalRaw32(7,   2);

        ASSERT(!Util::notEqual(test32,  alt32));
        ASSERT(!Util::notEqual( alt32, test32));

        test64 = Util::makeDecimalRaw64(700, 0);
        alt64  = Util::makeDecimalRaw64(7,   2);

        ASSERT(!Util::notEqual(test64,  alt64));
        ASSERT(!Util::notEqual( alt64, test64));

        test128 = Util::makeDecimalRaw128(700, 0);
        alt128  = Util::makeDecimalRaw128(7,   2);

        ASSERT(!Util::notEqual(test128,  alt128));
        ASSERT(!Util::notEqual( alt128, test128));
    }

    // Original testing for 'NaN problems.

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

    // Transitivity tests

    {
        // Positive transitivity

        Util::ValueType32 a32;
        Util::ValueType32 b32;
        Util::ValueType32 c32;

        Util::ValueType64 a64;
        Util::ValueType64 b64;
        Util::ValueType64 c64;

        Util::ValueType128 a128;
        Util::ValueType128 b128;
        Util::ValueType128 c128;

        a32 = Util::makeDecimalRaw32(   1, 3);
        b32 = Util::makeDecimalRaw32(1000, 0);
        c32 = Util::makeDecimalRaw32(  10, 2);

        ASSERT(!Util::notEqual(a32, b32));
        ASSERT(!Util::notEqual(b32, a32));
        ASSERT(!Util::notEqual(b32, c32));
        ASSERT(!Util::notEqual(c32, b32));
        ASSERT(!Util::notEqual(a32, c32));
        ASSERT(!Util::notEqual(c32, a32));

        a64 = Util::makeDecimalRaw64(   1, 3);
        b64 = Util::makeDecimalRaw64(1000, 0);
        c64 = Util::makeDecimalRaw64(  10, 2);

        ASSERT(!Util::notEqual(a64, b64));
        ASSERT(!Util::notEqual(b64, a64));
        ASSERT(!Util::notEqual(b64, c64));
        ASSERT(!Util::notEqual(c64, b64));
        ASSERT(!Util::notEqual(a64, c64));
        ASSERT(!Util::notEqual(c64, a64));

        a128 = Util::makeDecimalRaw128(   1, 3);
        b128 = Util::makeDecimalRaw128(1000, 0);
        c128 = Util::makeDecimalRaw128(  10, 2);

        ASSERT(!Util::notEqual(a128, b128));
        ASSERT(!Util::notEqual(b128, a128));
        ASSERT(!Util::notEqual(b128, c128));
        ASSERT(!Util::notEqual(c128, b128));
        ASSERT(!Util::notEqual(a128, c128));
        ASSERT(!Util::notEqual(c128, a128));

        // Negative transitivity

        a32 = Util::makeDecimalRaw32(   1, 3);
        b32 = Util::makeDecimalRaw32(1000, 0);
        c32 = Util::makeDecimalRaw32(  20, 2);

        ASSERT(!Util::notEqual(a32, b32));
        ASSERT(!Util::notEqual(b32, a32));
        ASSERT( Util::notEqual(b32, c32));
        ASSERT( Util::notEqual(c32, b32));
        ASSERT( Util::notEqual(a32, c32));
        ASSERT( Util::notEqual(c32, a32));

        a64 = Util::makeDecimalRaw64(   1, 3);
        b64 = Util::makeDecimalRaw64(1000, 0);
        c64 = Util::makeDecimalRaw64(  20, 2);

        ASSERT(!Util::notEqual(a64, b64));
        ASSERT(!Util::notEqual(b64, a64));
        ASSERT( Util::notEqual(b64, c64));
        ASSERT( Util::notEqual(c64, b64));
        ASSERT( Util::notEqual(a64, c64));
        ASSERT( Util::notEqual(c64, a64));

        a128 = Util::makeDecimalRaw128(   1, 3);
        b128 = Util::makeDecimalRaw128(1000, 0);
        c128 = Util::makeDecimalRaw128(  20, 2);

        ASSERT(!Util::notEqual(a128, b128));
        ASSERT(!Util::notEqual(b128, a128));
        ASSERT( Util::notEqual(b128, c128));
        ASSERT( Util::notEqual(c128, b128));
        ASSERT( Util::notEqual(a128, c128));
        ASSERT( Util::notEqual(c128, a128));
    }

    // 'NaN' and 'Inf' testing

    // These tests should be in the same order as Concern #10.

    {
        Util::ValueType32   nan32  = Util::parse32(  "NaN");
        Util::ValueType32  qnan32  = Util::parse32( "qNaN");
        Util::ValueType32  snan32  = Util::parse32( "sNaN");
        Util::ValueType32  pinf32  = Util::parse32( "+Inf");
        Util::ValueType32  ninf32  = Util::parse32( "-Inf");
        Util::ValueType32  test32  = Util::makeDecimalRaw32(42, 1);

        Util::ValueType64   nan64  = Util::parse64(  "NaN");
        Util::ValueType64  qnan64  = Util::parse64( "qNaN");
        Util::ValueType64  snan64  = Util::parse64( "sNaN");
        Util::ValueType64  pinf64  = Util::parse64( "+Inf");
        Util::ValueType64  ninf64  = Util::parse64( "-Inf");
        Util::ValueType64  test64  = Util::makeDecimalRaw64(42, 1);

        Util::ValueType128  nan128 = Util::parse128( "NaN");
        Util::ValueType128 qnan128 = Util::parse128("qNaN");
        Util::ValueType128 snan128 = Util::parse128("sNaN");
        Util::ValueType128 pinf128 = Util::parse128("+Inf");
        Util::ValueType128 ninf128 = Util::parse128("-Inf");
        Util::ValueType128 test128 = Util::makeDecimalRaw128(42, 1);

        // 'NaN' is not notEqual to itself, in any form.

        ASSERT( Util::notEqual( nan32,   nan32));
        ASSERT( Util::notEqual(qnan32,  qnan32));
        ASSERT( Util::notEqual(snan32,  snan32));

        ASSERT( Util::notEqual( nan64,   nan64));
        ASSERT( Util::notEqual(qnan64,  qnan64));
        ASSERT( Util::notEqual(snan64,  snan64));

        ASSERT( Util::notEqual( nan128,  nan128));
        ASSERT( Util::notEqual(qnan128, qnan128));
        ASSERT( Util::notEqual(snan128, snan128));

        // Cross 'NaN' comparisons should all be inequal.

        ASSERT( Util::notEqual( nan32,  qnan32));
        ASSERT( Util::notEqual(qnan32,   nan32));
        ASSERT( Util::notEqual( nan32,  snan32));
        ASSERT( Util::notEqual(snan32,   nan32));
        ASSERT( Util::notEqual(qnan32,  snan32));
        ASSERT( Util::notEqual(snan32,  qnan32));

        ASSERT( Util::notEqual( nan64,  qnan64));
        ASSERT( Util::notEqual(qnan64,   nan64));
        ASSERT( Util::notEqual( nan64,  snan64));
        ASSERT( Util::notEqual(snan64,   nan64));
        ASSERT( Util::notEqual(qnan64,  snan64));
        ASSERT( Util::notEqual(snan64,  qnan64));

        ASSERT( Util::notEqual( nan128, qnan128));
        ASSERT( Util::notEqual(qnan128,  nan128));
        ASSERT( Util::notEqual( nan128, snan128));
        ASSERT( Util::notEqual(snan128,  nan128));
        ASSERT( Util::notEqual(qnan128, snan128));
        ASSERT( Util::notEqual(snan128, qnan128));

        // 'NaN' to value comparisons should be inequal too.

        ASSERT( Util::notEqual( nan32,  test32));
        ASSERT( Util::notEqual(test32,   nan32));
        ASSERT( Util::notEqual(qnan32,  test32));
        ASSERT( Util::notEqual(test32,  qnan32));
        ASSERT( Util::notEqual(snan32,  test32));
        ASSERT( Util::notEqual(test32,  snan32));

        ASSERT( Util::notEqual( nan64,  test64));
        ASSERT( Util::notEqual(test64,   nan64));
        ASSERT( Util::notEqual(qnan64,  test64));
        ASSERT( Util::notEqual(test64,  qnan64));
        ASSERT( Util::notEqual(snan64,  test64));
        ASSERT( Util::notEqual(test64,  snan64));

        ASSERT( Util::notEqual( nan128, test128));
        ASSERT( Util::notEqual(test128,  nan128));
        ASSERT( Util::notEqual(qnan128, test128));
        ASSERT( Util::notEqual(test128, qnan128));
        ASSERT( Util::notEqual(snan128, test128));
        ASSERT( Util::notEqual(test128, snan128));

        // 'Inf's should compare notEqual only when they have different
        // signs

        ASSERT(!Util::notEqual(pinf32,  pinf32));
        ASSERT(!Util::notEqual(ninf32,  ninf32));
        ASSERT( Util::notEqual(pinf32,  ninf32));
        ASSERT( Util::notEqual(ninf32,  pinf32));

        ASSERT(!Util::notEqual(pinf64,  pinf64));
        ASSERT(!Util::notEqual(ninf64,  ninf64));
        ASSERT( Util::notEqual(pinf64,  ninf64));
        ASSERT( Util::notEqual(ninf64,  pinf64));

        ASSERT(!Util::notEqual(pinf128, pinf128));
        ASSERT(!Util::notEqual(ninf128, ninf128));
        ASSERT( Util::notEqual(pinf128, ninf128));
        ASSERT( Util::notEqual(ninf128, pinf128));

        // Value to 'Inf' comparisons should be inequal.

        ASSERT( Util::notEqual(test32,  pinf32));
        ASSERT( Util::notEqual(test32,  ninf32));
        ASSERT( Util::notEqual(pinf32,  test32));
        ASSERT( Util::notEqual(ninf32,  test32));

        ASSERT( Util::notEqual(test64,  pinf64));
        ASSERT( Util::notEqual(test64,  ninf64));
        ASSERT( Util::notEqual(pinf64,  test64));
        ASSERT( Util::notEqual(ninf64,  test64));

        ASSERT( Util::notEqual(test128, pinf128));
        ASSERT( Util::notEqual(test128, ninf128));
        ASSERT( Util::notEqual(pinf128, test128));
        ASSERT( Util::notEqual(ninf128, test128));

        // 'Inf' to 'NaN' comparisons should be inequal.

        ASSERT( Util::notEqual( nan32,  pinf32));
        ASSERT( Util::notEqual( nan32,  ninf32));
        ASSERT( Util::notEqual(qnan32,  pinf32));
        ASSERT( Util::notEqual(qnan32,  ninf32));
        ASSERT( Util::notEqual(snan32,  pinf32));
        ASSERT( Util::notEqual(snan32,  ninf32));
        ASSERT( Util::notEqual(pinf32,   nan32));
        ASSERT( Util::notEqual(ninf32,   nan32));
        ASSERT( Util::notEqual(pinf32,  qnan32));
        ASSERT( Util::notEqual(ninf32,  qnan32));
        ASSERT( Util::notEqual(pinf32,  snan32));
        ASSERT( Util::notEqual(ninf32,  snan32));

        ASSERT( Util::notEqual( nan64,  pinf64));
        ASSERT( Util::notEqual( nan64,  ninf64));
        ASSERT( Util::notEqual(qnan64,  pinf64));
        ASSERT( Util::notEqual(qnan64,  ninf64));
        ASSERT( Util::notEqual(snan64,  pinf64));
        ASSERT( Util::notEqual(snan64,  ninf64));
        ASSERT( Util::notEqual(pinf64,   nan64));
        ASSERT( Util::notEqual(ninf64,   nan64));
        ASSERT( Util::notEqual(pinf64,  qnan64));
        ASSERT( Util::notEqual(ninf64,  qnan64));
        ASSERT( Util::notEqual(pinf64,  snan64));
        ASSERT( Util::notEqual(ninf64,  snan64));

        ASSERT( Util::notEqual( nan128, pinf128));
        ASSERT( Util::notEqual( nan128, ninf128));
        ASSERT( Util::notEqual(qnan128, pinf128));
        ASSERT( Util::notEqual(qnan128, ninf128));
        ASSERT( Util::notEqual(snan128, pinf128));
        ASSERT( Util::notEqual(snan128, ninf128));
        ASSERT( Util::notEqual(pinf128,  nan128));
        ASSERT( Util::notEqual(ninf128,  nan128));
        ASSERT( Util::notEqual(pinf128, qnan128));
        ASSERT( Util::notEqual(ninf128, qnan128));
        ASSERT( Util::notEqual(pinf128, snan128));
        ASSERT( Util::notEqual(ninf128, snan128));
    }
}

void TestDriver::testCase3()
{
    // ------------------------------------------------------------------------
    // EQUALITY COMPARISON 'Util::equal'
    //
    // Concerns:
    //:  1 'equal' should return true if the bit patterns are the same (and are
    //:    not a bit pattern representing a 'NaN').
    //:
    //:  2 'equal' should return true if the bit patterns are different, but
    //:    represent the same value.  I.e. '10e0' should be equal to '1e1'.
    //:
    //:  3 Values which differ by only one of sign, exponent, or mantissa
    //:    should not be equal.
    //:
    //:  4 'NaN' values should never be equal.
    //:
    //:  5 The implementation of 'equal' normally would be a forwarding
    //:    function (to the underlying implementation), but some
    //:    implementations require a call to a helper function to determine
    //:    value.  This means that we don't have to aggressively test this
    //:    function, but should perform more than a cursory forwarding test.
    //:    (This will result in more than a simple forwarding test.)
    //:
    //:  6 Equality is a "Commutative" operation:
    //:       'A == B' implies 'B == A'
    //:    (Commutative property.)
    //:
    //:  7 Equality is a "Commutative" operation in the negative:
    //:       'A != B' implies 'B != A'
    //:    (Commutative property.)
    //:
    //:  8 Any two things equal to a third are equal to each other:
    //:       'A == B' and 'B == C' implies 'A == C'.
    //:    (Transitive property.)
    //:
    //:  9 Any two things equal to each other are both inequal to an inequal
    //:    third:
    //:       'A == B' and 'B != C' implies 'A != C'
    //:    (Transitive property.)
    //:
    //: 10 'NaN' and 'Inf' states are excluded from these rules and instead
    //:    have the following properties:
    //:
    //:     1  'NaN' !=  'NaN'
    //:     2  value !=  'NaN'
    //:     3  'NaN' !=  value
    //:     4 +'Inf' == +'Inf'
    //:     5 -'Inf' == -'Inf'
    //:     6 +'Inf' != -'Inf'
    //:     7 -'Inf' != +'Inf'
    //:     8  value != +'Inf'
    //:     9  value != -'Inf'
    //:    10 +'Inf' !=  value
    //:    11 -'Inf' !=  value
    //:    12  'NaN' != +'Inf'
    //:    13  'NaN' != -'Inf'
    //:    14 +'Inf' !=  'NaN'
    //:    15 -'Inf' !=  'NaN'
    //:
    //: 11 Equality tests more than identity (same 'this' pointer).
    //
    // Plan:
    //:  1 Myriad values will be tested for equality and inequality with
    //:    slightly altered values.  ('A != A + 1', and 'A + 1 != A', for
    //:    example)
    //:
    //:  2 All equality, and inequality tests will be tested with both
    //:    possible orderings to check the Commutative property.
    //:
    //:  3 Values will be tested for equality with copies of themselves
    //
    // Testing:
    //   equal(ValueType32,  ValueType32)
    //   equal(ValueType64,  ValueType64)
    //   equal(ValueType128, ValueType128)
    // --------------------------------------------------------------------


    if (verbose) cout << endl
                      << "EQUALITY COMPARISON 'Util::equal'" << endl
                      << "=================================" << endl;

    // Basic value comparison testing

    {

        Util::ValueType32  test32;
        Util::ValueType64  test64;
        Util::ValueType128 test128;

        Util::ValueType32  same32;
        Util::ValueType64  same64;
        Util::ValueType128 same128;

        Util::ValueType32  negated32;
        Util::ValueType64  negated64;
        Util::ValueType128 negated128;

        Util::ValueType32  incremented32;
        Util::ValueType64  incremented64;
        Util::ValueType128 incremented128;

        Util::ValueType32  exponentChanged32;
        Util::ValueType64  exponentChanged64;
        Util::ValueType128 exponentChanged128;

                //    0e0

                // (All 0's are special cases which only differ by mantissa
                // value not by exponent or sign.)

        test32 = Util::makeDecimalRaw32(0, 0);
        same32 = Util::makeDecimalRaw32(0, 0);
        incremented32 = Util::makeDecimalRaw32(1, 0);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));

        test64 = Util::makeDecimalRaw64(0, 0);
        same64 = Util::makeDecimalRaw64(0, 0);
        incremented64 = Util::makeDecimalRaw64(1, 0);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));

        test128 = Util::makeDecimalRaw128(0, 0);
        same128 = Util::makeDecimalRaw128(0, 0);
        incremented128 = Util::makeDecimalRaw128(1, 0);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));

                //    7e0

        test32 = Util::makeDecimalRaw32(7, 0);
        same32 = Util::makeDecimalRaw32(7, 0);
        negated32 = Util::makeDecimalRaw32(-7, 0);
        incremented32 = Util::makeDecimalRaw32(8, 0);
        exponentChanged32 = Util::makeDecimalRaw32(7, 2);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(negated32, test32));
        ASSERT(!Util::equal(test32, negated32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));
        ASSERT(!Util::equal(exponentChanged32, test32));
        ASSERT(!Util::equal(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(7, 0);
        same64 = Util::makeDecimalRaw64(7, 0);
        negated64 = Util::makeDecimalRaw64(-7, 0);
        incremented64 = Util::makeDecimalRaw64(8, 0);
        exponentChanged64 = Util::makeDecimalRaw64(7, 2);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(negated64, test64));
        ASSERT(!Util::equal(test64, negated64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));
        ASSERT(!Util::equal(exponentChanged64, test64));
        ASSERT(!Util::equal(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(7, 0);
        same128 = Util::makeDecimalRaw128(7, 0);
        negated128 = Util::makeDecimalRaw128(-7, 0);
        incremented128 = Util::makeDecimalRaw128(8, 0);
        exponentChanged128 = Util::makeDecimalRaw128(7, 2);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(negated128, test128));
        ASSERT(!Util::equal(test128, negated128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));
        ASSERT(!Util::equal(exponentChanged128, test128));
        ASSERT(!Util::equal(test128, exponentChanged128));

                //  100e0

        test32 = Util::makeDecimalRaw32(100, 0);
        same32 = Util::makeDecimalRaw32(100, 0);
        negated32 = Util::makeDecimalRaw32(-100, 0);
        incremented32 = Util::makeDecimalRaw32(101, 0);
        exponentChanged32 = Util::makeDecimalRaw32(100, 2);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(negated32, test32));
        ASSERT(!Util::equal(test32, negated32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));
        ASSERT(!Util::equal(exponentChanged32, test32));
        ASSERT(!Util::equal(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(100, 0);
        same64 = Util::makeDecimalRaw64(100, 0);
        negated64 = Util::makeDecimalRaw64(-100, 0);
        incremented64 = Util::makeDecimalRaw64(101, 0);
        exponentChanged64 = Util::makeDecimalRaw64(100, 2);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(negated64, test64));
        ASSERT(!Util::equal(test64, negated64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));
        ASSERT(!Util::equal(exponentChanged64, test64));
        ASSERT(!Util::equal(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(100, 0);
        same128 = Util::makeDecimalRaw128(100, 0);
        negated128 = Util::makeDecimalRaw128(-100, 0);
        incremented128 = Util::makeDecimalRaw128(101, 0);
        exponentChanged128 = Util::makeDecimalRaw128(100, 2);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(negated128, test128));
        ASSERT(!Util::equal(test128, negated128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));
        ASSERT(!Util::equal(exponentChanged128, test128));
        ASSERT(!Util::equal(test128, exponentChanged128));

                //   -7e0

        test32 = Util::makeDecimalRaw32(-7, 0);
        same32 = Util::makeDecimalRaw32(-7, 0);
        negated32 = Util::makeDecimalRaw32(7, 0);
        incremented32 = Util::makeDecimalRaw32(-6, 0);
        exponentChanged32 = Util::makeDecimalRaw32(-7, 2);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(negated32, test32));
        ASSERT(!Util::equal(test32, negated32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));
        ASSERT(!Util::equal(exponentChanged32, test32));
        ASSERT(!Util::equal(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(-7, 0);
        same64 = Util::makeDecimalRaw64(-7, 0);
        negated64 = Util::makeDecimalRaw64(7, 0);
        incremented64 = Util::makeDecimalRaw64(-6, 0);
        exponentChanged64 = Util::makeDecimalRaw64(-7, 2);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(negated64, test64));
        ASSERT(!Util::equal(test64, negated64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));
        ASSERT(!Util::equal(exponentChanged64, test64));
        ASSERT(!Util::equal(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(-7, 0);
        same128 = Util::makeDecimalRaw128(-7, 0);
        negated128 = Util::makeDecimalRaw128(7, 0);
        incremented128 = Util::makeDecimalRaw128(-6, 0);
        exponentChanged128 = Util::makeDecimalRaw128(-7, 2);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(negated128, test128));
        ASSERT(!Util::equal(test128, negated128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));
        ASSERT(!Util::equal(exponentChanged128, test128));
        ASSERT(!Util::equal(test128, exponentChanged128));

                // -100e0

        test32 = Util::makeDecimalRaw32(-100, 0);
        same32 = Util::makeDecimalRaw32(-100, 0);
        negated32 = Util::makeDecimalRaw32(100, 0);
        incremented32 = Util::makeDecimalRaw32(-99, 0);
        exponentChanged32 = Util::makeDecimalRaw32(-100, 2);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(negated32, test32));
        ASSERT(!Util::equal(test32, negated32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));
        ASSERT(!Util::equal(exponentChanged32, test32));
        ASSERT(!Util::equal(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(-100, 0);
        same64 = Util::makeDecimalRaw64(-100, 0);
        negated64 = Util::makeDecimalRaw64(100, 0);
        incremented64 = Util::makeDecimalRaw64(-99, 0);
        exponentChanged64 = Util::makeDecimalRaw64(-100, 2);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(negated64, test64));
        ASSERT(!Util::equal(test64, negated64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));
        ASSERT(!Util::equal(exponentChanged64, test64));
        ASSERT(!Util::equal(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(-100, 0);
        same128 = Util::makeDecimalRaw128(-100, 0);
        negated128 = Util::makeDecimalRaw128(100, 0);
        incremented128 = Util::makeDecimalRaw128(-99, 0);
        exponentChanged128 = Util::makeDecimalRaw128(-100, 2);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(negated128, test128));
        ASSERT(!Util::equal(test128, negated128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));
        ASSERT(!Util::equal(exponentChanged128, test128));
        ASSERT(!Util::equal(test128, exponentChanged128));

                //    0e3

        test32 = Util::makeDecimalRaw32(0, 3);
        same32 = Util::makeDecimalRaw32(0, 3);
        incremented32 = Util::makeDecimalRaw32(1, 3);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));

        test64 = Util::makeDecimalRaw64(0, 3);
        same64 = Util::makeDecimalRaw64(0, 3);
        incremented64 = Util::makeDecimalRaw64(1, 3);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));

        test128 = Util::makeDecimalRaw128(0, 3);
        same128 = Util::makeDecimalRaw128(0, 3);
        incremented128 = Util::makeDecimalRaw128(1, 3);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));

                //    7e3

        test32 = Util::makeDecimalRaw32(7, 3);
        same32 = Util::makeDecimalRaw32(7, 3);
        negated32 = Util::makeDecimalRaw32(-7, 3);
        incremented32 = Util::makeDecimalRaw32(8, 3);
        exponentChanged32 = Util::makeDecimalRaw32(7, 5);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(negated32, test32));
        ASSERT(!Util::equal(test32, negated32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));
        ASSERT(!Util::equal(exponentChanged32, test32));
        ASSERT(!Util::equal(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(7, 3);
        same64 = Util::makeDecimalRaw64(7, 3);
        negated64 = Util::makeDecimalRaw64(-7, 3);
        incremented64 = Util::makeDecimalRaw64(8, 3);
        exponentChanged64 = Util::makeDecimalRaw64(7, 5);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(negated64, test64));
        ASSERT(!Util::equal(test64, negated64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));
        ASSERT(!Util::equal(exponentChanged64, test64));
        ASSERT(!Util::equal(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(7, 3);
        same128 = Util::makeDecimalRaw128(7, 3);
        negated128 = Util::makeDecimalRaw128(-7, 3);
        incremented128 = Util::makeDecimalRaw128(8, 3);
        exponentChanged128 = Util::makeDecimalRaw128(7, 5);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(negated128, test128));
        ASSERT(!Util::equal(test128, negated128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));
        ASSERT(!Util::equal(exponentChanged128, test128));
        ASSERT(!Util::equal(test128, exponentChanged128));

                //  100e3

        test32 = Util::makeDecimalRaw32(100, 3);
        same32 = Util::makeDecimalRaw32(100, 3);
        negated32 = Util::makeDecimalRaw32(-100, 3);
        incremented32 = Util::makeDecimalRaw32(101, 3);
        exponentChanged32 = Util::makeDecimalRaw32(100, 5);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(negated32, test32));
        ASSERT(!Util::equal(test32, negated32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));
        ASSERT(!Util::equal(exponentChanged32, test32));
        ASSERT(!Util::equal(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(100, 3);
        same64 = Util::makeDecimalRaw64(100, 3);
        negated64 = Util::makeDecimalRaw64(-100, 3);
        incremented64 = Util::makeDecimalRaw64(101, 3);
        exponentChanged64 = Util::makeDecimalRaw64(100, 5);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(negated64, test64));
        ASSERT(!Util::equal(test64, negated64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));
        ASSERT(!Util::equal(exponentChanged64, test64));
        ASSERT(!Util::equal(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(100, 3);
        same128 = Util::makeDecimalRaw128(100, 3);
        negated128 = Util::makeDecimalRaw128(-100, 3);
        incremented128 = Util::makeDecimalRaw128(101, 3);
        exponentChanged128 = Util::makeDecimalRaw128(100, 5);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(negated128, test128));
        ASSERT(!Util::equal(test128, negated128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));
        ASSERT(!Util::equal(exponentChanged128, test128));
        ASSERT(!Util::equal(test128, exponentChanged128));

                //   -7e3

        test32 = Util::makeDecimalRaw32(-7, 3);
        same32 = Util::makeDecimalRaw32(-7, 3);
        negated32 = Util::makeDecimalRaw32(7, 3);
        incremented32 = Util::makeDecimalRaw32(-6, 3);
        exponentChanged32 = Util::makeDecimalRaw32(-7, 5);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(negated32, test32));
        ASSERT(!Util::equal(test32, negated32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));
        ASSERT(!Util::equal(exponentChanged32, test32));
        ASSERT(!Util::equal(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(-7, 3);
        same64 = Util::makeDecimalRaw64(-7, 3);
        negated64 = Util::makeDecimalRaw64(7, 3);
        incremented64 = Util::makeDecimalRaw64(-6, 3);
        exponentChanged64 = Util::makeDecimalRaw64(-7, 5);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(negated64, test64));
        ASSERT(!Util::equal(test64, negated64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));
        ASSERT(!Util::equal(exponentChanged64, test64));
        ASSERT(!Util::equal(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(-7, 3);
        same128 = Util::makeDecimalRaw128(-7, 3);
        negated128 = Util::makeDecimalRaw128(7, 3);
        incremented128 = Util::makeDecimalRaw128(-6, 3);
        exponentChanged128 = Util::makeDecimalRaw128(-7, 5);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(negated128, test128));
        ASSERT(!Util::equal(test128, negated128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));
        ASSERT(!Util::equal(exponentChanged128, test128));
        ASSERT(!Util::equal(test128, exponentChanged128));

                // -100e3

        test32 = Util::makeDecimalRaw32(-100, 3);
        same32 = Util::makeDecimalRaw32(-100, 3);
        negated32 = Util::makeDecimalRaw32(100, 3);
        incremented32 = Util::makeDecimalRaw32(-99, 3);
        exponentChanged32 = Util::makeDecimalRaw32(-100, 5);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(negated32, test32));
        ASSERT(!Util::equal(test32, negated32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));
        ASSERT(!Util::equal(exponentChanged32, test32));
        ASSERT(!Util::equal(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(-100, 3);
        same64 = Util::makeDecimalRaw64(-100, 3);
        negated64 = Util::makeDecimalRaw64(100, 3);
        incremented64 = Util::makeDecimalRaw64(-99, 3);
        exponentChanged64 = Util::makeDecimalRaw64(-100, 5);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(negated64, test64));
        ASSERT(!Util::equal(test64, negated64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));
        ASSERT(!Util::equal(exponentChanged64, test64));
        ASSERT(!Util::equal(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(-100, 3);
        same128 = Util::makeDecimalRaw128(-100, 3);
        negated128 = Util::makeDecimalRaw128(100, 3);
        incremented128 = Util::makeDecimalRaw128(-99, 3);
        exponentChanged128 = Util::makeDecimalRaw128(-100, 5);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(negated128, test128));
        ASSERT(!Util::equal(test128, negated128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));
        ASSERT(!Util::equal(exponentChanged128, test128));
        ASSERT(!Util::equal(test128, exponentChanged128));

                //    0e-5

        test32 = Util::makeDecimalRaw32(0, -5);
        same32 = Util::makeDecimalRaw32(0, -5);
        incremented32 = Util::makeDecimalRaw32(1, -5);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));

        test64 = Util::makeDecimalRaw64(0, -5);
        same64 = Util::makeDecimalRaw64(0, -5);
        incremented64 = Util::makeDecimalRaw64(1, -5);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));

        test128 = Util::makeDecimalRaw128(0, -5);
        same128 = Util::makeDecimalRaw128(0, -5);
        incremented128 = Util::makeDecimalRaw128(1, -5);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));

                //    7e-5

        test32 = Util::makeDecimalRaw32(7, -5);
        same32 = Util::makeDecimalRaw32(7, -5);
        negated32 = Util::makeDecimalRaw32(-7, -5);
        incremented32 = Util::makeDecimalRaw32(8, -5);
        exponentChanged32 = Util::makeDecimalRaw32(7, -3);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(negated32, test32));
        ASSERT(!Util::equal(test32, negated32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));
        ASSERT(!Util::equal(exponentChanged32, test32));
        ASSERT(!Util::equal(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(7, -5);
        same64 = Util::makeDecimalRaw64(7, -5);
        negated64 = Util::makeDecimalRaw64(-7, -5);
        incremented64 = Util::makeDecimalRaw64(8, -5);
        exponentChanged64 = Util::makeDecimalRaw64(7, -3);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(negated64, test64));
        ASSERT(!Util::equal(test64, negated64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));
        ASSERT(!Util::equal(exponentChanged64, test64));
        ASSERT(!Util::equal(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(7, -5);
        same128 = Util::makeDecimalRaw128(7, -5);
        negated128 = Util::makeDecimalRaw128(-7, -5);
        incremented128 = Util::makeDecimalRaw128(8, -5);
        exponentChanged128 = Util::makeDecimalRaw128(7, -3);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(negated128, test128));
        ASSERT(!Util::equal(test128, negated128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));
        ASSERT(!Util::equal(exponentChanged128, test128));
        ASSERT(!Util::equal(test128, exponentChanged128));

                //  100e-5

        test32 = Util::makeDecimalRaw32(100, -5);
        same32 = Util::makeDecimalRaw32(100, -5);
        negated32 = Util::makeDecimalRaw32(-100, -5);
        incremented32 = Util::makeDecimalRaw32(101, -5);
        exponentChanged32 = Util::makeDecimalRaw32(100, -3);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(negated32, test32));
        ASSERT(!Util::equal(test32, negated32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));
        ASSERT(!Util::equal(exponentChanged32, test32));
        ASSERT(!Util::equal(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(100, -5);
        same64 = Util::makeDecimalRaw64(100, -5);
        negated64 = Util::makeDecimalRaw64(-100, -5);
        incremented64 = Util::makeDecimalRaw64(101, -5);
        exponentChanged64 = Util::makeDecimalRaw64(100, -3);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(negated64, test64));
        ASSERT(!Util::equal(test64, negated64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));
        ASSERT(!Util::equal(exponentChanged64, test64));
        ASSERT(!Util::equal(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(100, -5);
        same128 = Util::makeDecimalRaw128(100, -5);
        negated128 = Util::makeDecimalRaw128(-100, -5);
        incremented128 = Util::makeDecimalRaw128(101, -5);
        exponentChanged128 = Util::makeDecimalRaw128(100, -3);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(negated128, test128));
        ASSERT(!Util::equal(test128, negated128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));
        ASSERT(!Util::equal(exponentChanged128, test128));
        ASSERT(!Util::equal(test128, exponentChanged128));

                //   -7e-5

        test32 = Util::makeDecimalRaw32(-7, -5);
        same32 = Util::makeDecimalRaw32(-7, -5);
        negated32 = Util::makeDecimalRaw32(7, -5);
        incremented32 = Util::makeDecimalRaw32(-6, -5);
        exponentChanged32 = Util::makeDecimalRaw32(-7, -3);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(negated32, test32));
        ASSERT(!Util::equal(test32, negated32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));
        ASSERT(!Util::equal(exponentChanged32, test32));
        ASSERT(!Util::equal(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(-7, -5);
        same64 = Util::makeDecimalRaw64(-7, -5);
        negated64 = Util::makeDecimalRaw64(7, -5);
        incremented64 = Util::makeDecimalRaw64(-6, -5);
        exponentChanged64 = Util::makeDecimalRaw64(-7, -3);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(negated64, test64));
        ASSERT(!Util::equal(test64, negated64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));
        ASSERT(!Util::equal(exponentChanged64, test64));
        ASSERT(!Util::equal(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(-7, -5);
        same128 = Util::makeDecimalRaw128(-7, -5);
        negated128 = Util::makeDecimalRaw128(7, -5);
        incremented128 = Util::makeDecimalRaw128(-6, -5);
        exponentChanged128 = Util::makeDecimalRaw128(-7, -3);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(negated128, test128));
        ASSERT(!Util::equal(test128, negated128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));
        ASSERT(!Util::equal(exponentChanged128, test128));
        ASSERT(!Util::equal(test128, exponentChanged128));

                // -100e-5

        test32 = Util::makeDecimalRaw32(-100, -5);
        same32 = Util::makeDecimalRaw32(-100, -5);
        negated32 = Util::makeDecimalRaw32(100, -5);
        incremented32 = Util::makeDecimalRaw32(-99, -5);
        exponentChanged32 = Util::makeDecimalRaw32(-100, -3);
        ASSERT( Util::equal(test32, test32));
        ASSERT( Util::equal(same32, test32));
        ASSERT( Util::equal(test32, same32));
        ASSERT(!Util::equal(negated32, test32));
        ASSERT(!Util::equal(test32, negated32));
        ASSERT(!Util::equal(incremented32, test32));
        ASSERT(!Util::equal(test32, incremented32));
        ASSERT(!Util::equal(exponentChanged32, test32));
        ASSERT(!Util::equal(test32, exponentChanged32));

        test64 = Util::makeDecimalRaw64(-100, -5);
        same64 = Util::makeDecimalRaw64(-100, -5);
        negated64 = Util::makeDecimalRaw64(100, -5);
        incremented64 = Util::makeDecimalRaw64(-99, -5);
        exponentChanged64 = Util::makeDecimalRaw64(-100, -3);
        ASSERT( Util::equal(test64, test64));
        ASSERT( Util::equal(same64, test64));
        ASSERT( Util::equal(test64, same64));
        ASSERT(!Util::equal(negated64, test64));
        ASSERT(!Util::equal(test64, negated64));
        ASSERT(!Util::equal(incremented64, test64));
        ASSERT(!Util::equal(test64, incremented64));
        ASSERT(!Util::equal(exponentChanged64, test64));
        ASSERT(!Util::equal(test64, exponentChanged64));

        test128 = Util::makeDecimalRaw128(-100, -5);
        same128 = Util::makeDecimalRaw128(-100, -5);
        negated128 = Util::makeDecimalRaw128(100, -5);
        incremented128 = Util::makeDecimalRaw128(-99, -5);
        exponentChanged128 = Util::makeDecimalRaw128(-100, -3);
        ASSERT( Util::equal(test128, test128));
        ASSERT( Util::equal(same128, test128));
        ASSERT( Util::equal(test128, same128));
        ASSERT(!Util::equal(negated128, test128));
        ASSERT(!Util::equal(test128, negated128));
        ASSERT(!Util::equal(incremented128, test128));
        ASSERT(!Util::equal(test128, incremented128));
        ASSERT(!Util::equal(exponentChanged128, test128));
        ASSERT(!Util::equal(test128, exponentChanged128));

    }

    // Non bitwise equal testing:

    {
        Util::ValueType32 test32;
        Util::ValueType32 alt32;

        Util::ValueType64 test64;
        Util::ValueType64 alt64;

        Util::ValueType128 test128;
        Util::ValueType128 alt128;

                    // 0

        test32 = Util::makeDecimalRaw32(0, 0);
        alt32  = Util::makeDecimalRaw32(0, 5);

        ASSERT( Util::equal(test32,  alt32));
        ASSERT( Util::equal( alt32, test32));

        test64 = Util::makeDecimalRaw64(0, 0);
        alt64  = Util::makeDecimalRaw64(0, 5);

        ASSERT( Util::equal(test64,  alt64));
        ASSERT( Util::equal( alt64, test64));

        test128 = Util::makeDecimalRaw128(0, 0);
        alt128  = Util::makeDecimalRaw128(0, 5);

        ASSERT( Util::equal(test128,  alt128));
        ASSERT( Util::equal( alt128, test128));

                    // 700 vs 7e2

        test32 = Util::makeDecimalRaw32(700, 0);
        alt32  = Util::makeDecimalRaw32(7,   2);

        ASSERT( Util::equal(test32,  alt32));
        ASSERT( Util::equal( alt32, test32));

        test64 = Util::makeDecimalRaw64(700, 0);
        alt64  = Util::makeDecimalRaw64(7,   2);

        ASSERT( Util::equal(test64,  alt64));
        ASSERT( Util::equal( alt64, test64));

        test128 = Util::makeDecimalRaw128(700, 0);
        alt128  = Util::makeDecimalRaw128(7,   2);

        ASSERT( Util::equal(test128,  alt128));
        ASSERT( Util::equal( alt128, test128));
    }

    // Transitivity tests

    {
        // Positive transitivity

        Util::ValueType32 a32;
        Util::ValueType32 b32;
        Util::ValueType32 c32;

        Util::ValueType64 a64;
        Util::ValueType64 b64;
        Util::ValueType64 c64;

        Util::ValueType128 a128;
        Util::ValueType128 b128;
        Util::ValueType128 c128;

        a32 = Util::makeDecimalRaw32(   1, 3);
        b32 = Util::makeDecimalRaw32(1000, 0);
        c32 = Util::makeDecimalRaw32(  10, 2);

        ASSERT( Util::equal(a32, b32));
        ASSERT( Util::equal(b32, a32));
        ASSERT( Util::equal(b32, c32));
        ASSERT( Util::equal(c32, b32));
        ASSERT( Util::equal(a32, c32));
        ASSERT( Util::equal(c32, a32));

        a64 = Util::makeDecimalRaw64(   1, 3);
        b64 = Util::makeDecimalRaw64(1000, 0);
        c64 = Util::makeDecimalRaw64(  10, 2);

        ASSERT( Util::equal(a64, b64));
        ASSERT( Util::equal(b64, a64));
        ASSERT( Util::equal(b64, c64));
        ASSERT( Util::equal(c64, b64));
        ASSERT( Util::equal(a64, c64));
        ASSERT( Util::equal(c64, a64));

        a128 = Util::makeDecimalRaw128(   1, 3);
        b128 = Util::makeDecimalRaw128(1000, 0);
        c128 = Util::makeDecimalRaw128(  10, 2);

        ASSERT( Util::equal(a128, b128));
        ASSERT( Util::equal(b128, a128));
        ASSERT( Util::equal(b128, c128));
        ASSERT( Util::equal(c128, b128));
        ASSERT( Util::equal(a128, c128));
        ASSERT( Util::equal(c128, a128));

        // Negative transitivity

        a32 = Util::makeDecimalRaw32(   1, 3);
        b32 = Util::makeDecimalRaw32(1000, 0);
        c32 = Util::makeDecimalRaw32(  20, 2);

        ASSERT( Util::equal(a32, b32));
        ASSERT( Util::equal(b32, a32));
        ASSERT(!Util::equal(b32, c32));
        ASSERT(!Util::equal(c32, b32));
        ASSERT(!Util::equal(a32, c32));
        ASSERT(!Util::equal(c32, a32));

        a64 = Util::makeDecimalRaw64(   1, 3);
        b64 = Util::makeDecimalRaw64(1000, 0);
        c64 = Util::makeDecimalRaw64(  20, 2);

        ASSERT( Util::equal(a64, b64));
        ASSERT( Util::equal(b64, a64));
        ASSERT(!Util::equal(b64, c64));
        ASSERT(!Util::equal(c64, b64));
        ASSERT(!Util::equal(a64, c64));
        ASSERT(!Util::equal(c64, a64));

        a128 = Util::makeDecimalRaw128(   1, 3);
        b128 = Util::makeDecimalRaw128(1000, 0);
        c128 = Util::makeDecimalRaw128(  20, 2);

        ASSERT( Util::equal(a128, b128));
        ASSERT( Util::equal(b128, a128));
        ASSERT(!Util::equal(b128, c128));
        ASSERT(!Util::equal(c128, b128));
        ASSERT(!Util::equal(a128, c128));
        ASSERT(!Util::equal(c128, a128));
    }

    // 'NaN' and 'Inf' testing

    // These tests should be in the same order as Concern #10.

    {
        Util::ValueType32   nan32  = Util::parse32(  "NaN");
        Util::ValueType32  qnan32  = Util::parse32( "qNaN");
        Util::ValueType32  snan32  = Util::parse32( "sNaN");
        Util::ValueType32  pinf32  = Util::parse32( "+Inf");
        Util::ValueType32  ninf32  = Util::parse32( "-Inf");
        Util::ValueType32  test32  = Util::makeDecimalRaw32(42, 1);

        Util::ValueType64   nan64  = Util::parse64(  "NaN");
        Util::ValueType64  qnan64  = Util::parse64( "qNaN");
        Util::ValueType64  snan64  = Util::parse64( "sNaN");
        Util::ValueType64  pinf64  = Util::parse64( "+Inf");
        Util::ValueType64  ninf64  = Util::parse64( "-Inf");
        Util::ValueType64  test64  = Util::makeDecimalRaw64(42, 1);

        Util::ValueType128  nan128 = Util::parse128( "NaN");
        Util::ValueType128 qnan128 = Util::parse128("qNaN");
        Util::ValueType128 snan128 = Util::parse128("sNaN");
        Util::ValueType128 pinf128 = Util::parse128("+Inf");
        Util::ValueType128 ninf128 = Util::parse128("-Inf");
        Util::ValueType128 test128 = Util::makeDecimalRaw128(42, 1);

        // 'NaN' is not equal to itself, in any form.

        ASSERT(!Util::equal( nan32,   nan32));
        ASSERT(!Util::equal(qnan32,  qnan32));
        ASSERT(!Util::equal(snan32,  snan32));

        ASSERT(!Util::equal( nan64,   nan64));
        ASSERT(!Util::equal(qnan64,  qnan64));
        ASSERT(!Util::equal(snan64,  snan64));

        ASSERT(!Util::equal( nan128,  nan128));
        ASSERT(!Util::equal(qnan128, qnan128));
        ASSERT(!Util::equal(snan128, snan128));

        // Cross 'NaN' comparisons should all be false.

        ASSERT(!Util::equal( nan32,  qnan32));
        ASSERT(!Util::equal(qnan32,   nan32));
        ASSERT(!Util::equal( nan32,  snan32));
        ASSERT(!Util::equal(snan32,   nan32));
        ASSERT(!Util::equal(qnan32,  snan32));
        ASSERT(!Util::equal(snan32,  qnan32));

        ASSERT(!Util::equal( nan64,  qnan64));
        ASSERT(!Util::equal(qnan64,   nan64));
        ASSERT(!Util::equal( nan64,  snan64));
        ASSERT(!Util::equal(snan64,   nan64));
        ASSERT(!Util::equal(qnan64,  snan64));
        ASSERT(!Util::equal(snan64,  qnan64));

        ASSERT(!Util::equal( nan128, qnan128));
        ASSERT(!Util::equal(qnan128,  nan128));
        ASSERT(!Util::equal( nan128, snan128));
        ASSERT(!Util::equal(snan128,  nan128));
        ASSERT(!Util::equal(qnan128, snan128));
        ASSERT(!Util::equal(snan128, qnan128));

        // 'NaN' to value comparisons should be false too.

        ASSERT(!Util::equal( nan32,  test32));
        ASSERT(!Util::equal(test32,   nan32));
        ASSERT(!Util::equal(qnan32,  test32));
        ASSERT(!Util::equal(test32,  qnan32));
        ASSERT(!Util::equal(snan32,  test32));
        ASSERT(!Util::equal(test32,  snan32));

        ASSERT(!Util::equal( nan64,  test64));
        ASSERT(!Util::equal(test64,   nan64));
        ASSERT(!Util::equal(qnan64,  test64));
        ASSERT(!Util::equal(test64,  qnan64));
        ASSERT(!Util::equal(snan64,  test64));
        ASSERT(!Util::equal(test64,  snan64));

        ASSERT(!Util::equal( nan128, test128));
        ASSERT(!Util::equal(test128,  nan128));
        ASSERT(!Util::equal(qnan128, test128));
        ASSERT(!Util::equal(test128, qnan128));
        ASSERT(!Util::equal(snan128, test128));
        ASSERT(!Util::equal(test128, snan128));

        // 'Inf's should compare equal only when they have the same sign.

        ASSERT( Util::equal(pinf32,  pinf32));
        ASSERT( Util::equal(ninf32,  ninf32));
        ASSERT(!Util::equal(pinf32,  ninf32));
        ASSERT(!Util::equal(ninf32,  pinf32));

        ASSERT( Util::equal(pinf64,  pinf64));
        ASSERT( Util::equal(ninf64,  ninf64));
        ASSERT(!Util::equal(pinf64,  ninf64));
        ASSERT(!Util::equal(ninf64,  pinf64));

        ASSERT( Util::equal(pinf128, pinf128));
        ASSERT( Util::equal(ninf128, ninf128));
        ASSERT(!Util::equal(pinf128, ninf128));
        ASSERT(!Util::equal(ninf128, pinf128));

        // Value to 'Inf' comparisons should be false.

        ASSERT(!Util::equal(test32,  pinf32));
        ASSERT(!Util::equal(test32,  ninf32));
        ASSERT(!Util::equal(pinf32,  test32));
        ASSERT(!Util::equal(ninf32,  test32));

        ASSERT(!Util::equal(test64,  pinf64));
        ASSERT(!Util::equal(test64,  ninf64));
        ASSERT(!Util::equal(pinf64,  test64));
        ASSERT(!Util::equal(ninf64,  test64));

        ASSERT(!Util::equal(test128, pinf128));
        ASSERT(!Util::equal(test128, ninf128));
        ASSERT(!Util::equal(pinf128, test128));
        ASSERT(!Util::equal(ninf128, test128));

        // 'Inf' to 'NaN' comparisons should be false.

        ASSERT(!Util::equal( nan32,  pinf32));
        ASSERT(!Util::equal( nan32,  ninf32));
        ASSERT(!Util::equal(qnan32,  pinf32));
        ASSERT(!Util::equal(qnan32,  ninf32));
        ASSERT(!Util::equal(snan32,  pinf32));
        ASSERT(!Util::equal(snan32,  ninf32));
        ASSERT(!Util::equal(pinf32,   nan32));
        ASSERT(!Util::equal(ninf32,   nan32));
        ASSERT(!Util::equal(pinf32,  qnan32));
        ASSERT(!Util::equal(ninf32,  qnan32));
        ASSERT(!Util::equal(pinf32,  snan32));
        ASSERT(!Util::equal(ninf32,  snan32));

        ASSERT(!Util::equal( nan64,  pinf64));
        ASSERT(!Util::equal( nan64,  ninf64));
        ASSERT(!Util::equal(qnan64,  pinf64));
        ASSERT(!Util::equal(qnan64,  ninf64));
        ASSERT(!Util::equal(snan64,  pinf64));
        ASSERT(!Util::equal(snan64,  ninf64));
        ASSERT(!Util::equal(pinf64,   nan64));
        ASSERT(!Util::equal(ninf64,   nan64));
        ASSERT(!Util::equal(pinf64,  qnan64));
        ASSERT(!Util::equal(ninf64,  qnan64));
        ASSERT(!Util::equal(pinf64,  snan64));
        ASSERT(!Util::equal(ninf64,  snan64));

        ASSERT(!Util::equal( nan128, pinf128));
        ASSERT(!Util::equal( nan128, ninf128));
        ASSERT(!Util::equal(qnan128, pinf128));
        ASSERT(!Util::equal(qnan128, ninf128));
        ASSERT(!Util::equal(snan128, pinf128));
        ASSERT(!Util::equal(snan128, ninf128));
        ASSERT(!Util::equal(pinf128,  nan128));
        ASSERT(!Util::equal(ninf128,  nan128));
        ASSERT(!Util::equal(pinf128, qnan128));
        ASSERT(!Util::equal(ninf128, qnan128));
        ASSERT(!Util::equal(pinf128, snan128));
        ASSERT(!Util::equal(ninf128, snan128));
    }
}

void TestDriver::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING 'makeDecimalRaw'
    //
    // Concerns:
    //: 1 'makeDecimalRawXX' constructs a decimal floating point value having
    //:   the specified mantissa and exponent, with no rounding.
    //:
    //: 2 The correctness of the conversion of each 3-digit group (or declet)
    //:   into a 10-bit binary-encoded declet.  Note that this is superfluous
    //:   on BID architectures, but causes no significant slowdown.
    //:
    //: 3 The conversion of each 3-digit declet into a 10-bit binary-encoded
    //:   declet does not intefere each other.
    //:
    //: 4 The combination field is correctly computed given the leading digit
    //:   of the mantissa and the exponent.
    //:
    //: 5 'makeDecimalRawXX' asserts in the right build modes for undefined
    //:   behavior.
    //:
    //: 6 'makeDecimalRawXX' generates expected values.
    //
    // Plan:
    //: 1 Test 'makeDecimalRawXX' contract using
    //:   'AssertFailureHandlerGuard' and 'BSLS_ASSERTTEST_ASSERT_OPT_FAIL'
    //:   (C-5)
    //:
    //: 2 Test each overload of each 'makeDecimalRawXX' by representing the
    //:   mantissa in a string format and parsing in each intergral type.
    //:   (C-1..4, 6)
    //:
    //: 3 Test a mantissa of each length in base 10.  (C-1..4, 6)
    //:
    //: 4 Test that 'makeDecimalRawXX' returns the same value as the decimal
    //:   created by invoking the decNumber constructors using binary-coded
    //:   decimals.  (C-1..4, 6)
    //
    // Testing:
    //   makeDecimalRaw32 (                   int, int)
    //   makeDecimalRaw64 (unsigned long long int, int)
    //   makeDecimalRaw64 (         long long int, int)
    //   makeDecimalRaw64 (unsigned           int, int)
    //   makeDecimalRaw64 (                   int, int)
    //   makeDecimalRaw128(unsigned long long int, int)
    //   makeDecimalRaw128(         long long int, int)
    //   makeDecimalRaw128(unsigned           int, int)
    //   makeDecimalRaw128(                   int, int)
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TESTING 'makeDecimalRaw'" << endl
                      << "========================" << endl;

    // Test that with any of a set of exponents, we can create values with
    // mantissas from 1 to 16 digit for 32-bit Decimal values
    {
        Util::ValueType32 test;
        Util::ValueType32 witnessAlternate;
        Util::ValueType32 witnessParse;


                    // 0e0

        test             =    Util::makeDecimalRaw32( 0,0);
        witnessAlternate = alternateMakeDecimalRaw32( 0,0);
        witnessParse     =             Util::parse32("0e0");

        ASSERT(!bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
        ASSERT(!bsl::memcmp(&test, &witnessParse,     sizeof(test)));

                    // 7e0

        test             =    Util::makeDecimalRaw32( 7,0);
        witnessAlternate = alternateMakeDecimalRaw32( 7,0);
        witnessParse     =             Util::parse32("7e0");

        ASSERT(!bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
        ASSERT(!bsl::memcmp(&test, &witnessParse,     sizeof(test)));

                    // 52e0

        test             =    Util::makeDecimalRaw32( 52,0);
        witnessAlternate = alternateMakeDecimalRaw32( 52,0);
        witnessParse     =             Util::parse32("52e0");

        ASSERT(!bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
        ASSERT(!bsl::memcmp(&test, &witnessParse,     sizeof(test)));

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose) cout << "makeDecimalRaw32, mantissa num: "
                                  << t_m << ", " << mantissas[t_m] << endl;
            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose) cout << "makeDecimalRaw32, exponent num: "
                                      << t_e << ", "
                                      << exponents[t_e] << endl;
                long long int mantissa = mantissas[t_m];
                          int exponent = exponents[t_e];


                if (-9999999 <= mantissa && mantissa <= 9999999
                 && -101     <= exponent && exponent <= 90) {
                    int intMantissa = static_cast<int>(mantissa);

                    bsl::string parseString = makeParseString(
                                                    intMantissa, exponent);

                    test = Util::makeDecimalRaw32(intMantissa, exponent);
                    witnessAlternate = alternateMakeDecimalRaw32(
                                                    intMantissa, exponent);

                    witnessParse = Util::parse32(parseString.c_str());

                    LOOP5_ASSERT(t_m, t_e, mantissa, exponent, parseString,
                     !bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
                    LOOP5_ASSERT(t_m, t_e, mantissa, exponent, parseString,
                     !bsl::memcmp(&test, &witnessParse,     sizeof(test)));
                }
            }
        }
    }

    // Test that with any of a set of exponents, we can create values with
    // mantissas from 1 to 16 digit for 64-bit Decimal values
    {
        Util::ValueType64 test;
        Util::ValueType64 witnessAlternate;
        Util::ValueType64 witnessParse;


                    // 0e0

        test             =    Util::makeDecimalRaw64( 0,0);
        witnessAlternate = alternateMakeDecimalRaw64( 0,0);
        witnessParse     =             Util::parse64("0e0");

        ASSERT(!bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
        ASSERT(!bsl::memcmp(&test, &witnessParse,     sizeof(test)));

                    // 7e0

        test             =    Util::makeDecimalRaw64( 7,0);
        witnessAlternate = alternateMakeDecimalRaw64( 7,0);
        witnessParse     =             Util::parse64("7e0");

        ASSERT(!bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
        ASSERT(!bsl::memcmp(&test, &witnessParse,     sizeof(test)));

                    // 52e0

        test             =    Util::makeDecimalRaw64( 52,0);
        witnessAlternate = alternateMakeDecimalRaw64( 52,0);
        witnessParse     =             Util::parse64("52e0");

        ASSERT(!bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
        ASSERT(!bsl::memcmp(&test, &witnessParse,     sizeof(test)));

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose) cout << "makeDecimalRaw64, mantissa num: "
                                  << t_m << ", " << mantissas[t_m] << endl;
            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose) cout << "makeDecimalRaw64, exponent num: "
                                      << t_e << ", "
                                      << exponents[t_e] << endl;
                long long int mantissa = mantissas[t_m];
                          int exponent = exponents[t_e];

                if (mantissa <= bsl::numeric_limits<int>::min()
                 && mantissa <= bsl::numeric_limits<int>::max()) {
                    int intMantissa = static_cast<int>(mantissa);

                    bsl::string parseString = makeParseString(intMantissa,
                                                              exponent);

                    test = Util::makeDecimalRaw64(intMantissa, exponent);
                    witnessAlternate = alternateMakeDecimalRaw64(
                                                               intMantissa,
                                                               exponent);
                    witnessParse = Util::parse64(parseString.c_str());

                    LOOP5_ASSERT(t_m,         t_e,
                                 intMantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessAlternate,
                                              sizeof(test)));
                    LOOP5_ASSERT(t_m,         t_e,
                                 intMantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessParse,
                                              sizeof(test)));
                }

                if (0ll      <= mantissa
                 && mantissa <= bsl::numeric_limits<unsigned>::max()) {
                    unsigned uMantissa = static_cast<unsigned>(mantissa);

                    bsl::string parseString = makeParseString(uMantissa,
                                                              exponent);

                    test = Util::makeDecimalRaw64(uMantissa, exponent);
                    witnessAlternate = alternateMakeDecimalRaw64(
                                                               uMantissa,
                                                               exponent);
                    witnessParse = Util::parse64(parseString.c_str());

                    LOOP5_ASSERT(t_m,       t_e,
                                 uMantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessAlternate,
                                              sizeof(test)));
                    LOOP5_ASSERT(t_m,       t_e,
                                 uMantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessParse,
                                              sizeof(test)));
                }

                if (0ll      <= mantissa) {
                    // And mantissa <= ULONGLONG_MAX.

                    unsigned long long ullMantissa =
                              static_cast<unsigned long long>(mantissa);

                    bsl::string parseString = makeParseString(ullMantissa,
                                                              exponent);

                    test = Util::makeDecimalRaw64(ullMantissa, exponent);
                    witnessAlternate = alternateMakeDecimalRaw64(
                                                               ullMantissa,
                                                               exponent);
                    witnessParse = Util::parse64(parseString.c_str());

                    LOOP5_ASSERT(t_m,         t_e,
                                 ullMantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessAlternate,
                                              sizeof(test)));
                    LOOP5_ASSERT(t_m,         t_e,
                                 ullMantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessParse,
                                              sizeof(test)));
                }

                if (mantissa <= bsl::numeric_limits<long long>::max() &&
                    mantissa >= bsl::numeric_limits<long long>::min()) {
                    bsl::string parseString = makeParseString(mantissa,
                                                              exponent);

                    test = Util::makeDecimalRaw64(mantissa, exponent);
                    witnessAlternate = alternateMakeDecimalRaw64(mantissa,
                                                                 exponent);
                    witnessParse = Util::parse64(parseString.c_str());

                    LOOP5_ASSERT(t_m,      t_e,
                                 mantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessAlternate,
                                              sizeof(test)));
                    LOOP5_ASSERT(t_m,      t_e,
                                 mantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessParse,
                                              sizeof(test)));
                }
            }
        }
    }

    // Test that with any of a set of exponents, we can create values with
    // mantissas from 1 to 16 digit for 128-bit Decimal values
    {
        long long mantissas[] = {
                                                 0LL,
                                                 2LL,
                                                 7LL,
                                                35LL,
                                                72LL,
                                               135LL,
                                               924LL,

                    // Exhaustive mantissa cases
                                                 1LL,
                                                 9LL,
                                                12LL,
                                                98LL,
                                               123LL,
                                               987LL,
                                              1234LL,
                                              9876LL,
                                             12345LL,
                                             98765LL,
                                            123456LL,
                                            987654LL,
                                           1234567LL,
                                           9876543LL,
                                          12345678LL,
                                          98765432LL,
                                         123456789LL,
                                         987654321LL,
                                        1234567890LL,
                                        9876543210LL,
                                       12345678901LL,
                                       98765432109LL,
                                      123456789012LL,
                                      987654321098LL,
                                     1234567890123LL,
                                     9876543210987LL,
                                    12345678901234LL,
                                    98765432109876LL,
                                   123456789012345LL,
                                   987654321098765LL,
                                  1234567890123456LL,
                                  9876543210987654LL,

                                 -               1LL,
                                 -               9LL,
                                 -              12LL,
                                 -              98LL,
                                 -             123LL,
                                 -             987LL,
                                 -            1234LL,
                                 -            9876LL,
                                 -           12345LL,
                                 -           98765LL,
                                 -          123456LL,
                                 -          987654LL,
                                 -         1234567LL,
                                 -         9876543LL,
                                 -        12345678LL,
                                 -        98765432LL,
                                 -       123456789LL,
                                 -       987654321LL,
                                 -      1234567890LL,
                                 -      9876543210LL,
                                 -     12345678901LL,
                                 -     98765432109LL,
                                 -    123456789012LL,
                                 -    987654321098LL,
                                 -   1234567890123LL,
                                 -   9876543210987LL,
                                 -  12345678901234LL,
                                 -  98765432109876LL,
                                 - 123456789012345LL,
                                 - 987654321098765LL,
                                 -1234567890123456LL,
                                 -9876543210987654LL
                                };
        int numMantissas = sizeof(mantissas) / sizeof(*mantissas);

        int exponents[] = {
                              0,
                              1,
                              7,
                             13,
                             64,
                            123,
                            321,

                           -  1,
                           -  7,
                           - 13,
                           - 64,
                           -123,
                           -321
                          };
        int numExponents = sizeof(exponents) / sizeof(*exponents);

        Util::ValueType128 test;
        Util::ValueType128 witnessAlternate;
        Util::ValueType128 witnessParse;


                    // 0e0

        test             =    Util::makeDecimalRaw128( 0,0);
        witnessAlternate = alternateMakeDecimalRaw128( 0,0);
        witnessParse     =             Util::parse128("0e0");

        ASSERT(!bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
        ASSERT(!bsl::memcmp(&test, &witnessParse,     sizeof(test)));

                    // 7e0

        test             =    Util::makeDecimalRaw128( 7,0);
        witnessAlternate = alternateMakeDecimalRaw128( 7,0);
        witnessParse     =             Util::parse128("7e0");

        ASSERT(!bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
        ASSERT(!bsl::memcmp(&test, &witnessParse,     sizeof(test)));

                    // 52e0

        test             =    Util::makeDecimalRaw128( 52,0);
        witnessAlternate = alternateMakeDecimalRaw128( 52,0);
        witnessParse     =             Util::parse128("52e0");

        ASSERT(!bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
        ASSERT(!bsl::memcmp(&test, &witnessParse,     sizeof(test)));

        for (int t_m = 0; t_m < numMantissas; ++t_m) {
            if (veryVerbose) cout << "makeDecimalRaw128, mantissa num: "
                                  << t_m << ", " << mantissas[t_m] << endl;
            for (int t_e = 0; t_e < numExponents; ++t_e) {
                if (veryVerbose) cout << "makeDecimalRaw128, exponent num:"
                                      << " " << t_e << ", "
                                      << exponents[t_e] << endl;
                long long int mantissa = mantissas[t_m];
                          int exponent = exponents[t_e];

                if (mantissa <= bsl::numeric_limits<int>::max() &&
                    mantissa >= bsl::numeric_limits<int>::min()) {
                    int intMantissa = static_cast<int>(mantissa);

                    bsl::string parseString = makeParseString(intMantissa,
                                                              exponent);

                    test = Util::makeDecimalRaw128(intMantissa, exponent);
                    witnessAlternate = alternateMakeDecimalRaw128(
                                                               intMantissa,
                                                               exponent);
                    witnessParse = Util::parse128(parseString.c_str());

                    LOOP5_ASSERT(t_m,         t_e,
                                 intMantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessAlternate,
                                              sizeof(test)));
                    LOOP5_ASSERT(t_m,         t_e,
                                 intMantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessParse,
                                              sizeof(test)));
                }

                if (mantissa <= bsl::numeric_limits<unsigned>::max() &&
                    mantissa >= 0ll) {
                    unsigned uMantissa = static_cast<unsigned>(mantissa);

                    bsl::string parseString = makeParseString(uMantissa,
                                                              exponent);

                    test = Util::makeDecimalRaw128(uMantissa, exponent);
                    witnessAlternate = alternateMakeDecimalRaw128(
                                                                 uMantissa,
                                                                 exponent);
                    witnessParse = Util::parse128(parseString.c_str());

                    LOOP5_ASSERT(t_m,       t_e,
                                 uMantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessAlternate,
                                              sizeof(test)));
                    LOOP5_ASSERT(t_m,       t_e,
                                 uMantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessParse,
                                              sizeof(test)));
                }

                if (mantissa >= 0ll) {
                    // && mantissa <= ULONGLONG_MAX

                    unsigned long long ullMantissa = mantissa;

                    bsl::string parseString = makeParseString(ullMantissa,
                                                              exponent);

                    test = Util::makeDecimalRaw128(ullMantissa, exponent);
                    witnessAlternate = alternateMakeDecimalRaw128(
                                                               ullMantissa,
                                                               exponent);
                    witnessParse = Util::parse128(parseString.c_str());

                    LOOP5_ASSERT(t_m,         t_e,
                                 ullMantissa, exponent,
                                 parseString,
                     !bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
                    LOOP5_ASSERT(t_m,         t_e,
                                 ullMantissa, exponent,
                                 parseString,
                     !bsl::memcmp(&test,
                                  &witnessParse,
                                  sizeof(test)));
                }

                if (mantissa <= bsl::numeric_limits<long long>::max() &&
                    mantissa >= bsl::numeric_limits<long long>::min()) {
                    bsl::string parseString = makeParseString(mantissa,
                                                              exponent);

                    test = Util::makeDecimalRaw128(mantissa, exponent);
                    witnessAlternate = alternateMakeDecimalRaw128(mantissa,
                                                                 exponent);
                    witnessParse = Util::parse128(parseString.c_str());

                    LOOP5_ASSERT(t_m,      t_e,
                                 mantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessAlternate,
                                              sizeof(test)));
                    LOOP5_ASSERT(t_m,      t_e,
                                 mantissa, exponent,
                                 parseString,
                                 !bsl::memcmp(&test,
                                              &witnessParse,
                                              sizeof(test)));
                }
            }
        }
    }


    // Test that 'makeDecimalRaw32' enforces undefined behavior in the
    // right build mode
    {
        AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

        // Check exponent lower bound
        BSLS_ASSERTTEST_ASSERT_FAIL(
                                  Util::makeDecimalRaw32(       42, -102));
        BSLS_ASSERTTEST_ASSERT_PASS(
                                  Util::makeDecimalRaw32(       42, -101));

        // Check exponent upper bound
        BSLS_ASSERTTEST_ASSERT_PASS(
                                  Util::makeDecimalRaw32(       42,   90));
        BSLS_ASSERTTEST_ASSERT_FAIL(
                                  Util::makeDecimalRaw32(       42,   91));

        // Check mantissa lower bound
        BSLS_ASSERTTEST_ASSERT_FAIL(
                                  Util::makeDecimalRaw32(-10000000,   42));
        BSLS_ASSERTTEST_ASSERT_PASS(
                                  Util::makeDecimalRaw32(- 9999999,   42));

        // Check mantissa upper bound
        BSLS_ASSERTTEST_ASSERT_PASS(
                                  Util::makeDecimalRaw32(  9999999,   42));
        BSLS_ASSERTTEST_ASSERT_FAIL(
                                  Util::makeDecimalRaw32( 10000000,   42));
    }

    // Test that 'makeDecimalRaw64' variants enforce undefined behavior in
    // the right build mode
    {
        AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

        // Test 'makeDecimalRaw64(int, int)':

        // Check exponent lower bound
        BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw64(42, -399));
        BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw64(42, -398));

        // Check exponent upper bound
        BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw64(42,  369));
        BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw64(42,  370));

        // 'makeDecimalRaw64(int, int)' needs no test for mantissa bounds
        // because 9,999,999,999,999,999 is unrepresentable as an int.

        // Test 'makeDecimalRaw64(unsigned int, int)':

        // Check exponent lower bound
        BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw64(42u, -399));
        BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw64(42u, -398));

        // Check exponent upper bound
        BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw64(42u,  369));
        BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw64(42u,  370));

        // 'makeDecimalRaw64(unsigned int, int)' needs no test for mantissa
        // bounds because 9,999,999,999,999,999 is unrepresentable as an
        // int.

        // Test 'makeDecimalRaw64(long long, int)':

        // Check exponent lower bound
        BSLS_ASSERTTEST_ASSERT_FAIL(
                       Util::makeDecimalRaw64(                42ll, -399));
        BSLS_ASSERTTEST_ASSERT_PASS(
                       Util::makeDecimalRaw64(                42ll, -398));

        // Check exponent upper bound
        BSLS_ASSERTTEST_ASSERT_PASS(
                       Util::makeDecimalRaw64(                42ll,  369));
        BSLS_ASSERTTEST_ASSERT_FAIL(
                       Util::makeDecimalRaw64(                42ll,  370));

        // Check mantissa lower bound
        BSLS_ASSERTTEST_ASSERT_FAIL(
                       Util::makeDecimalRaw64(-10000000000000000ll,   42));
        BSLS_ASSERTTEST_ASSERT_PASS(
                       Util::makeDecimalRaw64(- 9999999999999999ll,   42));

        // Check mantissa upper bound
        BSLS_ASSERTTEST_ASSERT_PASS(
                       Util::makeDecimalRaw64(  9999999999999999ll,   42));
        BSLS_ASSERTTEST_ASSERT_FAIL(
                       Util::makeDecimalRaw64( 10000000000000000ll,   42));

        // Test 'makeDecimalRaw64(unsigned long long, int)':

        // Check exponent lower bound
        BSLS_ASSERTTEST_ASSERT_FAIL(
                       Util::makeDecimalRaw64(               42ull, -399));
        BSLS_ASSERTTEST_ASSERT_PASS(
                       Util::makeDecimalRaw64(               42ull, -398));

        // Check exponent upper bound
        BSLS_ASSERTTEST_ASSERT_PASS(
                       Util::makeDecimalRaw64(               42ull,  369));
        BSLS_ASSERTTEST_ASSERT_FAIL(
                       Util::makeDecimalRaw64(               42ull,  370));

        // Unsigned numbers cannot be negative, so we do not have a lower
        // bound test.

        // Check mantissa upper bound
        BSLS_ASSERTTEST_ASSERT_PASS(
                       Util::makeDecimalRaw64( 9999999999999999ull,   42));
        BSLS_ASSERTTEST_ASSERT_FAIL(
                       Util::makeDecimalRaw64(10000000000000000ull,   42));
    }

    // Test that 'makeDecimalRaw128' variants enforce undefined behavior in
    // the right build mode
    {
        AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);

        // Test 'makeDecimalRaw128(int, int)':

        // Check exponent lower bound
        BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(42, -6177));
        BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(42, -6176));
        // Check exponent upper bound
        BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(42,  6111));
        BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(42,  6112));

        // Test 'makeDecimalRaw128(unsigned, int)':

        // Check exponent lower bound
        BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(42u, -6177));
        BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(42u, -6176));

        // Check exponent upper bound
        BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(42u,  6111));
        BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(42u,  6112));

        // Test 'makeDecimalRaw128(long long, int)':

        // Check exponent lower bound
        BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(42ll, -6177));
        BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(42ll, -6176));

        // Check exponent upper bound
        BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(42ll,  6111));
        BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(42ll,  6112));

        // Test 'makeDecimalRaw128(unsigned long long, int)':

        // Check exponent lower bound
        BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(42ull, -6177));
        BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(42ull, -6176));

        // Check exponent upper bound
        BSLS_ASSERTTEST_ASSERT_PASS(Util::makeDecimalRaw128(42ull,  6111));
        BSLS_ASSERTTEST_ASSERT_FAIL(Util::makeDecimalRaw128(42ull,  6112));
    }
}

void TestDriver::testCase1()
{
    // ------------------------------------------------------------------------
    // BREATHING TEST
    //
    // Concerns:
    //:  1 N/A
    //
    // Plan:
    //:  1 N/A
    //
    // Testing:
    //   BREATHING TEST
    //   checkLiteral(double)
    // ------------------------------------------------------------------------
    if (verbose) cout << endl << "BREATHING TEST"
                      << endl << "==============" << endl;

    // 'checkLiteral' is called in this test case, to silence bde_verify.
    // This is not tested in its own test, because it is not a public
    // method.

#ifdef BDLDFP_DECIMALPLATFORM_SOFTWARE
    Util::checkLiteral(0.0);
#endif
}


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
                   test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;  // always the last

    using bsls::AssertFailureHandlerGuard;

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    cout.precision(35);


    switch (test) { case 0:
      case 23: {
        TestDriver::testCase23();
      } break;
      case 22: {
        TestDriver::testCase22();
      } break;
      case 21: {
        TestDriver::testCase21();
      } break;
      case 20: {
        TestDriver::testCase20();
      } break;
      case 19: {
        TestDriver::testCase19();
      } break;
      case 18: {
        TestDriver::testCase18();
      } break;
      case 17: {
        TestDriver::testCase17();
      } break;
      case 16: {
        TestDriver::testCase16();
      } break;
      case 15: {
        TestDriver::testCase15();
      } break;
      case 14: {
        TestDriver::testCase14();
      } break;
      case 13: {
        TestDriver::testCase13();
      } break;
      case 12: {
        TestDriver::testCase12();
      } break;
      case 11: {
        TestDriver::testCase11();
      } break;
      case 10: {
        TestDriver::testCase10();
      } break;
      case 9: {
        TestDriver::testCase9();
      } break;
      case 8: {
        TestDriver::testCase8();
      } break;
      case 7: {
        TestDriver::testCase7();
      } break;
      case 6: {
        TestDriver::testCase6();
      } break;
      case 5: {
        TestDriver::testCase5();
      } break;
      case 4: {
        TestDriver::testCase4();
      } break;
      case 3: {
        TestDriver::testCase3();
      } break;
      case 2: {
        TestDriver::testCase2();
      } break;
      case 1: {
        TestDriver::testCase1();
      } break; // Breathing test dummy
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: No memory came from the global or default allocator.

    //LOOP2_ASSERT(test, globalAllocator.numBlocksTotal(),
                 //0 == globalAllocator.numBlocksTotal());
    //LOOP2_ASSERT(test, defaultAllocator.numBlocksTotal(),
                 //0 == defaultAllocator.numBlocksTotal());

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
