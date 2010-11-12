// bdet_dayofweek.cpp                                                 -*-C++-*-
#include <bdet_dayofweek.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdet_dayofweek_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_assert.h>

namespace BloombergLP {

BSLMF_ASSERT(bdet_DayOfWeek::BDET_SUN < bdet_DayOfWeek::BDET_SAT);

                            // --------------------
                            // class bdet_DayOfWeek
                            // --------------------

// CLASS METHODS
const char *bdet_DayOfWeek::toAscii(Day dayOfWeek)
{
#define CASE(X) case(BDET_ ## X): return #X

    switch(dayOfWeek) {
      CASE(SUN);
      CASE(MON);
      CASE(TUE);
      CASE(WED);
      CASE(THU);
      CASE(FRI);
      CASE(SAT);
      default:
        BSLS_ASSERT_SAFE("Unknown Enumerator" && 0);
        return "(* Unknown Enumerator *)";
    }

#undef CASE
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
