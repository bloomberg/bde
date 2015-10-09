// btlso_resolveutil.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_resolveutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_resolveutil_cpp,"$Id$ $CSID$")

#include <btlso_ipv4address.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bdlma_bufferedsequentialallocator.h>

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_unordered_set.h>

#ifdef BSLS_PLATFORM_OS_UNIX

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>        // MAXHOSTNAMELEN (ibm and others)
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>           // gethostname()
#include <bsl_c_errno.h>      // errno
#include <netdb.h>            // h_errno and prototype for _AIX, MAXHOSTNAMELEN

#else                         // windows

#include <bsl_cstring.h>      // memcpy
#include <bsl_cstdlib.h>      // atoi()
#include <winsock2.h>         // getservbyname()
#include <ws2tcpip.h>         // getaddrinfo() getnameinfo()
#define MAXHOSTNAMELEN  256

#endif

namespace BloombergLP {

namespace {

                      // ========================
                      // struct IPv4AddressHasher
                      // ========================

struct IPv4AddressHasher {
    // ACCESSOR
    bsl::size_t operator()(const btlso::IPv4Address& x) const
        // Hash the specified 'x'.
    {
        enum { k_SHIFT = (sizeof(bsl::size_t) - sizeof(int)) * 8 };

        return x.ipAddress() +
                         (static_cast<bsl::size_t>(x.portNumber()) << k_SHIFT);
    }
};

}  // close unnamed namespace

#if defined(BSLS_PLATFORM_OS_HPUX)      \
 || defined(BSLS_PLATFORM_OS_CYGWIN)    \
 || defined(BSLS_PLATFORM_OS_DARWIN)
    // The re-entrant function 'getservbyname_r', is not available on these
    // platforms, so we create a local version here.

static
struct servent *getservbyname_r(const char        *name,
                                const char        *proto,
                                struct servent    *result,
                                bsl::vector<char> *buffer)
    // Re-entrant version of 'getservbyname', with a private improvement that
    // it never fails with ERANGE but instead resizes 'buffer' as needed.
    // Return 'result' with all entries filled in upon success, and 0 if
    // 'getservbyname' fails.
{
    static BloombergLP::bslmt::Mutex                         mutex;
    BloombergLP::bslmt::LockGuard<BloombergLP::bslmt::Mutex> lockguard(&mutex);

    struct servent *server = getservbyname(const_cast<char *>(name),
                                           const_cast<char *>(proto));
    if (0 == server) {
        return 0;                                                     // RETURN
    }

    char **alias;
    int    len = 0;

    // Compute size of server data and resize 'buffer' if necessary.

    len += bsl::strlen(server->s_proto) + 1;
    len += bsl::strlen(server->s_name) + 1;
    len += sizeof alias;

    for (alias = server->s_aliases; *alias != 0; ++alias) {
        len += sizeof *alias + bsl::strlen(*alias) + 1;
    }

    if (len > buffer->size()) {
        buffer->reserve(len);  // to force capacity == len,
        buffer->resize(len);   // otherwise the resize may round capacity
                               // to smallest power of two not smaller than len
    }

    // Buffer is large enough; copy server data, and set 'result' pointers.

    char   *sp = &buffer->front() +
                               (1 + alias - server->s_aliases) * sizeof *alias;
    char  **ap = reinterpret_cast<char **>(&buffer->front());

    bsl::strcpy(sp, server->s_name);
    result->s_name = sp;
    sp += bsl::strlen(sp) + 1;

    bsl::strcpy(sp, server->s_proto);
    result->s_proto = sp;
    sp += bsl::strlen(sp) + 1;

    result->s_aliases = ap;
    for (alias = server->s_aliases; *alias != 0; ++alias) {
        bsl::strcpy(sp, *alias);
        *ap++ = sp;
        sp += bsl::strlen(sp) + 1;
    }
    *ap = 0;
    result->s_port = server->s_port;

    return result;
}

#endif

static
int defaultResolveByNameImp(bsl::vector<btlso::IPv4Address> *hostAddresses,
                            const char                      *hostName,
                            int                              numAddresses,
                            int                             *errorCode)
    // Populate the specified vector 'hostAddresses' with the set of IP
    // addresses that refer to the specifed host 'hostName', but only take the
    // the first 'numAddresses' addresses found.  Return 0 on success and a
    // non-zero value otherwise, and set '*errorCode' to the platform-dependent
    // error code encountered if the hostname could not be resolve.  On
    // success, '*errorCode' is not modified.  's_callback_p' is set to this
    // function by default.
{
    BSLS_ASSERT(hostAddresses);
    BSLS_ASSERT(hostName);
    BSLS_ASSERT(0 < numAddresses);

    hostAddresses->clear();

    struct addrinfo *head, *it;

    int rc = getaddrinfo(hostName, 0, 0, &head);
    if (0 != rc) {
        if (errorCode) {

#if defined(BSLS_PLATFORM_OS_WINDOWS)
            *errorCode = WSAGetLastError();
#else
            *errorCode = rc;
#endif
        }

        return -1;                                                    // RETURN
    }

    // We sometimes get redundant copies of the same address, so we need to
    // keep them in a hash table to avoid duplicates, then copy from the hash
    // table to the vector.

    bsl::unordered_set<btlso::IPv4Address, IPv4AddressHasher> addressHT;

    for (it = head;
         it && (int) addressHT.size() < numAddresses;
         it = it->ai_next) {
        if (AF_INET == it->ai_family) {
            // Since it's 'AF_INET', we know we can cast 'it->ai_addr' (which
            // is a 'struct sockaddr *') to a 'struct sockaddr_in *'.

            struct sockaddr_in *sockAddrIn =
                           reinterpret_cast<struct sockaddr_in *>(it->ai_addr);

            // Note that 's_addr' below is in network byte order; but this is
            // OK because the contract for 'btlso::IPv4Address::setIpAddress'
            // specifies that its argument is to be in network byte order.

            btlso::IPv4Address address;
            address.setIpAddress(sockAddrIn->sin_addr.s_addr);

            addressHT.insert(address);
        }

        // AF_INET6, AF_IRDA, AF_BTH, etc. are ignored.
    }

    freeaddrinfo(head);

    hostAddresses->reserve(addressHT.size());
    hostAddresses->insert(hostAddresses->end(),
                          addressHT.begin(),
                          addressHT.end());

    return 0;
}

namespace btlso {
                          // ------------------
                          // struct ResolveUtil
                          // ------------------

ResolveUtil::ResolveByNameCallback s_callback_p = &defaultResolveByNameImp;

// CLASS METHODS
int ResolveUtil::getAddress(IPv4Address *result,
                            const char  *hostName,
                            int         *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostName);

    // Avoid dynamic memory allocation.

    IPv4Address                        stackBuffer;
    bdlma::BufferedSequentialAllocator allocator(
                                        reinterpret_cast<char *>(&stackBuffer),
                                        sizeof stackBuffer);
    bsl::vector<IPv4Address>           buffer(&allocator);

    if (s_callback_p(&buffer, hostName, 1, errorCode) || buffer.size() < 1) {
        return -1;                                                    // RETURN
    }

    result->setIpAddress(buffer.front().ipAddress());
    return 0;
}

int ResolveUtil::getAddressDefault(IPv4Address *result,
                                   const char  *hostName,
                                   int         *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostName);

    // Avoid dynamic memory allocation.

    IPv4Address                        stackBuffer;
    bdlma::BufferedSequentialAllocator allocator(
                                        reinterpret_cast<char *>(&stackBuffer),
                                        sizeof stackBuffer);
    bsl::vector<IPv4Address>           buffer(&allocator);

    if (defaultResolveByNameImp(&buffer,
                                hostName,
                                1,
                                errorCode) || buffer.size() < 1) {
        return -1;                                                    // RETURN
    }

    result->setIpAddress(buffer.front().ipAddress());
    return 0;
}

int ResolveUtil::getAddresses(bsl::vector<IPv4Address> *result,
                              const char               *hostName,
                              int                      *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostName);

    return s_callback_p(result, hostName, INT_MAX, errorCode);
}

int ResolveUtil::getServicePort(IPv4Address *result,
                                const char  *serviceName,
                                const char  *protocol,
                                int         *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(serviceName);

    enum {
        k_BUF_LEN = 64
    };

#if defined(BSLS_PLATFORM_OS_AIX)
    servent      serverEntry;
    servent_data buffer;

    if (getservbyname_r(serviceName, protocol, &serverEntry, &buffer)) {
        // Note: according to contract, errorCode should not be loaded!
        // This sounds like an insane contract, so we keep the code.

        if (errorCode) {
            *errorCode = h_errno;  // or errno ??  Insufficient AIX doc!!!
        }
        return -1;                                                    // RETURN
    }

    result->setPortNumber(serverEntry.s_port);

#elif defined(BSLS_PLATFORM_OS_UNIX)
   // BSLS_PLATFORM_OS_SOLARIS || BSLS_PLATFORM_OS_LINUX ||
   // BSLS_PLATFORM_OS_CYGWIN || BSLS_PLATFORM_OS_HPUX

    servent serverEntry;

    // Avoid dynamic memory allocation in most common cases.

    char                               stackBuffer[k_BUF_LEN];
    int                                bufferLength = sizeof stackBuffer;
    bdlma::BufferedSequentialAllocator allocator(stackBuffer, bufferLength);

    bsl::vector<char>                  buffer(bufferLength, '\0', &allocator);

    while (1) {
        #if defined(BSLS_PLATFORM_OS_LINUX) \
         || defined(BSLS_PLATFORM_OS_FREEBSD)
        struct servent *glibcResult;

        #if defined(BSLS_PLATFORM_OS_FREEBSD)
        int getServRet;
        #endif
        if ((
        #if defined(BSLS_PLATFORM_OS_FREEBSD)
          getServRet =
        #endif
            getservbyname_r(serviceName,
                            protocol,
                            &serverEntry,
                            &buffer.front(),
                            bufferLength,
                            &glibcResult)) || !glibcResult)
        #elif defined(BSLS_PLATFORM_OS_HPUX)   \
           || defined(BSLS_PLATFORM_OS_CYGWIN) \
           || defined(BSLS_PLATFORM_OS_DARWIN)
        if (0 == getservbyname_r(serviceName,
                                 protocol,
                                 &serverEntry,
                                 &buffer))
        #else
        if (0 == getservbyname_r(serviceName,
                                 protocol,
                                 &serverEntry,
                                 &buffer.front(),
                                 bufferLength))
        #endif
        {
            #if defined(BSLS_PLATFORM_OS_FREEBSD)
              errno = getServRet;
            #endif
            if (errno == ERANGE) {
                // The buffer size is too small; not an error, retry.

                bufferLength *= 4;
                buffer.resize(bufferLength);
                errno = 0;
            }
            else {
                // Other errors: optionally set 'errorCode'.

                if (errorCode) {
                    *errorCode = h_errno;
                }
                return -1;                                            // RETURN
            }
        }
        else {
            // Note that 'getservbyname' returns the port number
            // in network byte order.

            result->setPortNumber(ntohs(static_cast<short>(
                                                         serverEntry.s_port)));
            return 0;                                                 // RETURN
        }
    }

#elif defined(BSLS_PLATFORM_OS_WINDOWS)
        servent *sp = getservbyname(serviceName, protocol);
        if (0 == sp) {
            if (errorCode) {
                *errorCode = WSAGetLastError();
            }
            return -1;                                                // RETURN
        }

        result->setPortNumber(sp->s_port);
#else

#error getServicePort does not handle current platform type!

#endif

    return 0;
}

int ResolveUtil::getHostnameByAddress(bsl::string        *canonicalHostname,
                                      const IPv4Address&  address,
                                      int                *errorCode)
{
    BSLS_ASSERT(canonicalHostname);

    struct hostent *hp = NULL;
    unsigned int    addr = address.ipAddress();   // in network order

#if defined(BSLS_PLATFORM_OS_AIX)
    struct hostent      hent;
    struct hostent_data hdt;

    if (gethostbyaddr_r(reinterpret_cast<char *>(&addr),
                        sizeof (struct in_addr),
                        AF_INET,
                        &hent,
                        &hdt)) {
        if (errorCode) {
            *errorCode = h_errno;
        }
        return -1;                                                    // RETURN
    }

    *canonicalHostname = hent.h_name;

#elif defined(BSLS_PLATFORM_OS_SUNOS) || defined(BSLS_PLATFORM_OS_SOLARIS)
    struct hostent hent;
    char           hdt[2048];
    int            err;

    hp = gethostbyaddr_r(reinterpret_cast<char *>(&addr),
                         sizeof (struct in_addr),
                         AF_INET,
                         &hent,
                         hdt,
                         sizeof(hdt),
                         &err);

    if (0 == hp) {
        if (errorCode) {
            *errorCode = err;
        }
        return -1;                                                    // RETURN
    }

    *canonicalHostname = hp->h_name;

#elif defined(BSLS_PLATFORM_OS_LINUX) \
   || defined(BSLS_PLATFORM_OS_FREEBSD)
    struct hostent hent;
    char           hdt[2048];
    int            err;

    if (gethostbyaddr_r(reinterpret_cast<char *>(&addr),
                        sizeof (struct in_addr),
                        AF_INET,
                        &hent,
                        hdt,
                        sizeof(hdt),
                        &hp,
                        &err) || 0 == hp) {
        if (errorCode) {
            *errorCode = err;
        }
        return -1;                                                    // RETURN
    }

    *canonicalHostname = hp->h_name;

#elif defined(BSLS_PLATFORM_OS_UNIX)
    // Standard call cannot be assumed to be re-entrant (it often is not).
    {
        static bslmt::Mutex            mutex;
        bslmt::LockGuard<bslmt::Mutex> guard(&mutex);

        hp = gethostbyaddr(reinterpret_cast<char *>(&addr),
                           sizeof (struct in_addr),
                           AF_INET);

        if (0 == hp) {
            if (errorCode) {
#ifdef BSLS_PLATFORM_OS_HPUX
                *errorCode = h_errno;
#else
                *errorCode = errno;
#endif
            }
            return -1;                                                // RETURN
        }

        *canonicalHostname = hp->h_name;
    }

#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    unsigned short port = address.portNumber();  // in host order

    struct sockaddr_in saGNI;
    char               hostName[NI_MAXHOST];
    char               servInfo[NI_MAXSERV];

    saGNI.sin_family = AF_INET;
    saGNI.sin_addr.s_addr = addr;
    saGNI.sin_port = htons(port);

    if (getnameinfo(reinterpret_cast<SOCKADDR *>(&saGNI),
                    sizeof(sockaddr),
                    hostName,
                    sizeof(hostName),
                    servInfo,
                    sizeof(servInfo),
                    NI_NUMERICSERV|NI_NAMEREQD)) {
        if (errorCode) {
            *errorCode = WSAGetLastError();
        }
        return -1;                                                    // RETURN
    }

    *canonicalHostname = hostName; // a Fully Qualified Domain Name (FQDN)

#else

#error getHostnameByAddress does not handle current platform type!

#endif

    return 0;
}

int ResolveUtil::getLocalHostname(bsl::string *result)
{
    BSLS_ASSERT(result);

    char buf[MAXHOSTNAMELEN + 1];
    bsl::memset(buf, 0, sizeof(buf));

    int rc = gethostname(buf, sizeof(buf));

    if (0 == rc) {
        result->assign(buf);
    }
    return rc;
}

ResolveUtil::ResolveByNameCallback
ResolveUtil::setResolveByNameCallback(ResolveByNameCallback callback)
{
    ResolveByNameCallback previousCallback = s_callback_p;
    s_callback_p = callback;
    return previousCallback;
}

ResolveUtil::ResolveByNameCallback
ResolveUtil::currentResolveByNameCallback()
{
    return s_callback_p;
}

ResolveUtil::ResolveByNameCallback
ResolveUtil::defaultResolveByNameCallback()
{
    return &defaultResolveByNameImp;
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
