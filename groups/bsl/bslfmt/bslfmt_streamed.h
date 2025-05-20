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
///Example 1: Formatting a Streamable Object Using the Function
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<`.  When writing portable code that
// should work on compilers that do not support class template argument
// deduction we would use the wrapper-creator function `bslfmt::streamed` to
// avoid having to know and write the type of the object.
//
// First, for the sake of demonstration we create a type with an obscure and
// long name that we neither want to remember nor ever to write down, and which
// can be streamed out:
//
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
//
// Then, we create an object of said type that we want to print out:
//
//```
//  const ThisTypeHasLongAndObscureNameButStreamable obj;
//```
//
// Next, we format the "value" using `bsl::format` with the wrapper-creator
// function:
//```
//  bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
//```
//
// Finally, we verify the output is correct:
//```
//  assert(s == "The printout");
//```
//
///Example 2: Formatting with CTAD support
///- - - - - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<` and we target only modern compilers.
// In such case the wrapper class template can be used directly, without the
// need for the function.
//
// First, for the sake of demonstration we create a type with an obscure and
// long name that we neither want to remember nor ever to write down, and which
// can be streamed out:
//
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
//
// Then, we create an object of said type that we want to print out:
//
//```
//  const ThisTypeHasLongAndObscureNameButStreamable obj;
//```
// Next, we format the "value" using `bsl::format` with the wrapper class
// template, class template argument deduction takes care of the type:
//
//```
//  bsl::string s = bsl::format("{}", bslfmt::Streamed(obj));
//```
// Finally, we verify the output is correct:
//
//```
//  assert(s == "The printout");
//```
//
///Example 3: Format String Options
/// - - - - - - - - - - - - - - - -
// Suppose we want to format an object that already supports streaming into an
// `ostream` using the insert `operator<<` for an environment that requires the
// output to be padded, aligned, and/or truncation of the output.  In this
// example we will introduce the effects of the various possible format string
// specifications.
//
// First, for the sake of demonstration we create a type that prints a series
// of digits to help demonstrate the effects of the various formattings:
//
//```
//  class Streamable {
//  };
//
//  std::ostream& operator<<(std::ostream& os, const Streamable&)
//  {
//      return os << "12345678";
//  }
//```
//
// Then, we create an object of said type that we will format:
//
//```
//  const Streamable obj;
//```
//
// Next, we format the "value" using many different format strings, starting
// with the default for completeness:
//```
//  bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
//  assert(s == "12345678");
//```
//
// Then, we format with specifying just a width:
//```
//  s = bsl::format("{:10}", bslfmt::streamed(obj));
//  assert(s == "12345678  ");
//```
//
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
//
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
#include <bslfmt_standardformatspecification.h>

#include <bsla_maybeunused.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>

#include <bsls_compilerfeatures.h>
#include <bsls_deprecatefeature.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

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

                             // ========
                             // Streamed
                             // ========

/// A wrapper class to be used with streamable (`ostream::operator<<`) types
/// that adds `bsl::format` capability to the type (and `std::format` when that
/// is available).
template <class t_STREAMABLE>
class Streamed_Imp {
  public:
    // TYPES
    typedef t_STREAMABLE object_type;

  private:
    // DATA
    const t_STREAMABLE& d_object;

  public:
    // CREATORS

    /// Create a wrapper instance around the specified `object`.  The behavior
    /// is undefined unless the lifetime of `object` is at least as long as
    /// that of the wrapper created.
    Streamed_Imp(const t_STREAMABLE& object);

    // ACCESSORS

    /// Return a non-modifiable reference to the wrapped object.
    const t_STREAMABLE& object() const;
};

template <class t_STREAMABLE>
struct Streamed : Streamed_Imp<t_STREAMABLE> {
    // CREATORS

    /// Create a `Streamed` wrapper instance around the specified `object`.
    /// The behavior is undefined unless the lifetime of `object` is at least
    /// as long as that of the wrapper created.
    template <class t_TYPE>
    Streamed(const t_TYPE& object)
#ifdef BSLFMT_FORMATTABLE_DEFINED
    requires(!bsl::formattable<t_TYPE, char> &&
             std::is_same_v<t_TYPE, t_STREAMABLE>)
#endif  // BSLFMT_FORMATTABLE_DEFINED
    : Streamed_Imp<t_STREAMABLE>(object)
    {
    }
#ifdef BSLFMT_FORMATTABLE_DEFINED
    template <class t_TYPE>
    requires(bsl::formattable<t_TYPE, char> &&
             std::is_same_v<t_TYPE, t_STREAMABLE>)
    BSLFMT_STREAMED_INSTANCE_DEPRECATED_
    Streamed(const t_TYPE& object)
    : Streamed_Imp<t_STREAMABLE>(object)
    {
    }
#endif  // BSLFMT_FORMATTABLE_DEFINED
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
template <class t_TYPE>
Streamed(const t_TYPE& object) -> Streamed<t_TYPE>;
#endif

                            // ===================
                            // Streamed_OutIterBuf
                            // ===================

/// `Streamed_OutIterBuf` provides a standard stream buffer for the specified
/// `t_OUT_ITER`.  The behavior is undefined unless `t_OUT_ITER` is an output
/// iterator (new concept, not the `LegacyOutputIterator`).  This stream buffer
/// supports not only writing the output to a specified output iterator, but
/// also to limit said output to a certain number of characters (see
/// constructor), and to count the number of characters actually written.  The
/// current value of the output iterator and the counter is accessible to the
/// user.
template <class t_OUT_ITER>
class Streamed_OutIterBuf : public std::streambuf {
    t_OUT_ITER d_iter;
    size_t     d_limit;
    size_t     d_counter;

  public:
    // CREATORS

    /// Create a `Streamed_OutIterBuf` object that writes into the specified
    /// output iterator `iter`, and limits its output to the specified `limit`
    /// number of characters.  Note that to have no limit the maximum value of
    /// `size_t` needs to be passed as the `limit` argument.  Also note that we
    /// do not concern ourselves with the possible difference between the size
    /// of `size_t` and `streamsize_t` (on 32-bit platforms) as this buffer is
    /// only intended to write into string-like output, not files.
    Streamed_OutIterBuf(t_OUT_ITER iter, size_t limit);

    // MANUPILATORS

    /// If the current limit value is not zero write the specified character
    /// `c` to the output iterator, increment the output iterator, decrement
    /// limit, and return an unspecified non-EOF value.  If the current limit
    /// is zero do nothing and return `traits_type::eof()` value.
    int_type overflow(int_type c) BSLS_KEYWORD_OVERRIDE;

    // ACCESSORS

    /// Return the current value of the output iterator.
    t_OUT_ITER outIterator() const;

    /// Return the current value of the outputted character counter.
    size_t counter() const;
};

                         // =========================
                         // struct Streamed_Formatter
                         // =========================

/// This component-private class provides the implementations for parsing
/// stream formatting specifications and for formatting streamed according to
/// that specification.
template <class t_TYPE>
struct Streamed_Formatter;

template <class t_STREAMABLE>
struct Streamed_Formatter<Streamed_Imp<t_STREAMABLE> > {
  private:
    // PRIVATE TYPES
    typedef StandardFormatSpecification<char> Specification;

    // DATA
    Specification d_spec;  // parsed specification.

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
                        const Streamed_Imp<t_STREAMABLE>& value,
                        t_FORMAT_CONTEXT&                 formatContext) const;
};

// ============================================================================
//                         FREESTANDING FUNCTIONS
// ============================================================================

#ifdef BSLFMT_FORMATTABLE_DEFINED
template <class t_STREAMABLE>
requires(!bsl::formattable<t_STREAMABLE, char>)
Streamed_Imp<t_STREAMABLE> streamed(const t_STREAMABLE& object);

template <class t_STREAMABLE>
requires(bsl::formattable<t_STREAMABLE, char>)
BSLFMT_STREAMED_INSTANCE_DEPRECATED_
Streamed_Imp<t_STREAMABLE>
streamed(const t_STREAMABLE& object);
#else
template <class t_STREAMABLE>
Streamed_Imp<t_STREAMABLE> streamed(const t_STREAMABLE& object);
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

// CREATORS
template <class t_STREAMABLE>
inline
Streamed_Imp<t_STREAMABLE>::Streamed_Imp(const t_STREAMABLE& object)
: d_object(object)
{
}

// ACCESSORS
template <class t_STREAMABLE>
inline
const t_STREAMABLE& Streamed_Imp<t_STREAMABLE>::object() const
{
    return d_object;
}

                            // -------------------
                            // Streamed_OutIterBuf
                            // -------------------

// CREATORS
template <class t_OUT_ITER>
inline
Streamed_OutIterBuf<t_OUT_ITER>::Streamed_OutIterBuf(t_OUT_ITER iter,
                                                     size_t     limit)
: d_iter(iter)
, d_limit(limit)
, d_counter(0)
{
}

// MANUPILATORS
template <class t_OUT_ITER>
inline
typename Streamed_OutIterBuf<t_OUT_ITER>::int_type
Streamed_OutIterBuf<t_OUT_ITER>::overflow(int_type c)
{
    if (0 == d_limit) {
        return traits_type::eof();                                    // RETURN
    }

    if (traits_type::eof() != c) {
        *d_iter = traits_type::to_char_type(c);
        ++d_iter;
        ++d_counter;
        --d_limit;
    }
    return traits_type::not_eof(c);
}

// ACCESSORS
template <class t_OUT_ITER>
inline
t_OUT_ITER Streamed_OutIterBuf<t_OUT_ITER>::outIterator() const
{
    return d_iter;
}

template <class t_OUT_ITER>
inline
size_t Streamed_OutIterBuf<t_OUT_ITER>::counter() const
{
    return d_counter;
}

                             // ------------------
                             // Streamed_Formatter
                             // ------------------

// MANIPULATORS
template <class t_STREAMABLE>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator
Streamed_Formatter<Streamed_Imp<t_STREAMABLE> >::parse(
                                                 t_PARSE_CONTEXT& parseContext)
{
    d_spec.parse(&parseContext, Specification::e_CATEGORY_STRING);

    if (d_spec.sign() != Specification::e_SIGN_DEFAULT)
        BSLS_THROW(bsl::format_error(
                    "Formatting sign specifier not valid for streamed types"));

    if (d_spec.alternativeFlag())
        BSLS_THROW(bsl::format_error(
                       "Formatting # specifier not valid for streamed types"));

    if (d_spec.zeroPaddingFlag())
        BSLS_THROW(bsl::format_error(
                       "Formatting 0 specifier not valid for streamed types"));

    if (d_spec.localeSpecificFlag())
        BSLS_THROW(bsl::format_error("Formatting L specifier not supported"));

    if (d_spec.formatType() == Specification::e_STRING_ESCAPED) {
        BSLS_THROW(bsl::format_error("Unsupported format type '?'"));
    }

    return parseContext.begin();
}

// ACCESSORS
template <class t_STREAMABLE>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
Streamed_Formatter<Streamed_Imp<t_STREAMABLE> >::format(
                         const Streamed_Imp<t_STREAMABLE>& value,
                         t_FORMAT_CONTEXT&                 formatContext) const
{
    Specification finalSpec(d_spec);

    finalSpec.postprocess(formatContext);

    typedef FormatterSpecificationNumericValue NumericValue;

    NumericValue finalWidth(finalSpec.postprocessedWidth());

    NumericValue finalPrecision(finalSpec.postprocessedPrecision());

    // Retrieve the limit on the number of characters printed from the stream

    size_t maxStreamedCharacters = 0;
    switch (finalPrecision.category()) {
      case NumericValue::e_DEFAULT: {
        maxStreamedCharacters = std::numeric_limits<size_t>::max();
      } break;
      case NumericValue::e_VALUE: {
        maxStreamedCharacters = finalPrecision.value();
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Invalid precision specifier"));  // THROW
      }
    }

    const bool thereMayBePadding = finalWidth.category() ==
                                   NumericValue::e_VALUE;

    const bool leftPadded =
                        thereMayBePadding &&
                        (d_spec.alignment() == Specification::e_ALIGN_MIDDLE ||
                         d_spec.alignment() == Specification::e_ALIGN_RIGHT);

    // Anything that is padded on the left needs to know the number of printed
    // characters before it can start "printing", therefore we handle all such
    // formats by first printing the possibly truncated value into a string,
    // then print the string itself via the private base which is the string
    // formatter itself.

    typename t_FORMAT_CONTEXT::iterator outIterator = formatContext.out();

    bsl::string content;
    size_t      contentWidth;
    if (leftPadded) {
        Streamed_OutIterBuf<bsl::back_insert_iterator<bsl::string> > strBuf(
                              bsl::back_insert_iterator<bsl::string>(content),
                              maxStreamedCharacters);
        std::ostream strOs(&strBuf);
        strOs << value.object();
        contentWidth = content.length();

        size_t allPadding =
              finalWidth.category() == NumericValue::e_VALUE
            ? bsl::max<size_t>(finalWidth.value(), contentWidth) - contentWidth
            : 0;

        size_t numPaddingChars = 0;
        switch (d_spec.alignment()) {
          case Specification::e_ALIGN_MIDDLE: {
            numPaddingChars = (allPadding / 2);
          } break;
          case Specification::e_ALIGN_RIGHT: {
            numPaddingChars = allPadding;
          } break;
          default: {
            ;
          } break;
        }

        for (size_t i = 0; i < numPaddingChars; ++i) {
            // Note that, per the C++ spec, the fill character is always
            // assumed to have a field width of one, regardless of its actual
            // field width.
            outIterator = bsl::copy(
                          finalSpec.filler(),
                          finalSpec.filler() + finalSpec.numFillerCharacters(),
                          outIterator);
        }

        Streamed_OutIterBuf<typename t_FORMAT_CONTEXT::iterator> buf(
                                                        outIterator,
                                                        maxStreamedCharacters);
        std::ostream os(&buf);
        os << content;
        outIterator = buf.outIterator();
    }
    else {
        // Print the necessary characters from the stream output and calculate
        // the printed length.  (Only need it when there is no left padding
        // present but there is no reason to have an `if`, it'll be the same
        // value.)
        Streamed_OutIterBuf<typename t_FORMAT_CONTEXT::iterator> buf(
                                                        outIterator,
                                                        maxStreamedCharacters);
        std::ostream os(&buf);
        os << value.object();
        outIterator  = buf.outIterator();
        contentWidth = buf.counter();
    }

    // At this point left padding (if present) and the content itself had been
    // output, `contentWidth` is set, and `outIterator` points where the right
    // padding (if present) has to be placed.

    size_t allPadding =
              finalWidth.category() == NumericValue::e_VALUE
            ? bsl::max<size_t>(finalWidth.value(), contentWidth) - contentWidth
            : 0;

    size_t numPaddingChars = 0;
    switch (d_spec.alignment()) {
      case Specification::e_ALIGN_DEFAULT:
      case Specification::e_ALIGN_LEFT: {
        numPaddingChars = allPadding;
      } break;
      case Specification::e_ALIGN_MIDDLE: {
        numPaddingChars = (allPadding + 1) / 2;
      } break;
      default: {
        ;
      } break;
    }

    for (size_t i = 0; i < numPaddingChars; ++i) {
        // Note that, per the C++ spec, the fill character is always assumed to
        // have a field width of one, regardless of its actual field width.
        outIterator = bsl::copy(
                          finalSpec.filler(),
                          finalSpec.filler() + finalSpec.numFillerCharacters(),
                          outIterator);
    }

    return outIterator;
}

}  // close package namespace

// ============================================================================
//                         FREESTANDING FUNCTIONS
// ============================================================================

#ifdef BSLFMT_FORMATTABLE_DEFINED
template <class t_STREAMABLE>
requires(!bsl::formattable<t_STREAMABLE, char>)
bslfmt::Streamed_Imp<t_STREAMABLE> bslfmt::streamed(const t_STREAMABLE& object)
{
    return Streamed_Imp<t_STREAMABLE>(object);
}

template <class t_STREAMABLE>
requires(bsl::formattable<t_STREAMABLE, char>)
BSLFMT_STREAMED_INSTANCE_DEPRECATED_
bslfmt::Streamed_Imp<t_STREAMABLE>
bslfmt::streamed(const t_STREAMABLE& object)
{
    return Streamed_Imp<t_STREAMABLE>(object);
}
#else  // BSLFMT_FORMATTABLE_DEFINED
template <class t_STREAMABLE>
bslfmt::Streamed_Imp<t_STREAMABLE> bslfmt::streamed(const t_STREAMABLE& object)
{
    return Streamed_Imp<t_STREAMABLE>(object);
}
#endif  // BSLFMT_FORMATTABLE_DEFINED
}  // close enterprise namespace

namespace bsl {

template <class t_STREAMABLE>
struct formatter<BloombergLP::bslfmt::Streamed_Imp<t_STREAMABLE>, char>
: BloombergLP::bslfmt::Streamed_Formatter<
      BloombergLP::bslfmt::Streamed_Imp<t_STREAMABLE> > {
};
template <class t_STREAMABLE>
struct formatter<BloombergLP::bslfmt::Streamed<t_STREAMABLE>, char>
: BloombergLP::bslfmt::Streamed_Formatter<
      BloombergLP::bslfmt::Streamed_Imp<t_STREAMABLE> > {
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
