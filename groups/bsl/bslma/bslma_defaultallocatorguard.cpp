// bslma_defaultallocatorguard.cpp                                    -*-C++-*-
#include <bslma_defaultallocatorguard.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_default.h>
#include <bslma_newdeleteallocator.h>      // for testing only
#include <bslma_testallocator.h>           // for testing only
#include <bsls_assert.h>

namespace BloombergLP {

namespace bslma {

                        // ---------------------------
                        // class DefaultAllocatorGuard
                        // ---------------------------

// CREATORS
DefaultAllocatorGuard::DefaultAllocatorGuard(Allocator *temporary)
: d_original_p(Default::defaultAllocator())
{
    BSLS_ASSERT(temporary);

    Default::setDefaultAllocatorRaw(temporary);
}

DefaultAllocatorGuard::~DefaultAllocatorGuard()
{
    BSLS_ASSERT(d_original_p);

    Default::setDefaultAllocatorRaw(d_original_p);
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
