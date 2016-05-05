// bdldfp_decimalimputil_ibmxlc.h                                     -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL_IBMXLC
#define INCLUDED_BDLDFP_DECIMALIMPUTIL_IBMXLC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide utility to implement decimal 'float's on the IBM compiler.
//
//@CLASSES:
//  bdldfp::DecimalImpUtil_IbmXlc: Namespace for IBM decimal FP functions
//
//@SEE_ALSO: bdldfp_decimal, bdldfp_decimalplatform
//
//@DESCRIPTION: This component is for internal use only by the
// 'bdldfp_decimal*' components.  Direct use of any names declared in this
// component by any other code invokes undefined behavior.  In other words:
// this code may change, disappear, break, move without notice, and no support
// whatsoever will ever be provided for it.
//
///Usage
///-----
// This section shows the intended use of this component.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#include <bdldfp_decimalplatform.h>
#endif

#ifndef INCLUDED_BDLDFP_DENSELYPACKEDDECIMALIMPUTIL
#include <bdldfp_denselypackeddecimalimputil.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR

#ifndef INCLUDED_DECSINGLE
extern "C" {
#include <decnumber/decSingle.h>
}
#endif

namespace BloombergLP {
namespace bdldfp {

                          // ============================
                          // class DecimalImplUtil_IbmXlc
                          // ============================

struct DecimalImpUtil_IbmXlc {
    // This 'struct' provides a namespace for implementation functions that
    // work in terms of the underlying C-style decimal floating point
    // implementation, Intel's DFP library.

    // TYPES
    typedef _Decimal32  ValueType32;
    typedef _Decimal64  ValueType64;
    typedef _Decimal128 ValueType128;

    // CLASS METHODS

                        // Integer construction (32-bit)

    static ValueType32   int32ToDecimal32 (                   int value);
    static ValueType32  uint32ToDecimal32 (unsigned           int value);
    static ValueType32   int64ToDecimal32 (         long long int value);
    static ValueType32  uint64ToDecimal32 (unsigned long long int value);
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

                        // Integer construction (64-bit)

    static ValueType64   int32ToDecimal64 (                   int value);
    static ValueType64  uint32ToDecimal64 (unsigned           int value);
    static ValueType64   int64ToDecimal64 (         long long int value);
    static ValueType64  uint64ToDecimal64 (unsigned long long int value);
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

                        // Integer construction (128-bit)

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

                        // Arithmetic functions

                        // Addition functions

    static ValueType64  add(ValueType64  lhs,  ValueType64  rhs);
    static ValueType128 add(ValueType128 lhs,  ValueType128 rhs);
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

    static ValueType64  subtract(ValueType64  lhs,  ValueType64  rhs);
    static ValueType128 subtract(ValueType128 lhs,  ValueType128 rhs);
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

    static ValueType64  multiply(ValueType64  lhs,  ValueType64  rhs);
    static ValueType128 multiply(ValueType128 lhs,  ValueType128 rhs);
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

    static ValueType64  divide(ValueType64  lhs,  ValueType64  rhs);
    static ValueType128 divide(ValueType128 lhs,  ValueType128 rhs);
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
        // Return the result of applying the unary - operator to the specified
        // 'value' as described by IEEE-754.  Note that floating-point numbers
        // have signed zero, therefore this operation is not the same as
        // '0-value'.

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

    static ValueType32 binaryToDecimal32(      float value);
    static ValueType32 binaryToDecimal32(     double value);
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

    static ValueType64 binaryToDecimal64(      float value);
    static ValueType64 binaryToDecimal64(     double value);
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

    static ValueType32  makeDecimalRaw32(int significand, int exponent);
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

                        // IEEE Scale B functions

    static ValueType32  scaleB(ValueType32  value, int exponent);
    static ValueType64  scaleB(ValueType64  value, int exponent);
    static ValueType128 scaleB(ValueType128 value, int exponent);
        // Return the result of multiplying the specified 'value' by ten raised
        // to the specified 'exponent'.  The quantum of 'value' is scaled
        // according to IEEE 754's 'scaleB' operations.  The result is
        // unspecified if 'value' is NaN or infinity.  The behavior is
        // undefined unless '-1999999997 <= y <= 99999999'.

                        // Parsing functions

    static ValueType32 parse32 (const char *string);
        // Parse the specified 'string' as a 32 bit decimal floating- point
        // value and return the result.  The parsing is as specified for the
        // 'strtod32' function in section 9.6 of the ISO/EIC TR 24732 C Decimal
        // Floating-Point Technical Report, except that it is unspecified
        // whether the NaNs returned are quiet or signaling.  The behavior is
        // undefined unless 'input' represents a valid 32 bit decimal
        // floating-point number in scientific or fixed notation, and no
        // unrelated characters precede (not even whitespace) that textual
        // representation and a terminating nul character immediately follows
        // it.  Note that this method does not guarantee the behavior of
        // ISO/EIC TR 24732 C when parsing NaN because the AIX compiler
        // intrinsics return a signaling NaN.

    static ValueType64 parse64(const char *string);
        // Parse the specified 'string' string as a 64 bit decimal floating-
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

    static ValueType128 parse128(const char *string);
        // Parse the specified 'string' string as a 128 bit decimal floating-
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

                        // Formatting Functions

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
        // Return a 'DenselyPackedDecimalImpUtil::StorageTypeXX' representing
        // the specified 'value' in Densely Packed Decimal (DPD) format.  This
        // format is compatible with the IBM compiler's native type, and the
        // decNumber library.

                        // Binary Integral Conversion Functions

    // The IBM XLC compiler's Decimal implementation does not currently support
    // conversion to or from 'BID', so these functions are not provided.  This
    // will cause a compile-time failure for this implementation.  A portable
    // implementation of this algorithm is forthcoming.

  //static ValueType32  convertFromBID(
  //                         BinaryIntegralDecimalImpUtil::StorageType32  bid);
  //static ValueType64  convertFromBID(
  //                         BinaryIntegralDecimalImpUtil::StorageType64  bid);
  //static ValueType128 convertFromBID(
  //                         BinaryIntegralDecimalImpUtil::StorageType128 bid);
        // Return a 'ValueTypeXX' representing the specified 'bid', which is
        // currently in Binary Integral Decimal (BID) format.  This format is
        // compatible with the Intel DFP implementation type.

  //static
  //BinaryIntegralDecimalImpUtil::StorageType32  convertToBID(
  //                                                       ValueType32  value);
  //static
  //BinaryIntegralDecimalImpUtil::StorageType64  convertToBID(
  //                                                       ValueType64  value);
  //static
  //BinaryIntegralDecimalImpUtil::StorageType128 convertToBID(
  //                                                       ValueType128 value);
        // Return a 'BinaryIntegralDecimalImpUtil::StorageTypeXX' representing
        // the specified 'value' in Binary Integral Decimal (BID) format.  This
        // format is compatible with the Intel DFP implementation type.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // Integer construction

inline
DecimalImpUtil_IbmXlc::ValueType32
DecimalImpUtil_IbmXlc::int32ToDecimal32(int value)
{
    return value;
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::int32ToDecimal64(int value)
{
    return value;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::int32ToDecimal128(int value)
{
    return value;
}


inline
DecimalImpUtil_IbmXlc::ValueType32
DecimalImpUtil_IbmXlc::uint32ToDecimal32(unsigned int value)
{
    return value;
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::uint32ToDecimal64(unsigned int value)
{
    return value;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::uint32ToDecimal128(unsigned int value)
{
    return value;
}


inline
DecimalImpUtil_IbmXlc::ValueType32
DecimalImpUtil_IbmXlc::int64ToDecimal32(long long int value)
{
    return value;
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::int64ToDecimal64(long long int value)
{
    return value;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::int64ToDecimal128(long long int value)
{
    return value;
}


inline
DecimalImpUtil_IbmXlc::ValueType32
DecimalImpUtil_IbmXlc::uint64ToDecimal32(unsigned long long int value)
{
    return value;
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::uint64ToDecimal64(unsigned long long int value)
{
    return value;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::uint64ToDecimal128(unsigned long long int value)
{
    return value;
}

                        // Arithmetic

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::add(DecimalImpUtil_IbmXlc::ValueType64 lhs,
                           DecimalImpUtil_IbmXlc::ValueType64 rhs)
{
    return lhs + rhs;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::add(DecimalImpUtil_IbmXlc::ValueType128 lhs,
                           DecimalImpUtil_IbmXlc::ValueType128 rhs)
{
    return lhs + rhs;
}



inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::subtract(DecimalImpUtil_IbmXlc::ValueType64 lhs,
                                DecimalImpUtil_IbmXlc::ValueType64 rhs)
{
    return lhs - rhs;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::subtract(DecimalImpUtil_IbmXlc::ValueType128 lhs,
                                DecimalImpUtil_IbmXlc::ValueType128 rhs)
{
    return lhs - rhs;
}



inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::multiply(DecimalImpUtil_IbmXlc::ValueType64 lhs,
                                DecimalImpUtil_IbmXlc::ValueType64 rhs)
{
    return lhs * rhs;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::multiply(DecimalImpUtil_IbmXlc::ValueType128 lhs,
                                DecimalImpUtil_IbmXlc::ValueType128 rhs)
{
    return lhs * rhs;
}



inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::divide(DecimalImpUtil_IbmXlc::ValueType64 lhs,
                              DecimalImpUtil_IbmXlc::ValueType64 rhs)
{
    return lhs / rhs;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::divide(DecimalImpUtil_IbmXlc::ValueType128  lhs,
                              DecimalImpUtil_IbmXlc::ValueType128  rhs)
{
    return lhs / rhs;
}


inline
DecimalImpUtil_IbmXlc::ValueType32
DecimalImpUtil_IbmXlc::negate(DecimalImpUtil_IbmXlc::ValueType32 value)
{
    return -value;
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::negate(DecimalImpUtil_IbmXlc::ValueType64 value)
{
    return -value;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::negate(DecimalImpUtil_IbmXlc::ValueType128 value)
{
    return -value;
}


inline
bool
DecimalImpUtil_IbmXlc::less(DecimalImpUtil_IbmXlc::ValueType32 lhs,
                            DecimalImpUtil_IbmXlc::ValueType32 rhs)
{
    return lhs < rhs;
}

inline
bool
DecimalImpUtil_IbmXlc::less(DecimalImpUtil_IbmXlc::ValueType64 lhs,
                            DecimalImpUtil_IbmXlc::ValueType64 rhs)
{
    return lhs < rhs;
}

inline
bool
DecimalImpUtil_IbmXlc::less(DecimalImpUtil_IbmXlc::ValueType128 lhs,
                            DecimalImpUtil_IbmXlc::ValueType128 rhs)
{
    return lhs < rhs;
}


inline
bool
DecimalImpUtil_IbmXlc::greater(DecimalImpUtil_IbmXlc::ValueType32 lhs,
                               DecimalImpUtil_IbmXlc::ValueType32 rhs)
{
    return lhs > rhs;
}

inline
bool
DecimalImpUtil_IbmXlc::greater(DecimalImpUtil_IbmXlc::ValueType64 lhs,
                               DecimalImpUtil_IbmXlc::ValueType64 rhs)
{
    return lhs > rhs;
}

inline
bool
DecimalImpUtil_IbmXlc::greater(DecimalImpUtil_IbmXlc::ValueType128 lhs,
                               DecimalImpUtil_IbmXlc::ValueType128 rhs)
{
    return lhs > rhs;
}


inline
bool
DecimalImpUtil_IbmXlc::lessEqual(DecimalImpUtil_IbmXlc::ValueType32 lhs,
                                 DecimalImpUtil_IbmXlc::ValueType32 rhs)
{
    return lhs <= rhs;
}

inline
bool
DecimalImpUtil_IbmXlc::lessEqual(DecimalImpUtil_IbmXlc::ValueType64 lhs,
                                 DecimalImpUtil_IbmXlc::ValueType64 rhs)
{
    return lhs <= rhs;
}

inline
bool
DecimalImpUtil_IbmXlc::lessEqual(DecimalImpUtil_IbmXlc::ValueType128 lhs,
                                 DecimalImpUtil_IbmXlc::ValueType128 rhs)
{
    return lhs <= rhs;
}


inline
bool
DecimalImpUtil_IbmXlc::greaterEqual(DecimalImpUtil_IbmXlc::ValueType32 lhs,
                                    DecimalImpUtil_IbmXlc::ValueType32 rhs)
{
    return lhs >= rhs;
}

inline
bool
DecimalImpUtil_IbmXlc::greaterEqual(DecimalImpUtil_IbmXlc::ValueType64 lhs,
                                    DecimalImpUtil_IbmXlc::ValueType64 rhs)
{
    return lhs >= rhs;
}

inline
bool
DecimalImpUtil_IbmXlc::greaterEqual(DecimalImpUtil_IbmXlc::ValueType128 lhs,
                                    DecimalImpUtil_IbmXlc::ValueType128 rhs)
{
    return lhs >= rhs;
}


inline
bool
DecimalImpUtil_IbmXlc::equal(DecimalImpUtil_IbmXlc::ValueType32 lhs,
                             DecimalImpUtil_IbmXlc::ValueType32 rhs)
{
    return lhs == rhs;
}

inline
bool
DecimalImpUtil_IbmXlc::equal(DecimalImpUtil_IbmXlc::ValueType64 lhs,
                             DecimalImpUtil_IbmXlc::ValueType64 rhs)
{
    return lhs == rhs;
}

inline
bool
DecimalImpUtil_IbmXlc::equal(DecimalImpUtil_IbmXlc::ValueType128 lhs,
                             DecimalImpUtil_IbmXlc::ValueType128 rhs)
{
    return lhs == rhs;
}


inline
bool
DecimalImpUtil_IbmXlc::notEqual(DecimalImpUtil_IbmXlc::ValueType32 lhs,
                                DecimalImpUtil_IbmXlc::ValueType32 rhs)
{
    return lhs != rhs;
}

inline
bool
DecimalImpUtil_IbmXlc::notEqual(DecimalImpUtil_IbmXlc::ValueType64 lhs,
                                DecimalImpUtil_IbmXlc::ValueType64 rhs)
{
    return lhs != rhs;
}

inline
bool
DecimalImpUtil_IbmXlc::notEqual(DecimalImpUtil_IbmXlc::ValueType128 lhs,
                                DecimalImpUtil_IbmXlc::ValueType128 rhs)
{
    return lhs != rhs;
}


inline
DecimalImpUtil_IbmXlc::ValueType32
DecimalImpUtil_IbmXlc::convertToDecimal32(
                               const DecimalImpUtil_IbmXlc::ValueType64& input)
{
    return static_cast<ValueType32>(input);
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::convertToDecimal64(
                               const DecimalImpUtil_IbmXlc::ValueType32& input)
{
    return input;
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::convertToDecimal64(
                              const DecimalImpUtil_IbmXlc::ValueType128& input)
{
    return static_cast<ValueType64>(input);
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::convertToDecimal128(
                               const DecimalImpUtil_IbmXlc::ValueType32& input)
{
    return input;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::convertToDecimal128(
                               const DecimalImpUtil_IbmXlc::ValueType64& input)
{
    return input;
}


inline
DecimalImpUtil_IbmXlc::ValueType32
DecimalImpUtil_IbmXlc::binaryToDecimal32(float input)
{
    return static_cast<ValueType32>(input);
}

inline
DecimalImpUtil_IbmXlc::ValueType32
DecimalImpUtil_IbmXlc::binaryToDecimal32(double input)
{
    return static_cast<ValueType32>(input);
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::binaryToDecimal64(float input)
{
    return static_cast<ValueType64>(input);
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::binaryToDecimal64(double input)
{
    return static_cast<ValueType64>(input);
}


inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::binaryToDecimal128(float input)
{
    return static_cast<ValueType128>(input);
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::binaryToDecimal128(double input)
{
    return static_cast<ValueType128>(input);
}

inline
DecimalImpUtil_IbmXlc::ValueType32
DecimalImpUtil_IbmXlc::makeDecimalRaw32(int significand,
                                        int exponent)
{
    DecimalImpUtil_IbmXlc::ValueType32 result;
    result = DecimalImpUtil_IbmXlc::int32ToDecimal32(significand);
    result = DecimalImpUtil_IbmXlc::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::makeDecimalRaw64(unsigned long long significand,
                                        int                exponent)
{
    DecimalImpUtil_IbmXlc::ValueType64 result;
    result = DecimalImpUtil_IbmXlc::uint64ToDecimal64(significand);
    result = DecimalImpUtil_IbmXlc::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::makeDecimalRaw64(long long significand,
                                        int       exponent)
{
    DecimalImpUtil_IbmXlc::ValueType64 result;
    result = DecimalImpUtil_IbmXlc::int64ToDecimal64(significand);
    result = DecimalImpUtil_IbmXlc::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::makeDecimalRaw64(unsigned int significand,
                                        int          exponent)
{
    DecimalImpUtil_IbmXlc::ValueType64 result;
    result = DecimalImpUtil_IbmXlc::uint32ToDecimal64(significand);
    result = DecimalImpUtil_IbmXlc::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IbmXlc::ValueType64 DecimalImpUtil_IbmXlc::makeDecimalRaw64(
                                                               int significand,
                                                               int exponent)
{
    DecimalImpUtil_IbmXlc::ValueType64 result;
    result = DecimalImpUtil_IbmXlc::int32ToDecimal64(significand);
    result = DecimalImpUtil_IbmXlc::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::makeDecimalRaw128(unsigned long long significand,
                                         int                exponent)
{
    DecimalImpUtil_IbmXlc::ValueType128 result;
    result = DecimalImpUtil_IbmXlc::uint64ToDecimal128(significand);
    result = DecimalImpUtil_IbmXlc::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::makeDecimalRaw128(long long significand,
                                         int       exponent)
{
    DecimalImpUtil_IbmXlc::ValueType128 result;
    result = DecimalImpUtil_IbmXlc::int64ToDecimal128(significand);
    result = DecimalImpUtil_IbmXlc::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::makeDecimalRaw128(unsigned int significand,
                                         int          exponent)
{
    DecimalImpUtil_IbmXlc::ValueType128 result;
    result = DecimalImpUtil_IbmXlc::uint32ToDecimal128(significand);
    result = DecimalImpUtil_IbmXlc::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::makeDecimalRaw128(int significand,
                                         int exponent)
{
    DecimalImpUtil_IbmXlc::ValueType128 result;
    result = DecimalImpUtil_IbmXlc::uint32ToDecimal128(significand);
    result = DecimalImpUtil_IbmXlc::scaleB(result, exponent);
    return result;
}


inline
DecimalImpUtil_IbmXlc::ValueType32
DecimalImpUtil_IbmXlc::scaleB(DecimalImpUtil_IbmXlc::ValueType32 value,
                              int                                power)
{
    return convertToDecimal32(scaleB(convertToDecimal64(value), power));
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::scaleB(DecimalImpUtil_IbmXlc::ValueType64 value,
                              int                                power)
{
    return scalblnd64(value, power);
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::scaleB(DecimalImpUtil_IbmXlc::ValueType128 value,
                              int                                 power)
{
    return scalblnd128(value, power);
}


inline
DecimalImpUtil_IbmXlc::ValueType32
DecimalImpUtil_IbmXlc::parse32(const char *str)
{
    DecimalImpUtil_IbmXlc::ValueType32 result;

    int parsed = sscanf(str, "%Hf", &result);
    (void) parsed;

    return result;
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::parse64(const char *str)
{
    DecimalImpUtil_IbmXlc::ValueType64 result;

    int parsed = sscanf(str, "%Df", &result);
    (void) parsed;

    return result;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::parse128(const char *str)
{
    DecimalImpUtil_IbmXlc::ValueType128 result;

    int parsed = sscanf(str, "%DDf", &result);
    (void) parsed;

    return result;
}

                        // Formatting Functions

static
void
DecimalImpUtil_IbmXlc::format(DecimalImpUtil_IbmXlc::ValueType32  value,
                              char                               *buffer)
{
    BSLS_ASSERT(buffer);

    return decSingleToString(value, buffer);
}

static
void
DecimalImpUtil_IbmXlc::format(DecimalImpUtil_IbmXlc::ValueType64  value,
                              char                               *buffer)
{
    BSLS_ASSERT(buffer);

    return decDoubleToString(value, buffer);
}

static
void
DecimalImpUtil_IbmXlc::format(DecimalImpUtil_IbmXlc::ValueType128  value,
                              char                                *buffer)
{
    BSLS_ASSERT(buffer);

    return decQuadToString(value, buffer);
}

                        // Densely Packed Conversion Functions

inline
DecimalImpUtil_IbmXlc::ValueType32
DecimalImpUtil_IbmXlc::convertFromDPD(
                                DenselyPackedDecimalImpUtil::StorageType32 dpd)
{
    DecimalImpUtil_IbmXlc::ValueType32 value;
    bsl::memcpy(&value, &dpd, sizeof(value));

    return value;
}

inline
DecimalImpUtil_IbmXlc::ValueType64
DecimalImpUtil_IbmXlc::convertFromDPD(
                                DenselyPackedDecimalImpUtil::StorageType64 dpd)
{
    DecimalImpUtil_IbmXlc::ValueType64 value;
    bsl::memcpy(&value, &dpd, sizeof(value));

    return value;
}

inline
DecimalImpUtil_IbmXlc::ValueType128
DecimalImpUtil_IbmXlc::convertFromDPD(
                               DenselyPackedDecimalImpUtil::StorageType128 dpd)
{
    DecimalImpUtil_IbmXlc::ValueType128 value;
    bsl::memcpy(&value, &dpd, sizeof(value));

    return value;
}

inline
DenselyPackedDecimalImpUtil::StorageType32
DecimalImpUtil_IbmXlc::convertToDPD(
                                      DecimalImpUtil_IbmXlc::ValueType32 value)
{
    DenselyPackedDecimalImpUtil::StorageType32 dpd;
    bsl::memcpy(&dpd, &value, sizeof(dpd));

    return dpd;
}

inline
DenselyPackedDecimalImpUtil::StorageType64
DecimalImpUtil_IbmXlc::convertToDPD(
                                      DecimalImpUtil_IbmXlc::ValueType64 value)
{
    DenselyPackedDecimalImpUtil::StorageType64 dpd;
    bsl::memcpy(&dpd, &value, sizeof(dpd));

    return dpd;
}

inline
DenselyPackedDecimalImpUtil::StorageType128
DecimalImpUtil_IbmXlc::convertToDPD(
                                     DecimalImpUtil_IbmXlc::ValueType128 value)
{
    DenselyPackedDecimalImpUtil::StorageType128 dpd;
    bsl::memcpy(&dpd, &value, sizeof(dpd));

    return dpd;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
