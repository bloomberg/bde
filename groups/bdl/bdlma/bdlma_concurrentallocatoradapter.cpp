// bdlma_concurrentallocatoradapter.cpp          -*-C++-*-
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
