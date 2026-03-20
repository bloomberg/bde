// bdlt_datetimeformatter.h                                           -*-C++-*-
#ifndef INCLUDED_BDLT_DATETIMEFORMATTER
#define INCLUDED_BDLT_DATETIMEFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide `bsl::formatter` specialization for `bdlt::Datetime`.
//
//@CLASSES:
//  bdlt::DatetimeFormatter: datetime formatter for `bslfmt`
//  bsl::formatter<bdlt::Datetime, t_CHAR>: specialization
//
//@SEE_ALSO: bdlt_datetime, bdlt_dateformatter,
//           bdlt_timeformatter, bslfmt_formatter
//
//@DESCRIPTION: This component provides `bdlt::DatetimeFormatter` and a
// specialization of `bsl::formatter` that allow `bsl::format` to output
// values of `bdlt::Datetime`.
//
// The formatter interprets the following modifier:
// - ',' (comma) - the decimal point when displaying seconds is shown as a
//   comma rather than a period.
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
// - Composite: "{}" (`Datetime` in default format), or 'i' (`Datetime` in
//   ISO 8601 format)

#include <bdlscm_version.h>

#include <bdlt_date.h>
#include <bdlt_dateformatter.h>
#include <bdlt_datetime.h>
#include <bdlt_formatter.h>
#include <bdlt_timeformatter.h>

#include <bslfmt_formatspecificationparser.h>

#include <bsls_exceptionutil.h>

#include <bsl_iosfwd.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdlt {

class DatetimeFormatter_Cache;

                      // =======================
                      // class DatetimeFormatter
                      // =======================

/// This `class` provides a specifier formatter for printing `Datetime`
/// objects.
template <class t_CHAR>
class DatetimeFormatter {
    typedef bsl::basic_string_view<t_CHAR>       StringView;
    typedef DatetimeFormatter_Cache              FormatCache;

    // DATA
    int                    d_fixedWidth;

    DateFormatter<t_CHAR>  d_dateFormatter;
        // Formatter for date specifiers.

    TimeFormatter<t_CHAR>  d_timeFormatter;
        // Formatter for time specifiers.

  public:
    // CREATORS

    /// Create an object in its default initial state.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    DatetimeFormatter();

    // MANIPULATORS

    /// Parse a date time that will be formatted in default mode.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseDefault();

    /// Parse a date time that will be formatted in Iso8601 mode.
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

                    // =============================
                    // class DatetimeFormatter_Cache
                    // =============================

/// This `class` facilitates faster access to a `Datetime` object during
/// printing by batching access to several fields per call during construction
/// and caching them for quick access later.
class DatetimeFormatter_Cache {
    // DATA
    DateFormatter_Cache     d_dateFormatCache;
    TimeFormatter_Cache     d_timeFormatCache;

  public:
    // CREATORS

    /// Create a format cache initialized with the specified `value`.
    explicit DatetimeFormatter_Cache(const Datetime& value);

    // ACCESSORS

    /// Return the date format cache held by this object.
    const DateFormatter_Cache& date() const;

    /// Return the time format cache held by this object.
    const TimeFormatter_Cache& time() const;
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // -----------------------
                      // class DatetimeFormatter
                      // -----------------------

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
DatetimeFormatter<t_CHAR>::DatetimeFormatter()
: d_fixedWidth(0)
, d_dateFormatter()
, d_timeFormatter()
{}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void DatetimeFormatter<t_CHAR>::parseDefault()
{
    d_dateFormatter.parseDefault();
    ++d_fixedWidth;  // For '_' between date and time
    d_timeFormatter.parseDefault();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void DatetimeFormatter<t_CHAR>::parseIso8601()
{
    d_dateFormatter.parseIso8601();
    ++d_fixedWidth;  // For 'T' between date and time
    d_timeFormatter.parseIso8601();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool DatetimeFormatter<t_CHAR>::parseNextModifier(StringView *specInOut)
{
    return d_dateFormatter.parseNextModifier(specInOut) ||
           d_timeFormatter.parseNextModifier(specInOut);
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool DatetimeFormatter<t_CHAR>::parseNextSpecifier(StringView *specInOut)
{
    BSLS_ASSERT(!specInOut->empty());

    if (t_CHAR('i') == specInOut->front()) {
        // Intercept iso format

        parseIso8601();
        specInOut->remove_prefix(1);
        return true;                                                  // RETURN
    }

    return d_dateFormatter.parseNextSpecifier(specInOut) ||
           d_timeFormatter.parseNextSpecifier(specInOut);
}

template <class t_CHAR>
inline
void DatetimeFormatter<t_CHAR>::postprocess(
                         const bslfmt::FormatSpecificationParser<t_CHAR>& spec)
{
    d_dateFormatter.postprocess(spec);
    d_timeFormatter.postprocess(spec);
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int DatetimeFormatter<t_CHAR>::extraSections() const
{
    return d_dateFormatter.extraSections() |
           d_timeFormatter.extraSections();
}

template <class t_CHAR>
inline
int DatetimeFormatter<t_CHAR>::totalWidth(const FormatCache& value) const
{
    return d_fixedWidth + d_dateFormatter.totalWidth(value.date()) +
           d_timeFormatter.totalWidth(value.time());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
bool DatetimeFormatter<t_CHAR>::formatNextSpecifier(
                                               StringView         *specInOut,
                                               t_ITERATOR         *outIt,
                                               const FormatCache&  value) const
{
    BSLS_ASSERT_SAFE(!specInOut->empty());

    if (t_CHAR('i') == specInOut->front()) {
        *outIt     = formatIso8601(*outIt, value);
        specInOut->remove_prefix(1);

        return true;                                                  // RETURN
    }

    return d_dateFormatter.formatNextSpecifier(specInOut, outIt, value.date())
        || d_timeFormatter.formatNextSpecifier(specInOut, outIt, value.time());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR DatetimeFormatter<t_CHAR>::formatDefault(
                                t_ITERATOR out, const FormatCache& value) const
{
    out    = d_dateFormatter.formatDefault(out, value.date());
    *out++ = t_CHAR('_');
    return   d_timeFormatter.formatDefault(out, value.time());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR DatetimeFormatter<t_CHAR>::formatIso8601(
                                t_ITERATOR out, const FormatCache& value) const
{
    out    = d_dateFormatter.formatIso8601(out, value.date());
    *out++ = t_CHAR('T');
    return   d_timeFormatter.formatIso8601(out, value.time());
}

                  // -----------------------------
                  // class DatetimeFormatter_Cache
                  // -----------------------------

// CREATORS
inline
DatetimeFormatter_Cache::DatetimeFormatter_Cache(const Datetime& value)
: d_dateFormatCache(value.date())
, d_timeFormatCache(value.time())
{}

// ACCESSORS

inline
const DateFormatter_Cache& DatetimeFormatter_Cache::date() const
{
    return d_dateFormatCache;
}

inline
const TimeFormatter_Cache& DatetimeFormatter_Cache::time() const
{
    return d_timeFormatCache;
}

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

/// This type implements the formatter logic specific for `Datetime` objects.
template <class t_CHAR>
class formatter<BloombergLP::bdlt::Datetime, t_CHAR> {
    // PRIVATE TYPES
    typedef BloombergLP::bdlt::Datetime                            Datetime;
    typedef BloombergLP::bdlt::DatetimeFormatter_Cache             FormatCache;
    typedef BloombergLP::bdlt::Formatter<
                 BloombergLP::bdlt::DatetimeFormatter, t_CHAR>  Formatter;

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
                                        const Datetime&    value,
                                        t_FORMAT_CONTEXT&  formatContext) const
    {
        return d_formatter.format(FormatCache(value), formatContext);
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
