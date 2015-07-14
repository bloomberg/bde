// bdlb_xxxbitutil.cpp                                                   -*-C++-*-
#include <bdlb_xxxbitutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_xxxbitutil_cpp,"$Id$ $CSID$")


#include <bslmf_assert.h>

namespace BloombergLP {

BSLMF_ASSERT(4 == sizeof(int));
BSLMF_ASSERT(8 == sizeof(bsls::Types::Int64));

namespace bdlb {
                        // -------------------
                        // struct BitUtil
                        // -------------------

// PRIVATE CLASS METHODS
int BitUtil::privateFind1AtLargestIndex(int srcInteger)
{
    // Note that it doesn't matter whether the right shifts sign extend or not.

    srcInteger |= srcInteger >> 16;
    srcInteger |= srcInteger >>  8;
    srcInteger |= srcInteger >>  4;
    srcInteger |= srcInteger >>  2;
    srcInteger |= srcInteger >>  1;
    return numSetOne(srcInteger) - 1;
}

int BitUtil::privateFind1AtLargestIndex64(Int64 srcInteger)
{
    // Note that it doesn't matter whether the right shifts sign extend or not.

    srcInteger |= srcInteger >> 32;
    srcInteger |= srcInteger >> 16;
    srcInteger |= srcInteger >>  8;
    srcInteger |= srcInteger >>  4;
    srcInteger |= srcInteger >>  2;
    srcInteger |= srcInteger >>  1;
    return numSetOne64(srcInteger) - 1;
}

int BitUtil::privateFind1AtSmallestIndex(int srcInteger)
{
    srcInteger |= srcInteger << 16;
    srcInteger |= srcInteger <<  8;
    srcInteger |= srcInteger <<  4;
    srcInteger |= srcInteger <<  2;
    srcInteger |= srcInteger <<  1;
    return numSetZero(srcInteger);
}

int BitUtil::privateFind1AtSmallestIndex64(Int64 srcInteger)
{
    srcInteger |= srcInteger << 32;
    srcInteger |= srcInteger << 16;
    srcInteger |= srcInteger <<  8;
    srcInteger |= srcInteger <<  4;
    srcInteger |= srcInteger <<  2;
    srcInteger |= srcInteger <<  1;
    return numSetZero64(srcInteger);
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
