// bteso_ipv4address.cpp       -*-C++-*-
#include <bteso_ipv4address.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_ipv4address_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>
#include <bsl_cstdio.h>

#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

                       // =======================
                       // class bteso_IPv4Address
                       // =======================

                            // -------------
                            // CLASS METHODS
                            // -------------

namespace BloombergLP {

int bteso_IPv4Address::isValid(const char *address)
{
    int addr = inet_addr(address);
    if (-1 == addr) return -1;
    else            return  0;
}

                            // --------
                            // CREATORS
                            // --------

bteso_IPv4Address::bteso_IPv4Address(const char *address,
                                     int         portNumber)
: d_portNumber(portNumber)
{
    d_address = inet_addr(address);
}

                            // ------------
                            // MANIPULATORS
                            // ------------

int bteso_IPv4Address::setIpAddress(const char *address)
{
    d_address = inet_addr(address);
    if (-1 == d_address) return -1;
    else                 return  0;
}

                            // ---------
                            // ACCESSORS
                            // ---------

int bteso_IPv4Address::loadIpAddress(char *result) const
{
    const unsigned char *ip = (const unsigned char *) &d_address;
    return bsl::sprintf(result,
                 "%d.%d.%d.%d",
                 (int) ip[0],
                 (int) ip[1],
                 (int) ip[2],
                 (int) ip[3]) + 1;  // +1 for null terminated char.
}

int bteso_IPv4Address::formatIpAddress(char *result) const
{
    const unsigned char *ip = (const unsigned char *) &d_address;
    return bsl::sprintf(result,
                 "%d.%d.%d.%d:%d",
                 (int) ip[0],
                 (int) ip[1],
                 (int) ip[2],
                 (int) ip[3],
                 d_portNumber) + 1; // +1 for null terminated char.
}

bsl::ostream& bteso_IPv4Address::streamOut(bsl::ostream& stream) const
{
    int ip = ipAddress();
    unsigned char *ipp = (unsigned char *) &ip;
    stream << (int) ipp[0] << '.'
           << (int) ipp[1] << '.'
           << (int) ipp[2] << '.'
           << (int) ipp[3] << ':'
           << portNumber();

    return stream;
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
