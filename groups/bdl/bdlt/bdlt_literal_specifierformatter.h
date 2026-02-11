// bdlt_literal_specifierformatter.h                                  -*-C++-*-
#ifndef INCLUDED_BDLT_LITERAL_SPECIFIERFORMATTER
#define INCLUDED_BDLT_LITERAL_SPECIFIERFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a specifier formatter for formatting simple literal values.
//
//@CLASSES:
//  bdlt::Literal_SpecifierFormatter: basic format spec
//
//@DESCRIPTION: This common specifier formatter reads characters from the spec
// and parses those characters it recognizes.  In the parse phase, this updates
// the anticipated width of output, in the format phase, it does output
// according to the specification input.  Note that both parsing and formatting
// are independent of the type and value of the object being formatted.
//
// The common specifier formatter interprets the following sequences (beginning
// with '%'), ending with the following characters:
// '%' - output a '%'
// 'n' - output a '\n'
// 't' - output a '\t'

#include <bdlscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsla_fallthrough.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>

#include <bslstl_stringview.h>

namespace BloombergLP {
namespace bdlt {

                        // ================================
                        // class Literal_SpecifierFormatter
                        // ================================

/// This `class` does parsing and output of sequences whose corresponding
/// output is independent of the type and value of the object being formatted.
template <class t_CHAR>
class Literal_SpecifierFormatter {
    BSLMF_ASSERT((bsl::is_same<t_CHAR, char>::value ||
                  bsl::is_same<t_CHAR, wchar_t>::value));

    // PRIVATE TYPES
    typedef bsl::basic_string_view<t_CHAR> StringView;

    // DATA
    int d_totalWidth;

  public:
    // CREATORS

    /// Create an object with `totalWidth` set to 0.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    Literal_SpecifierFormatter();

    // MANIPULATORS

    /// Parse the front character of the specified `spec`.  If that character
    /// matches any of those recognized by this specifier formatter, increment
    /// `d_totalWidth`, pop the first character off of `spec` and return
    /// `true`, otherwise return `false`.  The behavior is undefined if `*spec`
    /// is empty.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool parseNextSpecifier(StringView *spec);

    // ACCESSORS

    /// Examine the front character of the specified `spec`.  If that character
    /// matches any of those recognized by this specifier formatter, perform
    /// the sort of output indicated by that character off of `spec` and return
    /// `true`, otherwise return `false`.
    template <class t_ITERATOR>
    bool formatNextSpecifier(StringView *spec, t_ITERATOR *outIt) const;

    /// Return the total width of the output anticipated from all the previous
    /// calls to `parseNextSpecifier`.
    int totalWidth() const;
};

                        // --------------------------------
                        // class Literal_SpecifierFormatter
                        // --------------------------------

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
Literal_SpecifierFormatter<t_CHAR>::Literal_SpecifierFormatter()
: d_totalWidth(0)
{}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool Literal_SpecifierFormatter<t_CHAR>::parseNextSpecifier(StringView *spec)
{
    BSLS_ASSERT(!spec->empty());

    switch (spec->front()) {
      case '%': BSLA_FALLTHROUGH;
      case 'n': BSLA_FALLTHROUGH;
      case 't': {
        ++d_totalWidth;
        spec->remove_prefix(1);

        return true;                                                  // RETURN
      } break;
    }

    return false;
}

// ACCESSORS
template <class t_CHAR>
template <class t_ITERATOR>
inline
bool Literal_SpecifierFormatter<t_CHAR>::
                 formatNextSpecifier(StringView *spec, t_ITERATOR *outIt) const
{
    BSLS_ASSERT_SAFE(!spec->empty());

    t_CHAR outChar;

    switch (spec->front()) {
      case t_CHAR('%'): {
        outChar = '%';
      } break;
      case t_CHAR('n'): {
        outChar = '\n';
      } break;
      case t_CHAR('t'): {
        outChar = '\t';
      } break;
      default: {
        return false;                                             // RETURN
      } break;
    }

    *(*outIt)++ = outChar;
    spec->remove_prefix(1);

    return true;
}

template <class t_CHAR>
inline
int Literal_SpecifierFormatter<t_CHAR>::totalWidth() const
{
    return d_totalWidth;
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
