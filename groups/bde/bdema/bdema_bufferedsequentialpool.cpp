// bdema_bufferedsequentialpool.cpp                                   -*-C++-*-
#include <bdema_bufferedsequentialpool.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_bufferedsequentialpool_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <bsl_climits.h>  // 'INT_MAX'
#include <bsl_cstdio.h>   // 'fprintf'

enum {
    GROWTH_FACTOR = 2  // multiplicative factor by which to grow allocation
                       // size
};

namespace BloombergLP {

                    // ----------------------------------
                    // class bdema_BufferedSequentialPool
                    // ----------------------------------

// PRIVATE ACCESSORS
int bdema_BufferedSequentialPool::calculateNextBufferSize(int size) const
{
    int nextSize = d_buffer.bufferSize();

    if (bsls_BlockGrowth::BSLS_CONSTANT == d_growthStrategy) {
        return nextSize;                                              // RETURN
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
bdema_BufferedSequentialPool::bdema_BufferedSequentialPool(
                             char            *buffer,
                             int              size,
                             bslma_Allocator *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_buffer(buffer, size)
, d_growthStrategy(bsls_BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

bdema_BufferedSequentialPool::bdema_BufferedSequentialPool(
                                 char                       *buffer,
                                 int                         size,
                                 bsls_BlockGrowth::Strategy  growthStrategy,
                                 bslma_Allocator            *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_buffer(buffer, size)
, d_growthStrategy(growthStrategy)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

bdema_BufferedSequentialPool::bdema_BufferedSequentialPool(
                                 char                     *buffer,
                                 int                       size,
                                 bsls_Alignment::Strategy  alignmentStrategy,
                                 bslma_Allocator          *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_buffer(buffer, size, alignmentStrategy)
, d_growthStrategy(bsls_BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

bdema_BufferedSequentialPool::bdema_BufferedSequentialPool(
                                 char                       *buffer,
                                 int                         size,
                                 bsls_BlockGrowth::Strategy  growthStrategy,
                                 bsls_Alignment::Strategy    alignmentStrategy,
                                 bslma_Allocator            *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_buffer(buffer, size, alignmentStrategy)
, d_growthStrategy(growthStrategy)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

bdema_BufferedSequentialPool::bdema_BufferedSequentialPool(
                                 char            *buffer,
                                 int              size,
                                 int              maxBufferSize,
                                 bslma_Allocator *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_buffer(buffer, size)
, d_growthStrategy(bsls_BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(size <= maxBufferSize);
}

bdema_BufferedSequentialPool::bdema_BufferedSequentialPool(
                                 char                       *buffer,
                                 int                         size,
                                 int                         maxBufferSize,
                                 bsls_BlockGrowth::Strategy  growthStrategy,
                                 bslma_Allocator            *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_buffer(buffer, size)
, d_growthStrategy(growthStrategy)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(size <= maxBufferSize);
}

bdema_BufferedSequentialPool::bdema_BufferedSequentialPool(
                                 char                     *buffer,
                                 int                       size,
                                 int                       maxBufferSize,
                                 bsls_Alignment::Strategy  alignmentStrategy,
                                 bslma_Allocator          *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_buffer(buffer, size, alignmentStrategy)
, d_growthStrategy(bsls_BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(size <= maxBufferSize);
}

bdema_BufferedSequentialPool::bdema_BufferedSequentialPool(
                                 char                       *buffer,
                                 int                         size,
                                 int                         maxBufferSize,
                                 bsls_BlockGrowth::Strategy  growthStrategy,
                                 bsls_Alignment::Strategy    alignmentStrategy,
                                 bslma_Allocator            *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_buffer(buffer, size, alignmentStrategy)
, d_growthStrategy(growthStrategy)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(size <= maxBufferSize);
}

// MANIPULATORS
void *bdema_BufferedSequentialPool::allocate(bsls_Types::size_type size)
{
    BSLS_ASSERT(0 <= size);

    // TBD: Change this block to 'BSLS_ASSERT(0 < size)' after robo is clean.
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

    void *result = d_buffer.allocate(size);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(result)) {
        return result;                                                // RETURN
    }

    const int nextSize = calculateNextBufferSize(size);

    if (nextSize < size) {
        return d_blockList.allocate(size);                            // RETURN
    }

    // Manage the new buffer using 'bdema_Buffer'.

    d_buffer.replaceBuffer(static_cast<char *>(d_blockList.allocate(nextSize)),
                           nextSize);

    return d_buffer.allocateRaw(size);
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
