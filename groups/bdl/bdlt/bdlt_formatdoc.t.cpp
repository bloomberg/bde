// bdlt_formatdoc.t.cpp                                               -*-C++-*-
#include <bdlt_formatdoc.h>

#include <bdlt_datetz.h>
#include <bdlt_datetzformatter.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetimetzformatter.h>
#include <bdlt_timetz.h>
#include <bdlt_timetzformatter.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
// [ 1] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2; (void)             verbose;
    const bool         veryVerbose = argc > 3; (void)         veryVerbose;
    const bool     veryVeryVerbose = argc > 4; (void)     veryVeryVerbose;
    const bool veryVeryVeryVerbose = argc > 5; (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

//
///Usage
///-----
// First, create a form of assertion macro that will compare a literal string
// to the result of an expression and output the value of the expression if
// they don't match (continuations elided in .h to suppress compiler errors):
// ```
    #undef  U_EQ
    #define U_EQ(expectedString, rhsExpr) do {                                \
        const bsl::string& s = rhsExpr;                                       \
        if (expectedString != s) {                                            \
            cout << "Error:(" << __LINE__ << "): " <<                         \
                                            "\"" expectedString "\"" <<       \
                         " != " << "(" #rhsExpr ": \"" << s << "\")" << endl; \
            ASSERT(0);                                                        \
        }                                                                     \
    } while (false)
// ```
// Declare a few variables of bdlt value types:
// ```
    const bdlt::Time       TIME(14, 32, 17, 123, 456);
    const bdlt::TimeTz     TIME_TZ(TIME, 300);
    const bdlt::Date       DATE(1776, 7, 4);
    const bdlt::Datetime   DATETIME(DATE, TIME);
    const bdlt::DatetimeTz DATETIME_TZ(DATETIME, 0);
//
    bsl::ostringstream     oss;
// ```
// The simplest way to format a bdlt type using `bsl::format` is to use the
// "{}" format string.  In this case, `bsl::format` will produce the same
// output as streaming that value into a `bsl::ostream`.  If more control is
// required, we can use '%'-sequences to select which parts of the value are
// formatted, in which order, and to add any text to separate them.
// ```
    oss.str("");    oss << TIME;
    U_EQ("14:32:17.123456",         oss.str());
    U_EQ("14:32:17.123456",         bsl::format("{}", TIME));
    U_EQ("14:32:17.123456",         bsl::format("{:%T}",         TIME));
    U_EQ("14:32:17.123456 -- boo!", bsl::format("{:%T -- boo!}", TIME));
// ```
// A `Date` can be formatted with '%D':
// ```
    oss.str("");    oss << DATE;
    U_EQ("04JUL1776", oss.str());
    U_EQ("04JUL1776", bsl::format("{}",    DATE));
    U_EQ("04JUL1776", bsl::format("{:%D}", DATE));
// ```
// The `format` function can print multiple variables in a single call:
// ```
    U_EQ("04JUL1776 .. 14:32:17.123456", bsl::format("{} .. {}", DATE, TIME));
// ```
// In bdlt formatting, any '%'-sequence that works on a type also works on
// other types that contain that type, and time zones can be formatted as `%z`.
// ```
    oss.str("");    oss << TIME_TZ;
    U_EQ("14:32:17.123456+0500", oss.str());
    U_EQ("14:32:17.123456+0500", bsl::format("{}",      TIME_TZ));
    U_EQ("14:32:17.123456+0500", bsl::format("{:%T%z}", TIME_TZ));
//
    oss.str("");    oss << DATETIME;
    U_EQ("04JUL1776_14:32:17.123456", oss.str());
    U_EQ("04JUL1776_14:32:17.123456", bsl::format("{}",       DATETIME));
    U_EQ("04JUL1776_14:32:17.123456", bsl::format("{:%D_%T}", DATETIME));
//
    oss.str("");    oss << DATETIME_TZ;
    U_EQ("04JUL1776_14:32:17.123456+0000", oss.str());
    U_EQ("04JUL1776_14:32:17.123456+0000", bsl::format("{}",     DATETIME_TZ));
    U_EQ("04JUL1776_14:32:17.123456+0000",
                                       bsl::format("{:%D_%T%z}", DATETIME_TZ));
// ```
// '%'-sequences can appear any number of times:
// ```
    U_EQ("04JUL1776 == 04JUL1776 == 04JUL1776!!!",
                             bsl::format("{:%D == %D == %D!!!}", DATETIME_TZ));
// ```
// After the first ':' and before the first '%', if any, one can indicate
// padding with "<pad char><alignment char><width>" where:
// * <pad char> -- the character to pad with, (space if omitted)
// * <alignment char> -- '<' for left, '>' for right, '^' for centered
// * <width> -- minimum total width of the value output
// ```
    U_EQ("14:32:17.123456          ", bsl::format("{:<25%T}",  TIME));
    U_EQ("*****14:32:17.123456*****", bsl::format("{:*^25%T}", TIME));
    U_EQ("++++++++++14:32:17.123456", bsl::format("{:+>25}",   TIME));
// ```
// After the padding specification (if any) and before the first '%' (if any),
// one can indicate the `precision`, the number of digits of fractional
// sECONDS, between the first ':' and the first '%', if any: You can specify
// the `precision`, the number of digits of fractional seconds, between the
// first ':' and the first '%', if any:
// ```
    U_EQ("14:32:17.12",              bsl::format("{:.2}",   TIME));
    U_EQ("14:32:17.12",              bsl::format("{:.2%T}", TIME));
    U_EQ("14:32:17",                 bsl::format("{:.0}",   TIME));
    U_EQ("14:32:17.123456000000000", bsl::format("{:.15}",  TIME));
// ```
// After the padding and/or precision specifiers (if any), when formatting
// `bdlt` types, one can provide modifiers that change the behavior of the
// correlating '%'-sequences.  Currently, there is one modifier that affects
// the formatting of fractional seconds and three modifiers that affect time
// zone formatting:
// * ',' the decimal in `seconds` is to be a comma rather than a period
// * ':' always separate time zone hours and minutes by colon
// * '_' never separate time zone hours and minutes by colon
// * 'Z' if the time zone offset is zero, print it as 'Z', otherwise, the 'Z'
//   modifier is ignored
// ```
    U_EQ("14:32:17.12",    bsl::format("{:.2%T}",  TIME_TZ));
    U_EQ("14:32:17,12",    bsl::format("{:.2,%T}", TIME_TZ)); // comma
    U_EQ("14:32:17+05:00", bsl::format("{:.0:}",   TIME_TZ)); // colon
    U_EQ("14:32:17+0500",  bsl::format("{:.0}",    TIME_TZ)); // default tz
    U_EQ("14:32:17+0500",  bsl::format("{:.0Z}",   TIME_TZ)); // 'Z' ignored

    U_EQ("14:32:17+00:00", bsl::format("{:.0:%T%z}",  DATETIME_TZ)); // no 'Z'
    U_EQ("14:32:17Z",      bsl::format("{:.0:Z%T%z}", DATETIME_TZ)); // 'Z'
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
    U_EQ("+++14:32:17,12+++", bsl::format("{:+^{}.{},}", TIME, 17, 2));
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
    U_EQ("14:32:17.123",                  bsl::format("{:%i}", TIME));
    U_EQ("14:32:17.123+05:00",            bsl::format("{:%i}", TIME_TZ));
    U_EQ("1776-07-04",                    bsl::format("{:%i}", DATE));
    U_EQ("1776-07-04T14:32:17.123+00:00", bsl::format("{:%i}", DATETIME_TZ));
// ```
// The '_' bdlt-modifier can be used to suppress the colon in the time zone:
// ```
    U_EQ("14:32:17.123+0500",             bsl::format("{:_%i}", TIME_TZ));
// ```
// `bsl::format` does not support showing names or abbreviations of time zones,
// they are always listed as numerical hours & minutes.
//
// Using various '%'-sequences, one can arrange different pieces of the value
// in any desired order.  For example, it is possible to replicate the "%i"
// format using other sequences:
// ```
    U_EQ("1776-07-04T14:32:17.123+00:00", bsl::format("{:%i}", DATETIME_TZ));
    U_EQ("1776-07-04T14:32:17.123+00:00",
                                  bsl::format("{:.3:%FT%T%z}", DATETIME_TZ));
    U_EQ("1776-07-04T14:32:17.123+00:00",
                      bsl::format("{:.3:%Y-%m-%dT%H:%M:%S%z}", DATETIME_TZ));
// ```
// Other '%'-sequences can be used to arrange the value in other ways, or to
// achieve specific output formats.  For example:
// ```
    U_EQ("THU, JUL 04, 02:32:17 PM, 1776",
                     bsl::format("{:.0%a, %b %d, %I:%M:%S %p, %Y}", DATETIME));
//
    U_EQ("JUL  4 1776", bsl::format("{:%h %e %C%y}", DATE));
//
    const bdlt::Date SUNDAY(1776, 6, 30);
//
    U_EQ("Sunday %a: SUN, Sunday %u: 7, Sunday %w: 0", bsl::format(
                 "Sunday %a: {:%a, Sunday %%u: %u, Sunday %%w: %w}", SUNDAY));
//
    U_EQ("Day of year Sunday: 182, Thursday: 186", bsl::format(
                 "Day of year Sunday: {:%j}, Thursday: {:%j}", SUNDAY, DATE));
// ```
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

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
