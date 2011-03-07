// bslma_default.cpp                  -*-C++-*-
#include <bslma_default.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_allocator.h>            // for testing only
#include <bsls_assert.h>

namespace BloombergLP {

class bslma_Allocator;

                        // --------------------
                        // struct bslma_Default
                        // --------------------

// STATIC DATA MEMBERS

                        // *** default allocator ***

bslma_Allocator *bslma_Default::s_allocator_p = 0;
int              bslma_Default::s_locked      = 0;

                        // *** global allocator ***

bslma_Allocator *bslma_Default::s_globalAllocator_p = 0;

// CLASS METHODS

                        // *** default allocator ***

int bslma_Default::setDefaultAllocator(bslma_Allocator *basicAllocator)
{
    BSLS_ASSERT(0 != basicAllocator);

    if (!s_locked) {
        s_allocator_p = basicAllocator;
        return 0;  // success
    }
    return -1;     // locked -- 'set' fails
}

void bslma_Default::setDefaultAllocatorRaw(bslma_Allocator *basicAllocator)
{
    BSLS_ASSERT(0 != basicAllocator);

    s_allocator_p = basicAllocator;
}

                        // *** global allocator ***

bslma_Allocator *bslma_Default::setGlobalAllocator(
                                               bslma_Allocator *basicAllocator)
{
    bslma_Allocator *previous = s_globalAllocator_p
                                      ? s_globalAllocator_p
                                      : &bslma_NewDeleteAllocator::singleton();

    s_globalAllocator_p = basicAllocator;

    return previous;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
