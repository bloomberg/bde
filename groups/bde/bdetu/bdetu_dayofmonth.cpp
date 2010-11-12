// bdetu_dayofmonth.cpp                                               -*-C++-*-
#include <bdetu_dayofmonth.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdetu_dayofmonth_cpp,"$Id$ $CSID$")

#include <bdeimp_dateutil.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>

namespace BloombergLP {

                        // -----------------------
                        // struct bdetu_DayOfMonth
                        // -----------------------

// CLASS METHODS
bdet_Date bdetu_DayOfMonth::adjustDay(int              dayOfMonth,
                                      const bdet_Date& initialDate,
                                      int              count)
{
    BSLS_ASSERT(1 <= dayOfMonth);
    BSLS_ASSERT(     dayOfMonth <= 31);
    BSLS_ASSERT(count != 0);

    int y, m, d;
    initialDate.getYearMonthDay(&y, &m, &d);

    enum { X = -9999999 };           // "Don't Care" value for tables below

    enum {
        NUM_MONTHS_IN_YEAR    = 12,  // number of months in a year
        NUM_30_MONTHS_IN_YEAR = 11,  // months of year with at least 30 days
        NUM_31_MONTHS_IN_YEAR =  7   // months of year with exactly 31 days
    };

    // The following arrays hold the number of 30-day and 31-day months,
    // respectively, above or below the index corresponding to a specified
    // month's enumerated value.  For Example, there are ten months with at
    // least 30 days above January (enumerated value of 1), and there are six
    // months with exactly 31 days above January.  There are no 30-day or
    // 31-day months below January.  Note that the 'num30above' table applies
    // also to 29-day months in non-leap years.

    static int num30above[13] = { X, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,  0 };
    static int num30below[13] = { X,  0,  1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    static int num31above[13] = { X,  6,  6, 5, 5, 4, 4, 3, 2, 2, 1, 1,  0 };
    static int num31below[13] = { X,  0,  1, 1, 2, 2, 3, 3, 4, 5, 5, 6,  6 };

    // The i'th entry in the following arrays [1 .. 11] and [1 .. 7] holds the
    // index of the i'th month with at least 30 days and exactly 31 days,
    // respectively, from the start or end of the year.  For example, the
    // fourth month from the start of the year with at least 30 days is May
    // (5), and the fourth month from the start of the year with exactly 31
    // days is July (7).  The fourth month from the end of the year with at
    // least 30 days is September (9), and the fourth month from end of the
    // year with exactly 31 days is July (7).  Note that
    // 'which30MonthFromStart' and 'which30MonthFromEnd' apply also to 29-day
    // months in non-leap years.

    static int which30MonthFromStart[12] = {
        X,  1,  3,  4, 5, 6,  7,  8, 9, 10, 11, 12
    };
    static int which30MonthFromEnd[12]   = {
        X, 12, 11, 10, 9, 8,  7,  6, 5,  4,  3,  1
    };

    static int which31MonthFromStart[8] = { X,  1,  3,  5, 7, 8, 10, 12 };
    static int which31MonthFromEnd[8]   = { X, 12, 10,  8, 7, 5,  3,  1 };

    if (count > 0) {
        int ldom = bdeimp_DateUtil::lastDayOfMonth(y, m);
        count -= d + 1 <= dayOfMonth && dayOfMonth <= ldom;

        if (0 == count) {
            return initialDate + (dayOfMonth - d);                    // RETURN
        }
        BSLS_ASSERT(count > 0);

        if (dayOfMonth <= 28) {
            int totalMonths = m + count;     // also does the advance of m
            --totalMonths;                   // make m go back a month
            y += totalMonths / NUM_MONTHS_IN_YEAR;
            m = totalMonths % NUM_MONTHS_IN_YEAR + 1;
            return bdet_Date(y, m, dayOfMonth);                       // RETURN
        }
        else {                               // 'dayOfMonth' is 29, 30, or 31
            BSLS_ASSERT(dayOfMonth > 28);
            switch (dayOfMonth - 29) {
              case 0: {
                bool isLeapYear = bdeimp_DateUtil::isLeapYear(y);
                bool isLeapAndJan = (1 == m && isLeapYear);

                int num29MonthsAboveM = num30above[m] + isLeapAndJan;
                if (count <= num29MonthsAboveM) {      // same year
                    if (isLeapYear) {
                        return bdet_Date(y, m + count, dayOfMonth);   // RETURN
                    }
                    else {
                        int num29MonthsAtOrBelowM = NUM_30_MONTHS_IN_YEAR -
                                                    num29MonthsAboveM;
                        m = which30MonthFromStart[num29MonthsAtOrBelowM+count];
                        return bdet_Date(y, m, dayOfMonth);           // RETURN
                    }
                }
                BSLS_ASSERT(count > num29MonthsAboveM);
                // first year
                count -= num29MonthsAboveM;

                // middle years
                while (count > NUM_MONTHS_IN_YEAR) {
                    int oldYear   = y;
                    int firstYear = y + 1;

                    y += count / NUM_MONTHS_IN_YEAR;
                    count %= NUM_MONTHS_IN_YEAR;
                    int numNonLeapYears = y - oldYear -
                                   bdeimp_DateUtil::numLeapYears(firstYear, y);
                    count += numNonLeapYears;     // Note that 'count' and
                }                                 // 'numNonLeapYears' can
                BSLS_ASSERT(1     <= count);      // never both be equal to 0.
                BSLS_ASSERT(count <= NUM_MONTHS_IN_YEAR);

                ++y;
                isLeapYear = bdeimp_DateUtil::isLeapYear(y);
                if (NUM_MONTHS_IN_YEAR == count && !isLeapYear) {
                    ++y;  // consume 11 months
                    return bdet_Date(y, 1, dayOfMonth);               // RETURN
                }

                // final year
                BSLS_ASSERT(count > 0);
                BSLS_ASSERT(count <= NUM_30_MONTHS_IN_YEAR
                        || (count <= NUM_MONTHS_IN_YEAR && isLeapYear));
                m = count + (count > 1 && !isLeapYear);
                return bdet_Date(y, m, dayOfMonth);                   // RETURN
              }
              case 1: {
                int num30MonthsAboveM = num30above[m];
                if (count <= num30MonthsAboveM) {      // same year
                    int num30MonthsAtOrBelowM = NUM_30_MONTHS_IN_YEAR -
                                                num30MonthsAboveM;
                    m = which30MonthFromStart[num30MonthsAtOrBelowM + count];
                    return bdet_Date(y, m, dayOfMonth);               // RETURN
                }
                count -= num30MonthsAboveM;            // first year
                y += count / NUM_30_MONTHS_IN_YEAR;    // whole years
                count %= NUM_30_MONTHS_IN_YEAR;        // remaining months
                if (0 == count) {
                    return bdet_Date(y, 12, dayOfMonth);              // RETURN
                }
                BSLS_ASSERT(count > 0);
                return bdet_Date(++y, which30MonthFromStart[count],dayOfMonth);
                                                                      // RETURN
              }
              case 2: {
                int num31MonthsAboveM = num31above[m];
                if (count <= num31MonthsAboveM) {      // same year
                    int num31MonthsAtOrBelowM = NUM_31_MONTHS_IN_YEAR -
                                                num31MonthsAboveM;
                    m = which31MonthFromStart[num31MonthsAtOrBelowM + count];
                    return bdet_Date(y, m, dayOfMonth);               // RETURN
                }
                count -= num31MonthsAboveM;            // first year
                y += count / NUM_31_MONTHS_IN_YEAR;    // whole years
                count %= NUM_31_MONTHS_IN_YEAR;        // remaining months
                if (0 == count) {
                    return bdet_Date(y, 12, dayOfMonth);              // RETURN
                }
                BSLS_ASSERT(count > 0);
                return bdet_Date(++y, which31MonthFromStart[count],dayOfMonth);
                                                                      // RETURN
              }
            }
        }
    }
    else {  // count < 0
        count = -count;
        BSLS_ASSERT(count > 0);
        count -= d > dayOfMonth;
        BSLS_ASSERT(dayOfMonth > 0);

        if (0 == count) {
            return initialDate + (dayOfMonth - d);                    // RETURN
        }
        BSLS_ASSERT(count > 0);

        if (dayOfMonth <= 28) {
            y -= count / NUM_MONTHS_IN_YEAR;
            m -= count % NUM_MONTHS_IN_YEAR;
            if (m < 1) {
                m += NUM_MONTHS_IN_YEAR;
                --y;
            }
            return bdet_Date(y, m, dayOfMonth);                       // RETURN
        }
        else {                          // 'dayOfMonth' is 29, 30, or 31
            BSLS_ASSERT(dayOfMonth > 28);
            switch (dayOfMonth - 29) {
              case 0: {
                bool isLeapYear = bdeimp_DateUtil::isLeapYear(y);
                bool isLeapAndMar2Dec = (m > 2 && isLeapYear);

                int num29MonthsBelowM = num30below[m] + isLeapAndMar2Dec;
                if (count <= num29MonthsBelowM) {      // same year
                    if (isLeapYear) {
                        return bdet_Date(y, m - count, dayOfMonth);   // RETURN
                    }
                    else {
                        int num29MonthsAtOrAboveM = NUM_30_MONTHS_IN_YEAR -
                                                    num29MonthsBelowM;
                        m = which30MonthFromEnd[num29MonthsAtOrAboveM + count];
                        return bdet_Date(y, m, dayOfMonth);           // RETURN
                    }
                }
                BSLS_ASSERT(count > num29MonthsBelowM);

                // first year
                count -= num29MonthsBelowM;

                // middle years
                while (count > NUM_MONTHS_IN_YEAR) {
                    int oldYear = y;
                    int firstYear = y - 1;
                    y -= count / NUM_MONTHS_IN_YEAR;
                    count %= NUM_MONTHS_IN_YEAR;
                    int numNonLeapYears = oldYear - y -
                                 bdeimp_DateUtil::numLeapYears(y, firstYear);
                    count += numNonLeapYears;     // Note that 'count' and
                }                                 // 'numNonLeapYears' can
                BSLS_ASSERT(1     <= count);      // never both be equal to 0.
                BSLS_ASSERT(count <= NUM_MONTHS_IN_YEAR);

                --y;
                isLeapYear = bdeimp_DateUtil::isLeapYear(y);
                if (NUM_MONTHS_IN_YEAR == count && !isLeapYear) {
                    --y;  // consume 11 months;
                    return bdet_Date(y, 12, dayOfMonth);              // RETURN
                }

                // final year
                BSLS_ASSERT(count > 0);
                BSLS_ASSERT(count <= NUM_30_MONTHS_IN_YEAR
                           || (count <= NUM_MONTHS_IN_YEAR && isLeapYear));
                m = NUM_MONTHS_IN_YEAR + 1 - count - // a count of 1 equals DEC
                    (11 == count && !isLeapYear);    // If FEB && !leapYear
                                                     // backup to JAN.
                return bdet_Date(y, m, dayOfMonth);                   // RETURN
              }
              case 1: {
                int num30MonthsBelowM = num30below[m];
                if (count <= num30MonthsBelowM) {      // same year
                    int num30MonthsAtOrAboveM = NUM_30_MONTHS_IN_YEAR -
                                                num30MonthsBelowM;
                    m = which30MonthFromEnd[num30MonthsAtOrAboveM + count];
                    return bdet_Date(y, m, dayOfMonth);               // RETURN
                }
                count -= num30MonthsBelowM;            // first year
                y -= count / NUM_30_MONTHS_IN_YEAR;    // whole years
                count %= NUM_30_MONTHS_IN_YEAR;        // remaining months
                if (0 == count) {
                    return bdet_Date(y, 1, dayOfMonth);               // RETURN
                }
                BSLS_ASSERT(count > 0);
                return bdet_Date(--y, which30MonthFromEnd[count], dayOfMonth);
                                                                      // RETURN
              }
              case 2: {
                int num31MonthsBelowM = num31below[m];
                if (count <= num31MonthsBelowM) {      // same year
                    int num31MonthsAtOrAboveM = NUM_31_MONTHS_IN_YEAR -
                                                num31MonthsBelowM;
                    m = which31MonthFromEnd[num31MonthsAtOrAboveM + count];
                    return bdet_Date(y, m, dayOfMonth);               // RETURN
                }
                count -= num31MonthsBelowM;            // first year
                y -= count / NUM_31_MONTHS_IN_YEAR;    // whole years
                count %= NUM_31_MONTHS_IN_YEAR;        // remaining months
                if (0 == count) {
                    return bdet_Date(y, 1, dayOfMonth);               // RETURN
                }

                BSLS_ASSERT(count > 0);
                return bdet_Date(--y, which31MonthFromEnd[count], dayOfMonth);
                                                                      // RETURN
              }
            }
        }
    }
    BSLS_ASSERT(0);
    return bdet_Date();
}

bdet_Date
bdetu_DayOfMonth::ceilOrPreviousDayInMonth(int              dayOfMonth,
                                           const bdet_Date& date)
{
    BSLS_ASSERT(1 <= dayOfMonth);
    BSLS_ASSERT(     dayOfMonth <= 31);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(9999 == date.year()
                                           && 12   == date.month()
                                           && date.day() > dayOfMonth)) {
        return previousDay(dayOfMonth, date);
    }
    bdet_Date d = ceilDay(dayOfMonth, date);
    return d.month() == date.month() ? d : previousDay(dayOfMonth, date);
}

bdet_Date bdetu_DayOfMonth::ceilAdjustDay(int              dayOfMonth,
                                          const bdet_Date& date,
                                          int              count)
{
    BSLS_ASSERT(1 <= dayOfMonth);
    BSLS_ASSERT(     dayOfMonth <= 31);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(9999 == date.year()
                                           && 12   == date.month()
                                           && date.day() > dayOfMonth)) {
        BSLS_ASSERT(count < 0);
        return adjustDay(dayOfMonth, date, count);
    }
    bdet_Date d = ceilDay(dayOfMonth, date);
    return 0 == count ? d : adjustDay(dayOfMonth, d, count);
}

bdet_Date bdetu_DayOfMonth::floorOrNextDayInMonth(int              dayOfMonth,
                                                  const bdet_Date& date)
{
    BSLS_ASSERT(1 <= dayOfMonth);
    BSLS_ASSERT(     dayOfMonth <= 31);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 == date.year()
                                           && 1 == date.month()
                                           && date.day() < dayOfMonth)) {
        return nextDay(dayOfMonth, date);
    }
    bdet_Date d = floorDay(dayOfMonth, date);
    return d.month() == date.month() ? d : nextDay(dayOfMonth, date);
}

bdet_Date bdetu_DayOfMonth::floorAdjustDay(int              dayOfMonth,
                                           const bdet_Date& date,
                                           int              count)
{
    BSLS_ASSERT(1 <= dayOfMonth);
    BSLS_ASSERT(     dayOfMonth <= 31);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 == date.year()
                                           && 1 == date.month()
                                           && date.day() < dayOfMonth)) {
        BSLS_ASSERT(count > 0);
        return adjustDay(dayOfMonth, date, count);
    }
    bdet_Date d = floorDay(dayOfMonth, date);
    return 0 == count ? d : adjustDay(dayOfMonth, d, count);
}

bdet_Date bdetu_DayOfMonth::nextDay(int              dayOfMonth,
                                    const bdet_Date& initialDate)
{
    BSLS_ASSERT(1 <= dayOfMonth);
    BSLS_ASSERT(     dayOfMonth <= 31);

    // The worst case is to advance by two months (e.g., from MAR31 to MAY31).

    bdet_Date result = initialDate;

    int y, m, d;
    initialDate.getYearMonthDay(&y, &m, &d);

    // first month
    int ldom = bdeimp_DateUtil::lastDayOfMonth(y, m);
    if (d < dayOfMonth && dayOfMonth <= ldom) {
                                        // found in first month
        return result += dayOfMonth - d;                              // RETURN
    }

    result += ldom - d;                 // not found in first month
    ++(m %= 12);                        // No Worries, JAN always has 31 days
    ldom = bdeimp_DateUtil::lastDayOfMonth(y, m); // so year can be off.

    // second or third month
    if (dayOfMonth > ldom) {
        result += ldom;
    }

    return result += dayOfMonth;
}

int bdetu_DayOfMonth::numDaysInRange(int              dayOfMonth,
                                     const bdet_Date& date1,
                                     const bdet_Date& date2)
{
    BSLS_ASSERT(1 <= dayOfMonth);
    BSLS_ASSERT(     dayOfMonth <= 31);

    if (date1 > date2) {
        return 0;                                                     // RETURN
    }

    int y1, m1, d1, y2, m2, d2;
    date1.getYearMonthDay(&y1, &m1, &d1);
    date2.getYearMonthDay(&y2, &m2, &d2);

    int count = 0;

    if (y1 == y2 && m1 == m2) {
        return d1 <= dayOfMonth && dayOfMonth <= d2;                  // RETURN
    }

    // More than one month; look at ends, then treat whole months separately.

    int ldom = bdeimp_DateUtil::lastDayOfMonth(y1, m1);
    count += d1 <= dayOfMonth && dayOfMonth <= ldom;
    count += d2 >= dayOfMonth;

    // --------------------------------------------------------------------
    // From now on, look only at the whole months; the end months are done.
    // --------------------------------------------------------------------

    int numMonths = (y2 - y1) * 12 + m2 - m1 - 1;

    if (dayOfMonth <= 28) {
        return count + numMonths;                                     // RETURN
    }

    BSLS_ASSERT(dayOfMonth > 28);

    // The following arrays are from specified month index on (up or down)
    // Note that 30 also applies to 29 (except for leap-year adjustment).

    static int num31up[14]   = {
        0,  7,  6,  6, 5, 5, 4, 4, 3, 2, 2,  1,  1, 0
    };
    static int num31down[14] = {
        0,  1,  1,  2, 2, 3, 3, 4, 5, 5, 6,  6,  7, 0
    };
    static int num30up[14]   = {
        0, 11, 10, 10, 9, 8, 7, 6, 5, 4, 3,  2,  1, 0
    };
    static int num30down[14] = {
        0,  1,  1,  2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0
    };

    if (31 == dayOfMonth) {    // there are 7 31-day months in a year
        if (y2 > y1) {
            count += num31up[m1 + 1];       // first year
            count += (y2 - y1 - 1) * 7;     // whole years
            count += num31down[m2 - 1];     // final year
        }
        else {
            count += num31down[m2 - 1] - num31down[m1];
        }
    }

    else {                     // there are 11 30-or-more-day months in a year
        if (y2 > y1) {
            count += num30up[m1 + 1];       // first year
            count += (y2 - y1 - 1) * 11;    // whole years
            count += num30down[m2 - 1];     // final year
        }
        else {
            count += num30down[m2 - 1] - num30down[m1];
        }

        if (29 == dayOfMonth) {
            if (y2 > y1) {
                count += m1 <= 1 && bdeimp_DateUtil::isLeapYear(y1);
                count += m2 >= 3 && bdeimp_DateUtil::isLeapYear(y2);
                if (y1 + 1 < y2) {
                    count += bdeimp_DateUtil::numLeapYears(y1 + 1, y2 - 1);
                }
            }
            else { // y1 == y2
                count += m1 <= 1 && m2 >= 3
                                 && bdeimp_DateUtil::isLeapYear(y1);
            }
        }
    }
    return count;
}

bdet_Date bdetu_DayOfMonth::previousDay(int              dayOfMonth,
                                        const bdet_Date& initialDate)
{
    BSLS_ASSERT(1 <= dayOfMonth);
    BSLS_ASSERT(     dayOfMonth <= 31);

    // The worst case is to go back by two months (e.g., from MAR30 to JAN30).

    bdet_Date result = initialDate;

    int y, m, d;
    initialDate.getYearMonthDay(&y, &m, &d);

    // first month
    if (d > dayOfMonth) {
        return result -= d - dayOfMonth;                              // RETURN
    }
    result -= d;                        // not found in first month
    --m;                                // No Worries, DEC always has 31 days..
    m += 12 * !m;                       // if (0 == m) m = 12;
    int ldom = bdeimp_DateUtil::lastDayOfMonth(y, m); // ..so year can be off.

    // second month
    if (dayOfMonth <= ldom) {
        return result -= ldom - dayOfMonth;                           // RETURN
    }

    // third month -- note second Month must be FEB, APR, JUN, SEP, or NOV...
    return result -= ldom + 31 - dayOfMonth; // 31 represents the last day
                                             // of month of the third month.
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
