// bdlt_timeformatter.t.cpp                                           -*-C++-*-
#include <bdlt_timeformatter.h>

#include <bdlt_formattestutil.h>
#include <bdlt_time.h>

#include <bslim_testutil.h>

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
//                       GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

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

void setIso8601Time(bsl::string       *result,
                    const bdlt::Time&  time)
{
    bsl::format_to(result, "{:02}:{:02}:{:02}.{:03}",
                time.hour(), time.minute(), time.second(), time.millisecond());
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

    (void) veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // TEST `bsl::format`
        //
        // Concerns:
        // 1. The default "{}" output of a `Time` is the same as if output by
        //    `operator<<`.  The "{:%T}" output is the same, as is the output
        //    by "{:%H:%M:%S}".
        //
        // 2. The default output of "{:%i}" is the same as "{:%T}", the only
        //    difference being that the default precision of %i is 3 while that
        //    of %T is 6.
        //
        // 3. Check output with varying precision.
        //
        // 4. Check output with varying widths and padding (left, right, and
        //    center padding).
        //
        // Plan:
        // 1. Use the `TDATA` table to drive different values of `Time`.
        //
        // 2. Use a `ostringstream` to gather the output using `operator<<` for
        //    modeling expected output, assigned to an `exps` string variable
        //    to be compared to `bsl::format` output to assess correctness.
        //    Throughout the test, we further edit `exps` to be the expected
        //    result of each test.
        //
        // 3. Use `U_TEST_FORMAT` and `U_TEST_FORMAT_ARG` to test that
        //    formatting an object with a given string produces a given result.
        //    Tests on `char` and `wchar_t` strings.
        //
        // 4. Have a `trimPrecision` function that will trim one digit of
        //    precision (and the decimal point when it trims the last digit).
        //    Iterate reducing the precision of the expected result and passing
        //    the reducing precision to `bsl::format`.
        //
        // 5. Iterate with width values passing to `bsl::format` varying from
        //    below the basic width of the output to several characters wider.
        //    To this for left, right, and center padding.
        //
        // Testing:
        //   string  bsl::format( "...", ...);
        //   wstring bsl::format(L"...", ...);
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST `bsl::format`\n"
                             "==================\n";

        for (int ti = 0; ti < k_NUM_TDATA; ++ti) {
            const TData& tData  = TDATA[ti];
            const int    TLINE  = tData.d_tline;    (void) TLINE;
            const int    HOUR   = tData.d_hour;
            const int    MINUTE = tData.d_minute;
            const int    SECOND = tData.d_second;
            const int    MILLI  = tData.d_milli;
            const int    MICRO  = tData.d_micro;

            bdlt::Time mX(HOUR, MINUTE, SECOND, MILLI, MICRO);
            const bdlt::Time& X = mX;

            bsl::ostringstream oss;
            oss << X;
            const bsl::string nakedObj = oss.str();

            bsl::string s;
            u::setIso8601Time(&s, X);
            const bsl::string nakedIso8601 = s;

            bsl::string  exps = nakedObj;      const bsl::string&  EXPS = exps;
            size_t nakedWidth = EXPS.length();

            U_TEST_FORMAT(EXPS, "{}", X);
            U_TEST_FORMAT(EXPS, "{:%T}", X);
            U_TEST_FORMAT(EXPS, "{:%H:%M:%S}", X);
            U_TEST_FORMAT(EXPS, "{:.6%i}", X);
            U_TEST_FORMAT(EXPS, "{:15.6%T}", X);

            exps = "% ";

            U_TEST_FORMAT(EXPS, "{:%% }", X);

            exps = nakedIso8601;

            U_TEST_FORMAT(EXPS, "{:%i}", X);
            U_TEST_FORMAT(EXPS, "{:.3%T}", X);

            {
                char *pc = &exps[0] + EXPS.find('.');
                *pc = ',';
            }
            exps.insert(0, "%\n\t");

            U_TEST_FORMAT(EXPS, "{:.3,%%%n%t%T}", X);

            size_t precision = 10;
            exps = oss.str() + "0000";
            do {
                u::trimPrecision(&exps);
                --precision;

                if (verbose) { cout << "{:.{}%T}\t"; P_(precision); P(EXPS); }

                ASSERT((npos == EXPS.find('.')) == (0 == precision));

                U_TEST_FORMAT_ARG(EXPS, "{:.{}%T}", X, precision);
                U_TEST_FORMAT_ARG(EXPS, "{:.{}%i}", X, precision);
            } while (0 < precision);

            for (int rightPad = -2; rightPad < 10; ++rightPad) {
                exps = oss.str();
                if (0 < rightPad) {
                    exps.append(rightPad, '*');
                }
                U_TEST_FORMAT_ARG(EXPS, "{:*<{}%T}", X, nakedWidth + rightPad);
            }

            for (int leftPad = -2; leftPad < 10; ++leftPad) {
                exps.clear();
                if (0 < leftPad) {
                    exps.append(leftPad, '*');
                }
                exps += oss.str();
                U_TEST_FORMAT_ARG(EXPS, "{:*>{}%T}", X, nakedWidth + leftPad);
            }

            for (int centerPad = -2; centerPad < 10; ++centerPad) {
                exps.clear();
                if (0 < centerPad / 2) {
                    exps.append(centerPad / 2, '*');
                }
                exps += oss.str();
                int rightPad = centerPad - centerPad / 2;
                if (0 < rightPad) {
                    exps.append(rightPad, '*');
                }
                U_TEST_FORMAT_ARG(EXPS, "{:*^{}%T}", X, nakedWidth + centerPad);
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
