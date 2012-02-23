// bdepu_iso8601.cpp                                                  -*-C++-*-
#include <bdepu_iso8601.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdepu_iso8601_cpp,"$Id$ $CSID$")

#include <bdepu_typesparser.h>

#include <bdet_date.h>
#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdet_datetz.h>
#include <bdet_time.h>
#include <bdet_timetz.h>
#include <bdeu_chartype.h>

#include <bsls_assert.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS

static
int parseUint(const char **nextPos,
              int         *result,
              const char  *begin,
              const char  *end)
    // Parse the unsigned integer described by the string starting at
    // specified 'begin' and ending before the specified 'end', then load the
    // integer value into the specified 'result' and load '*nextPos' with the
    // address one past the last character parsed.  Return 0 on success and a
    // non-zero value otherwise.  Failure will occur if there '*begin' is not a
    // digit, or if the string of digits represents a value too large to
    // represent in a signed integer.  The behavior is undefined unless 'begin'
    // and 'end' describe a contiguous range of memory.
{
    BSLS_ASSERT(nextPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);

    bool allZeroes = true;
    int nextResult = 0;
    const char *inPtr = begin;
    while (inPtr < end && bdeu_CharType::isDigit(*inPtr)) {
        int digit = *inPtr++ - '0';

        allZeroes &= 0 == digit;
        if (!allZeroes) {
            enum { INT_MAX_OVER_10 = INT_MAX / 10 };
            if (nextResult > INT_MAX_OVER_10) {
                return -1;                                            // RETURN
            }
            nextResult *= 10;

            if (nextResult > INT_MAX - digit) {
                return -1;                                            // RETURN
            }
            nextResult += digit;
        }
    }

    if (inPtr == begin) {
        // no digits were found, not a number

        return -1;                                                    // RETURN
    }

    *nextPos = inPtr;
    *result  = nextResult;

    return 0;
}

static
int parseDate(int         *year,
              int         *month,
              int         *day,
              const char **begin,
              const char  *end)
    // Parse a date, represented in "YYYY-MM-DD" format, from the string
    // starting at the specified '*begin' and ending before the specified 'end'
    // then load the specified 'year', 'month' and 'day' with their respective
    // parsed values, and set '*begin' to the location one past the last parsed
    // character.  In the "YYYY-MM-DD" format accepted by this function,
    // 'YYYY', 'MM', and 'DD' are strings representing positive integers, '-'
    // is literally a dash character, 'YYYY' is 4 chars long, and 'MM' and 'DD'
    // are both 2 chars long.  Return 0 on success and a non-zero value if the
    // string being parsed does not match the specified format (including
    // partial representations).  Note that if the pattern is successfully
    // completed before 'end' is reached, that is not an error.
{
    BSLS_ASSERT(year);
    BSLS_ASSERT(month);
    BSLS_ASSERT(day);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);

    enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = -1 };

    const char *p = *begin;

    // Parse year.

    const char *expectedEnd = p + 4;

    if   (0   != parseUint(&p, year, p, end)
       || p   >= end
       || p   != expectedEnd
       || '-' != *p) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    ++p;  // skip hyphen

    // Parse month.

    expectedEnd = p + 2;

    if   (0   != parseUint(&p, month, p, end)
       || p   >= end
       || p   != expectedEnd
       || '-' != *p) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    ++p;  // skip hyphen

    // Parse day.

    expectedEnd = p + 2;

    if   (0 != parseUint(&p, day, p, expectedEnd)
       || p != expectedEnd) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    *begin = p;

    return BDEPU_SUCCESS;
}

static
int parseTime(int         *hour,
              int         *minute,
              int         *second,
              int         *millisecond,
              const char **begin,
              const char  *end)
    // Parse a time, represented in "hh:mm:ss[.d+]" format, from the string
    // starting at the specified '*begin' and ending before the specified
    // 'end', then load the specified 'hour', 'minute', 'second' and
    // 'millisecond' with their respective parsed values, and set '*begin' to
    // the location one past the last parsed character.  In the "hh:mm:ss[.d+]"
    // format accepted by this function, 'hh', 'mm', 'ss' are all 2 digit
    // integers (left padded with 0's if necessary) denoting hours, minutes,
    // and seconds, ':' is literaly a colon character, and [.d+] is the
    // optional fraction of a second, consisting of a '.' followed by one or
    // more decimal digits.  If '[.d+]' contains more than 3 digits, the value
    // will be rounded to the nearest value in milliseconds.  Return 0 on
    // success and a non-zero value if the string being parsed does not match
    // the specified format (including partial representations).  Note that a
    // fractional second value of '.9995' or more will be rounded to 1000
    // milliseconds (and higher level functions are responsible for
    // incrementing the represented value of 'seconds' if necessary).  Also
    // note that if the pattern is successfully completed before 'end' is
    // reached, that is not an error.
{
    BSLS_ASSERT(hour);
    BSLS_ASSERT(minute);
    BSLS_ASSERT(second);
    BSLS_ASSERT(millisecond);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);

    enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = -1 };

    const char *p = *begin;

    // Parse hour.

    const char *expectedEnd = p + 2;

    if (0   != parseUint(&p, hour, p, end)
     || p   >= end
     || p   != expectedEnd
     || ':' != *p) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    ++p;  // skip colon

    // Parse minute.

    expectedEnd = p + 2;

    if (0   != parseUint(&p, minute, p, end)
     || p   >= end
     || p   != expectedEnd
     || ':' != *p) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    ++p;  // skip colon

    // Parse second.

    expectedEnd = p + 2;

    if   (0 != parseUint(&p, second, p, expectedEnd)
       || p != expectedEnd) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    // Parse millisecond.

    *millisecond = 0;

    if (p < end && '.' == *p) {
        // We have a fraction of a second.

        ++p;  // skip dot

        if (p >= end || !bdeu_CharType::isDigit(*p)) {
            return BDEPU_FAILURE;                                     // RETURN
        }

        char fractionBuffer[5];  // use exactly 4 digits after decimal point
        // Pad with zeros and null-terminate.
        bsl::memcpy(fractionBuffer, "0000", 5);

        // The 'fractionBuffer' represents tenths of milliseconds.  Copy up to
        // 4 digits from input string.  String remains padded with zero to 4
        // digits.  Thus, an input of "02" yields a 'fractionBuffer' of "0200"
        // (20 milliseconds or 200 tenths).

        for (int i = 0; i < 4 && p < end && bdeu_CharType::isDigit(*p); ++i) {
            fractionBuffer[i] = *p++;
        }

        const char *endOfFraction;
        int         fraction;
        if (0 != parseUint(&endOfFraction,
                           &fraction,
                           fractionBuffer,
                           fractionBuffer + 4)) {
            return BDEPU_FAILURE;                                     // RETURN
        }

        // Fraction holds tenths of milliseconds.  Round to milliseconds.
        *millisecond = (fraction + 5) / 10;

        // Skip remaining digits in fraction:
        while (p < end && bdeu_CharType::isDigit(*p)) {
            ++p;
        }

    }

    *begin = p;

    return BDEPU_SUCCESS;
}

static
int parseTimezoneOffset(int         *minuteOffset,
                        const char **begin,
                        const char  *end)
    // Parse a time zone offset, represented in "Shh:mm" format, from the
    // string starting at the specified '*begin' and ending before the
    // specified 'end' then load the specified 'minuteOffset' with the parsed
    // time zone offset (in minutes), and set '*begin' to the location one past
    // the last parsed character.  In the "Shh:mm" format accepted by this
    // function, 'S' is either '+' or '-', 'hh' and 'mm' are 2 digit integers
    // (left padded with '0's if necessary).  'hh' must be in the range
    // '[ 00, 24 )' and 'mm' must be in the range '[ 0, 60 )'.  An alternate
    // form of the representation is 'Z' or 'z', signifying a zero offset.
    // Return 0 on success and a non-zero value if the string being parsed does
    // not match the specified format.  Note that if the pattern is
    // successfully completed before 'end' is reached, that is not an error.
{
    BSLS_ASSERT(minuteOffset);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);

    enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = -1 };

    const char *p = *begin;

    if (p >= end) {
        return BDEPU_FAILURE;
    }

    char sign = *p;

    if ('Z' == sign || 'z' == sign) {
        *minuteOffset = 0;

        *begin = (*begin) + 1;

        return BDEPU_SUCCESS;                                         // RETURN
    }
    else if ('+' != sign && '-' != sign) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    ++p;  // skip sign

    const char *expectedEnd = p + 2;

    int hourVal;
    if   (0           != parseUint(&p, &hourVal, p, end)
       || p           != expectedEnd
       || hourVal     >= 24  // Max TZ offset is 24 hours.
       || p           >= end
       || ':' != *p) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    ++p;  // skip ':'

    expectedEnd = p + 2;

    int minuteVal;
    if    (0 != parseUint(&p, &minuteVal, p, end)
       ||  p != expectedEnd
       ||  minuteVal > 59) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    *minuteOffset = hourVal * 60 + minuteVal;
    if ('-' == sign) {
        *minuteOffset *= -1;
    }

    *begin = p;

    return BDEPU_SUCCESS;
}

static
char *generateInt(char *buffer, int val, int len)
    // Write into the specified 'buffer', the decimal value of the specified
    // 'val', left-padded with zeros to the specified 'len' and return
    // 'buffer + len'.  The buffer is NOT null-terminated.  Note that if the
    // decimal string representation of 'val' is more than 'len' digits, only
    // the low order digits are printed.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= len);

    buffer += len;
    for (char *p = buffer; len != 0; --len) {
        *--p = '0' + val % 10;
        val /= 10;
    }

    return buffer;
}

static inline
char *generateInt(char *buffer, int val, int len, char separator)
    // Write into the specified 'buffer', the decimal value of the specified
    // 'val', left-padded with zeros to the specified 'len' and followed by
    // the specified 'separator' character then return 'buffer + len + 1'.
    // The buffer is NOT null-terminated.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= len);

    buffer = generateInt(buffer, val, len);
    *buffer++ = separator;
    return buffer;
}

static
int copyBuf(char *dest, int destLen, const char *src, int srcLen)
    // Copy the contents of the specified 'src' string having the specified
    // 'srcLen' into the specified 'dest' buffer having the specified
    // 'destLen' and return 'srcLen'.  If 'destLen > srcLen', add a null
    // terminator.  If 'srcLen >= destLen', then copy only 'destLen' bytes
    // and add do not add a null terminator.
{
    BSLS_ASSERT(dest);
    BSLS_ASSERT(0 <= destLen);
    BSLS_ASSERT(src);
    BSLS_ASSERT(0 <= srcLen);

    if (destLen > srcLen) {
        bsl::memcpy(dest, src, srcLen);
        dest[srcLen] = '\0';
    }
    else {
        bsl::memcpy(dest, src, destLen);
    }

    return srcLen;
}

                            // --------------------
                            // struct bdepu_Iso8601
                            // --------------------

                           // *** GENERATING FUNCTIONS

int bdepu_Iso8601::generate(char             *buffer,
                            const bdet_Date&  object,
                            int               bufLen)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufLen);

    char outBuf[BDEPU_DATE_STRLEN];
    int  outLen = generateRaw(outBuf, object);
    BSLS_ASSERT(outLen == sizeof(outBuf));
    return copyBuf(buffer, bufLen, outBuf, sizeof(outBuf));
}

int bdepu_Iso8601::generate(char                 *buffer,
                            const bdet_Datetime&  object,
                            int                   bufLen)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufLen);

    char outBuf[BDEPU_DATETIME_STRLEN];
    int  outLen = generateRaw(outBuf, object);
    BSLS_ASSERT(outLen == sizeof(outBuf));
    return copyBuf(buffer, bufLen, outBuf, sizeof(outBuf));
}

int bdepu_Iso8601::generate(char                   *buffer,
                            const bdet_DatetimeTz&  object,
                            int                     bufLen)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufLen);

    char outBuf[BDEPU_DATETIMETZ_STRLEN];
    int  outLen = generateRaw(outBuf, object);
    BSLS_ASSERT(outLen == sizeof(outBuf));
    return copyBuf(buffer, bufLen, outBuf, sizeof(outBuf));
}

int bdepu_Iso8601::generate(char               *buffer,
                            const bdet_DateTz&  object,
                            int                 bufLen)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufLen);

    char outBuf[BDEPU_DATETZ_STRLEN];
    int  outLen = generateRaw(outBuf, object);
    BSLS_ASSERT(outLen == sizeof(outBuf));
    return copyBuf(buffer, bufLen, outBuf, sizeof(outBuf));
}

int bdepu_Iso8601::generate(char             *buffer,
                            const bdet_Time&  object,
                            int               bufLen)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufLen);

    char outBuf[BDEPU_TIME_STRLEN];
    int  outLen = generateRaw(outBuf, object);
    BSLS_ASSERT(outLen == sizeof(outBuf));
    return copyBuf(buffer, bufLen, outBuf, sizeof(outBuf));
}

int bdepu_Iso8601::generate(char               *buffer,
                            const bdet_TimeTz&  object,
                            int                 bufLen)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufLen);

    char outBuf[BDEPU_TIMETZ_STRLEN];
    int  outLen = generateRaw(outBuf, object);
    BSLS_ASSERT(outLen == sizeof(outBuf));
    return copyBuf(buffer, bufLen, outBuf, sizeof(outBuf));
}

int bdepu_Iso8601::generateRaw(char             *buffer,
                               const bdet_Date&  object)
{
    BSLS_ASSERT(buffer);

    char *outp = buffer;
    outp = generateInt(outp, object.year() , 4, '-');
    outp = generateInt(outp, object.month(), 2, '-');
    outp = generateInt(outp, object.day()  , 2     );

    return outp - buffer;
}

int bdepu_Iso8601::generateRaw(char                 *buffer,
                               const bdet_Datetime&  object)
{
    BSLS_ASSERT(buffer);

    char *outp = buffer;
    outp = generateInt(outp, object.year()       , 4, '-');
    outp = generateInt(outp, object.month()      , 2, '-');
    outp = generateInt(outp, object.day()        , 2, 'T');
    outp = generateInt(outp, object.hour()       , 2, ':');
    outp = generateInt(outp, object.minute()     , 2, ':');
    outp = generateInt(outp, object.second()     , 2, '.');
    outp = generateInt(outp, object.millisecond(), 3     );

    return outp - buffer;
}

int bdepu_Iso8601::generateRaw(char                   *buffer,
                               const bdet_DatetimeTz&  object)
{
    BSLS_ASSERT(buffer);

    int  timezoneOffset = object.offset();
    char timezoneSign;

    if (0 > timezoneOffset) {
        timezoneOffset = -timezoneOffset;
        timezoneSign   = '-';
    }
    else {
        timezoneSign = '+';
    }

    // TZ offset cannot be more than 24 hours.
    BSLS_ASSERT(timezoneOffset <= 24 * 60);

    bdet_Datetime localDatetime = object.localDatetime();

    char *outp = buffer;
    outp = generateInt(outp, localDatetime.year()       , 4, '-');
    outp = generateInt(outp, localDatetime.month()      , 2, '-');
    outp = generateInt(outp, localDatetime.day()        , 2, 'T');
    outp = generateInt(outp, localDatetime.hour()       , 2, ':');
    outp = generateInt(outp, localDatetime.minute()     , 2, ':');
    outp = generateInt(outp, localDatetime.second()     , 2, '.');
    outp = generateInt(outp, localDatetime.millisecond(), 3, timezoneSign);
    outp = generateInt(outp, timezoneOffset / 60        , 2, ':');
    outp = generateInt(outp, timezoneOffset % 60        , 2     );

    return outp - buffer;
}

int bdepu_Iso8601::generateRaw(char               *buffer,
                               const bdet_DateTz&  object)
{
    BSLS_ASSERT(buffer);

    int  timezoneOffset = object.offset();
    char timezoneSign;

    if (0 > timezoneOffset) {
        timezoneOffset = -timezoneOffset;
        timezoneSign   = '-';
    }
    else {
        timezoneSign = '+';
    }

    // TZ offset cannot be more than 24 hours.
    BSLS_ASSERT(timezoneOffset <= 24 * 60);

    bdet_Date localDate = object.localDate();

    char *outp = buffer;
    outp = generateInt(outp, localDate.year()   , 4, '-');
    outp = generateInt(outp, localDate.month()  , 2, '-');
    outp = generateInt(outp, localDate.day()    , 2, timezoneSign);
    outp = generateInt(outp, timezoneOffset / 60, 2, ':');
    outp = generateInt(outp, timezoneOffset % 60, 2     );

    return outp - buffer;
}

int bdepu_Iso8601::generateRaw(char             *buffer,
                               const bdet_Time&  object)
{
    BSLS_ASSERT(buffer);

    char *outp = buffer;
    outp = generateInt(outp, object.hour()       , 2, ':');
    outp = generateInt(outp, object.minute()     , 2, ':');
    outp = generateInt(outp, object.second()     , 2, '.');
    outp = generateInt(outp, object.millisecond(), 3     );

    return outp - buffer;
}

int bdepu_Iso8601::generateRaw(char               *buffer,
                               const bdet_TimeTz&  object)
{
    BSLS_ASSERT(buffer);

    char *outp = buffer;
    int  timezoneOffset = object.offset();
    char timezoneSign;

    if (0 > timezoneOffset) {
        timezoneOffset = -timezoneOffset;
        timezoneSign   = '-';
    }
    else {
        timezoneSign = '+';
    }

    // TZ offset cannot be more than 24 hours.
    BSLS_ASSERT(timezoneOffset <= 24 * 60);

    bdet_Time localTime = object.localTime();

    outp = generateInt(outp, localTime.hour()        , 2, ':');
    outp = generateInt(outp, localTime.minute()      , 2, ':');
    outp = generateInt(outp, localTime.second()      , 2, '.');
    outp = generateInt(outp, localTime.millisecond() , 3, timezoneSign);
    outp = generateInt(outp, timezoneOffset / 60     , 2, ':');
    outp = generateInt(outp, timezoneOffset % 60     , 2     );

    return outp - buffer;
}

                           // *** PARSING FUNCTIONS

int bdepu_Iso8601::parse(bdet_Date  *result,
                         const char *input,
                         int         inputLength)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(input);
    BSLS_ASSERT(0 <= inputLength);

    enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = -1 };

    // Sample XML date: "2005-01-31".

    const char *begin = input;
    const char *end   = input + inputLength;

    // Parse date.

    int year, month, day;

    if (0 != parseDate(&year, &month, &day, &begin, end)) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    if (end != begin) {
        // Parse timezone.  Note that the timezone is always ignored for
        // 'bdet_Date'.

        int tzOffset;

        if (0 != parseTimezoneOffset(&tzOffset, &begin, end) || end != begin) {
            return BDEPU_FAILURE;                                     // RETURN
        }
    }

    if (!bdet_Date::isValid(year, month, day)) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    *result = bdet_Date(year, month, day);

    return BDEPU_SUCCESS;
}

int bdepu_Iso8601::parse(bdet_Datetime *result,
                         const char    *input,
                         int            inputLength)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(input);
    BSLS_ASSERT(0 <= inputLength);

    enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = -1 };

    // Sample XML datetime: "2005-01-31T08:59:59.999".
    // Timezone may be optionally specified: "2005-01-31T08:59:59.999-04:00".
    // Also, there might be more than 3 decimal places for the fraction of a
    // second.  But when storing in bdet_Datetime, we only take the 3 most
    // significant digits.

    const char *begin = input;
    const char *end   = input + inputLength;

    // Parse date.

    int year, month, day;

    if (0   != parseDate(&year, &month, &day, &begin, end)
     || end <= begin
     || 'T' != *begin) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    ++begin;  // skip 'T'

    // Parse time.

    int hour, minute, second, millisecond;

    if (0 != parseTime(&hour, &minute, &second, &millisecond, &begin, end)) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    int timezoneOffset = 0;  // minutes from GMT

    if (end != begin) {
        // Parse timezone.

        if (0   != parseTimezoneOffset(&timezoneOffset, &begin, end)
         || end != begin) {
            return BDEPU_FAILURE;                                     // RETURN
        }
    }

    // Milliseconds could be 1000 (if fraction is .9995 or greater).  Thus, we
    // have to add it after setting the datetime else it might not validate.

    bdet_Datetime localDatetime;
    if (localDatetime.setDatetimeIfValid(year, month, day,
                                         hour, minute, second)) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    // 'addTime' and/or 'addMinutes' will reset '24:00:00' to '00:00:00' (even
    // if the quantities added are 0), which we don't want to happen.
    // 'hours == 24' is only allowed for the value '24:00:00.000' with timezone
    // GMT.

    if (millisecond || timezoneOffset) {
        if (24 == hour) {
            return BDEPU_FAILURE;                                     // RETURN
        }
        localDatetime.addTime(0, 0, 0, millisecond);
        localDatetime.addMinutes(-timezoneOffset);  // convert to GMT
    }

    *result = localDatetime;
    return BDEPU_SUCCESS;
}

int bdepu_Iso8601::parse(bdet_DatetimeTz *result,
                         const char      *input,
                         int              inputLength)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(input);
    BSLS_ASSERT(0 <= inputLength);

    enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = -1 };

    // Sample XML datetime: "2005-01-31T08:59:59.999".
    // Timezone may be optionally specified: "2005-01-31T08:59:59.999-04:00".
    // Also, there might be more than 3 decimal places for the fraction of a
    // second.  But when storing in bdet_Datetime, we only take the 3 most
    // significant digits.

    const char *begin = input;
    const char *end   = input + inputLength;

    // Parse date.

    int year, month, day;

    if (0   != parseDate(&year, &month, &day, &begin, end)
     || end <= begin
     || 'T' != *begin) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    ++begin;  // skip 'T'

    // Parse time.

    int hour, minute, second, millisecond;

    if (0 != parseTime(&hour, &minute, &second, &millisecond, &begin, end)) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    int timezoneOffset = 0;  // minutes from GMT

    if (end != begin) {
        // Parse timezone.
        if (0   != parseTimezoneOffset(&timezoneOffset, &begin, end)
         || end != begin) {
            return BDEPU_FAILURE;                                     // RETURN
        }
    }

    // Milliseconds could be 1000 (if fraction is .9995 or greater).  Thus, we
    // have to add it after setting the datetime else it might not validate.

    bdet_Datetime localDatetime;
    if (localDatetime.setDatetimeIfValid(year, month, day,
                                         hour, minute, second)) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    // 'addTime' will reset '24:00:00' to '00:00:00' (even if the quantity
    // added is 0), which we don't want to happen.  'hours == 24' is only
    // allowed for the value '24:00:00.000' and timezone GMT.

    if (millisecond || timezoneOffset) {
        if (24 == hour) {
            return BDEPU_FAILURE;                                     // RETURN
        }
        localDatetime.addTime(0, 0, 0, millisecond);
    }

    result->setDatetimeTz(localDatetime, timezoneOffset);

    return BDEPU_SUCCESS;
}

int bdepu_Iso8601::parse(bdet_DateTz *result,
                         const char  *input,
                         int          inputLength)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(input);
    BSLS_ASSERT(0 <= inputLength);

    enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = -1 };

    // Sample XML datetime: "2005-01-31".
    // Timezone may be optionally specified: "2005-01-31-04:00".
    // Also, there might be more than 3 decimal places for the fraction of a
    // second.  But when storing in bdet_Datetime, we only take the 3 most
    // significant digits.

    const char *begin = input;
    const char *end   = input + inputLength;

    // Parse date.

    int year, month, day;

    if (0 != parseDate(&year, &month, &day, &begin, end)) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    int timezoneOffset = 0;  // minutes from GMT

    if (end != begin) {
        // Parse timezone.
        if (0   != parseTimezoneOffset(&timezoneOffset, &begin, end)
         || end != begin) {
            return BDEPU_FAILURE;                                     // RETURN
        }
    }

    if (!bdet_Date::isValid(year, month, day)) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    bdet_Date localDate(year, month, day);

    result->setDateTz(localDate, timezoneOffset);

    return BDEPU_SUCCESS;
}

int bdepu_Iso8601::parse(bdet_Time  *result,
                         const char *input,
                         int         inputLength)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(input);
    BSLS_ASSERT(0 <= inputLength);

    enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = -1 };

    // Sample XML time: "08:59:59.999".
    // Also, there might be more than 3 decimal places for the fraction of a
    // second.  But when storing in bdet_Datetime, we only take the 3 most
    // significant digits.

    const char *begin = input;
    const char *end   = input + inputLength;

    // Parse time.

    int hour, minute, second, millisecond;

    if (0 != parseTime(&hour, &minute, &second, &millisecond, &begin, end)) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    int timezoneOffset = 0;  // minutes from GMT

    if (end != begin) {
        // Parse timezone.
        if (0   != parseTimezoneOffset(&timezoneOffset, &begin, end)
         || end != begin) {
            return BDEPU_FAILURE;                                     // RETURN
        }
    }

    // Milliseconds could be 1000 (if fraction is .9995 or greater).  Thus, we
    // have to add it after setting the time else it might not validate.

    bdet_Time localTime;
    if (localTime.setTimeIfValid(hour, minute, second)) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    // 'addTime' and/or 'addMinutes' will reset '24:00:00' to '00:00:00' (even
    // if the quantities added are 0), which we don't want to happen.
    // 'hours == 24' is only allowed for the value '24:00:00.000' with timezone
    // GMT.

    if (millisecond || timezoneOffset) {
        if (24 == hour) {
            return BDEPU_FAILURE;                                     // RETURN
        }
        localTime.addMilliseconds(millisecond);
        localTime.addMinutes(-timezoneOffset);  // convert to GMT
    }

    *result = localTime;
    return BDEPU_SUCCESS;
}

int bdepu_Iso8601::parse(bdet_TimeTz *result,
                         const char  *input,
                         int          inputLength)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(input);
    BSLS_ASSERT(0 <= inputLength);

    enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = -1 };

    // Sample XML time: "08:59:59.999".
    // Also, there might be more than 3 decimal places for the fraction of a
    // second.  But when storing in bdet_Datetime, we only take the 3 most
    // significant digits.

    const char *begin = input;
    const char *end   = input + inputLength;

    // Parse time.

    int hour, minute, second, millisecond;

    if (0 != parseTime(&hour, &minute, &second, &millisecond, &begin, end)) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    int timezoneOffset = 0;  // minutes from GMT

    if (end != begin) {
        // Parse timezone.
        if (0   != parseTimezoneOffset(&timezoneOffset, &begin, end)
         || end != begin) {
            return BDEPU_FAILURE;                                     // RETURN
        }
    }

    // Milliseconds could be 1000 (if fraction is .9995 or greater).  Thus, we
    // have to add it after setting the time else it might not validate.

    bdet_Time localTime;
    if (localTime.setTimeIfValid(hour, minute, second)) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    // 'addMilliseconds' will reset '24:00:00' to '00:00:00' (even if the
    // quantity added is 0), which we don't want to happen.  'hours == 24' is
    // only allowed for the value '24:00:00.000' and timezone GMT.

    if (millisecond || timezoneOffset) {
        if (24 == hour) {
            return BDEPU_FAILURE;                                     // RETURN
        }
        localTime.addMilliseconds(millisecond);
    }

    result->setTimeTz(localTime, timezoneOffset);

    return BDEPU_SUCCESS;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
