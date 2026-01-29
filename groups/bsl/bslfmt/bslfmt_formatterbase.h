// bslfmt_formatterbase.h                                             -*-C++-*-
#ifndef INCLUDED_BSLFMT_FORMATTERBASE
#define INCLUDED_BSLFMT_FORMATTERBASE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a base template for formatter specializations.
//
//@CLASSES:
//  bsl::formatter: standard-compliant formatter base template
//
//@CANONICAL_HEADER: bsl_format.h
//
//@DESCRIPTION: This component provides a base template of the C++20 Standard
// Library's `formatter`, which is a customization point for user defined
// types seeking to use the formatting library.
//
// It also provides a mechanism, when the standard library `<format>` header is
// available, to forward those partial specializations to the `std` namespace
// to enable use of `std::format` as well as `bsl::format`.
//
// This header is not intended to be included directly.  Please include
// `<bsl_format.h>` to be able to use specializations of `bsl::formatter`.
//
///User-provided Formatters
///------------------------
// User-provided formatters are supported by the BSL implementation, just as
// they are by the standard library implementation.  However, in order for them
// to be compatible with both implementations, there are specific requirements,
// notably:
// * The formatter for a user defined type `T`, should be declared in the same
//   component header in which this type is declared to avoid issues due to
//   users forgetting to include the header for the formatter.
// * Formatter must be defined in the namespace `bsl`, not `std`.
// * Template arguments must be used for the format and parse context
//   parameters.  This is essential as the parameter type passed in might
//   depend upon underlying implementation.
// * The `parse` function should be constexpr in C++20, but this is not
//   required (and may not be possible) for earlier C++ standards.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating Custom Formatter For User Type
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a custom type representing a date. And we want to output it
// to the stream in different formats depending on the circumstances using
// `bsl::format` function. The following example shows how this can be done.
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
//      Date(int year, int month, int day);
//
//      // ACCESSORS
//
//      /// Return the year of this date.
//      int year() const;
//
//      /// Return the month of this date.
//      int month() const;
//
//      /// Return the day of this date.
//      int day() const;
//  };
// ```
// Now, we define `formatter` specialization for the `Date` and in particular
// `parse()` and `format()` functions which will be called by `bsl::format`.
// Note that specialization must be defined in the namespace `bsl`.
// ```
//  namespace bsl {
//
//  template <class t_CHAR>
//  struct formatter<Date, t_CHAR> {
//      // MANIPULATORS
//
//      /// Parse the specified `parseContext` and return an iterator, pointing
//      /// to the beginning of the format string.
//      template <class t_PARSE_CONTEXT>
//      BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
//                                              t_PARSE_CONTEXT& parseContext);
//
//      // ACCESSORS
//
//      /// Create string representation of the specified `value`, customized
//      /// in accordance with the requested format and the specified
//      /// `formatContext`, and copy it to the output that the output iterator
//      /// of the `formatContext` points to.
//      template <class t_FORMAT_CONTEXT>
//      typename t_FORMAT_CONTEXT::iterator format(
//                                      Date              value,
//                                      t_FORMAT_CONTEXT& formatContext) const;
//  };
//
//  }  // close namespace bsl
// ```
// Unfortunately, due to the position of this component in the class hierarchy,
// a full-fledged example would require duplicating a huge amount of code.  A
// complete example of a custom formatter implementation can be found in the
// `bslfmt_format` component.

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #include <chrono>
  #include <concepts>
  #include <format>
  #include <string>
  #include <string_view>
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
namespace BloombergLP {
namespace bslfmt {

/// Simple trait type that tells is `t_TYPE` is an instance of
/// `std::basic_string`.
template <class t_TYPE>
struct FormatterBase_IsStdBasicString {
    static const bool value = false;
};

template <class t_CHAR, class t_TRAITS, class t_ALLOCATOR>
struct FormatterBase_IsStdBasicString<
    std::basic_string<t_CHAR, t_TRAITS, t_ALLOCATOR> > {
    static const bool value = true;
};

/// Simple trait type that tells is `t_TYPE` is an instance of
/// `std::basic_string_view`.
template <class t_TYPE>
struct FormatterBase_IsStdBasicStringView {
    static const bool value = false;
};

template <class t_CHAR, class t_TRAITS>
struct FormatterBase_IsStdBasicStringView<
    std::basic_string_view<t_CHAR, t_TRAITS> > {
    static const bool value = true;
};

/// Simple trait type that tells is `t_TYPE` is an instance of
/// `std::chrono::duration`.
template <class t_TYPE>
struct FormatterBase_IsStdChronoDuration {
    static const bool value = false;
};

template <class t_REP, class t_PERIOD>
struct FormatterBase_IsStdChronoDuration<
    std::chrono::duration<t_REP, t_PERIOD> > {
    static const bool value = true;
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR
/// Simple trait type that tells is `t_TYPE` is an instance of
/// `std::chrono::sys_time`.
template <class t_TYPE>
struct FormatterBase_IsStdChronoSysTime {
    static const bool value = false;
};

template <class t_DURATION>
struct FormatterBase_IsStdChronoSysTime<
    std::chrono::sys_time<t_DURATION> > {
    static const bool value = true;
};

/// Simple trait type that tells is `t_TYPE` is an instance of
/// `std::chrono::utc_time`.
template <class t_TYPE>
struct FormatterBase_IsStdChronoUtcTime {
    static const bool value = false;
};

template <class t_DURATION>
struct FormatterBase_IsStdChronoUtcTime<
    std::chrono::utc_time<t_DURATION> > {
    static const bool value = true;
};

/// Simple trait type that tells is `t_TYPE` is an instance of
/// `std::chrono::tai_time`.
template <class t_TYPE>
struct FormatterBase_IsStdChronoTaiTime {
    static const bool value = false;
};

template <class t_DURATION>
struct FormatterBase_IsStdChronoTaiTime<
    std::chrono::tai_time<t_DURATION> > {
    static const bool value = true;
};

/// Simple trait type that tells is `t_TYPE` is an instance of
/// `std::chrono::gps_time`.
template <class t_TYPE>
struct FormatterBase_IsStdChronoGpsTime {
    static const bool value = false;
};

template <class t_DURATION>
struct FormatterBase_IsStdChronoGpsTime<
    std::chrono::gps_time<t_DURATION> > {
    static const bool value = true;
};

/// Simple trait type that tells is `t_TYPE` is an instance of
/// `std::chrono::file_time`.
template <class t_TYPE>
struct FormatterBase_IsStdChronoFileTime {
    static const bool value = false;
};

template <class t_DURATION>
struct FormatterBase_IsStdChronoFileTime<
    std::chrono::file_time<t_DURATION> > {
    static const bool value = true;
};

/// Simple trait type that tells is `t_TYPE` is an instance of
/// `std::chrono::local_time`.
template <class t_TYPE>
struct FormatterBase_IsStdChronoLocalTime {
    static const bool value = false;
};

template <class t_DURATION>
struct FormatterBase_IsStdChronoLocalTime<
    std::chrono::local_time<t_DURATION> > {
    static const bool value = true;
};

/// Simple trait type that tells is `t_TYPE` is an instance of
/// `std::chrono::hh_mm_ss`.
template <class t_TYPE>
struct FormatterBase_IsStdChronoHhMmSs {
    static const bool value = false;
};

template <class t_DURATION>
struct FormatterBase_IsStdChronoHhMmSs<
    std::chrono::hh_mm_ss<t_DURATION> > {
    static const bool value = true;
};

/// Simple trait type that tells is `t_TYPE` is an instance of any of
/// `std::chrono::*_time`.
template <class t_TYPE>
struct FormatterBase_IsStdChronoTimeType {
    static const bool value =
                           FormatterBase_IsStdChronoSysTime<t_TYPE>::value   ||
                           FormatterBase_IsStdChronoUtcTime<t_TYPE>::value   ||
                           FormatterBase_IsStdChronoTaiTime<t_TYPE>::value   ||
                           FormatterBase_IsStdChronoGpsTime<t_TYPE>::value   ||
                           FormatterBase_IsStdChronoFileTime<t_TYPE>::value  ||
                           FormatterBase_IsStdChronoLocalTime<t_TYPE>::value ||
                           FormatterBase_IsStdChronoHhMmSs<t_TYPE>::value;
};
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR

/// Simple trait type that tells is `t_TYPE` is an instance of any of the
/// `std::chrono` types that deal with points in time.
template <class t_TYPE>
struct FormatterBase_IsStdChronoPointInTimeType {
    static const bool value =
                std::is_same_v<t_TYPE, std::chrono::day>                     ||
                std::is_same_v<t_TYPE, std::chrono::month>                   ||
                std::is_same_v<t_TYPE, std::chrono::year>                    ||
                std::is_same_v<t_TYPE, std::chrono::weekday>                 ||
                std::is_same_v<t_TYPE, std::chrono::weekday_indexed>         ||
                std::is_same_v<t_TYPE, std::chrono::weekday_last>            ||
                std::is_same_v<t_TYPE, std::chrono::month_day>               ||
                std::is_same_v<t_TYPE, std::chrono::month_day_last>          ||
                std::is_same_v<t_TYPE, std::chrono::month_weekday>           ||
                std::is_same_v<t_TYPE, std::chrono::month_weekday_last>      ||
                std::is_same_v<t_TYPE, std::chrono::year_month>              ||
                std::is_same_v<t_TYPE, std::chrono::year_month_day>          ||
                std::is_same_v<t_TYPE, std::chrono::year_month_day_last>     ||
                std::is_same_v<t_TYPE, std::chrono::year_month_weekday>      ||
                std::is_same_v<t_TYPE, std::chrono::year_month_weekday_last>;
};

/// Simple trait type that tells is `t_TYPE` is an instance of
/// `std::chrono::zoned_time`.
template <class t_TYPE>
struct FormatterBase_IsStdChronoZonedTime {
    static const bool value = false;
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE
template <class t_DURATION, class t_TIME_ZONE_PTR>
struct FormatterBase_IsStdChronoZonedTime<
    std::chrono::zoned_time<t_DURATION, t_TIME_ZONE_PTR> > {
    static const bool value = true;
};
#endif

/// Simple trait type that tells is `t_TYPE` is an instance of any of the C++20
/// standard formattable `std::chrono` types.
template <class t_TYPE>
struct FormatterBase_IsStdChronoCpp20FormattableType {
    static const bool value =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR
                     FormatterBase_IsStdChronoTimeType<t_TYPE>::value        ||
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE
                     FormatterBase_IsStdChronoZonedTime<t_TYPE>::value       ||
                     std::is_same_v<t_TYPE, std::chrono::sys_info>           ||
                     std::is_same_v<t_TYPE, std::chrono::local_info>         ||
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE
                     FormatterBase_IsStdChronoPointInTimeType<t_TYPE>::value;
};

}  // close package namespace
}  // close enterprise namespace
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

namespace bsl {

                          // ================
                          // struct formatter
                          // ================

/// This is the base template for the `bsl::formatter` class.  Its members are
/// deleted to ensure attempts to format a type without a partial
/// specialization of `formatter` for that type will result in a compile time
/// error.
template <class t_ARG, class t_CHAR = char>
struct formatter {
  private:
    // NOT IMPLEMENTED
    formatter(const formatter&) BSLS_KEYWORD_DELETED;
    formatter& operator=(const formatter&) BSLS_KEYWORD_DELETED;
};

}  // close namespace bsl

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
namespace std {

/// Partial `formatter` specialization in the `std` namespace to enable use of
/// formatters defined in the `bsl` namespace.
template <class t_ARG, class t_CHAR>
    requires(
          !std::is_arithmetic_v<t_ARG>                                       &&
          !std::is_same_v<t_ARG, std::nullptr_t>                             &&
          !std::is_same_v<t_ARG, void *>                                     &&
          !std::is_same_v<t_ARG, const void *>                               &&
          !std::is_same_v<t_ARG, t_CHAR *>                                   &&
          !std::is_same_v<t_ARG, const t_CHAR *>                             &&
          !std::is_same_v<std::remove_extent_t<t_ARG>, const t_CHAR>         &&
          !BloombergLP::bslfmt::FormatterBase_IsStdBasicString<t_ARG>::value &&
          !BloombergLP::bslfmt::FormatterBase_IsStdBasicStringView<
              t_ARG>::value                                                  &&
          !BloombergLP::bslfmt::FormatterBase_IsStdChronoCpp20FormattableType<
              t_ARG>::value                                                  &&
          std::default_initializable<bsl::formatter<t_ARG, t_CHAR> >)
struct formatter<t_ARG, t_CHAR> : bsl::formatter<t_ARG, t_CHAR> {};

}  // close namespace std
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

#endif  // INCLUDED_BSLFMT_FORMATTERBASE

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
