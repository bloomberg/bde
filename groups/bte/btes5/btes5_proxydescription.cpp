// btes5_proxydescription.cpp                                         -*-C++-*-
#include <btes5_proxydescription.h>

#include <bsl_ostream.h>
#include <bsls_assert.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btes5_proxydescription, "$Id$ $CSID$")

namespace BloombergLP {

                         // ----------------------------
                         // class btes5_ProxyDescription
                         // ----------------------------

// CREATORS
btes5_ProxyDescription::btes5_ProxyDescription(
    const bteso_Endpoint&         address,
    bslma::Allocator             *allocator)
: d_address(address, allocator)
, d_credentials(allocator)
{
}

btes5_ProxyDescription::btes5_ProxyDescription(
    const bteso_Endpoint&         address,
    const btes5_Credentials&  credentials,
    bslma::Allocator             *allocator)
: d_address(address, allocator)
, d_credentials(credentials, allocator)
{
}

btes5_ProxyDescription::btes5_ProxyDescription(
    const btes5_ProxyDescription& original,
    bslma::Allocator             *allocator)
: d_address(original.d_address, allocator)
, d_credentials(original.d_credentials, allocator)
{
}

btes5_ProxyDescription::~btes5_ProxyDescription()
{
}

// MANIPULATORS
void btes5_ProxyDescription::setAddress(const bteso_Endpoint& value)
{
    d_address = value;
}

void btes5_ProxyDescription::setCredentials(const btes5_Credentials& value)
{
    d_credentials = value;
}

// ACCESSORS
const bteso_Endpoint& btes5_ProxyDescription::address() const
{
    return d_address;
}

const btes5_Credentials& btes5_ProxyDescription::credentials() const
{
    return d_credentials;
}

// FREE OPERATORS
bool operator==(const btes5_ProxyDescription& lhs,
                const btes5_ProxyDescription& rhs)
{
    return lhs.address() == rhs.address()
        && lhs.credentials() == rhs.credentials();
}

bool operator!=(const btes5_ProxyDescription& lhs,
                const btes5_ProxyDescription& rhs)
{
    return lhs.address() != rhs.address()
        || lhs.credentials() != rhs.credentials();
}

bsl::ostream& operator<<(bsl::ostream&                 output,
                         const btes5_ProxyDescription& object)
{
    output << object.address();
    if (object.credentials().isSet()) {
        output << " (" << object.credentials() << ")";
    }
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
