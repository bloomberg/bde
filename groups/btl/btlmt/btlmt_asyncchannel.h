// btlmt_asyncchannel.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLMT_ASYNCCHANNEL
#define INCLUDED_BTLMT_ASYNCCHANNEL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for asynchronous IO operations
//
//@CLASSES:
//  btlmt::AsyncChannel: *pure* protocol for asynchronous IO operations
//
//@SEE_ALSO: btlmt_channelpoolchannel, btlmt_sessionpool
//
//@DESCRIPTION: This component provides a pure protocol, 'btlmt::AsyncChannel',
// for asynchronous read and write operations over TCP/IP communication
// channel.  Concrete implementations of this protocol are intended to be used
// with 'btlmt_sessionpool' and will rarely be used stand-alone.  A concrete
// implementation of this protocol is provided in 'btlmt_channelpoolchannel'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a Concrete Channel Type
///- - - - - - - - - - - - - - - - - - - - - - - -
// This 'class', 'btlmt::AsyncChannel', provides a pure protocol for a network
// connection type that once connected to the peer allows asynchronous reads
// and writes.
//
// In this usage example we will implement a derived implementation of
// 'btlmt::AsyncChannel', 'my_AsyncChannel'.  An object of this type is
// constructed with a 'btlmt::ChannelPool' object and provided a channel id
// that represents this connection in that channel pool object.  For the
// brevity of this example, we will elide some portions of the class
// implementation including error checking for connection closure and
// synchronizing access to a 'my_AsyncChannel' across multiple threads.
//
// First, we provide the class definition:
//..
//  class my_AsyncChannel : public btlmt::AsyncChannel {
//      // This 'class' provides a concrete implementation of
//      // 'btlmt::AsyncChannel'.
//
//..
// Next, we specify an 'Entry' type that stores the data corresponding to a
// read callback:
//..
//      // TYPES
//      struct Entry {
//          // This 'struct' provides a type storing the attributes of a read
//          // callback.
//
//          BlobBasedReadCallback d_readCallback;    // read callback
//
//          bsls::TimeInterval    d_timeOut;         // optional read timeout
//
//          int                   d_numBytesNeeded;  // number of bytes needed
//                                                   // before to invoke the
//                                                   // read callback
//      };
//
//      typedef bsl::vector<Entry> ReadQueue;
//
//..
// Then, we specify the data for this type.  This type stores a
// 'btlmt::ChannelPool', the channelId in that channel pool used to represent
// this connection, and a vector storing all the read callbacks:
//..
//      // DATA
//      int                 d_channelId;
//      btlmt::ChannelPool *d_channelPool_p;
//      ReadQueue           d_entries;
//
//..
// Next, we specify the interface of this class:
//..
//    public:
//      // CREATORS
//      my_AsyncChannel(int                 channelId,
//                      btlmt::ChannelPool *channelPool,
//                      bslma::Allocator   *basicAllocator = 0);
//          // Create a 'my_AsyncChannel' concrete implementation reading from
//          // and writing to the channel referenced by the specified
//          // 'channelId' in the specified 'channelPool'.  Optionally specify
//          // a 'basicAllocator' used to supply memory.  If 'basicAllocator'
//          // is 0, the currently installed default allocator is used.
//
//      virtual ~my_AsyncChannel();
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
//..
// Then, we define a method, 'blobBasedReadCb', that can be invoked by a higher
// level component after reading data from the channel pool held by this object
// on the channel id represented by this object.  Note that the 'read' and
// 'timedRead' methods of this type are asynchronous and they do not directly
// result in any data being read from the network.  Instead, this type relies
// on a higher class (like 'btlmt::SessionPool') for the data reads:
//..
//      void blobBasedReadCb(int *numNeeded, btlb::Blob *msg);
//          // This method is invoked in response to a blob based channel pool
//          // data callback on the channel id associated with this channel,
//          // and invokes the pending read requests until either more data is
//          // needed or this channel is closed.
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
//  my_AsyncChannel::my_AsyncChannel(int                 channelId,
//                                   btlmt::ChannelPool *channelPool,
//                                   bslma::Allocator   *basicAllocator)
//  : d_channelId(channelId)
//  , d_channelPool_p(channelPool)
//  , d_entries(basicAllocator)
//  {
//  }
//
//  my_AsyncChannel::~my_AsyncChannel()
//  {
//      // Cancel future callbacks, but do not invoke them if this channel is
//      // closed.
//
//      cancelRead();
//      d_channelPool_p->shutdown(d_channelId);
//  }
//
//  int my_AsyncChannel::read(int                          numBytes,
//                            const BlobBasedReadCallback& readCallback)
//  {
//      return timedRead(numBytes, bsls::TimeInterval(), readCallback);
//  }
//
//  int my_AsyncChannel::timedRead(int                          numBytes,
//                                 const bsls::TimeInterval&    timeOut,
//                                 const BlobBasedReadCallback& readCallback)
//  {
//      Entry entry;
//      entry.d_numBytesNeeded = numBytes;
//      entry.d_timeOut        = timeOut;
//      entry.d_readCallback   = readCallback;
//
//      if (bsls::TimeInterval(0) != timeOut) {
//          // Register a callback to be invoked when timeout expires (elided
//          // for brevity).
//
//          // . . .
//      }
//      d_entries.push_back(entry);
//  }
//
//  int my_AsyncChannel::write(const btlb::Blob& blob,
//                             int               highWaterMark)
//  {
//      return d_channelPool_p->write(d_channelId, blob, highWaterMark);
//  }
//
//  int my_AsyncChannel::setSocketOption(int option, int level, int value)
//  {
//      return d_channelPool_p->setSocketOption(option,
//                                              level,
//                                              value,
//                                              d_channelId);
//  }
//
//  void my_AsyncChannel::cancelRead()
//  {
//      ReadQueue::iterator iter = d_entries.begin();
//      ReadQueue::iterator end  = d_entries.end();
//
//      int        dummy = 0;
//      btlb::Blob dummyBlob;
//      for (iter = d_entries.begin(); iter != end; ++iter) {
//          BlobBasedReadCallback callback = iter->d_readCallback;
//
//          callback(btlmt::AsyncChannel::e_CANCELED,
//                   &dummy,
//                   &dummyBlob,
//                   d_channelId);
//      }
//
//      d_entries.erase(iter, end);
//  }
//
//  void my_AsyncChannel::close()
//  {
//      d_channelPool_p->shutdown(d_channelId);
//
//      ReadQueue::iterator iter = d_entries.begin();
//      ReadQueue::iterator end  = d_entries.end();
//
//      for (iter = d_entries.begin(); iter != end; ++iter) {
//          if (bsls::TimeInterval(0) != iter->d_timeOut) {
//              // Deregister timeout.  Not shown here.
//
//              // . . .
//          }
//      }
//
//      d_entries.erase(iter, end);
//  }
//
//  void my_AsyncChannel::blobBasedReadCb(int *numNeeded, btlb::Blob *msg)
//  {
//      *numNeeded            = 1;
//      int numBytesAvailable = msg->length();
//
//      while (d_entries.size()
//          && d_entries.front().d_numBytesNeeded <= numBytesAvailable) {
//
//          Entry& entry = d_entries.front();
//
//          int numConsumed = 0;
//          int nNeeded     = 0;
//
//          const BlobBasedReadCallback& callback = entry.d_readCallback;
//          numBytesAvailable = msg->length();
//
//          {
//              callback(e_SUCCESS, &nNeeded, msg, d_channelId);
//              numConsumed = numBytesAvailable - msg->length();
//          }
//
//          assert(0 <= nNeeded);
//          assert(0 <= numConsumed);
//
//          numBytesAvailable -= numConsumed;
//
//          if (nNeeded) {
//              entry.d_numBytesNeeded = nNeeded;
//              if (nNeeded <= numBytesAvailable) {
//                  continue;
//              }
//
//              *numNeeded = nNeeded - numBytesAvailable;
//          }
//          else {
//              d_entries.erase(d_entries.begin());
//              if (!d_entries.size()) {
//                  d_channelPool_p->disableRead(d_channelId);
//              }
//          }
//      }
//  }
//
//  // ACCESSORS
//  btlso::IPv4Address my_AsyncChannel::localAddress() const
//  {
//      btlso::IPv4Address address;
//      d_channelPool_p->getLocalAddress(&address, d_channelId);
//      return address;
//  }
//
//  btlso::IPv4Address my_AsyncChannel::peerAddress() const
//  {
//      btlso::IPv4Address address;
//      d_channelPool_p->getPeerAddress(&address, d_channelId);
//      return address;
//  }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_IPV4ADDRESS
#include <btlso_ipv4address.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

namespace BloombergLP {

namespace bsls { class TimeInterval; }

namespace btlb { class Blob; }

namespace btlmt {

                          // ==================
                          // class AsyncChannel
                          // ==================

class AsyncChannel {
    // This class defines a *pure* protocol for asynchronous IO operations.

  public:
    // PUBLIC TYPES
    enum ReadResult{
        // Result codes passed in read callback

        e_SUCCESS  = 0,   // The operation was successful.

        e_TIMEOUT  = 1,   // The 'timedRead' operation has timed out.

        e_CLOSED   = 2,   // The underlying stream used by this channel
                          // has been closed.

        e_FAILURE  = 3,   // An I/O error occurred during the read
                          // operation.

        e_CANCELED = 4    // The asynchronous read request was canceled.


    };

    typedef bsl::function<void(int, int *, btlb::Blob *, int)>
                                                         BlobBasedReadCallback;
        // A callback of this type is invoked for 'read' and 'timeRead' when
        // either the requested number of bytes are available, or an error
        // occurs.  The callback is invoked with four arguments: (1) an integer
        // result code corresponding to 'ReadResult' that indicates the result
        // of the read operation, and if this code is equal to 'e_SUCCESS', (2)
        // a pointer to an integer value where the callee can indicate how many
        // more bytes are needed to complete the read operation, (3) a
        // modifiable 'btlb::Blob' object containing the payload.  The caller
        // is responsible for taking ownership of a certain number of bytes in
        // the 'btlb::Blob' and readjusting it appropriately.  Note that the
        // read operation is not considered completed until the callee
        // indicates that zero more bytes are needed (argument 2), and (4)
        // channel id.  Also note that the last three arguments are ignored if
        // the first argument is different from 'e_SUCCESS'.
        //
        // A typical function matching this interface might look as follows:
        //..
        //  void myReadCallback(int         result,
        //                      int        *numNeeded,
        //                      btlb::Blob *blob,
        //                      int         channelId);
        //..

    // CREATORS
    virtual ~AsyncChannel();
        // Destroy this channel.

    // MANIPULATORS
    virtual int read(int                          numBytes,
                     const BlobBasedReadCallback& readCallback) = 0;
        // Initiate an asynchronous read operation on this channel, or append
        // this request to the currently pending requests if an asynchronous
        // read operation was already initiated.  When at least the specified
        // 'numBytes' of data are available after all previous requests have
        // been processed, if any, the specified 'readCallback' will be invoked
        // (with 'e_SUCCESS').  Return 0 on success, and a non-zero value
        // otherwise.  On error, the return value *may* equal to one of the
        // enumerators in 'ChannelStatus::Enum'.

    virtual int timedRead(int                          numBytes,
                          const bsls::TimeInterval&    timeout,
                          const BlobBasedReadCallback& readCallback) = 0;
        // Initiate an asynchronous timed read operation on this channel, or
        // append this request to the currently pending requests if an
        // asynchronous read operation was already initiated, with an
        // associated specified absolute 'timeOut'.  When at least the
        // specified 'numBytes' of data are available after all previous
        // requests have been processed, if any, or when the 'timeOut' is
        // reached, the specified 'readCallback' will be invoked (with either
        // 'e_SUCCESS' or 'e_TIMEOUT', respectively).  Return 0 on success, and
        // a non-zero value otherwise.  On error, the return value *may* equal
        // to one of the enumerators in 'ChannelStatus::Enum'.

    virtual int write(const btlb::Blob& blob,
                      int               highWaterMark = INT_MAX) = 0;
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

    virtual int setSocketOption(int option, int level, int value) = 0;
        // Set the specified 'option' (of the specified 'level') socket option
        // on this channel to the specified 'value'.  Return 0 on success and a
        // non-zero value otherwise.  (See 'btlso_socketoptutil' for the list
        // of commonly supported options.)

    virtual void cancelRead() = 0;
        // Cancel all pending 'read' or 'timedRead' requests, and invoke their
        // read callbacks with a 'e_CANCELED' status.  Note that if the channel
        // has not been closed, the read callbacks are invoked in the thread in
        // which the channel's data callbacks are invoked, else they are
        // invoked in the thread calling 'cancelRead'.

    virtual void close() = 0;
        // Shutdown this channel, and cancel all pending 'read' or 'timedRead'
        // requests (but do not invoke them).

    // ACCESSORS
    virtual btlso::IPv4Address localAddress() const = 0;
        // Return the address of the "local" end of this channel.

    virtual btlso::IPv4Address peerAddress() const = 0;
        // Return the address of the "remote" end of this channel.
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
