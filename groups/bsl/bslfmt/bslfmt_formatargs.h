// bslfmt_formatargs.h                                                -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATARGS
#define INCLUDED_BSLFMT_FORMATARGS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container of arguments for use by bsl::format
//
//@CLASSES:
//  bslfmt::basic_format_args: standard-compliant argment store
//
//@DESCRIPTION: This component provides an implementation of the C++20 Standard
// Library's `basic_format_args`, providing access to an array of
// `basic_format_arg` types.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Construct a `basic_format_args` object and verify what it contains.
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`. In addition, there
// are only a very limited number of public methods so this example is
// necessarily unrealistic.
//
// Suppose we want to construct a `basic_format_args` containing a single int.
//
//..
//   bslfmt::format_args args(bslfmt::make_format_args((int)5);
//
//   assert( args.get(0));
//   assert(!args.get(1));
//..
//

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

#include <bslfmt_formatarg.h>
#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Jul  4 12:04:46 2024
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
/// This component-private type holds an array of `basic_format_arg` objects
/// and is intended to be implicitly convertible to `basic_format_args`. Note
/// that this type has reference semantics and users must ensure that this type
/// does not outlive the arguments used in its construction.
template <class t_CONTEXT, class... t_ARGS>
class Format_FormatArgStore {

  private:
    // DATA
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> d_args;
                                             // Contained array of arguments.

    // FRIENDS
    template <class t_INNER_CONTEXT>
    friend class basic_format_args;

    friend class Format_FormatArgs_ImpUtil;

    // PRIVATE CREATORS

    /// Construct a `Format_FormatArgStore` containing a copy of the specified
    /// `args`.
    explicit Format_FormatArgStore(
        const bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)>& args)
        BSLS_KEYWORD_NOEXCEPT;
};

#endif


                               // --------------
                               // FREE FUNCTIONS
                               // --------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// From the specified `fmt_args` return a type convertible to
/// `format_arg_store` that is suitable for use by the `char`-based
/// `bslfmt::format` functions.
template <class... t_ARGS>
Format_FormatArgStore<format_context, t_ARGS...> make_format_args(
                                                          t_ARGS&... fmt_args);

/// From the specified `fmt_args` return a type convertible to
/// `format_arg_store` that is suitable for use by the `wchar_t`-based
/// `bslfmt::format` functions.
template <class... t_ARGS>
Format_FormatArgStore<wformat_context, t_ARGS...> make_wformat_args(
                                                          t_ARGS&... fmt_args);
#endif


                     // ----------------------------------
                     // class basic_format_args<t_CONTEXT>
                     // ----------------------------------

/// A class intended to provide access to formatting arguments. This should not
/// be constructed directly but indirectly using one of the `make_format_args`
/// or the `make_wformat_args` functions. Note that this type has reference
/// semantics and users must ensure that this type does not outlive the
/// arguments used in its construction.
template <class t_CONTEXT>
class basic_format_args {
  private:
    // DATA
    size_t                             d_size;   // Number of arguments held
    const basic_format_arg<t_CONTEXT> *d_data;   // Pointer to the first element
                                                 // of the contained array.

    // PRIVATE ACCESSORS

    // Return the number of arguments contained within this object.
    size_t size() const;

    // FRIENDS
    friend class Format_FormatArgs_ImpUtil;

  public:
    // CREATORS

    // Construct a `basic_format_args` object which contains no arguments.
    basic_format_args() BSLS_KEYWORD_NOEXCEPT;

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    // Construct a `basic_format_args` object which contains the arguments held
    // by the specified `store`. Typically this constructor would be called
    // using the return value of `make_format_args` or of `make_wformat_args`.
    template <class... t_ARGS>
    basic_format_args(const Format_FormatArgStore<t_CONTEXT, t_ARGS...>& store)
        BSLS_KEYWORD_NOEXCEPT;                                     // IMPLICIT
#endif

    // ACCESSORS

    // Return the argument held at the position given by the specified `pos`.
    // If `pos >= size()` then a default-constructed `basic_format_arg` is
    // returned.
    basic_format_arg<t_CONTEXT> get(size_t pos) const BSLS_KEYWORD_NOEXCEPT;
};

                      // -------------------------------
                      // class Format_FormatArgs_ImpUtil
                      // -------------------------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

/// This class provides utility functions to enable manipulation of types
/// declared by this component. It is solely for private use by other components
/// of the `bslfmt` package and should not be used directly.
class Format_FormatArgs_ImpUtil {
  public:
    // CLASS METHODS

    /// Construct a `Format_FormatArgStore` object containing
    /// `basic_format_arg` objects constructed from the specified `fmt_args`
    /// values.
    template <class t_CONTEXT, class... t_ARGS>
    static Format_FormatArgStore<t_CONTEXT, t_ARGS...> makeFormatArgs(
                                                          t_ARGS&... fmt_args);

    /// Call 'size()' on the specified 'args' parameter and return the result.
    /// This is to permit access to the private 'size' accessor of
    /// 'basic_format_args' without requiring long distance friendship.
    template <class t_CONTEXT>
    static size_t formatArgsSize(const basic_format_args<t_CONTEXT>& args);
};

#endif

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

#endif


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
#endif

// ACCESSORS
template <class t_CONTEXT>
basic_format_arg<t_CONTEXT> basic_format_args<t_CONTEXT>::get(
                                        size_t pos) const BSLS_KEYWORD_NOEXCEPT
{
    return pos < d_size ? d_data[pos] : basic_format_arg<t_CONTEXT>();
}


// PRIVATE ACCESSORS
template <class t_CONTEXT>
size_t basic_format_args<t_CONTEXT>::size() const
{
    return d_size;
}


                      // -------------------------------
                      // class Format_FormatArgs_ImpUtil
                      // -------------------------------

// CLASS METHODS

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

template <class t_CONTEXT, class... t_ARGS>
Format_FormatArgStore<t_CONTEXT, t_ARGS...>
Format_FormatArgs_ImpUtil::makeFormatArgs(t_ARGS&... fmt_args)
{
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> arg_array;
    Format_FormatArg_ImpUtil::makeFormatArgArray<t_CONTEXT, t_ARGS...>(
                                                                  &arg_array,
                                                                  fmt_args...);
    return Format_FormatArgStore<t_CONTEXT, t_ARGS...>(arg_array);
}

#endif

template <class t_CONTEXT>
size_t Format_FormatArgs_ImpUtil::formatArgsSize(
                                      const basic_format_args<t_CONTEXT>& args)
{
    return args.size();
}


                               // --------------
                               // FREE FUNCTIONS
                               // --------------

// FREE FUNCTIONS

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

template <class... t_ARGS>
Format_FormatArgStore<format_context, t_ARGS...> make_format_args(
                                                           t_ARGS&... fmt_args)
{
    return Format_FormatArgs_ImpUtil::makeFormatArgs<format_context>(
                                                                  fmt_args...);
}

template <class... t_ARGS>
Format_FormatArgStore<wformat_context, t_ARGS...> make_wformat_args(
                                                           t_ARGS&... fmt_args)
{
    return Format_FormatArgs_ImpUtil::makeFormatArgs<wformat_context>(
                                                                  fmt_args...);
}

#endif

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
