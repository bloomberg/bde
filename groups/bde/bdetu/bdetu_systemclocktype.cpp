// bdetu_systemclocktype.cpp                                          -*-C++-*-
#include <bdetu_systemclocktype.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdetu_systemclocktype_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_assert.h>

namespace BloombergLP {

BSLMF_ASSERT(bdetu_SystemClockType::e_REALTIME <
                                           bdetu_SystemClockType::e_MONOTONIC);

                            // ---------------------------
                            // class bdetu_SystemClockType
                            // ---------------------------

// CLASS METHODS
const char *bdetu_SystemClockType::toAscii(bdetu_SystemClockType::Type clock)
{
#define CASE(X) case(e_ ## X): return #X

  switch(clock) {
    CASE(REALTIME);
    CASE(MONOTONIC);
    default:
      BSLS_ASSERT_SAFE("Unknown Enumerator" && 0);
      return "(* Unknown Enumerator *)";
  }

#undef CASE
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
