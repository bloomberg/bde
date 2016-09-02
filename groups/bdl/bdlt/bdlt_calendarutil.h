// bdlt_calendarutil.h                                                -*-C++-*-
#ifndef INCLUDED_BDLT_CALENDARUTIL
#define INCLUDED_BDLT_CALENDARUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide common date manipulations requiring a calendar.
//
//@CLASSES:
//  bdlt::CalendarUtil: common date manipulations requiring a calendar
//
//@SEE_ALSO: bdlt_date, bdlt_calendar
//
//@DESCRIPTION: This component provides a 'struct', 'bdlt::CalendarUtil', that
// serves as a namespace for date-manipulation functions that require the use
// of a calendar.
//
// This utility component provides the following (static) methods:
//..
//  'addBusinessDaysIfValid'   Add an integral number of business days to the
//                             specified original date within the valid range
//                             of the specified calendar.
//
//  'nthBusDayOfMonthOrMaxIfValid'
//                             Determine the 'n'th business day of the
//                             specified year and month subject to a maximum of
//                             the total number of business days in the
//                             specified year and month, based on the provided
//                             calendar.
//
//  'shiftFollowingIfValid'    If original date is not a business day, move
//                             the date forward until it is a business day.
//
//  'shiftPrecedingIfValid'    If original date is not a business day, move
//                             the date backwards until it is a business day.
//
//  'shiftModifiedFollowingIfValid'
//                             If original date is not a business day, move
//                             the date forward until it is a business day,
//                             unless the date goes into the next month, in
//                             which case the date is moved to the previous
//                             business day.
//
//  'shiftModifiedPrecedingIfValid'
//                             If original date is not a business day, move
//                             the date backward until it is a business day
//                             unless the date goes into the previous month, in
//                             which case the date is moved to the next
//                             business day.
//
//  'shiftIfValid'             Shift a date based on a provided convention.
//                             Note that this function delegates its operation
//                             to one of the above shift functions, based on
//                             the date-shifting convention specified.
//
//  'shiftIfValidExceptOddDay' Shift a date based on a provided convention,
//                             except when it falls on a specified day-of-week,
//                             in which case use a different convention.  Note
//                             that this function delegates its operation to
//                             'shiftIfValid'.
//
//  'shiftIfValidExceptOddDays'
//                             Shift a date based on a provided convention,
//                             except when it falls on either a specified
//                             day-of-week or is one of a continuous series of
//                             non-business days preceding the specified
//                             day-of-week, in which case use a different
//                             convention.  Note that this function delegates
//                             its operation to 'shiftIfValid'.
//
//  'subtractBusinessDaysIfValid'
//                             Subtract an integral number of business days
//                             from the specified original date within the
//                             valid range of the specified calendar.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Manipulating Dates with 'CalendarUtil'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to determine the actual interest payment date in
// January 2014 from a US bond that pays on the 20th of each month and uses the
// modified-following date-shifting convention.
//
// We create a calendar, 'calUS', that has the calendar information populated
// for the US in 2014.  We then use the 'shiftIfValid' function, provided by
// 'CalendarUtil', to compute the payment date.
//
// First, we create a date for January 1, 2014 that corresponds to the nominal
// payment date (which happens to be holiday) and a calendar with valid range
// from April 20, 2012 through April 20, 2014, typical weekend days, and the
// holiday:
//..
//  const bdlt::Date unadjustedDate(2014, 1, 20);
//
//  const bdlt::Date startDate(2012, 4, 20);
//  const bdlt::Date endDate(2014, 4, 20);
//
//  bdlt::Calendar calUS(startDate, endDate);
//  calUS.addWeekendDay(bdlt::DayOfWeek::e_SAT);
//  calUS.addWeekendDay(bdlt::DayOfWeek::e_SUN);
//  calUS.addHoliday(unadjustedDate);
//..
// Now, we determine the actual payment date by invoking the 'shiftIfValid'
// function:
//..
//  bdlt::Date result;
//  int        status = CalendarUtil::shiftIfValid(
//                                         &result,
//                                         unadjustedDate,
//                                         calUS,
//                                         CalendarUtil::e_MODIFIED_FOLLOWING);
//..
// Notice that, 'e_MODIFIED_FOLLOWING' is specified as an argument to
// 'shiftIfValid' to indicate that we want to use the modified-following
// date-shifting convention.
//
// Finally, we verify that the resulting date is correct:
//..
//  const bdlt::Date expected(2014, 1, 21);
//
//  assert(0 == status);
//  assert(expected == result);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_CALENDAR
#include <bdlt_calendar.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BDLT_DAYOFWEEK
#include <bdlt_dayofweek.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bdlt {

                           // ===================
                           // struct CalendarUtil
                           // ===================

struct CalendarUtil {
    // This 'struct' provides a namespace for utility functions that operate on
    // dates in the context of supplied calendars.

    // TYPES
    enum ShiftConvention {
        // Enumeration used to delineate various date-shifting conventions.

        e_UNADJUSTED,           // The date is not adjusted

        e_FOLLOWING,            // The date is adjusted using
                                // 'shiftFollowingIfValid'

        e_PRECEDING,            // The date is adjusted using
                                // 'shiftPrecedingIfValid'

        e_MODIFIED_FOLLOWING,   // The date is adjusted using
                                // 'shiftModifiedFollowingIfValid'

        e_MODIFIED_PRECEDING    // The date is adjusted using
                                // 'shiftModifiedPrecedingIfValid'
    };

    // CLASS METHODS
    static int addBusinessDaysIfValid(bdlt::Date            *result,
                                      const bdlt::Date&      original,
                                      const bdlt::Calendar&  calendar,
                                      int                    numBusinessDays);
        // Load, into the specified 'result', the date that is the specified
        // 'numBusinessDays' chronologically after the specified 'original'
        // date according to the specified 'calendar'.  The resulting date is
        // chronologically before the 'original' date for negative values of
        // 'numBusinessDays', the chronologically earliest business day that is
        // on or after the 'original' date for '0 == numBusinessDays', and
        // chronologically after the 'original' date for positive values of
        // 'numBusinessDays'.  Return 0 on success, and a non-zero value
        // without modifying '*result' if either the original or the resulting
        // date are not within the valid range of 'calendar'.  Note that if
        // '0 != numBusinessDays' then the result of
        // 'addBusinessDaysIfValid(res, orig, cal, numBusinessDays)' is
        // identical to the result of
        // 'subtractBusinessDaysIfValid(res, orig, cal, -numBusinessDays)'.

    static int nthBusinessDayOfMonthOrMaxIfValid(
                                               bdlt::Date            *result,
                                               const bdlt::Calendar&  calendar,
                                               int                    year,
                                               int                    month,
                                               int                    n);
        // Load, into the specified 'result', the date corresponding to the
        // specified 'n'th business day of the specified 'month' and the
        // specified 'year' based on the specified 'calendar'.  'n' may take
        // both positive and negative values.  A positive value of 'n'
        // indicates that counting the number of business days begins from the
        // first calendar date of the month (inclusive), and a negative value
        // of 'n' indicates that counting the number of business days begins
        // from the last calendar date of the month (inclusive).  If there are
        // fewer than 'abs(n)' business days in the month according to the
        // 'calendar', the business day furthest from the first date of the
        // month is chosen if 'n > 0' and the business day furthest from the
        // last date of the month is chose if 'n < 0'.  Return 0 on success,
        // and a non-zero value without modifying '*result' if the entire month
        // specified by 'year' and 'month' is not within the valid range of the
        // 'calendar' or there are no business days in the month specified by
        // 'year' and 'month'.  The behavior is undefined unless 'n != 0',
        // '1 <= year <= 9999', and '1 <= month <= 12'.

    static int shiftFollowingIfValid(bdlt::Date            *result,
                                     const bdlt::Date&      original,
                                     const bdlt::Calendar&  calendar);
        // Load, into the specified 'result', the date of the chronologically
        // earliest business day that is on or after the specified 'original'
        // date based on the specified 'calendar'.  Return 0 on success, and a
        // non-zero value, without modifying '*result', if the following
        // business day can not be found within the valid range of 'calendar'.

    static int shiftIfValid(bdlt::Date            *result,
                            const bdlt::Date&      original,
                            const bdlt::Calendar&  calendar,
                            ShiftConvention        convention);
        // Load, into the specified 'result', the date of the business day that
        // is derived from the specified 'original' date based on the specified
        // 'calendar' according to the specified date-shifting 'convention'.
        // Return 0 on success, and a non-zero value, without modifying
        // '*result', if a valid business date can not be found according to
        // the 'convention' within the valid range of 'calendar'.

    static int shiftIfValid(bdlt::Date            *result,
                            const bdlt::Date&      original,
                            const bdlt::Calendar&  calendar,
                            ShiftConvention        convention,
                            bdlt::DayOfWeek::Enum  specialDay,
                            bool                   extendSpecialDay,
                            ShiftConvention        specialConvention);
        // Load, into the specified 'result', the date of the business day that
        // is derived from the specified 'original' date based on the specified
        // 'calendar' according to the specified date-shifting 'convention',
        // except when the 'original' is either the specified 'specialDay' of
        // the week, or - if the specified 'extendSpecialDay' is 'true' - one
        // of the (possibly empty) set of contiguous non-business days
        // immediately *preceding* a 'specialDay' according to the 'calendar',
        // in which case the 'result' is determined using the specified
        // 'specialConvention'.  Return 0 on success, and a non-zero value,
        // without modifying '*result', if a valid business date can not be
        // found according to the above algorithm within the valid range of
        // 'calendar'.  Note that this method is useful for computing, for
        // example, Korean bond coupon payment dates.

    static int shiftModifiedFollowingIfValid(bdlt::Date            *result,
                                             const bdlt::Date&      original,
                                             const bdlt::Calendar&  calendar);
        // Load, into the specified 'result', the date of the chronologically
        // earliest business day that is on or after the specified 'original'
        // date, unless a date can not be found in the same month, in which
        // case load the chronologically latest business day before the
        // 'original' date based on the specified 'calendar'.  Return 0 on
        // success, and a non-zero value, without modifying '*result' if a
        // business day can not be found within the month of the 'original'
        // date or the valid range of the 'calendar'.

    static int shiftModifiedPrecedingIfValid(bdlt::Date            *result,
                                             const bdlt::Date&      original,
                                             const bdlt::Calendar&  calendar);
        // Load, into the specified 'result', the date of the chronologically
        // latest business day that is on or before the specified 'original'
        // date, unless a date can not be found in the same month, in which
        // case load the chronologically earliest business day after the
        // 'original' date based on the specified 'calendar'.  Return 0 on
        // success, and a non-zero value, without modifying '*result', if a
        // business day can not be found within the month of the 'original'
        // date or the valid range of the 'calendar'.

    static int shiftPrecedingIfValid(bdlt::Date            *result,
                                     const bdlt::Date&      original,
                                     const bdlt::Calendar&  calendar);
        // Load, into the specified 'result', the date of the chronologically
        // latest business day that is on or before the specified 'original'
        // date based on the specified 'calendar'.  Return 0 on success, and a
        // non-zero value, without modifying '*result' if the preceding
        // business day can not be found within the valid range of 'calendar'.

    static int subtractBusinessDaysIfValid(
                                       bdlt::Date            *result,
                                       const bdlt::Date&      original,
                                       const bdlt::Calendar&  calendar,
                                       int                    numBusinessDays);
        // Load, into the specified 'result', the date that is the specified
        // 'numBusinessDays' chronologically before the specified 'original'
        // date according to the specified 'calendar'.  The resulting date is
        // chronologically before the 'original' date for positive values of
        // 'numBusinessDays', the chronologically latest business day that is
        // on or before the 'original' date for '0 == numBusinessDays', and
        // chronologically after the 'original' date for negative values of
        // 'numBusinessDays'.  Return 0 on success, and a non-zero value
        // without modifying '*result' if either the original or the resulting
        // date are not within the valid range of 'calendar'.  Note that if
        // '0 != numBusinessDays' then the result of
        // 'subtractBusinessDaysIfValid(res, orig, cal, numBusinessDays)' is
        // identical to the result of
        // 'addBusinessDaysIfValid(res, orig, cal, -numBusinessDays)'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

inline
int CalendarUtil::shiftFollowingIfValid(bdlt::Date            *result,
                                        const bdlt::Date&      original,
                                        const bdlt::Calendar&  calendar)
{
    BSLS_ASSERT_SAFE(result);

    enum {
        e_SUCCESS      = 0,
        e_BAD_INPUT    = 1,
        e_OUT_OF_RANGE = 2,
        e_NOT_FOUND    = 3
    };

    if (!calendar.isInRange(original)) {
        return e_BAD_INPUT;                                           // RETURN
    }

    bdlt::Calendar::BusinessDayConstIterator iter =
                                          calendar.beginBusinessDays(original);
    if (iter != calendar.endBusinessDays()) {
        *result = *iter;
        return e_SUCCESS;                                             // RETURN
    }
    else {
        return e_OUT_OF_RANGE;                                        // RETURN
    }

    return e_NOT_FOUND;
}

inline
int CalendarUtil::shiftPrecedingIfValid(bdlt::Date            *result,
                                        const bdlt::Date&      original,
                                        const bdlt::Calendar&  calendar)
{
    BSLS_ASSERT_SAFE(result);

    enum {
        e_SUCCESS      = 0,
        e_BAD_INPUT    = 1,
        e_OUT_OF_RANGE = 2,
        e_NOT_FOUND    = 3
    };

    if (!calendar.isInRange(original)) {
        return e_BAD_INPUT;                                           // RETURN
    }

    bdlt::Calendar::BusinessDayConstReverseIterator iter =
                                         calendar.rbeginBusinessDays(original);
    if (iter != calendar.rendBusinessDays()) {
        *result = *iter;
        return e_SUCCESS;                                             // RETURN
    }
    else {
        return e_OUT_OF_RANGE;                                        // RETURN
    }

    return e_NOT_FOUND;
}

}  // close package namespace
}  // close enterprise namespace

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
