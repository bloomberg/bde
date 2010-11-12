// bteso_socketimputil.h         -*-C++-*-
#ifndef INCLUDED_BTESO_SOCKETIMPUTIL
#define INCLUDED_BTESO_SOCKETIMPUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide platform-independent operations on sockets.
//
//@CLASSES:
// bteso_SocketImpUtil: platform-independent socket function suite
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@SEE_ALSO: bteso_socketoptutil, bteso_ioutil
//
//@DESCRIPTION: This component provides a suite of pure procedures to support
// various commonly used operations on sockets.  No provisions are made within
// this component for setting socket options (see 'bteso_socketoptutil' for
// option-setting functions and 'bteso_ioutil' for IO modes).  However,
// specific behavior is documented for sockets in blocking and non-blocking
// mode.
//
///SYNOPSIS
///--------
// The table below lists the functions of this component, and summarizes their
// behavior.  Where the behavior for sockets in blocking and non-blocking mode
// is different, the behavior is documented separately.
//..
//  +=========================================================================+
//  | OPERATION         | BLOCKING SOCKET         | NON-BLOCKING SOCKET       |
//  +=========================================================================+
//  |                   |                                                     |
//  | startup           |              initialize environment                 |
//  |                   |                                                     |
//  +-------------------------------------------------------------------------+
//  |                   |                                                     |
//  | open              |                  open new socket                    |
//  |                   |                                                     |
//  +-------------------------------------------------------------------------+
//  |                   |                                                     |
//  | bind              |          bind socket to a local address             |
//  |                   |                                                     |
//  +-------------------------------------------------------------------------+
//  |                   |              listen passively for                   |
//  | listen            |              connection requests;                   |
//  |                   |              specify the queue size                 |
//  +-------------------------------------------------------------------------+
//  |                   | accept request from the |                           |
//  | accept            | queue if available,     | accept request from the   |
//  |                   | block otherwise         |        queue              |
//  +-------------------------------------------------------------------------+
//  |                   | connect to the specified|  return immediately with  |
//  | connect           | peer; block until       |  an error indicating that |
//  |                   | connection              |  the call would block     |
//  +-------------------------------------------------------------------------+
//  |                   |                                                     |
//  | getLocalAddress   |    get address of local socket                      |
//  |                   |                                                     |
//  +-------------------------------------------------------------------------+
//  |                   |                                                     |
//  | getPeerAddress    |    get address of peer socket                       |
//  |                   |                                                     |
//  +-------------------------------------------------------------------------+
//  |                   | read data if available, |                           |
//  | read              | block otherwise         | read data if available    |
//  |                   |                         |                           |
//  +-------------------------------------------------------------------------+
//  |                   | read data if available, |                           |
//  | readFrom          | block otherwise;        | read data if available;   |
//  |                   | load address and port   | load address and port     |
//  +-------------------------------------------------------------------------+
//  |                   | write data if possible, |                           |
//  | write             | block otherwise         | write data if possible    |
//  |                   |                         |                           |
//  +-------------------------------------------------------------------------+
//  |                   | write data if possible, | write data if possible;   |
//  | writeTo           | block otherwise;        | specify address, port     |
//  |                   | specify address, port   |                           |
//  +-------------------------------------------------------------------------+
//  |                   |                                                     |
//  | shutDown          |                shut down the connection             |
//  |                   |                                                     |
//  +-------------------------------------------------------------------------+
//  |                   |                                                     |
//  | close             |                  close the socket                   |
//  |                   |                                                     |
//  +-------------------------------------------------------------------------+
//  |                   |                                                     |
//  | cleanup           |               clean up the environment              |
//  |                   |                                                     |
//  +-------------------------------------------------------------------------+
//..
//
///Note About Transport Layers
///---------------------------
// The utilities in this component explicitly assume that the sockets will be
// using the Network Layer protocol, but no assumptions are made regarding the
// transport layer, other than that it is consistent with the address type
// specified as the template parameter.  Individual functions may behave
// differently depending on the transport layer in use.  It is not possible to
// document every eventuality (since this component supports transport layer
// protocols that have not yet been written), and so only the most common
// cases (i.e., the behavior of TCP and UDP sockets) are discussed in the
// function-level documentation.
//
///Signals
///-------
// Signals may be generated in response to socket operations including the
// SIGPIPE and SIGIO signals.  The processing of these signals is outside
// the scope of this component.  The SIGPIPE signal will be generated on UNIX
// like systems when a write operation is performed on a socket that has no
// reader.
//
///Addresses
///---------
// Some functions are templatized by 'ADDRESS', where 'ADDRESS' is a network
// address type.   The address type implicitly specifies the native socket
// domain (e.g., AF_INET for IPv4 addresses).  The address type for IPv4
// addresses is 'bteso_IPv4Address'.  After a socket has been created with
// a particular address type,  all further functions on this socket taking
// the 'ADDRESS' parameter must use the same address type.
//
///Errors
///------
// On success, all functions return a non-negative integer value.  On errors,
// these functions return a error classification as a negative integer as
// defined 'bteso_SocketHandle'.  Additionally, each function has an optional
// parameter 'errorCode' which returns the platform-specific error code.
// Platform specific error codes map onto the error classifications.  When
// a mapping does not exist, the classification is
// 'bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED'.
// This component does not use the 'bteso_SocketHandle::BTESO_ERROR_EOF' or the
// 'bteso_SocketHandle::BTESO_ERROR_TIMEDOUT' classifications.
//
///Interrupted system calls 'bteso_SocketHandle::BTESO_ERROR_INTERRUPTED'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// On some platforms, calls can be interrupted without completing the requested
// operation.  For read or write calls, when some data was transferred the
// number of bytes transferred is returned and 'ERROR_INTERRUPTED' otherwise.
// For other calls 'ERROR_INTERRUPTED' is returned.  When 'ERROR_INTERRUPTED'
// is returned, the function call did not perform its requested operation and
// the operation can be repeated.  An exception is the 'connect' operation
// which should not be repeated.  In this case, the state of the socket is
// undefined and should be closed.  The platform specific error code on UNIX is
// EINTR.
//
///Would Block 'bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// When a socket is in non-blocking mode and the call cannot complete without
// blocking,  the error classification 'BTESO_ERROR_WOULDBLOCK' is returned.
// The platform specific error codes on UNIX are 'EINPROGRESS', 'EAGAIN' or
// 'EWOULDBLOCK'.  On Windows, the platform specified error codes are
// 'WSAEINPROGRESS' or 'WSAEWOULDBLOCK'.
//
///Connection Dead 'bteso_SocketHandle::BTESO_ERROR_CONNDEAD'
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// This error is returned when an attempt to setup a connection has failed or
// an operation was attempted on an existing connection that has been
// terminated.
//
//
///Startup and Cleanup
///-------------------
// Prior to any socket operations on a process, the 'startup' function must
// be called.  Although some socket implementations (e.g., UNIX) do not
// require any initialization, this function must be called to ensure
// maximum portability.  After completing all socket operations, the
// 'cleanup' function should be called.  The state of any open sockets after
// cleanup is undefined.  The 'startup' function may be invoked any number
// of times.  However, the 'cleanup' function must invoked the same number
// of times.
//
///USAGE
//------
// This component supports inter-process communication functionality, and so
// possible usage within a single process would consist of half of a dialog,
// either the "client" or the "server".  Note that the 'shutDown' operation
// is optional.  The following diagram shows a typical sequence of operations
// on the client and the server processes, for sockets using the TCP protocol:
//..
//                                                 TCP SERVER
//                                               +-----------+ prepares
//                                               | startup() | sockets
//                                               +-----------+ environment
//                                                     |
//                                               +-----------+ gets a
//                                               |   open()  | socket
//                                               +-----------+
//                                                     |
//                                               +-----------+ binds this
//                                               |   bind()  | socket to a
//                                               +-----------+ port number
//                TCP CLIENT                           |
//                                               +-----------+ queues up
//                                               |  listen() | connection
//                                               +-----------+ requests
//                                                     |
//                                               +-----------+ accepts
//                                               |  accept() | connection
//  prepares     +-----------+                   +-----------+ requests from
//  sockets      | startup() |                         |       the queue
//  environment  +-----------+                         |
//                     |                     blocks until a connection
//    gets a     +-----------+                  request is accepted
//    socket     |   open()  |                         |
//               +-----------+                         |
//                     |                               | connection request
// establishes a +-----------+                         | "accepted" by server,
// connection    |  connect()|------------------------>| creates a new socket
// request with  +-----------+                         | socket
// the server          |                               |
//               +-----------+                   +-----------+
//     writes    |   write() |------------------>| read()    | reads client
//   requests    +-----------+                   +-----------+ requests,
//     and             :                               :       process them
//    reads         :                               :       and writes
//   replies     +-----------+                   +-----------+ back replies
//               | read()    |<------------------|   write() | until client
//               +-----------+                   +-----------+ closes the
//                     |                               |       connection
//  closes the   +-----------+                   +-----------+
//  connection   | shutDown()|-------------------| shutDown()|
//  when done    +-----------+                   +-----------+
//                     |                               |       connection
//  closes the   +-----------+                   +-----------+
//  connection   |   close() |-------------------|   close() |
//  when done    +-----------+                   +-----------+
//                     |                               |       connection
//  cleans up    +-----------+                   +-----------+ cleans up
//  sockets      | cleanup() |                   | cleanup() | sockets
//  environment  +-----------+                   +-----------+ environment
//..
///Typical Server Usage
///- - - - - - - - - -
// The following snippets of code illustrate a simple single user echo server
// implemented using 'bteso_socketimputil'.  Note that the server binds to and
// the client connects to the specified IP_ADDR.
//..
//      bteso_SocketHandle::Handle serverSocket, sessionSocket;
//      const int                  BACKLOG      = 32;
//      const int                  RECEIVE_SIZE = 32;
//      char                       readBuffer[RECEIVE_SIZE];
//      int                        rc, errCode = 0;
//
//      rc = bteso_SocketImpUtil::startup(&errCode);      assert(0 == rc);
//      rc = bteso_SocketImpUtil::open<bteso_IPv4Address>(
//                                    &serverSocket,
//                                    bteso_SocketImpUtil::BTESO_SOCKET_STREAM,
//                                    &errCode);
//                                                        assert(0 == rc);
//      rc = bteso_SocketImpUtil::bind<bteso_IPv4Address>(serverSocket,
//                                                        IP_ADDR,
//                                                        &errCode);
//                                                        assert(0 == rc);
//      rc = bteso_SocketImpUtil::listen(serverSocket,
//                                       BACKLOG,
//                                       &errCode);       assert(0 == rc);
//      rc = bteso_SocketImpUtil::accept<bteso_IPv4Address>(&sessionSocket,
//                                                          serverSocket,
//                                                          &errCode);
//                                                        assert(0 == rc);
//      do {
//          // Echo each packet back until the client disconnects
//          rc = bteso_SocketImpUtil::read(readBuffer,
//                                         sessionSocket,
//                                         RECEIVE_SIZE,
//                                         &errCode);
//          if (rc > 0) {
//              bteso_SocketImpUtil::write(sessionSocket,
//                                         readBuffer,
//                                         rc,
//                                         &errCode);
//          }
//      } while (rc > 0);
//      rc = bteso_SocketImpUtil::shutDown(
//                                    sessionSocket,
//                                    bteso_SocketImpUtil::BTESO_SHUTDOWN_BOTH,
//                                    &errCode);          assert(0 == rc);
//      rc = bteso_SocketImpUtil::close(sessionSocket,
//                                      &errCode);        assert(0 == rc);
//      rc = bteso_SocketImpUtil::close(serverSocket,
//                                      &errCode);        assert(0 == rc);
//      rc = bteso_SocketImpUtil::cleanup(&errCode);      assert(0 == rc);
//..
///Typical Client Usage
///- - - - - - - - - -
// The following snippets of code illustrate a simple client implemented using
// 'bteso_socketutil'.
//..
//      bteso_SocketHandle::Handle  sendSocket;
//      const int                   RECEIVE_SIZE = 32;
//      char                        buffer[RECEIVE_SIZE];
//      int                         rc, errorCode = 0;
//      const char                 *DATA = "Is it raining in London now?";
//      const int                   DATA_SIZE = bsl::strlen(DATA) + 1;
//
//      rc = bteso_SocketImpUtil::startup(&errorCode);       assert(0 == rc);
//      rc = bteso_SocketImpUtil::open<bteso_IPv4Address>(
//                                    &sendSocket,
//                                    bteso_SocketImpUtil::BTESO_SOCKET_STREAM,
//                                    &errorCode);
//                                                           assert(0 == rc);
//      rc = bteso_SocketImpUtil::connect<bteso_IPv4Address>(sendSocket,
//                                                           IP_ADDR,
//                                                           &errorCode);
//                                                           assert(0 == rc);
//      rc = bteso_SocketImpUtil::write(sendSocket,
//                                      DATA,
//                                      DATA_SIZE,
//                                      &errorCode);   assert(DATA_SIZE == rc);
//      rc = bteso_SocketImpUtil::read(buffer,
//                                     sendSocket,
//                                     RECEIVE_SIZE,
//                                     &errorCode);    assert(DATA_SIZE == rc);
//
//      assert(0 == bsl::strcmp(DATA, buffer));
//
//      rc = bteso_SocketImpUtil::shutDown(
//                                    sendSocket,
//                                    bteso_SocketImpUtil::BTESO_SHUTDOWN_BOTH,
//                                    &errorCode);          assert(0 == rc);
//      rc = bteso_SocketImpUtil::close(sendSocket,
//                                      &errorCode);         assert(0 == rc);
//      rc = bteso_SocketImpUtil::cleanup(&errorCode);       assert(0 == rc);
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BTESO_SOCKETHANDLE
#include <bteso_sockethandle.h>
#endif

#ifndef INCLUDED_BTESO_PLATFORM
#include <bteso_platform.h>
#endif

#ifndef INCLUDED_BTES_IOVEC
#include <btes_iovec.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BTESO_PLATFORM__WIN_SOCKETS
    #ifndef INCLUDED_WINSOCK2
    #include <winsock2.h>                    // for WSABUF
    #define INCLUDED_WINSOCK2
    #endif

    #ifndef INCLUDED_BSL_ALGORITHM
    #include <bsl_algorithm.h>                     // for copy_n
    #endif
#endif

#ifdef BTESO_PLATFORM__BSD_SOCKETS

    #ifndef INCLUDED_SYS_TYPES
    #include <sys/types.h>
    #define INCLUDED_SYS_TYPES
    #endif

    #ifndef INCLUDED_SYS_SOCKET
    #include <sys/socket.h>
    #define INCLUDED_SYS_SOCKET
    #endif

    #ifndef INCLUDED_NETINET_IN
    #include <netinet/in.h>
    #define INCLUDED_NETINET_IN
    #endif

    #ifndef INCLUDED_BSL_C_STRING
    #include <bsl_c_string.h> // memset
    #endif

#endif

namespace BloombergLP {

                          // ==========================
                          // struct bteso_SocketImpUtil
                          // ==========================

struct bteso_SocketImpUtil {
    // This class provides a suite of platform-independent pure procedures that
    // manipulate operating-system-level sockets.  All methods take
    // an 'errorCode' as an optional parameter, which is loaded with
    // platform-specific error number if an error occurs during the operation.
    // All functions return a negative value on error.  This value defined in
    // 'bteso_SocketHandle' classifies the error and is not platform-specific.

    enum Type {
        // When creating a socket with the 'open' or 'socketPair' operations,
        // this indicates the type of socket(s) to be created.
        BTESO_SOCKET_STREAM = SOCK_STREAM,
            // Provides sequenced, reliable, two-way, connection-based byte
            // streams.  An out-of-band data transmission mechanism may be
            // supported.
        BTESO_SOCKET_DATAGRAM = SOCK_DGRAM,
            // Supports datagrams (connectionless, unreliable messages of a
            // fixed maximum length).
        BTESO_SOCKET_RAW = SOCK_RAW
            // Provides raw network protocol access.
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , SOCKET_STREAM   = BTESO_SOCKET_STREAM
      , SOCKET_DATAGRAM = BTESO_SOCKET_DATAGRAM
      , SOCKET_RAW      = BTESO_SOCKET_RAW
#endif
    };

    enum ShutDownType {
        // Indicates how to shut down the socket with 'shutDown' operation.
        BTESO_SHUTDOWN_RECEIVE,
            // shut down the receive half of the full-duplex connection
            // associated with the specified 'socket', leaving its send half
            // unaffected.
        BTESO_SHUTDOWN_SEND,
            // shut down the send half of the full-duplex connection associated
            // with the specified 'socket', leaving its receive half
            // unaffected.
        BTESO_SHUTDOWN_BOTH
            // shut down both halves of the full-duplex connection associated
            // with the specified 'socket'.
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , SHUTDOWN_RECEIVE = BTESO_SHUTDOWN_RECEIVE
      , SHUTDOWN_SEND    = BTESO_SHUTDOWN_SEND
      , SHUTDOWN_BOTH    = BTESO_SHUTDOWN_BOTH
#endif
    };

    template <class ADDRESS>
    static int accept(bteso_SocketHandle::Handle       *newSocket,
                      const bteso_SocketHandle::Handle& socket);
    template <class ADDRESS>
    static int accept(bteso_SocketHandle::Handle       *newSocket,
                      const bteso_SocketHandle::Handle& socket,
                      int                              *errorCode);
        // Accept a connection request on the specified listening 'socket'.  On
        // success, load the specified 'newSocket' with the handle for the
        // socket created for the newly established connection.  Load the
        // optionally specified 'errorCode' with the platform-specific error
        // number on error.  Return 0 on success and a negative value otherwise
        // representing an error classification.  Note that if 'socket' is in
        // non-blocking mode and there is no pending connection request
        // available, this call returns immediately with an error status, but
        // if 'socket' is in blocking mode, this call blocks until either it
        // succeeds or an error occurs.  Also note that, when created,
        // 'newSocket' has exactly the same socket options as 'socket'.

    template <class ADDRESS>
    static int accept(bteso_SocketHandle::Handle        *newSocket,
                      ADDRESS                           *peerAddress,
                      const bteso_SocketHandle::Handle&  socket);
    template <class ADDRESS>
    static int accept(bteso_SocketHandle::Handle        *newSocket,
                      ADDRESS                           *peerAddress,
                      const bteso_SocketHandle::Handle&  socket,
                      int                               *errorCode);
        // Accept a connection request on the specified listening 'socket'.  On
        // success, load the specified 'newSocket' with the handle for the
        // socket created for the newly established connection, and also load
        // the specified 'peerAddress' with the address of the newly connected
        // client.  Load the optionally specified 'errorCode' with the
        // platform-specific error number on error.  Return 0 on success,
        // and a negative value otherwise representing an error classification.
        // Note that if 'socket' is in non-blocking mode and there is no
        // pending connection request available, this call returns immediately
        // with an error status, but if 'socket' is in blocking mode, this call
        // blocks until either it succeeds or an error occurs.  Also note that,
        // when created, 'newSocket' has exactly the same socket options as
        // 'socket'.  If a connection has been closed before the accept call is
        // made, the call may fail and
        // 'bteso_SocketHandle::BTESO_ERROR_CONNDEAD' will be returned.

    template <class ADDRESS>
    static int bind(const bteso_SocketHandle::Handle& socket,
                    const ADDRESS&                    address);
    template <class ADDRESS>
    static int bind(const bteso_SocketHandle::Handle& socket,
                    const ADDRESS&                    address,
                    int                              *errorCode);
        // Associate the specified 'address' with the specified 'socket'.
        // Load the optionally specified 'errorCode' with a platform-specific
        // error number on error.  Return 0 on success and a negative value
        // otherwise representing an error classification.  Note that
        // (a) typically, servers call 'bind' with a well-known port number
        // since they are known by their port numbers, and (b) typically,
        // stream based clients allow the kernel to choose a temporary
        // (ephemeral) port by not invoking this operation at all.  Note also
        // that a port number 0 can be specified so that an address can be
        // bound to a socket without binding a specific port number.

    static int cleanup(int *errorCode = 0);
        // Clean up the environment created for sockets-based activity.  All
        // open sockets should be explicitly closed prior to this operation.
        // No assumptions should be made about the state of any remaining
        // open socket after this operation.  Load the optionally specified
        // 'errorCode' with a platform-specific error number on error.  Return
        // 0 on success and a negative value otherwise representing an error
        // classification.

    static int close(const bteso_SocketHandle::Handle& socket,
                     int                              *errorCode = 0);
        // Close the specified 'socket' and release all the resources
        // associated with it.  If this socket is the last reference to an
        // existing connection, this connection will be terminated.
        // Load the optionally specified 'errorCode' with a platform-specific
        // error number on error.  Return 0 on success, and a negative value
        // otherwise representing an error classification.  Note that although
        // call to 'close' returns immediately, by default, the kernel tries to
        // send all untransmitted data before actually closing the connection.
        // This default behavior can be altered by changing the socket linger
        // options (see bteso_socketoptutil).

    template <class ADDRESS>
    static int connect(const bteso_SocketHandle::Handle& socket,
                       const ADDRESS&                    address);
    template <class ADDRESS>
    static int connect(const bteso_SocketHandle::Handle& socket,
                       const ADDRESS&                    address,
                       int                              *errorCode);
        // For a stream type socket, connect the specified 'socket' with the
        // server process listening at the specified 'address'.  Load the
        // optionally specified 'errorCode' with a platform-specific error
        // number on error.  Return 0 on success, and a negative value
        // otherwise representing an error classification.  If the socket is in
        // blocking mode, the call will block until the connection is
        // established or an error occurs.  If the socket is in non-blocking
        // mode, the call may complete immediately returning
        // 'bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK' indicating that the
        // operation has been initiated but may not have
        // completed.  In this case the 'select' function can be used to
        // determine when the connection has completed.  For a datagram socket,
        // associate the specified 'address' with the specified 'socket'.  If
        // no address is specified for a send operation, the specified
        // 'address' is used.  For receive operations, only data from the
        // specified 'address' is received.

    template <class ADDRESS>
    static int getLocalAddress(ADDRESS                          *localAddress,
                               const bteso_SocketHandle::Handle& socket);
    template <class ADDRESS>
    static int getLocalAddress(ADDRESS                          *localAddress,
                               const bteso_SocketHandle::Handle& socket,
                               int                              *errorCode);
        // Load the local address and port number associated with the specified
        // 'socket' in the specified 'localAddress'.  The result is undefined
        // if the socket was not initially created with the specified ADDRESS
        // type.  Load the optionally specified 'errorCode' with a
        // platform-specific error number on error.  Return 0 on success, and
        // a negative value otherwise representing an error classification.

    template <class ADDRESS>
    static int getPeerAddress(ADDRESS                           *peerAddress,
                              const bteso_SocketHandle::Handle&  socket);
    template <class ADDRESS>
    static int getPeerAddress(ADDRESS                           *peerAddress,
                              const bteso_SocketHandle::Handle&  socket,
                              int                               *errorCode);
        // Load the specified 'peerAddress' with the address and port number
        // of the peer connected to the specified 'socket'.  Load the
        // optionally specified 'errorCode' with a platform-specific error
        // number on error.  Return 0 on success, and a negative value
        // otherwise representing an error classification.

    static int listen(const bteso_SocketHandle::Handle& socket,
                      int                               queueSize,
                      int                              *errorCode = 0);
        // Listen passively to the incoming connection requests on the
        // specified stream 'socket'.  Also set the maximum number of
        // pending connection requests to be queued before "accepted" to the
        // specified 'queueSize'.  If the backlog value is (system dependent)
        // 'SOMAXCONN', then the backlog will be set to the maximum
        // "reasonable" value.  There is no standard provision to find out the
        // actual backlog value.  Load the optionally specified 'errorCode'
        // with a platform-specific error number on error.  Return 0 on
        // success, and a negative value otherwise representing an error
        // classification.  The behavior is undefined unless 0 < queuesize.

    template <class ADDRESS>
    static int open(bteso_SocketHandle::Handle  *newSocket,
                    bteso_SocketImpUtil::Type    type);
    template <class ADDRESS>
    static int open(bteso_SocketHandle::Handle  *newSocket,
                    bteso_SocketImpUtil::Type    type,
                    int                         *errorCode);
        // Initialize the specified 'newSocket' with a newly created socket of
        // the specified 'type'.  Load the optionally specified 'errorCode'
        // with the platform-specific error number on error.  Return 0 on
        // success, and a negative value otherwise representing an error
        // classification.

    template <class ADDRESS>
    static int open(bteso_SocketHandle::Handle  *newSocket,
                    bteso_SocketImpUtil::Type    type,
                    int                          protocol);
    template <class ADDRESS>
    static int open(bteso_SocketHandle::Handle  *newSocket,
                    bteso_SocketImpUtil::Type    type,
                    int                          protocol,
                    int                         *errorCode);
        // Initialize the specified 'newSocket' with a newly created socket of
        // the specified 'type' and the specified 'protocol'.  Load the
        // optionally specified 'errorCode' with the platform-specific error
        // number on error.  Return 0 on success, and a negative value
        // otherwise representing an error classification.

    static int read(void                             *buffer,
                    const bteso_SocketHandle::Handle& socket,
                    int                               numBytes,
                    int                              *errorCode = 0);
        // Receive up to a maximum of the specified 'numBytes' bytes from the
        // specified 'socket' in the specified 'buffer'.  Load the optionally
        // specified 'errorCode' with the platform-specific error number on
        // error.  Return the non-negative number of bytes read on success, 0
        // on EOF (when 0 < numBytes), and a negative value otherwise
        // representing an error classification.  The behavior is undefined
        // unless 0 <= numBytes and 'buffer' refers to at least 'numBytes'
        // valid memory locations.  Note that if 'socket' is in blocking mode,
        // this call blocks until (a) data is read, (b) an EOF indication
        // occurs, or (c) an error occurs.  Note that if 'numBytes' is 0, this
        // call returns immediately, with no effect on 'buffer'.

    template <class ADDRESS>
    static int readFrom(ADDRESS                           *fromAddress,
                        void                              *buffer,
                        const bteso_SocketHandle::Handle&  socket,
                        int                                numBytes);
    template <class ADDRESS>
    static int readFrom(ADDRESS                           *fromAddress,
                        void                              *buffer,
                        const bteso_SocketHandle::Handle&  socket,
                        int                                numBytes,
                        int                               *errorCode);
        // Receive up to a maximum of the specified 'numBytes' bytes from the
        // specified 'socket' in the specified 'buffer' and load the address
        // and port number of the source of the data in 'fromAddress'.
        // Load the optionally specified 'errorCode' with the
        // platform-specific error number on error.
        // Return the non-negative number of bytes read on success, 0 on EOF
        // or when 0 == numBytes, and a negative value otherwise representing
        // an error classification.  The 'socket' must not be connected.  The
        // 'fromAddress' parameter will be ignored for connection-oriented
        // sockets.  For message-oriented sockets data is extracted from
        // the first enqueued message, up to the size of 'buffer'.  If the
        // datagram or message is larger than the buffer, it is filled with
        // the first part of the datagram, and an error is generated.
        // For unreliable protocols (e.g.,  UDP) the excess data is lost.
        // The behavior is undefined unless 0 <= numBytes and 'buffer' refers
        // to at least 'numBytes' valid memory locations.
        // Note that if 'socket' is in blocking mode, this call blocks until
        // (a) data is read, (b) an EOF indication occurs, or (c) an error
        // occurs.  Note also that if 'numBytes' is 0, this call returns
        // immediately, with no effect on 'buffer'.

    static int readv(const btes_Iovec          *iovec,
                     const bteso_SocketHandle::Handle& socket,
                     int                               numBuffs,
                     int                              *errorCode = 0);
        // Receive from the specified 'socket' into the buffers specified by
        // 'iovec' where the specified 'numBuffs' is the number of buffers.
        // Load the optionally specified 'errorCode' with the
        // platform-specific error number on error.  Return the non-negative
        // number of bytes read on success, 0 on EOF or when 0 == numBytes,
        // and a negative value otherwise representing an error classification.
        // The behavior is undefined unless 0 < numBuffs and 'iovec' refers to
        // buffers at valid memory locations.  Note that if 'socket' is in
        // blocking mode, this call blocks until (a) data is read, (b) an EOF
        // indication occurs, or (c) an error occurs.

    template <class ADDRESS>
    static int socketPair(bteso_SocketHandle::Handle  *newSockets,
                          bteso_SocketImpUtil::Type    type);
    template <class ADDRESS>
    static int socketPair(bteso_SocketHandle::Handle  *newSockets,
                          bteso_SocketImpUtil::Type    type,
                          int                         *errorCode);
        // Create a pair of connected sockets of the specified 'type'.  On
        // success, the specified 'newSockets' will point to the handles for
        // the new sockets.  Load the optionally specified 'errorCode' with the
        // platform-specific error number on error.  Return 0 on success, and
        // a negative value otherwise representing an error classification.

    template <class ADDRESS>
    static int socketPair(bteso_SocketHandle::Handle  *newSockets,
                          bteso_SocketImpUtil::Type    type,
                          int                          protocol);
    template <class ADDRESS>
    static int socketPair(bteso_SocketHandle::Handle  *newSockets,
                          bteso_SocketImpUtil::Type    type,
                          int                          protocol,
                          int                         *errorCode);
        // Create a pair of connected sockets of the specified 'type' and
        // 'protocol'.  On success, the specified 'newSockets' will point to
        // the handles for the new sockets.  Load the optionally specified
        // 'errorCode' with the platform-specific error number on error.
        // Return 0 on success, and a negative value otherwise representing an
        // error classification.  The behavior is undefined unless
        // 'newSockets' refers to at least 2 writable memory locations.

    static int write(const bteso_SocketHandle::Handle& socket,
                     const void                       *buffer,
                     int                               numBytes,
                     int                              *errorCode = 0);
        // Send up to a maximum of the specified 'numBytes' bytes using the
        // specified 'socket' from the specified 'buffer'.  Load the optionally
        // specified 'errorCode' with the platform-specific error number on
        // error.  Return the non-negative number of bytes written on success,
        // and a negative value otherwise representing an error classification.
        // The behavior is undefined unless 0 <= numBytes and 'buffer' refers
        // to at least 'numBytes' valid memory locations.  Note that if
        // 'socket' is in blocking mode, this call blocks until either data is
        // written or an error occurs.

    template <class ADDRESS>
    static int writeTo(const bteso_SocketHandle::Handle&  socket,
                       const ADDRESS&                     toAddress,
                       const void                        *buffer,
                       int                                numBytes);
    template <class ADDRESS>
    static int writeTo(const bteso_SocketHandle::Handle&  socket,
                       const ADDRESS&                     toAddress,
                       const void                        *buffer,
                       int                                numBytes,
                       int                               *errorCode);
        // Send up to a maximum of the specified 'numBytes' bytes using the
        // specified 'socket' from the specified 'buffer' to the destination
        // described by the specified 'toAddress'.  Load the optionally
        // specified 'errorCode' with the platform-specific error number on
        // error.  Return the non-negative number of bytes written on success,
        // and a negative value otherwise representing an error classification.
        // If 'socket' is a connectionless socket that had previously been
        // connected to a specific destination, the 'toAddress' will override
        // the destination (if different) for this call only.  If 'socket'
        // is a connection-oriented socket, 'toAddress' are ignored, making
        // the function equivalent to 'write'.  For message-oriented sockets,
        // care must be taken not to exceed the maximum packet size of the
        // underlying subnets; if the data is too long to pass atomically
        // through the underlying protocol, an error indication will be
        // returned and no data will be sent.  The behavior is undefined unless
        // 0 <= numBytes and 'buffer' refers to at least 'numBytes' valid
        // memory locations.  Note that if 'socket' is in blocking mode, this
        // call blocks until either data is written or an error occurs.  Note
        // also that if 'numBytes' is 0, this call returns immediately.

    static int writev(const bteso_SocketHandle::Handle&  socket,
                      const btes_Ovec            *ovec,
                      int                                numBuffs,
                      int                               *errorCode = 0);
        // Send to the specified 'socket' from the buffers specified by 'ovec'
        // where 'numBuffs' is the number of buffers.  Load the optionally
        // specified 'errorCode' with the platform-specific error number on
        // error.  Return the non-negative number of bytes written on success,
        // and a negative value otherwise representing an error classification.
        // The behavior is undefined unless 0 < numBuffs and 'iovec' refers to
        // buffers at valid writable memory locations.  Note that if 'socket'
        // is in blocking mode, this call blocks until either data is written
        // or an error occurs.

    template <class ADDRESS>
    static int writevTo(const bteso_SocketHandle::Handle&  socket,
                        const ADDRESS&                     toAddress,
                        const btes_Ovec                   *ovec,
                        int                                numBuffs,
                        int                               *errorCode = 0);
        // Send to the specified 'socket' from the buffers specified
        // by 'ovec' where 'numBuffs' is the number of buffers to the
        // destination described by the specified 'toAddress'.  Load
        // the optionally specified 'errorCode' with the
        // platform-specific error number on error.  Return the
        // non-negative number of bytes written on success and a
        // negative value otherwise representing an error
        // classification.  For message-oriented sockets, care must be
        // taken not to exceed the maximum packet size of the
        // underlying subnets; if the data is too long to pass
        // atomically through the underlying protocol, an error
        // indication will be returned and no data will be sent.  The
        // behavior is undefined unless the specified 'socket' is not
        // connected, 0 < numBuffs and 'iovec' refers to buffers at
        // valid writable memory locations.  Note that if 'socket' is
        // in blocking mode, this call blocks until either data is
        // written or an error occurs.

    static int shutDown(const bteso_SocketHandle::Handle& socket,
                        bteso_SocketImpUtil::ShutDownType value,
                        int                              *errorCode = 0);
        // Shutdown the specified send and/or receive direction of the
        // full-duplexed connection associated with this socket.  Return 0 on
        // success, and a non-zero value otherwise representing an error
        // classification.  A 'value' of 'BTESO_SHUTDOWN_RECEIVE' or
        // 'BTESO_SHUTDOWN_SEND' will close the receive or send stream,
        // respectively, leaving the other stream unaffected.  A 'value' of
        // 'BTESO_SHUTDOWN_BOTH' will close both.  Once the receive(send)
        // stream has been closed, any attempted read(write) operations will
        // fail.  Any data received after successfully shutting down the
        // receive stream will be acknowledged but silently discarded.  Note
        // that this function is typically used to transmit an EOF indication
        // to the peer.  Load the optionally specified 'errorCode' with the
        // platform-specific error number on error.

    static int startup(int *errorCode = 0);
        // Initialize the environment for sockets-based activity.  Load the
        // optionally specified 'errorCode' with the platform-specific error
        // number on error.  Return 0 on success, and a negative value
        // otherwise representing an error classification.
};

                          // ===============================
                          // struct bteso_SocketImpUtil_Util
                          // ===============================

struct bteso_SocketImpUtil_Util {
    // This struct provides a namespace for typedefs, enums and static
    // member functions used by 'bteso_SocketImpUtil'.

    // The type for the length of the sockaddr structure differs by platform
    // create a type that is platform independent here.
#if defined(BTESO_PLATFORM__WIN_SOCKETS) \
    || defined(BSLS_PLATFORM__OS_HPUX)
    typedef int ADDRLEN_T;

#else
    typedef socklen_t ADDRLEN_T;
#endif

#ifdef BTESO_PLATFORM__WIN_SOCKETS
    enum {
        BTESO_INVALID_SOCKET_HANDLE = INVALID_SOCKET
#else
    enum {
        BTESO_INVALID_SOCKET_HANDLE = -1
#endif
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , INVALID_SOCKET_HANDLE = BTESO_INVALID_SOCKET_HANDLE
#endif
    };
        // The platform-specific value returned by a socket call that creates
        // a new socket indicating failure to create a socket.

    static int mapErrorCode(int errorNumber);
        // Return the error classification as defined in 'bteso_SocketHandle'
        // mapped from specified platform-specific socket operation
        // 'errorCode'.

    static int getErrorCode(void);
        // Return the platform-specific socket operation error code.

    static bool isValid(const bteso_SocketHandle::Handle& handle);
        // Return 'true' if the specified socket 'handle' returned from a
        // socket operation creating a new socket represents a valid socket
        // handle, and 'false' otherwise.
};

inline bool bteso_SocketImpUtil_Util::isValid(
      const bteso_SocketHandle::Handle& socket)
{
    return socket != BTESO_INVALID_SOCKET_HANDLE;
}

                          // ==================================
                          // struct bteso_SocketImpUtil_Address
                          // ==================================

template <class ADDRESS> struct bteso_SocketImpUtil_Address {
    // Encapsulate the platform-specific socket address for a particular
    // non platform-specific socket address.  Provide a mapping between
    // objects of the two types.  This structure also provides the
    // domain type for socket and socketpair calls.
};

template <>
struct bteso_SocketImpUtil_Address<class bteso_IPv4Address> {
    // Encapsulate the 'sockaddr_in' structure and provide a mapping
    // to the equivalent 'bteso_IPv4Address'.

    sockaddr_in d_address;

    enum {
        SocketDomain = AF_INET
    };

    // CREATORS
    bteso_SocketImpUtil_Address() { }

    bteso_SocketImpUtil_Address(const bteso_IPv4Address& addr)
    {
        BSLMF_ASSERT(sizeof(int) == sizeof(in_addr));

        // Obtain the ip address.  The address is already in network
        // byte order.
        int ipAddr = addr.ipAddress();
        if (bteso_IPv4Address::BTESO_ANY_ADDRESS != INADDR_ANY) {
            if (ipAddr == bteso_IPv4Address::BTESO_ANY_ADDRESS) {
                ipAddr = INADDR_ANY;
            }
        }
        d_address.sin_addr = *((struct in_addr *) &ipAddr);

        // The port number must be stored in network byte order.
        d_address.sin_port   = htons((unsigned short)addr.portNumber());
        d_address.sin_family = SocketDomain;

        // Set sin_zero to zero.
        enum { sizezero = sizeof(d_address.sin_zero) };
        struct zerobuf {
            char d_space[sizezero];
        };

        *((zerobuf *) d_address.sin_zero) = zerobuf();
    }

    // ACCESSORS
    void fromSocketAddress(bteso_IPv4Address *addr) const
    {
        // No need to change network/host byte order.
        if (d_address.sin_addr.s_addr == INADDR_ANY) {
            addr->setIpAddress(bteso_IPv4Address::BTESO_ANY_ADDRESS);
        }
        else {
            addr->setIpAddress(d_address.sin_addr.s_addr);
        }

        // Must change to host byte order.
        addr->setPortNumber(ntohs(d_address.sin_port));
    }
};

                          // ==============================
                          // struct bteso_SocketImpUtil_Imp
                          // ==============================

template <class ADDRESS>
class bteso_SocketImpUtil_Imp {
    // This is an implementation class for the static templatized member
    // functions of 'bteso_SocketImpUtil'.  Templatized static member
    // functions are not well supported by all compilers.  Such functions are
    // forwarded onto specializations of this class.  This class is only
    // used by 'bteso_SocketImpUtil'.

    friend struct bteso_SocketImpUtil;

    static int accept(bteso_SocketHandle::Handle       *newSocket,
                      const bteso_SocketHandle::Handle& socket,
                      int                              *errorCode);

    static int accept(bteso_SocketHandle::Handle        *newSocket,
                      ADDRESS                           *peerAddress,
                      const bteso_SocketHandle::Handle&  socket,
                      int                               *errorCode);

    static int bind(const bteso_SocketHandle::Handle& socket,
                    const ADDRESS&                    address,
                    int                              *errorCode);

    static int connect(const bteso_SocketHandle::Handle& socket,
                       const ADDRESS&                    address,
                       int                              *errorCode);

    static int getLocalAddress(ADDRESS                          *localAddress,
                               const bteso_SocketHandle::Handle& socket,
                               int                              *errorCode);

    static int getPeerAddress(ADDRESS                           *peerAddress,
                              const bteso_SocketHandle::Handle&  socket,
                              int                               *errorCode);

    static int open(bteso_SocketHandle::Handle  *newSocket,
                    bteso_SocketImpUtil::Type    type,
                    int                         *errorCode);

    static int open(bteso_SocketHandle::Handle  *newSocket,
                    bteso_SocketImpUtil::Type    type,
                    int                          protocol,
                    int                         *errorCode);

    static int readFrom(ADDRESS                           *fromAddress,
                        void                              *buffer,
                        const bteso_SocketHandle::Handle&  socket,
                        int                                numBytes,
                        int                               *errorCode);

    static int socketPair(bteso_SocketHandle::Handle  *newSockets,
                          bteso_SocketImpUtil::Type    type,
                          int                         *errorCode);

    static int socketPair(bteso_SocketHandle::Handle  *newSockets,
                          bteso_SocketImpUtil::Type    type,
                          int                          protocol,
                          int                         *errorCode);

    static int writeTo(const bteso_SocketHandle::Handle&  socket,
                       const ADDRESS&                     toAddress,
                       const void                        *buffer,
                       int                                numBytes,
                       int                               *errorCode);

    static int writevTo(const bteso_SocketHandle::Handle&  socket,
                        const ADDRESS&                     toAddress,
                        const btes_Ovec                   *ovec,
                        int                                numBuffs,
                        int                               *errorCode);

};

template <>
int bteso_SocketImpUtil_Imp<bteso_IPv4Address>::socketPair(
        bteso_SocketHandle::Handle  *newSockets,
        bteso_SocketImpUtil::Type    type,
        int                          protocol,
        int                         *errorCode);

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

template <class ADDRESS>
inline int bteso_SocketImpUtil::accept(
                      bteso_SocketHandle::Handle        *newSocket,
                      ADDRESS                           *peerAddress,
                      const bteso_SocketHandle::Handle&  socket,
                      int                               *errorCode)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::accept(newSocket, peerAddress,
                                                    socket, errorCode);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::accept(
                      bteso_SocketHandle::Handle        *newSocket,
                      const bteso_SocketHandle::Handle&  socket,
                      int                               *errorCode)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::accept(newSocket,
                                                    socket, errorCode);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::accept(
                      bteso_SocketHandle::Handle        *newSocket,
                      ADDRESS                           *peerAddress,
                      const bteso_SocketHandle::Handle&  socket)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::accept(newSocket, peerAddress,
                                                    socket, 0);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::accept(
                      bteso_SocketHandle::Handle        *newSocket,
                      const bteso_SocketHandle::Handle&  socket)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::accept(newSocket, socket, 0);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::bind(
                                const bteso_SocketHandle::Handle& socket,
                                const ADDRESS&                    address,
                                int                              *errorCode)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::bind(socket, address, errorCode);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::bind(const bteso_SocketHandle::Handle& socket,
                                     const ADDRESS&                    address)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::bind(socket, address, 0);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::connect(
                       const bteso_SocketHandle::Handle& socket,
                       const ADDRESS&                    address,
                       int                              *errorCode)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::connect(socket, address,
                                                     errorCode);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::connect(
                       const bteso_SocketHandle::Handle& socket,
                       const ADDRESS&                    address)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::connect(socket, address, 0);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::getLocalAddress(
                        ADDRESS                          *localAddress,
                        const bteso_SocketHandle::Handle& socket,
                        int                              *errorCode)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::getLocalAddress(localAddress,
                                                             socket,
                                                             errorCode);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::getLocalAddress(
                        ADDRESS                          *localAddress,
                        const bteso_SocketHandle::Handle& socket)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::getLocalAddress(localAddress,
                                                             socket, 0);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::getPeerAddress(
                        ADDRESS                          *peerAddress,
                        const bteso_SocketHandle::Handle& socket,
                        int                              *errorCode)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::getPeerAddress(peerAddress,
                                                            socket, errorCode);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::getPeerAddress(
                        ADDRESS                          *peerAddress,
                        const bteso_SocketHandle::Handle& socket)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::getPeerAddress(peerAddress,
                                                            socket, 0);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::open(
            bteso_SocketHandle::Handle  *newSocket,
            bteso_SocketImpUtil::Type    type,
            int                          protocol,
            int                         *errorCode)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::open(newSocket, type,
                                                  protocol, errorCode);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::open(
            bteso_SocketHandle::Handle  *newSocket,
            bteso_SocketImpUtil::Type    type,
            int                          protocol)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::open(newSocket, type,
                                                  protocol, 0);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::open(
            bteso_SocketHandle::Handle  *newSocket,
            bteso_SocketImpUtil::Type    type,
            int                         *errorCode)
{
    // The protocol has not been specified so 0 is used.  This is appropriate
    // for most socket types and protocol families, where only one protocol
    // exists.
    return bteso_SocketImpUtil_Imp<ADDRESS>::open(newSocket, type,
                                                  0, errorCode);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::open(bteso_SocketHandle::Handle  *newSocket,
            bteso_SocketImpUtil::Type    type)
{
    // The protocol has not been specified so 0 is used.  This is appropriate
    // for most socket types and protocol families, where only one protocol
    // exists.
    return bteso_SocketImpUtil_Imp<ADDRESS>::open(newSocket, type, 0, 0);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::readFrom(
                           ADDRESS                           *fromAddress,
                           void                              *buffer,
                           const bteso_SocketHandle::Handle&  socket,
                           int                                numBytes,
                           int                               *errorCode)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::readFrom(
           fromAddress, buffer, socket, numBytes, errorCode);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::readFrom(
                           ADDRESS                           *fromAddress,
                           void                              *buffer,
                           const bteso_SocketHandle::Handle&  socket,
                           int                                numBytes)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::readFrom(
           fromAddress, buffer, socket, numBytes, 0);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::socketPair(
                          bteso_SocketHandle::Handle  *newSockets,
                          bteso_SocketImpUtil::Type    type,
                          int                         *errorCode)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::socketPair(newSockets, type,
                                                        0, errorCode);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::socketPair(
                          bteso_SocketHandle::Handle  *newSockets,
                          bteso_SocketImpUtil::Type    type)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::socketPair(newSockets, type,
                                                        0, 0);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::socketPair(
                          bteso_SocketHandle::Handle  *newSockets,
                          bteso_SocketImpUtil::Type    type,
                          int                          protocol,
                          int                         *errorCode)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::socketPair(newSockets, type,
                                                        protocol, errorCode);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::socketPair(
                          bteso_SocketHandle::Handle  *newSockets,
                          bteso_SocketImpUtil::Type    type,
                          int                          protocol)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::socketPair(newSockets,
                                                        type, protocol, 0);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::writeTo(
                      const bteso_SocketHandle::Handle&  socket,
                      const ADDRESS&                     toAddress,
                      const void                        *buffer,
                      int                                numBytes,
                      int                               *errorCode)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::writeTo(socket, toAddress,
                                                     buffer, numBytes,
                                                     errorCode);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::writeTo(
                      const bteso_SocketHandle::Handle&  socket,
                      const ADDRESS&                     toAddress,
                      const void                        *buffer,
                      int                                numBytes)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::writeTo(socket, toAddress,
                                                     buffer, numBytes, 0);
}

template <class ADDRESS>
inline int bteso_SocketImpUtil::writevTo(
                      const bteso_SocketHandle::Handle&  socket,
                      const ADDRESS&                     toAddress,
                      const btes_Ovec                   *ovec,
                      int                                numBuffs,
                      int                               *errorCode)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::writevTo(socket, toAddress,
                                                      ovec, numBuffs,
                                                      errorCode);
}

// ===========================================================================
//                 NON-INLINE TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================

template <class ADDRESS>
int bteso_SocketImpUtil_Imp<ADDRESS>::accept(
                       bteso_SocketHandle::Handle       *newSocket,
                       const bteso_SocketHandle::Handle& listeningSocket,
                       int                              *errorCode)
{
    bteso_SocketImpUtil_Address<ADDRESS> address;
    bteso_SocketImpUtil_Util::ADDRLEN_T siLen = sizeof address.d_address;

    *newSocket = ::accept(listeningSocket,
                          (sockaddr *)&address.d_address,
                          &siLen);

    int errorNumber = bteso_SocketImpUtil_Util::isValid(*newSocket) ? 0 :
           bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
           bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

template <class ADDRESS>
int bteso_SocketImpUtil_Imp<ADDRESS>::accept(
       bteso_SocketHandle::Handle       *newSocket,
       ADDRESS                          *peerAddress,
       const bteso_SocketHandle::Handle& listeningSocket,
       int                              *errorCode)
{
    bteso_SocketImpUtil_Address<ADDRESS> address;
    bteso_SocketImpUtil_Util::ADDRLEN_T siLen = sizeof address.d_address;

    *newSocket = ::accept(listeningSocket,
                          (sockaddr *) &address.d_address, &siLen);
    if (bteso_SocketImpUtil_Util::isValid(*newSocket)) {
        address.fromSocketAddress(peerAddress);
        return 0;
    }
    else {
        int errorNumber = bteso_SocketImpUtil_Util::getErrorCode();
        if (errorNumber && errorCode)
            *errorCode = errorNumber;
        return bteso_SocketImpUtil_Util::mapErrorCode(errorNumber);
    }
}

template <class ADDRESS>
int bteso_SocketImpUtil_Imp<ADDRESS>::bind(
                              const bteso_SocketHandle::Handle& theSocket,
                              const ADDRESS&                    address,
                              int                              *errorCode)
{
    int rc;
    bteso_SocketImpUtil_Address<ADDRESS> sockAddress(address);

    rc = ::bind(theSocket, (sockaddr *) &sockAddress.d_address,
                sizeof sockAddress.d_address);

    int errorNumber = rc >= 0 ? 0 : bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

template <class ADDRESS>
int bteso_SocketImpUtil_Imp<ADDRESS>::connect(
                                 const bteso_SocketHandle::Handle& theSocket,
                                 const ADDRESS&                    address,
                                 int                              *errorCode)
{
    int rc;
    bteso_SocketImpUtil_Address<ADDRESS> sockAddress(address);

    rc = ::connect(theSocket, (sockaddr *) &sockAddress.d_address,
                   sizeof sockAddress.d_address);
    int errorNumber = rc >= 0 ? 0 : bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

template <class ADDRESS>
int bteso_SocketImpUtil_Imp<ADDRESS>::getLocalAddress(
                         ADDRESS                          *localAddress,
                         const bteso_SocketHandle::Handle& theSocket,
                         int                              *errorCode)
{
    int rc;
    bteso_SocketImpUtil_Address<ADDRESS> address;
    bteso_SocketImpUtil_Util::ADDRLEN_T siLen = sizeof address.d_address;

    rc = getsockname(theSocket, (sockaddr *) &address.d_address, &siLen);

    if (rc >= 0) {
        address.fromSocketAddress(localAddress);
        return 0;
    }
    else {
        int errorNumber = bteso_SocketImpUtil_Util::getErrorCode();
        if (errorNumber && errorCode)
            *errorCode = errorNumber;
        return bteso_SocketImpUtil_Util::mapErrorCode(errorNumber);
    }
}

template <class ADDRESS>
int bteso_SocketImpUtil_Imp<ADDRESS>::getPeerAddress(
                             ADDRESS                          *address,
                             const bteso_SocketHandle::Handle& theSocket,
                             int                              *errorCode)
{
    int rc;
    bteso_SocketImpUtil_Address<ADDRESS> sockAddress;
    bteso_SocketImpUtil_Util::ADDRLEN_T siLen = sizeof sockAddress.d_address;

    rc = getpeername(theSocket, (sockaddr *) &sockAddress.d_address, &siLen);

    if (rc >= 0) {
        sockAddress.fromSocketAddress(address);
        return 0;
    }
    else {
        int errorNumber = bteso_SocketImpUtil_Util::getErrorCode();
        if (errorNumber && errorCode)
            *errorCode = errorNumber;
        return bteso_SocketImpUtil_Util::mapErrorCode(errorNumber);
    }
}

template <class ADDRESS>
inline int bteso_SocketImpUtil_Imp<ADDRESS>::open(
                                       bteso_SocketHandle::Handle *newSocket,
                                       bteso_SocketImpUtil::Type   type,
                                       int                        *errorCode)
{
    return bteso_SocketImpUtil_Imp<ADDRESS>::open(newSocket, type,
                                                 0, errorCode);
}

template <class ADDRESS>
int bteso_SocketImpUtil_Imp<ADDRESS>::open(
                                       bteso_SocketHandle::Handle *newSocket,
                                       bteso_SocketImpUtil::Type   type,
                                       int                         protocol,
                                       int                        *errorCode)
{
    *newSocket = ::socket(bteso_SocketImpUtil_Address<ADDRESS>::SocketDomain,
                          (int) type, protocol);

    int errorNumber = bteso_SocketImpUtil_Util::isValid(*newSocket) ? 0 :
          bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : 0;
}

template <class ADDRESS>
int bteso_SocketImpUtil_Imp<ADDRESS>::readFrom(
                                 ADDRESS                          *fromAddress,
                                 void                             *buffer,
                                 const bteso_SocketHandle::Handle& socket,
                                 int                               numBytes,
                                 int                              *errorCode)
{
    int rc;
    bteso_SocketImpUtil_Address<ADDRESS> address;
    bteso_SocketImpUtil_Util::ADDRLEN_T siLen = sizeof address.d_address;

    rc = ::recvfrom(socket, (char *) buffer, numBytes,
                  0, (sockaddr *) &address.d_address, &siLen);
    if (rc >= 0) {
        address.fromSocketAddress(fromAddress);
        return rc;
    }
    else {
        int errorNumber = bteso_SocketImpUtil_Util::getErrorCode();
        if (errorNumber && errorCode)
            *errorCode = errorNumber;
        return bteso_SocketImpUtil_Util::mapErrorCode(errorNumber);
    }
}

template <class ADDRESS>
int bteso_SocketImpUtil_Imp<ADDRESS>::writeTo(
                                const bteso_SocketHandle::Handle& socket,
                                const ADDRESS&                    toAddress,
                                const void                       *buffer,
                                int                               numBytes,
                                int                              *errorCode)
{
    int rc;
    bteso_SocketImpUtil_Address<ADDRESS> sockAddress(toAddress);

    if (!numBytes) return 0;
    rc = ::sendto(socket, (char *) buffer, numBytes,
                  0, (sockaddr *) &sockAddress.d_address,
                  sizeof sockAddress.d_address);
    int errorNumber = rc >= 0 ? 0 : bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : rc;
}

template <class ADDRESS>
int bteso_SocketImpUtil_Imp<ADDRESS>::writevTo(
                                const bteso_SocketHandle::Handle& socket,
                                const ADDRESS&                    toAddress,
                                const btes_Ovec                   *ovec,
                                int                                numBuffs,
                                int                               *errorCode)
{
#if defined(BTESO_PLATFORM__BSD_SOCKETS)
    int rc;

    bteso_SocketImpUtil_Address<ADDRESS> sockAddress(toAddress);

    struct msghdr msg;
    // set unused fields to 0 portably
    memset(&msg, 0, sizeof msg);

#if defined(BSLS_PLATFORM__OS_HPUX)
    msg.msg_name = (caddr_t) &sockAddress.d_address;
#else
    msg.msg_name = (sockaddr *) &sockAddress.d_address;
#endif
    msg.msg_namelen = sizeof(sockAddress.d_address);
    msg.msg_iov = (::iovec*)ovec;
    msg.msg_iovlen = numBuffs;

    rc = ::sendmsg(socket, &msg, 0);

    int errorNumber = rc >= 0 ? 0 : bteso_SocketImpUtil_Util::getErrorCode();
    if (errorNumber && errorCode)
        *errorCode = errorNumber;
    return errorNumber ?
          bteso_SocketImpUtil_Util::mapErrorCode(errorNumber) : rc;
#endif

#if defined(BTESO_PLATFORM__WIN_SOCKETS)
    // On platforms which don't have sendmsg we use the writeTo method
    // and copy data if necessary.
    const int maxPacket = 1500; // Assume MTU size of 1500
    char mergedData[maxPacket];

    if (numBuffs==1)
    {
        // Simple case
        return writeTo(socket, toAddress, ovec[0].buffer(), ovec[0].length(),
                       errorCode);
    }
    else
    {
        // Copy the data into our buffer
        char* to = &(mergedData[0]);
        int numBytes = 0;
        for (int i=0; i<numBuffs; ++i)
        {
            if (ovec[i].length() > (maxPacket-numBytes))
            {
                // Behavior in this case is undefined but avoid
                // trashing the stack.
                return -1;
            }
            bsl::copy_n(static_cast<char const*>(ovec[i].buffer()),
                        ovec[i].length(),
                        to);
            numBytes += ovec[i].length();
            to += ovec[i].length();
        }
        return writeTo(socket, toAddress, &(mergedData[0]), numBytes,
                       errorCode);
    }
#endif
}

template <class ADDRESS>
int bteso_SocketImpUtil_Imp<ADDRESS>::socketPair(
                          bteso_SocketHandle::Handle  *newSockets,
                          bteso_SocketImpUtil::Type    type,
                          int                         *errorCode)
{
    return socketPair(newSockets, type, 0, errorCode);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
