// bteso_endpoint.cpp                                                 -*-C++-*-
#include <bteso_endpoint.h>

#include <bsl_ostream.h>
#include <bsls_assert.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_endpoint_cpp, "$Id$ $CSID$")

namespace BloombergLP {

namespace {

}  // close anonymous namespace

                            // --------------------
                            // class bteso_Endpoint
                            // --------------------

// CREATORS
bteso_Endpoint::bteso_Endpoint(bslma_Allocator *allocator)
: d_hostname(bslma::Default::allocator(allocator))
, d_port(-1)
{
    // 'port >=- 0' is the internal indicator for 'this->isSet()'.
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
, d_port(-1)
{
    set(hostname, port);
}

bteso_Endpoint::~bteso_Endpoint()
{
}

// MANIPULATORS
void bteso_Endpoint::set(const bslstl::StringRef& hostname, int port)
{
    BSLS_ASSERT(0 < hostname.length() && hostname.length() <= 255);
    BSLS_ASSERT(0 <= port && port <= 65535);
    d_hostname = hostname;
    d_port = port;
}

void bteso_Endpoint::reset()
{
    d_port = -1;
    d_hostname.clear();
}

// ACCESSORS
bool bteso_Endpoint::isSet() const
{
    return d_port >= 0;
}

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
    return ((!lhs.isSet() && !rhs.isSet())
            || (lhs.port() == rhs.port() && lhs.hostname() == rhs.hostname()));
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
