// bdldfp_decimalconvertutil_ibmxlc.h                                 -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALCONVERTUTIL_IBMXLC
#define INCLUDED_BDLDFP_DECIMALCONVERTUTIL_IBMXLC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide decimal floating-point conversion functions.
//
//@CLASSES:
//  bdldfp::DecimalConvertUtil_IbmXlc: IBM xlC conversion functions
//
//@SEE ALSO: bdldfp_decimal, bdldfp_decimalplatform
//
//@DESCRIPTION:
// This component provides conversion operations between the decimal types
// supplied in this package ('Decimal32', 'Decimal64', 'Decimal128') and
// various alternative representations.  Some of the alternative
// representations that this component provides conversions for are IEEE-754
// binary floating point (i.e., 'float' and 'double') and a network format
// (big-endian, Densely Packed Decimal encoding).
//
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: None.
/// - - - - - - - -

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMAL
#include <bdldfp_decimal.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL
#include <bdldfp_decimalimputil.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#include <bdldfp_decimalplatform.h>
#endif

#ifdef BDLDFP_DECIMALPLATFORM_C99_TR

namespace BloombergLP {
namespace bdldfp {
                        // ===============================
                        // class DecimalConvertUtil_IbmXlc
                        // ===============================

struct DecimalConvertUtil_IbmXlc {
    // This 'struct' provides a namespace for utility functions that convert
    // between the decimal floating-point types of 'bdldfp_decimal' and various
    // other formats.

    // Convert to Binary Floating-Point from C++ Decimal TR

    // CLASS METHODS

                        // decimalToLongDouble functions

    static long double decimal32ToLongDouble (Decimal32  decimal);
    static long double decimal64ToLongDouble (Decimal64  decimal);
    static long double decimal128ToLongDouble(Decimal128 decimal);
    static long double decimalToLongDouble   (Decimal32  decimal);
    static long double decimalToLongDouble   (Decimal64  decimal);
    static long double decimalToLongDouble   (Decimal128 decimal);
        // Return a 'long double' object having the value closest to the value
        // of the specified 'decimal' object following the conversion rules
        // defined by IEEE-754:
        //
        //: o If the 'decimal' object is a NaN, return a NaN.
        //:
        //: o Otherwise if 'decimal' is positive or negative infinity, return
        //:   infinity of the same sign.
        //:
        //: o Otherwise if 'decimal' is positive or negative zero, return zero
        //:   of the same sign.
        //:
        //: o Otherwise if 'decimal' object has an absolute value that is
        //:   larger than 'std::numeric_limits<long double>::max()', raise the
        //:   "overflow" floating-point exception and return infinity of the
        //:   same sign as 'decimal'.
        //:
        //: o Otherwise if 'decimal' has an absolute value that is smaller than
        //:   'std::numeric_limits<long double>::min()', raise the "underflow"
        //:   floating-point exception and return zero of the same sign as
        //:   'decimal'.
        //:
        //: o Otherwise if 'decimal' has a value that has more significant
        //:   base-10 digits than 'std::numeric_limits<long double>::digits10',
        //:   raise the "inexact" floating-point exception, round that value
        //:   according to the *binary* rounding direction setting of the
        //:   floating-point environment, and return the result of that.
        //:
        //: o Otherwise if 'decimal' has a significand that cannot be exactly
        //:   represented using binary floating-point, raise the "inexact"
        //:   floating-point exception, roundthat value according to the
        //:   *binary* rounding direction setting of the environment, and
        //:   return the result of that.
        //:
        //: o Otherwise use the exact value of the 'other' object for the
        //:   initialization if this object.

                        // decimalToDouble functions

    static double decimal32ToDouble (Decimal32  decimal);
    static double decimal64ToDouble (Decimal64  decimal);
    static double decimal128ToDouble(Decimal128 decimal);
    static double decimalToDouble   (Decimal32  decimal);
    static double decimalToDouble   (Decimal64  decimal);
    static double decimalToDouble   (Decimal128 decimal);
        // Return a 'double' object having the value closest to the value of
        // the specified 'decimal' object following the conversion rules
        // defined by IEEE-754:
        //
        //: o If the 'decimal' object is a NaN, return a NaN.
        //:
        //: o Otherwise if 'decimal' is positive or negative infinity, return
        //:   infinity of the same sign.
        //:
        //: o Otherwise if 'decimal' is positive or negative zero, return zero
        //:   of the same sign.
        //:
        //: o Otherwise if 'decimal' object has an absolute value that is
        //:   larger than 'std::numeric_limits<double>::max()', raise the
        //:   "overflow" floating-point exception and return infinity of the
        //:   same sign as 'decimal'.
        //:
        //: o Otherwise if 'decimal' has an absolute value that is smaller than
        //:   'std::numeric_limits<double>::min()', raise the "underflow"
        //:   floating-point exception and return zero of the same sign as
        //:   'decimal'.
        //:
        //: o Otherwise if 'decimal' has a value that has more significant
        //:   base-10 digits than 'std::numeric_limits<double>::digits10',
        //:   raise the "inexact" floating-point exception, round that value
        //:   according to the *binary* rounding direction setting of the
        //:   floating-point environment, and return the result of that.
        //:
        //: o Otherwise if 'decimal' has a significand that cannot be exactly
        //:   represented using binary floating-point, raise the "inexact"
        //:   floating-point exception, roundthat value according to the
        //:   *binary* rounding direction setting of the environment, and
        //:   return the result of that.
        //:
        //: o Otherwise use the exact value of the 'other' object for the
        //:   initialization if this object.

                        // decimalToFloat functions

    static float decimal32ToFloat (Decimal32  decimal);
    static float decimal64ToFloat (Decimal64  decimal);
    static float decimal128ToFloat(Decimal128 decimal);
    static float decimalToFloat   (Decimal32  decimal);
    static float decimalToFloat   (Decimal64  decimal);
    static float decimalToFloat   (Decimal128 decimal);
        // Return a 'float' object having the value closest to the value of the
        // specified 'decimal' object following the conversion rules defined
        // by IEEE-754:
        //
        //: o If the 'decimal' object is a NaN, return a NaN.
        //:
        //: o Otherwise if 'decimal' is positive or negative infinity, return
        //:   infinity of the same sign.
        //:
        //: o Otherwise if 'decimal' is positive or negative zero, return zero
        //:   of the same sign.
        //:
        //: o Otherwise if 'decimal' object has an absolute value that is
        //:   larger than 'std::numeric_limits<long double>::max()', raise the
        //:   "overflow" floating-point exception and return infinity of the
        //:   same sign as 'decimal'.
        //:
        //: o Otherwise if 'decimal' has an absolute value that is smaller than
        //:   'std::numeric_limits<float>::min()', raise the "underflow"
        //:   floating-point exception and return zero of the same sign as
        //:   'decimal'.
        //:
        //: o Otherwise if 'decimal' has a value that has more significant
        //:   base-10 digits than 'std::numeric_limits<float>::digits10',
        //:   raise the "inexact" floating-point exception, round that value
        //:   according to the *binary* rounding direction setting of the
        //:   floating-point environment, and return the result of that.
        //:
        //: o Otherwise if 'decimal' has a significand that cannot be exactly
        //:   represented using binary floating-point, raise the "inexact"
        //:   floating-point exception, roundthat value according to the
        //:   *binary* rounding direction setting of the environment, and
        //:   return the result of that.
        //:
        //: o Otherwise use the exact value of the 'other' object for the
        //:   initialization if this object.

                        // decimalFromLongDouble functions

    static Decimal32  decimal32FromLongDouble (long double binary);
    static Decimal64  decimal64FromLongDouble (long double binary);
    static Decimal128 decimal128FromLongDouble(long double binary);
        // Return the original decimal floating-point value stored in the
        // specified 'binary' floating-point value by a call to the
        // corresponding 'decimalToLongDouble' function earlier.  Thus this
        // function provides a limited decimal-binary-decimal round-trip
        // conversion when used together with 'decimalToLongDouble'.  The
        // behavior is undefined:
        //
        //: o unless 'std::numeric_limits<long double>::radix == 2'.
        //:
        //: o unless the decimal is read back into the same size decimal type
        //    that was passed as argument to 'decimalToLongDouble'.
        //:
        //: o unless the decimal is read back from an unchanged 'long double'
        //:   returned by 'decimalToLongDouble'.
        //:
        //: o if the decimal originally stored into the 'long double' had more
        //:   than 'std::numeric_limits<long double>::digits10' significant
        //:   digits.
        //:
        //: o if the absolute value of the decimal originally stored into the
        //:   'long double' was larger than
        //:   'std::numeric_limits<long double>::max()'.
        //:
        //: o If the absolute value of the decimal originally stored into the
        //:   'long double' was larger than
        //:   'std::numeric_limits<long double>::min()'.
        //
        // Note that the purpose of this function is to restore a decimal value
        // that has been stored earlier into a base-2 floating-point type and
        // *not* to create a decimal from the exact base-2 value.  Use the
        // conversion constructors when you are not restoring a decimal.

                        // decimalFromDouble functions

    static Decimal32  decimal32FromDouble (double binary);
    static Decimal64  decimal64FromDouble (double binary);
    static Decimal128 decimal128FromDouble(double binary);
        // Return the original decimal floating-point value stored in the
        // specified 'binary' floating-point value by a call to the
        // corresponding 'decimalToDouble' function earlier.  Thus this
        // function provides a limited decimal-binary-decimal round-trip
        // conversion when used together with 'decimalToDouble'.  The behavior
        // is undefined:
        //
        //: o unless 'std::numeric_limits<long double>::radix == 2'.
        //:
        //: o unless the decimal is read back into the same size decimal type
        //    that was passed as argument to 'decimalToFloat'.
        //:
        //: o unless the decimal is read back from an unchanged 'double'
        //:   returned by 'decimalToFloat'.
        //:
        //: o if the decimal originally stored into the 'long double' had more
        //:   than 'std::numeric_limits<double>::digits10' significant digits.
        //:
        //: o if the absolute value of the decimal originally stored into the
        //:   'long double' was larger than
        //:   'std::numeric_limits<double>::max()'.
        //:
        //: o if the absolute value of the decimal originally stored into the
        //:   'double' was larger than 'std::numeric_limits<double>::min()'.
        //
        // Note that the purpose of this function is to restore a decimal value
        // that has been stored earlier into a base-2 floating-point type and
        // *not* to create a decimal from the exact base-2 value.  Use the
        // conversion constructors when you are not restoring a decimal.

                        // decimalFromFloat functions

    static Decimal32  decimal32FromFloat (float binary);
    static Decimal64  decimal64FromFloat (float binary);
    static Decimal128 decimal128FromFloat(float binary);
        // Return the original decimal floating-point value stored in the
        // specified 'binary' floating-point value by a call to the
        // corresponding 'decimalToFloat' function earlier.  Thus this
        // function provides a limited decimal-binary-decimal round-trip
        // conversion when used together with 'decimalToFloat'.  The behavior
        // is undefined:
        //
        //: o unless 'std::numeric_limits<float>::radix == 2'.
        //:
        //: o unless the decimal is read back into the same size decimal type
        //    that was passed as argument to 'decimalToFloat'.
        //:
        //: o unless the decimal is read back from an unchanged 'float'
        //:   returned by 'decimalToFloat'.
        //:
        //: o if the decimal originally stored into the 'float' had more than
        //:   'std::numeric_limits<float>::digits10' significant digits.
        //:
        //: o if the absolute value of the decimal originally stored into the
        //:   'long double' was larger than
        //:   'std::numeric_limits<float>::max()'.
        //:
        //: o if the absolute value of the decimal originally stored into the
        //:   'float' was larger than 'std::numeric_limits<float>::min()'.
        // Note that the purpose of this function is to restore a decimal value
        // that has been stored earlier into a base-2 floating-point type and
        // *not* to create a decimal from the exact base-2 value.  Use the
        // conversion constructors when you are not restoring a decimal.

                        // decimalToDenselyPacked functions

    static void decimal32ToDenselyPacked( unsigned char *buffer,
                                          Decimal32      decimal);
    static void decimal64ToDenselyPacked( unsigned char *buffer,
                                          Decimal64      decimal);
    static void decimal128ToDenselyPacked(unsigned char *buffer,
                                          Decimal128     decimal);
    static void decimalToDenselyPacked(   unsigned char *buffer,
                                          Decimal32      decimal);
    static void decimalToDenselyPacked(   unsigned char *buffer,
                                          Decimal64      decimal);
    static void decimalToDenselyPacked(   unsigned char *buffer,
                                          Decimal128     decimal);
        // Populate the specified 'buffer' with the Densely Packed Decimal
        // (DPD) representation of the specified 'decimal' value.  The DPD
        // representations of 'Decimal32', 'Decimal64', and 'Decimal128'
        // require 4, 8, and 16 bytes respectively.  The behavior is undefined
        // unless 'buffer' points to a contiguous sequence of at least
        // 'sizeof(decimal)' bytes.  Note that the DPD representation is
        // defined in section 3.5 of IEEE 754-2008.

                        // decimalFromDenselyPacked functions

    static Decimal32  decimal32FromDenselyPacked( const unsigned char *buffer);
    static Decimal64  decimal64FromDenselyPacked( const unsigned char *buffer);
    static Decimal128 decimal128FromDenselyPacked(const unsigned char *buffer);
        // Return the native implementation representation of the value of the
        // same size base-10 floating-point value stored in Densely Packed
        // Decimal format at the specified 'buffer' address.  The behavior is
        // undefined unless 'buffer' points to a memory area at least
        // 'sizeof(decimal)' in size containing a value in DPD format.

    static void decimalFromDenselyPacked(Decimal32           *decimal,
                                         const unsigned char *buffer);
    static void decimalFromDenselyPacked(Decimal64           *decimal,
                                         const unsigned char *buffer);
    static void decimalFromDenselyPacked(Decimal128          *decimal,
                                         const unsigned char *buffer);
        // Store, into the specified 'decimal', the native implmentation
        // representation of the value of the same size base-10 floating point
        // value represented in Densely Packed Decimal format, at the specified
        // 'buffer' address.  The behavior is undefined unless 'buffer' points
        // to a memory area at least 'sizeof(decimal)' in size containing a
        // value in DPD format.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -------------------------------
                        // class DecimalConvertUtil_IbmXlc
                        // -------------------------------

                        // decimalToLongDouble functions

inline
long double
DecimalConvertUtil_IbmXlc::decimal32ToLongDouble(Decimal32 decimal)
{
    return decimalToLongDouble(decimal);
}

inline
long double
DecimalConvertUtil_IbmXlc::decimal64ToLongDouble(Decimal64 decimal)
{
    return decimalToLongDouble(decimal);
}

inline
long double
DecimalConvertUtil_IbmXlc::decimal128ToLongDouble(Decimal128 decimal)
{
    return decimalToLongDouble(decimal);
}

inline
long double
DecimalConvertUtil_IbmXlc::decimalToLongDouble(Decimal32 decimal)
{
    return decimal;
}

inline
long double
DecimalConvertUtil_IbmXlc::decimalToLongDouble(Decimal64 decimal)
{
    return decimal;
}

inline
long double
DecimalConvertUtil_IbmXlc::decimalToLongDouble(Decimal128 decimal)
{
    return decimal;
}
                        // decimalToDouble functions

inline
double
DecimalConvertUtil_IbmXlc::decimal32ToDouble(Decimal32 decimal)
{
    return decimalToDouble(decimal);
}

inline
double
DecimalConvertUtil_IbmXlc::decimal64ToDouble(Decimal64 decimal)
{
    return decimalToDouble(decimal);
}

inline
double
DecimalConvertUtil_IbmXlc::decimal128ToDouble(Decimal128 decimal)
{
    return decimalToDouble(decimal);
}

inline
double
DecimalConvertUtil_IbmXlc::decimalToDouble(Decimal32 decimal)
{
    return decimal;
}

inline
double
DecimalConvertUtil_IbmXlc::decimalToDouble(Decimal64 decimal)
{
    return decimal;
}

inline
double
DecimalConvertUtil_IbmXlc::decimalToDouble(Decimal128 decimal)
{
    return decimal;
}

                        // decimalToFloat functions

inline
float
DecimalConvertUtil_IbmXlc::decimal32ToFloat(Decimal32 decimal)
{
    return decimalToFloat(decimal);
}

inline float
DecimalConvertUtil_IbmXlc::decimal64ToFloat(Decimal64 decimal)
{
    return decimalToFloat(decimal);
}

inline float
DecimalConvertUtil_IbmXlc::decimal128ToFloat(Decimal128 decimal)
{
    return decimalToFloat(decimal);
}

inline
float
DecimalConvertUtil_IbmXlc::decimalToFloat(Decimal32 decimal)
{
    return decimal;
}

inline float
DecimalConvertUtil_IbmXlc::decimalToFloat(Decimal64 decimal)
{
    return decimal;
}

inline float
DecimalConvertUtil_IbmXlc::decimalToFloat(Decimal128 decimal)
{
    return decimal;
}

                        // decimalToNetwork functions

inline
void
DecimalConvertUtil_IbmXlc::decimal32ToDenselyPacked(unsigned char *buffer,
                                                       Decimal32      decimal)
{
    BSLS_ASSERT(buffer);

    decimalToDenselyPacked(buffer, decimal);
}

inline
void
DecimalConvertUtil_IbmXlc::decimal64ToDenselyPacked(unsigned char *buffer,
                                                       Decimal64      decimal)
{
    BSLS_ASSERT(buffer);

    decimalToDenselyPacked(buffer, decimal);
}

inline
void
DecimalConvertUtil_IbmXlc::decimal128ToDenselyPacked(unsigned char *buffer,
                                                        Decimal128     decimal)
{
    BSLS_ASSERT(buffer);

    decimalToDenselyPacked(buffer, decimal);
}

inline
void
DecimalConvertUtil_IbmXlc::decimalToDenselyPacked(unsigned char *buffer,
                                                     Decimal32      decimal)
{
    BSLS_ASSERT(buffer);

    bsl::memcpy(buffer, &decimal, sizeof(decimal));
}

inline
void
DecimalConvertUtil_IbmXlc::decimalToDenselyPacked(unsigned char *buffer,
                                                     Decimal64      decimal)
{
    BSLS_ASSERT(buffer);

    bsl::memcpy(buffer, &decimal, sizeof(decimal));
}

inline
void
DecimalConvertUtil_IbmXlc::decimalToDenselyPacked(unsigned char *buffer,
                                                     Decimal128     decimal)
{
    BSLS_ASSERT(buffer);

    bsl::memcpy(buffer, &decimal, sizeof(decimal));
}

                        // decimalFromDenselyPacked functions

inline
Decimal32
DecimalConvertUtil_IbmXlc::decimal32FromDenselyPacked(
                                                   const unsigned char *buffer)
{
    BSLS_ASSERT(buffer);

    Decimal32 result;
    bsl::memcpy(&result, buffer, sizeof(result));
    return result;
}

inline
Decimal64
DecimalConvertUtil_IbmXlc::decimal64FromDenselyPacked(
                                                   const unsigned char *buffer)
{
    BSLS_ASSERT(buffer);

    Decimal64 result;
    bsl::memcpy(&result, buffer, sizeof(result));
    return result;
}

inline
Decimal128
DecimalConvertUtil_IbmXlc::decimal128FromDenselyPacked(
                                                   const unsigned char *buffer)
{
    BSLS_ASSERT(buffer);

    Decimal128 result;
    bsl::memcpy(&result, buffer, sizeof(result));
    return result;
}

inline
void
DecimalConvertUtil_IbmXlc::decimalFromDenselyPacked(
                                                  Decimal32           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal);
    BSLS_ASSERT(buffer);

    *decimal = decimal32FromDenselyPacked(buffer);
}

inline
void
DecimalConvertUtil_IbmXlc::decimalFromDenselyPacked(
                                                  Decimal64           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal);
    BSLS_ASSERT(buffer);

    *decimal = decimal64FromDenselyPacked(buffer);
}

inline
void
DecimalConvertUtil_IbmXlc::decimalFromDenselyPacked(
                                                  Decimal128          *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal);
    BSLS_ASSERT(buffer);

    *decimal = decimal128FromDenselyPacked(buffer);
}


}  // close package namespace
}  // close enterprise namespace

#endif

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
