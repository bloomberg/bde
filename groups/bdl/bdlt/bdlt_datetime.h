// bdlt_datetime.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLT_DATETIME
#define INCLUDED_BDLT_DATETIME

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic type representing both date and time.
//
//@CLASSES:
//  bdlt::Datetime: date and time value (at least microsecond resolution)
//
//@SEE_ALSO: bdlt_date, bdlt_time, bdlt_datetimetz
//
//@AUTHOR: John Lakos (jlakos), Steven Breitstein (sbreitstein)
//
//@DESCRIPTION: This component implements a value-semantic type,
// 'bdlt::Datetime', that represents the composition of a date and time value.
// For common dates (those from 1970/01/01 through 2470/01/01), the combined
// "date+time" value of a 'bdlt::Datetime' object is expressed textually as
// "yyyy/mm/dd_hh:mm:ss.sssssssss", where "yyyy/mm/dd" represents the "date"
// part of the value and "hh:mm:ss.sssssssss" represents the "time" part.  For
// dates outside this range, the value is expressed textually as
// "yyyy/mm/dd_hh:mm:ss.ssssss".
//
// In addition to the usual value-semantic complement of methods for getting
// and setting value, the 'bdlt::Datetime' class provides methods and operators
// for making relative adjustments to value ('addDays', 'addTime', 'addHours',
// etc.). In particular, note that adding units of time to a 'bdlt::Datetime'
// object can affect the values of both the time and date parts of the object.
// For example, invoking 'addHours(2)' on a 'bdlt::Datetime' object whose value
// is "1987/10/03_22:30:00.000" updates the value to "1987/10/04_00:30:00.000".
//
///Valid 'bdlt::Datetime' Values and Their Representations
///-------------------------------------------------------
// The "date" part of a 'bdlt::Datetime' value has a range of validity
// identical to a 'bdlt::Date' object -- i.e., valid dates (according to the
// Unix [POSIX] calendar) having years in the range '[1 .. 9999]'.  The set of
// valid "time" values for a 'bdlt::Datetime' object is dependent on the date.
// For dates in the range [1970/01/01 .. 2470/01/01), the valid time values are
// '[00:00:00.000000000 .. 23:59:59.999999999]'.  For valid dates not in this
// range, the valid time values are '[00:00:00.000000 .. 23:59:59.999999]'.
// Furthermore, the unset time value (i.e., 24:00:00.000000, corresponding to
// the default constructed value for 'bdlt::Time') is available for every valid
// date.  Note that the supported range of time does *not* allow for the
// injection of leap seconds.  The value "0001/01/01_24:00:00.000000" is the
// default constructed value of 'bdlt::Datetime'.
//
// Furthermore, consistent with the 'bdlt::Time' type, a 'bdlt::Datetime'
// object whose "time" part has the default constructed value, behaves the
// same, with respect to manipulators and (most) free operators, as if the
// "time" part had the value 00:00:00.000000.  As for 'bdlt::Time', the
// behavior of all 'bdlt::Datetime' relational comparison operators is
// undefined if the "time" part of either operand is 24:00:00.000000.
// Consequently, 'bdlt::Datetime' objects whose "time" part has the default
// constructed value must *not* be used as keys for the standard associative
// containers, since 'operator<' is not defined for such objects.
//
// Finally, when the value of the date portion of a 'bdlt::Datetime' object is
// not in the range [1970/01/01 .. 2470/01/01), the nanosecond portion of the
// value is always 0.  In other words, any manipulation resulting in a value
// whose date portion is outside [1970/01/01 .. 2470/01/01) will have the value
// 0 for the nanosecond portion of its value.  Furthermore, any access of the
// nanosecond portion of the value for which the date portion is outside
// [1970/01/01 .. 2470/01/01) will obtain the value 0 for the nanosecond
// portion of the access.
//
///Attributes
///----------
// Conceptually, the two primary attributes of 'bdlt::Datetime' are the
// constituent date and time values.  These attributes are given the special
// designation "part" in this component (i.e., the "time" part and the "date"
// part, respectively) to distinguish them from the many other attributes (see
// below) that derive from these two main parts.
//..
//  Name  Type        Default          Range
//  ----  ----------  ---------------  ------------------------------
//  date  bdlt::Date  0001/01/01       [0001/01/01      .. 9999/12/31]
//  time  bdlt::Time  24:00:00.000000  [00:00:00.000000 .. 23:59:59.999999]*
//..
// (*) Note that the range has higher precision for common dates.
//
// A 'bdlt::Datetime' object can be used in terms of its "date" and "time"
// parts or, if appropriate to an application, the object can be viewed as a
// single, integrated type having the combined individual attributes of
// date and time.  Accessors and manipulators are provided for each of these
// nine (derived) attributes:
//..
//  Name         Type  Default  Range        Constraint
//  -----------  ----  -------  -----------  -----------------------------
//  year         int    1       [1 .. 9999]  none
//  month        int    1       [1 ..   12]  none
//  day          int    1       [1 ..   31]  must exist for year and month
//  hour         int   24       [0 ..   24]  none
//  minute       int    0       [0 ..   59]  must be 0 if '24 == hour'
//  second       int    0       [0 ..   59]  must be 0 if '24 == hour'
//  millisecond  int    0       [0 ..  999]  must be 0 if '24 == hour'
//  microsecond  int    0       [0 ..  999]  must be 0 if '24 == hour'
//  nanosecond   int    0       [0 ..  999]  must be 0 if '24 == hour'
//..
// There are two additional "date" part attributes to 'bdlt::Datetime':
//..
//  Name      Type                  Default Range        Constraint
//  --------- --------------------- ------- ------------ ----------------------
//  dayOfYear int                   1       [  1 .. 366] 366 only on leap years
//  dayOfWeek bdlt::DayOfWeek::Enum SAT     [SUN .. SAT] tied to calendar day
//..
// where 'dayOfYear' tracks the value of 'year/month/day' (and *vice* *versa*),
// and 'dayOfWeek' can be accessed but not explicitly set.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// Values represented by objects of type 'bdlt::Datetime' are used widely in
// practice.  The values of the individual attributes resulting from a
// default-constructed 'bdlt::Datetime' object, 'dt', are
// "0001/01/01_24:00:00.000000000":
//..
//  bdlt::Datetime dt;          assert( 1 == dt.date().year());
//                              assert( 1 == dt.date().month());
//                              assert( 1 == dt.date().day());
//                              assert(24 == dt.time().hour());
//                              assert( 0 == dt.time().minute());
//                              assert( 0 == dt.time().second());
//                              assert( 0 == dt.time().millisecond());
//                              assert( 0 == dt.time().microsecond());
//                              assert( 0 == dt.time().nanosecond());
//..
// We can then set 'dt' to have a specific value, say, 8:43pm on January 6,
// 2013:
//..
//  dt.setDatetime(2013, 1, 6, 20, 43);
//                              assert(2013 == dt.date().year());
//                              assert(   1 == dt.date().month());
//                              assert(   6 == dt.date().day());
//                              assert(  20 == dt.time().hour());
//                              assert(  43 == dt.time().minute());
//                              assert(   0 == dt.time().second());
//                              assert(   0 == dt.time().millisecond());
//                              assert(   0 == dt.time().microsecond());
//                              assert(   0 == dt.time().nanosecond());
//..
// Now suppose we add 6 hours and 9 seconds to this value.  There is more than
// one way to do it:
//..
//  bdlt::Datetime dt2(dt);
//  dt2.addHours(6);
//  dt2.addSeconds(9);
//                              assert(2013 == dt2.date().year());
//                              assert(   1 == dt2.date().month());
//                              assert(   7 == dt2.date().day());
//                              assert(   2 == dt2.time().hour());
//                              assert(  43 == dt2.time().minute());
//                              assert(   9 == dt2.time().second());
//                              assert(   0 == dt2.time().millisecond());
//                              assert(   0 == dt2.time().microsecond());
//                              assert(   0 == dt2.time().nanosecond());
//
//  bdlt::Datetime dt3(dt);
//  dt3.addTime(6, 0, 9);
//                              assert(dt2 == dt3);
//..
// Notice that (in both cases) the date changed as a result of adding time;
// however, changing just the date never affects the time:
//..
//  dt3.addDays(10);
//                              assert(2013 == dt3.date().year());
//                              assert(   1 == dt3.date().month());
//                              assert(  17 == dt3.date().day());
//                              assert(   2 == dt3.time().hour());
//                              assert(  43 == dt3.time().minute());
//                              assert(   9 == dt3.time().second());
//                              assert(   0 == dt3.time().millisecond());
//                              assert(   0 == dt2.time().millisecond());
//                              assert(   0 == dt2.time().microsecond());
//                              assert(   0 == dt2.time().nanosecond());
//..
// We can also add more than a day's worth of time:
//..
//  dt2.addHours(240);
//                              assert(dt3 == dt2);
//..
// The individual arguments can also be negative:
//..
//  dt2.addTime(-246, 0, -10, 1000);  // -246 h, -10 s, +1000 ms
//                              assert(dt == dt2);
//..
// Finally, we stream the value of 'dt2' to 'stdout':
//..
//  bsl::cout << dt2 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  06JAN2013_20:43:00.000000000
//..
//
///Example 2: Creating a Schedule of Equal Time Intervals
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Calculations involving date and time values are difficult to get correct
// manually; consequently, people tend to schedule events on natural time
// boundaries (e.g., on the hour) even if that is sub-optimal.  Having a class
// such as 'bdlt::Datetime' makes doing date and time calculations trivial.
//
// Suppose one wants to divide into an arbitrary interval such as the time
// between sunset and sunrise into an arbitrary number (say 7) of equal
// intervals (perhaps to use as a duty roster for teams making astronomical
// observations).
//
// First, we create objects containing values for the start and end of the time
// interval:
//..
//  bdlt::Datetime  sunset(2014, 6, 26, 20, 31, 23); // New York City
//  bdlt::Datetime sunrise(2014, 6, 27,  5, 26, 51); // New York City
//..
// Then, we calculate the length of each shift in milliseconds (for good
// precision -- we may be synchronizing astronomical instruments).  Note that
// the difference of 'sunrise' and 'sunset' creates a temporary
// 'bdlt::DatetimeInterval' object:
//..
//  const int                numShifts = 7;
//  const bsls::Types::Int64 shiftLengthInMsec
//                                     = (sunrise - sunset).totalMilliseconds()
//                                     / numShifts;
//..
// Now, we calculate (and print to 'stdout') the beginning and end times for
// each shift:
//..
//  for (int i = 0; i <= numShifts; ++i) {
//      bdlt::Datetime startOfShift(sunset);
//      startOfShift.addMilliseconds(shiftLengthInMsec * i);
//      bsl::cout << startOfShift << bsl::endl;
//  }
//..
// Finally, we observe:
//..
//  26JUN2014_20:31:23.000000000
//  26JUN2014_21:47:52.714000000
//  26JUN2014_23:04:22.428000000
//  27JUN2014_00:20:52.142000000
//  27JUN2014_01:37:21.856000000
//  27JUN2014_02:53:51.570000000
//  27JUN2014_04:10:21.284000000
//  27JUN2014_05:26:50.998000000
//..
// Notice how our objects (since they manage both "date" and "time of day"
// parts of each point in time) seamlessly handle the transition between the
// two days.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMEIMPUTIL
#include <bdlt_datetimeimputil.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMEINTERVAL
#include <bdlt_datetimeinterval.h>
#endif

#ifndef INCLUDED_BDLT_DAYOFWEEK
#include <bdlt_dayofweek.h>
#endif

#ifndef INCLUDED_BDLT_TIME
#include <bdlt_time.h>
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

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bdlt {

                              // ==============
                              // class Datetime
                              // ==============

class Datetime {
    // This class implements a simply-constrained value-semantic type
    // representing the composition of date and time values.  Valid date values
    // for the "date" part of a 'Datetime' object are the same as those
    // defined for 'Date' objects; similarly, valid time values for the "time"
    // part of a 'Datetime' object are the similar to those defined for 'Time'
    // objects (but with additional precision).  Relational operators are
    // disallowed on 'Datetime' objects whose "time" part has the same value
    // as that of a default consaructed 'Time' object.

    // DATA
    bsls::Types::Uint64 d_value;  // encoded offset from the epoch

    // FRIENDS
    friend DatetimeInterval operator-(const Datetime&, const Datetime&);

    friend bool operator==(const Datetime&, const Datetime&);
    friend bool operator!=(const Datetime&, const Datetime&);
    friend bool operator< (const Datetime&, const Datetime&);
    friend bool operator<=(const Datetime&, const Datetime&);
    friend bool operator> (const Datetime&, const Datetime&);
    friend bool operator>=(const Datetime&, const Datetime&);

    // PRIVATE MANIPULATOR
    void setTimeIntervalFromEpoch(const bsls::TimeInterval& interval);
        // Assign to 'd_value' the representation of a datetime such that the
        // difference between this datetime and the epoch is the specified
        // 'interval' (potentially truncated at microseconds depending on the
        // date portion of the resultant datetime value).

    void setUnset(int totalDays);
        // Assign to 'd_value' the representation of a datetime such that the
        // difference, measured at day accuracy, between this datetime and the
        // epoch is the specified 'totalDays' and the time portion of this
        // datetime corresponds to a default constructed 'bdlt::Time' value.

    // PRIVATE ACCESSORS
    void timeIntervalFromEpoch(bsls::TimeInterval *interval) const;
        // Load into the specified 'interval' the difference between this
        // datetime value, with 24:00:00.000000 converted to 0:00:00.000000,
        // and the epoch.

    void timeIntervalFromEpoch(bsls::TimeInterval *interval,
                               bool               *unset) const;
        // Load into the specified 'interval' the difference between this
        // datetime value, with 24:00:00.000000 converted to 0:00:00.000000,
        // and the epoch and load into the specified 'unset' the value 'true'
        // if the time portion of this datetime has the value 24:00:00.000000,
        // and 'false' otherwise.

  public:
    // CLASS METHODS
    static bool isValid(int year,
                        int month,
                        int day,
                        int hour = 0,
                        int minute = 0,
                        int second = 0,
                        int millisecond = 0,
                        int microsecond = 0,
                        int naoosecond = 0);
        // Return 'true' if the specified 'year', 'month', and 'day' attribute
        // values, and the optionally specified 'hour', 'minute', 'second', 'millisecond', 'microsecond', and
        // 'nanosecond' attribute values, represent a valid 'Datetime' value,
        // and 'false' otherwise.  Unspecified trailing optional parameters
        // default to 0.  'year', 'month', 'day', 'hour', 'minute', 'second',
        // 'millisecond', 'microsecond', and 'nanosecond' attribute values represent a valid 'Datetime'
        // value if 'true == Date::isValidYearMonthDay(year, month, day)',
        // '0 <= hour < 24', '0 <= minute < 60', '0 <= second < 60',
        // '0 <= millisecond < 1000', '0 <= microsecond < 1000', and '0 <= nanosecond < 1000'.  Additionally, a valid 'year', 'month', 'day' with the time portion equal to 24:00:00.000000000 also
        // represents a valid 'Datetime' value.

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
    Datetime();
        // Create a 'Datetime' object whose "date" and "time" parts have their
        // respective default-constructed values, "0001/01/01" and
        // "24:00:00.000000".

    Datetime(const Date& date);                                     // IMPLICIT
        // Create a 'Datetime' object whose "date" part has the value of the
        // specified 'date' and whose "time" part has the value
        // "00:00:00.000000".

    Datetime(const Date& date, const Time& time);
        // Create a 'Datetime' object whose "date" and "time" parts have the
        // values of the specified 'date' and 'time', respectively.

    Datetime(int year,
             int month,
             int day,
             int hour = 0,
             int minute = 0,
             int second = 0,
             int millisecond = 0,
             int microsecond = 0,
             int nanosecond = 0);
        // Create a 'Datetime' object whose "date" part has the value
        // represented by the specified 'year', 'month', and 'day' attributes,
        // and whose "time" part has the value represented by the optionally
        // specified 'hour', 'minute', 'second', 'millisecond', 'microsecond', and 'nanosecond' attributes.
        // Unspecified trailing optional parameters default to 0.  The behavior
        // is undefined unless the nine attributes (collectively) represent a
        // valid 'Datetime' value (see 'isValid').

    Datetime(const Datetime& original);
        // Create a 'Datetime' object having the value of the specified
        // 'original' object.

//! ~Datetime() = default;
        // Destroy this 'Datetime' object.

    // MANIPULATORS
    Datetime& operator=(const Datetime& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    Datetime& operator+=(const DatetimeInterval& rhs);
        // Add to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  If
        // '24 == hour()' on entry, set the 'hour' attribute of this object to
        // 0 before performing the addition.  The behavior is undefined unless
        // the resulting value is valid for 'Datetime' (see 'isValid').

    Datetime& operator-=(const DatetimeInterval& rhs);
        // Subtract from this object the value of the specified 'rhs' object,
        // and return a reference providing modifiable access to this object.
        // If '24 == hour()' on entry, set the 'hour' attribute of this object
        // to 0 before performing the subtraction.  The behavior is undefined
        // unless the resulting value is valid for 'Datetime' (see 'isValid').

    void setDatetime(const Date& date,
                     int         hour = 0,
                     int         minute = 0,
                     int         second = 0,
                     int         millisecond = 0,
                     int         microsecond = 0,
                     int         nanosecond = 0);
        // Set the value of this object to a 'Datetime' whose "date" part has the value
        // represented by the specified 'date',
        // and whose "time" part has the value represented by the optionally
        // specified 'hour', 'minute', 'second', 'millisecond', 'microsecond', and 'nanosecond' attributes.
        // Unspecified trailing optional parameters default to 0.  The behavior
        // is undefined unless the attributes (collectively) represent a
        // valid 'Datetime' value (see 'isValid').

    void setDatetime(const Date& date, const Time& time);
        // Set the value of this object to a 'Datetime' whose "date" part has the value
        // represented by the specified 'date', and whose "time" part has the value represented by the specified 'time'.

    void setDatetime(int year,
                     int month,
                     int day,
                     int hour = 0,
                     int minute = 0,
                     int second = 0,
                     int millisecond = 0,
                     int microsecond = 0,
                     int nanosecond = 0);
        // Set the value of this object to a 'Datetime' whose "date" part has the value
        // represented by the specified 'year', 'month', and 'day' attributes,
        // and whose "time" part has the value represented by the optionally
        // specified 'hour', 'minute', 'second', 'millisecond', 'microsecond', and 'nanosecond' attributes.
        // Unspecified trailing optional parameters default to 0.  The behavior
        // is undefined unless the nine attributes (collectively) represent a
        // valid 'Datetime' value (see 'isValid').

    int setDatetimeIfValid(int year,
                           int month,
                           int day,
                           int hour = 0,
                           int minute = 0,
                           int second = 0,
                           int millisecond = 0);
        // Set the "date" part of this object to have the value represented by
        // the specified 'year', 'month', and 'day' attributes, and set the
        // "time" part to have the value represented by the optionally
        // specified 'hour', 'minute', 'second', and 'millisecond' attributes,
        // *if* the seven attribute values (collectively) represent a valid
        // 'Datetime' value (see 'isValid').  Unspecified trailing optional
        // parameters default to 0.  Return 0 on success, and a non-zero value
        // (with no effect) otherwise.

    void setDate(const Date& date);
        // Set the "date" part of this object to have the value of the
        // specified 'date'.  Note that this method has no effect on the "time"
        // part of this object.

    void setYearDay(int year, int dayOfYear);
        // Set the "date" part of this object to have the value represented by
        // the specified 'year' and 'dayOfYear' attribute values.  The behavior
        // is undefined unless 'year' and 'dayOfYear' represent a valid 'Date'
        // value (i.e., 'true == Date::isValidYearDay(year, dayOfYear)').  Note
        // that this method has no effect on the "time" part of this object.

    void setYearMonthDay(int year, int month, int day);
        // Set the "date" part of this object to have the value represented by
        // the specified 'year', 'month', and 'day' attribute values.  The
        // behavior is undefined unless 'year', 'month', and 'day' represent a
        // valid 'Date' value (i.e.,
        // 'true == Date::isValidYearMonthDay(year, month, day)').  Note that
        // this method has no effect on the "time" part of this object.

    void setTime(const Time& time);
        // Set the "time" part of this object to have the value of the
        // specified 'time'.  Note that this method has no effect on the "date"
        // part of this object.

    void setTime(int hour,
                 int minute = 0,
                 int second = 0,
                 int millisecond = 0,
                 int microsecond = 0,
                 int nanosecond = 0);
        // Set the "time" part of this object to have the value represented by
        // the specified 'hour' attribute value and the optionally specified
        // 'minute', 'second', 'millisecond', 'microsecond', and 'nanosecond' attribute values.  Unspecified
        // trailing optional parameters default to 0.  The behavior is
        // undefined unless 'hour', 'minute', 'second', 'millisecond', 'microsecond', and 'nanosecond'
        // represent a valid "time" portion of a 'Datetime' value.  Note
        // that this method has no effect on the "date" part of this object.

    void setHour(int hour);
        // Set the 'hour' attribute of this object to the specified 'hour'
        // value.  If '24 == hour', set the 'minute', 'second', and
        // 'millisecond' attributes to 0.  The behavior is undefined unless
        // '0 <= hour <= 24'.  Note that this method has no effect on the
        // "date" part of this object.

    void setMinute(int minute);
        // Set the 'minute' attribute of this object to the specified 'minute'
        // value.  If '24 == hour()', set the 'hour' attribute to 0.  The
        // behavior is undefined unless '0 <= minute <= 59'.  Note that this
        // method has no effect on the "date" part of this object.

    void setSecond(int second);
        // Set the 'second' attribute of this object to the specified 'second'
        // value.  If '24 == hour()', set the 'hour' attribute to 0.  The
        // behavior is undefined unless '0 <= second <= 59'.  Note that this
        // method has no effect on the "date" part of this object.

    void setMillisecond(int millisecond);
        // Set the 'millisecond' attribute of this object to the specified
        // 'millisecond' value.  If '24 == hour()', set the 'hour' attribute to
        // 0.  The behavior is undefined unless '0 <= millisecond <= 999'.
        // Note that this method has no effect on the "date" part of this
        // object.

    void setMicrosecond(int microsecond);
        // Set the 'microsecond' attribute of this object to the specified
        // 'microsecond' value.  If '24 == hour()', set the 'hour' attribute to
        // 0.  The behavior is undefined unless '0 <= microsecond <= 999'.
        // Note that this method has no effect on the "date" part of this
        // object.

    void setNanosecond(int nanosecond);
        // Set the 'nanosecond' attribute of this object to the specified
        // 'nanosecond' value.  If '24 == hour()', set the 'hour' attribute to
        // 0.  The behavior is undefined unless '0 <= nanosecond <= 999'.
        // Note that this method has no effect on the "date" part of this
        // object.

    void addDays(int days);
        // Add the specified number of 'days' to the value of this object.  The
        // behavior is undefined unless the resulting value is in the valid
        // range for a 'Datetime' object.  Note that this method has no effect
        // on the "time" part of this object.  Also note that 'days' may be
        // positive, 0, or negative.

    void addTime(bsls::Types::Int64 hours,
                 bsls::Types::Int64 minutes = 0,
                 bsls::Types::Int64 seconds = 0,
                 bsls::Types::Int64 milliseconds = 0,
                 bsls::Types::Int64 microseconds = 0,
                 bsls::Types::Int64 nanoseconds = 0);
        // Add the specified number of 'hours', and the optionally specified
        // number of 'minutes', 'seconds', 'milliseconds', 'microseconds', and 'nanoseconds' to the value of
        // this object, adjusting the "date" part of this object accordingly.
        // Unspecified trailing optional parameters default to 0.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  The behavior is undefined unless the
        // resulting value is in the valid range for a 'Datetime' object.  Note
        // that each argument independently may be positive, negative, or 0.

    void addHours(bsls::Types::Int64 hours);
        // Add the specified number of 'hours' to the value of this object,
        // adjusting the "date" part of the object accordingly.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  The behavior is undefined unless the
        // resulting value is in the valid range for a 'Datetime' object.  Note
        // that 'hours' may be positive, negative, or 0.

    void addMinutes(bsls::Types::Int64 minutes);
        // Add the specified number of 'minutes' to the value of this object,
        // adjusting the "date" part of the object accordingly.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  The behavior is undefined unless the
        // resulting value is in the valid range for a 'Datetime' object.  Note
        // that 'minutes' may be positive, negative, or 0.

    void addSeconds(bsls::Types::Int64 seconds);
        // Add the specified number of 'seconds' to the value of this object,
        // adjusting the "date" part of the object accordingly.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  The behavior is undefined unless the
        // resulting value is in the valid range for a 'Datetime' object.  Note
        // that 'seconds' may be positive, negative, or 0.

    void addMilliseconds(bsls::Types::Int64 milliseconds);
        // Add the specified number of 'milliseconds' to the value of this
        // object, adjusting the "date" part of the object accordingly.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  The behavior is undefined unless the
        // resulting value is in the valid range for a 'Datetime' object.  Note
        // that 'milliseconds' may be positive, negative, or 0.

    void addMicroseconds(bsls::Types::Int64 microseconds);
        // Add the specified number of 'microseconds' to the value of this
        // object, adjusting the "date" part of the object accordingly.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  The behavior is undefined unless the
        // resulting value is in the valid range for a 'Datetime' object.  Note
        // that 'microseconds' may be positive, negative, or 0.

    void addNanoseconds(bsls::Types::Int64 nanoseconds);
        // Add the specified number of 'nanoseconds' to the value of this
        // object, adjusting the "date" part of the object accordingly.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  The behavior is undefined unless the
        // resulting value is in the valid range for a 'Datetime' object.  Note
        // that 'nanoseconds' may be positive, negative, or 0.

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
    Date date() const;
        // Return the value of the "date" part of this object.

    Time time() const;
        // Return the value of the "time" part of this object, truncated to millisecond resolution.

    int year() const;
        // Return the value of the 'year' attribute of this object.

    int month() const;
        // Return the value of the 'month' attribute of this object.

    int day() const;
        // Return the value of the 'day' (of the month) attribute of this
        // object.

    int dayOfYear() const;
        // Return the value of the 'dayOfYear' attribute of this object.

    DayOfWeek::Enum dayOfWeek() const;
        // Return the value of the 'dayOfWeek' attribute associated with the
        // 'day' (of the month) attribute of this object.

    int hour() const;
        // Return the value of the 'hour' attribute of this object.

    int minute() const;
        // Return the value of the 'minute' attribute of this object.

    int second() const;
        // Return the value of the 'second' attribute of this object.

    int millisecond() const;
        // Return the value of the 'millisecond' attribute of this object.

    int microsecond() const;
        // Return the value of the 'microsecond' attribute of this object.

    int nanosecond() const;
        // Return the value of the 'nanosecond' attribute of this object.

    int printToBuffer(char *result, int numBytes) const;
        // Efficiently write to the specified 'result' buffer no more than the
        // specified 'numBytes' of a representation of the value of this
        // object.  Return the number of characters (not including the null
        // character) that would have been written if the limit due to
        // 'numBytes' were not imposed.  'result' is null-terminated unless
        // 'numBytes' is 0.  The behavior is undefined unless '0 <= numBytes'
        // and 'result' refers to at least 'numBytes' contiguous bytes.  Note
        // that the return value is greater than or equal to 'numBytes' if the
        // output representation was truncated to avoid 'result' overrun.

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

#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation
    static int maxSupportedBdexVersion();
        // !DEPRECATED!: Use 'maxSupportedBdexVersion(int)' instead.
        //
        // Return the most current BDEX streaming version number supported by
        // this class.

#endif  // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
    static int maxSupportedVersion();
        // !DEPRECATED!: Use 'maxSupportedBdexVersion(int)' instead.
        //
        // Return the most current BDEX streaming version number supported by
        // this class.

    bsl::ostream& streamOut(bsl::ostream& stream) const;
        // !DEPRECATED!: Use 'print' instead.
        //
        // Format this datetime to the specified output 'stream' and return a
        // reference to the modifiable 'stream'.

    int validateAndSetDatetime(int year,
                               int month,
                               int day,
                               int hour = 0,
                               int minute = 0,
                               int second = 0,
                               int millisecond = 0);
        // !DEPRECATED!: Use 'setDatetimeIfValid' instead.
        //
        // Set the "date" part of this object's value to the specified 'year',
        // 'month', and 'day', and the "time" part to the optionally specified
        // 'hour', 'minute', 'second', and 'millisecond', if they represent a
        // valid 'Datetime' value, with trailing fields that are not specified
        // set to 0.  Return 0 on success, and a non-zero value (with no
        // effect) otherwise.

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22

};

// FREE OPERATORS
Datetime operator+(const Datetime& lhs, const DatetimeInterval& rhs);
    // Return a 'Datetime' object having a value that is the sum of the
    // specified 'lhs' ('Datetime') and the specified 'rhs'
    // ('DatetimeInterval').  If '24 == lhs.hour()', the result is the same as
    // if the 'hour' attribute of 'lhs' is 0.  The behavior is undefined unless
    // the resulting value is in the valid range for a 'Datetime' object.

Datetime operator+(const DatetimeInterval& lhs, const Datetime& rhs);
    // Return a 'Datetime' object having a value that is the sum of the
    // specified 'lhs' ('DatetimeInterval') and the specified 'rhs'
    // ('Datetime').  If '24 == rhs.hour()', the result is the same as if the
    // 'hour' attribute of 'rhs' is 0.  The behavior is undefined unless the
    // resulting value is in the valid range for a 'Datetime' object.

Datetime operator-(const Datetime& lhs, const DatetimeInterval& rhs);
    // Return a 'Datetime' object having a value that is the difference between
    // the specified 'lhs' ('Datetime') and the specified 'rhs'
    // ('DatetimeInterval').  If '24 == lhs.hour()', the result is the same as
    // if the 'hour' attribute of 'lhs' is 0.  The behavior is undefined unless
    // the resulting value is in the valid range for a 'Datetime' object.

DatetimeInterval operator-(const Datetime& lhs, const Datetime& rhs);
    // Return a 'DatetimeInterval' object having a value that is the difference
    // between the specified 'lhs' ('Datetime') and the specified 'rhs'
    // ('Datetime').  If the 'hour' attribute of either operand is 24, the
    // result is the same as if that 'hour' attribute is 0.  The behavior is
    // undefined unless the resulting value is in the valid range for a
    // 'DatetimeInterval' object.

bool operator==(const Datetime& lhs, const Datetime& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Datetime' objects have the same
    // value if they have the same values for their "date" and "time" parts,
    // respectively.

bool operator!=(const Datetime& lhs, const Datetime& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'Datetime' objects do not
    // have the same value, and 'false' otherwise.  Two 'Datetime' objects do
    // not have the same value if they do not have the same values for either
    // of their "date" or "time" parts, respectively.

bool operator<(const Datetime& lhs, const Datetime& rhs);
    // Return 'true' if the value of the specified 'lhs' object is less than
    // the value of the specified 'rhs' object, and 'false' otherwise.  A
    // 'Datetime' object 'a' is less than a 'Datetime' object 'b' if
    // 'a.date() < b.date()', or if
    // 'a.date() == b.date()' and the time portion of 'a' is less than the time portion of 'b'.  The behavior is
    // undefined unless '24 != lhs.hour() && 24 != rhs.hour()'.

bool operator<=(const Datetime& lhs, const Datetime& rhs);
    // Return 'true' if the value of the specified 'lhs' object is less than or
    // equal to the value of the specified 'rhs' object, and 'false' otherwise.
    // The behavior is undefined unless '24 != lhs.hour() && 24 != rhs.hour()'.

bool operator>(const Datetime& lhs, const Datetime& rhs);
    // Return 'true' if the value of the specified 'lhs' object is greater than
    // the value of the specified 'rhs' object, and 'false' otherwise.  A
    // 'Datetime' object 'a' is greater than a 'Datetime' object 'b' if
    // 'a.date() > b.date()', or if
    // 'a.date() == b.date()' and the time portion of 'a' is greater than the time portion of 'b'.  The behavior is
    // undefined unless '24 != lhs.hour() && 24 != rhs.hour()'.

bool operator>=(const Datetime& lhs, const Datetime& rhs);
    // Return 'true' if the value of the specified 'lhs' object is greater than
    // or equal to the value of the specified 'rhs' object, and 'false'
    // otherwise.  The behavior is undefined unless
    // '24 != lhs.hour() && 24 != rhs.hour()'.

bsl::ostream& operator<<(bsl::ostream& stream, const Datetime& object);
    // Write the value of the specified 'object' object to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                              // --------------
                              // class Datetime
                              // --------------

// PRIVATE MANIPULATOR
inline
void Datetime::setTimeIntervalFromEpoch(const bsls::TimeInterval& interval)
{
    if (   BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                    DatetimeImpUtil::k_1970_01_01_TOTAL_SECONDS
                                    <= interval.totalSeconds())
        && BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                    DatetimeImpUtil::k_2470_01_01_TOTAL_SECONDS
                                 >  interval.totalSeconds())) {
        d_value = (  interval.totalSeconds()
                   - DatetimeImpUtil::k_1970_01_01_TOTAL_SECONDS)
                                                    * TimeUnitRatio::k_NS_PER_S
                + interval.nanoseconds()
                + DatetimeImpUtil::k_1970_01_01_VALUE;
    }
    else if (DatetimeImpUtil::k_2470_01_01_TOTAL_SECONDS
                                                  <= interval.totalSeconds()) {
        d_value = (  interval.totalSeconds()
                   - DatetimeImpUtil::k_2470_01_01_TOTAL_SECONDS)
                                                    * TimeUnitRatio::k_US_PER_S
                + interval.nanoseconds() / TimeUnitRatio::k_NS_PER_US
                + DatetimeImpUtil::k_2470_01_01_VALUE;
    }
    else {
        d_value = interval.totalSeconds() * TimeUnitRatio::k_US_PER_S
                + interval.nanoseconds() / TimeUnitRatio::k_NS_PER_US
                + DatetimeImpUtil::k_0001_01_01_VALUE;
    }
}

inline
void Datetime::setUnset(int totalDays)
{
    d_value = totalDays;
}

// PRIVATE ACCESSORS
inline
void Datetime::timeIntervalFromEpoch(bsls::TimeInterval *interval) const
{
    if (   BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                             DatetimeImpUtil::k_1970_01_01_VALUE <= d_value)
        && BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                             DatetimeImpUtil::k_2470_01_01_VALUE >  d_value)) {
        bsls::Types::Uint64 totalNanoseconds =
                                 d_value - DatetimeImpUtil::k_1970_01_01_VALUE;
        interval->setInterval(totalNanoseconds / TimeUnitRatio::k_NS_PER_S
                                 + DatetimeImpUtil::k_1970_01_01_TOTAL_SECONDS,
                              static_cast<int>(  totalNanoseconds
                                               % TimeUnitRatio::k_NS_PER_S));
    }
    else if (DatetimeImpUtil::k_2470_01_01_VALUE <= d_value) {
        bsls::Types::Uint64 totalMicroseconds =
                                 d_value - DatetimeImpUtil::k_2470_01_01_VALUE;
        interval->setInterval(totalMicroseconds / TimeUnitRatio::k_US_PER_S
                                 + DatetimeImpUtil::k_2470_01_01_TOTAL_SECONDS,
                              static_cast<int>(  totalMicroseconds
                                               % TimeUnitRatio::k_US_PER_S
                                               * TimeUnitRatio::k_NS_PER_US));
    }
    else if (DatetimeImpUtil::k_0001_01_01_VALUE <= d_value) {
        bsls::Types::Uint64 totalMicroseconds =
                                 d_value - DatetimeImpUtil::k_0001_01_01_VALUE;
        interval->setInterval(totalMicroseconds / TimeUnitRatio::k_US_PER_S,
                              static_cast<int>(  totalMicroseconds
                                               % TimeUnitRatio::k_US_PER_S
                                               * TimeUnitRatio::k_NS_PER_US));
    }
    else {
        interval->setInterval(d_value * TimeUnitRatio::k_S_PER_D);
    }
}

inline
void Datetime::timeIntervalFromEpoch(bsls::TimeInterval *interval,
                                     bool               *unset) const
{
    timeIntervalFromEpoch(interval);
    *unset = d_value < DatetimeImpUtil::k_0001_01_01_VALUE;
}

// CLASS METHODS
inline
bool Datetime::isValid(int year,
                       int month,
                       int day,
                       int hour,
                       int minute,
                       int second,
                       int millisecond)
{
    // TBD
    return Date::isValidYearMonthDay(year, month, day)
        && Time::isValid(hour, minute, second, millisecond);
}

                                  // Aspects

inline
int Datetime::maxSupportedBdexVersion(int versionSelector)
{
    if (versionSelector >= 20160314) {
        return 2;                                                     // RETURN
    }
    return 1;
}

// CREATORS
inline
Datetime::Datetime()
: d_value(0)
{
}

inline
Datetime::Datetime(const Date& date)
{
    setTimeIntervalFromEpoch(bsls::TimeInterval((date - Date())
                                                    * TimeUnitRatio::k_S_PER_D,
                                                0));
}

inline
Datetime::Datetime(const Date& date, const Time& time)
{
    setDatetime(date, time);
}

inline
Datetime::Datetime(int year,
                   int month,
                   int day,
                   int hour,
                   int minute,
                   int second,
                   int millisecond)
{
    // TBD
    BSLS_ASSERT_SAFE(Datetime::isValid(year,
                                       month,
                                       day,
                                       hour,
                                       minute,
                                       second,
                                       millisecond));

    if (24 != hour) {
        bsls::Types::Uint64 totalSeconds =
                   (Date(year, month, day) - Date()) * TimeUnitRatio::k_S_PER_D
                 + hour * TimeUnitRatio::k_S_PER_H
                 + minute * TimeUnitRatio::k_S_PER_M
                 + second;

        setTimeIntervalFromEpoch(bsls::TimeInterval(
                  totalSeconds,
                  static_cast<int>(millisecond * TimeUnitRatio::k_NS_PER_MS)));
    }
    else {
        setUnset(Date(year, month, day) - Date());
    }
}

inline
Datetime::Datetime(const Datetime& original)
: d_value(original.d_value)
{
}

// MANIPULATORS
inline
Datetime& Datetime::operator=(const Datetime& rhs)
{
    d_value = rhs.d_value;

    return *this;
}

inline
Datetime& Datetime::operator+=(const DatetimeInterval& rhs)
{
    BSLS_ASSERT_SAFE(rhs <= Datetime(9999, 12, 31, 23, 59, 59, 999) - *this);
    BSLS_ASSERT_SAFE(rhs >= Datetime(   1,  1,  1,  0,  0,  0,   0) - *this);

    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);
    interval.addMilliseconds(rhs.totalMilliseconds());
    setTimeIntervalFromEpoch(interval);

    return *this;
}

inline
Datetime& Datetime::operator-=(const DatetimeInterval& rhs)
{
    BSLS_ASSERT_SAFE(-rhs <= Datetime(9999, 12, 31, 23, 59, 59, 999) - *this);
    BSLS_ASSERT_SAFE(-rhs >= Datetime(   1,  1,  1,  0,  0,  0,   0) - *this);

    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);
    interval.addMilliseconds(-rhs.totalMilliseconds());
    setTimeIntervalFromEpoch(interval);

    return *this;
}

inline
void Datetime::setDatetime(const Date& date,
                           int         hour,
                           int         minute,
                           int         second,
                           int         millisecond)
{
    if (24 != hour) {
        bsls::Types::Uint64 totalSeconds =
                                     (date - Date()) * TimeUnitRatio::k_S_PER_D
                                   + hour            * TimeUnitRatio::k_S_PER_H
                                   + minute          * TimeUnitRatio::k_S_PER_M
                                   + second;
        setTimeIntervalFromEpoch(bsls::TimeInterval(
                  totalSeconds,
                  static_cast<int>(millisecond * TimeUnitRatio::k_NS_PER_MS)));
    }
    else {
        setUnset(date - Date());
    }
}

inline
void Datetime::setDatetime(const Date& date, const Time& time)
{
    int hour;
    int minute;
    int second;
    int millisecond;

    time.getTime(&hour, &minute, &second, &millisecond);

    setDatetime(date, hour, minute, second, millisecond);
}

inline
void Datetime::setDatetime(int year,
                           int month,
                           int day,
                           int hour,
                           int minute,
                           int second,
                           int millisecond)
{
    // TBD
    BSLS_ASSERT_SAFE(Datetime::isValid(year,
                                       month,
                                       day,
                                       hour,
                                       minute,
                                       second,
                                       millisecond));

    setDatetime(Date(year, month, day), hour, minute, second, millisecond);
}

inline
int Datetime::setDatetimeIfValid(int year,
                                 int month,
                                 int day,
                                 int hour,
                                 int minute,
                                 int second,
                                 int millisecond)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (Datetime::isValid(year,
                          month,
                          day,
                          hour,
                          minute,
                          second,
                          millisecond)) {
        setDatetime(year, month, day, hour, minute, second, millisecond);

        return k_SUCCESS;                                             // RETURN
    }

    return k_FAILURE;
}

inline
void Datetime::setDate(const Date& date)
{
    bsls::TimeInterval interval;
    bool               unset;

    timeIntervalFromEpoch(&interval, &unset);

    if (!unset) {
        bsls::Types::Uint64 seconds     = interval.totalSeconds()
                                                    % TimeUnitRatio::k_S_PER_D;
        int                 nanoseconds = interval.nanoseconds();

        interval.setInterval((date - Date()) * TimeUnitRatio::k_S_PER_D
                                                                     + seconds,
                             nanoseconds);
        setTimeIntervalFromEpoch(interval);
    }
    else {
        setUnset(date - Date());
    }
}

inline
void Datetime::setYearDay(int year, int dayOfYear)
{
    BSLS_ASSERT_SAFE(Date::isValidYearDay(year, dayOfYear));

    setDate(Date(year, dayOfYear));
}

inline
void Datetime::setYearMonthDay(int year, int month, int day)
{
    BSLS_ASSERT_SAFE(Date::isValidYearMonthDay(year, month, day));

    setDate(Date(year, month, day));
}

inline
void Datetime::setTime(const Time& time)
{
    int hour;
    int minute;
    int second;
    int millisecond;

    time.getTime(&hour, &minute, &second, &millisecond);

    setTime(hour, minute, second, millisecond);
}

inline
void Datetime::setTime(int hour, int minute, int second, int millisecond)
{
    // TBD
    BSLS_ASSERT_SAFE(Time::isValid(hour, minute, second, millisecond));

    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);

    if (24 != hour) {
        bsls::Types::Uint64 totalSeconds =
                                interval.totalDays() * TimeUnitRatio::k_S_PER_D
                              + hour                 * TimeUnitRatio::k_S_PER_H
                              + minute               * TimeUnitRatio::k_S_PER_M
                              + second;
        interval.setInterval(totalSeconds,
                             millisecond * TimeUnitRatio::k_NS_PER_MS_32);
        setTimeIntervalFromEpoch(interval);
    }
    else {
        setUnset(static_cast<int>(interval.totalDays()));
    }
}

inline
void Datetime::setHour(int hour)
{
    BSLS_ASSERT_SAFE(0 <= hour);
    BSLS_ASSERT_SAFE(     hour <= 24);

    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);

    if (24 != hour) {
        bsls::Types::Uint64 totalSeconds =
                            interval.totalSeconds() / TimeUnitRatio::k_S_PER_D
                                                    * TimeUnitRatio::k_S_PER_D
                          + interval.totalSeconds() % TimeUnitRatio::k_S_PER_H
                          + hour                    * TimeUnitRatio::k_S_PER_H;
        interval.setInterval(totalSeconds, interval.nanoseconds());
        setTimeIntervalFromEpoch(interval);
    }
    else {
        setUnset(static_cast<int>(interval.totalDays()));
    }
}

inline
void Datetime::setMinute(int minute)
{
    BSLS_ASSERT_SAFE(0 <= minute);
    BSLS_ASSERT_SAFE(     minute <= 59);

    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);

    bsls::Types::Uint64 totalSeconds =
                            interval.totalSeconds() / TimeUnitRatio::k_S_PER_H
                                                    * TimeUnitRatio::k_S_PER_H
                          + interval.totalSeconds() % TimeUnitRatio::k_S_PER_M
                          + minute                  * TimeUnitRatio::k_S_PER_M;

    interval.setInterval(totalSeconds, interval.nanoseconds());
    setTimeIntervalFromEpoch(interval);
}

inline
void Datetime::setSecond(int second)
{
    BSLS_ASSERT_SAFE(0 <= second);
    BSLS_ASSERT_SAFE(     second <= 59);

    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);

    bsls::Types::Uint64 totalSeconds =
                            interval.totalSeconds() / TimeUnitRatio::k_S_PER_M
                                                    * TimeUnitRatio::k_S_PER_M
                          + second;

    interval.setInterval(totalSeconds, interval.nanoseconds());
    setTimeIntervalFromEpoch(interval);
}

inline
void Datetime::setMillisecond(int millisecond)
{
    BSLS_ASSERT_SAFE(0 <= millisecond);
    BSLS_ASSERT_SAFE(     millisecond <= 999);

    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);
    interval.setInterval(interval.totalSeconds(),
                         static_cast<int>(  millisecond
                                          * TimeUnitRatio::k_NS_PER_MS));
    setTimeIntervalFromEpoch(interval);
}

inline
void Datetime::addDays(int days)
{
    BSLS_ASSERT_SAFE(0 == Date(date()).addDaysIfValid(days));

    bsls::TimeInterval interval;
    bool               unset;

    timeIntervalFromEpoch(&interval, &unset);

    interval.addDays(days);

    if (!unset) {
        setTimeIntervalFromEpoch(interval);
    }
    else {
        setUnset(static_cast<int>(interval.totalDays()));
    }
}

inline
void Datetime::addTime(bsls::Types::Int64 hours,
                       bsls::Types::Int64 minutes,
                       bsls::Types::Int64 seconds,
                       bsls::Types::Int64 milliseconds)
{
    // TBD
    const DatetimeInterval delta(0,
                                 hours,
                                 minutes,
                                 seconds,
                                 milliseconds);

    BSLS_ASSERT_SAFE(delta <= Datetime(9999, 12, 31, 23, 59, 59, 999) - *this);
    BSLS_ASSERT_SAFE(delta >= Datetime(   1,  1,  1,  0,  0,  0,   0) - *this);

    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);
    interval.addMilliseconds(delta.totalMilliseconds());
    setTimeIntervalFromEpoch(interval);
}

inline
void Datetime::addHours(bsls::Types::Int64 hours)
{
    // TBD
    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);
    interval.addHours(hours);
    setTimeIntervalFromEpoch(interval);
}

inline
void Datetime::addMinutes(bsls::Types::Int64 minutes)
{
    // TBD
    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);
    interval.addMinutes(minutes);
    setTimeIntervalFromEpoch(interval);
}

inline
void Datetime::addSeconds(bsls::Types::Int64 seconds)
{
    // TBD
    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);
    interval.addSeconds(seconds);
    setTimeIntervalFromEpoch(interval);
}

inline
void Datetime::addMilliseconds(bsls::Types::Int64 milliseconds)
{
    // TBD
    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);
    interval.addMilliseconds(milliseconds);
    setTimeIntervalFromEpoch(interval);
}

                                  // Aspects

template <class STREAM>
STREAM& Datetime::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            Date date;
            Time time;

            date.bdexStreamIn(stream, 1);
            time.bdexStreamIn(stream, 1);

            if (stream) {
                setDatetime(date, time);
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
Date Datetime::date() const
{
    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);

    return Date() + static_cast<int>(interval.totalDays());
}

inline
Time Datetime::time() const
{
    bsls::TimeInterval interval;
    bool               unset;

    timeIntervalFromEpoch(&interval, &unset);

    if (unset) {
        return Time(24);                                              // RETURN
    }

    Time time;

    time.addMilliseconds(static_cast<int>(
                         interval.totalSeconds() % TimeUnitRatio::k_S_PER_D
                                                    * TimeUnitRatio::k_MS_PER_S
                       + interval.nanoseconds() / TimeUnitRatio::k_NS_PER_MS));

    return time;
}

inline
int Datetime::year() const
{
    return date().year();
}

inline
int Datetime::month() const
{
    return date().month();
}

inline
int Datetime::day() const
{
    return date().day();
}

inline
int Datetime::dayOfYear() const
{
    return date().dayOfYear();
}

inline
DayOfWeek::Enum Datetime::dayOfWeek() const
{
    return date().dayOfWeek();
}

inline
int Datetime::hour() const
{
    bsls::TimeInterval interval;
    bool               unset;

    timeIntervalFromEpoch(&interval, &unset);

    if (unset) {
        return 24;                                                    // RETURN
    }

    return static_cast<int>(  interval.totalSeconds()
                            / TimeUnitRatio::k_S_PER_H
                            % TimeUnitRatio::k_H_PER_D);
}

inline
int Datetime::minute() const
{
    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);

    return static_cast<int>(  interval.totalSeconds()
                            / TimeUnitRatio::k_S_PER_M
                            % TimeUnitRatio::k_M_PER_H);
}

inline
int Datetime::second() const
{
    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);

    return static_cast<int>(  interval.totalSeconds()
                            % TimeUnitRatio::k_S_PER_M);
}

inline
int Datetime::millisecond() const
{
    bsls::TimeInterval interval;

    timeIntervalFromEpoch(&interval);

    return interval.nanoseconds() / TimeUnitRatio::k_NS_PER_MS;
}

                                  // Aspects

template <class STREAM>
STREAM& Datetime::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            date().bdexStreamOut(stream, 1);
            time().bdexStreamOut(stream, 1);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation
inline
int Datetime::maxSupportedBdexVersion()
{
    return maxSupportedBdexVersion(0);
}

#endif  // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
inline
int Datetime::maxSupportedVersion()
{
    return maxSupportedBdexVersion(0);
}

inline
bsl::ostream& Datetime::streamOut(bsl::ostream& stream) const
{
    return stream << *this;
}

inline
int Datetime::validateAndSetDatetime(int year,
                                     int month,
                                     int day,
                                     int hour,
                                     int minute,
                                     int second,
                                     int millisecond)
{
    return setDatetimeIfValid(year,
                              month,
                              day,
                              hour,
                              minute,
                              second,
                              millisecond);
}

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22

}  // close package namespace

// FREE OPERATORS
inline
bdlt::Datetime bdlt::operator+(const Datetime&         lhs,
                               const DatetimeInterval& rhs)
{
    Datetime result(lhs);

    return result += rhs;
}

inline
bdlt::Datetime bdlt::operator+(const DatetimeInterval& lhs,
                               const Datetime&         rhs)
{
    Datetime result(rhs);

    return result += lhs;
}

inline
bdlt::Datetime bdlt::operator-(const Datetime&         lhs,
                               const DatetimeInterval& rhs)
{
    Datetime result(lhs);

    return result -= rhs;
}

inline
bdlt::DatetimeInterval bdlt::operator-(const Datetime& lhs,
                                       const Datetime& rhs)
{
    bsls::TimeInterval lhsInterval;
    bsls::TimeInterval rhsInterval;

    lhs.timeIntervalFromEpoch(&lhsInterval);
    rhs.timeIntervalFromEpoch(&rhsInterval);

    lhsInterval -= rhsInterval;

    return bdlt::DatetimeInterval(
                 0,
                 0,
                 0,
                 lhsInterval.totalSeconds(),
                 lhsInterval.nanoseconds() / bdlt::TimeUnitRatio::k_NS_PER_MS);
}

inline
bool bdlt::operator==(const Datetime& lhs, const Datetime& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool bdlt::operator!=(const Datetime& lhs, const Datetime& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bool bdlt::operator<(const Datetime& lhs, const Datetime& rhs)
{
    BSLS_ASSERT_SAFE(bdlt::DatetimeImpUtil::k_0001_01_01_VALUE <= lhs.d_value);
    BSLS_ASSERT_SAFE(bdlt::DatetimeImpUtil::k_0001_01_01_VALUE <= rhs.d_value);

    return lhs.d_value < rhs.d_value;
}

inline
bool bdlt::operator<=(const Datetime& lhs, const Datetime& rhs)
{
    BSLS_ASSERT_SAFE(bdlt::DatetimeImpUtil::k_0001_01_01_VALUE <= lhs.d_value);
    BSLS_ASSERT_SAFE(bdlt::DatetimeImpUtil::k_0001_01_01_VALUE <= rhs.d_value);

    return lhs.d_value <= rhs.d_value;
}

inline
bool bdlt::operator>(const Datetime& lhs, const Datetime& rhs)
{
    BSLS_ASSERT_SAFE(bdlt::DatetimeImpUtil::k_0001_01_01_VALUE <= lhs.d_value);
    BSLS_ASSERT_SAFE(bdlt::DatetimeImpUtil::k_0001_01_01_VALUE <= rhs.d_value);

    return lhs.d_value > rhs.d_value;
}

inline
bool bdlt::operator>=(const Datetime& lhs, const Datetime& rhs)
{
    BSLS_ASSERT_SAFE(bdlt::DatetimeImpUtil::k_0001_01_01_VALUE <= lhs.d_value);
    BSLS_ASSERT_SAFE(bdlt::DatetimeImpUtil::k_0001_01_01_VALUE <= rhs.d_value);

    return lhs.d_value >= rhs.d_value;
}

}  // close enterprise namespace

namespace bsl {

// TRAITS
template <>
struct is_trivially_copyable<BloombergLP::bdlt::Datetime> : bsl::true_type {
    // This template specialization for 'is_trivially_copyable' indicates that
    // 'bdlt::Datetime' is a trivially copyable type.
};

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
