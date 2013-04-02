// btemt_socks5udpchannel.h               -*-C++-*-
#ifndef INCLUDED_BTEMT_SOCKS5UDPCHANNEL
#define INCLUDED_BTEMT_SOCKS5UDPCHANNEL

//@PURPOSE:
//
//@CLASSES:
//   Socks5UdpChannel: <<description>>
//
//@AUTHOR: Kevin McMahon (kmcmahon)
//
//@SEE ALSO:
//
//@DESCRIPTION: This component defines ...
//
///Usage Example
///-------------
// Text
//..
//  Example code here
//..

#ifndef INCLUDED_BTEMT_INTERNETADDRESS
#include <btemt_internetaddress.h>
#endif

#ifndef INClUDED_BTEMT_ASYNCHANNEL
#include <btemt_asyncchannel.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTR
#include <bdema_managedptr.h>
#endif

namespace BloombergLP {

namespace bslma {class Allocator;}
class bteso_IPv4Address;
class bcema_BlobBufferFactory;

namespace btemt {

                        // ======================
                        // class Socks5UdpChannel
                        // ======================

class Socks5UdpChannel : public btemt_AsyncChannel {
    friend struct Socks5UdpChannel_ReadCallback;
    // Class documentation goes here

  public:
    // PUBLIC TYPES

  private:
    // PRIVATE TYPES

    // DATA
      bdema_ManagedPtr<btemt_AsyncChannel>  d_udpAsyncChannel_mp;
      InternetAddress                       d_destinationAddress;
      bcema_BlobBufferFactory              *d_blobBufferFactory_p;
      btemt_AsyncChannel                   *d_associatedTcpChannel_p;

    // PRIVATE MANIPULATORS

    void handleRead(int result,
                    int                                       *bytesNeeded,
                    bcema_Blob                                *payload,
                    int                                        channelId,
                    btemt_AsyncChannel::BlobBasedReadCallback  callback);

    // PRIVATE eACCESSORS

  private:
    // NOT IMPLEMENTED
    Socks5UdpChannel(const Socks5UdpChannel&);
    Socks5UdpChannel& operator=(const Socks5UdpChannel&);

  public:
    // CREATORS
    Socks5UdpChannel(
                    bdema_ManagedPtr<btemt_AsyncChannel>  udpAsyncChannel,
                    btemt_AsyncChannel                   *associatedTcpChannel,
                    bcema_BlobBufferFactory              *blobBufferFactory,
                    const InternetAddress&                destAddress,
                    bslma::Allocator                     *allocator = 0);
        // Create a 'Socks5UdpChannel' object having ...
        // Use the optionally specified 'allocator' to supply memory.

    virtual ~Socks5UdpChannel();
        // Destroy this object.

    // MANIPULATORS
    virtual int read(int                  numBytes,
                     const ReadCallback&  readCallback);
        // Initiate an asynchronous read operation on this channel, or append
        // this request to the currently pending requests if an asynchronous
        // read operation was already initiated.  When at least the specified
        // 'numBytes' of data are available after all previous requests have
        // been processed, if any, the specified 'readCallback' will be
        // invoked (with 'BTEMT_SUCCESS').  Return zero on success, and a
        // non-zero value on error.  The behavior is undefined unless this
        // channel was constructed to use a 'ReadCallback'.

    virtual int read(int                          numBytes,
                     const BlobBasedReadCallback& readCallback);
        // Initiate an asynchronous read operation on this channel, or append
        // this request to the currently pending requests if an asynchronous
        // read operation was already initiated.  When at least the specified
        // 'numBytes' of data are available after all previous requests have
        // been processed, if any, the specified 'readCallback' will be
        // invoked (with 'BTEMT_SUCCESS').  Return zero on success, and a
        // non-zero value on error.  The behavior is undefined unless this
        // channel was constructed to use a 'BlobBasedReadCallback'.

    virtual int timedRead(int                      numBytes,
                          const bdet_TimeInterval& timeout,
                          const ReadCallback&      readCallback);
        // Initiate an asynchronous timed read operation on this channel, or
        // append this request to the currently pending requests if an
        // asynchronous read operation was already initiated, with an
        // associated specified absolute 'timeOut'.  When at least the
        // specified 'numBytes' of data are available after all previous
        // requests have been processed, if any, or when the 'timeOut' is
        // reached, the specified 'readCallback' will be invoked (with either
        // 'BTEMT_SUCCESS' or 'BTEMT_TIMEOUT', respectively).  Return zero on
        // success, and a non-zero value on error.  The behavior is undefined
        // unless this channel was constructed to use a 'ReadCallback'.

    virtual int timedRead(int                          numBytes,
                          const bdet_TimeInterval&     timeout,
                          const BlobBasedReadCallback& readCallback);
        // Initiate an asynchronous timed read operation on this channel, or
        // append this request to the currently pending requests if an
        // asynchronous read operation was already initiated, with an
        // associated specified absolute 'timeOut'.  When at least the
        // specified 'numBytes' of data are available after all previous
        // requests have been processed, if any, or when the 'timeOut' is
        // reached, the specified 'readCallback' will be invoked (with either
        // 'BTEMT_SUCCESS' or 'BTEMT_TIMEOUT', respectively).  Return zero on
        // success, and a non-zero value on error.  The behavior is undefined
        // unless this channel was constructed to use a
        // 'BlobBasedReadCallback'.

    virtual int write(const bcema_Blob&    blob,
                      int                  highWaterMark = INT_MAX);
    virtual int write(const btemt_BlobMsg& blob,
                      int                  highWaterMark = INT_MAX);
        // Enqueue the specified 'blob' message to be written to this channel,
        // return zero on success, and a non-zero value otherwise.  Note that a
        // success does not imply that the data has been written or will be
        // successfully written to the underlying stream used by this channel.

    virtual int write(const btemt_DataMsg&  data,
                      btemt_BlobMsg        *msg = 0);
    virtual int write(const btemt_DataMsg&  data,
                      int                   highWaterMark,
                      btemt_BlobMsg        *msg = 0);
        // Enqueue the specified 'data' message to be written to this channel,
        // return zero on success, and a non-zero value otherwise.  Note that
        // success does not imply that the data has been written or will be
        // successfully written to the underlying stream used by this channel.

    virtual int setSocketOption(int option, int level, int value);
        // Set the specified 'option' (of the specified 'level') socket option
        // on this channel to the specified 'value'.  Return 0 on success and
        // a non-zero value otherwise.
        // (See 'bteso_socketoptutil' for the list of commonly supported
        // options.)

    virtual void cancelRead();
        // Cancel all pending 'read' or 'timedRead' requests, and invoke their
        // read callbacks with a 'btemt_AsyncChannel::BTEMT_CANCELED' status.
        // Note that if the channel is active, the read callbacks are invoked
        // in the thread in which the channel's data callbacks are invoked,
        // else they are invoked in the thread calling 'cancelRead'.

    virtual void close();
        // Shutdown this channel, and cancel all pending 'read' or 'timedRead'
        // requests (but do not invoke them).

    // ACCESSORS
    virtual bteso_IPv4Address localAddress() const;
        // Return the address of the "local" end of this channel.

    virtual bteso_IPv4Address peerAddress() const;
        // Return the address of the "remote" end of this channel.

};

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                        // ----------------------
                        // class Socks5UdpChannel
                        // ----------------------

// CREATORS

// MANIPULATORS

// ACCESSORS

}  // close package namespace

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
