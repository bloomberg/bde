// bdljsn_location.cpp                                                -*-C++-*-
#include <bdljsn_location.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdljsn_location_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

#include <bslim_printer.h>

namespace BloombergLP {
namespace bdljsn {

                               // --------------
                               // class Location
                               // --------------

// ACCESSORS

                                  // Aspects

bsl::ostream& Location::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("offset", d_offset);
    printer.end();
    return stream;
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& bdljsn::operator<<(bsl::ostream&           stream,
                                 const bdljsn::Location& object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start(true);                  // suppress left  bracket
    printer.printValue(object.offset());
    printer.end(true);                    // suppress right bracket

    return stream;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
