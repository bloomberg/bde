// bdlma_memoryblockdescriptor.cpp           -*-C++-*-
#include <bdlma_memoryblockdescriptor.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_memoryblockdescriptor_cpp,"$Id$ $CSID$")

#include <bslma_defaultallocatorguard.h>   // for testing only
#include <bslma_testallocator.h>           // for testing only

#include <bsls_platform.h>
#include <bsls_bsltestutil.h>

#include <cstdio>                          // for 'std::printf'

namespace BloombergLP {

namespace bdlma {
              // ---------------------------------
              // class MemoryBlockDescriptor
              // ---------------------------------

// ACCESSORS
void MemoryBlockDescriptor::print() const
{
#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU
    // Unfortunately, %zu is not correctly supported across all platforms.

    std::printf("[%p, " ZU "]", address(), size());
#undef ZU
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
