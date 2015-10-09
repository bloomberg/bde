// btlso_ipv4address.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_ipv4address.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_ipv4address_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_cctype.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_ostream.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <mstcpip.h>
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

namespace BloombergLP {
namespace btlso {

                       // -----------------
                       // class IPv4Address
                       // -----------------


// PRIVATE CLASS METHODS

int IPv4Address::machineIndependentInetPtonIPv4(int *addr, const char *address)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS

    *addr = inet_addr(address);
    if (-1 != *addr) {
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
    int     errorcode = inet_aton(address, &inaddr);

    *addr = inaddr.s_addr;

    return errorcode;

#endif
}

// CLASS METHODS
int IPv4Address::isLocalBroadcastAddress(const char *addr)
    // Windows XP currently does not support the inet_aton function as
    // specified by the contract above (inet_pton does not handle hexadecimal
    // or octal numerals.)  Note that 255.255.255.255, while being a valid
    // address, is not parsed correctly by inet_addr because -1 is used as an
    // error code.  This function checks if the specified 'address' is an IP
    // representation of a address with an integer value of -1.  This function
    // is intended to detect all cases in which a valid address of
    // 255.255.255.255 is wrongfully detected as an invalid address by
    // inet_addr.
{
    unsigned long segs[4] = { 0, 0, 0, 0 };
    int           numSeg = 0;

    for (int i = 0; i < 4; ++i) {
        if (!bsl::isdigit(*addr)) {
            return false;                                             // RETURN
        }

        // Consume one numerical token.

        segs[numSeg++] = bsl::strtoul(addr, const_cast<char **>(&addr), 0);

        if (0 == *addr) {
            break;
        }

        if (*addr++ != "..."[i]) {
            return false;                                             // RETURN
        }
    }

    // The four binary representations of -1.
    static const unsigned long minusOne[4][4] = {
                                  {0xFFFFFFFFul, 0,          0,        0     },
                                  {0xFFul,       0xFFFFFFul, 0,        0     },
                                  {0xFFul,       0xFFul,     0xFFFFul, 0     },
                                  {0xFFul,       0xFFul,     0xFFul,   0xFFul},
    };
    return bsl::memcmp(segs, minusOne[numSeg - 1], sizeof segs) == 0;
}

// CREATORS
IPv4Address::IPv4Address(const char *address, int portNumber)
: d_portNumber(static_cast<unsigned short>(portNumber))
{
    BSLS_ASSERT(address);
    BSLS_ASSERT_SAFE(isValidAddress(address));
    BSLS_ASSERT(0 <= portNumber && portNumber <= 65535);

    machineIndependentInetPtonIPv4(&d_address, address);
}

// MANIPULATORS
int IPv4Address::setIpAddress(const char *address)
{
    BSLS_ASSERT(address);

    int addr;

    if (!machineIndependentInetPtonIPv4(&addr, address)) {
        d_address = 0;
        return -1;                                                    // RETURN
    }
    d_address = addr;
    return 0;
}

// ACCESSORS
int IPv4Address::loadIpAddress(char *result) const
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

int IPv4Address::formatIpAddress(char *result) const
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

bsl::ostream& IPv4Address::streamOut(bsl::ostream& stream) const
{
    char ipAddressBuffer[22];

    formatIpAddress(ipAddressBuffer);

    stream << ipAddressBuffer;

    return stream;
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
