// bslfmt_escapeutil.h                                                -*-C++-*-

#ifndef INCLUDED_BSLFMT_ESCAPEUTIL
#define INCLUDED_BSLFMT_ESCAPEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide escaped ("debug") rendering of characters and strings.
//
//@CLASSES:
//  bslfmt::EscapeUtil: utilities to produce escaped characters and strings
//
//@DESCRIPTION: This component provides an `EscapeUtil` `struct` template that
// is a namespace for utility functions implementing the "escaped" presentation
// of characters and strings -- the presentation selected by the `?` format
// type, and the one that ranges and tuples apply to their elements by default.
// The transformation implemented here is the one specified by the C++ standard
// in [format.string.escaped].
//
// A string is rendered surrounded by double quotes, a character surrounded by
// single quotes.  Within those quotes:
//
// * The characters in the table below are replaced by a two-character escape
//   sequence.
// * A character that is not printable is replaced by `\u{...}`, where the
//   braces surround the hexadecimal value of the code point in lowercase with
//   no leading zeros.  So is a character having the `Grapheme_Extend`
//   property, unless the character immediately preceding it was itself copied
//   through unchanged -- a combining mark that has nothing to combine with is
//   shown rather than applied.
// * Each code unit of an ill-formed code unit sequence is replaced by
//   `\x{...}`, formatted in the same way.
// * Every other character is copied unchanged.
//
// ```
//  Character  Escape sequence
//  ---------  ---------------
//  U+0009     \t
//  U+000A     \n
//  U+000D     \r
//  U+005C     backslash backslash
//  U+0022     \"                   (when escaping a string)
//  U+0027     \'                   (when escaping a character)
// ```
//
// Note that `"` is escaped only when escaping a string, and `'` only when
// escaping a character; each is printable, and therefore copied unchanged, in
// the other context.
//
// This component is private to this package and should not be used by clients.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Escaping A String Containing A Newline
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wish to render a string in a form that shows its control
// characters rather than acting on them.
//
// First, we escape a string containing a newline into a `bsl::string`:
// ```
//  bsl::string result;
//  bslfmt::EscapeUtil<char>::escapeString(bsl::back_inserter(result), "a\nb");
// ```
// Then, we verify that the newline was replaced by its escape sequence and
// that the result is surrounded by double quotes:
// ```
//  assert("\"a\\nb\"" == result);
// ```

#include <bslscm_version.h>

#include <bslfmt_formattercharutil.h>
#include <bslfmt_formatterunicodedata.h>
#include <bslfmt_unicodecodepoint.h>

#include <bslalg_numericformatterutil.h>

#include <bsls_assert.h>

#include <bslstl_algorithm.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <cstddef>  // `std::size_t`

namespace BloombergLP {
namespace bslfmt {

                            // =================
                            // struct EscapeUtil
                            // =================

/// This struct provides a namespace for utility functions that render
/// characters and strings in the escaped form specified by
/// [format.string.escaped].  The (template parameter) `t_CHAR_TRAITS` is the
/// traits type of the string views accepted by `escapeString`.  Notice that
/// this structure is intended to be used only with the `char` and `wchar_t`
/// types.
template <class t_CHAR, class t_CHAR_TRAITS = bsl::char_traits<t_CHAR> >
struct EscapeUtil {
  private:
    // PRIVATE TYPES
    typedef FormatterUnicodeData                          UnicodeData;
    typedef bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS> StringView;

    // PRIVATE CLASS METHODS

    /// Return the UTF encoding used by a sequence of `t_CHAR` code units.
    static UnicodeCodePoint::UtfEncoding encoding();

    /// Return `true` if the specified `codePoint` falls within one of the
    /// specified `count` ranges beginning at the specified `ranges` address,
    /// and `false` otherwise.  The behavior is undefined unless the ranges are
    /// sorted by `d_start` and do not overlap.
    static bool inRanges(const UnicodeData::BooleanRange *ranges,
                         int                              count,
                         unsigned long int                codePoint);

    /// Output to the specified output iterator `out` an escape sequence
    /// introduced by the specified `kind` character (`u` or `x`) and holding
    /// the specified `value` rendered in lowercase hexadecimal with no leading
    /// zeros, surrounded by braces.  Return `out` incremented by the number of
    /// characters written.
    template <class t_ITERATOR>
    static t_ITERATOR outputHexEscape(t_ITERATOR        out,
                                      char              kind,
                                      unsigned long int value);

    /// Output to the specified output iterator `out` the escaped rendering of
    /// the specified `value`, surrounded by single quotes if the specified
    /// `isCharacter` is `true` and by double quotes otherwise.  Return `out`
    /// incremented by the number of characters written.
    template <class t_ITERATOR>
    static t_ITERATOR escapeImpl(t_ITERATOR        out,
                                 const StringView& value,
                                 bool              isCharacter);

  public:
    // CLASS METHODS

    /// Return `true` if the specified `codePoint` has the derived
    /// `Grapheme_Extend` property, and `false` otherwise.
    static bool isGraphemeExtend(unsigned long int codePoint);

    /// Return `true` if the specified `codePoint` is printable -- i.e., if its
    /// Unicode `General_Category` is none of `Cc`, `Cf`, `Cs`, `Co`, `Cn`,
    /// `Zl`, `Zp`, and `Zs`, U+0020 SPACE excepted -- and `false` otherwise.
    static bool isPrintable(unsigned long int codePoint);

    /// Output to the specified output iterator `out` the escaped rendering of
    /// the specified `value`, surrounded by single quotes.  Return `out`
    /// incremented by the number of characters written.
    template <class t_ITERATOR>
    static t_ITERATOR escapeCharacter(t_ITERATOR out, t_CHAR value);

    /// Output to the specified output iterator `out` the escaped rendering of
    /// the specified `value`, surrounded by double quotes.  Return `out`
    /// incremented by the number of characters written.
    template <class t_ITERATOR>
    static t_ITERATOR escapeString(
                   t_ITERATOR                                           out,
                   const bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS>& value);
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                      // ===============================
                      // struct EscapeUtil_EndComparator
                      // ===============================

/// [**PRIVATE**] Component-private comparator class to facilitate range
/// searches using standard algorithms.
struct EscapeUtil_EndComparator {

    // ACCESSORS

    /// Return `true` if the `d_end` member of the specified `range` is less
    /// than the specified `value`, and `false` otherwise.
    bool operator()(const FormatterUnicodeData::BooleanRange& range,
                    const unsigned long int                   value) const
    {
        return range.d_end < value;
    }
};

                            // -----------------
                            // struct EscapeUtil
                            // -----------------

// PRIVATE CLASS METHODS
template <class t_CHAR, class t_CHAR_TRAITS>
inline
UnicodeCodePoint::UtfEncoding EscapeUtil<t_CHAR, t_CHAR_TRAITS>::encoding()
{
    return 1 == sizeof(t_CHAR) ? UnicodeCodePoint::e_UTF8
         : 2 == sizeof(t_CHAR) ? UnicodeCodePoint::e_UTF16
         :                       UnicodeCodePoint::e_UTF32;
}

template <class t_CHAR, class t_CHAR_TRAITS>
inline
bool EscapeUtil<t_CHAR, t_CHAR_TRAITS>::inRanges(
                                  const UnicodeData::BooleanRange *ranges,
                                  int                              count,
                                  unsigned long int                codePoint)
{
    const UnicodeData::BooleanRange *first = ranges;
    const UnicodeData::BooleanRange *last  = ranges + count;

    EscapeUtil_EndComparator comparator;

    const UnicodeData::BooleanRange *found =
                          bsl::lower_bound(first, last, codePoint, comparator);

    return found != last
        && found->d_start <= codePoint
        && codePoint      <= found->d_end;
}

template <class t_CHAR, class t_CHAR_TRAITS>
template <class t_ITERATOR>
t_ITERATOR EscapeUtil<t_CHAR, t_CHAR_TRAITS>::outputHexEscape(
                                                       t_ITERATOR        out,
                                                       char              kind,
                                                       unsigned long int value)
{
    // `\x{`, the hexadecimal digits of the largest `unsigned long int`, and
    // `}`.

    typedef bslalg::NumericFormatterUtil NFU;

    char buffer[3 + NFU::ToCharsMaxLength<unsigned long int, 16>::k_VALUE + 1];

    char *position = buffer;

    *position++ = '\\';
    *position++ = kind;
    *position++ = '{';

    position = NFU::toChars(position, bsl::end(buffer) - 1, value, 16);
    BSLS_ASSERT(position);

    *position++ = '}';

    return FormatterCharUtil<t_CHAR>::outputFromChar(buffer, position, out);
}

template <class t_CHAR, class t_CHAR_TRAITS>
template <class t_ITERATOR>
t_ITERATOR EscapeUtil<t_CHAR, t_CHAR_TRAITS>::escapeImpl(
                                                 t_ITERATOR        out,
                                                 const StringView& value,
                                                 bool              isCharacter)
{
    typedef FormatterCharUtil<t_CHAR> CharUtil;

    const char quote = isCharacter ? '\'' : '"';

    out = CharUtil::outputFromChar(quote, out);

    StringView remaining(value);

    // [format.string.escaped] escapes a `Grapheme_Extend` character unless it
    // is immediately preceded by a character that was itself appended without
    // translation to an escape sequence.  Tracking that is not the same as
    // tracking "is this the first character": in `"\t<combining acute>"` the
    // mark follows an escape sequence, so it too must be escaped.

    bool previousWasVerbatim = false;

    while (!remaining.empty()) {
        UnicodeCodePoint codePoint;
        codePoint.extract(encoding(),
                          remaining.data(),
                          remaining.size() * sizeof(t_CHAR));

        if (!codePoint.isValid()) {
            // Ill-formed input: escape a single code unit and resynchronize on
            // the next one.  The standard escapes every code unit of an
            // ill-formed subsequence individually, so the boundary of the
            // maximal subpart is unobservable and advancing by one unit gives
            // the same result.  Note that the decoder leaves
            // `numSourceBytes()` zero on failure, so this advance must be done
            // by hand.

            const typename CharUtil::UnsignedType unit =
                    static_cast<typename CharUtil::UnsignedType>(
                                                           remaining.front());

            out = outputHexEscape(out, 'x', unit);
            remaining.remove_prefix(1);
            previousWasVerbatim = false;
            continue;                                             // CONTINUE
        }

        const unsigned long int value32  = codePoint.codePointValue();
        const std::size_t       numUnits =
                                codePoint.numSourceBytes() / sizeof(t_CHAR);

        bsl::string_view simpleEscape;
        switch (value32) {
          case '\t': { simpleEscape = "\\t";  } break;
          case '\n': { simpleEscape = "\\n";  } break;
          case '\r': { simpleEscape = "\\r";  } break;
          case '\\': { simpleEscape = "\\\\"; } break;
          case '"': {
            if (!isCharacter) {
                simpleEscape = "\\\"";
            }
          } break;
          case '\'': {
            if (isCharacter) {
                simpleEscape = "\\'";
            }
          } break;
          default: {
          } break;
        }

        if (!simpleEscape.empty()) {
            out = CharUtil::outputFromChar(
                                     simpleEscape.data(),
                                     simpleEscape.data() + simpleEscape.size(),
                                     out);
            previousWasVerbatim = false;
        }
        else if (!isPrintable(value32) ||
                 (isGraphemeExtend(value32) && !previousWasVerbatim)) {
            out = outputHexEscape(out, 'u', value32);
            previousWasVerbatim = false;
        }
        else {
            // Printable: copy the original code units through unchanged.

            out = bsl::copy(remaining.begin(),
                            remaining.begin() + numUnits,
                            out);
            previousWasVerbatim = true;
        }

        remaining.remove_prefix(numUnits);
    }

    return CharUtil::outputFromChar(quote, out);
}

// CLASS METHODS
template <class t_CHAR, class t_CHAR_TRAITS>
inline
bool EscapeUtil<t_CHAR, t_CHAR_TRAITS>::isGraphemeExtend(
                                                  unsigned long int codePoint)
{
    // Early exit for code points below the first `Grapheme_Extend` range,
    // which include all of ASCII.

    if (codePoint < UnicodeData::s_graphemeExtendRanges[0].d_start) {
        return false;                                                 // RETURN
    }

    return inRanges(UnicodeData::s_graphemeExtendRanges,
                    UnicodeData::s_graphemeExtendRangeCount,
                    codePoint);
}

template <class t_CHAR, class t_CHAR_TRAITS>
inline
bool EscapeUtil<t_CHAR, t_CHAR_TRAITS>::isPrintable(
                                                  unsigned long int codePoint)
{
    // Early exit for the common (ASCII) case.  U+0020 through U+007E are
    // printable; everything below U+0020 and U+007F itself are `Cc`.

    if (codePoint < 0x80) {
        return codePoint >= 0x20 && codePoint != 0x7f;                // RETURN
    }

    return inRanges(UnicodeData::s_printableRanges,
                    UnicodeData::s_printableRangeCount,
                    codePoint);
}

template <class t_CHAR, class t_CHAR_TRAITS>
template <class t_ITERATOR>
inline
t_ITERATOR EscapeUtil<t_CHAR, t_CHAR_TRAITS>::escapeCharacter(t_ITERATOR out,
                                                              t_CHAR     value)
{
    return escapeImpl(out, StringView(&value, 1), true);
}

template <class t_CHAR, class t_CHAR_TRAITS>
template <class t_ITERATOR>
inline
t_ITERATOR EscapeUtil<t_CHAR, t_CHAR_TRAITS>::escapeString(
                    t_ITERATOR                                           out,
                    const bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS>& value)
{
    return escapeImpl(out, value, false);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_ESCAPEUTIL

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
