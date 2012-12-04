// btemt_channelerror.h                                               -*-C++-*-
#ifndef INCLUDED_BTEMT_CHANNELERROR
#define INCLUDED_BTEMT_CHANNELERROR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a set of enumerations for the channel status codes.
//
//@CLASSES:
//   btemt::ChannelErrorRead:   namespace for channel read status code 'enum'
//   btemt::ChannelErrorWrite:  namespace for channel write status code 'enum'
//
//@AUTHOR: Chen He (che2)
//
//@SEE ALSO:  btemt_asyncchannel btemt_channelpool btemt_channelpoolchannel
//
//@DESCRIPTION: This component provides namespaces for the following 'enum'
// types listing the possible return codes for methods that operate on a
// channel: (1) 'btemt::ChannelErrorRead:Enum', which enumerates the set of
// status codes return by a channel read operation, and (2)
// 'btemt::ChannelErrorWrite::Enum', which enumerates the set of status codes
// return by a channel write operation.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// Suppose that we have a class 'MyChannel' that defines the following elided
// interface:
//..
//  class MyChannel {
//      // This class represents a channel over which data may be sent and
//      // received.
//
//    public:
//      //...
//      int write(const bcema_Blob& blob);
//          // Enqueue the specified 'blob' message to be written to this
//          // channel.  Return 0 on success, and non-zero value in the
//          // enumeration 'btemt::ChannelErrorWrite::Enum' otherwise.
//
//      //...
//  };
//..
// Further suppose that we have a 'bcema_Blob' object, 'blob', and a
// 'MyChannel' object, 'channel'.
//
// First, we use the 'write' method to write 'blob' into 'channel':
//..
//  int rc = channel.write(blob);
//..
// Now, we compare the return code the enumerators in
// 'btemt::ChannelErrorRead::Enum' to handle the different error conditions:
//..
//  switch(rc) {
//    case btemt::ChannelErrorWrite::BTEMT_SUCCESS:  // Success
//    {
//        // ...
//    } break;
//
//    // We handle the different types of failures in the cases below.
//
//    case btemt::ChannelErrorWrite::BTEMT_CACHE_HIWAT:
//    {
//        // ...
//    } break;
//    case btemt::ChannelErrorWrite::BTEMT_HIT_CACHE_HIWAT:
//    {
//        // ...
//    } break;
//    case btemt::ChannelErrorWrite::BTEMT_CHANNEL_DOWN:
//    {
//        // ...
//    } break;
//    default:  // Handle other failures.
//    {
//        //...
//    }
//  };
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

namespace BloombergLP {
namespace btemt {

                        // ======================
                        // class ChannelErrorRead
                        // ======================

class ChannelErrorRead {
    // This 'struct' provides a namespace for enumerating the set of named
    // status codes that may be returned by functions that read from a channel,
    // e.g., 'btemt_AsyncChannel::read'.

  public:
    // TYPES
    enum Enum {
        BTEMT_SUCCESS        =  0,  // The read request was successfully
                                    // enqeued.

        BTEMT_CHANNEL_CLOSED = -2,  // The read request falied because the
                                    // channel had already been closed.
    };
};

                        // =======================
                        // class ChannelErrorWrite
                        // =======================

class ChannelErrorWrite {
    // This 'struct' provides a namespace for enumerating the set of named
    // status codes that may be returned by functions that write to a channel,
    // e.g., 'btemt_AsyncChannel::write' and 'btemt_ChannelPool::write'.

  public:
    // TYPES
    enum Enum {
        BTEMT_SUCCESS         =  0,  // The write request was successfully
                                     // enqueued or have been successfully
                                     // written synchronously.

        BTEMT_CACHE_HIWAT     = -1,  // The write request failed because the
                                     // existing write cache size (not
                                     // including the size of the message being
                                     // written) is greater than the write
                                     // cache high-watermark.  Note that this
                                     // status can only occur when the write
                                     // cache high-watermark is modified after
                                     // construction of the object managing the
                                     // channels.

        BTEMT_HIT_CACHE_HIWAT = -2,  // The write request failed because the
                                     // existing write cache size after the
                                     // current message gets enqueued will be
                                     // greater than the write cache
                                     // high-watermark.

        BTEMT_CHANNEL_DOWN    = -3,  // The write request failed because the
                                     // write part of the channel is down.

        BTEMT_ENQUEUE_WAT     = -4,  // The write request failed because the
                                     // existing write cache size (not
                                     // including the size of the message being
                                     // written) is greater than the enqueued
                                     // cache high-watermark (specified as
                                     // argument of the function being called).

        BTEMT_UNKNOWN         = -5,  // The write request failed because the
                                     // channel identified by an specified id
                                     // does not exist.
    };
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
