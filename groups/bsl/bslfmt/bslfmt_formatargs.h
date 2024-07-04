// bslfmt_formatargs.h                                                -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATARGS
#define INCLUDED_BSLFMT_FORMATARGS

#include <bslscm_version.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_conditional.h>
#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isintegral.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>

#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_libraryfeatures.h>
#include <bsls_unspecifiedbool.h>
#include <bsls_util.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>
#include <bslstl_array.h>
#include <bslstl_monostate.h>
#include <bslstl_utility.h>
#include <bslstl_variant.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Tue Jun 18 08:19:54 2024
// Command line: sim_cpp11_features.pl bslfmt_formatargs.h
# define COMPILING_BSLFMT_FORMATARGS_H
# include <bslfmt_formatargs_cpp03.h>
# undef COMPILING_BSLFMT_FORMATARGS_H
#else

namespace BloombergLP {
namespace bslfmt {

// FORWARD DECLARATIONS

template <class t_OUT, class t_CHAR>
class basic_format_context;

template <class t_CONTEXT>
class basic_format_arg;

template <class t_CONTEXT>
class basic_format_args;

template <class t_VALUE>
class Format_OutputIteratorRef;

// TYPEDEFS

typedef basic_format_context<Format_OutputIteratorRef<char>, char>
    format_context;

typedef basic_format_context<Format_OutputIteratorRef<wchar_t>, wchar_t>
    wformat_context;

typedef basic_format_args<format_context> format_args;

typedef basic_format_args<wformat_context> wformat_args;

                 // ------------------------------------------
                 // class Format_FormatArgStore<t_OUT, T_CHAR>
                 // ------------------------------------------


#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_CONTEXT, class... t_ARGS>
class Format_FormatArgStore {

  private:
    // DATA
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> d_args;

    // FRIENDS
    template <class t_INNER_CONTEXT>
    friend class basic_format_args;

    template <class t_INNER_CONTEXT, class... t_INNER_ARGS>
    friend Format_FormatArgStore<t_INNER_CONTEXT, t_INNER_ARGS...>
        Format_MakeFormatArgs(t_INNER_ARGS&... fmt_args);

    // PRIVATE CREATORS
    explicit Format_FormatArgStore(
        const bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)>& args)
        BSLS_KEYWORD_NOEXCEPT;
};

// FREE FUNCTIONS

template <class t_CONTEXT, class... t_ARGS>
Format_FormatArgStore<t_CONTEXT, t_ARGS...> Format_MakeFormatArgs(
                                                          t_ARGS&... fmt_args);

template <class... t_ARGS>
Format_FormatArgStore<format_context, t_ARGS...> make_format_args(
                                                          t_ARGS&... fmt_args);

template <class... t_ARGS>
Format_FormatArgStore<wformat_context, t_ARGS...> make_wformat_args(
                                                          t_ARGS&... fmt_args);
#endif


                     // ----------------------------------
                     // class basic_format_args<t_CONTEXT>
                     // ----------------------------------


template <class t_CONTEXT>
class basic_format_args {
  private:
    // DATA
    size_t                             d_size;
    const basic_format_arg<t_CONTEXT> *d_data;

    // PRIVATE ACCESSORS
    size_t size() const;

    // FRIENDS
    template <class t_INNER_CONTEXT>
    friend size_t Format_FormatArgsSize(
                               const basic_format_args<t_INNER_CONTEXT>& args);

  public:
    // CREATORS
    basic_format_args() BSLS_KEYWORD_NOEXCEPT;

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... t_ARGS>
    basic_format_args(const Format_FormatArgStore<t_CONTEXT, t_ARGS...>& store)
        BSLS_KEYWORD_NOEXCEPT;                                     // IMPLICIT
#endif

    // ACCESSORS
    basic_format_arg<t_CONTEXT> get(size_t i) const BSLS_KEYWORD_NOEXCEPT;
};

// FREE FUNCTIONS

template<class t_CONTEXT>
size_t Format_FormatArgsSize(const basic_format_args<t_CONTEXT>& args);
    // This component-private function returns the result of calling 'size()'
    // on the specified 'args' parameter. This is to permit access to the
    // private 'size' accessor of 'basic_format_args' without requiring long
    // distance friendship.


// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                 // ------------------------------------------
                 // class Format_FormatArgStore<t_OUT, T_CHAR>
                 // ------------------------------------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

// PRIVATE CREATORS
template <class t_CONTEXT, class... t_ARGS>
Format_FormatArgStore<t_CONTEXT, t_ARGS...>::Format_FormatArgStore(
        const bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)>& args)
    BSLS_KEYWORD_NOEXCEPT : d_args(args)
{
}

// FREE FUNCTIONS

template <class t_CONTEXT, class... t_ARGS>
Format_FormatArgStore<t_CONTEXT, t_ARGS...> Format_MakeFormatArgs(
                                                           t_ARGS&... fmt_args)
{
    // Use the form of braced initialization that is valid in C++03
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> arg_array;
    Format_MakeFormatArgArray<t_CONTEXT, t_ARGS...>(&arg_array, fmt_args...);
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> arg_array2 = {
        {basic_format_arg<t_CONTEXT>(fmt_args)...}};
    return Format_FormatArgStore<t_CONTEXT, t_ARGS...>(arg_array);
}

template <class... t_ARGS>
Format_FormatArgStore<format_context, t_ARGS...> make_format_args(
                                                           t_ARGS&... fmt_args)
{
    return Format_MakeFormatArgs<format_context>(fmt_args...);
}

template <class... t_ARGS>
Format_FormatArgStore<wformat_context, t_ARGS...> make_wformat_args(
                                                           t_ARGS&... fmt_args)
{
    return Format_MakeFormatArgs<wformat_context>(fmt_args...);
}

#endif // !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

                     // ----------------------------------
                     // class basic_format_args<t_CONTEXT>
                     // ----------------------------------



// CREATORS
template <class t_CONTEXT>
basic_format_args<t_CONTEXT>::basic_format_args() BSLS_KEYWORD_NOEXCEPT
    : d_size(0) {}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_CONTEXT>
template <class... t_ARGS>
basic_format_args<t_CONTEXT>::basic_format_args(
                      const Format_FormatArgStore<t_CONTEXT, t_ARGS...>& store)
    BSLS_KEYWORD_NOEXCEPT                                           // IMPLICIT
: d_size(sizeof...(t_ARGS)),
  d_data(store.d_args.data())
{
}
#endif  // !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

// ACCESSORS
template <class t_CONTEXT>
basic_format_arg<t_CONTEXT> basic_format_args<t_CONTEXT>::get(
                                          size_t i) const BSLS_KEYWORD_NOEXCEPT
{
    return i < d_size ? d_data[i] : basic_format_arg<t_CONTEXT>();
}


// PRIVATE ACCESSORS
template <class t_CONTEXT>
size_t basic_format_args<t_CONTEXT>::size() const
{
    return d_size;
}

// FREE FUNCTIONS

template <class t_CONTEXT>
size_t Format_FormatArgsSize(const basic_format_args<t_CONTEXT>& args)
{
    return args.size();
}

}  // close namespace bslfmt
} // close enterprise namespace

#endif // End C++11 code

#endif  // INCLUDED_BSLFMT_FORMATARG

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
