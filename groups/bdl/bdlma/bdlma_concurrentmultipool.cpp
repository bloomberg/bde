// bdlma_concurrentmultipool.cpp                                                -*-C++-*-
#include <bdlma_concurrentmultipool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdema_multipool_cpp,"$Id$ $CSID$")

#include <bdlma_concurrentpool.h>
#include <bdlmtt_barrier.h>                  // for testing only
#include <bdlmtt_lockguard.h>
#include <bdlmtt_xxxthread.h>

#include <bdlb_xxxbitutil.h>

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

namespace bdlma {
                      // ---------------------
                      // class ConcurrentMultipool
                      // ---------------------

// PRIVATE MANIPULATORS
void ConcurrentMultipool::initialize(bsls::BlockGrowth::Strategy growthStrategy,
                                 int                         maxBlocksPerChunk)
{
    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<ConcurrentPool *>(
                      d_allocAdapter.allocate(d_numPools * sizeof *d_pools_p));

    bslma::DeallocatorProctor<bslma::Allocator> autoPoolsDeallocator(
                                                              d_pools_p,
                                                              &d_allocAdapter);
    bslma::AutoDestructor<ConcurrentPool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) ConcurrentPool(d_maxBlockSize + sizeof(Header),
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

void ConcurrentMultipool::initialize(
                        const bsls::BlockGrowth::Strategy *growthStrategyArray,
                        int                                maxBlocksPerChunk)
{
    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<ConcurrentPool *>(
                      d_allocAdapter.allocate(d_numPools * sizeof *d_pools_p));

    bslma::DeallocatorProctor<bslma::Allocator> autoPoolsDeallocator(
                                                              d_pools_p,
                                                              &d_allocAdapter);
    bslma::AutoDestructor<ConcurrentPool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) ConcurrentPool(d_maxBlockSize + sizeof(Header),
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

void ConcurrentMultipool::initialize(
                           bsls::BlockGrowth::Strategy  growthStrategy,
                           const int                   *maxBlocksPerChunkArray)
{
    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<ConcurrentPool *>(
                      d_allocAdapter.allocate(d_numPools * sizeof *d_pools_p));

    bslma::DeallocatorProctor<bslma::Allocator> autoPoolsDeallocator(
                                                              d_pools_p,
                                                              &d_allocAdapter);
    bslma::AutoDestructor<ConcurrentPool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) ConcurrentPool(d_maxBlockSize + sizeof(Header),
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

void ConcurrentMultipool::initialize(
                     const bsls::BlockGrowth::Strategy *growthStrategyArray,
                     const int                         *maxBlocksPerChunkArray)
{
    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<ConcurrentPool *>(
                      d_allocAdapter.allocate(d_numPools * sizeof *d_pools_p));

    bslma::DeallocatorProctor<bslma::Allocator> autoPoolsDeallocator(
                                                              d_pools_p,
                                                              &d_allocAdapter);
    bslma::AutoDestructor<ConcurrentPool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) ConcurrentPool(d_maxBlockSize + sizeof(Header),
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
int ConcurrentMultipool::findPool(int size) const
{
    return bdlb::BitUtil::find1AtLargestIndex(
                                   ((size + MIN_BLOCK_SIZE - 1) >> 3) * 2 - 1);
}

// CREATORS
ConcurrentMultipool::
ConcurrentMultipool(bslma::Allocator *basicAllocator)
: d_numPools(DEFAULT_NUM_POOLS)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(bsls::BlockGrowth::BSLS_GEOMETRIC, DEFAULT_MAX_CHUNK_SIZE);
}

ConcurrentMultipool::
ConcurrentMultipool(int               numPools,
                bslma::Allocator *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(bsls::BlockGrowth::BSLS_GEOMETRIC, DEFAULT_MAX_CHUNK_SIZE);
}

ConcurrentMultipool::
ConcurrentMultipool(bsls::BlockGrowth::Strategy  growthStrategy,
                bslma::Allocator            *basicAllocator)
: d_numPools(DEFAULT_NUM_POOLS)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategy, DEFAULT_MAX_CHUNK_SIZE);
}

ConcurrentMultipool::
ConcurrentMultipool(int                          numPools,
                bsls::BlockGrowth::Strategy  growthStrategy,
                bslma::Allocator            *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategy, DEFAULT_MAX_CHUNK_SIZE);
}

ConcurrentMultipool::
ConcurrentMultipool(int                                numPools,
                const bsls::BlockGrowth::Strategy *growthStrategyArray,
                bslma::Allocator                  *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategyArray, DEFAULT_MAX_CHUNK_SIZE);
}

ConcurrentMultipool::
ConcurrentMultipool(int                          numPools,
                bsls::BlockGrowth::Strategy  growthStrategy,
                int                          maxBlocksPerChunk,
                bslma::Allocator            *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategy, maxBlocksPerChunk);
}

ConcurrentMultipool::
ConcurrentMultipool(int                                numPools,
                const bsls::BlockGrowth::Strategy *growthStrategyArray,
                int                                maxBlocksPerChunk,
                bslma::Allocator                  *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategyArray, maxBlocksPerChunk);
}

ConcurrentMultipool::
ConcurrentMultipool(int                          numPools,
                bsls::BlockGrowth::Strategy  growthStrategy,
                const int                   *maxBlocksPerChunkArray,
                bslma::Allocator            *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategy, maxBlocksPerChunkArray);
}

ConcurrentMultipool::
ConcurrentMultipool(int                                numPools,
                const bsls::BlockGrowth::Strategy *growthStrategyArray,
                const int                         *maxBlocksPerChunkArray,
                bslma::Allocator                  *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategyArray, maxBlocksPerChunkArray);
}

ConcurrentMultipool::~ConcurrentMultipool()
{
    d_blockList.release();
    for (int i = 0; i < d_numPools; ++i) {
        d_pools_p[i].release();
        d_pools_p[i].~ConcurrentPool();
    }
    d_allocAdapter.deallocate(d_pools_p);
}

// MANIPULATORS
void *ConcurrentMultipool::allocate(int size)
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

    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    Header *p = static_cast<Header *>(
                                  d_blockList.allocate(size + sizeof(Header)));
    p->d_header.d_poolIdx = -1;
    return p + 1;
}

void ConcurrentMultipool::deallocate(void *address)
{
    Header *h = static_cast<Header *>(address) - 1;

    const int pool = h->d_header.d_poolIdx;

    if (-1 == pool) {
        bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
        d_blockList.deallocate(h);
    }
    else {
        d_pools_p[pool].deallocate(h);
    }
}

void ConcurrentMultipool::release()
{
    for (int i = 0; i < d_numPools; ++i) {
        d_pools_p[i].release();
    }
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(&d_mutex);
    d_blockList.release();
}

void ConcurrentMultipool::reserveCapacity(int size, int numBlocks)
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
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
