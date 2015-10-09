// bdlt_date.h                                                        -*-C++-*-
#ifndef INCLUDED_BDLT_DATE
#define INCLUDED_BDLT_DATE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic type to represent dates.
//
//@CLASSES:
//  bdlt::Date: value-semantic date type using the proleptic Gregorian calendar
//
//@SEE_ALSO: bdlt_dayofweek, bdlt_serialdateimputil
//
//@DESCRIPTION: This component defines a value-semantic class, 'bdlt::Date',
// capable of representing any valid date that is consistent with the proleptic
// Gregorian calendar restricted to the years 1 through 9999 (inclusive):
//..
//  http://en.wikipedia.org/wiki/Proleptic_Gregorian_calendar
//..
// "Actual" (i.e., natural) day and date calculations are supported directly by
// 'bdlt::Date' and its associated free operators.  Calculations involving
// business days (or holidays), and day-count conventions (e.g., "ISMA30360"),
// can be found elsewhere.
//
///Valid Date Values and Their Representations
///-------------------------------------------
// A 'bdlt::Date' object *always* represents a valid date value as defined by
// the proleptic Gregorian calendar.  The value of a 'bdlt::Date' object can be
// expressed in the interface as either '(year, month, day)', the canonical
// representation of dates, or '(year, dayOfYear)'.  For example,
// '(1959, 3, 8)' represents the same valid 'bdlt::Date' value as '(1959, 67)'
// because the 8th day of the 3rd month of the year 1959 is also the 67th day
// of the year 1959.
//
// Of course, not all combinations of '(year, month, day)' and
// '(year, dayOfYear)' constitute valid values for 'bdlt::Date' objects.  A
// '(year, dayOfYear)' pair does *not* represent a valid 'bdlt::Date' value
// unless '1 <= year <= 9999' and '1 <= dayOfYear <= 366'.  Additionally, if
// 'year' is not a leap year, then the representation is not valid unless
// '1 <= dayOfYear <= 365'.
//
// Note that, in a leap year, February has 29 days instead of the usual 28.
// (Thus, leap years have 366 days instead of the usual 365.)  The proleptic
// Gregorian calendar retroactively applies the leap year rules instituted by
// the Gregorian Reformation to *all* years.  In particular, a year in the
// range '[1 .. 9999]' is a leap year if it is divisible by 4, but *not*
// divisible by 100, *unless* it is *also* divisible by 400.
//
// A '(year, month, day)' triple does *not* represent a valid 'bdlt::Date'
// value unless '1 <= year <= 9999', '1 <= month <= 12', and '1 <= day <= 31'.
// Also, when 'month' is 4, 6, 9, or 11 (April, June, September, or November),
// the representation is not valid unless '1 <= day <= 30', and, when 'month'
// is 2 (February), unless '1 <= day <= 29'.  Finally, when 'month' is 2 and
// 'year' is not a leap year, then the representation is not valid unless
// '1 <= day <= 28'.
//
// Note that two 'static' (class) methods:
//..
//  bool isValidYearDay(int year, int dayOfYear);
//  bool isValidYearMonthDay(int year, int month, int day);
//..
// are provided within 'bdlt::Date' to indicate whether the given pair or
// triple of integers would represent a valid 'bdlt::Date' value (e.g., prior
// to using them to construct a 'bdlt::Date' object).
//
///Ensuring 'bdlt::Date' Validity
///------------------------------
// Note that it is incumbent on the client of a 'bdlt::Date' object never to
// cause it, directly or indirectly, to hold an invalid value, which can occur
// only by violating explicitly stated preconditions.  For example, invoking
// 'operator++' on a date object that represents the valid 'bdlt::Date' value
// 9999/12/31 (December 31, 9999) is a contract violation that can lead to
// undefined behavior.  Similarly, attempting to set the value of an existing
// date using the 'setYearMonthDay' manipulator such that invoking
// 'isValidYearMonthDay' would return 'false' on the same '(year, month, day)'
// arguments is another contract violation.
//
// When setting a 'bdlt::Date' object to a particular value, there are two
// forms of methods provided for both of the '(year, month, day)' and
// '(year, dayOfYear)' representations of date values.  When you are *certain*
// that the value you are trying to set is valid, either of the following two
// runtime-efficient methods can be used safely:
//..
//  void setYearDay(int year, int dayOfYear);
//  void setYearMonthDay(int year, int month, int day);
//..
// If, however, the integral date attributes at hand are not known to represent
// a valid date, they must first be validated, e.g., by calling one of the two
// 'static' 'isValid*' methods, or by calling the appropriate set method having
// the 'IfValid' suffix, which will always verify validity before either
// setting the value and returning 0, or returning a non-zero status with no
// effect on the object:
//..
//  int setYearDayIfValid(int year, int dayOfYear);
//  int setYearMonthDayIfValid(int year, int month, int day);
//..
// Note that if the value is "known" to be valid, and these latter 'IfValid'
// variants are called without checking their return status, we run the risk of
// a "double fault" in that if the value is not actually valid, there is no way
// for a robust implementation (such as this one) to check for the error in a
// defensive (e.g., "DEBUG" or "SAFE") build mode.
//
///BDEX Compatibility with Legacy POSIX-Based Date
///-----------------------------------------------
// The version 1 format supported by 'bdlt::Date' for BDEX streaming is
// expressly intended for maintaining some degree of "compatibility" with a
// legacy date class that uses a (non-proleptic) Gregorian calendar matching
// the POSIX 'cal' command.  Over the range of dates supported by 'bdlt::Date'
// ('[0001JAN01 .. 9999DEC31']), the proleptic Gregorian calendar (used by
// 'bdlt::Date') has two fewer days than 'cal', and some dates that exist in
// one calendar do not exist in the other; therefore, true compatibility is not
// possible.  The compatibility guaranteed by BDEX streaming version 1 is such
// that all dates in the range '[1752SEP14 .. 9999DEC31]', as well as the
// default value ('0001JAN01'), can be successfully exchanged, via BDEX,
// between 'bdlt::Date' and the legacy date class.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlt::Date'
/// - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to create and use a
// 'bdlt::Date' object.
//
// First, we create a default date 'd1':
//..
//  bdlt::Date d1;           assert(   1 == d1.year());
//                           assert(   1 == d1.month());
//                           assert(   1 == d1.day());
//..
// Next, we set 'd1' to July 4, 1776:
//..
//  d1.setYearMonthDay(1776, 7, 4);
//                           assert(1776 == d1.year());
//                           assert(   7 == d1.month());
//                           assert(   4 == d1.day());
//..
// We can also use 'setYearMonthDayIfValid' if we are not sure whether a
// particular year/month/day combination constitutes a valid 'bdlt::Date'.  For
// example, if we want to set 'd1' to '1900/02/29', and it turns out that year
// 1900 was not a leap year (it wasn't), there will be no effect on the current
// value of the object:
//..
//  int ret = d1.setYearMonthDayIfValid(1900, 2, 29);
//                           assert(   0 != ret);         // 1900 not leap year
//                           assert(1776 == d1.year());   // no effect on 'd1'
//                           assert(   7 == d1.month());
//                           assert(   4 == d1.day());
//..
// Then, from 'd1', we can determine the day of the year, and the day of the
// week, of July 4, 1776:
//..
//  int dayOfYear = d1.dayOfYear();
//                           assert( 186 == dayOfYear);
//
//  bdlt::DayOfWeek::Enum dayOfWeek = d1.dayOfWeek();
//                           assert(bdlt::DayOfWeek::e_THU == dayOfWeek);
//..
// Next, we create a 'bdlt::Date' object, 'd2', using the year/day-of-year
// representation for dates:
//..
//  bdlt::Date d2(1776, dayOfYear);
//                           assert(1776 == d2.year());
//                           assert( 186 == d2.dayOfYear());
//                           assert(   7 == d2.month());
//                           assert(   4 == d2.day());
//                           assert(  d1 == d2);
//..
// Then, we add six days to the value of 'd2':
//..
//  d2 += 6;                 assert(1776 == d2.year());
//                           assert(   7 == d2.month());
//                           assert(  10 == d2.day());
//..
// Now, we subtract 'd1' from 'd2', storing the (signed) difference in days
// (a.k.a. *Actual* difference) in 'daysDiff':
//..
//  int daysDiff = d2 - d1;  assert(   6 == daysDiff);
//..
// Finally, we stream the value of 'd2' to 'stdout':
//..
//  bsl::cout << d2 << bsl::endl;
//..
// The streaming operator produces:
//..
//  10JUL1776
//..
// on 'stdout'.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DAYOFWEEK
#include <bdlt_dayofweek.h>
#endif

#ifndef INCLUDED_BDLT_MONTHOFYEAR
#include <bdlt_monthofyear.h>
#endif

#ifndef INCLUDED_BDLT_SERIALDATEIMPUTIL
#include <bdlt_serialdateimputil.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLH_HASH
#include <bslh_hash.h>
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
                                  // class Date
                                  // ==========

class Date {
    // This class implements a complex-constrained, value-semantic type for
    // representing dates according to the proleptic Gregorian calendar.  Each
    // object of this class *always* represents a *valid* date value in the
    // range '[0001JAN01 .. 9999DEC31]' inclusive.  The interface of this class
    // supports 'Date' values expressed in terms of either year/month/day (the
    // canonical representation) or year/day-of-year (an alternate
    // representation).  See {Valid Date Values and Their Representations} for
    // details.


    // DATA
    int d_serialDate;  // absolute serial date (1 == 1/1/1, 2 == 1/1/2, ...)

    // FRIENDS
    friend bool operator==(const Date&, const Date&);
    friend bool operator!=(const Date&, const Date&);
    friend bool operator< (const Date&, const Date&);
    friend bool operator<=(const Date&, const Date&);
    friend bool operator>=(const Date&, const Date&);
    friend bool operator> (const Date&, const Date&);
    friend Date operator+(const Date&, int);
    friend Date operator+(int, const Date&);
    friend Date operator-(const Date&, int);
    friend int  operator-(const Date&, const Date&);
    template <class HASHALG>
    friend void hashAppend(HASHALG& hashAlg, const Date&);

  private:
    // PRIVATE CLASS METHODS
    static bool isValidSerial(int serialDate);
        // Return 'true' if the specified 'serialDate' represents a valid value
        // for a 'Date' object, and 'false' otherwise.  'serialDate' represents
        // a valid 'Date' value if it corresponds to a valid date as defined by
        // the proleptic Gregorian calendar confined to the year range
        // '[1 .. 9999]' inclusive, where serial date 1 corresponds to
        // '0001/01/01' and each successive day has a serial date value that is
        // 1 greater than that of the previous day.  See {Valid Date Values and
        // Their Representations} for details.


    // PRIVATE CREATORS
    explicit Date(int serialDate);
        // Create a date initialized with the value indicated by the specified
        // 'serialDate'.  The behavior is undefined unless 'serialDate'
        // represents a valid 'Date' value.

  public:
    // CLASS METHODS
    static bool isValidYearDay(int year, int dayOfYear);
        // Return 'true' if the specified 'year' and 'dayOfYear' represent a
        // valid value for a 'Date' object, and 'false' otherwise.  'year' and
        // 'dayOfYear' represent a valid 'Date' value if they correspond to a
        // valid date as defined by the proleptic Gregorian calendar confined
        // to the year range '[1 .. 9999]' inclusive.  See {Valid Date Values
        // and Their Representations} for details.

    static bool isValidYearMonthDay(int year, int month, int day);
        // Return 'true' if the specified 'year', 'month', and 'day' represent
        // a valid value for a 'Date' object, and 'false' otherwise.  'year',
        // 'month', and 'day' represent a valid 'Date' value if they correspond
        // to a valid date as defined by the proleptic Gregorian calendar
        // confined to the year range '[1 .. 9999]' inclusive.  See {Valid Date
        // Values and Their Representations} for details.

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
    Date();
        // Create a 'Date' object having the earliest supported date value,
        // i.e., having a year/month/day representation of '0001/01/01'.

    Date(int year, int dayOfYear);
        // Create a 'Date' object having the value represented by the specified
        // 'year' and 'dayOfYear'.  The behavior is undefined unless 'year' and
        // 'dayOfYear' represent a valid 'Date' value (see 'isValidYearDay').

    Date(int year, int month, int day);
        // Create a 'Date' object having the value represented by the specified
        // 'year', 'month', and 'day'.  The behavior is undefined unless
        // 'year', 'month', and 'day' represent a valid 'Date' value (see
        // 'isValidYearMonthDay').

    Date(const Date& original);
        // Create a 'Date' object having the value of the specified 'original'
        // date.

    ~Date();
        // Destroy this object.

    // MANIPULATORS
    Date& operator=(const Date& rhs);
        // Assign to this object the value of the specified 'rhs' date, and
        // return a reference providing modifiable access to this object.

    Date& operator+=(int numDays);
        // Assign to this object the value that is later by the specified
        // (signed) 'numDays' from its current value, and return a reference
        // providing modifiable access to this object.  The behavior is
        // undefined unless the resulting value falls within the range of dates
        // supported by this class (see 'isValidYearMonthDay').  Note that
        // 'numDays' may be negative.

    Date& operator-=(int numDays);
        // Assign to this object the value that is earlier by the specified
        // (signed) 'numDays' from its current value, and return a reference
        // providing modifiable access to this object.  The behavior is
        // undefined unless the resulting value falls within the range of dates
        // supported by this class (see 'isValidYearMonthDay').  Note that
        // 'numDays' may be negative.

    Date& operator++();
        // Set this object to have the value that is one day later than its
        // current value, and return a reference providing modifiable access to
        // this object.  The behavior is undefined if the year/month/day
        // representation of the current value is '9999/12/31'.

    Date& operator--();
        // Set this object to have the value that is one day earlier than its
        // current value, and return a reference providing modifiable access to
        // this object.  The behavior is undefined if the year/month/day
        // representation of the current value is '0001/01/01'.

    int addDaysIfValid(int numDays);
        // Set this object to have the value that is later by the specified
        // (signed) 'numDays' from its current value, if the resulting value
        // falls within the range of dates supported by this class (see
        // 'isValidYearMonthDay').  Return 0 on success, and a non-zero value
        // (with no effect) otherwise.  Note that 'numDays' may be negative.

    void setYearDay(int year, int dayOfYear);
        // Set this object to have the value represented by the specified
        // 'year' and 'dayOfYear'.  The behavior is undefined unless 'year' and
        // 'dayOfYear' represent a valid 'Date' value (see 'isValidYearDay').

    int setYearDayIfValid(int year, int dayOfYear);
        // Set this object to have the value represented by the specified
        // 'year' and 'dayOfYear' if they comprise a valid 'Date' value (see
        // 'isValidYearDay').  Return 0 on success, and a non-zero value (with
        // no effect) otherwise.

    void setYearMonthDay(int year, int month, int day);
        // Set this object to have the value represented by the specified
        // 'year', 'month', and 'day'.  The behavior is undefined unless
        // 'year', 'month', and 'day' represent a valid 'Date' value (see
        // 'isValidYearMonthDay').

    int setYearMonthDayIfValid(int year, int month, int day);
        // Set this object to have the value represented by the specified
        // 'year', 'month', and 'day' if they comprise a valid 'Date' value
        // (see 'isValidYearMonthDay').  Return 0 on success, and a non-zero
        // value (with no effect) otherwise.

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

    // ATTRIBUTE ACCESSORS
    int day() const;
        // Return the day of the month in the range '[1 .. 31]' of this date.

    int dayOfYear() const;
        // Return the day of the year in the range '[1 .. 366]' of this date.

    int month() const;
        // Return the month of the year in the range '[1 .. 12]' of this date.

    int year() const;
        // Return the year in the range '[1 .. 9999]' of this date.

    // ACCESSORS
    DayOfWeek::Enum dayOfWeek() const;
        // Return the day of the week in the range
        // '[DayOfWeek::e_SUN .. DayOfWeek::e_SAT]' of this date.

    void getYearDay(int *year, int *dayOfYear) const;
        // Load, into the specified 'year' and 'dayOfYear', the respective
        // 'year' and 'dayOfYear' attribute values of this date.

    void getYearMonthDay(int *year, int *month, int *day) const;
        // Load, into the specified 'year', 'month', and 'day', the respective
        // 'year', 'month', and 'day' attribute values of this date.

    MonthOfYear::Enum monthOfYear() const;
        // Return the month of the year in the range
        // '[MonthOfYear::e_JAN .. MonthOfYear::e_DEC]' of this date.

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
bool operator==(const Date& lhs, const Date& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Date' objects have the same value if
    // each of their 'year', 'month', and 'day' attributes (respectively) have
    // the same value.

bool operator!=(const Date& lhs, const Date& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'Date' objects do not have the
    // same value if any of their 'year', 'month', and 'day' attributes
    // (respectively) do not have the same value.

bsl::ostream& operator<<(bsl::ostream& stream, const Date& date);
    // Write the value of the specified 'date' object to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

bool operator<(const Date& lhs, const Date& rhs);
    // Return 'true' if the specified 'lhs' date is earlier than the specified
    // 'rhs' date, and 'false' otherwise.

bool operator<=(const Date& lhs, const Date& rhs);
    // Return 'true' if the specified 'lhs' date is earlier than or the same as
    // the specified 'rhs' date, and 'false' otherwise.

bool operator>(const Date& lhs, const Date& rhs);
    // Return 'true' if the specified 'lhs' date is later than the specified
    // 'rhs' date, and 'false' otherwise.

bool operator>=(const Date& lhs, const Date& rhs);
    // Return 'true' if the specified 'lhs' date is later than or the same as
    // the specified 'rhs' date, and 'false' otherwise.

Date operator++(Date& date, int);
    // Set the specified 'date' object to have the value that is one day later
    // than its current value, and return the value of 'date' on entry.  The
    // behavior is undefined if the value of 'date' on entry is '9999/12/31'.

Date operator--(Date& date, int);
    // Set the specified 'date' object to have the value that is one day
    // earlier than its current value, and return the value of 'date' on entry.
    // The behavior is undefined if the value of 'date' on entry is
    // '0001/01/01'.

Date operator+(const Date& date,    int         numDays);
Date operator+(int         numDays, const Date& date);
    // Return the date value that is later by the specified (signed) 'numDays'
    // from the specified 'date'.  The behavior is undefined unless the
    // resulting value falls within the range of dates supported by this class
    // (see 'isValidYearMonthDay').  Note that 'numDays' may be negative.

Date operator-(const Date& date, int numDays);
    // Return the date value that is earlier by the specified (signed)
    // 'numDays' from the specified 'date'.  The behavior is undefined unless
    // the resulting value falls within the range of dates supported by this
    // class (see 'isValidYearMonthDay').  Note that 'numDays' may be negative.

int operator-(const Date& lhs, const Date& rhs);
    // Return the (signed) number of days between the specified 'lhs' and 'rhs'
    // dates.  Note that if 'lhs < rhs' the result will be negative.

template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const Date& date);
    // Pass the specified 'date' to the specified 'hashAlg'.  Note that this
    // function is intended to integrate with the 'bslh' modular hashing
    // system, and effectively provides a 'bsl::hash' specialization for
    // 'date'.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                                  // ----------
                                  // class Date
                                  // ----------

// PRIVATE CLASS METHODS
inline
bool Date::isValidSerial(int serialDate)
{
    return SerialDateImpUtil::isValidSerial(serialDate);
}

// PRIVATE CREATORS
inline
Date::Date(int serialDate)
: d_serialDate(serialDate)
{
    BSLS_ASSERT_SAFE(Date::isValidSerial(d_serialDate));
}

// CLASS METHODS
inline
bool Date::isValidYearDay(int year, int dayOfYear)
{
    return SerialDateImpUtil::isValidYearDay(year, dayOfYear);
}

inline
bool Date::isValidYearMonthDay(int year, int month, int day)
{
    return SerialDateImpUtil::isValidYearMonthDay(year, month, day);
}

                                  // Aspects

inline
int Date::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// CREATORS
inline
Date::Date()
: d_serialDate(1)
{
}

inline
Date::Date(int year, int dayOfYear)
: d_serialDate(SerialDateImpUtil::ydToSerial(year, dayOfYear))
{
    BSLS_ASSERT_SAFE(isValidYearDay(year, dayOfYear));

}

inline
Date::Date(int year, int month, int day)
: d_serialDate(SerialDateImpUtil::ymdToSerial(year, month, day))
{
    BSLS_ASSERT_SAFE(isValidYearMonthDay(year, month, day));

}

inline
Date::Date(const Date& original)
: d_serialDate(original.d_serialDate)
{
}

inline
Date::~Date()
{
    BSLS_ASSERT_SAFE(Date::isValidSerial(d_serialDate));
}

// MANIPULATORS
inline
Date& Date::operator=(const Date& rhs)
{
    d_serialDate = rhs.d_serialDate;
    return *this;
}

inline
Date& Date::operator+=(int numDays)
{
    BSLS_ASSERT_SAFE(Date::isValidSerial(d_serialDate + numDays));


    d_serialDate += numDays;
    return *this;
}

inline
Date& Date::operator-=(int numDays)
{
    BSLS_ASSERT_SAFE(Date::isValidSerial(d_serialDate - numDays));


    d_serialDate -= numDays;
    return *this;
}

inline
Date& Date::operator++()
{
    BSLS_ASSERT_SAFE(*this != Date(9999, 12, 31));


    ++d_serialDate;
    return *this;
}

inline
Date& Date::operator--()
{
    BSLS_ASSERT_SAFE(*this != Date(1, 1, 1));


    --d_serialDate;
    return *this;
}

inline
void Date::setYearDay(int year, int dayOfYear)
{
    BSLS_ASSERT_SAFE(isValidYearDay(year, dayOfYear));

    d_serialDate = SerialDateImpUtil::ydToSerial(year, dayOfYear);
}

inline
int Date::setYearDayIfValid(int year, int dayOfYear)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (isValidYearDay(year, dayOfYear)) {
        setYearDay(year, dayOfYear);
        return k_SUCCESS;                                             // RETURN
    }

    return k_FAILURE;
}

inline
void Date::setYearMonthDay(int year, int month, int day)
{
    BSLS_ASSERT_SAFE(isValidYearMonthDay(year, month, day));

    d_serialDate = SerialDateImpUtil::ymdToSerial(year, month, day);
}

inline
int Date::setYearMonthDayIfValid(int year, int month, int day)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    if (isValidYearMonthDay(year, month, day)) {
        setYearMonthDay(year, month, day);
        return k_SUCCESS;                                             // RETURN
    }

    return k_FAILURE;
}

                                  // Aspects

template <class STREAM>
STREAM& Date::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            int tmpSerialDate = 0;

            stream.getInt24(tmpSerialDate);

            // See {BDEX Compatibility with Legacy POSIX-Based 'Date'} in the
            // component-level documentation.

            if (tmpSerialDate > 3) {
                tmpSerialDate -= 2;  // ensure that serial values for 1752SEP14
                                     // and later dates "align"
            }
            else if (tmpSerialDate > 0) {
                tmpSerialDate = 1;   // "fuzzy" default value '[1 .. 3]'
            }

            if (stream && Date::isValidSerial(tmpSerialDate)) {
                d_serialDate = tmpSerialDate;

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

// ATTRIBUTE ACCESSORS
inline
int Date::day() const
{
    return SerialDateImpUtil::serialToDay(d_serialDate);
}

inline
int Date::dayOfYear() const
{
    return SerialDateImpUtil::serialToDayOfYear(d_serialDate);
}

inline
int Date::month() const
{
    return SerialDateImpUtil::serialToMonth(d_serialDate);
}

inline
int Date::year() const
{
    return SerialDateImpUtil::serialToYear(d_serialDate);
}

// ACCESSORS
inline
DayOfWeek::Enum Date::dayOfWeek() const
{
    return static_cast<DayOfWeek::Enum>(
                           SerialDateImpUtil::serialToDayOfWeek(d_serialDate));
}

inline
void Date::getYearDay(int *year, int *dayOfYear) const
{
    BSLS_ASSERT_SAFE(year);
    BSLS_ASSERT_SAFE(dayOfYear);

    SerialDateImpUtil::serialToYd(year, dayOfYear, d_serialDate);
}

inline
void Date::getYearMonthDay(int *year, int *month, int *day) const
{
    BSLS_ASSERT_SAFE(year);
    BSLS_ASSERT_SAFE(month);
    BSLS_ASSERT_SAFE(day);

    SerialDateImpUtil::serialToYmd(year, month, day, d_serialDate);
}

inline
MonthOfYear::Enum Date::monthOfYear() const
{
    return static_cast<MonthOfYear::Enum>(month());
}

                                  // Aspects

template <class STREAM>
STREAM& Date::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {

            // See {BDEX Compatibility with Legacy POSIX-Based 'Date'} in the
            // component-level documentation.

            if (1 == d_serialDate) {  // preserve default value
                stream.putInt24(d_serialDate);
            }
            else {
                stream.putInt24(d_serialDate + 2);
                                      // ensure that serial values for
                                      // 1752SEP14 and later dates "align"
            }
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
bool bdlt::operator==(const Date& lhs, const Date& rhs)
{
    return lhs.d_serialDate == rhs.d_serialDate;
}

inline
bool bdlt::operator!=(const Date& lhs, const Date& rhs)
{
    return lhs.d_serialDate != rhs.d_serialDate;
}

inline
bsl::ostream& bdlt::operator<<(bsl::ostream& stream, const Date& date)
{
    return date.print(stream, 0, -1);
}

inline
bool bdlt::operator<(const Date& lhs, const Date& rhs)
{
    return lhs.d_serialDate < rhs.d_serialDate;
}

inline
bool bdlt::operator<=(const Date& lhs, const Date& rhs)
{
    return lhs.d_serialDate <= rhs.d_serialDate;
}

inline
bool bdlt::operator>(const Date& lhs, const Date& rhs)
{
    return lhs.d_serialDate > rhs.d_serialDate;
}

inline
bool bdlt::operator>=(const Date& lhs, const Date& rhs)
{
    return lhs.d_serialDate >= rhs.d_serialDate;
}

inline
bdlt::Date bdlt::operator++(Date& date, int)
{
    BSLS_ASSERT_SAFE(date != Date(9999, 12, 31));

    Date tmp(date);
    ++date;
    return tmp;
}

inline
bdlt::Date bdlt::operator--(Date& date, int)
{
    BSLS_ASSERT_SAFE(date != Date(1, 1, 1));

    Date tmp(date);
    --date;
    return tmp;
}

inline
bdlt::Date bdlt::operator+(const Date& date, int numDays)
{
    BSLS_ASSERT_SAFE(Date::isValidSerial(date.d_serialDate + numDays));


    return Date(date.d_serialDate + numDays);
}

inline
bdlt::Date bdlt::operator+(int numDays, const Date& date)
{
    BSLS_ASSERT_SAFE(Date::isValidSerial(numDays + date.d_serialDate));


    return Date(numDays + date.d_serialDate);
}

inline
bdlt::Date bdlt::operator-(const Date& date, int numDays)
{
    BSLS_ASSERT_SAFE(Date::isValidSerial(date.d_serialDate - numDays));


    return Date(date.d_serialDate - numDays);
}

inline
int bdlt::operator-(const Date& lhs, const Date& rhs)
{

    return lhs.d_serialDate - rhs.d_serialDate;
}

// ASPECTS
template <class HASHALG>
void bdlt::hashAppend(HASHALG& hashAlg, const Date& date)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlg, date.d_serialDate);
}

}  // close enterprise namespace

namespace bsl {

template <>
struct is_trivially_copyable<BloombergLP::bdlt::Date> : bsl::true_type {
    // This template specialization for 'is_trivially_copyable' indicates that
    // 'Date' is a trivially copyable type.
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
