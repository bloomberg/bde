// bslfmt_streamed.h                                                  -*-C++-*-

#ifndef INCLUDED_BSLFMT_STREAMED
#define INCLUDED_BSLFMT_STREAMED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a wrapper to format using the ostream insert operator
//
//@CLASSES:
//  bsl::Streamed<t_TYPE>: narrow `char` streaming wrapper class template
//
//@DESCRIPTION: This component provides both a wrapper class template and (for
// use with C++ standards that have no class template argument deduction) a
// wrapper-creator function template to enable `bsl::format`ting values that
// already offer an `ostream` insert `operator<<`.  (On platforms that have a
// working `std::format` the wrapper enables that, too.
//
///The Format String
///-----------------
// The wrapper's formatter supports the same format string syntax as formatting
// a string-like type: alignment, width, and truncation using the "precision"
// value.
//
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
//                       const ThisTypeHasLongAndObscureNameButStreamable& obj)
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
// Next, we format the "value" using `bsl::format` with the wrapper-creator
// function:
//
//```
//  bsl::string s = bsl::format("{}", bslfmt::streamed(obj));
//```
// Finally, we verify the output is correct:
//
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
//                       const ThisTypeHasLongAndObscureNameButStreamable& obj)
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

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterstring.h>
#include <bslfmt_standardformatspecification.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>

#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <limits>     // for 'std::numeric_limits'

#include <streambuf>

namespace BloombergLP {
namespace bslfmt {

                             // ========
                             // Streamed
                             // ========

/// A wrapper class to be used with streamable (`ostream::operator<<`) types
/// that adds `bsl::format` capability to the type (and `std::format` when that
/// is available).
template <class t_STREAMED>
class Streamed {
    // DATA
    const t_STREAMED& d_object;

  public:
    // CREATORS

    /// Created a `Streamed` wrapper instance around the specified `object`.
    /// The behavior is undefined unless the lifetime of `object` is at least
    /// as long as that of the wrapper created.
    Streamed(const t_STREAMED& object);

    // ACCESSORS

    /// Return a non-modifiable reference to the wrapped object.
    const t_STREAMED& object() const;
};

                            // ===================
                            // Streamed_OutIterBuf
                            // ===================

template <class t_OUT_ITER>
class Streamed_OutIterBuf : public std::streambuf {
    t_OUT_ITER d_iter;
    size_t     d_limit;
    size_t     d_counter;

  public:
    // CREATORS
    Streamed_OutIterBuf(t_OUT_ITER iter, size_t limit);

    // MANUPILATORS
    int_type overflow(int_type c) BSLS_KEYWORD_OVERRIDE;

    // ACCESSORS
    t_OUT_ITER outIterator() const;

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

template <class t_STREAMED>
struct Streamed_Formatter<Streamed<t_STREAMED> >
: bsl::formatter<bsl::basic_string<char>, char> {
  private:
    // PRIVATE TYPES
    typedef typename bsl::formatter<bsl::string>::Specification Specification;

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
                              const Streamed<t_STREAMED>& value,
                              t_FORMAT_CONTEXT&           formatContext) const;
};

// ============================================================================
//                         FREESTANDING FUNCTIONS
// ============================================================================

template <class t_STREAMABLE>
Streamed<t_STREAMABLE> streamed(const t_STREAMABLE& object);

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
template <class t_STREAMED>
inline
Streamed<t_STREAMED>::Streamed(const t_STREAMED& object)
: d_object(object)
{
}

// ACCESSORS
template <class t_STREAMED>
inline
const t_STREAMED& Streamed<t_STREAMED>::object() const
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
    if (d_limit && traits_type::eof() != c && d_limit--) {
        *d_iter = traits_type::to_char_type(c);
        ++d_iter;
        ++d_counter;
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
template <class t_STREAMED>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator
Streamed_Formatter<Streamed<t_STREAMED> >::parse(t_PARSE_CONTEXT& parseContext)
{
    return static_cast<bsl::formatter<bsl::string> *>(this)->parse(
                                                                 parseContext);
}

// ACCESSORS
template <class t_STREAMED>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
Streamed_Formatter<Streamed<t_STREAMED> >::format(
                               const Streamed<t_STREAMED>& value,
                               t_FORMAT_CONTEXT&           formatContext) const
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

    const bool leftPadded = d_spec.alignment() ==
                                Specification::e_ALIGN_MIDDLE ||
                            d_spec.alignment() == Specification::e_ALIGN_RIGHT;

    // Anything that is padded on the left needs to know the number of printed
    // characters before it can start "printing", therefore we handle all such
    // formats by first printing the possibly truncated value into a string,
    // then print the string itself via the private base which is the string
    // formatter itself.

    if (leftPadded) {
        bsl::string content;

        Streamed_OutIterBuf<bsl::back_insert_iterator<bsl::string> > buf(
                              bsl::back_insert_iterator<bsl::string>(content),
                              maxStreamedCharacters);
        std::ostream os(&buf);
        os << value.object();

        return static_cast<const bsl::formatter<bsl::string> *>(this)->format(
                                                     content,
                                                     formatContext);  // RETURN
    }

    // Left-padded formats have been handled above, so there is no padding
    // before "printing" the content.

    typename t_FORMAT_CONTEXT::iterator outIterator = formatContext.out();

    // Print the necessary characters from the stream output and calculate the
    // printed length.  (Only need it when there is no left padding present but
    // there is no reason to have an `if`, it'll be the same value.)

    Streamed_OutIterBuf<typename t_FORMAT_CONTEXT::iterator> buf(
                                                        outIterator,
                                                        maxStreamedCharacters);
    std::ostream os(&buf);
    os << value.object();
    outIterator = buf.outIterator();
    size_t contentWidth = buf.counter();

    // Knowing the width of the content "printed" via streaming we can now
    // calculate the necessary trailing padding length.  Since anything
    // left-padded has been handled on a different branch here we may only have
    // the cases when the alignment is left alignment (which is also the
    // default alignment).  Right and middle alignments both may need
    // left-padding so they were handled separately.

    size_t numPaddingChars = finalWidth.category() == NumericValue::e_VALUE
                                 ? finalWidth.value() - contentWidth
                                 : 0;

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

template <class t_STREAMABLE>
bslfmt::Streamed<t_STREAMABLE> bslfmt::streamed(const t_STREAMABLE& object)
{
    return Streamed<t_STREAMABLE>(object);
}

}  // close enterprise namespace

namespace bsl {

template <class t_STREAMED>
struct formatter<BloombergLP::bslfmt::Streamed<t_STREAMED>, char>
: BloombergLP::bslfmt::Streamed_Formatter<
      BloombergLP::bslfmt::Streamed<t_STREAMED> >{};

}  // close namespace bsl

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
