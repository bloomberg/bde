// bslfmt_formatstring.h                                              -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATSTRING
#define INCLUDED_BSLFMT_FORMATSTRING

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bslmf_enableif.h>
#include <bslmf_typeidentity.h>

#include <stdexcept>

//#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
//// Include version that can be compiled with C++03
//// Generated on Tue Jun 18 12:03:43 2024
//// Command line: sim_cpp11_features.pl bslfmt_formatstring.h
//# define COMPILING_BSLFMT_FORMATSTRING_H
//# include <bslfmt_formatstring_cpp03.h>
//# undef COMPILING_BSLFMT_FORMATSTRING_H
//#else

namespace BloombergLP {
namespace bslfmt {

// FORWARD DECLARATIONS

template <class t_CHAR>
struct Format_FormatString_Test_Updater;



                // --------------------------------------------
                // class basic_format_string<t_CHAR, t_ARGS...>
                // --------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)

template <class t_CHAR, class... t_ARGS>
class basic_format_string {
  private:
    // DATA
    bsl::basic_string_view<t_CHAR> d_formatString;

    // FRIENDS
    template <class t_INNER_CHAR>
    friend struct Format_FormatString_Test_Updater;

  public:
    // CREATORS
    template <class t_STR,
              class = typename bsl::enable_if<bsl::is_convertible<
                  t_STR,
                  bsl::basic_string_view<t_CHAR> >::value>::type>
    consteval basic_format_string(t_STR&& s);

    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR> get();
};

#elif defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)

template <class t_CHAR, class... t_ARGS>
class basic_format_string {
  private:
    // DATA
    bsl::basic_string_view<t_CHAR> d_formatString;

    // FRIENDS
    template <class t_INNER_CHAR>
    friend struct Format_FormatString_Test_Updater;

  public:
    // CREATORS
    BSLS_KEYWORD_CONSTEXPR_CPP14 basic_format_string(const t_CHAR *s);

     // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR> get();
};

#else // C++03

template <class t_CHAR>
class basic_format_string {
  private:
    // DATA
    bsl::basic_string_view<t_CHAR> d_formatString;

    // FRIENDS
    template <class t_INNER_CHAR>
    friend struct Format_FormatString_Test_Updater;

  public:
    // CREATORS
    BSLS_KEYWORD_CONSTEXPR_CPP14 basic_format_string(const t_CHAR *s);

    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR> get();
};

#endif

// ALIAS TEMPLATES AND TYPEDEFS

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)

template <class... t_ARGS>
using format_string =
                    basic_format_string<char, bsl::type_identity_t<t_ARGS>...>;

template <class... t_ARGS>
using wformat_string =
                 basic_format_string<wchar_t, bsl::type_identity_t<t_ARGS>...>;

#else // C++03

typedef basic_format_string<char>    format_string;
typedef basic_format_string<wchar_t> wformat_string;

#endif  // Support for Alias and Variadic templates

               // ----------------------------------------------
               // class Format_FormatString_Test_Updater<t_CHAR>
               // ----------------------------------------------


template <class t_CHAR>
struct Format_FormatString_Test_Updater
{
    template <class t_FORMATSTRING>
    static void update(t_FORMATSTRING *out, const t_CHAR *v);

    template <class t_FORMATSTRING>
    static void update(t_FORMATSTRING *out, bsl::basic_string_view<t_CHAR> v);
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                // --------------------------------------------
                // class basic_format_string<t_CHAR, t_ARGS...>
                // --------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)

// CREATORS
template <class t_CHAR, class... t_ARGS>
template <class t_STR, class>
consteval basic_format_string<t_CHAR, t_ARGS...>::basic_format_string(
                                                                     t_STR&& s)
{
    d_formatString = std::forward<t_STR>(s);
}

// ACCESSORS
template <class t_CHAR, class... t_ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR>
basic_format_string<t_CHAR, t_ARGS...>::get()
{
    return d_formatString;
}

#elif defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)

template <class t_CHAR, class... t_ARGS>
BSLS_KEYWORD_CONSTEXPR_CPP14
basic_format_string<t_CHAR, t_ARGS...>::basic_format_string(const t_CHAR *s)
{
    d_formatString = s;
}

template <class t_CHAR, class... t_ARGS>
BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR>
basic_format_string<t_CHAR, t_ARGS...>::get()
{
    return d_formatString;
}

#else // C++03

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP14
basic_format_string<t_CHAR>::basic_format_string(const t_CHAR *s)
{
    d_formatString = s;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR>
basic_format_string<t_CHAR>::get()
{
    return d_formatString;
}

#endif



               // ----------------------------------------------
               // class Format_FormatString_Test_Updater<t_CHAR>
               // ----------------------------------------------

template <class t_CHAR>
template <class t_FORMATSTRING>
void Format_FormatString_Test_Updater<t_CHAR>::update(
                                           t_FORMATSTRING *out,
                                           const t_CHAR   *v)
{
    out->d_formatString = v;
}

template <class t_CHAR>
template <class t_FORMATSTRING>
void Format_FormatString_Test_Updater<t_CHAR>::update(
                                           t_FORMATSTRING                 *out,
                                           bsl::basic_string_view<t_CHAR>  v)
{
    out->d_formatString = v;
}


} // close namespace bslfmt
} // close enterprise namespace

//#endif // End C++11 code

#endif  // INCLUDED_BSLFMT_FORMATSTRING

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
