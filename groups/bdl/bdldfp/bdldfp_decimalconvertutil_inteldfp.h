// bdldfp_decimalconvertutil_inteldfp.h                               -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALCONVERTUTIL_INTELDFP
#define INCLUDED_BDLDFP_DECIMALCONVERTUTIL_INTELDFP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide decimal floating-point conversion functions for Intel DFP.
//
//@CLASSES:
//  bdldfp::DecimalConvertUtil_IntelDfp: Intel DFP conversion functions
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

#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#include <bdldfp_decimalplatform.h>
#endif

#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP

#ifndef INCLUDED_BDLDFP_DECIMAL
#include <bdldfp_decimal.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL
#include <bdldfp_decimalimputil.h>
#endif

#ifndef INCLUDED_BDLDFP_INTELIMPWRAPPER
#include <bdldfp_intelimpwrapper.h>
#endif

namespace BloombergLP {
namespace bdldfp {
                        // =================================
                        // class DecimalConvertUtil_IntelDfp
                        // =================================

struct DecimalConvertUtil_IntelDfp {
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

                        // decimalToDouble functions

inline
double
DecimalConvertUtil_IntelDfp::decimal32ToDouble(Decimal32 decimal)
{
    return decimalToDouble(decimal);
}

inline
double
DecimalConvertUtil_IntelDfp::decimal64ToDouble(Decimal64 decimal)
{
    return decimalToDouble(decimal);
}

inline
double
DecimalConvertUtil_IntelDfp::decimal128ToDouble(Decimal128 decimal)
{
    return decimalToDouble(decimal);
}

inline
double
DecimalConvertUtil_IntelDfp::decimalToDouble(Decimal32 decimal)
{
    _IDEC_flags flags;
    return __bid32_to_binary64(decimal.data()->d_raw, &flags);
}

inline
double
DecimalConvertUtil_IntelDfp::decimalToDouble(Decimal64 decimal)
{
    _IDEC_flags flags;
    return __bid64_to_binary64(decimal.data()->d_raw, &flags);
}

inline
double
DecimalConvertUtil_IntelDfp::decimalToDouble(Decimal128 decimal)
{
    _IDEC_flags flags;
    return __bid128_to_binary64(decimal.data()->d_raw, &flags);
}

                        // decimalToFloat functions

inline
float
DecimalConvertUtil_IntelDfp::decimal32ToFloat(Decimal32 decimal)
{
    return decimalToFloat(decimal);
}

inline float
DecimalConvertUtil_IntelDfp::decimal64ToFloat(Decimal64 decimal)
{
    return decimalToFloat(decimal);
}

inline float
DecimalConvertUtil_IntelDfp::decimal128ToFloat(Decimal128 decimal)
{
    return decimalToFloat(decimal);
}

inline
float
DecimalConvertUtil_IntelDfp::decimalToFloat(Decimal32 decimal)
{
    _IDEC_flags flags;
    return __bid32_to_binary32(decimal.data()->d_raw, &flags);
}

inline
float
DecimalConvertUtil_IntelDfp::decimalToFloat(Decimal64 decimal)
{
    _IDEC_flags flags;
    return __bid64_to_binary32(decimal.data()->d_raw, &flags);
}

inline
float
DecimalConvertUtil_IntelDfp::decimalToFloat(Decimal128 decimal)
{
    _IDEC_flags flags;
    return __bid128_to_binary32(decimal.data()->d_raw, &flags);
}

                        // decimalToNetwork functions

inline
void
DecimalConvertUtil_IntelDfp::decimal32ToDPD(unsigned char *buffer,
                                                      Decimal32      decimal)
{
    decimalToDPD(buffer, decimal);
}

inline
void
DecimalConvertUtil_IntelDfp::decimal64ToDPD(unsigned char *buffer,
                                                      Decimal64      decimal)
{
    decimalToDPD(buffer, decimal);
}

inline
void
DecimalConvertUtil_IntelDfp::decimal128ToDPD(unsigned char *buffer,
                                                       Decimal128     decimal)
{
    decimalToDPD(buffer, decimal);
}

inline
void
DecimalConvertUtil_IntelDfp::decimalToDPD(unsigned char *buffer,
                                                    Decimal32      decimal)
{
    decimal.data()->d_raw = __bid_to_dpd32(decimal.data()->d_raw);
    bsl::memcpy(buffer, &decimal, sizeof(decimal));
}

inline
void
DecimalConvertUtil_IntelDfp::decimalToDPD(unsigned char *buffer,
                                                    Decimal64      decimal)
{
    decimal.data()->d_raw = __bid_to_dpd64(decimal.data()->d_raw);
    bsl::memcpy(buffer, &decimal, sizeof(decimal));
}

inline
void
DecimalConvertUtil_IntelDfp::decimalToDPD(unsigned char *buffer,
                                                    Decimal128     decimal)
{
    decimal.data()->d_raw = __bid_to_dpd128(decimal.data()->d_raw);
    bsl::memcpy(buffer, &decimal, sizeof(decimal));
}

                        // decimalFromDPD functions

inline
Decimal32
DecimalConvertUtil_IntelDfp::decimal32FromDPD(
                                                   const unsigned char *buffer)
{
    BSLS_ASSERT(buffer);

    DecimalImpUtil::ValueType32 value;
    bsl::memcpy(&value, buffer, sizeof(value));
    value.d_raw = __bid_dpd_to_bid32(value.d_raw);

    return Decimal32(value);
}

inline
Decimal64
DecimalConvertUtil_IntelDfp::decimal64FromDPD(
                                                   const unsigned char *buffer)
{
    BSLS_ASSERT(buffer);

    DecimalImpUtil::ValueType64 value;
    bsl::memcpy(&value, buffer, sizeof(value));
    value.d_raw = __bid_dpd_to_bid64(value.d_raw);

    return Decimal64(value);
}

inline
Decimal128
DecimalConvertUtil_IntelDfp::decimal128FromDPD(
                                                   const unsigned char *buffer)
{
    BSLS_ASSERT(buffer);

    DecimalImpUtil::ValueType128 value;
    bsl::memcpy(&value, buffer, sizeof(value));
    value.d_raw = __bid_dpd_to_bid128(value.d_raw);

    return Decimal128(value);
}

inline
void
DecimalConvertUtil_IntelDfp::decimalFromDPD(
                                                  Decimal32           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal);
    BSLS_ASSERT(buffer);

    *decimal = decimal32FromDPD(buffer);
}

inline
void
DecimalConvertUtil_IntelDfp::decimalFromDPD(
                                                  Decimal64           *decimal,
                                                  const unsigned char *buffer)
{
    BSLS_ASSERT(decimal);
    BSLS_ASSERT(buffer);

    *decimal = decimal64FromDPD(buffer);
}

inline
void
DecimalConvertUtil_IntelDfp::decimalFromDPD(
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
