// bslma_testallocatormonitor.cpp                                     -*-C++-*-
#include <bslma_testallocatormonitor.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_allocator.h>      // for testing only
#include <bslma_default.h>        // for testing only

namespace BloombergLP {

//
///IMPLEMENTATION NOTE
///-------------------
// The static function 'validateArgument' tests (in SAFE mode) the specified
// 'testAllocator' before it is dereferenced for any member of the
// initialization list.  Should this implementation change (e.g., the insertion
// of other test allocator statistics before 'd_initialInUse'), the
// 'validateArgument' test must be migrated to the first dereference of
// 'testAllocator'.

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
