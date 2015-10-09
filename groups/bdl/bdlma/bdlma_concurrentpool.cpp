// bdlma_concurrentpool.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_concurrentpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_concurrentpool_cpp,"$Id$ $CSID$")

#include <bslmt_lockguard.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>

#include <bslma_testallocator.h>  // for testing purpose only

#include <bsl_algorithm.h>  // for 'max()'
#include <bsl_cstddef.h>    // for 'offsetof()'
#include <bsl_cstdlib.h>

namespace BloombergLP {
namespace {

                                  // -----
                                  // TYPES
                                  // -----

struct LLink {
    // This 'struct' implements a link data structure that stores the address
    // of the next link, used to implement the internal linked list of free
    // memory blocks.  Note that this type is copied from
    // 'bdlma_concurrentpool.h' to provide access to this type from static
    // methods.

    union {
        bsls::AtomicOperations::AtomicTypes::Int d_refCount;
        bsls::AlignmentUtil::MaxAlignedType      d_dummy;
    };
    LLink *d_next_p;
};

                                // ---------
                                // CONSTANTS
                                // ---------

enum {
    k_INITIAL_CHUNK_SIZE =  1, // default 'numObjects' value

    k_MAX_CHUNK_SIZE     = 32  // minimum 'd_numObjects' value beyond which
                               // 'd_numObjects' becomes positive
};

}  // close unnamed namespace

// implementation details of private support functions

static inline
int roundUp(int x, int y)
    // Round up the specified 'x' to the nearest whole integer multiple of the
    // specified 'y'.  The behavior is undefined unless '0 <= x' and '0 < y'.
{
    BSLS_ASSERT(0 <= x);

    return (x + y - 1) / y * y;
}

static inline
LLink *toLink(char *address)
    // Return a linked-list link at the specified 'address'.
{
    // Note that a 'char *' cannot be converted directly to a 'LLink *'.

    return static_cast<LLink *>(static_cast<void *>(address));
}

// private support functions

static inline
int computeInternalBlockSize(int blockSize)
    // Return the number of bytes that must be allocated to provide an aligned
    // block of memory of the specified 'blockSize' that can also be used to
    // represent a 'object' 'LLink' (on the 'bdlma::ConcurrentPool' objects
    // free list).  Note that this value is the maximum of either the size of a
    // 'LLink' object or 'blockSize' rounded up to the alignment required for a
    // 'LLink' object (i.e., the maximum platform alignment).
{
    const int HEADER_LENGTH  = offsetof(LLink, d_next_p);
    const int MINIMUM_LENGTH = sizeof(LLink);

    return roundUp(bsl::max(blockSize + HEADER_LENGTH, MINIMUM_LENGTH),
                   bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
}

static
void replenishImp(bsls::AtomicPointer<LLink>       *nextList,
                  bdlma::InfrequentDeleteBlockList *blockList,
                  int                               blockSize,
                  int                               numBlocks)
    // Append to the specified 'nextList', 'numBlocks' free memory blocks each
    // having the specified 'blockSize', using memory provided by the specified
    // 'blockList'.  The behavior is undefined unless '1 <= blockSize' and
    // '1 <= numBlocks'.
{
    using namespace BloombergLP;

    BSLS_ASSERT(blockList);
    BSLS_ASSERT(1 <= blockSize);
    BSLS_ASSERT(1 <= numBlocks);

    char  *start = static_cast<char *>(
                                  blockList->allocate(numBlocks * blockSize));
    char  *end   = start + (numBlocks - 1) * blockSize;
    for (char *p = start; p < end; p += blockSize) {
        LLink *nextLink = toLink(p);
        bsls::AtomicOperations::initInt(&nextLink->d_refCount, 0 );
        nextLink->d_next_p = toLink(p + blockSize);
    }
    bsls::AtomicOperations::initInt(&toLink(end)->d_refCount, 0 );

    LLink *old;
    do {
        old                   = *nextList;
        toLink(end)->d_next_p = old;
    } while (old != nextList->testAndSwap(old, toLink(start)));
}

namespace bdlma {

                           // --------------------
                           // class ConcurrentPool
                           // --------------------

// PRIVATE MANIPULATORS
void ConcurrentPool::replenish()
{
    replenishImp(reinterpret_cast<bsls::AtomicPointer<LLink> *>(&d_freeList),
                 &d_blockList,
                 d_internalBlockSize,
                 d_chunkSize);

    if (bsls::BlockGrowth::BSLS_GEOMETRIC == d_growthStrategy
     && d_chunkSize < d_maxBlocksPerChunk) {

        if (d_chunkSize * 2 <= d_maxBlocksPerChunk) {
            d_chunkSize = d_chunkSize * 2;
        }
        else {
            d_chunkSize = d_maxBlocksPerChunk;
        }
    }
}

// CREATORS
ConcurrentPool::ConcurrentPool(int blockSize, bslma::Allocator *basicAllocator)
: d_blockSize(blockSize)
, d_chunkSize(k_INITIAL_CHUNK_SIZE)
, d_maxBlocksPerChunk(k_MAX_CHUNK_SIZE)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_freeList(0)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(1 <= blockSize);

    d_internalBlockSize = computeInternalBlockSize(blockSize);
}

ConcurrentPool::ConcurrentPool(int                          blockSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               bslma::Allocator            *basicAllocator)
: d_blockSize(blockSize)
, d_chunkSize(bsls::BlockGrowth::BSLS_CONSTANT == growthStrategy
              ? k_MAX_CHUNK_SIZE : k_INITIAL_CHUNK_SIZE)
, d_maxBlocksPerChunk(k_MAX_CHUNK_SIZE)
, d_growthStrategy(growthStrategy)
, d_freeList(0)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(1 <= blockSize);

    d_internalBlockSize = computeInternalBlockSize(blockSize);
}

ConcurrentPool::ConcurrentPool(int                          blockSize,
                               bsls::BlockGrowth::Strategy  growthStrategy,
                               int                          maxBlocksPerChunk,
                               bslma::Allocator            *basicAllocator)
: d_blockSize(blockSize)
, d_chunkSize(bsls::BlockGrowth::BSLS_CONSTANT == growthStrategy
              ? maxBlocksPerChunk : k_INITIAL_CHUNK_SIZE)
, d_maxBlocksPerChunk(maxBlocksPerChunk)
, d_growthStrategy(growthStrategy)
, d_freeList(0)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(1 <= blockSize);
    BSLS_ASSERT(1 <= maxBlocksPerChunk);

    d_internalBlockSize = computeInternalBlockSize(blockSize);
}

ConcurrentPool::~ConcurrentPool()
{
    BSLS_ASSERT(static_cast<int>(sizeof(LLink)) <= d_internalBlockSize);
    BSLS_ASSERT(0 != d_chunkSize);
}

// MANIPULATORS
void *ConcurrentPool::allocate()
{
    Link *p;
    for (;;) {
        p = d_freeList.loadRelaxed();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!p)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
            p = d_freeList;
            if (!p) {
                replenish();
                continue;
            }
        }

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY
                  (2 != bsls::AtomicOperations::addIntNv(&p->d_refCount, 2))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            for (int i = 0; i < 3; ++i) {
                // To avoid unnecessary contention, assume that if we did not
                // get the first reference, then the other thread is about to
                // complete the pop.  Wait for a few cycles until he does.  If
                // he does not complete then go on and try to acquire it
                // ourselves.

                if (d_freeList.loadRelaxed() != p) {
                    break;
                }
            }
        }

        // Force a dependent read of 'd_next_p' to make sure that we're not
        // racing against another thread calling 'deallocate' for 'p' and that
        // checked the refcount *before* we incremented it, put back 'p' in the
        // freelist with a potentially different 'd_next_p'.
        //
        // There are two possibilities in this particular case:
        //   - The following 'loadRelaxed()' will return the new 'freelist'
        //     value (== p) and because of the release barrier before the last
        //     CAS in deallocate, we can observe the new 'd_next_p' value (this
        //     relies on dependent load ordering)
        //   - loadRelaxed() will return the "old" (!= p) and the CAS and thus
        //     the condition will be false.
        //
        // Note that 'h' is made volatile so that the compiler does not replace
        // the 'h->d_inUse' load with 'p->d_inUse' (and thus removing the data
        // dependency).  TBD to be completely thorough 'h->d_next_p' needs a
        // load dependent barrier (no-op on all current architectures though).

        const Link * volatile h = d_freeList.loadRelaxed();

        // gcc 4.3, 4.4 seems to have trouble processing likely(a && b), using
        // likely(a) && likely(b) fixes the problem.  3.4.6 seems to generate
        // the proper code though.

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(h == p)
         && BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                d_freeList.testAndSwap(p, h->d_next_p) == p)) {
            break;
        }

        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        for (;;) {
            int refCount = bsls::AtomicOperations::getInt(&p->d_refCount);

            if (refCount & 1) {
                if (refCount ==
                        bsls::AtomicOperations::testAndSwapInt(
                            &p->d_refCount,
                            refCount,
                            refCount^1)) {
                    // The node is now free but not on the free list.  Try to
                    // take it.

                    return static_cast<void *>(const_cast<Link **>(
                                                      &p->d_next_p)); // RETURN
                }
            }
            else if (refCount ==
                        bsls::AtomicOperations::testAndSwapInt(
                            &p->d_refCount,
                            refCount,
                            refCount - 2)) {
                break;
            }
        }
    }

    return static_cast<void *>(const_cast<Link **>(&p->d_next_p));
}

void ConcurrentPool::deallocate(void *address)
{
    Link *p = static_cast<Link *>(static_cast<void *>(
                     static_cast<char *>(address) - offsetof(Link, d_next_p)));
    int refCount = bsls::AtomicOperations::getIntRelaxed(&p->d_refCount);
    for (;;) {
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(2 == refCount)) {
            refCount = bsls::AtomicOperations::testAndSwapInt(&p->d_refCount,
                                                              2,
                                                              0);
            if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(2 == refCount)) {
                break;
            }
        }
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        const int oldRefCount = refCount;
        refCount = bsls::AtomicOperations::testAndSwapInt(&p->d_refCount,
                                                          refCount,
                                                          refCount - 1);
        if (oldRefCount == refCount) {
            // Someone else is still trying to pop this item.  Just let them
            // have it.

            return;                                                   // RETURN
        }
    }

    Link *old = d_freeList.loadRelaxed();
    for (;;) {
        p->d_next_p = old;
        const Link * const swap = old;
        old = d_freeList.testAndSwap(old, p);  // release
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(swap == old)) {
            break;
        }
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
    }
}

void ConcurrentPool::reserveCapacity(int numBlocks)
{
    BSLS_ASSERT(0 <= numBlocks);

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    Link *list = d_freeList.swap(0);
    Link *last = list;

    while (last) {
        --numBlocks;
        if (!last->d_next_p) break;
        last = last->d_next_p;
    }

    if (last) {
        Link *old;

        do {
            old = d_freeList;
            last->d_next_p = old;
        } while (old != d_freeList.testAndSwap(old, list));
    }

    if (numBlocks > 0) {
        replenishImp(
                   reinterpret_cast<bsls::AtomicPointer<LLink> *>(&d_freeList),
                   &d_blockList,
                   d_internalBlockSize,
                   numBlocks);
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
