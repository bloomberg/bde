// baljsn_convertfromjsonoptions.cpp                                  -*-C++-*-
#include <baljsn_convertfromjsonoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_convertfromjsonoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace baljsn {

                        // ----------------------------
                        // class ConvertFromJsonOptions
                        // ----------------------------

// CLASS DATA

const int ConvertFromJsonOptions::DEFAULT_INITIALIZER_MAX_DEPTH = 512;

// ACCESSORS

bsl::ostream& ConvertFromJsonOptions::print(bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("maxDepth",            d_maxDepth);
    printer.printAttribute("skipUnknownElements", d_skipUnknownElements);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
