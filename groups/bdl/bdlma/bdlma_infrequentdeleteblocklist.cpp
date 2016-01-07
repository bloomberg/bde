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
