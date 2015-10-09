// bdldfp_decimalimputil.h                                            -*-C++-*-

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL
#define INCLUDED_BDLDFP_DECIMALIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide a unified low-level interface for decimal floating point.
//
//@CLASSES:
//  bdldfp::DecimalImpUtil: Unified low-level decimal floating point functions.
//
//@SEE_ALSO: bdldfp_decimalimputil_inteldfp, bdldfp_decimalimputil_decnumber
//
//@DESCRIPTION: This component provides a namespace, 'bdldfp::DecimalImpUtil',
// containing primitive utilities used in the implementation of a decimal
// floating point type (e.g., see 'bdldfp_decimal').
//
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
//..
//  int coefficient = 425; // Yet another name for significand
//  int exponent    =  -1;
//..
// Then we call 'makeDecimal32', 'makeDecimal64', and 'makeDecimal128' to
// construct a 'Decimal32', 'Decimal64', and 'Decimal128' respectively.
//..
//  bdldfp::DecimalImpUtil::ValueType32  d32 =
//            bdldfp::DecimalImpUtil::makeDecimalRaw32( coefficient, exponent);
//  bdldfp::DecimalImpUtil::ValueType64  d64 =
//            bdldfp::DecimalImpUtil::makeDecimalRaw64( coefficient, exponent);
//  bdldfp::DecimalImpUtil::ValueType128 d128 =
//            bdldfp::DecimalImpUtil::makeDecimalRaw128(coefficient, exponent);
//
//  ASSERT(bdldfp::DecimalImpUtil::equal(
//                    bdldfp::DecimalImpUtil::binaryToDecimal32( 42.5), d32));
//  ASSERT(bdldfp::DecimalImpUtil::equal(
//                    bdldfp::DecimalImpUtil::binaryToDecimal64( 42.5), d64));
//  ASSERT(bdldfp::DecimalImpUtil::equal(
//                    bdldfp::DecimalImpUtil::binaryToDecimal128(42.5), d128));
//..
//
///Example 2: Adding Two Decimal Floating Point Values
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Decimal floating point values are frequently used in arithmetic computations
// where the precise representation of decimal values is of paramount
// importance (for example, financial calculations, as currency is typically
// denominated in base-10 decimal values).  In the following example we
// demonstrate computing the sum of a sequence of security prices, where each
// price is held in a 'DecimalImpUtil::ValueType64' value.
//
// First, we we define the signature of a function that computes the sum of an
// array of security prices, and returns that sum as a decimal floating point
// value:
//..
//  bdldfp::DecimalImpUtil::ValueType64
//  totalSecurities(bdldfp::DecimalImpUtil::ValueType64 *prices,
//                  int                                  numPrices)
//      // Return a Decimal Floating Point number representing the arithmetic
//      // total of the values specified by 'prices' and 'numPrices'.
//  {
//..
// Then, we create a local variable to hold the intermediate sum, and set it to
// 0:
//..
//      bdldfp::DecimalImpUtil::ValueType64 total;
//      total = bdldfp::DecimalImpUtil::int32ToDecimal64(0);
//..
// Next, we loop over the array of 'prices' and add each price to the
// intermediate 'total':
//..
//      for (int i = 0; i < numPrices; ++i) {
//          total = bdldfp::DecimalImpUtil::add(total, prices[i]);
//      }
//..
// Now, we return the computed total value of the securities:
//..
//      return total;
//  }
//..
// Notice that 'add' is called as a function, and is not an operator overload
// for '+'; this is because the 'bdldfp::DecimalImpUtil' utility is intended to
// be used in the implementation of operator overloads on a more full fledged
// type.
//
// Finally, we call the function with some sample data, and check the result:
//..
//  bdldfp::DecimalImpUtil::ValueType64 data[16];
//
//  for (int i = 0; i < 16; ++i) {
//      data[i] = bdldfp::DecimalImpUtil::int32ToDecimal64(i + 1);
//  }
//
//  bdldfp::DecimalImpUtil::ValueType64 result;
//  result = totalSecurities(data, 16);
//
//  bdldfp::DecimalImpUtil::ValueType64 expected;
//
//  expected = bdldfp::DecimalImpUtil::int32ToDecimal64(16);
//
//  // Totals of values from 1 to 'x' are '(x * x + x) / 2':
//
//  expected = bdldfp::DecimalImpUtil::add(
//               bdldfp::DecimalImpUtil::multiply(expected, expected),
//               expected);
//  expected = bdldfp::DecimalImpUtil::divide(
//                       expected,
//                       bdldfp::DecimalImpUtil::int32ToDecimal64(2));
//
//  assert(bdldfp::DecimalImpUtil::equal(expected, result));
//..
// Notice that arithmetic is unwieldy and hard to visualize.  This is by
// design, as the DecimalImpUtil and subordinate components are not intended
// for public consumption, or direct use in decimal arithmetic.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL_DECNUMBER
#include <bdldfp_decimalimputil_decnumber.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL_IBMXLC
#include <bdldfp_decimalimputil_ibmxlc.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL_INTELDFP
#include <bdldfp_decimalimputil_inteldfp.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#include <bdldfp_decimalplatform.h>
#endif

#ifndef INCLUDED_BDLDFP_DENSELYPACKEDDECIMALIMPUTIL
#include <bdldfp_denselypackeddecimalimputil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifdef BDLDFP_DECIMALPLATFORM_SOFTWARE

                // DECIMAL FLOATING-POINT LITERAL EMULATION


#define BDLDFP_DECIMALIMPUTIL_DF(lit)                                        \
    BloombergLP::bdldfp::DecimalImpUtil::parse32(                            \
        (BloombergLP::bdldfp::DecimalImpUtil::checkLiteral(lit), #lit))

#define BDLDFP_DECIMALIMPUTIL_DD(lit)                                        \
    BloombergLP::bdldfp::DecimalImpUtil::parse64(                            \
        (BloombergLP::bdldfp::DecimalImpUtil::checkLiteral(lit), #lit))

#define BDLDFP_DECIMALIMPUTIL_DL(lit)                                        \
    BloombergLP::bdldfp::DecimalImpUtil::parse128(                           \
        (BloombergLP::bdldfp::DecimalImpUtil::checkLiteral(lit), #lit))

#elif defined(BDLDFP_DECIMALPLATFORM_C99_TR) || defined( __IBM_DFP__ )

#define BDLDFP_DECIMALIMPUTIL_JOIN_(a,b) a##b

               // Portable decimal floating-point literal support

#define BDLDFP_DECIMALIMPUTIL_DF(lit) BDLDFP_DECIMALIMPUTIL_JOIN_(lit,df)

#define BDLDFP_DECIMALIMPUTIL_DD(lit) BDLDFP_DECIMALIMPUTIL_JOIN_(lit,dd)

#define BDLDFP_DECIMALIMPUTIL_DL(lit) BDLDFP_DECIMALIMPUTIL_JOIN_(lit,dl)

#endif


namespace BloombergLP {
namespace bdldfp {

                        // ====================
                        // class DecimalImpUtil
                        // ====================

class DecimalImpUtil {
    // This 'struct' provides a namespace for utility functions that implement
    // core decimal floating-poing operations.

  private:
#ifdef BDLDFP_DECIMALPLATFORM_DECNUMBER
    typedef DecimalImpUtil_DecNumber Imp;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    typedef DecimalImpUtil_IntelDfp  Imp;
#elif defined(BDLDFP_DECIMALPLATFORM_C99_TR)
    typedef DecimalImpUtil_IbmXlc    Imp;
#else
    BSLMF_ASSERT(false);
#endif

  public:
    // TYPES
    typedef Imp::ValueType32  ValueType32;
    typedef Imp::ValueType64  ValueType64;
    typedef Imp::ValueType128 ValueType128;

    // CLASS METHODS
    static ValueType64 makeDecimal64(                   int significand,
                                                        int exponent);
    static ValueType64 makeDecimal64(unsigned           int significand,
                                                        int exponent);
    static ValueType64 makeDecimal64(         long long int significand,
                                                        int exponent);
    static ValueType64 makeDecimal64(unsigned long long int significand,
                                                        int exponent);
        // Return a 'Decimal64' object that has the specified 'significand' and
        // 'exponent', rounded according to the current decimal rounding mode,
        // if necessary.  If an overflow condition occurs, store the value of
        // the macro 'ERANGE' into 'errno' and return infinity with the
        // appropriate sign.

    static ValueType64 makeInfinity64(bool isNegative = false);
        // Return a 'ValueType64' representing an infinity.  Optionally specify
        // whether the infinity 'isNegative'.  If 'isNegative' is 'false' or is
        // is not supplied, the returned value will be infinity, and negative
        // infinity otherwise.

#ifdef BDLDFP_DECIMALPLATFORM_SOFTWARE

                        // Literal Checking Functions

    struct This_is_not_a_floating_point_literal {};
        // This 'struct' is a helper type used togenerate error messages for
        // bad literals.

    template <class TYPE>
    static void checkLiteral(const TYPE& t);
        // Generate an error if the specified 't' is bad decimal
        // floating-point.  Note that this function is intended for use with
        // literals

    static void checkLiteral(double);
        // Overload to avoid an error when the decimal floating-point literal
        // (without the suffix) can be interpreted as a 'double' literal.

#elif defined(BDLDFP_DECIMALPLATFORM_HARDWARE)

#else

#error Improperly configured decimal floating point platform settings

#endif

                        // compose and decompose

    //static ValueType32  composeDecimal32 (DecimalTriple triple);
    //static ValueType64  composeDecimal64 (DecimalTriple triple);
    //static ValueType128 composeDecimal128(DecimalTriple triple);
        // Return a 'ValueTypeXX' number having the value as specified by the
        // salient attributes of the specified 'triple'.  The behavior is
        // undefined if the 'significand' has too many decimal digits for
        // 'ValueType', or the 'exponent' is too large for 'ValueType'

    //static DecimalTriple decomposeDecimal(ValueType32  value);
    //static DecimalTriple decomposeDecimal(ValueType64  value);
    //static DecimalTriple decomposeDecimal(ValueType128 value);
        // Return a 'DecimalTriple' object representing the salient attributes
        // of the specified 'value'.  The behavior is undefined, unless 'value'
        // is neither 'NaN' nor 'Inf'.

                        // Integer construction

    static ValueType32  int32ToDecimal32(                   int value);
    static ValueType32 uint32ToDecimal32(unsigned           int value);
    static ValueType32  int64ToDecimal32(         long long int value);
    static ValueType32 uint64ToDecimal32(unsigned long long int value);
        // Return a 'Decimal32' object having the value closest to the
        // specified 'value' following the conversion rules as defined by
        // IEEE-754:
        //
        //: o If 'value' is zero then initialize this object to a zero with an
        //:   unspecified sign and an unspecified exponent.
        //:
        //: o Otherwise if 'value' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal32>::max()' then raise the "overflow"
        //:   floating-point exception and initialize this object to infinity
        //:   with the same sign as 'other'.
        //:
        //: o Otherwise if 'value' has a value that is not exactly
        //:   representable using 'std::numeric_limits<Decimal32>::max_digit'
        //:   decimal digits then raise the "inexact" floating-point exception
        //:   and initialize this object to the value of 'other' rounded
        //:   according to the rounding direction.
        //:
        //: o Otherwise initialize this object to the value of the 'value'.
        //
        // The exponent 0 (quantum 1e-6) is preferred during conversion unless
        // it would cause unnecessary loss of precision.

    static ValueType64  int32ToDecimal64(                   int value);
    static ValueType64 uint32ToDecimal64(unsigned           int value);
    static ValueType64  int64ToDecimal64(         long long int value);
    static ValueType64 uint64ToDecimal64(unsigned long long int value);
        // Return a 'Decimal64' object having the value closest to the
        // specified 'value' following the conversion rules as defined by
        // IEEE-754:
        //
        //: o If 'value' is zero then initialize this object to a zero with an
        //:   unspecified sign and an unspecified exponent.
        //:
        //: o Otherwise if 'value' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
        //:   floating-point exception and initialize this object to infinity
        //:   with the same sign as 'other'.
        //:
        //: o Otherwise if 'value' has a value that is not exactly
        //:   representable using 'std::numeric_limits<Decimal64>::max_digit'
        //:   decimal digits then raise the "inexact" floating-point exception
        //:   and initialize this object to the value of 'other' rounded
        //:   according to the rounding direction.
        //:
        //: o Otherwise initialize this object to the value of the 'value'.
        //
        // The exponent 0 (quantum 1e-15) is preferred during conversion unless
        // it would cause unnecessary loss of precision.

    static ValueType128  int32ToDecimal128(                   int value);
    static ValueType128 uint32ToDecimal128(unsigned           int value);
    static ValueType128  int64ToDecimal128(         long long int value);
    static ValueType128 uint64ToDecimal128(unsigned long long int value);
        // Return a 'Decimal128' object having the value closest to the
        // specified 'value' subject to the conversion rules as defined by
        // IEEE-754:
        //
        //: o If 'value' is zero then initialize this object to a zero with an
        //:   unspecified sign and an unspecified exponent.
        //:
        //: o Otherwise if 'value' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal128>::max()' then raise the
        //:   "overflow" floating-point exception and initialize this object to
        //:   infinity with the same sign as 'other'.
        //:
        //: o Otherwise if 'value' has a value that is not exactly
        //:   representable using 'std::numeric_limits<Decimal128>::max_digit'
        //:   decimal digits then raise the "inexact" floating-point exception
        //:   and initialize this object to the value of 'value' rounded
        //:   according to the rounding direction.
        //:
        //: o Otherwise initialize this object to 'value'.
        //
        // The exponent 0 (quantum 1e-33) is preferred during conversion unless
        // it would cause unnecessary loss of precision.

                        // Arithmetic

                        // Addition functions

    static ValueType64  add(ValueType64  lhs, ValueType64  rhs);
    static ValueType128 add(ValueType128 lhs, ValueType128 rhs);
        // Add the value of the specified 'rhs' to the value of the specified
        // 'lhs' as described by IEEE-754 and return the result.
        //
        //: o If either of 'lhs' or 'rhs' is NaN, then raise the "invalid"
        //:   floating-point exception and return a NaN.
        //:
        //: o Otherwise if 'lhs' and 'rhs' are infinities of differing signs,
        //:   raise the "invalid" floating-point exception and return a NaN.
        //:
        //: o Otherwise if 'lhs' and 'rhs' are infinities of the same sign then
        //:   return infinity of that sign.
        //:
        //: o Otherwise if 'rhs' is zero (positive or negative), return 'lhs'.
        //:
        //: o Otherwise if the sum of 'lhs' and 'rhs' has an absolute value
        //:   that is larger than 'std::numeric_limits<Decimal64>::max()' then
        //:   raise the "overflow" floating-point exception and return infinity
        //:   with the same sign as that result.
        //:
        //: o Otherwise return the sum of the number represented by 'lhs' and
        //:   the number represented by 'rhs'.

                        // Subtraction functions

    static ValueType64  subtract(ValueType64  lhs, ValueType64  rhs);
    static ValueType128 subtract(ValueType128 lhs, ValueType128 rhs);
        // Subtract the value of the specified 'rhs' from the value of the
        // specified 'lhs' as described by IEEE-754 and return the result.
        //
        //: o If either 'lhs' or 'rhs' is NaN, then raise the "invalid"
        //:   floating-point exception and return a NaN.
        //:
        //: o Otherwise if 'lhs' and the 'rhs' have infinity values of the same
        //:   sign, then raise the "invalid" floating-point exception and
        //:   return a NaN.
        //:
        //: o Otherwise if 'lhs' and the 'rhs' have infinity values of
        //:   differing signs, then return 'lhs'.
        //:
        //: o Otherwise if 'rhs' has a zero value (positive or negative), then
        //:   return 'lhs'.
        //:
        //: o Otherwise if subtracting the value of the 'rhs' object from the
        //:   value of 'lhs' results in an absolute value that is larger than
        //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
        //:   floating-point exception and return infinity with the same sign
        //:   as that result.
        //:
        //: o Otherwise return the result of subtracting the value of 'rhs'
        //:   from the value of 'lhs'.

                        // Multiplication functions

    static ValueType64  multiply(ValueType64  lhs, ValueType64  rhs);
    static ValueType128 multiply(ValueType128 lhs, ValueType128 rhs);
        // Multiply the value of the specified 'lhs' object by the value of the
        // specified 'rhs' as described by IEEE-754 and return the result.
        //
        //: o If either of 'lhs' or 'rhs' is NaN, return a NaN.
        //:
        //: o Otherwise if one of the operands is infinity (positive or
        //:   negative) and the other is zero (positive or negative), then
        //:   raise the "invalid" floating-point exception raised and return a
        //:   NaN.
        //:
        //: o Otherwise if both 'lhs' and 'rhs' are infinity (positive or
        //:   negative), return infinity.  The sign of the returned value will
        //:   be positive if 'lhs' and 'rhs' have the same sign, and negative
        //:   otherwise.
        //:
        //: o Otherwise, if either 'lhs' or 'rhs' is zero, return zero.  The
        //:   sign of the returned value will be positive if 'lhs' and 'rhs'
        //:   have the same sign, and negative otherwise.
        //:
        //: o Otherwise if the product of 'lhs' and 'rhs' has an absolute value
        //:   that is larger than 'std::numeric_limits<Decimal64>::max()' then
        //:   raise the "overflow" floating-point exception and return infinity
        //:   with the same sign as that result.
        //:
        //: o Otherwise if the product of 'lhs' and 'rhs' has an absolute value
        //:   that is smaller than 'std::numeric_limits<Decimal64>::min()' then
        //:   raise the "underflow" floating-point exception and return zero
        //:   with the same sign as that result.
        //:
        //: o Otherwise return the product of the value of 'rhs' and the number
        //:   represented by 'rhs'.

                        // Division functions

    static ValueType64  divide(ValueType64  lhs, ValueType64  rhs);
    static ValueType128 divide(ValueType128 lhs, ValueType128 rhs);
        // Divide the value of the specified 'lhs' by the value of the
        // specified 'rhs' as described by IEEE-754, and return the result.
        //
        //: o If 'lhs' or 'rhs' is NaN, raise the "invalid" floating-point
        //:   exception and return a NaN.
        //:
        //: o Otherwise if 'lhs' and 'rhs' are both infinity (positive or
        //:   negative) or both zero (positive or negative), raise the
        //:   "invalid" floating-point exception and return a NaN.
        //:
        //: o Otherwise if 'rhs' has a positive zero value, raise the
        //:   "overflow" floating-point exception and return infinity with the
        //:   sign of 'lhs'.
        //:
        //: o Otherwise if 'rhs' has a negative zero value, raise the
        //:   "overflow" floating-point exception and return infinity with the
        //:   opposite sign as 'lhs'.
        //:
        //: o Otherwise if dividing the value of 'lhs' with the value of 'rhs'
        //:   results in an absolute value that is larger than
        //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
        //:   floating-point exception and return infinity with the same sign
        //:   as that result.
        //:
        //: o Otherwise if dividing the value of 'lhs' with the value of 'rhs'
        //:   results in an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal64>::min()' then raise the
        //:   "underflow" floating-point exception and return zero with the
        //:   same sign as that result.
        //:
        //: o Otherwise return the result of dividing the value of 'lhs' with
        //:   the value of 'rhs'.

                        // Negation functions

    static ValueType32  negate(ValueType32  value);
    static ValueType64  negate(ValueType64  value);
    static ValueType128 negate(ValueType128 value);
        // Return the result of applying the unary negation ('-') operator to
        // the specified 'value' as described by IEEE-754.  Note that decimal
        // floating point representations can encode signed zero values, thus
        // negating 0 results in -0 and negating -0 results in 0.

                        // Comparison functions

                        // Less Than functions

    static bool less(ValueType32  lhs, ValueType32  rhs);
    static bool less(ValueType64  lhs, ValueType64  rhs);
    static bool less(ValueType128 lhs, ValueType128 rhs);
        // Return 'true' if the specified 'lhs' has a value less than the
        // specified 'rhs' and 'false' otherwise.  The value of a 'Decimal64'
        // object 'lhs' is less than that of an object 'rhs' if the
        // 'compareQuietLess' operation (IEEE-754 defined, non-total ordering
        // comparison) considers the underlying IEEE representation of 'lhs' to
        // be less than of that of 'rhs'.  In other words, 'lhs' is less than
        // 'rhs' if:
        //
        //: o neither 'lhs' nor 'rhs' are NaN, or
        //: o 'lhs' is zero (positive or negative) and 'rhs' is positive, or
        //: o 'rhs' is zero (positive or negative) and 'lhs' negative, or
        //: o 'lhs' is not positive infinity, or
        //: o 'lhs' is negative infinity and 'rhs' is not, or
        //: o 'lhs' and 'rhs' both represent a real number and the real number
        //:   of 'lhs' is less than that of 'rhs'
        //
        // This operation raises the "invalid" floating-point exception if
        // either or both operands are NaN.

                        // Greater Than functions

    static bool greater(ValueType32  lhs, ValueType32  rhs);
    static bool greater(ValueType64  lhs, ValueType64  rhs);
    static bool greater(ValueType128 lhs, ValueType128 rhs);
        // Return 'true' if the specified 'lhs' has a greater value than the
        // specified 'rhs' and 'false' otherwise.  The value of a 'Decimal64'
        // object 'lhs' is greater than that of an object 'rhs' if the
        // 'compareQuietGreater' operation (IEEE-754 defined, non-total
        // ordering comparison) considers the underlying IEEE representation of
        // 'lhs' to be greater than of that of 'rhs'.  In other words, 'lhs' is
        // greater than 'rhs' if:
        //
        //: o neither 'lhs' nor 'rhs' are NaN, or
        //: o 'rhs' is zero (positive or negative) and 'lhs' positive, or
        //: o 'lhs' is zero (positive or negative) and 'rhs' negative, or
        //: o 'lhs' is not negative infinity, or
        //: o 'lhs' is positive infinity and 'rhs' is not, or
        //: o 'lhs' and 'rhs' both represent a real number and the real number
        //:   of 'lhs' is greater than that of 'rhs'
        //
        // This operation raises the "invalid" floating-point exception if
        // either or both operands are NaN.

                        // Less Or Equal functions

    static bool lessEqual(ValueType32  lhs, ValueType32  rhs);
    static bool lessEqual(ValueType64  lhs, ValueType64  rhs);
    static bool lessEqual(ValueType128 lhs, ValueType128 rhs);
        // Return 'true' if the specified 'lhs' has a value less than or equal
        // the value of the specified 'rhs' and 'false' otherwise.  The value
        // of a 'Decimal64' object 'lhs' is less than or equal to the value of
        // an object 'rhs' if the 'compareQuietLessEqual' operation (IEEE-754
        // defined, non-total ordering comparison) considers the underlying
        // IEEE representation of 'lhs' to be less or equal to that of 'rhs'.
        // In other words, 'lhs' is less or equal than 'rhs' if:
        //
        //: o neither 'lhs' nor 'rhs' are NaN, or
        //: o 'lhs' and 'rhs' are both zero (positive or negative), or
        //: o both 'lhs' and 'rhs' are positive infinity, or
        //: o 'lhs' is negative infinity, or
        //: o 'lhs' and 'rhs' both represent a real number and the real number
        //:   of 'lhs' is less or equal to that of 'rhs'
        //
        // This operation raises the "invalid" floating-point exception if
        // either or both operands are NaN.

                        // Greater Or Equal functions

    static bool greaterEqual(ValueType32  lhs, ValueType32  rhs);
    static bool greaterEqual(ValueType64  lhs, ValueType64  rhs);
    static bool greaterEqual(ValueType128 lhs, ValueType128 rhs);
        // Return 'true' if the specified 'lhs' has a value greater than or
        // equal to the value of the specified 'rhs' and 'false' otherwise.
        // The value of a 'Decimal64' object 'lhs' is greater or equal to a
        // 'Decimal64' object 'rhs' if the 'compareQuietGreaterEqual' operation
        // (IEEE-754 defined, non-total ordering comparison ) considers the
        // underlying IEEE representation of 'lhs' to be greater or equal to
        // that of 'rhs'.  In other words, 'lhs' is greater than or equal to
        // 'rhs' if:
        //
        //: o neither 'lhs' nor 'rhs' are NaN, or
        //: o 'lhs' and 'rhs' are both zero (positive or negative), or
        //: o both 'lhs' and 'rhs' are negative infinity, or
        //: o 'lhs' is positive infinity, or
        //: o 'lhs' and 'rhs' both represent a real number and the real number
        //:   of 'lhs' is greater or equal to that of 'rhs'
        //
        // This operation raises the "invalid" floating-point exception if
        // either or both operands are NaN.

                        // Equality functions

    static bool equal(ValueType32  lhs, ValueType32  rhs);
    static bool equal(ValueType64  lhs, ValueType64  rhs);
    static bool equal(ValueType128 lhs, ValueType128 rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' have the same value,
        // and 'false' otherwise.  Two decimal objects have the same value if
        // the 'compareQuietEqual' operation (IEEE-754 defined, non-total
        // ordering comparison) considers the underlying IEEE representations
        // equal.  In other words, two decimal objects have the same value if:
        //
        //: o both have a zero value (positive or negative), or
        //:
        //: o both have the same infinity value (both positive or negative), or
        //:
        //: o both have the value of a real number that are equal, even if they
        //:   are represented differently (cohorts have the same value)
        //
        // This operation raises the "invalid" floating-point exception if
        // either or both operands are NaN.

                        // Inequality functions

    static bool notEqual(ValueType32  lhs, ValueType32  rhs);
    static bool notEqual(ValueType64  lhs, ValueType64  rhs);
    static bool notEqual(ValueType128 lhs, ValueType128 rhs);
        // Return 'false' if the specified 'lhs' and 'rhs' have the same value,
        // and 'true' otherwise.  Two decimal objects have the same value if
        // the 'compareQuietEqual' operation (IEEE-754 defined, non-total
        // ordering comparison) considers the underlying IEEE representations
        // equal.  In other words, two decimal objects have the same value if:
        //
        //: o both have a zero value (positive or negative), or
        //:
        //: o both have the same infinity value (both positive or negative), or
        //:
        //: o both have the value of a real number that are equal, even if they
        //:   are represented differently (cohorts have the same value)
        //
        // This operation raises the "invalid" floating-point exception if
        // either or both operands are NaN.

                        // Inter-type Conversion functions

    static ValueType32  convertToDecimal32 (const ValueType64&  input);
    static ValueType64  convertToDecimal64 (const ValueType32&  input);
    static ValueType64  convertToDecimal64 (const ValueType128& input);
    static ValueType128 convertToDecimal128(const ValueType32&  input);
    static ValueType128 convertToDecimal128(const ValueType64&  input);
        // Convert the specified 'input' to the indicated result type.  Note
        // that a conversion from 'ValueType128' to 'ValueType32' is not
        // provided (because such a conversion is not provided by the
        // 'decNumber' library).  A conversion from 128-bit to 32-bit
        // representations is *not* identical to the composing the conversions
        // from 128-bit to 64-bit, and 64-bit to 32-bit representations,
        // because rounding should only be performed once.

                        // Binary floating point conversion functions

    static ValueType32  binaryToDecimal32(      float value);
    static ValueType32  binaryToDecimal32(     double value);
        // Create a 'Decimal32' object having the value closest to the
        // specified 'value' following the conversion rules as defined by
        // IEEE-754:
        //
        //: o If 'value' is NaN, return a NaN.
        //:
        //: o Otherwise if 'value' is infinity (positive or negative), then
        //:   return an object equal to infinity with the same sign.
        //:
        //: o Otherwise if 'value' is a zero value, then return an object equal
        //:   to zero with the same sign.
        //:
        //: o Otherwise if 'value' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal32>::max()' then raise the "overflow"
        //:   floating-point exception and return an infinity with the same
        //:   sign as 'value'.
        //:
        //: o Otherwise if 'value' has an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal32>::min()' then raise the
        //:   "underflow" floating-point exception and return a zero with the
        //:   same sign as 'value'.
        //:
        //: o Otherwise if 'value' needs more than
        //:   'std::numeric_limits<Decimal32>::max_digit' significant decimal
        //:   digits to represent then raise the "inexact" floating-point
        //:   exception and return the 'value' rounded according to the
        //:   rounding direction.
        //:
        //: o Otherwise return a 'Decimal32' object representing 'value'.

    static ValueType64  binaryToDecimal64(      float value);
    static ValueType64  binaryToDecimal64(     double value);
        // Create a 'Decimal64' object having the value closest to the
        // specified 'value' following the conversion rules as defined by
        // IEEE-754:
        //
        //: o If 'value' is NaN, return a NaN.
        //:
        //: o Otherwise if 'value' is infinity (positive or negative), then
        //:   return an object equal to infinity with the same sign.
        //:
        //: o Otherwise if 'value' is a zero value, then return an object equal
        //:   to zero with the same sign.
        //:
        //: o Otherwise if 'value' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
        //:   floating-point exception and return an infinity with the same
        //:   sign as 'value'.
        //:
        //: o Otherwise if 'value' has an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal64>::min()' then raise the
        //:   "underflow" floating-point exception and return a zero with the
        //:   same sign as 'value'.
        //:
        //: o Otherwise if 'value' needs more than
        //:   'std::numeric_limits<Decimal64>::max_digit' significant decimal
        //:   digits to represent then raise the "inexact" floating-point
        //:   exception and return the 'value' rounded according to the
        //:   rounding direction.
        //:
        //: o Otherwise return a 'Decimal64' object representing 'value'.

    static ValueType128 binaryToDecimal128(      float value);
    static ValueType128 binaryToDecimal128(     double value);
        // Create a 'Decimal128' object having the value closest to the
        // specified 'value' following the conversion rules as defined by
        // IEEE-754:
        //
        //: o If 'value' is NaN, return a NaN.
        //:
        //: o Otherwise if 'value' is infinity (positive or negative), then
        //:   return an object equal to infinity with the same sign.
        //:
        //: o Otherwise if 'value' is a zero value, then return an object equal
        //:   to zero with the same sign.
        //:
        //: o Otherwise if 'value' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal128>::max()' then raise the
        //:   "overflow" floating-point exception and return an infinity with
        //:   the same sign as 'value'.
        //:
        //: o Otherwise if 'value' has an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal128>::min()' then raise the
        //:   "underflow" floating-point exception and return a zero with the
        //:   same sign as 'value'.
        //:
        //: o Otherwise if 'value' needs more than
        //:   'std::numeric_limits<Decimal128>::max_digit' significant decimal
        //:   digits to represent then raise the "inexact" floating-point
        //:   exception and return the 'value' rounded according to the
        //:   rounding direction.
        //:
        //: o Otherwise return a 'Decimal128' object representing 'value'.

                        // makeDecimalRaw functions

    static ValueType32 makeDecimalRaw32(int significand, int exponent);
        // Create a 'ValueType32' object representing a decimal floating point
        // number consisting of the specified 'significand' and 'exponent',
        // with the sign given by 'significand'.  The behavior is undefined
        // unless 'abs(significand) <= 9,999,999' and '-101 <= exponent <= 90'.

    static ValueType64 makeDecimalRaw64(unsigned long long int significand,
                                                           int exponent);
    static ValueType64 makeDecimalRaw64(         long long int significand,
                                                           int exponent);
    static ValueType64 makeDecimalRaw64(unsigned           int significand,
                                                           int exponent);
    static ValueType64 makeDecimalRaw64(                   int significand,
                                                           int exponent);
        // Create a 'ValueType64' object representing a decimal floating point
        // number consisting of the specified 'significand' and 'exponent',
        // with the sign given by 'significand'.  The behavior is undefined
        // unless 'abs(significand) <= 9,999,999,999,999,999' and
        // '-398 <= exponent <= 369'.

    static ValueType128 makeDecimalRaw128(unsigned long long int significand,
                                                             int exponent);
    static ValueType128 makeDecimalRaw128(         long long int significand,
                                                             int exponent);
    static ValueType128 makeDecimalRaw128(unsigned           int significand,
                                                             int exponent);
    static ValueType128 makeDecimalRaw128(                   int significand,
                                                             int exponent);
        // Create a 'ValueType128' object representing a decimal floating point
        // number consisting of the specified 'significand' and 'exponent',
        // with the sign given by 'significand'.  The behavior is undefined
        // unless '-6176 <= exponent <= 6111'.

                        // ScaleB functions

    static ValueType32  scaleB(ValueType32  value, int exponent);
    static ValueType64  scaleB(ValueType64  value, int exponent);
    static ValueType128 scaleB(ValueType128 value, int exponent);
        // Return the result of multiplying the specified 'value' by ten raised
        // to the specified 'exponent'.  The quantum of 'value' is scaled
        // according to IEEE 754's 'scaleB' operations.  The result is
        // unspecified if 'value' is NaN or infinity.  The behavior is
        // undefined unless '-1999999997 <= y <= 99999999'.

                        // Parsing functions

    static ValueType32 parse32(const char *input);
        // Parse the specified 'input' string as a 32 bit decimal floating-
        // point value and return the result.  The parsing is as specified for
        // the 'strtod32' function in section 9.6 of the ISO/EIC TR 24732 C
        // Decimal Floating-Point Technical Report, except that it is
        // unspecified whether the NaNs returned are quiet or signaling.  The
        // behavior is undefined unless 'input' represents a valid 32 bit
        // decimal floating-point number in scientific or fixed notation, and
        // no unrelated characters precede (not even whitespace) that textual
        // representation and a terminating nul character immediately follows
        // it.  Note that this method does not guarantee the behavior of
        // ISO/EIC TR 24732 C when parsing NaN because the AIX compiler
        // intrinsics return a signaling NaN.

    static ValueType64 parse64(const char *input);
        // Parse the specified 'input' string as a 64 bit decimal floating-
        // point value and return the result.  The parsing is as specified for
        // the 'strtod64' function in section 9.6 of the ISO/EIC TR 24732 C
        // Decimal Floating-Point Technical Report, except that it is
        // unspecified whether the NaNs returned are quiet or signaling.  The
        // behavior is undefined unless 'input' represents a valid 64 bit
        // decimal floating-point number in scientific or fixed notation, and
        // no unrelated characters precede (not even whitespace) that textual
        // representation and a terminating nul character immediately follows
        // it.  Note that this method does not guarantee the behavior of
        // ISO/EIC TR 24732 C when parsing NaN because the AIX compiler
        // intrinsics return a signaling NaN.

    static ValueType128 parse128(const char *input);
        // Parse the specified 'input' string as a 128 bit decimal floating-
        // point value and return the result.  The parsing is as specified for
        // the 'strtod128' function in section 9.6 of the ISO/EIC TR 24732 C
        // Decimal Floating-Point Technical Report, except that it is
        // unspecified whether the NaNs returned are quiet or signaling.  The
        // behavior is undefined unless 'input' represents a valid 128 bit
        // decimal floating-point number in scientific or fixed notation, and
        // no unrelated characters precede (not even whitespace) that textual
        // representation and a terminating nul character immediately follows
        // it.  Note that this method does not guarantee the behavior of
        // ISO/EIC TR 24732 C when parsing NaN because the AIX compiler
        // intrinsics return a signaling NaN.

                        // Formatting functions

    static void format(ValueType32  value, char *buffer);
    static void format(ValueType64  value, char *buffer);
    static void format(ValueType128 value, char *buffer);
        // Produce a string representation of the specified decimal 'value', in
        // the specified 'buffer', which is at least
        // 'BDLDFP_DECIMALPLATFORM_SNPRINTF_BUFFER_SIZE' bytes in length.  The
        // string will be suitable for use with the 'strtod128' function in
        // section 9.6 of the ISO/EIC TR 24732 C Decimal Floating-Point
        // Technical Report, except that it is unspecified whether the NaNs
        // returned are quiet or signaling.  The behavior is undefined unless
        // there are 'size' bytes available in 'buffer'.

                        // Densely Packed Conversion Functions

    static ValueType32  convertFromDPD(
                              DenselyPackedDecimalImpUtil::StorageType32  dpd);
    static ValueType64  convertFromDPD(
                              DenselyPackedDecimalImpUtil::StorageType64  dpd);
    static ValueType128 convertFromDPD(
                              DenselyPackedDecimalImpUtil::StorageType128 dpd);
        // Return a 'ValueTypeXX' representing the specified 'dpd', which is
        // currently in Densely Packed Decimal (DPD) format.  This format is
        // compatible with the IBM compiler's native type, and the decNumber
        // library.

    static DenselyPackedDecimalImpUtil::StorageType32  convertToDPD(
                                                           ValueType32  value);
    static DenselyPackedDecimalImpUtil::StorageType64  convertToDPD(
                                                           ValueType64  value);
    static DenselyPackedDecimalImpUtil::StorageType128 convertToDPD(
                                                           ValueType128 value);
        // Return a 'DenselyPackeDecimalImpUtil::StorageTypeXX' representing
        // the specified 'value' in Densely Packed Decimal (DPD) format.  This
        // format is compatible with the IBM compiler's native type, and the
        // decNumber library.

                        // Binary Integral Conversion Functions

    static ValueType32  convertFromBID(
                             BinaryIntegralDecimalImpUtil::StorageType32  bid);
    static ValueType64  convertFromBID(
                             BinaryIntegralDecimalImpUtil::StorageType64  bid);
    static ValueType128 convertFromBID(
                             BinaryIntegralDecimalImpUtil::StorageType128 bid);
        // Return a 'ValueTypeXX' representing the specified 'bid', which is
        // currently in Binary Integral Decimal (BID) format.  This format is
        // compatible with the Intel DFP implementation type.

    static
    BinaryIntegralDecimalImpUtil::StorageType32  convertToBID(
                                                           ValueType32  value);
    static
    BinaryIntegralDecimalImpUtil::StorageType64  convertToBID(
                                                           ValueType64  value);
    static
    BinaryIntegralDecimalImpUtil::StorageType128 convertToBID(
                                                           ValueType128 value);
        // Return a 'BinaryIntegralDecimalImpUtil::StorageTypeXX' representing
        // the specified 'value' in Binary Integral Decimal (BID) format.  This
        // format is compatible with the Intel DFP implementation type.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                          // --------------------
                          // class DecimalImpUtil
                          // --------------------

#ifdef BDLDFP_DECIMALPLATFORM_SOFTWARE

template <class TYPE>
inline
void DecimalImpUtil::checkLiteral(const TYPE& t)
{
    (void)static_cast<This_is_not_a_floating_point_literal>(t);
}

inline
void DecimalImpUtil::checkLiteral(double)
{
}
#endif


// CLASS METHODS

                    // Integer construction

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::int32ToDecimal32(int value)
{
    return Imp::int32ToDecimal32(value);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::int32ToDecimal64(int value)
{
    return Imp::int32ToDecimal64(value);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::int32ToDecimal128(int value)
{
    return Imp::int32ToDecimal128(value);
}


inline
DecimalImpUtil::ValueType32
DecimalImpUtil::uint32ToDecimal32(unsigned int value)
{
    return Imp::uint32ToDecimal32(value);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::uint32ToDecimal64(unsigned int value)
{
    return Imp::uint32ToDecimal64(value);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::uint32ToDecimal128(unsigned int value)
{
    return Imp::uint32ToDecimal128(value);
}


inline
DecimalImpUtil::ValueType32
DecimalImpUtil::int64ToDecimal32(long long int value)
{
    return Imp::int64ToDecimal32(value);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::int64ToDecimal64(long long int value)
{
    return Imp::int64ToDecimal64(value);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::int64ToDecimal128(long long int value)
{
    return Imp::int64ToDecimal128(value);
}


inline
DecimalImpUtil::ValueType32
DecimalImpUtil::uint64ToDecimal32(unsigned long long int value)
{
    return Imp::uint64ToDecimal32(value);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::uint64ToDecimal64(unsigned long long int value)
{
    return Imp::uint64ToDecimal64(value);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::uint64ToDecimal128(unsigned long long int value)
{
    return Imp::uint64ToDecimal128(value);
}

                        // Arithmetic

                        // Addition Functions

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::add(DecimalImpUtil::ValueType64 lhs,
                    DecimalImpUtil::ValueType64 rhs)
{
    return Imp::add(lhs, rhs);
}

inline DecimalImpUtil::ValueType128
DecimalImpUtil::add(DecimalImpUtil::ValueType128 lhs,
                    DecimalImpUtil::ValueType128 rhs)
{
    return Imp::add(lhs, rhs);
}

                        // Subtraction Functions

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::subtract(DecimalImpUtil::ValueType64 lhs,
                         DecimalImpUtil::ValueType64 rhs)
{
    return Imp::subtract(lhs, rhs);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::subtract(DecimalImpUtil::ValueType128 lhs,
                         DecimalImpUtil::ValueType128 rhs)
{
    return Imp::subtract(lhs, rhs);
}

                        // Multiplication Functions

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::multiply(DecimalImpUtil::ValueType64 lhs,
                         DecimalImpUtil::ValueType64 rhs)
{
    return Imp::multiply(lhs, rhs);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::multiply(DecimalImpUtil::ValueType128 lhs,
                         DecimalImpUtil::ValueType128 rhs)
{
    return Imp::multiply(lhs, rhs);
}

                        // Division Functions

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::divide(DecimalImpUtil::ValueType64 lhs,
                       DecimalImpUtil::ValueType64 rhs)
{
    return Imp::divide(lhs, rhs);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::divide(DecimalImpUtil::ValueType128 lhs,
                       DecimalImpUtil::ValueType128 rhs)
{
    return Imp::divide(lhs, rhs);
}

                        // Negation Functions

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::negate(DecimalImpUtil::ValueType32 value)
{
    return Imp::negate(value);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::negate(DecimalImpUtil::ValueType64 value)
{
    return Imp::negate(value);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::negate(DecimalImpUtil::ValueType128 value)
{
    return Imp::negate(value);
}

                        // Comparison

                        // Less Than Functions

inline
bool
DecimalImpUtil::less(DecimalImpUtil::ValueType32 lhs,
                     DecimalImpUtil::ValueType32 rhs)
{
    return Imp::less(lhs, rhs);
}

inline
bool
DecimalImpUtil::less(DecimalImpUtil::ValueType64 lhs,
                     DecimalImpUtil::ValueType64 rhs)
{
    return Imp::less(lhs, rhs);
}

inline
bool
DecimalImpUtil::less(DecimalImpUtil::ValueType128 lhs,
                     DecimalImpUtil::ValueType128 rhs)
{
    return Imp::less(lhs, rhs);
}

                        // Greater Than Functions

inline
bool
DecimalImpUtil::greater(DecimalImpUtil::ValueType32 lhs,
                        DecimalImpUtil::ValueType32 rhs)
{
    return Imp::greater(lhs, rhs);
}

inline
bool
DecimalImpUtil::greater(DecimalImpUtil::ValueType64 lhs,
                        DecimalImpUtil::ValueType64 rhs)
{
    return Imp::greater(lhs, rhs);
}

inline
bool
DecimalImpUtil::greater(DecimalImpUtil::ValueType128 lhs,
                        DecimalImpUtil::ValueType128 rhs)
{
    return Imp::greater(lhs, rhs);
}

                        // Less Or Equal Functions

inline
bool
DecimalImpUtil::lessEqual(DecimalImpUtil::ValueType32 lhs,
                          DecimalImpUtil::ValueType32 rhs)
{
    return Imp::lessEqual(lhs, rhs);
}

inline
bool
DecimalImpUtil::lessEqual(DecimalImpUtil::ValueType64 lhs,
                          DecimalImpUtil::ValueType64 rhs)
{
    return Imp::lessEqual(lhs, rhs);
}

inline
bool
DecimalImpUtil::lessEqual(DecimalImpUtil::ValueType128 lhs,
                          DecimalImpUtil::ValueType128 rhs)
{
    return Imp::lessEqual(lhs, rhs);
}

                        // Greater Or Equal Functions

inline
bool
DecimalImpUtil::greaterEqual(DecimalImpUtil::ValueType32 lhs,
                             DecimalImpUtil::ValueType32 rhs)
{
    return Imp::greaterEqual(lhs, rhs);
}

inline
bool
DecimalImpUtil::greaterEqual(DecimalImpUtil::ValueType64 lhs,
                             DecimalImpUtil::ValueType64 rhs)
{
    return Imp::greaterEqual(lhs, rhs);
}

inline
bool
DecimalImpUtil::greaterEqual(DecimalImpUtil::ValueType128 lhs,
                             DecimalImpUtil::ValueType128 rhs)
{
    return Imp::greaterEqual(lhs, rhs);
}

                        // Equality Functions

inline
bool
DecimalImpUtil::equal(DecimalImpUtil::ValueType32 lhs,
                      DecimalImpUtil::ValueType32 rhs)
{
    return Imp::equal(lhs, rhs);
}

inline
bool
DecimalImpUtil::equal(DecimalImpUtil::ValueType64 lhs,
                      DecimalImpUtil::ValueType64 rhs)
{
    return Imp::equal(lhs, rhs);
}

inline
bool
DecimalImpUtil::equal(DecimalImpUtil::ValueType128 lhs,
                      DecimalImpUtil::ValueType128 rhs)
{
    return Imp::equal(lhs, rhs);
}

                        // Inequality Functions

inline
bool
DecimalImpUtil::notEqual(DecimalImpUtil::ValueType32 lhs,
                         DecimalImpUtil::ValueType32 rhs)
{
    return Imp::notEqual(lhs, rhs);
}

inline
bool
DecimalImpUtil::notEqual(DecimalImpUtil::ValueType64 lhs,
                         DecimalImpUtil::ValueType64 rhs)
{
    return Imp::notEqual(lhs, rhs);
}

inline
bool
DecimalImpUtil::notEqual(DecimalImpUtil::ValueType128 lhs,
                         DecimalImpUtil::ValueType128 rhs)
{
    return Imp::notEqual(lhs, rhs);
}

                        // Inter-type Conversion functions

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::convertToDecimal32(const DecimalImpUtil::ValueType64& input)
{
    return Imp::convertToDecimal32(input);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::convertToDecimal64 (const DecimalImpUtil::ValueType32& input)
{
    return Imp::convertToDecimal64(input);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::convertToDecimal64(const DecimalImpUtil::ValueType128& input)
{
    return Imp::convertToDecimal64(input);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::convertToDecimal128(const DecimalImpUtil::ValueType32& input)
{
    return Imp::convertToDecimal128(input);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::convertToDecimal128(const DecimalImpUtil::ValueType64& input)
{
    return Imp::convertToDecimal128(input);
}

                        // Binary floating point conversion functions

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::binaryToDecimal32(float value)
{
    return Imp::binaryToDecimal32(value);
}

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::binaryToDecimal32(double value)
{
    return Imp::binaryToDecimal32(value);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::binaryToDecimal64(float value)
{
    return Imp::binaryToDecimal64(value);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::binaryToDecimal64(double value)
{
    return Imp::binaryToDecimal64(value);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::binaryToDecimal128(float value)
{
    return Imp::binaryToDecimal128(value);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::binaryToDecimal128(double value)
{
    return Imp::binaryToDecimal128(value);
}

                        // makeDecimalRaw Functions

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::makeDecimalRaw32(int significand, int exponent)
{
    BSLS_ASSERT(-101 <= exponent);
    BSLS_ASSERT(        exponent <= 90);
    BSLS_ASSERT(bsl::max(significand, -significand) <= 9999999);
    return Imp::makeDecimalRaw32(significand, exponent);
}


inline
DecimalImpUtil::ValueType64
DecimalImpUtil::makeDecimalRaw64(unsigned long long significand, int exponent)
{
    BSLS_ASSERT(-398 <= exponent);
    BSLS_ASSERT(        exponent <= 369);
    BSLS_ASSERT(significand <= 9999999999999999LL);

    return Imp::makeDecimalRaw64(significand, exponent);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::makeDecimalRaw64(long long significand, int exponent)
{
    BSLS_ASSERT(-398 <= exponent);
    BSLS_ASSERT(        exponent <= 369);
    BSLS_ASSERT(std::max(significand, -significand) <= 9999999999999999LL);

    return Imp::makeDecimalRaw64(significand, exponent);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::makeDecimalRaw64(unsigned int significand, int exponent)
{
    BSLS_ASSERT(-398 <= exponent);
    BSLS_ASSERT(        exponent <= 369);

    return Imp::makeDecimalRaw64(significand, exponent);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::makeDecimalRaw64(int significand, int exponent)
{
    BSLS_ASSERT(-398 <= exponent);
    BSLS_ASSERT(        exponent <= 369);
    return Imp::makeDecimalRaw64(significand, exponent);
}


inline
DecimalImpUtil::ValueType128
DecimalImpUtil::makeDecimalRaw128(unsigned long long significand, int exponent)
{
    BSLS_ASSERT(-6176 <= exponent);
    BSLS_ASSERT(         exponent <= 6111);

    return Imp::makeDecimalRaw128(significand, exponent);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::makeDecimalRaw128(long long significand, int exponent)
{
    BSLS_ASSERT(-6176 <= exponent);
    BSLS_ASSERT(         exponent <= 6111);

    return Imp::makeDecimalRaw128(significand, exponent);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::makeDecimalRaw128(unsigned int significand, int exponent)
{
    BSLS_ASSERT(-6176 <= exponent);
    BSLS_ASSERT(         exponent <= 6111);

    return Imp::makeDecimalRaw128(significand, exponent);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::makeDecimalRaw128(int significand, int exponent)
{
    BSLS_ASSERT(-6176 <= exponent);
    BSLS_ASSERT(         exponent <= 6111);

    return Imp::makeDecimalRaw128(significand, exponent);
}

                        // IEEE Scale B Functions

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::scaleB(DecimalImpUtil::ValueType32 value, int exponent)
{
    return Imp::scaleB(value, exponent);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::scaleB(DecimalImpUtil::ValueType64 value, int exponent)
{
    return Imp::scaleB(value, exponent);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::scaleB(DecimalImpUtil::ValueType128 value, int exponent)
{
    return Imp::scaleB(value, exponent);
}

                        // Parsing functions

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::parse32(const char *input)
{
    return Imp::parse32(input);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::parse64(const char *input)
{
    return Imp::parse64(input);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::parse128(const char *input)
{
    return Imp::parse128(input);
}

                        // Format functions

inline
void DecimalImpUtil::format(DecimalImpUtil::ValueType32 value, char *buffer)
{
    return Imp::format(value, buffer);
}

inline
void DecimalImpUtil::format(DecimalImpUtil::ValueType64 value, char *buffer)
{
    return Imp::format(value, buffer);
}

inline
void DecimalImpUtil::format(DecimalImpUtil::ValueType128 value, char *buffer)
{
    return Imp::format(value, buffer);
}

                        // Densely Packed Conversion Functions

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::convertFromDPD(
                                DenselyPackedDecimalImpUtil::StorageType32 dpd)
{
    return Imp::convertFromDPD(dpd);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::convertFromDPD(
                                DenselyPackedDecimalImpUtil::StorageType64 dpd)
{
    return Imp::convertFromDPD(dpd);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::convertFromDPD(
                               DenselyPackedDecimalImpUtil::StorageType128 dpd)
{
    return Imp::convertFromDPD(dpd);
}

inline
DenselyPackedDecimalImpUtil::StorageType32
DecimalImpUtil::convertToDPD(ValueType32 value)
{
    return Imp::convertToDPD(value);
}

inline
DenselyPackedDecimalImpUtil::StorageType64
DecimalImpUtil::convertToDPD(ValueType64 value)
{
    return Imp::convertToDPD(value);
}

inline
DenselyPackedDecimalImpUtil::StorageType128
DecimalImpUtil::convertToDPD(ValueType128 value)
{
    return Imp::convertToDPD(value);
}

                        // Binary Integral Conversion Functions

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::convertFromBID(
                               BinaryIntegralDecimalImpUtil::StorageType32 bid)
{
    return Imp::convertFromBID(bid);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::convertFromBID(
                               BinaryIntegralDecimalImpUtil::StorageType64 bid)
{
    return Imp::convertFromBID(bid);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::convertFromBID(
                              BinaryIntegralDecimalImpUtil::StorageType128 bid)
{
    return Imp::convertFromBID(bid);
}

inline
BinaryIntegralDecimalImpUtil::StorageType32
DecimalImpUtil::convertToBID(ValueType32 value)
{
    return Imp::convertToBID(value);
}

inline
BinaryIntegralDecimalImpUtil::StorageType64
DecimalImpUtil::convertToBID(ValueType64 value)
{
    return Imp::convertToBID(value);
}

inline
BinaryIntegralDecimalImpUtil::StorageType128
DecimalImpUtil::convertToBID(ValueType128 value)
{
    return Imp::convertToBID(value);
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
