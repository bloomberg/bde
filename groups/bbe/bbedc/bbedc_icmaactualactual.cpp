// bbedc_icmaactualactual.cpp               -*-C++-*-
#include <bdes_ident.h>
BDES_IDENT_RCSID(bbedc_icmaactualactual_cpp,"$Id$ $CSID$")


#include <bbedc_icmaactualactual.h>

#include <bdeimp_dateutil.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>

namespace BloombergLP {

                       // -----------------------------
                       // struct bbedc_IcmaActualActual
                       // -----------------------------

// CLASS METHODS
double bbedc_IcmaActualActual::yearsDiff(const bdet_Date& beginDateIn,
                                         const bdet_Date& endDateIn)
{
    bdet_Date beginDate(beginDateIn);
    bdet_Date endDate(  endDateIn);

    bool negationFlag = beginDate > endDate;
    if (negationFlag) {
        bsl::swap(beginDate, endDate);
    }
    BSLS_ASSERT(beginDate <= endDate);

    // get Pivot End (pe) date
    int peYY = beginDate.year();
    int peMM = endDate.month();
    int peDD = endDate.day();
    if (endDate.month() < beginDate.month()
     || (endDate.month() == beginDate.month()
      && endDate.day() < beginDate.day())) {
        ++peYY;
    }
    // get Pivot Begin (pb) date
    int pbYY = peYY - 1;
    int pbMM = peMM;
    int pbDD = peDD;
    if (2 == peMM && 29 == peDD && !bdet_Date::isValid(peYY, peMM, peDD)) {
        peDD = 28;
    }
    if (2 == pbMM && 29 == pbDD && !bdet_Date::isValid(pbYY, pbMM, pbDD)) {
        pbDD = 28;
    }
    bdet_Date pivotEndDate(  peYY, peMM, peDD);
    bdet_Date pivotBeginDate(pbYY, pbMM, pbDD);

    double daysInPivotYear = pivotEndDate - pivotBeginDate;

    double result = endDate.year() - pivotEndDate.year() +
                                  (pivotEndDate - beginDate) / daysInPivotYear;

    if (negationFlag) {
        result = -result;
    }

    return result;
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
