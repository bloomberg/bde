// bteso_teststreamsocket.h      -*-C++-*-
#ifndef INCLUDED_BTESO_TESTSTREAMSOCKET
#define INCLUDED_BTESO_TESTSTREAMSOCKET

//@PURPOSE: Provide test implementation of a stream socket.
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES:
//  bteso_TestStreamSocket: stream socket for testing
//
//@SEE_ALSO: bteso_teststreamsocketfactory
//
//@DESCRIPTION:
//
///Configuration description
///-------------------------
// This`
//
///Thread-safety
///-------------
//
///Performance
///-----------
//
///USAGE EXAMPLE
///=============
//..
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_STREAMSOCKET
#include <bteso_streamsocket.h>
#endif

                        // ============================
                        // class bteso_TestStreamSocket
                        // ============================

namespace BloombergLP {

template <class ADDRESS>
class bteso_TestStreamSocket : public bteso_StreamSocket <ADDRESS> {
  // [TBD - Class description]
  public:
    // CREATORS
    ~bteso_TestStreamSocket();
        // Destroy this 'bteso_StreamSocket' object.

    // MANIPULATORS
    int accept(bteso_StreamSocket<ADDRESS> **result);
        // Accept an incoming connection request and load in the
        // specified 'result', the address of the new socket. Return
        // 0 on success, and a non-zero value otherwise. Note  that if
        // this socket is in non-blocking mode and there is no pending
        // connection request, this call returns immediately with an
        // error status of 'bteso_SocketHandle::ERROR_WOULDBLOCK'. In
        // blocking mode, this function waits until a connection request is
        // received or an error occurs. Also note that this socket
        // must be listening for connection ('listen').

    int accept(bteso_StreamSocket<ADDRESS> **socket,
               ADDRESS *peerAddress );
        // Accept an incoming connection request; load in the specified
        // 'socket', the address of the new socket and the address of the peer
        // in specified 'peerAddress' . Return 0 on success, and a non-zero
        // value otherwise.  Note that if this 'bteso_StreamSocket' is in
        // non-blocking mode and there is no pending connection request, this
        // call returns immediately with an error status of
        // 'bteso_SocketHandle::ERROR_WOULDBLOCK'.  If this socket is in
        // blocking mode, this function waits until a connection request is
        // received or an error occurs.  Also note that this socket must be
        // listening for connection('listen').

    int bind(const ADDRESS& address);
        // When a socket is created, it has no associated address. Associate
        // the specified 'address' with this socket. Note that in order to
        // receive connections on a socket, it must have an address associated
        // with it. Return 0 on success, and a non-zero value otherwise.

    int connect(const ADDRESS& address);
        // Initiate a connection to a peer process at the specified 'address'.
        // Return 0 on success, and a non-zero value otherwise.
        // If this socket is in non-blocking mode and the connection
        // cannot be established imediately
        // 'bteso_SocketHandle::ERROR_WOULDBLOCK' is returned.  The
        // 'waitForConnect' method may then be used to determine when the
        // connection request has completed.  In blocking mode, the call will
        // wait until a connection is established or an error occurs.

    int listen(int backlog);
        // Register this socket for accepting up to the specified 'backlog'
        // simulaneous connection requests.  Return 0 on success, and a
        // non-zero value otherwise.

    int read(char *buffer, int length);
        // Attempt to read up to 'length' bytes from this socket into the
        // specifed 'buffer'; return the non-negative number of bytes  read or,
        // a negative value on error.  In blocking mode, if there are less than
        // 'length' bytes of data available, the call waits for data to arrive.
        // In non-blocking mode, the function reads as many bytes as possible
        // without block, and returns the number of bytes read or
        // 'bteso_SocketHandle::ERROR_WOULDBLOCK', if zero bytes were
        // immediately available.  If the connection has been closed and
        // there is no data available, 'bteso_SocketHandle::EOF' is returned.
        // If the call is interrupted before data is available,
        // 'bteso_SocketHandle::INTERRUPTED' is returned.  The behavious is
        // undefined unless 0 < length and buffer provides capacity for at
        //  least 'length' bytes.

    int readv(btes_Iovec *buffers, int numBuffers);
        // Attempt to read from this socket into the specified sequence of
        // 'buffers' of specified sequence length 'numBuffers' the respective
        // numbers of bytes as defined by the 'd_length' fields of each
        // 'btes_Iovec' structure.  Return the non-negative total number of
        // bytes read or, a negative value on error.  In blocking mode, if
        // there are less than 'length' bytes of data available, the call
        // waits for data to arrive.  In non-blocking mode, the function reads
        // as many bytes as possible without blocking,  and returns the number
        // of bytes read or 'bteso_SocketHandle::ERROR_WOULDBLOCK', if zero
        // bytes were immediately available. If the connection has been closed,
        // and there is no data available, 'bteso_SocketHandle::EOF' is
        // returned.  If the call is interrupted before data is available,
        // 'bteso_SocketHandle::INTERRUPTED' is returned. The behaviour is
        // undefined unless 0 < 'numBuffers'.

    int write(const char *buffer, int length);
        // Attempt to write up to 'length' bytes to this socket from the
        // specified 'buffer'; return the non-negative number of bytes written
        // or, a negative value on error.  In blocking mode, if there is no
        // room to write the full message, the call will block until the
        // message is fully written.  In non-blocking mode, the function
        // writes as many bytes as possible without blocking, and returns the
        // number of bytes writen, or 'bteso_SocketHandle::ERROR_WOULDBLOCK',
        // if no bytes were written.  If the connection has been closed
        // 'bteso_SocketHandle::CONNDEAD' is returned.  If the call is
        // interrupted before any data is written,
        //  'bteso_SocketHandle::INTERRUPTED' is returned. The behaviour is
        // undefined unless 0 < 'length'.  Note that a successful call to this
        // function does not gaurantee that the data will be transmitted
        // successfully, but merely that the data was written successfully to
        // the underlying socket's transmit buffers.

    int writev(const btes_Iovec *buffers, int numBuffers);
        // Write to this socket from the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers' the respective numbers of
        // bytes as defined by the 'd_length' fields of each 'btes_Iovec'
        // structure; return the non-negative total number of bytes written or,
        // a negative value on error.  In blocking mode,  if there is no room
        // to write the the full message, the call will block until the
        // message is fully written. In non-blocking mode, the function
        // writes as many bytes as possible without blocking, and returns the
        // number of bytes writen,  or'bteso_SocketHandle::ERROR_WOULDBLOCK',
        // if no bytes were written. If the connection has been closed
        // 'bteso_SocketHandle::CONNDEAD' is returned. If the call is
        // interrupted before any data is written,
        // 'bteso_SocketHandle::INTERRUPTED' is returned. The
        // behaviour is undefined unless  0 <'numBuffers'. Note that a
        // successful call to this function does not gaurantee that the data
        // will be transmitted successfully, but merely that the data was
        // successfully written to the underlying socket's transmit buffers.

    int setBlockingMode(bteso_Flag::BlockingMode);
        // Set the current blocking mode of this socket to the
        // specified 'mode'.  Return 0 on success, an a non-zero value
        // otherwise.

    int shutdown(bteso_Flag::ShutdownType value);
        // Shutdown the specified input and/or output stream(s) of the
        // full-duplexed connection associated with this socket. Return 0 on
        // success, and a non-zero value otherwise. A 'value' of
        // 'SHUTDOWN_RECIEVE' or 'SHUTDOWN_SEND' will close the input or
        // output streams, respectively, leaving the other stream unaffected.
        // A 'value' of 'SHUTDOWN_BOTH' will close both streams.  Once the
        // input(output) stream has been closed, any attempted read(write)
        // operations will fail.  Any data received after successfully
        // shutting down the input stream will be acknowledged but silently
        // discarded.  Note that this function is typically used to transmit
        // an EOF indication to the peer.

     int waitForConnect(const bdet_TimeInterval& timeout);
        // Wait until a pending outgoing connection request completes or until
        // the specified absolute 'timeout' is reached, whichever occurs first.
        // Return 0 if the connection request has completed, and a non-zero
        // value otherwise.  Note that if a connection request has completed,
        // a second call to 'connect' must be made to establish the connection
        // or determine the reason for failure.

     int waitForAccept(const bdet_TimeInterval& timeout);
        // Wait for an incoming connection request on this socket or until the
        // specified absolute 'timeout' is reached, whichever occurs first.
        // Return 0 if a connection request has been received, and a non-zero
        // value otherwise.  Note that once a conection request has been
        // received, a call to 'accept' can be made to establish the
        // connection.

     int waitForIO(bteso_Flag::IOWaitType     type,
                   const bdet_TimeInterval&   timeout);
        // Wait for an I/O of the specifed 'type' to occurr or until the
        // specified absolute 'timeout' is reached, whichever occurs first.
        // Return 0 if the requested event occured, non-zero otherwise.  If
        // a timeout occcured, a value of return ERROR_TIMEOUT will be
        // returned.

     template<class T>
     int setOption(int level, int option, const T& value);
         // Set the socket option specified by 'option' for the
         // specified 'level' to the specified 'value'.  Return 0 on
         // success, and a non-zero value otherwise.

     template<class T>
     int getOption(T *result, int level, int option);
         // Load into the specified 'result', the value of the
         // specified 'option' of the specified 'level'. The behavior
         // is undefined if 'result' is 0;

    // ACCESSORS
    int blockingMode(bteso_Flag::BlockingMode *result) const;
        // Load into the specifed 'result', the current blocking mode of this
        // socket. Return 0 on success, and a non-zero value without affecting
        // 'result' otherwise.

    int localAddress(ADDRESS *result) const;
        // Load into the specified 'result', the local address of this socket.
        // Return 0 on success, and a non-zero value without affecting 'result'
        // otherwise.


    int peerAddress(ADDRESS *result) const;
        // Load into the specified 'result', the address of the peer connected
        // to this socket.  Return 0 on success, and a non-zero value without
        // affecting 'result' otherwise.


    bteso_SocketHandle::Handle handle() const;
        // Return the os handle associated with this socket.  Note that
        // direct manipulation of the underlying stream may result in
        // undefined behaviour.
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTIONS' DEFINITIONS
//-----------------------------------------------------------------------------

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
