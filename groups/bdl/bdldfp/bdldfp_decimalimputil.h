// bdldfp_decimalimputil.h                                            -*-C++-*-

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL
#define INCLUDED_BDLDFP_DECIMALIMPUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id$")

//@PURPOSE: Provide a unified low-level interface for decimal floating point.
//
//@CLASSES:
//  bdldfp::DecimalImpUtil: Unified low-level decimal floating point functions.
//
//@SEE_ALSO: bdldfp_decimalimputil_inteldfp
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
// following example we use such a 'coefficient' and 'exponent' to create
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
// First, we define the signature of a function that computes the sum of an
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

#include <bdlscm_version.h>

#include <bdldfp_decimalformatconfig.h>
#include <bdldfp_decimalimputil_inteldfp.h>
#include <bdldfp_decimalplatform.h>
#include <bdldfp_decimalstorage.h>
#include <bdldfp_intelimpwrapper.h>
#include <bdldfp_uint128.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cmath.h>
#include <bsl_c_errno.h>
#include <bsl_iostream.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsl_c_signal.h>  // Formerly transitively included via decContext.h
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

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
#if defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    typedef DecimalImpUtil_IntelDfp  Imp;
#else
    BDLDFP_DECIMALPLATFORM_COMPILER_ERROR;
#endif

  public:
    // TYPES
    typedef Imp::ValueType32  ValueType32;
    typedef Imp::ValueType64  ValueType64;
    typedef Imp::ValueType128 ValueType128;

    enum {
        // Status flag bitmask for numeric operations.

        k_STATUS_INEXACT = Imp::k_STATUS_INEXACT,
        k_STATUS_UNDERFLOW = Imp::k_STATUS_UNDERFLOW,
        k_STATUS_OVERFLOW = Imp::k_STATUS_OVERFLOW
    };

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
        // Return a 'ValueType64' representing infinity.  Optionally specify
        // whether the infinity 'isNegative'.  If 'isNegative' is 'false' or is
        // is not supplied, the returned value will be infinity, and negative
        // infinity otherwise.

#ifdef BDLDFP_DECIMALPLATFORM_SOFTWARE

                        // Literal Checking Functions

    struct This_is_not_a_floating_point_literal {};
        // This 'struct' is a helper type used to generate error messages for
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
                            // classify

    static int classify(ValueType32  x);
    static int classify(ValueType64  x);
    static int classify(ValueType128 x);
        // Return the integer value that respresents the floating point
        // classification of the specified 'x' value as follows:
        //
        //: o if 'x' is NaN, return FP_NAN;
        //: o otherwise if 'x' is positive or negative infinity, return
        //:   'FP_INFINITE';
        //: o otherwise if 'x' is a subnormal value, return 'FP_SUBNORMAL'
        //: o otherwise if 'x' is a zero value, return 'FP_ZERO'
        //: o otherwise return 'FP_NORMAL'
        //
        // Note that the mention 'FP_XXX' constants are C99 standard macros and
        // they are defined in the math.h (cmath) standard header.  On systems
        // that fail to define those standard macros we define the in this
        // component as public macros.


                          // normalize

    static ValueType32  normalize(ValueType32  original);
    static ValueType64  normalize(ValueType64  original);
    static ValueType128 normalize(ValueType128 original);
        // Return a 'ValueTypeXX' number having the value as the specified
        // 'original, but with the significand, that can not be divided by ten,
        // and appropriate exponent.
        //
        //: o Any representations of zero value (either positive or negative)
        //:   are normalized to positive zero having null significand and
        //:   exponent.
        //:
        //: o Any NaN values (either signaling or quiet) are normalized to
        //:   quiet NaN.
        //:
        //: o Normalized non-zero value has the same sign as the original one.

                             // Quantum functions

    static ValueType32  quantize(ValueType32  value, ValueType32  exponent);
    static ValueType64  quantize(ValueType64  value, ValueType64  exponent);
    static ValueType128 quantize(ValueType128 value, ValueType128 exponent);
        // Return a number equal to the specified 'value' (except for possible
        // rounding) having the exponent equal to the exponent of the specified
        // 'exponent'.  Rounding may occur when the exponent is greater than
        // the quantum of 'value'.  E.g., 'quantize(147e-2_d32, 1e-1_d32)'
        // yields '15e-1_d32'.  In the opposite direction, if 'exponent' is
        // sufficiently less than the quantum of 'value', it may not be
        // possible to construct the requested result, and if so, 'NaN' is
        // returned.  E.g., 'quantize(1234567e0_d32, 1e-1_d32)' returns 'NaN'.

    static ValueType32  quantize(ValueType32  value, int exponent);
    static ValueType64  quantize(ValueType64  value, int exponent);
    static ValueType128 quantize(ValueType128 value, int exponent);
        // Return a number equal to the specified 'value' (except for possible
        // rounding) having the specified 'exponent'.  Rounding may occur when
        // 'exponent' is greater than the quantum of 'value'.  E.g.,
        // 'quantize(147e-2_d32, -1)' yields '15e-1_d32'.  In the opposite
        // direction, if 'exponent' is sufficiently less than the quantum of
        // 'value', it may not be possible to construct the requested result,
        // and if so, 'NaN' is returned.  E.g., 'quantize(1234567e0_d32, -1)'
        // returns 'NaN'.  Behavior is undefined unless the 'exponent'
        // satisfies the following conditions
        //: o for 'Decimal32'  type:  '-101 <= exponent <=   90'
        //: o for 'Decimal64'  type:  '-398 <= exponent <=  369'
        //: o for 'Decimal128' type: '-6176 <= exponent <= 6111'

    static int quantizeEqual(ValueType32  *x, ValueType32  y, int exponent);
    static int quantizeEqual(ValueType64  *x, ValueType64  y, int exponent);
    static int quantizeEqual(ValueType128 *x, ValueType128 y, int exponent);
        // If a floating-point number equal to the specified 'y' and having the
        // specified 'exponent' can be constructed, set that value into the
        // specified 'x' and return 0.  Otherwise, or if 'y' is NaN or
        // infinity, leave the contents of 'x' unchanged and return a non-zero
        // value.  The behavior is undefined unless 'exponent' satisfies the
        // following conditions
        //: o for 'Decimal32'  type:  '-101 <= exponent <=   90'
        //: o for 'Decimal64'  type:  '-398 <= exponent <=  369'
        //: o for 'Decimal128' type: '-6176 <= exponent <= 6111'
        //
        // Example:
        //     'Decimal32 x;'
        //     'BSLS_ASSERT(0 == quantizeEqual(&x, 123e+3_d32, 2);'
        //     'BSLS_ASSERT(1230e+2_d32 == x);'
        //     'BSLS_ASSERT(0 != quantizeEqual(&x, 123e+3_d32, -2);'
        //     'BSLS_ASSERT(1230e+2_d32 == x);'

    static bool sameQuantum(ValueType32  x, ValueType32  y);
    static bool sameQuantum(ValueType64  x, ValueType64  y);
    static bool sameQuantum(ValueType128 x, ValueType128 y);
        // Return 'true' if the specified 'x' and 'y' values have the same
        // quantum exponents, and 'false' otherwise.  If both arguments are NaN
        // or both arguments are infinity, they have the same quantum
        // exponents.  Note that if exactly one operand is NaN or exactly one
        // operand is infinity, they do not have the same quantum exponents.

                        // compose and decompose

    //static ValueType32  composeDecimal32 (DecimalTriple triple);
    //static ValueType64  composeDecimal64 (DecimalTriple triple);
    //static ValueType128 composeDecimal128(DecimalTriple triple);
        // Return a 'ValueTypeXX' number having the value as specified by the
        // salient attributes of the specified 'triple'.  The behavior is
        // undefined if the 'significand' has too many decimal digits for
        // 'ValueType', or the 'exponent' is too large for 'ValueType'

    static int decompose(int                 *sign,
                         unsigned  int       *significand,
                         int                 *exponent,
                         ValueType32          value);
    static int decompose(int                 *sign,
                         bsls::Types::Uint64 *significand,
                         int                 *exponent,
                         ValueType64          value);
    static int decompose(int                 *sign,
                         Uint128             *significand,
                         int                 *exponent,
                         ValueType128         value);
        // Decompose the specified decimal 'value' into the components of
        // the decimal floating-point format and load the result into the
        // specified 'sign', 'significand' and 'exponent' such that
        // 'value' is equal to 'sign * significand * (10 ** exponent)'.
        // The special values infinity and NaNs are decomposed to 'sign',
        // 'exponent' and 'significand' parts, even though they don't have
        // their normal meaning (except 'sign').  That is those specific values
        // cannot be restored using these parts, unlike the finite ones.
        // Return the integer value that represents the floating point
        // classification of the specified 'value' as follows:
        //
        //: o if 'value' is NaN, return FP_NAN;
        //: o if 'value' is infinity, return 'FP_INFINITE';
        //: o if 'value' is a subnormal value, return 'FP_SUBNORMAL';
        //: o if 'value' is a zero value, return 'FP_ZERO';
        //: o otherwise return 'FP_NORMAL'.
        //
        // Note that a decomposed representation may not be unique,
        // for example 10 can be represented as either '10 * (10 ** 0)'
        // or '1 * (10 ** 1)'.  The returned 'significand' and 'exponent'
        // reflect the encoded representation of 'value' (i.e., they
        // reflect the 'quantum' of 'value').

                         // Format functions

    static int format(char                      *buffer,
                      int                        length,
                      ValueType32                value,
                      const DecimalFormatConfig& cfg);

    static int format(char                      *buffer,
                      int                        length,
                      ValueType64                value,
                      const DecimalFormatConfig& cfg);

    static int format(char                      *buffer,
                      int                        length,
                      ValueType128               value,
                      const DecimalFormatConfig& cfg);
        // Format the specified 'value', according to the parameters in the
        // specified 'cfg'. Place the output in the buffer designated by the
        // specified 'buffer' and 'length', and return the length of the
        // formatted value.  If there is insufficient room in the buffer, its
        // contents will be left in an unspecified state, with the returned
        // value indicating the necessary size.  This function does not write
        // a terminating null character.  If 'length' is not positive, 'buffer'
        // is permitted to be null.  This can be used to determine the
        // necessary buffer size.  See the Attributes section under
        // @DESCRIPTION in the component-level documentation for
        // 'bdldfp::DecimalFormatConfig' component for information on the
        // configuration attributes.
        //
        // Note that for some combinations of 'value' and precision provided by
        // 'cfg' object, the number being written must first be rounded to
        // fewer digits than it initially contains.  The number written must be
        // as close as possible to the initial value given the constraints on
        // precision.  The rounding should be done as "round-half-up", i.e.,
        // round up in magnitude when the first of the discarded digits is
        // between 5 and 9.
        //
        // Also note that if the configuration format attribute 'style' is
        // 'e_NATURAL' then all significand digits of the 'value' are output in
        // the buffer regardless of the value specified in configuration's
        // 'precision' attribute.

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
        //: o Otherwise if 'value' has a value that is not exactly
        //:   representable using 'std::numeric_limits<Decimal32>::max_digit'
        //:   decimal digits then return a decimal value initialized to the
        //:   value of 'value' rounded according to the rounding direction.
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
        //: o Otherwise if 'value' has a value that is not exactly
        //:   representable using 'std::numeric_limits<Decimal64>::max_digit'
        //:   decimal digits then return a decimal value initialized to the
        //:   value of 'value' rounded according to the rounding direction.
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
        //: o Otherwise if 'value' has a value that is not exactly
        //:   representable using 'std::numeric_limits<Decimal128>::max_digit'
        //:   decimal digits then return a decimal value initialized to the
        //:   value of 'value' rounded according to the rounding direction.
        //:
        //: o Otherwise initialize this object to 'value'.
        //
        // The exponent 0 (quantum 1e-33) is preferred during conversion unless
        // it would cause unnecessary loss of precision.

                        // Arithmetic

                        // Addition functions

    static ValueType32  add(ValueType32  lhs, ValueType32  rhs);
    static ValueType64  add(ValueType64  lhs, ValueType64  rhs);
    static ValueType128 add(ValueType128 lhs, ValueType128 rhs);
        // Add the value of the specified 'rhs' to the value of the specified
        // 'lhs' as described by IEEE-754 and return the result.
        //
        //: o If either of 'lhs' or 'rhs' is signaling NaN, then store the
        //:   value of the macro 'EDOM' into 'errno' and return a NaN.
        //:
        //: o Otherwise if either of 'lhs' or 'rhs' is NaN, return a NaN.
        //:
        //: o Otherwise if 'lhs' and 'rhs' are infinities of differing signs,
        //:   store the value of the macro 'EDOM' into 'errno' and return a
        //:   NaN.
        //:
        //: o Otherwise if 'lhs' and 'rhs' are infinities of the same sign then
        //:   return infinity of that sign.
        //:
        //: o Otherwise if 'rhs' is zero (positive or negative), return 'lhs'.
        //:
        //: o Otherwise if the sum of 'lhs' and 'rhs' has an absolute value
        //:   that is larger than the maximum value supported by the indicated
        //:   result type then store the value of the macro 'ERANGE' into
        //:   'errno' and return infinity with the same sign as that result.
        //:
        //: o Otherwise return the sum of the number represented by 'lhs' and
        //:   the number represented by 'rhs'.

                        // Subtraction functions

    static ValueType32  subtract(ValueType32  lhs, ValueType32  rhs);
    static ValueType64  subtract(ValueType64  lhs, ValueType64  rhs);
    static ValueType128 subtract(ValueType128 lhs, ValueType128 rhs);
        // Subtract the value of the specified 'rhs' from the value of the
        // specified 'lhs' as described by IEEE-754 and return the result.
        //
        //: o If either of 'lhs' or 'rhs' is signaling NaN, then store the
        //:   value of the macro 'EDOM' into 'errno' and return a NaN.
        //:
        //: o Otherwise if either of 'lhs' or 'rhs' is NaN, return a NaN.
        //:
        //: o Otherwise if 'lhs' and the 'rhs' have infinity values of the same
        //:   sign, store the value of the macro 'EDOM' into 'errno' and return
        //:   a NaN.
        //:
        //: o Otherwise if 'lhs' and the 'rhs' have infinity values of
        //:   differing signs, then return 'lhs'.
        //:
        //: o Otherwise if 'rhs' has a zero value (positive or negative), then
        //:   return 'lhs'.
        //:
        //: o Otherwise if the subtracting of 'lhs' and 'rhs' has an absolute
        //:   value that is larger than the maximum value supported by the
        //:   indicated result type then store the value of the macro 'ERANGE'
        //:   into 'errno' and return infinity with the same sign as that
        //:   result.
        //:
        //: o Otherwise return the result of subtracting the value of 'rhs'
        //:   from the value of 'lhs'.

                        // Multiplication functions

    static ValueType32  multiply(ValueType32  lhs, ValueType32  rhs);
    static ValueType64  multiply(ValueType64  lhs, ValueType64  rhs);
    static ValueType128 multiply(ValueType128 lhs, ValueType128 rhs);
        // Multiply the value of the specified 'lhs' object by the value of the
        // specified 'rhs' as described by IEEE-754 and return the result.
        //
        //: o If either of 'lhs' or 'rhs' is signaling NaN, then store the
        //:   value of the macro 'EDOM' into 'errno' and return a NaN.
        //:
        //: o Otherwise if either of 'lhs' or 'rhs' is NaN, return a NaN.
        //:
        //: o Otherwise if one of the operands is infinity (positive or
        //:   negative) and the other is zero (positive or negative), then
        //:   store the value of the macro 'EDOM' into 'errno' and return a
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
        //:   that is larger than the maximum value of the indicated result
        //:   type then store the value of the macro 'ERANGE' into 'errno' and
        //:   return infinity with the same sign as that result.
        //:
        //: o Otherwise if the product of 'lhs' and 'rhs' has an absolute value
        //:   that is smaller than min value of the indicated result type then
        //:   store the value of the macro 'ERANGE' into 'errno' and return
        //:   zero with the same sign as that result.
        //:
        //: o Otherwise return the product of the value of 'rhs' and the number
        //:   represented by 'rhs'.

                        // Division functions

    static ValueType32  divide(ValueType32  lhs, ValueType32  rhs);
    static ValueType64  divide(ValueType64  lhs, ValueType64  rhs);
    static ValueType128 divide(ValueType128 lhs, ValueType128 rhs);
        // Divide the value of the specified 'lhs' by the value of the
        // specified 'rhs' as described by IEEE-754, and return the result.
        //
        //: o If either of 'lhs' or 'rhs' is signaling NaN, then store the
        //:   value of the macro 'EDOM' into 'errno' and return a NaN.
        //:
        //: o Otherwise if either of 'lhs' or 'rhs' is NaN, return a NaN.
        //:
        //: o Otherwise if 'lhs' and 'rhs' are both infinity (positive or
        //:   negative) or both zero (positive or negative) then store the
        //:   value of the macro 'EDOM' into 'errno' and return a NaN.
        //:
        //: o Otherwise if 'lhs' has a normal value and 'rhs' has a positive
        //:   zero value, store the value of the macro 'ERANGE' into 'errno'
        //:   and return infinity with the sign of 'lhs'.
        //:
        //: o Otherwise if 'lhs' has a normal value and 'rhs' has a negative
        //:   zero value, store the value of the macro 'ERANGE' into 'errno'
        //:   and return infinity with the opposite sign as 'lhs'.
        //:
        //: o Otherwise if dividing the value of 'lhs' by the value of 'rhs'
        //:   results in an absolute value that is larger than the maximum
        //:   value supported by the result type then store the value of the
        //:   macro 'ERANGE' into 'errno' and return infinity with the same
        //:   sign as that result.
        //:
        //: o Otherwise if dividing the value of 'lhs' by the value of 'rhs'
        //:   results in an absolute value that is smaller than min value
        //:   supported by the indicated result type then store the value of
        //:   the macro 'ERANGE' into 'errno'and return zero with the same sign
        //:   as that result.
        //:
        //: o Otherwise return the result of dividing the value of 'lhs' by the
        //:   value of 'rhs'.

                        // Math functions

    static ValueType32  copySign(ValueType32  x, ValueType32  y);
    static ValueType64  copySign(ValueType64  x, ValueType64  y);
    static ValueType128 copySign(ValueType128 x, ValueType128 y);
        // Return a decimal value with the magnitude of the specifed 'x' and
        // the sign of the specified 'y'.  If 'x' is NaN, then NaN with the
        // sign of 'y' is returned.
        //
        // Examples: 'copysign( 5.0, -2.0)' ==> -5.0;
        //           'copysign(-5.0, -2.0)' ==> -5.0

    static ValueType32  exp(ValueType32  x);
    static ValueType64  exp(ValueType64  x);
    static ValueType128 exp(ValueType128 x);
        // Return 'e' (Euler's number, 2.7182818) raised to the specified power
        // 'x'.
        //
        // Special value handling:
        //: o If 'x' is +/-0, 1 is returned.
        //: o If 'x' is negative infinity, +0 is returned.
        //: o If 'x' is +infinity, +infinity is returned.
        //: o If 'x' is quiet NaN, quiet NaN is returned.
        //: o If 'x' is signaling NaN, quiet NaN is returned and the value of
        //:   the macro 'EDOM' is stored into 'errno'.
        //: o If 'x' is finite, but the result value is outside the range of
        //:   the return type, store the value of the macro 'ERANGE' into
        //:   'errno' and +infinity value is returned.

    static ValueType32  log(ValueType32  x);
    static ValueType64  log(ValueType64  x);
    static ValueType128 log(ValueType128 x);
        // Return the natural (base 'e') logarithm of the specified 'x'.
        //
        // Special value handling:
        //: o If 'x' is +/-0, -infinity is returned and the value of the macro
        //:   'ERANGE' is stored into 'errno'.
        //: o If 'x' is 1, +0 is returned.
        //: o If 'x' is negative, quiet NaN is returned and the value of the
        //:   macro 'EDOM' is stored into 'errno'.
        //: o If 'x' is +infinity, +infinity is returned.
        //: o If 'x' is quiet NaN, quiet NaN is returned.
        //: o If 'x' is signaling NaN, quiet NaN is returned and the value of
        //:   the macro 'EDOM' is stored into 'errno'.

    static ValueType32  logB(ValueType32  x);
    static ValueType64  logB(ValueType64  x);
    static ValueType128 logB(ValueType128 x);
        // Return the FLT_RADIX-based logarithm (i.e., base 10) of the absolute
        //  value of the specified 'x'.
        //
        // Special value handling:
        //: o If 'x' is +/-0, -infinity is returned and the value of the macro
        //:   'ERANGE' is stored into 'errno'.
        //: o If 'x' is 1, +0 is returned.
        //: o If 'x' is +/-infinity, +infinity is returned.
        //: o If 'x' is quiet NaN, quiet NaN is returned.
        //: o If 'x' is signaling NaN, quiet NaN is returned and the value of
        //:   the macro 'EDOM' is stored into 'errno'.
        //
        // Examples: 'logB(  10.0)' ==> 1.0;
        //           'logB(-100.0)' ==> 2.0

    static ValueType32  log10(ValueType32  x);
    static ValueType64  log10(ValueType64  x);
    static ValueType128 log10(ValueType128 x);
        // Return the common (base-10) logarithm of the specified 'x'.
        //
        // Special value handling:
        //: o If 'x' is +/-0, -infinity is returned and the value of the macro
        //:   'ERANGE' is stored into 'errno'.
        //: o If 'x' is 1, +0 is returned.
        //: o If 'x' is negative, quiet NaN is returned and the value of the
        //:   macro 'EDOM' is stored into 'errno'.
        //: o If 'x' is +infinity, +infinity is returned.
        //: o If 'x' is quiet NaN, quiet NaN is returned.
        //: o If 'x' is signaling NaN, quiet NaN is returned and the value of
        //:   the macro 'EDOM' is stored into 'errno'.

    static ValueType32  fmod(ValueType32  x, ValueType32  y);
    static ValueType64  fmod(ValueType64  x, ValueType64  y);
    static ValueType128 fmod(ValueType128 x, ValueType128 y);
        // Return the remainder of the division of the specified 'x' by the
        // specified 'y'.  The returned value has the same sign as 'x' and is
        // less than 'y' in magnitude.
        //
        // Special value handling:
        //: o If either argument is quiet NaN, quiet NaN is returned.
        //: o If either argument is signaling NaN, quiet NaN is returned, and
        //:   the value of the macro 'EDOM' is stored into 'errno'.
        //: o If 'x' is +/-infnity and 'y' is not NaN, quiet NaN is returned
        //:   and the value of the macro 'EDOM' is stored into 'errno'.
        //: o If 'x' is +/-0 and 'y' is not zero, +/-0 is returned.
        //: o If 'y' is +/-0, quite NaN is returned and the value of the macro
        //:   'EDOM' is stored into 'errno'.
        //: o If 'x' is finite and 'y' is +/-infnity, 'x' is returned.

    static ValueType32  remainder(ValueType32  x, ValueType32  y);
    static ValueType64  remainder(ValueType64  x, ValueType64  y);
    static ValueType128 remainder(ValueType128 x, ValueType128 y);
        // Return the remainder of the division of the specified 'x' by the
        // specified 'y'.  The remainder of the division operation 'x/y'
        // calculated by this function is exactly the value 'x - n*y', where
        // 'n' s the integral value nearest the exact value 'x/y'.  When
        // '|n - x/y| == 0.5', the value 'n' is chosen to be even.  Note that
        // in contrast to 'DecimalImpUtil::fmod()', the returned value is not
        // guaranteed to have the same sign as 'x'.
        //
        // Special value handling:
        //: o The current rounding mode has no effect.
        //: o If either argument is quiet NaN, quiet NaN is returned.
        //: o If either argument is signaling NaN, quiet NaN is returned, and
        //:   the value of the macro 'EDOM' is stored into 'errno'.
        //: o If 'y' is +/-0, quiet NaN is returned and the value of the macro
        //:   'EDOM' is stored into 'errno'.
        //: o If 'x' is +/-infnity and 'y' is not NaN, quiet NaN is returned
        //:   and the value of the macro 'EDOM' is stored into 'errno'.
        //: o If 'x' is finite and 'y' is +/-infnity, 'x' is returned.

    static long int        lrint(ValueType32  x);
    static long int        lrint(ValueType64  x);
    static long int        lrint(ValueType128 x);
    static long long int  llrint(ValueType32  x);
    static long long int  llrint(ValueType64  x);
    static long long int  llrint(ValueType128 x);
        // Return an integer value nearest to the specified 'x'.  Round 'x'
        // using the current rounding mode.  If 'x' is +/-infnity, NaN (either
        // signaling or quiet) or the rounded value is outside the range of the
        // return type, store the value of the macro 'EDOM' into 'errno' and
        // return implementation-defined value.

    static ValueType32  nextafter( ValueType32  from, ValueType32  to);
    static ValueType64  nextafter( ValueType64  from, ValueType64  to);
    static ValueType128 nextafter( ValueType128 from, ValueType128 to);
    static ValueType32  nexttoward(ValueType32  from, ValueType128 to);
    static ValueType64  nexttoward(ValueType64  from, ValueType128 to);
    static ValueType128 nexttoward(ValueType128 from, ValueType128 to);
        // Return the next representable value of the specified 'from' in the
        // direction of the specified 'to'.
        //
        // Special value handling:
        //: o If 'from' equals 'to', 'to' is returned.
        //: o If either argument is quiet NaN, quiet NaN is returned.
        //: o If either argument is signaling NaN, quiet NaN is returned and
        //:   the value of the macro 'EDOM' is stored into 'errno'.
        //: o If 'from' is finite, but the expected result is infinity,
        //:   infinity is returned and the value of the macro 'ERANGE' is
        //:   stored into 'errno'.
        //: o If 'from' does not equal 'to' and the result is subnormal or
        //:   zero, the value of the macro 'ERANGE' is stored into 'errno'.

    static ValueType32  pow(ValueType32  base, ValueType32  exp);
    static ValueType64  pow(ValueType64  base, ValueType64  exp);
    static ValueType128 pow(ValueType128 base, ValueType128 exp);
        // Return the value of the specified 'base' raised to the power of the
        // specified 'exp'.
        //
        // Special value handling:
        //: o If 'base' is finite and negative and 'exp' is finite and
        //:   non-integer, quiet NaN is returned and the value of the macro
        //:   'EDOM' is stored into 'errno'.
        //: o If the mathematical result of this function is infinity or
        //:   undefined or a range error due to overflow occurs, infinity is
        //:   returned and the value of the macro 'ERANGE' is stored into
        //:   'errno'.
        //: o If a range error occurs due to underflow, the correct result
        //:   (after rounding) is returned and the value of the macro 'ERANGE'
        //:   is stored into 'errno'.
        //: o If either argument is signaling NaN, quiet NaN is returned and
        //:   the value of the macro 'EDOM' is stored into 'errno'.

    static ValueType32  ceil(ValueType32  x);
    static ValueType64  ceil(ValueType64  x);
    static ValueType128 ceil(ValueType128 x);
        // Return the smallest integral value that is not less than the
        // specified 'x'.
        //
        // Special value handling:
        //: o if 'x' is quiet NaN, quiet NaN is returned.
        //: o If 'x' is signaling NaN, quiet NaN is returned and the value of
        //:   the macro 'EDOM' is stored into 'errno'.
        //: o if 'x' is +/-infinity or +/-0, it is returned unmodified.
        //
        // Examples: 'ceil(0.5)' ==> 1.0; 'ceil(-0.5)' ==> 0.0

    static ValueType32  floor(ValueType32  x);
    static ValueType64  floor(ValueType64  x);
    static ValueType128 floor(ValueType128 x);
        // Return the largest integral value that is not greater than the
        // specified 'x'.
        //
        // Special value handling:
        //: o if 'x' is quiet NaN, quiet NaN is returned.
        //: o If 'x' is signaling NaN, quiet NaN is returned and the value of
        //:   the macro 'EDOM' is stored into 'errno'.
        //: o if 'x' is +/-infinity or +/-0, it is returned unmodified.
        //
        // Examples: 'floor(0.5)' ==> 0.0; 'floor(-0.5)' ==> -1.0

    static ValueType32  round(ValueType32  x);
    static ValueType64  round(ValueType64  x);
    static ValueType128 round(ValueType128 x);
        // Return the integral value nearest to the specified 'x'.  Round
        // halfway cases away from zero, regardless of the current decimal
        // floating point rounding mode.
        //
        // Special value handling:
        //: o if 'x' is quiet NaN, quiet NaN is returned.
        //: o If 'x' is signaling NaN, quiet NaN is returned and the value of
        //:   the macro 'EDOM' is stored into 'errno'.
        //: o if 'x' is +/-infinity or +/-0, it is returned unmodified.
        //
        // Examples: 'round(0.5)' ==> 1.0; 'round(-0.5)' ==> -1.0

    static long int lround(ValueType32  x);
    static long int lround(ValueType64  x);
    static long int lround(ValueType128 x);
        // Return the integral value nearest to the specified 'x'.  Round
        // halfway cases away from zero, regardless of the current decimal
        // floating point rounding mode.
        //
        // Special value handling:
        //: o if 'x' is NaN (either quiet or signaling), quiet NaN is returned
        //:   and the value of the macro 'EDOM' is stored into 'errno'.
        //: o if 'x' is +/-infinity, quite NaN is returned and the value of the
        //:   macro 'EDOM' is stored into 'errno'.
        //: o If the result of the rounding is outside the range of the return
        //:   type, the macro 'EDOM' is stored into 'errno'.
        //
        // Examples: 'lround(0.5)' ==> 1.0; 'lround(-0.5)' ==> -1.0

    static ValueType32  round(ValueType32  x, unsigned int precision);
    static ValueType64  round(ValueType64  x, unsigned int precision);
    static ValueType128 round(ValueType128 x, unsigned int precision);
        // Return the specified 'x' value rounded to the specified 'precision'.
        // Round halfway cases away from zero, regardless of the current
        // decimal floating point rounding mode.  If 'x' is integral, positive
        // zero, negative zero, NaN, or infinity then return 'x' itself.
        //
        //  Examples: 'round(3.14159, 3)' ==> 3.142


    static ValueType32  trunc(ValueType32  x);
    static ValueType64  trunc(ValueType64  x);
    static ValueType128 trunc(ValueType128 x);
        // Return the nearest integral value that is not greater in absolute
        // value than the specified 'x'.
        //
        // Special value handling:
        //: o if 'x' is quiet NaN, quiet NaN is returned.
        //: o If 'x' is signaling NaN, quiet NaN is returned and the value of
        //:   the macro 'EDOM' is stored into 'errno'.
        //: o if 'x' is +/-infinity or +/-0, it is returned unmodified.
        //
        // Examples: 'trunc(0.5)' ==> 0.0; 'trunc(-0.5)' ==> 0.0

    static ValueType32  fma(ValueType32  x, ValueType32  y, ValueType32  z);
    static ValueType64  fma(ValueType64  x, ValueType64  y, ValueType64  z);
    static ValueType128 fma(ValueType128 x, ValueType128 y, ValueType128 z);
        // Return, using the specified 'x', 'y', and 'z', the value of the
        // expression 'x * y + z', rounded as one ternary operation according
        // to the current decimal floating point rounding mode.
        //
        // Special value handling:
        //: o If 'x' or 'y' are quiet NaN, quiet NaN is returned.
        //: o If any argument is signaling NaN, quiet NaN is returned and the
        //:   value of the macro 'EDOM' is stored into 'errno'.
        //: o If 'x*y' is an exact infinity and 'z' is infinity with the
        //:   opposite sign, quiet NaN is returned and the value of the macro
        //:   'EDOM' is stored into 'errno'.
        //: o If 'x' is zero and 'y' is infinite or if 'x' is infinite and 'y'
        //:   is zero, and 'z' is not a NaN, then quiet NaN is returned and the
        //:   value of the macro 'EDOM' is stored into 'errno'.
        //: o If 'x' is zero and 'y' is infinite or if 'x' is infinite and 'y'
        //:   is zero, and 'z' is NaN, then quiet NaN is returned.

    static ValueType32  fabs(ValueType32  x);
    static ValueType64  fabs(ValueType64  x);
    static ValueType128 fabs(ValueType128 x);
        // Return the absolute value of the specified 'x'.
        //
        // Special value handling:
        //: o if 'x' is NaN (either signaling or quiet), quiet NaN is returned.
        //: o if 'x' is +/-infinity or +/-0, it is returned unmodified.

    static ValueType32  sqrt(ValueType32  x);
    static ValueType64  sqrt(ValueType64  x);
    static ValueType128 sqrt(ValueType128 x);
        // Return the square root of the specified 'x'.
        //
        // Special value handling:
        //: o If 'x' is quiet NaN, quiet NaN is returned.
        //: o If 'x' is signaling NaN, quiet NaN is returned and the value of
        //:   the macro 'EDOM' is stored into 'errno'.
        //: o If 'x' is less than -0, quiet NaN is returned and the value of
        //:   the macro 'EDOM' is stored into 'errno'.
        //: o If 'x' is +/-infinity or +/-0, it is returned unmodified.

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
        // If either or both operands are signaling NaN, store the value of the
        // macro 'EDOM' into 'errno' and return 'false'.

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
        // If either or both operands are signaling NaN, store the value of the
        // macro 'EDOM' into 'errno' and return 'false'.

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
        // If either or both operands are signaling NaN, store the value of the
        // macro 'EDOM' into 'errno' and return 'false'.

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
        // If either or both operands are signaling NaN, store the value of the
        // macro 'EDOM' into 'errno' and return 'false'.

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
        // If either or both operands are signaling NaN, store the value of the
        // macro 'EDOM' into 'errno' and return 'false'.

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
        // If either or both operands are signaling NaN, store the value of the
        // macro 'EDOM' into 'errno' and return 'false'.

                        // Inter-type Conversion functions

    static ValueType32  convertToDecimal32 (const ValueType64&  input);
    static ValueType32  convertToDecimal32 (const ValueType128& input);
    static ValueType64  convertToDecimal64 (const ValueType32&  input);
    static ValueType64  convertToDecimal64 (const ValueType128& input);
    static ValueType128 convertToDecimal128(const ValueType32&  input);
    static ValueType128 convertToDecimal128(const ValueType64&  input);
        // Convert the specified 'input' to the closest value of the indicated
        // result type following the conversion rules as defined by IEEE-754:
        //
        //: o If 'input' is signaling NaN, store the value of the macro 'EDOM'
        //:   into 'errno' and return signaling NaN value.
        //:
        //: o If 'input' is NaN, return NaN value.
        //:
        //: o Otherwise if 'input' is infinity (positive or negative), then
        //:   return infinity with the same sign.
        //:
        //: o Otherwise if 'input' is zero (positive or negative), then
        //:   return zero with the same sign.
        //:
        //: o Otherwise if 'input' has an absolute value that is larger than
        //:   maximum or is smaller than minimum value supported by the result
        //:   type, store the value of the macro 'ERANGE' into 'errno' and
        //:   return infinity or zero with the same sign respectively.
        //:
        //: o Otherwise if 'input' has a value that is not exactly
        //:   representable using maximum digit number supported by the
        //:   indicated result type then return the 'input' rounded according
        //:   to the rounding direction.
        //:
        //: o Otherwise return 'input' value of the result type.

                        // Binary floating point conversion functions

    static ValueType32  binaryToDecimal32(      float value);
    static ValueType32  binaryToDecimal32(     double value);
        // Create a 'Decimal32' object having the value closest to the
        // specified 'value' following the conversion rules as defined by
        // IEEE-754:
        //
        //: o If 'value' is signaling NaN, store the value of the macro 'EDOM'
        //:   into 'errno' and return signaling NaN value.
        //:
        //: o Otherwise if 'value' is NaN, return a NaN.
        //:
        //: o Otherwise if 'value' is infinity (positive or negative), then
        //:   return an object equal to infinity with the same sign.
        //:
        //: o Otherwise if 'value' is a zero value, then return an object equal
        //:   to zero with the same sign.
        //:
        //: o Otherwise if 'value' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal32>::max()' then store the value of
        //:   the macro 'ERANGE' into 'errno' and return infinity with the same
        //:   sign as 'value'.
        //:
        //: o Otherwise if 'value' has an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal32>::min()' then store the value of
        //:   the macro 'ERANGE' into 'errno' and return a zero with the same
        //:   sign as 'value'.
        //:
        //: o Otherwise if 'value' needs more than
        //:   'std::numeric_limits<Decimal32>::max_digit' significant decimal
        //:   digits to represent then return the 'value' rounded according to
        //:   the rounding direction.
        //:
        //: o Otherwise return a 'Decimal32' object representing 'value'.

    static ValueType64  binaryToDecimal64(      float value);
    static ValueType64  binaryToDecimal64(     double value);
        // Create a 'Decimal64' object having the value closest to the
        // specified 'value' following the conversion rules as defined by
        // IEEE-754:
        //
        //: o If 'value' is signaling NaN, store the value of the macro 'EDOM'
        //:   into 'errno' and return signaling NaN value.
        //:
        //: o Otherwise if 'value' is NaN, return a NaN.
        //:
        //: o Otherwise if 'value' is infinity (positive or negative), then
        //:   return an object equal to infinity with the same sign.
        //:
        //: o Otherwise if 'value' is a zero value, then return an object equal
        //:   to zero with the same sign.
        //:
        //: o Otherwise if 'value' needs more than
        //:   'std::numeric_limits<Decimal64>::max_digit' significant decimal
        //:   digits to represent then return the 'value' rounded according to
        //:   the rounding direction.
        //:
        //: o Otherwise return a 'Decimal64' object representing 'value'.

    static ValueType128 binaryToDecimal128(      float value);
    static ValueType128 binaryToDecimal128(     double value);
        // Create a 'Decimal128' object having the value closest to the
        // specified 'value' following the conversion rules as defined by
        // IEEE-754:
        //
        //: o If 'value' is signaling NaN, store the value of the macro 'EDOM'
        //:   into 'errno' and return signaling NaN value.
        //:
        //: o Otherwise if 'value' is NaN, return a NaN.
        //:
        //: o Otherwise if 'value' is infinity (positive or negative), then
        //:   return an object equal to infinity with the same sign.
        //:
        //: o Otherwise if 'value' is a zero value, then return an object equal
        //:   to zero with the same sign.
        //:
        //: o Otherwise if 'value' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal128>::max()' then store the value of
        //:   the macro 'ERANGE' into 'errno' and return infinity with the same
        //:   sign as 'value'.
        //:
        //: o Otherwise if 'value' has an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal128>::min()' then store the value of
        //:   the macro 'ERANGE' into 'errno' and return a zero with the same
        //:   sign as 'value'.
        //:
        //: o Otherwise if 'value' needs more than
        //:   'std::numeric_limits<Decimal128>::max_digit' significant decimal
        //:   digits to represent then return the 'value' rounded according to
        //:   the rounding direction.
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
        // according to IEEE 754's 'scaleB' operations.
        //
        // Special value handling:
        //: o If 'value' is quiet NaN, quiet NaN is returned.
        //: o If 'value' is signaling NaN, quiet NaN is returned and the value
        //:   of the macro 'EDOM' is stored into 'errno'.
        //: o If 'x' is infinite, then infinity is returned.
        //: o If a range error due to overflow occurs, infinity is returned and
        //: the value of the macro 'ERANGE' is stored into 'errno'.

                        // Parsing functions

    static ValueType32  parse32( const char *input);
    static ValueType64  parse64( const char *input);
    static ValueType128 parse128(const char *input);
    static ValueType32  parse32( const char *input, unsigned int *status);
    static ValueType64  parse64( const char *input, unsigned int *status);
    static ValueType128 parse128(const char *input, unsigned int *status);
        // Produce an object of the indicated return type by parsing the
        // specified 'input' string that represents a floating-point number
        // (written in either fixed or scientific notation).  Optionally
        // specify 'status' in which to load a bit mask of additional status
        // information.  The resulting decimal object is initialized as follows:
        //
        //: o If 'input' does not represent a floating-point value, then return
        //:   a decimal object of the indicated return type initialized to a
        //:   NaN.
        //:
        //: o Otherwise if 'input' represents infinity (positive or negative),
        //:   as case-insensitive "Inf" or "Infinity" string, then return a
        //:   decimal object of the indicated return type initialized to
        //:   infinity value with the same sign.
        //:
        //: o Otherwise if 'input' represents zero (positive or negative), then
        //:   return a decimal object of the indicated return type initialized
        //:   to zero with the same sign.
        //:
        //: o Otherwise if 'str' represents a value that has an absolute value
        //:   that is larger than 'std::numeric_limits<Decimal32>::max()' then
        //:   store the value of the macro 'ERANGE' into 'errno' and return a
        //:   decimal object of the indicated return type initialized to
        //:   infinity with the same sign.
        //:
        //: o Otherwise if 'input' represents a value that has an absolute
        //:   value that is smaller than
        //:   'std::numeric_limits<Decimal32>::min()', then store the value of
        //:   the macro 'ERANGE' into 'errno' and return a decimal object of
        //:   the indicated return type initialized to zero with the same sign.
        //:
        //: o Otherwise if 'input' has a value that is not exactly
        //:   representable using 'std::numeric_limits<Decimal32>::max_digit'
        //:   decimal digits then return a decimal object of the indicated
        //:   return type initialized to the value represented by 'input'
        //:   rounded according to the rounding direction.
        //:
        //: o Otherwise return a decimal object of the indicated return type
        //:   initialized to the decimal value representation of 'input'.
        //
        // The '*status', if supplied, mest be 0, and may be loaded with a bit
        // mask of 'k_STATUS_INEXACT', 'k_STATUS_UNDERFLOW', and
        // 'k_STATUS_OVERFLOW' indicating wether the conversion from text was
        // inexact, underflowed, or overflowed (or some combination)
        // respectively.  The behavior is undefined unless '*status' (if
        // supplied) is 0.
        //
        // Note that the parsing follows the rules as specified for the
        // 'strtod32' function in section 9.6 of the ISO/EIC TR 24732 C Decimal
        // Floating-Point Technical Report.
        //
        // Also note that the quanta of the resultant value is determined by
        // the number of decimal places starting with the most significand
        // digit in 'input' string and cannot exceed the maximum number of
        // digits necessary to differentiate all values of the indicated return
        // type, for example:
        //
        //..
        //  'parse32("0.015")    => 15e-3'
        //  'parse32("1.5")      => 15e-1'
        //  'parse32("1.500")    => 1500e-3'
        //  'parse32("1.2345678) => 1234568e-6'
        //..

    // Densely Packed Conversion Functions

    static ValueType32  convertDPDtoBID(DecimalStorage::Type32  dpd);
    static ValueType64  convertDPDtoBID(DecimalStorage::Type64  dpd);
    static ValueType128 convertDPDtoBID(DecimalStorage::Type128 dpd);
        // Return a 'ValueTypeXX' representing the specified 'dpd', which is
        // currently in Densely Packed Decimal (DPD) format.  This format is
        // compatible with the IBM compiler's native type.

    static DecimalStorage::Type32  convertBIDtoDPD(ValueType32  value);
    static DecimalStorage::Type64  convertBIDtoDPD(ValueType64  value);
    static DecimalStorage::Type128 convertBIDtoDPD(ValueType128 value);
        // Return a 'DecimalStorage::TypeXX' representing the specified 'value'
        // in Densely Packed Decimal (DPD) format.  This format is compatible
        // with the IBM compiler's native type.

                        // Binary Integral Conversion Functions

    static ValueType32  convertFromBID(DecimalStorage::Type32  bid);
    static ValueType64  convertFromBID(DecimalStorage::Type64  bid);
    static ValueType128 convertFromBID(DecimalStorage::Type128 bid);
        // Return a 'ValueTypeXX' representing the specified 'bid', which is
        // currently in Binary Integral Decimal (BID) format.  This format is
        // compatible with the Intel DFP implementation type.

    static
    DecimalStorage::Type32  convertToBID(ValueType32  value);
    static
    DecimalStorage::Type64  convertToBID(ValueType64  value);
    static
    DecimalStorage::Type128 convertToBID(ValueType128 value);
        // Return a 'DecimalStorage::TypeXX' representing  the specified
        // 'value' in Binary Integral Decimal (BID) format.  This format is
        // compatible with the Intel DFP implementation type.

                  // Functions returning special values

    static
    ValueType32 min32() BSLS_KEYWORD_NOEXCEPT;
        // Return the smallest positive normalized number 'ValueType32' can
        // represent (IEEE-754: +1e-95).

    static
    ValueType32 max32() BSLS_KEYWORD_NOEXCEPT;
        // Return the largest number 'ValueType32' can represent (IEEE-754:
        // +9.999999e+96).

    static
    ValueType32 epsilon32() BSLS_KEYWORD_NOEXCEPT;
        // Return the difference between the least representable value of type
        // 'ValueType32' greater than 1 and 1 (IEEE-754: +1e-6).

    static
    ValueType32 roundError32() BSLS_KEYWORD_NOEXCEPT;
        // Return the maximum rounding error for the 'ValueType32' type.  The
        // actual value returned depends on the current decimal floating point
        // rounding setting.

    static
    ValueType32 denormMin32() BSLS_KEYWORD_NOEXCEPT;
        // Return the smallest positive denormalized value for the
        // 'ValueType32' type (IEEE-754: +0.000001e-95).

    static
    ValueType32 infinity32() BSLS_KEYWORD_NOEXCEPT;
        // Return the value that represents positive infinity for the
        // 'ValueType32' type.

    static
    ValueType32 quietNaN32() BSLS_KEYWORD_NOEXCEPT;
        // Return a value that represents non-signaling NaN for the
        // 'ValueType32' type.

    static
    ValueType32 signalingNaN32() BSLS_KEYWORD_NOEXCEPT;
        // Return a value that represents signaling NaN for the 'ValueType32'
        // type.

    static
    ValueType64 min64() BSLS_KEYWORD_NOEXCEPT;
        // Return the smallest positive normalized number 'ValueType64' can
        // represent (IEEE-754: +1e-383).

    static
    ValueType64 max64() BSLS_KEYWORD_NOEXCEPT;
        // Return the largest number 'ValueType64' can represent (IEEE-754:
        // +9.999999999999999e+384).

    static
    ValueType64 epsilon64() BSLS_KEYWORD_NOEXCEPT;
        // Return the difference between the least representable value of type
        // 'ValueType64' greater than 1 and 1 (IEEE-754: +1e-15).

    static
    ValueType64 roundError64() BSLS_KEYWORD_NOEXCEPT;
        // Return the maximum rounding error for the 'ValueType64' type.  The
        // actual value returned depends on the current decimal floating point
        // rounding setting.

    static
    ValueType64 denormMin64() BSLS_KEYWORD_NOEXCEPT;
        // Return the smallest positive denormalized value for the
        // 'ValueType64' type (IEEE-754: +0.000000000000001e-383).

    static
    ValueType64 infinity64() BSLS_KEYWORD_NOEXCEPT;
        // Return the value that represents positive infinity for the
        // 'ValueType64' type.

    static
    ValueType64 quietNaN64() BSLS_KEYWORD_NOEXCEPT;
        // Return a value that represents non-signaling NaN for the
        // 'ValueType64' type.

    static
    ValueType64 signalingNaN64() BSLS_KEYWORD_NOEXCEPT;
        // Return a value that represents signaling NaN for the 'ValueType64'
        // type.

    static
    ValueType128 min128() BSLS_KEYWORD_NOEXCEPT;
        // Return the smallest positive normalized number 'ValueType128' can
        // represent (IEEE-754: +1e-6143).

    static
    ValueType128 max128() BSLS_KEYWORD_NOEXCEPT;
        // Return the largest number 'ValueType128' can represent (IEEE-754:
        // +9.999999999999999999999999999999999e+6144).

    static
    ValueType128 epsilon128() BSLS_KEYWORD_NOEXCEPT;
        // Return the difference between the least representable value of type
        // 'ValueType128' greater than 1 and 1 (IEEE-754: +1e-33).

    static
    ValueType128 roundError128() BSLS_KEYWORD_NOEXCEPT;
        // Return the maximum rounding error for the 'ValueType128' type.  The
        // actual value returned depends on the current decimal floating point
        // rounding setting.

    static
    ValueType128 denormMin128() BSLS_KEYWORD_NOEXCEPT;
        // Return the smallest positive denormalized value for the
        // 'ValueType128' type (IEEE-754:
        // +0.000000000000000000000000000000001e-6143).

    static
    ValueType128 infinity128() BSLS_KEYWORD_NOEXCEPT;
        // Return the value that represents positive infinity for the
        // 'ValueType128' type.

    static
    ValueType128 quietNaN128() BSLS_KEYWORD_NOEXCEPT;
        // Return a value that represents non-signaling NaN for the
        // 'ValueType128' type.

    static
    ValueType128 signalingNaN128() BSLS_KEYWORD_NOEXCEPT;
        // Return a value that represents signaling NaN for the 'ValueType128'
        // type.
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
DecimalImpUtil::ValueType32 DecimalImpUtil::int32ToDecimal32(int value)
{
    return Imp::int32ToDecimal32(value);
}

inline
DecimalImpUtil::ValueType64 DecimalImpUtil::int32ToDecimal64(int value)
{
    return Imp::int32ToDecimal64(value);
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::int32ToDecimal128(int value)
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
DecimalImpUtil::ValueType32
DecimalImpUtil::add(DecimalImpUtil::ValueType32 lhs,
                    DecimalImpUtil::ValueType32 rhs)
{
    return Imp::add(lhs, rhs);
}

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
DecimalImpUtil::ValueType32
DecimalImpUtil::subtract(DecimalImpUtil::ValueType32 lhs,
                         DecimalImpUtil::ValueType32 rhs)
{
    return Imp::subtract(lhs, rhs);
}

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
DecimalImpUtil::ValueType32
DecimalImpUtil::multiply(DecimalImpUtil::ValueType32 lhs,
                         DecimalImpUtil::ValueType32 rhs)
{
    return Imp::multiply(lhs, rhs);
}

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

                        // Division Functions

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::divide(DecimalImpUtil::ValueType32 lhs,
                       DecimalImpUtil::ValueType32 rhs)
{
    return Imp::divide(lhs, rhs);
}

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

inline
DecimalImpUtil::ValueType32  DecimalImpUtil::quantize(ValueType32  value,
                                                      ValueType32  exponent)
{
    DecimalImpUtil::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_quantize(value.d_raw,
                                    exponent.d_raw,
                                    &flags);
    return retval;
}

inline
DecimalImpUtil::ValueType64  DecimalImpUtil::quantize(ValueType64  value,
                                                      ValueType64  exponent)
{
    DecimalImpUtil::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_quantize(value.d_raw,
                                    exponent.d_raw,
                                    &flags);
    return retval;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::quantize(ValueType128 value,
                                                      ValueType128 exponent)
{
    DecimalImpUtil::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_quantize(value.d_raw,
                                     exponent.d_raw,
                                     &flags);
    return retval;
}

inline
DecimalImpUtil::ValueType32  DecimalImpUtil::quantize(ValueType32  value,
                                                      int          exponent)
{
    BSLS_ASSERT(-101 <= exponent);
    BSLS_ASSERT(        exponent <= 90);

    DecimalImpUtil::ValueType32 retval;
    DecimalImpUtil::ValueType32 exp = DecimalImpUtil::makeDecimalRaw32(
                                                                     1,
                                                                     exponent);
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_quantize(value.d_raw, exp.d_raw, &flags);
    return retval;
}

inline
DecimalImpUtil::ValueType64  DecimalImpUtil::quantize(ValueType64  value,
                                                      int          exponent)
{
    BSLS_ASSERT(-398 <= exponent);
    BSLS_ASSERT(        exponent <= 369);

    DecimalImpUtil::ValueType64 retval;
    DecimalImpUtil::ValueType64 exp = DecimalImpUtil::makeDecimalRaw64(
                                                                     1,
                                                                     exponent);
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_quantize(value.d_raw, exp.d_raw, &flags);
    return retval;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::quantize(ValueType128 value,
                                                      int          exponent)
{
    BSLS_ASSERT(-6176 <= exponent);
    BSLS_ASSERT(         exponent <= 6111);

    DecimalImpUtil::ValueType128 retval;
    DecimalImpUtil::ValueType128 exp = DecimalImpUtil::makeDecimalRaw128(
                                                                     1,
                                                                     exponent);
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_quantize(value.d_raw, exp.d_raw, &flags);
    return retval;
}

inline
int DecimalImpUtil::quantizeEqual(ValueType32 *x, ValueType32 y, int exponent)
{
    BSLS_ASSERT(x);
    BSLS_ASSERT(-101 <= exponent);
    BSLS_ASSERT(        exponent <= 90);

    DecimalImpUtil::ValueType32 retval;
    DecimalImpUtil::ValueType32 exp = DecimalImpUtil::makeDecimalRaw32(
                                                                     1,
                                                                     exponent);
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_quantize(y.d_raw, exp.d_raw, &flags);
    if (DecimalImpUtil::equal(retval, y)) {
        *x = retval;
        return 0;
    }
    return -1;
}

inline
int DecimalImpUtil::quantizeEqual(ValueType64 *x, ValueType64 y, int exponent)
{
    BSLS_ASSERT(x);
    BSLS_ASSERT(-398 <= exponent);
    BSLS_ASSERT(        exponent <= 369);
    DecimalImpUtil::ValueType64 retval;
    DecimalImpUtil::ValueType64 exp = DecimalImpUtil::makeDecimalRaw64(
                                                                     1,
                                                                     exponent);
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_quantize(y.d_raw, exp.d_raw, &flags);
    if (DecimalImpUtil::equal(retval, y)) {
        *x = retval;
        return 0;
    }
    return -1;
}

inline
int DecimalImpUtil::quantizeEqual(ValueType128 *x, ValueType128 y, int exponent)
{
    BSLS_ASSERT(x);
    BSLS_ASSERT(-6176 <= exponent);
    BSLS_ASSERT(         exponent <= 6111);

    DecimalImpUtil::ValueType128 retval;
    DecimalImpUtil::ValueType128 exp = DecimalImpUtil::makeDecimalRaw128(
                                                                     1,
                                                                     exponent);
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_quantize(y.d_raw, exp.d_raw, &flags);
    if (DecimalImpUtil::equal(retval, y)) {
        *x = retval;
        return 0;
    }
    return -1;
}

inline
bool DecimalImpUtil::sameQuantum(ValueType32  x, ValueType32  y)
{
    return __bid32_sameQuantum(x.d_raw, y.d_raw);
}

inline
bool DecimalImpUtil::sameQuantum(ValueType64  x, ValueType64  y)
{
    return __bid64_sameQuantum(x.d_raw, y.d_raw);
}

inline
bool DecimalImpUtil::sameQuantum(ValueType128 x, ValueType128 y)
{
    return __bid128_sameQuantum(x.d_raw, y.d_raw);
}


                        // Math functions

inline
long int DecimalImpUtil::lrint(ValueType32  x)
{
    _IDEC_flags flags(0);
    long int ret =  __bid32_lrint(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    return ret;
}

inline
long int DecimalImpUtil::lrint(ValueType64  x)
{
    _IDEC_flags flags(0);
    long int ret =  __bid64_lrint(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    return ret;
}

inline
long int DecimalImpUtil::lrint(ValueType128 x)
{
    _IDEC_flags flags(0);
    long int ret =  __bid128_lrint(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    return ret;
}

inline
long long int DecimalImpUtil::llrint(ValueType32  x)
{
    _IDEC_flags flags(0);
    long long int ret =  __bid32_llrint(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    return ret;
}

inline
long long int DecimalImpUtil::llrint(ValueType64  x)
{
    _IDEC_flags flags(0);
    long long int ret =  __bid64_llrint(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    return ret;
}

inline
long long int DecimalImpUtil::llrint(ValueType128 x)
{
    _IDEC_flags flags(0);
    long long int ret =  __bid128_llrint(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    return ret;
}

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::nextafter(ValueType32  x, ValueType32  y)
{
    DecimalImpUtil::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_nextafter(x.d_raw, y.d_raw, &flags);
    if (BID_OVERFLOW_EXCEPTION  & flags ||
        BID_UNDERFLOW_EXCEPTION & flags)
    {
        errno = ERANGE;
    }
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::nextafter(ValueType64  x, ValueType64  y)
{
    DecimalImpUtil::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_nextafter(x.d_raw, y.d_raw, &flags);
    if (BID_OVERFLOW_EXCEPTION  & flags ||
        BID_UNDERFLOW_EXCEPTION & flags)
    {
        errno = ERANGE;
    }
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::nextafter(ValueType128 x, ValueType128 y)
{
    DecimalImpUtil::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_nextafter(x.d_raw, y.d_raw, &flags);
    if (BID_OVERFLOW_EXCEPTION  & flags ||
        BID_UNDERFLOW_EXCEPTION & flags)
    {
        errno = ERANGE;
    }
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::nexttoward(ValueType32  x, ValueType128 y)
{
    DecimalImpUtil::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_nexttoward(x.d_raw, y.d_raw, &flags);
    if (BID_OVERFLOW_EXCEPTION  & flags ||
        BID_UNDERFLOW_EXCEPTION & flags)
    {
        errno = ERANGE;
    }
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::nexttoward(ValueType64  x, ValueType128 y)
{
    DecimalImpUtil::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_nexttoward(x.d_raw, y.d_raw, &flags);
    if (BID_OVERFLOW_EXCEPTION  & flags ||
        BID_UNDERFLOW_EXCEPTION & flags)
    {
        errno = ERANGE;
    }
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::nexttoward(ValueType128 x, ValueType128 y)
{
    DecimalImpUtil::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_nexttoward(x.d_raw, y.d_raw, &flags);
    if (BID_OVERFLOW_EXCEPTION  & flags ||
        BID_UNDERFLOW_EXCEPTION & flags)
    {
        errno = ERANGE;
    }
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::pow(ValueType32  base, ValueType32  exp)
{
    DecimalImpUtil::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_pow(base.d_raw, exp.d_raw, &flags);
    if (BID_OVERFLOW_EXCEPTION    & flags ||
        BID_UNDERFLOW_EXCEPTION   & flags ||
        BID_ZERO_DIVIDE_EXCEPTION & flags)
    {
        errno = ERANGE;
    }
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::pow(ValueType64  base, ValueType64  exp)
{
    DecimalImpUtil::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_pow(base.d_raw, exp.d_raw, &flags);
    if (BID_OVERFLOW_EXCEPTION    & flags ||
        BID_UNDERFLOW_EXCEPTION   & flags ||
        BID_ZERO_DIVIDE_EXCEPTION & flags)
    {
        errno = ERANGE;
    }
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }

    return retval;
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::pow(ValueType128 base, ValueType128 exp)
{
    DecimalImpUtil::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_pow(base.d_raw, exp.d_raw, &flags);
    if (BID_OVERFLOW_EXCEPTION    & flags ||
        BID_UNDERFLOW_EXCEPTION   & flags ||
        BID_ZERO_DIVIDE_EXCEPTION & flags)
    {
        errno = ERANGE;
    }
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType32 DecimalImpUtil::ceil(ValueType32  x)
{
    DecimalImpUtil::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_round_integral_positive(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType64 DecimalImpUtil::ceil(ValueType64  x)
{
    DecimalImpUtil::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_round_integral_positive(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::ceil(ValueType128 x)
{
    DecimalImpUtil::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_round_integral_positive(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;

}

inline
DecimalImpUtil::ValueType32 DecimalImpUtil::floor(ValueType32 x)
{
    DecimalImpUtil::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_round_integral_negative(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType64 DecimalImpUtil::floor(ValueType64 x)
{
    DecimalImpUtil::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_round_integral_negative(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::floor(ValueType128 x)
{
    DecimalImpUtil::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_round_integral_negative(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType32 DecimalImpUtil::round(ValueType32 x)
{
    DecimalImpUtil::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_round_integral_nearest_away(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType64 DecimalImpUtil::round(ValueType64 x)
{
    DecimalImpUtil::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_round_integral_nearest_away(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::round(ValueType128 x)
{
    DecimalImpUtil::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_round_integral_nearest_away(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
long int DecimalImpUtil::lround(ValueType32 x)
{
    _IDEC_flags flags(0);
    long int rv = __bid32_lround(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return rv;
}

inline
long int DecimalImpUtil::lround(ValueType64 x)
{
    _IDEC_flags flags(0);
    long int rv = __bid64_lround(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return rv;
}

inline
long int DecimalImpUtil::lround(ValueType128 x)
{
    _IDEC_flags flags(0);
    long int rv = __bid128_lround(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType32 DecimalImpUtil::trunc(ValueType32 x)
{
    DecimalImpUtil::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_round_integral_zero(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType64 DecimalImpUtil::trunc(ValueType64 x)
{
    DecimalImpUtil::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_round_integral_zero(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::trunc(ValueType128 x)
{
    DecimalImpUtil::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_round_integral_zero(x.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return retval;
}


inline
DecimalImpUtil::ValueType32 DecimalImpUtil::fma(ValueType32 x, ValueType32 y, ValueType32 z)
{
    ValueType32 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid32_fma(x.d_raw, y.d_raw, z.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType64 DecimalImpUtil::fma(ValueType64 x, ValueType64 y, ValueType64 z)
{
    ValueType64 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid64_fma(x.d_raw, y.d_raw, z.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::fma(ValueType128 x, ValueType128 y, ValueType128 z)
{
    ValueType128 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid128_fma(x.d_raw, y.d_raw, z.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return rv;
}
                       // Selecting, converting functions

inline
DecimalImpUtil::ValueType32 DecimalImpUtil::fabs(ValueType32 value)
{
    ValueType32 rv;
    rv.d_raw = __bid32_abs(value.d_raw);
    return rv;
}

inline
DecimalImpUtil::ValueType64 DecimalImpUtil::fabs(ValueType64 value)
{
    ValueType64 rv;
    rv.d_raw = __bid64_abs(value.d_raw);
    return rv;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::fabs(ValueType128 value)
{
    ValueType128 rv;
    rv.d_raw = __bid128_abs(value.d_raw);
    return rv;
}

inline
DecimalImpUtil::ValueType32 DecimalImpUtil::sqrt(ValueType32 value)
{
    ValueType32 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid32_sqrt(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType64 DecimalImpUtil::sqrt(ValueType64 value)
{
    ValueType64 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid64_sqrt(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::sqrt(ValueType128 value)
{
    ValueType128 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid128_sqrt(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType32  DecimalImpUtil::copySign(ValueType32  x,
                                                      ValueType32  y)
{
    ValueType32 rv;
    rv.d_raw = __bid32_copySign(x.d_raw, y.d_raw);
    return rv;
}

inline
DecimalImpUtil::ValueType64  DecimalImpUtil::copySign(ValueType64  x,
                                                      ValueType64  y)
{
    ValueType64 rv;
    rv.d_raw = __bid64_copySign(x.d_raw, y.d_raw);
    return rv;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::copySign(ValueType128 x,
                                                      ValueType128 y)
{
    ValueType128 rv;
    rv.d_raw = __bid128_copySign(x.d_raw, y.d_raw);
    return rv;
}

inline
DecimalImpUtil::ValueType32 DecimalImpUtil::exp(ValueType32 value)
{
    ValueType32 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid32_exp(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_OVERFLOW_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType64 DecimalImpUtil::exp(ValueType64 value)
{
    ValueType64 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid64_exp(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_OVERFLOW_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::exp(ValueType128 value)
{
    ValueType128 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid128_exp(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_OVERFLOW_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType32 DecimalImpUtil::log(ValueType32 value)
{
    ValueType32 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid32_log(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_ZERO_DIVIDE_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType64 DecimalImpUtil::log(ValueType64 value)
{
    ValueType64 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid64_log(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_ZERO_DIVIDE_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::log(ValueType128 value)
{
    ValueType128 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid128_log(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_ZERO_DIVIDE_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType32 DecimalImpUtil::logB(ValueType32 value)
{
    ValueType32 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid32_logb(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_ZERO_DIVIDE_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType64 DecimalImpUtil::logB(ValueType64 value)
{
    ValueType64 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid64_logb(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_ZERO_DIVIDE_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::logB(ValueType128 value)
{
    ValueType128 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid128_logb(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_ZERO_DIVIDE_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType32 DecimalImpUtil::log10(ValueType32 value)
{
    ValueType32 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid32_log10(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_ZERO_DIVIDE_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType64 DecimalImpUtil::log10(ValueType64 value)
{
    ValueType64 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid64_log10(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_ZERO_DIVIDE_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::log10(ValueType128 value)
{
    ValueType128 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid128_log10(value.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_ZERO_DIVIDE_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType32  DecimalImpUtil::fmod(ValueType32  x,
                                                  ValueType32  y)
{
    ValueType32 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid32_fmod(x.d_raw, y.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType64  DecimalImpUtil::fmod(ValueType64  x,
                                                  ValueType64  y)
{
    ValueType64 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid64_fmod(x.d_raw, y.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::fmod(ValueType128 x,
                                                  ValueType128 y)
{
    ValueType128 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid128_fmod(x.d_raw, y.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType32  DecimalImpUtil::remainder(ValueType32  x,
                                                       ValueType32  y)
{
    ValueType32 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid32_rem(x.d_raw, y.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType64  DecimalImpUtil::remainder(ValueType64  x,
                                                       ValueType64  y)
{
    ValueType64 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid64_rem(x.d_raw, y.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    return rv;
}

inline
DecimalImpUtil::ValueType128 DecimalImpUtil::remainder(ValueType128 x,
                                                       ValueType128 y)
{
    ValueType128 rv;
    _IDEC_flags flags(0);
    rv.d_raw = __bid128_rem(x.d_raw, y.d_raw, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    return rv;
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
DecimalImpUtil::ValueType32
DecimalImpUtil::convertToDecimal32(const DecimalImpUtil::ValueType128& input)
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
    ValueType32 result;
    _IDEC_flags flags(0);
    result.d_raw = __bid32_scalbn(value.d_raw, exponent, &flags);
    if (BID_INVALID_EXCEPTION     & flags) {
        errno = EDOM;
    }
    if (BID_OVERFLOW_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return result;
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::scaleB(DecimalImpUtil::ValueType64 value, int exponent)
{
    ValueType64 result;
    _IDEC_flags flags(0);
    result.d_raw = __bid64_scalbn(value.d_raw, exponent, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_OVERFLOW_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return result;
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::scaleB(DecimalImpUtil::ValueType128 value, int exponent)
{
    ValueType128 result;
    _IDEC_flags flags(0);
    result.d_raw = __bid128_scalbn(value.d_raw, exponent, &flags);
    if (BID_INVALID_EXCEPTION & flags) {
        errno = EDOM;
    }
    if (BID_OVERFLOW_EXCEPTION & flags) {
        errno = ERANGE;
    }
    return result;
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

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::parse32(const char *input, unsigned int *status)
{
    BSLS_ASSERT(0 == *status);
    return Imp::parse32(input, status);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::parse64(const char *input, unsigned int *status)
{
    BSLS_ASSERT(0 == *status);
    return Imp::parse64(input, status);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::parse128(const char *input, unsigned int *status)
{
    BSLS_ASSERT(0 == *status);
    return Imp::parse128(input, status);
}

                        // Densely Packed Conversion Functions
inline
DecimalImpUtil::ValueType32
DecimalImpUtil::convertDPDtoBID(DecimalStorage::Type32 dpd)
{
    return Imp::convertDPDtoBID(dpd);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::convertDPDtoBID(DecimalStorage::Type64 dpd)
{
    return Imp::convertDPDtoBID(dpd);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::convertDPDtoBID(DecimalStorage::Type128 dpd)
{
    return Imp::convertDPDtoBID(dpd);
}

inline
DecimalStorage::Type32
DecimalImpUtil::convertBIDtoDPD(ValueType32 value)
{
    return Imp::convertBIDtoDPD(value);
}

inline
DecimalStorage::Type64
DecimalImpUtil::convertBIDtoDPD(ValueType64 value)
{
    return Imp::convertBIDtoDPD(value);
}

inline
DecimalStorage::Type128
DecimalImpUtil::convertBIDtoDPD(ValueType128 value)
{
    return Imp::convertBIDtoDPD(value);
}
                        // Binary Integral Conversion Functions

inline
DecimalImpUtil::ValueType32
DecimalImpUtil::convertFromBID(DecimalStorage::Type32 bid)
{
    return Imp::convertFromBID(bid);
}

inline
DecimalImpUtil::ValueType64
DecimalImpUtil::convertFromBID(DecimalStorage::Type64 bid)
{
    return Imp::convertFromBID(bid);
}

inline
DecimalImpUtil::ValueType128
DecimalImpUtil::convertFromBID(DecimalStorage::Type128 bid)
{
    return Imp::convertFromBID(bid);
}

inline
DecimalStorage::Type32
DecimalImpUtil::convertToBID(ValueType32 value)
{
    return Imp::convertToBID(value);
}

inline
DecimalStorage::Type64
DecimalImpUtil::convertToBID(ValueType64 value)
{
    return Imp::convertToBID(value);
}

inline
DecimalStorage::Type128
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
