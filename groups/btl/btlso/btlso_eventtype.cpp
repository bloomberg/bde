// btlso_eventtype.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_eventtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_eventtype_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace btlso {

                            // ----------------
                            // struct EventType
                            // ----------------

// CLASS METHODS
bsl::ostream& EventType::print(bsl::ostream&   stream,
                               EventType::Type value,
                               int             level,
                               int             spacesPerLevel)
{
    bslim::Printer printer(&stream, level, spacesPerLevel);

    printer.start(true);
    stream << toAscii(value);
    printer.end(true);

    return stream;
}

const char *EventType::toAscii(EventType::Type value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(ACCEPT)
      CASE(CONNECT)
      CASE(READ)
      CASE(WRITE)
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
