// bael_countingallocator.cpp       -*-C++-*-
#include <bael_countingallocator.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_countingallocator_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {

                        // ----------------------------
                        // class bael_CountingAllocator
                        // ----------------------------

// CREATORS

bael_CountingAllocator::~bael_CountingAllocator()
{
    BSLS_ASSERT(d_allocator_p);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
