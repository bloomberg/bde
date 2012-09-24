// bslma_default.cpp                                                  -*-C++-*-
#include <bslma_default.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_allocator.h>            // for testing only
#include <bsls_assert.h>

namespace BloombergLP {

namespace bslma {

class Allocator;

                               // --------------
                               // struct Default
                               // --------------

// STATIC DATA MEMBERS

                        // *** default allocator ***

bsls::AtomicOperations::AtomicTypes::Pointer Default::s_allocator = {0};
bsls::AtomicOperations::AtomicTypes::Int     Default::s_locked    = {0};

                        // *** global allocator ***

bsls::AtomicOperations::AtomicTypes::Pointer Default::s_globalAllocator = {0};

// CLASS METHODS

                        // *** default allocator ***

int Default::setDefaultAllocator(Allocator *basicAllocator)
{
    BSLS_ASSERT(0 != basicAllocator);

    if (!bsls::AtomicOperations::getIntRelaxed(&s_locked)) {
        bsls::AtomicOperations::setPtrRelease(&s_allocator, basicAllocator);
        return 0;  // success
    }

    return -1;     // locked -- 'set' fails
}

void Default::setDefaultAllocatorRaw(Allocator *basicAllocator)
{
    BSLS_ASSERT(0 != basicAllocator);

    bsls::AtomicOperations::setPtrRelease(&s_allocator, basicAllocator);
}

                        // *** global allocator ***

Allocator *Default::setGlobalAllocator(Allocator *basicAllocator)
{
    Allocator *previous =
        (Allocator *) bsls::AtomicOperations::swapPtrAcqRel(&s_globalAllocator,
                                                            basicAllocator);

    return previous ? previous
                    : &NewDeleteAllocator::singleton();
}

}  // close package namespace

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
