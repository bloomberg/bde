// btls5_detailedstatus.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btls5_detailedstatus.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btls5_detailedstatus_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace btls5 {

                            // --------------------
                            // class DetailedStatus
                            // --------------------

// ACCESSORS

                                  // Aspects

bsl::ostream& DetailedStatus::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("description", d_description);
    if (address().port()) {
        printer.printAttribute("address", d_address);
    }
    printer.end();

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& btls5::operator<<(bsl::ostream&         stream,
                                const DetailedStatus& object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.description());
    if (object.address().port()) {
        stream << " [";
        printer.printValue(object.address().hostname());
        printer.printValue(object.address().port());
        stream << " ]";
    }
    printer.end();

    return stream;
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
