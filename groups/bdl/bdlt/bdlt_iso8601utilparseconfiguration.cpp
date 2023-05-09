// bdlt_iso8601utilparseconfiguration.cpp                             -*-C++-*-
#include <bdlt_iso8601utilparseconfiguration.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_iso8601utilparseconfiguration_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlt {

                    // -----------------------------------
                    // class Iso8601UtilParseConfiguration
                    // -----------------------------------

// ACCESSORS

                                  // Aspects

bsl::ostream&
Iso8601UtilParseConfiguration::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("basic", basic());
    printer.printAttribute("relaxed", relaxed());
    printer.end();

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& bdlt::operator<<(bsl::ostream&                        stream,
                               const Iso8601UtilParseConfiguration& object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.basic());
    printer.printValue(object.relaxed());
    printer.end();

    return stream;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
