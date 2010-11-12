// bdetu_dayofmonth.h                                                 -*-C++-*-
#ifndef INCLUDED_BDETU_DAYOFMONTH
#define INCLUDED_BDETU_DAYOFMONTH

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide non-primitive functionality on days of the month.
//
//@CLASSES:
//  bdetu_DayOfMonth: namespace for operations on days of the month
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: The bdetu_dayofmonthutil component provides a namespace for a
// suite of pure procedures that operate directly on days of the month ('int'
// in the range [1 .. 31]).  For example,
//..
//   bdet_Date nextDay(int dayOfMonth, const bdet_Date& date)
//..
// returns the first date greater than the specified date, that falls on the
// specified day of the month, while
//..
//   bdet_Date ceilDay(int dayOfMonth, const bdet_Date& date)
//..
// returns the first date greater than OR EQUAL TO the specified date, that
// falls on the specified day of the month.  Several other variants of this
// type of functionality are provided, as illustrated in the following tables:
//..
//                                  2000
//
//           Jan                    Feb                    Mar
//   S  M Tu  W Th  F  S    S  M Tu  W Th  F  S    S  M Tu  W Th  F  S
//                     1          1  2  3  4  5             1  2  3  4
//   2  3  4  5  6  7  8    6  7  8  9 10 11 12    5  6  7  8  9 10 11
//   9 10 11 12 13 14 15   13 14 15 16 17 18 19   12 13 14 15 16 17 18
//  16 17 18 19 20 21 22   20 21 22 23 24 25 26   19 20 21 22 23 24 25
//  23 24 25 26 27 28 29   27 28 29               26 27 28 29 30 31
//  30 31
//
// +==================================================================+
// | function       | dayOfMonth |    date1    |    date2    | result |
// +------------------------------------------------------------------+
// | numDaysInRange |     30     |  2000JAN01  |  2000MAR31  |   2    |
// +------------------------------------------------------------------+
//
// +===================================================================+
// | function                 | dayOfMonth | initialDate |   result    |
// +-------------------------------------------------------------------+
// | nextDay                  |     15     |  2000FEB15  |  2000MAR15  |
// | ceilDay                  |     15     |  2000FEB15  |  2000FEB15  |
// | previousDay              |     15     |  2000FEB15  |  2000JAN15  |
// | floorDay                 |     15     |  2000FEB15  |  2000FEB15  |
// | ceilOrPreviousDayInMonth |     15     |  2000FEB15  |  2000FEB15  |
// | floorOrNextDayInMonth    |     15     |  2000FEB15  |  2000FEB15  |
// +===================================================================+
//
// +=================================================================+
// | function       | dayOfMonth | initialDate | count |   result    |
// +-----------------------------------------------------------------+
// | adjustDay      |     15     |  2000JAN14  |   2   |  2000FEB15  |
// | ceilAdjustDay  |     15     |  2000JAN14  |   2   |  2000MAR15  |
// | floorAdjustDay |     15     |  2000JAN14  |   2   |  2000FEB15  |
// | adjustDay      |     15     |  2000JAN14  |  -2   |  1999NOV15  |
// | ceilAdjustDay  |     15     |  2000JAN14  |  -2   |  1999NOV15  |
// | floorAdjustDay |     15     |  2000JAN14  |  -2   |  1999OCT15  |
// +=================================================================+
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                           // =======================
                           // struct bdetu_DayOfMonth
                           // =======================

struct bdetu_DayOfMonth {
    // Provide a namespace for a suite of non-primitive operations on days of
    // the month (i.e., in the range '[1 .. 31]').

    // CLASS METHODS
    static int numDaysInRange(int              dayOfMonth,
                              const bdet_Date& date1,
                              const bdet_Date& date2);
        // Return the number of days in the range beginning with 'date1' up to
        // and including 'date2' that fall on the specified 'dayOfMonth'.  Note
        // that this function returns 0 if 'date1 > date2'.  The behavior is
        // undefined unless 'dayOfMonth' is within the range '[1 .. 31]'.

    static bdet_Date nextDay(int dayOfMonth, const bdet_Date& initialDate);
        // Return the first date greater than the specified 'initialDate' that
        // falls on the specified 'dayOfMonth'.  The behavior is undefined
        // unless 'dayOfMonth' is within the range '[1 .. 31]' and the
        // resulting date does not exceed 9999DEC31.

    static bdet_Date ceilDay(int dayOfMonth, const bdet_Date& initialDate);
        // Return the first date greater than OR EQUAL TO the specified
        // 'initialDate' that falls on the specified 'dayOfMonth'.  The
        // behavior is undefined unless 'dayOfMonth' is within the range
        // '[1 .. 31]' and the resulting date does not exceed 9999DEC31.

    static bdet_Date previousDay(int dayOfMonth, const bdet_Date& initialDate);
        // Return the first date less than the specified 'initialDate' that
        // falls on the specified 'dayOfMonth'.  The behavior is undefined
        // unless 'dayOfMonth' is within the range '[1 .. 31]' and the
        // resulting date does not precede 0001JAN01.

    static bdet_Date floorDay(int dayOfMonth, const bdet_Date& initialDate);
        // Return the first date less than OR EQUAL TO the specified
        // 'initialDate' that falls on the specified 'dayOfMonth'.  The
        // behavior is undefined unless 'dayOfMonth' is within the range
        // '[1 .. 31]' and the resulting date does not precede 0001JAN01.

    static bdet_Date adjustDay(int              dayOfMonth,
                               const bdet_Date& initialDate,
                               int              count);
        // Return the date that is the specified 'count'th occurrence of the
        // specified 'dayOfMonth' after ('count > 0') or before ('count < 0')
        // the specified 'initialDate'.  The behavior is undefined unless
        // 'count != 0', 'dayOfMonth' is within the range '[1 .. 31]', and the
        // resulting date falls within the range '[0001JAN01 .. 9999DEC31]'.
        // Note that for a count of 1 or -1, the behavior of this function is
        // identical to 'nextDay' and 'previousDay', respectively.

    static bdet_Date ceilAdjustDay(int              dayOfMonth,
                                   const bdet_Date& initialDate,
                                   int              count);
        // Return the date that is the specified 'count'th occurrence of the
        // specified 'dayOfMonth' after ('count >= 0') or before ('count < 0')
        // the first 'dayOfMonth' day greater than OR EQUAL TO the specified
        // 'initialDate'.  The behavior is undefined unless 'dayOfMonth' is
        // within the range '[1 .. 31]' and the resulting date falls within the
        // range '[0001JAN01 .. 9999DEC31]'.  Note that this function is
        // logically equivalent to:
        //..
        //  0 == count
        //  ? ceilDay(dayOfMonth, initialDate)
        //  : adjustDay(dayOfMonth, ceilDay(dayOfMonth, initialDate), count);
        //..

    static bdet_Date floorAdjustDay(int              dayOfMonth,
                                    const bdet_Date& initialDate,
                                    int              count);
        // Return the date that is the specified 'count'th occurrence of the
        // specified 'dayOfMonth' after ('count >= 0') or before ('count < 0')
        // the first 'dayOfMonth' day less than OR EQUAL TO the specified
        // 'initialDate'.  The behavior is undefined unless 'dayOfMonth' is
        // within the range '[1 .. 31]' and the resulting date falls within the
        // range '[0001JAN01 .. 9999DEC31]'.  Note that this function is
        // logically equivalent to:
        //..
        //  0 == count
        //  ? floorDay(dayOfMonth, initialDate)
        //  : adjustDay(dayOfMonth, floorDay(dayOfMonth, initialDate), count);
        //..

    static bdet_Date ceilOrPreviousDayInMonth(int              dayOfMonth,
                                              const bdet_Date& initialDate);
        // Return the first date greater than OR EQUAL TO the specified
        // 'initialDate' in the same month that falls on the specified
        // 'dayOfMonth'.  If the resulting date would fall in the following
        // month, return instead the first date less than 'initialDate' that
        // falls on 'dayOfMonth'.  The behavior is undefined unless
        // 'dayOfMonth' is within the range '[1 .. 31]' and the resulting date
        // falls within the range '[0001JAN01 .. 9999DEC31]'.  Note that
        // this function is logically equivalent to:
        //..
        //  bdet_Date d = ceilDay(dayOfMonth, initialDate);
        //  return d.month() == initialDate.month()
        //         ? d
        //         : previousDay(dayOfMonth, initialDate);
        //..
        // Also note that this kind of dual operation within a single month is
        // sometimes characterized as "Modified Following."

    static bdet_Date floorOrNextDayInMonth(int              dayOfMonth,
                                           const bdet_Date& initialDate);
        // Return the first date less than OR EQUAL TO the specified
        // 'initialDate' in the same month that falls on the specified
        // 'dayOfMonth'.  If the resulting date would fall in the previous
        // month, return instead the first date greater than 'initialDate' that
        // falls on 'dayOfMonth'.  The behavior is undefined unless
        // 'dayOfMonth' is within the range '[1 .. 31]' and the resulting date
        // falls within the range '[0001JAN01 .. 9999DEC31]'.  Note that this
        // function is logically equivalent to:
        //..
        //  bdet_Date d = floorDay(dayOfMonth, initialDate);
        //  return d.month() == initialDate.month()
        //         ? d
        //         : nextDay(dayOfMonth, initialDate);
        //..
        // Also note that this kind of dual operation within a single month is
        // sometimes characterized as "Modified Previous."
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                           // -----------------------
                           // struct bdetu_DayOfMonth
                           // -----------------------

// CLASS METHODS
inline
bdet_Date bdetu_DayOfMonth::ceilDay(int dayOfMonth, const bdet_Date& date)
{
    BSLS_ASSERT_SAFE(1 <= dayOfMonth);
    BSLS_ASSERT_SAFE(     dayOfMonth <= 31);

    return date.day() == dayOfMonth ? date : nextDay(dayOfMonth, date);
}

inline
bdet_Date bdetu_DayOfMonth::floorDay(int dayOfMonth, const bdet_Date& date)
{
    BSLS_ASSERT_SAFE(1 <= dayOfMonth);
    BSLS_ASSERT_SAFE(     dayOfMonth <= 31);

    return date.day() == dayOfMonth ? date : previousDay(dayOfMonth, date);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
