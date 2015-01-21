// bdlb_guid.cpp                                                      -*-C++-*-
#include <bdlb_guid.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_guid_cpp,"$Id$ $CSID$")


#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
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

    bsl::ios save((bsl::streambuf *)0);
    save.copyfmt(stream);

    bslim::Printer printer(&stream, level, spacesPerLevel);

    printer.start(true);
    stream << bsl::internal
           << bsl::hex
           << bsl::setfill('0')
           << bsl::setw( 8) << timeLow()                              << '-'
           << bsl::setw( 4) << timeMid()                              << '-'
           << bsl::setw( 4) << timeHiAndVersion()                     << '-'
           << bsl::setw( 4) << (clockSeqHiRes() << 8 | clockSeqLow()) << '-'
           << bsl::setw(12) << node();
    printer.end(true);

    stream.copyfmt(save);

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
