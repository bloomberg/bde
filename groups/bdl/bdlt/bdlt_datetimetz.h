// bdlt_datetimetz.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLT_DATETIMETZ
#define INCLUDED_BDLT_DATETIMETZ

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a representation of a date and time with time zone offset.
//
//@CLASSES:
//  bdlt::DatetimeTz: local-datetime value with time zone offset from UTC
//
//@SEE_ALSO: bdlt_datetime
//
//@DESCRIPTION: This component provides a single value-semantic class,
// 'bdlt::DatetimeTz', that represents a datetime value in a particular time
// zone.  Each 'bdlt::DatetimeTz' object contains a time zone offset from UTC
// (in minutes) and a 'bdlt::Datetime' value in that time zone.  For logical
// consistency, the datetime value and offset should correspond to a
// geographically valid time zone, but such consistency is the user's
// responsibility.  This component does not enforce logical constraints on any
// values.
//
///Caveats on Time Zone Support
///----------------------------
// A 'bdlt::DatetimeTz' value is intended to be interpreted as a value in a
// local time zone, along with the offset of that value from UTC.  However,
// there are some problems with this simple interpretation.  First of all, the
// offset value may not correspond to any time zone that has ever existed.  For
// example, the offset value could be set to one minute, or to 1,234 minutes.
// The meaning of the resulting "local datetime" value is always clear, but the
// local datetime might not correspond to any geographical or historical time
// zone.
//
// The second problem is more subtle.  A given offset from UTC might be "valid"
// in that it corresponds to a real time zone, but the actual datetime value
// might not exist in that time zone.  To make matters worse, a "valid" offset
// may not (indeed, rarely will) specify one time zone uniquely.  Moreover, the
// datetime value might be valid in one time zone corresponding to a given
// offset, and not in another time zone.
//
// For these reasons (and others), this component cannot and does not perform
// any validation relating to time zones or offsets.  The user must take care
// to honor the "local datetime" contract of this component.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'bdlt::DatetimeTz' Usage
///- - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create and use a 'bdlt::DatetimeTz' object.
//
// First, create an object 'dt1' having the default value, and then verify that
// it contains an offset of 0, implying that the object represents a date and
// time in the UTC time zone, and the value of the datetime is the same as that
// of a default constructed 'bdlt::Datetime' object:
//..
//  bdlt::DatetimeTz dt1;
//  assert(0                == dt1.offset());
//  assert(bdlt::Datetime() == dt1.localDatetime());
//..
// Then, set 'dt1' to the value 12:00 noon (12:00:00.000) on 12/31/2005 in the
// EST time zone (UTC-5):
//..
//  bdlt::Datetime datetime1(2005, 12, 31, 12, 0, 0, 0);
//  bdlt::Datetime datetime2(datetime1);
//  int            offset1 = -5 * 60;
//
//  dt1.setDatetimeTz(datetime1, offset1);
//  assert(offset1             == dt1.offset());
//  assert(dt1.localDatetime() != dt1.utcDatetime());
//  assert(datetime1           == dt1.localDatetime());
//  assert(datetime2           != dt1.utcDatetime());
//
//  datetime2.addMinutes(-offset1);
//  assert(datetime2 == dt1.utcDatetime());
//..
// Next, create 'dt2' as a copy of 'dt1':
//..
//  bdlt::DatetimeTz dt2(dt1);
//  assert(offset1   == dt2.offset());
//  assert(datetime1 == dt2.localDatetime());
//  assert(datetime2 != dt2.utcDatetime());
//..
// Now, create a third object, 'dt3', representing the time 10:33:25.000 on
// 01/01/2001 in the PST time zone (UTC-8):
//..
//  bdlt::Datetime datetime3(2001, 1, 1, 10, 33, 25, 0);
//  bdlt::Datetime datetime4(datetime3);
//  int            offset2 = -8 * 60;
//
//  bdlt::DatetimeTz dt3(datetime3, offset2);
//  assert(offset2             == dt3.offset());
//  assert(dt3.localDatetime() != dt3.utcDatetime());
//  assert(datetime3           == dt3.localDatetime());
//  assert(datetime4           != dt3.utcDatetime());
//
//  datetime4.addMinutes(-offset2);
//  assert(datetime4 == dt3.utcDatetime());
//..
// Finally, stream the values of 'dt1', 'dt2', and 'dt3' to 'stdout':
//..
//  bsl::cout << dt1 << bsl::endl
//            << dt2 << bsl::endl
//            << dt3 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  31DEC2005_12:00:00.000-0500
//  31DEC2005_12:00:00.000-0500
//  01JAN2001_10:33:25.000-0800
//..
//
///Example 2: Delivery Estimation System
///- - - - - - - - - - - - - - - - - - -
// Let us suppose that we are implementing a delivery estimation system for a
// shipping company.  The system provides estimated delivery dates and times of
// client shipments.  This information is provided in the local time zone and
// is represented as a 'bdlt::DatetimeTz' object.  Below is the definition for
// a struct that returns the estimated delivery date.
//..
//                       // =====================
//                       // struct DeliverySystem
//                       // =====================
//
//  struct DeliverySystem {
//      // This struct provides a function that returns the estimated delivery
//      // date and time for a particular shipment.
//
//      // PRIVATE CLASS METHODS
//      static bdlt::Datetime getCurrentUTCDatetime();
//          // Return the current UTC date and time.
//
//    public:
//      // TYPES
//      enum City {
//          // This enumeration provides an identifier for the various cities.
//
//          e_CHICAGO = 0,
//          e_DUBAI,
//          e_NEW_YORK,
//          e_LONDON,
//          e_LOS_ANGELES
//      };
//
//      // CLASS METHODS
//      static bdlt::DatetimeTz getEstimatedDeliveryDatetime(City city);
//          // Return the estimated delivery date and time, in local time, for
//          // a shipment being sent to the specified 'city'.
//  };
//..
// All the relevant data used for delivery estimation is stored in a lookup
// table as shown below:
//..
//  const int k_MINUTES_PER_HOUR = 60;
//
//  static const struct {
//      int d_offset;         // time zone offset from UTC (in minutes)
//      int d_deliveryTime;   // delivery time (in minutes)
//  } DATA[] = {
//   //    Offset                   DeliveryTime
//   //    =======================  =======================
//   {     -6 * k_MINUTES_PER_HOUR, 10 * k_MINUTES_PER_HOUR  },  // Chicago
//   {      3 * k_MINUTES_PER_HOUR, 72 * k_MINUTES_PER_HOUR  },  // Dubai
//   {     -5 * k_MINUTES_PER_HOUR,      k_MINUTES_PER_HOUR  },  // New York
//   {          k_MINUTES_PER_HOUR, 36 * k_MINUTES_PER_HOUR  },  // London
//   {     -8 * k_MINUTES_PER_HOUR, 24 * k_MINUTES_PER_HOUR  },  // Los Angeles
//  };
//..
// And here are the function definitions:
//..
//                       // ---------------------
//                       // struct DeliverySystem
//                       // ---------------------
//
//  // PRIVATE CLASS METHODS
//  bdlt::Datetime DeliverySystem::getCurrentUTCDatetime()
//  {
//      // Return a fixed datetime so that output is known a priori.
//      return bdlt::Datetime(2014, 10, 17, 14, 48, 56);
//  }
//
//  // CLASS METHODS
//  bdlt::DatetimeTz DeliverySystem::getEstimatedDeliveryDatetime(City city)
//  {
//      bdlt::Datetime localDatetime(getCurrentUTCDatetime());
//      localDatetime.addMinutes(DATA[city].d_offset
//                             + DATA[city].d_deliveryTime);
//      return bdlt::DatetimeTz(localDatetime, DATA[city].d_offset);
//  }
//..
// When we print out the delivery times:
//..
//  bsl::cout << "Estimated Delivery Time in Chicago:     "
//            << DeliverySystem::getEstimatedDeliveryDatetime(
//                                                   DeliverySystem::e_CHICAGO)
//            << bsl::endl;
//  bsl::cout << "Estimated Delivery Time in Dubai:       "
//            << DeliverySystem::getEstimatedDeliveryDatetime(
//                                                     DeliverySystem::e_DUBAI)
//            << bsl::endl;
//  bsl::cout << "Estimated Delivery Time in New York:    "
//            << DeliverySystem::getEstimatedDeliveryDatetime(
//                                                  DeliverySystem::e_NEW_YORK)
//            << bsl::endl;
//  bsl::cout << "Estimated Delivery Time in London:      "
//            << DeliverySystem::getEstimatedDeliveryDatetime(
//                                                    DeliverySystem::e_LONDON)
//            << bsl::endl;
//  bsl::cout << "Estimated Delivery Time in Los Angeles: "
//            << DeliverySystem::getEstimatedDeliveryDatetime(
//                                               DeliverySystem::e_LOS_ANGELES)
//            << bsl::endl;
//..
// We get the following results:
//..
//  Estimated Delivery Time in Chicago:     17OCT2014_18:48:56.000-0600
//  Estimated Delivery Time in Dubai:       20OCT2014_17:48:56.000+0300
//  Estimated Delivery Time in New York:    17OCT2014_10:48:56.000-0500
//  Estimated Delivery Time in London:      19OCT2014_03:48:56.000+0100
//  Estimated Delivery Time in Los Angeles: 18OCT2014_06:48:56.000-0800
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
#endif

#ifndef INCLUDED_BDLT_DATETZ
#include <bdlt_datetz.h>
#endif

#ifndef INCLUDED_BDLT_TIMETZ
#include <bdlt_timetz.h>
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

                             // ================
                             // class DatetimeTz
                             // ================

class DatetimeTz {
    // This value-semantic class describes a datetime value in a particular
    // time zone, which is indicated using an offset from UTC (in minutes).

    // PRIVATE TYPES
    enum ValidOffsetRange {
        // This enumeration specifies the minimum and maximum time zone offset
        // values.

        k_MAX_OFFSET =  1440,
        k_MIN_OFFSET = -1440
    };

    // DATA
    Datetime d_localDatetime;  // datetime value in timezone specified by
                               // 'd_offset'

    int      d_offset;         // offset from UTC (in minutes)

  public:
    // CLASS METHODS
    static bool isValid(const Datetime& localDatetime, int offset);
        // Return 'true' if the specified 'localDatetime' and the specified
        // time zone 'offset' represent a valid 'DatetimeTz' value, and 'false'
        // otherwise.  A 'localDatetime' and 'offset' represent a valid
        // 'DatetimeTz' value if either 'bdlt::Time() == localDatetime.time()'
        // and '0 == offset', or 'bdlt::Time() != localDatetime.time()' and
        // 'offset' is in the range '( -1440 .. 1440 )'.  Note that a 'true'
        // result from this function does not guarantee that 'offset'
        // corresponds to any geographical or historical time zone.  Also note
        // that a 'true' result from this function does not guarantee that
        // 'localDatetime' itself is a valid 'Datetime' object.

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
    DatetimeTz();
        // Create a 'DatetimeTz' object having the (default) attribute values:
        //..
        //  localDatetime() == bdlt::Datetime()
        //  offset()        == 0
        //..

    DatetimeTz(const Datetime& localDatetime, int offset);
        // Create a 'DateTz' object having a local datetime value equal to the
        // specified 'localDatetime' and a time zone offset value from UTC
        // equal to the specified 'offset' (in minutes).  The behavior is
        // undefined unless all of the specified values are within their valid
        // ranges (see 'isValid').  Note that this method provides no
        // validation, and it is the user's responsibility to ensure that
        // 'offset' represents a valid time zone and that 'localDatetime'
        // represents a valid datetime in that time zone.

    DatetimeTz(const DatetimeTz& original);
        // Create a 'DatetimeTz' object having the same value as the specified
        // 'original' object.

    ~DatetimeTz();
        // Destroy this object.

    // MANIPULATORS
    DatetimeTz& operator=(const DatetimeTz& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setDatetimeTz(const Datetime& localDatetime, int offset);
        // Set the local datetime and the time zone offset of this object to
        // the specified 'localDatetime' and 'offset' values respectively.  The
        // behavior is undefined unless all of the specified values are within
        // their valid ranges (see 'isValid').  Note that this method provides
        // no validation, and it is the user's responsibility to assure the
        // consistency of the resulting value.

    int setDatetimeTzIfValid(const Datetime& localDatetime, int offset);
        // If the specified 'localDatetime' and 'offset' represent a valid
        // 'DatetimeTz' value (see 'isValid'), set the local datetime and the
        // time zone offset of this object to the 'localDatetime' and 'offset'
        // values respectively and return 0, leave this object unmodified and
        // return a non-zero value otherwise.

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
    DateTz dateTz() const;
        // Return a 'DateTz' object having the value of the local date and
        // offset represented by this object.

    Datetime localDatetime() const;
        // Return a 'Datetime' object having the value of the local datetime
        // represented by this object.  Note that the 'Datetime' value returned
        // is the current value stored in this object and may be different from
        // the local datetime of the system.

    int offset() const;
        // Return the time zone offset of this 'DatetimeTz' object.  Note that
        // the offset is in minutes from UTC.

    TimeTz timeTz() const;
        // Return a 'TimeTz' object having the value of the local time and
        // offset represented by this object.

    Datetime utcDatetime() const;
        // Return a 'Datetime' object having the value of the UTC datetime
        // represented by this object.  Note that if '0 != offset()', the
        // returned value is equal to 'localDatetime()' minus 'offset()'
        // minutes, and 'localDatetime()' otherwise.

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
bool operator==(const DatetimeTz& lhs, const DatetimeTz& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'DatetimeTz' objects
    // have the same value, and 'false' otherwise.  Two 'DatetimeTz' objects
    // have the same value if they have the same local datetime value and the
    // same time zone offset value.

bool operator!=(const DatetimeTz& lhs, const DatetimeTz& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'DatetimeTz' objects do
    // not have the same value, and 'false' otherwise.  Two 'DatetimeTz'
    // objects do not have the same value if they do not have the same local
    // datetime value or the same time zone offset value.

bsl::ostream& operator<<(bsl::ostream& stream, const DatetimeTz& rhs);
    // Write the value of the specified 'rhs' object to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.  Also note that this
    // method has the same behavior as 'object.print(stream, 0, -1)', but with
    // the attribute names elided.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                             // ----------------
                             // class DatetimeTz
                             // ----------------

// CLASS METHODS
inline
bool DatetimeTz::isValid(const Datetime& localDatetime, int offset)
{
    return k_MIN_OFFSET < offset
        && k_MAX_OFFSET > offset
        && (bdlt::Time() != localDatetime.time() || 0 == offset);
}

                                  // Aspects

inline
int DatetimeTz::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// CREATORS
inline
DatetimeTz::DatetimeTz()
: d_localDatetime()
, d_offset(0)
{
}

inline
DatetimeTz::DatetimeTz(const Datetime& localDatetime, int offset)
: d_localDatetime(localDatetime)
, d_offset(offset)
{
    BSLS_ASSERT_SAFE(isValid(localDatetime, offset));
}

inline
DatetimeTz::DatetimeTz(const DatetimeTz& original)
: d_localDatetime(original.d_localDatetime)
, d_offset(original.d_offset)
{
}

inline
DatetimeTz::~DatetimeTz()
{
    BSLS_ASSERT_SAFE(isValid(d_localDatetime, d_offset));
}

// MANIPULATORS
inline
DatetimeTz& DatetimeTz::operator=(const DatetimeTz& rhs)
{
    d_localDatetime = rhs.d_localDatetime;
    d_offset        = rhs.d_offset;

    return *this;
}

inline
void DatetimeTz::setDatetimeTz(const Datetime& localDatetime, int offset)
{
    BSLS_ASSERT_SAFE(isValid(localDatetime, offset));

    d_localDatetime = localDatetime;
    d_offset        = offset;
}

inline
int DatetimeTz::setDatetimeTzIfValid(const Datetime& localDatetime,
                                     int             offset)
{
    if (isValid(localDatetime, offset)) {
        setDatetimeTz(localDatetime, offset);
        return 0;                                                     // RETURN
    }
    return -1;
}

                                  // Aspects

template <class STREAM>
STREAM& DatetimeTz::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            Datetime localDatetime;
            localDatetime.bdexStreamIn(stream, 1);

            int offset;
            stream.getInt32(offset);

            if (stream && isValid(localDatetime, offset)) {
                d_localDatetime = localDatetime;
                d_offset        = offset;
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
DateTz DatetimeTz::dateTz() const
{
    return DateTz(d_localDatetime.date(), d_offset);
}

inline
Datetime DatetimeTz::localDatetime() const
{
    return d_localDatetime;
}

inline
int DatetimeTz::offset() const
{
    return d_offset;
}

inline
TimeTz DatetimeTz::timeTz() const
{
    return TimeTz(d_localDatetime.time(), d_offset);
}

inline
Datetime DatetimeTz::utcDatetime() const
{
    Datetime utc(d_localDatetime);
    if (d_offset) {
        utc.addMinutes(-d_offset);
    }
    return utc;
}

                                  // Aspects

template <class STREAM>
STREAM& DatetimeTz::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            d_localDatetime.bdexStreamOut(stream, 1);
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
bool bdlt::operator==(const DatetimeTz& lhs, const DatetimeTz& rhs)
{
    return lhs.localDatetime() == rhs.localDatetime()
        && lhs.offset()        == rhs.offset();
}

inline
bool bdlt::operator!=(const DatetimeTz& lhs, const DatetimeTz& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdlt::operator<<(bsl::ostream& stream, const DatetimeTz& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

namespace bsl {

// TRAITS
template <>
struct is_trivially_copyable<BloombergLP::bdlt::DatetimeTz> : bsl::true_type {
    // This template specialization for 'is_trivially_copyable' indicates that
    // 'bdlt::DatetimeTz' is a trivially copyable type.
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
