// bslfmt_format.t.cpp                                                -*-C++-*-
#include <bslfmt_format.h>

#include <bslfmt_standardformatspecification.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <limits.h>
#include <wchar.h>
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
//                       HELPER CLASSES AND FUNCTIONS
// ----------------------------------------------------------------------------

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
#else   // ndef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #define u_TESTUTIL_ORACLE_TEST(fmtstr, expected, ...)                       \
      do {                                                                    \
          const std::string stdResult = std::format(fmtstr, __VA_ARGS__);     \
          ASSERTV(stdResult.c_str(), result.c_str(), stdResult == result);    \
                                                                              \
          const size_t stdFmtdSize = std::formatted_size(fmtstr, __VA_ARGS__);\
          ASSERTV(stdFmtdSize, fmtdSize, stdFmtdSize == fmtdSize);            \
                                                                              \
    } while (false)

  #define u_TESTUTIL_ORACLE_WTEST(fmtstr, expected, ...)                      \
      do {                                                                    \
          const std::wstring stdResult = std::format(fmtstr, __VA_ARGS__);    \
          ASSERT(stdResult == result);                                        \
                                                                              \
          const size_t stdFmtdSize = std::formatted_size(fmtstr, __VA_ARGS__);\
          ASSERTV(stdFmtdSize, fmtdSize, stdFmtdSize == fmtdSize);            \
                                                                              \
    } while (false)
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

// TEST CALLS FOR FORMAT

#define u_VERIFY_FORMAT(fmtstr, expected, ...)                                \
    do {                                                                      \
        const bsl::string result = bsl::format(fmtstr, __VA_ARGS__);          \
        ASSERTV(result.c_str(), result == expected);                          \
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


#define u_VERIFY_WFORMAT(fmtstr, expected, ...)                               \
    do {                                                                      \
        const bsl::wstring result = bsl::format(fmtstr, __VA_ARGS__);         \
        ASSERT(result == expected);                                           \
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


#define u_VERIFY_FORMAT_BOTH(fmtstr, expected, ...)                           \
    do {                                                                      \
        u_VERIFY_FORMAT(fmtstr, expected, __VA_ARGS__);                       \
        u_VERIFY_WFORMAT(L##fmtstr, L##expected, __VA_ARGS__);                \
    } while (false)

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

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;  (void) veryVerbose;
    bool veryVeryVerbose = argc > 4;  (void) veryVeryVerbose;

    printf("TEST %s CASE %d \n", __FILE__, test);

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
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

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
      case 8: {
        // --------------------------------------------------------------------
        // COMBINATIONS
        //
        // Concerns:
        // 1. Mixed kinds (types) of arguments result in the invocation of
        //    differently configured parser that have to all do proper parsing
        //    of portions of the format string.
        //
        // Plan:
        // 1. Spot check different combinations by using real-life looking
        //    format strings.
        //
        // Testing:
        //   COMBINATIONS
        // --------------------------------------------------------------------

        if (verbose) puts("\nCOMBINATIONS"
                          "\n============");

        u_VERIFY_FORMAT(
                       "this={:010}, name={:<8}, id={:04x}, heat={:a}",
                       "this=0x00c0ffee, name=PgDown  , id=0020, heat=1.b8p+3",
                       reinterpret_cast<const void *>(0xC0FFEE),
                       "PgDown",
                       32,
                       13.75);

        u_VERIFY_WFORMAT(
                      L"this={:010}, name={:<8}, id={:04x}, heat={:a}",
                      L"this=0x00c0ffee, name=PgDown  , id=0020, heat=1.b8p+3",
                      reinterpret_cast<const void *>(0xC0FFEE),
                      L"PgDown",
                      32,
                      13.75);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // POINTERS
        //
        // Concerns:
        // 1. TBD
        //
        // Plan:
        // 1. TBD
        //
        // Testing:
        //   POINTERS
        // --------------------------------------------------------------------

        if (verbose) puts("\nPOINTERS"
                          "\n========");

        // Verify `nullptr`
#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
        u_VERIFY_FORMAT_BOTH("{}", "0x0", nullptr);
#endif  // C++11 or later
        u_VERIFY_FORMAT_BOTH("{}", "0x0", bsl::nullptr_t());

#define u_VERIFY_POINTER(fmt, res, val) \
    u_VERIFY_FORMAT_BOTH(fmt, res, reinterpret_cast<const void *>(val))

        // Simple printing of pointer values

        u_VERIFY_POINTER("{}", "0x12345", 0x12345);

        // Zero padding

        u_VERIFY_FORMAT_BOTH("{:010}", "0x00000000", bsl::nullptr_t());

        u_VERIFY_POINTER("{:010}", "0x00012345", 0x12345);

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

        u_VERIFY_POINTER("{:p}", "0x12345", 0x12345);

#undef u_VERIFY_POINTER

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // BOOLEAN
        //
        // Concerns:
        // 1. TBD
        //
        // Plan:
        // 1. TBD
        //
        // Testing:
        //   BOOLEAN
        // --------------------------------------------------------------------

        if (verbose) puts("\nBOOLEAN"
                          "\n========");

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
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // STRINGS
        //
        // Concerns:
        // 1. TBD
        //
        // Plan:
        // 1. TBD
        //
        // Testing:
        //   STRINGS
        // --------------------------------------------------------------------

        if (verbose) puts("\nSTRINGS"
                          "\n=======");

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
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // FLOAT AND DOUBLE
        //
        // Concerns:
        // 1. TBD
        //
        // Plan:
        // 1. TBD
        //
        // Testing:
        //   FLOAT AND DOUBLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nFLOAT AND DOUBLE"
                          "\n================");

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
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CHAR AND WCHAR_T
        //
        // Concerns:
        // 1. TBD
        //
        // Plan:
        // 1. TBD
        //
        // Testing:
        //   CHAR AND WCHAR_T
        // --------------------------------------------------------------------

        if (verbose) puts("\nCHAR AND WCHAR_T"
                          "\n================");

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
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // INTEGERS
        //
        // Concerns:
        // 1. TBD
        //
        // Plan:
        // 1. TBD
        //
        // Testing:
        //   INTEGERS
        // --------------------------------------------------------------------

        if (verbose) puts("\nINTEGERS"
                          "\n========");

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

        // Printing of corner case integral values

        u_VERIFY_FORMAT_BOTH("{}",   "-2147483648",            INT_MIN);
        u_VERIFY_FORMAT_BOTH("{}",    "2147483647",            INT_MAX);

        u_VERIFY_FORMAT_BOTH("{}",   "-128",                   SCHAR_MIN);
        u_VERIFY_FORMAT_BOTH("{}",    "127",                   SCHAR_MAX);

        u_VERIFY_FORMAT_BOTH("{}",   "-32768",                 SHRT_MIN);
        u_VERIFY_FORMAT_BOTH("{}",    "32767",                 SHRT_MAX);

        u_VERIFY_FORMAT_BOTH("{}",   "-2147483648",            LONG_MIN);
        u_VERIFY_FORMAT_BOTH("{}",    "2147483647",            LONG_MAX);

        u_VERIFY_FORMAT_BOTH("{}",   "-9223372036854775808",   LLONG_MIN);
        u_VERIFY_FORMAT_BOTH("{}",    "9223372036854775807",   LLONG_MAX);

        u_VERIFY_FORMAT_BOTH("{}",    "4294967295",            UINT_MAX);
        u_VERIFY_FORMAT_BOTH("{}",    "255",                   UCHAR_MAX);
        u_VERIFY_FORMAT_BOTH("{}",    "65535",                 USHRT_MAX);
        u_VERIFY_FORMAT_BOTH("{}",    "4294967295",            ULONG_MAX);
        u_VERIFY_FORMAT_BOTH("{}",    "18446744073709551615",  ULLONG_MAX);

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
