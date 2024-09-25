// bslstl_formatterintegral.t.cpp                                     -*-C++-*-
#include <bslfmt_formatterintegral.h>

#include <bslfmt_formatimp.h> // Testing only (`bsl::format` - breathing test)

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>
#include <string.h>

using namespace BloombergLP;
using namespace bsl;


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
        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        bsl::formatter<int, char>    dummy1;
        bsl::formatter<int, wchar_t> dummy2;
        (void)dummy1;
        (void)dummy2;

        static const struct {
            int         d_line;        // source line number
            const char *d_format_p;    // format spec
            const char *d_expected_p;  // format
            int         d_value;       // value to be formatted
        } POSITIVE_CHAR_DATA[] = {
            //LINE  FORMAT                        EXPECTED          VALUE
            //----  ----------------------------  ---------------   -----
            { L_,   "{0:}, {0:+}, {0:-}, {0: }",  "5, +5, 5,  5",   5    },
            { L_,   "{:#05x}",                    "0x00c",          12   },
            { L_,   "{:#05X}",                    "0X00C",          12   },
            { L_,   "{:*<6}",                     "5*****",         5    },
            { L_,   "{:*>6}",                     "*****5",         5    },
            { L_,   "{:*^6}",                     "**5***",         5    },
            { L_,   "{:*^+6}",                    "**+5**",         5    },
            { L_,   "{:#0x}",                     "0x5",            5    },
            { L_,   "{:#01x}",                    "0x5",            5    },
            { L_,   "{:#02x}",                    "0x5",            5    },
            { L_,   "{:#03x}",                    "0x5",            5    },
            { L_,   "{:#04x}",                    "0x05",           5    },
            { L_,   "{:#04X}",                    "0X05",           5    },
            { L_,   "{:#b}",                      "0b101",          5    },
            { L_,   "{:#05b}",                    "0b101",          5    },
            { L_,   "{:#06b}",                    "0b0101",         5    },
            { L_,   "{:#06B}",                    "0B1100",         12   },
            { L_,   "{:#d}",                      "5",              5    },
            { L_,   "{:#d}",                      "12",             12   },
            { L_,   "{:#o}",                      "05",             5    },
            { L_,   "{:#o}",                      "014",            12   },
            { L_,   "{:b}",                       "0b1100",         12   },
            { L_,   "{:B}",                       "0B1100",         12   },
            { L_,   "{:d}",                       "12",             12   },
            { L_,   "{:o}",                       "014",            12   },
            { L_,   "{:x}",                       "0xc",            12   },
            { L_,   "{:X}",                       "0XC",            12   },
        };

        enum {
            NUM_POSITIVE_CHAR_DATA = sizeof POSITIVE_CHAR_DATA /
                                     sizeof *POSITIVE_CHAR_DATA
        };

        static const struct {
            int         d_line;        // source line number
            const char *d_format_p;    // format spec
            const char *d_expected_p;  // format
            int         d_value;       // value to be formatted
        } NEGATIVE_CHAR_DATA[] = {
            //LINE  FORMAT                        EXPECTED           VALUE
            //----  ----------------------------  -----------------  -----
            { L_,   "{0:}, {0:+}, {0:-}, {0: }",  "-5, -5, -5, -5",  -5    },
            { L_,   "{:#05x}",                    "-0x0c",           -12   },
            { L_,   "{:#05X}",                    "-0X0C",           -12   },
            { L_,   "{:*<6}",                     "-5****",          -5    },
            { L_,   "{:*>6}",                     "****-5",          -5    },
            { L_,   "{:*^6}",                     "**-5**",          -5    },
            { L_,   "{:*^+6}",                    "**-5**",          -5    },
            { L_,   "{:#0x}",                     "-0x5",            -5    },
            { L_,   "{:#01x}",                    "-0x5",            -5    },
            { L_,   "{:#02x}",                    "-0x5",            -5    },
            { L_,   "{:#03x}",                    "-0x5",            -5    },
            { L_,   "{:#04x}",                    "-0x5",            -5    },
            { L_,   "{:#05X}",                    "-0X05",           -5    },
            { L_,   "{:#b}",                      "-0b101",          -5    },
            { L_,   "{:#05b}",                    "-0b101",          -5    },
            { L_,   "{:#07b}",                    "-0b0101",         -5    },
            { L_,   "{:#08B}",                    "-0B01100",        -12   },
            { L_,   "{:#d}",                      "-5",              -5    },
            { L_,   "{:#d}",                      "-12",             -12   },
            { L_,   "{:#o}",                      "-05",             -5    },
            { L_,   "{:#o}",                      "-014",            -12   },
            { L_,   "{:b}",                       "-0b1100",         -12   },
            { L_,   "{:B}",                       "-0B1100",         -12   },
            { L_,   "{:d}",                       "-12",             -12   },
            { L_,   "{:o}",                       "-014",            -12   },
            { L_,   "{:x}",                       "-0xc",            -12   },
            { L_,   "{:X}",                       "-0XC",            -12   },
        };

        enum {
            NUM_NEGATIVE_CHAR_DATA = sizeof NEGATIVE_CHAR_DATA /
                                     sizeof *NEGATIVE_CHAR_DATA
        };

        static const struct {
            int            d_line;        // source line number
            const wchar_t *d_format_p;    // format spec
            const wchar_t *d_expected_p;  // format
            int            d_value;       // value to be formatted
        } POSITIVE_WCHAR_DATA[] = {
            //LINE  FORMAT                         EXPECTED           VALUE
            //----  ----------------------------   ----------------   -----
            { L_,   L"{0:}, {0:+}, {0:-}, {0: }",  L"5, +5, 5,  5",   5    },
            { L_,   L"{:#05x}",                    L"0x00c",          12   },
            { L_,   L"{:#05X}",                    L"0X00C",          12   },
            { L_,   L"{:*<6}",                     L"5*****",         5    },
            { L_,   L"{:*>6}",                     L"*****5",         5    },
            { L_,   L"{:*^6}",                     L"**5***",         5    },
            { L_,   L"{:*^+6}",                    L"**+5**",         5    },
            { L_,   L"{:#0x}",                     L"0x5",            5    },
            { L_,   L"{:#01x}",                    L"0x5",            5    },
            { L_,   L"{:#02x}",                    L"0x5",            5    },
            { L_,   L"{:#03x}",                    L"0x5",            5    },
            { L_,   L"{:#04x}",                    L"0x05",           5    },
            { L_,   L"{:#04X}",                    L"0X05",           5    },
            { L_,   L"{:#b}",                      L"0b101",          5    },
            { L_,   L"{:#05b}",                    L"0b101",          5    },
            { L_,   L"{:#06b}",                    L"0b0101",         5    },
            { L_,   L"{:#06B}",                    L"0B1100",         12   },
            { L_,   L"{:#d}",                      L"5",              5    },
            { L_,   L"{:#d}",                      L"12",             12   },
            { L_,   L"{:#o}",                      L"05",             5    },
            { L_,   L"{:#o}",                      L"014",            12   },
            { L_,   L"{:b}",                       L"0b1100",         12   },
            { L_,   L"{:B}",                       L"0B1100",         12   },
            { L_,   L"{:d}",                       L"12",             12   },
            { L_,   L"{:o}",                       L"014",            12   },
            { L_,   L"{:x}",                       L"0xc",            12   },
            { L_,   L"{:X}",                       L"0XC",            12   },
        };

        enum {
            NUM_POSITIVE_WCHAR_DATA = sizeof POSITIVE_WCHAR_DATA /
                                     sizeof *POSITIVE_WCHAR_DATA
        };

        static const struct {
            int            d_line;        // source line number
            const wchar_t *d_format_p;    // format spec
            const wchar_t *d_expected_p;  // format
            int            d_value;       // value to be formatted
        } NEGATIVE_WCHAR_DATA[] = {
            //LINE  FORMAT                         EXPECTED            VALUE
            //----  -----------------------------  ------------------  -----
            { L_,   L"{0:}, {0:+}, {0:-}, {0: }",  L"-5, -5, -5, -5",  -5    },
            { L_,   L"{:#05x}",                    L"-0x0c",           -12   },
            { L_,   L"{:#05X}",                    L"-0X0C",           -12   },
            { L_,   L"{:*<6}",                     L"-5****",          -5    },
            { L_,   L"{:*>6}",                     L"****-5",          -5    },
            { L_,   L"{:*^6}",                     L"**-5**",          -5    },
            { L_,   L"{:*^+6}",                    L"**-5**",          -5    },
            { L_,   L"{:#0x}",                     L"-0x5",            -5    },
            { L_,   L"{:#01x}",                    L"-0x5",            -5    },
            { L_,   L"{:#02x}",                    L"-0x5",            -5    },
            { L_,   L"{:#03x}",                    L"-0x5",            -5    },
            { L_,   L"{:#04x}",                    L"-0x5",            -5    },
            { L_,   L"{:#05X}",                    L"-0X05",           -5    },
            { L_,   L"{:#b}",                      L"-0b101",          -5    },
            { L_,   L"{:#05b}",                    L"-0b101",          -5    },
            { L_,   L"{:#07b}",                    L"-0b0101",         -5    },
            { L_,   L"{:#08B}",                    L"-0B01100",        -12   },
            { L_,   L"{:#d}",                      L"-5",              -5    },
            { L_,   L"{:#d}",                      L"-12",             -12   },
            { L_,   L"{:#o}",                      L"-05",             -5    },
            { L_,   L"{:#o}",                      L"-014",            -12   },
            { L_,   L"{:b}",                       L"-0b1100",         -12   },
            { L_,   L"{:B}",                       L"-0B1100",         -12   },
            { L_,   L"{:d}",                       L"-12",             -12   },
            { L_,   L"{:o}",                       L"-014",            -12   },
            { L_,   L"{:x}",                       L"-0xc",            -12   },
            { L_,   L"{:X}",                       L"-0XC",            -12   },
        };

        enum {
            NUM_NEGATIVE_WCHAR_DATA = sizeof NEGATIVE_WCHAR_DATA /
                                     sizeof *NEGATIVE_WCHAR_DATA
        };

        for (int i = 0; i < NUM_POSITIVE_CHAR_DATA; ++i) {
            const int            LINE     = POSITIVE_CHAR_DATA[i].d_line;
            const char          *FORMAT   = POSITIVE_CHAR_DATA[i].d_format_p;
            const bsl::string    EXPECTED = POSITIVE_CHAR_DATA[i].d_expected_p;
            const int            VALUE    = POSITIVE_CHAR_DATA[i].d_value;
            const long           L_VALUE  = static_cast<long>(VALUE);
            const long long      LL_VALUE = static_cast<long long>(VALUE);
            const unsigned long  UL_VALUE  = static_cast<unsigned long>(VALUE);

            const unsigned long long ULL_VALUE  =
                                        static_cast<unsigned long long>(VALUE);

            bsl::string result = bslfmt::format(FORMAT, VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);

            result = bslfmt::format(FORMAT, L_VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);

            result = bslfmt::format(FORMAT, LL_VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);

            result = bslfmt::format(FORMAT, UL_VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);

            result = bslfmt::format(FORMAT, ULL_VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);
        }

        for (int i = 0; i < NUM_NEGATIVE_CHAR_DATA; ++i) {
            const int          LINE     = NEGATIVE_CHAR_DATA[i].d_line;
            const char        *FORMAT   = NEGATIVE_CHAR_DATA[i].d_format_p;
            const bsl::string  EXPECTED = NEGATIVE_CHAR_DATA[i].d_expected_p;
            const int          VALUE    = NEGATIVE_CHAR_DATA[i].d_value;
            const long         L_VALUE  = static_cast<long>(VALUE);
            const long long    LL_VALUE = static_cast<long long>(VALUE);

            bsl::string result = bslfmt::format(FORMAT, VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);

            result = bslfmt::format(FORMAT, L_VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);

            result = bslfmt::format(FORMAT, LL_VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);
        }

        if (verbose) printf("\tTesting wstrings.\n");

        for (int i = 0; i < NUM_POSITIVE_WCHAR_DATA; ++i) {
            const int            LINE     = POSITIVE_WCHAR_DATA[i].d_line;
            const wchar_t       *FORMAT   = POSITIVE_WCHAR_DATA[i].d_format_p;
            const bsl::wstring   EXPECTED =
                                           POSITIVE_WCHAR_DATA[i].d_expected_p;
            const int            VALUE    = POSITIVE_WCHAR_DATA[i].d_value;
            const long           L_VALUE  = static_cast<long>(VALUE);
            const long long      LL_VALUE = static_cast<long long>(VALUE);
            const unsigned long  UL_VALUE  = static_cast<unsigned long>(VALUE);

            const unsigned long long ULL_VALUE  =
                                        static_cast<unsigned long long>(VALUE);

            bsl::wstring result = bslfmt::format(FORMAT, VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);

            result = bslfmt::format(FORMAT, L_VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);

            result = bslfmt::format(FORMAT, LL_VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);

            result = bslfmt::format(FORMAT, UL_VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);

            result = bslfmt::format(FORMAT, ULL_VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);
        }

        for (int i = 0; i < NUM_NEGATIVE_WCHAR_DATA; ++i) {
            const int           LINE     = NEGATIVE_WCHAR_DATA[i].d_line;
            const wchar_t      *FORMAT   = NEGATIVE_WCHAR_DATA[i].d_format_p;
            const bsl::wstring  EXPECTED = NEGATIVE_WCHAR_DATA[i].d_expected_p;
            const int           VALUE    = NEGATIVE_WCHAR_DATA[i].d_value;
            const long          L_VALUE  = static_cast<long>(VALUE);
            const long long     LL_VALUE = static_cast<long long>(VALUE);

            bsl::wstring result = bslfmt::format(FORMAT, VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);

            result = bslfmt::format(FORMAT, L_VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);

            result = bslfmt::format(FORMAT, LL_VALUE);
            ASSERTV(LINE, EXPECTED.c_str(), result.c_str(),
                    EXPECTED == result);
        }
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
