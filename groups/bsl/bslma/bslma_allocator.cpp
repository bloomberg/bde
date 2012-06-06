// bslma_allocator.cpp                                                -*-C++-*-
#include <bslma_allocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_exceptionutil.h>

#include <new>   // 'std::bad_alloc'

namespace BloombergLP {

namespace bslma {

                        // ---------------
                        // class Allocator
                        // ---------------

// CLASS METHODS
void Allocator::throwBadAlloc()
{
    BSLS_THROW(std::bad_alloc());
}

// CREATORS
Allocator::~Allocator()
{
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
