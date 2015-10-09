// btlso_inetstreamsocket.h                                           -*-C++-*-
#ifndef INCLUDED_BTLSO_INETSTREAMSOCKET
#define INCLUDED_BTLSO_INETSTREAMSOCKET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Implementation for TCP based stream sockets.
//
//@CLASSES:
//  btlso::InetStreamSocket: implementation of TCP-based stream-sockets
//  btlso::InetStreamSocketCloseGuard: proctor for managing stream-sockets
//
//@SEE_ALSO: btlso_streamsocket, btlso_ipv4address, btlsos_tcpconnector
//
//@DESCRIPTION: This component implements TCP-based stream sockets of type
// 'btlso::InetStreamSocket<ADDRESS>' conforming to the
// 'btlso::StreamSocket<ADDRESS>' protocol.  The classes are templatized to
// provide type-safe address class specialization.  The only address type
// currently supported is IPv4 (as provided by 'btlso_ipv4address' component).
// Therefore, the template parameter will always be 'btlso::IPv4Address'.
//
///Thread Safety
///-------------
// The classes provided by this component depend on a 'bslma::Allocator'
// instance to supply memory.  If the allocator is not thread enabled then the
// instances of this component that use the same allocator instance will
// consequently not be thread safe.  Otherwise, this component provides the
// following guarantees:
//
// The 'btlso::InetStreamSocket' is not *thread-enabled* (it is not safe to
// invoke methods from different threads on the same instance).
// 'btlso::InetStreamSocket' is *thread-safe*, meaning that distinct threads
// can access distinct instances simultaneously without any side-effects (which
// usually means that there is no 'static' data).  The classes provided by this
// component are not *async-safe*, meaning that one or more functions cannot be
// invoked safely from a signal handler.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Exchanging Data Using a Stream Socket
/// - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we will show how to create two stream sockets, establish a
// connection, and then exchange data between them.
//
// First, we will open a pair of socket handles.
//..
//  btlso::SocketHandle::Handle clientHandle, serverHandle;
//
//  int rc = btlso::SocketImpUtil::open<btlso::IPv4Address>(
//                                      &clientHandle,
//                                      btlso::SocketImpUtil::k_SOCKET_STREAM);
//  assert(0 == rc);
//
//  rc = btlso::SocketImpUtil::open<btlso::IPv4Address>(
//                                      &serverHandle,
//                                      btlso::SocketImpUtil::k_SOCKET_STREAM);
//  assert(0 == rc);
//..
// Then, we will construct the server and client stream socket objects,
// 'serverSocket' and 'clientSocket' respectively.
//..
//  btlso::InetStreamSocket<btlso::IPv4Address> serverSocket(serverHandle);
//  btlso::InetStreamSocket<btlso::IPv4Address> clientSocket(clientHandle);
//..
// Next, we will bind 'serverSocket' to a local address and start listening on
// it.
//..
//  btlso::IPv4Address localAddress;
//  localAddress.setIpAddress("127.0.0.1");
//  localAddress.setPortNumber(0);
//
//  rc = serverSocket.bind(localAddress);
//  assert(0 == rc);
//
//  btlso::IPv4Address serverAddress;
//  rc = serverSocket.localAddress(&serverAddress);
//  assert(0 == rc);
//
//  rc = serverSocket.listen(10);
//  assert(0 == rc);
//..
// Then, we will connect 'clientSocket' to 'localAddress'.
//..
//  rc = clientSocket.connect(serverAddress);
//  assert(0 == rc);
//..
// Next, we will create a new stream socket, 'acceptSocket', after accepting
// the new connection.
//..
//  btlso::StreamSocket<btlso::IPv4Address> *acceptSocket;
//  btlso::IPv4Address clientAddress;
//
//  rc = serverSocket.accept(&acceptSocket);
//  assert(0 == rc);
//..
// Then, we will confirm that both sockets are connected.
//..
//  rc = clientSocket.connectionStatus();
//  assert(0 == rc);
//
//  rc = acceptSocket->connectionStatus();
//  assert(0 == rc);
//..
// Next, we will write a message through 'clientSocket'.
//..
//  char      *writeBuffer    = "Hello World!";
//  const int  writeBufferLen = bsl::strlen(writeBuffer);
//
//  rc = clientSocket.write(writeBuffer, writeBufferLen);
//  assert(rc == writeBufferLen);
//..
// Now, we will read the written message through 'acceptSocket'.
//..
//  const int  BUFSIZE = 64;
//  char       readBuffer[BUFSIZE];
//
//  rc = acceptSocket->read(readBuffer, BUFSIZE);
//  assert(rc == writeBufferLen);
//..
// Finally, we will close all the sockets.
//..
//  btlso::SocketImpUtil::close(clientHandle);
//  btlso::SocketImpUtil::close(acceptSocket->handle());
//  btlso::SocketImpUtil::close(serverHandle);
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_STREAMSOCKET
#include <btlso_streamsocket.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETIMPUTIL
#include <btlso_socketimputil.h>
#endif

#ifndef INCLUDED_BTLSO_IOUTIL
#include <btlso_ioutil.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

#ifndef INCLUDED_BTLS_IOVEC
#include <btls_iovec.h>
#endif

#ifndef INCLUDED_BDLT_CURRENTTIME
#include <bdlt_currenttime.h>
#endif

#ifndef INCLUDED_BDLT_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLS_PLATFORM_OS_UNIX

    #ifndef INCLUDED_BSL_C_SYS_TIME
    #include <bsl_c_sys_time.h>
    #endif

    #ifndef INCLUDED_SYS_POLL
    #include <sys/poll.h>
    #define INCLUDED_SYS_POLL
    #endif

    #ifndef INCLUDED_SYS_TYPES
    #include <sys/types.h>
    #define INCLUDED_SYS_TYPES
    #endif

#endif

namespace BloombergLP {

namespace btlso {

                        // ======================
                        // class InetStreamSocket
                        // ======================

template <class ADDRESS>
class InetStreamSocket : public StreamSocket<ADDRESS> {
    // This class implements the 'StreamSocket<ADDRESS>' protocol to provide
    // stream-based socket communications.  The class is templatized to provide
    // a family of type-safe address specializations (e.g., "IPv4", "IPv6").
    // Various socket-related operations, including accepting and initiating
    // connections and blocking/non-blocking I/O operations, are provided.
    // Vector I/O operations are also supported.

    // DATA
    SocketHandle::Handle  d_handle;

    bslma::Allocator     *d_allocator_p; // held, not owned

  private:
    // NOT IMPLEMENTED
    InetStreamSocket(const InetStreamSocket<ADDRESS>&);
    InetStreamSocket<ADDRESS>& operator=(const InetStreamSocket<ADDRESS>&);

  public:
    // CREATORS
    InetStreamSocket(SocketHandle::Handle  handle,
                     bslma::Allocator     *basicAllocator = 0);
        // Create a stream socket attached to the specified socket 'handle'
        // that uses the specified 'allocator' to supply memory.  If
        // 'allocator' is 0, the currently installed default allocator is used.
        // The behavior is undefined unless socket 'handle' refers to a valid
        // system socket.

    virtual ~InetStreamSocket<ADDRESS>();
        // Destroy this 'StreamSocket' object.

    // MANIPULATORS
    virtual int accept(StreamSocket<ADDRESS> **socket);
        // Accept an incoming connection request and load the address of a new
        // stream socket of type 'InetStreamSocket<ADDRESS>' into the specified
        // 'socket'.  Return 0 on success, and a non-zero value otherwise.  If
        // this socket is in blocking mode, this function waits until a
        // connection request is received or an error occurs.  If this socket
        // is in non-blocking mode and there is no pending connection request,
        // this call returns immediately with an error status of
        // 'SocketHandle::e_ERROR_WOULDBLOCK'.  Note that this socket must be
        // listening for connections ('listen').

    virtual int accept(StreamSocket<ADDRESS> **socket, ADDRESS *peerAddress);
        // Accept an incoming connection request, load the address of a new
        // stream socket of type 'InetStreamSocket<ADDRESS>' into the specified
        // 'socket', and load the address of the peer into the specified
        // 'peerAddress'.  Return 0 on success, and a non-zero value otherwise.
        // If this socket is in blocking mode, this function waits until a
        // connection request is received or an error occurs.  If this socket
        // is in non-blocking mode and there is no pending connection request,
        // this call returns immediately with an error status of
        // 'SocketHandle::e_ERROR_WOULDBLOCK'.  Note that this socket must be
        // listening for connections ('listen').

    virtual int bind(const ADDRESS& address);
        // Associate the specified 'address' with this socket.  Return 0 on
        // success, and a non-zero value otherwise.  Note that in order to
        // receive connections on a socket, it must have an address associated
        // with it.

    virtual int connect(const ADDRESS& address);
        // Initiate a connection to a peer process at the specified 'address'.
        // Return 0 on success, and a non-zero value otherwise.  If this socket
        // is in blocking mode, the call waits until a connection is
        // established or an error occurs.  If this socket is in non-blocking
        // mode and the connection cannot be established immediately
        // 'SocketHandle::e_ERROR_WOULDBLOCK' is returned.  The
        // 'waitForConnect' method may then be used to determine when the
        // connection request has completed.

    virtual int listen(int backlog);
        // Register this socket for accepting up to the specified 'backlog'
        // simultaneous connection requests.  Return 0 on success, and a
        // non-zero value otherwise.  The behavior is undefined unless
        // '0 < backlog'.

    virtual int read(char *buffer, int length);
        // Read up to the specified 'length' bytes from this socket into the
        // specified 'buffer'; return the non-negative number of bytes read, or
        // a negative value on error.  If this socket is in blocking mode, if
        // there are less than 'length' bytes of data available, the call waits
        // for data to arrive.  If this socket is in non-blocking mode, the
        // function reads as many bytes as possible without blocking, and
        // returns the number of bytes read, or
        // 'SocketHandle::e_ERROR_WOULDBLOCK' if zero bytes were immediately
        // available.  If the connection has been closed and there is no data
        // available, 'SocketHandle::e_ERROR_EOF' is returned.  If the call is
        // interrupted before data is available,
        // 'SocketHandle::e_ERROR_INTERRUPTED' is returned.  The behavior is
        // undefined unless '0 < length' and 'buffer' refers to at least
        // 'length' writable bytes.

    virtual int readv(const btls::Iovec *buffers, int numBuffers);
        // Read from this socket into the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers' the respective numbers of
        // bytes as defined by the 'length' methods of each 'btls::Iovec'
        // structure.  Return the non-negative total number of bytes read, or a
        // negative value on error.  If this socket is in blocking mode, if
        // there are less than 'length' bytes of data available, the call waits
        // for data to arrive.  If this socket is in non-blocking mode, the
        // function reads as many bytes as possible without blocking, and
        // returns the number of bytes read or
        // 'SocketHandle::e_ERROR_WOULDBLOCK' if zero bytes were immediately
        // available.  If the connection has been closed, and there is no data
        // available, 'SocketHandle::e_ERROR_EOF' is returned.  If the call is
        // interrupted before data is available,
        // 'SocketHandle::e_ERROR_INTERRUPTED' is returned.  The behavior is
        // undefined unless '0 < numBuffers'.

    virtual int write(const char *buffer, int length);
        // Write up to the specified 'length' bytes to this socket from the
        // specified 'buffer'; return the non-negative number of bytes written,
        // or a negative value on error.  If this socket is in blocking mode
        // the call blocks until the data is fully written.  If this socket is
        // in non-blocking mode, the function writes as many bytes as possible
        // without blocking, and returns the number of bytes written, or
        // 'SocketHandle::e_ERROR_WOULDBLOCK' if no bytes were written.  If the
        // connection has been closed, 'SocketHandle::e_ERROR_CONNDEAD' is
        // returned.  If the call is interrupted before any data is written,
        // 'SocketHandle::e_ERROR_INTERRUPTED' is returned.  The behavior is
        // undefined unless '0 < length'.  Note that a successful call to this
        // function does not guarantee that the data has been transmitted
        // successfully, but simply that the data was written successfully to
        // the underlying socket's transmit buffers.

    virtual int writev(const btls::Iovec *buffers, int numBuffers);
    virtual int writev(const btls::Ovec  *buffers, int numBuffers);
        // Write to this socket from the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers' the respective numbers of
        // bytes as reported by the 'length' methods of each 'btls::Ovec'
        // structure; return the non-negative total number of bytes written, or
        // a negative value on error.  If this socket is in blocking mode the
        // call blocks until the data is fully written.  If this socket is in
        // non-blocking mode, the function writes as many bytes as possible
        // without blocking, and returns the number of bytes written, or
        // 'SocketHandle::e_ERROR_WOULDBLOCK', if no bytes were written.  If
        // the connection has been closed, 'SocketHandle::e_ERROR_CONNDEAD' is
        // returned.  If the call is interrupted before any data is written,
        // 'SocketHandle::e_ERROR_INTERRUPTED' is returned.  The behavior is
        // undefined unless '0 < numBuffers'.  Note that a successful call to
        // this function does not guarantee that the data has been transmitted
        // successfully, but simply that the data was successfully written to
        // the underlying socket's transmit buffers.

    virtual int setBlockingMode(btlso::Flag::BlockingMode mode);
        // Set the current blocking mode of this socket to the specified
        // 'mode'.  Return 0 on success, an a non-zero value otherwise.

    virtual int shutdown(btlso::Flag::ShutdownType streamOption);
        // Shut down the input and/or output stream(s) indicated by the
        // specified 'streamOption' of the full-duplexed connection associated
        // with this socket.  Return 0 on success, and a non-zero value
        // otherwise.  Specifying 'SHUTDOWN_RECEIVE' or 'SHUTDOWN_SEND' closes
        // the input or output streams, respectively, leaving the other stream
        // unaffected.  Specifying 'SHUTDOWN_BOTH' closes both streams.  Once
        // the input(output) stream has been closed, any attempted read(write)
        // operations will fail.  Any data received after successfully shutting
        // down the input stream will be acknowledged but silently discarded.
        // Note that this function is typically used to transmit an EOF
        // indication to the peer.

    virtual int waitForConnect(const bsls::TimeInterval& timeout);
        // Wait until a pending outgoing connection request completes or until
        // the specified absolute 'timeout' is reached, whichever occurs first.
        // Return 0 if the connection request has completed, and a non-zero
        // value otherwise.  Note that the completion of a connection request
        // does not imply that a connection has been successfully established;
        // a call to 'connectionStatus' should be made to determine if the
        // connection request completed successfully.

    virtual int waitForAccept(const bsls::TimeInterval& timeout);
        // Wait for an incoming connection request on this socket or until the
        // specified absolute 'timeout' is reached, whichever occurs first.
        // Return 0 if a connection request has been received, and a non-zero
        // value otherwise.  Note that once a connection request has been
        // received, a call to 'accept' can be made to establish the
        // connection.

    virtual int waitForIO(btlso::Flag::IOWaitType   type,
                          const bsls::TimeInterval& timeout);
        // Wait for an I/O of the specified 'type' to occur or until the
        // specified absolute 'timeout' is reached, whichever occurs first.
        // Return IO_READ, IO_WRITE or IO_RW if the corresponding event
        // occurred, and a non-zero value otherwise.  If a timeout occurred, a
        // value of 'SocketHandle::e_ERROR_TIMEDOUT' is returned.  If this call
        // is interrupted, 'SocketHandle::e_ERROR_INTERRUPTED' is returned.

    virtual int waitForIO(btlso::Flag::IOWaitType type);
        // Wait for an I/O of the specified 'type' to occur.  Return 'type' if
        // the corresponding event occurred, and a negative value otherwise.
        // If this call is interrupted, 'SocketHandle::e_ERROR_INTERRUPTED' is
        // returned.

    virtual int setLingerOption(const SocketOptUtil::LingerData& options);
        // Set the current linger options of this socket to the specified
        // 'options'.  Return 0 on success, and a non-zero value otherwise.

    virtual int setOption(int level, int option, int value);
        // Set the specified socket 'option' (of the specified 'level') on this
        // socket to the specified 'value'.  Return 0 on success and a non-zero
        // value otherwise.  See 'btlso_socketoptutil' for the list of commonly
        // supported options.

    // ACCESSORS
    virtual int blockingMode(btlso::Flag::BlockingMode *result) const;
        // Load into the specified 'result' the current blocking mode of this
        // socket.  Return 0 on success, and a non-zero value without affecting
        // 'result' otherwise.

    virtual int localAddress(ADDRESS *result) const;
        // Load into the specified 'result' the local address of this socket.
        // Return 0 on success, and a non-zero value without affecting 'result'
        // otherwise.

    virtual int peerAddress(ADDRESS *result) const;
        // Load into the specified 'result' the address of the peer connected
        // to this socket.  Return 0 on success, and a non-zero value without
        // affecting 'result' otherwise.

    virtual SocketHandle::Handle handle() const;
        // Return the OS handle associated with this socket.  Note that direct
        // manipulation of the underlying stream may result in undefined
        // behavior.

    virtual int connectionStatus() const;
        // Test the connection status of this socket.  Return 0 if this socket
        // has an established connection, and a non-zero value otherwise.  If
        // there is no established or pending connection, a value of
        // 'SocketHandle::e_ERROR_CONNDEAD' is returned.  A value of
        // 'SocketHandle::e_ERROR_CONNDEAD' is also returned if a non-blocking
        // connection request fails.  Note that this method is typically used
        // to determine the result of a non-blocking connection request.

    virtual int lingerOption(SocketOptUtil::LingerData *result) const;
        // Load into the specified 'result' the value of this socket's current
        // linger options.  The behavior is undefined if 'result' is 0.

    virtual int socketOption(int *result, int level, int option) const;
        // Load int the specified 'result' the value of the specified socket
        // 'option' of the specified 'level' socket option on this socket.
        // Return 0 on success and a non-zero value otherwise.
};

                // ================================
                // class InetStreamSocketCloseGuard
                // ================================

class InetStreamSocketCloseGuard {
    // This class implements a proctor that automatically closes the managed
    // socket at destruction unless its 'release' method is invoked.

    // DATA
    SocketHandle::Handle d_socketHandle;   // managed socket handle
    int                  d_valid;          // true until 'release' called

  private:
    // NOT IMPLEMENTED
    InetStreamSocketCloseGuard(const InetStreamSocketCloseGuard&);
    InetStreamSocketCloseGuard& operator=(const InetStreamSocketCloseGuard&);
  public:
    // CREATORS
    explicit InetStreamSocketCloseGuard(SocketHandle::Handle socketHandle);
        // Create a proctor object to manage socket having the specified
        // 'socketHandle'.

    ~InetStreamSocketCloseGuard();
        // Destroy this proctor object and, unless the 'release' method has
        // been previously called, close the managed socket.

    // MANIPULATORS
    void release();
        // Release from management the socket currently managed by this
        // proctor.  If no socket is currently being managed, this operation
        // has no effect.
};

// ============================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                     // ----------------------
                     // class InetStreamSocket
                     // ----------------------

// CREATORS
template <class ADDRESS>
InetStreamSocket<ADDRESS>::InetStreamSocket(
                                          SocketHandle::Handle  handle,
                                          bslma::Allocator     *basicAllocator)
: d_handle(handle)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class ADDRESS>
InetStreamSocket<ADDRESS>::~InetStreamSocket()
{
}

// MANIPULATORS
template <class ADDRESS>
int InetStreamSocket<ADDRESS>::accept(StreamSocket<ADDRESS> **socket)
{
    SocketHandle::Handle newHandle;

    int ret = SocketImpUtil::accept<ADDRESS>(&newHandle, d_handle);
    if (ret != 0) {
        return ret;                                                   // RETURN
    }

    InetStreamSocketCloseGuard autoDeallocate(newHandle);

    *socket = new (*d_allocator_p) InetStreamSocket<ADDRESS>(newHandle,
                                                             d_allocator_p);

    autoDeallocate.release();

    return 0;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::accept(StreamSocket<ADDRESS>  **socket,
                                      ADDRESS                 *peerAddress)
{
    SocketHandle::Handle newHandle;

    int ret = SocketImpUtil::accept<ADDRESS>(&newHandle,
                                             peerAddress,
                                             d_handle);

    if (ret != 0) {
        return ret;                                                   // RETURN
    }

    *socket = new (*d_allocator_p) InetStreamSocket<ADDRESS>(newHandle,
                                                             d_allocator_p);
    return 0;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::bind(const ADDRESS& address)
{
    int ret = SocketImpUtil::bind<ADDRESS>(d_handle, address);

    return ret;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::connect(const ADDRESS& address)
{
    int ret = SocketImpUtil::connect<ADDRESS>(d_handle, address);

    return ret;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::listen(int backlog)
{
    int ret = SocketImpUtil::listen(d_handle, backlog);

    return ret;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::read(char *buffer, int length)
{
    int ret = SocketImpUtil::read(buffer, d_handle, length);

    if (length > 0 && ret == 0) {
        ret = SocketHandle::e_ERROR_EOF;
    }

    return ret;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::readv(const btls::Iovec *buffers,
                                     int                numBuffers)
{
    int ret = SocketImpUtil::readv(buffers, d_handle, numBuffers);

    if (ret == 0) {
        // readv returns 0 if either the number of bytes to read was zero or if
        // an EOF occurred.

        int i;
        for (i = 0; i < numBuffers; ++i) {
            if (buffers[i].length()) {
                return SocketHandle::e_ERROR_EOF;                     // RETURN
            }
        }
    }

    return ret;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::write(const char *buffer, int length)
{
    int ret = SocketImpUtil::write(d_handle, buffer, length);

    return ret;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::writev(const btls::Iovec *buffers,
                                      int                numBuffers)
{
    int ret = SocketImpUtil::writev(
                                 d_handle,
                                 reinterpret_cast<const btls::Ovec *>(buffers),
                                 numBuffers);

    return ret;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::writev(const btls::Ovec *buffers,
                                      int               numBuffers)
{
    return SocketImpUtil::writev(d_handle, buffers, numBuffers);
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::setBlockingMode(btlso::Flag::BlockingMode mode)
{
    return IoUtil::setBlockingMode(d_handle,
                                   btlso::Flag::e_BLOCKING_MODE == mode
                                   ? IoUtil::e_BLOCKING
                                   : IoUtil::e_NONBLOCKING);
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::setLingerOption(
                                      const SocketOptUtil::LingerData& options)
{
    const int rc = SocketOptUtil::setOption(handle(),
                                            SocketOptUtil::k_SOCKETLEVEL,
                                            SocketOptUtil::k_LINGER,
                                            options);

    return rc ? SocketHandle::e_ERROR_UNCLASSIFIED : 0;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::setOption(int level, int option, int value)
{
    const int rc = SocketOptUtil::setOption(handle(),
                                            level,
                                            option,
                                            value);

    return rc ? SocketHandle::e_ERROR_UNCLASSIFIED : 0;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::shutdown(btlso::Flag::ShutdownType value)
{
    static enum SocketImpUtil::ShutDownType shutDownTypeMapping[] = {
        SocketImpUtil::e_SHUTDOWN_RECEIVE,
        SocketImpUtil::e_SHUTDOWN_SEND,
        SocketImpUtil::e_SHUTDOWN_BOTH
    };

    BSLMF_ASSERT(0 == btlso::Flag::e_SHUTDOWN_RECEIVE
              && 1 == btlso::Flag::e_SHUTDOWN_SEND
              && 2 == btlso::Flag::e_SHUTDOWN_BOTH);

    int ret = SocketImpUtil::shutDown(d_handle, shutDownTypeMapping[value]);

    return ret;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::waitForConnect(
                                             const bsls::TimeInterval& timeout)
{
    bsls::TimeInterval interval     = timeout - bdlt::CurrentTime::now();
    int                seconds      = static_cast<int>(interval.seconds());
    int                microseconds = (interval.nanoseconds() + 999) / 1000;

    if (seconds < 0 || microseconds < 0) {
        seconds = 0;
        microseconds = 0;
    }

    int rc = SocketHandle::e_ERROR_UNCLASSIFIED;

#ifdef BSLS_PLATFORM_OS_UNIX
    struct pollfd fds;
    fds.fd = d_handle;
    fds.events = POLLOUT;
    fds.revents = 0;

    int milliseconds = seconds * 1000 + (microseconds + 999) / 1000;

    int ret = ::poll(&fds, 1, milliseconds);

    if (ret > 0) {
        rc = 0;
    }
    else if (0 == ret) {
        rc = SocketHandle::e_ERROR_TIMEDOUT;
    }
    else if (EINTR == errno) {
        rc = SocketHandle::e_ERROR_INTERRUPTED;
    }
    else {
        rc = SocketHandle::e_ERROR_UNCLASSIFIED;
    }
#else
    fd_set writeset;
    fd_set excset;
    FD_ZERO(&writeset);
    FD_ZERO(&excset);
    FD_SET(d_handle, &writeset);
    FD_SET(d_handle, &excset);

    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = microseconds;

    int ret = ::select(d_handle + 1, 0, &writeset, &excset, &tv);

    if (ret > 0) {
        rc = 0;
    }
    else if (0 == ret) {
        rc = SocketHandle::e_ERROR_TIMEDOUT;
    }
    else if (WSAEINTR == WSAGetLastError()) {
        rc = SocketHandle::e_ERROR_INTERRUPTED;
    }
    else {
        rc = SocketHandle::e_ERROR_UNCLASSIFIED;
    }
#endif

    return rc;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::waitForAccept(const bsls::TimeInterval& timeout)
{
    return InetStreamSocket<ADDRESS>::waitForIO(Flag::e_IO_READ, timeout);
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::waitForIO(btlso::Flag::IOWaitType   type,
                                         const bsls::TimeInterval& timeout)
{
    bsls::TimeInterval interval     = timeout - bdlt::CurrentTime::now();
    int                seconds      = static_cast<int>(interval.seconds());
    int                microseconds = (interval.nanoseconds() + 999) / 1000;

    if (seconds < 0 || microseconds < 0) {
        seconds = 0;
        microseconds = 0;
    }

    int rc = SocketHandle::e_ERROR_UNCLASSIFIED;

    switch (type) {
      case btlso::Flag::e_IO_READ: {
#ifdef BSLS_PLATFORM_OS_UNIX
        struct pollfd fds;
        fds.fd = d_handle;
        fds.events = POLLIN;
        fds.revents = 0;

        int milliseconds = seconds * 1000 + (microseconds + 999) / 1000;

        int ret = ::poll(&fds, 1, milliseconds);

        if (ret > 0) {
            rc = btlso::Flag::e_IO_READ;
        }
        else if (0 == ret) {
            rc = SocketHandle::e_ERROR_TIMEDOUT;
        }
        else if (EINTR == errno) {
            rc = SocketHandle::e_ERROR_INTERRUPTED;
        } else {
            rc = SocketHandle::e_ERROR_UNCLASSIFIED;
        }
#else
        fd_set readset;
        FD_ZERO(&readset);
        FD_SET(d_handle, &readset);

        struct timeval tv;
        tv.tv_sec = seconds;
        tv.tv_usec = microseconds;

        int ret = ::select(d_handle + 1, &readset, 0, 0, &tv);

        if (ret > 0) {
            rc = btlso::Flag::e_IO_READ;
        }
        else if (0 == ret) {
            rc = SocketHandle::e_ERROR_TIMEDOUT;
        }
        else if (WSAEINTR == WSAGetLastError()) {
            rc = SocketHandle::e_ERROR_INTERRUPTED;
        } else {
            rc = SocketHandle::e_ERROR_UNCLASSIFIED;
        }
#endif
      } break;
      case btlso::Flag::e_IO_WRITE: {
#ifdef BSLS_PLATFORM_OS_UNIX
        struct pollfd fds;
        fds.fd = d_handle;
        fds.events = POLLOUT;
        fds.revents = 0;

        int milliseconds = seconds * 1000 + (microseconds + 999) / 1000;

        int ret = ::poll(&fds, 1, milliseconds);

        if (ret > 0) {
            rc = btlso::Flag::e_IO_WRITE;
        }
        else if (0 == ret) {
            rc = SocketHandle::e_ERROR_TIMEDOUT;
        }
        else if (EINTR == errno) {
            rc = SocketHandle::e_ERROR_INTERRUPTED;
        } else {
            rc = SocketHandle::e_ERROR_UNCLASSIFIED;
        }
#else
        fd_set writeset;
        FD_ZERO(&writeset);
        FD_SET(d_handle, &writeset);

        struct timeval tv;
        tv.tv_sec = seconds;
        tv.tv_usec = microseconds;

        int ret = ::select(d_handle + 1, 0, &writeset, 0, &tv);

        if (ret > 0) {
            rc = btlso::Flag::e_IO_WRITE;
        }
        else if (0 == ret) {
            rc = SocketHandle::e_ERROR_TIMEDOUT;
        }
        else if (WSAEINTR == WSAGetLastError()) {
            rc = SocketHandle::e_ERROR_INTERRUPTED;
        } else {
            rc = SocketHandle::e_ERROR_UNCLASSIFIED;
        }
#endif
      } break;
      case btlso::Flag::e_IO_RW: {
#ifdef BSLS_PLATFORM_OS_UNIX
        struct pollfd fds;
        fds.fd = d_handle;
        fds.events = POLLIN | POLLOUT;
        fds.revents = 0;

        int milliseconds = seconds * 1000 + (microseconds + 999) / 1000;

        int ret = ::poll(&fds, 1, milliseconds);

        if (ret > 0) {
            if (fds.revents & POLLIN) {
                if (fds.revents & POLLOUT) {
                    rc = btlso::Flag::e_IO_RW;
                } else {
                    rc = btlso::Flag::e_IO_READ;
                    }
            } else {
                rc = btlso::Flag::e_IO_WRITE;
            }
        }
        else if (0 == ret) {
            rc = SocketHandle::e_ERROR_TIMEDOUT;
        }
        else if (EINTR == errno) {
            rc = SocketHandle::e_ERROR_INTERRUPTED;
        } else {
            rc = SocketHandle::e_ERROR_UNCLASSIFIED;
        }
#else
        fd_set readset;
        fd_set writeset;
        FD_ZERO(&readset);
        FD_ZERO(&writeset);
        FD_SET(d_handle, &readset);
        FD_SET(d_handle, &writeset);

        struct timeval tv;
        tv.tv_sec = seconds;
        tv.tv_usec = microseconds;

        int ret = ::select(d_handle + 1, &readset, &writeset, 0, &tv);

        if (ret > 0) {
            if (FD_ISSET(d_handle, &readset)) {
                if (FD_ISSET(d_handle, &writeset)) {
                    rc = btlso::Flag::e_IO_RW;
                } else {
                    rc = btlso::Flag::e_IO_READ;
                }
            } else {
                rc = btlso::Flag::e_IO_WRITE;
            }
        }
        else if (0 == ret) {
            rc = SocketHandle::e_ERROR_TIMEDOUT;
        }
        else if (WSAEINTR == WSAGetLastError()) {
            rc = SocketHandle::e_ERROR_INTERRUPTED;
        } else {
            rc = SocketHandle::e_ERROR_UNCLASSIFIED;
        }
#endif
      } break;
    }

    return rc;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::waitForIO(btlso::Flag::IOWaitType type)
{
    int rc = SocketHandle::e_ERROR_UNCLASSIFIED;

    switch (type) {
      case btlso::Flag::e_IO_READ: {
#ifdef BSLS_PLATFORM_OS_UNIX
        struct pollfd fds;
        fds.fd = d_handle;
        fds.events = POLLIN;
        fds.revents = 0;

        int milliseconds = -1;

        int ret = ::poll(&fds, 1, milliseconds);

        if (ret > 0) {
            rc = btlso::Flag::e_IO_READ;
        }
        else if (EINTR == errno) {
            rc = SocketHandle::e_ERROR_INTERRUPTED;
        } else {
            rc = SocketHandle::e_ERROR_UNCLASSIFIED;
        }
#else
        fd_set readset;
        FD_ZERO(&readset);
        FD_SET(d_handle, &readset);

        int ret = ::select(d_handle + 1, &readset, 0, 0, 0);

        if (ret > 0) {
            rc = btlso::Flag::e_IO_READ;
        }
        else if (WSAEINTR == WSAGetLastError()) {
            rc = SocketHandle::e_ERROR_INTERRUPTED;
        } else {
            rc = SocketHandle::e_ERROR_UNCLASSIFIED;
        }
#endif
      } break;
      case btlso::Flag::e_IO_WRITE: {
#ifdef BSLS_PLATFORM_OS_UNIX
        struct pollfd fds;
        fds.fd = d_handle;
        fds.events = POLLOUT;
        fds.revents = 0;

        int milliseconds = -1;

        int ret = ::poll(&fds, 1, milliseconds);

        if (ret > 0) {
            rc = btlso::Flag::e_IO_WRITE;
        }
        else if (EINTR == errno) {
            rc = SocketHandle::e_ERROR_INTERRUPTED;
        } else {
            rc = SocketHandle::e_ERROR_UNCLASSIFIED;
        }
#else
        fd_set writeset;
        FD_ZERO(&writeset);
        FD_SET(d_handle, &writeset);

        int ret = ::select(d_handle + 1, 0, &writeset, 0, 0);

        if (ret > 0) {
            rc = btlso::Flag::e_IO_WRITE;
        }
        else if (WSAEINTR == WSAGetLastError()) {
            rc = SocketHandle::e_ERROR_INTERRUPTED;
        } else {
            rc = SocketHandle::e_ERROR_UNCLASSIFIED;
        }
#endif
      } break;
      case btlso::Flag::e_IO_RW: {
#ifdef BSLS_PLATFORM_OS_UNIX
        struct pollfd fds;
        fds.fd = d_handle;
        fds.events = POLLIN | POLLOUT;
        fds.revents = 0;

        int milliseconds = -1;

        int ret = ::poll(&fds, 1, milliseconds);

        if (ret > 0) {
            if (fds.revents & POLLIN) {
                if (fds.revents & POLLOUT) {
                    rc = btlso::Flag::e_IO_RW;
                } else {
                    rc = btlso::Flag::e_IO_READ;
                }
            } else {
                rc = btlso::Flag::e_IO_WRITE;
            }
        }
        if (EINTR == errno) {
            rc = SocketHandle::e_ERROR_INTERRUPTED;
        } else {
            rc = SocketHandle::e_ERROR_UNCLASSIFIED;
        }
#else
        fd_set readset;
        fd_set writeset;
        FD_ZERO(&readset);
        FD_ZERO(&writeset);
        FD_SET(d_handle, &readset);
        FD_SET(d_handle, &writeset);

        int ret = ::select(d_handle + 1, &readset, &writeset, 0, 0);

        if (ret > 0) {
            if (FD_ISSET(d_handle, &readset)) {
                if (FD_ISSET(d_handle, &writeset)) {
                    rc = btlso::Flag::e_IO_RW;
                } else {
                    rc = btlso::Flag::e_IO_READ;
                }
            } else {
                rc = btlso::Flag::e_IO_WRITE;
            }
        }
        else if (WSAEINTR == WSAGetLastError()) {
            rc = SocketHandle::e_ERROR_INTERRUPTED;
        } else {
            rc = SocketHandle::e_ERROR_UNCLASSIFIED;
        }
#endif
      } break;
    }

    return rc;
}

// ACCESSORS
template <class ADDRESS>
int InetStreamSocket<ADDRESS>::blockingMode(
                                       btlso::Flag::BlockingMode *result) const
{
    IoUtil::BlockingMode mode;
    int                  rc = IoUtil::getBlockingMode(&mode, d_handle);

    if (0 == rc) {
        *result = mode == IoUtil::e_BLOCKING
                  ? btlso::Flag::e_BLOCKING_MODE
                  : btlso::Flag::e_NONBLOCKING_MODE;
    }

    return rc;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::connectionStatus() const
{
    // Determine if the connection succeeded.
    //
    // The socket may be in one of the following states: 1) The connection is
    // open.  Return 0.  2) The earlier connection on this socket has been
    // terminated but the socket is still open.  Return ERROR_CONNDEAD 3) The
    // connection request has failed.  Return ERROR_CONNDEAD
    //
    // This operation is not valid when no connection has been attempted or the
    // connection request is still pending.

    ADDRESS peerAddr;
    int     result = peerAddress(&peerAddr);

    if (0 == result) {
        // The connection is open.

        return 0;                                                     // RETURN
    }

    return SocketHandle::e_ERROR_CONNDEAD;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::lingerOption(
                                       SocketOptUtil::LingerData *result) const
{
    const int rc = SocketOptUtil::getOption(result,
                                            handle(),
                                            SocketOptUtil::k_SOCKETLEVEL,
                                            SocketOptUtil::k_LINGER);

    return rc ? SocketHandle::e_ERROR_UNCLASSIFIED : 0;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::socketOption(int *result,
                                            int  level,
                                            int  option) const
{
    const int rc = SocketOptUtil::getOption(result, handle(), level, option);

    return rc ? SocketHandle::e_ERROR_UNCLASSIFIED : 0;
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::localAddress(ADDRESS *result) const
{
    return SocketImpUtil::getLocalAddress<ADDRESS>(result, d_handle);
}

template <class ADDRESS>
int InetStreamSocket<ADDRESS>::peerAddress(ADDRESS *result) const
{
    return SocketImpUtil::getPeerAddress<ADDRESS>(result, d_handle);
}

template <class ADDRESS>
SocketHandle::Handle InetStreamSocket<ADDRESS>::handle() const
{
    return d_handle;
}

                // --------------------------------
                // class InetStreamSocketCloseGuard
                // --------------------------------

// CREATORS
inline
InetStreamSocketCloseGuard::
InetStreamSocketCloseGuard(SocketHandle::Handle socketHandle)
: d_socketHandle(socketHandle)
, d_valid(1)
{
}

inline
InetStreamSocketCloseGuard::~InetStreamSocketCloseGuard()
{
    if (d_valid) {
        SocketImpUtil::close(d_socketHandle);
    }
}

// MANIPULATORS
inline
void InetStreamSocketCloseGuard::release()
{
    d_valid = 0;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
