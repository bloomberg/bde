// bslma_allocator.cpp          -*-C++-*-
#include <bslma_allocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_alignment.h>                     // for testing only
#include <bsls_exceptionutil.h>
#include <bsls_platform.h>

#ifdef BSLS_PLATFORM__CMP_HP
#include <new>
#else
#include <stdexcept>
#endif

namespace BloombergLP {

bslma_Allocator::~bslma_Allocator()
{
}

void bslma_Allocator::throwBadAlloc()
{
    BSLS_THROW(std::bad_alloc());
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
