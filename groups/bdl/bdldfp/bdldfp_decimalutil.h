// bdldfp_decimalutil.h                                               -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALUTIL
#define INCLUDED_BDLDFP_DECIMALUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id$")

//@PURPOSE: Provide utilities dealing with floating point decimal objects.
//
//@CLASSES:
//  bdldfp::DecimalUtil: decimal floating point utility functions.
//
//@MACROS:
//  FP_SUBNORMAL: subnormal floating-point classification identifier constant
//  FP_NORMAL:    normal floating-point classification identifier constant
//  FP_ZERO:      zero floating-point classification identifier constant
//  FP_INFINITE:  infinity floating-point classification identifier constant
//  FP_NAN:       NaN floating-point classification identifier constant
//
// Note that these macros may *not* be defined in this header.  They are C99
// standard macros and this component defines them only for those platforms
// that have failed to implement C99 (such as Microsoft).
//
//@SEE_ALSO: bdldfp_decimal, bdldfp_decimalplatform
//
//@DESCRIPTION: The 'bdldfp::DecimalUtil' component provides utility functions
// for the decimal floating-point types defined in 'bdldfp_decimal':
//
//: o 'FP_XXX', C99 standard floating-point classification macros
//:
//: o the 'makeDecimal' functions building a decimal floating-point value out
//:   of a coefficient and exponent.
//:
//: o the 'parseDecimal' functions that convert text to decimal value.
//:
//: o 'fma', 'fabs', 'ceil', 'floor', 'trunc', 'round' - math functions
//:
//: o 'classify' and the 'isXxxx' floating-point value classification functions
//
// The 'FP_XXX' C99 floating-point classification macros may also be provided
// by this header for platforms where C99 support is still not provided.
//
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: Building Decimals From Integer Parts
///- - - - - - - - - - - - - - - - - - - - - - - -
// Floating-point numbers are built from a sign, a significand and an exponent.
// All those 3 are integers (of various sizes), therefore it is possible to
// build decimals from integers:
//..
//  long long coefficient = 42; // Yet another name for significand
//  int exponent          = -1;
//
//  Decimal32  d32  = makeDecimal32( coefficient, exponent);
//  Decimal64  d64  = makeDecimal64( coefficient, exponent);
//  Decimal128 d128 = makeDecimal128(coefficient, exponent);
//
//  assert(BDLDFP_DECIMAL_DF(4.2) == d32);
//  assert(BDLDFP_DECIMAL_DD(4.2) == d64);
//  assert(BDLDFP_DECIMAL_DL(4.2) == d128);
//..

// TODO TBD Priority description:
//
// 1 - these are already implemented so you should not see TBD/TODO for them
// E - implement when the thread-local Environment/Context is implemented
// 2 - implement as second priority (most probably after the 'E')
// N - Do not implement unless explicitly requested

#include <bdlscm_version.h>

#include <bdldfp_decimal.h>
#include <bdldfp_decimalformatconfig.h>
#include <bdldfp_decimalimputil_inteldfp.h>
#include <bdldfp_decimalplatform.h>
#include <bdldfp_uint128.h>

#include <bsls_assert.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_optional.h>
#include <bsl_string.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
#include <memory_resource>  // 'std::pmr::polymorphic_allocator'
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_PMR

#include <string>           // 'std::string', 'std::pmr::string'

namespace BloombergLP {
namespace bdldfp {
                            // =================
                            // class DecimalUtil
                            // =================

struct DecimalUtil {
    // This utility 'struct' provides a namespace for functions using the
    // decimal floating point types defined in the 'bdldfp_decimal' package.

    // CLASS METHODS

                            // Creators functions

    static Decimal32 makeDecimalRaw32 (int significand, int exponent);
        // Create a 'Decimal32' object representing a decimal floating point
        // number consisting of the specified 'significand' and 'exponent',
        // with the sign given by the 'significand' (if signed).  The behavior
        // is undefined unless '-9,999,999 <= significand <= 9,999,999' and
        // '-101 <= exponent <= 90'.

    static Decimal64 makeDecimalRaw64(int                significand,
                                      int                exponent);
    static Decimal64 makeDecimalRaw64(unsigned int       significand,
                                      int                exponent);
    static Decimal64 makeDecimalRaw64(long long          significand,
                                      int                exponent);
    static Decimal64 makeDecimalRaw64(unsigned long long significand,
                                      int                exponent);
        // Create a 'Decimal64' object representing a decimal floating point
        // number consisting of the specified 'significand' and 'exponent',
        // with the sign given by the 'significand' (if signed).  The behavior
        // is undefined unless
        // '-9,999,999,999,999,999 <= significand <= 9,999,999,999,999,999' and
        // '-398 <= exponent <= 369'.

    static Decimal128 makeDecimalRaw128(int                significand,
                                        int                exponent);
    static Decimal128 makeDecimalRaw128(unsigned int       significand,
                                        int                exponent);
    static Decimal128 makeDecimalRaw128(long long          significand,
                                        int                exponent);
    static Decimal128 makeDecimalRaw128(unsigned long long significand,
                                        int                exponent);
        // Create a 'Deciaml128' object representing a decimal floating point
        // number consisting of the specified 'significand' and specified
        // 'exponent', with the sign given by the 'significand' (if signed).
        // The behavior is undefined unless '-6176 <= exponent <= 6111'.

    static Decimal64 makeDecimal64(int                significand,
                                   int                exponent);
    static Decimal64 makeDecimal64(unsigned int       significand,
                                   int                exponent);
    static Decimal64 makeDecimal64(long long          significand,
                                   int                exponent);
    static Decimal64 makeDecimal64(unsigned long long significand,
                                   int                exponent);
        // Return a 'DecimalNN' object that has the specified 'significand' and
        // 'exponent', rounded according to the current decimal rounding mode,
        // if necessary.  If an overflow condition occurs, store the value of
        // the macro 'ERANGE' into 'errno' and return infinity with the
        // appropriate sign.

    static int parseDecimal32(Decimal32 *out, const char *str);
    static int parseDecimal64(Decimal64 *out, const char *str);
    static int parseDecimal128(Decimal128 *out, const char *str);
    template <class STRING_TYPE>
    static int parseDecimal32(Decimal32 *out, const STRING_TYPE& str);
    template <class STRING_TYPE>
    static int parseDecimal64(Decimal64 *out, const STRING_TYPE& str);
    template <class STRING_TYPE>
    static int parseDecimal128(Decimal128 *out, const STRING_TYPE& str);
        // Load into the specified 'out' the decimal floating point number
        // described by the specified 'str'; return zero if the conversion was
        // successful and non-zero otherwise.  The value of 'out' is
        // unspecified if the function returns a non-zero value.  The
        // parameterized 'STRING_TYPE' must be one of 'bsl::string',
        // 'std::string', 'std::pmr::string' (if supported), or
        // 'bslstl::StringRef'.

    static int parseDecimal32Exact(Decimal32 *out, const char *str);
    static int parseDecimal64Exact(Decimal64 *out, const char *str);
    static int parseDecimal128Exact(Decimal128 *out, const char *str);
    template <class STRING_TYPE>
    static int parseDecimal32Exact(Decimal32 *out, const STRING_TYPE& str);
    template <class STRING_TYPE>
    static int parseDecimal64Exact(Decimal64 *out, const STRING_TYPE& str);
    template <class STRING_TYPE>
    static int parseDecimal128Exact(Decimal128 *out, const STRING_TYPE& str);
        // Load into the specified 'out' the decimal floating point number
        // described by the specified 'str'.  Return 0 if 'out' is an exact
        // representation of 'str', a positive value if 'str' is an
        // approximation of 'str' (i.e., 'str' could not be represented
        // exactly), and a negative value if 'str' could not be parsed.  The
        // value of 'out' is unspecified if the function returns a negative
        // value.  The parameterized 'STRING_TYPE' must be one of
        // 'bsl::string', 'std::string', 'std::pmr::string' (if supported), or
        // 'bslstl::StringRef'.

    // math

    static Decimal32  copySign(Decimal32  x, Decimal32  y);
    static Decimal64  copySign(Decimal64  x, Decimal64  y);
    static Decimal128 copySign(Decimal128 x, Decimal128 y);
        // Return a decimal value with the magnitude of the specifed 'x' and
        // the sign of the specified 'y'.  If 'x' is NaN, then NaN with the
        // sign of 'y' is returned.
        //
        // Examples: 'copySign( 5.0, -2.0)' ==> -5.0;
        //           'copySign(-5.0, -2.0)' ==>  5.0

    static Decimal32  exp(Decimal32  x);
    static Decimal64  exp(Decimal64  x);
    static Decimal128 exp(Decimal128 x);
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

    static Decimal32  log(Decimal32  x);
    static Decimal64  log(Decimal64  x);
    static Decimal128 log(Decimal128 x);
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

    static Decimal32  logB(Decimal32  x);
    static Decimal64  logB(Decimal64  x);
    static Decimal128 logB(Decimal128 x);
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

    static Decimal32  log10(Decimal32  x);
    static Decimal64  log10(Decimal64  x);
    static Decimal128 log10(Decimal128 x);
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
        //: o If 'x' is signaling NaN, NaN is returned and the value of the
        //:   macro 'EDOM' is stored into 'errno'.

    static Decimal32  fmod(Decimal32  x, Decimal32  y);
    static Decimal64  fmod(Decimal64  x, Decimal64  y);
    static Decimal128 fmod(Decimal128 x, Decimal128 y);
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

    static Decimal32  remainder(Decimal32  x, Decimal32  y);
    static Decimal64  remainder(Decimal64  x, Decimal64  y);
    static Decimal128 remainder(Decimal128 x, Decimal128 y);
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

    static long int        lrint(Decimal32  x);
    static long int        lrint(Decimal64  x);
    static long int        lrint(Decimal128 x);
    static long long int  llrint(Decimal32  x);
    static long long int  llrint(Decimal64  x);
    static long long int  llrint(Decimal128 x);
        // Return an integer value nearest to the specified 'x'.  Round 'x'
        // using the current rounding mode.  If 'x' is +/-infnity, NaN (either
        // signaling or quiet) or the rounded value is outside the range of the
        // return type, store the value of the macro 'EDOM' into 'errno' and
        // return implementation-defined value.

    static Decimal32  nextafter( Decimal32  from, Decimal32  to);
    static Decimal64  nextafter( Decimal64  from, Decimal64  to);
    static Decimal128 nextafter( Decimal128 from, Decimal128 to);
    static Decimal32  nexttoward(Decimal32  from, Decimal128 to);
    static Decimal64  nexttoward(Decimal64  from, Decimal128 to);
    static Decimal128 nexttoward(Decimal128 from, Decimal128 to);
        // Return the next representable value of the specified 'from' in the
        // direction of the specified 'to'.
        //
        // Special value handling:
        //: o If 'from' equals 'to', 'to' is returned.
        //: o If either argument is quiet NaN, quiet NaN is returned.
        //: o If either argument is signaling NaN, quiet NaN is returned and
        //:   the value of the macro 'EDOM' is stored into 'errno'.
        //: o If 'from' is finite, but the expected result is an infinity,
        //:   infinity is returned and the value of the macro 'ERANGE' is
        //:   stored into 'errno'.
        //: o If 'from' does not equal 'to' and the result is subnormal or
        //:   zero, the value of the macro 'ERANGE' is stored into 'errno'.

    static Decimal32  pow(Decimal32  base, Decimal32  exp);
    static Decimal64  pow(Decimal64  base, Decimal64  exp);
    static Decimal128 pow(Decimal128 base, Decimal128 exp);
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

    static Decimal32  fma(Decimal32  x, Decimal32  y, Decimal32  z);
    static Decimal64  fma(Decimal64  x, Decimal64  y, Decimal64  z);
    static Decimal128 fma(Decimal128 x, Decimal128 y, Decimal128 z);
        // Return, using the specified 'x', 'y', and 'z', the value of the
        // expression 'x * y + z', rounded as one ternary operation according
        // to the current decimal floating point rounding mode.
        //
        // Special value handling:
        //: o If 'x' or 'y' are quiet NaN, quiet NaN is returned.
        //: o If any argument is signaling NaN, quiet NaN is returned and the
        //:   value of the macro 'EDOM' is stored into 'errno'.
        //: o If 'x*y' is an exact infinity and 'z' is an infinity with the
        //:   opposite sign, quiet NaN is returned and the value of the macro
        //:   'EDOM' is stored into 'errno'.
        //: o If 'x' is zero and 'y' is infinite or if 'x' is infinite and 'y'
        //:   is zero, and 'z' is not a NaN, then quiet NaN is returned and the
        //:   value of the macro 'EDOM' is stored into 'errno'.
        //: o If 'x' is zero and 'y' is infinite or if 'x' is infinite and 'y'
        //:   is zero, and 'z' is NaN, then quiet NaN is returned.

                       // Selecting, converting functions

    static Decimal32  fabs(Decimal32  value);
    static Decimal64  fabs(Decimal64  value);
    static Decimal128 fabs(Decimal128 value);
        // Return the absolute value of the specified 'x'.
        //
        // Special value handling:
        //: o if 'x' is NaN (either signaling or quiet), quiet NaN is returned.
        //: o if 'x' is +/-infinity or +/-0, it is returned unmodified.

    static Decimal32  sqrt(Decimal32  x);
    static Decimal64  sqrt(Decimal64  x);
    static Decimal128 sqrt(Decimal128 x);
        // Return the square root of the specified 'x'.
        //
        // Special value handling:
        //: o If 'x' is NaN, NaN is returned.
        //: o If 'x' is less than -0, NaN is returned and the value of the
        //:   macro 'EDOM' is stored into 'errno'.
        //: o If 'x' is +/-infinity or +/-0, it is returned unmodified.

                               // classification

    // Names are camelCase so they do not collide with macros of 'math.h'.

    static int classify(Decimal32  x);
    static int classify(Decimal64  x);
    static int classify(Decimal128 x);
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


    static bool isFinite(Decimal32  x);
    static bool isFinite(Decimal64  x);
    static bool isFinite(Decimal128 x);
        // Return 'true' if the specified 'x' is not an infinity value or NaN
        // and 'false' otherwise.  Note that this is equivalent to
        // 'classify(x) != FP_INFINITE && classify(x) != FP_NAN'.

    static bool isInf(Decimal32  x);
    static bool isInf(Decimal64  x);
    static bool isInf(Decimal128 x);
        // Return 'true' if the specified 'x' is an infinity value and 'false'
        // otherwise.  Note that this is equivalent to
        // 'classify(x) == FP_INFINITE'.

    static bool isNan(Decimal32  x);
    static bool isNan(Decimal64  x);
    static bool isNan(Decimal128 x);
        // Return 'true' if the specified 'x' is NaN and 'false' otherwise.
        // Note that this is equivalent to 'classify(x) == FP_NAN'.

    static bool isNormal(Decimal32  x);
    static bool isNormal(Decimal64  x);
    static bool isNormal(Decimal128 x);
        // Return 'true' if the specified 'x' is a normal value and 'false'
        // otherwise.  Note that this is equivalent to
        // 'classify(x) == FP_NORMAL'.

                           // Comparison functions

    static bool isUnordered(Decimal32  x, Decimal32  y);
    static bool isUnordered(Decimal64  x, Decimal64  y);
    static bool isUnordered(Decimal128 x, Decimal128 y);
        // Return 'true' if either (or both) of the specified 'x' and 'y'
        // arguments is a NaN, or 'false' otherwise.


                             // Rounding functions

    static Decimal32  ceil(Decimal32  x);
    static Decimal64  ceil(Decimal64  x);
    static Decimal128 ceil(Decimal128 x);
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

    static Decimal32  floor(Decimal32  x);
    static Decimal64  floor(Decimal64  x);
    static Decimal128 floor(Decimal128 x);
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

    static Decimal32   round(Decimal32  x);
    static Decimal64   round(Decimal64  x);
    static Decimal128  round(Decimal128 x);
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

    static long int   lround(Decimal32  x);
    static long int   lround(Decimal64  x);
    static long int   lround(Decimal128 x);
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

    static Decimal32  round(Decimal32  x, unsigned int precision);
    static Decimal64  round(Decimal64  x, unsigned int precision);
    static Decimal128 round(Decimal128 x, unsigned int precision);
        // Return the specified 'x' value rounded to the specified 'precision'
        // decimal places.  Round halfway cases away from zero, regardless of
        // the current decimal floating point rounding mode.  If 'x' is
        // integral, positive zero, negative zero, NaN, or infinity then return
        // 'x' itself.
        //
        //  Examples: 'round(3.14159, 3)' ==> 3.142

    static Decimal32  trunc(Decimal32  x);
    static Decimal64  trunc(Decimal64  x);
    static Decimal128 trunc(Decimal128 x);
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

    static Decimal32  trunc(Decimal32  x, unsigned int precision);
    static Decimal64  trunc(Decimal64  x, unsigned int precision);
    static Decimal128 trunc(Decimal128 x, unsigned int precision);
        // Return the specified 'x' value truncated to the specified
        // 'precision' decimal places.  Round towards zero, regardless of the
        // current decimal floating point rounding mode.  If precision of 'x'
        // is less than or equal the 'precision' or 'x' is positive zero,
        // negative zero, NaN, or infinity then return 'x' itself.
        //
        //  Examples: 'trunc(3.14159, 3)' ==> 3.141

                             // Quantum functions

    static Decimal32  multiplyByPowerOf10(Decimal32  value,
                                          int        exponent);
    static Decimal64  multiplyByPowerOf10(Decimal64 value,
                                          int        exponent);
    static Decimal128 multiplyByPowerOf10(Decimal128 value,
                                          int        exponent);
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

    static Decimal32  quantize(Decimal32  value, Decimal32  exponent);
    static Decimal64  quantize(Decimal64  value, Decimal64  exponent);
    static Decimal128 quantize(Decimal128 value, Decimal128 exponent);
        // Return a number equal to the specified 'value' (except for possible
        // rounding) having the exponent equal to the exponent of the specified
        // 'exponent'.  Rounding may occur when the exponent is greater than
        // the quantum of 'value'.  E.g., 'quantize(147e-2_d32, 1e-1_d32)'
        // yields '15e-1_d32'.  In the opposite direction, if 'exponent' is
        // sufficiently less than the quantum of 'value', it may not be
        // possible to construct the requested result, and if so, 'NaN' is
        // returned.  E.g., 'quantize(1234567e0_d32, 1e-1_d32)' returns 'NaN'.

    static Decimal32  quantize(Decimal32  value, int exponent);
    static Decimal64  quantize(Decimal64  value, int exponent);
    static Decimal128 quantize(Decimal128 value, int exponent);
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

    static int quantizeEqual(Decimal32  *x, Decimal32  y, int exponent);
    static int quantizeEqual(Decimal64  *x, Decimal64  y, int exponent);
    static int quantizeEqual(Decimal128 *x, Decimal128 y, int exponent);
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

    static int quantum(Decimal32  value);
    static int quantum(Decimal64  value);
    static int quantum(Decimal128 value);
        // Return an integer equal to the exponent field in the specified
        // 'value'.  Each decimal floating point number is a representation of
        // the ideal form 's * (10 ** e)', where 's' is significand and 'e' is
        // exponent.  This function returns that exponent value.  The behavior
        // is undefined if 'value' is NaN or 'value' is infinity.

    static bool sameQuantum(Decimal32  x, Decimal32  y);
    static bool sameQuantum(Decimal64  x, Decimal64  y);
    static bool sameQuantum(Decimal128 x, Decimal128 y);
        // Return 'true' if the specified 'x' and 'y' values have the same
        // quantum exponents, and 'false' otherwise.  If both arguments are NaN
        // or both arguments are infinity, they have the same quantum
        // exponents.  Note that if exactly one operand is NaN or exactly one
        // operand is infinity, they do not have the same quantum exponents.

                             // Decompose functions

    static int decompose(int                 *sign,
                         unsigned  int       *significand,
                         int                 *exponent,
                         Decimal32            value);
    static int decompose(int                 *sign,
                         bsls::Types::Uint64 *significand,
                         int                 *exponent,
                         Decimal64            value);
    static int decompose(int                 *sign,
                         Uint128             *significand,
                         int                 *exponent,
                         Decimal128           value);
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

    static
    int format(char                      *buffer,
               int                        length,
               Decimal32                  value,
               const DecimalFormatConfig& cfg = DecimalFormatConfig());

    static
    int format(char                      *buffer,
               int                        length,
               Decimal64                  value,
               const DecimalFormatConfig& cfg = DecimalFormatConfig());

    static
    int format(char                      *buffer,
               int                        length,
               Decimal128                 value,
               const DecimalFormatConfig& cfg = DecimalFormatConfig());
        // Format the specified 'value', placing the output in the buffer
        // designated by the specified 'buffer' and 'length', and return the
        // length of the formatted value.  If there is insufficient room in the
        // buffer, its contents will be left in an unspecified state, with the
        // returned value indicating the necessary size.  This function does
        // not write a terminating null character.  If 'length' is not
        // positive, 'buffer' is permitted to be null.  This can be used to
        // determine the necessary buffer size.  Optionally specify a 'cfg',
        // indicating formatting parameters.  If 'cfg' is not specified, then
        // default configuration instance, indicating scientific notation style
        // with a precision sufficient to produce all available digits is used.
        // See the Attributes section under @DESCRIPTION in the component-level
        // documentation for 'bdldfp::DecimalFormatConfig' component for
        // information on the configuration attributes.
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
};

                       // =============================
                       // class DecimalUtil_CStringUtil
                       // =============================

struct DecimalUtil_CStringUtil {
    // This component-private utility 'struct' provides a namespace for the
    // 'flatten' overload set intended to be used in concert with an overload
    // set consisting of a function template with a deduced argument and an
    // non-template overload accepting a 'const char *'.  The actual
    // implementation of the functionality would be in the 'const char *'
    // overload whereas the purpose of the function template is to invoke the
    // 'const char *' overload with a null-terminated string.
    //
    // The function template achieves null-termination by recursively calling
    // the function and supplying it with the result of 'flatten' invoked on
    // the deduced argument.  This 'flatten' invocation will call 'c_str()' on
    // various supported 'string' types, will produce a temporary 'bsl::string'
    // for possibly non-null-terminated 'bslstl::StringRef', and will result in
    // a 'BSLMF_ASSERT' for any unsupported type.  Calling the function with
    // the temporary 'bsl::string' produced from 'bslstl::StringRef' will
    // result in a second invocation of 'flatten', this time producing
    // 'const char *', and finally calling the function with a null-terminated
    // string.
    //
    // Note that the 'bslstl::StringRef' overload for 'flatten' is provided for
    // backwards compatibility.  Without it, the 'bsl::string' and
    // 'std::string' overloads would be ambiguous.  In new code, it is
    // preferable to not provide 'bslstl::StringRef' overload in a similar
    // facility and require the clients to explicitly state the string type in
    // their code, making a potential allocation obvious.  The
    // 'bsl::string_view' overload is not provided for the same reason.

    // CLASS METHODS

    static const char *flatten(const char *cString);
    static const char *flatten(char *cString);
        // Return the specified 'cString'.

    static const char *flatten(const bsl::string& string);
    static const char *flatten(const std::string& string);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static const char *flatten(const std::pmr::string& string);
#endif
        // Return the result of invoking 'c_str()' on the specified 'string'.

    static bsl::string flatten(const bslstl::StringRef& stringRef);
        // Return a temporary 'bsl::string' constructed from the specified
        // 'stringRef'.

    template <class TYPE>
    static const char *flatten(const TYPE&);
        // Produce a compile-time error informing the caller that the
        // parameterized 'TYPE' is not supported as the parameter for the call.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // -----------------
                             // class DecimalUtil
                             // -----------------

// CLASS METHODS
inline
Decimal32 DecimalUtil::makeDecimalRaw32(int significand, int exponent)
{
    return DecimalImpUtil::makeDecimalRaw32(significand, exponent);
}
inline
Decimal64 DecimalUtil::makeDecimalRaw64(int significand, int exponent)
{
    return DecimalImpUtil::makeDecimalRaw64(significand, exponent);
}
inline
Decimal64 DecimalUtil::makeDecimalRaw64(unsigned int significand, int exponent)
{
    return DecimalImpUtil::makeDecimalRaw64(significand, exponent);
}
inline
Decimal64 DecimalUtil::makeDecimalRaw64(long long significand, int exponent)
{
    return DecimalImpUtil::makeDecimalRaw64(significand, exponent);
}
inline
Decimal64
DecimalUtil::makeDecimalRaw64(unsigned long long significand, int exponent)
{
    return DecimalImpUtil::makeDecimalRaw64(significand, exponent);
}
inline
Decimal128 DecimalUtil::makeDecimalRaw128(int significand, int exponent)
{
    return DecimalImpUtil::makeDecimalRaw128(significand, exponent);
}
inline
Decimal128 DecimalUtil::makeDecimalRaw128(unsigned int significand,
                                          int          exponent)
{
    return DecimalImpUtil::makeDecimalRaw128(significand, exponent);
}
inline
Decimal128 DecimalUtil::makeDecimalRaw128(long long significand, int exponent)
{
    return DecimalImpUtil::makeDecimalRaw128(significand, exponent);
}
inline
Decimal128
DecimalUtil::makeDecimalRaw128(unsigned long long significand, int exponent)
{
    return DecimalImpUtil::makeDecimalRaw128(significand, exponent);
}

inline
Decimal64 DecimalUtil::makeDecimal64(int significand, int exponent)
{
    return DecimalImpUtil::makeDecimal64(significand, exponent);
}
inline
Decimal64 DecimalUtil::makeDecimal64(unsigned int significand, int exponent)
{
    return DecimalImpUtil::makeDecimal64(significand, exponent);
}
inline
Decimal64 DecimalUtil::makeDecimal64(long long significand, int exponent)
{
    return DecimalImpUtil::makeDecimal64(significand, exponent);
}
inline
Decimal64 DecimalUtil::makeDecimal64(unsigned long long significand,
                                     int                exponent)
{
    return DecimalImpUtil::makeDecimal64(significand, exponent);
}

template <class STRING_TYPE>
inline
int DecimalUtil::parseDecimal32(Decimal32 *out, const STRING_TYPE& str)
{
    return DecimalUtil::parseDecimal32(out,
                                       DecimalUtil_CStringUtil::flatten(str));
}

template <class STRING_TYPE>
inline
int DecimalUtil::parseDecimal64(Decimal64 *out, const STRING_TYPE& str)
{
    return DecimalUtil::parseDecimal64(out,
                                       DecimalUtil_CStringUtil::flatten(str));
}

template <class STRING_TYPE>
inline
int DecimalUtil::parseDecimal128(Decimal128 *out, const STRING_TYPE& str)
{
    return DecimalUtil::parseDecimal128(out,
                                        DecimalUtil_CStringUtil::flatten(str));
}

template <class STRING_TYPE>
inline
int DecimalUtil::parseDecimal32Exact(Decimal32 *out, const STRING_TYPE& str)
{
    return DecimalUtil::parseDecimal32Exact(
                                        out,
                                        DecimalUtil_CStringUtil::flatten(str));
}

template <class STRING_TYPE>
inline
int DecimalUtil::parseDecimal64Exact(Decimal64 *out, const STRING_TYPE& str)
{
    return DecimalUtil::parseDecimal64Exact(
                                        out,
                                        DecimalUtil_CStringUtil::flatten(str));
}

template <class STRING_TYPE>
inline
int DecimalUtil::parseDecimal128Exact(Decimal128 *out, const STRING_TYPE& str)
{
    return DecimalUtil::parseDecimal128Exact(
                                        out,
                                        DecimalUtil_CStringUtil::flatten(str));
}

                             // Quantum functions

inline
Decimal32 DecimalUtil::multiplyByPowerOf10(Decimal32 value, int exponent)
{
    return bdldfp::DecimalImpUtil::scaleB(*value.data(), exponent);
}

inline
Decimal64 DecimalUtil::multiplyByPowerOf10(Decimal64 value, int exponent)
{
    return bdldfp::DecimalImpUtil::scaleB(*value.data(), exponent);
}

inline
Decimal128 DecimalUtil::multiplyByPowerOf10(Decimal128 value, int exponent)
{
    return bdldfp::DecimalImpUtil::scaleB(*value.data(), exponent);
}

inline
Decimal32 DecimalUtil::quantize(Decimal32 value, Decimal32 exponent)
{
    return DecimalImpUtil::quantize(*value.data(), *exponent.data());
}

inline
Decimal64 DecimalUtil::quantize(Decimal64 value, Decimal64 exponent)
{
    return DecimalImpUtil::quantize(*value.data(), *exponent.data());
}

inline
Decimal128 DecimalUtil::quantize(Decimal128 value, Decimal128 exponent)
{
    return DecimalImpUtil::quantize(*value.data(), *exponent.data());
}

inline
Decimal32 DecimalUtil::quantize(Decimal32 value, int exponent)
{
    BSLS_ASSERT(-101 <= exponent);
    BSLS_ASSERT(        exponent <= 90);
    return DecimalImpUtil::quantize(*value.data(), exponent);
}

inline
Decimal64 DecimalUtil::quantize(Decimal64 value, int exponent)
{
    BSLS_ASSERT(-398 <= exponent);
    BSLS_ASSERT(        exponent <= 369);
    return DecimalImpUtil::quantize(*value.data(), exponent);
}

inline
Decimal128 DecimalUtil::quantize(Decimal128 value, int exponent)
{
    BSLS_ASSERT(-6176 <= exponent);
    BSLS_ASSERT(         exponent <= 6111);
    return DecimalImpUtil::quantize(*value.data(), exponent);
}

inline
int DecimalUtil::quantizeEqual(Decimal32 *x, Decimal32 y, int exponent)
{
    BSLS_ASSERT(x);
    BSLS_ASSERT(-101 <= exponent);
    BSLS_ASSERT(        exponent <= 90);
    return DecimalImpUtil::quantizeEqual(x->data(), *y.data(), exponent);
}

inline
int DecimalUtil::quantizeEqual(Decimal64 *x, Decimal64 y, int exponent)
{
    BSLS_ASSERT(x);
    BSLS_ASSERT(-398 <= exponent);
    BSLS_ASSERT(        exponent <= 369);
    return DecimalImpUtil::quantizeEqual(x->data(), *y.data(), exponent);
}

inline
int DecimalUtil::quantizeEqual(Decimal128 *x, Decimal128 y, int exponent)
{
    BSLS_ASSERT(x);
    BSLS_ASSERT(-6176 <= exponent);
    BSLS_ASSERT(         exponent <= 6111);
    return DecimalImpUtil::quantizeEqual(x->data(), *y.data(), exponent);
}

inline
bool DecimalUtil::sameQuantum(Decimal32 x, Decimal32 y)
{
    return DecimalImpUtil::sameQuantum(*x.data(), *y.data());
}

inline
bool DecimalUtil::sameQuantum(Decimal64 x, Decimal64 y)
{
    return DecimalImpUtil::sameQuantum(*x.data(), *y.data());
}

inline
bool DecimalUtil::sameQuantum(Decimal128 x, Decimal128 y)
{
    return DecimalImpUtil::sameQuantum(*x.data(), *y.data());
}

inline
Decimal32 DecimalUtil::copySign(Decimal32 x, Decimal32 y)
{
    return bdldfp::DecimalImpUtil::copySign(*x.data(), *y.data());
}

inline
Decimal64 DecimalUtil::copySign(Decimal64 x, Decimal64 y)
{
    return bdldfp::DecimalImpUtil::copySign(*x.data(), *y.data());
}

inline
Decimal128 DecimalUtil::copySign(Decimal128 x, Decimal128 y)
{
    return bdldfp::DecimalImpUtil::copySign(*x.data(), *y.data());
}

inline
Decimal32 DecimalUtil::exp(Decimal32 x)
{
    return bdldfp::DecimalImpUtil::exp(*x.data());
}

inline
Decimal64 DecimalUtil::exp(Decimal64 x)
{
    return bdldfp::DecimalImpUtil::exp(*x.data());
}

inline
Decimal128 DecimalUtil::exp(Decimal128 x)
{
    return bdldfp::DecimalImpUtil::exp(*x.data());
}

inline
Decimal32 DecimalUtil::log(Decimal32 x)
{
    return bdldfp::DecimalImpUtil::log(*x.data());
}

inline
Decimal64 DecimalUtil::log(Decimal64 x)
{
    return bdldfp::DecimalImpUtil::log(*x.data());
}

inline
Decimal128 DecimalUtil::log(Decimal128 x)
{
    return bdldfp::DecimalImpUtil::log(*x.data());
}

inline
Decimal32 DecimalUtil::logB(Decimal32 x)
{
    return bdldfp::DecimalImpUtil::logB(*x.data());
}

inline
Decimal64 DecimalUtil::logB(Decimal64 x)
{
    return bdldfp::DecimalImpUtil::logB(*x.data());
}

inline
Decimal128 DecimalUtil::logB(Decimal128 x)
{
    return bdldfp::DecimalImpUtil::logB(*x.data());
}

inline
Decimal32 DecimalUtil::log10(Decimal32 x)
{
    return bdldfp::DecimalImpUtil::log10(*x.data());
}

inline
Decimal64 DecimalUtil::log10(Decimal64 x)
{
    return bdldfp::DecimalImpUtil::log10(*x.data());
}

inline
Decimal128 DecimalUtil::log10(Decimal128 x)
{
    return bdldfp::DecimalImpUtil::log10(*x.data());
}

inline
Decimal32 DecimalUtil::fmod(Decimal32 x, Decimal32 y)
{
    return bdldfp::DecimalImpUtil::fmod(*x.data(), *y.data());
}

inline
Decimal64 DecimalUtil::fmod(Decimal64 x, Decimal64 y)
{
    return bdldfp::DecimalImpUtil::fmod(*x.data(), *y.data());
}

inline
Decimal128 DecimalUtil::fmod(Decimal128 x, Decimal128 y)
{
    return bdldfp::DecimalImpUtil::fmod(*x.data(), *y.data());
}

inline
Decimal32 DecimalUtil::remainder(Decimal32 x, Decimal32 y)
{
    return bdldfp::DecimalImpUtil::remainder(*x.data(), *y.data());
}

inline
Decimal64 DecimalUtil::remainder(Decimal64 x, Decimal64 y)
{
    return bdldfp::DecimalImpUtil::remainder(*x.data(), *y.data());
}

inline
Decimal128 DecimalUtil::remainder(Decimal128 x, Decimal128 y)
{
    return bdldfp::DecimalImpUtil::remainder(*x.data(), *y.data());
}

inline
long int DecimalUtil::lrint(Decimal32  x)
{
    return bdldfp::DecimalImpUtil::lrint(*x.data());
}

inline
long int DecimalUtil::lrint(Decimal64  x)
{
    return bdldfp::DecimalImpUtil::lrint(*x.data());
}

inline
long int DecimalUtil::lrint(Decimal128 x)
{
    return bdldfp::DecimalImpUtil::lrint(*x.data());
}

inline
long long int DecimalUtil::llrint(Decimal32  x)
{
    return bdldfp::DecimalImpUtil::llrint(*x.data());
}

inline
long long int DecimalUtil::llrint(Decimal64  x)
{
    return bdldfp::DecimalImpUtil::llrint(*x.data());
}

inline
long long int DecimalUtil::llrint(Decimal128 x)
{
    return bdldfp::DecimalImpUtil::llrint(*x.data());
}

inline
Decimal32 DecimalUtil::nextafter(Decimal32 x, Decimal32 y)
{
    return bdldfp::DecimalImpUtil::nextafter(*x.data(), *y.data());
}

inline
Decimal64 DecimalUtil::nextafter(Decimal64 x, Decimal64 y)
{
    return bdldfp::DecimalImpUtil::nextafter(*x.data(), *y.data());
}

inline
Decimal128 DecimalUtil::nextafter(Decimal128 x, Decimal128 y)
{
    return bdldfp::DecimalImpUtil::nextafter(*x.data(), *y.data());
}

inline
Decimal32 DecimalUtil::nexttoward(Decimal32 x, Decimal128 y)
{
    return bdldfp::DecimalImpUtil::nexttoward(*x.data(), *y.data());
}

inline
Decimal64 DecimalUtil::nexttoward(Decimal64 x, Decimal128 y)
{
    return bdldfp::DecimalImpUtil::nexttoward(*x.data(), *y.data());
}

inline
Decimal128 DecimalUtil::nexttoward(Decimal128 x, Decimal128 y)
{
    return bdldfp::DecimalImpUtil::nexttoward(*x.data(), *y.data());
}

inline
Decimal32 DecimalUtil::pow(Decimal32 x, Decimal32 y)
{
    return bdldfp::DecimalImpUtil::pow(*x.data(), *y.data());
}

inline
Decimal64 DecimalUtil::pow(Decimal64 x, Decimal64 y)
{
    return bdldfp::DecimalImpUtil::pow(*x.data(), *y.data());
}

inline
Decimal128 DecimalUtil::pow(Decimal128 x, Decimal128 y)
{
    return bdldfp::DecimalImpUtil::pow(*x.data(), *y.data());
}

inline
Decimal32 DecimalUtil::ceil(Decimal32 x)
{
    return bdldfp::DecimalImpUtil::ceil(*x.data());
}

inline
Decimal64 DecimalUtil::ceil(Decimal64 x)
{
    return bdldfp::DecimalImpUtil::ceil(*x.data());
}

inline
Decimal128 DecimalUtil::ceil(Decimal128 x)
{
    return bdldfp::DecimalImpUtil::ceil(*x.data());
}

inline
Decimal32 DecimalUtil::floor(Decimal32 x)
{
    return bdldfp::DecimalImpUtil::floor(*x.data());
}

inline
Decimal64 DecimalUtil::floor(Decimal64 x)
{
    return bdldfp::DecimalImpUtil::floor(*x.data());
}

inline
Decimal128 DecimalUtil::floor(Decimal128 x)
{
    return bdldfp::DecimalImpUtil::floor(*x.data());
}

inline
Decimal32 DecimalUtil::round(Decimal32  x)
{
    return bdldfp::DecimalImpUtil::round(*x.data());
}

inline
Decimal64 DecimalUtil::round(Decimal64  x)
{
    return bdldfp::DecimalImpUtil::round(*x.data());
}

inline
Decimal128 DecimalUtil::round(Decimal128 x)
{
    return bdldfp::DecimalImpUtil::round(*x.data());
}

inline
long int DecimalUtil::lround(Decimal32  x)
{
    return bdldfp::DecimalImpUtil::lround(*x.data());
}

inline
long int DecimalUtil::lround(Decimal64  x)
{
    return bdldfp::DecimalImpUtil::lround(*x.data());
}

inline
long int DecimalUtil::lround(Decimal128 x)
{
    return bdldfp::DecimalImpUtil::lround(*x.data());
}

inline
Decimal32  DecimalUtil::round(Decimal32  x, unsigned int decimalPlaces)
{
    return bdldfp::DecimalImpUtil::round(*x.data(), decimalPlaces);
}

inline
Decimal64 DecimalUtil::round(Decimal64  x, unsigned int decimalPlaces)
{
    return bdldfp::DecimalImpUtil::round(*x.data(), decimalPlaces);
}

inline
Decimal128 DecimalUtil::round(Decimal128 x, unsigned int decimalPlaces)
{
    return bdldfp::DecimalImpUtil::round(*x.data(), decimalPlaces);
}

inline
Decimal32 DecimalUtil::trunc(Decimal32 x)
{
    return bdldfp::DecimalImpUtil::trunc(*x.data());
}

inline
Decimal64 DecimalUtil::trunc(Decimal64 x)
{
    return bdldfp::DecimalImpUtil::trunc(*x.data());
}

inline
Decimal128 DecimalUtil::trunc(Decimal128 x)
{
    return bdldfp::DecimalImpUtil::trunc(*x.data());
}

inline
Decimal32 DecimalUtil::fma(Decimal32  x, Decimal32  y, Decimal32  z)
{
    return bdldfp::DecimalImpUtil::fma(*x.data(), *y.data(), *z.data());
}

inline
Decimal64 DecimalUtil::fma(Decimal64  x, Decimal64  y, Decimal64  z)
{
    return bdldfp::DecimalImpUtil::fma(*x.data(), *y.data(), *z.data());
}

inline
Decimal128 DecimalUtil::fma(Decimal128 x, Decimal128 y, Decimal128 z)
{
    return bdldfp::DecimalImpUtil::fma(*x.data(), *y.data(), *z.data());
}

inline
Decimal32 DecimalUtil::fabs(Decimal32 x)
{
    return bdldfp::DecimalImpUtil::fabs(*x.data());
}

inline
Decimal64 DecimalUtil::fabs(Decimal64 x)
{
    return bdldfp::DecimalImpUtil::fabs(*x.data());
}

inline
Decimal128 DecimalUtil::fabs(Decimal128 x)
{
    return bdldfp::DecimalImpUtil::fabs(*x.data());
}

inline
Decimal32 DecimalUtil::sqrt(Decimal32 x)
{
    return bdldfp::DecimalImpUtil::sqrt(*x.data());
}

inline
Decimal64 DecimalUtil::sqrt(Decimal64 x)
{
    return bdldfp::DecimalImpUtil::sqrt(*x.data());
}

inline
Decimal128 DecimalUtil::sqrt(Decimal128 x)
{
    return bdldfp::DecimalImpUtil::sqrt(*x.data());
}

                       // -----------------------------
                       // class DecimalUtil_CStringUtil
                       // -----------------------------

// CLASS METHODS
inline
const char *DecimalUtil_CStringUtil::flatten(const char *cString)
{
    return cString;
}

inline
const char *DecimalUtil_CStringUtil::flatten(char *cString)
{
    return cString;
}

inline
const char *DecimalUtil_CStringUtil::flatten(const bsl::string& string)
{
    return string.c_str();
}

inline
const char *DecimalUtil_CStringUtil::flatten(const std::string& string)
{
    return string.c_str();
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
inline
const char *DecimalUtil_CStringUtil::flatten(const std::pmr::string& string)
{
    return string.c_str();
}
#endif

inline
bsl::string DecimalUtil_CStringUtil::flatten(
                                            const bslstl::StringRef& stringRef)
{
    return stringRef;
}

template <class TYPE>
inline
const char *DecimalUtil_CStringUtil::flatten(const TYPE&)
{
    BSLMF_ASSERT(("Unsupported parameter type." && !sizeof(TYPE)));
    return 0;
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
