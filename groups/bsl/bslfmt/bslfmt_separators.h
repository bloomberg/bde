// bslfmt_separators.h                                                -*-C++-*-
#ifndef INCLUDED_BSLFMT_SEPARATORS
#define INCLUDED_BSLFMT_SEPARATORS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a set of `char`/`wchar_t` separator constants.
//
//@CLASSES:
//  bslfmt::Separators: bracket and separator constants
//
//@SEE_ALSO: bslfmt_rangeformatter, bslfmt_formatterpair
//
//@DESCRIPTION: This component provides a `Separators` class template that is a
// namespace for the bracket and separator strings from which the range and
// pair formatters assemble their output: `(`, `)`, `{`, `}`, `[`, `]`, `, `
// and `: `.
//
// This component is for use within `bslfmt` only.

#include <bslscm_version.h>

#include <bslstl_stringview.h>

#include <bsls_keyword.h>

#include <cstddef>  // `std::size_t`

namespace BloombergLP {
namespace bslfmt {

                            // ================
                            // class Separators
                            // ================

/// This utility class template provides string views of bracket and separator
/// characters.  The referenced characters have static storage duration.  Note
/// that this class is intended to be used only with the `char` and `wchar_t`
/// types.
template <class t_CHAR>
class Separators {
    // CLASS DATA
    static const t_CHAR k_OPENING_PAREN[1];           // `(`
    static const t_CHAR k_CLOSING_PAREN[1];           // `)`
    static const t_CHAR k_OPENING_BRACE[1];           // `{`
    static const t_CHAR k_CLOSING_BRACE[1];           // `}`
    static const t_CHAR k_OPENING_SQUARE_BRACKET[1];  // `[`
    static const t_CHAR k_CLOSING_SQUARE_BRACKET[1];  // `]`
    static const t_CHAR k_COMMA_SEPARATOR[2];         // `, `
    static const t_CHAR k_COLON_SEPARATOR[2];         // `: `

    // PRIVATE CLASS METHODS

    /// Return a string view of all of the specified `chars`.
    template <std::size_t k_SIZE>
    static BSLS_KEYWORD_CONSTEXPR
    bsl::basic_string_view<t_CHAR> toStringView(const t_CHAR (&chars)[k_SIZE]);

  public:
    // CLASS METHODS

    /// Return a string view of `(`.
    static BSLS_KEYWORD_CONSTEXPR
    bsl::basic_string_view<t_CHAR> openingParen();

    /// Return a string view of `)`.
    static BSLS_KEYWORD_CONSTEXPR
    bsl::basic_string_view<t_CHAR> closingParen();

    /// Return a string view of `{`.
    static BSLS_KEYWORD_CONSTEXPR
    bsl::basic_string_view<t_CHAR> openingBrace();

    /// Return a string view of `}`.
    static BSLS_KEYWORD_CONSTEXPR
    bsl::basic_string_view<t_CHAR> closingBrace();

    /// Return a string view of `[`.
    static BSLS_KEYWORD_CONSTEXPR
    bsl::basic_string_view<t_CHAR> openingSquareBracket();

    /// Return a string view of `]`.
    static BSLS_KEYWORD_CONSTEXPR
    bsl::basic_string_view<t_CHAR> closingSquareBracket();

    /// Return a string view of `, ` (a comma followed by a space).
    static BSLS_KEYWORD_CONSTEXPR
    bsl::basic_string_view<t_CHAR> commaSeparator();

    /// Return a string view of `: ` (a colon followed by a space).
    static BSLS_KEYWORD_CONSTEXPR
    bsl::basic_string_view<t_CHAR> colonSeparator();
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                            // ----------------
                            // class Separators
                            // ----------------

// CLASS DATA
template <class t_CHAR>
const t_CHAR Separators<t_CHAR>::k_OPENING_PAREN[1] = {t_CHAR('(')};
template <class t_CHAR>
const t_CHAR Separators<t_CHAR>::k_CLOSING_PAREN[1] = {t_CHAR(')')};
template <class t_CHAR>
const t_CHAR Separators<t_CHAR>::k_OPENING_BRACE[1] = {t_CHAR('{')};
template <class t_CHAR>
const t_CHAR Separators<t_CHAR>::k_CLOSING_BRACE[1] = {t_CHAR('}')};
template <class t_CHAR>
const t_CHAR Separators<t_CHAR>::k_OPENING_SQUARE_BRACKET[1] = {t_CHAR('[')};
template <class t_CHAR>
const t_CHAR Separators<t_CHAR>::k_CLOSING_SQUARE_BRACKET[1] = {t_CHAR(']')};
template <class t_CHAR>
const t_CHAR Separators<t_CHAR>::k_COMMA_SEPARATOR[2] =
                                                    {t_CHAR(','), t_CHAR(' ')};
template <class t_CHAR>
const t_CHAR Separators<t_CHAR>::k_COLON_SEPARATOR[2] =
                                                    {t_CHAR(':'), t_CHAR(' ')};

// PRIVATE CLASS METHODS
template <class t_CHAR>
template <std::size_t k_SIZE>
inline BSLS_KEYWORD_CONSTEXPR
bsl::basic_string_view<t_CHAR> Separators<t_CHAR>::toStringView(
                                                 const t_CHAR (&chars)[k_SIZE])
{
    return bsl::basic_string_view<t_CHAR>(chars, k_SIZE);
}

// CLASS METHODS
template <class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR
bsl::basic_string_view<t_CHAR> Separators<t_CHAR>::openingParen()
{
    return toStringView(k_OPENING_PAREN);
}

template <class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR
bsl::basic_string_view<t_CHAR> Separators<t_CHAR>::closingParen()
{
    return toStringView(k_CLOSING_PAREN);
}

template <class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR
bsl::basic_string_view<t_CHAR> Separators<t_CHAR>::openingBrace()
{
    return toStringView(k_OPENING_BRACE);
}

template <class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR
bsl::basic_string_view<t_CHAR> Separators<t_CHAR>::closingBrace()
{
    return toStringView(k_CLOSING_BRACE);
}

template <class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR
bsl::basic_string_view<t_CHAR> Separators<t_CHAR>::openingSquareBracket()
{
    return toStringView(k_OPENING_SQUARE_BRACKET);
}

template <class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR
bsl::basic_string_view<t_CHAR> Separators<t_CHAR>::closingSquareBracket()
{
    return toStringView(k_CLOSING_SQUARE_BRACKET);
}

template <class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR
bsl::basic_string_view<t_CHAR> Separators<t_CHAR>::commaSeparator()
{
    return toStringView(k_COMMA_SEPARATOR);
}

template <class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR
bsl::basic_string_view<t_CHAR> Separators<t_CHAR>::colonSeparator()
{
    return toStringView(k_COLON_SEPARATOR);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_SEPARATORS

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
