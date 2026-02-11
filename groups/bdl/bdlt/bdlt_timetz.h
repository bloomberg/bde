// bdlt_timetz.h                                                      -*-C++-*-
#ifndef INCLUDED_BDLT_TIMETZ
#define INCLUDED_BDLT_TIMETZ

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a representation of a time with time zone offset.
//
//@CLASSES:
//  bdlt::TimeTz: local-time value with time zone offset from UTC
//
//@SEE_ALSO: bdlt_time, bdlt_datetimetz
//
//@DESCRIPTION: This component provides a single, simply constrained
// value-semantic class, `bdlt::TimeTz`, that represents a time value in a
// particular time zone.  Each `bdlt::TimeTz` object contains a time zone
// offset from UTC (in minutes) and a `bdlt::Time` value in that time zone.
// For logical consistency, the time value and offset should correspond to a
// geographically valid time zone, but such consistency is the user's
// responsibility.  This component does not enforce logical constraints on any
// values.
//
// The `localTime` and `utcTime` methods return `bdlt::Time` values
// corresponding to the local time and UTC time represented by the object,
// respectively.  In addition, the `offset` method returns the time zone offset
// in minutes from UTC (i.e., `UTC + offset` equals local time).
//
///Attributes
///----------
// ```
// Name                Type         Default         Simple Constraints
// ------------------  -----------  --------------  ------------------
// localTime           bdlt::Time   '24:00:00.000'  none
// offset              int          0               ( -1440 .. 1440 )
// ```
// * `localTime`: local time in the timezone described by `offset`.
// * `offset`: offset from UTC (in minutes) of the time zone in which
//   `localTime` occurs.
//
///Caveats on Time Zone Support
///----------------------------
// A `bdlt::TimeTz` value is intended to be interpreted as a value in a local
// time zone, along with the offset of that value from UTC.  However, there are
// some problems with this simple interpretation.  First of all, the offset
// value may not correspond to any time zone that has ever existed.  For
// example, the offset value could be set to one minute, or to 1,234 minutes.
// The meaning of the resulting "local time" value is always clear, but the
// local time might not correspond to any geographical or historical time zone.
//
// For these reasons (and others), this component cannot and does not perform
// any validation relating to time zones or offsets.  The user must take care
// to honor the "local time" contract of this component.
//
///ISO Standard Text Representation
///--------------------------------
// A common standard text representation of a date and time value is described
// by ISO 8601.  BDE provides the `bdlt_iso8601util` component for conversion
// to and from the standard ISO8601 format.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Comparing Times from Multiple Time Zones
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Some legacy systems may represent points in time as a combination of a local
// time-of-day plus an offset from UTC, with an underlying assumption that the
// dates on which points in time occur can be inferred from context.  Assuming
// that we know that two such times fall on the same (local) calendar date, we
// can determine whether or not the two times coincide by comparing their
// `bdlt::TimeTz` representations.
//
// First, we define three `bdlt::TimeTz` objects representing the time in three
// different time zones on the same (local) date:
// ```
// bdlt::TimeTz newYorkTime(bdlt::Time(9, 30, 0, 0.0),
//                          -5 * bdlt::TimeUnitRatio::k_MINUTES_PER_HOUR);
// bdlt::TimeTz chicagoTime(bdlt::Time(8, 30, 0, 0.0),
//                          -6 * bdlt::TimeUnitRatio::k_MINUTES_PER_HOUR);
// bdlt::TimeTz phoenixTime(bdlt::Time(6, 30, 0, 0.0),
//                          -7 * bdlt::TimeUnitRatio::k_MINUTES_PER_HOUR);
// ```
// Then, we observe that the local times are distinct:
// ```
// assert(newYorkTime.localTime() != chicagoTime.localTime());
// assert(chicagoTime.localTime() != phoenixTime.localTime());
// assert(phoenixTime.localTime() != newYorkTime.localTime());
// ```
// Next, we observe that `newYorkTime` and `chicagoTime` actually represent the
// same point in time:
// ```
// assert(newYorkTime.utcTime() == chicagoTime.utcTime());
// ```
// Finally, we observe that `phoenixTime` is one hour earlier than
// `newYorkTime`:
// ```
// bdlt::DatetimeInterval delta =
//                              newYorkTime.utcTime() - phoenixTime.utcTime();
//
// assert(0 == delta.days());
// assert(1 == delta.hours());
// assert(0 == delta.minutes());
// assert(0 == delta.seconds());
// assert(0 == delta.milliseconds());
// ```

#include <bdlscm_version.h>

#include <bdlt_formatter.h>
#include <bdlt_time.h>
#include <bdlt_timetz_specifierformatter.h>

#include <bslh_hash.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isbitwisecopyable.h>

#include <bsls_annotation.h>
#include <bsls_assert.h>
#include <bsls_review.h>

#include <bsl_iosfwd.h>


namespace BloombergLP {
namespace bdlt {

                                 // ============
                                 // class TimeTz
                                 // ============

/// This value-semantic class describes a time value in a particular time
/// zone, which is indicated using an offset from UTC (in minutes).  The
/// offset is available via the `offset` method, and is defined by the
/// relationship: `localTime() - offset() == utcTime`.  The time and offset
/// values are logically assumed to correspond to geographically valid
/// values, however, this constraint is not enforced.
///
/// This class:
/// * supports a complete set of *value-semantic* operations
/// * supports BDEX streaming
/// For terminology see `bsldoc_glossary`.
class TimeTz {
    // PRIVATE TYPES

    /// This enumeration specifies the minimum and maximum time zone offset
    /// values.
    enum ValidOffsetRange {

        k_MAX_OFFSET =  1440,
        k_MIN_OFFSET = -1440
    };

    // DATA
    Time d_localTime;    // time value in timezone specified by `d_offset`
    int  d_offset;       // offset from UTC (in minutes)

  public:
    // CLASS METHODS

    /// Return `true` if the specified `localTime` and the specified time
    /// zone `offset` represent a valid `TimeTz` value, and `false`
    /// otherwise.  A `localTime` and `offset` represent a valid `TimeTz`
    /// value if `offset` is in the range `( -1440 .. 1440 )`, and `offset`
    /// is 0 if `localTime` has the value `24:00:00.000`.  Note that a
    /// `true` result from this function does not guarantee that `offset`
    /// corresponds to any geographical or historical time zone.  Also note
    /// that a `true` result from this function does not guarantee that
    /// `localTime` itself is a valid `Time` object.
    static bool isValid(const Time& localTime, int offset);

                               // BDEX Streaming

    /// Return the maximum valid BDEX format version, as indicated by the
    /// specified `versionSelector`, to be passed to the `bdexStreamOut`
    /// method.  Note that it is highly recommended that `versionSelector`
    /// be formatted as "YYYYMMDD", a date representation.  Also note that
    /// `versionSelector` should be a *compile*-time-chosen value that
    /// selects a format version supported by both externalizer and
    /// unexternalizer.  See the `bslx` package-level documentation for more
    /// information on BDEX streaming of value-semantic types and
    /// containers.
    static int maxSupportedBdexVersion(int versionSelector);

    // CREATORS

    /// Create a `TimeTz` object having the (default) attribute values.
    TimeTz();

    /// Create a `TimeTz` object whose local time and offset attributes have
    /// the specified `localTime` and `offset` values respectively.  The
    /// behavior is undefined unless `offset` is in the range
    /// `( -1440 .. 1440 )`, and `offset` is 0 if `localTime` has the value
    /// `24:00:00.000`.  Note that this method provides no validation, and
    /// it is the user's responsibility to ensure that `offset` represents a
    /// valid time zone and that `localTime` represents a valid time in that
    /// time zone.
    TimeTz(const Time& localTime, int offset);

    /// Construct a `TimeTz` object having the same value as the specified
    /// `original` `TimeTz` object.
    TimeTz(const TimeTz& original);

    /// Destroy this object.
    ~TimeTz();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.
    TimeTz& operator=(const TimeTz& rhs);

    /// Set the local time and time zone offset attributes of this object to
    /// the specified `localTime` and `offset` values respectively.  The
    /// behavior is undefined unless `offset` is in the range
    /// `( -1440 .. 1440 )`.  Note that this method provides no validation,
    /// and it is the user's responsibility to assure the consistency of the
    /// resulting value.
    void setTimeTz(const Time& localTime, int offset);

    /// Set the local time and the time zone offset of this object to the
    /// specified `localTime` and `offset` values respectively if
    /// `localTime` and `offset` represent a valid `TimeTz` value, and leave
    /// the object unmodified otherwise.  Return 0 on success, and a
    /// non-zero value otherwise.
    int setTimeTzIfValid(const Time& localTime, int offset);

                                  // Aspects

    /// Assign to this object the value read from the specified input
    /// `stream` using the specified `version` format, and return a
    /// reference to `stream`.  If `stream` is initially invalid, this
    /// operation has no effect.  If `version` is not supported, this object
    /// is unaltered and `stream` is invalidated, but otherwise unmodified.
    /// If `version` is supported but `stream` becomes invalid during this
    /// operation, this object has an undefined, but valid, state.  Note
    /// that no version is read from `stream`.  See the `bslx` package-level
    /// documentation for more information on BDEX streaming of
    /// value-semantic types and containers.
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);

    // ACCESSORS

    /// Return a `Time` object having the value of the local time attribute
    /// of this object.  Note that the `Time` value returned is the value
    /// stored in this object, and may be different from the local time of
    /// the system.
    Time localTime() const;

    /// Return the time zone offset of this object in minutes from UTC.
    int offset() const;

    /// Return a `Time` object having the value of the UTC time represented
    /// by this object.  Note that the returned value is equal to
    /// `localTime() - offset()` minutes.
    Time utcTime() const;

                                  // Aspects

    /// Write the value of this object, using the specified `version`
    /// format, to the specified output `stream`, and return a reference to
    /// `stream`.  If `stream` is initially invalid, this operation has no
    /// effect.  If `version` is not supported, `stream` is invalidated, but
    /// otherwise unmodified.  Note that `version` is not written to
    /// `stream`.  See the `bslx` package-level documentation for more
    /// information on BDEX streaming of value-semantic types and
    /// containers.
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a reference to `stream`.
    /// Optionally specify an initial indentation `level`, whose absolute
    /// value is incremented recursively for nested objects.  If `level` is
    /// specified, optionally specify `spacesPerLevel`, whose absolute value
    /// indicates the number of spaces per indentation level for this and
    /// all of its nested objects.  If `level` is negative, suppress
    /// indentation of the first line.  If `spacesPerLevel` is negative,
    /// format the entire output on one line, suppressing all but the
    /// initial indentation (as governed by `level`).  If `stream` is not
    /// valid on entry, this operation has no effect.  Note that the format
    /// is not fully specified, and can change without notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation

    // DEPRECATED

    /// Return a `Time` object having the value of the UTC time represented
    /// by this object.  Note that the returned value is equal to
    /// `localTime() - offset()` minutes.
    ///
    /// @DEPRECATED: replaced by `utcTime`.
    Time gmtTime() const;

    /// Return the most current BDEX streaming version number supported by
    /// this class.
    ///
    /// @DEPRECATED: Use `maxSupportedBdexVersion(int)` instead.
    static int maxSupportedBdexVersion();

    /// Set the local time and the time zone offset of this object to the
    /// specified `localTime` and `offset` values respectively if
    /// `localTime` and `offset` represent a valid `TimeTz` value.  Return 0
    /// on success, and a non-zero value with no effect on this object
    /// otherwise.
    ///
    /// @DEPRECATED: replaced by `setTimeTzIfValid`.
    int validateAndSetTimeTz(const Time& localTime, int offset);

#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `TimeTz` objects have the same value
/// if their corresponding `localTime` and `offset` attributes have the same
/// values.
bool operator==(const TimeTz& lhs, const TimeTz& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  Two `TimeTz` objects do not have the
/// same value if any of their corresponding `localTime` and `offset`
/// attributes have different values.
bool operator!=(const TimeTz& lhs, const TimeTz& rhs);

/// Write the value of the specified `object` to the specified output
/// `stream` in a single-line format, and return a reference providing
/// modifiable access to `stream`.  If `stream` is not valid on entry, this
/// operation has no effect.  Note that this human-readable format is not
/// fully specified and can change without notice.  Also note that this
/// method has the same behavior as `object.print(stream, 0, -1)`, but with
/// the attribute names elided.
bsl::ostream& operator<<(bsl::ostream& stream, const TimeTz& object);

// FREE FUNCTIONS

/// Pass the specified `object` to the specified `hashAlg`.  This function
/// integrates with the `bslh` modular hashing system and effectively
/// provides a `bsl::hash` specialization for `TimeTz`.  Note that two
/// objects which represent the same UTC time but have different offsets
/// will not (necessarily) hash to the same value.
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const TimeTz& object);

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                             // ------------
                             // class TimeTz
                             // ------------

// CLASS METHODS
inline
bool TimeTz::isValid(const Time& localTime, int offset)
{
    return offset > k_MIN_OFFSET
        && offset < k_MAX_OFFSET
        && (bdlt::Time() != localTime || 0 == offset);
}

                                  // Aspects

inline
int TimeTz::maxSupportedBdexVersion(int versionSelector)
{
    if (versionSelector >= 20170401) {
        return 2;                                                     // RETURN
    }
    return 1;
}

// CREATORS
inline
TimeTz::TimeTz()
: d_localTime()
, d_offset(0)
{
}

inline
TimeTz::TimeTz(const Time& localTime, int offset)
: d_localTime(localTime)
, d_offset(offset)
{
    BSLS_REVIEW(isValid(localTime, offset));
}

inline
TimeTz::TimeTz(const TimeTz& original)
: d_localTime(original.d_localTime)
, d_offset(original.d_offset)
{
}

inline
TimeTz::~TimeTz()
{
    BSLS_REVIEW(isValid(d_localTime, d_offset));
}

// MANIPULATORS
inline
TimeTz& TimeTz::operator=(const TimeTz& rhs)
{
    d_localTime = rhs.d_localTime;
    d_offset    = rhs.d_offset;

    return *this;
}

inline
void TimeTz::setTimeTz(const Time& localTime, int offset)
{
    BSLS_REVIEW(isValid(localTime, offset));

    d_localTime = localTime;
    d_offset    = offset;
}

inline
int TimeTz::setTimeTzIfValid(const Time& localTime, int offset)
{
    if (isValid(localTime, offset)) {
        setTimeTz(localTime, offset);

        return 0;                                                     // RETURN
    }
    return -1;
}

                                  // Aspects

template <class STREAM>
STREAM& TimeTz::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 2:
            BSLS_ANNOTATION_FALLTHROUGH;
          case 1: {
            Time time;
            time.bdexStreamIn(stream, version);

            int offset = 0;
            stream.getInt32(offset);

            if (stream && isValid(time, offset)) {
                setTimeTz(time, offset);
            }
            else {
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

// ACCESSORS
inline
Time TimeTz::localTime() const
{
    return d_localTime;
}

inline
int TimeTz::offset() const
{
    return d_offset;
}

inline
Time TimeTz::utcTime() const
{
    Time utc(d_localTime);

    if (d_offset) {
        // N.B. adding -0 minutes to a default-constructed `Time` object (with
        // value '24:00:00.000') would convert it to a `Time` object with value
        // '00:00:00.000'.  The branch here preserves the `localTime` attribute
        // for a default-constructed `TimeTz` object.

        utc.addMinutes(-d_offset);
    }

    return utc;
}

                                  // Aspects

template <class STREAM>
STREAM& TimeTz::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 2:
            BSLS_ANNOTATION_FALLTHROUGH;
          case 1: {
            d_localTime.bdexStreamOut(stream, version);
            stream.putInt32(d_offset);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation
// DEPRECATED
inline
Time TimeTz::gmtTime() const
{
    return utcTime();
}

inline
int TimeTz::maxSupportedBdexVersion()
{
    return maxSupportedBdexVersion(0);
}

inline
int TimeTz::validateAndSetTimeTz(const Time& localTime, int offset)
{
    return setTimeTzIfValid(localTime, offset);
}
#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const TimeTz& lhs, const TimeTz& rhs)
{
    return lhs.localTime() == rhs.localTime()
        && lhs.offset()    == rhs.offset();
}

inline
bool bdlt::operator!=(const TimeTz& lhs, const TimeTz& rhs)
{
    return lhs.localTime() != rhs.localTime()
        || lhs.offset()    != rhs.offset();
}

inline
bsl::ostream& bdlt::operator<<(bsl::ostream& stream, const TimeTz& object)
{
    return object.print(stream, 0, -1);
}

// FREE FUNCTIONS
template <class HASHALG>
inline
void bdlt::hashAppend(HASHALG& hashAlg, const TimeTz& object)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlg, object.localTime());
    hashAppend(hashAlg, object.offset());
}

namespace bslmf {

// TRAITS

/// This template specialization for `IsBitwiseCopyable` indicates that
/// `bdlt::TimeTz` is a bitwise copyable type.
template <>
struct IsBitwiseCopyable<BloombergLP::bdlt::TimeTz> : bsl::true_type {
};

}  // close namespace bslmf
}  // close enterprise namespace

namespace bsl {

/// This type implements the formatter logic specific for `TimeTz` objects.
template <class t_CHAR>
class formatter<BloombergLP::bdlt::TimeTz, t_CHAR> {
    // PRIVATE TYPES
    typedef BloombergLP::bdlt::TimeTz                           TimeTz;
    typedef BloombergLP::bdlt::TimeTz_SpecifierFormatter_Cache  FormatCache;

    // DATA
    BloombergLP::bdlt::Formatter<
           BloombergLP::bdlt::TimeTz_SpecifierFormatter,
           t_CHAR>                                          d_formatter;

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
// Copyright 2014 Bloomberg Finance L.P.
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
