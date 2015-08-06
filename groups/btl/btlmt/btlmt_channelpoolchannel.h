// btlmt_channelpoolchannel.h                                         -*-C++-*-
#ifndef INCLUDED_BTLMT_CHANNELPOOLCHANNEL
#define INCLUDED_BTLMT_CHANNELPOOLCHANNEL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provided a concrete implementation of the 'btlmt::AsyncChannel'
//          protocol for 'btlmt::ChannelPool' channels.
//
//@CLASSES:
//   btlmt::ChannelPoolChannel: implementation of the 'btlmt::AsyncChannel'
//                             protocol for 'btlmt::ChannelPool' channels
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@SEE_ALSO: btlmt_asyncchannel
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'btlmt::AsyncChannel' protocol for 'btlmt::ChannelPool' channels.  This
// component is intended to be used with 'btlmt_sessionpool'.  For a usage
// example, see 'btlmt_sessionpool'.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLMT_ASYNCCHANNEL
#include <btlmt_asyncchannel.h>
#endif

#ifndef INCLUDED_BTLMT_MESSAGE
#include <btlmt_message.h>
#endif

#ifndef INCLUDED_BTLSO_IPV4ADDRESS
#include <btlso_ipv4address.h>
#endif

#ifndef INCLUDED_BDLMCA_BLOB
#include <bdlmca_blob.h>
#endif

#ifndef INCLUDED_BDLQQ_LOCKGUARD
#include <bdlqq_lockguard.h>
#endif

#ifndef INCLUDED_BDLQQ_MUTEX
#include <bdlqq_mutex.h>
#endif

#ifndef INCLUDED_BDLB_VARIANT
#include <bdlb_variant.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDPTR
#include <bslma_managedptr.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BSL_LIST
#include <bsl_list.h>
#endif

namespace BloombergLP {


namespace bdlmca { class PooledBufferChainFactory; }
namespace bdlma { class ConcurrentPoolAllocator; }

namespace btlmt {

class ChannelPool;

                       // ==============================
                       // class ChannelPoolChannel
                       // ==============================

class ChannelPoolChannel: public AsyncChannel {
    // This class provides an implementation of the 'AsyncChannel'
    // protocol for 'ChannelPool'-based channels.

    // TYPES
    enum CallbackType {
        // Enumeration specifying various types of callbacks

        BTEMT_DATAMSG_BASED = 0,     // 'DataMsg' based callback
        BTEMT_BLOB_BASED             // 'bdlmca::Blob' based callback
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , DATAMSG_BASED = BTEMT_DATAMSG_BASED
      , BLOB_BASED    = BTEMT_BLOB_BASED
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    struct ReadQueueEntry {
        typedef bdlb::Variant2<ReadCallback, BlobBasedReadCallback> ReadCb;

        ReadCb                d_readCallback;    // read callback

        bsls::TimeInterval     d_timeOut;         // optional read timeout

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
    DataMsg         d_pooledBufferChainPendingData;
                                                  // pooled buffer chain data
                                                  // that has been read from
                                                  // channelPool but not yet
                                                  // consumed by any reader

    const bool            d_useBlobForDataReads;  // use blob for data reads

    bdlqq::Mutex           d_mutex;                // mutex used to control
                                                  // access to this channel

    volatile bool         d_callbackInProgress;   // indicates when a callback
                                                  // is in progress

    bool                  d_closed;

    ReadQueue             d_readQueue;            // queue of pending read
                                                  // requests.

    bslma::ManagedPtr<bdlmca::PooledBufferChainFactory>
                          d_bufferChainFactory_p; // buffer chain factory used
                                                  // to allocate
                                                  // 'DataMsg' objects
                                                  // returned in data callback

    bslma::ManagedPtr<bdlmca::BlobBufferFactory>
                          d_blobBufferFactory_p;  // blob buffer factory used
                                                  // to allocate
                                                  // 'bdlmca::Blob' objects
                                                  // returned in data callback

    bdlma::ConcurrentPoolAllocator  *d_spAllocator_p;        // shared ptr pool

    ChannelPool    *d_channelPool_p;        // held but not owned

    int                   d_nextClockId;          // next clock ID for timer
                                                  // registration

    int                   d_channelId;            // id of associated
                                                  // channelPool channel

    btlso::IPv4Address     d_peerAddress;          // cached peer address

    btlso::IPv4Address     d_localAddress;         // cached local address

    bslma::Allocator     *d_allocator_p;          // allocator (held, not
                                                  // owned)

  private:
    // PRIVATE MANIPULATORS
    template <typename BTEMT_CALLBACK>
    int addReadQueueEntry(int                      numBytes,
                          const BTEMT_CALLBACK&    callback,
                          const bsls::TimeInterval& timeOut);
        // Add a read queue entry with the specified 'callback' for the
        // specified 'numBytes' and the specified 'timeOut'.  Return 0 on
        // success, and a non-zero value otherwise.  Note that this function
        // assumes that 'd_mutex' is not locked when it is called.

    void assignData(bdlmca::Blob           *blob,
                    const DataMsg&  dataMsg,
                    int                   numBytes);
        // Assign the specified initial 'numBytes' of the specified 'dataMsg'
        // to the specified 'blob' resetting its length to 'numBytes'.

    void assignData(DataMsg     *dataMsg,
                    const bdlmca::Blob&  blob,
                    int                numBytes);
        // Assign the specified initial 'numBytes' of the specified 'blob'
        // to the specified 'dataMsg'.

    void removeTopReadEntry(bool invokeCallback);
        // Remove the top most read queue entry invoking its registered user
        // callback with dummy arguments if 'invokeCallback' is 'true'.  Note
        // that this function assumes that 'd_mutex' is locked before the call
        // and ensures that it is locked after the call.

    void registerTimeoutAndUpdateClockId(const bsls::TimeInterval& timeOut);
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
    ChannelPoolChannel(
                  int                             channelId,
                  ChannelPool              *channelPool,
                  bdlmca::PooledBufferChainFactory *bufferChainFactory,
                  bdlmca::BlobBufferFactory        *blobBufferFactory,
                  bdlma::ConcurrentPoolAllocator            *spAllocator,
                  bslma::Allocator               *allocator,
                  bool                            useBlobForDataReads = false);
        // Create a 'AsyncChannel' concrete implementation reading from
        // and writing to the channel referenced by the specified 'channelId'
        // in the specified 'channelPool', using the specified
        // 'bufferChainFactory', the specified 'blobBufferFactory', the
        // specified 'spAllocator' to supply memory to the data messages, and
        // the specifed 'allocator' to supply memory.  If 'bufferChainFactory'
        // is 0, create a 'bdlmca::PooledBufferChainFactory' object internally
        // using 'allocator'.  If 'blobBufferFactory' is 0, create a
        // 'bdlmca::PooledBlobBufferFactory' object internally using 'allocator'.
        // Optionally specify 'useBlobForDataReads' to control how data
        // messages are read from the 'channelPool'.  The contructed channel
        // reads data messages using 'bdlmca::Blob' (supplied by
        // 'blobBufferFactory') if 'useBlobForDataReads' is 'true',
        // and 'DataMsg' (supplied by 'bdlmca::PooledBufferChainFactory')
        // otherwise.  Note that clients can request data messages from the
        // constructed channel using either 'bdlmca::Blob' or 'DataMsg',
        // however, the data messages must be transfered if they are requested
        // using a type different than that used internally by the constructed
        // channel.

    virtual ~ChannelPoolChannel();
        // Destroy this channel.

    // MANIPULATORS
    virtual int read(int numBytes, const ReadCallback& readCallback);
        // Initiate an asynchronous read operation on this channel, or append
        // this request to the currently pending requests if an asynchronous
        // read operation was already initiated.  When at least the specified
        // 'numBytes' of data are available after all previous requests have
        // been processed, if any, the specified 'readCallback' will be invoked
        // (with 'SUCCESS').  Return 0 on success, and a non-zero value
        // otherwise.  On error, the return value *may* equal to one of the
        // enumerators in 'ChannelStatus::Enum'.

    virtual int read(int                           numBytes,
                     const BlobBasedReadCallback&  readCallback);
        // Initiate an asynchronous read operation on this channel, or append
        // this request to the currently pending requests if an asynchronous
        // read operation was already initiated.  When at least the specified
        // 'numBytes' of data are available after all previous requests have
        // been processed, if any, the specified 'readCallback' will be invoked
        // (with 'SUCCESS').  Return 0 on success, and a non-zero value
        // otherwise.  On error, the return value *may* equal to one of the
        // enumerators in 'ChannelStatus::Enum'.

    virtual int timedRead(int                      numBytes,
                          const bsls::TimeInterval& timeOut,
                          const ReadCallback&      readCallback);
        // Initiate an asynchronous timed read operation on this channel, or
        // append this request to the currently pending requests if an
        // asynchronous read operation was already initiated, with an
        // associated specified absolute 'timeOut'.  When at least the
        // specified 'numBytes' of data are available after all previous
        // requests have been processed, if any, or when the 'timeOut' is
        // reached, the specified 'readCallback' will be invoked (with either
        // 'SUCCESS' or 'TIMEOUT', respectively).  Return 0 on success, and a
        // non-zero value otherwise.  On error, the return value *may* equal to
        // one of the enumerators in 'ChannelStatus::Enum'.

    virtual int timedRead(int                          numBytes,
                          const bsls::TimeInterval&     timeOut,
                          const BlobBasedReadCallback& readCallback);
        // Initiate an asynchronous timed read operation on this channel, or
        // append this request to the currently pending requests if an
        // asynchronous read operation was already initiated, with an
        // associated specified absolute 'timeOut'.  When at least the
        // specified 'numBytes' of data are available after all previous
        // requests have been processed, if any, or when the 'timeOut' is
        // reached, the specified 'readCallback' will be invoked (with either
        // 'SUCCESS' or 'TIMEOUT', respectively).  Return 0 on success, and a
        // non-zero value otherwise.  On error, the return value *may* equal to
        // one of the enumerators in 'ChannelStatus::Enum'.

    virtual int write(const bdlmca::Blob&    blob,
                      int                  highWaterMark = INT_MAX);
    virtual int write(const BlobMsg& blob,
                      int                  highWaterMark = INT_MAX);
        // Enqueue the specified 'blob' message to be written to this channel.
        // Optionally provide 'highWaterMark' to specify the maximum data size
        // that can be enqueued.  If 'highWaterMark' is not specified then
        // 'INT_MAX' is used.  Return 0 on success, and a non-zero value
        // otherwise.  On error, the return value *may* equal to one of the
        // enumerators in 'ChannelStatus::Enum'.  Note that success does
        // not imply that the data has been written or will be successfully
        // written to the underlying stream used by this channel.  Also note
        // that in addition to 'highWatermark' the enqueued portion must also
        // be less than a high watermark value supplied at the construction of
        // this channel for the write to succeed.

    virtual int write(const DataMsg&  data,
                      BlobMsg        *msg = 0);
    virtual int write(const DataMsg&  data,
                      int                   highWaterMark,
                      BlobMsg        *msg = 0);
        // Enqueue the specified 'data' message to be written to this channel.
        // Optionally provide 'highWaterMark' to specify the maximum data size
        // that can be enqueued.  If 'highWaterMark' is not specified then
        // 'INT_MAX' is used.  Return 0 on success, and a non-zero value
        // otherwise.  On error, the return value *may* equal to one of the
        // enumerators in 'ChannelStatus::Enum'.  Note that success does
        // not imply that the data has been written or will be successfully
        // written to the underlying stream used by this channel.  Also note
        // that in addition to 'highWatermark' the enqueued portion must also
        // be less than a high watermark value supplied at the construction of
        // this channel for the write to succeed.  Also note that the specified
        // blob 'msg' is ignored.

    virtual int setSocketOption(int option, int level, int value);
        // Set the specified 'option' (of the specified 'level') socket option
        // on this channel to the specified 'value'.  Return 0 on success and
        // a non-zero value otherwise.
        // (See 'btlso_socketoptutil' for the list of commonly supported
        // options.)

    virtual void cancelRead();
        // Cancel all pending 'read' or 'timedRead' requests, and invoke their
        // read callbacks with a 'AsyncChannel::BTEMT_CANCELED' status.
        // Note that if the channel is active, the read callbacks are invoked
        // in the thread in which the channel's data callbacks are invoked,
        // else they are invoked in the thread calling 'cancelRead'.

    void dataCb(int *numConsumed, int *numNeeded, const DataMsg& msg);
        // This method is invoked in response to a channel pool data callback
        // on the channel id associated with this channel, and invokes the
        // pending read requests until either more data is needed or this
        // channel is closed.  Note that the behavior is undefined if this
        // method is invoked concurrently from multiple threads.

    void blobBasedDataCb(int *numNeeded, bdlmca::Blob *msg);
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
    virtual btlso::IPv4Address localAddress() const;
        // Return the address of the "local" end of the channel.

    virtual btlso::IPv4Address peerAddress() const;
        // Return the address of the "remote" end of the channel.

    int channelId() const;
        // Return the channelPool channel Id associated with this channel.

    ChannelPool *channelPool();
        // Return a pointer to the channelPool object associated with the
        // channel.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // ------------------------------
                       // class ChannelPoolChannel
                       // ------------------------------

// ACCESSORS
inline
int ChannelPoolChannel::channelId() const
{
    return d_channelId;
}

inline
ChannelPool *ChannelPoolChannel::channelPool()
{
    return d_channelPool_p;
}
}  // close package namespace

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
