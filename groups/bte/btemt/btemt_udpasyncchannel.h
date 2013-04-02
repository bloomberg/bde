// btemt_udpasyncchannel.h               -*-C++-*-
#ifndef INCLUDED_BTEMT_UDPASYNCCHANNEL
#define INCLUDED_BTEMT_UDPASYNCCHANNEL

//@PURPOSE: Provide a concrete mechanism of the 'btemt_AsyncChannel' protocol
//          backed by a UDP socket.
//
//@CLASSES:
//   UdpAsyncChannel: implementation of the 'btemt_AsyncChannel' protocol
//                    backed by a UDP socket.
//
//@AUTHOR: Eric Vander Weele (evander)
//
//@SEE ALSO: btemt_AsyncChannel
//
//@DESCRIPTION: This component defines ...
//
///Usage Example
///-------------
// Text
//..
//  Example code here
//..

#ifndef INCLUDED_BTEMT_ASYNCCHANNEL
#include <btemt_asyncchannel.h>
#endif

#ifndef INCLUDED_BTEMT_TCPTIMEREVENTMANAGER
#include <btemt_tcptimereventmanager.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BTESO_SOCKETHANDLE
#include <bteso_sockethandle.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEX
#include <bcemt_mutex.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_LIST
#include <bsl_list.h>
#endif

namespace BloombergLP {

class bcema_BlobBufferFactory;
class bdet_TimeInterval;
namespace bslma {class Allocator;}

namespace btemt {

                        // =====================
                        // class UdpAsyncChannel
                        // =====================

class UdpAsyncChannel : public btemt_AsyncChannel {
    // Class documentation goes here

  private:
    // PRIVATE TYPES
    typedef btemt_AsyncChannel::BlobBasedReadCallback BlobBasedReadCallback;
    typedef btemt_AsyncChannel::ReadResult            Progress;

    struct ReadQueueEntry {
        BlobBasedReadCallback  d_callback;
        Progress               d_progress;
        bdet_TimeInterval      d_timeout;
        void                  *d_timerId;
    };

    typedef bsl::list<ReadQueueEntry> ReadQueue;

    // DATA
    bslma::Allocator            *d_allocator_p;         // held
    bteso_IPv4Address           d_localAddress;
    bteso_IPv4Address           d_peerAddress;
    bcema_BlobBufferFactory    *d_blobBufferFactory_p; // held
    bteso_SocketHandle::Handle  d_socketHandle;
    bteso_TimerEventManager    *d_eventManager_p;      // held
    bcemt_Mutex                 d_mutex;
    ReadQueue                   d_readQueue;
    bool                        d_callbackInProgress;
    bool                        d_wasBindError;
    bcema_SharedPtr<bool>       d_closed_sp;

    // PRIVATE MANIPULATORS
    void socketReadCb(bcema_SharedPtr<bool> isClosed);
        // This method is invoked in response to socket READ events.  Data is
        // read from the UDP socket and this data is consumed if any enqueued
        // readers are available.

    void timeoutCb(bcema_SharedPtr<bool> isClosed, ReadQueue::iterator entry);
        // Remove the specified 'ReadQueueEntry' from the read queue and invoke
        // the read callback or make it as timed out (in the 'd_progress'
        // member) in case 'entry' is currently being processed.

    int addReadQueueEntry(
                     const btemt_AsyncChannel::BlobBasedReadCallback& callback,
                     const bdet_TimeInterval&                         timeout);
        // Add a 'ReadQueueEntry' for the specified 'callback' with a specified
        // 'timeout'.  Return 0 on success and a non-zero value otherwise.
        // Note that this function assumes that it is called after 'd_mutex' is
        // locked.

    void removeTopReadEntry(ReadResult            *progress,
                            BlobBasedReadCallback *callback);
        // TODO: update doc
        // Remove the top most read queue entry invoking its registered user
        // callback with dummy arguments if 'invokeCallback' is 'true'.  Note
        // that this function assumes that 'd_mutex' is locked before the call
        // and it ensures that it is locked after the call.

    bool openSocket();
        // Open a UDP socket.  Return 'true' on success, otherwise 'false'.

    bool bindSocket();
        // Bind a UDP socket to the specified 'd_localAddress'.  Return 'true'
        // on success, otherwise 'false'.

    bool connectSocket();
        // Connect to the specified 'd_peerAddress'.  Return 'true' on success,
        // otherwise 'false'.

  private:
    // NOT IMPLEMENTED
    UdpAsyncChannel(const UdpAsyncChannel&);
    UdpAsyncChannel& operator=(const UdpAsyncChannel&);

  public:
    // CREATORS
    UdpAsyncChannel(bteso_TimerEventManager  *eventManager,
                    const bteso_IPv4Address&  peerAddress,
                    bcema_BlobBufferFactory  *blobBufferFactory,
                    bslma::Allocator         *allocator = 0);
        // Create a 'UdpAsyncChannel' object to communicate with the specified
        // 'peerAddress'.  Use the specified 'blobBufferFactory' for blob
        // creation in this object.  'blobBufferFactory' *must* outlive this
        // object.  Use the optionally specified 'allocator' to supply memory.
        // Note that the bound address and port will be chosen by the operating
        // system.  If 'isClosed() == true' after this call then an error
        // occurred while initializing the channel.

    UdpAsyncChannel(bteso_TimerEventManager  *eventManager,
                    const bteso_IPv4Address&  localAddress,
                    const bteso_IPv4Address&  peerAddress,
                    bcema_BlobBufferFactory  *blobBufferFactory,
                    bslma::Allocator          *allocator = 0);
        // Create a 'UdpAsyncChannel' object bound to the specified
        // 'localAddress' to communicate with the specified 'peerAddress'.  Use
        // the specified 'blobBufferFactory' for blob creation in this object.
        // 'blobBufferFactory' *must* outlive this object.  Use the optionally
        // specified 'allocator' to supply memory.  If 'isClosed() == true'
        // after this call then an error occurred while initializing the
        // channel.

    ~UdpAsyncChannel();
        // Destroy this object.

    // MANIPULATORS
    virtual int read(int                 numBytes,
                     const ReadCallback& readCallback);
        // NOT SUPPORTED.  Behavior is undefined calling this method.

    virtual int read(int                          numBytes,
                     const BlobBasedReadCallback& readCallback);
        // Initiate an asynchronous read operation on this channel.  The
        // specified 'numBytes' parameter is ignored.  When a message is
        // received, if any, the specified 'readCallback' will be invoked (with
        // 'BTEMT_SUCCESS').  Return zero on success, and a non-zero otherwise.
        //
    virtual int timedRead(int                      numBytes,
                          const bdet_TimeInterval& timeout,
                          const ReadCallback&      readCallback);
        // NOT SUPPORTED.  Behavior is undefined calling this method.

    virtual int timedRead(int                          numBytes,
                          const bdet_TimeInterval&     timeout,
                          const BlobBasedReadCallback& readCallback);
        // Initiate an asynchronous timed read operation on this channel.  The
        // specified 'numBytes' parameter is ignored.  When the timeOut is
        // reached, the specified 'readCallback' will be invoked (with either
        // BTEMT_SUCCESS or BTEMT_TIMEOUT, respectively). Return zero on
        // success, and a non-zero value on error. The behavior is undefined
        // unless this channel was constructed to use a BlobBasedReadCallback.

    virtual int write(const bcema_Blob& blob,
                      int               highWaterMark = INT_MAX);
        // Write the specified 'blob' message to this channel, return zero on
        // success, and a non-zero value otherwise.  Note that the optionally
        // specified 'highWaterMark' is ignored.

    virtual int write(const btemt_BlobMsg& blob,
                      int                  highWaterMark = INT_MAX);
        // NOT SUPPORTED.  Behavior is undefined calling this method.

    virtual int write(const btemt_DataMsg&  data,
                      btemt_BlobMsg        *msg = 0);
        // NOT SUPPORTED

    virtual int write(const btemt_DataMsg&  data,
                      int                   highWaterMark,
                      btemt_BlobMsg        *msg = 0);
        // NOT SUPPORTED

    virtual int setSocketOption(int option, int level, int value);
        // Set the specified option (of the specified level) socket option on
        // this channel to the specified value. Return 0 on success and a
        // non-zero value otherwise. (See bteso_socketoptutil for the list of
        // commonly supported options.)

    virtual void cancelRead();
        //Cancel all pending read or timedRead requests, and invoke their read
        //callbacks with a btemt_AsyncChannel::BTEMT_CANCELED status. Note that
        //if the channel is active, the read callbacks are invoked in the
        //thread in which the channel's data callbacks are invoked, else they
        //are invoked in the thread calling cancelRead.

    virtual void close();
        // Shutdown this channel, and cancel all pending read or timedRead
        // requests (but do not invoke them).

    // ACCESSORS
    bool isClosed() const;
        // Return whether this channel is closed, and therefore cannot be used.

    bool wasBindError() const;
        // Return 'true' if the channel was closed because of a bind error.

    virtual bteso_IPv4Address localAddress() const;
        // Return the address of the "local" end of the channel.

    virtual bteso_IPv4Address peerAddress() const;
        // Return the address of the "remote" end of the channel.  Note that
        // this will always be the peer address passed in upon construction.
};

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
