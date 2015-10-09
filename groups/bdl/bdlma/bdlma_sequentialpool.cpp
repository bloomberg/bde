// bdlma_sequentialpool.cpp                                           -*-C++-*-
#include <bdlma_sequentialpool.h>

#include <bsls_performancehint.h>

#include <bsl_climits.h>  // 'INT_MAX'

enum {
    k_INITIAL_SIZE  = 256,  // default initial allocation size (in bytes)

    k_GROWTH_FACTOR =   2   // multiplicative factor by which to grow
                            // allocation size
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
        nextSize *= k_GROWTH_FACTOR;
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
, d_initialSize(k_INITIAL_SIZE)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
}

SequentialPool::
SequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
               bslma::Allocator            *basicAllocator)
: d_buffer()
, d_growthStrategy(growthStrategy)
, d_initialSize(k_INITIAL_SIZE)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
{
}

SequentialPool::
SequentialPool(bsls::Alignment::Strategy  alignmentStrategy,
               bslma::Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_initialSize(k_INITIAL_SIZE)
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
, d_initialSize(k_INITIAL_SIZE)
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

    const int nextSize = calculateNextBufferSize(static_cast<int>(size));

    if (nextSize < static_cast<int>(size)) {
        return d_blockList.allocate(static_cast<int>(size));          // RETURN
    }

    d_buffer.replaceBuffer(static_cast<char *>(d_blockList.allocate(nextSize)),
                           nextSize);

    return d_buffer.allocateRaw(static_cast<int>(size));
}

void *SequentialPool::allocateAndExpand(bsls::Types::size_type *size)
{
    BSLS_ASSERT(size);
    BSLS_ASSERT(0 < *size);

    void *result = allocate(static_cast<int>(*size));
    *size = d_buffer.expand(result, static_cast<int>(*size));

    return result;
}

void SequentialPool::reserveCapacity(int numBytes)
{
    BSLS_ASSERT(0 < numBytes);

    // If 'd_buffer.bufferSize()' is 0, 'd_buffer' is not managing any buffer
    // currently.

    if (0 != d_buffer.bufferSize()
     && d_buffer.hasSufficientCapacity(numBytes)) {
        return;                                                       // RETURN
    }

    int nextSize = calculateNextBufferSize(numBytes);

    if (nextSize < numBytes) {
        nextSize = numBytes;
    }

    d_buffer.replaceBuffer(static_cast<char *>(d_blockList.allocate(nextSize)),
                           nextSize);
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
