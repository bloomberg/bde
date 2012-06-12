// bbedc_isma30360.cpp          -*-C++-*-
#include <bdes_ident.h>
BDES_IDENT_RCSID(bbedc_isma30360_cpp,"$Id$ $CSID$")


///IMPLEMENTATION NOTES
///--------------------
// This component uses a static method 'computeDaysDiff' to enhance insulation.

#include <bbedc_isma30360.h>

#include <bdet_date.h>

namespace BloombergLP {

// STATIC METHODS
static int computeDaysDiff(const bdet_Date& beginDate,
                           const bdet_Date& endDate)
    // Return the number of days between the specified  'beginDate' and
    // 'endDate' according to the ISMA 30/360 day-count convention.  If
    // 'beginDate <= endDate', then the result is non-negative.  Note that
    // reversing the order of 'beginDate' and 'endDate' negates the result.
{
    int yBegin, mBegin, dBegin, yEnd, mEnd, dEnd;

    beginDate.getYearMonthDay(&yBegin, &mBegin, &dBegin);
    endDate.getYearMonthDay(  &yEnd,   &mEnd,   &dEnd);

    if (31 == dBegin) {
        dBegin = 30;
    }

    if (31 == dEnd) {
        dEnd = 30;
    }

    return (yEnd - yBegin) * 360 + (mEnd - mBegin) * 30 + dEnd - dBegin;
}

                        // ----------------------
                        // struct bbedc_Isma30360
                        // ----------------------

// CLASS METHODS
int bbedc_Isma30360::daysDiff(const bdet_Date& beginDate,
                              const bdet_Date& endDate)
{
    return computeDaysDiff(beginDate, endDate);
}

double bbedc_Isma30360::yearsDiff(const bdet_Date& beginDate,
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
