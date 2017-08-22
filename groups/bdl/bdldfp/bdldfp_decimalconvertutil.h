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
//@SEE_ALSO: bdldfp_decimal, bdldfp_decimalplatform
//
//@DESCRIPTION: This component provides namespace,
// 'bdldfp::DecimalConvertUtil', containing functions that are able to convert
// between the native decimal types of the platform and various other possible
// representations, such as binary floating-point, network encoding formats.
//
///Encoding Formats
///----------------
// This utility contains functions to encode decimal values to and from three
// different encoding formats:
//
//: o the IEEE decimal interchange format using decimal encoding for the
//:   significant (also known as the Densely Packed Decimal format, see IEEE
//:   754 - 2008, section 3.5.2, for more details)
//:
//: o the multi-width encoding format, which is a custom format that can encode
//:   subsets of decimal values using a smaller number of bytes
//:
//: o the variable-width encoding format, which is a custom format that is
//:   similar to the multi-width encoding format with the main difference being
//:   that it self describes its own width
//
// 64-bit decimal values encoded by the IEEE decimal interchange format always
// uses 8 bytes, which can be inefficient.  The two custom encoding formats
// provided by this to enable more space efficient encoding of values commonly
// encountered by financial applications.
//
// In the full IEEE encoding, 50 bits are used for the trailing bits of the
// mantissa, 13 bit is used for the combination field (exponent + special
// states to indicate NaN and Inf values + leading bits of the mantissa), and 1
// bit is used for the significant.  The basic idea for the custom encoding
// formats is that the mantissa and exponent of many values (in typical
// financial applications) can fit into fewer bits than those provided by the
// full encoding.  We can define a set of narrow formats to encode these
// smaller values without loss of precision.  For example, a ticker values less
// than 100 dollars with a 2 decimal places of precision can be encoded using a
// 2 bytes encoding, using no sign bit, 3 bits for the exponent, and 13 bits
// for the mantissa.
//
///IEEE Decimal Interchange Format
///- - - - - - - - - - - - - - - -
// The IEEE decimal interchange format is defined by the IEEE standard.  64 bit
// decimal values encoded by this format always uses 8 bytes.  The
// 'decimalFromNetwork' and 'decimalToNetwork' functions can be used encode to
// and decode from this format.
//
///Multi-Width Encoding Format
///- - - - - - - - - - - - - -
// The multi-width encoding format uses a set of narrow encoding formats having
// sizes smaller than that used by the for IEEE format.  Each of the narrower
// encoding format is used to encode a subset of values that can be represented
// by the full format.  The following configuration is used to encode 64-bit
// decimal values:
//
//..
// |------|----------|----------|-----|----------|----------------|
// | size | S (bits) | E (bits) |   B | T (bits) | max signficand |
// |------|----------|----------|-----|----------|----------------|
// |   1* |        0 |        1 |  -2 |        7 |            127 |
// |   2  |        0 |        2 |  -3 |       14 |          16383 |
// |   3  |        0 |        3 |  -6 |       21 |        2097151 |
// |   4  |        1 |        5 | -16 |       26 |       67108863 |
// |   5  |        1 |        5 | -16 |       34 |    17179869183 |
// |------|-------------------------------------------------------|
// |    8 |            FULL IEEE INTERCHANGE FORMAT**             |
// |------|-------------------------------------------------------|
//
// S = sign, E = exponent, B = bias, T = significant
//
// * 1 byte encoding will be supported by the decoder but not the encoder. This
//   is done due to the relatively large performance impact of adding the 1
//   byte encoding to the encoder (10%). Preserving the encoding size in the
//   decoder allows us to easily enable this encoding size at a later time.
//
// ** If the value to be encoded can not fit in the 5-byte encoding or is -Inf,
//    +Inf, or Nan, then the full 8-byte IEEE format will be used.
//..
//
// Since the multi-width encoding format consists of subformats having varying
// widths, the size of the subformat used must be supplied long with the
// encoding to the decode function.  This is not required for either the IEEE
// format or the variable-width encoding format.
//
// The 'decimal64ToMultiWidthEncoding' and 'decimal64FromMultiWidthEncoding'
// can be used to encode to and decode from this format.  Currently, only
// 64-bit decimal values are supported by this encoding format.
//
///Variable-Width Encoding Formats
///- - - - - - - - - - - - - - - -
// The variable-width encoding format can encode decimal values using a
// variable number of bytes, similar to the multi-width encoding format.  The
// difference is that the variable-width encoding format can self-describe its
// own size using special state (typically, predicate bits), so the decode
// function does not require the size of the encoding to work.  The following
// configuration is used to encode 64-bit decimal values:
//
//..
// |------|------------|---|---|-----|----|-----------------|
// | size |          P | S | E |   B |  T | max significant |
// |------|------------|---|---|-----|----|-----------------|
// |    2 |        0b0 | 0 | 2 |  -2 | 13 |            8191 |
// |    3 |       0b10 | 0 | 3 |  -4 | 19 |          524287 |
// |    4 |       0b11 | 1 | 5 | -16 | 24 |        16777215 |
// |------|------------|------------------------------------|
// |    9 | 0b11111111 |        FULL IEEE FORMAT*           |
// |------|------------|------------------------------------|
//
// P = predicate (bit values)
// S = sign (bits), E = exponent (bits), B = bias
// T = significant (bits)
//
// * If the value to be encoded can not fit in the 4-byte encoding or is -Inf,
//   +Inf, or Nan, then the full 8-byte IEEE format will be used prefixed by a
//   1 byte predicate having the value of 0xFF.
//..
//
// The 'decimal64ToVariableWidthEncoding' and
// 'decimal64FromVariableWidthEncoding' can be used to encode to and decode
// from this format.  Currently, only 64-bit decimal values are supported by
// this encoding format.
//
///Conversion between Binary to Decimal
///------------------------------------
// The desire to convert numbers from binary to decimal format is fraught with
// misunderstanding, and is often accompanied by ill-conceived attempts to
// "correct rounding errors" and otherwise coerce results into aesthetically
// pleasing forms.  In the Bloomberg environment, there is the additional
// complication of IBM/Perkin-Elmer/Interdata floating point.  This is a
// floating-point format that uses a base-16 rather than a base-2 underlying
// representation, and the Bloomberg environment contains numbers that began as
// decimals, were converted to IBM format, and then were converted from IBM
// format to IEEE format.
//
// Generically, when a decimal is converted to floating-point (using, for
// example, scanf from text, or 'DecimalConvertUtil::decimalToDouble'), the
// result is the representable binary number nearest in value to that decimal.
// If there are two such, the one whose least significant bit is 0 is generally
// chosen.  (This is also true for conversion of decimals to 32-bit
// Perkin-Elmer format, but when the Perkin-Elmer float is then converted to
// IEEE float, the resulting value is not the same as converting the decimal to
// IEEE float directly, because the Perkin-Elmer format can lose up to three
// bits of representation compared to the IEEE format.)  Unless the decimal
// value is exactly a multiple of a power of two (e.g., 3.4375 = 55 * 1/16),
// the converted binary value cannot equal the decimal value, it can only be
// close.  This utility provides 'decimal{,32,64,128}To{Float,Double}'
// functions to convert decimal floating-point numbers to their closest binary
// floating-point values.
//
// When converting from decimal to binary, it is hoped that two different
// decimal values (in the representable range of the target binary type)
// convert to two different binary values.  There is a maximum number of
// significant digits for which this will be true.  For example, all decimals
// with 6 significant digits convert to distinct 'float' values, but 8589973000
// and 8589974000, with 7 significant digits, convert to the same 'float'
// value.  Similarly, all decimals with 15 significant digits convert to unique
// 'double' values but 900719925474.0992 and 900719925474.0993, with 16
// significant digits, convert to the same 'double' value.  Over restricted
// ranges, the maximum may be higher - for example, every decimal value with 7
// or fewer significant digits between 1e-3 and 8.5e9 converts to a unique
// 'float' value.
//
// Because binary floating-point values are generally not equal to their
// decimal progenitors, "converting from binary to decimal" does not have a
// single meaning, and programmers who seek such an operation therefore need to
// know and specify the conversion they want.  Common examples of conversions a
// programmer might seek are listed below:
//
//: 1 Express the value as its nearest decimal value.
//:
//:   o For this conversion, use the conversion constructors:
//:   o 'Decimal{32,64,128}(value)'
//:
//: 2 Express the value rounded to a given number of significant digits.  (The
//:   significant digits of a decimal number are the digits with all leading
//:   and trailing 0s removed; e.g., 0.00103, 10.3 and 10300 each have 3
//:   significant digits.)  This conversion is the one that leads programmers
//:   to complain about "rounding error" (for example, .1f rounded to 9 digits
//:   is .100000001) but is the appropriate one to use when the programmer
//:   knows that the binary value was originally converted from a decimal value
//:   with that many significant digits.
//:
//:   o For this conversion, use:
//:   o 'Decimal{32,64,128}From{Float,Double}(value, digits)'
//:
//: 3 Express the value using the minimum number of significant digits for the
//:   type of the binary such that converting the decimal value back to binary
//:   will yield the same value.  (Note that 17 digits are needed for 'double'
//:   and 9 for 'float', so not all decimal types can hold such a result.)
//:
//:   o For this conversion, use:
//:   o 'Decimal{64,128}FromFloat(value, 9)' or
//:   o 'Decimal128FromDouble(value, 17)'
//:
//: 4 Express the value using a number of decimal places that restores the
//:   original decimal value from which the binary value was converted,
//:   assuming that the original decimal value had sufficiently few significant
//:   digits so that no two values with that number of digits would convert to
//:   the same binary value.  (That number is 15 for 'double' and 6 for 'float'
//:   in general but 7 over a limited range that spans '[1e-3 .. 8.5e9]').
//:
//:   o For this conversion, use:
//:   o 'Decimal{32,64,128}From{Float,Double}(value)'
//:
//: 5 Express the value as the shortest decimal number that converts back
//:   exactly to the binary value.  For example. given the binary value
//:   0x3DCCCCCD above, that corresponding shortest decimal value is
//:   (unsurprisingly) .1, while the next lower value 0x3DCCCCCC has the
//:   shortest decimal .099999994 and the next higher value 0x3DCCCCCE has the
//:   shortest decimal .010000001.  This is the most visually appealing result,
//:   but can be expensive and slow to compute.
//:
//:   o For this conversion, use:
//:   o 'Decimal{32,64,128}From{Float,Double}(value, -1)'
//:
//: 6 Express the value using a number of decimal places that restores the
//:   original decimal value assuming that it is a 'float' which originated as
//:   an IBM/Perkin-Elmer/Interdata 'float' value itself originally converted
//:   from a decimal value.
//:
//:   o For this conversion, use:
//:   o 'Decimal{32,64,128}FromFloat(value, 6)'
//:
//: 7 Express the value exactly as a decimal.  For example, the decimal
//:   value .1 converts to the 32-bit IEEE float value 0x3DCCCCCD, which has
//:   the exact value .100000001490116119384765625.  This conversion is seldom
//:   useful, except perhaps for debugging, since the exact value may have over
//:   1000 digits, and as well cannot be represented as a decimal
//:   floating-point type since those types do not have enough digits.
//:
//:   o For this conversion, use 'sprintf' into a large-enough buffer:
//:   o 'char buf[2000]; double value; sprintf(buf, "%.1100f", value);'
//:   o The result will have trailing 0s, which may be trimmed.
//:
//: 8 Express the value rounded to a given number of decimal places.  (The
//:   decimal places of a decimal number are the number of digits after the
//:   decimal point, with trailing 0s removed; .01, 10.01, and 1000.01 each
//:   have two decimal places.)  This conversion can be problematic when the
//:   integer portion of the value is large, as there may not be enough
//:   precision remaining to deliver a meaningful number of decimal places.  As
//:   seen above, for example, for numbers near one trillion, there is not
//:   enough precision in a 'double' for 4 decimal places.
//:
//:   o For this conversion, use 'sprintf' into a large-enough buffer:
//:   o 'char buf[2000]; double value; sprintf(buf, "%.*f", places, value);'
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
//  unsigned char  msgbuffer[256];
//  unsigned char *next = msgbuffer;
//
//  BDEC::Decimal64 number(BDLDFP_DECIMAL_DD(1.234567890123456e-42));
//  unsigned char   expected[] = {
//                            0x25, 0x55, 0x34, 0xb9, 0xc1, 0xe2, 0x8e, 0x56 };
//
//  next = bdldfp::DecimalConvertUtil::decimalToNetwork(next, number);
//
//  assert(memcmp(msgbuffer, expected, sizeof(number)) == 0);
//..
// The receiver/client shall then restore the number from network format:
//..
//  unsigned char  msgbuffer[] ={
//                            0x25, 0x55, 0x34, 0xb9, 0xc1, 0xe2, 0x8e, 0x56 };
//  unsigned char *next = msgbuffer;
//
//  BDEC::Decimal64 number;
//  BDEC::Decimal64 expected(BDLDFP_DECIMAL_DD(1.234567890123456e-42));
//
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

#ifndef INCLUDED_BDLDFP_DECIMALUTIL
#include <bdldfp_decimalutil.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
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
    // PRIVATE TYPES
#ifdef BDLDFP_DECIMALPLATFORM_DECNUMBER
    typedef DecimalConvertUtil_DecNumber Imp;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    typedef DecimalConvertUtil_IntelDfp  Imp;
#elif defined(BDLDFP_DECIMALPLATFORM_C99_TR)
    typedef DecimalConvertUtil_IbmXlc    Imp;
#else
    BSLMF_ASSERT(false);
#endif

    // PRIVATE CLASS DATA

                    // 'bid64' REPRESENTATION CONSTANTS

    static const int k_EXPONENT_SHIFT_SMALL64 = 53; // exponent position
    static const int k_EXPONENT_MASK64 = 0x3ff;     // exponent mask bits
    static const unsigned long long k_SPECIAL_ENCODING_MASK64 =
        0x6000000000000000ull;                      // special when non-zero
    static const unsigned long long k_SMALL_COEFF_MASK64 =
        0x001fffffffffffffull;                      // mask for mantissa

    // PRIVATE CLASS METHODS
    static int decimal64ToUnpackedSpecial(bool                *isNegative,
                                          int                 *biasedExponent,
                                          bsls::Types::Uint64 *mantissa,
                                          bdldfp::Decimal64    value);
        // If the specified 'value' is NaN, +infinity, -infinity, or its
        // unbiased exponent is 384, return a non-zero value and leave all
        // output parameters unmodified.  Otherwise, partition the 'value' into
        // sign, biased exponent, and mantissa compartments, and load the
        // corresponding values into the specified 'isNegative',
        // 'biasedExponent', and 'mantissa'.  Return 0.  Note that a non-zero
        // value does not indicate that 'value' can not be partitioned, just
        // that it can not be partitioned by this function.  Also note that the
        // bias for 'Decimal64' is 398.

    static bdldfp::Decimal64 decimal64FromUnpackedSpecial(
                                                bool                isNegative,
                                                bsls::Types::Uint64 mantissa,
                                                int                 exponent);
        // Return a 'Decimal64' object that has the specified 'mantissa',
        // 'exponent', and a sign based on the specified 'isNegative'.  The
        // behavior is undefined unless 'isNegative', 'mantissa', and the
        // biased exponent were originally obtained from
        // 'decimal64ToUnpackedSpecial'.  Note that 'exponent' should be
        // unbiased, so 398 should be subtracted from the biased exponent
        // gotten from 'decimal64ToUnpackedSpecial'.


    static bdldfp::Decimal64 decimal64FromUnpackedSpecial(int mantissa,
                                                          int exponent);
        // Return a 'Decimal64' object that has the specified 'mantissa',
        // 'exponent'.  The behavior is undefined unless 'isNegative',
        // 'mantissa', and the biased exponent were originally obtained from
        // 'decimal64ToUnpackedSpecial'.  Note that 'exponent' should be
        // unbiased, so 398 should be subtracted from the biased exponent
        // gotten from 'decimal64ToUnpackedSpecial'.

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
        //:   floating-point exception, round that value according to the
        //:   *binary* rounding direction setting of the environment, and
        //:   return the result of that.
        //:
        //: o Otherwise use the exact value of the 'other' object for the
        //:   initialization if this object.

                        // decimalFromDouble functions

    static Decimal32  decimal32FromDouble (double binary, int digits = 0);
    static Decimal32  decimal32FromFloat  (float  binary, int digits = 0);
    static Decimal64  decimal64FromDouble (double binary, int digits = 0);
    static Decimal64  decimal64FromFloat  (float  binary, int digits = 0);
    static Decimal128 decimal128FromDouble(double binary, int digits = 0);
    static Decimal128 decimal128FromFloat (float  binary, int digits = 0);
        // Return a decimal floating-point number converted from the specified
        // 'binary'.
        //
        // If 'binary' is singular (+/-NaN, +/-Inf, or +/-0) or is not within
        // the representable range of the return type, return a corresponding
        // decimal singular value.
        //
        // Optionally specify 'digits' to indicate the number of significant
        // digits to produce in the returned value.  The 'digits' parameter is
        // treated as follows:
        //
        // If 'digits' is larger than the number of digits in the destination
        // type, it will be reduced to that number of digits.
        //
        // If 'digits' is positive, the result is 'binary' rounded to that many
        // significant digits.
        //
        // If 'digits' is negative, the decimal value with the fewest
        // significant digits that converts back to 'binary' is returned if
        // possible, and otherwise the value closest to 'binary' is returned.
        // Note that this provides the most visually appealing result but is
        // the most expensive to compute.
        //
        // If 'digits' is not specified or 0, a default value will be used
        // (possibly depending on the value of 'binary') based on the premise
        // that 'binary' is a converted decimal value of no more significant
        // digits than is guaranteed to have a uniquely converted binary value
        // (15 for 'double', 6 for 'float' in general, and 7 for 'float' in the
        // range '[ .0009999995 .. 8589972000 ]').  Note that this is likely to
        // have the best performance for "business" numbers (i.e., numbers that
        // originate as decimal values in external market quote feeds).
        //
        // Note that the purpose of these functions is to restore a decimal
        // value that has been converted to a binary floating-point type.  It
        // is more efficient to use conversion constructors when all that is
        // needed is the nearest decimal to the 'binary' value.
        //
        // Note that if 'binary' is a 'float' value that was converted from an
        // IBM/Perkin-Elmer/Interdata binary 'float' value itself converted
        // from a decimal value of no more than 6 significant digits,
        // specifying 6 for 'digits' will recover the original decimal value.
        // Not specifying 'digits' may result in a value having a spurious
        // seventh digit.

                        // decimalToBID functions

    static void decimal32ToBID (unsigned char *buffer,
                                Decimal32      decimal);
    static void decimal64ToBID (unsigned char *buffer,
                                Decimal64      decimal);
    static void decimal128ToBID(unsigned char *buffer,
                                Decimal128     decimal);
    static void decimalToBID   (unsigned char *buffer,
                                Decimal32      decimal);
    static void decimalToBID   (unsigned char *buffer,
                                Decimal64      decimal);
    static void decimalToBID   (unsigned char *buffer,
                                Decimal128     decimal);
        // Populate the specified 'buffer' with the Binary Integral Decimal
        // (BID) representation of the specified 'decimal' value.  The BID
        // representations of 'Decimal32', 'Decimal64', and 'Decimal128'
        // require 4, 8, and 16 bytes respectively.  The behavior is undefined
        // unless 'buffer' points to a contiguous sequence of at least
        // 'sizeof(decimal)' bytes.  Note that the BID representation is
        // defined in section 3.5 of IEEE 754-2008.

                        // decimalFromBID functions

    static Decimal32  decimal32FromBID (const unsigned char *buffer);
    static Decimal64  decimal64FromBID (const unsigned char *buffer);
    static Decimal128 decimal128FromBID(const unsigned char *buffer);
        // Return the native implementation representation of the value of the
        // same size base-10 floating-point value stored in Binary Integral
        // Decimal format at the specified 'buffer' address.  The behavior is
        // undefined unless 'buffer' points to a memory area at least
        // 'sizeof(decimal)' in size containing a value in BID format.

    static void decimal32FromBID (Decimal32           *decimal,
                                  const unsigned char *buffer);
    static void decimal64FromBID (Decimal64           *decimal,
                                  const unsigned char *buffer);
    static void decimal128FromBID(Decimal128          *decimal,
                                  const unsigned char *buffer);
    static void decimalFromBID   (Decimal32           *decimal,
                                  const unsigned char *buffer);
    static void decimalFromBID   (Decimal64           *decimal,
                                  const unsigned char *buffer);
    static void decimalFromBID   (Decimal128          *decimal,
                                  const unsigned char *buffer);
        // Store, into the specified 'decimal', the native implementation
        // representation of the value of the same size base-10 floating point
        // value represented in Binary Integral Decimal format, at the
        // specified 'buffer' address.  The behavior is undefined unless
        // 'buffer' points to a memory area at least 'sizeof(decimal)' in size
        // containing a value in BID format.


                        // decimalToDPD functions

    static void decimal32ToDPD (unsigned char *buffer,
                                Decimal32      decimal);
    static void decimal64ToDPD (unsigned char *buffer,
                                Decimal64      decimal);
    static void decimal128ToDPD(unsigned char *buffer,
                                Decimal128     decimal);
    static void decimalToDPD   (unsigned char *buffer,
                                Decimal32      decimal);
    static void decimalToDPD   (unsigned char *buffer,
                                Decimal64      decimal);
    static void decimalToDPD   (unsigned char *buffer,
                                Decimal128     decimal);
        // Populate the specified 'buffer' with the Densely Packed Decimal
        // (DPD) representation of the specified 'decimal' value.  The DPD
        // representations of 'Decimal32', 'Decimal64', and 'Decimal128'
        // require 4, 8, and 16 bytes respectively.  The behavior is undefined
        // unless 'buffer' points to a contiguous sequence of at least
        // 'sizeof(decimal)' bytes.  Note that the DPD representation is
        // defined in section 3.5 of IEEE 754-2008.

                        // decimalFromDPD functions

    static Decimal32  decimal32FromDPD (const unsigned char *buffer);
    static Decimal64  decimal64FromDPD (const unsigned char *buffer);
    static Decimal128 decimal128FromDPD(const unsigned char *buffer);
        // Return the native implementation representation of the value of the
        // same size base-10 floating-point value stored in Densely Packed
        // Decimal format at the specified 'buffer' address.  The behavior is
        // undefined unless 'buffer' points to a memory area at least
        // 'sizeof(decimal)' in size containing a value in DPD format.

    static void decimal32FromDPD (Decimal32           *decimal,
                                  const unsigned char *buffer);
    static void decimal64FromDPD (Decimal64           *decimal,
                                  const unsigned char *buffer);
    static void decimal128FromDPD(Decimal128          *decimal,
                                  const unsigned char *buffer);
    static void decimalFromDPD   (Decimal32           *decimal,
                                  const unsigned char *buffer);
    static void decimalFromDPD   (Decimal64           *decimal,
                                  const unsigned char *buffer);
    static void decimalFromDPD   (Decimal128          *decimal,
                                  const unsigned char *buffer);
        // Store, into the specified 'decimal', the native implementation
        // representation of the value of the same size base-10 floating point
        // value represented in Densely Packed Decimal format, at the specified
        // 'buffer' address.  The behavior is undefined unless 'buffer' points
        // to a memory area at least 'sizeof(decimal)' in size containing a
        // value in DPD format.

                        // decimalToNetwork functions

    static unsigned char *decimal32ToNetwork (unsigned char *buffer,
                                              Decimal32      decimal);
    static unsigned char *decimal64ToNetwork (unsigned char *buffer,
                                              Decimal64      decimal);
    static unsigned char *decimal128ToNetwork(unsigned char *buffer,
                                              Decimal128     decimal);
    static unsigned char *decimalToNetwork   (unsigned char *buffer,
                                              Decimal32      decimal);
    static unsigned char *decimalToNetwork   (unsigned char *buffer,
                                              Decimal64      decimal);
    static unsigned char *decimalToNetwork   (unsigned char *buffer,
                                              Decimal128     decimal);
        // Store the specified 'decimal', in network format, into the specified
        // 'buffer' and return the address one past the last byte written into
        // the 'buffer'.  The network format is defined as big endian byte
        // order and densely packed base-10 significand encoding.  This
        // corresponds to the way IBM hardware represents these numbers in
        // memory.  The behavior is undefined unless 'buffer' points to a
        // memory area at least 'sizeof(decimal)' in size.  Note that these
        // functions always return 'buffer + sizeof(decimal)' on the supported
        // 8-bits-byte architectures.

                        // decimalFromNetwork functions

    static const unsigned char *decimal32FromNetwork(
                                                  Decimal32           *decimal,
                                                  const unsigned char *buffer);
    static const unsigned char *decimal64FromNetwork(
                                                  Decimal64           *decimal,
                                                  const unsigned char *buffer);
    static const unsigned char *decimal128FromNetwork(
                                                  Decimal128          *decimal,
                                                  const unsigned char *buffer);
    static const unsigned char *decimalFromNetwork(
                                                  Decimal32           *decimal,
                                                  const unsigned char *buffer);
    static const unsigned char *decimalFromNetwork(
                                                  Decimal64           *decimal,
                                                  const unsigned char *buffer);
    static const unsigned char *decimalFromNetwork(
                                                  Decimal128          *decimal,
                                                  const unsigned char *buffer);
        // Store into the specified 'decimal', the value of the same size
        // base-10 floating-point value stored in network format at the
        // specified 'buffer' address and return the address one past the last
        // byte read from 'buffer'.  The network format is defined as big
        // endian byte order and densely packed base-10 significand encoding.
        // This corresponds to the way IBM hardware represents these numbers in
        // memory.  The behavior is undefined unless 'buffer' points to a
        // memory area at least 'sizeof(decimal)' bytes.  Note that these
        // functions always return 'buffer + sizeof(decimal)' on the supported
        // 8-bits-byte architectures.

    static bsls::Types::size_type decimal64ToMultiWidthEncoding(
                                                   unsigned char     *buffer,
                                                   bdldfp::Decimal64  decimal);
        // Store the specified 'decimal', in the *multi-width encoding* format,
        // into the specified 'buffer' and return the number of bytes used by
        // the encoding.  The behavior is undefined unless 'buffer' points to a
        // memory area with enough room to hold the encode value (which has a
        // maximum size of 8 bytes).

    static bsls::Types::size_type decimal64ToMultiWidthEncodingRaw(
                                                   unsigned char     *buffer,
                                                   bdldfp::Decimal64  decimal);
        // If the specified 'decimal' can be encoded in 5 or fewer bytes of the
        // *multi-width encoding* format, then store 'decimal' into the
        // specified 'buffer' in that format, and return the number of bytes
        // written to 'buffer'.  Otherwise, return 0.  The behavior is
        // undefined unless 'buffer' points to a memory area having at least 5
        // bytes.  Note that this function does not supporting encoding values
        // requiring a full IEEE network encoding, which is supported by the
        // 'decimal64ToMultiWidthEncoding' function.

    static Decimal64 decimal64FromMultiWidthEncodingRaw(
                                                const unsigned char    *buffer,
                                                bsls::Types::size_type  size);
        // Decode a decimal value in the *multi-width encoding* format from the
        // specified 'buffer' having the specified 'size'.  Return the decoded
        // value.  The behavior is undefined unless 'buffer' has at least
        // 'size' bytes, 'size' is a valid encoding size in the
        // 'multi-width encoding' format, and 'size <= 5'.  Note that this
        // function does not support decoding values requiring a full IEEE
        // network encoding, which is supported by the
        // 'decimal64FromMultiWidthEncoding' function.

    static bool isValidMultiWidthSize(bsls::Types::size_type  size);
        // Return 'true' if the specified 'size' is a valid encoding size in
        // the *multi-width encoding* format, and 'false' otherwise.  Note that
        // valid encoding sizes are 1, 2, 3, 4, 5 and 8 bytes.

    static Decimal64 decimal64FromMultiWidthEncoding(
                                                const unsigned char    *buffer,
                                                bsls::Types::size_type  size);
        // Decode a decimal value in the *multi-width encoding* format from the
        // specified 'buffer' having the specified 'size'.  Return the decoded
        // value.  The behavior is undefined unless 'buffer' has at least
        // 'size' bytes, and 'size' is a valid encoding size in the
        // 'multi-width encoding' format.

    static int decimal64FromMultiWidthEncodingIfValid(
                                               Decimal64              *decimal,
                                               const unsigned char    *buffer,
                                               bsls::Types::size_type  size);
        // Decode a decimal value in the *multi-width encoding* format from the
        // specified 'buffer' having the specified 'size' and store the result
        // into the specified 'decimal' parameter.  Return 0 on success, and
        // non-zero value otherwise.  The behavior is undefined unless 'buffer'
        // has at least 'size' bytes.  Note that this function returns a
        // non-zero value and leaves 'decimal' unchanged if 'size' is not a
        // valid encoding size of the *multi-width encoding* format.

    static unsigned char *decimal64ToVariableWidthEncoding(
                                                   unsigned char     *buffer,
                                                   bdldfp::Decimal64  decimal);
        // Store the specified 'decimal', in the *variable-width encoding*
        // format, into the specified 'buffer' and return the address one past
        // the last byte written into the 'buffer'.  The behavior is undefined
        // unless 'buffer' points to a memory area with enough room to hold the
        // encoded value (which has a maximum size of 9 bytes).

    static const unsigned char *decimal64FromVariableWidthEncoding(
                                                  bdldfp::Decimal64   *decimal,
                                                  const unsigned char *buffer);
        // Store into the specified 'decimal', the value of 'Decimal64' value
        // stored in the *variable-width encoding* format at the specified
        // 'buffer' address.  Return the address one past the last byte read
        // from 'buffer'.  The behavior is undefined unless 'buffer' points to
        // a memory area holding a 'Decimal64' value encoded in the
        // *variable-width encoding* format.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================


// PRIVATE CLASS METHODS

inline
int DecimalConvertUtil::decimal64ToUnpackedSpecial(
                                           bool                *isNegative,
                                           int                 *biasedExponent,
                                           bsls::Types::Uint64 *mantissa,
                                           bdldfp::Decimal64    value)
{
#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    bsls::Types::Uint64 bidValue = value.data()->d_raw;
#else
    bsls::Types::Uint64 bidValue = bid_dpd_to_bid64(
                            static_cast<bsls::Types::Uint64>(*(value.data())));
#endif
    // This class method is based on inteldfp 'unpack_BID64' (bid_internal.h),
    // with a non-zero return if 'SPECIAL_ENCODING_MASK64' indicates a special
    // encoding; these are practically non-existent and no need to optimize.

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
        (bidValue & k_SPECIAL_ENCODING_MASK64) == k_SPECIAL_ENCODING_MASK64)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        // punt on special encodings
        return -1;                                                    // RETURN
    }

    *isNegative = (bidValue & 0x8000000000000000ull) ? 1 : 0;

    *biasedExponent = static_cast<int>(
                   (bidValue >> k_EXPONENT_SHIFT_SMALL64) & k_EXPONENT_MASK64);

    *mantissa = bidValue & k_SMALL_COEFF_MASK64;

    return 0;
}

inline
Decimal64 DecimalConvertUtil::decimal64FromUnpackedSpecial(
                                                bool                isNegative,
                                                bsls::Types::Uint64 mantissa,
                                                int                 exponent)
{
#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    bdldfp::Decimal64 result;
    result.data()->d_raw = (isNegative ? 0x8000000000000000ull : 0) |
                           (static_cast<BID_UINT64>(exponent + 398)
                            << k_EXPONENT_SHIFT_SMALL64) |
                           mantissa;
    return result;
#else
    if (isNegative) {
        return DecimalImpUtil::makeDecimalRaw64(
                                             -static_cast<long long>(mantissa),
                                             exponent);
    } else {
        return DecimalImpUtil::makeDecimalRaw64(mantissa, exponent);
    }
#endif
}

inline
Decimal64 DecimalConvertUtil::decimal64FromUnpackedSpecial(int mantissa,
                                                           int exponent)
{
#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    bdldfp::Decimal64 result;
    result.data()->d_raw = (static_cast<BID_UINT64>(exponent + 398)
                            << k_EXPONENT_SHIFT_SMALL64) |
                            mantissa;
    return result;
#else
    return DecimalUtil::makeDecimalRaw64(mantissa, exponent);
#endif
}

// CLASS METHODS
inline
bsls::Types::size_type DecimalConvertUtil::decimal64ToMultiWidthEncoding(
                                                    unsigned char     *buffer,
                                                    bdldfp::Decimal64  decimal)
{

    bsls::Types::size_type size = decimal64ToMultiWidthEncodingRaw(buffer,
                                                                   decimal);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(size != 0)) {
        return size;                                                  // RETURN
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        bsls::Types::Uint64 encoded;
        decimal64ToBID(reinterpret_cast<unsigned char *>(&encoded), decimal);

        encoded = BSLS_BYTEORDER_HTONLL(encoded);

        bsl::memcpy(buffer,
                    reinterpret_cast<unsigned char*>(&encoded),
                    8);
        return 8;                                                     // RETURN
    }
}

inline
bsls::Types::size_type DecimalConvertUtil::decimal64ToMultiWidthEncodingRaw(
                                                    unsigned char     *buffer,
                                                    bdldfp::Decimal64  decimal)
{
    bool                isNegative;
    int                 exponent;
    bsls::Types::Uint64 mantissa;

    // 'exponent' is biased --> biased exponent = exponent + 398

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 ==
             decimal64ToUnpackedSpecial(&isNegative,
                                        &exponent,
                                        &mantissa,
                                        decimal))) {
        if (!isNegative) {
            if (395 <= exponent && exponent < 399) {
                if (mantissa < (1u << 14)) {
                    unsigned short squished = static_cast<unsigned short>(
                        mantissa | (exponent - 395) << 14);

                    unsigned short squishedN = BSLS_BYTEORDER_HTONS(squished);
                    bsl::memcpy(buffer, &squishedN, 2);
                    return 2;                                         // RETURN
                }
            }
            if (392 <= exponent && exponent < 400) {
                if (mantissa < (1u << 21)) {
                    // On IBM (and Linux to a lesser extent), copying from a
                    // word-aligned source is faster, so we shift an extra the
                    // source by an extra 8 bits.

                    unsigned int squished = static_cast<unsigned int>(
                        (mantissa << 8) | (exponent - 392) << 29);
                    unsigned int squishedN = BSLS_BYTEORDER_HTONL(squished);

                    bsl::memcpy(buffer,
                                reinterpret_cast<unsigned char*>(&squishedN),
                                3);
                    return 3;                                         // RETURN
                }
            }
        }

        if (382 <= exponent && exponent < 414) {
            if (mantissa < (1u << 26)) {
                unsigned int squished = static_cast<unsigned int>(
                                            mantissa | (exponent - 382) << 26);
                if (isNegative) {
                    squished |= 1u << 31;
                }
                unsigned int squishedN = BSLS_BYTEORDER_HTONL(squished);
                bsl::memcpy(buffer, &squishedN, 4);
                return 4;                                             // RETURN
            }
            if (mantissa < (1ull << 34)) {
                bsls::Types::Uint64 squished =
                    static_cast<bsls::Types::Uint64>(
                     (mantissa << 24) |
                     (static_cast<bsls::Types::Uint64>(exponent - 382) << 58));
                if (isNegative) {
                    squished |= 1ull << 63;
                }
                bsls::Types::Uint64 squishedN =
                                               BSLS_BYTEORDER_HTONLL(squished);
                bsl::memcpy(buffer,
                            reinterpret_cast<unsigned char*>(&squishedN),
                            5);
                return 5;                                             // RETURN
            }
        }
    }

    return 0;
}

inline
bool DecimalConvertUtil::isValidMultiWidthSize(bsls::Types::size_type  size)
{
    return (size > 0 && size <= 5) || size == 8;
}

inline
Decimal64 DecimalConvertUtil::decimal64FromMultiWidthEncoding(
                                                const unsigned char    *buffer,
                                                bsls::Types::size_type  size)
{
    BSLS_ASSERT(1 <= size);
    BSLS_ASSERT(size <= 5 || size == 8);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(size < 6)) {
        return decimal64FromMultiWidthEncodingRaw(buffer, size);      // RETURN
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        bsls::Types::Uint64 encoded;
        bsl::memcpy(&encoded, buffer, 8);
        encoded = BSLS_BYTEORDER_NTOHLL(encoded);

        return decimal64FromBID(reinterpret_cast<unsigned char *>(&encoded));
                                                                      // RETURN
    }
}

inline
int DecimalConvertUtil::decimal64FromMultiWidthEncodingIfValid(
                                               Decimal64              *decimal,
                                               const unsigned char    *buffer,
                                               bsls::Types::size_type  size)
{
    int ret(0);

    if (isValidMultiWidthSize(size)) {
        *decimal = decimal64FromMultiWidthEncoding(buffer, size);
    }
    else {
        ret = 1;
    }
    return ret;
}

inline
Decimal64 DecimalConvertUtil::decimal64FromMultiWidthEncodingRaw(
                                                const unsigned char    *buffer,
                                                bsls::Types::size_type  size)
{
    BSLS_ASSERT(1 <= size);
    BSLS_ASSERT(size <= 5);

    switch(size) {
      case 2: {
        int exponent = (buffer[0] >> 6) - 3;
        int mantissa = static_cast<int>(((buffer[0] & 0x3F) << 8) |
                                        static_cast<int>(buffer[1]));

        return decimal64FromUnpackedSpecial(mantissa, exponent);      // RETURN
      } break;
      case 3: {
        int exponent = (buffer[0] >> 5) - 6;
        int mantissa = static_cast<int>(((buffer[0] & 0x1F) << 16) |
                                        static_cast<int>(buffer[1]) << 8 |
                                        static_cast<int>(buffer[2]));
        return decimal64FromUnpackedSpecial(mantissa, exponent);      // RETURN
      } break;
      case 4: {
        bool isNegative = buffer[0] >> 7;
        int  exponent   = ((buffer[0] & 0x7F) >> 2) - 16;
        int  mantissa   = static_cast<int>(((buffer[0] & 0x03)         << 24) |
                                           static_cast<int>(buffer[1]) << 16 |
                                           static_cast<int>(buffer[2]) << 8 |
                                           static_cast<int>(buffer[3]));
        return decimal64FromUnpackedSpecial(isNegative, mantissa, exponent);
                                                                      // RETURN
      } break;
      case 1: {
        int exponent = (buffer[0] >> 7) - 2;
        int mantissa = static_cast<int>(buffer[0] & 0x7F);
        return decimal64FromUnpackedSpecial(mantissa, exponent);      // RETURN
      } break;
#ifdef BSLS_PLATFORM_CMP_IBM
      case 5:
#else
      default:
#endif
      {
        // Xlc optimizes better when 'case 5:' is used instead of 'default:',
        // and vice versa for gcc.

        bool                isNegative = buffer[0] >> 7;
        int                 exponent   = ((buffer[0] & 0x7F) >> 2) - 16;
        bsls::Types::Uint64 mantissa   = static_cast<bsls::Types::Uint64>(
                     static_cast<bsls::Types::Uint64>(buffer[0] & 0x03) << 32 |
                     static_cast<bsls::Types::Uint64>(buffer[1]) << 24 |
                     static_cast<bsls::Types::Uint64>(buffer[2]) << 16 |
                     static_cast<bsls::Types::Uint64>(buffer[3]) << 8 |
                     static_cast<bsls::Types::Uint64>(buffer[4]));
        return decimal64FromUnpackedSpecial(isNegative, mantissa, exponent);
                                                                      // RETURN
      } break;
    }

#ifdef BSLS_PLATFORM_CMP_IBM
    // From here on, the function has undefined behavior.  We will return a
    // default constructed value to suppress compiler warnings.
    return bdldfp::Decimal64();
#endif
}

inline
unsigned char *DecimalConvertUtil::decimal64ToVariableWidthEncoding(
                                                    unsigned char     *buffer,
                                                    bdldfp::Decimal64  decimal)
{
    bool                isNegative;
    int                 exponent;
    bsls::Types::Uint64 mantissa;

    // 'exponent' is biased --> biased exponent = exponent + 398

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 ==
             decimal64ToUnpackedSpecial(&isNegative,
                                        &exponent,
                                        &mantissa,
                                        decimal))) {
        if (!isNegative) {
            if (396 <= exponent && exponent < 400) {
                if (mantissa < (1u << 13)) {

                    // The predicate disambiguation bit is implicitly 0.

                    unsigned short squished = static_cast<unsigned short>(
                        mantissa | (exponent - 396) << 13);

                    unsigned short squishedN = BSLS_BYTEORDER_HTONS(squished);
                    bsl::memcpy(buffer, &squishedN, 2);
                    return buffer + 2;                                // RETURN
                }
            }

            if (394 <= exponent && exponent < 402) {
                if (mantissa < (1u << 19)) {
                    // On IBM (and Linux to a lesser extent), copying from a
                    // word-aligned source is faster, so we shift the source of
                    // memcpy by an extra 8 bits.

                    unsigned int squished = static_cast<unsigned int>(
                                     (mantissa << 8) | (exponent - 394) << 27);

                    // The predicate bits should be 0b10.

                    squished |= 1u << 31;

                    unsigned int squishedN = BSLS_BYTEORDER_HTONL(squished);

                    bsl::memcpy(buffer,
                                reinterpret_cast<unsigned char*>(&squishedN),
                                3);
                    return buffer + 3;                                // RETURN
                }
            }
        }

        // If the value is negative, with exponent of 15 (biased exponent 413),
        // then the first byte will have a value of FF, which is the state used
        // to indicate that a full 9 byte representation should be used.

        if (382 <= exponent &&
                        (exponent < 413 || (!isNegative && exponent == 413))) {
            if (mantissa < (1u << 24)) {
                unsigned int squished = static_cast<unsigned int>(
                                            mantissa | (exponent - 382) << 24);
                if (isNegative) {
                    squished |= 1u << 29;
                }
                // The predicate bits should be 11.

                squished |= 3u << 30;
                unsigned int squishedN = BSLS_BYTEORDER_HTONL(squished);
                bsl::memcpy(buffer, &squishedN, 4);
                return buffer + 4;                                    // RETURN
            }
        }
    }

    *buffer++ = 0xFF;

    bsls::Types::Uint64 encoded;
    decimal64ToBID(reinterpret_cast<unsigned char *>(&encoded), decimal);

    encoded = BSLS_BYTEORDER_HTONLL(encoded);

    bsl::memcpy(buffer, reinterpret_cast<unsigned char*>(&encoded), 8);

    return buffer + 8;
}

inline
const unsigned char *DecimalConvertUtil::decimal64FromVariableWidthEncoding(
                                                  bdldfp::Decimal64   *decimal,
                                                  const unsigned char *buffer)
{
    if (!(*buffer & 0x80)) {

        // 2-byte encoding is used.

        int exponent = (buffer[0] >> 5) - 2;
        int mantissa = static_cast<int>(((buffer[0] & 0x1F) << 8) |
                                        static_cast<int>(buffer[1]));

        *decimal = decimal64FromUnpackedSpecial(mantissa, exponent);
        return buffer + 2;                                            // RETURN
    }
    else if ((*buffer & 0xC0) == 0x80) {

        // 3-byte encoding is used.

        unsigned char eByte1 = buffer[0] & 0x3F;

        int exponent = (eByte1 >> 3) - 4;
        int mantissa = static_cast<int>(((eByte1 & 0x07) << 16) |
                                        static_cast<int>(buffer[1] << 8) |
                                        static_cast<int>(buffer[2]));
        *decimal = decimal64FromUnpackedSpecial(mantissa, exponent);
        return buffer + 3;                                            // RETURN
    }
    else if (*buffer == 0xFF) {

        // Full 9-byte encoding is used.
        ++buffer;

        bsls::Types::Uint64 encoded;
        bsl::memcpy(&encoded, buffer, 8);
        encoded = BSLS_BYTEORDER_NTOHLL(encoded);

        decimal64FromBID(decimal, reinterpret_cast<unsigned char *>(&encoded));

        return buffer + 8;                                            // RETURN
    }
    else  {
        // Here, the condition ((*buffer & 0xC0) == 0xC0) is true, and so the
        // 4-byte encoding is used.

        unsigned char eByte1     = buffer[0] & 0x3F;
        bool          isNegative = eByte1 >> 5;
        int           exponent   = (eByte1 & 0x1F) - 16;
        int           mantissa   =
                           static_cast<int>(static_cast<int>(buffer[1] << 16) |
                                            static_cast<int>(buffer[2] << 8) |
                                            static_cast<int>(buffer[3]));

        *decimal = decimal64FromUnpackedSpecial(isNegative,
                                                mantissa,
                                                exponent);
        return buffer + 4;                                            // RETURN
    }
}

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

                        // decimalToBID functions

inline
void DecimalConvertUtil::decimal32ToBID(unsigned char *buffer,
                                        Decimal32      decimal)
{
    BinaryIntegralDecimalImpUtil::StorageType32 result;

    result = DecimalImpUtil::convertToBID(*decimal.data());

    bsl::memcpy(buffer, &result, sizeof(result));
}

inline
void DecimalConvertUtil::decimal64ToBID(unsigned char *buffer,
                                        Decimal64      decimal)
{
    BinaryIntegralDecimalImpUtil::StorageType64 result;

    result = DecimalImpUtil::convertToBID(*decimal.data());

    bsl::memcpy(buffer, &result, sizeof(result));
}

inline
void DecimalConvertUtil::decimal128ToBID(unsigned char *buffer,
                                         Decimal128     decimal)
{
    BinaryIntegralDecimalImpUtil::StorageType128 result;

    result = DecimalImpUtil::convertToBID(*decimal.data());

    bsl::memcpy(buffer, &result, sizeof(result));
}

inline
void DecimalConvertUtil::decimalToBID(unsigned char *buffer,
                                      Decimal32      decimal)
{
    BinaryIntegralDecimalImpUtil::StorageType32 result;

    result = DecimalImpUtil::convertToBID(*decimal.data());

    bsl::memcpy(buffer, &result, sizeof(result));
}

inline
void DecimalConvertUtil::decimalToBID(unsigned char *buffer,
                                      Decimal64      decimal)
{
    BinaryIntegralDecimalImpUtil::StorageType64 result;

    result = DecimalImpUtil::convertToBID(*decimal.data());

    bsl::memcpy(buffer, &result, sizeof(result));
}

inline
void DecimalConvertUtil::decimalToBID(unsigned char *buffer,
                                      Decimal128     decimal)
{
    BinaryIntegralDecimalImpUtil::StorageType128 result;

    result = DecimalImpUtil::convertToBID(*decimal.data());

    bsl::memcpy(buffer, &result, sizeof(result));
}

                        // decimalFromBID functions

inline
Decimal32
DecimalConvertUtil::decimal32FromBID(const unsigned char *buffer)
{
    BinaryIntegralDecimalImpUtil::StorageType32 bid;

    bsl::memcpy(&bid, buffer, sizeof(bid));

    return Decimal32(DecimalImpUtil::convertFromBID(bid));
}

inline
void
DecimalConvertUtil::decimal32FromBID(Decimal32           *decimal,
                                     const unsigned char *buffer)
{
    BinaryIntegralDecimalImpUtil::StorageType32 bid;

    bsl::memcpy(&bid, buffer, sizeof(bid));

    *decimal = Decimal32(DecimalImpUtil::convertFromBID(bid));
}

inline
Decimal64
DecimalConvertUtil::decimal64FromBID(const unsigned char *buffer)
{
    BinaryIntegralDecimalImpUtil::StorageType64 bid;

    bsl::memcpy(&bid, buffer, sizeof(bid));

    return Decimal64(DecimalImpUtil::convertFromBID(bid));
}

inline
void
DecimalConvertUtil::decimal64FromBID(Decimal64           *decimal,
                                     const unsigned char *buffer)
{
    BinaryIntegralDecimalImpUtil::StorageType64 bid;

    bsl::memcpy(&bid, buffer, sizeof(bid));

    *decimal = Decimal64(DecimalImpUtil::convertFromBID(bid));
}

inline
Decimal128
DecimalConvertUtil::decimal128FromBID(const unsigned char *buffer)
{
    BinaryIntegralDecimalImpUtil::StorageType128 bid;

    bsl::memcpy(&bid, buffer, sizeof(bid));

    return Decimal128(DecimalImpUtil::convertFromBID(bid));
}

inline
void
DecimalConvertUtil::decimal128FromBID(Decimal128          *decimal,
                                      const unsigned char *buffer)
{
    BinaryIntegralDecimalImpUtil::StorageType128 bid;

    bsl::memcpy(&bid, buffer, sizeof(bid));

    *decimal = Decimal128(DecimalImpUtil::convertFromBID(bid));
}

inline
void
DecimalConvertUtil::decimalFromBID(Decimal32           *decimal,
                                   const unsigned char *buffer)
{
    BinaryIntegralDecimalImpUtil::StorageType32 bid;

    bsl::memcpy(&bid, buffer, sizeof(bid));

    *decimal = Decimal32(DecimalImpUtil::convertFromBID(bid));
}

inline
void
DecimalConvertUtil::decimalFromBID(Decimal64           *decimal,
                                   const unsigned char *buffer)
{
    BinaryIntegralDecimalImpUtil::StorageType64 bid;

    bsl::memcpy(&bid, buffer, sizeof(bid));

    *decimal = Decimal64(DecimalImpUtil::convertFromBID(bid));
}

inline
void
DecimalConvertUtil::decimalFromBID(Decimal128          *decimal,
                                   const unsigned char *buffer)
{
    BinaryIntegralDecimalImpUtil::StorageType128 bid;

    bsl::memcpy(&bid, buffer, sizeof(bid));

    *decimal = Decimal128(DecimalImpUtil::convertFromBID(bid));
}

                        // decimalToDPD functions

inline
void DecimalConvertUtil::decimal32ToDPD(unsigned char *buffer,
                                        Decimal32      decimal)
{
    Imp::decimalToDPD(buffer, decimal);
}

inline
void DecimalConvertUtil::decimal64ToDPD(unsigned char *buffer,
                                        Decimal64      decimal)
{
    Imp::decimalToDPD(buffer, decimal);
}

inline
void DecimalConvertUtil::decimal128ToDPD(unsigned char *buffer,
                                         Decimal128     decimal)
{
    Imp::decimalToDPD(buffer, decimal);
}

inline
void DecimalConvertUtil::decimalToDPD(unsigned char *buffer,
                                      Decimal32      decimal)
{
    Imp::decimalToDPD(buffer, decimal);
}

inline
void DecimalConvertUtil::decimalToDPD(unsigned char *buffer,
                                      Decimal64      decimal)
{
    Imp::decimalToDPD(buffer, decimal);
}

inline
void DecimalConvertUtil::decimalToDPD(unsigned char *buffer,
                                      Decimal128     decimal)
{
    Imp::decimalToDPD(buffer, decimal);
}

                        // decimalFromDPD functions

inline
Decimal32
DecimalConvertUtil::decimal32FromDPD(const unsigned char *buffer)
{
    return Imp::decimal32FromDPD(buffer);
}

inline
void
DecimalConvertUtil::decimal32FromDPD(Decimal32           *decimal,
                                     const unsigned char *buffer)
{
    *decimal = Imp::decimal32FromDPD(buffer);
}

inline
Decimal64
DecimalConvertUtil::decimal64FromDPD(const unsigned char *buffer)
{
    return Imp::decimal64FromDPD(buffer);
}

inline
void
DecimalConvertUtil::decimal64FromDPD(Decimal64           *decimal,
                                     const unsigned char *buffer)
{
    *decimal = Imp::decimal64FromDPD(buffer);
}

inline
Decimal128
DecimalConvertUtil::decimal128FromDPD(const unsigned char *buffer)
{
    return Imp::decimal128FromDPD(buffer);
}

inline
void
DecimalConvertUtil::decimal128FromDPD(Decimal128          *decimal,
                                      const unsigned char *buffer)
{
    *decimal = Imp::decimal128FromDPD(buffer);
}

inline
void
DecimalConvertUtil::decimalFromDPD(Decimal32           *decimal,
                                   const unsigned char *buffer)
{
    Imp::decimalFromDPD(decimal, buffer);
}

inline
void
DecimalConvertUtil::decimalFromDPD(Decimal64           *decimal,
                                   const unsigned char *buffer)
{
    Imp::decimalFromDPD(decimal, buffer);
}

inline
void
DecimalConvertUtil::decimalFromDPD(Decimal128          *decimal,
                                   const unsigned char *buffer)
{
    Imp::decimalFromDPD(decimal, buffer);
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
