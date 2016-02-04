// bdlma_sequentialpool.cpp                                           -*-C++-*-
#include <bdlma_sequentialpool.h>

#include <bdlb_bitutil.h>

#include <bslma_default.h>

#include <bsl_cstring.h>

enum {
    k_INITIAL_SIZE      =  256,  // default constant growth strategy allocation size
                                 // (in bytes)
};

namespace BloombergLP {

// HELPER FUNCTIONS
inline
static bsls::Types::size_type alignedAllocationSize(
                                            bsls::Types::size_type size,
                                            bsls::Types::size_type sizeOfBlock)
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

namespace bdlma {

                           // --------------------
                           // class SequentialPool
                           // --------------------

// PRIVATE MANIPULATORS
void SequentialPool::replaceBufferConstantGrowth()
{
    Block *block = reinterpret_cast<Block *>(d_allocator_p->allocate(
                             alignedAllocationSize(d_minSize, sizeof(Block))));

    block->d_next_p = *d_reuseHead_p;
    *d_reuseHead_p  =  block;
    d_reuseHead_p   = &block->d_next_p;

    d_bufferManager.replaceBuffer(reinterpret_cast<char *>(&block->d_memory),
                                  d_minSize);
}

void SequentialPool::replaceBufferNonConstantGrowth(
                                                   bsls::Types::size_type size)
{
    // Find bin to use.

    bsls::Types::size_type unavailable = d_unavailable |
                               (bdlb::BitUtil::roundUpToBinaryPower(size) - 1);

    int index = bdlb::BitUtil::numTrailingUnsetBits(~unavailable);
    
    // Update 'd_bufferManager'.

    if (index < k_NUM_GEOMETRIC_BIN) {
        // Use memory from the geometric strategy.  If needed, allocate a block
        // of memory.

        char *& bin = d_geometricBin[index];

        bsls::Types::size_type allocatedSize =
                               static_cast<bsls::Types::size_type>(1) << index;

        if (0 == bin) {
            bin = reinterpret_cast<char *>(
                                       d_allocator_p->allocate(allocatedSize));
        }
        d_bufferManager.replaceBuffer(bin, allocatedSize);

        d_unavailable |= (static_cast<bsls::Types::size_type>(1) << index);
    }
    else {
        // Forward to underlying allocator.

        Block *block = reinterpret_cast<Block *>(d_allocator_p->allocate(
                                  alignedAllocationSize(size, sizeof(Block))));

        block->d_next_p    = d_largeBlockHead_p;
        d_largeBlockHead_p = block;

        d_bufferManager.replaceBuffer(reinterpret_cast<char *>(
                                                             &block->d_memory),
                                      size);
    }
}

void *SequentialPool::allocateNonFastPath(bsls::Types::size_type size)
{
    BSLS_ASSERT(0 < size);

    // Note that empty blocks are maximally aligned which allows simple size
    // comparisons.

    if (size <= d_minSize) {
        // Constant growth strategy.

        if (*d_reuseHead_p) {
            // Reuse an existing constant growth block.

            d_bufferManager.replaceBuffer(reinterpret_cast<char *>(
                                                  &(*d_reuseHead_p)->d_memory),
                                          d_minSize);

            d_reuseHead_p = &(*d_reuseHead_p)->d_next_p;
        }
        else {
            // Use a newly allocated block.

            replaceBufferConstantGrowth();
        }
    }
    else {
        // Geometric growth strategy.

        replaceBufferNonConstantGrowth(size);
    }

    return d_bufferManager.allocateRaw(size);
}

// CREATORS
SequentialPool::SequentialPool(bslma::Allocator *basicAllocator)
: d_bufferManager()
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(
                     static_cast<bsls::Types::size_type>(k_INITIAL_SIZE)) - 1))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);
}

SequentialPool::SequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
                               bslma::Allocator            *basicAllocator)
: d_bufferManager()
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(
                     static_cast<bsls::Types::size_type>(k_INITIAL_SIZE)) - 1))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : k_INITIAL_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);
}

SequentialPool::SequentialPool(bsls::Alignment::Strategy  alignmentStrategy,
                               bslma::Allocator          *basicAllocator)
: d_bufferManager(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(
                     static_cast<bsls::Types::size_type>(k_INITIAL_SIZE)) - 1))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);
}

SequentialPool::SequentialPool(bsls::BlockGrowth::Strategy  growthStrategy,
                               bsls::Alignment::Strategy    alignmentStrategy,
                               bslma::Allocator            *basicAllocator)
: d_bufferManager(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(
                     static_cast<bsls::Types::size_type>(k_INITIAL_SIZE)) - 1))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : k_INITIAL_SIZE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);
}

SequentialPool::SequentialPool(int initialSize)
: d_bufferManager()
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(
                        static_cast<bsls::Types::size_type>(initialSize)) - 1))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(0)
, d_allocator_p(bslma::Default::allocator(0))
{
    BSLS_ASSERT(0 < initialSize);

    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);

    reserveCapacity(initialSize);
}

SequentialPool::SequentialPool(bsls::Types::size_type  initialSize,
                               bslma::Allocator       *basicAllocator)
: d_bufferManager()
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(initialSize) - 1))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 < initialSize);

    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);

    reserveCapacity(initialSize);
}

SequentialPool::SequentialPool(bsls::Types::size_type       initialSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               bslma::Allocator            *basicAllocator)
: d_bufferManager()
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(initialSize) - 1))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 < initialSize);

    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);

    reserveCapacity(initialSize);
}

SequentialPool::SequentialPool(bsls::Types::size_type     initialSize,
                               bsls::Alignment::Strategy  alignmentStrategy,
                               bslma::Allocator          *basicAllocator)
: d_bufferManager(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(initialSize) - 1))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 < initialSize);

    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);

    reserveCapacity(initialSize);
}

SequentialPool::SequentialPool(bsls::Types::size_type       initialSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               bsls::Alignment::Strategy    alignmentStrategy,
                               bslma::Allocator            *basicAllocator)
: d_bufferManager(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(initialSize) - 1))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 < initialSize);

    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);

    reserveCapacity(initialSize);
}

SequentialPool::SequentialPool(bsls::Types::size_type  initialSize,
                               bsls::Types::size_type  maxBufferSize,
                               bslma::Allocator       *basicAllocator)
: d_bufferManager()
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(initialSize) - 1)
             | (static_cast<bsls::Types::size_type>(-1)
                                    << bdlb::BitUtil::log2(maxBufferSize + 1)))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);

    reserveCapacity(initialSize);
}

SequentialPool::SequentialPool(bsls::Types::size_type       initialSize,
                               bsls::Types::size_type       maxBufferSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               bslma::Allocator            *basicAllocator)
: d_bufferManager()
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(initialSize) - 1)
             | (static_cast<bsls::Types::size_type>(-1)
                                    << bdlb::BitUtil::log2(maxBufferSize + 1)))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);

    reserveCapacity(initialSize);
}

SequentialPool::SequentialPool(bsls::Types::size_type     initialSize,
                               bsls::Types::size_type     maxBufferSize,
                               bsls::Alignment::Strategy  alignmentStrategy,
                               bslma::Allocator          *basicAllocator)
: d_bufferManager(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(initialSize) - 1)
             | (static_cast<bsls::Types::size_type>(-1)
                                    << bdlb::BitUtil::log2(maxBufferSize + 1)))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);

    reserveCapacity(initialSize);
}

SequentialPool::SequentialPool(bsls::Types::size_type       initialSize,
                               bsls::Types::size_type       maxBufferSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               bsls::Alignment::Strategy    alignmentStrategy,
                               bslma::Allocator            *basicAllocator)
: d_bufferManager(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(initialSize) - 1)
             | (static_cast<bsls::Types::size_type>(-1)
                                    << bdlb::BitUtil::log2(maxBufferSize + 1)))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);

    reserveCapacity(initialSize);
}

SequentialPool::SequentialPool(
                            bsls::Types::size_type       initialSize,
                            bsls::Types::size_type       maxBufferSize,
                            bsls::BlockGrowth::Strategy  growthStrategy,
                            bsls::Alignment::Strategy    alignmentStrategy,
                            bool                         allocateInitialBuffer,
                            bslma::Allocator            *basicAllocator)
: d_bufferManager(alignmentStrategy)
, d_head_p(0)
, d_reuseHead_p(&d_head_p)
, d_alwaysUnavailable(
               (static_cast<bsls::Types::size_type>(-1) << k_NUM_GEOMETRIC_BIN)
             | (bdlb::BitUtil::roundUpToBinaryPower(initialSize) - 1)
             | (static_cast<bsls::Types::size_type>(-1)
                                    << bdlb::BitUtil::log2(maxBufferSize + 1)))
, d_unavailable(d_alwaysUnavailable)
, d_largeBlockHead_p(0)
, d_minSize(  growthStrategy == bsls::BlockGrowth::BSLS_GEOMETRIC
            ? 0
            : initialSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0           <  initialSize);
    BSLS_ASSERT(initialSize <= maxBufferSize);

    bsl::memset(d_geometricBin,
                0,
                sizeof *d_geometricBin * k_NUM_GEOMETRIC_BIN);

    if (allocateInitialBuffer) {
        reserveCapacity(initialSize);
    }
}

// MANIPULATORS
void SequentialPool::release()
{
    // Set 'd_bufferManager' to not manage any memory.

    d_bufferManager.reset();

    // Mark all constant growth blocks as reusable (the set of reusable blocks
    // is about to be emptied).

    d_reuseHead_p = &d_head_p;

    // Return all constant growth blocks to the underlying allocator.

    while (d_head_p) {
        void *lastBlock = d_head_p;
        d_head_p        = d_head_p->d_next_p;
        d_allocator_p->deallocate(lastBlock);
    }

    // Return all geometric growth blocks to the underlying allocator.

    d_unavailable = d_alwaysUnavailable;

    for (int i = 0; i < k_NUM_GEOMETRIC_BIN; ++i) {
        char *& bin = d_geometricBin[i];

        d_allocator_p->deallocate(bin);
        bin = 0;
    }

    // Return all blocks allocated outside the constant and growth strategies
    // to the underlying allocator.

    while (d_largeBlockHead_p) {
        void *lastBlock    = d_largeBlockHead_p;
        d_largeBlockHead_p = d_largeBlockHead_p->d_next_p;
        d_allocator_p->deallocate(lastBlock);
    }
}

void SequentialPool::reserveCapacity(bsls::Types::size_type numBytes)
{
    BSLS_ASSERT(0 < numBytes);

    // If 'd_bufferManager.bufferSize()' is 0, 'd_bufferManager' is not
    // currently managing a buffer.

    if (   d_bufferManager.bufferSize()
        && d_bufferManager.hasSufficientCapacity(numBytes)) {
        return;                                                       // RETURN
    }

    if (numBytes <= d_minSize) {
        // Constant growth strategy.

        if (0 == *d_reuseHead_p) {
            // No block available for reuse; allocate a block for future use.

            Block *block = reinterpret_cast<Block *>(d_allocator_p->allocate(
                             alignedAllocationSize(d_minSize, sizeof(Block))));

            block->d_next_p = *d_reuseHead_p;
            *d_reuseHead_p  =  block;
        }
    }
    else {
        // Geometric growth strategy.

        bsls::Types::size_type unavailable = d_unavailable |
                           (bdlb::BitUtil::roundUpToBinaryPower(numBytes) - 1);

        int index = bdlb::BitUtil::numTrailingUnsetBits(~unavailable);

        if (index < k_NUM_GEOMETRIC_BIN) {
            // Use memory from the geometric strategy.  If needed, allocate a
            // block of memory.

            char *& bin = d_geometricBin[index];

            if (0 == bin) {
                bsls::Types::size_type allocatedSize =
                               static_cast<bsls::Types::size_type>(1) << index;
                bin = reinterpret_cast<char *>(
                                       d_allocator_p->allocate(allocatedSize));
            }
        }
        else {
            // Forward to underlying allocator and update 'd_bufferManager'.

            Block *block = reinterpret_cast<Block *>(d_allocator_p->allocate(
                              alignedAllocationSize(numBytes, sizeof(Block))));

            block->d_next_p    = d_largeBlockHead_p;
            d_largeBlockHead_p = block;

            d_bufferManager.replaceBuffer(reinterpret_cast<char *>(
                                                             &block->d_memory),
                                          numBytes);
        }
    }
}

void SequentialPool::rewind()
{
    // Set 'd_bufferManager' to not manage any memory.

    d_bufferManager.reset();

    // Mark all constant growth blocks as reusable.

    d_reuseHead_p = &d_head_p;

    // Mark all geometric growth blocks as reusable.

    d_unavailable = d_alwaysUnavailable;

    // Return all blocks allocated outside the constant and growth strategies
    // to the underlying allocator.

    while (d_largeBlockHead_p) {
        void *lastBlock    = d_largeBlockHead_p;
        d_largeBlockHead_p = d_largeBlockHead_p->d_next_p;
        d_allocator_p->deallocate(lastBlock);
    }
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
