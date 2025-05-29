// bslstl_streamed.t.cpp                                              -*-C++-*-
#define BB_DEPRECATE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING                      1

#include <bslfmt_streamed.h>

#include <bslfmt_format.h>
#include <bslfmt_formattertestutil.h>

#include <bsls_bsltestutil.h>

#include <bslstl_algorithm.h>
#include <bslstl_iterator.h>
#include <bslstl_ostringstream.h>
#include <bslstl_string.h>

#include <limits.h>
#include <stdio.h>

#include <iostream>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #include <format>
  #include <string>
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Tested are the wrapper class template `bslfmt::Streamed<t_STREAMED>` as well
// as the wrapper-creator function template (mainly for C++03 compatibility)
// `bslfmt::streamed`.
//-----------------------------------------------------------------------------
// TEST MACHINERY
// [ 2] VariableLengthStreamable
//
// CREATORS
// [ 3] formatter();
// [ 3] ~formatter();
// [ 4] formatter(const formatter &);
// [ 8] Streamed(const T&);
//
// MANIPULATORS
// [ 6] operator=(const formatter &);
// [ 7] formatter<Streamed<T> >::parse(PARSE_CONTEXT&);
//
// ACCESSORS
// [ 8] formatter<Streamed<T> >::format(TYPE, FORMAT_CONTEXT&);
//
// FREE FUNCTIONS
// [ 5] swap(formatter &, formatter&);
// [ 8] streamed(const T&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE
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
//                  GLOBAL HELPER MACROS FOR TESTING
//-----------------------------------------------------------------------------

#define TEST_PARSE_FAIL(type, fmtStr)                                         \
    do {                                                                      \
        bsl::string errorMsg;                                                 \
        bool        rv;                                                       \
        rv = bslfmt::FormatterTestUtil<char>::testParseFailure<               \
            bslfmt::Streamed<type> >(&errorMsg, false, fmtStr);               \
        if (!rv) {                                                            \
            ASSERTV(errorMsg.c_str(), fmtStr, rv);                            \
        }                                                                     \
    } while (false)

#define TEST_PARSE_SUCCESS_F(type, fmtStr)                                    \
    do {                                                                      \
        bsl::string errorMsg;                                                 \
        bool        rv;                                                       \
                                                                              \
        rv = bslfmt::FormatterTestUtil<char>::testParseFormat<                \
            bslfmt::Streamed<type> >(&errorMsg, false, fmtStr);               \
        if (!rv) {                                                            \
            ASSERTV(errorMsg.c_str(), fmtStr, rv);                            \
        }                                                                     \
    } while (false)

#define TEST_PARSE_SUCCESS_VF(type, fmtStr)                                   \
    do {                                                                      \
        bsl::string errorMsg;                                                 \
        bool        rv;                                                       \
                                                                              \
        rv = bslfmt::FormatterTestUtil<char>::testParseVFormat<               \
            bslfmt::Streamed<type> >(&errorMsg, false, fmtStr);               \
        if (!rv) {                                                            \
            ASSERTV(errorMsg.c_str(), fmtStr, rv);                            \
        }                                                                     \
    } while (false)

//=============================================================================
//                               TEST TYPES
//-----------------------------------------------------------------------------

struct CanStream {
    const char *d_content;

    // CREATORS
    explicit CanStream(const char *content)
    : d_content(content)
    {
    }

    // MANIPULATORS
    void setContent(const char *content) { d_content = content; }

    // ACCESSORS
    const char *content() const { return d_content; }
};

std::ostream& operator<<(std::ostream& os, const CanStream& obj)
{
    return os << obj.d_content;
}

                      // ================================
                      // OutputIteratorStreamBufferTester
                      // ================================

/// This type can print to stream up to 62 characters, and it verifies the
/// state of the stream after each print.  The 62 characters printed go from
/// 0-9, then a-z, then A-Z.
class VariableLengthStreamable {
    // DATA
    int d_numCharsPrinted;

    // PRIVATE CLASS METHODS
    static char nthChar(int n)
    {
        if (n < 10) {
            return static_cast<char>('0' + n);
        }
        else if (n < 36) {
            return static_cast<char>('a' + (n - 10));
        }
        else {
            return static_cast<char>('A' + (n - 36));
        }
    }

  public:
    // CREATORS
    VariableLengthStreamable(int numChars)
    : d_numCharsPrinted(numChars)
    {
        ASSERT(numChars >= 0);
        ASSERT(numChars <= 62);
    }

    // MANIPULATORS
    void setNumChars(int numChars)
    {
        ASSERT(numChars >= 0);
        ASSERT(numChars <= 62);

        d_numCharsPrinted = numChars;
    }

    // ACCESSORS
    int numChars() const { return d_numCharsPrinted; }

    std::ostream& streamInsert(std::ostream& os) const
    {
        for (int n = 0; n < d_numCharsPrinted; ++n) {
            os << nthChar(n);
        }

        return os;
    }

    bsl::string asString() const
    {
        bsl::string rv;
        rv.reserve(d_numCharsPrinted);

        for (int n = 0; n < d_numCharsPrinted; ++n) {
            rv += nthChar(n);
        }

        return rv;
    }
};

std::ostream& operator<<(std::ostream&                   os,
                         const VariableLengthStreamable& obj)
{
    return obj.streamInsert(os);
}

// ============================================================================
//                     GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

static const int   k_FILLCHAR_EMPTY = 0;
static const char *FILLERS_C[]      = {"", "*", "-", "+", "."};
static const int   k_FILLCHAR_COUNT = sizeof FILLERS_C / sizeof *FILLERS_C;

static const int k_FILL_NONE   = 0;
static const int k_FILL_LEFT   = 1;
static const int k_FILL_RIGHT  = 2;
static const int k_FILL_MIDDLE = 3;
static const int k_FILL_COUNT  = 4;

static const char *FILL_C[] = {"", ">", "<", "^"};

static const int k_MAX_CHAR_COUNT = 20;

BSLA_MAYBE_UNUSED static const int k_ARG_VALUE         = 0;
static const int                   k_ARG_NESTED_NON_ID = 1;
static const int                   k_ARG_NESTED_ARG_ID = 2;
static const int                   k_ARG_COUNT         = 3;

// ============================================================================
//                     GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

/// Fill the specified `formatString` and the expected `outputString` based on
/// the specified `input`, `fillChar`, `fillType`, `argType`, `width`, and
/// `precision`.  A `width` of zero results in no width specification in the
/// output `formatString`.  A `fillType` of `k_FILL_NONE` results in no fill
/// character specified in the output `formatString`.  A negative `precision`
/// value results in no precision specified in the output `formatString`.
void calculateResult(bsl::string                     *formatString,
                     bsl::string                     *outputString,
                     const VariableLengthStreamable&  input,
                     int                              fillChar,
                     int                              fillType,
                     int                              argType,
                     int                              width,
                     int                              precision)
{
    if (width == 0) {
        width = -1;
    }
    if (fillType == k_FILL_NONE) {
        fillChar = k_FILLCHAR_EMPTY;
    }

    std::string fmt;
    std::string result;

    const int contentWidth = precision < 0
                                 ? input.numChars()
                                 : bsl::min(input.numChars(), precision);

    int padding = 0;
    if (width > 0) {
        padding = bsl::max(0, width - contentWidth);
    }

    int leftPad  = 0;
    int rightPad = 0;

    if (fillType == k_FILL_LEFT) {
        leftPad = padding;
    }
    if (fillType == k_FILL_MIDDLE) {
        leftPad = padding / 2;
    }

    if (fillType == k_FILL_RIGHT || fillType == k_FILL_NONE) {
        rightPad = padding;
    }
    if (fillType == k_FILL_MIDDLE) {
        rightPad = (padding + 1) / 2;
    }

    for (int i = 0; i < leftPad; i++) {
        const char *filler = FILLERS_C[fillChar];
        if (fillChar == k_FILLCHAR_EMPTY) {
            filler = " ";
        }
        result += filler;
    }

    result.append(input.asString(), 0, contentWidth);

    for (int i = 0; i < rightPad; i++) {
        const char *filler = FILLERS_C[fillChar];
        if (fillChar == k_FILLCHAR_EMPTY) {
            filler = " ";
        }
        result += filler;
    }

    fmt = "{";
    if (argType == k_ARG_NESTED_ARG_ID) {
        fmt += "0";
    }
    fmt += ":";
    fmt += FILLERS_C[fillChar];
    fmt += FILL_C[fillType];
    if (width >= 0) {
        if (argType == k_ARG_NESTED_ARG_ID) {
            fmt += "{1}";
        }
        else if (argType == k_ARG_NESTED_NON_ID) {
            fmt += "{}";
        }
        else {
            fmt += bsl::to_string(width);
        }
    }
    if (precision >= 0) {
        fmt += ".";
        if (argType == k_ARG_NESTED_ARG_ID) {
            fmt += "{2}";
        }
        else if (argType == k_ARG_NESTED_NON_ID) {
            fmt += "{}";
        }
        else {
            fmt += bsl::to_string(precision);
        }
    }

    fmt += "s}";

    *formatString = fmt;
    *outputString = result;
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 1: Formatting a Streamable Object Using the Function
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<`.  When writing portable code that
// should work on compilers that do not support class template argument
// deduction we would use the wrapper-creator function `bslfmt::streamed` to
// avoid having to know and write the type of the object.  When writing code
// that is aimed at modern compilers support CTAD (class template argument
// deduction) the wrapper (`bslfmt::Streamed`) can also be used directly,
// without specifying the type formatted.
//
// First, for the sake of demonstration we create a type with an obscure and
// long name that we neither want to remember nor ever to write down, and which
// can be streamed out:
//```
    class ThisTypeHasLongAndObscureNameButStreamable {
    };

    std::ostream& operator<<(
                         std::ostream&                                     os,
                         const ThisTypeHasLongAndObscureNameButStreamable& )
    {
        return os << "The printout";
    }
//```

///Example 3: Format String Options
/// - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<` for an environment that requires the
// output to be padded, aligned, and/or truncation of the output.  In this
// example we will introduce the effects of the various possible format string
// specifications.
//
// First, for the sake of demonstration we create a type that prints a series
// of digits to help demonstrate the effects of the various formattings:
//```
    class Streamable {
    };

    std::ostream& operator<<(std::ostream& os, const Streamable&)
    {
        return os << "12345678";
    }
//```

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int  test        = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose     = argc > 2;
    bool veryVerbose = argc > 3;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        // 1. Demonstrate the functioning of this component.
        //
        // Plan:
        // 1. Use the Usage Examples from the header by replacing `assert` with
        //    `ASSERT` and moving definitions of types and functions outside of
        //    `main`.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

        {
///Example 1: Formatting a Streamable Object Using the Function
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<`.  When writing portable code that
// should work on compilers that do not support class template argument
// deduction we would use the wrapper-creator function `bslfmt::streamed` to
// avoid having to know and write the type of the object.  When writing code
// that is aimed at modern compilers support CTAD (class template argument
// deduction) the wrapper (`bslfmt::Streamed`) can also be used directly,
// without specifying the type formatted.
//
// First, for the sake of demonstration we create a type with an obscure and
// long name that we neither want to remember nor ever to write down, and which
// can be streamed out:
//```
//  class ThisTypeHasLongAndObscureNameButStreamable {
//  };
//
//  std::ostream& operator<<(
//                       std::ostream&                                     os,
//                       const ThisTypeHasLongAndObscureNameButStreamable& )
//  {
//      return os << "The printout";
//  }
//```
// Then, we create an object of said type that we want to print out:
//```
    const ThisTypeHasLongAndObscureNameButStreamable obj;
//```
// Next, we format the "value" using `bsl::format` with the wrapper-creator
// function:
//```
    bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
//```
// Finally, we verify the output is correct:
//```
    ASSERT(s == "The printout");
//```
        }

        {
///Example 2: Format String Options
/// - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<` for an environment that requires the
// output to be padded, aligned, and/or truncation of the output.  In this
// example we will introduce the effects of the various possible format string
// specifications.
//
// First, for the sake of demonstration we create a type that prints a series
// of digits to help demonstrate the effects of the various formattings:
//```
//  class Streamable {
//  };
//
//  std::ostream& operator<<(std::ostream& os, const Streamable&)
//  {
//      return os << "12345678";
//  }
//```
// Then, we create an object of said type that we will format:
//```
    const Streamable obj;
//```
// Next, we format the "value" using many different format strings, starting
// with the default for completeness:
//```
    bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
    ASSERT(s == "12345678");
//```
// Then, we format with specifying just a width:
//```
    s = bsl::format("{:10}", bslfmt::streamed(obj));
    ASSERT(s == "12345678  ");
//```
//
// Next, we format with specifying a width, and alignments:
//```
    s = bsl::format("{:<10}", bslfmt::streamed(obj));
    ASSERT(s == "12345678  ");

    s = bsl::format("{:^10}", bslfmt::streamed(obj));
    ASSERT(s == " 12345678 ");

    s = bsl::format("{:>10}", bslfmt::streamed(obj));
    ASSERT(s == "  12345678");
//```
// Finally, we demonstrate the truncation using a "precision" value:
//```
    s = bsl::format("{:.6}", bslfmt::streamed(obj));
    ASSERT(s == "123456");

    s = bsl::format("{:8.6}", bslfmt::streamed(obj));
    ASSERT(s == "123456  ");

    s = bsl::format("{:<8.6}", bslfmt::streamed(obj));
    ASSERT(s == "123456  ");

    s = bsl::format("{:^8.6}", bslfmt::streamed(obj));
    ASSERT(s == " 123456 ");

    s = bsl::format("{:>8.6}", bslfmt::streamed(obj));
    ASSERT(s == "  123456");
//```
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING format(VALUE, FORMAT_CONTEXT&)
        //
        // Concerns:
        // 1. After parsing a valid format spec, `format` will correctly format
        //    a stream output.
        //
        // 2. Valid format strings will not generate a parse error.
        //
        // Plan:
        // 1. Construct format specifications corresponding to multiple
        //    precisions, widths and fills.
        //
        // 2. Construct streamable types that print various lengths.
        //
        // 3. Verify that, for each of the specifications and inputs
        //    constructed in steps 1 and 2 the result of the `format` function
        //    matches an independently calculated result.
        //
        // Testing:
        //   formatter<Streamed<T> >::format(VALUE, FORMAT_CONTEXT&);
        // --------------------------------------------------------------------
        if (verbose) puts("\nTESTING format(VALUE, FORMAT_CONTEXT&)"
                          "\n======================================");

        for (int fc = 0; fc < k_FILLCHAR_COUNT; fc++) {
            for (int ft = 0; ft < k_FILL_COUNT; ft++) {
                for (int ct = 0; ct < k_MAX_CHAR_COUNT; ct++) {
                    for (int argType = 0; argType < k_ARG_COUNT; argType++) {
                        for (int width = -1; width < 10; width++) {
                            for (int prec = -1; prec < 10; prec++) {
                                bsl::string              formatString;
                                VariableLengthStreamable input(ct);
                                bsl::string              outputString;

                                calculateResult(&formatString,
                                                &outputString,
                                                input,
                                                fc,
                                                ft,
                                                argType,
                                                width,
                                                prec);

                                bsl::string message;
                                bool        rv;

                                int arg2 = width;
                                if (argType == k_ARG_NESTED_NON_ID && width <=
                                                                          0) {
                                    arg2 = prec;
                                }

                                rv = bslfmt::FormatterTestUtil<char>::
                                    testEvaluateVFormat(
                                                       &message,
                                                       outputString,
                                                       false,
                                                       formatString,
                                                       bslfmt::streamed(input),
                                                       arg2,
                                                       prec);
                                ASSERTV(formatString.c_str(),
                                        message.c_str(),
                                        ct,
                                        width,
                                        prec,
                                        rv);
                            }
                        }
                    }
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING parse(PARSE_CONTEXT&)
        //
        // Concerns:
        // 1. Invalid format specs will generate a parse error
        // 2. Valid format specs will not generate a parse error
        //
        // Plan:
        // 1. Construct format specs corresponding to each of the known error
        //    conditions and verify that they result in a parse error.  (C-1)
        //
        // 2. Construct format specs containing different combinations of
        //    valid specification components and verify that they correctly
        //    parse.  (C-2)
        //
        // Testing:
        //   formatter<Streamed<T> >::parse(PARSE_CONTEXT&);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING parse(PARSE_CONTEXT&)"
                          "\n=============================");

        // Bad fill character
        // Note can only test '{' as '}' closes the parse string.
        TEST_PARSE_FAIL(Streamable, "{:{<5.5s}");

        // Missing fill specifier
        TEST_PARSE_FAIL(Streamable, "{:*5.5s}");

        // Sign
        TEST_PARSE_FAIL(Streamable, "{:*< 5.5s}");
        TEST_PARSE_FAIL(Streamable, "{:*<+5.5s}");
        TEST_PARSE_FAIL(Streamable, "{:*<-5.5s}");

        // Alternative option
        TEST_PARSE_FAIL(Streamable, "{:*<#5.5s}");

        // Zero pad option
        TEST_PARSE_FAIL(Streamable, "{:*<05.5s}");

        // Locale option
        TEST_PARSE_FAIL(Streamable, "{:*<5.5Ls}");

        // Escaped string type
        // Not supported in bslfmt at all or in std before C++23
        TEST_PARSE_FAIL(Streamable, "{:*<5.5?}");

        // Non-string type
        TEST_PARSE_FAIL(Streamable, "{:*<5.5d}");

        // Non-numeric width or precision
        TEST_PARSE_FAIL(Streamable, "{:*< X.5s}");
        TEST_PARSE_FAIL(Streamable, "{:*<+5.Xs}");
        TEST_PARSE_FAIL(Streamable, "{:*<-X.Xs}");

        // Missing precision marker
        TEST_PARSE_FAIL(Streamable, "{:*<{}{}s}"   );
        TEST_PARSE_FAIL(Streamable, "{0:*<{3}{2}s}");

        // Mixed numeric and non-numeric nested args
        TEST_PARSE_FAIL(Streamable, "{:*<{}.{2}s}" );
        TEST_PARSE_FAIL(Streamable, "{:*<{1}.{}s}" );
        TEST_PARSE_FAIL(Streamable, "{:*<{1}.{2}s}");
        TEST_PARSE_FAIL(Streamable, "{0:*<{}.{}s}" );
        TEST_PARSE_FAIL(Streamable, "{0:*<{}.{2}s}");
        TEST_PARSE_FAIL(Streamable, "{0:*<{1}.{}s}");

        // Nested args out of range
        // Not checked in std parsing
        TEST_PARSE_FAIL(Streamable, "{0:*<{1}.{3}s}");
        TEST_PARSE_FAIL(Streamable, "{0:*<{3}.{2}s}");

        // A selection of valid format strings (non-unicode)
        TEST_PARSE_SUCCESS_F(Streamable, "{:}"                   );
        TEST_PARSE_SUCCESS_F(Streamable, "{:.0}"                 );
        TEST_PARSE_SUCCESS_F(Streamable, "{:.8}"                 );
        TEST_PARSE_SUCCESS_F(Streamable, "{:5}"                  );
        TEST_PARSE_SUCCESS_F(Streamable, "{:5.0}"                );
        TEST_PARSE_SUCCESS_F(Streamable, "{:5.8}"                );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*<}"                 );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*<.0}"               );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*<.8}"               );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*<5}"                );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*<5.0}"              );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*<5.8}"              );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*>}"                 );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*>.0}"               );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*>.8}"               );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*>5}"                );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*>5.0}"              );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*>5.8}"              );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*^}"                 );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*^.0}"               );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*^.8}"               );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*^5}"                );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*^5.0}"              );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*^5.8}"              );
        TEST_PARSE_SUCCESS_F(Streamable, "{:{}.{}}"              );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*<{}.{}}"            );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*>{}.{}}"            );
        TEST_PARSE_SUCCESS_F(Streamable, "{:*^{}.{}}"            );
        TEST_PARSE_SUCCESS_F(Streamable, "{0:{1}.{1}}"           );
        TEST_PARSE_SUCCESS_F(Streamable, "{0:*<{1}.{1}}"         );
        TEST_PARSE_SUCCESS_F(Streamable, "{0:*>{1}.{1}}"         );
        TEST_PARSE_SUCCESS_F(Streamable, "{0:*^{1}.{1}}"         );

#if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE <= 13
  #define TPS TEST_PARSE_SUCCESS_VF
#else
  #define TPS TEST_PARSE_SUCCESS_F
#endif

        // A selection of valid format strings (unicode)
        TPS(Streamable, "{:\xF0\x9F\x98\x80<}"         );
        TPS(Streamable, "{:\xF0\x9F\x98\x80<.0}"       );
        TPS(Streamable, "{:\xF0\x9F\x98\x80<.8}"       );
        TPS(Streamable, "{:\xF0\x9F\x98\x80<5}"        );
        TPS(Streamable, "{:\xF0\x9F\x98\x80<5.0}"      );
        TPS(Streamable, "{:\xF0\x9F\x98\x80<5.8}"      );
        TPS(Streamable, "{:\xF0\x9F\x98\x80>}"         );
        TPS(Streamable, "{:\xF0\x9F\x98\x80>.0}"       );
        TPS(Streamable, "{:\xF0\x9F\x98\x80>.8}"       );
        TPS(Streamable, "{:\xF0\x9F\x98\x80>5}"        );
        TPS(Streamable, "{:\xF0\x9F\x98\x80>5.0}"      );
        TPS(Streamable, "{:\xF0\x9F\x98\x80>5.8}"      );
        TPS(Streamable, "{:\xF0\x9F\x98\x80^}"         );
        TPS(Streamable, "{:\xF0\x9F\x98\x80^.0}"       );
        TPS(Streamable, "{:\xF0\x9F\x98\x80^.8}"       );
        TPS(Streamable, "{:\xF0\x9F\x98\x80^5}"        );
        TPS(Streamable, "{:\xF0\x9F\x98\x80^5.0}"      );
        TPS(Streamable, "{:\xF0\x9F\x98\x80^5.8}"      );
        TPS(Streamable, "{:\xF0\x9F\x98\x80<{}.{}}"    );
        TPS(Streamable, "{:\xF0\x9F\x98\x80>{}.{}}"    );
        TPS(Streamable, "{:\xF0\x9F\x98\x80^{}.{}}"    );
        TPS(Streamable, "{0:\xF0\x9F\x98\x80<{1}.{1}}" );
        TPS(Streamable, "{0:\xF0\x9F\x98\x80>{1}.{1}}" );
        TPS(Streamable, "{0:\xF0\x9F\x98\x80^{1}.{1}}" );
        #undef TPS
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        // 1. We can assign 'bsl::formatter' types for `Streamed` wrapper
        //    specializations.
        //
        // 2. We can assign 'std::formatter' types for `Streamed` wrapper
        //    specializations.
        //
        // Plan:
        // 1. Construct two 'bsl::formatter's for `Streamed` wrapper
        //    specializations, and assign one to the other. (C-1)
        //
        // 2. Construct two 'std::formatter's for `Streamed` wrapper
        //    specializations, and assign one to the other. (C-2)
        //
        // Testing:
        //   operator=(const formatter &);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING ASSIGNMENT OPERATOR"
                          "\n===========================");

        {
            const bsl::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            bsl::formatter<bslfmt::Streamed<Streamable>, char>       dummy2_c;
            dummy2_c = dummy_c;
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        {
            const std::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            std::formatter<bslfmt::Streamed<Streamable>, char>       dummy2_c;
            dummy2_c = dummy_c;
        }
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING SWAP
        //
        // Concerns:
        // 1. We can swap two 'bsl::formatter' types of `Streamed` wrapper
        //    instantiations.
        //
        // 2. We can swap two 'std::formatter' types of `Streamed` wrapper
        //    instantiations.
        //
        // Plan:
        // 1. Construct two 'bsl::formatter's for a wrapper instance (type) and
        //    and swap them. (C-1)
        //
        // 2. Construct two 'std::formatter's for a wrapper instance (type) and
        //    and swap them. (C-2)
        //
        // Testing:
        //   swap(formatter &, formatter&);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING SWAP"
                          "\n============");

        {
            bsl::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            bsl::formatter<bslfmt::Streamed<Streamable>, char> dummy2_c;
            bsl::swap(dummy_c, dummy2_c);
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        {
            std::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            std::formatter<bslfmt::Streamed<Streamable>, char> dummy2_c;
            bsl::swap(dummy_c, dummy2_c);
        }
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        // 1. We can copy construct 'bsl::formatter' types of `Streamed`
        //    wrapper instances.
        //
        // 2. We can copy construct 'std::formatter' types of `Streamed`
        //    wrapper instances.
        //
        // Plan:
        // 1. Construct a 'bsl::formatter' for a wrapper instance and copy it.
        //    (C-1)
        //
        // 2. Construct a 'std::formatter' for a wrapper instance and copy it.
        //    (C-2)
        //
        // Testing:
        //   formatter(const formatter &);
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING COPY CONSTRUCTOR"
                          "\n========================");

        {
            const bsl::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            bsl::formatter<bslfmt::Streamed<Streamable>, char> copy_c(dummy_c);
            (void)copy_c;
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        {
            const std::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            std::formatter<bslfmt::Streamed<Streamable>, char> copy_c(dummy_c);
            (void)copy_c;
        }
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING (PRIMITIVE) GENERATORS
        //   The only generator for 'formatter' is the default constructor.
        //
        // Concerns:
        // 1. `bsl::formatter` types for `Streamed` types are default
        //    constructible.
        //
        // 2. `std::formatter` types for `Streamed` types are default
        //    constructible.
        //
        // Plan:
        // 1. Default construct a 'bsl::formatter' for a `Streamed` wrapper type
        //    instance.  (C-1)
        //
        // 2. Default construct a 'std::formatter' for a `Streamed` wrapper type
        //    instance.  (C-1)
        //
        // Testing:
        //   formatter();
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING (PRIMITIVE) GENERATORS"
                          "\n==============================");

        {
            bsl::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            (void)dummy_c;
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        {
            std::formatter<bslfmt::Streamed<Streamable>, char> dummy_c;
            (void)dummy_c;
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST MACHINERY
        //
        // Concern:
        // 1. `VariableLengthStreamable` print as many characters as we ask.
        //
        // Plan:
        // 1. Use a loop up to the maximum length.
        //
        // Testing:
        //   VariableLengthStreamable
        // --------------------------------------------------------------------

        if (verbose) puts("\nTEST MACHINERY"
                          "\n==============");

        static const char ALL_CHARS[] =
              "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const int NUM_ALL_CHARS = static_cast<int>(sizeof(ALL_CHARS) - 1);

        for (int i = 0; i < NUM_ALL_CHARS; ++i) {

            if (veryVerbose) {
                P(i);
            }

            const VariableLengthStreamable tester(i);

            bsl::ostringstream os;
            os << tester;

            const bsl::string_view result = os.view();

            ASSERTV(i, result.length() == static_cast<size_t>(i));
            ASSERTV(i,
                    0 == memcmp(result.data(),
                                ALL_CHARS,
                                bsl::min<size_t>(result.length(), i)));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. That basic functionality appears to work as advertised before
        //    before beginning testing in earnest.
        //
        // 2. That warning is generated for wrapped-types that already have a
        //    formatter for `char` (and the type itself) enabled on compilers
        //    that support concepts.
        //
        // Plan:
        // 1. Test formatting wrapped objects.  (C-1)
        //
        // 2. Attempt to format an `int` (if warning demonstration is
        //    requested) and observe if a warning is generated for that line of
        //    the test driver.  (C-2)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

        CanStream        mX("0123456789");
        const CanStream& X = mX;

#define TEST_LINE_BSL(fmt, res)                                               \
    result = bsl::format(fmt, bslfmt::Streamed<CanStream>(X));                \
    ASSERTV(result.c_str(), result == res);                                   \
                                                                              \
    result = bsl::format(fmt, bslfmt::streamed(X));                           \
    ASSERTV(result.c_str(), result == res)

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #define TEST_LINE_STD(fmt, res)                                             \
    stdResult = std::format(fmt, bslfmt::Streamed<CanStream>(X));             \
    ASSERTV(stdResult.c_str(), stdResult == res);                             \
                                                                              \
    stdResult = std::format(fmt, bslfmt::streamed(X));                        \
    ASSERTV(stdResult.c_str(), stdResult == res)
#else
  #define TEST_LINE_STD(fmt, res)
#endif

#define TEST_LINE(fmt, res)                                                   \
    TEST_LINE_BSL(fmt, res);                                                  \
    TEST_LINE_STD(fmt, res)

        bsl::string result;
        std::string stdResult;

        TEST_LINE("{}",     "0123456789"  );
        TEST_LINE("{:12}",  "0123456789  ");
        TEST_LINE("{:<12}", "0123456789  ");
        TEST_LINE("{:>12}", "  0123456789");
        TEST_LINE("{:^12}", " 0123456789 ");

        TEST_LINE("{:10.8}",  "01234567  ");
        TEST_LINE("{:<10.8}", "01234567  ");
        TEST_LINE("{:>10.8}", "  01234567");
        TEST_LINE("{:^10.8}", " 01234567 ");

        TEST_LINE("{:10.12}",  "0123456789");
        TEST_LINE("{:<10.12}", "0123456789");
        TEST_LINE("{:<10.12}", "0123456789");
        TEST_LINE("{:^10.12}", "0123456789");

        TEST_LINE("{:*<12}", "0123456789**");
        TEST_LINE("{:*>12}", "**0123456789");
        TEST_LINE("{:*^12}", "*0123456789*");

        TEST_LINE("{:*<10.7}", "0123456***");
        TEST_LINE("{:*>10.7}", "***0123456");
        TEST_LINE("{:*^10.7}", "*0123456**");

        mX.setContent(
          //           1         2         3         4         5         6
          //  12345678901234567890123456789012345678901234567890123456789012
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

        TEST_LINE(
             "{}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

        TEST_LINE(
             "{:62}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
            "{:63}",
            "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ");
        TEST_LINE(
           "{:64}",
           "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ  ");
        TEST_LINE(
        "{:67}",
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ     ");
        TEST_LINE(
        "{:<67}",
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ     ");
        TEST_LINE(
        "{:^67}",
        "  0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ   ");
        TEST_LINE(
        "{:>67}",
        "     0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
        "{:-<67}",
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-----");
        TEST_LINE(
        "{::^67}",
        "::0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ:::");
        TEST_LINE(
        "{:^>67}",
        "^^^^^0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:61}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:60}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:30}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:1}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

        TEST_LINE(
             "{:.100}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:.70}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:.63}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:.62}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
        TEST_LINE(
             "{:.61}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXY");
        TEST_LINE(
             "{:.60}",
             "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWX");
        TEST_LINE( "{:.36}", "0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE( "{:.10}", "0123456789");

        TEST_LINE("{:1.36}",    "0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE("{:10.36}",   "0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE("{:30.36}",   "0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE("{:36.36}",   "0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE("{:37.36}",   "0123456789abcdefghijklmnopqrstuvwxyz ");
        TEST_LINE("{:<37.36}",  "0123456789abcdefghijklmnopqrstuvwxyz ");
        TEST_LINE("{:^37.36}",  "0123456789abcdefghijklmnopqrstuvwxyz ");
        TEST_LINE("{:>37.36}",  " 0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE("{:<40.36}",  "0123456789abcdefghijklmnopqrstuvwxyz    ");
        TEST_LINE("{:^40.36}",  "  0123456789abcdefghijklmnopqrstuvwxyz  ");
        TEST_LINE("{:>40.36}",  "    0123456789abcdefghijklmnopqrstuvwxyz");
        TEST_LINE("{:|<40.36}", "0123456789abcdefghijklmnopqrstuvwxyz||||");
        TEST_LINE("{:+^40.36}", "++0123456789abcdefghijklmnopqrstuvwxyz++");
        TEST_LINE("{:.>40.36}", "....0123456789abcdefghijklmnopqrstuvwxyz");

#ifdef BSLFMT_STREAMED_DEMONSTRATE_FORMATTER_EXISTS_WARNING
        // Note that warnings for these two lines will appear only on compilers
        // that support concepts.
        bsl::string s = bsl::format("{}", bslfmt::streamed(12));
        s             = bsl::format("{}", bslfmt::Streamed<int>(12));
#endif  // BSLFMT_STREAMED_DEMONSTRATE_FORMATTER_EXISTS_WARNING
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
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
