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
