// baem_metricrecord.cpp      -*-C++-*-
#include <baem_metricrecord.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_metricrecord_cpp,"$Id$ $CSID$")

#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                          // -----------------------
                          // class baem_MetricRecord
                          // -----------------------

// PUBLIC CONSTANTS
const double baem_MetricRecord::DEFAULT_MIN =
                                       bsl::numeric_limits<double>::infinity();
const double baem_MetricRecord::DEFAULT_MAX =
                                      -bsl::numeric_limits<double>::infinity();

// ACCESSORS
bsl::ostream& baem_MetricRecord::print(bsl::ostream& stream) const
{
    stream << "[ " << d_metricId << ": " << d_count
           << " " << d_total
           << " " << d_min
           << " " << d_max
           << " ]";
    return stream;
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
