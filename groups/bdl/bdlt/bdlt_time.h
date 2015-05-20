// bdlt_time.h                                                        -*-C++-*-
#ifndef INCLUDED_BDLT_TIME
#define INCLUDED_BDLT_TIME

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic time-of-day type (millisecond resolution).
//
//@CLASSES:
//  bdlt::Time: time-of-day type 'hh:mm:ss.sss' (having millisecond resolution)
//
//@DESCRIPTION: This component implements a value-semantic time class,
// 'bdlt::Time', that can represent the time of day to a resolution of one
// millisecond (using a 24-hour clock).  Valid time values range from
// 00:00:00.000 (i.e., midnight) through 23:59:59.999.  A time value can be
// specified via four separate integer attribute values denoting hours
// '[ 0 .. 23 ]', minutes '[ 0 .. 59 ]', seconds '[ 0 .. 59 ]', and
// milliseconds '[ 0 ..  999 ]'.  In addition, the 'bdlt::Time' type has one
// more valid value, 24:00:00.000, which can be set explicitly and accessed.
// The value 24:00:00.000 behaves, in most cases, as if it were the value
// 00:00:00.000; however, for all relational comparison operators,
// 24:00:00.000 is not a valid argument and, therefore, would result in
// undefined behavior.  Each of the 'add' manipulators, along with modifying
// the value of the object, return the (signed) number of times that the
// 23:59:59.999 - 00:00:00.000 boundary was crossed in performing the
// addition.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'bdlt::Time' Usage
///- - - - - - - - - - - - - - - - - -
// This example demonstrates how to create and use a 'bdlt::Time' object.
//
// First, create an object 't1' having the default value, and then verify that
// it represents the value 24:00:00.000:
//..
//  bdlt::Time t1;               assert(24 == t1.hour());
//                               assert( 0 == t1.minute());
//                               assert( 0 == t1.second());
//                               assert( 0 == t1.millisecond());
//..
// Then, set 't1' to the value 2:34pm (14:34:00.000):
//..
//  t1.setTime(14, 34);          assert(14 == t1.hour());
//                               assert(34 == t1.minute());
//                               assert( 0 == t1.second());
//                               assert( 0 == t1.millisecond());
//..
// Next, use 'setTimeIfValid' to attempt to assign the invalid value 24:15 to
// 't1', then verify the method returns an error status and the value of 't1'
// is unmodified:
//..
//  int ret = t1.setTimeIfValid(24, 15);
//                               assert( 0 != ret);          // 24:15 is not
//                                                           // valid
//
//                               assert(14 == t1.hour());    // no effect
//                               assert(34 == t1.minute());  // on the
//                               assert( 0 == t1.second());  // object
//                               assert( 0 == t1.millisecond());
//..
// Then, create 't2' as a copy of 't1':
//..
//  bdlt::Time t2(t1);            assert(t1 == t2);
//..
// Next, add 5 minutes and 7 seconds to the value of 't2' (in two steps), and
// confirm the value of 't2':
//..
//  t2.addMinutes(5);
//  t2.addSeconds(7);
//                               assert(14 == t2.hour());
//                               assert(39 == t2.minute());
//                               assert( 7 == t2.second());
//                               assert( 0 == t2.millisecond());
//..
// Then, subtract 't1' from 't2' to yield a 'bdlt::DatetimeInterval' 'dt'
// representing the time-interval between those two times, and verify the value
// of 'dt' is 5 minutes and 7 seconds (or 307 seconds):
//..
//  bdlt::DatetimeInterval dt = t2 - t1;
//                               assert(307 == dt.totalSeconds());
//..
// Finally, stream the value of 't2' to 'stdout':
//..
//  bsl::cout << t2 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  14:39:07.000
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMEINTERVAL
#include <bdlt_datetimeinterval.h>
#endif

#ifndef INCLUDED_BDLT_TIMEUNITRATIO
#include <bdlt_timeunitratio.h>
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

                        // ==========
                        // class Time
                        // ==========

class Time {
    // This class implements a value-semantic type that represents the time of
    // day to a resolution of one millisecond.  Each object of this (almost)
    // simply constrained attribute class *always* represents a valid time
    // value to a resolution of one millisecond.  The valid range for times is
    // 00:00:00.000 through 23:59:59.999, except that 24:00:00.000 represents
    // the default-constructed value.  The value 24:00:00.000 behaves, in most
    // cases, as if it were the value 00:00:00.000; however for all relational
    // comparison operators, 24:00:00.000 is not a valid argument and,
    // therefore, would result in undefined behavior.  Each add operation on a
    // 'Time' object will return the (signed) number of times that the
    // 23:59:59.999 - 00:00:00.000 boundary was crossed while performing the
    // operation.  Attempting to construct a 'Time' with any attribute outside
    // its valid range (or with an hour attribute value of 24, and any other
    // attribute non-zero) will result in undefined behavior.

    // DATA
    int d_milliseconds;  // offset from 00:00:00.000; 86,400 is the default

    // FRIENDS
    friend DatetimeInterval operator-(const Time&, const Time&);
    friend bool operator==(const Time&, const Time&);
    friend bool operator!=(const Time&, const Time&);
    friend bool operator< (const Time&, const Time&);
    friend bool operator<=(const Time&, const Time&);
    friend bool operator>=(const Time&, const Time&);
    friend bool operator> (const Time&, const Time&);

  public:
    // CLASS METHODS
    static bool isValid(int hour,
                        int minute      = 0,
                        int second      = 0,
                        int millisecond = 0);
        // Return 'true' if the specified 'hour', and the optionally specified
        // 'minute', 'second', and 'millisecond', represent a valid 'Time'
        // value, and 'false' otherwise.  Unspecified arguments default to 0.
        // The 'hour', 'minute', 'second', and 'millisecond' attributes
        // comprise a valid 'Time' value if '0 <= hour < 24',
        // '0 <= minute < 60', '0 <= second < 60', and
        // '0 <= millisecond < 1000'.  Additionally, 24:00:00.000 also
        // represents a valid 'Time' value.

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
    Time();
        // Create a 'Time' object having the value 24:00:00.000.

    explicit
    Time(int hour, int minute = 0, int second = 0, int millisecond = 0);
        // Create a 'Time' object having the (valid) value represented by the
        // specified 'hour', and the optionally specified 'minute', 'second',
        // and 'millisecond'.  Unspecified arguments default to 0.  The
        // behavior is undefined unless all of the specified values are within
        // their valid ranges (see 'isValid').

    Time(const Time& original);
        // Create a 'Time' object having the value of the specified 'original'
        // time.

    ~Time();
        // Destroy this time object.

    // MANIPULATORS
    Time& operator=(const Time& rhs);
        // Assign to this time object the value of the specified 'rhs' object,
        // and return a reference providing modifiable access to this object.

    Time& operator+=(const DatetimeInterval& rhs);
        // Add to this time object the value of the specified 'rhs' datetime
        // interval, and return a reference providing modifiable access to this
        // object.

    Time& operator-=(const DatetimeInterval& rhs);
        // Subtract from this time object the value of the specified 'rhs'
        // datetime interval, and return a reference providing modifiable
        // access to this object.

    int addHours(int hours);
        // Increase the value of this time object by the specified number of
        // 'hours', and return the (signed) number of times that the
        // 23:59:59.999 - 00:00:00.000 boundary was crossed in performing the
        // operation.  Note that 'hours' may be negative.

    int addMinutes(int minutes);
        // Increase the value of this time object by the specified number of
        // 'minutes', and return the (signed) number of times that the
        // 23:59:59.999 - 00:00:00.000 boundary was crossed in performing the
        // operation.  Note that 'minutes' may be negative.

    int addSeconds(int seconds);
        // Increase the value of this time object by the specified number of
        // 'seconds', and return the (signed) number of times that the
        // 23:59:59.999 - 00:00:00.000 boundary was crossed in performing the
        // operation.  Note that 'seconds' may be negative.

    int addMilliseconds(int milliseconds);
        // Increase the value of this time object by the specified number of
        // 'milliseconds', and return the (signed) number of times that the
        // 23:59:59.999 - 00:00:00.000 boundary was crossed in performing the
        // operation.  Note that 'milliseconds' may be negative.

    int addInterval(const DatetimeInterval& interval);
        // Increase the value of this time object by the specified 'interval'
        // of time, and return the (signed) number of times that the
        // 23:59:59.999 - 00:00:00.000 boundary was crossed in performing the
        // operation.  The behavior is undefined unless the number of crossings
        // that would be returned can be represented by an 'int'.

    int addTime(int hours,
                int minutes      = 0,
                int seconds      = 0,
                int milliseconds = 0);
        // Add to the value of this time object the specified (signed) number
        // of 'hours' and optionally specified (signed) numbers of 'minutes',
        // 'seconds', and 'milliseconds'; return the (signed) number of times
        // that the 23:59:59.999 - 00:00:00.000 boundary was crossed in
        // performing the operation.  Unspecified arguments default to 0.

    void setHour(int hour);
        // Set the 'hour' attribute of this time object to the specified
        // 'hour'; if 'hour' is 24, set the remaining attributes of this object
        // to 0.  The behavior is undefined unless '0 <= hour <= 24'.

    void setMinute(int minute);
        // Set the 'minute' attribute of this time object to the specified
        // 'minute'; if the 'hour' attribute is 24, set the 'hour' attribute to
        // 0.  The behavior is undefined unless '0 <= minute < 60'.

    void setSecond(int second);
        // Set the 'second' attribute of this time object to the specified
        // 'second'; if the 'hour' attribute is 24, set the 'hour' attribute to
        // 0.  The behavior is undefined unless '0 <= second < 60'.

    void setMillisecond(int millisecond);
        // Set the 'millisecond' attribute of this time object to the specified
        // 'millisecond'; if the 'hour' attribute is 24, set the 'hour'
        // attribute to 0.  The behavior is undefined unless
        // '0 <= millisecond < 1000'.

    void setTime(int hour,
                 int minute      = 0,
                 int second      = 0,
                 int millisecond = 0);
        // Set the value of this time object to the specified 'hour', and the
        // optionally specified 'minute', 'second', and 'millisecond'.
        // Unspecified arguments default to 0.  The behavior is undefined
        // unless all of the specified value are within their valid ranges (see
        // 'isValid').

    int setTimeIfValid(int hour,
                       int minute      = 0,
                       int second      = 0,
                       int millisecond = 0);
        // Set the value of this time object to the specified 'hour', and the
        // optionally specified 'minute', 'second', and 'millisecond', if they
        // would comprise a valid 'Time' value (see 'isValid').  Return 0 on
        // success, and a non-zero value (with no effect) otherwise.
        // Unspecified arguments default to 0.

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
    void getTime(int *hour,
                 int *minute = 0,
                 int *second = 0,
                 int *millisecond = 0) const;
        // Load, into the specified 'hour', and the optionally specified
        // 'minute', 'second' and 'millisecond', the respective 'hour',
        // 'minute', 'second', and 'millisecond' attribute values from this
        // time object.  Unspecified arguments default to 0.  Supplying 0 for
        // an address argument suppresses the loading of the value for the
        // corresponding attribute, but has no effect on the loading of other
        // attribute values.

    int hour() const;
        // Return the value of the 'hour' attribute of this time object.

    int minute() const;
        // Return the value of the 'minute' attribute of this time object.

    int second() const;
        // Return the value of the 'second' attribute of this time object.

    int millisecond() const;
        // Return the value of the 'millisecond' attribute of this time object.

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
        // valid on entry, this operation has no effect.  Note that this
        // human-readable format is not fully specified, and can change without
        // notice.


};

// FREE OPERATORS
Time operator+(const Time& lhs, const DatetimeInterval& rhs);
    // Return a 'Time' value that is the sum of the specified 'lhs' time and
    // the specified 'rhs' datetime interval.

Time operator+(const DatetimeInterval& lhs, const Time& rhs);
    // Return a 'Time' value that is the sum of the specified 'lhs' datetime
    // interval and the specified 'rhs' time.

Time operator-(const Time& lhs, const DatetimeInterval& rhs);
    // Return a 'Time' value that is the difference between the specified 'lhs'
    // time and the specified 'rhs' datetime interval.

DatetimeInterval operator-(const Time& lhs, const Time& rhs);
    // Return a 'DatetimeInterval' object initialized with the difference
    // between the specified 'lhs' and 'rhs' time values.

bool operator==(const Time& lhs, const Time& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' time objects have the
    // same value, and 'false' otherwise.  Two time objects have the same value
    // if each of their corresponding 'hour', 'minute', 'second', and
    // 'millisecond' attributes respectively have the same value.

bool operator!=(const Time& lhs, const Time& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' time objects do not have
    // the same value, and 'false' otherwise.  Two time objects do not have the
    // same value if any of their corresponding 'hour', 'minute', 'second', and
    // 'millisecond' attributes respectively do not have the same value.

bool operator<(const Time& lhs, const Time& rhs);
    // Return 'true' if the specified 'lhs' time value is less than the
    // specified 'rhs' time value, and 'false' otherwise.  The behavior is
    // undefined unless 'lhs != Time()' and 'rhs != Time()' (i.e., they do not
    // have the, default, value 24:00:00.000).

bool operator<=(const Time& lhs, const Time& rhs);
    // Return 'true' if the specified 'lhs' time value is less than or equal to
    // the specified 'rhs' time value, and 'false' otherwise.  The behavior is
    // undefined unless 'lhs != Time()' and 'rhs != Time()' (i.e., they do not
    // have the, default, value 24:00:00.000).

bool operator>(const Time& lhs, const Time& rhs);
    // Return 'true' if the specified 'lhs' time value is greater than the
    // specified 'rhs' time value, and 'false' otherwise.  The behavior is
    // undefined unless 'lhs != Time()' and 'rhs != Time()' (i.e., they do not
    // have the, default, value 24:00:00.000).

bool operator>=(const Time& lhs, const Time& rhs);
    // Return 'true' if the specified 'lhs' time value is greater than or equal
    // to the specified 'rhs' time value, and 'false' otherwise.  The behavior
    // is undefined unless 'lhs != Time()' and 'rhs != Time()' (i.e., they do
    // not have the, default, value 24:00:00.000).

bsl::ostream& operator<<(bsl::ostream& stream, const Time& time);
    // Write the value of the specified 'time' object to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                                 // ----------
                                 // class Time
                                 // ----------

// CLASS METHODS
inline
bool Time::isValid(int hour, int minute, int second, int millisecond)
{
    return (0 <= hour        && hour     < bdlt::TimeUnitRatio::k_H_PER_D_32 &&
            0 <= minute      && minute   < bdlt::TimeUnitRatio::k_M_PER_H_32 &&
            0 <= second      && second   < bdlt::TimeUnitRatio::k_S_PER_M_32 &&
            0 <= millisecond && millisecond
                                         < bdlt::TimeUnitRatio::k_MS_PER_S_32)
        || (bdlt::TimeUnitRatio::k_H_PER_D_32 == hour &&
            0                                 == minute &&
            0                                 == second &&
            0                                 == millisecond);
}

                                  // Aspects

inline
int Time::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// CREATORS
inline
Time::Time()
: d_milliseconds(bdlt::TimeUnitRatio::k_MS_PER_D_32)
{
}

inline
Time::Time(int hour, int minute, int second, int millisecond)
{
    BSLS_ASSERT_SAFE(isValid(hour, minute, second, millisecond));

    d_milliseconds = millisecond
                   + second * bdlt::TimeUnitRatio::k_MS_PER_S_32
                   + minute * bdlt::TimeUnitRatio::k_MS_PER_M_32
                   + hour * bdlt::TimeUnitRatio::k_MS_PER_H_32;
}

inline
Time::Time(const Time& original)
: d_milliseconds(original.d_milliseconds)
{
}

inline
Time::~Time()
{
    BSLS_ASSERT_SAFE(0 <= d_milliseconds);
    BSLS_ASSERT_SAFE(     d_milliseconds
                                        <= bdlt::TimeUnitRatio::k_MS_PER_D_32);
}

// MANIPULATORS
inline
Time& Time::operator=(const Time& rhs)
{
    d_milliseconds = rhs.d_milliseconds;
    return *this;
}

inline
Time& Time::operator+=(const DatetimeInterval& rhs)
{
    addMilliseconds(static_cast<int>(rhs.totalMilliseconds()
                                     % bdlt::TimeUnitRatio::k_MS_PER_D));
    return *this;
}

inline
Time& Time::operator-=(const DatetimeInterval& rhs)
{
    addMilliseconds(static_cast<int>(-rhs.totalMilliseconds()
                                     % bdlt::TimeUnitRatio::k_MS_PER_D));
    return *this;
}

inline
int Time::setTimeIfValid(int hour,
                         int minute,
                         int second,
                         int millisecond)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (isValid(hour, minute, second, millisecond)) {
        setTime(hour, minute, second, millisecond);
        return k_SUCCESS;                                             // RETURN
    }

    return k_FAILURE;
}

                                  // Aspects

template <class STREAM>
STREAM& Time::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            int tmp;
            stream.getInt32(tmp);

            if (   stream
                && static_cast<unsigned int>(tmp) <=
                   static_cast<unsigned int>(
                                         bdlt::TimeUnitRatio::k_MS_PER_D_32)) {
                d_milliseconds = tmp;
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
int Time::hour() const
{
    return d_milliseconds / bdlt::TimeUnitRatio::k_MS_PER_H_32;
}

inline
int Time::minute() const
{
    return (d_milliseconds % bdlt::TimeUnitRatio::k_MS_PER_H_32)
                           / bdlt::TimeUnitRatio::k_MS_PER_M_32;
}

inline
int Time::second() const
{
    return (d_milliseconds % bdlt::TimeUnitRatio::k_MS_PER_M_32)
                           / bdlt::TimeUnitRatio::k_MS_PER_S_32;
}

inline
int Time::millisecond() const
{
    return d_milliseconds % bdlt::TimeUnitRatio::k_MS_PER_S_32;
}

                                  // Aspects

template <class STREAM>
STREAM& Time::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            stream.putInt32(d_milliseconds);
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
bdlt::Time bdlt::operator+(const Time& lhs, const DatetimeInterval& rhs)
{
    Time result(lhs);
    return result += rhs;
}

inline
bdlt::Time bdlt::operator+(const DatetimeInterval& lhs, const Time& rhs)
{
    Time result(rhs);
    return result += lhs;
}

inline
bdlt::Time bdlt::operator-(const Time& lhs, const DatetimeInterval& rhs)
{
    Time result(lhs);
    return result -= rhs;
}

inline
bdlt::DatetimeInterval bdlt::operator-(const Time& lhs, const Time& rhs)
{
    DatetimeInterval timeInterval;
    timeInterval.setTotalMilliseconds(
                      lhs.d_milliseconds % bdlt::TimeUnitRatio::k_MS_PER_D_32
                    - rhs.d_milliseconds % bdlt::TimeUnitRatio::k_MS_PER_D_32);
    return timeInterval;
}

inline
bool bdlt::operator==(const Time& lhs, const Time& rhs)
{
    return lhs.d_milliseconds == rhs.d_milliseconds;
}

inline
bool bdlt::operator!=(const Time& lhs, const Time& rhs)
{
    return lhs.d_milliseconds != rhs.d_milliseconds;
}

inline
bool bdlt::operator<(const Time& lhs, const Time& rhs)
{
    BSLS_ASSERT_SAFE(bdlt::TimeUnitRatio::k_MS_PER_D_32 != lhs.d_milliseconds);
    BSLS_ASSERT_SAFE(bdlt::TimeUnitRatio::k_MS_PER_D_32 != rhs.d_milliseconds);

    return lhs.d_milliseconds < rhs.d_milliseconds;
}

inline
bool bdlt::operator<=(const Time& lhs, const Time& rhs)
{
    BSLS_ASSERT_SAFE(bdlt::TimeUnitRatio::k_MS_PER_D_32 != lhs.d_milliseconds);
    BSLS_ASSERT_SAFE(bdlt::TimeUnitRatio::k_MS_PER_D_32 != rhs.d_milliseconds);

    return lhs.d_milliseconds <= rhs.d_milliseconds;
}

inline
bool bdlt::operator>(const Time& lhs, const Time& rhs)
{
    BSLS_ASSERT_SAFE(bdlt::TimeUnitRatio::k_MS_PER_D_32 != lhs.d_milliseconds);
    BSLS_ASSERT_SAFE(bdlt::TimeUnitRatio::k_MS_PER_D_32 != rhs.d_milliseconds);

    return lhs.d_milliseconds > rhs.d_milliseconds;
}

inline
bool bdlt::operator>=(const Time& lhs, const Time& rhs)
{
    BSLS_ASSERT_SAFE(bdlt::TimeUnitRatio::k_MS_PER_D_32 != lhs.d_milliseconds);
    BSLS_ASSERT_SAFE(bdlt::TimeUnitRatio::k_MS_PER_D_32 != rhs.d_milliseconds);

    return lhs.d_milliseconds >= rhs.d_milliseconds;
}

inline
bsl::ostream& bdlt::operator<<(bsl::ostream& stream, const Time& time)
{
    return time.print(stream, 0, -1);
}

}  // close enterprise namespace

namespace bsl {

// TRAITS
template <>
struct is_trivially_copyable<BloombergLP::bdlt::Time> : bsl::true_type {
    // This template specialization for 'is_trivially_copyable' indicates that
    // 'bdlt::Time' is a trivially copyable type.
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
