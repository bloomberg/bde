// bbedc_daycountconvention.cpp                                       -*-C++-*-
#include <bbedc_daycountconvention.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bbedc_daycountconvention_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

                     // -------------------------------
                     // struct bbedc_DayCountConvention
                     // -------------------------------

// CLASS METHODS
const char *bbedc_DayCountConvention::toAscii(
                                           bbedc_DayCountConvention::Type type)
{
#define CASE(X) case(BBEDC_ ## X): return #X

    switch (type) {
      CASE(ACTUAL_360);
      CASE(ACTUAL_365_FIXED);
      CASE(ICMA_ACTUAL_ACTUAL);
      CASE(ISDA_ACTUAL_ACTUAL);
      CASE(ISMA_ACTUAL_ACTUAL);
      CASE(ISMA_30_360);
      CASE(SIA_30_360_EOM);
      CASE(SIA_30_360_NEOM);
      CASE(PSA_30_360_EOM);
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

// FREE OPERATORS
bsl::ostream &operator<<(bsl::ostream&                  output,
                         bbedc_DayCountConvention::Type type)
{
    return output << bbedc_DayCountConvention::toAscii(type);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
