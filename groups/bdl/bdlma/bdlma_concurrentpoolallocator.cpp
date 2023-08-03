// bdlma_concurrentpoolallocator.cpp                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_concurrentpoolallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_concurrentpoolallocator_cpp,"$Id$ $CSID$")

#include <bdlma_guardingallocator.h>  // for testing only

#include <bslmt_threadutil.h>

#include <bslma_default.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>

// CONSTANTS
enum {
    k_MAX_CHUNK_SIZE = 32  // maximum number of blocks per chunk
};

// STATIC METHODS
static inline
BloombergLP::bsls::Types::size_type calculateMaxAlignedSize(
                                 BloombergLP::bsls::Types::size_type totalSize)
{
    using namespace BloombergLP;

    const bsls::Types::size_type objectAlignmentMin =
                                   bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1;
    const bsls::Types::size_type ret =
                      (totalSize + objectAlignmentMin) & ~(objectAlignmentMin);
    BSLS_ASSERT(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT ==
                         bsls::AlignmentUtil::calculateAlignmentFromSize(ret));
    return ret;
}

namespace BloombergLP {
namespace bdlma {

                      // -----------------------------
                      // class ConcurrentPoolAllocator
                      // -----------------------------

// CREATORS
ConcurrentPoolAllocator::ConcurrentPoolAllocator(
                                              bslma::Allocator *basicAllocator)
: d_initialized(k_UNINITIALIZED)
, d_blockSize(0)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_maxBlocksPerChunk(k_MAX_CHUNK_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ConcurrentPoolAllocator::ConcurrentPoolAllocator(
                                   bsls::BlockGrowth::Strategy  growthStrategy,
                                   bslma::Allocator            *basicAllocator)
: d_initialized(k_UNINITIALIZED)
, d_blockSize(0)
, d_growthStrategy(growthStrategy)
, d_maxBlocksPerChunk(k_MAX_CHUNK_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ConcurrentPoolAllocator::ConcurrentPoolAllocator(
                                              size_type         blockSize,
                                              bslma::Allocator *basicAllocator)
: d_initialized(k_UNINITIALIZED)
, d_blockSize(blockSize)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_maxBlocksPerChunk(k_MAX_CHUNK_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (blockSize) {
        new (d_pool.buffer()) ConcurrentPool(
                           calculateMaxAlignedSize(blockSize + sizeof(Header)),
                           d_growthStrategy,
                           d_allocator_p);
        d_initialized = k_INITIALIZED;
    }
}

ConcurrentPoolAllocator::ConcurrentPoolAllocator(
                                   size_type                    blockSize,
                                   bsls::BlockGrowth::Strategy  growthStrategy,
                                   bslma::Allocator            *basicAllocator)
: d_initialized(k_UNINITIALIZED)
, d_blockSize(blockSize)
, d_growthStrategy(growthStrategy)
, d_maxBlocksPerChunk(k_MAX_CHUNK_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (blockSize) {
        new (d_pool.buffer()) ConcurrentPool(
                           calculateMaxAlignedSize(blockSize + sizeof(Header)),
                           d_growthStrategy,
                           d_allocator_p);
        d_initialized = k_INITIALIZED;
    }
}

ConcurrentPoolAllocator::ConcurrentPoolAllocator(
                                bsls::BlockGrowth::Strategy  growthStrategy,
                                int                          maxBlocksPerChunk,
                                bslma::Allocator            *basicAllocator)
: d_initialized(k_UNINITIALIZED)
, d_blockSize(0)
, d_growthStrategy(growthStrategy)
, d_maxBlocksPerChunk(maxBlocksPerChunk)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ConcurrentPoolAllocator::ConcurrentPoolAllocator(
                                size_type                    blockSize,
                                bsls::BlockGrowth::Strategy  growthStrategy,
                                int                          maxBlocksPerChunk,
                                bslma::Allocator            *basicAllocator)
: d_initialized(k_UNINITIALIZED)
, d_blockSize(blockSize)
, d_growthStrategy(growthStrategy)
, d_maxBlocksPerChunk(maxBlocksPerChunk)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (blockSize) {
        new (d_pool.buffer()) ConcurrentPool(
                           calculateMaxAlignedSize(blockSize + sizeof(Header)),
                           d_growthStrategy,
                           d_maxBlocksPerChunk,
                           d_allocator_p);
        d_initialized = k_INITIALIZED;
    }
}

ConcurrentPoolAllocator::~ConcurrentPoolAllocator()
{
    if (k_INITIALIZED == d_initialized) {
        d_pool.object().~ConcurrentPool();
    }
}

// MANIPULATORS
void *ConcurrentPoolAllocator::allocate(size_type size)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;                                                     // RETURN
    }

    // TBD This should be replaced with a load with acquire semantics.

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(k_INITIALIZED !=
                                                              d_initialized)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        while (1) {
            int res = d_initialized.testAndSwap(k_UNINITIALIZED,
                                                k_INITIALIZING);
            if (k_INITIALIZING == res) {
                bslmt::ThreadUtil::yield();
                continue; // initialization in progress
            }
            else if (k_UNINITIALIZED != res) {
                break;    // initialized
            }
            d_blockSize = size;
            new (d_pool.buffer()) ConcurrentPool(
                                calculateMaxAlignedSize(size + sizeof(Header)),
                                d_growthStrategy,
                                d_maxBlocksPerChunk,
                                d_allocator_p);
            d_initialized = k_INITIALIZED;
            break;
        }
    }

    char *ptr;
    const size_type totalSize = size + sizeof(Header);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                    totalSize > d_pool.object().blockSize())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        ptr = static_cast<char *>(d_allocator_p->allocate(totalSize));
        static_cast<Header *>(static_cast<void *>(ptr))->d_magicNumber = 0;
    }
    else {
        ptr = static_cast<char *>(d_pool.object().allocate());
        static_cast<Header *>(static_cast<void *>(ptr))->d_magicNumber =
                                                                k_MAGIC_NUMBER;
    }
    return ptr + sizeof(Header);
}

void ConcurrentPoolAllocator::deallocate(void *address)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!address)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return;                                                       // RETURN
    }

    Header *header = static_cast<Header *>(
                        static_cast<void *>(
                            static_cast<char *>(address) - sizeof(Header)));
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                    k_MAGIC_NUMBER == header->d_magicNumber)) {
        d_pool.object().deallocate(header);
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        d_allocator_p->deallocate(header);
    }
}

}  // close package namespace
}  // close enterprise namespace

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
