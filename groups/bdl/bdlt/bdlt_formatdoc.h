// bdlt_formatdoc.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLT_FORMATDOC
#define INCLUDED_BDLT_FORMATDOC

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide documentation for `bsl::format` of bdlt value types
//
//@DESCRIPTION: This component provides documentation and a usage example
// demonstrating the behavior and features of `bsl::format` as applied to bdlt
// value types.
//
//@SEE ALSO: bdlt_dateformatter, bdlt_datetzformatter, bdlt_timeformatter,
// bdlt_timetzformatter, bdlt_datetimeformatter, bdlt_datetimetzformatter
//
// `bsl::format` is a powerful formatting function that can be used to format,
// among other things, `bdlt` value types.  The formatting of types is
// controlled by a set of formatting options that are specified in the format
// string passed to `bsl::format` in a way very similar to how the format
// string in `printf` formats types being printed.  While `printf` can only
// format a few hard-coded types, `bsl::format` can be extended to format more
// sophisticated types, and all `bdlt` value types are supported with such
// extensions.
//
// The format string passed to `bsl::format` can contain any number of '%'
// sequences, which are replaced by the formatted value of the corresponding
// argument, or part of the corresponding argument.  Each type has a specific,
// limited set of '%'-sequences that can be used with it, and using any other
// '%'-sequence is an error.  The formatting options for `bdlt` value types
// are described both in this component and in the component documentation of
// each `bdlt` value type.
//
// The following is a list of '%'-sequences supported for `bdlt` value types,
// though not all the sequences are supported for all the types.  Many of the
// sequences are copied from `std::format`s support for
// `std::chrono::duration`, see
// https://en.cppreference.com/w/cpp/chrono/duration.html, but some other ones
// are specific to `bdlt` value types.
//
// Just like for all other `bsl::format` errors, errors will be detected at
// compile time in C++20 and later, but in C++17 and earlier, errors will
// result in an exception being thrown at run time.
//
// Literal '%' sequences that just output one fixed character are:
// * "%%" - outputs '%'
// * "%n" - outputs newline ('\n')
// * "%t" - outputs tab ('\t')
//
// All '%' sequences supported for `bdlt` value types are:
// * Date alone: '%D' same as "{:%d%h%Y}"
// * Date alone: '%F' Iso8601, like "{:%Y-%m-%d}"
// * Time alone: '%T', same as "{:%H:%M:%S}", 6 digit fraction
// * Time zone: '%z', 2 digit hour and 2-digit minute offset from UTC, with or
//   without a colon separating hours and minutes
// * Year: '%Y' (4-digit), '%y' (2-digit)
// * Century: '%C' (2-digit)
// * Numeric month: '%m' (2-digit)
// * Abbreviation of month: (always 3-letter upper case): '%b' or '%h'
// * Day of month: (always 2-digit) '%d' ('0'-padded), '%e' (space-padded)
// * Abbreviated day of week: '%a' (3 letter upper case)
// * Day of week: '%u' (numeric 1-7, Monday is 1)
// * Day of week: '%w' (numeric 0-6, Sunday is 0)
// * Day of year: '%j' (numeric 3-digit)
// * Hour: '%H' (2 digit) "00" - "24"
// * Hour: '%I' (2 digit) "01" - "12"
// * AM/PM: '%p' - "AM" or "PM"
// * Minute: '%M' (2 digits) "00" - "59"
// * Second: '%S' (2 digits "00" - "59" + '.' + fraction)
//
// * Composite: "{}" formats the value as if streamed by '<<', default 6 digit
//   precision  for seconds, no ':' between hours and minutes of any time zone
// * Composite: '%i' for bdlt value types formats the entire value like
//   Iso8601, were precision defaults to 3 digit and there's a ':' between
//   hours and minutes of any time zone.
//
// It is an error to specify a '%'-sequence that does not apply to the type
// being formatted, or a sub-type of it -- for example, all sequences that
// apply to `Date` or `Time` can be used on a `Datetime` object.
//
// `bsl::format` strings of bdlt value types take the form of either "{}" or
// "{:[padding][precision][bdlt-modifiers]['%'-sequences]}" where all fields
// surrounded by '[]'s are optional.  While in `printf` the modifiers of a
// '%'-sequence occur between the '%' and the type specifier, in `bsl::format`
// the modifiers occur between the first ':' and the first '%' (if any).
// * The `padding` takes the form of
//   `<pad character><alignment command><width>` where
//   - <pad character> is the character to pad with, which defaults to space if
//     omitted, and must be a single character
//   - <alignment command> is one of: '<' for left, '>' for right, and '^' for
//     centered, and
//   - <width> is the minimum total width of the padded value output
// * The `precision` is indicated by a '.' followed by an integral number of
//   digits.
// * The `bdlt`-modifiers are specific to `bdlt` value types.  One affects the
//   output of seconds, and three affect the output of time zones:
//   - ',' decimal between integral seconds and fraction is a comma instead of
//     a period
//   - ':' separate hours and minutes in time zone with ':'
//   - '_' suppress ':' between hours and minutes in time zone
//   - 'Z' if time zone offset is 0, display it as 'Z'
// Characters after the first '%' that are not part of a valid '%'-sequence are
// copied to output without modification.  The modifiers must come in the order
// "<padding><precision><bdlt-specific modifiers>".  The bdlt-specific
// modifiers must come last, but among themselves, may occur in any order.
//
///Usage
///-----
// First, create a form of assertion macro that will compare a literal string
// to the result of an expression and output the value of the expression if
// they don't match (continuations elided in .h to suppress compiler errors):
// ```
//  #undef  U_EQ
//  #define U_EQ(expectedString, rhsExpr) do {
//      const bsl::string& s = rhsExpr;
//      if (expectedString != s) {
//          cout << "Error:(" << __LINE__ << "): " <<
//                                          "\"" expectedString "\"" <<
//                       " != " << "(" #rhsExpr ": \"" << s << "\")" << endl;
//          assert(0);
//      }
//  } while (false)
// ```
// Declare a few variables of bdlt value types:
// ```
//  const bdlt::Time       TIME(14, 32, 17, 123, 456);
//  const bdlt::TimeTz     TIME_TZ(TIME, 300);
//  const bdlt::Date       DATE(1776, 7, 4);
//  const bdlt::Datetime   DATETIME(DATE, TIME);
//  const bdlt::DatetimeTz DATETIME_TZ(DATETIME, 0);
//
//  bsl::ostringstream     oss;
// ```
// The simplest way to format a bdlt type using `bsl::format` is to use the
// "{}" format string.  In this case, `bsl::format` will produce the same
// output as streaming that value into a `bsl::ostream`.  If more control is
// required, we can use '%'-sequences to select which parts of the value are
// formatted, in which order, and to add any text to separate them.
// ```
//  oss.str("");    oss << TIME;
//  U_EQ("14:32:17.123456",         oss.str());
//  U_EQ("14:32:17.123456",         bsl::format("{}", TIME));
//  U_EQ("14:32:17.123456",         bsl::format("{:%T}",         TIME));
//  U_EQ("14:32:17.123456 -- boo!", bsl::format("{:%T -- boo!}", TIME));
// ```
// A `Date` can be formatted with '%D':
// ```
//  oss.str("");    oss << DATE;
//  U_EQ("04JUL1776", oss.str());
//  U_EQ("04JUL1776", bsl::format("{}",    DATE));
//  U_EQ("04JUL1776", bsl::format("{:%D}", DATE));
// ```
// The `format` function can print multiple variables in a single call:
// ```
//  U_EQ("04JUL1776 .. 14:32:17.123456", bsl::format("{} .. {}", DATE, TIME));
// ```
// In bdlt formatting, any '%'-sequence that works on a type also works on
// other types that contain that type, and time zones can be formatted as `%z`.
// ```
//  oss.str("");    oss << TIME_TZ;
//  U_EQ("14:32:17.123456+0500", oss.str());
//  U_EQ("14:32:17.123456+0500", bsl::format("{}",      TIME_TZ));
//  U_EQ("14:32:17.123456+0500", bsl::format("{:%T%z}", TIME_TZ));
//
//  oss.str("");    oss << DATETIME;
//  U_EQ("04JUL1776_14:32:17.123456", oss.str());
//  U_EQ("04JUL1776_14:32:17.123456", bsl::format("{}",       DATETIME));
//  U_EQ("04JUL1776_14:32:17.123456", bsl::format("{:%D_%T}", DATETIME));
//
//  oss.str("");    oss << DATETIME_TZ;
//  U_EQ("04JUL1776_14:32:17.123456+0000", oss.str());
//  U_EQ("04JUL1776_14:32:17.123456+0000", bsl::format("{}",     DATETIME_TZ));
//  U_EQ("04JUL1776_14:32:17.123456+0000",
//                                     bsl::format("{:%D_%T%z}", DATETIME_TZ));
// ```
// '%'-sequences can appear any number of times:
// ```
//  U_EQ("04JUL1776 == 04JUL1776 == 04JUL1776!!!",
//                           bsl::format("{:%D == %D == %D!!!}", DATETIME_TZ));
// ```
// After the first ':' and before the first '%', if any, one can indicate
// padding with "<pad char><alignment char><width>" where:
// * <pad char> -- the character to pad with, (space if omitted)
// * <alignment char> -- '<' for left, '>' for right, '^' for centered
// * <width> -- minimum total width of the value output
// ```
//  U_EQ("14:32:17.123456          ", bsl::format("{:<25%T}",  TIME));
//  U_EQ("*****14:32:17.123456*****", bsl::format("{:*^25%T}", TIME));
//  U_EQ("++++++++++14:32:17.123456", bsl::format("{:+>25}",   TIME));
// ```
// After the padding specification (if any) and before the first '%' (if any),
// one can indicate the `precision`, the number of digits of fractional
// seconds.  This is specified by a '.' followed by a non-negative integral
// number of digits to be output.  If the given precision is 0, no decimal
// point will be shown:
// ```
//  U_EQ("14:32:17.12",              bsl::format("{:.2}",   TIME));
//  U_EQ("14:32:17.12",              bsl::format("{:.2%T}", TIME));
//  U_EQ("14:32:17",                 bsl::format("{:.0}",   TIME));
//  U_EQ("14:32:17.123456000000000", bsl::format("{:.15}",  TIME));
// ```
// After the padding and/or precision specifiers (if any), when formatting
// `bdlt` types, one can provide modifiers that change the behavior of the
// correlating bdlt '%'-sequences.  Currently, there is one modifier that
// affects the formatting of fractional seconds and three modifiers that affect
// time zone formatting:
// * ',' the decimal in `seconds` is to be a comma rather than a period
// * ':' always separate time zone hours and minutes by colon
// * '_' never separate time zone hours and minutes by colon
// * 'Z' if the time zone offset is zero, print it as 'Z', otherwise, the 'Z'
//   modifier is ignored
// ```
//  U_EQ("14:32:17.12",    bsl::format("{:.2%T}",  TIME_TZ));
//  U_EQ("14:32:17,12",    bsl::format("{:.2,%T}", TIME_TZ)); // comma
//  U_EQ("14:32:17+05:00", bsl::format("{:.0:}",   TIME_TZ)); // colon
//  U_EQ("14:32:17+0500",  bsl::format("{:.0}",    TIME_TZ)); // default tz
//  U_EQ("14:32:17+0500",  bsl::format("{:.0Z}",   TIME_TZ)); // 'Z' ignored
//
//  U_EQ("14:32:17+00:00", bsl::format("{:.0:%T%z}",  DATETIME_TZ)); // no 'Z'
//  U_EQ("14:32:17Z",      bsl::format("{:.0:Z%T%z}", DATETIME_TZ)); // 'Z'
// ```
// Errors in the format string such as:
// * using a '%'-sequence that is not supported for that type
// * Specifying padding, precision, and bdlt-modifiers in the wrong order
//   (among themselves, bdlt-modifiers may appear in any order)
// * Specifying bdlt-modifiers that are inapplicable to the type
// * any characters between the first ':' and the first '%'-sequence that are
//   not part of a valid padding specification, precision, or bdlt-modifier
// will cause a compilation error in C++20 or later, or cause a
// `bsl::format_error` exception to be thrown at runtime in C++17 or earlier.
//
// Padding width and precision can be passed through the argument list at run
// time:
// ```
//  U_EQ("+++14:32:17,12+++", bsl::format("{:+^{}.{},}", TIME, 17, 2));
// ```
// * '%i' - format the whole value according to the same format as
//   `bdlt::Iso8601::generate`.  Iso8601 formatting differs from formatting
//   the value with the "{}" format string in several ways:
//   - precision defaults to 3 rather than 6
//   - time zones default to having a ':' between hours and minutes, rather
//     than no separator
//   - `Date`s are done in the format "YYYY-MM-DD" rather than "DDMMMYYYY", so
//     that when sorted, they will sort chronologically
//   - the date and time in a 'Datetime' are separated by 'T' rather than '_'
//
// '%i'-sequences are supported for all bdlt value types:
// ```
//  U_EQ("14:32:17.123",                  bsl::format("{:%i}", TIME));
//  U_EQ("14:32:17.123+05:00",            bsl::format("{:%i}", TIME_TZ));
//  U_EQ("1776-07-04",                    bsl::format("{:%i}", DATE));
//  U_EQ("1776-07-04T14:32:17.123+00:00", bsl::format("{:%i}", DATETIME_TZ));
// ```
// The '_' bdlt-modifier can be used to suppress the colon in the time zone:
// ```
//  U_EQ("14:32:17.123+0500",             bsl::format("{:_%i}", TIME_TZ));
// ```
// `bsl::format` does not support showing names or abbreviations of time zones,
// they are always listed as numerical hours & minutes.
//
// Using various '%'-sequences, one can arrange different pieces of the value
// in any desired order.  For example, it is possible to replicate the "%i"
// format using other sequences:
// ```
//  U_EQ("1776-07-04T14:32:17.123+00:00", bsl::format("{:%i}", DATETIME_TZ));
//  U_EQ("1776-07-04T14:32:17.123+00:00",
//                                bsl::format("{:.3:%FT%T%z}", DATETIME_TZ));
//  U_EQ("1776-07-04T14:32:17.123+00:00",
//                    bsl::format("{:.3:%Y-%m-%dT%H:%M:%S%z}", DATETIME_TZ));
// ```
// Other '%'-sequences can be used to arrange the value in other ways, or to
// achieve specific output formats.  For example:
// ```
//  U_EQ("THU, JUL 04, 02:32:17 PM, 1776",
//                   bsl::format("{:.0%a, %b %d, %I:%M:%S %p, %Y}", DATETIME));
//
//  U_EQ("JUL  4 1776", bsl::format("{:%h %e %C%y}", DATE));
//
//  const bdlt::Date SUNDAY(1776, 6, 30);
//
//  U_EQ("Sunday %a: SUN, Sunday %u: 7, Sunday %w: 0", bsl::format(
//               "Sunday %a: {:%a, Sunday %%u: %u, Sunday %%w: %w}", SUNDAY));
//
//  U_EQ("Day of year Sunday: 182, Thursday: 186", bsl::format(
//               "Day of year Sunday: {:%j}, Thursday: {:%j}", SUNDAY, DATE));
// ```

#include <bdlscm_version.h>

#endif

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
