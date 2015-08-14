// ball_userfieldtype.cpp                                             -*-C++-*-
#include <ball_userfieldtype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_userfieldtype_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace ball {

                     // ----------------------
                     // struct UserFieldType
                     // ----------------------

// CLASS METHODS
bsl::ostream& UserFieldType::print(bsl::ostream&       stream,
                                   UserFieldType::Enum value,
                                   int                 level,
                                   int                 spacesPerLevel)
{
    bslim::Printer printer(&stream, level, spacesPerLevel);

    printer.start(true);
    stream << toAscii(value);
    printer.end(true);

    return stream;
}

const char *UserFieldType::toAscii(UserFieldType::Enum value)
{
    switch (value) {
      case(e_VOID) : return "VOID";                                   // RETURN
      case(e_INT64) : return "INT64";                                 // RETURN
      case(e_DOUBLE) : return "DOUBLE";                               // RETURN
      case(e_STRING) : return "STRING";                               // RETURN
      case(e_DATETIMETZ) : return "DATETIMETZ";                       // RETURN
      default: return "(* UNKNOWN *)";                                // RETURN
    }
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
