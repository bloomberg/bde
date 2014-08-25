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
// CLASS METHODS
// [  ] makeDecimal64(                   int, int)
// [  ] makeDecimal64(unsigned           int, int)
// [  ] makeDecimal64(         long long int, int)
// [  ] makeDecimal64(unsigned long long int, int)
// [  ] makeInfinity64(bool)
// [  ]  int32ToDecimal32 (                   int)
// [  ] uint32ToDecimal32 (unsigned           int)
// [  ]  int64ToDecimal32 (         long long int)
// [  ] uint64ToDecimal32 (unsigned long long int)
// [  ]  int32ToDecimal64 (                   int)
// [  ] uint32ToDecimal64 (unsigned           int)
// [  ]  int64ToDecimal64 (         long long int)
// [  ] uint64ToDecimal64 (unsigned long long int)
// [  ]  int32ToDecimal128(                   int)
// [  ] uint32ToDecimal128(unsigned          int)
// [  ]  int64ToDecimal128(         long long int)
// [  ] uint64ToDecimal128(unsigned long long int)
// [  ] add(ValueType64,  ValueType64)
// [  ] add(ValueType128, ValueType128)
// [  ] subtract(ValueType64,  ValueType64)
// [  ] subtract(ValueType128, ValueType128)
// [  ] multiply(ValueType64,  ValueType64)
// [  ] multiply(ValueType128, ValueType128)
// [  ] divide(ValueType64,  ValueType64)
// [  ] divide(ValueType128, ValueType128)
// [  ] negate(ValueType32)
// [  ] negate(ValueType64)
// [  ] negate(ValueType128)
// [  ] less(ValueType32,  ValueType32)
// [  ] less(ValueType64,  ValueType64)
// [  ] less(ValueType128, ValueType128)
// [  ] greater(ValueType32,  ValueType32)
// [  ] greater(ValueType64,  ValueType64)
// [  ] greater(ValueType128, ValueType128)
// [  ] lessEqual(ValueType32,  ValueType32)
// [  ] lessEqual(ValueType64,  ValueType64)
// [  ] lessEqual(ValueType128, ValueType128)
// [  ] greaterEqual(ValueType32,  ValueType32)
// [  ] greaterEqual(ValueType64,  ValueType64)
// [  ] greaterEqual(ValueType128, ValueType128)
// [ 3] equal(ValueType32,  ValueType32)
// [ 3] equal(ValueType64,  ValueType64)
// [ 3] equal(ValueType128, ValueType128)
// [ 4] notEqual(ValueType32,  ValueType32)
// [ 4] notEqual(ValueType64,  ValueType64)
// [ 4] notEqual(ValueType128, ValueType128)
// [  ] convertToDecimal32( const ValueType64&)
// [  ] convertToDecimal64( const ValueType32&)
// [  ] convertToDecimal64( const ValueType128&)
// [  ] convertToDecimal128(const ValueType32&)
// [  ] convertToDecimal128(const ValueType64&)
// [  ] binaryToDecimal32 (float)
// [  ] binaryToDecimal32 (double)
// [  ] binaryToDecimal32 (long double)
// [  ] binaryToDecimal64 (float)
// [  ] binaryToDecimal64 (double)
// [  ] binaryToDecimal64 (long double)
// [  ] binaryToDecimal128(float)
// [  ] binaryToDecimal128(double)
// [  ] binaryToDecimal128(long double)
// [ 2] makeDecimalRaw32 (                   int, int)
// [ 2] makeDecimalRaw64 (unsigned long long int, int)
// [ 2] makeDecimalRaw64 (         long long int, int)
// [ 2] makeDecimalRaw64 (unsigned           int, int)
// [ 2] makeDecimalRaw64 (                   int, int)
// [ 2] makeDecimalRaw128(unsigned long long int, int)
// [ 2] makeDecimalRaw128(         long long int, int)
// [ 2] makeDecimalRaw128(unsigned           int, int)
// [ 2] makeDecimalRaw128(                   int, int)
// [  ] scaleB(ValueType32,  int)
// [  ] scaleB(ValueType64,  int)
// [  ] scaleB(ValueType128, int)
// [  ] parse32 (const char *)
// [  ] parse64 (const char *)
// [  ] parse128(const char *)
// [  ] format(ValueType32,  char *)
// [  ] format(ValueType64,  char *)
// [  ] format(ValueType128, char *)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] TEST 'notEqual' FOR 'NaN' CORRECTNESS
// [42] USAGE EXAMPLE
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
      case 42: {
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
      case 4: {
        // --------------------------------------------------------------------
        // INEQUALITY COMPARISON 'Util::notEqual'
        //
        // Concerns:
        //:  1 'notEqual' should return false if the bit patterns are the same (and
        //:    are not a bit pattern representing a 'NaN').
        //:
        //:  2 'notEqual' should return false if the bit patterns are different,
        //:    but represent the same value.  I.e. '10e0' should be equal to
        //:    '1e1'.
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
        //:    function, but should perform more than a cursory forwarding
        //:    test.  (This will result in more than a simple forwarding test.)
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
        //:  9 Any two things equal to each other are both inequal to an
        //:    inequal third.
        //:    (Transitive property.)
        //:       'A == B' and 'B != C' implies 'A != C'
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
        // --------------------------------------------------------------------

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
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // EQUALITY COMPARISON 'Util::equal'
        //
        // Concerns:
        //:  1 'equal' should return true if the bit patterns are the same (and
        //:    are not a bit pattern representing a 'NaN').
        //:
        //:  2 'equal' should return true if the bit patterns are different,
        //:    but represent the same value.  I.e. '10e0' should be equal to
        //:    '1e1'.
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
        //:    function, but should perform more than a cursory forwarding
        //:    test.  (This will result in more than a simple forwarding test.)
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
        //:  9 Any two things equal to each other are both inequal to an
        //:    inequal third.
        //:    (Transitive property.)
        //:       'A == B' and 'B != C' implies 'A != C'
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
      } break;
      case 2: {
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
        //: 4 Test that 'makeDecimalRawXX' returns the same value as the
        //:   decimal created by invoking the decNumber constructors using
        //:   binary-coded decimals.  (C-1..4, 6)
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
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << bsl::endl
                                << "PRIMARY MANIPULATORS"
                                << "====================" << bsl::endl;

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
                for (int t_e = 0; t_e < numExponents; ++t_e) {
                    long long mantissa = mantissas[t_m];
                    int exponent = exponents[t_e];

                    if (mantissa <= 9999999 && mantissa >= -9999999
                     && exponent <= 90      && exponent >= -101) {

                        bsl::string parseString = makeParseString(mantissa,
                                                                  exponent);

                        test = Util::makeDecimalRaw32(mantissa, exponent);
                        witnessAlternate = alternateMakeDecimalRaw32(mantissa,
                                                                     exponent);
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
                for (int t_e = 0; t_e < numExponents; ++t_e) {
                    long long mantissa = mantissas[t_m];
                    int exponent = exponents[t_e];

                    if (mantissa <= bsl::numeric_limits<int>::max() &&
                        mantissa >= bsl::numeric_limits<int>::min()) {
                        int intMantissa = mantissa;

                        bsl::string parseString = makeParseString(intMantissa,
                                                                  exponent);

                        test = Util::makeDecimalRaw64(intMantissa, exponent);
                        witnessAlternate = alternateMakeDecimalRaw64(
                                                                   intMantissa,
                                                                   exponent);
                        witnessParse = Util::parse64(parseString.c_str());

                        LOOP5_ASSERT(t_m, t_e, intMantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
                        LOOP5_ASSERT(t_m, t_e, intMantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessParse,     sizeof(test)));
                    }

                    if (mantissa <= bsl::numeric_limits<unsigned>::max() &&
                        mantissa >= 0ll) {
                        unsigned uMantissa = mantissa;

                        bsl::string parseString = makeParseString(uMantissa,
                                                                  exponent);

                        test = Util::makeDecimalRaw64(uMantissa, exponent);
                        witnessAlternate = alternateMakeDecimalRaw64(
                                                                   uMantissa,
                                                                   exponent);
                        witnessParse = Util::parse64(parseString.c_str());

                        LOOP5_ASSERT(t_m, t_e, uMantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
                        LOOP5_ASSERT(t_m, t_e, uMantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessParse,     sizeof(test)));
                    }

                    if (mantissa <=
                              bsl::numeric_limits<unsigned long long>::max() &&
                        mantissa >= 0ll) {
                        unsigned long long ullMantissa = mantissa;

                        bsl::string parseString = makeParseString(ullMantissa,
                                                                  exponent);

                        test = Util::makeDecimalRaw64(ullMantissa, exponent);
                        witnessAlternate = alternateMakeDecimalRaw64(
                                                                   ullMantissa,
                                                                   exponent);
                        witnessParse = Util::parse64(parseString.c_str());

                        LOOP5_ASSERT(t_m, t_e,
                                     ullMantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
                        LOOP5_ASSERT(t_m, t_e,
                                     ullMantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessParse,     sizeof(test)));
                    }

                    if (mantissa <= bsl::numeric_limits<long long>::max() &&
                        mantissa >= bsl::numeric_limits<long long>::min()) {
                        bsl::string parseString = makeParseString(mantissa,
                                                                  exponent);

                        test = Util::makeDecimalRaw64(mantissa, exponent);
                        witnessAlternate = alternateMakeDecimalRaw64(mantissa,
                                                                     exponent);
                        witnessParse = Util::parse64(parseString.c_str());

                        LOOP5_ASSERT(t_m, t_e, mantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
                        LOOP5_ASSERT(t_m, t_e, mantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessParse,     sizeof(test)));
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
                for (int t_e = 0; t_e < numExponents; ++t_e) {
                    long long mantissa = mantissas[t_m];
                    int exponent = exponents[t_e];

                    if (mantissa <= bsl::numeric_limits<int>::max() &&
                        mantissa >= bsl::numeric_limits<int>::min()) {
                        int intMantissa = mantissa;

                        bsl::string parseString = makeParseString(intMantissa,
                                                                  exponent);

                        test = Util::makeDecimalRaw128(intMantissa, exponent);
                        witnessAlternate = alternateMakeDecimalRaw128(
                                                                   intMantissa,
                                                                   exponent);
                        witnessParse = Util::parse128(parseString.c_str());

                        LOOP5_ASSERT(t_m, t_e, intMantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
                        LOOP5_ASSERT(t_m, t_e, intMantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessParse,     sizeof(test)));
                    }

                    if (mantissa <= bsl::numeric_limits<unsigned>::max() &&
                        mantissa >= 0ll) {
                        unsigned uMantissa = mantissa;

                        bsl::string parseString = makeParseString(uMantissa,
                                                                  exponent);

                        test = Util::makeDecimalRaw128(uMantissa, exponent);
                        witnessAlternate = alternateMakeDecimalRaw128(
                                                                   uMantissa,
                                                                   exponent);
                        witnessParse = Util::parse128(parseString.c_str());

                        LOOP5_ASSERT(t_m, t_e, uMantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
                        LOOP5_ASSERT(t_m, t_e, uMantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessParse,     sizeof(test)));
                    }

                    if (mantissa <=
                              bsl::numeric_limits<unsigned long long>::max() &&
                        mantissa >= 0ll ) {
                        unsigned long long ullMantissa = mantissa;

                        bsl::string parseString = makeParseString(ullMantissa,
                                                                  exponent);

                        test = Util::makeDecimalRaw128(ullMantissa, exponent);
                        witnessAlternate = alternateMakeDecimalRaw128(
                                                                   ullMantissa,
                                                                   exponent);
                        witnessParse = Util::parse128(parseString.c_str());

                        LOOP5_ASSERT(t_m, t_e,
                                     ullMantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
                        LOOP5_ASSERT(t_m, t_e,
                                     ullMantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessParse,     sizeof(test)));
                    }

                    if (mantissa <= bsl::numeric_limits<long long>::max() &&
                        mantissa >= bsl::numeric_limits<long long>::min()) {
                        bsl::string parseString = makeParseString(mantissa,
                                                                  exponent);

                        test = Util::makeDecimalRaw128(mantissa, exponent);
                        witnessAlternate = alternateMakeDecimalRaw128(mantissa,
                                                                     exponent);
                        witnessParse = Util::parse128(parseString.c_str());

                        LOOP5_ASSERT(t_m, t_e, mantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessAlternate, sizeof(test)));
                        LOOP5_ASSERT(t_m, t_e, mantissa, exponent, parseString,
                         !bsl::memcmp(&test, &witnessParse,     sizeof(test)));
                    }
                }
            }
        }


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
      case 1: {} break; // Breathing test dummy
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
