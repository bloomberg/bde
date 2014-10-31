// bdlt_monthofyear.cpp                                               -*-C++-*-
#include <bdlt_monthofyear.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_monthofyear_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlt {

                     // ------------------
                     // struct MonthOfYear
                     // ------------------

// CLASS METHODS
bsl::ostream& MonthOfYear::print(bsl::ostream&     stream,
                                 MonthOfYear::Enum value,
                                 int               level,
                                 int               spacesPerLevel)
{
    bslim::Printer printer(&stream, level, spacesPerLevel);

    printer.start(true);
    stream << toAscii(value);
    printer.end(true);

    return stream;
}

const char *MonthOfYear::toAscii(MonthOfYear::Enum value)
{
    switch (value) {
      case(e_JAN) : return "JAN";                                     // RETURN
      case(e_FEB) : return "FEB";                                     // RETURN
      case(e_MAR) : return "MAR";                                     // RETURN
      case(e_APR) : return "APR";                                     // RETURN
      case(e_MAY) : return "MAY";                                     // RETURN
      case(e_JUN) : return "JUN";                                     // RETURN
      case(e_JUL) : return "JUL";                                     // RETURN
      case(e_AUG) : return "AUG";                                     // RETURN
      case(e_SEP) : return "SEP";                                     // RETURN
      case(e_OCT) : return "OCT";                                     // RETURN
      case(e_NOV) : return "NOV";                                     // RETURN
      case(e_DEC) : return "DEC";                                     // RETURN
      default: return "(* UNKNOWN *)";                                // RETURN
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
