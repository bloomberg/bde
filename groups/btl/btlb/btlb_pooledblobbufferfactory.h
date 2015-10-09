// btlb_pooledblobbufferfactory.h                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLB_POOLEDBLOBBUFFERFACTORY
#define INCLUDED_BTLB_POOLEDBLOBBUFFERFACTORY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a concrete implementation of 'btlb::BlobBufferFactory'.
//
//@CLASSES:
//  btlb::PooledBlobBufferFactory: mechanism for pooling 'btlb::BlobBuffer's
//
//@SEE_ALSO: btlb_blob, bdlma_concurrentpool
//
//@DESCRIPTION: This component provides a mechanism for allocating
// 'btlb::BlobBuffer' objects of a fixed specified size.  The size is passed at
// construction of the 'btlb::PooledBlobBufferFactory' instance.  A
// 'btlb::BlobBuffer' is basically a shared pointer to a buffer of 'char' of
// the prescribed size.  Thus it is generally more efficient to create them
// with a uniform size, for the same reason as a pool is more efficient than a
// general-purpose memory allocator.  In order to gain further efficiency, this
// factory allocates the shared pointer representation together with the buffer
// (contiguously).

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BTLB_BLOB
#include <btlb_blob.h>
#endif

#ifndef INCLUDED_BDLMA_CONCURRENTPOOLALLOCATOR
#include <bdlma_concurrentpoolallocator.h>
#endif

namespace BloombergLP {
namespace btlb {

                      // =============================
                      // class PooledBlobBufferFactory
                      // =============================

class PooledBlobBufferFactory: public BlobBufferFactory {
    // This class implements the 'BlobBufferFactory' protocol and provides a
    // mechanism for allocating 'BlobBuffer' objects of a fixed size passed at
    // construction.

    // DATA
    int                 d_bufferSize;         // size of allocated blob buffers

    bdlma::ConcurrentPoolAllocator d_spPool;  // pool used to allocate shared
                                              // pointers and buffers
                                              // contiguously
  public:
    // CREATORS
    PooledBlobBufferFactory(int               bufferSize,
                            bslma::Allocator *basicAllocator=0);
        // Create a pooled factory for allocating 'BlobBuffer' objects of the
        // specified 'bufferSize'.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

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
//                             INLINE DEFINITIONS
// ============================================================================

                      // -----------------------------
                      // class PooledBlobBufferFactory
                      // -----------------------------

// ACCESSORS
inline
int PooledBlobBufferFactory::bufferSize() const
{
    return d_bufferSize;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
