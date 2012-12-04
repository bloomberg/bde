// btemt_asyncchannel.h                                               -*-C++-*-
#ifndef INCLUDED_BTEMT_ASYNCCHANNEL
#define INCLUDED_BTEMT_ASYNCCHANNEL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for asynchronous IO operations
//
//@CLASSES:
//  btemt_AsyncChannel: *pure* protocol for asynchronous IO operations
//
//@SEE_ALSO: btemt_channelpoolchannel, btemt_sessionpool
//
//@AUTHOR: Ilougino Rocha (irocha), Olga Zykova (ozykova)
//
//@DESCRIPTION: The class 'btemt_AsyncChannel' provides the interface for
// asynchronous read and write operations over TCP/IP communication channel.
// This class is intended to be used with 'btemt_sessionpool'.  For a usage
// example, see 'btemt_sessionpool'.  For a concrete implementation over BTE
// channels, see 'btemt_channelpoolchannel'.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

namespace BloombergLP {

class bdet_TimeInterval;
class bcema_Blob;
class btemt_BlobMsg;
class btemt_DataMsg;

                          // ========================
                          // class btemt_AsyncChannel
                          // ========================

class btemt_AsyncChannel {
    // This class defines a *pure* protocol for asynchronous IO operations.

  public:
    // PUBLIC TYPES
    enum ReadResult{
        // result codes passed in read callback

          BTEMT_SUCCESS  = 0  // The operation was successful.

        , BTEMT_TIMEOUT  = 1  // The 'timedRead' operation has timed out.

        , BTEMT_CLOSED   = 2  // The underlying stream used by this channel
                              // has been closed.

        , BTEMT_FAILURE  = 3  // An I/O error occurred during the read
                              // operation.

        , BTEMT_CANCELED = 4  // The asynchronous read request was canceled.
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        , SUCCESS  = BTEMT_SUCCESS
        , TIMEOUT  = BTEMT_TIMEOUT
        , CLOSED   = BTEMT_CLOSED
        , FAILURE  = BTEMT_FAILURE
        , CANCELED = BTEMT_CANCELED
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    typedef bdef_Function<void (*)(int ,int*, int*, const btemt_DataMsg&)>
                                                                  ReadCallback;
        // A callback of this type is invoked for 'read' and 'timeRead' when
        // either the requested number of bytes are available, or an error
        // occurs.  The callback is invoked with four arguments: (1) an integer
        // result code corresponding to 'ReadResult' that indicates the result
        // of the read operation, and if this code is equal to 'BTEMT_SUCCESS',
        // (2) an integer pointer where the callee can indicate the number of
        // bytes that have been consumed from the message, (3) a pointer to an
        // integer value where the callee can indicate how many more bytes are
        // needed to complete the read operation, (4) a 'btemt_DataMsg' object
        // containing the payload.  Note that the read operation is not
        // considered completed until the callee indicates that zero more bytes
        // are needed (argument 3).  Also note that the last three arguments
        // are ignored if the first argument is different from 'BTEMT_SUCCESS'.
        //
        // A typical function matching this interface might look as follows:
        //..
        //  void myReadCallback(int                   result,
        //                      int                  *numConsumed,
        //                      int                  *numNeeded,
        //                      const btemt_DataMsg&  msg);
        //..

    typedef bdef_Function<void (*)(int ,int *, bcema_Blob *, int)>
                                                         BlobBasedReadCallback;
        // A callback of this type is invoked for 'read' and 'timeRead' when
        // either the requested number of bytes are available, or an error
        // occurs.  The callback is invoked with four arguments: (1) an integer
        // result code corresponding to 'ReadResult' that indicates the result
        // of the read operation, and if this code is equal to 'BTEMT_SUCCESS',
        // (2) a pointer to an integer value where the callee can indicate how
        // many more bytes are needed to complete the read operation, (3) a
        // modifiable 'bcema_Blob' object containing the payload.  The caller
        // is responsible for taking ownership of a certain number of bytes in
        // the 'bcema_Blob' and readjusting it appropriately.  Note that
        // the read operation is not considered completed until the callee
        // indicates that zero more bytes are needed (argument 2), and (4)
        // channel id.  Also note that the last three arguments are ignored if
        // the first argument is different from 'BTEMT_SUCCESS'.
        //
        // A typical function matching this interface might look as follows:
        //..
        //  void myReadCallback(int         result,
        //                      int        *numNeeded,
        //                      bcema_Blob *blob,
        //                      int         channelId);
        //..

    // CREATORS
    virtual ~btemt_AsyncChannel();
        // Destroy this channel.

    // MANIPULATORS
    virtual int read(int                  numBytes,
                     const ReadCallback&  readCallback) = 0;
        // Initiate an asynchronous read operation on this channel, or append
        // this request to the currently pending requests if an asynchronous
        // read operation was already initiated.  When at least the specified
        // 'numBytes' of data are available after all previous requests have
        // been processed, if any, the specified 'readCallback' will be invoked
        // (with 'BTEMT_SUCCESS').  Return zero on success, and a non-zero
        // value (in 'btemt::ChannelErrorRead::Enum') on error.  The behavior
        // is undefined unless this channel was constructed to use a
        // 'ReadCallback'.

    virtual int read(int                          numBytes,
                     const BlobBasedReadCallback& readCallback) = 0;
        // Initiate an asynchronous read operation on this channel, or append
        // this request to the currently pending requests if an asynchronous
        // read operation was already initiated.  When at least the specified
        // 'numBytes' of data are available after all previous requests have
        // been processed, if any, the specified 'readCallback' will be invoked
        // (with 'BTEMT_SUCCESS').  Return 0 on success, and a non-zero
        // value (in 'btemt::ChannelErrorRead::Enum') on error.  The behavior
        // is undefined unless this channel was constructed to use a
        // 'BlobBasedReadCallback'.

    virtual int timedRead(int                      numBytes,
                          const bdet_TimeInterval& timeout,
                          const ReadCallback&      readCallback) = 0;
        // Initiate an asynchronous timed read operation on this channel, or
        // append this request to the currently pending requests if an
        // asynchronous read operation was already initiated, with an
        // associated specified absolute 'timeOut'.  When at least the
        // specified 'numBytes' of data are available after all previous
        // requests have been processed, if any, or when the 'timeOut' is
        // reached, the specified 'readCallback' will be invoked (with either
        // 'BTEMT_SUCCESS' or 'BTEMT_TIMEOUT', respectively).  Return 0 on
        // success, and a non-zero value (in 'btemt::ChannelErrorRead::Enum')
        // on error.  The behavior is undefined unless this channel was
        // constructed to use a 'ReadCallback'.

    virtual int timedRead(int                          numBytes,
                          const bdet_TimeInterval&     timeout,
                          const BlobBasedReadCallback& readCallback) = 0;
        // Initiate an asynchronous timed read operation on this channel, or
        // append this request to the currently pending requests if an
        // asynchronous read operation was already initiated, with an
        // associated specified absolute 'timeOut'.  When at least the
        // specified 'numBytes' of data are available after all previous
        // requests have been processed, if any, or when the 'timeOut' is
        // reached, the specified 'readCallback' will be invoked (with either
        // 'BTEMT_SUCCESS' or 'BTEMT_TIMEOUT', respectively).  Return 0 on
        // success, and a non-zero value (in 'btemt::ChannelErrorRead::Enum')
        // on error.  The behavior is undefined unless this channel was
        // constructed to use a 'BlobBasedReadCallback'.

    virtual int write(const bcema_Blob&    blob,
                      int                  highWaterMark = INT_MAX) = 0;
    virtual int write(const btemt_BlobMsg& blob,
                      int                  highWaterMark = INT_MAX) = 0;
        // Enqueue the specified 'blob' message to be written to this channel.
        // Optionally provide 'highWaterMark' to specify the maximum data size
        // that can be enqueued.  If 'highWaterMark' is not specified then
        // 'INT_MAX' is used.  Return 0 on success, and a non-zero value (in
        // 'btemt::ChannelErrorWrite::Enum') on error.  Note that success does
        // not imply that the data has been written or will be successfully
        // written to the underlying stream used by this channel.  Also note
        // that in addition to 'highWatermark' the enqueued portion must also
        // be less than a high watermark value supplied at the construction of
        // this channel for the write to succeed.

    virtual int write(const btemt_DataMsg&  data,
                      btemt_BlobMsg        *msg = 0) = 0;
    virtual int write(const btemt_DataMsg&  data,
                      int                   highWaterMark,
                      btemt_BlobMsg        *msg = 0) = 0;
        // Enqueue the specified 'data' message to be written to this channel.
        // Optionally provide 'highWaterMark' to specify the maximum data size
        // that can be enqueued.  If 'highWaterMark' is not specified then
        // 'INT_MAX' is used.  Return 0 on success, and a non-zero value (in
        // 'btemt::ChannelErrorWrite::Enum') on error.  Note that success does
        // not imply that the data has been written or will be successfully
        // written to the underlying stream used by this channel.  Also note
        // that in addition to 'highWatermark' the enqueued portion must also
        // be less than a high watermark value supplied at the construction of
        // this channel for the write to succeed.  Also note that the specified
        // blob 'msg' is ignored.

    virtual int setSocketOption(int option, int level, int value) = 0;
        // Set the specified 'option' (of the specified 'level') socket option
        // on this channel to the specified 'value'.  Return 0 on success and
        // a non-zero value otherwise.
        // (See 'bteso_socketoptutil' for the list of commonly supported
        // options.)

    virtual void cancelRead() = 0;
        // Cancel all pending 'read' or 'timedRead' requests, and invoke their
        // read callbacks with a 'btemt_AsyncChannel::BTEMT_CANCELED' status.
        // Note that if the channel is active, the read callbacks are invoked
        // in the thread in which the channel's data callbacks are invoked,
        // else they are invoked in the thread calling 'cancelRead'.

    virtual void close() = 0;
        // Shutdown this channel, and cancel all pending 'read' or 'timedRead'
        // requests (but do not invoke them).

    // ACCESSORS
    virtual bteso_IPv4Address localAddress() const = 0;
        // Return the address of the "local" end of this channel.

    virtual bteso_IPv4Address peerAddress() const = 0;
        // Return the address of the "remote" end of this channel.
};

} // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
