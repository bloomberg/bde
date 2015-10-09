// bdlma_concurrentallocatoradapter.cpp                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_concurrentallocatoradapter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_concurrentallocatoradapter_cpp,"$Id$ $CSID$")

#include <bslmt_lockguard.h>

namespace BloombergLP {
namespace bdlma {

                     // --------------------------------
                     // class ConcurrentAllocatorAdapter
                     // --------------------------------

// CREATORS
ConcurrentAllocatorAdapter::~ConcurrentAllocatorAdapter()
{
}

// MANIPULATORS
void *ConcurrentAllocatorAdapter::allocate(size_type numBytes)
{
    bslmt::LockGuard<bslmt::Mutex> guard(d_mutex_p);
    return d_allocator_p->allocate(numBytes);
}

void ConcurrentAllocatorAdapter::deallocate(void *address)
{
    bslmt::LockGuard<bslmt::Mutex> guard(d_mutex_p);
    d_allocator_p->deallocate(address);
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
