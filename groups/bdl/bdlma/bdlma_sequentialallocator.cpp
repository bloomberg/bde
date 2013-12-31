// bdlma_sequentialallocator.cpp                                      -*-C++-*-
#include <bdlma_sequentialallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_sequentialallocator_cpp,"$Id$ $CSID$")

#include <bsls_performancehint.h>

namespace BloombergLP {
namespace bdlma {

                      // -------------------------
                      // class SequentialAllocator
                      // -------------------------

// CREATORS
SequentialAllocator::~SequentialAllocator()
{
}

// MANIPULATORS
void *SequentialAllocator::allocate(size_type size)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;                                                     // RETURN
    }

    return d_sequentialPool.allocate(size);
}

void *SequentialAllocator::allocateAndExpand(size_type *size)
{
    BSLS_ASSERT(size);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == *size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;                                                     // RETURN
    }

    return d_sequentialPool.allocateAndExpand(size);
}

void SequentialAllocator::reserveCapacity(int numBytes)
{
    BSLS_ASSERT(0 <= numBytes);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == numBytes)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return;                                                       // RETURN
    }

    d_sequentialPool.reserveCapacity(numBytes);
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
