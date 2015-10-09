// btlsos_tcpchannel.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSOS_TCPCHANNEL
#define INCLUDED_BTLSOS_TCPCHANNEL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide synchronous TCP-based communication channel.
//
//@SEE_ALSO: btlsos_tcpacceptor       btlsos_tcpconnector
//@SEE_ALSO: btlsos_tcptimedchannel   btlsos_tcpcbchannel
//@SEE_ALSO: btlso_socketoptutil
//
//@DESCRIPTION: This component provides concrete implementation of the blocking
// communication channel ('btlsc_channel') over TCP/IPv4 sockets.  Both
// non-timed operations are supported (as mandated by the protocol).
// Additionally, operations to set various socket options and to get local and
// remote addresses are provided.
//
///Thread Safety
///-------------
// The channel is *thread safe*, meaning that any operation can be called on
// *distinct instances* from different threads without any side-effects (which,
// generally speaking, means that there is no 'static'), but not *thread
// enabled* (i.e., two threads cannot safely call methods on the *same
// instance* without external synchronization).  The channel is not
// *async-safe*, meaning that one or more functions cannot be invoked safely
// from a signal handler.
//
///Performance
///-----------
// This channel is optimized for operations with the timeout.  Non-timed
// operations will have worse performance than their respective counterparts in
// the non-timed version of the channel (i.e., 'btlsos_tcpchannel').  If only
// non-timed operations are required, 'btlsos::TcpChannel' should be used
// instead.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using a Pair of Sockets
/// - - - - - - - - - - - - - - - - -
// The following usage example shows a possible use of this component.  First,
// a pair of sockets connecting each other on the local host are created for
// our example, which could be any connected sockets on different hosts.  The
// channel only needs one of the socket as its I/O request endpoint, while the
// other end of connection will be used to write some data for the channel to
// read:
//..
//    btlso::SocketHandle::Handle handles[2];
//    int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
//                                  handles,
//                                  btlso::SocketImpUtil::k_SOCKET_STREAM);
//    assert(0 == ret);
//    // The following socket options are set only if necessary.
//
//    ret = btlso::SocketOptUtil::setOption(handles[0],
//                      btlso::SocketOptUtil::k_SOCKETLEVEL,
//                      btlso::SocketOptUtil::k_SENDBUFFER, 8192);
//    assert(0 == ret);
//
//    ret = btlso::SocketOptUtil::setOption(handles[1],
//                      btlso::SocketOptUtil::k_SOCKETLEVEL,
//                      btlso::SocketOptUtil::k_RECEIVEBUFFER, 8192);
//    assert(0 == ret);
//
//    ret = btlso::SocketOptUtil::setOption(handles[1],
//                          btlso::SocketOptUtil::k_TCPLEVEL,
//                          btlso::SocketOptUtil::k_TCPNODELAY, 1);
//    assert(0 == ret);
//..
// Next, create a 'btlso::StreamSocket' object, which is a part of the channel.
// The 'btlso::StreamSocket' object has a field of type
// 'btlso::SocketHandle::Handle', whose value is set to a socket created above.
// Then I/O operations can be invoked on the channel:
//..
//    btlso::InetStreamSocketFactory<btlso::IPv4Address>
//                                                     factory(&testAllocator);
//    btlso::StreamSocket<btlso::IPv4Address> *sSocket =
//                                            factory.allocate(handles[0]);
//    assert(sSocket);
//
//    {
//        // We should guarantee that the 'channel's destructor is
//        // invoked before the corresponding 'streamSocket'
//        // destructor, the behavior is undefined otherwise.
//        // We insure the required order by creating the 'channel'
//        // inside a block while the corresponding 'streamSocket'
//        // object outside the block as above.
//
//        Obj channel(sSocket);
//        assert(0 == channel.isInvalid());
//
//        // Write data at the other side of the channel and so "read"
//        // operations can be done at the channel side.
//        enum { k_LEN = 30 };
//        char writeBuf[k_LEN] = "abcdefghij1234567890",
//             readBuf[k_LEN];
//        int numBytes = 0, augStatus = -1, interruptFlag = 1;
//        int len = btlso::SocketImpUtil::write(handles[1], writeBuf,
//                                             strlen(writeBuf));
//
//        assert(len == strlen(writeBuf));
//        // Read 5 bytes from the channel.
//        numBytes = 5;
//        augStatus = -1;
//        len = channel.read(&augStatus, readBuf, numBytes, interruptFlag);
//        if (len != numBytes) {
//            assert(0 < augStatus);
//        }
//
//        numBytes = 10;
//        augStatus = -1;
//        len = channel.readRaw(&augStatus, readBuf,
//                              numBytes, interruptFlag);
//        if (len != numBytes) {
//            assert(0 < augStatus);
//        }
//
//        // Write 1 byte to the channel.
//        numBytes = 1;
//        augStatus = -1;
//        len = channel.write(&augStatus, writeBuf,
//                                    numBytes, interruptFlag);
//        if (len != numBytes) {
//            assert(0 < augStatus);
//        }
//
//        numBytes = 10;
//        augStatus = -1;
//        // Try writing 10 bytes to the channel with a timeout value.
//        len = channel.writeRaw(&augStatus, writeBuf,
//                               numBytes, interruptFlag);
//        if (len != numBytes) {
//            assert(0 < augStatus);
//        }
//        assert(0 == channel.isInvalid());
//        channel.invalidate();
//        assert(1 == channel.isInvalid());
//
//        numBytes = 5;
//        enum { e_INVALID = -2 };
//        // Try writing 5 bytes from the channel.
//        len = channel.read(&augStatus, readBuf,
//                                   numBytes, interruptFlag);
//        assert(e_INVALID == len);
//
//        numBytes = 10;
//        augStatus = -1;
//        // Try writing 10 bytes from the channel with a timeout value.
//        len = channel.read(&augStatus, readBuf,
//                           numBytes, interruptFlag);
//        assert(e_INVALID == len);
//        // Try writing 1 byte to the channel.
//        numBytes = 1;
//        augStatus = -1;
//        len = channel.write(&augStatus, writeBuf,
//                            numBytes, interruptFlag);
//        assert(e_INVALID == len);
//
//        numBytes = 10;
//        augStatus = -1;
//        // Try writing 10 bytes to the channel with a timeout value.
//        len = channel.writeRaw(&augStatus, writeBuf,
//                               numBytes, interruptFlag);
//        assert(e_INVALID == len);
//    }
//    factory.deallocate(sSocket);
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLS_IOVEC
#include <btls_iovec.h>
#endif

#ifndef INCLUDED_BTLSO_IPV4ADDRESS
#include <btlso_ipv4address.h>
#endif

#ifndef INCLUDED_BTLSC_CHANNEL
#include <btlsc_channel.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace btlso { template<class ADDRESS> class StreamSocket; }
namespace btlsos {

                             // ================
                             // class TcpChannel
                             // ================

class TcpChannel : public btlsc::Channel {
    // This class implements 'btlsc::Channel' protocol over TCP/IP sockets.  It
    // operates on top of the stream-socket interface, which is provided at
    // construction.

    btlso::StreamSocket<btlso::IPv4Address> *d_socket_p;     // not owned
    int                                    d_isInvalidFlag;

    bsl::vector<char>                      d_readBuffer;
    int                                    d_readBufferOffset;
    int                                    d_readBufferedStartPointer;
        // the index of the first unconsumed data in 'd_readBuffer'
    bslma::Allocator                      *d_allocator_p;

  private:  // not implemented
    TcpChannel(const TcpChannel&);
    TcpChannel& operator=(const TcpChannel&);

    // PRIVATE MANIPULATORS
    void initializeReadBuffer(int size = -1);
        // Initialize internal read buffer with the optionally specified
        // 'size'.  If 'size' is not specified, the default that is obtained by
        // querying the underlying socket is used.

  public:
    // CREATORS
    TcpChannel(btlso::StreamSocket<btlso::IPv4Address> *socket,
               bslma::Allocator                        *basicAllocator = 0);
        // Create a channel attached to the specified stream-oriented 'socket'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 'socket' is not 0.

    ~TcpChannel();
        // Destroy this channel and release the underlying socket.

    // MANIPULATORS
    int read(char *buffer, int numBytes, int flags = 0);
    int read(int *augStatus, char *buffer, int numBytes, int flags = 0);
        // Read from this channel into the specified 'buffer' the specified
        // 'numBytes'.  If the optionally specified 'flags' incorporates
        // 'btlsc::Channel::ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes newly
        // read into 'buffer' (indicating a partial result) otherwise.  On a
        // partial result, load 'augStatus', if supplied, with a positive
        // value, indicating that an asynchronous event caused the
        // interruption; otherwise, 'augStatus' is unmodified.  A partial
        // result typically does not invalidate this channel; hence, this (or
        // another) operation may be retried (with arguments suitably adjusted)
        // with some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error (leaving the contents of 'buffer'
        // undefined); -1 implies that the connection was closed by the peer
        // (but the converse is not guaranteed).  The behavior is undefined
        // unless 'buffer' has sufficient capacity to hold the requested data
        // and '0 < numBytes'.

    int readv(const btls::Iovec *buffers, int numBuffers, int flags = 0);
    int readv(int               *augStatus,
              const btls::Iovec *buffers,
              int                numBuffers,
              int                flags = 0);
        // Read from this channel into the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers', the respective numbers of
        // bytes as specified in each corresponding 'btls::Iovec' buffer.  If
        // the optionally specified 'flags' incorporates
        // 'btlsc::Channel::ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a partial result.  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes newly
        // read into 'buffers' (indicating a partial result) otherwise.  On a
        // partial result, load 'augStatus', if supplied, with a positive
        // value, indicating that an asynchronous event caused the
        // interruption; otherwise, 'augStatus' is unmodified.  A partial
        // result typically does not invalidate this channel; hence, this (or
        // another) operation may be retried (with arguments suitably adjusted)
        // with some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error (leaving the contents of 'buffers'
        // undefined); -1 implies that the connection was closed by the peer
        // (but the converse is not guaranteed).  The behavior is undefined
        // unless 'buffers' have sufficient capacity to hold the requested data
        // and 0 < 'numBuffers'.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    int readRaw(char *buffer, int numBytes, int flags = 0);
    int readRaw(int *augStatus, char *buffer, int numBytes, int flags = 0);
        // *Atomically* read from this channel into the specified 'buffer' *at*
        // *most* the specified 'numBytes'.  If the optionally specified
        // 'flags' incorporates 'btlsc::Channel::ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Optionally specify (as a
        // *leading* argument) 'augStatus' to receive status specific to a
        // "partial result".  Return 'numBytes' on success, a negative value on
        // error, and the number of bytes newly read into 'buffer' (indicating
        // a partial result) otherwise.  On a partial result, load 'augStatus',
        // if supplied, with a positive value if an asynchronous event
        // interrupted this operation and a negative value if the atomic
        // OS-level operation transmitted at least one byte, but less than
        // 'numBytes'; otherwise, 'augStatus' is unmodified.  A partial result
        // typically does not invalidate this channel; hence, this (or another)
        // operation may be retried (with arguments suitably adjusted) with
        // some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error (leaving the contents of 'buffer'
        // undefined); -1 implies that the connection was closed by the peer
        // (but the converse is not guaranteed).  The behavior is undefined
        // unless 'buffer' has sufficient capacity to hold the requested data
        // and '0 < numBytes'.  Note that if the specified 'timeout' value has
        // already passed, the "read" operation will still be attempted, but
        // the attempt will not block.

    int readvRaw(const btls::Iovec *buffers, int numBuffers, int flags = 0);
    int readvRaw(int               *augStatus,
                 const btls::Iovec *buffers,
                 int                numBuffers,
                 int                flags = 0);
        // *Atomically* read from this channel into the specified sequence of
        // 'buffers' of specified sequence length 'numBuffers' *at* *most* the
        // respective numbers of bytes as specified in each corresponding
        // 'btls::Iovec' buffer.  If the optionally specified 'flags'
        // incorporates 'btlsc::Channel::ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Optionally specify (as a *leading* argument)
        // 'augStatus' to receive status specific to a "partial result".
        // Return 'numBytes' on success, a negative value on error, and the
        // number of bytes newly read into 'buffers' (indicating a partial
        // result) otherwise.  On a partial result, load 'augStatus', if
        // supplied, with a positive value if an asynchronous event interrupted
        // this operation and a negative value if the atomic OS-level operation
        // transmitted at least one byte, but less than 'numBytes'; otherwise,
        // 'augStatus' is unmodified.  A partial result typically does not
        // invalidate this channel; hence, this (or another) operation may be
        // retried (with arguments suitably adjusted) with some reasonable hope
        // of success.  A negative "status", however, indicates a permanent
        // error (leaving the contents of 'buffers' undefined); -1 implies that
        // the connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless 'buffers' have
        // sufficient capacity to hold the requested data and 0 < numBytes.
        // Note that if the specified 'timeout' value has already passed, the
        // "read" operation will still be attempted, but the attempt will not
        // block.

    int bufferedRead(const char **buffer, int numBytes, int flags = 0 );
    int bufferedRead(int         *augStatus,
                     const char **buffer,
                     int          numBytes,
                     int          flags = 0 );
        // Read from this channel into a channel-supplied buffer, identified
        // via the specified 'buffer', the specified 'numBytes'.  If the
        // optionally specified 'flags' incorporates
        // 'btlsc::Channel::ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes newly
        // read into 'buffer' (indicating a partial result) otherwise.  Any
        // positive return value guarantees that 'buffer' will remain valid
        // until this channel is modified.  On a partial result, load
        // 'augStatus', if supplied, with a positive value, indicating that an
        // asynchronous event caused the interruption; otherwise, 'augStatus'
        // is unmodified.  A partial result typically does not invalidate this
        // channel; hence, this (or another) operation may be retried with some
        // reasonable hope of success -- buffered data from a partial result
        // remains available until consumed by subsequent read operations.  A
        // negative "status", however, indicates a permanent error (leaving
        // 'buffer' undefined); -1 implies that the connection was closed by
        // the peer (but the converse is not guaranteed).  The behavior is
        // undefined unless '0 < numBytes'.

    int bufferedReadRaw(const char **buffer, int numBytes, int flags = 0);
    int bufferedReadRaw(int         *augStatus,
                        const char **buffer,
                        int          numBytes,
                        int          flags = 0);
        // *Atomically* read from this channel into a channel-supplied buffer,
        // identified via the specified 'buffer', *at* *most* the specified
        // 'numBytes'.  If the optionally specified 'flags' incorporates
        // 'btlsc::Channel::ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes newly
        // read into 'buffer' (indicating a partial result) otherwise.  Any
        // positive return value guarantees that 'buffer' will remain valid
        // until this channel is modified.  On a partial result, load
        // 'augStatus', if supplied, with a positive value if an asynchronous
        // event interrupted this operation, or a negative value if the atomic
        // OS-level operation transmitted at least one but less than
        // 'numBytes'; otherwise, 'augStatus' is unmodified.  A partial result
        // typically does not invalidate this channel; hence, this (or another)
        // operation may be retried with some reasonable hope of success --
        // buffered data from a partial result remains available until consumed
        // by subsequent read operations.  A negative "status", however,
        // indicates a permanent error (leaving 'buffer' unset); -1 implies
        // that the connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless '0 < numBytes'.

    // = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

    int write(const char *buffer, int numBytes, int flags = 0);
    int write(int *augStatus, const char *buffer, int numBytes, int flags = 0);
        // Write to this channel from the specified 'buffer' the specified
        // 'numBytes'.  If the optionally specified 'flags' incorporates
        // 'btlsc::Channel::ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes newly
        // written from 'buffer' (indicating a partial result) otherwise.  On a
        // partial result, load 'augStatus', if supplied, with a positive
        // value, indicating that an asynchronous event caused the
        // interruption; otherwise, 'augStatus' is unmodified.  A partial
        // result typically does not invalidate this channel; hence, this (or
        // another) operation may be retried (with arguments suitably adjusted)
        // with some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error; -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless '0 < numBytes'.

    int writeRaw(const char *buffer, int numBytes, int flags = 0);
    int writeRaw(int        *augStatus,
                 const char *buffer,
                 int         numBytes,
                 int         flags = 0);
        // *Atomically* write to this channel from the specified 'buffer' *at*
        // *most* the specified 'numBytes'.  If the optionally specified
        // 'flags' incorporates 'btlsc::Channel::ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Optionally specify (as a
        // *leading* argument) 'augStatus' to receive status specific to a
        // "partial result".  Return 'numBytes' on success, a negative value on
        // error, and the number of bytes newly written from 'buffer'
        // (indicating a partial result) otherwise.  On a partial result, load
        // 'augStatus', if supplied, with a positive value if an asynchronous
        // event interrupted this operation and a negative value if the atomic
        // OS-level operation transmitted at least one byte, but less than
        // 'numBytes'; otherwise, 'augStatus' is unmodified.  A partial result
        // typically does not invalidate this channel; hence, this (or another)
        // operation may be retried (with arguments suitably adjusted) with
        // some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error; -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless '0 < numBytes'.  Note that if the
        // specified 'timeout' value has already passed, the "write" operation
        // will still be attempted, but the attempt will not block.

    int writev(const btls::Ovec *buffers, int numBuffers, int flags = 0);
    int writev(const btls::Iovec *buffers, int numBuffers, int flags = 0);
    int writev(int              *augStatus,
               const btls::Ovec *buffers,
               int               numBuffers,
               int               flags = 0);
    int writev(int               *augStatus,
               const btls::Iovec *buffers,
               int                numBuffers,
               int                flags = 0);
        // Write to this channel from the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers' the respective numbers of
        // bytes as specified in each corresponding 'btls::Ovec' (or
        // 'btls::Iovec') buffer.  If the optionally specified 'flags'
        // incorporates 'btlsc::Channel::ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Optionally specify (as a *leading* argument)
        // 'augStatus' to receive status specific to a partial result.  Return
        // 'numBytes' on success, a negative value on error, and the number of
        // bytes newly written from 'buffers' (indicating a partial result)
        // otherwise.  On a partial result, load 'augStatus', if supplied, with
        // a positive value, indicating that an asynchronous event caused the
        // interruption; otherwise, 'augStatus' is unmodified.  A partial
        // result typically does not invalidate this channel; hence, this (or
        // another) operation may be retried (with arguments suitably adjusted)
        // with some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error; -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 0 < numBytes.

    int writevRaw(const btls::Ovec *buffers, int numBuffers, int flags = 0);
    int writevRaw(const btls::Iovec *buffers, int numBuffers, int flags = 0);
    int writevRaw(int              *augStatus,
                  const btls::Ovec *buffers,
                  int               numBuffers,
                  int               flags = 0);
    int writevRaw(int               *augStatus,
                  const btls::Iovec *buffers,
                  int                numBuffers,
                  int                flags = 0);
        // *Atomically* write to this channel, from the specified sequence of
        // 'buffers' of specified sequence length 'numBuffers', *at* *most* the
        // respective numbers of bytes as specified in each corresponding
        // 'btls::Ovec' (or 'btls::Iovec') buffer.  If the optionally specified
        // 'flags' incorporates 'btlsc::Channel::ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Optionally specify (as a
        // *leading* argument) 'augStatus' to receive status specific to a
        // "partial result".  Return 'numBytes' on success, a negative value on
        // error, and the number of bytes newly written from 'buffer'
        // (indicating a partial result) otherwise.  On a partial result, load
        // 'augStatus', if supplied, with a positive value if an asynchronous
        // event interrupted this operation and a negative value if the atomic
        // OS-level operation transmitted at least one byte, but less than
        // 'numBytes'; otherwise, 'augStatus' is unmodified.  A partial result
        // typically does not invalidate this channel; hence, this (or another)
        // operation may be retried (with arguments suitably adjusted) with
        // some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error; -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 0 < numBytes.

    void invalidate();
        // Make this channel invalid; no subsequent operations can be completed
        // successfully.

    int getLocalAddress(btlso::IPv4Address *result);
        // Load into the specified 'result' the complete IP address associated
        // with the local (i.e., this process) end-point of this channel.
        // Return 0 on success and a non-zero value otherwise.

    int getOption(int *result, int level, int option);
        // Load into the specified 'result' the current value of the specified
        // 'option' of the specified 'level' set on the underlying socket.
        // Return 0 on success and a non-zero value otherwise.  The list of
        // commonly-supported options (and levels) is enumerated in
        // 'btlso_socketoptutil'.

    int getPeerAddress(btlso::IPv4Address *result);
        // Load into the specified 'result' the complete IP address associated
        // with the remote (i.e., peer process) end-point of this channel.
        // Return 0 on success and a non-zero value otherwise.

    int setOption(int level, int option, int value);
        // Set the specified socket 'option' of the specified 'level' on the
        // underlying socket to the specified 'value'.  Return 0 on success and
        // a non-zero value otherwise.  (The list of commonly-supported options
        // is available in 'btlso_socketoptutil'.)

    // ACCESSORS
    int isInvalid() const;
        // Return 1 if *any* transmission error has occurred or if the channel
        // has been explicitly invalidated (via 'invalidate') and 0 otherwise.
        // Once a channel is invalid, no operations can be completed
        // successfully.  Note also that 0 return value does NOT guarantee that
        // a subsequent I/O operation would not fail.

    btlso::StreamSocket<btlso::IPv4Address> *socket() const;
        // Return the address of the stream-socket used by this channel.

};

// ----------------------------------------------------------------------------
//                             INLINE DEFINITIONS
// ----------------------------------------------------------------------------

inline
void TcpChannel::invalidate()
{
    d_isInvalidFlag = 1;
}

inline
btlso::StreamSocket<btlso::IPv4Address> *TcpChannel::socket() const
{
    return d_socket_p;
}

inline
int TcpChannel::isInvalid() const
{
    return d_isInvalidFlag;
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
