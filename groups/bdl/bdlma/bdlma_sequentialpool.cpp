// bdlma_sequentialpool.cpp                                           -*-C++-*-
#include <bdlma_sequentialpool.h>

#include <bsls_performancehint.h>

#include <bsl_climits.h>  // 'INT_MAX'

enum {
    INITIAL_SIZE  = 256,  // default initial allocation size (in bytes)

    GROWTH_FACTOR =   2   // multiplicative factor by which to grow allocation
                          // size
};

namespace BloombergLP {
namespace bdlma {

                        // --------------------
                        // class SequentialPool
                        // --------------------

// PRIVATE ACCESSORS
int SequentialPool::calculateNextBufferSize(int size) const
{
    const int bufferSize = d_buffer.bufferSize();
    int nextSize = 0 == bufferSize ? d_initialSize : bufferSize;

    if (bsls::BlockGrowth::BSLS_CONSTANT == d_growthStrategy) {
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
SequentialPool::SequentialPool(bslma::Allocator *basicAllocator)
: d_buffer()
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_initialSize(INITIAL_SIZE)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
}

SequentialPool::
SequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
               bslma::Allocator            *basicAllocator)
: d_buffer()
, d_growthStrategy(growthStrategy)
, d_initialSize(INITIAL_SIZE)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
}

SequentialPool::
SequentialPool(bsls::Alignment::Strategy  alignmentStrategy,
               bslma::Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_initialSize(INITIAL_SIZE)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
}

SequentialPool::
SequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
               bsls::Alignment::Strategy    alignmentStrategy,
               bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(growthStrategy)
, d_initialSize(INITIAL_SIZE)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
}

SequentialPool::
SequentialPool(int initialSize, bslma::Allocator *basicAllocator)
: d_buffer()
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_initialSize(initialSize)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SequentialPool::
SequentialPool(int                          initialSize,
               bsls::BlockGrowth::Strategy  growthStrategy,
               bslma::Allocator            *basicAllocator)
: d_buffer()
, d_growthStrategy(growthStrategy)
, d_initialSize(initialSize)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SequentialPool::
SequentialPool(int                        initialSize,
               bsls::Alignment::Strategy  alignmentStrategy,
               bslma::Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_initialSize(initialSize)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SequentialPool::
SequentialPool(int                          initialSize,
               bsls::BlockGrowth::Strategy  growthStrategy,
               bsls::Alignment::Strategy    alignmentStrategy,
               bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(growthStrategy)
, d_initialSize(initialSize)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SequentialPool::
SequentialPool(int               initialSize,
               int               maxBufferSize,
               bslma::Allocator *basicAllocator)
: d_buffer()
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_initialSize(initialSize)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SequentialPool::
SequentialPool(int                          initialSize,
               int                          maxBufferSize,
               bsls::BlockGrowth::Strategy  growthStrategy,
               bslma::Allocator            *basicAllocator)
: d_buffer()
, d_growthStrategy(growthStrategy)
, d_initialSize(initialSize)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SequentialPool::
SequentialPool(int                        initialSize,
               int                        maxBufferSize,
               bsls::Alignment::Strategy  alignmentStrategy,
               bslma::Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_initialSize(initialSize)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SequentialPool::
SequentialPool(int                          initialSize,
               int                          maxBufferSize,
               bsls::BlockGrowth::Strategy  growthStrategy,
               bsls::Alignment::Strategy    alignmentStrategy,
               bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(growthStrategy)
, d_initialSize(initialSize)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

// MANIPULATORS
void *SequentialPool::allocate(bsls::Types::size_type size)
{
    BSLS_ASSERT(0 < size);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(d_buffer.buffer())) {
        void *result = d_buffer.allocate(size);
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(result)) {
            return result;                                            // RETURN
        }
    }

    const int nextSize = calculateNextBufferSize(size);

    if (nextSize < static_cast<int>(size)) {
        return d_blockList.allocate(size);                            // RETURN
    }

    d_buffer.replaceBuffer(static_cast<char *>(d_blockList.allocate(nextSize)),
                           nextSize);

    return d_buffer.allocateRaw(size);
}

void *SequentialPool::allocateAndExpand(bsls::Types::size_type *size)
{
    BSLS_ASSERT(size);
    BSLS_ASSERT(0 < *size);

    void *result = allocate(*size);
    *size = d_buffer.expand(result, *size);

    return result;
}

void SequentialPool::reserveCapacity(int size)
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

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
