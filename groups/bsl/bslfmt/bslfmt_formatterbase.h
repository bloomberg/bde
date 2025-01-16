// bslfmt_formatterbase.h                                             -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERBASE
#define INCLUDED_BSLFMT_FORMATTERBASE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a base template for formatter specializations
//
//@CLASSES:
//  bsl::formatter: standard-compliant formatter base template
//
//@DESCRIPTION: This component provides an base template of the C++20 Standard
// Library's `formatter`, which provides a customization point for use defined
// types seeking to use the formatting library.
//
// It also provides a mechanism, when the standard library `<format>` header is
// available, to enable those partial specializations to be forwarded to the
// `std` namespace to enable use of `std::format` as well as `bsl::format`
//
// Trait macros are provided to enable the prevention of that promotion where
// formatters already exist in the `std` namespace. These traits will not
// typically be used outside the `bslfmt` package.
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
// - Define `bsl::formatter<T>` - *DO NOT* define `std::formatter<T>` - Use
//   template arguments for the format context and parse context
//   parameters. This is essential as the parameter type passed in will
//   depend upon underlying implementation.
// - The `parse` function should be constexpr in C++20, but this is not
//   required (and may not be possible) for earlier C++ standards.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating custom formatter for user type
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
// a full-fledged example would require duplicating a huge amount of code. A
// full example of a custom formatter implementation can be found in
// the `bslfmt_format` component.

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
#define BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20                      \
    typedef void Formatter_PreventStdPromotion
#else
// On earlier C++ compilers we use a dummy typedef to avoid the compiler
// warning about extra semicolons.
#define BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20                      \
    typedef void Formatter_DoNotPreventStdPromotion_DummyTypedef
#endif

namespace bsl {

                          // ================
                          // struct formatter
                          // ================

/// This is the base template for the `bsl::formatter` class. Its members are
/// deleted to ensure attempts to format a type without a partial
/// specialization of `formatter` for that type will result in a compile time
/// error.
template <class t_ARG, class t_CHAR = char>
struct formatter {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

  private:
    // NOT IMPLEMENTED
    formatter(const formatter&) BSLS_KEYWORD_DELETED;
    formatter& operator=(const formatter&) BSLS_KEYWORD_DELETED;
};
}  // close namespace bsl

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

namespace BloombergLP {
namespace bslfmt {

/// This type exists to enable SFINAE-based detection of the presence or
/// absence of the `BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20` trait as
/// appropriate.
template <class t_FORMATTER, class = void>
struct FormatterBase_IsStdAliasingEnabled : bsl::true_type {
};

template <class t_FORMATTER>
struct FormatterBase_IsStdAliasingEnabled<
    t_FORMATTER,
    typename t_FORMATTER::Formatter_PreventStdPromotion> : bsl::false_type {
};

}  // close package namespace
}  // close enterprise namespace

namespace std {

template <class t_ARG, class t_CHAR>
struct formatter;

/// Partial `formatter` specialization in the `std` namespace to enable use of
/// formatters defined in the `bsl` namespace.
template <class t_ARG, class t_CHAR>
requires(
    BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
        bsl::formatter<t_ARG, t_CHAR> >::value
)
struct formatter<t_ARG, t_CHAR>
: bsl::formatter<t_ARG, t_CHAR> {};

}  // close namespace std
#endif

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
