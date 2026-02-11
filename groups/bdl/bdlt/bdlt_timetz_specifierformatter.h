// bdlt_timetz_specifierformatter.h                                   -*-C++-*-
#ifndef INCLUDED_BDLT_TIMETZ_SPECIFIERFORMATTER
#define INCLUDED_BDLT_TIMETZ_SPECIFIERFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a time-with-timezone specifier formatter
//
//@CLASSES:
//  bdlt::TimeTz_SpecifierFormatter: timetz formatter for `bslfmt` framework
//
//@SEE_ALSO: bdlt_timetz, bdlt_time_specifierformatter, bslfmt_formatter
//
//@DESCRIPTION: This component provides a single class,
// `bdlt::TimeTz_SpecifierFormatter`, that implements time-with-timezone
// formatting for the `bslfmt` framework.  This class is used by
// `bdlt::TimeTz` and other time-with-timezone-related components for
// consistent formatting.
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
// The formatter supports various time format specifiers including:
// - Hour: 'H' (2 digit) "00" - "24"
// - Hour: 'I' (2 digit) "01" - "12"
// - AM/PM: 'p' - "AM" or "PM"
// - Minute: 'M' (2 digits) "00" - "59"
// - Second: 'S' (2 digits) "00" - "59"
// all the above give figures relative to `bdlt::TimeTz::localTime()`.
// - Offset: 'z' (time zone, 2 digit hour, 2 digit minutes)
// - Composite: 'T' (`localTime()` in default format), "{}" (time in default
//   format with time zone), 'i' (time in ISO 8601 format with time zone)

#include <bdlscm_version.h>

#include <bdlt_time.h>
#include <bdlt_time_specifierformatter.h>
#include <bdlt_timezone_specifierformatter.h>

#include <bsl_iosfwd.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdlt {

class TimeTz_SpecifierFormatter_Cache;

                        // ===============================
                        // class TimeTz_SpecifierFormatter
                        // ===============================

/// This `class` provides a specifier formatter for printing `TimeTz` objects.
template <class t_CHAR>
class TimeTz_SpecifierFormatter {
    // PRIVATE TYPES
    typedef Time_SpecifierFormatter<t_CHAR>       TimeFormatter;
    typedef TimeZone_SpecifierFormatter<t_CHAR>   TimeZoneFormatter;
    typedef bsl::basic_string_view<t_CHAR>        StringView;
    typedef TimeTz_SpecifierFormatter_Cache       FormatCache;

    enum {
        k_HOUR_WIDTH                = 2,
        k_AMPM_WIDTH                = 2,
        k_MINUTE_WIDTH              = 2,
        k_SECOND_WIDTH              = 2,
        k_COLON_WIDTH               = 1,
        k_DEFAULT_PRECISION         = 6,
        k_DEFAULT_ISO8601_PRECISION = 3 };

    // DATA
    TimeFormatter        d_timeFormatter;
    TimeZoneFormatter    d_timeZoneFormatter;

  public:
    // CREATORS

    /// Create an object in its default initial state.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    TimeTz_SpecifierFormatter();

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

                      // =====================================
                      // class TimeTz_SpecifierFormatter_Cache
                      // =====================================

/// This `class` facilitates faster access to a `TimeTz` object during printing
/// by batching access to several fields in a single call during construction
/// and caching them for quick access later.
class TimeTz_SpecifierFormatter_Cache {
    // DATA
    Time_SpecifierFormatter_Cache d_timeFormatCache;
    int                           d_offset;

  public:
    // CREATORS

    /// Populate the time format cache with the time portion of the specified
    /// `value` and cache the timezone offset.
    TimeTz_SpecifierFormatter_Cache(const Time& localTime, int offset);

    // ACCESSORS

    /// Return the time format cache.
    const Time_SpecifierFormatter_Cache& time() const;

    /// Return the offset.
    int offset() const;
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // -------------------------------
                      // class TimeTz_SpecifierFormatter
                      // -------------------------------

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
TimeTz_SpecifierFormatter<t_CHAR>::TimeTz_SpecifierFormatter()
: d_timeFormatter()
, d_timeZoneFormatter()
{}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void TimeTz_SpecifierFormatter<t_CHAR>::parseDefault()
{
    d_timeFormatter.    parseDefault();
    d_timeZoneFormatter.parseDefault();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void TimeTz_SpecifierFormatter<t_CHAR>::parseIso8601()
{
    d_timeFormatter.    parseIso8601();
    d_timeZoneFormatter.parseIso8601();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool TimeTz_SpecifierFormatter<t_CHAR>::parseNextModifier(
                                                         StringView *specInOut)
{
    return d_timeFormatter.    parseNextModifier(specInOut) ||
           d_timeZoneFormatter.parseNextModifier(specInOut);
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool TimeTz_SpecifierFormatter<t_CHAR>::parseNextSpecifier(
                                                         StringView *specInOut)
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
void TimeTz_SpecifierFormatter<t_CHAR>::postprocess(
                         const bslfmt::FormatSpecificationParser<t_CHAR>& spec)
{
    d_timeFormatter.    postprocess(spec);
    d_timeZoneFormatter.postprocess(spec);
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int TimeTz_SpecifierFormatter<t_CHAR>::extraSections() const
{
    return d_timeFormatter.extraSections() |
                                       d_timeZoneFormatter.extraSections();
}

template <class t_CHAR>
inline
int TimeTz_SpecifierFormatter<t_CHAR>::totalWidth(
                                                const FormatCache& value) const
{
    return d_timeFormatter.totalWidth(value.time()) +
                                d_timeZoneFormatter.totalWidth(value.offset());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
bool TimeTz_SpecifierFormatter<t_CHAR>::formatNextSpecifier(
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
t_ITERATOR TimeTz_SpecifierFormatter<t_CHAR>::formatDefault(
                                t_ITERATOR out, const FormatCache& value) const
{
    out =  d_timeFormatter.    formatDefault(out, value.time());
    return d_timeZoneFormatter.formatDefault(out, value.offset());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR TimeTz_SpecifierFormatter<t_CHAR>::formatIso8601(
                                t_ITERATOR out, const FormatCache& value) const
{
    out =  d_timeFormatter.    formatIso8601(out, value.time());
    return d_timeZoneFormatter.formatIso8601(out, value.offset());
}

                    // -------------------------------------
                    // class TimeTz_SpecifierFormatter_Cache
                    // -------------------------------------

// CREATORS
inline
TimeTz_SpecifierFormatter_Cache::TimeTz_SpecifierFormatter_Cache(
                                             const Time& localTime, int offset)
: d_timeFormatCache(localTime)
, d_offset(offset)
{}

// ACCESSORS
inline
const Time_SpecifierFormatter_Cache&
                                  TimeTz_SpecifierFormatter_Cache::time() const
{
    return d_timeFormatCache;
}

inline
int TimeTz_SpecifierFormatter_Cache::offset() const
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
