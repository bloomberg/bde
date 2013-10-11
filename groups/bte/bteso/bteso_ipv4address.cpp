// bteso_ipv4address.cpp       -*-C++-*-
#include <bteso_ipv4address.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_ipv4address_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_cstdio.h>
#include <bsl_ostream.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
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
    BSLS_ASSERT(address);

    in_addr addr;
    if (!machineIndependentInetPtonIPv4(address, &addr)) {
        return -1;                                                    // RETURN
    }
    else {
        return 0;                                                     // RETURN
    }
}

int bteso_IPv4Address::machineIndependentInetPtonIPv4(const char *address,
                                                      in_addr *addr)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS

    return inet_pton(AF_INET, address, addr) == 1;

#else

    return inet_aton(address, addr);

#endif
}

                            // --------
                            // CREATORS
                            // --------

bteso_IPv4Address::bteso_IPv4Address(const char *address,
                                     int         portNumber)
: d_portNumber(portNumber)
{
    BSLS_ASSERT(address);
    BSLS_ASSERT_SAFE(0 == isValid(address));
    BSLS_ASSERT(0 <= portNumber && portNumber <= 65535);

    in_addr addr;
    machineIndependentInetPtonIPv4(address, &addr);
    d_address = addr.s_addr;
}

                            // ------------
                            // MANIPULATORS
                            // ------------

int bteso_IPv4Address::setIpAddress(const char *address)
{
    BSLS_ASSERT(address);

    in_addr addr;

    if (!machineIndependentInetPtonIPv4(address, &addr))
    {
        d_address = 0;
        return -1;
    }
    else {
        d_address = addr.s_addr;
        return 0;
    }
}

                            // ---------
                            // ACCESSORS
                            // ---------

int bteso_IPv4Address::loadIpAddress(char *result) const
{
    BSLS_ASSERT(result);

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
    BSLS_ASSERT(result);

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
