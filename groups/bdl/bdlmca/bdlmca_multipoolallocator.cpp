// bdlmca_multipoolallocator.cpp                                       -*-C++-*-
#include <bdlmca_multipoolallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmca_multipoolallocator_cpp,"$Id$ $CSID$")

#include <bsls_performancehint.h>

namespace BloombergLP {

namespace bdlmca {
                     // ------------------------------
                     // class MultipoolAllocator
                     // ------------------------------

// CREATORS
MultipoolAllocator::~MultipoolAllocator()
{
}

// MANIPULATORS
void *MultipoolAllocator::allocate(size_type size)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;                                                     // RETURN
    }

    return d_multipool.allocate(size);
}

void MultipoolAllocator::deallocate(void *address)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(address != 0)) {
        d_multipool.deallocate(address);
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
}

void MultipoolAllocator::release()
{
    d_multipool.release();
}

void MultipoolAllocator::reserveCapacity(size_type size,
                                               size_type numObjects)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return;                                                       // RETURN
    }

    d_multipool.reserveCapacity(size, numObjects);
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
