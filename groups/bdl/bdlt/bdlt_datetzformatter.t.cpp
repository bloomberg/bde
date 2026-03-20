// bdlt_datetzformatter.t.cpp                                         -*-C++-*-
#include <bdlt_datetzformatter.h>

#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_formattestutil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>     // `atoi`
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
using bsl::size_t;
using bsl::string;
using bsl::wstring;

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
#define PH_(exp)     (cout << (#exp ": 0x") << bsl::hex << (exp) <<           \
                                                     bsl::dec << "\t" << flush)

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

static const struct DData {
    int d_line;    // line number
    int d_year;    // year of date
    int d_month;   // month of date
    int d_day;     // day of month of date
    int d_offset;  // timezone offset
} DDATA[] = {
    //LINE  YEAR  MO  DAY  OFFSET
    //----  ----  --  ---  ------
    { L_,     1,  1,   1,       0 },
    { L_,     1,  1,   2,       0 },
    { L_,     1,  2,   1,       0 },
    { L_,     2,  1,   1,    1439 },
    { L_,     1,  1,   1,      -1 },
    { L_,     1,  1,   2,       1 },
    { L_,     1,  2,   1,   -1439 },
    { L_,     2,  1,   1,    1438 },

    { L_,  9998, 12,  31,       0 },
    { L_,  9999, 11,  30,       0 },
    { L_,  9999, 12,  30,       0 },
    { L_,  9999, 12,  31,       0 },
    { L_,  9998, 12,  31,       1 },
    { L_,  9999, 11,  30,   -1439 },
    { L_,  9999, 12,  30,      -1 },
    { L_,  9999, 12,  31,    -143 },
};
const int k_NUM_DDATA = sizeof DDATA / sizeof *DDATA;

const size_t npos = bsl::string::npos;

// ============================================================================
//                       GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

void appendIso8601Date(bsl::string       *result,
                       const bdlt::Date&  date)
{
    *result += bsl::format("{:04}-{:02}-{:02}",
                                        date.year(), date.month(), date.day());
}

void appendIso8601Offset(bsl::string *result,
                         int          offset)
{
    ASSERT(offset <=  1440);
    ASSERT(offset >= -1440);

    const char sign = offset < 0 ? '-' : '+';
    offset = bsl::abs(offset);

    *result += sign;
    *result += bsl::format("{:02}:{:02}", offset / 60, offset % 60);
}

void setIso8601DateTz(bsl::string         *result,
                      const bdlt::DateTz&  dateTz)
{
    result->clear();
    appendIso8601Date(  result, dateTz.localDate());
    appendIso8601Offset(result, dateTz.offset());
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
        // TESTING: `bsl::format` on `bdlt::DateTz`.
        //
        // Concerns:
        // 1. All the percent sequences that print output from a `Date`
        //    produce correct output when called on a `DatetimeTz`.
        //
        // 2. The `width` argument works properly when printing
        //    `DatetimeTz.date()`s by default or using '%D', we test left,
        //    right, and center padding.
        //
        // 3. The options in the format string that affect the time zone work
        //    properly.
        //
        // Plans:
        // 1. Use `U_TEST_FORMAT` and `U_TEST_FORMAT_ARG` to test that
        //    formatting an object with a given string produces a given result.
        //    Tests on `char` and `wchar_t` strings.
        //
        // 2. Have a table `DDATA[]` driving different `Date` values.  Loop
        //   v through the values, initializing `Date`s.
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
        //    * Loop iterating through different values of `width` to be passed
        //      and verify output is as expected, for right, left, and center
        //      padding.
        //
        // 7. Repeat the steps of '6' for '%i' output.
        //
        // 8. Nest another loop counting over the range `[ 0 .. 32 )` to
        //    drive all combinations of 4 bools.
        //
        //    1. `ISO` -- "%i" -- output is to be Iso8601 format.
        //
        //    2. `COLON` -- ":" -- there is to be a ':' separating the hours
        //       and minutes of the time zone.
        //
        //    3. `NO_COLON` -- "_"  -- there is to be no ':' separating the
        //       hours and minutes of the time zone.
        //
        //    4. `Z` -- "Z" -- if the offset is 0, the time zone is to be
        //       displayed as "Z".  If the offset is not 0, this has no effect.
        //
        // 5. Use the 4 `bool`s to drive mofication of the format string.
        //
        // 6. Start with the strings created in step P-3 and use the 4 bools to
        //    modify them into the expected output for format string.
        //
        // 7. Call `bsl::vformat_to` to output `X` with the format created,
        //    and check if it matches the expected output.  Repeat this with
        //    wide strings.
        //
        // Testing:
        //   bsl::string  bsl::format( "...", datetime);
        //   bsl::wstring bsl::format(L"...", datetime);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: `bsl::format` on `bdlt::DateTz`.\n"
                             "=========================================\n";

        bslma::TestAllocator da;
        bslma::DefaultAllocatorGuard defaultAllocatorGuard(&da);

        for (int td = 0; td < k_NUM_DDATA; ++td) {
            const DData& dData  = DDATA[td];
            const int    DLINE  = dData.d_line;    (void) DLINE;
            const int    YEAR   = dData.d_year;
            const int    MONTH  = dData.d_month;
            const int    DAY    = dData.d_day;
            const int    OFFSET = dData.d_offset;

            const bdlt::Date    date(YEAR, MONTH, DAY);
            const bdlt::DateTz  X(date, OFFSET);

            if (veryVerbose) P(X);

            bsl::ostringstream oss;
            oss << X;
            const string nakedObj = oss.str();

            string nakedIso8601;
            u::setIso8601DateTz(&nakedIso8601, X);

            string   exps;    const string&  EXPS = exps;
            string   ress;

            exps = nakedObj;

            if (veryVerbose) cout << "Date Part\n";

            U_TEST_FORMAT(EXPS, "{}", X);
            U_TEST_FORMAT(EXPS, "{:%D%z}", X);
            U_TEST_FORMAT(EXPS, "{:%d%b%Y%z}", X);
            U_TEST_FORMAT(EXPS, "{:%d%h%C%y%z}", X);

            exps = "% ";

            U_TEST_FORMAT(EXPS, "{:%% }", X);

            exps = nakedIso8601;

            U_TEST_FORMAT(EXPS, "{:%i}", X);
            U_TEST_FORMAT(EXPS, "{::%F%z}", X);
            U_TEST_FORMAT(EXPS, "{::%Y-%m-%d%z}", X);

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
            oss << date.dayOfWeek();// 3-letter weekday abbrev
            exps = oss.str();

            U_TEST_FORMAT(EXPS, "{:%a}", X);

            int dow = date.dayOfWeek() - 1;     // Sunday is 0
            oss.str("");
            oss << dow;
            exps = oss.str();

            U_TEST_FORMAT(EXPS, "{:%w}", X);     // index of weekday

            dow = (dow + 7 - 1) % 7 + 1;        // Monday is 1
            oss.str("");
            oss << dow;
            exps = oss.str();

            U_TEST_FORMAT(EXPS, "{:%u}", X);     // index of weekday

            oss.str("");
            {
                const size_t width = oss.width();
                const char   fill  = oss.fill();
                oss.width(2);
                oss.fill('0');
                oss << (date.year() % 100);    // 2-digit year
                oss.width(width);
                oss.fill(fill);
            }
            exps = oss.str();

            U_TEST_FORMAT(EXPS, "{:%y}", X);     // index of weekday

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

            U_TEST_FORMAT(EXPS, "{:%C}", X);     // index of weekday

            oss.str("");
            oss << date.monthOfYear();          // 3-char month abbrev
            oss << ',';
            oss << date.monthOfYear();          // 3-char month abbrev
            exps = oss.str();

            U_TEST_FORMAT(EXPS, "{:%b,%h}", X);     // 2 3-char month

            size_t nakedWidth = nakedObj.length();

            for (int rightPad = -2; rightPad < 10; ++rightPad) {
                exps = nakedObj;
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
                exps += nakedObj;
                U_TEST_FORMAT_ARG(EXPS, "{:*>{}}", X, nakedWidth + leftPad);
            }

            for (int centerPad = -2; centerPad < 10; ++centerPad) {
                exps.clear();
                if (0 < centerPad / 2) {
                    exps.append(centerPad / 2, '*');
                }
                exps += nakedObj;
                int rightPad = centerPad - centerPad / 2;
                if (0 < rightPad) {
                    exps.append(rightPad, '*');
                }
                U_TEST_FORMAT_ARG(EXPS, "{:*^{}}", X, nakedWidth+ centerPad);
            }

            nakedWidth = nakedIso8601.length();

            for (int rightPad = -2; rightPad < 10; ++rightPad) {
                exps = nakedIso8601;
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
                exps += nakedIso8601;
                U_TEST_FORMAT_ARG(EXPS, "{:*>{}%i}", X, nakedWidth + leftPad);
            }

            for (int centerPad = -2; centerPad < 10; ++centerPad) {
                exps.clear();
                if (0 < centerPad / 2) {
                    exps.append(centerPad / 2, '*');
                }
                exps += nakedIso8601;
                int rightPad = centerPad - centerPad / 2;
                if (0 < rightPad) {
                    exps.append(rightPad, '*');
                }
                U_TEST_FORMAT_ARG(EXPS, "{:*^{}%i}", X, nakedWidth +centerPad);
            }

            for (unsigned flags = 0; flags < 16; ++flags) {
                const unsigned FLAGS    = flags;
                const bool     ISO      = flags &  1;
                const bool     COLON    = flags &  2;
                const bool     NO_COLON = flags &  4;
                const bool     Z        = flags &  8;

                if (COLON & NO_COLON) {
                    continue;
                }

                string fmts = "{:}";
                if (ISO) {
                    fmts.insert(2, "%i");
                }
                if (COLON) {
                    fmts.insert(2, ":");
                }
                if (NO_COLON) {
                    fmts.insert(2, "_");
                }
                if (Z) {
                    fmts.insert(2, "Z");
                }
                const char    *FMTS = fmts.c_str();

                exps = ISO ? nakedIso8601 : nakedObj;
                if (!ISO && COLON) {
                    exps.insert(exps.length() - 2, ":");
                }
                if (ISO && NO_COLON) {
                    size_t idx = exps.rfind(':');
                    ASSERT(npos != idx);
                    exps.erase(idx, 1);
                }
                if (Z && 0 == OFFSET) {
                    size_t idx = exps.find_last_of("+-");;
                    ASSERT(npos != idx);
                    exps.resize(idx);
                    exps += 'Z';
                }

                if (veryVerbose) {
                    P_(X);    PH_(FLAGS);    P_(FMTS);    P(EXPS);
                }

                U_TEST_FORMAT(EXPS, FMTS, X);
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
