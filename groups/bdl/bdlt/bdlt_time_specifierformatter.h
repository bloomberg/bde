// bdlt_time_specifierformatter.h                                     -*-C++-*-
#ifndef INCLUDED_BDLT_TIME_SPECIFIERFORMATTER
#define INCLUDED_BDLT_TIME_SPECIFIERFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a time specifier formatter
//
//@CLASSES:
//  bdlt::Time_SpecifierFormatter: time formatter for `bslfmt` framework
//
//@SEE_ALSO: bdlt_time, bslfmt_formatter
//
//@DESCRIPTION: This component provides a single class,
// `bdlt::Time_SpecifierFormatter`, that implements time formatting for the
// `bslfmt` framework.  This class is used by `bdlt::Time` and other time-
// related components for consistent time formatting.
//
// The formatter interprets the following modifiers:
// - ',' (comma) - the decimal point when displaying seconds is shown as a
//   comma rather than a period.
//
// The formatter supports various time format specifiers including:
// - Hour: 'H' (2 digit) "00" - "24"
// - Hour: 'I' (2 digit) "01" - "12"
// - AM/PM: 'p' - "AM" or "PM"
// - Minute: 'M' (2 digits) "00" - "59"
// - Second: 'S' (2 digits) "00" - "59"
// - Composite: "{}" or 'T' (time in default format), or 'i' (time in ISO 8601
//   format)

#include <bdlscm_version.h>

#include <bdlt_formatutil.h>

#include <bslfmt_formattercharutil.h>

#include <bsl_iosfwd.h>
#include <bsl_optional.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdlt {

class Time_SpecifierFormatter_Cache;

                        // =============================
                        // class Time_SpecifierFormatter
                        // =============================

/// This `class` provides a specifier formatter for printing `Time` objects.
template <class t_CHAR>
class Time_SpecifierFormatter {
  private:
    // PRIVATE TYPES
    typedef FormatUtil<t_CHAR>              Util;
    typedef bsl::basic_string_view<t_CHAR>  StringView;
    typedef Time_SpecifierFormatter_Cache   FormatCache;

    enum {
        k_HOUR_WIDTH                = 2,
        k_AMPM_WIDTH                = 2,
        k_MINUTE_WIDTH              = 2,
        k_SECOND_WIDTH              = 2,
        k_COLON_WIDTH               = 1,
        k_DEFAULT_PRECISION         = 6,
        k_DEFAULT_ISO8601_PRECISION = 3 };

    // DATA
    int                d_fixedWidth;

    unsigned           d_numDefaultSecondSpecifiers;
    unsigned           d_numIso8601SecondSpecifiers;
        // Number of explicit or implicit seconds specifiers in the format
        // string, for default-style or iso8601-style seconds.

    bsl::optional<int> d_precision;

    bool               d_comma;

    // PRIVATE CLASS METHODS

    /// Write the two-digit, zero-padded hours from the specified `value` to
    /// the specified `out` and return `out`.
    template <class t_ITERATOR>
    static t_ITERATOR formatHours(t_ITERATOR out, const FormatCache& value);

    /// Write the two-digit, zero-padded minutes from the specified `value` to
    /// the specified `out` and return `out`.
    template <class t_ITERATOR>
    static t_ITERATOR formatMinutes(t_ITERATOR out, const FormatCache& value);

    // PRIVATE ACCESSORS

    /// Write the two-digit, zero-padded integral seconds from the specified
    /// `value` to the specified `out`, the write the decimal as either a
    /// period or comma, then the fraction of a second to the specified
    /// `precision` decimal places, then return `out`.
    template <class t_ITERATOR>
    t_ITERATOR formatSeconds(t_ITERATOR         out,
                             int                precision,
                             const FormatCache& value) const;

    /// Format the time from the specified `value` to `out` with the specified
    /// `precision` decimal points representing the fraction of a second, then
    /// return `out`.
    template <class t_ITERATOR>
    t_ITERATOR formatTime(t_ITERATOR         out,
                          int                precision,
                          const FormatCache& value) const;

    /// Return the precision to be used when default formatting a time.  If a
    /// precision has been specified, return that, otherwise return 6.
    int precision() const;

    /// Return the precision to be used when Iso8601 formatting a time.  If a
    /// precision has been specified, return that, otherwise return 3.
    int iso8601Precision() const;

  public:
    // CREATORS

    /// Create an object in its default initial state.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    Time_SpecifierFormatter();

    // MANIPULATORS

    /// Parse a time that will be formatted in default mode.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseDefault();

    /// Parse a time that will be formatted in Iso8601 mode.
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
    BSLS_KEYWORD_CONSTEXPR_CPP20
    bool parseNextSpecifier(StringView *specInOut);

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
    template <class t_ITERATOR>
    bool formatNextSpecifier(StringView         *specInOut,
                             t_ITERATOR         *outIt,
                             const FormatCache&  value) const;

    /// Format the specified `value` to the specified `out` using the default
    /// format and return `out`.
    template <class t_ITERATOR>
    t_ITERATOR formatDefault(t_ITERATOR out, const FormatCache& value) const;

    /// Format the specified `value` to the specified `out` using the Iso8601
    /// format and return `out`.
    template <class t_ITERATOR>
    t_ITERATOR formatIso8601(t_ITERATOR out, const FormatCache& value) const;

    /// Return the anticipated width of output given all the `parse*` calls
    /// that have been happened thus far and the specified `value`.
    int totalWidth(const FormatCache& value) const;
};

                      // ===================================
                      // class Time_SpecifierFormatter_Cache
                      // ===================================

/// This `class` facilitates faster access to a `Time` object during printing
/// by batching access to several fields in a single call during construction
/// and caching them for quick access later.
class Time_SpecifierFormatter_Cache {
    // DATA
    int d_hour;
    int d_minute;
    int d_second;
    int d_millisecond;
    int d_microsecond;

  public:
    // CREATORS

    /// Populate all the data members of this `class` with a call to `getTime`
    /// on the specified `value`.
    template <class t_TIME_TYPE>
    explicit Time_SpecifierFormatter_Cache(const t_TIME_TYPE& value);

    // ACCESSORS

    /// Return the cached `hour` field.
    int hour() const;

    /// Return the cached `minute` field.
    int minute() const;

    /// Return the cached `second` field.
    int second() const;

    /// Return the cached `millisecone` field.
    int millisecond() const;

    /// Return the cached `microsecond` field.
    int microsecond() const;
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------------------
                        // class Time_SpecifierFormatter
                        // -----------------------------

// PRIVATE CLASS METHODSS
template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR Time_SpecifierFormatter<t_CHAR>::formatHours(
                                      t_ITERATOR out, const FormatCache& value)
{
    return Util::writeZeroPaddedDigits(out, value.hour(), k_HOUR_WIDTH);
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR Time_SpecifierFormatter<t_CHAR>::formatMinutes(
                                      t_ITERATOR out, const FormatCache& value)
{
    return Util::writeZeroPaddedDigits(out, value.minute(), k_MINUTE_WIDTH);
}

// PRIVATE ACCESSORS

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR Time_SpecifierFormatter<t_CHAR>::formatSeconds(
                 t_ITERATOR out, int precision, const FormatCache& value) const
{
    out = Util::writeZeroPaddedDigits(out, value.second(), k_SECOND_WIDTH);
    out = Util::writeSecondFraction(out,
                                    precision,
                                    d_comma ? ',' : '.',
                                    value.millisecond(),
                                    value.microsecond());
    return out;
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR Time_SpecifierFormatter<t_CHAR>::formatTime(
                                                t_ITERATOR     out,
                                                int            precision,
                                                const FormatCache& value) const
{
    out    = formatHours(out, value);
    *out++ = t_CHAR(':');
    out    = formatMinutes(out, value);
    *out++ = t_CHAR(':');
    out    = formatSeconds(out, precision, value);

    return out;
}

template <class t_CHAR>
inline
int Time_SpecifierFormatter<t_CHAR>::precision() const
{
    return d_precision.value_or(6);
}

template <class t_CHAR>
inline
int Time_SpecifierFormatter<t_CHAR>::iso8601Precision() const
{
    return d_precision.value_or(3);
}

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
Time_SpecifierFormatter<t_CHAR>::Time_SpecifierFormatter()
: d_fixedWidth(0)
, d_numDefaultSecondSpecifiers(0)
, d_numIso8601SecondSpecifiers(0)
, d_precision()
, d_comma(false)
{}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void Time_SpecifierFormatter<t_CHAR>::parseDefault()
{
    ++d_numDefaultSecondSpecifiers;
    d_fixedWidth += k_HOUR_WIDTH + k_COLON_WIDTH + k_MINUTE_WIDTH +
                    k_COLON_WIDTH + k_SECOND_WIDTH;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void Time_SpecifierFormatter<t_CHAR>::parseIso8601()
{
    ++d_numIso8601SecondSpecifiers;
    d_fixedWidth += k_HOUR_WIDTH + k_COLON_WIDTH + k_MINUTE_WIDTH +
                    k_COLON_WIDTH + k_SECOND_WIDTH;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool Time_SpecifierFormatter<t_CHAR>::parseNextModifier(StringView *specInOut)
{
    if (t_CHAR(',') == specInOut->front()) {
        d_comma = true;
        specInOut->remove_prefix(1);
        return true;                                                  // RETURN
    }

    return false;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool Time_SpecifierFormatter<t_CHAR>::parseNextSpecifier(StringView *specInOut)
{
    BSLS_ASSERT(!specInOut->empty());

    switch(static_cast<char>(specInOut->front())) {
      case 'H':
      case 'I': {
        d_fixedWidth += k_HOUR_WIDTH;
      } break;
      case 'p': {
        d_fixedWidth += k_AMPM_WIDTH;
      } break;
      case 'M': {
        d_fixedWidth += k_MINUTE_WIDTH;
      } break;
      case 'S': {
        d_fixedWidth += k_SECOND_WIDTH;
        ++d_numDefaultSecondSpecifiers;
      } break;
      case 'R': {
        d_fixedWidth += k_HOUR_WIDTH + k_COLON_WIDTH + k_MINUTE_WIDTH;
      } break;
      case 'T': {
        parseDefault();
      } break;
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
void Time_SpecifierFormatter<t_CHAR>::postprocess(
                         const bslfmt::FormatSpecificationParser<t_CHAR>& spec)
{
    if (spec.postprocessedPrecision().category() ==
        bslfmt::FormatterSpecificationNumericValue::e_VALUE) {
        d_precision = spec.postprocessedPrecision().value();
    }
}

    // ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int Time_SpecifierFormatter<t_CHAR>::extraSections() const
{
    return bslfmt::FormatSpecificationParserEnums::e_SECTIONS_PRECISION;
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
bool Time_SpecifierFormatter<t_CHAR>::formatNextSpecifier(
                                               StringView         *specInOut,
                                               t_ITERATOR         *outIt,
                                               const FormatCache&  value) const
{
    BSLS_ASSERT_SAFE(!specInOut->empty());

    StringView spec = *specInOut;

    switch (static_cast<char>(spec.front())) {
      case 'H': {
        *outIt = formatHours(*outIt, value);
      } break;
      case 'I': {
        // hour - 01 - 12

        int hh = value.hour();
        hh %= 12;  // time is '00'-'11'
        if (00 == hh) {
            hh = 12;
        }
        *outIt = Util::writeZeroPaddedDigits(*outIt, hh);
      } break;
      case 'p': {
        bsl::string_view ampm = value.hour() < 12 ? "AM" : "PM";
        *outIt = bslfmt::FormatterCharUtil<t_CHAR>::outputFromChar(
                                               ampm.data(),
                                               ampm.data() + ampm.length(),
                                               *outIt);
      } break;
      case 'M': {
        *outIt = formatMinutes(*outIt, value);
      } break;
      case 'S': {
        *outIt = formatSeconds(*outIt, precision(), value);
      } break;
      case 'T': {
        *outIt = formatDefault(*outIt, value);
      } break;
      case 'i': {
        *outIt = formatIso8601(*outIt, value);
      } break;
      default: {
        return false;                                                 // RETURN
      } break;
    }

    *specInOut = spec.substr(1);

    return true;
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR Time_SpecifierFormatter<t_CHAR>::formatDefault(
                                t_ITERATOR out, const FormatCache& value) const
{
    return formatTime(out, precision(), value);
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR Time_SpecifierFormatter<t_CHAR>::formatIso8601(
                                t_ITERATOR out, const FormatCache& value) const
{
    return formatTime(out, iso8601Precision(), value);
}

template <class t_CHAR>
inline
int Time_SpecifierFormatter<t_CHAR>::totalWidth(const FormatCache&) const
{
    int ret = d_fixedWidth;

    if (d_precision) {
        if (0 < *d_precision) {
            ret += (*d_precision + 1) * (d_numDefaultSecondSpecifiers +
                                                 d_numIso8601SecondSpecifiers);
        }
    }
    else {
        ret += (k_DEFAULT_PRECISION + 1)       * d_numDefaultSecondSpecifiers;
        ret += (k_DEFAULT_ISO8601_PRECISION+1) * d_numIso8601SecondSpecifiers;
    }

    return ret;
}

                    // -----------------------------------
                    // class Time_SpecifierFormatter_Cache
                    // -----------------------------------

// CREATORS
template <class t_TIME_TYPE>
inline
Time_SpecifierFormatter_Cache::Time_SpecifierFormatter_Cache(
                                                      const t_TIME_TYPE& value)
{
    value.getTime(&d_hour,
                  &d_minute,
                  &d_second,
                  &d_millisecond,
                  &d_microsecond);
}

// ACCESSORS
inline
int Time_SpecifierFormatter_Cache::hour() const
{
    return d_hour;
}

inline
int Time_SpecifierFormatter_Cache::minute() const
{
    return d_minute;
}

inline
int Time_SpecifierFormatter_Cache::second() const
{
    return d_second;
}

inline
int Time_SpecifierFormatter_Cache::millisecond() const
{
    return d_millisecond;
}

inline
int Time_SpecifierFormatter_Cache::microsecond() const
{
    return d_microsecond;
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
