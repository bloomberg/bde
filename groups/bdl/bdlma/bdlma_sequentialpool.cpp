// bdlma_sequentialpool.cpp                                           -*-C++-*-
#include <bdlma_sequentialpool.h>

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
    // 'sizeOfBlock' is presumed to be
    // 'sizeof(bdlma::SequentialPool::Block)'.  Note that, to ensure
    // that both the payload and header portions of the allocated memory are
    // each separately guaranteed to be maximally aligned in the presence of a
    // supplied allocator returning naturally-aligned memory, the size of the
    // overall allocation will be rounded up to an integral multiple of
    // 'bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT'.
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

namespace bdlma {

                           // --------------------
                           // class SequentialPool
                           // --------------------

// PRIVATE MANIPULATORS
void *SequentialPool::allocateNonFastPath(BufferManager          **buffer,
                                          bsls::Types::size_type   size)
{
    *buffer = &d_buffer;
    
    if (static_cast<bsls::Types::size_type>(d_constantBlockSize) >= size) {
        // Constant allocation.

        if (d_reuseHead_p) {
            (*buffer)->replaceBuffer(reinterpret_cast<char *>(
                                                     &d_reuseHead_p->d_memory),
                                   d_constantBlockSize);

            d_reuseHead_p = d_reuseHead_p->d_next_p;

            return (*buffer)->allocateRaw(static_cast<int>(size));    // RETURN
        }
        
        Block *block = reinterpret_cast<Block *>(
                                    d_geometricBlockList.allocator()->allocate(
                                               d_constantBlockAllocationSize));

        (*buffer)->replaceBuffer(reinterpret_cast<char *>(&block->d_memory),
                                 d_constantBlockSize);

        block->d_next_p = d_head_p;
        d_head_p        = block;

        return (*buffer)->allocateRaw(static_cast<int>(size));        // RETURN
    }
    
    if (d_constantBlockSize) {
        // Constant strategy after constant allocation failure.

        *buffer = &d_secondaryBuffer;

        void *result = d_secondaryBuffer.allocate(size);
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(result)) {
            return result;                                            // RETURN
        }
    }
    
    // Geometric allocation.
    
    unsigned int nextSize = calculateNextBufferSize(static_cast<int>(size));
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                       nextSize <= d_maxGeometricBufferSize)) {
        (*buffer)->replaceBuffer(
                  static_cast<char *>(d_geometricBlockList.allocate(nextSize)),
                  nextSize);
        return (*buffer)->allocateRaw(static_cast<int>(size));
    }

    // Allocation for very large requests.

    *buffer = 0;

    return d_geometricBlockList.allocate(static_cast<int>(size));
}

// PRIVATE ACCESSORS
unsigned int SequentialPool::calculateNextBufferSize(int size) const
{
    unsigned int nextSize =   0 == d_buffer.bufferSize()
                            ? d_initialSize
                            : d_buffer.bufferSize();

    do {
        nextSize *= k_GROWTH_FACTOR;
    } while (nextSize < static_cast<unsigned>(size));

    return nextSize;
}

// CREATORS
SequentialPool::SequentialPool(bslma::Allocator *basicAllocator)
: d_buffer()
, d_secondaryBuffer()
, d_head_p(0)
, d_reuseHead_p(0)
, d_initialSize(k_INITIAL_SIZE)
, d_constantBlockSize(0)
, d_constantBlockAllocationSize(0)
, d_maxGeometricBufferSize(INT_MAX)
, d_geometricBlockList(basicAllocator)
{
}

SequentialPool::SequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
                               bslma::Allocator            *basicAllocator)
: d_buffer()
, d_secondaryBuffer()
, d_head_p(0)
, d_reuseHead_p(0)
, d_initialSize(k_INITIAL_SIZE)
, d_constantBlockSize(growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
                      ? 0
                      : d_initialSize)
, d_constantBlockAllocationSize(alignedAllocationSize(d_constantBlockSize,
                                                      sizeof(Block)))
, d_maxGeometricBufferSize(INT_MAX)
, d_geometricBlockList(basicAllocator)
{
}

SequentialPool::SequentialPool(bsls::Alignment::Strategy  alignmentStrategy,
                               bslma::Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_secondaryBuffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(0)
, d_initialSize(k_INITIAL_SIZE)
, d_constantBlockSize(0)
, d_constantBlockAllocationSize(0)
, d_maxGeometricBufferSize(INT_MAX)
, d_geometricBlockList(basicAllocator)
{
}

SequentialPool::SequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
                               bsls::Alignment::Strategy    alignmentStrategy,
                               bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_secondaryBuffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(0)
, d_initialSize(k_INITIAL_SIZE)
, d_constantBlockSize(growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
                      ? 0
                      : d_initialSize)
, d_constantBlockAllocationSize(alignedAllocationSize(d_constantBlockSize,
                                                      sizeof(Block)))
, d_maxGeometricBufferSize(INT_MAX)
, d_geometricBlockList(basicAllocator)
{
}

SequentialPool::SequentialPool(int               initialSize,
                               bslma::Allocator *basicAllocator)
: d_buffer()
, d_secondaryBuffer()
, d_head_p(0)
, d_reuseHead_p(0)
, d_initialSize(initialSize)
, d_constantBlockSize(0)
, d_constantBlockAllocationSize(0)
, d_maxGeometricBufferSize(INT_MAX)
, d_geometricBlockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer = static_cast<char *>(d_geometricBlockList.allocate(
                                                               d_initialSize));
    d_buffer.replaceBuffer(buffer, d_initialSize);
}

SequentialPool::SequentialPool(int                          initialSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               bslma::Allocator            *basicAllocator)
: d_buffer()
, d_secondaryBuffer()
, d_head_p(0)
, d_reuseHead_p(0)
, d_initialSize(initialSize)
, d_constantBlockSize(growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
                      ? 0
                      : d_initialSize)
, d_constantBlockAllocationSize(alignedAllocationSize(d_constantBlockSize,
                                                      sizeof(Block)))
, d_maxGeometricBufferSize(INT_MAX)
, d_geometricBlockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer;

    if (0 < d_constantBlockSize) {
        buffer = static_cast<char *>(
                    d_geometricBlockList.allocator()->allocate(d_initialSize));
    }
    else {
        buffer = static_cast<char *>(d_geometricBlockList.allocate(
                                                               d_initialSize));
    }

    d_buffer.replaceBuffer(buffer, d_initialSize);
}

SequentialPool::SequentialPool(int                        initialSize,
                               bsls::Alignment::Strategy  alignmentStrategy,
                               bslma::Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_secondaryBuffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(0)
, d_initialSize(initialSize)
, d_constantBlockSize(0)
, d_constantBlockAllocationSize(0)
, d_maxGeometricBufferSize(INT_MAX)
, d_geometricBlockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer = static_cast<char *>(d_geometricBlockList.allocate(
                                                               d_initialSize));
    d_buffer.replaceBuffer(buffer, d_initialSize);
}

SequentialPool::SequentialPool(int                          initialSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               bsls::Alignment::Strategy    alignmentStrategy,
                               bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_secondaryBuffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(0)
, d_initialSize(initialSize)
, d_constantBlockSize(growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
                      ? 0
                      : d_initialSize)
, d_constantBlockAllocationSize(alignedAllocationSize(d_constantBlockSize,
                                                      sizeof(Block)))
, d_maxGeometricBufferSize(INT_MAX)
, d_geometricBlockList(basicAllocator)
{
    BSLS_ASSERT(0 < initialSize);

    char *buffer;

    if (0 < d_constantBlockSize) {
        buffer = static_cast<char *>(
                    d_geometricBlockList.allocator()->allocate(d_initialSize));
    }
    else {
        buffer = static_cast<char *>(d_geometricBlockList.allocate(
                                                               d_initialSize));
    }

    d_buffer.replaceBuffer(buffer, d_initialSize);
}

SequentialPool::SequentialPool(int               initialSize,
                               int               maxBufferSize,
                               bslma::Allocator *basicAllocator)
: d_buffer()
, d_secondaryBuffer()
, d_head_p(0)
, d_reuseHead_p(0)
, d_initialSize(initialSize)
, d_constantBlockSize(0)
, d_constantBlockAllocationSize(0)
, d_maxGeometricBufferSize(maxBufferSize)
, d_geometricBlockList(basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer = static_cast<char *>(d_geometricBlockList.allocate(
                                                               d_initialSize));
    d_buffer.replaceBuffer(buffer, d_initialSize);
}

SequentialPool::SequentialPool(int                          initialSize,
                               int                          maxBufferSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               bslma::Allocator            *basicAllocator)
: d_buffer()
, d_secondaryBuffer()
, d_head_p(0)
, d_reuseHead_p(0)
, d_initialSize(initialSize)
, d_constantBlockSize(growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
                      ? 0
                      : d_initialSize)
, d_constantBlockAllocationSize(alignedAllocationSize(d_constantBlockSize,
                                                      sizeof(Block)))
, d_maxGeometricBufferSize(maxBufferSize)
, d_geometricBlockList(basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer;

    if (0 < d_constantBlockSize) {
        buffer = static_cast<char *>(
                    d_geometricBlockList.allocator()->allocate(d_initialSize));
    }
    else {
        buffer = static_cast<char *>(d_geometricBlockList.allocate(
                                                               d_initialSize));
    }

    d_buffer.replaceBuffer(buffer, d_initialSize);
}

SequentialPool::SequentialPool(int                        initialSize,
                               int                        maxBufferSize,
                               bsls::Alignment::Strategy  alignmentStrategy,
                               bslma::Allocator          *basicAllocator)
: d_buffer(alignmentStrategy)
, d_secondaryBuffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(0)
, d_initialSize(initialSize)
, d_constantBlockSize(0)
, d_constantBlockAllocationSize(0)
, d_maxGeometricBufferSize(maxBufferSize)
, d_geometricBlockList(basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer = static_cast<char *>(d_geometricBlockList.allocate(
                                                               d_initialSize));
    d_buffer.replaceBuffer(buffer, d_initialSize);
}

SequentialPool::SequentialPool(int                          initialSize,
                               int                          maxBufferSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               bsls::Alignment::Strategy    alignmentStrategy,
                               bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_secondaryBuffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(0)
, d_initialSize(initialSize)
, d_constantBlockSize(growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
                      ? 0
                      : d_initialSize)
, d_constantBlockAllocationSize(alignedAllocationSize(d_constantBlockSize,
                                                      sizeof(Block)))
, d_maxGeometricBufferSize(maxBufferSize)
, d_geometricBlockList(basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    char *buffer;

    if (0 < d_constantBlockSize) {
        buffer = static_cast<char *>(
                    d_geometricBlockList.allocator()->allocate(d_initialSize));
    }
    else {
        buffer = static_cast<char *>(d_geometricBlockList.allocate(
                                                               d_initialSize));
    }

    d_buffer.replaceBuffer(buffer, d_initialSize);
}

SequentialPool::SequentialPool(
                            int                          initialSize,
                            int                          maxBufferSize,
                            bsls::BlockGrowth::Strategy  growthStrategy,
                            bsls::Alignment::Strategy    alignmentStrategy,
                            bool                         allocateInitialBuffer,
                            bslma::Allocator            *basicAllocator)
: d_buffer(alignmentStrategy)
, d_secondaryBuffer(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(0)
, d_initialSize(initialSize)
, d_constantBlockSize(growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
                      ? 0
                      : d_initialSize)
, d_constantBlockAllocationSize(alignedAllocationSize(d_constantBlockSize,
                                                      sizeof(Block)))
, d_maxGeometricBufferSize(maxBufferSize)
, d_geometricBlockList(basicAllocator)
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    if (allocateInitialBuffer) {
        char *buffer;

        if (0 < d_constantBlockSize) {
            buffer = static_cast<char *>(
                    d_geometricBlockList.allocator()->allocate(d_initialSize));
        }
        else {
            buffer = static_cast<char *>(d_geometricBlockList.allocate(
                                                               d_initialSize));
        }

        d_buffer.replaceBuffer(buffer, d_initialSize);
    }
}

// MANIPULATORS
void SequentialPool::release()
{
    // 'BufferManager::release' keeps the buffer and just resets the internal
    // cursor, so 'reset' is used instead.

    d_buffer.reset();

    d_secondaryBuffer.reset();

    d_geometricBlockList.release();

    d_reuseHead_p = 0;

    while (d_head_p) {
        void *lastBlock = d_head_p;
        d_head_p        = d_head_p->d_next_p;
        d_geometricBlockList.allocator()->deallocate(lastBlock);
    }
}

void SequentialPool::reserveCapacity(int numBytes)
{
    // TBD
    BSLS_ASSERT(0 < numBytes);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(d_constantBlockSize >= numBytes)) {
        // If 'd_buffer.bufferSize()' is 0, 'd_buffer' is not currently
        // managing a buffer.

        if (0     == d_buffer.bufferSize()
         || false == d_buffer.hasSufficientCapacity(numBytes)) {
            // TBD allocateBlock();
        }
    }
    else {
        // TBD
        // d_geometricBlockList.reserveCapacity(numBytes);
    }
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
