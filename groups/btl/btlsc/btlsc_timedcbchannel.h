// btlsc_timedcbchannel.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSC_TIMEDCBCHANNEL
#define INCLUDED_BTLSC_TIMEDCBCHANNEL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide protocol for stream-based data communication with timeout.
//
//@CLASSES:
//  btlsc::TimedCbChannel: non-blocking stream-based channel protocol w/timeout
//
//@SEE_ALSO: btlsc_timedchannel
//
//@DESCRIPTION: This component provides a class, 'btlsc::TimedCbChannel', that
// defines an abstract interface for an end-point of a bi-directional
// (full-duplex) non-blocking stream-based communication channel with timeout
// capability.  The protocol supports efficient "buffered" transport and the
// syntax to enable efficient vector I/O operations (i.e., Unix-style
// scatter/gather 'readv' and 'writev').  Various forms of "partial
// transmission" authorizations (i.e., "raw" OS-level atomic operations and
// interruptions due to a user-specified "timeout" and "asynchronous events")
// are also supported for each method as appropriate.
//
// Read and write operations are enqueued separately (facilitating full-duplex
// operation) and executed in order within each queue.  In contrast, the
// relative sequence in which the "read" and "write" queues are serviced is
// implementation dependent.
//
// Users can cancel enqueued operations via the 'cancelRead', 'cancelWrite, and
// 'cancelAll' methods.  Also, if a read (write) operation returns with a
// partial result (see below), all pending read (write) operations will be
// dequeued; the callback status will reflect the reason for the incomplete
// result.  Such dequeued operations may be re-submitted (presumably after
// inspection of the callback status) at the user's discretion.
//
// Finally, users can invalidate the read and write portions of a channel
// separately, or the entire channel at once.
//
///Protocol Hierarchy
///------------------
// The interface hierarchy (defined by direct public inheritance) of the
// 'btlsc::TimedCbChannel' protocol is as follows:
//..
//                        ,---------------------.
//                       ( btlsc::TimedCbChannel )
//                        `---------------------'
//                                   |
//                                   V
//                           ,----------------.
//                          ( btlsc::CbChannel )
//                           `----------------'
//..
// 'btlsc::TimedCbChannel' adds a "timeout" capability for read and write
// methods.
//
///Non-Blocking Stream-Based Transport
///-----------------------------------
// This interface establishes methods for non-blocking stream-based transport.
// A method call registers the I/O request and a user-supplied callback
// function object (functor), and returns immediately with a status indicating
// the success or failure of the registration.  After successful registration,
// the channel eventually services the enqueued I/O operation and invokes the
// callback function, which in turn conveys the status of the transmission.
// Note that if the initial registration is not successful, the callback
// function will *not* be invoked.  Also note that whether the callback is
// invoked before or after the registering method returns is not specified; in
// either case, if the registration was successful, then the return value will
// reflect success.
//
// Enqueued read and write operations proceed asynchronously to one of four
// possible results: (1) "success" -- the specified number of bytes was
// transmitted, (2) "partial transmission" -- the operation was interrupted
// (e.g., via a timeout), (3) "canceled transmission" -- the request was
// dequeued (either by the user or by the channel), or (4) "error" -- an
// implementation-dependent error occurred.  In all cases, status information
// and any appropriately available data are communicated to the user via the
// registered callback function, including status information to distinguish
// among various reasons for incomplete transmissions (see below).  The user
// may retry incomplete operations (with method arguments suitably adjusted),
// with a reasonable expectation of success.
//
// Finally, concrete non-blocking stream-based channels do a "best effort" in
// sending and receiving the specified data, but need not guarantee successful
// transmission.
//
///Callback Functor Registration
///- - - - - - - - - - - - - - -
// Once an operation is successfully initiated, a (reference-counted) copy of
// the ('bdlf') callback functor is retained by the concrete channel until the
// callback is executed.  Therefore, the user need not be concerned with
// preserving the resources explicitly used by the 'bdlf' functor.
//
///Buffered Transport
///------------------
// Most (but not all) I/O operations support a "buffered" variant that may be
// more efficient in some situations.  For such operations, the prefix
// "buffered" (or infix "Buffered") appears before the basic operation name in
// the full method name (e.g., 'bufferedRead', 'timedBufferedWrite').
//
// The "buffered" read operations optimize throughput (as opposed to latency)
// for relatively small transmissions under high-volume conditions.  The
// specific buffered-read method signatures enable the concrete transport to
// read as much as it efficiently can (i.e., without blocking) and buffer that
// information for current and future use.  In the buffered variant, the caller
// does not provide a buffer, but rather receives direct (non-modifiable)
// access to the implementation's buffered data.  In the event of a partial
// read (see below), the data remains buffered and subsequent reads will behave
// as if the buffered operation had never occurred.  Once a buffered read
// operation succeeds (i.e., receives the requested number of bytes) the
// buffered contents will become invalid after the callback function returns.
// Note that "buffered" and "vector" ('readv', see below) are incompatible read
// options.
//
// The "buffered" write operations relieve the caller from having to preserve
// an input buffer during processing.  A "buffered write" operation transmits
// as much data as possible without blocking, and then copies any remaining
// data to a buffer owned by the channel for eventual transmission.  Note that
// the callback function is not invoked until the I/O operation terminates
// (successfully or otherwise, see above).  Also note that "buffered" and "raw"
// (see below) are incompatible write options.
//
///Incomplete Transmissions: "Partial" and "Canceled"
///--------------------------------------------------
// The "simple" results of read and write operations are "success" (with a
// status equal to the requested number of bytes) and "error" (with a negative
// status).  More complex behavior is also supported, some of which is at the
// option of the user.  Specifically, the caller may authorize the possibility
// of another outcome via combinations of the following three mechanisms: (1) a
// user-requested timeout, (2) an interruption due to an asynchronous event,
// and (3) a "raw" operation, i.e., accepting the results of a single low-level
// (implementation dependent) atomic read/write operation.  These three
// mechanisms (discussed in more detail below) may each result in a "partial
// transmission", characterized by a non-negative status that is less than the
// requested number of bytes.  Note that timeouts and asynchronous events, but
// *not* raw operations, may result in a return status of zero bytes.
//
// A return status of exactly zero bytes may also occur via a fourth mechanism.
// As discussed above, an enqueued operation may be dequeued due to the
// partial-transmission status of a prior operation.  Also, the 'cancelRead',
// 'cancelWrite', and 'cancelAll' methods may be used to dequeue all enqueued
// requests from the read queue, the write queue, and both queues,
// respectively.  All such dequeued operations result in "canceled
// transmissions".
//
// "Partial" and "canceled" transmissions share some common characteristics,
// and so are collectively referred to as "incomplete transmissions".  The
// specific cause of an incomplete transmission (as deduced from "status") can
// in turn be determined by examining a second status value, called the
// "augStatus", described in detail below.  All incomplete transmissions may be
// re-submitted with a reasonable expectation of success.
//
///Asynchronous Events
///- - - - - - - - - -
// Methods in this protocol anticipate the possible occurrence of an
// "asynchronous event" (AE) during execution.  This interface cannot specify
// what an AE is, but a common example of an AE is a Unix signal.  Note,
// however, that a Unix signal need not be an AE, and an AE certainly need not
// be a signal.  It is up to the concrete implementation to define what is and
// is not an AE.
//
// Since the specific nature of an AE is not defined here, method behavior
// resulting from an AE likewise cannot be fully specified.  Rather, certain
// restrictions are imposed.  By default, AEs are either ignored or, if that is
// not possible, cause an error.  The user may, however, choose to authorize
// (when a method is invoked) that an AE causes a concrete implementation to
// return, if possible, a "partial transmission" (which may be resumed -- see
// above).  This authorization is made by incorporating (i.e., bitwise OR-ing)
// the 'btlsc::Flag::k_ASYNC_INTERRUPT' value into an optional (trailing)
// integer 'flags' argument to the method call.
//
///Timeouts
/// - - - -
// A timeout is registered by the caller, when a method is invoked, as a
// 'bsls::TimeInterval' value indicating the absolute *system* time after which
// the operation should be interrupted.  A timeout expiration will result in a
// "partial transmission" (see above).  Information regarding the nature of a
// partial result is provided via the callback as an "augStatus" argument (see
// below).
//
// The concrete channel will make a "best effort" to honor the timeout request
// promptly, but no guarantee can be made as to the maximum duration of any
// particular I/O attempt.  Any implementation of this interface will support a
// timeout granularity of ten milliseconds (0.01s) or less.  The timeout is
// guaranteed *not* to occur before the specified time has passed.  If a
// timeout is specified with a time that has already passed, the I/O operation
// will be attempted, but will not block.  Note that since respective read and
// write operations are enqueued separately, a long-running read (write)
// operation will not impact the behavior of a concurrent write (read)
// operation, but *can* affect subsequent read (write) operations.
//
///Raw Transmissions
///- - - - - - - - -
// All read and unbuffered write operations support a "raw" variant in which
// the function is allowed to return with a "partial transmission" if (1) *at*
// *least* *one* *byte* has been transmitted and (2) no additional bytes are
// *immediately* transmittable.  The "raw" mode is particularly useful to
// callers waiting for some read activity, who will then follow up with
// additional read requests after observing the initial transmission.  Raw
// transmissions are authorized by methods whose names end in the suffix 'Raw'.
// Note that the "raw" mode is not supported (and nonsensical) for
// buffered-write operations.
//
///'augStatus'
///- - - - - -
// Since any enqueued I/O request can be dequeued (canceled) by the channel or
// the user, and the user can authorize various modes supporting "partial
// transmission", the caller may wish to know the cause of an incomplete
// transmission.  Callback functions therefore take a second 'int' status
// value, 'augStatus' ("augmented status").  This value will be 0 if the
// operation timed out, a positive value if the interruption was due to an
// "asynchronous event", and a negative value if the request was dequeued by
// the channel or the user.
//
///Scatter/Gather ('readv'/'writev')
///---------------------------------
// This interface supports "vector I/O" -- the simultaneous reading from or
// writing to multiple buffers -- via Unix-style 'readv' and 'writev' variants
// of the normal single-buffer methods.  These vector methods rely on two
// auxiliary data structures defined in the 'btls_iovec' component.
//
// The 'btls::Iovec' structure will be familiar to most Unix systems
// programmers.  The 'Ovec' variant enables read operations to avoid having to
// cast away 'const' in order to hold the address of non-modifiable data to be
// written.  In either structure, the total number of bytes to be read or
// written is determined by the sum of the non-negative 'length' method values
// in the contiguous sequence of structures supplied to the method along with a
// *positive* sequence length (note that at least one of the 'length' values
// must be positive).  The following simple example shows how to create and
// populate an 'Ovec' structure array in preparation for a 'bufferedWritev'
// operation:
//..
//      static void myPrintWriteStatus(int status, int augStatus, int numBytes)
//          // Print to the user console the result of attempting to write the
//          // specified 'numBytes' based on the specified write 'status' and
//          // the auxiliary 'augStatus' (discussed below).  The behavior is
//          // undefined unless '0 < numBytes' and 'status <= numBytes'.  Note
//          // that 'augStatus' is ignored unless '0 <= status < numBytes'.
//     {
//         assert(0 < numBytes);
//         assert(status <= numBytes);
//
//         if (status == numBytes) {
//             bsl::cout << "All requested bytes written successfully."
//                       << bsl::endl;
//         }
//         else if (0 <= status) { // PARTIAL RESULTS ARE NOT AUTHORIZED BELOW
//
//             if (0 < augStatus) {
//                 bsl::cout << "Write interrupted (not by time out) after "
//                           << "writing " << status << " of " << numBytes
//                           << " bytes." << bsl::endl;
//             }
//             else (0 == augStatus) {
//                 bsl::cout << "Write timed out after writing " <<
//                           << status << " of " << numBytes << " bytes."
//                           << bsl::endl;
//             }
//             else {
//                 assert(augStatus < 0);
//                 if (0 == status) {
//                     bsl::cout
//                             << "Write operation dequeued due to partial "
//                             << "write in some preceding enqueued operation."
//                             << bsl::endl;
//                 }
//                 else {
//                     assert(status > 0);
//                     bsl::cout << "Write (efficiently) transmitted "
//                               << status
//                               << " of " << numBytes << bytes." << bsl::endl;
//                 }
//             }
//         }
//         else if (-1 == status) {
//             bsl::cout << "Write failed: connection was closed by peer."
//                       << bsl::endl;
//         }
//         else {
//             assert(status < -1);
//             bsl::cout << "Write failed: the reason is unknown."
//                       << bsl::endl;
//         }
//     }
//
//     void myWritevAndPrintStatusWhenAvailable(btlsc::TimedCbChannel *channel)
//         // Write the integer representation of the length of a character
//         // string followed by the (non-terminated) string data itself to
//         // the specified 'channel'; upon completion, report the status
//         // of the "write" operation to 'cout'.
//     {
//         const char *const MESSAGE     = "Hello World!";
//         const int         HEADER      = strlen(MESSAGE);
//         const int         NUM_BUFFERS = 2;
//
//         btls::Ovec buffers[NUM_BUFFERS];
//
//         buffers[0].setBuffer(&HEADER, sizeof HEADER);
//         buffers[1].setBuffer(MESSAGE, HEADER);
//
//         const int TOTAL_NUM_BYTES = buffers[0].length()
//                                   + buffers[1].length());
//
//         // Create a functor.
//
//         using namespace bdlf::PlaceHolders;
//         btlsc::TimedCbChannel::WriteCallback functor(
//                      bdlf::BindUtil::bind(
//                             &myPrintWriteStatus,
//                             _1, _2,
//                             TOTAL_NUM_BYTES));        // extra user argument
//
//         if (0 != channel->bufferedWritev(buffers, NUM_BUFFERS)) {
//             bsl::cout << "Buffered write operation failed immediately!"
//                       << bsl::endl;
//         }
//
//         // Notice that the 'bufferedWritev' operation above does not
//         // authorize any partial write operations whatsoever.  When
//         // invoked, the "status" passed to the callback will be either
//         // 'TOTAL_NUM_BYTES' for *success* or negative for *error*
//         // (baring some other pending "write" operation on this channel
//         // that explicitly enabled a partial result).
//    }
//..
// Note that once the 'bufferedWritev' operation returns, there is no need to
// preserve either the output buffers or the array of 'Ovec' (or 'Iovec')
// structures used to identify them (they would potentially be needed only to
// continue after a partial write, say, due to an interruption if such were
// authorized).  By contrast, both the output buffers and the array of 'Ovec'
// (or 'Iovec') structures must remain valid until an *unbuffered* 'writev'
// operation completes and its corresponding 'WriteCallback' is invoked.  Since
// all 'readv' operations are necessarily unbuffered, both the array of 'Iovec'
// structures and each input buffer must remain valid until the 'readv'
// operation completes and its 'ReadCallback' is invoked.  Note that "vectored"
// and "buffered" reads are inherently incompatible.
//
///Synopsis
///--------
// The following chart summarizes the set of 20 transmission methods that are
// available to read and write data from and to a 'btlsc::TimedCbChannel'; note
// that, for efficiency reasons, only three vector operations, 'readvRaw',
// 'writevRaw', and 'bufferedWritev' (and their "timed" counterparts) are
// provided.  Also, the "buffered write raw" operations are not supported by
// this protocol:
//..
//    Buffered  Re/Wr  Vec  Raw  Untimed Method Name  Timed Method Name
//    --------  -----  ---  ---  -------------------  -----------------
//              READ             read                 timedRead
//              READ        RAW  readRaw              timedReadRaw
//
//              READ   VEC  RAW  readvRaw             timedReadvRaw
//
//    BUFFERED  READ             bufferedRead         timedBufferedRead
//    BUFFERED  READ        RAW  bufferedReadRaw      timedBufferedReadRaw
//
//              WRITE            write                timedWrite
//              WRITE       RAW  writeRaw             timedWriteRaw
//
//              WRITE  VEC  RAW  writevRaw            timedWritevRaw
//
//    BUFFERED  WRITE            bufferedWrite        timedBufferedWrite
//    BUFFERED  WRITE  VEC       bufferedWritev       timedBufferedWritev
//..
// Each of these methods supports the specification of a flag value:
//..
//  btlsc::Flag::k_ASYNC_INTERRUPT
//..
// supplied in an optional trailing integer to enable interruptions due to
// "asynchronous events" to result in partial transmissions; by default,
// asynchronous events are ignored.
//
// The following summarizes the return status values for a request to transmit
// 'N' bytes.
//..
//   'status'     'augStatus'    meaning
//  ----------    ----------    --------------------------------------------
//      N             x         Success
//  [0..(N-1)]     positive     Interruption by an "asynchronous event"
//  [0..(N-1)]        0         Interruption by a user-requested timeout
//  [1..(N-1)]        x         Raw operation could not complete w/o blocking
//      0          negative     Pending operation dequeued before execution
//  negative          x         Error
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSC_CBCHANNEL
#include <btlsc_cbchannel.h>
#endif

#ifndef INCLUDED_BTLS_IOVEC
#include <btls_iovec.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

namespace BloombergLP {

namespace bsls { class TimeInterval; }

namespace btlsc {
                            // ====================
                            // class TimedCbChannel
                            // ====================

class TimedCbChannel : public CbChannel {
    // This class represents a protocol (pure abstract interface) for a
    // communications channel that supports timed (non-blocking) buffered read
    // and write operations on a byte stream.  In general, a non-negative
    // status indicates the number of bytes read or written, while a negative
    // status implies an unspecified error.  Note that an error status of -1
    // indicates that the connection is *known* to have been closed by the
    // peer.  The converse -- that -1 will be returned if/when a peer drops a
    // connection -- cannot be guaranteed by implementations of this interface
    // and must not be relied upon by users.

  private:
    // NOT IMPLEMENTED
    TimedCbChannel& operator=(const TimedCbChannel&);

  public:
    // CREATORS
    virtual ~TimedCbChannel();
        // Destroy this object.

    // TYPES
    typedef bsl::function<void(int, int)> ReadCallback;
        // Invoked as a result of any non-buffered read method, 'ReadCallback'
        // is an alias for a callback function object (functor) that returns
        // 'void' and takes as arguments an integer "status" indicating
        // *success*, an *incomplete* *read*, or an *error*, and a second
        // integer "augStatus".  Together, the two status values indicate four
        // possible reasons for any incomplete result: (1) interruption by a
        // timeout, (2) a (caller-authorized) interruption by an asynchronous
        // event, (3) a (caller-authorized) implementation-dependent,
        // data-driven optimization, or (4) an operation dequeued (canceled)
        // by the implementation or the user.

    typedef bsl::function<void(const char *, int, int)> BufferedReadCallback;
        // Invoked as a result of any buffered read method,
        // 'BufferedReadCallback' is an alias for a callback function object
        // (functor) that returns 'void' and takes as arguments the (potential)
        // address of a non-modifiable character "buffer"; an integer "status"
        // indicating *success*, an *incomplete* *read*, or an *error*, and a
        // second integer "augStatus".  Together, the two status values
        // indicate four possible reasons for any incomplete result: (1)
        // interruption by a timeout, (2) a (caller-authorized) interruption by
        // an asynchronous event, (3) a (caller-authorized)
        // implementation-dependent, data-driven optimization, or (4) an
        // operation dequeued (canceled) by the implementation or the user.

    typedef bsl::function<void(int, int)> WriteCallback;
        // Invoked as a result of any write method, 'WriteCallback' is an alias
        // for a callback function object (functor) that returns 'void' and
        // takes as arguments an integer "status" indicating *success*, an
        // *incomplete* *write*, or an *error*, and a second integer
        // "augStatus".  Together, the two status values indicate four possible
        // reasons for any incomplete result: (1) interruption by a timeout,
        // (2) a (caller-authorized) interruption by an asynchronous event, (3)
        // a (caller-authorized) implementation-dependent, data-driven
        // optimization, or (4) an operation dequeued (canceled) by the
        // implementation or the user.

    // MANIPULATORS
    virtual int read(char                *buffer,
                     int                  numBytes,
                     const ReadCallback&  readCallback,
                     int                  flags = 0) = 0;
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
        // incoming data.  Otherwise, if "status" is non-negative (incomplete
        // read), it indicates the number of bytes read into 'buffer' in which
        // case "augStatus" will be positive if the operation was interrupted
        // due to an asynchronous event, and negative ("status" identically 0)
        // if this operation was canceled.  If the transmission is incomplete,
        // the channel itself potentially remains valid; hence, this (or
        // another) operation may be retried (with arguments suitably adjusted)
        // with some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error (leaving the contents of 'buffer'
        // undefined); -1 implies that the connection was closed by the peer
        // (but the converse is not guaranteed).  The behavior is undefined
        // unless 'buffer' has sufficient capacity to hold the requested data
        // and remains valid until the (non-null) 'readCallback' completes, and
        // '0 < numBytes'.

    virtual int timedRead(char                      *buffer,
                          int                        numBytes,
                          const bsls::TimeInterval&  timeout,
                          const ReadCallback&        readCallback,
                          int                        flags = 0) = 0;
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
        // incoming data.  Otherwise, if "status" is non-negative (incomplete
        // read), it indicates the number of bytes read into 'buffer' in which
        // case "augStatus" will be 0 if the interrupt was caused by the
        // caller-requested timeout event, positive if the operation was
        // interrupted due to an asynchronous event, and negative ("status"
        // identically 0) if this operation was canceled.  If the transmission
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

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    virtual int readRaw(char                *buffer,
                        int                  numBytes,
                        const ReadCallback&  readCallback,
                        int                  flags = 0) = 0;
        // Initiate a non-blocking operation to *atomically* read *up* *to* the
        // specified 'numBytes' from this channel into the specified 'buffer';
        // execute the specified 'readCallback' functor after this read
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return 0 on successful initiation, and a
        // non-zero value otherwise (in which case 'readCallback' will not be
        // invoked).
        //
        // When invoked, 'readCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful for this method only
        // when "status" is zero).  A non-negative "status" indicates the
        // number of bytes read into 'buffer'.  If "status" is positive, the
        // atomic OS-level ("raw") read operation completed; "status" equals
        // 'numBytes' upon success.  In either case, "augStatus" has no
        // meaning.  If "status" is zero, the atomic read operation did not
        // complete, in which case "augStatus" is positive if an "asynchronous
        // event" interrupted the operation, and negative if the operation was
        // canceled.  If the transmission was interrupted, the channel itself
        // potentially remains valid; hence, this (or another) operation may be
        // retried (with arguments suitably adjusted) with some reasonable hope
        // of success.  A negative "status", however, indicates a permanent
        // error: -1 implies that the connection was closed by the peer (but
        // the converse is not guaranteed).  The behavior is undefined unless
        // 'buffer' has sufficient capacity to hold the requested data and
        // remains valid until the (non-null) 'readCallback' completes, and
        // '0 < numBytes'.

    virtual int timedReadRaw(char                      *buffer,
                             int                        numBytes,
                             const bsls::TimeInterval&  timeout,
                             const ReadCallback&        readCallback,
                             int                        flags = 0) = 0;
        // Initiate a non-blocking operation to *atomically* read *up* *to* the
        // specified 'numBytes' from this channel into the specified 'buffer'
        // or interrupt after the specified absolute 'timeout' time is reached;
        // execute the specified 'readCallback' functor after this read
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return 0 on successful initiation, and a
        // non-zero value otherwise (in which case 'readCallback' will not be
        // invoked).
        //
        // When invoked, 'readCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful for this method only
        // when "status" is zero).  A non-negative "status" indicates the
        // number of bytes read into 'buffer'.  If "status" is positive, the
        // atomic OS-level ("raw") read operation completed; "status" equals
        // 'numBytes' upon success.  In either case, "augStatus" has no
        // meaning.  If "status" is zero, the atomic read operation did not
        // complete, in which case "augStatus" is positive if an "asynchronous
        // event" interrupted the operation, zero if the operation timed out,
        // and negative if the operation was canceled.  If the transmission
        // was interrupted, the channel itself potentially remains valid;
        // hence, this (or another) operation may be retried (with arguments
        // suitably adjusted) with some reasonable hope of success.  A negative
        // "status", however, indicates a permanent error (leaving the contents
        // of 'buffer' undefined); -1 implies that the connection was closed by
        // the peer (but the converse is not guaranteed).  The behavior is
        // undefined unless 'buffer' has sufficient capacity to hold the
        // requested data and remains valid until the (non-null) 'readCallback'
        // completes, and '0 < numBytes'.  Note that if the 'timeout' value has
        // already passed, the "read" operation will still be attempted, but
        // the attempt, once initiated, will not be permitted to block.

    virtual int readvRaw(const btls::Iovec   *buffers,
                         int                  numBuffers,
                         const ReadCallback&  readCallback,
                         int                  flags = 0) = 0;
        // Initiate a non-blocking operation to *atomically* read from this
        // channel into the specified sequence of 'buffers' of the specified
        // sequence length 'numBuffers' *up* *to* the respective number of
        // bytes as defined by the 'length' method of each 'Iovec' structure;
        // execute the specified 'readCallback' functor after this read
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return 0 on successful initiation, and a
        // non-zero value otherwise (in which case 'readCallback' will not be
        // invoked).
        //
        // When invoked, 'readCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful for this method only
        // when "status" is zero).  A non-negative "status" indicates the total
        // number of bytes read into 'buffers'.  If "status" is positive, the
        // atomic ("raw") vector-read operation completed; "status" equals the
        // requested number of bytes (i.e., the sum of the lengths of the
        // 'numBuffers' 'buffers') upon success.  In either case, "augStatus"
        // has no meaning.  If "status" is zero, the atomic read operation did
        // not complete, in which case "augStatus" is positive if an
        // "asynchronous event" interrupted the operation, and negative if the
        // operation was canceled.  A negative "status", however, indicates a
        // permanent error (leaving the contents of 'buffers' undefined); -1
        // implies that the connection was closed by the peer (but the converse
        // is not guaranteed).  The behavior is undefined unless 'buffers' has
        // sufficient capacity to hold the requested data and remains valid
        // until the (non-null) 'readCallback' completes, and '0 < numBytes'.

    virtual int timedReadvRaw(const btls::Iovec         *buffers,
                              int                        numBuffers,
                              const bsls::TimeInterval&  timeout,
                              const ReadCallback&        readCallback,
                              int                        flags = 0) = 0;
        // Initiate a non-blocking operation to *atomically* read from this
        // channel into the specified sequence of 'buffers' of the specified
        // sequence length 'numBuffers' *up* *to* the respective number of
        // bytes as defined by the 'length' method of each 'Iovec' structure
        // or interrupt after the specified absolute 'timeout' time is reached;
        // execute the specified 'readCallback' functor after this read
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return 0 on successful initiation, and a
        // non-zero value otherwise (in which case 'readCallback' will not be
        // invoked).
        //
        // When invoked, 'readCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful for this method only
        // when "status" is zero).  A non-negative "status" indicates the total
        // number of bytes read into 'buffers'.  If "status" is positive, the
        // atomic ("raw") vector-read operation completed; "status" equals the
        // requested number of bytes (i.e., the sum of the lengths of the
        // 'numBuffers' 'buffers') upon success.  In either case, "augStatus"
        // has no meaning.  If "status" is zero, the atomic read operation did
        // not complete, in which case "augStatus" is positive if an
        // "asynchronous event" interrupted the operation, zero if the
        // operation timed out, and negative if the operation was canceled.  A
        // negative "status", however, indicates a permanent error (leaving the
        // contents of 'buffers' undefined); -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 'buffers' has sufficient capacity to
        // hold the requested data and remains valid until the (non-null)
        // 'readCallback' completes, and '0 < numBytes'.  Note that if the
        // 'timeout' value has already passed, the "read" operation will still
        // be attempted, but the attempt, once initiated, will not be permitted
        // to block.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    virtual int bufferedRead(int                         numBytes,
                             const BufferedReadCallback& bufferedReadCallback,
                             int                         flags = 0) = 0;
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
        // was successful and "buffer" contains 'numBytes' of incoming data
        // (which will remain valid only until the callback completes).
        // Otherwise, if "status" is non-negative (incomplete read), it
        // indicates the number of (accessible) bytes in "buffer", in which
        // case "augStatus" will be positive if the operation was interrupted
        // due to an asynchronous event, and negative ("status" identically 0)
        // if this operation was canceled.  If the transmission is partial,
        // the data is retained in the channel's buffer and the channel itself
        // potentially remains valid; hence, this (or another) operation may be
        // retried (with arguments suitably adjusted) with some reasonable hope
        // of success.  A negative "status", however, indicates a permanent
        // error (leaving the contents of "buffer" undefined); -1 implies that
        // the connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless '0 < numBytes' and
        // 'bufferedReadCallback' is non-null.

    virtual int timedBufferedRead(
                              int                         numBytes,
                              const bsls::TimeInterval&   timeout,
                              const BufferedReadCallback& bufferedReadCallback,
                              int                         flags = 0) = 0;
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
        // caller-requested timeout event, positive if the operation was
        // interrupted due to an asynchronous event, and negative ("status"
        // identically 0) if this operation was canceled.  If the transmission
        // is partial, the data is retained in the channel's buffer and the
        // channel itself potentially remains valid; hence, this (or another)
        // operation may be retried (with arguments suitably adjusted) with
        // some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error (leaving the contents of "buffer"
        // undefined); -1 implies that the connection was closed by the peer
        // (but the converse is not guaranteed).  The behavior is undefined
        // unless '0 < numBytes' and 'bufferedReadCallback' is non-null.  Note
        // that if the 'timeout' value has already passed, the "read" operation
        // will still be attempted, but the attempt, once initiated, will not
        // be permitted to block.

    virtual int bufferedReadRaw(
                              int                         numBytes,
                              const BufferedReadCallback& bufferedReadCallback,
                              int                         flags = 0) = 0;
        // Initiate a non-blocking operation to *atomically* read *up* *to* the
        // specified 'numBytes' from this channel into a channel-supplied
        // buffer; execute the specified 'bufferedReadCallback' functor after
        // this read operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return 0 on successful initiation, and a
        // non-zero value otherwise (in which case 'bufferedReadCallback' will
        // not be invoked).
        //
        // When invoked, 'bufferedReadCallback' is passed the address of a
        // non-modifiable character "buffer", an integer "status", and a second
        // integer "augStatus" (which is meaningful for this method only when
        // "status" is zero).  A non-negative "status" indicates the total
        // number of bytes read into "buffer".  If "status" is positive, the
        // atomic OS-level ("raw") read operation completed; "status" equals
        // 'numBytes' upon success.  In either case, "augStatus" has no
        // meaning.  If "status" is zero, the atomic read operation did not
        // complete, in which case "augStatus" is positive if an "asynchronous
        // event" interrupted the operation, and negative if the operation was
        // canceled.  If the transmission is incomplete (i.e., "status" less
        // than 'numBytes') the data is retained in the channel's buffer and
        // the channel itself potentially remains valid; hence, this (or
        // another) operation may be retried (with arguments suitably adjusted)
        // with some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error (leaving the contents of "buffer"
        // undefined); -1 implies that the connection was closed by the peer
        // (but the converse is not guaranteed).  The behavior is undefined
        // unless '0 < numBytes' and 'bufferedReadCallback' is non-null.

    virtual int
    timedBufferedReadRaw(int                         numBytes,
                         const bsls::TimeInterval&   timeout,
                         const BufferedReadCallback& bufferedReadCallback,
                         int                         flags = 0) = 0;
        // Initiate a non-blocking operation to *atomically* read *up* *to* the
        // specified 'numBytes' from this channel into a channel-supplied
        // buffer or interrupt after the specified absolute 'timeout' time is
        // reached; execute the specified 'bufferedReadCallback' functor after
        // this read operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return 0 on successful initiation, and a
        // non-zero value otherwise (in which case 'bufferedReadCallback' will
        // not be invoked).
        //
        // When invoked, 'bufferedReadCallback' is passed the address of a
        // non-modifiable character "buffer", an integer "status", and a second
        // integer "augStatus" (which is meaningful for this method only when
        // "status" is zero).  A non-negative "status" indicates the total
        // number of bytes read into "buffer".  If "status" is positive, the
        // atomic OS-level ("raw") read operation completed; "status" equals
        // 'numBytes' upon success.  In either case, "augStatus" has no
        // meaning.  If "status" is zero, the atomic read operation did not
        // complete, in which case "augStatus" is positive if an "asynchronous
        // event" interrupted the operation, zero if the operation timed out,
        // and negative if the operation was canceled.  If the transmission is
        // incomplete (i.e., "status" less than 'numBytes') the data is
        // retained in the channel's buffer and the channel itself potentially
        // remains valid; hence, this (or another) operation may be retried
        // (with arguments suitably adjusted) with some reasonable hope of
        // success.  A negative "status", however, indicates a permanent error
        // (leaving the contents of "buffer" undefined); -1 implies that the
        // connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless '0 < numBytes' and
        // 'bufferedReadCallback' is non-null.  Note that if the 'timeout'
        // value has already passed, the "read" operation will still be
        // attempted, but the attempt, once initiated, will not be permitted to
        // block.

    // = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

    virtual int write(const char           *buffer,
                      int                   numBytes,
                      const WriteCallback&  writeCallback,
                      int                   flags = 0) = 0;
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
        // identically 0) if this operation was canceled.  If the transmission
        // is incomplete, the channel itself potentially remains valid; hence,
        // this (or another) operation may be retried (with arguments suitably
        // adjusted) with some reasonable hope of success.  A negative
        // "status", however, indicates a permanent error; -1 implies that the
        // connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless 'buffer' remains
        // valid until the (non-null) 'writeCallback' completes and
        // '0 < numBytes'.

    virtual int timedWrite(const char                *buffer,
                           int                        numBytes,
                           const bsls::TimeInterval&  timeout,
                           const WriteCallback&       writeCallback,
                           int                        flags = 0) = 0;
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
        // caller-requested timeout event, positive if the operation was
        // interrupted due to an asynchronous event, and negative ("status"
        // identically 0) if this operation was canceled.  If the transmission
        // is incomplete, the channel itself potentially remains valid; hence,
        // this (or another) operation may be retried (with arguments suitably
        // adjusted) with some reasonable hope of success.  A negative
        // "status", however, indicates a permanent error; -1 implies that the
        // connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless 'buffer' remains
        // valid until the (non-null) 'writeCallback' completes and
        // '0 < numBytes'.  Note that if the 'timeout' value has already
        // passed, the "write" operation will still be attempted, but the
        // attempt, once initiated, will not be permitted to block.

    virtual int writeRaw(const char           *buffer,
                         int                   numBytes,
                         const WriteCallback&  writeCallback,
                         int                   flags = 0) = 0;
        // Initiate a non-blocking operation to *atomically* write *up* *to*
        // the specified 'numBytes' from the specified 'buffer' to this
        // channel; execute the specified 'writeCallback' functor after this
        // write operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return 0 on successful initiation, and a
        // non-zero value otherwise (in which case 'writeCallback' will not be
        // invoked).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful for this method only
        // when "status" is zero).  A non-negative "status" indicates the
        // number of bytes written from 'buffer'.  If "status" is positive, the
        // atomic OS-level ("raw") write operation completed; "status" equals
        // 'numBytes' upon success.  In either case, "augStatus" has no
        // meaning.  If "status" is zero, the atomic write operation did not
        // complete, in which case "augStatus" is positive if an "asynchronous
        // event" interrupted the operation, and negative if the operation was
        // canceled.  If the transmission was interrupted, the channel itself
        // potentially remains valid; hence, this (or another) operation may be
        // retried (with arguments suitably adjusted) with some reasonable hope
        // of success.  A negative "status", however, indicates a permanent
        // error; -1 implies that the connection was closed by the peer (but
        // the converse is not guaranteed).  The behavior is undefined unless
        // 'buffer' remains valid until the (non-null) 'writeCallback'
        // completes and '0 < numBytes'.

    virtual int timedWriteRaw(const char                *buffer,
                              int                        numBytes,
                              const bsls::TimeInterval&  timeout,
                              const WriteCallback&       writeCallback,
                              int                        flags = 0) = 0;
        // Initiate a non-blocking operation to *atomically* write *up* *to*
        // the specified 'numBytes' from the specified 'buffer' to this channel
        // or interrupt after the specified absolute 'timeout' time is reached;
        // execute the specified 'writeCallback' functor after this write
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return 0 on successful initiation, and a
        // non-zero value otherwise (in which case 'writeCallback' will not be
        // invoked).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful for this method only
        // when "status" is zero).  A non-negative "status" indicates the
        // number of bytes written from 'buffer'.  If "status" is positive, the
        // atomic OS-level ("raw") write operation completed; "status" equals
        // 'numBytes' upon success.  In either case, "augStatus" has no
        // meaning.  If "status" is zero, the atomic write operation did not
        // complete, in which case "augStatus" is positive if an "asynchronous
        // event" interrupted the operation, zero if the operation timed out,
        // and negative if the operation was canceled.  If the transmission
        // was interrupted, the channel itself potentially remains valid;
        // hence, this (or another) operation may be retried (with arguments
        // suitably adjusted) with some reasonable hope of success.  A negative
        // "status", however, indicates a permanent error; -1 implies that the
        // connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless 'buffer' remains
        // valid until the (non-null) 'writeCallback' completes and
        // '0 < numBytes'.  Note that if the 'timeout' value has already
        // passed, the "write" operation will still be attempted, but the
        // attempt, once initiated, will not be permitted to block.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    virtual int writevRaw(const btls::Iovec    *buffers,
                          int                   numBuffers,
                          const WriteCallback&  writeCallback,
                          int                   flags = 0) = 0;
    virtual int writevRaw(const btls::Ovec     *buffers,
                          int                   numBuffers,
                          const WriteCallback&  writeCallback,
                          int                   flags = 0) = 0;
        // Initiate a non-blocking operation to *atomically* write *up* *to*
        // the total number of bytes indicated by the specified sequence of
        // 'buffers' of the specified sequence length 'numBuffers' the
        // respective number of bytes as defined by the 'length' method of
        // each 'Ovec' (or 'Iovec') structure; execute the specified
        // 'writeCallback' functor after this write operation terminates.  If
        // the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'writeCallback' will not be invoked).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful for this method only
        // when "status" is zero).  A non-negative "status" indicates the total
        // number of bytes written from 'buffers'.  If "status" is positive,
        // the atomic ("raw") vector-write operation completed; "status" equals
        // the requested number of bytes (i.e., the sum of the lengths of the
        // 'numBuffers' 'buffers') upon success.  In either case, "augStatus"
        // has no meaning.  If "status" is zero, the atomic write operation did
        // not complete, in which case "augStatus" is positive if an
        // "asynchronous event" interrupted the operation, and negative if the
        // operation was canceled.  If the transmission is incomplete, the
        // channel itself potentially remains valid; hence, this (or another)
        // operation may be retried (with arguments suitably adjusted) with
        // some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error; -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless the total number of bytes to be written
        // is *positive* and 'buffers' (and the data to which it refers)
        // remains valid until the (non-null) 'writeCallback' completes.

    virtual int timedWritevRaw(const btls::Iovec         *buffers,
                               int                        numBuffers,
                               const bsls::TimeInterval&  timeout,
                               const WriteCallback&       writeCallback,
                               int                        flags = 0) = 0;
    virtual int timedWritevRaw(const btls::Ovec          *buffers,
                               int                        numBuffers,
                               const bsls::TimeInterval&  timeout,
                               const WriteCallback&       writeCallback,
                               int                        flags = 0) = 0;
        // Initiate a non-blocking operation to *atomically* write *up* *to*
        // the total number of bytes indicated by the specified sequence of
        // 'buffers' of the specified sequence length 'numBuffers' the
        // respective number of bytes as defined by the 'length' method of
        // each 'Ovec' (or 'Iovec') structure or interrupt after the specified
        // absolute 'timeout' time is reached; execute the specified
        // 'writeCallback' functor after this write operation terminates.  If
        // the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Return 0 on successful initiation, and a non-zero value
        // otherwise (in which case 'writeCallback' will not be invoked).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful for this method only
        // when "status" is zero).  A non-negative "status" indicates the total
        // number of bytes written from 'buffers'.  If "status" is positive,
        // the atomic ("raw") vector-write operation completed; "status" equals
        // the requested number of bytes (i.e., the sum of the lengths of the
        // 'numBuffers' 'buffers') upon success.  In either case, "augStatus"
        // has no meaning.  If "status" is zero, the atomic write operation did
        // not complete, in which case "augStatus" is positive if an
        // "asynchronous event" interrupted the operation, zero if the
        // operation timed out, and negative if the operation was canceled.  If
        // the transmission is incomplete, the channel itself potentially
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

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    virtual int bufferedWrite(const char           *buffer,
                              int                   numBytes,
                              const WriteCallback&  writeCallback,
                              int                   flags = 0) = 0;
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
        // event that this operation results in a partial write).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to 'numBytes', the
        // operation was successful and all 'numBytes' of data were transmitted
        // to the channel.  Otherwise, if "status" is non-negative (incomplete
        // write), it indicates the number of bytes written from 'buffer', in
        // which case "augStatus" will be positive if the operation was
        // interrupted due to an asynchronous event and negative (with "status"
        // identically 0) if this operation was canceled.  If the transmission
        // is incomplete, the remaining buffered data is discarded, but the
        // channel itself potentially remains valid; hence, this (or another)
        // operation may be retried (with arguments suitably adjusted) with
        // some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error; -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless '0 < numBytes'.

    virtual int timedBufferedWrite(const char                *buffer,
                                   int                        numBytes,
                                   const bsls::TimeInterval&  timeout,
                                   const WriteCallback&       writeCallback,
                                   int                        flags = 0) = 0;
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
        // event that this operation results in a partial write).
        //
        // When invoked, 'writeCallback' is passed an integer "status" and a
        // second integer "augStatus" (which is meaningful only upon an
        // incomplete transmission).  If "status" is equal to 'numBytes', the
        // operation was successful and all 'numBytes' of data were transmitted
        // to the channel.  Otherwise, if "status" is non-negative (incomplete
        // write), it indicates the number of bytes written from 'buffer', in
        // which case "augStatus" will be 0 if the interrupt was caused by the
        // caller-requested timeout event, positive if the operation was
        // interrupted due to an asynchronous event, and negative ("status"
        // identically 0) if this operation was canceled.  If the transmission
        // is incomplete, the remaining buffered data is discarded, but the
        // channel itself potentially remains valid; hence, this (or another)
        // operation may be retried (with arguments suitably adjusted) with
        // some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error; -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless '0 < numBytes'.  Note that if the
        // 'timeout' value has already passed, the "write" operation will still
        // be attempted, but the attempt, once initiated, will not be permitted
        // to block.

    virtual int bufferedWritev(const btls::Iovec    *buffers,
                               int                   numBuffers,
                               const WriteCallback&  writeCallback,
                               int                   flags = 0) = 0;
    virtual int bufferedWritev(const btls::Ovec     *buffers,
                               int                   numBuffers,
                               const WriteCallback&  writeCallback,
                               int                   flags = 0) = 0;
        // Initiate a non-blocking operation to write to this channel from the
        // specified sequence of 'buffers' of the specified sequence length
        // 'numBuffers' the respective number of bytes as defined by the
        // 'length' method of each 'Ovec' (or 'Iovec') structure; execute the
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
        // bytes to be written (i.e., the sum of calls to 'length' on the
        // 'numBuffers' 'buffers'), all indicated data was transmitted to the
        // channel.  Otherwise, if "status" is non-negative (incomplete write),
        // it indicates the number of bytes written in sequence from the
        // indicated buffers, in which case "augStatus" will be positive if the
        // operation was interrupted due to an asynchronous event, and negative
        // ("status" identically 0) if this operation was canceled.  If the
        // transmission is incomplete, the remaining buffered data is
        // discarded, but the channel itself potentially remains valid; hence,
        // this (or another) operation may be retried (with arguments suitably
        // adjusted) with some reasonable hope of success.  A negative
        // "status", however, indicates a permanent error; -1 implies that the
        // connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless the total number of
        // bytes to be written is *positive*.  Note that neither 'buffers' nor
        // the data to which it refers need be preserved after this method
        // returns (except for the purpose of initiating a retry in the event
        // that this operation results in a partial write).

    virtual int timedBufferedWritev(const btls::Iovec         *buffers,
                                    int                        numBuffers,
                                    const bsls::TimeInterval&  timeout,
                                    const WriteCallback&       writeCallback,
                                    int                        flags = 0) = 0;
    virtual int timedBufferedWritev(const btls::Ovec          *buffers,
                                    int                        numBuffers,
                                    const bsls::TimeInterval&  timeout,
                                    const WriteCallback&       writeCallback,
                                    int                        flags = 0) = 0;
        // Initiate a non-blocking operation to write to this channel from the
        // specified sequence of 'buffers' of the specified sequence length
        // 'numBuffers' the respective number of bytes as defined by the
        // 'length' method of each 'Ovec' (or 'Iovec') structure or interrupt
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
        // bytes to be written (i.e., the sum of calls to 'length' on the
        // 'numBuffers' 'buffers') all indicated data was transmitted to the
        // channel.  Otherwise, if "status" is non-negative (incomplete write),
        // it indicates the number of bytes written in sequence from the
        // indicated buffers, in which case "augStatus" will be 0 if the
        // interrupt was caused by the caller-requested timeout event,
        // positive if the operation was interrupted due to an asynchronous
        // event, and negative ("status" identically 0) if this operation was
        // canceled.  If the transmission is incomplete, the remaining
        // buffered data is discarded, but the channel itself potentially
        // remains valid; hence, this (or another) operation may be retried
        // (with arguments suitably adjusted) with some reasonable hope of
        // success.  A negative "status", however, indicates a permanent error;
        // -1 implies that the connection was closed by the peer (but the
        // converse is not guaranteed).  The behavior is undefined unless the
        // total number of bytes to be written is *positive*.  Note that if the
        // 'timeout' value has already passed, the "write" operation will still
        // be attempted, but the attempt, once initiated, will not be permitted
        // to block.  Also note that neither 'buffers' nor the data to which it
        // refers need be preserved after this method returns (except for the
        // purpose of initiating a retry in the event that this operation
        // results in a partial write).

    // = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

    virtual void cancelAll() = 0;
        // Remove all enqueued operations from both the read queue and the
        // write queue of this channel, and, in turn, invoke each callback with
        // a 0 "status" and a negative "augStatus".  Note that the validity of
        // this channel is not altered by this operation.

    virtual void cancelRead() = 0;
        // Remove all enqueued read operations from this channel, and, in turn,
        // invoke each read callback with a 0 "status" and a negative
        // "augStatus".  Note that the validity of this channel is not altered
        // by this operation.

    virtual void cancelWrite() = 0;
        // Remove all enqueued write operations from this channel, and, in
        // turn, invoke each write callback with a 0 "status" and a negative
        // "augStatus".  Note that the validity of this channel is not altered
        // by this operation.

    virtual void invalidate() = 0;
        // Make this channel invalid; no new operations can be initiated
        // successfully.  Pending operations are not affected.

    virtual void invalidateRead() = 0;
        // Invalidate the read portion of this channel; no new read operations
        // can be initiated successfully.  Pending operations are not affected.

    virtual void invalidateWrite() = 0;
        // Invalidate the write portion of this channel; no new write
        // operations can be initiated successfully.  Pending operations are
        // not affected.

    // ACCESSORS
    virtual int isInvalidRead() const = 0;
        // Return 1 if the read portion of this (full-duplex) channel is
        // invalid (e.g., due to a read error or an explicit call to
        // 'invalidateRead'), and 0 otherwise.  Once the read portion of a
        // channel is invalid, no new read operations can be initiated
        // successfully.  Note that a 0 return value cannot be relied upon to
        // indicate that the read portion of this channel *is* valid.

    virtual int isInvalidWrite() const = 0;
        // Return 1 if the write portion of this (full-duplex) channel is
        // invalid (e.g., due to a write error or an explicit call to
        // 'invalidateWrite'), and 0 otherwise.  Once the write portion of a
        // channel is invalid, no new write operations can be initiated
        // successfully.  Note that a 0 return value cannot be relied upon to
        // indicate that the write portion of this channel *is* valid.

    virtual int numPendingReadOperations() const = 0;
        // Return the total number of pending read operations for this channel.

    virtual int numPendingWriteOperations() const = 0;
        // Return the total number of pending write operations for this
        // channel.
};

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
