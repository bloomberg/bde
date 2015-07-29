// bdlmca_xxxprotectableblockdispenseradapter.cpp          -*-C++-*-
#include <bdlmca_xxxprotectableblockdispenseradapter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmca_xxxprotectableblockdispenseradapter_cpp,"$Id$ $CSID$")


#include <bdlqq_lockguard.h>
#include <bdlqq_xxxthread.h>

namespace BloombergLP {

namespace bdlmca {
               // --------------------------------------------
               // class ProtectableBlockDispenserAdapter
               // --------------------------------------------

// CREATORS
ProtectableBlockDispenserAdapter::
~ProtectableBlockDispenserAdapter()
{
}

// MANIPULATORS
bdlma::MemoryBlockDescriptor ProtectableBlockDispenserAdapter::allocate(
                                                            size_type numBytes)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(d_mutex_p);
    return d_dispenser_p->allocate(numBytes);
}

void ProtectableBlockDispenserAdapter::deallocate(
                                      const bdlma::MemoryBlockDescriptor& block)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(d_mutex_p);
    d_dispenser_p->deallocate(block);
}

int ProtectableBlockDispenserAdapter::protect(
                                     const bdlma::MemoryBlockDescriptor& block)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(d_mutex_p);
    return d_dispenser_p->protect(block);
}

int ProtectableBlockDispenserAdapter::unprotect(
                                      const bdlma::MemoryBlockDescriptor& block)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(d_mutex_p);
    return d_dispenser_p->unprotect(block);
}

// ACCESSORS
int ProtectableBlockDispenserAdapter::minimumBlockSize() const
{
    return d_dispenser_p->minimumBlockSize();
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
