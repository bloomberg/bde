// bbedc_psa30360eom.cpp                -*-C++-*-
#include <bdes_ident.h>
BDES_IDENT_RCSID(bbedc_psa30360eom_cpp,"$Id$ $CSID$")


#include <bbedc_psa30360eom.h>

#include <bdeimp_dateutil.h>
#include <bdet_date.h>

namespace BloombergLP {

// STATIC METHODS
inline
static bool isLastDayOfFebruary(int year, int month, int day)
    // Return 'true' if the specified 'day' of the specified 'month' in the
    // specified 'year' is the last day of February for that 'year', and
    // 'false' otherwise.
{
    return 2 == month
           && (29 == day || (28 == day && !bdeimp_DateUtil::isLeapYear(year)));
}

inline
static int max(int lhs, int rhs)
    // Return the maximum of the specified 'lhs' and 'rhs' values.
{
    return lhs > rhs ? lhs : rhs;
}

static int computeDaysDiff(const bdet_Date& beginDate,
                           const bdet_Date& endDate)
    // Return the number of days between the specified  'beginDate' and
    // 'endDate' according to the PSA 30/360 end-of-month day-count convention.
    // If 'beginDate <= endDate', then the result is non-negative.  Note that
    // reversing the order of 'beginDate' and 'endDate' negates the result.
{
    int y1, m1, d1, y2, m2, d2;
    int negationFlag = beginDate > endDate;
    if (negationFlag) {
        endDate.getYearMonthDay(  &y1, &m1, &d1);
        beginDate.getYearMonthDay(&y2, &m2, &d2);
    } else {
        beginDate.getYearMonthDay(&y1, &m1, &d1);
        endDate.getYearMonthDay(  &y2, &m2, &d2);
    }

    // This implementation is coded to look exactly like the description
    // as it appears in the PSA doc:

    if (isLastDayOfFebruary(y1, m1, d1)) {
      d1 = 30;
    }
    if (31 == d1) {
      d1 = 30;
    }

    if (30 == d1 && 31 == d2) {
      d2 = 30;
    }

    int result = max((y2 - y1) * 360 + (m2 - m1) * 30 + d2 - d1, 0);

    if (negationFlag) {
        result = -result;
    }

    return result;
}

                        // ------------------------
                        // struct bbedc_Psa30360eom
                        // ------------------------

// CLASS METHODS
int bbedc_Psa30360eom::daysDiff(const bdet_Date& beginDate,
                                const bdet_Date& endDate)
{
    return computeDaysDiff(beginDate, endDate);
}

double bbedc_Psa30360eom::yearsDiff(const bdet_Date& beginDate,
                                    const bdet_Date& endDate)
{
    return (double)computeDaysDiff(beginDate, endDate) / 360.0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
