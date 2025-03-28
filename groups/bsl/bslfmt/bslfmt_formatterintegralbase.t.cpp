// bslfmt_formatterintegralbase.t.cpp                                 -*-C++-*-
#include <bslfmt_formatterintegralbase.h>

#include <bslfmt_mockformatcontext.h>
#include <bslfmt_mockparsecontext.h>

#include <bsls_bsltestutil.h>

#include <bslstl_stringview.h>

#include <stdio.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// TBD
// ----------------------------------------------------------------------------
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
        fflush(stdout);

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
        //: 1 Using table-based approach invoke public methods of class being
        //:   tested and verify the results.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef bslfmt::StandardFormatSpecification<char>     CharSpec;
        typedef bslfmt::StandardFormatSpecification<wchar_t> WcharSpec;
        typedef bslfmt::MockParseContext<char>                CharParseContext;
        typedef bslfmt::MockParseContext<wchar_t>            WcharParseContext;

        static const struct {
            int         d_line;      // source line number
            const char *d_char_p;    // format specification
            int         d_value ;    // value to format
            const char *d_prefix_p;  // expected prefix
            const char *d_value_p;   // expected formatted value
            const char *d_result_p;  // expected format result
        } CHAR_DATA[] = {
            //LINE  FORMAT     VALUE   EXP_PREFIX  EXP_VALUE   EXP_RESULT
            //----  -------    -----   ----------  ---------   ----------
            { L_,   "",        1,      "",         "1",        "1"        },
            { L_,   "+",       2,      "+",        "2",        "+2"       },
            { L_,   "-",       3,      "",         "3",        "3"        },
            { L_,   " ",       4,      " ",        "4",        " 4"       },
            { L_,   "#05x",    5,      "0x",       "5",        "0x005"    },
            { L_,   "#05X",    6,      "0X",       "6",        "0X006"    },
            { L_,   "*<6",     7,      "",         "7",        "7*****"   },
            { L_,   "*>6",     8,      "",         "8",        "*****8"   },
            { L_,   "*^6",     9,      "",         "9",        "**9***"   },
            { L_,   "*^+6",    10,     "+",        "10",       "*+10**"   },
            { L_,   "#0x",     11,     "0x",       "b",        "0xb"      },
            { L_,   "#01x",    12,     "0x",       "c",        "0xc"      },
            { L_,   "#02x",    13,     "0x",       "d",        "0xd"      },
            { L_,   "#03x",    14,     "0x",       "e",        "0xe"      },
            { L_,   "#04x",    15,     "0x",       "f",        "0x0f"     },
            { L_,   "#04X",    16,     "0X",       "10",       "0X10"     },
            { L_,   "#b",      17,     "0b",       "10001",    "0b10001"  },
            { L_,   "#05b",    18,     "0b",       "10010",    "0b10010"  },
            { L_,   "#06b",    19,     "0b",       "10011",    "0b10011"  },
            { L_,   "#06B",    20,     "0B",       "10100",    "0B10100"  },
            { L_,   "#d",      21,     "",         "21",       "21"       },
            { L_,   "#o",      22,     "0",        "26",       "026"      },
            { L_,   "B",       23,     "",         "10111",    "10111"    },
            { L_,   "x",       24,     "",         "18",       "18"       },
            { L_,   "X",       25,     "",         "19",       "19"       },
            { L_,   "6b",      26,     "",         "11010",    " 11010"   },
            { L_,   "6B",      27,     "",         "11011",    " 11011"   },
            { L_,   "6d",      28,     "",         "28",       "    28"   },
            { L_,   "6o",      29,     "",         "35",       "    35"   },
            { L_,   "6x",      30,     "",         "1e",       "    1e"   },
            { L_,   "6X",      31,     "",         "1F",       "    1F"   },
            { L_,   "c",       32,     "",         " ",        " "        },
            { L_,   "6c",      33,     "",         "!",        "!     "   },
            { L_,   "#<5x",    34,     "",         "22",       "22###"    },
        };

        size_t NUM_CHAR_DATA = sizeof CHAR_DATA / sizeof *CHAR_DATA;

        static const struct {
            int            d_line;      // source line number
            const wchar_t *d_char_p;    // format specification
            int            d_value ;    // value to format
            const char    *d_prefix_p;  // expected prefix
            const wchar_t *d_value_p;   // expected formatted value
            const wchar_t *d_result_p;  // expected format result
        } WCHAR_DATA[] = {
            //LINE  FORMAT      VALUE   EXP_PREFIX  EXP_VALUE    EXP_RESULT
            //----  -------     -----   ----------  ---------    ----------
            { L_,   L"",        1,      "",         L"1",        L"1"        },
            { L_,   L"+",       2,      "+",        L"2",        L"+2"       },
            { L_,   L"-",       3,      "",         L"3",        L"3"        },
            { L_,   L" ",       4,      " ",        L"4",        L" 4"       },
            { L_,   L"#05x",    5,      "0x",       L"5",        L"0x005"    },
            { L_,   L"#05X",    6,      "0X",       L"6",        L"0X006"    },
            { L_,   L"*<6",     7,      "",         L"7",        L"7*****"   },
            { L_,   L"*>6",     8,      "",         L"8",        L"*****8"   },
            { L_,   L"*^6",     9,      "",         L"9",        L"**9***"   },
            { L_,   L"*^+6",    10,     "+",        L"10",       L"*+10**"   },
            { L_,   L"#0x",     11,     "0x",       L"b",        L"0xb"      },
            { L_,   L"#01x",    12,     "0x",       L"c",        L"0xc"      },
            { L_,   L"#02x",    13,     "0x",       L"d",        L"0xd"      },
            { L_,   L"#03x",    14,     "0x",       L"e",        L"0xe"      },
            { L_,   L"#04x",    15,     "0x",       L"f",        L"0x0f"     },
            { L_,   L"#04X",    16,     "0X",       L"10",       L"0X10"     },
            { L_,   L"#b",      17,     "0b",       L"10001",    L"0b10001"  },
            { L_,   L"#05b",    18,     "0b",       L"10010",    L"0b10010"  },
            { L_,   L"#06b",    19,     "0b",       L"10011",    L"0b10011"  },
            { L_,   L"#06B",    20,     "0B",       L"10100",    L"0B10100"  },
            { L_,   L"#d",      21,     "",         L"21",       L"21"       },
            { L_,   L"#o",      22,     "0",        L"26",       L"026"      },
            { L_,   L"B",       23,     "",         L"10111",    L"10111"    },
            { L_,   L"x",       24,     "",         L"18",       L"18"       },
            { L_,   L"X",       25,     "",         L"19",       L"19"       },
            { L_,   L"6b",      26,     "",         L"11010",    L" 11010"   },
            { L_,   L"6B",      27,     "",         L"11011",    L" 11011"   },
            { L_,   L"6d",      28,     "",         L"28",       L"    28"   },
            { L_,   L"6o",      29,     "",         L"35",       L"    35"   },
            { L_,   L"6x",      30,     "",         L"1e",       L"    1e"   },
            { L_,   L"6X",      31,     "",         L"1F",       L"    1F"   },
            { L_,   L"c",       32,     "",         L" ",        L" "        },
            { L_,   L"6c",      33,     "",         L"!",        L"!     "   },
            { L_,   L"#<5x",    34,     "",         L"22",       L"22###"    },
        };

        size_t NUM_WCHAR_DATA = sizeof WCHAR_DATA / sizeof *WCHAR_DATA;

        for (size_t i = 0; i < NUM_CHAR_DATA; ++i) {
            const int               LINE       = CHAR_DATA[i].d_line;
            const bsl::string_view  FORMAT     = CHAR_DATA[i].d_char_p;
            const int               VALUE      = CHAR_DATA[i].d_value;
            const bsl::string_view  EXP_PREFIX = CHAR_DATA[i].d_prefix_p;
            const bsl::string_view  EXP_VALUE  = CHAR_DATA[i].d_value_p;
            const bsl::string_view  EXP_RESULT = CHAR_DATA[i].d_result_p;

            CharParseContext parseContextBase(FORMAT);
            CharParseContext parseContextSpec(FORMAT);

            bslfmt::FormatterIntegralBase<int, char>         mX;
            const bslfmt::FormatterIntegralBase<int, char>&  X = mX;
            CharSpec                                         spec;

            CharParseContext::iterator iterBase = mX.parse(parseContextBase);
            spec.parse(&parseContextSpec, CharSpec::e_CATEGORY_INTEGRAL);

            CharParseContext::iterator iterSpec = parseContextSpec.begin();

            ASSERTV(LINE, FORMAT.data(), iterSpec == iterBase);

            const CharSpec& baseSpec = X.specification();
            ASSERTV(LINE,
                    bsl::string_view(baseSpec.filler(),
                                     baseSpec.numFillerCharacters()) ==
                        bsl::string_view(spec.filler(),
                                         spec.numFillerCharacters()));
            ASSERTV(LINE, baseSpec.alignment() == spec.alignment());
            ASSERTV(LINE, baseSpec.sign() == spec.sign());
            ASSERTV(LINE,
                    baseSpec.alternativeFlag() == spec.alternativeFlag());
            ASSERTV(LINE,
                    baseSpec.zeroPaddingFlag() == spec.zeroPaddingFlag());
            ASSERTV(
                   LINE,
                   baseSpec.localeSpecificFlag() == spec.localeSpecificFlag());
            ASSERTV(LINE, baseSpec.formatType() == spec.formatType());

            // Testing `formatPrefix`

            char              prefixBuffer[8];
            char             *prefixEnd = X.formatPrefix(prefixBuffer,
                                                         8,
                                                         VALUE);
            bsl::string_view  prefix(prefixBuffer, prefixEnd);

            ASSERTV(LINE, EXP_PREFIX.data(), prefix.data(),
                    EXP_PREFIX == prefix);


            // Testing `formatValue`

            char              valueBuffer[65];
            char             *valueEnd = X.formatValue(valueBuffer,
                                                       65,
                                                       VALUE);
            bsl::string_view  value(valueBuffer, valueEnd);

            ASSERTV(LINE, EXP_VALUE.data(), value.data(),
                    EXP_VALUE == value);

            // Testing `outputValue`

            bslfmt::MockFormatContext<char> formatContext(0);

            formatContext.advance_to(X.outputValue(prefixBuffer,
                                                   prefixEnd,
                                                   valueBuffer,
                                                   valueEnd,
                                                   formatContext));
            bsl::string_view result = formatContext.finalString();
            ASSERTV(LINE, EXP_RESULT.data(), result.data(),
                    EXP_RESULT == result);
        }


        for (size_t i = 0; i < NUM_WCHAR_DATA; ++i) {
            const int                LINE       = WCHAR_DATA[i].d_line;
            const bsl::wstring_view  FORMAT     = WCHAR_DATA[i].d_char_p;
            const int                VALUE      = WCHAR_DATA[i].d_value;
            const bsl::string_view   EXP_PREFIX = WCHAR_DATA[i].d_prefix_p;
            const bsl::wstring_view  EXP_VALUE  = WCHAR_DATA[i].d_value_p;
            const bsl::wstring_view  EXP_RESULT = WCHAR_DATA[i].d_result_p;

            WcharParseContext parseContextBase(FORMAT);
            WcharParseContext parseContextSpec(FORMAT);

            bslfmt::FormatterIntegralBase<int, wchar_t>         mX;
            const bslfmt::FormatterIntegralBase<int, wchar_t>&  X = mX;
            WcharSpec                                           spec;

            WcharParseContext::iterator iterBase = mX.parse(parseContextBase);
            spec.parse(&parseContextSpec, WcharSpec::e_CATEGORY_INTEGRAL);

            WcharParseContext::iterator iterSpec = parseContextSpec.begin();

            ASSERTV(LINE, FORMAT.data(), iterSpec == iterBase);

            const WcharSpec& baseSpec = X.specification();
            ASSERTV(LINE,
                    bsl::wstring_view(baseSpec.filler(),
                                      baseSpec.numFillerCharacters()) ==
                        bsl::wstring_view(spec.filler(),
                                          spec.numFillerCharacters()));
            ASSERTV(LINE, baseSpec.alignment() == spec.alignment());
            ASSERTV(LINE, baseSpec.sign() == spec.sign());
            ASSERTV(LINE,
                    baseSpec.alternativeFlag() == spec.alternativeFlag());
            ASSERTV(LINE,
                    baseSpec.zeroPaddingFlag() == spec.zeroPaddingFlag());
            ASSERTV(
                   LINE,
                   baseSpec.localeSpecificFlag() == spec.localeSpecificFlag());
            ASSERTV(LINE, baseSpec.formatType() == spec.formatType());

            // Testing `formatPrefix`

            char              prefixBuffer[8];
            char             *prefixEnd = X.formatPrefix(prefixBuffer,
                                                         8,
                                                         VALUE);
            bsl::string_view  prefix(prefixBuffer, prefixEnd);

            ASSERTV(LINE, EXP_PREFIX.data(), prefix.data(),
                    EXP_PREFIX == prefix);


            // Testing `formatValue`

            wchar_t            valueBuffer[65];
            wchar_t           *valueEnd = X.formatValue(valueBuffer,
                                                        65,
                                                        VALUE);
            bsl::wstring_view  value(valueBuffer, valueEnd);

            ASSERTV(LINE, EXP_VALUE.data(), value.data(),
                    EXP_VALUE == value);

            // Testing `outputValue`

            bslfmt::MockFormatContext<wchar_t> formatContext(0);

            formatContext.advance_to(X.outputValue(prefixBuffer,
                                                   prefixEnd,
                                                   valueBuffer,
                                                   valueEnd,
                                                   formatContext));
            bsl::wstring_view result = formatContext.finalString();
            ASSERTV(LINE, EXP_RESULT.data(), result.data(),
                    EXP_RESULT == result);
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
// Copyright 2025 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS",BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
