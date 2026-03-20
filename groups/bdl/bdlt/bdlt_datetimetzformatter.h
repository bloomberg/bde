// bdlt_datetimetzformatter.h                                         -*-C++-*-
#ifndef INCLUDED_BDLT_DATETIMETZFORMATTER
#define INCLUDED_BDLT_DATETIMETZFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide `bsl::formatter` specialization for `bdlt::DatetimeTz`.
//
//@CLASSES:
//  bdlt::DatetimeTzFormatter: datetimetz formatter for `bslfmt`
//  bsl::formatter<bdlt::DatetimeTz, t_CHAR>: specialization
//
//@SEE_ALSO: bdlt_datetimetz, bdlt_dateformatter,
//           bdlt_timeformatter, bslfmt_formatter
//
//@DESCRIPTION: This component provides `bdlt::DatetimeTzFormatter` and a
// specialization of `bsl::formatter` that allow `bsl::format` to output
// values of `bdlt::DatetimeTz`.
//
// The formatter interprets the following modifiers:
// - ',' (comma) - the decimal point when displaying seconds is shown as a
//   comma rather than a period.
// - 'Z' - if the offset is zero, output the time zone as 'Z', otherwise output
//   it normally.
// - ':' (colon) - always print a colon between hours and minutes
// - '_' (underscore) - never print a colon between hours and minutes
// Note that it is an error for both ':' and '_' to be specified at the same
// time.
//
// The formatter supports various date and format specifiers including:
// - Year: 'Y' (4-digit), 'y' (2-digit), 'C' (century)
// - Month: 'm' (numeric), 'b'/'h' (abbreviated name)
// - Day: 'd' (zero-padded), 'e' (space-padded)
// - Day of week: 'a' (abbreviated name), 'u'/'w' (numeric)
// - Day of year: 'j' (numeric)
// - Hour: 'H' (2 digit) "00" - "24"
// - Hour: 'I' (2 digit) "01" - "12"
// - AM/PM: 'p' - "AM" or "PM"
// - Minute: 'M' (2 digits) "00" - "59"
// - Second: 'S' (2 digits) "00" - "59"
// - Composite: 'T' (time in default format), 'D' (date in default format),
//   'F' (date in ISO 8601 format)
// - Time Zone Offset: 'z' (2 digit hour, 2 digit minute)
// - Composite: "{}" (`localDatetime()` in default format with time zone) or
//   'i' (`localDatetime()` in ISO 8601 format with time zone)

#include <bdlscm_version.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimeformatter.h>
#include <bdlt_datetimetz.h>
#include <bdlt_formatter.h>
#include <bdlt_timezoneformatter.h>

#include <bsl_iosfwd.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdlt {

class DatetimeTzFormatter_Cache;

                    // =========================
                    // class DatetimeTzFormatter
                    // =========================

/// This `class` provides a specifier formatter for printing `DatetimeTz`
/// objects.
template <class t_CHAR>
class DatetimeTzFormatter {
    typedef bsl::basic_string_view<t_CHAR> StringView;
    typedef DatetimeTzFormatter_Cache      FormatCache;

    // DATA
    int                       d_fixedWidth;
    DatetimeFormatter<t_CHAR> d_datetimeFormatter;
    TimeZoneFormatter<t_CHAR> d_timeZoneFormatter;

  public:
    // CREATORS

    /// Create an object in its default initial state.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    DatetimeTzFormatter();

    // MANIPULATORS

    /// Parse a date time tz that will be formatted in default mode.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseDefault();

    /// Parse a date time tz that will be formatted in Iso8601 mode.
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
                                                        StringView *specInOut);

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

                      // ===============================
                      // class DatetimeTzFormatter_Cache
                      // ===============================

/// This `class` facilitates faster access to a `DatetimeTz` object during
/// printing by batching access to several fields per call during construction
/// and caching them for quick access later.
class DatetimeTzFormatter_Cache {
    // DATA
    DatetimeFormatter_Cache d_datetimeFormatCache;
    int                     d_offset;

  public:
    // CREATORS

    /// Create a `DatetimeTz` format cache using the specified `datetime` and
    /// `offset`.
    DatetimeTzFormatter_Cache(const Datetime& datetime, int offset);

    // ACCESSORS

    /// Return the `Datetime` format cache.
    const DatetimeFormatter_Cache& datetime() const;

    /// Return the offset.
    int offset() const;
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // -------------------------
                    // class DatetimeTzFormatter
                    // -------------------------

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
DatetimeTzFormatter<t_CHAR>::DatetimeTzFormatter()
: d_fixedWidth(0)
, d_datetimeFormatter()
, d_timeZoneFormatter()
{}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool DatetimeTzFormatter<t_CHAR>::parseNextSpecifier(StringView *specInOut)
{
    BSLS_ASSERT(!specInOut->empty());

    if (t_CHAR('i') == specInOut->front()) {
        parseIso8601();
        specInOut->remove_prefix(1);

        return true;                                                  // RETURN
    }

    return d_datetimeFormatter.parseNextSpecifier(specInOut) ||
           d_timeZoneFormatter.parseNextSpecifier(specInOut);
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void DatetimeTzFormatter<t_CHAR>::parseDefault()
{
    d_datetimeFormatter.parseDefault();
    d_timeZoneFormatter.parseDefault();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void DatetimeTzFormatter<t_CHAR>::parseIso8601()
{
    d_datetimeFormatter.parseIso8601();
    d_timeZoneFormatter.parseIso8601();
}

template <class t_CHAR>
inline
void DatetimeTzFormatter<t_CHAR>::postprocess(
                         const bslfmt::FormatSpecificationParser<t_CHAR>& spec)
{
    d_datetimeFormatter.postprocess(spec);
    d_timeZoneFormatter.postprocess(spec);
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool DatetimeTzFormatter<t_CHAR>::parseNextModifier(StringView *specInOut)
{
    return d_datetimeFormatter.parseNextModifier(specInOut) ||
           d_timeZoneFormatter.parseNextModifier(specInOut);
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int DatetimeTzFormatter<t_CHAR>::extraSections() const
{
    return d_datetimeFormatter.extraSections() |
                                           d_timeZoneFormatter.extraSections();
}

template <class t_CHAR>
inline
int DatetimeTzFormatter<t_CHAR>::totalWidth(const FormatCache& value)const
{
    return d_fixedWidth + d_datetimeFormatter.totalWidth(value.datetime()) +
                                d_timeZoneFormatter.totalWidth(value.offset());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
bool DatetimeTzFormatter<t_CHAR>::formatNextSpecifier(
                                               StringView         *specInOut,
                                               t_ITERATOR         *outIt,
                                               const FormatCache&  value) const
{
    BSLS_ASSERT(!specInOut->empty());

    if (t_CHAR('i') == specInOut->front()) {
        *outIt = formatIso8601(*outIt, value);
        specInOut->remove_prefix(1);
        return true;                                                  // RETURN
    }

    return d_datetimeFormatter.formatNextSpecifier(specInOut,
                                                   outIt,
                                                   value.datetime()) ||
           d_timeZoneFormatter.formatNextSpecifier(specInOut,
                                                   outIt,
                                                   value.offset());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR DatetimeTzFormatter<t_CHAR>::formatDefault(
                                t_ITERATOR out, const FormatCache& value) const
{
    out = d_datetimeFormatter.formatDefault(out, value.datetime());
    out = d_timeZoneFormatter.formatDefault(out, value.offset());
    return out;
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR DatetimeTzFormatter<t_CHAR>::formatIso8601(
                                t_ITERATOR out, const FormatCache& value) const
{
    out = d_datetimeFormatter.formatIso8601(out, value.datetime());
    out = d_timeZoneFormatter.formatIso8601(out, value.offset());
    return out;
}

                    // -------------------------
                    // DatetimeTzFormatter_Cache
                    // -------------------------

// CREATORS
inline
DatetimeTzFormatter_Cache::DatetimeTzFormatter_Cache(const Datetime& value,
                                                     int             offset)
: d_datetimeFormatCache(value)
, d_offset(offset)
{}

// ACCESSORS
inline
const DatetimeFormatter_Cache&
                          DatetimeTzFormatter_Cache::datetime() const
{
    return d_datetimeFormatCache;
}

inline
int DatetimeTzFormatter_Cache::offset() const
{
    return d_offset;
}

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

/// This type implements the formatter logic specific for `DatetimeTz` objects.
template <class t_CHAR>
class formatter<BloombergLP::bdlt::DatetimeTz, t_CHAR> {
    // PRIVATE TYPES
    typedef BloombergLP::bdlt::DatetimeTz                          DatetimeTz;
    typedef BloombergLP::bdlt::DatetimeTzFormatter_Cache           FormatCache;
    typedef BloombergLP::bdlt::Formatter<
                   BloombergLP::bdlt::DatetimeTzFormatter, t_CHAR> Formatter;

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
                                        const DatetimeTz&  value,
                                        t_FORMAT_CONTEXT&  formatContext) const
    {
        const FormatCache formatCache(value.localDatetime(), value.offset());
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
