// bdlma_concurrentpool.cpp                                           -*-C++-*-
#include <bdlma_concurrentpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_concurrentpool_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>

#include <bslmt_lockguard.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

///Implementation Note
///===================
// The implementation is composed of three parts: replenishment, deallocation,
// and reuse.  Deallocation is the simplest, returned memory is added to the
// one element reuse cache (`d_reuseCache`) or one of a few linked lists
// (`d_freeLists`).  Reuse occurs in two ways: through the one element reuse
// cache and as part of the allocation process when sufficient memory is
// available in the free lists (a chunk's worth of elements).  Allocation, for
// most threads, is simply taking the element in the one element reuse cache
// or, if none is available, an element from the allocation cache
// (`d_allocCache`).  The allocation cache is an array of addresses previously
// obtained from the underlying allocator; each non-null element points to one
// block. The allocation cache is divided into segments, which are contiguous
// sequences of `d_chunkSize` elements. Within each segment, the address stored
// in the left-most element is also designated as a token whose
// possession by a thread signifies a lock on the underlying allocator (reading
// a non-null value implies the reading thread has an exclusive lock on the
// allocator, a null value implies the thread must wait for the token). At any
// given time, there is either exactly one token in the allocation cache or
// none, meaning that the underlying allocator is unlocked or locked,
// respectively.  When the index of the cache element modulo the chunk size is
// zero, the thread is selected to perform a replenishment of the allocation
// cache.  First, the selected thread waits for the allocation token.  Then,
// the thread determines whether to use memory from the deallocation lists or
// to allocate additional memory.  If there is sufficient reuseable memory,
// memory is taken from the deallocations lists, the allocation cache is
// populated, and the token is provided for the next allocation to proceed.
// Otherwise, memory is allocated, the token is provided for the next
// allocation to proceed, and the allocation cache is populated.  As such, a
// lock on the allocator is obtained by holding the token.  Note that while the
// token is handed to the next location that indicates a replenishment, the
// indices serviced by the replenishment are located to minimize the
// contention.
//
// For example, if `d_chunkSize` is four, then there are four elements in each
// segment, and the below depicts the layout of `d_allocCache`.  A
// replenishment will place a token into the next token location when it
// completes population of chunk that is "far" from where the next allocation
// result is taken from (i.e., the first element of the next segment, and all
// elements except the first element of a different, further away segment).
// ```
//                                   +-- current index location with token
//       +-- new values loaded here  |               +-- next token location
//       v                           v               v
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+ ...
// | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | X | X | X | X | 0 | X | X | X |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+ ...
// ```

namespace BloombergLP {
namespace bdlma {

namespace {

// HELPER FUNCTIONS

/// Round up the specified `x` to the nearest whole integer multiple of the
/// specified `y`.  The behavior is undefined unless `1 <= y`.
static inline
bsls::Types::size_type roundUp(bsls::Types::size_type x,
                               bsls::Types::size_type y)
{
    BSLS_ASSERT(1 <= y);

    return (x + y - 1) / y * y;
}

}  // close unnamed namespace

                           // --------------------
                           // class ConcurrentPool
                           // --------------------

// PRIVATE MANIPULATORS
void *ConcurrentPool::allocateWithoutReuseCache()
{
    void *address;

    do {
        Uint64 index = (AtomicOp::addUint64NvAcqRel(
                         &d_nextAllocIndex.d_value, 1) - 1) & d_allocIndexMask;

        // obtain address to return
        address = AtomicOp::swapPtrAcqRel(&d_allocCache[index], 0);
        while (0 == address) {
            // wait for another thread to complete a replenish

            bslmt::ThreadUtil::yield();
            address = AtomicOp::swapPtrAcqRel(&d_allocCache[index], 0);
        }

        if (0 == (index & d_allocIndexInChunkMask)) {
            // Either perform the initial allocation or replenish the -2nd
            // (empty) chunk then advance the token to the next chunk.  Note
            // that only one thread can hold the token and the token acts as a
            // lock for using the allocator and the data members `d_reuseIndex`
            // and `d_reuseList` (though this entire block scope is a critical
            // section).

            if (d_chunkSize <=
                         AtomicOp::getUint64Acquire(&d_numAvailable.d_value)) {
                // Replenish from the `d_reuseList` and the free lists since
                // there are (or soon will be) enough blocks to fill a segment.
                // If `d_reuseList` does not have sufficient blocks, the free
                // lists are iterated over.  All of the blocks from a free list
                // are taken and used (to avoid ABA problem), and if there is a
                // need for more blocks the next free list is taken.  Any
                // excess blocks are stored in `d_reuseList` for the next
                // replenishment.  As an optimization, `d_reuseIndex` is used
                // to store the next free list to take from, since the free
                // lists should have - roughly - the same number of total
                // blocks inserted into each them.  Note that the free lists
                // could always be used in any order; `d_reuseIndex` is not
                // needed for correctness.

                AtomicOp::addUint64AcqRel(&d_numAvailable.d_value,
                                          -static_cast<Uint64>(d_chunkSize));

                Uint64 offset = (index + d_allocCacheSize - 2 * d_chunkSize)
                                                            & d_allocIndexMask;
                for (size_type i = 1; i < d_chunkSize; ++i) {
                    while (0 == d_reuseList) {
                        d_reuseList = AtomicOp::swapPtrAcqRel(
                                   &d_freeLists[  d_reuseIndex
                                                & k_FREE_INDEX_MASK].d_ptr, 0);

                        ++d_reuseIndex;

                        // Note that since the next attempt is on a different
                        // reuse list, do not `yield`.
                    }

                    void *next = static_cast<Link *>(d_reuseList)->d_next_p;

                    while (0 != AtomicOp::testAndSwapPtrAcqRel(
                                                     &d_allocCache[offset + i],
                                                     0,
                                                     d_reuseList)) {
                        bslmt::ThreadUtil::yield();
                    }

                    d_reuseList = next;
                }

                while (0 == d_reuseList) {
                    d_reuseList = AtomicOp::swapPtrAcqRel(
                                   &d_freeLists[  d_reuseIndex
                                                & k_FREE_INDEX_MASK].d_ptr, 0);

                    ++d_reuseIndex;

                    // Note that since the next attempt is on a different reuse
                    // list, do not `yield`.
                }

                Uint64 nextTokenIndex = (index + d_chunkSize)
                                                            & d_allocIndexMask;
                void *val = d_reuseList;
                d_reuseList = static_cast<Link *>(d_reuseList)->d_next_p;
                AtomicOp::swapPtrAcqRel(&d_allocCache[nextTokenIndex], val);
            }
            else if (address != this) {
                // insufficient free blocks; replenish using an allocation

                Uint64 offset = (index + d_allocCacheSize - 2 * d_chunkSize)
                                                            & d_allocIndexMask;
                Uint64 tokenIndex = (index + d_chunkSize) & d_allocIndexMask;

                AllocateProctor proctor(
                                this,
                                offset,
                                tokenIndex,
                                address);
                void *memory = d_blockList.allocate(d_chunkSize
                                                        * d_internalBlockSize);
                proctor.release();

                populateAllocCache(offset,
                                   tokenIndex,
                                   memory,
                                   memory,
                                   d_internalBlockSize);
            }
            else {
                // initial allocation

                AllocateProctor proctor(
                                      this,
                                      index,
                                      (index + d_chunkSize) & d_allocIndexMask,
                                      this);

                // allocate enough to pre-populate all but one of the chunks
                // (replenish populates the chunk two prior to the allocator
                // token and need to populate the chunk the allocation for
                // this token location would normally populate):
                //   * d_allocCacheSize is the full number of elements
                //   * - d_chunkSize removes the one chunk
                //   * - d_allocCacheSize / d_chunkSize + 1 token slots
                //   * + 2 for the result and the token
                address = d_blockList.allocate(
                                          (  d_allocCacheSize
                                           - d_chunkSize
                                           - d_allocCacheSize / d_chunkSize + 1
                                           + 2) * d_internalBlockSize);

                proctor.release();

                // reserve the first `address` location for the return value
                // and the second for the token
                size_type j = 2;
                for (size_type i = 0;
                     i < d_allocCacheSize - d_chunkSize;
                     ++i) {
                    if (0 == (i & d_allocIndexInChunkMask)) {
                        bsls::AtomicOperations::setPtrRelease(
                                 &d_allocCache[(index + i) & d_allocIndexMask],
                                 0);
                    }
                    else {
                        bsls::AtomicOperations::setPtrRelease(
                                 &d_allocCache[(index + i) & d_allocIndexMask],
                                 static_cast<char *>(address)
                                                    + j * d_internalBlockSize);
                        ++j;
                    }
                }

                // place the allocation token
                bsls::AtomicOperations::setPtrRelease(
                       &d_allocCache[(index + d_chunkSize) & d_allocIndexMask],
                       static_cast<char *>(address) + d_internalBlockSize);
            }
        }

        // `address == this` indicates an exception occurred (perhaps not
        // during the current invocation of `allocate`) and this `allocate`
        // failed
    } while (address == this);

    return address;
}

void ConcurrentPool::initialize()
{
    bsls::AtomicOperations::setPtrRelease(&d_reuseCache.d_ptr, 0);

    for (size_type i = 1; i < d_allocCacheSize; ++i) {
        bsls::AtomicOperations::setPtrRelease(&d_allocCache[i], 0);
    }

    for (int i = 0; i < k_NUM_FREE_LISTS; ++i) {
        bsls::AtomicOperations::setPtrRelease(&d_freeLists[i].d_ptr, 0);
    }

    bsls::AtomicOperations::initUint64(&d_nextAllocIndex.d_value, 0ull);
    bsls::AtomicOperations::initUint64(&d_numAvailable.d_value,   0ull);

    d_reuseIndex = 0;
    d_reuseList = 0;

    // place the allocation token indicating the initial allocation must occur
    bsls::AtomicOperations::setPtrRelease(&d_allocCache[0], this);
}

void ConcurrentPool::initializeParameters()
{
    BSLMF_ASSERT(0 == (k_NUM_FREE_LISTS & (k_NUM_FREE_LISTS - 1)));

    d_internalBlockSize = roundUp(d_blockSize,
                                  bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);

    // initialize chunk and cache size to efficient values
    d_chunkSize      = 32;
    d_allocCacheSize = 256;

    // reduce pre-allocated memory requirements for large block sizes,
    // while maintaining algorithm invariants
    size_type blockSize = 8192;
    while (d_internalBlockSize > blockSize && d_allocCacheSize > 8) {
        blockSize *= 2;
        if (d_allocCacheSize > 4 * d_chunkSize) {
            d_allocCacheSize /= 2;
        }
        else {
            d_chunkSize /= 2;
        }
    }

    // verify algorithm invariants
    BSLS_ASSERT(d_allocCacheSize >= 4 * d_chunkSize);
    BSLS_ASSERT(d_allocCacheSize <= k_MAX_ALLOC_CACHE_SIZE);

    d_allocIndexInChunkMask = d_chunkSize - 1;
    d_allocIndexMask        = d_allocCacheSize - 1;

    // verify mask validity
    BSLS_ASSERT(0 <  d_allocIndexInChunkMask);
    BSLS_ASSERT(0 <  d_allocIndexMask);
    BSLS_ASSERT(0 == (d_chunkSize & d_allocIndexInChunkMask));
    BSLS_ASSERT(0 == (d_allocCacheSize & d_allocIndexMask));
}

void ConcurrentPool::lockAllocator(Uint64 *index, void **address)
{
    *index = -static_cast<Uint64>(d_chunkSize);
    do {
        *index = (*index + d_chunkSize) & d_allocIndexMask;
        *address = AtomicOp::swapPtrAcqRel(&d_allocCache[*index], 0);
    } while (0 == *address);
}

void ConcurrentPool::populateAllocCache(Uint64                  index,
                                        Uint64                  tokenIndex,
                                        void                   *token,
                                        void                   *memory,
                                        bsls::Types::size_type  size)
{
    for (size_type i = 1; i < d_chunkSize; ++i) {
        while (0 != AtomicOp::testAndSwapPtrAcqRel(
                                     &d_allocCache[index + i],
                                     0,
                                     static_cast<char *>(memory) + i * size)) {
            bslmt::ThreadUtil::yield();
        }
    }

    AtomicOp::swapPtrAcqRel(&d_allocCache[tokenIndex], token);
}

// CREATORS
ConcurrentPool::ConcurrentPool(bsls::Types::size_type  blockSize,
                               bslma::Allocator       *basicAllocator)
: d_blockSize(blockSize)
, d_blockList(basicAllocator)
, d_mutex()
{
    BSLS_ASSERT(1 <= blockSize);

    initializeParameters();
    initialize();
}

ConcurrentPool::~ConcurrentPool()
{
}

// MANIPULATORS
void ConcurrentPool::reserveCapacity(int numBlocks)
{
    BSLS_ASSERT(0 <= numBlocks);

    // synchronize with `release` and other `reserveCapacity`
    bslmt::LockGuard<bslmt::Mutex> lockGuard(&d_mutex);

    // synchronize with `allocate`
    Uint64  index;
    void   *token;
    lockAllocator(&index, &token);

    numBlocks = static_cast<int>(roundUp(numBlocks, d_chunkSize));

    Uint64 avail = AtomicOp::getUint64Acquire(&d_numAvailable.d_value);
    if (avail < static_cast<Uint64>(numBlocks)) {
        numBlocks -= static_cast<int>(avail);

        size_type initialBlocks = 0;
        if (token == this) {
            // `reserveCapacity` was invoked before initial cache allocation;
            // need to allocate enough for the initial cache plus the reserve

            // allocate enough to pre-populate all but two of the chunks
            // (replenish populates the chunk two prior to the allocator
            // token):
            //   * d_allocCacheSize is the full number of elements
            //   * - 2 * d_chunkSize removes the two chunks
            //   * - d_allocCacheSize / d_chunkSize + 2 token slots
            //   * + 1 for the token
            initialBlocks = d_allocCacheSize
                           - 2 * d_chunkSize
                           - d_allocCacheSize / d_chunkSize + 2
                           + 1;
        }

        ReplaceValueProctor guard(this, index, token);
        char *memory = static_cast<char *>(d_blockList.allocate(
                              static_cast<size_type>(initialBlocks + numBlocks)
                                                       * d_internalBlockSize));
        guard.release();

        // place blocks for reserved capacity in the reuse list

        for (int i = 0; i < numBlocks - 1; ++i) {
            reinterpret_cast<Link *>(
                                  memory + i * d_internalBlockSize)->d_next_p =
                                          memory + (i+1) * d_internalBlockSize;
        }

        char *tail = memory + (numBlocks - 1) * d_internalBlockSize;
        reinterpret_cast<Link *>(tail)->d_next_p = d_reuseList;
        d_reuseList = memory;

        AtomicOp::addUint64AcqRel(&d_numAvailable.d_value, numBlocks);

        if (token == this) {
            // place blocks for initial allocation

            // reserve the first `address` location for the token
            token = static_cast<char *>(memory) +
                                               numBlocks * d_internalBlockSize;
            int j = numBlocks + 1;
            for (size_type i = 0;
                 i < d_allocCacheSize - 2 * d_chunkSize;
                 ++i) {
                if (0 == (i & d_allocIndexInChunkMask)) {
                    bsls::AtomicOperations::setPtrRelease(
                                 &d_allocCache[(index + i) & d_allocIndexMask],
                                 0);
                }
                else {
                    bsls::AtomicOperations::setPtrRelease(
                               &d_allocCache[(index + i) & d_allocIndexMask],
                               static_cast<char *>(memory)
                                                    + j * d_internalBlockSize);
                    ++j;
                }
            }
        }
    }

    unlockAllocator(index, token);
}

// DEPRECATED METHODS

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE4.40
ConcurrentPool::ConcurrentPool(
                             bsls::Types::size_type       blockSize,
                             bsls::BlockGrowth::Strategy  /* growthStrategy */,
                             bslma::Allocator            *basicAllocator)
: d_blockSize(blockSize)
, d_blockList(basicAllocator)
, d_mutex()
{
    BSLS_ASSERT(1 <= blockSize);

    initializeParameters();
    initialize();
}

ConcurrentPool::ConcurrentPool(
                          bsls::Types::size_type       blockSize,
                          bsls::BlockGrowth::Strategy  /* growthStrategy */,
                          int                          /* maxBlocksPerChunk */,
                          bslma::Allocator            *basicAllocator)
: d_blockSize(blockSize)
, d_blockList(basicAllocator)
, d_mutex()
{
    BSLS_ASSERT(1 <= blockSize);

    initializeParameters();
    initialize();
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE4.40

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
