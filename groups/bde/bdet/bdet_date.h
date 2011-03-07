// bdet_date.h                                                        -*-C++-*-
#ifndef INCLUDED_BDET_DATE
#define INCLUDED_BDET_DATE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic type supporting the Unix date convention.
//
//@CLASSES:
//  bdet_Date: fully value-semantic Gregorian date type consistent with Unix
//
//@SEE_ALSO: bdecs_calendar, bdet_dayofweek, bbedc_isma30360
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component defines a fully value-semantic date class,
// 'bdet_Date', capable of representing any valid Gregorian date from the year
// 1 through the year 9999, inclusive (consistent with the Unix standard).
// Actual day and date calculations are supported directly by 'bdet_Date' and
// its associated free operators.  Calculations involving business days (or
// holidays), or day-count conventions (e.g., "ISMA30360") can be found
// elsewhere (see 'bdecs_calendar' and the 'bbedc' day-count convention
// package).
//
///Valid Date Values and Their Representations
///-------------------------------------------
// A 'bdet_Date' object *always* represents a valid data value as defined
// by the standard Unix date convention.  The value of a 'bdet_Date' object is
// entirely independent of its internal representation, and can be expressed
// in the interface as either '(year, month, day)' or by '(year, dayOfYear)'.
// For example, '(1959, 3, 8)' represents the same valid 'bdet_Date' value as
// '(1959, 67)' because the 8th day of the 3rd month of the year 1959 is also
// the 67th day of the year 1959.
//
// Valid years range from 1 to 9999, valid months (of the year) range from
// 1 to 12, valid days of the month range from 1 to 31, and valid days of the
// year range from 1 to 366.  Of course, not all combinations of (valid 'year',
// valid 'month', valid 'day') or (valid 'year', valid 'dayOfYear') comprise
// a valid overall value for a 'bdet_Date' object.  Two overloaded static
// (class) 'isValid' methods:
//..
//  bool isValid(int year, int month, int day);
//  bool isValid(int year, int dayOfyear);
//..
// are provided within 'bdet_Date' to indicate whether the given sequence of
// integers would represent a valid 'bdet_Date' value (e.g., prior to using
// them to construct a 'bdet_Date' object).
//
// Note that it is incumbent on the client of a 'bdet_Date' object never to
// cause it, directly or indirectly, to hold an invalid value, which can occur
// only by violating explicitly stated pre-conditions.  For example, invoking
// 'operator++' on a date object that represents the valid 'bdet_Date' value
// December 31, 9999, would violate an explicit precondition, and therefore
// would result in undefined behavior.  Similarly, attempting to set the value
// of an existing date using the 'setYearMonthDay' manipulator such that
// invoking 'isValid' would return 'false' on the same '(year, month, day)'
// arguments would also result in undefined behavior, invalidating all
// guarantees, in particular, the invariant that every 'bdet_Date' object
// represents a valid value.
//
// When setting a 'bdet_Date' object to a particular value, there are two forms
// of methods provided for both '(year, month, day)' and '(year, dayOfYear)'
// representations of date values.  When you already know that the value you
// are trying to set is valid, either of the following two runtime-efficient
// functions can be used safely:
//..
//  void setYearDay(int year, int dayOfyear);
//  void setYearMonthDay(int year, int month, int day);
//..
// If, however, the individual valid integer attributes are not known to
// represent a valid date in history, they must first be validated (e.g., by
// first calling one of the two overloaded static 'isValid' methods, or by
// calling the appropriate (necessarily less efficient) set method having the
// "IfValid" suffix, which will always verify validity before either setting
// the value and returning 0, or otherwise returning a non-zero status with no
// effect on the object:
//..
//  int setYearDayIfValid(int year, int dayOfyear);
//  int setYearMonthDayIfValid(int year, int month, int day);
//..
// Note that if the value is "known" to be valid, and these latter "IfValid"
// variants are called without checking status, we run the risk of a "double
// fault" in that if the value is not valid, there is no way for a robust
// implementation (such as this one) to check for the error in a defensive
// (e.g., "DEBUG" or "SAFE") build mode.
//
///Usage
///-----
// The following snippets of code illustrate how to create and use a
// 'bdet_date' object.  First create a default date 'd1':
//..
//  bdet_Date d1;           assert(   1 == d1.year());
//                          assert(   1 == d1.month());
//                          assert(   1 == d1.day());
//..
// Next, set 'd1' to July 4, 1776:
//..
//  d1.setYearMonthDay(1776, 7, 4);
//                          assert(1776 == d1.year());
//                          assert(   7 == d1.month());
//                          assert(   4 == d1.day());
//..
// We can also use 'setYearMonthDayIfValid' if we are not sure whether the
// particular date we want to set to is a valid 'bdet_Date'.  For example, if
// we want to set the date to '1900/02/29', and it turns out that year 1900 is
// not a leap year, there will be no effect on the existing value of the date:
//..
//  int ret = d1.setYearMonthDayIfValid(1900, 2, 29);
//                          assert(   0 != ret);         // not leap year
//
//                          assert(1776 == d1.year());   // no effect
//                          assert(   7 == d1.month());  // on the
//                          assert(   4 == d1.day());    // object
//..
// From the date object, we can tell the day of year and day of week:
//..
//  int dayOfYear = d1.dayOfYear();
//                          assert( 186 == dayOfYear);
//
//  bdet_DayOfWeek::Day dayOfWeek = d1.dayOfWeek();
//                          assert(bdet_DayOfWeek::BDET_THU == dayOfWeek);
//..
// Finally, we can also create a date object 'd2' using the year and day of
// year:
//..
//  bdet_Date d2(1776, dayOfYear);
//                          assert(1776 == d2.year());
//                          assert(   7 == d2.month());
//                          assert(   4 == d2.day());
//..
// Now, add six days to the value of 'd2'.
//..
//  d2 += 6;                assert(1776 == d2.year());
//                          assert(   7 == d2.month());
//                          assert(  10 == d2.day());
//..
// Next subtract 'd1' from 'd2', storing the difference (in days) in
// 'daysDiff':
//..
//  int daysDiff = d2 - d1; assert(   6 == daysDiff);
//..
// Finally, stream the value of 'd2' to 'stdout':
//..
//  bsl::cout << d2 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  10JUL1776
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEIMP_DATEUTIL
#include <bdeimp_dateutil.h>
#endif

#ifndef INCLUDED_BDET_DAYOFWEEK
#include <bdet_dayofweek.h>
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

                        // ===============
                        // class bdet_Date
                        // ===============

class bdet_Date {
    // This class implements a fully value-semantic date type.  Each object of
    // this augmented, complex-constrained attribute class *always* represents
    // a *valid* date according to the (Gregorian) Unix date convention.  The
    // valid range of date values (expressed as "year/month/day") is from
    // "1/1/1" to "9999/12/31".  (Note that the interface of this class
    // similarly supports 'bdet_Date' values expressed as "year/dayOfYear").
    //
    // Operations on dates that remain within this range will result in valid
    // dates, and therefore defined behavior.  Attempting to set or modify a
    // date to a value outside this range -- either directly or indirectly --
    // will result in undefined behavior.  A static (class) method:
    //..
    //  bool isValid(int year, int month, int day);
    //..
    // is provided to determine (e.g., prior to construction) whether a
    // sequence of three separate integer arguments 'year', 'month', and 'day'
    // represents a valid 'bdet_Date' value.
    //
    // There are also two forms of the manipulator used to set the value of an
    // existing date expressed in terms of "year/month/day":
    //..
    //  void setYearMonthDay(int year, int month, int day);
    //  int setYearMonthDayIfValid(int year, int month, int day);
    //..
    // The first is more efficient, but should be used only when the client
    // is "sure" that the supplied year, month, and day values represent
    // a valid date value for a 'bdet_Date', otherwise the behavior is
    // undefined.  When the validity of the date value is not known, always
    // use the second form, which sets the date object to the indicated value
    // and returns 0 *if* *valid*, and otherwise returns a non-zero status
    // with no effect.  Note that use of the latter function without checking
    // status can lead to a "double fault", which cannot be detected by a
    // robust implementation when built in a defensive (e.g., "DEBUG" or
    // "SAFE") build mode.
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  A precise
    // operational definition of when two objects have the same value can be
    // found in the description of 'operator==' for the class.  This class is
    // *exception* *neutral* with no guarantee of rollback: If an exception is
    // thrown during the invocation of a method on a pre-existing object, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // DATA
    union {
        int  d_date;                // absolute (serial) date (as 'int')
        char d_bytes[sizeof(int)];  // provides access to individual bytes
    };

    // FRIENDS
    friend bdet_Date operator+(const bdet_Date&, int);
    friend bdet_Date operator+(int, const bdet_Date&);
    friend bdet_Date operator-(const bdet_Date&, int);
    friend int  operator- (const bdet_Date&, const bdet_Date&);
    friend bool operator==(const bdet_Date&, const bdet_Date&);
    friend bool operator!=(const bdet_Date&, const bdet_Date&);
    friend bool operator< (const bdet_Date&, const bdet_Date&);
    friend bool operator<=(const bdet_Date&, const bdet_Date&);
    friend bool operator>=(const bdet_Date&, const bdet_Date&);
    friend bool operator> (const bdet_Date&, const bdet_Date&);
    friend bsl::ostream& operator<<(bsl::ostream&, const bdet_Date&);

  private:
    // PRIVATE CREATORS
    explicit bdet_Date(int serialDate);
        // Create a date initialized with the value indicated by the specified
        // 'serialDate'.  The behavior is undefined unless 'serialDate'
        // represents a valid 'bdet_Date' value.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdet_Date, bslalg_TypeTraitBitwiseCopyable);

    // CLASS METHODS
    static bool isValid(int year, int month, int day);
        // Return 'true' if the specified 'year', 'month', and 'day' represent
        // a valid value for a 'bdet_Date' object, and 'false' otherwise.
        // 'year', 'month', and 'day' represent a valid 'bdet_Date' value if
        // they correspond to a valid date in history as defined by the Unix
        // date convention, which includes the change in leap-year calculations
        // for century years that are not divisible by 400, following the
        // eleven-day "gap" between the valid date representations "1752/09/03"
        // and "1752/09/13".
        //
        // Note that 'year', 'month', and 'day' do *not* represent a valid
        // 'bdet_date' value unless '1 <= year <= 9999', '1 <= month <= 12',
        // and '1 <= day <= 31'.  Additionally, when 'month' is 4, 6, 9, or 11,
        // the representation is not valid unless '1 <= day <= 30', and, when
        // 'month' is 2, unless '1 <= day <= 29'.  Also, when 'month' is 2, if
        // 'year' is not divisible by 4, or 'year > 1752' and year is
        // divisible by 100, but not by 400, then the representation is not
        // valid unless '1 <= day <= 28'.  Finally, as a one-time special case,
        // if 'year' is 1752 and 'month' is 9, then the representation is not
        // valid unless '1 <= day <= 2' or '14 <= day <= 30'.

    static bool isValid(int year, int dayOfYear);
        // Return 'true' if the specified 'year' and 'dayOfYear' represent a
        // a valid value for a 'bdet_date' object, and 'false' otherwise.
        // 'year' and 'dayOfYear' represent a valid 'bdet_Date' value if they
        // correspond to a valid date in history as defined by the Unix date
        // convention, which includes the change in leap-year calculations
        // for century years that are not divisible by 400, following the
        // eleven-day "gap" between the valid date representations
        // "1752/09/03" and "1752/09/13".
        //
        // Note that 'year' and 'dayOfyear' does *not* represent a valid
        // 'bdet_date' value unless '1 <= year <= 9999', and
        // '1 <= dayOfYear <= 366'.  Also, if the year is not divisible by 4,
        // or 'year > 1752' and year is divisible by 100, but not by 400, then
        // the representation is not valid unless '1 <= dayOfyear <= 365'.
        // Finally, as a one-time special case, if 'year' is 1752, then the
        // representation is not valid unless '1 <= dayOfYear <= 355'.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    bdet_Date();
        // Create a 'bdet_Date' object having the earliest supported valid date
        // value -- i.e., "year/month/day" is "0001/01/01".

    bdet_Date(int year, int month, int day);
        // Create a 'bdet_Date' object having the (valid) value represented by
        // the specified 'year', 'month', and 'day'.  The behavior is undefined
        // unless 'year', 'month', and 'day' represent a valid 'bdet_Date'
        // value (see 'isValid').  Note that failing to satisfy stated
        // pre-conditions invalidates *all* guarantees (e.g., object
        // invariants).

    bdet_Date(int year, int dayOfYear);
        // Create a 'bdet_Date' object having the (valid) value represented by
        // the specified 'year' and 'dayOfYear'.  The behavior is undefined
        // unless 'year' and 'dayOfYear' represent a valid 'bdet_Date' value
        // (see 'isValid').  Note that failing to satisfy stated pre-conditions
        // invalidates *all* guarantees (e.g., object invariants).

    bdet_Date(const bdet_Date& original);
        // Create a 'bdet_Date' object having the value of the specified
        // 'original' date.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    ~bdet_Date();
        // Destroy this date object.  Note that this trivial destructor is, in
        // some build modes, generated by the compiler.
#endif

    // MANIPULATORS
    bdet_Date& operator=(const bdet_Date& rhs);
        // Assign to this date object the value of the specified 'rhs' date,
        // and return a reference to this modifiable object.

    bdet_Date& operator++();
        // Increase the value of this date by one day, and return a reference
        // to this modifiable date.  The behavior is undefined if the
        // initial value of this date was 'bdet_Date(9999, 12, 31)'.

    bdet_Date& operator--();
        // Decrease the value of this date by one day, and return a reference
        // to this modifiable date.  The behavior is undefined if the
        // initial value of this date was 'bdet_Date(1, 1, 1)'.

    bdet_Date& operator+=(int numDays);
        // Increase the value of this date by the specified 'numDays', and
        // return a reference to this modifiable date.  The behavior is
        // undefined unless the resulting date represents a valid 'bdet_Date'
        // value (see 'isValid').  Note that 'numDays' may be negative.

    bdet_Date& operator-=(int numDays);
        // Decrease the value of this date by the specified 'numDays', and
        // return a reference to this modifiable date.  The behavior is
        // undefined unless the resulting date represents a valid 'bdet_Date'
        // value (see 'isValid').  Note that 'numDays' may be negative.

    int addDaysIfValid(int numDays);
        // Add the specified (signed) 'numDays' to the value of this date if
        // the resulting date represents a valid 'bdet_Date' value (see
        // 'isValid').  Return 0 on success, and a non-zero value (with no
        // effect) otherwise.  Note that 'numDays' maybe negative.  Also note
        // that 'addDaysIfValid(0)' should always return success.

    void setYearDay(int year, int dayOfYear);
        // Set the value of this date to the specified 'year' and 'dayOfYear'.
        // The behavior is undefined unless 'year' and 'dayOfYear' represent a
        // valid 'bdet_Date' value (see 'isValid').

    int setYearDayIfValid(int year, int dayOfYear);
        // Set the value of this date to the specified 'year' and 'dayOfYear'
        // if they would comprise a valid 'bdet_Date' value.  Return 0 on
        // success, and a non-zero value (with no effect) otherwise.

    void setYearMonthDay(int year, int month, int day);
        // Set the value of this date to the specified 'year', 'month', and
        // 'day'.  The behavior is undefined unless 'year', 'month', and 'day'
        // represent a valid 'bdet_Date' value (see 'isValid').

    int setYearMonthDayIfValid(int year, int month, int day);
        // Set the value of this date to the specified 'year', 'month', and
        // 'day' if they would comprise a valid 'bdet_Date' value.  Return 0 on
        // success, and a non-zero value (with no effect) otherwise.

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
    void getYearDay(int *year, int *dayOfYear) const;
        // Load, into the specified 'year' and 'dayOfYear', the individual
        // field values that represent the overall value of this date.

    void getYearMonthDay(int *year, int *month, int *day) const;
        // Load, into the specified 'year', 'month', and 'day', the individual
        // field values that represent the overall value of this date.

    int day() const;
        // Return the day of the month in the range '[ 1 .. 31 ]' of this date.

    int month() const;
        // Return the month of the year in the range '[ 1 .. 12 ]' of this
        // date.

    int year() const;
        // Return the year in the range '[ 1 .. 9999 ]' of this date.

    int dayOfYear() const;
        // Return the day of the year in the range '[ 1 .. 366 ]' of this date.

    bdet_DayOfWeek::Day dayOfWeek() const;
        // Return the day of week in the range '[ BDET_SUN .. BDET_SAT ]' of
        // this date.

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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to the modifiable
        // 'stream'.  If 'stream' is initially invalid, this operation has no
        // effect.  If 'version' is not supported, 'stream' is invalidated.
        // Note that in no event is 'version' written to 'stream'.  (See the
        // 'bdex' package-level documentation for more information on 'bdex'
        // streaming of value-semantic types and containers.)

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: Use 'maxSupportedBdexVersion' instead.

    int validateAndSetYearDay(int year, int dayOfYear);
        // Set the value of this date to the specified 'year' and 'dayOfYear'
        // if they would comprise a valid 'bdet_Date' value.  Return 0 on
        // success, and a non-zero value (with no effect) otherwise.
        //
        // DEPRECATED: Use 'setYearDayIfValid' instead.

    int validateAndSetYearMonthDay(int year, int month, int day);
        // Set the value of this date to the specified 'year', 'month', and
        // 'day' if they would comprise a valid 'bdet_Date' value.  Return 0 on
        // success, and a non-zero value (with no effect) otherwise.
        //
        // DEPRECATED: Use 'setYearMonthDayIfValid' instead.

    bsl::ostream& streamOut(bsl::ostream& stream) const;
        // Format this date to the specified output 'stream' and return a
        // reference to the modifiable 'stream'.
        //
        // DEPRECATED: Use 'print' instead.

#endif

};

// FREE OPERATORS
bool operator==(const bdet_Date& lhs, const bdet_Date& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' dates have the same
    // value, and 'false' otherwise.  Two dates have the same value if each
    // of the corresponding 'year', 'month', and 'day' attributes respectively
    // have the same value.

bool operator!=(const bdet_Date& lhs, const bdet_Date& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' dates do not have the
    // same value, and 'false' otherwise.  Two dates do not have the same
    // value if any of their corresponding 'year', 'month', and 'day'
    // attributes do not have respectively the same value.

bsl::ostream& operator<<(bsl::ostream& stream, const bdet_Date& date);
    // Write the value of the specified 'date' object to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

bool operator<(const bdet_Date& lhs, const bdet_Date& rhs);
    // Return 'true' if the specified 'lhs' date value is less than the
    // specified 'rhs' date value, and 'false' otherwise.

bool operator<=(const bdet_Date& lhs, const bdet_Date& rhs);
    // Return 'true' if the specified 'lhs' date value is less than or equal to
    // the specified 'rhs' date value, and 'false' otherwise.

bool operator>(const bdet_Date& lhs, const bdet_Date& rhs);
    // Return 'true' if the specified 'lhs' date value is greater than the
    // specified 'rhs' date value, and 'false' otherwise.

bool operator>=(const bdet_Date& lhs, const bdet_Date& rhs);
    // Return 'true' if the specified 'lhs' date value is greater than or equal
    // to the specified 'rhs' date value, and 'false' otherwise.

bdet_Date operator++(bdet_Date& date, int);
    // Increase the value of the specified 'date' by one day, and return the
    // initial value of 'date'.  The behavior is undefined if the initial value
    // of 'date' is 'bdet_Date(9999, 12, 31)'.

bdet_Date operator--(bdet_Date& date, int);
    // Decrease the value of the specified 'date' by one day, and return the
    // initial value of 'date'.  The behavior is undefined if the initial value
    // of 'date' is 'bdet_Date(1, 1, 1)'.

bdet_Date operator+(const bdet_Date& date, int numDays);
bdet_Date operator+(int numDays, const bdet_Date& date);
    // Return the date value that results from adding the specified (signed)
    // 'numDays' to the specified 'date'.  The behavior is undefined unless the
    // resulting date falls within the range '[ 1/1/1 .. 9999/12/31 ]'.  Note
    // that 'numDays' may be negative.

bdet_Date operator-(const bdet_Date& date, int numDays);
    // Return the date value that results from subtracting the specified
    // (signed) 'numDays' to the specified 'date'.  The behavior is undefined
    // unless the resulting date falls within the range
    // '[ 1/1/1 .. 9999/12/31 ]'.  Note that 'numDays' may be negative.

int operator-(const bdet_Date& lhs, const bdet_Date& rhs);
    // Return the number of days between the specified 'lhs' and 'rhs' dates.
    // Note that if 'lhs < rhs' the result will be negative.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ---------------
                        // class bdet_Date
                        // ---------------

// CLASS METHODS
inline
bool bdet_Date::isValid(int year, int month, int day)
{
    return bdeimp_DateUtil::isValidCalendarDate(year, month, day);
}

inline
bool bdet_Date::isValid(int year, int dayOfYear)
{
    return bdeimp_DateUtil::isValidYearDayDate(year, dayOfYear);
}

inline
int bdet_Date::maxSupportedBdexVersion()
{
    return 1;
}

// PRIVATE CREATORS
inline
bdet_Date::bdet_Date(int serialDate)
: d_date(serialDate)
{
    BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidSerialDate(d_date));
}

// CREATORS
inline
bdet_Date::bdet_Date()
: d_date(1)
{
}

inline
bdet_Date::bdet_Date(int year, int month, int day)
: d_date(bdeimp_DateUtil::ymd2serial(year, month, day))
{
    BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidSerialDate(d_date));
}

inline
bdet_Date::bdet_Date(int year, int dayOfYear)
: d_date(bdeimp_DateUtil::yd2serial(year, dayOfYear))
{
    BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidSerialDate(d_date));
}

inline
bdet_Date::bdet_Date(const bdet_Date& original)
: d_date(original.d_date)
{
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)

inline
bdet_Date::~bdet_Date()
{
    BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidSerialDate(d_date));
}

#endif

// MANIPULATORS
inline
bdet_Date& bdet_Date::operator=(const bdet_Date& rhs)
{
    d_date = rhs.d_date;
    return *this;
}

inline
void bdet_Date::setYearDay(int year, int dayOfYear)
{
    BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidYearDayDate(year, dayOfYear));

    d_date = bdeimp_DateUtil::yd2serial(year, dayOfYear);
}

inline
int bdet_Date::setYearDayIfValid(int year, int dayOfYear)
{
    enum { BDET_SUCCESS = 0, BDET_FAILURE = -1 };

    if (bdeimp_DateUtil::isValidYearDayDate(year, dayOfYear)) {
        setYearDay(year, dayOfYear);
        return BDET_SUCCESS;                                          // RETURN
    }
    return BDET_FAILURE;
}

inline
void bdet_Date::setYearMonthDay(int year, int month, int day)
{
    BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidCalendarDate(year, month, day));

    d_date = bdeimp_DateUtil::ymd2serial(year, month, day);
}

inline
int bdet_Date::setYearMonthDayIfValid(int year, int month, int day)
{
    enum { BDET_SUCCESS = 0, BDET_FAILURE = -1 };

    if (bdeimp_DateUtil::isValidCalendarDate(year, month, day)) {
        setYearMonthDay(year, month, day);
        return BDET_SUCCESS;                                          // RETURN
    }
    return BDET_FAILURE;
}

template <class STREAM>
STREAM& bdet_Date::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {    // switch on the schema version (starting with 1)
          case 1: {
            stream.getInt24(d_date);
            if (!stream) {
                d_date = 2;   // *might* be corrupted; value for testing
                return stream;                                        // RETURN
            }
            if (!bdeimp_DateUtil::isValidSerialDate(d_date)) {
                d_date = 3;   // arbitrary valid date for testing
                stream.invalidate();
                return stream;                                        // RETURN
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
bdet_Date& bdet_Date::operator+=(int numDays)
{
    d_date += numDays;
    BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidSerialDate(d_date));
    return *this;
}

inline
bdet_Date& bdet_Date::operator-=(int numDays)
{
    d_date -= numDays;
    BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidSerialDate(d_date));
    return *this;
}

inline
bdet_Date& bdet_Date::operator++()
{
    ++d_date;
    BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidSerialDate(d_date));
    return *this;
}

inline
bdet_Date& bdet_Date::operator--()
{
    --d_date;
    BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidSerialDate(d_date));
    return *this;
}

// ACCESSORS
inline
void bdet_Date::getYearDay(int *year, int *dayOfYear) const
{
    BSLS_ASSERT_SAFE(year);
    BSLS_ASSERT_SAFE(dayOfYear);

    bdeimp_DateUtil::serial2yd(year, dayOfYear, d_date);
}

inline
void bdet_Date::getYearMonthDay(int *year, int *month, int *day) const
{
    BSLS_ASSERT_SAFE(year);
    BSLS_ASSERT_SAFE(month);
    BSLS_ASSERT_SAFE(day);

    bdeimp_DateUtil::serial2ymd(year, month, day, d_date);
}

inline
int bdet_Date::year() const
{
    return bdeimp_DateUtil::serial2year(d_date);
}

inline
int bdet_Date::month() const
{
    return bdeimp_DateUtil::serial2month(d_date);
}

inline
int bdet_Date::day() const
{
    return bdeimp_DateUtil::serial2day(d_date);
}

inline
int bdet_Date::dayOfYear() const
{
    return bdeimp_DateUtil::serial2dayOfYear(d_date);
}

inline
bdet_DayOfWeek::Day bdet_Date::dayOfWeek() const
{
    return bdet_DayOfWeek::Day(bdeimp_DateUtil::serial2weekday(d_date));
}

template <class STREAM>
STREAM& bdet_Date::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) {
          case 1: {
            BSLS_ASSERT_OPT(bdeimp_DateUtil::isValidSerialDate(d_date));
            stream.putInt24(d_date);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

inline
int bdet_Date::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

inline
int bdet_Date::validateAndSetYearDay(int year, int dayOfYear)
{
    return setYearDayIfValid(year, dayOfYear);
}

inline
int bdet_Date::validateAndSetYearMonthDay(int year, int month, int day)
{
    return setYearMonthDayIfValid(year, month, day);
}

inline
bsl::ostream& bdet_Date::streamOut(bsl::ostream& stream) const
{
    return stream << *this;
}

#endif

// FREE OPERATORS
inline
bool operator==(const bdet_Date& lhs, const bdet_Date& rhs)
{
    return lhs.d_date == rhs.d_date;
}

inline
bool operator!=(const bdet_Date& lhs, const bdet_Date& rhs)
{
    return lhs.d_date != rhs.d_date;
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdet_Date& date)
{
    return date.print(stream, 0, -1);
}

inline
bool operator<(const bdet_Date& lhs, const bdet_Date& rhs)
{
    return lhs.d_date < rhs.d_date;
}

inline
bool operator<=(const bdet_Date& lhs, const bdet_Date& rhs)
{
    return lhs.d_date <= rhs.d_date;
}

inline
bool operator>(const bdet_Date& lhs, const bdet_Date& rhs)
{
    return lhs.d_date > rhs.d_date;
}

inline
bool operator>=(const bdet_Date& lhs, const bdet_Date& rhs)
{
    return lhs.d_date >= rhs.d_date;
}

inline
bdet_Date operator++(bdet_Date& date, int)
{
    bdet_Date tmp(date);
    ++date;
    return tmp;
}

inline
bdet_Date operator--(bdet_Date& date, int)
{
    bdet_Date tmp(date);
    --date;
    return tmp;
}

inline
bdet_Date operator+(const bdet_Date& date, int numDays)
{
    return bdet_Date(date.d_date + numDays);
}

inline
bdet_Date operator+(int numDays, const bdet_Date& date)
{
    return bdet_Date(date.d_date + numDays);
}

inline
bdet_Date operator-(const bdet_Date& date, int numDays)
{
    return bdet_Date(date.d_date - numDays);
}

inline
int operator-(const bdet_Date& lhs, const bdet_Date& rhs)
{
    return lhs.d_date - rhs.d_date;
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
