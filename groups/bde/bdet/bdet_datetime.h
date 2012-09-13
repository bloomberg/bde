// bdet_datetime.h                                                    -*-C++-*-
#ifndef INCLUDED_BDET_DATETIME
#define INCLUDED_BDET_DATETIME

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic type representing both date and time.
//
//@CLASSES:
//  bdet_Datetime: combined date and time value (millisecond resolution)
//
//@SEE_ALSO: bdet_date, bdet_time
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component implements a fully value-semantic type,
// 'bdet_Datetime', that represents the composition of the values of a
// 'bdet_Date' object and a 'bdet_Time' object.  A combined "date+time" value
// can be expressed textually as "yyyy/mm/dd_hh:mm:ss.sss", where "yyyy/mm/dd"
// represents the "date" part of the value and "hh:mm:ss.sss" represents the
// "time" part.
//
// The "date" part of a 'bdet_Datetime' value has a range of validity identical
// to a 'bdet_Date' object -- i.e., valid dates (according to the standard Unix
// date convention) having years in the range '[ 1 .. 9999 ]'.  The set of
// valid "time" values for a 'bdet_Datetime' object is, similarly, the same as
// the valid values for 'bdet_Time', namely '[ 00:00:00.000 .. 23:59:59.999 ]',
// plus "24:00:00.000", which represents the default-constructed value.  Note
// that "24:00:00.000" behaves the same as the value "00:00:00.000" with
// respect to the various manipulators, but results in undefined behavior for
// all relational comparison operators.
//
// Note that only non-modifiable direct access to both the "date" and "time"
// values of a 'bdet_Datetime' object are provided; modifiable direct access
// is not provided.  However, users have modifiable access to the four time
// fields directly.  (See the following section for more information on the
// seven fields of a 'bdet_Datetime' object.)
//
///Individual and Complex Constraints on Field Values
///--------------------------------------------------
// A datetime value may be viewed as having seven integer fields with the
// following envelope of constraints:
//..
//  Field Name  Maximum Valid Range  Auxiliary Conditions Limiting Validity
//  ----------  -------------------  ------------------------------------------
//  year           [ 1 .. 9999 ]     none
//  month          [ 1 .. 12 ]       none
//  day            [ 1 .. 31 ]       must exist for year and month (Unix)
//  hour           [ 0 .. 24 ]       24 valid only if 'min == sec == msec == 0'
//  minute         [ 0 .. 59 ]       must be 0 if 'hour == 24'
//  second         [ 0 .. 59 ]       must be 0 if 'hour == 24'
//  millisecond    [ 0 .. 999 ]      must be 0 if 'hour == 24'
//..
///Usage
///-----
// Values represented by objects of type 'bdet_Datetime' are used widely in
// practice.  The values of the individual fields resulting from a
// default-constructed 'bdet_Datetime' object, 'dt', are
// "0001/01/01_24:00:00.000":
//..
//  bdet_Datetime dt;           assert( 1 == dt.date().year());
//                              assert( 1 == dt.date().month());
//                              assert( 1 == dt.date().day());
//                              assert(24 == dt.time().hour());
//                              assert( 0 == dt.time().minute());
//                              assert( 0 == dt.time().second());
//                              assert( 0 == dt.time().millisecond());
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
//..
// Now suppose we add 6 hours and 9 seconds to this value.  There is more
// than one way to do it:
//..
//  bdet_Datetime dt2(dt);
//  dt2.addHours(6);
//  dt2.addSeconds(9);
//                              assert(2013 == dt2.date().year());
//                              assert(   1 == dt2.date().month());
//                              assert(   7 == dt2.date().day());
//                              assert(   2 == dt2.time().hour());
//                              assert(  43 == dt2.time().minute());
//                              assert(   9 == dt2.time().second());
//                              assert(   0 == dt2.time().millisecond());
//
//  bdet_Datetime dt3(dt);
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
//  06JAN2013_20:43:00.000
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

#ifndef INCLUDED_BDET_DATETIMEINTERVAL
#include <bdet_datetimeinterval.h>
#endif

#ifndef INCLUDED_BDET_DAYOFWEEK
#include <bdet_dayofweek.h>
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

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                        // ===================
                        // class bdet_Datetime
                        // ===================

class bdet_Datetime {
    // This class implements a fully value-semantic type representing the
    // composition of values of a 'bdet_Date' object and a 'bdet_Time' object.
    // Each object of this augmented, complex-constrained "date/time" attribute
    // class *always* represents a *valid* value, consisting of independently
    // valid "date" and "time" parts.  Valid date values for the "date" part of
    // a 'bdet_Datetime' object are the same as those described for 'bdet_Date'
    // objects; similarly, valid time values for the "time" part of a
    // 'bdet_Datetime' object are the same as those described for a 'bdet_Time'
    // object.
    //
    // Values of this class can be represented textually as
    // "yyyy/mm/dd_hh:mm:ss.sss", where "yyyy/mm/dd" represents the date part
    // of the value and "hh:mm:ss.sss" represents the time part.  The valid
    // range of dates includes the range of years from 1 to 9999, with valid
    // combinations of "yyyy/mm/dd" governed by the standard Unix date
    // convention.  Valid times range from "00:00:00.000" to "23:59:59.999",
    // with the special value "24:00:00.000" representing the value of a
    // default-constructed time object, which behaves like "00:00:00.000" when
    // acted upon by a manipulator.  The default-constructed value of a
    // 'bdet_Datetime' object as a whole is "0001/01/01_24:00:00.000".
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  A precise
    // operational definition of when two objects have the same value can be
    // found in the description of 'operator==' for the class.  This class is
    // *exception* *neutral* with no guarantee of rollback: If an exception is
    // thrown during the invocation of a method on a pre-existing object, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part
    // of an object as both source and destination) is supported in all cases.

    // PRIVATE TYPES
    enum {
        BDET_MILLISECONDS_PER_DAY    = 24 * 60 * 60 * 1000,
        BDET_MILLISECONDS_PER_HOUR   =      60 * 60 * 1000,
        BDET_MILLISECONDS_PER_MINUTE =           60 * 1000,
        BDET_MILLISECONDS_PER_SECOND =                1000,

        BDET_SECONDS_PER_DAY         = 24 * 60 * 60,
        BDET_SECONDS_PER_HOUR        =      60 * 60,
        BDET_SECONDS_PER_MINUTE      =           60,

        BDET_MINUTES_PER_DAY         = 24 * 60,
        BDET_MINUTES_PER_HOUR        =      60,

        BDET_HOURS_PER_DAY           = 24
    };

    typedef bslalg_PassthroughTrait<bdet_Date,
                                    bslalg_TypeTraitBitwiseCopyable> DateTrait;
    typedef bslalg_PassthroughTrait<bdet_Time,
                                    bslalg_TypeTraitBitwiseCopyable> TimeTrait;

    typedef bslmf_If<bslmf_IsSame<DateTrait, TimeTrait>::VALUE,
                    DateTrait,
                    bslalg_TypeTraits_NotTrait<bslalg_TypeTraitBitwiseCopyable>
                   >::Type                                       DateTimeTrait;

    // DATA
    bdet_Date d_date;  // "date" part of "date+time" value
    bdet_Time d_time;  // "time" part of "date+time" value

    // FRIENDS
    friend bdet_DatetimeInterval operator-(const bdet_Datetime&,
                                           const bdet_Datetime&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdet_Datetime, DateTimeTrait);

    // CLASS METHODS
    static bool isValid(int year,
                        int month,
                        int day,
                        int hour        = 0,
                        int minute      = 0,
                        int second      = 0,
                        int millisecond = 0);
        // Return 'true' if the specified 'year', 'month', and 'day', and the
        // optionally specified 'hour', 'minute', 'second', and 'millisecond',
        // represent a valid 'bdet_Datetime' value, and 'false' otherwise.  A
        // valid 'bdet_Datetime' value has valid "date" and "time" values,
        // where valid "date" values are the same as those described for
        // 'bdet_Date' objects; similarly, valid "time" values are the same as
        // those described for 'bdet_Time' objects.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    bdet_Datetime();
        // Create a 'bdet_Datetime' object having the "date" and "time" parts
        // of its value set to their respective default-constructed values:
        // "0001/01/01_24:00:00.000" (as defined in the 'bdet_date' and
        // 'bdet_time' components).

    bdet_Datetime(const bdet_Date& date);
        // Create a 'bdet_Datetime' object having the "date" part of its value
        // set to the specified 'date' and the "time" part of its value set to
        // "00:00:00.000".

    bdet_Datetime(const bdet_Date& date, const bdet_Time& time);
        // Create a 'bdet_Datetime' object having the "date" part of its value
        // set to the specified 'date' and the "time" part of its value set to
        // the specified 'time'.

    bdet_Datetime(int year,
                  int month,
                  int day,
                  int hour        = 0,
                  int minute      = 0,
                  int second      = 0,
                  int millisecond = 0);
        // Create a 'bdet_Datetime' object having the "date" part of the value
        // represented by the specified 'year', 'month', and 'day'.  Optionally
        // specify an 'hour', 'minute', 'second', and 'millisecond' to
        // represent the "time" part of the value; trailing fields that are not
        // specified will be set to 0.  The behavior is undefined unless the
        // combined representation comprises a valid 'bdet_Datetime' value
        // (see 'isValid').

    bdet_Datetime(const bdet_Datetime& original);
        // Create a 'bdet_Datetime' object having the value of the specified
        // 'original' datetime.

//! ~bdet_Datetime();
        // Destroy this 'bdet_Datetime' object.  Note that this method's
        // definition is compiler generated.

    // MANIPULATORS
    bdet_Datetime& operator=(const bdet_Datetime& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference to this modifiable object.

    bdet_Datetime& operator+=(const bdet_DatetimeInterval& rhs);
        // Add to this object the value of the specified 'rhs' object, and
        // return a reference to this modifiable object.  The behavior is
        // undefined unless the result can be represented as a valid
        // 'bdet_Datetime' (see 'isValid').

    bdet_Datetime& operator-=(const bdet_DatetimeInterval& rhs);
        // Subtract from this object the value of the specified 'rhs' object,
        // and return a reference to this modifiable object.  The behavior is
        // undefined unless the result can be represented as a valid
        // 'bdet_Datetime' (see 'isValid').

    void setDatetime(int year,
                     int month,
                     int day,
                     int hour        = 0,
                     int minute      = 0,
                     int second      = 0,
                     int millisecond = 0);
        // Set the "date" part of this object's value to the specified 'year',
        // 'month', and 'day', and the "time" part to the optionally specified
        // 'hour', 'minute', 'second', and 'millisecond', with trailing fields
        // that are not specified set to 0.  The behavior is undefined unless
        // the combined representation comprises a valid 'bdet_Datetime' value
        // (see 'isValid').

    int setDatetimeIfValid(int year,
                           int month,
                           int day,
                           int hour        = 0,
                           int minute      = 0,
                           int second      = 0,
                           int millisecond = 0);
        // Set the "date" part of this object's value to the specified 'year',
        // 'month', and 'day', and the "time" part to the optionally specified
        // 'hour', 'minute', 'second', and 'millisecond', if they would
        // comprise a valid 'bdet_Datetime' value, with trailing fields that
        // are not specified set to 0.  Return 0 on success, and a non-zero
        // value (with no effect) otherwise.

    void setDate(const bdet_Date& date);
        // Set the "date" part of this object's value to the specified 'date'.
        // Note that this method does not affect the "time" part of this
        // object's value.

    void setYearMonthDay(int year, int month, int day);
        // Set the "date" part of this object's value to the specified 'year',
        // 'month', and 'day'.  The behavior is undefined unless 'year',
        // 'month', and 'day' comprise a valid 'bdet_Date' value.  Note that
        // this method does not affect the "time" part of this object's
        // value.

    void setYearDay(int year, int dayOfYear);
        // Set the "date" part of this object's value to the specified 'year'
        // and 'dayOfYear'.  The behavior is undefined unless 'year' and
        // 'dayOfYear' represent a valid 'bdet_Date' value.  Note that this
        // method does not affect the "time" part of this object's value.

    void setTime(const bdet_Time& time);
        // Set the "time" part of this object's value to the specified 'time'.
        // Note that this method does not affect the "date" part of this
        // object's value.

    void setTime(int hour,
                 int minute      = 0,
                 int second      = 0,
                 int millisecond = 0);
        // Set the "time" part of this object's value to the specified 'hour',
        // and the optionally specified 'minute', 'second', and 'millisecond'
        // values, with trailing fields that are not specified set to 0.   The
        // behavior is undefined unless the supplied "time" fields comprises a
        // valid 'bdet_Time' value (see 'bdet_Time::isValid').  Note that this
        // method does not affect the "date" part of this object's value.  Also
        // note that if 'hour' is 24, then any other supplied arguments must be
        // 0.

    void setHour(int hour);
        // Set the "hour" field of the "time" part of this object's value to
        // the specified 'hour'.  If 'hour' is 24, all other fields of the
        // "time" part are set to 0; otherwise, no other fields are affected.
        // The behavior is undefined unless '0 <= hour <= 24'.

    void setMinute(int minute);
        // Set the "minute" field of the "time" part of this object's value to
        // the specified 'minute'.  If the "hour" field of this object is
        // initially 24, it will be set to 0; no other fields are affected.
        // The behavior is undefined unless '0 <= minute <= 59'.

    void setSecond(int second);
        // Set the "second" field of the "time" part of this object's value to
        // the specified 'second'.  If the "hour" field of this object is
        // initially 24, it will be set to 0; no other fields are affected.
        // The behavior is undefined unless '0 <= second <= 59'.

    void setMillisecond(int millisecond);
        // Set the "millisecond" field of the "time" part of this object's
        // value to the specified 'millisecond'.  If the "hour" field of this
        // object is initially 24, it will be set to 0; no other fields are
        // affected.  The behavior is undefined unless
        // '0 <= millisecond <= 999'.

    void addTime(bsls_Types::Int64 hours,
                 bsls_Types::Int64 minutes      = 0,
                 bsls_Types::Int64 seconds      = 0,
                 bsls_Types::Int64 milliseconds = 0);
        // Add the specified number of 'hours', and the optionally specified
        // number of 'minutes', 'seconds', and 'milliseconds', to the value of
        // this object, adjusting the "date" part of the value accordingly
        // with unspecified trailing arguments defaulting to 0.  Each argument
        // may independently be positive, 0, or negative, with the restriction
        // that it is representable as a *62-bit* integer number of
        // milliseconds.  The behavior is undefined unless the resulting value
        // is in the valid range for a 'bdet_Datetime' object.  Note that if
        // the "time" part of this object's value is "24:00:00.000", then it is
        // set to "00:00:00.000" before performing the addition.

    void addDays(int days);
        // Add the specified number of 'days' to the value of this object.
        // 'days' may be positive, 0, or negative.  The behavior is undefined
        // unless the resulting value would be in the valid range for a
        // 'bdet_Datetime' object.  Note that the "time" part of this datetime
        // value is unaffected.

    void addHours(bsls_Types::Int64 hours);
        // Add the specified number of 'hours' to the value of this object,
        // adjusting the "date" part of the value accordingly.  'hours' may be
        // positive, 0, or negative.  The behavior is undefined unless the
        // resulting value would be in the valid range for a 'bdet_Datetime'
        // object.  Note that if the "time" part of this object's value is
        // "24:00:00.000", it will be set to "00:00:00.000" before performing
        // the addition.

    void addMinutes(bsls_Types::Int64 minutes);
        // Add the specified number of 'minutes' to the value of this object,
        // adjusting the "date" part of the value accordingly.  'minutes' may
        // be positive, 0, or negative.  The behavior is undefined unless the
        // resulting value would be in the valid range for a 'bdet_Datetime'
        // object.  Note that if the "time" part of this object's value is
        // "24:00:00.000", it will be set to "00:00:00.000" before performing
        // the addition.

    void addSeconds(bsls_Types::Int64 seconds);
        // Add the specified number of 'seconds' to the value of this object,
        // adjusting the "date" part of the value accordingly.  'seconds' may
        // be positive, 0, or negative.  The behavior is undefined unless the
        // resulting value would be in the valid range for a 'bdet_Datetime'
        // object.  Note that if the "time" part of this object's value is
        // "24:00:00.000", it will be set to "00:00:00.000" before performing
        // the addition.

    void addMilliseconds(bsls_Types::Int64 milliseconds);
        // Add the specified number of 'milliseconds' to the value of this
        // object, adjusting the "date" part of the value accordingly.
        // 'milliseconds' may be positive, 0, or negative.  The behavior
        // undefined unless the resulting value would be in the valid range for
        // a 'bdet_Datetime' object.  Note that if the "time" part of this
        // object's value is "24:00:00.000", it will be set to "00:00:00.000"
        // before performing the addition.

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

    // ACCESSORS
    bdet_Date date() const;
        // Return the value of the "date" part of this datetime object.

    bdet_Time time() const;
        // Return the value of the "time" part of this datetime object.

    int year() const;
        // Return the value of the year field of this datetime object.

    int month() const;
        // Return the value of the month field of this datetime object.

    int day() const;
        // Return the value of the day (of the month) field of this datetime
        // object.

    int dayOfYear() const;
        // Return the value of the day of the year associated with this
        // datetime object.

    bdet_DayOfWeek::Day dayOfWeek() const;
        // Return the value of the day of the week associated with the day
        // (of the month) field of this datetime object.

    int hour() const;
        // Return the value of the hour field of this datetime object.

    int minute() const;
        // Return the value of the minute field of this datetime object.

    int second() const;
        // Return the value of the second field of this datetime object.

    int millisecond() const;
        // Return the value of the millisecond field of this datetime object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in
        // a human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute
        // value indicates the number of spaces per indentation level for this
        // and all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that this
        // human-readable format is not fully specified, and can change
        // without notice.

    int printToBuffer(char *result, int numBytes) const;
        // Efficiently write the value of this object to the specified 'result'
        // buffer of at least the specified 'numBytes'.  Return the number of
        // characters (not including the null character) that would have been
        // written if the limit due to 'numBytes' were not imposed.  'result' is
        // null-terminated unless 'numBytes' is 0.  The behavior is undefined
        // unless '0 <= numBytes' and 'result' refers to at least 'numBytes'
        // contiguous bytes.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to the modifiable
        // 'stream'.  If 'stream' is initially invalid, this operation has no
        // effect.  If 'version' is not supported, 'stream' is invalidated.
        // Note that in no event is 'version' written to 'stream'.  (See the
        // 'bdex' package-level documentation for more information on 'bdex'
        // streaming of value-semantic types and containers.)

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: Use 'maxSupportedBdexVersion' instead.

    bdet_Date& date();
        // Return a reference to the modifiable internal date object of this
        // datetime.
        //
        // DEPRECATED: Do *not* use.

    int validateAndSetDatetime(int year,
                               int month,
                               int day,
                               int hour        = 0,
                               int minute      = 0,
                               int second      = 0,
                               int millisecond = 0);
        // Set the "date" part of this object's value to the specified 'year',
        // 'month', and 'day', and the "time" part to the optionally specified
        // 'hour', 'minute', 'second', and 'millisecond', if they would
        // comprise a valid 'bdet_Datetime' value, with trailing fields that
        // are not specified set to 0.  Return 0 on success, and a non-zero
        // value (with no effect) otherwise.
        //
        // DEPRECATED: Use 'setDatetimeIfValid' instead.

    bsl::ostream& streamOut(bsl::ostream& stream) const;
        // Format this datetime to the specified output 'stream' and return a
        // reference to the modifiable 'stream'.
        //
        // DEPRECATED: Use 'print' instead.

#endif // BDE_OMIT_INTERNAL_DEPRECATED

};

// FREE OPERATORS
bdet_Datetime operator+(const bdet_Datetime&         lhs,
                        const bdet_DatetimeInterval& rhs);
    // Return a 'bdet_Datetime' value that is the sum of the specified 'lhs'
    // datetime and the specified 'rhs' datetime interval.  The behavior is
    // undefined unless the result can be represented as a valid
    // 'bdet_Datetime' value.

bdet_Datetime operator+(const bdet_DatetimeInterval& lhs,
                        const bdet_Datetime&         rhs);
    // Return a 'bdet_Datetime' value that is the sum of the specified 'lhs'
    // datetime interval and the specified 'rhs' datetime.  The behavior is
    // undefined unless the result can be represented as a valid
    // 'bdet_Datetime' value.

bdet_Datetime operator-(const bdet_Datetime&         lhs,
                        const bdet_DatetimeInterval& rhs);
    // Return a 'bdet_Datetime' value that is the difference between the
    // specified 'lhs' datetime and the specified 'rhs' datetime interval.  The
    // behavior is undefined unless the result can be represented as a valid
    // 'bdet_Datetime' value.

bdet_DatetimeInterval operator-(const bdet_Datetime& lhs,
                                const bdet_Datetime& rhs);
    // Return the 'bdet_DatetimeInterval' value that is the difference between
    // the specified 'lhs' and 'rhs' 'bdet_Datetime' values.

bool operator==(const bdet_Datetime& lhs, const bdet_Datetime& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' datetimes have the same
    // value, and 'false' otherwise.  Two datetimes have the same value if they
    // have the same respective values for their corresponding "date" and
    // "time" parts.

bool operator!=(const bdet_Datetime& lhs, const bdet_Datetime& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' datetimes do not have the
    // same value, and 'false' otherwise.  Two datetimes do not have the same
    // value if they do not have the same respective values for either of their
    // corresponding "date" or "time" parts.

bool operator<(const bdet_Datetime& lhs, const bdet_Datetime& rhs);
    // Return 'true' if the specified 'lhs' datetime value is less than the
    // specified 'rhs' datetime value, and 'false' otherwise.  A datetime 'a'
    // is less than another datetime 'b' if 'a.date() < b.date()', or if the
    // "date" part of 'a' and 'b' is the same and 'a.time() < b.time()'.  The
    // behavior is undefined if the "time" part of 'lhs' or 'rhs' has the value
    // "24:00:00.000".

bool operator<=(const bdet_Datetime& lhs, const bdet_Datetime& rhs);
    // Return 'true' if the specified 'lhs' datetime value is less than or
    // equal to the specified 'rhs' datetime value, and 'false' otherwise.  The
    // behavior is undefined if the "time" part of 'lhs' or 'rhs' has the value
    // "24:00:00.000".

bool operator>(const bdet_Datetime& lhs, const bdet_Datetime& rhs);
    // Return 'true' if the specified 'lhs' datetime value is greater than the
    // specified 'rhs' datetime value, and 'false' otherwise.  A datetime 'a'
    // is greater than another datetime 'b' if 'a.date() > b.date()', or if
    // the "date" part of 'a' and 'b' is the same and 'a.time() > b.time()'.
    // The behavior is undefined if the "time" part of 'lhs' or 'rhs' has the
    // value "24:00:00.000".

bool operator>=(const bdet_Datetime& lhs, const bdet_Datetime& rhs);
    // Return 'true' if the specified 'lhs' datetime value is greater than or
    // equal to the specified 'rhs' datetime value, and 'false' otherwise.  The
    // behavior is undefined if the "time" part of 'lhs' or 'rhs' has the value
    // "24:00:00.000".

bsl::ostream& operator<<(bsl::ostream& stream, const bdet_Datetime& datetime);
    // Write the value of the specified 'datetime' object to the specified
    // output 'stream' in a single-line format, and return a reference to
    // 'stream'.  If 'stream' is not valid on entry, this operation has no
    // effect.  Note that this human-readable format is not fully specified,
    // can change without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -------------------
                        // class bdet_Datetime
                        // -------------------

// CLASS METHODS
inline
bool bdet_Datetime::isValid(int year,
                            int month,
                            int day,
                            int hour,
                            int minute,
                            int second,
                            int millisecond)
{
    return bdet_Date::isValid(year, month, day)
        && bdet_Time::isValid(hour, minute, second, millisecond);
}

inline
int bdet_Datetime::maxSupportedBdexVersion()
{
    return 1;
}

// CREATORS
inline
bdet_Datetime::bdet_Datetime()
{
}

inline
bdet_Datetime::bdet_Datetime(const bdet_Date& date)
: d_date(date)
, d_time(0, 0, 0, 0)
{
}

inline
bdet_Datetime::bdet_Datetime(const bdet_Date& date, const bdet_Time& time)
: d_date(date)
, d_time(time)
{
}

inline
bdet_Datetime::bdet_Datetime(int year,
                             int month,
                             int day,
                             int hour,
                             int minute,
                             int second,
                             int millisecond)
: d_date(year, month, day)
, d_time(hour, minute, second, millisecond)
{
}

inline
bdet_Datetime::bdet_Datetime(const bdet_Datetime& original)
: d_date(original.d_date)
, d_time(original.d_time)
{
}

// MANIPULATORS
inline
bdet_Datetime& bdet_Datetime::operator=(const bdet_Datetime& rhs)
{
    d_date = rhs.d_date;
    d_time = rhs.d_time;
    return *this;
}

inline
bdet_Datetime& bdet_Datetime::operator+=(const bdet_DatetimeInterval& rhs)
{
    d_date += d_time.addInterval(rhs);
    return *this;
}

inline
bdet_Datetime& bdet_Datetime::operator-=(const bdet_DatetimeInterval& rhs)
{
    d_date += d_time.addInterval(-rhs);
    return *this;
}

inline
void bdet_Datetime::setDatetime(int year,
                                int month,
                                int day,
                                int hour,
                                int minute,
                                int second,
                                int millisecond)
{
    d_date.setYearMonthDay(year, month, day);
    d_time.setTime(hour, minute, second, millisecond);
}

inline
int bdet_Datetime::setDatetimeIfValid(int year,
                                      int month,
                                      int day,
                                      int hour,
                                      int minute,
                                      int second,
                                      int millisecond)
{
    enum { BDET_SUCCESS = 0, BDET_FAILURE = -1 };

    if (isValid(year, month, day, hour, minute, second, millisecond)) {
        setDatetime(year, month, day, hour, minute, second, millisecond);
        return BDET_SUCCESS;                                          // RETURN
    }

    return BDET_FAILURE;
}

inline
void bdet_Datetime::setDate(const bdet_Date& date)
{
    d_date = date;
}

inline
void bdet_Datetime::setYearMonthDay(int year, int month, int day)
{
    d_date.setYearMonthDay(year, month, day);
}

inline
void bdet_Datetime::setYearDay(int year, int dayOfYear)
{
    d_date.setYearDay(year, dayOfYear);
}

inline
void bdet_Datetime::setTime(const bdet_Time& time)
{
    d_time = time;
}

inline
void bdet_Datetime::setTime(int hour, int minute, int second, int millisecond)
{
    d_time.setTime(hour, minute, second, millisecond);
}

inline
void bdet_Datetime::setHour(int hour)
{
    d_time.setHour(hour);
}

inline
void bdet_Datetime::setMinute(int minute)
{
    d_time.setMinute(minute);
}

inline
void bdet_Datetime::setSecond(int second)
{
    d_time.setSecond(second);
}

inline
void bdet_Datetime::setMillisecond(int millisecond)
{
    d_time.setMillisecond(millisecond);
}

inline
void bdet_Datetime::addTime(bsls_Types::Int64 hours,
                            bsls_Types::Int64 minutes,
                            bsls_Types::Int64 seconds,
                            bsls_Types::Int64 milliseconds)
{
    const bdet_DatetimeInterval interval(0,
                                         hours,
                                         minutes,
                                         seconds,
                                         milliseconds);
    d_date += d_time.addInterval(interval);
}

inline
void bdet_Datetime::addDays(int days)
{
    d_date += days;
}

inline
void bdet_Datetime::addHours(bsls_Types::Int64 hours)
{
    const bsls_Types::Int64 totalMsec = hours * BDET_MILLISECONDS_PER_HOUR;

    const int normMsec  = (int)(totalMsec % BDET_MILLISECONDS_PER_DAY);
    const int wholeDays = (int)(totalMsec / BDET_MILLISECONDS_PER_DAY);

    d_date += wholeDays + d_time.addMilliseconds(normMsec);
}

inline
void bdet_Datetime::addMinutes(bsls_Types::Int64 minutes)
{
    const bsls_Types::Int64 totalMsec = minutes * BDET_MILLISECONDS_PER_MINUTE;

    const int normMsec  = static_cast<int>(
                                        totalMsec % BDET_MILLISECONDS_PER_DAY);
    const int wholeDays = static_cast<int>(
                                        totalMsec / BDET_MILLISECONDS_PER_DAY);

    d_date += wholeDays + d_time.addMilliseconds(normMsec);
}

inline
void bdet_Datetime::addSeconds(bsls_Types::Int64 seconds)
{
    const bsls_Types::Int64 totalMsec = seconds * BDET_MILLISECONDS_PER_SECOND;

    const int normMsec  = static_cast<int>(
                                        totalMsec % BDET_MILLISECONDS_PER_DAY);
    const int wholeDays = static_cast<int>(
                                        totalMsec / BDET_MILLISECONDS_PER_DAY);

    d_date += wholeDays + d_time.addMilliseconds(normMsec);
}

inline
void bdet_Datetime::addMilliseconds(bsls_Types::Int64 milliseconds)
{
    const int normMsec  = static_cast<int>(
                                     milliseconds % BDET_MILLISECONDS_PER_DAY);
    const int wholeDays = static_cast<int>(
                                     milliseconds / BDET_MILLISECONDS_PER_DAY);

    d_date += wholeDays + d_time.addMilliseconds(normMsec);
}

template <class STREAM>
STREAM& bdet_Datetime::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version (starting with 1)
          case 1: {
            bdet_Date dateTmp;
            bdet_Time timeTmp;
            dateTmp.bdexStreamIn(stream, 1);
            if (!stream) {
                return stream;                                        // RETURN
            }
            timeTmp.bdexStreamIn(stream, 1);
            if (!stream) {
                return stream;                                        // RETURN
            }
            d_date = dateTmp;
            d_time = timeTmp;
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

// ACCESSORS
inline
bdet_Date bdet_Datetime::date() const
{
    return d_date;
}

inline
bdet_Time bdet_Datetime::time() const
{
    return d_time;
}

inline
int bdet_Datetime::year() const
{
    return d_date.year();
}

inline
int bdet_Datetime::month() const
{
    return d_date.month();
}

inline
int bdet_Datetime::day() const
{
    return d_date.day();
}

inline
int bdet_Datetime::dayOfYear() const
{
    return d_date.dayOfYear();
}

inline
bdet_DayOfWeek::Day bdet_Datetime::dayOfWeek() const
{
    return d_date.dayOfWeek();
}

inline
int bdet_Datetime::hour() const
{
    return d_time.hour();
}

inline
int bdet_Datetime::minute() const
{
    return d_time.minute();
}

inline
int bdet_Datetime::second() const
{
    return d_time.second();
}

inline
int bdet_Datetime::millisecond() const
{
    return d_time.millisecond();
}

template <class STREAM>
STREAM& bdet_Datetime::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) {
          case 1: {
            d_date.bdexStreamOut(stream, 1);
            d_time.bdexStreamOut(stream, 1);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

inline
int bdet_Datetime::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

inline
bdet_Date& bdet_Datetime::date()
{
    return d_date;
}

inline
int bdet_Datetime::validateAndSetDatetime(int year,
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

inline
bsl::ostream& bdet_Datetime::streamOut(bsl::ostream& stream) const
{
    return stream << *this;
}

#endif // BDE_OMIT_INTERNAL_DEPRECATED

// FREE OPERATORS
inline
bdet_Datetime operator+(const bdet_Datetime&         lhs,
                        const bdet_DatetimeInterval& rhs)
{
    bdet_Datetime result(lhs);
    return result += rhs;
}

inline
bdet_Datetime operator+(const bdet_DatetimeInterval& lhs,
                        const bdet_Datetime&         rhs)
{
    bdet_Datetime result(rhs);
    return result += lhs;
}

inline
bdet_Datetime operator-(const bdet_Datetime&         lhs,
                        const bdet_DatetimeInterval& rhs)
{
    bdet_Datetime result(lhs);
    return result -= rhs;
}

inline
bdet_DatetimeInterval operator-(const bdet_Datetime& lhs,
                                const bdet_Datetime& rhs)
{
    bdet_DatetimeInterval timeInterval1(lhs.d_date - rhs.d_date, 0, 0);
    bdet_DatetimeInterval timeInterval2 = lhs.d_time - rhs.d_time;
    timeInterval1 += timeInterval2;

    return timeInterval1;
}

inline
bool operator==(const bdet_Datetime& lhs, const bdet_Datetime& rhs)
{
    return lhs.date() == rhs.date() && lhs.time() == rhs.time();
}

inline
bool operator!=(const bdet_Datetime& lhs, const bdet_Datetime& rhs)
{
    return lhs.date() != rhs.date() || lhs.time() != rhs.time();
}

inline
bool operator<(const bdet_Datetime& lhs, const bdet_Datetime& rhs)
{
    return lhs.date() <  rhs.date()
       || (lhs.date() == rhs.date() && lhs.time() < rhs.time());
}

inline
bool operator<=(const bdet_Datetime& lhs, const bdet_Datetime& rhs)
{
    return lhs.date() <  rhs.date()
       || (lhs.date() == rhs.date() && lhs.time() <= rhs.time());
}

inline
bool operator>(const bdet_Datetime& lhs, const bdet_Datetime& rhs)
{
    return lhs.date() >  rhs.date()
       || (lhs.date() == rhs.date() && lhs.time() > rhs.time());
}

inline
bool operator>=(const bdet_Datetime& lhs, const bdet_Datetime& rhs)
{
    return lhs.date() >  rhs.date()
       || (lhs.date() == rhs.date() && lhs.time() >= rhs.time());
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
