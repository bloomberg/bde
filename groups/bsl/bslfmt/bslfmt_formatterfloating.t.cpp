// bslfmt_formatterfloating.t.cpp                                     -*-C++-*-
#include <bslfmt_formatterfloating.h>

#include <bslfmt_format_args.h>
#include <bslfmt_formattertestutil.h>    // Testing only

#include <bslma_testallocator.h>         // Testing only
#include <bslma_defaultallocatorguard.h> // Testing only

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <limits>

#include <stdio.h>
#include <string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// In this component we are testing partial specializations of `bsl::format`
// using test helpers defined locally and elsewhere.
//-----------------------------------------------------------------------------
// [  ]
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] iterator format(t_VALUE, t_FORMAT_CONTEXT& ) const;
// [ 2] iterator parse(t_PARSE_CONTEXT& );
// [ 3] CONCERN: `double` values are formatted properly
// [ 4] CONCERN: `float` values are formatted properly
// [ 5] CONCERN: `long double` values are disabled from formatting
// [ 6] CONCERN: Format string is processed compile time
// [ 7] CONCERN: Locale flag is prohibited
// [ 8] CONCERN: Format string parsing with context
// [ 9] USAGE EXAMPLE
// [ *] CONCERN: No memory is leaked from the default allocator.
// [ *] CONCERN: No memory came from the global allocator.

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
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

/// Check whether the `bslfmt::formatter<t_TYPE, char>::parse` function works
/// as expected for the specified `format` string created at runtime.  The
/// specified `line` is used to identify the function call location.
template <class t_TYPE>
bool testRuntimeCharParse(int line, const char *format)
{
    bsl::string message;

    bool rv = bslfmt::FormatterTestUtil<char>::testParseVFormat<t_TYPE>(
                                                                      &message,
                                                                      false,
                                                                      format);
    ASSERTV(line, format, message.c_str(), rv);

    return rv;
}

/// Check whether the `bslfmt::formatter<t_TYPE, wchar_t>::parse` function
/// works as expected for the specified `format` string created at runtime.
/// The specified `line` is used to identify the function call location.
template <class t_TYPE>
bool testRuntimeWcharParse(int line, const wchar_t *format)
{
    bsl::string message;

    bool rv = bslfmt::FormatterTestUtil<wchar_t>::testParseVFormat<t_TYPE>(
                                                                      &message,
                                                                      false,
                                                                      format);
    ASSERTV(line, format, message.c_str(), rv);

    return rv;
}

/// Check whether the `bslfmt::formatter<t_TYPE, t_CHAR>::format` function
/// produces the specified `expected` result for the specified `format` and
/// `value`.  The specified `line` is used to identify the function call
/// location.
template <class t_CHAR, class t_TYPE>
bool testRuntimeFormat(int           line,
                       const t_CHAR *expected,
                       const t_CHAR *format,
                       t_TYPE        value)
{
    bsl::string message;
    int         dummyArg = 0;

    bool rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(&message,
                                                                     expected,
                                                                     true,
                                                                     format,
                                                                     value,
                                                                     dummyArg,
                                                                     dummyArg);
    ASSERTV(line, format, message.c_str(), rv);

    return rv;
}

/// Check whether the `bslfmt::formatter<t_TYPE, t_CHAR>::format` function
/// produces the specified `expected` result for the specified `format`,
/// `value1`, and `value2`.  The specified `line` is used to identify the
/// function call location.
template <class t_CHAR, class t_TYPE1, class t_TYPE2>
bool testRuntimeFormat(int           line,
                       const t_CHAR *expected,
                       const t_CHAR *format,
                       t_TYPE1       value1,
                       t_TYPE2       value2)
{
    bsl::string message;
    int         dummyArg = 0;

    bool rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(&message,
                                                                     expected,
                                                                     true,
                                                                     format,
                                                                     value1,
                                                                     value2,
                                                                     dummyArg);
    ASSERTV(line, format, message.c_str(), rv);

    return rv;
}

/// Check whether the `bslfmt::formatter<t_TYPE, t_CHAR>::format` function
/// produces the specified `expected` result for the specified `format`,
/// `value1`, `value2`, and `value3`.  The specified `line` is used to identify
/// the function call location.
template <class t_CHAR, class t_TYPE1, class t_TYPE2, class t_TYPE3>
bool testRuntimeFormat(int           line,
                       const t_CHAR *expected,
                       const t_CHAR *format,
                       t_TYPE1       value1,
                       t_TYPE2       value2,
                       t_TYPE3       value3)
{
    bsl::string message;
    int         dummyArg = 0;

    bool rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(&message,
                                                                     expected,
                                                                     true,
                                                                     format,
                                                                     value1,
                                                                     value2,
                                                                     value3);
    ASSERTV(line, format, message.c_str(), rv);

    return rv;
}
}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    const int  test                = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose             = argc > 2;
    const bool veryVerbose         = argc > 3;
    const bool veryVeryVerbose     = argc > 4;
    const bool veryVeryVeryVerbose = argc > 4;

    printf("TEST %s CASE %d \n", __FILE__, test);

    // CONCERN: No memory is leaked from the default allocator.
    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) {  case 0:
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, replace
        //    leading comment characters with spaces, and replace `assert` with
        //    `ASSERT`.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

///Example: Formatting a pointer
///- - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test pointer formatter's ability to format a number in
// hexadecimal format with defined alignment and padding.
//
// ```
    bslfmt::MockParseContext<char> mpc("*<8a", 1);

    bsl::formatter<double , char > f;
    mpc.advance_to(f.parse(mpc));

    const double value = 42.24;

    bslfmt::MockFormatContext<char> mfc(value, 0, 0);

    mfc.advance_to(bsl::as_const(f).format(value, mfc));

    ASSERT("1.51eb851eb851fp+5" == mfc.finalString());
// ```
      } break;
      case 8: {
        // -----------------------------------------------
        // FORMAT STRING PARSING
        //
        // Concerns:
        //: 1 Invalid format specs will generate a parse error
        //:
        //: 2 Valid format specs will not generate a parse error
        //
        // Plan:
        //: 1 Construct format specs corresponding to each of the known error
        //:   conditions and verify that they result in a parse error. (C-1)
        //:
        //: 2 Construct format specs containing different combinations of
        //:   valid specification components and verify that they correctly
        //:   parse. (C-2)
        //
        // Testing:
        //   CONCERN: Format string parsing with context
        // -----------------------------------------------
        if (verbose) puts("\nFORMAT STRING PARSING"
                          "\n=====================");

#define TEST_PARSE_FAIL_(charType, type, fmtStr, useOracle)                   \
    do {                                                                      \
        bsl::string errorMsg;                                                 \
        bool rv;                                                              \
        rv = bslfmt::FormatterTestUtil<charType>::                            \
                 testParseFailure<type>(&errorMsg, useOracle, fmtStr);        \
        if (!rv) {                                                            \
            bsl::basic_string<charType> formatStr(fmtStr);                    \
            ASSERTV(L_, errorMsg.c_str(), formatStr.c_str(), rv);             \
        }                                                                     \
    } while (false)

#define TEST_PARSE_FAIL(charType, fmtStr, useOracle)                          \
    do {                                                                      \
        TEST_PARSE_FAIL_(charType, float, fmtStr, useOracle);                 \
        TEST_PARSE_FAIL_(charType, double, fmtStr, useOracle);                \
    } while (false)

#define TEST_PARSE_SUCCESS_F_(charType, type, fmtStr, useOracle)              \
    do {                                                                      \
        bsl::string errorMsg;                                                 \
        bool rv;                                                              \
                                                                              \
        rv = bslfmt::FormatterTestUtil<charType>::                            \
                 testParseFormat<type>(&errorMsg, useOracle, fmtStr);         \
        if (!rv) {                                                            \
            bsl::basic_string<charType> formatStr(fmtStr);                    \
            ASSERTV(L_, errorMsg.c_str(), formatStr.c_str(), rv);             \
        }                                                                     \
    } while (false)

#define TEST_PARSE_SUCCESS_F(charType, fmtStr, useOracle)                     \
    do {                                                                      \
        TEST_PARSE_SUCCESS_F_(charType, float, fmtStr, useOracle);            \
        TEST_PARSE_SUCCESS_F_(charType, double, fmtStr, useOracle);           \
    } while (false)

#define TEST_PARSE_SUCCESS_VF_(charType, type, fmtStr, useOracle)             \
    do {                                                                      \
        bsl::string errorMsg;                                                 \
        bool        rv;                                                       \
                                                                              \
        rv = bslfmt::FormatterTestUtil<charType>::testParseVFormat<type>      \
                                                              (&errorMsg,     \
                                                               useOracle,     \
                                                               fmtStr);       \
        if (!rv) {                                                            \
            bsl::basic_string<charType> formatStr(fmtStr);                    \
            ASSERTV(L_, errorMsg.c_str(), formatStr.c_str(), rv);             \
        }                                                                     \
    } while (false)

#define TEST_PARSE_SUCCESS_VF(charType, fmtStr, useOracle)                    \
    do {                                                                      \
        TEST_PARSE_SUCCESS_VF_(charType, float, fmtStr, useOracle);           \
        TEST_PARSE_SUCCESS_VF_(charType, double, fmtStr, useOracle);          \
    } while (false)

        // Bad fill character
        // Note can only test '{' as '}' closes the parse string.
        TEST_PARSE_FAIL(char,     "{:{<5.5}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:{<5.5}",    true);

        // Missing fill specifier
        TEST_PARSE_FAIL(char,     "{:*5.5}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{:*5.5}",     true);

        // Locale option
        TEST_PARSE_FAIL(char,     "{:*<5.5L}",   false);
        TEST_PARSE_FAIL(wchar_t, L"{:*<5.5L}",   false);

        // Non-float type
        TEST_PARSE_FAIL(char,     "{:*<5.5d}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<5.5d}",    true);

        // Non-numeric width or precision
        TEST_PARSE_FAIL(char,     "{:*< X.5}",   true);
        TEST_PARSE_FAIL(char,     "{:*<+5.X}",   true);
        TEST_PARSE_FAIL(char,     "{:*<-X.X}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*< X.5}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<+5.X}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<-X.X}",   true);

        // Missing precision marker
        TEST_PARSE_FAIL(char,     "{:*<{}{}}",      false);
        TEST_PARSE_FAIL(char,     "{0:*<{3}{2}}",   false);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{}{}}",      false);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{3}{2}}",   false);

        // Mixed numeric and non-numeric nested args
        TEST_PARSE_FAIL(char,     "{:*<{}.{2}}",     true);
        TEST_PARSE_FAIL(char,     "{:*<{1}.{}}",     true);
        TEST_PARSE_FAIL(char,     "{:*<{1}.{2}}",    true);
        TEST_PARSE_FAIL(char,     "{0:*<{}.{}}",     true);
        TEST_PARSE_FAIL(char,     "{0:*<{}.{2}}",    true);
        TEST_PARSE_FAIL(char,     "{0:*<{1}.{}}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{}.{2}}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{1}.{}}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{1}.{2}}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{}.{}}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{}.{2}}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{1}.{}}",    true);

        // Nested args out of range
        // Not checked in std parsing
        TEST_PARSE_FAIL(char,     "{0:*<{1}.{3}}",   false);
        TEST_PARSE_FAIL(char,     "{0:*<{3}.{2}}",   false);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{1}.{3}}",   false);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{3}.{2}}",   false);

        // A selection of valid format strings (non-unicode)
        TEST_PARSE_SUCCESS_F(char,     "{:}"                   , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:}"                   , true);
        TEST_PARSE_SUCCESS_F(char,     "{:#}"                   , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:#}"                   , true);
        TEST_PARSE_SUCCESS_F(char,     "{:0}"                   , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:0}"                   , true);
        TEST_PARSE_SUCCESS_F(char,     "{:+}"                   , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:+}"                   , true);
        TEST_PARSE_SUCCESS_F(char,     "{:-}"                   , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:-}"                   , true);
        TEST_PARSE_SUCCESS_F(char,     "{: }"                   , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{: }"                   , true);
        TEST_PARSE_SUCCESS_F(char,     "{:e}"                  , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:e}"                  , true);
        TEST_PARSE_SUCCESS_F(char,     "{:f}"                  , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:f}"                  , true);
        TEST_PARSE_SUCCESS_F(char,     "{:g}"                  , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:g}"                  , true);
        TEST_PARSE_SUCCESS_F(char,     "{:a}"                  , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:a}"                  , true);
        TEST_PARSE_SUCCESS_F(char,     "{:E}"                  , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:E}"                  , true);
        TEST_PARSE_SUCCESS_F(char,     "{:F}"                  , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:F}"                  , true);
        TEST_PARSE_SUCCESS_F(char,     "{:G}"                  , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:G}"                  , true);
        TEST_PARSE_SUCCESS_F(char,     "{:A}"                  , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:A}"                  , true);
        TEST_PARSE_SUCCESS_F(char,     "{:.0}"                 , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:.0}"                 , true);
        TEST_PARSE_SUCCESS_F(char,     "{:.8}"                 , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:.8}"                 , true);
        TEST_PARSE_SUCCESS_F(char,     "{:5}"                  , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:5}"                  , true);
        TEST_PARSE_SUCCESS_F(char,     "{:5.0}"                , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:5.0}"                , true);
        TEST_PARSE_SUCCESS_F(char,     "{:5.8}"                , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:5.8}"                , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<}"                 , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<}"                 , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<.0}"               , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<.0}"               , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<.8}"               , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<.8}"               , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<5}"                , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<5}"                , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<5.0}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<5.0}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<5.8}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<5.8}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>}"                 , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>}"                 , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>.0}"               , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>.0}"               , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>.8}"               , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>.8}"               , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>5}"                , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>5}"                , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>5.0}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>5.0}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>5.8}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>5.8}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^}"                 , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^}"                 , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^.0}"               , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^.0}"               , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^.8}"               , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^.8}"               , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^5}"                , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^5}"                , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^5.0}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^5.0}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^5.8}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^5.8}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:{}.{}}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:{}.{}}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<{}.{}}"            , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<{}.{}}"            , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>{}.{}}"            , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>{}.{}}"            , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^{}.{}}"            , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^{}.{}}"            , true);
        TEST_PARSE_SUCCESS_F(char,     "{0:{1}.{1}}"           , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{0:{1}.{1}}"           , true);
        TEST_PARSE_SUCCESS_F(char,     "{0:*<{1}.{1}}"         , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{0:*<{1}.{1}}"         , true);
        TEST_PARSE_SUCCESS_F(char,     "{0:*>{1}.{1}}"         , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{0:*>{1}.{1}}"         , true);
        TEST_PARSE_SUCCESS_F(char,     "{0:*^{1}.{1}}"         , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{0:*^{1}.{1}}"         , true);

#if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE <= 13
        #define TPS TEST_PARSE_SUCCESS_VF
        BSLS_KEYWORD_CONSTEXPR_CPP20 bool oracle_uni = false;
#else
        #define TPS TEST_PARSE_SUCCESS_F
        BSLS_KEYWORD_CONSTEXPR_CPP20 bool oracle_uni = true;
#endif

        // A selection of vaid format strings (unicode)
        TPS(char,     "{:\xF0\x9F\x98\x80<}"         , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600<}"               , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80<.0}"       , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600<.0}"             , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80<.8}"       , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600<.8}"             , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80<5}"        , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600<5}"              , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80<5.0}"      , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600<5.0}"            , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80<5.8}"      , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600<5.8}"            , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80>}"         , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600>}"               , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80>.0}"       , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600>.0}"             , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80>.8}"       , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600>.8}"             , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80>5}"        , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600>5}"              , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80>5.0}"      , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600>5.0}"            , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80>5.8}"      , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600>5.8}"            , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80^}"         , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600^}"               , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80^.0}"       , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600^.0}"             , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80^.8}"       , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600^.8}"             , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80^5}"        , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600^5}"              , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80^5.0}"      , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600^5.0}"            , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80^5.8}"      , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600^5.8}"            , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80<{}.{}}"    , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600<{}.{}}"          , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80>{}.{}}"    , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600>{}.{}}"          , oracle_uni);
        TPS(char,     "{:\xF0\x9F\x98\x80^{}.{}}"    , oracle_uni);
        TPS(wchar_t, L"{:\U0001F600^{}.{}}"          , oracle_uni);
        TPS(char,     "{0:\xF0\x9F\x98\x80<{1}.{1}}" , oracle_uni);
        TPS(wchar_t, L"{0:\U0001F600<{1}.{1}}"       , oracle_uni);
        TPS(char,     "{0:\xF0\x9F\x98\x80>{1}.{1}}" , oracle_uni);
        TPS(wchar_t, L"{0:\U0001F600>{1}.{1}}"       , oracle_uni);
        TPS(char,     "{0:\xF0\x9F\x98\x80^{1}.{1}}" , oracle_uni);
        TPS(wchar_t, L"{0:\U0001F600^{1}.{1}}"       , oracle_uni);

#undef TPS
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // LOCALE FLAG PROHIBITION
        //
        // Concerns:
        // 1. The locale flag is prohibited in format strings and results in a
        //    `bsl::format_error` being thrown.
        //
        // Plan:
        // 1. Verify, for both supported floating point types (`double` and
        //    `float`), that while parsing a format string without a locale
        //    flag in it works if the locale flag is added to a format string
        //    the parsing fails.
        //
        // Testing:
        //   CONCERN: Locale flag is prohibited
        // --------------------------------------------------------------------

        if (verbose) puts("\nLOCALE FLAG PROHIBITION"
                          "\n=======================");

#ifdef BDE_BUILD_TARGET_EXC
        try {
            bsl::string message;
            bool        rv = bslfmt::FormatterTestUtil<char>::
                                testParseVFormat<double>(&message,false,"{:}");
            ASSERTV(message.c_str(), rv);

            rv = bslfmt::FormatterTestUtil<char>::
                              testParseVFormat<double>(&message,false, "{:L}");
            ASSERTV(message.c_str(), !rv);
        }
        catch(const bsl::format_error& err) {
            ASSERTV(err.what(),
                    "Exception should have been caught by the "
                    "`FormatterTestUtil`",
                    false);
        }
        try {
            bsl::string message;
            bool        rv = bslfmt::FormatterTestUtil<char>::
                                 testParseVFormat<float>(&message,false,"{:}");
            ASSERTV(message.c_str(), rv);

            rv = bslfmt::FormatterTestUtil<char>::
                               testParseVFormat<float>(&message,false, "{:L}");
            ASSERTV(message.c_str(), !rv);
        }
        catch(const bsl::format_error& err) {
            ASSERTV(err.what(),
                    "Exception should have been caught by the "
                    "`FormatterTestUtil`",
                    false);
        }
#endif  // BDE_BUILD_TARGET_EXC
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COMPILE TIME PROCESSING OF FORMAT STRING
        //
        // Concerns:
        // 1. Format string is processed compile time.
        //
        // Plan:
        // 1. Use the test harness to parse format strings compile-time.  The
        //    code wouldn't compile unless all methods are `constexpr`.
        //
        // Testing:
        //   CONCERN: Format string is processed compile time
        // --------------------------------------------------------------------

        if (verbose) puts("\nCOMPILE TIME PROCESSING OF FORMAT STRING"
                          "\n========================================");

        bsl::string message;

        // `parse`

        bool rv = bslfmt::FormatterTestUtil<char>::testParseFormat<
                                               double>(&message, true, "{0:}");
            ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<char>::testParseFormat<
                                              double>(&message, true, "{0:g}");
            ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<char>::testParseFormat<
                                              double>(&message, true, "{0:E}");
            ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<char>::testParseFormat<
                                              double>(&message, true, "{0:A}");
            ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<wchar_t>::testParseFormat<
                                               float>(&message, true, L"{0:}");
            ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<char>::testParseFormat<
                                          double>(&message, true, "{:*<6.2a}");
            ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<wchar_t>::testParseFormat<
                                          float>(&message, true, L"{:*<6.2g}");
            ASSERTV(message.c_str(), rv);

        // `format`

        const double VALUE     = 5.;
        const int    DUMMY_ARG = 0;
        rv = bslfmt::FormatterTestUtil<char>::testEvaluateFormat(&message,
                                                                    "5",
                                                                    true,
                                                                    "{:}",
                                                                    VALUE,
                                                                    DUMMY_ARG,
                                                                    DUMMY_ARG);
        ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<wchar_t>::testEvaluateFormat(&message,
                                                                    L"5",
                                                                    true,
                                                                    L"{:}",
                                                                    VALUE,
                                                                    DUMMY_ARG,
                                                                    DUMMY_ARG);
        ASSERTV(message.c_str(), rv);

        const float FLOAT_VALUE = 12.34f;
        rv = bslfmt::FormatterTestUtil<char>::testEvaluateFormat(&message,
                                                                 "12.34",
                                                                 true,
                                                                 "{:}",
                                                                 FLOAT_VALUE,
                                                                 DUMMY_ARG,
                                                                 DUMMY_ARG);
        ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<wchar_t>::testEvaluateFormat(
                                                                   &message,
                                                                   L"12.34",
                                                                   true,
                                                                   L"{:}",
                                                                   FLOAT_VALUE,
                                                                   DUMMY_ARG,
                                                                   DUMMY_ARG);
        ASSERTV(message.c_str(), rv);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // LONG DOUBLE PROHIBITION
        //
        // Concerns:
        // 1. The type `long double` is not supported hence `vformat` shall
        //    throw a `bslfmt::format_error` exception.
        //
        // Plan:
        // 1. Try vformatting a `long double`, verify that it throws.
        //
        // Testing:
        //   CONCERN: `long double` values are disabled from formatting
        // --------------------------------------------------------------------

        if (verbose) puts("\nLONG DOUBLE PROHIBITION"
                          "\n=======================");
        {
            bsl::formatter<long double, char>    dummy;   (void)dummy;
            bsl::formatter<long double, wchar_t> wdummy;  (void)wdummy;
        }

#ifdef BDE_BUILD_TARGET_EXC
        {
            try {
                bsl::string message;
                bool        rv = bslfmt::FormatterTestUtil<char>::
                                        testParseVFormat<long double>(&message,
                                                                      false,
                                                                      "{:}");
                ASSERTV(message.c_str(), !rv);
            }
            catch(const bsl::format_error& err) {
                ASSERTV(err.what(),
                        "Exception should have been caught by the "
                        "`FormatterTestUtil`",
                        false);
            }
        }
#endif  // BDE_BUILD_TARGET_EXC
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // FLOAT FORMATTING
        //
        // Concerns:
        // 1. All format specifiers and their combinations result in the
        //    expected string as per the C++ standard:
        //    1. Width
        //    2. Alignment
        //    3. Zero padding
        //    4. Format: General, fixed, scientific, hexfloat
        //    5. Uppercase formats
        //    6. Alternate formats
        //    7. Precision
        //    8. Sign
        //
        // 2. Special floating point values (NaN, Infinity) are formatted
        //    properly.
        //
        // 3. Negative values are formatted properly (for example padding is
        //    before the negative sign, zero padding is after the negative sign
        //    etc.)
        //
        // 4. Formatting works for `char` and `wchar_t` as well.
        //
        // Plan:
        // 1. Table based testing using the test facilities.
        //
        // Testing:
        //   CONCERN: `float` values are formatted properly
        // --------------------------------------------------------------------

        if (verbose) puts("\nFLOAT FORMATTING"
                          "\n================");

        {
            bsl::formatter<float, char>    dummy;   (void)dummy;
            bsl::formatter<float, wchar_t> wdummy;  (void)wdummy;
        }

        typedef std::numeric_limits<float> Limits;

        const float k_FLT_INF     = Limits::infinity();
        const float k_FLT_NEG_INF = -k_FLT_INF;

        const float k_FLT_NAN     = Limits::quiet_NaN();
        const float k_FLT_NEG_NAN = -k_FLT_NAN;

        const float k_FLT_SNAN     = Limits::signaling_NaN();

        // This probably won't be a signaling NaN as changing the sign may
        // actually count as a read, which (optionally signals and) turns
        // it into a quiet NaN.
        const float k_FLT_NEG_SNAN = -Limits::signaling_NaN();

        static struct {
            int            d_line;
            const char    *d_expected;
            const char    *d_format;
            const wchar_t *d_wexpected;
            const wchar_t *d_wformat;
            float          d_value;
        }  FLOAT_TEST[] = {
#define ROW(expected, format, value)                      \
    { L_, expected, format, L##expected, L##format, value }

            // NO PRECISION

            // no format specifier
            ROW("0",   "{:}",   0.f),
            ROW("0",   "{:1}",  0.f),
            ROW(" 0",  "{:2}",  0.f),
            ROW("0 ",  "{:<2}", 0.f),
            ROW("0  ", "{:<3}", 0.f),
            ROW(" 0 ", "{:^3}", 0.f),

            ROW("inf",  "{}", k_FLT_INF     ),
            ROW("-inf", "{}", k_FLT_NEG_INF ),
            ROW("nan",  "{}", k_FLT_NAN     ),
            ROW("-nan", "{}", k_FLT_NEG_NAN ),
            ROW("nan",  "{}", k_FLT_SNAN    ),
            ROW("-nan", "{}", k_FLT_NEG_SNAN),

            ROW("inf",  "{:3}", k_FLT_INF    ),
            ROW("-inf", "{:4}", k_FLT_NEG_INF),
            ROW("nan",  "{:3}", k_FLT_NAN    ),
            ROW("-nan", "{:4}", k_FLT_NEG_NAN),

            ROW("+inf", "{:+}", k_FLT_INF    ),
            ROW("-inf", "{:+}", k_FLT_NEG_INF),
            ROW("+nan", "{:+}", k_FLT_NAN    ),
            ROW("-nan", "{:+}", k_FLT_NEG_NAN),

            ROW(" inf", "{: }", k_FLT_INF    ),
            ROW("-inf", "{: }", k_FLT_NEG_INF),
            ROW(" nan", "{: }", k_FLT_NAN    ),
            ROW("-nan", "{: }", k_FLT_NEG_NAN),

            // hexfloat
            ROW("0p+0",   "{:a}",   0.f),
            ROW("0p+0",   "{:4a}",  0.f),
            ROW(" 0p+0",  "{:5a}",  0.f),
            ROW("0p+0 ",  "{:<5a}", 0.f),
            ROW("0p+0  ", "{:<6a}", 0.f),
            ROW(" 0p+0 ", "{:^6a}", 0.f),
            // uppercase hexfloat
            ROW("0P+0",   "{:A}",   0.f),
            ROW("0P+0",   "{:4A}",  0.f),
            ROW(" 0P+0",  "{:5A}",  0.f),
            ROW("0P+0 ",  "{:<5A}", 0.f),
            ROW("0P+0  ", "{:<6A}", 0.f),
            ROW(" 0P+0 ", "{:^6A}", 0.f),

            // hexfloat ALTERNATE
            ROW("0.p+0",   "{:#a}",   0.f),
            ROW("0.p+0",   "{:#5a}",  0.f),
            ROW(" 0.p+0",  "{:#6a}",  0.f),
            ROW("0.p+0 ",  "{:<#6a}", 0.f),
            ROW("0.p+0  ", "{:<#7a}", 0.f),
            ROW(" 0.p+0 ", "{:^#7a}", 0.f),
            // uppercase hexfloat
            ROW("0.P+0",   "{:#A}",   0.f),
            ROW("0.P+0",   "{:#5A}",  0.f),
            ROW(" 0.P+0",  "{:#6A}",  0.f),
            ROW("0.P+0 ",  "{:<#6A}", 0.f),
            ROW("0.P+0  ", "{:<#7A}", 0.f),
            ROW(" 0.P+0 ", "{:^#7A}", 0.f),

            // scientific
            ROW("0.000000e+00",   "{:e}",    0.f),
            ROW("0.000000e+00",   "{:12e}",  0.f),
            ROW(" 0.000000e+00",  "{:13e}",  0.f),
            ROW("0.000000e+00 ",  "{:<13e}", 0.f),
            ROW("0.000000e+00  ", "{:<14e}", 0.f),
            ROW(" 0.000000e+00 ", "{:^14e}", 0.f),
            // uppercase scientific
            ROW("0.000000E+00",   "{:E}",    0.f),
            ROW("0.000000E+00",   "{:12E}",  0.f),
            ROW(" 0.000000E+00",  "{:13E}",  0.f),
            ROW("0.000000E+00 ",  "{:<13E}", 0.f),
            ROW("0.000000E+00  ", "{:<14E}", 0.f),
            ROW(" 0.000000E+00 ", "{:^14E}", 0.f),

            // scientific ALTERNATE
            ROW("0.000000e+00",   "{:#e}",    0.f),
            ROW("0.000000e+00",   "{:#12e}",  0.f),
            ROW(" 0.000000e+00",  "{:#13e}",  0.f),
            ROW("0.000000e+00 ",  "{:<#13e}", 0.f),
            ROW("0.000000e+00  ", "{:<#14e}", 0.f),
            ROW(" 0.000000e+00 ", "{:^#14e}", 0.f),
            // uppercase scientific ALTERNATE
            ROW("0.000000E+00",   "{:#E}",    0.f),
            ROW("0.000000E+00",   "{:#12E}",  0.f),
            ROW(" 0.000000E+00",  "{:#13E}",  0.f),
            ROW("0.000000E+00 ",  "{:<#13E}", 0.f),
            ROW("0.000000E+00  ", "{:<#14E}", 0.f),
            ROW(" 0.000000E+00 ", "{:^#14E}", 0.f),

            // fixed
            ROW("0.000000",   "{:f}",    0.f),
            ROW("0.000000",   "{:8f}",   0.f),
            ROW(" 0.000000",  "{:9f}",   0.f),
            ROW("0.000000 ",  "{:<9f}",  0.f),
            ROW("0.000000  ", "{:<10f}", 0.f),
            ROW(" 0.000000 ", "{:^10f}", 0.f),
            // uppercase fixed (same thing as fixed)
            ROW("0.000000",   "{:F}",    0.f),
            ROW("0.000000",   "{:8F}",   0.f),
            ROW(" 0.000000",  "{:9F}",   0.f),
            ROW("0.000000 ",  "{:<9F}",  0.f),
            ROW("0.000000  ", "{:<10F}", 0.f),
            ROW(" 0.000000 ", "{:^10F}", 0.f),
            // fixed ALTERNATE
            ROW("0.000000",   "{:#f}",    0.f),
            ROW("0.000000",   "{: <#8f}", 0.f),
            ROW(" 0.000000",  "{: #9f}",  0.f),
            ROW("0.000000 ",  "{:<#9f}",  0.f),
            ROW("0.000000  ", "{:<#10f}", 0.f),
            ROW(" 0.000000 ", "{:^#10f}", 0.f),
            // uppercase fixed (same thing as fixed)
            ROW("0.000000",   "{:#F}",    0.f),
            ROW("0.000000",   "{: <#8F}", 0.f),
            ROW(" 0.000000",  "{: #9F}",  0.f),
            ROW("0.000000 ",  "{:<#9F}",  0.f),
            ROW("0.000000  ", "{:<#10F}", 0.f),
            ROW(" 0.000000 ", "{:^#10F}", 0.f),

            // general
            ROW("0",   "{:g}",   0.f),
            ROW("0",   "{:1g}",  0.f),
            ROW(" 0",  "{:2g}",  0.f),
            ROW("0 ",  "{:<2g}", 0.f),
            ROW("0  ", "{:<3g}", 0.f),
            ROW(" 0 ", "{:^3g}", 0.f),
            // uppercase general
            ROW("0",   "{:G}",   0.f),
            ROW("0",   "{:1G}",  0.f),
            ROW(" 0",  "{:2G}",  0.f),
            ROW("0 ",  "{:<2G}", 0.f),
            ROW("0  ", "{:<3G}", 0.f),
            ROW(" 0 ", "{:^3G}", 0.f),

            // general ALTERNATE
            ROW("0.00000",   "{:#g}",   0.f),
            ROW("0.00000",   "{:#7g}",  0.f),
            ROW(" 0.00000",  "{:#8g}",  0.f),
            ROW("0.00000 ",  "{:<#8g}", 0.f),
            ROW("0.00000  ", "{:<#9g}", 0.f),
            ROW(" 0.00000 ", "{:^#9g}", 0.f),
            // uppercase general
            ROW("0.00000",   "{:#G}",   0.f),
            ROW("0.00000",   "{:#7G}",  0.f),
            ROW(" 0.00000",  "{:#8G}",  0.f),
            ROW("0.00000 ",  "{:<#8G}", 0.f),
            ROW("0.00000  ", "{:<#9G}", 0.f),
            ROW(" 0.00000 ", "{:^#9G}", 0.f),

            // general
            ROW("1.234e-37",   "{:g}",    1.234e-37f),
            ROW("1.234e-37",   "{:9g}",   1.234e-37f),
            ROW(" 1.234e-37",  "{:10g}",  1.234e-37f),
            ROW("1.234e-37 ",  "{:<10g}", 1.234e-37f),
            ROW("1.234e-37  ", "{:<11g}", 1.234e-37f),
            ROW(" 1.234e-37 ", "{:^11g}", 1.234e-37f),
            // uppercase general
            ROW("1.234E-37",   "{:G}",    1.234e-37f),
            ROW("1.234E-37",   "{:9G}",   1.234e-37f),
            ROW(" 1.234E-37",  "{:10G}",  1.234e-37f),
            ROW("1.234E-37 ",  "{:<10G}", 1.234e-37f),
            ROW("1.234E-37  ", "{:<11G}", 1.234e-37f),
            ROW(" 1.234E-37 ", "{:^11G}", 1.234e-37f),

            ROW("1.23457e-37", "{:g}", 1.2345678e-37f),

            // general ALTERNATE
            ROW("1.23400e-37",   "{:#g}",    1.234e-37f),
            ROW("1.23400e-37",   "{:#11g}",  1.234e-37f),
            ROW(" 1.23400e-37",  "{:#12g}",  1.234e-37f),
            ROW("1.23400e-37 ",  "{:<#12g}", 1.234e-37f),
            ROW("1.23400e-37  ", "{:<#13g}", 1.234e-37f),
            ROW(" 1.23400e-37 ", "{:^#13g}", 1.234e-37f),
            // uppercase general
            ROW("1.23400E-37",   "{:#G}",    1.234e-37f),
            ROW("1.23400E-37",   "{:#11G}",  1.234e-37f),
            ROW(" 1.23400E-37",  "{:#12G}",  1.234e-37f),
            ROW("1.23400E-37 ",  "{:<#12G}", 1.234e-37f),
            ROW("1.23400E-37  ", "{:<#13G}", 1.234e-37f),
            ROW(" 1.23400E-37 ", "{:^#13G}", 1.234e-37f),

            ROW("1.23457e-37", "{:#g}", 1.2345678e-37f),

            // WITH PRECISION
            // --------------

            // hexfloat .0
            ROW("0p+0",   "{:.0a}",   0.f),
            ROW("0p+0",   "{:4.0a}",  0.f),
            ROW(" 0p+0",  "{:5.0a}",  0.f),
            ROW("0p+0 ",  "{:<5.0a}", 0.f),
            ROW("0p+0  ", "{:<6.0a}", 0.f),
            ROW(" 0p+0 ", "{:^6.0a}", 0.f),
            // uppercase hexfloat .0
            ROW("0P+0",   "{:.0A}",   0.f),
            ROW("0P+0",   "{:4.0A}",  0.f),
            ROW(" 0P+0",  "{:5.0A}",  0.f),
            ROW("0P+0 ",  "{:<5.0A}", 0.f),
            ROW("0P+0  ", "{:<6.0A}", 0.f),
            ROW(" 0P+0 ", "{:^6.0A}", 0.f),

            // hexfloat .0 ALTERNATE
            ROW("0.p+0",   "{:#.0a}",   0.f),
            ROW("0.p+0",   "{:#5.0a}",  0.f),
            ROW(" 0.p+0",  "{:#6.0a}",  0.f),
            ROW("0.p+0 ",  "{:<#6.0a}", 0.f),
            ROW("0.p+0  ", "{:<#7.0a}", 0.f),
            ROW(" 0.p+0 ", "{:^#7.0a}", 0.f),
            // uppercase hexfloat .0 ALTERNATE
            ROW("0.P+0",   "{:#.0A}",   0.f),
            ROW("0.P+0",   "{:#5.0A}",  0.f),
            ROW(" 0.P+0",  "{:#6.0A}",  0.f),
            ROW("0.P+0 ",  "{:<#6.0A}", 0.f),
            ROW("0.P+0  ", "{:<#7.0A}", 0.f),
            ROW(" 0.P+0 ", "{:^#7.0A}", 0.f),

            // hexfloat .1
            ROW("0.0p+0",   "{:.1a}",   0.f),
            ROW("0.0p+0",   "{:6.1a}",  0.f),
            ROW(" 0.0p+0",  "{:7.1a}",  0.f),
            ROW("0.0p+0 ",  "{:<7.1a}", 0.f),
            ROW("0.0p+0  ", "{:<8.1a}", 0.f),
            ROW(" 0.0p+0 ", "{:^8.1a}", 0.f),
            // uppercase hexfloat .1
            ROW("0.0P+0",   "{:.1A}",   0.f),
            ROW("0.0P+0",   "{:6.1A}",  0.f),
            ROW(" 0.0P+0",  "{:7.1A}",  0.f),
            ROW("0.0P+0 ",  "{:<7.1A}", 0.f),
            ROW("0.0P+0  ", "{:<8.1A}", 0.f),
            ROW(" 0.0P+0 ", "{:^8.1A}", 0.f),

            // hexfloat .3
            ROW("0.000p+0",   "{:.3a}",    0.f),
            ROW("0.000p+0",   "{:8.3a}",   0.f),
            ROW(" 0.000p+0",  "{:9.3a}",   0.f),
            ROW("0.000p+0 ",  "{:<9.3a}",  0.f),
            ROW("0.000p+0  ", "{:<10.3a}", 0.f),
            ROW(" 0.000p+0 ", "{:^10.3a}", 0.f),
            // uppercase hexfloat .3
            ROW("0.000P+0",   "{:.3A}",    0.f),
            ROW("0.000P+0",   "{:8.3A}",   0.f),
            ROW(" 0.000P+0",  "{:9.3A}",   0.f),
            ROW("0.000P+0 ",  "{:<9.3A}",  0.f),
            ROW("0.000P+0  ", "{:<10.3A}", 0.f),
            ROW(" 0.000P+0 ", "{:^10.3A}", 0.f),

            // hexfloat .51
            ROW("0.000000000000000000000000000000000000000000000000000p+0",
                                                            "{:.51a}",    0.f),
            ROW("0.000000000000000000000000000000000000000000000000000p+0",
                                                            "{:56.51a}",  0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000p+0",
                                                            "{:57.51a}",  0.f),
            ROW("0.000000000000000000000000000000000000000000000000000p+0 ",
                                                            "{:<57.51a}", 0.f),
            ROW("0.000000000000000000000000000000000000000000000000000p+0  ",
                                                            "{:<58.51a}", 0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000p+0 ",
                                                            "{:^58.51a}", 0.f),
            // uppercase hexfloat .51
            ROW("0.000000000000000000000000000000000000000000000000000P+0",
                                                            "{:.51A}",    0.f),
            ROW("0.000000000000000000000000000000000000000000000000000P+0",
                                                            "{:56.51A}",  0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000P+0",
                                                            "{:57.51A}",  0.f),
            ROW("0.000000000000000000000000000000000000000000000000000P+0 ",
                                                            "{:<57.51A}", 0.f),
            ROW("0.000000000000000000000000000000000000000000000000000P+0  ",
                                                            "{:<58.51A}", 0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000P+0 ",
                                                            "{:^58.51A}", 0.f),

            // scientific .0
            ROW("0e+00",   "{:.0e}",   0.f),
            ROW("0e+00",   "{:5.0e}",  0.f),
            ROW(" 0e+00",  "{:6.0e}",  0.f),
            ROW("0e+00 ",  "{:<6.0e}", 0.f),
            ROW("0e+00  ", "{:<7.0e}", 0.f),
            ROW(" 0e+00 ", "{:^7.0e}", 0.f),
            // uppercase scientific .0
            ROW("0E+00",   "{:.0E}",   0.f),
            ROW("0E+00",   "{:5.0E}",  0.f),
            ROW(" 0E+00",  "{:6.0E}",  0.f),
            ROW("0E+00 ",  "{:<6.0E}", 0.f),
            ROW("0E+00  ", "{:<7.0E}", 0.f),
            ROW(" 0E+00 ", "{:^7.0E}", 0.f),

            // scientific .0 ALTERNATE
            ROW("0.e+00",   "{:#.0e}",   0.f),
            ROW("0.e+00",   "{:#6.0e}",  0.f),
            ROW(" 0.e+00",  "{:#7.0e}",  0.f),
            ROW("0.e+00 ",  "{:<#7.0e}", 0.f),
            ROW("0.e+00  ", "{:<#8.0e}", 0.f),
            ROW(" 0.e+00 ", "{:^#8.0e}", 0.f),
            // uppercase scientific .0
            ROW("0.E+00",   "{:#.0E}",   0.f),
            ROW("0.E+00",   "{:#6.0E}",  0.f),
            ROW(" 0.E+00",  "{:#7.0E}",  0.f),
            ROW("0.E+00 ",  "{:<#7.0E}", 0.f),
            ROW("0.E+00  ", "{:<#8.0E}", 0.f),
            ROW(" 0.E+00 ", "{:^#8.0E}", 0.f),

            // scientific .1
            ROW("0.0e+00",   "{:.1e}",   0.f),
            ROW("0.0e+00",   "{:7.1e}",  0.f),
            ROW(" 0.0e+00",  "{:8.1e}",  0.f),
            ROW("0.0e+00 ",  "{:<8.1e}", 0.f),
            ROW("0.0e+00  ", "{:<9.1e}", 0.f),
            ROW(" 0.0e+00 ", "{:^9.1e}", 0.f),
            // uppercase scientific .1
            ROW("0.0E+00",   "{:.1E}",   0.f),
            ROW("0.0E+00",   "{:7.1E}",  0.f),
            ROW(" 0.0E+00",  "{:8.1E}",  0.f),
            ROW("0.0E+00 ",  "{:<8.1E}", 0.f),
            ROW("0.0E+00  ", "{:<9.1E}", 0.f),
            ROW(" 0.0E+00 ", "{:^9.1E}", 0.f),

            // scientific .1 ALTERNATE
            ROW("0.0e+00",   "{:#.1e}",   0.f),
            ROW("0.0e+00",   "{:#7.1e}",  0.f),
            ROW(" 0.0e+00",  "{:#8.1e}",  0.f),
            ROW("0.0e+00 ",  "{:<#8.1e}", 0.f),
            ROW("0.0e+00  ", "{:<#9.1e}", 0.f),
            ROW(" 0.0e+00 ", "{:^#9.1e}", 0.f),
            // uppercase scientific .1 ALTERNATE
            ROW("0.0E+00",   "{:#.1E}",   0.f),
            ROW("0.0E+00",   "{:#7.1E}",  0.f),
            ROW(" 0.0E+00",  "{:#8.1E}",  0.f),
            ROW("0.0E+00 ",  "{:<#8.1E}", 0.f),
            ROW("0.0E+00  ", "{:<#9.1E}", 0.f),
            ROW(" 0.0E+00 ", "{:^#9.1E}", 0.f),

            // scientific .17
            ROW("0.00000000000000000e+00",   "{:.17e}" ,   0.f),
            ROW("0.00000000000000000e+00",   "{:23.17e}",  0.f),
            ROW(" 0.00000000000000000e+00",  "{:24.17e}",  0.f),
            ROW("0.00000000000000000e+00 ",  "{:<24.17e}", 0.f),
            ROW("0.00000000000000000e+00  ", "{:<25.17e}", 0.f),
            ROW(" 0.00000000000000000e+00 ", "{:^25.17e}", 0.f),
            // uppercase scientific .17
            ROW("0.00000000000000000E+00",   "{:.17E}" ,   0.f),
            ROW("0.00000000000000000E+00",   "{:23.17E}",  0.f),
            ROW(" 0.00000000000000000E+00",  "{:24.17E}",  0.f),
            ROW("0.00000000000000000E+00 ",  "{:<24.17E}", 0.f),
            ROW("0.00000000000000000E+00  ", "{:<25.17E}", 0.f),
            ROW(" 0.00000000000000000E+00 ", "{:^25.17E}", 0.f),

            // scientific .50
            ROW("0.00000000000000000000000000000000000000000000000000e+00",
                                                            "{:.50e}" ,   0.f),
            ROW("0.00000000000000000000000000000000000000000000000000e+00",
                                                            "{:56.50e}",  0.f),
            ROW(" 0.00000000000000000000000000000000000000000000000000e+00",
                                                            "{:57.50e}",  0.f),
            ROW("0.00000000000000000000000000000000000000000000000000e+00 ",
                                                            "{:<57.50e}", 0.f),
            ROW("0.00000000000000000000000000000000000000000000000000e+00  ",
                                                            "{:<58.50e}", 0.f),
            ROW(" 0.00000000000000000000000000000000000000000000000000e+00 ",
                                                            "{:^58.50e}", 0.f),
            // uppercase scientific .50
            ROW("0.00000000000000000000000000000000000000000000000000E+00",
                                                            "{:.50E}" ,   0.f),
            ROW("0.00000000000000000000000000000000000000000000000000E+00",
                                                            "{:56.50E}",  0.f),
            ROW(" 0.00000000000000000000000000000000000000000000000000E+00",
                                                            "{:57.50E}",  0.f),
            ROW("0.00000000000000000000000000000000000000000000000000E+00 ",
                                                            "{:<57.50E}", 0.f),
            ROW("0.00000000000000000000000000000000000000000000000000E+00  ",
                                                            "{:<58.50E}", 0.f),
            ROW(" 0.00000000000000000000000000000000000000000000000000E+00 ",
                                                            "{:^58.50E}", 0.f),

            // general .6
            ROW("1.234e-37",   "{:.6g}",    1.234e-37f),
            ROW("1.234e-37",   "{:9.6g}",   1.234e-37f),
            ROW(" 1.234e-37",  "{:10.6g}",  1.234e-37f),
            ROW("1.234e-37 ",  "{:<10.6g}", 1.234e-37f),
            ROW("1.234e-37  ", "{:<11.6g}", 1.234e-37f),
            ROW(" 1.234e-37 ", "{:^11.6g}", 1.234e-37f),
            // uppercase general .6
            ROW("1.234E-37",   "{:.6G}",    1.234e-37f),
            ROW("1.234E-37",   "{:9.6G}",   1.234e-37f),
            ROW(" 1.234E-37",  "{:10.6G}",  1.234e-37f),
            ROW("1.234E-37 ",  "{:<10.6G}", 1.234e-37f),
            ROW("1.234E-37  ", "{:<11.6G}", 1.234e-37f),
            ROW(" 1.234E-37 ", "{:^11.6G}", 1.234e-37f),

            ROW("1.23457e-37", "{:.6g}", 1.2345678e-37f),

            // general .6 ALTERNATE
            ROW("1.23400e-37",   "{:#.6g}",    1.234e-37f),
            ROW("1.23400e-37",   "{:#11.6g}",  1.234e-37f),
            ROW(" 1.23400e-37",  "{:#12.6g}",  1.234e-37f),
            ROW("1.23400e-37 ",  "{:<#12.6g}", 1.234e-37f),
            ROW("1.23400e-37  ", "{:<#13.6g}", 1.234e-37f),
            ROW(" 1.23400e-37 ", "{:^#13.6g}", 1.234e-37f),
            // uppercase general .6 ALTERNATE
            ROW("1.23400E-37",   "{:#.6G}",    1.234e-37f),
            ROW("1.23400E-37",   "{:#11.6G}",  1.234e-37f),
            ROW(" 1.23400E-37",  "{:#12.6G}",  1.234e-37f),
            ROW("1.23400E-37 ",  "{:<#12.6G}", 1.234e-37f),
            ROW("1.23400E-37  ", "{:<#13.6G}", 1.234e-37f),
            ROW(" 1.23400E-37 ", "{:^#13.6G}", 1.234e-37f),

            ROW("1.23457e-37", "{:#.6g}", 1.2345678e-37f),

            // general .0
            ROW("1e-37",   "{:.0g}",   1.234e-37f),
            ROW("1e-37",   "{:5.0g}",  1.234e-37f),
            ROW(" 1e-37",  "{:6.0g}",  1.234e-37f),
            ROW("1e-37 ",  "{:<6.0g}", 1.234e-37f),
            ROW("1e-37  ", "{:<7.0g}", 1.234e-37f),
            ROW(" 1e-37 ", "{:^7.0g}", 1.234e-37f),
            // uppercase general .0
            ROW("1E-37",   "{:.0G}",   1.234E-37f),
            ROW("1E-37",   "{:5.0G}",  1.234E-37f),
            ROW(" 1E-37",  "{:6.0G}",  1.234E-37f),
            ROW("1E-37 ",  "{:<6.0G}", 1.234E-37f),
            ROW("1E-37  ", "{:<7.0G}", 1.234E-37f),
            ROW(" 1E-37 ", "{:^7.0G}", 1.234E-37f),

            // general .0 ALTERNATE
            ROW("1.e-37",   "{:#.0g}",   1.234e-37f),
#if defined(_MSC_FULL_VER) && _MSC_FULL_VER > u_MSVC_LAST_BAD_VER
    // MSVC as Oracle has a bug with width when precision is 0
            ROW("1.e-37",   "{:#6.0g}",  1.234e-37f),
            ROW(" 1.e-37",  "{:#7.0g}",  1.234e-37f),
            ROW("1.e-37 ",  "{:<#7.0g}", 1.234e-37f),
            ROW("1.e-37  ", "{:<#8.0g}", 1.234e-37f),
            ROW(" 1.e-37 ", "{:^#8.0g}", 1.234e-37f),
#endif
            // uppercase general .0 ALTERNATE
            ROW("1.E-37",   "{:#.0G}",   1.234e-37f),
#if defined(_MSC_FULL_VER) && _MSC_FULL_VER > u_MSVC_LAST_BAD_VER
    // MSVC as Oracle has a bug with width when precision is 0
            ROW("1.E-37",   "{:#6.0G}",  1.234e-37f),
            ROW(" 1.E-37",  "{:#7.0G}",  1.234e-37f),
            ROW("1.E-37 ",  "{:<#7.0G}", 1.234e-37f),
            ROW("1.E-37  ", "{:<#8.0G}", 1.234e-37f),
            ROW(" 1.E-37 ", "{:^#8.0G}", 1.234e-37f),
#endif

            // general .1
            ROW("1e-37",   "{:.1g}",   1.234e-37f),
            ROW("1e-37",   "{:5.1g}",  1.234e-37f),
            ROW(" 1e-37",  "{:6.1g}",  1.234e-37f),
            ROW("1e-37 ",  "{:<6.1g}", 1.234e-37f),
            ROW("1e-37  ", "{:<7.1g}", 1.234e-37f),
            ROW(" 1e-37 ", "{:^7.1g}", 1.234e-37f),
            // uppercase general .1
            ROW("1E-37",   "{:.1G}",   1.234E-37f),
            ROW("1E-37",   "{:5.1G}",  1.234E-37f),
            ROW(" 1E-37",  "{:6.1G}",  1.234E-37f),
            ROW("1E-37 ",  "{:<6.1G}", 1.234E-37f),
            ROW("1E-37  ", "{:<7.1G}", 1.234E-37f),
            ROW(" 1E-37 ", "{:^7.1G}", 1.234E-37f),

            // general .1 ALTERNATE
            ROW("1.e-37",   "{:#.1g}",   1.234e-37f),
            ROW("1.e-37",   "{:#6.1g}",  1.234e-37f),
            ROW(" 1.e-37",  "{:#7.1g}",  1.234e-37f),
            ROW("1.e-37 ",  "{:<#7.1g}", 1.234e-37f),
            ROW("1.e-37  ", "{:<#8.1g}", 1.234e-37f),
            ROW(" 1.e-37 ", "{:^#8.1g}", 1.234e-37f),
            // uppercase general .1
            ROW("1.E-37",   "{:#.1G}",   1.234E-37f),
            ROW("1.E-37",   "{:#6.1G}",  1.234E-37f),
            ROW(" 1.E-37",  "{:#7.1G}",  1.234E-37f),
            ROW("1.E-37 ",  "{:<#7.1G}", 1.234E-37f),
            ROW("1.E-37  ", "{:<#8.1G}", 1.234E-37f),
            ROW(" 1.E-37 ", "{:^#8.1G}", 1.234E-37f),

            // general .2
            ROW("1.2e-37",   "{:.2g}",   1.234e-37f),
            ROW("1.2e-37",   "{:7.2g}",  1.234e-37f),
            ROW(" 1.2e-37",  "{:8.2g}",  1.234e-37f),
            ROW("1.2e-37 ",  "{:<8.2g}", 1.234e-37f),
            ROW("1.2e-37  ", "{:<9.2g}", 1.234e-37f),
            ROW(" 1.2e-37 ", "{:^9.2g}", 1.234e-37f),
            // uppercase general .2
            ROW("1.2E-37",   "{:.2G}",   1.234E-37f),
            ROW("1.2E-37",   "{:7.2G}",  1.234E-37f),
            ROW(" 1.2E-37",  "{:8.2G}",  1.234E-37f),
            ROW("1.2E-37 ",  "{:<8.2G}", 1.234E-37f),
            ROW("1.2E-37  ", "{:<9.2G}", 1.234E-37f),
            ROW(" 1.2E-37 ", "{:^9.2G}", 1.234E-37f),

            // general .2 ALTERNATE
            ROW("1.2e-37",   "{:#.2g}",   1.234e-37f),
            ROW("1.2e-37",   "{:#7.2g}",  1.234e-37f),
            ROW(" 1.2e-37",  "{:#8.2g}",  1.234e-37f),
            ROW("1.2e-37 ",  "{:<#8.2g}", 1.234e-37f),
            ROW("1.2e-37  ", "{:<#9.2g}", 1.234e-37f),
            ROW(" 1.2e-37 ", "{:^#9.2g}", 1.234e-37f),
            // uppercase general .2 ALTERNATE
            ROW("1.2E-37",   "{:#.2G}",   1.234E-37f),
            ROW("1.2E-37",   "{:#7.2G}",  1.234E-37f),
            ROW(" 1.2E-37",  "{:#8.2G}",  1.234E-37f),
            ROW("1.2E-37 ",  "{:<#8.2G}", 1.234E-37f),
            ROW("1.2E-37  ", "{:<#9.2G}", 1.234E-37f),
            ROW(" 1.2E-37 ", "{:^#9.2G}", 1.234E-37f),

            // general .50
            ROW("1.2340000190582515184895560080033662978123930243179e-37",
                                                     "{:.50g}",    1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179e-37",
                                                     "{:55.50g}",  1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179e-37",
                                                     "{:56.50g}",  1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179e-37 ",
                                                     "{:<56.50g}", 1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179e-37  ",
                                                     "{:<57.50g}", 1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179e-37 ",
                                                     "{:^57.50g}", 1.234e-37f),
            // uppercase general .50
            ROW("1.2340000190582515184895560080033662978123930243179E-37",
                                                     "{:.50G}",    1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179E-37",
                                                     "{:55.50G}",  1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179E-37",
                                                     "{:56.50G}",  1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179E-37 ",
                                                     "{:<56.50G}", 1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179E-37  ",
                                                     "{:<57.50G}", 1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179E-37 ",
                                                     "{:^57.50G}", 1.234e-37f),

            // general .50 ALTERNATE
            ROW("1.2340000190582515184895560080033662978123930243179e-37",
                                                    "{:#.50g}",    1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179e-37",
                                                    "{:#55.50g}",  1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179e-37",
                                                    "{:#56.50g}",  1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179e-37 ",
                                                    "{:<#56.50g}", 1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179e-37  ",
                                                    "{:<#57.50g}", 1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179e-37 ",
                                                    "{:^#57.50g}", 1.234e-37f),
            // uppercase general .50 ALTERNATE
            ROW("1.2340000190582515184895560080033662978123930243179E-37",
                                                    "{:#.50G}",    1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179E-37",
                                                    "{:#55.50G}",  1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179E-37",
                                                    "{:#56.50G}",  1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179E-37 ",
                                                    "{:<#56.50G}", 1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179E-37  ",
                                                    "{:<#57.50G}", 1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179E-37 ",
                                                    "{:^#57.50G}", 1.234e-37f),

            // fixed .6
            ROW("0.000000",   "{:.6f}",    0.f),
            ROW("0.000000",   "{:8.6f}",   0.f),
            ROW(" 0.000000",  "{:9.6f}",   0.f),
            ROW("0.000000 ",  "{:<9.6f}",  0.f),
            ROW("0.000000  ", "{:<10.6f}", 0.f),
            ROW(" 0.000000 ", "{:^10.6f}", 0.f),
            // uppercase fixed .6 (same as fixed)
            ROW("0.000000",   "{:.6F}",    0.f),
            ROW("0.000000",   "{:8.6F}",   0.f),
            ROW(" 0.000000",  "{:9.6F}",   0.f),
            ROW("0.000000 ",  "{:<9.6F}",  0.f),
            ROW("0.000000  ", "{:<10.6F}", 0.f),
            ROW(" 0.000000 ", "{:^10.6F}", 0.f),

            // fixed .6 ALTERNATE
            ROW("0.000000",   "{:#.6f}",    0.f),
            ROW("0.000000",   "{:#8.6f}",   0.f),
            ROW(" 0.000000",  "{:#9.6f}",   0.f),
            ROW("0.000000 ",  "{:<#9.6f}",  0.f),
            ROW("0.000000  ", "{:<#10.6f}", 0.f),
            ROW(" 0.000000 ", "{:^#10.6f}", 0.f),
            // uppercase fixed .6 ALTERNATE (same as fixed)
            ROW("0.000000",   "{:#.6F}",    0.f),
            ROW("0.000000",   "{:#8.6F}",   0.f),
            ROW(" 0.000000",  "{:#9.6F}",   0.f),
            ROW("0.000000 ",  "{:<#9.6F}",  0.f),
            ROW("0.000000  ", "{:<#10.6F}", 0.f),
            ROW(" 0.000000 ", "{:^#10.6F}", 0.f),

            // fixed .0
            ROW("0",   "{:.0f}",   0.f),
            ROW("0",   "{:1.0f}",  0.f),
            ROW(" 0",  "{:2.0f}",  0.f),
            ROW("0 ",  "{:<2.0f}", 0.f),
            ROW("0  ", "{:<3.0f}", 0.f),
            ROW(" 0 ", "{:^3.0f}", 0.f),
            // uppercase fixed .6 (same as fixed)
            ROW("0",   "{:.0F}",   0.f),
            ROW("0",   "{:1.0F}",  0.f),
            ROW(" 0",  "{:2.0F}",  0.f),
            ROW("0 ",  "{:<2.0F}", 0.f),
            ROW("0  ", "{:<3.0F}", 0.f),
            ROW(" 0 ", "{:^3.0F}", 0.f),
            // fixed .0 ALTERNATE
            ROW("0.",   "{:#.0f}",   0.f),
            ROW("0.",   "{:#2.0f}",  0.f),
            ROW(" 0.",  "{:#3.0f}",  0.f),
            ROW("0. ",  "{:<#3.0f}", 0.f),
            ROW("0.  ", "{:<#4.0f}", 0.f),
            ROW(" 0. ", "{:^#4.0f}", 0.f),
            // uppercase fixed .6 (same as fixed)
            ROW("0.",   "{:#.0F}",   0.f),
            ROW("0.",   "{:#2.0F}",  0.f),
            ROW(" 0.",  "{:#3.0F}",  0.f),
            ROW("0. ",  "{:<#3.0F}", 0.f),
            ROW("0.  ", "{:<#4.0F}", 0.f),
            ROW(" 0. ", "{:^#4.0F}", 0.f),

            // fixed .54
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                            "{:.54f}",    0.f),
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                            "{:56.54f}",  0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000000",
                                                            "{:57.54f}",  0.f),
            ROW("0.000000000000000000000000000000000000000000000000000000 ",
                                                            "{:<57.54f}", 0.f),
            ROW("0.000000000000000000000000000000000000000000000000000000  ",
                                                            "{:<58.54f}", 0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000000 ",
                                                            "{:^58.54f}", 0.f),
            // uppercase fixed .6 (same as fixed)
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                            "{:.54F}",    0.f),
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                            "{:56.54F}",  0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000000",
                                                            "{:57.54F}",  0.f),
            ROW("0.000000000000000000000000000000000000000000000000000000 ",
                                                            "{:<57.54F}", 0.f),
            ROW("0.000000000000000000000000000000000000000000000000000000  ",
                                                            "{:<58.54F}", 0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000000 ",
                                                            "{:^58.54F}", 0.f),
        };
        const size_t NUM_FLOAT_TESTS = sizeof FLOAT_TEST / sizeof *FLOAT_TEST;

        if (veryVerbose) puts("\tTesting `float` to `char`");
        for (size_t ti = 0; ti < NUM_FLOAT_TESTS; ++ti) {
            const int          LINE     = FLOAT_TEST[ti].d_line;
            const char * const EXPECTED = FLOAT_TEST[ti].d_expected;
            const char * const FORMAT   = FLOAT_TEST[ti].d_format;
            const float        VALUE    = FLOAT_TEST[ti].d_value;

            if (veryVeryVerbose) {
                P_(LINE) P_(EXPECTED) P_(FORMAT) P(VALUE);
            }

            testRuntimeCharParse<float>(LINE, FORMAT);
            testRuntimeFormat(LINE, EXPECTED, FORMAT, VALUE);
        }

        if (veryVerbose) puts("\tTesting `float` to `wchar_t`");
        for (size_t ti = 0; ti < NUM_FLOAT_TESTS; ++ti) {
            const int             LINE      = FLOAT_TEST[ti].d_line;
            const wchar_t * const WEXPECTED = FLOAT_TEST[ti].d_wexpected;
            const wchar_t * const WFORMAT   = FLOAT_TEST[ti].d_wformat;
            const float           VALUE     = FLOAT_TEST[ti].d_value;

            if (veryVeryVerbose) {
                P_(LINE) P(VALUE);
            }

            testRuntimeWcharParse<float>(LINE, WFORMAT);
            testRuntimeFormat(LINE, WEXPECTED, WFORMAT, VALUE);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // DOUBLE FORMATTING
        //
        // Concerns:
        // 1. All format specifiers and their combinations result in the
        //    expected string as per the C++ standard:
        //    1. Width
        //    2. Alignment
        //    3. Zero padding
        //    4. Format: General, fixed, scientific, hexfloat
        //    5. Uppercase formats
        //    6. Alternate formats
        //    7. Precision
        //    8. Sign
        //
        // 2. Special floating point values (NaN, Infinity) are formatted
        //    properly.
        //
        // 3. Negative values are formatted properly (for example padding is
        //    before the negative sign, zero padding is after the negative sign
        //    etc.)
        //
        // 4. Formatting works for `char` and `wchar_t` as well.
        //
        // Plan:
        // 1. Table based testing using the test facilities.
        //
        // Testing:
        //   CONCERN: `double` values are formatted properly
        // --------------------------------------------------------------------

        if (verbose) puts("\nDOUBLE FORMATTING"
                          "\n=================");

        {
            bsl::formatter<double, char>    dummy;   (void)dummy;
            bsl::formatter<double, wchar_t> wdummy;  (void)wdummy;
        }

        typedef std::numeric_limits<double> Limits;

        const double k_DBL_INF     = Limits::infinity();
        const double k_DBL_NEG_INF = -k_DBL_INF;

        const double k_DBL_NAN     = Limits::quiet_NaN();
        const double k_DBL_NEG_NAN = -k_DBL_NAN;

        const double k_DBL_SNAN     = Limits::signaling_NaN();

        // This probably won't be a signaling NaN as changing the sign may
        // actually count as a read, which (optionally signals and) turns
        // it into a quiet NaN.
        const double k_DBL_NEG_SNAN = -Limits::signaling_NaN();

        static struct {
            int            d_line;
            const char    *d_expected;
            const char    *d_format;
            const wchar_t *d_wexpected;
            const wchar_t *d_wformat;
            double         d_value;
        }  DOUBLE_TEST[] = {
#define ROW(expected, format, value)                      \
    { L_, expected, format, L##expected, L##format, value }

            // NO PRECISION

            // no format specifier
            ROW("0",   "{}",    0.),
            ROW("0",   "{:}",   0.),
            ROW("0",   "{:1}",  0.),
            ROW(" 0",  "{:2}",  0.),
            ROW("0 ",  "{:<2}", 0.),
            ROW("0  ", "{:<3}", 0.),
            ROW(" 0 ", "{:^3}", 0.),

            // Zero padding
            ROW("00",  "{:02}",  0.),
            ROW(" 0",  "{:>02}", 0.),
            ROW("0 ",  "{:<02}", 0.),
            ROW("0  ", "{:<03}", 0.),
            ROW(" 0 ", "{:^03}", 0.),

            // Zero padding for a negative value
            ROW("-01", "{:03}", -1.),
            // Zero padding for a positive value w/o sign
            ROW("01", "{:02}", 1.),
            // Zero padding for a positive value with sign
            ROW("+01", "{:+03}", 1.),
            // Zero padding for a positive value with space for sign
            ROW(" 01", "{: 03}", 1.),

            ROW("inf",  "{}", k_DBL_INF),
            ROW("-inf", "{}", k_DBL_NEG_INF),
            ROW("nan",  "{}", k_DBL_NAN),
            ROW("-nan", "{}", k_DBL_NEG_NAN),
            ROW("nan",  "{}", k_DBL_SNAN),
            ROW("-nan", "{}", k_DBL_NEG_SNAN),

            ROW("inf",  "{:3}", k_DBL_INF),
            ROW("-inf", "{:4}", k_DBL_NEG_INF),
            ROW("nan",  "{:3}", k_DBL_NAN),
            ROW("-nan", "{:4}", k_DBL_NEG_NAN),

            ROW(" inf",  "{:4}", k_DBL_INF),
            ROW(" -inf", "{:5}", k_DBL_NEG_INF),
            ROW(" nan",  "{:4}", k_DBL_NAN),
            ROW(" -nan", "{:5}", k_DBL_NEG_NAN),

            ROW(" inf",  "{:04}", k_DBL_INF),
            ROW(" -inf", "{:05}", k_DBL_NEG_INF),
            ROW(" nan",  "{:04}", k_DBL_NAN),
            ROW(" -nan", "{:05}", k_DBL_NEG_NAN),

            ROW("+inf", "{:+}", k_DBL_INF),
            ROW("-inf", "{:+}", k_DBL_NEG_INF),
            ROW("+nan", "{:+}", k_DBL_NAN),
            ROW("-nan", "{:+}", k_DBL_NEG_NAN),

            ROW(" inf", "{: }", k_DBL_INF),
            ROW("-inf", "{: }", k_DBL_NEG_INF),
            ROW(" nan", "{: }", k_DBL_NAN),
            ROW("-nan", "{: }", k_DBL_NEG_NAN),

            // hexfloat
            ROW("0p+0",   "{:a}",   0.),
            ROW("0p+0",   "{:4a}",  0.),
            ROW(" 0p+0",  "{:5a}",  0.),
            ROW("0p+0 ",  "{:<5a}", 0.),
            ROW("0p+0  ", "{:<6a}", 0.),
            ROW(" 0p+0 ", "{:^6a}", 0.),
            // uppercase hexfloat
            ROW("0P+0",   "{:A}",   0.),
            ROW("0P+0",   "{:4A}",  0.),
            ROW(" 0P+0",  "{:5A}",  0.),
            ROW("0P+0 ",  "{:<5A}", 0.),
            ROW("0P+0  ", "{:<6A}", 0.),
            ROW(" 0P+0 ", "{:^6A}", 0.),

            // hexfloat ALTERNATE
            ROW("0.p+0",   "{:#a}",   0.),
            ROW("0.p+0",   "{:#5a}",  0.),
            ROW(" 0.p+0",  "{:#6a}",  0.),
            ROW("0.p+0 ",  "{:<#6a}", 0.),
            ROW("0.p+0  ", "{:<#7a}", 0.),
            ROW(" 0.p+0 ", "{:^#7a}", 0.),
            // uppercase hexfloat ALTERNATE
            ROW("0.P+0",   "{:#A}",   0.),
            ROW("0.P+0",   "{:#5A}",  0.),
            ROW(" 0.P+0",  "{:#6A}",  0.),
            ROW("0.P+0 ",  "{:<#6A}", 0.),
            ROW("0.P+0  ", "{:<#7A}", 0.),
            ROW(" 0.P+0 ", "{:^#7A}", 0.),

            // scientific
            ROW("0.000000e+00",   "{:e}",    0.),
            ROW("0.000000e+00",   "{:12e}",  0.),
            ROW(" 0.000000e+00",  "{:13e}",  0.),
            ROW("0.000000e+00 ",  "{:<13e}", 0.),
            ROW("0.000000e+00  ", "{:<14e}", 0.),
            ROW(" 0.000000e+00 ", "{:^14e}", 0.),
            // uppercase scientific
            ROW("0.000000E+00",   "{:E}",    0.),
            ROW("0.000000E+00",   "{:12E}",  0.),
            ROW(" 0.000000E+00",  "{:13E}",  0.),
            ROW("0.000000E+00 ",  "{:<13E}", 0.),
            ROW("0.000000E+00  ", "{:<14E}", 0.),
            ROW(" 0.000000E+00 ", "{:^14E}", 0.),

            // scientific ALTERNATE
            ROW("0.000000e+00",   "{:#e}",    0.),
            ROW("0.000000e+00",   "{:#12e}",  0.),
            ROW(" 0.000000e+00",  "{:#13e}",  0.),
            ROW("0.000000e+00 ",  "{:<#13e}", 0.),
            ROW("0.000000e+00  ", "{:<#14e}", 0.),
            ROW(" 0.000000e+00 ", "{:^#14e}", 0.),
            // uppercase scientific ALTERNATE
            ROW("0.000000E+00",   "{:#E}",    0.),
            ROW("0.000000E+00",   "{:#12E}",  0.),
            ROW(" 0.000000E+00",  "{:#13E}",  0.),
            ROW("0.000000E+00 ",  "{:<#13E}", 0.),
            ROW("0.000000E+00  ", "{:<#14E}", 0.),
            ROW(" 0.000000E+00 ", "{:^#14E}", 0.),

            // fixed
            ROW("0.000000",   "{:f}",    0.),
            ROW("0.000000",   "{:8f}",   0.),
            ROW(" 0.000000",  "{:9f}",   0.),
            ROW("0.000000 ",  "{:<9f}",  0.),
            ROW("0.000000  ", "{:<10f}", 0.),
            ROW(" 0.000000 ", "{:^10f}", 0.),
            // uppercase fixed (same thing as fixed)
            ROW("0.000000",   "{:F}",    0.),
            ROW("0.000000",   "{:8F}",   0.),
            ROW(" 0.000000",  "{:9F}",   0.),
            ROW("0.000000 ",  "{:<9F}",  0.),
            ROW("0.000000  ", "{:<10F}", 0.),
            ROW(" 0.000000 ", "{:^10F}", 0.),
            // fixed ALTERNATE
            ROW("0.000000",   "{:#f}",    0.),
            ROW("0.000000",   "{: <#8f}", 0.),
            ROW(" 0.000000",  "{: #9f}",  0.),
            ROW("0.000000 ",  "{:<#9f}",  0.),
            ROW("0.000000  ", "{:<#10f}", 0.),
            ROW(" 0.000000 ", "{:^#10f}", 0.),
            // uppercase fixed (same thing as fixed)
            ROW("0.000000",   "{:#F}",    0.),
            ROW("0.000000",   "{: <#8F}", 0.),
            ROW(" 0.000000",  "{: #9F}",  0.),
            ROW("0.000000 ",  "{:<#9F}",  0.),
            ROW("0.000000  ", "{:<#10F}", 0.),
            ROW(" 0.000000 ", "{:^#10F}", 0.),

            // general
            ROW("0",   "{:g}",   0.),
            ROW("0",   "{:1g}",  0.),
            ROW(" 0",  "{:2g}",  0.),
            ROW("0 ",  "{:<2g}", 0.),
            ROW("0  ", "{:<3g}", 0.),
            ROW(" 0 ", "{:^3g}", 0.),
            // uppercase general
            ROW("0",   "{:G}",   0.),
            ROW("0",   "{:1G}",  0.),
            ROW(" 0",  "{:2G}",  0.),
            ROW("0 ",  "{:<2G}", 0.),
            ROW("0  ", "{:<3G}", 0.),
            ROW(" 0 ", "{:^3G}", 0.),

            // general ALTERNATE
            ROW("0.00000",   "{:#g}",   0.),
            ROW("0.00000",   "{:#7g}",  0.),
            ROW(" 0.00000",  "{:#8g}",  0.),
            ROW("0.00000 ",  "{:<#8g}", 0.),
            ROW("0.00000  ", "{:<#9g}", 0.),
            ROW(" 0.00000 ", "{:^#9g}", 0.),
            // uppercase general
            ROW("0.00000",   "{:#G}",   0.),
            ROW("0.00000",   "{:#7G}",  0.),
            ROW(" 0.00000",  "{:#8G}",  0.),
            ROW("0.00000 ",  "{:<#8G}", 0.),
            ROW("0.00000  ", "{:<#9G}", 0.),
            ROW(" 0.00000 ", "{:^#9G}", 0.),

            // general
            ROW("1.234e-37",   "{:g}",    1.234e-37),
            ROW("1.234e-37",   "{:9g}",   1.234e-37),
            ROW(" 1.234e-37",  "{:10g}",  1.234e-37),
            ROW("1.234e-37 ",  "{:<10g}", 1.234e-37),
            ROW("1.234e-37  ", "{:<11g}", 1.234e-37),
            ROW(" 1.234e-37 ", "{:^11g}", 1.234e-37),
            // uppercase general
            ROW("1.234E-37",   "{:G}",    1.234e-37),
            ROW("1.234E-37",   "{:9G}",   1.234e-37),
            ROW(" 1.234E-37",  "{:10G}",  1.234e-37),
            ROW("1.234E-37 ",  "{:<10G}", 1.234e-37),
            ROW("1.234E-37  ", "{:<11G}", 1.234e-37),
            ROW(" 1.234E-37 ", "{:^11G}", 1.234e-37),

            ROW("1.23457e-37", "{:g}", 1.2345678e-37),

            // general ALTERNATE
            ROW("1.23400e-37",   "{:#g}",    1.234e-37),
            ROW("1.23400e-37",   "{:#11g}",  1.234e-37),
            ROW(" 1.23400e-37",  "{:#12g}",  1.234e-37),
            ROW("1.23400e-37 ",  "{:<#12g}", 1.234e-37),
            ROW("1.23400e-37  ", "{:<#13g}", 1.234e-37),
            ROW(" 1.23400e-37 ", "{:^#13g}", 1.234e-37),
            // uppercase general
            ROW("1.23400E-37",   "{:#G}",    1.234e-37),
            ROW("1.23400E-37",   "{:#11G}",  1.234e-37),
            ROW(" 1.23400E-37",  "{:#12G}",  1.234e-37),
            ROW("1.23400E-37 ",  "{:<#12G}", 1.234e-37),
            ROW("1.23400E-37  ", "{:<#13G}", 1.234e-37),
            ROW(" 1.23400E-37 ", "{:^#13G}", 1.234e-37),

            ROW("1.23457e-37", "{:#g}", 1.2345678e-37),

            // WITH PRECISION
            // --------------

            // hexfloat .0
            ROW("0p+0",   "{:.0a}",   0.),
            ROW("0p+0",   "{:4.0a}",  0.),
            ROW(" 0p+0",  "{:5.0a}",  0.),
            ROW("0p+0 ",  "{:<5.0a}", 0.),
            ROW("0p+0  ", "{:<6.0a}", 0.),
            ROW(" 0p+0 ", "{:^6.0a}", 0.),
            // uppercase hexfloat .0
            ROW("0P+0",   "{:.0A}",   0.),
            ROW("0P+0",   "{:4.0A}",  0.),
            ROW(" 0P+0",  "{:5.0A}",  0.),
            ROW("0P+0 ",  "{:<5.0A}", 0.),
            ROW("0P+0  ", "{:<6.0A}", 0.),
            ROW(" 0P+0 ", "{:^6.0A}", 0.),

            // hexfloat .0 ALTERNATE
            ROW("0.p+0",   "{:#.0a}",   0.),
            ROW("0.p+0",   "{:#5.0a}",  0.),
            ROW(" 0.p+0",  "{:#6.0a}",  0.),
            ROW("0.p+0 ",  "{:<#6.0a}", 0.),
            ROW("0.p+0  ", "{:<#7.0a}", 0.),
            ROW(" 0.p+0 ", "{:^#7.0a}", 0.),
            // uppercase hexfloat .0 ALTERNATE
            ROW("0.P+0",   "{:#.0A}",   0.),
            ROW("0.P+0",   "{:#5.0A}",  0.),
            ROW(" 0.P+0",  "{:#6.0A}",  0.),
            ROW("0.P+0 ",  "{:<#6.0A}", 0.),
            ROW("0.P+0  ", "{:<#7.0A}", 0.),
            ROW(" 0.P+0 ", "{:^#7.0A}", 0.),

            // hexfloat .1
            ROW("0.0p+0",   "{:.1a}",   0.),
            ROW("0.0p+0",   "{:6.1a}",  0.),
            ROW(" 0.0p+0",  "{:7.1a}",  0.),
            ROW("0.0p+0 ",  "{:<7.1a}", 0.),
            ROW("0.0p+0  ", "{:<8.1a}", 0.),
            ROW(" 0.0p+0 ", "{:^8.1a}", 0.),
            // uppercase hexfloat .1
            ROW("0.0P+0",   "{:.1A}",   0.),
            ROW("0.0P+0",   "{:6.1A}",  0.),
            ROW(" 0.0P+0",  "{:7.1A}",  0.),
            ROW("0.0P+0 ",  "{:<7.1A}", 0.),
            ROW("0.0P+0  ", "{:<8.1A}", 0.),
            ROW(" 0.0P+0 ", "{:^8.1A}", 0.),

            // hexfloat .3
            ROW("0.000p+0",   "{:.3a}",    0.),
            ROW("0.000p+0",   "{:8.3a}",   0.),
            ROW(" 0.000p+0",  "{:9.3a}",   0.),
            ROW("0.000p+0 ",  "{:<9.3a}",  0.),
            ROW("0.000p+0  ", "{:<10.3a}", 0.),
            ROW(" 0.000p+0 ", "{:^10.3a}", 0.),
            // uppercase hexfloat .3
            ROW("0.000P+0",   "{:.3A}",    0.),
            ROW("0.000P+0",   "{:8.3A}",   0.),
            ROW(" 0.000P+0",  "{:9.3A}",   0.),
            ROW("0.000P+0 ",  "{:<9.3A}",  0.),
            ROW("0.000P+0  ", "{:<10.3A}", 0.),
            ROW(" 0.000P+0 ", "{:^10.3A}", 0.),

            // hexfloat .51
            ROW("0.000000000000000000000000000000000000000000000000000p+0",
                                                             "{:.51a}",    0.),
            ROW("0.000000000000000000000000000000000000000000000000000p+0",
                                                             "{:56.51a}",  0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000p+0",
                                                             "{:57.51a}",  0.),
            ROW("0.000000000000000000000000000000000000000000000000000p+0 ",
                                                             "{:<57.51a}", 0.),
            ROW("0.000000000000000000000000000000000000000000000000000p+0  ",
                                                             "{:<58.51a}", 0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000p+0 ",
                                                             "{:^58.51a}", 0.),
            // uppercase hexfloat .51
            ROW("0.000000000000000000000000000000000000000000000000000P+0",
                                                             "{:.51A}",    0.),
            ROW("0.000000000000000000000000000000000000000000000000000P+0",
                                                             "{:56.51A}",  0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000P+0",
                                                             "{:57.51A}",  0.),
            ROW("0.000000000000000000000000000000000000000000000000000P+0 ",
                                                             "{:<57.51A}", 0.),
            ROW("0.000000000000000000000000000000000000000000000000000P+0  ",
                                                             "{:<58.51A}", 0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000P+0 ",
                                                             "{:^58.51A}", 0.),

            // scientific .0
            ROW("0e+00",   "{:.0e}",   0.),
            ROW("0e+00",   "{:5.0e}",  0.),
            ROW(" 0e+00",  "{:6.0e}",  0.),
            ROW("0e+00 ",  "{:<6.0e}", 0.),
            ROW("0e+00  ", "{:<7.0e}", 0.),
            ROW(" 0e+00 ", "{:^7.0e}", 0.),
            // uppercase scientific .0
            ROW("0E+00",   "{:.0E}",   0.),
            ROW("0E+00",   "{:5.0E}",  0.),
            ROW(" 0E+00",  "{:6.0E}",  0.),
            ROW("0E+00 ",  "{:<6.0E}", 0.),
            ROW("0E+00  ", "{:<7.0E}", 0.),
            ROW(" 0E+00 ", "{:^7.0E}", 0.),

            // scientific .0 ALTERNATE
            ROW("0.e+00",   "{:#.0e}",   0.),
            ROW("0.e+00",   "{:#6.0e}",  0.),
            ROW(" 0.e+00",  "{:#7.0e}",  0.),
            ROW("0.e+00 ",  "{:<#7.0e}", 0.),
            ROW("0.e+00  ", "{:<#8.0e}", 0.),
            ROW(" 0.e+00 ", "{:^#8.0e}", 0.),
            // uppercase scientific .0
            ROW("0.E+00",   "{:#.0E}",   0.),
            ROW("0.E+00",   "{:#6.0E}",  0.),
            ROW(" 0.E+00",  "{:#7.0E}",  0.),
            ROW("0.E+00 ",  "{:<#7.0E}", 0.),
            ROW("0.E+00  ", "{:<#8.0E}", 0.),
            ROW(" 0.E+00 ", "{:^#8.0E}", 0.),

            // scientific .1
            ROW("0.0e+00",   "{:.1e}",   0.),
            ROW("0.0e+00",   "{:7.1e}",  0.),
            ROW(" 0.0e+00",  "{:8.1e}",  0.),
            ROW("0.0e+00 ",  "{:<8.1e}", 0.),
            ROW("0.0e+00  ", "{:<9.1e}", 0.),
            ROW(" 0.0e+00 ", "{:^9.1e}", 0.),
            // uppercase scientific .1
            ROW("0.0E+00",   "{:.1E}",   0.),
            ROW("0.0E+00",   "{:7.1E}",  0.),
            ROW(" 0.0E+00",  "{:8.1E}",  0.),
            ROW("0.0E+00 ",  "{:<8.1E}", 0.),
            ROW("0.0E+00  ", "{:<9.1E}", 0.),
            ROW(" 0.0E+00 ", "{:^9.1E}", 0.),

            // scientific .1 ALTERNATE
            ROW("0.0e+00",   "{:#.1e}",   0.),
            ROW("0.0e+00",   "{:#7.1e}",  0.),
            ROW(" 0.0e+00",  "{:#8.1e}",  0.),
            ROW("0.0e+00 ",  "{:<#8.1e}", 0.),
            ROW("0.0e+00  ", "{:<#9.1e}", 0.),
            ROW(" 0.0e+00 ", "{:^#9.1e}", 0.),
            // uppercase scientific .1 ALTERNATE
            ROW("0.0E+00",   "{:#.1E}",   0.),
            ROW("0.0E+00",   "{:#7.1E}",  0.),
            ROW(" 0.0E+00",  "{:#8.1E}",  0.),
            ROW("0.0E+00 ",  "{:<#8.1E}", 0.),
            ROW("0.0E+00  ", "{:<#9.1E}", 0.),
            ROW(" 0.0E+00 ", "{:^#9.1E}", 0.),

            // scientific .17
            ROW("0.00000000000000000e+00",   "{:.17e}" ,   0.),
            ROW("0.00000000000000000e+00",   "{:23.17e}",  0.),
            ROW(" 0.00000000000000000e+00",  "{:24.17e}",  0.),
            ROW("0.00000000000000000e+00 ",  "{:<24.17e}", 0.),
            ROW("0.00000000000000000e+00  ", "{:<25.17e}", 0.),
            ROW(" 0.00000000000000000e+00 ", "{:^25.17e}", 0.),
            // uppercase scientific .17
            ROW("0.00000000000000000E+00",   "{:.17E}" ,   0.),
            ROW("0.00000000000000000E+00",   "{:23.17E}",  0.),
            ROW(" 0.00000000000000000E+00",  "{:24.17E}",  0.),
            ROW("0.00000000000000000E+00 ",  "{:<24.17E}", 0.),
            ROW("0.00000000000000000E+00  ", "{:<25.17E}", 0.),
            ROW(" 0.00000000000000000E+00 ", "{:^25.17E}", 0.),

            // scientific .50
            ROW("0.00000000000000000000000000000000000000000000000000e+00",
                                                             "{:.50e}" ,   0.),
            ROW("0.00000000000000000000000000000000000000000000000000e+00",
                                                             "{:56.50e}",  0.),
            ROW(" 0.00000000000000000000000000000000000000000000000000e+00",
                                                             "{:57.50e}",  0.),
            ROW("0.00000000000000000000000000000000000000000000000000e+00 ",
                                                             "{:<57.50e}", 0.),
            ROW("0.00000000000000000000000000000000000000000000000000e+00  ",
                                                             "{:<58.50e}", 0.),
            ROW(" 0.00000000000000000000000000000000000000000000000000e+00 ",
                                                             "{:^58.50e}", 0.),
            // uppercase scientific .50
            ROW("0.00000000000000000000000000000000000000000000000000E+00",
                                                             "{:.50E}" ,   0.),
            ROW("0.00000000000000000000000000000000000000000000000000E+00",
                                                             "{:56.50E}",  0.),
            ROW(" 0.00000000000000000000000000000000000000000000000000E+00",
                                                             "{:57.50E}",  0.),
            ROW("0.00000000000000000000000000000000000000000000000000E+00 ",
                                                             "{:<57.50E}", 0.),
            ROW("0.00000000000000000000000000000000000000000000000000E+00  ",
                                                             "{:<58.50E}", 0.),
            ROW(" 0.00000000000000000000000000000000000000000000000000E+00 ",
                                                             "{:^58.50E}", 0.),

            // general .6
            ROW("1.234e-37",   "{:.6g}",    1.234e-37),
            ROW("1.234e-37",   "{:9.6g}",   1.234e-37),
            ROW(" 1.234e-37",  "{:10.6g}",  1.234e-37),
            ROW("1.234e-37 ",  "{:<10.6g}", 1.234e-37),
            ROW("1.234e-37  ", "{:<11.6g}", 1.234e-37),
            ROW(" 1.234e-37 ", "{:^11.6g}", 1.234e-37),
            // uppercase general .6
            ROW("1.234E-37",   "{:.6G}",    1.234e-37),
            ROW("1.234E-37",   "{:9.6G}",   1.234e-37),
            ROW(" 1.234E-37",  "{:10.6G}",  1.234e-37),
            ROW("1.234E-37 ",  "{:<10.6G}", 1.234e-37),
            ROW("1.234E-37  ", "{:<11.6G}", 1.234e-37),
            ROW(" 1.234E-37 ", "{:^11.6G}", 1.234e-37),

            ROW("1.23457e-37", "{:.6g}",    1.2345678e-37),

            // general .6 ALTERNATE
            ROW("1.23400e-37",   "{:#.6g}",    1.234e-37),
            ROW("1.23400e-37",   "{:#11.6g}",  1.234e-37),
            ROW(" 1.23400e-37",  "{:#12.6g}",  1.234e-37),
            ROW("1.23400e-37 ",  "{:<#12.6g}", 1.234e-37),
            ROW("1.23400e-37  ", "{:<#13.6g}", 1.234e-37),
            ROW(" 1.23400e-37 ", "{:^#13.6g}", 1.234e-37),
            // uppercase general .6 ALTERNATE
            ROW("1.23400E-37",   "{:#.6G}",    1.234e-37),
            ROW("1.23400E-37",   "{:#11.6G}",  1.234e-37),
            ROW(" 1.23400E-37",  "{:#12.6G}",  1.234e-37),
            ROW("1.23400E-37 ",  "{:<#12.6G}", 1.234e-37),
            ROW("1.23400E-37  ", "{:<#13.6G}", 1.234e-37),
            ROW(" 1.23400E-37 ", "{:^#13.6G}", 1.234e-37),

            ROW("1.23457e-37", "{:#.6g}", 1.2345678e-37),

            // general .0
            ROW("1e-37",   "{:.0g}",   1.234e-37),
            ROW("1e-37",   "{:5.0g}",  1.234e-37),
            ROW(" 1e-37",  "{:6.0g}",  1.234e-37),
            ROW("1e-37 ",  "{:<6.0g}", 1.234e-37),
            ROW("1e-37  ", "{:<7.0g}", 1.234e-37),
            ROW(" 1e-37 ", "{:^7.0g}", 1.234e-37),
            // uppercase general .0
            ROW("1E-37",   "{:.0G}",   1.234E-37),
            ROW("1E-37",   "{:5.0G}",  1.234E-37),
            ROW(" 1E-37",  "{:6.0G}",  1.234E-37),
            ROW("1E-37 ",  "{:<6.0G}", 1.234E-37),
            ROW("1E-37  ", "{:<7.0G}", 1.234E-37),
            ROW(" 1E-37 ", "{:^7.0G}", 1.234E-37),

            // general .0 ALTERNATE
            ROW("1.e-37",   "{:#.0g}",   1.234e-37),
#define u_MSVC_LAST_BAD_VER 194234435
#if defined(_MSC_FULL_VER) && _MSC_FULL_VER > u_MSVC_LAST_BAD_VER
    // MSVC as Oracle has a bug with width when precision is 0
            ROW("1.e-37",   "{:#6.0g}",  1.234e-37),
            ROW(" 1.e-37",  "{:#7.0g}",  1.234e-37),
            ROW("1.e-37 ",  "{:<#7.0g}", 1.234e-37),
            ROW("1.e-37  ", "{:<#8.0g}", 1.234e-37),
            ROW(" 1.e-37 ", "{:^#8.0g}", 1.234e-37),
#endif
            // uppercase general .0 ALTERNATE
            ROW("1.E-37",   "{:#.0G}",   1.234e-37),
#if defined(_MSC_FULL_VER) && _MSC_FULL_VER > u_MSVC_LAST_BAD_VER
    // MSVC as Oracle has a bug with width when precision is 0
            ROW("1.E-37",   "{:#6.0G}",  1.234e-37),
            ROW(" 1.E-37",  "{:#7.0G}",  1.234e-37),
            ROW("1.E-37 ",  "{:<#7.0G}", 1.234e-37),
            ROW("1.E-37  ", "{:<#8.0G}", 1.234e-37),
            ROW(" 1.E-37 ", "{:^#8.0G}", 1.234e-37),
#endif

            // general .1
            ROW("1e-37",   "{:.1g}",   1.234e-37),
            ROW("1e-37",   "{:5.1g}",  1.234e-37),
            ROW(" 1e-37",  "{:6.1g}",  1.234e-37),
            ROW("1e-37 ",  "{:<6.1g}", 1.234e-37),
            ROW("1e-37  ", "{:<7.1g}", 1.234e-37),
            ROW(" 1e-37 ", "{:^7.1g}", 1.234e-37),
            // uppercase general .1
            ROW("1E-37",   "{:.1G}",   1.234E-37),
            ROW("1E-37",   "{:5.1G}",  1.234E-37),
            ROW(" 1E-37",  "{:6.1G}",  1.234E-37),
            ROW("1E-37 ",  "{:<6.1G}", 1.234E-37),
            ROW("1E-37  ", "{:<7.1G}", 1.234E-37),
            ROW(" 1E-37 ", "{:^7.1G}", 1.234E-37),

            // general .1 ALTERNATE
            ROW("1.e-37",   "{:#.1g}",   1.234e-37),
            ROW("1.e-37",   "{:#6.1g}",  1.234e-37),
            ROW(" 1.e-37",  "{:#7.1g}",  1.234e-37),
            ROW("1.e-37 ",  "{:<#7.1g}", 1.234e-37),
            ROW("1.e-37  ", "{:<#8.1g}", 1.234e-37),
            ROW(" 1.e-37 ", "{:^#8.1g}", 1.234e-37),
            // uppercase general .1
            ROW("1.E-37",   "{:#.1G}",   1.234E-37),
            ROW("1.E-37",   "{:#6.1G}",  1.234E-37),
            ROW(" 1.E-37",  "{:#7.1G}",  1.234E-37),
            ROW("1.E-37 ",  "{:<#7.1G}", 1.234E-37),
            ROW("1.E-37  ", "{:<#8.1G}", 1.234E-37),
            ROW(" 1.E-37 ", "{:^#8.1G}", 1.234E-37),

            // general .2
            ROW("1.2e-37",   "{:.2g}",   1.234e-37),
            ROW("1.2e-37",   "{:7.2g}",  1.234e-37),
            ROW(" 1.2e-37",  "{:8.2g}",  1.234e-37),
            ROW("1.2e-37 ",  "{:<8.2g}", 1.234e-37),
            ROW("1.2e-37  ", "{:<9.2g}", 1.234e-37),
            ROW(" 1.2e-37 ", "{:^9.2g}", 1.234e-37),
            // uppercase general .2
            ROW("1.2E-37",   "{:.2G}",   1.234E-37),
            ROW("1.2E-37",   "{:7.2G}",  1.234E-37),
            ROW(" 1.2E-37",  "{:8.2G}",  1.234E-37),
            ROW("1.2E-37 ",  "{:<8.2G}", 1.234E-37),
            ROW("1.2E-37  ", "{:<9.2G}", 1.234E-37),
            ROW(" 1.2E-37 ", "{:^9.2G}", 1.234E-37),

            // general .2 ALTERNATE
            ROW("1.2e-37",   "{:#.2g}",   1.234e-37),
            ROW("1.2e-37",   "{:#7.2g}",  1.234e-37),
            ROW(" 1.2e-37",  "{:#8.2g}",  1.234e-37),
            ROW("1.2e-37 ",  "{:<#8.2g}", 1.234e-37),
            ROW("1.2e-37  ", "{:<#9.2g}", 1.234e-37),
            ROW(" 1.2e-37 ", "{:^#9.2g}", 1.234e-37),
            // uppercase general .2 ALTERNATE
            ROW("1.2E-37",   "{:#.2G}",   1.234E-37),
            ROW("1.2E-37",   "{:#7.2G}",  1.234E-37),
            ROW(" 1.2E-37",  "{:#8.2G}",  1.234E-37),
            ROW("1.2E-37 ",  "{:<#8.2G}", 1.234E-37),
            ROW("1.2E-37  ", "{:<#9.2G}", 1.234E-37),
            ROW(" 1.2E-37 ", "{:^#9.2G}", 1.234E-37),

            // general .50
            ROW("1.2340000000000000187836107844281224919182953523414e-37",
                                                      "{:.50g}",    1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414e-37",
                                                      "{:55.50g}",  1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414e-37",
                                                      "{:56.50g}",  1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414e-37 ",
                                                      "{:<56.50g}", 1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414e-37  ",
                                                      "{:<57.50g}", 1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414e-37 ",
                                                      "{:^57.50g}", 1.234e-37),
            // uppercase general .50
            ROW("1.2340000000000000187836107844281224919182953523414E-37",
                                                      "{:.50G}",    1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414E-37",
                                                      "{:55.50G}",  1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414E-37",
                                                      "{:56.50G}",  1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414E-37 ",
                                                      "{:<56.50G}", 1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414E-37  ",
                                                      "{:<57.50G}", 1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414E-37 ",
                                                      "{:^57.50G}", 1.234e-37),

            // general .50 ALTERNATE
            ROW("1.2340000000000000187836107844281224919182953523414e-37",
                                                     "{:#.50g}",    1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414e-37",
                                                     "{:#55.50g}",  1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414e-37",
                                                     "{:#56.50g}",  1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414e-37 ",
                                                     "{:<#56.50g}", 1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414e-37  ",
                                                     "{:<#57.50g}", 1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414e-37 ",
                                                     "{:^#57.50g}", 1.234e-37),
            // uppercase general .50 ALTERNATE
            ROW("1.2340000000000000187836107844281224919182953523414E-37",
                                                     "{:#.50G}",    1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414E-37",
                                                     "{:#55.50G}",  1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414E-37",
                                                    "{:#56.50G}",  1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414E-37 ",
                                                     "{:<#56.50G}", 1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414E-37  ",
                                                     "{:<#57.50G}", 1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414E-37 ",
                                                     "{:^#57.50G}", 1.234e-37),

            // fixed .6
            ROW("0.000000",   "{:.6f}",    0.),
            ROW("0.000000",   "{:8.6f}",   0.),
            ROW(" 0.000000",  "{:9.6f}",   0.),
            ROW("0.000000 ",  "{:<9.6f}",  0.),
            ROW("0.000000  ", "{:<10.6f}", 0.),
            ROW(" 0.000000 ", "{:^10.6f}", 0.),
            // uppercase fixed .6 (same as fixed)
            ROW("0.000000",   "{:.6F}",    0.),
            ROW("0.000000",   "{:8.6F}",   0.),
            ROW(" 0.000000",  "{:9.6F}",   0.),
            ROW("0.000000 ",  "{:<9.6F}",  0.),
            ROW("0.000000  ", "{:<10.6F}", 0.),
            ROW(" 0.000000 ", "{:^10.6F}", 0.),

            // fixed .6 ALTERNATE
            ROW("0.000000",   "{:#.6f}",    0.),
            ROW("0.000000",   "{:#8.6f}",   0.),
            ROW(" 0.000000",  "{:#9.6f}",   0.),
            ROW("0.000000 ",  "{:<#9.6f}",  0.),
            ROW("0.000000  ", "{:<#10.6f}", 0.),
            ROW(" 0.000000 ", "{:^#10.6f}", 0.),
            // uppercase fixed .6 ALTERNATE (same as fixed)
            ROW("0.000000",   "{:#.6F}",    0.),
            ROW("0.000000",   "{:#8.6F}",   0.),
            ROW(" 0.000000",  "{:#9.6F}",   0.),
            ROW("0.000000 ",  "{:<#9.6F}",  0.),
            ROW("0.000000  ", "{:<#10.6F}", 0.),
            ROW(" 0.000000 ", "{:^#10.6F}", 0.),

            // fixed .0
            ROW("0",   "{:.0f}",   0.),
            ROW("0",   "{:1.0f}",  0.),
            ROW(" 0",  "{:2.0f}",  0.),
            ROW("0 ",  "{:<2.0f}", 0.),
            ROW("0  ", "{:<3.0f}", 0.),
            ROW(" 0 ", "{:^3.0f}", 0.),
            // uppercase fixed .6 (same as fixed)
            ROW("0",   "{:.0F}",   0.),
            ROW("0",   "{:1.0F}",  0.),
            ROW(" 0",  "{:2.0F}",  0.),
            ROW("0 ",  "{:<2.0F}", 0.),
            ROW("0  ", "{:<3.0F}", 0.),
            ROW(" 0 ", "{:^3.0F}", 0.),
            // fixed .0 ALTERNATE
            ROW("0.",   "{:#.0f}",   0.),
            ROW("0.",   "{:#2.0f}",  0.),
            ROW(" 0.",  "{:#3.0f}",  0.),
            ROW("0. ",  "{:<#3.0f}", 0.),
            ROW("0.  ", "{:<#4.0f}", 0.),
            ROW(" 0. ", "{:^#4.0f}", 0.),
            // uppercase fixed .6 (same as fixed)
            ROW("0.",   "{:#.0F}",   0.),
            ROW("0.",   "{:#2.0F}",  0.),
            ROW(" 0.",  "{:#3.0F}",  0.),
            ROW("0. ",  "{:<#3.0F}", 0.),
            ROW("0.  ", "{:<#4.0F}", 0.),
            ROW(" 0. ", "{:^#4.0F}", 0.),

            // fixed .54
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                             "{:.54f}",    0.),
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                             "{:56.54f}",  0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000000",
                                                             "{:57.54f}",  0.),
            ROW("0.000000000000000000000000000000000000000000000000000000 ",
                                                             "{:<57.54f}", 0.),
            ROW("0.000000000000000000000000000000000000000000000000000000  ",
                                                             "{:<58.54f}", 0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000000 ",
                                                             "{:^58.54f}", 0.),
            // uppercase fixed .6 (same as fixed)
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                             "{:.54F}",    0.),
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                             "{:56.54F}",  0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000000",
                                                             "{:57.54F}",  0.),
            ROW("0.000000000000000000000000000000000000000000000000000000 ",
                                                             "{:<57.54F}", 0.),
            ROW("0.000000000000000000000000000000000000000000000000000000  ",
                                                             "{:<58.54F}", 0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000000 ",
                                                             "{:^58.54F}", 0.),
        };
        const size_t NUM_DOUBLE_TESTS =
                                      sizeof DOUBLE_TEST / sizeof *DOUBLE_TEST;

        if (veryVerbose) puts("\tTesting `double` to `char`");
        for (size_t ti = 0; ti < NUM_DOUBLE_TESTS; ++ti) {
            const int          LINE     = DOUBLE_TEST[ti].d_line;
            const char * const EXPECTED = DOUBLE_TEST[ti].d_expected;
            const char * const FORMAT   = DOUBLE_TEST[ti].d_format;
            const double       VALUE    = DOUBLE_TEST[ti].d_value;

            if (veryVeryVerbose) {
                P_(LINE) P_(EXPECTED) P_(FORMAT) P(VALUE);
            }

            testRuntimeCharParse<double>(LINE, FORMAT);
            testRuntimeFormat(LINE, EXPECTED, FORMAT, VALUE);
        }

        if (veryVerbose) puts("\tTesting `double` to `wchar_t`");
        for (size_t ti = 0; ti < NUM_DOUBLE_TESTS; ++ti) {
            const int             LINE      = DOUBLE_TEST[ti].d_line;
            const wchar_t * const WEXPECTED = DOUBLE_TEST[ti].d_wexpected;
            const wchar_t * const WFORMAT   = DOUBLE_TEST[ti].d_wformat;
            const double          VALUE     = DOUBLE_TEST[ti].d_value;

            if (veryVeryVerbose) {
                P_(LINE) P(VALUE);
            }

            testRuntimeWcharParse<double>(LINE, WFORMAT);
            testRuntimeFormat(LINE, WEXPECTED, WFORMAT, VALUE);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // INDIVIDUAL METHODS
        //
        // Concerns:
        // 1. The `parse` and `format` functions return the proper iterator
        //    values.
        //
        // Plan:
        // 1. Verified by the test harness.
        //
        // Testing:
        //   iterator format(t_VALUE, t_FORMAT_CONTEXT& ) const;
        //   iterator parse(t_PARSE_CONTEXT& );
        // --------------------------------------------------------------------

        if (verbose) puts("\nINDIVIDUAL METHODS"
                          "\n==================");

        testRuntimeCharParse<double>(L_, "{}");
        testRuntimeFormat(L_, "2", "{}", 2.);

        testRuntimeCharParse<double>(L_, "{:{}}");
        testRuntimeFormat(L_, "  2", "{:{}}", 2., 3);

        testRuntimeCharParse<double>(L_, "{:.{}}");
        testRuntimeFormat(L_, "2.12", "{:.{}}", 2.1234, 3);

        testRuntimeCharParse<double>(L_, "{:{}.{}}");
        testRuntimeFormat(L_, "  2.12", "{:{}.{}}", 2.1234, 6, 3);

        testRuntimeCharParse<float>(L_, "{}");
        testRuntimeFormat(L_, "2", "{}", 2.);

        testRuntimeCharParse<float>(L_, "{:{}}");
        testRuntimeFormat(L_, "  2", "{:{}}", 2., 3);

        testRuntimeCharParse<float>(L_, "{:.{}}");
        testRuntimeFormat(L_, "2.12", "{:.{}}", 2.1234, 3);

        testRuntimeCharParse<float>(L_, "{:{}.{}}");
        testRuntimeFormat(L_, "  2.12", "{:{}.{}}", 2.1234, 6, 3);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises the component but tests nothing.
        //
        // Concerns:
        // 1. The code is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. TBD
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

        // This test case is intentionally empty.  Complete testing is done in
        // the subsequent test cases.
      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: No memory came from the global allocator.

    LOOP2_ASSERT(test, globalAllocator.numBlocksTotal(),
                 0 == globalAllocator.numBlocksTotal());

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
