// bdlt_formattestutil.t.cpp                                          -*-C++-*-
#include <bdlt_formattestutil.h>

#include <bdlt_datetime.h>

#include <bdlsb_memoutstreambuf.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_algorithm.h>
#include <bsl_charconv.h>
#include <bsl_cstdlib.h>     // `atoi`
#include <bsl_cstring.h>     // `memcmp`, `strcmp`
#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::ends;
using bsl::flush;
using bsl::ostream;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// [ 3] BDLT_FORMATTESTUTIL_TEST_FORMAT(    exp, format, value);
// [ 3] BDLT_FORMATTESTUTIL_TEST_FORMAT_ARG(exp, format, value, arg);
// [ 2] void widen(bsl::wstring *result, const bsl::string_view& in);
// [ 2] bsl::string narrow(const bsl::wstring_view& in);
// [ 1] BREATHING TEST
// ----------------------------------------------------------------------------

// ============================================================================
//                     BDE ASSERT TEST FUNCTION ADAPTED TO `cerr`
// ----------------------------------------------------------------------------

namespace {

int realTestStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cerr << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= realTestStatus && realTestStatus <= 100) {
            ++realTestStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               TEST DRIVER MACRO ABBREVIATIONS ADAPTED TO `cerr`
// ----------------------------------------------------------------------------

// The macros `BDLT_FORMATTESTUTIL_TEST_FORMAT` and
// `BDLT_FORMATTESTUTIL_TEST_FORMAT_ARG`, which are the most important things
// to test here, unavoidably do the output that we want to examine to `cout`.
// So in this test driver, we capture `cout` to a `streambuf` and write our own
// `ASSERT` macros that do their output to `cerr` rather than `cout`.

// The following macros are just copied from `bslim_testutil` with slight
// modification.

#define U_DEBUG_REP(X) BloombergLP::bslim::TestUtil::debugRep(X)

#define ASSERT(X)                                                             \
    aSsErT(!(X), #X, __LINE__);

#define LOOP_ASSERT(I,X)                                                      \
    if (!(X)) { bsl::cerr << #I ": " << U_DEBUG_REP(I) <<                     \
                                                         "    (context)\n";   \
                aSsErT(1, #X, __LINE__); }

#define LOOP2_ASSERT(I,J,X)                                                   \
    if (!(X)) { bsl::cerr << #I ": " << U_DEBUG_REP(I) << "\t"                \
                          << #J ": " << U_DEBUG_REP(J) <<                     \
                                                         "    (context)\n";   \
                aSsErT(1, #X, __LINE__); }

#define LOOP3_ASSERT(I,J,K,X)                                                 \
    if (!(X)) { bsl::cerr << #I ": " << U_DEBUG_REP(I) << "\t"                \
                          << #J ": " << U_DEBUG_REP(J) << "\t"                \
                          << #K ": " << U_DEBUG_REP(K) <<                     \
                                                         "    (context)\n";   \
                aSsErT(1, #X, __LINE__); }

#define LOOP4_ASSERT(I,J,K,L,X)                                               \
    if (!(X)) { bsl::cerr << #I ": " << U_DEBUG_REP(I) << "\t"                \
                          << #J ": " << U_DEBUG_REP(J) << "\t"                \
                          << #K ": " << U_DEBUG_REP(K) << "\t"                \
                          << #L ": " << U_DEBUG_REP(L) <<                     \
                                                         "    (context)\n";   \
                aSsErT(1, #X, __LINE__); }

#define L_    __LINE__

// ============================================================================
//                         MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define U_TEST_FORMAT     BDLT_FORMATTESTUTIL_TEST_FORMAT
#define U_TEST_FORMAT_ARG BDLT_FORMATTESTUTIL_TEST_FORMAT_ARG

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                     GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

/// Return a string view containing the character representation of the
/// specified `lineNum` written to the specified `buf`.
template <int LEN>
bsl::string_view writeLineNum(char (&buf)[LEN], int lineNum)
{
    bsl::to_chars_result result = bsl::to_chars(buf + 0, buf + LEN, lineNum);
    BSLS_ASSERT(bsl::ErrcEnum() == result.ec);
    return bsl::string_view(buf, result.ptr - buf);
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

const bsl::size_t npos = bsl::string::npos;

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;    (void) verbose;
    const bool         veryVerbose = argc > 3;    (void) veryVerbose;
    const bool     veryVeryVerbose = argc > 4;    (void) veryVeryVerbose;
    const bool veryVeryVeryVerbose = argc > 5;    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator ta("test", veryVeryVeryVerbose);

    bdlsb::MemOutStreamBuf sb(&ta);
    bsl::streambuf *saveCout = cout.rdbuf();
    cout.rdbuf(&sb);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ..._TEST_FORMAT, ..._TEST_FORMAT_ARG
        //
        // Concerns:
        // 1. That `BDLT_FORMATTESTUTIL_TEST_FORMAT` and
        //    `BDLT_FORMATTESTUTIL_TEST_FORMAT_ARG` function properly.
        //
        // Plan:
        // 1. Declare fixed variables `VALUE` and `ARG` to be passed to
        //    `U_TEST_FORMAT` and `U_TEST_FORMAT_ARG`.
        //
        // 2. Declare a table of values of `FORMAT`, the format string passed
        //    to `bsl::format` or an equivalent, `EXP`, the expected result
        //    when passing that format with `VALUE` and `ARG`, and `ARG_FLAG`,
        //    a boolean indicating when `ARG` is to be passed.
        //
        // 3. Iterate through the table.
        //
        // 4. Call `vformat` with `FORMAT`, `VALUE`, and maybe `ARG` and check
        //    that the result matches `EXP`.  This is just to test the
        //    correctness of the table.
        //
        // 5. Clear the streambuf that we're capturing `cout` to and set
        //    `testStatus` to 0 (note that we have renamed the `testStatus` in
        //    the `ASSERT` macros to something else so that the `testStatus`
        //    modified by the macros under test and the other one modified by
        //    `ASSERT` don't interfere with each other).
        //
        // 6. Call the macro under test with `FORMAT`, `VALUE`, and if
        //    `ARG_FLAG` is set, `ARG`.
        //
        // 7. Observe that no output was done to `cout` and `0 == testStatus`.
        //
        // 8. Iterate four times over int `tj`.
        //
        // 9. Copy string `badExp` from `EXP` and modify it one of 4 ways
        //    depending on `tj`.
        //
        // 10. Set `testStatus` to 0 and clear the streambuf that `cout` is
        //     captured to.
        //
        // 11. Call the macro under test.  First, set a string `lineMatch` to
        //     be "(#): " where # is the line number of the call to the macro.
        //
        // 12. Since the test the macro performs is expected to fail, we expect
        //     `testStatus` to have been incremented -- confirm this.
        //
        // 13. Examine the captured `cout` output from the macro under test
        //     and confirm that it contains all the expected substrings.
        //
        // Testing
        //   BDLT_FORMATTESTUTIL_TEST_FORMAT(    exp, format, value);
        //   BDLT_FORMATTESTUTIL_TEST_FORMAT_ARG(exp, format, value, arg);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING ..._TEST_FORMAT, ..._TEST_FORMAT_ARG\n"
                             "============================================\n";

        const bdlt::Datetime VALUE(2026, 1, 16, 12, 55, 26, 123, 456);
        const int            ARG(20);

        static const struct Data {
            int         d_line;
            const char *d_format;
            const char *d_exp;
            bool        d_argFlag;
        } DATA[] = {
            { L_, "{}",           "16JAN2026_12:55:26.123456",     0 },
            { L_, "{:.0}",        "16JAN2026_12:55:26",            0 },
            { L_, "{:^22.0}",     "  16JAN2026_12:55:26  ",        0 },
            { L_, "{:%Y}",        "2026",                          0 },
            { L_, "{:*<10%d}",    "16********",                    0 },
            { L_, "{:<16%D}",     "16JAN2026       ",              0 },
            { L_, "{:>24.3%T}",   "            12:55:26.123",      0 },
            { L_, "{:^{}.0%T}",   "      12:55:26      ",          1 },
            { L_, "{:*<{}%d}",    "16******************",          1 },
            { L_, "{:<{}%D}",     "16JAN2026           ",          1 },
            { L_, "{:>{}.3%T}",   "        12:55:26.123",          1 },
            { L_, "{:.{}%T}",     "12:55:26.12345600000000000000", 1 },
            { L_, "{:->29.{}%S}", "------26.12345600000000000000", 1 },
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&            data     = DATA[ti];
            const int              LINE     = data.d_line;
            const bsl::string_view FORMAT   = data.d_format;
            const bsl::string_view EXP      = data.d_exp;
            const bool             ARG_FLAG = data.d_argFlag;

            if (veryVerbose) {
                cerr << LINE << ": \"" << EXP << "\", " << FORMAT << endl;
            }

            // This is just to test that `EXP` is correct.

            const bsl::string& s = ARG_FLAG
                      ? bsl::vformat(FORMAT, bsl::make_format_args(VALUE, ARG))
                      : bsl::vformat(FORMAT, bsl::make_format_args(VALUE));
            LOOP2_ASSERT(s, EXP, s == EXP);

            sb.reset();

            int testStatus = 0;

            if (ARG_FLAG) {
                U_TEST_FORMAT_ARG(EXP, FORMAT, VALUE, ARG);
            }
            else {
                U_TEST_FORMAT(    EXP, FORMAT, VALUE);
            }

            LOOP_ASSERT(LINE, 0 == testStatus);
            LOOP_ASSERT(LINE, 0 == sb.length());

            for (int tj = 0; tj < 4; ++tj) {
                bsl::string badExp(EXP);
                ASSERT(!badExp.empty());

                switch (tj) {
                  case 0: {
                    badExp += '_';
                  } break;
                  case 1: {
                    badExp.resize(badExp.size() - 1);
                  } break;
                  case 2: {
                    badExp = "*";
                  } break;
                  case 3: {
                    badExp += EXP;
                  }
                }
                ASSERT(EXP != badExp);

                char        lineNumBuf[20];
                bsl::string lineMatch("(");

#define U_TEST_EITHER(exp, format, value, arg)                                \
                if (ARG_FLAG) {                                               \
                    U_TEST_FORMAT_ARG(exp, format, value, arg);               \
                }                                                             \
                else {                                                        \
                    U_TEST_FORMAT(    exp, format, value);                    \
                }

                testStatus = 0;
                sb.reset();
                lineMatch += u::writeLineNum(lineNumBuf, __LINE__ + 2);
                lineMatch += "): ";
                U_TEST_EITHER(badExp, FORMAT, VALUE, ARG);

                LOOP2_ASSERT(LINE, tj, 1 == testStatus);

                bsl::string_view output(sb.data(), sb.length());
                LOOP2_ASSERT(LINE, tj, !output.empty());
                LOOP2_ASSERT(LINE, tj, 0 == output.find("Error "));
                LOOP2_ASSERT(LINE, tj, bsl::strlen("Error ") ==
                                                        output.find(__FILE__));
                LOOP2_ASSERT(LINE, tj, npos != output.find(lineMatch));
                LOOP2_ASSERT(LINE, tj, npos != output.find(
                                                "<char>testEvaluateVFormat("));
                LOOP2_ASSERT(LINE, tj, npos != output.find(FORMAT));
                LOOP2_ASSERT(LINE, tj, npos != output.find(") failed, msg: "));
                LOOP2_ASSERT(LINE, tj, npos != output.find(badExp));
                LOOP2_ASSERT(LINE, tj, npos != output.find(EXP));
                if (ARG_FLAG) {
                    LOOP2_ASSERT(LINE, tj, npos != output.find("20"));
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING WIDEN AND NARROW
        //
        // Concerns:
        // 1. That the `widen` and `narrow` functions work properly.
        //
        // Plan:
        // 1. Start with a table of narrow and wide strings, where both the
        //    narrow and wide strings contain the same ascii string.
        //
        // 2. Iterate through the table.
        //
        // 3. Call `narrow` on the wide string and confirm the result matches
        //    the narrow string.
        //
        // 4. Call `widen` on the narrow string and confirm the result matches
        //    the wide string.
        //
        // 5. After the loop, restore `cout` and use the `BSLS_ASSERTTEST`
        //    macros for negative testing on non-ascii input.
        //
        // Testing:
        //   void widen(bsl::wstring *result, const bsl::string_view& in);
        //   bsl::string narrow(const bsl::wstring_view& in);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING WIDEN AND NARROW\n"
                             "========================\n";

        static const struct Data {
            int            d_line;
            const char    *d_cstring;
            const wchar_t *d_wstring;
        } DATA[] = {
            { L_, "",             L""             },
            { L_, "the quick",    L"the quick"    },
            { L_, "brown fox",    L"brown fox"    },
            { L_, "jumped over",  L"jumped over"  },
            { L_, "the lazy DOG", L"the lazy DOG" }
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&             data = DATA[ti];
            const int               LINE = data.d_line;
            const bsl::string_view  SC   = data.d_cstring;
            const bsl::wstring_view SW   = data.d_wstring;

            const bsl::string& sc = bdlt::FormatTestUtil::narrow(SW);
            bsl::wstring sw(&ta);
            bdlt::FormatTestUtil::widen(&sw, SC);

            LOOP_ASSERT(LINE, SC == sc);
            LOOP_ASSERT(LINE, SW == sw);
        }

       /// Negative testing.  Restore `cout` to avoid the necessity of defining
       /// local `ASSERTTEST` macros that use `cerr`.

        cout.rdbuf(saveCout);

        {
            bsls::AssertTestHandlerGuard guard;

            bsl::wstring w;
            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(
                           (void) bdlt::FormatTestUtil::widen(&w, "woof"));
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(
                           (void) bdlt::FormatTestUtil::widen(&w, "\xc4\x89"));

            BSLS_ASSERTTEST_ASSERT_SAFE_PASS(
                           (void) bdlt::FormatTestUtil::narrow(L"woof"));
            BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(
                           (void) bdlt::FormatTestUtil::narrow(L"\xc489"));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

      } break;
      default: {
        cout.rdbuf(saveCout);
        cout << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        realTestStatus = -1;
      }
    }

    cout.rdbuf(saveCout);

    if (realTestStatus > 0) {
        cout << "Error, non-zero test status = " << realTestStatus << "." <<
                                                                          endl;
    }
    return realTestStatus;
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
