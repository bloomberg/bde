// bdlde_base64alphabet.cpp                                           -*-C++-*-

#include <bdlde_base64alphabet.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_byteorder_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlde {

                          // ---------------------
                          // struct Base64Alphabet
                          // ---------------------

// CLASS METHODS
bsl::ostream& Base64Alphabet::print(bsl::ostream&        stream,
                                    Base64Alphabet::Enum value,
                                    int                  level,
                                    int                  spacesPerLevel)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);  // suppress bracket (here and below)
    stream << Base64Alphabet::toAscii(value);
    printer.end(true);
    return stream;
}

const char *Base64Alphabet::toAscii(Base64Alphabet::Enum value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(BASIC)
      CASE(URL)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& bdlde::operator<<(bsl::ostream&               stream,
                                bdlde::Base64Alphabet::Enum value)
{
    return Base64Alphabet::print(stream, value, 0, -1);
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
