// bteso_event.cpp             -*-C++-*-
#include <bteso_event.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_event_cpp,"$Id$ $CSID$")

#include <bsl_hash_map.h>

namespace BloombergLP {

                           // ---------------------
                           // class bteso_EventHash
                           // ---------------------

// ACCESSORS
bsl::size_t bteso_EventHash::operator()(const bteso_Event& value) const
{
    bsl::hash<int> hasher;
    return hasher((value.type() << 24) ^ (int) value.handle());
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
