// bdeimp_inthash.cpp              -*-C++-*-
#include <bdeimp_inthash.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeimp_inthash_cpp,"$Id$ $CSID$")

#include <bsls_platformutil.h>                  // for testing only

namespace BloombergLP {

struct bdeimp_IntHash_Assertions {
    char assertion[4 == sizeof(int) || 8 == sizeof(int)];
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
