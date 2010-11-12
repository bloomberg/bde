// bdema_multipool.cpp                                                -*-C++-*-
#include <bdema_multipool.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_multipool_cpp,"$Id$ $CSID$")

#include <bdema_bufferedsequentialallocator.h>  // for testing only
#include <bdes_bitutil.h>

#include <bslma_allocator.h>
#include <bslma_autodestructor.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_default.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <bsl_new.h>
#include <bsl_cstdio.h>     // 'fprintf'

namespace BloombergLP {

enum {
    DEFAULT_NUM_POOLS      = 10,
    DEFAULT_MAX_CHUNK_SIZE = 32,
    MIN_BLOCK_SIZE         =  8
};

                      // ---------------------
                      // class bdema_Multipool
                      // ---------------------

// PRIVATE MANIPULATORS
void bdema_Multipool::initialize(bsls_BlockGrowth::Strategy growthStrategy,
                                 int                        maxBlocksPerChunk)
{
    BSLS_ASSERT(1 <= maxBlocksPerChunk);

    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<bdema_Pool *>(
                      d_allocator_p->allocate(d_numPools * sizeof *d_pools_p));

    bslma_DeallocatorProctor<bslma_Allocator> autoPoolsDeallocator(
                                                               d_pools_p,
                                                               d_allocator_p);
    bslma_AutoDestructor<bdema_Pool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) bdema_Pool(d_maxBlockSize + sizeof(Header),
                                       growthStrategy,
                                       maxBlocksPerChunk,
                                       d_allocator_p);

        d_maxBlockSize *= 2;
        BSLS_ASSERT(d_maxBlockSize > 0);
    }

    d_maxBlockSize /= 2;

    autoDtor.release();
    autoPoolsDeallocator.release();
}

void bdema_Multipool::initialize(
                         const bsls_BlockGrowth::Strategy *growthStrategyArray,
                         int                               maxBlocksPerChunk)
{
    BSLS_ASSERT(growthStrategyArray);
    BSLS_ASSERT(1 <= maxBlocksPerChunk);

    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<bdema_Pool *>(
                      d_allocator_p->allocate(d_numPools * sizeof *d_pools_p));

    bslma_DeallocatorProctor<bslma_Allocator> autoPoolsDeallocator(
                                                               d_pools_p,
                                                               d_allocator_p);
    bslma_AutoDestructor<bdema_Pool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) bdema_Pool(d_maxBlockSize + sizeof(Header),
                                       growthStrategyArray[i],
                                       maxBlocksPerChunk,
                                       d_allocator_p);

        d_maxBlockSize *= 2;
        BSLS_ASSERT(d_maxBlockSize > 0);
    }

    d_maxBlockSize /= 2;

    autoDtor.release();
    autoPoolsDeallocator.release();
}

void bdema_Multipool::initialize(
                            bsls_BlockGrowth::Strategy  growthStrategy,
                            const int                  *maxBlocksPerChunkArray)
{
    BSLS_ASSERT(maxBlocksPerChunkArray);

    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<bdema_Pool *>(
                      d_allocator_p->allocate(d_numPools * sizeof *d_pools_p));

    bslma_DeallocatorProctor<bslma_Allocator> autoPoolsDeallocator(
                                                               d_pools_p,
                                                               d_allocator_p);
    bslma_AutoDestructor<bdema_Pool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) bdema_Pool(d_maxBlockSize + sizeof(Header),
                                       growthStrategy,
                                       maxBlocksPerChunkArray[i],
                                       d_allocator_p);

        d_maxBlockSize *= 2;
        BSLS_ASSERT(d_maxBlockSize > 0);
    }

    d_maxBlockSize /= 2;

    autoDtor.release();
    autoPoolsDeallocator.release();
}

void bdema_Multipool::initialize(
                      const bsls_BlockGrowth::Strategy *growthStrategyArray,
                      const int                        *maxBlocksPerChunkArray)
{
    BSLS_ASSERT(growthStrategyArray);
    BSLS_ASSERT(maxBlocksPerChunkArray);

    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<bdema_Pool *>(
                      d_allocator_p->allocate(d_numPools * sizeof *d_pools_p));

    bslma_DeallocatorProctor<bslma_Allocator> autoPoolsDeallocator(
                                                               d_pools_p,
                                                               d_allocator_p);
    bslma_AutoDestructor<bdema_Pool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) bdema_Pool(d_maxBlockSize + sizeof(Header),
                                       growthStrategyArray[i],
                                       maxBlocksPerChunkArray[i],
                                       d_allocator_p);

        d_maxBlockSize *= 2;
        BSLS_ASSERT(d_maxBlockSize > 0);
    }

    d_maxBlockSize /= 2;

    autoDtor.release();
    autoPoolsDeallocator.release();
}

// PRIVATE ACCESSORS
inline
int bdema_Multipool::findPool(int size) const
{
    BSLS_ASSERT_SAFE(0    <= size);
    BSLS_ASSERT_SAFE(size <= d_maxBlockSize);

    return bdes_BitUtil::find1AtLargestIndex(
                                   ((size + MIN_BLOCK_SIZE - 1) >> 3) * 2 - 1);
}

// CREATORS
bdema_Multipool::bdema_Multipool(bslma_Allocator *basicAllocator)
: d_numPools(DEFAULT_NUM_POOLS)
, d_blockList(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    initialize(bsls_BlockGrowth::BSLS_GEOMETRIC, DEFAULT_MAX_CHUNK_SIZE);
}

bdema_Multipool::bdema_Multipool(int              numPools,
                                 bslma_Allocator *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);

    initialize(bsls_BlockGrowth::BSLS_GEOMETRIC, DEFAULT_MAX_CHUNK_SIZE);
}

bdema_Multipool::bdema_Multipool(bsls_BlockGrowth::Strategy  growthStrategy,
                                 bslma_Allocator            *basicAllocator)
: d_numPools(DEFAULT_NUM_POOLS)
, d_blockList(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    initialize(growthStrategy, DEFAULT_MAX_CHUNK_SIZE);
}

bdema_Multipool::bdema_Multipool(int                         numPools,
                                 bsls_BlockGrowth::Strategy  growthStrategy,
                                 bslma_Allocator            *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);

    initialize(growthStrategy, DEFAULT_MAX_CHUNK_SIZE);
}

bdema_Multipool::bdema_Multipool(
                         int                               numPools,
                         const bsls_BlockGrowth::Strategy *growthStrategyArray,
                         bslma_Allocator                  *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);
    BSLS_ASSERT(growthStrategyArray);

    initialize(growthStrategyArray, DEFAULT_MAX_CHUNK_SIZE);
}

bdema_Multipool::bdema_Multipool(int                         numPools,
                                 bsls_BlockGrowth::Strategy  growthStrategy,
                                 int                         maxBlocksPerChunk,
                                 bslma_Allocator            *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);
    BSLS_ASSERT(1 <= maxBlocksPerChunk);

    initialize(growthStrategy, maxBlocksPerChunk);
}

bdema_Multipool::bdema_Multipool(
                         int                               numPools,
                         const bsls_BlockGrowth::Strategy *growthStrategyArray,
                         int                               maxBlocksPerChunk,
                         bslma_Allocator                  *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);
    BSLS_ASSERT(growthStrategyArray);
    BSLS_ASSERT(1 <= maxBlocksPerChunk);

    initialize(growthStrategyArray, maxBlocksPerChunk);
}

bdema_Multipool::bdema_Multipool(
                            int                         numPools,
                            bsls_BlockGrowth::Strategy  growthStrategy,
                            const int                  *maxBlocksPerChunkArray,
                            bslma_Allocator            *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);
    BSLS_ASSERT(maxBlocksPerChunkArray);

    initialize(growthStrategy, maxBlocksPerChunkArray);
}

bdema_Multipool::bdema_Multipool(
                      int                               numPools,
                      const bsls_BlockGrowth::Strategy *growthStrategyArray,
                      const int                        *maxBlocksPerChunkArray,
                      bslma_Allocator                  *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);
    BSLS_ASSERT(growthStrategyArray);
    BSLS_ASSERT(maxBlocksPerChunkArray);

    initialize(growthStrategyArray, maxBlocksPerChunkArray);
}

bdema_Multipool::~bdema_Multipool()
{
    BSLS_ASSERT(d_pools_p);
    BSLS_ASSERT(1 <= d_numPools);
    BSLS_ASSERT(1 <= d_maxBlockSize);
    BSLS_ASSERT(d_allocator_p);

    d_blockList.release();
    for (int i = 0; i < d_numPools; ++i) {
        d_pools_p[i].release();
        d_pools_p[i].~bdema_Pool();
    }
    d_allocator_p->deallocate(d_pools_p);
}

// MANIPULATORS
void *bdema_Multipool::allocate(int size)
{
    BSLS_ASSERT(0 <= size);

    // TBD: Change this block to 'BSLS_ASSERT(1 <= size)' after Robo is clean.
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
        return p + 1;
    }

    // The requested size is large and will not be pooled.

    Header *p = static_cast<Header *>(
                                  d_blockList.allocate(size + sizeof(Header)));
    p->d_header.d_poolIdx = -1;
    return p + 1;
}

void bdema_Multipool::deallocate(void *address)
{
    BSLS_ASSERT(address);

    Header *h = static_cast<Header *>(address) - 1;

    const int pool = h->d_header.d_poolIdx;

    if (-1 == pool) {
        d_blockList.deallocate(h);
    }
    else {
        d_pools_p[pool].deallocate(h);
    }
}

void bdema_Multipool::release()
{
    for (int i = 0; i < d_numPools; ++i) {
        d_pools_p[i].release();
    }
    d_blockList.release();
}

void bdema_Multipool::reserveCapacity(int size, int numBlocks)
{
    BSLS_ASSERT(0    <= size);
    BSLS_ASSERT(size <= d_maxBlockSize);
    BSLS_ASSERT(0    <= numBlocks);

    // TBD: Change this block to 'BSLS_ASSERT(1 <= size)' after Robo is clean.
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
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
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
