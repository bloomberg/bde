// bdlt_iso8601util.cpp                                               -*-C++-*-
#include <bdlt_iso8601util.h>

#include <bdlt_datetimeinterval.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_iso8601util_cpp,"$Id$ $CSID$")



#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_cstring.h>

namespace {
namespace u {

using namespace BloombergLP;
using namespace BloombergLP::bdlt;

                          // ======================
                          // class Iso8601Util_Impl
                          // ======================

class Impl {
    // This 'class' is private to this component and not to be called from
    // outside it.

    // PRIVATE TYPES
    typedef BloombergLP::bdlt::Iso8601Util Util;

    template <class TYPE>
    struct IsString {
        static const bool value = bsl::is_same<TYPE, bsl::string>::value
                               || bsl::is_same<TYPE, std::string>::value
    #ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                               || bsl::is_same<TYPE, std::pmr::string>::value
    #endif
        ;
    };

  public:
    // CLASS METHODS
    template <class STRING>
    static int generate(STRING                          *string,
                        const bsls::TimeInterval&        object,
                        const Iso8601UtilConfiguration&  configuration);
    template <class STRING>
    static int generate(STRING                          *string,
                        const Date&                      object,
                        const Iso8601UtilConfiguration&  configuration);
    template <class STRING>
    static int generate(STRING                          *string,
                        const Time&                      object,
                        const Iso8601UtilConfiguration&  configuration);
    template <class STRING>
    static int generate(STRING                          *string,
                        const Datetime&                  object,
                        const Iso8601UtilConfiguration&  configuration);
    template <class STRING>
    static int generate(STRING                          *string,
                        const DateTz&                    object,
                        const Iso8601UtilConfiguration&  configuration);
    template <class STRING>
    static int generate(STRING                          *string,
                        const TimeTz&                    object,
                        const Iso8601UtilConfiguration&  configuration);
    template <class STRING>
    static int generate(STRING                          *string,
                        const DatetimeTz&                object,
                        const Iso8601UtilConfiguration&  configuration);
    template <class STRING>
    static int generate(STRING                          *string,
                        const Util::DateOrDateTz&        object,
                        const Iso8601UtilConfiguration&  configuration);
    template <class STRING>
    static int generate(STRING                          *string,
                        const Util::TimeOrTimeTz&        object,
                        const Iso8601UtilConfiguration&  configuration);
    template <class STRING>
    static int generate(STRING                            *string,
                        const Util::DatetimeOrDatetimeTz&  object,
                        const Iso8601UtilConfiguration&    configuration);
        // Load the ISO 8601 representation of the specified 'object' into the
        // specified 'string'.  Optionally specify a 'configuration' to affect
        // the format of the generated string.  If 'configuration' is not
        // supplied, the process-wide default value
        // 'Iso8601UtilConfiguration::defaultConfiguration()' is used.
        // 'STRING' must be 'bsl::string', 'std::string', or
        // 'std::pmr::string'.  Return the number of characters in the
        // formatted string.  The previous contents of 'string' (if any) are
        // discarded.
};

                          // -----------------------
                          // struct Iso8601Util_Impl
                          // -----------------------

template <class STRING>
inline
int Impl::generate(STRING                          *string,
                   const bsls::TimeInterval&        object,
                   const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(string);

    BSLMF_ASSERT(IsString<STRING>::value);

    string->resize(Util::k_TIMEINTERVAL_STRLEN);

    const int len = Util::generateRaw(&(*string)[0], object, configuration);
    BSLS_ASSERT(Util::k_TIMEINTERVAL_STRLEN >= len);

    string->resize(len);

    return len;
}

template <class STRING>
inline
int Impl::generate(STRING                          *string,
                   const Date&                      object,
                   const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(string);

    BSLMF_ASSERT(IsString<STRING>::value);

    string->resize(Util::k_DATE_STRLEN);

    const int len = Util::generateRaw(&(*string)[0], object, configuration);
    BSLS_ASSERT(Util::k_DATE_STRLEN >= len);

    string->resize(len);

    return len;
}

template <class STRING>
inline
int Impl::generate(STRING                          *string,
                   const Time&                      object,
                   const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(string);

    BSLMF_ASSERT(IsString<STRING>::value);

    string->resize(Util::k_TIME_STRLEN);

    const int len = Util::generateRaw(&(*string)[0], object, configuration);

    BSLS_ASSERT(Util::k_TIME_STRLEN >= len);

    string->resize(len);

    return len;
}

template <class STRING>
inline
int Impl::generate(STRING                          *string,
                   const Datetime&                  object,
                   const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(string);

    BSLMF_ASSERT(IsString<STRING>::value);

    string->resize(Util::k_DATETIME_STRLEN);

    const int len = Util::generateRaw(&(*string)[0], object, configuration);

    BSLS_ASSERT(Util::k_DATETIME_STRLEN >= len);

    string->resize(len);

    return len;
}

template <class STRING>
inline
int Impl::generate(STRING                          *string,
                   const DateTz&                    object,
                   const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(string);

    BSLMF_ASSERT(IsString<STRING>::value);

    string->resize(Util::k_DATETZ_STRLEN);

    const int len = Util::generateRaw(&(*string)[0], object, configuration);

    BSLS_ASSERT(Util::k_DATETZ_STRLEN >= len);

    string->resize(len);

    return len;
}

template <class STRING>
inline
int Impl::generate(STRING                          *string,
                   const TimeTz&                    object,
                   const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(string);

    BSLMF_ASSERT(IsString<STRING>::value);

    string->resize(Util::k_TIMETZ_STRLEN);

    const int len = Util::generateRaw(&(*string)[0], object, configuration);

    BSLS_ASSERT(Util::k_TIMETZ_STRLEN >= len);

    string->resize(len);

    return len;
}

template <class STRING>
inline
int Impl::generate(STRING                          *string,
                   const DatetimeTz&                object,
                   const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(string);

    BSLMF_ASSERT(IsString<STRING>::value);

    string->resize(Util::k_DATETIMETZ_STRLEN);

    const int len = Util::generateRaw(&(*string)[0], object, configuration);

    BSLS_ASSERT(Util::k_DATETIMETZ_STRLEN >= len);

    string->resize(len);

    return len;
}

template <class STRING>
inline
int Impl::generate(STRING                          *string,
                   const Util::DateOrDateTz&        object,
                   const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(string);

    BSLMF_ASSERT(IsString<STRING>::value);

    string->resize(Util::k_DATETZ_STRLEN);

    const int len = Util::generateRaw(&(*string)[0], object, configuration);

    BSLS_ASSERT(Util::k_DATETZ_STRLEN >= len);

    string->resize(len);

    return len;
}

template <class STRING>
inline
int Impl::generate(STRING                          *string,
                   const Util::TimeOrTimeTz&        object,
                   const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(string);

    BSLMF_ASSERT(IsString<STRING>::value);

    string->resize(Util::k_TIMETZ_STRLEN);

    const int len = Util::generateRaw(&(*string)[0], object, configuration);

    BSLS_ASSERT(Util::k_TIMETZ_STRLEN >= len);

    string->resize(len);

    return len;
}

template <class STRING>
inline
int Impl::generate(STRING                            *string,
                   const Util::DatetimeOrDatetimeTz&  object,
                   const Iso8601UtilConfiguration&    configuration)
{
    BSLS_ASSERT(string);

    BSLMF_ASSERT(IsString<STRING>::value);

    string->resize(Util::k_DATETIMETZ_STRLEN);

    const int len = Util::generateRaw(&(*string)[0], object, configuration);

    BSLS_ASSERT(Util::k_DATETIMETZ_STRLEN >= len);

    string->resize(len);

    return len;
}

int asciiPrefixToInt64(const char         **nextPos,
                       bsls::Types::Int64  *result,
                       const char          *begin,
                       const char          *end)
    // Convert the (unsigned) ASCII decimal integer starting at the specified
    // 'begin' and ending anywhere before the specified 'end' into its
    // corresponding 'int' value, load the value into the specified 'result',
    // and set the specified '*nextPos' to the address immediately after the
    // last digit parsed.  Return 0 if there was at least one digit parsed, and
    // a non-zero value (with no effect) otherwise.  The behavior is undefined
    // unless 'begin < end' and the parsed value does not exceed the maximum
    // value allowed in a value of type 'bsls::Types::Int64'.
{
    BSLS_ASSERT(nextPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(begin < end);

    bsls::Types::Int64  tmp      = 0;
    const char         *position = begin;

    while (position < end && isdigit(*position)) {
        tmp *= 10;
        tmp += *position - '0';

        ++position;
    }

    if (position == begin) {
        return -1;                                                    // RETURN
    }

    *result  = tmp;
    *nextPos = position;

    return 0;
}

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
    // unless 'begin < end' and the parsed value does not exceed 'INT_MAX'.
{
    BSLS_ASSERT(nextPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(begin < end);

    const char         *tmpNext = 0;
    bsls::Types::Int64  tmp     = 0;

    if (0 != u::asciiPrefixToInt64(&tmpNext, &tmp, begin, end)
        || tmpNext != end) {
        return -1;                                                    // RETURN
    }

    *result  = static_cast<int>(tmp);
    *nextPos = end;

    return 0;
}

int parseDateRaw(const char **nextPos,
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

    if (0 != u::asciiToInt(&p, year, p, p + 4) || '-' != *p) {
        return -1;                                                    // RETURN
    }
    ++p;  // skip '-'

    // 2. Parse month.

    if (0 != u::asciiToInt(&p, month, p, p + 2) || '-' != *p) {
        return -1;                                                    // RETURN
    }
    ++p;  // skip '-'

    // 3. Parse day.

    if (0 != u::asciiToInt(&p, day, p, p + 2)) {
        return -1;                                                    // RETURN
    }

    *nextPos = p;

    return 0;
}

int parseFractionalSecond(const char         **nextPos,
                          bsls::Types::Int64  *nanosecond,
                          const char          *begin,
                          const char          *end,
                          int                  roundNanoseconds)
    // Parse the fractional second starting at the specified 'begin' and ending
    // before the specified 'end', load into the specified 'nanosecond' the
    // parsed value (in nanoseconds) rounded to the closest multiple of the
    // specified 'roundNanoseconds', and set the specified '*nextPos' to the
    // location one past the last parsed character (necessarily a decimal
    // digit).  Return 0 on success, and a non-zero value (with no effect)
    // otherwise.  There must be at least one digit, only the first ten digits
    // are significant, and all digits beyond the first ten are parsed but
    // ignored.  The behavior is undefined unless 'begin <= end' and
    // '0 <= roundNanoseconds < 1000000000'.  Note that successfully parsing a
    // fractional second before 'end' is reached is not an error.
{
    BSLS_ASSERT(nextPos);
    BSLS_ASSERT(nanosecond);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(begin <= end);
    BSLS_ASSERT(0     <= roundNanoseconds);
    BSLS_ASSERT(         roundNanoseconds < 1000000000);

    const char *p = begin;

    // There must be at least one digit.

    if (p == end || !isdigit(*p)) {
        return -1;                                                    // RETURN
    }

    const char *endSignificant = bsl::min(end, p + 10);

    bsls::Types::Int64 tmp    = 0;
    bsls::Types::Int64 factor = 10000000000LL;
                            // Since the result is in nanoseconds, we have to
                            // adjust it according to how many digits are
                            // present.

    do {
        tmp    *= 10;
        tmp    += *p - '0';
        factor /= 10;
    } while (++p < endSignificant && isdigit(*p));

    tmp *= factor;

    // round

    tmp = (tmp + roundNanoseconds * 5) / (roundNanoseconds * 10)
                                                           * roundNanoseconds;

    // Skip and ignore all digits beyond the first 10, if any.

    while (p < end && isdigit(*p)) {
        ++p;
    }

    *nextPos     = p;
    *nanosecond = tmp;

    return 0;
}

int parseTimeRaw(const char         **nextPos,
                 int                 *hour,
                 int                 *minute,
                 int                 *second,
                 int                 *millisecond,
                 bsls::Types::Int64  *microsecond,
                 bool                *hasLeapSecond,
                 const char          *begin,
                 const char          *end,
                 int                  roundMicroseconds)
    // Parse the time, represented in the "hh:mm:ss[.s+]" ISO 8601 extended
    // format, from the string starting at the specified 'begin' and ending
    // before the specified 'end', load into the specified 'hour', 'minute',
    // 'second', 'millisecond', and 'microsecond' their respective parsed
    // values with the fractional second rounded to the closest multiple of the
    // specified 'roundMicroseconds', set the specified 'hasLeapSecond' flag to
    // 'true' if a leap second was indicated and 'false' otherwise, and set the
    // specified '*nextPos' to the location one past the last parsed character.
    // Return 0 on success, and a non-zero value (with no effect on '*nextPos')
    // otherwise.  The behavior is undefined unless 'begin <= end' and
    // '0 <= roundMicroseconds < 1000000'.  Note that successfully parsing a
    // time before 'end' is reached is not an error.
{
    BSLS_ASSERT(nextPos);
    BSLS_ASSERT(hour);
    BSLS_ASSERT(minute);
    BSLS_ASSERT(second);
    BSLS_ASSERT(millisecond);
    BSLS_ASSERT(microsecond);
    BSLS_ASSERT(hasLeapSecond);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(end);
    BSLS_ASSERT(begin <= end);
    BSLS_ASSERT(0     <= roundMicroseconds);
    BSLS_ASSERT(         roundMicroseconds < 1000000);

    const char *p = begin;

    enum { k_MINIMUM_LENGTH = sizeof "hh:mm:ss" - 1 };

    if (end - p < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    // 1. Parse hour.

    if (0 != u::asciiToInt(&p, hour, p, p + 2) || ':' != *p) {
        return -1;                                                    // RETURN
    }
    ++p;  // skip ':'

    // 2. Parse minute.

    if (0 != u::asciiToInt(&p, minute, p, p + 2) || ':' != *p) {
        return -1;                                                    // RETURN
    }
    ++p;  // skip ':'

    // 3. Parse second.

    if (0 != u::asciiToInt(&p, second, p, p + 2)) {
        return -1;                                                    // RETURN
    }

    // 4. Parse (optional) fractional second, in microseconds.

    if (p < end && ('.' == *p || ',' == *p)) {
        // We have a fraction of a second.

        ++p;  // skip '.' or ','

        bsls::Types::Int64 nanosecond;
        if (0 != u::parseFractionalSecond(&p,
                                          &nanosecond,
                                          p,
                                          end,
                                          roundMicroseconds * 1000)) {
            return -1;                                                // RETURN
        }
        *microsecond = nanosecond / 1000;
        *millisecond = static_cast<int>(*microsecond / 1000);
        *microsecond %= 1000;
    }
    else {
        *millisecond = 0;
        *microsecond = 0;
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

int parseZoneDesignator(const char **nextPos,
                        int         *minuteOffset,
                        const char  *begin,
                        const char  *end)
    // Parse the zone designator, represented in the "Z|(+|-)hh:mm" ISO 8601
    // extended format, from the string starting at the specified 'begin' and
    // ending before the specified 'end', load into the specified
    // 'minuteOffset' the indicated offset (in minutes) from UTC, and set the
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

    const char sign = *p++;  // store and skip '(+|-|Z)'

    if ('Z' == sign || 'z' == sign) {
        *minuteOffset      = 0;
        *nextPos           = p;
        return 0;                                                     // RETURN
    }

    enum { k_MINIMUM_LENGTH = sizeof "hhmm" - 1 };

    if (('+' != sign && '-' != sign) || end - p < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    // We have parsed a '+' or '-' and established that there are sufficient
    // characters to represent "hhmm" (but not necessarily "hh:mm").

    // Parse hour.

    int hour;

    if (0 != u::asciiToInt(&p, &hour, p, p + 2) || hour >= 24) {
        return -1;                                                    // RETURN
    }
    if (':' == *p) {
        ++p;  // skip optional ':'
        if (end - p < 2) {
            return -1;                                                // RETURN
        }
    }

    // Parse minute.

    int minute;

    if (0 != u::asciiToInt(&p, &minute, p, p + 2) || minute > 59) {
        return -1;                                                    // RETURN
    }

    *minuteOffset = hour * 60 + minute;

    if ('-' == sign) {
        *minuteOffset = -*minuteOffset;
    }

    *nextPos = p;

    return 0;
}

int parseDate(Date       *date,
              int        *tzOffset,
              bool       *hasZoneDesignator,
              const char *string,
              int         length)
    // Parse the specified initial 'length' characters of the specified ISO
    // 8601 'string' as a 'Date' value, and load the value into the specified
    // 'date'.  If zone designator is presented in the input, load into the
    // specified 'tzOffset' the indicated offset (in minutes) from UTC and set
    // the variable pointed by the specified 'hasZoneDesignator' to 'true'.
    // Return 0 on success, and a non-zero value (with no effect on the 'date')
    // otherwise.
{
    // Sample ISO 8601 date: "2005-01-31+04:00"
    //
    // The zone designator is optional.

    enum { k_MINIMUM_LENGTH = sizeof "YYYY-MM-DD" - 1 };

    if (length < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    const char *shuttle = string;
    const char *end     = string + length;

    // 1. Parse and validate date.

    int year, month, day;

    if (0 != u::parseDateRaw(&shuttle, &year, &month, &day, shuttle, end)
     || !Date::isValidYearMonthDay(year, month, day)) {
        return -1;                                                    // RETURN
    }

    // 2. Parse and ignore zone designator, if any.

    *tzOffset          = 0;
    *hasZoneDesignator = false;

    if (shuttle != end) {
        *hasZoneDesignator = true;
        if (0 != u::parseZoneDesignator(&shuttle,
                                        tzOffset,
                                        shuttle,
                                        end)
          || shuttle != end) {
            return -1;                                                // RETURN
        }

    }

    date->setYearMonthDay(year, month, day);

    return 0;
}

int parseTime(Time       *time,
              int        *tzOffset,
              bool       *hasZoneDesignator,
              const char *string,
              int         length)
    // Parse the specified initial 'length' characters of the specified ISO
    // 8601 'string' as a 'Time' value, and load the value into the specified
    // 'time'.  If zone designator is presented in the input, load into the
    // specified 'tzOffset' the indicated offset (in minutes) from UTC and set
    // the variable pointed by the specified 'hasZoneDesignator' to 'true'.
    // Return 0 on success, and a non-zero value (with no effect on the 'time')
    // otherwise.
{
    // Sample ISO 8601 time: "08:59:59.999999-04:00"
    //
    // The fractional second and zone designator are independently optional.

    enum { k_MINIMUM_LENGTH = sizeof "hh:mm:ss" - 1 };

    if (length < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    const char *shuttle = string;
    const char *end     = string + length;

    // 1. Parse and validate time.

    // Milliseconds could be rounded to 1000 (if fractional second is .9999995
    // or greater).  Thus, we have to add it after setting the time, else it
    // might not validate.

    int                hour          = 0;
    int                minute        = 0;
    int                second        = 0;
    int                millisecond   = 0;
    bsls::Types::Int64 microsecond   = 0;
    bool               hasLeapSecond = false;

    if (0 != u::parseTimeRaw(&shuttle,
                             &hour,
                             &minute,
                             &second,
                             &millisecond,
                             &microsecond,
                             &hasLeapSecond,
                             shuttle,
                             end,
                             1)
     || 0 != time->setTimeIfValid(hour, minute, second)) {
        return -1;                                                    // RETURN
    }

    if (hasLeapSecond) {
        time->addSeconds(1);
    }

    if (millisecond) {
        time->addMilliseconds(millisecond);
    }

    if (microsecond) {
        time->addMicroseconds(microsecond);
    }

    // 2. Parse zone designator, if any.

    *tzOffset          = 0;
    *hasZoneDesignator = false;

    if (shuttle != end) {
        *hasZoneDesignator = true;
        if (0 != u::parseZoneDesignator(&shuttle,
                                        tzOffset,
                                        shuttle,
                                        end)
         || shuttle != end) {
            return -1;                                                // RETURN
        }
    }

    // '24 == hour' is allowed only for the value '24:00:00.000000' in UTC.
    // The case where '0 != minute || 0 != second' is caught by
    // 'setTimeIfValid' (above).

    if (24 == hour && (millisecond || microsecond || *tzOffset)) {
        return -1;                                                    // RETURN
    }

    return 0;
}

int parseDatetime(Datetime   *datetime,
                  int        *tzOffset,
                  bool       *hasZoneDesignator,
                  const char *string,
                  int         length,
                  bool        allowSpaceInsteadOfT = false)
    // Parse the specified initial 'length' characters of the specified ISO
    // 8601 'string' as a 'Datetime' value, and load the value into the
    // specified 'datetime'.  If zone designator is presented in the input,
    // load into the specified 'tzOffset' the indicated offset (in minutes)
    // from UTC and set the variable pointed by the specified
    // 'hasZoneDesignator' to 'true'.  Optionally specify a
    // 'allowSpaceInsteadOfT' to allow usage of a SPACE character instead of
    // 'T'.  Return 0 on success, and a non-zero value (with no effect on the
    // 'datetime') otherwise.
{

    // Sample ISO 8601 datetime: "2005-01-31T08:59:59.999999-04:00"
    //
    // The fractional second and zone designator are independently optional.

    enum { k_MINIMUM_LENGTH = sizeof "YYYY-MM-DDThh:mm:ss" - 1 };

    if (length < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    const char *shuttle = string;
    const char *end     = string + length;

    // 1. Parse date.

    int year  = 0;
    int month = 0;
    int day   = 0;

    if (0 != u::parseDateRaw(&shuttle, &year, &month, &day, shuttle, end)) {
        return -1;                                                    // RETURN
    }
    if (shuttle == end || !('T' == *shuttle || 't' == *shuttle ||
                                  (allowSpaceInsteadOfT && ' ' == *shuttle))) {
        // If 'shuttle' is not a valid date-time separator.
        return -1;                                                    // RETURN
    }
    ++shuttle;  // skip 'T', 't' or ' '

    // 2. Parse time.

    int                hour          = 0;
    int                minute        = 0;
    int                second        = 0;
    int                millisecond   = 0;
    bsls::Types::Int64 microsecond   = 0;
    bool               hasLeapSecond = false;

    if (0 != u::parseTimeRaw(&shuttle,
                             &hour,
                             &minute,
                             &second,
                             &millisecond,
                             &microsecond,
                             &hasLeapSecond,
                             shuttle,
                             end,
                             1)) {
        return -1;                                                    // RETURN
    }

    // 3. Parse zone designator, if any.

    *tzOffset          = 0;
    *hasZoneDesignator = false;

    if (shuttle != end) {
        *hasZoneDesignator = true;
        if (0 != u::parseZoneDesignator(
                     &shuttle, tzOffset, shuttle, end) ||
            shuttle != end) {
            return -1;                                                // RETURN
        }
    }

    // 4. Account for special ISO 8601 values.

    ///Leap Seconds and Maximum Fractional Seconds
    ///- - - - - - - - - - - - - - - - - - - - - -
    // Note that leap seconds or 'millisecond' values of 1000 (which result
    // from rounding up a fractional second that is .9999995 or greater) cannot
    // be directly represented with a 'Datetime'.  Hence, we create an initial
    // 'Datetime' object without accounting for these, then adjust it forward
    // by up to 2 seconds, as needed.
    //
    ///24:00
    ///- - -
    // An 'hour' value of 24 is not valid unless 'minute', 'second',
    // 'millisecond', and 'microsecond' are all 0.  Further note that supplying
    // an hour of 24 when constructing a 'Datetime' results in a different
    // interpretation than that provided by ISO 8601 (see
    // '{Note Regarding the Time 24:00}' in the component-level documentation).

    if (24 == hour) {
        // '24 == hour' is allowed only for the value '24:00:00.000000' in UTC.

        if (minute || second || millisecond || microsecond || *tzOffset) {
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

    if (0 != datetime->setDatetimeIfValid(year,
                                          month,
                                          day,
                                          hour,
                                          minute,
                                          second,
                                          millisecond,
                                          static_cast<int>(microsecond))) {
        return -1;                                                    // RETURN
    }

    // 6. Apply adjustments for special ISO 8601 values.

    if (DatetimeInterval() != resultAdjustment) {

        BSLS_ASSERT(resultAdjustment > DatetimeInterval());
        // We assert the above to prevent future developers from accidentally
        // introducing negative adjustments, which are not handled by the
        // following logic.

        const Datetime maxDatetime(9999, 12, 31, 23, 59, 59, 999, 999);

        if (maxDatetime - resultAdjustment < *datetime) {
            return -1;                                                // RETURN
        }

        *datetime += resultAdjustment;
    }

    return 0;
}

int parseImp(Datetime   *result,
             const char *string,
             int         length,
             bool        allowSpaceInsteadOfT = false)
{
    // Sample ISO 8601 datetime: "2005-01-31T08:59:59.999999-04:00"
    //
    // The fractional second and zone designator are independently optional.

    // 1. Parse as a 'DatetimeTz'.

    Datetime localDatetime;
    int      tzOffset          = 0;      // minutes from UTC
    bool     hasZoneDesignator = false;

    if (0 != u::parseDatetime(&localDatetime,
                              &tzOffset,
                              &hasZoneDesignator,
                              string,
                              length,
                              allowSpaceInsteadOfT)) {
        return -1;                                                    // RETURN
    }

    // 2. Account for edge cases.

    if (tzOffset > 0) {
        Datetime minDatetime(0001, 01, 01, 00, 00, 00, 000, 000);

        minDatetime.addMinutes(tzOffset);

        if (minDatetime > localDatetime) {
            return -1;                                                // RETURN
        }
    }
    else if (tzOffset < 0) {
        Datetime maxDatetime(9999, 12, 31, 23, 59, 59, 999, 999);

        maxDatetime.addMinutes(tzOffset);

        if (maxDatetime < localDatetime) {
            return -1;                                                // RETURN
        }
    }

    if (tzOffset) {
         localDatetime.addMinutes(-tzOffset);
    }

    *result = localDatetime;

    return 0;
}

int parseImp(DatetimeTz *result,
             const char *string,
             int         length,
             bool        allowSpaceInsteadOfT = false)
{
    Datetime localDatetime;
    int      tzOffset          = 0;      // minutes from UTC
    bool     hasZoneDesignator = false;

    if (0 != u::parseDatetime(&localDatetime,
                              &tzOffset,
                              &hasZoneDesignator,
                              string,
                              length,
                              allowSpaceInsteadOfT)) {
        return -1;                                                    // RETURN
    }

    result->setDatetimeTz(localDatetime, tzOffset);

    return 0;
}

int parseImp(Iso8601Util::DatetimeOrDatetimeTz *result,
             const char                        *string,
             int                                length,
             bool                               allowSpaceInsteadOfT = false)
{
    Datetime localDatetime;
    int      tzOffset          = 0;      // minutes from UTC
    bool     hasZoneDesignator = false;

    if (0 != u::parseDatetime(&localDatetime,
                              &tzOffset,
                              &hasZoneDesignator,
                              string,
                              length,
                              allowSpaceInsteadOfT)) {
        return -1;                                                    // RETURN
    }

    if (hasZoneDesignator) {
        result->createInPlace<DatetimeTz>(localDatetime, tzOffset);
    }
    else {
        result->assign<Datetime>(localDatetime);
    }

    return 0;
}

int generateUnpaddedInt(char *buffer, bsls::Types::Int64 value)
    // Write, to the specified 'buffer', the decimal string representation of
    // the specified 'value' and return the number of bytes written.  'buffer'
    // is NOT null-terminated.  The behavior is undefined unless '0 <= value'
    // and 'buffer' has sufficient capacity.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= value);

    char *current = buffer;

    while (value != 0) {
        *current = static_cast<char>('0' + value % 10);
        value /= 10;
        ++current;
    }
    bsl::reverse(buffer, current);

    return static_cast<int>(current - buffer);
}

int generateUnpaddedInt(char *buffer, bsls::Types::Int64 value, char separator)
    // Write, to the specified 'buffer', the decimal string representation of
    // the specified 'value' followed by the specified 'separator' character,
    // and return the number of bytes written.  'buffer' is NOT null-
    // terminated.  The behavior is undefined unless '0 <= value' and 'buffer'
    // has sufficient capacity.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= value);

    int separatorOffset = u::generateUnpaddedInt(buffer, value);

    if (0 != separatorOffset) {
        buffer[separatorOffset] = separator;
        ++separatorOffset;
    }

    return separatorOffset;
}

int generateInt(char *buffer, int value, int paddedLen)
    // Write, to the specified 'buffer', the decimal string representation of
    // the specified 'value' padded with leading zeros to the specified
    // 'paddedLen', and return 'paddedLen'.  'buffer' is NOT null-terminated.
    // The behavior is undefined unless '0 <= value', '0 <= paddedLen', and
    // 'buffer' has sufficient capacity to hold 'paddedLen' characters.  Note
    // that if the decimal string representation of 'value' is more than
    // 'paddedLen' digits, only the low-order 'paddedLen' digits of 'value' are
    // output.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= value);
    BSLS_ASSERT(0 <= paddedLen);

    char *p = buffer + paddedLen;

    while (p > buffer) {
        *--p = static_cast<char>('0' + value % 10);
        value /= 10;
    }

    return paddedLen;
}

inline
int generateInt(char *buffer, int value, int paddedLen, char separator)
    // Write, to the specified 'buffer', the decimal string representation of
    // the specified 'value' padded with leading zeros to the specified
    // 'paddedLen' followed by the specified 'separator' character, and return
    // 'paddedLen + 1'.  'buffer' is NOT null-terminated.  The behavior is
    // undefined unless '0 <= value', '0 <= paddedLen', and 'buffer' has
    // sufficient capacity to hold 'paddedLen' characters.  Note that if the
    // decimal string representation of 'value' is more than 'paddedLen'
    // digits, only the low-order 'paddedLen' digits of 'value' are output.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= value);
    BSLS_ASSERT(0 <= paddedLen);

    buffer += u::generateInt(buffer, value, paddedLen);
    *buffer = separator;

    return paddedLen + 1;
}

int generateZoneDesignator(char                            *buffer,
                           int                              tzOffset,
                           const Iso8601UtilConfiguration&  configuration)
    // Write, to the specified 'buffer', the formatted zone designator
    // indicated by the specified 'tzOffset' and 'configuration', and return
    // the number of bytes written.  The behavior is undefined unless 'buffer'
    // has sufficient capacity and '-(24 * 60) < tzOffset < 24 * 60'.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(-(24 * 60) < tzOffset);
    BSLS_ASSERT(             tzOffset < 24 * 60);

    char *p = buffer;

    if (0 == tzOffset && configuration.useZAbbreviationForUtc()) {
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

        *p++ = tzSign;

        if (configuration.omitColonInZoneDesignator()) {
            p += u::generateInt(p, tzOffset / 60, 2);
        }
        else {
            p += u::generateInt(p, tzOffset / 60, 2, ':');
        }

        p += u::generateInt(p, tzOffset % 60, 2);
    }

    return static_cast<int>(p - buffer);
}

#if defined(BSLS_ASSERT_SAFE_IS_USED)
int generatedLengthForDateTzObject(
                                 int                             defaultLength,
                                 int                             tzOffset,
                                 const Iso8601UtilConfiguration& configuration)
    // Return the number of bytes generated, when the specified 'configuration'
    // is used, for a 'bdlt::DateTz' object having the specified 'tzOffset'
    // whose ISO 8601 representation has the specified 'defaultLength' (in
    // bytes).  The behavior is undefined unless '0 <= defaultLength' and
    // '-(24 * 60) < tzOffset < 24 * 60'.
{
    BSLS_ASSERT_SAFE(0 <= defaultLength);
    BSLS_ASSERT_SAFE(-(24 * 60) < tzOffset);
    BSLS_ASSERT_SAFE(             tzOffset < 24 * 60);

    // Consider only those 'configuration' options that can affect the length
    // of the output.

    if (0 == tzOffset && configuration.useZAbbreviationForUtc()) {
        return defaultLength - static_cast<int>(sizeof "00:00") + 1;  // RETURN
    }

    if (configuration.omitColonInZoneDesignator()) {
        return defaultLength - 1;                                     // RETURN
    }

    return defaultLength;
}

int generatedLengthForDatetimeObject(
                                 int                             defaultLength,
                                 const Iso8601UtilConfiguration& configuration)
    // Return the number of bytes generated, when the specified 'configuration'
    // is used, for a 'bdlt::Datetime' object whose ISO 8601 representation has
    // the specified 'defaultLength' (in bytes).  The behavior is undefined
    // unless '0 <= defaultLength'.
{
    BSLS_ASSERT_SAFE(0 <= defaultLength);

    return defaultLength
         - (6 - configuration.fractionalSecondPrecision())
         - (0 == configuration.fractionalSecondPrecision() ? 1 : 0);
}

int generatedLengthForDatetimeTzObject(
                                 int                             defaultLength,
                                 int                             tzOffset,
                                 const Iso8601UtilConfiguration& configuration)
    // Return the number of bytes generated, when the specified 'configuration'
    // is used, for a 'bdlt::DatetimeTz' object having the specified 'tzOffset'
    // whose ISO 8601 representation has the specified 'defaultLength' (in
    // bytes).  The behavior is undefined unless '0 <= defaultLength' and
    // '-(24 * 60) < tzOffset < 24 * 60'.
{
    BSLS_ASSERT_SAFE(0 <= defaultLength);
    BSLS_ASSERT_SAFE(-(24 * 60) < tzOffset);
    BSLS_ASSERT_SAFE(             tzOffset < 24 * 60);

    // Consider only those 'configuration' options that can affect the length
    // of the output.

    defaultLength = defaultLength
                  - (6 - configuration.fractionalSecondPrecision())
                  - (0 == configuration.fractionalSecondPrecision() ? 1 : 0);

    if (0 == tzOffset && configuration.useZAbbreviationForUtc()) {
        return defaultLength - static_cast<int>(sizeof "00:00") + 1;  // RETURN
    }

    if (configuration.omitColonInZoneDesignator()) {
        return defaultLength - 1;                                     // RETURN
    }

    return defaultLength;
}

int generatedLengthForTimeObject(int                             defaultLength,
                                 const Iso8601UtilConfiguration& configuration)
    // Return the number of bytes generated, when the specified 'configuration'
    // is used, for a 'bdlt::Time' object whose ISO 8601 representation has the
    // specified 'defaultLength' (in bytes).  The behavior is undefined unless
    // '0 <= defaultLength'.
{
    BSLS_ASSERT_SAFE(0 <= defaultLength);

    int precision = configuration.fractionalSecondPrecision();

    return defaultLength - (6 - precision) - (0 == precision ? 1 : 0);
}

int generatedLengthForTimeTzObject(
                                 int                             defaultLength,
                                 int                             tzOffset,
                                 const Iso8601UtilConfiguration& configuration)
    // Return the number of bytes generated, when the specified 'configuration'
    // is used, for a 'bdlt::TimeTz' object having the specified 'tzOffset'
    // whose ISO 8601 representation has the specified 'defaultLength' (in
    // bytes).  The behavior is undefined unless '0 <= defaultLength' and
    // '-(24 * 60) < tzOffset < 24 * 60'.
{
    BSLS_ASSERT_SAFE(0 <= defaultLength);
    BSLS_ASSERT_SAFE(-(24 * 60) < tzOffset);
    BSLS_ASSERT_SAFE(             tzOffset < 24 * 60);

    // Consider only those 'configuration' options that can affect the length
    // of the output.

    int precision = configuration.fractionalSecondPrecision();

    defaultLength = defaultLength - (6 - precision) - (0 == precision ? 1 : 0);

    if (0 == tzOffset && configuration.useZAbbreviationForUtc()) {
        return defaultLength - static_cast<int>(sizeof "00:00") + 1;  // RETURN
    }

    if (configuration.omitColonInZoneDesignator()) {
        return defaultLength - 1;                                     // RETURN
    }

    return defaultLength;
}
#endif

void copyBuf(char *dst, int dstLen, const char *src, int srcLen)
    // Copy, to the specified 'dst' buffer having the specified 'dstLen', the
    // specified initial 'srcLen' characters in the specified 'src' string if
    // 'dstLen >= srcLen', and copy 'dstLen' characters otherwise.  Include a
    // null terminator if and only if 'dstLen > srcLen'.  The behavior is
    // undefined unless '0 <= dstLen' and '0 <= srcLen'.
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

int parseIntervalImpl(bsls::Types::Int64 *weeks,
                      bsls::Types::Int64 *days,
                      bsls::Types::Int64 *hours,
                      bsls::Types::Int64 *minutes,
                      bsls::Types::Int64 *seconds,
                      bsls::Types::Int64 *nanoseconds,
                      const char         *string,
                      int                 length)
    // Parse the specified initial 'length' characters of the specified ISO
    // 8601 'string' and load the values into the specified 'weeks', 'days',
    // 'hours', 'minutes', 'seconds', and 'nanoseconds'.  Nothing is written
    // into a value that does not have a corresponding component in 'string'.
    // Return 0 on success, and a non-zero value (with no effect) otherwise.
    // 'string' is assumed to be of the form:
    //..
    //  P{w+W}{d+D}{T{h+H}{m+M}s+(.|,)s+S}
    //..
    // *Exactly* 'length' characters are parsed; parsing will fail if a proper
    // prefix of 'string' matches the expected format, but the entire 'length'
    // characters do not.  If an optional fractional second having more than
    // nine digits is present in 'string', it is rounded to the nearest value
    // in nanoseconds.  The behavior is undefined unless '0 <= length'.
{
    BSLS_ASSERT(weeks);
    BSLS_ASSERT(days);
    BSLS_ASSERT(hours);
    BSLS_ASSERT(minutes);
    BSLS_ASSERT(seconds);
    BSLS_ASSERT(nanoseconds);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    enum { k_MINIMUM_LENGTH = sizeof "P0Y" - 1 };

    if (length < k_MINIMUM_LENGTH) {
        return -1;                                                    // RETURN
    }

    const char *p   = string;
    const char *end = string + length;

    if ('P' != *p) {
        return -1;                                                    // RETURN
    }
    ++p;

    const struct {
        char                d_terminator;
        bsls::Types::Int64 *d_value;
    } states[] = {
        { 'W', weeks },
        { 'D', days },
        { 'H', hours },
        { 'M', minutes },
        { 'S', seconds }
    };
    const int timeIndex = 2;
    const int stateSize = static_cast<int>(sizeof states / sizeof *states);

    bool               foundT       = false;
    bool               foundDecimal = false;
    bsls::Types::Int64 value        = -1;

    for (int index = 0; index != stateSize; ++index) {
        if (-1 == value && foundDecimal) {
            return -1;                                                // RETURN
        }

        if ('T' == *p) {
            if (foundT) {
                return -1;                                            // RETURN
            }

            foundT = true;
            ++p;

            if (index < timeIndex) {
                index = timeIndex;
            }
        }
        else if (index == timeIndex && !foundT) {
            return -1;                                                // RETURN
        }

        if (-1 == value) {
            if (0 != u::asciiPrefixToInt64(&p, &value, p, end)) {
                return -1;                                            // RETURN
            }
        }

        if ('.' == *p || ',' == *p) {
            ++p;
            if (0 != u::parseFractionalSecond(&p, nanoseconds, p, end, 1)) {
                return -1;                                            // RETURN
            }
            foundDecimal = true;
        }

        if (states[index].d_terminator == *p) {
            if (foundDecimal && 'S' != *p) {
                return -1;                                            // RETURN
            }
            *states[index].d_value = value;
            ++p;

            if (p == end) {
                return 0;                                             // RETURN
            }

            value = -1;
        }

        if (p == end) {
            // If 'value' still contains information we have not yet consumed,
            // it means we never found the corresponding character to say what
            // type of datum this is.  This occurs for strings like "PT1".

            return value == -1 ? 0 : 1;                               // RETURN
        }
    }

    // If we have gotten to this point, there are still characters left over to
    // parse, even though we have processed everything we can.

    return -1;
}

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bdlt {

                            // ------------------
                            // struct Iso8601Util
                            // ------------------

// CLASS METHODS
int Iso8601Util::generate(char                            *buffer,
                          int                              bufferLength,
                          const bsls::TimeInterval&        object,
                          const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    int outLen;

    if (bufferLength >= k_TIMEINTERVAL_STRLEN + 1) {
        outLen = generateRaw(buffer, object, configuration);
        BSLS_ASSERT(outLen <= k_TIMEINTERVAL_STRLEN);

        buffer[outLen] = '\0';
    }
    else {
        char outBuf[k_TIMEINTERVAL_STRLEN];

        outLen = generateRaw(outBuf, object, configuration);
        BSLS_ASSERT(outLen <= k_TIMEINTERVAL_STRLEN);

        bsl::memcpy(buffer, outBuf, bufferLength);
        if (outLen < bufferLength) {
            buffer[outLen] = '\0';
        }
    }

    return outLen;
}

int Iso8601Util::generate(char                            *buffer,
                          int                              bufferLength,
                          const Date&                      object,
                          const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    int outLen;

    if (bufferLength >= k_DATE_STRLEN + 1) {
        outLen = generateRaw(buffer, object, configuration);
        BSLS_ASSERT(outLen == k_DATE_STRLEN);

        buffer[outLen] = '\0';
    }
    else {
        char outBuf[k_DATE_STRLEN];

        outLen = generateRaw(outBuf, object, configuration);
        BSLS_ASSERT(outLen == k_DATE_STRLEN);

        bsl::memcpy(buffer, outBuf, bufferLength);
    }

    return outLen;
}

int Iso8601Util::generate(char                            *buffer,
                          int                              bufferLength,
                          const Time&                      object,
                          const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    int outLen;

    if (bufferLength >= k_TIME_STRLEN) {
        outLen = generateRaw(buffer, object, configuration);
    }
    else {
        char outBuf[k_TIME_STRLEN];

        outLen = generateRaw(outBuf, object, configuration);

        bsl::memcpy(buffer, outBuf, bufferLength);
    }

    if (bufferLength > outLen) {
        buffer[outLen] = '\0';
    }

    BSLS_ASSERT_SAFE(outLen == u::generatedLengthForTimeObject(k_TIME_STRLEN,
                                                               configuration));

    return outLen;
}

int Iso8601Util::generate(char                            *buffer,
                          int                              bufferLength,
                          const Datetime&                  object,
                          const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    int outLen;

    if (bufferLength >= k_DATETIME_STRLEN) {
        outLen = generateRaw(buffer, object, configuration);
    }
    else {
        char outBuf[k_DATETIME_STRLEN];

        outLen = generateRaw(outBuf, object, configuration);

        bsl::memcpy(buffer, outBuf, bufferLength);
    }

    if (bufferLength > outLen) {
        buffer[outLen] = '\0';
    }

    BSLS_ASSERT_SAFE(outLen == u::generatedLengthForDatetimeObject(
                                                             k_DATETIME_STRLEN,
                                                             configuration));

    return outLen;
}

int Iso8601Util::generate(char                            *buffer,
                          int                              bufferLength,
                          const DateTz&                    object,
                          const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    int outLen;

    if (bufferLength >= k_DATETZ_STRLEN + 1) {
        outLen = generateRaw(buffer, object, configuration);
        BSLS_ASSERT(outLen <= k_DATETZ_STRLEN);

        buffer[outLen] = '\0';
    }
    else {
        char outBuf[k_DATETZ_STRLEN];

        outLen = generateRaw(outBuf, object, configuration);
        BSLS_ASSERT(outLen <= k_DATETZ_STRLEN);

        u::copyBuf(buffer, bufferLength, outBuf, outLen);
    }

    BSLS_ASSERT_SAFE(outLen == u::generatedLengthForDateTzObject(
                                                               k_DATETZ_STRLEN,
                                                               object.offset(),
                                                               configuration));

    return outLen;
}

int Iso8601Util::generate(char                            *buffer,
                          int                              bufferLength,
                          const TimeTz&                    object,
                          const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    int outLen;

    if (bufferLength >= k_TIMETZ_STRLEN) {
        outLen = generateRaw(buffer, object, configuration);

        BSLS_ASSERT(outLen <= k_TIMETZ_STRLEN);
    }
    else {
        char outBuf[k_TIMETZ_STRLEN];

        outLen = generateRaw(outBuf, object, configuration);

        BSLS_ASSERT(outLen <= k_TIMETZ_STRLEN);

        u::copyBuf(buffer, bufferLength, outBuf, outLen);
    }

    if (bufferLength > outLen) {
        buffer[outLen] = '\0';
    }

    BSLS_ASSERT_SAFE(outLen == u::generatedLengthForTimeTzObject(
                                                               k_TIMETZ_STRLEN,
                                                               object.offset(),
                                                               configuration));

    return outLen;
}

int Iso8601Util::generate(char                            *buffer,
                          int                              bufferLength,
                          const DatetimeTz&                object,
                          const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    int outLen;

    if (bufferLength >= k_DATETIMETZ_STRLEN) {
        outLen = generateRaw(buffer, object, configuration);

        BSLS_ASSERT(outLen <= k_DATETIMETZ_STRLEN);
    }
    else {
        char outBuf[k_DATETIMETZ_STRLEN];

        outLen = generateRaw(outBuf, object, configuration);

        BSLS_ASSERT(outLen <= k_DATETIMETZ_STRLEN);

        u::copyBuf(buffer, bufferLength, outBuf, outLen);
    }

    if (bufferLength > outLen) {
        buffer[outLen] = '\0';
    }

    BSLS_ASSERT_SAFE(outLen ==
                     u::generatedLengthForDatetimeTzObject(k_DATETIMETZ_STRLEN,
                                                           object.offset(),
                                                           configuration));

    return outLen;
}

int Iso8601Util::generate(char                            *buffer,
                          int                              bufferLength,
                          const DateOrDateTz&              object,
                          const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    int outLen;

    if (object.is<Date>()) {
        if (bufferLength >= k_DATE_STRLEN) {
            outLen = generateRaw(buffer, object.the<Date>(), configuration);

            BSLS_ASSERT(outLen <= k_DATE_STRLEN);
        }
        else {
            char outBuf[k_DATE_STRLEN];

            outLen = generateRaw(outBuf, object.the<Date>(), configuration);

            BSLS_ASSERT(outLen <= k_DATE_STRLEN);

            u::copyBuf(buffer, bufferLength, outBuf, outLen);
        }
    }
    else {
        if (bufferLength >= k_DATETZ_STRLEN) {
            outLen = generateRaw(buffer, object.the<DateTz>(), configuration);

            BSLS_ASSERT(outLen <= k_DATETZ_STRLEN);
        }
        else {
            char outBuf[k_DATETZ_STRLEN];

            outLen = generateRaw(outBuf, object.the<DateTz>(), configuration);

            BSLS_ASSERT(outLen <= k_DATETZ_STRLEN);

            u::copyBuf(buffer, bufferLength, outBuf, outLen);
        }

        BSLS_ASSERT_SAFE(outLen == u::generatedLengthForDateTzObject(
                                                 k_DATETZ_STRLEN,
                                                 object.the<DateTz>().offset(),
                                                 configuration));
    }

    if (bufferLength > outLen) {
        buffer[outLen] = '\0';
    }

    return outLen;
}

int Iso8601Util::generate(char                            *buffer,
                          int                              bufferLength,
                          const TimeOrTimeTz&              object,
                          const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    int outLen;

    if (object.is<Time>()) {
        if (bufferLength >= k_TIME_STRLEN) {
            outLen = generateRaw(buffer, object.the<Time>(), configuration);

            BSLS_ASSERT(outLen <= k_TIME_STRLEN);
        }
        else {
            char outBuf[k_TIME_STRLEN];

            outLen = generateRaw(outBuf, object.the<Time>(), configuration);

            BSLS_ASSERT(outLen <= k_TIME_STRLEN);

            u::copyBuf(buffer, bufferLength, outBuf, outLen);
        }

        BSLS_ASSERT_SAFE(outLen == u::generatedLengthForTimeObject(
                                                               k_TIME_STRLEN,
                                                               configuration));
    }
    else {
        if (bufferLength >= k_TIMETZ_STRLEN) {
            outLen = generateRaw(buffer, object.the<TimeTz>(), configuration);

            BSLS_ASSERT(outLen <= k_TIMETZ_STRLEN);
        }
        else {
            char outBuf[k_TIMETZ_STRLEN];

            outLen = generateRaw(outBuf, object.the<TimeTz>(), configuration);

            BSLS_ASSERT(outLen <= k_TIMETZ_STRLEN);

            u::copyBuf(buffer, bufferLength, outBuf, outLen);
        }

        BSLS_ASSERT_SAFE(outLen == u::generatedLengthForTimeTzObject(
                                                 k_TIMETZ_STRLEN,
                                                 object.the<TimeTz>().offset(),
                                                 configuration));
    }

    if (bufferLength > outLen) {
        buffer[outLen] = '\0';
    }

    return outLen;
}

int Iso8601Util::generate(char                            *buffer,
                          int                              bufferLength,
                          const DatetimeOrDatetimeTz&      object,
                          const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= bufferLength);

    int outLen;

    if (object.is<Datetime>()) {
        if (bufferLength >= k_DATETIME_STRLEN) {
            outLen = generateRaw(buffer,
                                 object.the<Datetime>(),
                                 configuration);

            BSLS_ASSERT(outLen <= k_DATETIME_STRLEN);
        }
        else {
            char outBuf[k_DATETIME_STRLEN];

            outLen = generateRaw(outBuf,
                                 object.the<Datetime>(),
                                 configuration);

            BSLS_ASSERT(outLen <= k_DATETIME_STRLEN);

            u::copyBuf(buffer, bufferLength, outBuf, outLen);
        }

        BSLS_ASSERT_SAFE(outLen == u::generatedLengthForDatetimeObject(
                                                             k_DATETIME_STRLEN,
                                                             configuration));
    }
    else {
        if (bufferLength >= k_DATETIMETZ_STRLEN) {
            outLen = generateRaw(buffer,
                                 object.the<DatetimeTz>(),
                                 configuration);

            BSLS_ASSERT(outLen <= k_DATETIMETZ_STRLEN);
        }
        else {
            char outBuf[k_DATETIMETZ_STRLEN];

            outLen = generateRaw(outBuf,
                                 object.the<DatetimeTz>(),
                                 configuration);

            BSLS_ASSERT(outLen <= k_DATETIMETZ_STRLEN);

            u::copyBuf(buffer, bufferLength, outBuf, outLen);
        }

        BSLS_ASSERT_SAFE(outLen == u::generatedLengthForDatetimeTzObject(
                                             k_DATETIMETZ_STRLEN,
                                             object.the<DatetimeTz>().offset(),
                                             configuration));
    }

    if (bufferLength > outLen) {
        buffer[outLen] = '\0';
    }

    return outLen;
}

int Iso8601Util::generate(bsl::string                     *string,
                          const bsls::TimeInterval&        object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(bsl::string                     *string,
                          const Date&                      object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(bsl::string                     *string,
                          const Time&                      object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(bsl::string                     *string,
                          const Datetime&                  object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(bsl::string                     *string,
                          const DateTz&                    object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(bsl::string                     *string,
                          const TimeTz&                    object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(bsl::string                     *string,
                          const DatetimeTz&                object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(bsl::string                     *string,
                          const DateOrDateTz&              object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(bsl::string                     *string,
                          const TimeOrTimeTz&              object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(bsl::string                     *string,
                          const DatetimeOrDatetimeTz&      object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::string                     *string,
                          const bsls::TimeInterval&        object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::string                     *string,
                          const Date&                      object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::string                     *string,
                          const Time&                      object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::string                     *string,
                          const Datetime&                  object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::string                     *string,
                          const DateTz&                    object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::string                     *string,
                          const TimeTz&                    object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::string                     *string,
                          const DatetimeTz&                object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::string                     *string,
                          const DateOrDateTz&              object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::string                     *string,
                          const TimeOrTimeTz&              object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::string                     *string,
                          const DatetimeOrDatetimeTz&      object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int Iso8601Util::generate(std::pmr::string                *string,
                          const bsls::TimeInterval&        object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::pmr::string                *string,
                          const Date&                      object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::pmr::string                *string,
                          const Time&                      object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::pmr::string                *string,
                          const Datetime&                  object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::pmr::string                *string,
                          const DateTz&                    object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::pmr::string                *string,
                          const TimeTz&                    object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::pmr::string                *string,
                          const DatetimeTz&                object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::pmr::string                *string,
                          const DateOrDateTz&              object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::pmr::string                *string,
                          const TimeOrTimeTz&              object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}

int Iso8601Util::generate(std::pmr::string                *string,
                          const DatetimeOrDatetimeTz&      object,
                          const Iso8601UtilConfiguration&  configuration)
{
    return u::Impl::generate(string, object, configuration);
}
#endif

int Iso8601Util::generateRaw(char                            *buffer,
                             const bsls::TimeInterval&        object,
                             const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);

    char *p = buffer;

    bsls::Types::Int64 seconds = object.seconds();
    bsls::Types::Int64 minutes = seconds / 60;
    seconds %= 60;
    bsls::Types::Int64 hours   = minutes / 60;
    minutes %= 60;
    bsls::Types::Int64 days    = hours / 24;
    hours %= 24;
    bsls::Types::Int64 weeks   = days / 7;
    days %= 7;

    *p = 'P';
    ++p;

    p += u::generateUnpaddedInt(p, weeks, 'W');
    p += u::generateUnpaddedInt(p, days,  'D');

    *p = 'T';
    ++p;

    p += u::generateUnpaddedInt(p, hours,   'H');
    p += u::generateUnpaddedInt(p, minutes, 'M');

    // We always include the "seconds" portion of the time to indicate that
    // this is not a reduced-precision representation.

    if (0 == seconds) {
        *p = '0';
        ++p;
    }
    else {
        p += u::generateUnpaddedInt(p, seconds);
    }

    const char decimalSign = configuration.useCommaForDecimalSign()
                             ? ','
                             : '.';

    int precision = configuration.fractionalSecondPrecision();

    if (precision) {
        *p = decimalSign;
        ++p;

        int value = object.nanoseconds();

        for (int i = 9; i > precision; --i) {
            value /= 10;
        }

        p += u::generateInt(p, value, precision, 'S');
    }
    else {
        *p = 'S';
        ++p;
    }

    return static_cast<int>(p - buffer);
}

int Iso8601Util::generateRaw(char                            *buffer,
                             const Date&                      object,
                             const Iso8601UtilConfiguration&)
{
    BSLS_ASSERT(buffer);

    char *p = buffer;

    p += u::generateInt(p, object.year() , 4, '-');
    p += u::generateInt(p, object.month(), 2, '-');
    p += u::generateInt(p, object.day()  , 2     );

    return static_cast<int>(p - buffer);
}

int Iso8601Util::generateRaw(char                            *buffer,
                             const Time&                      object,
                             const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);

    char *p = buffer;

    p += u::generateInt(p, object.hour()       , 2, ':');
    p += u::generateInt(p, object.minute()     , 2, ':');

    const char decimalSign = configuration.useCommaForDecimalSign()
                             ? ','
                             : '.';

    int precision = configuration.fractionalSecondPrecision();

    if (precision) {
        p += u::generateInt(p, object.second(), 2, decimalSign);

        int value = object.millisecond() * 1000 + object.microsecond();

        for (int i = 6; i > precision; --i) {
            value /= 10;
        }

        p += u::generateInt(p, value, precision);
    }
    else {
        p += u::generateInt(p, object.second(), 2);
    }

    return static_cast<int>(p - buffer);
}

int Iso8601Util::generateRaw(char                            *buffer,
                             const Datetime&                  object,
                             const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);

    const int dateLen = generateRaw(buffer, object.date(), configuration);
    *(buffer + dateLen) = 'T';

    char *p = buffer + dateLen + 1;

    p += u::generateInt(p, object.hour()       , 2, ':');
    p += u::generateInt(p, object.minute()     , 2, ':');

    const char decimalSign = configuration.useCommaForDecimalSign()
                             ? ','
                             : '.';

    int precision = configuration.fractionalSecondPrecision();

    if (precision) {
        p += u::generateInt(p, object.second(), 2, decimalSign);

        int value = object.millisecond() * 1000 + object.microsecond();

        for (int i = 6; i > precision; --i) {
            value /= 10;
        }

        p += u::generateInt(p, value, precision);
    }
    else {
        p += u::generateInt(p, object.second(), 2);
    }

    return static_cast<int>(p - buffer);
}

int Iso8601Util::generateRaw(char                            *buffer,
                             const DateTz&                    object,
                             const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);

    const int dateLen = generateRaw(buffer,
                                    object.localDate(),
                                    configuration);

    const int zoneLen = u::generateZoneDesignator(buffer + dateLen,
                                                  object.offset(),
                                                  configuration);

    return dateLen + zoneLen;
}

int Iso8601Util::generateRaw(char                            *buffer,
                             const TimeTz&                    object,
                             const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);

    const int timeLen = generateRaw(buffer,
                                    object.localTime(),
                                    configuration);

    const int zoneLen = u::generateZoneDesignator(buffer + timeLen,
                                                  object.offset(),
                                                  configuration);

    return timeLen + zoneLen;
}

int Iso8601Util::generateRaw(char                            *buffer,
                             const DatetimeTz&                object,
                             const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);

    const int datetimeLen = generateRaw(buffer,
                                        object.localDatetime(),
                                        configuration);

    const int zoneLen     = u::generateZoneDesignator(buffer + datetimeLen,
                                                      object.offset(),
                                                      configuration);

    return datetimeLen + zoneLen;
}

int Iso8601Util::generateRaw(char                            *buffer,
                             const DateOrDateTz&              object,
                             const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);

    if (object.is<Date>()) {
        return generateRaw(buffer, object.the<Date>(), configuration);
                                                                       //RETURN
    }

    return generateRaw(buffer, object.the<DateTz>(), configuration);
}

int Iso8601Util::generateRaw(char                            *buffer,
                             const TimeOrTimeTz&              object,
                             const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);

    if (object.is<Time>()) {
        return generateRaw(buffer, object.the<Time>(), configuration);
                                                                       //RETURN
    }

    return generateRaw(buffer, object.the<TimeTz>(), configuration);
}

int Iso8601Util::generateRaw(char                            *buffer,
                             const DatetimeOrDatetimeTz&      object,
                             const Iso8601UtilConfiguration&  configuration)
{
    BSLS_ASSERT(buffer);

    if (object.is<Datetime>()) {
        return generateRaw(buffer, object.the<Datetime>(), configuration);
                                                                       //RETURN
    }

    return generateRaw(buffer, object.the<DatetimeTz>(), configuration);
}

int Iso8601Util::parse(bsls::TimeInterval *result,
                       const char         *string,
                       int                 length)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    // Sample ISO 8601 duration: "P3Y6M4DT12H30M5.35S"
    //
    // All components are optional except the leading "P" (for "Period"). If
    // there is any time component, then the "T" is also mandatory.  At least
    // one component must be present, and if the "T" is present, at least one
    // component must appear after the "T".

    bsls::Types::Int64 weeks       = 0;
    bsls::Types::Int64 days        = 0;
    bsls::Types::Int64 hours       = 0;
    bsls::Types::Int64 minutes     = 0;
    bsls::Types::Int64 seconds     = 0;
    bsls::Types::Int64 nanoseconds = 0;

    if (0 != u::parseIntervalImpl(&weeks,
                                  &days,
                                  &hours,
                                  &minutes,
                                  &seconds,
                                  &nanoseconds,
                                  string,
                                  length)) {
        return -1;                                                    // RETURN
    }

    result->setTotalDays(weeks * 7);
    result->addDays(days);
    result->addHours(hours);
    result->addMinutes(minutes);
    result->addSeconds(seconds);
    result->addNanoseconds(nanoseconds);

    return 0;
}

int Iso8601Util::parse(Date *result, const char *string, int length)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    int  tzOffset          = 0;
    bool hasZoneDesignator = false;

    return u::parseDate(result, &tzOffset, &hasZoneDesignator, string, length);
}

int Iso8601Util::parse(Time *result, const char *string, int length)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    Time localTime;
    int  tzOffset          = 0;      // minutes from UTC
    bool hasZoneDesignator = false;

    if (0 != u::parseTime(&localTime,
                          &tzOffset,
                          &hasZoneDesignator,
                          string,
                          length)) {
        return -1;                                                    // RETURN
    }

    if (tzOffset) {
        localTime.addMinutes(-tzOffset);  // convert to UTC
    }

    *result = localTime;

    return 0;
}

int Iso8601Util::parse(Datetime *result, const char *string, int length)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    return u::parseImp(result, string, length);
}

int Iso8601Util::parse(DateTz *result, const char *string, int length)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    Date localDate;
    int  tzOffset          = 0;      // minutes from UTC
    bool hasZoneDesignator = false;

    if (0 != u::parseDate(&localDate,
                          &tzOffset,
                          &hasZoneDesignator,
                          string,
                          length)) {
        return -1;                                                    // RETURN
    }

    result->setDateTz(localDate, tzOffset);

    return 0;
}

int Iso8601Util::parse(TimeTz *result, const char *string, int length)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    Time localTime;
    int  tzOffset          = 0;      // minutes from UTC
    bool hasZoneDesignator = false;

    if (0 != u::parseTime(&localTime,
                          &tzOffset,
                          &hasZoneDesignator,
                          string,
                          length)) {
        return -1;                                                    // RETURN
    }

    result->setTimeTz(localTime, tzOffset);

    return 0;
}

int Iso8601Util::parse(DatetimeTz *result, const char *string, int length)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    return u::parseImp(result, string, length);
}

int Iso8601Util::parse(DateOrDateTz *result, const char *string, int length)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    Date localDate;
    int  tzOffset          = 0;      // minutes from UTC
    bool hasZoneDesignator = false;

    if (0 != u::parseDate(&localDate,
                          &tzOffset,
                          &hasZoneDesignator,
                          string,
                          length)) {
        return -1;                                                    // RETURN
    }

    if (hasZoneDesignator) {
        result->createInPlace<DateTz>(localDate, tzOffset);
    }
    else {
        result->assign<Date>(localDate);
    }

    return 0;
}

int Iso8601Util::parse(TimeOrTimeTz *result, const char *string, int length)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    Time localTime;
    int  tzOffset           = 0;      // minutes from UTC
    bool hasZoneDesignator = false;

    if (0 != u::parseTime(&localTime,
                          &tzOffset,
                          &hasZoneDesignator,
                          string,
                          length)) {
        return -1;                                                    // RETURN
    }

    if (hasZoneDesignator) {
        result->createInPlace<TimeTz>(localTime, tzOffset);
    }
    else {
        result->assign<Time>(localTime);
    }

    return 0;
}

int Iso8601Util::parse(DatetimeOrDatetimeTz *result,
                       const char           *string,
                       int                   length)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    return u::parseImp(result, string, length);
}

int Iso8601Util::parseRelaxed(Datetime *result, const char *string, int length)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    return u::parseImp(result, string, length, true);
}

int Iso8601Util::parseRelaxed(DatetimeTz *result,
                              const char *string,
                              int         length)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    return u::parseImp(result, string, length, true);
}

int Iso8601Util::parseRelaxed(DatetimeOrDatetimeTz *result,
                              const char           *string,
                              int                   length)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(0 <= length);

    return u::parseImp(result, string, length, true);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
