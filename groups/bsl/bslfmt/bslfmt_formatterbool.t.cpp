// bslfmt_formatterbool.t.cpp                                         -*-C++-*-
#include <bslfmt_formatterbool.h>

#include <bslfmt_formattertestutil.h>

#include <bslmf_isaccessiblebaseof.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>

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

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslfmt::FormatterTestUtil<char>    TestUtilChar;
typedef bslfmt::FormatterTestUtil<wchar_t> TestUtilWchar;

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

/// Check whether the `bslfmt::formatter<t_TYPE, t_CHAR>::format` function
/// produces the specified `expected` result for the specified `format`,
/// `width` and `value`.  The specified `line` is used to identify the function
/// call location.
template <class t_CHAR, class t_TYPE>
void testWidthRuntimeFormat(int           line,
                            const t_CHAR *expected,
                            const t_CHAR *format,
                            int           width,
                            t_TYPE        value)
{
    bsl::string message;

    bool rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(&message,
                                                                     expected,
                                                                     true,
                                                                     format,
                                                                     value,
                                                                     width);
    ASSERTV(line, format, message.c_str(), rv);
}

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        // 1. Demonstrate the functioning of this component.
        //
        // Plan:
        // 1. Use test contexts to format a single boolean.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

///Example: Formatting a bool
/// - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test this formatter's ability to format a boolean with
// defined alignment and padding.
// ```
    bslfmt::MockParseContext<char> mpc("*<6s", 1);

    bsl::formatter<bool, char> f;
    mpc.advance_to(f.parse(mpc));

    bool value = false;

    bslfmt::MockFormatContext<char> mfc(value, 0, 0);

    mfc.advance_to(bsl::as_const(f).format(value, mfc));

    ASSERT("false*" == mfc.finalString());
// ```
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // STD NON-DELEGATION
        //   This test case verifies that we do not hijack the `std` formatter.
        //   This test case is executed only when `std::format` is present and
        //   usable/used by BDE.
        //
        // Concern:
        // 1. When `std::format` is present and used/usable the component
        //    `bslfmt_formatterbase` defines a `std::formatter` partial
        //    specialization for all types that do not already have a standard
        //    formatter) that is inherited from (implemented in terms of) the
        //    `bsl::formatter` for that type.  However we do not want that
        //    `std::formatter` partial specialization to be active for types
        //    that do have a formatter in the standard library, such as `bool`.
        //
        // Plan:
        // 1. Verify that `std::formatter` for `bool` (`char` and `wchar_t`)
        //    are not inherited from `bsl::formatter` of the same template
        //    parameters.
        //
        // Testing:
        //   STD NON-DELEGATION
        // --------------------------------------------------------------------

        if (verbose) printf("\nSTD NON-DELEGATION"
                            "\n==================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
        ASSERT(
             (!bslmf::IsAccessibleBaseOf<bsl::formatter<bool, char>,
                                         std::formatter<bool, char> >::value));

        ASSERT((!bslmf::IsAccessibleBaseOf<
                bsl::formatter<bool, wchar_t>,
                std::formatter<bool, wchar_t> >::value));
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        static const struct {
            int         d_line;        // source line number
            const char *d_format_p;    // format spec
            const char *d_expected_p;  // format
            bool        d_value;       // value to be formatted
        } CHAR_DATA[] = {
            //LINE  FORMAT        EXPECTED      VALUE
            //----  -----------   ---------     -----
            { L_,   "{:}",        "true",       true   },
            { L_,   "{0:}",       "true",       true   },
            { L_,   "{:6s}",      "true  ",     true   },
            { L_,   "{:*<6}",     "true**",     true   },
            { L_,   "{:*>6}",     "**true",     true   },
            { L_,   "{:*^6}",     "*true*",     true   },
            { L_,   "{:*^6s}",    "*true*",     true   },
            { L_,   "{:#0x}",     "0x1",        true   },
            { L_,   "{:#01x}",    "0x1",        true   },
            { L_,   "{:#02x}",    "0x1",        true   },
            { L_,   "{:#03x}",    "0x1",        true   },
            { L_,   "{:#04x}",    "0x01",       true   },
            { L_,   "{:#05X}",    "0X001",      true   },
            { L_,   "{:#b}",      "0b1",        true   },
            { L_,   "{:#05b}",    "0b001",      true   },
            { L_,   "{:#06b}",    "0b0001",     true   },
            { L_,   "{:#06B}",    "0B0001",     true   },
            { L_,   "{:#d}",      "1",          true   },
            { L_,   "{:#o}",      "01",         true   },
            { L_,   "{:#x}",      "0x1",        true   },
            { L_,   "{:#X}",      "0X1",        true   },
            { L_,   "{:b}",       "1",          true   },
            { L_,   "{:B}",       "1",          true   },
            { L_,   "{:d}",       "1",          true   },
            { L_,   "{:o}",       "1",          true   },
            { L_,   "{:x}",       "1",          true   },
            { L_,   "{:X}",       "1",          true   },
            { L_,   "{:s}",       "true",       true   },
            { L_,   "{:05x}",     "00001",      true   },
            { L_,   "{:05b}",     "00001",      true   },
            { L_,   "{:#<5x}",    "1####",      true   },
            { L_,   "{:0<5x}",    "10000",      true   },
            // false
            { L_,   "{:}",        "false",      false  },
            { L_,   "{0:}",       "false",      false  },
            { L_,   "{:6s}",      "false ",     false  },
            { L_,   "{:*<6}",     "false*",     false  },
            { L_,   "{:*>6}",     "*false",     false  },
            { L_,   "{:*^6}",     "false*",     false  },
            { L_,   "{:*^6s}",    "false*",     false  },
            { L_,   "{:#0x}",     "0x0",        false  },
            { L_,   "{:#01x}",    "0x0",        false  },
            { L_,   "{:#02x}",    "0x0",        false  },
            { L_,   "{:#03x}",    "0x0",        false  },
            { L_,   "{:#04x}",    "0x00",       false  },
            { L_,   "{:#05X}",    "0X000",      false  },
            { L_,   "{:#b}",      "0b0",        false  },
            { L_,   "{:#05b}",    "0b000",      false  },
            { L_,   "{:#06b}",    "0b0000",     false  },
            { L_,   "{:#06B}",    "0B0000",     false  },
            { L_,   "{:#d}",      "0",          false  },
            { L_,   "{:#o}",      "0",          false  },
            { L_,   "{:#x}",      "0x0",        false  },
            { L_,   "{:#X}",      "0X0",        false  },
            { L_,   "{:b}",       "0",          false  },
            { L_,   "{:B}",       "0",          false  },
            { L_,   "{:d}",       "0",          false  },
            { L_,   "{:o}",       "0",          false  },
            { L_,   "{:x}",       "0",          false  },
            { L_,   "{:X}",       "0",          false  },
            { L_,   "{:s}",       "false",      false  },
            { L_,   "{:05x}",     "00000",      false  },
            { L_,   "{:05b}",     "00000",      false  },
            { L_,   "{:#<5x}",    "0####",      false  },
            { L_,   "{:0<5x}",    "00000",      false  },
        };

        size_t NUM_CHAR_DATA = sizeof CHAR_DATA / sizeof *CHAR_DATA;

        static const struct {
            int            d_line;        // source line number
            const wchar_t *d_format_p;    // format spec
            const wchar_t *d_expected_p;  // format
            bool           d_value;       // value to be formatted
        } WCHAR_DATA[] = {
            //LINE  FORMAT         EXPECTED       VALUE
            //----  ------------   ----------     -----
            { L_,   L"{:}",        L"true",       true   },
            { L_,   L"{0:}",       L"true",       true   },
            { L_,   L"{:*<6}",     L"true**",     true   },
            { L_,   L"{:*>6}",     L"**true",     true   },
            { L_,   L"{:*^6}",     L"*true*",     true   },
            { L_,   L"{:*^6s}",    L"*true*",     true   },
            { L_,   L"{:#0x}",     L"0x1",        true   },
            { L_,   L"{:#01x}",    L"0x1",        true   },
            { L_,   L"{:#02x}",    L"0x1",        true   },
            { L_,   L"{:#03x}",    L"0x1",        true   },
            { L_,   L"{:#04x}",    L"0x01",       true   },
            { L_,   L"{:#05X}",    L"0X001",      true   },
            { L_,   L"{:#b}",      L"0b1",        true   },
            { L_,   L"{:#05b}",    L"0b001",      true   },
            { L_,   L"{:#06b}",    L"0b0001",     true   },
            { L_,   L"{:#06B}",    L"0B0001",     true   },
            { L_,   L"{:#d}",      L"1",          true   },
            { L_,   L"{:#o}",      L"01",         true   },
            { L_,   L"{:#x}",      L"0x1",        true   },
            { L_,   L"{:#X}",      L"0X1",        true   },
            { L_,   L"{:b}",       L"1",          true   },
            { L_,   L"{:B}",       L"1",          true   },
            { L_,   L"{:d}",       L"1",          true   },
            { L_,   L"{:o}",       L"1",          true   },
            { L_,   L"{:x}",       L"1",          true   },
            { L_,   L"{:X}",       L"1",          true   },
            { L_,   L"{:s}",       L"true",       true   },
            { L_,   L"{:05x}",     L"00001",      true   },
            { L_,   L"{:05b}",     L"00001",      true   },
            { L_,   L"{:#<5x}",    L"1####",      true   },
            { L_,   L"{:0<5x}",    L"10000",      true   },
            // false
            { L_,   L"{:}",        L"false",      false  },
            { L_,   L"{0:}",       L"false",      false  },
            { L_,   L"{:*<6}",     L"false*",     false  },
            { L_,   L"{:*>6}",     L"*false",     false  },
            { L_,   L"{:*^6}",     L"false*",     false  },
            { L_,   L"{:*^6s}",    L"false*",     false  },
            { L_,   L"{:#0x}",     L"0x0",        false  },
            { L_,   L"{:#01x}",    L"0x0",        false  },
            { L_,   L"{:#02x}",    L"0x0",        false  },
            { L_,   L"{:#03x}",    L"0x0",        false  },
            { L_,   L"{:#04x}",    L"0x00",       false  },
            { L_,   L"{:#05X}",    L"0X000",      false  },
            { L_,   L"{:#b}",      L"0b0",        false  },
            { L_,   L"{:#05b}",    L"0b000",      false  },
            { L_,   L"{:#06b}",    L"0b0000",     false  },
            { L_,   L"{:#06B}",    L"0B0000",     false  },
            { L_,   L"{:#d}",      L"0",          false  },
            { L_,   L"{:#o}",      L"0",          false  },
            { L_,   L"{:#x}",      L"0x0",        false  },
            { L_,   L"{:#X}",      L"0X0",        false  },
            { L_,   L"{:b}",       L"0",          false  },
            { L_,   L"{:B}",       L"0",          false  },
            { L_,   L"{:d}",       L"0",          false  },
            { L_,   L"{:o}",       L"0",          false  },
            { L_,   L"{:x}",       L"0",          false  },
            { L_,   L"{:X}",       L"0",          false  },
            { L_,   L"{:s}",       L"false",      false  },
            { L_,   L"{:05x}",     L"00000",      false  },
            { L_,   L"{:05b}",     L"00000",      false  },
            { L_,   L"{:#<5x}",    L"0####",      false  },
            { L_,   L"{:0<5x}",    L"00000",      false  },
        };

        size_t NUM_WCHAR_DATA = sizeof WCHAR_DATA / sizeof *WCHAR_DATA;

        if (verbose) printf("\tTesting runtime processing.\n");

        for (size_t i = 0; i < NUM_CHAR_DATA; ++i) {
            const int   LINE     = CHAR_DATA[i].d_line;
            const char *FORMAT   = CHAR_DATA[i].d_format_p;
            const char *EXPECTED = CHAR_DATA[i].d_expected_p;
            const bool  VALUE    = CHAR_DATA[i].d_value;

            bsl::string message;

            bool rv = TestUtilChar::testParseVFormat<bool>(&message,
                                                           false,
                                                           FORMAT);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);

            rv = TestUtilChar::testEvaluateVFormat(&message,
                                                   EXPECTED,
                                                   true,
                                                   FORMAT,
                                                   VALUE);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);
        }

        for (size_t i = 0; i < NUM_WCHAR_DATA; ++i) {
            const int      LINE     = WCHAR_DATA[i].d_line;
            const wchar_t *FORMAT   = WCHAR_DATA[i].d_format_p;
            const wchar_t *EXPECTED = WCHAR_DATA[i].d_expected_p;
            const bool     VALUE    = WCHAR_DATA[i].d_value;

            bsl::string message;
            bool        rv = TestUtilWchar::testParseVFormat<bool>(&message,
                                                                   false,
                                                                   FORMAT);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateVFormat(&message,
                                                    EXPECTED,
                                                    true,
                                                    FORMAT,
                                                    VALUE);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);
        }


        if (verbose) printf("\tTesting `width` option.\n");
        {
            bsl::string  message;
            bool         value    = true;
            int          width    = 5;

            // `char`

            const char  *format   = "{:{}}";
            const char  *expected = "true ";

            testWidthRuntimeFormat(L_, expected, format, width, value);

            format   = "{:{}s}";
            expected = "true ";

            testWidthRuntimeFormat(L_, expected, format, width, value);

            format   = "{:#>{}s}";
            expected = "#true";

            testWidthRuntimeFormat(L_, expected, format, width, value);

            // `wchar_t`

            value = false;
            width = 6;

            const wchar_t  *wFormat   = L"{:{}}";
            const wchar_t  *wExpected = L"false ";

            testWidthRuntimeFormat(L_, wExpected, wFormat, width, value);

            wFormat   = L"{:{}s}";
            wExpected = L"false ";

            testWidthRuntimeFormat(L_, wExpected, wFormat, width, value);

            wFormat   = L"{:#>{}s}";
            wExpected = L"#false";

            testWidthRuntimeFormat(L_, wExpected, wFormat, width, value);
        }

        if (verbose) printf("\tTesting compile-time processing.\n");
            {
            bsl::string message;

            // `parse`

            bool rv = TestUtilChar::testParseFormat<bool>(&message,
                                                          true,
                                                          "{0:}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<bool>(&message, true, L"{0:}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilChar::testParseFormat<bool>(&message, true, "{:*<6}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<bool>(&message,
                                                      true,
                                                      L"{:*<6}");
            ASSERTV(message.c_str(), rv);

            // `format`

            const bool VALUE = true;
            rv               = TestUtilChar::testEvaluateFormat(&message,
                                                                "true",
                                                                true,
                                                                "{:}",
                                                                VALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"true",
                                                   true,
                                                   L"{:}",
                                                   VALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilChar::testEvaluateFormat(&message,
                                                  "true**",
                                                  true,
                                                  "{:*<6}",
                                                  VALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"true**",
                                                   true,
                                                   L"{:*<6}",
                                                   VALUE);
            ASSERTV(message.c_str(), rv);
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
