// bdem_berutil.cpp                                                   -*-C++-*-
#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_berutil_cpp,"$Id$ $CSID$")

// IMPLEMENTATION NOTES:
//
// The IEEE 754 single precision floating point representation is:
//
// 31  30 - 23         22 - 0
// +-+--------+----------------------+
// |S|EEEEEEEE|MMMMMMMMMMMMMMMMMMMMMM|
// +-+--------+----------------------+
//
// where
//  S - sign bit (0 - positive, 1 for negative numbers)
//  E - exponent bits (0 - 255)
//  M - mantissa bits
//
// The value corresponding to a floating point number is calculated by the
// following formulas
//
//  1. If 0 < E < 255, then V = (-1) ** S * 2 ** (E - bias) * (1.F),
//       where 1.F is intended to represent the binary number created by
//       prefixing F with an implicit leading 1 and a binary point. bias = 127.
//  2. If E = 0, F is non-zero, then V = (-1) ** S * 2 ** (-bias + 1) * (0.F),
//       where 0.F is called the denormalized value.
//  3. If E = 255, F is non-zero, then V = NaN (not a number).
//  4. If E = 255, F = 0 and S = 1, then V = -Infinity.
//  5. If E = 255, F = 0 and S = 0, then V = Infinity.
//  6. IF E =   0, F = 0 AND S = X, then V = 0.
//
// For double precision floating point numbers the representation is
//
// 63   62 - 52                          51 - 0
// +-+-----------+----------------------------------------------------+
// |S|EEEEEEEEEEE|MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM|
// +-+-----------+----------------------------------------------------+
//
// where
//  S - sign bit (0 - positive, 1 for negative numbers)
//  E - exponent bits (0 - 2047)
//  M - mantissa bits
//
// The value corresponding to a floating point number is calculated by the
// following formulas
//
//  1. If 0 < E < 2047, then V = (-1) ** S * 2 ** (E - bias) * (1.F),
//       where 1.F is intended to represent the binary number created by
//       prefixing F with an implicit leading 1 and a binary point, and
//       bias = 1023.
//  2. If E = 0, F is non-zero, then V = (-1) ** S * 2 ** (-bias + 1) * (0.F),
//       where 0.F is called the unnormalized value.
//  3. If E = 2047, F is non-zero then V = NaN (not a number).
//  4. If E = 2047, F = 0 and S = 1, then V = -Infinity.
//  5. If E = 2047, F = 0 and S = 0, then V = Infinity.
//  6. IF E =    0, F = 0 AND S = X, then V = 0.
//
// The general algorithm for streaming out real data in BER format:
//
// 1. Extract the mantissa, exponent and sign values from the floating point
//    number, following the IEEE 754 representation.
// 2. Check the values for special values like positive and negative infinity,
//    and NaN value.  Numbers having exponent value zero are identified as
//    denormalized numbers.
// 3. Stream out the header information associated with that real value in the
//    first octet.  Essentially this entails specifying the base used, a
//    scaling factor for adjusting the position of the implicit decimal, sign,
//    and the length of exponent octets.  If the number is a special value,
//    then a specific format is used for the first octet.
// 4. Normalize the mantissa value by prefixing the implicit 1 and positioning
//    the implicit decimal point after the rightmost 1 bit.  Also adjust the
//    exponent accordingly, that is decrement it for each right shift of the
//    decimal point.  This provides the normalized mantissa and adjusted
//    exponent values.  Note that for denormalized numbers the implicit leading
//    1 is not prefixed to the mantissa.
// 5. Stream out the exponent - bias value.
// 6. Stream out the normalized mantissa value.
//
// Nothing needs to be done for negative numbers.  Only the sign will signify
// that the number under consideration is negative.
//
// The general algorithm for streaming in real data in BER format:
//
// 1. Read the length and the first octet from the stream.
// 2. The combination of the length and the first octet will identify if the
//    value is one of zero, positive infinity, or negative infinity.  Extract
//    the sign, base, scale factor, and the number of exponent octets from the
//    first octet.
// 3. Read the exponent from the stream.  Convert the exponent into base 2.
//    Subtract the scale factor from the exponent.  Note that the scale factor
//    needs to be subtracted as this amount will be accounted for later in the
//    algorithm.
// 4. Calculate the amount the mantissa should be left shifted to convert it
//    into IEEE representation.  This varies depending on whether the number is
//    being streamed in is a normalized/denormalized number.  The exponent of a
//    denormalized will have an extremely low exponent value (less than -1022
//    to be precise).  If a number is denormalized, 'NUM_MANTISSA_BITS - shift'
//    is added to the exponent and the mantissa shifted by the opposite amount.
// 5. The bias value is added to the exponent to get its final value.
// 6. Assemble the double value from the mantissa, exponent, and sign values.

#include <bdem_berutil.h>

#include <bdeimp_prolepticdateutil.h>

#include <bdepu_iso8601.h>

#include <bdes_bitutil.h>

#include <bdesb_fixedmemoutstreambuf.h>

#include <bdet_date.h>
#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdet_datetz.h>
#include <bdet_time.h>
#include <bdet_timetz.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_platformutil.h>

#include <cstring>

namespace BloombergLP {

namespace {

BSLMF_ASSERT(sizeof(int)       == sizeof(float));
BSLMF_ASSERT(sizeof(long long) == sizeof(double));

const bsls_PlatformUtil::Uint64 DOUBLE_EXPONENT_MASK = 0x7ff0000000000000ULL;
const bsls_PlatformUtil::Uint64 DOUBLE_MANTISSA_MASK = 0x000fffffffffffffULL;
const bsls_PlatformUtil::Uint64
                   DOUBLE_MANTISSA_IMPLICIT_ONE_MASK = 0x0010000000000000ULL;
const bsls_PlatformUtil::Uint64 DOUBLE_SIGN_MASK     = 0x8000000000000000ULL;

enum {
    // These constants are used by the implementation of this component.

    TAG_CLASS_MASK  = 0xC0,   // mask for tag class from first octet
    TAG_TYPE_MASK   = 0x20,   // mask for tag type from first octet
    TAG_NUMBER_MASK = 0x1f,   // mask for tag number from first octet

    MAX_TAG_NUMBER_IN_ONE_OCTET        = 30,    // the maximum tag number if
                                                // the tag has one octet

    NUM_VALUE_BITS_IN_TAG_OCTET        = 7,     // number of bits used for the
                                                // tag number in multi-octet
                                                // tags

    LONG_FORM_LENGTH_FLAG_MASK         = 0x80,  // mask that indicates a
                                                // "long-form" length

    LONG_FORM_LENGTH_VALUE_MASK        = 0x7f,  // mask for value from
                                                // "long-form" length

    NUM_BITS_IN_ONE_TAG_OCTET          = 7,

    MAX_TAG_NUMBER_OCTETS              = (sizeof(int) *
                                              bdem_BerUtil_Imp::BITS_PER_OCTET)
                                             / NUM_VALUE_BITS_IN_TAG_OCTET + 1,

    REAL_BINARY_ENCODING               = 0x80,  // value that indicates that
                                                // real encoding is used

    DOUBLE_EXPONENT_SHIFT              = 52,
    DOUBLE_OUTPUT_LENGTH               = 10,
    DOUBLE_EXPONENT_MASK_FOR_TWO_BYTES = 0x7ff,
    DOUBLE_NUM_EXPONENT_BITS           = 11,
    DOUBLE_NUM_MANTISSA_BITS           = 52,
    DOUBLE_NUM_EXPONENT_BYTES          = 2,
    DOUBLE_NUM_MANTISSA_BYTES          = 7,
    DOUBLE_BIAS                        = 1023,

    CHAR_MSB_MASK                      = 0x80,
    INT_MSB_MASK                       = 1 << (sizeof(int) *
                                         bdem_BerUtil_Imp::BITS_PER_OCTET - 1),
    SEVEN_BITS_MASK                    = 0x7f,

    POSITIVE_INFINITY_ID               = 0x40,
    NEGATIVE_INFINITY_ID               = 0x41,
    NAN_ID                             = 0x42,

    DOUBLE_INFINITY_EXPONENT_ID        = 0x7ff,

    INFINITY_MANTISSA_ID               = 0,

    BINARY_POSITIVE_NUMBER_ID          = 0x80,
    BINARY_NEGATIVE_NUMBER_ID          = 0xc0,

    REAL_SIGN_MASK                     = 0x40,
    REAL_BASE_MASK                     = 0x20,
    REAL_SCALE_FACTOR_MASK             = 0x0c,
    REAL_EXPONENT_LENGTH_MASK          = 0x03,

    BER_RESERVED_BASE                  = 3,
    REAL_MULTIPLE_EXPONENT_OCTETS      = 4,

    REAL_BASE_SHIFT                    = 4,
    REAL_SCALE_FACTOR_SHIFT            = 2,

    EPOCH_SERIAL_DATE                  = 737425,  // Serial date for 1/1/2020

    HOURS_PER_DAY                      = 24,
    MINUTES_PER_HOUR                   = 60,
    SECONDS_PER_MINUTE                 = 60,

    MILLISECS_PER_SEC                  = 1000,
    MILLISECS_PER_MIN                 = SECONDS_PER_MINUTE * MILLISECS_PER_SEC,
    MILLISECS_PER_HOUR                 = MINUTES_PER_HOUR * MILLISECS_PER_MIN,
    MILLISECS_PER_DAY                  = 86400000,

    TIMEZONE_LENGTH                    = 2,
    MIN_OFFSET                         = -1439,
    MAX_OFFSET                         = 1439,

    MAX_BINARY_DATE_LENGTH             = 3,
    MAX_BINARY_TIME_LENGTH             = 4,
    MAX_BINARY_DATETIME_LENGTH         = 6,

    MIN_BINARY_DATETZ_LENGTH           = MAX_BINARY_DATE_LENGTH + 1,
    MIN_BINARY_TIMETZ_LENGTH           = MAX_BINARY_TIME_LENGTH + 1,
    MIN_BINARY_DATETIMETZ_LENGTH       = MAX_BINARY_DATETIME_LENGTH + 1,

    MAX_BINARY_DATETZ_LENGTH           = 5,
    MAX_BINARY_TIMETZ_LENGTH           = 6,
    MAX_BINARY_DATETIMETZ_LENGTH       = 9
};

// HELPER FUNCTIONS

void assembleDouble(double    *value,
                    long long  exponent,
                    long long  mantissa,
                    int        sign)
    // Assemble the specified '*value' such that it has the specified
    // 'exponent', specified 'mantissa', and specified 'sign' values.
{
    unsigned long long longLongValue;

    longLongValue  = (unsigned long long) exponent << DOUBLE_EXPONENT_SHIFT;
    longLongValue |= mantissa & DOUBLE_MANTISSA_MASK;

    if (sign) {
        longLongValue |= DOUBLE_SIGN_MASK;
    }

    *value = reinterpret_cast<double&>(longLongValue);
}

inline
void parseDouble(int       *exponent,
                 long long *mantissa,
                 int       *sign,
                 double     value)
    // Parse the specified 'value' and populate the specified
    // 'exponent', specified 'mantissa', and specified 'sign' values from their
    // value of exponent, mantissa, and sign in 'value' respectively.
{
    unsigned long long longLongValue
                                = reinterpret_cast<unsigned long long&>(value);

    *sign     = longLongValue & DOUBLE_SIGN_MASK ? 1 : 0;
    *exponent = (longLongValue & DOUBLE_EXPONENT_MASK) >>DOUBLE_EXPONENT_SHIFT;
    *mantissa = longLongValue & DOUBLE_MANTISSA_MASK;
}

inline
void normalizeMantissaAndAdjustExp(long long *mantissa,
                                   int       *exponent,
                                   bool       denormalized)
    // Normalize the specified '*mantissa' value by prepending the implicit
    // 1 and adjusting the implicit decimal point to after the right most 1
    // bit.  Adjust the '*exponent' value accordingly.  Use the specified
    // 'denormalized' to decide if the value is normalized or not.
{
    if (!denormalized) {
        // If number is not denormalized then need to prefix with implicit
        // one.

        *mantissa |= DOUBLE_MANTISSA_IMPLICIT_ONE_MASK;
    }

    int shift = bdes_BitUtil::find1AtSmallestIndex64(*mantissa);
    *mantissa >>= shift;
    *exponent -= (DOUBLE_NUM_MANTISSA_BITS - shift);

    if (denormalized) {
        *exponent += 1;
    }
}

template <typename TYPE>
inline
int getValueUsingIso8601(bsl::streambuf *streamBuf,
                         TYPE           *value,
                         int             length)
    // Load into the specified 'value' the object in the ISO 8601 format of
    // the specified 'length' reading from the specified 'streamBuf'.  Return
    // 0 on success and a non-zero value otherwise.
{
    enum { FAILURE = -1 };

    if (length <= 0) {
        return FAILURE;                                               // RETURN
    }

    char localBuf[32];         // for common case where length < 32
    bsl::vector<char> vecBuf;  // for length >= 32
    char *buf;

    if (length < 32) {
        buf = localBuf;
    }
    else {
        vecBuf.resize(length);
        buf = &vecBuf[0];  // First byte of contiguous string
    }

    const int bytesConsumed = streamBuf->sgetn(buf, length);
    if (bytesConsumed != length) {
        return FAILURE;                                               // RETURN
    }

    return bdepu_Iso8601::parse(value, buf, length);
}

template <typename TYPE>
inline
int putValueUsingIso8601(bsl::streambuf *streamBuf,
                         const TYPE&     value)
    // Write to the specified 'streamBuf' the length and the value of the
    // specified 'value' in the ISO 8601 format.  Return 0 on success and a
    // non-zero value otherwise.
{
    char buf[bdepu_Iso8601::BDEPU_MAX_DATETIME_STRLEN];
    int len = bdepu_Iso8601::generate(buf, value, sizeof(buf));
    return bdem_BerUtil_Imp::putStringValue(streamBuf, buf, len);
}

void getTimezoneOffset(bsl::streambuf *streamBuf, short *offset)
    // Read from the specified 'streamBuf' and load into the specified
    // 'offset' the value of the time zone offset.
{
    const int firstOctet  = streamBuf->sbumpc();
    const int secondOctet = streamBuf->sbumpc();

    *offset = (short) ((firstOctet << 8) | secondOctet);
}

void putTimezoneOffset(bsl::streambuf *streamBuf, short offset)
    // Write to the specified 'streamBuf' the value of the specified time zone
    // 'offset'.  The behavior is undefined unless
    // 'MIN_OFFSET <= offset <= MAX_OFFSET'.
{
    BSLS_ASSERT(MIN_OFFSET <= offset);
    BSLS_ASSERT(offset <= MAX_OFFSET);

    streamBuf->sputc((char )((offset & 0xFF00) >> 8));
    streamBuf->sputc((char) (offset & 0xFF));
}

void putChars(bsl::streambuf *streamBuf, char value, int numChars)
    // Write to the specified 'streamBuf' the specified 'numChars' characters
    // having the specified 'value'.  The behavior is undefined unless
    // '0 <= numChars'.
{
//     BSLS_ASSERT(0 == padChar || ((char) -1) == padChar);
    BSLS_ASSERT(0 <= numChars);

    char buffer[MIN_BINARY_DATETIMETZ_LENGTH];
    bsl::memset(buffer, value, numChars);

#if BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
    streamBuf->sputn(buffer, numChars);
#else
    for (int i = 0; i < numChars; ++i) {
        streamBuf->sputc(value);
    }
#endif
}

bsls_Types::Int64 getSerialDateValue(const bdet_Date& value)
    // Return the binary proleptic serial value, in number of days', of the
    // specified date 'value' from the predefined epoch date.  Note that the
    // serial value could be negative if 'value' occurs before the predefined
    // epoch date.
{
    const int serialDate = bdeimp_ProlepticDateUtil::ymd2serial(value.year(),
                                                                value.month(),
                                                                value.day());

    const bsls_Types::Int64 dateOffset = serialDate - EPOCH_SERIAL_DATE;

    return dateOffset;
}

bsls_Types::Int64 getSerialTimeValue(const bdet_Time& value)
    // Return the binary serial value, in number of milli seconds, of the
    // specified time 'value' from midnight.
{
    const bdet_Time defaultTime;
    return (value - defaultTime).totalMilliseconds();
}

bsls_Types::Int64 getSerialDatetimeValue(const bdet_Datetime& value)
    // Return the binary serial value, in number of milli seconds, of the
    // specified datetime 'value' from midnight of the predefined epoch date
    // value.  Note that the serial value could be negative if the date
    // corresponding to 'value' occurs before the predefined epoch date.
{
    const bsls_Types::Int64 serialDate = getSerialDateValue(value.date());
    const bsls_Types::Int64 serialTime = getSerialTimeValue(value.time());
    const bsls_Types::Int64 serialDatetime =
                                   serialDate * MILLISECS_PER_DAY + serialTime;

    return serialDatetime;
}

struct BinaryDateTimeFormat {
    // This 'struct' provides a function that specifies the maximum length
    // required to encode an object in the binary BER (octet string) format.

    template <typename TYPE>
    static int maxLength();
        // Return the maximum length, in bytes, required to encode an object
        // of the templated 'TYPE' in the binary ber format.
};

template <>
int BinaryDateTimeFormat::maxLength<bdet_Date>()
{
    return MAX_BINARY_DATE_LENGTH;
}

template <>
int BinaryDateTimeFormat::maxLength<bdet_Time>()
{
    return MAX_BINARY_TIME_LENGTH;
}

template <>
int BinaryDateTimeFormat::maxLength<bdet_Datetime>()
{
    return MAX_BINARY_DATETIME_LENGTH;
}

template <>
int BinaryDateTimeFormat::maxLength<bdet_DateTz>()
{
    return MAX_BINARY_DATETZ_LENGTH;
}

template <>
int BinaryDateTimeFormat::maxLength<bdet_TimeTz>()
{
    return MAX_BINARY_TIMETZ_LENGTH;
}

template <>
int BinaryDateTimeFormat::maxLength<bdet_DatetimeTz>()
{
    return MAX_BINARY_DATETIMETZ_LENGTH;
}

struct StringDateTimeFormat {
    // This 'struct' provides a function that specifies the maximum length
    // required to encode an object in the ISO 8601 BER (visible string)
    // format.

    template <typename TYPE>
    static int maxLength();
        // Return the maximum length, in bytes, required to encode an object
        // of the templated 'TYPE' in the ISO 8601 format.
};

template <>
int StringDateTimeFormat::maxLength<bdet_Date>()
{
    return bdepu_Iso8601::BDEPU_DATE_STRLEN;
}

template <>
int StringDateTimeFormat::maxLength<bdet_Time>()
{
    return bdepu_Iso8601::BDEPU_TIME_STRLEN;
}

template <>
int StringDateTimeFormat::maxLength<bdet_Datetime>()
{
    return bdepu_Iso8601::BDEPU_DATETIME_STRLEN;
}

template <>
int StringDateTimeFormat::maxLength<bdet_DateTz>()
{
    return bdepu_Iso8601::BDEPU_DATETZ_STRLEN;
}

template <>
int StringDateTimeFormat::maxLength<bdet_TimeTz>()
{
    return bdepu_Iso8601::BDEPU_TIMETZ_STRLEN;
}

template <>
int StringDateTimeFormat::maxLength<bdet_DatetimeTz>()
{
    return bdepu_Iso8601::BDEPU_DATETIMETZ_STRLEN;
}

}  // close anonymous namespace

                            // -------------------
                            // struct bdem_BerUtil
                            // -------------------

int bdem_BerUtil::getIdentifierOctets(
                            bsl::streambuf              *streamBuf,
                            bdem_BerConstants::TagClass *tagClass,
                            bdem_BerConstants::TagType  *tagType,
                            int                         *tagNumber,
                            int                         *accumNumBytesConsumed)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    int nextOctet = streamBuf->sbumpc();

    if (bsl::streambuf::traits_type::eof() == nextOctet) {
        return FAILURE;                                               // RETURN
    }

    ++*accumNumBytesConsumed;

    *tagClass = static_cast<bdem_BerConstants::TagClass>
                                                  (nextOctet & TAG_CLASS_MASK);

    *tagType = static_cast<bdem_BerConstants::TagType>
                                                   (nextOctet & TAG_TYPE_MASK);

    if (TAG_NUMBER_MASK != (nextOctet & TAG_NUMBER_MASK)) {
        // The tag number fits in a single octet.

        *tagNumber = nextOctet & TAG_NUMBER_MASK;
        return SUCCESS;                                               // RETURN
    }

    *tagNumber = 0;

    for (int i = 0; i < MAX_TAG_NUMBER_OCTETS; ++i) {
        nextOctet = streamBuf->sbumpc();
        if (bsl::streambuf::traits_type::eof() == nextOctet) {
            return FAILURE;                                           // RETURN
        }

        ++*accumNumBytesConsumed;

        *tagNumber <<= NUM_VALUE_BITS_IN_TAG_OCTET;
        *tagNumber  |= nextOctet & SEVEN_BITS_MASK;

        if (!(nextOctet & CHAR_MSB_MASK)) {
            return SUCCESS;                                           // RETURN
        }
    }

    return FAILURE;
}

int bdem_BerUtil::putIdentifierOctets(bsl::streambuf              *streamBuf,
                                      bdem_BerConstants::TagClass  tagClass,
                                      bdem_BerConstants::TagType   tagType,
                                      int                          tagNumber)
    // Write the specified 'tag' to the specified 'streamBuf'.
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (tagNumber < 0) {
        return FAILURE;                                               // RETURN
    }

    if (tagNumber <= MAX_TAG_NUMBER_IN_ONE_OCTET) {
        unsigned char octet = static_cast<unsigned char>(tagClass
                                                         | tagType
                                                         | tagNumber);

        return octet == streamBuf->sputc(octet) ? SUCCESS
                                                : FAILURE;            // RETURN
    }

    // Send multiple identifier octets.

    unsigned char firstOctet = static_cast<unsigned char>(tagClass
                                                          | tagType
                                                          | TAG_NUMBER_MASK);

    if (firstOctet != streamBuf->sputc(firstOctet)) {
        return FAILURE;                                               // RETURN
    }

    // Find the number of octets required.

    int numOctetsRequired = 0;

    {
        enum {
            INT_NUM_BITS = sizeof(int) * bdem_BerUtil_Imp::BITS_PER_OCTET
        };

        int          shift = 0;
        unsigned int mask  = SEVEN_BITS_MASK;

        for (int i = 0; INT_NUM_BITS > shift; ++i) {
            if (tagNumber & mask) {
                numOctetsRequired = i + 1;
            }

            shift += NUM_VALUE_BITS_IN_TAG_OCTET;
            mask <<= NUM_VALUE_BITS_IN_TAG_OCTET;
        }
    }

    BSLS_ASSERT(numOctetsRequired <= MAX_TAG_NUMBER_OCTETS);

    // Put all octets except the last one.

    int          shift = (numOctetsRequired - 1) * NUM_VALUE_BITS_IN_TAG_OCTET;
    unsigned int mask  = SEVEN_BITS_MASK << shift;

    for (int i = 0; i < numOctetsRequired - 1; ++i) {
        unsigned char nextOctet = (mask & tagNumber) >> shift | CHAR_MSB_MASK;

        if (nextOctet != streamBuf->sputc(nextOctet)) {
            return FAILURE;                                           // RETURN
        }

        shift -= NUM_VALUE_BITS_IN_TAG_OCTET;
        mask   = SEVEN_BITS_MASK << shift;
    }

    // Put the final octet.

    tagNumber &= SEVEN_BITS_MASK;

    return tagNumber == streamBuf->sputc(tagNumber) ? SUCCESS
                                                    : FAILURE;
}

                          // -----------------------
                          // struct bdem_BerUtil_Imp
                          // -----------------------

int bdem_BerUtil_Imp::getBinaryDateValue(bsl::streambuf *streamBuf,
                                         bdet_Date      *value,
                                         int             length)
{
    bsls_Types::Int64 serialDate;
    getIntegerValue(streamBuf, &serialDate, length);

    int year, month, day;
    bdeimp_ProlepticDateUtil::serial2ymd(&year,
                                         &month,
                                         &day,
                                         serialDate + EPOCH_SERIAL_DATE);

    return value->setYearMonthDayIfValid(year, month, day);
}

int bdem_BerUtil_Imp::getBinaryTimeValue(bsl::streambuf *streamBuf,
                                         bdet_Time      *value,
                                         int             length)
{
    bsls_Types::Int64 serialTime;
    getIntegerValue(streamBuf, &serialTime, length);

    const int hour   = serialTime / MILLISECS_PER_HOUR;
    const int minute = (serialTime - hour * MILLISECS_PER_HOUR)
                                                           / MILLISECS_PER_MIN;
    const int second = (serialTime
                      - hour * MILLISECS_PER_HOUR
                      - minute * MILLISECS_PER_MIN) / MILLISECS_PER_SEC;
    const int millisec = serialTime
                       - hour * MILLISECS_PER_HOUR
                       - minute * MILLISECS_PER_MIN
                       - second * MILLISECS_PER_SEC;

    return value->setTimeIfValid(hour, minute, second, millisec);
}

int bdem_BerUtil_Imp::getBinaryDatetimeValue(bsl::streambuf *streamBuf,
                                             bdet_Datetime  *value,
                                             int             length)
{
    if (length > MIN_BINARY_DATETIMETZ_LENGTH) {
        short offset;
        getTimezoneOffset(streamBuf, &offset);

        length -= TIMEZONE_LENGTH;
    }

    bsls_Types::Int64 serialDatetime;
    getIntegerValue(streamBuf, &serialDatetime, length);

    bsls_Types::Int64 serialDate = serialDatetime / MILLISECS_PER_DAY;

    if (serialDatetime < 0) {
        --serialDate;
        if (EPOCH_SERIAL_DATE == -serialDate) {
            ++serialDate;
        }
    }

    const bsls_Types::Int64 serialTime = serialDatetime
                                       - serialDate * MILLISECS_PER_DAY;

    int hour   = serialTime / MILLISECS_PER_HOUR;

    const int minute = (serialTime - hour * MILLISECS_PER_HOUR)
                                                           / MILLISECS_PER_MIN;
    const int second = (serialTime
                      - hour * MILLISECS_PER_HOUR
                      - minute * MILLISECS_PER_MIN) / MILLISECS_PER_SEC;
    const int millisec = serialTime
                       - hour * MILLISECS_PER_HOUR
                       - minute * MILLISECS_PER_MIN
                       - second * MILLISECS_PER_SEC;

    if (24 == hour) {
        hour = 0;
        ++serialDate;
    }

    int year, month, day;
    bdeimp_ProlepticDateUtil::serial2ymd(&year,
                                         &month,
                                         &day,
                                         serialDate + EPOCH_SERIAL_DATE);

    return value->setDatetimeIfValid(year,
                                     month,
                                     day,
                                     hour,
                                     minute,
                                     second,
                                     millisec);
}

int bdem_BerUtil_Imp::getBinaryDateTzValue(bsl::streambuf *streamBuf,
                                           bdet_DateTz    *value,
                                           int             length)
{
    short offset = 0;
    if (length >= MIN_BINARY_DATETZ_LENGTH) {
        getTimezoneOffset(streamBuf, &offset);

        if (offset < MIN_OFFSET || offset > MAX_OFFSET) {
            *value = bdet_DateTz();
            return -1;                                                // RETURN
        }

        length -= TIMEZONE_LENGTH;
    }

    bdet_Date localDate;
    getBinaryDateValue(streamBuf, &localDate, length);

    return value->validateAndSetDateTz(localDate, offset);
}

int bdem_BerUtil_Imp::getBinaryTimeTzValue(bsl::streambuf *streamBuf,
                                           bdet_TimeTz    *value,
                                           int             length)
{
    short offset = 0;
    if (length >= MIN_BINARY_TIMETZ_LENGTH) {
        getTimezoneOffset(streamBuf, &offset);

        if (offset < MIN_OFFSET || offset > MAX_OFFSET) {
            *value = bdet_TimeTz();
            return -1;                                                // RETURN
        }

        length -= TIMEZONE_LENGTH;
    }

    bdet_Time localTime;
    getBinaryTimeValue(streamBuf, &localTime, length);

    return value->validateAndSetTimeTz(localTime, offset);
}

int bdem_BerUtil_Imp::getBinaryDatetimeTzValue(bsl::streambuf  *streamBuf,
                                               bdet_DatetimeTz *value,
                                               int              length)
{
    short offset = 0;
    if (length >= MIN_BINARY_DATETIMETZ_LENGTH) {
        getTimezoneOffset(streamBuf, &offset);

        if (offset < MIN_OFFSET || offset > MAX_OFFSET) {
            *value = bdet_DatetimeTz();
            return -1;                                                // RETURN
        }

        length -= TIMEZONE_LENGTH;
    }

    bdet_Datetime localDatetime;
    getBinaryDatetimeValue(streamBuf, &localDatetime, length);
    return value->validateAndSetDatetimeTz(localDatetime, offset);
}

int bdem_BerUtil_Imp::putBinaryDateValue(bsl::streambuf   *streamBuf,
                                         const bdet_Date&  value)
{
    const bsls_Types::Int64 serialDate = getSerialDateValue(value);
    const int               length     = numBytesToStream(serialDate);

    BSLS_ASSERT(length <= MAX_BINARY_DATE_LENGTH);

    putLength(streamBuf, length);
    return putIntegerGivenLength(streamBuf, serialDate, length);
}

int bdem_BerUtil_Imp::putBinaryTimeValue(bsl::streambuf   *streamBuf,
                                         const bdet_Time&  value)
{
    const bsls_Types::Int64 serialTime = getSerialTimeValue(value);
    const int               length     = numBytesToStream(serialTime);

    BSLS_ASSERT(length <= MAX_BINARY_TIME_LENGTH);

    putLength(streamBuf, length);
    return putIntegerGivenLength(streamBuf, serialTime, length);
}

int bdem_BerUtil_Imp::putBinaryDatetimeValue(bsl::streambuf       *streamBuf,
                                             const bdet_Datetime&  value)
{
    const bsls_Types::Int64 serialDatetime = getSerialDatetimeValue(value);
    int                     length         = numBytesToStream(serialDatetime);

    if (length >= MIN_BINARY_DATETIMETZ_LENGTH) {
        putLength(streamBuf, length + TIMEZONE_LENGTH);
        putTimezoneOffset(streamBuf, 0);
    }
    else {
        putLength(streamBuf, length);
    }
    return putIntegerGivenLength(streamBuf, serialDatetime, length);
}

int bdem_BerUtil_Imp::putBinaryDateTzValue(bsl::streambuf     *streamBuf,
                                           const bdet_DateTz&  value)
{
    const bdet_Date& date   = value.localDate();
    short            offset = value.offset();

    if (!offset) {
        return putBinaryDateValue(streamBuf, date);                   // RETURN
    }

    const bsls_Types::Int64 serialDate = getSerialDateValue(date);
    int                     length     = numBytesToStream(serialDate)
                                       + TIMEZONE_LENGTH;

    if (length < MIN_BINARY_DATETZ_LENGTH) {
        const char padChar      = serialDate < 0 ? 0xFF : 0;
        const int  numPadOctets = MIN_BINARY_DATETZ_LENGTH - length;

        putLength(streamBuf, MIN_BINARY_DATETZ_LENGTH);
        putTimezoneOffset(streamBuf, offset);
        putChars(streamBuf, padChar, numPadOctets);
    }
    else {
        putLength(streamBuf, length);
        putTimezoneOffset(streamBuf, offset);
    }
    return putIntegerGivenLength(streamBuf,
                                 serialDate,
                                 length - TIMEZONE_LENGTH);
}

int bdem_BerUtil_Imp::putBinaryTimeTzValue(bsl::streambuf     *streamBuf,
                                           const bdet_TimeTz&  value)
{
    const bdet_Time& time   = value.localTime();
    short            offset = value.offset();

    if (!offset) {
        return putBinaryTimeValue(streamBuf, time);                   // RETURN
    }

    const bsls_Types::Int64 serialTime = getSerialTimeValue(time);
    const int               length     = numBytesToStream(serialTime)
                                       + TIMEZONE_LENGTH;

    if (length < MIN_BINARY_TIMETZ_LENGTH) {
        const int  numPadOctets = MIN_BINARY_TIMETZ_LENGTH - length;

        putLength(streamBuf, MIN_BINARY_TIMETZ_LENGTH);
        putTimezoneOffset(streamBuf, offset);
        putChars(streamBuf, 0, numPadOctets);
    }
    else {
        putLength(streamBuf, length);
        putTimezoneOffset(streamBuf, offset);
    }
    return putIntegerGivenLength(streamBuf,
                                 serialTime,
                                 length - TIMEZONE_LENGTH);
}

int bdem_BerUtil_Imp::putBinaryDatetimeTzValue(
                                             bsl::streambuf         *streamBuf,
                                             const bdet_DatetimeTz&  value)
{
    const bdet_Datetime& datetime = value.localDatetime();
    short                offset   = value.offset();

    if (!offset) {
        return putBinaryDatetimeValue(streamBuf, datetime);           // RETURN
    }

    const bsls_Types::Int64 serialDatetime = getSerialDatetimeValue(datetime);
    const int               length         = numBytesToStream(serialDatetime)
                                           + TIMEZONE_LENGTH;

    if (length < MIN_BINARY_DATETIMETZ_LENGTH) {
        const char padChar      = serialDatetime < 0 ? 0xFF : 0;
        const int  numPadOctets = MIN_BINARY_DATETIMETZ_LENGTH - length;

        putLength(streamBuf, MIN_BINARY_DATETIMETZ_LENGTH);
        putTimezoneOffset(streamBuf, offset);
        putChars(streamBuf, padChar, numPadOctets);
    }
    else {
        putLength(streamBuf, length);
        putTimezoneOffset(streamBuf, offset);
    }
    return putIntegerGivenLength(streamBuf,
                                 serialDatetime,
                                 length - TIMEZONE_LENGTH);
}

int bdem_BerUtil_Imp::getDoubleValue(bsl::streambuf *stream,
                                     double         *value,
                                     int             length)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (0 == length) {
        *value = 0;
        return SUCCESS;                                               // RETURN
    }

    int nextOctet = stream->sbumpc();

    if (POSITIVE_INFINITY_ID == nextOctet) {
        assembleDouble(value,
                       DOUBLE_INFINITY_EXPONENT_ID,
                       INFINITY_MANTISSA_ID,
                       0);
        return SUCCESS;                                               // RETURN
    }
    else if (NEGATIVE_INFINITY_ID == nextOctet) {
        assembleDouble(value,
                       DOUBLE_INFINITY_EXPONENT_ID,
                       INFINITY_MANTISSA_ID,
                       1);
        return SUCCESS;                                               // RETURN
    }
    else if (NAN_ID == nextOctet) {
        assembleDouble(value,
                       DOUBLE_INFINITY_EXPONENT_ID,
                       1,
                       0);
        return SUCCESS;                                               // RETURN

    }

    if (!(nextOctet & (unsigned  char) REAL_BINARY_ENCODING)) {
        // Encoding is decimal, return as that is not handled currently.

        return FAILURE;                                               // RETURN
    }

    int sign = nextOctet & REAL_SIGN_MASK ? 1 : 0;
    int base = (nextOctet & REAL_BASE_MASK) >> REAL_BASE_SHIFT;
    if (BER_RESERVED_BASE == base) {
        // Base value is not supported.

        return FAILURE;                                               // RETURN
    }

    base *= 8;

    int scaleFactor = (nextOctet & REAL_SCALE_FACTOR_MASK)
                                                    >> REAL_SCALE_FACTOR_SHIFT;
    int expLength   = (nextOctet & REAL_EXPONENT_LENGTH_MASK) + 1;

    if (REAL_MULTIPLE_EXPONENT_OCTETS == expLength) {
        // Exponent length is encoded in the following octet.

        expLength = stream->sbumpc();

        if (bsl::streambuf::traits_type::eof() == expLength
         || (unsigned) expLength > sizeof(long long)) {
            // Exponent values that take greater than sizeof(long long) octets
            // are not handled by this implementation.

            return FAILURE;                                           // RETURN
        }
    }

    long long exponent;
    if (getIntegerValue(stream, &exponent, expLength)) {
        return FAILURE;                                               // RETURN
    }

    if (0 != base) {
        // Convert exponent to base 2.

        exponent *= 8 == base ? 3 : 4;
    }
    exponent -= scaleFactor;

    long long mantissa = 0;
    int       mantissaLength = length - expLength - 1;
    if (getIntegerValue(stream, &mantissa, mantissaLength)) {
        return FAILURE;                                               // RETURN
    }

    int shift = bdes_BitUtil::find1AtLargestIndex64(mantissa);
    if (FAILURE == shift) {
        return FAILURE;                                               // RETURN
    }
    shift = 63 - shift;

    // Subtract the number of exponent bits and the sign bit.

    shift            -= DOUBLE_NUM_EXPONENT_BITS + 1;
    exponent         += DOUBLE_BIAS + DOUBLE_NUM_MANTISSA_BITS - shift - 1;

    if (exponent > 0) { // Normal number
        // Shift the mantissa left by shift amount, account for the implicit
        // one, and then removing it.

        mantissa <<= shift + 1;
        mantissa &= ~DOUBLE_MANTISSA_IMPLICIT_ONE_MASK;
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

int bdem_BerUtil_Imp::getIntegerValue(bsl::streambuf *streamBuf,
                                      long long      *value,
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

    if ((unsigned) length > sizeof(long long)) {
        // Overflow.

        return FAILURE;                                               // RETURN
    }

    int sign = (streamBuf->sgetc() & SIGN_BIT_MASK) ? -1 : 0;
    unsigned int valueLo = sign;
    unsigned int valueHi = sign;

    // Decode high-order word.

    for ( ; length > (int) sizeof(int); --length) {
        int nextOctet = streamBuf->sbumpc();
        if (bsl::streambuf::traits_type::eof() == nextOctet) {
            return FAILURE;                                           // RETURN
        }

        valueHi <<= BITS_PER_OCTET;
        valueHi |= (unsigned char) nextOctet;
    }

    // Decode low-order word.

    for ( ; length > 0; --length) {
        int nextOctet = streamBuf->sbumpc();
        if (bsl::streambuf::traits_type::eof() == nextOctet) {
            return FAILURE;                                           // RETURN
        }

        valueLo <<= BITS_PER_OCTET;
        valueLo |= (unsigned char) nextOctet;
    }

    // Combine low and high word into a long word.
#ifdef BSLS_PLATFORMUTIL__IS_BIG_ENDIAN
    reinterpret_cast<unsigned int*>(value)[1] = valueLo;
    reinterpret_cast<unsigned int*>(value)[0] = valueHi;
#else
    reinterpret_cast<unsigned int*>(value)[0] = valueLo;
    reinterpret_cast<unsigned int*>(value)[1] = valueHi;
#endif
    return SUCCESS;
}

int bdem_BerUtil_Imp::getLength(bsl::streambuf *streamBuf,
                                int            *result,
                                int            *accumNumBytesConsumed)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    int nextOctet = streamBuf->sbumpc();
    if (bsl::streambuf::traits_type::eof() == nextOctet) {
        return FAILURE;                                               // RETURN
    }

    ++*accumNumBytesConsumed;

    if (nextOctet == bdem_BerUtil_Imp::INDEFINITE_LENGTH_OCTET) {
        *result = bdem_BerUtil_Imp::INDEFINITE_LENGTH;

        return SUCCESS;                                               // RETURN
    }

    unsigned int numOctets = (unsigned int) nextOctet;

    // Length has been transmitted in short form.

    if (!(numOctets & LONG_FORM_LENGTH_FLAG_MASK)) {
        *result = numOctets;
        return SUCCESS;                                               // RETURN
    }

    // Length has been transmitted in long form.

    numOctets &= LONG_FORM_LENGTH_VALUE_MASK;

    if (numOctets > sizeof(int)) {
        return FAILURE;                                               // RETURN
    }

    *result = 0;
    for (unsigned int i = 0; i < numOctets; ++i) {
        nextOctet = streamBuf->sbumpc();
        if (bsl::streambuf::traits_type::eof() == nextOctet) {
            return FAILURE;                                           // RETURN
        }

        *result <<= bdem_BerUtil_Imp::BITS_PER_OCTET;
        *result |=  nextOctet;
    }

    *accumNumBytesConsumed += numOctets;

    return SUCCESS;
}

int bdem_BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               bsl::string    *value,
                               int             length)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (0 == length) {
        return SUCCESS;                                               // RETURN
    }
    else if (length < 0) {
        return FAILURE;                                               // RETURN
    }

    value->resize(length);
    // KLUDGE: The standard does not guarantee that the contents of a string
    // are contiguous in memory.  For efficiency, we take advantage of the
    // fact that our implementation (and almost every other implementation) is
    // contiguous.  We assert this assumption here:

    BSLS_ASSERT(&value[length-1] == &value[0] + length - 1);

    const int bytesConsumed = streamBuf->sgetn(&(*value)[0], length);

    return length == bytesConsumed ? SUCCESS : FAILURE;
}

int bdem_BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               bdet_Date      *value,
                               int             length)
{
    return length > MAX_BINARY_DATE_LENGTH
         ? getValueUsingIso8601(streamBuf, value, length)
         : getBinaryDateValue(streamBuf, value, length);
}

int bdem_BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               bdet_Datetime  *value,
                               int             length)
{
    return length > MAX_BINARY_DATETIMETZ_LENGTH
         ? getValueUsingIso8601(streamBuf, value, length)
         : getBinaryDatetimeValue(streamBuf, value, length);
}

int bdem_BerUtil_Imp::getValue(bsl::streambuf  *streamBuf,
                               bdet_DatetimeTz *value,
                               int              length)
{
    return length > MAX_BINARY_DATETIMETZ_LENGTH
         ? getValueUsingIso8601(streamBuf, value, length)
         : getBinaryDatetimeTzValue(streamBuf, value, length);
}

int bdem_BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               bdet_DateTz    *value,
                               int             length)
{
    return length > MAX_BINARY_DATETZ_LENGTH
         ? getValueUsingIso8601(streamBuf, value, length)
         : getBinaryDateTzValue(streamBuf, value, length);
}

int bdem_BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               bdet_Time      *value,
                               int             length)
{
    return length > MAX_BINARY_TIME_LENGTH
         ? getValueUsingIso8601(streamBuf, value, length)
         : getBinaryTimeValue(streamBuf, value, length);
}

int bdem_BerUtil_Imp::getValue(bsl::streambuf *streamBuf,
                               bdet_TimeTz    *value,
                               int             length)
{
    return length > MAX_BINARY_TIMETZ_LENGTH
         ? getValueUsingIso8601(streamBuf, value, length)
         : getBinaryTimeTzValue(streamBuf, value, length);
}

template <typename TYPE, typename TYPETZ>
int bdem_BerUtil_Imp::getValue(bsl::streambuf               *streamBuf,
                               bdeut_Variant2<TYPE, TYPETZ> *value,
                               int                           length)
{
    BSLMF_ASSERT((bslmf_IsSame<bdet_Date, TYPE>::VALUE
               && bslmf_IsSame<bdet_DateTz, TYPETZ>::VALUE)
              || (bslmf_IsSame<bdet_Time, TYPE>::VALUE
               && bslmf_IsSame<bdet_TimeTz, TYPETZ>::VALUE)
              || (bslmf_IsSame<bdet_Datetime, TYPE>::VALUE
               && bslmf_IsSame<bdet_DatetimeTz, TYPETZ>::VALUE));

    const int MAX_BINARY_TYPETZ_LENGTH =
                                     BinaryDateTimeFormat::maxLength<TYPETZ>();
    const int MAX_BINARY_TYPE_LENGTH = BinaryDateTimeFormat::maxLength<TYPE>();
    const int MAX_STRING_TYPE_LENGTH = StringDateTimeFormat::maxLength<TYPE>();

    int rc;
    if (length > MAX_STRING_TYPE_LENGTH
     || (length > MAX_BINARY_TYPE_LENGTH
      && length <= MAX_BINARY_TYPETZ_LENGTH)) {
        TYPETZ object;
        rc = getValue(streamBuf, &object, length);
        if (!rc) {
            *value = object;
        }
    }
    else {
        TYPE object;
        rc = getValue(streamBuf, &object, length);
        if (!rc) {
            *value = object;
        }
    }
    return rc;
}

int bdem_BerUtil_Imp::numBytesToStream(short value)
{
    // This overload of 'numBytesToStream' is optimized for a 16-bit 'value'.

    if (0 == value) {
        return 1;                                                     // RETURN
    }

    int numBits;
    if (value > 0) {
        // For positive values, all but one 0 bits on the left are redundant.
        // - 'find1AtLargestIndex' returns 0 .. 14 (since bit 15 is 0).
        // - Add 1 to convert from an index to a count in range 1 .. 15.
        // - Add 1 to preserve the sign bit, for a value in range 2 .. 16.

        numBits = bdes_BitUtil::find1AtLargestIndex(value) + 2;
    }
    else {
        // For negative values, all but one 1 bits on the left are redundant.
        // - 'find0AtLargestIndex' returns 0 .. 14 (since bit 15 is 1).
        // - Add 1 to convert from an index to a count in range 1 .. 15.
        // - Add 1 to preserve the sign bit, for a value in range 2 .. 16.

        numBits = bdes_BitUtil::find0AtLargestIndex(value) + 2;
    }

    // Round up to correct number of bytes:

    return (numBits + BITS_PER_OCTET - 1) / BITS_PER_OCTET;
}

int bdem_BerUtil_Imp::numBytesToStream(int value)
{
    // This overload of 'numBytesToStream' is optimized for a 32-bit 'value'.

    if (0 == value) {
        return 1;                                                     // RETURN
    }

    int numBits;
    if (value > 0) {
        // For positive values, all but one 0 bits on the left are redundant.
        // - 'find1AtLargestIndex' returns 0 .. 30 (since bit 31 is 0).
        // - Add 1 to convert from an index to a count in range 1 .. 31.
        // - Add 1 to preserve the sign bit, for a value in range 2 .. 32.

        numBits = bdes_BitUtil::find1AtLargestIndex(value) + 2;
    }
    else {
        // For negative values, all but one 1 bits on the left are redundant.
        // - 'find0AtLargestIndex' returns 0 .. 30 (since bit 31 is 1).
        // - Add 1 to convert from an index to a count in range 1 .. 31.
        // - Add 1 to preserve the sign bit, for a value in range 2 .. 32.

        numBits = bdes_BitUtil::find0AtLargestIndex(value) + 2;
    }

    // Round up to correct number of bytes:

    return (numBits + BITS_PER_OCTET - 1) / BITS_PER_OCTET;
}

int bdem_BerUtil_Imp::numBytesToStream(long long value)
{
    // This overload of 'numBytesToStream' is optimized for a 64-bit 'value'.

    if (0 == value) {
        return 1;                                                     // RETURN
    }

    int numBits;
    if (value > 0) {
        // For positive values, all but one 0 bits on the left are redundant.
        // - 'find1AtLargestIndex64' returns 0 .. 62 (since bit 63 is 0).
        // - Add 1 to convert from an index to a count in range 1 .. 63.
        // - Add 1 to preserve the sign bit, for a value in range 2 .. 64.

        numBits = bdes_BitUtil::find1AtLargestIndex64(value) + 2;
    }
    else {
        // For negative values, all but one 1 bits on the left are redundant.
        // - 'find1AtLargestIndex64' returns 0 .. 62 (since bit 63 is 0).
        // - Add 1 to convert from an index to a count in range 1 .. 63.
        // - Add 1 to preserve the sign bit, for a value in range 2 .. 64.

        numBits = bdes_BitUtil::find0AtLargestIndex64(value) + 2;
    }

    // Round up to correct number of bytes:

    return (numBits + BITS_PER_OCTET - 1) / BITS_PER_OCTET;
}

int bdem_BerUtil_Imp::putDoubleValue(bsl::streambuf *stream, double value)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    // If 0 == value, put out length = 0 and return.

    if (0.0 == value) {
        return 0 == stream->sputc(0) ? SUCCESS : FAILURE;             // RETURN
    }

    // Else parse double value.

    int       exponent, sign;
    long long mantissa;

    parseDouble(&exponent, &mantissa, &sign, value);

    // Check for special cases +/- infinity and NaN.

    if (DOUBLE_INFINITY_EXPONENT_ID == exponent) {
        if (INFINITY_MANTISSA_ID == mantissa) {
            char signOctet = sign
                           ? NEGATIVE_INFINITY_ID
                           : POSITIVE_INFINITY_ID;

            return (1         == stream->sputc(1)
                && signOctet == stream->sputc(signOctet))
                 ? SUCCESS
                 : FAILURE;                                           // RETURN
        }
        else {
            // For NaN use bit pattern 0x42.

            char NaN = NAN_ID;
            return (1        == stream->sputc(1)
                && NaN       == stream->sputc(NaN))
                ? SUCCESS
                : FAILURE;                                            // RETURN
        }
    }

    bool denormalized = 0 == exponent ? true : false;

    // Normalize the mantissa, get its actual value and adjust the exponent
    // accordingly.

    normalizeMantissaAndAdjustExp(&mantissa, &exponent, denormalized);

    exponent -= DOUBLE_BIAS;

    int expLength = numBytesToStream(exponent);
    int manLength = numBytesToStream(mantissa);

    // Put out the length = expLength + manLength + 1.

    char totalLength = expLength + manLength + 1;
    if (totalLength != stream->sputc(totalLength)) {
        return FAILURE;                                               // RETURN
    }

    unsigned char firstOctet = sign
                             ? BINARY_NEGATIVE_NUMBER_ID
                             : BINARY_POSITIVE_NUMBER_ID;

    if (2 == expLength) {
        // Two exponent octets will be sent out.

        firstOctet |= 1;
    }

    if (firstOctet != stream->sputc(firstOctet)) {
        return FAILURE;                                               // RETURN
    }

    // Put out the exponent and mantissa.

    return putIntegerGivenLength(stream, exponent, expLength)
        || putIntegerGivenLength(stream, mantissa, manLength)
         ? FAILURE : SUCCESS;
}

int bdem_BerUtil_Imp::putLength(bsl::streambuf *streamBuf, int length)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (length < 0) {
        return FAILURE;                                               // RETURN
    }

    if (length <= 127) {
        return length == streamBuf->sputc(length) ? SUCCESS
                                                  : FAILURE;          // RETURN
    }

    // length > 127.

    int numOctets = sizeof(int);
    for (unsigned int mask = ~((unsigned int) -1 >> BITS_PER_OCTET);
         !(length & mask);
         mask >>= BITS_PER_OCTET) {
        --numOctets;
    }

    unsigned char lengthOctet = numOctets | LONG_FORM_LENGTH_FLAG_MASK;
    if (lengthOctet != streamBuf->sputc(lengthOctet)) {
        return FAILURE;                                               // RETURN
    }

    return putIntegerGivenLength(streamBuf, length, numOctets);
}

int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               const bdet_Date&              value,
                               const bdem_BerEncoderOptions *options)
{
    return options && options->encodeDateAndTimeTypesAsBinary()
         ? putBinaryDateValue(streamBuf, value)
         : putValueUsingIso8601(streamBuf, value);
}

int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               const bdet_Datetime&          value,
                               const bdem_BerEncoderOptions *options)
{
    return options && options->encodeDateAndTimeTypesAsBinary()
         ? putBinaryDatetimeValue(streamBuf, value)
         : putValueUsingIso8601(streamBuf, value);
}

int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               const bdet_DatetimeTz&        value,
                               const bdem_BerEncoderOptions *options)
{
    return options && options->encodeDateAndTimeTypesAsBinary()
         ? putBinaryDatetimeTzValue(streamBuf, value)
         : putValueUsingIso8601(streamBuf, value);
}

int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               const bdet_DateTz&            value,
                               const bdem_BerEncoderOptions *options)
{
    return options && options->encodeDateAndTimeTypesAsBinary()
         ? putBinaryDateTzValue(streamBuf, value)
         : putValueUsingIso8601(streamBuf, value);
}

int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               const bdet_Time&              value,
                               const bdem_BerEncoderOptions *options)
{
    return options && options->encodeDateAndTimeTypesAsBinary()
         ? putBinaryTimeValue(streamBuf, value)
         : putValueUsingIso8601(streamBuf, value);
}

int bdem_BerUtil_Imp::putValue(bsl::streambuf               *streamBuf,
                               const bdet_TimeTz&            value,
                               const bdem_BerEncoderOptions *options)
{
    return options && options->encodeDateAndTimeTypesAsBinary()
         ? putBinaryTimeTzValue(streamBuf, value)
         : putValueUsingIso8601(streamBuf, value);
}

// Template specializations to allow the linker to pick them up.

template
int bdem_BerUtil_Imp::getValue<bdet_Datetime, bdet_DatetimeTz>(
                             bsl::streambuf                                 *,
                             bdeut_Variant2<bdet_Datetime, bdet_DatetimeTz> *,
                             int                                             );

template
int bdem_BerUtil_Imp::getValue<bdet_Date, bdet_DateTz>(
                                     bsl::streambuf                         *,
                                     bdeut_Variant2<bdet_Date, bdet_DateTz> *,
                                     int                                     );

template
int bdem_BerUtil_Imp::getValue<bdet_Time, bdet_TimeTz>(
                                     bsl::streambuf                         *,
                                     bdeut_Variant2<bdet_Time, bdet_TimeTz> *,
                                     int                                     );

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
