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

int bteso_IPv4Address::isLocalBroadcastAddress(const char *address)
{
    int numDotChars = 0;
    const char *dotCharPtr[3];

    for (int i = 0; address[i] != 0; i++)
    {
        if (address[i] == '.')
        {
            if (numDotChars == 3)
            {
                // Can't have more than three dots.

                return 0;                                             // RETURN
            }
            else {
                dotCharPtr[numDotChars++] = address + i;
            }
        }
    }

    // Check that all but the last number represents 255.

    for (int i = 0; i < numDotChars; i++)
    {
        const char *start = (i == 0 ? address : (dotCharPtr[i - 1] + 1));

        // Should be 255 in some format.

        if (strncmp    (start, "-1.",   3) &&
            strncmp    (start, "255.",  4) &&
            strncasecmp(start, "0xFF.", 5) &&
            strncmp    (start, "0377.", 5)) {
            return 0;                                                 // RETURN
        }
    }

    switch(numDotChars)
    {
      // format a

      case 0:
        // The last number should represent 4294967295.

        return (!strcmp    (address,           "-1"          )) ||
               (!strcmp    (address,           "4294967295"  )) ||
               (!strcasecmp(address,           "0xFFFFFFFF"  )) ||
               (!strcmp    (address,           "037777777777"));      // RETURN

      // format a.b

      case 1:
        // The last number should represent 16777215.

        return (!strcmp    (dotCharPtr[0] + 1, "-1"          )) ||
               (!strcmp    (dotCharPtr[0] + 1, "16777215"    )) ||
               (!strcasecmp(dotCharPtr[0] + 1, "0xFFFFFF"    )) ||
               (!strcmp    (dotCharPtr[0] + 1, "077777777"   ));      // RETURN

      // format a.b.c

      case 2:

        // The last number should represent 65535.

        return (!strcmp    (dotCharPtr[1] + 1, "-1"          )) ||
               (!strcmp    (dotCharPtr[1] + 1, "65535"       )) ||
               (!strcasecmp(dotCharPtr[1] + 1, "0xFFFF"      )) ||
               (!strcmp    (dotCharPtr[1] + 1, "0177777"     ));      // RETURN

      // format a.b.c.d

      case 3:

        // The last number should represent 255.

        return (!strcmp    (dotCharPtr[2] + 1, "-1"          )) ||
               (!strcmp    (dotCharPtr[2] + 1, "255"         )) ||
               (!strcasecmp(dotCharPtr[2] + 1, "0xFF"        )) ||
               (!strcmp    (dotCharPtr[2] + 1, "0377"        ));      // RETURN
      default:

        // Should not get here, but added to suppress warnings.

        return 0;                                                     // RETURN
  }
}

int bteso_IPv4Address::machineIndependentInetPtonIPv4(const char *address,
                                                      int        *addr)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS

    *addr = inet_addr(address);
    if (addr != -1) {
        return 1;                                                     // RETURN
    }
    else {
        if (isLocalBroadcastAddress(address)) {
            *addr = -1;
            return 1;                                                 // RETURN
        }
        return 0;                                                     // RETURN
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
        return -1;                                                    // RETURN
    }
    else {
        d_address = addr;
        return 0;                                                     // RETURN
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
