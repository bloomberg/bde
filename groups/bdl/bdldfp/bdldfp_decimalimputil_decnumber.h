// bdldfp_decimalimputil_decnumber.h                                  -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL_DECNUMBER
#define INCLUDED_BDLDFP_DECIMALIMPUTIL_DECNUMBER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide utility implementing decimal FP on the decNumber library.
//
//@CLASSES:
//  bdldfp::DecimalImpUtil_DecNumber: Namespace for decNumber FP functions
//
//@SEE_ALSO: bdldfp_decimal, bdldfp_decimalplatform
//
//@DESCRIPTION: This component, 'bdldfp::DecimalImpUtil_DecNumber' is for
// internal use only by the 'bdldfp_decimal*' components.  Direct use of any
// names declared in this component by any other code invokes undefined
// behavior.  In other words: this code may change, disappear, break, move
// without notice, and no support whatsoever will ever be provided for it.
// This component provides implementations of core Decimal Floating Point
// functionality using the Intel DFP library.
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

#ifndef INCLUDED_BSL_LOCALE
#include <bsl_locale.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_DECSINGLE
extern "C" {
#include <decSingle.h>
}
#endif


namespace BloombergLP {
namespace bdldfp {

                          // ===============================
                          // class DecimalImplUtil_DecNumber
                          // ===============================

struct DecimalImpUtil_DecNumber {
    // This 'struct' provides a namespace for implementation functions that
    // work in terms of the underlying C-style decimal floating point
    // implementation, decNumber.

    // TYPES

    typedef decSingle ValueType32;
    typedef decDouble ValueType64;
    typedef decQuad   ValueType128;

    static decContext *getDecNumberContext();
        // Return a pointer providing modifiable access to the floating point
        // environment of the 'decNumber' library.  This function does not get
        // exposed through 'bdldfp::DecimalImpUtil'.



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
    static ValueType32 binaryToDecimal32(long double value);
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
    static ValueType64 binaryToDecimal64(long double value);
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
    static ValueType128 binaryToDecimal128(long double value);
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

    static ValueType32 makeDecimalRaw32(int mantissa, int exponent);
        // Create a 'ValueType32' object representing a decimal floating point
        // number consisting of the specified 'mantissa' and 'exponent', with
        // the sign given by 'mantissa'.  The behavior is undefined unless
        // 'abs(mantissa) <= 9,999,999' and '-101 <= exponent <= 90'.

    static ValueType64 makeDecimalRaw64(unsigned long long int mantissa,
                                                           int exponent);
    static ValueType64 makeDecimalRaw64(         long long int mantissa,
                                                           int exponent);
    static ValueType64 makeDecimalRaw64(unsigned           int mantissa,
                                                           int exponent);
    static ValueType64 makeDecimalRaw64(                   int mantissa,
                                                           int exponent);
        // Create a 'ValueType64' object representing a decimal floating point
        // number consisting of the specified 'mantissa' and 'exponent', with
        // the sign given by 'mantissa'.  The behavior is undefined unless
        // 'abs(mantissa) <= 9,999,999,999,999,999' and
        // '-398 <= exponent <= 369'.

    static ValueType128 makeDecimalRaw128(unsigned long long int mantissa,
                                                             int exponent);
    static ValueType128 makeDecimalRaw128(         long long int mantissa,
                                                             int exponent);
    static ValueType128 makeDecimalRaw128(unsigned           int mantissa,
                                                             int exponent);
    static ValueType128 makeDecimalRaw128(                   int mantissa,
                                                             int exponent);
        // Create a 'ValueType128' object representing a decimal floating point
        // number consisting of the specified 'mantissa' and 'exponent', with
        // the sign given by 'mantissa'.  The behavior is undefined unless
        // '-6176 <= exponent <= 6111'.

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
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

inline decContext *DecimalImpUtil_DecNumber::getDecNumberContext()
{
    static decContext context = { 0, 0, 0, DEC_ROUND_HALF_EVEN, 0, 0, 0 };
    return &context;
}

                        // Integer construction

inline
DecimalImpUtil_DecNumber::ValueType32
DecimalImpUtil_DecNumber::int32ToDecimal32(int value)
{
    union {
        DecimalImpUtil_DecNumber::ValueType32      result;
        DenselyPackedDecimalImpUtil::StorageType32 raw;
    } rawAccess;
    rawAccess.raw = DenselyPackedDecimalImpUtil::makeDecimalRaw32(value, 0);

    return rawAccess.result;
}

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::int32ToDecimal64(int value)
{
    union {
        DecimalImpUtil_DecNumber::ValueType64      result;
        DenselyPackedDecimalImpUtil::StorageType64 raw;
    } rawAccess;
    rawAccess.raw = DenselyPackedDecimalImpUtil::makeDecimalRaw64(value, 0);

    return rawAccess.result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::int32ToDecimal128(int value)
{
    DecimalImpUtil_DecNumber::ValueType128      result;
    DenselyPackedDecimalImpUtil::StorageType128 raw;
    raw = DenselyPackedDecimalImpUtil::makeDecimalRaw128(value, 0);
    bsl::memcpy(&result, &raw, sizeof(raw));

    return result;
}


inline
DecimalImpUtil_DecNumber::ValueType32
DecimalImpUtil_DecNumber::uint32ToDecimal32(unsigned int value)
{
    union {
        DecimalImpUtil_DecNumber::ValueType32      result;
        DenselyPackedDecimalImpUtil::StorageType32 raw;
    } rawAccess;
    rawAccess.raw = DenselyPackedDecimalImpUtil::makeDecimalRaw32(value, 0);

    return rawAccess.result;
}

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::uint32ToDecimal64(unsigned int value)
{
    union {
        DecimalImpUtil_DecNumber::ValueType64      result;
        DenselyPackedDecimalImpUtil::StorageType64 raw;
    } rawAccess;
    rawAccess.raw = DenselyPackedDecimalImpUtil::makeDecimalRaw64(value, 0);

    return rawAccess.result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::uint32ToDecimal128(unsigned int value)
{
    DecimalImpUtil_DecNumber::ValueType128      result;
    DenselyPackedDecimalImpUtil::StorageType128 raw;
    raw = DenselyPackedDecimalImpUtil::makeDecimalRaw128(value, 0);
    bsl::memcpy(&result, &raw, sizeof(raw));

    return result;
}


inline
DecimalImpUtil_DecNumber::ValueType32
DecimalImpUtil_DecNumber::int64ToDecimal32(long long int value)
{
    union {
        DecimalImpUtil_DecNumber::ValueType32      result;
        DenselyPackedDecimalImpUtil::StorageType32 raw;
    } rawAccess;
    rawAccess.raw = DenselyPackedDecimalImpUtil::makeDecimalRaw32(value, 0);

    return rawAccess.result;
}

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::int64ToDecimal64(long long int value)
{
    union {
        DecimalImpUtil_DecNumber::ValueType64      result;
        DenselyPackedDecimalImpUtil::StorageType64 raw;
    } rawAccess;
    rawAccess.raw = DenselyPackedDecimalImpUtil::makeDecimalRaw64(value, 0);

    return rawAccess.result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::int64ToDecimal128(long long int value)
{
    DecimalImpUtil_DecNumber::ValueType128      result;
    DenselyPackedDecimalImpUtil::StorageType128 raw;
    raw = DenselyPackedDecimalImpUtil::makeDecimalRaw128(value, 0);
    bsl::memcpy(&result, &raw, sizeof(raw));

    return result;
}


inline
DecimalImpUtil_DecNumber::ValueType32
DecimalImpUtil_DecNumber::uint64ToDecimal32(unsigned long long int value)
{
    union {
        DecimalImpUtil_DecNumber::ValueType32      result;
        DenselyPackedDecimalImpUtil::StorageType32 raw;
    } rawAccess;
    rawAccess.raw = DenselyPackedDecimalImpUtil::makeDecimalRaw32(value, 0);

    return rawAccess.result;
}

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::uint64ToDecimal64(unsigned long long int value)
{
    union {
        DecimalImpUtil_DecNumber::ValueType64      result;
        DenselyPackedDecimalImpUtil::StorageType64 raw;
    } rawAccess;
    rawAccess.raw = DenselyPackedDecimalImpUtil::makeDecimalRaw64(value, 0);

    return rawAccess.result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::uint64ToDecimal128(unsigned long long int value)
{
    DecimalImpUtil_DecNumber::ValueType128      result;
    DenselyPackedDecimalImpUtil::StorageType128 raw;
    raw = DenselyPackedDecimalImpUtil::makeDecimalRaw128(value, 0);
    bsl::memcpy(&result, &raw, sizeof(raw));

    return result;
}

                        // Arithmetic

                        // Addition Functions

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::add(DecimalImpUtil_DecNumber::ValueType64 lhs,
                              DecimalImpUtil_DecNumber::ValueType64 rhs)
{
    DecimalImpUtil_DecNumber::ValueType64 result;
    decDoubleAdd(&result, &lhs, &rhs, getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::add(DecimalImpUtil_DecNumber::ValueType128 lhs,
                              DecimalImpUtil_DecNumber::ValueType128 rhs)
{
    DecimalImpUtil_DecNumber::ValueType128 result;
    decQuadAdd(&result, &lhs, &rhs, getDecNumberContext());
    return result;
}

                        // Subtraction Functions

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::subtract(DecimalImpUtil_DecNumber::ValueType64 lhs,
                                   DecimalImpUtil_DecNumber::ValueType64 rhs)
{
    DecimalImpUtil_DecNumber::ValueType64 result;
    decDoubleSubtract(&result, &lhs, &rhs, getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::subtract(DecimalImpUtil_DecNumber::ValueType128 lhs,
                                   DecimalImpUtil_DecNumber::ValueType128 rhs)
{
    DecimalImpUtil_DecNumber::ValueType128 result;
    decQuadSubtract(&result, &lhs, &rhs, getDecNumberContext());
    return result;
}

                        // Multiplication Functions

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::multiply(DecimalImpUtil_DecNumber::ValueType64 lhs,
                                   DecimalImpUtil_DecNumber::ValueType64 rhs)
{
    DecimalImpUtil_DecNumber::ValueType64 result;
    decDoubleMultiply(&result, &lhs, &rhs, getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::multiply(DecimalImpUtil_DecNumber::ValueType128 lhs,
                                   DecimalImpUtil_DecNumber::ValueType128 rhs)
{
    DecimalImpUtil_DecNumber::ValueType128 result;
    decQuadMultiply(&result, &lhs, &rhs, getDecNumberContext());
    return result;
}

                        // Division Functions

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::divide(DecimalImpUtil_DecNumber::ValueType64 lhs,
                                 DecimalImpUtil_DecNumber::ValueType64 rhs)
{
    DecimalImpUtil_DecNumber::ValueType64 result;
    decDoubleDivide(&result, &lhs, &rhs, getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::divide(DecimalImpUtil_DecNumber::ValueType128 lhs,
                                 DecimalImpUtil_DecNumber::ValueType128 rhs)
{
    DecimalImpUtil_DecNumber::ValueType128 result;
    decQuadDivide(&result, &lhs, &rhs, getDecNumberContext());
    return result;
}

                        // Negation Functions

inline
DecimalImpUtil_DecNumber::ValueType32
DecimalImpUtil_DecNumber::negate(DecimalImpUtil_DecNumber::ValueType32 value)
{
    DecimalImpUtil_DecNumber::ValueType32 result;
    decSingleCopyNegate(&result, &value);
    return result;
}

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::negate(DecimalImpUtil_DecNumber::ValueType64 value)
{
    DecimalImpUtil_DecNumber::ValueType64 result;
    decDoubleCopyNegate(&result, &value);
    return result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::negate(DecimalImpUtil_DecNumber::ValueType128 value)
{
    DecimalImpUtil_DecNumber::ValueType128 result;
    decQuadCopyNegate(&result, &value);
    return result;
}

                        // Comparison Functions

                        // Less Than Functions

inline
bool
DecimalImpUtil_DecNumber::less(DecimalImpUtil_DecNumber::ValueType32 lhs,
                               DecimalImpUtil_DecNumber::ValueType32 rhs)
{
    return less(convertToDecimal64(lhs), convertToDecimal64(rhs));
}

inline
bool
DecimalImpUtil_DecNumber::less(DecimalImpUtil_DecNumber::ValueType64 lhs,
                               DecimalImpUtil_DecNumber::ValueType64 rhs)
{
    ValueType64 comparison;
    decDoubleCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decDoubleIsNegative(&comparison);
}

inline
bool
DecimalImpUtil_DecNumber::less(DecimalImpUtil_DecNumber::ValueType128 lhs,
                               DecimalImpUtil_DecNumber::ValueType128 rhs)
{
    ValueType128 comparison;
    decQuadCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decQuadIsNegative(&comparison);
}

                        // Greater Than Functions

inline
bool
DecimalImpUtil_DecNumber::greater(DecimalImpUtil_DecNumber::ValueType32 lhs,
                                  DecimalImpUtil_DecNumber::ValueType32 rhs)
{
    return greater(convertToDecimal64(lhs), convertToDecimal64(rhs));
}

inline
bool
DecimalImpUtil_DecNumber::greater(DecimalImpUtil_DecNumber::ValueType64 lhs,
                                  DecimalImpUtil_DecNumber::ValueType64 rhs)
{
    ValueType64 comparison;
    decDoubleCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decDoubleIsPositive(&comparison);
}

inline
bool
DecimalImpUtil_DecNumber::greater(DecimalImpUtil_DecNumber::ValueType128 lhs,
                                  DecimalImpUtil_DecNumber::ValueType128 rhs)
{
    ValueType128 comparison;
    decQuadCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decQuadIsPositive(&comparison);
}

                        // Less Or Equal Functions

inline
bool
DecimalImpUtil_DecNumber::lessEqual(DecimalImpUtil_DecNumber::ValueType32 lhs,
                                    DecimalImpUtil_DecNumber::ValueType32 rhs)
{
    return lessEqual(convertToDecimal64(lhs), convertToDecimal64(rhs));
}

inline
bool
DecimalImpUtil_DecNumber::lessEqual(DecimalImpUtil_DecNumber::ValueType64 lhs,
                                    DecimalImpUtil_DecNumber::ValueType64 rhs)
{
    ValueType64 comparison;
    decDoubleCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decDoubleIsNegative(&comparison) || decDoubleIsZero(&comparison);
}

inline
bool
DecimalImpUtil_DecNumber::lessEqual(DecimalImpUtil_DecNumber::ValueType128 lhs,
                                    DecimalImpUtil_DecNumber::ValueType128 rhs)
{
    ValueType128 comparison;
    decQuadCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decQuadIsNegative(&comparison) || decQuadIsZero(&comparison);
}

                        // Greater Or Equal Functions

inline
bool
DecimalImpUtil_DecNumber::greaterEqual(
                                     DecimalImpUtil_DecNumber::ValueType32 lhs,
                                     DecimalImpUtil_DecNumber::ValueType32 rhs)
{
    return greaterEqual(convertToDecimal64(lhs), convertToDecimal64(rhs));
}

inline
bool
DecimalImpUtil_DecNumber::greaterEqual(
                                     DecimalImpUtil_DecNumber::ValueType64 lhs,
                                     DecimalImpUtil_DecNumber::ValueType64 rhs)
{
    ValueType64 comparison;
    decDoubleCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decDoubleIsPositive(&comparison) || decDoubleIsZero(&comparison);
}

inline
bool
DecimalImpUtil_DecNumber::greaterEqual(
                                    DecimalImpUtil_DecNumber::ValueType128 lhs,
                                    DecimalImpUtil_DecNumber::ValueType128 rhs)
{
    ValueType128 comparison;
    decQuadCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decQuadIsPositive(&comparison) || decQuadIsZero(&comparison);
}

                        // Equality Functions

inline
bool
DecimalImpUtil_DecNumber::equal(DecimalImpUtil_DecNumber::ValueType32 lhs,
                                DecimalImpUtil_DecNumber::ValueType32 rhs)
{
    return equal(convertToDecimal64(lhs), convertToDecimal64(rhs));
}

inline
bool
DecimalImpUtil_DecNumber::equal(DecimalImpUtil_DecNumber::ValueType64 lhs,
                                DecimalImpUtil_DecNumber::ValueType64 rhs)
{
    ValueType64 comparison;
    decDoubleCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decDoubleIsZero(&comparison);
}

inline
bool
DecimalImpUtil_DecNumber::equal(DecimalImpUtil_DecNumber::ValueType128 lhs,
                                DecimalImpUtil_DecNumber::ValueType128 rhs)
{
    ValueType128 comparison;
    decQuadCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decQuadIsZero(&comparison);
}

                        // Inequality Functions

inline
bool
DecimalImpUtil_DecNumber::notEqual(DecimalImpUtil_DecNumber::ValueType32 lhs,
                                   DecimalImpUtil_DecNumber::ValueType32 rhs)
{
    return notEqual(convertToDecimal64(lhs), convertToDecimal64(rhs));
}

inline
bool
DecimalImpUtil_DecNumber::notEqual(DecimalImpUtil_DecNumber::ValueType64 lhs,
                                   DecimalImpUtil_DecNumber::ValueType64 rhs)
{
    ValueType64 comparison;
    decDoubleCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decDoubleIsPositive(&comparison)
        || decDoubleIsNegative(&comparison);
}

inline
bool
DecimalImpUtil_DecNumber::notEqual(DecimalImpUtil_DecNumber::ValueType128 lhs,
                                   DecimalImpUtil_DecNumber::ValueType128 rhs)
{
    ValueType128 comparison;
    decQuadCompare(&comparison, &lhs, &rhs, getDecNumberContext());
    return decQuadIsPositive(&comparison) || decQuadIsNegative(&comparison);
}

                        // Inter-type Conversion functions

inline
DecimalImpUtil_DecNumber::ValueType32
DecimalImpUtil_DecNumber::convertToDecimal32(
                            const DecimalImpUtil_DecNumber::ValueType64& input)
{
    DecimalImpUtil_DecNumber::ValueType32 retval;
    decSingleFromWider(&retval, &input, getDecNumberContext());
    return retval;
}

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::convertToDecimal64(
                            const DecimalImpUtil_DecNumber::ValueType32& input)
{
    DecimalImpUtil_DecNumber::ValueType64 retval;
    decSingleToWider(&input, &retval);
    return retval;
}

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::convertToDecimal64(
                           const DecimalImpUtil_DecNumber::ValueType128& input)
{
    DecimalImpUtil_DecNumber::ValueType64 retval;
    decDoubleFromWider(&retval, &input, getDecNumberContext());
    return retval;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::convertToDecimal128(
                            const DecimalImpUtil_DecNumber::ValueType32& input)
{
    return convertToDecimal128(convertToDecimal64(input));
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::convertToDecimal128(
                            const DecimalImpUtil_DecNumber::ValueType64& input)
{
    DecimalImpUtil_DecNumber::ValueType128 retval;
    decDoubleToWider(&input, &retval);
    return retval;
}

                        // makeDecimalRaw Functions

inline
DecimalImpUtil_DecNumber::ValueType32
DecimalImpUtil_DecNumber::makeDecimalRaw32(int mantissa,
                                           int exponent)
{
    union {
        DecimalImpUtil_DecNumber::ValueType32      result;
        DenselyPackedDecimalImpUtil::StorageType32 raw;
    } rawAccess;
    rawAccess.raw = DenselyPackedDecimalImpUtil::makeDecimalRaw32(mantissa,
                                                                  exponent);

    return rawAccess.result;
}

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::makeDecimalRaw64(unsigned long long mantissa,
                                           int                exponent)
{
    union {
        DecimalImpUtil_DecNumber::ValueType64      result;
        DenselyPackedDecimalImpUtil::StorageType64 raw;
    } rawAccess;
    rawAccess.raw = DenselyPackedDecimalImpUtil::makeDecimalRaw64(mantissa,
                                                                  exponent);

    return rawAccess.result;
}

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::makeDecimalRaw64(long long mantissa,
                                           int       exponent)
{
    union {
        DecimalImpUtil_DecNumber::ValueType64      result;
        DenselyPackedDecimalImpUtil::StorageType64 raw;
    } rawAccess;
    rawAccess.raw = DenselyPackedDecimalImpUtil::makeDecimalRaw64(mantissa,
                                                                  exponent);

    return rawAccess.result;
}

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::makeDecimalRaw64(unsigned int mantissa,
                                           int          exponent)
{
    union {
        DecimalImpUtil_DecNumber::ValueType64      result;
        DenselyPackedDecimalImpUtil::StorageType64 raw;
    } rawAccess;
    rawAccess.raw = DenselyPackedDecimalImpUtil::makeDecimalRaw64(mantissa,
                                                                  exponent);

    return rawAccess.result;
}

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::makeDecimalRaw64(int mantissa,
                                           int exponent)
{
    union {
        DecimalImpUtil_DecNumber::ValueType64      result;
        DenselyPackedDecimalImpUtil::StorageType64 raw;
    } rawAccess;
    rawAccess.raw = DenselyPackedDecimalImpUtil::makeDecimalRaw64(mantissa,
                                                                  exponent);

    return rawAccess.result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::makeDecimalRaw128(unsigned long long mantissa,
                                            int                exponent)
{
    DecimalImpUtil_DecNumber::ValueType128      result;
    DenselyPackedDecimalImpUtil::StorageType128 raw;
    raw = DenselyPackedDecimalImpUtil::makeDecimalRaw128(mantissa, exponent);
    bsl::memcpy(&result, &raw, sizeof(result));

    return result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::makeDecimalRaw128(long long mantissa,
                                            int       exponent)
{
    DecimalImpUtil_DecNumber::ValueType128      result;
    DenselyPackedDecimalImpUtil::StorageType128 raw;
    raw = DenselyPackedDecimalImpUtil::makeDecimalRaw128(mantissa, exponent);
    bsl::memcpy(&result, &raw, sizeof(result));

    return result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::makeDecimalRaw128(unsigned int mantissa,
                                            int          exponent)
{
    DecimalImpUtil_DecNumber::ValueType128      result;
    DenselyPackedDecimalImpUtil::StorageType128 raw;
    raw = DenselyPackedDecimalImpUtil::makeDecimalRaw128(mantissa, exponent);
    bsl::memcpy(&result, &raw, sizeof(result));

    return result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::makeDecimalRaw128(int mantissa,
                                            int exponent)
{
    DecimalImpUtil_DecNumber::ValueType128      result;
    DenselyPackedDecimalImpUtil::StorageType128 raw;
    raw = DenselyPackedDecimalImpUtil::makeDecimalRaw128(mantissa, exponent);
    bsl::memcpy(&result, &raw, sizeof(result));

    return result;
}

                        // IEEE Scale B Functions

inline
DecimalImpUtil_DecNumber::ValueType32
DecimalImpUtil_DecNumber::scaleB(
                                DecimalImpUtil_DecNumber::ValueType32 value,
                                int                                   exponent)
{
    return convertToDecimal32(scaleB(convertToDecimal64(value), exponent));
}

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::scaleB(
                                DecimalImpUtil_DecNumber::ValueType64 value,
                                int                                   exponent)
{
    DecimalImpUtil_DecNumber::ValueType64 result;
    DecimalImpUtil_DecNumber::ValueType64 decPower;
    decDoubleFromInt32(&decPower, exponent);
    decDoubleScaleB(&result, &value, &decPower, getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::scaleB(
                               DecimalImpUtil_DecNumber::ValueType128 value,
                               int                                    exponent)
{
    DecimalImpUtil_DecNumber::ValueType128 result;
    DecimalImpUtil_DecNumber::ValueType128 decPower;
    decQuadFromInt32(&decPower, exponent);
    decQuadScaleB(&result, &value, &decPower, getDecNumberContext());
    return result;
}

                        // Parsing functions

inline
DecimalImpUtil_DecNumber::ValueType32
DecimalImpUtil_DecNumber::parse32(const char *string)
{
    DecimalImpUtil_DecNumber::ValueType32 result;
    decSingleFromString(&result, string, getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_DecNumber::ValueType64
DecimalImpUtil_DecNumber::parse64(const char *string)
{
    DecimalImpUtil_DecNumber::ValueType64 result;
    decDoubleFromString(&result, string, getDecNumberContext());
    return result;
}

inline
DecimalImpUtil_DecNumber::ValueType128
DecimalImpUtil_DecNumber::parse128(const char *string)
{
    DecimalImpUtil_DecNumber::ValueType128 result;
    decQuadFromString(&result, string, getDecNumberContext());
    return result;
}

inline
void
DecimalImpUtil_DecNumber::format(DecimalImpUtil_DecNumber::ValueType32  value,
                                 char                                  *buffer)
{
    BSLS_ASSERT(buffer);

    decSingleToString(&value, buffer);
}

inline
void
DecimalImpUtil_DecNumber::format(DecimalImpUtil_DecNumber::ValueType64  value,
                                 char                                  *buffer)
{
    BSLS_ASSERT(buffer);

    decDoubleToString(&value, buffer);
}

inline
void
DecimalImpUtil_DecNumber::format(DecimalImpUtil_DecNumber::ValueType128  value,
                                 char                                   *buffer)
{
    BSLS_ASSERT(buffer);

    decQuadToString(&value, buffer);
}


}  // close package namespace
}  // close enterprise namespace


#endif

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
