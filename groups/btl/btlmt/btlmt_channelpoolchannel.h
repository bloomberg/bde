// btlmt_channelpoolchannel.h                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLMT_CHANNELPOOLCHANNEL
#define INCLUDED_BTLMT_CHANNELPOOLCHANNEL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a concrete implementation of 'btlmt::AsyncChannel'
//
//@CLASSES:
//  btlmt::ChannelPoolChannel: implementation of 'btlmt::AsyncChannel'
//
//@SEE_ALSO: btlmt_asyncchannel
//
//@DESCRIPTION: This component provides a class, 'btlmt::ChannelPoolChannel',
// that is a concrete implementation of the 'btlmt::AsyncChannel' protocol, and
// allows asynchronous read and write operations over TCP/IP communication
// channels.  This class is intended to be used with 'btlmt_sessionpool' and
// will rarely be used stand-alone.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a Secure Channel Type
///- - - - - - - - - - - - - - - - - - - - - - -
// This 'class', 'btlmt::ChannelPoolChannel', provides a concrete
// implementation of the 'btlmt::AsyncChannel' protocol for a network
// connection type that once connected to the peer allows asynchronous read and
// write operations.
//
// In this usage example we will implement a secure channel type,
// 'my_SecureChannel' that encrypts data written by the user and decrypts
// incoming data before providing it to the user.  This type uses
// 'btlmt::ChannelPoolChannel' to perform the read and write on the underlying
// connection while implementing encryption/decryption on top of it.  For the
// brevity of this example, we will elide some portions of the class
// implementation including error checking for connection closure and
// synchronizing access to a 'my_SecureChannel' across multiple threads.
//
// First, we provide the class definition:
//..
//  class my_SecureChannel : public btlmt::AsyncChannel {
//      // This 'class' provides a concrete implementation of
//      // 'btlmt::AsyncChannel'.
//
//      // DATA
//      btlmt::ChannelPoolChannel *d_channel_p;
//      btlb::BlobBufferFactory   *d_blobFactory_p;
//      btlb::Blob                 d_decryptedBlob;
//
//    public:
//..
// Next, we specify the interface of this class:
//..
//      // CREATORS
//      my_SecureChannel(btlmt::ChannelPoolChannel *channel,
//                       btlb::BlobBufferFactory   *blobFactory);
//          // Create a 'my_SecureChannel' concrete implementation reading from
//          // and writing to the channel referenced by the specified
//          // 'channelPoolChannel'.
//
//      virtual ~my_SecureChannel();
//          // Destroy this channel.
//
//      // MANIPULATORS
//      virtual int read(int                          numBytes,
//                       const BlobBasedReadCallback& readCallback);
//          // Initiate an asynchronous read operation on this channel, or
//          // append this request to the currently pending requests if an
//          // asynchronous read operation was already initiated.  When at
//          // least the specified 'numBytes' of data are available after all
//          // previous requests have been processed, if any, the specified
//          // 'readCallback' will be invoked (with
//          // 'btlmt::AsyncChannel::e_SUCCESS').  Return 0 on success, and a
//          // non-zero value otherwise.  On error, the return value *may*
//          // equal to one of the enumerators in 'ChannelStatus::Enum'.
//
//      virtual int timedRead(int                          numBytes,
//                            const bsls::TimeInterval&    timeOut,
//                            const BlobBasedReadCallback& readCallback);
//          // Initiate an asynchronous timed read operation on this channel,
//          // or append this request to the currently pending requests if an
//          // asynchronous read operation was already initiated, with an
//          // associated specified absolute 'timeOut'.  When at least the
//          // specified 'numBytes' of data are available after all previous
//          // requests have been processed, if any, or when the 'timeOut' is
//          // reached, the specified 'readCallback' will be invoked (with
//          // either 'btlmt::AsyncChannel::e_SUCCESS' or
//          // 'btlmt::AsyncChannel::e_TIMEOUT', respectively).  Return 0 on
//          // success, and a non-zero value otherwise.  On error, the return
//          // value *may* equal to one of the enumerators in
//          // 'ChannelStatus::Enum'.
//
//      virtual int write(const btlb::Blob& blob,
//                        int               highWaterMark = INT_MAX);
//          // Enqueue the specified 'blob' message to be written to this
//          // channel.  Optionally provide 'highWaterMark' to specify the
//          // maximum data size that can be enqueued.  If 'highWaterMark' is
//          // not specified then 'INT_MAX' is used.  Return 0 on success, and
//          // a non-zero value otherwise.  On error, the return value *may*
//          // equal to one of the enumerators in 'ChannelStatus::Enum'.  Note
//          // that success does not imply that the data has been written or
//          // will be successfully written to the underlying stream used by
//          // this channel.  Also note that in addition to 'highWatermark'
//          // the enqueued portion must also be less than a high watermark
//          // value supplied at the construction of this channel for the
//          // write to succeed.
//
//      virtual int setSocketOption(int option, int level, int value);
//          // Set the specified 'option' (of the specified 'level') socket
//          // option on this channel to the specified 'value'.  Return 0 on
//          // success and a non-zero value otherwise.  (See
//          // 'btlso_socketoptutil' for the list of commonly supported
//          // options.)
//
//      virtual void cancelRead();
//          // Cancel all pending 'read' or 'timedRead' requests, and invoke
//          // their read callbacks with a 'btlmt::AsyncChannel::e_CANCELED'
//          // status.  Note that if the channel is active, the read callbacks
//          // are invoked in the thread in which the channel's data callbacks
//          // are invoked, else they are invoked in the thread calling
//          // 'cancelRead'.
//
//      virtual void close();
//          // Shutdown this channel, and cancel all pending requests.  Note
//          // that this call will result in the shutdown of the channel pool
//          // channel associated with the channel, and will not invoke the
//          // pending read requests.
//
//      void blobBasedReadCb(
//                     int                                         result,
//                     int                                        *numNeeded,
//                     btlb::Blob                                 *blob,
//                     int                                         channelId,
//                     btlmt::AsyncChannel::BlobBasedReadCallback  callback);
//          // This method is invoked in response to a blob based channel pool
//          // channel data callback on 'channelId'.
//
//      // ACCESSORS
//      virtual btlso::IPv4Address localAddress() const;
//          // Return the address of the "local" end of the channel.
//
//      virtual btlso::IPv4Address peerAddress() const;
//          // Return the address of the "remote" end of the channel.
//  };
//..
// Then, we provide the function implementations:
//..
//  my_SecureChannel::my_SecureChannel(btlmt::ChannelPoolChannel *channel,
//                                     btlb::BlobBufferFactory   *blobFactory)
//  : d_channel_p(channel)
//  , d_blobFactory_p(blobFactory)
//  , d_decryptedBlob(d_blobFactory_p)
//  {
//  }
//
//  my_SecureChannel::~my_SecureChannel()
//  {
//  }
//
//  int my_SecureChannel::read(int                          numBytes,
//                             const BlobBasedReadCallback& readCallback)
//  {
//      return timedRead(numBytes, bsls::TimeInterval(), readCallback);
//  }
//
//  int my_SecureChannel::timedRead(int                          numBytes,
//                                  const bsls::TimeInterval&    timeOut,
//                                  const BlobBasedReadCallback& readCallback)
//  {
//      btlmt::AsyncChannel::BlobBasedReadCallback callback =
//                     bdlf::BindUtil::bind(&my_SecureChannel::blobBasedReadCb,
//                                          this,
//                                          bdlf::PlaceHolders::_1,
//                                          bdlf::PlaceHolders::_2,
//                                          bdlf::PlaceHolders::_3,
//                                          bdlf::PlaceHolders::_4,
//                                          readCallback);
//
//      return d_channel_p->timedRead(numBytes, timeOut, callback);
//  }
//
//  int my_SecureChannel::write(const btlb::Blob& blob,
//                              int               highWaterMark)
//  {
//      btlb::Blob encryptedBlob(d_blobFactory_p);
//
//      // Encrypt data in 'blob' and load into 'encryptedBlob' (elided for
//      // brevity)
//
//      encrypt(&encryptedBlob, blob);
//
//      return d_channel_p->write(blob, highWaterMark);
//  }
//
//  int my_SecureChannel::setSocketOption(int option, int level, int value)
//  {
//      return d_channel_p->setSocketOption(option, level, value);
//  }
//
//  void my_SecureChannel::cancelRead()
//  {
//      return d_channel_p->cancelRead();
//  }
//
//  void my_SecureChannel::close()
//  {
//      return d_channel_p->close();
//  }
//
//  void my_SecureChannel::blobBasedReadCb(
//                       int                                         result,
//                       int                                        *numNeeded,
//                       btlb::Blob                                 *blob,
//                       int                                         channelId,
//                       btlmt::AsyncChannel::BlobBasedReadCallback  callback)
//  {
//      btlb::Blob decryptedBlob(d_blobFactory_p);
//
//      // Decrypt data from 'blob' and load into 'decryptedBlob' (elided for
//      // brevity)
//
//      decrypt(&decryptedBlob, *blob);
//
//      btlb::BlobUtil::append(&d_decryptedBlob, decryptedBlob);
//
//      callback(result, numNeeded, &d_decryptedBlob, channelId);
//
//      blob->removeAll();
//  }
//
//  // ACCESSORS
//  btlso::IPv4Address my_SecureChannel::localAddress() const
//  {
//      return d_channel_p->localAddress();
//  }
//
//  btlso::IPv4Address my_SecureChannel::peerAddress() const
//  {
//      return d_channel_p->peerAddress();
//  }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLMT_ASYNCCHANNEL
#include <btlmt_asyncchannel.h>
#endif

#ifndef INCLUDED_BTLSO_IPV4ADDRESS
#include <btlso_ipv4address.h>
#endif

#ifndef INCLUDED_BTLB_BLOB
#include <btlb_blob.h>
#endif

#ifndef INCLUDED_BSLMT_LOCKGUARD
#include <bslmt_lockguard.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDPTR
#include <bslma_managedptr.h>
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

namespace bdlma { class ConcurrentPoolAllocator; }

namespace btlmt {

class ChannelPool;

                       // ========================
                       // class ChannelPoolChannel
                       // ========================

class ChannelPoolChannel: public AsyncChannel {
    // This class provides an implementation of the 'AsyncChannel' protocol for
    // 'ChannelPool'-based channels.

    // TYPES
    struct ReadQueueEntry {
        BlobBasedReadCallback d_readCallback;    // read callback

        bsls::TimeInterval    d_timeOut;         // optional read timeout

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
    bslmt::Mutex           d_mutex;                // mutex used to control
                                                   // access to this channel

    volatile bool          d_callbackInProgress;   // indicates when a callback
                                                   // is in progress

    bool                   d_closed;

    ReadQueue              d_readQueue;            // queue of pending read
                                                   // requests.

    bslma::ManagedPtr<btlb::BlobBufferFactory>
                           d_blobBufferFactory_p;  // blob buffer factory used
                                                   // to allocate
                                                   // 'btlb::Blob' objects
                                                   // returned in data callback

    bdlma::ConcurrentPoolAllocator
                          *d_spAllocator_p;        // shared ptr pool

    ChannelPool           *d_channelPool_p;        // held but not owned

    int                    d_nextClockId;          // next clock ID for timer
                                                   // registration

    int                    d_channelId;            // id of associated
                                                   // channelPool channel

    btlso::IPv4Address     d_peerAddress;          // cached peer address

    btlso::IPv4Address     d_localAddress;         // cached local address

    bslma::Allocator      *d_allocator_p;          // allocator (held, not
                                                   // owned)

  private:
    // PRIVATE MANIPULATORS
    int addReadQueueEntry(int                          numBytes,
                          const BlobBasedReadCallback& callback,
                          const bsls::TimeInterval&    timeOut);
        // Add a read queue entry with the specified 'callback' for the
        // specified 'numBytes' and the specified 'timeOut'.  Return 0 on
        // success, and a non-zero value otherwise.  Note that this function
        // assumes that 'd_mutex' is not locked when it is called.

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
        // that the callback will be invoked only once, with either
        // 'btlmt::AsyncChannel::e_SUCCESS' (in the 'dataCallback') or with
        // 'btlmt::AsyncChannel::e_TIMEOUT' or
        // 'btlmt::AsyncChannel::e_CANCELED' (even if both timeout and
        // cancellation happen at the same time as this read callback is
        // currently at the front of the read queue in 'dataCb').

  public:
    // CREATORS
    ChannelPoolChannel(int                             channelId,
                       ChannelPool                    *channelPool,
                       btlb::BlobBufferFactory        *blobBufferFactory,
                       bdlma::ConcurrentPoolAllocator *spAllocator,
                       bslma::Allocator               *basicAllocator = 0);
        // Create a 'AsyncChannel' concrete implementation reading from and
        // writing to the channel referenced by the specified 'channelId' in
        // the specified 'channelPool', using the specified 'blobBufferFactory'
        // and the specified 'spAllocator' to supply memory to the data
        // messages.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  If 'blobBufferFactory' is 0, create a
        // 'btlb::PooledBlobBufferFactory' object internally using
        // 'basicAllocator' or the currently installed default allocator if
        // 'basicAllocator' is 0.

    virtual ~ChannelPoolChannel();
        // Destroy this channel.

    // MANIPULATORS
    virtual int read(int numBytes, const BlobBasedReadCallback&  readCallback);
        // Initiate an asynchronous read operation on this channel, or append
        // this request to the currently pending requests if an asynchronous
        // read operation was already initiated.  When at least the specified
        // 'numBytes' of data are available after all previous requests have
        // been processed, if any, the specified 'readCallback' will be invoked
        // (with 'btlmt::AsyncChannel::e_SUCCESS').  Return 0 on success, and a
        // non-zero value otherwise.  On error, the return value *may* equal to
        // one of the enumerators in 'ChannelStatus::Enum'.

    virtual int timedRead(int                          numBytes,
                          const bsls::TimeInterval&    timeOut,
                          const BlobBasedReadCallback& readCallback);
        // Initiate an asynchronous timed read operation on this channel, or
        // append this request to the currently pending requests if an
        // asynchronous read operation was already initiated, with an
        // associated specified absolute 'timeOut'.  When at least the
        // specified 'numBytes' of data are available after all previous
        // requests have been processed, if any, or when the 'timeOut' is
        // reached, the specified 'readCallback' will be invoked (with either
        // 'btlmt::AsyncChannel::e_SUCCESS' or
        // 'btlmt::AsyncChannel::e_TIMEOUT', respectively).  Return 0 on
        // success, and a non-zero value otherwise.  On error, the return value
        // *may* equal to one of the enumerators in 'ChannelStatus::Enum'.

    virtual int write(const btlb::Blob& blob,
                      int               highWaterMark = INT_MAX);
        // Enqueue the specified 'blob' message to be written to this channel.
        // Optionally provide 'highWaterMark' to specify the maximum data size
        // that can be enqueued.  If 'highWaterMark' is not specified then
        // 'INT_MAX' is used.  Return 0 on success, and a non-zero value
        // otherwise.  On error, the return value *may* equal to one of the
        // enumerators in 'ChannelStatus::Enum'.  Note that success does not
        // imply that the data has been written or will be successfully written
        // to the underlying stream used by this channel.  Also note that in
        // addition to 'highWatermark' the enqueued portion must also be less
        // than a high watermark value supplied at the construction of this
        // channel for the write to succeed.

    virtual int setSocketOption(int option, int level, int value);
        // Set the specified 'option' (of the specified 'level') socket option
        // on this channel to the specified 'value'.  Return 0 on success and a
        // non-zero value otherwise.  (See 'btlso_socketoptutil' for the list
        // of commonly supported options.)

    virtual void cancelRead();
        // Cancel all pending 'read' or 'timedRead' requests, and invoke their
        // read callbacks with a 'btlmt::AsyncChannel::e_CANCELED' status.
        // Note that if the channel is active, the read callbacks are invoked
        // in the thread in which the channel's data callbacks are invoked,
        // else they are invoked in the thread calling 'cancelRead'.

    virtual void close();
        // Shutdown this channel, and cancel all pending requests.  Note that
        // this call will result in the shutdown of the channel pool channel
        // associated with the channel, and will not invoke the pending read
        // requests.

    void blobBasedDataCb(int *numNeeded, btlb::Blob *msg);
        // This method is invoked in response to a blob based channel pool data
        // callback on the channel id associated with this channel, and invokes
        // the pending read requests until either more data is needed or this
        // channel is closed.  Note that the behavior is undefined if this
        // method is invoked concurrently from multiple threads.

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

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // ------------------------
                       // class ChannelPoolChannel
                       // ------------------------

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
