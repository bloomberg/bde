// balber_berutil.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balber_berutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balber_berutil_cpp, "$Id$ $CSID$")

#include <bdlt_prolepticdateimputil.h>

#include <bdlt_iso8601util.h>

#include <bdlb_bitutil.h>

#include <bdlsb_fixedmemoutstreambuf.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bdldfp_decimalconvertutil.h>

#include <bslmf_assert.h>

#include <bslmt_once.h>

#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstdint.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace balber {
namespace {

                   // ======================================
                   // struct BerUtil_64BitFloatingPointMasks
                   // ======================================

struct BerUtil_64BitFloatingPointMasks {
    // This internal-linkage, component-private utility 'struct' provides a
    // namespace for a suite of 64-bit mask constants used in the construction
    // of floating-point values.
    //
    // Note that the constants in this component have integral values that are
    // too large to be used as enumerators on some platforms.

    // CLASS DATA
    static const bsls::Types::Uint64 k_DOUBLE_EXPONENT_MASK;
    static const bsls::Types::Uint64 k_DOUBLE_MANTISSA_MASK;
    static const bsls::Types::Uint64 k_DOUBLE_MANTISSA_IMPLICIT_ONE_MASK;
    static const bsls::Types::Uint64 k_DOUBLE_SIGN_MASK;
};

                   // --------------------------------------
                   // struct BerUtil_64BitFloatingPointMasks
                   // --------------------------------------

// CLASS DATA
const bsls::Types::Uint64
    BerUtil_64BitFloatingPointMasks::k_DOUBLE_EXPONENT_MASK =
        0x7FF0000000000000ULL;

const bsls::Types::Uint64
    BerUtil_64BitFloatingPointMasks::k_DOUBLE_MANTISSA_MASK =
        0x000FFFFFFFFFFFFFULL;

const bsls::Types::Uint64
    BerUtil_64BitFloatingPointMasks::k_DOUBLE_MANTISSA_IMPLICIT_ONE_MASK =
        0x0010000000000000ULL;

const bsls::Types::Uint64 BerUtil_64BitFloatingPointMasks::k_DOUBLE_SIGN_MASK =
    0x8000000000000000ULL;

}  // close unnamed namespace

                               // --------------
                               // struct BerUtil
                               // --------------

// CLASS METHODS
int BerUtil::getIdentifierOctets(bsl::streambuf         *streamBuf,
                                 BerConstants::TagClass *tagClass,
                                 BerConstants::TagType  *tagType,
                                 int                    *tagNumber,
                                 int                    *accumNumBytesConsumed)
{
    return BerUtil_IdentifierImpUtil::getIdentifierOctets(
        tagClass, tagType, tagNumber, accumNumBytesConsumed, streamBuf);
}

int BerUtil::putIdentifierOctets(bsl::streambuf         *streamBuf,
                                 BerConstants::TagClass  tagClass,
                                 BerConstants::TagType   tagType,
                                 int                     tagNumber)
{
    return BerUtil_IdentifierImpUtil::putIdentifierOctets(
        streamBuf, tagClass, tagType, tagNumber);
}

                      // --------------------------------
                      // struct BerUtil_IdentifierImpUtil
                      // --------------------------------

// CLASS METHODS
int BerUtil_IdentifierImpUtil::getIdentifierOctets(
                                 BerConstants::TagClass *tagClass,
                                 BerConstants::TagType  *tagType,
                                 int                    *tagNumber,
                                 int                    *accumNumBytesConsumed,
                                 bsl::streambuf         *streamBuf)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    int nextOctet = streamBuf->sbumpc();

    if (bsl::streambuf::traits_type::eof() == nextOctet) {
        return FAILURE;                                               // RETURN
    }

    ++*accumNumBytesConsumed;

    *tagClass =
        static_cast<BerConstants::TagClass>(nextOctet & k_TAG_CLASS_MASK);

    *tagType = static_cast<BerConstants::TagType>(nextOctet & k_TAG_TYPE_MASK);

    if (k_TAG_NUMBER_MASK != (nextOctet & k_TAG_NUMBER_MASK)) {
        // The tag number fits in a single octet.

        *tagNumber = nextOctet & k_TAG_NUMBER_MASK;
        return SUCCESS;                                               // RETURN
    }

    *tagNumber = 0;

    for (int i = 0; i < k_MAX_TAG_NUMBER_OCTETS; ++i) {
        nextOctet = streamBuf->sbumpc();
        if (bsl::streambuf::traits_type::eof() == nextOctet) {
            return FAILURE;                                           // RETURN
        }

        ++*accumNumBytesConsumed;

        *tagNumber <<= k_NUM_VALUE_BITS_IN_TAG_OCTET;
        *tagNumber |= nextOctet & k_SEVEN_BITS_MASK;

        if (!(nextOctet & k_CHAR_MSB_MASK)) {
            return SUCCESS;                                           // RETURN
        }
    }

    return FAILURE;
}

int BerUtil_IdentifierImpUtil::putIdentifierOctets(
                                             bsl::streambuf         *streamBuf,
                                             BerConstants::TagClass  tagClass,
                                             BerConstants::TagType   tagType,
                                             int                     tagNumber)
    // Write the specified 'tag*' to the specified 'streamBuf'.
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (tagNumber < 0) {
        return FAILURE;                                               // RETURN
    }

    const unsigned char tagClassUchar = static_cast<unsigned char>(tagClass);
    const unsigned char tagTypeUchar  = static_cast<unsigned char>(tagType);

    if (tagNumber <= k_MAX_TAG_NUMBER_IN_ONE_OCTET) {
        unsigned char octet = static_cast<unsigned char>(
            tagClassUchar | tagTypeUchar | tagNumber);

        return octet == streamBuf->sputc(octet) ? SUCCESS : FAILURE;  // RETURN
    }

    // Send multiple identifier octets.

    unsigned char firstOctet = static_cast<unsigned char>(
        tagClassUchar | tagTypeUchar | k_TAG_NUMBER_MASK);

    if (firstOctet != streamBuf->sputc(firstOctet)) {
        return FAILURE;                                               // RETURN
    }

    // Find the number of octets required.

    int numOctetsRequired = 0;

    {
        enum { INT_NUM_BITS = sizeof(int) * Constants::k_NUM_BITS_PER_OCTET };

        int          shift = 0;
        unsigned int mask  = k_SEVEN_BITS_MASK;

        for (int i = 0; INT_NUM_BITS > shift; ++i) {
            if (tagNumber & mask) {
                numOctetsRequired = i + 1;
            }

            shift += k_NUM_VALUE_BITS_IN_TAG_OCTET;
            mask <<= k_NUM_VALUE_BITS_IN_TAG_OCTET;
        }
    }

    BSLS_ASSERT(numOctetsRequired <= k_MAX_TAG_NUMBER_OCTETS);

    // Put all octets except the last one.

    int shift = (numOctetsRequired - 1) * k_NUM_VALUE_BITS_IN_TAG_OCTET;
    unsigned int mask = k_SEVEN_BITS_MASK << shift;

    for (int i = 0; i < numOctetsRequired - 1; ++i) {
        unsigned char nextOctet = static_cast<unsigned char>(
            (mask & tagNumber) >> shift | k_CHAR_MSB_MASK);

        if (nextOctet != streamBuf->sputc(nextOctet)) {
            return FAILURE;                                           // RETURN
        }

        shift -= k_NUM_VALUE_BITS_IN_TAG_OCTET;
        mask = k_SEVEN_BITS_MASK << shift;
    }

    // Put the final octet.

    tagNumber &= k_SEVEN_BITS_MASK;

    return tagNumber == streamBuf->sputc(static_cast<char>(tagNumber))
               ? SUCCESS
               : FAILURE;
}

                           // ---------------------
                           // BerUtil_LengthImpUtil
                           // ---------------------

// CLASS METHODS

// Length Decoding Functions

int BerUtil_LengthImpUtil::getLength(int            *result,
                                     int            *accumNumBytesConsumed,
                                     bsl::streambuf *streamBuf)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    int nextOctet = streamBuf->sbumpc();
    if (bsl::streambuf::traits_type::eof() == nextOctet) {
        return FAILURE;                                               // RETURN
    }

    ++*accumNumBytesConsumed;

    if (nextOctet == k_INDEFINITE_LENGTH_OCTET) {
        *result = k_INDEFINITE_LENGTH;

        return SUCCESS;                                               // RETURN
    }

    unsigned int numOctets = static_cast<unsigned int>(nextOctet);

    // Length has been transmitted in short form.

    if (!(numOctets & k_LONG_FORM_LENGTH_FLAG_MASK)) {
        *result = numOctets;
        return SUCCESS;                                               // RETURN
    }

    // Length has been transmitted in long form.

    numOctets &= k_LONG_FORM_LENGTH_VALUE_MASK;

    if (numOctets > sizeof(int)) {
        return FAILURE;                                               // RETURN
    }

    *result = 0;
    for (unsigned int i = 0; i < numOctets; ++i) {
        nextOctet = streamBuf->sbumpc();
        if (bsl::streambuf::traits_type::eof() == nextOctet) {
            return FAILURE;                                           // RETURN
        }

        *result <<= Constants::k_NUM_BITS_PER_OCTET;
        *result |= nextOctet;
    }

    *accumNumBytesConsumed += numOctets;

    return SUCCESS;
}

int BerUtil_LengthImpUtil::getEndOfContentOctets(
                                         int            *accumNumBytesConsumed,
                                         bsl::streambuf *streamBuf)
{
    *accumNumBytesConsumed += 2;

    char buffer[2];
    if (sizeof(buffer) != streamBuf->sgetn(buffer, sizeof(buffer))) {
        return -1;                                                    // RETURN
    }

    if ('\x00' != buffer[0] || '\x00' != buffer[1]) {
        return -1;                                                    // RETURN
    }

    return 0;
}

// Length Encoding Functions

int BerUtil_LengthImpUtil::putLength(bsl::streambuf *streamBuf, int length)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (length < 0) {
        return FAILURE;                                               // RETURN
    }

    if (length <= 127) {
        const char lengthChar = static_cast<char>(length);
        return length == streamBuf->sputc(lengthChar) ? SUCCESS
                                                      : FAILURE;      // RETURN
    }

    // length > 127.

    int numOctets = sizeof(int);
    for (unsigned int mask = ~(static_cast<unsigned int>(-1) >>
                               Constants::k_NUM_BITS_PER_OCTET);
         !(length & mask);
         mask >>= Constants::k_NUM_BITS_PER_OCTET) {
        --numOctets;
    }

    unsigned char lengthOctet =
        static_cast<unsigned char>(numOctets | k_LONG_FORM_LENGTH_FLAG_MASK);
    if (lengthOctet != streamBuf->sputc(lengthOctet)) {
        return FAILURE;                                               // RETURN
    }

    return RawIntegerUtil::putIntegerGivenLength(streamBuf, length, numOctets);
}

int BerUtil_LengthImpUtil::putIndefiniteLengthOctet(bsl::streambuf *streamBuf)
{
    // Extra unsigned char cast needed to suppress warning on Windows.
    int writtenOctet = streamBuf->sputc(static_cast<char>(
        static_cast<unsigned char>(k_INDEFINITE_LENGTH_OCTET)));

    if (k_INDEFINITE_LENGTH_OCTET != writtenOctet) {
        return -1;                                                    // RETURN
    }

    return 0;
}

int BerUtil_LengthImpUtil::putEndOfContentOctets(bsl::streambuf *streamBuf)
{
    const char buffer[2] = {'\x00', '\x00'};

    if (sizeof(buffer) != streamBuf->sputn(buffer, sizeof(buffer))) {
        return -1;                                                    // RETURN
    }

    return 0;
}

                       // -----------------------------
                       // struct BerUtil_IntegerImpUtil
                       // -----------------------------

// CLASS METHODS
int BerUtil_IntegerImpUtil::getNumOctetsToStream(short value)
{
    // This overload of 'numBytesToStream' is optimized for a 16-bit 'value'.

    if (0 == value) {
        return 1;                                                     // RETURN
    }

    int numBits;
    if (value > 0) {
        // For positive values, all but one 0 bits on the left are redundant.
        //: o 'find1AtLargestIndex' returns '[0 .. 14]' (since bit 15 is 0).
        //: o Add 1 to convert from an index to a count in range '[1 .. 15]'.
        //: o Add 1 to preserve the sign bit, for a value in range '[2 .. 16]'.

        numBits = 31 -
                  bdlb::BitUtil::numLeadingUnsetBits(
                      static_cast<bsl::uint32_t>(value)) +
                  2;
    }
    else {
        // For negative values, all but: one 1 bits on the left are redundant.
        //: o 'find0AtLargestIndex' returns '[0 .. 14]' (since bit 15 is 1).
        //: o Add 1 to convert from an index to a count in range '[1 .. 15]'.
        //: o Add 1 to preserve the sign bit, for a value in range '[2 .. 16]'.

        numBits = 31 -
                  bdlb::BitUtil::numLeadingUnsetBits(
                      ~static_cast<bsl::uint32_t>(value)) +
                  2;
    }

    // Round up to correct number of bytes:

    return (numBits + Constants::k_NUM_BITS_PER_OCTET - 1) /
           Constants::k_NUM_BITS_PER_OCTET;
}

int BerUtil_IntegerImpUtil::getNumOctetsToStream(int value)
{
    // This overload of 'numBytesToStream' is optimized for a 32-bit 'value'.

    if (0 == value) {
        return 1;                                                     // RETURN
    }

    int numBits;
    if (value > 0) {
        // For positive values, all but one 0 bits on the left are redundant.
        //: o 'find1AtLargestIndex' returns '[0 .. 30]' (since bit 31 is 0).
        //: o Add 1 to convert from an index to a count in range '[1 .. 31]'.
        //: o Add 1 to preserve the sign bit, for a value in range '[2 .. 32]'.

        numBits = 31 -
                  bdlb::BitUtil::numLeadingUnsetBits(
                      static_cast<bsl::uint32_t>(value)) +
                  2;
    }
    else {
        // For negative values, all but one 1 bits on the left are redundant.
        //: o 'find0AtLargestIndex' returns '[0 .. 30]' (since bit 31 is 1).
        //: o Add 1 to convert from an index to a count in range '[1 .. 31]'.
        //: o Add 1 to preserve the sign bit, for a value in range '[2 .. 32]'.

        numBits = 31 -
                  bdlb::BitUtil::numLeadingUnsetBits(
                      ~static_cast<bsl::uint32_t>(value)) +
                  2;
    }

    // Round up to correct number of bytes:

    return (numBits + Constants::k_NUM_BITS_PER_OCTET - 1) /
           Constants::k_NUM_BITS_PER_OCTET;
}

int BerUtil_IntegerImpUtil::getNumOctetsToStream(long long value)
{
    // This overload of 'numBytesToStream' is optimized for a 64-bit 'value'.

    if (0 == value) {
        return 1;                                                     // RETURN
    }

    int numBits;
    if (value > 0) {
        // For positive values, all but one 0 bits on the left are redundant.
        //: o 'find1AtLargestIndex64' returns '[0 .. 62]' (since bit 63 is 0).
        //: o Add 1 to convert from an index to a count in range '[1 .. 63]'.
        //: o Add 1 to preserve the sign bit, for a value in range '[2 .. 64]'.

        numBits = 63 -
                  bdlb::BitUtil::numLeadingUnsetBits(
                      static_cast<bsl::uint64_t>(value)) +
                  2;
    }
    else {
        // For negative values, all but one 1 bits on the left are redundant.
        //: o 'find1AtLargestIndex64' returns '[0 .. 62]' (since bit 63 is 0).
        //: o Add 1 to convert from an index to a count in range '[1 .. 63]'.
        //: o Add 1 to preserve the sign bit, for a value in range '[2 .. 64]'.

        numBits = 63 -
                  bdlb::BitUtil::numLeadingUnsetBits(
                      ~static_cast<bsl::uint64_t>(value)) +
                  2;
    }

    // Round up to correct number of bytes:

    return (numBits + Constants::k_NUM_BITS_PER_OCTET - 1) /
           Constants::k_NUM_BITS_PER_OCTET;
}

int BerUtil_IntegerImpUtil::getIntegerValue(long long      *value,
                                            bsl::streambuf *streamBuf,
                                            int             length)
{
    // IMPLEMENTATION NOTE: This overload of the 'getIntegerValue' function
    // template, for 'long long', is warranted solely for performance reasons:
    // the template definition performs up to 8 shift operations (which on
    // 32-bit SPARC are quite slow for 'long long').  This implementation
    // breaks up the 'long long' into high- and low-order words to perform the
    // shifts on 'int' instead of 'long long'.

    enum { SUCCESS = 0, FAILURE = -1 };
    static const int SIGN_BIT_MASK = 0x80;

    if (static_cast<unsigned>(length) > sizeof(long long)) {
        // Overflow.

        return FAILURE;                                               // RETURN
    }

    int          sign    = (streamBuf->sgetc() & SIGN_BIT_MASK) ? -1 : 0;
    unsigned int valueLo = sign;
    unsigned int valueHi = sign;

    // Decode high-order word.

    for (; length > static_cast<int>(sizeof(int)); --length) {
        int nextOctet = streamBuf->sbumpc();
        if (bsl::streambuf::traits_type::eof() == nextOctet) {
            return FAILURE;                                           // RETURN
        }

        valueHi <<= Constants::k_NUM_BITS_PER_OCTET;
        valueHi |= static_cast<unsigned char>(nextOctet);
    }

    // Decode low-order word.

    for (; length > 0; --length) {
        int nextOctet = streamBuf->sbumpc();
        if (bsl::streambuf::traits_type::eof() == nextOctet) {
            return FAILURE;                                           // RETURN
        }

        valueLo <<= Constants::k_NUM_BITS_PER_OCTET;
        valueLo |= static_cast<unsigned char>(nextOctet);
    }

    // Combine low and high word into a long word.
#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    reinterpret_cast<unsigned int *>(value)[1] = valueLo;
    reinterpret_cast<unsigned int *>(value)[0] = valueHi;
#else
    reinterpret_cast<unsigned int *>(value)[0] = valueLo;
    reinterpret_cast<unsigned int *>(value)[1] = valueHi;
#endif
    return SUCCESS;
}

                    // -----------------------------------
                    // struct BerUtil_FloatingPointImpUtil
                    // -----------------------------------

///Implementation Note:
///--------------------
//
// The IEEE 754 single precision floating point representation is:
//..
// 31  30 - 23         22 - 0
// +-+--------+----------------------+
// |S|EEEEEEEE|MMMMMMMMMMMMMMMMMMMMMM|
// +-+--------+----------------------+
//
// where
//  S - sign bit (0 - positive, 1 for negative numbers)
//  E - exponent bits (0 - 255)
//  M - mantissa bits
//..
//
// The value corresponding to a floating point number is calculated by the
// following formulas;
//
//: 1 If '0 < E < 255', then 'V = (-1) ** S * 2 ** (E - bias) * (1.F)', where
//:   '1.F' is intended to represent the binary number created by prefixing 'F'
//:   with an implicit leading 1 and a binary point.  'bias = 127'.
//: 2 If 'E = 0', 'F' is non-zero, then
//:   'V = (-1) ** S * 2 ** (-bias + 1) * (0.F)', where '0.F' is called the
//:   denormalized value.
//: 3 If 'E = 255', 'F' is non-zero, then 'V = NaN' (not a number).
//: 4 If 'E = 255', 'F = 0', and 'S = 1', then 'V = -Infinity'.
//: 5 If 'E = 255', 'F = 0', and 'S = 0', then 'V = Infinity'.
//: 6 IF 'E =   0', 'F = 0', AND 'S = X', then 'V = 0'.
//
// For double precision floating point numbers the representation is
//..
// 63   62 - 52                          51 - 0
// +-+-----------+----------------------------------------------------+
// |S|EEEEEEEEEEE|MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM|
// +-+-----------+----------------------------------------------------+
//
// where
//  S - sign bit (0 - positive, 1 for negative numbers)
//  E - exponent bits (0 - 2047)
//  M - mantissa bits
//..
//
// The value corresponding to a floating point number is calculated by the
// following formulas
//
//: 1 If '0 < E < 2047', then 'V = (-1) ** S * 2 ** (E - bias) * (1.F)', where
//:   '1.F' is intended to represent the binary number created by prefixing 'F'
//:   with an implicit leading 1 and a binary point, and 'bias = 1023'.
//: 2 If 'E = 0', 'F' is non-zero, then
//:   'V = (-1) ** S * 2 ** (-bias + 1) * (0.F)', where '0.F' is called the
//:   unnormalized value.
//: 3 If 'E = 2047', 'F' is non-zero then V = NaN (not a number).
//: 4 If 'E = 2047', 'F = 0', and 'S = 1', then 'V = -Infinity'.
//: 5 If 'E = 2047', 'F = 0', and 'S = 0', then 'V = Infinity'.
//: 6 IF 'E =    0', 'F = 0', AND 'S = X', then V = 0'.
//
// The general algorithm for streaming out real data in BER format:
//
//: 1 Extract the mantissa, exponent and sign values from the floating point
//:   number, following the IEEE 754 representation.
//: 2 Check the values for special values like positive and negative infinity,
//:   and NaN value.  Numbers having exponent value zero are identified as
//:   denormalized numbers.
//: 3 Stream out the header information associated with that real value in the
//:   first octet.  Essentially this entails specifying the base used, a
//:   scaling factor for adjusting the position of the implicit decimal, sign,
//:   and the length of exponent octets.  If the number is a special value,
//:   then a specific format is used for the first octet.
//: 4 Normalize the mantissa value by prefixing the implicit 1 and positioning
//:   the implicit decimal point after the rightmost 1 bit.  Also adjust the
//:   exponent accordingly, that is decrement it for each right shift of the
//:   decimal point.  This provides the normalized mantissa and adjusted
//:   exponent values.  Note that for denormalized numbers the implicit leading
//:   1 is not prefixed to the mantissa.
//: 5 Stream out the exponent - bias value.
//: 6 Stream out the normalized mantissa value.
//
// Nothing needs to be done for negative numbers.  Only the sign will signify
// that the number under consideration is negative.
//
// The general algorithm for streaming in real data in BER format:
//
//: 1 Read the length and the first octet from the stream.
//: 2 The combination of the length and the first octet will identify if the
//:   value is one of zero, positive infinity, or negative infinity.  Extract
//:   the sign, base, scale factor, and the number of exponent octets from the
//:   first octet.
//: 3 Read the exponent from the stream.  Convert the exponent into base 2.
//:   Subtract the scale factor from the exponent.  Note that the scale factor
//:   needs to be subtracted as this amount will be accounted for later in the
//:   algorithm.
//: 4 Calculate the amount the mantissa should be left shifted to convert it
//:   into IEEE representation.  This varies depending on whether the number is
//:   being streamed in is a normalized/denormalized number.  The exponent of a
//:   denormalized will have an extremely low exponent value (less than -1022
//:   to be precise).  If a number is denormalized, 'NUM_MANTISSA_BITS - shift'
//:   is added to the exponent and the mantissa shifted by the opposite amount.
//: 5 The bias value is added to the exponent to get its final value.
//: 6 Assemble the double value from the mantissa, exponent, and sign values.

// PRIVATE CLASS METHODS

// Utilities

void BerUtil_FloatingPointImpUtil::assembleDouble(double    *value,
                                                  long long  exponent,
                                                  long long  mantissa,
                                                  int        sign)
{
    typedef BerUtil_64BitFloatingPointMasks WideBitMasks;

    unsigned long long longLongValue;

    longLongValue = static_cast<unsigned long long>(exponent)
                    << k_DOUBLE_EXPONENT_SHIFT;
    longLongValue |= mantissa & WideBitMasks::k_DOUBLE_MANTISSA_MASK;

    if (sign) {
        longLongValue |= WideBitMasks::k_DOUBLE_SIGN_MASK;
    }

    bsl::memcpy(value, &longLongValue, sizeof(double));
}

void BerUtil_FloatingPointImpUtil::normalizeMantissaAndAdjustExp(
                                                       long long *mantissa,
                                                       int       *exponent,
                                                       bool       denormalized)
{
    typedef BerUtil_64BitFloatingPointMasks WideBitMasks;

    if (!denormalized) {
        // If number is not denormalized then need to prefix with implicit one.

        *mantissa |= WideBitMasks::k_DOUBLE_MANTISSA_IMPLICIT_ONE_MASK;
    }

    int shift = bdlb::BitUtil::numTrailingUnsetBits(
        static_cast<bsl::uint64_t>(*mantissa));
    *mantissa >>= shift;
    *exponent -= (k_DOUBLE_NUM_MANTISSA_BITS - shift);

    if (denormalized) {
        *exponent += 1;
    }
}

void BerUtil_FloatingPointImpUtil::parseDouble(int       *exponent,
                                               long long *mantissa,
                                               int       *sign,
                                               double     value)
{
    typedef BerUtil_64BitFloatingPointMasks WideBitMasks;

    unsigned long long longLongValue;
    bsl::memcpy(&longLongValue, &value, sizeof(unsigned long long));

    *sign     = longLongValue & WideBitMasks::k_DOUBLE_SIGN_MASK ? 1 : 0;
    *exponent = static_cast<int>(
        (longLongValue & WideBitMasks::k_DOUBLE_EXPONENT_MASK) >>
        k_DOUBLE_EXPONENT_SHIFT);
    *mantissa = longLongValue & WideBitMasks::k_DOUBLE_MANTISSA_MASK;
}

// CLASS METHODS

// Decoding

int BerUtil_FloatingPointImpUtil::getDoubleValue(double         *value,
                                                 bsl::streambuf *streamBuf,
                                                 int             length)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (0 == length) {
        *value = 0;
        return SUCCESS;                                               // RETURN
    }

    int nextOctet = streamBuf->sbumpc();

    if (k_POSITIVE_INFINITY_ID == nextOctet) {
        assembleDouble(
            value, k_DOUBLE_INFINITY_EXPONENT_ID, k_INFINITY_MANTISSA_ID, 0);
        return SUCCESS;                                               // RETURN
    }
    else if (k_NEGATIVE_INFINITY_ID == nextOctet) {
        assembleDouble(
            value, k_DOUBLE_INFINITY_EXPONENT_ID, k_INFINITY_MANTISSA_ID, 1);
        return SUCCESS;                                               // RETURN
    }
    else if (k_NAN_ID == nextOctet) {
        assembleDouble(value, k_DOUBLE_INFINITY_EXPONENT_ID, 1, 0);
        return SUCCESS;                                               // RETURN
    }

    if (!(nextOctet & static_cast<unsigned char>(k_REAL_BINARY_ENCODING))) {
        // Encoding is decimal, return as that is not handled currently.

        return FAILURE;                                               // RETURN
    }

    int sign = nextOctet & k_REAL_SIGN_MASK ? 1 : 0;
    int base = (nextOctet & k_REAL_BASE_MASK) >> k_REAL_BASE_SHIFT;
    if (k_BER_RESERVED_BASE == base) {
        // Base value is not supported.

        return FAILURE;                                               // RETURN
    }

    base *= 8;

    int scaleFactor =
        (nextOctet & k_REAL_SCALE_FACTOR_MASK) >> k_REAL_SCALE_FACTOR_SHIFT;
    int expLength = (nextOctet & k_REAL_EXPONENT_LENGTH_MASK) + 1;

    if (k_REAL_MULTIPLE_EXPONENT_OCTETS == expLength) {
        // Exponent length is encoded in the following octet.

        expLength = streamBuf->sbumpc();

        if (bsl::streambuf::traits_type::eof() == expLength ||
            static_cast<unsigned>(expLength) > sizeof(long long)) {
            // Exponent values that take greater than sizeof(long long) octets
            // are not handled by this implementation.

            return FAILURE;                                           // RETURN
        }
    }

    long long exponent;
    if (IntegerUtil::getIntegerValue(&exponent, streamBuf, expLength)) {
        return FAILURE;                                               // RETURN
    }

    if (0 != base) {
        // Convert exponent to base 2.

        exponent *= 8 == base ? 3 : 4;
    }
    exponent -= scaleFactor;

    long long mantissa       = 0;
    int       mantissaLength = length - expLength - 1;
    if (IntegerUtil::getIntegerValue(&mantissa, streamBuf, mantissaLength)) {
        return FAILURE;                                               // RETURN
    }

    int shift = bdlb::BitUtil::numLeadingUnsetBits(
        static_cast<bsl::uint64_t>(mantissa));
    if (64 == shift) {
        return FAILURE;                                               // RETURN
    }

    // Subtract the number of exponent bits and the sign bit.

    shift -= k_DOUBLE_NUM_EXPONENT_BITS + 1;
    exponent += k_DOUBLE_BIAS + k_DOUBLE_NUM_MANTISSA_BITS - shift - 1;

    if (exponent > 0) {  // Normal number
        // Shift the mantissa left by shift amount, account for the implicit
        // one, and then removing it.

        typedef BerUtil_64BitFloatingPointMasks WideBitMasks;
        mantissa <<= shift + 1;
        mantissa &= ~WideBitMasks::k_DOUBLE_MANTISSA_IMPLICIT_ONE_MASK;
    }
    else {
        // Denormalized number: shift mantissa only, no implicit one.

        mantissa <<= exponent + shift;
        exponent = 0;
    }

    *value = 0;
    assembleDouble(value, exponent, mantissa, sign);
    return SUCCESS;
}

int BerUtil_FloatingPointImpUtil::getDecimal64Value(
                                                  bdldfp::Decimal64 *value,
                                                  bsl::streambuf    *streamBuf,
                                                  int                length)
{
    if (k_MAX_MULTI_WIDTH_ENCODING_SIZE < length) {
        return -1;                                                    // RETURN
    }

    unsigned char         buffer[k_MAX_MULTI_WIDTH_ENCODING_SIZE];
    const bsl::streamsize bytesConsumed =
        streamBuf->sgetn(reinterpret_cast<char *>(buffer), length);
    if (bytesConsumed != length) {
        return -1;                                                    // RETURN
    }

    if (0 !=
        bdldfp::DecimalConvertUtil::decimal64FromMultiWidthEncodingIfValid(
            value, buffer, length)) {
        return -1;                                                    // RETURN
    }

    return 0;
}

// Encoding

int BerUtil_FloatingPointImpUtil::putDecimal64Value(
                                                  bsl::streambuf    *streamBuf,
                                                  bdldfp::Decimal64  value)
{
    unsigned char          buffer[k_MAX_MULTI_WIDTH_ENCODING_SIZE];
    bsls::Types::size_type length =
        bdldfp::DecimalConvertUtil::decimal64ToMultiWidthEncoding(buffer,
                                                                  value);

    if (0 != LengthUtil::putLength(streamBuf, static_cast<int>(length))) {
        return -1;                                                    // RETURN
    }

    if (static_cast<bsl::streamsize>(length) !=
        streamBuf->sputn(reinterpret_cast<char *>(buffer), length)) {
        return -1;                                                    // RETURN
    }

    return 0;
}

int BerUtil_FloatingPointImpUtil::putDoubleValue(bsl::streambuf *streamBuf,
                                                 double          value)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    // If 0 == value, put out length = 0 and return.

    if (0.0 == value) {
        return 0 == streamBuf->sputc(0) ? SUCCESS : FAILURE;          // RETURN
    }

    // Else parse double value.

    int       exponent, sign;
    long long mantissa;

    parseDouble(&exponent, &mantissa, &sign, value);

    // Check for special cases +/- infinity and NaN.

    if (k_DOUBLE_INFINITY_EXPONENT_ID == exponent) {
        if (k_INFINITY_MANTISSA_ID == mantissa) {
            char signOctet =
                sign ? k_NEGATIVE_INFINITY_ID : k_POSITIVE_INFINITY_ID;

            return (1 == streamBuf->sputc(1) &&
                    signOctet == streamBuf->sputc(signOctet))
                       ? SUCCESS
                       : FAILURE;                                     // RETURN
        }
        else {
            // For NaN use bit pattern 0x42.

            char NaN = k_NAN_ID;
            return (1 == streamBuf->sputc(1) && NaN == streamBuf->sputc(NaN))
                       ? SUCCESS
                       : FAILURE;                                     // RETURN
        }
    }

    bool denormalized = 0 == exponent ? true : false;

    // Normalize the mantissa, get its actual value and adjust the exponent
    // accordingly.

    normalizeMantissaAndAdjustExp(&mantissa, &exponent, denormalized);

    exponent -= k_DOUBLE_BIAS;

    int expLength = IntegerUtil::getNumOctetsToStream(exponent);
    int manLength = IntegerUtil::getNumOctetsToStream(mantissa);

    // Put out the length = expLength + manLength + 1.

    char totalLength = static_cast<char>(expLength + manLength + 1);
    if (totalLength != streamBuf->sputc(totalLength)) {
        return FAILURE;                                               // RETURN
    }

    unsigned char firstOctet =
        sign ? k_BINARY_NEGATIVE_NUMBER_ID : k_BINARY_POSITIVE_NUMBER_ID;

    if (2 == expLength) {
        // Two exponent octets will be sent out.

        firstOctet |= 1;
    }

    if (firstOctet != streamBuf->sputc(firstOctet)) {
        return FAILURE;                                               // RETURN
    }

    // Put out the exponent and mantissa.

    return IntegerUtil::putIntegerGivenLength(
               streamBuf, exponent, expLength) ||
                   IntegerUtil::putIntegerGivenLength(
                       streamBuf, mantissa, manLength)
               ? FAILURE
               : SUCCESS;
}

                        // ----------------------------
                        // struct BerUtil_StringImpUtil
                        // ----------------------------

// CLASS METHODS

// Utilities

int BerUtil_StringImpUtil::putChars(bsl::streambuf *streamBuf,
                                    char            value,
                                    int             numChars)
{
    BSLS_ASSERT(0 <= numChars);

    enum { k_LOCAL_BUFFER_SIZE = 16 };

    const int numLoops     = numChars / k_LOCAL_BUFFER_SIZE;
    const int numRemaining = numChars % k_LOCAL_BUFFER_SIZE;
    BSLS_ASSERT(numLoops * k_LOCAL_BUFFER_SIZE + numRemaining == numChars);

    char buffer[k_LOCAL_BUFFER_SIZE];
    bsl::memset(buffer, value, numChars);

    for (int i = 0; i != numLoops; ++i) {
        if (k_LOCAL_BUFFER_SIZE !=
            streamBuf->sputn(buffer, k_LOCAL_BUFFER_SIZE)) {
            return -1;                                                // RETURN
        }
    }

    if (numRemaining != streamBuf->sputn(buffer, numRemaining)) {
        return -1;                                                    // RETURN
    }

    return 0;
}

// 'bsl::string' Decoding

int BerUtil_StringImpUtil::getStringValue(bsl::string              *value,
                                          bsl::streambuf           *streamBuf,
                                          int                       length,
                                          const BerDecoderOptions&  options)
{
    if (0 == length) {
        if (options.defaultEmptyStrings() && !value->empty()) {
            BSLMT_ONCE_DO
            {
                BSLS_LOG_WARN("[BDE_INTERNAL] The current process will decode "
                              "an empty string as the default value for an "
                              "element in the type currently being decoded.  "
                              "This behavior is erroneous and will eventually "
                              "be deprecated.  The owners of the current "
                              "process should be contacted to audit it for "
                              "dependence on this behavior.");
            }
        }

        if (!options.defaultEmptyStrings()) {
            value->clear();
        }

        return 0;                                                     // RETURN
    }
    else if (length < 0) {
        return -1;                                                    // RETURN
    }

    value->resize(length);

    const bsl::streamsize bytesConsumed =
        streamBuf->sgetn(&(*value)[0], length);
    if (length != bytesConsumed) {
        return -1;                                                    // RETURN
    }

    return 0;
}

                       // -----------------------------
                       // struct BerUtil_Iso8601ImpUtil
                       // -----------------------------

// CLASS METHODS

// Decoding

int BerUtil_Iso8601ImpUtil::getDateValue(bdlt::Date     *value,
                                         bsl::streambuf *streamBuf,
                                         int             length)
{
    return getValue(value, streamBuf, length);
}

int BerUtil_Iso8601ImpUtil::getDateTzValue(bdlt::DateTz   *value,
                                           bsl::streambuf *streamBuf,
                                           int             length)
{
    return getValue(value, streamBuf, length);
}

int BerUtil_Iso8601ImpUtil::getDatetimeValue(bdlt::Datetime *value,
                                             bsl::streambuf *streamBuf,
                                             int             length)
{
    return getValue(value, streamBuf, length);
}

int BerUtil_Iso8601ImpUtil::getDatetimeTzValue(bdlt::DatetimeTz *value,
                                               bsl::streambuf   *streamBuf,
                                               int               length)
{
    return getValue(value, streamBuf, length);
}

int BerUtil_Iso8601ImpUtil::getTimeValue(bdlt::Time     *value,
                                         bsl::streambuf *streamBuf,
                                         int             length)
{
    return getValue(value, streamBuf, length);
}

int BerUtil_Iso8601ImpUtil::getTimeTzValue(bdlt::TimeTz   *value,
                                           bsl::streambuf *streamBuf,
                                           int             length)
{
    return getValue(value, streamBuf, length);
}

// Encoding

int BerUtil_Iso8601ImpUtil::putDateValue(bsl::streambuf          *streamBuf,
                                         const bdlt::Date&        value,
                                         const BerEncoderOptions *options)
{
    return putValue(streamBuf, value, options);
}

int BerUtil_Iso8601ImpUtil::putDateTzValue(bsl::streambuf          *streamBuf,
                                           const bdlt::DateTz&      value,
                                           const BerEncoderOptions *options)
{
    return putValue(streamBuf, value, options);
}

int BerUtil_Iso8601ImpUtil::putDatetimeValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::Datetime&    value,
                                            const BerEncoderOptions *options)
{
    return putValue(streamBuf, value, options);
}

int BerUtil_Iso8601ImpUtil::putDatetimeTzValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::DatetimeTz&  value,
                                            const BerEncoderOptions *options)
{
    return putValue(streamBuf, value, options);
}

int BerUtil_Iso8601ImpUtil::putTimeValue(bsl::streambuf          *streamBuf,
                                         const bdlt::Time&        value,
                                         const BerEncoderOptions *options)
{
    return putValue(streamBuf, value, options);
}

int BerUtil_Iso8601ImpUtil::putTimeTzValue(bsl::streambuf          *streamBuf,
                                           const bdlt::TimeTz&      value,
                                           const BerEncoderOptions *options)
{
    return putValue(streamBuf, value, options);
}

                    // ------------------------------------
                    // struct BerUtil_TimezoneOffsetImpUtil
                    // ------------------------------------

// CLASS METHODS
bool BerUtil_TimezoneOffsetImpUtil::isValidTimezoneOffsetInMinutes(int value)
{
    return (k_MIN_OFFSET <= value) && (k_MAX_OFFSET >= value);
}

int BerUtil_TimezoneOffsetImpUtil::getTimezoneOffsetInMinutes(
                                                     int            *value,
                                                     bsl::streambuf *streamBuf)
{
    char buffer[2];

    if (sizeof(buffer) != streamBuf->sgetn(buffer, sizeof(buffer))) {
        return -1;                                                    // RETURN
    }

    const unsigned short firstOctet =
        static_cast<unsigned short>(buffer[0]) & 0xFF;

    const unsigned short secondOctet =
        static_cast<unsigned short>(buffer[1]) & 0xFF;

    *value = static_cast<short>((firstOctet << 8) | secondOctet);
    return 0;
}

int BerUtil_TimezoneOffsetImpUtil::getTimezoneOffsetInMinutesIfValid(
                                                     int            *value,
                                                     bsl::streambuf *streamBuf)
{
    int tempValue = 0;
    if (0 != getTimezoneOffsetInMinutes(&tempValue, streamBuf)) {
        return -1;                                                    // RETURN
    }

    if (!isValidTimezoneOffsetInMinutes(tempValue)) {
        return -1;                                                    // RETURN
    }

    *value = tempValue;
    return 0;
}

int BerUtil_TimezoneOffsetImpUtil::putTimezoneOffsetInMinutes(
                                                     bsl::streambuf *streamBuf,
                                                     int             value)
{
    BSLS_ASSERT(isValidTimezoneOffsetInMinutes(value));

    const char buffer[2] = {static_cast<char>((value & 0xFF00) >> 8),
                            static_cast<char>((value & 0x00FF) >> 0)};

    if (sizeof(buffer) != streamBuf->sputn(buffer, sizeof(buffer))) {
        return -1;                                                    // RETURN
    }

    return 0;
}

                         // --------------------------
                         // struct BerUtil_DateImpUtil
                         // --------------------------

// PRIVATE CLASS METHODS

// 'bdlt::Date' Decoding

int BerUtil_DateImpUtil::getIso8601DateValue(bdlt::Date     *value,
                                             bsl::streambuf *streamBuf,
                                             int             length)
{
    return Iso8601Util::getDateValue(value, streamBuf, length);
}

int BerUtil_DateImpUtil::getCompactBinaryDateValue(bdlt::Date     *value,
                                                   bsl::streambuf *streamBuf,
                                                   int             length)
{
    bsls::Types::Int64 daysSinceEpoch;
    if (0 !=
        IntegerUtil::getIntegerValue(&daysSinceEpoch, streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    return daysSinceEpochToDate(value, daysSinceEpoch);
}

// 'bdlt::Date' Encoding

int BerUtil_DateImpUtil::putIso8601DateValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::Date&        value,
                                            const BerEncoderOptions *options)
{
    return Iso8601Util::putDateValue(streamBuf, value, options);
}

int BerUtil_DateImpUtil::putCompactBinaryDateValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::Date&        value,
                                            const BerEncoderOptions *)
{
    bsls::Types::Int64 daysSinceEpoch;
    dateToDaysSinceEpoch(&daysSinceEpoch, value);

    const int length = IntegerUtil::getNumOctetsToStream(daysSinceEpoch);
    BSLS_ASSERT(length <= k_MAX_COMPACT_BINARY_DATE_LENGTH);

    if (0 != LengthUtil::putLength(streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    return IntegerUtil::putIntegerGivenLength(
        streamBuf, daysSinceEpoch, length);
}

// 'bdlt::DateTz' Decoding

int BerUtil_DateImpUtil::getIso8601DateTzValue(bdlt::DateTz   *value,
                                               bsl::streambuf *streamBuf,
                                               int             length)
{
    return Iso8601Util::getDateTzValue(value, streamBuf, length);
}

int BerUtil_DateImpUtil::getCompactBinaryDateValue(bdlt::DateTz   *value,
                                                   bsl::streambuf *streamBuf,
                                                   int             length)
{
    bdlt::Date localDate;
    if (0 != getCompactBinaryDateValue(&localDate, streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    enum {
        k_TIMEZONE_OFFSET = 0,
    };

    return value->setDateTzIfValid(localDate, k_TIMEZONE_OFFSET);
}

int BerUtil_DateImpUtil::getCompactBinaryDateTzValue(bdlt::DateTz   *value,
                                                     bsl::streambuf *streamBuf,
                                                     int             length)
{
    int timezoneOffsetInMinutes = 0;
    if (length >= k_MIN_COMPACT_BINARY_DATETZ_LENGTH) {
        if (0 != TimezoneUtil::getTimezoneOffsetInMinutesIfValid(
                     &timezoneOffsetInMinutes, streamBuf)) {
            *value = bdlt::DateTz();
            return -1;                                                // RETURN
        }

        length -= TimezoneUtil::k_TIMEZONE_LENGTH;
    }

    bdlt::Date localDate;
    if (0 != getCompactBinaryDateValue(&localDate, streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    return value->setDateTzIfValid(localDate, timezoneOffsetInMinutes);
}

// 'bdlt::DateTz' Encoding

int BerUtil_DateImpUtil::putIso8601DateTzValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::DateTz&      value,
                                            const BerEncoderOptions *options)
{
    return Iso8601Util::putDateTzValue(streamBuf, value, options);
}

int BerUtil_DateImpUtil::putCompactBinaryDateValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::DateTz&      value,
                                            const BerEncoderOptions *options)
{
    BSLS_ASSERT_SAFE(0 == value.offset());
    return putCompactBinaryDateValue(streamBuf, value.localDate(), options);
}

int BerUtil_DateImpUtil::putCompactBinaryDateTzValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::DateTz&      value,
                                            const BerEncoderOptions *)
{
    const bdlt::Date& date   = value.localDate();
    const int         offset = value.offset();

    BSLS_ASSERT(0 != offset);

    bsls::Types::Int64 daysSinceEpoch;
    dateToDaysSinceEpoch(&daysSinceEpoch, date);

    const int length = IntegerUtil::getNumOctetsToStream(daysSinceEpoch) +
                       TimezoneUtil::k_TIMEZONE_LENGTH;

    if (k_MIN_COMPACT_BINARY_DATETZ_LENGTH > length) {
        if (0 != LengthUtil::putLength(streamBuf,
                                       k_MIN_COMPACT_BINARY_DATETZ_LENGTH)) {
            return -1;                                                // RETURN
        }

        if (0 != TimezoneUtil::putTimezoneOffsetInMinutes(streamBuf, offset)) {
            return -1;                                                // RETURN
        }

        const char padChar =
            daysSinceEpoch < 0 ? static_cast<char>(-1) : static_cast<char>(0);
        const int numPadOctets = k_MIN_COMPACT_BINARY_DATETZ_LENGTH - length;
        if (0 != StringUtil::putChars(streamBuf, padChar, numPadOctets)) {
            return -1;                                                // RETURN
        }

        return IntegerUtil::putIntegerGivenLength(
            streamBuf, daysSinceEpoch, length - TimezoneUtil::k_TIMEZONE_LENGTH);
                                                                      // RETURN
    }

    if (0 != LengthUtil::putLength(streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    if (0 != TimezoneUtil::putTimezoneOffsetInMinutes(streamBuf, offset)) {
        return -1;                                                    // RETURN
    }

    return IntegerUtil::putIntegerGivenLength(
        streamBuf, daysSinceEpoch, length - TimezoneUtil::k_TIMEZONE_LENGTH);
}

// CLASS METHODS

// 'bdlt::Date' Decoding

int BerUtil_DateImpUtil::getDateValue(bdlt::Date     *value,
                                      bsl::streambuf *streamBuf,
                                      int             length)
{
    char firstByte;
    if (0 != StreambufUtil::peekChar(&firstByte, streamBuf)) {
        return -1;                                                    // RETURN
    }

    DateEncoding::Value encoding;
    int rc = detectDateEncoding(&encoding, length, firstByte);

    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    switch (encoding) {
      case DateEncoding::e_ISO8601_DATE: {
        return getIso8601DateValue(value, streamBuf, length);         // RETURN
      } break;
      case DateEncoding::e_COMPACT_BINARY_DATE: {
        return getCompactBinaryDateValue(value, streamBuf, length);   // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

// 'bdlt::Date' Encoding

int BerUtil_DateImpUtil::putDateValue(bsl::streambuf          *streamBuf,
                                      const bdlt::Date&        value,
                                      const BerEncoderOptions *options)
{
    // Applications can create invalid 'bdlt::Date' objects in optimized build
    // modes.  As this function assumes that 'value' is valid, it is possible
    // to encode an invalid 'bdlt::Date' without returning an error.  Decoding
    // the corresponding output can result in hard-to-trace decoding errors.
    // So to identify such errors early, we return an error if 'value' is not
    // valid.

    bdlt::Date valueCopy(value);
    if (0 != valueCopy.addDaysIfValid(0)) {
        return -1;                                                    // RETURN
    }

    switch (selectDateEncoding(value, options)) {
      case DateEncoding::e_ISO8601_DATE: {
        return putIso8601DateValue(streamBuf, value, options);
      } break;
      case DateEncoding::e_COMPACT_BINARY_DATE: {
        return putCompactBinaryDateValue(streamBuf, value, options);
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

// 'bdlt::DateTz' Decoding

int BerUtil_DateImpUtil::getDateTzValue(bdlt::DateTz   *value,
                                        bsl::streambuf *streamBuf,
                                        int             length)
{
    char firstByte;
    if (0 != StreambufUtil::peekChar(&firstByte, streamBuf)) {
        return -1;                                                    // RETURN
    }

    DateTzEncoding::Value encoding;
    int rc = detectDateTzEncoding(&encoding, length, firstByte);

    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    switch (encoding) {
      case DateTzEncoding::e_ISO8601_DATETZ: {
        return getIso8601DateTzValue(value, streamBuf, length);       // RETURN
      } break;
      case DateTzEncoding::e_COMPACT_BINARY_DATE: {
        return getCompactBinaryDateValue(value, streamBuf, length);   // RETURN
      } break;
      case DateTzEncoding::e_COMPACT_BINARY_DATETZ: {
        return getCompactBinaryDateTzValue(value, streamBuf, length);
                                                                      // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

// 'bdlt::DateTz' Encoding

int BerUtil_DateImpUtil::putDateTzValue(bsl::streambuf          *streamBuf,
                                        const bdlt::DateTz&      value,
                                        const BerEncoderOptions *options)
{
    // Applications can create invalid 'bdlt::DateTz' objects in optimized
    // build modes.  As this function assumes that 'value' is valid, it is
    // possible to encode an invalid 'bdlt::DateTz' without returning an error.
    // Decoding the corresponding output can result in hard-to-trace decoding
    // errors.  So to identify such errors early, we return an error if 'value'
    // is not valid.

    if (0 != value.localDate().addDaysIfValid(0) ||
        !bdlt::DateTz::isValid(value.localDate(), value.offset())) {
        return -1;                                                    // RETURN
    }

    switch (selectDateTzEncoding(value, options)) {
      case DateTzEncoding::e_ISO8601_DATETZ: {
        return putIso8601DateTzValue(streamBuf, value, options);      // RETURN
      } break;
      case DateTzEncoding::e_COMPACT_BINARY_DATE: {
        return putCompactBinaryDateValue(streamBuf, value, options);
                                                                      // RETURN
      } break;
      case DateTzEncoding::e_COMPACT_BINARY_DATETZ: {
        return putCompactBinaryDateTzValue(streamBuf, value, options);
                                                                      // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reachable");
#if BSLA_UNREACHABLE_IS_ACTIVE
    BSLA_UNREACHABLE;
#else
    return -1;                                                        // RETURN
#endif
}

                         // --------------------------
                         // struct BerUtil_TimeImpUtil
                         // --------------------------

// PRIVATE CLASS METHODS

// 'bdlt::Time' Decoding

int BerUtil_TimeImpUtil::getCompactBinaryTimeValue(bdlt::Time     *value,
                                                   bsl::streambuf *streamBuf,
                                                   int             length)
{
    int millisecondsSinceMidnight;
    if (0 != IntegerUtil::getIntegerValue(
                 &millisecondsSinceMidnight, streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    return millisecondsSinceMidnightToTime(value, millisecondsSinceMidnight);
}

int BerUtil_TimeImpUtil::getExtendedBinaryTimeValue(bdlt::Time     *value,
                                                    bsl::streambuf *streamBuf,
                                                    int             length)
{
    if (k_EXTENDED_BINARY_TIME_LENGTH != length) {
        return -1;                                                    // RETURN
    }

    DateAndTimeHeader header;
    if (0 != DateAndTimeHeaderUtil::getValue(&header, streamBuf)) {
        return -1;                                                    // RETURN
    }

    if (!header.isExtendedBinary()) {
        return -1;                                                    // RETURN
    }

    bsls::Types::Int64 microsecondsSinceMidnight;
    if (0 != IntegerUtil::get40BitIntegerValue(&microsecondsSinceMidnight,
                                               streamBuf)) {
        return -1;                                                    // RETURN
    }

    return microsecondsSinceMidnightToTime(value, microsecondsSinceMidnight);
}

// 'bdlt::Time' Encoding

int BerUtil_TimeImpUtil::putCompactBinaryTimeValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::Time&        value,
                                            const BerEncoderOptions *)
{
    int millisecondsSinceMidnight;
    timeToMillisecondsSinceMidnight(&millisecondsSinceMidnight, value);

    const int length =
        IntegerUtil::getNumOctetsToStream(millisecondsSinceMidnight);

    BSLS_ASSERT(length <= k_MAX_COMPACT_BINARY_TIME_LENGTH);

    if (0 != LengthUtil::putLength(streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    return IntegerUtil::putIntegerGivenLength(
        streamBuf, millisecondsSinceMidnight, length);
}

int BerUtil_TimeImpUtil::putExtendedBinaryTimeValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::Time&        value,
                                            const BerEncoderOptions *)
{
    if (0 != LengthUtil::putLength(streamBuf, k_EXTENDED_BINARY_TIME_LENGTH)) {
        return -1;                                                    // RETURN
    }

    if (0 != DateAndTimeHeaderUtil::putExtendedBinaryWithoutTimezoneValue(
                 streamBuf)) {
        return -1;                                                    // RETURN
    }

    bsls::Types::Int64 microsecondsSinceMidnight;
    timeToMicrosecondsSinceMidnight(&microsecondsSinceMidnight, value);

    return IntegerUtil::put40BitIntegerValue(streamBuf,
                                             microsecondsSinceMidnight);
}

// 'bdlt::TimeTz' Decoding

int BerUtil_TimeImpUtil::getCompactBinaryTimeValue(bdlt::TimeTz   *value,
                                                   bsl::streambuf *streamBuf,
                                                   int             length)
{
    bdlt::Time localTime;
    if (0 != getCompactBinaryTimeValue(&localTime, streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    enum {
        k_TIMEZONE_OFFSET = 0,
    };

    return value->setTimeTzIfValid(localTime, k_TIMEZONE_OFFSET);
}

int BerUtil_TimeImpUtil::getCompactBinaryTimeTzValue(bdlt::TimeTz   *value,
                                                     bsl::streambuf *streamBuf,
                                                     int             length)
{
    int timezoneOffsetInMinutes = 0;
    if (length >= k_MIN_COMPACT_BINARY_TIMETZ_LENGTH) {
        typedef BerUtil_TimezoneOffsetImpUtil TimezoneUtil;

        if (0 != TimezoneUtil::getTimezoneOffsetInMinutesIfValid(
                     &timezoneOffsetInMinutes, streamBuf)) {
            *value = bdlt::TimeTz();
            return -1;                                                // RETURN
        }

        length -= TimezoneUtil::k_TIMEZONE_LENGTH;
    }

    bdlt::Time localTime;
    if (0 != getCompactBinaryTimeValue(&localTime, streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    return value->setTimeTzIfValid(localTime, timezoneOffsetInMinutes);
}

int BerUtil_TimeImpUtil::getExtendedBinaryTimeTzValue(
                                                     bdlt::TimeTz   *value,
                                                     bsl::streambuf *streamBuf,
                                                     int             length)
{
    if (k_EXTENDED_BINARY_TIMETZ_LENGTH != length) {
        return -1;                                                    // RETURN
    }

    DateAndTimeHeader header;
    if (0 != DateAndTimeHeaderUtil::getValue(&header, streamBuf)) {
        return -1;                                                    // RETURN
    }

    if (!header.isExtendedBinary()) {
        return -1;                                                    // RETURN
    }

    bsls::Types::Int64 microseconds;
    if (0 != IntegerUtil::get40BitIntegerValue(&microseconds, streamBuf)) {
        return -1;                                                    // RETURN
    }

    bdlt::Time localTime;
    if (0 != microsecondsSinceMidnightToTime(&localTime, microseconds)) {
        return -1;                                                    // RETURN
    }

    return value->setTimeTzIfValid(localTime,
                                   header.timezoneOffsetInMinutes());
}

// 'bdlt::TimeTz' Encoding

int BerUtil_TimeImpUtil::putCompactBinaryTimeValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::TimeTz&      value,
                                            const BerEncoderOptions *options)
{
    BSLS_ASSERT(0 == value.offset());

    const bdlt::Time& time = value.localTime();

    return putCompactBinaryTimeValue(streamBuf, time, options);
}

int BerUtil_TimeImpUtil::putCompactBinaryTimeTzValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::TimeTz&      value,
                                            const BerEncoderOptions *options)
{
    const bdlt::Time& time   = value.localTime();
    const int         offset = value.offset();

    if (!offset) {
        return putCompactBinaryTimeValue(streamBuf, time, options);   // RETURN
    }

    int serialTime;
    timeToMillisecondsSinceMidnight(&serialTime, time);

    const int length = IntegerUtil::getNumOctetsToStream(serialTime) +
                       TimezoneUtil::k_TIMEZONE_LENGTH;

    if (k_MIN_COMPACT_BINARY_TIMETZ_LENGTH > length) {
        const int numPadOctets = k_MIN_COMPACT_BINARY_TIMETZ_LENGTH - length;

        if (0 != LengthUtil::putLength(streamBuf,
                                       k_MIN_COMPACT_BINARY_TIMETZ_LENGTH)) {
            return -1;                                                // RETURN
        }

        if (0 != TimezoneUtil::putTimezoneOffsetInMinutes(streamBuf, offset)) {
            return -1;                                                // RETURN
        }

        if (0 != StringUtil::putChars(streamBuf, 0, numPadOctets)) {
            return -1;                                                // RETURN
        }

        return IntegerUtil::putIntegerGivenLength(
            streamBuf, serialTime, length - TimezoneUtil::k_TIMEZONE_LENGTH);
    }

    if (0 != LengthUtil::putLength(streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    if (0 != TimezoneUtil::putTimezoneOffsetInMinutes(streamBuf, offset)) {
        return -1;                                                    // RETURN
    }

    return IntegerUtil::putIntegerGivenLength(
        streamBuf, serialTime, length - TimezoneUtil::k_TIMEZONE_LENGTH);
}

int BerUtil_TimeImpUtil::putExtendedBinaryTimeTzValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::TimeTz&      value,
                                            const BerEncoderOptions *)
{
    if (0 !=
        LengthUtil::putLength(streamBuf, k_EXTENDED_BINARY_TIMETZ_LENGTH)) {
        return -1;                                                    // RETURN
    }

    const bdlt::Time localTime = value.localTime();
    const int        offset    = value.offset();

    if (0 != DateAndTimeHeaderUtil::putExtendedBinaryWithTimezoneValue(
                 streamBuf, offset)) {
        return -1;                                                    // RETURN
    }

    bsls::Types::Int64 microsecondsSinceMidnight;
    timeToMicrosecondsSinceMidnight(&microsecondsSinceMidnight, localTime);

    return IntegerUtil::putIntegerGivenLength(
        streamBuf, microsecondsSinceMidnight, 5);
}

                       // ------------------------------
                       // struct BerUtil_DatetimeImpUtil
                       // ------------------------------

// PRIVATE CLASS METHODS

// Utilities

void BerUtil_DatetimeImpUtil::datetimeToMillisecondsSinceEpoch(
                              bsls::Types::Int64    *millisecondsSinceEpoch,
                              const bdlt::Datetime&  value)
{
    bsls::Types::Int64 daysSinceEpoch;
    DateUtil::dateToDaysSinceEpoch(&daysSinceEpoch, value.date());

    int millisecondsSinceMidnight;
    TimeUtil::timeToMillisecondsSinceMidnight(&millisecondsSinceMidnight,
                                              value.time());

    typedef bdlt::TimeUnitRatio Ratio;
    *millisecondsSinceEpoch =
        daysSinceEpoch * Ratio::k_MILLISECONDS_PER_DAY +
        millisecondsSinceMidnight;
}

int BerUtil_DatetimeImpUtil::millisecondsSinceEpochToDatetime(
                                    bdlt::Datetime     *value,
                                    bsls::Types::Int64  millisecondsSinceEpoch)
{
    typedef bdlt::TimeUnitRatio Ratio;

    bsls::Types::Int64 daysSinceEpoch =
        millisecondsSinceEpoch / Ratio::k_MILLISECONDS_PER_DAY;

    if (millisecondsSinceEpoch < 0) {
        --daysSinceEpoch;
        if (DateUtil::k_COMPACT_BINARY_DATE_EPOCH == -daysSinceEpoch) {
            ++daysSinceEpoch;
        }
    }

    const int millisecondsSinceMidnight =
        static_cast<int>(millisecondsSinceEpoch -
                         daysSinceEpoch * Ratio::k_MILLISECONDS_PER_DAY);

    int hour = millisecondsSinceMidnight / Ratio::k_MILLISECONDS_PER_HOUR;

    const int minute = (millisecondsSinceMidnight -
                        hour * Ratio::k_MILLISECONDS_PER_HOUR_32) /
                       Ratio::k_MILLISECONDS_PER_MINUTE_32;

    const int second =
        (millisecondsSinceMidnight - hour * Ratio::k_MILLISECONDS_PER_HOUR_32 -
         minute * Ratio::k_MILLISECONDS_PER_MINUTE_32) /
        Ratio::k_MILLISECONDS_PER_SECOND_32;

    const int millisec = millisecondsSinceMidnight -
                         hour * Ratio::k_MILLISECONDS_PER_HOUR_32 -
                         minute * Ratio::k_MILLISECONDS_PER_MINUTE_32 -
                         second * Ratio::k_MILLISECONDS_PER_SECOND_32;

    if (24 == hour) {
        hour = 0;
        ++daysSinceEpoch;
    }

    const int daysSince0001Jan01 = static_cast<int>(
        daysSinceEpoch + DateUtil::k_COMPACT_BINARY_DATE_EPOCH);

    if (!bdlt::ProlepticDateImpUtil::isValidSerial(daysSince0001Jan01)) {
        return -1;                                                    // RETURN
    }

    int year, month, day;
    bdlt::ProlepticDateImpUtil::serialToYmd(
        &year, &month, &day, daysSince0001Jan01);

    return value->setDatetimeIfValid(
        year, month, day, hour, minute, second, millisec);
}

// 'bdlt::Datetime' Decoding

int BerUtil_DatetimeImpUtil::getIso8601DatetimeValue(bdlt::Datetime *value,
                                                     bsl::streambuf *streamBuf,
                                                     int             length)
{
    return Iso8601Util::getDatetimeValue(value, streamBuf, length);
}

int BerUtil_DatetimeImpUtil::getCompactBinaryDatetimeValue(
                                                     bdlt::Datetime *value,
                                                     bsl::streambuf *streamBuf,
                                                     int             length)
{
    bsls::Types::Int64 millisecondsSinceEpoch;
    if (0 != IntegerUtil::getIntegerValue(
                 &millisecondsSinceEpoch, streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    return millisecondsSinceEpochToDatetime(value, millisecondsSinceEpoch);
}

int BerUtil_DatetimeImpUtil::getCompactBinaryDatetimeTzValue(
                                                     bdlt::Datetime *value,
                                                     bsl::streambuf *streamBuf,
                                                     int             length)
{
    BSLS_ASSERT(length >= k_MIN_COMPACT_BINARY_DATETIMETZ_LENGTH);

    int offset;
    if (0 != TimezoneUtil::getTimezoneOffsetInMinutes(&offset, streamBuf)) {
        return -1;                                                    // RETURN
    }

    bsls::Types::Int64 millisecondsSinceEpoch;
    if (0 != IntegerUtil::getIntegerValue(
                 &millisecondsSinceEpoch,
                 streamBuf,
                 length - TimezoneUtil::k_TIMEZONE_LENGTH)) {
        return -1;                                                    // RETURN
    }

    return millisecondsSinceEpochToDatetime(value, millisecondsSinceEpoch);
}

int BerUtil_DatetimeImpUtil::getExtendedBinaryDatetimeValue(
                                                     bdlt::Datetime *value,
                                                     bsl::streambuf *streamBuf,
                                                     int             length)
{
    if (k_EXTENDED_BINARY_DATETIME_LENGTH != length) {
        return -1;                                                    // RETURN
    }

    DateAndTimeHeader header;
    if (0 != DateAndTimeHeaderUtil::getValue(&header, streamBuf)) {
        return -1;                                                    // RETURN
    }

    if (!header.isExtendedBinary()) {
        return -1;                                                    // RETURN
    }

    int daysSince0001Jan01;
    if (0 != IntegerUtil::getIntegerValue(&daysSince0001Jan01, streamBuf, 3)) {
        return -1;                                                    // RETURN
    }

    ++daysSince0001Jan01;

    if (!bdlt::ProlepticDateImpUtil::isValidSerial(daysSince0001Jan01)) {
        return -1;                                                    // RETURN
    }

    int year;
    int month;
    int day;

    bdlt::ProlepticDateImpUtil::serialToYmd(
        &year, &month, &day, daysSince0001Jan01);

    const bdlt::Date date(year, month, day);

    bsls::Types::Int64 microsecondsSinceMidnight;
    if (0 != IntegerUtil::get40BitIntegerValue(&microsecondsSinceMidnight,
                                               streamBuf)) {
        return -1;                                                    // RETURN
    }

    bdlt::Time time;
    if (0 != TimeUtil::microsecondsSinceMidnightToTime(
                 &time, microsecondsSinceMidnight)) {
        return -1;                                                    // RETURN
    }

    value->setDatetime(date, time);
    return 0;
}

// 'bdlt::Datetime' Encoding

int BerUtil_DatetimeImpUtil::putIso8601DatetimeValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::Datetime&    value,
                                            const BerEncoderOptions *options)
{
    return Iso8601Util::putDatetimeValue(streamBuf, value, options);
}

int BerUtil_DatetimeImpUtil::putCompactBinaryDatetimeValue(
                                       bsl::streambuf          *streamBuf,
                                       bsls::Types::Int64       serialDatetime,
                                       int                      length,
                                       const BerEncoderOptions *)
{
    if (0 != LengthUtil::putLength(streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    return IntegerUtil::putIntegerGivenLength(
        streamBuf, serialDatetime, length);
}

int BerUtil_DatetimeImpUtil::putCompactBinaryDatetimeValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::Datetime&    value,
                                            const BerEncoderOptions *options)
{
    bsls::Types::Int64 millisecondsSinceEpoch;
    datetimeToMillisecondsSinceEpoch(&millisecondsSinceEpoch, value);

    const int length =
        IntegerUtil::getNumOctetsToStream(millisecondsSinceEpoch);

    return putCompactBinaryDatetimeValue(
        streamBuf, millisecondsSinceEpoch, length, options);
}

int BerUtil_DatetimeImpUtil::putCompactBinaryDatetimeTzValue(
                               bsl::streambuf          *streamBuf,
                               bsls::Types::Int64       millisecondsSinceEpoch,
                               int                      length,
                               const BerEncoderOptions *)
{
    enum { k_TIMEZONE_OFFSET = 0 };

    BSLS_ASSERT(k_MIN_COMPACT_BINARY_DATETIMETZ_LENGTH <= length);

    if (0 != LengthUtil::putLength(streamBuf,
                                   length + TimezoneUtil::k_TIMEZONE_LENGTH)) {
        return -1;                                                    // RETURN
    }

    if (0 != TimezoneUtil::putTimezoneOffsetInMinutes(streamBuf,
                                                      k_TIMEZONE_OFFSET)) {
        return -1;                                                    // RETURN
    }

    return IntegerUtil::putIntegerGivenLength(
        streamBuf, millisecondsSinceEpoch, length);
}

int BerUtil_DatetimeImpUtil::putExtendedBinaryDatetimeValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::Datetime&    value,
                                            const BerEncoderOptions *)
{
    if (0 !=
        LengthUtil::putLength(streamBuf, k_EXTENDED_BINARY_DATETIME_LENGTH)) {
        return -1;                                                    // RETURN
    }

    if (0 != DateAndTimeHeaderUtil::putExtendedBinaryWithoutTimezoneValue(
                 streamBuf)) {
        return -1;                                                    // RETURN
    }

    const bdlt::Date date       = value.date();
    const int        serialDate = bdlt::ProlepticDateImpUtil::ymdToSerial(
                                      date.year(), date.month(), date.day()) -
                           1;

    if (0 != IntegerUtil::putIntegerGivenLength(streamBuf, serialDate, 3)) {
        return -1;                                                    // RETURN
    }

    bsls::Types::Int64 microsecondsSinceMidnight;
    TimeUtil::timeToMicrosecondsSinceMidnight(&microsecondsSinceMidnight,
                                              value.time());

    return IntegerUtil::putIntegerGivenLength(
        streamBuf, microsecondsSinceMidnight, 5);
}

// 'bdlt::DatetimeTz' Decoding

int BerUtil_DatetimeImpUtil::getIso8601DatetimeTzValue(
                                                   bdlt::DatetimeTz *value,
                                                   bsl::streambuf   *streamBuf,
                                                   int               length)
{
    return Iso8601Util::getDatetimeTzValue(value, streamBuf, length);
}

int BerUtil_DatetimeImpUtil::getCompactBinaryDatetimeValue(
                                                   bdlt::DatetimeTz *value,
                                                   bsl::streambuf   *streamBuf,
                                                   int               length)
{
    BSLS_ASSERT(length < k_MIN_COMPACT_BINARY_DATETIMETZ_LENGTH);

    enum { k_TIMEZONE_OFFSET = 0 };

    bdlt::Datetime localDatetime;
    if (0 !=
        getCompactBinaryDatetimeValue(&localDatetime, streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    return value->setDatetimeTzIfValid(localDatetime, k_TIMEZONE_OFFSET);
}

int BerUtil_DatetimeImpUtil::getCompactBinaryDatetimeTzValue(
                                                   bdlt::DatetimeTz *value,
                                                   bsl::streambuf   *streamBuf,
                                                   int               length)
{
    BSLS_ASSERT(length >= k_MIN_COMPACT_BINARY_DATETIMETZ_LENGTH);

    int timezoneOffsetInMinutes = 0;
    if (0 != TimezoneUtil::getTimezoneOffsetInMinutesIfValid(
                 &timezoneOffsetInMinutes, streamBuf)) {
        *value = bdlt::DatetimeTz();
        return -1;                                                    // RETURN
    }

    length -= TimezoneUtil::k_TIMEZONE_LENGTH;

    bdlt::Datetime localDatetime;
    if (0 !=
        getCompactBinaryDatetimeValue(&localDatetime, streamBuf, length)) {
        return -1;                                                    // RETURN
    }

    return value->setDatetimeTzIfValid(localDatetime, timezoneOffsetInMinutes);
}

int BerUtil_DatetimeImpUtil::getExtendedBinaryDatetimeTzValue(
                                                   bdlt::DatetimeTz *value,
                                                   bsl::streambuf   *streamBuf,
                                                   int               length)
{
    if (k_EXTENDED_BINARY_DATETIMETZ_LENGTH != length) {
        return -1;                                                    // RETURN
    }

    DateAndTimeHeader header;
    if (0 != DateAndTimeHeaderUtil::getValue(&header, streamBuf)) {
        return -1;                                                    // RETURN
    }

    if (!header.isExtendedBinary()) {
        return -1;                                                    // RETURN
    }

    int daysSince0001Jan01;
    if (0 != IntegerUtil::getIntegerValue(&daysSince0001Jan01, streamBuf, 3)) {
        return -1;                                                    // RETURN
    }

    ++daysSince0001Jan01;

    if (!bdlt::ProlepticDateImpUtil::isValidSerial(daysSince0001Jan01)) {
        return -1;                                                    // RETURN
    }

    int year;
    int month;
    int day;
    bdlt::ProlepticDateImpUtil::serialToYmd(
        &year, &month, &day, daysSince0001Jan01);

    const bdlt::Date date(year, month, day);

    bsls::Types::Int64 microsecondsSinceMidnight;
    if (0 != IntegerUtil::get40BitIntegerValue(&microsecondsSinceMidnight,
                                               streamBuf)) {
        return -1;                                                    // RETURN
    }

    bdlt::Time time;
    if (0 != TimeUtil::microsecondsSinceMidnightToTime(
                 &time, microsecondsSinceMidnight)) {
        return -1;                                                    // RETURN
    }

    const bdlt::Datetime datetime(date, time);

    return value->setDatetimeTzIfValid(datetime,
                                       header.timezoneOffsetInMinutes());
}

// 'bdlt::DatetimeTz' Encoding

int BerUtil_DatetimeImpUtil::putIso8601DatetimeTzValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::DatetimeTz&  value,
                                            const BerEncoderOptions *options)
{
    return Iso8601Util::putDatetimeTzValue(streamBuf, value, options);
}

int BerUtil_DatetimeImpUtil::putCompactBinaryDatetimeTzValue(
                              bsl::streambuf          *streamBuf,
                              int                      timezoneOffsetInMinutes,
                              bsls::Types::Int64       serialDatetime,
                              int                      serialDatetimeLength,
                              const BerEncoderOptions *)
{
    if (!TimezoneUtil::isValidTimezoneOffsetInMinutes(
            timezoneOffsetInMinutes)) {
        return -1;                                                    // RETURN
    }

    const int totalLength =
        TimezoneUtil::k_TIMEZONE_LENGTH + serialDatetimeLength;

    if (totalLength < k_MIN_COMPACT_BINARY_DATETIMETZ_LENGTH) {
        const char padChar =
            serialDatetime < 0 ? static_cast<char>(-1) : static_cast<char>(0);
        const int numPadOctets =
            k_MIN_COMPACT_BINARY_DATETIMETZ_LENGTH - totalLength;

        if (0 != LengthUtil::putLength(
                     streamBuf, k_MIN_COMPACT_BINARY_DATETIMETZ_LENGTH)) {
            return -1;                                                // RETURN
        }

        if (0 != TimezoneUtil::putTimezoneOffsetInMinutes(
                     streamBuf, timezoneOffsetInMinutes)) {
            return -1;                                                // RETURN
        }

        if (0 != StringUtil::putChars(streamBuf, padChar, numPadOctets)) {
            return -1;                                                // RETURN
        }
    }
    else {
        if (0 != LengthUtil::putLength(streamBuf, totalLength)) {
            return -1;                                                // RETURN
        }

        if (0 != TimezoneUtil::putTimezoneOffsetInMinutes(
                     streamBuf, timezoneOffsetInMinutes)) {
            return -1;                                                // RETURN
        }
    }

    return IntegerUtil::putIntegerGivenLength(
        streamBuf, serialDatetime, serialDatetimeLength);
}

int BerUtil_DatetimeImpUtil::putExtendedBinaryDatetimeTzValue(
                                            bsl::streambuf          *streamBuf,
                                            const bdlt::DatetimeTz&  value,
                                            const BerEncoderOptions *)
{
    if (0 != LengthUtil::putLength(streamBuf,
                                   k_EXTENDED_BINARY_DATETIMETZ_LENGTH)) {
        return -1;                                                    // RETURN
    }

    const bdlt::Datetime localDatetime = value.localDatetime();
    const bdlt::Date     localDate     = localDatetime.date();
    const bdlt::Time     localTime     = localDatetime.time();
    const int            offset        = value.offset();

    if (0 != DateAndTimeHeaderUtil::putExtendedBinaryWithTimezoneValue(
                 streamBuf, offset)) {
        return -1;                                                    // RETURN
    }

    const int serialDate =
        bdlt::ProlepticDateImpUtil::ymdToSerial(
            localDate.year(), localDate.month(), localDate.day()) -
        1;

    if (0 != IntegerUtil::putIntegerGivenLength(streamBuf, serialDate, 3)) {
        return -1;                                                    // RETURN
    }

    bsls::Types::Int64 microsecondsSinceMidnight;
    TimeUtil::timeToMicrosecondsSinceMidnight(&microsecondsSinceMidnight,
                                              localTime);

    return IntegerUtil::putIntegerGivenLength(
        streamBuf, microsecondsSinceMidnight, 5);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
