// bslfmt_format_imp.t.cpp                                            -*-C++-*-
#include <bslfmt_format_imp.h>

#include <bsls_bsltestutil.h>
#include <bsls_stopwatch.h>  // Testing only

#include <bslstl_iomanip.h>  // Testing only
#include <bslstl_string.h>
#include <bslstl_stringstream.h>  // Testing only

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
    printf("'bslfmt::format' should return 'bsl::string'\n");
    ASSERT(false);
}


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
bool doTestWithOracle(string_view                   result,
                      std::format_string<t_ARGS...> fmtstr,
                      t_ARGS&&...                   args)
{
    typedef string RT;

    // Work around for the fact we cannot construct a bslfmt::format_string
    // from non-consteval fmtstr.
    BSLFMT_FORMAT_STRING_PARAMETER          bslfmt("");
    bslfmt::Format_String_TestUpdater<char> tu;

    tu.update(&bslfmt, fmtstr.get());

    RT res_bde = bslfmt::format(bslfmt, std::forward<t_ARGS>(args)...);
    RT res_std = std::format(fmtstr, std::forward<t_ARGS>(args)...);

    return (result == res_bde && result == res_std);
}

template <class... t_ARGS>
bool doTestWithOracle(wstring_view                   result,
                      std::wformat_string<t_ARGS...> fmtstr,
                      t_ARGS&&...                    args)
{
    typedef wstring RT;

    // Work around for the fact we cannot construct a bslfmt::format_string
    // from non-consteval fmtstr.
    BSLFMT_FORMAT_WSTRING_PARAMETER            bslfmt(L"");
    bslfmt::Format_String_TestUpdater<wchar_t> tu;

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

template <class t_CHAR>
struct formatter<FormattableType, t_CHAR> {
  private:
    bsl::formatter<int, t_CHAR> d_formatter_bsl;

  private:
    // PRIVATE CLASS TYPES
    typedef bslfmt::StandardFormatSpecification<t_CHAR> Specification;

    // DATA
    Specification d_spec;

  public:
    template <class t_PARSE_CONTEXT>
    typename t_PARSE_CONTEXT::iterator BSLS_KEYWORD_CONSTEXPR_CPP20 parse(
                                                 t_PARSE_CONTEXT& parseContext)
    {
        d_spec.parse(&parseContext, Specification::e_CATEGORY_STRING);

        return d_formatter_bsl.parse(parseContext);
    }

    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                    const FormattableType& value,
                                    t_FORMAT_CONTEXT&      formatContext) const
    {
        Specification finalSpec(d_spec);

        finalSpec.postprocess(formatContext);

        typedef bslfmt::FormatterSpecificationNumericValue NumericValue;

        NumericValue finalWidth(finalSpec.postprocessedWidth());

        NumericValue finalPrecision(finalSpec.postprocessedPrecision());

        bsl::basic_stringstream<t_CHAR> converter;

        converter << "FormattableType"
                  << "{"
                  << value.x
                  << "}";

        bsl::basic_string<t_CHAR> output;

        converter >> output;

        // We know `output.size()` is less than INT_MAX
        int width = static_cast<int>(output.size());
        if (finalWidth.category() != NumericValue::e_DEFAULT) {
            width = finalWidth.value();
        }

        // We know `output.size()` is less than INT_MAX
        int precision = static_cast<int>(output.size());
        if (finalPrecision.category() != NumericValue::e_DEFAULT) {
            precision = finalPrecision.value();
        }

        typename t_FORMAT_CONTEXT::iterator oit = formatContext.out();

        oit = std::copy(output.begin(), output.begin() + precision, oit);
        formatContext.advance_to(oit);

        if (width > precision) {
            for (int i = 0; i < width - precision; i++) {
                *oit++ = '=';
            }
        }

        formatContext.advance_to(oit);

        return oit;
    }
};

}  // close namespace bsl

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    int        aaa     = sizeof(char);
    int        bbb     = sizeof(wchar_t);

    (void)aaa;
    (void)bbb;

    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 1: {
        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        std::formatter<bsl::string_view, char> dummy1;
        (void) dummy1;

        std::formatter<bsl::string, char> dummy2;
        (void) dummy2;
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&                 \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
        bslfmt::format_string<int> dummy3("{}");
        (void) dummy3;
#endif

        bsl::formatter<bsl::string_view, char> dummy4;
        (void)dummy4;

        bsl::formatter<bsl::string, char> dummy5;
        (void)dummy5;

        bsl::formatter<std::string, char> dummy6;
        (void)dummy6;


        { // simple test of format with char
            const bsl::string  intro  =  "Here is a simple equation";
            const int          x      = 1;
            const int          y      = 2;
            const int          sum    = x + y;

            check(bslfmt::format(L"{}", x), L"1");

            check(bslfmt::format("{}", y),
                "2");
            check(bslfmt::format("{}", 'x'), "x");
            check(bslfmt::format("{}: {} + {} = {}", intro, x, y, sum),
                "Here is a simple equation: 1 + 2 = 3");
            check(bslfmt::format(L"{}", L"Hello World"),
                L"Hello World");

            DOTESTWITHORACLE("Here is a simple equation: 1 + 2 = 3",
                        "{}: {} + {} = {}",
                        intro,
                        x,
                        y,
                        sum);
            check(bslfmt::format("{}: {} + {} = {}", intro, x, y, sum),
                "Here is a simple equation: 1 + 2 = 3");
            check(bslfmt::vformat(
                                        "{}: {} + {} = {}",
                        bslfmt::make_format_args(intro, x, y, sum)),
                "Here is a simple equation: 1 + 2 = 3");

            FormattableType ft;
            ft.x = 37;


            check(bslfmt::format("The value of {1:} is {0}", ft.x, ft),
                  "The value of FormattableType{37} is 37");
            check(bslfmt::vformat("The value of {1:} is {0}",
                                  bslfmt::make_format_args(ft.x, ft)),
                  "The value of FormattableType{37} is 37");

            check(bslfmt::format("The value of {1:{0}} is {0}", ft.x, ft),
                  "The value of FormattableType{37}================== is 37");
            check(bslfmt::vformat("The value of {1:{0}} is {0}",
                                  bslfmt::make_format_args(ft.x, ft)),
                  "The value of FormattableType{37}================== is 37");

            int prec = 5;

            check(bslfmt::format("The value of {1:{0}.{2}} is {0}",
                                 ft.x,
                                 ft,
                                 prec),
                  "The value of Forma================================ is 37");
            check(bslfmt::vformat("The value of {1:{0}.{2}} is {0}",
                                  bslfmt::make_format_args(ft.x, ft, prec)),
                  "The value of Forma================================ is 37");

            check(bslfmt::format("The value of {:{}} is {}", ft, ft.x, ft.x),
                "The value of FormattableType{37}================== is 37");
            check(bslfmt::vformat("The value of {:{}} is {}",
                                bslfmt::make_format_args(ft, ft.x, ft.x)),
                "The value of FormattableType{37}================== is 37");

            std::size_t len = bslfmt::formatted_size(
                                                 "The value of {1:{0}} is {0}",
                                                 ft.x,
                                                 ft);
             ASSERTV(len, 56 == len);

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
             ASSERT((std::format("The value of {1:{0}} is {0}", ft.x, ft) ==
                  "The value of FormattableType{37}================== is 37"));
#endif
             ASSERT((bslfmt::vformat("The value of {1:{0}} is {0}",
                                bslfmt::make_format_args(ft.x, ft)) ==
                  "The value of FormattableType{37}================== is 37"));
        }
        { // simple test of format with wchar_t
            const bsl::wstring  intro = L"Here is a simple equation";
            const int          x      = 1;
            const int          y      = 2;
            const int          sum    = x + y;

            check(bslfmt::format(L"{}", x), L"1");

            check(bslfmt::format(L"{}", y), L"2");
            check(bslfmt::format(L"{}", (wchar_t)'x'), L"x");
            check(bslfmt::format(L"{}: {} + {} = {}", intro, x, y, sum),
                L"Here is a simple equation: 1 + 2 = 3");
            check(bslfmt::format(L"{}", L"Hello World"),
                L"Hello World");

            DOTESTWITHORACLE(L"Here is a simple equation: 1 + 2 = 3",
                        L"{}: {} + {} = {}",
                        intro,
                        x,
                        y,
                        sum);
            check(bslfmt::format(L"{}: {} + {} = {}", intro, x, y, sum),
                L"Here is a simple equation: 1 + 2 = 3");
            check(bslfmt::vformat(L"{}: {} + {} = {}",
                        bslfmt::make_wformat_args(intro, x, y, sum)),
                L"Here is a simple equation: 1 + 2 = 3");

            FormattableType ft;
            ft.x = 37;
            check(bslfmt::format(L"The value of {1} is {0}", ft.x, ft),
                L"The value of FormattableType{37} is 37");
            check(bslfmt::vformat(L"The value of {1} is {0}",
                                bslfmt::make_wformat_args(ft.x, ft)),
                L"The value of FormattableType{37} is 37");

            int prec = 5;

            check(bslfmt::format(L"The value of {1:{0}.{2}} is {0}",
                                 ft.x,
                                 ft,
                                 prec),
                  L"The value of Forma================================ is 37");
            check(bslfmt::vformat(L"The value of {1:{0}.{2}} is {0}",
                                  bslfmt::make_wformat_args(ft.x, ft, prec)),
                  L"The value of Forma================================ is 37");

            check(bslfmt::format(L"The value of {1:{0}} is {0}", ft.x, ft),
                  L"The value of FormattableType{37}================== is 37");
            check(bslfmt::vformat(L"The value of {1:{0}} is {0}",
                                  bslfmt::make_wformat_args(ft.x, ft)),
                  L"The value of FormattableType{37}================== is 37");
        }
        { // Test format of char with wchar_t output
            check(bslfmt::format(L"{}", (char)'x'), L"x");
        }
        { // Simple test of format_to with char string
            bsl::string temp;
            bslfmt::format_to(&temp, "{}", "Hello World");
            check(temp, "Hello World");
            ptrdiff_t count = bslfmt::format_to_n(&temp, 5, "{}", "Hello World");
            ASSERT(11 == count);
            check(temp, "Hello");
        }
        { // Simple test of format_to with wchar_t string
            bsl::wstring temp;
            bslfmt::format_to(&temp, L"{}", L"Hello World");
            check(temp, L"Hello World");
            ptrdiff_t count = bslfmt::format_to_n(&temp, 5, L"{}", L"Hello World");
            ASSERT(11 == count);
            check(temp, L"Hello");
        }
        { // Simple test of formatted_size with char
            ptrdiff_t count = bslfmt::formatted_size("{}", "Hello World");
            ASSERT(11 == count);
        }
        { // Simple test of formatted_size with wchar_t
            ptrdiff_t count = bslfmt::formatted_size(L"{}", L"Hello World");
            ASSERT(11 == count);
        }
        { // Simple test of format_to with char output iterator
            char temp2[64];
            char *it = bslfmt::format_to(temp2, "{}", "Hello World");
            *it      = 0;
            check(bsl::string(temp2), "Hello World");
            bslfmt::format_to_n_result<char *> result =
                               bslfmt::format_to_n(temp2, 5, "{}", "Hello World");
            *result.out = 0;
            ASSERT(11 == result.size);
            check(bsl::string(temp2), "Hello");
        }
        { // Simple test of format_to with wchar_t output iterator
            wchar_t temp2[64];
            wchar_t *it = bslfmt::format_to(temp2, L"{}", L"Hello World");
            *it      = 0;
            check(bsl::wstring(temp2), L"Hello World");
            bslfmt::format_to_n_result<wchar_t *> result =
                             bslfmt::format_to_n(temp2, 5, L"{}", L"Hello World");
            *result.out = 0;
            ASSERT(11 == result.size);
            check(bsl::wstring(temp2), L"Hello");
        }
        { // Simple test of vformat_to with char string
            bsl::string temp;
            bslfmt::vformat_to(&temp, "{}", bslfmt::make_format_args("Hello World"));
            check(temp, "Hello World");
        }
        { // Simple test of vformat_to with wchar_t string
            bsl::wstring temp;
            bslfmt::vformat_to(&temp, L"{}", bslfmt::make_wformat_args(L"Hello World"));
            check(temp, L"Hello World");
        }
        { // Simple test of format_to with char output iterator
            char temp2[64];
            char *it = bslfmt::vformat_to(temp2,
                                       "{}",
                                       bslfmt::make_format_args("Hello World"));
            *it      = 0;
            check(bsl::string(temp2), "Hello World");
        }
        { // Simple test of format_to with wchar_t output iterator
            wchar_t temp2[64];
            wchar_t *it = bslfmt::vformat_to(temp2,
                                          L"{}",
                                          bslfmt::make_wformat_args(L"Hello World"));
            *it      = 0;
            check(bsl::wstring(temp2), L"Hello World");
        }

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE MEASUREMENTS
        //
        // Concerns:
        // 1. Is format sufficiently performant compared to other methods.
        //
        // Plan:
        // 1. Perform the tests inside a loop and report the timing
        //    using `bsls_stopwatch`.
        //
        // Testing:
        //    bsl::format(int)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nPERFORMANCE MEASUREMENTS"
                   "\n========================\n");

        const int k_MILLION = 1000 * 1000;
        {
            const int k_MIL_ITERATIONS = 10;
            unsigned long int    value = 0;
            bsls::Stopwatch timer;
            timer.start();
            for (int i = 0; i < k_MIL_ITERATIONS * k_MILLION; ++i) {
                value += bsl::to_string(99.9).size();
            }
            timer.stop();
            printf("floating to_string %dM values (in seconds): %f\n",
                   k_MIL_ITERATIONS,
                   timer.elapsedTime());
            (void)value;
        }
        {
            const int k_MIL_ITERATIONS = 10;
            unsigned long int value = 0;
            bsls::Stopwatch   timer;
            timer.start();
            for (int i = 0; i < k_MIL_ITERATIONS * k_MILLION; ++i) {
                bsl::stringstream buf1;
                buf1 << "The value is ";
                buf1 << bsl::setw(9) << bsl::setprecision(3) << 42.1;
                value += buf1.str().size();
            }
            timer.stop();
            printf("floating bsl::stringstream %dM values (in seconds): %f\n",
                   k_MIL_ITERATIONS,
                   timer.elapsedTime());
            (void)value;
        }
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        {
            const int k_MIL_ITERATIONS = 10;
            unsigned long int value = 0;
            bsls::Stopwatch   timer;
            timer.start();
            for (int i = 0; i < k_MIL_ITERATIONS * k_MILLION; ++i) {
                value += std::format("The value is {:9f}", 42.1).size();
            }
            timer.stop();
            printf("floating std::format %dM values (in seconds): %f\n",
                   k_MIL_ITERATIONS,
                   timer.elapsedTime());
            (void)value;
        }
#endif
        {
            const int k_MIL_ITERATIONS = 10;
            unsigned long int value = 0;
            bsls::Stopwatch   timer;
            timer.start();
            for (int i = 0; i < k_MIL_ITERATIONS * k_MILLION; ++i) {
                value += bslfmt::format("The value is {:9f}", 42.1).size();
            }
            timer.stop();
            printf("floating bslfmt::format %dM values (in seconds): %f\n",
                   k_MIL_ITERATIONS,
                   timer.elapsedTime());
            (void)value;
        }
        {
            const int k_MIL_ITERATIONS = 10;
            unsigned long int value = 0;
            bsls::Stopwatch   timer;
            timer.start();
            for (int i = 0; i < k_MIL_ITERATIONS * k_MILLION; ++i) {
                value += bsl::to_string(99).size();
            }
            timer.stop();
            printf("integer to_string %dM values (in seconds): %f\n",
                   k_MIL_ITERATIONS,
                   timer.elapsedTime());
            (void)value;
        }
        {
            const int k_MIL_ITERATIONS = 10;
            unsigned long int value = 0;
            bsls::Stopwatch   timer;
            timer.start();
            for (int i = 0; i < k_MIL_ITERATIONS * k_MILLION; ++i) {
                bsl::stringstream buf1;
                buf1 << "The value is ";
                buf1 << bsl::setw(9) << 42;
                value += buf1.str().size();
            }
            timer.stop();
            printf("integer bsl::stringstream %dM values (in seconds): %f\n",
                   k_MIL_ITERATIONS,
                   timer.elapsedTime());
            (void)value;
        }
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        {
            const int k_MIL_ITERATIONS = 10;
            unsigned long int value = 0;
            bsls::Stopwatch   timer;
            timer.start();
            for (int i = 0; i < k_MIL_ITERATIONS * k_MILLION; ++i) {
                value += std::format("The value is {:9d}", 42).size();
            }
            timer.stop();
            printf("integer std::format %dM values (in seconds): %f\n",
                   k_MIL_ITERATIONS,
                   timer.elapsedTime());
            (void)value;
        }
#endif
        {
            const int k_MIL_ITERATIONS = 10;
            unsigned long int value = 0;
            bsls::Stopwatch   timer;
            timer.start();
            for (int i = 0; i < k_MIL_ITERATIONS * k_MILLION; ++i) {
                value += bslfmt::format("The value is {:9d}", 42).size();
            }
            timer.stop();
            printf("integer bslfmt::format %dM values (in seconds): %f\n",
                   k_MIL_ITERATIONS,
                   timer.elapsedTime());
            (void)value;
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
