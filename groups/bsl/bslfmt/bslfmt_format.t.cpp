// bslfmt_format.t.cpp                                                -*-C++-*-
#include <bslfmt_format.h>

#include <bslfmt_standardformatspecification.h>

#include <bslma_testallocator.h>

#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_string.h>
#include <bslstl_vector.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #include <locale>
#endif

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
  #include <type_traits> // For std::is_constant_evaluated
  #define u_IF_NOT_CONSTEXPR if (not std::is_constant_evaluated())
#else
  #define u_IF_NOT_CONSTEXPR
#endif

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test provides the functionality of the `bsl_format.h`
// header therefore it defines its names in the `bsl` namespace.  It has two
// major "modes of operation": when BDE has to implement the functionality, and
// when the standard library provides a "good enough" implementation for us to
// build on.  To simplify the test driver we do not differentiate between those
// two modes of implementation but use the same verification for both.
//
// This special component defines classes, class templates, and namespace-level
// function templates as well.  Because many of the classes are designed for
// internal use by the format "system" and provide very limited testability we
// will often only test the presence of the name, or a very simple use case
// that does not require elaborate testing harness.
//
// Due to the large number of types (and combinations) the functions may be
// called as formatted arguments (and class templates instantiated) the
// verification for most named entities is done in more than one test case,
// and those test cases verify large concerns, for example if all integer types
// are formatted as expected, arguments are forwarded as expected, format
// strings with syntax errors result in an exception etc.
// ----------------------------------------------------------------------------
// CLASSES AND CLASS TEMPLATES
//  - `bsl::basic_format_arg`
//  - `bsl::basic_format_args`
//  - `bsl::basic_format_context`
//  - `bsl::basic_format_parse_context`
//  - `bsl::basic_format_string`
//  - `bsl::formatter`
//  - `bsl::format_args`
//  - `bsl::format_error`
//  - `bsl::format_parse_context`
//  - `bsl::format_string`
//  - `bsl::format_to_n_result`
//  - `bsl::wformat_args`
//  - `bsl::wformat_parse_context`
//  - `bsl::wformat_string`
//
// FUNCTIONS AND FUNCTION TEMPLATES
//  - `bsl::formatted_size()`
//
//  - `bsl::format(fmtstr, ...)`
//  - `bsl::format(wfmtstr, ...)`
//  - `bsl::format(locale, fmtstr, ...)`
//  - `bsl::format(locale, wfmtstr, ...)`
//  - `bsl::format(allocator, fmtstr, ...)`
//  - `bsl::format(allocator, wfmtstr, ...)`
//  - `bsl::format(allocator, locale, fmtstr, ...)`
//  - `bsl::format(allocator, locale, wfmtstr, ...)`
//
//  - `bsl::format_to(bsl::string* str,  fmtstr, ...)`
//  - `bsl::format_to(bsl::wstring* str, wfmtstr, ...)`
//  - `bsl::format_to(bsl::string* str,  locale, fmtstr, ...)`
//  - `bsl::format_to(bsl::wstring* str, locale, wfmtstr, ...)`
//
//  - `bsl::format_to_n(outIter, n, fmtstr, ...)`
//  - `bsl::format_to_n(outIter, n, wfmtstr, ...)`
//  - `bsl::format_to_n(outIter, n, locale, fmtstr, ...)`
//  - `bsl::format_to_n(outIter, n, locale, wfmtstr, ...)`
//
//  - `bsl::vformat(fmtstr, ...)`
//  - `bsl::vformat(wfmtstr, ...)`
//  - `bsl::vformat(locale, fmtstr, ...)`
//  - `bsl::vformat(locale, wfmtstr, ...)`
//  - `bsl::vformat(allocator, fmtstr, ...)`
//  - `bsl::vformat(allocator, wfmtstr, ...)`
//  - `bsl::vformat(allocator, locale, fmtstr, ...)`
//  - `bsl::vformat(allocator, locale, wfmtstr, ...)`
//
//  - `bsl::vformat_to(bsl::string *str,  locale, fmtstr, ...)`
//  - `bsl::vformat_to(bsl::wstring *str, locale, wfmtstr, ...)`
//  - `bsl::vformat_to(bsl::string *str,  locale, fmtstr, ...)`
//  - `bsl::vformat_to(bsl::wstring *str, locale, wfmtstr, ...)`
//
//  - `bsl::make_format_args(...)`
//  - `bsl::make_wformat_args(...)`
//
//  - `bsl::visit_format_arg(visitor, arg)`
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] CONCERN: CLASSES AND CLASS TEMPLATES
// [ 3] CONCERN: INTEGERS
// [ 4] CONCERN: CHAR AND WCHAR_T
// [ 5] CONCERN: FLOAT AND DOUBLE
// [ 6] CONCERN: STRINGS
// [ 7] CONCERN: BOOLEAN
// [ 8] CONCERN: POINTERS
// [ 9] CONCERN: COMBINATIONS OF FORMATTED TYPES
// [10] CONCERN: VFORMAT, VFORMAT_TO
// [11] CONCERN: ARGUMENT PASSING
// [12] CONCERN: BAD FORMAT STRINGS
// [13] EXAMPLES IN BDE DOCUMENTATION
// [14] USAGE EXAMPLES
// ----------------------------------------------------------------------------

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
//                        TEST DRIVER LOCAL MACROS
// ----------------------------------------------------------------------------


#if defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) &&                               \
    (13 == _GLIBCXX_RELEASE || 14 == _GLIBCXX_RELEASE)
// We disable tests for certain pointer-formatting on gcc-13 and gcc-14 because
// those platforms do not (yet) support zero-padding of pointers, and the
// (uppercase) 'P' format specifier, but are in other ways preferable to the
// proprietary (C++ 03 supporting) implementation in `bslfmt`.
//
// GNU C++ supports an almost perfect std::format implementation starting with
// gcc-13.  Unfortunately it does not support zero padding of pointers and the
// attempt results in a compilation error (for compile-time format string
// evaluation) stating that "0 width cannot be used".  Even more unfortunately
// gcc-14 does not implement the feature either.  The 'P' format specifier is
// also unsupported (for uppercase "0X" and hex digits), it throws a
// `std::format_error` saying that "the format string could not be parsed".
// Instead of giving up using the platform-native formatting (and benefiting
// from compile time evaluation) we decided to not try to zero-pad pointers in
// our test drivers when we are on gcc version that don't support it.
  #define u_GCC_PTR_FMT_WORKAROUND                                            1
#endif  // GNU libstdc++ of gcc-13 or 14


// ============================================================================
//                       HELPER CLASSES AND FUNCTIONS
// ----------------------------------------------------------------------------

#if defined(BSLSTL_STRING_VIEW_AND_STD_STRING_VIEW_COEXIST) ||                \
    defined(BSLSTL_STRING_VIEW_IS_ALIASED)
  #define u_STD_STRING_VIEW_EXISTS                                            1
#endif

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

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

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

        static const char                   name[] = "FormattableType";
        typename t_FORMAT_CONTEXT::iterator out    = formatContext.out();

        out  = std::copy(name, name + strlen(name), out);
        *out++ = '{';
        formatContext.advance_to(out);
        out  = d_formatter_bsl.format(value.x, formatContext);
        *out++ = '}';
        return out;
    }
};

}  // close namespace bsl

// ============================================================================
//                              TEST MACHINERY
// ----------------------------------------------------------------------------

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #define u_TESTUTIL_ORACLE_TEST(fmtstr, expected, ...)
  #define u_TESTUTIL_ORACLE_WTEST(fmtstr, expected, ...)
#define u_TESTUTIL_ORACLE_NUMCALLS 0

  #define u_TESTUTIL_VORACLE_TEST(fmtstr, expected, ...)
  #define u_TESTUTIL_VORACLE_WTEST(fmtstr, expected, ...)

  #define u_TESTUTIL_VORACLE_NUMCALLS 0
#else  // ndef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #define u_TESTUTIL_ORACLE_TEST(fmtstr, expected, ...)                       \
    do {                                                                      \
        const std::string stdResult = std::format(fmtstr, __VA_ARGS__);       \
        ASSERTV(stdResult.c_str(), result.c_str(), stdResult == result);      \
                                                                              \
        const size_t stdFmtdSize = std::formatted_size(fmtstr, __VA_ARGS__);  \
        ASSERTV(stdFmtdSize, fmtdSize, stdFmtdSize == fmtdSize);              \
    } while (false)

  #define u_TESTUTIL_ORACLE_WTEST(fmtstr, expected, ...)                      \
    do {                                                                      \
        const std::wstring stdResult = std::format(fmtstr, __VA_ARGS__);      \
        ASSERT(stdResult == result);                                          \
                                                                              \
        const size_t stdFmtdSize = std::formatted_size(fmtstr, __VA_ARGS__);  \
        ASSERTV(stdFmtdSize, fmtdSize, stdFmtdSize == fmtdSize);              \
    } while (false)

#define u_TESTUTIL_ORACLE_NUMCALLS 2

  // `vformat`, `vformat_to`

  #define u_TESTUTIL_VORACLE_TEST(fmtstr, expected, ...)                      \
    do {                                                                      \
        const std::string stdResult = std::vformat(                           \
                                      fmtstr,                                 \
                                      std::make_format_args(__VA_ARGS__));    \
        ASSERTV(stdResult.c_str(), result.c_str(), stdResult == result);      \
                                                                              \
        char *end = std::vformat_to(buff,                                     \
                                    fmtstr,                                   \
                                    std::make_format_args(__VA_ARGS__));      \
        *end = 0;                                                             \
        ASSERTV(buff, result.c_str(), buff == result);                        \
    } while (false)

  #define u_TESTUTIL_VORACLE_WTEST(fmtstr, expected, ...)                     \
    do {                                                                      \
          const std::wstring stdResult = std::vformat(                        \
                                        fmtstr,                               \
                                        std::make_wformat_args(__VA_ARGS__)); \
          ASSERTV(stdResult == result);                                       \
                                                                              \
          wchar_t *end = std::vformat_to(                                     \
                                      buff,                                   \
                                      fmtstr,                                 \
                                      std::make_wformat_args(__VA_ARGS__));   \
          *end = 0;                                                           \
          ASSERTV(buff == result);                                            \
    } while (false)

  #define u_TESTUTIL_VORACLE_NUMCALLS 2

  #define u_TESTUTIL_ORACLE_LTEST(fmtstr, expected, ...)                      \
    do {                                                                      \
        const std::string stdResult = std::format(myLocale,                   \
                                                  fmtstr,                     \
                                                  __VA_ARGS__);               \
        ASSERTV(stdResult.c_str(), result.c_str(), stdResult == result);      \
                                                                              \
        const size_t stdFmtdSize = std::formatted_size(myLocale,              \
                                                       fmtstr,                \
                                                       __VA_ARGS__);          \
        ASSERTV(stdFmtdSize, fmtdSize, stdFmtdSize == fmtdSize);              \
                                                                              \
    } while (false)

#define u_TESTUTIL_ORACLE_WLTEST(fmtstr, expected, ...)                       \
    do {                                                                      \
        const std::wstring stdResult = std::format(myWlocale,                 \
                                                   fmtstr,                    \
                                                   __VA_ARGS__);              \
        ASSERT(stdResult == result);                                          \
                                                                              \
        const size_t stdFmtdSize = std::formatted_size(myWlocale,             \
                                                       fmtstr,                \
                                                       __VA_ARGS__);          \
        ASSERTV(stdFmtdSize, fmtdSize, stdFmtdSize == fmtdSize);              \
    } while (false)

#define u_TESTUTIL_VORACLE_LTEST(fmtstr, expected, ...)                       \
    do {                                                                      \
        const std::string stdResult = std::vformat(                           \
                                         myLocale,                            \
                                         fmtstr,                              \
                                         std::make_format_args(__VA_ARGS__)); \
        ASSERTV(stdResult.c_str(), result.c_str(), stdResult == result);      \
                                                                              \
        char *end = std::vformat_to(buff,                                     \
                                    myLocale,                                 \
                                    fmtstr,                                   \
                                    std::make_format_args(__VA_ARGS__));      \
        *end      = 0;                                                        \
        ASSERTV(buff, result.c_str(), buff == result);                        \
    } while (false)

#define u_TESTUTIL_VORACLE_WLTEST(fmtstr, expected, ...)                      \
    do {                                                                      \
        const std::wstring stdResult = std::vformat(                          \
                                        myWlocale,                            \
                                        fmtstr,                               \
                                        std::make_wformat_args(__VA_ARGS__)); \
        ASSERTV(stdResult == result);                                         \
                                                                              \
        wchar_t *end = std::vformat_to(buff,                                  \
                                       myWlocale,                             \
                                       fmtstr,                                \
                                       std::make_wformat_args(__VA_ARGS__));  \
        *end         = 0;                                                     \
        ASSERTV(buff == result);                                              \
    } while (false)
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

// TEST CALLS FOR FORMAT

#define u_VERIFY_FORMAT(fmtstr, expected, ...)                                \
    do {                                                                      \
        const bsl::string result = bsl::format(fmtstr, __VA_ARGS__);          \
        ASSERTV(result.c_str(), result == expected);                          \
                                                                              \
        const bsl::string resulta = bsl::format(&oa, fmtstr, __VA_ARGS__);    \
        ASSERTV(resulta.c_str(), result == expected);                         \
        ASSERT(resulta.get_allocator() == &oa);                               \
                                                                              \
        const size_t fmtdSize = bsl::formatted_size(fmtstr, __VA_ARGS__);     \
        ASSERTV(strlen(expected), fmtdSize, strlen(expected) == fmtdSize);    \
                                                                              \
        char buff[sizeof expected + 8];                                       \
                                                                              \
        char * const end = bsl::format_to(buff, fmtstr,  __VA_ARGS__);        \
        ASSERTV(end - buff, sizeof expected - 1,                              \
                end - buff == sizeof expected - 1);                           \
        *end = 0;                                                             \
        ASSERTV(buff, expected, 0 == strcmp(buff, expected));                 \
                                                                              \
        bsl::string toBslString;                                              \
        bsl::format_to(&toBslString, fmtstr, __VA_ARGS__);                    \
        ASSERTV(toBslString.size(),                                           \
                sizeof expected - 1,                                          \
                toBslString.size() == sizeof expected - 1);                   \
        ASSERTV(toBslString.c_str(), expected, toBslString == expected);      \
                                                                              \
        memset(buff, 0, sizeof buff);                                         \
        bsl::format_to_n_result<char *> f2nRes = bsl::format_to_n(buff,       \
                                                          sizeof expected,    \
                                                          fmtstr,             \
                                                          __VA_ARGS__);       \
        ASSERTV(f2nRes.size,                                                  \
                strlen(expected),                                             \
                f2nRes.size == strlen(expected));                             \
        *f2nRes.out = 0;                                                      \
        ASSERT(0 == strcmp(buff, expected));                                  \
                                                                              \
        memset(buff, 0, sizeof buff);                                         \
        f2nRes = bsl::format_to_n(buff,                                       \
                                  strlen(expected) - 1,                       \
                                  fmtstr,                                     \
                                  __VA_ARGS__);                               \
        ASSERTV(f2nRes.size,                                                  \
                strlen(expected),                                             \
                f2nRes.size == strlen(expected));                             \
        *f2nRes.out = 0;                                                      \
        ASSERT(0 == strncmp(buff, expected, strlen(expected) - 1));           \
                                                                              \
        u_TESTUTIL_ORACLE_TEST(fmtstr, expected, __VA_ARGS__);                \
    } while (false)

#define u_VERIFY_FORMAT_NUMCALLS (7 + u_TESTUTIL_ORACLE_NUMCALLS)


#define u_VERIFY_WFORMAT(fmtstr, expected, ...)                               \
    do {                                                                      \
        const bsl::wstring result = bsl::format(fmtstr, __VA_ARGS__);         \
        ASSERT(result == expected);                                           \
                                                                              \
        const bsl::wstring resulta = bsl::format(&oa, fmtstr, __VA_ARGS__);   \
        ASSERT(resulta == expected);                                          \
        ASSERT(resulta.get_allocator() == &oa);                               \
                                                                              \
        const size_t fmtdSize = bsl::formatted_size(fmtstr, __VA_ARGS__);     \
        ASSERTV(wcslen(expected), fmtdSize, wcslen(expected) == fmtdSize);    \
                                                                              \
        wchar_t buff[sizeof expected / sizeof(wchar_t) + 16];                 \
                                                                              \
        wchar_t * const end = bsl::format_to(buff, fmtstr,  __VA_ARGS__);     \
        ASSERTV(end - buff, wcslen(expected),                                 \
                static_cast<size_t>(end - buff) == wcslen(expected));         \
        *end = 0;                                                             \
        ASSERTV(buff, expected, 0 == wcscmp(buff, expected));                 \
                                                                              \
        bsl::wstring toBslString;                                             \
        bsl::format_to(&toBslString, fmtstr,  __VA_ARGS__);                   \
        ASSERTV(toBslString.size(),                                           \
                wcslen(expected),                                             \
                toBslString.size() == wcslen(expected));                      \
        ASSERT(toBslString == expected);                                      \
                                                                              \
        memset(buff, 0, sizeof buff);                                         \
        bsl::format_to_n_result<wchar_t *> f2nRes = bsl::format_to_n(         \
                                          buff,                               \
                                          sizeof expected / sizeof(wchar_t),  \
                                          fmtstr,                             \
                                          __VA_ARGS__);                       \
        ASSERTV(f2nRes.size, wcslen(expected),                                \
                static_cast<size_t>(f2nRes.size) == wcslen(expected));        \
        *f2nRes.out = 0;                                                      \
        ASSERT(0 == wcscmp(buff, expected));                                  \
                                                                              \
        memset(buff, 0, sizeof buff);                                         \
        f2nRes = bsl::format_to_n(buff,                                       \
                                  wcslen(expected) - 1,                       \
                                  fmtstr,                                     \
                                  __VA_ARGS__);                               \
        ASSERTV(f2nRes.size, wcslen(expected),                                \
                static_cast<size_t>(f2nRes.size) == wcslen(expected));        \
        *f2nRes.out = 0;                                                      \
        ASSERT(0 == wcsncmp(buff, expected, wcslen(expected) - 1));           \
                                                                              \
        u_TESTUTIL_ORACLE_WTEST(fmtstr, expected, __VA_ARGS__);               \
    } while (false)

#define u_VERIFY_WFORMAT_NUMCALLS u_VERIFY_FORMAT_NUMCALLS

#define u_VERIFY_FORMAT_BOTH(fmtstr, expected, ...)                           \
    do {                                                                      \
        u_VERIFY_FORMAT(fmtstr, expected, __VA_ARGS__);                       \
        u_VERIFY_WFORMAT(L##fmtstr, L##expected, __VA_ARGS__);                \
    } while (false)

#define u_VERIFY_FORMAT_BOTH_NUMCALLS                                         \
    (u_VERIFY_FORMAT_NUMCALLS + u_VERIFY_WFORMAT_NUMCALLS)


// `vformat`, `vformat_to`

#define u_VERIFY_VFORMAT(fmtstr, expected, ...)                               \
    do {                                                                      \
        const bsl::string result = bsl::vformat(                              \
                                        fmtstr,                               \
                                        bsl::make_format_args(__VA_ARGS__));  \
        ASSERTV(result.c_str(), result == expected);                          \
                                                                              \
        const bsl::string resulta = bsl::vformat(                             \
                                        &oa,                                  \
                                        fmtstr,                               \
                                        bsl::make_format_args(__VA_ARGS__));  \
        ASSERTV(resulta.c_str(), result == expected);                         \
        ASSERT(resulta.get_allocator() == &oa);                               \
                                                                              \
        bsl::string buffStr;                                                  \
        buffStr.resize(result.size() + 8);                                    \
        char *buff = &buffStr[0];                                             \
                                                                              \
        char * const end = bsl::vformat_to(                                   \
                                        buff,                                 \
                                        fmtstr,                               \
                                        bsl::make_format_args(__VA_ARGS__));  \
        ASSERTV(end - buff,                                                   \
                sizeof expected - 1,                                          \
                end - buff == sizeof expected - 1);                           \
        *end = 0;                                                             \
        ASSERTV(buff, expected, 0 == strcmp(buff, expected));                 \
                                                                              \
        bsl::string toBslStrResult;                                           \
        bsl::vformat_to(&toBslStrResult,                                      \
                        fmtstr,                                               \
                        bsl::make_format_args(__VA_ARGS__));                  \
        ASSERTV(toBslStrResult.size(),                                        \
                sizeof expected - 1,                                          \
                toBslStrResult.size() == sizeof expected - 1);                \
        ASSERTV(toBslStrResult.c_str(), expected,                             \
                toBslStrResult == expected);                                  \
                                                                              \
        u_TESTUTIL_VORACLE_TEST(fmtstr, expected, __VA_ARGS__);               \
    } while (false)
#define u_VERIFY_VFORMAT_NUMCALLS (4 + u_TESTUTIL_VORACLE_NUMCALLS)

#define u_VERIFY_WVFORMAT(fmtstr, expected, ...)                              \
    do {                                                                      \
        const bsl::wstring result = bsl::vformat(                             \
                                       fmtstr,                                \
                                       bsl::make_wformat_args(__VA_ARGS__));  \
        ASSERT(result == expected);                                           \
                                                                              \
        const bsl::wstring resulta = bsl::vformat(                            \
                                       &oa,                                   \
                                       fmtstr,                                \
                                       bsl::make_wformat_args(__VA_ARGS__));  \
        ASSERT(resulta == expected);                                          \
        ASSERT(resulta.get_allocator() == &oa);                               \
                                                                              \
        bsl::wstring buffStr;                                                 \
        buffStr.resize(result.size() + 8);                                    \
        wchar_t *buff = &buffStr[0];                                          \
                                                                              \
        wchar_t * const end = bsl::vformat_to(                                \
                                       buff,                                  \
                                       fmtstr,                                \
                                       bsl::make_wformat_args(__VA_ARGS__));  \
        ASSERTV(end - buff,                                                   \
                wcslen(expected),                                             \
                static_cast<size_t>(end - buff) == wcslen(expected));         \
        *end = 0;                                                             \
        ASSERTV(buff, expected, 0 == wcscmp(buff, expected));                 \
                                                                              \
        bsl::wstring toBslStrResult;                                          \
        bsl::vformat_to(&toBslStrResult,                                      \
                        fmtstr,                                               \
                        bsl::make_wformat_args(__VA_ARGS__));                 \
        ASSERTV(toBslStrResult.size(),                                        \
                wcslen(expected),                                             \
                toBslStrResult.size() == wcslen(expected));                   \
        ASSERT(toBslStrResult == expected);                                   \
                                                                              \
        u_TESTUTIL_VORACLE_WTEST(fmtstr, expected, __VA_ARGS__);              \
    } while (false)

#define u_VERIFY_WVFORMAT_NUMCALLS u_VERIFY_VFORMAT_NUMCALLS

#define u_VERIFY_VFORMAT_BOTH(fmtstr, expected, ...)                          \
    do {                                                                      \
        u_VERIFY_VFORMAT(fmtstr, expected, __VA_ARGS__);                      \
        u_VERIFY_WVFORMAT(L##fmtstr, L##expected, __VA_ARGS__);               \
    } while (false)

#define u_VERIFY_VFORMAT_BOTH_NUMCALLS                                        \
    (u_VERIFY_VFORMAT_NUMCALLS + u_VERIFY_WVFORMAT_NUMCALLS)


#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
#define u_VERIFY_LFORMAT(fmtstr, expected, ...)                               \
    do {                                                                      \
        const bsl::string result = bsl::format(myLocale,                      \
                                               fmtstr,                        \
                                               __VA_ARGS__);                  \
        ASSERTV(result.c_str(), result == expected);                          \
                                                                              \
        const bsl::string resulta =                                           \
                             bsl::format(&oa, myLocale, fmtstr, __VA_ARGS__); \
        ASSERTV(resulta.c_str(), result == expected);                         \
        ASSERT(resulta.get_allocator() == &oa);                               \
                                                                              \
        const size_t fmtdSize = bsl::formatted_size(myLocale,                 \
                                                    fmtstr,                   \
                                                    __VA_ARGS__);             \
        ASSERTV(strlen(expected), fmtdSize, strlen(expected) == fmtdSize);    \
                                                                              \
        char buff[sizeof expected + 8];                                       \
                                                                              \
        char *const end =                                                     \
                         bsl::format_to(buff, myLocale, fmtstr, __VA_ARGS__); \
        ASSERTV(end - buff, sizeof expected - 1,                              \
                end - buff == sizeof expected - 1);                           \
        *end = 0;                                                             \
        ASSERTV(buff, expected, 0 == strcmp(buff, expected));                 \
                                                                              \
        memset(buff, 0, sizeof buff);                                         \
        bsl::format_to_n_result<char *> f2nRes = bsl::format_to_n(            \
                                                             buff,            \
                                                             sizeof expected, \
                                                             myLocale,        \
                                                             fmtstr,          \
                                                             __VA_ARGS__);    \
        ASSERTV(f2nRes.size,                                                  \
                strlen(expected),                                             \
                f2nRes.size == strlen(expected));                             \
        *f2nRes.out = 0;                                                      \
        ASSERT(0 == strcmp(buff, expected));                                  \
                                                                              \
        memset(buff, 0, sizeof buff);                                         \
        f2nRes = bsl::format_to_n(buff,                                       \
                                  strlen(expected) - 1,                       \
                                  myLocale,                                   \
                                  fmtstr,                                     \
                                  __VA_ARGS__);                               \
        ASSERTV(f2nRes.size,                                                  \
                strlen(expected),                                             \
                f2nRes.size == strlen(expected));                             \
        *f2nRes.out = 0;                                                      \
        ASSERT(0 == strncmp(buff, expected, strlen(expected) - 1));           \
                                                                              \
        bsl::string toBslStrResult;                                           \
        bsl::format_to(&toBslStrResult, myLocale, fmtstr, __VA_ARGS__);       \
        ASSERTV(toBslStrResult.size(),                                        \
                sizeof expected - 1,                                          \
                toBslStrResult.size() == sizeof expected - 1);                \
        ASSERTV(toBslStrResult.c_str(),                                       \
                expected,                                                     \
                toBslStrResult == expected);                                  \
                                                                              \
        u_TESTUTIL_ORACLE_LTEST(fmtstr, expected, __VA_ARGS__);               \
    } while (false)

#define u_VERIFY_WLFORMAT(fmtstr, expected, ...)                              \
    do {                                                                      \
        const bsl::wstring result = bsl::format(myWlocale,                    \
                                                fmtstr,                       \
                                                __VA_ARGS__);                 \
        ASSERT(result == expected);                                           \
                                                                              \
        const bsl::wstring resulta =                                          \
                            bsl::format(&oa, myWlocale, fmtstr, __VA_ARGS__); \
        ASSERT(resulta == expected);                                          \
        ASSERT(resulta.get_allocator() == &oa);                               \
                                                                              \
        const size_t fmtdSize = bsl::formatted_size(myWlocale,                \
                                                    fmtstr,                   \
                                                    __VA_ARGS__);             \
        ASSERTV(wcslen(expected), fmtdSize, wcslen(expected) == fmtdSize);    \
                                                                              \
        wchar_t buff[sizeof expected / sizeof(wchar_t) + 16];                 \
                                                                              \
        wchar_t *const end =                                                  \
                        bsl::format_to(buff, myWlocale, fmtstr, __VA_ARGS__); \
        ASSERTV(end - buff,                                                   \
                wcslen(expected),                                             \
                static_cast<size_t>(end - buff) == wcslen(expected));         \
        *end = 0;                                                             \
        ASSERTV(buff, expected, 0 == wcscmp(buff, expected));                 \
                                                                              \
        bsl::wstring toBslStrResult;                                          \
        bsl::format_to(&toBslStrResult, myWlocale, fmtstr, __VA_ARGS__);      \
        ASSERTV(toBslStrResult.size(),                                        \
                wcslen(expected),                                             \
                toBslStrResult.size() == wcslen(expected));                   \
        ASSERT(toBslStrResult == expected);                                   \
                                                                              \
        memset(buff, 0, sizeof buff);                                         \
        bsl::format_to_n_result<wchar_t *> f2nRes = bsl::format_to_n(         \
                                           buff,                              \
                                           sizeof expected / sizeof(wchar_t), \
                                           myWlocale,                         \
                                           fmtstr,                            \
                                           __VA_ARGS__);                      \
        ASSERTV(f2nRes.size,                                                  \
                wcslen(expected),                                             \
                static_cast<size_t>(f2nRes.size) == wcslen(expected));        \
        *f2nRes.out = 0;                                                      \
        ASSERT(0 == wcscmp(buff, expected));                                  \
                                                                              \
        memset(buff, 0, sizeof buff);                                         \
        f2nRes = bsl::format_to_n(buff,                                       \
                                  wcslen(expected) - 1,                       \
                                  myWlocale,                                  \
                                  fmtstr,                                     \
                                  __VA_ARGS__);                               \
        ASSERTV(f2nRes.size,                                                  \
                wcslen(expected),                                             \
                static_cast<size_t>(f2nRes.size) == wcslen(expected));        \
        *f2nRes.out = 0;                                                      \
        ASSERT(0 == wcsncmp(buff, expected, wcslen(expected) - 1));           \
                                                                              \
        u_TESTUTIL_ORACLE_WLTEST(fmtstr, expected, __VA_ARGS__);              \
    } while (false)

#define u_VERIFY_LFORMAT_BOTH(fmtstr, expected, ...)                          \
    do {                                                                      \
        u_VERIFY_LFORMAT(fmtstr, expected, __VA_ARGS__);                      \
        u_VERIFY_WLFORMAT(L##fmtstr, L##expected, __VA_ARGS__);               \
    } while (false)

// `vformat`, `vformat_to`

#define u_VERIFY_VLFORMAT(fmtstr, expected, ...)                              \
    do {                                                                      \
        const bsl::string result = bsl::vformat(                              \
                                         myLocale,                            \
                                         fmtstr,                              \
                                         bsl::make_format_args(__VA_ARGS__)); \
        ASSERTV(result.c_str(), result == expected);                          \
                                                                              \
        const bsl::string resulta = bsl::vformat(                             \
                                         &oa,                                 \
                                         myLocale,                            \
                                         fmtstr,                              \
                                         bsl::make_format_args(__VA_ARGS__)); \
        ASSERTV(resulta.c_str(), result == expected);                         \
        ASSERT(resulta.get_allocator() == &oa);                               \
                                                                              \
        bsl::string buffStr;                                                  \
        buffStr.resize(result.size() + 8);                                    \
        char *buff = &buffStr[0];                                             \
                                                                              \
        char *const end = bsl::vformat_to(                                    \
                                         buff,                                \
                                         myLocale,                            \
                                         fmtstr,                              \
                                         bsl::make_format_args(__VA_ARGS__)); \
        ASSERTV(end - buff,                                                   \
                sizeof expected - 1,                                          \
                end - buff == sizeof expected - 1);                           \
        *end = 0;                                                             \
        ASSERTV(buff, expected, 0 == strcmp(buff, expected));                 \
                                                                              \
        bsl::string toBslStrResult;                                           \
        bsl::vformat_to(&toBslStrResult,                                      \
                        myLocale,                                             \
                        fmtstr,                                               \
                        bsl::make_format_args(__VA_ARGS__));                  \
        ASSERTV(toBslStrResult.size(),                                        \
                sizeof expected - 1,                                          \
                toBslStrResult.size() == sizeof expected - 1);                \
        ASSERTV(toBslStrResult.c_str(),                                       \
                expected,                                                     \
                toBslStrResult == expected);                                  \
                                                                              \
        u_TESTUTIL_VORACLE_LTEST(fmtstr, expected, __VA_ARGS__);              \
    } while (false)

#define u_VERIFY_WVLFORMAT(fmtstr, expected, ...)                             \
    do {                                                                      \
        const bsl::wstring result = bsl::vformat(                             \
                                        myWlocale,                            \
                                        fmtstr,                               \
                                        bsl::make_wformat_args(__VA_ARGS__)); \
        ASSERT(result == expected);                                           \
                                                                              \
        const bsl::wstring resulta = bsl::vformat(                            \
                                        &oa,                                  \
                                        myWlocale,                            \
                                        fmtstr,                               \
                                        bsl::make_wformat_args(__VA_ARGS__)); \
        ASSERT(resulta == expected);                                          \
        ASSERT(resulta.get_allocator() == &oa);                               \
                                                                              \
        bsl::wstring buffStr;                                                 \
        buffStr.resize(result.size() + 8);                                    \
        wchar_t *buff = &buffStr[0];                                          \
                                                                              \
        wchar_t *const end = bsl::vformat_to(                                 \
                                        buff,                                 \
                                        myWlocale,                            \
                                        fmtstr,                               \
                                        bsl::make_wformat_args(__VA_ARGS__)); \
        ASSERTV(end - buff,                                                   \
                wcslen(expected),                                             \
                static_cast<size_t>(end - buff) == wcslen(expected));         \
        *end = 0;                                                             \
        ASSERTV(buff, expected, 0 == wcscmp(buff, expected));                 \
                                                                              \
        bsl::wstring toBslStrResult;                                          \
        bsl::vformat_to(&toBslStrResult,                                      \
                        myWlocale,                                            \
                        fmtstr,                                               \
                        bsl::make_wformat_args(__VA_ARGS__));                 \
        ASSERTV(toBslStrResult.size(),                                        \
                wcslen(expected),                                             \
                toBslStrResult.size() == wcslen(expected));                   \
        ASSERT(toBslStrResult == expected);                                   \
                                                                              \
        u_TESTUTIL_VORACLE_WLTEST(fmtstr, expected, __VA_ARGS__);             \
    } while (false)

#define u_VERIFY_VLFORMAT_BOTH(fmtstr, expected, ...)                         \
    do {                                                                      \
        u_VERIFY_VLFORMAT(fmtstr, expected, __VA_ARGS__);                     \
        u_VERIFY_WVLFORMAT(L##fmtstr, L##expected, __VA_ARGS__);              \
    } while (false)
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

// ============================================================================
//                      BASIC_FORMAT_ARG[S] TEST HELPERS
// ----------------------------------------------------------------------------

// There has to be two of these functions because if I make it into a template
// the lookup fails.

void verifyArgAndArgs(int LINE, const bsl::format_args& fargs)
{
    // Here we get to use the `basic_format_arg` name
    bsl::basic_format_arg<bsl::format_context> fa = fargs.get(0);
    ASSERTV(LINE, !!fa);

    ASSERTV(LINE, !!fargs.get(0)); // The only usable member is `operator bool`
    ASSERTV(LINE, !!fargs.get(1)); // in `basic_format_arg`
    ASSERTV(LINE, !fargs.get(2));
}

void verifyArgAndArgs(int LINE, const bsl::wformat_args& fargs)
{
    // Here we get to use the `basic_format_arg` name
    bsl::basic_format_arg<bsl::wformat_context> fa = fargs.get(0);
    ASSERTV(LINE, !!fa);

    ASSERTV(LINE, !!fargs.get(0)); // The only usable member is `operator bool`
    ASSERTV(LINE, !!fargs.get(1)); // in `basic_format_arg`
    ASSERTV(LINE, !fargs.get(2));
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Custom Formatter For User Defined Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a custom type representing a date and we want to output it
// to the stream in different formats depending on the circumstances using
// `bsl::format` function.  The following example demonstrates how such custom
// formatter may be implemented.
//
// First, we define our `Date` class:
// ```
    /// This class implements a complex-constrained, value-semantic type for
    /// representing dates.  Each object of this class *always* represents a
    /// *valid* date value in the range `[0001JAN01 .. 9999DEC31]` inclusive.
    class Date {
      private:
        // DATA
        int d_year;   // year
        int d_month;  // month
        int d_day;    // day

      public:
        // CREATORS

        /// Create an object having the value represented by the specified
        /// `year`, `month`, and `day`.
        Date(int year, int month, int day)
        : d_year(year)
        , d_month(month)
        , d_day(day)
        {
            ASSERT((1 <= year)  && (9999 >= year));
            ASSERT((1 <= month) && (12 >= month));
            ASSERT((1 <= day)   && (31 >= day));
        }

        // ACCESSORS

        /// Return the year of this date.
        int year() const { return d_year; }

        /// Return the month of this date.
        int month() const { return d_month; }

        /// Return the day of this date.
        int day() const { return d_day; }
    };
// ```
// Then, we define our custom formatter for this date class.  In it, two
// methods are necessary: `parse()` and `format()`.  The `parse` method parses
// the format string itself to determine the formatting to be used by the
// `format` method, which writes the formatted date into a string.  Both
// methods are required to conform to a specific interface.
// ```
    /// This struct is a base class for `bsl::formatter` specializations for
    /// the `Date` class.
    template <class t_CHAR>
    struct DateFormatter {
      private:
// ```
// The convenience of using the `bsl::format` function is that the users can
// come up with the description language themselves.  In our case, for
// simplicity, we will display the date in two formats - numeric (`1999-10-23`)
// and verbal (`23 October 1999`).  Accordingly, to indicate the desired type,
// we will use one of two letters in the format description: 'n' ('N') or 'v'
// ('V').  And one field is enough for us to store it.
// ```
        // PRIVATE TYPES
        enum Format {
            e_NUMERIC,  // 1999-10-23
            e_VERBAL    // 23 October 1999
        };

        // DATA
        Format d_format;  // output format

        // PRIVATE ACCESSORS

        /// Output the specified `yearValue` to the specified `outIterator`.
        /// The specified `paddingRequired` indicates whether additional
        /// characters need to be added to fill empty space.
        template <class t_FORMAT_CONTEXT>
        void outputYear(
                    typename t_FORMAT_CONTEXT::iterator& outIterator,
                    int                                  yearValue,
                    bool                                 paddingRequired) const
        {
            typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;

            char  buffer[4];
            char *bufferEnd = NFUtil::toChars(buffer, buffer + 4, yearValue);

            if (paddingRequired) {
                const char *paddingStr = "000";
                size_t      numPaddingCharacters = 0;

                if (1000 > yearValue) {
                    ++numPaddingCharacters;
                    if (100 > yearValue) {
                        ++numPaddingCharacters;
                        if (10 > yearValue) {
                            ++numPaddingCharacters;
                        }
                    }
                }

                if (numPaddingCharacters) {
                    outIterator = BloombergLP::bslfmt::FormatterCharUtil<
                        t_CHAR>::outputFromChar(paddingStr,
                                                paddingStr +
                                                    numPaddingCharacters,
                                                outIterator);
                }
            }
            outIterator =
                BloombergLP::bslfmt::FormatterCharUtil<t_CHAR>::outputFromChar(
                    buffer,
                    bufferEnd,
                    outIterator);
        }

        /// Output the specified `monthOrDayValue` to the specified
        /// `outIterator`.  The specified `paddingRequired` indicates whether
        /// an additional character needs to be added to fill empty space.
        template <class t_FORMAT_CONTEXT>
        void outputMonthDay(
                    typename t_FORMAT_CONTEXT::iterator& outIterator,
                    int                                  monthOrDayValue,
                    bool                                 paddingRequired) const
        {
            typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;

            char  buffer[2];
            char *bufferEnd = NFUtil::toChars(buffer,
                                              buffer + 2,
                                              monthOrDayValue);

            if (paddingRequired) {
                if (10 > monthOrDayValue) {
                    outIterator = BloombergLP::bslfmt::FormatterCharUtil<
                        t_CHAR>::outputFromChar('0', outIterator);
                }
            }
            outIterator = BloombergLP::bslfmt::FormatterCharUtil<
                t_CHAR>::outputFromChar(buffer, bufferEnd, outIterator);
        }
// ```
// Notice that if the standard implementation of the format is supported by
// your compiler, then the `parse` function as well as the constructor must be
// declared as `constexpr`.
// ```
      public:
        // CREATORS

        /// Create a formatter that outputs values in the `e_NUMERIC` format.
        /// Thus, numeric is the default format for the `Date` object.
        BSLS_KEYWORD_CONSTEXPR_CPP20 DateFormatter()
        : d_format(e_NUMERIC)
        {
        }

        // MANIPULATORS

        /// Parse the specified `context` and return end iterator of parsed
        /// range.
        template <class t_PARSE_CONTEXT>
        BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                      t_PARSE_CONTEXT& context)
        {
            typedef typename bsl::iterator_traits<
                typename t_PARSE_CONTEXT::const_iterator>::value_type
                IteratorValueType;
            BSLMF_ASSERT((bsl::is_same<IteratorValueType, t_CHAR>::value));

            typename t_PARSE_CONTEXT::const_iterator current = context.begin();
            typename t_PARSE_CONTEXT::const_iterator end     = context.end();
// ```
// `bsl::format` calls `parse()` function first and here we can configure our
// formatter so that it then outputs values in the format we need.
// `context.begin()` returns an iterator pointing to the end of the parsed
// range.
// ```
            // Handling empty string or empty specification
            if (current == end || *current == '}') {
                return context.begin();                               // RETURN
            }

            // Reading format specification
            switch (*current) {
                case 'V':
                case 'v': {
                  d_format = e_VERBAL;
                } break;
                case 'N':
                case 'n': {
                  // `e_NUMERIC` value is assigned at object construction
                } break;
                default: {
                  BSLS_THROW(bsl::format_error(
                       "Unexpected symbol in format specification"));  // THROW
                }
            }

            // Move the iterator to the next position and check that there are
            // no extra characters in the description.

            ++current;

            if (current != end && *current != '}') {
                BSLS_THROW(bsl::format_error(
                        "Too many symbols in format specification"));  // THROW
            }

            context.advance_to(current);
            return context.begin();
        }

        // ACCESSORS

        /// Create string representation of the specified `value`, customized
        /// in accordance with the requested format and the specified
        /// `context`, and copy it to the output that the output iterator of
        /// the `context` points to.
        template <class t_FORMAT_CONTEXT>
        typename t_FORMAT_CONTEXT::iterator format(
                                               Date              value,
                                               t_FORMAT_CONTEXT& context) const
        {
            typename t_FORMAT_CONTEXT::iterator outIterator = context.out();
// ```
// Next, we outputting the date in accordance with the previously set settings:
// ```
            if (e_VERBAL == d_format) {  // 23 October 1999
                static const char *const months[] = {"January",
                                                     "February",
                                                     "March",
                                                     "April",
                                                     "May",
                                                     "June",
                                                     "July",
                                                     "August",
                                                     "September",
                                                     "October",
                                                     "November",
                                                     "December"};

                // Outputting day
                outputMonthDay<t_FORMAT_CONTEXT>(outIterator,
                                                 value.day(),
                                                 false);
                outIterator = BloombergLP::bslfmt::FormatterCharUtil<
                    t_CHAR>::outputFromChar(' ', outIterator);

                // Outputting month
                const char *month = months[value.month() - 1];
                outIterator  = BloombergLP::bslfmt::FormatterCharUtil<
                    t_CHAR>::outputFromChar(month,
                                            month + std::strlen(month),
                                            outIterator);
                outIterator = BloombergLP::bslfmt::FormatterCharUtil<
                    t_CHAR>::outputFromChar(' ', outIterator);

                // Outputting year
                outputYear<t_FORMAT_CONTEXT>(outIterator, value.year(), false);
            }
            else if (e_NUMERIC == d_format) {  // 1999-10-23
                // Outputting year
                outputYear<t_FORMAT_CONTEXT>(outIterator, value.year(), true);
                outIterator = BloombergLP::bslfmt::FormatterCharUtil<
                    t_CHAR>::outputFromChar('-', outIterator);

                // Outputting month
                outputMonthDay<t_FORMAT_CONTEXT>(outIterator,
                                                 value.month(),
                                                 true);
                outIterator = BloombergLP::bslfmt::FormatterCharUtil<
                    t_CHAR>::outputFromChar('-', outIterator);

                // Outputting day
                outputMonthDay<t_FORMAT_CONTEXT>(outIterator,
                                                 value.day(),
                                                 true);
            }

            return outIterator;
        }
    };
// ```
// Now, we define the `bsl::formatter` specialization for our `Date` class
// simply as a child-class of `DateFormatter`. Alternatively, we could have
// placed the implementation directly into the `bsl::formatter` specialization.
// Notice that the specialization must be defined in the `bsl` namespace.
// ```
    namespace bsl {

    template <class t_CHAR>
    struct formatter<Date, t_CHAR> : DateFormatter<t_CHAR> {
    };

    }  // close namespace bsl
// ```

// ============================================================================
//                               WEBSITE EXAMPLES
// ----------------------------------------------------------------------------

// The following classes are used in the usage examples on the BDE web page
// about `bsl::format`.  We want to make sure that these examples compile and
// produce the expected output.
// Full link to the `bsl::format` page:
// https://bde.bloomberg.com/bde/articles/bslfmt.html

class MyType {
};

namespace bsl {
template <class t_CHAR>
struct formatter<MyType, t_CHAR> {
  public:
    template <class t_PARSE_CONTEXT>
    typename t_PARSE_CONTEXT::iterator BSLS_KEYWORD_CONSTEXPR_CPP20 parse(
                                                              t_PARSE_CONTEXT&)
    {
        // TODO
    }
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(const MyType&,
                                               t_FORMAT_CONTEXT&) const
    {
        // TODO
    }
};
}  // close namespace bsl

struct Complex {
    double d_real;
    double d_imaginary;
};

namespace bsl {  // Formatter needs to be in namespace `bsl`
/// Partial specialization of `bsl::formatter` for type `Complex`
template <class t_CHAR>
struct formatter<Complex, t_CHAR> {
  private:
    formatter<double, t_CHAR> d_doubleFormatter;
        // Use a standard formatter to which we delegate floating point
        // formatting

    bool                      d_isJ;
        // Whether the imaginary part uses an 'i' or a 'j'

  public:
    // CREATORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 formatter()
    : d_doubleFormatter()
    , d_isJ (false)
    {}

    // MANIPULATORS
    template <class t_PARSE_CONTEXT>
    typename t_PARSE_CONTEXT::iterator BSLS_KEYWORD_CONSTEXPR_CPP20 parse(
                                                           t_PARSE_CONTEXT& pc)
    {
        if (pc.begin() == pc.end() || *pc.begin() == '}') {
            // Initialize the standard floating point formatter If a
            // default (empty) format specification is provided.
            return d_doubleFormatter.parse(pc);
        }
        if (*pc.begin() == 'i' || *pc.begin() == 'j') {
            // The user has specified 'i' vs 'j' for output
            if (*pc.begin() == 'j') {
                d_isJ = true;
            }
            pc.advance_to(pc.begin() + 1);  // skip 'i' or 'I'
        }
        if (pc.begin() != pc.end()) {
            // Initialize the format specification with the provided
            // floating point format
            if (*pc.begin() != ':') {
                BSLS_THROW(format_error("missing separator"));
            }
            pc.advance_to(pc.begin() + 1);  // skip ':'
        }
        // Update the iterator in the context and return the same.
        pc.advance_to(d_doubleFormatter.parse(pc));
        return pc.begin();
    };

    // ACCESSORS
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(const Complex&    value,
                                               t_FORMAT_CONTEXT& fc) const
    {
        typename t_FORMAT_CONTEXT::iterator out = fc.out();
        // Write out the first floating point value
        out = d_doubleFormatter.format(value.d_real, fc);
        // Write out the separator
        *out++ = (t_CHAR)'+';
            // Update the iterator in the context before passing it to the
            // floating point formatter again.
        fc.advance_to(out);
        // Write out the second floating point value
        out = d_doubleFormatter.format(value.d_imaginary, fc);
        // Write out 'i' or 'j' for the imaginary part
        *out++ = d_isJ ? (t_CHAR)'j' : (t_CHAR)'i';
            // Update the iterator in the context and return the same.
        fc.advance_to(out);
        return out;
    }
};
}  // close namespace bsl

//=============================================================================
//                          GLOBAL VERBOSITIES
//-----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                        TEST HELPERS FOR VFORMAT
//-----------------------------------------------------------------------------

//-------------------------------------------------------------------
// Global variables that collect usage information for `DelimitedInt`
static int                      g_numCtorInvocations;
static int                      g_numParseInvocations;
static int                      g_numFormatInvocations;
static bsl::vector<int>         g_values;

// Contexts have to be collected separately for `char` and `wchar_t`
template <class t_CHAR>
struct ParseContextContents;

template <>
struct ParseContextContents<char>{
    typedef bsl::string             value_type;
    static bsl::vector<bsl::string> s_vector;
};
bsl::vector<bsl::string> ParseContextContents<char>::s_vector;

template <>
struct ParseContextContents<wchar_t> {
    typedef bsl::wstring             value_type;
    static bsl::vector<bsl::wstring> s_vector;
};
bsl::vector<bsl::wstring> ParseContextContents<wchar_t>::s_vector;

//--------------------------------------------------------------
// Access and reset global "spying" variables for `DelimitedInt`
static void resetDelimitedIntGlobals()
{
    g_numCtorInvocations = 0;
    g_numParseInvocations = 0;
    g_numFormatInvocations = 0;
    g_values.clear();
    ParseContextContents<char>::s_vector.clear();
    ParseContextContents<wchar_t>::s_vector.clear();
}

static void printDelimitedIntGlobals()
{
    P(g_numCtorInvocations);
    P(g_numParseInvocations);
    P(g_numFormatInvocations);
    printf("g_values[%d]: { ", static_cast<int>(g_values.size()));
    for (size_t i = 0; i < g_values.size(); ++i) {
        printf("%d ", g_values[i]);
    }
    puts("}");
    printf("ParseContextContents<char>[%d]: { ",
           static_cast<int>(ParseContextContents<char>::s_vector.size()));
    for (size_t i = 0; i < ParseContextContents<char>::s_vector.size(); ++i) {
        printf("\"%s\" ",
               ParseContextContents<char>::s_vector[i].c_str());
    }
    puts("}");
    printf("ParseContextContents<wchar_t>[%d]: { ",
           static_cast<int>(ParseContextContents<wchar_t>::s_vector.size()));
    for (size_t i = 0; i < ParseContextContents<wchar_t>::s_vector.size(); ++i) {
        wprintf(L"\"%s\" ",
                ParseContextContents<wchar_t>::s_vector[i].c_str());
    }
    puts("}");
}

                              //===================
                              // class DelimitedInt
                              //===================

/// Represents an integer that is special-formatted between delimiters.
class DelimitedInt {
    // DATA
    int d_value;

  public:
    // CREATORS
    DelimitedInt(int value) : d_value(value) {}

    // ACCESSORS
    int value() const { return d_value; }
};


                  //======================================
                  // template struct DelimitedIntFormatter
                  //======================================

/// Custom formatter class template that formats a `DelimitedInt`, integer
/// value as text between two delimiters that default to "<>" such as: "<42>".
/// The delimiters may be changed by the format string that may be an empty set
/// of curly braces, or it may contain the ':' for formatting information
/// followed by two exactly characters for the opening and closing delimiter,
/// followed by the closing '}'.
template <class t_CHAR>
struct DelimitedIntFormatter {
  // DATA
  bool   d_isDefaultDelimiter;
  t_CHAR d_delimiters[2];

  // CLASS DATA
  static const t_CHAR s_defaultDelimiters[2];

  public:
    // CREATORS

    /// Create an `IntFormatter` and increment `g_numCtorInvocations`.
    BSLS_KEYWORD_CONSTEXPR_CPP20 DelimitedIntFormatter()
    : d_isDefaultDelimiter(true)
    {
        u_IF_NOT_CONSTEXPR {
            ++g_numCtorInvocations;
        }

        d_delimiters[0] = '<';
        d_delimiters[1] = '>';
    }

    // MANIPULATORS

    /// Parse a format string from the specified `context` and return an
    /// iterator pointing one past the last character parsed, or in other words
    /// the first character not yet parsed.  Also increment
    /// `g_numParseInvocations` (once) and append the complete format-string
    /// content of the parsed context to `g_parseContextContents` (as a
    /// string).  Note that we are not just adding the parsed region of the
    /// format string but *all* of it.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                      t_PARSE_CONTEXT& context)
    {
        u_IF_NOT_CONSTEXPR {
            ++g_numParseInvocations;
        }

        typename t_PARSE_CONTEXT::const_iterator current = context.begin();
        typename t_PARSE_CONTEXT::const_iterator end     = context.end();
        typename t_PARSE_CONTEXT::const_iterator temp    = context.begin();

        u_IF_NOT_CONSTEXPR {
            ParseContextContents<t_CHAR>::s_vector.push_back(
                          typename ParseContextContents<t_CHAR>::value_type());
            while (temp != end) {
                ParseContextContents<t_CHAR>::s_vector.back().push_back(*temp);
                ++temp;
            }
        }

        if (end != current && '}' != *current) {
            // There is a delimiter specified
            d_delimiters[0] = *current;

            ++current;
            if (end == current || '}' == *current) {
                throw bsl::format_error("Two delimiters required");    // THROW
            }
            d_delimiters[1] = *current;

            ++current;  // Move off of the second delimiter
        }
        if (end != current  && '}' != *current) {
            throw bsl::format_error("Extra characters in format");     // THROW
        }

        context.advance_to(current);
        return context.begin();
    }

    // ACCESSORS

    /// Create string representation of the specified `value`, customized in
    /// accordance with the requested format and the specified `context`, and
    /// copy it to the output that the output iterator of the `context` points
    /// to.  Also increment `g_numFormatInvocations` and add the printed
    /// integer value to the end of `g_values`.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                             const DelimitedInt& value,
                                             t_FORMAT_CONTEXT&   context) const
    {
        ++g_numFormatInvocations;
        typename t_FORMAT_CONTEXT::iterator outIterator = context.out();
        g_values.push_back(value.value());

        *outIterator++ = d_delimiters[0];

        long divider = (  sizeof(int) == 4
                        ? 1000000000
                        : static_cast<long>(1000000000000000000));
            // INT_MAX 9,223,372,036,854,775,807 on 64-bit INT_MAX
            // 2,147,483,647 on 32-bit

        int output = value.value();
        if (output < 0) {
            *outIterator = '-';
            ++outIterator;
            output = -output;
        }
        bool printed = false;
        while (divider > 0) {
            int digit = static_cast<int>(output / divider);
            if (printed || digit != 0) {
                printed      = true;
                *outIterator = '0' + static_cast<char>(digit);
                ++outIterator;
            }
            output = static_cast<int>(output % divider);
            divider /= 10;
        }
        if (!printed) {
            *outIterator = '0';
            ++outIterator;
        }

        *outIterator++ = d_delimiters[1];

        return outIterator;
    }
};

template <>
const char DelimitedIntFormatter<char>::s_defaultDelimiters[2] = { '<', '>' };

template <>
const wchar_t DelimitedIntFormatter<wchar_t>::s_defaultDelimiters[2] = {L'<',
                                                                        L'>'};

// Partial specialization of `bsl::formatter` for `DelimitedInt`.
namespace bsl {
template <class t_CHAR>
struct formatter<DelimitedInt, t_CHAR> : DelimitedIntFormatter<t_CHAR> {
};
}  // close namespace bsl

//=============================================================================
//                   LOCALE ARGUMENT PASSING TEST SUPPORT
//-----------------------------------------------------------------------------

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
template <class t_CHAR>
struct MyDecSep;

template <>
struct MyDecSep<char> {
    static const char s_separator = ',';
};

template <>
struct MyDecSep<wchar_t> {
    static const wchar_t s_separator = L',';
};

template <class t_CHAR>
class CustomNumPunct : public std::numpunct<t_CHAR> {
  protected:
    t_CHAR do_decimal_point() const override
    {
        return MyDecSep<t_CHAR>::s_separator;
    }
};

#endif

//=============================================================================
//                          TEST CASE FUNCTIONS
//-----------------------------------------------------------------------------

void testCase12()
{
    // ------------------------------------------------------------------------
    // BAD FORMAT STRINGS
    //
    // Concerns:
    // 1. Too many format placeholders cause an error (not enough arguments to
    //    format).
    //
    // Plan:
    // 1. Use `bsl::vformat` for runtime testing.
    //
    // Testing:
    //   CONCERN: BAD FORMAT STRINGS
    // ------------------------------------------------------------------------

    if (verbose) puts("\nBAD FORMAT STRINGS"
                      "\n==================");

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    // `char`
    bool formatErrorCaught = false;
    try {
        bsl::string temp;
        bsl::vformat_to(&temp, "{}{}", make_format_args("Hello World"));
    }
    catch (const bsl::format_error& e) {
        formatErrorCaught = true;
        if (veryVerbose) P(e.what());
    }
    catch (...) {
        ASSERT(0 == "Unexpected exception caught!");
        throw;
    }
    ASSERT(formatErrorCaught);

    // `wchar_t`
    formatErrorCaught = false;
    try {
        bsl::wstring temp;
        bsl::vformat_to(&temp, L"{}{}", make_wformat_args(L"Hello World"));
    }
    catch (const bsl::format_error& e) {
        formatErrorCaught = true;
        if (veryVerbose) P(e.what());
    }
    catch (...) {
        ASSERT(0 == "Unexpected exception caught!");
        throw;
    }
    ASSERT(formatErrorCaught);
}

void testCase11()
{
    // ------------------------------------------------------------------------
    // ARGUMENT PASSING
    //
    // Concerns:
    // 1. Arguments are passed to the implementation.  Note that this test case
    //    works in conjunction of the rest of the test driver, not all
    //    parameters are directly verified here.
    //
    // 2. The locale argument is passed to the standard implementation.
    //
    // Plan:
    // 1. Use `bsl::vformat` overloads for runtime testing.
    //
    // 2. Also verify `bsl::format` overloads.
    //
    // 3. Use a custom formatted type in the tests.
    //
    // 4. Verify all overloads, including locale when supported.
    //
    // Testing:
    //   CONCERN: ARGUMENT PASSING
    // ------------------------------------------------------------------------

    if (verbose) puts("\nARGUMENT PASSING"
                      "\n================");

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    if (veryVerbose) puts("Verify user-defined formatter invocation");
    {
        {  // Calling vformat* with default delimiters
            resetDelimitedIntGlobals();

            DelimitedInt tested(42);

            u_VERIFY_VFORMAT_BOTH("{}", "<42>", tested);

            ASSERTV(g_numCtorInvocations,
                    u_VERIFY_VFORMAT_BOTH_NUMCALLS,
                    g_numCtorInvocations == u_VERIFY_VFORMAT_BOTH_NUMCALLS);
            ASSERTV(g_numFormatInvocations,
                    u_VERIFY_VFORMAT_BOTH_NUMCALLS,
                    g_numFormatInvocations == u_VERIFY_VFORMAT_BOTH_NUMCALLS);
            ASSERTV(g_numParseInvocations,
                    u_VERIFY_VFORMAT_BOTH_NUMCALLS,
                    g_numParseInvocations == u_VERIFY_VFORMAT_BOTH_NUMCALLS);
            ASSERTV(ParseContextContents<char>::s_vector.size(),
                    u_VERIFY_VFORMAT_NUMCALLS,
                    ParseContextContents<char>::s_vector.size() ==
                        u_VERIFY_VFORMAT_NUMCALLS);
            ASSERTV(ParseContextContents<wchar_t>::s_vector.size(),
                    u_VERIFY_WVFORMAT_NUMCALLS,
                    ParseContextContents<wchar_t>::s_vector.size() ==
                        u_VERIFY_WVFORMAT_NUMCALLS);
            ASSERTV(g_values.size(),
                    u_VERIFY_VFORMAT_BOTH_NUMCALLS,
                    g_values.size() == u_VERIFY_VFORMAT_BOTH_NUMCALLS);
            for (size_t i = 0; i < ParseContextContents<char>::s_vector.size();
                 ++i) {
                ASSERTV(i,
                        ParseContextContents<char>::s_vector[i].c_str(),
                        ParseContextContents<char>::s_vector[i] == "}");
            }
            for (size_t i = 0;
                 i < ParseContextContents<wchar_t>::s_vector.size();
                 ++i) {
                ASSERTV(i,
                        ParseContextContents<wchar_t>::s_vector[i].c_str(),
                        ParseContextContents<wchar_t>::s_vector[i] == L"}");
            }
            for (size_t i = 0; i < g_values.size(); ++i) {
                ASSERTV(i, g_values[i], g_values[i] == 42);
            }

            if (veryVerbose) printDelimitedIntGlobals();
        }

        {  // Calling format* with default delimiters
            resetDelimitedIntGlobals();

            DelimitedInt tested(42);

            u_VERIFY_FORMAT_BOTH("{}", "<42>", tested);

            ASSERTV(g_numCtorInvocations,
                    u_VERIFY_FORMAT_BOTH_NUMCALLS,
                    g_numCtorInvocations == u_VERIFY_FORMAT_BOTH_NUMCALLS);
            ASSERTV(g_numFormatInvocations,
                    u_VERIFY_FORMAT_BOTH_NUMCALLS,
                    g_numFormatInvocations == u_VERIFY_FORMAT_BOTH_NUMCALLS);
            ASSERTV(g_numParseInvocations,
                    u_VERIFY_FORMAT_BOTH_NUMCALLS,
                    g_numParseInvocations == u_VERIFY_FORMAT_BOTH_NUMCALLS);
            ASSERTV(ParseContextContents<char>::s_vector.size(),
                    u_VERIFY_FORMAT_NUMCALLS,
                    ParseContextContents<char>::s_vector.size() ==
                        u_VERIFY_FORMAT_NUMCALLS);
            ASSERTV(ParseContextContents<wchar_t>::s_vector.size(),
                    u_VERIFY_WFORMAT_NUMCALLS,
                    ParseContextContents<wchar_t>::s_vector.size() ==
                        u_VERIFY_WFORMAT_NUMCALLS);
            ASSERTV(g_values.size(),
                    u_VERIFY_FORMAT_BOTH_NUMCALLS,
                    g_values.size() == u_VERIFY_FORMAT_BOTH_NUMCALLS);
            for (size_t i = 0; i < ParseContextContents<char>::s_vector.size();
                 ++i) {
                ASSERTV(i,
                        ParseContextContents<char>::s_vector[i].c_str(),
                        ParseContextContents<char>::s_vector[i] == "}");
            }
            for (size_t i = 0;
                 i < ParseContextContents<wchar_t>::s_vector.size();
                 ++i) {
                ASSERTV(i,
                        ParseContextContents<wchar_t>::s_vector[i].c_str(),
                        ParseContextContents<wchar_t>::s_vector[i] == L"}");
            }
            for (size_t i = 0; i < g_values.size(); ++i) {
                ASSERTV(i, g_values[i], g_values[i] == 42);
            }

            if (veryVerbose) printDelimitedIntGlobals();
        }

        {  // Calling vformat* with custom delimiters
            resetDelimitedIntGlobals();

            DelimitedInt tested(42);

            u_VERIFY_VFORMAT_BOTH("{:][}", "]42[", tested);

            if (veryVerbose) printDelimitedIntGlobals();
        }

#ifdef BDE_BUILD_TARGET_EXC
        {  // Calling vformat* with error in the format string
            resetDelimitedIntGlobals();
            DelimitedInt tested(42);
            bool         formatErrorCaught = false;
            try {
                const char *fmt = "{:]}";
                u_VERIFY_VFORMAT(fmt, "", tested);
            }
            catch (bsl::format_error&) {
                formatErrorCaught = true;
            }
            ASSERT(formatErrorCaught);
            if (veryVerbose)
                printDelimitedIntGlobals();

            resetDelimitedIntGlobals();
            formatErrorCaught = false;
            try {
                const char *fmt = "{:123}";
                u_VERIFY_VFORMAT(fmt, "", tested);
            }
            catch (bsl::format_error&) {
                formatErrorCaught = true;
            }
            ASSERT(formatErrorCaught);
            if (veryVerbose) printDelimitedIntGlobals();
        }
#endif

        {  // Calling format* with custom delimiters
            resetDelimitedIntGlobals();

            DelimitedInt tested(42);

            u_VERIFY_FORMAT_BOTH("{:][}", "]42[", tested);

            if (veryVerbose) printDelimitedIntGlobals();
        }

        // format* with bad format string would not compile , hence no test

        {  // Calling vformat* with combinations of arguments
            resetDelimitedIntGlobals();

            DelimitedInt di42(42);
            DelimitedInt di65490(65490);
            int          i8(8);
            int          i234876(234876);

            u_VERIFY_VFORMAT_BOTH("{} over {}: {:()}.{:02}~{:==}",
                                  "<42> over 234876: (65490).08~=42=",
                                  di42,
                                  i234876,
                                  di65490,
                                  i8,
                                  di42);

            if (veryVerbose) printDelimitedIntGlobals();
        }


        {  // Calling format* with combinations of arguments
            resetDelimitedIntGlobals();

            DelimitedInt di42(42);
            DelimitedInt di65490(65490);
            int          i8(8);
            int          i234876(234876);

            u_VERIFY_FORMAT_BOTH("{} over {}: {:()}.{:02}~{:==}",
                                 "<42> over 234876: (65490).08~=42=",
                                 di42,
                                 i234876,
                                 di65490,
                                 i8,
                                 di42);

            if (veryVerbose) printDelimitedIntGlobals();
        }
    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
    if (veryVeryVerbose) puts("Verify passing of locale argument");
    {
        std::locale myLocale(std::locale(), new CustomNumPunct<char>);
        std::locale myWlocale(std::locale(), new CustomNumPunct<wchar_t>);

        u_VERIFY_LFORMAT_BOTH("{:L}",      "42,24",      42.24);
        u_VERIFY_LFORMAT_BOTH("{:L} {:L}", "42,24 3,14", 42.24, 3.14f);

        double d = 42.24;
        float  f = 3.14f;
        u_VERIFY_VLFORMAT_BOTH("{:L}",      "42,24",      d);
        u_VERIFY_VLFORMAT_BOTH("{:L} {:L}", "42,24 3,14", d, f);
    }
#endif
}

void testCase10()
{
    // ------------------------------------------------------------------------
    // VFORMAT, VFORMAT_TO
    //
    // Concerns:
    // 1. `vformat`, `vformat_t` calls format as expected.
    //
    // 2. `vformat` uses the specified allocator for the returned string.
    //
    // Plan:
    // 1. Use `bsl::vformat` for runtime testing.
    //
    // Testing:
    //   CONCERN: VFORMAT, VFORMAT_TO
    // ------------------------------------------------------------------------

    if (verbose) puts("\nVFORMAT, VFORMAT_TO"
                      "\n===================");

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    if (veryVerbose) puts("Verify integral types");
    {
        int i42 = 42;
        u_VERIFY_VFORMAT_BOTH("{}", "42", i42);
        signed char sc42 = 42;
        u_VERIFY_VFORMAT_BOTH("{}", "42", sc42);
        short s42 = 42;
        u_VERIFY_VFORMAT_BOTH("{}", "42", s42);
        long l42 = 42;
        u_VERIFY_VFORMAT_BOTH("{}", "42", l42);
        long long ll42 = 42;
        u_VERIFY_VFORMAT_BOTH("{}", "42", ll42);

        unsigned u42 = 42;
        u_VERIFY_VFORMAT_BOTH("{}", "42", u42);
        unsigned char uc42 = 42;
        u_VERIFY_VFORMAT_BOTH("{}", "42", uc42);
        unsigned short us42 = 42;
        u_VERIFY_VFORMAT_BOTH("{}", "42", us42);
        unsigned long ul42 = 42;
        u_VERIFY_VFORMAT_BOTH("{}", "42", ul42);
        unsigned long long ull42 = 42;
        u_VERIFY_VFORMAT_BOTH("{}", "42", ull42);
    }

    if (veryVerbose) puts("Verify `char`");
    {
        char c42 = '*';
        u_VERIFY_VFORMAT_BOTH("{}", "*", c42);
    }

    if (veryVerbose) puts("Verify binary floating point");
    {
        float f42 = 42.24f;
        u_VERIFY_VFORMAT_BOTH("{}", "42.24", f42);
        double d42 = 42.24;
        u_VERIFY_VFORMAT_BOTH("{}", "42.24", d42);
    }

    if (veryVerbose) puts("Verify string types");
    {
        bsl::string bslStr("Text");
        std::string stdStr("Text");

        bsl::wstring bslWstr(L"Text");
        std::wstring stdWstr(L"Text");

        bsl::string_view  bslView("Text");
        bsl::wstring_view bslWview(L"Text");

#ifdef u_STD_STRING_VIEW_EXISTS
        std::string_view stdView("Text");
        std::wstring_view stdWview(L"Text");
#endif

#ifdef u_STD_STRING_VIEW_EXISTS
    #define u_STD_VIEW_TESTS(fmt, res)                                        \
        u_VERIFY_VFORMAT(fmt, res, stdView);                                  \
        u_VERIFY_WVFORMAT(L##fmt, L##res, stdWview)
#else
    #define u_STD_VIEW_TESTS(fmt, res)
#endif

#define u_VERIFY_STRINGS(fmt, res)                                            \
    u_VERIFY_VFORMAT(fmt, res, bslStr);                                       \
    u_VERIFY_WVFORMAT(L##fmt, L##res, bslWstr);                               \
    u_VERIFY_VFORMAT(fmt, res, stdStr);                                       \
    u_VERIFY_WVFORMAT(L##fmt, L##res, stdWstr);                               \
    u_VERIFY_VFORMAT(fmt, res, bslView);                                      \
    u_VERIFY_WVFORMAT(L##fmt, L##res, bslWview);                              \
    u_STD_VIEW_TESTS(fmt, res)

        u_VERIFY_STRINGS("{}", "Text");
        u_VERIFY_STRINGS("{:^6}", " Text ");

#undef u_VERIFY_STRINGS
    }

    if (veryVerbose) puts("Verify `bool`");
    {
        bool b0 = false;
        bool b1 = true;

        u_VERIFY_VFORMAT_BOTH("{}", "false", b0);
        u_VERIFY_VFORMAT_BOTH("{}", "true", b1);
    }

    if (veryVerbose) puts("Verify pointers");
    {
        const void *ptr = reinterpret_cast<const void *>(0x123456);

        u_VERIFY_VFORMAT_BOTH("{}", "0x123456", ptr);
    }

    if (veryVerbose) puts("Verify combinations");
    {
        const void    *p  = reinterpret_cast<const void *>(0xC0FFEE);
        const char    *t  = "PgDown";
        const wchar_t *wt = L"PgDown";
        int            i  = 32;
        double         d  = 13.75;

#ifndef u_GCC_PTR_FMT_WORKAROUND
        u_VERIFY_VFORMAT(
                       "this={:010}, name={:<8}, id={:04x}, heat={:a}",
                       "this=0x00c0ffee, name=PgDown  , id=0020, heat=1.b8p+3",
                       p, t, i, d);

        u_VERIFY_WVFORMAT(
                      L"this={:010}, name={:<8}, id={:04x}, heat={:a}",
                      L"this=0x00c0ffee, name=PgDown  , id=0020, heat=1.b8p+3",
                       p, wt, i, d);
    }
#else   // Not the faulty gcc implementation
        u_VERIFY_VFORMAT(
                       "this={:10}, name={:<8}, id={:04x}, heat={:a}",
                       "this=  0xc0ffee, name=PgDown  , id=0020, heat=1.b8p+3",
                       p, t, i, d);

        u_VERIFY_WVFORMAT(
                      L"this={:10}, name={:<8}, id={:04x}, heat={:a}",
                      L"this=  0xc0ffee, name=PgDown  , id=0020, heat=1.b8p+3",
                      p, wt, i, d);
    }
#endif  // No zero padding of pointers in gcc
}

void testCase9()
{
    // ------------------------------------------------------------------------
    // COMBINATIONS OF FORMATTED TYPES
    //
    // Concerns:
    // 1. Mixed kinds (types) of arguments result in the invocation of
    //    differently configured parser that have to all do proper parsing
    //    of portions of the format string.
    //
    // Plan:
    // 1. Spot check different combinations by using real-life looking format
    //    strings.
    //
    // Testing:
    //   CONCERN: COMBINATIONS OF FORMATTED TYPES
    // ------------------------------------------------------------------------

    if (verbose) puts("\nCOMBINATIONS OF FORMATTED TYPES"
                      "\n===============================");

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

#ifndef u_GCC_PTR_FMT_WORKAROUND
    u_VERIFY_FORMAT("this={:010}, name={:<8}, id={:04x}, heat={:a}",
                    "this=0x00c0ffee, name=PgDown  , id=0020, heat=1.b8p+3",
                    reinterpret_cast<const void *>(0xC0FFEE),
                    "PgDown",
                    32,
                    13.75);

    u_VERIFY_WFORMAT(L"this={:010}, name={:<8}, id={:04x}, heat={:a}",
                     L"this=0x00c0ffee, name=PgDown  , id=0020, heat=1.b8p+3",
                     reinterpret_cast<const void *>(0xC0FFEE),
                     L"PgDown",
                     32,
                     13.75);
#else   // ptrs can be zero padded
    u_VERIFY_FORMAT("this={:10}, name={:<8}, id={:04x}, heat={:a}",
                    "this=  0xc0ffee, name=PgDown  , id=0020, heat=1.b8p+3",
                    reinterpret_cast<const void *>(0xC0FFEE),
                    "PgDown",
                    32,
                    13.75);

    u_VERIFY_WFORMAT(L"this={:10}, name={:<8}, id={:04x}, heat={:a}",
                     L"this=  0xc0ffee, name=PgDown  , id=0020, heat=1.b8p+3",
                     reinterpret_cast<const void *>(0xC0FFEE),
                     L"PgDown",
                     32,
                     13.75);
#endif  // ptrs cannot be zero padded
}

void testCase8()
{
    // ------------------------------------------------------------------------
    // POINTERS
    //
    // Concerns:
    // 1. `nullptr` (if supported) and `bsl::nullptr_t()` are printed as 0x0.
    //
    // 2. Pointer values are printed as hexadecimal addresses with `0x` prefix.
    //
    // 3. Zero padding places the right number of zeros after the prefix.
    //
    // 4. Width and alignment combinations work as expected.
    //
    // 5. Width may be specified as an argument, without or with arg id.
    //
    // 6. Presentation may be specified as 'p' and it results in the same
    //    output as no presentation format specified.
    //
    // 7. Presentation may be specified as 'P' and it results in the prefix
    //    changing to "0X" and the hexadecimal output being uppercase.
    //
    // 8. `char` and `wchar_t` formatting both work.
    //
    // Plan:
    // 1. Use individual macros to create rows of a table-like test that covers
    //    the concerns.
    //
    // Testing:
    //   CONCERN: POINTERS
    // ------------------------------------------------------------------------

    if (verbose) puts("\nPOINTERS"
                      "\n========");

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    // Verify `nullptr`
#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
        u_VERIFY_FORMAT_BOTH("{}", "0x0", nullptr);
#endif  // C++11 or later
        u_VERIFY_FORMAT_BOTH("{}", "0x0", bsl::nullptr_t());

#define u_VERIFY_POINTER(fmt, res, val) \
    u_VERIFY_FORMAT_BOTH(fmt, res, reinterpret_cast<const void *>(val))

    // Simple printing of pointer values

    u_VERIFY_POINTER("{}", "0x12345", 0x12345);

#ifndef u_GCC_PTR_FMT_WORKAROUND
    // Zero padding

    u_VERIFY_FORMAT_BOTH("{:010}", "0x00000000", bsl::nullptr_t());

    u_VERIFY_POINTER("{:010}", "0x00012345", 0x12345);
#endif  // gcc does not support 0 padding

    // Aligned by constant in format string

    u_VERIFY_FORMAT_BOTH("{:1}",  "0x0",   bsl::nullptr_t());
    u_VERIFY_FORMAT_BOTH("{:2}",  "0x0",   bsl::nullptr_t());
    u_VERIFY_FORMAT_BOTH("{:3}",  "0x0",   bsl::nullptr_t());
    u_VERIFY_FORMAT_BOTH("{:4}",  " 0x0",  bsl::nullptr_t());
    u_VERIFY_FORMAT_BOTH("{:5}",  "  0x0", bsl::nullptr_t());
    u_VERIFY_FORMAT_BOTH("{:<5}", "0x0  ", bsl::nullptr_t());
    u_VERIFY_FORMAT_BOTH("{:^5}", " 0x0 ", bsl::nullptr_t());
    u_VERIFY_FORMAT_BOTH("{:>5}", "  0x0", bsl::nullptr_t());

    u_VERIFY_POINTER("{:1}",  "0x12345",   0x12345);
    u_VERIFY_POINTER("{:2}",  "0x12345",   0x12345);
    u_VERIFY_POINTER("{:6}",  "0x12345",   0x12345);
    u_VERIFY_POINTER("{:7}",  "0x12345",   0x12345);
    u_VERIFY_POINTER("{:8}",  " 0x12345",  0x12345);
    u_VERIFY_POINTER("{:9}",  "  0x12345", 0x12345);
    u_VERIFY_POINTER("{:<9}", "0x12345  ", 0x12345);
    u_VERIFY_POINTER("{:^9}", " 0x12345 ", 0x12345);
    u_VERIFY_POINTER("{:>9}", "  0x12345", 0x12345);

    // Aligned by parameter

    u_VERIFY_FORMAT_BOTH("{:<{}}", "0x0  ", bsl::nullptr_t(), 5);
    u_VERIFY_FORMAT_BOTH("{:^{}}", " 0x0 ", bsl::nullptr_t(), 5);
    u_VERIFY_FORMAT_BOTH("{:>{}}", "  0x0", bsl::nullptr_t(), 5);

    u_VERIFY_FORMAT_BOTH("{1:<{0}}", "0x0  ", 5, bsl::nullptr_t());
    u_VERIFY_FORMAT_BOTH("{1:^{0}}", " 0x0 ", 5, bsl::nullptr_t());
    u_VERIFY_FORMAT_BOTH("{1:>{0}}", "  0x0", 5, bsl::nullptr_t());

#define u_VERIFY_POINTER_WIDTH(fmt, res, val, width)                          \
    u_VERIFY_FORMAT_BOTH(fmt, res, reinterpret_cast<const void *>(val), width)

    u_VERIFY_POINTER_WIDTH("{:<{}}", "0x12345  ", 0x12345, 9);
    u_VERIFY_POINTER_WIDTH("{:^{}}", " 0x12345 ", 0x12345, 9);
    u_VERIFY_POINTER_WIDTH("{:>{}}", "  0x12345", 0x12345, 9);
#undef u_VERIFY_POINTER_WIDTH

#define u_VERIFY_WIDTH_POINTER(fmt, res, width, val)                          \
    u_VERIFY_FORMAT_BOTH(fmt, res, width, reinterpret_cast<const void *>(val))

    u_VERIFY_WIDTH_POINTER("{1:<{0}}", "0x12345  ", 9, 0x12345);
    u_VERIFY_WIDTH_POINTER("{1:^{0}}", " 0x12345 ", 9, 0x12345);
    u_VERIFY_WIDTH_POINTER("{1:>{0}}", "  0x12345", 9, 0x12345);
#undef u_VERIFY_WIDTH_POINTER

    // Explicitly specified presentation

    u_VERIFY_FORMAT_BOTH("{:p}", "0x0", bsl::nullptr_t());

    u_VERIFY_POINTER("{:p}", "0x123ab", 0x123AB);

#ifndef u_GCC_PTR_FMT_WORKAROUND
    u_VERIFY_FORMAT_BOTH("{:P}", "0X0", bsl::nullptr_t());

    u_VERIFY_POINTER("{:P}", "0X123AB", 0x123AB);
#endif  // gcc does not support 'P' specifier

#undef u_VERIFY_POINTER
}

void testCase7()
{
    // ------------------------------------------------------------------------
    // BOOLEAN
    //
    // Concerns:
    // 1. Static text in format string is copied to the output unmodified.
    //
    // 2. Width specification with and without alignment works according to the
    //    standard requirements in its direct form as well as when width is
    //    specified by an argument (embedded).
    //
    // 3. Format specification characters have the intended result.
    //
    // Plan:
    // 1. Use individual macros to create rows of a table-like test that covers
    //    the concerns.
    //
    // Testing:
    //   CONCERN: BOOLEAN
    // ------------------------------------------------------------------------

    if (verbose) puts("\nBOOLEAN"
                      "\n========");

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    // Simple printing of boolean values

    u_VERIFY_FORMAT_BOTH("{}",            "true",            true);
    u_VERIFY_FORMAT_BOTH("Verified: {}",  "Verified: true",  true);
    u_VERIFY_FORMAT_BOTH("{} love",       "true love",       true);
    u_VERIFY_FORMAT_BOTH("autoConsumeData() {} (default)",
                         "autoConsumeData() true (default)", true);

    u_VERIFY_FORMAT_BOTH("{}",           "false",            false);
    u_VERIFY_FORMAT_BOTH("Error: {}",    "Error: false",     false);
    u_VERIFY_FORMAT_BOTH("{} dichotomy", "false dichotomy",  false);
    u_VERIFY_FORMAT_BOTH("Alternate format {} (default)",
                         "Alternate format false (default)", false);

    // Aligned by constant in format string

    u_VERIFY_FORMAT_BOTH("{:1}",  "true",   true);
    u_VERIFY_FORMAT_BOTH("{:2}",  "true",   true);
    u_VERIFY_FORMAT_BOTH("{:3}",  "true",   true);
    u_VERIFY_FORMAT_BOTH("{:4}",  "true",   true);
    u_VERIFY_FORMAT_BOTH("{:5}",  "true ",  true);
    u_VERIFY_FORMAT_BOTH("{:6}",  "true  ", true);
    u_VERIFY_FORMAT_BOTH("{:<6}", "true  ", true);
    u_VERIFY_FORMAT_BOTH("{:^6}", " true ", true);
    u_VERIFY_FORMAT_BOTH("{:>6}", "  true", true);

    u_VERIFY_FORMAT_BOTH("{:1}",  "false",   false);
    u_VERIFY_FORMAT_BOTH("{:2}",  "false",   false);
    u_VERIFY_FORMAT_BOTH("{:3}",  "false",   false);
    u_VERIFY_FORMAT_BOTH("{:4}",  "false",   false);
    u_VERIFY_FORMAT_BOTH("{:5}",  "false",   false);
    u_VERIFY_FORMAT_BOTH("{:6}",  "false ",  false);
    u_VERIFY_FORMAT_BOTH("{:7}",  "false  ", false);
    u_VERIFY_FORMAT_BOTH("{:<7}", "false  ", false);
    u_VERIFY_FORMAT_BOTH("{:^7}", " false ", false);
    u_VERIFY_FORMAT_BOTH("{:>7}", "  false", false);

    // Aligned boolean values by parameter

    u_VERIFY_FORMAT_BOTH("{:<{}}", "true  ", true, 6);
    u_VERIFY_FORMAT_BOTH("{:^{}}", " true ", true, 6);
    u_VERIFY_FORMAT_BOTH("{:>{}}", "  true", true, 6);

    u_VERIFY_FORMAT_BOTH("{1:<{0}}", "true  ", 6, true);
    u_VERIFY_FORMAT_BOTH("{1:^{0}}", " true ", 6, true);
    u_VERIFY_FORMAT_BOTH("{1:>{0}}", "  true", 6, true);

    u_VERIFY_FORMAT_BOTH("{:<{}}", "false  ", false, 7);
    u_VERIFY_FORMAT_BOTH("{:^{}}", " false ", false, 7);
    u_VERIFY_FORMAT_BOTH("{:>{}}", "  false", false, 7);

    u_VERIFY_FORMAT_BOTH("{1:<{0}}", "false  ", 7, false);
    u_VERIFY_FORMAT_BOTH("{1:^{0}}", " false ", 7, false);
    u_VERIFY_FORMAT_BOTH("{1:>{0}}", "  false", 7, false);

    // Explicitly specified presentations

    u_VERIFY_FORMAT_BOTH("{:s}", "true",  true);
    u_VERIFY_FORMAT_BOTH("{:s}", "false", false);

    u_VERIFY_FORMAT_BOTH("{:b}", "1", true);
    u_VERIFY_FORMAT_BOTH("{:b}", "0", false);
    u_VERIFY_FORMAT_BOTH("{:B}", "1", true);
    u_VERIFY_FORMAT_BOTH("{:B}", "0", false);

    u_VERIFY_FORMAT_BOTH("{:d}", "1", true);
    u_VERIFY_FORMAT_BOTH("{:d}", "0", false);

    u_VERIFY_FORMAT_BOTH("{:o}", "1", true);
    u_VERIFY_FORMAT_BOTH("{:o}", "0", false);

    u_VERIFY_FORMAT_BOTH("{:x}", "1", true);
    u_VERIFY_FORMAT_BOTH("{:x}", "0", false);
}

void testCase6()
{
    // ------------------------------------------------------------------------
    // STRINGS
    //
    // Concerns:
    // 1. All supported string types print their value properly.
    //
    // 2. Width specification with and without alignment works according to the
    //    standard requirements in its direct form as well as when width is
    //    specified by an argument (embedded).
    //
    // 3. The 's' format specification character may be used.
    //
    // 4. A "precision" specification ('.' followed by an integer) truncates
    //    the printed string.
    //
    // Plan:
    // 1. Use individual macros to create rows of a table-like test that covers
    //    the concerns.
    //
    // Testing:
    //   CONCERN: STRINGS
    // ------------------------------------------------------------------------

    if (verbose) puts("\nSTRINGS"
                      "\n=======");

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

#if defined(BSLSTL_STRING_VIEW_AND_STD_STRING_VIEW_COEXIST) ||                \
    defined(BSLSTL_STRING_VIEW_IS_ALIASED)
  #define u_VERIFY_STD_VIEWS(fmt, res, lit)                                   \
      u_VERIFY_FORMAT(fmt, res, std::string_view(lit));                       \
      u_VERIFY_WFORMAT(L##fmt, L##res, std::wstring_view(L##lit))
#else
  #define u_VERIFY_STD_VIEWS(fmt, res, lit)
#endif

#define u_VERIFY_STRINGS(fmt, res, lit)                                       \
    u_VERIFY_FORMAT(fmt, res, lit);                                           \
    u_VERIFY_WFORMAT(L##fmt, L##res, L##lit);                                 \
    u_VERIFY_FORMAT(fmt, res, std::string(lit));                              \
    u_VERIFY_WFORMAT(L##fmt, L##res, std::wstring(L##lit));                   \
    u_VERIFY_FORMAT(fmt, res, bsl::string(lit));                              \
    u_VERIFY_WFORMAT(L##fmt, L##res, bsl::wstring(L##lit));                   \
    u_VERIFY_FORMAT(fmt, res, bsl::string_view(lit));                         \
    u_VERIFY_WFORMAT(L##fmt, L##res, bsl::wstring_view(L##lit));              \
    u_VERIFY_STD_VIEWS(fmt, res, lit)

    // Simple printing of strings values

    u_VERIFY_STRINGS("{}",   "Text",   "Text");
    u_VERIFY_STRINGS(" {}",  " Text",  "Text");
    u_VERIFY_STRINGS("{} ",  "Text ",  "Text");
    u_VERIFY_STRINGS(" {} ", " Text ", "Text");

    u_VERIFY_STRINGS("{:s}",   "Text",   "Text");
    u_VERIFY_STRINGS(" {:s}",  " Text",  "Text");
    u_VERIFY_STRINGS("{:s} ",  "Text ",  "Text");
    u_VERIFY_STRINGS(" {:s} ", " Text ", "Text");

    // Aligned by constant width in format string

    u_VERIFY_STRINGS("{:1}",  "Text",   "Text");
    u_VERIFY_STRINGS("{:2}",  "Text",   "Text");
    u_VERIFY_STRINGS("{:3}",  "Text",   "Text");
    u_VERIFY_STRINGS("{:4}",  "Text",   "Text");
    u_VERIFY_STRINGS("{:5}",  "Text ",  "Text");
    u_VERIFY_STRINGS("{:6}",  "Text  ", "Text");
    u_VERIFY_STRINGS("{:<6}", "Text  ", "Text");
    u_VERIFY_STRINGS("{:^6}", " Text ", "Text");
    u_VERIFY_STRINGS("{:>6}", "  Text", "Text");

    // Truncated by constant "precision" in format string

    u_VERIFY_STRINGS("{:.1}", "T",    "Text");
    u_VERIFY_STRINGS("{:.2}", "Te",   "Text");
    u_VERIFY_STRINGS("{:.3}", "Tex",  "Text");
    u_VERIFY_STRINGS("{:.4}", "Text", "Text");
    u_VERIFY_STRINGS("{:.5}", "Text", "Text");

#undef u_VERIFY_STRINGS
}

void testCase5()
{
    // ------------------------------------------------------------------------
    // FLOAT AND DOUBLE
    //
    // Concerns:
    // 1. Both supported types ('double', and 'float') print their values
    //    properly.
    //
    // 2. Sign format specification works as intended (' ', '+', '-').
    //
    // 3. Width specification with and without alignment works according to the
    //    standard requirements in its direct form as well as when width is
    //    specified by an argument (embedded).
    //
    // 4. Presentation format specifiers (characters) print using the specified
    //    standard presentation format.
    //
    // 5. Precision specification works as per standard in all presentation
    //    formats including the default.
    //
    // 6. Alternate formats work as per standard.
    //
    // Plan:
    // 1. Use individual macros to create rows of a table-like test that covers
    //    the concerns.
    //
    // Testing:
    //   CONCERN: FLOAT AND DOUBLE
    // ------------------------------------------------------------------------

    if (verbose) puts("\nFLOAT AND DOUBLE"
                      "\n================");

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    // Simple printing of float/double values

    u_VERIFY_FORMAT_BOTH("{}",   "42.24",   42.24);
    u_VERIFY_FORMAT_BOTH(" {}",  " 42.24",  42.24);
    u_VERIFY_FORMAT_BOTH("{} ",  "42.24 ",  42.24);
    u_VERIFY_FORMAT_BOTH(" {} ", " 42.24 ", 42.24);

    u_VERIFY_FORMAT_BOTH("{}",   "42.24",   42.24f);
    u_VERIFY_FORMAT_BOTH(" {}",  " 42.24",  42.24f);
    u_VERIFY_FORMAT_BOTH("{} ",  "42.24 ",  42.24f);
    u_VERIFY_FORMAT_BOTH(" {} ", " 42.24 ", 42.24f);

    // Sign formatting specification

    u_VERIFY_FORMAT_BOTH("{: }",   " 42.24",   42.24);
    u_VERIFY_FORMAT_BOTH(" {: }",  "  42.24",  42.24);
    u_VERIFY_FORMAT_BOTH("{: } ",  " 42.24 ",  42.24);
    u_VERIFY_FORMAT_BOTH(" {: } ", "  42.24 ", 42.24);

    u_VERIFY_FORMAT_BOTH("{: }",   " 42.24",   42.24f);
    u_VERIFY_FORMAT_BOTH(" {: }",  "  42.24",  42.24f);
    u_VERIFY_FORMAT_BOTH("{: } ",  " 42.24 ",  42.24f);
    u_VERIFY_FORMAT_BOTH(" {: } ", "  42.24 ", 42.24f);

    u_VERIFY_FORMAT_BOTH("{: }",   "-42.24",   -42.24);
    u_VERIFY_FORMAT_BOTH(" {: }",  " -42.24",  -42.24);
    u_VERIFY_FORMAT_BOTH("{: } ",  "-42.24 ",  -42.24);
    u_VERIFY_FORMAT_BOTH(" {: } ", " -42.24 ", -42.24);

    u_VERIFY_FORMAT_BOTH("{: }",   "-42.24",   -42.24f);
    u_VERIFY_FORMAT_BOTH(" {: }",  " -42.24",  -42.24f);
    u_VERIFY_FORMAT_BOTH("{: } ",  "-42.24 ",  -42.24f);
    u_VERIFY_FORMAT_BOTH(" {: } ", " -42.24 ", -42.24f);

    u_VERIFY_FORMAT_BOTH("{:-}",   "42.24",    42.24);
    u_VERIFY_FORMAT_BOTH(" {:-}",  " 42.24",   42.24);
    u_VERIFY_FORMAT_BOTH("{:-} ",  "42.24 ",   42.24);
    u_VERIFY_FORMAT_BOTH(" {:-} ", " 42.24 ",  42.24);

    u_VERIFY_FORMAT_BOTH("{:-}",   "42.24",    42.24f);
    u_VERIFY_FORMAT_BOTH(" {:-}",  " 42.24",   42.24f);
    u_VERIFY_FORMAT_BOTH("{:-} ",  "42.24 ",   42.24f);
    u_VERIFY_FORMAT_BOTH(" {:-} ", " 42.24 ",  42.24f);

    u_VERIFY_FORMAT_BOTH("{:-}",   "-42.24",   -42.24);
    u_VERIFY_FORMAT_BOTH(" {:-}",  " -42.24",  -42.24);
    u_VERIFY_FORMAT_BOTH("{:-} ",  "-42.24 ",  -42.24);
    u_VERIFY_FORMAT_BOTH(" {:-} ", " -42.24 ", -42.24);

    u_VERIFY_FORMAT_BOTH("{:-}",   "-42.24",   -42.24f);
    u_VERIFY_FORMAT_BOTH(" {:-}",  " -42.24",  -42.24f);
    u_VERIFY_FORMAT_BOTH("{:-} ",  "-42.24 ",  -42.24f);
    u_VERIFY_FORMAT_BOTH(" {:-} ", " -42.24 ", -42.24f);

    u_VERIFY_FORMAT_BOTH("{:+}",   "+42.24",   42.24);
    u_VERIFY_FORMAT_BOTH(" {:+}",  " +42.24",  42.24);
    u_VERIFY_FORMAT_BOTH("{:+} ",  "+42.24 ",  42.24);
    u_VERIFY_FORMAT_BOTH(" {:+} ", " +42.24 ", 42.24);

    u_VERIFY_FORMAT_BOTH("{:+}",   "+42.24",   42.24f);
    u_VERIFY_FORMAT_BOTH(" {:+}",  " +42.24",  42.24f);
    u_VERIFY_FORMAT_BOTH("{:+} ",  "+42.24 ",  42.24f);
    u_VERIFY_FORMAT_BOTH(" {:+} ", " +42.24 ", 42.24f);

    u_VERIFY_FORMAT_BOTH("{:+}",   "-42.24",   -42.24);
    u_VERIFY_FORMAT_BOTH(" {:+}",  " -42.24",  -42.24);
    u_VERIFY_FORMAT_BOTH("{:+} ",  "-42.24 ",  -42.24);
    u_VERIFY_FORMAT_BOTH(" {:+} ", " -42.24 ", -42.24);

    u_VERIFY_FORMAT_BOTH("{:+}",   "-42.24",   -42.24f);
    u_VERIFY_FORMAT_BOTH(" {:+}",  " -42.24",  -42.24f);
    u_VERIFY_FORMAT_BOTH("{:+} ",  "-42.24 ",  -42.24f);
    u_VERIFY_FORMAT_BOTH(" {:+} ", " -42.24 ", -42.24f);

    // Aligned by constant in format string

    u_VERIFY_FORMAT_BOTH("{:1}",  "42.24",   42.24);
    u_VERIFY_FORMAT_BOTH("{:2}",  "42.24",   42.24);
    u_VERIFY_FORMAT_BOTH("{:3}",  "42.24",   42.24);
    u_VERIFY_FORMAT_BOTH("{:4}",  "42.24",   42.24);
    u_VERIFY_FORMAT_BOTH("{:5}",  "42.24",   42.24);
    u_VERIFY_FORMAT_BOTH("{:6}",  " 42.24",  42.24);
    u_VERIFY_FORMAT_BOTH("{:7}",  "  42.24", 42.24);
    u_VERIFY_FORMAT_BOTH("{:<7}", "42.24  ", 42.24);
    u_VERIFY_FORMAT_BOTH("{:^7}", " 42.24 ", 42.24);
    u_VERIFY_FORMAT_BOTH("{:>7}", "  42.24", 42.24);

    u_VERIFY_FORMAT_BOTH("{:1}",  "42.24",   42.24f);
    u_VERIFY_FORMAT_BOTH("{:2}",  "42.24",   42.24f);
    u_VERIFY_FORMAT_BOTH("{:3}",  "42.24",   42.24f);
    u_VERIFY_FORMAT_BOTH("{:4}",  "42.24",   42.24f);
    u_VERIFY_FORMAT_BOTH("{:5}",  "42.24",   42.24f);
    u_VERIFY_FORMAT_BOTH("{:6}",  " 42.24",  42.24f);
    u_VERIFY_FORMAT_BOTH("{:7}",  "  42.24", 42.24f);
    u_VERIFY_FORMAT_BOTH("{:<7}", "42.24  ", 42.24f);
    u_VERIFY_FORMAT_BOTH("{:^7}", " 42.24 ", 42.24f);
    u_VERIFY_FORMAT_BOTH("{:>7}", "  42.24", 42.24f);

    // Aligned by additional argument

    u_VERIFY_FORMAT_BOTH("{:<{}}", "42.24  ", 42.24, 7);
    u_VERIFY_FORMAT_BOTH("{:^{}}", " 42.24 ", 42.24, 7);
    u_VERIFY_FORMAT_BOTH("{:>{}}", "  42.24", 42.24, 7);

    u_VERIFY_FORMAT_BOTH("{:<{}}", "42.24  ", 42.24f, 7);
    u_VERIFY_FORMAT_BOTH("{:^{}}", " 42.24 ", 42.24f, 7);
    u_VERIFY_FORMAT_BOTH("{:>{}}", "  42.24", 42.24f, 7);

    u_VERIFY_FORMAT_BOTH("{1:<{0}}", "42.24  ", 7, 42.24);
    u_VERIFY_FORMAT_BOTH("{1:^{0}}", " 42.24 ", 7, 42.24);
    u_VERIFY_FORMAT_BOTH("{1:>{0}}", "  42.24", 7, 42.24);

    u_VERIFY_FORMAT_BOTH("{1:<{0}}", "42.24  ", 7, 42.24f);
    u_VERIFY_FORMAT_BOTH("{1:^{0}}", " 42.24 ", 7, 42.24f);
    u_VERIFY_FORMAT_BOTH("{1:>{0}}", "  42.24", 7, 42.24f);

    // Explicitly specified presentations

    u_VERIFY_FORMAT_BOTH(  "{:e}", "4.224000e+01", 42.24);
    u_VERIFY_FORMAT_BOTH(  "{:E}", "4.224000E+01", 42.24);
    u_VERIFY_FORMAT_BOTH(  "{:e}", "4.224000e+01", 42.24f);
    u_VERIFY_FORMAT_BOTH(  "{:E}", "4.224000E+01", 42.24f);

    u_VERIFY_FORMAT_BOTH(  "{:f}", "42.240000", 42.24);
    u_VERIFY_FORMAT_BOTH(  "{:F}", "42.240000", 42.24);
    u_VERIFY_FORMAT_BOTH(  "{:f}", "42.240002", 42.24f);
    u_VERIFY_FORMAT_BOTH(  "{:F}", "42.240002", 42.24f);

    u_VERIFY_FORMAT_BOTH(  "{:g}", "42.24", 42.24);
    u_VERIFY_FORMAT_BOTH(  "{:G}", "42.24", 42.24);
    u_VERIFY_FORMAT_BOTH(  "{:g}", "42.24", 42.24f);
    u_VERIFY_FORMAT_BOTH(  "{:G}", "42.24", 42.24f);

    u_VERIFY_FORMAT_BOTH(  "{:g}", "1.23e+25", 1.23e25);
    u_VERIFY_FORMAT_BOTH(  "{:G}", "1.23E+25", 1.23e25);
    u_VERIFY_FORMAT_BOTH(  "{:g}", "1.23e+25", 1.23e25f);
    u_VERIFY_FORMAT_BOTH(  "{:G}", "1.23E+25", 1.23e25f);

    u_VERIFY_FORMAT_BOTH("{:a}", "1.51eb851eb851fp+5", 42.24);
    u_VERIFY_FORMAT_BOTH("{:A}", "1.51EB851EB851FP+5", 42.24);
    u_VERIFY_FORMAT_BOTH("{:a}", "1.51eb86p+5", 42.24f);
    u_VERIFY_FORMAT_BOTH("{:A}", "1.51EB86P+5", 42.24f);

    // Precision is specified

    u_VERIFY_FORMAT_BOTH("{:.3e}", "4.224e+01", 42.24);
    u_VERIFY_FORMAT_BOTH("{:.3E}", "4.224E+01", 42.24);
    u_VERIFY_FORMAT_BOTH("{:.3e}", "4.224e+01", 42.24f);
    u_VERIFY_FORMAT_BOTH("{:.3E}", "4.224E+01", 42.24f);

    u_VERIFY_FORMAT_BOTH("{:.2f}", "42.24", 42.24);
    u_VERIFY_FORMAT_BOTH("{:.2F}", "42.24", 42.24);
    u_VERIFY_FORMAT_BOTH("{:.2f}", "42.24", 42.24f);
    u_VERIFY_FORMAT_BOTH("{:.2F}", "42.24", 42.24f);

    u_VERIFY_FORMAT_BOTH("{:.3g}", "42.2", 42.24);
    u_VERIFY_FORMAT_BOTH("{:.3G}", "42.2", 42.24);
    u_VERIFY_FORMAT_BOTH("{:.3g}", "42.2", 42.24f);
    u_VERIFY_FORMAT_BOTH("{:.3G}", "42.2", 42.24f);

    u_VERIFY_FORMAT_BOTH("{:.2g}", "1.2e+25", 1.23e25);
    u_VERIFY_FORMAT_BOTH("{:.2G}", "1.2E+25", 1.23e25);
    u_VERIFY_FORMAT_BOTH("{:.2g}", "1.2e+25", 1.23e25f);
    u_VERIFY_FORMAT_BOTH("{:.2G}", "1.2E+25", 1.23e25f);

    // Alternate forms

    u_VERIFY_FORMAT_BOTH("{}",   "42",  42.);
    u_VERIFY_FORMAT_BOTH("{:#}", "42.", 42.);
    u_VERIFY_FORMAT_BOTH("{}",   "42",  42.f);
    u_VERIFY_FORMAT_BOTH("{:#}", "42.", 42.f);

    u_VERIFY_FORMAT_BOTH("{:.0e}",  "4e+00",  4.);
    u_VERIFY_FORMAT_BOTH("{:#.0e}", "4.e+00", 4.);
    u_VERIFY_FORMAT_BOTH("{:.0e}",  "4e+00",  4.f);
    u_VERIFY_FORMAT_BOTH("{:#.0e}", "4.e+00", 4.f);

    u_VERIFY_FORMAT_BOTH("{:.0f}",  "4",  4.);
    u_VERIFY_FORMAT_BOTH("{:#.0F}", "4.", 4.);
    u_VERIFY_FORMAT_BOTH("{:.0f}",  "4",  4.f);
    u_VERIFY_FORMAT_BOTH("{:#.0F}", "4.", 4.f);

    u_VERIFY_FORMAT_BOTH("{:.0a}",  "1p+2",  4.);
    u_VERIFY_FORMAT_BOTH("{:#.0A}", "1.P+2", 4.);
    u_VERIFY_FORMAT_BOTH("{:.0a}",  "1p+2",  4.f);
    u_VERIFY_FORMAT_BOTH("{:#.0A}", "1.P+2", 4.f);

    u_VERIFY_FORMAT_BOTH("{:#g}", "42.2400", 42.24);
    u_VERIFY_FORMAT_BOTH("{:#G}", "42.2400", 42.24);
    u_VERIFY_FORMAT_BOTH("{:#g}", "42.2400", 42.24f);
    u_VERIFY_FORMAT_BOTH("{:#G}", "42.2400", 42.24f);
}

void testCase4()
{
    // ------------------------------------------------------------------------
    // CHAR AND WCHAR_T
    //
    // Concerns:
    // 1. Characters and wide characters are printed (by default) as a string
    //    that consists of that character only.
    //
    // 2. Width specification with and without alignment works according to the
    //    standard requirements in its direct form as well as when width is
    //    specified by an argument (embedded).
    //
    // 3. Presentation format specifiers (characters) work as per standard.
    //
    // 4. Alternate format adds the format prefixes in the appropriate
    //    presentation formats as per standard.
    //
    // Plan:
    // 1. Use individual macros to create rows of a table-like test that covers
    //    the concerns.
    //
    // Testing:
    //   CONCERN: CHAR AND WCHAR_T
    // ------------------------------------------------------------------------

    if (verbose) puts("\nCHAR AND WCHAR_T"
                      "\n================");

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    // Simple printing of char/wchar_t values

    u_VERIFY_FORMAT("{}",   "*",   '*');
    u_VERIFY_FORMAT(" {}",  " *",  '*');
    u_VERIFY_FORMAT("{} ",  "* ",  '*');
    u_VERIFY_FORMAT(" {} ", " * ", '*');

    u_VERIFY_WFORMAT(L"{}",   L"*",   L'*');
    u_VERIFY_WFORMAT(L" {}",  L" *",  L'*');
    u_VERIFY_WFORMAT(L"{} ",  L"* ",  L'*');
    u_VERIFY_WFORMAT(L" {} ", L" * ", L'*');

    // Aligned by constant in format string

    u_VERIFY_FORMAT("{:1}",  "*",   '*');
    u_VERIFY_FORMAT("{:2}",  "* ",  '*');
    u_VERIFY_FORMAT("{:3}",  "*  ", '*');
    u_VERIFY_FORMAT("{:<3}", "*  ", '*');
    u_VERIFY_FORMAT("{:^3}", " * ", '*');
    u_VERIFY_FORMAT("{:>3}", "  *", '*');

    u_VERIFY_WFORMAT(L"{:1}",  L"*",   L'*');
    u_VERIFY_WFORMAT(L"{:2}",  L"* ",  L'*');
    u_VERIFY_WFORMAT(L"{:3}",  L"*  ", L'*');
    u_VERIFY_WFORMAT(L"{:<3}", L"*  ", L'*');
    u_VERIFY_WFORMAT(L"{:^3}", L" * ", L'*');
    u_VERIFY_WFORMAT(L"{:>3}", L"  *", L'*');

    // Aligned by additional argument

    u_VERIFY_FORMAT("{:<{}}", "*  ", '*', 3);
    u_VERIFY_FORMAT("{:^{}}", " * ", '*', 3);
    u_VERIFY_FORMAT("{:>{}}", "  *", '*', 3);

    u_VERIFY_WFORMAT(L"{:<{}}", L"*  ", L'*', 3);
    u_VERIFY_WFORMAT(L"{:^{}}", L" * ", L'*', 3);
    u_VERIFY_WFORMAT(L"{:>{}}", L"  *", L'*', 3);

    u_VERIFY_FORMAT("{1:<{0}}", "*  ", 3, '*');
    u_VERIFY_FORMAT("{1:^{0}}", " * ", 3, '*');
    u_VERIFY_FORMAT("{1:>{0}}", "  *", 3, '*');

    u_VERIFY_WFORMAT(L"{1:<{0}}", L"*  ", 3, L'*');
    u_VERIFY_WFORMAT(L"{1:^{0}}", L" * ", 3, L'*');
    u_VERIFY_WFORMAT(L"{1:>{0}}", L"  *", 3, L'*');

    // Explicitly specified presentations

    u_VERIFY_FORMAT(  "{:c}",  "*",  '*');
    u_VERIFY_WFORMAT(L"{:c}", L"*", L'*');

    u_VERIFY_FORMAT(  "{:d}",  "42",  '*');
    u_VERIFY_WFORMAT(L"{:d}", L"42", L'*');

    u_VERIFY_FORMAT(  "{:o}",  "52",  '*');
    u_VERIFY_WFORMAT(L"{:o}", L"52", L'*');

    u_VERIFY_FORMAT(  "{:x}",  "2a",  '*');
    u_VERIFY_WFORMAT(L"{:x}", L"2a", L'*');

    u_VERIFY_FORMAT(  "{:X}",  "2A",  '*');
    u_VERIFY_WFORMAT(L"{:X}", L"2A", L'*');

    u_VERIFY_FORMAT(  "{:b}",  "101010",  '*');
    u_VERIFY_WFORMAT(L"{:b}", L"101010", L'*');

    u_VERIFY_FORMAT(  "{:B}",  "101010",  '*');
    u_VERIFY_WFORMAT(L"{:B}", L"101010", L'*');

    // Alternate form with prefix

    u_VERIFY_FORMAT(  "{:#o}",  "052",  '*');
    u_VERIFY_WFORMAT(L"{:#o}", L"052", L'*');
    u_VERIFY_FORMAT(  "{:#o}",  "0",    '\0');
    u_VERIFY_WFORMAT(L"{:#o}", L"0",   L'\0');

    u_VERIFY_FORMAT(  "{:#x}",  "0x2a",  '*');
    u_VERIFY_WFORMAT(L"{:#x}", L"0x2a", L'*');

    u_VERIFY_FORMAT("{:#X}",    "0X2A", '*');
    u_VERIFY_WFORMAT(L"{:#X}", L"0X2A", L'*');

    u_VERIFY_FORMAT(  "{:#b}",  "0b101010", '*');
    u_VERIFY_WFORMAT(L"{:#b}", L"0b101010", L'*');

    u_VERIFY_FORMAT(  "{:#B}",  "0B101010", '*');
    u_VERIFY_WFORMAT(L"{:#B}", L"0B101010", L'*');
}

void testCase3()
{
    // ------------------------------------------------------------------------
    // INTEGERS
    //
    // Concerns:
    // 1. All integers types are converted to string properly.
    //
    // 2. Sign format specification works as intended (' ', '+', '-').
    //
    // 3. Width specification with and without alignment works according to
    //    the standard requirements in its direct form as well as when width is
    //    specified by an argument (embedded).
    //
    // 4. Presentation format specifiers (characters) print using the specified
    //    standard presentation format.
    //
    // 5. Alternate formats work as per standard.
    //
    // Plan:
    // 1. Use individual macros to create rows of a table-like test that covers
    //    the concerns.
    //
    // Testing:
    //   CONCERN: INTEGERS
    // ------------------------------------------------------------------------

    if (verbose) puts("\nINTEGERS"
                      "\n========");

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    // Simple printing of integral values

    u_VERIFY_FORMAT_BOTH("{}",   "42",   42);
    u_VERIFY_FORMAT_BOTH(" {}",  " 42",  42);
    u_VERIFY_FORMAT_BOTH("{} ",  "42 ",  42);
    u_VERIFY_FORMAT_BOTH(" {} ", " 42 ", 42);

    u_VERIFY_FORMAT_BOTH("{}",   "42",   (signed char)42);
    u_VERIFY_FORMAT_BOTH(" {}",  " 42",  (signed char)42);
    u_VERIFY_FORMAT_BOTH("{} ",  "42 ",  (signed char)42);
    u_VERIFY_FORMAT_BOTH(" {} ", " 42 ", (signed char)42);

    u_VERIFY_FORMAT_BOTH("{}",   "42",   (short)42);
    u_VERIFY_FORMAT_BOTH(" {}",  " 42",  (short)42);
    u_VERIFY_FORMAT_BOTH("{} ",  "42 ",  (short)42);
    u_VERIFY_FORMAT_BOTH(" {} ", " 42 ", (short)42);

    u_VERIFY_FORMAT_BOTH("{}",   "42",   42l);
    u_VERIFY_FORMAT_BOTH(" {}",  " 42",  42l);
    u_VERIFY_FORMAT_BOTH("{} ",  "42 ",  42l);
    u_VERIFY_FORMAT_BOTH(" {} ", " 42 ", 42l);

    u_VERIFY_FORMAT_BOTH("{}",   "42",   42ll);
    u_VERIFY_FORMAT_BOTH(" {}",  " 42",  42ll);
    u_VERIFY_FORMAT_BOTH("{} ",  "42 ",  42ll);
    u_VERIFY_FORMAT_BOTH(" {} ", " 42 ", 42ll);

    u_VERIFY_FORMAT_BOTH("{}",   "42",   42u);
    u_VERIFY_FORMAT_BOTH(" {}",  " 42",  42u);
    u_VERIFY_FORMAT_BOTH("{} ",  "42 ",  42u);
    u_VERIFY_FORMAT_BOTH(" {} ", " 42 ", 42u);

    u_VERIFY_FORMAT_BOTH("{}",   "42",   (unsigned char)42);
    u_VERIFY_FORMAT_BOTH(" {}",  " 42",  (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{} ",  "42 ",  (unsigned char)42);
    u_VERIFY_FORMAT_BOTH(" {} ", " 42 ", (unsigned char)42);

    u_VERIFY_FORMAT_BOTH("{}",   "42",   (unsigned short)42);
    u_VERIFY_FORMAT_BOTH(" {}",  " 42",  (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{} ",  "42 ",  (unsigned short)42);
    u_VERIFY_FORMAT_BOTH(" {} ", " 42 ", (unsigned short)42);

    u_VERIFY_FORMAT_BOTH("{}",   "42",   42ul);
    u_VERIFY_FORMAT_BOTH(" {}",  " 42",  42ul);
    u_VERIFY_FORMAT_BOTH("{} ",  "42 ",  42ul);
    u_VERIFY_FORMAT_BOTH(" {} ", " 42 ", 42ul);

    u_VERIFY_FORMAT_BOTH("{}",   "42",   42ull);
    u_VERIFY_FORMAT_BOTH(" {}",  " 42",  42ull);
    u_VERIFY_FORMAT_BOTH("{} ",  "42 ",  42ull);
    u_VERIFY_FORMAT_BOTH(" {} ", " 42 ", 42ull);

    // Sign format specifications

    u_VERIFY_FORMAT_BOTH("{: }", " 42",  42);
    u_VERIFY_FORMAT_BOTH("{: }", " 42",  (signed char)42);
    u_VERIFY_FORMAT_BOTH("{: }", " 42",  (short)42);
    u_VERIFY_FORMAT_BOTH("{: }", " 42",  42l);
    u_VERIFY_FORMAT_BOTH("{: }", " 42",  42ll);
    u_VERIFY_FORMAT_BOTH("{: }", " 42",  42u);
    u_VERIFY_FORMAT_BOTH("{: }", " 42",  (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{: }", " 42",  (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{: }", " 42",  42ul);
    u_VERIFY_FORMAT_BOTH("{: }", " 42",  42ull);

    u_VERIFY_FORMAT_BOTH("{: }", "-42", -42);
    u_VERIFY_FORMAT_BOTH("{: }", "-42", (signed char)-42);
    u_VERIFY_FORMAT_BOTH("{: }", "-42", (short)-42);
    u_VERIFY_FORMAT_BOTH("{: }", "-42", -42l);
    u_VERIFY_FORMAT_BOTH("{: }", "-42", -42ll);

    u_VERIFY_FORMAT_BOTH("{:-}", "42",   42);
    u_VERIFY_FORMAT_BOTH("{:-}", "42",   (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:-}", "42",   (short)42);
    u_VERIFY_FORMAT_BOTH("{:-}", "42",   42l);
    u_VERIFY_FORMAT_BOTH("{:-}", "42",   42ll);
    u_VERIFY_FORMAT_BOTH("{:-}", "42",   42u);
    u_VERIFY_FORMAT_BOTH("{:-}", "42",   (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:-}", "42",   (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:-}", "42",   42ul);
    u_VERIFY_FORMAT_BOTH("{:-}", "42",   42ull);

    u_VERIFY_FORMAT_BOTH("{:-}", "-42", -42);
    u_VERIFY_FORMAT_BOTH("{:-}", "-42", (signed char)-42);
    u_VERIFY_FORMAT_BOTH("{:-}", "-42", (short)-42);
    u_VERIFY_FORMAT_BOTH("{:-}", "-42", -42l);
    u_VERIFY_FORMAT_BOTH("{:-}", "-42", -42ll);

    u_VERIFY_FORMAT_BOTH("{:+}", "+42",  42);
    u_VERIFY_FORMAT_BOTH("{:+}", "+42",  (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:+}", "+42",  (short)42);
    u_VERIFY_FORMAT_BOTH("{:+}", "+42",  42l);
    u_VERIFY_FORMAT_BOTH("{:+}", "+42",  42ll);
    u_VERIFY_FORMAT_BOTH("{:+}", "+42",  42u);
    u_VERIFY_FORMAT_BOTH("{:+}", "+42",  (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:+}", "+42",  (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:+}", "+42",  42ul);
    u_VERIFY_FORMAT_BOTH("{:+}", "+42",  42ull);

    u_VERIFY_FORMAT_BOTH("{:+}", "-42", -42);
    u_VERIFY_FORMAT_BOTH("{:+}", "-42", (signed char)-42);
    u_VERIFY_FORMAT_BOTH("{:+}", "-42", (short)-42);
    u_VERIFY_FORMAT_BOTH("{:+}", "-42", -42l);
    u_VERIFY_FORMAT_BOTH("{:+}", "-42", -42ll);

    // Aligned by constant in format string

    u_VERIFY_FORMAT_BOTH("{:1}",   "42",     42);
    u_VERIFY_FORMAT_BOTH("{:2}",   "42",     42);
    u_VERIFY_FORMAT_BOTH("{:3}",   " 42",    42);
    u_VERIFY_FORMAT_BOTH("{:4}",   "  42",   42);
    u_VERIFY_FORMAT_BOTH("{:04}",  "0042",   42);
    u_VERIFY_FORMAT_BOTH("{:<4}",  "42  ",   42);
    u_VERIFY_FORMAT_BOTH("{:^4}",  " 42 ",   42);
    u_VERIFY_FORMAT_BOTH("{:>4}",  "  42",   42);
    u_VERIFY_FORMAT_BOTH("{:<04}", "42  ",   42);
    u_VERIFY_FORMAT_BOTH("{:^04}", " 42 ",   42);
    u_VERIFY_FORMAT_BOTH("{:>04}", "  42",   42);

    u_VERIFY_FORMAT_BOTH("{:1}",   "42",     (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:2}",   "42",     (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:3}",   " 42",    (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:4}",   "  42",   (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:04}",  "0042",   (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:<4}",  "42  ",   (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:^4}",  " 42 ",   (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:>4}",  "  42",   (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:<04}", "42  ",   (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:^04}", " 42 ",   (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:>04}", "  42",   (signed char)42);

    u_VERIFY_FORMAT_BOTH("{:1}",   "42",     (short)42);
    u_VERIFY_FORMAT_BOTH("{:2}",   "42",     (short)42);
    u_VERIFY_FORMAT_BOTH("{:3}",   " 42",    (short)42);
    u_VERIFY_FORMAT_BOTH("{:4}",   "  42",   (short)42);
    u_VERIFY_FORMAT_BOTH("{:04}",  "0042",   (short)42);
    u_VERIFY_FORMAT_BOTH("{:<4}",  "42  ",   (short)42);
    u_VERIFY_FORMAT_BOTH("{:^4}",  " 42 ",   (short)42);
    u_VERIFY_FORMAT_BOTH("{:>4}",  "  42",   (short)42);
    u_VERIFY_FORMAT_BOTH("{:<04}", "42  ",   (short)42);
    u_VERIFY_FORMAT_BOTH("{:^04}", " 42 ",   (short)42);
    u_VERIFY_FORMAT_BOTH("{:>04}", "  42",   (short)42);

    u_VERIFY_FORMAT_BOTH("{:1}",   "42",     42l);
    u_VERIFY_FORMAT_BOTH("{:2}",   "42",     42l);
    u_VERIFY_FORMAT_BOTH("{:3}",   " 42",    42l);
    u_VERIFY_FORMAT_BOTH("{:4}",   "  42",   42l);
    u_VERIFY_FORMAT_BOTH("{:04}",  "0042",   42l);
    u_VERIFY_FORMAT_BOTH("{:<4}",  "42  ",   42l);
    u_VERIFY_FORMAT_BOTH("{:^4}",  " 42 ",   42l);
    u_VERIFY_FORMAT_BOTH("{:>4}",  "  42",   42l);
    u_VERIFY_FORMAT_BOTH("{:<04}", "42  ",   42l);
    u_VERIFY_FORMAT_BOTH("{:^04}", " 42 ",   42l);
    u_VERIFY_FORMAT_BOTH("{:>04}", "  42",   42l);

    u_VERIFY_FORMAT_BOTH("{:1}",   "42",     42ll);
    u_VERIFY_FORMAT_BOTH("{:2}",   "42",     42ll);
    u_VERIFY_FORMAT_BOTH("{:3}",   " 42",    42ll);
    u_VERIFY_FORMAT_BOTH("{:4}",   "  42",   42ll);
    u_VERIFY_FORMAT_BOTH("{:04}",  "0042",   42ll);
    u_VERIFY_FORMAT_BOTH("{:<4}",  "42  ",   42ll);
    u_VERIFY_FORMAT_BOTH("{:^4}",  " 42 ",   42ll);
    u_VERIFY_FORMAT_BOTH("{:>4}",  "  42",   42ll);
    u_VERIFY_FORMAT_BOTH("{:<04}", "42  ",   42ll);
    u_VERIFY_FORMAT_BOTH("{:^04}", " 42 ",   42ll);
    u_VERIFY_FORMAT_BOTH("{:>04}", "  42",   42ll);

    u_VERIFY_FORMAT_BOTH("{:1}",   "42",     42u);
    u_VERIFY_FORMAT_BOTH("{:2}",   "42",     42u);
    u_VERIFY_FORMAT_BOTH("{:3}",   " 42",    42u);
    u_VERIFY_FORMAT_BOTH("{:4}",   "  42",   42u);
    u_VERIFY_FORMAT_BOTH("{:04}",  "0042",   42u);
    u_VERIFY_FORMAT_BOTH("{:<4}",  "42  ",   42u);
    u_VERIFY_FORMAT_BOTH("{:^4}",  " 42 ",   42u);
    u_VERIFY_FORMAT_BOTH("{:>4}",  "  42",   42u);
    u_VERIFY_FORMAT_BOTH("{:<04}", "42  ",   42u);
    u_VERIFY_FORMAT_BOTH("{:^04}", " 42 ",   42u);
    u_VERIFY_FORMAT_BOTH("{:>04}", "  42",   42u);

    u_VERIFY_FORMAT_BOTH("{:1}",   "42",     (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:2}",   "42",     (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:3}",   " 42",    (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:4}",   "  42",   (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:04}",  "0042",   (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:<4}",  "42  ",   (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:^4}",  " 42 ",   (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:>4}",  "  42",   (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:<04}", "42  ",   (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:^04}", " 42 ",   (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:>04}", "  42",   (unsigned char)42);

    u_VERIFY_FORMAT_BOTH("{:1}",   "42",     (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:2}",   "42",     (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:3}",   " 42",    (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:4}",   "  42",   (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:04}",  "0042",   (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:<4}",  "42  ",   (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:^4}",  " 42 ",   (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:>4}",  "  42",   (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:<04}", "42  ",   (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:^04}", " 42 ",   (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:>04}", "  42",   (unsigned short)42);

    u_VERIFY_FORMAT_BOTH("{:1}",   "42",     42ul);
    u_VERIFY_FORMAT_BOTH("{:2}",   "42",     42ul);
    u_VERIFY_FORMAT_BOTH("{:3}",   " 42",    42ul);
    u_VERIFY_FORMAT_BOTH("{:4}",   "  42",   42ul);
    u_VERIFY_FORMAT_BOTH("{:04}",  "0042",   42ul);
    u_VERIFY_FORMAT_BOTH("{:<4}",  "42  ",   42ul);
    u_VERIFY_FORMAT_BOTH("{:^4}",  " 42 ",   42ul);
    u_VERIFY_FORMAT_BOTH("{:>4}",  "  42",   42ul);
    u_VERIFY_FORMAT_BOTH("{:<04}", "42  ",   42ul);
    u_VERIFY_FORMAT_BOTH("{:^04}", " 42 ",   42ul);
    u_VERIFY_FORMAT_BOTH("{:>04}", "  42",   42ul);

    // Aligned by additional argument

    u_VERIFY_FORMAT_BOTH("{:0{}}",  "0042", 42, 4);
    u_VERIFY_FORMAT_BOTH("{:<{}}",  "42  ", 42, 4);
    u_VERIFY_FORMAT_BOTH("{:^{}}",  " 42 ", 42, 4);
    u_VERIFY_FORMAT_BOTH("{:>{}}",  "  42", 42, 4);

    u_VERIFY_FORMAT_BOTH("{:0{}}",  "0042", (signed char)42, 4);
    u_VERIFY_FORMAT_BOTH("{:<{}}",  "42  ", (signed char)42, 4);
    u_VERIFY_FORMAT_BOTH("{:^{}}",  " 42 ", (signed char)42, 4);
    u_VERIFY_FORMAT_BOTH("{:>{}}",  "  42", (signed char)42, 4);

    u_VERIFY_FORMAT_BOTH("{:0{}}",  "0042",   (short)42, 4);
    u_VERIFY_FORMAT_BOTH("{:<{}}",  "42  ",   (short)42, 4);
    u_VERIFY_FORMAT_BOTH("{:^{}}",  " 42 ",   (short)42, 4);
    u_VERIFY_FORMAT_BOTH("{:>{}}",  "  42",   (short)42, 4);

    u_VERIFY_FORMAT_BOTH("{:0{}}",  "0042",   42l, 4);
    u_VERIFY_FORMAT_BOTH("{:<{}}",  "42  ",   42l, 4);
    u_VERIFY_FORMAT_BOTH("{:^{}}",  " 42 ",   42l, 4);
    u_VERIFY_FORMAT_BOTH("{:>{}}",  "  42",   42l, 4);

    u_VERIFY_FORMAT_BOTH("{:0{}}",  "0042",   42ll, 4);
    u_VERIFY_FORMAT_BOTH("{:<{}}",  "42  ",   42ll, 4);
    u_VERIFY_FORMAT_BOTH("{:^{}}",  " 42 ",   42ll, 4);
    u_VERIFY_FORMAT_BOTH("{:>{}}",  "  42",   42ll, 4);

    u_VERIFY_FORMAT_BOTH("{:0{}}",  "0042",   42u, 4);
    u_VERIFY_FORMAT_BOTH("{:<{}}",  "42  ",   42u, 4);
    u_VERIFY_FORMAT_BOTH("{:^{}}",  " 42 ",   42u, 4);
    u_VERIFY_FORMAT_BOTH("{:>{}}",  "  42",   42u, 4);

    u_VERIFY_FORMAT_BOTH("{:0{}}",  "0042",   (unsigned char)42, 4);
    u_VERIFY_FORMAT_BOTH("{:<{}}",  "42  ",   (unsigned char)42, 4);
    u_VERIFY_FORMAT_BOTH("{:^{}}",  " 42 ",   (unsigned char)42, 4);
    u_VERIFY_FORMAT_BOTH("{:>{}}",  "  42",   (unsigned char)42, 4);

    u_VERIFY_FORMAT_BOTH("{:0{}}",  "0042",   (unsigned short)42, 4);
    u_VERIFY_FORMAT_BOTH("{:<{}}",  "42  ",   (unsigned short)42, 4);
    u_VERIFY_FORMAT_BOTH("{:^{}}",  " 42 ",   (unsigned short)42, 4);
    u_VERIFY_FORMAT_BOTH("{:>{}}",  "  42",   (unsigned short)42, 4);

    u_VERIFY_FORMAT_BOTH("{:0{}}",  "0042",   42ul, 4);
    u_VERIFY_FORMAT_BOTH("{:<{}}",  "42  ",   42ul, 4);
    u_VERIFY_FORMAT_BOTH("{:^{}}",  " 42 ",   42ul, 4);
    u_VERIFY_FORMAT_BOTH("{:>{}}",  "  42",   42ul, 4);

    u_VERIFY_FORMAT_BOTH("{1:^{0}}",  " 42 ", 4, 42);
    u_VERIFY_FORMAT_BOTH("{1:^{0}}",  " 42 ", 4, (signed char)42);
    u_VERIFY_FORMAT_BOTH("{1:^{0}}",  " 42 ", 4, (short)42);
    u_VERIFY_FORMAT_BOTH("{1:^{0}}",  " 42 ", 4, 42l);
    u_VERIFY_FORMAT_BOTH("{1:^{0}}",  " 42 ", 4, 42ll);
    u_VERIFY_FORMAT_BOTH("{1:^{0}}",  " 42 ", 4, 42u);
    u_VERIFY_FORMAT_BOTH("{1:^{0}}",  " 42 ", 4, (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{1:^{0}}",  " 42 ", 4, (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{1:^{0}}",  " 42 ", 4, 42ul);

    // Explicitly specified presentations

    u_VERIFY_FORMAT_BOTH("{:d}", "42", 42);
    u_VERIFY_FORMAT_BOTH("{:d}", "42", (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:d}", "42", (short)42);
    u_VERIFY_FORMAT_BOTH("{:d}", "42", 42l);
    u_VERIFY_FORMAT_BOTH("{:d}", "42", 42ll);
    u_VERIFY_FORMAT_BOTH("{:d}", "42", 42u);
    u_VERIFY_FORMAT_BOTH("{:d}", "42", (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:d}", "42", (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:d}", "42", 42ul);

    u_VERIFY_FORMAT_BOTH("{:c}", "*", 42);
    u_VERIFY_FORMAT_BOTH("{:c}", "*", (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:c}", "*", (short)42);
    u_VERIFY_FORMAT_BOTH("{:c}", "*", 42l);
    u_VERIFY_FORMAT_BOTH("{:c}", "*", 42ll);
    u_VERIFY_FORMAT_BOTH("{:c}", "*", 42u);
    u_VERIFY_FORMAT_BOTH("{:c}", "*", (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:c}", "*", (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:c}", "*", 42ul);

    u_VERIFY_FORMAT_BOTH("{:b}", "101010", 42);
    u_VERIFY_FORMAT_BOTH("{:b}", "101010", (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:b}", "101010", (short)42);
    u_VERIFY_FORMAT_BOTH("{:b}", "101010", 42l);
    u_VERIFY_FORMAT_BOTH("{:b}", "101010", 42ll);
    u_VERIFY_FORMAT_BOTH("{:b}", "101010", 42u);
    u_VERIFY_FORMAT_BOTH("{:b}", "101010", (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:b}", "101010", (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:b}", "101010", 42ul);

    u_VERIFY_FORMAT_BOTH("{:B}", "101010", 42);
    u_VERIFY_FORMAT_BOTH("{:B}", "101010", (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:B}", "101010", (short)42);
    u_VERIFY_FORMAT_BOTH("{:B}", "101010", 42l);
    u_VERIFY_FORMAT_BOTH("{:B}", "101010", 42ll);
    u_VERIFY_FORMAT_BOTH("{:B}", "101010", 42u);
    u_VERIFY_FORMAT_BOTH("{:B}", "101010", (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:B}", "101010", (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:B}", "101010", 42ul);

    u_VERIFY_FORMAT_BOTH("{:o}", "52", 42);
    u_VERIFY_FORMAT_BOTH("{:o}", "52", (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:o}", "52", (short)42);
    u_VERIFY_FORMAT_BOTH("{:o}", "52", 42l);
    u_VERIFY_FORMAT_BOTH("{:o}", "52", 42ll);
    u_VERIFY_FORMAT_BOTH("{:o}", "52", 42u);
    u_VERIFY_FORMAT_BOTH("{:o}", "52", (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:o}", "52", (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:o}", "52", 42ul);

    u_VERIFY_FORMAT_BOTH("{:x}", "2a", 42);
    u_VERIFY_FORMAT_BOTH("{:x}", "2a", (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:x}", "2a", (short)42);
    u_VERIFY_FORMAT_BOTH("{:x}", "2a", 42l);
    u_VERIFY_FORMAT_BOTH("{:x}", "2a", 42ll);
    u_VERIFY_FORMAT_BOTH("{:x}", "2a", 42u);
    u_VERIFY_FORMAT_BOTH("{:x}", "2a", (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:x}", "2a", (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:x}", "2a", 42ul);

    u_VERIFY_FORMAT_BOTH("{:X}", "2A", 42);
    u_VERIFY_FORMAT_BOTH("{:X}", "2A", (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:X}", "2A", (short)42);
    u_VERIFY_FORMAT_BOTH("{:X}", "2A", 42l);
    u_VERIFY_FORMAT_BOTH("{:X}", "2A", 42ll);
    u_VERIFY_FORMAT_BOTH("{:X}", "2A", 42u);
    u_VERIFY_FORMAT_BOTH("{:X}", "2A", (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:X}", "2A", (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:X}", "2A", 42ul);

    // Alternate form with prefix

    u_VERIFY_FORMAT_BOTH("{:#b}", "0b101010", 42);
    u_VERIFY_FORMAT_BOTH("{:#b}", "0b101010", (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:#b}", "0b101010", (short)42);
    u_VERIFY_FORMAT_BOTH("{:#b}", "0b101010", 42l);
    u_VERIFY_FORMAT_BOTH("{:#b}", "0b101010", 42ll);
    u_VERIFY_FORMAT_BOTH("{:#b}", "0b101010", 42u);
    u_VERIFY_FORMAT_BOTH("{:#b}", "0b101010", (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:#b}", "0b101010", (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:#b}", "0b101010", 42ul);

    u_VERIFY_FORMAT_BOTH("{:#B}", "0B101010", 42);
    u_VERIFY_FORMAT_BOTH("{:#B}", "0B101010", (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:#B}", "0B101010", (short)42);
    u_VERIFY_FORMAT_BOTH("{:#B}", "0B101010", 42l);
    u_VERIFY_FORMAT_BOTH("{:#B}", "0B101010", 42ll);
    u_VERIFY_FORMAT_BOTH("{:#B}", "0B101010", 42u);
    u_VERIFY_FORMAT_BOTH("{:#B}", "0B101010", (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:#B}", "0B101010", (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:#B}", "0B101010", 42ul);

    u_VERIFY_FORMAT_BOTH("{:#o}", "052", 42);
    u_VERIFY_FORMAT_BOTH("{:#o}", "052", (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:#o}", "052", (short)42);
    u_VERIFY_FORMAT_BOTH("{:#o}", "052", 42l);
    u_VERIFY_FORMAT_BOTH("{:#o}", "052", 42ll);
    u_VERIFY_FORMAT_BOTH("{:#o}", "052", 42u);
    u_VERIFY_FORMAT_BOTH("{:#o}", "052", (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:#o}", "052", (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:#o}", "052", 42ul);

    u_VERIFY_FORMAT_BOTH("{:#o}", "0", 0);
    u_VERIFY_FORMAT_BOTH("{:#o}", "0", (signed char)0);
    u_VERIFY_FORMAT_BOTH("{:#o}", "0", (short)0);
    u_VERIFY_FORMAT_BOTH("{:#o}", "0", 0l);
    u_VERIFY_FORMAT_BOTH("{:#o}", "0", 0ll);
    u_VERIFY_FORMAT_BOTH("{:#o}", "0", 0u);
    u_VERIFY_FORMAT_BOTH("{:#o}", "0", (unsigned char)0);
    u_VERIFY_FORMAT_BOTH("{:#o}", "0", (unsigned short)0);
    u_VERIFY_FORMAT_BOTH("{:#o}", "0", 0ul);

    u_VERIFY_FORMAT_BOTH("{:#x}", "0x2a", 42);
    u_VERIFY_FORMAT_BOTH("{:#x}", "0x2a", (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:#x}", "0x2a", (short)42);
    u_VERIFY_FORMAT_BOTH("{:#x}", "0x2a", 42l);
    u_VERIFY_FORMAT_BOTH("{:#x}", "0x2a", 42ll);
    u_VERIFY_FORMAT_BOTH("{:#x}", "0x2a", 42u);
    u_VERIFY_FORMAT_BOTH("{:#x}", "0x2a", (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:#x}", "0x2a", (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:#x}", "0x2a", 42ul);

    u_VERIFY_FORMAT_BOTH("{:#X}", "0X2A", 42);
    u_VERIFY_FORMAT_BOTH("{:#X}", "0X2A", (signed char)42);
    u_VERIFY_FORMAT_BOTH("{:#X}", "0X2A", (short)42);
    u_VERIFY_FORMAT_BOTH("{:#X}", "0X2A", 42l);
    u_VERIFY_FORMAT_BOTH("{:#X}", "0X2A", 42ll);
    u_VERIFY_FORMAT_BOTH("{:#X}", "0X2A", 42u);
    u_VERIFY_FORMAT_BOTH("{:#X}", "0X2A", (unsigned char)42);
    u_VERIFY_FORMAT_BOTH("{:#X}", "0X2A", (unsigned short)42);
    u_VERIFY_FORMAT_BOTH("{:#X}", "0X2A", 42ul);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;  (void) veryVerbose;
        veryVeryVerbose = argc > 4;  (void) veryVeryVerbose;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLES"
                          "\n==============");

///Example 1: Simple Integer Formatting
/// - - - - - - - - - - - - - - - - - -
// Formatters for fundamental types are already defined, so to output such
// objects the `bsl::format` function can be used in exactly the same way as
// the original one from the `stl` library:
// ```
    int         value = 99;
    bsl::string res   = bsl::format("{:#06x}", value);

    ASSERT(bsl::string("0x0063") == res);
// ```
//
///Example 2: Creating a Custom Formatter For User Defined Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Finally, we create a `Date` object, output it to the string and verify the
// result:
// ```
    Date        date(1999, 10, 23);
    bsl::string result = bsl::format("{:v}", date);
    ASSERT(bsl::string("23 October 1999") == result);

    result = bsl::format("{:N}", date);
    ASSERT(bsl::string("1999-10-23") == result);
// ```
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING EXAMPLES IN BDE DOCUMENTATION
        //   Website `https://bde.bloomberg.com` contains several usage
        //   examples showing how to use `bsl::format`.  We want to make sure
        //   that these examples compile and produce the expected output.
        //   Full page link:
        //   https://bde.bloomberg.com/bde/articles/bslfmt.html
        //
        // Concerns:
        // 1. The usage examples provided on the BDE website compile, link, and
        //    run as shown.
        //
        // Plan:
        // 1. Incorporate usage examples from the website into test driver,
        //     remove comments, and replace `cout` outputs with `ASSERT`.
        //     (C-1)
        //
        // Testing:
        //   EXAMPLES IN BDE DOCUMENTATION
        // --------------------------------------------------------------------

        if (verbose)
                puts("\nTESTING EXAMPLES IN BDE DOCUMENTATION"
                     "\n=====================================");

        bsl::string result;

        // Simple Example

        {
            int x = 42;

            result = bsl::format("The value of {} is {}\n", "x", x);
            ASSERTV(result.c_str(), "The value of x is 42\n" == result);

            result = bsl::format("Escaped left {{ and right }} braces\n");
            ASSERTV(result.c_str(),
                    "Escaped left { and right } braces\n" == result);

            result = bsl::format("Print a value in braces {{{}}}", x);
            ASSERTV(result.c_str(), "Print a value in braces {42}" == result);
        }

        // Positional Placeholders

        {
            double value = 3.14159265359;
            int width = 8, precision = 3;

            result =
                    bsl::format("Pi is {2:{0}.{1}f}", width, precision, value);
            // Note: The numbers in the braces state which arguments to use
            ASSERTV(result.c_str(), "Pi is    3.142" == result);

            result = bsl::format("Pi is {:{}.{}f}", value, width, precision);
            // Note: The arguments are in the order of the (unescaped) opening
            // braces '{'
            ASSERTV(result.c_str(), "Pi is    3.142" == result);
        }

        // Formatting Specifications

        {
            double value = 3.14159265359;

            result = bsl::format("Pi is {}", value);
            ASSERTV(result.c_str(), "Pi is 3.14159265359" == result);

            result = bsl::format("Pi is {:f} default precision", value);
            ASSERTV(result.c_str(),
                    "Pi is 3.141593 default precision" == result);

            result = bsl::format("Pi is {0:f} default precision", value);
            ASSERTV(result.c_str(),
                    "Pi is 3.141593 default precision" == result);

            result = bsl::format("Pi is {:.4f} smaller precision", value);
            ASSERTV(result.c_str(),
                    "Pi is 3.1416 smaller precision" == result);

            result = bsl::format("Pi is {:.4e} exponential", value);
            ASSERTV(result.c_str(), "Pi is 3.1416e+00 exponential" == result);

            result = bsl::format("Pi is {:20.4e} in a wide field", value);
            ASSERTV(result.c_str(),
                    "Pi is           3.1416e+00 in a wide field" == result);
        }
        {
            int day = 23, month = 1, year = 24;

            result = bsl::format(
                             "The date in US format is {1:02}/{0:02}/{2:02}\n",
                             day,
                             month,
                             year);
            ASSERTV(result.c_str(),
                    "The date in US format is 01/23/24\n" == result);

            result = bsl::format(
                             "The date in UK format is {0:02}/{1:02}/{2:02}\n",
                             day,
                             month,
                             year);
            ASSERTV(result.c_str(),
                    "The date in UK format is 23/01/24\n" == result);
        }
        {
            double value = 3.14159265359;

            result = bsl::format("Pi to the left is   '{:<20f}'", value);
            ASSERTV(result.c_str(),
                    "Pi to the left is   '3.141593            '" == result);

            result = bsl::format("Pi to the right is  '{:>20f}'", value);
            ASSERTV(result.c_str(),
                    "Pi to the right is  '            3.141593'" == result);

            result = bsl::format("Pi in the middle is '{:^20f}'", value);
            ASSERTV(result.c_str(),
                    "Pi in the middle is '      3.141593      '" == result);

            result = bsl::format("Pi to the left is   '{:=<20f}'", value);
            ASSERTV(result.c_str(),
                    "Pi to the left is   '3.141593============'" == result);

            result = bsl::format("Pi to the right is  '{:=>20f}'", value);
            ASSERTV(result.c_str(),
                    "Pi to the right is  '============3.141593'" == result);

            result = bsl::format("Pi in the middle is '{:=^20f}'", value);
            ASSERTV(result.c_str(),
                    "Pi in the middle is '======3.141593======'" == result);
        }

        // Runtime Defined Format Strings

        {
            int value = 42;
            string spec = "Value is {}\n";

            // The following won't compile:
            // result = format(spec, value);

            // Need to use this instead:
            result = vformat(spec, make_format_args(value));
            ASSERTV(result.c_str(), "Value is 42\n" == result);

            // Note the following would also give a compile-time error:
            // result = vformat(spec, make_format_args(42));
            // (arguments have to be lvalues)
        }

        {
            int    value = 42;
            string spec = "Value is {:z}\n"; // Invalid format spec for in

            try {
                result = bsl::vformat(spec, make_format_args(value));
                ASSERTV(false);
            }
            catch (const format_error&) {
            }
        }

        // Support for Allocators

        {
            bslma::TestAllocator ta("test");

            ASSERT(ta.numAllocations() == 0);

            bsl::string testStr = bsl::format(
                    &ta,
                    "Here we do a test with a long string and some number {}",
                    42);

            ASSERT(testStr.get_allocator().mechanism() == &ta);
            ASSERT(ta.numAllocations() > 0);
        }

        // Basic rules for formatting user defined types

        {
            bsl::formatter<MyType, char> formatter;
            (void) formatter;  // suppress compiler warning
        }

        // Example code for a fully-fledged formatter for a user defined type
        {
            Complex c = {2.71828182845, 3.14159265359};

            result = bsl::format("Value is {}", c);
            ASSERTV(result.c_str(),
                    "Value is 2.71828182845+3.14159265359i" == result);

            result = bsl::format("Value is {0:}", c);
            ASSERTV(result.c_str(),
                    "Value is 2.71828182845+3.14159265359i" == result);

            result = bsl::format("Value is {::}", c);
            ASSERTV(result.c_str(),
                    "Value is 2.71828182845+3.14159265359i" == result);

            result = bsl::format("Value is {:j:}", c);
            ASSERTV(result.c_str(),
                    "Value is 2.71828182845+3.14159265359j" == result);

            result = bsl::format("Value is {:j:.2f}", c);
            ASSERTV(result.c_str(), "Value is 2.72+3.14j" == result);

            bsl::wstring wresult = bsl::format(L"Value is {:j:}", c);
            ASSERTV(L"Value is 2.71828182845+3.14159265359j" == wresult);

        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // BAD FORMAT STRINGS
        //
        // Concerns:
        // 1. Too many format placeholders cause an error (not enough arguments
        //    to format).
        // --------------------------------------------------------------------
        testCase12();
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // ARGUMENT PASSING
        //
        // Concerns:
        // 1. Arguments are passed to the implementation.  Note that this test
        //    case works in conjunction of the rest of the test driver, not all
        //    parameters are directly verified here.
        //
        // 2. The locale argument is passed to the standard implementation.
        // --------------------------------------------------------------------
        testCase11();
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // VFORMAT, VFORMAT_TO
        //
        // Concerns:
        // 1. `vformat`, `vformat_t` calls format as expected.
        //
        // 2. `vformat` uses the specified allocator for the returned string.
        // --------------------------------------------------------------------
        testCase10();
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COMBINATIONS
        //
        // Concerns:
        // 1. Mixed kinds (types) of arguments result in the invocation of
        //    differently configured parser that have to all do proper parsing
        //    of portions of the format string.
        // --------------------------------------------------------------------
        testCase9();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // POINTERS
        //
        // Concerns:
        // 1. `nullptr` (if supported) and `bsl::nullptr_t()` are printed as
        //    0x0.
        //
        // 2. Pointer values are printed as hexadecimal addresses with `0x`
        //    prefix.
        //
        // 3. Zero padding places the right number of zeros after the prefix.
        //
        // 4. Width and alignment combinations work as expected.
        //
        // 5. Width may be specified as an argument, without or with arg id.
        //
        // 6. Presentation may be specified as 'p' and it results in the same
        //    output as no presentation format specified.
        //
        // 7. Presentation may be specified as 'P' and it results in the prefix
        //    changing to "0X" and the hexadecimal output being uppercase.
        //
        // 8. `char` and `wchar_t` formatting both work.
        // --------------------------------------------------------------------
        testCase8();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // BOOLEAN
        //
        // Concerns:
        // 1. Static text in format string is copied to the output unmodified.
        //
        // 2. Width specification with and without alignment works according to
        //    the standard requirements in its direct form as well as when
        //    width is specified by an argument (embedded).
        //
        // 3. Format specification characters have the intended result.
        // --------------------------------------------------------------------
        testCase7();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // STRINGS
        //
        // Concerns:
        // 1. All supported string types print their value properly.
        //
        // 2. Width specification with and without alignment works according to
        //    the standard requirements in its direct form as well as when
        //    width is specified by an argument (embedded).
        //
        // 3. The 's' format specification character may be used.
        //
        // 4. A "precision" specification ('.' followed by an integer)
        //   truncates the printed string.
        // --------------------------------------------------------------------
        testCase6();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // FLOAT AND DOUBLE
        //
        // Concerns:
        // 1. Both supported types ('double', and 'float') print their values
        //    properly.
        //
        // 2. Sign format specification works as intended (' ', '+', '-').
        //
        // 3. Width specification with and without alignment works according to
        //    the standard requirements in its direct form as well as when
        //    width is specified by an argument (embedded).
        //
        // 4. Presentation format specifiers (characters) print using the
        //    specified standard presentation format.
        //
        // 5. Precision specification works as per standard in all presentation
        //    formats including the default.
        //
        // 6. Alternate formats work as per standard.
        // --------------------------------------------------------------------
        testCase5();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CHAR AND WCHAR_T
        //
        // Concerns:
        // 1. Characters and wide characters are printed (by default) as a
        //    string that consists of that character only.
        //
        // 2. Width specification with and without alignment works according to
        //    the standard requirements in its direct form as well as when
        //    width is specified by an argument (embedded).
        //
        // 3. Presentation format specifiers (characters) work as per standard.
        //
        // 4. Alternate format adds the format prefixes in the appropriate
        //    presentation formats as per standard.
        // --------------------------------------------------------------------
        testCase4();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // INTEGERS
        //
        // Concerns:
        // 1. All integers types are converted to string properly.
        //
        // 2. Sign format specification works as intended (' ', '+', '-').
        //
        // 3. Width specification with and without alignment works according to
        //    the standard requirements in its direct form as well as when
        //    width is specified by an argument (embedded).
        //
        // 4. Presentation format specifiers (characters) print using the
        //    specified standard presentation format.
        //
        // 5. Alternate formats work as per standard.
        // --------------------------------------------------------------------
        testCase3();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CLASSES AND CLASS TEMPLATES
        //
        // Concerns:
        // 1. The standard-mandated types and class templates exist in the
        //    `bsl` namespace.
        //
        // 2. Where possible (without significant test infrastructure) verify
        //    that the type/template behaves as expected.
        //
        // Plan:
        // 1. Verify the existence of names by creating variables or typedefs
        //    that create an instance of a template (and possibly a variable).
        //
        // 2. Verify functionality that is not directly or indirectly verified
        //    by other tests, or those that are very easy to do (like
        //    `format_to_n_result`).
        //
        // Testing:
        //   CONCERN: CLASSES AND CLASS TEMPLATES
        // --------------------------------------------------------------------

        if (verbose) puts("\nCLASSES AND CLASS TEMPLATES"
                          "\n===========================");

        // `basic_format_context` cannot be directly created since it is
        // unspecified what is their first template argument, and the main
        // class template of standard library implementations is not
        // necessarily defined, only those specializations that may only be
        // created indirectly by `make_format_args` and `make_wformat_args`.

        // `basic_format_args`, `format_context`, `wformat_context`
        ASSERT((bsl::is_same<
                    bsl::format_args,
                    bsl::basic_format_args<bsl::format_context> >::value));
        ASSERT((bsl::is_same<
                    bsl::wformat_args,
                    bsl::basic_format_args<bsl::wformat_context> >::value));

        // `basic_format_args`
        // `basic_format_arg`
        int   i = 42;
        float f = 3.14f;
        verifyArgAndArgs(L_, bsl::make_format_args(i, f));
        verifyArgAndArgs(L_, bsl::make_wformat_args(i, f));

        // `bsl::basic_format_parse_context`
        // `bsl::format_parse_context`
        // `bsl::wformat_parse_context`
        bsl::basic_format_parse_context<char> bfpc("{} {}", 2);
        (void)bfpc;
        bsl::basic_format_parse_context<wchar_t> wbfpc(L"{} {}", 2);
        (void)wbfpc;

        ASSERT((bsl::is_same<bsl::basic_format_parse_context<char>,
                             bsl::format_parse_context>::value));
        ASSERT((bsl::is_same<bsl::basic_format_parse_context<wchar_t>,
                             bsl::wformat_parse_context>::value));

        // `bsl::basic_format_string`
        // `bsl::format_string`
        // `bsl::wformat_string`
#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&                 \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
        bsl::basic_format_string<char, int, int> bfs("{} {}");
        (void)bfs;
        bsl::basic_format_string<wchar_t, int, int> wbfs(L"{} {}"); (void)wbfs;

        ASSERT((bsl::is_same<bsl::basic_format_string<char, int, int>,
                             bsl::format_string<int, int> >::value));
        ASSERT((bsl::is_same<bsl::basic_format_string<wchar_t, int, int>,
                             bsl::wformat_string<int, int> >::value));
#endif  // C++11 or above (essentially)

        // `bsl::formatter` is once verified by the formattable types we use in
        // this test driver (by specializing it).  In addition to that we
        // verify here that the expected standard specializations do exist:

        // formatters for character types
        bsl::formatter<char, char>       fmtrcc;    (void)fmtrcc;
        bsl::formatter<char, wchar_t>    fmtrcwc;   (void)fmtrcwc;
        bsl::formatter<wchar_t, wchar_t> fmtrwcwc;  (void)fmtrwcwc;

        // formatters for string types
        bsl::formatter<char*, char>             fmtrcpc;     (void)fmtrcpc;
        bsl::formatter<const char*, char>       fmtrccpc;    (void)fmtrccpc;
        bsl::formatter<wchar_t*, wchar_t>       fmtrwcpwc;   (void)fmtrwcpwc;
        bsl::formatter<const wchar_t*, wchar_t> fmtrcwcpwc;  (void)fmtrcwcpwc;
        // formatters for character arrays are templates on size
        bsl::formatter<std::string, char>     fmtrssc;    (void)fmtrssc;
        bsl::formatter<std::wstring, wchar_t> fmtrwsswc;  (void)fmtrwsswc;
        bsl::formatter<bsl::string, char>     fmtrbsc;    (void)fmtrbsc;
        bsl::formatter<bsl::wstring, wchar_t> fmtrwbswc;  (void)fmtrwbswc;

#ifdef u_STD_STRING_VIEW_EXISTS
        bsl::formatter<std::string_view, char>     fmtrsvc;    (void)fmtrsvc;
        bsl::formatter<std::wstring_view, wchar_t> fmtrwsvwc;  (void)fmtrwsvwc;
        bsl::formatter<bsl::string_view, char>     fmtrbvc;    (void)fmtrbvc;
        bsl::formatter<bsl::wstring_view, wchar_t> fmtrwbvwc;  (void)fmtrwbvwc;
#endif  // u_STD_STRING_VIEW_EXISTS

        // formatters for arithmetic types
        bsl::formatter<bool, char>    fmtrbc;  (void)fmtrbc;
        bsl::formatter<bool, wchar_t> fmtrbwc; (void)fmtrbwc;

        bsl::formatter<signed char, char>      fmtrscc;  (void)fmtrscc;
        bsl::formatter<signed char, wchar_t>   fmtrscwc; (void)fmtrscwc;
        bsl::formatter<unsigned char, char>    fmtrucc;  (void)fmtrucc;
        bsl::formatter<unsigned char, wchar_t> fmtrucwc; (void)fmtrucwc;

        bsl::formatter<short, char>             fmtrhc;   (void)fmtrhc;
        bsl::formatter<short, wchar_t>          fmtrhwc;  (void)fmtrhwc;
        bsl::formatter<unsigned short, char>    fmtruhc;  (void)fmtruhc;
        bsl::formatter<unsigned short, wchar_t> fmtruhwc; (void)fmtruhwc;

        bsl::formatter<int,      char>    fmtrsic;  (void)fmtrsic;
        bsl::formatter<int,      wchar_t> fmtrsiwc; (void)fmtrsiwc;
        bsl::formatter<unsigned, char>    fmtruic;  (void)fmtruic;
        bsl::formatter<unsigned, wchar_t> fmtruiwc; (void)fmtruiwc;

        bsl::formatter<long,          char>    fmtrslc;  (void)fmtrslc;
        bsl::formatter<long,          wchar_t> fmtrslwc; (void)fmtrslwc;
        bsl::formatter<unsigned long, char>    fmtrulc;  (void)fmtrulc;
        bsl::formatter<unsigned long, wchar_t> fmtrulwc; (void)fmtrulwc;

        bsl::formatter<long long,          char>    fmtrsllc;  (void)fmtrsllc;
        bsl::formatter<long long,          wchar_t> fmtrsllwc; (void)fmtrsllwc;
        bsl::formatter<unsigned long long, char>    fmtrullc;  (void)fmtrullc;
        bsl::formatter<unsigned long long, wchar_t> fmtrullwc; (void)fmtrullwc;

        // `bsl::format_error`
        {  // `char *` initializer
            const char        *WHAT = "Just an error.";
            bsl::format_error  err(WHAT);
            ASSERTV(err.what(), 0 == strcmp(err.what(), WHAT));
        }
        {  // `bsl::string` initializer
            bsl::string       WHAT("Just an error.");
            bsl::format_error err(WHAT);
            ASSERTV(err.what(), WHAT == err.what());
        }
        {  // `std::string` initializer
            std::string       WHAT("Just an error.");
            bsl::format_error err(WHAT);
            ASSERTV(err.what(), WHAT == err.what());
        }

        // `bsl::format_to_n_result`
        {
            char                            buff[32];
            bsl::format_to_n_result<char *> ftnr = {buff + 12, 12};
            ASSERTV((const void *)(buff + 12), (const void *)ftnr.out,
                    (const void *)(buff + 12) == (const void *)ftnr.out);
            ASSERTV(ftnr.size, 12 == ftnr.size);
        }
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
        // 1. TBD
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");
        {
#if BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
            std::formatter<int, char>              dummy1a;  (void)dummy1a;
            std::formatter<bsl::string, char>      dummy1b;  (void)dummy1b;
            std::formatter<bsl::wstring, wchar_t>  dummy1c;  (void)dummy1c;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

            bsl::formatter<int, char>              dummy2a;  (void)dummy2a;
            bsl::formatter<bsl::string, char>      dummy2b;  (void)dummy2b;
            bsl::formatter<bsl::wstring, wchar_t>  dummy2c;  (void)dummy2c;
            bsl::formatter<bsl::string_view, char> dummy2d;  (void)dummy2d;
            bsl::formatter<std::string, char>      dummy2e;  (void)dummy2e;
            bsl::formatter<std::wstring, wchar_t>  dummy2f;  (void)dummy2f;
#if defined(BSLSTL_STRING_VIEW_AND_STD_STRING_VIEW_COEXIST) ||                \
    defined(BSLSTL_STRING_VIEW_IS_ALIASED)
            bsl::formatter<std::string_view, char>     dummy2g;  (void)dummy2g;
            bsl::formatter<std::wstring_view, wchar_t> dummy2h;  (void)dummy2h;
#endif  // Standard string views exist
        }

        // -----------------------------------------------------------
        // simple test to see if we can pass in string and string_view
        {
            const std::string V1("Test 1");
            const bsl::string V2("Test 2");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
#if !defined(_GLIBCXX_RELEASE) || _GLIBCXX_RELEASE >= 14
             const char8_t *fmt = u8"{0:\U0001F600<4}";

             int intValue = 42;

             std::string rv1 = std::vformat((const char *)fmt,
                                            std::make_format_args(intValue));

             bsl::string rv2 = bslfmt::vformat((const char *)fmt,
                                         bslfmt::make_format_args(intValue));

             ASSERT(rv1 == rv2);
             ASSERT(rv1 == (const char *)u8"42\U0001F600\U0001F600");
#endif  // not GNU C++ Library, or 14.0 or newer release GNU C++ Library
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
            const std::string_view v3("Test 3");
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
            const bsl::string_view v4("Test 4");

            check(bsl::format("{}", V1), "Test 1");
            check(bsl::format("{}", V2), "Test 2");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
            check(bsl::format("{}", v3), "Test 3");
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
            check(bsl::format("{}", v4), "Test 4");

#if BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
            ASSERT(std::format("{}", V1) == bsl::string("Test 1"));
            ASSERT(std::format("{}", V2) == bsl::string("Test 2"));

            ASSERT(std::format("{}", v3) == bsl::string("Test 3"));
            ASSERT(std::format("{}", v4) == bsl::string("Test 4"));
#endif  //  BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

        }

        // -------------------------------
        // simple test of format with char
        {
            const bsl::string  intro  =  "Here is a simple equation";
            const int          x      = 1;
            const int          y      = 2;
            const int          sum    = x + y;

            check(bsl::format(L"{}", x), L"1");

            check(bsl::format("{}", y),   "2");
            check(bsl::format("{}", 'x'), "x");
            check(bsl::format("{}: {} + {} = {}", intro, x, y, sum),
                  "Here is a simple equation: 1 + 2 = 3");
            check(bsl::format(L"{}", L"Hello World"), L"Hello World");

            DOTESTWITHORACLE("Here is a simple equation: 1 + 2 = 3",
                             "{}: {} + {} = {}",
                             intro,
                             x,
                             y,
                             sum);
            check(bsl::format("{}: {} + {} = {}", intro, x, y, sum),
                  "Here is a simple equation: 1 + 2 = 3");
            check(bsl::vformat("{}: {} + {} = {}",
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

            const std::size_t len =
                  bsl::formatted_size("The value of {1:{0}} is {0}", ft.x, ft);
            ASSERTV(len, 38 == len);

#if BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
             ASSERT((std::format("The value of {1:{0}} is {0}", ft.x, ft)
                                 == "The value of FormattableType{37} is 37"));
             ASSERT((std::vformat("The value of {1:{0}} is {0}",
                                  bsl::make_format_args(ft.x, ft))
                                 == "The value of FormattableType{37} is 37"));
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
        }

        // ----------------------------------
        // simple test of format with wchar_t
        {
            const bsl::wstring intro = L"Here is a simple equation";
            const int          x     = 1;
            const int          y     = 2;
            const int          sum   = x + y;

            check(bsl::format(L"{}", x), L"1");
            check(bsl::format(L"{}", y), L"2");

            check(bsl::format(L"{}", (wchar_t)'x'), L"x");

            check(bsl::format(L"{}: {} + {} = {}", intro, x, y, sum),
                  L"Here is a simple equation: 1 + 2 = 3");

            check(bsl::format(L"{}", L"Hello World"), L"Hello World");

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

        // ---------------------------------------
        // Test format of char with wchar_t output
        {
            check(bsl::format(L"{}", (char)'x'), L"x");
        }

        // ---------------------------------------
        // Simple test of formatted_size with char
        {
            const ptrdiff_t count = bsl::formatted_size("{}", "Hello World");
            ASSERT(11 == count);
        }

        // ------------------------------------------
        // Simple test of formatted_size with wchar_t
        {
            const ptrdiff_t count = bsl::formatted_size(L"{}", L"Hello World");
            ASSERT(11 == count);
        }

        // --------------------------------------------------
        // Simple test of format_to with char output iterator
        {
            char  temp2[64];
            char *it = bsl::format_to(temp2, "{}", "Hello World");
            *it = 0;
            check(bsl::string(temp2), "Hello World");

            const format_to_n_result<char *> result =
                               bsl::format_to_n(temp2, 5, "{}", "Hello World");
            *result.out = 0;
            ASSERT(11 == result.size);
            check(bsl::string(temp2), "Hello");
        }

        // -----------------------------------------------------
        // Simple test of format_to with wchar_t output iterator
        {
            wchar_t  temp2[64];
            wchar_t *it = bsl::format_to(temp2, L"{}", L"Hello World");
            *it = 0;
            check(bsl::wstring(temp2), L"Hello World");
            const format_to_n_result<wchar_t *> result =
                             bsl::format_to_n(temp2, 5, L"{}", L"Hello World");
            *result.out = 0;
            ASSERT(11 == result.size);
            check(bsl::wstring(temp2), L"Hello");
        }

        // ------------------------------------------
        // Simple test of vformat_to with char string
        {
            bsl::string temp;
            bsl::vformat_to(&temp, "{}", make_format_args("Hello World"));
            check(temp, "Hello World");
        }

        // ---------------------------------------------
        // Simple test of vformat_to with wchar_t string
        {
            bsl::wstring temp;
            bsl::vformat_to(&temp, L"{}", make_wformat_args(L"Hello World"));
            check(temp, L"Hello World");
        }

        // --------------------------------------------------
        // Simple test of format_to with char output iterator
        {
            char  temp2[64];
            char *it = bsl::vformat_to(temp2,
                                       "{}",
                                       make_format_args("Hello World"));
            *it = 0;
            check(bsl::string(temp2), "Hello World");
        }

        // -----------------------------------------------------
        // Simple test of format_to with wchar_t output iterator
        {
            wchar_t  temp2[64];
            wchar_t *it = bsl::vformat_to(temp2,
                                          L"{}",
                                          make_wformat_args(L"Hello World"));
            *it = 0;
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
