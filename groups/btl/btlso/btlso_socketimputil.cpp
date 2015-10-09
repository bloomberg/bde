// btlso_socketimputil.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_socketimputil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_socketimputil_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <bsl_c_stdio.h>

#if defined(BTLSO_PLATFORM_WIN_SOCKETS)

// ========================= WINDOWS SPECIFIC CODE ============================

#include <winsock2.h>

#if !defined(_INC_ERRNO)
#include <bsl_c_errno.h>
#endif

// the location of the address in the sockaddr structure differs by platform
// create a macro to access that field here
#define SI_SADDR(x)   (x).sin_addr.S_un.S_addr

namespace BloombergLP {
namespace btlso {

                        // --------------------
                        // struct SocketImpUtil
                        // --------------------

int SocketImpUtil::cleanup(int *errorCode)
{
    int rc;

    rc = WSACleanup();

    int errorNumber = rc >= 0 ? 0 : SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode) {
        *errorCode = errorNumber;
    }
    return errorNumber ? SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

int SocketImpUtil_Util::getErrorCode(void)
{
    int rc = WSAGetLastError();
    if (!rc) rc = GetLastError();
    if (!rc) rc = errno;
    return rc;
}

int SocketImpUtil::startup(int *errorCode)
{

    WSADATA wd;
    int     rc;

    // if not already initialized request at least version 2.0
    WORD wVersionRequested;
    wVersionRequested = MAKEWORD(2, 0);
    rc = WSAStartup(wVersionRequested, &wd);
    if (!rc) {
        // verify version
        if (!((LOBYTE(wd.wVersion) >= 2) && (HIBYTE(wd.wVersion) >= 0))) {
            // requested version not supported, cleanup
            WSACleanup();
            rc = -1;
        }
    }

    int errorNumber = rc >= 0 ? 0 : SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode) {
        *errorCode = errorNumber;
    }
    return errorNumber ? SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

}  // close package namespace
}  // close enterprise namespace

#else

// =========================== UNIX SPECIFIC CODE =============================

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <bsl_c_errno.h>

// manifest constants for shutdown()
#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

// the location of the address in the sockaddr structure differs by platform
// create a macro to access that field here
//#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_LINUX)
#define SI_SADDR(x)   (x).sin_addr.s_addr
//#else
//#define SI_SADDR(x)   (x).sin_addr.S_un.S_addr
//#endif

// certain UNIX platforms do not define INADDR_NONE
// it it has not already been defined, define it here
#if !defined(INADDR_NONE)
#define INADDR_NONE   0xFFFFFFFF
#endif

namespace BloombergLP {
namespace btlso {

// dummy implementations of startup and cleanup for UNIX

int SocketImpUtil::startup(int *)
{
    return 0;
}

int SocketImpUtil::cleanup(int *)
{
    return 0;
}

int SocketImpUtil_Util::getErrorCode(void)
{
    return errno;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ============================ COMMON FUNCTIONS ==============================

namespace BloombergLP {
namespace btlso {

                        // -------------------------
                        // struct SocketImpUtil_Util
                        // -------------------------

int SocketImpUtil_Util::mapErrorCode(int errorNumber)
{
    switch (
#if defined(BTLSO_PLATFORM_WIN_SOCKETS)
            errorNumber
#else
            BSLS_PERFORMANCEHINT_PREDICT_EXPECT(errorNumber, EAGAIN)
#endif
                                                                    ) {
#if defined(BTLSO_PLATFORM_WIN_SOCKETS)
      // Windows (WINSOCK) specific socket error codes
      // WSAEWOULDBLOCK is the most likely error for common use.

      case WSAEWOULDBLOCK:                                      // FALL THROUGH
      case WSAEINPROGRESS: {
        return btlso::SocketHandle::e_ERROR_WOULDBLOCK;               // RETURN
      }
      case WSAECONNABORTED:                                     // FALL THROUGH
      case WSAECONNREFUSED:                                     // FALL THROUGH
      case WSAECONNRESET:                                       // FALL THROUGH
      case WSAEDISCON:                                          // FALL THROUGH
      case WSAEHOSTDOWN:                                        // FALL THROUGH
      case WSAEHOSTUNREACH:                                     // FALL THROUGH
      case WSAENETDOWN:                                         // FALL THROUGH
      case WSAENETRESET:                                        // FALL THROUGH
      case WSAENETUNREACH:                                      // FALL THROUGH
      case WSAENOTCONN:                                         // FALL THROUGH
      case WSAEREFUSED:                                         // FALL THROUGH
      case WSAETIMEDOUT: {
        return btlso::SocketHandle::e_ERROR_CONNDEAD;                 // RETURN
      }
      case WSAEINTR: {
        return btlso::SocketHandle::e_ERROR_INTERRUPTED;              // RETURN
      }
      case WSAEACCES:                                           // FALL THROUGH
      case WSAEADDRINUSE:                                       // FALL THROUGH
      case WSAEADDRNOTAVAIL:                                    // FALL THROUGH
      case WSAEISCONN:                                          // FALL THROUGH
      case WSAELOOP:                                            // FALL THROUGH
      case WSAEMFILE:                                           // FALL THROUGH
      case WSAEMSGSIZE:                                         // FALL THROUGH
      case WSAENOTSOCK:                                         // FALL THROUGH
      case WSAESTALE:                                           // FALL THROUGH
      case WSANOTINITIALISED:                                   // FALL THROUGH
      case WSASYSNOTREADY: {
        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
      case WSAEAFNOSUPPORT:                                     // FALL THROUGH
      case WSAEALREADY:                                         // FALL THROUGH
      case WSAEBADF:                                            // FALL THROUGH
      case WSAECANCELLED:                                       // FALL THROUGH
      case WSAEDESTADDRREQ:                                     // FALL THROUGH
      case WSAEDQUOT:                                           // FALL THROUGH
      case WSAEFAULT:                                           // FALL THROUGH
      case WSAEINVAL:                                           // FALL THROUGH
      case WSAEINVALIDPROCTABLE:                                // FALL THROUGH
      case WSAEINVALIDPROVIDER:                                 // FALL THROUGH
      case WSAENAMETOOLONG:                                     // FALL THROUGH
      case WSAENOBUFS:                                          // FALL THROUGH
      case WSAENOMORE:                                          // FALL THROUGH
      case WSAENOPROTOOPT:                                      // FALL THROUGH
      case WSAENOTEMPTY:                                        // FALL THROUGH
      case WSAEOPNOTSUPP:                                       // FALL THROUGH
      case WSAEPFNOSUPPORT:                                     // FALL THROUGH
      case WSAEPROCLIM:                                         // FALL THROUGH
      case WSAEPROTONOSUPPORT:                                  // FALL THROUGH
      case WSAEPROTOTYPE:                                       // FALL THROUGH
      case WSAEPROVIDERFAILEDINIT:                              // FALL THROUGH
      case WSAEREMOTE:                                          // FALL THROUGH
      case WSAESHUTDOWN:                                        // FALL THROUGH
      case WSAESOCKTNOSUPPORT:                                  // FALL THROUGH
      case WSAETOOMANYREFS:                                     // FALL THROUGH
      case WSAEUSERS:                                           // FALL THROUGH
      case WSASERVICE_NOT_FOUND:                                // FALL THROUGH
      case WSASYSCALLFAILURE:                                   // FALL THROUGH
      case WSATYPE_NOT_FOUND:                                   // FALL THROUGH
      case WSAVERNOTSUPPORTED:                                  // FALL THROUGH
      case WSA_E_CANCELLED:                                     // FALL THROUGH
      case WSA_E_NO_MORE: {
        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#else
#if EAGAIN!=EWOULDBLOCK
      case EAGAIN:
#endif
      case EWOULDBLOCK: {
        return btlso::SocketHandle::e_ERROR_WOULDBLOCK;               // RETURN
      }
#ifdef EACCES
      case EACCES: {
        // Search permission is denied for a component of the path prefix of
        // the pathname in name.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef EADDRINUSE
      case EADDRINUSE: {
         // The address is already in use.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef EADDRNOTAVAIL
      case EADDRNOTAVAIL: {
         // The specified address is not available on the remote
         // machine.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef EAFNOSUPPORT
      case EAFNOSUPPORT: {
         // Addresses in the specified address family cannot be
         // used with this socket.

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef EALREADY
      case EALREADY: {
         // The socket is non-blocking and a previous connection
         // attempt has not yet been completed.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef EBADF
      case EBADF: {
         // The descriptor is invalid.

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef ECONNREFUSED
      case ECONNREFUSED: {
         // The attempt to connect was forcefully rejected.

        return btlso::SocketHandle::e_ERROR_CONNDEAD;                 // RETURN
      }
#endif

#ifdef EINPROGRESS
      case EINPROGRESS: {
         // The socket is non-blocking and the connection cannot be completed
         // immediately.

        return btlso::SocketHandle::e_ERROR_WOULDBLOCK;               // RETURN
      }
#endif

      case EINTR: {
         // The operation was interrupted by the delivery of a signal.

        return btlso::SocketHandle::e_ERROR_INTERRUPTED;              // RETURN
      }
#ifdef EIO
      case EIO: {
         // An I/O error occurred while reading from or writing to the file
         // system.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef EINVAL
      case EINVAL: {
         // tolen is not the size of a valid address for the specified
         // address family.

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef EISCONN
      case EISCONN: {
         // The socket is already connected.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef ELOOP
      case ELOOP: {
         // Too many symbolic links were encountered in translating the
         // pathname in name.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef ENETUNREACH
      case ENETUNREACH: {
         // The network is not reachable from this host.

        return btlso::SocketHandle::e_ERROR_CONNDEAD;                 // RETURN
      }
#endif

#ifdef EMSGSIZE
      case EMSGSIZE: {
         // The socket requires that message be sent atomically, and the
         // message was too long.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef EMFILE
      case EMFILE: {
         // The per-process descriptor table is full.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef ENFILE
      case ENFILE: {
         // The maximum number of file descriptors in the system are already
         // open.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef ENODEV
      case ENODEV: {
         // The protocol family and type corresponding to s could not be
         // found in the netconfig file.

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef ENOMEM
      case ENOMEM: {
         // There was insufficient user memory available to complete the
         // operation.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef ENOSR
      case ENOSR: {
         // There were insufficient STREAMS resources available to complete
         // the operation.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef ENOENT
      case ENOENT: {
         // A component of the path prefix of the pathname in name does not
         // exist.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef ENXIO
      case ENXIO: {
         //  The server exited before the connection was complete.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif
      case ETIMEDOUT: {
         // Connection establishment timed out without establishing a
         // connection.

         return btlso::SocketHandle::e_ERROR_CONNDEAD;                // RETURN
      }

#ifdef ENOTSOCK
      case ENOTSOCK: {
         // The descriptor does not reference a socket.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#ifdef EOPNOTSUPP
      case EOPNOTSUPP: {
         // The referenced socket is not of type SOCK_STREAM.

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef EOPNOSUPPORT
      case EOPNOSUPPORT: {
         // The specified protocol does not support creation of socket
         // pairs.

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef EPIPE
      case EPIPE: {
        // An attempt is made to write to a socket that is not open for
        // reading, or that has only one end open or that is no longer
        // connected to a peer endpoint.  A SIGPIPE signal will also be sent
        // to the process.  The process dies unless special provisions were
        // taken to catch or ignore the signal.

        return btlso::SocketHandle::e_ERROR_CONNDEAD;                 // RETURN
      }
#endif

#ifdef EPROTONOSUPPORT
      case EPROTONOSUPPORT: {
        // The specified protocol is not supported on this machine.

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef EPROTO
      case EPROTO: {
         // A protocol error has occurred;

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef ECONNABORTED
      case ECONNABORTED: {
         // Software caused connection abort

        return btlso::SocketHandle::e_ERROR_CONNDEAD;                 // RETURN
      }
#endif

#ifdef ECONNRESET
      case ECONNRESET: {
         // Connection reset by peer

        return btlso::SocketHandle::e_ERROR_CONNDEAD;                 // RETURN
      }
#endif

#ifdef EDESTADDRREQ
      case EDESTADDRREQ: {
        // Destination address required

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef EHOSTDOWN
      case EHOSTDOWN: {
        // Host is down

        return btlso::SocketHandle::e_ERROR_CONNDEAD;                 // RETURN
      }
#endif

#ifdef EHOSTUNREACH
      case EHOSTUNREACH: {
        // No route to host

        return btlso::SocketHandle::e_ERROR_CONNDEAD;                 // RETURN
      }
#endif

#ifdef ENETDOWN
      case ENETDOWN: {
        // Network is down

        return btlso::SocketHandle::e_ERROR_CONNDEAD;                 // RETURN
      }
#endif

#ifdef ENETRESET
      case ENETRESET: {
        // Network dropped connection because

        return btlso::SocketHandle::e_ERROR_CONNDEAD;                 // RETURN
      }
#endif

#ifdef ENOBUFS
      case ENOBUFS: {
        // No buffer space available

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef ENOPROTOOPT
      case ENOPROTOOPT: {
        // Protocol not available

        return  btlso::SocketHandle::e_ERROR_UNCLASSIFIED;            // RETURN
      }
#endif

#ifdef ENOTCONN
      case ENOTCONN: {
        // Socket is not connected

        return btlso::SocketHandle::e_ERROR_CONNDEAD;                 // RETURN
      }
#endif

#ifdef EPFNOSUPPORT
      case EPFNOSUPPORT: {
        // Protocol family not supported

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef EPROTOTYPE
      case EPROTOTYPE: {
        // Protocol wrong type for socket

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef ESHUTDOWN
      case ESHUTDOWN: {
        // Cant send after socket shutdown

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef ESOCKTNOSUPPORT
      case ESOCKTNOSUPPORT: {
        // Socket type not supported

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef ETOOMANYREFS
      case ETOOMANYREFS: {
        // Too many references: cant splice

        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
#endif

#ifdef ESTALE
      case ESTALE: {
        // A stale NFS file handle exists.

        return btlso::SocketHandle::e_ERROR_NORESOURCES;              // RETURN
      }
#endif

#endif

      case 0: {
        return 0;                                                     // RETURN
      }

      default: {
        return btlso::SocketHandle::e_ERROR_UNCLASSIFIED;             // RETURN
      }
    }

    BSLS_ASSERT_OPT("Unreachable" && 0);

    return 0;
}

int btlso::SocketImpUtil::close(const btlso::SocketHandle::Handle&  theSocket,
                                int                                *errorCode)
{
    int rc;
#if defined(BTLSO_PLATFORM_WIN_SOCKETS)
    rc = closesocket(theSocket);
#else
    rc = ::close(theSocket);
#endif
    int errorNumber = rc >= 0 ? 0 : SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode) {
        *errorCode = errorNumber;
    }
    return errorNumber ? SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

int btlso::SocketImpUtil::listen(const btlso::SocketHandle::Handle&  theSocket,
                                 int                                 backlog,
                                 int                                *errorCode)
{
    int rc;

    BSLS_ASSERT(backlog >= 0);
    rc = ::listen(theSocket, backlog);
    int errorNumber = rc >= 0 ? 0 : SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode) {
        *errorCode = errorNumber;
    }
    return errorNumber ? SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

int btlso::SocketImpUtil::read(void                               *buffer,
                               const btlso::SocketHandle::Handle&  socket,
                               int                                 numBytes,
                               int                                *errorCode)
{
    int rc;

    BSLS_ASSERT(buffer);
    BSLS_ASSERT(numBytes >= 0);

    rc = recv(socket, static_cast<char *>(buffer), numBytes, 0);
    int errorNumber = rc >= 0 ? 0 : SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode) {
        *errorCode = errorNumber;
    }
    return errorNumber ? SocketImpUtil_Util::mapErrorCode(errorNumber) : rc;
}

                        // --------------------
                        // struct SocketImpUtil
                        // --------------------

int btlso::SocketImpUtil::readv(const btls::Iovec                  *iovecPtr,
                                const btlso::SocketHandle::Handle&  socket,
                                int                                 size,
                                int                                *errorCode)
{
    int rc;

    BSLS_ASSERT(iovecPtr);
    BSLS_ASSERT(size > 0);

#if defined(BTLSO_PLATFORM_WIN_SOCKETS)
    DWORD bytesReceived;
    DWORD lpFlags = 0;
    rc = ::WSARecv(
               socket,
               reinterpret_cast<WSABUF *>(const_cast<btls::Iovec *>(iovecPtr)),
               size,
               &bytesReceived,
               &lpFlags,
               0,
               0);
    if (0 == rc) {
        rc = bytesReceived;
    }
    else {
        rc = -1;
    }
#else
    rc = ::readv(socket, reinterpret_cast<const ::iovec *>(iovecPtr), size);
#endif

    int errorNumber = rc >= 0 ? 0 : SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode) {
        *errorCode = errorNumber;
    }
    return errorNumber ? SocketImpUtil_Util::mapErrorCode(errorNumber) : rc;
}

int btlso::SocketImpUtil::write(const btlso::SocketHandle::Handle&  socket,
                                const void                         *buffer,
                                int                                 numBytes,
                                int                                *errorCode)
{
    // Implementation notes: For non blocking IO, the send operation will write
    // out some or all of the buffer if possible.  However, since the
    // implementation of sockets may include internal buffers of certain sizes
    // and optimizations, the send function may return a "would block" error or
    // an smaller than expected number of bytes written even if it appears as
    // if the internal socket buffer has more room.  This is known as the low
    // water mark and may be implemented even if the equivalent socket option
    // is not available.  For the blocking IO case, the send operation may
    // block until all or part of the data is written.  No assumption should be
    // made regarding the number of bytes that will be written with a single
    // call.

    int rc;

    BSLS_ASSERT(buffer);
    BSLS_ASSERT(numBytes >= 0);

    rc = ::send(socket, static_cast<const char *>(buffer), numBytes, 0);

    int errorNumber = rc >= 0 ? 0 : SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode) {
        *errorCode = errorNumber;
    }
    return errorNumber ? SocketImpUtil_Util::mapErrorCode(errorNumber) : rc;
}

int btlso::SocketImpUtil::writev(const btlso::SocketHandle::Handle&  socket,
                                 const btls::Ovec                   *ovec,
                                 int                                 size,
                                 int                                *errorCode)
{
    int rc;

    BSLS_ASSERT(ovec);
    BSLS_ASSERT(size > 0);

#if defined(BTLSO_PLATFORM_WIN_SOCKETS)
    DWORD bytesSent;
    rc = ::WSASend(socket,
                   reinterpret_cast<WSABUF *>(const_cast<btls::Ovec *>(ovec)),
                   size,
                   &bytesSent,
                   0,
                   0,
                   0);
    if (0 == rc) {
        rc = bytesSent;
    }
    else {
        rc = 0;
    }
#else
    rc = ::writev(socket, reinterpret_cast<const ::iovec *>(ovec), size);
#endif

    int errorNumber = rc > 0 ? 0 : SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode) {
        *errorCode = errorNumber;
    }
    return errorNumber ? SocketImpUtil_Util::mapErrorCode(errorNumber) : rc;
}

int btlso::SocketImpUtil::shutDown(
                                 const btlso::SocketHandle::Handle&  socket,
                                 btlso::SocketImpUtil::ShutDownType  how,
                                 int                                *errorCode)
{
    int rc;
    BSLMF_ASSERT(e_SHUTDOWN_RECEIVE == 0
              && e_SHUTDOWN_SEND    == 1
              && e_SHUTDOWN_BOTH    == 2);

    BSLS_ASSERT(static_cast<int>(how) >= e_SHUTDOWN_RECEIVE
             && static_cast<int>(how) <= e_SHUTDOWN_BOTH);

    rc = ::shutdown(socket, static_cast<int>(how));

    int errorNumber = rc >= 0 ? 0 : SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode) {
        *errorCode = errorNumber;
    }
    return errorNumber ? SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

                        // ------------------------
                        // struct SocketImpUtil_Imp
                        // ------------------------
template <>
int btlso::SocketImpUtil_Imp<btlso::IPv4Address>::socketPair(
                                       btlso::SocketHandle::Handle *newSockets,
                                       btlso::SocketImpUtil::Type   type,
                                       int                          protocol,
                                       int                         *errorCode)
{
    BSLS_ASSERT(newSockets);

    // Validate the parameters
    if (type != btlso::SocketImpUtil::k_SOCKET_STREAM
     && type != btlso::SocketImpUtil::k_SOCKET_DATAGRAM) {
        return -1;                                                    // RETURN
    }

    if (type == btlso::SocketImpUtil::k_SOCKET_STREAM) {
        // A pair of TCP sockets are created each connected to each other via
        // the loopback address and an anonymous port.  The result will be a
        // symmetric pair of connected stream sockets.

        btlso::SocketHandle::Handle listenSocket;

        int classification = open(&listenSocket, type, protocol, errorCode);

        // Test for errors

        if (classification < 0) {
            return classification;                                    // RETURN
        }

        // Bind to localhost and anon port.

        btlso::IPv4Address localAnonAddress("127.0.0.1",
                                            btlso::IPv4Address::k_ANY_PORT);

        classification = btlso::SocketImpUtil::bind(listenSocket,
                                                    localAnonAddress,
                                                    errorCode);
        // Test for errors

        if (classification < 0) {
            btlso::SocketImpUtil::close(listenSocket);
            return classification;                                    // RETURN
        }

        // Get the address for the client to connect to

        btlso::IPv4Address localAddress;
        classification = btlso::SocketImpUtil::getLocalAddress(&localAddress,
                                                               listenSocket,
                                                               errorCode);

        // Test for errors

        if (classification < 0) {
            btlso::SocketImpUtil::close(listenSocket);
            return classification;                                    // RETURN
        }

        // Listen for the client connection

        classification = btlso::SocketImpUtil::listen(listenSocket,
                                                      1,
                                                      errorCode);
        // Test for errors

        if (classification < 0) {
            btlso::SocketImpUtil::close(listenSocket);
            return classification;                                    // RETURN
        }

        // Create the client socket

        btlso::SocketHandle::Handle clientSocket;
        classification = open(&clientSocket, type, protocol, errorCode);

        // Test for errors

        if (classification < 0) {
            btlso::SocketImpUtil::close(listenSocket);
            return classification;                                    // RETURN
        }

        // Connect to the listening socket (blocking)

        classification = btlso::SocketImpUtil::connect(clientSocket,
                                                       localAddress,
                                                       errorCode);
        // Test for errors

        if (classification < 0) {
            btlso::SocketImpUtil::close(listenSocket);
            btlso::SocketImpUtil::close(clientSocket);
            return classification;                                    // RETURN
        }

        // Accept the connection from the client

        btlso::SocketHandle::Handle serverSocket;
        classification = accept(&serverSocket, listenSocket, errorCode);

        // Test for errors

        if (classification < 0) {
            btlso::SocketImpUtil::close(listenSocket);
            btlso::SocketImpUtil::close(clientSocket);
            return classification;                                    // RETURN
        }

        // Close the listen socket

        btlso::SocketImpUtil::close(listenSocket);

        // Validate that both sockets are connected to each other.

        btlso::IPv4Address clientAddress, peerAddress;
        classification = btlso::SocketImpUtil::getLocalAddress(&clientAddress,
                                                               clientSocket,
                                                               errorCode);

        int classification2 = btlso::SocketImpUtil::getPeerAddress(
                                                                  &peerAddress,
                                                                  serverSocket,
                                                                  errorCode);

        if (classification < 0
         || classification2 < 0
         || clientAddress != peerAddress) {
            btlso::SocketImpUtil::close(clientSocket);
            btlso::SocketImpUtil::close(serverSocket);
            return -1;                                                // RETURN
        }

        newSockets[0] = clientSocket;
        newSockets[1] = serverSocket;
    }
    else {
        // Two identical datagram sockets are created.  They are both bound
        // using 'bind' to anonymous ports on the loopback address 'localhost'.
        // Using 'connect', each socket is associated with each over preventing
        // data from other sources from being received.

        // Create the client socket

        btlso::SocketHandle::Handle clientSocket;

        int classification = open(&clientSocket, type, protocol, errorCode);

        // Test for errors

        if (classification < 0) {
            return classification;                                    // RETURN
        }

        // Bind to localhost and anon port.

        btlso::IPv4Address localAnonAddress("127.0.0.1",
                                            btlso::IPv4Address::k_ANY_PORT);

        classification = btlso::SocketImpUtil::bind(clientSocket,
                                                    localAnonAddress,
                                                    errorCode);

        // Test for errors

        if (classification < 0) {
            btlso::SocketImpUtil::close(clientSocket);
            return classification;                                    // RETURN
        }

        // Get the address for the peer to connect to

        btlso::IPv4Address clientAddress;
        classification = btlso::SocketImpUtil::getLocalAddress(&clientAddress,
                                                               clientSocket,
                                                               errorCode);

        // Test for errors

        if (classification < 0) {
            btlso::SocketImpUtil::close(clientSocket);
            return classification;                                    // RETURN
        }

        // Create the server socket

        btlso::SocketHandle::Handle serverSocket;
        classification = open(&serverSocket, type, protocol, errorCode);

        // Test for errors

        if (classification < 0) {
            btlso::SocketImpUtil::close(clientSocket);
            return classification;                                    // RETURN
        }

        // Bind to localhost and anon port.

        classification = bind(serverSocket, localAnonAddress, errorCode);

        // Test for errors

        if (classification < 0) {
            btlso::SocketImpUtil::close(clientSocket);
            btlso::SocketImpUtil::close(serverSocket);
            return classification;                                    // RETURN
        }

        // Get the address for the peer to connect to

        btlso::IPv4Address serverAddress;
        classification = btlso::SocketImpUtil::getLocalAddress(&serverAddress,
                                                               serverSocket,
                                                               errorCode);

        // Test for errors

        if (classification < 0) {
            btlso::SocketImpUtil::close(clientSocket);
            btlso::SocketImpUtil::close(serverSocket);
            return classification;                                    // RETURN
        }

        // Associate sockets with each other.

        classification = btlso::SocketImpUtil::connect(serverSocket,
                                                       clientAddress,
                                                       errorCode);
        // Test for errors

        if (classification < 0) {
            btlso::SocketImpUtil::close(serverSocket);
            btlso::SocketImpUtil::close(clientSocket);
            return classification;                                    // RETURN
        }

        classification = btlso::SocketImpUtil::connect(clientSocket,
                                                       serverAddress,
                                                       errorCode);

        // Test for errors

        if (classification < 0) {
            btlso::SocketImpUtil::close(serverSocket);
            btlso::SocketImpUtil::close(clientSocket);
            return classification;                                    // RETURN
        }

        newSockets[0] = clientSocket;
        newSockets[1] = serverSocket;
    }

    return 0;
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
