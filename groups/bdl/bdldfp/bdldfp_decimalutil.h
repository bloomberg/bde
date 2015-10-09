// bdldfp_decimalutil.h                                               -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALUTIL
#define INCLUDED_BDLDFP_DECIMALUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMAL
#include <bdldfp_decimal.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#include <bdldfp_decimalplatform.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_CMATH
#include <bsl_cmath.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS) && !defined(FP_NAN)

// MS does not provide standard floating-point classification in math so we do

// First, make sure that the environment is sane

#if defined(FP_NORMAL) || defined(FP_INFINITE) || defined(FP_ZERO) || \
    defined(FP_SUBNORMAL)
#error Standard FP_ macros are not defined properly.
#endif

// Make it look like stiff MS has in ymath.h

#define FP_SUBNORMAL (-2)
#define FP_NORMAL    (-1)
#define FP_ZERO        0
#define FP_INFINITE    1
#define FP_NAN         2

#endif

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
        // with the sign given by the 'significand' (if signed).  If
        // 'significand' is 0, the result is 0 but the quanta of the result is
        // unspecified.  The behavior is undefined unless
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
        // If 'significand' is 0, the result is 0 but the quanta of the result
        // is unspecified.  The behavior is undefined unless
        // '-6176 <= exponent <= 6111'.

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
        // if necessary.  If an overflow condition occurs. store the value of
        // the macro 'ERANGE' into 'errno' and return infinity with the
        // appropriate sign.

    static int parseDecimal32( Decimal32  *out, const char *str);
    static int parseDecimal64( Decimal64  *out, const char *str);
    static int parseDecimal128(Decimal128 *out, const char *str);
    static int parseDecimal32( Decimal32  *out, const bsl::string& str);
    static int parseDecimal64( Decimal64  *out, const bsl::string& str);
    static int parseDecimal128(Decimal128 *out, const bsl::string& str);
        // Load into the specified 'out' the decimal floating point number
        // described by the specified 'str'; return zero if the conversion was
        // successful and non-zero otherwise.  The value of 'out' is
        // unspecified if the function returns a non-zero value.


                                  // math

    static Decimal64  fma(Decimal64  x, Decimal64  y, Decimal64  z);
    static Decimal128 fma(Decimal128 x, Decimal128 y, Decimal128 z);
        // Return, using the specified 'x', 'y', and 'z', the value of the
        // expression 'x * y + z', rounded as one ternary operation according
        // to the current decimal floating point rounding mode.


                       // Selecting, converting functions

    static Decimal32  fabs(Decimal32  value);
    static Decimal64  fabs(Decimal64  value);
    static Decimal128 fabs(Decimal128 value);
        // Return the absolute value of the specified 'value'.  Note that the
        // absolute value of NaN is NaN.  The absolute values of negative zero
        // or infinity are positive zero and infinity respectively.

    // TODO TBD priority 2
    // static Decimal32 fmax(Decimal32 x, Decimal32 y);
        // Return the larger value of the specified 'x' and 'y'.  If one
        // argument is NaN, return the other argument.  If both arguments are
        // NaN, return NaN.

    // TODO TBD priority 2
    // static Decimal32 fmin(Decimal32 x, Decimal32 y);
        // Return the smaller value of the specified 'x' and 'y'.  If one
        // argument is NaN, return the other argument.  If both arguments are
        // NaN, return NaN.

    // TODO TBD priority N static Decimal32 copysign(Decimal32 x, Decimal32 y);
        // Return a value whose absolute value matches that of the specified
        // 'x' and whose sign bit matches that of the specified 'y'.  If 'x' is
        // NaN, a NaN with the sign bit of 'y' is returned.

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
        // specified 'x'.  If 'x' is integral, plus zero, minus zero, NaN, or
        // infinity 'x' return 'x' itself.
        // Examples: 'ceil(0.5)' ==> 1.0; 'ceil(-0.5)' ==> 0.0

    static Decimal32  floor(Decimal32  x);
    static Decimal64  floor(Decimal64  x);
    static Decimal128 floor(Decimal128 x);
        // Return the largest integral value that is not greater than the
        // specified 'x'.  If 'x' is integral, positive zero, negative zero,
        // NaN, or infinity 'x' return 'x' itself.
        // Examples: 'floor(0.5)' ==> 0.0; 'floor(-0.5)' ==> -1.0

    static Decimal32  round(Decimal32  x);
    static Decimal64  round(Decimal64  x);
    static Decimal128 round(Decimal128 x);
        // Return the integal value nearest to the specified 'x'.  Round
        // halfway cases away from zero, regardless of the current decimal
        // floating point rounding mode.  If 'x' is integral, positive zero,
        // negative zero, NaN, or infinity then return 'x' itself.
        // Examples: 'round(0.5)' ==> 1.0; 'round(-0.5)' ==> -1.0

    static Decimal32  trunc(Decimal32  x);
    static Decimal64  trunc(Decimal64  x);
    static Decimal128 trunc(Decimal128 x);
        // Return the nearest integal value that is not greater in absolute
        // value than the specified 'x'.  If 'x' is integral, NaN, or infinity
        // then return 'x' itself.
        // Examples: 'trunc(0.5)' ==> 0.0; 'trunc(-0.5)' ==> 0.0

                             // Quantum functions

    static Decimal64  multiplyByPowerOf10(Decimal64  value,
                                          int        exponent);
    static Decimal64  multiplyByPowerOf10(Decimal64  value,
                                          Decimal64  exponent);
    static Decimal128 multiplyByPowerOf10(Decimal128 value,
                                          int        exponent);
    static Decimal128 multiplyByPowerOf10(Decimal128 value,
                                          Decimal128 exponent);
        // Return the result of multiplying the specified 'value' by ten raised
        // to the specified 'exponent'.  The quantum of 'value' is scaled
        // according to IEEE 754's 'scaleB' operations.  The result is
        // unspecified if 'value' is NaN or infinity.  The behavior is
        // undefined unless '-1999999997 <= y <= 99999999'.

    static Decimal64  quantize(Decimal64  value, Decimal64  exponent);
    static Decimal128 quantize(Decimal128 value, Decimal128 exponent);
        // Return a number that is equal in value (except for any rounding) and
        // sign to the specified 'value', and which has the exponent of the
        // specified 'exponent'.  If the exponent needs to be increased, round
        // the value according to the current decimal floating point rounding
        // mode; and if the result of the rounding is not equal to the value of
        // 'value'.  If the exponent needs to be decreased and the significant
        // of the result has more digits than the type would allow, return NaN.
        // The returned value is unspecified if either operand is NaN or
        // infinity of either sign.  Note that the 'invalid' and 'inexact'
        // floating-point exception may be raised.  Also note that the AIX
        // hardware function of '__d64_quantize' and '__d128_quantize',
        // produces some results contrary to N1312 on operands of infinity and
        // Nan.  Note that this function does not guarantee behavior consistent
        // with Decimal TR N1312 for infinity and NaN because the XLC compiler
        // intrinsics ('__d6_quantize' and '__d128_quantize') are
        // non-conformant.

    static int quantum(Decimal64  value);
    static int quantum(Decimal128 value);
        // Return an integer equal to the exponent field in the specified
        // 'value'.  Each decimal floating point number is a representation of
        // the ideal form 's * (10 ** e)', where 's' is significand and 'e' is
        // exponent.  This function returns that exponent value.  The behavior
        // is undefined if 'value' is NaN or 'value' is infinity.

    static bool sameQuantum(Decimal64  x, Decimal64  y);
    static bool sameQuantum(Decimal128 x, Decimal128 y);
        // Return 'true' if the specified 'x' and 'y' values have the same
        // quantum exponents, and 'false' otherwise.  If both arguments are NaN
        // or both arguments are infinity, they have the same quantum
        // exponents.  Note that if exactly one operand is NaN or exactly one
        // operand is infinity, they do not have the same quantum exponents.
        // Also note that this function will raise no exceptions.

    // TBD: The following functions would be logically consistent, but are not
    // provided since there its not clear whether there is a demand for them.

    //static int parseDecimal32(const wchar_t      *str, Decimal32 *out);
    //static int parseDecimal32(const bsl::wstring& str, Decimal32 *out);

    // static Decimal32 nextafter(Decimal32 x, Decimal32 y);
        // Return the next representable floating point value following the
        // specified 'x' in the direction of the specified 'y'.  If 'x' or 'y'
        // is NaN, NaN is returned.  If 'x' and 'y' are equal, 'y' is returned.
        // If 'x' is finite and the result would overflow an
        // 'overflow exception' is raised and the function will return
        // 'HUGE_VAL_D32'.  If 'x' is not equal to 'y' and the correct result
        // would be subnormal, zero or underflow a 'range error' occurs and the
        // correct value - if representable -- or 0.0 is returned.

    // static Decimal32 remainder(Decimal32 x, Decimal32 y);
        // Return the next remainder of dividing of the specified 'x' with the
        // specified 'y' such as that the return value is 'x-n*y', where 'n' is
        // the nearest integer of the value of 'x/y'.  If the absolute value of
        // the return value ('x-n*y') is 0.5, 'n' is chosen to be even.  If 'x'
        // or 'y' is NaN, NaN is returned.  If 'x' is infinity and 'y' is not
        // NaN, the 'invalid floating point expection' is raised and NaN is
        // returned.  If 'y' is zero and 'x' is not NaN, the
        // 'invalid floating point expection' is raised and NaN is returned.

    // static Decimal32 rint(Decimal32 x);
        // Return the integal value nearest to the specified 'x'.  Round
        // halfway according to the current decimal floating point rounding
        // mode.  Raise the 'inexact exception' if the return value differs
        // from the argument 'x' in value.  If 'x' is integral, plus zero,
        // minus zero, NaN, or infinity then return 'x' itself.

    // static long int lround(Decimal32 x);
    // static long long int llround(Decimal32 x);
        // Return the integal value nearest to the specified 'x', rounding away
        // from zero regardless of the current decimal floating point rounding
        // mode.  If the specified 'x' is NaN, infinity, or too large a value
        // to be stored in the return type raise a 'domain exception', the
        // return value in such case is unspecified.  Note that these
        // functions, unlike the other rounding functions, return different
        // types than their argument type.

    // static bool isGreater(Decimal32 x, Decimal32 y);
    // static bool isGreaterEqual(Decimal32 x, Decimal32 y);
    // static bool isLess(Decimal32 x, Decimal32 y);
    // static bool isEqual(Decimal32 x, Decimal32 y);
    // static bool isLessEqual(Decimal32 x, Decimal32 y);
        // Compare the specified 'x' and 'y' value without setting any floating
        // point exceptions.  Return false if either of the arguments is a NaN.



};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

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

                             // Quantum functions

inline
Decimal64 DecimalUtil::multiplyByPowerOf10(Decimal64 value, int exponent)
{
    BSLS_ASSERT(-1999999997 <= exponent);
    BSLS_ASSERT(               exponent <= 99999999);

    return bdldfp::DecimalImpUtil::scaleB(*value.data(), exponent);
}

inline
Decimal128 DecimalUtil::multiplyByPowerOf10(Decimal128 value, int exponent)
{
    BSLS_ASSERT(-1999999997 <= exponent);
    BSLS_ASSERT(               exponent <= 99999999);

    return bdldfp::DecimalImpUtil::scaleB(*value.data(), exponent);
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
