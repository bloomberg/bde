// bslfmt_rangeformatter.h                                            -*-C++-*-
#ifndef INCLUDED_BSLFMT_RANGEFORMATTER
#define INCLUDED_BSLFMT_RANGEFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide helper templates for implementing range formatters.
//
//@CLASSES:
//  bslfmt::range_formatter: base for range `formatter` specializations
//  bslfmt::SetLikeFormatter: base for the set-like format `{v1, v2}`
//  bslfmt::MapLikeFormatter: base for the map-like format `{k1: v1, k2: v2}`
//
//@CANONICAL_HEADER: bsl_format.h
//
//@SEE_ALSO: bslfmt_formatterrange, bslfmt_rangeformatspecification
//
//@DESCRIPTION: This component provides three class templates from which the
// `bsl::formatter` specializations for range types are derived.  They differ
// in their default brackets and, for `MapLikeFormatter`, in rendering each
// element as `k: v` rather than as `(k, v)`.
//
// Elements are rendered in the escaped ("debug") presentation unless an
// element sub-specification is supplied: `{}` renders a range of strings as
// `["a"]`, and `{::}` as `[a]`.
//
// This component is for use within `bslfmt` only.  The `bsl::formatter`
// specializations that use it are provided by `bslfmt_formatterrange`.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Formatting A Range
///- - - - - - - - - - - - - - -
// Suppose we want to render a sequence of integers without its brackets and
// with each element in hexadecimal.
//
// First, we create a formatter for a range of `int` and parse a specification
// into it.  The `n` flag asks for no brackets, and everything after the `:`
// describes an individual element:
// ```
//  bslfmt::MockParseContext<char> parseContext("n:#x}", 0);
//
//  bslfmt::range_formatter<int, char> formatter;
//  formatter.parse(parseContext);
// ```
// Now, we format a range with it:
// ```
//  bsl::vector<int> value;
//  value.push_back(10);
//  value.push_back(11);
//
//  bslfmt::MockFormatContext<char> formatContext(0);
//  formatContext.advance_to(formatter.format(value, formatContext));
// ```
// Finally, we observe that the brackets are gone and that each element carries
// the `0x` prefix that `#x` requested:
// ```
//  assert("0xa, 0xb" == formatContext.finalString());
// ```

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

#include <bslfmt_bufferformatcontext.h>
#include <bslfmt_debugformatutil.h>
#include <bslfmt_escapeutil.h>
#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterpair.h>
#include <bslfmt_formatterspecificationnumericvalue.h>
#include <bslfmt_padutil.h>
#include <bslfmt_separators.h>
#include <bslfmt_rangeformatspecification.h>

#include <bslmf_assert.h>
#include <bslmf_ispair.h>
#include <bslmf_issame.h>
#include <bslmf_removecvref.h>

#include <bslstl_algorithm.h>
#include <bslstl_iterator.h>
#include <bslstl_pair.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>

namespace BloombergLP {
namespace bslfmt {

                    // ==================================
                    // struct RangeFormatter_MapSpecifier
                    // ==================================

/// [**PRIVATE**] Apply the `m` range type to an element formatter.  The
/// (template parameter) `t_IS_PAIR` indicates whether the element type is a
/// `bsl::pair`.
template <bool t_IS_PAIR>
struct RangeFormatter_MapSpecifier {

    // CLASS METHODS

    /// Throw `bsl::format_error`.
    template <class t_CHAR, class t_FORMATTER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void apply(t_FORMATTER *)
    {
        BSLS_THROW(bsl::format_error(
              "`m` specifier requires a pair element type"));          // THROW
    }
};

/// [**PRIVATE**] Specialization for a `bsl::pair` element type.
template <>
struct RangeFormatter_MapSpecifier<true> {

    // CLASS METHODS

    /// Configure the specified `underlying` pair formatter to render each
    /// element as `k: v`, without brackets.
    template <class t_CHAR, class t_FORMATTER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void apply(t_FORMATTER *underlying)
    {
        bsl::basic_string_view<t_CHAR> empty;
        underlying->set_brackets(empty, empty);
        underlying->set_separator(Separators<t_CHAR>::colonSeparator());
    }
};

                    // ====================================
                    // struct RangeFormatter_StringRenderer
                    // ====================================

/// [**PRIVATE**] Copy the elements of a range into a string, for the `s` and
/// `?s` range types.  The (template parameter) `t_IS_CHARACTER` indicates
/// whether the element type is the character type.
template <bool t_IS_CHARACTER>
struct RangeFormatter_StringRenderer {

    // CLASS METHODS

    /// Do nothing.
    template <class t_CHAR, class t_RANGE>
    static void load(bsl::basic_string<t_CHAR> *, t_RANGE&)
    {
        // Never called: `range_formatter::parse` rejects the `s` and `?s`
        // range types unless the element type is the character type.  This
        // overload exists only so that `format` compiles for other ranges.
    }
};

/// [**PRIVATE**] Specialization for a range whose element is the character
/// type.
template <>
struct RangeFormatter_StringRenderer<true> {

    // CLASS METHODS

    /// Load into the specified `text` the characters of the specified
    /// `range`.
    template <class t_CHAR, class t_RANGE>
    static void load(bsl::basic_string<t_CHAR> *text, t_RANGE& range)
    {
        text->assign(bsl::begin(range), bsl::end(range));
    }
};

                            // =====================
                            // class range_formatter
                            // =====================

/// Implementation base class for the `bsl::formatter` specializations for
/// sequence types.
template <class t_TYPE, class t_CHAR = char>
class range_formatter {
    // PRIVATE TYPES
    typedef BloombergLP::bslfmt::RangeFormatSpecification<t_CHAR>
                                                                 Specification;
    typedef typename bsl::remove_cvref<t_TYPE>::type               ElementType;
    typedef bsl::formatter<ElementType, t_CHAR>            UnderlyingFormatter;
    typedef bsl::is_same<ElementType, t_CHAR>               IsCharacterElement;
    typedef bslmf::IsPair<ElementType>                           IsPairElement;

    // DATA
    UnderlyingFormatter            d_underlying;      // element formatter
    bsl::basic_string_view<t_CHAR> d_separator;       // element separator
    bsl::basic_string_view<t_CHAR> d_openingBracket;  // opening bracket
    bsl::basic_string_view<t_CHAR> d_closingBracket;  // closing bracket
    Specification                  d_spec;            // parsed specification

    // PRIVATE ACCESSORS

    /// Write to the output of the specified `ctx` the opening bracket, the
    /// elements in the range `[begin, end)`, each formatted by the underlying
    /// formatter and separated by the separator, and the closing bracket.
    /// Return an iterator referring to the end of the output.
    template <class t_ITERATOR, class t_SENTINEL, class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator formatElements(
                                                  t_ITERATOR        begin,
                                                  t_SENTINEL        end,
                                                  t_FORMAT_CONTEXT& ctx) const;

    /// Write the specified `body` to the specified `ctx`, padded to the
    /// specified `finalWidth` using the fill and alignment of the specified
    /// `finalSpec`.  Return an iterator referring to the end of the output.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator padAndOutput(
                  const bsl::basic_string<t_CHAR>&           body,
                  const Specification&                       finalSpec,
                  const FormatterSpecificationNumericValue&  finalWidth,
                  t_FORMAT_CONTEXT&                          ctx) const;

  public:
    // CREATORS

    /// Create a formatter having `[` and `]` as its brackets and `, ` as its
    /// separator.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    range_formatter();

    // MANIPULATORS

    /// Set the separator written between consecutive elements to the
    /// specified `separator`.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    void set_separator(const bsl::basic_string_view<t_CHAR>& separator)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Set the brackets written before the first and after the last element
    /// to the specified `openingBracket` and `closingBracket` respectively.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    void set_brackets(const bsl::basic_string_view<t_CHAR>& openingBracket,
                      const bsl::basic_string_view<t_CHAR>& closingBracket)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Return a reference providing modifiable access to the formatter used
    /// to format individual elements.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    UnderlyingFormatter& underlying() BSLS_KEYWORD_NOEXCEPT;

    /// Parse the range format specification held by the specified `ctx`,
    /// passing any element specification to the underlying formatter.
    /// Return an iterator referring to the closing `}` of the specification.
    /// Throw `bsl::format_error` if the specification is invalid for a range
    /// of `t_TYPE`, including if it has the `m` range type and `t_TYPE` is
    /// not a `bsl::pair`, or the `s` or `?s` range type and `t_TYPE` is not
    /// `t_CHAR`.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20
    typename t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& ctx);

    // ACCESSORS

    /// Write the elements of the specified `range` to the output of the
    /// specified `ctx` as specified by the most recent call to `parse`.
    /// Return an iterator referring to the end of the output.  The behavior
    /// is undefined unless `parse` has been called on this object.
    template <class t_RANGE, class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(t_RANGE&          range,
                                               t_FORMAT_CONTEXT& ctx) const;

    /// Return a reference providing non-modifiable access to the formatter
    /// used to format individual elements.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    const UnderlyingFormatter& underlying() const BSLS_KEYWORD_NOEXCEPT;
};

                            // =======================
                            // struct SetLikeFormatter
                            // =======================

/// Implementation base class for the `bsl::formatter` specializations for
/// set types.
template <class t_TYPE, class t_CHAR = char>
struct SetLikeFormatter : range_formatter<t_TYPE, t_CHAR> {
    // CREATORS

    /// Create a formatter having `{` and `}` as its brackets and `, ` as its
    /// separator.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    SetLikeFormatter();
};

                            // =======================
                            // struct MapLikeFormatter
                            // =======================

/// Implementation base class for the `bsl::formatter` specializations for
/// map types.  The (template parameter) `t_TYPE` must be a `bsl::pair`.
template <class t_TYPE, class t_CHAR = char>
struct MapLikeFormatter : range_formatter<t_TYPE, t_CHAR> {
    // CREATORS

    /// Create a formatter having `{` and `}` as its brackets and `, ` as its
    /// separator, that renders each element as `k: v`.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    MapLikeFormatter();
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                            // ---------------------
                            // class range_formatter
                            // ---------------------

// PRIVATE ACCESSORS
template <class t_TYPE, class t_CHAR>
template <class t_ITERATOR, class t_SENTINEL, class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
range_formatter<t_TYPE, t_CHAR>::formatElements(t_ITERATOR        begin,
                                                t_SENTINEL        end,
                                                t_FORMAT_CONTEXT& ctx) const
{
    typename t_FORMAT_CONTEXT::iterator out = ctx.out();
    out = bsl::copy(d_openingBracket.begin(), d_openingBracket.end(), out);
    bool separate = false;
    for (; begin != end; ++begin) {
        if (separate) {
            out = bsl::copy(d_separator.begin(), d_separator.end(), out);
        }
        else {
            separate = true;
        }
        ctx.advance_to(out);
        out = d_underlying.format(*begin, ctx);
    }
    return bsl::copy(d_closingBracket.begin(), d_closingBracket.end(), out);
}

template <class t_TYPE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
range_formatter<t_TYPE, t_CHAR>::padAndOutput(
                   const bsl::basic_string<t_CHAR>&           body,
                   const Specification&                       finalSpec,
                   const FormatterSpecificationNumericValue&  finalWidth,
                   t_FORMAT_CONTEXT&                          ctx) const
{
    const bsl::basic_string_view<t_CHAR> filler(
                                       finalSpec.filler(),
                                       finalSpec.numFillerCharacters());

    return PadUtil<t_CHAR>::writePadded(ctx.out(),
                                        body,
                                        finalWidth,
                                        finalSpec.alignment(),
                                        filler,
                                        Specification::e_ALIGN_LEFT);
}

// CREATORS
template <class t_TYPE, class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
range_formatter<t_TYPE, t_CHAR>::range_formatter()
: d_separator(Separators<t_CHAR>::commaSeparator())
, d_openingBracket(Separators<t_CHAR>::openingSquareBracket())
, d_closingBracket(Separators<t_CHAR>::closingSquareBracket())
{
}

// MANIPULATORS
template <class t_TYPE, class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR_CPP20
void range_formatter<t_TYPE, t_CHAR>::set_separator(
                               const bsl::basic_string_view<t_CHAR>& separator)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    d_separator = separator;
}

template <class t_TYPE, class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR_CPP20
void range_formatter<t_TYPE, t_CHAR>::set_brackets(
                          const bsl::basic_string_view<t_CHAR>& openingBracket,
                          const bsl::basic_string_view<t_CHAR>& closingBracket)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    d_openingBracket = openingBracket;
    d_closingBracket = closingBracket;
}

template <class t_TYPE, class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR_CPP20
typename range_formatter<t_TYPE, t_CHAR>::UnderlyingFormatter&
range_formatter<t_TYPE, t_CHAR>::underlying() BSLS_KEYWORD_NOEXCEPT
{
    return d_underlying;
}

template <class t_TYPE, class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename t_PARSE_CONTEXT::iterator
range_formatter<t_TYPE, t_CHAR>::parse(t_PARSE_CONTEXT& ctx)
{
    d_spec.parse(&ctx, &d_underlying);

    if (d_spec.isString()) {
        if (!IsCharacterElement::value) {
            BSLS_THROW(bsl::format_error(
                "`s` and `?s` specifiers require a character element type"));
        }
        if (d_spec.noBrackets()) {
            BSLS_THROW(bsl::format_error(
                "`s` and `?s` specifiers cannot be combined with `n`"));
        }
        if (d_spec.hasElementSpecification()) {
            BSLS_THROW(bsl::format_error(
                "`s` and `?s` specifiers cannot be combined with an element "
                "specification"));
        }
    }

    // This must follow `d_spec.parse`, which parses the element specification
    // into `d_underlying` and would otherwise overwrite the presentation set
    // here.

    if (!d_spec.hasElementSpecification() && !d_spec.isString()) {
        DebugFormatUtil::setDebugFormat(&d_underlying);
    }

    // Apply the `m` range type: braces around the range, and `k: v` without
    // brackets for each element.  `MapLikeFormatter` performs the same
    // configuration in its constructor, so that a map is rendered that way
    // by default, and the underlying pair formatter's `parse` never restores
    // its defaults; for a `MapLikeFormatter` the code below is therefore a
    // no-op, but a plain `range_formatter` of pairs needs it.

    if (d_spec.isMap()) {
        RangeFormatter_MapSpecifier<IsPairElement::value>::template apply<
                                                      t_CHAR>(&d_underlying);

        this->set_brackets(Separators<t_CHAR>::openingBrace(),
                           Separators<t_CHAR>::closingBrace());
    }
    if (d_spec.noBrackets()) {
        bsl::basic_string_view<t_CHAR> empty;
        this->set_brackets(empty, empty);
    }
    if (ctx.begin() == ctx.end() || *ctx.begin() != t_CHAR('}')) {
        BSLS_THROW(bsl::format_error("Unexpected format string"));
    }
    return ctx.begin();
}

// ACCESSORS
template <class t_TYPE, class t_CHAR>
template <class t_RANGE, class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
range_formatter<t_TYPE, t_CHAR>::format(t_RANGE&          range,
                                        t_FORMAT_CONTEXT& ctx) const
{
    typedef BloombergLP::bslfmt::FormatterSpecificationNumericValue
                                                                  NumericValue;

    Specification finalSpec(d_spec);
    finalSpec.postprocess(ctx);

    const NumericValue finalWidth = finalSpec.postprocessedWidth();

    if (finalSpec.isString()) {
        bsl::basic_string<t_CHAR> text;

        RangeFormatter_StringRenderer<IsCharacterElement::value>::
                                         template load<t_CHAR>(&text, range);

        if (finalSpec.isEscapedString()) {
            bsl::basic_string<t_CHAR> escaped;
            EscapeUtil<t_CHAR>::escapeString(bsl::back_inserter(escaped),
                                             text);
            text.swap(escaped);
        }

        return padAndOutput(text, finalSpec, finalWidth, ctx);        // RETURN
    }

    // Fast path: no outer width -- no padding needed.
    if (finalWidth.category() == NumericValue::e_DEFAULT) {
        return formatElements(bsl::begin(range), bsl::end(range), ctx);
                                                                      // RETURN
    }

    // Slow path: render the range body into a temporary string, then emit it
    // with fill / align / width padding.  The buffer context forwards
    // argument lookups to `ctx`, so that an element specification containing
    // a nested replacement field -- as in `{::{}}` -- still finds the
    // argument it refers to.

    typedef BufferFormatContext<t_CHAR, t_FORMAT_CONTEXT> BufferContext;

    bsl::basic_string<t_CHAR> buffer;
    BufferContext             nested(&buffer, &ctx);

    formatElements(bsl::begin(range), bsl::end(range), nested);

    return padAndOutput(buffer, finalSpec, finalWidth, ctx);
}

template <class t_TYPE, class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR_CPP20
const typename range_formatter<t_TYPE, t_CHAR>::UnderlyingFormatter&
range_formatter<t_TYPE, t_CHAR>::underlying() const BSLS_KEYWORD_NOEXCEPT
{
    return d_underlying;
}

                            // -----------------------
                            // struct SetLikeFormatter
                            // -----------------------

// CREATORS
template <class t_TYPE, class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
SetLikeFormatter<t_TYPE, t_CHAR>::SetLikeFormatter()
{
    this->set_brackets(Separators<t_CHAR>::openingBrace(),
                       Separators<t_CHAR>::closingBrace());
}

                            // -----------------------
                            // struct MapLikeFormatter
                            // -----------------------

// CREATORS
template <class t_TYPE, class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
MapLikeFormatter<t_TYPE, t_CHAR>::MapLikeFormatter()
{
    // The element formatter must be a formatter for a pair, both because the
    // calls below exist only on that formatter and because rendering `k: v`
    // is meaningless otherwise.

    BSLMF_ASSERT((bslmf::IsPair<
                          typename bsl::remove_cvref<t_TYPE>::type>::value));

    this->set_brackets(Separators<t_CHAR>::openingBrace(),
                       Separators<t_CHAR>::closingBrace());

    RangeFormatter_MapSpecifier<true>::apply<t_CHAR>(&this->underlying());
}

}  // close package namespace
}  // close enterprise namespace

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

#endif  // INCLUDED_BSLFMT_RANGEFORMATTER

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
