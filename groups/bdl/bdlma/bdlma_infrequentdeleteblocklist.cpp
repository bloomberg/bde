// bdlma_infrequentdeleteblocklist.cpp                                -*-C++-*-
#include <bdlma_infrequentdeleteblocklist.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_infrequentdeleteblocklist_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {

// HELPER FUNCTIONS
static inline
int alignedAllocationSize(int size, int sizeOfBlock)
    // Return the allocation size (in bytes) required to ensure proper
    // alignment for a 'bdlma::InfrequentDeleteBlockList::Block' containing a
    // maximally-aligned payload of the specified 'size', where the specified
    // 'sizeOfBlock' is presumed to be
    // 'sizeof(bdlma::InfrequentDeleteBlockList::Block)'.  Note that, to ensure
    // that both the payload and header portions of the allocated memory are
    // each separately guaranteed to be maximally aligned in the presence of a
    // supplied allocator returning naturally-aligned memory, the size of the
    // overall allocation will be rounded up to an integral multiple of
    // 'bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT'.
{
    ///IMPLEMENTATION NOTE
    ///-------------------
    //: 1 Append the size of the 'bdlma::InfrequentDeleteBlockList::Block'
    //:   header:
    //:   'size += sizeof(Block) - bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT'
    //:
    //: 2 Round to the nearest multiple of MAX_ALIGNMENT (a power of 2):
    //:   'size = (size + MAX_ALIGNMENT - 1) & ~(MAX_ALIGNMENT - 1)'

    return (size + sizeOfBlock - 1)
           & ~(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);
}

namespace bdlma {

                  // -------------------------------
                  // class InfrequentDeleteBlockList
                  // -------------------------------

// CREATORS
InfrequentDeleteBlockList::~InfrequentDeleteBlockList()
{
    release();
}

// MANIPULATORS
void *InfrequentDeleteBlockList::allocate(int size)
{
    BSLS_ASSERT(0 <= size);

    if (0 == size) {
        return 0;                                                     // RETURN
    }

    size = alignedAllocationSize(size, sizeof(Block));

    Block *block = reinterpret_cast<Block *>(d_allocator_p->allocate(size));

    BSLS_ASSERT(0 == bsls::AlignmentUtil::calculateAlignmentOffset(
                                     reinterpret_cast<void *>(block),
                                     bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT));

    block->d_next_p = d_head_p;
    d_head_p        = block;

    BSLS_ASSERT(0 == bsls::AlignmentUtil::calculateAlignmentOffset(
                                    reinterpret_cast<void *>(&block->d_memory),
                                    bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT));

    return reinterpret_cast<void *>(&block->d_memory);
}

void InfrequentDeleteBlockList::release()
{
    while (d_head_p) {
        void *lastBlock = d_head_p;
        d_head_p        = d_head_p->d_next_p;
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
