// bdlt_datetime.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLT_DATETIME
#define INCLUDED_BDLT_DATETIME

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic type representing both date and time.
//
//@CLASSES:
//  bdlt::Datetime: date and time value (at least microsecond resolution)
//
//@SEE_ALSO: bdlt_date, bdlt_time, bdlt_datetimetz
//
//@DESCRIPTION: This component implements a value-semantic type,
// 'bdlt::Datetime', that represents the composition of a date and a time
// value.  The combined "date+time" value of a 'bdlt::Datetime' object is
// expressed textually as "yyyy/mm/dd_hh:mm:ss.ssssss", where "yyyy/mm/dd"
// represents the "date" part of the value and "hh:mm:ss.ssssss" represents the
// "time" part.
//
// In addition to the usual value-semantic complement of methods for getting
// and setting value, the 'bdlt::Datetime' class provides methods and operators
// for making relative adjustments to value ('addDays', 'addTime', 'addHours',
// etc.).  In particular, note that adding units of time to a 'bdlt::Datetime'
// object can affect the values of both the time and date parts of the object.
// For example, invoking 'addHours(2)' on a 'bdlt::Datetime' object whose value
// is "1987/10/03_22:30:00.000000" updates the value to
// "1987/10/04_00:30:00.000000".
//
///Valid 'bdlt::Datetime' Values and Their Representations
///-------------------------------------------------------
// The "date" part of a 'bdlt::Datetime' value has a range of validity
// identical to a 'bdlt::Date' object -- i.e., valid dates (according to the
// Unix [POSIX] calendar) having years in the range '[1 .. 9999]'.  The valid
// time values are '[00:00:00.000000 .. 23:59:59.999999]'.  Furthermore, the
// unset time value (i.e., 24:00:00.000000, corresponding to the default
// constructed value for 'bdlt::Time') is available for every valid date.  Note
// that the supported range of time does *not* allow for the injection of leap
// seconds.  The value "0001/01/01_24:00:00.000000" is the default constructed
// value of 'bdlt::Datetime'.
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
///Attributes
///----------
// Conceptually, the two primary attributes of 'bdlt::Datetime' are the
// constituent date and time values.  These attributes are given the special
// designation "part" in this component (i.e., the "time" part and the "date"
// part, respectively) to distinguish them from the many other attributes (see
// below) that derive from these two main parts.
//..
//  Name  Related Type  Default          Range
//  ----  ------------  ---------------  ------------------------------------
//  date  bdlt::Date    0001/01/01       [0001/01/01      .. 9999/12/31]
//  time  bdlt::Time    24:00:00.000000  [00:00:00.000000 .. 23:59:59.999999]
//..
// A 'bdlt::Datetime' object can be used in terms of its "date" and "time"
// parts or, if appropriate to an application, the object can be viewed as a
// single, integrated type having the combined individual attributes of date
// and time.  Accessors and manipulators are provided for each of these eight
// (derived) attributes:
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
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// Values represented by objects of type 'bdlt::Datetime' are used widely in
// practice.  The values of the individual attributes resulting from a
// default-constructed 'bdlt::Datetime' object, 'dt', are
// "0001/01/01_24:00:00.000000":
//..
//  bdlt::Datetime dt;          assert( 1 == dt.date().year());
//                              assert( 1 == dt.date().month());
//                              assert( 1 == dt.date().day());
//                              assert(24 == dt.hour());
//                              assert( 0 == dt.minute());
//                              assert( 0 == dt.second());
//                              assert( 0 == dt.millisecond());
//                              assert( 0 == dt.microsecond());
//..
// We can then set 'dt' to have a specific value, say, 8:43pm on January 6,
// 2013:
//..
//  dt.setDatetime(2013, 1, 6, 20, 43);
//                              assert(2013 == dt.date().year());
//                              assert(   1 == dt.date().month());
//                              assert(   6 == dt.date().day());
//                              assert(  20 == dt.hour());
//                              assert(  43 == dt.minute());
//                              assert(   0 == dt.second());
//                              assert(   0 == dt.millisecond());
//                              assert(   0 == dt.microsecond());
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
//                              assert(   2 == dt2.hour());
//                              assert(  43 == dt2.minute());
//                              assert(   9 == dt2.second());
//                              assert(   0 == dt2.millisecond());
//                              assert(   0 == dt2.microsecond());
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
//                              assert(   2 == dt3.hour());
//                              assert(  43 == dt3.minute());
//                              assert(   9 == dt3.second());
//                              assert(   0 == dt3.millisecond());
//                              assert(   0 == dt3.microsecond());
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
//  06JAN2013_20:43:00.000000
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
//  26JUN2014_20:31:23.000000
//  26JUN2014_21:47:52.714000
//  26JUN2014_23:04:22.428000
//  27JUN2014_00:20:52.142000
//  27JUN2014_01:37:21.856000
//  27JUN2014_02:53:51.570000
//  27JUN2014_04:10:21.284000
//  27JUN2014_05:26:50.998000
//..
// Notice how our objects (since they manage both "date" and "time of day"
// parts of each point in time) seamlessly handle the transition between the
// two days.

#include <bdlscm_version.h>

#include <bdlt_date.h>
#include <bdlt_datetimeimputil.h>
#include <bdlt_datetimeinterval.h>
#include <bdlt_dayofweek.h>
#include <bdlt_time.h>
#include <bdlt_timeunitratio.h>

#include <bdlb_bitutil.h>

#include <bslh_hash.h>

#include <bslmf_integralconstant.h>
#include <bslmf_istriviallycopyable.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_log.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_preconditions.h>
#include <bsls_review.h>
#include <bsls_stackaddressutil.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_iosfwd.h>
#include <bsl_cstring.h> // memset
#include <bsl_sstream.h>

namespace BloombergLP {
namespace bdlt {

                               // ==============
                               // class Datetime
                               // ==============

class Datetime {
    // This class implements a simply-constrained value-semantic type
    // representing the composition of date and time values.  Valid date values
    // for the "date" part of a 'Datetime' object are the same as those defined
    // for 'Date' objects; similarly, valid time values for the "time" part of
    // a 'Datetime' object are similar to those defined for 'Time' objects (but
    // with additional precision).  Relational operators are disallowed on
    // 'Datetime' objects whose "time" part has the same value as that of a
    // default constructed 'Time' object.

    // PRIVATE TYPES
    enum {
        k_NUM_TIME_BITS = 37,
        k_DEFAULT_FRACTIONAL_SECOND_PRECISION = 6
    };

    // CLASS DATA
    static const bsls::Types::Uint64 k_MAX_US_FROM_EPOCH;

    static const bsls::Types::Uint64 k_REP_MASK  = 0x8000000000000000ULL;
    static const bsls::Types::Uint64 k_DATE_MASK = 0xffffffe000000000ULL;
    static const bsls::Types::Uint64 k_TIME_MASK = 0x0000001fffffffffULL;

    static bsls::AtomicInt64 s_invalidRepresentationCount;

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
    template <class HASHALG>
    friend void hashAppend(HASHALG& hashAlg, const Datetime&);

    // PRIVATE MANIPULATOR
    void setMicrosecondsFromEpoch(bsls::Types::Uint64 totalMicroseconds);
        // Assign to 'd_value' the representation of a datetime such that the
        // difference between this datetime and the epoch is the specified
        // 'totalMicroseconds'.

    // PRIVATE ACCESSORS
    bsls::Types::Uint64 microsecondsFromEpoch() const;
        // Return the difference, measured in microseconds, between this
        // datetime value, with 24:00:00.000000 converted to 0:00:00.000000,
        // and the epoch.

    bsls::Types::Uint64 updatedRepresentation() const;
        // If 'd_value' is a valid representation, return 'd_value'.
        // Otherwise, return the representation of the datetime corresponding
        // to the datetime implied by assuming the value in 'd_value' is the
        // concatenation of a 'Date' and a 'Time', and log or assert the
        // detection of an invalid date.

    bool validateAndTraceLogRepresentation() const;
        // Return 'true' if the representation is valid.  Invoke a review
        // failure notifying of an invalid use of a 'bdlt::Datetime' instance
        // and return 'false' if the representation is invalid and
        // 'BSLS_ASSERT_SAFE' is inactive.  The behavior is undefined if the
        // representation is invalid and 'BSLS_ASSERT_SAFE' is active.

  public:
    // CLASS METHODS
    static bool isValid(int year,
                        int month,
                        int day,
                        int hour = 0,
                        int minute = 0,
                        int second = 0,
                        int millisecond = 0,
                        int microsecond = 0);
        // Return 'true' if the specified 'year', 'month', and 'day' attribute
        // values, and the optionally specified 'hour', 'minute', 'second',
        // 'millisecond', and 'microsecond' attribute values, represent a valid
        // 'Datetime' value, and 'false' otherwise.  Unspecified trailing
        // optional parameters default to 0.  'year', 'month', 'day', 'hour',
        // 'minute', 'second', 'millisecond', and 'microsecond' attribute
        // values represent a valid 'Datetime' value if
        // 'true == Date::isValidYearMonthDay(year, month, day)',
        // '0 <= hour < 24', '0 <= minute < 60', '0 <= second < 60',
        // '0 <= millisecond < 1000', and '0 <= microsecond < 1000'.
        // Additionally, a valid 'year', 'month', 'day' with the time portion
        // equal to 24:00:00.000000 also represents a valid 'Datetime' value.

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
             int microsecond = 0);
        // Create a 'Datetime' object whose "date" part has the value
        // represented by the specified 'year', 'month', and 'day' attributes,
        // and whose "time" part has the value represented by the optionally
        // specified 'hour', 'minute', 'second', 'millisecond', and
        // 'microsecond' attributes.  Unspecified trailing optional parameters
        // default to 0.  The behavior is undefined unless the eight attributes
        // (collectively) represent a valid 'Datetime' value (see 'isValid').

    Datetime(const Datetime& original);
        // Create a 'Datetime' object having the value of the specified
        // 'original' object.

    //! ~Datetime() = default;
        // Destroy this 'Datetime' object.

    // MANIPULATORS
    Datetime& operator=(const Datetime& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    Datetime& operator+=(const bsls::TimeInterval& rhs);
        // Add to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  If
        // '24 == hour()' on entry, set the 'hour' attribute of this object to
        // 0 before performing the addition.  The behavior is undefined unless
        // the resulting value is valid for 'Datetime' (see 'isValid').

    Datetime& operator-=(const bsls::TimeInterval& rhs);
        // Subtract from this object the value of the specified 'rhs' object,
        // and return a reference providing modifiable access to this object.
        // If '24 == hour()' on entry, set the 'hour' attribute of this object
        // to 0 before performing the subtraction.  The behavior is undefined
        // unless the resulting value is valid for 'Datetime' (see 'isValid').

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
                     int         microsecond = 0);
        // Set the value of this object to a 'Datetime' whose "date" part has
        // the value represented by the specified 'date', and whose "time" part
        // has the value represented by the optionally specified 'hour',
        // 'minute', 'second', 'millisecond', and 'microsecond' attributes.
        // Unspecified trailing optional parameters default to 0.  The behavior
        // is undefined unless the attributes (collectively) represent a valid
        // 'Datetime' value (see 'isValid').

    void setDatetime(const Date& date, const Time& time);
        // Set the value of this object to a 'Datetime' whose "date" part has
        // the value represented by the specified 'date', and whose "time" part
        // has the value represented by the specified 'time'.

    void setDatetime(int year,
                     int month,
                     int day,
                     int hour = 0,
                     int minute = 0,
                     int second = 0,
                     int millisecond = 0,
                     int microsecond = 0);
        // Set the value of this object to a 'Datetime' whose "date" part has
        // the value represented by the specified 'year', 'month', and 'day'
        // attributes, and whose "time" part has the value represented by the
        // optionally specified 'hour', 'minute', 'second', 'millisecond', and
        // 'microsecond' attributes.  Unspecified trailing optional parameters
        // default to 0.  The behavior is undefined unless the eight attributes
        // (collectively) represent a valid 'Datetime' value (see 'isValid').

    int setDatetimeIfValid(int year,
                           int month,
                           int day,
                           int hour = 0,
                           int minute = 0,
                           int second = 0,
                           int millisecond = 0,
                           int microsecond = 0);
        // Set the "date" part of this object to have the value represented by
        // the specified 'year', 'month', and 'day' attributes, and set the
        // "time" part to have the value represented by the optionally
        // specified 'hour', 'minute', 'second', 'millisecond', and
        // 'microsecond' attributes, if the eight attribute values
        // (collectively) represent a valid 'Datetime' value (see 'isValid').
        // Unspecified trailing optional parameters default to 0.  Return 0 on
        // success, and a non-zero value (with no effect) otherwise.

    int setDatetimeIfValid(const Date& date,
                           int         hour = 0,
                           int         minute = 0,
                           int         second = 0,
                           int         millisecond = 0,
                           int         microsecond = 0);
        // Set the value of this object to a 'Datetime' whose "date" part has
        // the value represented by the specified 'date', and whose "time" part
        // has the value represented by the optionally specified 'hour',
        // 'minute', 'second', 'millisecond', and 'microsecond' attributes, if
        // the attribute values (collectively) represent a valid 'Datetime'
        // value (see 'isValid').  Unspecified trailing optional parameters
        // default to 0.  Return 0 on success, and a non-zero value (with no
        // effect) otherwise.

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

    int setYearDayIfValid(int year, int dayOfYear);
        // Set this object to have the value represented by the specified
        // 'year' and 'dayOfYear' if they comprise a valid 'Date' value (see
        // 'Date::isValidYearDay').  Return 0 on success, and a non-zero value
        // (with no effect) otherwise.

    void setYearMonthDay(int year, int month, int day);
        // Set the "date" part of this object to have the value represented by
        // the specified 'year', 'month', and 'day' attribute values.  The
        // behavior is undefined unless 'year', 'month', and 'day' represent a
        // valid 'Date' value (i.e.,
        // 'true == Date::isValidYearMonthDay(year, month, day)').  Note that
        // this method has no effect on the "time" part of this object.

    int setYearMonthDayIfValid(int year, int month, int day);
        // Set this object to have the value represented by the specified
        // 'year', 'month', and 'day' if they comprise a valid 'Date' value
        // (see 'Date::isValidYearMonthDay').  Return 0 on success, and a
        // non-zero value (with no effect) otherwise.

    void setTime(const Time& time);
        // Set the "time" part of this object to have the value of the
        // specified 'time'.  Note that this method has no effect on the "date"
        // part of this object.

    void setTime(int hour,
                 int minute = 0,
                 int second = 0,
                 int millisecond = 0,
                 int microsecond = 0);
        // Set the "time" part of this object to have the value represented by
        // the specified 'hour' attribute value and the optionally specified
        // 'minute', 'second', 'millisecond', and 'microsecond' attribute
        // values.  Unspecified trailing optional parameters default to 0.  The
        // behavior is undefined unless 'hour', 'minute', 'second',
        // 'millisecond', and 'microsecond' represent a valid "time" portion of
        // a 'Datetime' value.  Note that this method has no effect on the
        // "date" part of this object.

    int setTimeIfValid(int hour,
                       int minute = 0,
                       int second = 0,
                       int millisecond = 0,
                       int microsecond = 0);
        // Set the "time" part of this object to have the value represented by
        // the specified 'hour' attribute value and the optionally specified
        // 'minute', 'second', 'millisecond', and 'microsecond' attribute
        // values if they comprise a valid "time" portion of a 'DateTime'
        // value.  Unspecified trailing optional parameters default to 0.
        // Return 0 on success, and a non-zero value (with no effect)
        // otherwise.  Note that this method has no effect on the "date" part
        // of this object.

    void setHour(int hour);
        // Set the "hour" attribute of this object to the specified 'hour'
        // value.  If '24 == hour', set the 'minute', 'second', 'millisecond',
        // and 'microsecond' attributes to 0.  The behavior is undefined
        // unless '0 <= hour <= 24'.  Note that this method has no effect on
        // the "date" part of this object.

    int setHourIfValid(int hour);
        // Set the "hour" attribute of this object to the specified 'hour'
        // value if '0 <= hour <= 24'.  If '24 == hour', set the 'minute',
        // 'second', 'millisecond', and 'microsecond' attributes to 0.  Return
        // 0 on success, and a non-zero value (with no effect) otherwise.  Note
        // that this method has no effect on the "date" part of this object.

    void setMinute(int minute);
        // Set the "minute" attribute of this object to the specified 'minute'
        // value.  If '24 == hour()', set the 'hour' attribute to 0.  The
        // behavior is undefined unless '0 <= minute <= 59'.  Note that this
        // method has no effect on the "date" part of this object.

    int setMinuteIfValid(int minute);
        // Set the "minute" attribute of this object to the specified 'minute'
        // value if '0 <= minute <= 59'.  If '24 == hour()', set the 'hour'
        // attribute to 0.  Return 0 on success, and a non-zero value (with no
        // effect) otherwise.  Note that this method has no effect on the
        // "date" part of this object.

    void setSecond(int second);
        // Set the "second" attribute of this object to the specified 'second'
        // value.  If '24 == hour()', set the 'hour' attribute to 0.  The
        // behavior is undefined unless '0 <= second <= 59'.  Note that this
        // method has no effect on the "date" part of this object.

    int setSecondIfValid(int second);
        // Set the "second" attribute of this object to the specified 'second'
        // value if '0 <= second <= 59'.  If '24 == hour()', set the 'hour'
        // attribute to 0.  Return 0 on success, and a non-zero value (with no
        // effect) otherwise.  Note that this method has no effect on the
        // "date" part of this object.

    void setMillisecond(int millisecond);
        // Set the "millisecond" attribute of this object to the specified
        // 'millisecond' value.  If '24 == hour()', set the 'hour' attribute to
        // 0.  The behavior is undefined unless '0 <= millisecond <= 999'.
        // Note that this method has no effect on the "date" part of this
        // object.

    int setMillisecondIfValid(int millisecond);
        // Set the "millisecond" attribute of this object to the specified
        // 'millisecond' value if '0 <= millisecond <= 999'.  If
        // '24 == hour()', set the 'hour' attribute to 0.  Return 0 on success,
        // and a non-zero value (with no effect) otherwise.  Note that this
        // method has no effect on the "date" part of this object.

    void setMicrosecond(int microsecond);
        // Set the "microsecond" attribute of this object to the specified
        // 'microsecond' value.  If '24 == hour()', set the 'hour' attribute to
        // 0.  The behavior is undefined unless '0 <= microsecond <= 999'.
        // Note that this method has no effect on the "date" part of this
        // object.

    int setMicrosecondIfValid(int microsecond);
        // Set the "microsecond" attribute of this object to the specified
        // 'microsecond' value if '0 <= microsecond <= 999'.  If
        // '24 == hour()', set the 'hour' attribute to 0.  Return 0 on success,
        // and a non-zero value (with no effect) otherwise.  Note that this
        // method has no effect on the "date" part of this object.

    Datetime& addDays(int days);
        // Add the specified number of 'days' to the value of this object.
        // Return a reference providing modifiable access to this object.  The
        // behavior is undefined unless the resulting value is in the valid
        // range for a 'Datetime' object.  Note that this method has no effect
        // on the "time" part of this object.  Also note that 'days' may be
        // positive, 0, or negative.

    int addDaysIfValid(int days);
        // Add the specified number of 'days' to the value of this object, if
        // the resulting value is in the valid range for a 'Datetime' object.
        // Return 0 on success, and a non-zero value (with no effect)
        // otherwise.  Note that this method has no effect on the "time" part
        // of this object.  Also note that 'days' may be positive, 0, or
        // negative.

    Datetime& addTime(bsls::Types::Int64 hours,
                 bsls::Types::Int64 minutes = 0,
                 bsls::Types::Int64 seconds = 0,
                 bsls::Types::Int64 milliseconds = 0,
                 bsls::Types::Int64 microseconds = 0);
        // Add the specified number of 'hours', and the optionally specified
        // number of 'minutes', 'seconds', 'milliseconds', and 'microseconds'
        // to the value of this object, adjusting the "date" part of this
        // object accordingly.  Unspecified trailing optional parameters
        // default to 0.  Return a reference providing modifiable access to
        // this object.  If '24 == hour()' on entry, set the 'hour' attribute
        // to 0 before performing the addition.  The behavior is undefined
        // unless the resulting value is in the valid range for a 'Datetime'
        // object.  Note that each argument independently may be positive,
        // negative, or 0.

    int addTimeIfValid(bsls::Types::Int64 hours,
                       bsls::Types::Int64 minutes = 0,
                       bsls::Types::Int64 seconds = 0,
                       bsls::Types::Int64 milliseconds = 0,
                       bsls::Types::Int64 microseconds = 0);
        // Add the specified number of 'hours', and the optionally specified
        // number of 'minutes', 'seconds', 'milliseconds', and 'microseconds'
        // to the value of this object, adjusting the "date" part of this
        // object accordingly, if the resulting value is in the valid range for
        // a 'Datetime' object.  Unspecified trailing optional parameters
        // default to 0.  If '24 == hour()' on entry, set the 'hour' attribute
        // to 0 before performing the addition.  Return 0 on success, and a
        // non-zero value (with no effect) otherwise.  Note that each argument
        // independently may be positive, negative, or 0.

    Datetime& addHours(bsls::Types::Int64 hours);
        // Add the specified number of 'hours' to the value of this object,
        // adjusting the "date" part of the object accordingly.  Return a
        // reference providing modifiable access to this object.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  The behavior is undefined unless the
        // resulting value is in the valid range for a 'Datetime' object.  Note
        // that 'hours' may be positive, negative, or 0.

    int addHoursIfValid(bsls::Types::Int64 hours);
        // Add the specified number of 'hours' to the value of this object,
        // adjusting the "date" part of the object accordingly, if the
        // resulting value is in the valid range for a 'Datetime' object.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  Return 0 on success, and a non-zero value
        // (with no effect) otherwise.  Note that 'hours' may be positive,
        // negative, or 0.

    Datetime& addMinutes(bsls::Types::Int64 minutes);
        // Add the specified number of 'minutes' to the value of this object,
        // adjusting the "date" part of the object accordingly.  Return a
        // reference providing modifiable access to this object.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  The behavior is undefined unless the
        // resulting value is in the valid range for a 'Datetime' object.  Note
        // that 'minutes' may be positive, negative, or 0.

    int addMinutesIfValid(bsls::Types::Int64 minutes);
        // Add the specified number of 'minutes' to the value of this object,
        // adjusting the "date" part of the object accordingly, if the
        // resulting value is in the valid range for a 'Datetime' object.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  Return 0 on success, and a non-zero value
        // (with no effect) otherwise.  Note that 'minutes' may be positive,
        // negative, or 0.

    Datetime& addSeconds(bsls::Types::Int64 seconds);
        // Add the specified number of 'seconds' to the value of this object,
        // adjusting the "date" part of the object accordingly.  Return a
        // reference providing modifiable access to this object.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  The behavior is undefined unless the
        // resulting value is in the valid range for a 'Datetime' object.  Note
        // that 'seconds' may be positive, negative, or 0.

    int addSecondsIfValid(bsls::Types::Int64 seconds);
        // Add the specified number of 'seconds' to the value of this object,
        // adjusting the "date" part of the object accordingly, if the
        // resulting value is in the valid range for a 'Datetime' object.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  Return 0 on success, and a non-zero value
        // (with no effect) otherwise.  Note that 'seconds' may be positive,
        // negative, or 0.

    Datetime& addMilliseconds(bsls::Types::Int64 milliseconds);
        // Add the specified number of 'milliseconds' to the value of this
        // object, adjusting the "date" part of the object accordingly.  Return
        // a reference providing modifiable access to this object.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  The behavior is undefined unless the
        // resulting value is in the valid range for a 'Datetime' object.  Note
        // that 'milliseconds' may be positive, negative, or 0.

    int addMillisecondsIfValid(bsls::Types::Int64 milliseconds);
        // Add the specified number of 'milliseconds' to the value of this
        // object, adjusting the "date" part of the object accordingly, if the
        // resulting value is in the valid range for a 'Datetime' object.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  Return 0 on success, and a non-zero value
        // (with no effect) otherwise.  Note that 'milliseconds' may be
        // positive, negative, or 0.

    Datetime& addMicroseconds(bsls::Types::Int64 microseconds);
        // Add the specified number of 'microseconds' to the value of this
        // object, adjusting the "date" part of the object accordingly.  Return
        // a reference providing modifiable access to this object.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  The behavior is undefined unless the
        // resulting value is in the valid range for a 'Datetime' object.  Note
        // that 'microseconds' may be positive, negative, or 0.

    int addMicrosecondsIfValid(bsls::Types::Int64 microseconds);
        // Add the specified number of 'microseconds' to the value of this
        // object, adjusting the "date" part of the object accordingly, if the
        // resulting value is in the valid range for a 'Datetime' object.  If
        // '24 == hour()' on entry, set the 'hour' attribute to 0 before
        // performing the addition.  Return 0 on success, and a non-zero value
        // (with no effect) otherwise.  Note that 'microseconds' may be
        // positive, negative, or 0.

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

    int day() const;
        // Return the value of the 'day' (of the month) attribute of this
        // object.

    DayOfWeek::Enum dayOfWeek() const;
        // Return the value of the 'dayOfWeek' attribute associated with the
        // 'day' (of the month) attribute of this object.

    int dayOfYear() const;
        // Return the value of the 'dayOfYear' attribute of this object.

    void getTime(int *hour,
                 int *minute = 0,
                 int *second = 0,
                 int *millisecond = 0,
                 int *microsecond = 0) const;
        // Load, into the specified 'hour', and the optionally specified
        // 'minute', 'second', 'millisecond', and 'microsecond' the respective
        // 'hour', 'minute', 'second', 'millisecond', and 'microsecond'
        // attribute values from this time object.  Unspecified arguments
        // default to 0.  Supplying 0 for an address argument suppresses the
        // loading of the value for the corresponding attribute, but has no
        // effect on the loading of other attribute values.

    int hour() const;
        // Return the value of the 'hour' attribute of this object.

    int microsecond() const;
        // Return the value of the 'microsecond' attribute of this object.

    int millisecond() const;
        // Return the value of the 'millisecond' attribute of this object.

    int minute() const;
        // Return the value of the 'minute' attribute of this object.

    int month() const;
        // Return the value of the 'month' attribute of this object.

    int second() const;
        // Return the value of the 'second' attribute of this object.

    Time time() const;
        // Return the value of the "time" part of this object.

    int year() const;
        // Return the value of the 'year' attribute of this object.

    int printToBuffer(char *result,
                      int   numBytes,
                      int   fractionalSecondPrecision = 6) const;
        // Efficiently write to the specified 'result' buffer no more than the
        // specified 'numBytes' of a representation of the value of this
        // object.  Optionally specify 'fractionalSecondPrecision' digits to
        // indicate how many fractional second digits to output.  If
        // 'fractionalSecondPrecision' is not specified then 6 fractional
        // second digits will be output (3 digits for milliseconds and 3 digits
        // for microseconds).  Return the number of characters (not including
        // the null character) that would have been written if the limit due to
        // 'numBytes' were not imposed.  'result' is null-terminated unless
        // 'numBytes' is 0.  The behavior is undefined unless '0 <= numBytes',
        // '0 <= fractionalSecondPrecision <= 6', and 'result' refers to at
        // least 'numBytes' contiguous bytes.  Note that the return value is
        // greater than or equal to 'numBytes' if the output representation was
        // truncated to avoid 'result' overrun.

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
Datetime operator+(const Datetime& lhs, const bsls::TimeInterval& rhs);
    // Return a 'Datetime' object having a value that is the sum of the
    // specified 'lhs' ('Datetime') and the specified 'rhs'
    // ('bsls::TimeInterval').  If '24 == lhs.hour()', the result is the same
    // as if the 'hour' attribute of 'lhs' is 0.  The behavior is undefined
    // unless the resulting value is in the valid range for a 'Datetime'
    // object.

Datetime operator+(const bsls::TimeInterval& lhs, const Datetime& rhs);
    // Return a 'Datetime' object having a value that is the sum of the
    // specified 'lhs' ('bsls::TimeInterval') and the specified 'rhs'
    // ('Datetime').  If '24 == rhs.hour()', the result is the same as if the
    // 'hour' attribute of 'rhs' is 0.  The behavior is undefined unless the
    // resulting value is in the valid range for a 'Datetime' object.

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

Datetime operator-(const Datetime& lhs, const bsls::TimeInterval& rhs);
    // Return a 'Datetime' object having a value that is the difference between
    // the specified 'lhs' ('Datetime') and the specified 'rhs'
    // ('bsls::TimeInterval').  If '24 == lhs.hour()', the result is the same
    // as if the 'hour' attribute of 'lhs' is 0.  The behavior is undefined
    // unless the resulting value is in the valid range for a 'Datetime'
    // object.

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
    // 'a.date() < b.date()', or if 'a.date() == b.date()' and the time portion
    // of 'a' is less than the time portion of 'b'.  The behavior is undefined
    // unless '24 != lhs.hour() && 24 != rhs.hour()'.

bool operator<=(const Datetime& lhs, const Datetime& rhs);
    // Return 'true' if the value of the specified 'lhs' object is less than or
    // equal to the value of the specified 'rhs' object, and 'false' otherwise.
    // The behavior is undefined unless '24 != lhs.hour() && 24 != rhs.hour()'.

bool operator>(const Datetime& lhs, const Datetime& rhs);
    // Return 'true' if the value of the specified 'lhs' object is greater than
    // the value of the specified 'rhs' object, and 'false' otherwise.  A
    // 'Datetime' object 'a' is greater than a 'Datetime' object 'b' if
    // 'a.date() > b.date()', or if 'a.date() == b.date()' and the time portion
    // of 'a' is greater than the time portion of 'b'.  The behavior is
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

// FREE FUNCTIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const Datetime& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'Datetime'.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                               // --------------
                               // class Datetime
                               // --------------

// PRIVATE MANIPULATOR
inline
void Datetime::setMicrosecondsFromEpoch(bsls::Types::Uint64 totalMicroseconds)
{
    d_value = ((totalMicroseconds / TimeUnitRatio::k_US_PER_D)
                                                            << k_NUM_TIME_BITS)
            + totalMicroseconds % TimeUnitRatio::k_US_PER_D;
    d_value |= k_REP_MASK;
}

// PRIVATE ACCESSORS
inline
bsls::Types::Uint64 Datetime::microsecondsFromEpoch() const
{
    if (validateAndTraceLogRepresentation()) {
        int h = hour();

        bsls::Types::Uint64 value = d_value & (~k_REP_MASK);

        if (TimeUnitRatio::k_H_PER_D_32 != h) {
            return (value >> k_NUM_TIME_BITS) * TimeUnitRatio::k_US_PER_D
                 + (value & k_TIME_MASK);                             // RETURN
        }

        return (value >> k_NUM_TIME_BITS)
                                        * TimeUnitRatio::k_US_PER_D;  // RETURN
    }

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bsls::Types::Uint64 days = (d_value & 0xffffffff) - 1;
    bsls::Types::Uint64 milliseconds =
                                (d_value >> 32) % TimeUnitRatio::k_MS_PER_D_32;
#else
    bsls::Types::Uint64 days = (d_value >> 32) - 1;
    bsls::Types::Uint64 milliseconds =
                         (d_value & 0xffffffff) % TimeUnitRatio::k_MS_PER_D_32;
#endif

    return TimeUnitRatio::k_US_PER_D  * days
         + TimeUnitRatio::k_US_PER_MS * milliseconds;
}

inline
bsls::Types::Uint64 Datetime::updatedRepresentation() const
{
    if (validateAndTraceLogRepresentation()) {
        return d_value;                                               // RETURN
    }

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    bsls::Types::Uint64 days = (d_value & 0xffffffff) - 1;
    bsls::Types::Uint64 milliseconds = d_value >> 32;
#else
    bsls::Types::Uint64 days = (d_value >> 32) - 1;
    bsls::Types::Uint64 milliseconds = d_value & 0xffffffff;
#endif

    return (days << k_NUM_TIME_BITS)
         | (TimeUnitRatio::k_US_PER_MS * milliseconds)
         | k_REP_MASK;
}

inline
bool Datetime::validateAndTraceLogRepresentation() const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(k_REP_MASK <= d_value)) {
        return true;                                                  // RETURN
    }
    BSLS_ASSERT_SAFE(
                 0 && "detected invalid 'bdlt::Datetime'; see TEAM 579660115");
    BSLS_REVIEW_INVOKE(
                      "detected invalid 'bdlt::Datetime'; see TEAM 579660115");

    return false;
}

// CLASS METHODS
inline
bool Datetime::isValid(int year,
                       int month,
                       int day,
                       int hour,
                       int minute,
                       int second,
                       int millisecond,
                       int microsecond)
{
    return Date::isValidYearMonthDay(year, month, day)
        && (   (0 <= hour      && hour   < bdlt::TimeUnitRatio::k_H_PER_D_32 &&
                0 <= minute    && minute < bdlt::TimeUnitRatio::k_M_PER_H_32 &&
                0 <= second    && second < bdlt::TimeUnitRatio::k_S_PER_M_32 &&
                0 <= millisecond && millisecond
                                       < bdlt::TimeUnitRatio::k_MS_PER_S_32  &&
                0 <= microsecond && microsecond
                                       < bdlt::TimeUnitRatio::k_US_PER_MS_32)
            || (bdlt::TimeUnitRatio::k_H_PER_D_32 == hour &&
                0                                 == minute &&
                0                                 == second &&
                0                                 == millisecond &&
                0                                 == microsecond));
}

                                  // Aspects

inline
int Datetime::maxSupportedBdexVersion(int versionSelector)
{
    if (versionSelector >= 20160411) {
        return 2;                                                     // RETURN
    }
    return 1;
}

// CREATORS
inline
Datetime::Datetime()
: d_value(TimeUnitRatio::k_US_PER_D)
{
    d_value |= k_REP_MASK;
}

inline
Datetime::Datetime(const Date& date)
: d_value(static_cast<bsls::Types::Uint64>(date - Date()) << k_NUM_TIME_BITS)
{
    d_value |= k_REP_MASK;
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
                   int millisecond,
                   int microsecond)
{
    setDatetime(year,
                month,
                day,
                hour,
                minute,
                second,
                millisecond,
                microsecond);
}

inline
Datetime::Datetime(const Datetime& original)
: d_value(original.d_value)
{
    d_value = updatedRepresentation();
}

// MANIPULATORS
inline
Datetime& Datetime::operator=(const Datetime& rhs)
{
    d_value = rhs.d_value;
    d_value = updatedRepresentation();

    return *this;
}

inline
Datetime& Datetime::operator+=(const bsls::TimeInterval& rhs)
{
    BSLS_ASSERT_SAFE( rhs.totalMicroseconds()
                     <= static_cast<bsls::Types::Int64>(
                               k_MAX_US_FROM_EPOCH - microsecondsFromEpoch()));

    BSLS_ASSERT_SAFE(-rhs.totalMicroseconds()
                     <= static_cast<bsls::Types::Int64>(
                                                     microsecondsFromEpoch()));

    bsls::Types::Uint64 totalMicroseconds =
                             microsecondsFromEpoch() + rhs.totalMicroseconds();
    setMicrosecondsFromEpoch(totalMicroseconds);

    return *this;
}

inline
Datetime& Datetime::operator-=(const bsls::TimeInterval& rhs)
{
    BSLS_ASSERT_SAFE(-rhs.totalMicroseconds()
                     <= static_cast<bsls::Types::Int64>(
                               k_MAX_US_FROM_EPOCH - microsecondsFromEpoch()));

    BSLS_ASSERT_SAFE( rhs.totalMicroseconds()
                     <= static_cast<bsls::Types::Int64>(
                                                     microsecondsFromEpoch()));

    bsls::Types::Uint64 totalMicroseconds =
                             microsecondsFromEpoch() - rhs.totalMicroseconds();
    setMicrosecondsFromEpoch(totalMicroseconds);

    return *this;
}

inline
Datetime& Datetime::operator+=(const DatetimeInterval& rhs)
{
    BSLS_ASSERT_SAFE( rhs.totalMicroseconds()
                     <= static_cast<bsls::Types::Int64>(
                               k_MAX_US_FROM_EPOCH - microsecondsFromEpoch()));

    BSLS_ASSERT_SAFE(-rhs.totalMicroseconds()
                     <= static_cast<bsls::Types::Int64>(
                                                     microsecondsFromEpoch()));

    bsls::Types::Uint64 totalMicroseconds =
                             microsecondsFromEpoch() + rhs.totalMicroseconds();
    setMicrosecondsFromEpoch(totalMicroseconds);

    return *this;
}

inline
Datetime& Datetime::operator-=(const DatetimeInterval& rhs)
{
    BSLS_ASSERT_SAFE(-rhs.totalMicroseconds()
                     <= static_cast<bsls::Types::Int64>(
                               k_MAX_US_FROM_EPOCH - microsecondsFromEpoch()));

    BSLS_ASSERT_SAFE( rhs.totalMicroseconds()
                     <= static_cast<bsls::Types::Int64>(
                                                     microsecondsFromEpoch()));

    bsls::Types::Uint64 totalMicroseconds =
                             microsecondsFromEpoch() - rhs.totalMicroseconds();
    setMicrosecondsFromEpoch(totalMicroseconds);

    return *this;
}

inline
void Datetime::setDatetime(const Date& date,
                           int         hour,
                           int         minute,
                           int         second,
                           int         millisecond,
                           int         microsecond)
{
    BSLS_ASSERT_SAFE(Datetime::isValid(date.year(),
                                       date.month(),
                                       date.day(),
                                       hour,
                                       minute,
                                       second,
                                       millisecond,
                                       microsecond));

    d_value = (static_cast<bsls::Types::Uint64>(date - Date())
                                                            << k_NUM_TIME_BITS)
            + TimeUnitRatio::k_US_PER_H  * hour
            + TimeUnitRatio::k_US_PER_M  * minute
            + TimeUnitRatio::k_US_PER_S  * second
            + TimeUnitRatio::k_US_PER_MS * millisecond
            + microsecond;

    d_value |= k_REP_MASK;
}

inline
void Datetime::setDatetime(const Date& date, const Time& time)
{
    if (24 != time.hour()) {
        d_value = (static_cast<bsls::Types::Uint64>(date - Date())
                                                            << k_NUM_TIME_BITS)
                + (time - Time(0)).totalMicroseconds();
    }
    else {
        d_value = (static_cast<bsls::Types::Uint64>(date - Date())
                                                            << k_NUM_TIME_BITS)
                + TimeUnitRatio::k_US_PER_D;
    }

    d_value |= k_REP_MASK;
}

inline
void Datetime::setDatetime(int year,
                           int month,
                           int day,
                           int hour,
                           int minute,
                           int second,
                           int millisecond,
                           int microsecond)
{
    BSLS_ASSERT_SAFE(Datetime::isValid(year,
                                       month,
                                       day,
                                       hour,
                                       minute,
                                       second,
                                       millisecond,
                                       microsecond));

    setDatetime(Date(year, month, day),
                hour,
                minute,
                second,
                millisecond,
                microsecond);
}

inline
int Datetime::setDatetimeIfValid(int year,
                                 int month,
                                 int day,
                                 int hour,
                                 int minute,
                                 int second,
                                 int millisecond,
                                 int microsecond)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (Datetime::isValid(year,
                          month,
                          day,
                          hour,
                          minute,
                          second,
                          millisecond,
                          microsecond)) {
        setDatetime(Date(year, month, day),
                    hour,
                    minute,
                    second,
                    millisecond,
                    microsecond);

        return k_SUCCESS;                                             // RETURN
    }

    return k_FAILURE;
}

inline
int Datetime::setDatetimeIfValid(const Date& date,
                                 int         hour,
                                 int         minute,
                                 int         second,
                                 int         millisecond,
                                 int         microsecond)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (Datetime::isValid(date.year(),
                          date.month(),
                          date.day(),
                          hour,
                          minute,
                          second,
                          millisecond,
                          microsecond)) {
        setDatetime(date,
                    hour,
                    minute,
                    second,
                    millisecond,
                    microsecond);

        return k_SUCCESS;                                             // RETURN
    }

    return k_FAILURE;
}

inline
void Datetime::setDate(const Date& date)
{
    d_value = updatedRepresentation();

    d_value = (static_cast<bsls::Types::Uint64>(date - Date())
                                                            << k_NUM_TIME_BITS)
            | (d_value & k_TIME_MASK);

    d_value |= k_REP_MASK;
}

inline
void Datetime::setYearDay(int year, int dayOfYear)
{
    BSLS_ASSERT_SAFE(Date::isValidYearDay(year, dayOfYear));

    setDate(Date(year, dayOfYear));
}

inline
int Datetime::setYearDayIfValid(int year, int dayOfYear)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (Date::isValidYearDay(year, dayOfYear)) {
        setDate(Date(year, dayOfYear));
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
void Datetime::setYearMonthDay(int year, int month, int day)
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT_SAFE(Date::isValidYearMonthDay(year, month, day));
    BSLS_PRECONDITIONS_END();

    setDate(Date(year, month, day));
}

inline
int Datetime::setYearMonthDayIfValid(int year, int month, int day)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (Date::isValidYearMonthDay(year, month, day)) {
        setDate(Date(year, month, day));
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
void Datetime::setTime(const Time& time)
{
    d_value = updatedRepresentation();

    if (24 != time.hour()) {
        d_value = (d_value & k_DATE_MASK)
                | (time - Time(0)).totalMicroseconds();
    }
    else {
        d_value = (d_value & k_DATE_MASK) | TimeUnitRatio::k_US_PER_D;
    }
}

inline
void Datetime::setTime(int hour,
                       int minute,
                       int second,
                       int millisecond,
                       int microsecond)
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT_SAFE(
               (0 <= hour      && hour   < bdlt::TimeUnitRatio::k_H_PER_D_32 &&
                0 <= minute    && minute < bdlt::TimeUnitRatio::k_M_PER_H_32 &&
                0 <= second    && second < bdlt::TimeUnitRatio::k_S_PER_M_32 &&
                0 <= millisecond && millisecond
                                       < bdlt::TimeUnitRatio::k_MS_PER_S_32  &&
                0 <= microsecond && microsecond
                                       < bdlt::TimeUnitRatio::k_US_PER_MS_32)
            || (bdlt::TimeUnitRatio::k_H_PER_D_32 == hour &&
                0                                 == minute &&
                0                                 == second &&
                0                                 == millisecond &&
                0                                 == microsecond));
    BSLS_PRECONDITIONS_END();

    d_value = updatedRepresentation();

    d_value = TimeUnitRatio::k_US_PER_H  * hour
            + TimeUnitRatio::k_US_PER_M  * minute
            + TimeUnitRatio::k_US_PER_S  * second
            + TimeUnitRatio::k_US_PER_MS * millisecond
            + microsecond
            + (d_value & k_DATE_MASK);
}

inline
int Datetime::setTimeIfValid(int hour,
                             int minute,
                             int second,
                             int millisecond,
                             int microsecond)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (       (0 <= hour      && hour   < bdlt::TimeUnitRatio::k_H_PER_D_32 &&
                0 <= minute    && minute < bdlt::TimeUnitRatio::k_M_PER_H_32 &&
                0 <= second    && second < bdlt::TimeUnitRatio::k_S_PER_M_32 &&
                0 <= millisecond && millisecond
                                       < bdlt::TimeUnitRatio::k_MS_PER_S_32  &&
                0 <= microsecond && microsecond
                                       < bdlt::TimeUnitRatio::k_US_PER_MS_32)
            || (bdlt::TimeUnitRatio::k_H_PER_D_32 == hour &&
                0                                 == minute &&
                0                                 == second &&
                0                                 == millisecond &&
                0                                 == microsecond)) {
        setTime(hour, minute, second, millisecond, microsecond);
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
void Datetime::setHour(int hour)
{
    BSLS_ASSERT_SAFE(0 <= hour);
    BSLS_ASSERT_SAFE(     hour <= 24);

    d_value = updatedRepresentation();

    if (TimeUnitRatio::k_H_PER_D_32 != hour) {
        bsls::Types::Uint64 microseconds = d_value & k_TIME_MASK;
        microseconds = microseconds % TimeUnitRatio::k_US_PER_H
                     + hour         * TimeUnitRatio::k_US_PER_H;
        d_value = microseconds | (d_value & k_DATE_MASK);
    }
    else {
        d_value = TimeUnitRatio::k_US_PER_D | (d_value & k_DATE_MASK);
    }
}

inline
int Datetime::setHourIfValid(int hour)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (0 <= hour && hour <= 24) {
        setHour(hour);
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
void Datetime::setMinute(int minute)
{
    BSLS_ASSERT_SAFE(0 <= minute);
    BSLS_ASSERT_SAFE(     minute <= 59);

    d_value = updatedRepresentation();

    if (TimeUnitRatio::k_H_PER_D_32 != hour()) {
        bsls::Types::Uint64 microseconds = d_value & k_TIME_MASK;
        microseconds = microseconds / TimeUnitRatio::k_US_PER_H
                                                    * TimeUnitRatio::k_US_PER_H
                     + microseconds % TimeUnitRatio::k_US_PER_M
                     + minute       * TimeUnitRatio::k_US_PER_M;
        d_value = microseconds | (d_value & k_DATE_MASK);
    }
    else {
        d_value = TimeUnitRatio::k_US_PER_M * minute
                | (d_value & k_DATE_MASK);
    }
}

inline
int Datetime::setMinuteIfValid(int minute)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (0 <= minute && minute <= 59) {
        setMinute(minute);
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
void Datetime::setSecond(int second)
{
    BSLS_ASSERT_SAFE(0 <= second);
    BSLS_ASSERT_SAFE(     second <= 59);

    d_value = updatedRepresentation();

    if (TimeUnitRatio::k_H_PER_D_32 != hour()) {
        bsls::Types::Uint64 microseconds = d_value & k_TIME_MASK;
        microseconds = microseconds / TimeUnitRatio::k_US_PER_M
                                                    * TimeUnitRatio::k_US_PER_M
                     + microseconds % TimeUnitRatio::k_US_PER_S
                     + second       * TimeUnitRatio::k_US_PER_S;
        d_value = microseconds | (d_value & k_DATE_MASK);
    }
    else {
        d_value = TimeUnitRatio::k_US_PER_S * second
                | (d_value & k_DATE_MASK);
    }
}

inline
int Datetime::setSecondIfValid(int second)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (0 <= second && second <= 59) {
        setSecond(second);
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
void Datetime::setMillisecond(int millisecond)
{
    BSLS_ASSERT_SAFE(0 <= millisecond);
    BSLS_ASSERT_SAFE(     millisecond <= 999);

    d_value = updatedRepresentation();

    if (TimeUnitRatio::k_H_PER_D_32 != hour()) {
        bsls::Types::Uint64 microseconds = d_value & k_TIME_MASK;
        microseconds = microseconds / TimeUnitRatio::k_US_PER_S
                                                    * TimeUnitRatio::k_US_PER_S
                     + microseconds % TimeUnitRatio::k_US_PER_MS
                     + millisecond  * TimeUnitRatio::k_US_PER_MS;
        d_value = microseconds | (d_value & k_DATE_MASK);
    }
    else {
        d_value = TimeUnitRatio::k_US_PER_MS * millisecond
                | (d_value & k_DATE_MASK);
    }
}

inline
int Datetime::setMillisecondIfValid(int millisecond)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (0 <= millisecond && millisecond <= 999) {
        setMillisecond(millisecond);
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
void Datetime::setMicrosecond(int microsecond)
{
    BSLS_ASSERT_SAFE(0 <= microsecond);
    BSLS_ASSERT_SAFE(     microsecond <= 999);

    d_value = updatedRepresentation();

    if (TimeUnitRatio::k_H_PER_D_32 != hour()) {
        bsls::Types::Uint64 microseconds = d_value & k_TIME_MASK;
        microseconds = microseconds / TimeUnitRatio::k_US_PER_MS
                                                   * TimeUnitRatio::k_US_PER_MS
                     + microsecond;
        d_value = microseconds | (d_value & k_DATE_MASK);
    }
    else {
        d_value = (d_value & k_DATE_MASK) | microsecond;
    }
}

inline
int Datetime::setMicrosecondIfValid(int microsecond)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (0 <= microsecond && microsecond <= 999) {
        setMicrosecond(microsecond);
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
Datetime& Datetime::addDays(int days)
{
    BSLS_ASSERT_SAFE(0 == Date(date()).addDaysIfValid(days));

    d_value = updatedRepresentation();  // needed to avoid double logging from
                                        // 'date' and then 'setDate'

    setDate(date() + days);

    return *this;
}

inline
int Datetime::addDaysIfValid(int days)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (0 == Date(date()).addDaysIfValid(days)) {
        addDays(days);
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
Datetime& Datetime::addTime(bsls::Types::Int64 hours,
                            bsls::Types::Int64 minutes,
                            bsls::Types::Int64 seconds,
                            bsls::Types::Int64 milliseconds,
                            bsls::Types::Int64 microseconds)
{
    // Reduce the input parameters to 'days' and 'microseconds', without any
    // constraints on the representation, without the possibility of overflow
    // or underflow.

    bsls::Types::Int64 days = hours        / TimeUnitRatio::k_H_PER_D
                            + minutes      / TimeUnitRatio::k_M_PER_D
                            + seconds      / TimeUnitRatio::k_S_PER_D
                            + milliseconds / TimeUnitRatio::k_MS_PER_D
                            + microseconds / TimeUnitRatio::k_US_PER_D;

    hours        %= TimeUnitRatio::k_H_PER_D;
    minutes      %= TimeUnitRatio::k_M_PER_D;
    seconds      %= TimeUnitRatio::k_S_PER_D;
    milliseconds %= TimeUnitRatio::k_MS_PER_D;
    microseconds %= TimeUnitRatio::k_US_PER_D;

    microseconds = hours             * TimeUnitRatio::k_US_PER_H
                 + minutes           * TimeUnitRatio::k_US_PER_M
                 + seconds           * TimeUnitRatio::k_US_PER_S
                 + milliseconds      * TimeUnitRatio::k_US_PER_MS
                 + microseconds;

    // Modify the representation to ensure 'days' and 'microseconds' have the
    // same sign (i.e., both are positive or both are negative or 'days == 0').

    days         += microseconds / TimeUnitRatio::k_US_PER_D;
    microseconds %= TimeUnitRatio::k_US_PER_D;

    if (days > 0 && microseconds < 0) {
        --days;
        microseconds += TimeUnitRatio::k_US_PER_D;
    }
    else if (days < 0 && microseconds > 0) {
        ++days;
        microseconds -= TimeUnitRatio::k_US_PER_D;
    }

    // Piecewise add the 'days' and 'microseconds' to this datetime.

    bsls::Types::Uint64 totalMicroseconds = microsecondsFromEpoch();

    BSLS_ASSERT_SAFE( days <= static_cast<bsls::Types::Int64>
                                     ((k_MAX_US_FROM_EPOCH - totalMicroseconds)
                                                 / TimeUnitRatio::k_US_PER_D));
    BSLS_ASSERT_SAFE(-days <= static_cast<bsls::Types::Int64>
                              (totalMicroseconds / TimeUnitRatio::k_US_PER_D));

    totalMicroseconds += days * TimeUnitRatio::k_US_PER_D;

    BSLS_ASSERT_SAFE( microseconds <= static_cast<bsls::Types::Int64>
                                    (k_MAX_US_FROM_EPOCH - totalMicroseconds));
    BSLS_ASSERT_SAFE(-microseconds <= static_cast<bsls::Types::Int64>
                                                          (totalMicroseconds));

    totalMicroseconds += microseconds;

    // Assign the value.

    setMicrosecondsFromEpoch(totalMicroseconds);

    return *this;
}

inline
int Datetime::addTimeIfValid(bsls::Types::Int64 hours,
                             bsls::Types::Int64 minutes,
                             bsls::Types::Int64 seconds,
                             bsls::Types::Int64 milliseconds,
                             bsls::Types::Int64 microseconds)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    // Reduce the input parameters to 'days' and 'microseconds', without any
    // constraints on the representation, without the possibility of overflow
    // or underflow.

    bsls::Types::Int64 days = hours        / TimeUnitRatio::k_H_PER_D
                            + minutes      / TimeUnitRatio::k_M_PER_D
                            + seconds      / TimeUnitRatio::k_S_PER_D
                            + milliseconds / TimeUnitRatio::k_MS_PER_D
                            + microseconds / TimeUnitRatio::k_US_PER_D;

    hours        %= TimeUnitRatio::k_H_PER_D;
    minutes      %= TimeUnitRatio::k_M_PER_D;
    seconds      %= TimeUnitRatio::k_S_PER_D;
    milliseconds %= TimeUnitRatio::k_MS_PER_D;
    microseconds %= TimeUnitRatio::k_US_PER_D;

    microseconds = hours             * TimeUnitRatio::k_US_PER_H
                 + minutes           * TimeUnitRatio::k_US_PER_M
                 + seconds           * TimeUnitRatio::k_US_PER_S
                 + milliseconds      * TimeUnitRatio::k_US_PER_MS
                 + microseconds;

    // Modify the representation to ensure 'days' and 'microseconds' have the
    // same sign (i.e., both are positive or both are negative or 'days == 0').

    days         += microseconds / TimeUnitRatio::k_US_PER_D;
    microseconds %= TimeUnitRatio::k_US_PER_D;

    if (days > 0 && microseconds < 0) {
        --days;
        microseconds += TimeUnitRatio::k_US_PER_D;
    }
    else if (days < 0 && microseconds > 0) {
        ++days;
        microseconds -= TimeUnitRatio::k_US_PER_D;
    }

    // Piecewise add the 'days' and 'microseconds' to this datetime.

    bsls::Types::Uint64 totalMicroseconds = microsecondsFromEpoch();

    if (!(  days <= static_cast<bsls::Types::Int64>
                                     ((k_MAX_US_FROM_EPOCH - totalMicroseconds)
                                                   / TimeUnitRatio::k_US_PER_D)
        && -days <= static_cast<bsls::Types::Int64>
                            (totalMicroseconds / TimeUnitRatio::k_US_PER_D))) {
        return k_FAILURE;                                             // RETURN
    }

    totalMicroseconds += days * TimeUnitRatio::k_US_PER_D;

    if (!(  microseconds <= static_cast<bsls::Types::Int64>
                                      (k_MAX_US_FROM_EPOCH - totalMicroseconds)
        && -microseconds <= static_cast<bsls::Types::Int64>
                                                        (totalMicroseconds))) {
        return k_FAILURE;                                             // RETURN
    }

    totalMicroseconds += microseconds;

    // Assign the value.

    setMicrosecondsFromEpoch(totalMicroseconds);
    return k_SUCCESS;
}

inline
Datetime& Datetime::addHours(bsls::Types::Int64 hours)
{
    BSLS_ASSERT_SAFE( hours <= static_cast<bsls::Types::Int64>
                               ((k_MAX_US_FROM_EPOCH - microsecondsFromEpoch())
                                                 / TimeUnitRatio::k_US_PER_H));
    BSLS_ASSERT_SAFE(-hours <= static_cast<bsls::Types::Int64>
                        (microsecondsFromEpoch() / TimeUnitRatio::k_US_PER_H));

    bsls::Types::Uint64 totalMicroseconds = microsecondsFromEpoch();

    setMicrosecondsFromEpoch(hours * TimeUnitRatio::k_US_PER_H
                                                          + totalMicroseconds);

    return *this;
}

inline
int Datetime::addHoursIfValid(bsls::Types::Int64 hours)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (    hours <= static_cast<bsls::Types::Int64>
                               ((k_MAX_US_FROM_EPOCH - microsecondsFromEpoch())
                                                   / TimeUnitRatio::k_US_PER_H)
        && -hours <= static_cast<bsls::Types::Int64>
                       (microsecondsFromEpoch() / TimeUnitRatio::k_US_PER_H)) {
        addHours(hours);
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
Datetime& Datetime::addMinutes(bsls::Types::Int64 minutes)
{
    BSLS_ASSERT_SAFE( minutes <= static_cast<bsls::Types::Int64>
                               ((k_MAX_US_FROM_EPOCH - microsecondsFromEpoch())
                                                 / TimeUnitRatio::k_US_PER_M));
    BSLS_ASSERT_SAFE(-minutes <= static_cast<bsls::Types::Int64>
                        (microsecondsFromEpoch() / TimeUnitRatio::k_US_PER_M));

    bsls::Types::Uint64 totalMicroseconds = microsecondsFromEpoch();

    setMicrosecondsFromEpoch(minutes * TimeUnitRatio::k_US_PER_M
                                                          + totalMicroseconds);
    return *this;
}

inline
int Datetime::addMinutesIfValid(bsls::Types::Int64 minutes)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (    minutes <= static_cast<bsls::Types::Int64>
                               ((k_MAX_US_FROM_EPOCH - microsecondsFromEpoch())
                                                 / TimeUnitRatio::k_US_PER_M)
        && -minutes <= static_cast<bsls::Types::Int64>
                       (microsecondsFromEpoch() / TimeUnitRatio::k_US_PER_M)) {
        addMinutes(minutes);
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
Datetime& Datetime::addSeconds(bsls::Types::Int64 seconds)
{
    BSLS_ASSERT_SAFE( seconds <= static_cast<bsls::Types::Int64>
                               ((k_MAX_US_FROM_EPOCH - microsecondsFromEpoch())
                                                 / TimeUnitRatio::k_US_PER_S));
    BSLS_ASSERT_SAFE(-seconds <= static_cast<bsls::Types::Int64>
                        (microsecondsFromEpoch() / TimeUnitRatio::k_US_PER_S));

    bsls::Types::Uint64 totalMicroseconds = microsecondsFromEpoch();

    setMicrosecondsFromEpoch(seconds * TimeUnitRatio::k_US_PER_S
                                                          + totalMicroseconds);
    return *this;
}

inline
int Datetime::addSecondsIfValid(bsls::Types::Int64 seconds)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (    seconds <= static_cast<bsls::Types::Int64>
                               ((k_MAX_US_FROM_EPOCH - microsecondsFromEpoch())
                                                   / TimeUnitRatio::k_US_PER_S)
        && -seconds <= static_cast<bsls::Types::Int64>
                       (microsecondsFromEpoch() / TimeUnitRatio::k_US_PER_S)) {
        addSeconds(seconds);
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
Datetime& Datetime::addMilliseconds(bsls::Types::Int64 milliseconds)
{
    BSLS_ASSERT_SAFE( milliseconds <= static_cast<bsls::Types::Int64>
                               ((k_MAX_US_FROM_EPOCH - microsecondsFromEpoch())
                                                / TimeUnitRatio::k_US_PER_MS));
    BSLS_ASSERT_SAFE(-milliseconds <= static_cast<bsls::Types::Int64>
                       (microsecondsFromEpoch() / TimeUnitRatio::k_US_PER_MS));

    bsls::Types::Uint64 totalMicroseconds = microsecondsFromEpoch();

    setMicrosecondsFromEpoch(milliseconds * TimeUnitRatio::k_US_PER_MS
                                                          + totalMicroseconds);
    return *this;
}

inline
int Datetime::addMillisecondsIfValid(bsls::Types::Int64 milliseconds)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (    milliseconds <= static_cast<bsls::Types::Int64>
                               ((k_MAX_US_FROM_EPOCH - microsecondsFromEpoch())
                                                  / TimeUnitRatio::k_US_PER_MS)
        && -milliseconds <= static_cast<bsls::Types::Int64>
                      (microsecondsFromEpoch() / TimeUnitRatio::k_US_PER_MS)) {
        addMilliseconds(milliseconds);
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

inline
Datetime& Datetime::addMicroseconds(bsls::Types::Int64 microseconds)
{
    BSLS_ASSERT_SAFE( microseconds <= static_cast<bsls::Types::Int64>
                              (k_MAX_US_FROM_EPOCH - microsecondsFromEpoch()));
    BSLS_ASSERT_SAFE(-microseconds <= static_cast<bsls::Types::Int64>
                                                    (microsecondsFromEpoch()));

    bsls::Types::Uint64 totalMicroseconds = microsecondsFromEpoch();

    setMicrosecondsFromEpoch(microseconds + totalMicroseconds);

    return *this;
}

inline
int Datetime::addMicrosecondsIfValid(bsls::Types::Int64 microseconds)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (    microseconds <= static_cast<bsls::Types::Int64>
                                (k_MAX_US_FROM_EPOCH - microsecondsFromEpoch())
        && -microseconds <= static_cast<bsls::Types::Int64>
                                                   (microsecondsFromEpoch())) {
        addMicroseconds(microseconds);
        return k_SUCCESS;                                             // RETURN
    }
    return k_FAILURE;
}

                                  // Aspects

template <class STREAM>
STREAM& Datetime::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 2: {
            bsls::Types::Uint64 tmp;

            stream.getUint64(tmp);

            if (   stream
                && tmp <= (DatetimeImpUtil::k_MAX_VALUE & (~k_REP_MASK))) {
                d_value = tmp;
                d_value |= k_REP_MASK;
            }
            else {
                stream.invalidate();
            }
          } break;
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
    bsls::Types::Uint64 value = updatedRepresentation() & (~k_REP_MASK);

    return Date() + static_cast<int>(value >> k_NUM_TIME_BITS);
}

inline
int Datetime::day() const
{
    return date().day();
}

inline
DayOfWeek::Enum Datetime::dayOfWeek() const
{
    return date().dayOfWeek();
}

inline
int Datetime::dayOfYear() const
{
    return date().dayOfYear();
}

inline
void Datetime::getTime(int *hour,
                       int *minute,
                       int *second,
                       int *millisecond,
                       int *microsecond) const
{
    bsls::Types::Uint64 microseconds = updatedRepresentation() & k_TIME_MASK;

    if (hour) {
        *hour = static_cast<int>(microseconds / TimeUnitRatio::k_US_PER_H);
    }
    if (minute) {
        *minute = static_cast<int>(  microseconds
                                   / TimeUnitRatio::k_US_PER_M
                                   % TimeUnitRatio::k_M_PER_H);
    }
    if (second) {
        *second = static_cast<int>(  microseconds
                                   / TimeUnitRatio::k_US_PER_S
                                   % TimeUnitRatio::k_S_PER_M);
    }
    if (millisecond) {
        *millisecond = static_cast<int>(  microseconds
                                        / TimeUnitRatio::k_US_PER_MS
                                        % TimeUnitRatio::k_MS_PER_S);
    }
    if (microsecond) {
        *microsecond = static_cast<int>(
                                    microseconds % TimeUnitRatio::k_US_PER_MS);
    }
}

inline
int Datetime::hour() const
{
    bsls::Types::Uint64 microseconds = updatedRepresentation() & k_TIME_MASK;

    return static_cast<int>(microseconds / TimeUnitRatio::k_US_PER_H);
}

inline
int Datetime::microsecond() const
{
    bsls::Types::Uint64 microseconds = updatedRepresentation() & k_TIME_MASK;

    return static_cast<int>(microseconds % TimeUnitRatio::k_US_PER_MS);
}

inline
int Datetime::millisecond() const
{
    bsls::Types::Uint64 microseconds = updatedRepresentation() & k_TIME_MASK;

    return static_cast<int>(  microseconds
                            / TimeUnitRatio::k_US_PER_MS
                            % TimeUnitRatio::k_MS_PER_S);
}

inline
int Datetime::minute() const
{
    bsls::Types::Uint64 microseconds = updatedRepresentation() & k_TIME_MASK;

    return static_cast<int>(  microseconds
                            / TimeUnitRatio::k_US_PER_M
                            % TimeUnitRatio::k_M_PER_H);
}

inline
int Datetime::month() const
{
    return date().month();
}

inline
int Datetime::second() const
{
    bsls::Types::Uint64 microseconds = updatedRepresentation() & k_TIME_MASK;

    return static_cast<int>(  microseconds
                            / TimeUnitRatio::k_US_PER_S
                            % TimeUnitRatio::k_S_PER_M);
}

inline
Time Datetime::time() const
{
    int hour;
    int minute;
    int second;
    int millisecond;
    int microsecond;

    getTime(&hour, &minute, &second, &millisecond, &microsecond);

    return Time(hour, minute, second, millisecond, microsecond);
}

inline
int Datetime::year() const
{
    return date().year();
}

                                  // Aspects

template <class STREAM>
STREAM& Datetime::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 2: {
            bsls::Types::Uint64 value =
                                       updatedRepresentation() & (~k_REP_MASK);

            stream.putUint64(value);
          } break;
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
bdlt::Datetime bdlt::operator+(const Datetime&           lhs,
                               const bsls::TimeInterval& rhs)
{
    Datetime result(lhs);

    return result += rhs;
}

inline
bdlt::Datetime bdlt::operator+(const bsls::TimeInterval& lhs,
                               const Datetime&           rhs)
{
    Datetime result(rhs);

    return result += lhs;
}

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
bdlt::Datetime bdlt::operator-(const Datetime&           lhs,
                               const bsls::TimeInterval& rhs)
{
    Datetime result(lhs);

    return result -= rhs;
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
    bsls::Types::Uint64 lhsTotalMicroseconds = lhs.microsecondsFromEpoch();
    bsls::Types::Uint64 rhsTotalMicroseconds = rhs.microsecondsFromEpoch();

    if (lhsTotalMicroseconds >= rhsTotalMicroseconds) {
        lhsTotalMicroseconds -= rhsTotalMicroseconds;

        return bdlt::DatetimeInterval(
                               0, 0, 0, 0, 0, lhsTotalMicroseconds);  // RETURN
    }

    rhsTotalMicroseconds -= lhsTotalMicroseconds;

    return bdlt::DatetimeInterval(
        0, 0, 0, 0, 0, -static_cast<bsls::Types::Int64>(rhsTotalMicroseconds));
}

inline
bool bdlt::operator==(const Datetime& lhs, const Datetime& rhs)
{
    bsls::Types::Uint64 lhsValue = lhs.updatedRepresentation();
    bsls::Types::Uint64 rhsValue = rhs.updatedRepresentation();

    return lhsValue == rhsValue;
}

inline
bool bdlt::operator!=(const Datetime& lhs, const Datetime& rhs)
{
    bsls::Types::Uint64 lhsValue = lhs.updatedRepresentation();
    bsls::Types::Uint64 rhsValue = rhs.updatedRepresentation();

    return lhsValue != rhsValue;
}

inline
bool bdlt::operator<(const Datetime& lhs, const Datetime& rhs)
{
    BSLS_ASSERT_SAFE(24 != lhs.hour());
    BSLS_ASSERT_SAFE(24 != rhs.hour());

    bsls::Types::Uint64 lhsValue = lhs.updatedRepresentation();
    bsls::Types::Uint64 rhsValue = rhs.updatedRepresentation();

    return lhsValue < rhsValue;
}

inline
bool bdlt::operator<=(const Datetime& lhs, const Datetime& rhs)
{
    BSLS_ASSERT_SAFE(24 != lhs.hour());
    BSLS_ASSERT_SAFE(24 != rhs.hour());

    bsls::Types::Uint64 lhsValue = lhs.updatedRepresentation();
    bsls::Types::Uint64 rhsValue = rhs.updatedRepresentation();

    return lhsValue <= rhsValue;
}

inline
bool bdlt::operator>(const Datetime& lhs, const Datetime& rhs)
{
    BSLS_ASSERT_SAFE(24 != lhs.hour());
    BSLS_ASSERT_SAFE(24 != rhs.hour());

    bsls::Types::Uint64 lhsValue = lhs.updatedRepresentation();
    bsls::Types::Uint64 rhsValue = rhs.updatedRepresentation();

    return lhsValue > rhsValue;
}

inline
bool bdlt::operator>=(const Datetime& lhs, const Datetime& rhs)
{
    BSLS_ASSERT_SAFE(24 != lhs.hour());
    BSLS_ASSERT_SAFE(24 != rhs.hour());

    bsls::Types::Uint64 lhsValue = lhs.updatedRepresentation();
    bsls::Types::Uint64 rhsValue = rhs.updatedRepresentation();

    return lhsValue >= rhsValue;
}

// FREE FUNCTIONS
template <class HASHALG>
inline
void bdlt::hashAppend(HASHALG& hashAlg, const Datetime& object)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlg, object.updatedRepresentation());
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
// Copyright 2017 Bloomberg Finance L.P.
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
