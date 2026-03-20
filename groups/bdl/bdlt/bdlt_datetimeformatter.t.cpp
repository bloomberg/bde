// bdlt_datetimeformatter.t.cpp                                       -*-C++-*-
#include <bdlt_datetimeformatter.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_formattestutil.h>
#include <bdlt_time.h>

#include <bslim_testutil.h>

#include <bsl_cstdlib.h>     // `atoi`
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

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

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                             FORMAT TEST MACROS
// ----------------------------------------------------------------------------

#define U_TEST_FORMAT(exp, format, value) do {                                \
    bsl::string message;                                                      \
    bool rc = bdlt::FormatTestUtil::testFormat(&message, exp, format, value); \
    ASSERTV(message, rc);                                                     \
} while (false)

#define U_TEST_FORMAT_ARG(exp, format, value, arg) do {                       \
    bsl::string message;                                                      \
    bool rc = bdlt::FormatTestUtil::testFormatArg(                            \
                                         &message, exp, format, value, arg);  \
    ASSERTV(message, rc);                                                     \
} while (false)

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

namespace {
namespace u {

static const struct DData {
    int d_dline;
    int d_year;
    int d_month;
    int d_day;
} DDATA[] = {
    { L_,    1,  1,  1 },
    { L_,  700, 12,  6 },
    { L_, 1923,  5,  5 },
    { L_, 1926,  3, 23 },
    { L_, 1950,  4, 30 },
    { L_, 1951, 11,  4 },
    { L_, 1953, 10,  9 },
    { L_, 1960,  1, 16 },
    { L_, 1962,  9, 17 },
    { L_, 9996,  2, 29 },
    { L_, 9999, 12, 31 },
};
enum { k_NUM_DDATA = sizeof DDATA / sizeof *DDATA };

static const struct TData {
    int d_tline;
    int d_hour;
    int d_minute;
    int d_second;
    int d_milli;
    int d_micro;
} TDATA[] = {
    { L_,  0,  0,  0,   0,   0 },
    { L_, 23, 59, 59, 999, 999 },
    { L_, 24,  0,  0,   0,   0 },
    { L_, 12, 34, 56, 123, 456 },
    { L_,  6,  7,  8, 654, 321 },
};
enum { k_NUM_TDATA = sizeof TDATA / sizeof *TDATA };

const size_t npos = bsl::string::npos;

// ============================================================================
//                  HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

void trimPrecision(bsl::string *s)
{
    ASSERTV(*s, 2 <= s->length());
    ASSERTV(*s, npos != s->find('.'));

    size_t sz = s->length();
    if ('.' == (*s)[sz - 2]) {
        s->resize(sz - 2);
    }
    else {
        s->resize(sz - 1);
    }
}

void appendIso8601Date(bsl::string       *result,
                       const bdlt::Date&  date)
{
    *result += bsl::format("{:04}-{:02}-{:02}",
                           date.year(),
                           date.month(),
                           date.day());
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;

    (void) veryVerbose;
    (void) veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // TESTING `bsl::format`
        //
        // Concerns:
        // 1. By default, `bsl::format{"{}", ...)` produces output from a
        //    `Datetime` identical to that produced by `operator<<`.
        //
        // 2. All the percent sequences that print output from a `Datetime`
        //    produce the same output they would produce from the corresponding
        //    `Date` or `Time` objects, except for '%i', which will print the
        //    whole object with output equivalent to "{:%FT%T}" with a default
        //    precision of 3 rather than 6.
        //
        // 3. The `precision` argument works properly when printing entire
        //    `Datetime`s by default or using '%i'.
        //
        // 4. The `width` argument works properly when printing entire
        //    `Datetime`s by default or using '%i', we test left, right, and
        //    center padding.
        //
        // Plans:
        // 1. Use `U_TEST_FORMAT` and `U_TEST_FORMAT_ARG` to test that
        //    formatting an object with a given string produces a given result.
        //    Tests on `char` and `wchar_t` strings.
        //
        // 2. Have a table `DDATA[]` driving different `Date` values, and a
        //    table `TDATA[]` driving different `Time` values.  Have a loop
        //    iterating through `Date` values, and nest a loop iterating
        //    through `Time` values within that.
        //
        // 3. In the inner loop, create a `Datetime` value from the `Date` and
        //    `Time` values.
        //
        // 4. Verify that the default and '%i' output are as expected.
        //
        // 5. Verify using other format strings that produce output equivalent
        //    to default and '%i' output, thereby exercising other '%'
        //    sequences.
        //
        // 6. For default output:
        //    * Loop iterating through different values of `precision` to be
        //      passed and verify output is as expected.
        //    * Loop iterating through different values of `width` to be passed
        //      and verify output is as expected, for right, left, and center
        //      padding.
        //
        // 7. Repeat the steps of '6' for '%i' output.
        //
        // 8. Test the rest of the more obscure '%' sequences.
        //
        // Testing:
        //   bsl::string  bsl::format( "...", datetime);
        //   bsl::wstring bsl::format(L"...", datetime);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `bsl::format`\n"
                             "=====================\n";

        for (int td = 0; td < u::k_NUM_DDATA; ++td) {
            const u::DData& dData = u::DDATA[td];
            const int       DLINE = dData.d_dline;    (void) DLINE;
            const int       YEAR  = dData.d_year;
            const int       MONTH = dData.d_month;
            const int       DAY   = dData.d_day;

            const bdlt::Date date(YEAR, MONTH, DAY);

            if (veryVerbose) P(date);

            bsl::ostringstream oss;
            oss << date;

            const bsl::string nakedDate = oss.str();
            bsl::string  exps = nakedDate;   const bsl::string&  EXPS = exps;

            U_TEST_FORMAT(EXPS, "{}", date);
            U_TEST_FORMAT(EXPS, "{:%D}", date);

            exps = "% ";

            U_TEST_FORMAT(EXPS, "{:%% }", date);

            bsl::string fDate;
            u::appendIso8601Date(&fDate, date);
            exps = fDate;

            U_TEST_FORMAT(EXPS, "{:%i}", date);
            U_TEST_FORMAT(EXPS, "{:%F}", date);

            for (int tt = 0; tt < u::k_NUM_TDATA; ++tt) {
                const u::TData& tData  = u::TDATA[tt];
                const int       TLINE  = tData.d_tline;    (void) TLINE;
                const int       HOUR   = tData.d_hour;
                const int       MINUTE = tData.d_minute;
                const int       SECOND = tData.d_second;
                const int       MILLI  = tData.d_milli;
                const int       MICRO  = tData.d_micro;

                const bdlt::Time time(HOUR, MINUTE, SECOND, MILLI, MICRO);

                oss.str("");
                oss << time;
                const bsl::string nakedTime = oss.str();

                exps = nakedTime;

                U_TEST_FORMAT(EXPS, "{}", time);
                U_TEST_FORMAT(EXPS, "{:%T}", time);
                U_TEST_FORMAT(EXPS, "{:.6%i}", time);

                u::trimPrecision(&exps);
                u::trimPrecision(&exps);
                u::trimPrecision(&exps);
                const bsl::string iTime = exps;

                U_TEST_FORMAT(EXPS, "{:%i}", time);

                const bdlt::Datetime  datetime(date, time);
                const bdlt::Datetime& X = datetime;

                if (veryVeryVerbose) P(X);

                oss.str("");
                oss << X;
                const bsl::string nakedDatetime = oss.str();

                ASSERT(nakedDate + '_' + nakedTime == nakedDatetime);

                exps = nakedDatetime;

                U_TEST_FORMAT(EXPS, "{}", X);
                U_TEST_FORMAT(EXPS, "{:%D_%T}", X);

                U_TEST_FORMAT(EXPS, "{:%d%b%Y_%H:%M:%S}", X);
                U_TEST_FORMAT(EXPS, "{:%d%h%Y_%H:%M:%S}", X);
                U_TEST_FORMAT(EXPS, "{:%d%h%C%y_%H:%M:%S}", X);

                const bsl::string iDatetime = fDate + 'T' + iTime;

                exps = iDatetime;

                U_TEST_FORMAT(EXPS, "{:%i}", X);
                U_TEST_FORMAT(EXPS, "{:.3%FT%T}", X);
                U_TEST_FORMAT(EXPS, "{:.3%Y-%m-%dT%H:%M:%S}", X);
                U_TEST_FORMAT(EXPS, "{:.3%C%y-%m-%dT%H:%M:%S}", X);

                exps = nakedDatetime + "0000";
                int precision = 10;
                do {
                    u::trimPrecision(&exps);
                    --precision;

                    if (veryVeryVerbose) {
                        cout << "{:.{}}\t"; P_(precision); P(EXPS);
                    }

                    ASSERT((u::npos == EXPS.find('.')) == (0 == precision));

                    U_TEST_FORMAT_ARG(EXPS, "{:.{}}", X, precision);
                } while (0 < precision);

                size_t nakedWidth = nakedDatetime.length();

                for (int rightPad = -2; rightPad < 10; ++rightPad) {
                    exps = nakedDatetime;
                    if (0 < rightPad) {
                        exps.append(rightPad, '*');
                    }
                    U_TEST_FORMAT_ARG(EXPS,
                                      "{:*<{}}",
                                      X,
                                      nakedWidth + rightPad);
                }

                for (int leftPad = -2; leftPad < 10; ++leftPad) {
                    exps.clear();
                    if (0 < leftPad) {
                        exps.append(leftPad, '*');
                    }
                    exps += nakedDatetime;
                    U_TEST_FORMAT_ARG(EXPS,
                                      "{:*>{}}",
                                      X,
                                      nakedWidth + leftPad);
                }

                for (int centerPad = -2; centerPad < 10; ++centerPad) {
                    exps.clear();
                    if (0 < centerPad / 2) {
                        exps.append(centerPad / 2, '*');
                    }
                    exps += nakedDatetime;
                    int rightPad = centerPad - centerPad / 2;
                    if (0 < rightPad) {
                        exps.append(rightPad, '*');
                    }
                    U_TEST_FORMAT_ARG(EXPS,
                                      "{:*^{}}",
                                      X,
                                      nakedWidth + centerPad);
                }

                exps = fDate + 'T' + nakedTime + "0000";
                precision = 10;
                do {
                    u::trimPrecision(&exps);
                    --precision;

                    if (veryVeryVerbose) {
                        cout << "{:.{}%i}\t"; P_(precision); P(EXPS);
                    }

                    ASSERT((u::npos == EXPS.find('.')) == (0 == precision));

                    U_TEST_FORMAT_ARG(EXPS, "{:.{}%i}", X, precision);
                } while (0 < precision);

                nakedWidth = iDatetime.length();

                for (int rightPad = -2; rightPad < 10; ++rightPad) {
                    exps = iDatetime;
                    if (0 < rightPad) {
                        exps.append(rightPad, '*');
                    }
                    U_TEST_FORMAT_ARG(EXPS,
                                      "{:*<{}%i}",
                                      X,
                                      nakedWidth + rightPad);
                }

                for (int leftPad = -2; leftPad < 10; ++leftPad) {
                    exps.clear();
                    if (0 < leftPad) {
                        exps.append(leftPad, '*');
                    }
                    exps += iDatetime;
                    U_TEST_FORMAT_ARG(EXPS,
                                      "{:*>{}%i}",
                                      X,
                                      nakedWidth + leftPad);
                }

                for (int centerPad = -2; centerPad < 10; ++centerPad) {
                    exps.clear();
                    if (0 < centerPad / 2) {
                        exps.append(centerPad / 2, '*');
                    }
                    exps += iDatetime;
                    int rightPad = centerPad - centerPad / 2;
                    if (0 < rightPad) {
                        exps.append(rightPad, '*');
                    }
                    U_TEST_FORMAT_ARG(EXPS,
                                      "{:*^{}%i}",
                                      X,
                                      nakedWidth + centerPad);
                }

                oss.str("");
                {
                    const size_t width = oss.width();
                    const char   fill  = oss.fill();
                    oss.width(3);
                    oss.fill('0');
                    oss << date.dayOfYear();
                    oss.width(width);
                    oss.fill(fill);
                }
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%j}", X);    // 3-digit day of year

                oss.str("");
                oss << date.dayOfWeek();            // 3-letter weekday abbrev
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%a}", X);

                int dow = date.dayOfWeek() - 1;     // Sunday is 0
                oss.str("");
                oss << dow;
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%w}", X);

                dow = (dow + 7 - 1) % 7 + 1;        // Monday is 1
                oss.str("");
                oss << dow;
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%u}", X);

                oss.str("");
                {
                    const size_t width = oss.width();
                    const char   fill  = oss.fill();
                    oss.width(2);
                    oss.fill('0');
                    oss << (date.year() % 100);     // 2-digit year
                    oss.width(width);
                    oss.fill(fill);
                }
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%y}", X);

                oss.str("");
                {
                    const size_t width = oss.width();
                    const char   fill  = oss.fill();
                    oss.width(2);
                    oss.fill('0');
                    oss << (date.year() / 100);     // 2-digit century
                    oss.width(width);
                    oss.fill(fill);
                }
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%C}", X);

                oss.str("");
                oss << date.monthOfYear();          // 3-char month abbrev
                oss << ',';
                oss << date.monthOfYear();          // 3-char month abbrev
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%b,%h}", X);
            }
        }
#undef U_TEST_FORMAT
#undef U_TEST_FORMAT_ARG
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
