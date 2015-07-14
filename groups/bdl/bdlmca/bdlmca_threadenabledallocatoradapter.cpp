// bdlmca_threadenabledallocatoradapter.cpp          -*-C++-*-
#include <bdlmca_threadenabledallocatoradapter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmca_threadenabledallocatoradapter_cpp,"$Id$ $CSID$")

#include <bdlmtt_lockguard.h>
#include <bdlmtt_xxxthread.h>

namespace BloombergLP {

namespace bdlmca {
            // -----------------------------------------
            // class ThreadEnabledAllocatorAdapter
            // -----------------------------------------

// CREATORS
ThreadEnabledAllocatorAdapter::~ThreadEnabledAllocatorAdapter()
{
}

// MANIPULATORS
void *ThreadEnabledAllocatorAdapter::allocate(size_type size)
{
    bdlmtt::LockGuard<bdlmtt::Mutex> guard(d_mutex_p);
    return d_allocator_p->allocate(size);
}

void ThreadEnabledAllocatorAdapter::deallocate(void *address)
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
