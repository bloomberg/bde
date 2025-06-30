// bslfmt_streamed.h                                                  -*-C++-*-

#ifndef INCLUDED_BSLFMT_STREAMED
#define INCLUDED_BSLFMT_STREAMED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a wrapper to format using an `ostream` `operator<<`
//
//@CLASSES:
//  bslfmt::Streamed: formattable wrapper that uses `ostream` `operator<<`
//
//@DESCRIPTION: This component provides both a wrapper class template and a
// function template for creating a wrapper that enables `bsl::format`ing
// values that offer an `ostream` insert `operator<<`.  This wrapper is also
// compatible with `std::format` on platforms where it is provided.
//
//```
// bsl::format("Example: {}", bslfmt::streamed(ATypeWithoutAFormatter(42));
//```
//
// Using the `bslfmt::streamed` free function may be preferred to using the
// `bslfmt::Streamed` type directly because the `bslfmt::streamed`
// free-function supports simpler syntax on platforms that do not support class
// template argument deduction (CTAD, introduced in C++17).
//
// For more (important) information please see
// [the package documentation](bslfmt#Streaming-based Formatting).
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Formatting a Streamable Object
/// - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<`.
//
// First, we define a type with a streaming operator but without a formatter
// specialization:
//```
//  class NonFormattableType {};
//
//  std::ostream& operator<<(std::ostream& os, const NonFormattableType&)
//  {
//      return os << "The printout";
//  }
//```
// Then we create an instance of this type and use bsl::streamed to allow us to
// format it:
//```
//  const NonFormattableType obj;
//```
// Next, we format the "value" using `bsl::format` with the wrapper-creator
// function:
//```
// bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
//```
// Finally, we verify the output is correct:
//```
// assert(s == "The printout");
//```

#include <bslscm_version.h>

#include <bslfmt_format.h>
#include <bslfmt_streamedformatter.h>

#include <bsls_deprecatefeature.h>
#include <bsls_keyword.h>

namespace BloombergLP {
namespace bslfmt {

                                 // ========
                                 // Streamed
                                 // ========

/// This class provides a wrapper to enable a streamable type to be used with
/// `bsl::format` (and `std::format`) when no formatter specialization is
/// provided (the type is not formattable).  Note that it is possible to use
/// this wrapper with types that have a formatter, if the compiler supports it
/// we give a compilation warning directing the user to use the existing
/// formatter instead of wrapping to use the streaming operator.
template <class t_STREAMABLE>
class Streamed {
  private:
    // DATA
    const t_STREAMABLE& d_object;

  private:
    // PRIVATE CREATORS

#ifdef BSL_FORMATTABLE_DEFINED
    /// Create a `Streamed` wrapping the specified `object`.  The behavior is
    /// undefined unless the lifetime of `object` is at least as long as that
    /// of the wrapper created.  This constructor is the one used from the
    /// `bslfmt::streamed` wrapper-creator function to avoid two warnings for
    /// types that are already formattable.
    Streamed(const t_STREAMABLE& object, Streamed_NoWarningConstructorTag);

    // FRIENDS
    template <class t_STREAMABLE2>
    friend Streamed<t_STREAMABLE2> streamed(const t_STREAMABLE2& object)
        requires(!bsl::formattable<t_STREAMABLE2, char>);
    template <class t_STREAMABLE2>
    friend Streamed<t_STREAMABLE2> streamed(const t_STREAMABLE2& object)
        requires(bsl::formattable<t_STREAMABLE2, char>);
#endif

  public:
    // CREATORS

    /// Create a `Streamed` wrapper instance around the specified `object`.
    /// The behavior is undefined unless the lifetime of `object` is at least
    /// as long as that of the wrapper created.
#ifdef BSL_FORMATTABLE_DEFINED
    Streamed(const t_STREAMABLE& object)
        requires(!bsl::formattable<t_STREAMABLE, char>);
    BSLFMT_STREAMED_INSTANCE_DEPRECATED_
    Streamed(const t_STREAMABLE& object)
        requires(bsl::formattable<t_STREAMABLE, char>);
#else   // BSL_FORMATTABLE_DEFINED
    Streamed(const t_STREAMABLE& object);
#endif  // else of BSL_FORMATTABLE_DEFINED

    // ACCESSORS

    /// Return a non-modifiable reference to the wrapped object.
    const t_STREAMABLE& object() const;
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CTAD) &&                            \
    defined(__apple_build_version__) && BSLS_PLATFORM_CMP_VERSION < 180000
// The following deduction guide is needed for Apple versions of clang that do
// not support CTAD with concepts present due to CWG issue 2628 resolution not
// yet applied and therefore would give an ambiguity error indicating it does
// not know which constructor to use for CTAD.
template <class t_TYPE>
Streamed(const t_TYPE& object) -> Streamed<t_TYPE>;
#endif  // CTAD is supported and on older Apple clang

// FREE FUNCTIONS

#ifdef BSL_FORMATTABLE_DEFINED
/// Create and return a `bslfmt::Streamed` (wrapper) object for the specified
/// streamable `object`.  The behavior is undefined unless `t_STREAMABLE` has a
/// standard and well-behaved `std::ostream` output operator.  This
/// wrapper-creator free function to support compilation with no CTAD (Class
/// Template Argument Deduction); see (#Usage).  Notice that for compilations
/// that support the `bsl::formattable` trait we define two concept-driven
/// overloads of this function, one of which will warn the user (compiler
/// warning) if a `bsl::formatter` already exists for the type they try to
/// streamed-wrap.
template <class t_STREAMABLE>
Streamed<t_STREAMABLE> streamed(const t_STREAMABLE& object)
    requires(!bsl::formattable<t_STREAMABLE, char>);

template <class t_STREAMABLE>
BSLFMT_STREAMED_INSTANCE_DEPRECATED_ Streamed<t_STREAMABLE>
streamed(const t_STREAMABLE& object)
    requires(bsl::formattable<t_STREAMABLE, char>);
#else   // BSL_FORMATTABLE_DEFINED
template <class t_STREAMABLE>
Streamed<t_STREAMABLE> streamed(const t_STREAMABLE& object);
#endif  // else of BSL_FORMATTABLE_DEFINED

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                             // --------
                             // Streamed
                             // --------

// PRIVATE CREATORS
#ifdef BSL_FORMATTABLE_DEFINED
template <class t_STREAMABLE>
inline
Streamed<t_STREAMABLE>::Streamed(const t_STREAMABLE&              object,
                                 Streamed_NoWarningConstructorTag)
: d_object(object)
{
}
#endif  // BSL_FORMATTABLE_DEFINED

// ACCESSORS
template <class t_STREAMABLE>
inline
const t_STREAMABLE& Streamed<t_STREAMABLE>::object() const
{
    return d_object;
}

// CREATORS
#ifdef BSL_FORMATTABLE_DEFINED
template <class t_STREAMABLE>
Streamed<t_STREAMABLE>::Streamed(const t_STREAMABLE& object)
    requires(!bsl::formattable<t_STREAMABLE, char>)
: d_object(object)
{
}

template <class t_STREAMABLE>
BSLFMT_STREAMED_INSTANCE_DEPRECATED_
Streamed<t_STREAMABLE>::Streamed(const t_STREAMABLE& object)
    requires(bsl::formattable<t_STREAMABLE, char>)
: d_object(object)
{
}
#else   // BSL_FORMATTABLE_DEFINED
template <class t_STREAMABLE>
Streamed<t_STREAMABLE>::Streamed(const t_STREAMABLE& object)
: d_object(object)
{
}
#endif  // else of BSL_FORMATTABLE_DEFINED
}  // close package namespace

// FREE FUNCTIONS
#ifdef BSL_FORMATTABLE_DEFINED
template <class t_STREAMABLE>
bslfmt::Streamed<t_STREAMABLE> bslfmt::streamed(const t_STREAMABLE& object)
    requires(!bsl::formattable<t_STREAMABLE, char>)
{
    return Streamed<t_STREAMABLE>(object, Streamed_NoWarningConstructorTag());
}

template <class t_STREAMABLE>
BSLFMT_STREAMED_INSTANCE_DEPRECATED_
bslfmt::Streamed<t_STREAMABLE>
bslfmt::streamed(const t_STREAMABLE& object)
    requires(bsl::formattable<t_STREAMABLE, char>)
{
    return Streamed<t_STREAMABLE>(object, Streamed_NoWarningConstructorTag());
}
#else  // BSLFMT_FORMATTABLE_DEFINED
template <class t_STREAMABLE>
bslfmt::Streamed<t_STREAMABLE> bslfmt::streamed(const t_STREAMABLE& object)
{
    return Streamed<t_STREAMABLE>(object);
}
#endif  // BSLFMT_FORMATTABLE_DEFINED
}  // close enterprise namespace

namespace bsl {

/// Standard (`bsl`) formatter for the `bslfmt::Streamed` wrapper, delegates to
/// `bslfmt::StreamedFormatter` of the wrapped type.
template <class t_STREAMABLE>
struct formatter<BloombergLP::bslfmt::Streamed<t_STREAMABLE>, char> {
  private:
    // DATA
    BloombergLP::bslfmt::StreamedFormatter<t_STREAMABLE> d_formatter;

  public:
    // MANIPULATORS

    /// Parse and validate the specification string stored in the iterator
    /// accessed via the `begin()` method of the parseContext passed via the
    /// specified `parseContext` parameter.  Where nested parameters are
    /// encountered in the specification string then the `next_arg_id` and
    /// `check_arg_id` are called on `fc` as specified in the C++ Standard.
    /// Return an end iterator of the parsed range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                 t_PARSE_CONTEXT& parseContext)
    {
        return d_formatter.parse(parseContext);
    }

    // ACCESSORS

    /// Format the specified `value` according to the specification stored as a
    /// result of a previous call to the `parse` method, and write the result
    /// to the iterator accessed by calling the `out()` method on the specified
    /// `formatContext` parameter.  Return an end iterator of the output range.
    /// Throw an exception of type `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
        const BloombergLP::bslfmt::Streamed<t_STREAMABLE>& value,
        t_FORMAT_CONTEXT&                                  formatContext) const
    {
        return d_formatter.format(value.object(), formatContext);
    }
};

}  // close namespace bsl

#undef BSLFMT_STREAMED_INSTANCE_DEPRECATED_

#endif  // INCLUDED_BSLFMT_STREAMED

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
