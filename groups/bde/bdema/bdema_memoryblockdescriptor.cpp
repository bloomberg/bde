// bdema_memoryblockdescriptor.cpp           -*-C++-*-
#include <bdema_memoryblockdescriptor.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_memoryblockdescriptor_cpp,"$Id$ $CSID$")

#include <bslma_defaultallocatorguard.h>   // for testing only
#include <bslma_testallocator.h>           // for testing only

#include <bsls_platform.h>

#include <cstdio>                          // for 'std::printf'

namespace BloombergLP {

              // ---------------------------------
              // class bdema_MemoryBlockDescriptor
              // ---------------------------------

// ACCESSORS
void bdema_MemoryBlockDescriptor::print() const
{
#ifdef BSLS_PLATFORM_CPU_64_BIT
    std::printf("[%p, %lld]", address(), (long long) size());
#else
    std::printf("[%p, %d]", address(), size());
#endif
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
