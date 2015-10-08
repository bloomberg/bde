// bdlma_concurrentmultipool.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_concurrentmultipool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdema_multipool_cpp,"$Id$ $CSID$")

#include <bdlma_concurrentpool.h>

#include <bdlb_bitutil.h>

#include <bslmt_barrier.h>                  // for testing only
#include <bslmt_lockguard.h>
#include <bslmt_condition.h>

#include <bslma_deallocatorproctor.h>
#include <bslma_autodestructor.h>
#include <bslma_default.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <bsl_cstdio.h>  // 'fprintf'
#include <bsl_cstdint.h>

#include <new>           // placement 'new'

namespace BloombergLP {

enum {
    k_DEFAULT_NUM_POOLS      = 10,
    k_DEFAULT_MAX_CHUNK_SIZE = 32,
    k_MIN_BLOCK_SIZE         = 8
};

namespace bdlma {

                        // -------------------------
                        // class ConcurrentMultipool
                        // -------------------------

// PRIVATE MANIPULATORS
void ConcurrentMultipool::initialize(
                                 bsls::BlockGrowth::Strategy growthStrategy,
                                 int                         maxBlocksPerChunk)
{
    d_maxBlockSize = k_MIN_BLOCK_SIZE;

    d_pools_p = static_cast<ConcurrentPool *>(
                      d_allocAdapter.allocate(d_numPools * sizeof *d_pools_p));

    bslma::DeallocatorProctor<bslma::Allocator> autoPoolsDeallocator(
                                                              d_pools_p,
                                                              &d_allocAdapter);
    bslma::AutoDestructor<ConcurrentPool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) ConcurrentPool(
                             d_maxBlockSize + static_cast<int>(sizeof(Header)),
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
    d_maxBlockSize = k_MIN_BLOCK_SIZE;

    d_pools_p = static_cast<ConcurrentPool *>(
                      d_allocAdapter.allocate(d_numPools * sizeof *d_pools_p));

    bslma::DeallocatorProctor<bslma::Allocator> autoPoolsDeallocator(
                                                              d_pools_p,
                                                              &d_allocAdapter);
    bslma::AutoDestructor<ConcurrentPool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) ConcurrentPool(
                             d_maxBlockSize + static_cast<int>(sizeof(Header)),
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
    d_maxBlockSize = k_MIN_BLOCK_SIZE;

    d_pools_p = static_cast<ConcurrentPool *>(
                      d_allocAdapter.allocate(d_numPools * sizeof *d_pools_p));

    bslma::DeallocatorProctor<bslma::Allocator> autoPoolsDeallocator(
                                                              d_pools_p,
                                                              &d_allocAdapter);
    bslma::AutoDestructor<ConcurrentPool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) ConcurrentPool(
                             d_maxBlockSize + static_cast<int>(sizeof(Header)),
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
    d_maxBlockSize = k_MIN_BLOCK_SIZE;

    d_pools_p = static_cast<ConcurrentPool *>(
                      d_allocAdapter.allocate(d_numPools * sizeof *d_pools_p));

    bslma::DeallocatorProctor<bslma::Allocator> autoPoolsDeallocator(
                                                              d_pools_p,
                                                              &d_allocAdapter);
    bslma::AutoDestructor<ConcurrentPool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) ConcurrentPool(
                             d_maxBlockSize + static_cast<int>(sizeof(Header)),
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
    return 31 - bdlb::BitUtil::numLeadingUnsetBits(static_cast<bsl::uint32_t>(
                                ((size + k_MIN_BLOCK_SIZE - 1) >> 3) * 2 - 1));
}

// CREATORS
ConcurrentMultipool::
ConcurrentMultipool(bslma::Allocator *basicAllocator)
: d_numPools(k_DEFAULT_NUM_POOLS)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(bsls::BlockGrowth::BSLS_GEOMETRIC, k_DEFAULT_MAX_CHUNK_SIZE);
}

ConcurrentMultipool::
ConcurrentMultipool(int               numPools,
                    bslma::Allocator *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(bsls::BlockGrowth::BSLS_GEOMETRIC, k_DEFAULT_MAX_CHUNK_SIZE);
}

ConcurrentMultipool::
ConcurrentMultipool(bsls::BlockGrowth::Strategy  growthStrategy,
                    bslma::Allocator            *basicAllocator)
: d_numPools(k_DEFAULT_NUM_POOLS)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategy, k_DEFAULT_MAX_CHUNK_SIZE);
}

ConcurrentMultipool::
ConcurrentMultipool(int                          numPools,
                    bsls::BlockGrowth::Strategy  growthStrategy,
                    bslma::Allocator            *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategy, k_DEFAULT_MAX_CHUNK_SIZE);
}

ConcurrentMultipool::
ConcurrentMultipool(int                                numPools,
                    const bsls::BlockGrowth::Strategy *growthStrategyArray,
                    bslma::Allocator                  *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocAdapter(&d_mutex, basicAllocator)
{
    initialize(growthStrategyArray, k_DEFAULT_MAX_CHUNK_SIZE);
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
    // TBD: change this block to 'BSLS_ASSERT(1 <= size)' after 'robo' is clean
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        static unsigned int count = 0;
        if (count <= 100 && 0 == count % 10) {
            bsl::fprintf(stderr,
                         "Error: Allocating 0 bytes in %s\n", __FILE__);
        }
        ++count;
        return 0;                                                     // RETURN
    }

    if (size <= d_maxBlockSize) {
        const int pool = findPool(size);
        Header *p = static_cast<Header *>(d_pools_p[pool].allocate());
        p->d_header.d_poolIdx = pool;
        return p + 1;                                                 // RETURN
    }

    // The requested size is large and will not be pooled.

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    Header *p = static_cast<Header *>(
                d_blockList.allocate(size + static_cast<int>(sizeof(Header))));
    p->d_header.d_poolIdx = -1;
    return p + 1;
}

void ConcurrentMultipool::deallocate(void *address)
{
    Header *h = static_cast<Header *>(address) - 1;

    const int pool = h->d_header.d_poolIdx;

    if (-1 == pool) {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
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
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    d_blockList.release();
}

void ConcurrentMultipool::reserveCapacity(int size, int numBlocks)
{
    BSLS_ASSERT(0 <= size);
    BSLS_ASSERT(0 <= numBlocks);

    // TBD: Change this block to 'BSLS_ASSERT(1 <= size)' and
    // 'BSLS_ASSERT(size <= d_maxBlockSize)' after 'robo' is clean.
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
        return;                                                       // RETURN
    }

    const int pool = findPool(size);
    d_pools_p[pool].reserveCapacity(numBlocks);
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
