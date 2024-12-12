// bslfmt_formatterpointer.t.cpp                                      -*-C++-*-
#include <bslfmt_formatterpointer.h>

#include <bslfmt_formattertestutil.h>

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
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Use test contexts to format a single pointer.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

///Example: Formatting a pointer
///- - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test pointer formatter's ability to format a pointer with
// defined alignment and padding.
// ```
    bslfmt::MockParseContext<char> mpc("*<6p", 1);

    bsl::formatter<const void *, char> f;
    mpc.advance_to(f.parse(mpc));

    const void *value = 0;

    bslfmt::MockFormatContext<char> mfc(value, 0, 0);

    mfc.advance_to(bsl::as_const(f).format(value, mfc));

    ASSERT("0x0***" == mfc.finalString());
// ```
      } break;
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

        void           *ptr        = reinterpret_cast<void *>(0x13579BDF);
        void           *nullPtr    = 0;
        bsl::nullptr_t  nullptrObj = bsl::nullptr_t();

        // Unlike the other compilers (gcc and clang), msvc supports the
        // specifications allowed in the `c++26` standard when compiling with
        // the `c++20` flag.  This allows us to perform additional checking of
        // our implementation.

        const bool oracleRequired =
#if  defined(BSLS_PLATFORM_CMP_MSVC) ||                                       \
     defined(BSLS_LIBRARYFEATURES_HAS_CPP26_BASELINE_LIBRARY)
                                    true;
#else
                                    false;
#endif

        static const struct {
            int         d_line;        // source line number
            const char *d_format_p;    // format spec
            const char *d_expected_p;  // format
            void       *d_value_p;     // value to be formatted
            bool        d_runOracle;   // should example be tested with oracle
        } CHAR_DATA[] = {
            //LINE  FORMAT       EXPECTED          VALUE      ORACLE
            //----  -----------  ---------------   -------    --------------
            { L_,   "{:}",       "0x0",            nullPtr,   true           },
            { L_,   "{0:}",      "0x0",            nullPtr,   true           },
            { L_,   "{:p}",      "0x0",            nullPtr,   true           },
            { L_,   "{0:p}",     "0x0",            nullPtr,   true           },
            { L_,   "{:01}",     "0x0",            nullPtr,   oracleRequired },
            { L_,   "{:012}",    "0x0000000000",   nullPtr,   oracleRequired },
            { L_,   "{:<12}",    "0x0         ",   nullPtr,   true           },
            { L_,   "{:>12}",    "         0x0",   nullPtr,   true           },
            { L_,   "{:^12}",    "    0x0     ",   nullPtr,   true           },
            { L_,   "{:<012}",   "0x0         ",   nullPtr,   oracleRequired },
            { L_,   "{:>012}",   "         0x0",   nullPtr,   oracleRequired },
            { L_,   "{:^012}",   "    0x0     ",   nullPtr,   oracleRequired },
            { L_,   "{:*<12}",   "0x0*********",   nullPtr,   true           },
            { L_,   "{:*>12}",   "*********0x0",   nullPtr,   true           },
            { L_,   "{:*^12}",   "****0x0*****",   nullPtr,   true           },
            { L_,   "{:*<12p}",  "0x0*********",   nullPtr,   true           },
            { L_,   "{:*>12p}",  "*********0x0",   nullPtr,   true           },
            { L_,   "{:*^12p}",  "****0x0*****",   nullPtr,   true           },

            { L_,   "{:}",       "0x13579bdf",     ptr,       true           },
            { L_,   "{0:}",      "0x13579bdf",     ptr,       true           },
            { L_,   "{:p}",      "0x13579bdf",     ptr,       true           },
            { L_,   "{0:p}",     "0x13579bdf",     ptr,       true           },
            { L_,   "{:01}",     "0x13579bdf",     ptr,       oracleRequired },
            { L_,   "{:012}",    "0x0013579bdf",   ptr,       oracleRequired },
            { L_,   "{:<12}",    "0x13579bdf  ",   ptr,       true           },
            { L_,   "{:>12}",    "  0x13579bdf",   ptr,       true           },
            { L_,   "{:^12}",    " 0x13579bdf ",   ptr,       true           },
            { L_,   "{:<012}",   "0x13579bdf  ",   ptr,       oracleRequired },
            { L_,   "{:>012}",   "  0x13579bdf",   ptr,       oracleRequired },
            { L_,   "{:^012}",   " 0x13579bdf ",   ptr,       oracleRequired },
            { L_,   "{:*<12}",   "0x13579bdf**",   ptr,       true           },
            { L_,   "{:*>12}",   "**0x13579bdf",   ptr,       true           },
            { L_,   "{:*^12}",   "*0x13579bdf*",   ptr,       true           },
            { L_,   "{:*<12p}",  "0x13579bdf**",   ptr,       true           },
            { L_,   "{:*>12p}",  "**0x13579bdf",   ptr,       true           },
            { L_,   "{:*^12p}",  "*0x13579bdf*",   ptr,       true           },
        };

        enum { NUM_CHAR_DATA = sizeof CHAR_DATA / sizeof *CHAR_DATA };


        static const struct {
            int            d_line;        // source line number
            const wchar_t *d_format_p;    // format spec
            const wchar_t *d_expected_p;  // format
            void          *d_value_p;     // value to be formatted
            bool           d_runOracle;   // should be tested with oracle
        } WCHAR_DATA[] = {
            //LINE  FORMAT       EXPECTED          VALUE      ORACLE
            //----  -----------  ---------------   -------    --------------
            { L_,   L"{:}",      L"0x0",           nullPtr,   true           },
            { L_,   L"{0:}",     L"0x0",           nullPtr,   true           },
            { L_,   L"{:p}",     L"0x0",           nullPtr,   true           },
            { L_,   L"{0:p}",    L"0x0",           nullPtr,   true           },
            { L_,   L"{:01}",    L"0x0",           nullPtr,   oracleRequired },
            { L_,   L"{:012}",   L"0x0000000000",  nullPtr,   oracleRequired },
            { L_,   L"{:<12}",   L"0x0         ",  nullPtr,   true           },
            { L_,   L"{:>12}",   L"         0x0",  nullPtr,   true           },
            { L_,   L"{:^12}",   L"    0x0     ",  nullPtr,   true           },
            { L_,   L"{:<012}",  L"0x0         ",  nullPtr,   oracleRequired },
            { L_,   L"{:>012}",  L"         0x0",  nullPtr,   oracleRequired },
            { L_,   L"{:^012}",  L"    0x0     ",  nullPtr,   oracleRequired },
            { L_,   L"{:*<12}",  L"0x0*********",  nullPtr,   true           },
            { L_,   L"{:*>12}",  L"*********0x0",  nullPtr,   true           },
            { L_,   L"{:*^12}",  L"****0x0*****",  nullPtr,   true           },
            { L_,   L"{:*<12p}", L"0x0*********",  nullPtr,   true           },
            { L_,   L"{:*>12p}", L"*********0x0",  nullPtr,   true           },
            { L_,   L"{:*^12p}", L"****0x0*****",  nullPtr,   true           },

            { L_,   L"{:}",      L"0x13579bdf",    ptr,       true           },
            { L_,   L"{0:}",     L"0x13579bdf",    ptr,       true           },
            { L_,   L"{:p}",     L"0x13579bdf",    ptr,       true           },
            { L_,   L"{0:p}",    L"0x13579bdf",    ptr,       true           },
            { L_,   L"{:01}",    L"0x13579bdf",    ptr,       oracleRequired },
            { L_,   L"{:012}",   L"0x0013579bdf",  ptr,       oracleRequired },
            { L_,   L"{:<12}",   L"0x13579bdf  ",  ptr,       true           },
            { L_,   L"{:>12}",   L"  0x13579bdf",  ptr,       true           },
            { L_,   L"{:^12}",   L" 0x13579bdf ",  ptr,       true           },
            { L_,   L"{:<012}",  L"0x13579bdf  ",  ptr,       oracleRequired },
            { L_,   L"{:>012}",  L"  0x13579bdf",  ptr,       oracleRequired },
            { L_,   L"{:^012}",  L" 0x13579bdf ",  ptr,       oracleRequired },
            { L_,   L"{:*<12}",  L"0x13579bdf**",  ptr,       true           },
            { L_,   L"{:*>12}",  L"**0x13579bdf",  ptr,       true           },
            { L_,   L"{:*^12}",  L"*0x13579bdf*",  ptr,       true           },
            { L_,   L"{:*<12p}", L"0x13579bdf**",  ptr,       true           },
            { L_,   L"{:*>12p}", L"**0x13579bdf",  ptr,       true           },
            { L_,   L"{:*^12p}", L"*0x13579bdf*",  ptr,       true           },
        };

        enum { NUM_WCHAR_DATA = sizeof WCHAR_DATA / sizeof *WCHAR_DATA };

        if (verbose) printf("\tTesting runtime processing.\n");

        for (int i = 0; i < NUM_CHAR_DATA; ++i) {
            const int   LINE            = CHAR_DATA[i].d_line;
            const char *FORMAT          = CHAR_DATA[i].d_format_p;
            const char *EXPECTED        = CHAR_DATA[i].d_expected_p;
                  void *VALUE           = CHAR_DATA[i].d_value_p;
            const void *CVALUE          = CHAR_DATA[i].d_value_p;
            const bool  ORACLE_REQUIRED = CHAR_DATA[i].d_runOracle;

            bsl::string message;

            bool rv = TestUtilChar::testParseVFormat<const void *>(&message,
                                                                   false,
                                                                   FORMAT);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);

            rv = TestUtilChar::testEvaluateVFormat(&message,
                                                   EXPECTED,
                                                   ORACLE_REQUIRED,
                                                   FORMAT,
                                                   VALUE);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);

            rv = TestUtilChar::testEvaluateVFormat(&message,
                                                   EXPECTED,
                                                   ORACLE_REQUIRED,
                                                   FORMAT,
                                                   CVALUE);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);

            if (!VALUE) {
                // We expect the same result for the `nullptr_t` object as for
                // a pointer having null value.

                rv = TestUtilChar::testEvaluateVFormat(&message,
                                                       EXPECTED,
                                                       ORACLE_REQUIRED,
                                                       FORMAT,
                                                       nullptrObj);
                ASSERTV(LINE, FORMAT, message.c_str(), rv);
            }
        }

        for (int i = 0; i < NUM_WCHAR_DATA; ++i) {
            const int      LINE            = WCHAR_DATA[i].d_line;
            const wchar_t *FORMAT          = WCHAR_DATA[i].d_format_p;
            const wchar_t *EXPECTED        = WCHAR_DATA[i].d_expected_p;
                  void    *VALUE           = WCHAR_DATA[i].d_value_p;
            const void    *CVALUE          = WCHAR_DATA[i].d_value_p;
            const bool     ORACLE_REQUIRED = WCHAR_DATA[i].d_runOracle;

            bsl::string message;

            bool rv = TestUtilWchar::testParseVFormat<const void *>(&message,
                                                                    false,
                                                                    FORMAT);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateVFormat(&message,
                                                    EXPECTED,
                                                    ORACLE_REQUIRED,
                                                    FORMAT,
                                                    VALUE);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateVFormat(&message,
                                                    EXPECTED,
                                                    ORACLE_REQUIRED,
                                                    FORMAT,
                                                    CVALUE);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);

            if (!VALUE) {
                // We expect the same result for the `nullptr_t` object as for
                // a pointer having null value.

                rv = TestUtilWchar::testEvaluateVFormat(&message,
                                                        EXPECTED,
                                                        ORACLE_REQUIRED,
                                                        FORMAT,
                                                        nullptrObj);
                ASSERTV(LINE, FORMAT, message.c_str(), rv);
            }
        }

        if (verbose) printf("\tTesting compile-time processing.\n");
        {
            bsl::string message;

            // `parse`

            bool rv = TestUtilChar::testParseFormat<void *>(&message,
                                                            true,
                                                            "{0:}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilChar::testParseFormat<const void *>(&message,
                                                             true,
                                                             "{0:}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilChar::testParseFormat<bsl::nullptr_t>(&message,
                                                               true,
                                                               "{0:}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<void *>(&message,
                                                        true,
                                                        L"{0:}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<const void *>(&message,
                                                              true,
                                                              L"{0:}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<bsl::nullptr_t>(&message,
                                                                true,
                                                                L"{0:}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilChar::testParseFormat<void *>(&message,
                                                       true,
                                                       "{:*<6}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilChar::testParseFormat<const void *>(&message,
                                                             true,
                                                             "{:*<6}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilChar::testParseFormat<bsl::nullptr_t>(&message,
                                                               true,
                                                               "{:*<6}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<void *>(&message,
                                                        true,
                                                        L"{:*<6}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<const void *>(&message,
                                                              true,
                                                              L"{:*<6}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<bsl::nullptr_t>(&message,
                                                                true,
                                                                L"{:*<6}");
            ASSERTV(message.c_str(), rv);

            // `format`

            void       * const VALUE  = 0;
            const void * const CVALUE = 0;
            rv = TestUtilChar::testEvaluateFormat(&message,
                                                  "0x0",
                                                  true,
                                                  "{:}",
                                                  VALUE);
            ASSERTV(message.c_str(), rv);
            rv = TestUtilChar::testEvaluateFormat(&message,
                                                  "0x0",
                                                  true,
                                                  "{:}",
                                                  CVALUE);
            ASSERTV(message.c_str(), rv);
            rv = TestUtilChar::testEvaluateFormat(&message,
                                                  "0x0",
                                                  true,
                                                  "{:}",
                                                  nullptrObj);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"0x0",
                                                   true,
                                                   L"{:}",
                                                   VALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"0x0",
                                                   true,
                                                   L"{:}",
                                                   CVALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"0x0",
                                                   true,
                                                   L"{:}",
                                                   nullptrObj);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilChar::testEvaluateFormat(&message,
                                                  "0x0***",
                                                  true,
                                                  "{:*<6}",
                                                  VALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilChar::testEvaluateFormat(&message,
                                                  "0x0***",
                                                  true,
                                                  "{:*<6}",
                                                  CVALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilChar::testEvaluateFormat(&message,
                                                  "0x0***",
                                                  true,
                                                  "{:*<6}",
                                                  nullptrObj);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"0x0***",
                                                   true,
                                                   L"{:*<6}",
                                                   VALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"0x0***",
                                                   true,
                                                   L"{:*<6}",
                                                   CVALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"0x0***",
                                                   true,
                                                   L"{:*<6}",
                                                   nullptrObj);
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
