// bteso_ipv4address.cpp       -*-C++-*-
#include <bteso_ipv4address.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_ipv4address_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_cstdio.h>
#include <bsl_ostream.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <mstcpip.h>
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

    int addr;
    if (!machineIndependentInetPtonIPv4(address, &addr)) {
        return -1;                                                    // RETURN
    }
    else {
        return 0;                                                     // RETURN
    }
}

int bteso_IPv4Address::windowsCheckMinusOneAddr(const char *address) {
    int num_dot_char = 0;
    int dot_char[3];

    for (int i = 0; address[i] != 0; i++)
    {
        if (address[i] == '.')
        {
            if (num_dot_char == 3)
            {
                return 0;  // Can't have more than three dots.
            }
            else {
                dot_char[num_dot_char++] = i;
            }
        }
    }

    // Compute the position and length of each numeral in the IP address.
    const char *pos[4];
    int length[4];
    for (int i = 0; i <= num_dot_char; i++) {
        if (i == 0)
        {
            pos[i]    = address;
            length[i] = dot_char[0];
        } else if (i == num_dot_char)
        {
            pos[i]    = address + dot_char[i - 1] + 1;
            length[i] = strlen(address) - dot_char[i - 1];
        } else
        {
            pos[i]    = address + dot_char[i - 1] + 1;
            length[i] = dot_char[i] - dot_char[i - 1] - 1;
        }
    }

    switch (num_dot_char)
    {
        // format a
        case 0:
            return (!strcmp     (address, "-1")) ||
                   (!strcmp     (address, "4294967295")) ||
                   (!strcasecmp (address, "0xFFFFFFFF")) ||
                   (!strcmp     (address, "037777777777"));
        // format a.b
        case 1:
            return ((!strncmp    (pos[0], "-1",        length[0])) ||
                    (!strncmp    (pos[0], "255",       length[0])) ||
                    (!strncasecmp(pos[0], "0xFF",      length[0])) ||
                    (!strncmp    (pos[0], "0377",      length[0]))) &&
                   ((!strncmp    (pos[1], "-1",        length[1])) ||
                    (!strncmp    (pos[1], "16777215",  length[1])) ||
                    (!strncasecmp(pos[1], "0xFFFFFF",  length[1])) ||
                    (!strncmp    (pos[1], "077777777", length[1])));
        // format a.b.c
        case 2:
            return ((!strncmp    (pos[0], "-1",        length[0])) ||
                    (!strncmp    (pos[0], "255",       length[0])) ||
                    (!strncasecmp(pos[0], "0xFF",      length[0])) ||
                    (!strncmp    (pos[0], "0377",      length[0]))) &&
                   ((!strncmp    (pos[1], "-1",        length[1])) ||
                    (!strncmp    (pos[1], "255",       length[1])) ||
                    (!strncasecmp(pos[1], "0xFF",      length[1])) ||
                    (!strncmp    (pos[1], "0377",      length[1]))) &&
                   ((!strncmp    (pos[2], "-1",        length[2])) ||
                    (!strncmp    (pos[2], "65535",     length[2])) ||
                    (!strncasecmp(pos[2], "0xFFFF",    length[2])) ||
                    (!strncmp    (pos[2], "0177777",   length[2])));
        // format a.b.c.d
        case 3:
            return ((!strncmp    (pos[0], "-1",        length[0])) ||
                    (!strncmp    (pos[0], "255",       length[0])) ||
                    (!strncasecmp(pos[0], "0xFF",      length[0])) ||
                    (!strncmp    (pos[0], "0377",      length[0]))) &&
                   ((!strncmp    (pos[1], "-1",        length[1])) ||
                    (!strncmp    (pos[1], "255",       length[1])) ||
                    (!strncasecmp(pos[1], "0xFF",      length[1])) ||
                    (!strncmp    (pos[1], "0377",      length[1]))) &&
                   ((!strncmp    (pos[2], "-1",        length[2])) ||
                    (!strncmp    (pos[2], "255",       length[2])) ||
                    (!strncasecmp(pos[2], "0xFF",      length[2])) ||
                    (!strncmp    (pos[2], "0377",      length[2]))) &&
                   ((!strncmp    (pos[3], "-1",        length[3])) ||
                    (!strncmp    (pos[3], "255",       length[3])) ||
                    (!strncasecmp(pos[3], "0xFF",      length[3])) ||
                    (!strncmp    (pos[3], "0377",      length[3])));
    }
}

int bteso_IPv4Address::machineIndependentInetPtonIPv4(const char *address,
                                                      int        *addr)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS

    *addr = inet_addr(address);
    if (addr != -1) {
        return 1;
    }
    else {
        if (windowsCheckMinusOneAddr(address)) {
            *addr = -1;
            return 1;
        }
        return 0;
    }

#else

    in_addr inaddr;
    int errorcode = inet_aton(address, &inaddr);
    *addr = inaddr.s_addr;
    return errorcode;

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

    machineIndependentInetPtonIPv4(address, &d_address);
}

                            // ------------
                            // MANIPULATORS
                            // ------------

int bteso_IPv4Address::setIpAddress(const char *address)
{
    BSLS_ASSERT(address);

    int addr;

    if (!machineIndependentInetPtonIPv4(address, &addr))
    {
        d_address = 0;
        return -1;
    }
    else {
        d_address = addr;
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
