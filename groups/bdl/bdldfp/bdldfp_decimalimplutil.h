// bdldfp_decimalimplutil.h                                           -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALIMPLUTIL
#define INCLUDED_BDLDFP_DECIMALIMPLUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide utilities to help implement decimal floating-point types.
//
//@CLASSES:
//  bdldfp::DecimalImplUtil: Namespace for decimal FP implementation functions
//
//@MACROS:
//  BDLDFP_DECIMALIMPLUTIL_DF: ValueType32  from literal
//  BDLDFP_DECIMALIMPLUTIL_DD: ValueType64  from literal
//  BDLDFP_DECIMALIMPLUTIL_DL: ValueType128 from literal
//
//@SEE ALSO: bdldfp_decimal, bdldfp_decimalplatform
//
//@DESCRIPTION: This component is for internal use only by the
// 'bdldfp_decimal*' components.  Direct use of any names declared in this
// component by any other code invokes undefined behavior.  In other words:
// this code may change, disappear, break, move without notice, and no support
// whatsoever will ever be provided for it.
//
// This component contains:
//
//: o portable decimal floating-point macros that create the implementation
//:   type (C99 or 'decNumber' library)
//:
//: o the decimal floating-point environment/context for 'decNumber' library
//:   (when it is used)
//:
//: o the definitions of the implementation types (to be used, e.g., as members
//:   in the 'DecimalN' types)
//:
//: o the parsing functions that turn literals into values on platforms that do
//:   not yet support decimal floating-point literals, and possibly to use by
//:   stream input operators.
//
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: Parse a decimal floating point literal
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Decimal floating points are used to represent numbers for which the exact
// decimal representation is known and we wish to avoid rounding errors that
// are introduced when the number is represented in binary.  It is sometimes of
// interest to create a decimal floating point value from its string
// representation.  The functions of this component allow us to work with types
// for which we may create representations for 32-, 64-, and 128-bit decimal
// floating point values.
//
// As an example, one mile is equal to '1.6093' kilometers exactly.  First, we
// create the 64-bit pattern for the 64-bit decimal floating point value of
// '1.6093'.
//..
// BloombergLP::bdldfp::DecimalImplUtil::ValueType64 kilometersPerMile =
//     BloombergLP::bdldfp::DecimalImplUtil::parse64("1.6093");
//..
// Now, if a mantissa and an exponent within range of the decimal floating
// point type are given, we can also create the decimal floating point value of
// interest with the functions of ''makeDecimalRawXX.  For example, the 32-bit
// decimal floating point type can store 7 mantissa digits and has an exponent
// range of '-101' to '90' inclusive.  It therefore may fit a mantissa of
// '16093' and an exponent of '-4', representing the value of '1.6093' ('16093'
// times ten to the power of '-4').  We create this value using
// 'makeDecimalRaw32'.
//..
// BloombergLP::bdldfp::DecimalImplUtil::ValueType32
//     kilometersPerMileMakeDecimal =
//         BloombergLP::bdldfp::DecimalImplUtil::makeDecimalRaw32(16093, -4);
//..
// Finally, we verify that we obtain the same result without introducing
// rounding errors with both implementations.
//..
// assert(BloombergLP::bdldfp::DecimalImplUtil::equals(kilometersPerMile,
//      kilometersPerMileMakeDecimal));
//..
//
///Example 2: Create a 64-bit decimal floating point with possible adjustment
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to create a 'bdldfp::Decimal64', but have a mantissa and/or
// the exponent that is out of the range that can be exactly represented by a
// 64-bit decimal floating point type.  Notice that a 64-bit decimal floating
// point value may represent a mantissa of up to 16 figures, and an exponent
// between '-6176' and '6111' inclusive.  For example, on February 13, 2014,
// the US National Debt in Japanese Yen is approximately
// '1,765,002,361,949,679.79' Japanese Yen.  This number can be represented
// with a mantissa of '176500236194967979' and an exponent of '-2'.  First, we
// create this number.
//..
// BloombergLP::bdldfp::DecimalImplUtil::ValueType64 usNationalDebtInJpy =
//     BloombergLP::bdldfp::DecimalImplUtil::makeDecimal64(
//         176500236194967979ull, -2);
//..
// Notice that '176500236194967979' contains 18 significant figures, more than
// can be represented by a 64-bit decimal floating point type.
//
// Now, the number can also be parsed as a string.
//..
// BloombergLP::bdldfp::DecimalImplUtil::ValueType64
//     usNationalDebtInJpyParse =
//        BloombergLP::bdldfp::DecimalImplUtil::parse64("1765002361949679.79");
//..
// Finally, we verify that we obtain the same result.
//..
// assert(BloombergLP::bdldfp::DecimalImplUtil::equals(usNationalDebtInJpy,
//      usNationalDebtInJpyParse));
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#include <bdldfp_decimalplatform.h>
#endif

#if BDLDFP_DECIMALPLATFORM_DECNUMBER
# ifndef INCLUDED_DECSINGLE
 extern "C" {
#  include <decSingle.h>
}
#  define INCLUDED_DECSINGLE
# endif
#endif

#if BDLDFP_DECIMALPLATFORM_C99_TR

        // Implementation when we have C DecFP support only (no C++)

#define BDLDFP_DECIMALIMPLUTIL_JOIN_(a,b) a##b
    // Helper macro to create floating-point decimal literals

#elif BDLDFP_DECIMALPLATFORM_DECNUMBER


            // End of decNumber-base implementation specific area

#elif BDLDFP_DECIMALPLATFORM_INTELDFP


#ifndef INCLUDED_BID_FUNCTIONS

// Controlling macros for the intel library configuration

#  define DECIMAL_CALL_BY_REFERENCE      0
#  define DECIMAL_GLOBAL_ROUNDING        1
#  define DECIMAL_GLOBAL_EXCEPTION_FLAGS 1

// in C++, there's always a 'wchar_t' type, so we need to tell Intel's library
// about this.

#  define _WCHAR_T_DEFINED

   extern "C" {
#   include <bid_conf.h>
#   include <bid_functions.h>
   }
#  define INCLUDED_BID_FUNCTIONS
#endif

#else
namespace BloombergLP {
namespace bdldfp {

struct DecimalImpl_Assert;
    // This 'struct' is deliberately not defined, and is declared only to help
    // force a compilation error below, for badly configured builds.

#  error Unknown architecture, decimal floating-point not supported.
char die[sizeof(DecimalImpl_Assert)];     // if '#error' unsupported

}  // close package namespace
}  // close enterpise namespace
#endif


#if BDLDFP_DECIMALPLATFORM_SOFTWARE

                // DECIMAL FLOATING-POINT LITERAL EMULATION


#define BDLDFP_DECIMALIMPLUTIL_DF(lit)                                        \
    BloombergLP::bdldfp::DecimalImplUtil::parse32(                            \
        (BloombergLP::bdldfp::DecimalImplUtil::checkLiteral(lit), #lit))

#define BDLDFP_DECIMALIMPLUTIL_DD(lit)                                        \
    BloombergLP::bdldfp::DecimalImplUtil::parse64(                            \
        (BloombergLP::bdldfp::DecimalImplUtil::checkLiteral(lit), #lit))

#define BDLDFP_DECIMALIMPLUTIL_DL(lit)                                        \
    BloombergLP::bdldfp::DecimalImplUtil::parse128(                           \
        (BloombergLP::bdldfp::DecimalImplUtil::checkLiteral(lit), #lit))

#elif BDLDFP_DECIMALPLATFORM_C99_TR

               // Portable decimal floating-point literal support

#define BDLDFP_DECIMALIMPLUTIL_DF(lit) BDLDFP_DECIMALIMPLUTIL_JOIN_(lit,df)

#define BDLDFP_DECIMALIMPLUTIL_DD(lit) BDLDFP_DECIMALIMPLUTIL_JOIN_(lit,dd)

#define BDLDFP_DECIMALIMPLUTIL_DL(lit) BDLDFP_DECIMALIMPLUTIL_JOIN_(lit,dl)

#endif


namespace BloombergLP {
namespace bdldfp {

                          // =====================
                          // class DecimalImplUtil
                          // =====================

struct DecimalImplUtil {
    // This 'struct' provides a namespace for implementation functions that
    // work in terms of the underlying C-style decimal floating point
    // implementation (e.g., decNumber library or compiler implementation of
    // the C Decimal TR).

    // TYPES
#if BDLDFP_DECIMALPLATFORM_C99_TR

    typedef _Decimal32  ValueType32;
    typedef _Decimal64  ValueType64;
    typedef _Decimal128 ValueType128;

#elif BDLDFP_DECIMALPLATFORM_INTELDFP

    struct ValueType32  { BID_UINT32  d_raw; };
    struct ValueType64  { BID_UINT64  d_raw; };
    struct ValueType128 { BID_UINT128 d_raw; };

#elif BDLDFP_DECIMALPLATFORM_DECNUMBER

    typedef decSingle ValueType32;
    typedef decDouble ValueType64;
    typedef decQuad   ValueType128;

    static decContext *getDecNumberContext();
        // Return a pointer providing modifiable access to the floating point
        // environment of the 'decNumber' library.  This function exists on
        // certain supported platforms only.

#endif


    // CLASS METHODS
#if BDLDFP_DECIMALPLATFORM_SOFTWARE

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

#elif BDLDFP_DECIMALPLATFORM_HARDWARE

#else

#error Improperly configured decimal floating point platform settings

#endif

    // CLASS METHODS

                      // Parsing and formatting

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

    static ValueType32  makeDecimalRaw32(int          mantissa,
                                         int          exponent);
        // Create a 'ValueType32' object representing a decimal floating point
        // number consisting of the specified 'mantissa' and 'exponent', with
        // the sign given by the specified 'mantissa'.  If 'mantissa' is 0,
        // the result is 0 but the quanta of the result is unspecified.  The
        // behavior is undefined unless 'abs(mantissa) <= 9,999,999' and
        // '-101 <= exponent <= 90'.

    static ValueType64  makeDecimalRaw64(unsigned long long mantissa,
                                         int                exponent);
    static ValueType64  makeDecimalRaw64(long long          mantissa,
                                         int                exponent);
    static ValueType64  makeDecimalRaw64(unsigned int       mantissa,
                                         int                exponent);
    static ValueType64  makeDecimalRaw64(int                mantissa,
                                         int                exponent);
        // Create a 'ValueType64' object representing a decimal floating point
        // number consisting of the specified 'mantissa' and 'exponent', with
        // the sign given by the specified 'mantissa'.  If 'mantissa' is 0, the
        // result is 0 but the quanta of the result is unspecified.  The
        // behavior is undefined unless
        // 'abs(mantissa) <= 9,999,999,999,999,999' and
        // '-398 <= exponent <= 369'.

    static ValueType128 makeDecimalRaw128(unsigned long long mantissa,
                                          int                exponent);
    static ValueType128 makeDecimalRaw128(long long          mantissa,
                                          int                exponent);
    static ValueType128 makeDecimalRaw128(unsigned int       mantissa,
                                          int                exponent);
    static ValueType128 makeDecimalRaw128(int                mantissa,
                                          int                exponent);
        // Create a 'ValueType128' object representing a decimal floating point
        // number consisting of the specified 'mantissa' and 'exponent', with
        // the sign given by the specified 'mantissa'.  If 'mantissa' is 0, the
        // result is 0 but the quanta of the result is unspecified.  The
        // behavior is undefined unless '-6176 <= exponent <= 6111'.

    static ValueType64  makeDecimal64(unsigned long long mantissa,
                                      int                exponent);
    static ValueType64  makeDecimal64(long long          mantissa,
                                      int                exponent);
    static ValueType64  makeDecimal64(unsigned int       mantissa,
                                      int                exponent);
    static ValueType64  makeDecimal64(int                mantissa,
                                      int                exponent);
        // Create a 'ValueType64' object representing a decimal floating point
        // number having the value given by the specified 'mantissa' times ten
        // to the power of the specified 'exponent'.  This function will return
        // 'inf' with the sign of 'mantissa' if this number has a magnitude too
        // great to be represented, and '0' if this number has a magnitude too
        // small to be represented.  In the event that
        // 'abs(mantissa) <= 9,999,999,999,999,999' and
        // '-398 <= exponent <= 369', a 'ValueType64' object representing a
        // decimal floating point value consisting of 'mantissa' and
        // 'exponent', with the sign given by 'mantissa', is returned.
        // Otherwise, rounding will be performed.  The number closest to this
        // value that may be represented by a 'ValueType64' is returned.  Note
        // that this function can never return 'NaN'.  Also note that this
        // function may return +0, rather than -0 (as required by the IEEE
        // 754-2008 standard), for negative values whose magnitude is too small
        // to represented, this reflects the (non-conforming) behavior of the
        // the underlying 'decNumber' library implementation.

                        // Comparison functions.

    static bool equals(ValueType32  lhs, ValueType32  rhs);
    static bool equals(ValueType32  lhs, ValueType64  rhs);
    static bool equals(ValueType32  lhs, ValueType128 rhs);
    static bool equals(ValueType64  lhs, ValueType32  rhs);
    static bool equals(ValueType64  lhs, ValueType64  rhs);
    static bool equals(ValueType64  lhs, ValueType128 rhs);
    static bool equals(ValueType128 lhs, ValueType32  rhs);
    static bool equals(ValueType128 lhs, ValueType64  rhs);
    static bool equals(ValueType128 lhs, ValueType128 rhs);
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

};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================
#if BDLDFP_DECIMALPLATFORM_SOFTWARE

                          // ---------------------
                          // class DecimalImplUtil
                          // ---------------------

template <class TYPE>
inline
void DecimalImplUtil::checkLiteral(const TYPE& t)
{
    (void)static_cast<This_is_not_a_floating_point_literal>(t);
}

inline
void DecimalImplUtil::checkLiteral(double)
{
}
#endif


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
