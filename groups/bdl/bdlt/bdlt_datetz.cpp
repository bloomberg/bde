// bdlt_datetz.cpp                                                    -*-C++-*-
#include <bdlt_datetz.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_datetz_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslmf_assert.h>

#include <bsl_cstdio.h>    // 'sprintf'
#include <bsl_ostream.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace bdlt {

// 'DateTz' is trivially copyable only if 'Date' is also trivially copyable.
// In the header we have stated unconditionally that 'DateTz' is trivially
// copyable, so we assert our assumption about 'Date', as a sanity check.

BSLMF_ASSERT(bsl::is_trivially_copyable<Date>::value);

                             // ------------
                             // class DateTz
                             // ------------

// ACCESSORS
bsl::ostream& DateTz::print(bsl::ostream& stream,
                            int           level,
                            int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    // Write to a temporary stream having width 0 in case the caller has done
    // something like:
    //..
    //  os << bsl::setw(20) << myDateTz;
    //..
    // The user-specified width will be effective when 'buffer' is written to
    // 'stream' (below).

    bsl::ostringstream oss;

    oss << localDate();

    const char sign    = d_offset < 0 ? '-' : '+';
    const int  minutes = '-' == sign ? -d_offset : d_offset;
    const int  hours   = minutes / 60;

    // space usage: +-  hh  mm  nil

    const int offset_size = 1 + 2 + 2 + 1;
    char offsetBuffer[offset_size];

    // Use only 2 digits for 'hours'.

    if (hours < 100) {
        bsl::sprintf(offsetBuffer, "%c%02d%02d", sign, hours, minutes % 60);
    }
    else {
        bsl::sprintf(offsetBuffer, "%cXX%02d", sign, minutes % 60);
    }

    oss << offsetBuffer;

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);  // 'true' -> suppress '['
    stream << oss.str();
    printer.end(true);    // 'true' -> suppress ']'

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
