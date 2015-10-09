// btlsc_timedchannel.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSC_TIMEDCHANNEL
#define INCLUDED_BTLSC_TIMEDCHANNEL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for stream-based communication with a timeout.
//
//@CLASSES:
//  btlsc::TimedChannel: synchronous stream-based channel protocol with timeout
//
//@SEE_ALSO: btlsc_timedcbchannel, btlsc_timedchannelallocator
//
//@DESCRIPTION: This component provides a class, 'btlsc::TimedChannel', that
// defines an abstract interface for an end-point of a bi-directional
// synchronous (i.e., blocking) stream-based communication channel with timeout
// capability.  The 'btlsc::TimedChannel' protocol supports efficient
// "buffered" transport and the syntax to enable efficient vector I/O
// operations (i.e., Unix-style scatter/gather "readv" and "writev").  Various
// forms of "partial transmission" authorizations (i.e., "raw" OS-level atomic
// operations and interruptions due to a user-specified "timeout" or an
// "asynchronous event") are also supported as appropriate.
//
///Protocol Hierarchy
///------------------
// The interface hierarchy (defined by direct public inheritance) of the
// 'btlsc::TimedChannel' protocol is as follows:
//..
//                        ,-------------------.
//                       ( btlsc::TimedChannel )
//                        `-------------------'
//                                  |
//                                  V
//                           ,--------------.
//                          ( btlsc::Channel )
//                           `--------------'
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
// "status" value is returned; an optional leading ('int *') 'augStatus'
// argument may be provided to enable the caller to distinguish among various
// reasons for a partial result (see below).  The user may retry a
// partial-result operation (with method arguments suitably adjusted), with a
// reasonable expectation of success.  Finally, concrete blocking stream-based
// channels do a "best effort" in sending and receiving the specified data, but
// need not guarantee successful transmission.
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
// method call the 'btlsc::Flag::k_ASYNC_INTERRUPT' value.
//
///Timeouts
/// - - - -
// A timeout is registered by the caller, when a method is invoked, as a
// 'bsls::TimeInterval' value indicating the absolute *system* time after which
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
/// - - - - - - - - - - - - - - - - - - - - - - - - -
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
///'augStatus'
///- - - - - -
// Since there are several possible reasons for a "partial result", the caller
// may wish to know the specific cause.  A second status value, 'augStatus'
// ("augmented status") may be requested as an optional *initial* ('int *')
// argument to each transmission function.  If specified, 'augStatus' will be
// set to 0 if a user-supplied timeout interrupted the operation, and to a
// positive value if the interruption was due to an "asynchronous event".  Note
// that 'augStatus' will not be modified on success or error, but only on a
// partial result.
//
///Scatter/Gather ('readv'/'writev')
///---------------------------------
// This interface supports "vector I/O" -- the simultaneous reading from or
// writing to multiple buffers -- via Unix-style 'readv' and 'writev' variants
// of the normal single-buffer methods.  Scatter/Gather operations use either
// the 'btls::Iovec' or 'btls::Ovec' objects which use a 'iovec' 'struct' on
// Unix platforms or a 'WSABUF' 'struct' on Windows.  In either structure, the
// total number of bytes to be read or written is determined by the sum of each
// buffer of the non-negative 'numBuffers'.  Note that the 'btls::Ovec' variant
// enables write operations to avoid having to cast away 'const' in order to
// hold the address of non-modifiable data to be written.  The following simple
// example shows how to create and populate a 'btls::Ovec' array in
// preparation for a 'writev' operation:
//..
//  static void myPrintWriteStatus(int status, int augStatus, int numBytes)
//      // Print to the user console the result of a attempting to write
//      // the specified 'numBytes' based on the specified write 'status'
//      // and the auxiliary 'augStatus' (discussed below).  The behavior
//      // is undefined unless '0 < numBytes' and 'status <= numBytes'.  Note
//      // that 'augStatus' is ignored unless '0 <= status < numBytes'.
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
//             assert(-1 > status);
//             bsl::cout << "Write failed: the reason is unknown."
//                       << bsl::endl;
//         }
//     }
//
//     void myWritevAndPrintStatusWhenAvailable(btlsc::TimedChannel *channel)
//         // Write the integer representation of the length of a character
//         // string followed by the (null-terminated) string data itself to
//         // the specified 'channel'; upon completion, report the status
//         // of the "write" operation to 'cout'.
//     {
//         const char *const MESSAGE     = "Hello World!";
//         const int         HEADER      = strlen(MESSAGE);
//         const int         NUM_BUFFERS = 2;
//
//         btls::Ovec buffers[NUM_BUFFERS];
//
//         // Set each buffer's data and corresponding length.
//
//         buffer[0].setBuffer(&HEADER, sizeof HEADER);
//         buffer[1].setBuffer(MESSAGE, HEADER);
//
//         if (0 > channel->writev(buffers, NUM_BUFFERS)) {
//             bsl::cout << "Buffered write operation failed!" << bsl::endl;
//         }
//
//         // Notice that the 'writev' operation above does not
//         // authorize any partial write operations whatsoever.
//    }
//..
//
///Synopsis
///--------
// The following chart summarizes the set of 20 transmission methods that are
// available to read and write data from and to a 'btlsc::TimedChannel'; note
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
// Each of these methods supports the specification of a flag value:
//..
//  btlsc::Flag::k_ASYNC_INTERRUPT
//..
// supplied in an optional trailing integer to enable "asynchronous events" to
// cause partial results; by default, such events are ignored.
//
// Each of these methods is overloaded to allow the caller to optionally
// specify the address of an 'augStatus', which will then be modified in the
// event of a partial result.
//
///Usage
///-----
// The 'btlsc' style of channel interface is used to transmit sequences of
// specified size across some concrete channel implementation.  In this example
// we demonstrate how to implement a remote procedure call (RPC) to a factorial
// function taking an 'int' and returning a 'double'.  For simplicity, we will
// assume that both the 'int' and 'double' formats are binary compatible across
// client and server platforms.
//..
//  double factorial(int number)
//      // Return the factorial of the specified integral 'number' as a value
//      // of type 'double'.  The behavior is undefined unless '0 <= number'.
//      // Note that this helper function is provided for the server to
//      // calculate the factorial value.
//  {
//      if (0 == number) {
//          return 1;                                                 // RETURN
//      }
//      else {
//          return number * factorial(number - 1);                    // RETURN
//      }
//  }
//
//  int factorialClient(double              *result,
//                      int                  input,
//                      btlsc::TimedChannel *channel)
//      // Load into the specified 'result' the factorial of the specified
//      // 'input' using the specified 'channel' (which is assumed to be
//      // connected to an appropriate factorial service).  Return 0 on
//      // success, and -1, with no effect on 'result', on error.  The behavior
//      // is undefined unless '0 <= input'.
//  {
//      ASSERT(0 <= input);
//
//      enum {
//          k_ERROR_STATUS   = -1,
//          k_SUCCESS_STATUS =  0,
//          k_TIMEOUT_STATUS =  1
//      };
//
//      enum {
//          k_WRITE_TIMEOUT = 1,     // seconds
//          k_READ_TIMEOUT  = 5      // seconds
//      };
//      bsls::TimeInterval timeNow = bdlt::CurrentTime::now();
//      bsls::TimeInterval writeTimeout(timeNow + k_WRITE_TIMEOUT);
//      bsls::TimeInterval readTimeout(timeNow + k_READ_TIMEOUT);
//
//      int numBytes = sizeof input;
//      int writeStatus = channel->timedWrite((const char *)&input,
//                                             numBytes,
//                                             writeTimeout);
//      ASSERT(0 != writeStatus);
//
//      if (writeStatus != numBytes) {
//          return k_ERROR_STATUS;                                    // RETURN
//      }
//
//      int readStatus = channel->timedRead((char *)result,
//                                          sizeof *result,
//                                          readTimeout);
//      if (readStatus != sizeof *result) {
//          return k_ERROR_STATUS;                                    // RETURN
//      }
//
//      return k_SUCCESS_STATUS;                                      // RETURN
//  }
//
//  int factorialServer(btlsc::TimedChannel *channel)
//      // Repeatedly read integer sequences from the specified 'channel'.
//      // When a read succeeds, interpret the byte sequence as an integer
//      // value in host-byte order.  Return -1 if that value is negative.
//      // Otherwise, calculate the factorial of the (non-negative) integer and
//      // write back the result to 'channel' as a sequence of bytes
//      // representing a 'double' in the host's native format.  Return a
//      // negative value if any write operation doesn't succeed (refer to the
//      // following 'enum' values for specific errors).  Note that this
//      // implementation is just to show how a channel could be used; there is
//      // much room to improve.
//  {
//      enum {
//          k_SUCCESS            =  0,
//          k_INVALID_INPUT      = -1,
//          k_ERROR_READ         = -2,
//          k_ERROR_WRITE        = -3,
//          k_ERROR_TIMEOUT      = -4,
//          k_ERROR_INTERRUPTED  = -5,
//          k_ERROR_UNCLASSIFIED = -6
//      };
//
//      enum {
//          k_READ_TIME  = 3600, // an hour
//          k_WRITE_TIME = 2     // 2 seconds
//      };
//      bsls::TimeInterval timeNow = bdlt::CurrentTime::now();
//      const bsls::TimeInterval k_READ_TIMEOUT(timeNow + k_READ_TIME),
//                              k_WRITE_TIMEOUT(timeNow + k_WRITE_TIME);
//
//      while (1) {
//          int input, augStatus;
//          int readStatus = channel->timedRead(&augStatus,
//                                              (char *)&input,
//                                              sizeof input,
//                                              k_READ_TIMEOUT);
//          if (readStatus < 0) {
//              return k_ERROR_READ;                                  // RETURN
//          }
//          if (readStatus != sizeof input) {
//              if (0 == augStatus) {
//                  return k_ERROR_TIMEOUT;                           // RETURN
//              }
//              else if (augStatus > 0) {
//                  return k_ERROR_INTERRUPTED;                       // RETURN
//              }
//              return k_ERROR_UNCLASSIFIED;                          // RETURN
//          }
//          if (input < 0) {
//              return k_INVALID_INPUT;                               // RETURN
//          }
//
//          augStatus = 0;
//          double result = factorial(input);
//          int writeStatus = channel->timedWrite(&augStatus,
//                                                (const char *)&result,
//                                                sizeof input,
//                                                k_WRITE_TIMEOUT);
//          if (writeStatus < 0) {
//              return k_ERROR_WRITE;                                 // RETURN
//          }
//          else if (writeStatus != sizeof input){
//              if (0 == augStatus) {
//                  return k_ERROR_TIMEOUT;                           // RETURN
//              }
//              else if (augStatus > 0) {
//                  return k_ERROR_INTERRUPTED;                       // RETURN
//              }
//              return k_ERROR_UNCLASSIFIED;                          // RETURN
//          }
//      }
//  }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSC_CHANNEL
#include <btlsc_channel.h>
#endif

#ifndef INCLUDED_BTLS_IOVEC
#include <btls_iovec.h>
#endif

namespace BloombergLP {

namespace bsls { class TimeInterval; }

namespace btlsc {

                             // ==================
                             // class TimedChannel
                             // ==================

class TimedChannel : public Channel {
    // This class defines a protocol (pure abstract interface) for a
    // synchronous communication channel that supports timed (blocking) read,
    // write, and buffered read operations on a byte stream.  In general, a
    // non-negative status indicates the number of bytes read or written, while
    // a negative status implies an unspecified error.  Note that an error
    // status of -1 indicates that the connection is *known* to have been
    // closed by the peer.

  public:
    // CREATORS
    virtual ~TimedChannel();
        // Destroy this object.

    // MANIPULATORS
    virtual int read(char *buffer, int numBytes, int flags = 0) = 0;
    virtual int read(int  *augStatus,
                     char *buffer,
                     int   numBytes,
                     int   flags = 0) = 0;
        // Read from this channel into the specified 'buffer' the specified
        // 'numBytes'.  If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
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

    virtual int timedRead(char                      *buffer,
                          int                        numBytes,
                          const bsls::TimeInterval&  timeout,
                          int                        flags = 0) = 0;
    virtual int timedRead(int                       *augStatus,
                          char                      *buffer,
                          int                        numBytes,
                          const bsls::TimeInterval&  timeout,
                          int                        flags = 0) = 0;
        // Read from this channel into the specified 'buffer' the specified
        // 'numBytes' or interrupt after the specified absolute 'timeout' time
        // is reached.  If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
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
        // hold the requested data and '0 < numBytes'.  Note that if the
        // 'timeout' value has already passed, the "read" operation will still
        // be attempted, but the attempt will not block.

    virtual int readv(const btls::Iovec *buffers,
                      int                numBuffers,
                      int                flags = 0) = 0;
    virtual int readv(int               *augStatus,
                      const btls::Iovec *buffers,
                      int                numBuffers,
                      int                flags = 0) = 0;
        // Read from this channel into the specified sequence of 'buffers' of
        // the specified 'numBuffers', the respective number of bytes as
        // specified in each corresponding 'btls::Iovec' buffer.  If the
        // optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a partial result.  Return 'numBytes'
        // (i.e., the sum of calls to 'length' on the 'numBuffers' 'buffers')
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
        // and '0 < numBytes'.

    virtual int timedReadv(const btls::Iovec         *buffers,
                           int                        numBuffers,
                           const bsls::TimeInterval&  timeout,
                           int                        flags = 0) = 0;
    virtual int timedReadv(int                       *augStatus,
                           const btls::Iovec         *buffers,
                           int                        numBuffers,
                           const bsls::TimeInterval&  timeout,
                           int                        flags = 0) = 0;
        // Read from this channel into the specified sequence of 'buffers' of
        // the specified 'numBuffers', the respective number of bytes as
        // specified in each corresponding 'btls::Iovec' buffer, or interrupted
        // after the specified absolute 'timeout' time is reached.  If the
        // optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // (i.e., the sum of calls to 'length' on the 'numBuffers' 'buffers')
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
        // hold the requested data and '0 < numBytes'.  Note that if the
        // 'timeout' value has already passed, the "read" operation will still
        // be attempted, but the attempt will not block.

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
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT',
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
        // and '0 < numBytes'.

    virtual int timedReadRaw(char                      *buffer,
                             int                        numBytes,
                             const bsls::TimeInterval&  timeout,
                             int                        flags = 0) = 0;
    virtual int timedReadRaw(int                       *augStatus,
                             char                      *buffer,
                             int                        numBytes,
                             const bsls::TimeInterval&  timeout,
                             int                        flags = 0) = 0;
        // *Atomically* read from this channel into the specified 'buffer' *at*
        // *most* the specified 'numBytes' or interrupt after the specified
        // absolute 'timeout' time is reached.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT',
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
        // hold the requested data and '0 < numBytes'.  Note that if the
        // 'timeout' value has already passed, the "read" operation will still
        // be attempted, but the attempt will not block.

    virtual int readvRaw(const btls::Iovec *buffers,
                         int                numBuffers,
                         int                flags = 0) = 0;
    virtual int readvRaw(int               *augStatus,
                         const btls::Iovec *buffers,
                         int                numBuffers,
                         int                flags = 0) = 0;
        // *Atomically* read from this channel into the specified sequence of
        // 'buffers' of the specified 'numBuffers', *at* *most* the respective
        // number of bytes as specified in each corresponding 'btls::Iovec'
        // buffer.  If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // (i.e., the sum of calls to 'length' on the 'numBuffers' 'buffers')
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
        // hold the requested data and '0 < numBytes'.

    virtual int timedReadvRaw(int                       *augStatus,
                              const btls::Iovec         *buffers,
                              int                        numBuffers,
                              const bsls::TimeInterval&  timeout,
                              int                        flags = 0) = 0;
    virtual int timedReadvRaw(const btls::Iovec         *buffers,
                              int                        numBuffers,
                              const bsls::TimeInterval&  timeout,
                              int                        flags = 0) = 0;
        // *Atomically* read from this channel into the specified sequence of
        // 'buffers' of the specified 'numBuffers' *at* *most* the respective
        // number of bytes as specified in each corresponding 'btls::Iovec'
        // buffer, or interrupted after the specified absolute 'timeout' time
        // is reached.  If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // (i.e., the sum of calls to 'length' on the 'numBuffers' 'buffers')
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
        // and '0 < numBytes'.  Note that if the 'timeout' value has already
        // passed, the "read" operation will still be attempted, but the
        // attempt will not block.

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
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
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
        // '0 < numBytes'.

    virtual int timedBufferedRead(const char                **buffer,
                                  int                         numBytes,
                                  const bsls::TimeInterval&   timeout,
                                  int                         flags = 0) = 0;
    virtual int timedBufferedRead(int                        *augStatus,
                                  const char                **buffer,
                                  int                         numBytes,
                                  const bsls::TimeInterval&   timeout,
                                  int                         flags = 0) = 0;
        // Read from this channel into a channel-supplied buffer, identified
        // via the specified 'buffer', the specified 'numBytes', or interrupt
        // after the specified absolute 'timeout' time is reached.  If the
        // optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
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
        // behavior is undefined unless '0 < numBytes'.  Note that if the
        // 'timeout' value has already passed, the "read" operation will still
        // be attempted, but the attempt will not block.

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
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
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
        // guaranteed).  The behavior is undefined unless '0 < numBytes'.

    virtual int timedBufferedReadRaw(
                                    const char                **buffer,
                                    int                         numBytes,
                                    const bsls::TimeInterval&   timeout,
                                    int                         flags = 0) = 0;
    virtual int timedBufferedReadRaw(
                                    int                        *augStatus,
                                    const char                **buffer,
                                    int                         numBytes,
                                    const bsls::TimeInterval&   timeout,
                                    int                         flags = 0) = 0;
        // *Atomically* read from this channel into a channel-supplied buffer,
        // identified via the specified 'buffer', *at* *most* the specified
        // 'numBytes' or interrupt after the specified absolute 'timeout' time
        // is reached.  If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
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
        // guaranteed).  The behavior is undefined unless '0 < numBytes'.  Note
        // that if the 'timeout' value has already passed, the "read" operation
        // will still be attempted, but the attempt will not block.

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
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
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

    virtual int timedWrite(int                       *augStatus,
                           const char                *buffer,
                           int                        numBytes,
                           const bsls::TimeInterval&  timeout,
                           int                        flags = 0) = 0;
    virtual int timedWrite(const char                *buffer,
                           int                        numBytes,
                           const bsls::TimeInterval&  timeout,
                           int                        flags = 0) = 0;
        // Write to this channel from the specified 'buffer' the specified
        // 'numBytes' or interrupt after the specified absolute 'timeout' time
        // is reached.  If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
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
        // undefined unless '0 < numBytes'.  Note that if the 'timeout' value
        // has already passed, the "write" operation will still be attempted,
        // but the attempt will not block.

    virtual int writeRaw(const char *buffer,
                         int         numBytes,
                         int         flags = 0) = 0;
    virtual int writeRaw(int        *augStatus,
                         const char *buffer,
                         int         numBytes,
                         int         flags = 0) = 0;
        // *Atomically* write to this channel from the specified 'buffer' *at*
        // *most* the specified 'numBytes'.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT',
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
        // behavior is undefined unless '0 < numBytes'.

    virtual int timedWriteRaw(int                       *augStatus,
                              const char                *buffer,
                              int                        numBytes,
                              const bsls::TimeInterval&  timeout,
                              int                        flags = 0) = 0;
    virtual int timedWriteRaw(const char                *buffer,
                              int                        numBytes,
                              const bsls::TimeInterval&  timeout,
                              int                        flags = 0) = 0;
        // *Atomically* write to this channel from the specified 'buffer' *at*
        // *most* the specified 'numBytes', or interrupt after the specified
        // absolute 'timeout' time is reached.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT',
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
        // '0 < numBytes'.  Note that if the 'timeout' value has already
        // passed, the "write" operation will still be attempted, but the
        // attempt will not block.

    virtual int writev(const btls::Ovec  *buffers,
                       int                numBuffers,
                       int                flags = 0) = 0;
    virtual int writev(const btls::Iovec *buffers,
                       int                numBuffers,
                       int                flags = 0) = 0;
    virtual int writev(int               *augStatus,
                       const btls::Ovec  *buffers,
                       int                numBuffers,
                       int                flags = 0) = 0;
    virtual int writev(int               *augStatus,
                       const btls::Iovec *buffers,
                       int                numBuffers,
                       int                flags = 0) = 0;
        // Write to this channel from the specified sequence of 'buffers' of
        // the specified sequence length 'numBuffers', the respective number of
        // bytes as specified in each corresponding 'btls::Ovec' (or
        // 'btls::Iovec') buffer.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events"
        // are permitted to interrupt this operation; by default, such events
        // are ignored.  Optionally specify (as a *leading* argument)
        // 'augStatus' to receive status specific to a partial result.  Return
        // 'numBytes' (i.e., the sum of calls to 'length' on the 'numBuffers'
        // 'buffers') on success, a negative value on error, and the number of
        // bytes newly written from 'buffers' (indicating a partial result)
        // otherwise.  On a partial result, load 'augStatus', if supplied, with
        // a positive value, indicating that an asynchronous event caused the
        // interruption; otherwise, 'augStatus' is unmodified.  A partial
        // result typically does not invalidate this channel; hence, this (or
        // another) operation may be retried (with arguments suitably adjusted)
        // with some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error; -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless '0 < numBytes'.

    virtual int timedWritev(const btls::Ovec          *buffers,
                            int                        numBuffers,
                            const bsls::TimeInterval&  timeout,
                            int                        flags = 0) = 0;
    virtual int timedWritev(const btls::Iovec         *buffers,
                            int                        numBuffers,
                            const bsls::TimeInterval&  timeout,
                            int                        flags = 0) = 0;
    virtual int timedWritev(int                       *augStatus,
                            const btls::Ovec          *buffers,
                            int                        numBuffers,
                            const bsls::TimeInterval&  timeout,
                            int                        flags = 0) = 0;
    virtual int timedWritev(int                       *augStatus,
                            const btls::Iovec         *buffers,
                            int                        numBuffers,
                            const bsls::TimeInterval&  timeout,
                            int                        flags = 0) = 0;
        // Write to this channel from the specified sequence of 'buffers' of
        // the specified sequence length 'numBuffers' the respective number of
        // bytes specified in each 'btls::Ovec' (or 'btls::Iovec') buffer, or
        // interrupted after the specified absolute 'timeout' time is reached.
        // If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a "partial result".  Return 'numBytes'
        // (i.e., the sum of calls to 'length' on the 'numBuffers' 'buffers')
        // on success, a negative value on error, and the number of bytes newly
        // written from 'buffers' (indicating a partial result) otherwise.  On
        // a partial result, load 'augStatus', if supplied, with 0 if 'timeout'
        // interrupted this operation or a positive value if the interruption
        // was due to an asynchronous event; otherwise, 'augStatus' is
        // unmodified.  A partial result typically does not invalidate this
        // channel; hence, this (or another) operation may be retried (with
        // arguments suitably adjusted) with some reasonable hope of success.
        // A negative "status", however, indicates a permanent error; -1
        // implies that the connection was closed by the peer (but the converse
        // is not guaranteed).  The behavior is undefined unless
        // '0 < numBytes'.  Note that if the 'timeout' value has already
        // passed, the "write" operation will still be attempted, but the
        // attempt will not block.

    virtual int writevRaw(const btls::Ovec  *buffers,
                          int                numBuffers,
                          int                flags = 0) = 0;
    virtual int writevRaw(const btls::Iovec *buffers,
                          int                numBuffers,
                          int                flags = 0) = 0;
    virtual int writevRaw(int               *augStatus,
                          const btls::Ovec  *buffers,
                          int                numBuffers,
                          int                flags = 0) = 0;
    virtual int writevRaw(int               *augStatus,
                          const btls::Iovec *buffers,
                          int                numBuffers,
                          int                flags = 0) = 0;
        // *Atomically* write to this channel, from the specified sequence of
        // 'buffers' of the specified sequence length 'numBuffers', *at* *most*
        // the respective number of bytes as specified in each 'btls::Ovec' (or
        // 'btls::Iovec') buffer.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events"
        // are permitted to interrupt this operation; by default, such events
        // are ignored.  Optionally specify (as a *leading* argument)
        // 'augStatus' to receive status specific to a "partial result".
        // Return 'numBytes' (i.e., the sum of calls to 'length' on the
        // 'numBuffers' 'buffers') on success, a negative value on error, and
        // the number of bytes newly written from 'buffers' (indicating a
        // partial result) otherwise.  On a partial result, load 'augStatus',
        // if supplied, with a positive value if an asynchronous event
        // interrupted this operation and a negative value if the atomic
        // OS-level operation transmitted at least one byte, but less than
        // 'numBytes'; otherwise, 'augStatus' is unmodified.  A partial result
        // typically does not invalidate this channel; hence, this (or another)
        // operation may be retried (with arguments suitably adjusted) with
        // some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error; -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless '0 < numBytes'.

    virtual int timedWritevRaw(const btls::Ovec          *buffers,
                               int                        numBuffers,
                               const bsls::TimeInterval&  timeout,
                               int                        flags = 0) = 0;
    virtual int timedWritevRaw(const btls::Iovec         *buffers,
                               int                        numBuffers,
                               const bsls::TimeInterval&  timeout,
                               int                        flags = 0) = 0;
    virtual int timedWritevRaw(int                       *augStatus,
                               const btls::Ovec          *buffers,
                               int                        numBuffers,
                               const bsls::TimeInterval&  timeout,
                               int                        flags = 0) = 0;
    virtual int timedWritevRaw(int                       *augStatus,
                               const btls::Iovec         *buffers,
                               int                        numBuffers,
                               const bsls::TimeInterval&  timeout,
                               int                        flags = 0) = 0;
        // *Atomically* write to this channel, from the specified sequence of
        // 'buffers' of the specified sequence length 'numBuffers', *at* *most*
        // the respective number of bytes specified in each 'btls::Ovec' (or
        // 'btls::Iovec') buffer, or interrupted after the specified absolute
        // 'timeout' time is reached.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events"
        // are permitted to interrupt this operation; by default, such events
        // are ignored.  Optionally specify (as a *leading* argument)
        // 'augStatus' to receive status specific to a "partial result".
        // Return 'numBytes' (i.e., the sum of calls to 'length' on the
        // 'numBuffers' 'buffers') on success, a negative value on error, and
        // the number of bytes newly written from 'buffers' (indicating a
        // partial result) otherwise.  On a partial result, load 'augStatus',
        // if supplied, with 0 if 'timeout' interrupted this operation, a
        // positive value if an asynchronous event caused an interruption, or a
        // negative value if the atomic OS-level operation transmitted at least
        // one byte, but less than 'numBytes'; otherwise, 'augStatus' is
        // unmodified.  A partial result typically does not invalidate this
        // channel; hence, this (or another) operation may be retried (with
        // arguments suitably adjusted) with some reasonable hope of success.
        // A negative "status", however, indicates a permanent error; -1
        // implies that the connection was closed by the peer (but the converse
        // is not guaranteed).  The behavior is undefined unless
        // '0 < numBytes'.  Note that if the 'timeout' value has already
        // passed, the "write" operation will still be attempted, but the
        // attempt will not block.

    virtual void invalidate() = 0;
        // Make this channel invalid; no subsequent operations can be completed
        // successfully.

    // ACCESSORS
    virtual int isInvalid() const = 0;
        // Return 1 if this channel is invalid, and 0 otherwise.  Note that
        // once a channel is invalid, no operations can be completed
        // successfully.  Also note that a 0 return value does NOT guarantee
        // that a subsequent I/O operation would not fail.
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
