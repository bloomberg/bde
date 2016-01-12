// bdlma_sequentialpool.cpp                                           -*-C++-*-
#include <bdlma_sequentialpool.h>

#include <bslma_default.h>

#include <bsl_climits.h>  // 'INT_MAX'

enum {
    k_INITIAL_SIZE  = 256,  // default initial allocation size (in bytes)

    k_GROWTH_FACTOR =   2   // multiplicative factor by which to grow
                            // allocation size
};

namespace BloombergLP {

// HELPER FUNCTIONS
inline
static int alignedAllocationSize(int size, int sizeOfBlock)
    // Return the allocation size (in bytes) required to ensure proper
    // alignment for a 'bdlma::SequentialPool::Block' containing a
    // maximally-aligned payload of the specified 'size', where the specified
    // 'sizeOfBlock' is presumed to be 'sizeof(bdlma::SequentialPool::Block)'.
    // Note that, to ensure that both the payload and header portions of the
    // allocated memory are each separately guaranteed to be maximally aligned
    // in the presence of a supplied allocator returning naturally-aligned
    // memory, the size of the overall allocation will be rounded up to an
    // integral multiple of 'bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT'.
{
    ///IMPLEMENTATION NOTE
    ///-------------------
    //: 1 Append the size of the 'bdlma::SequentialPool::Block' header:
    //:   'size += sizeof(Block) - bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT'
    //:
    //: 2 Round to the nearest multiple of MAX_ALIGNMENT (a power of 2):
    //:   'size = (size + MAX_ALIGNMENT - 1) & ~(MAX_ALIGNMENT - 1)'

    return (size + sizeOfBlock - 1)
           & ~(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);
}

bsls::Types::size_type calculateNextBufferSize(
                                            bsls::Types::size_type initialSize,
                                            int                    size)
    // Return the next buffer size (in bytes) that is sufficiently large to
    // satisfy a memory allocation request of the specified 'size' (in bytes)
    // assuming geometric growth from the specifieid 'initialSize'.  The
    // behavior is undefined unless '0 < size'.
{
    BSLS_ASSERT(0 < size);

    bsls::Types::size_type nextSize = initialSize;

    do {
        nextSize *= k_GROWTH_FACTOR;
    } while (nextSize < static_cast<bsls::Types::size_type>(size));

    return nextSize;
}

namespace bdlma {

                           // --------------------
                           // class SequentialPool
                           // --------------------

// PRIVATE MANIPULATORS
void SequentialPool::allocateBlock(bsls::Types::size_type size)
{
    BSLS_ASSERT(0 < size);

    Block *block = reinterpret_cast<Block *>(d_allocator_p->allocate(
                      alignedAllocationSize(static_cast<int>(size),
                                            static_cast<int>(sizeof(Block)))));

    block->d_next_p = *d_reuseHead_p;
    block->d_size   = size;
    *d_reuseHead_p  = block;
    d_reuseHead_p   = &block->d_next_p;

    d_buffer.replaceBuffer(reinterpret_cast<char *>(&block->d_memory),
                           static_cast<int>(size));
}

void *SequentialPool::allocateNonFastPath(bsls::Types::size_type size)
{
    BSLS_ASSERT(0 < size);

    // Prioritize reuse.

    if (*d_reuseHead_p && (*d_reuseHead_p)->d_size >= size) {
        d_buffer.replaceBuffer(reinterpret_cast<char *>(
                                                  &(*d_reuseHead_p)->d_memory),
                               static_cast<int>((*d_reuseHead_p)->d_size));

        d_reuseHead_p = &(*d_reuseHead_p)->d_next_p;

        return d_buffer.allocateRaw(static_cast<int>(size));          // RETURN
    }

    // Determine size of the new block to be allocated.

    bsls::Types::size_type allocationSize;

    if (size <= d_minSize) {
        // Constant growth strategy.

        allocationSize = d_minSize;
    }
    else {
        // Geometric growth and large request strategies.

        bsls::Types::size_type nextSize = calculateNextBufferSize(
                                                       d_lastSize,
                                                       static_cast<int>(size));

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(nextSize <= d_maxSize)) {
            allocationSize = nextSize;
            d_lastSize = nextSize;
        }
        else {
            allocationSize = size;
        }
    }

    // Allocate the new block and return the requested bytes.

    allocateBlock(allocationSize);

    return d_buffer.allocateRaw(static_cast<int>(size));
}

// CREATORS
SequentialPool::SequentialPool(bslma::Allocator *basicAllocator)
: d_buffer()
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_minSize(0)
, d_maxSize(INT_MAX)
, d_lastSize(k_INITIAL_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

SequentialPool::SequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
                               bslma::Allocator            *basicAllocator)
: d_buffer()
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : k_INITIAL_SIZE)
, d_maxSize(INT_MAX)
, d_lastSize(k_INITIAL_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

SequentialPool::SequentialPool(bsls::Alignment::Strategy  alignmentStrategy,
                               bslma::Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_minSize(0)
, d_maxSize(INT_MAX)
, d_lastSize(k_INITIAL_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

SequentialPool::SequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
                               bsls::Alignment::Strategy    alignmentStrategy,
                               bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : k_INITIAL_SIZE)
, d_maxSize(INT_MAX)
, d_lastSize(k_INITIAL_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

SequentialPool::SequentialPool(int               initialSize,
                               bslma::Allocator *basicAllocator)
: d_buffer()
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_minSize(0)
, d_maxSize(INT_MAX)
, d_lastSize(initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 < initialSize);

    allocateBlock(initialSize);
}

SequentialPool::SequentialPool(int                          initialSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               bslma::Allocator            *basicAllocator)
: d_buffer()
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : initialSize)
, d_maxSize(INT_MAX)
, d_lastSize(initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 < initialSize);

    allocateBlock(initialSize);
}

SequentialPool::SequentialPool(int                        initialSize,
                               bsls::Alignment::Strategy  alignmentStrategy,
                               bslma::Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_minSize(0)
, d_maxSize(INT_MAX)
, d_lastSize(initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 < initialSize);

    allocateBlock(initialSize);
}

SequentialPool::SequentialPool(int                          initialSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               bsls::Alignment::Strategy    alignmentStrategy,
                               bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : initialSize)
, d_maxSize(INT_MAX)
, d_lastSize(initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 < initialSize);

    allocateBlock(initialSize);
}

SequentialPool::SequentialPool(int               initialSize,
                               int               maxBufferSize,
                               bslma::Allocator *basicAllocator)
: d_buffer()
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_minSize(0)
, d_maxSize(maxBufferSize)
, d_lastSize(initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    allocateBlock(initialSize);
}

SequentialPool::SequentialPool(int                          initialSize,
                               int                          maxBufferSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               bslma::Allocator            *basicAllocator)
: d_buffer()
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : initialSize)
, d_maxSize(maxBufferSize)
, d_lastSize(initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    allocateBlock(initialSize);
}

SequentialPool::SequentialPool(int                        initialSize,
                               int                        maxBufferSize,
                               bsls::Alignment::Strategy  alignmentStrategy,
                               bslma::Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_minSize(0)
, d_maxSize(maxBufferSize)
, d_lastSize(initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    allocateBlock(initialSize);
}

SequentialPool::SequentialPool(int                          initialSize,
                               int                          maxBufferSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               bsls::Alignment::Strategy    alignmentStrategy,
                               bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : initialSize)
, d_maxSize(maxBufferSize)
, d_lastSize(initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    allocateBlock(initialSize);
}

SequentialPool::SequentialPool(
                            int                          initialSize,
                            int                          maxBufferSize,
                            bsls::BlockGrowth::Strategy  growthStrategy,
                            bsls::Alignment::Strategy    alignmentStrategy,
                            bool                         allocateInitialBuffer,
                            bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : initialSize)
, d_maxSize(maxBufferSize)
, d_lastSize(initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    if (allocateInitialBuffer) {
        allocateBlock(initialSize);
    }
}

// MANIPULATORS
void SequentialPool::release()
{
    // 'BufferManager::release' keeps the buffer and just resets the internal
    // cursor, so 'reset' is used instead.

    d_buffer.reset();

    d_reuseHead_p = &d_head_p;

    while (d_head_p) {
        void *lastBlock = d_head_p;
        d_head_p        = d_head_p->d_next_p;
        d_allocator_p->deallocate(lastBlock);
    }
}

void SequentialPool::reserveCapacity(int numBytes)
{
    BSLS_ASSERT(0 < numBytes);

    // If 'd_buffer.bufferSize()' is 0, 'd_buffer' is not currently managing a
    // buffer.

    if (d_buffer.bufferSize() && d_buffer.hasSufficientCapacity(numBytes)) {
        return;                                                       // RETURN
    }

    // Check if reuse of a block will be possible.

    if (*d_reuseHead_p && (*d_reuseHead_p)->d_size >=
                               static_cast<bsls::Types::size_type>(numBytes)) {
        return;                                                       // RETURN
    }

    // Determine size of the new block to be allocated.

    bsls::Types::size_type allocationSize;

    if (static_cast<bsls::Types::size_type>(numBytes) <= d_minSize) {
        // Constant growth strategy.

        allocationSize = d_minSize;
    }
    else {
        // Geometric growth and large request strategies.

        bsls::Types::size_type nextSize = calculateNextBufferSize(d_lastSize,
                                                                  numBytes);

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(nextSize <= d_maxSize)) {
            allocationSize = nextSize;
            d_lastSize = nextSize;
        }
        else {
            allocationSize = numBytes;
        }
    }

    // Allocate the new block.

    Block *block = reinterpret_cast<Block *>(d_allocator_p->allocate(
                      alignedAllocationSize(static_cast<int>(allocationSize),
                                            static_cast<int>(sizeof(Block)))));

    block->d_next_p = *d_reuseHead_p;
    block->d_size   = allocationSize;
    *d_reuseHead_p  = block;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
