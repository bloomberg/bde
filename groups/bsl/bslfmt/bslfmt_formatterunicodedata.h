// bslfmt_formatterunicodedata.h                                      -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERUNICODEDATA
#define INCLUDED_BSLFMT_FORMATTERUNICODEDATA

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Private unicode data tables for use by `bsl::format`
//
//@CLASSES:
//  FormatterUnicodeData: Namespace struct for generated unicode data tables.
//
//@DESCRIPTION: This component is a namespace struct to provide access to
// script-generated tables of unicode character data as required by
// `bslfmt::format` and associated utilities.
//
// This component is for use within `bslfmt` only.

#include <bslscm_version.h>

namespace BloombergLP {
namespace bslfmt {

                      // ===========================
                      // struct FormatterUnicodeData
                      // ===========================

/// [**PRIVATE**] This private implementation `struct` contains data for
/// correct processing of Unicode characters.  These data are private to this
/// package and should not be used by clients.
struct FormatterUnicodeData {
  public:
    // TYPES

    /// These constants define a general type for a unicode character set.
    /// Although they do not match BDE naming conventions, the names chosen
    /// were those defined by the unicode standard.
    enum GraphemeBreakCategory {
        e_UNASSIGNED         = 0,
        e_PREPEND            = 1,
        e_CR                 = 2,
        e_LF                 = 3,
        e_CONTROL            = 4,
        e_EXTEND             = 5,
        e_REGIONAL_INDICATOR = 6,
        e_SPACING_MARK       = 7,
        e_HANGUL_L           = 8,
        e_HANGUL_V           = 9,
        e_HANGUL_T           = 10,
        e_HANGUL_LV          = 11,
        e_HANGUL_LVT         = 12,
        e_ZERO_WIDTH_JOINER  = 13
    };

    /// This nested 'struct' provides a container for storing a pair of
    /// codepoints that allows to specify a certain range within a list of
    /// codepoints.
    struct BooleanRange {
      public:
        // PUBLIC DATA
        unsigned long int d_start;  // first codepoint in range
        unsigned long int d_end;     // last codepoint in range
    };

    /// This nested 'struct' provides a container for storing a pair of
    /// codepoints that allows to specify a certain range within a list of
    /// codepoints and assign it a category.
    struct GraphemeBreakCategoryRange {
      public:
        // PUBLIC DATA
        unsigned long int     d_start;    // first codepoint in range
        unsigned long int     d_end;      // last codepoint in range
        GraphemeBreakCategory d_category; // category of all points in range
    };

    // PUBLIC CLASS DATA
    static const GraphemeBreakCategoryRange s_graphemeBreakCategoryRanges[];
        // grapheme break category for all unicode code points

    static const BooleanRange               s_extendedPictographicRanges[];
        // all unicode code points with the `Extended_Pictographic` property

    static const BooleanRange               s_printableRanges[];
        // all unicode code points representing printable characters.

    static const BooleanRange               s_graphemeExtendRanges[];
        // all unicode code points with derived `Grapheme_Extend` property.

    static const BooleanRange               s_doubleFieldWidthRanges[];
        // all unicode code points that are double-width per the C++ standard
        // [format.string.std]

    static const int                        s_graphemeBreakCategoryRangeCount;
        // length of `s_graphemeBreakCategoryRanges` array

    static const int                        s_extendedPictographicRangeCount;
        // length of `s_extendedPictographicRanges` array

    static const int                        s_printableRangeCount;
        // length of `s_printableRanges` array

    static const int                        s_graphemeExtendRangeCount;
        // length of `s_graphemeExtendRanges` array

    static const int                        s_doubleFieldWidthRangeCount;
        // length of `s_doubleFieldWidthRanges` array
};

}  // close namespace bslfmt
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMATTERUNICODEDATA

// ----------------------------------------------------------------------------
// The data herein is generated from Unicode Data Files which are
// Copyright 1991-2024 Unicode, Inc. and subject to the
// following license agreement:
//
// UNICODE, INC. LICENSE AGREEMENT V3 - DATA FILES AND SOFTWARE
//
// See Terms of Use <https://www.unicode.org/copyright.html>
// for definitions of Unicode Inc.'s Data Files and Software.
//
// Disclaimer
//
// The Unicode Character Database is provided as is by Unicode, Inc. No
// claims are made as to fitness for any particular purpose. No warranties
// of any kind are expressed or implied. The recipient agrees to determine
// applicability of information provided. If this file has been purchased
// on magnetic or optical media from Unicode, Inc., the sole remedy for any
// claim will be exchange of defective media within 90 days of receipt.
//
// This disclaimer is applicable for all other data files accompanying the
// Unicode Character Database, some of which have been compiled by the
// Unicode Consortium, and some of which have been supplied by other sources.
//
// Limitations on Rights to Redistribute This Data
//
// Recipient is granted the right to make copies in any form for internal
// distribution and to freely use the information supplied in the creation
// of products supporting the UnicodeTM Standard. The files in the Unicode
// Character Database can be redistributed to third parties or other
// organizations (whether for profit or not) as long as this notice and the
// disclaimer notice are retained. Information can be extracted from these
// files and used in documentation or programs, as long as there is an
// accompanying notice indicating the source.
//
// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
