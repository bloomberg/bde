// bteso_resolveutil.cpp        -*-C++-*-
#include <bteso_resolveutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_resolveutil_cpp,"$Id$ $CSID$")

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <bteso_sockethandle.h>                 // for testing only
#include <bteso_socketimputil.h>                // for testing only
#endif

#include <bteso_ipv4address.h>

#include <bdema_bufferedsequentialallocator.h>

#include <bcemt_thread.h>
#include <bcemt_lockguard.h>

#include <bslma_default.h>
#include <bsls_platform.h>

#include <bsls_assert.h>

#ifdef BSLS_PLATFORM__OS_UNIX

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>  // MAXHOSTNAMELEN (ibm and others)
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>     // gethostname()
#include <bsl_c_errno.h>      // errno
#include <netdb.h>      // h_errno and prototype for _AIX, MAXHOSTNAMELEN

#else                   // windows

#include <bsl_cstring.h>      // memcpy
#include <bsl_cstdlib.h>      // atoi()
#include <winsock2.h>   // getservbyname()
#include <ws2tcpip.h>   // getaddrinfo() getnameinfo()
#define MAXHOSTNAMELEN  256

#endif

namespace BloombergLP {

namespace {

#if defined(BSLS_PLATFORM__OS_HPUX)         \
    || defined(BSLS_PLATFORM__OS_CYGWIN)    \
    || defined(BSLS_PLATFORM__OS_DARWIN)
    // The re-entrant functions 'gethostbyname_r' and 'getservbyname_r',
    // are not available on these platforms, so we create local
    // versions here.

                       // ==============================
                       // local function gethostbyname_r
                       // ==============================

struct hostent *gethostbyname_r(
        const char        *name,
        struct hostent    *result,
        bsl::vector<char> *buffer,    // private improvement
        int               *h_errnop)
    // Re-entrant version of 'gethostbyname', with a custom improvement that it
    // never fails with ERANGE but instead resizes 'buffer' as needed.  If a
    // failure occurs, return NULL, and set the integer pointer to by h_errnop
    // to one of HOST_NOT_FOUND, TRY_AGAIN, NO_RECOVERY, NO_DATA, and
    // NO_ADDRESS.  Otherwise, return 'result' with all entries filled in.
{
    static BloombergLP::bcemt_Mutex mutex;
    BloombergLP::bcemt_LockGuard<BloombergLP::bcemt_Mutex> lockguard(&mutex);

    struct hostent *host = gethostbyname(name);
    if (host == 0) {
        *h_errnop = h_errno;
        return 0;
    }

    char  **alias;
    char  **addr;
    int     len = 0;

    // Compute size of host data and resize 'buffer' if necessary.

    len += bsl::strlen(host->h_name) + 1;
    len += sizeof alias;
    for (alias = host->h_aliases; *alias != 0; ++alias) {
        len += sizeof *alias + bsl::strlen(*alias) + 1;
    }
    len += sizeof addr;
    for (addr = host->h_addr_list; *addr != 0; ++addr) {
        len += sizeof *addr + host->h_length;
    }

    if (len > buffer->size()) {
        buffer->reserve(len);  // to force capacity == len,
        buffer->resize(len);   // otherwise the resize may round capacity
                               // to smallest power of two not smaller than len
    }

    // Buffer is large enough; copy host data, and set 'result' pointers.

    int     aliasLength = (1 + alias - host->h_aliases) * sizeof *alias;
    int     addrLength  = (1 + addr - host->h_addr_list) * sizeof *addr;
    char   *sp = &buffer->front() + aliasLength + addrLength;
    char  **ap = reinterpret_cast<char **>(&buffer->front());

    bsl::strcpy(sp, host->h_name);
    result->h_name = sp;
    sp += bsl::strlen(sp) + 1;

    result->h_aliases = ap;
    for (alias = host->h_aliases; *alias != 0; ++alias) {
        bsl::strcpy(sp, *alias);
        *ap++ = sp;
        sp += bsl::strlen(sp) + 1;
    }
    *ap++ = 0;

    result->h_addr_list = ap;
    for (addr = host->h_addr_list; *addr != 0; ++addr) {
        bsl::memcpy(sp, *addr, host->h_length);
        *ap++ = sp;
        sp += host->h_length + 1;
    }
    *ap++ = 0;
    result->h_addrtype = host->h_addrtype;
    result->h_length   = host->h_length;

    return result;
}

                       // ==============================
                       // local function getservbyname_r
                       // ==============================

struct servent *getservbyname_r(const char        *name,
                                const char        *proto,
                                struct servent    *result,
                                bsl::vector<char> *buffer)
    // Re-entrant version of 'getservbyname', with a private improvement that
    // it never fails with ERANGE but instead resizes 'buffer' as needed.
    // Return 'result' with all entries filled in upon success, and 0 if
    // 'getservbyname' fails.
{
    static BloombergLP::bcemt_Mutex mutex;
    BloombergLP::bcemt_LockGuard<BloombergLP::bcemt_Mutex> lockguard(&mutex);

    struct servent *server = getservbyname((char *)name, (char *)proto);
    if (server == 0) {
        return 0;
    }

    char  **alias;
    int     len = 0;

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

                // ======================================
                // local function defaultResolveByNameImp
                // ======================================

int defaultResolveByNameImp(bsl::vector<bteso_IPv4Address> *hostAddresses,
                            const char                     *hostName,
                            int                             numAddresses,
                            int                            *errorCode)
{
    BSLS_ASSERT(hostAddresses);
    BSLS_ASSERT(hostName);
    BSLS_ASSERT(0 < numAddresses);

    enum {
        // For some reason, when you call the resolving functions on Linux with
        // a buffer of 256 bytes, you might get an ENOENT error instead of an
        // ERANGE.  This is bug and it has been reported at
        //    http://sources.redhat.com/bugzilla/show_bug.cgi?id=515
        // Setting the size to 512 or higher makes it work, though.

        BUF_LEN = 1024   // slightly higher, if expected more than one answer
    };

    hostAddresses->clear();

#if defined(BSLS_PLATFORM__OS_UNIX)
    struct hostent hostEntry;

#if defined(BSLS_PLATFORM__OS_AIX)
    // On AIX, we have a fixed buffer length for 'gethostbyname_r',
    // equal to 'sizeof(hostent_data)'.  It either suffices or fails.

    hostent_data buffer;

    if (gethostbyname_r(hostName, &hostEntry, &buffer)) {
        if (errorCode) {
            *errorCode = h_errno;
        }
        return -1;
    }
#else // BSLS_PLATFORM__OS_SOLARIS || BSLS_PLATFORM__OS_LINUX ||
      // BSLS_PLATFORM__OS_CYGWIN || BSLS_PLATFORM__OS_HPUX

    // On Solaris and Linux, we have a choice of buffer length.  We start with
    // default length, and increase it as necessary.  On Cygwin, we call
    // 'gethostbyname' protected by a mutex, and resize our buffer as necessary
    // to guarantee success.

    // Avoid dynamic memory allocation in most common cases.

    char stackBuffer[BUF_LEN];
    int bufferLength = sizeof stackBuffer;
    bdema_BufferedSequentialAllocator allocator(stackBuffer, bufferLength);
    bsl::vector<char> buffer(bufferLength, '\0', &allocator);

    int errCode = 0;
    while (1) {
        #if defined(BSLS_PLATFORM__OS_LINUX) \
         || defined(BDES_PLATFORM__OS_FREEBSD)
        struct hostent *result;
        if (gethostbyname_r(hostName,
                            &hostEntry,
                            &buffer.front(),
                            bufferLength,
                            &result,
                            &errCode) || !result) {
        #elif defined(BSLS_PLATFORM__OS_HPUX)   \
           || defined(BSLS_PLATFORM__OS_CYGWIN) \
           || defined(BSLS_PLATFORM__OS_DARWIN)
        if (0 == gethostbyname_r(hostName,
                                 &hostEntry,
                                 &buffer,  // private improvement
                                 &errCode)) {
        #else
        if (0 == gethostbyname_r(hostName,
                                 &hostEntry,
                                 &buffer.front(),
                                 bufferLength,
                                 &errCode)) {
        #endif
            if (errno == ERANGE) {    // The buffer size is too small.
                // This is never reached on FreeBSD.  The API is pure garbage.
                // There is no way to know that the buffer is too small.

                bufferLength *= 4;
                buffer.resize(bufferLength);
                errno = 0;
            }
            else {
                if (errorCode) {
                    *errorCode = errCode;
                }
                return -1;
            }
        }
        else {
            break;
        }
    }
#endif

    // Note that 'hostEntry' contains pointers into 'buffer', defined
    // either as 'struct hostent_data' on AIX, or 'bsl::vector<char>'
    // otherwise.  However, the definition of 'buffer' does not matter, but
    // the lifetime of 'buffer' must extend beyond that of 'hostEntry'.

    for (int i = 0; i < numAddresses && hostEntry.h_addr_list[i]; ++i) {
        if (AF_INET == hostEntry.h_addrtype) {
            // Set address to buffer if four bytes pointed to by
            // 'hostEntry.h_addr_list[i]', reinterpreted as 'int' for
            // 'bteso_IPv4Address' (note that that buffer is not
            // necessarily aligned as int, hence copy into temporary).

            int address;  // in network byte order
            bsl::memcpy(&address, hostEntry.h_addr_list[i], sizeof(int));

            hostAddresses->resize(i + 1);
            hostAddresses->back().setIpAddress(address);
        } else {
            // AF_INET6 not supported.

            return -1;
        }
    }

#elif defined(BSLS_PLATFORM__OS_WINDOWS)
    // On Windows, we resort to 'getaddrinfo' because...
    // [Why??? Is gethostbyname cheaper on Unix but not on Windows???]

    addrinfo *buffer;
    if (getaddrinfo(hostName, 0, 0, &buffer)) {
        if (errorCode) {
            *errorCode = WSAGetLastError();
        }
        return -1;
    }

    addrinfo *iter = buffer;
    for (int i = 0; i < numAddresses && iter; iter = iter->ai_next) {
        (*hostAddresses).resize(i + 1);
        sockaddr_in *addr_in = (sockaddr_in*)(iter->ai_addr);
        if (AF_INET == addr_in->sin_family) {
            hostAddresses->back().setIpAddress(addr_in->sin_addr.s_addr);
            ++i;
        }
        else {
            // AF_INET6, AF_IRDA, AF_BTH, etc. not supported.
        }

    }
    freeaddrinfo(buffer);

#else

#error defaultResolveByNameCallback does not handle current platform type!

#endif

    return 0;
}

                          // ========================
                          // static data s_callback_p
                          // ========================

bteso_ResolveUtil::ResolveByNameCallback s_callback_p =
                                                      &defaultResolveByNameImp;

}  // closing unnamed namespace

                          // ------------------------
                          // struct bteso_ResolveUtil
                          // ------------------------

// CLASS METHODS
int bteso_ResolveUtil::getAddress(bteso_IPv4Address *result,
                                  const char        *hostName,
                                  int               *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostName);

    // Avoid dynamic memory allocation.

    bteso_IPv4Address stackBuffer;
    bdema_BufferedSequentialAllocator allocator((char *)&stackBuffer,
                                                sizeof stackBuffer);
    bsl::vector<bteso_IPv4Address> buffer(&allocator);

    if (s_callback_p(&buffer, hostName, 1, errorCode) || buffer.size() < 1) {
        return -1;
    }

    result->setIpAddress(buffer.front().ipAddress());
    return 0;
}

int bteso_ResolveUtil::getAddressDefault(bteso_IPv4Address *result,
                                         const char        *hostName,
                                         int               *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostName);

    // Avoid dynamic memory allocation.

    bteso_IPv4Address stackBuffer;
    bdema_BufferedSequentialAllocator allocator((char *)&stackBuffer,
                                                sizeof stackBuffer);
    bsl::vector<bteso_IPv4Address> buffer(&allocator);

    if (defaultResolveByNameImp(&buffer,
                                hostName,
                                1,
                                errorCode) || buffer.size() < 1) {
        return -1;
    }

    result->setIpAddress(buffer.front().ipAddress());
    return 0;
}

int bteso_ResolveUtil::getAddresses(bsl::vector<bteso_IPv4Address> *result,
                                    const char                     *hostName,
                                    int                            *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostName);

    return s_callback_p(result, hostName, INT_MAX, errorCode);
}

int bteso_ResolveUtil::getServicePort(bteso_IPv4Address *result,
                                      const char        *serviceName,
                                      const char        *protocol,
                                      int               *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(serviceName);

    enum {
        BUF_LEN = 64
    };

#if defined(BSLS_PLATFORM__OS_AIX)
    servent serverEntry;
    servent_data buffer;

    if (getservbyname_r(serviceName,
                        protocol,
                        &serverEntry,
                        &buffer)) {
        // Note: according to contract, errorCode should not be loaded!
        // This sounds like an insane contract, so we keep the code.

        if (errorCode) {
            *errorCode = h_errno;  // or errno ??  Insufficient AIX doc!!!
        }
        return -1;
    }

    result->setPortNumber(serverEntry.s_port);

#elif defined(BSLS_PLATFORM__OS_UNIX)
   // BSLS_PLATFORM__OS_SOLARIS || BSLS_PLATFORM__OS_LINUX ||
   // BSLS_PLATFORM__OS_CYGWIN || BSLS_PLATFORM__OS_HPUX

    servent serverEntry;

    // Avoid dynamic memory allocation in most common cases.

    char stackBuffer[BUF_LEN];
    int bufferLength = sizeof stackBuffer;
    bdema_BufferedSequentialAllocator allocator(stackBuffer, bufferLength);

    bsl::vector<char> buffer(bufferLength, '\0', &allocator);

    while (1) {
        #if defined(BSLS_PLATFORM__OS_LINUX) \
         || defined(BDES_PLATFORM__OS_FREEBSD)
        struct servent *glibcResult;

        #if defined(BDES_PLATFORM__OS_FREEBSD)
        int getServRet;
        #endif
        if ((
        #if defined(BDES_PLATFORM__OS_FREEBSD)
          getServRet =
        #endif
            getservbyname_r(serviceName,
                            protocol,
                            &serverEntry,
                            &buffer.front(),
                            bufferLength,
                            &glibcResult)) || !glibcResult)
        #elif defined(BSLS_PLATFORM__OS_HPUX)   \
           || defined(BSLS_PLATFORM__OS_CYGWIN) \
           || defined(BSLS_PLATFORM__OS_DARWIN)
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
            #if defined(BDES_PLATFORM__OS_FREEBSD)
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
                return -1;
            }
        }
        else {
            // Note that 'getservbyname' returns the port number
            // in network byte order.

            result->setPortNumber(ntohs(serverEntry.s_port));
            return 0;
        }
    }

#elif defined(BSLS_PLATFORM__OS_WINDOWS)
        servent *sp = getservbyname(serviceName, protocol);
        if (0 == sp) {
            if (errorCode) {
                *errorCode = WSAGetLastError();
            }
            return -1;
        }

        result->setPortNumber(sp->s_port);
#else

#error getServicePort does not handle current platform type!

#endif

    return 0;
}

int bteso_ResolveUtil::getHostnameByAddress(
                                   bsl::string*              canonicalHostname,
                                   const bteso_IPv4Address&  address,
                                   int                      *errorCode)
{
    BSLS_ASSERT(canonicalHostname);

    struct hostent *hp = NULL;

    unsigned int addr   = address.ipAddress();   // in network order
#if defined(BSLS_PLATFORM__OS_AIX)
    struct hostent hent;
    struct hostent_data hdt;

    if (gethostbyaddr_r((char *)&addr,
                        sizeof (struct in_addr),
                        AF_INET,
                        &hent,
                        &hdt)) {
        if (errorCode) {
            *errorCode = h_errno;
        }
        return -1;
    }

    *canonicalHostname = hent.h_name;

#elif defined(BSLS_PLATFORM__OS_SUNOS) || defined(BSLS_PLATFORM__OS_SOLARIS)
    struct hostent hent;
    char hdt[2048];
    int err;

    hp = gethostbyaddr_r((char *)&addr,
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
        return -1;
    }

    *canonicalHostname = hp->h_name;

#elif defined(BSLS_PLATFORM__OS_LINUX) \
   || defined(BDES_PLATFORM__OS_FREEBSD)
    struct hostent hent;
    char hdt[2048];
    int err;

    if (gethostbyaddr_r((char *)&addr,
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
        return -1;
    }

    *canonicalHostname = hp->h_name;

#elif defined(BSLS_PLATFORM__OS_UNIX)
    // Standard call cannot be assumed to be re-entrant (it often is not).
    {
        static bcemt_Mutex mutex;
        bcemt_LockGuard<bcemt_Mutex> guard(&mutex);

        hp = gethostbyaddr((char *)&addr,
                           sizeof (struct in_addr),
                           AF_INET);

        if (0 == hp) {
            if (errorCode) {
#ifdef BSLS_PLATFORM__OS_HPUX
                *errorCode = h_errno;
#else
                *errorCode = errno;
#endif
            }
            return -1;
        }

        *canonicalHostname = hp->h_name;
    }

#elif defined(BSLS_PLATFORM__OS_WINDOWS)
    unsigned short port = address.portNumber();  // in host order

    struct sockaddr_in saGNI;
    char hostName[NI_MAXHOST];
    char servInfo[NI_MAXSERV];

    saGNI.sin_family = AF_INET;
    saGNI.sin_addr.s_addr = addr;
    saGNI.sin_port = htons(port);

    if (getnameinfo((SOCKADDR *)&saGNI,
                          sizeof(sockaddr),
                          hostName,
                          sizeof(hostName),
                          servInfo,
                          sizeof(servInfo),
                          NI_NUMERICSERV|NI_NAMEREQD)) {
        if (errorCode) {
            *errorCode = WSAGetLastError();
        }
        return -1;
    }

    *canonicalHostname = hostName; // a Fully Qualified Domain Name (FQDN)

#else

#error getHostnameByAddress does not handle current platform type!

#endif

    return 0;
}

int bteso_ResolveUtil::getLocalHostname(bsl::string *result)
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

bteso_ResolveUtil::ResolveByNameCallback
bteso_ResolveUtil::setResolveByNameCallback(
        ResolveByNameCallback callback)
{
    ResolveByNameCallback previousCallback = s_callback_p;
    s_callback_p = callback;
    return previousCallback;
}

bteso_ResolveUtil::ResolveByNameCallback
bteso_ResolveUtil::currentResolveByNameCallback()
{
    return s_callback_p;
}

bteso_ResolveUtil::ResolveByNameCallback
bteso_ResolveUtil::defaultResolveByNameCallback()
{
    return &defaultResolveByNameImp;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
