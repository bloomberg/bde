// bslfmt_format.t.cpp                                                -*-C++-*-
#include <bslfmt_format.h>

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

void check(const bsl::string& actual, const char *expected) {
    ASSERT(expected == actual);
}

void check(const bsl::wstring& actual, const wchar_t *expected)
{
    ASSERT(expected == actual);
}

void check(const std::string&, const char *) {
    printf("'bsl::format' should return 'bsl::string'\n");
    ASSERT(false);
}

//
//template <class... t_ARGS>
//bool doTestWithOracle(string_view              result,
//                      format_string<t_ARGS...> fmtstr,
//                      t_ARGS&&...              args)
//{
//#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
//    typedef string RT;
//
//    RT res_our   = bslfmt::format(fmtstr.get(), args...);
//    RT res_alias = bsl::format(fmtstr, args...);
//    RT res_std   = std::format(fmtstr, args...);
//
//    return (result == res_our &&
//            result == res_alias &&
//            result == res_std);
//#else
//    return (result == bslfmt::format(fmtstr.get(), args...));
//#endif
//}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&                 \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
#define BSLFMT_FORMAT_STRING_PARAMETER                                        \
    bslfmt::format_string<bsl::decay_t<t_ARGS>...>
#define BSLFMT_FORMAT_WSTRING_PARAMETER                                       \
    bslfmt::wformat_string<bsl::decay_t<t_ARGS>...>
#else
// We cannot define format_string<t_ARGS...> in a C++03 compliant manner, so
// have to use non-template versions instead.
#define BSLFMT_FORMAT_STRING_PARAMETER bslfmt::format_string
#define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

template <class... t_ARGS>
bool doTestWithOracle(string_view              result,
                      std::format_string<t_ARGS...> fmtstr,
                      t_ARGS&&...              args)
{
    typedef string RT;

    // Work around for the fact we cannot construct a bslfmt::format_string
    // from non-consteval fmtstr.
    BSLFMT_FORMAT_STRING_PARAMETER          bslfmt("");
    bslfmt::Format_FormatString_Test_Updater<char> tu;

    tu.update(&bslfmt, fmtstr.get());

    RT res_bde = bslfmt::format(bslfmt, std::forward<t_ARGS>(args)...);
    RT res_std = std::format(fmtstr, std::forward<t_ARGS>(args)...);

    return (result == res_bde && result == res_std);
}
template <class... t_ARGS>
bool doTestWithOracle(wstring_view              result,
                      std::wformat_string<t_ARGS...> fmtstr,
                      t_ARGS&&...              args)
{
    typedef wstring RT;

    // Work around for the fact we cannot construct a bslfmt::format_string
    // from non-consteval fmtstr.
    BSLFMT_FORMAT_WSTRING_PARAMETER             bslfmt(L"");
    bslfmt::Format_FormatString_Test_Updater<wchar_t>  tu;

    tu.update(&bslfmt, fmtstr.get());

    RT res_bde = bslfmt::format(bslfmt, std::forward<t_ARGS>(args)...);
    RT res_std = std::format(fmtstr, std::forward<t_ARGS>(args)...);

    return (result == res_bde && result == res_std);
}
#  define DOTESTWITHORACLE(...) doTestWithOracle(__VA_ARGS__);
#else
# define DOTESTWITHORACLE(RESULT, ...)                                        \
             (RESULT == bslfmt::format(__VA_ARGS__));
#endif

struct NonFormattableType {};

struct FormattableType {
    int x;
};

namespace bsl {
#if 0
template <>
struct formatter<FormattableType, char> {
    std::formatter<int, char> d_formatter;

    BSL_FORMAT_CONSTEXPR bslfmt::bslfmt_FormatUtil_Alias_FormatParseContext::iterator parse(
                                      bslfmt::bslfmt_FormatUtil_Alias_FormatParseContext& pc)
    {
        return d_formatter.parse(pc);
    }

    template <class t_OUT>
    t_OUT format(const FormattableType&             value,
                 bslfmt::bslfmt_FormatUtil_Alias_BasicFormatContext<t_OUT, char>& fc) const
    {
        const char name[] = "FormattableType";
        t_OUT out = fc.out();
        out = std::copy(name, name + strlen(name), out);
        *out++ = '{';
        fc.advance_to(out);
        out = d_formatter.format(value.x, fc);
        *out++ = '}';
        return out;
    }
};
#endif 
#if 0
template <>
struct formatter<FormattableType, char> {
    bsl::formatter<int, char> d_formatter;

    BSLS_KEYWORD_CONSTEXPR_CPP20 format_parse_context::iterator parse(
                                                      format_parse_context& pc)
    {
        return d_formatter.parse(pc);
    }

    template <class t_OUT>
    t_OUT format(const FormattableType&             value,
                 basic_format_context<t_OUT, char>& fc) const
    {
        const char name[] = "FormattableType";
        t_OUT      out    = fc.out();
        out               = std::copy(name, name + strlen(name), out);
        *out++            = '{';
        fc.advance_to(out);
        out    = d_formatter.format(value.x, fc);
        *out++ = '}';
        return out;
    }
};
#endif

#if 0
template <>
struct formatter<FormattableType, char> {
    bsl::formatter<int, char> d_formatter_bsl;
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    std::formatter<int, char> d_formatter_std;
#endif

    BSLS_KEYWORD_CONSTEXPR_CPP20 format_parse_context::iterator parse(
                                                      bslfmt::format_parse_context& pc)
    {
        return d_formatter_bsl.parse(pc);
    }

    template <class t_OUT>
    t_OUT format(const FormattableType&             value,
                 bslfmt::basic_format_context<t_OUT, char>& fc) const
    {
        const char name[] = "FormattableType";
        t_OUT out = fc.out();
        out = std::copy(name, name + strlen(name), out);
        *out++ = '{';
        fc.advance_to(out);
        out    = d_formatter_bsl.format(value.x, fc);
        *out++ = '}';
        return out;
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    BSLS_KEYWORD_CONSTEXPR_CPP20 format_parse_context::iterator parse(
                                                      std::format_parse_context& pc)
    {
        return d_formatter_std.parse(pc);
    }

    template <class t_OUT>
    t_OUT format(const FormattableType&             value,
                 std::basic_format_context<t_OUT, char>& fc) const
    {
        const char name[] = "FormattableType";
        t_OUT      out    = fc.out();
        out               = std::copy(name, name + strlen(name), out);
        *out++            = '{';
        fc.advance_to(out);
        out    = d_formatter_std.format(value.x, fc);
        *out++ = '}';
        return out;
    }
#endif
};
#endif

#if 1
template <class t_CHAR>
struct formatter<FormattableType, t_CHAR> {
  private:
    bsl::formatter<int, t_CHAR> d_formatter_bsl;
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    std::formatter<int, t_CHAR> d_formatter_std;
#endif

  private:
    // PRIVATE CLASS TYPES
    typedef bslfmt::FormatterSpecificationStandard<t_CHAR> FSS;

    // DATA
    FSS d_spec;

  public:


        typename bslfmt::basic_format_parse_context<t_CHAR>::iterator
        BSLS_KEYWORD_CONSTEXPR_CPP20
        parse(bslfmt::basic_format_parse_context<t_CHAR>& pc)
    {
        FSS::parse(&d_spec, &pc, FSS::e_CATEGORY_STRING);

        return d_formatter_bsl.parse(pc);
    }

    template <class t_OUT>
    t_OUT format(const FormattableType&             value,
                 bslfmt::basic_format_context<t_OUT, t_CHAR>& fc) const
    {
        FSS final_spec(d_spec);

        FSS::postprocess(&final_spec, fc);

        typedef bslfmt::FormatterSpecification_NumericValue FSNVAlue;

        FSNVAlue finalWidth(final_spec.postprocessedWidth());

        FSNVAlue finalPrecision(final_spec.postprocessedPrecision());

        const char name[] = "FormattableType";
        t_OUT out = fc.out();
        out  = std::copy(name, name + strlen(name), out);
        *out = '{';
        ++out;
        fc.advance_to(out);
        out    = d_formatter_bsl.format(value.x, fc);
        *out   = '}';
        ++out;
        return out;
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

        typename std::basic_format_parse_context<t_CHAR>::iterator
        BSLS_KEYWORD_CONSTEXPR_CPP20
        parse(std::basic_format_parse_context<t_CHAR>& pc)
    {
        FSS::parse(&d_spec, &pc, FSS::e_CATEGORY_STRING);

        return d_formatter_std.parse(pc);
    }

    template <class t_OUT>
    t_OUT format(const FormattableType&             value,
                 std::basic_format_context<t_OUT, t_CHAR>& fc) const
    {
        FSS final_spec(d_spec);

        FSS::postprocess(&final_spec, fc);

        typedef bslfmt::FormatterSpecification_NumericValue FSNVAlue;

        FSNVAlue finalWidth(final_spec.postprocessedWidth());

        FSNVAlue finalPrecision(final_spec.postprocessedPrecision());

        const char name[] = "FormattableType";
        t_OUT      out    = fc.out();
        out               = std::copy(name, name + strlen(name), out);
        *out              = '{';
        ++out;
        fc.advance_to(out);
        out    = d_formatter_std.format(value.x, fc);
        *out   = '}';
        ++out;
        return out;
    }
#endif
};
#endif
}  // close namespace bsl

#if 0
#include <iostream>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <locale>

void dummy()
{
    SetConsoleOutputCP(65001);

    const char8_t * random = u8"\U0001F600";

    const char8_t *ufills[] = {u8"",
                              u8"*<",
                              u8"*>",
                              u8"*^",
                              u8"\U0001F600<",
                              u8"\U0001F600>",
                              u8"\U0001F600^"};
    const char *afills[] = {"",
                              "*<",
                              "*>",
                              "*^",
                              "\\U0001F600<",
                              "\\U0001F600>",
                              "\\U0001F600^"};
    const char *cafills[] =
        {"", "*<", "*>", "*^", "\\xF0\\x9F\\x98\\x80<", "\\xF0\\x9F\\x98\\x80>", "\\xF0\\x9F\\x98\\x80^"};

    const char8_t *uwidths[] = {u8"", u8"5"};
    const char *awidths[] = {"", "5"};

    const char8_t *uprecisions[] =
                                {u8"", u8".0", u8".3"};
    const char *aprecisions[] =
                                {"", ".0", ".8"};

    const char8_t *uinputs[] = {
        u8"",
        u8"x",
        u8"abcdefghijklm",
        u8"\u006e\u0303p\u006e\u0303q\u006e\u0303r"
        u8"\u006e\u0303s\u006e\u0303t"};

    const char *ainputs[] = {
        "",
        "x",
        "abcdefghijklm",
        "\\u006e\\u0303p\\u006e\\u0303q\\u006e\\u0303r"
        "\\u006e\\u0303s\\u006e\\u0303t"};

    int lines = 0;

    std::string aprev = "unknown";

    for (int at = 0; at < 3; at++) {
    
    for (int f = 0; f < sizeof(ufills) / sizeof(char8_t *); f++) {
            for (int w = 0; w < sizeof(uwidths) / sizeof(char8_t *); w++) {
                for (int p = 0; p < sizeof(uprecisions) / sizeof(char8_t *);
                     p++) {
                    //for (int i = 0;
                    // i < sizeof(uinputs) / sizeof(char8_t *); i++) {
                    int         i    = 0;

                    std::string afmt = "{";
                    if (at == 2)
                        afmt += "0";
                    afmt += ":";

                    afmt += (const char *)afills[f];

                    if (at == 0 && awidths[w] != "") {
                        afmt += (const char *)awidths[w];
                    }
                    if (at == 1 && awidths[w] != "") {
                        afmt += "{}";
                    }
                    if (at == 2 && awidths[w] != "") {
                        afmt += "{1}";
                    }

                    if (at == 0 && aprecisions[p] != "") {
                        afmt += (const char *)aprecisions[p];
                    }
                    if (at == 1 && aprecisions[p] != "") {
                        afmt += ".{}";
                    }
                    if (at == 2 && aprecisions[p] != "") {
                        afmt += ".{1}";
                    }
                    
                    afmt += "}";

                    std::string cafmt = "{";
                    if (at == 2)
                        cafmt += "0";
                    cafmt += ":";

                    cafmt += (const char *)cafills[f];

                    if (at == 0 && awidths[w] != "") {
                        cafmt += (const char *)awidths[w];
                    }
                    if (at == 1 && awidths[w] != "") {
                        cafmt += "{}";
                    }
                    if (at == 2 && awidths[w] != "") {
                        cafmt += "{1}";
                    }

                    if (at == 0 && aprecisions[p] != "") {
                        cafmt += (const char *)aprecisions[p];
                    }
                    if (at == 1 && aprecisions[p] != "") {
                        cafmt += ".{}";
                    }
                    if (at == 2 && aprecisions[p] != "") {
                        cafmt += ".{1}";
                    }

                    cafmt += "}";



                    std::string::size_type pos;
                    bool                   unicode = false;

                    if (f > 3)
                        unicode = true;
                    if (afmt != aprev) {
                        std::string qafmt = std::format("\"{}\"", afmt);
                        std::string cqafmt = std::format(" \"{}\"", cafmt);
                        std::string wqafmt = std::format("L\"{}\"", afmt);
                        if (lines < 99999 && unicode)
                            std::cout
                                << std::format("        TPS(char,    "
                                               "{:32}, oracle_uni);\n",
                                               cqafmt)
                                << std::format("        TPS(wchar_t, "
                                               "{:32}, oracle_uni);",
                                               wqafmt)
                                << std::endl;
                    }

                    aprev = afmt;
                    //for (int j = 0; j < result.size(); j++) {
                    //    std::cout << j << ":
                    //    " << (int)result[j] << std::endl;
                    //}
                    if (unicode)
                        lines++;
                        //}
                }
            }
        }
    }

    std::cout << (const char *)u8"hello world"
              << (const char *)u8"\U0001F600<";

    std::cout << std::format("{:s}", (const char *)u8"\U0001F600<");

    std::cout << "\nlines=" << lines << std::endl;
}
#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
         //dummy();
         //return 0;

    int        aaa     = sizeof(char);
    int        bbb     = sizeof(wchar_t);

    (void) aaa;
    (void) bbb;

    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;
    // const bool veryVerbose = argc > 3;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 1: {
        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        std::formatter<int, char>              dummy1a;
        std::formatter<bsl::string, char>      dummy1b;
        std::formatter<bsl::wstring, wchar_t>  dummy1c;
        (void) dummy1a;
        (void) dummy1b;
        (void) dummy1c;
#endif
        bsl::formatter<bsl::string_view, char> dummy2a;
        (void) dummy2a;

        { // simple test to see if we can pass in string and string_view

            const std::string v1("Test 1");
            const bsl::string v2("Test 2");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
            // const char8_t *fmt = u8"{0:\U0001F600<6}";
            // //const char8_t *fmt = u8"{:\U0000006e\U00000303<6}";

            // std::string rv1 = std::vformat((const char *)fmt,
            //                                 std::make_format_args(1));
            // std::string rv2 = std::vformat((const char *)u8"{:\U0001F600<6}",
            //                     std::make_format_args((int)1));
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
            const std::string_view v3("Test 3");
#endif
            const bsl::string_view v4("Test 4");

            check(bsl::format("{}", v1), "Test 1");
            check(bsl::format("{}", v2), "Test 2");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
            check(bsl::format("{}", v3), "Test 3");
#endif
            check(bsl::format("{}", v4), "Test 4");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
            ASSERT(std::format("{}", v1) == bsl::string("Test 1"));
            ASSERT(std::format("{}", v2) == bsl::string("Test 2"));

            ASSERT(std::format("{}", v3) == bsl::string("Test 3"));
            ASSERT(std::format("{}", v4) == bsl::string("Test 4"));
#endif

        }

        { // simple test of format with char
            const bsl::string  intro  =  "Here is a simple equation";
            const int          x      = 1;
            const int          y      = 2;
            const int          sum    = x + y;

            check(bsl::format(L"{}", x), L"1");

            check(bsl::format("{}", y),
                "2");
            check(bsl::format("{}", 'x'), "x");
            check(bsl::format("{}: {} + {} = {}", intro, x, y, sum),
                "Here is a simple equation: 1 + 2 = 3");
            check(bsl::format(L"{}", L"Hello World"),
                L"Hello World");

            DOTESTWITHORACLE("Here is a simple equation: 1 + 2 = 3",
                        "{}: {} + {} = {}",
                        intro,
                        x,
                        y,
                        sum);
            check(bsl::format("{}: {} + {} = {}", intro, x, y, sum),
                "Here is a simple equation: 1 + 2 = 3");
            check(bsl::vformat(
                                        "{}: {} + {} = {}",
                        bsl::make_format_args(intro, x, y, sum)),
                "Here is a simple equation: 1 + 2 = 3");

            FormattableType ft;
            ft.x = 37;

            check(bslfmt::format("The value of {1:{0}} is {0}", ft.x, ft),
                  "The value of FormattableType{37} is 37");
            check(bslfmt::vformat("The value of {1:{0}} is {0}",
                                  bslfmt::make_format_args(ft.x, ft)),
                  "The value of FormattableType{37} is 37");

            check(bsl::format("The value of {1:{0}} is {0}", ft.x, ft),
               "The value of FormattableType{37} is 37");
             check(bsl::vformat("The value of {1:{0}} is {0}",
                                bsl::make_format_args(ft.x, ft)),
                "The value of FormattableType{37} is 37");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
             ASSERT((std::format("The value of {1:{0}} is {0}", ft.x, ft) ==
                   "The value of FormattableType{37} is 37"));
             ASSERT((std::vformat("The value of {1:{0}} is {0}",
                                bsl::make_format_args(ft.x, ft)) ==
                   "The value of FormattableType{37} is 37"));
#endif
        }
        { // simple test of format with wchar_t
            const bsl::wstring  intro = L"Here is a simple equation";
            const int          x      = 1;
            const int          y      = 2;
            const int          sum    = x + y;

            check(bsl::format(L"{}", x), L"1");

            check(bsl::format(L"{}", y), L"2");
            check(bsl::format(L"{}", (wchar_t)'x'), L"x");
            check(bsl::format(L"{}: {} + {} = {}", intro, x, y, sum),
                L"Here is a simple equation: 1 + 2 = 3");
            check(bsl::format(L"{}", L"Hello World"),
                L"Hello World");

            DOTESTWITHORACLE(L"Here is a simple equation: 1 + 2 = 3",
                        L"{}: {} + {} = {}",
                        intro,
                        x,
                        y,
                        sum);
            check(bsl::format(L"{}: {} + {} = {}", intro, x, y, sum),
                L"Here is a simple equation: 1 + 2 = 3");
            check(bsl::vformat(L"{}: {} + {} = {}",
                        bsl::make_wformat_args(intro, x, y, sum)),
                L"Here is a simple equation: 1 + 2 = 3");

            FormattableType ft;
            ft.x = 37;
            check(bsl::format(L"The value of {1} is {0}", ft.x, ft),
                L"The value of FormattableType{37} is 37");
            check(bsl::vformat(L"The value of {1} is {0}",
                                bsl::make_wformat_args(ft.x, ft)),
                L"The value of FormattableType{37} is 37");
        }
        { // Test format of char with wchar_t output
            check(bsl::format(L"{}", (char)'x'), L"x");
        }
        { // Simple test of format_to with char string
            bsl::string temp;
            bsl::format_to(&temp, "{}", "Hello World");
            check(temp, "Hello World");
            ptrdiff_t count = bsl::format_to_n(&temp, 5, "{}", "Hello World");
            ASSERT(11 == count);
            check(temp, "Hello");
        }
        { // Simple test of format_to with wchar_t string
            bsl::wstring temp;
            bsl::format_to(&temp, L"{}", L"Hello World");
            check(temp, L"Hello World");
            ptrdiff_t count = bsl::format_to_n(&temp, 5, L"{}", L"Hello World");
            ASSERT(11 == count);
            check(temp, L"Hello");
        }
        { // Simple test of formatted_size with char
            ptrdiff_t count = bsl::formatted_size("{}", "Hello World");
            ASSERT(11 == count);
        }
        { // Simple test of formatted_size with wchar_t
            ptrdiff_t count = bsl::formatted_size(L"{}", L"Hello World");
            ASSERT(11 == count);
        }
        { // Simple test of format_to with char output iterator
            char temp2[64];
            char *it = bsl::format_to(temp2, "{}", "Hello World");
            *it      = 0;
            check(bsl::string(temp2), "Hello World");
            format_to_n_result<char *> result =
                               bsl::format_to_n(temp2, 5, "{}", "Hello World");
            *result.out = 0;
            ASSERT(11 == result.size);
            check(bsl::string(temp2), "Hello");
        }
        { // Simple test of format_to with wchar_t output iterator
            wchar_t temp2[64];
            wchar_t *it = bsl::format_to(temp2, L"{}", L"Hello World");
            *it      = 0;
            check(bsl::wstring(temp2), L"Hello World");
            format_to_n_result<wchar_t *> result =
                             bsl::format_to_n(temp2, 5, L"{}", L"Hello World");
            *result.out = 0;
            ASSERT(11 == result.size);
            check(bsl::wstring(temp2), L"Hello");
        }
        { // Simple test of vformat_to with char string
            bsl::string temp;
            bsl::vformat_to(&temp, "{}", make_format_args("Hello World"));
            check(temp, "Hello World");
        }
        { // Simple test of vformat_to with wchar_t string
            bsl::wstring temp;
            bsl::vformat_to(&temp, L"{}", make_wformat_args(L"Hello World"));
            check(temp, L"Hello World");
        }
        { // Simple test of format_to with char output iterator
            char temp2[64];
            char *it = bsl::vformat_to(temp2,
                                       "{}",
                                       make_format_args("Hello World"));
            *it      = 0;
            check(bsl::string(temp2), "Hello World");
        }
        { // Simple test of format_to with wchar_t output iterator
            wchar_t temp2[64];
            wchar_t *it = bsl::vformat_to(temp2,
                                          L"{}",
                                          make_wformat_args(L"Hello World"));
            *it      = 0;
            check(bsl::wstring(temp2), L"Hello World");
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
