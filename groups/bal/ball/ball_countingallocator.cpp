// ball_countingallocator.cpp       -*-C++-*-
#include <ball_countingallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_countingallocator_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {

namespace ball {
                        // ----------------------------
                        // class CountingAllocator
                        // ----------------------------

// CREATORS

CountingAllocator::~CountingAllocator()
{
    BSLS_ASSERT(d_allocator_p);
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
