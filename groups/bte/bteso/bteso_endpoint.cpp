// bteso_endpoint.cpp                                                 -*-C++-*-
#include <bteso_endpoint.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_endpoint_cpp, "$Id$ $CSID$")

#include <bsl_ostream.h>
#include <bsls_assert.h>

namespace BloombergLP {

                            // --------------------
                            // class bteso_Endpoint
                            // --------------------

// CLASS METHODS
bool bteso_Endpoint::isValid(const bslstl::StringRef& hostname, int port)
{
    return (1 <= hostname.length() && hostname.length() <= 255
            && 1 <= port && port <= 65535)
        || (hostname.isEmpty() && 0 == port);
}

// CREATORS
bteso_Endpoint::bteso_Endpoint(bslma::Allocator *allocator)
: d_hostname(bslma::Default::allocator(allocator))
, d_port(0)
{
}

bteso_Endpoint::bteso_Endpoint(const bteso_Endpoint&  original,
                               bslma::Allocator      *allocator)
: d_hostname(original.d_hostname, bslma::Default::allocator(allocator))
, d_port(original.d_port)
{
}

bteso_Endpoint::bteso_Endpoint(const bslstl::StringRef& hostname,
                               int                      port,
                               bslma::Allocator        *allocator)
: d_hostname(bslma::Default::allocator(allocator))
, d_port(0)
{
    set(hostname, port);
}

bteso_Endpoint::~bteso_Endpoint()
{
}

// MANIPULATORS
void bteso_Endpoint::set(const bslstl::StringRef& hostname, int port)
{
    BSLS_ASSERT(isValid(hostname, port));
    d_hostname = hostname;
    d_port = port;
}

int bteso_Endpoint::setIfValid(const bslstl::StringRef& hostname, int port)
{
    if (isValid(hostname, port)) {
        d_hostname = hostname;
        d_port = port;
        return 0;
    }
    else {
        return -1;
    }
}

void bteso_Endpoint::reset()
{
    d_port = 0;
    d_hostname.clear();
}

// ACCESSORS
const bsl::string& bteso_Endpoint::hostname() const
{
    return d_hostname;
}

int bteso_Endpoint::port() const
{
    return d_port;
}

// FREE OPERATORS
bool operator==(const bteso_Endpoint& lhs, const bteso_Endpoint& rhs)
{
    return lhs.port() == rhs.port() && lhs.hostname() == rhs.hostname();
}

bool operator!=(const bteso_Endpoint& lhs, const bteso_Endpoint& rhs)
{
    return ! (lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream&         output,
                         const bteso_Endpoint& object)
{
    output << object.hostname() << ':' << object.port();
    return output;
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
