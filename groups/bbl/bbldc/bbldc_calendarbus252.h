// bbldc_calendarbus252.h                                             -*-C++-*-
#ifndef INCLUDED_BBLDC_CALENDARBUS252
#define INCLUDED_BBLDC_CALENDARBUS252

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide stateless functions for calendar-based BUS-252.
//
//@CLASSES:
//  bbldc::CalendarBus252: BUS-252 convention stateless functions
//
//@DESCRIPTION: This component provides a 'struct', 'bbldc::CalendarBus252',
// that serves as a namespace for defining a suite of date-related functions
// used to compute the day count and the year fraction between two dates as per
// the BUS-252 day-count convention.  In this day-count convention, the day
// count between two ordered dates, 'beginDate' and 'endDate' where
// 'beginDate < endDate', is exactly the number of *business* days occurring in
// the time period '[beginDate .. endDate)'.  Reversing the order of the dates
// negates the result.  When the two dates have the same value, the day count
// is 0.  The year fraction is the day count divided by 252.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing Day Count and Year Fraction
/// - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use 'bbldc::CalendarBus252'
// methods.  First, create two 'bdlt::Date' variables, 'd1' and 'd2':
//..
//  const bdlt::Date d1(2003, 10, 19);
//  const bdlt::Date d2(2003, 12, 31);
//..
// Then, create a 'calendar' with a valid range spanning 2003 and typical
// weekend days:
//..
//  bdlt::Calendar calendar;
//  calendar.setValidRange(bdlt::Date(2003, 1, 1), bdlt::Date(2003, 12, 31));
//  calendar.addWeekendDay(bdlt::DayOfWeek::e_SUN);
//  calendar.addWeekendDay(bdlt::DayOfWeek::e_SAT);
//..
// Next, compute the day count between 'd1' and 'd2':
//..
//  const int daysDiff = bbldc::CalendarBus252::daysDiff(d1, d2, calendar);
//  assert(52 == daysDiff);
//..
// Finally, compute the year fraction between the two dates:
//..
//  const double yearsDiff = bbldc::CalendarBus252::yearsDiff(d1,
//                                                            d2,
//                                                            calendar);
//  // Need fuzzy comparison since 'yearsDiff' is a 'double'.
//  assert(yearsDiff > 0.2063 && yearsDiff < 0.2064);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bblscm_version.h>
#endif

#ifndef INCLUDED_BDLT_CALENDAR
#include <bdlt_calendar.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bbldc {

                          // =====================
                          // struct CalendarBus252
                          // =====================

struct CalendarBus252 {
    // This 'struct' provides a namespace for a suite of pure functions that
    // compute values based on dates according to the BUS-252 day-count
    // convention.

    // CLASS METHODS
    static int daysDiff(const bdlt::Date&     beginDate,
                        const bdlt::Date&     endDate,
                        const bdlt::Calendar& calendar);
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate' according to the BUS-252 day-count convention with the
        // specified 'calendar' providing the definition of business days.  If
        // 'beginDate <= endDate', then the result is non-negative.  The
        // behavior is undefined unless 'calendar.isInRange(beginDate)' and
        // 'calendar.isInRange(endDate)'.  Note that reversing the order of
        // 'beginDate' and 'endDate' negates the result and that the result is
        // 0 when 'beginDate == endDate'.

    static double yearsDiff(const bdlt::Date&     beginDate,
                            const bdlt::Date&     endDate,
                            const bdlt::Calendar& calendar);
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate' according to the BUS-252 day-count
        // convention with the specified 'calendar' providing the definition of
        // business days.  If 'beginDate <= endDate', then the result is
        // non-negative.  The behavior is undefined unless
        // 'calendar.isInRange(beginDate)' and 'calendar.isInRange(endDate)'.
        // Note that reversing the order of 'beginDate' and 'endDate' negates
        // the result; specifically,
        // '|yearsDiff(b, e, c) + yearsDiff(e, b, c)| <= 1.0e-15' for all
        // calendars 'c' and valid dates 'b' and 'e'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                          // ---------------------
                          // struct CalendarBus252
                          // ---------------------

// CLASS METHODS
inline
int CalendarBus252::daysDiff(const bdlt::Date&     beginDate,
                             const bdlt::Date&     endDate,
                             const bdlt::Calendar& calendar)
{
    BSLS_ASSERT_SAFE(calendar.isInRange(beginDate));
    BSLS_ASSERT_SAFE(calendar.isInRange(endDate));

    if (beginDate < endDate) {
        return calendar.numBusinessDays(beginDate, endDate - 1);      // RETURN
    }
    else if (beginDate > endDate) {
        return -calendar.numBusinessDays(endDate, beginDate - 1);     // RETURN
    }
    return 0;
}

inline
double CalendarBus252::yearsDiff(const bdlt::Date&     beginDate,
                                 const bdlt::Date&     endDate,
                                 const bdlt::Calendar& calendar)
{
    return static_cast<double>(daysDiff(beginDate, endDate, calendar)) / 252.0;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
