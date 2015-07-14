// btlmt_session.cpp                                                  -*-C++-*-
#include <btlmt_session.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlmt_session_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace btlmt {
                            // -------------------
                            // class Session
                            // -------------------

// CREATORS
Session::~Session()
{
}

                            // --------------------------
                            // class SessionFactory
                            // --------------------------

// CREATORS
SessionFactory::~SessionFactory()
{
}

// MANIPULATORS
void SessionFactory::allocate(
                           const bsl::shared_ptr<AsyncChannel>& channel,
                           const SessionFactory::Callback&      callback)
{
    allocate(channel.get(), callback);
}
}  // close package namespace

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
