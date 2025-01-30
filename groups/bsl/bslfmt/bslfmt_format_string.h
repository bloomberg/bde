// bslfmt_format_string.h                                             -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMAT_STRING
#define INCLUDED_BSLFMT_FORMAT_STRING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a string_view wrapper for formatting library usage
//
//@CLASSES:
//  bslfmt::basic_format_string: formatting library string_view wrapper
//
//@CANONICAL_HEADER: bsl_format.h
//
//@DESCRIPTION: This component provides an implementation of the C++20 Standard
// Library's `basic_format_string`, providing wrapper around string_view for
// format specification strings.
//
// Using this rather than string_view directly serves three main purposes:
//
// * We ensure that the interface to `format` and `vformat` are consistent with
//   the standard library, which takes a `format_string` rather than a
//   `string_view`.
// * Under C++20 we can enforce that the source string used for construction is
//   a compile-time constant.
// * Under C++17 and earlier, by limiting construction to a `const t_CHAR*` we
//   can limit the number of scenarios which would fail to compile under C++20
//   but succeed under earlier versions.
// * It is possible to perform compile-time checking of the format string when
//   compiled under C++20, in the same way as `std::format_string` (not yet
//   implemented).
//
// Suppression of the argument deduction that would normally result in a
// compile-time error requires that we use intermediate template aliases for
// `format_string` and `wformat_string`.  As this is not possible under C++03,
// usage of this type necessarily differs under C++03.  This is acceptable as
// this type is typically only used internally within the `bslfmt::format`
// family of functions.
//
// This header is not intended to be included directly.  Please include
// `<bsl_format.h>` to be able to use `bsl::basic_format_string`.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Own format function
/// - - - - - - - - - - - - - -
//
// This usage example reflects how `bslfmt::format` typically uses this type.
//
// Suppose we have a function that takes a format string and requires that the
// string be constant evaluated under C++20:
//
// ```
// #if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&
//     defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
//   template <class t_ARG>
//   void myFormatLikeFunction(bslfmt::format_string<t_ARG> fmtstr,
//                             const t_ARG&)
//   {
//     assert(fmtstr.get() == "{:}");
//   }
// #else
//   template <class t_ARG>
//   void myFormatLikeFunction(bslfmt::format_string        fmtstr,
//                             const t_ARG&)
//   {
//     assert(fmtstr.get() == "{:}");
//   }
// #endif
// ```
//
// We can then invoke our function:
//
// ```
//   int value = 5;
//   myFormatLikeFunction("{:}", value);
// ```
//

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bslmf_enableif.h>
#include <bslmf_typeidentity.h>

#include <stdexcept>

namespace BloombergLP {
namespace bslfmt {

// FORWARD DECLARATIONS

template <class t_CHAR>
struct Format_String_TestUpdater;


                     // =========================
                     // class basic_format_string
                     // =========================

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)

/// Type that wraps a `basic_string_view` for use by formatting function.  The
/// constructor performs additional compile-time checks of the format string
/// provided (not yet implemented).
template <class t_CHAR, class... t_ARGS>
class basic_format_string {
  private:
    // DATA
    bsl::basic_string_view<t_CHAR> d_formatString;  // the wrapped string

    // FRIENDS
    template <class t_INNER_CHAR>
    friend struct Format_String_TestUpdater;

  public:
    // CREATORS

    /// Create an instance of this type from the specified `str`.  This
    /// function only takes part in overload resolution if `str` is convertible
    /// to a `basic_string_view`.  Construction is ill-formed if `str` is not a
    /// compile-time constant.
    template <class t_STR,
              class = typename bsl::enable_if<bsl::is_convertible<
                  const t_STR&,
                  bsl::basic_string_view<t_CHAR> >::value>::type>
    consteval basic_format_string(const t_STR& str);

    // ACCESSORS

    /// Return the wrapped `basic_string_view` contained by this instance.
    BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR> get() const;
};

#elif defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)

template <class t_CHAR, class... t_ARGS>
class basic_format_string {
  private:
    // DATA
    bsl::basic_string_view<t_CHAR> d_formatString;  // the wrapped string

    // FRIENDS
    template <class t_INNER_CHAR>
    friend struct Format_String_TestUpdater;

  public:
    // CREATORS

    /// Create an instance of this type from the specified `str`.
    BSLS_KEYWORD_CONSTEXPR_CPP14 basic_format_string(const t_CHAR *str);

    // ACCESSORS

    /// Return the wrapped `basic_string_view` contained by this instance.
    BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR> get() const;
};

#else // C++03

template <class t_CHAR>
class basic_format_string {
  private:
    // DATA
    bsl::basic_string_view<t_CHAR> d_formatString;  // the wrapped string

    // FRIENDS
    template <class t_INNER_CHAR>
    friend struct Format_String_TestUpdater;

  public:
    // CREATORS

    /// Create an instance of this type from the specified `str`.
    BSLS_KEYWORD_CONSTEXPR_CPP14 basic_format_string(const t_CHAR *str);

    // ACCESSORS

    /// Return the wrapped `basic_string_view` contained by this instance.
    BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR> get() const;
};

#endif

// ALIAS TEMPLATES AND TYPEDEFS

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&                 \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)

template <class... t_ARGS>
using format_string =
                    basic_format_string<char, bsl::type_identity_t<t_ARGS>...>;

template <class... t_ARGS>
using wformat_string =
                 basic_format_string<wchar_t, bsl::type_identity_t<t_ARGS>...>;

#else // C++03

// Template aliases are not supported in C++03
typedef basic_format_string<char>    format_string;
typedef basic_format_string<wchar_t> wformat_string;

#endif  // Support for Alias and Variadic templates

                     // ===============================
                     // class Format_String_TestUpdater
                     // ===============================

/// This is a component-private type which enables test drivers to work around
/// the compile-time restrictions in `basic_format_string` construction.
template <class t_CHAR>
struct Format_String_TestUpdater
{
    /// Update the string contained in the specified `out` to the specified
    /// `value`.
    template <class t_FORMATSTRING>
    static void update(t_FORMATSTRING *out, const t_CHAR *value);

    /// Update the string contained in the specified `out` to the specified
    /// `value`.
    template <class t_FORMATSTRING>
    static void update(t_FORMATSTRING                 *out,
                       bsl::basic_string_view<t_CHAR>  value);
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class basic_format_string
                        // -------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)

// CREATORS
template <class t_CHAR, class... t_ARGS>
template <class t_STR, class>
consteval basic_format_string<t_CHAR, t_ARGS...>::basic_format_string(
                                                              const t_STR& str)
: d_formatString(str)
{
}

// ACCESSORS
template <class t_CHAR, class... t_ARGS>
inline
BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR>
basic_format_string<t_CHAR, t_ARGS...>::get() const
{
    return d_formatString;
}

#elif defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)

template <class t_CHAR, class... t_ARGS>
BSLS_KEYWORD_CONSTEXPR_CPP14
basic_format_string<t_CHAR, t_ARGS...>::basic_format_string(const t_CHAR *str)
{
    d_formatString = str;
}

template <class t_CHAR, class... t_ARGS>
inline
BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR>
basic_format_string<t_CHAR, t_ARGS...>::get() const
{
    return d_formatString;
}

#else // C++03

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP14
basic_format_string<t_CHAR>::basic_format_string(const t_CHAR *str)
{
    d_formatString = str;
}

template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR>
basic_format_string<t_CHAR>::get() const
{
    return d_formatString;
}

#endif

                     // -------------------------------
                     // class Format_String_TestUpdater
                     // -------------------------------

template <class t_CHAR>
template <class t_FORMATSTRING>
void Format_String_TestUpdater<t_CHAR>::update(t_FORMATSTRING *out,
                                               const t_CHAR   *value)
{
    out->d_formatString = value;
}

template <class t_CHAR>
template <class t_FORMATSTRING>
void Format_String_TestUpdater<t_CHAR>::update(
                                         t_FORMATSTRING                 *out,
                                         bsl::basic_string_view<t_CHAR>  value)
{
    out->d_formatString = value;
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMAT_STRING

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
