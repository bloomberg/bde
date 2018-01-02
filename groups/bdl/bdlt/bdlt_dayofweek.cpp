// bdlt_dayofweek.cpp                                                 -*-C++-*-
#include <bdlt_dayofweek.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_dayofweek_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>

namespace BloombergLP {

BSLMF_ASSERT(bdlt::DayOfWeek::e_SUN < bdlt::DayOfWeek::e_SAT);

namespace bdlt {

                            // ---------------
                            // class DayOfWeek
                            // ---------------

// CLASS METHODS
bsl::ostream& DayOfWeek::print(bsl::ostream&   stream,
                               DayOfWeek::Enum value,
                               int             level,
                               int             spacesPerLevel)
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);  // 'true' -> suppress '['
    stream << toAscii(value);
    printer.end(true);    // 'true' -> suppress ']'

    return stream;
}

const char *DayOfWeek::toAscii(Enum dayOfWeek)
{
#define CASE(X) case(e_ ## X): return #X

    switch(dayOfWeek) {
        // Strange indentation here is required to pacify bde_verify in its
        // current incarnation.

        CASE(SUN);                                                    // RETURN
        CASE(MON);                                                    // RETURN
        CASE(TUE);                                                    // RETURN
        CASE(WED);                                                    // RETURN
        CASE(THU);                                                    // RETURN
        CASE(FRI);                                                    // RETURN
        CASE(SAT);                                                    // RETURN
      default: {
        return "(* Unknown Enumerator *)";                            // RETURN
      } break;
    }

#undef CASE
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
