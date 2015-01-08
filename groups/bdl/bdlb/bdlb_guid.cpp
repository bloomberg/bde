// bdlb_guid.cpp                                                      -*-C++-*-
#include <bdlb_guid.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_guid_cpp,"$Id$ $CSID$")


#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_ostream.h>

#include <bslim_printer.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>

namespace BloombergLP {
namespace bdlb {

bsl::ostream& Guid::print(bsl::ostream& stream,
                          int           level,
                          int           spacesPerLevel) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(stream.bad())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);

    printer.start();

    if (spacesPerLevel < 0) {
        stream << " ";
    }

    for (bsl::size_t i = 0; i < k_GUID_NUM_BYTES; ++i) {
        // split in conventional UUID style
        static const char        hexdig[] = "0123456789abcdef";
        static const char *const seps[k_GUID_NUM_BYTES] = {
            "", "", "", "-", "", "-", "", "-", "", "-", "", "", "", "", "", ""
        };
        stream << hexdig[d_buffer[i] >>  4]
               << hexdig[d_buffer[i] & 0xF]
               << seps[i];
    }

    if (0 <= spacesPerLevel) {
        stream << '\n';
    }

    printer.end();

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bool bdlb::operator<(const bdlb::Guid& lhs, const bdlb::Guid& rhs)
{
    return bsl::memcmp(&lhs[0], &rhs[0], bdlb::Guid::k_GUID_NUM_BYTES) < 0;
}

bool bdlb::operator>(const bdlb::Guid& lhs, const bdlb::Guid& rhs)
{
    return bsl::memcmp(&lhs[0], &rhs[0], bdlb::Guid::k_GUID_NUM_BYTES) > 0;
}

bool bdlb::operator<=(const bdlb::Guid& lhs, const bdlb::Guid& rhs)
{
    return bsl::memcmp(&lhs[0], &rhs[0], bdlb::Guid::k_GUID_NUM_BYTES) <= 0;
}

bool bdlb::operator>=(const bdlb::Guid& lhs, const bdlb::Guid& rhs)
{
    return bsl::memcmp(&lhs[0], &rhs[0], bdlb::Guid::k_GUID_NUM_BYTES) >= 0;
}

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
