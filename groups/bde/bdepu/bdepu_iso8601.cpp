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

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS

static
int parseUint(const char **nextPos,
              int         *result,
              const char  *begin,
              const char  *end)
{
    BSLS_ASSERT(nextPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);

    static const int maxDigits = 15;

    // Stop parsing at 'maxDigits'.
    if (end - begin > maxDigits) {
        end = begin + maxDigits + 1;  // Parse one extra digit.
    }

    char buf[maxDigits + 2];  // Make room for one extra digit.
    char *bufPtr = buf;
    const char *inPtr = begin;
    while (inPtr < end && bdeu_CharType::isDigit(*inPtr)) {
        *bufPtr++ = *inPtr++;
    }

    int parsedLen = inPtr - begin;
    if (0 == parsedLen || parsedLen > maxDigits) {
        // Parsed too few or too many digits.
        return -1;                                                    // RETURN
    }

    *nextPos = inPtr;
    *bufPtr = '\0';
    *result = bsl::atoi(buf);
    return 0;
}

static
int parseDate(int         *year,
              int         *month,
              int         *day,
              const char **begin,
              const char  *end)
{
    BSLS_ASSERT(year);
    BSLS_ASSERT(month);
    BSLS_ASSERT(day);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);

    enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = -1 };

    const char *p = *begin;

    // Parse year.

    if    (0   != parseUint(&p, year, p, end)
       ||  p   >= end
       ||  '-' != *p) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    ++p;  // skip hyphen

    // Parse month.

    const char *expectedEnd = p + 2;

    if (0   != parseUint(&p, month, p, end)
     || p   >= end
     || p   != expectedEnd
     || '-' != *p) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    ++p;  // skip hyphen

    // Parse day.

    expectedEnd = p + 2;

    if   (0 != parseUint(&p, day, p, end)
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

    if   (0 != parseUint(&p, second, p, end)
       || p != expectedEnd) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    // Parse millisecond.

    *millisecond = 0;

    if (end != p && '.' == *p) {
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

        for (int i = 0; i < 4 && p != end && bdeu_CharType::isDigit(*p); ++i) {
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
        while (p != end && bdeu_CharType::isDigit(*p)) {
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
{
    BSLS_ASSERT(minuteOffset);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);

    enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = -1 };

    const char *p = *begin;

    if (p >= end) {
        return BDEPU_FAILURE;
    }

    int sign;

    if ('+' == *p) {
        sign = +1;
    }
    else if ('-' == *p) {
        sign = -1;
    }
    else if ('Z' == *p || 'z' == *p) {
        *minuteOffset = 0;

        *begin = (*begin) + 1;

        return BDEPU_SUCCESS;                                         // RETURN
    }
    else {
        return BDEPU_FAILURE;                                         // RETURN
    }

    ++p;  // skip sign

    const char *expectedEnd = p + 2;

    int hourVal;
    if   (0           != parseUint(&p, &hourVal, p, end)
       || p           != expectedEnd
       || hourVal     >  14  // Max TZ offset is 14 hours.
       || p           >= end
       || ':' != *p) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    ++p;  // skip ':'

    expectedEnd = p + 2;

    int minuteVal;
    if   (0           != parseUint(&p, &minuteVal, p, end)
       || p           != expectedEnd
       || minuteVal   >  59) {
        return BDEPU_FAILURE;                                         // RETURN
    }

    *minuteOffset = sign * (hourVal * 60 + minuteVal);

    *begin = p;

    return BDEPU_SUCCESS;
}

static
char *generateInt(char *buffer, int val, int len)
    // Write into the specified 'buffer', the decimal value of the specified
    // 'val', left-padded with zeros to the specified 'len' and return
    // 'buffer + len'.  The buffer is NOT null-terminated.
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

    // TZ offset cannot be more than 14 hours.
    BSLS_ASSERT(timezoneOffset <= 14 * 60);

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

    // TZ offset cannot be more than 14 hours.
    BSLS_ASSERT(timezoneOffset <= 14 * 60);

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

    // TZ offset cannot be more than 14 hours.
    BSLS_ASSERT(timezoneOffset <= 14 * 60);

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
