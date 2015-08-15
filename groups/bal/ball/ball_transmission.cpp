// ball_transmission.cpp                                              -*-C++-*-
#include <ball_transmission.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_transmission_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

namespace ball {
// PRIVATE CLASS METHODS

void Transmission::print(bsl::ostream&            stream,
                              Transmission::Cause value)
{
    stream << Transmission::toAscii(value);
}

// CLASS METHODS

const char *Transmission::toAscii(Transmission::Cause value)
{
#define CASE(X) case(X): return #X;

    switch(value) {
      case(BAEL_PASSTHROUGH)       : return "PASSTHROUGH";
      case(BAEL_TRIGGER)           : return "TRIGGER";
      case(BAEL_TRIGGER_ALL)       : return "TRIGGER_ALL";
      case(BAEL_MANUAL_PUBLISH)    : return "MANUAL_PUBLISH";
      case(BAEL_MANUAL_PUBLISH_ALL): return "MANUAL_PUBLISH_ALL";
      default: return "(* UNKNOWN *)";
    }

#undef CASE
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
