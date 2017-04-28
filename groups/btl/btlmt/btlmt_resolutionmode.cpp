// btlmt_resolutionmode.cpp                                           -*-C++-*-
#include <btlmt_resolutionmode.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlmt_resolutionmode_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                          // ---------------------
                          // struct ResolutionMode
                          // ---------------------

// CLASS METHODS
bsl::ostream& btlmt::ResolutionMode::print(bsl::ostream&        stream,
                                           ResolutionMode::Enum value,
                                           int                  level,
                                           int                  spacesPerLevel)
{
    bslim::Printer printer(&stream, level, spacesPerLevel);

    printer.start(true);
    stream << toAscii(value);
    printer.end(true);

    return stream;
}

const char *btlmt::ResolutionMode::toAscii(ResolutionMode::Enum value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(RESOLVE_ONCE)                                              // RETURN
      CASE(RESOLVE_EACH_TIME)                                         // RETURN
      default: return "(* UNKNOWN *)";                                // RETURN
    }

#undef CASE
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
