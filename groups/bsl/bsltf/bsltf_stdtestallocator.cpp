// bsltf_stdtestallocator.cpp                                 -*-C++-*-
#include <bsltf_stdtestallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {
namespace bsltf {


                        // -----------------------------------
                        // class StdTestAllocatorConfiguration
                        // -----------------------------------

// STATIC DATA
bslma_Allocator *StdTestAllocatorConfiguration::s_allocator_p = 0;


// CLASS METHODS
void StdTestAllocatorConfiguration::setDelegateAllocatorRaw(
                                              bslma_Allocator *basicAllocator)
{
    s_allocator_p = basicAllocator;
}

                        // ----------------------
                        // class StdTestAllocator
                        // ----------------------


}  // close package namespace
}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
