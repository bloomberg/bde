// bcema_multipool.cpp                                                -*-C++-*-
#include <bcema_multipool.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_multipool_cpp,"$Id$ $CSID$")

#include <bcema_pool.h>
#include <bcemt_barrier.h>                  // for testing only
#include <bcemt_lockguard.h>
#include <bcemt_thread.h>

#include <bdes_bitutil.h>

#include <bslma_deallocatorproctor.h>
#include <bslma_autodestructor.h>
#include <bslma_default.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <bsl_cstdio.h>  // 'fprintf'

namespace BloombergLP {

enum {
    DEFAULT_NUM_POOLS      = 10,
    DEFAULT_MAX_CHUNK_SIZE = 32,
    MIN_BLOCK_SIZE         = 8
};

                      // ---------------------
                      // class bcema_Multipool
                      // ---------------------

// PRIVATE MANIPULATORS
void bcema_Multipool::initialize(bsls_BlockGrowth::Strategy growthStrategy,
                                 int                        maxBlocksPerChunk)
{
    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<bcema_Pool *>(
                      d_allocAdapter.allocate(d_numPools * sizeof *d_pools_p));

    bslma_DeallocatorProctor<bslma_Allocator> autoPoolsDeallocator(
                                                              d_pools_p,
                                                              &d_allocAdapter);
    bslma_AutoDestructor<bcema_Pool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) bcema_Pool(d_maxBlockSize + sizeof(Header),
                                       growthStrategy,
                                       maxBlocksPerChunk,
                                       &d_allocAdapter);

        d_maxBlockSize *= 2;
        BSLS_ASSERT(d_maxBlockSize > 0);
    }

    d_maxBlockSize /= 2;

    autoDtor.release();
    autoPoolsDeallocator.release();
}

void bcema_Multipool::initialize(
                         const bsls_BlockGrowth::Strategy *growthStrategyArray,
                         int                         maxBlocksPerChunk)
{
    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<bcema_Pool *>(
                      d_allocAdapter.allocate(d_numPools * sizeof *d_pools_p));

    bslma_DeallocatorProctor<bslma_Allocator> autoPoolsDeallocator(
                                                              d_pools_p,
                                                              &d_allocAdapter);
    bslma_AutoDestructor<bcema_Pool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) bcema_Pool(d_maxBlockSize + sizeof(Header),
                                       growthStrategyArray[i],
                                       maxBlocksPerChunk,
                                       &d_allocAdapter);

        d_maxBlockSize *= 2;
        BSLS_ASSERT(d_maxBlockSize > 0);
    }

    d_maxBlockSize /= 2;

    autoDtor.release();
    autoPoolsDeallocator.release();
}

void bcema_Multipool::initialize(
                            bsls_BlockGrowth::Strategy  growthStrategy,
                            const int                  *maxBlocksPerChunkArray)
{
    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<bcema_Pool *>(
                      d_allocAdapter.allocate(d_numPools * sizeof *d_pools_p));

    bslma_DeallocatorProctor<bslma_Allocator> autoPoolsDeallocator(
                                                              d_pools_p,
                                                              &d_allocAdapter);
    bslma_AutoDestructor<bcema_Pool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) bcema_Pool(d_maxBlockSize + sizeof(Header),
                                       growthStrategy,
                                       maxBlocksPerChunkArray[i],
                                       &d_allocAdapter);

        d_maxBlockSize *= 2;
        BSLS_ASSERT(d_maxBlockSize > 0);
    }

    d_maxBlockSize /= 2;

    autoDtor.release();
    autoPoolsDeallocator.release();
}

void bcema_Multipool::initialize(
                      const bsls_BlockGrowth::Strategy *growthStrategyArray,
                      const int                        *maxBlocksPerChunkArray)
{
    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<bcema_Pool *>(
                      d_allocAdapter.allocate(d_numPools * sizeof *d_pools_p));

    bslma_DeallocatorProctor<bslma_Allocator> autoPoolsDeallocator(
                                                              d_pools_p,
                                                              &d_allocAdapter);
    bslma_AutoDestructor<bcema_Pool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) bcema_Pool(d_maxBlockSize + sizeof(Header),
                                       growthStrategyArray[i],
                                       maxBlocksPerChunkArray[i],
                                       &d_allocAdapter);

        d_maxBlockSize *= 2;
        BSLS_ASSERT(d_maxBlockSize > 0);
    }

    d_maxBlockSize /= 2;

    autoDtor.release();
    autoPoolsDeallocator.release();
}

// PRIVATE ACCESSORS
inline
int bcema_Multipool::findPool(int size) const
{
    return bdes_BitUtil::find1AtLargestIndex(
                                   ((size + MIN_BLOCK_SIZE - 1) >> 3) * 2 - 1);
}

// CREATORS
bcema_Multipool::
bcema_Multipool(bslma_Allocator *basicAllocator)
: d_numPools(DEFAULT_NUM_POOLS)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(bsls_BlockGrowth::BSLS_GEOMETRIC, DEFAULT_MAX_CHUNK_SIZE);
}

bcema_Multipool::
bcema_Multipool(int              numPools,
                bslma_Allocator *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(bsls_BlockGrowth::BSLS_GEOMETRIC, DEFAULT_MAX_CHUNK_SIZE);
}

bcema_Multipool::
bcema_Multipool(bsls_BlockGrowth::Strategy  growthStrategy,
                bslma_Allocator            *basicAllocator)
: d_numPools(DEFAULT_NUM_POOLS)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategy, DEFAULT_MAX_CHUNK_SIZE);
}

bcema_Multipool::
bcema_Multipool(int                         numPools,
                bsls_BlockGrowth::Strategy  growthStrategy,
                bslma_Allocator            *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategy, DEFAULT_MAX_CHUNK_SIZE);
}

bcema_Multipool::
bcema_Multipool(int                               numPools,
                const bsls_BlockGrowth::Strategy *growthStrategyArray,
                bslma_Allocator                  *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategyArray, DEFAULT_MAX_CHUNK_SIZE);
}

bcema_Multipool::
bcema_Multipool(int                         numPools,
                bsls_BlockGrowth::Strategy  growthStrategy,
                int                         maxBlocksPerChunk,
                bslma_Allocator            *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategy, maxBlocksPerChunk);
}

bcema_Multipool::
bcema_Multipool(int                               numPools,
                const bsls_BlockGrowth::Strategy *growthStrategyArray,
                int                               maxBlocksPerChunk,
                bslma_Allocator                  *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategyArray, maxBlocksPerChunk);
}

bcema_Multipool::
bcema_Multipool(int                         numPools,
                bsls_BlockGrowth::Strategy  growthStrategy,
                const int                  *maxBlocksPerChunkArray,
                bslma_Allocator            *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategy, maxBlocksPerChunkArray);
}

bcema_Multipool::
bcema_Multipool(int                               numPools,
                const bsls_BlockGrowth::Strategy *growthStrategyArray,
                const int                        *maxBlocksPerChunkArray,
                bslma_Allocator                  *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategyArray, maxBlocksPerChunkArray);
}

bcema_Multipool::~bcema_Multipool()
{
    d_blockList.release();
    for (int i = 0; i < d_numPools; ++i) {
        d_pools_p[i].release();
        d_pools_p[i].~bcema_Pool();
    }
    d_allocAdapter.deallocate(d_pools_p);
}

// MANIPULATORS
void *bcema_Multipool::allocate(int size)
{
    // TBD: Change this block to 'BSLS_ASSERT(1 <= size)' after robo is clean.
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        static unsigned int count = 0;
        if (count <= 100 && 0 == count % 10) {
            bsl::fprintf(stderr,
                         "Error: Allocating 0 bytes in %s\n", __FILE__);
        }
        ++count;
        return 0;
    }

    if (size <= d_maxBlockSize) {
        const int pool = findPool(size);
        Header *p = static_cast<Header *>(d_pools_p[pool].allocate());
        p->d_header.d_poolIdx = pool;
        return p + 1;                                                 // RETURN
    }

    // The requested size is large and will not be pooled.

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    Header *p = static_cast<Header *>(
                                  d_blockList.allocate(size + sizeof(Header)));
    p->d_header.d_poolIdx = -1;
    return p + 1;
}

void bcema_Multipool::deallocate(void *address)
{
    Header *h = static_cast<Header *>(address) - 1;

    const int pool = h->d_header.d_poolIdx;

    if (-1 == pool) {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
        d_blockList.deallocate(h);
    }
    else {
        d_pools_p[pool].deallocate(h);
    }
}

void bcema_Multipool::release()
{
    for (int i = 0; i < d_numPools; ++i) {
        d_pools_p[i].release();
    }
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_blockList.release();
}

void bcema_Multipool::reserveCapacity(int size, int numBlocks)
{
    BSLS_ASSERT(0 <= size);
    BSLS_ASSERT(0 <= numBlocks);

    // TBD: Change this block to 'BSLS_ASSERT(1 <= size)' and
    // 'BSLS_ASSERT(size <= d_maxBlockSize)' after robo is clean.
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(size > d_maxBlockSize) ||
        BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        static unsigned int count = 0;
        if (count <= 100 && 0 == count % 10) {
            bsl::fprintf(stderr,
                         "Error: Allocating 0 bytes [reserveCapacity] in %s\n",
                         __FILE__);
        }
        ++count;
        return;
    }

    const int pool = findPool(size);
    d_pools_p[pool].reserveCapacity(numBlocks);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
