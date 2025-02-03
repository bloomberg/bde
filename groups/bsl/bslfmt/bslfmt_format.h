// bslfmt_format.h                                                    -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMAT
#define INCLUDED_BSLFMT_FORMAT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard compliant `format` implementation
//
//@CLASSES:
//  bsl::basic_format_arg:           access to standard-compliant argument
//  bsl::basic_format_args:          access to formatting arguments
//  bsl::basic_format_context:       access to formatting state
//  bsl::basic_format_parse_context: access to format string parsing state
//  bsl::basic_format_string:        checked format string
//  bsl::formatter:                  template type for BDE formatters
//  bsl::format_args:                `basic_format_args` for `char`
//  bsl::format_error:               standard-compliant exception type
//  bsl::format_parse_context:       `basic_format_parse_context` for `char`
//  bsl::format_string:              `basic_format_string` for `char`
//  bsl::format_to_n_result:         result type for `format_to_n`
//  bsl::wformat_args:               `basic_format_args` for `wchar_t`
//  bsl::wformat_parse_context:      `basic_format_parse_context` for `wchar_t`
//  bsl::wformat_string:             format_string for `wchar_t`
//
//@CANONICAL_HEADER: bsl_format.h
//
//@SEE_ALSO: ISO C++ Standard, <format>
//
//@DESCRIPTION: This component will provide, in the `bsl` namespace, wrappers
// around the functions and types exposed by the standard <format> header where
// such is available, otherwise aliases to the `bslfmt` implementation.
//
// This will provide, where such are available, wrappers around the
// `std::format`, `std::format_to`, `std::format_to_n`, `std::vformat`, and
// `std::vformat_to` functions of which the `format` and `vformat` wrappers are
// allocator-aware.  Where they are not available such as on older compilers,
// or when compiling C++17 and earlier, BSL implementation is provided.
//
// Where a BSL implementation is provided, functionality is limited to that
// provided by C++20 and excludes the following features:
//
// - Support for locales other than the default ("C") locale.
// - Support for wide strings
// - Alternative date/time representations
// - Date/time directives not supported by the standard `strftime` function
// - Character escaping
// - Compile-time format string checking
//
// This header is not intended to be included directly.  Please include
// `<bsl_format.h>` to be able to use BDE formatting functionality.
//
///User-provided formatters
///------------------------
//
// User-provided formatters are supported by the BSL implementation, just as
// they are by the standard library implementation. However, in order for them
// to be compatible with both implementations, there are specific requirements,
// notably:
//
// - If you will define a formatter for your type `T`, do so in the same
//   component header that defines `T` itself.  This avoids issues due to
//   users forgetting to include the header for the formatter.
// - Define `bsl::formatter<T>`
// - *DO NOT* define `std::formatter<T>`
// - Use template arguments for the format context and parse context
//   parameters. This is essential as the parameter type passed in will
//   depend upon underlying implementation.
// - The `parse` function must be `constexpr` in C++20, but this is not
//   required (and may not be possible) for earlier C++ standards.
//
// An example of a user defined formatter is as follows:
//
// ```
// namespace bsl {
//
// template <class t_CHAR>
// struct formatter<UserDefinedType, t_CHAR> {
//     template <class t_PARSE_CONTEXT>
//     BSLS_KEYWORD_CONSTEXPR_CPP20
//     t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& pc)
//     {
//         // implementation goes here
//     }
//
//     template <class t_FORMAT_CONTEXT>
//     t_FORMAT_CONTEXT::iterator format(UserDefinedType   s,
//                                       t_FORMAT_CONTEXT& ctx) const
//     {
//         // implementation goes here
//     }
// };
//
// }  // close namespace bsl
// ```
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Simple integer formatting
/// - - - - - - - - - - - - - - - - - -
//
// Formatters for base types are already defined, so to output such objects the
// `bsl::format` function can be used in exactly the same way as the original
// one from the `stl` library:
// ```
//  int         value = 99;
//  bsl::string res   = bsl::format("{:#06x}", value);
//
//  assert(bsl::string("0x0063") == res);
// ```
//
///Example 2: Creating custom formatter for user type
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a custom type representing a date and we want to output it
// to the stream in different formats depending on the circumstances using
// `bsl::format` function.  The following example demonstrates how such custom
// formatter may be implemented.
//
// First, we define our `Date` class:
// ```
//  /// This class implements a complex-constrained, value-semantic type for
//  /// representing dates.  Each object of this class *always* represents a
//  /// *valid* date value in the range `[0001JAN01 .. 9999DEC31]` inclusive.
//  class Date {
//    private:
//      // DATA
//      int d_year;   // year
//      int d_month;  // month
//      int d_day;    // day
//
//    public:
//      // CREATORS
//
//      /// Create an object having the value represented by the specified
//      /// `year`, `month`, and `day`.
//      Date(int year, int month, int day)
//      : d_year(year)
//      , d_month(month)
//      , d_day(day)
//      {
//          assert((1 <= year)  && (9999 >= year));
//          assert((1 <= month) && (12 >= month));
//          assert((1 <= day)   && (31 >= day));
//      }
//
//      // ACCESSORS
//
//      /// Return the year of this date.
//      int year() const { return d_year; }
//
//      /// Return the month of this date.
//      int month() const { return d_month; }
//
//      /// Return the day of this date.
//      int day() const { return d_day; }
//  };
// ```
// Then, we define our custom formatter for this date class.  In it, two
// methods are necessary: `parse()` and `format()`.  The `parse` method parses
// the format string itself to determine the formatting to be used by the
// `format` method, which writes the formatted date into a string.  Both
// methods are required to conform to a specific interface.
// ```
//  /// This struct is a base class for `bsl::formatter` specializations for
//  /// the `Date` class.
//  template <class t_CHAR>
//  struct DateFormatter {
//    private:
// ```
// The convenience of using the `bsl::format` function is that the users can
// come up with the description language themselves.  In our case, for
// simplicity, we will display the date in two formats - numeric (`1999-10-23`)
// and verbal (`23 October 1999`).  Accordingly, to indicate the desired type,
// we will use one of two letters in the format description: 'n' ('N') or 'v'
// ('V').  And one field is enough for us to store it.
// ```
//      // PRIVATE TYPES
//      enum Format {
//          e_NUMERIC,  // 1999-10-23
//          e_VERBAL    // 23 October 1999
//      };
//
//      // DATA
//      Format d_format;  // output format
//
//      // PRIVATE ACCESSORS
//
//      /// Output the specified `yearValue` to the specified `outIterator`.
//      /// The specified `paddingRequired` indicates whether additional
//      /// characters need to be added to fill empty space.
//      template <class t_FORMAT_CONTEXT>
//      void outputYear(
//                  typename t_FORMAT_CONTEXT::iterator& outIterator,
//                  int                                  yearValue,
//                  bool                                 paddingRequired) const
//      {
//          typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
//
//          char  buffer[4];
//          char *bufferEnd = NFUtil::toChars(buffer, buffer + 4, yearValue);
//
//          if (paddingRequired) {
//              const char *paddingStr = "000";
//              size_t      numPaddingCharacters = 0;
//
//              if (1000 > yearValue) {
//                  ++numPaddingCharacters;
//                  if (100 > yearValue) {
//                      ++numPaddingCharacters;
//                      if (10 > yearValue) {
//                          ++numPaddingCharacters;
//                      }
//                  }
//              }
//
//              if (numPaddingCharacters) {
//                  outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//                      t_CHAR>::outputFromChar(paddingStr,
//                                              paddingStr +
//                                                  numPaddingCharacters,
//                                              outIterator);
//              }
//          }
//          outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//              t_CHAR>::outputFromChar(buffer, bufferEnd, outIterator);
//      }
//
//      /// Output the specified `monthOrDayValue` to the specified
//      /// `outIterator`.  The specified `paddingRequired` indicates whether
//      /// an additional character needs to be added to fill empty space.
//      template <class t_FORMAT_CONTEXT>
//      void outputMonthDay(
//                  typename t_FORMAT_CONTEXT::iterator& outIterator,
//                  int                                  monthOrDayValue,
//                  bool                                 paddingRequired) const
//      {
//          typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
//
//          char  buffer[2];
//          char *bufferEnd = NFUtil::toChars(buffer,
//                                            buffer + 2,
//                                            monthOrDayValue);
//
//          if (paddingRequired) {
//              if (10 > monthOrDayValue) {
//                  outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//                      t_CHAR>::outputFromChar('0', outIterator);
//              }
//          }
//          outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//              t_CHAR>::outputFromChar(buffer, bufferEnd, outIterator);
//      }
// ```
// Notice that if the standard implementation of the format is supported by
// your compiler, then the `parse` function as well as the constructor must be
// declared as `constexpr`.
// ```
//    public:
//      // CREATORS
//
//      /// Create a formatter that outputs values in the `e_NUMERIC` format.
//      /// Thus, numeric is the default format for the `Date` object.
//      BSLS_KEYWORD_CONSTEXPR_CPP20 DateFormatter()
//      : d_format(e_NUMERIC)
//      {
//      }
//
//      // MANIPULATORS
//
//      /// Parse the specified `context` and return end iterator of parsed
//      /// range.
//      template <class t_PARSE_CONTEXT>
//      BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
//                                                    t_PARSE_CONTEXT& context)
//      {
//          typedef typename bsl::iterator_traits<
//              typename t_PARSE_CONTEXT::const_iterator>::value_type
//              IteratorValueType;
//          BSLMF_ASSERT((bsl::is_same<IteratorValueType, t_CHAR>::value));
//
//          typename t_PARSE_CONTEXT::const_iterator current = context.begin();
//          typename t_PARSE_CONTEXT::const_iterator end     = context.end();
// ```
// `bsl::format` calls `parse()` function first and here we can configure our
// formatter so that it then outputs values in the format we need.
// `context.begin()` returns an iterator pointing to the end of the parsed
// range.
// ```
//          // Handling empty string or empty specification
//          if (current == end || *current == '}') {
//              return context.begin();                               // RETURN
//          }
//
//          // Reading format specification
//          switch (*current) {
//              case ('V'):
//              case ('v'): {
//                d_format = e_VERBAL;
//              } break;
//              case ('N'):
//              case ('n'): {
//                // `e_NUMERIC` value is assigned at object construction
//              } break;
//              default: {
//                BSLS_THROW(bsl::format_error(
//                     "Unexpected symbol in format specification"));  // THROW
//              }
//          }
//
//          // Move the iterator to the next position and check that there are
//          // no extra characters in the description.
//
//          ++current;
//
//          if (current != end && *current != '}') {
//              BSLS_THROW(bsl::format_error(
//                      "Too many symbols in format specification"));  // THROW
//          }
//
//          context.advance_to(current);
//          return context.begin();
//      }
//
//      // ACCESSORS
//
//      /// Create string representation of the specified `value`, customized
//      /// in accordance with the requested format and the specified
//      /// `formatContext`, and copy it to the output that the output iterator
//      /// of the `formatContext` points to.
//      template <class t_FORMAT_CONTEXT>
//      typename t_FORMAT_CONTEXT::iterator format(
//                                       Date              value,
//                                       t_FORMAT_CONTEXT& formatContext) const
//      {
//          typename t_FORMAT_CONTEXT::iterator outIterator =
//                                                         formatContext.out();
// ```
// Next, we outputting the date in accordance with the previously set settings:
// ```
//          if (e_VERBAL == d_format) {  // 23 October 1999
//              static const char *const months[] = {"January",
//                                                   "February",
//                                                   "March",
//                                                   "April",
//                                                   "May",
//                                                   "June",
//                                                   "July",
//                                                   "August",
//                                                   "September",
//                                                   "October",
//                                                   "November",
//                                                   "December"};
//
//              // Outputting day
//              outputMonthDay<t_FORMAT_CONTEXT>(outIterator,
//                                               value.day(),
//                                               false);
//              outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//                  t_CHAR>::outputFromChar(' ', outIterator);
//
//              // Outputting month
//              const char *month = months[value.month() - 1];
//              outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//                  t_CHAR>::outputFromChar(month,
//                                          month + std::strlen(month),
//                                          outIterator);
//              outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//                  t_CHAR>::outputFromChar(' ', outIterator);
//
//              // Outputting year
//              outputYear<t_FORMAT_CONTEXT>(outIterator, value.year(), false);
//          }
//          else if (e_NUMERIC == d_format) {  // 1999-10-23
//              // Outputting year
//              outputYear<t_FORMAT_CONTEXT>(outIterator, value.year(), true);
//              outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//                  t_CHAR>::outputFromChar('-', outIterator);
//
//              // Outputting month
//              outputMonthDay<t_FORMAT_CONTEXT>(outIterator,
//                                               value.month(),
//                                               true);
//              outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//                  t_CHAR>::outputFromChar('-', outIterator);
//
//              // Outputting day
//              outputMonthDay<t_FORMAT_CONTEXT>(outIterator,
//                                               value.day(),
//                                               true);
//          }
//
//          return outIterator;
//      }
//  };
// ```
// Now, we define the `bsl::formatter` specialization for our `Date` class
// simply as a child-class of `DateFormatter`. Alternatively, we could have
// placed the implementation directly into the `bsl::formatter` specialization.
// Notice that the specialization must be defined in the `bsl` namespace.
// ```
//  namespace bsl {
//
//  template <class t_CHAR>
//  struct formatter<Date, t_CHAR> : DateFormatter<t_CHAR> {
//  };
//
//  }  // close namespace bsl
// ```
// Finally, we create a `Date` object, output it to the string and verify the
// result:
// ```
//  Date        date(1999, 10, 23);
//  bsl::string result = bsl::format("{:v}", date);
//  assert(bsl::string("23 October 1999") == result);
//
//  result = bsl::format("{:N}", date);
//  assert(bsl::string("1999-10-23") == result);
// ```

#include <bslscm_version.h>

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <bslfmt_format_arg.h>
#include <bslfmt_format_args.h>
#include <bslfmt_format_context.h>
#include <bslfmt_format_imp.h>
#include <bslfmt_format_string.h>
#endif

#include <bslfmt_formaterror.h>

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <bslfmt_formatparsecontext.h>
#endif

#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterbool.h>
#include <bslfmt_formattercharacter.h>
#include <bslfmt_formatterintegral.h>
#include <bslfmt_formatterfloating.h>
#include <bslfmt_formatterpointer.h>
#include <bslfmt_formatterstring.h>

#include <bsls_exceptionutil.h>
#include <bsls_libraryfeatures.h>

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <bslstl_utility.h>
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <format>
#include <locale>
#include <string_view>
#endif

#include <stdexcept>

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

namespace bsl {
using BloombergLP::bslfmt::basic_format_arg;
using BloombergLP::bslfmt::basic_format_args;
using BloombergLP::bslfmt::basic_format_context;
using BloombergLP::bslfmt::basic_format_parse_context;
using BloombergLP::bslfmt::basic_format_string;
using BloombergLP::bslfmt::format;
using BloombergLP::bslfmt::format_args;
using BloombergLP::bslfmt::format_error;
using BloombergLP::bslfmt::format_parse_context;
using BloombergLP::bslfmt::format_string;
using BloombergLP::bslfmt::format_to;
using BloombergLP::bslfmt::format_to_n;
using BloombergLP::bslfmt::format_to_n_result;
using BloombergLP::bslfmt::formatted_size;
using BloombergLP::bslfmt::make_format_args;
using BloombergLP::bslfmt::make_wformat_args;
using BloombergLP::bslfmt::vformat;
using BloombergLP::bslfmt::vformat_to;
using BloombergLP::bslfmt::visit_format_arg;
using BloombergLP::bslfmt::wformat_args;
using BloombergLP::bslfmt::wformat_parse_context;
using BloombergLP::bslfmt::wformat_string;
}  // close namespace bsl

#else

namespace bsl {

// TYPE ALIASES

using std::basic_format_arg;
using std::basic_format_args;
using std::basic_format_context;
using std::basic_format_parse_context;
using std::basic_format_string;
using std::format_args;
using std::format_error;
using std::format_parse_context;
using std::format_string;
using std::format_to;
using std::format_to_n;
using std::format_to_n_result;
using std::formatted_size;
using std::vformat_to;
using std::visit_format_arg;
using std::wformat_args;
using std::wformat_parse_context;
using std::wformat_string;

// FREE FUNCTIONS

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` and return the result as a `bsl::string`.  In the event
/// of an error the exception `format_error` is thrown.
template <class... t_ARGS>
string format(format_string<t_ARGS...> fmtStr, t_ARGS&&... args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` and return the result.  In the event of an error the
/// exception `format_error` is thrown.
template <class... t_ARGS>
wstring format(wformat_string<t_ARGS...> fmtStr, t_ARGS&&... args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` in the locale of the specified `loc` and return the
/// result.  In the event of an error the exception `format_error` is thrown.
template <class... t_ARGS>
string format(const std::locale&       loc,
              format_string<t_ARGS...> fmtStr,
              t_ARGS&&...              args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` in the locale of the specified `loc` and return the
/// result.  In the event of an error the exception `format_error` is thrown.
template <class... t_ARGS>
wstring format(const std::locale&        loc,
               wformat_string<t_ARGS...> fmtStr,
               t_ARGS&&...               args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, using the specified `alloc` to supply memory (if
/// required), and return the result.  In the event of an error the exception
/// `format_error` is thrown.
template <class... t_ARGS>
string format(allocator<char>          alloc,
              format_string<t_ARGS...> fmtStr,
              t_ARGS&&...              args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, using the specified `alloc` to supply memory (if
/// required), and return the result.  In the event of an error the exception
/// `format_error` is thrown.
template <class... t_ARGS>
wstring format(allocator<wchar_t>        alloc,
               wformat_string<t_ARGS...> fmtStr,
               t_ARGS&&...               args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` in the locale of the specified `loc`, using the
/// specified `allocator` to supply memory (if required), and return the
/// result.  In the event of an error the exception `format_error` is thrown.
template <class... t_ARGS>
string format(allocator<char>          alloc,
              const std::locale&       loc,
              format_string<t_ARGS...> fmtStr,
              t_ARGS&&...              args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` in the locale of the specified `loc`, using the
/// specified `allocator` to supply memory (if required), and return the
/// result.  In the event of an error the exception `format_error` is thrown.
template <class... t_ARGS>
wstring format(allocator<wchar_t>        alloc,
               const std::locale&        loc,
               wformat_string<t_ARGS...> fmtStr,
               t_ARGS&&...               args);

/// Return an object, whose type is not specified, holding an array of
/// `format_arg` types constructed from the specified `args`.  The type
/// returned is implicitly convertible to a `format_args` holding a reference
/// to the contained array.  This function will statically assert if any of the
/// specified template parameters `t_ARGS` is of type `long double`.
template <class t_CONTEXT = std::format_context, class... t_ARGS>
auto make_format_args(t_ARGS&... args);

/// Return an object, whose type is not specified, holding an array of
/// `wformat_arg` types constructed from the specified `args`.  The type
/// returned is implicitly convertible to a `wformat_args` holding a reference
/// to the contained array.  This function will statically assert if any of the
/// specified template parameters `t_ARGS` is of type `long double`.
template <class... t_ARGS>
auto make_wformat_args(t_ARGS&... args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write the result of this operation into the string
/// addressed by the specified `out` parameter.  In the event of an error the
/// exception `format_error` is thrown.  Behavior is undefined if `out` does
/// not point to a valid `bsl::string` object.  Note that this overload is
/// provided in addition to the overloads in the standard library, and the
/// requires clause is necessary to avoid ambiguity.
template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::string>)
void format_to(t_STRING                 *out,
               format_string<t_ARGS...>  fmtStr,
               t_ARGS&&...               args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write the result of this operation into the string
/// addressed by the specified `out` parameter.  In the event of an error the
/// exception `format_error` is thrown.  Behavior is undefined if `out` does
/// not point to a valid `bsl::string` object.  Note that this overload is
/// provided in addition to the overloads in the standard library, and the
/// requires clause is necessary to avoid ambiguity.
template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::wstring>)
void format_to(t_STRING                  *out,
               wformat_string<t_ARGS...>  fmtStr,
               t_ARGS&&...                args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` in the locale of the specified `loc`, and write the
/// result of this operation into the string addressed by the specified `out`
/// parameter.  In the event of an error the exception `format_error` is
/// thrown.  Behavior is undefined if `out` does not point to a valid
/// `bsl::string` object.  Note that this overload is provided in addition to
/// the overloads in the standard library, and the requires clause is necessary
/// to avoid ambiguity.
template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::string>)
void format_to(t_STRING                 *out,
               const std::locale&        loc,
               format_string<t_ARGS...>  fmtStr,
               t_ARGS&&...               args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` in the locale of the specified `loc`, and write the
/// result of this operation into the string addressed by the specified `out`
/// parameter.  In the event of an error the exception `format_error` is
/// thrown.  Behavior is undefined if `out` does not point to a valid
/// `bsl::string` object.  Note that this overload is provided in addition to
/// the overloads in the standard library, and the requires clause is necessary
/// to avoid ambiguity.
template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::wstring>)
void format_to(t_STRING                  *out,
               const std::locale&         loc,
               wformat_string<t_ARGS...>  fmtStr,
               t_ARGS&&...                args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` and return the result.  In the event of an error the
/// exception `format_error` is thrown.
string vformat(std::string_view fmtStr, format_args args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` and return the result.  In the event of an error the
/// exception `format_error` is thrown.
wstring vformat(std::wstring_view fmtStr, wformat_args args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` in the locale of the specified `loc` and return the
/// result.  In the event of an error the exception `format_error` is thrown.
string vformat(const std::locale& loc,
               std::string_view   fmtStr,
               format_args        args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` in the locale of the specified `loc` and return the
/// result.  In the event of an error the exception `format_error` is thrown.
wstring vformat(const std::locale& loc,
                std::wstring_view  fmtStr,
                wformat_args       args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, using the specified `allocator` to supply memory (if
/// required), and return the result.  In the event of an error the exception
/// `format_error` is thrown.
string vformat(allocator<char>  alloc,
               std::string_view fmtStr,
               format_args      args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, using the specified `allocator` to supply memory (if
/// required), and return the result.  In the event of an error the exception
/// `format_error` is thrown.
wstring vformat(allocator<wchar_t> alloc,
                std::wstring_view  fmtStr,
                wformat_args       args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` in the locale of the specified `loc`, using the
/// specified `allocator` to supply memory (if required), and return the
/// result.  In the event of an error the exception `format_error` is thrown.
string vformat(allocator<char>    alloc,
               const std::locale& loc,
               std::string_view   fmtStr,
               format_args        args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` in the locale of the specified `loc`, using the
/// specified `allocator` to supply memory (if required), and return the
/// result.  In the event of an error the exception `format_error` is thrown.
wstring vformat(allocator<wchar_t> alloc,
                const std::locale& loc,
                std::wstring_view  fmtStr,
                wformat_args       args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write the result of this operation into the string
/// addressed by the specified `out` parameter.  In the event of an error the
/// exception `format_error` is thrown.  Behavior is undefined if `out` does
/// not point to a valid `bsl::string` object.  Note that this overload is
/// provided in addition to the overloads in the standard library.
void vformat_to(string *out, std::string_view fmtStr, format_args args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write the result of this operation into the string
/// addressed by the specified `out` parameter.  In the event of an error the
/// exception `format_error` is thrown.  Behavior is undefined if `out` does
/// not point to a valid `bsl::string` object.  Note that this overload is
/// provided in addition to the overloads in the standard library.
void vformat_to(wstring *out, std::wstring_view fmtStr, wformat_args args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` in the locale of the specified `loc`, and write the
/// result of this operation into the string addressed by the specified `out`
/// parameter.  In the event of an error the exception `format_error` is
/// thrown.  Behavior is undefined if `out` does not point to a valid
/// `bsl::string` object.  Note that this overload is provided in addition to
/// the overloads in the standard library.
void vformat_to(string             *out,
                const std::locale&  loc,
                std::string_view    fmtStr,
                format_args         args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` in the locale of the specified `loc`, and write the
/// result of this operation into the string addressed by the specified `out`
/// parameter.  In the event of an error the exception `format_error` is
/// thrown.  Behavior is undefined if `out` does not point to a valid
/// `bsl::string` object.  Note that this overload is provided in addition to
/// the overloads in the standard library.
void vformat_to(wstring            *out,
                const std::locale&  loc,
                std::wstring_view   fmtStr,
                wformat_args        args);


// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

// FREE FUNCTIONS
template <class t_CONTEXT, class... t_ARGS>
auto make_format_args(t_ARGS&... args)
{
    static_assert(
                 (... && (!std::is_same_v<std::decay_t<t_ARGS>, long double>)),
                 "long double not supported in bsl::format");
    return std::make_format_args<t_CONTEXT, t_ARGS...>(args...);
}

template <class... t_ARGS>
auto make_wformat_args(t_ARGS&... args)
{
    static_assert(
                 (... && (!std::is_same_v<std::decay_t<t_ARGS>, long double>)),
                 "long double not supported in bsl::format");
    return std::make_wformat_args<t_ARGS...>(args...);
}

template <class... t_ARGS>
string format(format_string<t_ARGS...> fmtStr, t_ARGS&&... args)
{
    return bsl::vformat(fmtStr.get(),
                        bsl::make_format_args(args...));
}

template <class... t_ARGS>
wstring format(wformat_string<t_ARGS...> fmtStr, t_ARGS&&... args)
{
    return bsl::vformat(fmtStr.get(),
                        bsl::make_wformat_args(args...));
}

template <class... t_ARGS>
string format(const std::locale&       loc,
              format_string<t_ARGS...> fmtStr,
              t_ARGS&&...              args)
{
    return bsl::vformat(loc,
                        fmtStr.get(),
                        bsl::make_format_args(args...));
}

template <class... t_ARGS>
wstring format(const std::locale&        loc,
               wformat_string<t_ARGS...> fmtStr,
               t_ARGS&&...               args)
{
    return bsl::vformat(loc,
                        fmtStr.get(),
                        bsl::make_wformat_args(args...));
}

template <class... t_ARGS>
string format(allocator<char>          alloc,
              format_string<t_ARGS...> fmtStr,
              t_ARGS&&...              args)
{
    return bsl::vformat(alloc,
                        fmtStr.get(),
                        bsl::make_format_args(args...));
}

template <class... t_ARGS>
wstring format(allocator<wchar_t>        alloc,
               wformat_string<t_ARGS...> fmtStr,
               t_ARGS&&...               args)
{
    return bsl::vformat(alloc,
                        fmtStr.get(),
                        bsl::make_wformat_args(args...));
}

template <class... t_ARGS>
string format(allocator<char>          alloc,
              const std::locale&       loc,
              format_string<t_ARGS...> fmtStr,
              t_ARGS&&...              args)
{
    return bsl::vformat(alloc,
                        loc,
                        fmtStr.get(),
                        bsl::make_format_args(args...));
}

template <class... t_ARGS>
wstring format(allocator<char>           alloc,
               const std::locale&        loc,
               wformat_string<t_ARGS...> fmtStr,
               t_ARGS&&...               args)
{
    return bsl::vformat(alloc,
                        loc,
                        fmtStr.get(),
                        bsl::make_wformat_args(args...));
}

inline
string vformat(std::string_view fmt, format_args args)
{
    string result;
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
wstring vformat(std::wstring_view fmt, wformat_args args)
{
    wstring result;
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
string vformat(const std::locale& loc, std::string_view fmt, format_args args)
{
    string result;
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

inline
wstring vformat(const std::locale& loc,
                std::wstring_view  fmt,
                wformat_args       args)
{
    wstring result;
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

inline
string vformat(allocator<char> alloc, std::string_view fmt, format_args args)
{
    string result(alloc);
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
wstring vformat(allocator<wchar_t> alloc,
                std::wstring_view  fmt,
                wformat_args       args)
{
    wstring result(alloc);
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
string vformat(allocator<char>    alloc,
               const std::locale& loc,
               std::string_view   fmt,
               format_args        args)
{
    string result(alloc);
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

inline
wstring vformat(allocator<wchar_t> alloc,
                const std::locale& loc,
                std::wstring_view  fmt,
                wformat_args       args)
{
    wstring result(alloc);
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::string>) void format_to(
                                              t_STRING                 *out,
                                              format_string<t_ARGS...>  fmtStr,
                                              t_ARGS&&...               args)
{
    bsl::vformat_to(out,
                    fmtStr.get(),
                    bsl::make_format_args(args...));
}

template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::wstring>)
void format_to(t_STRING                  *out,
               wformat_string<t_ARGS...>  fmtStr,
               t_ARGS&&...                args)
{
    bsl::vformat_to(out,
                    fmtStr.get(),
                    bsl::make_wformat_args(args...));
}

template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::string>)
void format_to(t_STRING                 *out,
               const std::locale&        loc,
               format_string<t_ARGS...>  fmtStr,
               t_ARGS&&...               args)
{
    bsl::vformat_to(out,
                    loc,
                    fmtStr.get(),
                    bsl::make_format_args(args...));
}

template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::wstring>)
void format_to(t_STRING                  *out,
               const std::locale&         loc,
               wformat_string<t_ARGS...>  fmtStr,
               t_ARGS&&...                args)
{
    bsl::vformat_to(out,
                    loc,
                    fmtStr.get(),
                    bsl::make_wformat_args(args...));
}


inline
void vformat_to(string *out, std::string_view fmt, format_args args)
{
    out->clear();
    std::vformat_to(std::back_inserter(*out), fmt, args);
}

inline
void vformat_to(wstring *out, std::wstring_view fmt, wformat_args args)
{
    out->clear();
    std::vformat_to(std::back_inserter(*out), fmt, args);
}

inline
void vformat_to(string             *out,
                const std::locale&  loc,
                std::string_view    fmt,
                format_args         args)
{
    out->clear();
    std::vformat_to(std::back_inserter(*out), loc, fmt, args);
}

inline
void vformat_to(wstring            *out,
                const std::locale&  loc,
                std::wstring_view   fmt,
                wformat_args        args)
{
    out->clear();
    std::vformat_to(std::back_inserter(*out), loc, fmt, args);
}


}  // close namespace bsl

#endif  // !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

#endif  // INCLUDED_BSLFMT_FORMAT

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
