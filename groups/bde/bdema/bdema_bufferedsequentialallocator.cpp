// bdema_bufferedsequentialallocator.cpp                              -*-C++-*-
#include <bdema_bufferedsequentialallocator.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_bufferedsequentialallocator_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_performancehint.h>

namespace BloombergLP {

                    // ---------------------------------------
                    // class bdema_BufferedSequentialAllocator
                    // ---------------------------------------

// CREATORS
bdema_BufferedSequentialAllocator::~bdema_BufferedSequentialAllocator()
{
    d_pool.release();
}

// MANIPULATORS
void *bdema_BufferedSequentialAllocator::allocate(bsls_Types::size_type size)
{
    BSLS_ASSERT(0 <= size);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;                                                     // RETURN
    }

    return d_pool.allocate(size);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
