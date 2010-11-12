// bsls_blockgrowth.cpp                                               -*-C++-*-
#include <bsls_blockgrowth.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

                        // -----------------------
                        // struct bsls_BlockGrowth
                        // -----------------------

// CLASS METHODS
const char *bsls_BlockGrowth::toAscii(bsls_BlockGrowth::Strategy value)
{
#define CASE(X) case(BSLS_ ## X): return #X;

    switch (value) {
      CASE(GEOMETRIC)
      CASE(CONSTANT)
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
