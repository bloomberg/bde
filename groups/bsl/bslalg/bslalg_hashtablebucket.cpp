// bslalg_hashtablebucket.cpp                                         -*-C++-*-
#include <bslalg_hashtablebucket.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslalg_bidirectionallink.h>

namespace BloombergLP
{

namespace bslalg
{

int HashTableBucket::size() const
{
    if (BidirectionalLink *cursor = d_first_p) {
        int result = 1;
        while (cursor != d_last_p) {
            cursor = cursor->next();
            ++result;
        }
        return result;
    }
    else {
        return 0;
    }
}


}  // close namespace BloobmergLP::bslalg
}  // close namespace BloombergLP
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
