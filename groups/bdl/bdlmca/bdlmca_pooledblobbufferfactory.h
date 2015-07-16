// bdlmca_pooledblobbufferfactory.h  -*-C++-*-
#ifndef INCLUDED_BDLMCA_POOLEDBLOBBUFFERFACTORY
#define INCLUDED_BDLMCA_POOLEDBLOBBUFFERFACTORY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide a concrete implementation of 'bdlmca::BlobBufferFactory'.
//
//@CLASSES:
//  bdlmca::PooledBlobBufferFactory: mechanism for pooling 'bdlmca::BlobBuffer's
//
//@SEE_ALSO: bdlmca_blob, bdlma_concurrentpool
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides a mechanism for allocating
// 'bdlmca::BlobBuffer' objects of a fixed specified size.  The size is passed at
// construction of the 'bdlmca::PooledBlobBufferFactory' instance.  A
// 'bdlmca::BlobBuffer' is basically a shared pointer to a buffer of 'char' of
// the prescribed size.  Thus it is generally more efficient to create them
// with a uniform size, for the same reason as a pool is more efficient than a
// general-purpose memory allocator.  In order to gain further efficiency, this
// factory allocates the shared pointer representation together with the buffer
// (contiguously).
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMCA_BLOB
#include <bdlmca_blob.h>
#endif

#ifndef INCLUDED_BDLMA_CONCURRENTPOOLALLOCATOR
#include <bdlma_concurrentpoolallocator.h>
#endif

namespace BloombergLP {

namespace bdlmca {
                     // ===================================
                     // class PooledBlobBufferFactory
                     // ===================================

class PooledBlobBufferFactory: public BlobBufferFactory {
    // This class implements the 'BlobBufferFactory' protocol and
    // provides a mechanism for allocating 'BlobBuffer' objects of a
    // fixed size passed at construction.

    // DATA
    int                 d_bufferSize;  // size of allocated blob buffers

    bdlma::ConcurrentPoolAllocator d_spPool;      // pool used to allocate shared pointers
                                       // and buffers contiguously
  public:
    // CREATORS
    PooledBlobBufferFactory(int               bufferSize,
                                  bslma::Allocator *basicAllocator=0);
        // Create a pooled factory for allocating 'BlobBuffer' objects of
        // the specified 'bufferSize'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~PooledBlobBufferFactory();
        // Destroy this factory.

    // MANIPULATORS
    void allocate(BlobBuffer *buffer);
        // Allocate a new buffer with the buffer size specified at construction
        // and load it into the specified 'buffer'.

    // ACCESSORS
    int bufferSize() const;
        // Return the buffer size specified at construction of this factory.
};

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                     // -----------------------------------
                     // class PooledBlobBufferFactory
                     // -----------------------------------

// ACCESSORS
inline
int PooledBlobBufferFactory::bufferSize() const
{
    return d_bufferSize;
}
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
