// btemt_channelpoolchannel.h      -*-C++-*-
#ifndef INCLUDED_BTEMT_CHANNELPOOLCHANNEL
#define INCLUDED_BTEMT_CHANNELPOOLCHANNEL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provided a concrete implementation of the 'btemt_AsyncChannel'
//          protocol for 'btemt_ChannelPool' channels.
//
//@CLASSES:
//   btemt_ChannelPoolChannel: implementation of the 'btemt_AsyncChannel'
//                             protocol for 'btemt_ChannelPool' channels
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@SEE_ALSO: btemt_asyncchannel
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'btemt_AsyncChannel' protocol for 'btemt_ChannelPool' channels.  This
// component is intended to be used with 'btemt_sessionpool'.  For a usage
// example, see 'btemt_sessionpool'.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTEMT_ASYNCCHANNEL
#include <btemt_asyncchannel.h>
#endif

#ifndef INCLUDED_BTEMT_MESSAGE
#include <btemt_message.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BCEMT_LOCKGUARD
#include <bcemt_lockguard.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BDEUT_VARIANT
#include <bdeut_variant.h>
#endif

#ifndef INCLUDED_BSL_LIST
#include <bsl_list.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

class btemt_ChannelPool;
class bcema_PooledBufferChainFactory;
class bcema_PooledBlobBufferFactory;
class bcema_PoolAllocator;
                       // ==============================
                       // class btemt_ChannelPoolChannel
                       // ==============================

class btemt_ChannelPoolChannel: public btemt_AsyncChannel {
    // This class provides an implementation of the 'btemt_AsyncChannel'
    // protocol for 'btemt_ChannelPool'-based channels.

    // TYPES
    enum CallbackType {
        // Enumeration specifying various types of callbacks

        BTEMT_DATAMSG_BASED = 0,     // 'btemt_DataMsg' based callback
        BTEMT_BLOB_BASED             // 'bcema_Blob' based callback
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , DATAMSG_BASED = BTEMT_DATAMSG_BASED
      , BLOB_BASED    = BTEMT_BLOB_BASED
#endif
    };

    struct ReadQueueEntry {
        typedef bdeut_Variant2<ReadCallback, BlobBasedReadCallback> ReadCb;

        ReadCb                d_readCallback;    // read callback

        bdet_TimeInterval     d_timeOut;         // optional read timeout

        int                   d_timeOutTimerId;  // for timedRead requests

        int                   d_numBytesNeeded;  // number of bytes needed
                                                 // before to invoke the read
                                                 // callback 

        int                   d_progress;        // status of read request,
                                                 // one of
                                                 // AsyncChannel::ReadResult
                                                 // (SUCCESS, TIMEOUT or 
                                                 // CANCELED)
    };

    typedef bsl::list<ReadQueueEntry> ReadQueue;

    // DATA
    btemt_DataMsg         d_pooledBufferChainPendingData;
                                                  // pooled buffer chain data
                                                  // that has been read from
                                                  // channelPool but not yet
                                                  // consumed by any reader

    const bool            d_useBlobForDataReads;  // use blob for data reads

    bcemt_Mutex           d_mutex;                // mutex used to control
                                                  // access to this channel

    volatile bool         d_callbackInProgress;   // indicates when a callback
                                                  // is in progress

    bool                  d_closed;

    ReadQueue             d_readQueue;            // queue of pending read
                                                  // requests.

    bcema_PooledBufferChainFactory
                         *d_bufferChainFactory_p; // buffer chain factory used
                                                  // to allocate
                                                  // 'btemt_DataMsg' objects
                                                  // returned in data callback

    bool                  d_isBufferChainFactoryOwnedFlag;
                                                  // flag specifying if the
                                                  // buffer chain factory is
                                                  // owned by this channel

    bcema_PooledBlobBufferFactory
                         *d_blobBufferFactory_p;  // blob buffer factory used
                                                  // to allocate
                                                  // 'bcema_Blob' objects
                                                  // returned in data callback

    bool                  d_isBlobBufferFactoryOwnedFlag;
                                                  // flag specifying if the
                                                  // blob buffer factory is
                                                  // owned by this channel

    bcema_PoolAllocator  *d_spAllocator_p;        // shared ptr pool

    btemt_ChannelPool    *d_channelPool_p;        // held but not owned

    int                   d_nextClockId;          // next clock ID for timer
                                                  // registration

    int                   d_channelId;            // id of associated
                                                  // channelPool channel

    bteso_IPv4Address     d_peerAddress;          // cached peer address

    bteso_IPv4Address     d_localAddress;         // cached local address

    bslma_Allocator      *d_allocator_p;          // allocator (held, not
                                                  // owned)

  private:
    // PRIVATE MANIPULATORS
    template <typename BTEMT_CALLBACK>
    int addReadQueueEntry(int                      numBytes,
                          const BTEMT_CALLBACK&    callback,
                          const bdet_TimeInterval& timeOut);
        // Add a read queue entry with the specified 'callback' for the
        // specified 'numBytes' and the specified 'timeOut'.  Return 0 on
        // success, and a non-zero value otherwise.  Note that this function
        // assumes that 'd_mutex' is not locked when it is called.

    void assignData(bcema_Blob           *blob,
                    const btemt_DataMsg&  dataMsg,
                    int                   numBytes);
        // Assign the specified initial 'numBytes' of the specified 'dataMsg'
        // to the specified 'blob' resetting its length to 'numBytes'.

    void assignData(btemt_DataMsg     *dataMsg,
                    const bcema_Blob&  blob,
                    int                numBytes);
        // Assign the specified initial 'numBytes' of the specified 'blob'
        // to the specified 'dataMsg'.

    void removeTopReadEntry(bool invokeCallback);
        // Remove the top most read queue entry invoking its registered user
        // callback with dummy arguments if 'invokeCallback' is 'true'.  Note
        // that this function assumes that 'd_mutex' is locked before the call
        // and ensures that it is locked after the call.

    void registerTimeoutAndUpdateClockId(const bdet_TimeInterval& timeOut);
        // Register the specified 'timeOut' with the channel pool and update
        // the next clock id for this channel appropriately.  Note that this
        // function assumes that it is called after 'd_mutex' is locked.

    void timeoutCb(ReadQueue::iterator entry);
        // Remove the specified 'entry' from the read queue and invoke the read
        // callback, or mark it as timed out (in the 'd_progress' member) in
        // case 'entry' is currently being processed in the 'dataCb'.  Note
        // that the callback will be invoked only once, with either 'SUCCESS'
        // (in the 'dataCallback') or with 'TIMEOUT' or 'CANCELED' (even if
        // both timeout and cancellation happen at the same time as this read
        // callback is currently at the front of the read queue in 'dataCb').

  public:
    // CREATORS
    btemt_ChannelPoolChannel(
                        int                             channelId,
                        btemt_ChannelPool              *channelPool,
                        bcema_PooledBufferChainFactory *bufferFactory,
                        bcema_PoolAllocator            *spAllocator,
                        bslma_Allocator                *allocator,
                        bcema_PooledBlobBufferFactory  *blobBufferFactory = 0);
        // Create a 'btemt_AsyncChannel' concrete implementation reading from
        // and writing to the channel referenced by the specified 'channelId'
        // in the specified 'channelPool', using the specified 'bufferFactory'
        // and 'spAllocator' to supply memory to the data messages, and the
        // specified 'allocator' to supply memory.  Note that the constructed
        // channel will use 'bcema_PooledBufferChain' to read data from the
        // 'channelPool'.  Optionally specify 'blobBufferFactory' used to
        // supply memory for 'bcema_Blob' objects returned in the data callback
        // when a read is registered using a 'BlobBasedReadCallback'.

    btemt_ChannelPoolChannel(
                            int                             channelId,
                            btemt_ChannelPool              *channelPool,
                            bcema_PooledBlobBufferFactory  *blobBufferFactory,
                            bcema_PoolAllocator            *spAllocator,
                            bslma_Allocator                *allocator,
                            bcema_PooledBufferChainFactory *bufferFactory = 0);
        // Create a 'btemt_AsyncChannel' concrete implementation reading from
        // and writing to the channel referenced by the specified 'channelId'
        // in the specified 'channelPool', using the specified
        // 'blobBufferFactory' and 'spAllocator' to supply memory to the data
        // messages, and the specified 'allocator' to supply memory.  Note
        // that the constructed channel will use 'bcema_Blob' to read data
        // from the 'channelPool'.  Optionally specify 'bufferFactory' used to
        // supply memory for 'btemt_DataMsg' objects returned in the data
        // callback when a read is registered using a 'ReadCallback'.

    virtual ~btemt_ChannelPoolChannel();
        // Destroy this channel.

    // MANIPULATORS
    virtual int read(int numBytes, const ReadCallback& readCallback);
        // Initiate an asynchronous read operation on this channel, or append
        // this request to the currently pending requests if an asynchronous
        // read operation was already initiated.  When at least the specified
        // 'numBytes' of data are available after all previous requests have
        // been processed, if any, the specified 'readCallback' will be invoked
        // (with 'SUCCESS').  Return zero on success, and a non-zero value
        // otherwise.

    virtual int read(int                           numBytes,
                     const BlobBasedReadCallback&  readCallback);
        // Initiate an asynchronous read operation on this channel, or append
        // this request to the currently pending requests if an asynchronous
        // read operation was already initiated.  When at least the specified
        // 'numBytes' of data are available after all previous requests have
        // been processed, if any, the specified 'readCallback' will be
        // invoked (with 'SUCCESS').  Return zero on success, and a non-zero
        // value otherwise.

    virtual int timedRead(int                      numBytes,
                          const bdet_TimeInterval& timeOut,
                          const ReadCallback&      readCallback);
        // Initiate an asynchronous timed read operation on this channel, or
        // append this request to the currently pending requests if an
        // asynchronous read operation was already initiated, with an
        // associated specified absolute 'timeOut'.  When at least the
        // specified 'numBytes' of data are available after all previous
        // requests have been processed, if any, or when the 'timeOut' is
        // reached, the specified 'readCallback' will be invoked (with either
        // 'SUCCESS' or 'TIMEOUT', respectively).  Return zero on success, and
        // a non-zero value otherwise.

    virtual int timedRead(int                          numBytes,
                          const bdet_TimeInterval&     timeOut,
                          const BlobBasedReadCallback& readCallback);
        // Initiate an asynchronous timed read operation on this channel, or
        // append this request to the currently pending requests if an
        // asynchronous read operation was already initiated, with an
        // associated specified absolute 'timeOut'.  When at least the
        // specified 'numBytes' of data are available after all previous
        // requests have been processed, if any, or when the 'timeOut' is
        // reached, the specified 'readCallback' will be invoked (with either
        // 'SUCCESS' or 'TIMEOUT', respectively).  Return zero on success,
        // and a non-zero value otherwise.

    virtual int write(const bcema_Blob&    blob,
                      int                  highWaterMark = INT_MAX);
    virtual int write(const btemt_BlobMsg& blob,
                      int                  highWaterMark = INT_MAX);
        // Enqueue the specified 'blob' message to be written to this channel.
        // Return zero on success, and a non-zero value otherwise.

    virtual int write(const btemt_DataMsg&  data,
                      btemt_BlobMsg        *msg = 0);
    virtual int write(const btemt_DataMsg&  data,
                      int                   highWaterMark,
                      btemt_BlobMsg        *msg = 0);
        // Enqueue the specified 'data' message to be written to this channel.
        // If the optionally specified 'msg' is not zero, load in 'msg' the
        // message converted to a 'btemt_BlobMsg'.  Return zero on success, and
        // a non-zero value otherwise.

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

    void dataCb(int *numConsumed, int *numNeeded, const btemt_DataMsg& msg);
        // This method is invoked in response to a channel pool data callback
        // on the channel id associated with this channel, and invokes the
        // pending read requests until either more data is needed or this
        // channel is closed.  Note that the behavior is undefined if this
        // method is invoked concurrently from multiple threads.

    void blobBasedDataCb(int *numNeeded, bcema_Blob *msg);
        // This method is invoked in response to a blob based channel pool
        // data callback on the channel id associated with this channel, and
        // invokes the pending read requests until either more data is needed
        // or this channel is closed.  Note that the behavior is undefined if
        // this method is invoked concurrently from multiple threads.

    virtual void close();
        // Shutdown this channel, and cancel all pending requests.  Note that
        // this call will result in the shutdown of the channel pool channel
        // associated with the channel, and will not invoke the pending read
        // requests.

    // ACCESSORS
    virtual bteso_IPv4Address localAddress() const;
        // Return the address of the "local" end of the channel.

    virtual bteso_IPv4Address peerAddress() const;
        // Return the address of the "remote" end of the channel.

    int channelId() const;
        // Return the channelPool channel Id associated with this channel.

    btemt_ChannelPool *channelPool();
        // Return a pointer to the channelPool object associated with the
        // channel.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // ------------------------------
                       // class btemt_ChannelPoolChannel
                       // ------------------------------

// ACCESSORS
inline
int btemt_ChannelPoolChannel::channelId() const
{
    return d_channelId;
}

inline
btemt_ChannelPool *btemt_ChannelPoolChannel::channelPool()
{
    return d_channelPool_p;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
