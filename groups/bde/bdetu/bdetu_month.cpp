// bdetu_month.cpp   -*-C++-*-
#include <bdetu_month.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdetu_month_cpp,"$Id$ $CSID$")


#include <bdeimp_dateutil.h>
#include <bsls_assert.h>

namespace BloombergLP {

static int min(int a, int b) { return a < b ? a : b; }

int bdetu_Month::numMonthsInRange(bdet_Month::Month month,
                                  const bdet_Date&  date1,
                                  const bdet_Date&  date2)
{
    if (date1 > date2) {
        return 0;
    }

    int count = 0;
    int y1, m1, day1NotUsed;
    int y2, m2, day2NotUsed;

    date1.getYearMonthDay(&y1, &m1, &day1NotUsed);
    date2.getYearMonthDay(&y2, &m2, &day2NotUsed);

    while (m1 != m2) {
        count += bdet_Month::Month(m2) == month;
        --m2;

        if (!m2) {
            --y2;
            m2 = 12;
        }
    }

    BSLS_ASSERT(m2 == m1);

    return count + y2 - y1 + (bdet_Month::Month(m2) == month);
}

bdet_Date bdetu_Month::nextMonth(bdet_Month::Month month,
                                 const bdet_Date&  date)
{
    int y, m, d;

    date.getYearMonthDay(&y, &m, &d);

    if (bdet_Month::Month(m) >= month) {
        ++y;
    }

    int day = min(d, bdeimp_DateUtil::lastDayOfMonth(y, month));

    return bdet_Date(y, month, day);
}

bdet_Date bdetu_Month::previousMonth(bdet_Month::Month month,
                                     const bdet_Date&  date)
{
    int y, m, d;

    date.getYearMonthDay(&y, &m, &d);

    if (bdet_Month::Month(m) <= month) {
        --y;
    }

    int day = min(d, bdeimp_DateUtil::lastDayOfMonth(y, month));

    return bdet_Date(y, month, day);
}

bdet_Date bdetu_Month::adjustMonth(bdet_Month::Month month,
                                   const bdet_Date&  date,
                                   int               count)
{
    BSLS_ASSERT(count != 0);

    int y, m, d;

    date.getYearMonthDay(&y, &m, &d);

    if (count > 0) {
        y += count - (bdet_Month::Month(m) < month);
    }
    else {                              // count < 0
        y += count + (bdet_Month::Month(m) > month);
    }

    int day = min(d, bdeimp_DateUtil::lastDayOfMonth(y, month));

    return bdet_Date(y, month, day);
}

bdet_Date bdetu_Month::ceilAdjustMonth(bdet_Month::Month month,
                                       const bdet_Date&  date,
                                       int               count)
{
    bdet_Date ceilDate = ceilMonth(month, date);

    if (0 == count) {
        return ceilDate;
    }

    bdet_Date d   = adjustMonth(month, ceilDate, count);
    int       y   = d.year();
    int       day = min(date.day(), bdeimp_DateUtil::lastDayOfMonth(y, month));

    return bdet_Date(y, month, day);
}

bdet_Date bdetu_Month::floorAdjustMonth(bdet_Month::Month month,
                                        const bdet_Date&  date,
                                        int               count)
{
    bdet_Date floorDate = floorMonth(month, date);

    if (0 == count) {
        return floorDate;
    }

    bdet_Date d   = adjustMonth(month, floorDate, count);
    int       y   = d.year();
    int       day = min(date.day(), bdeimp_DateUtil::lastDayOfMonth(y, month));

    return bdet_Date(y, month, day);
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
