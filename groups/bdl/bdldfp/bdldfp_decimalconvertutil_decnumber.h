// bdldfp_decimalconvertutil_decnumber.h                              -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALCONVERTUTIL_DECNUMBER
#define INCLUDED_BDLDFP_DECIMALCONVERTUTIL_DECNUMBER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide decimal floating-point conversion functions for decNumber.
//
//@CLASSES:
//  bdldfp::DecimalConvertUtil: Namespace for decNumber conversion functions
//
//@SEE ALSO: bdldfp_decimal, bdldfp_decimalplatform
//
//@DESCRIPTION:
// This component provides functions that are able to convert between the
// native decimal types of the decNumber DFP implementation and various other
// possible representations, such as binary floating-point, network format (big
// endian, DPD encoded decimals).
//
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: Sending Decimals As Octets Using Network Format
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose you have two communicating entities (programs) that talk to each
// other using a binary (as opposed to text) protocol.  In such protocol it is
// important to establish a so-called network format, and convert to and from
// that format in the protocol layer.  The sender (suppose that it is an IBM
// server that has just finished an expensive calculation involving millions
// of numbers and needs to send the result to its client) will need to convert
// the data to network format before sending:
//..
//  unsigned char   msgbuffer[256];
//  BDEC::Decimal64 number(BDLDFP_DECIMAL_DD(1.234567890123456e-42));
//  unsigned char   expected[] = {
//                            0x25, 0x55, 0x34, 0xb9, 0xc1, 0xe2, 0x8e, 0x56 };
//
//  unsigned char *next = msgbuffer;
//  next = bdldfp::DecimalConvertUtil_decNumber::decimalToNetwork(next,
//                                                                     number);
//
//  assert(memcmp(msgbuffer, expected, sizeof(number)) == 0);
//..
// The receiver/client shall then restore the number from network format:
//..
//  unsigned char   msgbuffer[] ={
//                            0x25, 0x55, 0x34, 0xb9, 0xc1, 0xe2, 0x8e, 0x56 };
//  BDEC::Decimal64 number;
//  BDEC::Decimal64 expected(BDLDFP_DECIMAL_DD(1.234567890123456e-42));
//
//  unsigned char *next = msgbuffer;
//  next = bdldfp::DecimalConvertUtil_decNumber::decimalFromNetwork(number,
//                                                                  next);
//
//  assert(number == expected);
//..
//
///Example 2: Storing/Sending Decimals In Binary Floating-Point
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose you have two communicating entities (programs) that talk to each
// other using a legacy protocol that employs binary floating-point formats to
// send/receive numbers.  So your application layer will have to store the
// decimal into a binary FP variable, ensure that it can be restored (in other
// words that it has "fit" into the binary type) when sending, and restore the
// decimal number (from the binary type) when receiving:
//..
//  const BDEC::Decimal64 number(BDLDFP_DECIMAL_DD(1.23456789012345e-42));
//
//  typedef bdldfp::DecimalConvertUtil_decNumber Util;
//  double dbl = Util::decimalToDouble(number);
//
//  if (Util::decimal64FromDouble(dbl) != number) {
//      // Do what is appropriate for the application
//  }
//..
// Note that the above assert would probably be a lot more complicated if
// statement in production code.  It may actually be acceptable to put the
// decimal onto the wire with certain amount of imprecision.
//
// The receiver would then restore the number using the appropriate
// 'decimal64FromDouble' function:
//..
//  BDEC::Decimal64 restored = Util::decimal64FromDouble(dbl);
//
//  assert(number == restored);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMAL
#include <bdldfp_decimal.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL
#include <bdldfp_decimalimputil.h>
#endif

namespace BloombergLP {
namespace bdldfp {
                        // ==================================
                        // class DecimalConvertUtil_decNumber
                        // ==================================

struct DecimalConvertUtil_decNumber {
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


                        // decimalToNetwork functions

    static void decimal32ToDenselyPacked( unsigned char *buffer,
                                          Decimal32  decimal);
    static void decimal64ToDenselyPacked( unsigned char *buffer,
                                          Decimal64  decimal);
    static void decimal128ToDenselyPacked(unsigned char *buffer,
                                          Decimal128 decimal);
    static void decimalToDenselyPacked(   unsigned char *buffer,
                                          Decimal32  decimal);
    static void decimalToDenselyPacked(   unsigned char *buffer,
                                          Decimal64  decimal);
    static void decimalToDenselyPacked(   unsigned char *buffer,
                                          Decimal128 decimal);
        // Store the specified 'decimal', in DPD format, into the specified
        // 'buffer' and return the address one past the last byte written into
        // the 'buffer'.  The DPD format is the densely packed base-10
        // significand encoding.

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

#ifdef BDLDFP_DECIMALPLATFORM_DECNUMBER
typedef DecimalConvertUtil_decNumber DecimalConvertUtil_Platform;
#endif

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------------------
                        // class DecimalConvertUtil_IntelDFP
                        // ---------------------------------

                        // decimalToLongDouble functions

inline
long double
DecimalConvertUtil_decNumber::decimal32ToLongDouble(Decimal32 decimal)
{
    return decimalToLongDouble(decimal);
}

inline
long double
DecimalConvertUtil_decNumber::decimal64ToLongDouble(Decimal64 decimal)
{
    return decimalToLongDouble(decimal);
}

inline
long double
DecimalConvertUtil_decNumber::decimal128ToLongDouble(Decimal128 decimal)
{
    return decimalToLongDouble(decimal);
}

                        // decimalToDouble functions

inline
double
DecimalConvertUtil_decNumber::decimal32ToDouble(Decimal32 decimal)
{
    return decimalToDouble(decimal);
}

inline
double
DecimalConvertUtil_decNumber::decimal64ToDouble(Decimal64 decimal)
{
    return decimalToDouble(decimal);
}

inline
double
DecimalConvertUtil_decNumber::decimal128ToDouble(Decimal128 decimal)
{
    return decimalToDouble(decimal);
}

                        // decimalToFloat functions

inline
float
DecimalConvertUtil_decNumber::decimal32ToFloat(Decimal32 decimal)
{
    return decimalToFloat(decimal);
}

inline float
DecimalConvertUtil_decNumber::decimal64ToFloat(Decimal64 decimal)
{
    return decimalToFloat(decimal);
}

inline float
DecimalConvertUtil_decNumber::decimal128ToFloat(Decimal128 decimal)
{
    return decimalToFloat(decimal);
}

                        // decimalToNetwork functions

inline
void
DecimalConvertUtil_decNumber::decimal32ToDenselyPacked(unsigned char *buffer,
                                                       Decimal32      decimal)
{
    BSLS_ASSERT(buffer);

    decimalToDenselyPacked(buffer, decimal);
}

inline
void
DecimalConvertUtil_decNumber::decimal64ToDenselyPacked(unsigned char *buffer,
                                                       Decimal64      decimal)
{
    BSLS_ASSERT(buffer);

    decimalToDenselyPacked(buffer, decimal);
}

inline
void
DecimalConvertUtil_decNumber::decimal128ToDenselyPacked(unsigned char *buffer,
                                                        Decimal128     decimal)
{
    BSLS_ASSERT(buffer);

    decimalToDenselyPacked(buffer, decimal);
}

inline
void
DecimalConvertUtil_decNumber::decimalToDenselyPacked(unsigned char *buffer,
                                                     Decimal32      decimal)
{
    BSLS_ASSERT(buffer);

    bsl::memcpy(buffer, &decimal, sizeof(decimal));
}

inline
void
DecimalConvertUtil_decNumber::decimalToDenselyPacked(unsigned char *buffer,
                                                     Decimal64      decimal)
{
    BSLS_ASSERT(buffer);

    bsl::memcpy(buffer, &decimal, sizeof(decimal));
}

inline
void
DecimalConvertUtil_decNumber::decimalToDenselyPacked(unsigned char *buffer,
                                                     Decimal128     decimal)
{
    BSLS_ASSERT(buffer);

    bsl::memcpy(buffer, &decimal, sizeof(decimal));
}

                        // decimalFromDenselyPacked functions

inline
Decimal32
DecimalConvertUtil_decNumber::decimal32FromDenselyPacked(
                                                   const unsigned char *buffer)
{
    BSLS_ASSERT(buffer);

    Decimal32 result;
    bsl::memcpy(&result, buffer, sizeof(result));
    return result;
}

inline
Decimal64
DecimalConvertUtil_decNumber::decimal64FromDenselyPacked(
                                                   const unsigned char *buffer)
{
    BSLS_ASSERT(buffer);

    Decimal64 result;
    bsl::memcpy(&result, buffer, sizeof(result));
    return result;
}

inline
Decimal128
DecimalConvertUtil_decNumber::decimal128FromDenselyPacked(
                                                   const unsigned char *buffer)
{
    BSLS_ASSERT(buffer);

    Decimal128 result;
    bsl::memcpy(&result, buffer, sizeof(result));
    return result;
}

inline
void
DecimalConvertUtil_decNumber::decimalFromDenselyPacked(
                                                  Decimal32           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal);
    BSLS_ASSERT(buffer);

    *decimal = decimal32FromDenselyPacked(buffer);
}

inline
void
DecimalConvertUtil_decNumber::decimalFromDenselyPacked(
                                                  Decimal64           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal);
    BSLS_ASSERT(buffer);

    *decimal = decimal64FromDenselyPacked(buffer);
}

inline
void
DecimalConvertUtil_decNumber::decimalFromDenselyPacked(
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
