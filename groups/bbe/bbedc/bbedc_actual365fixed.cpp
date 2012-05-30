// bbedc_actual365fixed.cpp          -*-C++-*-
#include <bdes_ident.h>
BDES_IDENT_RCSID(bbedc_actual365fixed_cpp,"$Id$ $CSID$")


#include <bbedc_actual365fixed.h>

#include <bdet_date.h>

namespace BloombergLP {

                        // ---------------------------
                        // struct bbedc_Actual365Fixed
                        // ---------------------------

// CLASS METHODS
double bbedc_Actual365Fixed::yearsDiff(const bdet_Date& beginDate,
                                       const bdet_Date& endDate)
{
    return (endDate - beginDate) / 365.0;
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
