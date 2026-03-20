// bdlt_timetzformatter.t.cpp                                         -*-C++-*-
#include <bdlt_timetzformatter.h>

#include <bdlt_formattestutil.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>     // `atoi`
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

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

typedef bdlt::FormatTestUtil TU;

namespace {
namespace u {

const size_t npos = string::npos;

void trimPrecision(bsl::string *s)
{
    ASSERTV(*s, 2 <= s->length());
    ASSERTV(*s, npos != s->find_first_of(".,"));

    size_t idx = s->find_last_of("+-");
    if ('.' == (*s)[idx - 2]) {
        s->erase(idx - 2, 2);
    }
    else {
        ASSERT(npos != s->find('.'));
        s->erase(idx - 1, 1);
    }
}

void appendIso8601Time(bsl::string       *result,
                       const bdlt::Time&  time)
{
    *result += bsl::format("{:02}:{:02}:{:02}.{:03}",
                time.hour(), time.minute(), time.second(), time.millisecond());
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

void setIso8601TimeTz(bsl::string         *result,
                      const bdlt::TimeTz&  timeTz)
{
    result->clear();
    appendIso8601Time(  result, timeTz.localTime());
    appendIso8601Offset(result, timeTz.offset());
}

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
        // TESTING: `bsl::format`
        //
        // Concerns:
        // 1. Test all the percent sequences that print output from a `TimeTz`.
        //
        // 3. The `width` argument works properly when formatting
        //    `DatetimeTz.date()`s by default or using '%D', we test left,
        //    right, and center padding.
        //
        // Plans:
        // 1. Use `U_TEST_FORMAT` and `U_TEST_FORMAT_ARG` to test that
        //    formatting an object with a given string produces a given result.
        //    Tests on `char` and `wchar_t` strings.
        //
        // 2. Have a table `TDATA[]` driving different `Time` values and a
        //    table `OFFSETS` having different tize zone offsets.  Loop through
        //   the values, initializing `TimeTz`s.
        //
        // 3. Use `operator<<` and `setIso8601TimeTz` to determine the expected
        //    output from `format("{}", ...` and `format("{:%i}", ...`.
        //
        // 4. Call `bsl::format` and verify the output matches what's expected.
        //
        // 5. Call `bsl::format` with all the `%` sequences that output fields
        //    from `bdlt::Time` and observe that they all yield correct
        //    results.
        //
        // 6. Nest another loop counting over the range `[ 0 .. 32 )` to
        //    drive all combinations of 5 bools.
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
        //    5. `COMMA` -- the decimal point in the seconds field is to be
        //       printed as ',' rather than '.'.
        //
        // 7. Use the 5 `bool`s to drive mofication of the format string.
        //
        // 8. Start with the strings created in step P-3 and use the 5 bools to
        //    modify them into the expected output for format string.
        //
        // 9. Call `bsl::vformat_to` to output `X` with the format created,
        //    and check if it matches the expected output.  Repeat this with
        //    wide strings.
        //
        // Testing:
        //   bsl::string  bsl::format( "...", datetime);
        //   bsl::wstring bsl::format(L"...", datetime);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: `bsl::format`\n"
                             "======================\n";

        bslma::TestAllocator da;
        bslma::DefaultAllocatorGuard defaultAllocatorGuard(&da);

        static const int OFFSETS[] = { 0, -1, 1, 1439, -1439, 60, -60 };
        enum { k_NUM_OFFSETS    = sizeof OFFSETS / sizeof *OFFSETS,
               k_NUM_ITERATIONS = u::k_NUM_TDATA * k_NUM_OFFSETS,
               k_NUM_FLAGS      = 1 << 5 };

        for (unsigned uu = 0; uu < k_NUM_ITERATIONS; ++uu) {
            const unsigned  ut      = uu / k_NUM_OFFSETS;
            const unsigned  uo      = uu % k_NUM_OFFSETS;

            const u::TData& tData   = u::TDATA[ut];
            const int       TLINE   = tData.d_tline;    (void) TLINE;
            const int       HOUR    = tData.d_hour;
            const int       MINUTE  = tData.d_minute;
            const int       SECOND  = tData.d_second;
            const int       MILLI   = tData.d_milli;
            const int       MICRO   = tData.d_micro;

            const int       OFFSET  = OFFSETS[uo];

            const bdlt::Time TIME(HOUR, MINUTE, SECOND, MILLI, MICRO);

            if (HOUR == 24 && 0 != OFFSET) {
                continue;
            }

            const bdlt::TimeTz X(TIME, OFFSET);

            if (veryVerbose) P(X);

            bsl::ostringstream oss;
            oss << X;
            const string   nakedObj = oss.str();

            string nakedIso8601;
            u::setIso8601TimeTz(&nakedIso8601, X);

            string         exps = nakedObj;
            const string&  EXPS = exps;

            size_t nakedWidth = nakedObj.length();

            U_TEST_FORMAT(EXPS, "{}", X);
            U_TEST_FORMAT(EXPS, "{:%T%z}", X);
            U_TEST_FORMAT(EXPS, "{:%H:%M:%S%z}", X);
            U_TEST_FORMAT(EXPS, "{:.6_%i}", X);
            U_TEST_FORMAT(EXPS, "{:15.6%T%z}", X);

            {    // default %i precision is 3 not 6
                size_t idx = exps.find_last_of("+-");
                ASSERT(u::npos != idx);
                exps.erase(idx - 3, 3);
            }

            U_TEST_FORMAT(EXPS, "{:_%i}", X);
            U_TEST_FORMAT(EXPS, "{:.3%T%z}", X);

            {
                char *pc = &exps[0] + EXPS.find('.');
                *pc = ',';
            }
            bsl::string nakedObj_3 = exps;

            exps = "%\n\t ";

            U_TEST_FORMAT(EXPS, "{:.3,%%%n%t }", X);

            exps += nakedObj_3;

            U_TEST_FORMAT(EXPS, "{:.3,%%%n%t %T%z}", X);

            size_t precision = 10;
            exps = nakedObj;
            exps.insert(exps.find_last_of("+-"), "0000");
            do {
                u::trimPrecision(&exps);
                --precision;

                ASSERT((u::npos == EXPS.find_first_of('.')) ==
                                                         (0 == precision));

                U_TEST_FORMAT_ARG(EXPS, "{:.{}}", X, precision);
                U_TEST_FORMAT_ARG(EXPS, "{:.{}%T%z}", X, precision);
            } while (0 < precision);

            for (int rightPad = -1; rightPad < 10; ++rightPad) {
                exps = "%";
                if (0 < rightPad) {
                    exps.append(rightPad, '*');
                }

                U_TEST_FORMAT_ARG(EXPS, "{:*<{}%%}", X, 1 + rightPad);

                exps = "%" + nakedObj;
                if (0 < rightPad) {
                    exps.append(rightPad, '*');
                }

                U_TEST_FORMAT_ARG(EXPS,
                                  "{:*<{}%%%T%z}",
                                  X,
                                  1 + nakedWidth + rightPad);
            }

            for (int leftPad = -1; leftPad < 10; ++leftPad) {
                exps.clear();
                if (0 < leftPad) {
                    exps.append(leftPad, '*');
                }
                exps += "\t";

                U_TEST_FORMAT_ARG(EXPS, "{:*>{}%t}", X, 1 + leftPad);

                exps += nakedObj;

                U_TEST_FORMAT_ARG(EXPS,
                                  "{:*>{}%t%T%z}",
                                  X,
                                  1 + nakedWidth + leftPad);
            }

            for (int centerPad = -1; centerPad < 10; ++centerPad) {
                exps.clear();
                if (0 < centerPad / 2) {
                    exps.append(centerPad / 2, '*');
                }
                exps += nakedObj;
                int rightPad = centerPad - centerPad / 2;
                if (0 < rightPad) {
                    exps.append(rightPad, '*');
                }

                U_TEST_FORMAT_ARG(EXPS, "{:*^{}}", X, nakedWidth +centerPad);
                U_TEST_FORMAT_ARG(EXPS,
                                  "{:*^{}%T%z}",
                                  X,
                                  nakedWidth + centerPad);
            }

            nakedWidth = nakedIso8601.length();

            precision = 10;
            exps = nakedObj;    // We want `nakedIso8601` with precision 6
            exps.insert(exps.length() - 2, ":");
            exps.insert(exps.find_last_of("-+"), "0000");
            do {
                u::trimPrecision(&exps);
                --precision;

                ASSERT((u::npos == EXPS.find_first_of('.')) ==
                                                         (0 == precision));

                U_TEST_FORMAT_ARG(EXPS, "{:.{}%i}", X, precision);
            } while (0 < precision);

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

            for (int fi = 0; fi < 32; ++fi) {
                const unsigned FLAGS    = fi;
                const bool     ISO      = FLAGS &  1;
                const bool     COLON    = FLAGS &  2;
                const bool     NO_COLON = FLAGS &  4;
                const bool     Z        = FLAGS &  8;
                const bool     COMMA    = FLAGS & 16;

                if (COLON && NO_COLON) {
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
                if (COMMA) {
                    fmts.insert(2, ",");
                }
                const char    *FMTS = fmts.c_str();
                wstring        fmtwString;
                TU::widen(&fmtwString, FMTS);
                const wchar_t *FMTW = fmtwString.c_str();

                exps = ISO ? nakedIso8601 : nakedObj;
                if (!ISO && COLON) {
                    exps.insert(exps.length() - 2, ":");
                }
                if (ISO && NO_COLON) {
                    size_t idx = exps.rfind(':');
                    ASSERT(u::npos != idx);
                    exps.erase(idx, 1);
                }
                if (Z && 0 == OFFSET) {
                    size_t idx = exps.find_last_of("+-");;
                    ASSERT(u::npos != idx);
                    exps.resize(idx);
                    exps += 'Z';
                }
                if (COMMA) {
                    size_t idx = exps.find('.');
                    ASSERT(u::npos != idx);
                    exps[idx] = ',';
                }

                if (veryVeryVerbose) {
                    PH_(FLAGS);    P_(FMTS);    P(EXPS);
                }

                char bufS[1024];
                char *ends = bsl::vformat_to(bufS,
                                             FMTS,
                                             bsl::make_format_args(X));
                *ends = '\0';
                string ress = bufS;
                ASSERTV(TLINE, FLAGS, EXPS, ress, EXPS == ress);

                wchar_t bufW[1024];
                wchar_t *endw = bsl::vformat_to(bufW,
                                                FMTW,
                                                bsl::make_wformat_args(X));
                *endw = 0;
                wstring resw = bufW;
                wstring expw;
                TU::widen(&expw, exps);
                ASSERTV(TLINE, FLAGS, EXPS, TU::narrow(resw), expw == resw);
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
