// bteso_inetstreamsocketfactory.h       -*-C++-*-
#ifndef INCLUDED_BTESO_INETSTREAMSOCKETFACTORY
#define INCLUDED_BTESO_INETSTREAMSOCKETFACTORY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Implementation for TCP based stream sockets.
//
//@CLASSES:
//        bteso_InetStreamSocket: implementation of TCP-based stream-sockets
// bteso_InetStreamSocketFactory: factory for TCP-based stream-sockets
//
//@SEE_ALSO: bteso_streamsocket, bteso_ipv4address, btesos_tcpconnector
//
//@AUTHOR: Paul Staniforth
//
//@DESCRIPTION: This component implements TCP-based stream sockets together
// with a factory to allocate and deallocate them.  The stream sockets are of
// type 'bteso_InetStreamSocket<ADDRESS>' conforming to the
// 'bteso_StreamSocket<ADDRESS>' protocol.  The classes are templatized to
// provide type-safe address class specialization.  The only address type
// currently supported is IPv4 (as provided by 'bteso_ipv4address' component).
// Therefore, the template parameter will always be 'bteso_IPv4Address'.  The
// factory, 'bteso_InetStreamSocketFactory<ADDRESS>', creates and destroys
// instances of the 'bteso_InetStreamSocket<ADDRESS>'.  Two interfaces are
// available for creation of stream sockets.  One does not take a socket handle
// creates a new socket in the default initial state when a new stream socket
// is allocated.  The second takes the handle to an existing TCP-based stream
// socket and loads it into newly-allocated stream socket object.  In this
// case, no assumption is made about the state of the existing socket.  Every
// instance of 'bteso_InetStreamSocket<ADDRESS>' must be destroyed using the
// deallocate operation of 'bteso_InetStreamSocketFactory<ADDRESS>'.
//
// The creation of the socket factory provided by this component will enable
// socket operations (by calling 'bteso_SocketImpUtil::startup') method; the
// the destruction will disable socket operations (by calling
// 'bteso_SocketImpUtil::cleanup' method).
//
///Thread-safety
///-------------
// The classes provided by this component depend on a 'bslma_Allocator'
// instance to supply memory.  If the allocator is not thread enabled then the
// instances of this component that use the same allocator instance will
// consequently not be thread safe.  Otherwise, this component provides the
// following guarantees:
//
// The 'bteso_InetStreamSocketFactory' is *thread-enabled* and any thread can
// call any method *on the same instance* safely and the operations will
// proceed in parallel.  WARNING: simultaneous deallocation of the same
// instance of a stream socket may result in undefined behavior.  The
// 'bteso_InetStreamSocket' is not *thread-enabled* (it is not safe to invoke
// methods from different threads on the same instance).
// 'bteso_InetStreamSocket' is *thread-safe*, meaning that distinct threads can
// access distinct instances simultaneously without any side-effects (which
// usually means that there is no 'static' data).  The classes provided by this
// component are not *async-safe*, meaning that one or more functions cannot be
// invoked safely from a signal handler.
//
///Usage
///-----
// In this section we show intended usage of this component
//
///Example 1: Create a New Stream Socket
///- - - - - - - - - - - - - - - - - - -
// We can use 'bteso_InetStreamSocketFactory' to allocate a new TCP-based
// stream socket.
//
// First, we create a 'bteso_InetStreamSocketFactory' object:
//..
//  bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
//..
// Then, we create a stream socket:
//..
//  bteso_StreamSocket<bteso_IPv4Address> *mySocket = factory.allocate();
//  assert(mySocket);
//..
// 'mySocket' can now be used for TCP communication.
//
// Finally, when we're done, we recycle the socket:
//..
//  factory.deallocate(mySocket);
//..
//
///Example 2: Create a 'bteso_StreamSocket' Object From Existing Socket Handle
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Alternatively, we can use 'bteso_InetStreamSocketFactory' to allocate a
// 'bteso_StreamSocket' object that attaches to an existing socket handle.
// This socket handle may be created from a third-party library (such as
// OpenSSL).  Using a 'bteso_StreamSocket' object rather than the socket handle
// directly is highly desirable as it enables the use of other BTE components
// on the socket.  In this example, the socket handle is created from the
// 'bteso_socketimputil' component for illustrative purpose.
//
// First, we create a socket handle 'fd':
//..
//  bteso_SocketHandle::Handle fd;
//  int nativeErrNo = 0;
//  bteso_SocketImpUtil::open<bteso_IPv4Address>(
//                                    &fd,
//                                    bteso_SocketImpUtil::BTESO_SOCKET_STREAM,
//                                    &nativeErrNo);
//  assert(0 == nativeErrNo);
//..
// Then, we create factory:
//..
//  bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
//..
// Next, we allocate a stream socket attached to 'fd':
//..
//  bteso_StreamSocket<bteso_IPv4Address> *mySocket = factory.allocate(fd);
//  assert(mySocket);
//..
// Notice that 'fd' is passed into the 'allocate' method as an argument.  Any
// BTE component that uses 'bteso_StreamSocket<bteso_IPv4Address>' can now be
// used on 'mySocket'.
//
// Finally, when we're done, we recycle the socket:
//..
//  factory.deallocate(mySocket);
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_STREAMSOCKETFACTORY
#include <bteso_streamsocketfactory.h>
#endif

#ifndef INCLUDED_BTESO_STREAMSOCKET
#include <bteso_streamsocket.h>
#endif

#ifndef INCLUDED_BTESO_SOCKETIMPUTIL
#include <bteso_socketimputil.h>
#endif

#ifndef INCLUDED_BTESO_IOUTIL
#include <bteso_ioutil.h>
#endif

#ifndef INCLUDED_BTESO_SOCKETHANDLE
#include <bteso_sockethandle.h>
#endif

#ifndef INCLUDED_BTES_IOVEC
#include <btes_iovec.h>
#endif

#ifndef INCLUDED_BDETU_SYSTEMTIME
#include <bdetu_systemtime.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
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

#ifdef BSLS_PLATFORM__OS_UNIX

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

                    // ===================================
                    // class bteso_InetStreamSocketFactory
                    // ===================================

template <class ADDRESS>
class bteso_InetStreamSocketFactory : public bteso_StreamSocketFactory<ADDRESS>
{
    // Provide an implementation for a factory to allocate and deallocate
    // TCP-based stream socket objects of type
    // 'bteso_InetStreamSocket<ADDRESS>'.

    bslma_Allocator *d_allocator_p; // holds (but doesn't own) object

  public:
    // CREATORS
    explicit bteso_InetStreamSocketFactory(
                                          bslma_Allocator *basicAllocator = 0);
        // Create a stream socket factory.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    virtual ~bteso_InetStreamSocketFactory();
        // Destroy this factory.  The stream sockets created by this factory
        // will not be destroyed when this factory is destroyed.

    // MANIPULATORS
    virtual bteso_StreamSocket<ADDRESS> *allocate();
        // Create a stream-based socket of type
        // 'bteso_InetStreamSocket<ADDRESS>'.  Return the address of the
        // newly-created socket on success, and 0 otherwise.

    virtual bteso_StreamSocket<ADDRESS> *allocate(
                                            bteso_SocketHandle::Handle handle);
        // Create a stream-based socket of type
        // 'bteso_InetStreamSocket<ADDRESS>' attached to the specified socket
        // 'handle'.  Return the address of the newly-created socket on
        // success, and 0 otherwise.  The behavior is undefined unless 'handle'
        // refers to a valid TCP-based stream socket with the address class
        // 'ADDRESS'.

    virtual void deallocate(bteso_StreamSocket<ADDRESS> *socket);
        // Return the specified 'socket' back to this factory.  The behavior is
        // undefined unless 'socket' was allocated using this factory or was
        // created through an 'accept' from a stream socket created using this
        // factory, and has not already been deallocated.  Note that the
        // underlying TCP-based stream socket will be closed.

    virtual void deallocate(bteso_StreamSocket<ADDRESS> *socket,
                            bool                         closeHandleFlag);
        // Return the specified 'socket' back to this factory and, if the
        // specified 'closeHandleFlag' is 'true', also close the underlying
        // TCP-based stream socket.  If 'closeHandleFlag' is 'false', then
        // ownership of the underlying TCP-based stream socket is transferred
        // to the caller and the TCP-based socket is left open.  The behavior
        // is undefined unless 'socket' was allocated using this factory or was
        // created through an 'accept' from a stream socket created using this
        // factory, and has not already been deallocated.
};

                        // ============================
                        // class bteso_InetStreamSocket
                        // ============================

template <class ADDRESS>
class bteso_InetStreamSocket : public bteso_StreamSocket<ADDRESS> {
    // This class implements the 'bteso_StreamSocket<ADDRESS>' protocol to
    // provide stream-based socket communications.  The class is templatized to
    // provide a family of type-safe address specializations (e.g., "IPv4",
    // "IPv6").  Various socket-related operations, including accepting and
    // initiating connections and blocking/non-blocking I/O operations, are
    // provided.  Vector I/O operations are also supported.

    // DATA
    bteso_SocketHandle::Handle d_handle;

    bslma_Allocator           *d_allocator_p; // holds (but doesn't own) object

    // PRIVATE CREATORS
    bteso_InetStreamSocket(bteso_SocketHandle::Handle handle,
                           bslma_Allocator           *allocator);
        // Create a stream socket attached to the specified socket 'handle'
        // that uses the specified 'allocator' to supply memory.  If
        // 'allocator' is 0, the currently installed default allocator is used.
        // The behavior is undefined unless socket 'handle' refers to a valid
        // system socket.

  private:
    // NOT IMPLEMENTED
    bteso_InetStreamSocket(const bteso_InetStreamSocket<ADDRESS>&);
    bteso_InetStreamSocket<ADDRESS>& operator=(
                                       const bteso_InetStreamSocket<ADDRESS>&);

    // FRIENDS
    friend class bteso_InetStreamSocketFactory<ADDRESS>;

  public:
    // CREATORS
    virtual ~bteso_InetStreamSocket<ADDRESS>();
        // Destroy this 'bteso_StreamSocket' object.

    // MANIPULATORS
    virtual int accept(bteso_StreamSocket<ADDRESS> **socket);
        // Accept an incoming connection request and load the address of a new
        // stream socket of type 'bteso_InetStreamSocket<ADDRESS>' into the
        // specified 'socket'.  Return 0 on success, and a non-zero value
        // otherwise.  If this socket is in blocking mode, this function waits
        // until a connection request is received or an error occurs.  If this
        // socket is in non-blocking mode and there is no pending connection
        // request, this call returns immediately with an error status of
        // 'bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK'.  Note that this socket
        // must be listening for connections ('listen').

    virtual int accept(bteso_StreamSocket<ADDRESS> **socket,
                       ADDRESS                      *peerAddress );
        // Accept an incoming connection request, load the address of a new
        // stream socket of type 'bteso_InetStreamSocket<ADDRESS>' into the
        // specified 'socket', and load the address of the peer into the
        // specified 'peerAddress'.  Return 0 on success, and a non-zero value
        // otherwise.  If this socket is in blocking mode, this function waits
        // until a connection request is received or an error occurs.  If this
        // socket is in non-blocking mode and there is no pending connection
        // request, this call returns immediately with an error status of
        // 'bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK'.  Note that this socket
        // must be listening for connections ('listen').

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
        // 'bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK' is returned.  The
        // 'waitForConnect' method may then be used to determine when the
        // connection request has completed.

    virtual int listen(int backlog);
        // Register this socket for accepting up to the specified 'backlog'
        // simultaneous connection requests.  Return 0 on success, and a
        // non-zero value otherwise.  The behavior is undefined unless
        // 0 < backlog.

    virtual int read(char *buffer, int length);
        // Read up to the specified 'length' bytes from this socket into the
        // specified 'buffer'; return the non-negative number of bytes read, or
        // a negative value on error.  If this socket is in blocking mode, if
        // there are less than 'length' bytes of data available, the call waits
        // for data to arrive.  If this socket is in non-blocking mode, the
        // function reads as many bytes as possible without blocking, and
        // returns the number of bytes read, or
        // 'bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK' if zero bytes were
        // immediately available.  If the connection has been closed and there
        // is no data available, 'bteso_SocketHandle::BTESO_ERROR_EOF' is
        // returned.  If the call is interrupted before data is available,
        // 'bteso_SocketHandle::BTESO_ERROR_INTERRUPTED' is returned.  The
        // behavior is undefined unless 0 < length and 'buffer' refers to at
        // least 'length' writable bytes.

    virtual int readv(const btes_Iovec *buffers, int numBuffers);
        // Read from this socket into the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers' the respective numbers of
        // bytes as defined by the 'length' methods of each 'btes_Iovec'
        // structure.  Return the non-negative total number of bytes read, or a
        // negative value on error.  If this socket is in blocking mode, if
        // there are less than 'length' bytes of data available, the call waits
        // for data to arrive.  If this socket is in non-blocking mode, the
        // function reads as many bytes as possible without blocking, and
        // returns the number of bytes read or
        // 'bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK' if zero bytes were
        // immediately available.  If the connection has been closed, and there
        // is no data available, 'bteso_SocketHandle::BTESO_ERROR_EOF' is
        // returned.  If the call is interrupted before data is available,
        // 'bteso_SocketHandle::BTESO_ERROR_INTERRUPTED' is returned.  The
        // behavior is undefined unless 0 < numBuffers.

    virtual int write(const char *buffer, int length);
        // Write up to the specified 'length' bytes to this socket from the
        // specified 'buffer'; return the non-negative number of bytes written,
        // or a negative value on error.  If this socket is in blocking mode
        // the call blocks until the data is fully written.  If this socket is
        // in non-blocking mode, the function writes as many bytes as possible
        // without blocking, and returns the number of bytes written, or
        // 'bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK' if no bytes were
        // written.  If the connection has been closed,
        // 'bteso_SocketHandle::BTESO_ERROR_CONNDEAD' is returned.  If the call
        // is interrupted before any data is written,
        // 'bteso_SocketHandle::BTESO_ERROR_INTERRUPTED' is returned.  The
        // behavior is undefined unless 0 < length.  Note that a successful
        // call to this function does not guarantee that the data has been
        // transmitted successfully, but simply that the data was written
        // successfully to the underlying socket's transmit buffers.

    virtual int writev(const btes_Iovec *buffers,
                       int               numBuffers);
    virtual int writev(const btes_Ovec  *buffers,
                       int               numBuffers);
        // Write to this socket from the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers' the respective numbers of
        // bytes as reported by the 'length' methods of each 'btes_Ovec'
        // structure; return the non-negative total number of bytes written, or
        // a negative value on error.  If this socket is in blocking mode the
        // call blocks until the data is fully written.  If this socket is in
        // non-blocking mode, the function writes as many bytes as possible
        // without blocking, and returns the number of bytes written, or
        // 'bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK', if no bytes were
        // written.  If the connection has been closed,
        // 'bteso_SocketHandle::BTESO_ERROR_CONNDEAD' is returned.  If the call
        // is interrupted before any data is written,
        // 'bteso_SocketHandle::BTESO_ERROR_INTERRUPTED' is returned.  The
        // behavior is undefined unless 0 < numBuffers.  Note that a successful
        // call to this function does not guarantee that the data has been
        // transmitted successfully, but simply that the data was successfully
        // written to the underlying socket's transmit buffers.

    virtual int setBlockingMode(bteso_Flag::BlockingMode mode);
        // Set the current blocking mode of this socket to the specified
        // 'mode'.  Return 0 on success, an a non-zero value otherwise.

    virtual int shutdown(bteso_Flag::ShutdownType streamOption);
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

    virtual int waitForConnect(const bdet_TimeInterval& timeout);
        // Wait until a pending outgoing connection request completes or until
        // the specified absolute 'timeout' is reached, whichever occurs first.
        // Return 0 if the connection request has completed, and a non-zero
        // value otherwise.  Note that the completion of a connection request
        // does not imply that a connection has been successfully established;
        // a call to 'connectionStatus' should be made to determine if the
        // connection request completed successfully.

    virtual int waitForAccept(const bdet_TimeInterval& timeout);
        // Wait for an incoming connection request on this socket or until the
        // specified absolute 'timeout' is reached, whichever occurs first.
        // Return 0 if a connection request has been received, and a non-zero
        // value otherwise.  Note that once a connection request has been
        // received, a call to 'accept' can be made to establish the
        // connection.

    virtual int waitForIO(bteso_Flag::IOWaitType   type,
                          const bdet_TimeInterval& timeout);
        // Wait for an I/O of the specified 'type' to occur or until the
        // specified absolute 'timeout' is reached, whichever occurs first.
        // Return IO_READ, IO_WRITE or IO_RW if the corresponding event
        // occurred, and a non-zero value otherwise.  If a timeout occurred, a
        // value of 'bteso_SocketHandle::BTESO_ERROR_TIMEDOUT' is returned.  If
        // this call is interrupted,
        // 'bteso_SocketHandle::BTESO_ERROR_INTERRUPTED' is returned.

    virtual int waitForIO(bteso_Flag::IOWaitType type);
        // Wait for an I/O of the specified 'type' to occur.  Return 'type' if
        // the corresponding event occurred, and a negative value otherwise.
        // If this call is interrupted,
        // 'bteso_SocketHandle::BTESO_ERROR_INTERRUPTED' is returned.

    virtual int setLingerOption(
                               const bteso_SocketOptUtil::LingerData& options);
        // Set the current linger options of this socket to the specified
        // 'options'.  Return 0 on success, and a non-zero value otherwise.

    virtual int setOption(int level, int option, int value);
        // Set the specified socket 'option' (of the specified 'level') on this
        // socket to the specified 'value'.  Return 0 on success and a non-zero
        // value otherwise.  See 'bteso_socketoptutil' for the list of commonly
        // supported options.

    // ACCESSORS
    virtual int blockingMode(bteso_Flag::BlockingMode *result) const;
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

    virtual bteso_SocketHandle::Handle handle() const;
        // Return the OS handle associated with this socket.  Note that direct
        // manipulation of the underlying stream may result in undefined
        // behavior.

    virtual int connectionStatus() const;
        // Test the connection status of this socket.  Return 0 if this socket
        // has an established connection, and a non-zero value otherwise.  If
        // there is no established or pending connection, a value of
        // 'bteso_SocketHandle::BTESO_ERROR_CONNDEAD' is returned.  A value of
        // 'bteso_SocketHandle::BTESO_ERROR_CONNDEAD' is also returned if a
        // non-blocking connection request fails.  Note that this method is
        // typically used to determine the result of a non-blocking connection
        // request.

    virtual int lingerOption(bteso_SocketOptUtil::LingerData *result) const;
        // Load into the specified 'result' the value of this socket's current
        // linger options.  The behavior is undefined if 'result' is 0;

    virtual int socketOption(int *result, int level, int option) const;
        // Load int the specified 'result' the value of the specified socket
        // 'option' of the specified 'level' socket option on this socket.
        // Return 0 on success and a non-zero value otherwise.
};

                // ============================================
                // class bteso_InetStreamSocket_AutoCloseSocket
                // ============================================

class bteso_InetStreamSocket_AutoCloseSocket {
    // This class implements a proctor that automatically closes the managed
    // socket at destruction unless its 'release' method is invoked.  Note that
    // we can't use a 'bteso_StreamSocketFactoryAutoDeallocateGuard' since
    // class that does not operator on socket handles.

    // DATA
    bteso_SocketHandle::Handle d_socketHandle;   // managed socket handle
    int                        d_valid;          // true until 'release' called

  private:
    // NOT IMPLEMENTED
    bteso_InetStreamSocket_AutoCloseSocket(
                                const bteso_InetStreamSocket_AutoCloseSocket&);
    bteso_InetStreamSocket_AutoCloseSocket& operator=(
                                const bteso_InetStreamSocket_AutoCloseSocket&);
  public:
    // CREATORS
    explicit bteso_InetStreamSocket_AutoCloseSocket(
                                      bteso_SocketHandle::Handle socketHandle);
        // Create a proctor object to manage socket having the specified
        // 'socketHandle'.

    ~bteso_InetStreamSocket_AutoCloseSocket();
        // Destroy this proctor object and, unless the 'release' method has
        // been previously called, close the managed socket.

    // MANIPULATORS
    void release();
        // Release from management the socket currently managed by this
        // proctor.  If no socket is currently being managed, this operation
        // has no effect.
};

// ===========================================================================
// TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================

                     // ----------------------------
                     // class bteso_InetStreamSocket
                     // ----------------------------

// PRIVATE CREATORS
template <class ADDRESS>
bteso_InetStreamSocket<ADDRESS>::bteso_InetStreamSocket(
                                         bteso_SocketHandle::Handle  handle,
                                         bslma_Allocator            *allocator)
: d_handle(handle)
, d_allocator_p(allocator)
{
}

// CREATORS
template <class ADDRESS>
bteso_InetStreamSocket<ADDRESS>::~bteso_InetStreamSocket()
{
}

// MANIPULATORS
template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::accept(
                                          bteso_StreamSocket<ADDRESS> **socket)
{
    bteso_SocketHandle::Handle newHandle;

    int ret = bteso_SocketImpUtil::accept<ADDRESS>(&newHandle, d_handle);
    if (ret != 0) {
        return ret;                                                   // RETURN
    }

    bteso_InetStreamSocket_AutoCloseSocket autoDeallocate(newHandle);

    *socket = new (*d_allocator_p) bteso_InetStreamSocket<ADDRESS>(
                                                     newHandle, d_allocator_p);

    autoDeallocate.release();

    return 0;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::accept(
                                    bteso_StreamSocket<ADDRESS>  **socket,
                                    ADDRESS                       *peerAddress)
{
    bteso_SocketHandle::Handle newHandle;

    int ret = bteso_SocketImpUtil::accept<ADDRESS>(&newHandle, peerAddress,
                                                   d_handle);
    if (ret != 0) {
        return ret;                                                   // RETURN
    }

    *socket = new (*d_allocator_p) bteso_InetStreamSocket<ADDRESS>(
         newHandle, d_allocator_p);
    return 0;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::bind(const ADDRESS& address)
{
    int ret = bteso_SocketImpUtil::bind<ADDRESS>(d_handle, address);

    return ret;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::connect(const ADDRESS& address)
{
    int ret = bteso_SocketImpUtil::connect<ADDRESS>(d_handle, address);

    return ret;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::listen(int backlog)
{
    int ret = bteso_SocketImpUtil::listen(d_handle, backlog);

    return ret;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::read(char *buffer, int length)
{
    int ret = bteso_SocketImpUtil::read(buffer, d_handle, length);

    if (length > 0 && ret == 0) {
        ret = bteso_SocketHandle::BTESO_ERROR_EOF;
    }

    return ret;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::readv(const btes_Iovec   *buffers,
                                           int                 numBuffers)
{
    int ret = bteso_SocketImpUtil::readv(buffers, d_handle, numBuffers);

    if (ret == 0) {
        // readv returns 0 if either the number of bytes to read was zero or if
        // an EOF occurred.

        int i;
        for (i = 0; i < numBuffers; ++i) {
            if (buffers[i].length()) {
                return bteso_SocketHandle::BTESO_ERROR_EOF;           // RETURN
            }
        }
    }

    return ret;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::write(const char *buffer, int length)
{
    int ret = bteso_SocketImpUtil::write(d_handle, buffer, length);

    return ret;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::writev(const btes_Iovec   *buffers,
                                            int                 numBuffers)
{
    int ret = bteso_SocketImpUtil::writev(d_handle,
                                          (const btes_Ovec *) buffers,
                                          numBuffers);

    return ret;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::writev(const btes_Ovec   *buffers,
                                            int                numBuffers)
{
    int ret = bteso_SocketImpUtil::writev(d_handle, buffers, numBuffers);

    return ret;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::setBlockingMode(
                                                 bteso_Flag::BlockingMode mode)
{
    return bteso_IoUtil::setBlockingMode(
                                       d_handle,
                                       mode == bteso_Flag::BTESO_BLOCKING_MODE
                                       ? bteso_IoUtil::BTESO_BLOCKING
                                       : bteso_IoUtil::BTESO_NONBLOCKING);
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::setLingerOption(
                                const bteso_SocketOptUtil::LingerData& options)
{
    return bteso_SocketOptUtil::setOption(handle(),
          bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
          bteso_SocketOptUtil::BTESO_LINGER,
          options) ? bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED : 0;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::setOption(int level,
                                               int option,
                                               int value)
{
    return bteso_SocketOptUtil::setOption(handle(),
          level, option,
          value) ? bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED : 0;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::shutdown(bteso_Flag::ShutdownType value)
{
    static enum bteso_SocketImpUtil::ShutDownType shutDownTypeMapping[] = {
         bteso_SocketImpUtil::BTESO_SHUTDOWN_RECEIVE,
         bteso_SocketImpUtil::BTESO_SHUTDOWN_SEND,
         bteso_SocketImpUtil::BTESO_SHUTDOWN_BOTH
    };
    BSLMF_ASSERT(bteso_Flag::BTESO_SHUTDOWN_RECEIVE == 0 &&
                 bteso_Flag::BTESO_SHUTDOWN_SEND == 1 &&
                 bteso_Flag::BTESO_SHUTDOWN_BOTH == 2);

    int ret = bteso_SocketImpUtil::shutDown(d_handle,
                                            shutDownTypeMapping[value]);

    return ret;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::waitForConnect(
                                              const bdet_TimeInterval& timeout)
{
    bdet_TimeInterval interval = timeout - bdetu_SystemTime::now();
    int seconds = (int) interval.seconds();
    int microseconds = (interval.nanoseconds() + 999) / 1000;

    if (seconds < 0 || microseconds < 0) {
        seconds = 0;
        microseconds = 0;
    }

    int rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;

#ifdef BSLS_PLATFORM__OS_UNIX
    struct pollfd fds;
    fds.fd = d_handle;
    fds.events = POLLOUT;
    fds.revents = 0;

    int milliseconds = seconds * 1000 + (microseconds + 999) / 1000;

    int ret = ::poll(&fds, 1, milliseconds);

    if (ret > 0) {
        rc = 0;
    }
    else if (ret == 0) {
        rc = bteso_SocketHandle::BTESO_ERROR_TIMEDOUT;
    }
    else if (EINTR == errno) {
        rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
    }
    else {
        rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
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
    else if (ret == 0) {
        rc = bteso_SocketHandle::BTESO_ERROR_TIMEDOUT;
    }
    else if (WSAEINTR == WSAGetLastError()) {
        rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
    }
    else {
        rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
    }
#endif

    return rc;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::waitForAccept(
                                              const bdet_TimeInterval& timeout)
{
    return bteso_InetStreamSocket<ADDRESS>::waitForIO(
                                                     bteso_Flag::BTESO_IO_READ,
                                                     timeout);
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::waitForIO(
                                              bteso_Flag::IOWaitType   type,
                                              const bdet_TimeInterval& timeout)
{
    bdet_TimeInterval interval = timeout - bdetu_SystemTime::now();
    int seconds = (int) interval.seconds();
    int microseconds = (interval.nanoseconds() + 999) / 1000;

    if (seconds < 0 || microseconds < 0) {
        seconds = 0;
        microseconds = 0;
    }

    int rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;

    switch (type)
    {
      case bteso_Flag::BTESO_IO_READ:
        {
#ifdef BSLS_PLATFORM__OS_UNIX
            struct pollfd fds;
            fds.fd = d_handle;
            fds.events = POLLIN;
            fds.revents = 0;

            int milliseconds = seconds * 1000 + (microseconds + 999) / 1000;

            int ret = ::poll(&fds, 1, milliseconds);

            if (ret > 0) {
                rc = bteso_Flag::BTESO_IO_READ;
            }
            else if (ret == 0) {
                rc = bteso_SocketHandle::BTESO_ERROR_TIMEDOUT;
            }
            else if (EINTR == errno) {
                rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
            } else {
                rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
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
                rc = bteso_Flag::BTESO_IO_READ;
            }
            else if (ret == 0) {
                rc = bteso_SocketHandle::BTESO_ERROR_TIMEDOUT;
            }
            else if (WSAEINTR == WSAGetLastError()) {
                rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
            } else {
                rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
            }
#endif
        } break;
      case bteso_Flag::BTESO_IO_WRITE:
        {
#ifdef BSLS_PLATFORM__OS_UNIX
            struct pollfd fds;
            fds.fd = d_handle;
            fds.events = POLLOUT;
            fds.revents = 0;

            int milliseconds = seconds * 1000 + (microseconds + 999) / 1000;

            int ret = ::poll(&fds, 1, milliseconds);

            if (ret > 0) {
                rc = bteso_Flag::BTESO_IO_WRITE;
            }
            else if (ret == 0) {
                rc = bteso_SocketHandle::BTESO_ERROR_TIMEDOUT;
            }
            else if (EINTR == errno) {
                rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
            } else {
                rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
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
                rc = bteso_Flag::BTESO_IO_WRITE;
            }
            else if (ret == 0) {
                rc = bteso_SocketHandle::BTESO_ERROR_TIMEDOUT;
            }
            else if (WSAEINTR == WSAGetLastError()) {
                rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
            } else {
                rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
            }
#endif
        } break;
      case bteso_Flag::BTESO_IO_RW:
        {
#ifdef BSLS_PLATFORM__OS_UNIX
            struct pollfd fds;
            fds.fd = d_handle;
            fds.events = POLLIN | POLLOUT;
            fds.revents = 0;

            int milliseconds = seconds * 1000 + (microseconds + 999) / 1000;

            int ret = ::poll(&fds, 1, milliseconds);

            if (ret > 0) {
                if (fds.revents & POLLIN) {
                    if (fds.revents & POLLOUT) {
                        rc = bteso_Flag::BTESO_IO_RW;
                    } else {
                        rc = bteso_Flag::BTESO_IO_READ;
                    }
                } else {
                    rc = bteso_Flag::BTESO_IO_WRITE;
                }
            }
            else if (ret == 0) {
                rc = bteso_SocketHandle::BTESO_ERROR_TIMEDOUT;
            }
            else if (EINTR == errno) {
                rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
            } else {
                rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
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
                          rc = bteso_Flag::BTESO_IO_RW;
                    } else {
                          rc = bteso_Flag::BTESO_IO_READ;
                    }
                } else {
                    rc = bteso_Flag::BTESO_IO_WRITE;
                }
            }
            else if (ret == 0) {
                rc = bteso_SocketHandle::BTESO_ERROR_TIMEDOUT;
            }
            else if (WSAEINTR == WSAGetLastError()) {
                rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
            } else {
                rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
            }
#endif
        } break;
    }

    return rc;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::waitForIO(bteso_Flag::IOWaitType type)
{
    int rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;

    switch (type)
    {
      case bteso_Flag::BTESO_IO_READ:
        {
#ifdef BSLS_PLATFORM__OS_UNIX
            struct pollfd fds;
            fds.fd = d_handle;
            fds.events = POLLIN;
            fds.revents = 0;

            int milliseconds = -1;

            int ret = ::poll(&fds, 1, milliseconds);

            if (ret > 0) {
                rc = bteso_Flag::BTESO_IO_READ;
            }
            else if (EINTR == errno) {
                rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
            } else {
                rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
            }
#else
            fd_set readset;
            FD_ZERO(&readset);
            FD_SET(d_handle, &readset);

            int ret = ::select(d_handle + 1, &readset, 0, 0, 0);

            if (ret > 0) {
                rc = bteso_Flag::BTESO_IO_READ;
            }
            else if (WSAEINTR == WSAGetLastError()) {
                rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
            } else {
                rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
            }
#endif
        } break;
      case bteso_Flag::BTESO_IO_WRITE:
        {
#ifdef BSLS_PLATFORM__OS_UNIX
            struct pollfd fds;
            fds.fd = d_handle;
            fds.events = POLLOUT;
            fds.revents = 0;

            int milliseconds = -1;

            int ret = ::poll(&fds, 1, milliseconds);

            if (ret > 0) {
                rc = bteso_Flag::BTESO_IO_WRITE;
            }
            else if (EINTR == errno) {
                rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
            } else {
                rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
            }
#else
            fd_set writeset;
            FD_ZERO(&writeset);
            FD_SET(d_handle, &writeset);

            int ret = ::select(d_handle + 1, 0, &writeset, 0, 0);

            if (ret > 0) {
                rc = bteso_Flag::BTESO_IO_WRITE;
            }
            else if (WSAEINTR == WSAGetLastError()) {
                rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
            } else {
                rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
            }
#endif
        } break;
      case bteso_Flag::BTESO_IO_RW:
        {
#ifdef BSLS_PLATFORM__OS_UNIX
            struct pollfd fds;
            fds.fd = d_handle;
            fds.events = POLLIN | POLLOUT;
            fds.revents = 0;

            int milliseconds = -1;

            int ret = ::poll(&fds, 1, milliseconds);

            if (ret > 0) {
                if (fds.revents & POLLIN) {
                    if (fds.revents & POLLOUT) {
                        rc = bteso_Flag::BTESO_IO_RW;
                    } else {
                        rc = bteso_Flag::BTESO_IO_READ;
                    }
                } else {
                    rc = bteso_Flag::BTESO_IO_WRITE;
                }
            }
            if (EINTR == errno) {
                rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
            } else {
                rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
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
                          rc = bteso_Flag::BTESO_IO_RW;
                    } else {
                          rc = bteso_Flag::BTESO_IO_READ;
                    }
                } else {
                    rc = bteso_Flag::BTESO_IO_WRITE;
                }
            }
            else if (WSAEINTR == WSAGetLastError()) {
                rc = bteso_SocketHandle::BTESO_ERROR_INTERRUPTED;
            } else {
                rc = bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED;
            }
#endif
        } break;
    }

    return rc;
}

// ACCESSORS
template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::blockingMode(
                                        bteso_Flag::BlockingMode *result) const
{
    bteso_IoUtil::BlockingMode mode;
    int res = bteso_IoUtil::getBlockingMode(&mode, d_handle);

    if (res == 0) {
        *result = (mode == bteso_IoUtil::BTESO_BLOCKING)
                ? bteso_Flag::BTESO_BLOCKING_MODE
                : bteso_Flag::BTESO_NONBLOCKING_MODE;
    }

    return res;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::connectionStatus() const
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
    int result = peerAddress(&peerAddr);

    if (result == 0) {
        // The connection is open.

        return 0;                                                     // RETURN
    }

    return bteso_SocketHandle::BTESO_ERROR_CONNDEAD;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::lingerOption(
                                 bteso_SocketOptUtil::LingerData *result) const
{
    return bteso_SocketOptUtil::getOption(
                                        result,
                                        handle(),
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_LINGER)
           ? bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED
           : 0;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::socketOption(int *result,
                                                  int  level,
                                                  int  option) const
{
    return bteso_SocketOptUtil::getOption(result, handle(), level, option)
           ? bteso_SocketHandle::BTESO_ERROR_UNCLASSIFIED
           : 0;
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::localAddress(ADDRESS *result) const
{
    return bteso_SocketImpUtil::getLocalAddress<ADDRESS>(result, d_handle);
}

template <class ADDRESS>
int bteso_InetStreamSocket<ADDRESS>::peerAddress(ADDRESS *result) const
{
    return bteso_SocketImpUtil::getPeerAddress<ADDRESS>(result, d_handle);
}

template <class ADDRESS>
bteso_SocketHandle::Handle bteso_InetStreamSocket<ADDRESS>::handle() const
{
    return d_handle;
}

                    // -----------------------------------
                    // class bteso_InetStreamSocketFactory
                    // -----------------------------------

// CREATORS
template <class ADDRESS>
inline
bteso_InetStreamSocketFactory<ADDRESS>::bteso_InetStreamSocketFactory(
                                               bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    bteso_SocketImpUtil::startup();
}

template <class ADDRESS>
inline
bteso_InetStreamSocketFactory<ADDRESS>::~bteso_InetStreamSocketFactory()
{
    bteso_SocketImpUtil::cleanup();
}

// MANIPULATORS
template <class ADDRESS>
bteso_StreamSocket<ADDRESS> *bteso_InetStreamSocketFactory<ADDRESS>::allocate()
{
    bteso_SocketHandle::Handle newSocketHandle;

    int ret = bteso_SocketImpUtil::open<ADDRESS>(
                   &newSocketHandle, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);

    if (ret < 0) {
        return 0;                                                     // RETURN
    }

    bteso_InetStreamSocket_AutoCloseSocket autoDeallocate(newSocketHandle);

    // On successfully opening a new socket, create a new
    // 'bteso_InetStreamSocket' for this socket.

    bteso_StreamSocket<ADDRESS> *newStreamSocket =  new (*d_allocator_p)
                               bteso_InetStreamSocket<ADDRESS>(newSocketHandle,
                                                               d_allocator_p);

    autoDeallocate.release();

    return newStreamSocket;
}

template <class ADDRESS>
bteso_StreamSocket<ADDRESS> *bteso_InetStreamSocketFactory<ADDRESS>::allocate(
        bteso_SocketHandle::Handle socketHandle)
{
    // Create a new 'bteso_InetStreamSocket' for this socket.

    return new (*d_allocator_p) bteso_InetStreamSocket<ADDRESS>(
                                                  socketHandle, d_allocator_p);
}

template <class ADDRESS>
void bteso_InetStreamSocketFactory<ADDRESS>::deallocate(
        bteso_StreamSocket<ADDRESS> * socket)
{
    bteso_SocketImpUtil::close(socket->handle());

    ((bteso_InetStreamSocket<ADDRESS> *) socket)->
                                            ~bteso_InetStreamSocket<ADDRESS>();
    d_allocator_p->deallocate(socket);
}

template <class ADDRESS>
void bteso_InetStreamSocketFactory<ADDRESS>::deallocate(
        bteso_StreamSocket<ADDRESS> * socket,
        bool                          closeHandleFlag)
{
    if (closeHandleFlag) {
        bteso_SocketImpUtil::close(socket->handle());
    }

    ((bteso_InetStreamSocket<ADDRESS> *) socket)->
                                            ~bteso_InetStreamSocket<ADDRESS>();
    d_allocator_p->deallocate(socket);
}

                // --------------------------------------------
                // class bteso_InetStreamSocket_AutoCloseSocket
                // --------------------------------------------

// CREATORS
inline
bteso_InetStreamSocket_AutoCloseSocket::
bteso_InetStreamSocket_AutoCloseSocket(bteso_SocketHandle::Handle socketHandle)
: d_socketHandle(socketHandle)
, d_valid(1)
{
}

inline
bteso_InetStreamSocket_AutoCloseSocket::
~bteso_InetStreamSocket_AutoCloseSocket()
{
    if (d_valid) {
        bteso_SocketImpUtil::close(d_socketHandle);
    }
}

// MANIPULATORS
inline
void bteso_InetStreamSocket_AutoCloseSocket::release()
{
    d_valid = 0;
}

} // close namespace BloombergLP

#endif
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
