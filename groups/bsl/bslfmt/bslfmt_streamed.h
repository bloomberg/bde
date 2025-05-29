// bslfmt_streamed.h                                                  -*-C++-*-

#ifndef INCLUDED_BSLFMT_STREAMED
#define INCLUDED_BSLFMT_STREAMED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a wrapper to format using the `ostream` insert operator
//
//@CLASSES:
//  bsl::Streamed<t_TYPE>: narrow `char` streaming wrapper class template
//
//@DESCRIPTION: This component provides both a wrapper class template and (for
// use with C++ standards that have no class template argument deduction) a
// wrapper-creator function template to enable `bsl::format`ting values that
// already offer an `ostream` insert `operator<<`.  (On platforms that have a
// working `std::format` the wrapper enables that, too.)
//
///The Format String
///-----------------
// The wrapper's formatter supports the same format string syntax as formatting
// a string-like type: alignment, width, and truncation using the "precision"
// value.  See Format String Options usage example.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example 1: Formatting a Streamable Object
///- - - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<`.  When writing portable code that
// should work on compilers that do not support class template argument
// deduction we would use the wrapper-creator function `bslfmt::streamed` to
// avoid having to know and write the type of the object.
//
// First, for the sake of demonstration we create a type with an obscure and
// long name that we neither want to remember nor ever to write down, and which
// can be streamed out:
//```
//  class ThisTypeHasLongAndObscureNameButStreamable {
//  };
//
//  std::ostream& operator<<(
//                       std::ostream&                                     os,
//                       const ThisTypeHasLongAndObscureNameButStreamable& )
//  {
//      return os << "The printout";
//  }
//```
// Then, we create an object of said type that we want to print out:
//```
//  const ThisTypeHasLongAndObscureNameButStreamable obj;
//```
// Next, we format the "value" using `bsl::format` with the wrapper-creator
// function:
//```
//  bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
//```
// Finally, we verify the output is correct:
//```
//  assert(s == "The printout");
//```
//
///Example 2: Format String Options
/// - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<` for an environment that requires the
// output to be padded, aligned, and/or truncation of the output.  In this
// example we will introduce the effects of the various possible format string
// specifications.
//
// First, for the sake of demonstration we create a type that prints a series
// of digits to help demonstrate the effects of the various formattings:
//```
//  class Streamable {
//  };
//
//  std::ostream& operator<<(std::ostream& os, const Streamable&)
//  {
//      return os << "12345678";
//  }
//```
// Then, we create an object of said type that we will format:
//
//```
//  const Streamable obj;
//```
// Next, we format the "value" using many different format strings, starting
// with the default for completeness:
//```
//  bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
//  assert(s == "12345678");
//```
// Then, we format with specifying just a width:
//```
//  s = bsl::format("{:10}", bslfmt::streamed(obj));
//  assert(s == "12345678  ");
//```
// Next, we format with specifying a width, and alignments:
//```
//  s = bsl::format("{:<10}", bslfmt::streamed(obj));
//  assert(s == "12345678  ");
//
//  s = bsl::format("{:^10}", bslfmt::streamed(obj));
//  assert(s == " 12345678 ");
//
//  s = bsl::format("{:>10}", bslfmt::streamed(obj));
//  assert(s == "  12345678");
//```
// Finally, we demonstrate the truncation using a "precision" value:
//```
//  s = bsl::format("{:.6}", bslfmt::streamed(obj));
//  assert(s == "123456");
//
//  s = bsl::format("{:8.6}", bslfmt::streamed(obj));
//  assert(s == "123456  ");
//
//  s = bsl::format("{:<8.6}", bslfmt::streamed(obj));
//  assert(s == "123456  ");
//
//  s = bsl::format("{:^8.6}", bslfmt::streamed(obj));
//  assert(s == " 123456 ");
//
//  s = bsl::format("{:>8.6}", bslfmt::streamed(obj));
//  assert(s == "  123456");
//```

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formattable.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterstring.h>

#include <bslstl_iterator.h>
#include <bslstl_ostringstream.h>
#include <bslstl_string.h>

#include <bsls_deprecatefeature.h>
#include <bsls_keyword.h>

#include <limits>     // for 'std::numeric_limits'

#include <streambuf>

namespace BloombergLP {
namespace bslfmt {

#define BSLFMT_STREAMED_INSTANCE_DEPRECATED_                                  \
    BSLS_DEPRECATE_FEATURE(                                                   \
            "bslfmt",                                                         \
            "streamed",                                                       \
            "There is already a formatter available for the specified type, " \
            "please use that instead of streaming.")

                      // ================================
                      // Streamed_NoWarningConstructorTag
                      // ================================

/// Just a tag type for the private constructor of `Streamed` that is used from
/// the `streamed` wrapper-creator method.
#ifdef BSL_FORMATTABLE_DEFINED
struct Streamed_NoWarningConstructorTag {
};
#endif

                                 // ========
                                 // Streamed
                                 // ========

/// A wrapper class to be used with streamable (`ostream::operator<<`) types
/// that adds `bsl::format` capability to the type (and `std::format` when that
/// is available).
template <class t_STREAMABLE>
class Streamed {
  private:
    // DATA
    const t_STREAMABLE& d_object;

  private:
    // PRIVATE CREATORS

#ifdef BSL_FORMATTABLE_DEFINED
    /// Create a wrapper instance around the specified `object`.  The behavior
    /// is undefined unless the lifetime of `object` is at least as long as
    /// that of the wrapper created.  This constructor is the one used from the
    /// `bslfmt::streamed` wrapper-creator function to avoid two warnings for
    /// types that are already formattable.
    Streamed(const t_STREAMABLE& object, Streamed_NoWarningConstructorTag);

    // FRIENDS
    template <class t_STREAMABLE2>
    friend
    Streamed<t_STREAMABLE2> streamed(const t_STREAMABLE2& object)
    requires(!bsl::formattable<t_STREAMABLE2, char>);
    template <class t_STREAMABLE2>
    friend
    Streamed<t_STREAMABLE2> streamed(const t_STREAMABLE2& object)
    requires(bsl::formattable<t_STREAMABLE2, char>);
#endif

  public:
    // CREATORS

    /// Create a `Streamed` wrapper instance around the specified `object`.
    /// The behavior is undefined unless the lifetime of `object` is at least
    /// as long as that of the wrapper created.
    Streamed(const t_STREAMABLE& object)
#ifdef BSL_FORMATTABLE_DEFINED
    requires(!bsl::formattable<t_STREAMABLE, char>)
#endif  // BSLFMT_FORMATTABLE_DEFINED
    ;
#ifdef BSL_FORMATTABLE_DEFINED
    BSLFMT_STREAMED_INSTANCE_DEPRECATED_
    Streamed(const t_STREAMABLE& object)
    requires(bsl::formattable<t_STREAMABLE, char>);
#endif

    // ACCESSORS

    /// Return a non-modifiable reference to the wrapped object.
    const t_STREAMABLE& object() const;
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// Certain versions of clang do not support CTAD with concepts present due to
// CWG issue 2628 resolution not yet applied, therefore we need an explicit
// deduction guide.
template <class t_TYPE>
Streamed(const t_TYPE& object) -> Streamed<t_TYPE>;
#endif
                      // ================================
                      // struct Streamed_WrapperFormatter
                      // ================================

/// This component-private class provides the implementations for parsing
/// stream formatting specifications and for formatting streamed according to
/// that specification.
template <class t_TYPE>
struct Streamed_WrapperFormatter;

template <class t_STREAMABLE>
struct Streamed_WrapperFormatter<Streamed<t_STREAMABLE> > {
  private:
    // DATA
    bsl::formatter<bsl::string_view, char> d_stringFormatter;

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
                                                t_PARSE_CONTEXT& parseContext);

    // ACCESSORS

    /// Format the specified `value` according to the specification stored as a
    /// result of a previous call to the `parse` method, and write the result
    /// to the iterator accessed by calling the `out()` method on the specified
    /// `formatContext` parameter.  Return an end iterator of the output range.
    /// Throw an exception of type `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                            const Streamed<t_STREAMABLE>& value,
                            t_FORMAT_CONTEXT&             formatContext) const;
};

// ============================================================================
//                         FREESTANDING FUNCTIONS
// ============================================================================

#ifdef BSL_FORMATTABLE_DEFINED
template <class t_STREAMABLE>
Streamed<t_STREAMABLE> streamed(const t_STREAMABLE& object)
requires(!bsl::formattable<t_STREAMABLE, char>);

template <class t_STREAMABLE>
BSLFMT_STREAMED_INSTANCE_DEPRECATED_
Streamed<t_STREAMABLE>
streamed(const t_STREAMABLE& object)
requires(bsl::formattable<t_STREAMABLE, char>);
#else
template <class t_STREAMABLE>
Streamed<t_STREAMABLE> streamed(const t_STREAMABLE& object);
#endif

}  // close package namespace
}  // close enterprise namespace

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bslfmt {
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
template <class t_STREAMABLE>
Streamed<t_STREAMABLE>::Streamed(const t_STREAMABLE& object)
#ifdef BSL_FORMATTABLE_DEFINED
requires(!bsl::formattable<t_STREAMABLE, char>)
#endif
: d_object(object)
{
}

#ifdef BSL_FORMATTABLE_DEFINED
template <class t_STREAMABLE>
BSLFMT_STREAMED_INSTANCE_DEPRECATED_
Streamed<t_STREAMABLE>::Streamed(const t_STREAMABLE& object)
requires(bsl::formattable<t_STREAMABLE, char>)
: d_object(object)
{
}
#endif

                         // -------------------------
                         // Streamed_WrapperFormatter
                         // -------------------------

// MANIPULATORS
template <class t_STREAMABLE>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator
Streamed_WrapperFormatter<Streamed<t_STREAMABLE> >::parse(
                                                 t_PARSE_CONTEXT& parseContext)
{
    return d_stringFormatter.parse(parseContext);
}

// ACCESSORS
template <class t_STREAMABLE>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
Streamed_WrapperFormatter<Streamed<t_STREAMABLE> >::format(
                             const Streamed<t_STREAMABLE>& value,
                             t_FORMAT_CONTEXT&             formatContext) const
{
    bsl::ostringstream os;
    os << value.object();

    return d_stringFormatter.format(os.view(), formatContext);
}

}  // close package namespace

// ============================================================================
//                         FREESTANDING FUNCTIONS
// ============================================================================

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

template <class t_STREAMABLE>
struct formatter<BloombergLP::bslfmt::Streamed<t_STREAMABLE>, char>
: BloombergLP::bslfmt::Streamed_WrapperFormatter<
      BloombergLP::bslfmt::Streamed<t_STREAMABLE> > {
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
