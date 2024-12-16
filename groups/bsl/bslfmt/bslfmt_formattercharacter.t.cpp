// bslfmt_formattercharacter.t.cpp                                    -*-C++-*-
#include <bslfmt_formattercharacter.h>

#include <bslfmt_formattertestutil.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

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
    const bool veryVerbose = argc > 3;

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
        //: 1 Use test contexts to format a single character.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

///Example: Formatting a character
///- - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test this formatter's ability to present a character with
// defined alignment and padding.
// ```
    bslfmt::MockParseContext<char> mpc("*<6c", 1);

    bsl::formatter<char, char> formatter;
    mpc.advance_to(formatter.parse(mpc));

    char value = 'a';

    bslfmt::MockFormatContext<char> mfc(value, 0, 0);

    mfc.advance_to(bsl::as_const(formatter).format(value, mfc));

    ASSERT("a*****" == mfc.finalString());
// ```
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

        static const struct {
            int         d_line;        // source line number
            const char *d_format_p;    // format spec
            const char *d_expected_p;  // expected result
            char        d_value;       // value to be formatted
        } CHAR_DATA[] = {
            //LINE  FORMAT        EXPECTED       VALUE
            //----  -----------   -----------    -----
            { L_,   "{:}",        "a",           'a'  },
            { L_,   "{:}",        "#",           '#'  },
            { L_,   "{0:}",       "a",           'a'  },
            { L_,   "{0:}",       "#",           '#'  },
            { L_,   "{:#05x}",    "0x061",       'a'  },
            { L_,   "{:#05X}",    "0X061",       'a'  },
            { L_,   "{:*<6}",     "a*****",      'a'  },
            { L_,   "{:*>6}",     "*****a",      'a'  },
            { L_,   "{:*^6}",     "**a***",      'a'  },
            { L_,   "{:*<6c}",    "#*****",      '#'  },
            { L_,   "{:*>6c}",    "*****#",      '#'  },
            { L_,   "{:*^6c}",    "**#***",      '#'  },
            { L_,   "{:#0x}",     "0x61",        'a'  },
            { L_,   "{:#01x}",    "0x61",        'a'  },
            { L_,   "{:#02x}",    "0x61",        'a'  },
            { L_,   "{:#03x}",    "0x61",        'a'  },
            { L_,   "{:#04x}",    "0x61",        'a'  },
            { L_,   "{:#04X}",    "0X23",        '#'  },
            { L_,   "{:#b}",      "0b1100001",   'a'  },
            { L_,   "{:#05b}",    "0b1100001",   'a'  },
            { L_,   "{:#06b}",    "0b1100001",   'a'  },
            { L_,   "{:#06B}",    "0B100011",    '#'  },
            { L_,   "{:#d}",      "97",          'a'  },
            { L_,   "{:#d}",      "35",          '#'  },
            { L_,   "{:#o}",      "0141",        'a'  },
            { L_,   "{:#o}",      "043",         '#'  },
            { L_,   "{:b}",       "1100001",     'a'  },
            { L_,   "{:b}",       "100011",      '#'  },
            { L_,   "{:B}",       "1100001",     'a'  },
            { L_,   "{:c}",       "a",           'a'  },
            { L_,   "{:c}",       "#",           '#'  },
            { L_,   "{:d}",       "97",          'a'  },
            { L_,   "{:d}",       "35",          '#'  },
            { L_,   "{:o}",       "141",         'a'  },
            { L_,   "{:o}",       "43",          '#'  },
            { L_,   "{:x}",       "61",          'a'  },
            { L_,   "{:x}",       "23",          '#'  },
            { L_,   "{:X}",       "61",          'a'  },
            { L_,   "{:6c}",      "a     ",      'a'  },
            { L_,   "{:#<5x}",    "61###",       'a'  },

            // negative value
            { L_,   "{:#05x}",    "0x0a5",       -91  },
            { L_,   "{:#05X}",    "0X0A5",       -91  },
            { L_,   "{:#0x}",     "0xa5",        -91  },
            { L_,   "{:#01x}",    "0xa5",        -91  },
            { L_,   "{:#02x}",    "0xa5",        -91  },
            { L_,   "{:#03x}",    "0xa5",        -91  },
            { L_,   "{:#04x}",    "0xa5",        -91  },
            { L_,   "{:#04X}",    "0XA5",        -91  },
            { L_,   "{:#b}",      "0b10100101",  -91  },
            { L_,   "{:#05b}",    "0b10100101",  -91  },
            { L_,   "{:#06b}",    "0b10100101",  -91  },
            { L_,   "{:#06B}",    "0B10100101",  -91  },
            { L_,   "{:#d}",      "165",         -91  },
            { L_,   "{:#o}",      "0245",        -91  },
            { L_,   "{:b}",       "10100101",    -91  },
            { L_,   "{:B}",       "10100101",    -91  },
            { L_,   "{:d}",       "165",         -91  },
            { L_,   "{:o}",       "245",         -91  },
            { L_,   "{:x}",       "a5",          -91  },
            { L_,   "{:X}",       "A5",          -91  },
            { L_,   "{:#<5x}",    "a5###",       -91  },
        };

        const int NUM_CHAR_DATA = sizeof CHAR_DATA / sizeof *CHAR_DATA;

        static const struct {
            int            d_line;        // source line number
            const wchar_t *d_format_p;    // format spec
            const wchar_t *d_expected_p;  // expected result
            char           d_value;       // value to be formatted
        } WCHAR_DATA[] = {
            //LINE  FORMAT         EXPECTED        VALUE
            //----  ------------   ----------      -----
            { L_,   L"{:}",        L"a",           'a'  },
            { L_,   L"{:}",        L"#",           '#'  },
            { L_,   L"{0:}",       L"a",           'a'  },
            { L_,   L"{0:}",       L"#",           '#'  },
            { L_,   L"{:#05x}",    L"0x061",       'a'  },
            { L_,   L"{:#05X}",    L"0X061",       'a'  },
            { L_,   L"{:*<6}",     L"a*****",      'a'  },
            { L_,   L"{:*>6}",     L"*****a",      'a'  },
            { L_,   L"{:*^6}",     L"**a***",      'a'  },
            { L_,   L"{:*<6c}",    L"#*****",      '#'  },
            { L_,   L"{:*>6c}",    L"*****#",      '#'  },
            { L_,   L"{:*^6c}",    L"**#***",      '#'  },
            { L_,   L"{:#0x}",     L"0x61",        'a'  },
            { L_,   L"{:#01x}",    L"0x61",        'a'  },
            { L_,   L"{:#02x}",    L"0x61",        'a'  },
            { L_,   L"{:#03x}",    L"0x61",        'a'  },
            { L_,   L"{:#04x}",    L"0x61",        'a'  },
            { L_,   L"{:#04X}",    L"0X23",        '#'  },
            { L_,   L"{:#b}",      L"0b1100001",   'a'  },
            { L_,   L"{:#05b}",    L"0b1100001",   'a'  },
            { L_,   L"{:#06b}",    L"0b1100001",   'a'  },
            { L_,   L"{:#06B}",    L"0B100011",    '#'  },
            { L_,   L"{:#d}",      L"97",          'a'  },
            { L_,   L"{:#d}",      L"35",          '#'  },
            { L_,   L"{:#o}",      L"0141",        'a'  },
            { L_,   L"{:#o}",      L"043",         '#'  },
            { L_,   L"{:b}",       L"1100001",     'a'  },
            { L_,   L"{:b}",       L"100011",      '#'  },
            { L_,   L"{:B}",       L"1100001",     'a'  },
            { L_,   L"{:c}",       L"a",           'a'  },
            { L_,   L"{:c}",       L"#",           '#'  },
            { L_,   L"{:d}",       L"97",          'a'  },
            { L_,   L"{:d}",       L"35",          '#'  },
            { L_,   L"{:o}",       L"141",         'a'  },
            { L_,   L"{:o}",       L"43",          '#'  },
            { L_,   L"{:x}",       L"61",          'a'  },
            { L_,   L"{:x}",       L"23",          '#'  },
            { L_,   L"{:X}",       L"61",          'a'  },
            { L_,   L"{:6c}",      L"a     ",      'a'  },
            { L_,   L"{:#<5x}",    L"61###",       'a'  },

            // negative value
            { L_,   L"{:#05x}",    L"0x0a5",       -91  },
            { L_,   L"{:#05X}",    L"0X0A5",       -91  },
            { L_,   L"{:#0x}",     L"0xa5",        -91  },
            { L_,   L"{:#01x}",    L"0xa5",        -91  },
            { L_,   L"{:#02x}",    L"0xa5",        -91  },
            { L_,   L"{:#03x}",    L"0xa5",        -91  },
            { L_,   L"{:#04x}",    L"0xa5",        -91  },
            { L_,   L"{:#04X}",    L"0XA5",        -91  },
            { L_,   L"{:#b}",      L"0b10100101",  -91  },
            { L_,   L"{:#05b}",    L"0b10100101",  -91  },
            { L_,   L"{:#06b}",    L"0b10100101",  -91  },
            { L_,   L"{:#06B}",    L"0B10100101",  -91  },
            { L_,   L"{:#d}",      L"165",         -91  },
            { L_,   L"{:#o}",      L"0245",        -91  },
            { L_,   L"{:b}",       L"10100101",    -91  },
            { L_,   L"{:B}",       L"10100101",    -91  },
            { L_,   L"{:d}",       L"165",         -91  },
            { L_,   L"{:o}",       L"245",         -91  },
            { L_,   L"{:x}",       L"a5",          -91  },
            { L_,   L"{:X}",       L"A5",          -91  },
            { L_,   L"{:#<5x}",    L"a5###",       -91  },
        };

        const int NUM_WCHAR_DATA = sizeof WCHAR_DATA / sizeof *WCHAR_DATA;

        ASSERTV(NUM_CHAR_DATA, NUM_WCHAR_DATA,
                NUM_CHAR_DATA == NUM_WCHAR_DATA);


        static const struct {
            int         d_line;        // source line number
            const char *d_format_p;    // format spec
            const char  d_expected[5]; // expected result
            char        d_value;       // value to be formatted
        } NEGATIVE_CHAR_DATA[] = {
            //LINE  FORMAT     EXPECTED                     VALUE
            //----  ---------- --------------------------   -----
            { L_,   "{:}",     { -91,   0,   0,   0,   0 }, -91   },
            { L_,   "{0:}",    { -92,   0,   0,   0,   0 }, -92   },
            { L_,   "{:*<4}",  { -93, '*', '*', '*',   0 }, -93   },
            { L_,   "{:*>4}",  { '*', '*', '*', -94,   0 }, -94   },
            { L_,   "{:*^4}",  { '*', -95, '*', '*',   0 }, -95   },
            { L_,   "{:c}",    { -96,   0,   0,   0,   0 }, -96   },
            { L_,   "{:4c}",   { -97, ' ', ' ', ' ',   0 }, -97   },
        };

        const int NUM_NEGATIVE_CHAR_DATA = sizeof  NEGATIVE_CHAR_DATA /
                                           sizeof *NEGATIVE_CHAR_DATA;

        static const struct {
            int            d_line;         // source line number
            const wchar_t *d_format_p;     // format spec
            const wchar_t  d_expected[5];  // expected result
            wchar_t        d_value;        // value to be formatted
        } NEGATIVE_WCHAR_DATA[] = {
            //LINE  FORMAT      EXPECTED                      VALUE
            //----  ----------- --------------------------    -----
            { L_,   L"{:}",     { -91,   0,   0,   0,   0 },  -91   },
            { L_,   L"{0:}",    { -92,   0,   0,   0,   0 },  -92   },
            { L_,   L"{:*<4}",  { -93, '*', '*', '*',   0 },  -93   },
            { L_,   L"{:*>4}",  { '*', '*', '*', -94,   0 },  -94   },
            { L_,   L"{:*^4}",  { '*', -95, '*', '*',   0 },  -95   },
            { L_,   L"{:c}",    { -96,   0,   0,   0,   0 },  -96   },
            { L_,   L"{:4c}",   { -97, ' ', ' ', ' ',   0 },  -97   },
        };

        const int NUM_NEGATIVE_WCHAR_DATA = sizeof  NEGATIVE_WCHAR_DATA /
                                            sizeof *NEGATIVE_WCHAR_DATA;


        static const struct {
            int            d_line;         // source line number
            const wchar_t *d_format_p;     // format spec
            const wchar_t  d_expected[5];  // expected result
            char           d_value;        // value to be formatted
        } NEGATIVE_C_TO_W_DATA[] = {
            //LINE  FORMAT      EXPECTED                      VALUE
            //----  ----------- --------------------------    -----
            { L_,   L"{:}",     { 165,   0,   0,   0,   0 },  -91   },
            { L_,   L"{0:}",    { 164,   0,   0,   0,   0 },  -92   },
            { L_,   L"{:*<4}",  { 163, '*', '*', '*',   0 },  -93   },
            { L_,   L"{:*>4}",  { '*', '*', '*', 162,   0 },  -94   },
            { L_,   L"{:*^4}",  { '*', 161, '*', '*',   0 },  -95   },
            { L_,   L"{:c}",    { 160,   0,   0,   0,   0 },  -96   },
            { L_,   L"{:4c}",   { 159, ' ', ' ', ' ',   0 },  -97   },
        };

        const int NUM_NEGATIVE_C_TO_W_DATA = sizeof  NEGATIVE_C_TO_W_DATA /
                                             sizeof *NEGATIVE_C_TO_W_DATA;


        // gcc version 13.2 and earlier incorrectly handles negative char
        // values.

        const bool compilerCorrectlyHandlesNegativeValues =
#if defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION < 130300
                                                            false;
#else
                                                            true;
#endif

        if (verbose) printf("\tTesting runtime processing.\n");

        for (int i = 0; i < NUM_CHAR_DATA; ++i) {
            const int      LINE            =  CHAR_DATA[i].d_line;
            const char    *FORMAT          =  CHAR_DATA[i].d_format_p;
            const wchar_t *WFORMAT         = WCHAR_DATA[i].d_format_p;
            const char    *EXPECTED        =  CHAR_DATA[i].d_expected_p;
            const wchar_t *WEXPECTED       = WCHAR_DATA[i].d_expected_p;
            const char     VALUE           =  CHAR_DATA[i].d_value;
            const bool     ORACLE_REQUIRED =
                           0 < VALUE || compilerCorrectlyHandlesNegativeValues;

            if (veryVerbose) { T_ T_ P_(LINE); P(FORMAT); }

            bsl::string message;

            bool rv = TestUtilChar::testParseVFormat<char>(&message,
                                                           true,
                                                           FORMAT);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);

            rv = TestUtilChar::testEvaluateVFormat(&message,
                                                   EXPECTED,
                                                   ORACLE_REQUIRED,
                                                   FORMAT,
                                                   VALUE);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateVFormat(&message,
                                                    WEXPECTED,
                                                    ORACLE_REQUIRED,
                                                    WFORMAT,
                                                    VALUE);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);
        }

        for (int i = 0; i < NUM_WCHAR_DATA; ++i) {
            const int      LINE            = WCHAR_DATA[i].d_line;
            const wchar_t *FORMAT          = WCHAR_DATA[i].d_format_p;
            const wchar_t *EXPECTED        = WCHAR_DATA[i].d_expected_p;
            const char     VALUE           = WCHAR_DATA[i].d_value;
            const bool     ORACLE_REQUIRED =
                           0 < VALUE || compilerCorrectlyHandlesNegativeValues;

            if (veryVerbose) { T_ T_ P_(LINE); P(FORMAT); }

            bsl::string message;

            bool rv = TestUtilWchar::testParseVFormat<wchar_t>(&message,
                                                               true,
                                                               FORMAT);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateVFormat(&message,
                                                    EXPECTED,
                                                    ORACLE_REQUIRED,
                                                    FORMAT,
                                                    VALUE);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);
        }

        if (verbose)
           printf("\tTesting negative values formatted as characters.\n");

        const bool ORACLE_REQUIRED = compilerCorrectlyHandlesNegativeValues;

        // `char` value to `char` output.

        for (int i = 0; i < NUM_NEGATIVE_CHAR_DATA; ++i) {
            const int               LINE   = NEGATIVE_CHAR_DATA[i].d_line;
            const char             *FORMAT = NEGATIVE_CHAR_DATA[i].d_format_p;
            const bsl::string_view  EXP    = NEGATIVE_CHAR_DATA[i].d_expected;
            const char              VALUE  = NEGATIVE_CHAR_DATA[i].d_value;

            if (veryVerbose) { T_ T_ P_(LINE); P(FORMAT); }

            bsl::string message;

            bool rv = TestUtilChar::testEvaluateVFormat(&message,
                                                        EXP,
                                                        ORACLE_REQUIRED,
                                                        FORMAT,
                                                        VALUE);
            ASSERTV(LINE, FORMAT, message.c_str(), rv);
        }

        // `wchar_t` value to `wchar_t` output.

        for (int i = 0; i < NUM_NEGATIVE_WCHAR_DATA; ++i) {
            const int                LINE = NEGATIVE_WCHAR_DATA[i].d_line;
            const wchar_t           *FMT  = NEGATIVE_WCHAR_DATA[i].d_format_p;
            const bsl::wstring_view  EXP  = NEGATIVE_WCHAR_DATA[i].d_expected;
            const wchar_t            VAL  = NEGATIVE_WCHAR_DATA[i].d_value;

            if (veryVerbose) { T_ T_ P_(LINE); P(FMT); }

            bsl::string message;

            bool rv = TestUtilWchar::testEvaluateVFormat(&message,
                                                         EXP,
                                                         ORACLE_REQUIRED,
                                                         FMT,
                                                         VAL);
            ASSERTV(LINE, FMT, message.c_str(), rv);
        }

        // `char` value to `wchar_t` output.

        for (int i = 0; i < NUM_NEGATIVE_C_TO_W_DATA; ++i) {
            const int                LINE = NEGATIVE_C_TO_W_DATA[i].d_line;
            const wchar_t           *FMT  = NEGATIVE_C_TO_W_DATA[i].d_format_p;
            const bsl::wstring_view  EXP  = NEGATIVE_C_TO_W_DATA[i].d_expected;
            const char               VAL  = NEGATIVE_C_TO_W_DATA[i].d_value;

            if (veryVerbose) { T_ T_ P_(LINE); P(FMT); }

            bsl::string message;

            bool rv = TestUtilWchar::testEvaluateVFormat(&message,
                                                         EXP,
                                                         ORACLE_REQUIRED,
                                                         FMT,
                                                         VAL);
            ASSERTV(LINE, FMT, message.c_str(), rv);
        }

        if (verbose) printf("\tTesting compile-time processing.\n");
        {
            bsl::string message;

            // `parse`

            bool rv = TestUtilChar::testParseFormat<char>(&message,
                                                          true,
                                                          "{0:}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<char>(&message, true, L"{0:}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<wchar_t>(&message,
                                                         true,
                                                         L"{0:}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilChar::testParseFormat<char>(&message, true, "{:*<6}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<char>(&message,
                                                      true,
                                                      L"{:*<6}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<wchar_t>(&message,
                                                         true,
                                                         L"{:*<6}");
            ASSERTV(message.c_str(), rv);

            // `format`

            const char     VALUE  = 'A';
            const wchar_t WVALUE  = 'A';
            rv = TestUtilChar::testEvaluateFormat(&message,
                                                  "A",
                                                  true,
                                                  "{:}",
                                                  VALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"A",
                                                   true,
                                                   L"{:}",
                                                   VALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"A",
                                                   true,
                                                   L"{:}",
                                                   WVALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilChar::testEvaluateFormat(&message,
                                                  "A*****",
                                                  true,
                                                  "{:*<6}",
                                                  VALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"A*****",
                                                   true,
                                                   L"{:*<6}",
                                                   VALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"A*****",
                                                   true,
                                                   L"{:*<6}",
                                                   WVALUE);

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
