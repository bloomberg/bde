// bdet_timetz.h                                                      -*-C++-*-
#ifndef INCLUDED_BDET_TIMETZ
#define INCLUDED_BDET_TIMETZ

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a representation of a time with time zone offset.
//
//@CLASSES:
//     bdet_TimeTz: local-time value with time zone offset from UTC
//
//@SEE_ALSO: bdet_time, bdet_datetimetz
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a time value and an offset from UTC.
// Specifically, the 'bdet_TimeTz' class provided by this component represents
// a time value similar to 'bdet_Time', but explicitly as a local time; the
// offset of that local time from UTC (in minutes) is also part of the value of
// a 'bdet_TimeTz' object.  For logical consistency, the time value and offset
// should correspond to a geographically valid time zone, but such consistency
// is the user's responsibility.  This component does not enforce logical
// constraints on any values.
//
// The 'localTime' and 'utcTime' methods return 'bdet_Time' values
// corresponding to the local time and UTC time represented by the object,
// respectively.  In addition, the 'offset' method returns the time zone offset
// in minutes from UTC (i.e., 'UTC + offset' equals local time).
//
///A Note on Time Zone Support
///---------------------------
// 'bdet_TimeTz' is a convenient, value-semantic class that provides some
// support for the notion of "time zone".  In some sense, 'bdet_TimeTz'
// "extends" 'bdet_Time', which has no notion of "time zone" at all, but
// otherwise represents the same kind of value as 'bdet_TimeTz'.  However, time
// zone "support" is limited to providing an offset value (in minutes).  The
// rest is left to convention and to the interpretations of the user.
//
// A 'bdet_TimeTz' value is intended to be interpreted as a value in some local
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
///Example 1
///---------
// The following snippets of code illustrate how to create and use
// 'bdet_TimeTz' objects.  First we will default construct an object 'timeTz'.
// A default constructed object contains an offset of 0, implying that the
// object represents time in the UTC time zone.  The value of the time is the
// same as that of a default constructed 'bdet_Time' object:
//..
//  bdet_TimeTz timeTz1;
//  assert(0                   == timeTz1.offset());
//  assert(timeTz1.localTime() == bdet_Time());
//..
// Next we set 'timeTz1' to 12:00 noon (12:00:00.000) in the EST time zone
// (UTC-5):
//..
//  bdet_Time time1(12, 0, 0, 0);
//  bdet_Time time2(time1);
//  int       offset1 = -5 * 60;
//
//  timeTz1.setTimeTz(time1, offset1);
//  assert(offset1             == timeTz1.offset());
//  assert(timeTz1.localTime() != timeTz1.utcTime());
//  assert(timeTz1.localTime() == time1);
//  assert(timeTz1.utcTime()   != time2);
//
//  time2.addMinutes(-offset1);
//  assert(timeTz1.utcTime()   == time2);
//..
// Then we create 'timeTz2' as a copy of 'timeTz1':
//..
//  bdet_TimeTz timeTz2(timeTz1);
//  assert(offset1             == timeTz2.offset());
//  assert(timeTz2.localTime() == time1);
//  assert(timeTz2.utcTime()   == time2);
//..
// We now create a third object, 'timeTz3', representing the time 10:33:25.000
// in the PST time zone (UTC-8):
//..
//  bdet_Time time3(10, 33, 25, 0);
//  bdet_Time time4(time3);
//  int       offset2 = -8 * 60;
//
//  bdet_TimeTz timeTz3(time3, offset2);
//  assert(offset2             == timeTz3.offset());
//  assert(timeTz3.localTime() != timeTz3.utcTime());
//  assert(timeTz3.localTime() == time3);
//  assert(timeTz3.utcTime()   != time4);
//
//  time4.addMinutes(-offset2);
//  assert(timeTz3.utcTime()   == time4);
//..
// Finally we stream the values of 'timeTz1', 'timeTz2' and 'timeTz3' to
// 'stdout':
//..
//  bsl::cout << timeTz1 << bsl::endl
//            << timeTz2 << bsl::endl
//            << timeTz3 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  12:00:00.000-0500
//  12:00:00.000-0500
//  10:33:25.000-0800
//..
///Example 2
///---------
// Let us provide a more concrete example where we use a 'bdet_TimeTz' object.
// Suppose that we want to create a time oracle struct that returns the current
// time in a particular city.  The struct provides a simple function taking an
// enum value representing a city, and returns a 'bdet_TimeTz' object with the
// current time in that city.
//..
//                           // =================
//                           // struct TimeOracle
//                           // =================
//
//  struct TimeOracle {
//      // This struct provides a function that returns the current time in a
//      // user-specified city.
//
//      // PRIVATE CLASS METHODS
//      static bdet_Time getCurrentUTCTime();
//          // Return the current UTC time.
//
//      enum { MINUTES_PER_HOUR = 60 };
//
//    public:
//      // TYPES
//      enum CITY {
//          // This enumeration provides the offsets from UTC corresponding to
//          // various cities.
//
//          LOS_ANGELES      = -8 * MINUTES_PER_HOUR,
//          LOS_ANGELES_DST  = -7 * MINUTES_PER_HOUR,
//          PHOENIX          = -7 * MINUTES_PER_HOUR,
//          PHOENIX_DST      = -7 * MINUTES_PER_HOUR,
//          CHICAGO          = -6 * MINUTES_PER_HOUR,
//          CHICAGO_DST      = -5 * MINUTES_PER_HOUR,
//          NEW_YORK         = -5 * MINUTES_PER_HOUR,
//          NEW_YORK_DST     = -4 * MINUTES_PER_HOUR
//      };
//
//      // CLASS METHODS
//      static bdet_TimeTz getLocalTime(CITY city);
//          // Return the current time in the time zone corresponding to the
//          // specified 'city'.
//  };
//
//                           // -----------------
//                           // struct TimeOracle
//                           // -----------------
//
//  #include <sys/types.h>         // type definitions
//  #include <bsl_ctime.h>         // various time functions
//
//  // PRIVATE CLASS METHODS
//  bdet_Time TimeOracle::getCurrentUTCTime()
//  {
//      bsl::time_t currentTime = bsl::time(0);
//      bsl::tm     utcTime;
//
//  #if defined(BSLS_PLATFORM_OS_WINDOWS) || ! defined(BDE_BUILD_TARGET_MT)
//      utcTime = *bsl::gmtime(&currentTime);
//  #else
//      gmtime_r(&currentTime, &utcTime);
//  #endif
//
//      return bdet_Time(utcTime.tm_hour, utcTime.tm_min, utcTime.tm_sec);
//  }
//
//  // CLASS METHODS
//  bdet_TimeTz TimeOracle::getLocalTime(CITY city)
//  {
//      bdet_Time localTime(getCurrentUTCTime());
//      localTime.addMinutes((int) city);
//      return bdet_TimeTz(localTime, (int) city);
//  }
//..
// Finally, the following statements produce the time in various time zones:
//..
//  bsl::cout << "Current New York Time:    "
//            << TimeOracle::getLocalTime(TimeOracle::NEW_YORK)
//            << bsl::endl;
//  bsl::cout << "Current Chicago DST Time: "
//            << TimeOracle::getLocalTime(TimeOracle::CHICAGO_DST)
//            << bsl::endl;
//  bsl::cout << "Current Phoenix Time:     "
//            << TimeOracle::getLocalTime(TimeOracle::PHOENIX)
//            << bsl::endl;
//  bsl::cout << "Current Los Angeles Time: "
//            << TimeOracle::getLocalTime(TimeOracle::LOS_ANGELES)
//            << bsl::endl;
//..
// The result produced is as follows (the actual result might vary based on the
// current time at which the example is run):
//..
//  Current New York Time:    10:19:44.000-0500
//  Current Chicago DST Time: 10:19:44.000-0500
//  Current Phoenix Time:     08:19:44.000-0700
//  Current Los Angeles Time: 07:19:44.000-0800
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_TIME
#include <bdet_time.h>
#endif

#ifndef INCLUDED_BSLALG_PASSTHROUGHTRAIT
#include <bslalg_passthroughtrait.h>
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

                             // =================
                             // class bdet_TimeTz
                             // =================

class bdet_TimeTz {
    // This 'class' represents a time value explicitly in a local time zone;
    // the offset of that time (in minutes) from UTC is also part of the value
    // of this class.  The offset is available via the 'offset' method, and is
    // defined by the relationship: 'UTC + offset' equals local time.  The time
    // and offset values are logically assumed to correspond to geographically
    // valid values, however, this constraint is not enforced.  The default
    // output format is 'ddMONyyyy_hh:mm:ss.sss[+-]hhmm'.
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

    // PRIVATE TYPES
    typedef bslalg_PassthroughTrait<bdet_Time,
                                    bslalg_TypeTraitBitwiseCopyable> TimeTrait;

    // DATA
    bdet_Time d_time;    // time (local)
    int       d_offset;  // offset from UTC (in minutes)

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdet_TimeTz, TimeTrait);

    // CLASS METHODS
    static bool isValid(const bdet_Time& time, int offset);
        // Return 'true' if the specified 'time' and 'offset' represent a valid
        // 'bdet_TimeTz' value, and 'false' otherwise.  A 'time' and 'offset'
        // represent a valid 'bdet_TimeTz' value if 'offset' is in the range
        // '( -1440 .. 1440 )', and 'offset' is 0 if 'time' has the value
        // '24:00:00.000'.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of value semantic types.)

    // CREATORS
    bdet_TimeTz();
        // Create a valid 'bdet_TimeTz' object having an implementation-
        // dependent value.

    bdet_TimeTz(const bdet_Time& time, int offset);
        // Create a 'bdet_TimeTz' object having a local time value equal to the
        // specified 'time' and a time zone offset value from UTC equal to the
        // specified 'offset' (in minutes).  The behavior is undefined unless
        // 'offset' is in the range '( -1440 .. 1440 )', and 'offset' is 0 if
        // 'time' has the value '24:00:00.000'.  Note that this method provides
        // no validation, and it is the user's responsibility to assure the
        // consistency of the resulting value.

    bdet_TimeTz(const bdet_TimeTz& original);
        // Construct a 'bdet_TimeTz' object having the value of the specified
        // 'original' 'bdet_TimeTz' object.

    //! ~bdet_TimeTz();
        // Destroy this 'bdet_TimeTz' object.  Note that this method's
        // definition is compiler generated.

    // MANIPULATORS
    bdet_TimeTz& operator=(const bdet_TimeTz& rhs);
        // Assign to this 'bdet_TimeTz' object the value of the specified 'rhs'
        // 'bdet_TimeTz' object, and return a reference to this modifiable
        // object.

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

    void setTimeTz(const bdet_Time& time, int offset);
        // Set the local time and the time zone offset of this object to the
        // specified 'time' and 'offset' values, respectively.  The behavior is
        // undefined unless 'offset' is in the range '( -1440 .. 1440 )'.  Note
        // that this method provides no validation, and it is the user's
        // responsibility to assure the consistency of the resulting value.

    int validateAndSetTimeTz(const bdet_Time& time, int offset);
        // Set the time value of this 'bdet_TimeTz' object to the specified
        // 'time' and the offset value to the specified 'offset' if 'time' and
        // 'offset' represent a valid 'bdet_TimeTz' value.  Return 0 on
        // success, and a non-zero value with no effect on this 'bdet_TimeTz'
        // object otherwise.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.  See the 'bdex'
        // package-level documentation for more information on 'bdex' streaming
        // of value-semantic types and containers.

    bdet_Time utcTime() const;
        // Return a 'bdet_Time' object having the value of the UTC time
        // represented by this object.  Note that the returned value is equal
        // to 'localTime() - offset()' minutes.

    bdet_Time gmtTime() const;
        // Return a 'bdet_Time' object having the value of the UTC time
        // represented by this object.  Note that the returned value is equal
        // to 'localTime() - offset()' minutes.
        //
        // DEPRECATED: replaced by 'utcTime'.

    bdet_Time localTime() const;
        // Return a 'bdet_Time' object having the value of the local time
        // represented by this object.  Note that this is the 'bdet_Time'
        // supplied at construction and may not correspond to the actual time
        // zone offset of the local system.

    int offset() const;
        // Return the time zone offset of this 'bdet_TimeTz' object.  Note that
        // the offset is in minutes from UTC.

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
bool operator==(const bdet_TimeTz& lhs, const bdet_TimeTz& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdet_TimeTz' objects
    // have the same value, and 'false' otherwise.  Two 'bdet_TimeTz' objects
    // have the same value if they have the same local time value and the same
    // time zone offset value.

bool operator!=(const bdet_TimeTz& lhs, const bdet_TimeTz& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'bdet_TimeTz' objects do
    // not have the same value, and 'false' otherwise.  Two 'bdet_TimeTz'
    // objects differ in value if they differ in local time value or in time
    // zone offset value, or in both values.

bsl::ostream& operator<<(bsl::ostream& stream, const bdet_TimeTz& rhs);
    // Write the specified 'rhs' value to the specified output 'stream' in the
    // following 17-character format:
    //..
    //  hh:mm:ss.sss[+-]hhmm
    //..
    // and return a reference to the modifiable 'stream'.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // -----------------
                             // class bdet_TimeTz
                             // -----------------

// CLASS METHODS
inline
bool bdet_TimeTz::isValid(const bdet_Time& time, int offset)
{
    return bdet_Time::isValid(time.hour(),
                              time.minute(),
                              time.second(),
                              time.millisecond())
        && offset > -1440 && offset < 1440 && !(24 == time.hour() && offset);
}

inline
int bdet_TimeTz::maxSupportedBdexVersion()
{
    return 1;
}

// CREATORS
inline
bdet_TimeTz::bdet_TimeTz()
: d_time()
, d_offset(0)
{
}

inline
bdet_TimeTz::bdet_TimeTz(const bdet_Time& time, int offset)
: d_time(time)
, d_offset(offset)
{
    BSLS_ASSERT_SAFE(isValid(time, offset));
}

inline
bdet_TimeTz::bdet_TimeTz(const bdet_TimeTz& original)
: d_time(original.d_time)
, d_offset(original.d_offset)
{
}

// MANIPULATORS
inline
bdet_TimeTz& bdet_TimeTz::operator=(const bdet_TimeTz& rhs)
{
    d_time   = rhs.d_time;
    d_offset = rhs.d_offset;

    return *this;
}

template <class STREAM>
STREAM& bdet_TimeTz::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdet_Time time;
            time.bdexStreamIn(stream, 1);

            int offset;
            stream.getInt32(offset);

            if (!stream || !isValid(time, offset)) {
                stream.invalidate();
            }
            else {
                setTimeTz(time, offset);
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
void bdet_TimeTz::setTimeTz(const bdet_Time& time, int offset)
{
    BSLS_ASSERT_SAFE(isValid(time, offset));

    d_time   = time;
    d_offset = offset;
}

inline
int bdet_TimeTz::validateAndSetTimeTz(const bdet_Time& time, int offset)
{
    if (isValid(  time, offset)) {
        setTimeTz(time, offset);
        return 0;
    }
    return -1;
}

// ACCESSORS
template <class STREAM>
STREAM& bdet_TimeTz::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        d_time.bdexStreamOut(stream, 1);
        stream.putInt32(d_offset);
      } break;
    }
    return stream;
}

inline
bdet_Time bdet_TimeTz::utcTime() const
{
    bdet_Time utc(d_time);
    if (d_offset) {
        utc.addMinutes(-d_offset);
    }
    return utc;
}

inline
bdet_Time bdet_TimeTz::gmtTime() const
{
    return utcTime();
}

inline
bdet_Time bdet_TimeTz::localTime() const
{
    return d_time;
}

inline
int bdet_TimeTz::offset() const
{
    return d_offset;
}

// FREE OPERATORS
inline
bool operator==(const bdet_TimeTz& lhs, const bdet_TimeTz& rhs)
{
    return lhs.localTime() == rhs.localTime()
        && lhs.offset()    == rhs.offset();
}

inline
bool operator!=(const bdet_TimeTz& lhs, const bdet_TimeTz& rhs)
{
    return lhs.localTime() != rhs.localTime()
        || lhs.offset()    != rhs.offset();
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdet_TimeTz& rhs)
{
    return rhs.print(stream, 0, -1);
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
