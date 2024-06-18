// bslfmt_formatstring.h                                              -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATSTRING
#define INCLUDED_BSLFMT_FORMATSTRING

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_string.h>

#include <stdexcept>


#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03 Generated on Tue Jun 18
// 08:08:38 2024 Command line: sim_cpp11_features.pl bslfmt_formatimp.h
#define COMPILING_BSLFMT_FORMATSTRING_H
#include <bslfmt_formatstring_cpp03.h>
#undef COMPILING_BSLFMT_FORMATSTRING_H
#else

namespace BloombergLP {
namespace bslfmt {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY

template <class t_CHAR, class... t_ARGS>
struct basic_format_string {
  public:
    // DATA
    bsl::basic_string_view<t_CHAR> d_formatString;

  public:
    // CREATORS
    template <class t_STR,
              class = typename bsl::enable_if<bsl::is_convertible<
                  t_STR,
                  bsl::basic_string_view<t_CHAR> >::value>::type>
    consteval basic_format_string(t_STR&& s)
    {
        d_formatString = std::forward<t_STR>(s);
    }

    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR> get()
    {
        return d_formatString;
    }

  private:
    // FRIENDS
    template <class t_INNER_CHAR, class... t_INNER_ARGS>
    friend class FormatString_Basic_Tester;
};

template <class... t_ARGS>
using format_string =
                 basic_format_string<char, bsl::type_identity_t<t_ARGS>...>;

template <class... t_ARGS>
using wformat_string =
                 basic_format_string<wchar_t, bsl::type_identity_t<t_ARGS>...>;

#elif BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

template <class t_CHAR, class... t_ARGS>
struct basic_format_string {
    bsl::basic_string_view<t_CHAR> d_formatString;

    BSLS_KEYWORD_CONSTEXPR_CPP14 basic_format_string(const t_CHAR *s)
    {
        d_formatString = s;
    }

    BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR> get()
    {
        return d_formatString;
    }

  private:
    // FRIENDS
    template <class t_INNER_CHAR, class... t_INNER_ARGS>
    friend class FormatString_Basic_Tester;
};

template <class... t_ARGS>
using format_string =
                 basic_format_string<char, bsl::type_identity_t<t_ARGS>...>;

template <class... t_ARGS>
using wformat_string =
                 basic_format_string<wchar_t, bsl::type_identity_t<t_ARGS>...>;

#else

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

template <class t_CHAR, class... t_ARGS>
struct basic_format_string {
    bsl::basic_string_view<t_CHAR> d_formatString;

    BSLS_KEYWORD_CONSTEXPR_CPP14 basic_format_string(const t_CHAR *s)
    {
        d_formatString = s;
    }

    BSLS_KEYWORD_CONSTEXPR bsl::basic_string_view<t_CHAR> get()
    {
        return d_formatString;
    }

  private:
    // FRIENDS
    template <class t_INNER_CHAR, class... t_INNER_ARGS>
    friend class FormatString_Basic_Tester;
};

template <class... t_ARGS>
using format_string =
                    basic_format_string<char, bsl::type_identity_t<t_ARGS>...>;

template <class... t_ARGS>
using wformat_string =
                  basic_format_string<wchar_t, bsl::type_identity_t<t_ARGS>...>;

#endif // !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

#endif // BSLS_LIBRARYFEATURES_HAS_CPP11/20_BASELINE_LIBRARY

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

template <class t_CHAR, class... t_ARGS>
struct FormatString_Basic_Tester
: public basic_format_string<t_CHAR, bsl::decay_t<t_ARGS>...> {
    FormatString_Basic_Tester(const t_CHAR *v)
    : basic_format_string<t_CHAR, bsl::decay_t<t_ARGS>...>("")
    {
        this->d_formatString = v;
    }

    FormatString_Basic_Tester(bsl::basic_string_view<t_CHAR> v)
    : basic_format_string<t_CHAR, bsl::decay_t<t_ARGS>...>("")
    {
        this->d_formatString = v;
    }
};

#endif  // !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

} // close namespace bslfmt
} // close enterprise namespace

#endif  // End C++11 code

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
