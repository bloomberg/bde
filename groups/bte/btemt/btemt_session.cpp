// btemt_session.cpp                                                  -*-C++-*-
#include <btemt_session.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_session_cpp,"$Id$ $CSID$")

namespace BloombergLP {

                            // -------------------
                            // class btemt_Session
                            // -------------------

// CREATORS
btemt_Session::~btemt_Session()
{
}

                            // --------------------------
                            // class btemt_SessionFactory
                            // --------------------------

// CREATORS
btemt_SessionFactory::~btemt_SessionFactory()
{
}

// MANIPULATORS
void btemt_SessionFactory::allocate(
                           const bcema_SharedPtr<btemt_AsyncChannel>& channel,
                           const btemt_SessionFactory::Callback&      callback)
{
    allocate(channel.ptr(), callback);
}

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
