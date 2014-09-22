// bdlma_pool.cpp                                                     -*-C++-*-
#include <bdlma_pool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_pool_cpp,"$Id$ $CSID$")

#include <bsls_alignmentfromtype.h>
#include <bsls_performancehint.h>

#include <bsl_algorithm.h>

namespace BloombergLP {
namespace bdlma {

namespace {

// TYPES
struct Link {
    // This 'struct' implements a link data structure that stores the address
    // of the next link, used to implement the internal linked list of free
    // memory blocks.  Note that this type was copied from 'bdlma_pool.h' to
    // provide access to this type from static methods.

    Link *d_next_p;
};

// CONSTANTS
enum {
    INITIAL_CHUNK_SIZE =  1,  // default number of blocks per chunk

    GROWTH_FACTOR      =  2,  // multiplicative factor by which to grow pool
                              // capacity

    MAX_CHUNK_SIZE     = 32   // maximum number of blocks per chunk
};

// LOCAL FUNCTIONS
static
void *replenishImp(BloombergLP::bdlma::InfrequentDeleteBlockList *blockList,
                   int                                            blockSize,
                   int                                            numBlocks,
                   void                                          *nextList)
    // Return the address of a linked list of modifiable free memory blocks
    // having the specified 'numBlocks', with each memory block having the
    // specified 'blockSize' (in bytes).  Append the specified 'nextList' to
    // the newly-created linked list.  Allocate memory using the specified
    // 'blockList'.  The behavior is undefined unless '1 <= blockSize' and
    // '1 <= numBlocks'.
{
    BSLS_ASSERT(blockList);
    BSLS_ASSERT(1 <= blockSize);
    BSLS_ASSERT(1 <= numBlocks);

    char *begin = static_cast<char *>(
                                   blockList->allocate(numBlocks * blockSize));
    char *end   = begin + (numBlocks - 1) * blockSize;

    for (char *p = begin; p < end; p += blockSize) {
        reinterpret_cast<Link *>(p)->d_next_p =
                                       reinterpret_cast<Link *>(p + blockSize);
    }
    reinterpret_cast<Link *>(end)->d_next_p = static_cast<Link *>(nextList);

    return begin;
}

static inline
int roundUp(int x, int y)
    // Round up the specified 'x' to the nearest whole integer multiple of the
    // specified 'y'.  The behavior is undefined unless '0 <= x' and '1 <= y'.
{
    BSLS_ASSERT(0 <= x);
    BSLS_ASSERT(1 <= y);

    return (x + y - 1) / y * y;
}

}  // close unnamed namepace

                        // ----------
                        // class Pool
                        // ----------

// PRIVATE MANIPULATORS
void Pool::replenish()
{
    d_freeList_p = static_cast<Link *>(replenishImp(&d_blockList,
                                                    d_internalBlockSize,
                                                    d_chunkSize,
                                                    0));

    if (bsls::BlockGrowth::BSLS_GEOMETRIC == d_growthStrategy
     && d_chunkSize < d_maxBlocksPerChunk) {

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                     d_chunkSize * 2 <= d_maxBlocksPerChunk)) {
            d_chunkSize = d_chunkSize * 2;
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            d_chunkSize = d_maxBlocksPerChunk;
        }
    }
}

// CREATORS
Pool::Pool(int blockSize, bslma::Allocator *basicAllocator)
: d_blockSize(blockSize)
, d_chunkSize(INITIAL_CHUNK_SIZE)
, d_maxBlocksPerChunk(MAX_CHUNK_SIZE)
, d_growthStrategy(bsls::BlockGrowth::BSLS_GEOMETRIC)
, d_freeList_p(0)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(1 <= blockSize);

    d_internalBlockSize = bsl::max(
                     static_cast<int>(sizeof(Link)),
                     roundUp(blockSize, bsls::AlignmentFromType<Link>::VALUE));
}

Pool::Pool(int                          blockSize,
           bsls::BlockGrowth::Strategy  growthStrategy,
           bslma::Allocator            *basicAllocator)
: d_blockSize(blockSize)
, d_chunkSize(bsls::BlockGrowth::BSLS_CONSTANT == growthStrategy
              ? MAX_CHUNK_SIZE
              : INITIAL_CHUNK_SIZE)
, d_maxBlocksPerChunk(MAX_CHUNK_SIZE)
, d_growthStrategy(growthStrategy)
, d_freeList_p(0)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(1 <= blockSize);

    d_internalBlockSize = bsl::max(
                     static_cast<int>(sizeof(Link)),
                     roundUp(blockSize, bsls::AlignmentFromType<Link>::VALUE));
}

Pool::Pool(int                          blockSize,
           bsls::BlockGrowth::Strategy  growthStrategy,
           int                          maxBlocksPerChunk,
           bslma::Allocator            *basicAllocator)
: d_blockSize(blockSize)
, d_chunkSize(bsls::BlockGrowth::BSLS_CONSTANT == growthStrategy
              ? maxBlocksPerChunk
              : INITIAL_CHUNK_SIZE)
, d_maxBlocksPerChunk(maxBlocksPerChunk)
, d_growthStrategy(growthStrategy)
, d_freeList_p(0)
, d_blockList(basicAllocator)
{
    BSLS_ASSERT(1 <= blockSize);
    BSLS_ASSERT(1 <= maxBlocksPerChunk);

    d_internalBlockSize = bsl::max(
                     static_cast<int>(sizeof(Link)),
                     roundUp(blockSize, bsls::AlignmentFromType<Link>::VALUE));
}

Pool::~Pool()
{
    BSLS_ASSERT(static_cast<int>(sizeof(Link)) <= d_internalBlockSize);
    BSLS_ASSERT(0 < d_chunkSize);
}

// MANIPULATORS
void Pool::reserveCapacity(int numBlocks)
{
    BSLS_ASSERT(0 <= numBlocks);

    Link *p = d_freeList_p;
    while (p && numBlocks) {
        p = p->d_next_p;
        --numBlocks;
    }

    if (numBlocks) {
        d_freeList_p = static_cast<Link *>(replenishImp(&d_blockList,
                                                        d_internalBlockSize,
                                                        numBlocks,
                                                        d_freeList_p));
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2012 Bloomberg Finance L.P.
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
