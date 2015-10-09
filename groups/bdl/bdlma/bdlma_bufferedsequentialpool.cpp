// bdlma_bufferedsequentialpool.cpp                                   -*-C++-*-
#include <bdlma_bufferedsequentialpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_bufferedsequentialpool_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <bsl_climits.h>  // 'INT_MAX'

enum {
    k_GROWTH_FACTOR = 2  // multiplicative factor by which to grow allocation
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
        nextSize *= k_GROWTH_FACTOR;
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

    const int nextSize = calculateNextBufferSize(static_cast<int>(size));

    if (nextSize < static_cast<int>(size)) {
        return d_blockList.allocate(static_cast<int>(size));          // RETURN
    }

    // Manage the new buffer using 'BufferManager'.

    d_buffer.replaceBuffer(static_cast<char *>(d_blockList.allocate(nextSize)),
                           nextSize);

    return d_buffer.allocateRaw(static_cast<int>(size));
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
