// bslfmt_formatterpair.h                                             -*-C++-*-
#ifndef INCLUDED_BSLFMT_FORMATTERPAIR
#define INCLUDED_BSLFMT_FORMATTERPAIR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter customization for `bsl::pair`.
//
//@CLASSES:
//  bsl::formatter<bsl::pair<U,V>, t_CHAR>: formatter specialization for `pair`
//
//@CANONICAL_HEADER: bsl_format.h
//
//@SEE_ALSO: bslfmt_tupleformatspecification, bslfmt_formatterrange
//
//@DESCRIPTION: This component provides a partial specialization of
// `bsl::formatter` for `bsl::pair`, implementing the tuple formatting of
// [format.tuple].  A pair is rendered as `(v1, v2)` by default; the `n` format
// type drops the parentheses and the `m` format type additionally replaces the
// separator with `: `, giving `v1: v2`, which is what makes a `bsl::map` print
// as `{1: "a"}`.
//
// The two values are always rendered in the escaped ("debug") presentation: a
// pair of strings appears as `("a", "b")`.
//
// This header is not intended to be included directly.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Formatting A Pair
/// - - - - - - - - - - - - - -
// Suppose we wish to render a key/value pair as it would appear inside a map.
//
// First, we parse the map-like presentation, `m`, into a formatter for the
// pair:
// ```
//  bslfmt::MockParseContext<char> parseContext("m}", 0);
//
//  bsl::formatter<bsl::pair<int, bsl::string>, char> formatter;
//  formatter.parse(parseContext);
// ```
// Now, we format a pair with it:
// ```
//  bsl::pair<int, bsl::string> value(1, "one");
//
//  bslfmt::MockFormatContext<char> formatContext(0);
//  formatContext.advance_to(
//                bsl::as_const(formatter).format(value, formatContext));
// ```
// Finally, we observe that the parentheses are gone, that the two values are
// separated by `: `, and that the string is quoted:
// ```
//  assert("1: \"one\"" == formatContext.finalString());
// ```

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

#include <bslfmt_bufferformatcontext.h>
#include <bslfmt_debugformatutil.h>
#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterspecificationnumericvalue.h>
#include <bslfmt_padutil.h>
#include <bslfmt_separators.h>
#include <bslfmt_tupleformatspecification.h>

#include <bsla_fallthrough.h>

#include <bslmf_removecvref.h>

#include <bslstl_algorithm.h>
#include <bslstl_pair.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>

namespace bsl {

                            // ==========================
                            // class formatter<bsl::pair>
                            // ==========================

/// Partial specialization of the `bsl::formatter` template for `bsl::pair`.
template <class t_TYPE1, class t_TYPE2, class t_CHAR>
class formatter<bsl::pair<t_TYPE1, t_TYPE2>, t_CHAR> {
    // PRIVATE TYPES
    typedef BloombergLP::bslfmt::TupleFormatSpecification<t_CHAR>
                                                                 Specification;

    // DATA
    formatter<typename bsl::remove_cvref<t_TYPE1>::type, t_CHAR> d_underlying1;
    formatter<typename bsl::remove_cvref<t_TYPE2>::type, t_CHAR> d_underlying2;

    basic_string_view<t_CHAR> d_separator;
    basic_string_view<t_CHAR> d_openingBracket;
    basic_string_view<t_CHAR> d_closingBracket;
    Specification             d_spec;

    // PRIVATE ACCESSORS

    /// Write the "pair body" into the specified format context `ctx`, without
    /// applying any fill / align / width padding.  Return the iterator
    /// obtained after the write.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator formatBody(
                                 const bsl::pair<t_TYPE1, t_TYPE2>& pair,
                                 t_FORMAT_CONTEXT&                  ctx) const;

  public:
    // CREATORS

    /// Create a formatter having `(` and `)` as its brackets and `, ` as its
    /// separator.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    formatter();

    // MANIPULATORS

    /// Set the separator written between the two values to the specified
    /// `separator`.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    void set_separator(basic_string_view<t_CHAR> separator)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Set the brackets written before the first and after the second value
    /// to the specified `openingBracket` and `closingBracket` respectively.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    void set_brackets(basic_string_view<t_CHAR> openingBracket,
                      basic_string_view<t_CHAR> closingBracket)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Parse the tuple format specification held by the specified `ctx`.
    /// Return an iterator referring to the closing `}` of the specification.
    /// Throw `bsl::format_error` if the specification is invalid.  Note that
    /// the `n` format type sets empty brackets, and the `m` format type
    /// additionally sets `: ` as the separator.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20
    typename t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& ctx);

    // ACCESSORS

    /// Write the specified `pair` to the output of the specified `ctx` as
    /// specified by the most recent call to `parse`, rendering each of its
    /// values in the escaped ("debug") presentation.  Return an iterator
    /// referring to the end of the output.  The behavior is undefined unless
    /// `parse` has been called on this object.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                 const bsl::pair<t_TYPE1, t_TYPE2>& pair,
                                 t_FORMAT_CONTEXT&                  ctx) const;
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                            // --------------------------
                            // class formatter<bsl::pair>
                            // --------------------------

// PRIVATE ACCESSORS
template <class t_TYPE1, class t_TYPE2, class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<bsl::pair<t_TYPE1, t_TYPE2>, t_CHAR>::formatBody(
                                  const bsl::pair<t_TYPE1, t_TYPE2>& pair,
                                  t_FORMAT_CONTEXT&                  ctx) const
{
    typename t_FORMAT_CONTEXT::iterator out = ctx.out();
    out = bsl::copy(d_openingBracket.begin(), d_openingBracket.end(), out);
    ctx.advance_to(out);
    out = d_underlying1.format(pair.first, ctx);
    out = bsl::copy(d_separator.begin(), d_separator.end(), out);
    ctx.advance_to(out);
    out = d_underlying2.format(pair.second, ctx);
    return bsl::copy(d_closingBracket.begin(), d_closingBracket.end(), out);
}

// CREATORS
template <class t_TYPE1, class t_TYPE2, class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
formatter<bsl::pair<t_TYPE1, t_TYPE2>, t_CHAR>::formatter()
: d_separator(BloombergLP::bslfmt::Separators<t_CHAR>::commaSeparator())
, d_openingBracket(BloombergLP::bslfmt::Separators<t_CHAR>::openingParen())
, d_closingBracket(BloombergLP::bslfmt::Separators<t_CHAR>::closingParen())
{
}

// MANIPULATORS
template <class t_TYPE1, class t_TYPE2, class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR_CPP20
void formatter<bsl::pair<t_TYPE1, t_TYPE2>, t_CHAR>::
set_separator(basic_string_view<t_CHAR> separator) BSLS_KEYWORD_NOEXCEPT
{
    d_separator = separator;
}

template <class t_TYPE1, class t_TYPE2, class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR_CPP20
void formatter<bsl::pair<t_TYPE1, t_TYPE2>, t_CHAR>::
set_brackets(basic_string_view<t_CHAR> openingBracket,
             basic_string_view<t_CHAR> closingBracket) BSLS_KEYWORD_NOEXCEPT
{
    d_openingBracket = openingBracket;
    d_closingBracket = closingBracket;
}

template <class t_TYPE1, class t_TYPE2, class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename t_PARSE_CONTEXT::iterator
formatter<bsl::pair<t_TYPE1, t_TYPE2>, t_CHAR>::parse(t_PARSE_CONTEXT& ctx)
{
    d_spec.parse(&ctx);
    switch (d_spec.formatType()) {
      case Specification::e_TUPLE_PAIR_MAP:
        this->set_separator(
                    BloombergLP::bslfmt::Separators<t_CHAR>::colonSeparator());
        BSLA_FALLTHROUGH;
      case Specification::e_TUPLE_NO_BRACKETS: {
        basic_string_view<t_CHAR> empty;
        this->set_brackets(empty, empty);
      } break;
      default: {
      } break;
    }
    if (ctx.begin() == ctx.end() || *ctx.begin() != t_CHAR('}')) {
        BSLS_THROW(bsl::format_error("Unexpected format string"));
    }
    d_underlying1.parse(ctx);
    d_underlying2.parse(ctx);

    // [format.tuple] renders elements in the escaped ("debug") presentation
    // unconditionally -- there is no per-element sub-specification in a
    // tuple-format-spec to suppress it, as there is for ranges.

    BloombergLP::bslfmt::DebugFormatUtil::setDebugFormat(&d_underlying1);
    BloombergLP::bslfmt::DebugFormatUtil::setDebugFormat(&d_underlying2);

    return ctx.begin();
}

// ACCESSORS
template <class t_TYPE1, class t_TYPE2, class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<bsl::pair<t_TYPE1, t_TYPE2>, t_CHAR>::format(
                                  const bsl::pair<t_TYPE1, t_TYPE2>& pair,
                                  t_FORMAT_CONTEXT&                  ctx) const
{
    typedef BloombergLP::bslfmt::FormatterSpecificationNumericValue
                                                                  NumericValue;

    Specification finalSpec(d_spec);
    finalSpec.postprocess(ctx);

    const NumericValue finalWidth = finalSpec.postprocessedWidth();

    // Fast path: no outer width -- no padding needed.
    if (finalWidth.category() == NumericValue::e_DEFAULT) {
        return this->formatBody(pair, ctx);                           // RETURN
    }

    // Slow path: render the pair body into a temporary string, then emit it
    // with fill / align / width padding.
    typedef BloombergLP::bslfmt::BufferFormatContext<t_CHAR, t_FORMAT_CONTEXT>
                                                                 BufferContext;

    bsl::basic_string<t_CHAR> buffer;
    BufferContext             nested(&buffer, &ctx);

    this->formatBody(pair, nested);

    const basic_string_view<t_CHAR> filler(finalSpec.filler(),
                                           finalSpec.numFillerCharacters());

    typedef BloombergLP::bslfmt::PadUtil<t_CHAR> PadUtil;

    return PadUtil::writePadded(ctx.out(),
                                buffer,
                                finalWidth,
                                finalSpec.alignment(),
                                filler,
                                Specification::e_ALIGN_LEFT);
}

}  // close namespace bsl

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

#endif

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
