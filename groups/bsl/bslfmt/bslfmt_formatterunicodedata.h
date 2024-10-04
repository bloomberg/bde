// bslfmt_formatterunicodedata.h                                      -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERUNICODEDATA
#define INCLUDED_BSLFMT_FORMATTERUNICODEDATA

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Private unicode data tables for use by `bsl::format`
//
//@CLASSES:
//  Formatter_UnicodeData: Namespace struct for generated unicode data tables.
//
//@DESCRIPTION: This component is a namespace struct to provide access to
// script-generated tables of unicode character data as required by
// `bslfmt::format` and associated utilities.
//
// This component is for use within `bslfmt` only.

#include <bslscm_version.h>

#include <bslalg_numericformatterutil.h>

#include <bslfmt_formaterror.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'


namespace BloombergLP {
namespace bslfmt {

struct Formatter_UnicodeData {
  public:
    // PUBLIC TYPES

    /// These constants define a general type for a unicode character set.
    /// Although they do not match BDE naming conventions, the names chosen
    /// were those defined by the unicode standard.
    enum GraphemeBreakCategory {
        e_UNASSIGNED                      = 0,
        e_PREPEND                         = 1,
        e_CR                              = 2,
        e_LF                              = 3,
        e_CONTROL                         = 4,
        e_EXTEND                          = 5,
        e_REGIONAL_INDICATOR              = 6,
        e_SPACING_MARK                    = 7,
        e_HANGUL_L                        = 8,
        e_HANGUL_V                        = 9,
        e_HANGUL_T                        = 10,
        e_HANGUL_LV                       = 11,
        e_HANGUL_LVT                      = 12,
        e_ZERO_WIDTH_JOINER               = 13
    };

    struct BooleanRange {
      public:
        // PUBLIC TYPES
        unsigned long int d_start; // first codepoint in range
        unsigned long int d_end;   // last codepoint in range
    };

    struct GraphemeBreakCategoryRange {
      public:
        // PUBLIC TYPES
        unsigned long int     d_start;    // first codepoint in range
        unsigned long int     d_end;      // last codepoint in range
        GraphemeBreakCategory d_category; // category of all points in range
    };

    // PUBLIC CLASS DATA

    static const GraphemeBreakCategoryRange
        s_graphemeBreakCategoryRanges[];  // Grapheme break category for all
                                          // unicode code points

    static const BooleanRange
        s_extendedPictographicRanges[];   // All unicode code points with the
                                          // `Extended_Pictographic` property

    static const BooleanRange
        s_printableRanges[];              // All unicode code points
                                          // representing printable characters.

    static const BooleanRange
        s_graphemeExtendRanges[];         // All unicode code points with
                                          // derived `Grapheme_Extend`
                                          // property.

    static const BooleanRange
        s_doubleFieldWidthRanges[];       // All unicode code points that are
                                          // double-width per the C++ standard
                                          // [format.string.std]

    // The following are initialized to the lengths of the above arrays.
    static const int s_graphemeBreakCategoryRangeCount;
    static const int s_extendedPictographicRangeCount;
    static const int s_printableRangeCount;
    static const int s_graphemeExtendRangeCount;
    static const int s_doubleFieldWidthRangeCount;
};


}  // close namespace bslfmt
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMATTERUNICODEDATA

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
