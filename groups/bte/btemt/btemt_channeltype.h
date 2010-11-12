// btemt_channeltype.h             -*-C++-*-
#ifndef INCLUDED_BTEMT_CHANNELTYPE
#define INCLUDED_BTEMT_CHANNELTYPE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for representing channel type values.
//
//@CLASSES:
//   btemt_ChannelType: namespace for enumerated channel type values
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a namespace for enumerating
// channel types, and provides a function that converts each of
// these enumerators to its corresponding string representation.
// Functionality is also provided to write the string form directly
// to a standard 'ostream'.
//
///Usage
///-----
// First, create a variable 'channelType' of type 'btemt_ChannelType::Value'
// and initialize it to the value 'btemt_ChannelType::BTEMT_CONNECTED_CHANNEL'.
//..
//  btemt_ChannelType::Value channelType
//                                = btemt_ChannelType::BTEMT_CONNECTED_CHANNEL;
//..
// Next, store its representation in a variable 'rep' of type 'const char*'.
//..
//  const char *rep = btemt_ChannelType::toAscii(channelType);
//  assert(0 == strcmp(rep, "CONNECTED_CHANNEL"));
//..
// Finally, print the value of 'channelType' to 'stdout'.
//..
//  bsl::cout << channelType << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  CONNECTED_CHANNEL
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                        // =======================
                        // class btemt_ChannelType
                        // =======================

struct btemt_ChannelType {
    // This class provides a namespace for enumerating channel type values.

  public:
    // TYPES
    enum Value {
        BTEMT_LISTENING_CHANNEL = 1,
        BTEMT_ACCEPTED_CHANNEL,
        BTEMT_CONNECTING_CHANNEL,
        BTEMT_CONNECTED_CHANNEL,
        BTEMT_IMPORTED_CHANNEL
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , LISTENING_CHANNEL  = BTEMT_LISTENING_CHANNEL
      , ACCEPTED_CHANNEL   = BTEMT_ACCEPTED_CHANNEL
      , CONNECTING_CHANNEL = BTEMT_CONNECTING_CHANNEL
      , CONNECTED_CHANNEL  = BTEMT_CONNECTED_CHANNEL
      , IMPORTED_CHANNEL   = BTEMT_IMPORTED_CHANNEL
#endif
    };

    enum { BTEMT_LENGTH = BTEMT_IMPORTED_CHANNEL };
        // Define 'LENGTH' to be the number of consecutively valued enumerators
        // in the range '[ LISTENING_CHANNEL .. IMPORTED_CHANNEL ]'.

    // CLASS METHODS
    static const char *toAscii(Value channelType);
        // Return the string representation of the enumerator corresponding to
        // the specified 'channelType'.  This representation corresponds
        // exactly to the enumerator's name (e.g., "LISTENING_CHANNEL").
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, btemt_ChannelType::Value rhs);
    // Format to the specified output 'stream' the specified 'rhs' channel type
    // in a string representation matching the enumerator name (e.g.,
    // "LISTENING_CHANNEL"), and return a reference to the modifiable 'stream'.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
