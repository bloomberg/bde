// bsltf_stdtestallocator.cpp                                         -*-C++-*-
#include <bsltf_stdtestallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_newdeleteallocator.h>

#include <bsls_assert.h>

namespace
{
// STATIC DATA
// This global static data is declared and defined entirely hidden inside
// the .cpp file, as the IBM compiler may create multiple copies if accessed
// through inline functions defined in the header.
    static ::BloombergLP::bslma::Allocator
                              *s_StdTestAllocatorConfiguration_allocator_p = 0;
}  // close anonymous namespace

namespace BloombergLP {
namespace bsltf {


                        // -----------------------------------
                        // class StdTestAllocatorConfiguration
                        // -----------------------------------

// CLASS METHODS
bslma::Allocator* StdTestAllocatorConfiguration::delegateAllocator()
{
    return s_StdTestAllocatorConfiguration_allocator_p
         ? s_StdTestAllocatorConfiguration_allocator_p
         : &bslma::NewDeleteAllocator::singleton();
}

void StdTestAllocatorConfiguration::setDelegateAllocatorRaw(
                                              bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT_OPT(basicAllocator);

    s_StdTestAllocatorConfiguration_allocator_p = basicAllocator;
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
