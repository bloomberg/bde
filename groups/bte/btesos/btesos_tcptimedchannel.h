// btesos_tcptimedchannel.h     -*-C++-*-
#ifndef INCLUDED_BTESOS_TCPTIMEDCHANNEL
#define INCLUDED_BTESOS_TCPTIMEDCHANNEL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide synchronous TCP-based communication channel with timeouts.
//
//@AUTHOR: Andrei Basov (abasov)
//
//@SEE_ALSO: btesos_tcptimedacceptor  btesos_tcptimedconnector
//@SEE_ALSO: btesos_tcpchannel        btesos_tcptimedcbchannel
//@SEE_ALSO: bteso_socketoptutil
//
//@DESCRIPTION: This component provides concrete implementation of the blocking
// communication channel with timeouts ('btesc_timedchannel') over TCP/IPv4
// sockets.  Both timed and non-timed operations are supported (as mandated by
// the protocol).  Additionally, operations to set various socket options and
// to get local and remote addresses are provided.
//
///Thread-safety
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
// the non-timed version of the channel (i.e., 'btesos_tcpchannel').  If only
// non-timed operations are required, 'btesos_TcpChannel' should be used
// instead.
//
// The following usage example shows a possible use of this component.  First,
// a pair of sockets connecting each other on the local host are created for
// our example, which could be any connected sockets on different hosts.  The
// channel only needs one of the socket as its I/O request endpoint, while the
// other end of connection will be used to write some data for the channel to
// read:
///USAGE EXAMPLE
///=============
//..
//    bteso_SocketHandle::Handle handles[2];
//    int ret = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
//                                   handles,
//                                   bteso_SocketImpUtil::BTESO_SOCKET_STREAM);
//    assert(0 == ret);
//    // The following socket options are set only if necessary.
//
//    ret = bteso_SocketOptUtil::setOption(handles[0],
//                      bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
//                      bteso_SocketOptUtil::BTESO_SENDBUFFER, 8192);
//    assert(0 == ret);
//
//    ret = bteso_SocketOptUtil::setOption(handles[1],
//                      bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
//                      bteso_SocketOptUtil::BTESO_RECEIVEBUFFER, 8192);
//    assert(0 == ret);
//
//    ret = bteso_SocketOptUtil::setOption(handles[1],
//                          bteso_SocketOptUtil::BTESO_TCPLEVEL,
//                          bteso_SocketOptUtil::BTESO_TCPNODELAY, 1);
//    assert(0 == ret);
//..
// Next, create a 'bteso_StreamSocket' object, which is a part of the channel.
// The 'bteso_StreamSocket' object has a field of type
// 'bteso_SocketHandle::Handle', whose value is set to a socket created above.
// Then I/O operations can be invoked on the channel:
//..
//    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&testAllocator);
//    bteso_StreamSocket<bteso_IPv4Address> *sSocket =
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
//        enum { LEN = 30 };
//        char writeBuf[LEN] = "abcdefghij1234567890",
//             readBuf[LEN];
//        int numBytes = 0, augStatus = -1, interruptFlag = 1;
//        int len = bteso_SocketImpUtil::write(handles[1], writeBuf,
//                                                     strlen(writeBuf));
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
//        // We need to set a timeout value which is relative to the
//        // current system time.
//        bdet_TimeInterval timer = bdetu_SystemTime::now();
//        int milliSec = 100, nanoSec = 400;
//        timer.addMilliseconds(milliSec);
//        timer.addNanoseconds(nanoSec);
//        numBytes = 10;
//        augStatus = -1;
//        len = channel.timedRead(&augStatus, readBuf,
//                                        numBytes, timer, interruptFlag);
//        if (len != numBytes) {
//            assert(0 <= augStatus);
//        }
//
//        timer.addMilliseconds(milliSec);
//        timer.addNanoseconds(nanoSec);
//        numBytes = 20;
//        augStatus = -1;
//        // Try reading 20 bytes from the channel with a timeout value.
//        // The timeout will be reached since no enough data in the
//        // channel.
//        len = channel.timedRead(&augStatus, (char*) readBuf,
//                                        numBytes, timer, interruptFlag);
//        if (len != numBytes) {
//            assert(0 == augStatus);
//        }
//        // Write 1 byte to the channel.
//        numBytes = 1;
//        augStatus = -1;
//        len = channel.write(&augStatus, writeBuf,
//                                    numBytes, interruptFlag);
//        if (len != numBytes) {
//            assert(0 < augStatus);
//        }
//
//        timer.addMilliseconds(milliSec);
//        timer.addNanoseconds(nanoSec);
//        numBytes = 10;
//        augStatus = -1;
//        // Try writing 10 bytes to the channel with a timeout value.
//        len = channel.timedWrite(&augStatus, writeBuf,
//                                         numBytes, timer, interruptFlag);
//        if (len != numBytes) {
//            assert(0 <= augStatus);
//        }
//        assert(0 == channel.isInvalid());
//        channel.invalidate();
//        assert(1 == channel.isInvalid());
//
//        numBytes = 5;
//        enum { INVALID = -2 };
//        // Try writing 5 bytes from the channel.
//        len = channel.read(&augStatus, readBuf,
//                                   numBytes, interruptFlag);
//        assert(INVALID == len);
//
//        timer.addMilliseconds(milliSec);
//        timer.addNanoseconds(nanoSec);
//        numBytes = 10;
//        augStatus = -1;
//        // Try writing 10 bytes from the channel with a timeout value.
//        len = channel.timedRead(&augStatus, readBuf,
//                                        numBytes, timer, interruptFlag);
//        assert(INVALID == len);
//        // Try writing 1 byte to the channel.
//        numBytes = 1;
//        augStatus = -1;
//        len = channel.write(&augStatus, writeBuf,
//                                    numBytes, interruptFlag);
//        assert(INVALID == len);
//
//        timer.addMilliseconds(milliSec);
//        timer.addNanoseconds(nanoSec);
//        numBytes = 10;
//        augStatus = -1;
//        // Try writing 10 bytes to the channel with a timeout value.
//        len = channel.timedWrite(&augStatus, writeBuf,
//                                          numBytes, timer, interruptFlag);
//        assert(INVALID == len);
//    }
//    factory.deallocate(sSocket);
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESC_TIMEDCHANNEL
#include <btesc_timedchannel.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

template<class ADDRESS> class bteso_StreamSocket;

class bdet_TimeInterval;

                        // ============================
                        // class btesos_TcpTimedChannel
                        // ============================

class btesos_TcpTimedChannel : public btesc_TimedChannel {
    // This class implements 'btesc_TimedChannel' protocol over TCP/IP sockets.
    // It operates on top of the stream-socket interface, which is provided at
    // construction.

    bteso_StreamSocket<bteso_IPv4Address> *d_socket_p;       // not owned
    int                                    d_isInvalidFlag;

    bsl::vector<char>                      d_readBuffer;
    int                                    d_readBufferOffset;
    int                                    d_readBufferedStartPointer;
        // the index of the first unconsumed data in 'd_readBuffer'
    bslma_Allocator                       *d_allocator_p;

  private: // not implemented
    btesos_TcpTimedChannel(const btesos_TcpTimedChannel&);
    btesos_TcpTimedChannel& operator=(const btesos_TcpTimedChannel&);

    // PRIVATE MANIPULATORS
    void initializeReadBuffer(int size = -1);
        // Initialize internal read buffer with the optionally-specified
        // 'size'.  If 'size' is not specified, the default that is obtained by
        // querying the underlying socket is used.

  public:
    // CREATORS
    btesos_TcpTimedChannel(
                   bteso_StreamSocket<bteso_IPv4Address> *socket,
                   bslma_Allocator                       *basicAllocator = 0);
        // Create a timed channel attached to the specified stream-oriented
        // 'socket'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless 'socket' is not
        // 0.

    ~btesos_TcpTimedChannel();
        // Destroy this channel and release the underlying socket.

    // MANIPULATORS
    int read(char *buffer, int numBytes, int flags = 0);
    int read(int  *augStatus, char *buffer, int numBytes, int flags = 0);
        // Read from this channel into the specified 'buffer' the specified
        // 'numBytes'.  If the optionally specified 'flags' incorporates
        // 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous events" are
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
        // and 0 < numBytes.

    int timedRead(char                     *buffer,
                  int                       numBytes,
                  const bdet_TimeInterval&  timeout,
                  int                       flags = 0);
    int timedRead(int                      *augStatus,
                  char                     *buffer,
                  int                       numBytes,
                  const bdet_TimeInterval&  timeout,
                  int                       flags = 0);
        // Read from this channel into the specified 'buffer' the specified
        // 'numBytes' or interrupt after the specified absolute 'timeout' time
        // is reached.  If the optionally specified 'flags' incorporates
        // 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes newly
        // read into 'buffer' (indicating a partial result) otherwise.  On a
        // partial result, load 'augStatus', if supplied, with 0 if 'timeout'
        // interrupted this operation, or a positive value if the interruption
        // was due to an asynchronous event; otherwise, 'augStatus' is
        // unmodified.  A partial result typically does not invalidate this
        // channel; hence, this (or another) operation may be retried (with
        // arguments suitably adjusted) with some reasonable hope of success.
        // A negative "status", however, indicates a permanent error (leaving
        // the contents of 'buffer' undefined); -1 implies that the connection
        // was closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 'buffer' has sufficient capacity to
        // hold the requested data and 0 < numBytes.  Note that if the
        // specified 'timeout' value has already passed, the "read" operation
        // will still be attempted, but the attempt will not block.

    int readv(const btes_Iovec *buffers,
               int              numBuffers,
               int              flags = 0);
    int readv(int               *augStatus,
              const btes_Iovec *buffers,
              int               numBuffers,
              int               flags = 0);
        // Read from this channel into the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers' the respective numbers of
        // bytes as specified in each corresponding 'btes_Iovec' buffer.  If
        // the optionally specified 'flags' incorporates
        // 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous events" are
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
        // and 0 < numBytes.

     int timedReadv(const btes_Iovec         *buffers,
                    int                       numBuffers,
                    const bdet_TimeInterval&  timeout,
                    int                       flags = 0);
     int timedReadv(int                      *augStatus,
                    const btes_Iovec         *buffers,
                    int                       numBuffers,
                    const bdet_TimeInterval&  timeout,
                    int                       flags = 0);
        // Read from this channel into the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers' the respective numbers of
        // bytes as specified in each corresponding 'btes_Iovec' buffer, or
        // interrupt after the specified absolute 'timeout' time is reached.
        // If the optionally specified 'flags' incorporates
        // 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes newly
        // read into 'buffers' (indicating a partial result) otherwise.  On a
        // partial result, load 'augStatus', if supplied, with 0 if 'timeout'
        // interrupted this operation or a positive value if the interruption
        // was due to an asynchronous event; otherwise, 'augStatus' is
        // unmodified.  A partial result typically does not invalidate this
        // channel; hence, this (or another) operation may be retried (with
        // arguments suitably adjusted) with some reasonable hope of success.
        // A negative "status", however, indicates a permanent error (leaving
        // the contents of 'buffers' undefined); -1 implies that the connection
        // was closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 'buffers' have sufficient capacity to
        // hold the requested data and 0 < numBytes.  Note that if the
        // specified 'timeout' value has already passed, the "read" operation
        // will still be attempted, but the attempt will not block.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    int readRaw(char *buffer,
                int   numBytes,
                int   flags = 0);
    int readRaw(int  *augStatus,
                char *buffer,
                int   numBytes,
                int   flags = 0);
        // *Atomically* read from this channel into the specified 'buffer' *at*
        // *most* the specified 'numBytes'.  If the optionally specified
        // 'flags' incorporates 'btesc_TimedChannel::ASYNC_INTERRUPT',
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
        // and 0 < numBytes.  Note that if the specified 'timeout' value has
        // already passed, the "read" operation will still be attempted, but
        // the attempt will not block.

    int timedReadRaw(char                     *buffer,
                      int                      numBytes,
                      const bdet_TimeInterval& timeout,
                      int                      flags = 0);
    int timedReadRaw(int                      *augStatus,
                      char                    *buffer,
                      int                      numBytes,
                      const bdet_TimeInterval& timeout,
                      int                      flags = 0);
        // *Atomically* read from this channel into the specified 'buffer' *at*
        // *most* the specified 'numBytes' or interrupt after the specified
        // absolute 'timeout' time is reached.  If the optionally specified
        // 'flags' incorporates 'btesc_TimedChannel::ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Optionally specify (as a
        // *leading* argument) 'augStatus' to receive status specific to a
        // "partial result".  Return 'numBytes' on success, a negative value on
        // error, and the number of bytes newly read into 'buffer' (indicating
        // a partial result) otherwise.  On a partial result, load 'augStatus',
        // if supplied, with 0 if 'timeout' interrupted this operation, a
        // positive value if an asynchronous event caused an interruption, or a
        // negative value if the atomic OS-level operation transmitted at least
        // one byte, but less than 'numBytes'; otherwise, 'augStatus' is
        // unmodified.  A partial result typically does not invalidate this
        // channel; hence, this (or another) operation may be retried (with
        // arguments suitably adjusted) with some reasonable hope of success.
        // A negative "status", however, indicates a permanent error (leaving
        // the contents of 'buffer' undefined); -1 implies that the connection
        // was closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 'buffer' has sufficient capacity to
        // hold the requested data and 0 < numBytes.  Note that if the
        // specified 'timeout' value has already passed, the "read" operation
        // will still be attempted, but the attempt will not block.

     int readvRaw(const btes_Iovec *buffers,
                  int               numBuffers,
                  int               flags = 0);
     int readvRaw(int              *augStatus,
                  const btes_Iovec *buffers,
                  int               numBuffers,
                  int               flags = 0);
        // *Atomically* read from this channel into the specified sequence of
        // 'buffers' of specified sequence length 'numBuffers' *at* *most* the
        // respective numbers of bytes as specified in each corresponding
        // 'btes_Iovec' buffer.  If the optionally specified 'flags'
        // incorporates 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous
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

     int timedReadvRaw(int                     *augStatus,
                       const btes_Iovec        *buffers,
                       int                      numBuffers,
                       const bdet_TimeInterval& timeout,
                       int                      flags = 0);
     int timedReadvRaw(const btes_Iovec        *buffers,
                       int                      numBuffers,
                       const bdet_TimeInterval& timeout,
                       int                      flags = 0);
        // *Atomically* read from this channel into the specified sequence of
        // 'buffers' of specified sequence length 'numBuffers' *at* *most* the
        // respective numbers of bytes as specified in each corresponding
        // 'btes_Iovec' buffer, or interrupt after the specified absolute
        // 'timeout' time is reached.  If the optionally specified 'flags'
        // incorporates 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Optionally specify (as a *leading* argument)
        // 'augStatus' to receive status specific to a "partial result".
        // Return 'numBytes' on success, a negative value on error, and the
        // number of bytes newly read into 'buffers' (indicating a partial
        // result) otherwise.  On a partial result, load 'augStatus', if
        // supplied, with 0 if 'timeout' interrupted this operation, a positive
        // value if an asynchronous event caused an interruption, or a negative
        // value if the atomic OS-level operation transmitted at least one
        // byte, but less than 'numBytes'; otherwise, 'augStatus' is
        // unmodified.  A partial result typically does not invalidate this
        // channel; hence, this (or another) operation may be retried (with
        // arguments suitably adjusted) with some reasonable hope of success.
        // A negative "status", however, indicates a permanent error (leaving
        // the contents of 'buffers' undefined); -1 implies that the connection
        // was closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 'buffers' have sufficient capacity to
        // hold the requested data and 0 < numBytes.  Note that if the
        // specified 'timeout' value has already passed, the "read" operation
        // will still be attempted, but the attempt will not block.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    int bufferedRead(const char **buffer,
                     int          numBytes,
                     int          flags = 0);
    int bufferedRead(int         *augStatus,
                     const char **buffer,
                     int          numBytes,
                     int          flags = 0);
        // Read from this channel into a channel-supplied buffer, identified
        // via the specified 'buffer', the specified 'numBytes'.  If the
        // optionally specified 'flags' incorporates
        // 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous events" are
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
        // undefined unless 0 < numBytes.

    int timedBufferedRead(const char               **buffer,
                          int                        numBytes,
                          const bdet_TimeInterval&   timeout,
                          int                        flags = 0);
    int timedBufferedRead(int                       *augStatus,
                          const char               **buffer,
                          int                        numBytes,
                          const bdet_TimeInterval&   timeout,
                          int                        flags = 0);
        // Read from this channel into a channel-supplied buffer, identified
        // via the specified 'buffer', the specified 'numBytes', or interrupt
        // after the specified absolute 'timeout' time is reached.  If the
        // optionally specified 'flags' incorporates
        // 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes newly
        // read into 'buffer' (indicating a partial result) otherwise.  Any
        // positive return value guarantees that 'buffer' will remain valid
        // until this channel is modified.  On a partial result, load
        // 'augStatus', if supplied, with 0 if 'timeout' interrupted this
        // operation, or a positive value if the interruption was due to an
        // asynchronous event; otherwise, 'augStatus' is unmodified.  A partial
        // result typically does not invalidate this channel; hence, this (or
        // another) operation may be retried with some reasonable hope of
        // success -- buffered data from a partial result remains available
        // until consumed by subsequent read operations.  A negative "status",
        // however, indicates a permanent error (leaving 'buffer' undefined);
        // -1 implies that the connection was closed by the peer (but the
        // converse is not guaranteed).  The behavior is undefined unless
        // 0 < numBytes.  Note that if the specified 'timeout' value has
        // already passed, the "read" operation will still be attempted, but
        // the attempt will not block.

    int bufferedReadRaw(const char **buffer,
                        int          numBytes,
                        int          flags = 0);
    int bufferedReadRaw(int         *augStatus,
                        const char **buffer,
                        int          numBytes,
                        int          flags = 0);
        // *Atomically* read from this channel into a channel-supplied buffer,
        // identified via the specified 'buffer', *at* *most* the specified
        // 'numBytes'.  If the optionally specified 'flags' incorporates
        // 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous events" are
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
        // guaranteed).  The behavior is undefined unless 0 < numBytes.

    int timedBufferedReadRaw(const char               **buffer,
                             int                        numBytes,
                             const bdet_TimeInterval&   timeout,
                             int                        flags = 0);
    int timedBufferedReadRaw(int                       *augStatus,
                             const char               **buffer,
                             int                        numBytes,
                             const bdet_TimeInterval&   timeout,
                             int                        flags = 0);
        // *Atomically* read from this channel into a channel-supplied buffer,
        // identified via the specified 'buffer', *at* *most* the specified
        // 'numBytes' or interrupt after the specified absolute 'timeout' time
        // is reached.  If the optionally specified 'flags' incorporates
        // 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes newly
        // read into 'buffer' (indicating a partial result) otherwise.  On a
        // partial result, load 'augStatus', if supplied, with 0 if 'timeout'
        // interrupted this operation, a positive value if an asynchronous
        // event caused an interruption, or a negative value if the atomic
        // OS-level operation transmitted at least one byte, but less than
        // 'numBytes'; otherwise, 'augStatus' is unmodified.  A partial result
        // typically does not invalidate this channel; hence, this (or another)
        // operation may be retried with some reasonable hope of success --
        // buffered data from a partial result remains available until consumed
        // by subsequent read operations.  A negative "status", however,
        // indicates a permanent error (leaving 'buffer' unset); -1 implies
        // that the connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless 0 < numBytes.  Note
        // that if the specified 'timeout' value has already passed, the "read"
        // operation will still be attempted, but the attempt will not block.

    // = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

    int write(const char *buffer,
              int         numBytes,
              int         flags = 0);
    int write(int        *augStatus,
              const char *buffer,
              int         numBytes,
              int         flags = 0);
        // Write to this channel from the specified 'buffer' the specified
        // 'numBytes'.  If the optionally specified 'flags' incorporates
        // 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous events" are
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
        // behavior is undefined unless 0 < numBytes.

    int timedWrite(int                      *augStatus,
                   const char               *buffer,
                   int                       numBytes,
                   const bdet_TimeInterval&  timeout,
                   int                       flags = 0);
    int timedWrite(const char               *buffer,
                   int                       numBytes,
                   const bdet_TimeInterval&  timeout,
                   int                       flags = 0);
        // Write to this channel from the specified 'buffer' the specified
        // 'numBytes' or interrupt after the specified absolute 'timeout' time
        // is reached.  If the optionally specified 'flags' incorporates
        // 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 'numBytes' on success, a negative value on error,
        // and the number of bytes newly written from 'buffer' (indicating a
        // partial result) otherwise.  On a partial result, load 'augStatus',
        // if supplied, with 0 if 'timeout' interrupted this operation, or a
        // positive value if the interruption was due to an asynchronous event;
        // otherwise, 'augStatus' is unmodified.  A partial result typically
        // does not invalidate this channel; hence, this (or another) operation
        // may be retried (with arguments suitably adjusted) with some
        // reasonable hope of success.  A negative "status", however, indicates
        // a permanent error; -1 implies that the connection was closed by the
        // peer (but the converse is not guaranteed).  The behavior is
        // undefined unless 0 < numBytes.  Note that if the specified 'timeout'
        // value has already passed, the "write" operation will still be
        // attempted, but the attempt will not block.

    int writeRaw(const char *buffer,
                 int         numBytes,
                 int         flags = 0);
    int writeRaw(int        *augStatus,
                 const char *buffer,
                 int         numBytes,
                 int         flags = 0);
        // *Atomically* write to this channel from the specified 'buffer' *at*
        // *most* the specified 'numBytes'.  If the optionally specified
        // 'flags' incorporates 'btesc_TimedChannel::ASYNC_INTERRUPT',
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
        // behavior is undefined unless 0 < numBytes.  Note that if the
        // specified 'timeout' value has already passed, the "write" operation
        // will still be attempted, but the attempt will not block.

    int timedWriteRaw(int                      *augStatus,
                      const char               *buffer,
                      int                       numBytes,
                      const bdet_TimeInterval&  timeout,
                      int                       flags = 0);
    int timedWriteRaw(const char               *buffer,
                      int                       numBytes,
                      const bdet_TimeInterval&  timeout,
                      int                       flags = 0);
        // *Atomically* write to this channel from the specified 'buffer' *at*
        // *most* the specified 'numBytes', or interrupt after the specified
        // absolute 'timeout' time is reached.  If the optionally specified
        // 'flags' incorporates 'btesc_TimedChannel::ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Optionally specify (as a
        // *leading* argument) 'augStatus' to receive status specific to a
        // "partial result".  Return 'numBytes' on success, a negative value on
        // error, and the number of bytes newly written from 'buffer'
        // (indicating a partial result) otherwise.  On a partial result, load
        // 'augStatus', if supplied, with 0 if 'timeout' interrupted this
        // operation, a positive value if an asynchronous event caused an
        // interruption, or a negative value if the atomic OS-level operation
        // transmitted at least one byte, but less than 'numBytes'; otherwise,
        // 'augStatus' is unmodified.  A partial result typically does not
        // invalidate this channel; hence, this (or another) operation may be
        // retried (with arguments suitably adjusted) with some reasonable hope
        // of success.  A negative "status", however, indicates a permanent
        // error; -1 implies that the connection was closed by the peer (but
        // the converse is not guaranteed).  The behavior is undefined unless
        // 0 < numBytes.  Note that if the specified 'timeout' value has
        // already passed, the "write" operation will still be attempted, but
        // the attempt will not block.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    int writev(const btes_Ovec *buffers,
               int              numBuffers,
               int              flags = 0);
    int writev(const btes_Iovec *buffers,
               int               numBuffers,
               int               flags = 0);
    int writev(int              *augStatus,
               const btes_Ovec *buffers,
               int              numBuffers,
               int              flags = 0);
    int writev(int             *augStatus,
               const btes_Iovec *buffers,
               int               numBuffers,
               int               flags = 0);
        // Write to this channel from the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers' the respective numbers of
        // bytes as specified in each corresponding 'btes_Ovec' (or
        // 'btes_Iovec') structure.  If the optionally specified 'flags'
        // incorporates 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous
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

    int timedWritev(const btes_Ovec         *buffers,
                    int                      numBuffers,
                    const bdet_TimeInterval& timeout,
                    int                      flags = 0);
    int timedWritev(const btes_Iovec        *buffers,
                    int                      numBuffers,
                    const bdet_TimeInterval& timeout,
                    int                      flags = 0);
    int timedWritev(int                     *augStatus,
                    const btes_Ovec         *buffers,
                    int                      numBuffers,
                    const bdet_TimeInterval& timeout,
                    int                      flags = 0);
    int timedWritev(int                     *augStatus,
                    const btes_Iovec        *buffers,
                    int                      numBuffers,
                    const bdet_TimeInterval& timeout,
                    int                      flags = 0);
        // Write to this channel from the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers' the respective numbers of
        // bytes as specified in each corresponding 'btes_Ovec' (or
        // 'btes_Iovec') structure, or interrupt after the specified absolute
        // 'timeout' time is reached.  If the optionally specified 'flags'
        // incorporates 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Optionally specify (as a *leading* argument)
        // 'augStatus' to receive status specific to a "partial result".
        // Return 'numBytes' on success, a negative value on error, and the
        // number of bytes newly written from 'buffer' (indicating a partial
        // result) otherwise.  On a partial result, load 'augStatus', if
        // supplied, with 0 if 'timeout' interrupted this operation or a
        // positive value if the interruption was due to an asynchronous event;
        // otherwise, 'augStatus' is unmodified.  A partial result typically
        // does not invalidate this channel; hence, this (or another) operation
        // may be retried (with arguments suitably adjusted) with some
        // reasonable hope of success.  A negative "status", however, indicates
        // a permanent error; -1 implies that the connection was closed by the
        // peer (but the converse is not guaranteed).  The behavior is
        // undefined unless 0 < numBytes.  Note that if the specified 'timeout'
        // value has already passed, the "write" operation will still be
        // attempted, but the attempt will not block.

    int writevRaw(const btes_Ovec *buffers,
                  int              numBuffers,
                  int              flags = 0);
    int writevRaw(const btes_Iovec *buffers,
                  int               numBuffers,
                  int               flags = 0);
    int writevRaw(int              *augStatus,
                  const btes_Ovec  *buffers,
                  int               numBuffers,
                  int               flags = 0);
    int writevRaw(int              *augStatus,
                  const btes_Iovec *buffers,
                  int               numBuffers,
                  int               flags = 0);
        // *Atomically* write to this channel, from the specified sequence of
        // 'buffers' of specified sequence length 'numBuffers', *at* *most* the
        // respective numbers of bytes as specified in each corresponding
        // 'btes_Ovec' (or 'btes_Iovec') buffer.  If the optionally specified
        // 'flags' incorporates 'btesc_TimedChannel::ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Optionally specify (as a
        // *leading* argument) 'augStatus' to receive status specific to a
        // "partial result".  Return 'numBytes' on success, a negative value on
        // error, and the number of bytes newly written from ' buffer'
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

    int timedWritevRaw(const btes_Ovec         *buffers,
                       int                      numBuffers,
                       const bdet_TimeInterval& timeout,
                       int                      flags = 0);
    int timedWritevRaw(const btes_Iovec        *buffers,
                       int                      numBuffers,
                       const bdet_TimeInterval& timeout,
                       int                      flags = 0);
    int timedWritevRaw(int                     *augStatus,
                       const btes_Ovec         *buffers,
                       int                      numBuffers,
                       const bdet_TimeInterval& timeout,
                       int                      flags = 0);
    int timedWritevRaw(int                     *augStatus,
                       const btes_Iovec        *buffers,
                       int                      numBuffers,
                       const bdet_TimeInterval& timeout,
                       int                      flags = 0);
        // *Atomically* write to this channel, from the specified sequence of
        // 'buffers' of specified sequence length 'numBuffers', *at* *most* the
        // respective numbers of bytes as specified in each corresponding
        // 'btes_Ovec' buffer, or interrupt after the specified absolute
        // 'timeout' time is reached.  If the optionally specified 'flags'
        // incorporates 'btesc_TimedChannel::ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Optionally specify (as a *leading* argument)
        // 'augStatus' to receive status specific to a "partial result".
        // Return 'numBytes' on success, a negative value on error, and the
        // number of bytes newly written from 'buffer' (indicating a partial
        // result) otherwise.  On a partial result, load 'augStatus', if
        // supplied, with 0 if 'timeout' interrupted this operation, a positive
        // value if an asynchronous event caused an interruption, or a negative
        // value if the atomic OS-level operation transmitted at least one
        // byte, but less than 'numBytes'; otherwise, 'augStatus' is
        // unmodified.  A partial result typically does not invalidate this
        // channel; hence, this (or another) operation may be retried (with
        // arguments suitably adjusted) with some reasonable hope of success.
        // A negative "status", however, indicates a permanent error; -1
        // implies that the connection was closed by the peer (but the converse
        // is not guaranteed).  The behavior is undefined unless 0 < numBytes.
        // Note that if the specified 'timeout' value has already passed, the
        // "write" operation will still be attempted, but the attempt will not
        // block.

    void invalidate();
        // Make this channel invalid; no subsequent operations can be completed
        // successfully.

    int getLocalAddress(bteso_IPv4Address *result);
        // Load into the specified 'result' the complete IP address associated
        // with the local (i.e., this process) end-point of this channel.
        // Return 0 on success and a non-zero value otherwise.

    int getOption(int *result, int level, int option);
        // Load into the specified 'result' the current value of the specified
        // option of the specified 'level' set on the underlying socket.
        // Return 0 on success and a non-zero value otherwise.  The list of
        // commonly-supported options (and levels) is enumerated in
        // 'bteso_socketoptutil'.

    int getPeerAddress(bteso_IPv4Address *result);
        // Load into the specified 'result' the complete IP address associated
        // with the remote (i.e., peer process) end-point of this channel.
        // Return 0 on success and a non-zero value otherwise.

    int setOption(int level, int option, int value);
        // Set the specified socket 'option' of the specified 'level' on the
        // underlying socket to the specified 'value'.  Return 0 on success and
        // a non-zero value otherwise.  (The list of commonly-supported options
        // is available in 'bteso_socketoptutil'.)

    // ACCESSORS
    int isInvalid() const;
        // Return 1 if *any* transmission error has occurred or if the channel
        // has been explicitly invalidated (via 'invalidate') and 0 otherwise.
        // Once a channel is invalid, no operations can be completed
        // successfully.  Note also that 0 return value does NOT guarantee that
        // a subsequent I/O operation would not fail.

    bteso_StreamSocket<bteso_IPv4Address> *socket() const;
        // Return the address of the stream-socket used by this channel.

};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTIONS' DEFINITIONS
//-----------------------------------------------------------------------------
inline
void btesos_TcpTimedChannel::invalidate()
{
    d_isInvalidFlag = 1;
}

inline
bteso_StreamSocket<bteso_IPv4Address> *btesos_TcpTimedChannel::socket() const
{
    return d_socket_p;
}

inline
int btesos_TcpTimedChannel::isInvalid() const
{
    return d_isInvalidFlag;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
