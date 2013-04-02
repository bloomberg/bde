// btemt_internetaddress.cpp               -*-C++-*-
#include <btemt_internetaddress.h>

#include <bteso_resolveutil.h>
#include <bdepcre_regex.h>
#include <bdeut_stringref.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace btemt {

                    // --------------------------------
                    // class InternetAddress_StringPort
                    // --------------------------------

InternetAddress_StringPort::InternetAddress_StringPort(
                                             const bdeut_StringRef&  host,
                                             unsigned short          port,
                                             bslma::Allocator        *allocator)
: d_host(host, allocator)
, d_port(port)
{
}


InternetAddress_StringPort::InternetAddress_StringPort(
                                 const InternetAddress_StringPort&  rhs,
                                  bslma::Allocator                   *allocator)
: d_host(rhs.d_host, allocator)
, d_port(rhs.d_port)
{
}

                        // ---------------------
                        // class InternetAddress
                        // ---------------------

// CREATORS
InternetAddress::InternetAddress(const bdeut_StringRef&  host,
                                 unsigned short          port,
                                 bslma::Allocator        *allocator)
: d_address(allocator)
{
    if (0 == bteso_IPv4Address::isValid(host.data())) {
        d_address.assign(bteso_IPv4Address(host.data(), port));
    }
    else {
        d_address.assign(InternetAddress_StringPort(host, port));
    }
}

InternetAddress::InternetAddress(const bteso_IPv4Address&  address,
                                 bslma::Allocator          *allocator)
: d_address(address, allocator)
{
}

InternetAddress::InternetAddress(bslma::Allocator *allocator)
: d_address(allocator)
{
}

InternetAddress::InternetAddress(const InternetAddress&  other,
                                 bslma::Allocator        *allocator)
: d_address(other.d_address, allocator)
{
}

InternetAddress::~InternetAddress()
{
}

// MANIPULATORS
InternetAddress& InternetAddress::operator=(const InternetAddress& rhs)
{
    if (this == &rhs) {
        return *this;
    }
    d_address = rhs.d_address;
    return *this;
}

// ACCESSORS
bool InternetAddress::isDomainname() const
{
    return d_address.is<InternetAddress_StringPort>();
}

bool InternetAddress::isIPv4() const
{
    return d_address.is<bteso_IPv4Address>();
}

bool InternetAddress::isUnset() const
{
    return d_address.isUnset();
}

const bsl::string& InternetAddress::domainnameHost() const
{
    BSLS_ASSERT(isDomainname());
    return d_address.the<InternetAddress_StringPort>().d_host;
}

unsigned short InternetAddress::port() const
{
    if (isDomainname()) {
        return d_address.the<InternetAddress_StringPort>().d_port;
    }
    else if (isIPv4()) {
        return d_address.the<bteso_IPv4Address>().portNumber();
    }
    else {
        BSLS_ASSERT(isUnset());
        return 0;
    }
}

const bteso_IPv4Address& InternetAddress::ipv4() const
{
    return d_address.the<bteso_IPv4Address>();
}

bsl::ostream& InternetAddress::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    if (isIPv4()) {
        stream << "[IPv4 = " << d_address.the<bteso_IPv4Address>() << "]";
    }
    else {
        stream << "[DNSName = "
               << d_address.the<InternetAddress_StringPort>().d_host
               << ", Port = "
               << d_address.the<InternetAddress_StringPort>().d_port << "]";
    }
    return stream;
}

bsl::string InternetAddressUtil::getHostName(const InternetAddress& addr)
{
    if (addr.isUnset()) {
        return bsl::string();
    }
    else if (addr.isDomainname()) {
        return addr.domainnameHost();
    }
    else {
        BSLS_ASSERT(addr.isIPv4());
        const bteso_IPv4Address& ipv4Address = addr.ipv4();
        char addrStr[16];
        ipv4Address.loadIpAddress(addrStr);
        return addrStr;
    }
}


int InternetAddressUtil::loadAddressPort(InternetAddress        *addr,
                                         const bdeut_StringRef&  hostPort)
{
    BSLS_ASSERT(addr);
    bdepcre_RegEx hostPortRegEx;
    int rc = hostPortRegEx.prepare(
                             0, 0, "^(?P<hostname>[^:]+)(:(?P<port>\\d+))?", 0);
    BSLS_ASSERT(0 == rc);
    bsl::vector<bsl::pair<int, int> > matchVector;
    rc = hostPortRegEx.match(&matchVector, hostPort.data(), hostPort.length());
    if (0 != rc) {
        return rc;
    }
    int port = 0;
    const bsl::pair<int, int> capturedHost =
                        matchVector[hostPortRegEx.subpatternIndex("hostname")];
    bsl::string theHost = bsl::string(hostPort.data() + capturedHost.first,
                                      capturedHost.second);
    const bsl::pair<int, int> capturedPort =
                            matchVector[hostPortRegEx.subpatternIndex("port")];
    if (capturedPort.second > 0) {
        bsl::string thePort = bsl::string(hostPort.data() + capturedPort.first,
                                          capturedPort.second);
        bsl::stringstream ss(thePort);
        ss >> port;
        BSLS_ASSERT(ss);
    }

    *addr = InternetAddress(theHost, port);
    return 0;
}

int InternetAddressUtil::resolve(bteso_IPv4Address      *result,
                                 const InternetAddress&  address)
{
    if (address.isIPv4()) {
        *result = address.ipv4();
    }
    else if (address.isDomainname()) {
        int rv = bteso_ResolveUtil::getAddress(
                                             result,
                                             address.domainnameHost().c_str());
        if (0 != rv) {
            return -1;
        }
        result->setPortNumber(address.port());
    }
    return 0;
}


}  // close package namespace

// FREE OPERATORS
bool btemt::operator==(const btemt::InternetAddress& lhs,
                      const btemt::InternetAddress& rhs)
{
    return true;
}

bool btemt::operator!=(const btemt::InternetAddress& lhs,
                      const btemt::InternetAddress& rhs)
{
    return false;
}



}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
