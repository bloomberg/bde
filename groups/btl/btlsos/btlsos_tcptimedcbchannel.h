// btlsos_tcptimedcbchannel.h                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSOS_TCPTIMEDCBCHANNEL
#define INCLUDED_BTLSOS_TCPTIMEDCBCHANNEL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide timed stream-based communication channel over TCP sockets.
//
//@CLASSES:
//  btlsos::TcpTimedCbChannel: non-blocking channel over TCP/IPv4 sockets
//
//@SEE_ALSO:  btlsos_tcptimedcbacceptor btlsos_tcptimedcbconnector
//
//@SEE_ALSO:  btlsos_tcpcbchannel btlso_streamsocket
//
//@SEE_ALSO:  btlsc_timedcbchannel btlsc_timedchannel
//
//@DESCRIPTION: This component provides a non-blocking communication channel
// over TCP/IPv4 sockets, 'btlsos::TcpTimedCbChannel', that adheres to
// 'btlsc::TimedCbChannel' protocol.  Following the protocol, both timed and
// non-timed operations are supported.  The channel uses user-installed timer
// event manager(s) to monitor an underlying socket for incoming events; two
// event managers can be installed in order to monitor for read and write
// events in different event managers.
//
// This channel uses a user-provided stream socket for IPv4 address family.
// Any concrete implementation that provides socket-like primitives is allowed.
// Common paradigms of acceptor and connector that produce channels directly is
// implemented (in other components) for both user convenience and completeness
// of the framework (see 'btlsos_tcptimedcbacceptor' and
// 'btlsos_tcptimedcbconnector' components).
//
///Thread Safety
///-------------
// Generally speaking, this channel is *not* *thread-safe*, meaning that any
// operation can not be called on *distinct instances* from different threads
// without any side-effects.  Thread-safety of this channel depends on whether
// or not given event manager(s), stream socket implementation, and memory
// allocator is *thread enabled (meaning that any operation can be invoked on
// the same instance from different threads).  If two channels share the same
// event manager, memory manager, and stream socket, then this two instances
// are thread safe if and only if the user-installed components are thread
// enabled.
//
///Performance
///-----------
// This channel is optimized for timed homogeneous operations, where
// homogeneity is consider within read or write domains in three different
// categories: buffered, non-buffered, and vectored.  Thus, it is advised that,
// for optimal performance, only one category within domain is used.  If timed
// operations are not required, 'btlsos_tcpcbchannel' is preferred from
// performance perspective.
//
///Usage
///-----
//..
// See the {'btlsos_tcptimedcbacceptor'} component.
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSC_TIMEDCBCHANNEL
#include <btlsc_timedcbchannel.h>
#endif

#ifndef INCLUDED_BDLMA_POOL
#include <bdlma_pool.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_DEQUE
#include <bsl_deque.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace btlso { class IPv4Address; }
namespace btlso { template<class ADDRESS> class StreamSocket; }
namespace btlso { class TimerEventManager; }
namespace btlsos {class TcpTimedCbChannel_RReg;  // Component local class

                                                 // declaration

class TcpTimedCbChannel_WReg;  // Component local class declaration

                         // =======================
                         // class TcpTimedCbChannel
                         // =======================

class TcpTimedCbChannel : public btlsc::TimedCbChannel {
    // This class implements a 'btesc'-style timed callback-based channel for a
    // single TCP/IPv4 socket.  The I/O requests are enqueued along with their
    // callbacks.  The callbacks are invoked as appropriate.  In general, the
    // status supplied to the callback is less than -1 for failure, -1 when
    // channel detects that connection is closed, 0 for an interrupted
    // operation due to either timeout (secondary status of 0), restartable
    // interruption of an underlying system call (secondary status of 1), or
    // dequeueing/cancellation (secondary status of -1).  A registration method
    // returns 0 if registration is successful, and a non-zero value otherwise,
    // in which case the channel is invalid and no other registration attempts
    // will succeed.

    btlso::StreamSocket<btlso::IPv4Address>
                                *d_socket_p;
    btlso::TimerEventManager    *d_rManager_p;
    btlso::TimerEventManager    *d_wManager_p;

    bsl::vector<char>            d_readBuffer;
    int                          d_readBufferOffset;
    int                          d_readBufferSP;      // stack pointer

    bsl::vector<char>            d_writeBuffer;
    int                          d_writeBufferOffset;

    bsl::deque<TcpTimedCbChannel_RReg *>
                                 d_readRequests;

    bsl::deque<TcpTimedCbChannel_WReg *>
                                 d_writeRequests;

    bsl::function<void()>        d_bufferedReadFunctor;
    bsl::function<void()>        d_readFunctor;
    bsl::function<void()>        d_writeFunctor;

    bsl::function<void()>        d_bufferedWriteFunctor;
    bsl::function<void()>        d_readTimerFunctor;
    bsl::function<void()>        d_writeTimerFunctor;

    bdlma::Pool                  d_rrequestPool; // read requests' pool
    bdlma::Pool                  d_wrequestPool; // write requests' pool
    bslma::Allocator            *d_allocator_p;

    int                          d_isInvalidReadFlag;
    int                          d_isInvalidWriteFlag;

    void                        *d_readTimerId;
    void                        *d_writeTimerId;

    TcpTimedCbChannel_RReg      *d_currentReadRequest_p;
    TcpTimedCbChannel_WReg      *d_currentWriteRequest_p;

  private:
    void initializeReadBuffer(int size = -1);
    void initializeWriteBuffer(int size = -1);
        // Initialize internal read/write buffers with the optionally specified
        // 'size'.  If 'size' is not specified, the default, which is obtained
        // by querying underlying socket, is used.

    // Callback methods.
    void bufferedReadCb();
        // Process the request on top of the read requests queue, invoke the
        // callback associated with the request as needed, and register
        // callbacks, as needed for the next request, if any.  The behavior is
        // undefined if read requests queue is empty or the request on top of
        // this queue is not a buffered read request.

    void readCb();
        // Process the request on top of the read requests queue, invoke the
        // callback associated with the request as needed, and register
        // callbacks, as needed for the next request, if any.  The behavior is
        // undefined if read requests queue is empty or the request on top of
        // this queue is a buffered read request.

    void readTimerCb();
        // Process the request on top of the read requests queue, invoke the
        // callback associated indicating timeout for the operation, and
        // register callbacks, as needed for the next request, if any.  The
        // behavior is undefined if read requests queue is empty.

    void bufferedWriteCb();
        // Process the request on top of the write requests queue, invoke the
        // callback associated with the request as needed, and register
        // callbacks, as needed for the next request, if any.  The behavior is
        // undefined if write requests queue is empty or the request on top of
        // this queue is not a buffered write request.

    void writeCb();
        // Process the request on top of the write requests queue, invoke the
        // callback associated with the request as needed, and register
        // callbacks, as needed for the next request, if any.  The behavior is
        // undefined if write requests queue is empty or the request on top of
        // this queue is a buffered write request.

    void writeTimerCb();
        // Process the request on top of the write requests queue, invoke the
        // callback associated indicating timeout for the operation, and
        // register callbacks, as needed for the next request, if any.  The
        // behavior is undefined if write requests queue is empty.

  private:
    // Not implemented:
    TcpTimedCbChannel(const TcpTimedCbChannel&);
    TcpTimedCbChannel& operator=(const TcpTimedCbChannel&);

  public:
    // CREATORS
    TcpTimedCbChannel(
                  btlso::StreamSocket<btlso::IPv4Address> *sSocket,
                  btlso::TimerEventManager                *manager,
                  bslma::Allocator                        *basicAllocator = 0);
        // Create a timed callback channel for the TCP/IPv4 that uses the
        // specified 'sSocket' for socket-level communications, the specified
        // 'manager' to monitor for incoming data and for availability of space
        // in the system's buffer for outgoing data.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless both 'sSocket' and 'manager' are not 0.  Note that
        // the I/O mode of 'sSocket' will be changed to non-blocking.  Usage of
        // 'sSocket' directly simultaneously with this channel may result in an
        // undefined behavior of the channel.

    TcpTimedCbChannel(
                  btlso::StreamSocket<btlso::IPv4Address> *sSocket,
                  btlso::TimerEventManager                *rManager,
                  btlso::TimerEventManager                *wManager,
                  bslma::Allocator                        *basicAllocator = 0);
        // Create a timed callback channel for the TCP/IPv4 that uses the
        // specified 'sSocket' for socket-level communications, the specified
        // 'rManager' to monitor for incoming data and the specified 'wManager'
        // for availability of space in the system's buffer for outgoing data.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 'sSocket', 'rManager' and
        // 'wManager' are not 0.  Note that the I/O mode of 'sSocket' will be
        // changed to non-blocking.  Usage of 'sSocket' directly simultaneously
        // with this channel may result in an undefined behavior of the
        // channel.

    ~TcpTimedCbChannel();
        // Invalidate this channel, cancel all read and write pending requests
        // (and invoke the associated callbacks), deregister from event
        // manager(s) as required, and destroy this channel.

    // MANIPULATORS
    int read(char                *buffer,
             int                  numBytes,
             const ReadCallback&  readCallback,
             int                  flags = 0);
        // Initiate a non-blocking operation to read the specified 'numBytes'
        // from this channel into the specified 'buffer'; execute the specified
        // 'readCallback' functor after this read operation terminates.  If the
        // optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'readCallback' will not be invoked).
        //
        // When invoked, the 'readCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to 'numBytes', the
        // operation was successful and 'buffer' is loaded with 'numBytes' of
        // newly read data.  Otherwise, if "status" is non-negative (incomplete
        // read), it indicates the number of bytes read into 'buffer' in which
        // case "augStatus" will be positive if the operation was interrupted
        // due to an asynchronous event, and negative ("status" identically 0)
        // if this operation was cancelled.  If the transmission is incomplete,
        // the channel itself potentially remains valid; hence, this (or
        // another) operation may be retried (with arguments suitably adjusted)
        // with some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error (leaving the contents of 'buffer'
        // undefined); -1 implies that the connection was closed by the peer
        // (but the converse is not guaranteed).  The behavior is undefined
        // unless 'buffer' has sufficient capacity to hold the requested data
        // and remains valid until the (non-null) 'readCallback' completes, and
        // '0 < numBytes'.

    int timedRead(char                      *buffer,
                  int                        numBytes,
                  const bsls::TimeInterval&  timeout,
                  const ReadCallback&        readCallback,
                  int                        flags = 0);
        // Initiate a non-blocking operation to read the specified 'numBytes'
        // from this channel into the specified 'buffer' or interrupt after the
        // specified absolute 'timeout' time is reached; execute the specified
        // 'readCallback' functor after this read operation terminates.  If the
        // optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'readCallback' will not be invoked).
        //
        // When invoked, 'readCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to 'numBytes', the
        // operation was successful and 'buffer' is loaded with 'numBytes' of
        // newly read data.  Otherwise, if "status" is non-negative (incomplete
        // read), it indicates the number of bytes read into 'buffer' in which
        // case "augStatus" will be 0 if the interrupt was caused by the
        // caller-requested time-out event, positive if the operation was
        // interrupted due to an asynchronous event, and negative ("status"
        // identically 0) if this operation was cancelled.  If the transmission
        // is incomplete, the channel itself potentially remains valid; hence,
        // this (or another) operation may be retried (with arguments suitably
        // adjusted) with some reasonable hope of success.  A negative
        // "status", however, indicates a permanent error (leaving the contents
        // of 'buffer' undefined); -1 implies that the connection was closed by
        // the peer (but the converse is not guaranteed).  The behavior is
        // undefined unless 'buffer' has sufficient capacity to hold the
        // requested data and remains valid until the (non-null) 'readCallback'
        // completes, and '0 < numBytes'.  Note that if the 'timeout' value has
        // already passed, the "read" operation will still be attempted, but
        // the attempt, once initiated, will not be permitted to block.

    int readv(const btls::Iovec   *buffers,
              int                  numBuffers,
              const ReadCallback&  readCallback,
              int                  flags = 0);
        // Initiate a non-blocking operation to read from this channel into the
        // specified sequence of 'buffers' of specified sequence length
        // 'numBuffers' the respective numbers of bytes as defined by the
        // 'd_length' fields of each 'Iovec' structure; execute the specified
        // 'readCallback' functor after this read operation terminates.  If the
        // optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'readCallback' will not be invoked).
        //
        // When invoked, 'readCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to the total number
        // of requested bytes (i.e., the sum of the 'numBuffers' 'd_length'
        // fields in 'buffers') the operation was successful and each
        // respective 'd_buffer_p' is loaded with its corresponding 'd_length'
        // bytes of newly read data.  Otherwise, if "status" is non-negative
        // (incomplete read), it indicates the number of bytes read into these
        // respective buffers in sequence in which case "augStatus" will be
        // positive if the operation was interrupted due to an asynchronous
        // event, and negative ("status" identically 0) if this operation was
        // cancelled.  A negative "status", however, indicates a permanent
        // error (leaving the contents of 'buffer' undefined); -1 implies that
        // the connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless 'buffer' has
        // sufficient capacity to hold the requested data and remains valid
        // until the (non-null) 'readCallback' completes, and 0 < numBytes.

    int timedReadv(const btls::Iovec         *buffers,
                   int                        numBuffers,
                   const bsls::TimeInterval&  timeout,
                   const ReadCallback&        readCallback,
                   int                        flags = 0);
        // Initiate a non-blocking operation to read from this channel into the
        // specified sequence of 'buffers' of specified sequence length
        // 'numBuffers' the respective numbers of bytes as defined by the
        // 'd_length' fields of each 'Iovec' structure or interrupt after the
        // specified absolute 'timeout' time is reached; execute the specified
        // 'readCallback' functor after this read operation terminates.  If the
        // optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'readCallback' will not be invoked).
        //
        // When invoked, 'readCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to the total number
        // of requested bytes (i.e., the sum of the 'numBuffers' 'd_length'
        // fields in 'buffers') the operation was successful and each
        // respective 'd_buffer_p' is loaded with its corresponding 'd_length'
        // bytes of newly read data.  Otherwise, if "status" is non-negative
        // (incomplete read), it indicates the number of bytes read into these
        // respective buffers in sequence in which case "augStatus" will be 0
        // if the interrupt was caused by the caller-requested time-out event,
        // positive if the operation was interrupted due to an asynchronous
        // event, and negative ("status" identically 0) if this operation was
        // cancelled.  A negative "status", however, indicates a permanent
        // error (leaving the contents of 'buffer' undefined); -1 implies that
        // the connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless 'buffer' has
        // sufficient capacity to hold the requested data and remains valid
        // until the (non-null) 'readCallback' completes, and 0 < numBytes.
        // Note that if the 'timeout' value has already passed, the "read"
        // operation will still be attempted, but the attempt, once initiated,
        // will not be permitted to block.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    int readRaw(char                *buffer,
                int                  numBytes,
                const ReadCallback&  readCallback,
                int                  flags = 0);
        // Initiate a non-blocking operation to *atomically* read *up *to* the
        // specified 'numBytes' from this channel into the specified 'buffer';
        // execute the specified 'readCallback' functor after this read
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events"
        // are permitted to interrupt this operation; by default, such events
        // are ignored.  Return 0 on successful initiation, and a non-zero
        // value otherwise (in which case 'readCallback' will not be invoked).
        //
        // When invoked, 'readCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to 'numBytes', the
        // operation was successful and 'buffer' is loaded with 'numBytes' of
        // newly read data.  Otherwise, if "status" is non-negative (incomplete
        // read), it indicates the number of bytes read into 'buffer' in which
        // case "augStatus" will be positive if the operation was interrupted
        // due to an asynchronous event, and negative (status > 0) if the
        // atomic OS-level write operation wrote at least one but less than
        // 'numBytes' or ("status" identically 0) if this operation was
        // cancelled.  If the transmission is incomplete, the channel itself
        // potentially remains valid; hence, this (or another) operation may be
        // retried (with arguments suitably adjusted) with some reasonable hope
        // of success.  A negative "status", however, indicates a permanent
        // error: -1 implies that the connection was closed by the peer (but
        // the converse is not guaranteed).  The behavior is undefined unless
        // 'buffer' has sufficient capacity to hold the requested data and
        // remains valid until the (non-null) 'readCallback' completes, and
        // '0 < numBytes'.

    int timedReadRaw(char                      *buffer,
                     int                        numBytes,
                     const bsls::TimeInterval&  timeout,
                     const ReadCallback&        readCallback,
                     int                        flags = 0);
        // Initiate a non-blocking operation to *atomically* read *up *to* the
        // specified 'numBytes' from this channel into the specified 'buffer'
        // or interrupt after the specified absolute 'timeout' time is reached;
        // execute the specified 'readCallback' functor after this read
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events"
        // are permitted to interrupt this operation; by default, such events
        // are ignored.  Return 0 on successful initiation, and a non-zero
        // value otherwise (in which case 'readCallback' will not be invoked).
        //
        // When invoked, 'readCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to 'numBytes', the
        // operation was successful and 'buffer' is loaded with 'numBytes' of
        // newly read data.  Otherwise, if "status" is non-negative (incomplete
        // read), it indicates the number of bytes read into 'buffer' in which
        // case "augStatus" will be 0 if the interrupt was caused by the
        // caller-requested time-out event, positive if the operation was
        // interrupted due to an asynchronous event, and negative (status > 0)
        // if the atomic OS-level operation transmitted at least one but less
        // than 'numBytes' or ("status" identically 0) if this operation was
        // cancelled.  If the transmission is incomplete, the channel itself
        // potentially remains valid; hence, this (or another) operation may be
        // retried (with arguments suitably adjusted) with some reasonable hope
        // of success.  A negative "status", however, indicates a permanent
        // error (leaving the contents of 'buffer' undefined); -1 implies that
        // the connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless 'buffer' has
        // sufficient capacity to hold the requested data and remains valid
        // until the (non-null) 'readCallback' completes, and '0 < numBytes'.
        // Note that if the 'timeout' value has already passed, the "read"
        // operation will still be attempted, but the attempt, once initiated,
        // will not be permitted to block.

    int readvRaw(const btls::Iovec   *buffers,
                 int                  numBuffers,
                 const ReadCallback&  readCallback,
                 int                  flags = 0);
        // Initiate a non-blocking operation to *atomically* read from this
        // channel into the specified sequence of 'buffers' of specified
        // sequence length 'numBuffers' *up* *to* the respective numbers of
        // bytes as defined by the 'd_length' fields of each 'Iovec' structure;
        // execute the specified 'readCallback' functor after this read
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events"
        // are permitted to interrupt this operation; by default, such events
        // are ignored.  Return 0 on successful initiation, and a non-zero
        // value otherwise (in which case 'readCallback' will not be invoked).
        //
        // When invoked, 'readCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to the total number
        // of requested bytes (i.e., the sum of the 'numBuffers' 'd_length'
        // fields in 'buffers') the operation was successful and each
        // respective 'd_buffer_p' is loaded with its corresponding 'd_length'
        // bytes of newly read data.  Otherwise, if "status" is non-negative
        // (incomplete read), it indicates the number of bytes read into these
        // respective buffers in sequence in which case "augStatus" will be
        // positive if the operation was interrupted due to an asynchronous
        // event, and negative (status > 0) if the atomic OS-level operation
        // transmitted at least one but less than the total requested number of
        // bytes or ("status" identically 0) if this operation was cancelled.
        // A negative "status", however, indicates a permanent error (leaving
        // the contents of 'buffer' undefined); -1 implies that the connection
        // was closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 'buffer' has sufficient capacity to
        // hold the requested data and remains valid until the (non-null)
        // 'readCallback' completes, and 0 < numBytes.

    int timedReadvRaw(const btls::Iovec         *buffers,
                      int                        numBuffers,
                      const bsls::TimeInterval&  timeout,
                      const ReadCallback&        readCallback,
                      int                        flags = 0);
        // Initiate a non-blocking operation to *atomically* read from this
        // channel into the specified sequence of 'buffers' of specified
        // sequence length 'numBuffers' *up* *to* the respective numbers of
        // bytes as defined by the 'd_length' fields of each 'Iovec' structure
        // or interrupt after the specified absolute 'timeout' time is reached;
        // execute the specified 'readCallback' functor after this read
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events"
        // are permitted to interrupt this operation; by default, such events
        // are ignored.  Return 0 on successful initiation, and a non-zero
        // value otherwise (in which case 'readCallback' will not be invoked).
        //
        // When invoked, 'readCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to the total number
        // of requested bytes (i.e., the sum of the 'numBuffers' 'd_length'
        // fields in 'buffers') the operation was successful and each
        // respective 'd_buffer_p' is loaded with its corresponding 'd_length'
        // bytes of newly read data.  Otherwise, if "status" is non-negative
        // (incomplete read), it indicates the number of bytes read into these
        // respective buffers in sequence in which case "augStatus" will be 0
        // if the interrupt was caused by the caller-requested time-out event,
        // positive if the operation was interrupted due to an asynchronous
        // event, and negative (status > 0) if the atomic OS-level operation
        // transmitted at least one but less than the total requested number of
        // bytes or ("status" identically 0) if this operation was cancelled.
        // A negative "status", however, indicates a permanent error (leaving
        // the contents of 'buffer' undefined); -1 implies that the connection
        // was closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 'buffer' has sufficient capacity to
        // hold the requested data and remains valid until the (non-null)
        // 'readCallback' completes, and 0 < numBytes.  Note that if the
        // 'timeout' value has already passed, the "read" operation will still
        // be attempted, but the attempt, once initiated, will not be permitted
        // to block.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    int bufferedRead(int                         numBytes,
                     const BufferedReadCallback& bufferedReadCallback,
                     int                         flags = 0);
        // Initiate a non-blocking operation to read the specified 'numBytes'
        // from this channel into a channel-supplied buffer; execute the
        // specified 'bufferedReadCallback' functor after this read operation
        // terminates.  If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'bufferedReadCallback' will not be
        // invoked).
        //
        // When invoked, 'bufferedReadCallback' is passed the address of a
        // non-modifiable character "buffer", an integer "status", and a second
        // integer "augStatus" (which is meaningful only upon an incomplete
        // transmission).  If "status" is equal to 'numBytes', the operation
        // was successful and "buffer" contains 'numBytes' of newly read data
        // (which will remain valid only until the callback completes).
        // Otherwise, if "status" is non-negative (incomplete read), it
        // indicates the number of (accessible) bytes in "buffer", in which
        // case "augStatus" will be positive if the operation was interrupted
        // due to an asynchronous event, and negative ("status" identically 0)
        // if this operation was cancelled.  If the transmission is partial,
        // the data is retained in the channel's buffer and the channel itself
        // potentially remains valid; hence, this (or another) operation may be
        // retried (with arguments suitably adjusted) with some reasonable hope
        // of success.  A negative "status", however, indicates a permanent
        // error (leaving the contents of 'buffer' undefined); -1 implies that
        // the connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless '0 < numBytes' and
        // 'bufferedReadCallback' is non-null.

    int timedBufferedRead(int                         numBytes,
                          const bsls::TimeInterval&   timeout,
                          const BufferedReadCallback& bufferedReadCallback,
                          int                         flags = 0);
        // Initiate a non-blocking operation to read the specified 'numBytes'
        // from this channel into a channel-supplied buffer or interrupt after
        // the specified absolute 'timeout' time is reached; execute the
        // specified 'bufferedReadCallback' functor after this read operation
        // terminates.  If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'bufferedReadCallback' will not be
        // invoked).
        //
        // When invoked, 'bufferedReadCallback' is passed the address of a
        // non-modifiable character "buffer", an integer "status", and a second
        // integer "augStatus" (which is meaningful only upon an incomplete
        // transmission).  If "status" is equal to 'numBytes', the operation
        // was successful and "buffer" contains 'numBytes' of newly read data
        // (which will remain valid only until the callback completes).
        // Otherwise, if "status" is non-negative (incomplete read), it
        // indicates the number of (accessible) bytes in "buffer", in which
        // case "augStatus" will be 0 if the interrupt was caused by the
        // caller-requested time-out event, positive if the operation was
        // interrupted due to an asynchronous event, and negative ("status"
        // identically 0) if this operation was cancelled.  If the transmission
        // is partial, the data is retained in the channel's buffer and the
        // channel itself potentially remains valid; hence, this (or another)
        // operation may be retried (with arguments suitably adjusted) with
        // some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error (leaving the contents of 'buffer'
        // undefined); -1 implies that the connection was closed by the peer
        // (but the converse is not guaranteed).  The behavior is undefined
        // unless '0 < numBytes' and 'bufferedReadCallback' is non-null.  Note
        // that if the 'timeout' value has already passed, the "read" operation
        // will still be attempted, but the attempt, once initiated, will not
        // be permitted to block.

    int bufferedReadRaw(int                         numBytes,
                        const BufferedReadCallback& bufferedReadCallback,
                        int                         flags = 0);
        // Initiate a non-blocking operation to *atomically* read *up *to* the
        // specified 'numBytes' from this channel into a channel-supplied
        // buffer; execute the specified 'bufferedReadCallback' functor after
        // this read operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events"
        // are permitted to interrupt this operation; by default, such events
        // are ignored.  Return 0 on successful initiation, and a non-zero
        // value otherwise (in which case 'bufferedReadCallback' will not be
        // invoked).
        //
        // When invoked, 'bufferedReadCallback' is passed the address of a
        // non-modifiable character "buffer", an integer "status", and a second
        // integer "augStatus" (which is meaningful only upon an incomplete
        // transmission).  If "status" is equal to 'numBytes', the operation
        // was successful and "buffer" contains 'numBytes' of newly read data
        // (which will remain valid only until the callback completes).
        // Otherwise, if "status" is non-negative (incomplete read), it
        // indicates the number of (accessible) bytes in "buffer", in which
        // case "augStatus" will be positive if the operation was interrupted
        // due to an asynchronous event and negative (status > 0) if the atomic
        // OS-level operation transmitted at least one but less than 'numBytes'
        // or ("status" identically 0) if this operation was cancelled.  If the
        // transmission is incomplete, the data is retained in the channel's
        // buffer and the channel itself potentially remains valid; hence, this
        // (or another) operation may be retried (with arguments suitably
        // adjusted) with some reasonable hope of success.  A negative
        // "status", however, indicates a permanent error (leaving the contents
        // of 'buffer' undefined); -1 implies that the connection was closed by
        // the peer (but the converse is not guaranteed).  The behavior is
        // undefined unless '0 < numBytes' and 'bufferedReadCallback' is
        // non-null.

    int timedBufferedReadRaw(int                         numBytes,
                             const bsls::TimeInterval&   timeout,
                             const BufferedReadCallback& bufferedReadCallback,
                             int                         flags = 0);
        // Initiate a non-blocking operation to *atomically* read *up *to* the
        // specified 'numBytes' from this channel into a channel-supplied
        // buffer or interrupt after the specified absolute 'timeout' time is
        // reached; execute the specified 'bufferedReadCallback' functor after
        // this read operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events"
        // are permitted to interrupt this operation; by default, such events
        // are ignored.  Return 0 on successful initiation, and a non-zero
        // value otherwise (in which case 'bufferedReadCallback' will not be
        // invoked).
        //
        // When invoked, 'bufferedReadCallback' is passed the address of a
        // non-modifiable character "buffer", an integer "status", and a second
        // integer "augStatus" (which is meaningful only upon an incomplete
        // transmission).  If "status" is equal to 'numBytes', the operation
        // was successful and "buffer" contains 'numBytes' of newly read data
        // (which will remain valid only until the callback completes).
        // Otherwise, if "status" is non-negative (incomplete read), it
        // indicates the number of (accessible) bytes in "buffer", in which
        // case "augStatus" will be 0 if the interrupt was caused by the
        // caller-requested time-out event, positive if the operation was
        // interrupted due to an asynchronous event, and negative (status > 0)
        // if the atomic OS-level operation transmitted at least one but less
        // than 'numBytes' or ("status" identically 0) if this operation was
        // cancelled.  If the transmission is incomplete, the data is retained
        // in the channel's buffer and the channel itself potentially remains
        // valid; hence, this (or another) operation may be retried (with
        // arguments suitably adjusted) with some reasonable hope of success.
        // A negative "status", however, indicates a permanent error (leaving
        // the contents of 'buffer' undefined); -1 implies that the connection
        // was closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless '0 < numBytes' and
        // 'bufferedReadCallback' is non-null.  Note that if the 'timeout'
        // value has already passed, the "read" operation will still be
        // attempted, but the attempt, once initiated, will not be permitted to
        // block.

    // = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

    int write(const char           *buffer,
              int                   numBytes,
              const WriteCallback&  writeCallback,
              int                   flags = 0);
        // Initiate a non-blocking operation to write the specified 'numBytes'
        // from the specified 'buffer' to this channel; execute the specified
        // 'writeCallback' functor after this write operation terminates.  If
        // the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'writeCallback' will not be invoked).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to 'numBytes', the
        // operation was successful and all 'numBytes' of data were transmitted
        // to the channel.  Otherwise, if "status" is non-negative (incomplete
        // write), it indicates the number of bytes written from 'buffer', in
        // which case "augStatus" will be positive if the operation was
        // interrupted due to an asynchronous event, and negative ("status"
        // identically 0) if this operation was cancelled.  If the transmission
        // is incomplete, the channel itself potentially remains valid; hence,
        // this (or another) operation may be retried (with arguments suitably
        // adjusted) with some reasonable hope of success.  A negative
        // "status", however, indicates a permanent error; -1 implies that the
        // connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless 'buffer' remains
        // valid until the (non-null) 'writeCallback' completes, and
        // '0 < numBytes'.

    int timedWrite(const char                *buffer,
                   int                        numBytes,
                   const bsls::TimeInterval&  timeout,
                   const WriteCallback&       writeCallback,
                   int                        flags = 0);
        // Initiate a non-blocking operation to write the specified 'numBytes'
        // from the specified 'buffer' to this channel or interrupt after the
        // specified absolute 'timeout' time is reached; execute the specified
        // 'writeCallback' functor after this write operation terminates.  If
        // the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'writeCallback' will not be invoked).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to 'numBytes', the
        // operation was successful and all 'numBytes' of data were transmitted
        // to the channel.  Otherwise, if "status" is non-negative (incomplete
        // write), it indicates the number of bytes written from 'buffer', in
        // which case "augStatus" will be 0 if the interrupt was caused by the
        // caller-requested time-out event, positive if the operation was
        // interrupted due to an asynchronous event, and negative ("status"
        // identically 0) if this operation was cancelled.  If the transmission
        // is incomplete, the channel itself potentially remains valid; hence,
        // this (or another) operation may be retried (with arguments suitably
        // adjusted) with some reasonable hope of success.  A negative
        // "status", however, indicates a permanent error; -1 implies that the
        // connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless 'buffer' remains
        // valid until the (non-null) 'writeCallback' completes, and 0 <
        // 'numBytes'.  Note that if the 'timeout' value has already passed,
        // the "write" operation will still be attempted, but the attempt, once
        // initiated, will not be permitted to block.

    int writeRaw(const char           *buffer,
                 int                   numBytes,
                 const WriteCallback&  writeCallback,
                 int                   flags = 0);
        // Initiate a non-blocking operation to *atomically* write *up *to* the
        // specified 'numBytes' from the specified 'buffer' to this channel;
        // execute the specified writeCallback' functor after this write
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events"
        // are permitted to interrupt this operation; by default, such events
        // are ignored.  Return 0 on successful initiation, and a non-zero
        // value otherwise (in which case the specified 'writeCallback' will
        // not be invoked).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to 'numBytes', the
        // operation was successful and all 'numBytes' of data were transmitted
        // to the channel.  Otherwise, if "status" is non-negative (incomplete
        // write), it indicates the number of bytes written from 'buffer', in
        // which case "augStatus" will be positive if the operation was
        // interrupted due to an asynchronous event, and negative (status > 0)
        // if the atomic OS-level operation transmitted at least one but less
        // than 'numBytes' or ("status" identically 0) if this operation was
        // cancelled.  If the transmission is incomplete, the channel itself
        // potentially remains valid; hence, this (or another) operation may be
        // retried (with arguments suitably adjusted) with some reasonable hope
        // of success.  A negative "status", however, indicates a permanent
        // error; -1 implies that the connection was closed by the peer (but
        // the converse is not guaranteed).  The behavior is undefined unless
        // 'buffer' remains valid until the (non-null) 'writeCallback'
        // completes, and '0 < numBytes'.

    int timedWriteRaw(const char                *buffer,
                      int                        numBytes,
                      const bsls::TimeInterval&  timeout,
                      const WriteCallback&       writeCallback,
                      int                        flags = 0);
        // Initiate a non-blocking operation to *atomically* write *up *to* the
        // specified 'numBytes' from the specified 'buffer' to this channel or
        // interrupt after the specified absolute 'timeout' time is reached;
        // execute the specified writeCallback' functor after this write
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events"
        // are permitted to interrupt this operation; by default, such events
        // are ignored.  Return 0 on successful initiation, and a non-zero
        // value otherwise (in which case the specified 'writeCallback' will
        // not be invoked).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to 'numBytes', the
        // operation was successful and all 'numBytes' of data were transmitted
        // to the channel.  Otherwise, if "status" is non-negative (incomplete
        // write), it indicates the number of bytes written from 'buffer', in
        // which case "augStatus" will be 0 if the interrupt was caused by the
        // caller-requested time-out event, positive if the operation was
        // interrupted due to an asynchronous event, and negative (status > 0)
        // if the atomic OS-level operation transmitted at least one but less
        // than 'numBytes' or ("status" identically 0) if this operation was
        // cancelled.  If the transmission is incomplete, the channel itself
        // potentially remains valid; hence, this (or another) operation may be
        // retried (with arguments suitably adjusted) with some reasonable hope
        // of success.  A negative "status", however, indicates a permanent
        // error; -1 implies that the connection was closed by the peer (but
        // the converse is not guaranteed).  The behavior is undefined unless
        // 'buffer' remains valid until the (non-null) 'writeCallback'
        // completes, and '0 < numBytes'.  Note that if the 'timeout' value has
        // already passed, the "write" operation will still be attempted, but
        // the attempt, once initiated, will not be permitted to block.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    int writev(const btls::Ovec     *buffers,
               int                   numBuffers,
               const WriteCallback&  writeCallback,
               int                   flags = 0);
    int writev(const btls::Iovec    *buffers,
               int                   numBuffers,
               const WriteCallback&  writeCallback,
               int                   flags = 0);
        // Initiate a non-blocking operation to write to this channel from the
        // specified sequence of 'buffers' of specified sequence length
        // 'numBuffers' the respective numbers of bytes as defined by the
        // 'd_length' fields of each 'Ovec' (or 'Iovec') structure; execute the
        // specified 'writeCallback' functor after this write operation
        // terminates.  If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'writeCallback' will not be invoked).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to the total number
        // bytes to be written (i.e., the sum of the 'numBuffers' 'd_length'
        // fields in 'buffers'), all indicated data was transmitted to the
        // channel.  Otherwise, if "status" is non-negative (incomplete write),
        // it indicates the number of bytes written in sequence from the
        // indicated buffers, in which case "augStatus" will be positive if the
        // operation was interrupted due to an asynchronous event, and negative
        // ("status" identically 0) if this operation was cancelled.  If the
        // transmission is incomplete, the channel itself potentially remains
        // valid; hence, this (or another) operation may be retried (with
        // arguments suitably adjusted) with some reasonable hope of success.
        // A negative "status", however, indicates a permanent error; -1
        // implies that the connection was closed by the peer (but the converse
        // is not guaranteed).  The behavior is undefined unless the total
        // number of bytes to be written is *positive* and 'buffers' (and the
        // data to which it refers) remains valid until the (non-null)
        // 'writeCallback' completes.

    int timedWritev(const btls::Ovec          *buffers,
                    int                        numBuffers,
                    const bsls::TimeInterval&  timeout,
                    const WriteCallback&       writeCallback,
                    int                        flags = 0);
    int timedWritev(const btls::Iovec         *buffers,
                    int                        numBuffers,
                    const bsls::TimeInterval&  timeout,
                    const WriteCallback&       writeCallback,
                    int                        flags = 0);
        // Initiate a non-blocking operation to write to this channel from the
        // specified sequence of 'buffers' of specified sequence length
        // 'numBuffers' the respective numbers of bytes as defined by the
        // 'd_length' fields of each 'Ovec' (or 'Iovec') structure or interrupt
        // after the specified absolute 'timeout' time is reached; execute the
        // specified 'writeCallback' functor after this write operation
        // terminates.  If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'writeCallback' will not be invoked).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to the total number
        // bytes to be written (i.e., the sum of the 'numBuffers' 'd_length'
        // fields in 'buffers'), all indicated data was transmitted to the
        // channel.  Otherwise, if "status" is non-negative (incomplete write),
        // it indicates the number of bytes written in sequence from the
        // indicated buffers, in which case "augStatus" will be 0 if the
        // interrupt was caused by the caller-requested time-out event,
        // positive if the operation was interrupted due to an asynchronous
        // event, and negative ("status" identically 0) if this operation was
        // cancelled.  If the transmission is incomplete, the channel itself
        // potentially remains valid; hence, this (or another) operation may be
        // retried (with arguments suitably adjusted) with some reasonable hope
        // of success.  A negative "status", however, indicates a permanent
        // error; -1 implies that the connection was closed by the peer (but
        // the converse is not guaranteed).  The behavior is undefined unless
        // the total number of bytes to be written is *positive* and 'buffers'
        // (and the data to which it refers) remains valid until the (non-null)
        // 'writeCallback' completes.  Note that if the 'timeout' value has
        // already passed, the "write" operation will still be attempted, but
        // the attempt, once initiated, will not be permitted to block.

    int writevRaw(const btls::Ovec     *buffers,
                  int                   numBuffers,
                  const WriteCallback&  writeCallback,
                  int                   flags = 0);
    int writevRaw(const btls::Iovec    *buffers,
                  int                   numBuffers,
                  const WriteCallback&  writeCallback,
                  int                   flags = 0);
        // Initiate a non-blocking operation to *atomically* write *up *to* the
        // total number of bytes indicated by the specified sequence of
        // 'buffers' of specified sequence length 'numBuffers' the respective
        // numbers of bytes as defined by the 'd_length' fields of each 'Ovec'
        // (or 'Iovec') structure; execute the specified 'writeCallback'
        // functor after this write operation terminates.  If the optionally
        // specified 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Return 0 on successful
        // initiation, and a non-zero value otherwise (in which case
        // 'writeCallback' will not be invoked).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to the total number
        // bytes to be written (i.e., the sum of the 'numBuffers' 'd_length'
        // fields in 'buffers'), all indicated data was transmitted to the
        // channel.  Otherwise, if "status" is non-negative (incomplete write),
        // it indicates the number of bytes written in sequence from the
        // indicated buffers, in which case "augStatus" will be positive if the
        // operation was interrupted due to an asynchronous event, and negative
        // (status > 0) if the atomic OS-level operation transmitted at least
        // one but less than 'numBytes' or ("status" identically 0) if this
        // operation was cancelled.  If the transmission is incomplete, the
        // channel itself potentially remains valid; hence, this (or another)
        // operation may be retried (with arguments suitably adjusted) with
        // some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error; -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless the total number of bytes to be written
        // is *positive* and 'buffers' (and the data to which it refers)
        // remains valid until the (non-null) 'writeCallback' completes.

    int timedWritevRaw(const btls::Ovec          *buffers,
                       int                        numBuffers,
                       const bsls::TimeInterval&  timeout,
                       const WriteCallback&       writeCallback,
                       int                        flags = 0);
    int timedWritevRaw(const btls::Iovec         *buffers,
                       int                        numBuffers,
                       const bsls::TimeInterval&  timeout,
                       const WriteCallback&       writeCallback,
                       int                        flags = 0);
        // Initiate a non-blocking operation to *atomically* write *up *to* the
        // total number of bytes indicated by the specified sequence of
        // 'buffers' of specified sequence length 'numBuffers' the respective
        // numbers of bytes as defined by the 'd_length' fields of each 'Ovec'
        // (or 'Iovec') structure or interrupt after the specified absolute
        // 'timeout' time is reached; execute the specified 'writeCallback'
        // functor after this write operation terminates.  If the optionally
        // specified 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Return 0 on successful
        // initiation, and a non-zero value otherwise (in which case
        // 'writeCallback' will not be invoked).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to the total number
        // bytes to be written (i.e., the sum of the 'numBuffers' 'd_length'
        // fields in 'buffers'), all indicated data was transmitted to the
        // channel.  Otherwise, if "status" is non-negative (incomplete write),
        // it indicates the number of bytes written in sequence from the
        // indicated buffers, in which case "augStatus" will be 0 if the
        // interrupt was caused by the caller-requested time-out event,
        // positive if the operation was interrupted due to an asynchronous
        // event, and negative (status > 0) if the atomic OS-level operation
        // transmitted at least one but less than 'numBytes' or ("status"
        // identically 0) if this operation was cancelled.  If the transmission
        // is incomplete, the channel itself potentially remains valid; hence,
        // this (or another) operation may be retried (with arguments suitably
        // adjusted) with some reasonable hope of success.  A negative
        // "status", however, indicates a permanent error; -1 implies that the
        // connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless the total number of
        // bytes to be written is *positive* and 'buffers' (and the data to
        // which it refers) remains valid until the(non-null) 'writeCallback'
        // completes.  Note that if the 'timeout' value has already passed, the
        // "write" operation will still be attempted, but the attempt, once
        // initiated, will not be permitted to block.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    int bufferedWrite(const char           *buffer,
                      int                   numBytes,
                      const WriteCallback&  writeCallback,
                      int                   flags = 0);
        // Initiate a non-blocking operation to write the specified 'numBytes'
        // from the specified 'buffer' to this channel; execute the specified
        // 'writeCallback' functor after this write operation terminates.  If
        // the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'writeCallback' will not be invoked).  Note
        // that the contents of 'buffer' need not be preserved after this
        // method returns (except for the purpose of initiating a retry in the
        // event that this operation results in a partial write.
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to 'numBytes', the
        // operation was successful and all 'numBytes' of data were transmitted
        // to the channel.  Otherwise, if "status" is non-negative (incomplete
        // write), it indicates the number of bytes written from 'buffer', in
        // which case "augStatus" will be positive if the operation was
        // interrupted due to an asynchronous event and negative (with "status"
        // identically 0) if this operation was cancelled.  If the transmission
        // is incomplete, the remaining buffered data is discarded, but the
        // channel itself potentially remains valid; hence, this (or another)
        // operation may be retried (with arguments suitably adjusted) with
        // some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error; -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 'buffer' remains valid until the
        // (non-null) 'writeCallback' completes, and '0 < numBytes'.

    int timedBufferedWrite(const char                *buffer,
                           int                        numBytes,
                           const bsls::TimeInterval&  timeout,
                           const WriteCallback&       writeCallback,
                           int                        flags = 0);
        // Initiate a non-blocking operation to write the specified 'numBytes'
        // from the specified 'buffer' to this channel or interrupt after the
        // specified absolute 'timeout' time is reached; execute the specified
        // 'writeCallback' functor after this write operation terminates.  If
        // the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'writeCallback' will not be invoked).  Note
        // that the contents of 'buffer' need not be preserved after this
        // method returns (except for the purpose of initiating a retry in the
        // event that this operation results in a partial write.
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to 'numBytes', the
        // operation was successful and all 'numBytes' of data were transmitted
        // to the channel.  Otherwise, if "status" is non-negative (incomplete
        // write), it indicates the number of bytes written from 'buffer', in
        // which case "augStatus" will be 0 if the interrupt was caused by the
        // caller-requested time-out event, positive if the operation was
        // interrupted due to an asynchronous event, and negative ("status"
        // identically 0) if this operation was cancelled.  If the transmission
        // is incomplete, the remaining buffered data is discarded, but the
        // channel itself potentially remains valid; hence, this (or another)
        // operation may be retried (with arguments suitably adjusted) with
        // some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error; -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 'buffer' remains valid until the
        // (non-null) 'writeCallback' completes, and '0 < numBytes'.  Note that
        // if the 'timeout' value has already passed, the "write" operation
        // will still be attempted, but the attempt, once initiated, will not
        // be permitted to block.

    int bufferedWritev(const btls::Ovec     *buffers,
                       int                   numBuffers,
                       const WriteCallback&  writeCallback,
                       int                   flags = 0);
    int bufferedWritev(const btls::Iovec    *buffers,
                       int                   numBuffers,
                       const WriteCallback&  writeCallback,
                       int                   flags = 0);
        // Initiate a non-blocking operation to write to this channel from the
        // specified sequence of 'buffers' of specified sequence length
        // 'numBuffers' the respective numbers of bytes as defined by the
        // 'd_length' fields of each 'Ovec' (or 'Iovec') structure; execute the
        // specified 'writeCallback' functor after this write operation
        // terminates.  If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'writeCallback' will not be invoked).  Note
        // that neither 'buffers' nor the data to which it refers need be
        // preserved after this method returns (except for the purpose of
        // initiating a retry in the event that this operation results in a
        // partial write.
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to the total number
        // bytes to be written (i.e., the sum of the 'numBuffers' 'd_length'
        // fields in 'buffers'), all indicated data was transmitted to the
        // channel.  Otherwise, if "status" is non-negative (incomplete write),
        // it indicates the number of bytes written in sequence from the
        // indicated buffers, in which case "augStatus" will be positive if the
        // operation was interrupted due to an asynchronous event, and negative
        // ("status" identically 0) if this operation was cancelled.  If the
        // transmission is incomplete, the remaining buffered data is
        // discarded, but the channel itself potentially remains valid; hence,
        // this (or another) operation may be retried (with arguments suitably
        // adjusted) with some reasonable hope of success.  A negative
        // "status", however, indicates a permanent error; -1 implies that the
        // connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless the total number of
        // bytes to be written is *positive* and 'buffers' (and the data to
        // which it refers) remains valid until the (non-null) 'writeCallback'
        // completes.

    int timedBufferedWritev(const btls::Ovec          *buffers,
                            int                        numBuffers,
                            const bsls::TimeInterval&  timeout,
                            const WriteCallback&       writeCallback,
                            int                        flags = 0);
    int timedBufferedWritev(const btls::Iovec         *buffers,
                            int                        numBuffers,
                            const bsls::TimeInterval&  timeout,
                            const WriteCallback&       writeCallback,
                            int                        flags = 0);
        // Initiate a non-blocking operation to write to this channel from the
        // specified sequence of 'buffers' of specified sequence length
        // 'numBuffers' the respective numbers of bytes as defined by the
        // 'd_length' fields of each 'Ovec' (or 'Iovec') structure or interrupt
        // after the specified absolute 'timeout' time is reached; execute the
        // specified 'writeCallback' functor after this write operation
        // terminates.  If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'writeCallback' will not be invoked).  Note
        // that neither 'buffers' nor the data to which it refers need be
        // preserved after this method returns (except for the purpose of
        // initiating a retry in the event that this operation results in a
        // partial write.
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to the total number
        // bytes to be written (i.e., the sum of the 'numBuffers' 'd_length'
        // fields in 'buffers'), all indicated data was transmitted to the
        // channel.  Otherwise, if "status" is non-negative (incomplete write),
        // it indicates the number of bytes written in sequence from the
        // indicated buffers, in which case "augStatus" will be 0 if the
        // interrupt was caused by the caller-requested time-out event,
        // positive if the operation was interrupted due to an asynchronous
        // event, and negative ("status" identically 0) if this operation was
        // cancelled.  If the transmission is incomplete, the remaining
        // buffered data is discarded, but the channel itself potentially
        // remains valid; hence, this (or another) operation may be retried
        // (with arguments suitably adjusted) with some reasonable hope of
        // success.  A negative "status", however, indicates a permanent error;
        // -1 implies that the connection was closed by the peer (but the
        // converse is not guaranteed).  The behavior is undefined unless the
        // total number of bytes to be written is *positive* and 'buffers' (and
        // the data to which it refers) remains valid until the (non-null)
        // 'writeCallback' completes.  Note that if the 'timeout' value has
        // already passed, the "write" operation will still be attempted, but
        // the attempt, once initiated, will not be permitted to block.

    // = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

    void cancelAll();
        // Remove all enqueued operations from both the read queue and the
        // write queue of this channel, and, in turn, invoke each callback with
        // a 0 "status" and a negative "augStatus".  Note that the validity of
        // this channel is not altered by this operation.

    void cancelRead();
        // Remove all enqueued operations from the read queue of this channel,
        // and, in turn, invoke each callback with a 0 "status" and a negative
        // "augStatus".  Note that the validity of this channel is not altered
        // by this operation.

    void cancelWrite();
        // Remove all enqueued operations from the write queue of this channel,
        // and, in turn, invoke each callback with a 0 "status" and a negative
        // "augStatus".  Note that the validity of this channel is not altered
        // by this operation.

    void invalidate();
        // Make this channel invalid; no new operations can be initiated
        // successfully.  Pending operations are not affected.

    void invalidateRead();
        // Invalidate the read portion of this channel; no new read operations
        // can be initiated successfully.  Pending operations are not affected.

    void invalidateWrite();
        // Invalidate the write portion of this channel; no new write
        // operations can be initiated successfully.  Pending operations are
        // not affected.

    void setReadEventManager(btlso::TimerEventManager *manager);
        // Set the event manager used for read operations on this channel to
        // the specified 'manager'.  The behavior is undefined if manager is 0
        // or unless numPendingReadOperations() is 0.

    void setWriteEventManager(btlso::TimerEventManager *manager);
        // Set the event manager used for write operations on this channel to
        // the specified 'manager'.  The behavior is undefined if manager is 0
        // or unless numPendingWriteOperations() is 0.

    // ACCESSORS
    int isInvalidRead() const;
        // Return 1 if the read portion of this (full-duplex) channel is
        // invalid (e.g., due to a read error or an explicit call to
        // 'invalidateRead'), and 0 otherwise.  Once the read portion of a
        // channel is invalid, no new read operations can be initiated
        // successfully.  Note that a 0 return value cannot be relied upon to
        // indicate that the read portion of this channel *is* valid.

    int isInvalidWrite() const;
        // Return 1 if the write portion of this (full-duplex) channel is
        // invalid (e.g., due to a write error or an explicit call to
        // 'invalidateWrite'), and 0 otherwise.  Once the write portion of a
        // channel is invalid, no new write operations can be initiated
        // successfully.  Note that a 0 return value cannot be relied upon to
        // indicate that the write portion of this channel *is* valid.

    int isInvalid() const;        // TBD - Remove this method
        // Return 1 if *any* transmission error has occurred or if the channel
        // has been explicitly invalidated (via 'invalidate'), and 0 otherwise.
        // Once a channel is invalid, no new operations can be initiated
        // successfully.  Note that the significance of a 0 return cannot be
        // relied upon beyond the return of this method.

    int numPendingReadOperations() const;
        // Return the total number of pending (buffered and unbuffered) read
        // operations for this channel.

    int numPendingWriteOperations() const;
        // Return the total number of pending write operations for this
        // channel.

    btlso::StreamSocket<btlso::IPv4Address> *socket() const;
        // Return the address of the stream-socket used by this channel.

    btlso::TimerEventManager *readEventManager() const;
        // Return the address of an event manager used for monitoring for
        // incoming data on this channel.

    btlso::TimerEventManager *writeEventManager() const;
        // Return the address of an event manager used for monitoring for
        // availability of space in the outgoing buffer on this channel.
};

// ----------------------------------------------------------------------------
//                             INLINE DEFINITIONS
// ----------------------------------------------------------------------------

inline
btlso::StreamSocket<btlso::IPv4Address>
         *TcpTimedCbChannel::socket() const
{
    return d_socket_p;
}

inline
btlso::TimerEventManager *TcpTimedCbChannel::readEventManager() const {
    return d_rManager_p;
}

inline
btlso::TimerEventManager *TcpTimedCbChannel::writeEventManager() const {
    return d_wManager_p;
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
