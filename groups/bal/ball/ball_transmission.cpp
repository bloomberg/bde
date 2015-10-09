// ball_transmission.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_transmission.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_transmission_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

namespace ball {

// PRIVATE CLASS METHODS
void Transmission::print(bsl::ostream&       stream,
                         Transmission::Cause value)
{
    stream << Transmission::toAscii(value);
}

// CLASS METHODS

const char *Transmission::toAscii(Transmission::Cause value)
{
    switch(value) {
      case(e_PASSTHROUGH)       : return "PASSTHROUGH";
      case(e_TRIGGER)           : return "TRIGGER";
      case(e_TRIGGER_ALL)       : return "TRIGGER_ALL";
      case(e_MANUAL_PUBLISH)    : return "MANUAL_PUBLISH";
      case(e_MANUAL_PUBLISH_ALL): return "MANUAL_PUBLISH_ALL";
      default: return "(* UNKNOWN *)";
    }
}

}  // close package namespace

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
