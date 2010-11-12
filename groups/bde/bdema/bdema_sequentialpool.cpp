// bdema_sequentialpool.cpp                                           -*-C++-*-
#include <bdema_sequentialpool.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <bsl_climits.h>  // 'INT_MAX'
#include <bsl_cstdio.h>   // 'fprintf'
#include <bsl_cstdlib.h>  // 'abs'

enum {
    INITIAL_SIZE  = 256,  // default initial allocation size (in bytes)

    GROWTH_FACTOR =   2   // multiplicative factor by which to grow allocation
                          // size
};

namespace BloombergLP {

                        // --------------------------
                        // class bdema_SequentialPool
                        // --------------------------

// PRIVATE ACCESSORS
int bdema_SequentialPool::calculateNextBufferSize(int size) const
{
    const int bufferSize = d_buffer.bufferSize();
    int nextSize = 0 == bufferSize
                   ? INITIAL_SIZE : bufferSize;

    if (bsls_BlockGrowth::BSLS_CONSTANT == d_growthStrategy) {
        return nextSize;
    }

    int oldSize;
    do {
        oldSize   = nextSize;
        nextSize *= GROWTH_FACTOR;
    } while (nextSize < size && oldSize < nextSize);

    // If 'nextSize' overflows, use 'oldSize'.

    if (oldSize >= nextSize) {
        nextSize = oldSize;
    }

    return nextSize <= d_maxBufferSize ? nextSize : d_maxBufferSize;
}

// CREATORS
bdema_SequentialPool::bdema_SequentialPool(bslma_Allocator *basicAllocator)
: d_buffer()
, d_growthStrategy(bsls_BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
}

bdema_SequentialPool::
bdema_SequentialPool(bsls_BlockGrowth::Strategy  growthStrategy,
                     bslma_Allocator            *basicAllocator)
: d_buffer()
, d_growthStrategy(growthStrategy)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
}

bdema_SequentialPool::
bdema_SequentialPool(bsls_Alignment::Strategy  alignmentStrategy,
                     bslma_Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(bsls_BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
}

bdema_SequentialPool::
bdema_SequentialPool(bsls_BlockGrowth::Strategy  growthStrategy,
                     bsls_Alignment::Strategy    alignmentStrategy,
                     bslma_Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(growthStrategy)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
}

bdema_SequentialPool::
bdema_SequentialPool(int              initialSize,
                     bslma_Allocator *basicAllocator)
: d_buffer()
, d_growthStrategy(bsls_BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    // TBD: BSLS_ASSERT(0 < initialSize);
    initialSize = bsl::abs(initialSize);

    // TBD: An initial size of 0 should not be accepted.
    if (initialSize) {
        char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
        d_buffer.replaceBuffer(buffer, initialSize);
    }
}

bdema_SequentialPool::
bdema_SequentialPool(int                         initialSize,
                     bsls_BlockGrowth::Strategy  growthStrategy,
                     bslma_Allocator            *basicAllocator)
: d_buffer()
, d_growthStrategy(growthStrategy)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

bdema_SequentialPool::
bdema_SequentialPool(int                       initialSize,
                     bsls_Alignment::Strategy  alignmentStrategy,
                     bslma_Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(bsls_BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

bdema_SequentialPool::
bdema_SequentialPool(int                         initialSize,
                     bsls_BlockGrowth::Strategy  growthStrategy,
                     bsls_Alignment::Strategy    alignmentStrategy,
                     bslma_Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(growthStrategy)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}


bdema_SequentialPool::
bdema_SequentialPool(int              initialSize,
                     int              maxBufferSize,
                     bslma_Allocator *basicAllocator)
: d_buffer()
, d_growthStrategy(bsls_BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    // TBD: BSLS_ASSERT(0 < initialSize);
    BSLS_ASSERT(bsl::abs(initialSize) <= maxBufferSize);

    initialSize = bsl::abs(initialSize);

    // TBD: An initial size of 0 should not be accepted.
    if (initialSize) {
        char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
        d_buffer.replaceBuffer(buffer, initialSize);
    }
}

bdema_SequentialPool::
bdema_SequentialPool(int                         initialSize,
                     int                         maxBufferSize,
                     bsls_BlockGrowth::Strategy  growthStrategy,
                     bslma_Allocator            *basicAllocator)
: d_buffer()
, d_growthStrategy(growthStrategy)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

bdema_SequentialPool::
bdema_SequentialPool(int                       initialSize,
                     int                       maxBufferSize,
                     bsls_Alignment::Strategy  alignmentStrategy,
                     bslma_Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(bsls_BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

bdema_SequentialPool::
bdema_SequentialPool(int                         initialSize,
                     int                         maxBufferSize,
                     bsls_BlockGrowth::Strategy  growthStrategy,
                     bsls_Alignment::Strategy    alignmentStrategy,
                     bslma_Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(growthStrategy)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

// MANIPULATORS
void *bdema_SequentialPool::allocate(bsls_PlatformUtil::size_type size)
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

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(d_buffer.buffer())) {
        void *result = d_buffer.allocate(size);
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(result)) {
            return result;                                            // RETURN
        }
    }

    const int nextSize = calculateNextBufferSize(size);

    if (nextSize < size) {
        return d_blockList.allocate(size);                            // RETURN
    }

    d_buffer.replaceBuffer(static_cast<char *>(d_blockList.allocate(nextSize)),
                           nextSize);
    return d_buffer.allocateRaw(size);
}

void *bdema_SequentialPool::allocateAndExpand(
                                            bsls_PlatformUtil::size_type *size)
{
    BSLS_ASSERT(size);
    BSLS_ASSERT(0 < *size);

    void *result = allocate(*size);
    *size = d_buffer.expand(result, *size);

    return result;
}

void bdema_SequentialPool::reserveCapacity(int size)
{
    BSLS_ASSERT(0 < size);

    // If 'd_buffer.bufferSize()' is 0, 'd_buffer' is not managing any buffer
    // currently.

    if (0 != d_buffer.bufferSize() && d_buffer.hasSufficientCapacity(size)) {
        return;                                                       // RETURN
    }

    int nextSize = calculateNextBufferSize(size);

    if (nextSize < size) {
        nextSize = size;
    }

    d_buffer.replaceBuffer(static_cast<char *>(d_blockList.allocate(nextSize)),
                           nextSize);
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
