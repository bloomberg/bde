// bbldc_daycountconvention.cpp                                       -*-C++-*-
#include <bbldc_daycountconvention.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bbldc_daycountconvention_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bbldc {

                        // -------------------------
                        // struct DayCountConvention
                        // -------------------------

// CLASS METHODS
bsl::ostream& DayCountConvention::print(
                                       bsl::ostream&            stream,
                                       DayCountConvention::Enum value,
                                       int                      level,
                                       int                      spacesPerLevel)
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);  // 'true' -> suppress '['
    stream << toAscii(value);
    printer.end(true);    // 'true' -> suppress ']'

    return stream;
}

const char *DayCountConvention::toAscii(Enum convention)
{
#define CASE(X) case(e_ ## X): return #X

    switch (convention) {
      CASE(ACTUAL_360);                                               // RETURN
      CASE(ACTUAL_365_FIXED);                                         // RETURN
      CASE(ISDA_ACTUAL_ACTUAL);                                       // RETURN
      CASE(ISMA_30_360);                                              // RETURN
      CASE(PSA_30_360_EOM);                                           // RETURN
      CASE(SIA_30_360_EOM);                                           // RETURN
      CASE(SIA_30_360_NEOM);                                          // RETURN
      CASE(PERIOD_ICMA_ACTUAL_ACTUAL);                                // RETURN
      CASE(CALENDAR_BUS_252);                                         // RETURN
      default: {
        return "(* Unknown Enumerator *)";                            // RETURN
      } break;
    }

#undef CASE
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
