// bdema_infrequentdeleteblocklist.cpp                                -*-C++-*-
#include <bdema_infrequentdeleteblocklist.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_infrequentdeleteblocklist_cpp,"$Id$ $CSID$")

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

namespace BloombergLP {

// HELPER FUNCTIONS

static inline
int alignedAllocationSize(int numBytes, int sizeOfBlock)
    // Return the allocation size required to ensure proper alignment for a
    // 'bdema_InfrequentDeleteBlockList::Block' containing a maximally aligned
    // payload of the specified 'numBytes', where the specified 'sizeOfBlock'
    // is presumed to be 'sizeof(bdema_InfrequentDeleteBlockList::Block)'.
    // Note that, to ensure that both the payload and header portions of the
    // allocated memory are each separately guaranteed to be maximally aligned
    // in the presense of a supplied allocator returning naturally-aligned
    // memory, the size of the overall allocation will be rounded up to an
    // integral multiple of 'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT'.
{
    ///IMPLEMENTATION NOTE
    ///-------------------
    //: 1 Append the size of the 'bdema_InfrequentDeleteBlockList::Block'
    //:   header:
    //:   'numBytes += sizeof(Block) - bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT'
    //:
    //: 2 Round to the nearest multiple of MAX_ALIGNMENT (a power of 2):
    //:   'numBytes = (numBytes + MAX_ALIGNMENT - 1) & ~(MAX_ALIGNMENT - 1)'

    return (numBytes + sizeOfBlock - 1)
           & ~(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);
}

                  // -------------------------------------
                  // class bdema_InfrequentDeleteBlockList
                  // -------------------------------------

// CREATORS
bdema_InfrequentDeleteBlockList::~bdema_InfrequentDeleteBlockList()
{
    release();
}

// MANIPULATORS
void *bdema_InfrequentDeleteBlockList::allocate(int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    if (0 == numBytes) {
        return 0;
    }

    numBytes = alignedAllocationSize(numBytes, sizeof(Block));

    Block *block =
                  reinterpret_cast<Block *>(d_allocator_p->allocate(numBytes));

    BSLS_ASSERT(0 == bsls_AlignmentUtil::calculateAlignmentOffset(
                                      reinterpret_cast<void *>(block),
                                      bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT));

    block->d_next_p = d_head_p;
    d_head_p        = block;

    BSLS_ASSERT(0 == bsls_AlignmentUtil::calculateAlignmentOffset(
                                    reinterpret_cast<void *>(&block->d_memory),
                                    bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT));

    return reinterpret_cast<void *>(&block->d_memory);
}

void bdema_InfrequentDeleteBlockList::release()
{
    while (d_head_p) {
        void *lastBlock = d_head_p;
        d_head_p        = d_head_p->d_next_p;
        d_allocator_p->deallocate(lastBlock);
    }
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
