// bdlt_serialdateimputil.h                                            -*-C++-*-
#ifndef INCLUDED_BDLT_SERIALDATEIMPUTIL
#define INCLUDED_BDLT_SERIALDATEIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide low-level support functions for date-value manipulation.
//
//@CLASSES:
// bdlt::SerialDateImpUtil: suite of low-level date-related stateless functions
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component implements a utility class,
// 'bdlt::SerialDateImpUtil', that provides a suite of low-level, date-related
// functions that can be used to validate, manipulate, and convert among values
// in three different formats:
//..
//  YMD: year/month/day
//   YD: year/dayOfYear
//    S: serialDate
//..
// The supplied functionality can also be used for determining leap years,
// finding the last day in a month, and for determining the day of the week for
// a given date.  Note that in this component, a "date" is understood to
// represent a valid day in the range '0001JAN01' to '9999DEC31', according to
// the standard UNIX date implementation.
//
///Representations, Valid Dates, and Leap Years
///--------------------------------------------
// The "Calendar Date", or "year-month-day (ymd)", is the primary
// representation and is denoted as "yyyy/mm/dd", with valid years being in the
// range '[ 1 .. 9999 ]'.  Within a valid year, valid months and valid days are
// confined to the respective ranges '[ 1 .. 12 ]' and '[ 1 .. 31 ]'.  Valid
// dates in this representation will range from '0001/01/01' to '9999/12/31',
// but are also governed by the standard UNIX (Gregorian) calendar convention.
//
// The "Day-Of-Year Date", or "year-day (yd)" representation, denoted by
// "yyyy/ddd", represents dates by their year, again in the range
// '[ 1 .. 9999 ]', and the day of year, in the range '[ 1 .. 366 ]'.  Note
// that valid date values in this representation range from '0001/01' to
// '9999/365', with a day-of-year value of 366 permitted for leap years only.
//
// The "Serial Date" representation depicts dates as consecutive integers,
// beginning with 1 (representing '0001JAN01').  In this representation, valid
// date value are in the range '[ 1 .. 3652061 ]'.
//
// In a leap year, February has 29 days instead of the usual 28.  For years
// subsequent to 1752, a year is a leap year if it is divisible by 4, but not
// divisible by 100, unless it is also divisible by 400.  Prior to 1752, all
// years divisible by 4 were leap years.  Note that the year 1752 had only
// '366 - 11 = 355' days, as September 3rd through the 13th were omitted (in a
// one-time correction to the previous, less accurate, leap-year convention).
//
///Caching
///-------
// To achieve maximal runtime performance, several of the functions in this
// component reserve the right to be implemented using statically cached (i.e.,
// tabulated) values (which is inherently thread-safe).  In all cases where a
// cache may be used, 'bdlt::SerialDateImpUtil' explicitly provides a 'NoCache'
// version (e.g., 'ymdToSerialNoCache') that is guaranteed NOT to use a cache.
// Although the "normal" (potentially cached) functions typically gain huge
// performance advantages, the 'NoCache' versions may conceivably be preferred
// by the performance-minded user who is *reasonably* *certain* that the vast
// majority of date values of interest will miss the cache (thus incurring a
// small, but unnecessary overhead for the cache-hit tests).  Note, however,
// that the 'NoCache' function variants are provided primarily for testing and
// for generating that cache in the first place (see
// 'bdlt_serialdateimputil.t.cpp').
//
///Usage
///-----
// This component was created primarily to support the implementation of a
// general-purpose, value-semantic (vocabulary) "Date" type, but also provides
// many low-level utility functions suitable for direct use by other clients.
//
///Creating a General Purpose Utility
/// - - - - - - - - - - - - - - - - -
// Many of the functions provided by this component can be used directly by
// clients that want to ask questions about a particular date in one of the
// three supported formats.
//
// What day of the week was January 3, 2010?
//..
//  assert(2 == bdlt::SerialDateImpUtil::ymdToDayOfWeek(2010, 3, 1));
//                                                           // 2 means Monday.
//..
// Was the year 2000 a leap year?
//..
//  assert(true == bdlt::SerialDateImpUtil::isLeapYear(2000));
//                                                           // Yes, it was.
//..
// Was February 29, 1900, a valid date in history?
//..
//  assert(false == bdlt::SerialDateImpUtil::isValidYearMonthDay(1900, 2, 29));
//                                                           // No, it was not.
//..
// What was the last day of February in 1600?
//..
//  assert(29 == bdlt::SerialDateImpUtil::lastDayOfMonth(1600, 2));
//                                                           // The 29th.
//..
// How many leap years occur from 1959 to 2012 inclusive?
//..
//  assert(14 == bdlt::SerialDateImpUtil::numLeapYears(1959, 2012));
//                                                           // There are 14.
//..
// On what day of the year does February 29, 2020 fall?
//..
//  assert(60 == bdlt::SerialDateImpUtil::ymdToDayOfYear(2020, 2, 29));
//                                                           // The 60th one.
//..
// In what month does the 120th day of 2011 fall?
//..
//  assert(4 == bdlt::SerialDateImpUtil::ydToMonth(2011, 120));
//                                                           // 4 means April.
//..
//
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
//      Day dayOfWeek() const;
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
//      BSLS_ASSERT_SAFE(bdlt::SerialDateImpUtil::isValidSerial(d_serialDate));
//      return *this;
//  }
//
//  inline
//  MyDate& MyDate::operator--()
//  {
//      --d_serialDate;
//      BSLS_ASSERT_SAFE(bdlt::SerialDateImpUtil::isValidSerial(d_serialDate));
//      return *this;
//  }
//
//  inline
//  MyDate& MyDate::operator+=(int numDays)
//  {
//      d_serialDate += numDays;
//      BSLS_ASSERT_SAFE(bdlt::SerialDateImpUtil::isValidSerial(d_serialDate));
//      return *this;
//  }
//
//  inline
//  MyDate& MyDate::operator-=(int numDays)
//  {
//      d_serialDate -= numDays;
//      BSLS_ASSERT_SAFE(bdlt::SerialDateImpUtil::isValidSerial(d_serialDate));
//      return *this;
//  }
//
//  inline
//  void MyDate::setYearMonthDay(int year, int month, int day)
//  {
//      d_serialDate = bdlt::SerialDateImpUtil::ymdToSerial(year, month, day);
//      BSLS_ASSERT_SAFE(bdlt::SerialDateImpUtil::isValidSerial(d_serialDate));
//  }
//
//  inline
//  bool MyDate::setYearMonthDayIfValid(int year, int month, int day)
//  {
//      const int newDate = bdlt::SerialDateImpUtil::ymdToSerial(year,
//                                                              month,
//                                                              day);
//      if (bdlt::SerialDateImpUtil::isValidSerial(newDate)) {
//          d_serialDate = newDate;
//          return true;                                              // RETURN
//      }
//      return false;
//  }
//
//  // ACCESSORS
//  inline
//  void MyDate::getYearMonthDay(int *year, int *month, int *day) const
//  {
//      bdlt::SerialDateImpUtil::serialToYmd(year, month, day, d_serialDate);
//  }
//
//  inline
//  int MyDate::year() const
//  {
//      return bdlt::SerialDateImpUtil::serialToYear(d_serialDate);
//  }
//
//  inline
//  int MyDate::month() const
//  {
//      return bdlt::SerialDateImpUtil::serialToMonth(d_serialDate);
//  }
//
//  inline
//  int MyDate::day() const
//  {
//      return bdlt::SerialDateImpUtil::serialToDay(d_serialDate);
//  }
//
//  inline
//  MyDate::Day MyDate::dayOfWeek() const
//  {
//      return MyDate::Day(bdlt::SerialDateImpUtil::serialToDayOfWeek(
//                                                              d_serialDate));
//  }
//
//  inline
//  bool MyDate::isLeapYear() const
//  {
//      return bdlt::SerialDateImpUtil::isLeapYear(year());
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
//      bdlt::SerialDateImpUtil::serialToYmd(&y, &m, &d, d_serialDate);
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef BDE_OPENSOURCE_PUBLICATION
    #ifdef BDE_USE_PROLEPTIC_DATES
    #error 'BDE_USE_PROLEPTIC_DATES' option disallowed for Bloomberg code.
    #endif
#endif

#ifdef BDE_USE_PROLEPTIC_DATES
#ifndef INCLUDED_BDLT_PROLEPTICDATEIMPUTIL
#include <bdlt_prolepticdateimputil.h>
#endif
#else
#ifndef INCLUDED_BDLT_POSIXDATEIMPUTIL
#include <bdlt_posixdateimputil.h>
#endif
#endif

namespace BloombergLP {
namespace bdlt {

                           // ========================
                           // struct SerialDateImpUtil
                           // ========================

#ifdef BDE_USE_PROLEPTIC_DATES
    typedef ProlepticDateImpUtil SerialDateImpUtil;
#else
    typedef     PosixDateImpUtil SerialDateImpUtil;
#endif

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================


}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
// ----------------------------- END-OF-FILE ----------------------------------
