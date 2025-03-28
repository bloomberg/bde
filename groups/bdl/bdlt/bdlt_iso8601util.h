// bdlt_iso8601util.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLT_ISO8601UTIL
#define INCLUDED_BDLT_ISO8601UTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide conversions between date/time objects and ISO 8601 strings.
//
//@CLASSES:
//  bdlt::Iso8601Util: namespace for ISO 8601 date/time conversion functions
//
//@SEE_ALSO: bdlt_iso8601utilconfiguration
//
//@DESCRIPTION: This component provides a namespace, `bdlt::Iso8601Util`,
// containing functions that convert `bsls` timeinterval and `bdlt` date, time,
// and datetime objects to and from ("generate" and "parse", respectively)
// corresponding string representations that are compliant with the ISO 8601
// standard.  The version of the ISO 8601 standard that is the basis for this
// component can be found at:
// ```
// http://dotat.at/tmp/ISO_8601-2004_E.pdf
// ```
// In general terms, `Iso8601Util` functions support what ISO 8601 refers to as
// *complete* *representations* in *extended* *format*.  We first present a
// brief overview before delving into the details of the ISO 8601
// representations that are supported for each of the relevant vocabulary
// types.
//
// Each function that *generates* ISO 8601 strings (named `generate` and
// `generateRaw`) takes an object and a `char *` buffer, `bsl::string`, or
// `bsl::ostream`, and writes an ISO 8601 representation of the object to the
// buffer, string, or stream.  The "raw" functions are distinguished from their
// non-"raw" counterparts in three respects:
//
// * The length of the `char *` buffer is not supplied to the `generateRaw`
//   functions.
// * The `generateRaw` functions do not output a null terminator.
// * The `generate` functions that provide an `ptrdiff_t bufferLength`
//   parameter truncate the generated output to `bufferLength` characters.
//   (Neither the `generateRaw` functions nor the `generate` functions taking
//   `bsl::string` or `bsl::ostream` do any truncation of their generated
//   output.)
//
// Since the generate functions always succeed, no status value is returned.
// Instead, either the number of characters output to the `char *` buffer or
// string, or a reference to the stream, is returned.  (Note that the
// generating functions also take an optional `bdlt::Iso8601UtilConfiguration`
// object, which is discussed shortly.)
//
// Each function that *parses* ISO 8601 strings (named `parse`) take the
// address of a target object and a `const char *` (paired with a `length`
// argument) or `bsl::string_view`, and loads the object with the result of
// parsing the character string.  Since parsing can fail, the parse functions
// return an `int` status value (0 for success and a non-zero value for
// failure).  Note that, besides elementary syntactical considerations, the
// validity of parsed strings are subject to the semantic constraints imposed
// by the various `isValid*` class methods, (i.e., `Date::isValidYearMonthDay`,
// `Time::isValid`, etc.).
//
///Terminology
///-----------
// As this component concerns ISO 8601, some terms from that specification are
// used liberally in what follows.  Two ISO 8601 terms of particular note are
// *zone* *designator* and *fractional* *second*.
//
// An ISO 8601 *zone* *designator* corresponds to what other `bdlt` components
// commonly refer to as a timezone offset (or simply as an offset; e.g., see
// `bdlt_datetimetz`).  For example, the ISO 8601 string
// `2002-03-17T15:46:00+04:00` has a zone designator of `+04:00`, indicating a
// timezone 4 hours ahead of UTC.
//
// An ISO 8601 *fractional* *second* corresponds to, for example, the
// combined `millisecond` and `microsecond` attributes of a `bdlt::Time`
// object, or the combined `millisecond` and `microsecond` attributes of a
// `bdlt::Datetime` object.  For example, the `Time` value (and ISO 8601
// string) `15:46:09.330000` has a `millisecond` attribute value of 330 and a
// `microsecond` attribute of 0 (i.e., a fractional second of .33).
//
///ISO 8601 String Generation
///--------------------------
// Strings produced by the `generate` and `generateRaw` functions are a
// straightforward transposition of the attributes of the source value into an
// appropriate ISO 8601 format, and are best illustrated by a few examples.
// Note that for `Datetime` and `DatetimeTz`, the fractional second is
// generated with the precision specified in the configuration.  Also note that
// for `Time` and `TimeTz`, the fractional second is generated with the
// precision specified in the configuration up to a maximum precision of 6.
// ```
// +--------------------------------------+---------------------------------+
// |             Object Value             |    Generated ISO 8601 String    |
// |                                      |  (using default configuration)  |
// +======================================+=================================+
// |  bsls::TimeInterval(1000, 3000000)   |  PT16M40.003S                   |
// +--------------------------------------+---------------------------------+
// |  Date(2002, 03, 17)                  |  2002-03-17                     |
// +--------------------------------------+---------------------------------+
// |  Time(15, 46, 09, 330)               |  15:46:09.330                   |
// +--------------------------------------+---------------------------------+
// |  Datetime(Date(2002, 03, 17)         |                                 |
// |           Time(15, 46, 09, 330))     |  2002-03-17T15:46:09.330        |
// +--------------------------------------+---------------------------------+
// |  DateTz(Date(2002, 03, 17), -120)    |  2002-03-17-02:00 [*]           |
// +--------------------------------------+---------------------------------+
// |  TimeTz(Time(15, 46, 09, 330), 270)  |  15:46:09.330+04:30             |
// +--------------------------------------+---------------------------------+
// |  DatetimeTz(Datetime(                |                                 |
// |              Date(2002, 03, 17),     |                                 |
// |              Time(15, 46, 09, 330)), |                                 |
// |             0)                       |  2002-03-17T15:46:09.330+00:00  |
// +--------------------------------------+---------------------------------+
// ```
// [*] Note that the ISO 8601 specification does not have an equivalent to
// `bdlt::DateTz`.
//
///Configuration
///- - - - - - -
// The `generate` and `generateRaw` functions provide an optional configuration
// parameter.  This optional parameter, of type `Iso8601UtilConfiguration`,
// enables configuration of four aspects of ISO 8601 string generation:
//
// * The decimal sign to use in fractional seconds: `.` or `,`.
// * The precision of the fractional seconds.
// * Whether `:` is optional in zone designators.
// * Whether `Z` is output for the zone designator instead of `+00:00` (UTC).
//
// `Iso8601UtilConfiguration` has four attributes that directly correspond to
// these aspects.  In addition, for generate methods that are not supplied with
// a configuration argument, a process-wide configuration takes effect.  See
// `bdlt_iso8601utilconfiguration` for details.
//
///ISO 8601 String Parsing
///-----------------------
// The `parse` functions accept *all* strings that are produced by the generate
// functions.  In addition, the parse functions accept some variation in the
// generated strings, the details of which are discussed next.  Note that the
// parse methods are not configurable like the generate methods (i.e., via an
// optional `Iso8601UtilConfiguration` argument).  Moreover, the process-wide
// configuration has no effect on parsing either.  Instead, the parse methods
// automatically accept `.` or `,` as the decimal sign in fractional seconds,
// and treat `+00:00`, `+0000`, `Z`, and `z` as equivalent zone designators
// (all denoting UTC).
//
// Parsing in `Relaxed` mode accepts "relaxed" ISO 8601 format that is a
// superset of the strict ISO 8601 format, meaning this function will parse
// ISO 8601 values as well as supporting some common variations.  Currently
// this allows a SPACE character to be used as an alternative separator between
// date and time elements (where strict ISO 8601 requires a `T`), but the set
// of extensions may grow in the future.  `Relaxed` parsing is done by
// specifying the relaxed option using the `Iso8601UtilParseConfiguration` type
// and passing it to the `parse` functions.  There are also `parseRelaxed`
// functions which are now deprecated.
//
///Zone Designators
/// - - - - - - - -
// The zone designator is optional, and can be present when parsing for *any*
// type other than bsls::TimeInterval, i.e., even for `Date`, `Time`, and
// `Datetime`.  If a zone designator is parsed for a `Date`, it must be valid,
// so it can affect the status value that is returned in that case, but it is
// otherwise ignored.  For `Time` and `Datetime`, any zone designator present
// in the parsed string will affect the resulting object value (unless the zone
// designator denotes UTC) because the result is converted to UTC.  If the zone
// designator is absent, it is treated as if `+00:00` were specified:
// ```
// +------------------------------------+-----------------------------------+
// |       Parsed ISO 8601 String       |        Result Object Value        |
// +====================================+===================================+
// |  2002-03-17-02:00                  |  Date(2002, 03, 17)               |
// |                                    |  # zone designator ignored        |
// +------------------------------------+-----------------------------------+
// |  2002-03-17-02:65                  |  Date: parsing fails              |
// |                                    |  # invalid zone designator        |
// +------------------------------------+-----------------------------------+
// |  15:46:09.330+04:30                |  Time(11, 16, 09, 330)            |
// |                                    |  # converted to UTC               |
// +------------------------------------+-----------------------------------+
// |  15:46:09.330+04:30                |  TimeTz(Time(15, 46, 09, 330),    |
// |                                    |         270)                      |
// +------------------------------------+-----------------------------------+
// |  15:46:09.330                      |  TimeTz(Time(15, 46, 09, 330),    |
// |                                    |         0)                        |
// |                                    |  # implied '+00:00'               |
// +------------------------------------+-----------------------------------+
// |  2002-03-17T23:46:09.222-05:00     |  Datetime(Date(2002, 03, 18),     |
// |                                    |           Time(04, 46, 09, 222))  |
// |                                    |  # carry into 'day' attribute     |
// |                                    |  # when converted to UTC          |
// +------------------------------------+-----------------------------------+
// ```
// There is also the `basic` format, which may be specified when parsing by
// using the `bdlt::Iso8601UtilParseConfiguration` type, in which case there
// are to be no `-`s in the `Date` and no `:`s in the `Time`.  The `:` in the
// time zone is always optional, whether in basic or default format.
// ```
// +------------------------------------+-----------------------------------+
// |   Parsed Basic ISO 8601 String     |        Result Object Value        |
// +====================================+===================================+
// |  20020317-0200                     |  Date(2002, 03, 17)               |
// |                                    |  # zone designator ignored        |
// +------------------------------------+-----------------------------------+
// |  20020317-0265                     |  Date: parsing fails              |
// |                                    |  # invalid zone designator        |
// +------------------------------------+-----------------------------------+
// |  154609.330+0430                   |  Time(11, 16, 09, 330)            |
// |                                    |  # converted to UTC               |
// +------------------------------------+-----------------------------------+
// |  154609.330+0430                   |  TimeTz(Time(15, 46, 09, 330),    |
// |                                    |         270)                      |
// +------------------------------------+-----------------------------------+
// |  154609.330                        |  TimeTz(Time(15, 46, 09, 330),    |
// |                                    |         0)                        |
// |                                    |  # implied '+00:00'               |
// +------------------------------------+-----------------------------------+
// |  20020317T234609.222-0500          |  Datetime(Date(2002, 03, 18),     |
// |                                    |           Time(04, 46, 09, 222))  |
// |                                    |  # carry into 'day' attribute     |
// |                                    |  # when converted to UTC          |
// +------------------------------------+-----------------------------------+
// ```
// In the last example above, the conversion to UTC incurs a carry into the
// `day` attribute of the `Date` component of the resulting `Datetime` value.
// Note that if such a carry causes an underflow or overflow at the extreme
// ends of the valid range of dates (0001/01/01 and 9999/12/31), then parsing
// for `Datetime` fails.
//
///Fractional Seconds
/// - - - - - - - - -
// The fractional second is optional.  When the fractional second is absent, it
// is treated as if `.0` were specified.  When the fractional second is
// present, it can have one or more digits (i.e., it can contain more than
// six).  For `Datetime`, `DatetimeTz`, `Time`, and `TimeTz`, if more than six
// digits are included in the fractional second, values are rounded to a full
// microsecond; i.e., values greater than or equal to .5 microseconds are
// rounded up.  For `bsls::TimeInterval`, if more than nine digits are included
// in the fractional second, values are rounded to a full nanosecond; i.e.,
// values greater than or equal to .5 microseconds are rounded up.  These
// roundings may incur a carry of one second into the `second` attribute:
// ```
// +--------------------------------------+---------------------------------+
// |        Parsed ISO 8601 String        |      Result Object Value        |
// +======================================+=================================+
// |  15:46:09.1                          |  Time(15, 46, 09, 100)          |
// +--------------------------------------+---------------------------------+
// |  15:46:09-05:00                      |  TimeTz(Time(15, 46, 09, 000),  |
// |                                      |         -300)                   |
// |                                      |  # implied '.0'                 |
// +--------------------------------------+---------------------------------+
// |  15:46:09.99999949                   |  Time(15, 46, 09, 999, 999)     |
// |                                      |  # truncate last two digits     |
// +--------------------------------------+---------------------------------+
// |  15:46:09.9999995                    |  Time(15, 46, 10, 000, 000)     |
// |                                      |  # round up and carry           |
// +--------------------------------------+---------------------------------+
// ```
// Note that, for `Datetime` and `DatetimeTz`, if a carry due to rounding of
// the fractional second would cause an overflow at the extreme upper end of
// the valid range of dates (i.e., 9999/12/31), then parsing would fail.
//
///Leap Seconds
/// - - - - - -
// Leap seconds are not representable by `bdlt::Time` or `bdlt::Datetime`.
// Hence, they are not produced by any of the `Iso8601Util` generate functions.
// However, positive leap seconds *are* supported by the parse functions.  A
// leap second is recognized when the value parsed for the `second` attribute
// of a `Time` is 60 -- regardless of the values parsed for the `hour`,
// `minute`, `millisecond`, and `microsecond` attributes.  Note that this
// behavior is more generous than that afforded by the ISO 8601 specification
// (which indicates that a positive leap second can only be represented as
// "23:59:60Z").
//
// When a leap second is detected during parsing of an ISO 8601 string, the
// `second` attribute is taken to be 59, so that the value of the `Time` object
// can be validly set; then an additional second is added to the object.  Note
// that the possible carry incurred by a leap second (i.e., when loading the
// result of parsing into a `Datetime` or `DatetimeTz` object) has the same
// potential for overflow as may occur with fractional seconds that are rounded
// up (although in admittedly pathological cases).
//
///The Time 24:00
/// - - - - - - -
// According to the ISO 8601 specification, the time 24:00 is interpreted as
// midnight, i.e., the last instant of a day.  However, this concept is not
// supported by `bdlt`.  Although 24:00 is *representable* by `bdlt`, i.e., as
// the default value for `bdlt::Time`, `Time(24, 0)` does *not* represent
// midnight when it is the value for the "time" attribute of a `Datetime` (or
// `DatetimeTz`) object.  For example:
// ```
// bdlt::Datetime notMidnight =
//             bdlt::Datetime(bdlt::Date(2002, 03, 17), bdlt::Time(24, 0, 0));
//
// notMidnight.addSeconds(1);
// assert(notMidnight ==
//             bdlt::Datetime(bdlt::Date(2002, 03, 17), bdlt::Time( 0, 0, 1));
// ```
// It is important to be aware of this peculiarity of `Datetime` (and
// `DatetimeTz`) as it relates to ISO 8601.
//
// The following table shows some examples of parsing an ISO 8601 string
// containing "24:00".  Note that parsing fails if the zone designator is not
// equivalent to "+00:00" when the time 24:00 is encountered:
// ```
// +------------------------------------+-----------------------------------+
// |       Parsed ISO 8601 String       |        Result Object Value        |
// +====================================+===================================+
// |  24:00:00.000000                   |  Time(24, 0, 0, 0)                |
// |                                    |  # preserve default 'Time' value  |
// +------------------------------------+-----------------------------------+
// |  24:00:00.000000-04:00             |  TimeTz: parsing fails            |
// |                                    |  # zone designator not UTC        |
// +------------------------------------+-----------------------------------+
// |  0001-01-01T24:00:00.000000        |  Datetime(Date(0001, 01, 01),     |
// |                                    |           Time(24, 0, 0, 0))      |
// |                                    |  # preserve 'Datetime' default    |
// |                                    |  # value                          |
// +------------------------------------+-----------------------------------+
// |  2002-03-17T24:00:00.000000        |  Datetime(Date(2002, 03, 17),     |
// |                                    |           Time(24, 0, 0, 0))      |
// |                                    |  # preserve default 'Time' value  |
// +------------------------------------+-----------------------------------+
// ```
// An `hour` attribute value of 24 is also "preserved" by the generate
// functions provided by this component:
// ```
// +------------------------------------+-----------------------------------+
// |        Source Object Value         |     Generated ISO 8601 String     |
// +====================================+===================================+
// |  Time(24, 0, 0, 0)                 |  24:00:00.000                     |
// +------------------------------------+-----------------------------------+
// |  Datetime(Date(2002, 03, 17),      |  2002-03-17T24:00:00.000          |
// |           Time(24, 0, 0, 0))       |                                   |
// +------------------------------------+-----------------------------------+
// ```
//
///Summary of Supported ISO 8601 Representations
///- - - - - - - - - - - - - - - - - - - - - - -
// The syntax description below summarizes the ISO 8601 string representations
// supported by this component.  Although not quoted (for readability),
// `[+-:.,TtZz]` are literal characters that can occur in ISO 8601 strings.
// Furthermore, for clarity, the (rarely used) lowercase `t` and `z` characters
// are omitted from the specifications below, as well as from the
// function-level documentation.  The characters `[YMDhms]` each denote a
// decimal digit, `{}` brackets optional elements, `()` is used for grouping,
// and `|` separates alternatives:
// ```
// <Generated Date>          ::=  <DATE>
//
// <Parsed Date>             ::=  <Parsed DateTz>
//
// <Generated DateTz>        ::=  <DATE><ZONE>
//
// <Parsed DateTz>           ::=  <DATE>{<ZONE>}
//
// <Generated Time>          ::=  <TIME FLEXIBLE>
//
// <Parsed Time>             ::=  <Parsed TimeTz>
//
// <Generated TimeTz>        ::=  <TIME FLEXIBLE><ZONE>
//
// <Parsed TimeTz>           ::=  <TIME FLEXIBLE>{<ZONE>}
//
// <Generated Datetime>      ::=  <DATE>T<TIME FLEXIBLE>
//
// <Parsed Datetime>         ::=  <Parsed DatetimeTz>
//
// <Generated DatetimeTz>    ::=  <DATE>T<TIME FLEXIBLE><ZONE>
//
// <Parsed DatetimeTz>       ::=  <DATE>(T|t)<TIME FLEXIBLE>{<ZONE>}
// (Default)
//
// <Parsed DatetimeTz>       ::=  <DATE>(T|t| )<TIME FLEXIBLE>{<ZONE>}
// (Relaxed)
//
// <DATE> (Default)          ::=  YYYY-MM-DD
//
// <DATE> (Basic)            ::=  YYYYMMDD
//
// <TIME FLEXIBLE> (Default) ::=  hh:mm:ss{(.|,)s+}  # one or more digits in
//                                                   # the fractional second
//
// <TIME FLEXIBLE> (Basic)   ::=  hhmmss{(.|,)s+}  # one or more digits in the
//                                                 # fractional second
//
// <ZONE>                    ::=  (+|-)hh{:}mm|Z   # zone designator (':' is
//                                                 # optional whether default
//                                                 # or basic)
// ```
//
///Summary of Supported ISO 8601 Duration Representations
///- - - - - - - - - - - - - - - - - - - - - - -
// The syntax description below summarizes the ISO 8601 string representations
// for durations supported by this component.  Although not quoted (for
// readability), `[.,PWDTHMS]` are literal characters that can occur in ISO
// 8601 strings.  The characters `[wdhms]` each denote a decimal digit, `{}`
// brackets optional elements, `()` is used for grouping, and `|` separates
// alternatives:
// ```
// <Date Duration>      ::=  {w+W}{d+D}
//
// <Time Duration>      ::=  T{h+H}{m+M}{s+{(.|,)s+}S}         # must contain
//                                                             # at least one
//                                                             # optional field
//                                                             # (i.e. "T" is
//                                                             # not valid)
//
// <Generated Duration> ::=  P<Date Duration><Time Duration>   # all values
//                                                             # guaranteed to
//                                                             # be less than
//                                                             # their modulus
//                                                             # (weeks can be
//                                                             # be up to 14
//                                                             # digits), and
//                                                             # it must
//                                                             # contain the
//                                                             # seconds
//                                                             # portion
//
// <Parsed Duration>    ::=  P<Date Duration>{<Time Duration>} # must contain
//                                                             # at least one
//                                                             # optional field
//                                                             # in <Date
//                                                             # Duration> or
//                                                             # must contain
//                                                             # a <Time
//                                                             # Duration>
//                                                             # (i.e. "P" is
//                                                             # not valid)
// ```
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic `bdlt::Iso8601Util` Usage
/// - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates basic use of one `generate` function and two
// `parse` functions.
//
// First, we construct a few objects that are prerequisites for this and the
// following example:
// ```
// const bdlt::Date date(2005, 1, 31);     // 2005/01/31
// const bdlt::Time time(8, 59, 59, 123);  // 08:59:59.123
// const int        tzOffset = 240;        // +04:00 (four hours west of UTC)
// ```
// Then, we construct a `bdlt::DatetimeTz` object for which a corresponding ISO
// 8601-compliant string will be generated shortly:
// ```
// const bdlt::DatetimeTz sourceDatetimeTz(bdlt::Datetime(date, time),
//                                         tzOffset);
// ```
// For comparison with the ISO 8601 string generated below, note that streaming
// the value of `sourceDatetimeTz` to `stdout`:
// ```
// bsl::cout << sourceDatetimeTz << bsl::endl;
// ```
// produces:
// ```
// 31JAN2005_08:59:59.123000+0400
// ```
// Next, we use a `generate` function to produce an ISO 8601-compliant string
// for `sourceDatetimeTz`, writing the output to a `bsl::ostringstream`, and
// assert that both the return value and the string that is produced are as
// expected:
// ```
// bsl::ostringstream  oss;
// const bsl::ostream& ret =
//                        bdlt::Iso8601Util::generate(oss, sourceDatetimeTz);
// assert(&oss == &ret);
//
// const bsl::string iso8601 = oss.str();
// assert(iso8601 == "2005-01-31T08:59:59.123+04:00");
// ```
// For comparison, see the output that was produced by the streaming operator
// above.
//
// Now, we parse the string that was just produced, loading the result of the
// parse into a second `bdlt::DatetimeTz` object, and assert that the parse was
// successful and that the target object has the same value as that of the
// original (i.e., `sourceDatetimeTz`):
// ```
// bdlt::DatetimeTz targetDatetimeTz;
//
// int rc = bdlt::Iso8601Util::parse(&targetDatetimeTz,
//                                   iso8601.c_str(),
//                                   static_cast<int>(iso8601.length()));
// assert(               0 == rc);
// assert(sourceDatetimeTz == targetDatetimeTz);
// ```
// Finally, we parse the `iso8601` string a second time, this time loading the
// result into a `bdlt::Datetime` object (instead of a `bdlt::DatetimeTz`):
// ```
// bdlt::Datetime targetDatetime;
//
// rc = bdlt::Iso8601Util::parse(&targetDatetime,
//                               iso8601.c_str(),
//                               static_cast<int>(iso8601.length()));
// assert(                             0 == rc);
// assert(sourceDatetimeTz.utcDatetime() == targetDatetime);
// ```
// Note that this time the value of the target object has been converted to
// UTC.
//
///Example 2: Configuring ISO 8601 String Generation
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates use of a `bdlt::Iso8601UtilConfiguration` object
// to influence the format of the ISO 8601 strings that are generated by this
// component by passing that configuration object to `generate`.  We also take
// this opportunity to illustrate the flavor of the `generate` functions that
// outputs to a `char *` buffer of a specified length.
//
// First, we construct a `bdlt::TimeTz` object for which a corresponding ISO
// 8601-compliant string will be generated shortly:
// ```
// const bdlt::TimeTz sourceTimeTz(time, tzOffset);
// ```
// For comparison with the ISO 8601 string generated below, note that streaming
// the value of `sourceTimeTz` to `stdout`:
// ```
// bsl::cout << sourceTimeTz << bsl::endl;
// ```
// produces:
// ```
// 08:59:59.123+0400
// ```
// Then, we construct the `bdlt::Iso8601UtilConfiguration` object that
// indicates how we would like to affect the generated output ISO 8601 string.
// In this case, we want to use `,` as the decimal sign (in fractional seconds)
// and omit the `:` in zone designators:
// ```
// bdlt::Iso8601UtilConfiguration configuration;
// configuration.setOmitColonInZoneDesignator(true);
// configuration.setUseCommaForDecimalSign(true);
// ```
// Next, we define the `char *` buffer that will be used to stored the
// generated string.  A buffer of size `bdlt::Iso8601Util::k_TIMETZ_STRLEN + 1`
// is large enough to hold any string generated by this component for a
// `bdlt::TimeTz` object, including a null terminator:
// ```
// const int BUFLEN = bdlt::Iso8601Util::k_TIMETZ_STRLEN + 1;
// char      buffer[BUFLEN];
// ```
// Then, we use a `generate` function that accepts our `configuration` to
// produce an ISO 8601-compliant string for `sourceTimeTz`, this time writing
// the output to a `char *` buffer, and assert that both the return value and
// the string that is produced are as expected.  Note that in comparing the
// return value against `BUFLEN - 5` we account for the omission of the `:`
// from the zone designator, and also for the fact that, although a null
// terminator was generated, it is not included in the character count returned
// by `generate`.  Also note that we use `bsl::strcmp` to compare the resulting
// string knowing that we supplied a buffer having sufficient capacity to
// accommodate a null terminator:
// ```
// rc = bdlt::Iso8601Util::generate(buffer,
//                                  BUFLEN,
//                                  sourceTimeTz,
//                                  configuration);
// assert(BUFLEN - 5 == rc);
// assert(         0 == bsl::strcmp(buffer, "08:59:59,123+0400"));
// ```
// For comparison, see the output that was produced by the streaming operator
// above.
//
// Next, we parse the string that was just produced, loading the result of the
// parse into a second `bdlt::TimeTz` object, and assert that the parse was
// successful and that the target object has the same value as that of the
// original (i.e., `sourceTimeTz`).  Note that `BUFLEN - 5` is passed and *not*
// `BUFLEN` because the former indicates the correct number of characters in
// `buffer` that we wish to parse:
// ```
// bdlt::TimeTz targetTimeTz;
//
// rc = bdlt::Iso8601Util::parse(&targetTimeTz, buffer, BUFLEN - 5);
//
// assert(           0 == rc);
// assert(sourceTimeTz == targetTimeTz);
// ```
// Then, we parse the string in `buffer` a second time, this time loading the
// result into a `bdlt::Time` object (instead of a `bdlt::TimeTz`):
// ```
// bdlt::Time targetTime;
//
// rc = bdlt::Iso8601Util::parse(&targetTime, buffer, BUFLEN - 5);
// assert(                     0 == rc);
// assert(sourceTimeTz.utcTime() == targetTime);
// ```
// Note that this time the value of the target object has been converted to
// UTC.
//
// Finally, we modify the `configuration` to display the `bdlt::TimeTz` without
// fractional seconds:
// ```
// configuration.setFractionalSecondPrecision(0);
// rc = bdlt::Iso8601Util::generate(buffer,
//                                  BUFLEN,
//                                  sourceTimeTz,
//                                  configuration);
// assert(BUFLEN - 9 == rc);
// assert(         0 == bsl::strcmp(buffer, "08:59:59+0400"));
// ```

#include <bdlscm_version.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetz.h>
#include <bdlt_iso8601utilconfiguration.h>
#include <bdlt_iso8601utilparseconfiguration.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bdlb_variant.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_deprecatefeature.h>
#include <bsls_libraryfeatures.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>
#include <bsl_ostream.h>
#include <bsl_string.h>

#include <string>

namespace BloombergLP {
namespace bsls {

class TimeInterval;

}  // close namespace bsls

#define BDLT_ISO8601UTIL_DEPRECATE_GENERATE                                   \
    BSLS_DEPRECATE_FEATURE("bdl",                                             \
                           "generate",                                        \
                           "use overload with GenerateConfiguration")

#define BDLT_ISO8601UTIL_DEPRECATE_GENERATE_ORDER                             \
    BSLS_DEPRECATE_FEATURE("bdl",                                             \
                           "generate",                                        \
                           "use overload with length before object")

#define BDLT_ISO8601UTIL_DEPRECATE_GENERATERAW                                \
    BSLS_DEPRECATE_FEATURE("bdl",                                             \
                           "generateRaw",                                     \
                           "use overload with GenerateConfiguration")

#define BDLT_ISO8601UTIL_DEPRECATE_PARSERELAXED                               \
    BSLS_DEPRECATE_FEATURE("bdl",                                             \
                           "parseRelaxed",                                    \
                           "use parse with ParseConfiguration")

namespace bdlt {


                            // ==================
                            // struct Iso8601Util
                            // ==================

/// This `struct` provides a namespace for a suite of pure functions that
/// perform conversions between objects of `bdlt` vocabulary type and their
/// ISO 8601 representations.  Each `generate` and `generateRaw` method
/// takes a `bdlt` object (of type `Date`, `DateTz`, `Time`, `TimeTz`,
/// `Datetime`, or `DatetimeTz`) and outputs its corresponding ISO 8601
/// representation to a user-supplied character buffer or `bsl::ostream`.
/// The `parse` methods effect the opposite conversion in that they populate
/// a `bdlt` object from the result of parsing an ISO 8601 representation.
struct Iso8601Util {

  private:
    // PRIVATE CLASS METHODS

    /// Return a default configured configuration object.
    static Iso8601UtilConfiguration defaultConfiguration();

  public:
    // TYPES

    // This enumeration defines fixed lengths for the ISO 8601
    // representations of date, time, and datetime values.  Note that these
    // constants do *not* account for the null terminator that may be
    // produced by the `generate` functions taking a `bufferLength`
    // argument.
    enum {

        k_DATE_STRLEN         = 10,  // `bdlt::Date`
        k_DATETZ_STRLEN       = 16,  // `bdlt::DateTz`

        k_TIME_STRLEN         = 15,  // `bdlt::Time`
        k_TIMETZ_STRLEN       = 21,  // `bdlt::TimeTz`

        k_DATETIME_STRLEN     = 26,  // `bdlt::Datetime`
        k_DATETIMETZ_STRLEN   = 32,  // `bdlt::DatetimeTz`

        k_TIMEINTERVAL_STRLEN = 38,  // `bsls::TimeInterval`

        k_MAX_STRLEN          = k_TIMEINTERVAL_STRLEN

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BDEPU_DATE_STRLEN         = k_DATE_STRLEN
      , BDEPU_DATETIME_STRLEN     = k_DATETIME_STRLEN
      , BDEPU_DATETIMETZ_STRLEN   = k_DATETIMETZ_STRLEN
      , BDEPU_DATETZ_STRLEN       = k_DATETZ_STRLEN
      , BDEPU_TIME_STRLEN         = k_TIME_STRLEN
      , BDEPU_TIMETZ_STRLEN       = k_TIMETZ_STRLEN
      , BDEPU_MAX_DATETIME_STRLEN = k_MAX_STRLEN

      , DATE_STRLEN               = k_DATE_STRLEN
      , DATETIME_STRLEN           = k_DATETIME_STRLEN
      , DATETIMETZ_STRLEN         = k_DATETIMETZ_STRLEN
      , DATETZ_STRLEN             = k_DATETZ_STRLEN
      , TIME_STRLEN               = k_TIME_STRLEN
      , TIMETZ_STRLEN             = k_TIMETZ_STRLEN
      , MAX_DATETIME_STRLEN       = k_MAX_STRLEN
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    typedef bsl::ptrdiff_t                ssize_t;

    typedef Iso8601UtilConfiguration      Configuration;

    /// Configuration for string generation.
    typedef Iso8601UtilConfiguration      GenerateConfiguration;

    /// Configuration for string parsing.
    typedef Iso8601UtilParseConfiguration ParseConfiguration;

    /// `DateOrDateTz` is a convenient alias for
    /// `bdlb::Variant2<Date, DateTz>`.
    typedef bdlb::Variant2<Date, DateTz>         DateOrDateTz;

    /// `TimeOrTimeTz` is a convenient alias for
    /// `bdlb::Variant2<Time, TimeTz>`.
    typedef bdlb::Variant2<Time, TimeTz>         TimeOrTimeTz;

    /// `DatetimeOrDatetimeTz` is a convenient alias for
    /// `bdlb::Variant2<Datetime, DatetimeTz>`.
    typedef bdlb::Variant2<Datetime, DatetimeTz> DatetimeOrDatetimeTz;

    // CLASS METHODS

    /// Write the ISO 8601 representation of the specified `object` to the
    /// specified `buffer` of the specified `bufferLength` (in bytes),
    /// truncating (if necessary) to `bufferLength`.  Optionally specify a
    /// `configuration` to affect the format of the generated string.  If
    /// `configuration` is not supplied, the process-wide default value
    /// `Iso8601UtilConfiguration::defaultConfiguration()` is used.  Return
    /// the number of characters in the formatted string before truncation
    /// (not counting a null terminator).  If `bufferLength` indicates
    /// sufficient capacity, `buffer` is null terminated.  The behavior is
    /// undefined unless `0 <= bufferLength`.  Note that a buffer of size
    /// `k_MAX_STRLEN + 1` is large enough to hold any string generated by
    /// this component (counting a null terminator, if any).
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const bsls::TimeInterval&        object);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const bsls::TimeInterval&        object,
                        const GenerateConfiguration&     configuration);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const Date&                      object);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const Date&                      object,
                        const GenerateConfiguration&     configuration);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const Time&                      object);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const Time&                      object,
                        const GenerateConfiguration&     configuration);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const Datetime&                  object);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const Datetime&                  object,
                        const GenerateConfiguration&     configuration);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const DateTz&                    object);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const DateTz&                    object,
                        const GenerateConfiguration&     configuration);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const TimeTz&                    object);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const TimeTz&                    object,
                        const GenerateConfiguration&     configuration);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const DatetimeTz&                object);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const DatetimeTz&                object,
                        const GenerateConfiguration&     configuration);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const DateOrDateTz&              object);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const DateOrDateTz&              object,
                        const GenerateConfiguration&     configuration);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const TimeOrTimeTz&              object);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const TimeOrTimeTz&              object,
                        const GenerateConfiguration&     configuration);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const DatetimeOrDatetimeTz&      object);
    static int generate(char                            *buffer,
                        ssize_t                          bufferLength,
                        const DatetimeOrDatetimeTz&      object,
                        const GenerateConfiguration&     configuration);

    static int generate(bsl::string                     *string,
                        const bsls::TimeInterval&        object);
    static int generate(bsl::string                     *string,
                        const bsls::TimeInterval&        object,
                        const GenerateConfiguration&     configuration);
    static int generate(bsl::string                     *string,
                        const Date&                      object);
    static int generate(bsl::string                     *string,
                        const Date&                      object,
                        const GenerateConfiguration&     configuration);
    static int generate(bsl::string                     *string,
                        const Time&                      object);
    static int generate(bsl::string                     *string,
                        const Time&                      object,
                        const GenerateConfiguration&     configuration);
    static int generate(bsl::string                     *string,
                        const Datetime&                  object);
    static int generate(bsl::string                     *string,
                        const Datetime&                  object,
                        const GenerateConfiguration&     configuration);
    static int generate(bsl::string                     *string,
                        const DateTz&                    object);
    static int generate(bsl::string                     *string,
                        const DateTz&                    object,
                        const GenerateConfiguration&     configuration);
    static int generate(bsl::string                     *string,
                        const TimeTz&                    object);
    static int generate(bsl::string                     *string,
                        const TimeTz&                    object,
                        const GenerateConfiguration&     configuration);
    static int generate(bsl::string                     *string,
                        const DatetimeTz&                object);
    static int generate(bsl::string                     *string,
                        const DatetimeTz&                object,
                        const GenerateConfiguration&     configuration);
    static int generate(bsl::string                     *string,
                        const DateOrDateTz&              object);
    static int generate(bsl::string                     *string,
                        const DateOrDateTz&              object,
                        const GenerateConfiguration&     configuration);
    static int generate(bsl::string                     *string,
                        const TimeOrTimeTz&              object);
    static int generate(bsl::string                     *string,
                        const TimeOrTimeTz&              object,
                        const GenerateConfiguration&     configuration);
    static int generate(bsl::string                     *string,
                        const DatetimeOrDatetimeTz&      object);
    static int generate(bsl::string                     *string,
                        const DatetimeOrDatetimeTz&      object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::string                     *string,
                        const bsls::TimeInterval&        object);
    static int generate(std::string                     *string,
                        const bsls::TimeInterval&        object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::string                     *string,
                        const Date&                      object);
    static int generate(std::string                     *string,
                        const Date&                      object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::string                     *string,
                        const Time&                      object);
    static int generate(std::string                     *string,
                        const Time&                      object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::string                     *string,
                        const Datetime&                  object);
    static int generate(std::string                     *string,
                        const Datetime&                  object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::string                     *string,
                        const DateTz&                    object);
    static int generate(std::string                     *string,
                        const DateTz&                    object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::string                     *string,
                        const TimeTz&                    object);
    static int generate(std::string                     *string,
                        const TimeTz&                    object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::string                     *string,
                        const DatetimeTz&                object);
    static int generate(std::string                     *string,
                        const DatetimeTz&                object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::string                     *string,
                        const DateOrDateTz&              object);
    static int generate(std::string                     *string,
                        const DateOrDateTz&              object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::string                     *string,
                        const TimeOrTimeTz&              object);
    static int generate(std::string                     *string,
                        const TimeOrTimeTz&              object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::string                     *string,
                        const DatetimeOrDatetimeTz&      object);
    static int generate(std::string                     *string,
                        const DatetimeOrDatetimeTz&      object,
                        const GenerateConfiguration&     configuration);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
    /// Load the ISO 8601 representation of the specified `object` into the
    /// specified `string` of type `bsl::string`, `std::string`, or
    /// `std::pmr::string`.  Optionally specify a `configuration` to affect
    /// the format of the generated string.  If `configuration` is not
    /// supplied, the process-wide default value
    /// `Iso8601UtilConfiguration::defaultConfiguration()` is used.  Return
    /// the number of characters in the formatted string.  The previous
    /// contents of `string` (if any) are discarded.
    static int generate(std::pmr::string                *string,
                        const bsls::TimeInterval&        object);
    static int generate(std::pmr::string                *string,
                        const bsls::TimeInterval&        object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::pmr::string                *string,
                        const Date&                      object);
    static int generate(std::pmr::string                *string,
                        const Date&                      object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::pmr::string                *string,
                        const Time&                      object);
    static int generate(std::pmr::string                *string,
                        const Time&                      object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::pmr::string                *string,
                        const Datetime&                  object);
    static int generate(std::pmr::string                *string,
                        const Datetime&                  object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::pmr::string                *string,
                        const DateTz&                    object);
    static int generate(std::pmr::string                *string,
                        const DateTz&                    object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::pmr::string                *string,
                        const TimeTz&                    object);
    static int generate(std::pmr::string                *string,
                        const TimeTz&                    object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::pmr::string                *string,
                        const DatetimeTz&                object);
    static int generate(std::pmr::string                *string,
                        const DatetimeTz&                object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::pmr::string                *string,
                        const DateOrDateTz&              object);
    static int generate(std::pmr::string                *string,
                        const DateOrDateTz&              object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::pmr::string                *string,
                        const TimeOrTimeTz&              object);
    static int generate(std::pmr::string                *string,
                        const TimeOrTimeTz&              object,
                        const GenerateConfiguration&     configuration);
    static int generate(std::pmr::string                *string,
                        const DatetimeOrDatetimeTz&      object);
    static int generate(std::pmr::string                *string,
                        const DatetimeOrDatetimeTz&      object,
                        const GenerateConfiguration&     configuration);
#endif

    /// Write the ISO 8601 representation of the specified `object` to the
    /// specified `stream`.  Optionally specify a `configuration` to affect
    /// the format of the generated string.  If `configuration` is not
    /// supplied, the process-wide default value
    /// `Iso8601UtilConfiguration::defaultConfiguration()` is used.  Return
    /// a reference to `stream`.  Note that `stream` is not null terminated.
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const bsls::TimeInterval&       object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const bsls::TimeInterval&       object,
                                const GenerateConfiguration&    configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Date&                     object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Date&                     object,
                                const GenerateConfiguration&    configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Time&                     object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Time&                     object,
                                const GenerateConfiguration&    configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Datetime&                 object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Datetime&                 object,
                                const GenerateConfiguration&    configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DateTz&                   object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DateTz&                   object,
                                const GenerateConfiguration&    configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const TimeTz&                   object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const TimeTz&                   object,
                                const GenerateConfiguration&    configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DatetimeTz&               object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DatetimeTz&               object,
                                const GenerateConfiguration&    configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DateOrDateTz&             object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DateOrDateTz&             object,
                                const GenerateConfiguration&    configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const TimeOrTimeTz&             object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const TimeOrTimeTz&             object,
                                const GenerateConfiguration&    configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DatetimeOrDatetimeTz&     object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DatetimeOrDatetimeTz&     object,
                                const GenerateConfiguration&    configuration);

    /// Write the ISO 8601 representation of the specified `object` to the
    /// specified `buffer`.  Optionally specify a `configuration` to affect
    /// the format of the generated string.  If `configuration` is not
    /// supplied, the process-wide default value
    /// `Iso8601UtilConfiguration::defaultConfiguration()` is used.  Return
    /// the number of characters in the formatted string.  `buffer` is not
    /// null terminated.  The behavior is undefined unless `buffer` has
    /// sufficient capacity.  Note that a buffer of size `k_MAX_STRLEN + 1`
    /// is large enough to hold any string generated by this component
    /// (counting a null terminator, if any).
    static int generateRaw(char                            *buffer,
                           const bsls::TimeInterval&        object);
    static int generateRaw(char                            *buffer,
                           const bsls::TimeInterval&        object,
                           const GenerateConfiguration&     configuration);
    static int generateRaw(char                            *buffer,
                           const Date&                      object);
    static int generateRaw(char                            *buffer,
                           const Date&                      object,
                           const GenerateConfiguration&     configuration);
    static int generateRaw(char                            *buffer,
                           const Time&                      object);
    static int generateRaw(char                            *buffer,
                           const Time&                      object,
                           const GenerateConfiguration&     configuration);
    static int generateRaw(char                            *buffer,
                           const Datetime&                  object);
    static int generateRaw(char                            *buffer,
                           const Datetime&                  object,
                           const GenerateConfiguration&     configuration);
    static int generateRaw(char                            *buffer,
                           const DateTz&                    object);
    static int generateRaw(char                            *buffer,
                           const DateTz&                    object,
                           const GenerateConfiguration&     configuration);
    static int generateRaw(char                            *buffer,
                           const TimeTz&                    object);
    static int generateRaw(char                            *buffer,
                           const TimeTz&                    object,
                           const GenerateConfiguration&     configuration);
    static int generateRaw(char                            *buffer,
                           const DatetimeTz&                object);
    static int generateRaw(char                            *buffer,
                           const DatetimeTz&                object,
                           const GenerateConfiguration&     configuration);
    static int generateRaw(char                            *buffer,
                           const DateOrDateTz&              object);
    static int generateRaw(char                            *buffer,
                           const DateOrDateTz&              object,
                           const GenerateConfiguration&     configuration);
    static int generateRaw(char                            *buffer,
                           const TimeOrTimeTz&              object);
    static int generateRaw(char                            *buffer,
                           const TimeOrTimeTz&              object,
                           const GenerateConfiguration&     configuration);
    static int generateRaw(char                            *buffer,
                           const DatetimeOrDatetimeTz&      object);
    static int generateRaw(char                            *buffer,
                           const DatetimeOrDatetimeTz&      object,
                           const GenerateConfiguration&     configuration);

    /// Parse the specified initial `length` characters of the specified ISO
    /// 8601 `string` as a `bsls::TimeInterval` value, and load the value
    /// into the specified `result`.  Return 0 on success, and a non-zero
    /// value (with no effect) otherwise.  `string` is assumed to be of the
    /// form:
    /// ```
    /// <Parsed Duration>
    /// ```
    /// See "Summary of Supported ISO 8601 Duration Representations" for a
    /// complete description of this format.
    ///
    /// *Exactly* `length` characters are parsed; parsing will fail if a
    /// proper prefix of `string` matches the expected format, but the
    /// entire `length` characters do not.  If an optional fractional second
    /// having more than nine digits is present in `string`, it is rounded
    /// to the nearest value in nanoseconds.  The behavior is undefined
    /// unless `0 <= length`.
    static int parse(bsls::TimeInterval *result,
                     const char         *string,
                     ssize_t             length);

    /// Parse the specified initial `length` characters of the specified ISO
    /// 8601 `string` as a `Date` value, and load the value into the
    /// specified `result`, using the optionally specified `configuration`.
    /// Return 0 on success, and a non-zero value (with no effect)
    /// otherwise.  `string` is assumed to be of the form:
    /// ```
    /// Default: YYYY-MM-DD{(+|-)hh{:}mm|Z}
    /// Basic:   YYYYMMDD{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `length` characters are parsed; parsing will fail if a
    /// proper prefix of `string` matches the expected format, but the
    /// entire `length` characters do not.  If the optional zone designator
    /// is present in `string`, it is parsed but ignored.  The behavior is
    /// undefined unless `0 <= length`.
    static int parse(Date               *result,
                     const char         *string,
                     ssize_t             length,
                     ParseConfiguration  configuration = ParseConfiguration());

    /// Parse the specified initial `length` characters of the specified ISO
    /// 8601 `string` as a `Time` value, and load the value into the
    /// specified `result`, using the optionally specified `configuration`.
    /// Return 0 on success, and a non-zero value (with no effect)
    /// otherwise.  `string` is assumed to be of the form:
    /// ```
    /// Default: hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// Basic:   hhmmss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `length` characters are parsed; parsing will fail if a
    /// proper prefix of `string` matches the expected format, but the
    /// entire `length` characters do not.  If an optional fractional second
    /// having more than six digits is present in `string`, it is rounded to
    /// the nearest value in microseconds.  If the optional zone designator
    /// is present in `string`, the resulting `Time` value is converted to
    /// the equivalent UTC time; if the zone designator is absent, UTC is
    /// assumed.  If a leap second is detected (i.e., the parsed value of
    /// the `second` attribute is 60; see {Leap Seconds}), the `second`
    /// attribute is taken to be 59, then an additional second is added to
    /// `result` at the end.  If the "hh:mm:ss" portion of `string` is
    /// "24:00:00", then the fractional second must be absent or 0, and the
    /// zone designator must be absent or indicate UTC.  The behavior is
    /// undefined unless `0 <= length`.
    static int parse(Time               *result,
                     const char         *string,
                     ssize_t             length,
                     ParseConfiguration  configuration = ParseConfiguration());

    /// Parse the specified initial `length` characters of the specified ISO
    /// 8601 `string` as a `Datetime` value, and load the value into the
    /// specified `result`, using the optionally specified `configuration`.
    /// Return 0 on success, and a non-zero value (with no effect)
    /// otherwise.  `string` is assumed to be of the form:
    /// ```
    /// Default: YYYY-MM-DDThh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// Basic:   YYYYMMDDThhmmss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `length` characters are parsed; parsing will fail if a
    /// proper prefix of `string` matches the expected format, but the
    /// entire `length` characters do not.  If an optional fractional second
    /// having more than six digits is present in `string`, it is rounded to
    /// the nearest value in microseconds.  If the optional zone designator
    /// is present in `string`, the resulting `Datetime` value is converted
    /// to the equivalent UTC value; if the zone designator is absent, UTC
    /// is assumed.  If a leap second is detected (i.e., the parsed value of
    /// the `second` attribute is 60; see {Leap Seconds}), the `second`
    /// attribute is taken to be 59, then an additional second is added to
    /// `result` at the end.  If the "hh:mm:ss" portion of `string` is
    /// "24:00:00", then the fractional second must be absent or 0, and the
    /// zone designator must be absent or indicate UTC.  The behavior is
    /// undefined unless `0 <= length`.
    static int parse(Datetime           *result,
                     const char         *string,
                     ssize_t             length,
                     ParseConfiguration  configuration = ParseConfiguration());

    /// Parse the specified initial `length` characters of the specified ISO
    /// 8601 `string` as a `DateTz` value, and load the value into the
    /// specified `result`, using the optionally specified `configuration`.
    /// Return 0 on success, and a non-zero value (with no effect)
    /// otherwise.  `string` is assumed to be of the form:
    /// ```
    /// Default: YYYY-MM-DD{(+|-)hh{:}mm|Z}
    /// Basic:   YYYYMMDD{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `length` characters are parsed; parsing will fail if a
    /// proper prefix of `string` matches the expected format, but the
    /// entire `length` characters do not.  If the optional zone designator
    /// is not present in `string`, UTC is assumed.  The behavior is
    /// undefined unless `0 <= length`.
    static int parse(DateTz             *result,
                     const char         *string,
                     ssize_t             length,
                     ParseConfiguration  configuration = ParseConfiguration());

    /// Parse the specified initial `length` characters of the specified ISO
    /// 8601 `string` as a `TimeTz` value, and load the value into the
    /// specified `result`, using the optionally specified `configuration`.
    /// Return 0 on success, and a non-zero value (with no effect)
    /// otherwise.  `string` is assumed to be of the form:
    /// ```
    /// Default: hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// Basic:   hhmmss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `length` characters are parsed; parsing will fail if a
    /// proper prefix of `string` matches the expected format, but the
    /// entire `length` characters do not.  If an optional fractional second
    /// having more than six digits is present in `string`, it is rounded to
    /// the nearest value in microseconds.  If the optional zone designator
    /// is not present in `string`, UTC is assumed.  If a leap second is
    /// detected (i.e., the parsed value of the `second` attribute is 60;
    /// see {Leap Seconds}), the `second` attribute is taken to be 59, then
    /// an additional second is added to `result` at the end.  If the
    /// "hh:mm:ss" portion of `string` is "24:00:00", then the fractional
    /// second must be absent or 0, and the zone designator must be absent
    /// or indicate UTC.  The behavior is undefined unless `0 <= length`.
    static int parse(TimeTz             *result,
                     const char         *string,
                     ssize_t             length,
                     ParseConfiguration  configuration = ParseConfiguration());

    /// Parse the specified initial `length` characters of the specified ISO
    /// 8601 `string` as a `DatetimeTz` value, and load the value into the
    /// specified `result`, using the optionally specified `configuration`.
    /// Return 0 on success, and a non-zero value (with no effect)
    /// otherwise.  `string` is assumed to be of the form:
    /// ```
    /// Default: YYYY-MM-DDThh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// Basic:   YYYYMMDDThhmmss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `length` characters are parsed; parsing will fail if a
    /// proper prefix of `string` matches the expected format, but the
    /// entire `length` characters do not.  If an optional fractional second
    /// having more than six digits is present in `string`, it is rounded to
    /// the nearest value in microseconds.  If the optional zone designator
    /// is not present in `string`, UTC is assumed.  If a leap second is
    /// detected (i.e., the parsed value of the `second` attribute is 60;
    /// see {Leap Seconds}), the `second` attribute is taken to be 59, then
    /// an additional second is added to `result` at the end.  If the
    /// "hh:mm:ss" portion of `string` is "24:00:00", then the fractional
    /// second must be absent or 0, and the zone designator must be absent
    /// or indicate UTC.  The behavior is undefined unless `0 <= length`.
    static int parse(DatetimeTz         *result,
                     const char         *string,
                     ssize_t             length,
                     ParseConfiguration  configuration = ParseConfiguration());

    /// Parse the specified initial `length` characters of the specified ISO
    /// 8601 `string` as a `Date` or `DateTz` value, depending on the
    /// presence of a zone designator, and load the value into the specified
    /// `result`, using the optionally specified `configuration`.  Return 0
    /// on success, and a non-zero value (with no effect) otherwise.
    /// `string` is assumed to be of the form:
    /// ```
    /// Default: YYYY-MM-DD{(+|-)hh{:}mm|Z}
    /// Basic:   YYYYMMDD{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `length` characters are parsed; parsing will fail if a
    /// proper prefix of `string` matches the expected format, but the
    /// entire `length` characters do not.  If the optional zone designator
    /// is present in the `string`, the input is parsed as a `DateTz` value,
    /// and as a `Date` value otherwise.  The behavior is undefined unless
    /// `0 <= length`.
    static int parse(DateOrDateTz       *result,
                     const char         *string,
                     ssize_t             length,
                     ParseConfiguration  configuration = ParseConfiguration());

    /// Parse the specified initial `length` characters of the specified ISO
    /// 8601 `string` as a `Time` or `TimeTz` value, depending on the
    /// presence of a zone designator, and load the value into the specified
    /// `result`, using the optionally specified `configuration`.  Return 0
    /// on success, and a non-zero value (with no effect) otherwise.
    /// `string` is assumed to be of the form:
    /// ```
    /// Default: hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// Basic:   hhmmss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `length` characters are parsed; parsing will fail if a
    /// proper prefix of `string` matches the expected format, but the
    /// entire `length` characters do not.  If an optional fractional second
    /// having more than six digits is present in `string`, it is rounded to
    /// the nearest value in microseconds.  If the optional zone designator
    /// is present in the `string`, the input is parsed as a `TimeTz` value,
    /// and as a `Time` value otherwise.  If a leap second is detected
    /// (i.e., the parsed value of the `second` attribute is 60; see {Leap
    /// Seconds}), the `second` attribute is taken to be 59, then an
    /// additional second is added to `result` at the end.  If the
    /// "hh:mm:ss" portion of `string` is "24:00:00", then the fractional
    /// second must be absent or 0, and the zone designator must be absent
    /// or indicate UTC.  The behavior is undefined unless `0 <= length`.
    static int parse(TimeOrTimeTz       *result,
                     const char         *string,
                     ssize_t             length,
                     ParseConfiguration  configuration = ParseConfiguration());

    /// Parse the specified initial `length` characters of the specified ISO
    /// 8601 `string` as a `Datetime` or  `DatetimeTz` value, depending on
    /// the presence of a zone designator, and load the value into the
    /// specified `result`, using the optionally specified `configuration`.
    /// Return 0 on success, and a non-zero value (with no effect)
    /// otherwise.  `string` is assumed to be of the form:
    /// ```
    /// Default: YYYY-MM-DDThh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// Basic:   YYYYMMDDThhmmss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `length` characters are parsed; parsing will fail if a
    /// proper prefix of `string` matches the expected format, but the
    /// entire `length` characters do not.  If an optional fractional second
    /// having more than six digits is present in `string`, it is rounded to
    /// the nearest value in microseconds.  If the optional zone designator
    /// is present in the `string`, the input is parsed as a `DatetimeTz`
    /// value, and as a `Datetime` value otherwise.  If a leap second is
    /// detected (i.e., the parsed value of the `second` attribute is 60;
    /// see {Leap Seconds}), the `second` attribute is taken to be 59, then
    /// an additional second is added to `result` at the end.  If the
    /// "hh:mm:ss" portion of `string` is "24:00:00", then the fractional
    /// second must be absent or 0, and the zone designator must be absent
    /// or indicate UTC.  The behavior is undefined unless `0 <= length`.
    static int parse(DatetimeOrDatetimeTz *result,
                     const char           *string,
                     ssize_t               length,
                     ParseConfiguration    configuration =
                                                         ParseConfiguration());

    /// Parse the specified ISO 8601 `string` as a `bsls::TimeInterval`
    /// value, and load the value into the specified `result`.  Return 0 on
    /// success, and a non-zero value (with no effect) otherwise.  `string`
    /// is assumed to be of the form:
    /// ```
    /// <Parsed Duration>
    /// ```
    /// See "Summary of Supported ISO 8601 Duration Representations" for a
    /// complete description of this format.
    ///
    /// *Exactly* `string.length()` characters are parsed; parsing will fail
    /// if a proper prefix of `string` matches the expected format, but the
    /// entire `string.length()` characters do not.  If an optional
    /// fractional second having more than nine digits is present in
    /// `string`, it is rounded to the nearest value in nanoseconds.  The
    /// behavior is undefined unless `0 <= length`.
    static int parse(bsls::TimeInterval      *result,
                     const bsl::string_view&  string);

    /// Parse the specified ISO 8601 `string` as a `Date` value, and load
    /// the value into the specified `result`, using the optionally
    /// specified `configuration`.  Return 0 on success, and a non-zero
    /// value (with no effect) otherwise.  `string` is assumed to be of the
    /// form:
    /// ```
    /// Default: YYYY-MM-DD{(+|-)hh{:}mm|Z}
    /// Basic:   YYYYMMDD{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `string.length()` characters are parsed; parsing will fail
    /// if a proper prefix of `string` matches the expected format, but the
    /// entire `string.length()` characters do not.  If the optional zone
    /// designator is present in `string`, it is parsed but ignored.  The
    /// behavior is undefined unless `string.data()` is non-null.
    static int parse(Date                    *result,
                     const bsl::string_view&  string,
                     ParseConfiguration       configuration =
                                                         ParseConfiguration());

    /// Parse the specified ISO 8601 `string` as a `Time` value, and load
    /// the value into the specified `result`, using the optionally
    /// specified `configuration`.  Return 0 on success, and a non-zero
    /// value (with no effect) otherwise.  `string` is assumed to be of the
    /// form:
    /// ```
    /// Default: hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// Basic:   hhmmss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `string.length()` characters are parsed; parsing will fail
    /// if a proper prefix of `string` matches the expected format, but the
    /// entire `string.length()` characters do not.  If an optional
    /// fractional second having more than six digits is present in
    /// `string`, it is rounded to the nearest value in microseconds.  If
    /// the optional zone designator is present in `string`, the resulting
    /// `Time` value is converted to the equivalent UTC time; if the zone
    /// designator is absent, UTC is assumed.  If a leap second is detected
    /// (i.e., the parsed value of the `second` attribute is 60; see {Leap
    /// Seconds}), the `second` attribute is taken to be 59, then an
    /// additional second is added to `result` at the end.  If the
    /// "hh:mm:ss" portion of `string` is "24:00:00", then the fractional
    /// second must be absent or 0, and the zone designator must be absent
    /// or indicate UTC.  The behavior is undefined unless `string.data()`
    /// is non-null.
    static int parse(Time                    *result,
                     const bsl::string_view&  string,
                     ParseConfiguration       configuration =
                                                         ParseConfiguration());

    /// Parse the specified ISO 8601 `string` as a `Datetime` value, and
    /// load the value into the specified `result`, using the optionally
    /// specified `configuration`.  Return 0 on success, and a non-zero
    /// value (with no effect) otherwise.  `string` is assumed to be of the
    /// form:
    /// ```
    /// Default: YYYY-MM-DDThh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// Basic:   YYYYMMDDThhmmss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `string.length()` characters are parsed; parsing will fail
    /// if a proper prefix of `string` matches the expected format, but the
    /// entire `string.length()` characters do not.  If an optional
    /// fractional second having more than six digits is present in
    /// `string`, it is rounded to the nearest value in microseconds.  If
    /// the optional zone designator is present in `string`, the resulting
    /// `Datetime` value is converted to the equivalent UTC value; if the
    /// zone designator is absent, UTC is assumed.  If a leap second is
    /// detected (i.e., the parsed value of the `second` attribute is 60;
    /// see {Leap Seconds}), the `second` attribute is taken to be 59, then
    /// an additional second is added to `result` at the end.  If the
    /// "hh:mm:ss" portion of `string` is "24:00:00", then the fractional
    /// second must be absent or 0, and the zone designator must be absent
    /// or indicate UTC.  The behavior is undefined unless `string.data()`
    /// is non-null.
    static int parse(Datetime                *result,
                     const bsl::string_view&  string,
                     ParseConfiguration       configuration =
                                                         ParseConfiguration());

    /// Parse the specified ISO 8601 `string` as a `DateTz` value, and load
    /// the value into the specified `result`, using the optionally
    /// specified `configuration`.  Return 0 on success, and a non-zero
    /// value (with no effect) otherwise.  `string` is assumed to be of the
    /// form:
    /// ```
    /// Default: YYYY-MM-DD{(+|-)hh{:}mm|Z}
    /// Basic:   YYYYMMDD{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `string.length()` characters are parsed; parsing will fail
    /// if a proper prefix of `string` matches the expected format, but the
    /// entire `string.length()` characters do not.  If the optional zone
    /// designator is not present in `string`, UTC is assumed.  The behavior
    /// is undefined unless `string.data()` is non-null.
    static int parse(DateTz                  *result,
                     const bsl::string_view&  string,
                     ParseConfiguration       configuration =
                                                         ParseConfiguration());

    /// Parse the specified ISO 8601 `string` as a `TimeTz` value, and load
    /// the value into the specified `result`, using the optionally
    /// specified `configuration`.  Return 0 on success, and a non-zero
    /// value (with no effect) otherwise.  `string` is assumed to be of the
    /// form:
    /// ```
    /// Default: hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// Basic:   hhmmss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `string.length()` characters are parsed; parsing will fail
    /// if a proper prefix of `string` matches the expected format, but the
    /// entire `string.length()` characters do not.  If an optional
    /// fractional second having more than six digits is present in
    /// `string`, it is rounded to the nearest value in microseconds.  If
    /// the optional zone designator is not present in `string`, UTC is
    /// assumed.  If a leap second is detected (i.e., the parsed value of
    /// the `second` attribute is 60; see {Leap Seconds}), the `second`
    /// attribute is taken to be 59, then an additional second is added to
    /// `result` at the end.  If the "hh:mm:ss" portion of `string` is
    /// "24:00:00", then the fractional second must be absent or 0, and the
    /// zone designator must be absent or indicate UTC.  The behavior is
    /// undefined unless `string.data()` is non-null.
    static int parse(TimeTz                  *result,
                     const bsl::string_view&  string,
                     ParseConfiguration       configuration =
                                                         ParseConfiguration());

    /// Parse the specified ISO 8601 `string` as a `DatetimeTz` value, and
    /// load the value into the specified `result`, using the optionally
    /// specified `configuration`.  Return 0 on success, and a non-zero
    /// value (with no effect) otherwise.  `string` is assumed to be of the
    /// form:
    /// ```
    /// Default: YYYY-MM-DDThh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// Basic:   YYYYMMDDThhmmss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `string.length()` characters are parsed; parsing will fail
    /// if a proper prefix of `string` matches the expected format, but the
    /// entire `string.length()` characters do not.  If an optional
    /// fractional second having more than six digits is present in
    /// `string`, it is rounded to the nearest value in microseconds.  If
    /// the optional zone designator is not present in `string`, UTC is
    /// assumed.  If a leap second is detected (i.e., the parsed value of
    /// the `second` attribute is 60; see {Leap Seconds}), the `second`
    /// attribute is taken to be 59, then an additional second is added to
    /// `result` at the end.  If the "hh:mm:ss" portion of `string` is
    /// "24:00:00", then the fractional second must be absent or 0, and the
    /// zone designator must be absent or indicate UTC.  The behavior is
    /// undefined unless `string.data()` is non-null.
    static int parse(DatetimeTz              *result,
                     const bsl::string_view&  string,
                     ParseConfiguration       configuration =
                                                         ParseConfiguration());

    /// Parse the specified ISO 8601 `string` as a `Date` or `DateTz` value,
    /// depending on the presence of a zone designator, and load the value
    /// into the specified `result`, using the optionally specified
    /// `configuration`.  Return 0 on success, and a non-zero value (with no
    /// effect) otherwise.  `string` is assumed to be of the form:
    /// ```
    /// Default: YYYY-MM-DD{(+|-)hh{:}mm|Z}
    /// Basic:   YYYYMMDD{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `string.length()` characters are parsed; parsing will fail
    /// if a proper prefix of `string` matches the expected format, but the
    /// entire `string.length()` characters do not.  If the optional zone
    /// designator is present in the `string`, the input is parsed as a
    /// `DateTz` value, and as a `Date` value otherwise.  The behavior is
    /// undefined unless `string.data()` is non-null.
    static int parse(DateOrDateTz            *result,
                     const bsl::string_view&  string,
                     ParseConfiguration       configuration =
                                                         ParseConfiguration());

    /// Parse the specified ISO 8601 `string` as a `Time` or `TimeTz` value,
    /// depending on the presence of a zone designator, and load the value
    /// into the specified `result`, using the optionally specified
    /// `configuration`.  Return 0 on success, and a non-zero value (with no
    /// effect) otherwise.  `string` is assumed to be of the form:
    /// ```
    /// Default: hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// Basic:   hhmmss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `string.length()` characters are parsed; parsing will fail
    /// if a proper prefix of `string` matches the expected format, but the
    /// entire `string.length()` characters do not.  If an optional
    /// fractional second having more than six digits is present in
    /// `string`, it is rounded to the nearest value in microseconds.  If
    /// the optional zone designator is present in the `string`, the input
    /// is parsed as a `TimeTz` value, and as a `Time` value otherwise.  If
    /// a leap second is detected (i.e., the parsed value of the `second`
    /// attribute is 60; see {Leap Seconds}), the `second` attribute is
    /// taken to be 59, then an additional second is added to `result` at
    /// the end.  If the "hh:mm:ss" portion of `string` is "24:00:00", then
    /// the fractional second must be absent or 0, and the zone designator
    /// must be absent or indicate UTC.  The behavior is undefined unless
    /// `string.data()` is non-null.
    static int parse(TimeOrTimeTz            *result,
                     const bsl::string_view&  string,
                     ParseConfiguration       configuration =
                                                         ParseConfiguration());

    /// Parse the specified ISO 8601 `string` as a `Datetime` or
    /// `DatetimeTz` value, depending on the presence of a zone designator,
    /// and load the value into the specified `result`, using the optionally
    /// specified `configuration`.  Return 0 on success, and a non-zero
    /// value (with no effect) otherwise.  `string` is assumed to be of the
    /// form:
    /// ```
    /// Default: YYYY-MM-DDThh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// Basic:   YYYYMMDDThhmmss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// *Exactly* `string.length()` characters are parsed; parsing will fail
    /// if a proper prefix of `string` matches the expected format, but the
    /// entire `string.length()` characters do not.  If an optional
    /// fractional second having more than six digits is present in
    /// `string`, it is rounded to the nearest value in microseconds.  If
    /// the optional zone designator is present in the `string`, the input
    /// is parsed as a `DatetimeTz` value, and as a `Datetime` value
    /// otherwise.  If a leap second is detected (i.e., the parsed value of
    /// the `second` attribute is 60; see {Leap Seconds}), the `second`
    /// attribute is taken to be 59, then an additional second is added to
    /// `result` at the end.  If the "hh:mm:ss" portion of `string` is
    /// "24:00:00", then the fractional second must be absent or 0, and the
    /// zone designator must be absent or indicate UTC.  The behavior is
    /// undefined unless `string.data()` is non-null.
    static int parse(DatetimeOrDatetimeTz    *result,
                     const bsl::string_view&  string,
                     ParseConfiguration       configuration =
                                                         ParseConfiguration());

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // DEPRECATED METHODS

    /// Parse the specified initial `length` characters of the specified
    /// "relaxed" ISO 8601 `string` as a `Datetime` value, and load the
    /// value into the specified `result`.  Return 0 on success, and a
    /// non-zero value (with no effect) otherwise.  `string` is assumed to
    /// be of the form:
    /// ```
    /// YYYY-MM-DD(T| )hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// The "relaxed" format parsed by this function is a superset of the
    /// strict ISO 8601 format, currently allowing a SPACE character to be
    /// used as a separated (where ISO 8601 requires a `T`).  *Exactly*
    /// `length` characters are parsed; parsing will fail if a proper prefix
    /// of `string` matches the expected format, but the entire `length`
    /// characters do not.  If an optional fractional second having more
    /// than six digits is present in `string`, it is rounded to the nearest
    /// value in microseconds.  If the optional zone designator is present
    /// in `string`, the resulting `Datetime` value is converted to the
    /// equivalent UTC value; if the zone designator is absent, UTC is
    /// assumed.  If a leap second is detected (i.e., the parsed value of
    /// the `second` attribute is 60; see {Leap Seconds}), the `second`
    /// attribute is taken to be 59, then an additional second is added to
    /// `result` at the end.  If the "hh:mm:ss" portion of `string` is
    /// "24:00:00", then the fractional second must be absent or 0, and the
    /// zone designator must be absent or indicate UTC.  The behavior is
    /// undefined unless `0 <= length`.
    ///
    /// @DEPRECATED: Use `parse` with `configuration.relaxed() == true`
    /// instead.
    BDLT_ISO8601UTIL_DEPRECATE_PARSERELAXED
    static int parseRelaxed(Datetime   *result,
                            const char *string,
                            ssize_t     length);

    /// Parse the specified initial `length` characters of the specified
    /// "relaxed" ISO 8601 `string` as a `DatetimeTz` value, and load the
    /// value into the specified `result`.  Return 0 on success, and a
    /// non-zero value (with no effect) otherwise.  `string` is assumed to
    /// be of the form:
    /// ```
    /// YYYY-MM-DD(T| )hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// The "relaxed" format parsed by this function is a superset of the
    /// strict ISO 8601 format, currently allowing a SPACE character to be
    /// used as a separated (where ISO 8601 requires a `T`).  *Exactly*
    /// `length` characters are parsed; parsing will fail if a proper prefix
    /// of `string` matches the expected format, but the entire `length`
    /// characters do not.  If an optional fractional second having more
    /// than six digits is present in `string`, it is rounded to the nearest
    /// value in microseconds.  If the optional zone designator is not
    /// present in `string`, UTC is assumed.  If a leap second is detected
    /// (i.e., the parsed value of the `second` attribute is 60; see {Leap
    /// Seconds}), the `second` attribute is taken to be 59, then an
    /// additional second is added to `result` at the end.  If the
    /// "hh:mm:ss" portion of `string` is "24:00:00", then the fractional
    /// second must be absent or 0, and the zone designator must be absent
    /// or indicate UTC.  The behavior is undefined unless `0 <= length`.
    ///
    /// @DEPRECATED: Use `parse` with `configuration.relaxed() == true`
    /// instead.
    BDLT_ISO8601UTIL_DEPRECATE_PARSERELAXED
    static int parseRelaxed(DatetimeTz *result,
                            const char *string,
                            ssize_t     length);

    /// Parse the specified initial `length` characters of the specified
    /// "relaxed" ISO 8601 `string` as a `Datetime` or `DatetimeTz` value,
    /// depending on the presence of a zone designator, and load the value
    /// into the specified `result`.  Return 0 on success, and a non-zero
    /// value (with no effect) otherwise.  `string` is assumed to be of the
    /// form:
    /// ```
    /// YYYY-MM-DD(T| )hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// The "relaxed" format parsed by this function is a superset of the
    /// strict ISO 8601 format, currently allowing a SPACE character to be
    /// used as a separated (where ISO 8601 requires a `T`).  *Exactly*
    /// `length` characters are parsed; parsing will fail if a proper prefix
    /// of `string` matches the expected format, but the entire `length`
    /// characters do not.  If an optional fractional second having more
    /// than six digits is present in `string`, it is rounded to the nearest
    /// value in microseconds.  If the optional zone designator is present
    /// in the `string`, the input is parsed as a `DatetimeTz` value, and as
    /// a `Datetime` value otherwise.  If a leap second is detected (i.e.,
    /// the parsed value of the `second` attribute is 60; see {Leap
    /// Seconds}), the `second` attribute is taken to be 59, then an
    /// additional second is added to `result` at the end.  If the
    /// "hh:mm:ss" portion of `string` is "24:00:00", then the fractional
    /// second must be absent or 0, and the zone designator must be absent
    /// or indicate UTC.  The behavior is undefined unless `0 <= length`.
    ///
    /// @DEPRECATED: Use `parse` with `configuration.relaxed() == true`
    /// instead.
    BDLT_ISO8601UTIL_DEPRECATE_PARSERELAXED
    static int parseRelaxed(DatetimeOrDatetimeTz *result,
                            const char           *string,
                            ssize_t               length);

    /// Parse the specified "relaxed" ISO 8601 `string` as a `Datetime`
    /// value, and load the value into the specified `result`.  Return 0 on
    /// success, and a non-zero value (with no effect) otherwise.  `string`
    /// is assumed to be of the form:
    /// ```
    /// YYYY-MM-DD(T| )hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// The "relaxed" format parsed by this function is a superset of the
    /// strict ISO 8601 format, currently allowing a SPACE character to be
    /// used as a separated (where ISO 8601 requires a `T`).  *Exactly*
    /// `string.length()` characters are parsed; parsing will fail if a
    /// proper prefix of `string` matches the expected format, but the
    /// entire `string.length()` characters do not.  If an optional
    /// fractional second having more than six digits is present in
    /// `string`, it is rounded to the nearest value in microseconds.  If
    /// the optional zone designator is present in `string`, the resulting
    /// `Datetime` value is converted to the equivalent UTC value; if the
    /// zone designator is absent, UTC is assumed.  If a leap second is
    /// detected (i.e., the parsed value of the `second` attribute is 60;
    /// see {Leap Seconds}), the `second` attribute is taken to be 59, then
    /// an additional second is added to `result` at the end.  If the
    /// "hh:mm:ss" portion of `string` is "24:00:00", then the fractional
    /// second must be absent or 0, and the zone designator must be absent
    /// or indicate UTC.  The behavior is undefined unless `string.data()`
    /// is non-null.
    ///
    /// @DEPRECATED: Use `parse` with `configuration.relaxed() == true`
    /// instead.
    BDLT_ISO8601UTIL_DEPRECATE_PARSERELAXED
    static int parseRelaxed(Datetime *result, const bsl::string_view& string);

    /// Parse the specified "relaxed" ISO 8601 `string` as a `DatetimeTz`
    /// value, and load the value into the specified `result`.  Return 0 on
    /// success, and a non-zero value (with no effect) otherwise.  `string`
    /// is assumed to be of the form:
    /// ```
    /// YYYY-MM-DD(T| )hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// The "relaxed" format parsed by this function is a superset of the
    /// strict ISO 8601 format, currently allowing a SPACE character to be
    /// used as a separated (where ISO 8601 requires a `T`).  *Exactly*
    /// `string.length()` characters are parsed; parsing will fail if a
    /// proper prefix of `string` matches the expected format, but the
    /// entire `string.length()` characters do not.  If an optional
    /// fractional second having more than six digits is present in
    /// `string`, it is rounded to the nearest value in microseconds.  If
    /// the optional zone designator is not present in `string`, UTC is
    /// assumed.  If a leap second is detected (i.e., the parsed value of
    /// the `second` attribute is 60; see {Leap Seconds}), the `second`
    /// attribute is taken to be 59, then an additional second is added to
    /// `result` at the end.  If the "hh:mm:ss" portion of `string` is
    /// "24:00:00", then the fractional second must be absent or 0, and the
    /// zone designator must be absent or indicate UTC.  The behavior is
    /// undefined unless `string.data()` is non-null.
    ///
    /// @DEPRECATED: Use `parse` with `configuration.relaxed() == true`
    /// instead.
    BDLT_ISO8601UTIL_DEPRECATE_PARSERELAXED
    static int parseRelaxed(DatetimeTz              *result,
                            const bsl::string_view&  string);

    /// Parse the specified "relaxed" ISO 8601 `string` as a `Datetime` or
    /// `DatetimeTz` value, depending on the presence of a zone designator,
    /// and load the value into the specified `result`.  Return 0 on
    /// success, and a non-zero value (with no effect) otherwise.  `string`
    /// is assumed to be of the form:
    /// ```
    /// YYYY-MM-DD(T| )hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
    /// ```
    /// The "relaxed" format parsed by this function is a superset of the
    /// strict ISO 8601 format, currently allowing a SPACE character to be
    /// used as a separated (where ISO 8601 requires a `T`).  *Exactly*
    /// `string.length()` characters are parsed; parsing will fail if a
    /// proper prefix of `string` matches the expected format, but the
    /// entire `string.length()` characters do not.  If an optional
    /// fractional second having more than six digits is present in
    /// `string`, it is rounded to the nearest value in microseconds.  If
    /// the optional zone designator is present in the `string`, the input
    /// is parsed as a `DatetimeTz` value, and as a `Datetime` value
    /// otherwise.  If a leap second is detected (i.e., the parsed value of
    /// the `second` attribute is 60; see {Leap Seconds}), the `second`
    /// attribute is taken to be 59, then an additional second is added to
    /// `result` at the end.  If the "hh:mm:ss" portion of `string` is
    /// "24:00:00", then the fractional second must be absent or 0, and the
    /// zone designator must be absent or indicate UTC.  The behavior is
    /// undefined unless `string.data()` is non-null.
    ///
    /// @DEPRECATED: Use `parse` with `configuration.relaxed() == true`
    /// instead.
    BDLT_ISO8601UTIL_DEPRECATE_PARSERELAXED
    static int parseRelaxed(DatetimeOrDatetimeTz    *result,
                            const bsl::string_view&  string);

    /// @DEPRECATED: Use the overloads taking the `bufferLength` argument
    /// *before* the `object` argument instead.
    BDLT_ISO8601UTIL_DEPRECATE_GENERATE_ORDER
    static int generate(char              *buffer,
                        const Date&        object,
                        ssize_t            bufferLength);
    BDLT_ISO8601UTIL_DEPRECATE_GENERATE_ORDER
    static int generate(char              *buffer,
                        const Time&        object,
                        ssize_t            bufferLength);
    BDLT_ISO8601UTIL_DEPRECATE_GENERATE_ORDER
    static int generate(char              *buffer,
                        const Datetime&    object,
                        ssize_t            bufferLength);
    BDLT_ISO8601UTIL_DEPRECATE_GENERATE_ORDER
    static int generate(char              *buffer,
                        const DateTz&      object,
                        ssize_t            bufferLength);
    BDLT_ISO8601UTIL_DEPRECATE_GENERATE_ORDER
    static int generate(char              *buffer,
                        const TimeTz&      object,
                        ssize_t            bufferLength);
    BDLT_ISO8601UTIL_DEPRECATE_GENERATE_ORDER
    static int generate(char              *buffer,
                        const DatetimeTz&  object,
                        ssize_t            bufferLength);

    /// @DEPRECATED: Use the overloads taking an `Iso8601UtilConfiguration`
    /// object instead.
    BDLT_ISO8601UTIL_DEPRECATE_GENERATE
    static int generate(char              *buffer,
                        const DateTz&      object,
                        ssize_t            bufferLength,
                        bool               useZAbbreviationForUtc);
    BDLT_ISO8601UTIL_DEPRECATE_GENERATE
    static int generate(char              *buffer,
                        const TimeTz&      object,
                        ssize_t            bufferLength,
                        bool               useZAbbreviationForUtc);
    BDLT_ISO8601UTIL_DEPRECATE_GENERATE
    static int generate(char              *buffer,
                        const DatetimeTz&  object,
                        ssize_t            bufferLength,
                        bool               useZAbbreviationForUtc);

    /// @DEPRECATED: Use the overloads taking an `Iso8601UtilConfiguration`
    /// object instead.
    BDLT_ISO8601UTIL_DEPRECATE_GENERATE
    static bsl::ostream& generate(bsl::ostream&     stream,
                                  const DateTz&     object,
                                  bool              useZAbbreviationForUtc);
    BDLT_ISO8601UTIL_DEPRECATE_GENERATE
    static bsl::ostream& generate(bsl::ostream&     stream,
                                  const TimeTz&     object,
                                  bool              useZAbbreviationForUtc);
    BDLT_ISO8601UTIL_DEPRECATE_GENERATE
    static bsl::ostream& generate(bsl::ostream&     stream,
                                  const DatetimeTz& object,
                                  bool              useZAbbreviationForUtc);

    /// @DEPRECATED: Use the overloads taking an `Iso8601UtilConfiguration`
    /// object instead.
    BDLT_ISO8601UTIL_DEPRECATE_GENERATERAW
    static int generateRaw(char              *buffer,
                           const DateTz&      object,
                           bool               useZAbbreviationForUtc);
    BDLT_ISO8601UTIL_DEPRECATE_GENERATERAW
    static int generateRaw(char              *buffer,
                           const TimeTz&      object,
                           bool               useZAbbreviationForUtc);
    BDLT_ISO8601UTIL_DEPRECATE_GENERATERAW
    static int generateRaw(char              *buffer,
                           const DatetimeTz&  object,
                           bool               useZAbbreviationForUtc);

#endif  // BDE_OMIT_INTERNAL_DEPRECATED
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                            // ------------------
                            // struct Iso8601Util
                            // ------------------

// PRIVATE CLASS METHODS
inline
Iso8601UtilConfiguration Iso8601Util::defaultConfiguration()
{
    return Iso8601UtilConfiguration::defaultConfiguration();
}

// CLASS METHODS
inline
int Iso8601Util::generate(char                     *buffer,
                          ssize_t                   bufferLength,
                          const bsls::TimeInterval& object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    defaultConfiguration());
}

inline
int Iso8601Util::generate(char        *buffer,
                          ssize_t      bufferLength,
                          const Date&  object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    defaultConfiguration());
}

inline
int Iso8601Util::generate(char        *buffer,
                          ssize_t      bufferLength,
                          const Time&  object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    defaultConfiguration());
}

inline
int
Iso8601Util::generate(char            *buffer,
                      ssize_t          bufferLength,
                      const Datetime&  object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    defaultConfiguration());
}

inline
int Iso8601Util::generate(char          *buffer,
                          ssize_t        bufferLength,
                          const DateTz&  object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    defaultConfiguration());
}

inline
int Iso8601Util::generate(char          *buffer,
                          ssize_t        bufferLength,
                          const TimeTz&  object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    defaultConfiguration());
}

inline
int
Iso8601Util::generate(char              *buffer,
                      ssize_t            bufferLength,
                      const DatetimeTz&  object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    defaultConfiguration());
}

inline
int Iso8601Util::generate(char                *buffer,
                          ssize_t              bufferLength,
                          const DateOrDateTz&  object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    defaultConfiguration());
}

inline
int Iso8601Util::generate(char                *buffer,
                          ssize_t              bufferLength,
                          const TimeOrTimeTz&  object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    defaultConfiguration());
}

inline
int Iso8601Util::generate(char                        *buffer,
                          ssize_t                      bufferLength,
                          const DatetimeOrDatetimeTz&  object)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    bufferLength,
                    object,
                    defaultConfiguration());
}

inline
int Iso8601Util::generate(bsl::string              *string,
                          const bsls::TimeInterval& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(bsl::string *string, const Date& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(bsl::string *string, const Time& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(bsl::string *string, const Datetime& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(bsl::string *string, const DateTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(bsl::string *string, const TimeTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(bsl::string *string, const DatetimeTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(bsl::string *string, const DateOrDateTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(bsl::string *string, const TimeOrTimeTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(bsl::string                 *string,
                          const DatetimeOrDatetimeTz&  object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::string              *string,
                          const bsls::TimeInterval& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::string *string, const Date& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::string *string, const Time& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::string *string, const Datetime& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::string *string, const DateTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::string *string, const TimeTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::string *string, const DatetimeTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::string *string, const DateOrDateTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::string *string, const TimeOrTimeTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::string                 *string,
                          const DatetimeOrDatetimeTz&  object)
{
    return generate(string, object, defaultConfiguration());
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
inline
int Iso8601Util::generate(std::pmr::string         *string,
                          const bsls::TimeInterval& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::pmr::string *string, const Date& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::pmr::string *string, const Time& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::pmr::string *string, const Datetime& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::pmr::string *string, const DateTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::pmr::string *string, const TimeTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::pmr::string *string, const DatetimeTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::pmr::string *string, const DateOrDateTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::pmr::string *string, const TimeOrTimeTz& object)
{
    return generate(string, object, defaultConfiguration());
}

inline
int Iso8601Util::generate(std::pmr::string            *string,
                          const DatetimeOrDatetimeTz&  object)
{
    return generate(string, object, defaultConfiguration());
}
#endif

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream&             stream,
                                    const bsls::TimeInterval& object)
{
    return generate(stream,
                    object,
                    defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const bsls::TimeInterval&       object,
                                 const GenerateConfiguration&    configuration)
{
    char buffer[k_TIMEINTERVAL_STRLEN + 1];

    const int len = generate(buffer,
                             k_TIMEINTERVAL_STRLEN,
                             object,
                             configuration);
    BSLS_ASSERT_SAFE(k_TIMEINTERVAL_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream, const Date& object)
{
    return generate(stream,
                    object,
                    defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const Date&                     object,
                                 const GenerateConfiguration&    configuration)
{
    char buffer[k_DATE_STRLEN + 1];

    const int len = generate(buffer, k_DATE_STRLEN, object, configuration);
    BSLS_ASSERT_SAFE(k_DATE_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream, const Time& object)
{
    return generate(stream,
                    object,
                    defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const Time&                     object,
                                 const GenerateConfiguration&    configuration)
{
    char buffer[k_TIME_STRLEN + 1];

    const int len = generate(buffer, k_TIME_STRLEN, object, configuration);
    BSLS_ASSERT_SAFE(k_TIME_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream&
Iso8601Util::generate(bsl::ostream& stream, const Datetime& object)
{
    return generate(stream,
                    object,
                    defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const Datetime&                 object,
                                 const GenerateConfiguration&    configuration)
{
    char buffer[k_DATETIME_STRLEN + 1];

    const int len = generate(buffer, k_DATETIME_STRLEN, object, configuration);
    BSLS_ASSERT_SAFE(k_DATETIME_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream, const DateTz& object)
{
    return generate(stream,
                    object,
                    defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const DateTz&                   object,
                                 const GenerateConfiguration&    configuration)
{
    char buffer[k_DATETZ_STRLEN + 1];

    const int len = generate(buffer, k_DATETZ_STRLEN, object, configuration);
    BSLS_ASSERT_SAFE(k_DATETZ_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream, const TimeTz& object)
{
    return generate(stream,
                    object,
                    defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const TimeTz&                   object,
                                 const GenerateConfiguration&    configuration)
{
    char buffer[k_TIMETZ_STRLEN + 1];

    const int len = generate(buffer, k_TIMETZ_STRLEN, object, configuration);
    BSLS_ASSERT_SAFE(k_TIMETZ_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream&
Iso8601Util::generate(bsl::ostream& stream, const DatetimeTz& object)
{
    return generate(stream,
                    object,
                    defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const DatetimeTz&               object,
                                 const GenerateConfiguration&    configuration)
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
bsl::ostream&
Iso8601Util::generate(bsl::ostream& stream, const DateOrDateTz& object)
{
    return generate(stream,
                    object,
                    defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const DateOrDateTz&             object,
                                 const GenerateConfiguration&    configuration)
{
    char buffer[k_DATETZ_STRLEN + 1];

    const int len = generate(buffer,
                             k_DATETZ_STRLEN,
                             object,
                             configuration);
    BSLS_ASSERT_SAFE(k_DATETZ_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream&
Iso8601Util::generate(bsl::ostream& stream, const TimeOrTimeTz& object)
{
    return generate(stream,
                    object,
                    defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const TimeOrTimeTz&             object,
                                 const GenerateConfiguration&    configuration)
{
    char buffer[k_TIMETZ_STRLEN + 1];

    const int len = generate(buffer,
                             k_TIMETZ_STRLEN,
                             object,
                             configuration);
    BSLS_ASSERT_SAFE(k_TIMETZ_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream&
Iso8601Util::generate(bsl::ostream& stream, const DatetimeOrDatetimeTz& object)
{
    return generate(stream,
                    object,
                    defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const DatetimeOrDatetimeTz&     object,
                                 const GenerateConfiguration&    configuration)
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
int Iso8601Util::generateRaw(char *buffer, const bsls::TimeInterval& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const Date& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const Time& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const Datetime& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const DateTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const TimeTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const DatetimeTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const DateOrDateTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const TimeOrTimeTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const DatetimeOrDatetimeTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       defaultConfiguration());
}

inline
int Iso8601Util::parse(bsls::TimeInterval      *result,
                       const bsl::string_view&  string)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), string.length());
}

inline
int Iso8601Util::parse(Date                    *result,
                       const bsl::string_view&  string,
                       ParseConfiguration       configuration)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), string.length(), configuration);
}

inline
int Iso8601Util::parse(Time                    *result,
                       const bsl::string_view&  string,
                       ParseConfiguration       configuration)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), string.length(), configuration);
}

inline
int Iso8601Util::parse(Datetime                *result,
                       const bsl::string_view&  string,
                       ParseConfiguration       configuration)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), string.length(), configuration);
}

inline
int Iso8601Util::parse(DateTz                  *result,
                       const bsl::string_view&  string,
                       ParseConfiguration       configuration)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), string.length(), configuration);
}

inline
int Iso8601Util::parse(TimeTz                  *result,
                       const bsl::string_view&  string,
                       ParseConfiguration       configuration)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), string.length(), configuration);
}

inline
int Iso8601Util::parse(DatetimeTz              *result,
                       const bsl::string_view&  string,
                       ParseConfiguration       configuration)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), string.length(), configuration);
}

inline
int Iso8601Util::parse(DateOrDateTz            *result,
                       const bsl::string_view&  string,
                       ParseConfiguration       configuration)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), string.length(), configuration);
}

inline
int Iso8601Util::parse(TimeOrTimeTz            *result,
                       const bsl::string_view&  string,
                       ParseConfiguration       configuration)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), string.length(), configuration);
}

inline
int Iso8601Util::parse(DatetimeOrDatetimeTz    *result,
                       const bsl::string_view&  string,
                       ParseConfiguration       configuration)
{
    BSLS_ASSERT_SAFE(string.data());

    return parse(result, string.data(), string.length(), configuration);
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// DEPRECATED METHODS
inline
int Iso8601Util::parseRelaxed(Datetime *result, const bsl::string_view& string)
{
    BSLS_ASSERT_SAFE(string.data());

    return parseRelaxed(result,
                        string.data(),
                        string.length());
}

inline
int Iso8601Util::parseRelaxed(DatetimeTz              *result,
                              const bsl::string_view&  string)
{
    BSLS_ASSERT_SAFE(string.data());

    return parseRelaxed(result,
                        string.data(),
                        string.length());
}

inline
int Iso8601Util::parseRelaxed(DatetimeOrDatetimeTz    *result,
                              const bsl::string_view&  string)
{
    BSLS_ASSERT_SAFE(string.data());

    return parseRelaxed(result,
                        string.data(),
                        string.length());
}

inline
int Iso8601Util::generate(char        *buffer,
                          const Date&  object,
                          ssize_t      bufferLength)
{
    return generate(buffer, bufferLength, object);
}

inline
int Iso8601Util::generate(char        *buffer,
                          const Time&  object,
                          ssize_t      bufferLength)
{
    return generate(buffer, bufferLength, object);
}

inline
int Iso8601Util::generate(char            *buffer,
                          const Datetime&  object,
                          ssize_t          bufferLength)
{
    return generate(buffer, bufferLength, object);
}

inline
int Iso8601Util::generate(char          *buffer,
                          const DateTz&  object,
                          ssize_t        bufferLength)
{
    return generate(buffer, bufferLength, object);
}

inline
int Iso8601Util::generate(char          *buffer,
                          const TimeTz&  object,
                          ssize_t        bufferLength)
{
    return generate(buffer, bufferLength, object);
}

inline
int Iso8601Util::generate(char              *buffer,
                          const DatetimeTz&  object,
                          ssize_t            bufferLength)
{
    return generate(buffer, bufferLength, object);
}

inline
int Iso8601Util::generate(char          *buffer,
                          const DateTz&  object,
                          ssize_t        bufferLength,
                          bool           useZAbbreviationForUtc)
{
    Iso8601UtilConfiguration configuration = defaultConfiguration();
    configuration.setUseZAbbreviationForUtc(useZAbbreviationForUtc);

    return generate(buffer, bufferLength, object, configuration);
}

inline
int Iso8601Util::generate(char          *buffer,
                          const TimeTz&  object,
                          ssize_t        bufferLength,
                          bool           useZAbbreviationForUtc)
{
    Iso8601UtilConfiguration configuration = defaultConfiguration();
    configuration.setUseZAbbreviationForUtc(useZAbbreviationForUtc);

    return generate(buffer, bufferLength, object, configuration);
}

inline
int Iso8601Util::generate(char              *buffer,
                          const DatetimeTz&  object,
                          ssize_t            bufferLength,
                          bool               useZAbbreviationForUtc)
{
    Iso8601UtilConfiguration configuration = defaultConfiguration();
    configuration.setUseZAbbreviationForUtc(useZAbbreviationForUtc);

    return generate(buffer, bufferLength, object, configuration);
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream,
                                    const DateTz& object,
                                    bool          useZAbbreviationForUtc)
{
    Iso8601UtilConfiguration configuration = defaultConfiguration();
    configuration.setUseZAbbreviationForUtc(useZAbbreviationForUtc);

    return generate(stream, object, configuration);
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream,
                                    const TimeTz& object,
                                    bool          useZAbbreviationForUtc)
{
    Iso8601UtilConfiguration configuration = defaultConfiguration();
    configuration.setUseZAbbreviationForUtc(useZAbbreviationForUtc);

    return generate(stream, object, configuration);
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream&     stream,
                                    const DatetimeTz& object,
                                    bool              useZAbbreviationForUtc)
{
    Iso8601UtilConfiguration configuration = defaultConfiguration();
    configuration.setUseZAbbreviationForUtc(useZAbbreviationForUtc);

    return generate(stream, object, configuration);
}

inline
int Iso8601Util::generateRaw(char          *buffer,
                             const DateTz&  object,
                             bool           useZAbbreviationForUtc)
{
    Iso8601UtilConfiguration configuration = defaultConfiguration();
    configuration.setUseZAbbreviationForUtc(useZAbbreviationForUtc);

    return generateRaw(buffer, object, configuration);
}

inline
int Iso8601Util::generateRaw(char          *buffer,
                             const TimeTz&  object,
                             bool           useZAbbreviationForUtc)
{
    Iso8601UtilConfiguration configuration = defaultConfiguration();
    configuration.setUseZAbbreviationForUtc(useZAbbreviationForUtc);

    return generateRaw(buffer, object, configuration);
}

inline
int Iso8601Util::generateRaw(char              *buffer,
                             const DatetimeTz&  object,
                             bool               useZAbbreviationForUtc)
{
    Iso8601UtilConfiguration configuration = defaultConfiguration();
    configuration.setUseZAbbreviationForUtc(useZAbbreviationForUtc);

    return generateRaw(buffer, object, configuration);
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

}  // close package namespace
}  // close enterprise namespace

#endif

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
