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
//@DESCRIPTION: This component provides a
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: TBD
/// - - - - - - - - - - - - - - - - -
// Suppose we want to TBD
//
// ```
//  TBD
// ```
//

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_standardformatspecification.h>

#include <bslalg_numericformatterutil.h>

#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <limits>     // for 'std::numeric_limits'

#include <streambuf>

namespace BloombergLP {
namespace bslfmt {


template <class t_STREAMED>
class Streamed {
    // DATA
    const t_STREAMED& d_object;

  public:
    // CREATORS
    Streamed(const t_STREAMED& object)
    : d_object(object)
    {
    }

    // ACCESSORS
    const t_STREAMED& object() const { return d_object; }
};


class Streamed_CountingBuf : public std::streambuf {
    // DATA
    size_t d_counter;
    size_t d_limit;

  public:
    // CREATORS
    Streamed_CountingBuf(size_t limit)
    : d_counter(0)
    , d_limit(limit)
    {
    }

    // MANIPULATORS
    int_type overflow(int_type c) BSLS_KEYWORD_OVERRIDE
    {
        if (d_limit && traits_type::eof() != c && d_limit--) {
            ++d_counter;
        }
        return traits_type::not_eof(c);
    }

    // ACCESSORS
    size_t counter() const { return d_counter; }
};

template <class t_OUT_ITER>
class Streamed_OutIterBuf : public std::streambuf {
    t_OUT_ITER d_iter;
    size_t     d_limit;
    size_t     d_counter;

  public:
    // CREATORS
    Streamed_OutIterBuf(t_OUT_ITER iter, size_t limit)
    : d_iter(iter)
    , d_limit(limit)
    , d_counter(0)
    {
    }

    // MANUPILATORS
    int_type overflow(int_type c) override
    {
        if (d_limit && traits_type::eof() != c && d_limit--) {
            *d_iter = traits_type::to_char_type(c);
            ++d_iter;
            ++d_counter;
        }
        return traits_type::not_eof(c);
    }

    // ACCESSORS
    t_OUT_ITER outIterator() const { return d_iter; }

    size_t counter() const { return d_counter; }
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
struct Streamed_Formatter<Streamed<t_STREAMED> > {
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
                              const Streamed<t_STREAMED>& value,
                              t_FORMAT_CONTEXT&           formatContext) const;
};

// ============================================================================
//                         FREESTANDING FUNCTIONS
// ============================================================================

template <class t_STREAMABLE>
Streamed<t_STREAMABLE> streamed(const t_STREAMABLE& object)
{
    return Streamed<t_STREAMABLE>(object);
}

}  // close package namespace
}  // close enterprise namespace

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bslfmt {

// MANIPULATORS
template <class t_STREAMED>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator
Streamed_Formatter<Streamed<t_STREAMED> >::parse(t_PARSE_CONTEXT& parseContext)
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

    return parseContext.begin();
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

    // If necessary, count the number of the characters printed to the stream,
    // up to the maximum of `maxStreamedCharacters` determined above.  Notice
    // that this pre-calculation is necessary only when we have to start with
    // padding characters due to alignment.

    size_t streamedWidth = 0;
    bool   leftPadded    = false;

    // Note that, per the C++ spec, the fill character is always assumed to
    // have a field width of one, regardless of its actual field width.
    switch (d_spec.alignment()) {
      case Specification::e_ALIGN_MIDDLE:
      case Specification::e_ALIGN_RIGHT: {
        Streamed_CountingBuf buf(maxStreamedCharacters);
        std::ostream         os(&buf);
        os << value.object();
        streamedWidth = buf.counter();
        leftPadded    = true;
      } break;
      default: {
        ;
      } break;
    }

    size_t fullPaddingLength = 0;

    switch (finalWidth.category()) {
      case NumericValue::e_DEFAULT: {
        fullPaddingLength = 0;
      } break;
      case NumericValue::e_VALUE: {
        fullPaddingLength = finalWidth.value() - streamedWidth;
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Invalid precision specifier"));
      }
    }

    // Calculate the number of padding characters on the left; the padding that
    // we begin "printing" with.

    int numPaddingChars = 0;

    // Note that, per the C++ spec, the fill character is always assumed to
    // have a field width of one, regardless of its actual field width.
    switch (d_spec.alignment()) {
      case Specification::e_ALIGN_MIDDLE: {
        numPaddingChars = (fullPaddingLength / 2);
      } break;
      case Specification::e_ALIGN_RIGHT: {
        numPaddingChars = fullPaddingLength;
      } break;
      default: {
      } break;
    }

    typename t_FORMAT_CONTEXT::iterator outIterator = formatContext.out();

    if (leftPadded) {
        for (int i = 0; i < numPaddingChars; ++i) {
            outIterator = bsl::copy(
                          finalSpec.filler(),
                          finalSpec.filler() + finalSpec.numFillerCharacters(),
                          outIterator);
        }
    }

    // Print the necessary characters from the stream output and calculate the
    // printed length.  (Only need it when there is no left padding present but
    // there is no reason to have an `if`, it'll be the same value.)

    Streamed_OutIterBuf<typename t_FORMAT_CONTEXT::iterator> buf(
                                                        outIterator,
                                                        maxStreamedCharacters);
    std::ostream os(&buf);
    os << value.object();
    outIterator = buf.outIterator();
    streamedWidth = buf.counter();

    if (!leftPadded) {
        switch (finalWidth.category()) {
          case NumericValue::e_VALUE: {
            fullPaddingLength = finalWidth.value() - streamedWidth;
          } break;
          default: {
            ;
          } break;
        }
    }

    numPaddingChars = 0;

    // Note that, per the C++ spec, the fill character is always assumed to
    // have a field width of one, regardless of its actual field width.
    switch (d_spec.alignment()) {
      case Specification::e_ALIGN_DEFAULT:
      case Specification::e_ALIGN_LEFT: {
        numPaddingChars = fullPaddingLength;
      } break;
      case Specification::e_ALIGN_MIDDLE: {
        numPaddingChars = ((fullPaddingLength + 1) / 2);
      } break;
      default: {
        ;
      } break;
    }

    for (int i = 0; i < numPaddingChars; ++i) {
        outIterator = bsl::copy(
                          finalSpec.filler(),
                          finalSpec.filler() + finalSpec.numFillerCharacters(),
                          outIterator);
    }

    return outIterator;
}

}  // close package namespace
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
