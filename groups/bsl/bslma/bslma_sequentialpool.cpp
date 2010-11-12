// bslma_sequentialpool.cpp    -*-C++-*-
#include <bslma_sequentialpool.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_bufferallocator.h>
#include <bslma_testallocator.h>       // for testing only

#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <climits>                     // for 'INT_MAX'

namespace {

enum {
    INITIAL_SIZE  = 256,  // default initial allocation size (divided by 2)

    GROWTH_FACTOR =   2   // multiplicative factor by which to grow allocation
                          // size
};

// Note: If 'GROWTH_FACTOR' is changed, 'calculateNextBufferSize' should be
// revisited for its implications on overflow.

}  // close unnamed namespace

namespace BloombergLP {

                             // --------------------
                             // bslma_SequentialPool
                             // --------------------

// PRIVATE ACCESSORS
int bslma_SequentialPool::calculateNextBufferSize(int size) const
{
    unsigned int nextSize = 0 == d_bufSize ? INITIAL_SIZE : d_bufSize;

    if (CONSTANT == d_growthStrategy) {
        return nextSize;
    }

    do {
        nextSize *= GROWTH_FACTOR;
    } while (nextSize < (unsigned) size);

    return nextSize < (unsigned) d_maxBufferSize ? nextSize : d_maxBufferSize;
}

// CREATORS
bslma_SequentialPool::bslma_SequentialPool(bslma_Allocator *basicAllocator)
: d_buffer(0)
, d_cursor(0)
, d_bufSize(0)
, d_alignmentStrategy(bslma_BufferAllocator::NATURAL_ALIGNMENT)
, d_maxBufferSize(INT_MAX)
, d_growthStrategy(GEOMETRIC)
, d_blockList(basicAllocator)
{
}

bslma_SequentialPool::bslma_SequentialPool(
                      bslma_BufferAllocator::AlignmentStrategy  strategy,
                      bslma_Allocator                          *basicAllocator)
: d_buffer(0)
, d_cursor(0)
, d_bufSize(0)
, d_alignmentStrategy(strategy)
, d_maxBufferSize(INT_MAX)
, d_growthStrategy(GEOMETRIC)
, d_blockList(basicAllocator)
{
}

bslma_SequentialPool::bslma_SequentialPool(int              initialSize,
                                           bslma_Allocator *basicAllocator)
: d_cursor(0)
, d_bufSize(initialSize < 0 ? -initialSize : initialSize)
, d_alignmentStrategy(bslma_BufferAllocator::NATURAL_ALIGNMENT)
, d_maxBufferSize(INT_MAX)
, d_growthStrategy(initialSize < 0 ? GEOMETRIC : CONSTANT)
, d_blockList(basicAllocator)
{
    d_buffer = (char *) d_blockList.allocate(d_bufSize);
}

bslma_SequentialPool::bslma_SequentialPool(
                      int                                       initialSize,
                      bslma_BufferAllocator::AlignmentStrategy  strategy,
                      bslma_Allocator                          *basicAllocator)
: d_cursor(0)
, d_bufSize(initialSize < 0 ? -initialSize : initialSize)
, d_alignmentStrategy(strategy)
, d_maxBufferSize(INT_MAX)
, d_growthStrategy(initialSize < 0 ? GEOMETRIC : CONSTANT)
, d_blockList(basicAllocator)
{
    d_buffer = (char *) d_blockList.allocate(d_bufSize);
}

bslma_SequentialPool::bslma_SequentialPool(char            *buffer,
                                           int              bufferSize,
                                           bslma_Allocator *basicAllocator)
: d_buffer(buffer)
, d_cursor(0)
, d_bufSize(bufferSize < 0 ? -bufferSize : bufferSize)
, d_alignmentStrategy(bslma_BufferAllocator::NATURAL_ALIGNMENT)
, d_maxBufferSize(INT_MAX)
, d_growthStrategy(bufferSize < 0 ? GEOMETRIC : CONSTANT)
, d_blockList(basicAllocator)
{
}

bslma_SequentialPool::bslma_SequentialPool(
                      char                                     *buffer,
                      int                                       bufferSize,
                      bslma_BufferAllocator::AlignmentStrategy  strategy,
                      bslma_Allocator                          *basicAllocator)
: d_buffer(buffer)
, d_cursor(0)
, d_bufSize(bufferSize < 0 ? -bufferSize : bufferSize)
, d_alignmentStrategy(strategy)
, d_maxBufferSize(INT_MAX)
, d_growthStrategy(bufferSize < 0 ? GEOMETRIC : CONSTANT)
, d_blockList(basicAllocator)
{
}

bslma_SequentialPool::bslma_SequentialPool(int              initialSize,
                                           int              maxBufferSize,
                                           bslma_Allocator *basicAllocator)
: d_cursor(0)
, d_bufSize(initialSize < 0 ? -initialSize : initialSize)
, d_alignmentStrategy(bslma_BufferAllocator::NATURAL_ALIGNMENT)
, d_maxBufferSize(maxBufferSize)
, d_growthStrategy(initialSize < 0 ? GEOMETRIC : CONSTANT)
, d_blockList(basicAllocator)
{
    d_buffer = (char *) d_blockList.allocate(d_bufSize);
}

bslma_SequentialPool::bslma_SequentialPool(
                      int                                       initialSize,
                      int                                       maxBufferSize,
                      bslma_BufferAllocator::AlignmentStrategy  strategy,
                      bslma_Allocator                          *basicAllocator)
: d_cursor(0)
, d_bufSize(initialSize < 0 ? -initialSize : initialSize)
, d_alignmentStrategy(strategy)
, d_maxBufferSize(maxBufferSize)
, d_growthStrategy(initialSize < 0 ? GEOMETRIC : CONSTANT)
, d_blockList(basicAllocator)
{
    d_buffer = (char *) d_blockList.allocate(d_bufSize);
}

bslma_SequentialPool::bslma_SequentialPool(char            *buffer,
                                           int              bufferSize,
                                           int              maxBufferSize,
                                           bslma_Allocator *basicAllocator)
: d_buffer(buffer)
, d_cursor(0)
, d_bufSize(bufferSize < 0 ? -bufferSize : bufferSize)
, d_alignmentStrategy(bslma_BufferAllocator::NATURAL_ALIGNMENT)
, d_maxBufferSize(maxBufferSize)
, d_growthStrategy(bufferSize < 0 ? GEOMETRIC : CONSTANT)
, d_blockList(basicAllocator)
{
}

bslma_SequentialPool::bslma_SequentialPool(
                      char                                     *buffer,
                      int                                       bufferSize,
                      int                                       maxBufferSize,
                      bslma_BufferAllocator::AlignmentStrategy  strategy,
                      bslma_Allocator                          *basicAllocator)
: d_buffer(buffer)
, d_cursor(0)
, d_bufSize(bufferSize < 0 ? -bufferSize : bufferSize)
, d_alignmentStrategy(strategy)
, d_maxBufferSize(maxBufferSize)
, d_growthStrategy(bufferSize < 0 ? GEOMETRIC : CONSTANT)
, d_blockList(basicAllocator)
{
}

// MANIPULATORS
void *bslma_SequentialPool::allocate(int size)
{
    BSLS_ASSERT(0 <= size);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        return 0;                                                     // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(d_buffer)) {
        void *memory = bslma_BufferAllocator::allocateFromBuffer(
                                                          &d_cursor,
                                                          d_buffer,
                                                          d_bufSize,
                                                          size,
                                                          d_alignmentStrategy);
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(memory)) {
            return memory;                                            // RETURN
        }
    }

    int nextSize = calculateNextBufferSize(size);
    if (nextSize < size) {
        return d_blockList.allocate(size);                            // RETURN
    }

    d_buffer  = (char *) d_blockList.allocate(nextSize);
    d_bufSize = nextSize;
    d_cursor  = 0;

    return bslma_BufferAllocator::allocateFromBuffer(&d_cursor,
                                                     d_buffer,
                                                     d_bufSize,
                                                     size,
                                                     d_alignmentStrategy);
}

void bslma_SequentialPool::release()
{
    d_blockList.release();
    d_cursor  = 0;
    d_buffer  = 0;
    d_bufSize = 0;
}

void bslma_SequentialPool::reserveCapacity(int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    if (numBytes) {
        int   cursor = d_cursor;
        void *memory = d_buffer
                     ? bslma_BufferAllocator::allocateFromBuffer(
                                                           &cursor,
                                                           d_buffer,
                                                           d_bufSize,
                                                           numBytes,
                                                           d_alignmentStrategy)
                     : 0;

        if (0 == memory) {
            // Current buffer insufficient, so replenish.

            int nextSize = calculateNextBufferSize(numBytes);
            if (nextSize < numBytes) {
                // Ignore the maximum buffer limit.  Allocate the memory that
                // the user wants to reserve.
                nextSize = numBytes;
            }

            d_buffer  = (char *) d_blockList.allocate(nextSize);
            d_bufSize = nextSize;
            d_cursor  = 0;
        }
    }
}

void *bslma_SequentialPool::allocateAndExpand(int *size)
{
    BSLS_ASSERT(0 <= *size);

    void *memory = allocate(*size);
    if (memory) {
        if (d_buffer <= memory && memory < d_buffer + d_bufSize) {
            // Memory is managed and not directly from the block list.

            *size += d_bufSize - d_cursor;
            d_cursor = d_bufSize;
        }
    }
    else {
        *size = 0;
    }

    return memory;
}

void *bslma_SequentialPool::allocateAndExpand(int *size, int maxNumBytes)
{
    BSLS_ASSERT(0 <= *size);  BSLS_ASSERT(*size <= maxNumBytes);

    void *memory = allocate(*size);
    if (memory) {
        if (d_buffer <= memory && memory < d_buffer + d_bufSize) {
            // Memory is managed and not directly from the block list.

            int newNumBytes = *size + d_bufSize - d_cursor;
            if (maxNumBytes < newNumBytes) {
                newNumBytes = maxNumBytes;
            }
            d_cursor += newNumBytes - *size;
            *size = newNumBytes;
        }
    }
    else {
        *size = 0;
    }

    return memory;
}

int bslma_SequentialPool::expand(void *address, int originalNumBytes)
{
    BSLS_ASSERT(0 <= originalNumBytes);

    if ((char *)address + originalNumBytes == d_buffer + d_cursor) {
        // Memory is managed and not directly from the block list.

        int newNumBytes = originalNumBytes + d_bufSize - d_cursor;
        d_cursor = d_bufSize;
        return newNumBytes;                                           // RETURN
    }

    return originalNumBytes;
}

int bslma_SequentialPool::expand(void *address,
                                 int   originalNumBytes,
                                 int   maxNumBytes)
{
    BSLS_ASSERT(               0 <= originalNumBytes);
    BSLS_ASSERT(originalNumBytes <= maxNumBytes);

    if ((char *)address + originalNumBytes == d_buffer + d_cursor) {
        // Memory is managed and not directly from the block list.

        int newNumBytes = originalNumBytes + d_bufSize - d_cursor;
        if (maxNumBytes < newNumBytes) {
            newNumBytes = maxNumBytes;
        }
        d_cursor += newNumBytes - originalNumBytes;

        return newNumBytes;                                           // RETURN
    }

    return originalNumBytes;
}

int bslma_SequentialPool::truncate(void *address,
                                   int   originalNumBytes,
                                   int   newNumBytes)
{
    BSLS_ASSERT_SAFE(newNumBytes <= originalNumBytes);

    if ((char *)address + originalNumBytes == d_buffer + d_cursor) {
        // Memory is managed and not directly from the block list.

        d_cursor -= originalNumBytes - newNumBytes;

        return newNumBytes;                                           // RETURN
    }

    return originalNumBytes;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
