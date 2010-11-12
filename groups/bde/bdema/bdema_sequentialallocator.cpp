// bdema_sequentialallocator.cpp                                      -*-C++-*-
#include <bdema_sequentialallocator.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_sequentialallocator_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_performancehint.h>

namespace BloombergLP {

                      // -------------------------------
                      // class bdema_SequentialAllocator
                      // -------------------------------

// CREATORS
bdema_SequentialAllocator::~bdema_SequentialAllocator()
{
}

// MANIPULATORS
void *bdema_SequentialAllocator::allocate(bsls_PlatformUtil::size_type size)
{
    BSLS_ASSERT(0 <= size);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;
    }

    return d_sequentialPool.allocate(size);
}

void *bdema_SequentialAllocator::allocateAndExpand(
                                            bsls_PlatformUtil::size_type *size)
{
    BSLS_ASSERT(size);
    BSLS_ASSERT(0 <= *size);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == *size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;
    }

    return d_sequentialPool.allocateAndExpand(size);
}

void bdema_SequentialAllocator::reserveCapacity(int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == numBytes)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return;
    }

    d_sequentialPool.reserveCapacity(numBytes);
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
