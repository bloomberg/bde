// btemt_socks5connector.cpp                                          -*-C++-*-
#include <btemt_socks5connector.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_socks5connector_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace btemt {

                        // ---------------------
                        // class Socks5Connector
                        // ---------------------
// CLASS METHODS
const char *Socks5Connector::errorToString(int status)
{
#define CASE(X) case(X): return #X;
    switch (status)
    {
        CASE(SOCKS5CONNECTOR_SUCCESS)
        CASE(SOCKS5CONNECTOR_METHOD_REQUEST_WRITE)
        CASE(SOCKS5CONNECTOR_REGISTER_READ)
        CASE(SOCKS5CONNECTOR_METHOD_RESPONSE_READ)
        CASE(SOCKS5CONNECTOR_UNSUPPORTED_METHOD)
        CASE(SOCKS5CONNECTOR_CONNECT_REQUEST_WRITE)
        CASE(SOCKS5CONNECTOR_CONNECT_RESPONSE_READ)
        CASE(SOCKS5CONNECTOR_CONNECT_SOCKS_SERVER_FAILURE)
        CASE(SOCKS5CONNECTOR_CONNECT_CONNECTION_NOT_ALLOWED)
        CASE(SOCKS5CONNECTOR_CONNECT_NETWORK_UNREACHABLE)
        CASE(SOCKS5CONNECTOR_CONNECT_HOST_UNREACHABLE)
        CASE(SOCKS5CONNECTOR_CONNECT_CONNECTION_REFUSED)
        CASE(SOCKS5CONNECTOR_CONNECT_TTL_EXPIRED)
        CASE(SOCKS5CONNECTOR_CONNECT_COMMAND_NOT_SUPPORTED)
        CASE(SOCKS5CONNECTOR_CONNECT_ADDRESS_TYPE_NOT_SUPPORTED)
        CASE(SOCKS5CONNECTOR_CONNECT_OTHER)
        default: return "(* NA *)";
    };
#undef CASE
}


}  // close package namespace
}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
