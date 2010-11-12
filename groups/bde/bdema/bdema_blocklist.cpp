// bdema_blocklist.cpp                                                -*-C++-*-
#include <bdema_blocklist.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_blocklist_cpp,"$Id$ $CSID$")

#include <bslma_testallocator.h>   // for testing only

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

namespace BloombergLP {

// HELPER FUNCTIONS

static inline
int alignedAllocationSize(int numBytes, int sizeOfBlock)
    // Return the allocation size required to ensure proper alignment for a
    // 'bdema_BlockList::Block' containing a maximally aligned payload of the
    // specified 'numBytes', where the specified 'sizeOfBlock' is presumed to
    // be 'sizeof(bdema_BlockList::Block)'.  Note that, to ensure that both the
    // payload and header portions of the allocated memory are each separately
    // guaranteed to be maximally aligned in the presense of a supplied
    // allocator returning naturally-aligned memory, the size of the overall
    // allocation will be rounded up to an integral multiple of
    // 'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT'.
{
    ///IMPLEMENTATION NOTE
    ///-------------------
    //: 1 Append the size of the 'bdema_BlockList::Block' header:
    //:   'numBytes += sizeof(Block) - bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT'
    //:
    //: 2 Round to the nearest multiple of MAX_ALIGNMENT (a power of 2):
    //:   'numBytes = (numBytes + MAX_ALIGNMENT - 1) & ~(MAX_ALIGNMENT - 1)'

    return (numBytes + sizeOfBlock - 1)
           & ~(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);
}

                        // ---------------------
                        // class bdema_BlockList
                        // ---------------------

// CREATORS
bdema_BlockList::~bdema_BlockList()
{
    release();
}

// MANIPULATORS
void *bdema_BlockList::allocate(int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    if (0 == numBytes) {
        return 0;
    }

    numBytes = alignedAllocationSize(numBytes, sizeof(Block));

    Block *block = (Block *)d_allocator_p->allocate(numBytes);

    BSLS_ASSERT(0 == bsls_AlignmentUtil::calculateAlignmentOffset(
                                      (void *)block,
                                      bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT));

    block->d_next_p = (Block *)d_head_p;
    block->d_addrPrevNext = (Block **)&d_head_p;
    if (d_head_p) {
        d_head_p->d_addrPrevNext = &block->d_next_p;
    }
    d_head_p = block;

    BSLS_ASSERT(0 == bsls_AlignmentUtil::calculateAlignmentOffset(
                                      (void *)&block->d_memory,
                                      bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT));
    return (void *)&block->d_memory;
}

void bdema_BlockList::deallocate(void *address)
{
    if (address) {
        Block *block = (Block *)(void *)((char *)address - sizeof(Block) +
                                       bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);
        *block->d_addrPrevNext = block->d_next_p;
        if (block->d_next_p) {
            block->d_next_p->d_addrPrevNext = block->d_addrPrevNext;
        }
        d_allocator_p->deallocate(block);
    }
}

void bdema_BlockList::release()
{
    while (d_head_p) {
        void *lastBlock = d_head_p;
        d_head_p = d_head_p->d_next_p;
        d_allocator_p->deallocate(lastBlock);
    }
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
