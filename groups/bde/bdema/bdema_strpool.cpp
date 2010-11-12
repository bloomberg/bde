// bdema_strpool.cpp    -*-C++-*-
#include <bdema_strpool.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_strpool_cpp,"$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
// To provide efficient and consistent block growth behavior, the constructor
// of 'bdema_StrPool' initializes 'd_blockSize' to
// 'INITIAL_SIZE / GROW_FACTOR' and 'd_cursor' to 'd_blockSize'.  The very
// first memory request from the pool causes 'allocateImp' to be invoked
// since 'd_cursor' is equal to 'd_blockSize', and 'd_blockSize' is correctly
// increased to 'INITIAL_SIZE' as the pool's first memory block size.

#include <bdema_sequentialallocator.h>     // for testing only

#include <bsls_assert.h>

// CONSTANTS
enum {
    INITIAL_SIZE = 64,       // initial block size (divisible by 'GROW_FACTOR')
    GROW_FACTOR  = 2,        // multiplicative factor to grow block size
    THRESHOLD    = 128,      // size beyond which a separate block is allocated
                             // if the current block cannot satisfy the request
    MAX_SIZE     = 8 * 1024, // maximum block size

    INITIAL_BLOCKSIZE = INITIAL_SIZE / GROW_FACTOR
};

namespace BloombergLP {

                        // -------------------
                        // class bdema_StrPool
                        // -------------------

// PRIVATE MANIPULATORS
void *bdema_StrPool::allocateImp(int numBytes)
{
    BSLS_ASSERT(0 < numBytes);

    // Restore to value prior to optimistic commit.  This is also needed below
    // when 'THRESHOLD >= numBytes' to preserve exception neutrality.

    d_cursor -= numBytes;

    if (THRESHOLD < numBytes) {  // Allocate separate block if above threshold.
        return d_blockList.allocate(numBytes);
    }

    reserveCapacity(numBytes);

    char *ret = d_block_p + d_cursor;
    d_cursor += numBytes;

    BSLS_ASSERT(d_cursor <= d_blockSize);

    return ret;
}

// CREATORS
bdema_StrPool::bdema_StrPool(bslma_Allocator *basicAllocator)
: d_blockSize(INITIAL_BLOCKSIZE)
, d_block_p(0)
, d_cursor(INITIAL_BLOCKSIZE)
, d_blockList(basicAllocator)
{
}

bdema_StrPool::~bdema_StrPool()
{
    BSLS_ASSERT(d_blockSize >= INITIAL_BLOCKSIZE);
    BSLS_ASSERT(d_blockSize <  MAX_SIZE * GROW_FACTOR);

    BSLS_ASSERT(       0 <= d_cursor);
    BSLS_ASSERT(d_cursor <= d_blockSize);
}

// MANIPULATORS
void bdema_StrPool::reserveCapacity(int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    // Determine if block needs to be allocated.

    if (d_cursor + numBytes <= d_blockSize) {
        return;
    }

    // Determine size of block to allocate; must do this in order to match
    // other assumptions.

    int size;
    if (numBytes > MAX_SIZE) {
        size = numBytes;
    }
    else if (d_blockSize >= MAX_SIZE) {
        size = MAX_SIZE;
    }
    else {
        size = d_blockSize;
        do {
            size *= GROW_FACTOR;

        } while (size < numBytes);
    }

    // Allocate memory and set the cursor.

    d_block_p = (char *)d_blockList.allocate(size);
    d_blockSize = size;
    d_cursor = 0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
