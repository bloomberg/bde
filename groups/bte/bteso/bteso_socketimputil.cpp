// bteso_socketimputil.cpp         -*-C++-*-
#include <bteso_socketimputil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_socketimputil_cpp,"$Id$ $CSID$")

#include <bcemt_thread.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <bsl_c_stdio.h>

namespace BloombergLP {
#if defined(BTESO_PLATFORM_WIN_SOCKETS)

// ========================= WINDOWS SPECIFIC CODE ============================

#include <winsock2.h>

#if !defined(_INC_ERRNO)
#include <bsl_c_errno.h>
#endif

// the location of the address in the sockaddr structure differs by platform
// create a macro to access that field here
#define SI_SADDR(x)   (x).sin_addr.S_un.S_addr

int bteso_SocketImpUtil::cleanup(int *errorCode)
{
    int rc;

    rc = WSACleanup();

    int errorNumber = rc >= 0 ? 0 : bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

int bteso_SocketImpUtil_Util::getErrorCode(void)
{
    int rc = WSAGetLastError();
    if (!rc) rc = GetLastError();
    if (!rc) rc = errno;
    return rc;
}

int bteso_SocketImpUtil::startup(int *errorCode)
{

    WSADATA wd;
    int rc;

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

    int errorNumber = rc >= 0 ? 0 : bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

#else

// =========================== UNIX SPECIFIC CODE =============================

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <bsl_cstring.h>
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

// dummy implementations of startup and cleanup for UNIX
int bteso_SocketImpUtil::startup(int *)
{
    return 0;
}

int bteso_SocketImpUtil::cleanup(int *)
{
    return 0;
}

int bteso_SocketImpUtil_Util::getErrorCode(void)
{
    return errno;
}

#endif

// ============================ COMMON FUNCTIONS ==============================

int bteso_SocketImpUtil_Util::mapErrorCode(int errorNumber)
{

//-------------
// TODO - mappings are not complete!!!! (e.g., most map to unclassified)
//------------

#if defined(BTESO_PLATFORM_WIN_SOCKETS)
    switch (errorNumber) {
        // Windows (WINSOCK) specific socket error codes
        // WSAEWOULDBLOCK is the most likely error for common use.

        case WSAEWOULDBLOCK:
        case WSAEINPROGRESS:
             return bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK;

        case WSAECONNABORTED:
        case WSAECONNREFUSED:
        case WSAECONNRESET:
        case WSAEDISCON:
        case WSAEHOSTDOWN:
        case WSAEHOSTUNREACH:
        case WSAENETDOWN:
        case WSAENETRESET:
        case WSAENETUNREACH:
        case WSAENOTCONN:
        case WSAEREFUSED:
        case WSAETIMEDOUT:
             return bteso_SocketHandle::BTESO_ERROR_CONNDEAD;

        case WSAEINTR:
             return bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;

        case WSAEACCES:
        case WSAEADDRINUSE:
        case WSAEADDRNOTAVAIL:
        case WSAEISCONN:
        case WSAELOOP:
        case WSAEMFILE:
        case WSAEMSGSIZE:
        case WSAENOTSOCK:
        case WSAESTALE:
        case WSANOTINITIALISED:
        case WSASYSNOTREADY:
             return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;

        case WSAEAFNOSUPPORT:
        case WSAEALREADY:
        case WSAEBADF:
        case WSAECANCELLED:
        case WSAEDESTADDRREQ:
        case WSAEDQUOT:
        case WSAEFAULT:
        case WSAEINVAL:
        case WSAEINVALIDPROCTABLE:
        case WSAEINVALIDPROVIDER:
        case WSAENAMETOOLONG:
        case WSAENOBUFS:
        case WSAENOMORE:
        case WSAENOPROTOOPT:
        case WSAENOTEMPTY:
        case WSAEOPNOTSUPP:
        case WSAEPFNOSUPPORT:
        case WSAEPROCLIM:
        case WSAEPROTONOSUPPORT:
        case WSAEPROTOTYPE:
        case WSAEPROVIDERFAILEDINIT:
        case WSAEREMOTE:
        case WSAESHUTDOWN:
        case WSAESOCKTNOSUPPORT:
        case WSAETOOMANYREFS:
        case WSAEUSERS:
        case WSASERVICE_NOT_FOUND:
        case WSASYSCALLFAILURE:
        case WSATYPE_NOT_FOUND:
        case WSAVERNOTSUPPORTED:
        case WSA_E_CANCELLED:
        case WSA_E_NO_MORE:
             return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#else
    switch (BSLS_PERFORMANCEHINT_PREDICT_EXPECT(errorNumber, EAGAIN)) {
#if EAGAIN!=EWOULDBLOCK
     case EAGAIN:
#endif
     case EWOULDBLOCK:
         return bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK;

#ifdef EACCES
        case EACCES:
          // Search permission is denied for a component of the path prefix of
          // the pathname in name.

          return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef EADDRINUSE
        case EADDRINUSE:
           // The address is already in use.

         return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef EADDRNOTAVAIL
        case EADDRNOTAVAIL:
           // The specified address is not available on the remote
           // machine.

           return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef EAFNOSUPPORT
        case EAFNOSUPPORT:
           // Addresses in the specified address family cannot be
           // used with this socket.

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef EALREADY
        case EALREADY:
           // The socket is non-blocking and a previous connection
           // attempt has not yet been completed.

           return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef EBADF
        case EBADF:
           // The descriptor is invalid.

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef ECONNREFUSED
        case ECONNREFUSED:
           // The attempt to connect was forcefully rejected.

           return bteso_SocketHandle::BTESO_ERROR_CONNDEAD;
#endif

#ifdef EINPROGRESS
        case EINPROGRESS:
           // The socket is non-blocking and the connection cannot be completed
           // immediately.

           return bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK;
#endif

        case EINTR:
           // The operation was interrupted by the delivery of a signal.

           return bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;

#ifdef EIO
        case EIO:
           // An I/O error occurred while reading from or writing to the file
           // system.

           return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef EINVAL
        case EINVAL:
           // tolen is not the size of a valid address for the specified
           // address family.

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef EISCONN
        case EISCONN:
           // The socket is already connected.

           return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef ELOOP
        case ELOOP:
           // Too many symbolic links were encountered in translating the
           // pathname in name.

           return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef ENETUNREACH
        case ENETUNREACH:
           // The network is not reachable from this host.

           return bteso_SocketHandle::BTESO_ERROR_CONNDEAD;
#endif

#ifdef EMSGSIZE
        case EMSGSIZE:
           // The socket requires that message be sent atomically, and the
           // message was too long.

           return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef EMFILE
        case EMFILE:
           // The per-process descriptor table is full.

           return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef ENFILE
        case ENFILE:
           // The maximum number of file descriptors in the system are already
           // open.

           return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef ENODEV
        case ENODEV:
           // The protocol family and type corresponding to s could not be
           // found in the netconfig file.

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef ENOMEM
        case ENOMEM:
           // There was insufficient user memory available to complete the
           // operation.

           return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef ENOSR
     case ENOSR:
           // There were insufficient STREAMS resources available to complete
           // the operation.

           return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef ENOENT
     case ENOENT:
           // A component of the path prefix of the pathname in name does not
           // exist.

           return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef ENXIO
     case ENXIO:
           //  The server exited before the connection was complete.

           return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

     case ETIMEDOUT:
           // Connection establishment timed out without establishing a
           // connection.

           return bteso_SocketHandle::BTESO_ERROR_CONNDEAD;

#ifdef ENOTSOCK
     case ENOTSOCK:
           // The descriptor does not reference a socket.

          return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#ifdef EOPNOTSUPP
     case EOPNOTSUPP:
           // The referenced socket is not of type SOCK_STREAM.

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef EOPNOSUPPORT
     case EOPNOSUPPORT:
           // The specified protocol does not support creation of socket
           // pairs.

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef EPIPE
     case EPIPE:
           // An attempt is made to write to a socket that is not open for
           // reading, or that has only one end open or that is no longer
           // connected to a peer endpoint.  A SIGPIPE signal will also be sent
           // to the process.  The process dies unless special provisions were
           // taken to catch or ignore the signal.

           return bteso_SocketHandle::BTESO_ERROR_CONNDEAD;
#endif

#ifdef EPROTONOSUPPORT
     case EPROTONOSUPPORT:
           // The specified protocol is not supported on this
           // machine.

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef EPROTO
     case EPROTO:
           // A protocol error has occurred;

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef ECONNABORTED
     case ECONNABORTED:
           // Software caused connection abort

           return bteso_SocketHandle::BTESO_ERROR_CONNDEAD;
#endif

#ifdef ECONNRESET
     case ECONNRESET:
           // Connection reset by peer

           return bteso_SocketHandle::BTESO_ERROR_CONNDEAD;
#endif

#ifdef EDESTADDRREQ
     case EDESTADDRREQ:
           // Destination address required

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef EHOSTDOWN
     case EHOSTDOWN:
           // Host is down

           return bteso_SocketHandle::BTESO_ERROR_CONNDEAD;
#endif

#ifdef EHOSTUNREACH
     case EHOSTUNREACH:
           // No route to host

          return bteso_SocketHandle::BTESO_ERROR_CONNDEAD;
#endif

#ifdef ENETDOWN
     case ENETDOWN:
           // Network is down

           return bteso_SocketHandle::BTESO_ERROR_CONNDEAD;
#endif

#ifdef ENETRESET
     case ENETRESET:
           // Network dropped connection because

           return bteso_SocketHandle::BTESO_ERROR_CONNDEAD;
#endif

#ifdef ENOBUFS
     case ENOBUFS:
           // No buffer space available

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef ENOPROTOOPT
     case ENOPROTOOPT:
           // Protocol not available

           return  bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef ENOTCONN
     case ENOTCONN:
           // Socket is not connected

           return bteso_SocketHandle::BTESO_ERROR_CONNDEAD;
#endif

#ifdef EPFNOSUPPORT
     case EPFNOSUPPORT:
           // Protocol family not supported

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef EPROTOTYPE
     case EPROTOTYPE:
           // Protocol wrong type for socket

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef ESHUTDOWN
     case ESHUTDOWN:
           // Cant send after socket shutdown

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef ESOCKTNOSUPPORT
     case ESOCKTNOSUPPORT:
           // Socket type not supported

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef ETOOMANYREFS
     case ETOOMANYREFS:
           // Too many references: cant splice

           return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
#endif

#ifdef ESTALE
     case ESTALE:
           // A stale NFS file handle exists.

           return bteso_SocketHandle::BTESO_ERROR_NORESOURCES;
#endif

#endif

     case 0: return 0;

     default: return bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
    }

    BSLS_ASSERT_OPT("Unreachable" && 0);

    return 0;
}

int bteso_SocketImpUtil::close(const bteso_SocketHandle::Handle& theSocket,
                               int                              *errorCode)
{
    int rc;
#if defined(BTESO_PLATFORM_WIN_SOCKETS)
    rc = closesocket(theSocket);
#else
    rc = ::close(theSocket);
#endif
    int errorNumber = rc >= 0 ? 0 : bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

int bteso_SocketImpUtil::listen(const bteso_SocketHandle::Handle& theSocket,
                                int                               backlog,
                                int                              *errorCode)
{
    int rc;

    BSLS_ASSERT(backlog >= 0);
    rc = ::listen(theSocket, backlog);
    int errorNumber = rc >= 0 ? 0 : bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

int bteso_SocketImpUtil::read(void                             *buffer,
                              const bteso_SocketHandle::Handle& socket,
                              int                               numBytes,
                              int                              *errorCode)
{
    int rc;

    BSLS_ASSERT(buffer);
    BSLS_ASSERT(numBytes >= 0);

    rc = recv(socket, (char *) buffer, numBytes, 0);
    int errorNumber = rc >= 0 ? 0 : bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : rc;
}

int bteso_SocketImpUtil::readv(const btes_Iovec          *iovecPtr,
                               const bteso_SocketHandle::Handle& socket,
                               int                               size,
                               int                              *errorCode)
{
    int rc;

    BSLS_ASSERT(iovecPtr);
    BSLS_ASSERT(size > 0);

#if defined(BTESO_PLATFORM_WIN_SOCKETS)
    DWORD bytesReceived;
    DWORD lpFlags = 0;
    rc = ::WSARecv(socket, (WSABUF *) iovecPtr, size,
                      &bytesReceived, &lpFlags, 0, 0);
    if (rc == 0) rc = bytesReceived;
    else rc = -1;
#else
    rc = ::readv(socket, (const ::iovec *) iovecPtr, size);
#endif

    int errorNumber = rc >= 0 ? 0 : bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : rc;
}

int bteso_SocketImpUtil::write(const bteso_SocketHandle::Handle& socket,
                               const void                       *buffer,
                               int                               numBytes,
                               int                              *errorCode)
{
    // Implementation notes: For non blocking IO, the send operation will
    // write out some or all of the buffer if possible.  However, since
    // the implementation of sockets may include internal buffers of certain
    // sizes and optimizations, the send function may return a "would block"
    // error or an smaller than expected number of bytes written even if it
    // appears as if the internal socket buffer has more room.  This is known
    // as the low water mark and may be implemented even if the equivalent
    // socket option is not available.
    // For the blocking IO case, the send operation may block until all or
    // part of the data is written.
    // No assumption should be made regarding the number of bytes that will be
    // written with a single call.
    int rc;

    BSLS_ASSERT(buffer);
    BSLS_ASSERT(numBytes >= 0);

    rc = ::send(socket, (char *)buffer, numBytes, 0);
    int errorNumber = rc >= 0 ? 0 : bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : rc;
}

int bteso_SocketImpUtil::writev(const bteso_SocketHandle::Handle& socket,
                                const btes_Ovec           *ovec,
                                int                               size,
                                int                              *errorCode)
{
    int rc;

    BSLS_ASSERT(ovec);
    BSLS_ASSERT(size > 0);

#if defined(BTESO_PLATFORM_WIN_SOCKETS)
    DWORD bytesSent;
    rc = ::WSASend(socket, (WSABUF *) ovec, size, &bytesSent, 0, 0, 0);
    if (rc == 0) rc = bytesSent;
    else rc = 0;
#else
    rc = ::writev(socket, (const ::iovec *) ovec, size);
#endif

    int errorNumber = rc > 0 ? 0 : bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : rc;
}

int bteso_SocketImpUtil::shutDown(const bteso_SocketHandle::Handle& socket,
                                  bteso_SocketImpUtil::ShutDownType how,
                                  int                              *errorCode)
{
    int rc;
    BSLMF_ASSERT(BTESO_SHUTDOWN_RECEIVE == 0 &&
                 BTESO_SHUTDOWN_SEND == 1 &&
                 BTESO_SHUTDOWN_BOTH == 2);

    BSLS_ASSERT((int) how >= BTESO_SHUTDOWN_RECEIVE
             && (int) how <= BTESO_SHUTDOWN_BOTH);

    rc = ::shutdown(socket, (int) how);
    int errorNumber = rc >= 0 ? 0 : bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

template <>
int bteso_SocketImpUtil_Imp<bteso_IPv4Address>::socketPair(
                          bteso_SocketHandle::Handle  *newSockets,
                          bteso_SocketImpUtil::Type    type,
                          int                          protocol,
                          int                         *errorCode)
{
    BSLS_ASSERT(newSockets);

    // Validate the parameters
    if (type != bteso_SocketImpUtil::BTESO_SOCKET_STREAM
    &&  type != bteso_SocketImpUtil::BTESO_SOCKET_DATAGRAM) {
        return -1;
    }

    if (type == bteso_SocketImpUtil::BTESO_SOCKET_STREAM) {
        // A pair of TCP sockets are created each connected to
        // each other via the loopback address and an anonymous
        // port.
        // The result will be a symmetric pair of connected
        // stream sockets.

        bteso_SocketHandle::Handle listenSocket;
        int classification = open(&listenSocket, type, protocol, errorCode);
        // Test for errors
        if (classification < 0) {
            return classification;
        }

        // Bind to localhost and anon port.
        bteso_IPv4Address localAnonAddress("127.0.0.1",
                                           bteso_IPv4Address::BTESO_ANY_PORT);
        classification = bteso_SocketImpUtil::bind(listenSocket,
                                                   localAnonAddress,
                                                   errorCode);
        // Test for errors
        if (classification < 0) {
            bteso_SocketImpUtil::close(listenSocket);
            return classification;
        }

        // Get the address for the client to connect to
        bteso_IPv4Address localAddress;
        classification = bteso_SocketImpUtil::getLocalAddress(&localAddress,
                                         listenSocket, errorCode);
        // Test for errors
        if (classification < 0) {
            bteso_SocketImpUtil::close(listenSocket);
            return classification;
        }

        // Listen for the client connection
        classification = bteso_SocketImpUtil::listen(listenSocket,
                                                     1, errorCode);
        // Test for errors
        if (classification < 0) {
            bteso_SocketImpUtil::close(listenSocket);
            return classification;
        }

        // Create the client socket
        bteso_SocketHandle::Handle clientSocket;
        classification = open(&clientSocket, type, protocol, errorCode);
        // Test for errors
        if (classification < 0) {
            bteso_SocketImpUtil::close(listenSocket);
            return classification;
        }

        // Connect to the listening socket (blocking)
        classification = bteso_SocketImpUtil::connect(clientSocket,
                                                      localAddress,
                                                      errorCode);
        // Test for errors
        if (classification < 0) {
            bteso_SocketImpUtil::close(listenSocket);
            bteso_SocketImpUtil::close(clientSocket);
            return classification;
        }

        // Accept the connection from the client
        bteso_SocketHandle::Handle serverSocket;
        classification = accept(&serverSocket, listenSocket, errorCode);
        // Test for errors
        if (classification < 0) {
            bteso_SocketImpUtil::close(listenSocket);
            bteso_SocketImpUtil::close(clientSocket);
            return classification;
        }

        // Close the listen socket
        bteso_SocketImpUtil::close(listenSocket);

        // Validate that both sockets are connected to each other.
        bteso_IPv4Address clientAddress, peerAddress;
        classification = bteso_SocketImpUtil::getLocalAddress(&clientAddress,
                                                              clientSocket,
                                                              errorCode);

        int classification2 = bteso_SocketImpUtil::getPeerAddress(&peerAddress,
                                             serverSocket, errorCode);

        if (classification < 0 || classification2 < 0 ||
            clientAddress != peerAddress) {
            bteso_SocketImpUtil::close(clientSocket);
            bteso_SocketImpUtil::close(serverSocket);
            return -1;
        }

        newSockets[0] = clientSocket;
        newSockets[1] = serverSocket;
    }
    else {
        // Two identical datagram sockets are created.  They are both
        // bound using 'bind' to anonymous ports on the loopback
        // address 'localhost'.  Using 'connect', each socket is
        // associated with each over preventing data from other sources
        // from being received.

        // Create the client socket
        bteso_SocketHandle::Handle clientSocket;
        int classification = open(&clientSocket, type, protocol, errorCode);
        // Test for errors
        if (classification < 0) {
            return classification;
        }

        // Bind to localhost and anon port.
        bteso_IPv4Address localAnonAddress("127.0.0.1",
                                           bteso_IPv4Address::BTESO_ANY_PORT);
        classification = bteso_SocketImpUtil::bind(clientSocket,
                                                   localAnonAddress,
                                                   errorCode);
        // Test for errors
        if (classification < 0) {
            bteso_SocketImpUtil::close(clientSocket);
            return classification;
        }

        // Get the address for the peer to connect to
        bteso_IPv4Address clientAddress;
        classification = bteso_SocketImpUtil::getLocalAddress(&clientAddress,
                                         clientSocket, errorCode);
        // Test for errors
        if (classification < 0) {
            bteso_SocketImpUtil::close(clientSocket);
            return classification;
        }

        // Create the server socket
        bteso_SocketHandle::Handle serverSocket;
        classification = open(&serverSocket, type, protocol, errorCode);
        // Test for errors
        if (classification < 0) {
            bteso_SocketImpUtil::close(clientSocket);
            return classification;
        }

        // Bind to localhost and anon port.
        classification = bind(serverSocket, localAnonAddress, errorCode);
        // Test for errors
        if (classification < 0) {
            bteso_SocketImpUtil::close(clientSocket);
            bteso_SocketImpUtil::close(serverSocket);
            return classification;
        }

        // Get the address for the peer to connect to
        bteso_IPv4Address serverAddress;
        classification = bteso_SocketImpUtil::getLocalAddress(&serverAddress,
                                         serverSocket, errorCode);
        // Test for errors
        if (classification < 0) {
            bteso_SocketImpUtil::close(clientSocket);
            bteso_SocketImpUtil::close(serverSocket);
            return classification;
        }

        // Associate sockets with each other.
        classification = bteso_SocketImpUtil::connect(serverSocket,
                                                      clientAddress,
                                                      errorCode);
        // Test for errors
        if (classification < 0) {
            bteso_SocketImpUtil::close(serverSocket);
            bteso_SocketImpUtil::close(clientSocket);
            return classification;
        }

        classification = bteso_SocketImpUtil::connect(clientSocket,
                                                      serverAddress,
                                                      errorCode);
        // Test for errors
        if (classification < 0) {
            bteso_SocketImpUtil::close(serverSocket);
            bteso_SocketImpUtil::close(clientSocket);
            return classification;
        }

        newSockets[0] = clientSocket;
        newSockets[1] = serverSocket;
    }

    return 0;
}

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
