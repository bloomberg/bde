// bcema_pooledblobbufferfactory.h  -*-C++-*-
#ifndef INCLUDED_BCEMA_POOLEDBLOBBUFFERFACTORY
#define INCLUDED_BCEMA_POOLEDBLOBBUFFERFACTORY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")


//@PURPOSE: Provide a concrete implementation of 'bcema_BlobBufferFactory'.
//
//@CLASSES:
//  bcema_PooledBlobBufferFactory: mechanism for pooling 'bcema_BlobBuffer's
//
//@SEE_ALSO: bcema_blob, bcema_pool
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides a mechanism for allocating
// 'bcema_BlobBuffer' objects of a fixed specified size.  The size is passed at
// construction of the 'bcema_PooledBlobBufferFactory' instance.  A
// 'bcema_BlobBuffer' is basically a shared pointer to a buffer of 'char' of
// the prescribed size.  Thus it is generally more efficient to create them
// with a uniform size, for the same reason as a pool is more efficient than a
// general-purpose memory allocator.  In order to gain further efficiency, this
// factory allocates the shared pointer representation together with the buffer
// (contiguously).
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_BLOB
#include <bcema_blob.h>
#endif

#ifndef INCLUDED_BCEMA_POOLALLOCATOR
#include <bcema_poolallocator.h>
#endif

namespace BloombergLP {

                     // ===================================
                     // class bcema_PooledBlobBufferFactory
                     // ===================================

class bcema_PooledBlobBufferFactory: public bcema_BlobBufferFactory {
    // This class implements the 'bcema_BlobBufferFactory' protocol and
    // provides a mechanism for allocating 'bcema_BlobBuffer' objects of a
    // fixed size passed at construction.

    // DATA
    int                 d_bufferSize;  // size of allocated blob buffers

    bcema_PoolAllocator d_spPool;      // pool used to allocate shared pointers
                                       // and buffers contiguously
  public:
    // CREATORS
    bcema_PooledBlobBufferFactory(int              bufferSize,
                                  bslma_Allocator *basicAllocator=0);
        // Create a pooled factory for allocating 'bcema_BlobBuffer' objects of
        // the specified 'bufferSize'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~bcema_PooledBlobBufferFactory();
        // Destroy this factory.

    // MANIPULATORS
    void allocate(bcema_BlobBuffer *buffer);
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
                     // class bcema_PooledBlobBufferFactory
                     // -----------------------------------

// ACCESSORS
inline
int bcema_PooledBlobBufferFactory::bufferSize() const
{
    return d_bufferSize;
}

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
