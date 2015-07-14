// bdlb_bigendian.cpp                                                -*-C++-*-
#include <bdlb_bigendian.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlb_bigendian_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bslmf_assert.h>
#include <bsls_types.h>

#include <bsl_ostream.h>

namespace BloombergLP {

// COMPILE-TIME ASSERTIONS

BSLMF_ASSERT(8 == sizeof(bsls::Types::Int64));
BSLMF_ASSERT(8 == sizeof(bsls::Types::Uint64));
BSLMF_ASSERT(4 == sizeof(int));
BSLMF_ASSERT(2 == sizeof(short));

namespace bdlb {
                         // --------------------------
                         // class BigEndianInt16
                         // --------------------------

// ACCESSORS
bsl::ostream& BigEndianInt16::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;
    }

    if (level > 0) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    // We need to cast the return value to ensure proper formatting since the
    // return of 'BSLS_BYTEORDER_NTOHS' is unsigned.

    stream << static_cast<short>(BSLS_BYTEORDER_NTOHS(d_value));

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
    return stream;
}

                         // ---------------------------
                         // class BigEndianUint16
                         // ---------------------------

// ACCESSORS
bsl::ostream& BigEndianUint16::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;
    }

    if (level > 0) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    stream << BSLS_BYTEORDER_NTOHS(d_value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
    return stream;
}

                         // --------------------------
                         // class BigEndianInt32
                         // --------------------------

// ACCESSORS
bsl::ostream& BigEndianInt32::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;
    }

    if (level > 0) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    // We need to cast the return value to ensure proper formatting since the
    // return of 'BSLS_BYTEORDER_NTOHL' is unsigned.

    stream << static_cast<int>(BSLS_BYTEORDER_NTOHL(d_value));

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
    return stream;
}

                         // ---------------------------
                         // class BigEndianUint32
                         // ---------------------------

// ACCESSORS
bsl::ostream& BigEndianUint32::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;
    }

    if (level > 0) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    stream << BSLS_BYTEORDER_NTOHL(d_value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
    return stream;
}

                         // --------------------------
                         // class BigEndianInt64
                         // --------------------------

// ACCESSORS
bsl::ostream& BigEndianInt64::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;
    }

    if (level > 0) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    // We need to cast the return value to ensure proper formatting since the
    // return of 'BSLS_BYTEORDER_HOST_U64_TO_BE' is unsigned.

    stream << static_cast<bsls::Types::Int64>
                                      (BSLS_BYTEORDER_HOST_U64_TO_BE(d_value));

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
    return stream;
}

                         // ---------------------------
                         // class BigEndianUint64
                         // ---------------------------

// ACCESSORS
bsl::ostream& BigEndianUint64::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;
    }

    if (level > 0) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    stream << BSLS_BYTEORDER_HOST_U64_TO_BE(d_value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
    return stream;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
