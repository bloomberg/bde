// bdlt_fixutil.h                                                     -*-C++-*-
#ifndef INCLUDED_BDLT_FIXUTIL
#define INCLUDED_BDLT_FIXUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide conversions between date/time objects and FIX strings.
//
//@CLASSES:
//  bdlt::FixUtil: namespace for FIX date/time conversion functions
//
//@SEE_ALSO: bdlt_fixutilconfiguration
//
//@DESCRIPTION: This component provides a namespace, 'bdlt::FixUtil',
// containing functions that convert 'bdlt' date, time, and datetime objects to
// and from ("generate" and "parse", respectively) corresponding string
// representations that are compliant with the FIX standard.  The version of
// the FIX standard that is the basis for this component can be found at:
//..
//  http://www.fixtradingcommunity.org/FIXimate/FIXimate3.0/latestEP/en/
//                                          FIX.5.0SP2_EP208/fix_datatypes.html
//..
// In general terms, 'FixUtil' functions support what FIX refers to as
// *complete* *representations* in *extended* *format*.  We first present a
// brief overview before delving into the details of the FIX representations
// that are supported for each of the relevant 'bdlt' vocabulary types.
//
// Each function that *generates* FIX strings (named 'generate' and
// 'generateRaw') takes a 'bdlt' object and a 'char *' buffer, 'bsl::string',
// or 'bsl::ostream', and writes a FIX representation of the object to the
// buffer, string, or stream.  The "raw" functions are distinguished from their
// non-"raw" counterparts in three respects:
//
//: o The length of the 'char *' buffer is not supplied to the 'generateRaw'
//:   functions.
//:
//: o The 'generateRaw' functions do not output a null terminator.
//:
//: o The 'generate' functions that provide an 'int bufferLength' parameter
//:   truncate the generated output to 'bufferLength' characters.  (Neither the
//:   'generateRaw' functions nor the 'generate' functions taking 'bsl::string'
//:   or 'bsl::ostream' do any truncation of their generated output.)
//
// Since the generate functions always succeed, no status value is returned.
// Instead, either the number of characters output to the 'char *' buffer or
// string, or a reference to the stream, is returned.  (Note that the
// generating functions also take an optional 'bdlt::FixUtilConfiguration'
// object, which is discussed shortly.)
//
// Each function that *parses* FIX strings (named 'parse') take the address of
// a target 'bdlt' object and a 'const char *' (paired with a 'length'
// argument) or 'bslstl::StringRef', and loads the object with the result of
// parsing the character string.  Since parsing can fail, the parse functions
// return an 'int' status value (0 for success and a non-zero value for
// failure).  Note that, besides elementary syntactical considerations, the
// validity of parsed strings are subject to the semantic constraints imposed
// by the various 'isValid*' class methods (i.e., 'Date::isValidYearMonthDay',
// 'Time::isValid', etc.).
//
///Terminology
///-----------
// As this component concerns FIX, some terms from that specification are used
// liberally in what follows.  Two FIX terms of particular note are *timezone*
// *offset* and *fractional* *second*.
//
// A FIX *timezone* *offset* corresponds to what other 'bdlt' components
// commonly refer to as a timezone offset (or simply as an offset; e.g., see
// 'bdlt_datetimetz').  For example, the FIX string '20020317-15:46:00+04:00'
// has a timezone offset of '+04:00', indicating a timezone 4 hours ahead of
// UTC.
//
// A FIX *fractional* *second* corresponds to, for example, the combined
// 'millisecond' and 'microsecond' attributes of a 'bdlt::Time' object, or the
// combined 'millisecond' and 'microsecond' attributes of a 'bdlt::Datetime'
// object.  For example, the 'Time' value (and FIX string) '15:46:09.330000'
// has a 'millisecond' attribute value of 330 and a microsecond attribute of 0
// (i.e., a fractional second of .33).
//
///FIX String Generation
///---------------------
// Strings produced by the 'generate' and 'generateRaw' functions are a
// straightforward transposition of the attributes of the source 'bdlt' value
// into an appropriate FIX format, and are best illustrated by a few examples.
// Note that for 'Datetime', 'DatetimeTz', and 'Time', the fractional second is
// generated with the precision specified in the configuration.  Also note that
// for 'TimeTz', no fractional second is generated (as per the FIX
// specification for "TZTimeOnly").
//..
//  +--------------------------------------+---------------------------------+
//  |             Object Value             |      Generated FIX String       |
//  |                                      |  (using default configuration)  |
//  +======================================+=================================+
//  |  Date(2002, 03, 17)                  |  20020317                       |
//  +--------------------------------------+---------------------------------+
//  |  Time(15, 46, 09, 330)               |  15:46:09.330                   |
//  +--------------------------------------+---------------------------------+
//  |  Datetime(Date(2002, 03, 17)         |                                 |
//  |           Time(15, 46, 09, 330))     |  20020317-15:46:09.330          |
//  +--------------------------------------+---------------------------------+
//  |  DateTz(Date(2002, 03, 17), -120)    |  20020317-02:00                 |
//  +--------------------------------------+---------------------------------+
//  |  TimeTz(Time(15, 46, 09, 330), 270)  |  15:46:09+04:30                 |
//  +--------------------------------------+---------------------------------+
//  |  DatetimeTz(Datetime(                |                                 |
//  |              Date(2002, 03, 17),     |                                 |
//  |              Time(15, 46, 09, 330)), |                                 |
//  |             0)                       |  20020317-15:46:09.330+00:00    |
//  +--------------------------------------+---------------------------------+
//..
// Note that the FIX specification does not have an equivalent to
// 'bdlt::DateTz'.
//
///Configuration
///- - - - - - -
// The 'generate' and 'generateRaw' functions provide an optional configuration
// parameter.  This optional parameter, of type 'FixUtilConfiguration', enables
// configuration of two aspects of FIX string generation:
//
//: o The precision of the fractional seconds.
//:
//: o Whether 'Z' is output for the timezone offset instead of '+00:00' (UTC).
//
// 'FixUtilConfiguration' has two attributes that directly correspond to these
// aspects.  In addition, for generate methods that are not supplied with a
// configuration argument, a process-wide configuration takes effect.  See
// 'bdlt_fixutilconfiguration' for details.
//
///FIX String Parsing
///------------------
// The parse functions accept *all* strings that are produced by the generate
// functions.  In addition, the parse functions accept some variation in the
// generated strings, the details of which are discussed next.  Note that the
// parse methods are not configurable like the generate methods (i.e., via an
// optional 'FixUtilConfiguration' argument).  Moreover, the process-wide
// configuration has no effect on parsing either.  Instead, the parse methods
// automatically treat '+00:00' and 'Z' as equivalent timezone offsets (both
// denoting UTC).  Finally, the parsing allows seconds to be optionally
// specified in all types, which is in contradiction to some of the types in
// the referenced FIX protocol specification.
//
///Timezone Offsets
/// - - - - - - - -
// The timezone offset is optional, and can be present when parsing for *any*
// type, i.e., even for 'Date', 'Time', and 'Datetime'.  If a timezone offset
// is parsed for a 'Date', it must be valid, so it can affect the status value
// that is returned in that case, but it is otherwise ignored.  For 'Time' and
// 'Datetime', any timezone offset present in the parsed string will affect the
// resulting object value (unless the timezone offset denotes UTC) because the
// result is converted to UTC.  If the timezone offset is absent, it is treated
// as if '+00:00' were specified:
//..
//  +------------------------------------+-----------------------------------+
//  |         Parsed FIX String          |        Result Object Value        |
//  +====================================+===================================+
//  |  20020317-02:00                    |  Date(2002, 03, 17)               |
//  |                                    |  # timezone offset ignored        |
//  +------------------------------------+-----------------------------------+
//  |  20020317-02:65                    |  Date: parsing fails              |
//  |                                    |  # invalid timezone offset        |
//  +------------------------------------+-----------------------------------+
//  |  15:46:09.330+04:30                |  Time(11, 16, 09, 330)            |
//  |                                    |  # converted to UTC               |
//  +------------------------------------+-----------------------------------+
//  |  15:46:09.330+04:30                |  TimeTz(Time(15, 46, 09, 330),    |
//  |                                    |         270)                      |
//  +------------------------------------+-----------------------------------+
//  |  15:46:09.330                      |  TimeTz(Time(15, 46, 09, 330),    |
//  |                                    |         0)                        |
//  |                                    |  # implied '+00:00'               |
//  +------------------------------------+-----------------------------------+
//  |  20020317-23:46:09.222-05:00       |  Datetime(Date(2002, 03, 18),     |
//  |                                    |           Time(04, 46, 09, 222))  |
//  |                                    |  # carry into 'day' attribute     |
//  |                                    |  # when converted to UTC          |
//  +------------------------------------+-----------------------------------+
//..
// In the last example above, the conversion to UTC incurs a carry into the
// 'day' attribute of the 'Date' component of the resulting 'Datetime' value.
// Note that if such a carry causes an underflow or overflow at the extreme
// ends of the valid range of dates (0001/01/01 and 9999/12/31), then parsing
// for 'Datetime' fails.
//
///Fractional Seconds
/// - - - - - - - - -
// The fractional second is optional.  When the fractional second is absent, it
// is treated as if '.0' were specified.  When the fractional second is
// present, it can have one or more digits (in divergence with the referenced
// FIX protocol document, which indicates the fractional second may be
// unspecified or have a positive multiple of three digits).  Although FIX has
// provision for picosecond (or finer) time resolution, be aware that 'bdlt' is
// limited to microsecond resolution.  If more than six digits are included in
// the fractional second, values are rounded to a full microsecond; i.e.,
// values greater than or equal to .5 microseconds are rounded up.  These
// roundings may incur a carry of one second into the 'second' attribute:
//..
//  +--------------------------------------+---------------------------------+
//  |          Parsed FIX String           |      Result Object Value        |
//  +======================================+=================================+
//  |  15:46:09.1                          |  Time(15, 46, 09, 100)          |
//  +--------------------------------------+---------------------------------+
//  |  15:46:09-05:00                      |  TimeTz(Time(15, 46, 09, 000),  |
//  |                                      |         -300)                   |
//  |                                      |  # implied '.0'                 |
//  +--------------------------------------+---------------------------------+
//  |  15:46:09.99999949                   |  Time(15, 46, 09, 999, 999)     |
//  |                                      |  # truncate last two digits     |
//  +--------------------------------------+---------------------------------+
//  |  15:46:09.9999995                    |  Time(15, 46, 10, 000)          |
//  |                                      |  # round up and carry           |
//  +--------------------------------------+---------------------------------+
//..
// Note that, for 'Datetime' and 'DatetimeTz', if a carry due to rounding of
// the fractional second causes an overflow at the extreme upper end of the
// valid range of dates (i.e., 9999/12/31), then parsing fails.
//
///Leap Seconds
/// - - - - - -
// Leap seconds are not representable by 'bdlt::Time' or 'bdlt::Datetime'.
// Hence, they are not produced by any of the 'FixUtil' generate functions.
// However, positive leap seconds *are* supported by the parse functions.  A
// leap second is recognized when the value parsed for the 'second' attribute
// of a 'Time' is 60 -- regardless of the values parsed for the 'hour',
// 'minute', and 'millisecond' attributes.  Note that this behavior is more
// generous than that afforded by the FIX specification (which indicates that a
// positive leap second can only be represented as "23:59:60Z").
//
// When a leap second is detected during parsing of a FIX string, the 'second'
// attribute is taken to be 59, so that the value of the 'Time' object can be
// validly set; then an additional second is added to the object.  Note that
// the possible carry incurred by a leap second (i.e., when loading the result
// of parsing into a 'Datetime' or 'DatetimeTz' object) has the same potential
// for overflow as may occur with fractional seconds that are rounded up
// (although in admittedly pathological cases).
//
///The Time 24:00
/// - - - - - - -
// Although 24:00 is *representable* by 'bdlt', i.e., as the default value for
// 'bdlt::Time', "24:00:00.000" is *not* a valid string in the FIX protocol.
// As per other methods acting upon 24:00 within 'bdlt', an 'hour' attribute
// value of 24 is mapped to 0 by the generate functions provided by this
// component:
//..
//  +------------------------------------+-----------------------------------+
//  |        Source Object Value         |       Generated FIX String        |
//  +====================================+===================================+
//  |  Time(24, 0, 0, 0)                 |  00:00:00.000                     |
//  +------------------------------------+-----------------------------------+
//  |  Datetime(Date(2002, 03, 17),      |  20020317-00:00:00.000            |
//  |           Time(24, 0, 0, 0))       |                                   |
//  +------------------------------------+-----------------------------------+
//..
// Finally, a string representing 24:00 is rejected by the 'bdlt::FixUtil'
// parse methods.
//
///Summary of Supported FIX Representations
///- - - - - - - - - - - - - - - - - - - -
// The syntax description below summarizes the FIX string representations
// supported by this component.  Although not quoted (for readability),
// '[+-:.Z]' are literal characters that can occur in FIX strings.  The
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
// <Generated Time>        ::=  <TIME FLEXIBLE>
//
// <Parsed Time>           ::=  <Parsed TimeTz>
//
// <Generated TimeTz>      ::=  <TIME FIXED><ZONE>
//
// <Parsed TimeTz>         ::=  <TIME FLEXIBLE>{<ZONE>}
//
// <Generated Datetime>    ::=  <DATE>-<TIME FLEXIBLE>
//
// <Parsed Datetime>       ::=  <Parsed DatetimeTz>
//
// <Generated DatetimeTz>  ::=  <DATE>-<TIME FLEXIBLE><ZONE>
//
// <Parsed DatetimeTz>     ::=  <DATE>-<TIME FLEXIBLE>{<ZONE>}
//
// <DATE>                  ::=  YYYYMMDD
//
// <TIME FIXED>            ::=  hh:mm:ss
//
// <TIME FLEXIBLE>         ::=  hh:mm{:ss{.s+}}
//
// <ZONE>                  ::=  ((+|-)hh{:mm})|Z  # timezone offset, the colon
//                                                # and minute attribute are
//                                                # optional during parsing
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'bdlt::FixUtil' Usage
/// - - - - - - - - - - - - - - - - - - -
// This example demonstrates basic use of one 'generate' function and two
// 'parse' functions.
//
// First, we construct a few objects that are prerequisites for this and the
// following example:
//..
//  const bdlt::Date date(2005, 1, 31);     // 2005/01/31
//  const bdlt::Time time(8, 59, 59, 123);  // 08:59:59.123
//  const int        tzOffset = 240;        // +04:00 (four hours west of UTC)
//..
// Then, we construct a 'bdlt::DatetimeTz' object for which a corresponding
// FIX-compliant string will be generated shortly:
//..
//  const bdlt::DatetimeTz sourceDatetimeTz(bdlt::Datetime(date, time),
//                                          tzOffset);
//..
// For comparison with the FIX string generated below, note that streaming the
// value of 'sourceDatetimeTz' to 'stdout':
//..
//  bsl::cout << sourceDatetimeTz << bsl::endl;
//..
// produces:
//..
//  31JAN2005_08:59:59.123000+0400
//..
// Next, we use a 'generate' function to produce a FIX-compliant string for
// 'sourceDatetimeTz', writing the output to a 'bsl::ostringstream', and assert
// that both the return value and the string that is produced are as expected:
//..
//  bsl::ostringstream  oss;
//  const bsl::ostream& ret = bdlt::FixUtil::generate(oss, sourceDatetimeTz);
//  assert(&oss == &ret);
//
//  const bsl::string fix = oss.str();
//  assert(fix == "20050131-08:59:59.123+04:00");
//..
// For comparison, see the output that was produced by the streaming operator
// above.
//
// Now, we parse the string that was just produced, loading the result of the
// parse into a second 'bdlt::DatetimeTz' object, and assert that the parse was
// successful and that the target object has the same value as that of the
// original (i.e., 'sourceDatetimeTz'):
//..
//  bdlt::DatetimeTz targetDatetimeTz;
//
//  int rc = bdlt::FixUtil::parse(&targetDatetimeTz,
//                                fix.c_str(),
//                                static_cast<int>(fix.length()));
//  assert(               0 == rc);
//  assert(sourceDatetimeTz == targetDatetimeTz);
//..
// Finally, we parse the 'fix' string a second time, this time loading the
// result into a 'bdlt::Datetime' object (instead of a 'bdlt::DatetimeTz'):
//..
//  bdlt::Datetime targetDatetime;
//
//  rc = bdlt::FixUtil::parse(&targetDatetime,
//                            fix.c_str(),
//                            static_cast<int>(fix.length()));
//  assert(                             0 == rc);
//  assert(sourceDatetimeTz.utcDatetime() == targetDatetime);
//..
// Note that this time the value of the target object has been converted to
// UTC.
//
///Example 2: Configuring FIX String Generation
///- - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates use of a 'bdlt::FixUtilConfiguration' object to
// influence the format of the FIX strings that are generated by this component
// by passing that configuration object to 'generate'.  We also take this
// opportunity to illustrate the flavor of the 'generate' functions that
// outputs to a 'char *' buffer of a specified length.
//
// First, we construct the 'bdlt::FixUtilConfiguration' object that indicates
// how we would like to affect the generated output FIX string.  In this case,
// we want to have microsecond precision displayed:
//..
//  bdlt::FixUtilConfiguration configuration;
//
//  configuration.setFractionalSecondPrecision(6);
//..
// Then, we define the 'char *' buffer that will be used to stored the
// generated string.  A buffer of size 'bdlt::FixUtil::k_DATETIMETZ_STRLEN + 1'
// is large enough to hold any string generated by this component for a
// 'bdlt::DatetimeTz' object, including a null terminator:
//..
//  const int BUFLEN = bdlt::FixUtil::k_DATETIMETZ_STRLEN + 1;
//  char      buffer[BUFLEN];
//..
// Next, we use a 'generate' function that accepts our 'configuration' to
// produce a FIX-compliant string for 'sourceDatetimeTz', this time writing the
// output to a 'char *' buffer, and assert that both the return value and the
// string that is produced are as expected.  Note that in comparing the return
// value against 'BUFLEN - 1' we account for the fact that, although a null
// terminator was generated, it is not included in the character count returned
// by 'generate'.  Also note that we use 'bsl::strcmp' to compare the resulting
// string knowing that we supplied a buffer having sufficient capacity to
// accommodate a null terminator:
//..
//  rc = bdlt::FixUtil::generate(buffer,
//                               BUFLEN,
//                               sourceDatetimeTz,
//                               configuration);
//  assert(BUFLEN - 1 == rc);
//  assert(         0 == bsl::strcmp(buffer,
//                                   "20050131-08:59:59.123000+04:00"));
//..
// For comparison, see the output that was produced by the streaming operator
// above.
//
// Next, we parse the string that was just produced, loading the result of the
// parse into a second 'bdlt::DatetimeTz' object, and assert that the parse was
// successful and that the target object has the same value as that of the
// original (i.e., 'sourceDatetimeTz').  Note that 'BUFLEN - 1' is passed and
// *not* 'BUFLEN' because the former indicates the correct number of characters
// in 'buffer' that we wish to parse:
//..
//  rc = bdlt::FixUtil::parse(&targetDatetimeTz, buffer, BUFLEN - 1);
//
//  assert(               0 == rc);
//  assert(sourceDatetimeTz == targetDatetimeTz);
//..
// Then, we parse the string in 'buffer' a second time, this time loading the
// result into a 'bdlt::Datetime' object (instead of a 'bdlt::DatetimeTz'):
//..
//  rc = bdlt::FixUtil::parse(&targetDatetime, buffer, BUFLEN - 1);
//
//  assert(                             0 == rc);
//  assert(sourceDatetimeTz.utcDatetime() == targetDatetime);
//..
// Note that this time the value of the target object has been converted to
// UTC.
//
// Finally, we modify the 'configuration' to display the 'bdlt::DatetimeTz'
// without fractional seconds:
//..
//  configuration.setFractionalSecondPrecision(0);
//  rc = bdlt::FixUtil::generate(buffer,
//                               BUFLEN,
//                               sourceDatetimeTz,
//                               configuration);
//  assert(BUFLEN - 8 == rc);
//  assert(         0 == bsl::strcmp(buffer, "20050131-08:59:59+04:00"));
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_FIXUTILCONFIGURATION
#include <bdlt_fixutilconfiguration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {
namespace bdlt {

class Date;
class DateTz;
class Datetime;
class DatetimeTz;
class Time;
class TimeTz;

class FixUtilConfiguration;

                              // ==============
                              // struct FixUtil
                              // ==============

struct FixUtil {
    // This 'struct' provides a namespace for a suite of pure functions that
    // perform conversions between objects of 'bdlt' vocabulary type and their
    // FIX representations.  Each 'generate' and 'generateRaw' method takes a
    // 'bdlt' object (of type 'Date', 'DateTz', 'Time', 'TimeTz', 'Datetime',
    // or 'DatetimeTz') and outputs its corresponding FIX representation to a
    // user-supplied character buffer or 'bsl::ostream'.  The 'parse' methods
    // effect the opposite conversion in that they populate a 'bdlt' object
    // from the result of parsing a FIX representation.

    // TYPES
    enum {
        // This enumeration defines fixed lengths for the FIX representations
        // of date, time, and datetime values.  Note that these constants do
        // *not* account for the null terminator that may be produced by the
        // 'generate' functions taking a 'bufferLength' argument.

        k_DATE_STRLEN       =  8,  // 'bdlt::Date'
        k_DATETZ_STRLEN     = 14,  // 'bdlt::DateTz'

        k_TIME_STRLEN       = 15,  // 'bdlt::Time'
        k_TIMETZ_STRLEN     = 14,  // 'bdlt::TimeTz'

        k_DATETIME_STRLEN   = 24,  // 'bdlt::Datetime'
        k_DATETIMETZ_STRLEN = 30,  // 'bdlt::DatetimeTz'

        k_MAX_STRLEN        = k_DATETIMETZ_STRLEN
    };

    // CLASS METHODS
    static int generate(char                        *buffer,
                        int                          bufferLength,
                        const Date&                  object);
    static int generate(char                        *buffer,
                        int                          bufferLength,
                        const Date&                  object,
                        const FixUtilConfiguration&  configuration);
    static int generate(char                        *buffer,
                        int                          bufferLength,
                        const Time&                  object);
    static int generate(char                        *buffer,
                        int                          bufferLength,
                        const Time&                  object,
                        const FixUtilConfiguration&  configuration);
    static int generate(char                        *buffer,
                        int                          bufferLength,
                        const Datetime&              object);
    static int generate(char                        *buffer,
                        int                          bufferLength,
                        const Datetime&              object,
                        const FixUtilConfiguration&  configuration);
    static int generate(char                        *buffer,
                        int                          bufferLength,
                        const DateTz&                object);
    static int generate(char                        *buffer,
                        int                          bufferLength,
                        const DateTz&                object,
                        const FixUtilConfiguration&  configuration);
    static int generate(char                        *buffer,
                        int                          bufferLength,
                        const TimeTz&                object);
    static int generate(char                        *buffer,
                        int                          bufferLength,
                        const TimeTz&                object,
                        const FixUtilConfiguration&  configuration);
    static int generate(char                        *buffer,
                        int                          bufferLength,
                        const DatetimeTz&            object);
    static int generate(char                        *buffer,
                        int                          bufferLength,
                        const DatetimeTz&            object,
                        const FixUtilConfiguration&  configuration);
        // Write the FIX representation of the specified 'object' to the
        // specified 'buffer' of the specified 'bufferLength' (in bytes),
        // truncating (if necessary) to 'bufferLength'.  Optionally specify a
        // 'configuration' to affect the format of the generated string.  If
        // 'configuration' is not supplied, the process-wide default value
        // 'FixUtilConfiguration::defaultConfiguration()' is used.  Return the
        // number of characters in the formatted string before truncation (not
        // counting a null terminator).  If 'bufferLength' indicates sufficient
        // capacity, 'buffer' is null terminated.  The behavior is undefined
        // unless '0 <= bufferLength'.  Note that a buffer of size
        // 'k_MAX_STRLEN + 1' is large enough to hold any string generated by
        // this component (counting a null terminator, if any).

    static int generate(bsl::string                 *string,
                        const Date&                  object);
    static int generate(bsl::string                 *string,
                        const Date&                  object,
                        const FixUtilConfiguration&  configuration);
    static int generate(bsl::string                 *string,
                        const Time&                  object);
    static int generate(bsl::string                 *string,
                        const Time&                  object,
                        const FixUtilConfiguration&  configuration);
    static int generate(bsl::string                 *string,
                        const Datetime&              object);
    static int generate(bsl::string                 *string,
                        const Datetime&              object,
                        const FixUtilConfiguration&  configuration);
    static int generate(bsl::string                 *string,
                        const DateTz&                object);
    static int generate(bsl::string                 *string,
                        const DateTz&                object,
                        const FixUtilConfiguration&  configuration);
    static int generate(bsl::string                 *string,
                        const TimeTz&                object);
    static int generate(bsl::string                 *string,
                        const TimeTz&                object,
                        const FixUtilConfiguration&  configuration);
    static int generate(bsl::string                 *string,
                        const DatetimeTz&            object);
    static int generate(bsl::string                 *string,
                        const DatetimeTz&            object,
                        const FixUtilConfiguration&  configuration);
        // Load the FIX representation of the specified 'object' into the
        // specified 'string'.  Optionally specify a 'configuration' to affect
        // the format of the generated string.  If 'configuration' is not
        // supplied, the process-wide default value
        // 'FixUtilConfiguration::defaultConfiguration()' is used.  Return the
        // number of characters in the formatted string.  The previous contents
        // of 'string' (if any) are discarded.

    static bsl::ostream& generate(bsl::ostream&               stream,
                                  const Date&                 object);
    static bsl::ostream& generate(bsl::ostream&               stream,
                                  const Date&                 object,
                                  const FixUtilConfiguration& configuration);
    static bsl::ostream& generate(bsl::ostream&               stream,
                                  const Time&                 object);
    static bsl::ostream& generate(bsl::ostream&               stream,
                                  const Time&                 object,
                                  const FixUtilConfiguration& configuration);
    static bsl::ostream& generate(bsl::ostream&               stream,
                                  const Datetime&             object);
    static bsl::ostream& generate(bsl::ostream&               stream,
                                  const Datetime&             object,
                                  const FixUtilConfiguration& configuration);
    static bsl::ostream& generate(bsl::ostream&               stream,
                                  const DateTz&               object);
    static bsl::ostream& generate(bsl::ostream&               stream,
                                  const DateTz&               object,
                                  const FixUtilConfiguration& configuration);
    static bsl::ostream& generate(bsl::ostream&               stream,
                                  const TimeTz&               object);
    static bsl::ostream& generate(bsl::ostream&               stream,
                                  const TimeTz&               object,
                                  const FixUtilConfiguration& configuration);
    static bsl::ostream& generate(bsl::ostream&               stream,
                                  const DatetimeTz&           object);
    static bsl::ostream& generate(bsl::ostream&               stream,
                                  const DatetimeTz&           object,
                                  const FixUtilConfiguration& configuration);
        // Write the FIX representation of the specified 'object' to the
        // specified 'stream'.  Optionally specify a 'configuration' to affect
        // the format of the generated string.  If 'configuration' is not
        // supplied, the process-wide default value
        // 'FixUtilConfiguration::defaultConfiguration()' is used.  Return a
        // reference to 'stream'.  Note that 'stream' is not null terminated.

    static int generateRaw(char                        *buffer,
                           const Date&                  object);
    static int generateRaw(char                        *buffer,
                           const Date&                  object,
                           const FixUtilConfiguration&  configuration);
    static int generateRaw(char                        *buffer,
                           const Time&                  object);
    static int generateRaw(char                        *buffer,
                           const Time&                  object,
                           const FixUtilConfiguration&  configuration);
    static int generateRaw(char                        *buffer,
                           const Datetime&              object);
    static int generateRaw(char                        *buffer,
                           const Datetime&              object,
                           const FixUtilConfiguration&  configuration);
    static int generateRaw(char                        *buffer,
                           const DateTz&                object);
    static int generateRaw(char                        *buffer,
                           const DateTz&                object,
                           const FixUtilConfiguration&  configuration);
    static int generateRaw(char                        *buffer,
                           const TimeTz&                object);
    static int generateRaw(char                        *buffer,
                           const TimeTz&                object,
                           const FixUtilConfiguration&  configuration);
    static int generateRaw(char                        *buffer,
                           const DatetimeTz&            object);
    static int generateRaw(char                        *buffer,
                           const DatetimeTz&            object,
                           const FixUtilConfiguration&  configuration);
        // Write the FIX representation of the specified 'object' to the
        // specified 'buffer'.  Optionally specify a 'configuration' to affect
        // the format of the generated string.  If 'configuration' is not
        // supplied, the process-wide default value
        // 'FixUtilConfiguration::defaultConfiguration()' is used.  Return the
        // number of characters in the formatted string.  'buffer' is not null
        // terminated.  The behavior is undefined unless 'buffer' has
        // sufficient capacity.  Note that a buffer of size 'k_MAX_STRLEN + 1'
        // is large enough to hold any string generated by this component
        // (counting a null terminator, if any).

    static int parse(Date *result, const char *string, int length);
        // Parse the specified initial 'length' characters of the specified FIX
        // 'string' as a 'Date' value, and load the value into the specified
        // 'result'.  Return 0 on success, and a non-zero value (with no
        // effect) otherwise.  'string' is assumed to be of the form:
        //..
        //  YYYYMMDD{(+|-)hh{:mm}|Z}
        //..
        // *Exactly* 'length' characters are parsed; parsing will fail if a
        // proper prefix of 'string' matches the expected format, but the
        // entire 'length' characters do not.  If the optional timezone offset
        // is present in 'string', it is parsed but ignored.  The behavior is
        // undefined unless '0 <= length'.

    static int parse(Time *result, const char *string, int length);
        // Parse the specified initial 'length' characters of the specified FIX
        // 'string' as a 'Time' value, and load the value into the specified
        // 'result'.  Return 0 on success, and a non-zero value (with no
        // effect) otherwise.  'string' is assumed to be of the form:
        //..
        //  hh:mm:ss{.s+}{(+|-)hh{:mm}|Z}
        //..
        // *Exactly* 'length' characters are parsed; parsing will fail if a
        // proper prefix of 'string' matches the expected format, but the
        // entire 'length' characters do not.  If an optional fractional second
        // having more than three digits is present in 'string', it is rounded
        // to the nearest value in milliseconds.  If the optional timezone
        // offset is present in 'string', the resulting 'Time' value is
        // converted to the equivalent UTC time; if the timezone offset is
        // absent, UTC is assumed.  If a leap second is detected (i.e., the
        // parsed value of the 'second' attribute is 60; see {Leap Seconds}),
        // the 'second' attribute is taken to be 59, then an additional second
        // is added to 'result' at the end.  The behavior is undefined unless
        // '0 <= length'.

    static int parse(Datetime *result, const char *string, int length);
        // Parse the specified initial 'length' characters of the specified FIX
        // 'string' as a 'Datetime' value, and load the value into the
        // specified 'result'.  Return 0 on success, and a non-zero value (with
        // no effect) otherwise.  'string' is assumed to be of the form:
        //..
        //  YYYYMMDD-hh:mm{:ss{.s+}}{(+|-)hh{:mm}|Z}
        //..
        // *Exactly* 'length' characters are parsed; parsing will fail if a
        // proper prefix of 'string' matches the expected format, but the
        // entire 'length' characters do not.  If an optional fractional second
        // having more than six digits is present in 'string', it is rounded to
        // the nearest value in microseconds.  If the optional timezone offset
        // is present in 'string', the resulting 'Datetime' value is converted
        // to the equivalent UTC value; if the timezone offset is absent, UTC
        // is assumed.  If a leap second is detected (i.e., the parsed value of
        // the 'second' attribute is 60; see {Leap Seconds}), the 'second'
        // attribute is taken to be 59, then an additional second is added to
        // 'result' at the end.  The behavior is undefined unless
        // '0 <= length'.

    static int parse(DateTz *result, const char *string, int length);
        // Parse the specified initial 'length' characters of the specified FIX
        // 'string' as a 'DateTz' value, and load the value into the specified
        // 'result'.  Return 0 on success, and a non-zero value (with no
        // effect) otherwise.  'string' is assumed to be of the form:
        //..
        //  YYYYMMDD{(+|-)hh{:mm}|Z}
        //..
        // *Exactly* 'length' characters are parsed; parsing will fail if a
        // proper prefix of 'string' matches the expected format, but the
        // entire 'length' characters do not.  If the optional timezone offset
        // is not present in 'string', UTC is assumed.  The behavior is
        // undefined unless '0 <= length'.

    static int parse(TimeTz *result, const char *string, int length);
        // Parse the specified initial 'length' characters of the specified FIX
        // 'string' as a 'TimeTz' value, and load the value into the specified
        // 'result'.  Return 0 on success, and a non-zero value (with no
        // effect) otherwise.  'string' is assumed to be of the form:
        //..
        //  hh:mm{:ss{.s+}}{(+|-)hh{:mm}|Z}
        //..
        // *Exactly* 'length' characters are parsed; parsing will fail if a
        // proper prefix of 'string' matches the expected format, but the
        // entire 'length' characters do not.  If an optional fractional second
        // having more than three digits is present in 'string', it is rounded
        // to the nearest value in milliseconds.  If the optional timezone
        // offset is not present in 'string', UTC is assumed.  If a leap second
        // is detected (i.e., the parsed value of the 'second' attribute is 60;
        // see {Leap Seconds}), the 'second' attribute is taken to be 59, then
        // an additional second is added to 'result' at the end.  The behavior
        // is undefined unless '0 <= length'.

    static int parse(DatetimeTz *result, const char *string, int length);
        // Parse the specified initial 'length' characters of the specified FIX
        // 'string' as a 'DatetimeTz' value, and load the value into the
        // specified 'result'.  Return 0 on success, and a non-zero value (with
        // no effect) otherwise.  'string' is assumed to be of the form:
        //..
        //  YYYYMMDD-hh:mm{:ss{.s+}}{(+|-)hh{:mm}|Z}
        //..
        // *Exactly* 'length' characters are parsed; parsing will fail if a
        // proper prefix of 'string' matches the expected format, but the
        // entire 'length' characters do not.  If an optional fractional second
        // having more than six digits is present in 'string', it is rounded to
        // the nearest value in microseconds.  If the optional timezone offset
        // is not present in 'string', UTC is assumed.  If a leap second is
        // detected (i.e., the parsed value of the 'second' attribute is 60;
        // see {Leap Seconds}), the 'second' attribute is taken to be 59, then
        // an additional second is added to 'result' at the end.  The behavior
        // is undefined unless '0 <= length'.

    static int parse(Date *result, const bslstl::StringRef& string);
        // Parse the specified FIX 'string' as a 'Date' value, and load the
        // value into the specified 'result'.  Return 0 on success, and a
        // non-zero value (with no effect) otherwise.  'string' is assumed to
        // be of the form:
        //..
        //  YYYYMMDD{(+|-)hh{:mm}|Z}
        //..
        // *Exactly* 'string.length()' characters are parsed; parsing will fail
        // if a proper prefix of 'string' matches the expected format, but the
        // entire 'string.length()' characters do not.  If the optional
        // timezone offset is present in 'string', it is parsed but ignored.
        // The behavior is undefined unless 'string.data()' is non-null.

    static int parse(Time *result, const bslstl::StringRef& string);
        // Parse the specified FIX 'string' as a 'Time' value, and load the
        // value into the specified 'result'.  Return 0 on success, and a
        // non-zero value (with no effect) otherwise.  'string' is assumed to
        // be of the form:
        //..
        //  hh:mm{:ss{.s+}}{(+|-)hh{:mm}|Z}
        //..
        // *Exactly* 'string.length()' characters are parsed; parsing will fail
        // if a proper prefix of 'string' matches the expected format, but the
        // entire 'string.length()' characters do not.  If an optional
        // fractional second having more than three digits is present in
        // 'string', it is rounded to the nearest value in milliseconds.  If
        // the optional timezone offset is present in 'string', the resulting
        // 'Time' value is converted to the equivalent UTC time; if the
        // timezone offset is absent, UTC is assumed.  If a leap second is
        // detected (i.e., the parsed value of the 'second' attribute is 60;
        // see {Leap Seconds}), the 'second' attribute is taken to be 59, then
        // an additional second is added to 'result' at the end.  The behavior
        // is undefined unless 'string.data()' is non-null.

    static int parse(Datetime *result, const bslstl::StringRef& string);
        // Parse the specified FIX 'string' as a 'Datetime' value, and load the
        // value into the specified 'result'.  Return 0 on success, and a
        // non-zero value (with no effect) otherwise.  'string' is assumed to
        // be of the form:
        //..
        //  YYYYMMDD-hh:mm{:ss{.s+}}{(+|-)hh{:mm}|Z}
        //..
        // *Exactly* 'string.length()' characters are parsed; parsing will fail
        // if a proper prefix of 'string' matches the expected format, but the
        // entire 'string.length()' characters do not.  If an optional
        // fractional second having more than six digits is present in
        // 'string', it is rounded to the nearest value in microseconds.  If
        // the optional timezone offset is present in 'string', the resulting
        // 'Datetime' value is converted to the equivalent UTC value; if the
        // timezone offset is absent, UTC is assumed.  If a leap second is
        // detected (i.e., the parsed value of the 'second' attribute is 60;
        // see {Leap Seconds}), the 'second' attribute is taken to be 59, then
        // an additional second is added to 'result' at the end.  The behavior
        // is undefined unless 'string.data()' is non-null.

    static int parse(DateTz *result, const bslstl::StringRef& string);
        // Parse the specified FIX 'string' as a 'DateTz' value, and load the
        // value into the specified 'result'.  Return 0 on success, and a
        // non-zero value (with no effect) otherwise.  'string' is assumed to
        // be of the form:
        //..
        //  YYYYMMDD{(+|-)hh{:mm}|Z}
        //..
        // *Exactly* 'string.length()' characters are parsed; parsing will fail
        // if a proper prefix of 'string' matches the expected format, but the
        // entire 'string.length()' characters do not.  If the optional
        // timezone offset is not present in 'string', UTC is assumed.  The
        // behavior is undefined unless 'string.data()' is non-null.

    static int parse(TimeTz *result, const bslstl::StringRef& string);
        // Parse the specified FIX 'string' as a 'TimeTz' value, and load the
        // value into the specified 'result'.  Return 0 on success, and a
        // non-zero value (with no effect) otherwise.  'string' is assumed to
        // be of the form:
        //..
        //  hh:mm{:ss{.s+}}{(+|-)hh{:mm}|Z}
        //..
        // *Exactly* 'string.length()' characters are parsed; parsing will fail
        // if a proper prefix of 'string' matches the expected format, but the
        // entire 'string.length()' characters do not.  If an optional
        // fractional second having more than three digits is present in
        // 'string', it is rounded to the nearest value in milliseconds.  If
        // the optional timezone offset is not present in 'string', UTC is
        // assumed.  If a leap second is detected (i.e., the parsed value of
        // the 'second' attribute is 60; see {Leap Seconds}), the 'second'
        // attribute is taken to be 59, then an additional second is added to
        // 'result' at the end.  The behavior is undefined unless
        // 'string.data()' is non-null.

    static int parse(DatetimeTz *result, const bslstl::StringRef& string);
        // Parse the specified FIX 'string' as a 'DatetimeTz' value, and load
        // the value into the specified 'result'.  Return 0 on success, and a
        // non-zero value (with no effect) otherwise.  'string' is assumed to
        // be of the form:
        //..
        //  YYYYMMDD-hh:mm{:ss{.s+}}{(+|-)hh{:mm}|Z}
        //..
        // *Exactly* 'string.length()' characters are parsed; parsing will fail
        // if a proper prefix of 'string' matches the expected format, but the
        // entire 'string.length()' characters do not.  If an optional
        // fractional second having more than six digits is present in
        // 'string', it is rounded to the nearest value in microseconds.  If
        // the optional timezone offset is not present in 'string', UTC is
        // assumed.  If a leap second is detected (i.e., the parsed value of
        // the 'second' attribute is 60; see {Leap Seconds}), the 'second'
        // attribute is taken to be 59, then an additional second is added to
        // 'result' at the end.  The behavior is undefined unless
        // 'string.data()' is non-null.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                              // --------------
                              // struct FixUtil
                              // --------------

// CLASS METHODS
inline
int FixUtil::generate(char *buffer, int bufferLength, const Date& object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generate(char *buffer, int bufferLength, const Time& object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
int
FixUtil::generate(char *buffer, int bufferLength, const Datetime& object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generate(char *buffer, int bufferLength, const DateTz& object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generate(char *buffer, int bufferLength, const TimeTz& object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
int
FixUtil::generate(char *buffer, int bufferLength, const DatetimeTz& object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generate(bsl::string *string, const Date& object)
{
    BSLS_ASSERT_SAFE(string);

    return generate(string,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generate(bsl::string *string, const Time& object)
{
    BSLS_ASSERT_SAFE(string);

    return generate(string,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generate(bsl::string *string, const Datetime& object)
{
    BSLS_ASSERT_SAFE(string);

    return generate(string,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generate(bsl::string *string, const DateTz& object)
{
    BSLS_ASSERT_SAFE(string);

    return generate(string,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generate(bsl::string *string, const TimeTz& object)
{
    BSLS_ASSERT_SAFE(string);

    return generate(string,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generate(bsl::string *string, const DatetimeTz& object)
{
    BSLS_ASSERT_SAFE(string);

    return generate(string,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& FixUtil::generate(bsl::ostream& stream, const Date& object)
{
    return generate(stream,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& FixUtil::generate(bsl::ostream&               stream,
                                const Date&                 object,
                                const FixUtilConfiguration& configuration)
{
    char buffer[k_DATE_STRLEN + 1];

    const int len = generate(buffer, k_DATE_STRLEN, object, configuration);
    BSLS_ASSERT_SAFE(k_DATE_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream& FixUtil::generate(bsl::ostream& stream, const Time& object)
{
    return generate(stream,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& FixUtil::generate(bsl::ostream&               stream,
                                const Time&                 object,
                                const FixUtilConfiguration& configuration)
{
    char buffer[k_TIME_STRLEN + 1];

    const int len = generate(buffer, k_TIME_STRLEN, object, configuration);
    BSLS_ASSERT_SAFE(k_TIME_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream&
FixUtil::generate(bsl::ostream& stream, const Datetime& object)
{
    return generate(stream,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& FixUtil::generate(bsl::ostream&               stream,
                                const Datetime&             object,
                                const FixUtilConfiguration& configuration)
{
    char buffer[k_DATETIME_STRLEN + 1];

    const int len = generate(buffer, k_DATETIME_STRLEN, object, configuration);
    BSLS_ASSERT_SAFE(k_DATETIME_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream& FixUtil::generate(bsl::ostream& stream, const DateTz& object)
{
    return generate(stream,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& FixUtil::generate(bsl::ostream&               stream,
                                const DateTz&               object,
                                const FixUtilConfiguration& configuration)
{
    char buffer[k_DATETZ_STRLEN + 1];

    const int len = generate(buffer, k_DATETZ_STRLEN, object, configuration);
    BSLS_ASSERT_SAFE(k_DATETZ_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream& FixUtil::generate(bsl::ostream& stream, const TimeTz& object)
{
    return generate(stream,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& FixUtil::generate(bsl::ostream&               stream,
                                const TimeTz&               object,
                                const FixUtilConfiguration& configuration)
{
    char buffer[k_TIMETZ_STRLEN + 1];

    const int len = generate(buffer, k_TIMETZ_STRLEN, object, configuration);
    BSLS_ASSERT_SAFE(k_TIMETZ_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream&
FixUtil::generate(bsl::ostream& stream, const DatetimeTz& object)
{
    return generate(stream,
                    object,
                    FixUtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& FixUtil::generate(bsl::ostream&               stream,
                                const DatetimeTz&           object,
                                const FixUtilConfiguration& configuration)
{
    char buffer[k_DATETIMETZ_STRLEN + 1];

    const int len = generate(buffer,
                             k_DATETIMETZ_STRLEN,
                             object,
                             configuration);
    BSLS_ASSERT_SAFE(k_DATETIMETZ_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
int FixUtil::generateRaw(char *buffer, const Date& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generateRaw(char *buffer, const Time& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generateRaw(char *buffer, const Datetime& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generateRaw(char *buffer, const DateTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generateRaw(char *buffer, const TimeTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::generateRaw(char *buffer, const DatetimeTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       FixUtilConfiguration::defaultConfiguration());
}

inline
int FixUtil::parse(Date *result, const bslstl::StringRef& string)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), static_cast<int>(string.length()));
}

inline
int FixUtil::parse(Time *result, const bslstl::StringRef& string)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), static_cast<int>(string.length()));
}

inline
int FixUtil::parse(Datetime *result, const bslstl::StringRef& string)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), static_cast<int>(string.length()));
}

inline
int FixUtil::parse(DateTz *result, const bslstl::StringRef& string)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), static_cast<int>(string.length()));
}

inline
int FixUtil::parse(TimeTz *result, const bslstl::StringRef& string)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), static_cast<int>(string.length()));
}

inline
int FixUtil::parse(DatetimeTz *result, const bslstl::StringRef& string)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), static_cast<int>(string.length()));
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
