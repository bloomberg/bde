// btlso_event.cpp             -*-C++-*-
#include <btlso_event.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_event_cpp,"$Id$ $CSID$")

#include <bsl_functional.h>

namespace BloombergLP {

namespace btlso {
                           // ---------------------
                           // class EventHash
                           // ---------------------

// ACCESSORS
bsl::size_t EventHash::operator()(const Event& value) const
{
    bsl::hash<int> hasher;
    return hasher((value.type() << 24) ^ (int) value.handle());
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
