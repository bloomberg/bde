// bsltf_stdtestallocator.cpp                                 -*-C++-*-
#include <bsltf_stdtestallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_newdeleteallocator.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bsltf {


                        // -----------------------------------
                        // class StdTestAllocatorConfiguration
                        // -----------------------------------

// STATIC DATA
// This global static data is declared and defined entirely hidden inside
// the .cpp file, as the IBM compiler may create multiple copies if accessed
// through inline functions defined in the header.
static bslma::Allocator *StdTestAllocatorConfiguration_s_allocator_p = 0;


// CLASS METHODS
bslma::Allocator* StdTestAllocatorConfiguration::delegateAllocator()
{
    return StdTestAllocatorConfiguration_s_allocator_p
         ? StdTestAllocatorConfiguration_s_allocator_p
         : &bslma::NewDeleteAllocator::singleton();
}

void StdTestAllocatorConfiguration::setDelegateAllocatorRaw(
                                              bslma::Allocator *basicAllocator)
{
    StdTestAllocatorConfiguration_s_allocator_p = basicAllocator;
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
