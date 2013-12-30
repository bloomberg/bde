// bdlma_bufferedsequentialpool.cpp                                   -*-C++-*-
#include <bdlma_bufferedsequentialpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_bufferedsequentialpool_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <bsl_climits.h>  // 'INT_MAX'

enum {
    GROWTH_FACTOR = 2  // multiplicative factor by which to grow allocation
                       // size
};

namespace BloombergLP {
namespace bdlma {

                    // ----------------------------
                    // class BufferedSequentialPool
                    // ----------------------------

// PRIVATE ACCESSORS
int BufferedSequentialPool::calculateNextBufferSize(int size) const
{
    int nextSize = d_buffer.bufferSize();

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
BufferedSequentialPool::BufferedSequentialPool(
                                              char             *buffer,
                                              int               size,
                                              bslma::Allocator *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_buffer(buffer, size)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

BufferedSequentialPool::BufferedSequentialPool(
                                   char                        *buffer,
                                   int                          size,
                                   bsls::BlockGrowth::Strategy  growthStrategy,
                                   bslma::Allocator            *basicAllocator)
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

BufferedSequentialPool::BufferedSequentialPool(
                                  char                      *buffer,
                                  int                        size,
                                  bsls::Alignment::Strategy  alignmentStrategy,
                                  bslma::Allocator          *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_buffer(buffer, size, alignmentStrategy)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
}

BufferedSequentialPool::BufferedSequentialPool(
                                char                        *buffer,
                                int                          size,
                                bsls::BlockGrowth::Strategy  growthStrategy,
                                bsls::Alignment::Strategy    alignmentStrategy,
                                bslma::Allocator            *basicAllocator)
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

BufferedSequentialPool::BufferedSequentialPool(
                                              char             *buffer,
                                              int               size,
                                              int               maxBufferSize,
                                              bslma::Allocator *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_buffer(buffer, size)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(size <= maxBufferSize);
}

BufferedSequentialPool::BufferedSequentialPool(
                                 char                        *buffer,
                                 int                          size,
                                 int                          maxBufferSize,
                                 bsls::BlockGrowth::Strategy  growthStrategy,
                                 bslma::Allocator            *basicAllocator)
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

BufferedSequentialPool::BufferedSequentialPool(
                                  char                      *buffer,
                                  int                        size,
                                  int                        maxBufferSize,
                                  bsls::Alignment::Strategy  alignmentStrategy,
                                  bslma::Allocator          *basicAllocator)
: d_initialBuffer_p(buffer)
, d_initialSize(size)
, d_buffer(buffer, size, alignmentStrategy)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(size <= maxBufferSize);
}

BufferedSequentialPool::BufferedSequentialPool(
                                char                        *buffer,
                                int                          size,
                                int                          maxBufferSize,
                                bsls::BlockGrowth::Strategy  growthStrategy,
                                bsls::Alignment::Strategy    alignmentStrategy,
                                bslma::Allocator            *basicAllocator)
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
void *BufferedSequentialPool::allocate(bsls::Types::size_type size)
{
    BSLS_ASSERT(0 < size);

    void *result = d_buffer.allocate(size);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(result)) {
        return result;                                                // RETURN
    }

    const int nextSize = calculateNextBufferSize(size);

    if (nextSize < static_cast<int>(size)) {
        return d_blockList.allocate(size);                            // RETURN
    }

    // Manage the new buffer using 'BufferManager'.

    d_buffer.replaceBuffer(static_cast<char *>(d_blockList.allocate(nextSize)),
                           nextSize);

    return d_buffer.allocateRaw(size);
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
