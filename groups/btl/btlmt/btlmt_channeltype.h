// btlmt_channeltype.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLMT_CHANNELTYPE
#define INCLUDED_BTLMT_CHANNELTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the set of channel types.
//
//@CLASSES:
//   btlmt::ChannelType: namespace for enumerating channel types
//
//@DESCRIPTION: This component provides a namespace, 'btlmt::ChannelType', for
// enumerating channel types, and provides a function that converts each of
// these enumerators to their corresponding string representation.
// Functionality is also provided to write the string form directly to a
// standard 'ostream'.
//
///Enumerators
///-----------
//..
//  Name                    Description
//  ----------------------  ------------------
//  e_LISTENING_CHANNEL     Listening channel
//  e_ACCEPTED_CHANNEL      Accepted channel
//  e_CONNECTING_CHANNEL    Connecting channel
//  e_CONNECTED_CHANNEL     Connected channel
//  e_IMPORTED_CHANNEL      Imported channel
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// First, create a variable 'channelType' of type 'btlmt::ChannelType::Value'
// and initialize it to the value
// 'btlmt::ChannelType::e_CONNECTED_CHANNEL':
//..
//  btlmt::ChannelType::Value channelType =
//                                     btlmt::ChannelType::e_CONNECTED_CHANNEL;
//..
// Next, store its representation in a variable 'rep' of type 'const char*'.
//..
//  const char *rep = btlmt::ChannelType::toAscii(channelType);
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace btlmt {
                        // ==================
                        // struct ChannelType
                        // ==================

struct ChannelType {
    // This class provides a namespace for enumerating channel type values.

  public:
    // TYPES
    enum Value {
        e_LISTENING_CHANNEL = 1,
        e_ACCEPTED_CHANNEL,
        e_CONNECTING_CHANNEL,
        e_CONNECTED_CHANNEL,
        e_IMPORTED_CHANNEL


    };

    enum {
        // Define 'LENGTH' to be the number of consecutively valued enumerators
        // in the range '[ LISTENING_CHANNEL .. IMPORTED_CHANNEL ]'.

        e_LENGTH = e_IMPORTED_CHANNEL

    };

    // CLASS METHODS
    static const char *toAscii(Value channelType);
        // Return the string representation of the enumerator corresponding to
        // the specified 'channelType'.  This representation corresponds
        // exactly to the enumerator's name (e.g., "LISTENING_CHANNEL").
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, ChannelType::Value rhs);
    // Format to the specified output 'stream' the specified 'rhs' channel type
    // in a string representation matching the enumerator name (e.g.,
    // "LISTENING_CHANNEL"), and return a reference to the modifiable 'stream'.

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
