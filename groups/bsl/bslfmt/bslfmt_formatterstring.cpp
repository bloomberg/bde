// bslfmt_formatterstring.cpp                                         -*-C++-*-

#include <bslfmt_formatterstring.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslfmt_formatterstring_cpp, "$Id$ $CSID$")

#include <bslfmt_formatterunicodedata.h>

namespace BloombergLP {

namespace {

typedef bslfmt::FormatterUnicodeData            UnicodeData;
typedef UnicodeData::GraphemeBreakCategoryRange GraphemeBreakCategoryRange;


                      // ====================================
                      // struct EmojiModifierSequenceDetector
                      // ====================================

/// A component-private state machine required to detect emoji modifier
/// sequences per https://www.unicode.org/reports/tr29/#GB11
class EmojiModifierSequenceDetector
{
  private:
    // PRIVATE TYPES
    enum State {
        e_START,   // startup state

        e_EXT_PIC  // state indicating `match` has been called for a single
                   // Extended_Pictogram and zero or more Extend characters.
    };

    // DATA
    State d_state;  // the current state.

    // NOT IMPLEMENTED
    bool operator==(const EmojiModifierSequenceDetector&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS

    /// Create an object in its initial state.
    EmojiModifierSequenceDetector();

    // MANIPULATORS

    /// Return true for the ZWJ codePoint (determined by the specified
    /// `leftGbp` being `e_ZERO_WIDTH_JOINER`) and the function was previously
    /// called for an Extended_Pictogram codePoint (determined by the specified
    /// `leftEpv` being true) followed by zero or more Extend codePoints
    /// (determined by the specified `leftGbp` being `e_EXTEND`).  Update the
    /// internal state to enable correct calculations on subsequent calls.
    bool match(const UnicodeData::GraphemeBreakCategory leftGbp,
               bool                                     leftEpv);
};

                      // ====================
                      // struct EndComparator
                      // ====================

/// Component-private comparator class to facilitate range searches using
/// standard algorithms.
template <class t_RANGE_TYPE>
struct EndComparator
{
    // MANIPULATORS

    /// Return true if the `d_end` member of the specified `range` is less
    /// than the specified `value`, false otherwise.
    bool operator()(const t_RANGE_TYPE& range, const unsigned long int value);
};

// ============================================================================
//                        STATIC HELPER FUNCTIONS
// ============================================================================


/// Find and return the Unicode Grapheme Break category for the specified
/// `codePoint` if one exists, otherwise return `e_UNASSIGNED`.
static UnicodeData::GraphemeBreakCategory getGraphemeBreakCategory(
                                                  unsigned long int codePoint);

/// Return `true` if the specified `codePoint` is an extended pictogram, and
/// `false` otherwise.
static bool getExtendedPictogramValue(unsigned long int codePoint);

                      // --------------------
                      // struct EndComparator
                      // --------------------

template <class t_RANGE_TYPE>
bool EndComparator<t_RANGE_TYPE>::operator()(const t_RANGE_TYPE&     range,
                                             const unsigned long int value)
{
    return range.d_end < value;
}

                  // ------------------------------------
                  // struct EmojiModifierSequenceDetector
                  // ------------------------------------

inline
EmojiModifierSequenceDetector::EmojiModifierSequenceDetector()
: d_state(e_START)
{
}

inline
bool EmojiModifierSequenceDetector::match(
                              const UnicodeData::GraphemeBreakCategory leftGbp,
                              bool                                     leftEpv)
{
    bool returnValue = false;
    switch (d_state) {
      case e_START: {
        if (leftEpv) {
            d_state = e_EXT_PIC;
        }
      } break;
      case e_EXT_PIC: {
        if (leftGbp == UnicodeData::e_ZERO_WIDTH_JOINER) {
            d_state = e_START;
            returnValue = true;
        }
        else if (leftGbp != UnicodeData::e_EXTEND) {
            d_state = e_START;
        }
      } break;
      default: {
        BSLS_ASSERT(false && "Unexpected detector state");
      }
    }

    return returnValue;
}

// ----------------------------------------------------------------------------
//                         STATIC HELPER FUNCTIONS
// ----------------------------------------------------------------------------

inline
UnicodeData::GraphemeBreakCategory getGraphemeBreakCategory(
                                                   unsigned long int codePoint)
{
    // Early exit for the common (ascii) case
    if (codePoint <= 0xff) {
        if (codePoint <= 0x09) {
            return UnicodeData::e_CONTROL;                            // RETURN
        }
        if (codePoint == 0x0a) {
            return UnicodeData::e_LF;                                 // RETURN
        }
        if (codePoint == 0x0b || codePoint == 0x0c) {
            return UnicodeData::e_CONTROL;                            // RETURN
        }
        if (codePoint == 0x0d) {
            return UnicodeData::e_CR;                                 // RETURN
        }
        if (codePoint >= 0x0e && codePoint <= 0x1f) {
            return UnicodeData::e_CONTROL;                            // RETURN
        }
        if (codePoint >= 0x7f && codePoint <= 0x9f) {
            return UnicodeData::e_CONTROL;                            // RETURN
        }
        if (codePoint == 0xad) {
            return UnicodeData::e_CONTROL;                            // RETURN
        }

        return UnicodeData::e_UNASSIGNED;                             // RETURN
    }

    const GraphemeBreakCategoryRange *first =
                                    UnicodeData::s_graphemeBreakCategoryRanges;
    const GraphemeBreakCategoryRange *last =
                                UnicodeData::s_graphemeBreakCategoryRanges +
                                UnicodeData::s_graphemeBreakCategoryRangeCount;

    EndComparator<GraphemeBreakCategoryRange> comparator;

    const GraphemeBreakCategoryRange *found =
                          bsl::lower_bound(first, last, codePoint, comparator);

    // Below the first element in the array.
    if ((found == last) ||
        (found->d_start > codePoint) ||
        (found->d_end   < codePoint)) {
        return UnicodeData::e_UNASSIGNED;              // RETURN
    }

    return found->d_category;
}

inline
bool getExtendedPictogramValue(unsigned long int codePoint)
{
    const UnicodeData::BooleanRange *first =
                                     UnicodeData::s_extendedPictographicRanges;

    // Early exit for the common (ascii) case
    if (codePoint < first->d_start) {
        return true;                                                  // RETURN
    }

    const UnicodeData::BooleanRange *last =
                                 UnicodeData::s_extendedPictographicRanges +
                                 UnicodeData::s_extendedPictographicRangeCount;

    EndComparator<UnicodeData::BooleanRange> comparator;

    const UnicodeData::BooleanRange *found =
                          bsl::lower_bound(first, last, codePoint, comparator);

    // Below the first element in the array
    if ((found == last) ||
        (found->d_start > codePoint) ||
        (found->d_end   < codePoint)) {
        return false;                                                 // RETURN
    }

    return true;
}

}  // close unnamed namespace

namespace bslfmt {

                    // -------------------------------------
                    // class FormatterString_GraphemeCluster
                    // -------------------------------------

// MANIPULATORS
void FormatterString_GraphemeCluster::extract(
                                       UnicodeCodePoint::UtfEncoding  encoding,
                                       const void                    *bytes,
                                       size_t                         maxBytes)
{
    UnicodeCodePoint  codePoint;
    codePoint.extract(encoding, bytes, maxBytes);

    bool isValid       = codePoint.isValid();
    int  numBytes      = codePoint.numSourceBytes();
    int  numCodepoints = 1;

    // Failed to extract first code point.
    if (!isValid) {
        return;                                                       // RETURN
    }

    d_firstCodePointValue = codePoint.codePointValue();
    d_firstCodePointWidth = codePoint.codePointWidth();

    UnicodeData::GraphemeBreakCategory leftGbc =
                          getGraphemeBreakCategory(codePoint.codePointValue());
    bool leftEpv = getExtendedPictogramValue(codePoint.codePointValue());

    UnicodeData::GraphemeBreakCategory rightGbc = UnicodeData::e_UNASSIGNED;
    bool                               rightEpv = false;
    size_t                             numRIs   = 0;

    EmojiModifierSequenceDetector emsMatcher;

    for (;; leftGbc = rightGbc, leftEpv = rightEpv) {

        d_isValid        = isValid;
        d_numCodePoints  = numCodepoints;
        d_numSourceBytes = numBytes;

        if (0 == maxBytes - numBytes) {
            // GB2 Any % eot
            return;                                                   // RETURN
        }

        const void *cp = static_cast<const void *>(
                                  static_cast<const char *>(bytes) + numBytes);
        codePoint.reset();
        codePoint.extract(encoding, cp, maxBytes - numBytes);

        isValid = isValid && codePoint.isValid();
        numBytes += codePoint.numSourceBytes();
        numCodepoints++;

        // Failed to extract valid code point.
        if (!isValid) {
            reset();
            return;                                                   // RETURN
        }

        rightGbc = getGraphemeBreakCategory(codePoint.codePointValue());
        rightEpv = getExtendedPictogramValue(codePoint.codePointValue());

        // Match GB11 now, so that we're sure to update it for every character,
        // not just ones where the GB11 rule is considered
        const bool isGB11Match = emsMatcher.match(leftGbc, leftEpv);

        // Also update the number of sequential RIs immediately
        if (leftGbc == UnicodeData::e_REGIONAL_INDICATOR) {
            ++numRIs;
        }
        else {
            numRIs = 0;
        }

        if (leftGbc == UnicodeData::e_CR && rightGbc == UnicodeData::e_LF) {
            // GB3 CR x LF
            continue;                                               // CONTINUE
        }

        if (leftGbc == UnicodeData::e_CONTROL ||
            leftGbc == UnicodeData::e_CR || leftGbc == UnicodeData::e_LF) {
            // GB4 (Control | CR | LF) % Any
            return;                                                   // RETURN
        }

        if (rightGbc == UnicodeData::e_CONTROL ||
            rightGbc == UnicodeData::e_CR || rightGbc == UnicodeData::e_LF) {
            // GB5 Any % (Control | CR | LF)
            return;                                                   // RETURN
        }

        if ((leftGbc == UnicodeData::e_HANGUL_L) &&
            (rightGbc == UnicodeData::e_HANGUL_L ||
             rightGbc == UnicodeData::e_HANGUL_V ||
             rightGbc == UnicodeData::e_HANGUL_LV ||
             rightGbc == UnicodeData::e_HANGUL_LVT)) {
            // GB6 L x (L | V | LV | LVT)
            continue;                                               // CONTINUE
        }

        if ((leftGbc == UnicodeData::e_HANGUL_LV ||
             leftGbc == UnicodeData::e_HANGUL_V) &&
            (rightGbc == UnicodeData::e_HANGUL_V ||
             rightGbc == UnicodeData::e_HANGUL_T)) {
            // GB7 (LV | V) x (V | T)
            continue;                                               // CONTINUE
        }

        if ((leftGbc == UnicodeData::e_HANGUL_LVT ||
             leftGbc == UnicodeData::e_HANGUL_T) &&
            (rightGbc == UnicodeData::e_HANGUL_T)) {
            // GB8 (LVT | T) x T
            continue;                                               // CONTINUE
        }

        if (rightGbc == UnicodeData::e_EXTEND ||
            rightGbc == UnicodeData::e_ZERO_WIDTH_JOINER) {
            // GB9 x (Extend | ZWJ)
            continue;                                               // CONTINUE
        }

        if (rightGbc == UnicodeData::e_SPACING_MARK) {
            // GB9a x SpacingMark
            continue;                                               // CONTINUE
        }

        if (leftGbc == UnicodeData::e_PREPEND) {
            // GB9b Prepend x
            continue;                                               // CONTINUE
        }

        if (isGB11Match && rightEpv) {
            // GB11 \p{ExtendedPictographic} Extend* ZWJ x
            // \p{ExtendedPictographic}
            continue;                                               // CONTINUE
        }

        if (leftGbc == UnicodeData::e_REGIONAL_INDICATOR &&
            rightGbc == UnicodeData::e_REGIONAL_INDICATOR && numRIs % 2 != 0) {
            // GB12 and 13, do not break between RIs if there are an odd number
            // of RIs before the breakpoint
            continue;                                               // CONTINUE
        }

        // No rule will cause `right` to extend the cluster, so return what we
        // have.
        return;                                                       // RETURN
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
//
// The implementation of classes `EmojiModifierSequenceDetector` and
// `FormatterString_GraphemeCluster` have been adapted for BDE use from
// Microsoft MSVC library code, 2024
// 'https://github.com/microsoft/STL/blob/vs-2022-17.11/stl/inc/format'
//
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// NOTE:
// The Microsoft `format` header was itself derived in part from `libfmt` under
// the following license:
//
// Copyright (c) 2012 - present, Victor Zverovich
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// --- Optional exception to the license ---
//
// As an exception, if, as a result of your compiling your source code,
// portions of this Software are embedded into a machine-executable object form
// of such source code, you may redistribute such embedded portions in such
// object form without including the above copyright and permission notices.
//
// ----------------------------------------------------------------------------
//
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
