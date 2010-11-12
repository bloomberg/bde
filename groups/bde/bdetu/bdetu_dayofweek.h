// bdetu_dayofweek.h                                                  -*-C++-*-
#ifndef INCLUDED_BDETU_DAYOFWEEK
#define INCLUDED_BDETU_DAYOFWEEK

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide non-primitive functionality on days of the week.
//
//@CLASSES:
//  bdetu_DayOfWeek: namespace for operations on days of the week
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: The bdetu_dayofweek component provides a namespace for a suite
// of pure procedures that operate directly on days of the week
// ('bdet_DayOfWeek::Day').  For example,
//..
//  bdet_Date nextDay(bdet_DayOfWeek::Day dayOfWeek, const bdet_Date& date)
//..
// returns the first date greater than the specified date, that falls on the
// specified day of the week, while
//..
//  bdet_Date ceilDay(bdet_DayOfWeek::Day dayOfWeek, const bdet_Date& date)
//..
// returns the first date greater than OR EQUAL TO the specified date, that
// falls on the specified day of the week.  Several other variants of this type
// of functionality are provided, as illustrated in the following tables:
//..
//                            January 2000
//                      S   M  Tu   W  Th   F   S
//                                              1
//                      2   3   4   5   6   7   8
//                      9  10  11  12  13  14  15
//                     16  17  18  19  20  21  22
//                     23  24  25  26  27  28  29
//                     30  31
//
// +=================================================================+
// | function       | dayOfWeek |    date1    |    date2    | result |
// +-----------------------------------------------------------------+
// | numDaysInRange |    THU    |  2000JAN06  |  2000JAN27  |   4    |
// +-----------------------------------------------------------------+
//
// +==================================================================+
// | function                 | dayOfWeek | initialDate |   result    |
// +------------------------------------------------------------------+
// | nextDay                  |    THU    |  2000JAN13  |  2000JAN20  |
// | ceilDay                  |    THU    |  2000JAN13  |  2000JAN13  |
// | previousDay              |    THU    |  2000JAN13  |  2000JAN06  |
// | floorDay                 |    THU    |  2000JAN13  |  2000JAN13  |
// | ceilOrPreviousDayInMonth |    THU    |  2000JAN31  |  2000JAN27  |
// | floorOrNextDayInMonth    |    THU    |  2000JAN01  |  2000JAN06  |
// +==================================================================+
//
// +================================================================+
// | function       | dayOfWeek | initialDate | count |   result    |
// +----------------------------------------------------------------+
// | adjustDay      |    THU    |  2000JAN12  |   2   |  2000JAN20  |
// | ceilAdjustDay  |    THU    |  2000JAN13  |   2   |  2000JAN27  |
// | floorAdjustDay |    THU    |  2000JAN14  |   2   |  2000JAN27  |
// | adjustDay      |    THU    |  2000JAN13  |  -2   |  1999DEC30  |
// | ceilAdjustDay  |    THU    |  2000JAN12  |  -2   |  1999DEC30  |
// | floorAdjustDay |    THU    |  2000JAN14  |  -2   |  1999DEC30  |
// +================================================================+
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

#ifndef INCLUDED_BDET_DAYOFWEEK
#include <bdet_dayofweek.h>
#endif

namespace BloombergLP {

                        // ======================
                        // struct bdetu_DayOfWeek
                        // ======================

struct bdetu_DayOfWeek {
    // Provide a namespace for a suite of non-primitive operations on days of
    // the week ('bdet_DayOfWeek::Day').

    // CLASS METHODS
    static int numDaysInRange(bdet_DayOfWeek::Day dayOfWeek,
                              const bdet_Date&    date1,
                              const bdet_Date&    date2);
        // Return the number of days in the range beginning with 'date1' up to
        // and including 'date2' that fall on the specified 'dayOfWeek'.  Note
        // that this function returns 0 if 'date1 > date2'.

    static bdet_Date nextDay(bdet_DayOfWeek::Day dayOfWeek,
                             const bdet_Date&    date);
        // Return the first date greater than the specified 'date' that falls
        // on the specified 'dayOfWeek'.  The behavior is undefined unless the
        // resulting date does not exceed 9999DEC31.

    static bdet_Date ceilDay(bdet_DayOfWeek::Day dayOfWeek,
                             const bdet_Date&    date);
        // Return the first date greater than OR EQUAL TO the specified
        // 'date' that falls on the specified 'dayOfWeek'.  The behavior is
        // undefined unless the resulting date does not exceed 9999DEC31.

    static bdet_Date previousDay(bdet_DayOfWeek::Day dayOfWeek,
                                 const bdet_Date&    date);
        // Return the first date less than the specified 'date' that falls on
        // the specified 'dayOfWeek'.  The behavior is undefined unless the
        // resulting date does not precede 0001JAN01.

    static bdet_Date floorDay(bdet_DayOfWeek::Day dayOfWeek,
                              const bdet_Date&    date);
        // Return the first date less than OR EQUAL TO the specified 'date'
        // that falls on the specified 'dayOfWeek'.  The behavior is undefined
        // unless the resulting date does not precede 0001JAN01.

    static bdet_Date adjustDay(bdet_DayOfWeek::Day dayOfWeek,
                               const bdet_Date&    initialDate,
                               int                 count);
        // Return the date that is the specified 'count'th occurrence of the
        // specified 'dayOfWeek' after ('count > 0') or before ('count < 0')
        // the specified 'initialDate'.  The behavior is undefined unless
        // 'count != 0' and the resulting date falls within the range
        // [0001JAN01 ..9999DEC31].  Note that for a count of 1 or -1, the
        // behavior of this function is identical to 'nextDay' and
        // 'previousDay', respectively.

    static bdet_Date ceilAdjustDay(bdet_DayOfWeek::Day dayOfWeek,
                                   const bdet_Date&    initialDate,
                                   int                 count);
        // Return the date that is the specified 'count'th occurrence of the
        // specified 'dayOfWeek' after ('count >= 0') or before ('count < 0')
        // the first 'dayOfWeek' day greater than OR EQUAL TO the specified
        // 'initialDate'.  The behavior is undefined unless the resulting date
        // falls within the range [0001JAN01 .. 9999DEC31].  Note that this
        // function is logically equivalent to
        //..
        //  0 == count
        //  ? ceilDay(dayOfWeek, initialDate)
        //  : adjustDay(dayOfWeek, ceilDay(dayOfWeek, initialDate), count);
        //..

    static bdet_Date floorAdjustDay(bdet_DayOfWeek::Day dayOfWeek,
                                    const bdet_Date&    initialDate,
                                    int                 count);
        // Return the date that is the specified 'count'th occurrence of the
        // specified 'dayOfWeek' after ('count >= 0') or before ('count < 0')
        // the first 'dayOfWeek' day less than OR EQUAL TO the specified
        // 'initialDate'.  The behavior is undefined unless the resulting date
        // falls within the range [0001JAN01 .. 9999DEC31].  Note that this
        // function is logically equivalent to
        //..
        //  0 == count
        //  ? floorDay(dayOfWeek, initialDate)
        //  : adjustDay(dayOfWeek, floorDay(dayOfWeek, initialDate), count);
        //..

    static bdet_Date ceilOrPreviousDayInMonth(bdet_DayOfWeek::Day dayOfWeek,
                                              const bdet_Date&    date);
        // Return the first date greater than OR EQUAL TO the specified 'date'
        // in the same month that falls on the specified 'dayOfWeek'.  If the
        // resulting date would fall in the following month, return instead the
        // first date less than 'date' that falls on 'dayOfWeek'.  The behavior
        // is undefined unless the resulting date (and all intermediate result
        // dates in the above documentation) falls within the range
        // [0001JAN01 .. 9999DEC31].  Note that this function is logically
        // equivalent to
        //..
        //  bdet_Date d = ceilDay(dayOfWeek, date);
        //  return d.month() == date.month()
        //         ? d
        //         : previousDay(dayOfWeek, date);
        //..
        // This kind of dual operation within a single month is sometimes
        // characterized as "Modified Following."

    static bdet_Date floorOrNextDayInMonth(bdet_DayOfWeek::Day dayOfWeek,
                                           const bdet_Date&    date);
        // Return the first date less than OR EQUAL TO the specified 'date' in
        // the same month that falls on the specified 'dayOfWeek'.  If the
        // resulting date would fall in the previous month, return instead the
        // first date greater than 'date' that falls on 'dayOfWeek'.  The
        // behavior is undefined unless the resulting date (and all
        // intermediate result dates in the above documentation) falls within
        // the range [0001JAN01 .. 9999DEC31].  Note that this function is
        // logically equivalent to
        //..
        //  bdet_Date d = floorDay(dayOfWeek, date);
        //  return d.month() == date.month()
        //         ? d
        //         : nextDay(dayOfWeek, date);
        //..
        // This kind of dual operation within a single month is sometimes
        // characterized as "Modified Previous."

    static
    int weekdayInMonth(int                 year,
                       int                 month,
                       bdet_DayOfWeek::Day dayOfWeek,
                       int                 ordinal);
        // Return the day of the specified 'month' of the specified
        // 'year' corresponding to the specified 'ordinal' instance of the
        // specified 'dayOfWeek' in that month, or 0 if no such day exists.
        // Absolute values for 'ordinal' begin with 1.  E.g.,
        //..
        //  weekdayInMonth(11, 2004, bdet_DayOfWeek::THURSDAY, 4);
        //..
        // requests the day in November, 2004 of the fourth Thursday
        // (which is 25).  Making 'ordinal' negative indicates that its
        // *absolute* *value* references a day counting from the *last*
        // 'dayOfWeek' of the month rather than the first.  E.g.,
        //..
        //  weekdayInMonth(10, 2004, bdet_DayOfWeek::SUNDAY, -1);
        //..
        // requests the date of the last Sunday in October, 2004 (which
        // is 31).  The behavior is undefined unless '1 <= year <= 9999',
        // '1 <= month <= 12', '-5 <= ordinal <= 5', and 'ordinal != 0'.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ----------------------
                        // struct bdetu_DayOfWeek
                        // ----------------------

// CLASS METHODS
inline
bdet_Date bdetu_DayOfWeek::ceilDay(bdet_DayOfWeek::Day dayOfWeek,
                                   const bdet_Date&    date)
{
    return date.dayOfWeek() == dayOfWeek ? date : nextDay(dayOfWeek, date);
}

inline
bdet_Date bdetu_DayOfWeek::floorDay(bdet_DayOfWeek::Day dayOfWeek,
                                    const bdet_Date&    date)
{
    return date.dayOfWeek() == dayOfWeek ? date : previousDay(dayOfWeek, date);
}

inline
bdet_Date bdetu_DayOfWeek::ceilOrPreviousDayInMonth(
                                                 bdet_DayOfWeek::Day dayOfWeek,
                                                 const bdet_Date&    date)
{
    bdet_Date d = ceilDay(dayOfWeek, date);
    return d.month() == date.month() ? d : previousDay(dayOfWeek, date);
}

inline
bdet_Date bdetu_DayOfWeek::floorOrNextDayInMonth(bdet_DayOfWeek::Day dayOfWeek,
                                                 const bdet_Date&    date)
{
    bdet_Date d = floorDay(dayOfWeek, date);
    return d.month() == date.month() ? d : nextDay(dayOfWeek, date);
}

inline
bdet_Date bdetu_DayOfWeek::ceilAdjustDay(bdet_DayOfWeek::Day dayOfWeek,
                                         const bdet_Date&    initialDate,
                                         int                 count)
{
    bdet_Date d = ceilDay(dayOfWeek, initialDate);
    return 0 == count ? d : adjustDay(dayOfWeek, d, count);
}

inline
bdet_Date bdetu_DayOfWeek::floorAdjustDay(bdet_DayOfWeek::Day dayOfWeek,
                                          const bdet_Date&    initialDate,
                                          int                 count)
{
    bdet_Date d = floorDay(dayOfWeek, initialDate);
    return 0 == count ? d : adjustDay(dayOfWeek, d, count);
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
