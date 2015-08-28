// baljsn_encoderoptions.cpp                                          -*-C++-*-
#include <baljsn_encoderoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_encoderoptions_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace baljsn {

                            // --------------------
                            // class EncoderOptions
                            // --------------------

// ACCESSORS

                                  // Aspects

bsl::ostream& EncoderOptions::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("initialIndentLevel", d_initialIndentLevel);
    printer.printAttribute("spacesPerLevel",     d_spacesPerLevel);
    printer.printAttribute("encodingStyle",      d_encodingStyle);
    printer.printAttribute("encodeEmptyArrays",  d_encodeEmptyArrays);
    printer.printAttribute("encodeNullElements", d_encodeNullElements);
    printer.end();

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bsl::ostream& baljsn::operator<<(bsl::ostream&         stream,
                                 const EncoderOptions& object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.initialIndentLevel());
    printer.printValue(object.spacesPerLevel());
    printer.printValue(object.encodingStyle());
    printer.printValue(object.encodeEmptyArrays());
    printer.printValue(object.encodeNullElements());
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
