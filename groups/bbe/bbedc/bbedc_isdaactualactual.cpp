// bbedc_isdaactualactual.cpp               -*-C++-*-
#include <bdes_ident.h>
BDES_IDENT_RCSID(bbedc_isdaactualactual_cpp,"$Id$ $CSID$")


#include <bbedc_isdaactualactual.h>

#include <bdeimp_dateutil.h>

namespace BloombergLP {

                       // -----------------------------
                       // struct bbedc_IsdaActualActual
                       // -----------------------------

// CLASS METHODS
double bbedc_IsdaActualActual::yearsDiff(const bdet_Date& beginDate,
                                         const bdet_Date& endDate)
{
    const int beginYear = beginDate.year();
    const int endYear   = endDate.year();
    const double daysInBeginYear =
                                365.0 + bdeimp_DateUtil::isLeapYear(beginYear);
    const double daysInEndYear   =
                                365.0 + bdeimp_DateUtil::isLeapYear(endYear);

    return (double)(endYear - beginYear - 1)
       + (double)(bdet_Date(beginYear + 1, 1, 1) - beginDate) / daysInBeginYear
       + (double)(endDate -         bdet_Date(endYear, 1, 1)) / daysInEndYear;
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
