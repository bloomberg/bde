// bslma_infrequentdeleteblocklist.cpp                                -*-C++-*-
#include <bslma_infrequentdeleteblocklist.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_testallocator.h>         // for testing only

#include <bsls_alignmentutil.h>

namespace BloombergLP {

namespace bslma {

                  // -------------------------------
                  // class InfrequentDeleteBlockList
                  // -------------------------------

// CREATORS
InfrequentDeleteBlockList::~InfrequentDeleteBlockList()
{
    release();
}

// MANIPULATORS
void *InfrequentDeleteBlockList::allocate(int numBytes)
{
    if (0 == numBytes) {
        return 0;
    }

    // Add size of block header to 'numBytes', then round up to
    // the nearest multiple of 'MAX_ALIGNMENT'.

    numBytes += sizeof(Block) - 1;
    numBytes &= ~(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);

    Block *block = (Block *)d_allocator_p->allocate(numBytes);
    block->d_next_p = d_head_p;
    d_head_p = block;
    return (void *)&block->d_memory;
}

void InfrequentDeleteBlockList::release()
{
    while (d_head_p) {
        void *lastBlock = d_head_p;
        d_head_p = d_head_p->d_next_p;
        d_allocator_p->deallocate(lastBlock);
    }
}

}  // close package namespace

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
