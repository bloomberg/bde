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
// [  ] uint32ToDecimal128(unsigned           int)
// [  ]  int64ToDecimal128(         long long int)
// [  ] uint64ToDecimal128(unsigned long long int)
// [ 5] add(ValueType64,  ValueType64)
// [ 5] add(ValueType128, ValueType128)
// [ 6] subtract(ValueType64,  ValueType64)
// [ 6] subtract(ValueType128, ValueType128)
// [ 7] multiply(ValueType64,  ValueType64)
// [ 7] multiply(ValueType128, ValueType128)
// [ 8] divide(ValueType64,  ValueType64)
// [ 8] divide(ValueType128, ValueType128)
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
// [  ] equal(ValueType32,  ValueType32)
// [  ] equal(ValueType64,  ValueType64)
// [  ] equal(ValueType128, ValueType128)
// [  ] notEqual(ValueType32,  ValueType32)
// [  ] notEqual(ValueType64,  ValueType64)
// [  ] notEqual(ValueType128, ValueType128)
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
    // Return true if the specified 'lhs' and 'rhs' are the same value,
    // even in the case of 'NaN'.  Two 'ValueType64' objects are considered
    // equal if either 'Util::equal' returns true, or both 'lhs' and 'rhs'
    // are not equal to themselves (implying them both to be 'NaN').
{
    return  Util::equal(lhs, rhs)
        || (Util::notEqual(lhs, lhs) && Util::notEqual(rhs, rhs));
}

bool nanEqual(Util::ValueType128 lhs, Util::ValueType128 rhs)
    // Return true if the specified 'lhs' and 'rhs' are the same value,
    // even in the case of 'NaN'.  Two 'ValueType128' objects are considered
    // equal if either 'Util::equal' returns true, or both 'lhs' and 'rhs'
    // are not equal to themselves (implying them both to be 'NaN').
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
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'divide'
        //
        // Concerns:
        //:  1 'divide' should never (except in identity, 'NaN' and 'Inf'
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
        //:  3 When not dealing with special cases (or rounding cases), make
        //:    sure the result value does not compare equal to either source.
        //:    (C-3)
        //:
        //:  4 Test all 16 special cases in the chart, which covers both
        //:    orders. (C-4,6)
        //
        // Testing:
        //   divide(ValueType64,  ValueType64)
        //   divide(ValueType128, ValueType128)
        // --------------------------------------------------------------------

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
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'multiply'
        //
        // Concerns:
        //:  1 'multiply' should never (except in identity, 'NaN' and 'Inf'
        //:    cases) return the same value as at least one of the operands.
        //:
        //:  2 'multiply' behaves correctly with identity operand (1).
        //:
        //:  3 'multiply' will return a distinct value from both of its
        //:    operands, when both are non-zero and value-states (not 'NaN' or
        //:    'Inf').
        //:
        //:  4 'multiply' to 'NaN' or 'Inf' should follow the IEEE rules for
        //:    the cases listed in this chart:
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
        //:  5 'multiply' behaves reasonably when working with different
        //:    quanta.
        //:
        //:  6 'multiply' by 0 behaves as expected.
        //
        // Plan:
        //:  1 Test a handful of expected values of different exponent and
        //:    mantissa, with both possible orders. (C-1,5,6)
        //:
        //:  2 Test identity with '1' case. (C-2)
        //:
        //:  3 When not dealing with special cases (or rounding cases), make
        //:    sure the result value does not compare equal to either source.
        //:    (C-3)
        //:
        //:  4 Test all 16 special cases in the chart, which covers both
        //:    orders. (C-4,6)
        //
        // Testing:
        //   multiply(ValueType64,  ValueType64)
        //   multiply(ValueType128, ValueType128)
        // --------------------------------------------------------------------

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
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'subtract'
        //
        // Concerns:
        //:  1 'subtract' should never (except in identity, 'NaN' and 'Inf'
        //:    cases) return the same value as at least one of the operands.
        //:
        //:  2 'subtract' behaves correctly with identity operands (0 and -0).
        //:
        //:  3 'subtract' will return a distinct value from both of its
        //:    operands, when both are non-zero and value-states (not 'NaN' or
        //:    'Inf').
        //:
        //:  4 'subtract' to 'NaN' or 'Inf' should follow the IEEE rules for
        //:    the cases listed in this chart:
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
        //:  3 When not dealing with special cases (or rounding cases), make
        //:    sure the result value does not compare equal to either source.
        //:    (C-3)
        //:
        //:  4 Test all 16 special cases in the chart, which covers both
        //:    orders. (C-4,6)
        //
        // Testing:
        //   subtract(ValueType64,  ValueType64)
        //   subtract(ValueType128, ValueType128)
        // --------------------------------------------------------------------

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

            Util::ValueType64 negativeZero64 = Util::parse64("-0");

               lhs64 = Util::makeDecimalRaw64(lhsMantissa, lhsExponent);
               rhs64 = Util::makeDecimalRaw64(rhsMantissa, rhsExponent);
            result64 = Util::subtract(lhs64, rhs64);

            LOOP6_ASSERT(lhsMantissa, lhsExponent,
                         rhsMantissa, rhsExponent,
                         resMantissa, resExponent,
                   Util::equal(result64, Util::makeDecimalRaw64(resMantissa,
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
                   Util::equal(result128, Util::makeDecimalRaw128(resMantissa,
                                                                  resExponent)));
            LOOP6_ASSERT(lhsMantissa, lhsExponent,
                         rhsMantissa, rhsExponent,
                         resMantissa, resExponent,
                   Util::equal(lhs128, Util::subtract(lhs128,
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
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'add'
        //
        // Concerns:
        //:  1 'add' should never (except in 'NaN' and 'Inf' cases) return the
        //:    same value as at least one of the operands.
        //:
        //:  2 'add' behaves correctly with identity operands (0 and -0).
        //:
        //:  3 'add' will return a distinct value from both of its operands,
        //:    when both are non-zero and value-states (not 'NaN' or 'Inf').
        //:
        //:  4 'add' to 'NaN' or 'Inf' should follow the IEEE rules for the
        //:    cases listed in this chart:
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
        //:  6 'add' must be a commutative operation (when avoiding corner
        //     cases).
        //
        // Plan:
        //:  1 Test a handful of expected values of different exponent and
        //:    mantissa, with both possible orders. (C-1,5,6)
        //:
        //:  2 Test identity with both '-0' and '0' cases. (C-2)
        //:
        //:  3 When not dealing with special cases (or rounding cases), make
        //:    sure the result value does not compare equal to either source.
        //:    (C-3)
        //:
        //:  4 Test all 16 special cases in the chart, which covers both
        //:    orders. (C-4,6)
        //
        // Testing:
        //   add(ValueType64,  ValueType64)
        //   add(ValueType128, ValueType128)
        // --------------------------------------------------------------------

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
      } break;
      case 4: {
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
            int mantissas[] = {
                                      0,
                                      2,
                                      7,
                                     35,
                                     72,
                                    135,
                                    924,

                        // Exhaustive mantissa cases
                                      1,
                                      9,
                                     12,
                                     98,
                                    123,
                                    987,
                                   1234,
                                   9876,
                                  12345,
                                  98765,
                                 123456,
                                 987654,
                                1234567,
                                9876543,

                               -      1,
                               -      9,
                               -     12,
                               -     98,
                               -    123,
                               -    987,
                               -   1234,
                               -   9876,
                               -  12345,
                               -  98765,
                               - 123456,
                               - 987654,
                               -1234567,
                               -9876543
                              };
            int numMantissas = sizeof(mantissas) / sizeof(*mantissas);

            int exponents[] = {
                                  0,
                                  1,
                                  7,
                                 13,
                                 64,

                                - 1,
                                - 7,
                                -13,
                                -64
                              };
            int numExponents = sizeof(exponents) / sizeof(exponents);

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
                    int mantissa = mantissas[t_m];
                    int exponent = exponents[t_e];

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

        // Test that with any of a set of exponents, we can create values with
        // mantissas from 1 to 16 digit for 64-bit Decimal values
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
                        mantissa >= bsl::numeric_limits<unsigned>::min()) {
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
                        mantissa >=
                              bsl::numeric_limits<unsigned long long>::min()) {
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
                        mantissa >= bsl::numeric_limits<unsigned>::min()) {
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
                        mantissa >=
                              bsl::numeric_limits<unsigned long long>::min()) {
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
