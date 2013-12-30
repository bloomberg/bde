// bdlma_blocklist.cpp                                                -*-C++-*-
#include <bdlma_blocklist.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_blocklist_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {
namespace bdlma {

// HELPER FUNCTIONS

static inline
int alignedAllocationSize(int size, int sizeOfBlock)
    // Return the allocation size (in bytes) required to ensure proper
    // alignment for a 'bdlma::BlockList::Block' containing a maximally-aligned
    // payload of the specified 'size', where the specified 'sizeOfBlock' is
    // presumed to be 'sizeof(bdlma::BlockList::Block)'.  Note that, to ensure
    // that both the payload and header portions of the allocated memory are
    // each separately guaranteed to be maximally aligned in the presence of a
    // supplied allocator returning naturally-aligned memory, the size of the
    // overall allocation will be rounded up to an integral multiple of
    // 'bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT'.
{
    ///IMPLEMENTATION NOTE
    ///-------------------
    //: 1 Append the size of the 'bdlma::BlockList::Block' header:
    //:   'size += sizeof(Block) - bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT'
    //:
    //: 2 Round to the nearest multiple of 'MAX_ALIGNMENT' (a power of 2):
    //:   'size = (size + MAX_ALIGNMENT - 1) & ~(MAX_ALIGNMENT - 1)'

    return (size + sizeOfBlock - 1)
           & ~(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);
}

                        // ---------------
                        // class BlockList
                        // ---------------

// CREATORS
BlockList::~BlockList()
{
    release();
}

// MANIPULATORS
void *BlockList::allocate(int size)
{
    BSLS_ASSERT(0 <= size);

    if (0 == size) {
        return 0;
    }

    size = alignedAllocationSize(size, sizeof(Block));

    Block *block = (Block *)d_allocator_p->allocate(size);

    BSLS_ASSERT(0 == bsls::AlignmentUtil::calculateAlignmentOffset(
                                     (void *)block,
                                     bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT));

    block->d_next_p       = (Block *)d_head_p;
    block->d_addrPrevNext = (Block **)&d_head_p;
    if (d_head_p) {
        d_head_p->d_addrPrevNext = &block->d_next_p;
    }
    d_head_p = block;

    BSLS_ASSERT(0 == bsls::AlignmentUtil::calculateAlignmentOffset(
                                     (void *)&block->d_memory,
                                     bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT));

    return (void *)&block->d_memory;
}

void BlockList::deallocate(void *address)
{
    if (address) {
        Block *block = (Block *)(void *)((char *)address - sizeof(Block) +
                                      bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        *block->d_addrPrevNext = block->d_next_p;
        if (block->d_next_p) {
            block->d_next_p->d_addrPrevNext = block->d_addrPrevNext;
        }
        d_allocator_p->deallocate(block);
    }
}

void BlockList::release()
{
    while (d_head_p) {
        void *lastBlock = d_head_p;
        d_head_p = d_head_p->d_next_p;
        d_allocator_p->deallocate(lastBlock);
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
