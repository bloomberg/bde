// bslfmt_tupleformatspecification.h                                  -*-C++-*-
#ifndef INCLUDED_BSLFMT_TUPLEFORMATSPECIFICATION
#define INCLUDED_BSLFMT_TUPLEFORMATSPECIFICATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Utility to parse a format spec for `tuple` and `pair`.
//
//@CLASSES:
//  bslfmt::TupleFormatSpecificationEnums: namespace holding tuple spec enums
//  bslfmt::TupleFormatSpecification: `pair`/`tuple` format spec parser
//
//@SEE_ALSO: bslfmt_formatterpair, bslfmt_rangeformatspecification
//
//@DESCRIPTION: This component provides a `TupleFormatSpecification` class
// template that parses the `tuple-format-spec` of [format.tuple]:
// ```
//  tuple-fill-and-align(opt) width(opt) tuple-type(opt)
// ```
// where `tuple-type` is one of `m` and `n`.
//
// Note that a tuple specification has no element sub-specification, and no
// sign, `#`, zero-pad, precision or locale option.
//
// This component is for use within `bslfmt` only.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Parsing A Tuple Format Specification
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing a formatter for a pair and need to know which
// brackets and separator the caller has asked for.
//
// First, we parse a format string into a specification:
// ```
//  typedef bslfmt::TupleFormatSpecification<char> Specification;
//
//  bslfmt::MockParseContext<char> context("*<10m}", 0);
//
//  Specification spec;
//  spec.parse(&context);
// ```
// Finally, we verify the parsed specification:
// ```
//  assert(Specification::e_TUPLE_PAIR_MAP == spec.formatType());
//  assert('*'                             == *spec.filler());
//  assert(Specification::e_ALIGN_LEFT     == spec.alignment());
// ```

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatspecificationparser.h>
#include <bslfmt_formatterspecificationnumericvalue.h>

#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>

namespace BloombergLP {
namespace bslfmt {

                            // ====================================
                            // struct TupleFormatSpecificationEnums
                            // ====================================

/// This `struct` provides a namespace for the enumeration of the tuple
/// presentation types, and for the enumerations it inherits from
/// `FormatSpecificationParserEnums`.
struct TupleFormatSpecificationEnums : public FormatSpecificationParserEnums {
    // TYPES

    /// Enumeration used to describe the presentation type of a tuple.
    enum TupleType {
        e_TUPLE_DEFAULT,     // (v1, ...)
        e_TUPLE_PAIR_MAP,    // v1: v2
        e_TUPLE_NO_BRACKETS  // v1, ...
    };
};

                            // ==============================
                            // class TupleFormatSpecification
                            // ==============================

/// This class parses and holds the `tuple-format-spec` of a `bsl::pair` or
/// `bsl::tuple` format string: the fill, alignment and width, and the
/// presentation type selected by the trailing `m` or `n` letter.
template <class t_CHAR>
class TupleFormatSpecification : public TupleFormatSpecificationEnums {
    // PRIVATE TYPES
    typedef FormatSpecificationParser<t_CHAR> Parser;

    // DATA
    Parser    d_basicParser;
    TupleType d_type;

  public:
    // CREATORS

    /// Create an unparsed specification having the default presentation
    /// type, `e_TUPLE_DEFAULT`.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    TupleFormatSpecification();

    // MANIPULATORS

    /// Parse the tuple format specification held by the specified `ctx`.  On
    /// return, `ctx` refers to the closing `}` or to the end of the format
    /// string.  Throw `bsl::format_error` if the specification is invalid,
    /// including when `:` is given as the fill character.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20
    void parse(t_PARSE_CONTEXT *ctx);

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

    /// Return the number of `t_CHAR` units in the fill character returned by
    /// `filler`.  If `parse` has not previously been called throw a
    /// `bsl::format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    int numFillerCharacters() const;

    /// Return the presentation type selected by `parse`.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    TupleType formatType() const;

    /// Return the numeric width extracted during postprocessing.  If
    /// `postprocess` has not previously been called throw a `format_error`
    /// exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    FormatterSpecificationNumericValue postprocessedWidth() const;
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                            // ------------------------------
                            // class TupleFormatSpecification
                            // ------------------------------

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
TupleFormatSpecification<t_CHAR>::TupleFormatSpecification()
: d_type(e_TUPLE_DEFAULT)
{
}

// MANIPULATORS
template <class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20
void TupleFormatSpecification<t_CHAR>::parse(t_PARSE_CONTEXT *ctx)
{
    const Sections sect = static_cast<Sections>(
            e_SECTIONS_FILL_ALIGN |
            e_SECTIONS_WIDTH |
            e_SECTIONS_REMAINING_SPEC);

    d_basicParser.parse(ctx, sect);

    // Reject `:` as an explicit fill character: [format.tuple] excludes it,
    // along with `{` and `}`, from the characters permitted as a tuple fill.
    if (d_basicParser.numFillerCharacters() == 1 &&
        d_basicParser.filler()[0] == t_CHAR(':')) {
        BSLS_THROW(bsl::format_error(
                        "Invalid fill character (':')"));              // THROW
    }

    if (d_basicParser.remainingSpec().length() > 1) {
        BSLS_THROW(bsl::format_error("Unexpected format string"));     // THROW
    }
    if (d_basicParser.remainingSpec().length() == 1) {
        t_CHAR type = d_basicParser.remainingSpec().front();
        switch(type) {
          case t_CHAR('n'): {
            d_type = e_TUPLE_NO_BRACKETS;
          } break;
          case t_CHAR('m'): {
            d_type = e_TUPLE_PAIR_MAP;
          } break;
          default: {
            BSLS_THROW(bsl::format_error("Invalid format string"));    // THROW
          } break;
        }
    }

    if (ctx->begin() == ctx->end() || *ctx->begin() == '}') {
        return;                                                       // RETURN
    }

    BSLS_THROW(bsl::format_error("Specification parse failure "
                                 "(invalid character)"));              // THROW
}

template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
void TupleFormatSpecification<t_CHAR>::postprocess(
                                              const t_FORMAT_CONTEXT& context)
{
    d_basicParser.postprocess(context);
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatSpecificationParserEnums::Alignment
TupleFormatSpecification<t_CHAR>::alignment() const
{
    return d_basicParser.alignment();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const t_CHAR *TupleFormatSpecification<t_CHAR>::filler() const
{
    return d_basicParser.filler();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int TupleFormatSpecification<t_CHAR>::numFillerCharacters() const
{
    return d_basicParser.numFillerCharacters();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
TupleFormatSpecificationEnums::TupleType
TupleFormatSpecification<t_CHAR>::formatType() const
{
    return d_type;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatterSpecificationNumericValue
TupleFormatSpecification<t_CHAR>::postprocessedWidth() const
{
    return d_basicParser.postprocessedWidth();
}

}  // close package namespace
}  // close enterprise namespace

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
