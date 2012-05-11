// bdeimp_prolepticdateutil.h                                         -*-C++-*-
#ifndef INCLUDED_BDEIMP_PROLEPTICDATEUTIL
#define INCLUDED_BDEIMP_PROLEPTICDATEUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide low-level functions for proleptic date-value manipulation.
//
//@CLASSES:
//   bdeimp_prolepticdateutil: suite of proleptic date-related procedures
//
//@SEE_ALSO: bdeimp_dateutil
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: !!** DO NOT USE.  This component is a work in progress and
// should not be relied on by application code.  Please take a look at
// 'bdeimp_dateutil' in the interim. **!!
//
// This component implements a utility class 'bdeimp_prolepticdateutil', that
// provides a suite of low-level, proleptic date-related procedures that can be
// used to validate, manipulate, and convert among values in two different
// formats:
//..
//  YMD: year/month/day
//    S: serialDate
//..
// Note that in this component, a "date" is understood to represent a valid day
// in the range '0001JAN01' to '9999DEC31', according to the standard UNIX date
// implementation.
//
///Representations, Valid Dates, and Leap Years
///--------------------------------------------
// The "Calendar Date", or "year-month-day (ymd)", is the primary
// representation and is denoted as "yyyy/mm/dd", with valid years being in the
// range '[ 1 .. 9999 ]'.  Within a valid year, valid months and valid days are
// confined to the respective ranges '[ 1 .. 12 ]' and '[ 1 .. 31 ]'.  Valid
// dates in this representation will range from '0001/01/01' to '9999/12/31',
// but are also governed by the standard proleptic calendar convention.
//
// The "Serial Date" representation depicts dates as consecutive integers,
// beginning with 1 (representing '0001JAN01').  In this representation, valid
// date value are in the range '[ 1 .. 3652059 ]'.
//
// In a leap year, February has 29 days instead of the usual 28.  A year is a
// leap year if it is divisible by 4, but not divisible by 100, unless it is
// also divisible by 400.
//
///Usage
///-----
// This component was created primarily to support the implementation of a
// general-purpose, value-semantic (vocabulary) "Date" type, but also provides
// many low-level utility functions suitable for direct use by other clients.
//
///As a General Purpose Utility
/// - - - - - - - - - - - - - -
// Many of the functions provided by this component can be used directly by
// clients that want to ask questions about a particular date in one of the
// three supported formats.
//
// Was the year 2000 a leap year?
//..
//  // Yes, it was.
//  assert(true == bdeimp_ProlepticDateUtil::isLeapYear(2000));
//..
// Was February 29, 1900, a valid date in history?
//..
//  assert(false == bdeimp_ProlepticDateUtil::isValidCalendarDate(1900,2,29));
//                                                           // No, it was not.
//..
///Implementing a Value-Semantic Date Type
///- - - - - - - - - - - - - - - - - - - -
// Using the functions supplied in this component, we can easily implement a
// C++ class that represents abstract (*mathematical*) date values and performs
// a few common operations on them.  The internal representation could be any
// of the three supported by this component; in this example, we will choose to
// represent the date value internally as a "serial date":
//..
//  class MyDate {
//      // This class represents a valid date in the range
//      // '[ 0001/01/01 .. 9999/12/31 ]'.
//
//      // DATA
//      int d_serialDate;  // 1 = 0001JAN01, 2 = 0001JAN02, ...
//
//      // FRIENDS
//      friend MyDate operator+(const MyDate&, int);
//      friend MyDate operator+(int, const MyDate&);
//      friend int  operator- (const MyDate&, const MyDate&);
//      friend bool operator==(const MyDate&, const MyDate&);
//
//    private:
//      // PRIVATE CREATORS
//      explicit MyDate(int serialDate);
//..
// Next we define the public interface of the class, with function-level
// documentation conspicuously omitted (note, however, that reference
// implementations with preconditions asserted will follow):
//..
//    public:
//      // TYPES
//      enum Day {
//          SUN = 1,
//          MON = 2,
//          TUE = 3,
//          WED = 4,
//          THU = 5,
//          FRI = 6,
//          SAT = 7
//      };
//
//      // CLASS METHODS
//      static bool isValid(int year, int month, int day);
//
//      // CREATORS
//      MyDate();
//      MyDate(const MyDate& original);
//      ~MyDate();
//
//      // MANIPULATORS
//      MyDate& operator=(const MyDate& rhs);
//      MyDate& operator++();
//      MyDate& operator--();
//      MyDate& operator+=(int numDays);
//      MyDate& operator-=(int numDays);
//      void setYearMonthDay(int year, int month, int day);
//      bool setYearMonthDayIfValid(int year, int month, int day);
//
//      // ACCESSORS
//      void getYearMonthDay(int *year, int *month, int *day) const;
//      int year() const;
//      int month() const;
//      int day() const;
//      bool isLeapYear() const;
//      bsl::ostream& print(bsl::ostream& stream) const;
//  };
//
//  // FREE OPERATORS
//  bool operator==(const MyDate& lhs, const MyDate& rhs);
//  bool operator!=(const MyDate& lhs, const MyDate& rhs);
//
//  bsl::ostream& operator<<(bsl::ostream& stream, const MyDate& rhs);
//
//  MyDate operator+(const MyDate& lhs, int           rhs);
//  MyDate operator+(int           lhs, const MyDate& rhs);
//
//  int operator-(const MyDate& lhs, const MyDate& rhs);
//
//  MyDate operator++(MyDate& object, int);
//  MyDate operator--(MyDate& object, int);
//..
// We now provide a reference implementation of each of the methods and free
// (operator) functions associated with the 'MyDate' class defined above (using
// 'bsls_assert' to identify preconditions and invariants where appropriate):
//..
//  // PRIVATE CREATORS
//  inline
//  MyDate::MyDate(int serialDate)
//  : d_serialDate(serialDate)
//  {
//  }
//
//  // CLASS METHODS
//  inline
//  bool MyDate::isValid(int year, int month, int day)
//  {
//      return bdeimp_ProlepticDateUtil::isValidCalendarDate(year, month, day);
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
//  MyDate::MyDate(const MyDate& original)
//  : d_serialDate(original.d_serialDate)
//  {
//  }
//
//  inline
//  MyDate::~MyDate()
//  {
//  }
//
//  // MANIPULATORS
//  inline
//  MyDate& MyDate::operator=(const MyDate& rhs)
//  {
//      d_serialDate = rhs.d_serialDate;
//      return *this;
//  }
//
//  inline
//  MyDate& MyDate::operator++()
//  {
//      ++d_serialDate;
//      BSLS_ASSERT_SAFE(bdeimp_ProlepticDateUtil::isValidSerialDate(
//                                                              d_serialDate));
//      return *this;
//  }
//
//  inline
//  MyDate& MyDate::operator--()
//  {
//      --d_serialDate;
//      BSLS_ASSERT_SAFE(bdeimp_ProlepticDateUtil::isValidSerialDate(
//                                                              d_serialDate));
//      return *this;
//  }
//
//  inline
//  MyDate& MyDate::operator+=(int numDays)
//  {
//      d_serialDate += numDays;
//      BSLS_ASSERT_SAFE(bdeimp_ProlepticDateUtil::isValidSerialDate(
//                                                              d_serialDate));
//      return *this;
//  }
//
//  inline
//  MyDate& MyDate::operator-=(int numDays)
//  {
//      d_serialDate -= numDays;
//      BSLS_ASSERT_SAFE(bdeimp_ProlepticDateUtil::isValidSerialDate(
//                                                              d_serialDate));
//      return *this;
//  }
//
//  inline
//  void MyDate::setYearMonthDay(int year, int month, int day)
//  {
//      d_serialDate = bdeimp_prolepticdateutil::ymd2serial(year, month, day);
//      BSLS_ASSERT_SAFE(bdeimp_ProlepticDateUtil::isValidSerialDate(
//                                                              d_serialDate));
//  }
//
//  inline
//  bool MyDate::setYearMonthDayIfValid(int year, int month, int day)
//  {
//      const int newDate = bdeimp_ProlepticDateUtil::ymd2serial(year,
//                                                               month,
//                                                               day);
//      if (bdeimp_ProlepticDateUtil::isValidSerialDate(newDate)) {
//          d_serialDate = newDate;
//          return true;
//      }
//      return false;
//  }
//
//  // ACCESSORS
//  inline
//  void MyDate::getYearMonthDay(int *year, int *month, int *day) const
//  {
//      bdeimp_ProlepticDateUtil::serial2ymd(year, month, day, d_serialDate);
//  }
//
//  inline
//  int MyDate::year() const
//  {
//      int year, month, day;
//      bdeimp_ProlepticDateUtil::serial2ymd(&year,
//                                           &month,
//                                           &day,
//                                           d_serialDate);
//      return year;
//  }
//
//  inline
//  int MyDate::month() const
//  {
//      int year, month, day;
//      bdeimp_ProlepticDateUtil::serial2ymd(&year,
//                                           &month,
//                                           &day,
//                                           d_serialDate);
//      return month;
//  }
//
//  inline
//  int MyDate::day() const
//  {
//      int year, month, day;
//      bdeimp_ProlepticDateUtil::serial2ymd(&year,
//                                           &month,
//                                           &day,
//                                           d_serialDate);
//      return day;
//  }
//
//  inline
//  bool MyDate::isLeapYear() const
//  {
//      return bdeimp_ProlepticDateUtil::isLeapYear(year());
//  }
//
//  // FREE OPERATORS
//  inline
//  bool operator==(const MyDate& lhs, const MyDate& rhs)
//  {
//      return lhs.d_serialDate == rhs.d_serialDate;
//  }
//
//  inline
//  bool operator!=(const MyDate& lhs, const MyDate& rhs)
//  {
//      return !(lhs == rhs);
//  }
//
//  inline
//  bsl::ostream& operator<<(bsl::ostream& stream, const MyDate& rhs)
//  {
//      return rhs.print(stream);
//  }
//
//  inline
//  MyDate operator+(const MyDate& lhs, int rhs)
//  {
//      return MyDate(lhs.d_serialDate + rhs);
//  }
//
//  inline
//  MyDate operator+(int lhs, const MyDate& rhs)
//  {
//      return MyDate(lhs + rhs.d_serialDate);
//  }
//
//  inline
//  int operator-(const MyDate& lhs, const MyDate& rhs)
//  {
//      return lhs.d_serialDate - rhs.d_serialDate;
//  }
//
//  inline
//  MyDate operator++(MyDate& object, int)
//  {
//      MyDate tmp(object);
//      ++object;
//      return tmp;
//  }
//
//  inline
//  MyDate operator--(MyDate& object, int)
//  {
//      MyDate tmp(object);
//      --object;
//      return tmp;
//  }
//..
// The following definitions would appropriately appear in the implementation
// ('.cpp') file:
//..
//  const char *const monthNames[] = {
//      0, "JAN", "FEB", "MAR", "APR",
//         "MAY", "JUN", "JUL", "AUG",
//         "SEP", "OCT", "NOV", "DEC"
//  };
//
//  // MANIPULATORS
//  bsl::ostream& MyDate::print(bsl::ostream& stream) const
//  {
//      if (!stream) {
//          return stream;                                            // RETURN
//      }
//
//      // space usage: ddMMMyyyy null
//      const int SIZE = 2 + 3 + 4 + 1;
//      char buf[SIZE];
//
//      int y, m, d;
//      bdeimp_ProlepticDateUtil::serial2ymd(&y, &m, &d, d_serialDate);
//
//      buf[0] = d / 10 + '0';
//      buf[1] = d % 10 + '0';
//
//      bsl::memcpy(&buf[2], monthNames[m], 3);
//
//      buf[5] = y / 1000 + '0';
//      buf[6] = ((y % 1000) / 100) + '0';
//      buf[7] = ((y % 100) / 10) + '0';
//      buf[8] = y % 10 + '0';
//      buf[9] = 0;
//
//      stream << buf;
//
//      return stream;
//  }
//..
// The following snippets of code illustrate how to create and use a 'Mydate'
// object.  First create a default object, 'd1':
//..
//  MyDate d1;                        assert(   1 == d1.year());
//                                    assert(   1 == d1.month());
//                                    assert(   1 == d1.day());
//..
// Next, set 'd1' to July 4, 1776:
//..
//  d1.setYearMonthDay(1776, 7, 4);   assert(1776 == d1.year());
//                                    assert(   7 == d1.month());
//                                    assert(   4 == d1.day());
//..
// Then create 'd2' as a copy of 'd1':
//..
//  MyDate d2(d1);                    assert(1776 == d2.year());
//                                    assert(   7 == d2.month());
//                                    assert(   4 == d2.day());
//..
// Now, add six days to the value of 'd2':
//..
//  d2 += 6;                          assert(1776 == d2.year());
//                                    assert(   7 == d2.month());
//                                    assert(  10 == d2.day());
//..
// Next subtract 'd1' from 'd2', storing the difference (in days) in 'dDays':
//..
//  int dDays = d2 - d1;              assert(6 == dDays);
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

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                        // ===============================
                        // struct bdeimp_ProlepticDateUtil
                        // ===============================

struct bdeimp_ProlepticDateUtil {
    // This 'struct' provides a namespace for a suite of pure procedures that
    // perform low-level operations on date values in a variety of formats.
    // Note that all of these functions, whether or not implemented in terms of
    // a static cache, are stateless, and, as such, are inherently thread-safe.

    // CLASS METHODS
    static bool isLeapYear(int year);
        // Return 'true' if the specified 'year' is a proleptic leap year, and
        // 'false' otherwise.  The behavior is undefined unless
        // '1 <= year <= 9999'.

    static bool isValidCalendarDate(int year, int month, int day);
        // Return 'true' if the specified 'year', 'month', and 'day' represents
        // a valid proleptic date value, and 'false' otherwise.  Note that
        // valid date values are in the range '[ 0001/01/01 .. 9999/12/31 ]'.

    static bool isValidSerialDate(int serialDay);
        // Return 'true' if the specified 'serialDay' represents a valid
        // proleptic date value, and 'false' otherwise.  Note that valid date
        // values are in the range '[ 1 .. 3652059 ]'.

    static int ymd2serial(int year, int month, int day);
        // Return the serial date representation of the date value indicated by
        // the specified 'year', 'month', and 'day' using the proleptic
        // calendar.  The behavior is undefined unless
        // 'true == isValidCalendarDate(year, month, day)'.

    static void serial2ymd(int *year, int *month, int *day, int serialDay);
        // Load, into the specified 'year', 'month', and 'day', the date value
        // indicated by the specified proleptic 'serialDay'.  The behavior is
        // undefined unless 'true == isValidSerialDate(serialDay)'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -------------------------------
                        // struct bdeimp_ProlepticDateUtil
                        // -------------------------------

// CLASS METHODS
inline
bool bdeimp_ProlepticDateUtil::isLeapYear(int year)
{
    BSLS_ASSERT_SAFE(1 <= year);
    BSLS_ASSERT_SAFE(year <= 9999);

    return 0 == year % 4 && (0 != year % 100 || 0 == year % 400);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
