// bdlt_date_specifierformatter.h                                     -*-C++-*-
#ifndef INCLUDED_BDLT_DATE_SPECIFIERFORMATTER
#define INCLUDED_BDLT_DATE_SPECIFIERFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a date specifier formatter for formatting date values.
//
//@CLASSES:
//  bdlt::Date_SpecifierFormatter: date specifier formatter template
//
//@SEE_ALSO: bdlt_date, bdlt_formatter
//
//@DESCRIPTION: This component defines a class template,
// `bdlt::Date_SpecifierFormatter`, that provides formatting functionality for
// date values. The template is parameterized by a character type and provides
// methods to parse format specifiers and format date values accordingly.
//
// The formatter supports various `Date` format specifiers including:
// - Year: 'Y' (4-digit), 'y' (2-digit), 'C' (century)
// - Month: 'm' (numeric), 'b'/'h' (abbreviated name)
// - Day: 'd' (zero-padded), 'e' (space-padded)
// - Day of week: 'a' (abbreviated name), 'u'/'w' (numeric)
// - Day of year: 'j' (numeric)
// - Compound: "{}" or 'D' (default format), 'F'/'i' (ISO 8601 format)

#include <bdlscm_version.h>

#include <bdlt_dayofweek.h>
#include <bdlt_formatutil.h>
#include <bdlt_monthofyear.h>

#include <bslfmt_formatspecificationparser.h>
#include <bslfmt_formattercharutil.h>

#include <bsla_fallthrough.h>

#include <bsls_keyword.h>

#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdlt {

template <class t_DATE>
class Date_SpecifierFormatter_Cache;

                        // =============================
                        // class Date_SpecifierFormatter
                        // =============================

/// This `class` provides a specifier formatter for printing `Date` objects.
template <class t_CHAR>
class Date_SpecifierFormatter {
  private:
    // PRIVATE TYPES
    typedef bslfmt::FormatterCharUtil<t_CHAR>   FormatterCharUtil;
    typedef FormatUtil<t_CHAR>                  Util;
    typedef typename Util::StringView           StringView;

    enum {
        k_DAY_OF_MONTH_WIDTH     = 2,
        k_DAY_OF_YEAR_WIDTH      = 3,
        k_WEEKDAY_INDEX_WIDTH    = 1,
        k_WEEKDAY_NAME_WIDTH     = 3,
        k_MONTH_INDEX_WIDTH      = 2,
        k_MONTH_NAME_WIDTH       = 3,
        k_YEAR_WIDTH             = 4,
        k_ABBREVIATED_YEAR_WIDTH = 2,
        k_CENTURY_WIDTH          = 2,
        k_DASH_WIDTH             = 1 };

    // DATA
    int d_fixedWidth;

    // PRIVATE CLASS METHODS

    /// Write the two-digit, zero-padded day of month from the specified
    /// `value` to the specified `out` and return `out`.
    template <class t_ITERATOR, class t_DATE>
    static
    t_ITERATOR formatDayOfMonth(
                           t_ITERATOR                                   out,
                           const Date_SpecifierFormatter_Cache<t_DATE>& value);

    /// Write the three-letter English abbreviation of the day of week from the
    /// specified `value` to the specified `out` and return `out`.
    template <class t_ITERATOR, class t_DATE>
    static
    t_ITERATOR formatDayOfWeek(
                           t_ITERATOR                                   out,
                           const Date_SpecifierFormatter_Cache<t_DATE>& value);

    /// Write the two-digit, zero-padded month of year from the specified
    /// `value` to the specified `out` and return `out`.
    template <class t_ITERATOR, class t_DATE>
    static
    t_ITERATOR formatMonthIndex(
                           t_ITERATOR                                   out,
                           const Date_SpecifierFormatter_Cache<t_DATE>& value);

    /// Write the three-letter English abbreviation of the month of year from
    /// the specified `value` to the specified `out` and return `out`.
    template <class t_ITERATOR, class t_DATE>
    static
    t_ITERATOR formatMonthOfYear(
                           t_ITERATOR                                   out,
                           const Date_SpecifierFormatter_Cache<t_DATE>& value);

    /// Write the four-digit, zero-padded year from the specified `value` to
    /// the specified `out` and return `out`.
    template <class t_ITERATOR, class t_DATE>
    static
    t_ITERATOR formatYear(t_ITERATOR                                   out,
                          const Date_SpecifierFormatter_Cache<t_DATE>& value);

  public:
    // CREATORS

    /// Create an object in its default initial state.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    Date_SpecifierFormatter();

    // MANIPULATORS

    /// Update this object as-if parsing `Date` in default mode.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseDefault();

    /// Update this object as-if parsing `Date` in Iso8601 mode.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseIso8601();

    /// Examine the first character of the specified `*specInOut` and if it is
    /// recognized as a modifier by this object, update this object's state to
    /// reflect it and pop it off the front of `*specInOut` and return `true`,
    /// and if not, return `false` with no modification to `*specInOut`.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool parseNextModifier(StringView *specInOut);

    /// If the first character of the specified `*specInOut` is recognized by
    /// this specifier formatter, parse it, remove it from `*specInOut`, and
    /// return `true`, otherwise return `false` with no modification to
    /// `*specInOut`.  The behavior is undefined if `*specInOut` is empty.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool parseNextSpecifier(
                                    bsl::basic_string_view<t_CHAR> *specInOut);

    /// Read post-processed fields from the specified `spec` that are relevant
    /// to this specifier formatter.
    void postprocess(const bslfmt::FormatSpecificationParser<t_CHAR>& spec);

    // ACCESSORS

    /// Return the `bslfmt::FormatSpecificationParser::Sections` flags that
    /// apply to this value type.
    BSLS_KEYWORD_CONSTEXPR_CPP20 int extraSections() const;

    /// If the first character of the specified `*specInOut` is recognized by
    /// this specifier formatter, use it to format the specified `value` to
    /// `*outIt`, remove the character from `*specInOut`, and return `true`,
    /// otherwise return `false` with no modification to `*specInOut`.
    template <class t_ITERATOR, class t_DATE>
    bool formatNextSpecifier(
                    bsl::basic_string_view<t_CHAR>               *specInOut,
                    t_ITERATOR                                   *outIt,
                    const Date_SpecifierFormatter_Cache<t_DATE>&  value) const;

    /// Format the specified `value` to the specified `out` using the default
    /// format and return `out`.
    template <class t_ITERATOR, class t_DATE>
    t_ITERATOR formatDefault(
                     t_ITERATOR                                   out,
                     const Date_SpecifierFormatter_Cache<t_DATE>& value) const;

    /// Format the specified `value` to the specified `out` using the Iso8601
    /// format and return `out`.
    template <class t_ITERATOR, class t_DATE>
    t_ITERATOR formatIso8601(
                     t_ITERATOR                                   out,
                     const Date_SpecifierFormatter_Cache<t_DATE>& value) const;

    /// Return the anticipated width of output given all the `parse*` calls
    /// that have been happened thus far and the specified `value`.
    template <class t_DATE>
    BSLS_KEYWORD_CONSTEXPR_CPP20
    int totalWidth(const Date_SpecifierFormatter_Cache<t_DATE>& value) const;
};

                      // ===================================
                      // class Date_SpecifierFormatter_Cache
                      // ===================================

/// This `class` facilitates faster access to a `Date` object during printing
/// by batching access to several fields in a single call during construction
/// and caching them for quick access later.
template <class t_DATE>
class Date_SpecifierFormatter_Cache {
    t_DATE    d_value;
    int       d_year;
    int       d_month;
    int       d_day;

  public:
    // CREATORS

    /// Cache a copy the specified `value` and its `year`, `month`, and `day`
    /// fields.
    Date_SpecifierFormatter_Cache(const t_DATE& value);

    // ACCESSORS

    /// Return the cached `year` field.
    int year() const;

    /// Return the cached `month` field.
    int month() const;

    /// Return the cached `day` field.
    int day() const;

    /// Call the `dayOfWeek` accessor of the cached date and return the value.
    int dayOfWeek() const;

    /// Call the `dayOfYear` accessor of the cached date and return the value.
    int dayOfYear() const;
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                          // -----------------------
                          // Date_SpecifierFormatter
                          // -----------------------

// PRIVATE CLASS METHODS

template <class t_CHAR>
template <class t_ITERATOR, class t_DATE>
inline
t_ITERATOR Date_SpecifierFormatter<t_CHAR>::formatDayOfMonth(
                            t_ITERATOR                                   out,
                            const Date_SpecifierFormatter_Cache<t_DATE>& value)
{
    return Util::writeZeroPaddedDigits(out, value.day(), k_DAY_OF_MONTH_WIDTH);
}

template <class t_CHAR>
template <class t_ITERATOR, class t_DATE>
inline
t_ITERATOR Date_SpecifierFormatter<t_CHAR>::formatDayOfWeek(
                            t_ITERATOR                                   out,
                            const Date_SpecifierFormatter_Cache<t_DATE>& value)
{
    bsl::string_view dayOfWeekName = DayOfWeek::toAscii(
                              static_cast<DayOfWeek::Enum>(value.dayOfWeek()));

    return FormatterCharUtil::outputFromChar(
                                 dayOfWeekName.data(),
                                 dayOfWeekName.data() + dayOfWeekName.length(),
                                 out);
}

template <class t_CHAR>
template <class t_ITERATOR, class t_DATE>
inline
t_ITERATOR Date_SpecifierFormatter<t_CHAR>::formatMonthIndex(
                            t_ITERATOR                                   out,
                            const Date_SpecifierFormatter_Cache<t_DATE>& value)
{
    return Util::writeZeroPaddedDigits(out,
                                       value.month(),
                                       k_MONTH_INDEX_WIDTH);
}

template <class t_CHAR>
template <class t_ITERATOR, class t_DATE>
inline
t_ITERATOR Date_SpecifierFormatter<t_CHAR>::formatMonthOfYear(
                            t_ITERATOR                                   out,
                            const Date_SpecifierFormatter_Cache<t_DATE>& value)
{
    bsl::string_view monthName = MonthOfYear::toAscii(
                                static_cast<MonthOfYear::Enum>(value.month()));

    return FormatterCharUtil::outputFromChar(
                                         monthName.data(),
                                         monthName.data() + monthName.length(),
                                         out);
}

template <class t_CHAR>
template <class t_ITERATOR, class t_DATE>
inline
t_ITERATOR Date_SpecifierFormatter<t_CHAR>::formatYear(
                            t_ITERATOR                                   out,
                            const Date_SpecifierFormatter_Cache<t_DATE>& value)
{
    return Util::writeZeroPaddedDigits(out, value.year(), k_YEAR_WIDTH);
}

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
Date_SpecifierFormatter<t_CHAR>::Date_SpecifierFormatter()
: d_fixedWidth(0)
{}

    // MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void Date_SpecifierFormatter<t_CHAR>::parseDefault()
{
    d_fixedWidth += k_YEAR_WIDTH + k_MONTH_NAME_WIDTH + k_DAY_OF_MONTH_WIDTH;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void Date_SpecifierFormatter<t_CHAR>::parseIso8601()
{
    d_fixedWidth += k_YEAR_WIDTH + k_DASH_WIDTH + k_MONTH_INDEX_WIDTH +
                    k_DASH_WIDTH + k_DAY_OF_MONTH_WIDTH;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool Date_SpecifierFormatter<t_CHAR>::parseNextModifier(StringView *)
{
    return false;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool Date_SpecifierFormatter<t_CHAR>::parseNextSpecifier(
                                     bsl::basic_string_view<t_CHAR> *specInOut)
{
    BSLS_ASSERT(!specInOut->empty());

    switch (static_cast<char>(specInOut->front())) {
      case 'C': {
        d_fixedWidth += k_CENTURY_WIDTH;
      } break;
      case 'y': {
        d_fixedWidth += k_ABBREVIATED_YEAR_WIDTH;
      } break;
      case 'Y': {
        d_fixedWidth += k_YEAR_WIDTH;
      } break;
      case 'm': {
        d_fixedWidth += k_MONTH_INDEX_WIDTH;
      } break;
      case 'b':
      case 'h': {
        d_fixedWidth += k_MONTH_NAME_WIDTH;
      } break;
      case 'd':
      case 'e': {
        d_fixedWidth += k_DAY_OF_MONTH_WIDTH;
      } break;
      case 'a': {
        d_fixedWidth += k_WEEKDAY_NAME_WIDTH;
      } break;
      case 'u':
      case 'w': {
        d_fixedWidth += k_WEEKDAY_INDEX_WIDTH;
      } break;
      case 'j': {
        d_fixedWidth += k_DAY_OF_YEAR_WIDTH;
      } break;
      case 'D': {
        parseDefault();
      } break;
      case 'F': BSLA_FALLTHROUGH;
      case 'i': {
        parseIso8601();
      } break;
      default: {
        return false;                                                 // RETURN
      } break;
    }

    specInOut->remove_prefix(1);

    return true;
}

template <class t_CHAR>
inline
void Date_SpecifierFormatter<t_CHAR>::postprocess(
                              const bslfmt::FormatSpecificationParser<t_CHAR>&)
{}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int Date_SpecifierFormatter<t_CHAR>::extraSections() const {
    return 0;
}

template <class t_CHAR>
template <class t_ITERATOR, class t_DATE>
bool Date_SpecifierFormatter<t_CHAR>::formatNextSpecifier(
                     bsl::basic_string_view<t_CHAR>               *specInOut,
                     t_ITERATOR                                   *outIt,
                     const Date_SpecifierFormatter_Cache<t_DATE>&  value) const
{
    BSLS_ASSERT_SAFE(!specInOut->empty());

    switch (static_cast<char>(specInOut->front())) {
      case 'C': {
        *outIt = Util::writeZeroPaddedDigits(*outIt,
                                             value.year() / 100,
                                             k_CENTURY_WIDTH);
      } break;
      case 'y': {
        *outIt = Util::writeZeroPaddedDigits(*outIt,
                                             value.year() % 100,
                                             k_ABBREVIATED_YEAR_WIDTH);
      } break;
      case 'Y': {
        *outIt = formatYear(*outIt, value);
      } break;
      case 'm': {
        *outIt = formatMonthIndex(*outIt, value);
      } break;
      case 'b':
      case 'h': {
        *outIt = formatMonthOfYear(*outIt, value);
      } break;
      case 'd': {
        *outIt = formatDayOfMonth(*outIt, value);
      } break;
      case 'e': {
        unsigned dom = value.day();
        if (dom < 10) {
            *(*outIt)++ = t_CHAR(' ');
            *(*outIt)++ = t_CHAR('0' + dom);
        }
        else {
            *outIt = Util::writeZeroPaddedDigits(*outIt,
                                                 value.day(),
                                                 k_DAY_OF_MONTH_WIDTH);
        }
      } break;
      case 'a': {
        *outIt = formatDayOfWeek(*outIt, value);
      } break;
      case 'u': {
        // 1 digit day of week, starts with Monday == 1

        unsigned dow = value.dayOfWeek();

        // we have "Sunday is 1", we want "Monday is 1"

        dow         = (dow + 7 - 2) % 7 + 1;  // Monday is 1
        *(*outIt)++ = t_CHAR('0' + dow);
      } break;
      case 'w': {
        unsigned dow = value.dayOfWeek();

        // we have "Sunday is 1", we want "Sunday is 0"

        --dow;  // Sunday is 0
        *(*outIt)++ = t_CHAR('0' + dow);
      } break;
      case 'j': {
        *outIt = Util::writeZeroPaddedDigits(*outIt,
                                             value.dayOfYear(),
                                             k_DAY_OF_YEAR_WIDTH);
      } break;
      case 'D': {
        *outIt = formatDefault(*outIt, value);
      } break;
      case 'F': BSLA_FALLTHROUGH;
      case 'i': {
        *outIt = formatIso8601(*outIt, value);
      } break;
      default: {
        return false;                                                 // RETURN
      } break;
    }

    specInOut->remove_prefix(1);

    return true;
}

template <class t_CHAR>
template <class t_ITERATOR, class t_DATE>
inline
t_ITERATOR Date_SpecifierFormatter<t_CHAR>::formatDefault(
                      t_ITERATOR                                   out,
                      const Date_SpecifierFormatter_Cache<t_DATE>& value) const
{
    out = formatDayOfMonth(out, value);
    out = formatMonthOfYear(out, value);
    out = formatYear(out, value);
    return out;
}

template <class t_CHAR>
template <class t_ITERATOR, class t_DATE>
inline
t_ITERATOR Date_SpecifierFormatter<t_CHAR>::formatIso8601(
                      t_ITERATOR                                   out,
                      const Date_SpecifierFormatter_Cache<t_DATE>& value) const
{
    out    = formatYear(out, value);
    *out++ = t_CHAR('-');
    out    = formatMonthIndex(out, value);
    *out++ = t_CHAR('-');
    out    = formatDayOfMonth(out, value);
    return out;
}

template <class t_CHAR>
template <class t_DATE>
BSLS_KEYWORD_CONSTEXPR_CPP20
int Date_SpecifierFormatter<t_CHAR>::totalWidth(
                            const Date_SpecifierFormatter_Cache<t_DATE>&) const
{
    return d_fixedWidth;
}

                        // ------------------------------
                        // Date_SpecifierFormatter_Cache
                        // -----------------------------

// CREATORS
template <class t_DATE>
inline
Date_SpecifierFormatter_Cache<t_DATE>::Date_SpecifierFormatter_Cache(
                                                           const t_DATE& value)
: d_value(value)
{
    value.getYearMonthDay(&d_year, &d_month, &d_day);
}

// ACCESSORS
template <class t_DATE>
inline
int Date_SpecifierFormatter_Cache<t_DATE>::year() const
{
    return d_year;
}

template <class t_DATE>
inline
int Date_SpecifierFormatter_Cache<t_DATE>::month() const
{
    return d_month;
}

template <class t_DATE>
inline
int Date_SpecifierFormatter_Cache<t_DATE>::day() const
{
    return d_day;
}

template <class t_DATE>
inline
int Date_SpecifierFormatter_Cache<t_DATE>::dayOfWeek() const
{
    return d_value.dayOfWeek();
}

template <class t_DATE>
inline
int Date_SpecifierFormatter_Cache<t_DATE>::dayOfYear() const
{
    return d_value.dayOfYear();
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
