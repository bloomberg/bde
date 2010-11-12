// bsls_alignment.cpp                                                 -*-C++-*-

#include <bsls_alignment.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

                        // ---------------------
                        // struct bsls_Alignment
                        // ---------------------

// CLASS METHODS
const char *bsls_Alignment::toAscii(bsls_Alignment::Strategy value)
{
#define CASE(X) case(BSLS_ ## X): return #X;

    switch (value) {
      CASE(MAXIMUM)
      CASE(NATURAL)
      default: return "(* UNKNOWN *)";
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
