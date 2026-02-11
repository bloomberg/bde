// bdlt_formatter.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLT_FORMATTER
#define INCLUDED_BDLT_FORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard compliant `format` implementation.
//
//@CLASSES:
//  bdlt::Formatter: `class` for parsing specs and formatting variables.
//
//@DESCRIPTION: This component provides a class template, `Formatter` that is
// configured with a `tt_SPECIFIER_FORMATTER` template class.  Each type that
// is to be formatted has its own specifier formatter class, and the specifier
// class does most of the work of formatting the output for the type.  The
// `t_CHAR` template parameter is always to be `char` or `wchar_t`.
//
///Usage
///-----

#include <bdlscm_version.h>

#include <bdlt_formatutil.h>
#include <bdlt_literal_specifierformatter.h>

#include <bslfmt_format.h>
#include <bslfmt_formatspecificationparser.h>
#include <bslfmt_padutil.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>

namespace BloombergLP {
namespace bdlt {

                                    // =========
                                    // Formatter
                                    // =========

template <template<typename> class tt_SPECIFIER_FORMATTER, class t_CHAR>
class Formatter {
    BSLMF_ASSERT((bsl::is_same<t_CHAR, char>::value ||
                  bsl::is_same<t_CHAR, wchar_t>::value));

    // PRIVATE TYPES
    typedef bslfmt::FormatSpecificationParser<t_CHAR> Spec;
    typedef bslfmt::PadUtil<t_CHAR>                   PadUtil;
    typedef bsl::basic_string_view<t_CHAR>            StringView;
    typedef bsl::ptrdiff_t                            ptrdiff_t;

  private:
    // DATA
    mutable Spec                           d_spec;
        // Specification parser.  Mutable because we modify it in the `format`
        // accessor.

    Literal_SpecifierFormatter<t_CHAR>     d_literalFormatter;
        // Formatter for literal characters.

    mutable tt_SPECIFIER_FORMATTER<t_CHAR> d_specifierFormatter;
        // Custom formatter for the type being formatted.

    int                                    d_literalWidth;
        // Number of characters in the parsed spec after the first '%' that are
        // not in '%'-sequences.  They will be written to output without
        // modification.

    bsl::size_t                            d_numModifiers;
        // Number of modifiers at the start of the spec.

    // PRIVATE ACCESSORS

    /// Parse modifiers at the beginning of the specified `*spec`.  Throw an
    /// instance of `bsl::format_error` if an unrecognized modifier character
    /// is encountered before either the first '%' or the end of `*spec`.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseModifiers(StringView *spec);

  public:
    // CREATORS

    /// Default-construct an object.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    Formatter();

    // MANIPULATORS

    /// Parse the input and arguments specified by `context` and return an
    /// iterator pointing after the end of parsed input.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                     t_PARSE_CONTEXT& context);

    // ACCESSORS

    /// Write the output to `formatContext.out()` and return the output
    /// iterator after the output is done.
    template <class t_FORMAT_CONTEXT, class t_VALUE>
    typename t_FORMAT_CONTEXT::iterator format(
                                        const t_VALUE&    value,
                                        t_FORMAT_CONTEXT& formatContext) const;
};

                                  // ---------
                                  // Formatter
                                  // ---------

// PRIVATE ACCESSORS
template <template<typename> class tt_SPECIFIER_FORMATTER, class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void Formatter<tt_SPECIFIER_FORMATTER, t_CHAR>::
                                               parseModifiers(StringView *spec)
{
    while (!spec->empty() && t_CHAR('%') != spec->front()) {
        if (!d_specifierFormatter.parseNextModifier(spec)) {
            BSLS_THROW(bsl::format_error("Illegal modifier"));
        }

        ++d_numModifiers;
    }
}

// CREATORS
template <template<typename> class tt_SPECIFIER_FORMATTER, class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
Formatter<tt_SPECIFIER_FORMATTER, t_CHAR>::Formatter()
: d_spec()
, d_literalFormatter()
, d_specifierFormatter()
, d_literalWidth(0)
, d_numModifiers(0)
{}

// MANIPULATORS
template <template<typename> class tt_SPECIFIER_FORMATTER, class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename t_PARSE_CONTEXT::iterator
Formatter<tt_SPECIFIER_FORMATTER, t_CHAR>::parse(t_PARSE_CONTEXT& context)
{
    typedef typename Spec::Sections Sections;
    const Sections sections = static_cast<Sections>(
                         Spec::e_SECTIONS_WIDTH | Spec::e_SECTIONS_FILL_ALIGN |
                         Spec::e_SECTIONS_REMAINING_SPEC |
                                         d_specifierFormatter.extraSections());

    d_spec.parse(&context, sections);

    StringView spec = d_spec.remainingSpec();
    parseModifiers(&spec);

    if (spec.empty()) {
        d_specifierFormatter.parseDefault();
    }
    else {
        if (spec.front() != t_CHAR('%')) {
            BSLS_THROW(bsl::format_error(
                                       "Format string must start with a '%'"));
        }

        while (!spec.empty()) {
            const t_CHAR c = spec.front();
            spec.remove_prefix(1);

            if (t_CHAR('%') == c) {
                if (spec.empty()) {
                    BSLS_THROW(bsl::format_error("Incomplete '%' sequence"));
                }

                if (!d_specifierFormatter.parseNextSpecifier(&spec) &&
                    !d_literalFormatter.  parseNextSpecifier(&spec)) {
                    BSLS_THROW(bsl::format_error(
                                             "Unrecognized format specifier"));
                }
            }
            else {
                ++d_literalWidth;
            }
        }
    }

    return context.begin();
}

// ACCESSORS

template <template<typename> class tt_SPECIFIER_FORMATTER, class t_CHAR>
template <class t_FORMAT_CONTEXT, class t_VALUE>
typename t_FORMAT_CONTEXT::iterator
Formatter<tt_SPECIFIER_FORMATTER, t_CHAR>::format(
                                         const t_VALUE&    value,
                                         t_FORMAT_CONTEXT& formatContext) const
{
    BSLMF_ASSERT((bsl::is_same<t_CHAR,
                               typename t_FORMAT_CONTEXT::char_type>::value));

    d_spec.postprocess(formatContext);
    StringView spec = d_spec.remainingSpec().substr(d_numModifiers);

    d_specifierFormatter.postprocess(d_spec);

    const ptrdiff_t contentWidth = d_literalWidth +
                                              d_literalFormatter.totalWidth() +
                                        d_specifierFormatter.totalWidth(value);

    ptrdiff_t leftPadding, rightPadding;
    PadUtil::computePadding(&leftPadding,
                            &rightPadding,
                            d_spec.postprocessedWidth(),
                            contentWidth,
                            d_spec.alignment());
    StringView filler(d_spec.filler(), d_spec.numFillerCharacters());

    typename t_FORMAT_CONTEXT::iterator outIt = formatContext.out();

    outIt = PadUtil::pad(outIt, leftPadding, filler);

    if (spec.empty()) {
        outIt = d_specifierFormatter.formatDefault(outIt, value);
    }
    else {
        BSLS_ASSERT(t_CHAR('%') == spec.front());

        while (!spec.empty()) {
            const t_CHAR c = spec.front();
            spec.remove_prefix(1);

            if (c == t_CHAR('%')) {
                BSLS_ASSERT(!spec.empty());

                if (!d_specifierFormatter.formatNextSpecifier(
                                                       &spec, &outIt, value) &&
                      !d_literalFormatter.formatNextSpecifier(&spec, &outIt)) {
                    BSLS_ASSERT_INVOKE_NORETURN(
                                              "unrecognized format specifier");
                }
            }
            else {
                *outIt++ = c;
            }
        }
    }

    outIt = PadUtil::pad(outIt, rightPadding, filler);

    return outIt;
}

}  // close namespace bdlt
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
