// bdlt_iso8601util.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLT_ISO8601UTIL
#define INCLUDED_BDLT_ISO8601UTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functions that convert date/time objects to/from ISO 8601.
//
//@CLASSES:
//  bdlt::Iso8601Util: namespace for ISO 8601 date/time conversion functions
//  bdlt::Iso8601UtilConfiguration: define process-wide default behaviors
//
//@SEE_ALSO: bdlt_date, bdlt_datetz, bdlt_datetime, bdlt_datetimetz, bdlt_time,
//           bdlt_timetz
//
//@DESCRIPTION: This component provides a namespace, 'bdlt::Iso8601Util'
// containing functions that convert 'bdlt' date and time objects to and from
// a string representation defined by the ISO 8601 standard.  The string
// representation generated and parsed by this component adheres to the ISO
// 8601 standard for dates and times and is suitable for use in XML generation
// or parsing.
//
// Each 'generate' function takes a 'bsl::ostream' and a 'bdlt' date or time
// object and writes the ISO 8601 representation to the stream.  Each 'parse'
// function takes a pointer to a 'bdlt' date or time object, a character
// string, and a length and parses the character string into the date or time
// object, returning a non-zero status on error.
//
// This component also provides a class 'bdlt::Iso8601UtilConfiguration' that
// allows clients to configure process-wide default behavior for
// 'bdlt::Iso8601Util'.
// 'bdlt::Iso8601UtilConfiguration::setUseZAbbreviationForUtc' sets whether to
// use 'Z' as an abbreviation for a time zone offset of 00:00 (the default
// value is 'false').
//
///Subset of ISO 8601 Supported by 'bdlt::Iso8601Util'
///---------------------------------------------------
// This component provides support for a subset of the formats defined by the
// ISO 8601 standard, particularly those dealing with the representation of
// date, time, and date-time values.  The standard can be found at:
//
//: o http://dotat.at/tmp/ISO_8601-2004_E.pdf
//
///Supported Formats
///- - - - - - - - -
// This component supports the following formats defined in the ISO 8601
// standard:
//
//: o <FRAC>       := .d+
//: o <TZ>         := ((+|-)hh:mm|Z)
//: o <Date>       := YYYY-MM-DD
//: o <Time>       := hh:mm:ss{<FRAC>}
//: o <Datetime>   := <Date>T<Time>
//: o <DateTz>     := <Date>{<TZ>}
//: o <TimeTz>     := <Time>{<TZ>}
//: o <DatetimeTz> := <Datetime>{<TZ>}
//:
//: o 'bdlt::Date' and 'bdlt::DateTz' both accept input of the form '<DateTz>'.
//:
//: o 'bdlt::Time' and 'bdlt::TimeTz' both accept input of the form '<TimeTz>'.
//:
//: o 'bdlt::Datetime' and 'bdlt::DatetimeTz' both accept input of the form
//:   '<DatetimeTz>'.
//:
//: o '<FRAC>' is optional.
//:   o 'd' in 'FRAC' is any decimal digit.
//:   o If present, 'FRAC' must consist of a '.' followed by at least one digit
//:     d, and is interpreted as a fraction of a second.  Digits after the
//:     first 4 are ignored.  If more than 3 digits, it is rounded to the
//:     nearest 3 digits, possibly resulting in a value of 1.0, which will
//:     carry into the higher fields of the time and possibly date.
//:
//: o '<TZ>' is optional.
//:   o '<TZ>' is always exactly 1 or 6 characters long.  If it is 6 chars, it
//:     is either '+hh:mm' or '-hh:mm', if it is 1 char, it is 'Z', which
//:     has the same meaning as '+00:00' (UTC).
//:   o In 'bdlt::Date', '<TZ>' is ignored if present.
//:   o In 'bdlt::Time' and 'bdlt::Datetime', which have no time zone fields,
//:     '<TZ>' is converted to minutes and subtracted from the result.
//:   o In 'bdlt::DateTz', 'bdlt::TimeTz', and 'bdlt::DatetimeTz' it sets the
//:     timezone field.  If absent while parsing an object with a timezone
//:     field, the timezone field is set to '+00:00' (UTC).
//:   o In <TZ>, 'hh' is in the range '[00 .. 23]', 'mm' is in the range
//:     '[00 .. 59]'.  A value of '24' for 'hh' is not allowed.
//:
//: o In types containing '<Date>':
//:   o 'YYYY' is always 4 digits long, 'MM' and 'DD' are always 2 decimal
//:     digits long.
//:   o 'YYYY' is in the range '[0001 .. 9999]'.
//:   o 'MM' is in the range '[01 .. 12]'
//:   o 'DD' is in the range '[01 .. 31]', also subject to constraints
//:     depending on year and month.
//:   o 'YYYY-MM-DD' must be a valid year, month, day combination according to
//:     'bdlt::Date'.
//:
//: o In types containing '<Time>':
//:   o 'hh', 'mm', and 'ss' are always two decimal digits long.
//:   o 'hh' must be in the range '[00 .. 24]'.  If 'hh' is 24, then 'mm', and
//:      'ss' must be zero, as must '<FRAC>' and '<TZ>' if present.
//:   o 'mm' must be in the range '[00 .. 59]'.
//:   o 'ss' must be in the range '[00 .. 59]'.
//:   o 'hh:mm:ss' must be a valid hour, minute, second combination according
//:     to 'bdlt::Time'.
//:
//: o 'T' in '<Datetime>' literally is the uppercase letter 'T'.  It must be
//:   present.
//
///Subsets of ISO 8601 Format Not Accepted by 'bdlt::Iso8601Util'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The ISO 8601 standard allows for the following formats which this component
// does not support:
//
//: o Ordinal Dates: Dates represented by year and day of year: 2.1.10
//: o Week Dates: Dates represented by year, ordinal week within the year, and
//:   ordinal day within the week: 2.1.11, 4.1.3.2
//: o Recurring Time Intervals: 2.1.17
//: o Leap Seconds: 2.2.2
//: o Calendar Week Numbers: 2.2.10, 4.1.4.
//: o Basic Formats, i.e.,  Dates represented as 'YYYYMMDD', times represented
//:   as 'hhmmss', etc: 2.3.3.  This component supports complete formats only,
//:   that is, only formats with separators such as '-' in dates and ':' in
//:   times, and only formats that where 'YYYY' is 4 digits and 'MM', 'DD',
//:   'hh', 'mm', and 'ss' are always 2 digits.
//: o Extended Formats: 2.3.4 Many of the 'Extended Formats' described in the
//:   document are in fact complete, and are thus supported, but many extended
//:   formats, that have an inappropriate number of digits representing their
//:   fields, are not supported.
//: o Representation with Reduced Accuracy: 2.3.7, 4.2.2.3, 4.2.4, 4.3.3.
//:   Abbreviation of representations by omission of lower order components of
//:   dates or years are not supported, except that fractions of a second are
//:   optional.
//: o Expanded Representation: 2.3.8, 4.1.3.3.  Years outside the range
//:   '[0000 .. 9999]'.  In fact, only the range '[0001 .. 9999]' is supported.
//: o Fractions are only permissible using a '.'; commas ',' are not allowed
//:   4.2.2.4.  Fractions of hours or minutes are not allowed; only fractions
//:   of seconds are allowed.
//: o The time designator 'T' is only allowed within 'Datetime's and
//:   'DatetimeTz's to delimit the end of the date from the beginning of the
//:   time.  4.2.2.5.
//: o 4.3.2 says that the 'T' delimiting the end of date and beginning of time
//:   in a 'Datetime' or 'DatetimeTz' is optional.  In this implementation the
//:   'T' is mandatory.
//: o We do not support Time Intervals, Durations: 4.4.
//: o We do not support Recurring Time Intervals: 4.5.
//
///Note Regarding the Time 24:00
///-----------------------------
// For historic reasons, this component (dubiously) interprets the time string
// "24:00" to be the (singular) default constructed 'Time' value (24:00).
// According to the ISO 8601 specification, "24:00" should be interpreted as
// midnight, the last instant of the day, whereas the default constructed
// 'Time' is treated as 00:00, the first instant of the day.  This behavior is
// maintained for compatibility with existing systems.  A replacement component
// will be made available in 'bdlt' that treats "24:00" as 00:00 of the
// following day.
//
// Behavior changes to 'Datetime' implemented in 'bdlt' mean that ISO 8601
// date-time values having "24:00" will not be created in newly built tasks
// (except for "01-01-01T24:00:00") so this behavior may eventually be safely
// removed (Note added February, 2015).
//
///Usage
///-----
// The following example illustrates how to generate an ISO 8601-compliant
// string from a 'bdlt::DatetimeTz' value:
//..
//  const bdlt::DatetimeTz theDatetime(bdlt::Datetime(2005, 1, 31,
//                                                    8, 59, 59, 123), 240);
//  bsl::stringstream ss;
//  bdlt::Iso8601Util::generate(ss, theDatetime);
//  assert(ss.str() == "2005-01-31T08:59:59.123+04:00");
//..
// The following example illustrates how to parse an ISO 8601-compliant string
// into a 'bdlt::DatetimeTz' object:
//..
//  bdlt::DatetimeTz datetime;
//  const char dtStr[] = "2005-01-31T08:59:59.123+04:00";
//  const int ret = bdlt::Iso8601Util::parse(
//                                       &datetime,
//                                       dtStr,
//                                       static_cast<int>(bsl::strlen(dtStr)));
//  assert(   0 == ret);
//  assert(2005 == datetime.localDatetime().year());
//  assert(   1 == datetime.localDatetime().month());
//  assert(  31 == datetime.localDatetime().day());
//  assert(   8 == datetime.localDatetime().hour());
//  assert(  59 == datetime.localDatetime().minute());
//  assert(  59 == datetime.localDatetime().second());
//  assert( 123 == datetime.localDatetime().millisecond());
//  assert( 240 == datetime.offset());
//..
// Note that fractions of a second was rounded up to 123 milliseconds and that
// the offset from UTC was converted to minutes.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

namespace BloombergLP {
namespace bdlt {

class Date;
class DateTz;
class Datetime;
class DatetimeTz;
class Time;
class TimeTz;

class Iso8601UtilConfiguration;

                            // ==================
                            // struct Iso8601Util
                            // ==================

struct Iso8601Util {
    // This 'struct' provides a namespace for a suite of pure functions that
    // perform conversions between objects of 'bdlt' vocabulary type and their
    // ISO 8601 representations.  Each 'generate' and 'generateRaw' method
    // takes a 'bdlt' object (of type 'Date', 'DateTz', 'Time', 'TimeTz',
    // 'Datetime', or 'DatetimeTz') and outputs its corresponding ISO 8601
    // representation to a user-supplied character buffer or 'bsl::ostream'.
    // The 'parse' methods effect the opposite conversion in that they populate
    // a 'bdlt' object from the result of parsing an ISO 8601 representation.

    // TYPES
    enum {
        // This enumeration defines fixed lengths for the ISO 8601
        // representations of date, time, and datetime values.

        k_DATE_STRLEN       = 10,  // 'bdlt::Date'
        k_DATETZ_STRLEN     = 16,  // 'bdlt::DateTz'

        k_TIME_STRLEN       = 12,  // 'bdlt::Time'
        k_TIMETZ_STRLEN     = 18,  // 'bdlt::TimeTz'

        k_DATETIME_STRLEN   = 23,  // 'bdlt::Datetime'
        k_DATETIMETZ_STRLEN = 29,  // 'bdlt::DatetimeTz'

        k_MAX_STRLEN        = k_DATETIMETZ_STRLEN
    };

    // CLASS METHODS
    static int generate(char                            *buffer,
                        const Date&                      object,
                        int                              bufferLength);
    static int generate(char                            *buffer,
                        const Date&                      object,
                        int                              bufferLength,
                        const Iso8601UtilConfiguration&  configuration);
    static int generate(char                            *buffer,
                        const Time&                      object,
                        int                              bufferLength);
    static int generate(char                            *buffer,
                        const Time&                      object,
                        int                              bufferLength,
                        const Iso8601UtilConfiguration&  configuration);
    static int generate(char                            *buffer,
                        const Datetime&                  object,
                        int                              bufferLength);
    static int generate(char                            *buffer,
                        const Datetime&                  object,
                        int                              bufferLength,
                        const Iso8601UtilConfiguration&  configuration);
    static int generate(char                            *buffer,
                        const DateTz&                    object,
                        int                              bufferLength);
    static int generate(char                            *buffer,
                        const DateTz&                    object,
                        int                              bufferLength,
                        const Iso8601UtilConfiguration&  configuration);
    static int generate(char                            *buffer,
                        const TimeTz&                    object,
                        int                              bufferLength);
    static int generate(char                            *buffer,
                        const TimeTz&                    object,
                        int                              bufferLength,
                        const Iso8601UtilConfiguration&  configuration);
    static int generate(char                            *buffer,
                        const DatetimeTz&                object,
                        int                              bufferLength);
    static int generate(char                            *buffer,
                        const DatetimeTz&                object,
                        int                              bufferLength,
                        const Iso8601UtilConfiguration&  configuration);
        // Write the ISO 8601 representation of the specified 'object' to the
        // specified 'buffer', truncating (if necessary) to the specified
        // 'bufferLength'.  Optionally specify a 'configuration' to affect the
        // format of the generated string.  If 'configuration' is not supplied,
        // the process-wide default value
        // 'Iso8601UtilConfiguration::defaultConfiguration()' is used.  Return
        // the number of characters in the formatted string before truncation
        // (not counting a null terminator).  If 'bufferLength' indicates
        // sufficient capacity, 'buffer' is null terminated.  The behavior is
        // undefined unless '0 <= bufferLength'.  Note that a buffer of size
        // 'k_MAX_STRLEN + 1' is large enough to hold any string generated by
        // this component (counting a null terminator, if any).

    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Date&                     object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Date&                     object,
                                const Iso8601UtilConfiguration& configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Time&                     object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Time&                     object,
                                const Iso8601UtilConfiguration& configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Datetime&                 object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Datetime&                 object,
                                const Iso8601UtilConfiguration& configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DateTz&                   object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DateTz&                   object,
                                const Iso8601UtilConfiguration& configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const TimeTz&                   object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const TimeTz&                   object,
                                const Iso8601UtilConfiguration& configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DatetimeTz&               object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DatetimeTz&               object,
                                const Iso8601UtilConfiguration& configuration);
        // Write the ISO 8601 representation of the specified 'object' to the
        // specified 'stream'.  Optionally specify a 'configuration' to affect
        // the format of the generated string.  If 'configuration' is not
        // supplied, the process-wide default value
        // 'Iso8601UtilConfiguration::defaultConfiguration()' is used.  Return
        // a reference to 'stream'.  Note that 'stream' is not null terminated.

    static int generateRaw(char                            *buffer,
                           const Date&                      object);
    static int generateRaw(char                            *buffer,
                           const Date&                      object,
                           const Iso8601UtilConfiguration&  configuration);
    static int generateRaw(char                            *buffer,
                           const Time&                      object);
    static int generateRaw(char                            *buffer,
                           const Time&                      object,
                           const Iso8601UtilConfiguration&  configuration);
    static int generateRaw(char                            *buffer,
                           const Datetime&                  object);
    static int generateRaw(char                            *buffer,
                           const Datetime&                  object,
                           const Iso8601UtilConfiguration&  configuration);
    static int generateRaw(char                            *buffer,
                           const DateTz&                    object);
    static int generateRaw(char                            *buffer,
                           const DateTz&                    object,
                           const Iso8601UtilConfiguration&  configuration);
    static int generateRaw(char                            *buffer,
                           const TimeTz&                    object);
    static int generateRaw(char                            *buffer,
                           const TimeTz&                    object,
                           const Iso8601UtilConfiguration&  configuration);
    static int generateRaw(char                            *buffer,
                           const DatetimeTz&                object);
    static int generateRaw(char                            *buffer,
                           const DatetimeTz&                object,
                           const Iso8601UtilConfiguration&  configuration);
        // Write the ISO 8601 representation of the specified 'object' to the
        // specified 'buffer'.  Optionally specify a 'configuration' to affect
        // the format of the generated string.  If 'configuration' is not
        // supplied, the process-wide default value
        // 'Iso8601UtilConfiguration::defaultConfiguration()' is used.  Return
        // the number of characters in the formatted string.  'buffer' is not
        // null terminated.  The behavior is undefined unless 'buffer' has
        // sufficient capacity.  Note that a buffer of size 'k_MAX_STRLEN + 1'
        // is large enough to hold any string generated by this component
        // (counting a null terminator, if any).

    static int parse(Date *result, const char *input, int inputLength);
        // TBD update doc
        // Attempt to parse the specified 'inputLength' characters of the
        // specified 'input' as a date, and if successful load this date into
        // the specified 'result'.  Parsing is successful if the input is of
        // the format "YYYY-MM-DD{((+|-)hh:mm|Z)}" where 'YYYY', 'MM', and
        // 'DD' are strings representing positive integers, '-' is literally a
        // dash character, 'YYYY' is 4 chars long, and 'MM' and 'DD' are both 2
        // chars long.  'YYYY' must be in the range '[0001 .. 9999]', 'MM' must
        // be in the range '[01 .. 12]', and 'DD' must be in the range
        // '[01 .. 31]', such that 'YYYY-MM-DD' represents a valid date.
        // Optional time zone information may be provided, in which case it is
        // parsed and ignored.  Do not modify '*result' on failure.  Return 0
        // if parsing is successful, and return a non-zero value (and do not
        // modify 'result') otherwise.  The behavior is undefined unless
        // '0 <= inputLength'.  Note that this function parses *exactly*
        // 'inputLength' characters; parsing will fail if a subset of the
        // passed string matches the specified format and is then followed by
        // trailing characters.

    static int parse(Time *result, const char *input, int inputLength);
        // TBD update doc
        // Parse a time, represented in the "hh:mm:ss{.d+}{((+|-)hh:mm|Z)}"
        // format, from the specified 'input' of the specified 'inputLength'
        // and load it into the specified '*result'.  In the "hh:mm:ss{.d+}"
        // format accepted by this function, 'hh', 'mm', 'ss' are all 2 digit
        // integers (left padded with 0's if necessary) denoting hours,
        // minutes, and seconds, ':' is literally a colon character, and {.d+}
        // is the optional fraction of a second, consisting of a '.' followed
        // by one or more decimal digits.  'hh' must be in the range
        // '[00 .. 23]', 'mm' must be in the range '[00 .. 59]', and 'ss' must
        // be in the range '[00 .. 60]'.  If 'ss == 60' (a leap second), then
        // a value of 59 is loaded into the 'seconds' field, and after all
        // fields are loaded, 1 second is added to '*result'.  If '{.d+}'
        // contains more than 3 digits, the value will be rounded to the
        // nearest value in milliseconds, possibly rounding '*result' up a full
        // second.  Optional time zone information may be provided, in which
        // case '*result' is converted to the equivalent GMT time.  An
        // exceptional time value of '24:00:00' may be provided, in which case
        // the fraction of a second must be 0 and the time zone must be absent
        // or GMT.  Do not modify '*result' on failure.  Return 0 on success,
        // and a non-zero otherwise.  The behavior is undefined unless
        // '0 <= inputLength'.  Note that if 'inputLength' is longer than the
        // length of the parsed data, parsing will fail.  Also note that it is
        // possible for the resulting 'ss' value to be rounded up twice if
        // originally 'ss == 60' and there was rounding up due to the '{.d+}'
        // field.

    static int parse(Datetime *result, const char *input, int inputLength);
        // TBD update doc
        // Parse a date time, represented in the
        // "YYYY-MM-DDThh:mm:ss{.d+}{((+|-)hh:mm|Z)}" format, from the
        // specified 'input' of the specified 'inputLength' and load it into
        // the specified '*result'.  In the "YYYY-MM-DD" format accepted by
        // this function, 'YYYY', 'MM', and 'DD' are strings representing
        // positive integers, '-' is literally a dash character, 'YYYY' is 4
        // chars long, and 'MM' and 'DD' are both 2 chars long.  'YYYY' must be
        // in the range '[0001 .. 9999]', 'MM' must be in the range
        // '[01 .. 12]', and 'DD' must be in the range '[01 .. 31]', such that
        // 'YYYY-MM-DD' represents a valid date.  'T' literally represents the
        // 'T' character.  In the "hh:mm:ss{.d+}" format, 'hh', 'mm', 'ss' are
        // all 2 digit integers (left padded with 0's if necessary) denoting
        // hours, minutes, and seconds, ':' is literally a colon character, and
        // {.d+} is the optional fraction of a second, consisting of a '.'
        // followed by one or more decimal digits.  'hh' must be in the range
        // '[00 .. 23]', 'mm' must be in the range '[00 .. 59]', and 'ss' must
        // be in the range '[00 .. 60]'.  If 'ss == 60' (a leap-second), then a
        // value of 59 is loaded into the 'seconds' field, and after all fields
        // are loaded, 1 second is added to '*result'.  If '{.d+}' contains
        // more than 3 digits, the value will be rounded to the nearest value
        // in milliseconds, possibly resulting in time being rounded up a full
        // second.  Optional time zone information may be provided, in which
        // case '*result' is converted to the equivalent GMT time.  An
        // exceptional case is that a time value of '24:00:00' is allowed, but
        // only if the fraction of a second is 0 and the time zone is absent or
        // GMT.  Do not modify '*result' on failure.  Return 0 on success, and
        // a non-zero value otherwise.  The behavior is undefined unless
        // '0 <= inputLength'.  Note that if 'inputLength' is longer than the
        // length of the parsed data, parsing will fail.  Also note that the
        // final 'ss' may be rounded up twice if originally 'ss == 60' and
        // there is rounding up due to the '{.d+}' field.

    static int parse(DateTz *result, const char *input, int inputLength);
        // TBD update doc
        // Parse a date, represented in the "YYYY-MM-DD{((+|-)hh:mm|Z)}"
        // format, from the specified 'input' of the specified 'inputLength'
        // and load it into the specified '*result'.  In the "YYYY-MM-DD"
        // format accepted by this function, 'YYYY', 'MM', and 'DD' are strings
        // representing positive integers, '-' is literally a dash character,
        // 'YYYY' is 4 chars long, and 'MM' and 'DD' are both 2 chars long.
        // 'YYYY' must be in the range '[0001 .. 9999]', 'MM' must be in the
        // range '[01 .. 12]', and 'DD' must be in the range '[01 .. 31]', such
        // that 'YYYY-MM-DD' represents a valid date.  Optional time zone
        // information may be provided in the "Shh:mm" format accepted by this
        // function, 'hh' and 'mm' are 2 digit integers (left padded with '0's
        // if necessary).  'hh' must be in the range '[00 .. 23]' and 'mm' must
        // be in the range '[00 .. 59]'.  An alternate form of the
        // representation for the time zone is 'Z', signifying GMT.  If
        // no time zone is provided, GMT is assumed.  Do not modify '*result'
        // on failure.  The behavior is undefined unless '0 <= inputLength'.
        // Return 0 on success, and a non-zero value otherwise.  Note that if
        // 'inputLength' is longer than the length of the parsed data, parsing
        // will fail.

    static int parse(TimeTz *result, const char *input, int inputLength);
        // TBD update doc
        // Parse a time, represented in the "hh:mm:ss{.d+}{((+|-)hh:mm|Z)}"
        // format, from the specified 'input' of the specified 'inputLength'
        // and load it into the specified '*result'.  In the "hh:mm:ss{.d+}"
        // format accepted by this function, 'hh', 'mm', 'ss' are all 2 digit
        // integers (left padded with 0's if necessary) denoting hours,
        // minutes, and seconds, ':' is literally a colon character, and {.d+}
        // is the optional fraction of a second, consisting of a '.' followed
        // by one or more decimal digits.  'hh' must be in the range
        // '[00 .. 23]', 'mm' must be in the range '[00 .. 59]', and 'ss' must
        // be in the range '[00 .. 60]'.  If 'ss == 60' (a leap-second), then a
        // value of 59 is loaded into the 'seconds' field, and after all fields
        // are loaded, 1 second is added to '*result'.  If '{.d+}' contains
        // more than 3 digits, the value will be rounded to the nearest value
        // in milliseconds, possibly rounding '*result' up a full second.
        // Optional time zone information may be provided in the "Shh:mm"
        // format accepted by this function, 'hh' and 'mm' are 2 digit integers
        // (left padded with '0's if necessary).  'hh' must be in the range
        // '[00 .. 23]' and 'mm' must be in the range '[00 .. 59]'.  An
        // alternate form of the representation for the time zone is 'Z',
        // signifying GMT.  If time zone information is not provided, GMT
        // is assumed.  An exceptional time value of '24:00:00' may be
        // provided, in which case the fraction of a second must be 0 and the
        // time zone, if present, must be GMT.  Do not modify '*result' on
        // failure.  Return 0 on success, and a non-zero value otherwise.  The
        // behavior is undefined unless '0 <= inputLength'.  Note that if
        // 'inputLength' is longer than the length of the parsed data, parsing
        // will fail.  Also note that the final 'ss' may be rounded up twice if
        // originally 'ss == 60' and there is rounding up due to the '{.d+}'
        // field.

    static int parse(DatetimeTz *result, const char *input, int inputLength);
        // TBD update doc
        // Parse a date time, represented in the
        // "YYYY-MM-DDThh:mm:ss{.d+}{((+|-)hh:mm|Z)}" format, from the
        // specified 'input' of the specified 'inputLength' and load it into
        // the specified '*result'.  In the "YYYY-MM-DD" format accepted by
        // this function, 'YYYY', 'MM', and 'DD' are strings representing
        // positive integers, '-' is literally a dash character, 'YYYY' is 4
        // chars long, and 'MM' and 'DD' are both 2 chars long.  'YYYY' must be
        // in the range '[0001 .. 9999]', 'MM' must be in the range
        // '[01 .. 12]', and 'DD' must be in the range '[01 .. 31]', such that
        // 'YYYY-MM-DD' represents a valid date.  'T' literally represents the
        // 'T' character.  In the "hh:mm:ss{.d+}" format, 'hh', 'mm', 'ss' are
        // all 2 digit integers (left padded with 0's if necessary) denoting
        // hours, minutes, and seconds, ':' is literally a colon character, and
        // {.d+} is the optional fraction of a second, consisting of a '.'
        // followed by one or more decimal digits.  If '{.d+}' contains more
        // than 3 digits, the value will be rounded to the nearest value in
        // milliseconds, possibly rounding '*result' up by a full second.  'hh'
        // must be in the range '[00 .. 23]', 'mm' must be in the range
        // '[00 .. 59]', and 'ss' must be in the range '[00 .. 60]'.  If
        // 'ss == 60' (a leap-second), then a value of 59 is loaded into the
        // 'seconds' field, and after all fields are loaded, 1 second is added
        // to '*result'.  The time zone information is optional but if it is
        // provided then it must be in the "Shh:mm" format, 'hh' and 'mm' are 2
        // digit integers (left padded with '0's if necessary).  'hh' must be
        // in the range '[00 .. 23]' and 'mm' must be in the range
        // '[00 .. 59]'.  An alternate form of representing the time zone is
        // 'Z', signifying GMT.  If the time zone is not provided, GMT
        // is assumed.  Do not modify '*result' on failure.  An exceptional
        // time value of '24:00;00' may be provided, but if so the fraction of
        // a second must be 0 and time zone, if any, must be GMT.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless '0 <= inputLength'.  Note that if 'inputLength' is longer
        // than the length of the parsed data, parsing will fail.  Also note
        // that the final 'ss' may be rounded up twice if originally 'ss == 60'
        // and there is rounding up due to the '{.d+}' field.
};

                        // ==============================
                        // class Iso8601UtilConfiguration
                        // ==============================

class Iso8601UtilConfiguration {
    // This unconstrained (value-semantic) attribute class characterizes how to
    // configure certain behavior in 'Iso8601Util' functions.  Currently, only
    // the 'generate' and 'generateRaw' methods of that utility are affected
    // by 'Iso8601UtilConfiguration' settings.  See the Attributes section
    // under @DESCRIPTION in the component-level documentation for information
    // on the class attributes.

  private:
    // PRIVATE TYPES
    enum {
        // This enumeration denotes the distinct bits that define the values of
        // each of the three configuration attributes.

        k_omitColonInZoneDesignatorBit = 0x1,
        k_useCommaForDecimalSignBit    = 0x2,
        k_useZAbbreviationForUtcBit    = 0x4
    };

    // CLASS DATA
    static bsls::AtomicOperations::AtomicTypes::Int
                    s_defaultConfiguration;  // process-wide configuration

    // DATA
    int             d_configurationMask;     // bitmask defining configuration

    // FRIENDS
    friend bool operator==(const Iso8601UtilConfiguration&,
                           const Iso8601UtilConfiguration&);
    friend bool operator!=(const Iso8601UtilConfiguration&,
                           const Iso8601UtilConfiguration&);

  private:
    // PRIVATE CREATORS
    Iso8601UtilConfiguration(int configurationMask);
        // Create an 'Iso8601UtilConfiguration' object having the value
        // indicated by the specified 'configurationMask'.  The behavior is
        // undefined unless 'configurationMask' represents a valid
        // 'Iso8601UtilConfiguration' value.

  public:
    // CLASS METHODS
    static void setDefaultConfiguration(
                                const Iso8601UtilConfiguration& configuration);
        // Set the value of the process-wide 'Iso8601UtilConfiguration' to the
        // specified 'configuration'.  Note that the expected usage is that the
        // process-wide configuration will be established *once*, early in
        // 'main', and not changed throughout the lifetime of a process.

    static Iso8601UtilConfiguration defaultConfiguration();
        // Return the value of the process-wide 'Iso8601UtilConfiguration' that
        // is currently in effect.

    // CREATORS
    Iso8601UtilConfiguration();
        // Create an 'Iso8601UtilConfiguration' object having the (default)
        // attribute values:
        //..
        //  omitColonInZoneDesignator() == false
        //  useCommaForDecimalSign()    == false
        //  useZAbbreviationForUtc()    == false
        //..

    Iso8601UtilConfiguration(const Iso8601UtilConfiguration& original);
        // Create an 'Iso8601UtilConfiguration' object having the value of the
        // specified 'original' configuration.

    ~Iso8601UtilConfiguration();
        // Destroy this object.

    // MANIPULATORS
    Iso8601UtilConfiguration& operator=(const Iso8601UtilConfiguration& rhs);
        // Assign to this object the value of the specified 'rhs'
        // configuration, and return a reference providing modifiable access to
        // this object.

    void setOmitColonInZoneDesignator(bool value);
        // Set the 'omitColonInZoneDesignator' attribute of this object to the
        // specified 'value'.

    void setUseCommaForDecimalSign(bool value);
        // Set the 'useCommaForDecimalSign' attribute of this object to the
        // specified 'value'.

    void setUseZAbbreviationForUtc(bool value);
        // Set the 'useZAbbreviationForUtc' attribute of this object to the
        // specified 'value'.

    // ACCESSORS
    bool omitColonInZoneDesignator() const;
        // Return the value of the 'omitColonInZoneDesignator' attribute of
        // this object.

    bool useCommaForDecimalSign() const;
        // Return the value of the 'useCommaForDecimalSign' attribute of this
        // object.

    bool useZAbbreviationForUtc() const;
        // Return the value of the 'useZAbbreviationForUtc' attribute of this
        // object.
};

// FREE OPERATORS
bool operator==(const Iso8601UtilConfiguration& lhs,
                const Iso8601UtilConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Iso8601UtilConfiguration' objects
    // have the same value if each of their attributes (respectively) have the
    // same value.

bool operator!=(const Iso8601UtilConfiguration& lhs,
                const Iso8601UtilConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'Iso8601UtilConfiguration'
    // objects do not have the same value if any of their attributes
    // (respectively) do not have the same value.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                            // ------------------
                            // struct Iso8601Util
                            // ------------------

// CLASS METHODS
inline
int Iso8601Util::generate(char *buffer, const Date& object, int bufferLength)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    object,
                    bufferLength,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generate(char *buffer, const Time& object, int bufferLength)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    object,
                    bufferLength,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int
Iso8601Util::generate(char *buffer, const Datetime& object, int bufferLength)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    object,
                    bufferLength,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generate(char *buffer, const DateTz& object, int bufferLength)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    object,
                    bufferLength,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generate(char *buffer, const TimeTz& object, int bufferLength)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    object,
                    bufferLength,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int
Iso8601Util::generate(char *buffer, const DatetimeTz& object, int bufferLength)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    object,
                    bufferLength,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream, const Date& object)
{
    return generate(stream,
                    object,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const Date&                     object,
                                 const Iso8601UtilConfiguration& configuration)
{
    char buffer[k_DATE_STRLEN + 1];

    const int len = generate(buffer, object, k_DATE_STRLEN, configuration);
    BSLS_ASSERT_SAFE(k_DATE_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream, const Time& object)
{
    return generate(stream,
                    object,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const Time&                     object,
                                 const Iso8601UtilConfiguration& configuration)
{
    char buffer[k_TIME_STRLEN + 1];

    const int len = generate(buffer, object, k_TIME_STRLEN, configuration);
    BSLS_ASSERT_SAFE(k_TIME_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream&
Iso8601Util::generate(bsl::ostream& stream, const Datetime& object)
{
    return generate(stream,
                    object,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const Datetime&                 object,
                                 const Iso8601UtilConfiguration& configuration)
{
    char buffer[k_DATETIME_STRLEN + 1];

    const int len = generate(buffer, object, k_DATETIME_STRLEN, configuration);
    BSLS_ASSERT_SAFE(k_DATETIME_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream, const DateTz& object)
{
    return generate(stream,
                    object,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const DateTz&                   object,
                                 const Iso8601UtilConfiguration& configuration)
{
    char buffer[k_DATETZ_STRLEN + 1];

    const int len = generate(buffer, object, k_DATETZ_STRLEN, configuration);
    BSLS_ASSERT_SAFE(k_DATETZ_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream, const TimeTz& object)
{
    return generate(stream,
                    object,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const TimeTz&                   object,
                                 const Iso8601UtilConfiguration& configuration)
{
    char buffer[k_TIMETZ_STRLEN + 1];

    const int len = generate(buffer, object, k_TIMETZ_STRLEN, configuration);
    BSLS_ASSERT_SAFE(k_TIMETZ_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream&
Iso8601Util::generate(bsl::ostream& stream, const DatetimeTz& object)
{
    return generate(stream,
                    object,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const DatetimeTz&               object,
                                 const Iso8601UtilConfiguration& configuration)
{
    char buffer[k_DATETIMETZ_STRLEN + 1];

    const int len = generate(buffer,
                             object,
                             k_DATETIMETZ_STRLEN,
                             configuration);
    BSLS_ASSERT_SAFE(k_DATETIMETZ_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
int Iso8601Util::generateRaw(char *buffer, const Date& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const Time& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const Datetime& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const DateTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const TimeTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const DatetimeTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       Iso8601UtilConfiguration::defaultConfiguration());
}

                        // ------------------------------
                        // class Iso8601UtilConfiguration
                        // ------------------------------

// PRIVATE CREATORS
inline
Iso8601UtilConfiguration::Iso8601UtilConfiguration(int configurationMask)
: d_configurationMask(configurationMask)
{
    BSLS_ASSERT_SAFE(0 == (configurationMask
                           & ~(k_omitColonInZoneDesignatorBit
                             | k_useCommaForDecimalSignBit
                             | k_useZAbbreviationForUtcBit)));
}

// CLASS METHODS
inline
void Iso8601UtilConfiguration::setDefaultConfiguration(
                                 const Iso8601UtilConfiguration& configuration)
{
    bsls::AtomicOperations::setIntRelease(&s_defaultConfiguration,
                                          configuration.d_configurationMask);
}

inline
Iso8601UtilConfiguration Iso8601UtilConfiguration::defaultConfiguration()
{
    return Iso8601UtilConfiguration(
               bsls::AtomicOperations::getIntRelaxed(&s_defaultConfiguration));
}

// CREATORS
inline
Iso8601UtilConfiguration::Iso8601UtilConfiguration()
: d_configurationMask(0)
{
}

inline
Iso8601UtilConfiguration::Iso8601UtilConfiguration(
                                      const Iso8601UtilConfiguration& original)
: d_configurationMask(original.d_configurationMask)
{
}

inline
Iso8601UtilConfiguration::~Iso8601UtilConfiguration()
{
}

// MANIPULATORS
inline
Iso8601UtilConfiguration& Iso8601UtilConfiguration::operator=(
                                           const Iso8601UtilConfiguration& rhs)
{
    d_configurationMask = rhs.d_configurationMask;

    return *this;
}

// ACCESSORS
inline
bool Iso8601UtilConfiguration::omitColonInZoneDesignator() const
{
    return d_configurationMask & k_omitColonInZoneDesignatorBit;
}

inline
bool Iso8601UtilConfiguration::useCommaForDecimalSign() const
{
    return d_configurationMask & k_useCommaForDecimalSignBit;
}

inline
bool Iso8601UtilConfiguration::useZAbbreviationForUtc() const
{
    return d_configurationMask & k_useZAbbreviationForUtcBit;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const Iso8601UtilConfiguration& lhs,
                      const Iso8601UtilConfiguration& rhs)
{
    return lhs.d_configurationMask == rhs.d_configurationMask;
}

inline
bool bdlt::operator!=(const Iso8601UtilConfiguration& lhs,
                      const Iso8601UtilConfiguration& rhs)
{
    return lhs.d_configurationMask != rhs.d_configurationMask;
}

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
