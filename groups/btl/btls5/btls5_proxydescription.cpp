// btls5_proxydescription.cpp                                         -*-C++-*-
#include <btls5_proxydescription.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btls5_proxydescription_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace btls5 {
                         // ----------------------------
                         // class ProxyDescription
                         // ----------------------------

// CREATORS
ProxyDescription::ProxyDescription(
                                         bslma::Allocator      *basicAllocator)
: d_address(basicAllocator)
, d_credentials(basicAllocator)
{
}

ProxyDescription::ProxyDescription(
                                         const btlso::Endpoint&  address,
                                         bslma::Allocator      *basicAllocator)
: d_address(address, basicAllocator)
, d_credentials(basicAllocator)
{
}

ProxyDescription::ProxyDescription(
                                      const btlso::Endpoint&     address,
                                      const Credentials&  credentials,
                                      bslma::Allocator         *basicAllocator)
: d_address(address, basicAllocator)
, d_credentials(credentials, basicAllocator)
{
}

ProxyDescription::ProxyDescription(
                                 const ProxyDescription&  original,
                                 bslma::Allocator              *basicAllocator)
: d_address(original.d_address, basicAllocator)
, d_credentials(original.d_credentials, basicAllocator)
{
}

// ACCESSORS

                                  // Aspects

bsl::ostream& ProxyDescription::print(bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("address",     d_address);
    printer.printAttribute("credentials", d_credentials);
    printer.end();

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bool btls5::operator==(const ProxyDescription& lhs,
                const ProxyDescription& rhs)
{
    return lhs.address()     == rhs.address()
        && lhs.credentials() == rhs.credentials();
}

bool btls5::operator!=(const ProxyDescription& lhs,
                const ProxyDescription& rhs)
{
    return lhs.address()     != rhs.address()
        || lhs.credentials() != rhs.credentials();
}

bsl::ostream& btls5::operator<<(bsl::ostream&                 stream,
                         const ProxyDescription& object)
{
    stream << "[ " << object.address();
    if (!object.credentials().username().empty()) {
        stream << ' ' << object.credentials();
    }
    stream << " ]";

    return stream;
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
