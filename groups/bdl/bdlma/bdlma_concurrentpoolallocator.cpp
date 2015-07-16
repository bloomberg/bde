// bdlma_concurrentpoolallocator.cpp                                            -*-C++-*-
#include <bdlma_concurrentpoolallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_concurrentpoolallocator_cpp,"$Id$ $CSID$")

#include <bslma_testallocator.h>    // for testing only
#include <bdlmtt_xxxthread.h>

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

    const int objectAlignmentMin = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1;
    const int ret = (totalSize + objectAlignmentMin) & ~(objectAlignmentMin);
    BSLS_ASSERT(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT ==
                         bsls::AlignmentUtil::calculateAlignmentFromSize(ret));
    return ret;
}

namespace BloombergLP {

namespace bdlma {
                      // -------------------------
                      // class ConcurrentPoolAllocator
                      // -------------------------

// CREATORS
ConcurrentPoolAllocator::ConcurrentPoolAllocator(bslma::Allocator *basicAllocator)
: d_initialized(BCEMA_UNINITIALIZED)
, d_blockSize(0)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_maxBlocksPerChunk(MAX_CHUNK_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ConcurrentPoolAllocator::ConcurrentPoolAllocator(
                                   bsls::BlockGrowth::Strategy  growthStrategy,
                                   bslma::Allocator            *basicAllocator)
: d_initialized(BCEMA_UNINITIALIZED)
, d_blockSize(0)
, d_growthStrategy(growthStrategy)
, d_maxBlocksPerChunk(MAX_CHUNK_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ConcurrentPoolAllocator::ConcurrentPoolAllocator(size_type         blockSize,
                                         bslma::Allocator *basicAllocator)
: d_initialized(BCEMA_UNINITIALIZED)
, d_blockSize(blockSize)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_maxBlocksPerChunk(MAX_CHUNK_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (blockSize) {
        new (d_pool.buffer()) ConcurrentPool(
                           calculateMaxAlignedSize(blockSize + sizeof(Header)),
                           d_growthStrategy,
                           d_allocator_p);
        d_initialized = BCEMA_INITIALIZED;
    }
}

ConcurrentPoolAllocator::ConcurrentPoolAllocator(
                                   size_type                    blockSize,
                                   bsls::BlockGrowth::Strategy  growthStrategy,
                                   bslma::Allocator            *basicAllocator)
: d_initialized(BCEMA_UNINITIALIZED)
, d_blockSize(blockSize)
, d_growthStrategy(growthStrategy)
, d_maxBlocksPerChunk(MAX_CHUNK_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (blockSize) {
        new (d_pool.buffer()) ConcurrentPool(
                           calculateMaxAlignedSize(blockSize + sizeof(Header)),
                           d_growthStrategy,
                           d_allocator_p);
        d_initialized = BCEMA_INITIALIZED;
    }
}

ConcurrentPoolAllocator::ConcurrentPoolAllocator(
                                bsls::BlockGrowth::Strategy  growthStrategy,
                                int                          maxBlocksPerChunk,
                                bslma::Allocator            *basicAllocator)
: d_initialized(BCEMA_UNINITIALIZED)
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
: d_initialized(BCEMA_UNINITIALIZED)
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
        d_initialized = BCEMA_INITIALIZED;
    }
}

ConcurrentPoolAllocator::~ConcurrentPoolAllocator()
{
    if (BCEMA_INITIALIZED == d_initialized) {
        d_pool.object().~ConcurrentPool();
    }
}

// MANIPULATORS
void *ConcurrentPoolAllocator::allocate(size_type size)
{
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
                bdlmtt::ThreadUtil::yield();
                continue; // initialization in progress
            }
            else if (BCEMA_UNINITIALIZED != res) {
                break;    // initialized
            }
            d_blockSize = size;
            new (d_pool.buffer()) ConcurrentPool(
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

void ConcurrentPoolAllocator::deallocate(void *ptr)
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
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
