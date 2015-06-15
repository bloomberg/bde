// bslma_infrequentdeleteblocklist.cpp                                -*-C++-*-
#ifndef BDE_OPENSOURCE_PUBLICATION // DEPRECATED

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

    numBytes += static_cast<int>(sizeof(Block)) - 1;
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

#endif // BDE_OPENSOURCE_PUBLICATION -- DEPRECATED

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
