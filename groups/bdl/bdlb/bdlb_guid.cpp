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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2015
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
