// baem_metricid.cpp      -*-C++-*-
#include <baem_metricid.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_metricid_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

                   // -------------------
                   // class baem_MetricId
                   // -------------------

// ACCESSORS
bsl::ostream& baem_MetricId::print(bsl::ostream& stream) const
{
    if (0 == d_description_p) {
        stream << "INVALID_ID";
    }
    else {
        stream << *d_description_p;
    }
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
