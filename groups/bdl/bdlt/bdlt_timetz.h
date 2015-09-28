// bdlt_timetz.h                                                      -*-C++-*-
#ifndef INCLUDED_BDLT_TIMETZ
#define INCLUDED_BDLT_TIMETZ

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a representation of a time with time zone offset.
//
//@CLASSES:
//  bdlt::TimeTz: local-time value with time zone offset from UTC
//
//@SEE_ALSO: bdlt_time, bdlt_datetimetz
//
//@DESCRIPTION: This component provides a single, simply constrained
// value-semantic class, 'bdlt::TimeTz', that represents a time value in a
// particular time zone.  Each 'bdlt::TimeTz' object contains a time zone
// offset from UTC (in minutes) and a 'bdlt::Time' value in that time zone.
// For logical consistency, the time value and offset should correspond to a
// geographically valid time zone, but such consistency is the user's
// responsibility.  This component does not enforce logical constraints on any
// values.
//
// The 'localTime' and 'utcTime' methods return 'bdlt::Time' values
// corresponding to the local time and UTC time represented by the object,
// respectively.  In addition, the 'offset' method returns the time zone offset
// in minutes from UTC (i.e., 'UTC + offset' equals local time).
//
///Attributes
///----------
//..
//  Name                Type         Default         Simple Constraints
//  ------------------  -----------  --------------  ------------------
//  localTime           bdlt::Time   '24:00:00.000'  none
//  offset              int          0               ( -1440 .. 1440 )
//..
//: o 'localTime': local time in the timezone described by 'offset'.
//:
//: o 'offset': offset from UTC (in minutes) of the time zone in which
//:   'localTime' occurs.
//
///Caveats on Time Zone Support
///----------------------------
// A 'bdlt::TimeTz' value is intended to be interpreted as a value in a local
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
// 'bdlt::TimeTz' representations.
//
// First, we define three 'bdlt::TimeTz' objects representing the time in three
// different time zones on the same (local) date:
//..
//  bdlt::TimeTz newYorkTime(bdlt::Time(9, 30, 0, 0.0),
//                           -5 * bdlt::TimeUnitRatio::k_MINUTES_PER_HOUR);
//  bdlt::TimeTz chicagoTime(bdlt::Time(8, 30, 0, 0.0),
//                           -6 * bdlt::TimeUnitRatio::k_MINUTES_PER_HOUR);
//  bdlt::TimeTz phoenixTime(bdlt::Time(6, 30, 0, 0.0),
//                           -7 * bdlt::TimeUnitRatio::k_MINUTES_PER_HOUR);
//..
// Then, we observe that the local times are distinct:
//..
//  assert(newYorkTime.localTime() != chicagoTime.localTime());
//  assert(chicagoTime.localTime() != phoenixTime.localTime());
//  assert(phoenixTime.localTime() != newYorkTime.localTime());
//..
// Next, we observe that 'newYorkTime' and 'chicagoTime' actually represent the
// same point in time:
//..
//  assert(newYorkTime.utcTime() == chicagoTime.utcTime());
//..
// Finally, we observe that 'phoenixTime' is one hour earlier than
// 'newYorkTime':
//..
//  bdlt::DatetimeInterval delta =
//                               newYorkTime.utcTime() - phoenixTime.utcTime();
//
//  assert(0 == delta.days());
//  assert(1 == delta.hours());
//  assert(0 == delta.minutes());
//  assert(0 == delta.seconds());
//  assert(0 == delta.milliseconds());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_TIME
#include <bdlt_time.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif


namespace BloombergLP {
namespace bdlt {

                                // ============
                                // class TimeTz
                                // ============

class TimeTz {
    // This value-semantic class describes a time value in a particular time
    // zone, which is indicated using an offset from UTC (in minutes).  The
    // offset is available via the 'offset' method, and is defined by the
    // relationship: 'localTime() - offset() == utcTime'.  The time and offset
    // values are logically assumed to correspond to geographically valid
    // values, however, this constraint is not enforced.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //: o supports BDEX streaming
    // For terminology see 'bsldoc_glossary'.

    // PRIVATE TYPES
    enum ValidOffsetRange {
        // This enumeration specifies the minimum and maximum time zone offset
        // values.

        k_MAX_OFFSET =  1440,
        k_MIN_OFFSET = -1440
    };

    // DATA
    Time d_localTime;    // time value in timezone specified by 'd_offset'
    int  d_offset;       // offset from UTC (in minutes)

  public:
    // CLASS METHODS
    static bool isValid(const Time& localTime, int offset);
        // Return 'true' if the specified 'localTime' and the specified time
        // zone 'offset' represent a valid 'TimeTz' value, and 'false'
        // otherwise.  A 'localTime' and 'offset' represent a valid 'TimeTz'
        // value if 'offset' is in the range '( -1440 .. 1440 )', and 'offset'
        // is 0 if 'localTime' has the value '24:00:00.000'.  Note that a
        // 'true' result from this function does not guarantee that 'offset'
        // corresponds to any geographical or historical time zone.  Also note
        // that a 'true' result from this function does not guarantee that
        // 'localTime' itself is a valid 'Time' object.

                               // BDEX Streaming

    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that it is highly recommended that 'versionSelector'
        // be formatted as "YYYYMMDD", a date representation.  Also note that
        // 'versionSelector' should be a *compile*-time-chosen value that
        // selects a format version supported by both externalizer and
        // unexternalizer.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    // CREATORS
    TimeTz();
        // Create a 'TimeTz' object having the (default) attribute values.

    TimeTz(const Time& localTime, int offset);
        // Create a 'TimeTz' object whose local time and offset attributes have
        // the specified 'localTime' and 'offset' values respectively.  The
        // behavior is undefined unless 'offset' is in the range
        // '( -1440 .. 1440 )', and 'offset' is 0 if 'localTime' has the value
        // '24:00:00.000'.  Note that this method provides no validation, and
        // it is the user's responsibility to ensure that 'offset' represents a
        // valid time zone and that 'localTime' represents a valid time in that
        // time zone.

    TimeTz(const TimeTz& original);
        // Construct a 'TimeTz' object having the same value as the specified
        // 'original' 'TimeTz' object.

    ~TimeTz();
        // Destroy this object.

    // MANIPULATORS
    TimeTz& operator=(const TimeTz& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setTimeTz(const Time& localTime, int offset);
        // Set the local time and time zone offset attributes of this object to
        // the specified 'localTime' and 'offset' values respectively.  The
        // behavior is undefined unless 'offset' is in the range
        // '( -1440 .. 1440 )'.  Note that this method provides no validation,
        // and it is the user's responsibility to assure the consistency of the
        // resulting value.

    int setTimeTzIfValid(const Time& localTime, int offset);
        // Set the local time and the time zone offset of this object to the
        // specified 'localTime' and 'offset' values respectively if
        // 'localTime' and 'offset' represent a valid 'TimeTz' value, and leave
        // the object unmodified otherwise.  Return 0 on success, and a
        // non-zero value otherwise.

                                  // Aspects

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, this object
        // is unaltered and 'stream' is invalidated, but otherwise unmodified.
        // If 'version' is supported but 'stream' becomes invalid during this
        // operation, this object has an undefined, but valid, state.  Note
        // that no version is read from 'stream'.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    // ACCESSORS
    Time localTime() const;
        // Return a 'Time' object having the value of the local time attribute
        // of this object.  Note that the 'Time' value returned is the value
        // stored in this object, and may be different from the local time of
        // the system.

    int offset() const;
        // Return the time zone offset of this object in minutes from UTC.

    Time utcTime() const;
        // Return a 'Time' object having the value of the UTC time represented
        // by this object.  Note that the returned value is equal to
        // 'localTime() - offset()' minutes.

                                  // Aspects

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object, using the specified 'version'
        // format, to the specified output 'stream', and return a reference to
        // 'stream'.  If 'stream' is initially invalid, this operation has no
        // effect.  If 'version' is not supported, 'stream' is invalidated, but
        // otherwise unmodified.  Note that 'version' is not written to
        // 'stream'.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the format
        // is not fully specified, and can change without notice.

};

// FREE OPERATORS
bool operator==(const TimeTz& lhs, const TimeTz& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'TimeTz' objects have the same value
    // if their corresponding 'localTime' and 'offset' attributes have the same
    // values.

bool operator!=(const TimeTz& lhs, const TimeTz& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'TimeTz' objects do not have the
    // same value if any of their corresponding 'localTime' and 'offset'
    // attributes have different values.

bsl::ostream& operator<<(bsl::ostream& stream, const TimeTz& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.  Also note that this
    // method has the same behavior as 'object.print(stream, 0, -1)', but with
    // the attribute names elided.

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
int TimeTz::maxSupportedBdexVersion(int /* versionSelector */)
{
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
    BSLS_ASSERT_SAFE(isValid(localTime, offset));
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
    BSLS_ASSERT_SAFE(isValid(d_localTime, d_offset));
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
    BSLS_ASSERT_SAFE(isValid(localTime, offset));

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
          case 1: {
            Time time;
            time.bdexStreamIn(stream, 1);

            int offset;
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
        // N.B. adding -0 minutes to a default-constructed 'Time' object (with
        // value '24:00:00.000') would convert it to a 'Time' object with value
        // '00:00:00.000'.  The branch here preserves the 'localTime' attribute
        // for a default-constructed 'TimeTz' object.

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
          case 1: {
            d_localTime.bdexStreamOut(stream, 1);
            stream.putInt32(d_offset);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}


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

}  // close enterprise namespace

namespace bsl {

// TRAITS
template <>
struct is_trivially_copyable<BloombergLP::bdlt::TimeTz> : bsl::true_type {
    // This template specialization for 'is_trivially_copyable' indicates that
    // 'bdlt::TimeTz' is a trivially copyable type.
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
