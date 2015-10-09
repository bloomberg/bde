// bdlt_serialdateimputil.h                                           -*-C++-*-
#ifndef INCLUDED_BDLT_SERIALDATEIMPUTIL
#define INCLUDED_BDLT_SERIALDATEIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide low-level support functions for date-value manipulation.
//
//@CLASSES:
//  bdlt::SerialDateImpUtil: low-level date-related stateless functions
//
//@SEE_ALSO: bdlt_date
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bdlt::SerialDateImpUtil', that defines a suite of low-level, date-related
// functions, which can be used to validate, manipulate, and convert among
// values in three different formats:
//..
//  YMD: year/month/day date
//   YD: year/day-of-year date
//    S: serial date
//..
// The supplied functionality can also be used (e.g.) for determining leap
// years, finding the last day in a given month, and for determining the day of
// the week for a given date.  Note that in this component a "date" is
// understood to represent a valid day in the (YMD) range '0001/01/01' to
// '9999/12/31' according to the *proleptic* *Gregorian* *calendar*:
//..
//  http://en.wikipedia.org/wiki/Proleptic_Gregorian_calendar
//..
//
///Representations, Valid Dates, and Leap Years
///--------------------------------------------
// The "Calendar Date", or "year-month-day (ymd)", is the canonical
// representation and is denoted as "YYYY/MM/DD", with valid years being in the
// range '[1 .. 9999]'.  Within a valid year, valid months and valid days are
// confined to the respective ranges '[1 .. 12]' and '[1 .. 31]'.  Valid dates
// in this representation range from '0001/01/01' to '9999/12/31', governed by
// the proleptic Gregorian calendar.  Specifically, within the 4th, 6th, 9th,
// and 11th months (respectively, April, June, September, and November) of any
// year, valid days are in the range '[1 .. 30]'.  Valid days for all other
// months of any year, with exception of the 2nd month (February), are in the
// range '[1 .. 31]'.
//
// In a *leap* *year*, February has 29 days instead of the usual 28.  Thus, the
// range of valid days for February in a leap year is '[1 .. 29]'; otherwise,
// the range of valid days for February is '[1 .. 28]'.  The proleptic
// Gregorian calendar retroactively applies the leap year rules instituted by
// the Gregorian Reformation to all years.  In particular, a year in the range
// '[1 .. 9999]' is a leap year if it is divisible by 4, but *not* divisible by
// 100, *unless* it is *also* divisible by 400.  (Expressed conversely, all
// years *not* divisible by 4 are non-leap years, as are all century years
// *not* divisible by 400 (e.g., 1900).
//
// The "Day-Of-Year Date", or "year-day (yd)" representation, denoted by
// "YYYY/DDD", represents dates by their year (again, in the range
// '[1 .. 9999]') and the day of year, in the range '[1 .. 366]'.  Valid date
// values in this representation range from '0001/001' to '9999/365', with a
// day-of-year value of 366 permitted for leap years only.
//
// The "Serial Date" representation depicts dates as consecutive integers
// beginning with 1 (representing '0001/01/01').  In this representation, valid
// date values are in the range '[1 .. 3652059]', with 3652059 representing
// '9999/12/31'.
//
///Caching
///-------
// To achieve maximal runtime performance, several of the functions in this
// component reserve the right to be implemented using statically cached (i.e.,
// tabulated, pre-calculated) values (which is inherently thread-safe).  For
// all functions where a cache may be used, 'bdlt::SerialDateImpUtil' also
// explicitly provides a 'NoCache' version (e.g., 'ymdToSerialNoCache') that is
// guaranteed NOT to use a cache.  Although the "normal" (potentially cached)
// functions typically gain huge performance advantages, the 'NoCache' versions
// may conceivably be preferred by the performance-minded user who is
// *reasonably* *certain* that the vast majority of date values of interest
// will miss the cache (thus incurring a small, but unnecessary overhead for
// the cache-hit tests).  Note, however, that the 'NoCache' function variants
// are provided primarily for testing and for generating the cache in the first
// place (see this component's test driver).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Use as a General Purpose Utility
///- - - - - - - - - - - - - - - - - - - - - -
// The primary purpose of this component is to support the implementation of a
// general-purpose, value-semantic (vocabulary) "Date" type.  However, it also
// provides many low-level utility functions suitable for direct use by other
// clients.  In this example we employ several of the functions from this
// component to ask questions about particular dates in one of the three
// supported formats.
//
// First, what day of the week was January 3, 2010?
//..
//  assert(2 == bdlt::SerialDateImpUtil::ymdToDayOfWeek(2010, 3, 1));
//                                                           // 2 means Monday.
//..
// Then, was the year 2000 a leap year?
//..
//  assert(true == bdlt::SerialDateImpUtil::isLeapYear(2000));
//                                                           // Yes, it was.
//..
// Next, was February 29, 1900 a valid date in history?
//..
//  assert(false == bdlt::SerialDateImpUtil::isValidYearMonthDay(1900, 2, 29));
//                                                           // No, it was not.
//..
// Then, what was the last day of February in 1600?
//..
//  assert(29 == bdlt::SerialDateImpUtil::lastDayOfMonth(1600, 2));
//                                                           // The 29th.
//..
// Next, how many leap years occurred from 1959 to 2012, inclusive?
//..
//  assert(14 == bdlt::SerialDateImpUtil::numLeapYears(1959, 2012));
//                                                           // There were 14.
//..
// Now, on what day of the year will February 29, 2020 fall?
//..
//  assert(60 == bdlt::SerialDateImpUtil::ymdToDayOfYear(2020, 2, 29));
//                                                           // The 60th one.
//..
// Finally, in what month did the 120th day of 2011 fall?
//..
//  assert(4 == bdlt::SerialDateImpUtil::ydToMonth(2011, 120));
//                                                           // 4 means April.
//..
//
///Example 2: Implement a Value-Semantic Date Type
///- - - - - - - - - - - - - - - - - - - - - - - -
// Using the functions supplied in this component, we can easily implement a
// C++ class that represents abstract (*mathematical*) date values and performs
// common operations on them.  The internal representation could be any of the
// three supported by this component.  In this example, we choose to represent
// the date value internally as a "serial date".
//
// First, we define a partial interface of our date class, 'MyDate', omitting
// many methods, free operators, and 'friend' declarations that do not
// contribute substantively to illustrating use of this component:
//..
//  class MyDate {
//      // This class represents a valid date, in the proleptic Gregorian
//      // calendar, in the range '[0001/01/01 .. 9999/12/31]'.
//
//      // DATA
//      int d_serialDate;  // 1 = 0001/01/01, 2 = 0001/01/02, etc.
//
//      // FRIENDS
//      friend bool operator==(const MyDate&, const MyDate&);
//      // ...
//
//    private:
//      // PRIVATE CREATORS
//      explicit MyDate(int serialDate);
//          // Create a 'MyDate' object initialized with the value indicated by
//          // the specified 'serialDate'.  The behavior is undefined unless
//          // 'serialDate' represents a valid 'MyDate' value.
//
//    public:
//      // CLASS METHODS
//      static bool isValid(int year, int month, int day);
//          // Return 'true' if the specified 'year', 'month', and 'day'
//          // represent a valid value for a 'MyDate' object, and 'false'
//          // otherwise.
//
//      // CREATORS
//      MyDate();
//          // Create a 'MyDate' object having the earliest supported valid
//          // date value, i.e., "0001/01/01".
//
//      MyDate(int year, int month, int day);
//          // Create a 'MyDate' object having the value represented by the
//          // specified 'year', 'month', and 'day'.  The behavior is undefined
//          // unless 'isValid(year, month, day)' returns 'true'.
//
//      // ...
//
//      // MANIPULATORS
//
//      // ...
//
//      void setYearMonthDay(int year, int month, int day);
//          // Set this 'MyDate' object to have the value represented by the
//          // specified 'year', 'month', and 'day'.  The behavior is undefined
//          // unless 'isValid(year, month, day)' returns 'true'.
//
//      // ACCESSORS
//      void getYearMonthDay(int *year, int *month, int *day) const;
//          // Load, into the specified 'year', 'month', and 'day', the
//          // individual attribute values of this 'MyDate' object.
//
//      int day() const;
//          // Return the day of the month in the range '[1 .. 31]' of this
//          // 'MyDate' object.
//
//      int month() const;
//          // Return the month of the year in the range '[1 .. 12]' of this
//          // 'MyDate' object.
//
//      int year() const;
//          // Return the year in the range '[1 .. 9999]' of this 'MyDate'
//          // object.
//
//      // ...
//  };
//
//  // FREE OPERATORS
//  bool operator==(const MyDate& lhs, const MyDate& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' 'MyDate' objects have
//      // the same value, and 'false' otherwise.  Two dates have the same
//      // value if each of the corresponding 'year', 'month', and 'day'
//      // attributes respectively have the same value.
//
//  // ...
//..
// Then, we provide an implementation of the 'MyDate' methods and associated
// free operators declared above, using 'bsls_assert' to identify preconditions
// and invariants where appropriate.  Note the use of various
// 'bdlt::SerialDateImpUtil' functions in the code:
//..
//  // PRIVATE CREATORS
//  inline
//  MyDate::MyDate(int serialDate)
//  : d_serialDate(serialDate)
//  {
//      BSLS_ASSERT_SAFE(bdlt::SerialDateImpUtil::isValidSerial(d_serialDate));
//  }
//
//  // CLASS METHODS
//  inline
//  bool MyDate::isValid(int year, int month, int day)
//  {
//      return bdlt::SerialDateImpUtil::isValidYearMonthDay(year, month, day);
//  }
//
//  // CREATORS
//  inline
//  MyDate::MyDate()
//  : d_serialDate(1)
//  {
//  }
//
//  inline
//  MyDate::MyDate(int year, int month, int day)
//  : d_serialDate(bdlt::SerialDateImpUtil::ymdToSerial(year, month, day))
//  {
//      BSLS_ASSERT_SAFE(isValid(year, month, day));
//  }
//
//  // ...
//
//  // MANIPULATORS
//
//  // ...
//
//  inline
//  void MyDate::setYearMonthDay(int year, int month, int day)
//  {
//      BSLS_ASSERT_SAFE(isValid(year, month, day));
//
//      d_serialDate = bdlt::SerialDateImpUtil::ymdToSerial(year, month, day);
//  }
//
//  // ACCESSORS
//  inline
//  void MyDate::getYearMonthDay(int *year, int *month, int *day) const
//  {
//      BSLS_ASSERT_SAFE(year);
//      BSLS_ASSERT_SAFE(month);
//      BSLS_ASSERT_SAFE(day);
//
//      bdlt::SerialDateImpUtil::serialToYmd(year, month, day, d_serialDate);
//  }
//
//  inline
//  int MyDate::day() const
//  {
//      return bdlt::SerialDateImpUtil::serialToDay(d_serialDate);
//  }
//
//  inline
//  int MyDate::month() const
//  {
//      return bdlt::SerialDateImpUtil::serialToMonth(d_serialDate);
//  }
//
//  inline
//  int MyDate::year() const
//  {
//      return bdlt::SerialDateImpUtil::serialToYear(d_serialDate);
//  }
//
//  // FREE OPERATORS
//  inline
//  bool operator==(const MyDate& lhs, const MyDate& rhs)
//  {
//      return lhs.d_serialDate == rhs.d_serialDate;
//  }
//..
// Next, we illustrate basic use of our 'MyDate' class, starting with the
// creation of a default object, 'd1':
//..
//  MyDate d1;                        assert(   1 == d1.year());
//                                    assert(   1 == d1.month());
//                                    assert(   1 == d1.day());
//..
// Now, we set 'd1' to July 4, 1776 via the 'setYearMonthDay' method, but we
// first verify that it is a valid date using 'isValid':
//..
//                                    assert(MyDate::isValid(1776, 7, 4));
//  d1.setYearMonthDay(1776, 7, 4);   assert(1776 == d1.year());
//                                    assert(   7 == d1.month());
//                                    assert(   4 == d1.day());
//..
// Finally, using the value constructor, we create 'd2' to have the same value
// as 'd1':
//..
//  MyDate d2(1776, 7, 4);            assert(1776 == d2.year());
//                                    assert(   7 == d2.month());
//                                    assert(   4 == d2.day());
//                                    assert(  d1 == d2);
//..
// Note that equality comparison of 'MyDate' objects is very efficient, being
// comprised of a comparison of two 'int' values.  Similarly, the 'MyDate'
// methods and free operators (not shown) that add a (signed) number of days
// to a date are also very efficient.  However, one of the trade-offs of
// storing a date internally as a serial value is that operations involving
// conversion among the serial value and one or more of the 'year', 'month',
// and 'day' attributes (e.g., 'setYearMonthDay', 'getYearMonthDay') entail
// considerably more computation.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bdlt {

                           // ========================
                           // struct SerialDateImpUtil
                           // ========================

struct SerialDateImpUtil {
    // This 'struct' provides a namespace for a suite of pure functions that
    // perform low-level operations on date values in a variety of formats:
    // year/month/day, year/day-of-year, and serial date.  Dates in the range
    // '[0001/01/01 .. 9999/12/31]' that are valid per the proleptic Gregorian
    // calendar are supported, with serial date 1 (3652059) corresponding to
    // '0001/01/01' ('9999/12/31').  Note that all of the functions, whether or
    // not implemented in terms of a static cache, are stateless, and, as such,
    // are inherently thread-safe.

  private:
    // PRIVATE TYPES
    struct YearMonthDay {
        short d_year;
        char  d_month;
        char  d_day;
    };

    enum {
        k_MAX_SERIAL_DATE = 3652059
    };

    // PRIVATE CLASS DATA
    static const int          s_firstCachedYear;
    static const int          s_lastCachedYear;
    static const int          s_firstCachedSerialDate;
    static const int          s_lastCachedSerialDate;
    static const int          s_cachedSerialDate[][13];
    static const YearMonthDay s_cachedYearMonthDay[];
    static const char         s_cachedDaysInMonth[][13];

  public:
    // CLASS METHODS
    static bool isLeapYear(int year);
        // Return 'true' if the specified 'year' is a leap year, and 'false'
        // otherwise.  The behavior is undefined unless '1 <= year <= 9999'.

    static int lastDayOfMonth(int year, int month);
        // Return the last day of the specified 'month' in the specified
        // 'year'.  The behavior is undefined unless '1 <= year <= 9999' and
        // '1 <= month <= 12'.  Note that the value returned will be in the
        // range '[28 .. 31]'.

    static int numLeapYears(int year1, int year2);
        // Return the number of leap years occurring between the specified
        // 'year1' and 'year2', inclusive.  The behavior is undefined unless
        // '1 <= year1 <= 9999', '1 <= year2 <= 9999', and 'year1 <= year2'.

                        // Is Valid Date

    static bool isValidSerial(int serialDay);
        // Return 'true' if the specified 'serialDay' represents a valid date
        // value, and 'false' otherwise.  Note that valid date values are (as
        // fully defined in the component-level documentation) in the range
        // '[1 .. 3652059]'.

    static bool isValidYearDay(int year, int dayOfYear);
        // Return 'true' if the specified 'year' and 'dayOfYear' represents a
        // valid date value, and 'false' otherwise.  Note that valid date
        // values are (as fully defined in the component-level documentation)
        // in the range '[0001/01 .. 9999/366]'.

    static bool isValidYearMonthDay(int year, int month, int day);
        // Return 'true' if the specified 'year', 'month', and 'day' represents
        // a valid date value, and 'false' otherwise.  Note that valid date
        // values are (as fully defined in the component-level documentation)
        // in the range '[0001/01/01 .. 9999/12/31]'.

    static bool isValidYearMonthDayNoCache(int year, int month, int day);
        // Return 'true' if the specified 'year', 'month', and 'day' represents
        // a valid date value,  and 'false' otherwise.  Note that valid date
        // values are (as fully defined in the component-level documentation)
        // in the range '[0001/01/01 .. 9999/12/31]'.  Also note that this
        // function is guaranteed not to use any date-cache optimizations.

                        // To Serial Date (s)

    static int ydToSerial(int year, int dayOfYear);
        // Return the serial date representation of the date value indicated
        // by the specified 'year' and 'dayOfYear'.  The behavior is undefined
        // unless 'isValidYearDay(year, dayOfYear)' returns 'true'.

    static int ymdToSerial(int year, int month, int day);
        // Return the serial date representation of the date value indicated by
        // the specified 'year', 'month', and 'day'.  The behavior is undefined
        // unless 'isValidYearMonthDay(year, month, day)' returns 'true'.

    static int ymdToSerialNoCache(int year, int month, int day);
        // Return the serial date representation of the date value indicated
        // by the specified 'year', 'month', and 'day'.  The behavior is
        // undefined unless 'isValidYearMonthDay(year, month, day)' returns
        // 'true'.  Note that this function is guaranteed not to use any
        // date-cache optimizations.

                        // To Day-Of-Year Date (yd)

    static int serialToDayOfYear(int serialDay);
        // Return the day of the year of the date value indicated by the
        // specified 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.

    static void serialToYd(int *year, int *dayOfYear, int serialDay);
        // Load, into the specified 'year' and 'dayOfYear', the year-day
        // representation of the date value indicated by the specified
        // 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.

    static int ymdToDayOfYear(int year, int month, int day);
        // Return the day of the year of the date value indicated by the
        // specified 'year', 'month', and 'day'.  The behavior is undefined
        // unless 'isValidYearMonthDay(year, month, day)' returns 'true'.

                        // To Calendar Date (ymd)

    static int serialToDay(int serialDay);
        // Return the day (of the month) of the date value indicated by the
        // specified 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.

    static int serialToDayNoCache(int serialDay);
        // Return the day (of the month) of the date value indicated by the
        // specified 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.  Note that this function
        // is guaranteed not to use any date-cache optimizations.

    static int serialToMonth(int serialDay);
        // Return the month of the date value indicated by the specified
        // 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.

    static int serialToMonthNoCache(int serialDay);
        // Return the month of the date value indicated by the specified
        // 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.  Note that this function
        // is guaranteed not to use any date-cache optimizations.

    static int serialToYear(int serialDay);
        // Return the year of the date value indicated by the specified
        // 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.

    static int serialToYearNoCache(int serialDay);
        // Return the year of the date value indicated by the specified
        // 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.  Note that this function
        // is guaranteed not to use any date-cache optimizations.

    static void serialToYmd(int *year, int *month, int *day, int serialDay);
        // Load, into the specified 'year', 'month', and 'day', the date value
        // indicated by the specified 'serialDay'.  The behavior is undefined
        // unless 'isValidSerial(serialDay)' returns 'true'.

    static void serialToYmdNoCache(int *year,
                                   int *month,
                                   int *day,
                                   int  serialDay);
        // Load, into the specified 'year', 'month', and 'day', the date value
        // indicated by the specified 'serialDay'.  The behavior is undefined
        // unless 'isValidSerial(serialDay)' returns 'true'.  Note that this
        // function is guaranteed not to use any date-cache-optimizations.

    static int ydToDay(int year, int dayOfYear);
        // Return the day (of the month) of the date value indicated by the
        // specified 'year' and 'dayOfYear'.  The behavior is undefined unless
        // 'isValidYearDay(year, dayOfYear)' returns 'true'.

    static void ydToMd(int *month, int *day, int year, int dayOfYear);
        // Load, into the specified 'month' and 'day', the (partial) calendar
        // date representation of the date value indicated by the specified
        // 'year' and 'dayOfYear'.  The behavior is undefined unless
        // 'isValidYearDay(year, dayOfYear)' returns 'true'.

    static int ydToMonth(int year, int dayOfYear);
        // Return the month of the date value indicated by the specified 'year'
        // and 'dayOfYear'.  The behavior is undefined unless
        // 'isValidYearDay(year, dayOfYear)' returns 'true'.

                        // To Day of Week '[SUN = 1, MON .. SAT]'

    static int serialToDayOfWeek(int serialDay);
        // Return, as an integer (with '1 = SUN', '2 = MON', ..., '7 = SAT'),
        // the day (of the week) of the date value indicated by the specified
        // 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.

    static int ydToDayOfWeek(int year, int dayOfYear);
        // Return, as an integer (with '1 = SUN', '2 = MON', ..., '7 = SAT'),
        // the day (of the week) of the date value indicated by the specified
        // 'year' and 'dayOfYear'.  The behavior is undefined unless
        // 'isValidYearDay(year, dayOfYear)' returns 'true'.

    static int ymdToDayOfWeek(int year, int month, int day);
        // Return, as an integer (with '1 = SUN', '2 = MON', ..., '7 = SAT'),
        // the day (of the week) of the date value indicated by the specified
        // 'year', 'month', and 'day'.  The behavior is undefined unless
        // 'isValidYearMonthDay(year, month, day)' returns 'true'.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                           // ------------------------
                           // struct SerialDateImpUtil
                           // ------------------------

// CLASS METHODS
inline
bool SerialDateImpUtil::isLeapYear(int year)
{
    BSLS_ASSERT_SAFE(1 <= year);
    BSLS_ASSERT_SAFE(     year <= 9999);

    // Note the relative probabilities, from most likely to least likely:
    //: o Is not a leap year.
    //: o Is not the turn of any century (e.g., is not 1900).
    //: o Is a multiple of 400 (e.g., is 2000).

    return 0 == year % 4 && (0 != year % 100 || 0 == year % 400);
}

                        // Is Valid Date

inline
bool SerialDateImpUtil::isValidSerial(int serialDay)
{
    return static_cast<unsigned>(serialDay) - 1 < k_MAX_SERIAL_DATE;
}

inline
bool SerialDateImpUtil::isValidYearMonthDay(int year, int month, int day)
{
    if (s_firstCachedYear <= year && year <= s_lastCachedYear) {
        // Check 'month' and 'day'; the cache cannot catch out-of-range issues.

        if (month < 1 || month > 12 || day < 1) {
            return false;                                             // RETURN
        }

        return day <= s_cachedDaysInMonth[year - s_firstCachedYear][month];
                                                                      // RETURN
    }
    else {
        return isValidYearMonthDayNoCache(year, month, day);          // RETURN
    }
}

                        // To Day-Of-Year Date (yd)

inline
int SerialDateImpUtil::serialToDayOfYear(int serialDay)
{
    BSLS_ASSERT_SAFE(isValidSerial(serialDay));

    int dayOfYear, year;
    serialToYd(&year, &dayOfYear, serialDay);
    return dayOfYear;
}

                        // To Calendar Date (ymd)

inline
int SerialDateImpUtil::serialToDayNoCache(int serialDay)
{
    BSLS_ASSERT_SAFE(isValidSerial(serialDay));

    int year, month, day;
    serialToYmdNoCache(&year, &month, &day, serialDay);
    return day;
}

inline
int SerialDateImpUtil::serialToMonthNoCache(int serialDay)
{
    BSLS_ASSERT_SAFE(isValidSerial(serialDay));

    int year, month, day;
    serialToYmdNoCache(&year, &month, &day, serialDay);
    return month;
}

inline
int SerialDateImpUtil::serialToYearNoCache(int serialDay)
{
    BSLS_ASSERT_SAFE(isValidSerial(serialDay));

    int year, dayOfYear;
    serialToYd(&year, &dayOfYear, serialDay);
    return year;
}

inline
void SerialDateImpUtil::serialToYmdNoCache(int *year,
                                           int *month,
                                           int *day,
                                           int  serialDay)
{
    BSLS_ASSERT_SAFE(year);
    BSLS_ASSERT_SAFE(month);
    BSLS_ASSERT_SAFE(day);
    BSLS_ASSERT_SAFE(isValidSerial(serialDay));

    int dayOfYear;
    serialToYd(year, &dayOfYear, serialDay);
    ydToMd(month, day, *year, dayOfYear);
}

inline
int SerialDateImpUtil::ydToDay(int year, int dayOfYear)
{
    BSLS_ASSERT_SAFE(isValidYearDay(year, dayOfYear));

    int month, day;
    ydToMd(&month, &day, year, dayOfYear);
    return day;
}

inline
int SerialDateImpUtil::ydToMonth(int year, int dayOfYear)
{
    BSLS_ASSERT_SAFE(isValidYearDay(year, dayOfYear));

    int month, day;
    ydToMd(&month, &day, year, dayOfYear);
    return month;
}

                        // To Day of Week '[SUN = 1, MON .. SAT]'

inline
int SerialDateImpUtil::serialToDayOfWeek(int serialDay)
{
    BSLS_ASSERT_SAFE(isValidSerial(serialDay));

    // 0001/01/01 was a Monday (MON == 2).

    return 1 + serialDay % 7;
}

inline
int SerialDateImpUtil::ydToDayOfWeek(int year, int dayOfYear)
{
    BSLS_ASSERT_SAFE(isValidYearDay(year, dayOfYear));

    return serialToDayOfWeek(ydToSerial(year, dayOfYear));
}

inline
int SerialDateImpUtil::ymdToDayOfWeek(int year, int month, int day)
{
    BSLS_ASSERT_SAFE(isValidYearMonthDay(year, month, day));

    return serialToDayOfWeek(ymdToSerial(year, month, day));
}

}  // close package namespace
}  // close enterprise namespace

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
