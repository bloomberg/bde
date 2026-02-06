// bslfmt_padutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLFMT_PADUTIL
#define INCLUDED_BSLFMT_PADUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide padding utilities for the `bslfmt` package and clients.
//
//@CLASSES:
//  bslfmt::PadUtil: formatter padding utilities
//
//@DESCRIPTION: `PadUtil` is a `struct` that serves as a namespace containing
// functions suitable for padding output.  It consists of 2 functions,
// `computePadding` which computes the amount of left and right padding, and
// `pad`, which repeatedly outputs a sequence to an iterator.
//
///Usage
///-----
// Suppose we have a string and we want to be able to pad it with any desired
// filler string, and pad it to the left or right, or pad it on both sides, to
// reach a desired total width.
//
// First, we define a couple of typedef's to shorten some bslfmt package types:
// ```
//  typedef bslfmt::FormatterSpecificationNumericValue     NumericValue;
//  typedef bslfmt::FormatSpecificationParserEnums         ParserEnums;
//  typedef bslfmt::PadUtil<char>                          PadUtil;
// ```
// Then, we are now able to define a function `paddedString` which will take a
// a string, `content`, and pad it to a given `fieldWidth` with the specified
// `filler`.  `pad` does not assume that `filler` is a single Unicode code
// point, though that will normally be the case.
// ```
//  void paddedString(bsl::string             *result,
//                    const bsl::string_view&  content,
//                    unsigned                 fieldWidth,
//                    ParserEnums::Alignment   alignment,
//                    const bsl::string_view&  filler)
//  {
//      std::ptrdiff_t leftPadding, rightPadding;
//
//      PadUtil::computePadding(&leftPadding,
//                              &rightPadding,
//                              NumericValue(NumericValue::e_VALUE,
//                                           fieldWidth),
//                              content.length(),
//                              alignment,
//                              ParserEnums::e_ALIGN_LEFT);
//
//      result->resize(filler.length() * (leftPadding + rightPadding) +
//                                                           content.length());
//
//      bsl::string::iterator it = result->begin();
//      it = PadUtil::pad(it, leftPadding,  filler);
//
//      it = bsl::copy(content.begin(), content.end(), it);
//
//      it = PadUtil::pad(it, rightPadding, filler);
//
//      assert(result->end() == it);
//  }
// ```
// Next, we use our new `paddedString` to create a string with "John Brown"
// padded on either side to a total width of 20 bytes, padding with dashes:
// ```
//  bsl::string s;
//  paddedString(&s, "John Brown", 20, ParserEnums::e_ALIGN_MIDDLE, "-");
//  assert("-----John Brown-----" == s);
// ```
// Now, let's pad to right with Unicode `Euro` symbols to a total width of 20
// bytes:
// ```
//  const char *euro = "\xe2\x82\xac";        // Unicode for the Euro symbol.
//  paddedString(&s, "Seven Euros: ", 20, ParserEnums::e_ALIGN_LEFT, euro);
//  assert("Seven Euros: €€€€€€€" == s);
// ```
// Note that because `euro` is not a single byte, the output is longer than
// `fieldWidth`, which was 20:
// ```
//  assert(34 == s.length());
// ```

#include <bslscm_version.h>

#include <bslfmt_formatspecificationparser.h>
#include <bslfmt_formatterspecificationnumericvalue.h>

#include <bslstl_algorithm.h>
#include <bslstl_iterator.h>
#include <bslstl_stringview.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <cstddef>

namespace BloombergLP {
namespace bslfmt {

                                    // =======
                                    // PadUtil
                                    // =======

/// This `struct` serves as a namespace for static functions related to padding
/// output by bslfmt.
template <class t_CHAR>
struct PadUtil {
    BSLMF_ASSERT((bsl::is_same<t_CHAR, char   >::value ||
                  bsl::is_same<t_CHAR, wchar_t>::value));

    // PUBLIC TYPES
    typedef FormatterSpecificationNumericValue NumericValue;
    typedef FormatSpecificationParserEnums     Enums;
    typedef Enums::Alignment                   Alignment;

  private:
    // PRIVATE TYPES
    template <class t_ITERATOR>
    struct IteratorTypeIsOK {
        typedef typename bsl::iterator_traits<t_ITERATOR>::value_type
                                                              IteratorCharType;
        static const bool value =
                              bsl::is_same<IteratorCharType, t_CHAR >::value ||
                              bsl::is_same<IteratorCharType, void   >::value;
    };

    struct CharConverter {
        /// Return the specified `c` cast to type `t_CHAR`.
        template <class t_INPUT_CHAR>
        t_CHAR operator()(t_INPUT_CHAR c) const
        {
            return static_cast<t_CHAR>(c);
        }
    };

    // PRIVATE CLASS METHODS

#ifdef BSLS_ASSERT_IS_USED
    /// Return `true` if the specified `filler` is a valid filler character for
    /// padding, and `false` otherwise.
    static bool fillerIsOK(t_CHAR filler);
    static bool fillerIsOK(const bsl::basic_string_view<t_CHAR>& filler);
    template <class t_INPUT_CHAR>
    static bool fillerIsOK(t_INPUT_CHAR filler);
    template <class t_INPUT_CHAR>
    static bool fillerIsOK(const bsl::basic_string_view<t_INPUT_CHAR>& filler);
#endif

    /// Output the specified `filler` to the specified `out` once and return
    /// the new output iterator.
    template <class t_ITERATOR, class t_INPUT_CHAR>
    static t_ITERATOR writeFiller(
                           t_ITERATOR                                  out,
                           const bsl::basic_string_view<t_INPUT_CHAR>& filler);
    template <class t_ITERATOR, class t_INPUT_CHAR>
    static t_ITERATOR writeFiller(
                           t_ITERATOR                                  out,
                           const t_INPUT_CHAR&                         filler);

    /// Output the specified `filler` to the specified `out` the specified
    /// `width` times and return the new output iterator.
    template <class t_ITERATOR, class t_FILLER>
    static t_ITERATOR padImpl(
                           t_ITERATOR                                  out,
                           std::ptrdiff_t                              width,
                           const t_FILLER&                             filler);

  public:
    // CLASS METHODS

    /// Compute the left and right padding values needed to format content with
    /// the specified `contentWidth` within a field of the width specified by
    /// `widthValue` using the specified `alignment`.  If
    /// `Enums::e_ALIGN_DEFAULT` is passed to `alignment`, use the specified
    /// `defaultAlign` instead.  Load the computed padding values into the
    /// specified `leftPadding` and `rightPadding`.  If the category of
    /// `widthValue` is not `e_VALUE` or if the value indicated by `widthValue`
    /// is less than `contentWidth`, the padding is zero.  The behavior is
    /// undefined if `contentWidth` specifies a negative width, or if
    /// `defaultAlign` specifies `e_ALIGN_DEFAULT`.
    static void computePadding(
                      std::ptrdiff_t      *leftPadding,
                      std::ptrdiff_t      *rightPadding,
                      const NumericValue&  widthValue,
                      std::ptrdiff_t       contentWidth,
                      Alignment            alignment,
                      Alignment            defaultAlign = Enums::e_ALIGN_LEFT);

    /// Write the specified `filler` (possibly a multi-byte unicode sequence)
    /// to the specified `out` the specified `padWidth` times and return the
    /// new output iterator.  If `padWidth < 0`, no output is done.  The
    /// behavior is undefined unless the character type of `filler` is the same
    /// as `t_CHAR` or `filler` is entirely ascii.  Note that the overloads
    /// that input a single character have an advantage over `bsl::fill_n` in
    /// that they will automatically widen or narrow the input character to
    /// `t_CHAR` if necessary.
    template <class t_ITERATOR>
    static t_ITERATOR pad(t_ITERATOR              out,
                          std::ptrdiff_t          padWidth,
                          const bsl::string_view& filler);
    template <class t_ITERATOR>
    static t_ITERATOR pad(t_ITERATOR               out,
                          std::ptrdiff_t           padWidth,
                          const bsl::wstring_view& filler);
    template <class t_ITERATOR>
    static t_ITERATOR pad(t_ITERATOR               out,
                          std::ptrdiff_t           padWidth,
                          char                     filler);
    template <class t_ITERATOR>
    static t_ITERATOR pad(t_ITERATOR               out,
                          std::ptrdiff_t           padWidth,
                          wchar_t                  filler);
};

                                    // -------
                                    // PadUtil
                                    // -------

// PRIVATE CLASS METHODS

#ifdef BSLS_ASSERT_IS_USED
template <class t_CHAR>
inline
bool PadUtil<t_CHAR>::fillerIsOK(t_CHAR)
{
    return true;
}

template <class t_CHAR>
inline
bool PadUtil<t_CHAR>::fillerIsOK(const bsl::basic_string_view<t_CHAR>&)
{
    return true;
}

template <class t_CHAR>
template <class t_INPUT_CHAR>
inline
bool PadUtil<t_CHAR>::fillerIsOK(t_INPUT_CHAR filler)
{
    return 0 == (~0x7f & filler);
}

template <class t_CHAR>
template <class t_INPUT_CHAR>
inline
bool PadUtil<t_CHAR>::fillerIsOK(
                            const bsl::basic_string_view<t_INPUT_CHAR>& filler)
{
    typedef typename bsl::basic_string_view<t_INPUT_CHAR>::const_iterator Iter;
    for (Iter it = filler.begin(); it != filler.end(); ++it) {
        if (!fillerIsOK(*it)) {
            return false;                                             // RETURN
        }
    }

    return true;
}
#endif

template <class t_CHAR>
template <class t_ITERATOR, class t_INPUT_CHAR>
inline
t_ITERATOR PadUtil<t_CHAR>::writeFiller(
                            t_ITERATOR                                  out,
                            const bsl::basic_string_view<t_INPUT_CHAR>& filler)
{
    return bsl::transform(filler.begin(), filler.end(), out, CharConverter());
}

template <class t_CHAR>
template <class t_ITERATOR, class t_INPUT_CHAR>
inline
t_ITERATOR PadUtil<t_CHAR>::writeFiller(
                            t_ITERATOR                                  out,
                            const t_INPUT_CHAR&                         filler)
{
    BSLMF_ASSERT((bsl::is_same<t_INPUT_CHAR, char   >::value ||
                  bsl::is_same<t_INPUT_CHAR, wchar_t>::value));

    *out++ = CharConverter()(filler);

    return out;
}

template <class t_CHAR>
template <class t_ITERATOR, class t_FILLER>
inline
t_ITERATOR PadUtil<t_CHAR>::padImpl(t_ITERATOR      out,
                                    std::ptrdiff_t  width,
                                    const t_FILLER& filler)
{
    BSLMF_ASSERT(IteratorTypeIsOK<t_ITERATOR>::value);
    BSLS_ASSERT(fillerIsOK(filler));

    while (0 < width--) {
        out = writeFiller(out, filler);
    }

    return out;
}

// PUBLIC CLASS METHODS
template <class t_CHAR>
void PadUtil<t_CHAR>::computePadding(std::ptrdiff_t      *leftPadding,
                                     std::ptrdiff_t      *rightPadding,
                                     const NumericValue&  widthValue,
                                     std::ptrdiff_t       contentWidth,
                                     Alignment            alignment,
                                     Alignment            defaultAlign)
{
    BSLS_ASSERT(0 <= contentWidth);
    BSLS_ASSERT(Enums::e_ALIGN_DEFAULT != defaultAlign);

    if (NumericValue::e_VALUE != widthValue.category()) {
        *leftPadding  = 0;
        *rightPadding = 0;
        return;                                                       // RETURN
    }
    const std::ptrdiff_t width = widthValue.value();

    if (width <= contentWidth) {
        *leftPadding  = 0;
        *rightPadding = 0;
        return;                                                       // RETURN
    }
    const std::ptrdiff_t totalPadding = width - contentWidth;

    if (Enums::e_ALIGN_DEFAULT == alignment) {
        alignment = defaultAlign;
    }

    switch (alignment) {
      case Enums::e_ALIGN_LEFT: {
        *leftPadding  = 0;
        *rightPadding = totalPadding;
      } break;
      case Enums::e_ALIGN_RIGHT: {
        *leftPadding  = totalPadding;
        *rightPadding = 0;
      } break;
      case Enums::e_ALIGN_MIDDLE: {
        *leftPadding  = totalPadding / 2;
        *rightPadding = totalPadding - *leftPadding;
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("invalid alignment");
      }
    }
}

template <class t_CHAR>
template <class t_ITERATOR>
t_ITERATOR PadUtil<t_CHAR>::pad(t_ITERATOR              out,
                                std::ptrdiff_t          padWidth,
                                const bsl::string_view& filler)
{
    return padImpl(out, padWidth, filler);
}

template <class t_CHAR>
template <class t_ITERATOR>
t_ITERATOR PadUtil<t_CHAR>::pad(t_ITERATOR               out,
                                std::ptrdiff_t           padWidth,
                                const bsl::wstring_view& filler)
{
    return padImpl(out, padWidth, filler);
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR PadUtil<t_CHAR>::pad(t_ITERATOR out,
                                std::ptrdiff_t padWidth,
                                char filler)
{
    return padImpl(out, padWidth, filler);
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR PadUtil<t_CHAR>::pad(t_ITERATOR     out,
                                std::ptrdiff_t padWidth,
                                wchar_t filler)
{
    return padImpl(out, padWidth, filler);
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
