// bdex_byteoutstreamformatter.cpp               -*-C++-*-
#include <bdex_byteoutstreamformatter.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdex_byteoutstreamformatter_cpp,"$Id$ $CSID$")

#include <bsls_platformutil.h>
#include <bslmf_assert.h>

namespace BloombergLP {

BSLMF_ASSERT(8 == sizeof(bsls_PlatformUtil::Int64));
BSLMF_ASSERT(4 == sizeof(int));
BSLMF_ASSERT(2 == sizeof(short));
BSLMF_ASSERT(8 == sizeof(double));
BSLMF_ASSERT(4 == sizeof(float));

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
