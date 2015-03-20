// bdlt_iso8601util.cpp                                               -*-C++-*-
#include <bdlt_iso8601util.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_iso8601util_cpp,"$Id$ $CSID$")

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace bdlt {

namespace {

// STATIC HELPER FUNCTIONS

static
int asciiToInt(const char **nextPos,
               int         *result,
               const char  *begin,
               const char  *end)
    // Convert the (unsigned) ASCII decimal integer starting at the specified
    // 'begin' and ending immediately before the specified 'end' into its
    // corresponding 'int' value, load the value into the specified 'result',
    // and set the specified '*nextPos' to 'end'.  Return 0 on success, and a
    // non-zero value (with no effect) otherwise.  All characters in the range
    // '[begin .. end)' must be decimal digits.  The behavior is undefined
    // unless 'begin < end' and the parsed value does not overflow an 'int'.
{
    BSLS_ASSERT(nextPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(begin < end);

    int tmp = 0;

    while (begin < end) {
        if (!isdigit(*begin)) {
            return -1;                                                // RETURN
        }

        tmp *= 10;
        tmp += *begin - '0';

        ++begin;
    }

    *result  = tmp;
    *nextPos = end;

    return 0;
}

static
int parseDate(const char **nextPos,
              int         *year,
              int         *month,
              int         *day,
              const char  *begin,
              const char  *end)
    // Parse the date, represented in the "YYYY-MM-DD" ISO 8601 extended
    // format, from the string starting at the specified 'begin' and ending
    // before the specified 'end', load into the specified 'year', 'month', and
    // 'day' their respective parsed values, and set the specified '*nextPos'
    // to the location one past the last parsed character.  Return 0 on
    // success, and a non-zero value (with no effect on '*nextPos') otherwise.
    // The behavior is undefined unless 'begin <= end'.  Note that successfully
    // parsing a date before 'end' is reached is not an error.
{
    BSLS_ASSERT(nextPos);
    BSLS_ASSERT(year);
    BSLS_ASSERT(month);
    BSLS_ASSERT(day);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(begin <= end);

    const char *p = begin;

    enum { k_MINIMUM_LENGTH = sizeof "YYYY-MM-DD" - 1 };

    if (end - p < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    // 1. Parse year.

    if (0 != asciiToInt(&p, year, p, p + 4) || '-' != *p) {
        return -1;                                                    // RETURN
    }
    ++p;  // skip '-'

    // 2. Parse month.

    if (0 != asciiToInt(&p, month, p, p + 2) || '-' != *p) {
        return -1;                                                    // RETURN
    }
    ++p;  // skip '-'

    // 3. Parse day.

    if (0 != asciiToInt(&p, day, p, p + 2)) {
        return -1;                                                    // RETURN
    }

    *nextPos = p;

    return 0;
}

static
int parseFractionalSecond(const char **nextPos,
                          int         *millisecond,
                          const char  *begin,
                          const char  *end)
    // Parse the fractional second starting at the specified 'begin' and ending
    // before the specified 'end', load into the specified 'millisecond' the
    // parsed value (in milliseconds), and set the specified '*nextPos' to the
    // location one past the last parsed character (necessarily a decimal
    // digit).  Return 0 on success, and a non-zero value (with no effect)
    // otherwise.  There must be at least one digit, only the first 4 digits
    // are significant, and all digits beyond the first 4 are parsed but
    // ignored.  The behavior is undefined unless 'begin <= end'.  Note that
    // the resulting value is rounded up to 1000 if the parsed value is .9995
    // or greater.  Also note that successfully parsing a fractional second
    // before 'end' is reached is not an error.
{
    BSLS_ASSERT(nextPos);
    BSLS_ASSERT(millisecond);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(begin <= end);

    const char *p = begin;

    // There must be at least one digit.

    if (p == end || !isdigit(*p)) {
        return -1;                                                    // RETURN
    }

    // Only the first 4 digits are significant.

    const char *endSignificant = bsl::min(end, p + 4);

    int tmp    = 0;
    int factor = 10000;  // Since the result is in milliseconds, we have to
                         // adjust it according to how many digits are present.

    do {
        tmp    *= 10;
        tmp    += *p - '0';
        factor /= 10;
    } while (++p < endSignificant && isdigit(*p));

    tmp = (tmp * factor + 5) / 10;  // round to nearest millisecond

    // Skip and ignore all digits beyond the first 4, if any.

    while (p < end && isdigit(*p)) {
        ++p;
    }

    *millisecond = tmp;
    *nextPos     = p;

    return 0;
}

static
int parseTime(const char **nextPos,
              int         *hour,
              int         *minute,
              int         *second,
              int         *millisecond,
              bool        *hasLeapSecond,
              const char  *begin,
              const char  *end)
    // Parse the time, represented in the "hh:mm:ss[.s+]" ISO 8601 extended
    // format, from the string starting at the specified 'begin' and ending
    // before the specified 'end', load into the specified 'hour', 'minute',
    // 'second', and 'millisecond' their respective parsed values, set the
    // specified 'hasLeapSecond' flag to 'true' if a leap second was indicated
    // and 'false' otherwise, and set the specified '*nextPos' to the location
    // one past the last parsed character.  Return 0 on success, and a non-zero
    // value (with no effect on '*nextPos') otherwise.  The optional fractional
    // second is converted to milliseconds, and is rounded up to 1000 if the
    // parsed value is .9995 or greater.  The behavior is undefined unless
    // 'begin <= end'.  Note that successfully parsing a time before 'end' is
    // reached is not an error.
{
    BSLS_ASSERT(nextPos);
    BSLS_ASSERT(hour);
    BSLS_ASSERT(minute);
    BSLS_ASSERT(second);
    BSLS_ASSERT(millisecond);
    BSLS_ASSERT(hasLeapSecond);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(begin <= end);

    const char *p = begin;

    enum { k_MINIMUM_LENGTH = sizeof "hh:mm:ss" - 1 };

    if (end - p < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    // 1. Parse hour.

    if (0 != asciiToInt(&p, hour, p, p + 2) || ':' != *p) {
        return -1;                                                    // RETURN
    }
    ++p;  // skip ':'

    // 2. Parse minute.

    if (0 != asciiToInt(&p, minute, p, p + 2) || ':' != *p) {
        return -1;                                                    // RETURN
    }
    ++p;  // skip ':'

    // 3. Parse second.

    if (0 != asciiToInt(&p, second, p, p + 2)) {
        return -1;                                                    // RETURN
    }

    // 4. Parse (optional) fractional second, in milliseconds.

    if (p < end && '.' == *p) {
        // We have a fraction of a second.

        ++p;  // skip '.'

        if (0 != parseFractionalSecond(&p, millisecond, p, end)) {
            return -1;                                                // RETURN
        }
    }
    else {
        *millisecond = 0;
    }

    // 5. Handle leap second.

    if (60 == *second) {
        *hasLeapSecond = true;
        *second        = 59;
    }
    else {
        *hasLeapSecond = false;
    }

    *nextPos = p;

    return 0;
}

static
int parseZoneDesignator(const char **nextPos,
                        int         *minuteOffset,
                        const char  *begin,
                        const char  *end)
    // Parse the zone designator, represented in the "[Zz]|(+|-])hh:mm" ISO
    // 8601 extended format, from the string starting at the specified 'begin'
    // and ending before the specified 'end', load into the specified
    // 'minuteOffset' the indicated offset (in minutes) from GMT, and set the
    // specified '*nextPos' to the location one past the last parsed character.
    // Return 0 on success, and a non-zero value (with no effect on '*nextPos')
    // otherwise.  The behavior is undefined unless 'begin <= end'.  Note that
    // successfully parsing a zone designator before 'end' is reached is not an
    // error.
{
    BSLS_ASSERT(nextPos);
    BSLS_ASSERT(minuteOffset);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(begin <= end);

    const char *p = begin;

    if (p >= end) {
        return -1;                                                    // RETURN
    }

    const char sign = *p++;  // store and skip '(+|-|Z|z)'

    // TBD the ISO 8601 spec. does not mention 'z'; why do we accept it??

    if ('Z' == sign || 'z' == sign) {
        *minuteOffset = 0;
        *nextPos      = p;

        return 0;                                                     // RETURN
    }

    enum { k_MINIMUM_LENGTH = sizeof "hh:mm" - 1 };

    if (('+' != sign && '-' != sign) || end - p < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    // We have parsed a '+' or '-' and established that there are sufficient
    // characters to represent "hh:mm".

    // Parse hour.

    int hour;

    if (0 != asciiToInt(&p, &hour, p, p + 2) || ':' != *p || hour >= 24) {
        return -1;                                                    // RETURN
    }
    ++p;  // skip ':'

    // Parse minute.

    int minute;

    if (0 != asciiToInt(&p, &minute, p, p + 2) || minute > 59) {
        return -1;                                                    // RETURN
    }

    *minuteOffset = hour * 60 + minute;

    if ('-' == sign) {
        *minuteOffset = -*minuteOffset;
    }

    *nextPos = p;

    return 0;
}

static
int generateInt(char *buffer, int value, int paddedLen)
    // Write, to the specified 'buffer', the decimal string representation of
    // the specified 'value' padded with leading zeros to the specified
    // 'paddedLen', and return 'paddedLen'.  'buffer' is NOT null-terminated.
    // The behavior is undefined unless 'buffer' has sufficient capacity and
    // '0 <= paddedLen'.  Note that if the decimal string representation of
    // 'value' is more than 'paddedLen' digits, only the low-order 'paddedLen'
    // digits of 'value' are output.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= paddedLen);

    char *p = buffer + paddedLen;

    while (p > buffer) {
        *--p = static_cast<char>('0' + value % 10);
        value /= 10;
    }

    return paddedLen;
}

static inline
int generateInt(char *buffer, int value, int paddedLen, char separator)
    // Write, to the specified 'buffer', the decimal string representation of
    // the specified 'value' padded with leading zeros to the specified
    // 'paddedLen' followed by the specified 'separator' character, and return
    // 'paddedLen + 1'.  'buffer' is NOT null-terminated.  The behavior is
    // undefined unless 'buffer' has sufficient capacity and '0 <= paddedLen'.
    // Note that if the decimal string representation of 'value' is more than
    // 'paddedLen' digits, only the low-order 'paddedLen' digits of 'value' are
    // output.
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= paddedLen);

    buffer += generateInt(buffer, value, paddedLen);
    *buffer = separator;

    return paddedLen + 1;
}

static
int generateZoneDesignator(char *buffer,
                           int   tzOffset,
                           bool  useZAbbreviationForUtc)
    // Write, to the specified 'buffer', the formatted zone designator
    // indicated by the specified 'tzOffset' and 'useZAbbreviationForUtc' flag,
    // and return the number of bytes written.  The formatted zone designator
    // is the character 'Z' if '0 == tzOffset && useZAbbreviationForUtc', and
    // is of the form "(+|-)hh:mm" otherwise.  The behavior is undefined unless
    // 'buffer' has sufficient capacity and 'tzOffset < 24 * 60'.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(tzOffset < 24 * 60);

    char *p = buffer;

    if (0 == tzOffset && useZAbbreviationForUtc) {
        *p++ = 'Z';
    }
    else {
        char tzSign;

        if (0 > tzOffset) {
            tzOffset = -tzOffset;
            tzSign   = '-';
        }
        else {
            tzSign   = '+';
        }

        // Timezone offset (in minutes) must be less than 24 hours.

        BSLS_ASSERT(tzOffset < 24 * 60);

        *p++ = tzSign;
        p += generateInt(p, tzOffset / 60, 2, ':');
        p += generateInt(p, tzOffset % 60, 2     );
    }

    return static_cast<int>(p - buffer);
}

static
void copyBuf(char *dst, int dstLen, const char *src, int srcLen)
    // Copy, to the specified 'dst' buffer having the specified 'dstLen', the
    // specified initial 'srcLen' characters in the specified 'src' string if
    // 'dstLen >= srcLen', and copy 'dstLen' characters otherwise.  Include a
    // null terminator iff 'dstLen > srcLen'.  The behavior is undefined unless
    // '0 <= dstLen' and '0 <= srcLen'.
{
    BSLS_ASSERT(dst);
    BSLS_ASSERT(0 <= dstLen);
    BSLS_ASSERT(src);
    BSLS_ASSERT(0 <= srcLen);

    if (dstLen > srcLen) {
        bsl::memcpy(dst, src, srcLen);
        dst[srcLen] = '\0';
    }
    else {
        bsl::memcpy(dst, src, dstLen);
    }
}

}  // close unnamed namespace

                            // ------------------
                            // struct Iso8601Util
                            // ------------------

// CLASS METHODS
int Iso8601Util::generate(char *buffer, const Date& object, int bufferLength)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    char outBuf[k_DATE_STRLEN];
    const int outLen = generateRaw(outBuf, object);
    BSLS_ASSERT(outLen == sizeof outBuf);

    copyBuf(buffer, bufferLength, outBuf, outLen);

    return outLen;
}

int Iso8601Util::generate(char *buffer, const Time& object, int bufferLength)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    char outBuf[k_TIME_STRLEN];
    const int outLen = generateRaw(outBuf, object);
    BSLS_ASSERT(outLen == sizeof outBuf);

    copyBuf(buffer, bufferLength, outBuf, outLen);

    return outLen;
}

int
Iso8601Util::generate(char *buffer, const Datetime& object, int bufferLength)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    char outBuf[k_DATETIME_STRLEN];
    const int outLen = generateRaw(outBuf, object);
    BSLS_ASSERT(outLen == sizeof outBuf);

    copyBuf(buffer, bufferLength, outBuf, outLen);

    return outLen;
}

int Iso8601Util::generate(char          *buffer,
                          const DateTz&  object,
                          int            bufferLength,
                          bool           useZAbbreviationForUtc)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    char outBuf[k_DATETZ_STRLEN];
    const int outLen = generateRaw(outBuf, object, useZAbbreviationForUtc);
    BSLS_ASSERT(outLen == sizeof outBuf
             || (0 == object.offset() && useZAbbreviationForUtc
                 && outLen <= static_cast<int>(sizeof outBuf)));

    copyBuf(buffer, bufferLength, outBuf, outLen);

    return outLen;
}

int Iso8601Util::generate(char          *buffer,
                          const TimeTz&  object,
                          int            bufferLength,
                          bool           useZAbbreviationForUtc)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    char outBuf[k_TIMETZ_STRLEN];
    const int outLen = generateRaw(outBuf, object, useZAbbreviationForUtc);
    BSLS_ASSERT(outLen == sizeof outBuf
             || (0 == object.offset() && useZAbbreviationForUtc
                 && outLen <= static_cast<int>(sizeof outBuf)));

    copyBuf(buffer, bufferLength, outBuf, outLen);

    return outLen;
}

int Iso8601Util::generate(char              *buffer,
                          const DatetimeTz&  object,
                          int                bufferLength,
                          bool               useZAbbreviationForUtc)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    char outBuf[k_DATETIMETZ_STRLEN];
    const int outLen = generateRaw(outBuf, object, useZAbbreviationForUtc);
    BSLS_ASSERT(outLen == sizeof outBuf
             || (0 == object.offset() && useZAbbreviationForUtc
                 && outLen <= static_cast<int>(sizeof outBuf)));

    copyBuf(buffer, bufferLength, outBuf, outLen);

    return outLen;
}

int Iso8601Util::generateRaw(char *buffer, const Date& object)
{
    BSLS_ASSERT(buffer);

    char *p = buffer;

    p += generateInt(p, object.year() , 4, '-');
    p += generateInt(p, object.month(), 2, '-');
    p += generateInt(p, object.day()  , 2     );

    return static_cast<int>(p - buffer);
}

int Iso8601Util::generateRaw(char *buffer, const Time& object)
{
    BSLS_ASSERT(buffer);

    char *p = buffer;

    p += generateInt(p, object.hour()       , 2, ':');
    p += generateInt(p, object.minute()     , 2, ':');
    p += generateInt(p, object.second()     , 2, '.');
    p += generateInt(p, object.millisecond(), 3     );

    return static_cast<int>(p - buffer);
}

int Iso8601Util::generateRaw(char *buffer, const Datetime& object)
{
    BSLS_ASSERT(buffer);

    const int dateLen = generateRaw(buffer, object.date());
    *(buffer + dateLen) = 'T';
    const int timeLen = generateRaw(buffer + dateLen + 1, object.time());

    return dateLen + timeLen + 1;
}

int Iso8601Util::generateRaw(char          *buffer,
                             const DateTz&  object,
                             bool           useZAbbreviationForUtc)
{
    BSLS_ASSERT(buffer);

    const int dateLen = generateRaw(buffer, object.localDate());
    const int zoneLen = generateZoneDesignator(buffer + dateLen,
                                               object.offset(),
                                               useZAbbreviationForUtc);

    return dateLen + zoneLen;
}

int Iso8601Util::generateRaw(char          *buffer,
                             const TimeTz&  object,
                             bool           useZAbbreviationForUtc)
{
    BSLS_ASSERT(buffer);

    const int timeLen = generateRaw(buffer, object.localTime());
    const int zoneLen = generateZoneDesignator(buffer + timeLen,
                                               object.offset(),
                                               useZAbbreviationForUtc);

    return timeLen + zoneLen;
}

int Iso8601Util::generateRaw(char              *buffer,
                             const DatetimeTz&  object,
                             bool               useZAbbreviationForUtc)
{
    BSLS_ASSERT(buffer);

    const int datetimeLen = generateRaw(buffer, object.localDatetime());
    const int zoneLen     = generateZoneDesignator(buffer + datetimeLen,
                                                   object.offset(),
                                                   useZAbbreviationForUtc);

    return datetimeLen + zoneLen;
}

int Iso8601Util::parse(Date *result, const char *input, int inputLength)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(input);
    BSLS_ASSERT(0 <= inputLength);

    // Sample ISO 8601 date: "2005-01-31+04:00"
    //
    // The zone designator is optional.

    enum { k_MINIMUM_LENGTH = sizeof "YYYY-MM-DD" - 1 };

    if (inputLength < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    const char *p   = input;
    const char *end = input + inputLength;

    // 1. Parse and validate date.

    int year, month, day;

    if (0 != parseDate(&p, &year, &month, &day, p, end)
     || !Date::isValid(year, month, day)) {
        return -1;                                                    // RETURN
    }

    // 2. Parse and ignore zone designator, if any.

    if (p != end) {
        int tzOffset;

        if (0 != parseZoneDesignator(&p, &tzOffset, p, end) || p != end) {
            return -1;                                                // RETURN
        }
    }

    result->setYearMonthDay(year, month, day);

    return 0;
}

int Iso8601Util::parse(Time *result, const char *input, int inputLength)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(input);
    BSLS_ASSERT(0 <= inputLength);

    // Sample ISO 8601 time: "08:59:59.999-04:00"
    //
    // The fractional second and zone designator are independently optional.

    enum { k_MINIMUM_LENGTH = sizeof "hh:mm:ss" - 1 };

    if (inputLength < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    const char *p   = input;
    const char *end = input + inputLength;

    // 1. Parse and validate time.

    // Milliseconds could be rounded to 1000 (if fractional second is .9995 or
    // greater).  Thus, we have to add it after setting the time, else it might
    // not validate.

    int  hour, minute, second, millisecond;
    bool hasLeapSecond;
    Time localTime;

    if (0 != parseTime(&p,
                       &hour,
                       &minute,
                       &second,
                       &millisecond,
                       &hasLeapSecond,
                       p,
                       end)
     || 0 != localTime.setTimeIfValid(hour, minute, second)) {
        return -1;                                                    // RETURN
    }

    if (hasLeapSecond) {
        localTime.addSeconds(1);
    }

    if (millisecond) {
        localTime.addMilliseconds(millisecond);
    }

    // 2. Parse zone designator, if any.

    int tzOffset = 0;  // minutes from GMT

    if (p != end) {
        if (0 != parseZoneDesignator(&p, &tzOffset, p, end) || p != end) {
            return -1;                                                // RETURN
        }

        if (tzOffset) {
            localTime.addMinutes(-tzOffset);  // convert to GMT
        }
    }

    // '24 == hour' is allowed only for the value '24:00:00.000' in GMT.

    if (24 == hour && (millisecond || tzOffset)) {
        return -1;                                                    // RETURN
    }

    *result = localTime;

    return 0;
}

int Iso8601Util::parse(Datetime *result, const char *input, int inputLength)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(input);
    BSLS_ASSERT(0 <= inputLength);

    // Sample ISO 8601 datetime: "2005-01-31T08:59:59.999-04:00"
    //
    // The fractional second and zone designator are independently optional.

    // 1. Parse as a 'DatetimeTz'.

    DatetimeTz datetimeTz;

    const int rc = parse(&datetimeTz, input, inputLength);

    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    // 2. Account for edge cases.

    if (datetimeTz.offset() > 0) {
        Datetime minDatetime(0001, 01, 01, 00, 00, 00, 000);

        minDatetime.addMinutes(datetimeTz.offset());

        if (minDatetime > datetimeTz.localDatetime()) {
            return -1;                                                // RETURN
        }
    }
    else if (datetimeTz.offset() < 0) {
        Datetime maxDatetime(9999, 12, 31, 23, 59, 59, 999);

        maxDatetime.addMinutes(datetimeTz.offset());

        if (maxDatetime < datetimeTz.localDatetime()) {
            return -1;                                                // RETURN
        }
    }

    *result = datetimeTz.utcDatetime();

    return 0;
}

int Iso8601Util::parse(DateTz *result, const char *input, int inputLength)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(input);
    BSLS_ASSERT(0 <= inputLength);

    // Sample ISO 8601 date: "2005-01-31+04:00"
    //
    // The zone designator is optional.

    enum { k_MINIMUM_LENGTH = sizeof "YYYY-MM-DD" - 1 };

    if (inputLength < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    const char *p   = input;
    const char *end = input + inputLength;

    // 1. Parse and validate date.

    int  year, month, day;
    Date localDate;

    if (0 != parseDate(&p, &year, &month, &day, p, end)
     || 0 != localDate.setYearMonthDayIfValid(year, month, day)) {
        return -1;                                                    // RETURN
    }

    // 2. Parse zone designator, if any.

    int tzOffset = 0;  // minutes from GMT

    if (p != end) {
        if (0 != parseZoneDesignator(&p, &tzOffset, p, end) || p != end) {
            return -1;                                                // RETURN
        }
    }

    result->setDateTz(localDate, tzOffset);

    return 0;
}

int Iso8601Util::parse(TimeTz *result, const char *input, int inputLength)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(input);
    BSLS_ASSERT(0 <= inputLength);

    // Sample ISO 8601 time: "08:59:59.999-04:00"
    //
    // The fractional second and zone designator are independently optional.

    enum { k_MINIMUM_LENGTH = sizeof "hh:mm:ss" - 1 };

    if (inputLength < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    const char *p   = input;
    const char *end = input + inputLength;

    // 1. Parse and validate time.

    // Milliseconds could be rounded to 1000 (if fractional second is .9995 or
    // greater).  Thus, we have to add it after setting the time, else it might
    // not validate.

    int  hour, minute, second, millisecond;
    bool hasLeapSecond;
    Time localTime;

    if (0 != parseTime(&p,
                       &hour,
                       &minute,
                       &second,
                       &millisecond,
                       &hasLeapSecond,
                       p,
                       end)
     || 0 != localTime.setTimeIfValid(hour, minute, second)) {
        return -1;                                                    // RETURN
    }

    if (hasLeapSecond) {
        localTime.addSeconds(1);
    }

    if (millisecond) {
        localTime.addMilliseconds(millisecond);
    }

    // 2. Parse zone designator, if any.

    int tzOffset = 0;  // minutes from GMT

    if (p != end) {
        if (0 != parseZoneDesignator(&p, &tzOffset, p, end) || p != end) {
            return -1;                                                // RETURN
        }
    }

    // '24 == hour' is allowed only for the value '24:00:00.000' in GMT.

    if (24 == hour && (millisecond || tzOffset)) {
        return -1;                                                    // RETURN
    }

    result->setTimeTz(localTime, tzOffset);

    return 0;
}

int Iso8601Util::parse(DatetimeTz *result, const char *input, int inputLength)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(input);
    BSLS_ASSERT(0 <= inputLength);

    // Sample ISO 8601 datetime: "2005-01-31T08:59:59.999-04:00"
    //
    // The fractional second and zone designator are independently optional.

    enum { k_MINIMUM_LENGTH = sizeof "YYYY-MM-DDThh:mm:ss" - 1 };

    if (inputLength < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    const char *p   = input;
    const char *end = input + inputLength;

    // 1. Parse date.

    int year, month, day;

    if (0   != parseDate(&p, &year, &month, &day, p, end)
     || p   == end
     || 'T' != *p) {

        return -1;                                                    // RETURN
    }
    ++p;  // skip 'T'

    // 2. Parse time.

    int  hour, minute, second, millisecond;
    bool hasLeapSecond;

    if (0 != parseTime(&p,
                       &hour,
                       &minute,
                       &second,
                       &millisecond,
                       &hasLeapSecond,
                       p,
                       end)) {
        return -1;                                                    // RETURN
    }

    // 3. Parse zone designator, if any.

    int tzOffset = 0;  // minutes from GMT

    if (p != end) {
        if (0 != parseZoneDesignator(&p, &tzOffset, p, end) || p != end) {
            return -1;                                                // RETURN
        }
    }

    // 4. Account for special ISO 8601 values.

    ///Leap Seconds and Maximum Fractional Seconds
    ///- - - - - - - - - - - - - - - - - - - - - -
    // Note that leap seconds or 'millisecond' values of 1000 (which result
    // from rounding up a fractional second that is .9995 or greater) cannot be
    // directly represented with a 'Datetime'.  Hence, we create an initial
    // 'Datetime' object without accounting for these, then adjust it forward
    // by up to 2 seconds, as needed.
    //
    ///24:00
    ///- - -
    // An 'hour' value of 24 is not valid unless 'minute', 'second', and
    // 'millisecond' are all 0.  Further note that supplying an hour of 24 when
    // constructing a 'Datetime' results in a different interpretation than
    // that provided by ISO 8601 (see '{Note Regarding the Time 24:00}' in the
    // component-level documentation).

    if (24 == hour) {
        // '24 == hour' is allowed only for the value '24:00:00.000' in GMT.

        if (minute || second || millisecond || tzOffset) {
            return -1;                                                // RETURN
        }
    }

    DatetimeInterval resultAdjustment;  // adjust for leap second and maximum
                                        // fractional second

    if (hasLeapSecond) {
        resultAdjustment.addSeconds(1);
    }

    if (1000 == millisecond) {
        millisecond = 0;
        resultAdjustment.addSeconds(1);
    }

    // 5. Load a 'Datetime'.

    Datetime localDatetime;

    if (0 != localDatetime.setDatetimeIfValid(
                        year, month, day, hour, minute, second, millisecond)) {
        return -1;                                                    // RETURN
    }

    // 6. Apply adjustments for special ISO 8601 values.

    if (DatetimeInterval() != resultAdjustment) {

        BSLS_ASSERT(resultAdjustment > DatetimeInterval());
        // We assert the above to prevent future developers from accidentally
        // introducing negative adjustments, which are not handled by the
        // following logic.

        const Datetime maxDatetime(9999, 12, 31, 23, 59, 59, 999);

        if (maxDatetime - resultAdjustment < localDatetime) {
            return -1;                                                // RETURN
        }

        localDatetime += resultAdjustment;
    }

    result->setDatetimeTz(localDatetime, tzOffset);

    return 0;
}

                        // -------------------------------
                        // struct Iso8601UtilConfiguration
                        // -------------------------------

// CLASS DATA
bool Iso8601UtilConfiguration::s_useZAbbreviationForUtc = false;

// CLASS METHODS
void Iso8601UtilConfiguration::setUseZAbbreviationForUtc(
                                               bool useZAbbreviationForUtcFlag)
{
    s_useZAbbreviationForUtc = useZAbbreviationForUtcFlag;
}

bool Iso8601UtilConfiguration::useZAbbreviationForUtc()
{
    return s_useZAbbreviationForUtc;
}

}  // close package namespace
}  // close enterprise namespace

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
