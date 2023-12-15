// bdljsn_readoptions.cpp                                             -*-C++-*-
#include <bdljsn_readoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdljsn_datumreadoptions_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace bdljsn {

                             // -----------------
                             // class ReadOptions
                             // -----------------

// CONSTANTS
const bool ReadOptions::s_DEFAULT_INITIALIZER_ALLOW_TRAILING_TEXT = false;
const int  ReadOptions::s_DEFAULT_INITIALIZER_MAX_NESTED_DEPTH    = 64;

// CREATORS
ReadOptions::ReadOptions()
: d_allowTrailingText(s_DEFAULT_INITIALIZER_ALLOW_TRAILING_TEXT)
, d_maxNestedDepth   (s_DEFAULT_INITIALIZER_MAX_NESTED_DEPTH)
{
}

// MANIPULATORS
void ReadOptions::reset()
{
    d_allowTrailingText = s_DEFAULT_INITIALIZER_ALLOW_TRAILING_TEXT;
    d_maxNestedDepth    = s_DEFAULT_INITIALIZER_MAX_NESTED_DEPTH;
}

// ACCESSORS

                                  // Aspects

bsl::ostream& ReadOptions::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("allowTrailingText", d_allowTrailingText);
    printer.printAttribute("maxNestedDepth",    d_maxNestedDepth);
    printer.end();
    return stream;
}

}  // close package namespace
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
