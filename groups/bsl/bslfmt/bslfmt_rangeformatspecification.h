// bslfmt_rangeformatspecification.h                                  -*-C++-*-
#ifndef INCLUDED_BSLFMT_RANGEFORMATSPECIFICATION
#define INCLUDED_BSLFMT_RANGEFORMATSPECIFICATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Utility to parse a format spec for ranges.
//
//@CLASSES:
//  bslfmt::RangeFormatSpecification: format spec parser for ranges
//
//@SEE_ALSO: bslfmt_rangeformatter, bslfmt_tupleformatspecification
//
//@DESCRIPTION: This component provides a `RangeFormatSpecification` class
// template that parses the `range-format-spec` of [format.range.formatter]:
// ```
//  range-fill-and-align(opt) width(opt) n(opt) range-type(opt)
//                                                  range-underlying-spec(opt)
// ```
// where `range-type` is one of `m`, `s` and `?s`, and `range-underlying-spec`
// is a `:` followed by a specification for the individual elements.
//
// This component is for use within `bslfmt` only.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Parsing A Range Format Specification
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing a formatter for a range and need to know how to
// punctuate the range itself.
//
// First, we parse a format string into a specification, supplying a formatter
// for `int` to parse the element specification:
// ```
//  typedef bslfmt::RangeFormatSpecification<char> Specification;
//
//  bslfmt::MockParseContext<char> context("*<20n:#x}", 0);
//
//  Specification             spec;
//  bsl::formatter<int, char> element;
//
//  spec.parse(&context, &element);
// ```
// Finally, we verify the parsed specification:
// ```
//  assert(true                        == spec.noBrackets());
//  assert(false                       == spec.isMap());
//  assert(true                        == spec.hasElementSpecification());
//  assert('*'                         == *spec.filler());
//  assert(Specification::e_ALIGN_LEFT == spec.alignment());
// ```

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatspecificationparser.h>
#include <bslfmt_formatterspecificationnumericvalue.h>

#include <bslstl_iterator.h>
#include <bslstl_stringview.h>

#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <cstddef>  // `std::size_t`

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT
namespace BloombergLP {
namespace bslfmt {

                  // ===========================================
                  // class RangeFormatSpecification_ProxyContext
                  // ===========================================

/// This component-private class template provides a minimal parse context
/// facade over the characters held by another parse context.  Argument-id
/// operations are forwarded to the parse context supplied on construction,
/// but advancing an object of this type does *not* advance that context.
template <class t_CHAR, class t_PARSE_CONTEXT>
class RangeFormatSpecification_ProxyContext {
  public:
    // TYPES
    typedef t_CHAR char_type;
    typedef
        typename bsl::basic_string_view<t_CHAR>::const_iterator const_iterator;
    typedef const_iterator                                      iterator;

  private:
    // DATA
    bsl::basic_string_view<t_CHAR>  d_spec;       // specification
    const_iterator                  d_unparsed;   // start of unparsed portion
                                                  // of `d_spec`
    t_PARSE_CONTEXT                *d_context_p;  // context to which
                                                  // argument-id operations
                                                  // are forwarded (held, not
                                                  // owned)

  public:
    // CREATORS

    /// Create an object presenting the characters of the specified `spec` and
    /// forwarding argument-id operations to the specified `context`.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    RangeFormatSpecification_ProxyContext(
                               const bsl::basic_string_view<t_CHAR>&  spec,
                               t_PARSE_CONTEXT                       *context);

    // MANIPULATORS

    /// Set the parse position of this object to the specified `it`.  The
    /// behavior is undefined unless `begin() <= it` and `it <= end()`.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    void advance_to(const_iterator it);

    /// Verify, using the parse context supplied on construction, that the
    /// specified `id` is a valid argument index.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    void check_arg_id(std::size_t id);

    /// Return the next argument index obtained from the parse context
    /// supplied on construction.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    std::size_t next_arg_id();

    // ACCESSORS

    /// Return the current parse position.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    const_iterator begin() const;

    /// Return the end of the specification.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    const_iterator end() const;
};

                            // ==============================
                            // class RangeFormatSpecification
                            // ==============================

/// This class parses and holds the `range-format-spec` of a range format
/// string: the fill, alignment and width, the `n` flag, the range type, and
/// whether an element sub-specification was supplied.
template <class t_CHAR>
class RangeFormatSpecification : public FormatSpecificationParserEnums {
    // PRIVATE TYPES
    typedef FormatSpecificationParser<t_CHAR> Parser;

    // DATA
    Parser d_basicParser;       // parser of the fill, alignment and width
    bool   d_isMap;             // the `m` range type was supplied
    bool   d_noBrackets;        // the `n` flag was supplied
    bool   d_hasElementSpec;    // an element sub-specification was supplied
    bool   d_isString;          // the `s` or `?s` range type was supplied
    bool   d_isEscapedString;   // the `?s` range type was supplied

  public:
    // CREATORS

    /// Create an unparsed specification.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    RangeFormatSpecification();

    // MANIPULATORS

    /// Parse the range format specification held by the specified `ctx`,
    /// have the specified `underlying` formatter parse the element
    /// sub-specification, if any, and advance `ctx` to the position at which
    /// `underlying` stops.  Throw `bsl::format_error` if the specification is
    /// invalid.
    template <class t_PARSE_CONTEXT, class t_UNDERLYING_FORMATTER>
    BSLS_KEYWORD_CONSTEXPR_CPP20
    void parse(t_PARSE_CONTEXT *ctx, t_UNDERLYING_FORMATTER *underlying);

    /// Update any nested width value to contain an actual value based on the
    /// appropriate arguments held by the specified `context` format context.
    /// In the event of an error throw a `format_error` exception.
    template <class t_FORMAT_CONTEXT>
    void postprocess(const t_FORMAT_CONTEXT& context);

    // ACCESSORS

    /// Return the alignment parsed by `parse`.  If `parse` has not previously
    /// been called throw a `bsl::format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    Alignment alignment() const;

    /// Return the address of the fill character, which may be a multi-unit
    /// code point, parsed by `parse`.  If `parse` has not previously been
    /// called throw a `bsl::format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    const t_CHAR *filler() const;

    /// Return `true` if the specification parsed by this object supplied an
    /// element sub-specification -- the part after the `:` -- and `false`
    /// otherwise.  Note that an empty sub-specification, as in `{::}`, counts
    /// as supplied.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    bool hasElementSpecification() const;

    /// Return `true` if the specification parsed by this object supplied the
    /// `?s` range type, and `false` otherwise.  Note that `isString` is also
    /// `true` whenever this is.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    bool isEscapedString() const;

    /// Return `true` if the specification parsed by this object supplied the
    /// `m` range type, and `false` otherwise.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    bool isMap() const;

    /// Return `true` if the specification parsed by this object supplied the
    /// `s` or `?s` range type, and `false` otherwise.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    bool isString() const;

    /// Return `true` if the specification parsed by this object supplied the
    /// `n` flag, and `false` otherwise.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    bool noBrackets() const;

    /// Return the number of `t_CHAR` units in the fill character returned by
    /// `filler`.  If `parse` has not previously been called throw a
    /// `bsl::format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    int numFillerCharacters() const;

    /// Return the numeric width extracted during postprocessing.  If
    /// `postprocess` has not previously been called throw a `format_error`
    /// exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    FormatterSpecificationNumericValue postprocessedWidth() const;
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                  // -------------------------------------------
                  // class RangeFormatSpecification_ProxyContext
                  // -------------------------------------------

// CREATORS
template <class t_CHAR, class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20
RangeFormatSpecification_ProxyContext<t_CHAR, t_PARSE_CONTEXT>::
    RangeFormatSpecification_ProxyContext(
                               const bsl::basic_string_view<t_CHAR>&  spec,
                               t_PARSE_CONTEXT                       *context)
: d_spec(spec)
, d_unparsed(d_spec.begin())
, d_context_p(context)
{
}

// MANIPULATORS
template <class t_CHAR, class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20
void RangeFormatSpecification_ProxyContext<t_CHAR, t_PARSE_CONTEXT>::
    advance_to(const_iterator it)
{
    d_unparsed = it;
}

template <class t_CHAR, class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20
void RangeFormatSpecification_ProxyContext<t_CHAR, t_PARSE_CONTEXT>::
    check_arg_id(std::size_t id)
{
    d_context_p->check_arg_id(id);
}

template <class t_CHAR, class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20
std::size_t RangeFormatSpecification_ProxyContext<t_CHAR, t_PARSE_CONTEXT>::
    next_arg_id()
{
    return d_context_p->next_arg_id();
}

// ACCESSORS
template <class t_CHAR, class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename RangeFormatSpecification_ProxyContext<t_CHAR, t_PARSE_CONTEXT>::
    const_iterator
RangeFormatSpecification_ProxyContext<t_CHAR, t_PARSE_CONTEXT>::begin() const
{
    return d_unparsed;
}

template <class t_CHAR, class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename RangeFormatSpecification_ProxyContext<t_CHAR, t_PARSE_CONTEXT>::
    const_iterator
RangeFormatSpecification_ProxyContext<t_CHAR, t_PARSE_CONTEXT>::end() const
{
    return d_spec.end();
}

                            // ------------------------------
                            // class RangeFormatSpecification
                            // ------------------------------

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
RangeFormatSpecification<t_CHAR>::RangeFormatSpecification()
: d_isMap(false)
, d_noBrackets(false)
, d_hasElementSpec(false)
, d_isString(false)
, d_isEscapedString(false)
{
}

// MANIPULATORS
template <class t_CHAR>
template <class t_PARSE_CONTEXT, class t_UNDERLYING_FORMATTER>
BSLS_KEYWORD_CONSTEXPR_CPP20
void RangeFormatSpecification<t_CHAR>::parse(
                                            t_PARSE_CONTEXT        *ctx,
                                            t_UNDERLYING_FORMATTER *underlying)
{
    typedef typename t_PARSE_CONTEXT::const_iterator ContextIterator;
    typedef typename bsl::iterator_traits<ContextIterator>::difference_type
                                                                    Difference;
    typedef RangeFormatSpecification_ProxyContext<t_CHAR, t_PARSE_CONTEXT>
                                                                  ProxyContext;

    // A parse context can only ever be advanced *forwards* -- `advance_to`
    // requires its argument to be reachable from `begin()` -- but the leading
    // sections of a range specification cannot be parsed without consuming
    // the element sub-specification that follows them.  Parse those sections
    // using a proxy over the same characters, leaving `ctx` untouched, and
    // then advance `ctx` exactly once, to the position at which the
    // underlying element formatter is to resume.

    const bsl::basic_string_view<t_CHAR> spec(ctx->begin(), ctx->end());

    ProxyContext proxy(spec, ctx);

    // A leading `:` denotes the underlying-element sub-spec separator, so
    // skip fill/align/width parsing in that case to avoid misconsuming
    // `:<...` as a fill-and-align pair.
    const bool leadingColon = (!spec.empty() && spec.front() == t_CHAR(':'));

    const Sections sect = leadingColon
        ? static_cast<Sections>(e_SECTIONS_REMAINING_SPEC)
        : static_cast<Sections>(e_SECTIONS_FILL_ALIGN |
                                e_SECTIONS_WIDTH |
                                e_SECTIONS_REMAINING_SPEC);

    d_basicParser.parse(&proxy, sect);

    bsl::basic_string_view<t_CHAR> remaining = d_basicParser.remainingSpec();

    if (!remaining.empty() && remaining.front() == t_CHAR('n')) {
        d_noBrackets = true;
        remaining.remove_prefix(1);
    }

    // At most one range type is consumed.  A `?` that is not followed by `s`
    // is left in place, to be rejected below as a stray character.

    if (!remaining.empty() && remaining.front() == t_CHAR('m')) {
        d_isMap = true;
        remaining.remove_prefix(1);
    }
    else if (!remaining.empty() && remaining.front() == t_CHAR('s')) {
        d_isString = true;
        remaining.remove_prefix(1);
    }
    else if (remaining.size() >= 2 && remaining[0] == t_CHAR('?') &&
                                      remaining[1] == t_CHAR('s')) {
        d_isString        = true;
        d_isEscapedString = true;
        remaining.remove_prefix(2);
    }

    // Determine where the underlying element formatter is to resume: the
    // character following the `:` separator if there is an element
    // sub-specification, and the end of the range specification otherwise.
    // Positions are tracked as offsets into `spec`; note that `remaining`
    // ends where `proxy` stopped parsing.

    Difference resume = bsl::ssize(spec) -
                                     bsl::distance(proxy.begin(), proxy.end());

    d_hasElementSpec = !remaining.empty();

    if (!remaining.empty()) {
        if (remaining.front() != t_CHAR(':')) {
            BSLS_THROW(bsl::format_error("Specification parse failure "
                                         "(invalid character)"));      // THROW
        }

        remaining.remove_prefix(1);
        resume -= bsl::ssize(remaining);
    }

    ContextIterator current = ctx->begin();
    current += resume;

    ctx->advance_to(current);
    ctx->advance_to(underlying->parse(*ctx));
}

template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
void RangeFormatSpecification<t_CHAR>::postprocess(
                                              const t_FORMAT_CONTEXT& context)
{
    d_basicParser.postprocess(context);
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatSpecificationParserEnums::Alignment
RangeFormatSpecification<t_CHAR>::alignment() const
{
    return d_basicParser.alignment();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const t_CHAR *RangeFormatSpecification<t_CHAR>::filler() const
{
    return d_basicParser.filler();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool RangeFormatSpecification<t_CHAR>::hasElementSpecification() const
{
    return d_hasElementSpec;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool RangeFormatSpecification<t_CHAR>::isEscapedString() const
{
    return d_isEscapedString;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool RangeFormatSpecification<t_CHAR>::isMap() const
{
    return d_isMap;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool RangeFormatSpecification<t_CHAR>::isString() const
{
    return d_isString;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool RangeFormatSpecification<t_CHAR>::noBrackets() const
{
    return d_noBrackets;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int RangeFormatSpecification<t_CHAR>::numFillerCharacters() const
{
    return d_basicParser.numFillerCharacters();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatterSpecificationNumericValue
RangeFormatSpecification<t_CHAR>::postprocessedWidth() const
{
    return d_basicParser.postprocessedWidth();
}

}  // close package namespace
}  // close enterprise namespace

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
