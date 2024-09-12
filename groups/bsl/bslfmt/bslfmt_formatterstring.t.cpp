// bslstl_formatterstring.t.cpp                                       -*-C++-*-
#include <bslfmt_formatterstring.h>

#include <bsls_bsltestutil.h>

#include <bslstl_algorithm.h>
#include <bslstl_string.h>

#include <stdio.h>
#include <string.h>

#include <bslfmt_formatimp.h> // Testing only (`bsl::format` - breathing test)

#include <bslfmt_formattertestutil.h> // Testing only

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bsl::formatter<t_TYPE, t_CHAR>' where t_TYPE is a string type is a
// partial specialization of 'bsl::formatter' to enable formatting of
// strings. It is hard to test standalone as the interface is designed to
// be called by the 'bslfmt::format' suite of functions, and such testing
// requires the creation of "mock" contexts. The 'format' and 'parse'
// functions are designed to be called in order, and it is not possible to
// test a successful parse individually in isolation. As a result test 10
// ('parse') will focus on parsing failures while parsing successes will be
// tested in test 11 ('format').
// 
// It should meet the requirements specified in [format.string.std].
//
//-----------------------------------------------------------------------------
// CLASS 'bsl::formtter'
//
// CREATORS
// [ 3] formatter();
// [ 3] ~formatter();
// [ 7] formatter(const formatter &);
//
// MANIPULATORS
// [ 9] operator=(const formatter &);
// [11] parse(PARSE_CONTEXT&);
//
// ACCESSORS
// [12] format(TYPE, FORMAT_CONTEXT&);
//
// FREE FUNCTIONS
// [ 8] swap(formatter &, formatter&);
//
// OUTPUT
// [ 5] parsing failures
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING PRIMARY MANIPULATORS: Not Applicable
// [ 4] TESTING BASIC ACCESSORS:      Not Applicable
// [ 5] TESTING OUTPUT:               Not Applicable
// [ 6] TESTING EQUALITY OPERATOR:    Not Applicable
// [10] STREAMING FUNCTIONALITY:      Not Applicable
// [13] USAGE EXAMPLE
//-----------------------------------------------------------------------------


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
//                  GLOBAL HELPER MACROS FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
#define UTF8_LITERAL(...)                                                     \
    static_cast<const char *>(static_cast<const void *>(u8##__VA_ARGS__))
#else
#define UTF8_LITERAL(EXPR) EXPR
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY)
#define TEST_STD_STRING_VIEW std::basic_string_view
#else
#define TEST_STD_STRING_VIEW bsl::basic_string_view
#endif

#define TEST_PARSE_FAIL(type, fmtStr, useOracle)                               \
    {                                                                          \
        bsl::string errorMsg;                                                  \
        bool rv;                                                               \
        rv = bslfmt::Formatter_TestUtil<type>::                                \
                 testParseFailure<const type *>(&errorMsg, useOracle, fmtStr); \
        if (!rv) {                                                             \
            bsl::basic_string<type> formatStr(fmtStr);                         \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                  \
        }                                                                      \
    }



#define TEST_PARSE_SUCCESS_F(type, fmtStr, useOracle)                          \
    {                                                                          \
        bsl::string errorMsg;                                                  \
        bool rv;                                                               \
                                                                               \
        const type *dummyArg1 = 0;                                             \
        rv = bslfmt::Formatter_TestUtil<type>::                                \
                 testParseFormat<const type *>(&errorMsg,                      \
                                  useOracle,                                   \
                                  fmtStr);                                     \
        if (!rv) {                                                             \
            bsl::basic_string<type> formatStr(fmtStr);                         \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                  \
        }                                                                      \
                                                                               \
        type *dummyArg2 = 0;                                                   \
        rv = bslfmt::Formatter_TestUtil<type>::                                \
                 testParseFormat<type *>(&errorMsg,                            \
                                  useOracle,                                   \
                                  fmtStr);                                     \
        if (!rv) {                                                             \
            bsl::basic_string<type> formatStr(fmtStr);                         \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                  \
        }                                                                      \
                                                                               \
                                                                               \
        rv = bslfmt::Formatter_TestUtil<type>::                                \
                 testParseFormat<type[10]>(&errorMsg,                          \
                                  useOracle,                                   \
                                  fmtStr);                                     \
        if (!rv) {                                                             \
            bsl::basic_string<type> formatStr(fmtStr);                         \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                  \
        }                                                                      \
                                                                               \
        bsl::basic_string<type> dummyArg4;                                     \
        rv = bslfmt::Formatter_TestUtil<type>::                                \
                 testParseFormat<bsl::basic_string<type> >(&errorMsg,          \
                                  useOracle,                                   \
                                  fmtStr);                                     \
        if (!rv) {                                                             \
            bsl::basic_string<type> formatStr(fmtStr);                         \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                  \
        }                                                                      \
                                                                               \
        std::basic_string<type> dummyArg5;                                     \
        rv = bslfmt::Formatter_TestUtil<type>::                                \
                 testParseFormat<std::basic_string<type> > (&errorMsg,         \
                                  useOracle,                                   \
                                  fmtStr);                                     \
        if (!rv) {                                                             \
            bsl::basic_string<type> formatStr(fmtStr);                         \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                  \
        }                                                                      \
                                                                               \
        constexpr bsl::basic_string_view<type> dummyArg6;                      \
        rv = bslfmt::Formatter_TestUtil<type>::                                \
                 testParseFormat<bsl::basic_string_view<type> >(&errorMsg,     \
                                  useOracle,                                   \
                                  fmtStr);                                     \
        if (!rv) {                                                             \
            bsl::basic_string<type> formatStr(fmtStr);                         \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                  \
        }                                                                      \
                                                                               \
        constexpr TEST_STD_STRING_VIEW<type> dummyArg7;                        \
        rv = bslfmt::Formatter_TestUtil<type>::                                \
                 testParseFormat<TEST_STD_STRING_VIEW<type> >(                 \
                                 &errorMsg,                                    \
                                  useOracle,                                   \
                                  fmtStr);                                     \
        if (!rv) {                                                             \
            bsl::basic_string<type> formatStr(fmtStr);                         \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                  \
        }                                                                      \
    }

#define TEST_PARSE_SUCCESS_VF(type, fmtStr, useOracle)                        \
    {                                                                         \
        bsl::string errorMsg;                                                 \
        bool        rv;                                                       \
                                                                              \
        rv = bslfmt::Formatter_TestUtil<type>::testParseVFormat<const type *> \
                                                              (&errorMsg,     \
                                                               useOracle,     \
                                                               fmtStr);       \
        if (!rv) {                                                            \
            bsl::basic_string<type> formatStr(fmtStr);                        \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                 \
        }                                                                     \
                                                                              \
        rv = bslfmt::Formatter_TestUtil<type>::testParseVFormat<type *>       \
                                                              (&errorMsg,     \
                                                               useOracle,     \
                                                               fmtStr);       \
        if (!rv) {                                                            \
            bsl::basic_string<type> formatStr(fmtStr);                        \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                 \
        }                                                                     \
                                                                              \
        rv = bslfmt::Formatter_TestUtil<type>::testParseVFormat<type[10]>     \
                                                              (&errorMsg,     \
                                                               useOracle,     \
                                                               fmtStr);       \
        if (!rv) {                                                            \
            bsl::basic_string<type> formatStr(fmtStr);                        \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                 \
        }                                                                     \
                                                                              \
        rv = bslfmt::Formatter_TestUtil<type>::testParseVFormat               \
                                                    <bsl::basic_string<type> >\
                                                              (&errorMsg,     \
                                                               useOracle,     \
                                                               fmtStr);       \
        if (!rv) {                                                            \
            bsl::basic_string<type> formatStr(fmtStr);                        \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                 \
        }                                                                     \
                                                                              \
        rv = bslfmt::Formatter_TestUtil<type>::testParseVFormat               \
                                                    <std::basic_string<type> >\
                                                              (&errorMsg,     \
                                                               useOracle,     \
                                                               fmtStr);       \
        if (!rv) {                                                            \
            bsl::basic_string<type> formatStr(fmtStr);                        \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                 \
        }                                                                     \
                                                                              \
        rv = bslfmt::Formatter_TestUtil<type>::testParseVFormat               \
                                               <bsl::basic_string_view<type> >\
                                                              (&errorMsg,     \
                                                               useOracle,     \
                                                               fmtStr);       \
        if (!rv) {                                                            \
            bsl::basic_string<type> formatStr(fmtStr);                        \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                 \
        }                                                                     \
                                                                              \
        rv = bslfmt::Formatter_TestUtil<type>::testParseVFormat               \
                                                 <TEST_STD_STRING_VIEW<type> >\
                                                              (&errorMsg,     \
                                                               useOracle,     \
                                                               fmtStr);       \
        if (!rv) {                                                            \
            bsl::basic_string<type> formatStr(fmtStr);                        \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                 \
        }                                                                     \
    }

// ============================================================================
//                     GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE <= 13
static const bool k_ORACLE_SUPPORT_UNICODE = false;
#else
static const bool k_ORACLE_SUPPORT_UNICODE = true;
#endif

static const int k_FILLCHAR_EMPTY   = 0;
static const int k_FILLCHAR_ASCII   = 1;
static const int k_FILLCHAR_UNICODE = 2;
static const int k_FILLCHAR_DOUBLE  = 3;
static const int k_FILLCHAR_COUNT   = 4;

static const char    *FILLERS_C[] = {UTF8_LITERAL(""),
                                     UTF8_LITERAL("*"),
                                     UTF8_LITERAL("\U00000401"),
                                     UTF8_LITERAL("\U0001F680")};
static const wchar_t *FILLERS_W[] = {L"", L"*", L"\U00000401", L"\U0001F680"};

static const int k_FILL_NONE   = 0;
static const int k_FILL_LEFT   = 1;
static const int k_FILL_RIGHT  = 2;
static const int k_FILL_MIDDLE = 3;
static const int k_FILL_COUNT  = 4;

static const char    *FILL_C[] = {UTF8_LITERAL(""),
                                  UTF8_LITERAL(">"),
                                  UTF8_LITERAL("<"),
                                  UTF8_LITERAL("^")};
static const wchar_t *FILL_W[] = {L"", L">", L"<", L"^"};

static const int k_VALUE_NONE            = 0;
static const int k_VALUE_ASCII           = 1;
static const int k_VALUE_UNICODE         = 2;
static const int k_VALUE_DOUBLE          = 3;
static const int k_VALUE_UNICODE_COMPLEX = 4;
static const int k_VALUE_COUNT           = 5;

static const char    *VALUE_C[] = {UTF8_LITERAL(""),
                                   UTF8_LITERAL("a"),
                                   UTF8_LITERAL("\U00013000"),
                                   UTF8_LITERAL("\U0001F600"),
                                   UTF8_LITERAL("\U0000006e\U00000303")};
static const wchar_t *VALUE_W[] = {L"",
                                   L"a",
                                   L"\U00013000",
                                   L"\U0001F600",
                                   L"\U0000006e\U00000303"};

static const int k_ARG_VALUE         = 0;
static const int k_ARG_NESTED_NON_ID = 1;
static const int k_ARG_NESTED_ARG_ID = 2;
static const int k_ARG_COUNT         = 2;



// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------


int getFillWidth(int fillChar)
{
    switch (fillChar) {
      case 0: {
        return 1;
      } break;
      case 1: {
        return 1;
      } break;
      case 2: {
        return 1;
      } break;
      case 3: {
        return 2;
      } break;
    }
    return 0;
}

int getContentWidth(int contentChar)
{
    switch (contentChar) {
      case 0: {
        return 0;
      } break;
      case 1: {
        return 1;
      } break;
      case 2: {
        return 1;
      } break;
      case 3: {
        return 2;
      } break;
      case 4: {
        return 1;
      } break;
    }
    return 0;
}

// ============================================================================
//                     GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------





template <class t_CHAR>
struct ResultCalculator {
};

template<>
struct ResultCalculator<char> {
    static void calculate(bool        *isUnicodeSupportRequired,
                          bsl::string *formatString,
                          bsl::string *inputString,
                          bsl::string *outputString,
                          int          fillChar,
                          int          fillType,
                          int          contentType,
                          int          argType,
                          int          contentCopyCount,
                          int          width,
                          int          precision)
    {
        if (width == 0)
            width = -1;
        if (fillType == k_FILL_NONE)
            fillChar = k_FILLCHAR_EMPTY;

        *isUnicodeSupportRequired = false;

        if (fillChar >= k_FILLCHAR_UNICODE)
            *isUnicodeSupportRequired = true;

        if (contentType >= k_VALUE_UNICODE)
            *isUnicodeSupportRequired = true;

        int         contentWidth = getContentWidth(contentType);

        std::string fmt;
        std::string contentString;
        std::string result;
        std::string resultInfill;

        int precisionLeft     = (precision < 0) ? 99 : precision;
        int contentCopiesLeft = (contentCopyCount < 0) ? 99 : contentCopyCount;
        int widthUsed         = 0;

        while (precisionLeft > 0 && contentCopiesLeft > 0 &&
               contentWidth <= precisionLeft) {
            contentString += VALUE_C[contentType];
            contentCopiesLeft--;
            resultInfill += VALUE_C[contentType];
            precisionLeft -= contentWidth;
            widthUsed += contentWidth;
        }

        int padding = 0;
        if (width > 0)
            padding = bsl::max(0, width - widthUsed);

        int leftPad = 0;
        int rightPad = 0;

        if (fillType == k_FILL_LEFT)
            leftPad = padding;
        if (fillType == k_FILL_MIDDLE)
            leftPad = padding / 2;

        if (fillType == k_FILL_RIGHT || fillType == k_FILL_NONE)
            rightPad = padding;
        if (fillType == k_FILL_MIDDLE)
            rightPad = (padding+1) / 2;

        for (int i = 0; i < leftPad; i++) {
            const char *filler = FILLERS_C[fillChar];
            if (fillChar == k_FILLCHAR_EMPTY)
                filler = " ";
            result += filler;
        }

        result += resultInfill;

        for (int i = 0; i < rightPad; i++) {
            const char *filler = FILLERS_C[fillChar];
            if (fillChar == k_FILLCHAR_EMPTY)
                filler = " ";
            result += filler;
        }

        fmt = "{";
        if (argType == k_ARG_NESTED_ARG_ID)
            fmt += "0";
        fmt += ":";
        fmt += FILLERS_C[fillChar];
        fmt += FILL_C[fillType];
        if (width >= 0) {
            if (argType == k_ARG_NESTED_ARG_ID)
                fmt += "{1}";
            else if (argType == k_ARG_NESTED_NON_ID)
                fmt += "{}";
            else fmt += bsl::to_string(width);
        }
        if (precision >= 0) {
            fmt += ".";
            if (argType == k_ARG_NESTED_ARG_ID)
                fmt += "{2}";
            else if (argType == k_ARG_NESTED_NON_ID)
                fmt += "{}";
            else fmt += bsl::to_string(precision);
        }

        fmt += "s}";

        *formatString = fmt;
        *inputString  = contentString;
        *outputString = result;
    }
};

template <>
struct ResultCalculator<wchar_t> {
    static void calculate(bool         *isUnicodeSupportRequired,
                          bsl::wstring *formatString,
                          bsl::wstring *inputString,
                          bsl::wstring *outputString,
                          int           fillChar,
                          int           fillType,
                          int           contentType,
                          int           argType,
                          int           contentCopyCount,
                          int           width,
                          int           precision)
    {
        if (width == 0)
            width = -1;
        if (fillType == k_FILL_NONE)
            fillChar = k_FILLCHAR_EMPTY;

        *isUnicodeSupportRequired = false;

        if (fillChar >= k_FILLCHAR_UNICODE)
            *isUnicodeSupportRequired = true;

        if (contentType >= k_VALUE_UNICODE)
            *isUnicodeSupportRequired = true;

        int contentWidth = getContentWidth(contentType);

        std::wstring fmt;
        std::wstring contentString;
        std::wstring result;
        std::wstring resultInfill;

        int precisionLeft     = (precision < 0) ? 99 : precision;
        int contentCopiesLeft = (contentCopyCount < 0) ? 99 : contentCopyCount;
        int widthUsed         = 0;

        while (precisionLeft > 0 && contentCopiesLeft > 0 &&
               contentWidth <= precisionLeft) {
            contentString += VALUE_W[contentType];
            contentCopiesLeft--;
            resultInfill += VALUE_W[contentType];
            precisionLeft -= contentWidth;
            widthUsed += contentWidth;
        }

        int padding = 0;
        if (width > 0)
            padding = bsl::max(0, width - widthUsed);

        int leftPad  = 0;
        int rightPad = 0;

        if (fillType == k_FILL_LEFT)
            leftPad = padding;
        if (fillType == k_FILL_MIDDLE)
            leftPad = padding / 2;

        if (fillType == k_FILL_RIGHT || fillType == k_FILL_NONE)
            rightPad = padding;
        if (fillType == k_FILL_MIDDLE)
            rightPad = (padding + 1) / 2;

        for (int i = 0; i < leftPad; i++) {
            const wchar_t *filler = FILLERS_W[fillChar];
            if (fillChar == k_FILLCHAR_EMPTY)
                filler = L" ";
            result += filler;
        }

        result += resultInfill;

        for (int i = 0; i < rightPad; i++) {
            const wchar_t *filler = FILLERS_W[fillChar];
            if (fillChar == k_FILLCHAR_EMPTY)
                filler = L" ";
            result += filler;
        }

        fmt = L"{";
        if (argType == k_ARG_NESTED_ARG_ID)
            fmt += L"0";
        fmt += L":";
        fmt += FILLERS_W[fillChar];
        fmt += FILL_W[fillType];
        if (width >= 0) {
            if (argType == k_ARG_NESTED_ARG_ID)
                fmt += L"{1}";
            else if (argType == k_ARG_NESTED_NON_ID)
                fmt += L"{}";
            else
                fmt += bsl::to_wstring(width);
        }
        if (precision >= 0) {
            fmt += L".";
            if (argType == k_ARG_NESTED_ARG_ID)
                fmt += L"{2}";
            else if (argType == k_ARG_NESTED_NON_ID)
                fmt += L"{}";
            else
                fmt += bsl::to_wstring(precision);
        }

        fmt += L"s}";

        *formatString = fmt;
        *inputString  = contentString;
        *outputString = result;
    }
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test        = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose     = argc > 2;
    const bool veryVerbose = argc > 3;
    (void) veryVerbose;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 13: {
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

///Example: Formatting a basic string
/// - - - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test this formatter's ability to a substring with padding
// and minimum width.
//
//..
        bslfmt::Formatter_MockParseContext<char> mpc("*<5.3s", 1);

        bsl::formatter<const char *, char> f;
        mpc.advance_to(f.parse(mpc));

        const char *value = "abcdefghij";

        bslfmt::Formatter_MockFormatContext<char> mfc(value, 0, 0);

        mfc.advance_to(bsl::as_const(f).format(value, mfc));

        ASSERT("abc**" == mfc.finalString());
//..
//
      } break;
      case 12: {
        // -----------------------------------------------
        // TESTING format(VALUE, FORMAT_CONTEXT&);
        //
        // Concerns:
        //: 1 After parsing a valid format spec, `format` will correctly format
        //:   a valid string type.
        //:
        //: 2 Valid format strings will not generate a parse error
        //
        // Plan:
        //: 1 Construct format specifications corresponding to multiple
        //:   precisions, widths and fills.
        //:
        //: 2 Construct input strings of various lenghts, containing both ascii
        //:   and unicode.
        //:
        //: 3 Verify that, for each of the specificaitons and inputs
        //:   constructed in steps 1 and 2 the result of the `format` function
        //:   matches an independently calculated result.
        //:
        //: 4 Verify that, for each of the specificaitons and inputs
        //:   constructed in steps 1 and 2 the result of the `format` function
        //:   matches the result from calling `std::format` where supported on the
        //:   platform.
        //:
        //: 5 Repeat steps 1-4 for all of the supported string types:
        //:   - char *
        //:   - const char *
        //:   - std::string_view
        //:   - bsl::string_view
        //:   - std::string
        //:   - bsl::string
        //:
        //: 6 Repeat step 5 for the wchar_t equivalent types.
        //
        // Testing:
        //   format(VALUE, FORMAT_CONTEXT&);
        // -----------------------------------------------
        if (verbose)
            printf("\nTESTING parse(PARSE_CONTEXT&);"
                   "\n==============================\n");

        for (int fc = 0; fc < k_FILLCHAR_COUNT; fc++) {
            for (int ft = 0; ft < k_FILL_COUNT; ft++) {
                for (int ct = 0; ct < k_VALUE_COUNT; ct++) {
                    for (int argType = 0; argType < k_ARG_COUNT; argType++) {
                        for (int copies = -1; copies < 10; copies++) {
                            for (int width = -1; width < 10; width++) {
                                for (int prec = -1; prec < 10; prec++) {
                                    bool        isUnicodeSupportRequired;
                                    bsl::string formatString;
                                    bsl::string inputString;
                                    bsl::string outputString;
                                    ResultCalculator<char>::calculate(
                                                     &isUnicodeSupportRequired,
                                                     &formatString,
                                                     &inputString,
                                                     &outputString,
                                                     fc,
                                                     ft,
                                                     ct,
                                                     argType,
                                                     copies,
                                                     width,
                                                     prec);

                                    bool testOracle =
                                                   !isUnicodeSupportRequired ||
                                                   k_ORACLE_SUPPORT_UNICODE;

                                    bsl::string message;
                                    bool        rv;

                                    int arg2 = width;
                                    if (argType == k_ARG_NESTED_NON_ID &&
                                        width <= 0)
                                        arg2 = prec;

                                    char *input_cp = const_cast<char *>
                                                         (inputString.c_str());
                                    rv = bslfmt::Formatter_TestUtil<char>::
                                        testEvaluateVFormat(&message,
                                                            outputString,
                                                            testOracle,
                                                            formatString,
                                                            input_cp,
                                                            arg2,
                                                            prec);
                                    ASSERTV(formatString.c_str(),
                                            message.c_str(),
                                            rv);

                                    const char *input_ccp =
                                                           inputString.c_str();
                                    rv = bslfmt::Formatter_TestUtil<char>::
                                        testEvaluateVFormat(&message,
                                                            outputString,
                                                            testOracle,
                                                            formatString,
                                                            input_ccp,
                                                            arg2,
                                                            prec);
                                    ASSERTV(formatString.c_str(),
                                            message.c_str(),
                                            rv);

                                    std::string input_ss = inputString.c_str();
                                    rv = bslfmt::Formatter_TestUtil<char>::
                                        testEvaluateVFormat(&message,
                                                            outputString,
                                                            testOracle,
                                                            formatString,
                                                            input_ss,
                                                            arg2,
                                                            prec);
                                    ASSERTV(formatString.c_str(),
                                            message.c_str(),
                                            rv);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

                                    std::string_view input_sv =
                                                           inputString.c_str();
                                    rv = bslfmt::Formatter_TestUtil<char>::
                                        testEvaluateVFormat(&message,
                                                            outputString,
                                                            testOracle,
                                                            formatString,
                                                            input_sv,
                                                            arg2,
                                                            prec);
                                    ASSERTV(formatString.c_str(),
                                            message.c_str(),
                                            rv);

#endif

                                    bsl::string input_bs = inputString.c_str();
                                    rv = bslfmt::Formatter_TestUtil<char>::
                                        testEvaluateVFormat(&message,
                                                            outputString,
                                                            testOracle,
                                                            formatString,
                                                            input_bs,
                                                            arg2,
                                                            prec);
                                    ASSERTV(formatString.c_str(),
                                            message.c_str(),
                                            rv);

                                    bsl::string_view input_bv = inputString;
                                    rv = bslfmt::Formatter_TestUtil<char>::
                                        testEvaluateVFormat(&message,
                                                            outputString,
                                                            testOracle,
                                                            formatString,
                                                            input_bv,
                                                            arg2,
                                                            prec);
                                    ASSERTV(formatString.c_str(),
                                            message.c_str(),
                                            rv);
                                }
                            }
                        }
                    }
                }
            }
        }

        for (int fc = 0; fc < k_FILLCHAR_COUNT; fc++) {
            for (int ft = 0; ft < k_FILL_COUNT; ft++) {
                for (int ct = 0; ct < k_VALUE_COUNT; ct++) {
                    for (int argType = 0; argType < k_ARG_COUNT; argType++) {
                        for (int copies = -1; copies < 10; copies++) {
                            for (int width = -1; width < 10; width++) {
                                for (int prec = -1; prec < 10; prec++) {
                                    bool        isUnicodeSupportRequired;
                                    bsl::wstring formatString;
                                    bsl::wstring inputString;
                                    bsl::wstring outputString;
                                    ResultCalculator<wchar_t>::calculate(
                                                     &isUnicodeSupportRequired,
                                                     &formatString,
                                                     &inputString,
                                                     &outputString,
                                                     fc,
                                                     ft,
                                                     ct,
                                                     argType,
                                                     copies,
                                                     width,
                                                     prec);

                                    bool testOracle =
                                                   !isUnicodeSupportRequired ||
                                                   k_ORACLE_SUPPORT_UNICODE;

                                    bsl::string message;
                                    bool        rv;

                                    int arg2 = width;
                                    if (argType == k_ARG_NESTED_NON_ID &&
                                        width <= 0)
                                        arg2 = prec;

                                    wchar_t *input_cp = const_cast<wchar_t *>(
                                                          inputString.c_str());
                                    rv = bslfmt::Formatter_TestUtil<wchar_t>::
                                        testEvaluateVFormat(&message,
                                                            outputString,
                                                            testOracle,
                                                            formatString,
                                                            input_cp,
                                                            arg2,
                                                            prec);
                                    ASSERTV(formatString.c_str(),
                                            message.c_str(),
                                            rv);

                                    const wchar_t *input_ccp =
                                                           inputString.c_str();
                                    rv = bslfmt::Formatter_TestUtil<wchar_t>::
                                        testEvaluateVFormat(&message,
                                                            outputString,
                                                            testOracle,
                                                            formatString,
                                                            input_ccp,
                                                            arg2,
                                                            prec);
                                    ASSERTV(formatString.c_str(),
                                            message.c_str(),
                                            rv);

                                    std::wstring input_ss = inputString.c_str();
                                    rv = bslfmt::Formatter_TestUtil<wchar_t>::
                                        testEvaluateVFormat(&message,
                                                            outputString,
                                                            testOracle,
                                                            formatString,
                                                            input_ss,
                                                            arg2,
                                                            prec);
                                    ASSERTV(formatString.c_str(),
                                            message.c_str(),
                                            rv);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

                                    std::wstring_view input_sv =
                                                           inputString.c_str();
                                    rv = bslfmt::Formatter_TestUtil<wchar_t>::
                                        testEvaluateVFormat(&message,
                                                            outputString,
                                                            testOracle,
                                                            formatString,
                                                            input_sv,
                                                            arg2,
                                                            prec);
                                    ASSERTV(formatString.c_str(),
                                            message.c_str(),
                                            rv);

#endif

                                    bsl::wstring input_bs = inputString.c_str();
                                    rv = bslfmt::Formatter_TestUtil<wchar_t>::
                                        testEvaluateVFormat(&message,
                                                            outputString,
                                                            testOracle,
                                                            formatString,
                                                            input_bs,
                                                            arg2,
                                                            prec);
                                    ASSERTV(formatString.c_str(),
                                            message.c_str(),
                                            rv);

                                    bsl::wstring_view input_bv = inputString;
                                    rv = bslfmt::Formatter_TestUtil<wchar_t>::
                                        testEvaluateVFormat(&message,
                                                            outputString,
                                                            testOracle,
                                                            formatString,
                                                            input_bv,
                                                            arg2,
                                                            prec);
                                    ASSERTV(formatString.c_str(),
                                            message.c_str(),
                                            rv);
                                }
                            }
                        }
                    }
                }
            }
        }


      } break;
      case 11: {
        // -----------------------------------------------
        // TESTING parse(PARSE_CONTEXT&);
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
        //   parse(PARSE_CONTEXT&);
        // -----------------------------------------------
        if (verbose)
            printf("\nTESTING parse(PARSE_CONTEXT&);"
                   "\n==============================\n");

        // Bad fill character
        // Note can only test '{' as '}' closes the parse string.
        TEST_PARSE_FAIL(char,     "{:{<5.5s}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:{<5.5s}",    true);

        // Missing fill specifier
        TEST_PARSE_FAIL(char,     "{:*5.5s}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{:*5.5s}",     true);

        // Sign
        TEST_PARSE_FAIL(char,     "{:*< 5.5s}",   true);
        TEST_PARSE_FAIL(char,     "{:*<+5.5s}",   true);
        TEST_PARSE_FAIL(char,     "{:*<-5.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*< 5.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<+5.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<-5.5s}",   true);

        // Alternative option
        TEST_PARSE_FAIL(char,     "{:*<#5.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<#5.5s}",   true);

        // Zero pad option
        TEST_PARSE_FAIL(char,     "{:*<05.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<05.5s}",   true);

        // Locale option
        TEST_PARSE_FAIL(char,     "{:*<5.5Ls}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<5.5Ls}",   true);

        // Escaped string type
        // Not supported in bslfmt at all or in std before C++23
        TEST_PARSE_FAIL(char,     "{:*<5.5?}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<5.5?}",    true);

        // Non-string type
        TEST_PARSE_FAIL(char,     "{:*<5.5d}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<5.5d}",    true);

        // Non-numeric width or precision
        TEST_PARSE_FAIL(char,     "{:*< X.5s}",   true);
        TEST_PARSE_FAIL(char,     "{:*<+5.Xs}",   true);
        TEST_PARSE_FAIL(char,     "{:*<-X.Xs}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*< X.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<+5.Xs}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<-X.Xs}",   true);

        // Missing precision marker
        TEST_PARSE_FAIL(char,     "{:*<{}{}s}",      false);
        TEST_PARSE_FAIL(char,     "{0:*<{3}{2}s}",   false);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{}{}s}",      false);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{3}{2}s}",   false);

        // Mixed numeric and non-numeric nested args
        TEST_PARSE_FAIL(char,     "{:*<{}.{2}s}",     true);
        TEST_PARSE_FAIL(char,     "{:*<{1}.{}s}",     true);
        TEST_PARSE_FAIL(char,     "{:*<{1}.{2}s}",    true);
        TEST_PARSE_FAIL(char,     "{0:*<{}.{}s}",     true);
        TEST_PARSE_FAIL(char,     "{0:*<{}.{2}s}",    true);
        TEST_PARSE_FAIL(char,     "{0:*<{1}.{}s}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{}.{2}s}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{1}.{}s}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{1}.{2}s}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{}.{}s}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{}.{2}s}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{1}.{}s}",    true);

        // Nested args out of range
        // Not checked in std parsing
        TEST_PARSE_FAIL(char,     "{0:*<{1}.{3}s}",   false);
        TEST_PARSE_FAIL(char,     "{0:*<{3}.{2}s}",   false);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{1}.{3}s}",   false);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{3}.{2}s}",   false);

        // A selection of valid format strings (non-unicode)
        TEST_PARSE_SUCCESS_F(char,     "{:}"                   , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:}"                   , true);
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
      case 10: {
        // -----------------------------------------------
        // TESTING STREAMING FUNCTIONALITY: Not Applicable
        //
        // Testing:
        //   STREAMING FUNCTIONALITY: Not Applicable
        // -----------------------------------------------
        if (verbose)
            printf("\nSTREAMING FUNCTIONALITY: Not Applicable"
                   "\n=======================================\n");
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //: 1 We can swap two 'bsl::formatter' types for all the string
        //:   specializations.
        //:
        //: 2 We can swap two 'std::formatter' types for those partial
        //:   specializations we expect to be aliased into the 'std' namespace.
        //
        // Plan:
        //: 1 Construct two 'bsl::formatter's for each of the partial
        //:   specializations, and assign one to the other. (C-1)
        //:
        //: 2 Construct two 'std::formatter' for each of the partial
        //:   specializations that we promote to 'std' and assign one to the
        //:   other. (C-2)
        //
        // Testing:
        //   operator=(const formatter &);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING SWAP"
                   "\n============\n");

        typedef char    C;
        typedef wchar_t W;

        if (verbose)
            printf("\nValidating bslfmt swap\n");

        {
            bsl::formatter<const C *, C> dummy_c;
            bsl::formatter<const W *, W> dummy_w;
            bsl::formatter<const C *, C> dummy2_c;
            bsl::formatter<const W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<C *, C> dummy_c;
            bsl::formatter<W *, W> dummy_w;
            bsl::formatter<C *, C> dummy2_c;
            bsl::formatter<W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<C[10], C> dummy_c;
            bsl::formatter<W[10], W> dummy_w;
            bsl::formatter<C[10], C> dummy2_c;
            bsl::formatter<W[10], W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<bsl::basic_string<C>, C> dummy_c;
            bsl::formatter<bsl::basic_string<W>, W> dummy_w;
            bsl::formatter<bsl::basic_string<C>, C> dummy2_c;
            bsl::formatter<bsl::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<std::basic_string<C>, C> dummy_c;
            bsl::formatter<std::basic_string<W>, W> dummy_w;
            bsl::formatter<std::basic_string<C>, C> dummy2_c;
            bsl::formatter<std::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<bsl::basic_string_view<C>, C> dummy_c;
            bsl::formatter<bsl::basic_string_view<W>, W> dummy_w;
            bsl::formatter<bsl::basic_string_view<C>, C> dummy2_c;
            bsl::formatter<bsl::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        {
            bsl::formatter<std::basic_string_view<C>, C> dummy_c;
            bsl::formatter<std::basic_string_view<W>, W> dummy_w;
            bsl::formatter<std::basic_string_view<C>, C> dummy2_c;
            bsl::formatter<std::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        if (verbose)
            printf("\nValidating std swap\n");

        {
            std::formatter<const C *, C> dummy_c;
            std::formatter<const W *, W> dummy_w;
            std::formatter<const C *, C> dummy2_c;
            std::formatter<const W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<C *, C> dummy_c;
            std::formatter<W *, W> dummy_w;
            std::formatter<C *, C> dummy2_c;
            std::formatter<W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<C[10], C> dummy_c;
            std::formatter<W[10], W> dummy_w;
            std::formatter<C[10], C> dummy2_c;
            std::formatter<W[10], W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<bsl::basic_string<C>, C> dummy_c;
            std::formatter<bsl::basic_string<W>, W> dummy_w;
            std::formatter<bsl::basic_string<C>, C> dummy2_c;
            std::formatter<bsl::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<std::basic_string<C>, C> dummy_c;
            std::formatter<std::basic_string<W>, W> dummy_w;
            std::formatter<std::basic_string<C>, C> dummy2_c;
            std::formatter<std::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<bsl::basic_string_view<C>, C> dummy_c;
            std::formatter<bsl::basic_string_view<W>, W> dummy_w;
            std::formatter<bsl::basic_string_view<C>, C> dummy2_c;
            std::formatter<bsl::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<std::basic_string_view<C>, C> dummy_c;
            std::formatter<std::basic_string_view<W>, W> dummy_w;
            std::formatter<std::basic_string_view<C>, C> dummy2_c;
            std::formatter<std::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SWAP
        //
        // Concerns:
        //: 1 We can swap two 'bsl::formatter' types for all the string
        //:   specializations.
        //:
        //: 2 We can swap two 'std::formatter' types for those partial
        //:   specializations we expect to be aliased into the 'std' namespace.
        //
        // Plan:
        //: 1 Construct two 'bsl::formatter's for each of the partial
        //:   specializations, and swap them. (C-1)
        //:
        //: 2 Construct two 'std::formatter' for each of the partial
        //:   specializations that we promote to 'std' and swap them. (C-2)
        //
        // Testing:
        //   formatter(const formatter &);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING SWAP"
                   "\n============\n");

        typedef char    C;
        typedef wchar_t W;

        if (verbose)
            printf("\nValidating bslfmt swap\n");

        {
            bsl::formatter<const C *, C> dummy_c;
            bsl::formatter<const W *, W> dummy_w;
            bsl::formatter<const C *, C> dummy2_c;
            bsl::formatter<const W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<C *, C> dummy_c;
            bsl::formatter<W *, W> dummy_w;
            bsl::formatter<C *, C> dummy2_c;
            bsl::formatter<W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<C[10], C> dummy_c;
            bsl::formatter<W[10], W> dummy_w;
            bsl::formatter<C[10], C> dummy2_c;
            bsl::formatter<W[10], W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<bsl::basic_string<C>, C> dummy_c;
            bsl::formatter<bsl::basic_string<W>, W> dummy_w;
            bsl::formatter<bsl::basic_string<C>, C> dummy2_c;
            bsl::formatter<bsl::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<std::basic_string<C>, C> dummy_c;
            bsl::formatter<std::basic_string<W>, W> dummy_w;
            bsl::formatter<std::basic_string<C>, C> dummy2_c;
            bsl::formatter<std::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            bsl::formatter<bsl::basic_string_view<C>, C> dummy_c;
            bsl::formatter<bsl::basic_string_view<W>, W> dummy_w;
            bsl::formatter<bsl::basic_string_view<C>, C> dummy2_c;
            bsl::formatter<bsl::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        {
            bsl::formatter<std::basic_string_view<C>, C> dummy_c;
            bsl::formatter<std::basic_string_view<W>, W> dummy_w;
            bsl::formatter<std::basic_string_view<C>, C> dummy2_c;
            bsl::formatter<std::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        if (verbose)
            printf("\nValidating std swap\n");

        {
            std::formatter<const C *, C> dummy_c;
            std::formatter<const W *, W> dummy_w;
            std::formatter<const C *, C> dummy2_c;
            std::formatter<const W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<C *, C> dummy_c;
            std::formatter<W *, W> dummy_w;
            std::formatter<C *, C> dummy2_c;
            std::formatter<W *, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<C[10], C> dummy_c;
            std::formatter<W[10], W> dummy_w;
            std::formatter<C[10], C> dummy2_c;
            std::formatter<W[10], W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<bsl::basic_string<C>, C> dummy_c;
            std::formatter<bsl::basic_string<W>, W> dummy_w;
            std::formatter<bsl::basic_string<C>, C> dummy2_c;
            std::formatter<bsl::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<std::basic_string<C>, C> dummy_c;
            std::formatter<std::basic_string<W>, W> dummy_w;
            std::formatter<std::basic_string<C>, C> dummy2_c;
            std::formatter<std::basic_string<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<bsl::basic_string_view<C>, C> dummy_c;
            std::formatter<bsl::basic_string_view<W>, W> dummy_w;
            std::formatter<bsl::basic_string_view<C>, C> dummy2_c;
            std::formatter<bsl::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
        {
            std::formatter<std::basic_string_view<C>, C> dummy_c;
            std::formatter<std::basic_string_view<W>, W> dummy_w;
            std::formatter<std::basic_string_view<C>, C> dummy2_c;
            std::formatter<std::basic_string_view<W>, W> dummy2_w;
            bsl::swap(dummy_c, dummy2_c);
            bsl::swap(dummy_w, dummy2_w);
        }
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 We can copy construct 'bsl::formatter' types for all the string
        //:   specializations.
        //:
        //: 2 We can copy construct 'std::formatter' types for those partial
        //:   specializations we expect to be aliased into the 'std' namespace.
        //
        // Plan:
        //: 1 Construct a 'bsl::formatter' for each of the partial
        //:   specializations, and copy it. (C-1)
        //:
        //: 2 Construct a 'std::formatter' for each of the partial
        //:   specializations that we promote to 'std' and copy it. (C-2)
        //
        // Testing:
        //   formatter(const formatter &);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING COPY CONSTRUCTOR"
                   "\n========================\n");

        typedef char    C;
        typedef wchar_t W;

        if (verbose)
            printf("\nValidating bslfmt copy construction\n");

        {
            bsl::formatter<const C *, C> dummy_c;
            bsl::formatter<const W *, W> dummy_w;
            bsl::formatter<const C *, C> copy_c(dummy_c);
            bsl::formatter<const W *, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            bsl::formatter<C *, C> dummy_c;
            bsl::formatter<W *, W> dummy_w;
            bsl::formatter<C *, C> copy_c(dummy_c);
            bsl::formatter<W *, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            bsl::formatter<C[10], C> dummy_c;
            bsl::formatter<W[10], W> dummy_w;
            bsl::formatter<C[10], C> copy_c(dummy_c);
            bsl::formatter<W[10], W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            bsl::formatter<bsl::basic_string<C>, C> dummy_c;
            bsl::formatter<bsl::basic_string<W>, W> dummy_w;
            bsl::formatter<bsl::basic_string<C>, C> copy_c(dummy_c);
            bsl::formatter<bsl::basic_string<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            bsl::formatter<std::basic_string<C>, C> dummy_c;
            bsl::formatter<std::basic_string<W>, W> dummy_w;
            bsl::formatter<std::basic_string<C>, C> copy_c(dummy_c);
            bsl::formatter<std::basic_string<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            bsl::formatter<bsl::basic_string_view<C>, C> dummy_c;
            bsl::formatter<bsl::basic_string_view<W>, W> dummy_w;
            bsl::formatter<bsl::basic_string_view<C>, C> copy_c(dummy_c);
            bsl::formatter<bsl::basic_string_view<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        {
            bsl::formatter<std::basic_string_view<C>, C> dummy_c;
            bsl::formatter<std::basic_string_view<W>, W> dummy_w;
            bsl::formatter<std::basic_string_view<C>, C> copy_c(dummy_c);
            bsl::formatter<std::basic_string_view<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        if (verbose)
            printf("\nValidating std copy construction\n");

        {
            std::formatter<const C *, C> dummy_c;
            std::formatter<const W *, W> dummy_w;
            std::formatter<const C *, C> copy_c(dummy_c);
            std::formatter<const W *, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            std::formatter<C *, C> dummy_c;
            std::formatter<W *, W> dummy_w;
            std::formatter<C *, C> copy_c(dummy_c);
            std::formatter<W *, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            std::formatter<C[10], C> dummy_c;
            std::formatter<W[10], W> dummy_w;
            std::formatter<C[10], C> copy_c(dummy_c);
            std::formatter<W[10], W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            std::formatter<bsl::basic_string<C>, C> dummy_c;
            std::formatter<bsl::basic_string<W>, W> dummy_w;
            std::formatter<bsl::basic_string<C>, C> copy_c(dummy_c);
            std::formatter<bsl::basic_string<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            std::formatter<std::basic_string<C>, C> dummy_c;
            std::formatter<std::basic_string<W>, W> dummy_w;
            std::formatter<std::basic_string<C>, C> copy_c(dummy_c);
            std::formatter<std::basic_string<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            std::formatter<bsl::basic_string_view<C>, C> dummy_c;
            std::formatter<bsl::basic_string_view<W>, W> dummy_w;
            std::formatter<bsl::basic_string_view<C>, C> copy_c(dummy_c);
            std::formatter<bsl::basic_string_view<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
        {
            std::formatter<std::basic_string_view<C>, C> dummy_c;
            std::formatter<std::basic_string_view<W>, W> dummy_w;
            std::formatter<std::basic_string_view<C>, C> copy_c(dummy_c);
            std::formatter<std::basic_string_view<W>, W> copy_w(dummy_w);
            (void)copy_c;
            (void)copy_w;
        }
#endif
      } break;
      case 6: {
        // --------------------------------------------
        // TESTING EQUALITY OPERATOR: Not Applicable
        //
        // Testing:
        //   EQUALITY OPERATOR: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nEQUALITY OPERATOR: Not Applicable"
                   "\n= ===============================\n");
      } break;
      case 5: {
        // --------------------------------------------
        // TESTING OUTPUT: Not Applicable
        //
        // Testing:
        //   OUTPUT: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nOUTPUT: Not Applicable"
                   "\n======================\n");
      } break;
      case 4: {
        // --------------------------------------------
        // TESTING BASIC ACCESSORS: Not Applicable
        //
        // Testing:
        //   BASIC ACCESSORS: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nBASIC ACCESSORS: Not Applicable"
                   "\n===============================\n");
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING (PRIMITIVE) GENERATORS
        //   The only generators for 'formatter' is the default constructor.
        //
        // Concerns:
        //: 1 We can construct 'bsl::formatter' types for all the string
        //:   specializations.
        //:
        //: 2 We can construct 'std::formatter' types for those partial
        //:   specializations we expect to be aliased into the 'std' namespace.
        //
        // Plan:
        //: 1 Construct a 'bsl::formatter' for each of the partial
        //:   specializations. (C-1)
        //:
        //: 2 Construct a 'std::formatter' for each of the partial
        //:   specializations that we promote to 'std'. (C-2)
        //
        // Testing:
        //   formatter();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING (PRIMITIVE) GENERATORS"
                            "\n==============================\n");

        if (verbose)
            printf("\nValidating bslfmt construction\n");

        {
            bsl::formatter<const char *,    char>    dummy_c;
            bsl::formatter<const wchar_t *, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            bsl::formatter<char *,    char>    dummy_c;
            bsl::formatter<wchar_t *, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            bsl::formatter<char[10],    char>    dummy_c;
            bsl::formatter<wchar_t[10], wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            bsl::formatter<bsl::basic_string<char>,    char>    dummy_c;
            bsl::formatter<bsl::basic_string<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            bsl::formatter<std::basic_string<char>, char>       dummy_c;
            bsl::formatter<std::basic_string<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            bsl::formatter<bsl::basic_string_view<char>, char>       dummy_c;
            bsl::formatter<bsl::basic_string_view<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        {
            bsl::formatter<std::basic_string_view<char>, char>       dummy_c;
            bsl::formatter<std::basic_string_view<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        if (verbose)
            printf("\nValidating std construction\n");

        {
            std::formatter<const char *, char>       dummy_c;
            std::formatter<const wchar_t *, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            std::formatter<char *, char>       dummy_c;
            std::formatter<wchar_t *, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            std::formatter<char[10], char>       dummy_c;
            std::formatter<wchar_t[10], wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            std::formatter<bsl::basic_string<char>, char>       dummy_c;
            std::formatter<bsl::basic_string<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            std::formatter<std::basic_string<char>, char>       dummy_c;
            std::formatter<std::basic_string<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            std::formatter<bsl::basic_string_view<char>, char>       dummy_c;
            std::formatter<bsl::basic_string_view<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
        {
            std::formatter<std::basic_string_view<char>, char>       dummy_c;
            std::formatter<std::basic_string_view<wchar_t>, wchar_t> dummy_w;
            (void)dummy_c;
            (void)dummy_w;
        }
#endif

      } break;
      case 2: {
        // --------------------------------------------
        // TESTING PRIMARY MANIPULATORS: Not Applicable
        // 
        // Testing:
        //   PRIMARY MANIPULATORS: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nPRIMARY MANIPULATORS: Not Applicable"
                   "\n====================================\n");
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That basic functionality appears to work as advertised before
        //:   before beginning testing in earnest:
        //:   - default and copy constructors
        //:   - assignment operator
        //:   - primary manipulators, basic accessors
        //:   - 'operator==', 'operator!='
        //
        // Plan:
        //: 1 Test all public methods mentioned in concerns.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        bsl::formatter<const char *, char>                  dummy1;
        bsl::formatter<bsl::string_view, char>     dummy2;
        bsl::formatter<bsl::wstring_view, wchar_t> dummy3;
        (void)dummy1;
        (void)dummy2;
        (void)dummy3;
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        std::formatter<bsl::string, char>          dummy4;
        std::formatter<bsl::wstring, wchar_t>      dummy5;
        (void)dummy4;
        (void)dummy5;
#endif
        ASSERT(true);  // placeholder

        ASSERT((bslfmt::format("String={:*^10.5}.",
                               "abcdefg") == "String=**abcde***."));
        ASSERT((bslfmt::format("String={:*^10.5}.", std::string("abcdefg")) ==
                "String=**abcde***."));
        ASSERT((bslfmt::format("String={:*^10.5}.", bsl::string("abcdefg")) ==
                "String=**abcde***."));

        ASSERT((bslfmt::format("String={2:*^{0}.{1}}.",
                               10, 5, "abcdefg") == "String=**abcde***."));

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        std::string val = std::format("String={:*^10.5}.",
                                      bsl::string("abcdefg"));
#endif

        bsl::string message;

        const wchar_t *winput = L"abcdefg";

        bool rv;

        rv = bslfmt::Formatter_TestUtil<char>::testParseFormat<const char *>(
                                                               &message,
                                                               true,
                                                               "{0:*^{1}.{2}}");

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::Formatter_TestUtil<wchar_t>::testParseFormat<const wchar_t *>(
                                                              &message,
                                                              true,
                                                              L"{0:*^{1}.{2}}");

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::Formatter_TestUtil<char>::testParseVFormat<char *>(
                                                               &message,
                                                               true,
                                                               "{0:*^{1}.{2}}");

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::Formatter_TestUtil<wchar_t>::testParseVFormat<wchar_t *>(
                                                              &message,
                                                              true,
                                                              L"{0:*^{1}.{2}}");

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::Formatter_TestUtil<char>::testEvaluateFormat(&message,
                                                            "**abcde***",
                                                            true,
                                                            "{0:*^{1}.{2}}",
                                                            "abcdefghi",
                                                            10,
                                                            5);

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::Formatter_TestUtil<wchar_t>::testEvaluateFormat(
                                                              &message,
                                                              L"**abcde***",
                                                              true,
                                                              L"{0:*^{1}.{2}}",
                                                              winput,
                                                              10,
                                                              5);

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::Formatter_TestUtil<char>::testEvaluateVFormat(
                                                               &message,
                                                               "**abcde***",
                                                               true,
                                                               "{0:*^{1}.{2}}",
                                                               "abcdefghi",
                                                               10,
                                                               5);

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::Formatter_TestUtil<wchar_t>::testEvaluateVFormat(
                                                              &message,
                                                              L"**abcde***",
                                                              true,
                                                              L"{0:*^{1}.{2}}",
                                                              winput,
                                                              10,
                                                              5);

        ASSERTV(message.c_str(), rv);

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
