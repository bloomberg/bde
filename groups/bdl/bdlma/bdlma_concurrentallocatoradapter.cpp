// bdlma_concurrentallocatoradapter.cpp                               -*-C++-*-
#include <bdlma_concurrentallocatoradapter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_concurrentallocatoradapter_cpp,"$Id$ $CSID$")

#include <bdlmtt_lockguard.h>
#include <bdlmtt_xxxthread.h>

namespace BloombergLP {

namespace bdlma {
            // -----------------------------------------
            // class ConcurrentAllocatorAdapter
            // -----------------------------------------

// CREATORS
ConcurrentAllocatorAdapter::~ConcurrentAllocatorAdapter()
{
}

// MANIPULATORS
void *ConcurrentAllocatorAdapter::allocate(size_type size)
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(d_mutex_p);
    return d_allocator_p->allocate(size);
}

void ConcurrentAllocatorAdapter::deallocate(void *address)
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(d_mutex_p);
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
