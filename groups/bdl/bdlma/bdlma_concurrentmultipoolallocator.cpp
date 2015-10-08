// bdlma_concurrentmultipoolallocator.cpp                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_concurrentmultipoolallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_concurrentmultipoolallocator_cpp,"$Id$ $CSID$")

#include <bsls_performancehint.h>

namespace BloombergLP {
namespace bdlma {

                    // ----------------------------------
                    // class ConcurrentMultipoolAllocator
                    // ----------------------------------

// CREATORS
ConcurrentMultipoolAllocator::~ConcurrentMultipoolAllocator()
{
}

// MANIPULATORS
void *ConcurrentMultipoolAllocator::allocate(size_type size)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;                                                     // RETURN
    }

    return d_multipool.allocate(static_cast<int>(size));
}

void ConcurrentMultipoolAllocator::deallocate(void *address)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(address != 0)) {
        d_multipool.deallocate(address);
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
}

void ConcurrentMultipoolAllocator::release()
{
    d_multipool.release();
}

void ConcurrentMultipoolAllocator::reserveCapacity(size_type size,
                                                   size_type numObjects)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return;                                                       // RETURN
    }

    d_multipool.reserveCapacity(static_cast<int>(size),
                                static_cast<int>(numObjects));
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
