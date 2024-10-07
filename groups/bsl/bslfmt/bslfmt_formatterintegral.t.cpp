// bslstl_formatterintegral.t.cpp                                     -*-C++-*-
#include <bslfmt_formatterintegral.h>

#include <bslfmt_formatargs.h>
#include <bslfmt_formattertestutil.h> // Testing only

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
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#define BSLFMT_FORMATTER_TEST_CONSTEVAL consteval
#else
#define BSLFMT_FORMATTER_TEST_CONSTEVAL
#endif

/// Check whether the `bslfmt::formatter<t_TYPE, t_CHAR>::parse` function works
/// as expected for the specified `format` string created at compile-time
/// (under c++20 only) for the specified (template parameter) value `t_TYPE`.
/// The specified `message` is used to pass a description of the error if one
/// occurs.  The specified `line` is used to identify the function call
/// location.
template <class t_CHAR, class t_TYPE>
BSLFMT_FORMATTER_TEST_CONSTEVAL void testParse(bsl::string  *message,
                                               const t_CHAR *format,
                                               const int     line)
{
    bool rv =
          bslfmt::Formatter_TestUtil<t_CHAR>::template testParseFormat<t_TYPE>(
              message,
              false,
              format);
    ASSERTV(line, format, message->c_str(), rv);
}

/// Check whether the `bslfmt::formatter<t_TYPE, t_CHAR>::parse` function works
/// as expected for the specified `format` string created at compile-time
/// (under c++20 only) for several integer types.  The specified `message` is
/// used to pass a description of the error if one occurs.  The specified `line`
/// is used to identify the function call location.
template <class t_CHAR>
BSLFMT_FORMATTER_TEST_CONSTEVAL void testCompileTimeParse(
                                                         bsl::string  *message,
                                                         const t_CHAR *format,
                                                         const int     line)
{
    testParse<t_CHAR,                    int>(message, format, line);
    testParse<t_CHAR,           unsigned int>(message, format, line);
    testParse<t_CHAR,               long int>(message, format, line);
    testParse<t_CHAR,      unsigned long int>(message, format, line);
    testParse<t_CHAR,          long long int>(message, format, line);
    testParse<t_CHAR, unsigned long long int>(message, format, line);
}

#undef BSLFMT_FORMATTER_TEST_CONSTEVAL

/// Check whether the `bslfmt::formatter<t_TYPE, t_CHAR>::parse` function works
/// as expected for the specified `format` string created at runtime.  The
/// specified `line` is used to identify the function call location.
template <class t_CHAR, class t_TYPE>
void testRuntimeParse(int line, const t_CHAR *format)
{
    bsl::string message;

    bool rv =
         bslfmt::Formatter_TestUtil<t_CHAR>::template testParseVFormat<t_TYPE>(
             &message,
             false,
             format);
    ASSERTV(line, format, message.c_str(), rv);
}

/// Check whether the `bslfmt::formatter<t_TYPE, t_CHAR>::format` function
/// produces the specified `expected` result for the specified `format` and
/// `value`.  The specified `line` is used to identify the function call
/// location.
template <class t_CHAR, class t_TYPE>
void testRuntimeFormat(int           line,
                       const t_CHAR *expected,
                       const t_CHAR *format,
                       t_TYPE        value)
{
    bsl::string message;
    int         arg1 = 0;

    bool rv = bslfmt::Formatter_TestUtil<t_CHAR>::testEvaluateVFormat(&message,
                                                                 expected,
                                                                 false,
                                                                 format,
                                                                 value,
                                                                 arg1,
                                                                 arg1);
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
        //: 1 Use test contexts to format a single string.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

///Example: Formatting an integer
/// - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test this formatter's ability to a substring with padding
// and minimum width.
//
//..
    bslfmt::Formatter_MockParseContext<char> mpc("*<5x", 1);

    bsl::formatter<int, char> f;
    mpc.advance_to(f.parse(mpc));

    int value = 42;

    bslfmt::Formatter_MockFormatContext<char> mfc(value, 0, 0);

    mfc.advance_to(bsl::as_const(f).format(value, mfc));

    ASSERT("2a***" == mfc.finalString());
//..
//
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
            //LINE  FORMAT        EXPECTED      VALUE
            //----  -----------   ---------     -----
            { L_,   "{0:}",       "5",          5    },
            { L_,   "{0:+}",      "+5",         5    },
            { L_,   "{0:-}",      "5",          5    },
            { L_,   "{0: }",      " 5",         5    },
            { L_,   "{:#05x}",    "0x00c",      12   },
            { L_,   "{:#05X}",    "0X00C",      12   },
            { L_,   "{:*<6}",     "5*****",     5    },
            { L_,   "{:*>6}",     "*****5",     5    },
            { L_,   "{:*^6}",     "**5***",     5    },
            { L_,   "{:*^+6}",    "**+5**",     5    },
            { L_,   "{:#0x}",     "0x5",        5    },
            { L_,   "{:#01x}",    "0x5",        5    },
            { L_,   "{:#02x}",    "0x5",        5    },
            { L_,   "{:#03x}",    "0x5",        5    },
            { L_,   "{:#04x}",    "0x05",       5    },
            { L_,   "{:#04X}",    "0X05",       5    },
            { L_,   "{:#b}",      "0b101",      5    },
            { L_,   "{:#05b}",    "0b101",      5    },
            { L_,   "{:#06b}",    "0b0101",     5    },
            { L_,   "{:#06B}",    "0B1100",     12   },
            { L_,   "{:#d}",      "5",          5    },
            { L_,   "{:#d}",      "12",         12   },
            { L_,   "{:#o}",      "05",         5    },
            { L_,   "{:#o}",      "014",        12   },
            { L_,   "{:b}",       "1100",       12   },
            { L_,   "{:B}",       "1100",       12   },
            { L_,   "{:d}",       "12",         12   },
            { L_,   "{:o}",       "14",         12   },
            { L_,   "{:x}",       "c",          12   },
            { L_,   "{:X}",       "C",          12   },
            { L_,   "{:#<5x}",    "5####",      5    },
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
            //LINE  FORMAT         EXPECTED        VALUE
            //----  -----------    ------------    -----
            { L_,   "{0:}",        "-5",           -5    },
            { L_,   "{0:+}",       "-5",           -5    },
            { L_,   "{0:-}",       "-5",           -5    },
            { L_,   "{0: }",       "-5",           -5    },
            { L_,   "{:#05x}",     "-0x0c",        -12   },
            { L_,   "{:#05X}",     "-0X0C",        -12   },
            { L_,   "{:*<6}",      "-5****",       -5    },
            { L_,   "{:*>6}",      "****-5",       -5    },
            { L_,   "{:*^6}",      "**-5**",       -5    },
            { L_,   "{:*^+6}",     "**-5**",       -5    },
            { L_,   "{:#0x}",      "-0x5",         -5    },
            { L_,   "{:#01x}",     "-0x5",         -5    },
            { L_,   "{:#02x}",     "-0x5",         -5    },
            { L_,   "{:#03x}",     "-0x5",         -5    },
            { L_,   "{:#04x}",     "-0x5",         -5    },
            { L_,   "{:#05X}",     "-0X05",        -5    },
            { L_,   "{:#b}",       "-0b101",       -5    },
            { L_,   "{:#05b}",     "-0b101",       -5    },
            { L_,   "{:#07b}",     "-0b0101",      -5    },
            { L_,   "{:#08B}",     "-0B01100",     -12   },
            { L_,   "{:#d}",       "-5",           -5    },
            { L_,   "{:#d}",       "-12",          -12   },
            { L_,   "{:#o}",       "-05",          -5    },
            { L_,   "{:#o}",       "-014",         -12   },
            { L_,   "{:b}",        "-1100",        -12   },
            { L_,   "{:B}",        "-1100",        -12   },
            { L_,   "{:d}",        "-12",          -12   },
            { L_,   "{:o}",        "-14",          -12   },
            { L_,   "{:x}",        "-c",           -12   },
            { L_,   "{:X}",        "-C",           -12   },
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
            //LINE  FORMAT         EXPECTED       VALUE
            //----  -----------    -----------    -----
            { L_,   L"{0:}",       L"5",          5    },
            { L_,   L"{0:+}",      L"+5",         5    },
            { L_,   L"{0:-}",      L"5",          5    },
            { L_,   L"{0: }",      L" 5",         5    },
            { L_,   L"{:#05x}",    L"0x00c",      12   },
            { L_,   L"{:#05X}",    L"0X00C",      12   },
            { L_,   L"{:*<6}",     L"5*****",     5    },
            { L_,   L"{:*>6}",     L"*****5",     5    },
            { L_,   L"{:*^6}",     L"**5***",     5    },
            { L_,   L"{:*^+6}",    L"**+5**",     5    },
            { L_,   L"{:#0x}",     L"0x5",        5    },
            { L_,   L"{:#01x}",    L"0x5",        5    },
            { L_,   L"{:#02x}",    L"0x5",        5    },
            { L_,   L"{:#03x}",    L"0x5",        5    },
            { L_,   L"{:#04x}",    L"0x05",       5    },
            { L_,   L"{:#04X}",    L"0X05",       5    },
            { L_,   L"{:#b}",      L"0b101",      5    },
            { L_,   L"{:#05b}",    L"0b101",      5    },
            { L_,   L"{:#06b}",    L"0b0101",     5    },
            { L_,   L"{:#06B}",    L"0B1100",     12   },
            { L_,   L"{:#d}",      L"5",          5    },
            { L_,   L"{:#d}",      L"12",         12   },
            { L_,   L"{:#o}",      L"05",         5    },
            { L_,   L"{:#o}",      L"014",        12   },
            { L_,   L"{:b}",       L"1100",       12   },
            { L_,   L"{:B}",       L"1100",       12   },
            { L_,   L"{:d}",       L"12",         12   },
            { L_,   L"{:o}",       L"14",         12   },
            { L_,   L"{:x}",       L"c",          12   },
            { L_,   L"{:X}",       L"C",          12   },
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
            //LINE  FORMAT          EXPECTED         VALUE
            //----  -----------     ------------     -----
            { L_,   L"{0:}",        L"-5",           -5    },
            { L_,   L"{0:+}",       L"-5",           -5    },
            { L_,   L"{0:-}",       L"-5",           -5    },
            { L_,   L"{0: }",       L"-5",           -5    },
            { L_,   L"{:#05x}",     L"-0x0c",        -12   },
            { L_,   L"{:#05X}",     L"-0X0C",        -12   },
            { L_,   L"{:*<6}",      L"-5****",       -5    },
            { L_,   L"{:*>6}",      L"****-5",       -5    },
            { L_,   L"{:*^6}",      L"**-5**",       -5    },
            { L_,   L"{:*^+6}",     L"**-5**",       -5    },
            { L_,   L"{:#0x}",      L"-0x5",         -5    },
            { L_,   L"{:#01x}",     L"-0x5",         -5    },
            { L_,   L"{:#02x}",     L"-0x5",         -5    },
            { L_,   L"{:#03x}",     L"-0x5",         -5    },
            { L_,   L"{:#04x}",     L"-0x5",         -5    },
            { L_,   L"{:#05X}",     L"-0X05",        -5    },
            { L_,   L"{:#b}",       L"-0b101",       -5    },
            { L_,   L"{:#05b}",     L"-0b101",       -5    },
            { L_,   L"{:#07b}",     L"-0b0101",      -5    },
            { L_,   L"{:#08B}",     L"-0B01100",     -12   },
            { L_,   L"{:#d}",       L"-5",           -5    },
            { L_,   L"{:#d}",       L"-12",          -12   },
            { L_,   L"{:#o}",       L"-05",          -5    },
            { L_,   L"{:#o}",       L"-014",         -12   },
            { L_,   L"{:b}",        L"-1100",        -12   },
            { L_,   L"{:B}",        L"-1100",        -12   },
            { L_,   L"{:d}",        L"-12",          -12   },
            { L_,   L"{:o}",        L"-14",          -12   },
            { L_,   L"{:x}",        L"-c",           -12   },
            { L_,   L"{:X}",        L"-C",           -12   },
        };

        enum {
            NUM_NEGATIVE_WCHAR_DATA = sizeof NEGATIVE_WCHAR_DATA /
                                     sizeof *NEGATIVE_WCHAR_DATA
        };

        for (int i = 0; i < NUM_POSITIVE_CHAR_DATA; ++i) {
            const int           LINE     = POSITIVE_CHAR_DATA[i].d_line;
            const char         *FORMAT   = POSITIVE_CHAR_DATA[i].d_format_p;
            const char         *EXPECTED = POSITIVE_CHAR_DATA[i].d_expected_p;
            const int           VALUE    = POSITIVE_CHAR_DATA[i].d_value;
            const long          L_VALUE  = static_cast<long>(VALUE);
            const long long     LL_VALUE = static_cast<long long>(VALUE);
            const long          U_VALUE  = static_cast<unsigned>(VALUE);
            const unsigned long UL_VALUE = static_cast<unsigned long>(VALUE);

            const unsigned long long ULL_VALUE =
                                        static_cast<unsigned long long>(VALUE);

            testRuntimeParse<char,                    int>(LINE, FORMAT);
            testRuntimeParse<char,           unsigned int>(LINE, FORMAT);
            testRuntimeParse<char,               long int>(LINE, FORMAT);
            testRuntimeParse<char,      unsigned long int>(LINE, FORMAT);
            testRuntimeParse<char,          long long int>(LINE, FORMAT);
            testRuntimeParse<char, unsigned long long int>(LINE, FORMAT);

            testRuntimeFormat(LINE, EXPECTED, FORMAT,     VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,   U_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,   L_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  UL_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  LL_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT, ULL_VALUE);
        }

        for (int i = 0; i < NUM_NEGATIVE_CHAR_DATA; ++i) {
            const int        LINE     = NEGATIVE_CHAR_DATA[i].d_line;
            const char      *FORMAT   = NEGATIVE_CHAR_DATA[i].d_format_p;
            const char      *EXPECTED = NEGATIVE_CHAR_DATA[i].d_expected_p;
            const int        VALUE    = NEGATIVE_CHAR_DATA[i].d_value;
            const long       L_VALUE  = static_cast<long>(VALUE);
            const long long  LL_VALUE = static_cast<long long>(VALUE);

            testRuntimeFormat(LINE, EXPECTED, FORMAT,     VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,   L_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  LL_VALUE);
        }

        if (verbose) printf("\tTesting wstrings.\n");

        for (int i = 0; i < NUM_POSITIVE_WCHAR_DATA; ++i) {
            const int            LINE     = POSITIVE_WCHAR_DATA[i].d_line;
            const wchar_t       *FORMAT   = POSITIVE_WCHAR_DATA[i].d_format_p;
            const wchar_t       *EXPECTED =
                                           POSITIVE_WCHAR_DATA[i].d_expected_p;
            const int            VALUE    = POSITIVE_WCHAR_DATA[i].d_value;
            const long           L_VALUE  = static_cast<long>(VALUE);
            const long long      LL_VALUE = static_cast<long long>(VALUE);
            const unsigned long  UL_VALUE  = static_cast<unsigned long>(VALUE);

            const unsigned long long ULL_VALUE  =
                                        static_cast<unsigned long long>(VALUE);

            testRuntimeParse<wchar_t,                    int>(LINE, FORMAT);
            testRuntimeParse<wchar_t,           unsigned int>(LINE, FORMAT);
            testRuntimeParse<wchar_t,               long int>(LINE, FORMAT);
            testRuntimeParse<wchar_t,      unsigned long int>(LINE, FORMAT);
            testRuntimeParse<wchar_t,          long long int>(LINE, FORMAT);
            testRuntimeParse<wchar_t, unsigned long long int>(LINE, FORMAT);

            testRuntimeFormat(LINE, EXPECTED, FORMAT,     VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,   L_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  UL_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  LL_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT, ULL_VALUE);
        }

        for (int i = 0; i < NUM_NEGATIVE_WCHAR_DATA; ++i) {
            const int        LINE     = NEGATIVE_WCHAR_DATA[i].d_line;
            const wchar_t   *FORMAT   = NEGATIVE_WCHAR_DATA[i].d_format_p;
            const wchar_t   *EXPECTED = NEGATIVE_WCHAR_DATA[i].d_expected_p;
            const int        VALUE    = NEGATIVE_WCHAR_DATA[i].d_value;
            const long       L_VALUE  = static_cast<long>(VALUE);
            const long long  LL_VALUE = static_cast<long long>(VALUE);

            testRuntimeFormat(LINE, EXPECTED, FORMAT,     VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,   L_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  LL_VALUE);
        }

        if (verbose) printf("\tTesting compile-time processing.\n");
        {
            bsl::string message;
            testCompileTimeParse<char>(&message, "{0:}",    L_);
            testCompileTimeParse<char>(&message, "{0:+}",   L_);
            testCompileTimeParse<char>(&message, "{0:-}",   L_);
            testCompileTimeParse<char>(&message, "{0: }",   L_);
            testCompileTimeParse<char>(&message, "{:*<6}",  L_);
            testCompileTimeParse<char>(&message, "{:*>6}",  L_);
            testCompileTimeParse<char>(&message, "{:*^6}",  L_);
            testCompileTimeParse<char>(&message, "{:*^+6}", L_);

            testCompileTimeParse<wchar_t>(&message, L"{0:}",    L_);
            testCompileTimeParse<wchar_t>(&message, L"{0:+}",   L_);
            testCompileTimeParse<wchar_t>(&message, L"{0:-}",   L_);
            testCompileTimeParse<wchar_t>(&message, L"{0: }",   L_);
            testCompileTimeParse<wchar_t>(&message, L"{:*<6}",  L_);
            testCompileTimeParse<wchar_t>(&message, L"{:*>6}",  L_);
            testCompileTimeParse<wchar_t>(&message, L"{:*^6}",  L_);
            testCompileTimeParse<wchar_t>(&message, L"{:*^+6}", L_);

            const int VALUE = 5;
            const int DUMMY_ARG = 0;
            bool rv = bslfmt::Formatter_TestUtil<char>::testEvaluateFormat(
                                                                    &message,
                                                                    "5",
                                                                    false,
                                                                    "{:}",
                                                                    VALUE,
                                                                    DUMMY_ARG,
                                                                    DUMMY_ARG);
            ASSERTV(message.c_str(), rv);

            rv = bslfmt::Formatter_TestUtil<wchar_t>::testEvaluateFormat(
                                                                    &message,
                                                                    L"5",
                                                                    false,
                                                                    L"{:}",
                                                                    VALUE,
                                                                    DUMMY_ARG,
                                                                    DUMMY_ARG);
            ASSERTV(message.c_str(), rv);

            const unsigned long long int ULL_VALUE = 12;
            rv = bslfmt::Formatter_TestUtil<char>::testEvaluateFormat(
                                                                    &message,
                                                                    "12",
                                                                    false,
                                                                    "{:}",
                                                                    ULL_VALUE,
                                                                    DUMMY_ARG,
                                                                    DUMMY_ARG);
            ASSERTV(message.c_str(), rv);

            rv = bslfmt::Formatter_TestUtil<wchar_t>::testEvaluateFormat(
                                                                    &message,
                                                                    L"12",
                                                                    false,
                                                                    L"{:}",
                                                                    ULL_VALUE,
                                                                    DUMMY_ARG,
                                                                    DUMMY_ARG);
            ASSERTV(message.c_str(), rv);
        }


#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\tTesting locale prohibition.\n");
        {
            try {
                bsl::string message;
                bool        rv =
                       bslfmt::Formatter_TestUtil<char>::testParseVFormat<int>(
                           &message,
                           false,
                           "{:}");

                ASSERTV(message.c_str(), rv);

                rv = bslfmt::Formatter_TestUtil<char>::testParseVFormat<int>(
                                                                      &message,
                                                                      false,
                                                                      "{:L}");
                ASSERTV(message.c_str(), !rv);
            }
            catch(const bsl::format_error& err) {
                ASSERTV(err.what(),
                        "Exception should have been caught by the "
                        "`Formatter_TestUtil`",
                        false);
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
