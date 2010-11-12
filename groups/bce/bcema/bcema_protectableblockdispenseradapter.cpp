// bcema_protectableblockdispenseradapter.cpp          -*-C++-*-
#include <bcema_protectableblockdispenseradapter.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcema_protectableblockdispenseradapter_cpp,"$Id$ $CSID$")


#include <bcemt_lockguard.h>
#include <bcemt_thread.h>

namespace BloombergLP {

               // --------------------------------------------
               // class bcema_ProtectableBlockDispenserAdapter
               // --------------------------------------------

// CREATORS
bcema_ProtectableBlockDispenserAdapter::
~bcema_ProtectableBlockDispenserAdapter()
{
}

// MANIPULATORS
bdema_MemoryBlockDescriptor bcema_ProtectableBlockDispenserAdapter::allocate(
                                                            size_type numBytes)
{
    bcemt_LockGuard<bcemt_Mutex> guard(d_mutex_p);
    return d_dispenser_p->allocate(numBytes);
}

void bcema_ProtectableBlockDispenserAdapter::deallocate(
                                      const bdema_MemoryBlockDescriptor& block)
{
    bcemt_LockGuard<bcemt_Mutex> guard(d_mutex_p);
    d_dispenser_p->deallocate(block);
}

int bcema_ProtectableBlockDispenserAdapter::protect(
                                     const bdema_MemoryBlockDescriptor& block)
{
    bcemt_LockGuard<bcemt_Mutex> guard(d_mutex_p);
    return d_dispenser_p->protect(block);
}

int bcema_ProtectableBlockDispenserAdapter::unprotect(
                                      const bdema_MemoryBlockDescriptor& block)
{
    bcemt_LockGuard<bcemt_Mutex> guard(d_mutex_p);
    return d_dispenser_p->unprotect(block);
}

// ACCESSORS
int bcema_ProtectableBlockDispenserAdapter::minimumBlockSize() const
{
    return d_dispenser_p->minimumBlockSize();
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
