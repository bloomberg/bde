// bdlde_base64decoderoptions.cpp                                     -*-C++-*-

#include <bdlde_base64decoderoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_decoderoptions_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlde {

                            // --------------------
                            // class DecoderOptions
                            // --------------------

// ACCESSORS
bsl::ostream& Base64DecoderOptions::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("ignoreMode", d_ignoreMode);
    printer.printAttribute("alphabet",   d_alphabet);
    printer.printAttribute("isPadded",   d_isPadded);

    printer.end();
    return stream;
}

}  // close package namespace

// Implementation note: the following functions are defined here rather than
// inline in anticipation of their being rarely used.

// FREE FUNCTIONS
bool bdlde::operator==(const bdlde::Base64DecoderOptions& lhs,
                       const bdlde::Base64DecoderOptions& rhs)
{
    return lhs.ignoreMode() == rhs.ignoreMode()
        && lhs.alphabet()   == rhs.alphabet()
        && lhs.isPadded()   == rhs.isPadded();
}

bool bdlde::operator!=(const bdlde::Base64DecoderOptions& lhs,
                       const bdlde::Base64DecoderOptions& rhs)
{
    return lhs.ignoreMode() != rhs.ignoreMode()
        || lhs.alphabet()   != rhs.alphabet()
        || lhs.isPadded()   != rhs.isPadded();
}

bsl::ostream& bdlde::operator<<(bsl::ostream&                      stream,
                                const bdlde::Base64DecoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
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
