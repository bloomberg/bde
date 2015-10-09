// bdlt_dateutil.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLT_DATEUTIL
#define INCLUDED_BDLT_DATEUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide common non-primitive operations on date objects.
//
//@CLASSES:
//  bdlt::DateUtil: namespace for non-primitive operations on date objects
//
//@SEE_ALSO: bdlt_date
//
//@DESCRIPTION: This component provides a 'struct', 'bdlt::DateUtil', that
// serves as a namespace for utility functions that operate on 'bdlt::Date'
// objects.
//
// The following list of methods are provided by 'bdlt::DateUtil':
//..
//  'isValidYYYYMMDD'             o Validate or convert to and from the
//  'convertFromYYYYMMDDRaw'        "YYYYMMDD" format
//  'convertFromYYYYMMDD'           (see {"YYYYMMDD" Format}).
//  'convertToYYYYMMDD'
//
//  'nextDayOfWeek'               o Move a date to the next or the previous
//  'nextDayOfWeekInclusive'        specified day of week.
//  'previousDayOfWeek'
//  'previousDayOfWeekInclusive'
//
//  'nthDayOfWeekInMonth'         o Find a specified day of the week in a
//  'lastDayOfWeekInMonth'          specified year and month.
//
//  'addMonthsEom'                o Add a specified number of months to a date
//  'addMonthsNoEom'                using either the end-of-month or the
//  'addMonths'                     non-end-of-month convention (see
//                                  {End-of-Month Adjustment Conventions}).
//
//  'addYearsEom'                 o Add a specified number of years to a date
//  'addYearsNoEom'                 using either the end-of-month or the
//  'addYears'                      non-end-of-month convention (see
//                                  {End-of-Month Adjustment Conventions}).
//..
//
///"YYYYMMDD" Format
///-----------------
// The "YYYYMMDD" format is a common integral representation of a date that is
// human readable and maintains appropriate ordering when sorted using integer
// comparisons.  The notation uses eight digits (from left to right): four
// digits for the year, two digits for the month, and two digits for the day of
// the month.  For example, February 1, 2014, is represented by the number
// 20140201.
//
// Note that the year is not restricted to values on or after 1000, so, for
// example, 10102 (or 00010102) represents the date January 2, 0002.
//
///End-of-Month Adjustment Conventions
///-----------------------------------
// Two adjustment conventions are used to determine the behavior of the
// functions ('addMonths' and 'addYears') that adjust a date by a particular
// number of months or years: the end-of-month convention and the
// non-end-of-month convention.  The difference between the two conventions is
// that the end-of-month convention adjusts the resulting date to the end of
// the month if the original date is the last day of the month, while the
// non-end-of-month convention does not perform this adjustment.
//
// For example, if we add 3 months to February 28, 2013 using the
// non-end-of-month convention, then the resulting date will be May 28,
// 2013.  If we do the same operation except using the end-of-month convention,
// then the resulting date will be May 31, 2013.
//
// More formal definitions of the two conventions are provided below:
//
//: The End-of-Month Convention:
//:     If the original date to be adjusted is the last day of a month, or if
//:     the day of the month of the original date does not exist in the
//:     resulting date, then adjust the resulting date to be the last day of
//:     the month.
//:
//: The Non-End-of-Month Convention:
//:     If the day of the month of the original date does not exist in the
//:     resulting date, then adjust the resulting date to be the last day of
//:     the month.
//
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Schedule Generation
/// - - - - - - - - - - - - - - -
// Suppose that given a starting date in the 'YYYYMMDD' format, we want to
// generate a schedule for an event that occurs on the same day of the month
// for 12 months.
//
// First, we use the 'bdlt::DateUtil::convertFromYYYYMMDD' function to convert
// the integer into a 'bdlt::Date':
//..
//  const int startingDateYYYYMMDD = 20130430;
//
//  bdlt::Date date;
//  int rc = bdlt::DateUtil::convertFromYYYYMMDD(&date, startingDateYYYYMMDD);
//  assert(0 == rc);
//..
// Now, we use the 'addMonthsEom' function to generate the schedule.  Note
// that 'addMonthsEom' adjusts the resulting date to be the last day of the
// month if the original date is the last day of the month, while
// 'addMonthsNoEom' does not make this adjustment.
//..
//  bsl::vector<bdlt::Date> schedule;
//  schedule.push_back(date);
//
//  for (int i = 1; i < 12; ++i) {
//      schedule.push_back(bdlt::DateUtil::addMonthsEom(date, i));
//  }
//..
// Finally, we print the generated schedule to the console and observe the
// output:
//..
//  bsl::copy(schedule.begin(),
//            schedule.end(),
//            bsl::ostream_iterator<bdlt::Date>(bsl::cout, "\n"));
//
//  // Expected output on the console:
//  //
//  //   30APR2013
//  //   31MAY2013
//  //   30JUN2013
//  //   31JUL2013
//  //   31AUG2013
//  //   30SEP2013
//  //   31OCT2013
//  //   30NOV2013
//  //   31DEC2013
//  //   31JAN2014
//  //   28FEB2014
//  //   31MAR2014
//..
// Notice that the dates have been adjusted to the end of the month.  If we had
// used 'addMonthsNoEom' instead of 'addMonthsEom', this adjustment would not
// have occurred.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BDLT_DAYOFWEEK
#include <bdlt_dayofweek.h>
#endif

#ifndef INCLUDED_BDLT_SERIALDATEIMPUTIL
#include <bdlt_serialdateimputil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bdlt {

                         // ===============
                         // struct DateUtil
                         // ===============

struct DateUtil {
    // This 'struct' provides a namespace for utility functions that provide
    // non-primitive operations on dates.

  private:
    // PRIVATE CLASS METHODS
    static Date addYearsEomEndOfFebruary(const Date& original, int numYears);
        // Return the date that is the specified 'numYears' from the specified
        // 'original' date (which must be either the 28th or 29th of February),
        // adjusted as necessary according to the following (end-of-month)
        // rules: (1) if 'original' is the last day of a month, adjust the
        // result to be the last day of the month, and (2) if the day of the
        // month in 'original' does not exist in the month of the result (e.g.,
        // February 29, 2001), move the resulting date to the last day of the
        // month.  The behavior is undefined unless 'original' is either the
        // 28th or 29th of February, and the resulting date results in a valid
        // 'Date' value.  Note that 'numYears' may be negative.

  public:
    // CLASS METHODS
    static Date addMonths(const Date& original, int numMonths, bool eomFlag);
        // Return the date that is the specified 'numMonths' from the specified
        // 'original' date, adjusted as necessary according to the specified
        // 'eomFlag' (end-of-month flag).  If 'eomFlag' is 'true' and
        // 'original' is the last day of the month, then adjust the result to
        // be the last day of the month; if 'eomFlag' is 'false', then no such
        // adjustment is performed.  In any case, if the day of the month in
        // 'original' does not exist in the month of the result (e.g., February
        // 29, 2001), move the resulting date to the last day of the month.
        // The behavior is undefined unless the operation results in a valid
        // 'Date' value.  Note that 'numMonths' may be negative.

    static Date addMonthsEom(const Date& original, int numMonths);
        // Return the date that is the specified 'numMonths' from the specified
        // 'original' date, adjusted as necessary according to the following
        // (end-of-month) rules: (1) if 'original' is the last day of a month,
        // adjust the result to be the last day of the month, and (2) if the
        // day of the month in 'original' does not exist in the month of the
        // result (e.g., February 30), move the resulting date to the last day
        // of the month.  The behavior is undefined unless the operation
        // results in a valid 'Date' value.  Note that 'numMonths' may be
        // negative.

    static Date addMonthsNoEom(const Date& original, int numMonths);
        // Return the date that is the specified 'numMonths' from the specified
        // 'original' date, adjusted as necessary according to the following
        // (non-end-of-month) rule: if the day of the month in 'original' does
        // not exist in the month of the result (e.g., February 29, 2001), move
        // the resulting date to the last day of the month.  The behavior is
        // undefined unless the operation results in a valid 'Date' value.
        // Note that 'numMonths' may be negative.

    static Date addYears(const Date& original, int numYears, bool eomFlag);
        // Return the date that is the specified 'numYears' from the specified
        // 'original' date, adjusted as necessary according to the specified
        // 'eomFlag' (end-of-month flag).  If 'eomFlag' is 'true' and
        // 'original' is the last day of the month, then adjust the result to
        // be the last day of the month; if 'eomFlag' is 'false', then no such
        // adjustment is performed.  In any case, if the day of the month in
        // 'original' does not exist in the month of the result (e.g., February
        // 29, 2001), move the resulting date to the last day of the month.
        // The behavior is undefined unless the operation results in a valid
        // 'Date' value.  Note that 'numYears' may be negative.

    static Date addYearsEom(const Date& original, int numYears);
        // Return the date that is the specified 'numYears' from the specified
        // 'original' date, adjusted as necessary according to the following
        // (end-of-month) rules: (1) if 'original' is the last day of a month,
        // adjust the result to be the last day of the month, and (2) if the
        // day of the month in 'original' does not exist in the month of the
        // result (e.g., February 29, 2001), move the resulting date to the
        // last day of the month.  The behavior is undefined unless the
        // operation results in a valid 'Date' value.  Note that 'numYears' may
        // be negative.

    static Date addYearsNoEom(const Date& original, int numYears);
        // Return the date that is the specified 'numYears' from the specified
        // 'original' date, adjusted as necessary according to the following
        // (non-end-of-month) rule: if the day of the month in 'original' does
        // not exist in the month of the result (e.g., February 30), move the
        // resulting date to the last day of the month.  The behavior is
        // undefined unless the operation results in a valid 'Date' value.
        // Note that 'numYears' may be negative.

    static int convertFromYYYYMMDD(Date *result, int yyyymmddValue);
        // Load, into the specified 'result', the 'Date' value represented by
        // the specified 'yyyymmddValue' in the "YYYYMMDD" format.  Return 0 on
        // success, and a non-zero value, with no effect on 'result', if
        // 'yyyymmddValue' does not represent a valid 'Date'.

    static Date convertFromYYYYMMDDRaw(int yyyymmddValue);
        // Return the 'Date' value represented by the specified 'yyyymmddValue'
        // in the "YYYYMMDD" format.  The behavior is undefined unless
        // 'yyyymmddValue' represents a valid 'Date'.

    static int convertToYYYYMMDD(const Date& date);
        // Return the integer value in the "YYYYMMDD" format that represents
        // the specified 'date'.

    static bool isValidYYYYMMDD(int yyyymmddValue);
        // Return 'true' if the specified 'yyyymmddValue' represents a valid
        // 'Date' value in the "YYYYMMDD" format, and 'false' otherwise.

    static Date lastDayOfWeekInMonth(int             year,
                                     int             month,
                                     DayOfWeek::Enum dayOfWeek);
        // Return the latest date in the specified 'month' of the specified
        // 'year' that falls on the specified 'dayOfWeek'.  The behavior is
        // undefined unless '1 <= year <= 9999' and '1 <= month <= 12'.

    static Date nextDayOfWeek(DayOfWeek::Enum dayOfWeek, const Date& date);
        // Return the first date *after* the specified 'date' that falls on the
        // specified 'dayOfWeek'.  The behavior is undefined unless the
        // resulting date is no later than 9999/12/31.

    static Date nextDayOfWeekInclusive(DayOfWeek::Enum dayOfWeek,
                                       const Date&     date);
        // Return the first date *on* or *after* the specified 'date' that
        // falls on the specified 'dayOfWeek'.  The behavior is undefined
        // unless the resulting date is no later than 9999/12/31.

    static Date nthDayOfWeekInMonth(int             year,
                                    int             month,
                                    DayOfWeek::Enum dayOfWeek,
                                    int             n);
        // Return the date in the specified 'month' of the specified 'year'
        // corresponding to the specified 'n'th occurrence of the specified
        // 'dayOfWeek'.  If 'n == 5', and a result can not be found in 'month',
        // then return the date of the first 'dayOfWeek' in the following
        // month.  The behavior is undefined unless '1 <= year <= 9999',
        // '1 <= month <= 12', '1 <= n <= 5', and the resulting date is no
        // later than 9999/12/31.
        //
        // For example:
        //..
        //  nthDayOfWeekInMonth(2004, 11, DayOfWeek::k_THURSDAY, 4);
        //..
        // returns November 25, 2004, the fourth Thursday in November, 2004.

    static Date previousDayOfWeek(DayOfWeek::Enum dayOfWeek, const Date& date);
        // Return the last date *before* the specified 'date' that falls on the
        // specified 'dayOfWeek'.  The behavior is undefined unless the
        // resulting date is no earlier than 1/1/1.

    static Date previousDayOfWeekInclusive(DayOfWeek::Enum dayOfWeek,
                                           const Date&     date);
        // Return the last date *on* or *before* the specified 'date' that
        // falls on the specified 'dayOfWeek'.  The behavior is undefined
        // unless the resulting date is no earlier than 1/1/1.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // struct DateUtil
                              // ---------------

// CLASS METHODS
inline
Date DateUtil::addMonths(const Date& original, int numMonths, bool eomFlag)
{

    return eomFlag ? addMonthsEom(original, numMonths)
                   : addMonthsNoEom(original, numMonths);
}

inline
Date DateUtil::addYears(const Date& original, int numYears, bool eomFlag)
{

    return eomFlag ? addYearsEom(original, numYears)
                   : addYearsNoEom(original, numYears);
}

inline
Date DateUtil::addYearsEom(const Date& original, int numYears)
{
    BSLS_ASSERT_SAFE(1    <= original.year() + numYears);
    BSLS_ASSERT_SAFE(9999 >= original.year() + numYears);

    if (2 == original.month() && 28 <= original.day()) {
        return addYearsEomEndOfFebruary(original, numYears);          // RETURN
    }
    return Date(original.year() + numYears, original.month(), original.day());
}

inline
Date DateUtil::addYearsNoEom(const Date& original, int numYears)
{
    BSLS_ASSERT_SAFE(1    <= original.year() + numYears);
    BSLS_ASSERT_SAFE(9999 >= original.year() + numYears);

    const int newYear = original.year() + numYears;

    if (2 == original.month() && 29 == original.day()) {
        return Date(newYear,
                    original.month(),
                    SerialDateImpUtil::isLeapYear(newYear) ? 29 : 28);
                                                                      // RETURN
    }
    return Date(newYear, original.month(), original.day());
}

inline
int DateUtil::convertFromYYYYMMDD(Date *result, int yyyymmddValue)
{
    BSLS_ASSERT_SAFE(result);

    if (!isValidYYYYMMDD(yyyymmddValue)) {
        return 1;                                                     // RETURN
    }
    *result = convertFromYYYYMMDDRaw(yyyymmddValue);

    return 0;
}

inline
Date DateUtil::convertFromYYYYMMDDRaw(int yyyymmddValue)
{
    BSLS_ASSERT_SAFE(isValidYYYYMMDD(yyyymmddValue));

    return Date(yyyymmddValue / 10000,
                (yyyymmddValue / 100) % 100,
                yyyymmddValue % 100);
}

inline
int DateUtil::convertToYYYYMMDD(const Date& date)
{
    return date.year() * 10000 + date.month() * 100 + date.day();
}

inline
bool DateUtil::isValidYYYYMMDD(int yyyymmddValue)
{
    const int day    = yyyymmddValue % 100;
    yyyymmddValue   /= 100;
    const int month  = yyyymmddValue % 100;

    return SerialDateImpUtil::isValidYearMonthDay(yyyymmddValue / 100,
                                                  month,
                                                  day);
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
