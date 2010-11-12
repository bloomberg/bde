// bdetu_month.h             -*-C++-*-
#ifndef INCLUDED_BDETU_MONTH
#define INCLUDED_BDETU_MONTH

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide non-primitive functionality on months of the year.
//
//@CLASSES:
//  bdetu_Month: namespace for operations on months of the year
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: The 'bdet_month' component provides a namespace for a suite of
// pure procedures that operate directly on months of the year
// ('bdet_Month::Month').  For example,
//..
//   bdet_Date nextMonth(bdet_Month::Month month, const bdet_Date& date)
//..
// returns the first date greater than the specified date, that falls in the
// specified month and has the largest valid day-of-month less than or equal to
// that of date, while
//..
//   bdet_Date ceilMonth(bdet_Month::Month month, const bdet_Date& date)
//..
// returns the first date greater than OR EQUAL TO the specified date, that
// falls in the specified month and has the largest valid day-of-month less
// than or equal to that of date.  (Note that the initial day-of-month never
// affects the resulting month or year.)  Several other variants of this type
// of functionality are provided, as illustrated in the following tables:
//..
//
//                            February 2000
//                       S   M  Tu   W  Th   F   S
//                               1   2   3   4   5
//                       6   7   8   9  10  11  12
//                      13  14  15  16  17  18  19
//                      20  21  22  23  24  25  26
//                      27  28  29
//
// +===============================================================+
// | function         | month |    date1    |    date2    | result |
// +---------------------------------------------------------------+
// | numMonthsInRange |  JAN  |  2000JAN31  |  2001JAN01  |   2    |
// +---------------------------------------------------------------+
//
// +===============================================================+
// | function                  | month | initialDate |   result    |
// +---------------------------------------------------------------+
// | nextMonth                 |  FEB  |  2000FEB29  |  2001FEB28  |
// | ceilMonth                 |  FEB  |  2000FEB29  |  2000FEB29  |
// | previousMonth             |  FEB  |  2000FEB29  |  1999FEB28  |
// | floorMonth                |  FEB  |  2000FEB29  |  2000FEB29  |
// | ceilOrPreviousMonthInYear |  FEB  |  2000FEB29  |  2000FEB29  |
// | floorOrNextMonthInYear    |  FEB  |  2000FEB29  |  2000FEB29  |
// +===============================================================+
//
// +===============================================================+
// | function          | month | initialDate | count |   result    |
// +---------------------------------------------------------------+
// | adjustMonth       |  FEB  |  2002JAN31  |   2   |  2003FEB28  |
// | ceilAdjustMonth   |  FEB  |  2002JAN31  |   2   |  2004FEB29* |
// | ceilAdjustMonth2  |  FEB  |  2002JAN31  |   2   |  2004FEB28  |
// | floorAdjustMonth  |  FEB  |  2002JAN31  |   2   |  2003FEB28  |
// | floorAdjustMonth2 |  FEB  |  2002JAN31  |   2   |  2003FEB28  |
// | adjustMonth       |  FEB  |  2002JAN31  |  -2   |  2000FEB29  |
// | ceilAdjustMonth   |  FEB  |  2002JAN31  |  -2   |  2000FEB29* |
// | ceilAdjustMonth2  |  FEB  |  2002JAN31  |  -2   |  2000FEB28  |
// | floorAdjustMonth  |  FEB  |  2002JAN31  |  -2   |  1999FEB28  |
// | floorAdjustMonth2 |  FEB  |  2002JAN31  |  -2   |  1999FEB28  |
// +==============================================================+
// *Note that unlike the ceilAdjustMonth2 and floorAdjustMonth2
// functions, the ceilAdjustMonth and floorAdjustMonth functions
// do not apply the end-of-month adjustment until after the final
// year is determined.
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_MONTH
#include <bdet_month.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif


namespace BloombergLP {


struct bdetu_Month {
    // Provide a namespace for a suite of non-primitive operations on months of
    // the year ('bdet_Month::Month').  Note that the day-of-month is rounded
    // down if necessary to produce a valid result date only after the final
    // month and year are determined.  Note that the initial day-of-month never
    // has any effect on the resulting month or year.

    static int numMonthsInRange(bdet_Month::Month month,
                                const bdet_Date&  date1,
                                const bdet_Date&  date2);
        // Return the number of months in the range beginning with
        // [date1.year(), date1.month()] up to and including [date2.year(),
        // date2.month()] equal to the specified 'month', or 0 if
        // [date1.year(), date1.month()] > [date2.year(), date2.month()].  Note
        // that 'date1.day()' and 'date2.day()' are ignored.

    static bdet_Date nextMonth(bdet_Month::Month month,
                               const bdet_Date&  initialDate);
        // Return the first date greater than the specified 'initialDate' that
        // falls in the specified 'month' and has the largest valid
        // day-of-month less than or equal to that of 'initialDate'.  The
        // behavior is undefined unless the resulting date does not exceed
        // 9999DEC31 and does not fall in the range [1752SEP03 .. 1752SEP13].

    static bdet_Date ceilMonth(bdet_Month::Month month,
                               const bdet_Date&  initialDate);
        // Return the first date greater than OR EQUAL TO the specified
        // 'initialDate' that falls in the specified 'month' and has the
        // largest valid day-of-month less than or equal to that of
        // 'initialDate'.  The behavior is undefined unless the resulting date
        // does not exceed 9999DEC31 and does not fall in the range
        // [1752SEP03 .. 1752SEP13].

    static bdet_Date previousMonth(bdet_Month::Month month,
                                   const bdet_Date&  initialDate);
        // Return the first date less than the specified 'initialDate' that
        // falls in the specified 'month' and has the largest valid
        // day-of-month less than or equal to that of 'initialDate'.  The
        // behavior is undefined unless the resulting date does not precede
        // 0001JAN01 and does not fall in the range [1752SEP03 .. 1752SEP13].

    static bdet_Date floorMonth(bdet_Month::Month month,
                                const bdet_Date&  initialDate);
        // Return the first date less than OR EQUAL TO the specified
        // 'initialDate' that falls in the specified 'month' and has the
        // largest valid day-of-month less than or equal to that of
        // 'initialDate'.  The behavior is undefined unless the resulting date
        // does not precede 0001JAN01 and does not fall in the range
        // [1752SEP03 .. 1752SEP13].

    static bdet_Date adjustMonth(bdet_Month::Month month,
                                 const bdet_Date&  initialDate,
                                 int               count);
        // Return the date that falls on the specified 'count'th occurrence of
        // the specified 'month' after ('count > 0') or before ('count < 0')
        // the specified 'initialDate' with the largest valid day-of-month less
        // than or equal to that of 'initialDate'.  The behavior is undefined
        // unless 'count != 0' and the resulting date falls within the ranges
        // [0001JAN01 .. 1752SEP02] or [1752SEP14 .. 31DEC9999].  Note that for
        // a count of 1 or -1, the behavior of this function is identical to
        // 'nextMonth' and 'previousMonth', respectively.

    static bdet_Date ceilAdjustMonth(bdet_Month::Month month,
                                     const bdet_Date&  initialDate,
                                     int               count);
        // Return the date that falls on the specified 'count'th occurrence of
        // the specified 'month' after ('count >= 0') or before ('count < 0')
        // the first month greater than OR EQUAL TO that of the specified
        // 'initialDate' with the largest valid day-of-month less than or equal
        // to that of 'initialDate'.  The behavior is undefined unless the
        // resulting date falls within the ranges [0001JAN01 .. 1752SEP02] or
        // [1752SEP14 .. 31DEC9999].  Note that unlike the 'ceilAdjustMonth2'
        // function, the end-of-month adjustment is not applied until after the
        // final year is determined:
        //..
        //      2000FEB29 <- ceilAdjustMonth (FEB, 2002JAN31, -2)
        //      2000FEB28 <- ceilAdjustMonth2(FEB, 2002JAN31, -2)
        //..

    static bdet_Date ceilAdjustMonth2(bdet_Month::Month month,
                                      const bdet_Date&  initialDate,
                                      int               count);
        // Return the date that falls on the specified 'count'th occurrence of
        // the specified 'month' after ('count >= 0') or before ('count < 0')
        // the first month greater than OR EQUAL TO that of the specified
        // 'initialDate' with the largest valid day-of-month less than or equal
        // to that of 'initialDate' AND THE INTERMEDIATE DATE RESULTING FROM
        // CEIL.  The behavior is undefined unless the resulting date falls
        // within the ranges [0001JAN01 .. 1752SEP02] or
        // [1752SEP14 .. 31DEC9999].  This function is logically equivalent to
        //..
        //    0 == count
        //    ? ceilMonth(month, initialDate)
        //    : adjustMonth(month, ceilMonth(month, initialDate), count)
        //..
        // Note that unlike the 'ceilAdjustMonth' function, the end-of-month
        // adjustment is applied on the intermediate value before the final
        // year is determined:
        //..
        //      2000FEB29 <- ceilAdjustMonth (FEB, 2002JAN31, -2)
        //      2000FEB28 <- ceilAdjustMonth2(FEB, 2002JAN31, -2)
        //..

    static bdet_Date floorAdjustMonth(bdet_Month::Month month,
                                      const bdet_Date&  initialDate,
                                      int               count);
        // Return the date that falls on the specified 'count'th occurrence of
        // the specified 'month' after ('count >= 0') or before ('count < 0')
        // the first month less than OR EQUAL TO that of the specified
        // 'initialDate' with the largest valid day-of-month less than or equal
        // to that of 'initialDate'.  The behavior is undefined unless the
        // resulting date falls within the ranges [0001JAN01 .. 1752SEP02] or
        // [1752SEP14 .. 31DEC9999].  Note that unlike the 'floorAdjustMonth2'
        // function, the end-of-month adjustment is not applied until after the
        // final year is determined:
        //..
        //      2000FEB29 <- floorAdjustMonth (FEB, 2002MAR31, -2)
        //      2000FEB28 <- floorAdjustMonth2(FEB, 2002MAR31, -2)
        //..

    static bdet_Date floorAdjustMonth2(bdet_Month::Month month,
                                       const bdet_Date&  initialDate,
                                       int               count);
        // Return the date that falls on the specified 'count'th occurrence of
        // the specified 'month' after ('count >= 0') or before ('count < 0')
        // the first month less than OR EQUAL TO that of the specified
        // 'initialDate' with the largest valid day-of-month less than or equal
        // to that of initialDate AND THE INTERMEDIATE DATE RESULTING FROM
        // FLOOR.  The behavior is undefined unless the resulting date falls
        // within the ranges [0001JAN01 .. 1752SEP02] or
        // [1752SEP14 .. 31DEC9999].  This function is logically equivalent to
        //..
        //    0 == count
        //    ? floorMonth(month, initialDate)
        //    : adjustMonth(month, floorMonth(month, initialDate), count)
        //..
        // Note that unlike the 'floorAdjustMonth' function, the end-of-month
        // adjustment is applied on the intermediate value before the final
        // year is determined:
        //..
        //      2000FEB29 <- floorAdjustMonth (FEB, 2002MAR31, -2)
        //      2000FEB28 <- floorAdjustMonth2(FEB, 2002MAR31, -2)
        //..

    static bdet_Date ceilOrPreviousMonthInYear(bdet_Month::Month month,
                                               const bdet_Date&  initialDate);
        // Return the first date greater than OR EQUAL TO the specified
        // 'initialDate' in the same year that falls in the specified 'month'
        // with the largest valid day-of-month less than or equal to that of
        // 'initialDate'.  If the resulting month would fall in the following
        // year, return instead the first date less than 'initialDate' that
        // falls in 'month' and has the largest valid day-of-month less than or
        // equal to that of 'initialDate'.  The behavior is undefined unless
        // the resulting date falls within the ranges [0001JAN01 .. 1752SEP02]
        // or [1752SEP14 .. 31DEC9999].  Note that this function is logically
        // equivalent to
        //..
        //      bdet_Date d = ceilMonth(month, initialDate);
        //      return d.year() == initialDate.year()
        //             ? d
        //             : previousMonth(month, initialDate);
        //..
        // This kind of dual operation within a single year is sometimes
        // characterized as "Modified Following."

    static bdet_Date floorOrNextMonthInYear(bdet_Month::Month month,
                                            const bdet_Date&  initialDate);
        // Return the first date less than OR EQUAL TO the specified
        // 'initialDate' in the same year that falls in the specified 'month'
        // with the largest valid day-of-month less than or equal to that of
        // 'initialDate'.  If the resulting date would fall in the previous
        // year, return instead the first date greater than 'initialDate' that
        // falls in 'month' and has the largest valid day-of-month less than or
        // equal to that of 'initialDate'.  The behavior is undefined unless
        // the resulting date falls within the ranges [0001JAN01 .. 1752SEP02]
        // or [1752SEP14 .. 31DEC9999].  Note that this function is logically
        // equivalent to
        //..
        //      bdet_Date d = floorMonth(month, initialDate);
        //      return d.year() == initialDate.year()
        //             ? d
        //             : nextMonth(month, initialDate);
        //..
        // This kind of dual operation within a single year is sometimes
        // characterized as "Modified Previous."
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

inline
bdet_Date bdetu_Month::ceilMonth(bdet_Month::Month month,
                                 const bdet_Date&  date)
{
    return bdet_Month::Month(date.month()) == month
           ? date
           : nextMonth(month, date);
}

inline
bdet_Date bdetu_Month::floorMonth(bdet_Month::Month month,
                                  const bdet_Date&  date)
{
    return bdet_Month::Month(date.month()) == month
           ? date
           : previousMonth(month, date);
}

inline
bdet_Date bdetu_Month::ceilOrPreviousMonthInYear(bdet_Month::Month month,
                                                 const bdet_Date&  date)
{
    bdet_Date d = ceilMonth(month, date);
    return d.year() == date.year() ? d : previousMonth(month, date);
}

inline
bdet_Date bdetu_Month::floorOrNextMonthInYear(bdet_Month::Month month,
                                              const bdet_Date&  date)
{
    bdet_Date d = floorMonth(month, date);
    return d.year() == date.year() ? d : nextMonth(month, date);
}

inline
bdet_Date bdetu_Month::ceilAdjustMonth2(bdet_Month::Month month,
                                        const bdet_Date&  date,
                                        int               count)
{
    bdet_Date d = ceilMonth(month, date);
    return 0 == count ? d : adjustMonth(month, d, count);
}

inline
bdet_Date bdetu_Month::floorAdjustMonth2(bdet_Month::Month month,
                                         const bdet_Date&  date,
                                         int               count)
{
    bdet_Date d = floorMonth(month, date);
    return 0 == count ? d : adjustMonth(month, d, count);
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
