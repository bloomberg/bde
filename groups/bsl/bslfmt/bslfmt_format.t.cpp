// bslfmt_format.t.cpp                                                -*-C++-*-
#include <bslfmt_format.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <bslfmt_formatterspecificationstandard.h> // Testing only

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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

template <class... t_ARGS>
bool doTestWithOracle(string_view                   result,
                      std::format_string<t_ARGS...> fmtstr,
                      t_ARGS&&...                   args)
{
    typedef string RT;

    RT res_bde = bsl::format(fmtstr, std::forward<t_ARGS>(args)...);
    RT res_std = std::format(fmtstr, std::forward<t_ARGS>(args)...);

    return (result == res_bde && result == res_std);
}

template <class... t_ARGS>
bool doTestWithOracle(wstring_view                   result,
                      std::wformat_string<t_ARGS...> fmtstr,
                      t_ARGS&&...                    args)
{
    typedef wstring RT;

    RT res_bde = bsl::format(fmtstr, std::forward<t_ARGS>(args)...);
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

template <class t_CHAR>
struct formatter<FormattableType, t_CHAR> {
  private:
    bsl::formatter<int, t_CHAR> d_formatter_bsl;

  private:
    // PRIVATE CLASS TYPES
    typedef bslfmt::FormatterSpecificationStandard<t_CHAR> FSS;

    // DATA
    FSS d_spec;

  public:
    template <class t_PARSE_CONTEXT>
    typename t_PARSE_CONTEXT::iterator BSLS_KEYWORD_CONSTEXPR_CPP20 parse(
                                                           t_PARSE_CONTEXT& pc)
    {
        FSS::parse(&d_spec, &pc, FSS::e_CATEGORY_STRING);

        return d_formatter_bsl.parse(pc);
    }

    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(const FormattableType& value,
                                               t_FORMAT_CONTEXT&      fc) const
    {
        FSS final_spec(d_spec);

        FSS::postprocess(&final_spec, fc);

        typedef bslfmt::FormatterSpecification_NumericValue FSNVAlue;

        FSNVAlue finalWidth(final_spec.postprocessedWidth());

        FSNVAlue finalPrecision(final_spec.postprocessedPrecision());

        static const char                   name[] = "FormattableType";
        typename t_FORMAT_CONTEXT::iterator out    = fc.out();

        out  = std::copy(name, name + strlen(name), out);
        *out++ = '{';
        fc.advance_to(out);
        out  = d_formatter_bsl.format(value.x, fc);
        *out++ = '}';
        return out;
    }
};


}  // close namespace bsl

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
#if !defined(_GLIBCXX_RELEASE) || _GLIBCXX_RELEASE >= 14
             const char8_t *fmt = u8"{0:\U0001F600<4}";

             int intValue = 42;

             std::string rv1 = std::vformat((const char *)fmt,
                                             std::make_format_args(intValue));

             // We cannot do this test yet without integer formattters.
             //bsl::string rv2 = bslfmt::vformat((const char *)fmt,
             //                               bslfmt::make_format_args(intValue));

             //ASSERT(rv1 == rv2);
             ASSERT(rv1 == (const char *)u8"42\U0001F600\U0001F600");
#endif
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

            check(bsl::format("The value of {1:{0}} is {0}", ft.x, ft),
                  "The value of FormattableType{37} is 37");
            check(bsl::vformat("The value of {1:{0}} is {0}",
                                  bsl::make_format_args(ft.x, ft)),
                  "The value of FormattableType{37} is 37");

            check(bsl::format("The value of {1:{0}} is {0}", ft.x, ft),
               "The value of FormattableType{37} is 37");
            check(bsl::vformat("The value of {1:{0}} is {0}",
                                bsl::make_format_args(ft.x, ft)),
                "The value of FormattableType{37} is 37");

            std::size_t len =
                  bsl::formatted_size("The value of {1:{0}} is {0}", ft.x, ft);
            ASSERTV(len, 38 == len);

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
