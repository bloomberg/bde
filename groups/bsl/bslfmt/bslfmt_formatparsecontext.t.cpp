// bslfmt_formatparsecontext.t.cpp                                    -*-C++-*-
#include <bslfmt_formatparsecontext.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>
#include <string.h>

using namespace BloombergLP;
using namespace bsl;


// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// TBD.
// ----------------------------------------------------------------------------
// CREATORS
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------
namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;
    // const bool veryVerbose = argc > 3;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        typedef bslfmt::basic_format_parse_context<char>    CharObj;
        typedef bslfmt::basic_format_parse_context<wchar_t> WcharObj;

        static const struct {
            int            d_line;       // source line number
            const char    *d_format_p;   // format spec
            const wchar_t *d_wformat_p;  // wide format spec
        } DATA[] = {
            //LINE  FORMAT        WIDE FORMAT
            //----  ------------  ------------
            { L_,   "",           L""           },
            { L_,   "{0:}",       L"{0:}"       },
            { L_,   "{0:+}",      L"{0:+}"      },
            { L_,   "{0:-}",      L"{0:-}"      },
            { L_,   "{0: }",      L"{0: }"      },
            { L_,   "{:}",        L"{:}"        },
            { L_,   "{:+}",       L"{:+}"       },
            { L_,   "{:-}",       L"{:-}"       },
            { L_,   "{: }",       L"{: }"       },
            { L_,   "{:#05x}",    L"{:#05x}"    },
            { L_,   "{:#05X}",    L"{:#05X}"    },
            { L_,   "{:*<6}",     L"{:*<6}"     },
            { L_,   "{:*>6}",     L"{:*>6}"     },
            { L_,   "{:*^6}",     L"{:*^6}"     },
            { L_,   "{:*^+6}",    L"{:*^+6}"    },
            { L_,   "Dummy spec", L"Dummy spec" },
        };

        enum {
            NUM_DATA = sizeof DATA / sizeof *DATA
        };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE    = DATA[i].d_line;
            bsl::string_view  FORMAT  = DATA[i].d_format_p;
            bsl::wstring_view WFORMAT = DATA[i].d_wformat_p;

            // Testing constructor and accessors.

            CharObj        mX(FORMAT);
            const CharObj& X = mX;

            ASSERTV(LINE, FORMAT.begin() == X.begin());
            ASSERTV(LINE, FORMAT.end()   == X.end()  );

            WcharObj        mXWide(WFORMAT);
            const WcharObj& XWide = mXWide;

            ASSERTV(LINE, WFORMAT.begin() == XWide.begin());
            ASSERTV(LINE, WFORMAT.end()   == XWide.end()  );

            // Testing constructor with two parameters.

            for (size_t j = 0; j < 3; ++j) {
                const size_t   NUM_ARGUMENTS = j;
                CharObj        mXParam(FORMAT, NUM_ARGUMENTS);
                const CharObj& XParam = mXParam;

                ASSERTV(LINE, FORMAT.begin() == XParam.begin());
                ASSERTV(LINE, FORMAT.end() == XParam.end());

                WcharObj        mXWideParam(WFORMAT, NUM_ARGUMENTS);
                const WcharObj& XWideParam = mXWideParam;

                ASSERTV(LINE, WFORMAT.begin() == XWideParam.begin());
                ASSERTV(LINE, WFORMAT.end() == XWideParam.end());
            }

            // Testing `advance_to()`.

            CharObj::const_iterator  it     = FORMAT.begin();
            WcharObj::const_iterator itWide = WFORMAT.begin();

            ASSERTV(LINE, FORMAT.length(), WFORMAT.length(),
                    FORMAT.length() == WFORMAT.length());

            for (; it <= FORMAT.end(); ++it, ++itWide) {
                mX.advance_to(it);
                ASSERTV(LINE, *it, it == X.begin());

                mXWide.advance_to(itWide);
                ASSERTV(LINE, *itWide, itWide == XWide.begin());
            }

#ifdef BDE_BUILD_TARGET_EXC
            try {
                mX.advance_to(it);
                ASSERTV(LINE, "Exception has not been thrown", false);
            }
            catch (const bsl::format_error& err) {
                const char *expectedMessage =
                               "Parse error: advance to iterator out of range";
                ASSERTV(LINE, err.what(),
                        0 == strcmp(expectedMessage, err.what()));
            }
            catch (...) {
                ASSERTV(LINE, "Unexpected exception", false);
            }

            try {
                mXWide.advance_to(itWide);
                ASSERTV(LINE, "Exception has not been thrown", false);
            }
            catch (const bsl::format_error& err) {
                const char *expectedMessage =
                               "Parse error: advance to iterator out of range";
                ASSERTV(LINE, err.what(),
                        0 == strcmp(expectedMessage, err.what()));
            }
            catch (...) {
                ASSERTV(LINE, "Unexpected exception", false);
            }
#endif
        }

        // Testing `next_arg_id()` and `check_arg_id()`.

        const bsl::string_view emptyFormatString;
        for (size_t i = 0; i < 3; ++i) {
            const size_t NUM_ARGUMENTS = i;
            CharObj      mX(emptyFormatString, NUM_ARGUMENTS);

            for (size_t j = 0; j < NUM_ARGUMENTS; ++j) {
                const size_t EXPECTED = j;
                size_t       rc       = mX.next_arg_id();  // action
                ASSERTV(NUM_ARGUMENTS, EXPECTED, rc, EXPECTED == rc);
            }

#ifdef BDE_BUILD_TARGET_EXC
            try {
                mX.next_arg_id();  // action
                ASSERTV("Exception has not been thrown", false);
            }
            catch (const bsl::format_error& err) {
                const char *expectedMessage =
                           "Number of conversion specifiers exceeds number of "
                           "arguments";
                ASSERTV(NUM_ARGUMENTS,
                        err.what(),
                        0 == strcmp(expectedMessage, err.what()));
            }
            catch (...) {
                ASSERTV(NUM_ARGUMENTS, "Unexpected exception", false);
            }
#endif
        }

        for (size_t i = 0; i < 3; ++i) {
            const size_t NUM_ARGUMENTS = i;
            CharObj      mX(emptyFormatString, NUM_ARGUMENTS);

            for (size_t j = 0; j < NUM_ARGUMENTS; ++j) {
                const size_t ARGUMENT_NUM = j;
                mX.check_arg_id(ARGUMENT_NUM);  // action
            }

#ifdef BDE_BUILD_TARGET_EXC
            try {
                mX.check_arg_id(NUM_ARGUMENTS);  // action
                ASSERTV("Exception has not been thrown", false);
            }
            catch (const bsl::format_error& err) {
                const char *expectedMessage = "Invalid argument index";
                ASSERTV(NUM_ARGUMENTS,
                        err.what(),
                        0 == strcmp(expectedMessage, err.what()));
            }
            catch (...) {
                ASSERTV(NUM_ARGUMENTS, "Unexpected exception", false);
            }
#endif
        }


#ifdef BDE_BUILD_TARGET_EXC
        {
            CharObj mX(emptyFormatString, 1);
            ASSERTV(0 == mX.next_arg_id());  // set to `e_AUTOMATIC` state
            try {
                mX.check_arg_id(0);  // action
                ASSERTV("Exception has not been thrown", false);
            }
            catch (const bsl::format_error& err) {
                const char *expectedMessage =
                                     "Mixing of automatic and manual indexing";
                ASSERTV(err.what(),
                        0 == strcmp(expectedMessage, err.what()));
            }
            catch (...) {
                ASSERTV("Unexpected exception", false);
            }
        }
        {
            CharObj mX(emptyFormatString, 1);
            mX.check_arg_id(0);  // set to `e_MANUAL` state
            try {
                mX.next_arg_id();  // action
                ASSERTV("Exception has not been thrown", false);
            }
            catch (const bsl::format_error& err) {
                const char *expectedMessage =
                                     "Mixing of automatic and manual indexing";
                ASSERTV(err.what(),
                        0 == strcmp(expectedMessage, err.what()));
            }
            catch (...) {
                ASSERTV("Unexpected exception", false);
            }
        }
#endif
      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
