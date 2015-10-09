// btlmt_channeltype.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlmt_channeltype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlmt_channeltype_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

namespace btlmt {

                     // ------------------
                     // struct ChannelType
                     // ------------------

const char *ChannelType::toAscii(Value channelType)
{
#define CASE(X) case(e_ ## X): return #X

    switch (channelType) {
      CASE(LISTENING_CHANNEL);
      CASE(ACCEPTED_CHANNEL);
      CASE(CONNECTING_CHANNEL);
      CASE(CONNECTED_CHANNEL);
      CASE(IMPORTED_CHANNEL);
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

}  // close package namespace

bsl::ostream& btlmt::operator<<(bsl::ostream& output, ChannelType::Value rhs)
{
    return output << ChannelType::toAscii(rhs);
}

}  // close enterprise namespace

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
