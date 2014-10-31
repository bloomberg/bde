// bdlt_timetz.cpp                                                    -*-C++-*-
#include <bdlt_timetz.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_timetz_cpp,"$Id$ $CSID$")

#include <bdlt_timeunitratio.h>  // for testing only

#include <bslim_printer.h>
#include <bslmf_assert.h>

#include <bsl_complex.h>   // 'bsl::abs'
#include <bsl_iomanip.h>   // 'bsl::setw', 'bsl::setfill'
#include <bsl_ostream.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace bdlt {

// 'TimeTz' is trivially copyable only if 'Time' is also trivially copyable.
// In the header we have stated unconditionally that 'TimeTz' is trivially
// copyable, so we assert our assumption about 'Time', as a sanity check.

BSLMF_ASSERT(bsl::is_trivially_copyable<Time>::value);

                             // ------------
                             // class TimeTz
                             // ------------

// ACCESSORS
bsl::ostream& TimeTz::print(bsl::ostream& stream,
                            int           level,
                            int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    // Write to a temporary stream having width 0 in case the caller has done
    // something like:
    //..
    //  os << bsl::setw(20) << myTimeTz;
    //..
    // The user-specified width will be effective when 'buffer' is written to
    // 'stream' (below).

    bsl::ostringstream oss;
    bslim::Printer printer(&oss, level, spacesPerLevel);
    printer.start(true);  // 'true' -> suppress '['

    oss << localTime();

    const char sign    = d_offset < 0 ? '-' : '+';
    const int  minutes = bsl::abs(d_offset) % 60;
    const int  hours   = bsl::abs(d_offset) / 60;

    // Show invalid values for 'hours' to aid debugging (DRQS 12693813).

    oss << bsl::setfill('0');
    if (hours < 100) {
        oss << sign << bsl::setw(2) << hours << bsl::setw(2) << minutes;
    }
    else {
        oss << sign << "XX"                  << bsl::setw(2) << minutes;
    }

    printer.end(true);    // 'true' -> suppress ']'

    stream << oss.str();

    return stream;
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
