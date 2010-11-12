// bdema_multipoolallocator.cpp                                       -*-C++-*-
#include <bdema_multipoolallocator.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_multipoolallocator_cpp,"$Id$ $CSID$")

#include <bdema_bufferedsequentialallocator.h>  // for testing only

#include <bsls_performancehint.h>

namespace BloombergLP {

                     // ------------------------------
                     // class bdema_MultipoolAllocator
                     // ------------------------------

// CREATORS
bdema_MultipoolAllocator::~bdema_MultipoolAllocator()
{
}

// MANIPULATORS
void *bdema_MultipoolAllocator::allocate(size_type size)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;                                                     // RETURN
    }

    return d_multipool.allocate(size);
}

void bdema_MultipoolAllocator::deallocate(void *address)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(address != 0)) {
        d_multipool.deallocate(address);
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
}

void bdema_MultipoolAllocator::reserveCapacity(size_type size,
                                               size_type numObjects)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return;                                                       // RETURN
    }

    d_multipool.reserveCapacity(size, numObjects);
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
