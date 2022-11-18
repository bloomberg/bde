// bdldfp_decimalimputil_inteldfp.h                                   -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL_INTELDFP
#define INCLUDED_BDLDFP_DECIMALIMPUTIL_INTELDFP

#include <bsls_ident.h>
BSLS_IDENT("$Id$")

//@PURPOSE: Provide utility to implement decimal 'float's on the Intel library.
//
//@CLASSES:
//  bdldfp::DecimalImpUtil_IntelDfp: Namespace for Intel decimal FP functions
//
//@SEE_ALSO: bdldfp_decimal, bdldfp_decimalplatform
//
//@DESCRIPTION: This component, 'bdldfp::DecimalImpUtil_IntelDfp' is for
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

#include <bdlscm_version.h>

#include <bdldfp_decimalplatform.h>
#include <bdldfp_decimalstorage.h>

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
#include <bdldfp_intelimpwrapper.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>
#include <bsls_assert.h>

#include <bsl_locale.h>
#include <bsl_cstring.h>
#include <bsl_c_errno.h>

namespace BloombergLP {
namespace bdldfp {

                          // ==============================
                          // class DecimalImplUtil_IntelDfp
                          // ==============================

struct DecimalImpUtil_IntelDfp {
    // This 'struct' provides a namespace for implementation functions that
    // work in terms of the underlying C-style decimal floating point
    // implementation, Intel's DFP library.

    // TYPES
    struct ValueType32  { BID_UINT32  d_raw; };
    struct ValueType64  { BID_UINT64  d_raw; };
    struct ValueType128 { BID_UINT128 d_raw; };

    enum {
        // Status flag bitmask for numeric operations.

        k_STATUS_INEXACT = BID_INEXACT_EXCEPTION,
        k_STATUS_UNDERFLOW = BID_UNDERFLOW_EXCEPTION,
        k_STATUS_OVERFLOW = BID_OVERFLOW_EXCEPTION
    };

  private:
    // CLASS METHODS
    static void setErrno(_IDEC_flags flags);
        // Convert bit flags from the specified 'flags' into error codes as
        // follows and load the result into a prepocessor macro 'errno':
        //
        //: o flag BID_INVALID_EXCEPTION     => 'errno = EDOM'
        //: o flag BID_OVERFLOW_EXCEPTION    => 'errno = ERANGE'
        //: o flag BID_UNDERFLOW_EXCEPTION   => 'errno = ERANGE'
        //: o flag BID_ZERO_DIVIDE_EXCEPTION => 'errno = ERANGE'

  public:
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
        //: o Otherwise if 'value' has a value that is not exactly
        //:   representable using 'std::numeric_limits<Decimal32>::max_digit'
        //:   decimal digits then return the value rounded according to the
        //:   rounding direction.
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
        //: o Otherwise if 'value' has a value that is not exactly
        //:   representable using 'std::numeric_limits<Decimal64>::max_digit'
        //:   decimal digits then return 'value' rounded according to the
        //:   rounding direction.
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
        //: o Otherwise if 'value' has a value that is not exactly
        //:   representable using 'std::numeric_limits<Decimal128>::max_digit'
        //:   decimal digits then return 'value' rounded according to the
        //:   rounding direction.
        //:
        //: o Otherwise initialize this object to 'value'.
        //
        // The exponent 0 (quantum 1e-33) is preferred during conversion unless
        // it would cause unnecessary loss of precision.

                        // Arithmetic functions

                        // Addition functions

    static ValueType32  add(ValueType32  lhs,  ValueType32  rhs);
    static ValueType64  add(ValueType64  lhs,  ValueType64  rhs);
    static ValueType128 add(ValueType128 lhs,  ValueType128 rhs);
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
        //:   that is larger than max value supported by indicated result type
        //:   then store the value of the macro 'ERANGE' into 'errno' and
        //:   return infinity with the same sign as that result.
        //:
        //: o Otherwise return the sum of the number represented by 'lhs' and
        //:   the number represented by 'rhs'.

                        // Subtraction functions

    static ValueType32  subtract(ValueType32  lhs,  ValueType32  rhs);
    static ValueType64  subtract(ValueType64  lhs,  ValueType64  rhs);
    static ValueType128 subtract(ValueType128 lhs,  ValueType128 rhs);
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
        //:   value that is larger than max value supported by indicated result
        //:   type then store the value of the macro 'ERANGE' into 'errno' and
        //:   return infinity with the same sign as that result.
        //:
        //: o Otherwise return the result of subtracting the value of 'rhs'
        //:   from the value of 'lhs'.

                        // Multiplication functions

    static ValueType32  multiply(ValueType32  lhs,  ValueType32  rhs);
    static ValueType64  multiply(ValueType64  lhs,  ValueType64  rhs);
    static ValueType128 multiply(ValueType128 lhs,  ValueType128 rhs);
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
        //:   that is larger than max value of the indicated result type then
        //:   store the value of the macro 'ERANGE' into 'errno' and return
        //:   infinity with the same sign as that result.
        //:
        //: o Otherwise if the product of 'lhs' and 'rhs' has an absolute value
        //:   that is smaller than min value of the indicated result type then
        //:   store the value of the macro 'ERANGE' into 'errno' and return
        //:   zero with the same sign as that result.
        //:
        //: o Otherwise return the product of the value of 'rhs' and the number
        //:   represented by 'rhs'.

                        // Division functions

    static ValueType32  divide(ValueType32  lhs,  ValueType32  rhs);
    static ValueType64  divide(ValueType64  lhs,  ValueType64  rhs);
    static ValueType128 divide(ValueType128 lhs,  ValueType128 rhs);
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
        //: o Otherwise if 'lhs' has infinity value and 'rhs' has a positive
        //:   zero value, return infinity with the sign of 'lhs'.
        //:
        //: o Otherwise if 'lhs' has infinity value and 'rhs' has a negative
        //:   zero value, return infinity with the opposite sign as 'lhs'.
        //:
        //: o Otherwise if dividing the value of 'lhs' with the value of 'rhs'
        //:   results in an absolute value that is larger than max value
        //:   supported by the return type then store the value of the macro
        //:   'ERANGE' into 'errno' and return infinity with the same sign as
        //:   that result.
        //:
        //: o Otherwise if dividing the value of 'lhs' with the value of 'rhs'
        //:   results in an absolute value that is smaller than min value
        //:   supported by indicated result type then store the value of the
        //:   macro 'ERANGE' into 'errno'and return zero with the same sign as
        //:   that result.
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
        // Convert the specified 'input' to the closest value of indicated
        // result type following the conversion rules:
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
        //:   representable using maximum digit number supported by indicated
        //:   result type then return the 'input' rounded according to the
        //:   rounding direction.
        //:
        //: o Otherwise return 'input' value of the result type.

                        // Binary floating point conversion functions

    static ValueType32 binaryToDecimal32(      float value);
    static ValueType32 binaryToDecimal32(     double value);
        // Create a 'Decimal32' object having the value closest to the
        // specified 'value' following the conversion rules as defined by
        // IEEE-754:
        //
        //: o If 'value' is signaling NaN, store the value of the macro 'EDOM'
        //:   into 'errno' and return signaling NaN value.
        //:
        //: o If 'value' is NaN, return a NaN.
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
        //: o If 'value' is NaN, return a NaN.
        //:
        //: o Otherwise if 'value' is infinity (positive or negative), then
        //:   return an object equal to infinity with the same sign.
        //:
        //: o Otherwise if 'value' is a zero value, then return an object equal
        //:   to zero with the same sign.
        //:
        //: o Otherwise if 'value' needs more than
        //:   'std::numeric_limits<Decimal128>::max_digit' significant decimal
        //:   digits to represent then return the 'value' rounded according to
        //:   the rounding direction.
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
        // unspecified if 'value' is NaN or infinity.

                        // Parsing functions

    static ValueType32 parse32 (const char *string);
        // Parse the specified 'string' as a 32 bit decimal floating-point
        // value and return the result.  The parsing is as specified for the
        // 'strtod32' function in section 9.6 of the ISO/EIC TR 24732 C Decimal
        // Floating-Point Technical Report, except that it is unspecified
        // whether the NaNs returned are quiet or signaling.  If 'string'
        // represents a value that absolute value exceeds the maximum value or
        // is less than the smallest value supported by 'ValueType32' type then
        // store the value of the macro 'ERANGE' into 'errno' and return the
        // value initialized to infinity or zero respectively with the same
        // sign as specified in 'string'.  The behavior is undefined unless
        // 'input' represents a valid 32 bit decimalfloating-point number in
        // scientific or fixed notation, and no unrelated characters precede
        // (not even whitespace) that textual representation and a terminating
        // nul character immediately follows it.  Note that this method does
        // not guarantee the behavior of ISO/EIC TR 24732 C when parsing NaN
        // because the AIX compiler intrinsics return a signaling NaN.

    static ValueType64 parse64(const char *string);
        // Parse the specified 'string' string as a 64 bit decimal floating-
        // point value and return the result.  The parsing is as specified for
        // the 'strtod64' function in section 9.6 of the ISO/EIC TR 24732 C
        // Decimal Floating-Point Technical Report, except that it is
        // unspecified whether the NaNs returned are quiet or signaling.  If
        // 'string' represents a value that absolute value exceeds the maximum
        // value or is less than the smallest value supported by 'ValueType63'
        // type then store the value of the macro 'ERANGE' into 'errno' and
        // return the value initialized to infinity or zero respectively with
        // the same sign as specified in 'string'.  The behavior is undefined
        // unless 'input' represents a valid 64 bit decimal floating-point
        // number in scientific or fixed notation, and no unrelated characters
        // precede (not even whitespace) that textual representation and a
        // terminating nul character immediately follows it.  Note that this
        // method does not guarantee the behavior of ISO/EIC TR 24732 C when
        // parsing NaN because the AIX compiler intrinsics return a signaling
        // NaN.

    static ValueType128 parse128(const char *string);
        // Parse the specified 'string' string as a 128 bit decimal floating-
        // point value and return the result.  The parsing is as specified for
        // the 'strtod128' function in section 9.6 of the ISO/EIC TR 24732 C
        // Decimal Floating-Point Technical Report, except that it is
        // unspecified whether the NaNs returned are quiet or signaling. If
        // 'string' represents a value that absolute value exceeds the maximum
        // value or is less than the smallest value supported by 'ValueType128'
        // type then store the value of the macro 'ERANGE' into 'errno' and
        // return the value initialized to infinity or zero respectively with
        // the same sign as specified in 'string'.  The behavior is undefined
        // unless 'input' represents a valid 128 bit decimal floating-point
        // number in scientific or fixed notation, and no unrelated characters
        // precede (not even whitespace) that textual representation and a
        // terminating null character immediately follows it.  Note that this
        // method does not guarantee the behavior of ISO/EIC TR 24732 C when
        // parsing NaN because the AIX compiler intrinsics return a signaling
        // NaN.

    static ValueType32 parse32(const char *string, unsigned int *status);
    static ValueType64 parse64(const char *string, unsigned int *status);
    static ValueType128 parse128(const char *string, unsigned int *status);
        // Parse the specified 'string' string as a decimal floating-point
        // value and return the result, loading the specified 'status' with a
        // bit mask providing additional status information about the result.
        // The supplied '*status' must be 0, and may be loaded with a bit mask
        // of 'k_STATUS_INEXACT', 'k_STATUS_UNDERFLOW', and 'k_STATUS_OVERFLOW'
        // constants indicating wether the conversion from text inexact,
        // underflowed, or overflowed (or some combination) respectively. The
        // parsing is as specified for the 'strtod128' function in section 9.6
        // of the ISO/EIC TR 24732 C Decimal Floating-Point Technical Report,
        // except that it is unspecified whether the NaNs returned are quiet or
        // signaling. The behavior is undefined unless 'input' represents a
        // valid decimal floating-point number in scientific or fixed notation,
        // and no unrelated characters precede (not even whitespace) that
        // textual representation and a terminating null character immediately
        // follows it.  The behavior is undefined unless '*status' is 0.  Note
        // that this method does not guarantee the behavior of ISO/EIC TR 24732
        // C when parsing NaN because the AIX compiler intrinsics return a
        // signaling NaN.  Also note that the intel decimal floating point
        // library documents that inexact, underflow, and overflow are the
        // possible floating point exceptions for this operation.

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
        // Return a 'DenselyPackedDecimalImpUtil::StorageTypeXX' representing
        // the specified 'value' in Densely Packed Decimal (DPD) format.  This
        // format is compatible with the IBM compiler's native type.

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
        // Return a 'DecimalStorage::TypeXX' representing
        // the specified 'value' in Binary Integral Decimal (BID) format.  This
        // format is compatible with the Intel DFP implementation type.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                          // -----------------------------
                          // class DecimalImpUtil_IntelDfp
                          // -----------------------------

// CLASS METHODS
inline
void DecimalImpUtil_IntelDfp::setErrno(_IDEC_flags flags)
{
    if (BID_INVALID_EXCEPTION  & flags) {
        errno = EDOM;
    }
    else if (BID_OVERFLOW_EXCEPTION    & flags ||
             BID_UNDERFLOW_EXCEPTION   & flags ||
             BID_ZERO_DIVIDE_EXCEPTION & flags)
    {
        errno = ERANGE;
    }
}

// CLASS METHODS

                        // Integer construction

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::int32ToDecimal32(int value)
{
    DecimalImpUtil_IntelDfp::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_from_int32(value, &flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::int32ToDecimal64(int value)
{
    DecimalImpUtil_IntelDfp::ValueType64 retval;
    retval.d_raw = __bid64_from_int32(value);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::int32ToDecimal128(int value)
{
    DecimalImpUtil_IntelDfp::ValueType128 retval;
    retval.d_raw = __bid128_from_int32(value);
    return retval;
}


inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::uint32ToDecimal32(unsigned int value)
{
    DecimalImpUtil_IntelDfp::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_from_uint32(value, &flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::uint32ToDecimal64(unsigned int value)
{
    DecimalImpUtil_IntelDfp::ValueType64 retval;
    retval.d_raw = __bid64_from_uint32(value);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::uint32ToDecimal128(unsigned int value)
{
    DecimalImpUtil_IntelDfp::ValueType128 retval;
    retval.d_raw = __bid128_from_uint32(value);
    return retval;
}


inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::int64ToDecimal32(long long int value)
{
    DecimalImpUtil_IntelDfp::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_from_int64(value, &flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::int64ToDecimal64(long long int value)
{
    DecimalImpUtil_IntelDfp::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_from_int64(value, &flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::int64ToDecimal128(long long int value)
{
    DecimalImpUtil_IntelDfp::ValueType128 retval;
    retval.d_raw = __bid128_from_int64(value);
    return retval;
}


inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::uint64ToDecimal32(unsigned long long int value)
{
    DecimalImpUtil_IntelDfp::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_from_uint64(value, &flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::uint64ToDecimal64(unsigned long long int value)
{
    DecimalImpUtil_IntelDfp::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_from_uint64(value, &flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::uint64ToDecimal128(unsigned long long int value)
{
    DecimalImpUtil_IntelDfp::ValueType128 retval;
    retval.d_raw = __bid128_from_uint64(value);
    return retval;
}

                        // Arithmetic

                        // Addition Functions

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::add(DecimalImpUtil_IntelDfp::ValueType32 lhs,
                             DecimalImpUtil_IntelDfp::ValueType32 rhs)
{
    DecimalImpUtil_IntelDfp::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_add(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::add(DecimalImpUtil_IntelDfp::ValueType64 lhs,
                             DecimalImpUtil_IntelDfp::ValueType64 rhs)
{
    DecimalImpUtil_IntelDfp::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_add(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::add(DecimalImpUtil_IntelDfp::ValueType128 lhs,
                             DecimalImpUtil_IntelDfp::ValueType128 rhs)
{
    DecimalImpUtil_IntelDfp::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_add(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return retval;
}

                        // Subtraction Functions

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::subtract(DecimalImpUtil_IntelDfp::ValueType32 lhs,
                                  DecimalImpUtil_IntelDfp::ValueType32 rhs)
{
    DecimalImpUtil_IntelDfp::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_sub(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::subtract(DecimalImpUtil_IntelDfp::ValueType64 lhs,
                                  DecimalImpUtil_IntelDfp::ValueType64 rhs)
{
    DecimalImpUtil_IntelDfp::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_sub(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::subtract(DecimalImpUtil_IntelDfp::ValueType128 lhs,
                                  DecimalImpUtil_IntelDfp::ValueType128 rhs)
{
    DecimalImpUtil_IntelDfp::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_sub(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return retval;
}

                        // Multiplication Functions

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::multiply(DecimalImpUtil_IntelDfp::ValueType32 lhs,
                                  DecimalImpUtil_IntelDfp::ValueType32 rhs)
{
    DecimalImpUtil_IntelDfp::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_mul(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::multiply(DecimalImpUtil_IntelDfp::ValueType64 lhs,
                                  DecimalImpUtil_IntelDfp::ValueType64 rhs)
{
    DecimalImpUtil_IntelDfp::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_mul(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::multiply(DecimalImpUtil_IntelDfp::ValueType128 lhs,
                                  DecimalImpUtil_IntelDfp::ValueType128 rhs)
{
    DecimalImpUtil_IntelDfp::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_mul(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return retval;
}

                        // Division Functions

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::divide(DecimalImpUtil_IntelDfp::ValueType32 lhs,
                                DecimalImpUtil_IntelDfp::ValueType32 rhs)
{
    DecimalImpUtil_IntelDfp::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_div(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::divide(DecimalImpUtil_IntelDfp::ValueType64 lhs,
                                DecimalImpUtil_IntelDfp::ValueType64 rhs)
{
    DecimalImpUtil_IntelDfp::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_div(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::divide(DecimalImpUtil_IntelDfp::ValueType128 lhs,
                                DecimalImpUtil_IntelDfp::ValueType128 rhs)
{
    DecimalImpUtil_IntelDfp::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_div(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return retval;
}

                        // Negation Functions

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::negate(DecimalImpUtil_IntelDfp::ValueType32 value)
{
    DecimalImpUtil_IntelDfp::ValueType32 retval;
    retval.d_raw = __bid32_negate(value.d_raw);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::negate(DecimalImpUtil_IntelDfp::ValueType64 value)
{
    DecimalImpUtil_IntelDfp::ValueType64 retval;
    retval.d_raw = __bid64_negate(value.d_raw);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::negate(DecimalImpUtil_IntelDfp::ValueType128 value)
{
    DecimalImpUtil_IntelDfp::ValueType128 retval;
    retval.d_raw = __bid128_negate(value.d_raw);
    return retval;
}

                        // Comparison Functions

                        // Less Than Functions

inline
bool
DecimalImpUtil_IntelDfp::less(DecimalImpUtil_IntelDfp::ValueType32 lhs,
                              DecimalImpUtil_IntelDfp::ValueType32 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid32_quiet_less(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

inline
bool
DecimalImpUtil_IntelDfp::less(DecimalImpUtil_IntelDfp::ValueType64 lhs,
                              DecimalImpUtil_IntelDfp::ValueType64 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid64_quiet_less(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

inline
bool
DecimalImpUtil_IntelDfp::less(DecimalImpUtil_IntelDfp::ValueType128 lhs,
                              DecimalImpUtil_IntelDfp::ValueType128 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid128_quiet_less(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

                        // Greater Than Functions

inline
bool
DecimalImpUtil_IntelDfp::greater(DecimalImpUtil_IntelDfp::ValueType32 lhs,
                                 DecimalImpUtil_IntelDfp::ValueType32 rhs)
{
    _IDEC_flags flags(0);
    bool res =  __bid32_quiet_greater(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

inline
bool DecimalImpUtil_IntelDfp::greater(DecimalImpUtil_IntelDfp::ValueType64 lhs,
                                      DecimalImpUtil_IntelDfp::ValueType64 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid64_quiet_greater(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

inline
bool
DecimalImpUtil_IntelDfp::greater(DecimalImpUtil_IntelDfp::ValueType128 lhs,
                                 DecimalImpUtil_IntelDfp::ValueType128 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid128_quiet_greater(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

                        // Less Or Equal Functions

inline
bool
DecimalImpUtil_IntelDfp::lessEqual(DecimalImpUtil_IntelDfp::ValueType32 lhs,
                                   DecimalImpUtil_IntelDfp::ValueType32 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid32_quiet_less_equal(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

inline
bool
DecimalImpUtil_IntelDfp::lessEqual(DecimalImpUtil_IntelDfp::ValueType64 lhs,
                                   DecimalImpUtil_IntelDfp::ValueType64 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid64_quiet_less_equal(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

inline
bool
DecimalImpUtil_IntelDfp::lessEqual(DecimalImpUtil_IntelDfp::ValueType128 lhs,
                                   DecimalImpUtil_IntelDfp::ValueType128 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid128_quiet_less_equal(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

                        // Greater Or Equal Functions

inline
bool
DecimalImpUtil_IntelDfp::greaterEqual(DecimalImpUtil_IntelDfp::ValueType32 lhs,
                                      DecimalImpUtil_IntelDfp::ValueType32 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid32_quiet_greater_equal(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

inline
bool
DecimalImpUtil_IntelDfp::greaterEqual(DecimalImpUtil_IntelDfp::ValueType64 lhs,
                                      DecimalImpUtil_IntelDfp::ValueType64 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid64_quiet_greater_equal(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

inline
bool
DecimalImpUtil_IntelDfp::greaterEqual(
                                     DecimalImpUtil_IntelDfp::ValueType128 lhs,
                                     DecimalImpUtil_IntelDfp::ValueType128 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid128_quiet_greater_equal(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

                        // Equality Functions

inline
bool
DecimalImpUtil_IntelDfp::equal(DecimalImpUtil_IntelDfp::ValueType32 lhs,
                               DecimalImpUtil_IntelDfp::ValueType32 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid32_quiet_equal(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

inline
bool
DecimalImpUtil_IntelDfp::equal(DecimalImpUtil_IntelDfp::ValueType64 lhs,
                               DecimalImpUtil_IntelDfp::ValueType64 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid64_quiet_equal(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

inline
bool
DecimalImpUtil_IntelDfp::equal(DecimalImpUtil_IntelDfp::ValueType128 lhs,
                               DecimalImpUtil_IntelDfp::ValueType128 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid128_quiet_equal(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

                        // Inequality Functions

inline
bool
DecimalImpUtil_IntelDfp::notEqual(DecimalImpUtil_IntelDfp::ValueType32 lhs,
                                  DecimalImpUtil_IntelDfp::ValueType32 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid32_quiet_not_equal(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

inline
bool
DecimalImpUtil_IntelDfp::notEqual(DecimalImpUtil_IntelDfp::ValueType64 lhs,
                                  DecimalImpUtil_IntelDfp::ValueType64 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid64_quiet_not_equal(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

inline
bool
DecimalImpUtil_IntelDfp::notEqual(DecimalImpUtil_IntelDfp::ValueType128 lhs,
                                  DecimalImpUtil_IntelDfp::ValueType128 rhs)
{
    _IDEC_flags flags(0);
    bool res = __bid128_quiet_not_equal(lhs.d_raw, rhs.d_raw, &flags);
    setErrno(flags);
    return res;
}

                        // Inter-type Conversion functions

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::convertToDecimal32(
                             const DecimalImpUtil_IntelDfp::ValueType64& input)
{
    DecimalImpUtil_IntelDfp::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_to_bid32(input.d_raw, &flags);
    setErrno(flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::convertToDecimal32(
                            const DecimalImpUtil_IntelDfp::ValueType128& input)
{
    DecimalImpUtil_IntelDfp::ValueType32 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_to_bid32(input.d_raw, &flags);
    setErrno(flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::convertToDecimal64(
                             const DecimalImpUtil_IntelDfp::ValueType32& input)
{
    DecimalImpUtil_IntelDfp::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_to_bid64(input.d_raw, &flags);
    setErrno(flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::convertToDecimal64(
                            const DecimalImpUtil_IntelDfp::ValueType128& input)
{
    DecimalImpUtil_IntelDfp::ValueType64 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid128_to_bid64(input.d_raw, &flags);
    setErrno(flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::convertToDecimal128(
                             const DecimalImpUtil_IntelDfp::ValueType32& input)
{
    DecimalImpUtil_IntelDfp::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid32_to_bid128(input.d_raw, &flags);
    setErrno(flags);
    return retval;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::convertToDecimal128(
                             const DecimalImpUtil_IntelDfp::ValueType64& input)
{
    DecimalImpUtil_IntelDfp::ValueType128 retval;
    _IDEC_flags flags(0);
    retval.d_raw = __bid64_to_bid128(input.d_raw, &flags);
    setErrno(flags);
    return retval;
}

                        // Binary floating point conversion functions

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::binaryToDecimal32(float value)
{
    ValueType32 result;
    _IDEC_flags flags(0);
    result.d_raw = __binary32_to_bid32(value, &flags);
    setErrno(flags);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::binaryToDecimal32(double value)
{
    ValueType32 result;
    _IDEC_flags flags(0);
    result.d_raw = __binary64_to_bid32(value, &flags);
    setErrno(flags);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::binaryToDecimal64(float value)
{
    ValueType64 result;
    _IDEC_flags flags(0);
    result.d_raw = __binary32_to_bid64(value, &flags);
    setErrno(flags);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::binaryToDecimal64(double value)
{
    ValueType64 result;
    _IDEC_flags flags(0);
    result.d_raw = __binary64_to_bid64(value, &flags);
    setErrno(flags);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::binaryToDecimal128(float value)
{
    ValueType128 result;
    _IDEC_flags flags(0);
    result.d_raw = __binary32_to_bid128(value, &flags);
    setErrno(flags);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::binaryToDecimal128(double value)
{
    ValueType128 result;
    _IDEC_flags flags(0);
    result.d_raw = __binary64_to_bid128(value, &flags);
    setErrno(flags);
    return result;
}

                        // makeDecimalRaw Functions

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::makeDecimalRaw32(int significand,
                                          int exponent)
{
    DecimalImpUtil_IntelDfp::ValueType32 result;
    result = DecimalImpUtil_IntelDfp::int32ToDecimal32(significand);
    result = DecimalImpUtil_IntelDfp::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::makeDecimalRaw64(unsigned long long significand,
                                          int                exponent)
{
    DecimalImpUtil_IntelDfp::ValueType64 result;
    result = DecimalImpUtil_IntelDfp::uint64ToDecimal64(significand);
    result = DecimalImpUtil_IntelDfp::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::makeDecimalRaw64(long long significand,
                                          int       exponent)
{
    DecimalImpUtil_IntelDfp::ValueType64 result;
    result = DecimalImpUtil_IntelDfp::int64ToDecimal64(significand);
    result = DecimalImpUtil_IntelDfp::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::makeDecimalRaw64(unsigned int significand,
                                          int          exponent)
{
    DecimalImpUtil_IntelDfp::ValueType64 result;
    result = DecimalImpUtil_IntelDfp::uint32ToDecimal64(significand);
    result = DecimalImpUtil_IntelDfp::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::makeDecimalRaw64(int significand,
                                          int exponent)
{
    DecimalImpUtil_IntelDfp::ValueType64 result;
    result = DecimalImpUtil_IntelDfp::int32ToDecimal64(significand);
    result = DecimalImpUtil_IntelDfp::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::makeDecimalRaw128(unsigned long long significand,
                                           int                exponent)
{
    DecimalImpUtil_IntelDfp::ValueType128 result;
    result = DecimalImpUtil_IntelDfp::uint64ToDecimal128(significand);
    result = DecimalImpUtil_IntelDfp::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::makeDecimalRaw128(long long significand,
                                           int       exponent)
{
    DecimalImpUtil_IntelDfp::ValueType128 result;
    result = DecimalImpUtil_IntelDfp::int64ToDecimal128(significand);
    result = DecimalImpUtil_IntelDfp::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::makeDecimalRaw128(unsigned int significand,
                                           int          exponent)
{
    DecimalImpUtil_IntelDfp::ValueType128 result;
    result = DecimalImpUtil_IntelDfp::uint32ToDecimal128(significand);
    result = DecimalImpUtil_IntelDfp::scaleB(result, exponent);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::makeDecimalRaw128(int significand,
                                           int exponent)
{
    DecimalImpUtil_IntelDfp::ValueType128 result;
    result = DecimalImpUtil_IntelDfp::int32ToDecimal128(significand);
    result = DecimalImpUtil_IntelDfp::scaleB(result, exponent);
    return result;
}

                        // IEEE Scale B Functions

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::scaleB(DecimalImpUtil_IntelDfp::ValueType32 value,
                                int                                  exponent)
{
    DecimalImpUtil_IntelDfp::ValueType32 result;
    _IDEC_flags flags(0);
    result.d_raw = __bid32_scalbn(value.d_raw, exponent, &flags);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::scaleB(DecimalImpUtil_IntelDfp::ValueType64 value,
                                int                                  exponent)
{
    DecimalImpUtil_IntelDfp::ValueType64 result;
    _IDEC_flags flags(0);
    result.d_raw = __bid64_scalbn(value.d_raw, exponent, &flags);
    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::scaleB(DecimalImpUtil_IntelDfp::ValueType128 value,
                                int                                   exponent)
{
    DecimalImpUtil_IntelDfp::ValueType128 result;
    _IDEC_flags flags(0);
    result.d_raw = __bid128_scalbn(value.d_raw, exponent, &flags);
    return result;
}

                        // Parsing functions

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::parse32(const char *string)
{
    DecimalImpUtil_IntelDfp::ValueType32 result;
    _IDEC_flags flags(0);
    // NOTE: It is probably safe to convert from a 'const char *' to a 'char *'
    // because the __bid* interfaces are C interfaces.
    result.d_raw = __bid32_from_string(const_cast<char *>(string), &flags);

    if (BID_OVERFLOW_EXCEPTION  & flags ||
        BID_UNDERFLOW_EXCEPTION & flags)
    {
        errno = ERANGE;
    }

    return result;
}


inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::parse64(const char *string)
{
    DecimalImpUtil_IntelDfp::ValueType64 result;
    _IDEC_flags flags(0);
    // NOTE: It is probably safe to convert from a 'const char *' to a 'char *'
    // because the __bid* interfaces are C interfaces.
    result.d_raw = __bid64_from_string(const_cast<char *>(string), &flags);

    if (BID_OVERFLOW_EXCEPTION  & flags ||
        BID_UNDERFLOW_EXCEPTION & flags)
    {
        errno = ERANGE;
    }

    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::parse128(const char *string)
{
    DecimalImpUtil_IntelDfp::ValueType128 result;
    _IDEC_flags flags(0);
    // NOTE: It is probably safe to convert from a 'const char *' to a 'char *'
    // because the __bid* interfaces are C interfaces.
    result.d_raw = __bid128_from_string(const_cast<char *>(string), &flags);

    if (BID_OVERFLOW_EXCEPTION  & flags ||
        BID_UNDERFLOW_EXCEPTION & flags)
    {
        errno = ERANGE;
    }

    return result;
}


inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::parse32(const char *string, unsigned int *status)
{
    BSLS_ASSERT(0 == *status);

    DecimalImpUtil_IntelDfp::ValueType32 result;
    BSLMF_ASSERT((bsl::is_same<_IDEC_flags, unsigned int>::value));

    // NOTE: It is probably safe to convert from a 'const char *' to a 'char *'
    // because the __bid* interfaces are C interfaces.  Also note that inexact,
    // underflow, and overflow are the only dcoumented floating point
    // exceptions for this function.

    result.d_raw = __bid32_from_string(const_cast<char *>(string), status);

    return result;
}


inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::parse64(const char *string, unsigned int *status)
{
    BSLS_ASSERT(0 == *status);

    DecimalImpUtil_IntelDfp::ValueType64 result;

    BSLMF_ASSERT((bsl::is_same<_IDEC_flags, unsigned int>::value));

    // NOTE: It is probably safe to convert from a 'const char *' to a 'char *'
    // because the __bid* interfaces are C interfaces.  Also note that inexact,
    // underflow, and overflow are the only dcoumented floating point
    // exceptions for this function.

    result.d_raw = __bid64_from_string(const_cast<char *>(string), status);

    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::parse128(const char *string, unsigned int *status)
{
    BSLS_ASSERT(0 == *status);

    DecimalImpUtil_IntelDfp::ValueType128 result;
    BSLMF_ASSERT((bsl::is_same<_IDEC_flags, unsigned int>::value));

    // NOTE: It is probably safe to convert from a 'const char *' to a 'char *'
    // because the __bid* interfaces are C interfaces.  Also note that inexact,
    // underflow, and overflow are the only dcoumented floating point
    // exceptions for this function.

    result.d_raw = __bid128_from_string(const_cast<char *>(string), status);

    return result;
}

                        // Densely Packed Conversion Functions
inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::convertDPDtoBID(DecimalStorage::Type32 dpd)
{
    ValueType32 value;
    bsl::memcpy(&value, &dpd, sizeof(value));

    ValueType32 result;
    result.d_raw = __bid_dpd_to_bid32(value.d_raw);

    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::convertDPDtoBID(DecimalStorage::Type64 dpd)
{
    ValueType64 value;
    bsl::memcpy(&value, &dpd, sizeof(value));

    ValueType64 result;
    result.d_raw = __bid_dpd_to_bid64(value.d_raw);

    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::convertDPDtoBID(DecimalStorage::Type128 dpd)
{
    ValueType128 value;
    bsl::memcpy(&value, &dpd, sizeof(value));

    ValueType128 result;
    result.d_raw = __bid_dpd_to_bid128(value.d_raw);

    return result;
}

inline
DecimalStorage::Type32
DecimalImpUtil_IntelDfp::convertBIDtoDPD(
                                    DecimalImpUtil_IntelDfp::ValueType32 value)
{
    ValueType32 result;
    result.d_raw = __bid_to_dpd32(value.d_raw);

    DecimalStorage::Type32 dpd;
    bsl::memcpy(&dpd, &result, sizeof(dpd));

    return dpd;
}

inline
DecimalStorage::Type64
DecimalImpUtil_IntelDfp::convertBIDtoDPD(
                                    DecimalImpUtil_IntelDfp::ValueType64 value)
{
    ValueType64 result;
    result.d_raw = __bid_to_dpd64(value.d_raw);

    DecimalStorage::Type64 dpd;
    bsl::memcpy(&dpd, &result, sizeof(dpd));

    return dpd;
}

inline
DecimalStorage::Type128
DecimalImpUtil_IntelDfp::convertBIDtoDPD(
                                   DecimalImpUtil_IntelDfp::ValueType128 value)
{
    ValueType128 result;
    result.d_raw = __bid_to_dpd128(value.d_raw);

    DecimalStorage::Type128 dpd;
    bsl::memcpy(&dpd, &result, sizeof(dpd));

    return dpd;
}
                        // Binary Integral Conversion Functions

inline
DecimalImpUtil_IntelDfp::ValueType32
DecimalImpUtil_IntelDfp::convertFromBID(DecimalStorage::Type32 bid)
{
    ValueType32 result;
    bsl::memcpy(&result, &bid, sizeof(result));

    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType64
DecimalImpUtil_IntelDfp::convertFromBID(DecimalStorage::Type64 bid)
{
    ValueType64 result;
    bsl::memcpy(&result, &bid, sizeof(result));

    return result;
}

inline
DecimalImpUtil_IntelDfp::ValueType128
DecimalImpUtil_IntelDfp::convertFromBID(DecimalStorage::Type128 bid)
{
    ValueType128 result;
    bsl::memcpy(&result, &bid, sizeof(result));

    return result;
}

inline
DecimalStorage::Type32
DecimalImpUtil_IntelDfp::convertToBID(
                                    DecimalImpUtil_IntelDfp::ValueType32 value)
{
    DecimalStorage::Type32 bid;
    bsl::memcpy(&bid, &value, sizeof(bid));

    return bid;
}

inline
DecimalStorage::Type64
DecimalImpUtil_IntelDfp::convertToBID(
                                    DecimalImpUtil_IntelDfp::ValueType64 value)
{
    DecimalStorage::Type64 bid;
    bsl::memcpy(&bid, &value, sizeof(bid));

    return bid;
}

inline
DecimalStorage::Type128
DecimalImpUtil_IntelDfp::convertToBID(
                                   DecimalImpUtil_IntelDfp::ValueType128 value)
{
    DecimalStorage::Type128 bid;
    bsl::memcpy(&bid, &value, sizeof(bid));

    return bid;
}

}  // close package namespace
}  // close enterprise namespace

#endif  // #ifdef BDLDFP_DECIMALPLATFORM_INTELDFP

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
