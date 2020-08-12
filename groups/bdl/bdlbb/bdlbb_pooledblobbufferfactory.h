// bdlbb_pooledblobbufferfactory.h                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLBB_POOLEDBLOBBUFFERFACTORY
#define INCLUDED_BDLBB_POOLEDBLOBBUFFERFACTORY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a concrete implementation of 'bdlbb::BlobBufferFactory'.
//
//@CLASSES:
//  bdlbb::PooledBlobBufferFactory: mechanism pooling 'bdlbb::BlobBuffer's
//
//@SEE_ALSO: bdlbb_blob, bdlma_concurrentpool
//
//@DESCRIPTION: This component provides a mechanism for allocating
// 'bdlbb::BlobBuffer' objects of a fixed specified size.  The size is passed
// at construction of the 'bdlbb::PooledBlobBufferFactory' instance.  A
// 'bdlbb::BlobBuffer' is basically a shared pointer to a buffer of 'char' of
// the prescribed size.  Thus it is generally more efficient to create them
// with a uniform size, for the same reason as a pool is more efficient than a
// general-purpose memory allocator.  In order to gain further efficiency, this
// factory allocates the shared pointer representation together with the buffer
// (contiguously).
//
///Potential Lifetime Issues
///-------------------------
// Be aware that the destruction of a 'bdlbb::PooledBlobBufferFactory' object
// releases all the 'BlobBuffer' objects allocated by that factory. A common
// misconception is that, because of the use of 'shared_ptr', the data referred
// to in 'BlobBuffer' objects created will remain valid until the last shared
// reference is destroyed, even if a reference outlives the
// 'PooledBlobBufferFactory' that created it. This is *not* the case.
// Destroying a 'PooledBlobBufferFactory' releases any memory created by that
// pool, even if shared references remain to the data, and it is therefore
// undefined behavior to use any 'BlobBuffer' created by a pool after that pool
// is destroyed. A user must clearly understand the lifetime of memory
// allocated by a subsystem, and scope the lifetime of the
// 'PooledBlobBufferFactory' to be greater than the active lifetime of that
// subsystem.

#include <bdlscm_version.h>

#include <bdlbb_blob.h>

#include <bdlma_concurrentpoolallocator.h>

namespace BloombergLP {
namespace bdlbb {

                       // =============================
                       // class PooledBlobBufferFactory
                       // =============================

class PooledBlobBufferFactory : public BlobBufferFactory {
    // This class implements the 'BlobBufferFactory' protocol and provides a
    // mechanism for allocating 'BlobBuffer' objects of a fixed size passed at
    // construction.

    // DATA
    int d_bufferSize;                         // size of allocated blob buffers

    bdlma::ConcurrentPoolAllocator d_spPool;  // pool used to allocate shared
                                              // pointers and buffers
                                              // contiguously
  public:
    // CREATORS
    PooledBlobBufferFactory(int               bufferSize,
                            bslma::Allocator *basicAllocator = 0);
    PooledBlobBufferFactory(int                          bufferSize,
                            bsls::BlockGrowth::Strategy  growthStrategy,
                            bslma::Allocator            *basicAllocator = 0);
    PooledBlobBufferFactory(int                          bufferSize,
                            bsls::BlockGrowth::Strategy  growthStrategy,
                            int                          maxBlocksPerChunk,
                            bslma::Allocator            *basicAllocator = 0);
        // Create a pooled factory for allocating 'BlobBuffer' objects of the
        // specified 'bufferSize'.  Optionally specify a 'growthStrategy' used
        // to control the growth of internal memory chunks (from which memory
        // blocks are dispensed).  If 'growthStrategy' is not specified,
        // geometric growth is used.  If 'growthStrategy' is specified,
        // optionally specify a 'maxBlocksPerChunk', indicating the maximum
        // number of blocks to be allocated at once when the underlying pool
        // must be replenished.  If 'maxBlocksPerChunk' is not specified, an
        // implementation-defined value is used.  If geometric growth is used,
        // the chunk size grows starting at the value returned by 'blockSize',
        // doubling in size until the size is exactly
        // 'blockSize() * maxBlocksPerChunk'.  If constant growth is used, the
        // chunk size is always 'maxBlocksPerChunk'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 < bufferSize', and '1 <= maxBlocksPerChunk'.

    ~PooledBlobBufferFactory();
        // Destroy this factory.  This operation releases all 'BlobBuffer'
        // objects allocated via this factory.

    // MANIPULATORS
    void allocate(BlobBuffer *buffer);
        // Allocate a new buffer with the buffer size specified at construction
        // and load it into the specified 'buffer'.  Note that destruction of
        // the 'bdlbb::PooledBlobBufferFactory' object releases all
        // 'BlobBuffer' objects allocated via this factory.

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
// Copyright 2018 Bloomberg Finance L.P.
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
