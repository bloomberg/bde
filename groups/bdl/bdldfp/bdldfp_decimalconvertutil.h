// bdldfp_decimalconvertutil.h                                        -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALCONVERTUTIL
#define INCLUDED_BDLDFP_DECIMALCONVERTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide decimal floating-point conversion functions.
//
//@CLASSES:
//  bdldfp::DecimalConvertUtil: Namespace for decimal FP conversion functions
//
//@SEE ALSO: bdldfp_decimal, bdldfp_decimalplatform
//
//@DESCRIPTION: This component provides namespace,
// 'bdldfp::DecimalConvertUtil', containing functions that are able to convert
// between the native decimal types of the platform and various other possible
// representations, such as binary floating-point, network format (big endian,
// DPD encoded decimals).
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
//  next = bdldfp::DecimalConvertUtil::decimalToNetwork(next, number);
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
//  next = bdldfp::DecimalConvertUtil::decimalFromNetwork(number, next);
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
//  typedef bdldfp::DecimalConvertUtil Util;
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

#ifndef INCLUDED_BDLDFP_DECIMALCONVERTUTIL_DECNUMBER
#include <bdldfp_decimalconvertutil_decnumber.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALCONVERTUTIL_IBMXLC
#include <bdldfp_decimalconvertutil_ibmxlc.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALCONVERTUTIL_INTELDFP
#include <bdldfp_decimalconvertutil_inteldfp.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL
#include <bdldfp_decimalimputil.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

namespace BloombergLP {
namespace bdldfp {
                        // ========================
                        // class DecimalConvertUtil
                        // ========================

struct DecimalConvertUtil {
    // This 'struct' provides a namespace for utility functions that convert
    // between the decimal floating-point types of 'bdldfp_decimal' and various
    // other formats.

  private:
#ifdef BDLDFP_DECIMALPLATFORM_DECNUMBER
    typedef DecimalConvertUtil_DecNumber Imp;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    typedef DecimalConvertUtil_IntelDfp  Imp;
#elif defined(BDLDFP_DECIMALPLATFORM_C99_TR)
    typedef DecimalConvertUtil_IbmXlc    Imp;
#else
    BSLMF_ASSERT(false);
#endif

  public:
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

    static void decimalFromDenselyPacked(   Decimal32           *decimal,
                                            const unsigned char *buffer);
    static void decimalFromDenselyPacked(   Decimal64           *decimal,
                                            const unsigned char *buffer);
    static void decimalFromDenselyPacked(   Decimal128          *decimal,
                                            const unsigned char *buffer);
    static void decimal32FromDenselyPacked( Decimal32           *decimal,
                                            const unsigned char *buffer);
    static void decimal64FromDenselyPacked( Decimal64           *decimal,
                                            const unsigned char *buffer);
    static void decimal128FromDenselyPacked(Decimal128          *decimal,
                                            const unsigned char *buffer);
        // Store, into the specified 'decimal', the native implmentation
        // representation of the value of the same size base-10 floating point
        // value represented in Densely Packed Decimal format, at the specified
        // 'buffer' address.  The behavior is undefined unless 'buffer' points
        // to a memory area at least 'sizeof(decimal)' in size containing a
        // value in DPD format.

                        // decimalToNetwork functions

    static unsigned char *decimal32ToNetwork(unsigned char *buffer,
                                             Decimal32      decimal);
    static unsigned char *decimal64ToNetwork(unsigned char *buffer,
                                             Decimal64      decimal);
    static unsigned char *decimal128ToNetwork(unsigned char *buffer,
                                              Decimal128     decimal);
    static unsigned char *decimalToNetwork(unsigned char *buffer,
                                           Decimal32      decimal);
    static unsigned char *decimalToNetwork(unsigned char *buffer,
                                           Decimal64      decimal);
    static unsigned char *decimalToNetwork(unsigned char *buffer,
                                           Decimal128     decimal);
        // Store the specified 'decimal', in network format, into the specified
        // 'buffer' and return the address one past the last byte written into
        // the 'buffer'. The network format is defined as big endian byte order
        // and densely packed base-10 significand encoding.  This corresponds
        // to the way IBM hardware represents these numbers in memory. The
        // behavior is undefined unless 'buffer' points to a memory area at
        // least 'sizeof(decimal)' in size.  Note that these functions always
        // return 'buffer + sizeof(decimal)' on the supported 8-bits-byte
        // architectures.

                        // decimalFromNetwork functions

    static unsigned char *decimal32FromNetwork(Decimal32           *decimal,
                                               const unsigned char *buffer);
    static unsigned char *decimal64FromNetwork(Decimal64           *decimal,
                                               const unsigned char *buffer);
    static unsigned char *decimal128FromNetwork(Decimal128          *decimal,
                                                const unsigned char *buffer);
    static unsigned char *decimalFromNetwork(Decimal32           *decimal,
                                             const unsigned char *buffer);
    static unsigned char *decimalFromNetwork(Decimal64           *decimal,
                                             const unsigned char *buffer);
    static unsigned char *decimalFromNetwork(Decimal128          *decimal,
                                             const unsigned char *buffer);
        // Store into the specified 'decimal', the value of the same size
        // base-10 floating-point value stored in network format at the
        // specified 'buffer' address and return the address one past the last
        // byte read from 'buffer'.  The network format is defined as big
        // endian byte order and densely packed base-10 significand encoding.
        // This corresponds to the way IBM hardware represents these numbers in
        // memory.  The behavior is undefined unless 'buffer' points to a
        // memory area at least 'sizeof(decimal)' in size.  Note that these
        // functions always return 'buffer + sizeof(decimal)' on the supported
        // 8-bits-byte architectures.

};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================


                        // decimalToDouble functions

inline
double DecimalConvertUtil::decimal32ToDouble(Decimal32 decimal)
{
    return Imp::decimalToDouble(decimal);
}

inline
double DecimalConvertUtil::decimal64ToDouble(Decimal64 decimal)
{
    return Imp::decimalToDouble(decimal);
}

inline
double DecimalConvertUtil::decimal128ToDouble(Decimal128 decimal)
{
    return Imp::decimalToDouble(decimal);
}

inline
double DecimalConvertUtil::decimalToDouble(Decimal32 decimal)
{
    return Imp::decimalToDouble(decimal);
}

inline
double DecimalConvertUtil::decimalToDouble(Decimal64 decimal)
{
    return Imp::decimalToDouble(decimal);
}

inline
double DecimalConvertUtil::decimalToDouble(Decimal128 decimal)
{
    return Imp::decimalToDouble(decimal);
}

                        // decimalToFloat functions

inline
float DecimalConvertUtil::decimal32ToFloat(Decimal32 decimal)
{
    return Imp::decimalToFloat(decimal);
}

inline
float DecimalConvertUtil::decimal64ToFloat(Decimal64 decimal)
{
    return Imp::decimalToFloat(decimal);
}

inline
float DecimalConvertUtil::decimal128ToFloat(Decimal128 decimal)
{
    return Imp::decimalToFloat(decimal);
}

inline
float DecimalConvertUtil::decimalToFloat(Decimal32 decimal)
{
    return Imp::decimalToFloat(decimal);
}

inline
float DecimalConvertUtil::decimalToFloat(Decimal64 decimal)
{
    return Imp::decimalToFloat(decimal);
}

inline
float DecimalConvertUtil::decimalToFloat(Decimal128 decimal)
{
    return Imp::decimalToFloat(decimal);
}

                        // decimalToDenselyPacked functions

inline
void DecimalConvertUtil::decimal32ToDenselyPacked(unsigned char *buffer,
                                                  Decimal32      decimal)
{
    Imp::decimalToDenselyPacked(buffer, decimal);
}

inline
void DecimalConvertUtil::decimal64ToDenselyPacked(unsigned char *buffer,
                                                  Decimal64      decimal)
{
    Imp::decimalToDenselyPacked(buffer, decimal);
}

inline
void DecimalConvertUtil::decimal128ToDenselyPacked(unsigned char *buffer,
                                                  Decimal128     decimal)
{
    Imp::decimalToDenselyPacked(buffer, decimal);
}

inline
void DecimalConvertUtil::decimalToDenselyPacked(unsigned char *buffer,
                                                Decimal32      decimal)
{
    Imp::decimalToDenselyPacked(buffer, decimal);
}

inline
void DecimalConvertUtil::decimalToDenselyPacked(unsigned char *buffer,
                                                Decimal64      decimal)
{
    Imp::decimalToDenselyPacked(buffer, decimal);
}

inline
void DecimalConvertUtil::decimalToDenselyPacked(unsigned char *buffer,
                                                Decimal128     decimal)
{
    Imp::decimalToDenselyPacked(buffer, decimal);
}


                        // decimalFromDenselyPacked functions

inline
Decimal32
DecimalConvertUtil::decimal32FromDenselyPacked(const unsigned char *buffer)
{
    return Imp::decimal32FromDenselyPacked(buffer);
}

inline
Decimal64
DecimalConvertUtil::decimal64FromDenselyPacked(const unsigned char *buffer)
{
    return Imp::decimal64FromDenselyPacked(buffer);
}

inline
Decimal128
DecimalConvertUtil::decimal128FromDenselyPacked(const unsigned char *buffer)
{
    return Imp::decimal128FromDenselyPacked(buffer);
}

inline
void
DecimalConvertUtil::decimalFromDenselyPacked(Decimal32           *decimal,
                                             const unsigned char *buffer)
{
    Imp::decimalFromDenselyPacked(decimal, buffer);
}

inline
void
DecimalConvertUtil::decimalFromDenselyPacked(Decimal64           *decimal,
                                             const unsigned char *buffer)
{
    Imp::decimalFromDenselyPacked(decimal, buffer);
}

inline
void
DecimalConvertUtil::decimalFromDenselyPacked(Decimal128          *decimal,
                                             const unsigned char *buffer)
{
    Imp::decimalFromDenselyPacked(decimal, buffer);
}

inline
void
DecimalConvertUtil::decimal32FromDenselyPacked(Decimal32           *decimal,
                                               const unsigned char *buffer)
{
    *decimal = Imp::decimal32FromDenselyPacked(buffer);
}

inline
void
DecimalConvertUtil::decimal64FromDenselyPacked(Decimal64           *decimal,
                                               const unsigned char *buffer)
{
    *decimal = Imp::decimal64FromDenselyPacked(buffer);
}

inline
void
DecimalConvertUtil::decimal128FromDenselyPacked(Decimal128          *decimal,
                                                const unsigned char *buffer)
{
    *decimal = Imp::decimal128FromDenselyPacked(buffer);
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
