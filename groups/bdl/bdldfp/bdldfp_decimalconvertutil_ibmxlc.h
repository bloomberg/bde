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
//@SEE_ALSO: bdldfp_decimal, bdldfp_decimalplatform
//
//@DESCRIPTION: This component provides conversion operations between the
// decimal types supplied in this package ('Decimal32', 'Decimal64',
// 'Decimal128') and various alternative representations.  Some of the
// alternative representations that this component provides conversions for are
// IEEE-754 binary floating point (i.e., 'float' and 'double') and a network
// format (big-endian, Densely Packed Decimal encoding).
//
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: TBD
/// - - - - - - -

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

                        // decimalToDPD functions

    static void decimal32ToDPD( unsigned char *buffer,
                                          Decimal32      decimal);
    static void decimal64ToDPD( unsigned char *buffer,
                                          Decimal64      decimal);
    static void decimal128ToDPD(unsigned char *buffer,
                                          Decimal128     decimal);
    static void decimalToDPD(   unsigned char *buffer,
                                          Decimal32      decimal);
    static void decimalToDPD(   unsigned char *buffer,
                                          Decimal64      decimal);
    static void decimalToDPD(   unsigned char *buffer,
                                          Decimal128     decimal);
        // Populate the specified 'buffer' with the Densely Packed Decimal
        // (DPD) representation of the specified 'decimal' value.  The DPD
        // representations of 'Decimal32', 'Decimal64', and 'Decimal128'
        // require 4, 8, and 16 bytes respectively.  The behavior is undefined
        // unless 'buffer' points to a contiguous sequence of at least
        // 'sizeof(decimal)' bytes.  Note that the DPD representation is
        // defined in section 3.5 of IEEE 754-2008.

                        // decimalFromDPD functions

    static Decimal32  decimal32FromDPD( const unsigned char *buffer);
    static Decimal64  decimal64FromDPD( const unsigned char *buffer);
    static Decimal128 decimal128FromDPD(const unsigned char *buffer);
        // Return the native implementation representation of the value of the
        // same size base-10 floating-point value stored in Densely Packed
        // Decimal format at the specified 'buffer' address.  The behavior is
        // undefined unless 'buffer' points to a memory area at least
        // 'sizeof(decimal)' in size containing a value in DPD format.

    static void decimalFromDPD(Decimal32           *decimal,
                                         const unsigned char *buffer);
    static void decimalFromDPD(Decimal64           *decimal,
                                         const unsigned char *buffer);
    static void decimalFromDPD(Decimal128          *decimal,
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
DecimalConvertUtil_IbmXlc::decimal32ToDPD(unsigned char *buffer,
                                                       Decimal32      decimal)
{
    BSLS_ASSERT(buffer);

    decimalToDPD(buffer, decimal);
}

inline
void
DecimalConvertUtil_IbmXlc::decimal64ToDPD(unsigned char *buffer,
                                                       Decimal64      decimal)
{
    BSLS_ASSERT(buffer);

    decimalToDPD(buffer, decimal);
}

inline
void
DecimalConvertUtil_IbmXlc::decimal128ToDPD(unsigned char *buffer,
                                                        Decimal128     decimal)
{
    BSLS_ASSERT(buffer);

    decimalToDPD(buffer, decimal);
}

inline
void
DecimalConvertUtil_IbmXlc::decimalToDPD(unsigned char *buffer,
                                                     Decimal32      decimal)
{
    BSLS_ASSERT(buffer);

    bsl::memcpy(buffer, &decimal, sizeof(decimal));
}

inline
void
DecimalConvertUtil_IbmXlc::decimalToDPD(unsigned char *buffer,
                                                     Decimal64      decimal)
{
    BSLS_ASSERT(buffer);

    bsl::memcpy(buffer, &decimal, sizeof(decimal));
}

inline
void
DecimalConvertUtil_IbmXlc::decimalToDPD(unsigned char *buffer,
                                                     Decimal128     decimal)
{
    BSLS_ASSERT(buffer);

    bsl::memcpy(buffer, &decimal, sizeof(decimal));
}

                        // decimalFromDPD functions

inline
Decimal32
DecimalConvertUtil_IbmXlc::decimal32FromDPD(
                                                   const unsigned char *buffer)
{
    BSLS_ASSERT(buffer);

    Decimal32 result;
    bsl::memcpy(&result, buffer, sizeof(result));
    return result;
}

inline
Decimal64
DecimalConvertUtil_IbmXlc::decimal64FromDPD(
                                                   const unsigned char *buffer)
{
    BSLS_ASSERT(buffer);

    Decimal64 result;
    bsl::memcpy(&result, buffer, sizeof(result));
    return result;
}

inline
Decimal128
DecimalConvertUtil_IbmXlc::decimal128FromDPD(
                                                   const unsigned char *buffer)
{
    BSLS_ASSERT(buffer);

    Decimal128 result;
    bsl::memcpy(&result, buffer, sizeof(result));
    return result;
}

inline
void
DecimalConvertUtil_IbmXlc::decimalFromDPD(
                                                  Decimal32           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal);
    BSLS_ASSERT(buffer);

    *decimal = decimal32FromDPD(buffer);
}

inline
void
DecimalConvertUtil_IbmXlc::decimalFromDPD(
                                                  Decimal64           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal);
    BSLS_ASSERT(buffer);

    *decimal = decimal64FromDPD(buffer);
}

inline
void
DecimalConvertUtil_IbmXlc::decimalFromDPD(
                                                  Decimal128          *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal);
    BSLS_ASSERT(buffer);

    *decimal = decimal128FromDPD(buffer);
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
