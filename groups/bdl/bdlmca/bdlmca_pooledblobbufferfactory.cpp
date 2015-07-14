// bdlmca_pooledblobbufferfactory.cpp        -*-C++-*-
#include <bdlmca_pooledblobbufferfactory.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmca_pooledblobbufferfactory_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace bdlmca {
                     // -----------------------------------
                     // class PooledBlobBufferFactory
                     // -----------------------------------

// CREATORS
PooledBlobBufferFactory::PooledBlobBufferFactory(
        int               bufferSize,
        bslma::Allocator *basicAllocator)
: d_bufferSize(bufferSize)
, d_spPool(basicAllocator)
{
}

PooledBlobBufferFactory::~PooledBlobBufferFactory()
{
}

// MANIPULATORS
void PooledBlobBufferFactory::allocate(BlobBuffer *buffer)
{
    buffer->reset(bslstl::SharedPtrUtil::createInplaceUninitializedBuffer(
                                               d_bufferSize, &d_spPool),
                  d_bufferSize);
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
