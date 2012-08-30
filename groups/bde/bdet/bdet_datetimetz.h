// bdet_datetimetz.h                                                  -*-C++-*-
#ifndef INCLUDED_BDET_DATETIMETZ
#define INCLUDED_BDET_DATETIMETZ

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a representation of a date and time with time zone offset.
//
//@CLASSES:
//  bdet_DatetimeTz: local-datetime value with time zone offset from UTC
//
//@SEE_ALSO: bdet_datetime
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@DESCRIPTION: This component provides a local-time datetime value and an
// offset from UTC.  Specifically, the 'bdet_DatetimeTz' class provided by this
// component represents a datetime value similar to 'bdet_Datetime', but
// explicitly as a local time; the offset of that local time from UTC (in
// minutes) is also part of the value of a 'bdet_DatetimeTz' object.  For
// logical consistency, the datetime value and offset should correspond to a
// geographically valid time and time zone, but such consistency is the user's
// responsibility.  This component does not enforce logical constraints on any
// values.
//
// The 'localDatetime' and 'utcDatetime' methods return 'bdet_Datetime' values
// corresponding to the local time and UTC time represented by the object,
// respectively.  In addition, the 'offset' method returns the time zone offset
// in minutes from UTC (i.e., 'UTC + offset' equals local time).
//
///A Note on Time Zone Support
///---------------------------
// 'bdet_DatetimeTz' is a convenient, value-semantic class that provides some
// support for the notion of "time zone".  In some sense, 'bdet_DatetimeTz'
// "extends" 'bdet_Datetime', which has no notion of "time zone" at all, but
// otherwise represents the same kind of value as 'bdet_DatetimeTz'.  However,
// time zone "support" is limited to providing an offset value (in integer
// minutes).  The rest is left to convention and to the interpretations of the
// user.
//
// A 'bdet_DatetimeTz' value is intended to be interpreted as a value in some
// local time zone, along with the offset of that value from UTC.  However,
// there are some problems with this simple interpretation.  First of all, the
// offset value may not correspond to any time zone that has ever existed.  For
// example, the offset value could be set to one minute, or to 1,234 minutes.
// The meaning of the resulting "local time" value is always clear, but the
// local time might not correspond to any geographical or historical time zone.
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
// to honor the "local time" contract of this component.
//
///Usage
///-----
///Example 1
///---------
// The following snippets of code illustrate how to create and use
// 'bdet_DateTz' objects.  First we will default construct an object
// 'datetimeTz1'.  A default constructed object contains an offset of 0,
// implying that the object represents a date and time in the UTC time zone.
// The value of the date and time is the same as that of a default constructed
// 'bdet_Datetime' object:
//..
//  bdet_DatetimeTz datetimeTz1;
//  assert(0                           == datetimeTz1.offset());
//  assert(datetimeTz1.localDatetime() == bdet_Datetime());
//..
// Next we set 'datetimeTz1' to 12:00 noon (12:00:00.000) on 12/31/2005 in the
// EST time zone (UTC-5):
//..
//  bdet_Datetime datetime1(2005, 12, 31, 12, 0, 0, 0);
//  bdet_Datetime datetime2(datetime1);
//  int           offset1 = -5 * 60;
//
//  datetimeTz1.setDatetimeTz(datetime1, offset1);
//  assert(offset1                     == datetimeTz1.offset());
//  assert(datetimeTz1.localDatetime() != datetimeTz1.utcDatetime());
//  assert(datetimeTz1.localDatetime() == datetime1);
//  assert(datetimeTz1.utcDatetime()   != datetime2);
//
//  datetime2.addMinutes(-offset1);
//  assert(datetimeTz1.utcDatetime()   == datetime2);
//..
// Then we create 'datetimeTz2' as a copy of 'datetimeTz1':
//..
//  bdet_DatetimeTz datetimeTz2(datetimeTz1);
//  assert(offset1                     == datetimeTz2.offset());
//  assert(datetimeTz2.localDatetime() == datetime1);
//  assert(datetimeTz2.utcDatetime()   == datetime2);
//..
// We now create a third object, 'datetimeTz3', representing the time
// 10:33:25.000 on 01/01/2001 in the PST time zone (UTC-8):
//..
//  bdet_Datetime  datetime3(2001, 1, 1, 10, 33, 25, 0);
//  bdet_Datetime  datetime4(datetime3);
//  int            offset2 = -8 * 60;
//
//  bdet_DatetimeTz datetimeTz3(datetime3, offset2);
//  assert(offset2                     == datetimeTz3.offset());
//  assert(datetimeTz3.localDatetime() != datetimeTz3.utcDatetime());
//  assert(datetimeTz3.localDatetime() == datetime3);
//  assert(datetimeTz3.utcDatetime()   != datetime4);
//
//  datetime4.addMinutes(-offset2);
//  assert(datetimeTz3.utcDatetime()   == datetime4);
//..
// Finally we stream out the values of 'datetimeTz1', 'datetimeTz2' and
// 'datetimeTz3' to 'stdout':
//..
//  bsl::cout << datetimeTz1 << bsl::endl
//            << datetimeTz2 << bsl::endl
//            << datetimeTz3 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  31DEC2005-0500
//  31DEC2005-0500
//  01JAN2001-0800
//..
///Example 2
///---------
// Let us suppose that we are implementing a delivery estimation system for a
// shipping company.  The system provides estimated delivery dates and times of
// client shipments.  This information is provided in the local time zone and
// is represented as a 'bdet_DatetimeTz' object.  Below is the definition for a
// struct that returns the estimated delivery date.
//..
//                           // =====================
//                           // struct DeliverySystem
//                           // =====================
//
//  struct DeliverySystem {
//      // This struct provides a function that returns the estimated delivery
//      // date and time for a particular shipment.
//
//      // PRIVATE CLASS METHODS
//      static bdet_Datetime getCurrentUTCDatetime();
//          // Return the current UTC date and time.
//
//    public:
//      // TYPES
//      enum CITY {
//          // This enumeration provides an identifier for the various cities.
//
//          CHICAGO = 0,
//          DUBAI,
//          NEW_YORK,
//          LONDON,
//          LOS_ANGELES
//      };
//
//      // CLASS METHODS
//      static bdet_DatetimeTz getEstimatedDeliveryDatetime(CITY city);
//          // Return the estimated delivery date and time, in local time, for
//          // a shipment being sent to the specified 'city'.
//  };
//..
// All the relevant data used for delivery estimation is stored in a lookup
// table as shown below:
//..
//  const int MINUTES_PER_HOUR = 60;
//
//  static const struct {
//      int d_offset;         // time zone offset from UTC (in minutes)
//      int d_deliveryTime;   // delivery time (in minutes)
//  } DATA[] = {
//     //    Offset                 DeliveryTime
//     //    ======                 ============
//   {     -6 * MINUTES_PER_HOUR, 10 * MINUTES_PER_HOUR  },     // Chicago
//   {      3 * MINUTES_PER_HOUR, 72 * MINUTES_PER_HOUR  },     // Dubai
//   {     -5 * MINUTES_PER_HOUR,      MINUTES_PER_HOUR  },     // New York
//   {          MINUTES_PER_HOUR, 36 * MINUTES_PER_HOUR  },     // London
//   {     -8 * MINUTES_PER_HOUR, 24 * MINUTES_PER_HOUR  },     // Los Angeles
//  };
//..
// And here are the function definitions:
//..
//  #include <sys/types.h>         // type definitions
//  #include <bsl_ctime.h>         // various time functions
//
//                              // ---------------------
//                              // struct DeliverySystem
//                              // ---------------------
//
//  // PRIVATE CLASS METHODS
//  bdet_Datetime DeliverySystem::getCurrentUTCDatetime()
//  {
//      bsl::time_t currentTime = bsl::time(0);
//      bsl::tm     utcTime;
//
//  #if defined(BSLS_PLATFORM__OS_WINDOWS) || ! defined(BDE_BUILD_TARGET_MT)
//      utcTime = *bsl::gmtime(&currentTime);
//  #else
//      gmtime_r(&currentTime, &utcTime);
//  #endif
//
//      return bdet_Datetime(1900 + utcTime.tm_year,
//                           utcTime.tm_mon,
//                           utcTime.tm_mday,
//                           utcTime.tm_hour,
//                           utcTime.tm_min,
//                           utcTime.tm_sec);
//  }
//
//  // CLASS METHODS
//  bdet_DatetimeTz DeliverySystem::getEstimatedDeliveryDatetime(CITY city)
//  {
//      bdet_Datetime localDatetime(getCurrentUTCDatetime());
//      localDatetime.addMinutes(DATA[city].d_offset
//                             + DATA[city].d_deliveryTime);
//      return bdet_DatetimeTz(localDatetime, DATA[city].d_offset);
//  }
//..
// When we print out the delivery times:
//..
//  bsl::cout << "Estimated Delivery Time in Chicago:     "
//            << DeliverySystem::getEstimatedDeliveryDatetime(
//                                                     DeliverySystem::CHICAGO)
//            << bsl::endl;
//  bsl::cout << "Estimated Delivery Time in Dubai:       "
//            << DeliverySystem::getEstimatedDeliveryDatetime(
//                                                       DeliverySystem::DUBAI)
//            << bsl::endl;
//  bsl::cout << "Estimated Delivery Time in New York:    "
//            << DeliverySystem::getEstimatedDeliveryDatetime(
//                                                    DeliverySystem::NEW_YORK)
//            << bsl::endl;
//  bsl::cout << "Estimated Delivery Time in London:      "
//            << DeliverySystem::getEstimatedDeliveryDatetime(
//                                                      DeliverySystem::LONDON)
//            << bsl::endl;
//  bsl::cout << "Estimated Delivery Time in Los Angeles: "
//            << DeliverySystem::getEstimatedDeliveryDatetime(
//                                                 DeliverySystem::LOS_ANGELES)
//            << bsl::endl;
//..
// we get the following results:
//..
//  Estimated Delivery Time in Chicago:     04JAN2008_22:45:49.000-0600
//  Estimated Delivery Time in Dubai:       07JAN2008_21:45:49.000+0300
//  Estimated Delivery Time in New York:    04JAN2008_14:45:49.000-0500
//  Estimated Delivery Time in London:      06JAN2008_07:45:49.000+0100
//  Estimated Delivery Time in Los Angeles: 05JAN2008_10:45:49.000-0800
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDET_DATETZ
#include <bdet_datetz.h>
#endif

#ifndef INCLUDED_BDET_TIMETZ
#include <bdet_timetz.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                             // =====================
                             // class bdet_DatetimeTz
                             // =====================

class bdet_DatetimeTz {
    // This 'class' represents a datetime value explicitly in a local time
    // zone; the offset of that time (in minutes) from UTC is also part of the
    // value of this class.  The offset is available via the 'offset' method,
    // and is defined by the relationship: 'UTC + offset' equals local time.
    // The datetime and offset values are logically assumed to correspond to
    // geographically valid values, however, this constraint is not enforced.
    // The default output format is 'ddMONyyyy_hh:mm:ss.sss[+-]hhmm'.
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  A precise
    // operational definition of when two objects have the same value can be
    // found in the description of 'operator==' for the class.  This class is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing object, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // DATA
    bdet_Datetime d_datetime;  // date and time (local)
    int           d_offset;    // offset from UTC (in minutes)

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(bdet_DatetimeTz,
                             bsl::is_trivially_copyable,
                             bsl::is_trivially_copyable<bdet_Datetime>::value);

    // CLASS METHODS
    static bool isValid(const bdet_Datetime& datetime, int offset);
        // Return 'true' if the specified 'datetime' and 'offset' represent a
        // valid 'bdet_DatetimeTz' value, and 'false' otherwise.  A 'datetime'
        // and 'offset' represent a valid 'bdet_DatetimeTz' value if 'offset'
        // is in the range '( -1440 .. 1440 )', and 'offset' is 0 if
        // 'datetime.time()' has the value '24:00:00.000'.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of value semantic types.)

    // CREATORS
    bdet_DatetimeTz();
        // Create a valid 'bdet_DatetimeTz' object having an implementation-
        // dependent value.

    bdet_DatetimeTz(const bdet_Datetime& datetime, int offset);
        // Create a 'bdet_DatetimeTz' object having a local datetime value
        // equal to the specified 'datetime' and a time zone offset value from
        // UTC equal to the specified 'offset' (in minutes).  The behavior is
        // undefined unless 'offset' is in the range '( -1440 .. 1440 )', and
        // 'offset' is 0 if 'datetime.time()' has the value '24:00:00.000'.
        // Note that this method provides no validation, and it is the user's
        // responsibility to assure the consistency of the resulting value.

    bdet_DatetimeTz(const bdet_DatetimeTz& original);
        // Construct a 'bdet_DatetimeTz' object having the value of the
        // specified 'original' 'bdet_DatetimeTz' object.

    //! ~bdet_DatetimeTz();
        // Destroy this 'bdet_DatetimeTz' object.  Note that this method's
        // definition is compiler generated.

    // MANIPULATORS
    bdet_DatetimeTz& operator=(const bdet_DatetimeTz& rhs);
        // Assign to this 'bdet_DatetimeTz' object the value of the specified
        // 'rhs' 'bdet_DatetimeTz' object, and return a reference to this
        // modifiable object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void setDatetimeTz(const bdet_Datetime& datetime, int offset);
        // Set the local time and the time zone offset of this object to the
        // specified 'datetime' and 'offset' values, respectively.  The
        // behavior is undefined unless 'offset' is in the range
        // '( -1440 .. 1440 )', and 'offset' is 0 if 'datetime.time()' has the
        // value '24:00:00.000'.  Note that this method provides no validation,
        // and it is the user's responsibility to assure the consistency of the
        // resulting value.

    int validateAndSetDatetimeTz(const bdet_Datetime& datetime, int offset);
        // Set the datetime value of this 'bdet_DatetimeTz' object to the
        // specified 'datetime' and the offset value to the specified 'offset'
        // if 'datetime' and 'offset' represent a valid 'bdet_DatetimeTz'
        // value.  Return 0 on success, and a non-zero value with no effect on
        // this 'bdet_DatetimeTz' object otherwise.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.  See the 'bdex'
        // package-level documentation for more information on 'bdex' streaming
        // of value-semantic types and containers.

    bdet_DateTz dateTz() const;
        // Return a 'bdet_DateTz' object having the value of the local date and
        // offset represented by this object.

    bdet_Datetime utcDatetime() const;
        // Return a 'bdet_Datetime' object having the value of the UTC time
        // represented by this object.  Note that the returned value is equal
        // to 'localDatetime() - offset()' minutes.

    bdet_Datetime gmtDatetime() const;
        // Return a 'bdet_Datetime' object having the value of the UTC time
        // represented by this object.  Note that the returned value is equal
        // to 'localDatetime() - offset()' minutes.
        //
        // DEPRECATED: replaced by 'utcDatetime.'

    bdet_Datetime localDatetime() const;
        // Return a 'bdet_Datetime' object having the value of the local time
        // represented by this object.  Note that this is the 'bdet_Datetime'
        // supplied at construction and may not correspond to the actual time
        // zone offset of the local system.

    int offset() const;
        // Return the time zone offset of this 'bdet_DatetimeTz' object.  Note
        // that the offset is in minutes from UTC.

    bdet_TimeTz timeTz() const;
        // Return a 'bdet_TimeTz' object having the value of the local time and
        // offset represented by this object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, suppress all indentation AND format the entire output on
        // one line.  If 'stream' is not valid on entry, this operation has no
        // effect.
};

// FREE OPERATORS
bool operator==(const bdet_DatetimeTz& lhs, const bdet_DatetimeTz& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdet_DatetimeTz' objects
    // have the same value, and 'false' otherwise.  Two 'bdet_DatetimeTz'
    // objects have the same value if they have the same local datetime value
    // and the same time zone offset value.

bool operator!=(const bdet_DatetimeTz& lhs, const bdet_DatetimeTz& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdet_DatetimeTz' objects
    // do not have the same value, and 'false' otherwise.  Two
    // 'bdet_DatetimeTz' objects differ in value if they differ in local
    // datetime value or in time zone offset value, or in both values.

bsl::ostream& operator<<(bsl::ostream& stream, const bdet_DatetimeTz& rhs);
    // Write the specified 'rhs' value to the specified output 'stream' in the
    // following 27-character format:
    //..
    //  ddMONyyyy_hh:mm:ss.sss[+-]hhmm
    //..
    // and return a reference to the modifiable 'stream'.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // ---------------------
                             // class bdet_DatetimeTz
                             // ---------------------

// CLASS METHODS
inline
bool bdet_DatetimeTz::isValid(const bdet_Datetime& datetime, int offset)
{
    const bdet_Date& date = datetime.date();
    const bdet_Time& time = datetime.time();
    return bdet_Datetime::isValid(date.year(),
                                  date.month(),
                                  date.day(),
                                  time.hour(),
                                  time.minute(),
                                  time.second(),
                                  time.millisecond())
        && offset > -1440 && offset < 1440 && !(24 == time.hour() && offset);
}

inline
int bdet_DatetimeTz::maxSupportedBdexVersion()
{
    return 1;
}

// CREATORS
inline
bdet_DatetimeTz::bdet_DatetimeTz()
: d_datetime()
, d_offset(0)
{
}

inline
bdet_DatetimeTz::bdet_DatetimeTz(const bdet_Datetime& datetime, int offset)
: d_datetime(datetime)
, d_offset(offset)
{
    BSLS_ASSERT_SAFE(isValid(datetime, offset));
}

inline
bdet_DatetimeTz::bdet_DatetimeTz(const bdet_DatetimeTz& original)
: d_datetime(original.d_datetime)
, d_offset(original.d_offset)
{
}

// MANIPULATORS
inline
bdet_DatetimeTz& bdet_DatetimeTz::operator=(const bdet_DatetimeTz& rhs)
{
    d_datetime = rhs.d_datetime;
    d_offset   = rhs.d_offset;

    return *this;
}

template <class STREAM>
STREAM& bdet_DatetimeTz::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdet_Datetime datetime;
            datetime.bdexStreamIn(stream, 1);

            int offset;
            stream.getInt32(offset);

            if (!stream || !isValid(datetime, offset)) {
                stream.invalidate();
            }
            else {
                d_datetime = datetime;
                d_offset   = offset;
            }
          } break;
          default: {
            stream.invalidate();
          } break;
        }
    }
    return stream;
}

inline
void bdet_DatetimeTz::setDatetimeTz(const bdet_Datetime& datetime, int offset)
{
    BSLS_ASSERT_SAFE(isValid(datetime, offset));

    d_datetime = datetime;
    d_offset   = offset;
}

inline
int bdet_DatetimeTz::validateAndSetDatetimeTz(const bdet_Datetime& datetime,
                                              int                  offset)
{
    if (isValid(datetime, offset)) {
        setDatetimeTz(datetime, offset);
        return 0;
    }
    return -1;
}

// ACCESSORS
template <class STREAM>
STREAM& bdet_DatetimeTz::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        d_datetime.bdexStreamOut(stream, 1);
        stream.putInt32(d_offset);
      } break;
    }
    return stream;
}

inline
bdet_DateTz bdet_DatetimeTz::dateTz() const
{
    return bdet_DateTz(d_datetime.date(), d_offset);
}

inline
bdet_Datetime bdet_DatetimeTz::utcDatetime() const
{
    bdet_Datetime gmt(d_datetime);
    if (d_offset) {
        gmt.addMinutes(-d_offset);
    }
    return gmt;
}

inline
bdet_Datetime bdet_DatetimeTz::gmtDatetime() const
{
    return utcDatetime();
}

inline
bdet_Datetime bdet_DatetimeTz::localDatetime() const
{
    return d_datetime;
}

inline
int bdet_DatetimeTz::offset() const
{
    return d_offset;
}

inline
bdet_TimeTz bdet_DatetimeTz::timeTz() const
{
    return bdet_TimeTz(d_datetime.time(), d_offset);
}

// FREE OPERATORS
inline
bool operator==(const bdet_DatetimeTz& lhs, const bdet_DatetimeTz& rhs)
{
    return lhs.localDatetime() == rhs.localDatetime()
        && lhs.offset()        == rhs.offset();
}

inline
bool operator!=(const bdet_DatetimeTz& lhs, const bdet_DatetimeTz& rhs)
{
    return lhs.localDatetime() != rhs.localDatetime()
        || lhs.offset()        != rhs.offset();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
