// bdlt_formattestutil.t.cpp                                          -*-C++-*-
#include <bdlt_formattestutil.h>

#include <bdlsb_memoutstreambuf.h>

#include <bslim_testutil.h>

#include <bslfmt_format.h>

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
// [ 3] U_TEST_FORMAT(    exp, format, value);
// [ 3] U_TEST_FORMAT_ARG(exp, format, value, arg);
// [ 2] void widen(bsl::wstring *result, const bsl::string_view& in);
// [ 2] bsl::string narrow(const bsl::wstring_view& in);
// [ 1] BREATHING TEST
// ----------------------------------------------------------------------------

//=========================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-------------------------------------------------------------------------

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

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define LOOP7_ASSERT BSLIM_TESTUTIL_LOOP7_ASSERT
#define LOOP8_ASSERT BSLIM_TESTUTIL_LOOP8_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q       // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P       // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_      // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_      // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_      // current Line number

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
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                     GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

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

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ..._TEST_FORMAT, ..._TEST_FORMAT_ARG
        //
        // Concerns:
        // 1. That `U_TEST_FORMAT` and `U_TEST_FORMAT_ARG` function properly.
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
        //   U_TEST_FORMAT(    exp, format, value);
        //   U_TEST_FORMAT_ARG(exp, format, value, arg);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING ..._TEST_FORMAT, ..._TEST_FORMAT_ARG\n"
                             "============================================\n";

        const double VALUE(3.1415926);
        const int    ARG(5);

        static const struct Data {
            int         d_line;
            const char *d_format;
            const char *d_exp;
            bool        d_argFlag;
        } DATA[] = {
            { L_, "{}",           "3.1415926",                     0 },
            { L_, "{:.0}",        "3",                             0 },
            { L_, "{:_^22.0}",    "__________3___________",        0 },
            { L_, "{:_^{}.0}",    "__3__",                         1 },
            { L_, "{:.{}}",       "3.1416",                        1 },
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&            data     = DATA[ti];
            const int              LINE     = data.d_line;
            const bsl::string_view FORMAT   = data.d_format;
            bsl::string            exp      = data.d_exp;
            const bool             ARG_FLAG = data.d_argFlag;

            if (veryVerbose) {
                cout << LINE << ": \"" << exp << "\", " << FORMAT << endl;
            }

            // This is just to test that `EXP` is correct.
            const bsl::string& s = ARG_FLAG
                      ? bsl::vformat(FORMAT, bsl::make_format_args(VALUE, ARG))
                      : bsl::vformat(FORMAT, bsl::make_format_args(VALUE));
            ASSERTV(s, exp, s == exp);

            if (ARG_FLAG) {
                U_TEST_FORMAT_ARG(exp, FORMAT, VALUE, ARG);
            }
            else {
                U_TEST_FORMAT(    exp, FORMAT, VALUE);
            }

            ASSERTV(testStatus, ti, ti == testStatus);

            exp += '_';

            if (ARG_FLAG) {
                U_TEST_FORMAT_ARG(exp, FORMAT, VALUE, ARG);
            }
            else {
                U_TEST_FORMAT(    exp, FORMAT, VALUE);
            }

            ASSERTV(testStatus, ti, ti + 1 == testStatus);
        }

        testStatus -= k_NUM_DATA;
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
        cout << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cout << "Error, non-zero test status = " << testStatus << "." << endl;
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
