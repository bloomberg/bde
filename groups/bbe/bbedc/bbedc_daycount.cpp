// bbedc_daycount.cpp           -*-C++-*-
#include <bdes_ident.h>
BDES_IDENT_RCSID(bbedc_daycount_cpp,"$Id$ $CSID$")


#include <bbedc_daycount.h>

#include <bbedc_actual360.h>
#include <bbedc_actual365fixed.h>
#include <bbedc_icmaactualactual.h>
#include <bbedc_isdaactualactual.h>
#include <bbedc_isma30360.h>
#include <bbedc_psa30360eom.h>
#include <bbedc_sia30360eom.h>
#include <bbedc_sia30360neom.h>

#include <bsls_assert.h>

namespace BloombergLP {

                        // ---------------------
                        // struct bbedc_DayCount
                        // ---------------------

// CLASS METHODS
int bbedc_DayCount::daysDiff(const bdet_Date&               beginDate,
                             const bdet_Date&               endDate,
                             bbedc_DayCountConvention::Type dayCountType)
{
    int numDays;

    switch (dayCountType) {
      case bbedc_DayCountConvention::BBEDC_ACTUAL_360: {
        numDays = bbedc_Actual360::daysDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_ACTUAL_365_FIXED: {
        numDays = bbedc_Actual365Fixed::daysDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_ICMA_ACTUAL_ACTUAL: {
        numDays = bbedc_IcmaActualActual::daysDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_ISDA_ACTUAL_ACTUAL: {
        numDays = bbedc_IsdaActualActual::daysDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_ISMA_ACTUAL_ACTUAL: {
// TBD
        BSLS_ASSERT(0 && "ISMA_ACTUAL_ACTUAL not supported by daysDiff");
//      numDays = bbedc_IsmaActualActual::daysDiff(beginDate, endDate);
        numDays = 0;
      } break;
      case bbedc_DayCountConvention::BBEDC_ISMA_30_360: {
        numDays = bbedc_Isma30360::daysDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_PSA_30_360_EOM: {
        numDays = bbedc_Psa30360eom::daysDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_SIA_30_360_EOM: {
        numDays = bbedc_Sia30360eom::daysDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_SIA_30_360_NEOM: {
        numDays = bbedc_Sia30360neom::daysDiff(beginDate, endDate);
      } break;
      default: {
        BSLS_ASSERT(0);
        numDays = 0;
      } break;
    }

    return numDays;
}

double bbedc_DayCount::yearsDiff(const bdet_Date&               beginDate,
                                 const bdet_Date&               endDate,
                                 bbedc_DayCountConvention::Type dayCountType)
{
    double numYears;

    switch (dayCountType) {
      case bbedc_DayCountConvention::BBEDC_ACTUAL_360: {
        numYears = bbedc_Actual360::yearsDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_ACTUAL_365_FIXED: {
        numYears = bbedc_Actual365Fixed::yearsDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_ICMA_ACTUAL_ACTUAL: {
        numYears = bbedc_IcmaActualActual::yearsDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_ISDA_ACTUAL_ACTUAL: {
        numYears = bbedc_IsdaActualActual::yearsDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_ISMA_ACTUAL_ACTUAL: {
// TBD
        BSLS_ASSERT(0 && "ISMA_ACTUAL_ACTUAL not supported by yearsDiff");
//      numYears = bbedc_IsmaActualActual::yearsDiff(beginDate, endDate);
        numYears = 0.0;
      } break;
      case bbedc_DayCountConvention::BBEDC_ISMA_30_360: {
        numYears = bbedc_Isma30360::yearsDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_PSA_30_360_EOM: {
        numYears = bbedc_Psa30360eom::yearsDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_SIA_30_360_EOM: {
        numYears = bbedc_Sia30360eom::yearsDiff(beginDate, endDate);
      } break;
      case bbedc_DayCountConvention::BBEDC_SIA_30_360_NEOM: {
        numYears = bbedc_Sia30360neom::yearsDiff(beginDate, endDate);
      } break;
      default: {
        BSLS_ASSERT(0 && "Unrecognized convention");
        numYears = 0.0;
      } break;
    }

    return numYears;
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
