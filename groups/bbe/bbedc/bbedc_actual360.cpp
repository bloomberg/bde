// bbedc_actual360.cpp          -*-C++-*-
#include <bdes_ident.h>
BDES_IDENT_RCSID(bbedc_actual360_cpp,"$Id$ $CSID$")


#include <bbedc_actual360.h>

#include <bdet_date.h>

namespace BloombergLP {

                        // ----------------------
                        // struct bbedc_Actual360
                        // ----------------------

// CLASS METHODS
double bbedc_Actual360::yearsDiff(const bdet_Date& beginDate,
                                  const bdet_Date& endDate)
{
    return (endDate - beginDate) / 360.0;
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
