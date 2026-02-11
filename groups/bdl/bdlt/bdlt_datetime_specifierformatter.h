// bdlt_datetime_specifierformatter.h                                 -*-C++-*-
#ifndef INCLUDED_BDLT_DATETIME_SPECIFIERFORMATTER
#define INCLUDED_BDLT_DATETIME_SPECIFIERFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a datetime specifier formatter.
//
//@CLASSES:
//  bdlt::Datetime_SpecifierFormatter: datetime formatter for `bslfmt`
//
//@SEE_ALSO: bdlt_datetime, bdlt_date_specifierformatter,
//           bdlt_time_specifierformatter, bslfmt_formatter
//
//@DESCRIPTION: This component provides a single class,
// `bdlt::Datetime_SpecifierFormatter`, that implements datetime formatting for
// the `bslfmt` framework.  This class is used by `bdlt::Datetime` and other
// datetime-related components for consistent formatting.
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
#include <bdlt_date_specifierformatter.h>
#include <bdlt_time_specifierformatter.h>

#include <bslfmt_formatspecificationparser.h>

#include <bsls_exceptionutil.h>

#include <bsl_iosfwd.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdlt {

class Datetime_SpecifierFormatter_Cache;

                      // ==================================
                      // class Datetime_SpecifierFormatter
                      // ==================================

/// This `class` provides a specifier formatter for printing `Datetime`
/// objects.
template <class t_CHAR>
class Datetime_SpecifierFormatter {
    typedef bsl::basic_string_view<t_CHAR>       StringView;
    typedef Datetime_SpecifierFormatter_Cache    FormatCache;

    // DATA
    int                              d_fixedWidth;

    Date_SpecifierFormatter<t_CHAR>  d_dateFormatter;
        // Formatter for date specifiers.

    Time_SpecifierFormatter<t_CHAR>  d_timeFormatter;
        // Formatter for time specifiers.

  public:
    // CREATORS

    /// Create an object in its default initial state.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    Datetime_SpecifierFormatter();

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

                    // =======================================
                    // class Datetime_SpecifierFormatter_Cache
                    // =======================================

/// This `class` facilitates faster access to a `Datetime` object during
/// printing by batching access to several fields per call during construction
/// and caching them for quick access later.
class Datetime_SpecifierFormatter_Cache {
    // DATA
    Date_SpecifierFormatter_Cache<Date>     d_dateFormatCache;
    Time_SpecifierFormatter_Cache           d_timeFormatCache;

  public:
    // CREATORS

    /// Create a format cache initialized with the specified `value`.
    template <class t_DATETIME>
    explicit Datetime_SpecifierFormatter_Cache(const t_DATETIME& value);

    // ACCESSORS

    /// Return the date format cache held by this object.
    const Date_SpecifierFormatter_Cache<Date>& date() const;

    /// Return the time format cache held by this object.
    const Time_SpecifierFormatter_Cache& time() const;
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // ---------------------------------
                      // class Datetime_SpecifierFormatter
                      // ---------------------------------

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
Datetime_SpecifierFormatter<t_CHAR>::Datetime_SpecifierFormatter()
: d_fixedWidth(0)
, d_dateFormatter()
, d_timeFormatter()
{}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void Datetime_SpecifierFormatter<t_CHAR>::parseDefault()
{
    d_dateFormatter.parseDefault();
    ++d_fixedWidth;  // For '_' between date and time
    d_timeFormatter.parseDefault();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void Datetime_SpecifierFormatter<t_CHAR>::parseIso8601()
{
    d_dateFormatter.parseIso8601();
    ++d_fixedWidth;  // For 'T' between date and time
    d_timeFormatter.parseIso8601();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool Datetime_SpecifierFormatter<t_CHAR>::parseNextModifier(
                                                         StringView *specInOut)
{
    return d_dateFormatter.parseNextModifier(specInOut) ||
           d_timeFormatter.parseNextModifier(specInOut);
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool Datetime_SpecifierFormatter<t_CHAR>::parseNextSpecifier(
                                                         StringView *specInOut)
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
void Datetime_SpecifierFormatter<t_CHAR>::postprocess(
                         const bslfmt::FormatSpecificationParser<t_CHAR>& spec)
{
    d_dateFormatter.postprocess(spec);
    d_timeFormatter.postprocess(spec);
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int Datetime_SpecifierFormatter<t_CHAR>::extraSections() const
{
    return d_dateFormatter.extraSections() |
           d_timeFormatter.extraSections();
}

template <class t_CHAR>
inline
int Datetime_SpecifierFormatter<t_CHAR>::totalWidth(
                                                const FormatCache& value) const
{
    return d_fixedWidth + d_dateFormatter.totalWidth(value.date()) +
                                      d_timeFormatter.totalWidth(value.time());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
bool Datetime_SpecifierFormatter<t_CHAR>::formatNextSpecifier(
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
t_ITERATOR Datetime_SpecifierFormatter<t_CHAR>::formatDefault(
                                t_ITERATOR out, const FormatCache& value) const
{
    out    = d_dateFormatter.formatDefault(out, value.date());
    *out++ = t_CHAR('_');
    return   d_timeFormatter.formatDefault(out, value.time());
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR Datetime_SpecifierFormatter<t_CHAR>::formatIso8601(
                                t_ITERATOR out, const FormatCache& value) const
{
    out    = d_dateFormatter.formatIso8601(out, value.date());
    *out++ = t_CHAR('T');
    return   d_timeFormatter.formatIso8601(out, value.time());
}

                  // ---------------------------------------
                  // class Datetime_SpecifierFormatter_Cache
                  // ---------------------------------------

// CREATORS
template <class t_DATETIME>
inline
Datetime_SpecifierFormatter_Cache::
                     Datetime_SpecifierFormatter_Cache(const t_DATETIME& value)
: d_dateFormatCache(value.date())
, d_timeFormatCache(value)
{}

// ACCESSORS

inline
const Date_SpecifierFormatter_Cache<Date>&
                                Datetime_SpecifierFormatter_Cache::date() const
{
    return d_dateFormatCache;
}

inline
const Time_SpecifierFormatter_Cache&
                                Datetime_SpecifierFormatter_Cache::time() const
{
    return d_timeFormatCache;
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
