// bcema_pooledblobbufferfactory.cpp        -*-C++-*-
#include <bcema_pooledblobbufferfactory.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcema_pooledblobbufferfactory_cpp,"$Id$ $CSID$")

namespace BloombergLP {

                     // -----------------------------------
                     // class bcema_PooledBlobBufferFactory
                     // -----------------------------------

// CREATORS
bcema_PooledBlobBufferFactory::bcema_PooledBlobBufferFactory(
        int              bufferSize,
        bslma_Allocator *basicAllocator)
: d_bufferSize(bufferSize)
, d_spPool(basicAllocator)
{
}

bcema_PooledBlobBufferFactory::~bcema_PooledBlobBufferFactory()
{
}

// MANIPULATORS
void bcema_PooledBlobBufferFactory::allocate(bcema_BlobBuffer *buffer)
{
    buffer->reset(bcema_SharedPtrUtil::createInplaceUninitializedBuffer(
                                               d_bufferSize, &d_spPool),
                  d_bufferSize);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
