// baem_integercollector.cpp                                          -*-C++-*-
#include <baem_integercollector.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_integercollector_cpp,"$Id$ $CSID$")

#include <bsl_climits.h>

namespace BloombergLP {

                          // ---------------------------
                          // class baem_IntegerCollector
                          // ---------------------------

// PUBLIC CONSTANTS
const int baem_IntegerCollector::DEFAULT_MIN = INT_MAX;
const int baem_IntegerCollector::DEFAULT_MAX = INT_MIN;

// MANIPULATORS
void baem_IntegerCollector::loadAndReset(baem_MetricRecord *record)
{
    int                      count;
    bsls_PlatformUtil::Int64 total;
    int                      min;
    int                      max;
    {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
        count = d_count;
        total = d_total;
        min   = d_min;
        max   = d_max;

        d_count = 0;
        d_total = 0;
        d_min   = DEFAULT_MIN;
        d_max   = DEFAULT_MAX;
    }
    // Perform the conversion to double values outside of the lock.
    record->metricId() = d_metricId;
    record->count()    = count;
    record->total()    = total;
    record->min()      = (DEFAULT_MIN == min)
                       ? baem_MetricRecord::DEFAULT_MIN
                       : min;
    record->max()      = (DEFAULT_MAX == max)
                       ? baem_MetricRecord::DEFAULT_MAX
                       : max;
}

// ACCESSORS
void baem_IntegerCollector::load(baem_MetricRecord *record) const
{
    int                      count;
    bsls_PlatformUtil::Int64 total;
    int                      min;
    int                      max;

    {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
        count = d_count;
        total = d_total;
        min   = d_min;
        max   = d_max;
    }

    // Perform the conversion to double values outside of the lock.
    record->metricId() = d_metricId;
    record->count()    = count;
    record->total()    = total;
    record->min()      = (DEFAULT_MIN == min)
                       ? baem_MetricRecord::DEFAULT_MIN
                       : min;
    record->max()      = (DEFAULT_MAX == max)
                       ? baem_MetricRecord::DEFAULT_MAX
                       : max;
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
