// bdeimp_voidptrhash.cpp              -*-C++-*-
#include <bdeimp_voidptrhash.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeimp_voidptrhash_cpp,"$Id$ $CSID$")

#include <bsls_platformutil.h>                  // for testing only

namespace BloombergLP {

struct bdeimp_VoidPtrHash_Assertions {
    char assertion[4 == sizeof(void *) || 8 == sizeof(void *)];
};

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
