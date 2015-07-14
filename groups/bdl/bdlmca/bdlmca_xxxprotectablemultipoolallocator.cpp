// bdlmca_xxxprotectablemultipoolallocator.cpp    -*-C++-*-
#include <bdlmca_xxxprotectablemultipoolallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmca_xxxprotectablemultipoolallocator_cpp,"$Id$ $CSID$")

#include <bsls_performancehint.h>

namespace BloombergLP {

namespace bdlmca {
                // -----------------------------------------
                // class ProtectableMultipoolAllocator
                // -----------------------------------------

// MANIPULATORS
void *ProtectableMultipoolAllocator::allocate(size_type size)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;                                                     // RETURN
    }

    return d_multipool.allocate(size);
}

void ProtectableMultipoolAllocator::deallocate(void *address)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(address != 0)) {
        d_multipool.deallocate(address);
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
}

void ProtectableMultipoolAllocator::release()
{
    return d_multipool.release();
}

void ProtectableMultipoolAllocator::reserveCapacity(size_type size,
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
