// bcema_poolallocator.cpp                                            -*-C++-*-
#include <bcema_poolallocator.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcema_poolallocator_cpp,"$Id$ $CSID$")

#include <bcema_testallocator.h>    // for testing only
#include <bcemt_thread.h>

#include <bslma_default.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>

// CONSTANTS
enum {
    MAX_CHUNK_SIZE = 32  // maximum number of blocks per chunk
};

// STATIC METHODS
static inline
int calculateMaxAlignedSize(int totalSize)
{
    using namespace BloombergLP;

    const int objectAlignmentMin1 = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1;
    const int ret = (totalSize + objectAlignmentMin1) & ~(objectAlignmentMin1);
    BSLS_ASSERT(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT ==
                          bsls_AlignmentUtil::calculateAlignmentFromSize(ret));
    return ret;
}

namespace BloombergLP {

                      // -------------------------
                      // class bcema_PoolAllocator
                      // -------------------------

// CREATORS
bcema_PoolAllocator::bcema_PoolAllocator(bslma_Allocator *basicAllocator)
: d_initialized(BCEMA_UNINITIALIZED)
, d_blockSize(0)
, d_growthStrategy(bsls_BlockGrowth::BSLS_GEOMETRIC)
, d_maxBlocksPerChunk(MAX_CHUNK_SIZE)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bcema_PoolAllocator::bcema_PoolAllocator(
                                    bsls_BlockGrowth::Strategy  growthStrategy,
                                    bslma_Allocator            *basicAllocator)
: d_initialized(BCEMA_UNINITIALIZED)
, d_blockSize(0)
, d_growthStrategy(growthStrategy)
, d_maxBlocksPerChunk(MAX_CHUNK_SIZE)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bcema_PoolAllocator::bcema_PoolAllocator(size_type        blockSize,
                                         bslma_Allocator *basicAllocator)
: d_initialized(BCEMA_UNINITIALIZED)
, d_blockSize(blockSize)
, d_growthStrategy(bsls_BlockGrowth::BSLS_GEOMETRIC)
, d_maxBlocksPerChunk(MAX_CHUNK_SIZE)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 <= blockSize);

    if (blockSize) {
        new (d_pool.buffer()) bcema_Pool(
                           calculateMaxAlignedSize(blockSize + sizeof(Header)),
                           d_growthStrategy,
                           d_allocator_p);
        d_initialized = BCEMA_INITIALIZED;
    }
}

bcema_PoolAllocator::bcema_PoolAllocator(
                                    size_type                   blockSize,
                                    bsls_BlockGrowth::Strategy  growthStrategy,
                                    bslma_Allocator            *basicAllocator)
: d_initialized(BCEMA_UNINITIALIZED)
, d_blockSize(blockSize)
, d_growthStrategy(growthStrategy)
, d_maxBlocksPerChunk(MAX_CHUNK_SIZE)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 <= blockSize);

    if (blockSize) {
        new (d_pool.buffer()) bcema_Pool(
                           calculateMaxAlignedSize(blockSize + sizeof(Header)),
                           d_growthStrategy,
                           d_allocator_p);
        d_initialized = BCEMA_INITIALIZED;
    }
}

bcema_PoolAllocator::bcema_PoolAllocator(
                                 bsls_BlockGrowth::Strategy  growthStrategy,
                                 int                         maxBlocksPerChunk,
                                 bslma_Allocator            *basicAllocator)
: d_initialized(BCEMA_UNINITIALIZED)
, d_blockSize(0)
, d_growthStrategy(growthStrategy)
, d_maxBlocksPerChunk(maxBlocksPerChunk)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bcema_PoolAllocator::bcema_PoolAllocator(
                                 size_type                   blockSize,
                                 bsls_BlockGrowth::Strategy  growthStrategy,
                                 int                         maxBlocksPerChunk,
                                 bslma_Allocator            *basicAllocator)
: d_initialized(BCEMA_UNINITIALIZED)
, d_blockSize(blockSize)
, d_growthStrategy(growthStrategy)
, d_maxBlocksPerChunk(maxBlocksPerChunk)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 <= blockSize);

    if (blockSize) {
        new (d_pool.buffer()) bcema_Pool(
                           calculateMaxAlignedSize(blockSize + sizeof(Header)),
                           d_growthStrategy,
                           d_maxBlocksPerChunk,
                           d_allocator_p);
        d_initialized = BCEMA_INITIALIZED;
    }
}

bcema_PoolAllocator::~bcema_PoolAllocator()
{
    if (BCEMA_INITIALIZED == d_initialized) {
        d_pool.object().~bcema_Pool();
    }
}

// MANIPULATORS
void *bcema_PoolAllocator::allocate(size_type size)
{
    BSLS_ASSERT(0 <= size);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;
    }

    // TBD This should be replaced with a load with acquire semantics.

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(BCEMA_INITIALIZED !=
                                                              d_initialized)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        while (1) {
            int res = d_initialized.testAndSwap(BCEMA_UNINITIALIZED,
                                                BCEMA_INITIALIZING);
            if (BCEMA_INITIALIZING == res) {
                bcemt_ThreadUtil::yield();
                continue; // initialization in progress
            }
            else if (BCEMA_UNINITIALIZED != res) {
                break;    // initialized
            }
            d_blockSize = size;
            new (d_pool.buffer()) bcema_Pool(
                                calculateMaxAlignedSize(size + sizeof(Header)),
                                d_growthStrategy,
                                d_maxBlocksPerChunk,
                                d_allocator_p);
            d_initialized = BCEMA_INITIALIZED;
            break;
        }
    }

    char *ptr;
    const size_t totalSize = static_cast<size_t>(size) + sizeof(Header);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
               totalSize > static_cast<size_t>(d_pool.object().blockSize()))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        ptr = static_cast<char *>(d_allocator_p->allocate(totalSize));
        static_cast<Header *>(static_cast<void *>(ptr))->d_magicNumber = 0;
    }
    else {
        ptr = static_cast<char *>(d_pool.object().allocate());
        static_cast<Header *>(static_cast<void *>(ptr))->d_magicNumber =
                                                            BCEMA_MAGIC_NUMBER;
    }
    return ptr + sizeof(Header);
}

void bcema_PoolAllocator::deallocate(void *ptr)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!ptr)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return;
    }

    Header *header = static_cast<Header *>(
                        static_cast<void *>(
                            static_cast<char *>(ptr) - sizeof(Header)));
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                BCEMA_MAGIC_NUMBER == header->d_magicNumber)) {
        d_pool.object().deallocate(header);
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        d_allocator_p->deallocate(header);
    }
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
