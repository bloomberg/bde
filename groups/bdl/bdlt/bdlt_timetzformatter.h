// bdlt_timetzformatter.h                                             -*-C++-*-
#ifndef INCLUDED_BDLT_TIMETZFORMATTER
#define INCLUDED_BDLT_TIMETZFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide `bsl::formatter` specialization for `bdlt::TimeTz`.
//
//@CLASSES:
//  bdlt::TimeTzFormatter: timetz formatter for `bslfmt` framework
//  bsl::formatter<bdlt::TimeTz, t_CHAR>: specialization
//
//@SEE_ALSO: bdlt_timetz, bdlt_formatdoc, bdlt_timeformatter, bslfmt_formatter
//
//@DESCRIPTION: This component provides `bdlt::TimeTzFormatter` and a
// specialization of `bsl::formatter` that allow `bsl::format` to output
// values of `bdlt::TimeTz`.
//
// The formatter interprets the following modifiers:
// * `,` decimal between integral seconds and fraction is a comma instead of a
//   period
// * `:` separate hours and minutes in time zone with `:`
// * `_` suppress `:` between hours and minutes in time zone
// * `Z` if time zone offset is 0, display it as `Z`
// Note that it is an error for both `:` and `_` to be specified at the same
// time.
//
// The formatter supports the following `%`-sequences:
// * Hour: `"%H"` (2 digit) `00` - `24`
// * Hour: `"%I"` (2 digit) `01` - `12`
// * AM/PM: `"%p"` - `AM` or `PM`
// * Minute: `"%M"` (2 digits) `00` - `59`
// * Second: `"%S"` (2 digits `00` - `59` + `.` + fraction)
// * Time alone: `"%T"`, same as `"{:%H:%M:%S}"`, 6 digit fraction
// * Hours and minutes alone: `"%R"`, same as `"{:%H:%M}"`
// all the above give figures relative to `bdlt::TimeTz::localTime()`.
// * Time zone: `"%z"`, 2 digit hour and 2-digit minute offset from UTC, with
//   or without a colon separating hours and minutes
// * Composite: `"{}"` formats the value as if streamed by `<<`, default 6
//   digit precision for seconds, no `:` between hours and minutes of the time
//   zone
// * Composite: `"%i"` formats the entire value like Iso8601, where precision
//   defaults to 3 digits and there is a `:` between hours and minutes of the
//   time zone
//
// See `bdlt_formatdoc` for the full description of `bsl::format` support for
// `bdlt` value types.

#include <bdlscm_version.h>

#include <bdlt_formatter.h>
#include <bdlt_time.h>
#include <bdlt_timeformatter.h>
#include <bdlt_timetz.h>
#include <bdlt_timezoneformatter.h>

#include <bsl_iosfwd.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdlt {

class TimeTzFormatter_Cache;

                        // =====================
                        // class TimeTzFormatter
                        // =====================

/// This `class` provides a specifier formatter for printing `TimeTz` objects.
template <class t_CHAR>
class TimeTzFormatter {
    // PRIVATE TYPES
    typedef TimeFormatter<t_CHAR>           Time_Formatter;
    typedef TimeZoneFormatter<t_CHAR>       TimeZone_Formatter;
    typedef bsl::basic_string_view<t_CHAR>  StringView;
    typedef TimeTzFormatter_Cache           FormatCache;

    enum {
        k_HOUR_WIDTH                = 2,
        k_AMPM_WIDTH                = 2,
        k_MINUTE_WIDTH              = 2,
        k_SECOND_WIDTH              = 2,
        k_COLON_WIDTH               = 1,
        k_DEFAULT_PRECISION         = 6,
        k_DEFAULT_ISO8601_PRECISION = 3 };

    // DATA
    Time_Formatter        d_timeFormatter;
    TimeZone_Formatter    d_timeZoneFormatter;

  public:
    // CREATORS

    /// Create an object in its default initial state.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    TimeTzFormatter();

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
                      // class TimeTzFormatter_Cache
                      // ===========================

/// This `class` facilitates faster access to a `TimeTz` object during printing
/// by batching access to several fields in a single call during construction
/// and caching them for quick access later.
class TimeTzFormatter_Cache {
    // DATA
    TimeFormatter_Cache d_timeFormatCache;
    int                 d_offset;

  public:
    // CREATORS

    /// Populate the time format cache with the time portion of the specified
    /// `value` and cache the timezone offset.
    TimeTzFormatter_Cache(const Time& localTime, int offset);

    // ACCESSORS

    /// Return the time format cache.
    const TimeFormatter_Cache& time() const;

    /// Return the offset.
    int offset() const;
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // ---------------------
                      // class TimeTzFormatter
                      // ---------------------

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
TimeTzFormatter<t_CHAR>::TimeTzFormatter()
: d_timeFormatter()
, d_timeZoneFormatter()
{}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void TimeTzFormatter<t_CHAR>::parseDefault()
{
    d_timeFormatter.    parseDefault();
    d_timeZoneFormatter.parseDefault();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void TimeTzFormatter<t_CHAR>::parseIso8601()
{
    d_timeFormatter.    parseIso8601();
    d_timeZoneFormatter.parseIso8601();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool TimeTzFormatter<t_CHAR>::parseNextModifier(StringView *specInOut)
{
    return d_timeFormatter.    parseNextModifier(specInOut) ||
           d_timeZoneFormatter.parseNextModifier(specInOut);
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool TimeTzFormatter<t_CHAR>::parseNextSpecifier(StringView *specInOut)
{
    BSLS_ASSERT(!specInOut->empty());

    if (t_CHAR('i') == specInOut->front()) {
        d_timeFormatter.    parseIso8601();
        d_timeZoneFormatter.parseIso8601();
        specInOut->remove_prefix(1);

        return true;                                                  // RETURN
    }

    return d_timeFormatter.    parseNextSpecifier(specInOut) ||
           d_timeZoneFormatter.parseNextSpecifier(specInOut);
}

template <class t_CHAR>
inline
void TimeTzFormatter<t_CHAR>::postprocess(
                         const bslfmt::FormatSpecificationParser<t_CHAR>& spec)
{
    d_timeFormatter.    postprocess(spec);
    d_timeZoneFormatter.postprocess(spec);
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int TimeTzFormatter<t_CHAR>::extraSections() const
{
    return d_timeFormatter.extraSections() |
                                           d_timeZoneFormatter.extraSections();
}

template <class t_CHAR>
inline
int TimeTzFormatter<t_CHAR>::totalWidth(const FormatCache& value) const
{
    return d_timeFormatter.totalWidth(value.time()) +
                                d_timeZoneFormatter.totalWidth(value.offset());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
bool TimeTzFormatter<t_CHAR>::formatNextSpecifier(
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

    return d_timeFormatter.    formatNextSpecifier(specInOut,
                                                   outIt,
                                                   value.time()) ||
           d_timeZoneFormatter.formatNextSpecifier(specInOut,
                                                   outIt,
                                                   value.offset());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR TimeTzFormatter<t_CHAR>::formatDefault(
                                t_ITERATOR out, const FormatCache& value) const
{
    out =  d_timeFormatter.    formatDefault(out, value.time());
    return d_timeZoneFormatter.formatDefault(out, value.offset());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR TimeTzFormatter<t_CHAR>::formatIso8601(
                                t_ITERATOR out, const FormatCache& value) const
{
    out =  d_timeFormatter.    formatIso8601(out, value.time());
    return d_timeZoneFormatter.formatIso8601(out, value.offset());
}

                    // ---------------------------
                    // class TimeTzFormatter_Cache
                    // ---------------------------

// CREATORS
inline
TimeTzFormatter_Cache::TimeTzFormatter_Cache(const Time& localTime, int offset)
: d_timeFormatCache(localTime)
, d_offset(offset)
{}

// ACCESSORS
inline
const TimeFormatter_Cache& TimeTzFormatter_Cache::time() const
{
    return d_timeFormatCache;
}

inline
int TimeTzFormatter_Cache::offset() const
{
    return d_offset;
}

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

/// This type implements the formatter logic specific for `TimeTz` objects.
template <class t_CHAR>
class formatter<BloombergLP::bdlt::TimeTz, t_CHAR> {
    // PRIVATE TYPES
    typedef BloombergLP::bdlt::TimeTz                           TimeTz;
    typedef BloombergLP::bdlt::TimeTzFormatter_Cache            FormatCache;
    typedef BloombergLP::bdlt::Formatter<
                   BloombergLP::bdlt::TimeTzFormatter, t_CHAR> Formatter;

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
                                        const TimeTz&      value,
                                        t_FORMAT_CONTEXT&  formatContext) const
    {
        const FormatCache formatCache(value.localTime(), value.offset());
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
