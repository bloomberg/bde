// bslfmt_displaywidthutil.h                                          -*-C++-*-
#ifndef INCLUDED_BSLFMT_DISPLAYWIDTHUTIL
#define INCLUDED_BSLFMT_DISPLAYWIDTHUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities to compute the display width of text.
//
//@CLASSES:
//  bslfmt::DisplayWidthUtil: namespace for display width functions
//
//@SEE_ALSO: bslfmt_formatterstring, bslfmt_padutil
//
//@DESCRIPTION: This component provides a utility `struct`,
// `DisplayWidthUtil`, containing functions that compute the display width, in
// columns, of Unicode text using the rules in [format.string.std]: the text is
// divided into extended grapheme clusters, and each cluster is as wide as its
// first code point.  Text of `char` type is treated as UTF-8, and text of
// `wchar_t` type as UTF-16 or UTF-32, depending on the size of `wchar_t`.
// Note that the standard calls this width the *estimated* width, because a
// particular terminal or font may render text differently.
//
// This component is for use within `bslfmt` only.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Measuring Text
///- - - - - - - - - - - - -
// Suppose we need to know how many columns a piece of text occupies, and how
// much of it fits within a given number of columns.
//
// First, we measure a string consisting of `a` followed by the CJK character
// U+4E2D, which is two columns wide:
// ```
//  typedef bslfmt::DisplayWidthUtil Util;
//
//  const char *text = "a\xe4\xb8\xad";
//
//  assert(3 == Util::displayWidth(text));
// ```
// Then, we find the longest prefix of the string that fits within two columns:
// ```
//  std::size_t numCharacters = 0;
//  int         width         = 0;
//
//  Util::findWidthLimitedPrefix(&numCharacters, &width, text, 2);
// ```
// Finally, we observe that only `a` fits, because the CJK character would
// need a third column:
// ```
//  assert(1 == numCharacters);
//  assert(1 == width);
// ```

#include <bslscm_version.h>

#include <bslstl_stringview.h>

#include <cstddef>

namespace BloombergLP {
namespace bslfmt {

                          // =======================
                          // struct DisplayWidthUtil
                          // =======================

/// This `struct` provides a namespace for functions that compute the display
/// width of text.
struct DisplayWidthUtil {
    // CLASS METHODS

    /// Return the display width, in columns, of the specified `text`.  Throw
    /// `bsl::format_error` if `text` is not well-formed Unicode.
    static int displayWidth(const bsl::string_view&  text);
    static int displayWidth(const bsl::wstring_view& text);

    /// Load into the specified `numCharacters` the number of characters in
    /// the longest prefix of the specified `text` whose display width does
    /// not exceed the specified `maxWidth`, and load that width into the
    /// specified `width`.  Throw `bsl::format_error` if ill-formed Unicode is
    /// encountered while searching for the prefix.
    static void findWidthLimitedPrefix(std::size_t              *numCharacters,
                                       int                      *width,
                                       const bsl::string_view&   text,
                                       int                       maxWidth);
    static void findWidthLimitedPrefix(std::size_t              *numCharacters,
                                       int                      *width,
                                       const bsl::wstring_view&  text,
                                       int                       maxWidth);
};

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
