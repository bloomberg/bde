// balm_metricrecord.cpp      -*-C++-*-
#include <balm_metricrecord.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_metricrecord_cpp,"$Id$ $CSID$")

#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                          // -----------------------
                          // class balm::MetricRecord
                          // -----------------------

// PUBLIC CONSTANTS
const double balm::MetricRecord::k_DEFAULT_MIN =
                                       bsl::numeric_limits<double>::infinity();
const double balm::MetricRecord::k_DEFAULT_MAX =
                                      -bsl::numeric_limits<double>::infinity();
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
const double balm::MetricRecord::DEFAULT_MIN =
                                       bsl::numeric_limits<double>::infinity();
const double balm::MetricRecord::DEFAULT_MAX =
                                      -bsl::numeric_limits<double>::infinity();
#endif

namespace balm {
// ACCESSORS
bsl::ostream& MetricRecord::print(bsl::ostream& stream) const
{
    stream << "[ " << d_metricId << ": " << d_count
           << " " << d_total
           << " " << d_min
           << " " << d_max
           << " ]";
    return stream;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
