// bslfmt_format_args.h                                               -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMAT_ARGS
#define INCLUDED_BSLFMT_FORMAT_ARGS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container of arguments for use by bsl::format
//
//@CLASSES:
//  bslfmt::basic_format_args: standard-compliant argument store
//
//@CANONICAL_HEADER: bsl_format.h
//
//@DESCRIPTION: This component provides an implementation of the C++20 Standard
// Library's `std::basic_format_args`, providing access to an array of
// `basic_format_arg` types.  It also provides implementations of the standard
// library free functions `make_format_args` and `make_wformat_args`.
//
// As also specified by the standard, the provided free functions return an
// exposition-only type that holds *references to* the arguments passed in.  A
// `basic_format_args` constructed from an instance of this type holds a
// reference to it.  This means it is the users responsibility to ensure that
// the lifetime of the returned type does not end before the lifetime of the
// constructed `basic_format_args` type.  This means that, for example, the
// following code results in Undefined Behavior, in both the standard library
// and the `bslfmt` versions:
// ```
//     int value = 5;
//     format_args args = make_format_args(value);
//     // args now holds a reference to a temporary whose lifetime has ended.
//     do_something_with(args);
// ```
//
// This header is not intended to be included directly.  Please include
// `<bsl_format.h>` to be able to use `bsl::basic_format_args`.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: 1 Non-default construction and value verification
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`.  In addition,
// there are only a very limited number of public methods so this example is
// necessarily unrealistic.
//
// Suppose we want to construct a int-containing `basic_format_args` and verify
// that it contains that int.  Note the use of a function to workaround the
// lifetime issues specified above.  `Format_ArgsStore` passed to the
// `basic_format_args` constructor must outlive the constructed object.
// `bslfmt::make_format_args` returns temporary `Format_ArgsStore` object so to
// avoid its destruction we have to do all the useful work within the
// execution of one function.
// ```
//   struct UsageExampleVisitor {
//
//     void operator()(bsl::monostate) const
//     {
//       assert(false); // contains no value
//     }
//
//     template <class t_TYPE>
//     typename bsl::enable_if<bsl::is_integral<t_TYPE>::value>::type
//                                           operator()(t_TYPE x) const
//     {
//       assert(static_cast<t_TYPE>(99) == x);
//     }
//
//     template <class t_TYPE>
//     typename bsl::enable_if<!bsl::is_integral<t_TYPE>::value>::type
//                                           operator()(t_TYPE) const
//     {
//       assert(false); // contains non-integral value
//     }
//   };
//
//   struct UsageExampleChecker {
//     static void checkValue(bslfmt::format_args args)
//     {
//       UsageExampleVisitor visitor;
//       visit_format_arg(visitor, args.get(0));
//       asssert(args.get(1));
//     }
//   };
//
//   int value = 99;
//   UsageExampleChecker::checkValue(bslfmt::make_format_args(value));
// ```

#include <bslscm_version.h>

#include <bslfmt_format_arg.h>
#include <bslfmt_formaterror.h>

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

#include <bslstl_array.h>
#include <bslstl_iterator.h>
#include <bslstl_monostate.h>
#include <bslstl_utility.h>
#include <bslstl_variant.h>
#include <bslstl_vector.h>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// clang-format off
// Include version that can be compiled with C++03
// Generated on Thu Jan 23 14:54:23 2025
// Command line: sim_cpp11_features.pl bslfmt_format_args.h

# define COMPILING_BSLFMT_FORMAT_ARGS_H
# include <bslfmt_format_args_cpp03.h>
# undef COMPILING_BSLFMT_FORMAT_ARGS_H

// clang-format on
#else

namespace BloombergLP {
namespace bslfmt {

// FORWARD DECLARATIONS

template <class t_OUT, class t_CHAR>
class basic_format_context;

template <class t_CONTEXT>
class basic_format_args;

template <class t_VALUE>
class Format_ContextOutputIteratorRef;

// TYPEDEFS

typedef basic_format_context<Format_ContextOutputIteratorRef<char>, char>
    format_context;

typedef basic_format_context<Format_ContextOutputIteratorRef<wchar_t>, wchar_t>
    wformat_context;

typedef basic_format_args<format_context> format_args;

typedef basic_format_args<wformat_context> wformat_args;

                        // ======================
                        // class Format_ArgsStore
                        // ======================

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// This component-private type holds an array of `basic_format_arg` objects
/// and is intended to be implicitly convertible to `basic_format_args`.  Note
/// that this type has reference semantics and users must ensure that this type
/// does not outlive the arguments used in its construction.
template <class t_CONTEXT, class... t_ARGS>
class Format_ArgsStore {

  private:
    // DATA
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> d_args;
                                             // Contained array of arguments.

    // FRIENDS
    template <class t_INNER_CONTEXT>
    friend class basic_format_args;

    friend class Format_ArgsUtil;

    // PRIVATE CREATORS

    /// Create a `Format_ArgsStore` containing a copy of the specified
    /// `args`.
    explicit Format_ArgsStore(
        const bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)>& args)
        BSLS_KEYWORD_NOEXCEPT;

    explicit Format_ArgsStore(
         bslmf::MovableRef<
             bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> > args)
        BSLS_KEYWORD_NOEXCEPT;
};

#endif

// FREE FUNCTIONS

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// From the specified `fmt_args` return a type convertible to
/// `format_arg_store` that is suitable for use by the `char`-based
/// `bslfmt::format` functions.
template <class... t_ARGS>
Format_ArgsStore<format_context, t_ARGS...> make_format_args(
                                                          t_ARGS&... fmt_args);

/// From the specified `fmt_args` return a type convertible to
/// `format_arg_store` that is suitable for use by the `wchar_t`-based
/// `bslfmt::format` functions.
template <class... t_ARGS>
Format_ArgsStore<wformat_context, t_ARGS...> make_wformat_args(
                                                          t_ARGS&... fmt_args);
#endif


                         // =======================
                         // class basic_format_args
                         // =======================

/// A class intended to provide access to formatting arguments.  This should
/// not be constructed directly but indirectly using one of the
/// `make_format_args` or the `make_wformat_args` functions.  Note that this
/// type has reference semantics and users must ensure that this type does not
/// outlive the arguments used in its construction.
template <class t_CONTEXT>
class basic_format_args {
  private:
    // DATA
    size_t                             d_size;    // number of arguments held

    const basic_format_arg<t_CONTEXT> *d_data_p;  // pointer to the first
                                                  // element of the contained
                                                  // array.

    // PRIVATE ACCESSORS

    /// Return the number of arguments contained within this object.
    size_t size() const;

    // FRIENDS
    friend class Format_ArgsUtil;

  public:
    // CREATORS

    /// Create a `basic_format_args` object which contains no arguments.
    basic_format_args() BSLS_KEYWORD_NOEXCEPT;

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Create a `basic_format_args` object which contains the arguments held
    /// by the specified `store`. Typically this constructor would be called
    /// using the return value of `make_format_args` or of `make_wformat_args`.
    template <class... t_ARGS>
    basic_format_args(const Format_ArgsStore<t_CONTEXT, t_ARGS...>& store)
        BSLS_KEYWORD_NOEXCEPT;                                     // IMPLICIT
#endif

    // ACCESSORS

    /// Return the argument held at the position given by the specified `pos`.
    /// If `pos >= size()` then a default-constructed `basic_format_arg` is
    /// returned.
    basic_format_arg<t_CONTEXT> get(size_t pos) const BSLS_KEYWORD_NOEXCEPT;
};

                           // ---------------------
                           // class Format_ArgsUtil
                           // ---------------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

/// This class provides utility functions to enable manipulation of types
/// declared by this component.  It is solely for private use by other
/// components of the `bslfmt` package and should not be used directly.
class Format_ArgsUtil {
  public:
    // CLASS METHODS

    /// Create a `Format_ArgsStore` object containing
    /// `basic_format_arg` objects constructed from the specified `fmt_args`
    /// values.
    template <class t_CONTEXT, class... t_ARGS>
    static Format_ArgsStore<t_CONTEXT, t_ARGS...> makeFormatArgs(
                                                          t_ARGS&... fmt_args);

    /// Call `size()` on the specified `args` parameter and return the result.
    /// This is to permit access to the private `size` accessor of
    /// `basic_format_args` without requiring long distance friendship.
    template <class t_CONTEXT>
    static size_t formatArgsSize(const basic_format_args<t_CONTEXT>& args);
};

#endif

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                   // -------------------------------------
                   // class Format_ArgsStore<t_OUT, T_CHAR>
                   // -------------------------------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

// PRIVATE CREATORS
template <class t_CONTEXT, class... t_ARGS>
inline
Format_ArgsStore<t_CONTEXT, t_ARGS...>::Format_ArgsStore(
        const bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)>& args)
    BSLS_KEYWORD_NOEXCEPT : d_args(args)
{
}

template <class t_CONTEXT, class... t_ARGS>
inline
Format_ArgsStore<t_CONTEXT, t_ARGS...>::Format_ArgsStore(
         bslmf::MovableRef<
             bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> > args)
    BSLS_KEYWORD_NOEXCEPT : d_args(bslmf::MovableRefUtil::move(args))
{
}

#endif

                     // ----------------------------------
                     // class basic_format_args<t_CONTEXT>
                     // ----------------------------------

// CREATORS
template <class t_CONTEXT>
inline
basic_format_args<t_CONTEXT>::basic_format_args() BSLS_KEYWORD_NOEXCEPT
: d_size(0)
, d_data_p(0)
{}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_CONTEXT>
template <class... t_ARGS>
inline
basic_format_args<t_CONTEXT>::basic_format_args(
                      const Format_ArgsStore<t_CONTEXT, t_ARGS...>& store)
    BSLS_KEYWORD_NOEXCEPT                                           // IMPLICIT
: d_size(sizeof...(t_ARGS))
, d_data_p(store.d_args.data())
{
}
#endif

// ACCESSORS
template <class t_CONTEXT>
inline
basic_format_arg<t_CONTEXT> basic_format_args<t_CONTEXT>::get(
                                        size_t pos) const BSLS_KEYWORD_NOEXCEPT
{
    return pos < d_size ? d_data_p[pos] : basic_format_arg<t_CONTEXT>();
}


// PRIVATE ACCESSORS
template <class t_CONTEXT>
inline
size_t basic_format_args<t_CONTEXT>::size() const
{
    return d_size;
}


                           // ---------------------
                           // class Format_ArgsUtil
                           // ---------------------

// CLASS METHODS

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

template <class t_CONTEXT, class... t_ARGS>
inline
Format_ArgsStore<t_CONTEXT, t_ARGS...>
Format_ArgsUtil::makeFormatArgs(t_ARGS&... fmt_args)
{
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> arg_array;
    Format_ArgUtil::makeFormatArgArray<t_CONTEXT, t_ARGS...>(&arg_array,
                                                             fmt_args...);
    return Format_ArgsStore<t_CONTEXT, t_ARGS...>(
                                       bslmf::MovableRefUtil::move(arg_array));
}

#endif

template <class t_CONTEXT>
inline
size_t Format_ArgsUtil::formatArgsSize(
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
inline
Format_ArgsStore<format_context, t_ARGS...> make_format_args(
                                                           t_ARGS&... fmt_args)
{
    return Format_ArgsUtil::makeFormatArgs<format_context>(fmt_args...);
}

template <class... t_ARGS>
inline
Format_ArgsStore<wformat_context, t_ARGS...> make_wformat_args(
                                                           t_ARGS&... fmt_args)
{
    return Format_ArgsUtil::makeFormatArgs<wformat_context>(fmt_args...);
}

#endif

}  // close package namespace
}  // close enterprise namespace

#endif // End C++11 code

#endif  // INCLUDED_BSLFMT_FORMAT_ARGS

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
