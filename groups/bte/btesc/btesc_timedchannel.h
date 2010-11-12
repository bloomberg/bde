// btesc_timedchannel.h        -*-C++-*-
#ifndef INCLUDED_BTESC_TIMEDCHANNEL
#define INCLUDED_BTESC_TIMEDCHANNEL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide protocol for stream-based communication with timeout.
//
//@CLASSES:
//  btesc_TimedChannel: synchronous stream-based channel protocol with timeout
//
//@SEE_ALSO: btesc_timedcbchannel, btemc_timedchannel,
//           btesc_timedchannelallocator, btesc_timedallocatorfactory
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION: This component defines an abstract interface for an end-point
// of a bi-directional synchronous (i.e., blocking) stream-based communication
// channel with timeout capability.  The 'btesc_TimedChannel' protocol supports
// efficient "buffered" transport and the syntax to enable efficient vector I/O
// operations (i.e., Unix-style scatter/gather "readv" and "writev").  Various
// forms of "partial transmission" authorizations (i.e., "raw" OS-level atomic
// operations and interruptions due to a user-specified "timeout" or an
// "asynchronous event") are also supported as appropriate.
//
///Protocol Hierarchy
///------------------
// The interface hierarchy (defined by direct public inheritance) of the
// 'btesc_TimedChannel' protocol is as follows:
//..
//                        ,------------------.
//                       ( btesc_TimedChannel )
//                        `------------------'
//                                  |
//                                  V
//                           ,-------------.
//                          ( btesc_Channel )
//                           `-------------'
//..
// This protocol adds a "timeout" capability for read and write methods.
//
///Synchronous Stream-Based Transport
///----------------------------------
// This interface establishes methods for synchronous stream-based transport.
// Each read and write method will block until one of three possible outcomes
// is reached: (1) "success" -- the specified number of bytes was transmitted,
// (2) "partial result" -- the operation was interrupted (e.g., via a timeout),
// (3) "error" -- an implementation-dependent error occurred.  In all cases, a
// "status" value is returned; an optional leading (int *) 'augStatus' argument
// may be provided to enable the caller to distinguish among various reasons
// for a partial result (see below).  The user may retry a partial-result
// operation (with method arguments suitably adjusted), with a reasonable
// expectation of success.  Finally, concrete blocking stream-based channels do
// a "best effort" in sending and receiving the specified data, but need not
// guarantee successful transmission.
//
///Buffered Transport
///------------------
// Non-vector read operations support a "buffered" variant that may be more
// efficient in some situations.  For such operations, the prefix "buffered"
// (or infix "Buffered") appears before the basic operation name in the full
// method name (e.g., 'bufferedRead', 'timedBufferedRead').  Note that, for
// synchronous channels, there is no need for buffered write operations.
//
// In the buffered operations, the caller does not provide a buffer, but rather
// receives direct (non-modifiable) access to the implementation's buffer.  In
// the event of a partial read (see below), the data remains buffered and
// subsequent reads will behave as if the buffered operation had never
// occurred.  Once a buffered read operation succeeds (i.e., receives the
// requested number of bytes) the buffered contents will remain valid only as
// long as the channel is not modified (or deallocated).  Note that "buffered"
// and "vector" ('readv', see below) are incompatible read options.
//
///Partial Results
///---------------
// The "simple" results of read and write operations are "success" (with a
// status equal to the requested number of bytes) and "error" (with a negative
// status).  More complex behavior is also supported, some of which is at the
// option of the user.  Specifically, the caller may authorize the possibility
// of another outcome via combinations of the following two mechanisms: (1) a
// user-requested timeout, and (2) an interruption due to an asynchronous
// event.  These two mechanisms (discussed in more detail below) may each
// return with a "partial result", indicated by a non-negative status that is
// less than the requested number of bytes.  Note that timeouts and
// asynchronous events may result in a return status of zero bytes.
//
///Asynchronous Events
///- - - - - - - - - -
// Methods in this protocol anticipate the possible occurrence of an
// "asynchronous event" (AE) during execution.  A common example of an AE is a
// Unix signal, but note that a specific Unix signal, if not detected or
// implemented, *may* not result in an AE, and an AE is certainly not limited
// to signals, even on Unix platforms.
//
// This interface cannot fully specify either the nature of or the behavior
// resulting from an AE, but certain restrictions can be (and are) imposed.  By
// default, AEs are either ignored or, if that is not possible, cause an error.
// At the user's option, however, a concrete implementation can be authorized
// to return, if such occurrence is detected, a "partial result" upon
// occurrence of an AE.  Such authorizations are made explicitly by
// incorporating into the optional (trailing) integer 'flags' argument to a
// method call the 'btesc_Flag::BTESC_ASYNC_INTERRUPT' value.
//
///Timeouts
///- - - -
// A timeout is registered by the caller, when a method is invoked, as a
// 'bdet_TimeInterval' value indicating the absolute *system* time after which
// the operation should be interrupted.  A timeout expiration will return a
// "partial result" (see above).  Information regarding the nature of a partial
// result is provided if the optional 'augStatus' argument is specified (see
// below).
//
// The concrete channel will make a "best effort" to honor the timeout request
// promptly, but no guarantee can be made as to the maximum duration of any
// particular I/O attempt.  Any implementation of this interface will support a
// timeout granularity of ten milliseconds (0.01s) or less.  The timeout is
// guaranteed *not* to occur before the specified time has passed.  If a
// timeout is specified with a time that has already passed, the I/O operation
// will be attempted, but will not block.
//
///Raw Transmissions Cannot Produce "Partial Results"
///- - - - - - - - - - - - - - - - - - - - - - - - -
// All read and unbuffered write methods support a "raw" variant in which the
// operation is allowed to return after transmitting at least one byte, but
// less than a "partial result" if (1) *at* *least* *one* *byte* has been
// transmitted and (2) no additional bytes are *immediately* transmittable.
// The "raw" mode is particularly useful to callers waiting for some read
// activity, who will then follow up with additional read requests after
// observing the initial transmission.  Raw transmissions are authorized by
// methods whose names end in the suffix 'Raw'.  Note that the "raw" mode is
// not supported for buffered-write operations.
//
///augStatus
///- - - - -
// Since there are several possible reasons for a "partial result", the caller
// may wish to know the specific cause.  A second status value, 'augStatus'
// ("augmented status") may be requested as an optional *initial* ('int*')
// argument to each transmission function.  If specified, 'augStatus' will be
// set to 0 if a user-supplied timeout interrupted the operation, and to a
// positive value if the interruption was due to an "asynchronous event".  Note
// that 'augStatus' will not be modified on success or error, but only on a
// partial result.
//
///Scatter/Gather (readv/writev)
///-----------------------------
// This interface supports "vector I/O" -- the simultaneous reading from or
// writing to multiple buffers -- via Unix-style 'readv' and 'writev' variants
// of the normal single-buffer methods.  Scatter/Gather operations use either
// the 'btes_Iovec' or 'btes_Ovec' objects which use a 'iovec' 'struct' on UNIX
// platforms or a 'WSABUF' 'struct' on Windows.  In either structure, the total
// number of bytes to be read or written is determined by the sum of each
// buffer of the non-negative 'numBuffers'.  Note that the 'btes_Ovec' variant
// enables write operations to avoid having to cast away 'const' in order to
// hold the address of non-modifiable data to be written.  The following simple
// example shows how to create and populate an 'btes_Ovec' array in preparation
// for a 'writev' operation.
//..
//  static void myPrintWriteStatus(int status,
//                                 int augStatus,
//                                 int numBytes)
//      // Print to the user console the result of a attempting to write
//      // the specified 'numBytes' based on the specified write 'status'
//      // and the auxiliary 'augStatus' (discussed below).  The behavior
//      // is undefined unless 0 < numBytes and status <= numBytes.  Note
//      // that 'augStatus' is ignored unless 0 <= status < numBytes.
//     {
//         assert(0 < numBytes);
//         assert(status <= numBytes);
//
//         if (status == numBytes) {
//             bsl::cout << "All requested bytes written successfully."
//                       << bsl::endl;
//         }
//         else if (status >= 0) { // PARTIAL RESULTS ARE NOT AUTHORIZED BELOW.
//
//             if (augStatus > 0) {
//                 bsl::cout << "Write interrupted (not by time out) after "
//                           << "writing " << status << " of " << numBytes
//                           << " bytes." << bsl::endl;
//             }
//             else (0 == augStatus) {
//                 bsl::cout << "Write timed out after writing " << status
//                           << " of " << numBytes << " bytes." << bsl::endl;
//             }
//             else {
//                 assert(0 < status);
//                 bsl::cout << "Write (efficiently) transmitted "
//                           << status << " of " << numBytes << bytes."
//                           << bsl::endl;
//             }
//         }
//         else if (-1 == status) {
//             bsl::cout << "Write failed: connection was closed by peer."
//                       << bsl::endl;
//         }
//         else {
//             assert (-1 > status);
//             bsl::cout << "Write failed: the reason is unknown."
//                       << bsl::endl;
//         }
//     }
//
//     void myWritevAndPrintStatusWhenAvailable(btesc_TimedChannel *channel)
//         // Write the integer representation of the length of a character
//         // string followed by the (null-terminated) string data itself to
//         // the specified 'channel'; upon completion, report the status
//         // of the "write" operation to 'cout'.
//     {
//         const char *const MESSAGE     = "Hello World!";
//         const int         HEADER      = strlen(MESSAGE);
//         const int         NUM_BUFFERS = 2;
//
//         btes_Ovec buffers[NUM_BUFFERS];
//
//         // Set each buffer's data and corresponding length.
//
//         buffer[0].setBuffer(&HEADER, sizeof HEADER);
//         buffer[1].setBuffer(MESSAGE, HEADER);
//
//         if (0 > channel->writev(buffers, NUM_BUFFERS)) {
//             bsl::cout << "Buffered write operation failed!"
//                       << bsl::endl;
//         }
//
//         // Notice that the 'writev' operation above does not
//         // authorize any partial write operations whatsoever.
//    }
//..
///Synopsis
///--------
// The following chart summarizes the set of 20 transmission methods that are
// available to read and write data from and to a 'btesc_TimedChannel'; note
// that "buffered readv" and all "buffered write (and writev)" operations (and
// their "timed" counterparts) are nonsensical and, therefore, omitted from the
// protocol:
//..
//    Buffered  Re/Wr  Vec  Raw  Untimed Method Name  Timed Method Name
//    --------  -----  ---  ---  -------------------  -----------------
//              READ             read                 timedRead
//              READ        RAW  readRaw              timedReadRaw
//
//              READ   VEC       readv                timedReadv
//              READ   VEC  RAW  readvRaw             timedReadvRaw
//
//    BUFFERED  READ             bufferedRead         timedBufferedRead
//    BUFFERED  READ        RAW  bufferedReadRaw      timedBufferedReadRaw
//
//              WRITE            write                timedWrite
//              WRITE       RAW  writeRaw             timedWriteRaw
//
//              WRITE  VEC       writev               timedWritev
//              WRITE  VEC  RAW  writevRaw            timedWritevRaw
//
//..
// Each of these methods supports the specification of a flag
//..
//                  btesc_Flag::BTESC_ASYNC_INTERRUPT
//..
// supplied in an optional trailing integer to enable "asynchronous events" to
// cause partial results; by default, such events are ignored.
//
// Each of these method is overloaded to allow the caller to optionally specify
// the address of an 'augStatus', which will then be modified in the event of a
// partial result.
//
///USAGE EXAMPLE
///=============
// The 'btesc' style of channel interface is used to transmit sequences of
// specified size across some concrete channel implementation.  In this example
// we demonstrate how to implement a remote procedure call (RPC) to a factorial
// function taking an 'int' and returning a 'double'.  For simplicity, we will
// assume that both the 'int' and 'double' formats are binary compatible across
// client and server platforms.
//..
//  double factorial(int number)
//      // Return the factorial of the specified integral number as a value of
//      // type 'double'.  The behavior is undefined unless 0 <= number.  Note
//      // that this helper function is provided for the server to calculate
//      // the factorial value.
//  {
//      if (0 == number) {
//          return 1;
//      }
//      else {
//          return number * factorial(number - 1);
//      }
//  }
//
//  int factorialClient(double *result, int input, btesc_TimedChannel *channel)
//      // Load into the specified 'result' the factorial of the specified
//      // 'input' using the specified 'channel' (which is assumed to be
//      // connected to an appropriate factorial service).  Return 0 on
//      // success and -1, with no effect on 'result', on error.  The behavior
//      // is undefined unless 0 <= input.
//  {
//      assert (0 <= input);
//      enum {
//          ERROR_STATUS   = -1,
//          SUCCESS_STATUS =  0,
//          TIMEOUT_STATUS =  1
//      };
//
//      enum {
//          WRITE_TIMEOUT = 1,     // seconds
//          READ_TIMEOUT  = 5      // seconds
//      };
//      bdet_TimeInterval timeNow = bdetu_SystemTime::now();
//      bdet_TimeInterval writeTimeout(timeNow + WRITE_TIMEOUT);
//      bdet_TimeInterval readTimeout(timeNow + READ_TIMEOUT);
//
//      int numBytes = sizeof input;
//      int writeStatus = channel->timedWrite((const char *)&input,
//                                             numBytes,
//                                             writeTimeout);
//      assert(0 != writeStatus);
//
//      if (writeStatus != numBytes) {
//          return ERROR_STATUS;
//      }
//
//      int readStatus = channel->timedRead((char *)result,
//                                          sizeof *result,
//                                          readTimeout);
//      if (readStatus != sizeof *result) {
//          return ERROR_STATUS;
//      }
//
//      return SUCCESS_STATUS;
//  }
//
//  int factorialServer(btesc_TimedChannel *channel)
//      // Repeatedly read integer sequences from the specified channel.
//      // Return a negative value if any read operation doesn't succeed
//      // (refer to the following 'enum' values for specific errors).
//      // When a read succeeds, interpret the byte sequence as an integer
//      // value in host-byte order.  Return -1 if that value is negative.
//      // Otherwise, calculate the factorial of the (non-negative)
//      // integer and write back the result to 'channel' as a sequence of
//      // bytes representing a 'double' in the host's native format.  Return
//      // a negative value if any write operation doesn't succeed (refer to
//      // the following 'enum' values for specific errors).
//      // Note that this implementation is just to show how a channel could be
//      // used, there's much room to improve.
//  {
//      enum {
//          SUCCESS            =  0,
//          INVALID_INPUT      = -1,
//          ERROR_READ         = -2,
//          ERROR_WRITE        = -3,
//          ERROR_TIMEOUT      = -4,
//          ERROR_INTERRUPTED  = -5,
//          ERROR_UNCLASSIFIED = -6
//      };
//
//      enum {
//          READ_TIME  = 3600, // an hour
//          WRITE_TIME = 2     // 2 seconds
//      };
//      bdet_TimeInterval timeNow = bdetu_SystemTime::now();
//      const bdet_TimeInterval READ_TIMEOUT(timeNow + READ_TIME),
//                              WRITE_TIMEOUT(timeNow + WRITE_TIME);
//
//      while (1) {
//          int input, augStatus;
//          int readStatus = channel->timedRead(&augStatus,
//                                              (char *) &input,
//                                              sizeof input,
//                                              READ_TIMEOUT);
//          if (readStatus < 0) {
//              return ERROR_READ;
//          }
//          if (readStatus != sizeof input) {
//              if (0 == augStatus) {
//                  return ERROR_TIMEOUT;
//              else if (augStatus > 0) {
//                  return ERROR_INTERRUPTED;
//              }
//              return ERROR_UNCLASSIFIED;
//          }
//          if (input < 0) {
//              return INVALID_INPUT;
//          }
//
//          augStatus = 0;
//          double result = factorial(input);
//          int writeStatus = channel->timedWrite(&augStatus,
//                                                (const char *)&result,
//                                                sizeof input,
//                                                WRITE_TIMEOUT);
//          if (writeStatus < 0) {
//              return ERROR_WRITE;
//          }
//          else if (writeStatus != sizeof input){
//              if (0 == augStatus) {
//                  return ERROR_TIMEOUT;
//              else if (augStatus > 0) {
//                  return ERROR_INTERRUPTED;
//              }
//              return ERROR_UNCLASSIFIED;
//          }
//      }
//  }
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESC_CHANNEL
#include <btesc_channel.h>
#endif

#ifndef INCLUDED_BTES_IOVEC
#include <btes_iovec.h>
#endif

namespace BloombergLP {

class bdet_TimeInterval;

                        // ========================
                        // class btesc_TimedChannel
                        // ========================

class btesc_TimedChannel : public btesc_Channel
{
    // This class defines a protocol (pure abstract interface) for a
    // synchronous communication channel that supports timed (blocking) read,
    // write, and buffered read operations on a byte stream.  In general, a
    // non-negative status indicates the number of bytes read or written, while
    // a negative status implies an unspecified error.  Note that an error
    // status of -1 indicates that the connection is *known* to have been
    // closed by the peer.

  public:
    // CREATORS
    virtual ~btesc_TimedChannel();
        // Destroy this channel (required for syntactic consistency only).

    // MANIPULATORS
    virtual int read(char *buffer, int numBytes, int flags = 0) = 0;
    virtual int read(int  *augStatus,
                     char *buffer,
                     int   numBytes,
                     int   flags = 0) = 0;
        // Read from this channel into the specified 'buffer' the specified
        // 'numBytes'.  If the optionally specified 'flags' incorporates
        // 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous events" are
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

    virtual int timedRead(char                     *buffer,
                          int                       numBytes,
                          const bdet_TimeInterval&  timeout,
                          int                       flags = 0) = 0;
    virtual int timedRead(int                      *augStatus,
                          char                     *buffer,
                          int                       numBytes,
                          const bdet_TimeInterval&  timeout,
                          int                       flags = 0) = 0;
        // Read from this channel into the specified 'buffer' the specified
        // 'numBytes' or interrupt after the specified absolute 'timeout' time
        // is reached.  If the optionally specified 'flags' incorporates
        // 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous events" are
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

    virtual int readv(const btes_Iovec *buffers,
                      int               numBuffers,
                      int               flags = 0) = 0;
    virtual int readv(int              *augStatus,
                      const btes_Iovec *buffers,
                      int               numBuffers,
                      int               flags = 0) = 0;
        // Read from this channel into the specified sequence of 'buffers' of
        // the specified 'numBuffers', the respective numbers of bytes as
        // specified in each corresponding 'btes_Iovec' buffer.  If the
        // optionally specified 'flags' incorporates
        // 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous events" are
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

    virtual int timedReadv(const btes_Iovec         *buffers,
                           int                       numBuffers,
                           const bdet_TimeInterval&  timeout,
                           int                       flags = 0) = 0;
    virtual int timedReadv(int                      *augStatus,
                           const btes_Iovec         *buffers,
                           int                       numBuffers,
                           const bdet_TimeInterval&  timeout,
                           int                       flags = 0) = 0;
        // Read from this channel into the specified sequence of 'buffers' of
        // the specified 'numBuffers', the respective numbers of bytes as
        // specified in each corresponding 'btes_Iovec' buffer, or interrupted
        // after the specified absolute 'timeout' time is reached.  If the
        // optionally specified 'flags' incorporates
        // 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous events" are
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

    virtual int readRaw(char *buffer,
                        int   numBytes,
                        int   flags = 0) = 0;
    virtual int readRaw(int  *augStatus,
                        char *buffer,
                        int   numBytes,
                        int   flags = 0) = 0;
        // *Atomically* read from this channel into the specified 'buffer' *at*
        // *most* the specified 'numBytes'.  If the optionally specified
        // 'flags' incorporates 'btesc_Flag::BTESC_ASYNC_INTERRUPT',
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
        // and 0 < numBytes.

    virtual int timedReadRaw(char                     *buffer,
                             int                       numBytes,
                             const bdet_TimeInterval&  timeout,
                             int                       flags = 0         ) = 0;
    virtual int timedReadRaw(int                      *augStatus,
                             char                     *buffer,
                             int                       numBytes,
                             const bdet_TimeInterval&  timeout,
                             int                       flags = 0         ) = 0;
        // *Atomically* read from this channel into the specified 'buffer' *at*
        // *most* the specified 'numBytes' or interrupt after the specified
        // absolute 'timeout' time is reached.  If the optionally specified
        // 'flags' incorporates 'btesc_Flag::BTESC_ASYNC_INTERRUPT',
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

    virtual int readvRaw(const btes_Iovec *buffers,
                         int               numBuffers,
                         int               flags = 0) = 0;
    virtual int readvRaw(int              *augStatus,
                         const btes_Iovec *buffers,
                         int               numBuffers,
                         int               flags = 0) = 0;
        // *Atomically* read from this channel into the specified sequence of
        // 'buffers' of the specified 'numBuffers', *at* *most* the respective
        // numbers of bytes as specified in each corresponding 'btes_Iovec'
        // buffer.  If the optionally specified 'flags' incorporates
        // 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes newly
        // read into 'buffers' (indicating a partial result) otherwise.  On a
        // partial result, load 'augStatus', if supplied, with a positive value
        // if an asynchronous event interrupted this operation and a negative
        // value if the atomic OS-level operation transmitted at least one
        // byte, but less than 'numBytes'; otherwise, 'augStatus' is
        // unmodified.  A partial result typically does not invalidate this
        // channel; hence, this (or another) operation may be retried (with
        // arguments suitably adjusted) with some reasonable hope of success.
        // A negative "status", however, indicates a permanent error (leaving
        // the contents of 'buffers' undefined); -1 implies that the connection
        // was closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 'buffers' have sufficient capacity to
        // hold the requested data and 0 < numBytes.

    virtual int timedReadvRaw(int                      *augStatus,
                              const btes_Iovec         *buffers,
                              int                       numBuffers,
                              const bdet_TimeInterval&  timeout,
                              int                       flags = 0) = 0;
    virtual int timedReadvRaw(const btes_Iovec         *buffers,
                              int                       numBuffers,
                              const bdet_TimeInterval&  timeout,
                              int                       flags = 0) = 0;
        // *Atomically* read from this channel into the specified sequence of
        // 'buffers' of the specified 'numBuffers' *at* *most* the respective
        // numbers of bytes as specified in each corresponding 'btes_Iovec'
        // buffer, or interrupted after the specified absolute 'timeout' time
        // is reached.  If the optionally specified 'flags' incorporates
        // 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes newly
        // read into 'buffers' (indicating a partial result) otherwise.  On a
        // partial result, load 'augStatus', if supplied, with 0 if 'timeout'
        // interrupted this operation, a positive value if an asynchronous
        // event caused an interruption, or a negative value if the atomic
        // OS-level operation transmitted at least one byte, but less than
        // 'numBytes'; otherwise, 'augStatus' is unmodified.  A partial result
        // typically does not invalidate this channel; hence, this (or another)
        // operation may be retried (with arguments suitably adjusted) with
        // some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error (leaving the contents of 'buffers'
        // undefined); -1 implies that the connection was closed by the peer
        // (but the converse is not guaranteed).  The behavior is undefined
        // unless 'buffers' have sufficient capacity to hold the requested data
        // and 0 < numBytes.  Note that if the specified 'timeout' value has
        // already passed, the "read" operation will still be attempted, but
        // the attempt will not block.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    virtual int bufferedRead(const char **buffer,
                             int          numBytes,
                             int          flags = 0) = 0;
    virtual int bufferedRead(int         *augStatus,
                             const char **buffer,
                             int          numBytes,
                             int          flags = 0) = 0;
        // Read from this channel into a channel-supplied buffer, identified
        // via the specified 'buffer', the specified 'numBytes'.  If the
        // optionally specified 'flags' incorporates
        // 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes
        // (indicating a partial result) otherwise.  Any positive return value
        // guarantees that '*buffer' will remain valid until this channel is
        // modified.  On a partial result, load 'augStatus', if supplied, with
        // a positive value, indicating that an asynchronous event caused the
        // interruption; otherwise, 'augStatus' is unmodified.  A partial
        // result typically does not invalidate this channel; hence, this (or
        // another) operation may be retried with some reasonable hope of
        // success -- buffered data from a partial result remains available
        // until consumed by subsequent read operations.  A negative "status",
        // however, indicates a permanent error (leaving 'buffer' undefined);
        // -1 implies that the connection was closed by the peer (but the
        // converse is not guaranteed).  The behavior is undefined unless
        // 0 < numBytes.

    virtual int timedBufferedRead(const char               **buffer,
                                  int                        numBytes,
                                  const bdet_TimeInterval&   timeout,
                                  int                        flags = 0) = 0;
    virtual int timedBufferedRead(int                       *augStatus,
                                  const char               **buffer,
                                  int                        numBytes,
                                  const bdet_TimeInterval&   timeout,
                                  int                        flags = 0) = 0;
        // Read from this channel into a channel-supplied buffer, identified
        // via the specified 'buffer', the specified 'numBytes', or interrupt
        // after the specified absolute 'timeout' time is reached.  If the
        // optionally specified 'flags' incorporates
        // 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes
        // (indicating a partial result) otherwise.  Any positive return value
        // guarantees that '*buffer' will remain valid until this channel is
        // modified.  On a partial result, load 'augStatus', if supplied, with
        // 0 if 'timeout' interrupted this operation, or a positive value if
        // the interruption was due to an asynchronous event; otherwise,
        // 'augStatus' is unmodified.  A partial result typically does not
        // invalidate this channel; hence, this (or another) operation may be
        // retried with some reasonable hope of success -- buffered data from a
        // partial result remains available until consumed by subsequent read
        // operations.  A negative "status", however, indicates a permanent
        // error (leaving 'buffer' undefined); -1 implies that the connection
        // was closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 0 < numBytes.  Note that if the
        // specified 'timeout' value has already passed, the "read" operation
        // will still be attempted, but the attempt will not block.

    virtual int bufferedReadRaw(const char **buffer,
                                int          numBytes,
                                int          flags = 0) = 0;
    virtual int bufferedReadRaw(int         *augStatus,
                                const char **buffer,
                                int          numBytes,
                                int          flags = 0) = 0;
        // *Atomically* read from this channel into a channel-supplied buffer,
        // identified via the specified 'buffer', *at* *most* the specified
        // 'numBytes'.  If the optionally specified 'flags' incorporates
        // 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes newly
        // read into 'buffer' (indicating a partial result) otherwise.  Any
        // positive return value guarantees that '*buffer' will remain valid
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

    virtual int timedBufferedReadRaw(const char               **buffer,
                                     int                        numBytes,
                                     const bdet_TimeInterval&   timeout,
                                     int                        flags = 0) = 0;
    virtual int timedBufferedReadRaw(int                       *augStatus,
                                     const char               **buffer,
                                     int                        numBytes,
                                     const bdet_TimeInterval&   timeout,
                                     int                        flags = 0) = 0;
        // *Atomically* read from this channel into a channel-supplied buffer,
        // identified via the specified 'buffer', *at* *most* the specified
        // 'numBytes' or interrupt after the specified absolute 'timeout' time
        // is reached.  If the optionally specified 'flags' incorporates
        // 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous events" are
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
        // indicates a permanent error (leaving '*buffer' unset); -1 implies
        // that the connection was closed by the peer (but the converse is not
        // guaranteed).  The behavior is undefined unless 0 < numBytes.  Note
        // that if the specified 'timeout' value has already passed, the "read"
        // operation will still be attempted, but the attempt will not block.

    // = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

    virtual int write(const char *buffer,
                      int         numBytes,
                      int         flags = 0) = 0;
    virtual int write(int        *augStatus,
                      const char *buffer,
                      int         numBytes,
                      int         flags = 0) = 0;
        // Write to this channel from the specified 'buffer' the specified
        // 'numBytes'.  If the optionally specified 'flags' incorporates
        // 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous events" are
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

    virtual int timedWrite(int                      *augStatus,
                           const char               *buffer,
                           int                       numBytes,
                           const bdet_TimeInterval&  timeout,
                           int                       flags = 0) = 0;
    virtual int timedWrite(const char               *buffer,
                           int                       numBytes,
                           const bdet_TimeInterval&  timeout,
                           int                       flags = 0) = 0;
        // Write to this channel from the specified 'buffer' the specified
        // 'numBytes' or interrupt after the specified absolute 'timeout' time
        // is reached.  If the optionally specified 'flags' incorporates
        // 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous events" are
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

    virtual int writeRaw(const char *buffer,
                         int         numBytes,
                         int         flags = 0) = 0;
    virtual int writeRaw(int        *augStatus,
                         const char *buffer,
                         int         numBytes,
                         int         flags = 0) = 0;
        // *Atomically* write to this channel from the specified 'buffer' *at*
        // *most* the specified 'numBytes'.  If the optionally specified
        // 'flags' incorporates 'btesc_Flag::BTESC_ASYNC_INTERRUPT',
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

    virtual int timedWriteRaw(int                      *augStatus,
                              const char               *buffer,
                              int                       numBytes,
                              const bdet_TimeInterval&  timeout,
                              int                       flags = 0) = 0;
    virtual int timedWriteRaw(const char               *buffer,
                              int                       numBytes,
                              const bdet_TimeInterval&  timeout,
                              int                       flags = 0) = 0;
        // *Atomically* write to this channel from the specified 'buffer' *at*
        // *most* the specified 'numBytes', or interrupt after the specified
        // absolute 'timeout' time is reached.  If the optionally specified
        // 'flags' incorporates 'btesc_Flag::BTESC_ASYNC_INTERRUPT',
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

    virtual int writev(const btes_Ovec  *buffers,
                       int               numBuffers,
                       int               flags = 0) = 0;
    virtual int writev(const btes_Iovec *buffers,
                       int               numBuffers,
                       int               flags = 0) = 0;
    virtual int writev(int             *augStatus,
                       const btes_Ovec *buffers,
                       int              numBuffers,
                       int              flags = 0) = 0;
    virtual int writev(int              *augStatus,
                       const btes_Iovec *buffers,
                       int               numBuffers,
                       int               flags = 0) = 0;
        // Write to this channel from the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers', the respective numbers of
        // bytes as specified in each corresponding 'btes_Ovec ' (or
        // 'btes_Iovec') buffer.  If the optionally specified 'flags'
        // incorporates 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous
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

    virtual int timedWritev(const btes_Ovec          *buffers,
                            int                       numBuffers,
                            const bdet_TimeInterval&  timeout,
                            int                       flags = 0) = 0;
    virtual int timedWritev(const btes_Iovec         *buffers,
                            int                       numBuffers,
                            const bdet_TimeInterval&  timeout,
                            int                       flags = 0) = 0;
    virtual int timedWritev(int                      *augStatus,
                            const btes_Ovec          *buffers,
                            int                       numBuffers,
                            const bdet_TimeInterval&  timeout,
                            int                       flags = 0) = 0;
    virtual int timedWritev(int                      *augStatus,
                            const btes_Iovec         *buffers,
                            int                       numBuffers,
                            const bdet_TimeInterval&  timeout,
                            int                       flags = 0) = 0;
        // Write to this channel from the specified sequence of 'buffers' of
        // specified sequence length 'numBuffers' the respective numbers of
        // bytes specified in each 'btes_Ovec' (or 'btes_Iovec') buffer, or
        // interrupted after the specified absolute 'timeout' time is reached.
        // If the optionally specified 'flags' incorporates
        // 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // on success, a negative value on error, and the number of bytes newly
        // written from 'buffer' (indicating a partial result) otherwise.  On a
        // partial result, load 'augStatus', if supplied, with 0 if 'timeout'
        // interrupted this operation or a positive value if the interruption
        // was due to an asynchronous event; otherwise, 'augStatus' is
        // unmodified.  A partial result typically does not invalidate this
        // channel; hence, this (or another) operation may be retried (with
        // arguments suitably adjusted) with some reasonable hope of success.
        // A negative "status", however, indicates a permanent error; -1
        // implies that the connection was closed by the peer (but the converse
        // is not guaranteed).  The behavior is undefined unless 0 < numBytes.
        // Note that if the specified 'timeout' value has already passed, the
        // "write" operation will still be attempted, but the attempt will not
        // block.

    virtual int writevRaw(const btes_Ovec  *buffers,
                          int               numBuffers,
                          int               flags = 0) = 0;
    virtual int writevRaw(const btes_Iovec *buffers,
                          int               numBuffers,
                          int               flags = 0) = 0;
    virtual int writevRaw(int              *augStatus,
                          const btes_Ovec  *buffers,
                          int               numBuffers,
                          int               flags = 0) = 0;
    virtual int writevRaw(int              *augStatus,
                          const btes_Iovec *buffers,
                          int               numBuffers,
                          int               flags = 0) = 0;
        // *Atomically* write to this channel, from the specified sequence of
        // 'buffers' of specified sequence length 'numBuffers', *at* *most* the
        // respective numbers of bytes as specified in each 'btes_Ovec' (or
        // 'btes_Iovec') buffer.  If the optionally specified 'flags'
        // incorporates 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Optionally specify (as a *leading* argument)
        // 'augStatus' to receive status specific to a "partial result".
        // Return 'numBytes' on success, a negative value on error, and the
        // number of bytes newly written from 'buffer' (indicating a partial
        // result) otherwise.  On a partial result, load 'augStatus', if
        // supplied, with a positive value if an asynchronous event interrupted
        // this operation and a negative value if the atomic OS-level operation
        // transmitted at least one byte, but less than 'numBytes'; otherwise,
        // 'augStatus' is unmodified.  A partial result typically does not
        // invalidate this channel; hence, this (or another) operation may be
        // retried (with arguments suitably adjusted) with some reasonable hope
        // of success.  A negative "status", however, indicates a permanent
        // error; -1 implies that the connection was closed by the peer (but
        // the converse is not guaranteed).  The behavior is undefined unless
        // 0 < numBytes.

    virtual int timedWritevRaw(const btes_Ovec          *buffers,
                               int                       numBuffers,
                               const bdet_TimeInterval&  timeout,
                               int                       flags = 0) = 0;
    virtual int timedWritevRaw(const btes_Iovec         *buffers,
                               int                       numBuffers,
                               const bdet_TimeInterval&  timeout,
                               int                       flags = 0) = 0;
    virtual int timedWritevRaw(int                      *augStatus,
                               const btes_Ovec          *buffers,
                               int                       numBuffers,
                               const bdet_TimeInterval&  timeout,
                               int                       flags = 0) = 0;
    virtual int timedWritevRaw(int                      *augStatus,
                               const btes_Iovec         *buffers,
                               int                       numBuffers,
                               const bdet_TimeInterval&  timeout,
                               int                       flags = 0) = 0;
        // *Atomically* write to this channel, from the specified sequence of
        // 'buffers' of specified sequence length 'numBuffers', *at* *most* the
        // respective numbers of bytes specified in each 'btes_Ovec' (or
        // 'btes_Iovec') buffer, or interrupted after the specified absolute
        // 'timeout' time is reached.  If the optionally specified 'flags'
        // incorporates 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous
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

    virtual void invalidate() = 0;
        // Make this channel invalid; no subsequent operations can be completed
        // successfully.

    // ACCESSORS
    virtual int isInvalid() const = 0;
        // Return 1 if this channel is invalid and 0 otherwise.  Note that once
        // a channel is invalid, no operations can be completed successfully.
        // Note also that 0 return value does NOT guarantee that a subsequent
        // I/O operation would not fail.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
