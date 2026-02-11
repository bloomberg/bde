// bdlt_datetz_specifierformatter.h                                   -*-C++-*-
#ifndef INCLUDED_BDLT_DATETZ_SPECIFIERFORMATTER
#define INCLUDED_BDLT_DATETZ_SPECIFIERFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a date-with-timezone specifier formatter.
//
//@CLASSES:
//  bdlt::DateTz_SpecifierFormatter: datetz formatter for `bslfmt` framework
//
//@SEE_ALSO: bdlt_datetz, bdlt_date_specifierformatter, bslfmt_formatter
//
//@DESCRIPTION: This component provides a single class,
// `bdlt::DateTz_SpecifierFormatter`, that implements date-with-timezone
// formatting for the `bslfmt` framework.  This class is used by
// `bdlt::DateTz` and other date-with-timezone-related components for
// consistent formatting.
//
// The formatter interprets the following modifiers:
// - 'Z' - if the offset is zero, output the time zone as 'Z', otherwise output
//   it normally.
// - ':' (colon) - always print a colon between hours and minutes
// - '_' (underscore) - never print a colon between hours and minutes
// Note that it is an error for both ':' and '_' to be specified at the same
// time.
//
// The formatter supports various `Date` format specifiers including:
// - Year: 'Y' (4-digit), 'y' (2-digit), 'C' (century)
// - Month: 'm' (numeric), 'b'/'h' (abbreviated name)
// - Day: 'd' (zero-padded), 'e' (space-padded)
// - Day of week: 'a' (abbreviated name), 'u'/'w' (numeric)
// - Day of year: 'j' (numeric)
// all of the above will yield results for `bdlt::DateTz::localDate()`.
// - Time Zone: 'z' (2 digit hour, 2 digit minute)
// - Compound: 'D' (default local date format), 'F' (ISO 8601 local date
//   format) "{}" (default local date format with time zone), 'i' (ISO 8601
//   local date format with time zone).

#include <bdlscm_version.h>

#include <bdlt_date.h>
#include <bdlt_date_specifierformatter.h>
#include <bdlt_timezone_specifierformatter.h>

#include <bsl_iosfwd.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdlt {

class DateTz_SpecifierFormatter_Cache;

                      // ===============================
                      // class DateTz_SpecifierFormatter
                      // ===============================

/// This `class` provides a specifier formatter for printing `DateTz` objects.
template <class t_CHAR>
class DateTz_SpecifierFormatter {
    typedef Date_SpecifierFormatter<t_CHAR>       DateFormatter;
    typedef TimeZone_SpecifierFormatter<t_CHAR>   TimeZoneFormatter;
    typedef bsl::basic_string_view<t_CHAR>        StringView;
    typedef DateTz_SpecifierFormatter_Cache       FormatCache;

    // DATA
    DateFormatter      d_dateFormatter;
        // Formatter for date specifiers.

    TimeZoneFormatter  d_timeZoneFormatter;
        // Formatter for time zone specifiers.

  public:
    // CREATORS

    /// Create an object in its default initial state.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    DateTz_SpecifierFormatter();

    // MANIPULATORS

    /// Parse a date tz that will be formatted in default mode.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseDefault();

    /// Parse a date tz that will be formatted in Iso8601 mode.
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

    /// Return the anticipated width of output given all the `parse*` calls
    /// that have been happened thus far and the specified `value`.
    int totalWidth(const FormatCache& value) const;

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
};

                      // =====================================
                      // class DateTz_SpecifierFormatter_Cache
                      // =====================================

/// This `class` facilitates faster access to a `DateTz` object during printing
/// by batching access to several fields in a single call during construction
/// and caching them for quick access later.
class DateTz_SpecifierFormatter_Cache {
    // DATA
    Date_SpecifierFormatter_Cache<Date>   d_dateFormatCache;
    int                                   d_offset;

  public:
    // CREATORS

    /// Cache a copy of the specified `date` and `offset`.
    DateTz_SpecifierFormatter_Cache(const Date& date, int offset);

    // ACCESSORS

    /// Return the `Date_SpecifierFormatter_Cache` held by this object
    const Date_SpecifierFormatter_Cache<Date>& date() const;

    /// Return the offset.
    int offset() const;
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // -------------------------------
                      // class DateTz_SpecifierFormatter
                      // -------------------------------

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
DateTz_SpecifierFormatter<t_CHAR>::DateTz_SpecifierFormatter()
: d_dateFormatter()
, d_timeZoneFormatter()
{}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void DateTz_SpecifierFormatter<t_CHAR>::parseDefault()
{
    d_dateFormatter.    parseDefault();
    d_timeZoneFormatter.parseDefault();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void DateTz_SpecifierFormatter<t_CHAR>::parseIso8601()
{
    d_dateFormatter.    parseIso8601();
    d_timeZoneFormatter.parseIso8601();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool DateTz_SpecifierFormatter<t_CHAR>::parseNextModifier(
                                                         StringView *specInOut)
{
    return d_dateFormatter.    parseNextModifier(specInOut) ||
           d_timeZoneFormatter.parseNextModifier(specInOut);
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool DateTz_SpecifierFormatter<t_CHAR>::parseNextSpecifier(
                                                         StringView *specInOut)
{
    BSLS_ASSERT(!specInOut->empty());

    if (t_CHAR('i') == specInOut->front()) {
        // Intercept iso format

        d_dateFormatter.    parseIso8601();
        d_timeZoneFormatter.parseIso8601();
        specInOut->remove_prefix(1);

        return true;                                                  // RETURN
    }

    return d_dateFormatter.    parseNextSpecifier(specInOut) ||
           d_timeZoneFormatter.parseNextSpecifier(specInOut);         // RETURN
}

template <class t_CHAR>
inline
void DateTz_SpecifierFormatter<t_CHAR>::postprocess(
                         const bslfmt::FormatSpecificationParser<t_CHAR>& spec)
{
    d_dateFormatter.    postprocess(spec);
    d_timeZoneFormatter.postprocess(spec);
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int DateTz_SpecifierFormatter<t_CHAR>::extraSections() const
{
    return d_dateFormatter.extraSections() |
                                           d_timeZoneFormatter.extraSections();
}

template <class t_CHAR>
inline
int DateTz_SpecifierFormatter<t_CHAR>::totalWidth(
                                                const FormatCache& value) const
{
    return d_dateFormatter.    totalWidth(value.date()) +
           d_timeZoneFormatter.totalWidth(value.offset());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
bool DateTz_SpecifierFormatter<t_CHAR>::formatNextSpecifier(
                                               StringView         *specInOut,
                                               t_ITERATOR         *outIt,
                                               const FormatCache&  value) const
{
    BSLS_ASSERT_SAFE(!specInOut->empty());

    if (t_CHAR('i') == specInOut->front()) {
        *outIt = formatIso8601(*outIt, value);
        specInOut->remove_prefix(1);
        return true;                                                  // RETURN
    }

    return d_dateFormatter.    formatNextSpecifier(specInOut,
                                                   outIt,
                                                   value.date()) ||
           d_timeZoneFormatter.formatNextSpecifier(specInOut,
                                                   outIt,
                                                   value.offset());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR DateTz_SpecifierFormatter<t_CHAR>::formatDefault(
                                t_ITERATOR out, const FormatCache& value) const
{
    out = d_dateFormatter.    formatDefault(out, value.date());
    out = d_timeZoneFormatter.formatDefault(out, value.offset());

    return out;
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR DateTz_SpecifierFormatter<t_CHAR>::formatIso8601(
                                t_ITERATOR out, const FormatCache& value) const
{
    out = d_dateFormatter.    formatIso8601(out, value.date());
    out = d_timeZoneFormatter.formatIso8601(out, value.offset());

    return out;
}

                        // -------------------------------
                        // DateTz_SpecifierFormatter_Cache
                        // -------------------------------

// CREATORS
inline
DateTz_SpecifierFormatter_Cache::DateTz_SpecifierFormatter_Cache(
                                                  const Date& date, int offset)
: d_dateFormatCache(date)
, d_offset(offset)
{}

// ACCESSORS
inline
const Date_SpecifierFormatter_Cache<Date>&
                                  DateTz_SpecifierFormatter_Cache::date() const
{
    return d_dateFormatCache;
}

inline
int DateTz_SpecifierFormatter_Cache::offset() const
{
    return d_offset;
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
