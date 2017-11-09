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

#include <bdlma_bufferedsequentialallocator.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_deallocatorguard.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_unordered_set.h>

#ifdef BSLS_PLATFORM_OS_UNIX

#include <bsl_c_errno.h>      // errno
#include <arpa/inet.h>
#include <netdb.h>            // h_errno and prototype for _AIX, MAXHOSTNAMELEN
#include <netinet/in.h>
#include <net/if.h>           // definitions of 'ifconf' and 'ifreq' structures
#include <sys/ioctl.h>        // declarations of 'ioctl()' and ioctl's requests

#if defined(BSLS_PLATFORM_OS_SUNOS) || defined(BSLS_PLATFORM_OS_SOLARIS)
#include <sys/sockio.h>       // SunOS keeps ioctl's requests apart
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>        // MAXHOSTNAMELEN (ibm and others)
#include <unistd.h>           // gethostname()

#else                         // windows

#include <bsl_cstring.h>      // memcpy
#include <bsl_cstdlib.h>      // atoi()
#include <iphlpapi.h>         // GetAdaptersAddresses()
#include <winsock2.h>         // getservbyname()
#include <ws2tcpip.h>         // getaddrinfo() getnameinfo()
#define MAXHOSTNAMELEN  256

#pragma comment(lib, "iphlpapi.lib")

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

int getAddressImp(btlso::ResolveUtil::ResolveByNameCallback  cb,
                  btlso::IPv4Address                        *result,
                  const char                                *hostName,
                  int                                       *errorCode)
    // Use the specified 'cb' to load into the specified 'result' the primary
    // IPv4 address of the specified 'hostName'.  Return 0, with no effect on
    // 'errorCode', on success, and return a negative value otherwise.  If an
    // error occurs, the optionally specified 'errorCode' is set to the native
    // error code of the operation and 'result' is unchanged.
{
    // Avoid dynamic memory allocation.

    bsls::AlignedBuffer<sizeof(btlso::IPv4Address)> bufferStorage;
    bdlma::BufferedSequentialAllocator              allocator(
                                                   bufferStorage.buffer(),
                                                   sizeof(btlso::IPv4Address));
    bsl::vector<btlso::IPv4Address>                 addresses(&allocator);

    if (cb(&addresses, hostName, 1, errorCode)
     || addresses.size() < 1) {
        return -1;                                                    // RETURN
    }

    result->setIpAddress(addresses.front().ipAddress());
    return 0;
}

                         // ======================
                         // class CloseSocketGuard
                         // ======================

class CloseSocketGuard {
    // This class implements a guard that closes an open socket file descriptor
    // upon destruction.

  private:
    // PRIVATE TYPES
#if defined(BSLS_PLATFORM_OS_UNIX)
    typedef int    SocketFileDescriptor;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    typedef SOCKET SocketFileDescriptor;
#endif

    // DATA
    SocketFileDescriptor d_sfd;  // socket file descriptor

  public:
    // CREATORS
    explicit
    CloseSocketGuard(SocketFileDescriptor sfd);
         // Create a guard object that will manage the specified 'sfd', closing
         // it upon destruction.

    ~CloseSocketGuard();
        // Close the descriptor, managed by this object.
};

                         // ----------------------
                         // class CloseSocketGuard
                         // ----------------------
// CREATORS
inline
CloseSocketGuard::CloseSocketGuard(CloseSocketGuard::SocketFileDescriptor sfd)
: d_sfd(sfd)
{
}

inline
CloseSocketGuard::~CloseSocketGuard()
{
#if defined(BSLS_PLATFORM_OS_UNIX)
    int rc = close(d_sfd);
    (void)rc;  // Supress compiler warning.
    BSLS_ASSERT(rc == 0);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    int rc = closesocket(d_sfd);
    (void)rc;  // Supress compiler warning.
    BSLS_ASSERT(rc == NO_ERROR);
#endif
}

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
    // error code encountered if the hostname could not be resolved.  On
    // success, '*errorCode' is not modified.  's_byNameCallback_p' is set to
    // this function by default.
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

    // We sometimes get redundant copies of the same address, so keep a
    // hash table to detect and ignore duplicates.  Ensure that values appear
    // in the vector in the same order they appear in the 'getaddrinfo' list.

    bsl::unordered_set<btlso::IPv4Address, IPv4AddressHasher> addressHT;

    for (it = head;
         it && (int) addressHT.size() < numAddresses;
         it = it->ai_next) {
        if (AF_INET == it->ai_family) {
            // Since it's 'AF_INET', we know we can cast 'it->ai_addr' (which
            // is a 'struct sockaddr *') to a 'struct sockaddr_in *'.

            struct sockaddr_in *sockAddrIn = static_cast<struct sockaddr_in *>(
                                             static_cast<void *>(it->ai_addr));

            // Note that 's_addr' below is in network byte order; but this is
            // OK because the contract for 'btlso::IPv4Address::setIpAddress'
            // specifies that its argument is to be in network byte order.

            btlso::IPv4Address address;
            address.setIpAddress(sockAddrIn->sin_addr.s_addr);

            if (addressHT.insert(address).second) {
                hostAddresses->push_back(address);
            }
        }

        // AF_INET6, AF_IRDA, AF_BTH, etc. are ignored.
    }

    freeaddrinfo(head);
    return 0;
}

#if defined(BSLS_PLATFORM_OS_UNIX)
static
int resolveLocalAddrForUnix(bsl::vector<btlso::IPv4Address> *localAddresses,
                            int                              numAddresses,
                            int                             *errorCode)
    // Populate the specified vector 'localAddresses' with the set of IP
    // addresses that refer to the local Unix-like machine, but only take the
    // the first 'numAddresses' addresses found.  Return 0 on success and a
    // non-zero value otherwise, and set '*errorCode' to the platform-dependent
    // error code encountered if the local address could not be resolved.  On
    // success, '*errorCode' is not modified.  IP addresses of ethernet and
    // wireless network interfaces only are loaded to the 'localAddresses'.
{
    BSLS_ASSERT(localAddresses);
    BSLS_ASSERT(0 < numAddresses);

    localAddresses->clear();

    // To obtain a list of local addresses on Unix-like machine, we use 'ioctl'
    // function. 'ioctl' called with 'SIOCGIFCONF' code fills passed array of
    // 'ifreq' structures with information about all network interfaces being
    // presented.  The required size of this array is obtained from another
    // 'ioctl' call with request code specific for different platforms.

    struct ifconf  ifc;
    int            sd;

    // Create a socket so we can use ioctl on the file descriptor to retrieve
    // interfaces info.

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd) {
        if (errorCode) {
            *errorCode = errno;
        }
        return -1;                                                    // RETURN
    }

    CloseSocketGuard cld(sd);

    ifc.ifc_len = 0;
    ifc.ifc_req = 0;

    // Calculating size of buffer for addresses.  There is no universal
    // solution for all Unix-like platforms, so we use specific ioctl request
    // code in each case.

    int bufLen = 0;
    int ret = 0;

#if  defined(BSLS_PLATFORM_OS_AIX)
    // 'SIOCGSIZIFCONF': Gets the size of memory that is required to get
    // configuration information for all interfaces returned by 'SIOCGIFCONF'.

    int ifconfSize = 0;
    ret = ioctl(sd, SIOCGSIZIFCONF, (caddr_t)&ifconfSize);
    bufLen = ifconfSize;
#elif defined(BSLS_PLATFORM_OS_SUNOS) || defined (BSLS_PLATFORM_OS_SOLARIS)
    // 'SIOCGIFNUM': Gets the number of interfaces returned by 'SIOCGIFCONF'.

    int NumberOfInterfaces = 0;
    ret = ioctl(sd, SIOCGIFNUM, &NumberOfInterfaces);
    bufLen = NumberOfInterfaces * sizeof(struct ifreq);
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    // There are not any special ioctls to get neccessary buffer size on
    // Darwin, so it can be obtained experimentally.  We need to give system
    // excessive buffer and the required size will be returned through
    // 'ifc.ifc_len' field.

    const int         MAX_TRIES = 5;
    int               numReqs = 10;
    int               numTries = 0;
    bslma::Allocator *eda = bslma::Default::defaultAllocator();

    do {
        int   experimentalSize = sizeof(struct ifreq)*numReqs;
        char *experimentalBuf =
                          static_cast<char *>(eda->allocate(experimentalSize));

        bslma::DeallocatorGuard<bslma::Allocator> guard(experimentalBuf, eda);

        ifc.ifc_len = experimentalSize;
        ifc.ifc_buf = experimentalBuf;

        // Keep calling ioctl until we provide it a big enough buffer.

        if (ioctl(sd, SIOCGIFCONF, &ifc) < 0) {
            if (errorCode) {
                *errorCode = EFAULT;
            }
            return -1;                                                // RETURN
        }

        if (ifc.ifc_len == bufLen) {
            // As soon as we reach (or exceed) the requested amount of memory,
            // system call will return the same value for any excess.

            break;
        } else {
            // It is possibly insufficient amout of memory.

            bufLen = ifc.ifc_len;
            numReqs *= 2;
        }
        ++numTries;
    } while (MAX_TRIES > numTries);

#else
    // If 'ifc_req' is NULL, 'SIOCGIFCONF' returns the  necessary  buffer size
    // in bytes  for  receiving  all  available  addresses  in 'ifc_len'.

    ret = ioctl(sd, SIOCGIFCONF, &ifc);
    bufLen = ifc.ifc_len;
#endif

    if (ret) {
        if (errorCode) {
            *errorCode = EFAULT;
        }
        return -1;                                                    // RETURN
    }

    if (bufLen == 0) {
        if (errorCode) {
            *errorCode = ENODATA;
        }
        return -1;                                                    // RETURN
    }

    // Memory allocation.

    bslma::Allocator *da = bslma::Default::defaultAllocator();
    char             *buf = static_cast<char *>(da->allocate(bufLen));
    bsl::memset(buf, 0, bufLen);

    bslma::DeallocatorGuard<bslma::Allocator> guard(buf, da);

    // Receiving addresses.

    ifc.ifc_req = reinterpret_cast<struct ifreq *>(buf);
    ifc.ifc_len = bufLen;

    if (ioctl(sd, SIOCGIFCONF, &ifc) == 0) {
        int   handledBytesNum = 0;
        while (handledBytesNum < ifc.ifc_len &&
               static_cast<int>(localAddresses->size()) < numAddresses) {
            ifreq *ifr = reinterpret_cast<struct ifreq *>(buf);
            if (ifr->ifr_addr.sa_family == AF_INET) {
                const bsl::string LOCALHOST("127.0.0.1");
                char              host[NI_MAXHOST] = {0};
                int               rc = getnameinfo(&ifr->ifr_addr,
                                                   sizeof(struct sockaddr_in),
                                                   host,
                                                   sizeof host,
                                                   0,
                                                   0,
                                                   NI_NUMERICHOST);
                if (0 == rc && LOCALHOST != host) {
                    sockaddr_in        *saIn = reinterpret_cast<sockaddr_in *>(
                                                             &ifr->ifr_addr);
                    btlso::IPv4Address  address(saIn->sin_addr.s_addr, 0);
                    localAddresses->push_back(address);
                }
            }

            size_t delta = 0;

#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_DARWIN)
            // These platforms store data of interfaces in a different way.
            // That is why we have to shift pointer manually.

            size_t addrSize = (ifr->ifr_addr.sa_len > sizeof(ifr->ifr_addr)
                               ? ifr->ifr_addr.sa_len
                               : sizeof(ifr->ifr_addr));
            delta = sizeof(ifr->ifr_name) + addrSize;
#else
            delta = sizeof(struct ifreq);
#endif

            buf = buf + delta;
            handledBytesNum += static_cast<int>(delta);
        }
    } else {
        if (errorCode) {
            *errorCode = EFAULT;
        }
        return -1;                                                    // RETURN
    }

    return 0;
}
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
static
int resolveLocalAddrForWindows(bsl::vector<btlso::IPv4Address> *localAddresses,
                               int                              numAddresses,
                               int                             *errorCode)
    // Populate the specified vector 'localAddresses' with the set of IP
    // addresses that refer to the local Windows machine, but only take the
    // the first 'numAddresses' addresses found.  Return 0 on success and a
    // non-zero value otherwise, and set '*errorCode' to the platform-dependent
    // error code encountered if the local address could not be resolved.  On
    // success, '*errorCode' is not modified.  IP addresses of ethernet and
    // wireless network interfaces only are loaded to the 'localAddresses'.
{
    BSLS_ASSERT(localAddresses);
    BSLS_ASSERT(0 < numAddresses);

    localAddresses->clear();

    enum {
        k_WORKING_BUFFER_SIZE = 16*1024,
        k_MAX_TRIES           = 3
    };

    PIP_ADAPTER_ADDRESSES        pAddresses = NULL;
    PIP_ADAPTER_ADDRESSES        pCurrAddresses = 0;
    PIP_ADAPTER_UNICAST_ADDRESS  pUnicast = 0;
    ULONG                        outBufLen = k_WORKING_BUFFER_SIZE;
    DWORD                        retValue = NO_ERROR;
    int                          iterationsNum = 0;
    bslma::Allocator            *da = bslma::Default::defaultAllocator();

    // Memory allocation.  To determine the memory needed to return the
    // IP_ADAPTER_ADDRESSES structures we pre-allocate a 16KB working buffer.
    // If memory is not enough, the function will fail with
    // 'ERROR_BUFFER_OVERFLOW' error code. When the return value is
    // 'ERROR_BUFFER_OVERFLOW', the 'outBufLen' argument contains the required
    // size of the buffer to hold the adapter information.
    // msdn.microsoft.com/en-us/library/windows/desktop/aa365915(v=vs.85).aspx

    do {
        pAddresses =
                   static_cast<PIP_ADAPTER_ADDRESSES>(da->allocate(outBufLen));

        if (pAddresses == NULL) {
            if (errorCode) {
                *errorCode = ERROR_NOT_ENOUGH_MEMORY;
            }
            return -1;                                                // RETURN
        }

        retValue = GetAdaptersAddresses(AF_INET,
                                        GAA_FLAG_INCLUDE_PREFIX,
                                        NULL,
                                        pAddresses,
                                        &outBufLen);

        if (retValue == ERROR_BUFFER_OVERFLOW) {
            da->deallocate(pAddresses);
            pAddresses = NULL;
        } else {
            break;
        }

        iterationsNum++;

    } while (retValue == ERROR_BUFFER_OVERFLOW && iterationsNum < k_MAX_TRIES);

    if(retValue != NO_ERROR) {
        if (errorCode) {
            *errorCode = retValue;
        }
        return -1;                                                    // RETURN
    }

    bslma::DeallocatorGuard<bslma::Allocator> guard(
                                               static_cast<void *>(pAddresses),
                                               da);
    // Extracting addresses.

    pCurrAddresses = pAddresses;
    for (pCurrAddresses = pAddresses;
         pCurrAddresses &&
         static_cast<int>(localAddresses->size()) < numAddresses;
         pCurrAddresses = pCurrAddresses->Next) {
        if ((pCurrAddresses->IfType == IF_TYPE_ETHERNET_CSMACD) ||
            (pCurrAddresses->IfType == IF_TYPE_IEEE80211) ) {
            pUnicast = pCurrAddresses->FirstUnicastAddress;
            for (pUnicast = pCurrAddresses->FirstUnicastAddress;
                 pUnicast &&
                 static_cast<int>(localAddresses->size()) < numAddresses;
                 pUnicast = pUnicast->Next) {
                sockaddr_in *saIn = reinterpret_cast<sockaddr_in *>(
                                                 pUnicast->Address.lpSockaddr);
                btlso::IPv4Address  address(saIn->sin_addr.s_addr, 0);
                localAddresses->push_back(address);
            }
        }
    }

    return 0;
}
#endif

static
int defaultResolveLocalAddrImp(bsl::vector<btlso::IPv4Address> *localAddresses,
                               int                              numAddresses,
                               int                             *errorCode)
    // Populate the specified vector 'localAddresses' with the set of IP
    // addresses that refer to the local machine, but only take the the first
    // 'numAddresses' addresses found.  Return 0 on success and a non-zero
    // value otherwise, and set '*errorCode' to the platform-dependent error
    // code encountered if the local address could not be resolved.  On
    // success, '*errorCode' is not modified.  IP addresses of ethernet and
    // wireless network interfaces only are loaded to the 'localAddresses'.
    // 's_localCallback_p' is set to this function by default.
{

#if defined(BSLS_PLATFORM_OS_UNIX)
    return resolveLocalAddrForUnix(localAddresses, numAddresses, errorCode);
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    return resolveLocalAddrForWindows(localAddresses, numAddresses, errorCode);
#endif

}


namespace btlso {

                           // ================
                           // static callbacks
                           // ================

static ResolveUtil::ResolveByNameCallback    s_byNameCallback_p =
                                                      &defaultResolveByNameImp;
static ResolveUtil::ResolveLocalAddrCallback s_localCallback_p =
                                                   &defaultResolveLocalAddrImp;

                          // ------------------
                          // struct ResolveUtil
                          // ------------------

// CLASS METHODS
int ResolveUtil::getAddress(IPv4Address *result,
                            const char  *hostName,
                            int         *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostName);

    return getAddressImp(s_byNameCallback_p,
                         result,
                         hostName,
                         errorCode);
}

int ResolveUtil::getAddressDefault(IPv4Address *result,
                                   const char  *hostName,
                                   int         *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostName);

    return getAddressImp(defaultResolveByNameImp,
                         result,
                         hostName,
                         errorCode);
}

int ResolveUtil::getAddresses(bsl::vector<IPv4Address> *result,
                              const char               *hostName,
                              int                      *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostName);

    return s_byNameCallback_p(result, hostName, INT_MAX, errorCode);
}

int ResolveUtil::getAddressesDefault(bsl::vector<IPv4Address> *result,
                                     const char               *hostName,
                                     int                      *errorCode)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(hostName);

    return defaultResolveByNameImp(result, hostName, INT_MAX, errorCode);
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

int ResolveUtil::getLocalAddresses(bsl::vector<IPv4Address> *result,
                                   int                      *errorCode)
{
    BSLS_ASSERT(result);

    return s_localCallback_p(result, INT_MAX, errorCode);
}

int ResolveUtil::getLocalAddressesDefault(bsl::vector<IPv4Address> *result,
                                          int                      *errorCode)
{
    BSLS_ASSERT(result);

    return defaultResolveLocalAddrImp(result, INT_MAX, errorCode);
}

ResolveUtil::ResolveByNameCallback
ResolveUtil::setResolveByNameCallback(ResolveByNameCallback callback)
{
    ResolveByNameCallback previousCallback = s_byNameCallback_p;
    s_byNameCallback_p = callback;
    return previousCallback;
}

ResolveUtil::ResolveByNameCallback
ResolveUtil::currentResolveByNameCallback()
{
    return s_byNameCallback_p;
}

ResolveUtil::ResolveByNameCallback
ResolveUtil::defaultResolveByNameCallback()
{
    return &defaultResolveByNameImp;
}

ResolveUtil::ResolveLocalAddrCallback
ResolveUtil::setResolveLocalAddrCallback(ResolveLocalAddrCallback callback)
{
    ResolveLocalAddrCallback previousCallback = s_localCallback_p;
    s_localCallback_p = callback;
    return previousCallback;
}

ResolveUtil::ResolveLocalAddrCallback
ResolveUtil::currentResolveLocalAddrCallback()
{
    return s_localCallback_p;
}

ResolveUtil::ResolveLocalAddrCallback
ResolveUtil::defaultResolveLocalAddrCallback()
{
    return &defaultResolveLocalAddrImp;
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
