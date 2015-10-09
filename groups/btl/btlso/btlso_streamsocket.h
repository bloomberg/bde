// btlso_streamsocket.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_STREAMSOCKET
#define INCLUDED_BTLSO_STREAMSOCKET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Define a protocol for stream-based socket communications.
//
//@CLASSES:
//  btlso::StreamSocket: stream-socket protocol class
//  btlso::StreamSocketAutoClose: proctor for managing socket shutdown
//
//@SEE_ALSO: btlso_streamsocketfactory btlso_ipv4address
//
//@DESCRIPTION: This component defines an abstract interface (protocol) for
// stream-based socket communications.  'btlso::StreamSocket<ADDRESS>' defines
// an interface for accepting and initiating connections, and for setting
// various modes (e.g., non-blocking) and options on the underlying socket.
// Efficient vector I/O operations are also supported, such as UNIX-style
// 'readv', 'writev', and 'select'.  The protocol class is templatized to
// provide type-safe address class specialization (e.g., "IPv4", "IPv6").
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a Simple Client
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates a simple dictionary client.  The client program
// connects to a dictionary server at a know address and port, transmits a word
// to be defined, and reads and prints the definition.
//
// Note that the signature of 'dictionary_client' is unconventional because it
// takes a pointer to an allocated, but unbound socket.  A better parameter
// choice might be a pointer to a socket factory.  However, the implementation
// here is chosen in order to remove cyclic dependencies between
// 'btlso_streamsocket' and 'btlso_streamsocketfactory' in the test driver.
//..
// void dictionary_client(btlso::StreamSocket<btlso::IPv4Address> *client)
// {
//     enum {
//         k_MAX_DEF_LENGTH = 512,   // maximum length of definition
//         k_SERVER_PORT    = 2698   // arbitrary port number
//     };
//
//     const char         *SERVER_ADDRESS = "127.0.0.1";
//     btlso::IPv4Address  address(SERVER_ADDRESS, k_SERVER_PORT);
//     int                 rc;
//..
// Now, make a connection to the server.
//..
//     rc = client->connect(address);
//     if (rc) {
//         bsl::cout << "Failed to connect to server: " << rc << bsl::endl;
//         return;                                                    // RETURN
//     }
//..
// Next, transmit the length of the word to be defined, followed by the actual
// word to be defined.
//..
//     const char word[] = "socket";
//     char       definition[k_MAX_DEF_LENGTH];
//     int        length;
//
//     length = htonl(sizeof word); // includes the trailing null character
//     rc = client->write((char *)&length, sizeof length);
//
//     if (sizeof length != rc) {
//         bsl::cout << "Error writing request header to server: " << rc
//                   << bsl::endl;
//         client->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);
//         return;                                                    // RETURN
//     }
//     rc = client->write(word, sizeof word);
//     if (rc != length) {
//         bsl::cout << "Error writing request body to server: " << rc
//                   << bsl::endl;
//         client->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);
//         return;                                                    // RETURN
//     }
//..
// Next, read the length of the reply and the definition of the word.
//..
//     rc = client->read((char *)&length, sizeof length);
//     if (rc != sizeof length) {
//         bsl::cout << "Error reading from server: " << rc << bsl::endl;
//         client->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);
//         return;                                                    // RETURN
//     }
//
//     length = ntohl(length);
//     assert(0 < length);
//     assert(sizeof definition >= length);
//     rc = client->read(definition, length);
//     if (rc != length) {
//         bsl::cout << "Error reading from server: " << rc << bsl::endl;
//         client->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);
//         return;                                                    // RETURN
//     }
//     bsl::cout << definition << bsl::endl;
//     client->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);
// }
//..
//
///Example 2: Implementing a Simple Server
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates a simple dictionary server.  The server accepts
// connections on a well known address/port, receives a word, looks up the
// definition of the word, and transmits this definition to the client.  Since
// this server can handle only one connection at a time, it disconnects the
// client after processing the request.  We assume the existence of the
// 'lookupWord' function, which accepts a null-terminated string and returns a
// pointer to a definition, or a null pointer if no definition is found.
//
// Note that the signature of 'dictionary_server' is unconventional because it
// takes a pointer to an allocated, but unbound socket.  A better parameter
// choice might be a pointer to a socket factory.  However, the implementation
// here is chosen in order to remove cyclic dependencies between
// 'btlso_streamsocket' and 'btlso_streamsocketfactory' in the test driver.
//..
// void dictionary_server(btlso::StreamSocket<btlso::IPv4Address> *server)
// {
//     enum {
//         k_MAX_WORD_LENGTH        = 512,  // max length of word to look up
//         k_MAX_LISTEN_QUEUE_DEPTH = 20,   // max number of pending requests
//         k_SERVER_PORT            = 2698  // arbitrary port number
//     };
//
//     btlso::IPv4Address addr(btlso::IPv4Address::k_ANY_ADDRESS,
//                             k_SERVER_PORT);
//     int rc;
//..
// Name the server socket ('bind'), and set it to listening mode ('listen').
//..
//     rc = server->bind(addr);
//     if (rc != 0) {
//         bsl::cout << "Failed to bind socket: " << rc << bsl::endl;
//         return;                                                    // RETURN
//     }
//
//     rc = server->listen(k_MAX_LISTEN_QUEUE_DEPTH);
//     if (rc != 0) {
//         bsl::cout << "Failed to listen on socket: " << rc << bsl::endl;
//         return;                                                    // RETURN
//     }
//..
// Now, go into the processing loop.
//..
//     do {
//         btlso::StreamSocket<btlso::IPv4Address> *client = 0;
//         rc = server->accept(&client);
//         assert(0 == rc);
//         assert(client);
//..
// Next, process the request.  For each request, begin by reading the integer
// containing the size of the word(including the null-terminating character) to
// be defined, followed by the actual word to be defined.
//..
//         char word[k_MAX_WORD_LENGTH];
//         int  length;
//
//         rc = client->read((char*)&length, sizeof length);
//         if (rc != sizeof length) {
//             bsl::cout << "Error reading from client: " << rc << bsl::endl;
//             client->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);
//             continue;
//         }
//
//         length = ntohl(length);
//         if (length < 0 || length > sizeof word) {
//             bsl::cout << "Invalid request length: " << length << bsl::endl;
//             client->shutdown(
//                     btlso::StreamSocket<btlso::IPv4Address>::SHUTDOWN_BOTH);
//             continue;
//         }
//
//         rc = client->read(word, length);
//         if (length != rc) {
//             bsl::cout << "Error reading from client: " << rc << bsl::endl;
//             client->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);
//             continue;
//         }
//..
// Now, lookup the definition of the word.
//..
//         const char *definition = lookupWord(word);
//         if (0 == definition) {
//             definition = "UNKNOWN!";
//         }
//..
// Next, transmit the definition to the client.
//..
//         length = strlen(definition) + 1;
//         int tmp = htonl(length);
//         rc = client->write((char*)&tmp, sizeof tmp);
//         if (sizeof tmp != rc) {
//             bsl::cout << "Error writing to client: " << rc << bsl::endl;
//             client->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);
//             continue;
//         }
//         rc = client->write(definition, length);
//         if (rc != length) {
//             bsl::cerr << "Error writing to client: " << rc << bsl::endl;
//             client->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);
//             continue;
//         }
//..
// Now, terminate the client connection.
//..
//         client->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);
//..
// Finally, loop for the next connection.
//..
//     } while(1);
// }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETOPTUTIL
#include <btlso_socketoptutil.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

#ifndef INCLUDED_BTLSO_FLAG
#include <btlso_flag.h>
#endif

#ifndef INCLUDED_BTLS_IOVEC
#include <btls_iovec.h>
#endif

#ifdef readv
#error Microsoft Windows headers make writing portable code extremely painful.
#endif

namespace BloombergLP {

namespace bsls { class TimeInterval; }

namespace btlso {

                          // ==================
                          // class StreamSocket
                          // ==================

template <class ADDRESS>
class StreamSocket {
    // This class defines a protocol for stream-based socket communications.
    // The class is templatized to provide a family of type-safe address
    // specializations (e.g., "IPv4", "IPv6").  Various socket-related
    // operations including accepting and initiating connections and
    // blocking/non-blocking I/O operations are provided.  Vector I/O
    // operations are also supported.

  public:
    // CREATORS
    virtual ~StreamSocket();
        // Destroy this stream socket object.

    // MANIPULATORS
    virtual int accept(StreamSocket<ADDRESS> **result) = 0;
        // Accept an incoming connection request and load into the specified
        // 'result' the address of the new socket.  Return 0 on success, and a
        // non-zero value otherwise.  If this socket is in blocking mode, this
        // function waits until a connection request is received or an error
        // occurs.  In non-blocking mode, if there is no pending connection
        // request, this call returns immediately with an error status of
        // 'SocketHandle::e_ERROR_WOULDBLOCK'.  The behavior is undefined
        // unless this socket is listening for connections (i.e., unless
        // 'listen' has been called).

    virtual int accept(StreamSocket<ADDRESS> **socket,
                       ADDRESS                *peerAddress) = 0;
        // Accept an incoming connection request; load into the specified
        // 'socket', the address of the new socket and into specified
        // 'peerAddress' the address of the peer.  Return 0 on success, and a
        // non-zero value otherwise.  If this socket is in blocking mode, this
        // function waits until a connection request is received or an error
        // occurs.  In non-blocking mode and there is no pending connection
        // request, this call returns immediately with an error status of
        // 'SocketHandle::e_ERROR_WOULDBLOCK'.  The behavior is undefined
        // unless this socket is listening for connections (i.e., unless
        // 'listen' has been called).

    virtual int bind(const ADDRESS& address) = 0;
        // Associate the specified 'address' with this socket.  Return 0 on
        // success, and a non-zero value otherwise.  Note that, in order to
        // receive connections on a socket, it must have an address associated
        // with it, and when a socket is created, it has no associated address.

    virtual int connect(const ADDRESS& address) = 0;
        // Initiate a connection to a peer process at the specified 'address'.
        // Return 0 on success, and a non-zero value otherwise.  If this socket
        // is in blocking mode, the call waits until a connection is
        // established or an error occurs.  In non-blocking mode and the
        // connection cannot be established immediately
        // 'SocketHandle::e_ERROR_WOULDBLOCK' is returned.  The
        // 'waitForConnect' method may then be used to determine when the
        // connection request has completed.

    virtual int listen(int backlog) = 0;
        // Register this socket for accepting up to the specified 'backlog'
        // simultaneous connection requests.  Return 0 on success, and a
        // non-zero value otherwise.  Note that the behavior is undefined
        // unless '0 < backlog'.

    virtual int read(char *buffer, int length) = 0;
        // Read up to the specified 'length' bytes from this socket into the
        // specified 'buffer'.  If this socket is in non-blocking mode, or is
        // in blocking mode and there is data available, the function reads as
        // many bytes as possible without blocking.  In blocking mode, if no
        // data is available, the call waits until data is available, then
        // reads as many bytes as possible without blocking.  Return the
        // positive total number of bytes read, or a negative value on error.
        // If this socket is in non-blocking mode and zero bytes were
        // immediately available, 'SocketHandle::e_ERROR_WOULDBLOCK' is
        // returned.  If this socket is in blocking mode and the call is
        // interrupted before data is available,
        // 'SocketHandle::e_ERROR_INTERRUPTED' is returned.  If the connection
        // has been closed prior to this call and there is no data available,
        // 'SocketHandle::EOF' is returned.  The behavior is undefined unless
        // '0 < length' and buffer provides capacity for at least 'length'
        // bytes.

    virtual int readv(const btls::Iovec *buffers, int numBuffers) = 0;
        // Read from this socket into the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers' the respective numbers of
        // bytes as defined by the sum of the 'buffers[i].length()' values.  If
        // this socket is in non-blocking mode, or is in blocking mode and
        // there is data available, the function reads as many bytes as
        // possible without blocking.  In blocking mode, if no data is
        // available, the call waits until data is available, then reads as
        // many bytes as possible without blocking.  Return the positive total
        // number of bytes read, or a negative value on error.  If this socket
        // is in non-blocking mode and zero bytes were immediately available,
        // 'SocketHandle::e_ERROR_WOULDBLOCK' is returned.  If this socket is
        // in blocking mode and the call is interrupted before data is
        // available, 'SocketHandle::e_ERROR_INTERRUPTED' is returned.  If the
        // connection has been closed prior to this call and there is no data
        // available, 'SocketHandle::EOF' is returned.  The behavior is
        // undefined unless '0 < numBuffers' and at least one of the
        // 'buffers[i].length()' values is positive.

    virtual int write(const char *buffer, int length) = 0;
        // Write up to 'length' bytes to this socket from the specified
        // 'buffer'; return the non-negative number of bytes written or, a
        // negative value on error.  In blocking mode, if there is no room to
        // write the full message, the call blocks until the message is fully
        // written.  In non-blocking mode, the function writes as many bytes as
        // possible without blocking, and returns the number of bytes written,
        // or 'SocketHandle::e_ERROR_WOULDBLOCK' if no bytes were written.  If
        // the connection has been closed 'SocketHandle::e_ERROR_CONNDEAD' is
        // returned.  If the call is interrupted before any data is written,
        // 'SocketHandle::e_ERROR_INTERRUPTED' is returned.  The behavior is
        // undefined unless '0 < length'.  Note that a successful call to this
        // function does not guarantee that the data has been transmitted
        // successfully, but simply that the data was written successfully to
        // the underlying socket's transmit buffers.

    virtual int writev(const btls::Iovec *buffers, int numBuffers) = 0;
    virtual int writev(const btls::Ovec  *buffers, int numBuffers) = 0;
        // Write to this socket from the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers' the respective number of
        // bytes as defined by the sum of the 'buffers[i].length()' values.
        // Return the non-negative total number of bytes written, or a negative
        // value on error.  In blocking mode, if there is no room to write the
        // full message, the call blocks until the message is fully written.
        // In non-blocking mode, the function writes as many bytes as possible
        // without blocking, and returns the number of bytes written, or
        // 'SocketHandle::e_ERROR_WOULDBLOCK', if no bytes were written.  If
        // the connection has been closed 'SocketHandle::e_ERROR_CONNDEAD' is
        // returned.  If the call is interrupted before any data is written,
        // 'SocketHandle::e_ERROR_INTERRUPTED' is returned.  The behavior is
        // undefined unless '0 < numBuffers' and at least one of the
        // 'buffers[i].length()' values is positive.  Note that a successful
        // call to this function does not guarantee that the data has been
        // transmitted successfully, but simply that the data was successfully
        // written to the underlying socket's transmit buffers.

    virtual int setBlockingMode(btlso::Flag::BlockingMode mode) = 0;
        // Set the current blocking mode of this socket to the specified
        // 'mode'.  Return 0 on success, an a non-zero value otherwise.

    virtual int shutdown(btlso::Flag::ShutdownType streamOption) = 0;
        // Shut down the input and/or output stream(s) specified by
        // 'streamOption' of the full-duplexed connection associated with this
        // socket.  Return 0 on success, and a non-zero value otherwise.
        // Specifying 'SHUTDOWN_RECEIVE' or 'SHUTDOWN_SEND' closes the input or
        // output streams, respectively, leaving the other stream unaffected.
        // Specifying 'SHUTDOWN_BOTH' closes both streams.  Once the input
        // (output) stream has been closed, any attempted read (write)
        // operations will fail.  Any data received after successfully shutting
        // down the input stream will be acknowledged but silently discarded.
        // Note that this function is typically used to transmit an EOF
        // indication to the peer.

    virtual int waitForConnect(const bsls::TimeInterval& timeout) = 0;
        // Wait until a pending outgoing connection request completes or until
        // the specified absolute 'timeout' is reached, whichever occurs first.
        // Return 0 if the connection request has completed, and a non-zero
        // value otherwise.  Note that the completion of a connection request
        // does not imply that a connection has been successfully established;
        // a call to 'connectionStatus' should be made to determine if the
        // connection request completed successfully.

    virtual int waitForAccept(const bsls::TimeInterval& timeout) = 0;
        // Wait for an incoming connection request on this socket or until the
        // specified absolute 'timeout' is reached, whichever occurs first.
        // Return 0 if a connection request has been received, and a non-zero
        // value otherwise.  Note that once a connection request has been
        // received, a call to 'accept' can be made to establish the
        // connection.

    virtual int waitForIO(btlso::Flag::IOWaitType   type,
                          const bsls::TimeInterval& timeout) = 0;
        // Wait for the occurrence of an I/O event matching the specified
        // 'type', or until the specified absolute 'timeout' is reached,
        // whichever occurs first.  Return a value indicating the type(s) of
        // the event(s) occurred before 'timeout', if any, and a negative value
        // otherwise.  If 'timeout' is reached without an I/O event matching
        // 'type', a value of 'SocketHandle::e_ERROR_TIMEDOUT' is returned.  If
        // this call is interrupted, 'SocketHandle::e_ERROR_INTERRUPTED' is
        // returned.  Note that the return value is 'type' if 'type' is
        // different from 'IO_RW', and one of 'IO_READ', 'IO_WRITE', or 'IO_RW'
        // if 'type' is 'IO_RW.  Also note that if 'timeout' is in the past,
        // this function will return a value indicating the type(s) of the
        // event(s) currently available if polling the socket returns a
        // matching event, and 'SocketHandle::e_ERROR_TIMEDOUT' otherwise.

    virtual int waitForIO(btlso::Flag::IOWaitType type) = 0;
        // Wait for the occurrence of an I/O event matching the specified
        // 'type'.  Return a value indicating the type(s) of the event(s)
        // occurred, and a negative value on error.  If this call is
        // interrupted, 'SocketHandle::e_ERROR_INTERRUPTED' is returned.  Note
        // that the return value is 'type' if 'type' is different from 'IO_RW',
        // and one of 'IO_READ', 'IO_WRITE', or 'IO_RW' if 'type' is 'IO_RW.

    virtual int setLingerOption(const SocketOptUtil::LingerData& options) = 0;
        // Set the current linger options of this socket to the specified
        // 'options'.  Return 0 on success, and a non-zero value otherwise.

    virtual int setOption(int level, int option, int value) = 0;
        // Set the specified 'option' (of the specified 'level') socket option
        // on this socket to the specified 'value'.  Return 0 on success and a
        // non-zero value otherwise.  See 'btlso_socketoptutil' for the list of
        // commonly supported options.

    // ACCESSORS
    virtual int blockingMode(btlso::Flag::BlockingMode *result) const = 0;
        // Load into the specified 'result' the current blocking mode of this
        // socket.  Return 0 on success, and a non-zero value without affecting
        // 'result' otherwise.

    virtual int connectionStatus() const = 0;
        // Test the connection status of this socket.  Return 0 if this socket
        // has an established connection, and a non-zero value otherwise.  A
        // value of 'SocketHandle::e_ERROR_CONNDEAD' is returned if a
        // connection request has failed.  Note that this method is typically
        // used to determine the result of a non-blocking connection request.
        // The behavior is undefined unless a connection request with 'connect'
        // has completed (independent of success or failure).

    virtual SocketHandle::Handle handle() const = 0;
        // Return the OS handle associated with this socket.  Note that direct
        // manipulation of the underlying stream may result in undefined
        // behavior.

    virtual int localAddress(ADDRESS *result) const = 0;
        // Load into the specified 'result' the local address of this socket.
        // Return 0 on success, and a non-zero value with no effect on 'result'
        // otherwise.

    virtual int peerAddress(ADDRESS *result) const = 0;
        // Load into the specified 'result' the address of the peer connected
        // to this socket.  Return 0 on success, and a non-zero value with no
        // effect on 'result' otherwise.

    virtual int lingerOption(SocketOptUtil::LingerData *result) const = 0;
        // Load into the specified 'result' the value of this socket's current
        // linger option.  Return 0 on success, and a non-zero value with no
        // effect on 'result' otherwise.

    virtual int socketOption(int *result, int level, int option) const = 0;
        // Load int the specified 'result' the value of the specified 'option'
        // of the specified 'level' socket option on this socket.  Return 0 on
        // success and a non-zero value with no effect on 'result' otherwise.
};

                    // ===========================
                    // class StreamSocketAutoClose
                    // ===========================

template <class ADDRESS>
class StreamSocketAutoClose{
    // This class implements a proctor for a socket, using the specified
    // factory to deallocate it upon destruction unless this socket has been
    // released from management by this proctor prior to destruction.

    // DATA
    StreamSocket<ADDRESS>     *d_socket_p;
    btlso::Flag::ShutdownType  d_option;

  public:
    // CREATORS
    StreamSocketAutoClose(StreamSocket<ADDRESS>     *socket,
                          btlso::Flag::ShutdownType  streamOption);
        // Create a proctor for the specified 'socket', recording the specified
        // 'streamOption' for shutting down the 'socket' upon destruction of
        // this proctor.

    ~StreamSocketAutoClose();
        // Shutdown the proctored socket with the stream option specified at
        // construction, unless released from management by this proctor.

    // MANIPULATORS
    void release();
        // Release the proctored socket from management by this proctor.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // ------------------
                          // class StreamSocket
                          // ------------------

// CREATORS
template <class ADDRESS>
inline
StreamSocket<ADDRESS>::~StreamSocket()
{
}

                     // ---------------------------
                     // class StreamSocketAutoClose
                     // ---------------------------

// CREATORS
template <class ADDRESS>
inline
StreamSocketAutoClose<ADDRESS>::StreamSocketAutoClose(
                                       StreamSocket<ADDRESS>     *socket,
                                       btlso::Flag::ShutdownType  streamOption)
: d_socket_p(socket)
, d_option(streamOption)
{
}

template <class ADDRESS>
inline
StreamSocketAutoClose<ADDRESS>::~StreamSocketAutoClose()
{
    if (d_socket_p) {
        d_socket_p->shutdown(d_option);
    }
}

// MANIPULATORS
template <class ADDRESS>
inline
void StreamSocketAutoClose<ADDRESS>::release()
{
    d_socket_p = 0;
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
