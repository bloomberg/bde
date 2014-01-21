// bslma_infrequentdeleteblocklist.cpp                                -*-C++-*-
#ifndef BDE_OMIT_TRANSITIONAL // DEPRECATED

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

#endif // BDE_OMIT_TRANSITIONAL -- DEPRECATED

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
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
