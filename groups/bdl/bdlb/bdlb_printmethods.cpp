// bdlb_printmethods.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_printmethods.h>

#include <bslim_formatguard.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_printmethods_cpp,"$Id$ $CSID$")

#include <bsl_cctype.h>
#include <bsl_ios.h>
#include <bsl_iomanip.h>

namespace {
namespace u {

using namespace BloombergLP;

inline
void printHexChar(bsl::ostream& stream, unsigned char object)
    // Print the specified 'object' in 2 digit hex, preceded by "0x", to the
    // specified 'stream'.
{
    bslim::FormatGuard guard(&stream);

    stream << "0x" << bsl::hex << bsl::setfill('0') << bsl::setw(2) <<
                                                 static_cast<unsigned>(object);
}

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bdlb {

bsl::ostream& PrintMethods::print(bsl::ostream& stream,
                                  char          object,
                                  int           level,
                                  int           spacesPerLevel)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    Print::indent(stream, level, spacesPerLevel);

    if (bsl::isprint(static_cast<unsigned char>(object))) {
        stream << object;
    }
    else {
        u::printHexChar(stream, object);
    }

    if (0 <= spacesPerLevel) {
        stream << '\n';
    }

    return stream;
}

bsl::ostream& PrintMethods::print(bsl::ostream& stream,
                                  unsigned char object,
                                  int           level,
                                  int           spacesPerLevel)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    Print::indent(stream, level, spacesPerLevel);

    u::printHexChar(stream, object);

    if (0 <= spacesPerLevel) {
        stream << '\n';
    }

    return stream;
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
