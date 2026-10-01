// bdlt_datetzformatter.h                                   -*-C++-*-
#ifndef INCLUDED_BDLT_DATETZFORMATTER
#define INCLUDED_BDLT_DATETZFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide `bsl::formatter` specialization for `bdlt::DateTz`.
//
//@CLASSES:
//  bdlt::DateTzFormatter: datetz formatter for `bslfmt` framework
//  bsl::formatter<bdlt::DateTz, t_CHAR>: specialization
//
//@SEE_ALSO: bdlt_datetz, bdlt_dateformatter, bdlt_formatdoc, bslfmt_formatter
//
//@DESCRIPTION: This component provides `bdlt::DateTzFormatter` and a
// specialization of `bsl::formatter` that allow `bsl::format` to output
// values of `bdlt::DateTz`.
//
// The formatter interprets the following modifiers:
// * `:` separate hours and minutes in time zone with `:`
// * `_` suppress `:` between hours and minutes in time zone
// * `Z` if time zone offset is 0, display it as `Z`
// Note that it is an error for both `:` and `_` to be specified at the same
// time.
//
// The formatter supports the following `%`-sequences:
// * Year: `"%Y"` (4-digit), `"%y"` (2-digit)
// * Century: `"%C"` (2-digit)
// * Numeric month: `"%m"` (2-digit)
// * Abbreviation of month: (always 3-letter upper case): `"%b"` or `"%h"`
// * Day of month: (always 2-digit) `"%d"` (`0`-padded), `"%e"` (space-padded)
// * Abbreviated day of week: `"%a"` (3 letter upper case)
// * Day of week: `"%u"` (numeric 1-7, Monday is 1)
// * Day of week: `"%w"` (numeric 0-6, Sunday is 0)
// * Day of year: `"%j"` (numeric 3-digit)
// * Date alone: `"%D"` same as `"{:%d%h%Y}"`
// * Date alone: `"%F"` Iso8601, like `"{:%Y-%m-%d}"`
// all of the above will yield results for `bdlt::DateTz::localDate()`.
// * Time zone: `"%z"`, 2 digit hour and 2-digit minute offset from UTC, with
//   or without a colon separating hours and minutes
// * Composite: `"{}"` formats the value as if streamed by `<<`, no `:`
//   between hours and minutes of the time zone
// * Composite: `"%i"` formats the entire value like Iso8601, where there is a
//   `:` between hours and minutes of the time zone
//
// See `bdlt_formatdoc` for the full description of `bsl::format` support for
// `bdlt` value types.

#include <bdlscm_version.h>

#include <bdlt_date.h>
#include <bdlt_dateformatter.h>
#include <bdlt_datetz.h>
#include <bdlt_formatter.h>
#include <bdlt_timezoneformatter.h>

#include <bsl_iosfwd.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdlt {

class DateTzFormatter_Cache;

                      // =====================
                      // class DateTzFormatter
                      // =====================

/// This `class` provides a specifier formatter for printing `DateTz` objects.
template <class t_CHAR>
class DateTzFormatter {
    typedef DateFormatter<t_CHAR>                 Date_Formatter;
    typedef TimeZoneFormatter<t_CHAR>             TimeZone_Formatter;
    typedef bsl::basic_string_view<t_CHAR>        StringView;
    typedef DateTzFormatter_Cache                 FormatCache;

    // DATA
    Date_Formatter      d_dateFormatter;
        // Formatter for date specifiers.

    TimeZone_Formatter  d_timeZoneFormatter;
        // Formatter for time zone specifiers.

  public:
    // CREATORS

    /// Create an object in its default initial state.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    DateTzFormatter();

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

                      // ===========================
                      // class DateTzFormatter_Cache
                      // ===========================

/// This `class` facilitates faster access to a `DateTz` object during printing
/// by batching access to several fields in a single call during construction
/// and caching them for quick access later.
class DateTzFormatter_Cache {
    // DATA
    DateFormatter_Cache d_dateFormatCache;
    int                 d_offset;

  public:
    // CREATORS

    /// Cache a copy of the specified `date` and `offset`.
    DateTzFormatter_Cache(const Date& date, int offset);

    // ACCESSORS

    /// Return the `DateFormatter_Cache` held by this object
    const DateFormatter_Cache& date() const;

    /// Return the offset.
    int offset() const;
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // ---------------------
                      // class DateTzFormatter
                      // ---------------------

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
DateTzFormatter<t_CHAR>::DateTzFormatter()
: d_dateFormatter()
, d_timeZoneFormatter()
{}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void DateTzFormatter<t_CHAR>::parseDefault()
{
    d_dateFormatter.    parseDefault();
    d_timeZoneFormatter.parseDefault();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void DateTzFormatter<t_CHAR>::parseIso8601()
{
    d_dateFormatter.    parseIso8601();
    d_timeZoneFormatter.parseIso8601();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool DateTzFormatter<t_CHAR>::parseNextModifier(StringView *specInOut)
{
    return d_dateFormatter.    parseNextModifier(specInOut) ||
           d_timeZoneFormatter.parseNextModifier(specInOut);
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool DateTzFormatter<t_CHAR>::parseNextSpecifier(StringView *specInOut)
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
void DateTzFormatter<t_CHAR>::postprocess(
                         const bslfmt::FormatSpecificationParser<t_CHAR>& spec)
{
    d_dateFormatter.    postprocess(spec);
    d_timeZoneFormatter.postprocess(spec);
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int DateTzFormatter<t_CHAR>::extraSections() const
{
    return d_dateFormatter.extraSections() |
                                           d_timeZoneFormatter.extraSections();
}

template <class t_CHAR>
inline
int DateTzFormatter<t_CHAR>::totalWidth(const FormatCache& value) const
{
    return d_dateFormatter.    totalWidth(value.date()) +
           d_timeZoneFormatter.totalWidth(value.offset());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
bool DateTzFormatter<t_CHAR>::formatNextSpecifier(
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
t_ITERATOR DateTzFormatter<t_CHAR>::formatDefault(
                                t_ITERATOR out, const FormatCache& value) const
{
    out = d_dateFormatter.    formatDefault(out, value.date());
    out = d_timeZoneFormatter.formatDefault(out, value.offset());

    return out;
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR DateTzFormatter<t_CHAR>::formatIso8601(
                                t_ITERATOR out, const FormatCache& value) const
{
    out = d_dateFormatter.    formatIso8601(out, value.date());
    out = d_timeZoneFormatter.formatIso8601(out, value.offset());

    return out;
}

                        // ---------------------
                        // DateTzFormatter_Cache
                        // ---------------------

// CREATORS
inline
DateTzFormatter_Cache::DateTzFormatter_Cache(const Date& date, int offset)
: d_dateFormatCache(date)
, d_offset(offset)
{}

// ACCESSORS
inline
const DateFormatter_Cache& DateTzFormatter_Cache::date() const
{
    return d_dateFormatCache;
}

inline
int DateTzFormatter_Cache::offset() const
{
    return d_offset;
}

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

/// This type implements the formatter logic specific for `DateTz` objects.
template <class t_CHAR>
class formatter<BloombergLP::bdlt::DateTz, t_CHAR> {
    // PRIVATE TYPES
    typedef BloombergLP::bdlt::DateTz                              DateTz;
    typedef BloombergLP::bdlt::DateTzFormatter_Cache               FormatCache;
    typedef BloombergLP::bdlt::Formatter<
                   BloombergLP::bdlt::DateTzFormatter, t_CHAR>     Formatter;

    // DATA
    Formatter                                               d_formatter;

  public:
    /// Parse and validate the specification string stored in the specified
    /// `parseContext`.  Return an end iterator of the parsed range.  Throw
    /// `bsl::format_error`, in the event of failure.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                      t_PARSE_CONTEXT& context)
    {
        return d_formatter.parse(context);
    }

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                        const DateTz&      value,
                                        t_FORMAT_CONTEXT&  formatContext) const
    {
        const FormatCache formatCache(value.localDate(), value.offset());
        return d_formatter.format(formatCache, formatContext);
    }
};

}  // close namespace bsl

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
