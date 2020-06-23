// bdlma_smallsequentialpool.cpp                                      -*-C++-*-
#include <bdlma_smallsequentialpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_smallsequentialpool_cpp, "$Id$ $CSID$")

#include <bsl_climits.h>  // 'INT_MAX'

enum {
    k_INITIAL_SIZE  = 256,  // default initial allocation size (in bytes)

    k_GROWTH_FACTOR =   2   // multiplicative factor by which to grow
                            // allocation size
};

namespace BloombergLP {
namespace bdlma {

                           // -------------------------
                           // class SmallSequentialPool
                           // -------------------------

// PRIVATE MANIPULATORS
void *SmallSequentialPool::allocateNonFastPath(bsl::size_t size)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;                                                     // RETURN
    }

    const bsl::size_t nextSize = calculateNextBufferSize(size);

    if (nextSize < size) {
        return d_largeBlockList.allocate(size);                       // RETURN
    }

    d_buffer.replaceBuffer(static_cast<char *>(d_blockList.allocate(nextSize)),
                           nextSize);

    return d_buffer.allocateRaw(size);
}

// PRIVATE ACCESSORS
bsl::size_t SmallSequentialPool::calculateNextBufferSize(bsl::size_t size)
                                                                          const
{
    const bsl::size_t bufferSize = d_buffer.bufferSize();
    bsl::size_t       nextSize   = 0 == bufferSize
                                 ? d_initialSize
                                 : bufferSize;

    if (bsls::BlockGrowth::BSLS_CONSTANT == d_growthStrategy) {
        return nextSize;                                              // RETURN
    }

    if (0 == bufferSize && size <= nextSize) {
        ;  // First allocation satisfied by initial size.
    } else {
        bsl::size_t oldSize;
        do {
            oldSize   = nextSize;
            nextSize *= k_GROWTH_FACTOR;
        } while (nextSize < size && oldSize < nextSize);

        // If 'nextSize' overflows, use 'oldSize'.

        if (oldSize >= nextSize) {
            nextSize = oldSize;
        }
    }

    return nextSize <= d_maxBufferSize ? nextSize : d_maxBufferSize;
}

// CREATORS
SmallSequentialPool::SmallSequentialPool(bslma::Allocator *basicAllocator)
: d_buffer()
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_initialSize(k_INITIAL_SIZE)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
, d_largeBlockList(basicAllocator)
{
}

SmallSequentialPool::
SmallSequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
                    bslma::Allocator            *basicAllocator)
: d_buffer()
, d_growthStrategy(growthStrategy)
, d_initialSize(k_INITIAL_SIZE)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
, d_largeBlockList(basicAllocator)
{
}

SmallSequentialPool::
SmallSequentialPool(bsls::Alignment::Strategy  alignmentStrategy,
                    bslma::Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_initialSize(k_INITIAL_SIZE)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
, d_largeBlockList(basicAllocator)
{
}

SmallSequentialPool::
SmallSequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
                    bsls::Alignment::Strategy    alignmentStrategy,
                    bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(growthStrategy)
, d_initialSize(k_INITIAL_SIZE)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
, d_largeBlockList(basicAllocator)
{
}

SmallSequentialPool::
SmallSequentialPool(int initialSize, bslma::Allocator *basicAllocator)
: d_buffer()
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_initialSize(initialSize)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
, d_largeBlockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SmallSequentialPool::
SmallSequentialPool(bsl::size_t                  initialSize,
                    bsls::BlockGrowth::Strategy  growthStrategy,
                    bslma::Allocator            *basicAllocator)
: d_buffer()
, d_growthStrategy(growthStrategy)
, d_initialSize(initialSize)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
, d_largeBlockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SmallSequentialPool::
SmallSequentialPool(bsl::size_t                initialSize,
                    bsls::Alignment::Strategy  alignmentStrategy,
                    bslma::Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_initialSize(initialSize)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
, d_largeBlockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SmallSequentialPool::
SmallSequentialPool(bsl::size_t                  initialSize,
                    bsls::BlockGrowth::Strategy  growthStrategy,
                    bsls::Alignment::Strategy    alignmentStrategy,
                    bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(growthStrategy)
, d_initialSize(initialSize)
, d_maxBufferSize(INT_MAX)
, d_blockList(basicAllocator)
, d_largeBlockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SmallSequentialPool::
SmallSequentialPool(bsl::size_t       initialSize,
                    bsl::size_t       maxBufferSize,
                    bslma::Allocator *basicAllocator)
: d_buffer()
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_initialSize(initialSize)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
, d_largeBlockList(basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SmallSequentialPool::
SmallSequentialPool(bsl::size_t                  initialSize,
                    bsl::size_t                  maxBufferSize,
                    bsls::BlockGrowth::Strategy  growthStrategy,
                    bslma::Allocator            *basicAllocator)
: d_buffer()
, d_growthStrategy(growthStrategy)
, d_initialSize(initialSize)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
, d_largeBlockList(basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SmallSequentialPool::
SmallSequentialPool(bsl::size_t                initialSize,
                    bsl::size_t                maxBufferSize,
                    bsls::Alignment::Strategy  alignmentStrategy,
                    bslma::Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_initialSize(initialSize)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
, d_largeBlockList(basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SmallSequentialPool::
SmallSequentialPool(bsl::size_t                  initialSize,
                    bsl::size_t                  maxBufferSize,
                    bsls::BlockGrowth::Strategy  growthStrategy,
                    bsls::Alignment::Strategy    alignmentStrategy,
                    bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(growthStrategy)
, d_initialSize(initialSize)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
, d_largeBlockList(basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer = static_cast<char *>(d_blockList.allocate(initialSize));
    d_buffer.replaceBuffer(buffer, initialSize);
}

SmallSequentialPool::
SmallSequentialPool(bsl::size_t                  initialSize,
                    bsl::size_t                  maxBufferSize,
                    bsls::BlockGrowth::Strategy  growthStrategy,
                    bsls::Alignment::Strategy    alignmentStrategy,
                    bool                         allocateInitialBuffer,
                    bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_growthStrategy(growthStrategy)
, d_initialSize(initialSize)
, d_maxBufferSize(maxBufferSize)
, d_blockList(basicAllocator)
, d_largeBlockList(basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    if (allocateInitialBuffer) {
        reserveCapacity(initialSize);
    }
}

// MANIPULATORS
void SmallSequentialPool::reserveCapacity(bsl::size_t numBytes)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == numBytes)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return;                                                       // RETURN
    }

    // If 'd_buffer.bufferSize()' is 0, 'd_buffer' is not managing any buffer
    // currently.

    if (0 != d_buffer.bufferSize()
     && d_buffer.hasSufficientCapacity(numBytes)) {
        return;                                                       // RETURN
    }

    bsl::size_t nextSize = calculateNextBufferSize(numBytes);

    if (nextSize < numBytes) {
        nextSize = numBytes;
    }

    d_buffer.replaceBuffer(static_cast<char *>(d_blockList.allocate(nextSize)),
                           nextSize);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
