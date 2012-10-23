// bslalg_hashtablebucket.cpp                                         -*-C++-*-
#include <bslalg_hashtablebucket.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslalg_bidirectionallink.h>
#include <bslalg_bidirectionallinklistutil.h>    // Testing only

namespace BloombergLP {

namespace bslalg {

native_std::size_t HashTableBucket::countElements() const
{
    native_std::size_t result = 0;
    for (BidirectionalLink *cursor     = d_first_p,
                           * endBucket = end();
                            endBucket != cursor; cursor = cursor->nextLink()) {
        ++result;
    }

    return result;
}

}  // close namespace bslalg
}  // close namespace BloombergLP
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
