// bdlt_timezone_specifierformatter.h                                 -*-C++-*-
#ifndef INCLUDED_BDLT_TIMEZONE_SPECIFIERFORMATTER
#define INCLUDED_BDLT_TIMEZONE_SPECIFIERFORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism for formatting time zones.
//
//@CLASSES:
//  bdlt::TimeZone_SpecifierFormatter
//
//@DESCRIPTION: This component implements a formatter for time zone
// information.  Output can be done with the 'z' specifier, which outputs a
// 2-digit hour and a 2-digit minute, sometimes but not always separated by a
// ':'.
//
// This formatter interprets the following modifiers:
// - 'Z' - if the offset is zero, output the time zone as 'Z', otherwise output
//   it normally.
// - ':' (colon) - always print a colon between hours and minutes
// - '_' (underscore) - never print a colon between hours and minutes
// Note that it is an error for both ':' and '_' to be specified at the same
// time.
//
// This formatter interprets only the following format specifier:
// - 'z' - output the time zone in default format (subject to any applicable
//   modifiers).

#include <bdlscm_version.h>

#include <bdlt_formatutil.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_exceptionutil.h>

#include <bsl_cmath.h>

namespace BloombergLP {
namespace bdlt {

                     // =================================
                     // class TimeZone_SpecifierFormatter
                     // =================================

/// This `class` provides a specifier formatter for printing time zones.
template <class t_CHAR>
class TimeZone_SpecifierFormatter {
    // PRIVATE TYPES
    enum { k_OFFSET_LIMIT   = 24 * 60,
           k_INITIAL_OFFSET = 2 * k_OFFSET_LIMIT };

    typedef FormatUtil<t_CHAR>         Util;
    typedef typename Util::StringView  StringView;

    // DATA
    int  d_numDefaultTimeZones;
    int  d_numIso8601TimeZones;
    bool d_colon;
    bool d_noColon;
    bool d_tzZ;

    BSLMF_ASSERT((bsl::is_same<t_CHAR, char>::value ||
                                        bsl::is_same<t_CHAR, wchar_t>::value));

    // PRIVATE ACCESSORS

    /// Format the time zone indicated by the specified `offset` to the
    /// specified `out`, where the specified `isIso8601` indicates whether the
    /// time zone is to be formatted Iso8601 style or default style.
    template <class t_ITERATOR>
    t_ITERATOR formatImp(t_ITERATOR out, int offset, bool isIso8601) const;

    /// Return `true` if the time zone indicated by the specified `offset`
    /// should be shown as "Z" and `false` otherwise.
    bool isTimeZoneZ(int offset) const;

  public:
    // CREATORS

    /// Create an object in its default initial state.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    TimeZone_SpecifierFormatter();

    // MANIPULATORS

    /// Parse a time zone that will be formatted in default mode.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseDefault();

    /// Parse a time zone that will be formatted in Iso8601 mode.
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
    BSLS_KEYWORD_CONSTEXPR_CPP20
    int extraSections() const;

    /// Format the time zone indicated by the specified `offset` to the
    /// specified `out` using the default format and return `out`.
    template <class t_ITERATOR>
    t_ITERATOR formatDefault(t_ITERATOR out, int offset) const;

    /// Format the time zone indicated by the specified `offset` to the
    /// specified `out` using the Iso8601 format and return `out`.
    template <class t_ITERATOR>
    t_ITERATOR formatIso8601(t_ITERATOR out, int offset) const;

    /// If the first character of the specified `*specInOut` is recognized by
    /// this specifier formatter, use it to format the time zone indicated by
    /// the specified `offset` to `*outIt`, remove the character from
    /// `*specInOut`, and return `true`, otherwise return `false` with no
    /// modification to `*specInOut`.
    template <class t_ITERATOR>
    bool formatNextSpecifier(bsl::basic_string_view<t_CHAR> *specInOut,
                             t_ITERATOR                     *outIt,
                             int                             offset) const;

    /// Return the anticipated width of output given all the `parse*` calls
    /// that have been happened thus far and the time zone indicated by the
    /// specified `offset`.
    int totalWidth(int offset) const;
};

                         // ---------------------------------
                         // class TimeZone_SpecifierFormatter
                         // ---------------------------------

// PRIVATE ACCESSORS
template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR TimeZone_SpecifierFormatter<t_CHAR>::formatImp(
                              t_ITERATOR out, int offset, bool isIso8601) const
{
    if (isTimeZoneZ(offset)) {
        *out++ = t_CHAR('Z');

        return out;                                                   // RETURN
    }

    *out++ = offset < 0 ? t_CHAR('-') : t_CHAR('+');

    const int absOffset = bsl::abs(offset);

    BSLS_ASSERT_SAFE(absOffset <= k_OFFSET_LIMIT);

    const int hours   = absOffset / 60;
    const int minutes = absOffset % 60;

    out =  Util::writeZeroPaddedDigits(out, hours,   2);
    if (d_colon || (isIso8601 && !d_noColon)) {
        *out++ = t_CHAR(':');
    }
    return Util::writeZeroPaddedDigits(out, minutes, 2);
}

template <class t_CHAR>
bool TimeZone_SpecifierFormatter<t_CHAR>::isTimeZoneZ(int offset) const
{
    return d_tzZ && 0 == offset;
}

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
TimeZone_SpecifierFormatter<t_CHAR>::TimeZone_SpecifierFormatter()
: d_numDefaultTimeZones(0)
, d_numIso8601TimeZones(0)
, d_colon(false)
, d_noColon(false)
, d_tzZ(false)
{}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void TimeZone_SpecifierFormatter<t_CHAR>::parseDefault()
{
    ++d_numDefaultTimeZones;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void TimeZone_SpecifierFormatter<t_CHAR>::parseIso8601()
{
    ++d_numIso8601TimeZones;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool TimeZone_SpecifierFormatter<t_CHAR>::parseNextModifier(
                                                         StringView *specInOut)
{
    BSLS_ASSERT(!specInOut->empty());

    switch (specInOut->front()) {
      case t_CHAR(':'): {
        d_colon = true;
      } break;
      case t_CHAR('_'): {
        d_noColon = true;
      } break;
      case t_CHAR('Z'): {
        d_tzZ = true;
      } break;
      default: {
        return false;                                                 // RETURN
      }
    }

    if (d_colon && d_noColon) {
        BSLS_THROW(bsl::format_error(
                             "Illegal: both ':' and '_' modifiers specified"));
    }

    specInOut->remove_prefix(1);

    return true;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool TimeZone_SpecifierFormatter<t_CHAR>::parseNextSpecifier(
                                                         StringView *specInOut)
{
    BSLS_ASSERT(!specInOut->empty());

    if (t_CHAR('z') == specInOut->front()) {
        ++d_numDefaultTimeZones;
        specInOut->remove_prefix(1);

        return true;                                                  // RETURN
    }

    return false;
}

template <class t_CHAR>
inline
void TimeZone_SpecifierFormatter<t_CHAR>::postprocess(
                              const bslfmt::FormatSpecificationParser<t_CHAR>&)
{}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int TimeZone_SpecifierFormatter<t_CHAR>::extraSections() const
{
    return 0;
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR TimeZone_SpecifierFormatter<t_CHAR>::formatDefault(
                                              t_ITERATOR out, int offset) const
{
    return formatImp(out, offset, false);
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR TimeZone_SpecifierFormatter<t_CHAR>::formatIso8601(
                                              t_ITERATOR out, int offset) const
{
    return formatImp(out, offset, true);
}

template <class t_CHAR>
template <class t_ITERATOR>
bool TimeZone_SpecifierFormatter<t_CHAR>::formatNextSpecifier(
                                  bsl::basic_string_view<t_CHAR> *specInOut,
                                  t_ITERATOR                     *outIt,
                                  int                             offset) const
{
    BSLS_ASSERT_SAFE(!specInOut->empty());

    if (t_CHAR('z') == specInOut->front()) {
        *outIt = formatDefault(*outIt, offset);
        specInOut->remove_prefix(1);

        return true;                                                  // RETURN
    }

    return false;
}

template <class t_CHAR>
inline
int TimeZone_SpecifierFormatter<t_CHAR>::totalWidth(int offset) const
{
    enum { k_Z_TZ_WIDTH        = 1,
           k_COLON_TZ_WIDTH    = 6,
           k_NO_COLON_TZ_WIDTH = 5 };

    int totalNumTimeZones = d_numDefaultTimeZones + d_numIso8601TimeZones;

    return isTimeZoneZ(offset) ? k_Z_TZ_WIDTH        * totalNumTimeZones
         : d_colon             ? k_COLON_TZ_WIDTH    * totalNumTimeZones
         : d_noColon           ? k_NO_COLON_TZ_WIDTH * totalNumTimeZones

         :                       k_NO_COLON_TZ_WIDTH * d_numDefaultTimeZones +
                                 k_COLON_TZ_WIDTH    * d_numIso8601TimeZones;
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
