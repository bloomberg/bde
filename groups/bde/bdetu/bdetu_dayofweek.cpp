// bdetu_dayofweek.cpp                                                -*-C++-*-
#include <bdetu_dayofweek.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdetu_dayofweek_cpp,"$Id$ $CSID$")

#include <bdeimp_dateutil.h>
#include <bsls_assert.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS

static int dayOfWeekDifference(bdet_DayOfWeek::Day day1,
                               bdet_DayOfWeek::Day day2)
    // Return the difference in number of days from the specified 'day1' to the
    // specified 'day2'.
{
    if (day1 > day2) {
        return 7 - day1 + day2;
    }
    else {
        return day2 - day1;
    }
}

                        // ----------------------
                        // struct bdetu_DayOfWeek
                        // ----------------------

// CLASS METHODS
int bdetu_DayOfWeek::numDaysInRange(bdet_DayOfWeek::Day dayOfWeek,
                                    const bdet_Date&    date1,
                                    const bdet_Date&    date2)
{
    if (date1 > date2) {
        return 0;
    }

    int                 count = 0;
    bdet_DayOfWeek::Day dow1  = date1.dayOfWeek();
    bdet_DayOfWeek::Day dow2  = date2.dayOfWeek();
    bdet_Date           d     = date2;

    while(dow1 != dow2) {
        count += dow2 == dayOfWeek;
        --d;
        dow2 = d.dayOfWeek();
    }

    count += dow2 == dayOfWeek;

    BSLS_ASSERT(0 == (d - date1) % 7);
    count += (d - date1) / 7;

    return count;
}

bdet_Date bdetu_DayOfWeek::nextDay(bdet_DayOfWeek::Day dayOfWeek,
                                   const bdet_Date&    initialDate)
{
    bdet_Date d = initialDate + 1;

    return d + dayOfWeekDifference(d.dayOfWeek(), dayOfWeek);
}

bdet_Date bdetu_DayOfWeek::previousDay(bdet_DayOfWeek::Day dayOfWeek,
                                       const bdet_Date&    initialDate)
{
    bdet_Date d = initialDate - 1;

    return d - dayOfWeekDifference(dayOfWeek, d.dayOfWeek());
}

bdet_Date bdetu_DayOfWeek::adjustDay(bdet_DayOfWeek::Day dayOfWeek,
                                     const bdet_Date&    initialDate,
                                     int                 count)
{
    BSLS_ASSERT(count != 0);

    if (0 < count) {
        return nextDay(dayOfWeek, initialDate) + (count - 1) * 7;
    }
    else {
        return previousDay(dayOfWeek, initialDate) + (count + 1) * 7;
    }
}

int bdetu_DayOfWeek::weekdayInMonth(int                 year,
                                    int                 month,
                                    bdet_DayOfWeek::Day dayOfWeek,
                                    int                 ordinal)
{
    BSLS_ASSERT(1  <= year);        BSLS_ASSERT(year    <= 9999);
    BSLS_ASSERT(1  <= month);       BSLS_ASSERT(month   <= 12);
    BSLS_ASSERT(-5 <= ordinal);     BSLS_ASSERT(ordinal <= 5);
    BSLS_ASSERT(0  != ordinal);

    enum { NONE = 0 }; // Returned when a requested fifth weekday does
                       // not exist in the specified month and year.

    int eom = bdeimp_DateUtil::lastDayOfMonth(year, month);
    if (ordinal > 0) {                          // from start of month
        int dow = bdeimp_DateUtil::ymd2weekday(year, month, 1);
        int diff = (7 + dayOfWeek - dow) % 7;
        int dom = 1 + diff; // First weekday in month is [1 .. 7].

        dom += 7 * (ordinal - 1);
        return dom <= eom ? dom : NONE;                               // RETURN
    }
    else {                                      // from end of month
        int dow = bdeimp_DateUtil::ymd2weekday(year, month, eom);
        int diff = (7 + dow - dayOfWeek) % 7;
        int dom = eom - diff; // Last weekday in month is [22 .. 31].

        dom -= 7 * (-ordinal - 1);
        return 1 <= dom ? dom : NONE;                                 // RETURN
    }
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
