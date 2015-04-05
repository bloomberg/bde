// bdlt_iso8601util.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLT_ISO8601UTIL
#define INCLUDED_BDLT_ISO8601UTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide conversions between date/time objects and ISO 8601 strings.
//
//@CLASSES:
//  bdlt::Iso8601Util: namespace for ISO 8601 date/time conversion functions
//  bdlt::Iso8601UtilConfiguration: configuration for generated strings
//
//@SEE_ALSO: bdlt_date, bdlt_datetz, bdlt_datetime, bdlt_datetimetz, bdlt_time,
//           bdlt_timetz
//
//@DESCRIPTION: This component provides a namespace, 'bdlt::Iso8601Util',
// containing functions that convert 'bdlt' date, time, and datetime objects to
// and from ("generate" and "parse", respectively) corresponding string
// representations that are compliant with the ISO 8601 standard.  The version
// of the ISO 8601 standard that is the basis for this component can be found
// at:
//..
//  http://dotat.at/tmp/ISO_8601-2004_E.pdf
//..
// In general terms, 'Iso8601Util' functions support what ISO 8601 refers to as
// *complete* *representations* in *extended* *format*.  We first present a
// brief overview before delving into the details of the ISO 8601
// representations that are supported for each of the relevant 'bdlt'
// vocabulary types.
//
// Each function that *generates* ISO 8601 strings (named 'generate' and
// 'generateRaw') takes a 'bdlt' object and a 'char *' buffer or
// 'bsl::ostream', and writes an ISO 8601 representation of the object to the
// buffer or stream.  The "raw" functions are distinguished from their
// non-"raw" counterparts in two respects:
//
//: o The length of the 'char *' buffer is not supplied to the "raw" functions.
//:
//: o The "raw" functions do not output a null terminator.
//
// Since the generate functions always succeed, no status value is returned.
// Instead, either the number of characters output to the 'char *' buffer or a
// reference to the stream is returned.  (Note that the generating functions
// also take an optional "configuration" object, which is discussed shortly.)
//
// Each function that *parses* ISO 8601 strings (named 'parse') take the
// address of a target 'bdlt' object and a 'const char *', and loads the object
// with the result of parsing the character string.  Since parsing can fail,
// the parse functions return an 'int' status value (0 for success and a
// non-zero value for failure).  Note that, besides elementary syntactical
// considerations, the validity of parsed strings are subject to the semantic
// constraints imposed by the various 'isValid*' class methods, (i.e.,
// 'Date::isValidYearMonthDay', 'Time::isValid', etc.).
//
///ISO 8601 String Generation
///--------------------------
// Strings produced by the 'generate' and 'generateRaw' functions are a
// straightforward transposition of the attributes of the source 'bdlt' value
// into an appropriate ISO 8601 format, and are best illustrated by a few
// examples.  Note that for any type having a time component ('Time', 'TimeTz',
// 'Datetime', and 'DatetimeTz'), the fractional second is always generated,
// and always with three decimal digits:
//..
//  +--------------------------------------+---------------------------------+
//  |             Object Value             |    Generated ISO 8601 String    |
//  +======================================+=================================+
//  |  Date(2002, 03, 17)                  |  2002-03-17                     |
//  +--------------------------------------+---------------------------------+
//  |  Time(15, 46, 09, 330)               |  15:46:09.330                   |
//  +--------------------------------------+---------------------------------+
//  |  Datetime(Date(2002, 03, 17)         |                                 |
//  |           Time(15, 46, 09, 330))     |  2002-03-17T15:46:09.330        |
//  +--------------------------------------+---------------------------------+
//  |  DateTz(Date(2002, 03, 17), -120)    |  2002-03-17-02:00               |
//  +--------------------------------------+---------------------------------+
//  |  TimeTz(Time(15, 46, 09, 330), 270)  |  15:46:09.330+04:30             |
//  +--------------------------------------+---------------------------------+
//  |  DatetimeTz(Datetime(                |                                 |
//  |              Date(2002, 03, 17),     |                                 |
//  |              Time(15, 46, 09, 330)), |                                 |
//  |             0)                       |  2002-03-17T15:46:09.330+00:00  |
//  +--------------------------------------+---------------------------------+
//..
///Configuration
///- - - - - - -
// This component also provides a (value-semantic) attribute class,
// 'bdlt::Iso8601UtilConfiguration', that enables configuration of a few
// aspects of string generation.  The 'generate' and 'generateRaw' functions
// come in matched pairs, where the two functions in each pair are
// distinguished as to whether or not a 'Iso8601UtilConfiguration' object is
// supplied.
//
// Three aspects of ISO 8601 string generation are adjustable via the
// 'Iso8601UtilConfiguration' type:
//: o The decimal sign to use in fractional seconds: '.' or ','.
//:
//: o Whether ':' is optional in zone designators.
//:
//: o Whether 'Z' is output for the zone designator instead of '+00:00' (UTC).
//
// Hence, 'Iso8601UtilConfiguration' has these three unconstrained attributes:
//..
//              Name            Type   Default
//  -------------------------   ----   -------
//  omitColonInZoneDesignator   bool    false
//  useCommaForDecimalSign      bool    false
//  useZAbbreviationForUtc      bool    false
//..
// For generate methods not passed an 'Iso8601UtilConfiguration' object, a
// process-wide configuration takes effect.  This "default" configuration,
// returned by 'Iso8601UtilConfiguration::defaultConfiguration', may be set by
// the client.  The initial setting for the process-wide configuration (i.e.,
// as established at start-up) has the default value for a
// 'Iso8601UtilConfiguration' object.
//
///ISO 8601 String Parsing
///-----------------------
// The parse functions accept *all* strings that are produced by the generate
// functions.  In addition, the parse functions accept some variation in the
// generated strings, the details of which are discussed next.  Note that the
// parse methods are not configurable like the generate methods; in particular,
// the process-wide configuration has no effect on parsing.  Instead, the parse
// methods automatically accept '.' or ',' as the decimal sign in fractional
// seconds, and treat '+00:00', '+0000', and 'Z' as equivalent zone designators
// (all denoting UTC).
//
///Zone Designators
/// - - - - - - - -
// The zone designator is optional, and can be present when parsing for *any*
// type, i.e., even for 'Date', 'Time', and 'Datetime'.  If a zone designator
// is parsed for a 'Date', it must be valid, so it can affect the status value
// that is returned in that case, but it is otherwise ignored.  For 'Time' and
// 'Datetime', any zone designator present in the parsed string will affect the
// resulting object value (unless the zone designator denotes UTC) because the
// result is converted to GMT.  If the zone designator is absent, it is treated
// as if '+00:00' were specified:
//..
//  +------------------------------------+-----------------------------------+
//  |       Parsed ISO 8601 String       |        Result Object Value        |
//  +====================================+===================================+
//  |  2002-03-17-02:00                  |  Date(2002, 03, 17)               |
//  |                                    |  # zone designator ignored        |
//  +------------------------------------+-----------------------------------+
//  |  2002-03-17-02:65                  |  Date: parsing fails              |
//  |                                    |  # invalid zone designator        |
//  +------------------------------------+-----------------------------------+
//  |  15:46:09.330+04:30                |  Time(11, 16, 09, 330)            |
//  |                                    |  # converted to GMT               |
//  +------------------------------------+-----------------------------------+
//  |  15:46:09.330+04:30                |  TimeTz(Time(15, 46, 09, 330),    |
//  |                                    |         270)                      |
//  +------------------------------------+-----------------------------------+
//  |  15:46:09.330                      |  TimeTz(Time(15, 46, 09, 330),    |
//  |                                    |         0)                        |
//  |                                    |  # implied '+00:00'               |
//  +------------------------------------+-----------------------------------+
//  |  2002-03-17T23:46:09.222-5:00      |  Datetime(Date(2002, 03, 18),     |
//  |                                    |           Time(04, 46, 09, 222))  |
//  |                                    |  # carry into 'day' attribute     |
//  |                                    |  # when converted to GMT          |
//  +------------------------------------+-----------------------------------+
//..
// In the last example above, the conversion to GMT incurs a carry into the
// 'day' attribute of the 'Date' component of the resulting 'Datetime' value.
// Note that if such a carry would cause an underflow or overflow at the
// extreme ends of the valid range of dates (0001/01/01 and 9999/12/31), then
// parsing for 'Datetime' would fail.
//
///Fractional Seconds
/// - - - - - - - - -
// The fractional second is optional.  When the fractional second is absent, it
// is treated as if '.0' were specified.  When the fractional second is
// present, it can have one or more digits (i.e., it can contain more than
// three).  If more than three digits are included in the fractional second,
// values greater than or equal to .9995 are rounded up to 1000 milliseconds.
// This incurs a carry of one second into the 'second' attribute of the 'Time'
// component:
//..
//  +--------------------------------------+---------------------------------+
//  |        Parsed ISO 8601 String        |      Result Object Value        |
//  +======================================+=================================+
//  |  15:46:09.1                          |  Time(15, 46, 09, 100)          |
//  +--------------------------------------+---------------------------------+
//  |  15:46:09-5:00                       |  TimeTz(Time(15, 46, 09, 000),  |
//  |                                      |         -300)                   |
//  |                                      |  # implied '.0'                 |
//  +--------------------------------------+---------------------------------+
//  |  15:46:09.99949                      |  Time(15, 46, 09, 999)          |
//  |                                      |  # truncate last two digits     |
//  +--------------------------------------+---------------------------------+
//  |  15:46:09.9995                       |  Time(15, 46, 10, 000)          |
//  |                                      |  # round up and carry           |
//  +--------------------------------------+---------------------------------+
//..
// Note that if a carry due to rounding of the fractional second would cause an
// overflow at the extreme upper end of the valid range of dates (i.e.,
// 9999/12/31), then parsing for 'Datetime' and 'DatetimeTz' would fail.
//
///Leap Seconds
/// - - - - - -
// Although leap seconds are not representable by 'bdlt::Time', positive leap
// seconds are supported by the parse functions.  A leap second is recognized
// when the value parsed for the 'second' attribute of a 'Time' is 60.  When a
// leap second is detected, the 'second' attribute is taken to be 59, so that
// the value of the 'Time' object can be validly set, then an additional second
// is added to the object.  Note that the possible carry incurred by a leap
// second has the same potential for overflow as may occur with fractional
// seconds that are rounded up (although in admittedly pathological cases).
//
// TBD Should we tighten up our handling of leap seconds?  The ISO 8601
// specification seems to suggest that a positive leap second can only be
// represented as "23:59:60Z", but we allow 60 (for the 'second') to be coupled
// with any 'hour', 'minute', 'millisecond', and zone designator.
//
///The Time 24:00
/// - - - - - - -
// According to the ISO 8601 specification, the time 24:00 is interpreted as
// midnight, i.e., the last instant of a day.  However, this concept is not
// representable by 'bdlt'.  Furthermore, 24:00 can only be paired with the
// default date, 0001/01/01 (in a 'Datetime' or 'DatetimeTz' object), or with a
// timezone offset of 0.  If 24:00 is detected while parsing an ISO 8601
// string, the 'hour' attribute of 'Time' is set to 24 only if needed to
// preserve the default value for a 'Time', 'TimeTz', 'Datetime', or
// 'DatetimeTz'.  Otherwise, if 24:00 is paired with any date other than the
// default, the resulting object is set to represent the first instant of the
// next day (i.e., the date component is advanced by one day and the time
// component is set to 00:00).  Note that parsing fails if the zone designator
// is not equivalent to "+00:00", whether implied or explicit, when 24:00 is
// encountered:
//..
//  +------------------------------------+-----------------------------------+
//  |       Parsed ISO 8601 String       |        Result Object Value        |
//  +====================================+===================================+
//  |  24:00:00.000                      |  Time(24, 0, 0, 0)                |
//  |                                    |  # preserve default 'Time' value  |
//  +------------------------------------+-----------------------------------+
//  |  24:00:00.000-4:00                 |  TimeTz: parsing fails            |
//  |                                    |  # zone designator not UTC        |
//  +------------------------------------+-----------------------------------+
//  |  0001-01-01T24:00:00.000           |  Datetime(Date(0001, 01, 01),     |
//  |                                    |           Time(24, 0, 0, 0))      |
//  |                                    |  # preserve 'Datetime' default    |
//  |                                    |  # value                          |
//  +------------------------------------+-----------------------------------+
//  |  2002-03-17T24:00:00.000           |  Datetime(Date(2002, 03, 18),     |
//  |                                    |           Time(0, 0, 0, 0))       |
//  |                                    |  # first instant of the next day  |
//  |                                    |  # if 'Date' not 0001/01/01       |
//  +------------------------------------+-----------------------------------+
//..
// TBD the implementation has not yet been changed to match the above table
// with respect to 24:00 being treated as the first instant of the next day.
// This warrants some discussion before doing so.
//
///Summary of Supported ISO 8601 Representations
///- - - - - - - - - - - - - - - - - - - - - - -
// The syntax description below summarizes the ISO 8601 string representations
// supported by this component.  Although not quoted (for readability),
// '[+-:.,TZ]' are literal characters that can occur in ISO 8601 strings.  The
// characters '[YMDhms]' each denote a decimal digit, '{}' brackets optional
// elements, '()' is used for grouping, and '|' separates alternatives:
//..
// <Generated Date>        ::=  <DATE>
//
// <Parsed Date>           ::=  <Parsed DateTz>
//
// <Generated DateTz>      ::=  <DATE><ZONE>
//
// <Parsed DateTz>         ::=  <DATE>{<ZONE>}
//
// <Generated Time>        ::=  <TIME FIXED>
//
// <Parsed Time>           ::=  <Parsed TimeTz>
//
// <Generated TimeTz>      ::=  <TIME FIXED><ZONE>
//
// <Parsed TimeTz>         ::=  <TIME FLEXIBLE>{<ZONE>}
//
// <Generated Datetime>    ::=  <DATE>T<TIME FIXED>
//
// <Parsed Datetime>       ::=  <Parsed DatetimeTz>
//
// <Generated DatetimeTz>  ::=  <DATE>T<TIME FIXED><ZONE>
//
// <Parsed DatetimeTz>     ::=  <DATE>T<TIME FLEXIBLE>{<ZONE>}
//
// <DATE>                  ::=  YYYY-MM-DD
//
// <TIME FIXED>            ::=  hh:mm:ss(.|,)sss   # exactly three digits in
//                                                 # the fractional second
//
// <TIME FLEXIBLE>         ::=  hh:mm:ss{(.|,)s+}  # one or more digits in the
//                                                 # fractional second
//
// <ZONE>                  ::=  (+|-)hh{:}mm|Z     # zone designator
//..
///Usage
///-----
// TBD recast Usage in new style
//
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
//
// TBD provide a second Usage example illustrating use of a configuration

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
