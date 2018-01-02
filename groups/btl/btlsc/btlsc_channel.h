// btlsc_channel.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSC_CHANNEL
#define INCLUDED_BTLSC_CHANNEL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for synchronous stream-based communication.
//
//@CLASSES:
//  btlsc::Channel: a synchronous stream-based channel protocol
//
//@SEE_ALSO: btlsc_timedchannel, btlsc_cbchannel, btlsc_channelallocator
//
//@DESCRIPTION: This component provides a class, 'btlsc::Channel', that defines
// an abstract interface for an end-point of a bi-directional synchronous
// (i.e., blocking) stream-based communication channel.  The 'btlsc::Channel'
// protocol supports efficient "buffered" transport and the syntax to enable
// efficient vector I/O operations (i.e., Unix-style scatter/gather 'readv' and
// 'writev').  Various forms of "partial transmission" authorizations (i.e.,
// "raw" OS-level atomic operations and interruptions due to an "asynchronous
// event") are also supported as appropriate.
//
///Protocol Hierarchy
///------------------
// 'btlsc::Channel' forms the base of an interface hierarchy; other interfaces
// may be defined by direct public inheritance:
//..
//                           ,--------------.
//                          ( btlsc::Channel )
//                           `--------------'
//..
//
///Synchronous Stream-Based Transport
///----------------------------------
// This interface establishes methods for synchronous stream-based transport.
// Each read or write method will block until one of three possible outcomes
// is reached: (1) "success" -- the specified number of bytes was transmitted,
// (2) "partial result" -- the operation was interrupted (e.g., via an
// asynchronous event), or (3) "error" -- an implementation-dependent error
// occurred.  In all cases, a "status" value is returned; an optional leading
// ('int *') 'augStatus' argument may be provided to enable the caller to
// distinguish among various reasons for a partial result (see below).  The
// user may retry a partial-result operation (with method arguments suitably
// adjusted), with a reasonable expectation of success.  Finally, concrete
// synchronous stream-based channels do a "best effort" in sending and
// receiving the specified data, but need not guarantee successful
// transmission.
//
///Buffered Transport
///------------------
// Non-vector read operations support a "buffered" variant that may be more
// efficient in some situations.  For such operations, the prefix "buffered"
// appears before the basic operation name in the full method name (e.g.,
// 'bufferedRead').  Note that, for blocking channels, there is no need for
// buffered write operations.
//
// In buffered operations, the caller does not provide a buffer, but rather
// receives direct (non-modifiable) access to the implementation's buffer.  In
// the event of a partial read (see below), the data remains buffered and
// subsequent reads will behave as if the buffered operation had never
// occurred.  Once a buffered read operation succeeds (i.e., receives the
// requested number of bytes) the buffered contents will remain valid only as
// long as the channel is not modified.  Note that "buffered" and "vector"
// ('readv', see below) are incompatible read options.
//
///Partial Results
///---------------
// The "simple" results of read and write operations are "success" (with a
// status equal to the requested number of bytes) and "error" (with a negative
// status).  More complex behavior is also supported, some of which is at the
// option of the user.  Specifically, the caller may authorize the possibility
// of another outcome via combinations of the following two mechanisms: (1) an
// interruption due to an asynchronous event and (2) a "raw" operation, i.e.,
// accepting the results of a single low-level (implementation dependent)
// atomic I/O operation (without retrying).  These two mechanisms (discussed
// in more detail below) may each return with a "partial result", indicated by
// a non-negative status that is less than the requested number of bytes.  Note
// that asynchronous events, but *not* raw operations, may result in a return
// status of zero bytes.
//
///Asynchronous Events
///- - - - - - - - - -
// Methods in this protocol anticipate the possible occurrence of an
// "asynchronous event" (AE) during execution.  A common example of an AE is a
// Unix signal, but note that a specific Unix signal, if not detected or
// implemented, *may* not result in an AE.
//
// This interface cannot fully specify either the nature of or the behavior
// resulting from an AE, but certain restrictions can be imposed.  By default,
// AEs are either ignored or, if that is not possible, cause an error.  At the
// user's option, however, a concrete implementation can be authorized to
// return, if such occurrence is detected, a "partial result" upon occurrence
// of an AE.  Such authorizations are made explicitly by incorporating into the
// optional (trailing) integer 'flags' argument to a method call the
// 'btlsc::Flag::k_ASYNC_INTERRUPT' value.
//
///Raw Transmissions
///- - - - - - - - -
// All read and unbuffered write methods support a "raw" variant in which the
// operation is allowed to return with a "partial result" if (1) *at* *least*
// *one* *byte* has been transmitted and (2) no additional bytes are
// *immediately* transmittable.  The "raw" mode is particularly useful to
// callers waiting for some read activity, who will then follow up with
// additional read requests after observing the initial transmission.  Raw
// transmissions are authorized by methods whose names end in the suffix 'Raw'.
// Note that the "raw" mode is not supported for buffered-write operations.
//
///'augStatus'
///- - - - - -
// Since there are several possible reasons for a "partial result", the caller
// may wish to know the specific cause.  A second status value, 'augStatus'
// ("augmented status") may be requested as an optional *initial* ('int *')
// argument to each transmission function.  If specified, 'augStatus' will be
// set to a positive value if the interruption was due to an "asynchronous
// event" and to a negative value if a "raw" operation could not complete.
// Note that 'augStatus' will not be modified on success or error, but only on
// a partial result.  Also note that a raw operation cannot terminate normally
// with less than 1 byte transmitted.  Finally, note that an 'augStatus' of
// zero is not possible for this protocol; the zero value is reserved for
// protocols supporting timeouts.
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
// hold the address of non-modifiable data to be written.  The following
// simple example shows how to create and populate an 'btls::Ovec' array in
// preparation for a 'writev' operation:
//..
//     void myWritevAndPrintStatusWhenAvailable(btlsc::Channel *channel)
//         // Write the integer representation of the length of a character
//         // string followed by the (null-terminated) string data itself to
//         // the specified 'channel'; upon completion, report the status
//         // of the "write" operation to 'cout'.
//     {
//         const char *const MESSAGE = "Hello World!";
//         const int         HEADER  = strlen(MESSAGE);
//         enum { NUM_BUFFERS = 2 };
//
//         btls::Ovec buffers[NUM_BUFFERS];
//
//         // Set each buffer's data and corresponding length.
//
//         buffer[0].setBuffer(&HEADER, sizeof HEADER);
//         buffer[1].setBuffer(MESSAGE, HEADER);
//
//         if (channel->writev(buffers, NUM_BUFFERS) < 0) {
//             bsl::cout << "Buffered write operation failed!" << bsl::endl;
//         }
//
//         // Notice that the 'writev' operation above does not
//         // authorize any partial write operations whatsoever.
//     }
//..
//
///Synopsis
///--------
// The following chart summarizes the set of 20 transmission methods that are
// available to read and write data from and to a 'btlsc::Channel'; note that
// "buffered readv" and all "buffered write (and writev)" operations are
// nonsensical and, therefore, omitted from the protocol:
//..
//    Buffered    Re/Wr    Vec    Raw    Method Name
//    --------    -----    ---    ---    --------------
//                READ                   read
//                READ            RAW    readRaw
//
//                READ     VEC           readv
//                READ     VEC    RAW    readvRaw
//
//    BUFFERED    READ                   bufferedRead
//    BUFFERED    READ            RAW    bufferedReadRaw
//
//                WRITE                  write
//                WRITE           RAW    writeRaw
//
//                WRITE    VEC           writev
//                WRITE    VEC    RAW    writevRaw
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
// client and server platforms:
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
//  int factorialClient(double *result, int input, btlsc::Channel *channel)
//      // Load into the specified 'result' the factorial of the specified
//      // 'input' using the specified 'channel' (which is assumed to be
//      // connected to an appropriate factorial service).  Return 0 on
//      // success, and -1, with no effect on 'result', on error.  The
//      // behavior is undefined unless '0 <= input'.
//  {
//      assert(0 <= input);
//
//      enum {
//          ERROR_STATUS   = -1,
//          SUCCESS_STATUS = 0
//      };
//      int numBytes = sizeof input;
//      int writeStatus = channel->write((const char *)&input, numBytes);
//      assert(0 != writeStatus);
//
//      if (writeStatus != numBytes) {
//          return ERROR_STATUS;                                      // RETURN
//      }
//
//      int readStatus = channel->read((char *)result, sizeof *result);
//      if (readStatus != sizeof *result) {
//          return ERROR_STATUS;                                      // RETURN
//      }
//
//      return SUCCESS_STATUS;                                        // RETURN
//  }
//
//  int factorialServer(btlsc::Channel *channel)
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
//          SUCCESS            =  0,
//          INVALID_INPUT      = -1,
//          ERROR_READ         = -2,
//          ERROR_WRITE        = -3,
//          ERROR_INTERRUPTED  = -4,
//          ERROR_UNCLASSIFIED = -5
//      };
//
//      while (1) {
//          int input, augStatus;
//          int readStatus = channel->read(&augStatus,
//                                         (char *)&input,
//                                         sizeof input);
//          if (readStatus < 0) {
//              return ERROR_READ;                                    // RETURN
//          }
//          else if (readStatus != sizeof input) {
//              if (augStatus > 0) {
//                  return ERROR_INTERRUPTED;                         // RETURN
//              }
//              return ERROR_UNCLASSIFIED;                            // RETURN
//          }
//
//          if (input < 0) {
//              return INVALID_INPUT;                                 // RETURN
//          }
//
//          double result = factorial(input);
//          augStatus = 0;
//          int writeStatus = channel->write(&augStatus,
//                                           (const char *)&result,
//                                           sizeof result);
//
//          if (writeStatus < 0) {
//              return ERROR_WRITE;                                   // RETURN
//          }
//          else if (writeStatus != sizeof input){
//              if (augStatus > 0) {
//                  return ERROR_INTERRUPTED;                         // RETURN
//              }
//              return ERROR_UNCLASSIFIED;                            // RETURN
//          }
//      }
//  }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLS_IOVEC
#include <btls_iovec.h>
#endif

namespace BloombergLP {
namespace btlsc {

                            // =============
                            // class Channel
                            // =============

class Channel {
    // This class defines a protocol (pure abstract interface) for a
    // synchronous communications channel that supports blocking read, write,
    // and buffered read operations on a byte stream.  In general, a
    // non-negative status indicates the number of bytes read or written,
    // while a negative status implies an unspecified error.  Note that an
    // error status of -1 indicates that the connection is *known* to have
    // been closed by the peer.  The reverse, however, is not true.

  public:
    // CREATORS
    virtual ~Channel();
        // Destroy this object.

    // MANIPULATORS
    virtual int read(char *buffer,
                     int   numBytes,
                     int   flags = 0) = 0;

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

    virtual int readv(const btls::Iovec *buffers,
                      int                numBuffers,
                      int                flags = 0) = 0;

    virtual int readv(int               *augStatus,
                      const btls::Iovec *buffers,
                      int                numBuffers,
                      int                flags = 0) = 0;
        // Read from this channel into the specified sequence of 'buffers' of
        // the specified sequence length 'numBuffers' the respective number of
        // bytes as specified in each 'btls::Iovec' buffer.  If the optionally
        // specified 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Optionally specify (as a
        // *leading* argument) 'augStatus' to receive status specific to a
        // partial result.  Return 'numBytes' (i.e., the sum of calls to
        // 'length' on the 'numBuffers' 'buffers') on success, a negative value
        // on error, and the number of bytes newly read into 'buffers'
        // (indicating a partial result) otherwise.  On a partial result, load
        // 'augStatus', if supplied, with a positive value, indicating that an
        // asynchronous event caused the interruption; otherwise, 'augStatus'
        // is unmodified.  A partial result typically does not invalidate this
        // channel; hence, this (or another) operation may be retried (with
        // arguments suitably adjusted) with some reasonable hope of success.
        // A negative "status", however, indicates a permanent error (leaving
        // the contents of 'buffers' undefined); -1 implies that the connection
        // was closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless 'buffers' have sufficient capacity to
        // hold the requested data and '0 < numBytes'.

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

    virtual int readvRaw(const btls::Iovec *buffers,
                         int                numBuffers,
                         int                flags = 0) = 0;

    virtual int readvRaw(int               *augStatus,
                         const btls::Iovec *buffers,
                         int                numBuffers,
                         int                flags = 0) = 0;
        // *Atomically* read from this channel into the specified sequence of
        // 'buffers' of the specified sequence length 'numBuffers' *at* *most*
        // the number of bytes as the sum of length in each 'btls::Iovec'
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
        // guarantees that 'buffer' will remain valid until this channel is
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
        // on success, a negative value on error, and the number of bytes
        // (indicating a partial result) otherwise.  Any positive return value
        // guarantees that 'buffer' will remain valid until this channel is
        // modified.  On a partial result, load 'augStatus', if supplied, with
        // a positive value if an asynchronous event interrupted this
        // operation, or a negative value if the atomic OS-level operation
        // transmitted at least one but less than 'numBytes'; otherwise,
        // 'augStatus' is unmodified.  A partial result typically does not
        // invalidate this channel; hence, this (or another) operation may be
        // retried with some reasonable hope of success -- buffered data from a
        // partial result remains available until consumed by subsequent read
        // operations.  A negative "status", however, indicates a permanent
        // error (leaving 'buffer' unset); -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless '0 < numBytes'.

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

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    virtual int writev(const btls::Ovec *buffers,
                       int               numBuffers,
                       int               flags = 0) = 0;

    virtual int writev(const btls::Iovec *buffers,
                       int                numBuffers,
                       int                flags = 0) = 0;

    virtual int writev(int              *augStatus,
                       const btls::Ovec *buffers,
                       int               numBuffers,
                       int               flags = 0) = 0;

    virtual int writev(int               *augStatus,
                       const btls::Iovec *buffers,
                       int                numBuffers,
                       int                flags = 0) = 0;
        // Write to this channel from the specified sequence of 'buffers' of
        // the specified sequence length 'numBuffers' the respective number of
        // bytes as specified in each 'btls::Ovec' (or 'btls::Iovec') buffer.
        // If the optionally specified 'flags' incorporates
        // 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events" are
        // permitted to interrupt this operation; by default, such events are
        // ignored.  Optionally specify (as a *leading* argument) 'augStatus'
        // to receive status specific to a partial result.  Return 'numBytes'
        // (i.e., the sum of calls to 'length' on the 'numBuffers' 'buffers')
        // on success, a negative value on error, and the number of bytes newly
        // written from 'buffers' (indicating a partial result) otherwise.  On
        // a partial result, load 'augStatus', if supplied, with a positive
        // value, indicating that an asynchronous event caused the
        // interruption; otherwise, 'augStatus' is unmodified.  A partial
        // result typically does not invalidate this channel; hence, this (or
        // another) operation may be retried (with arguments suitably adjusted)
        // with some reasonable hope of success.  A negative "status", however,
        // indicates a permanent error; -1 implies that the connection was
        // closed by the peer (but the converse is not guaranteed).  The
        // behavior is undefined unless '0 < numBytes'.

    virtual int writevRaw(const btls::Ovec *buffers,
                          int               numBuffers,
                          int               flags = 0) = 0;

    virtual int writevRaw(const btls::Iovec *buffers,
                          int                numBuffers,
                          int                flags = 0) = 0;

    virtual int writevRaw(int              *augStatus,
                          const btls::Ovec *buffers,
                          int               numBuffers,
                          int               flags = 0) = 0;

    virtual int writevRaw(int               *augStatus,
                          const btls::Iovec *buffers,
                          int                numBuffers,
                          int                flags = 0) = 0;
        // *Atomically* write to this channel, from the specified sequence of
        // 'buffers' of the specified sequence length 'numBuffers', *at* *most*
        // the number of bytes as the sum of the specified length in each
        // 'btls::Ovec' (or 'btls::Iovec') buffer.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Optionally specify (as a *leading* argument)
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
