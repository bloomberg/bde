// bdlt_datetz.h                                                      -*-C++-*-
#ifndef INCLUDED_BDLT_DATETZ
#define INCLUDED_BDLT_DATETZ

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a representation of a date with time zone offset.
//
//@CLASSES:
//  bdlt::DateTz: local-date value with time zone offset from UTC
//
//@SEE_ALSO: bdlt_date, bdlt_datetimetz
//
//@DESCRIPTION: This component provides a single value-semantic class,
// 'bdlt::DateTz', that represents a date value in a particular time zone.
// Each 'bdlt::DateTz' object contains a time zone offset from UTC (in minutes)
// and a 'bdlt::Date' value in that time zone.  For logical consistency, the
// date value and offset should correspond to a geographically valid time zone,
// but such consistency is the user's responsibility.  This component does not
// enforce logical constraints on any values.
//
///Caveats on Time Zone Support
///----------------------------
// A 'bdlt::DateTz' value is intended to be interpreted as a value in a local
// time zone, along with the offset of that value from UTC.  However, there are
// some problems with this simple interpretation.  First of all, the offset
// value may not correspond to any time zone that has ever existed.  For
// example, the offset value could be set to one minute, or to 1,234 minutes.
// The meaning of the resulting "local time" value is always clear, but the
// local time might not correspond to any geographical or historical time zone.
//
// The second problem is more subtle.  A given offset from UTC might be "valid"
// in that it corresponds to a real time zone, but the actual date value might
// not exist in that time zone.  To make matters worse, a "valid" offset may
// not (indeed, rarely will) specify one time zone uniquely.  Moreover, the
// date value might be valid in one time zone corresponding to a given offset,
// and not in another time zone.
//
// For these reasons (and others), this component cannot and does not perform
// any validation relating to time zones or offsets.  The user must take care
// to honor the "local date" contract of this component.
//
///ISO Standard Text Representation
///--------------------------------
// A common standard text representation of a date and time value is described
// by ISO 8601.  BDE provides the 'bdlt_iso8601util' component for conversion
// to and from the standard ISO8601 format.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Representing Dates In Different Time Zones
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we need to compare dates in different time zones.  The
// 'bdlt::DateTz' type helps us to accomplish this by providing the
// 'utcStartTime' method, which returns a 'bdlt::Datetime' value corresponding
// to the UTC "point in time" when the local date starts (i.e. 000 hours local
// time).
//
// First, we default construct an object 'dateTz1', which has an offset of 0,
// implying that the object represents a date in the UTC time zone.
//..
//  bdlt::DateTz dateTz1;
//  assert(0                   == dateTz1.offset());
//  assert(dateTz1.localDate() == dateTz1.utcStartTime().date());
//  assert(dateTz1.localDate() == bdlt::Date());
//..
// Notice the value of a default contructed 'bdlt::DateTz' object is the same
// as that of a default constructed 'bdlt::Date' object.
//
// Then, we construct two objects 'dateTz2' and 'dateTz3' to have a local date
// of 2013/12/31 in the EST time zone (UTC-5) and the pacific time zone (UTC-8)
// respectively:
//..
//  bdlt::DateTz dateTz2 (bdlt::Date(2013, 12, 31), -5 * 60);
//  bdlt::DateTz dateTz3 (bdlt::Date(2013, 12, 31), -8 * 60);
//..
// Next, we compare the local dates of the two 'DateTz' objects, and verify
// that they compare equal:
//..
//  bdlt::Date localDate(2013, 12, 31);
//  assert(localDate == dateTz2.localDate());
//  assert(localDate == dateTz3.localDate());
//..
// Now, we compare the starting time of the two 'DateTz' objects using the
// 'utcStartTime' method:
//..
//  assert(dateTz2.utcStartTime() < dateTz3.utcStartTime());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
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
                             // class DateTz
                             // ============

class DateTz {
    // This value-semantic class describes a date value in a particular time
    // zone, which is indicated using an offset from UTC (in minutes).

    // PRIVATE TYPES
    enum ValidOffsetRange {
        // This enumeration specifies the minimum and maximum time zone offset
        // values.

        k_MAX_OFFSET =  1440,
        k_MIN_OFFSET = -1440
    };

    // DATA
    Date d_localDate;  // date value in timezone specified by 'd_offset'
    int  d_offset;     // offset from UTC (in minutes)

  public:
    // CLASS METHODS
    static bool isValid(const Date& localDate, int offset);
        // Return 'true' if the specified 'localDate' and the specified time
        // zone 'offset' represent a valid 'DateTz' value, and 'false'
        // otherwise.  A 'localDate' and 'offset' represent a valid 'DateTz'
        // value if 'offset' is in the range '( -1440 .. 1440 )'.  Note that a
        // 'true' result from this function does not guarantee that 'offset'
        // corresponds to any geographical or historical time zone.  Also note
        // that a 'true' result from this function does not guarantee that
        // 'localDate' itself is a valid 'Date' object.

                                  // Aspects

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
    DateTz();
        // Create a 'DateTz' object having the (default) attribute values.

    DateTz(const Date& localDate, int offset);
        // Create a 'DateTz' object having a local date value equal to the
        // specified 'localDate' and a time zone offset value from UTC equal to
        // the specified 'offset' (in minutes).  The behavior is undefined
        // unless 'offset' is in the range '( -1440 .. 1440 )'.  Note that this
        // method provides no validation, and it is the user's responsibility
        // to ensure that 'offset' represents a valid time zone and that
        // 'localDate' represents a valid date in that time zone.

    DateTz(const DateTz& original);
        // Create a 'DateTz' object having the same value as the specified
        // 'original' object.

    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).
    ~DateTz();
        // Destroy this object.

    // MANIPULATORS
    DateTz& operator=(const DateTz& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setDateTz(const Date& localDate, int offset);
        // Set the local date and the time zone offset of this object to the
        // specified 'localDate' and 'offset' values respectively.  The
        // behavior is undefined unless 'offset' is in the range
        // '( -1440 .. 1440 )'.  Note that this method provides no validation,
        // and it is the user's responsibility to assure the consistency of the
        // resulting value.

    int setDateTzIfValid(const Date& localDate, int offset);
        // Set the local date and time zone offset of this object to the
        // specified 'localDate' and 'offset' values respectively if
        // 'localDate' and 'offset' represent a valid 'DateTz' value.  Return 0
        // on success, and a non-zero value with no effect on this 'DateTz'
        // object otherwise.

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
    Date localDate() const;
        // Return a 'Date' object having the value of the local date
        // represented by this object.  Note that this is the 'Date' supplied
        // at construction and may not correspond to the actual time zone
        // offset of the local system.

    int offset() const;
        // Return the time zone offset of this 'DateTz' object.  Note that the
        // offset is in minutes from UTC.

    Datetime utcStartTime() const;
        // Return a 'Datetime' object having the value of the UTC "point in
        // time" when the local date starts (i.e., 0000 hours local time).  The
        // behavior is undefined unless the local date starting time represents
        // a valid 'Datetime' value for the UTC timezone.  Note that the
        // returned value is equal to:
        //..
        //  Datetime(localDate()).addMinutes(-offset());
        //..

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
bool operator==(const DateTz& lhs, const DateTz& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'DateTz' objects have the
    // same value, and 'false' otherwise.  Two 'DateTz' objects have the same
    // value if they have the same local date value and the same time zone
    // offset value.

bool operator!=(const DateTz& lhs, const DateTz& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'DateTz' objects do not
    // have the same value, and 'false' otherwise.  Two 'DateTz' objects do not
    // have the same value if they do not have the same local date values or
    // the same time zone offset values.

bsl::ostream& operator<<(bsl::ostream& stream, const DateTz& rhs);
    // Write the value of the specified 'rhs' object to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.  Also note that this
    // method has the same behavior as 'object.print(stream, 0, -1)', but with
    // the attribute names elided.

// FREE FUNCTIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const DateTz& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'DateTz'.  Note that two
    // objects which represent the same UTC time but have different offsets
    // will not (necessarily) hash to the same value.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                                // ------------
                                // class DateTz
                                // ------------

// CLASS METHODS
inline
bool DateTz::isValid(const Date&, int offset)
{
     return k_MIN_OFFSET < offset
         && k_MAX_OFFSET > offset;
}

                                  // Aspects

inline
int DateTz::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// CREATORS
inline
DateTz::DateTz()
: d_localDate()
, d_offset(0)
{
}

inline
DateTz::DateTz(const Date& localDate, int offset)
: d_localDate(localDate)
, d_offset(offset)
{
    BSLS_ASSERT_SAFE(isValid(localDate, offset));
}

inline
DateTz::DateTz(const DateTz& original)
: d_localDate(original.d_localDate)
, d_offset(original.d_offset)
{
}

inline
DateTz::~DateTz()
{
    BSLS_ASSERT_SAFE(isValid(d_localDate, d_offset));
}

// MANIPULATORS
inline
DateTz& DateTz::operator=(const DateTz& rhs)
{
    d_localDate = rhs.d_localDate;
    d_offset    = rhs.d_offset;

    return *this;
}

inline
void DateTz::setDateTz(const Date& localDate, int offset)
{
    BSLS_ASSERT_SAFE(isValid(localDate, offset));

    d_localDate = localDate;
    d_offset    = offset;
}

inline
int DateTz::setDateTzIfValid(const Date& localDate, int offset)
{
    if (isValid(localDate, offset)) {
        setDateTz(localDate, offset);
        return 0;                                                     // RETURN
    }
    return -1;
}

                                  // Aspects

template <class STREAM>
STREAM& DateTz::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            Date localDate;
            localDate.bdexStreamIn(stream, 1);

            int offset;
            stream.getInt32(offset);

            if (stream && isValid(localDate, offset)) {
                d_localDate = localDate;
                d_offset    = offset;
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
Date DateTz::localDate() const
{
    return d_localDate;
}

inline
int DateTz::offset() const
{
    return d_offset;
}

inline
Datetime DateTz::utcStartTime() const
{
    Datetime utc(d_localDate, Time(0,0,0,0));
    utc.addMinutes(-d_offset);
    return utc;
}

                                  // Aspects

template <class STREAM>
STREAM& DateTz::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            d_localDate.bdexStreamOut(stream, 1);
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
bool bdlt::operator==(const DateTz& lhs, const DateTz& rhs)
{
    return lhs.offset()    == rhs.offset()
        && lhs.localDate() == rhs.localDate();
}

inline
bool bdlt::operator!=(const DateTz& lhs, const DateTz& rhs)
{
    return lhs.offset()    != rhs.offset()
        || lhs.localDate() != rhs.localDate();
}

inline
bsl::ostream& bdlt::operator<<(bsl::ostream& stream, const DateTz& rhs)
{
    return rhs.print(stream, 0, -1);
}

// FREE FUNCTIONS
template <class HASHALG>
inline
void bdlt::hashAppend(HASHALG& hashAlg, const DateTz& object)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlg, object.localDate());
    hashAppend(hashAlg, object.offset());
}

}  // close enterprise namespace

namespace bsl {

// TRAITS
template <>
struct is_trivially_copyable<BloombergLP::bdlt::DateTz> : bsl::true_type {
    // This template specialization for 'is_trivially_copyable' indicates that
    // 'bdlt::DateTz' is a trivially copyable type.

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
