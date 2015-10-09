// btlmt_channelstatus.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLMT_CHANNELSTATUS
#define INCLUDED_BTLMT_CHANNELSTATUS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of channel status codes.
//
//@CLASSES:
//   btlmt::ChannelStatus:  namespace for channel status codes
//
//@SEE_ALSO:  btlmt_asyncchannel btlmt_channelpool btlmt_channelpoolchannel
//
//@DESCRIPTION: This component provides namespace, 'btlmt::ChannelStatus', for
// the 'enum' type listing the possible return codes for methods that operate
// on a channel.
//
///Enumerators
///-----------
//..
//  Name                      Descriptions
//  ---------------------     -------------------------------------------------
//  BTEMT_SUCCESS             If the request had been a write request, then the
//                            request was successfully enqueued or has been
//                            successfully written synchronously.  If the
//                            request had been a read request, then the request
//                            was successfully enqueued.
//
//  BTEMT_CACHE_OVERFLOW      The write request failed because the existing
//                            write cache size (not including the size of the
//                            message being written) is greater than the write
//                            cache high-water mark.  Note that this status can
//                            only occur when the write cache high-water mark
//                            is modified after construction of the object
//                            managing the channels.
//
//  BTEMT_CACHE_HIGHWATER     The write request failed because the existing
//                            write cache size after the current message gets
//                            enqueued will be greater than the write cache
//                            high-water mark.
//
//  BTEMT_READ_CHANNEL_DOWN   The read request failed because the read part of
//                            the channel had already been closed.
//
//  BTEMT_WRITE_CHANNEL_DOWN  The write request failed because the write part
//                            of the channel had already been closed.
//
//  BTEMT_ENQUEUE_HIGHWATER   The write request failed because the existing
//                            write cache size (not including the size of the
//                            message being written) is greater than the
//                            enqueued cache high-water mark provided as a
//                            function argument.
//
//  BTEMT_UNKNOWN_ID          The write request failed because the channel
//                            identified by an specified id does not exist.
//..
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
//      int write(const bdlmca::Blob& blob);
//          // Enqueue the specified 'blob' message to be written to this
//          // channel.  Return 0 on success, and a non-zero value otherwise.
//          // On error, the return value *may* equal to one of the enumerators
//          // in 'btlmt::ChannelErrorWrite::Enum'.
//
//      //...
//  };
//..
// Further suppose that we have a 'btlb::Blob' object, 'blob', and a
// 'MyChannel' object, 'channel'.
//
// First, we use the 'write' method to write 'blob' into 'channel':
//..
//  int rc = channel.write(blob);
//..
// Now, we compare the return code to the enumerators in
// 'btlmt::ChannelStatus::Enum' to handle the different return conditions:
//..
//  switch(rc) {
//    case btlmt::ChannelStatus::BTEMT_SUCCESS: { // Success
//
//        // ...
//
//    } break;
//
//    // We handle the different types of failures in the cases below.
//
//    case btlmt::ChannelStatus::BTEMT_CACHE_OVERFLOW: {
//
//        // ...
//
//    } break;
//    case btlmt::ChannelStatus::BTEMT_CACHE_HIGHWATER: {
//
//        // ...
//
//    } break;
//    case btlmt::ChannelStatus::BTEMT_WRITE_CHANNEL_DOWN: {
//
//        // ...
//
//    } break;
//    default: { // Handle other failures.
//
//        // ...
//
//    }
//  };
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace btlmt {

                        // ====================
                        // struct ChannelStatus
                        // ====================

struct ChannelStatus {
    // This 'struct' provides a namespace for enumerating the set of named
    // status codes that may be returned by functions that operate on a
    // channel, e.g., 'AsyncChannel::write' and 'AsyncChannel::write'.  See
    // 'Enum' in the TYPES sub-section for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        e_SUCCESS            =  0,  // The write request was successfully
                                    // enqueued or has been successfully
                                    // written synchronously.

        e_CACHE_OVERFLOW     = -1,  // The write request failed because the
                                    // existing write cache size (not including
                                    // the size of the message being written)
                                    // is greater than the write cache
                                    // high-water mark.  Note that this status
                                    // can only occur when the write cache
                                    // high-water mark is modified after
                                    // construction of the object managing the
                                    // channels.

        e_CACHE_HIGHWATER    = -2,  // The write request failed because the
                                    // existing write cache size after the
                                    // current message gets enqueued will be
                                    // greater than the write cache high-water
                                    // mark.

        e_READ_CHANNEL_DOWN  = -6,  // The read request falied because the
                                    // read part of the channel had already
                                    // been closed.

        e_WRITE_CHANNEL_DOWN = -3,  // The write request failed because the
                                    // write part of the channel had
                                    // already been closed.

        e_ENQUEUE_HIGHWATER  = -4,  // The write request failed because the
                                    // existing write cache size (not
                                    // including the size of the message
                                    // being written) is greater than the
                                    // enqueued cache high-water mark
                                    // provided as a function argument.

        e_UNKNOWN_ID         = -5   // The write request failed because the
                                    // channel identified by an specified
                                    // id does not exist.

    };

  public:
    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream&       stream,
                               ChannelStatus::Enum value,
                               int                 level          = 0,
                               int                 spacesPerLevel = 4);
        // Write the string representation of the specified enumeration 'value'
        // to the specified output 'stream', and return a reference to
        // 'stream'.  Optionally specify an initial indentation 'level', whose
        // absolute value is incremented recursively for nested objects.  If
        // 'level' is specified, optionally specify 'spacesPerLevel', whose
        // absolute value indicates the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  See 'toAscii' for
        // what constitutes the string representation of a
        // 'ChannelStatus::Enum' value.

    static const char *toAscii(ChannelStatus::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "BALTZO_" prefix elided.  For
        // example:
        //..
        //  bsl::cut << ChannelStatus::toAscii(ChannelStatus::e_SUCCESS);
        //..
        // will print the following on standard output:
        //..
        //  SUCCESS
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&       stream,
                         ChannelStatus::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'btlmt::ChannelStatus::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  btlmt::ChannelStatus::print(stream, value, 0, -1);
    //..

}  // close package namespace

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------------
                        // struct ChannelStatus
                        // --------------------

// FREE OPERATORS
inline
bsl::ostream& btlmt::operator<<(bsl::ostream&       stream,
                                ChannelStatus::Enum value)
{
    return ChannelStatus::print(stream, value, 0, -1);
}

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
