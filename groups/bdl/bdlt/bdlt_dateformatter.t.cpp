// bdlt_dateformatter.t.cpp                                           -*-C++-*-
#include <bdlt_dateformatter.h>

#include <bdlt_date.h>
#include <bdlt_formattestutil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>     // `atoi`
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

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

struct DefaultDataRow {
    int d_line;   // source line number
    int d_year;   // year under test
    int d_month;  // month under test
    int d_day;    // day under test
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE   YEAR   MONTH   DAY
    //----   ----   -----   ---
    { L_,       1,      1,    1 },
    { L_,       1,      1,    2 },
    { L_,       1,      2,    1 },
    { L_,       2,      1,    1 },
    { L_,      10,      4,    5 },
    { L_,      10,     10,   11 },
    { L_,     100,      6,    7 },
    { L_,     100,     11,   12 },
    { L_,    1000,      8,    9 },
    { L_,    1100,      1,   31 },
    { L_,    1200,      2,   29 },
    { L_,    1300,      3,   31 },
    { L_,    1400,      4,   30 },
    { L_,    1500,      5,   31 },
    { L_,    1600,      6,   30 },
    { L_,    1700,      7,   31 },
    { L_,    1800,      8,   31 },
    { L_,    1900,      9,   30 },
    { L_,    2000,     10,   31 },
    { L_,    2100,     11,   30 },
    { L_,    2200,     12,   31 },
    { L_,    9999,      1,    1 },
    { L_,    9999,      2,   28 },
    { L_,    9999,     11,   30 },
    { L_,    9999,     12,   30 },
    { L_,    9999,     12,   31 },
};
const int DEFAULT_NUM_DATA =
                  static_cast<int>(sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA);

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;

    (void) verbose;
    (void) veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // TESTING: `bsl::format`
        //
        // Concerns:
        // 1. By default, `bsl::format{"{}", ...)` produces output from a
        //    `Date` identical to that produced by `operator<<`.
        //
        // 2. All the percent sequences that print output from a `Date`
        //    produce correct output.
        //
        // 3. The `precision` argument works properly when printing entire
        //    `Date`s by default or using '%i'.
        //
        // 4. The `width` argument works properly when printing entire `Date`s
        //    by default or using '%i', we test left, right, and center
        //    padding.
        //
        // Plans:
        // 1. Use `U_TEST_FORMAT` and `U_TEST_FORMAT_ARG` to test that
        //    formatting an object with a given string produces a given result.
        //    Tests on `char` and `wchar_t` strings.
        //
        // 2. Have a table `DEFAULT_DATA[]` driving different `Date` values.
        //    Loop through the values, initializing `Date`s.
        //
        // 3. Verify that the default and '%i' output are as expected.
        //
        // 4. Verify using other format strings that produce output equivalent
        //    to default and '%i' output, thereby exercising other '%'
        //    sequences.
        //
        // 5. Test the rest of the more obscure '%' sequences.
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
        // Testing:
        //   bsl::string  bsl::format( "...", datetime);
        //   bsl::wstring bsl::format(L"...", datetime);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: `bsl::format`\n"
                             "======================\n";

        bslma::TestAllocator da;
        bslma::DefaultAllocatorGuard defaultAllocatorGuard(&da);

        for (int td = 0; td < DEFAULT_NUM_DATA; ++td) {
            const DefaultDataRow& dData = DEFAULT_DATA[td];
            const int             DLINE = dData.d_line;    (void) DLINE;
            const int             YEAR  = dData.d_year;
            const int             MONTH = dData.d_month;
            const int             DAY   = dData.d_day;

            const bdlt::Date  X(YEAR, MONTH, DAY);

            if (veryVerbose) P(X);

            bsl::ostringstream oss;
            oss << X;

            const bsl::string nakedDate = oss.str();
            bsl::string  exps = nakedDate;   const bsl::string&  EXPS = exps;

            U_TEST_FORMAT(EXPS, "{}", X);
            U_TEST_FORMAT(EXPS, "{:%D}", X);
            U_TEST_FORMAT(EXPS, "{:%d%b%Y}", X);
            U_TEST_FORMAT(EXPS, "{:%d%h%C%y}", X);

            exps = "% ";

            U_TEST_FORMAT(EXPS, "{:%% }", X);

            const bsl::string& fDate = bsl::format("{:04}-{:02}-{:02}",
                                                             YEAR, MONTH, DAY);
            exps = fDate;

            U_TEST_FORMAT(EXPS, "{:%i}", X);
            U_TEST_FORMAT(EXPS, "{:%F}", X);
            U_TEST_FORMAT(EXPS, "{:%Y-%m-%d}", X);

            oss.str("");
            {
                const size_t width = oss.width();
                const char   fill  = oss.fill();
                oss.width(3);
                oss.fill('0');
                oss << X.dayOfYear();
                oss.width(width);
                oss.fill(fill);
            }
            exps = oss.str();

            U_TEST_FORMAT(EXPS, "{:%j}", X);    // 3-digit day of year

            oss.str("");
            oss << X.dayOfWeek();            // 3-letter weekday abbrev
            exps = oss.str();

            U_TEST_FORMAT(EXPS, "{:%a}", X);

            int dow = X.dayOfWeek() - 1;     // Sunday is 0
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
                oss << (X.year() % 100);     // 2-digit year
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
                oss << (X.year() / 100);     // 2-digit century
                oss.width(width);
                oss.fill(fill);
            }
            exps = oss.str();

            U_TEST_FORMAT(EXPS, "{:%C}", X);

            oss.str("");
            oss << X.monthOfYear();          // 3-char month abbrev
            oss << ',';
            oss << X.monthOfYear();          // 3-char month abbrev
            exps = oss.str();

            U_TEST_FORMAT(EXPS, "{:%b,%h}", X);

            size_t nakedWidth = nakedDate.length();

            for (int rightPad = -2; rightPad < 10; ++rightPad) {
                exps = nakedDate;
                if (0 < rightPad) {
                    exps.append(rightPad, '*');
                }
                U_TEST_FORMAT_ARG(EXPS, "{:*<{}}", X, nakedWidth + rightPad);
            }

            for (int leftPad = -2; leftPad < 10; ++leftPad) {
                exps.clear();
                if (0 < leftPad) {
                    exps.append(leftPad, '*');
                }
                exps += nakedDate;
                U_TEST_FORMAT_ARG(EXPS, "{:*>{}}", X, nakedWidth + leftPad);
            }

            for (int centerPad = -2; centerPad < 10; ++centerPad) {
                exps.clear();
                if (0 < centerPad / 2) {
                    exps.append(centerPad / 2, '*');
                }
                exps += nakedDate;
                int rightPad = centerPad - centerPad / 2;
                if (0 < rightPad) {
                    exps.append(rightPad, '*');
                }
                U_TEST_FORMAT_ARG(EXPS, "{:*^{}}", X, nakedWidth + centerPad);
            }

            nakedWidth = fDate.length();

            for (int rightPad = -2; rightPad < 10; ++rightPad) {
                exps = fDate;
                if (0 < rightPad) {
                    exps.append(rightPad, '*');
                }
                U_TEST_FORMAT_ARG(EXPS, "{:*<{}%i}", X, nakedWidth + rightPad);
            }

            for (int leftPad = -2; leftPad < 10; ++leftPad) {
                exps.clear();
                if (0 < leftPad) {
                    exps.append(leftPad, '*');
                }
                exps += fDate;
                U_TEST_FORMAT_ARG(EXPS, "{:*>{}%i}", X, nakedWidth + leftPad);
            }

            for (int centerPad = -2; centerPad < 10; ++centerPad) {
                exps.clear();
                if (0 < centerPad / 2) {
                    exps.append(centerPad / 2, '*');
                }
                exps += fDate;
                int rightPad = centerPad - centerPad / 2;
                if (0 < rightPad) {
                    exps.append(rightPad, '*');
                }
                U_TEST_FORMAT_ARG(EXPS, "{:*^{}%i}", X, nakedWidth+centerPad);
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
