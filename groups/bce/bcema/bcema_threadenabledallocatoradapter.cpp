// bcema_threadenabledallocatoradapter.cpp          -*-C++-*-
#include <bcema_threadenabledallocatoradapter.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcema_threadenabledallocatoradapter_cpp,"$Id$ $CSID$")

#include <bcemt_lockguard.h>
#include <bcemt_thread.h>

namespace BloombergLP {

            // -----------------------------------------
            // class bcema_ThreadEnabledAllocatorAdapter
            // -----------------------------------------

// CREATORS
bcema_ThreadEnabledAllocatorAdapter::~bcema_ThreadEnabledAllocatorAdapter()
{
}

// MANIPULATORS
void *bcema_ThreadEnabledAllocatorAdapter::allocate(size_type size)
{
    bcemt_LockGuard<bcemt_Mutex> guard(d_mutex_p);
    return d_allocator_p->allocate(size);
}

void bcema_ThreadEnabledAllocatorAdapter::deallocate(void *address)
{
    bcemt_LockGuard<bcemt_Mutex> guard(d_mutex_p);
    d_allocator_p->deallocate(address);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
