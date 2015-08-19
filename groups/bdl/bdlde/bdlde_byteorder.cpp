// bdlde_byteorder.cpp                                                -*-C++-*-

#include <bdlde_byteorder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_byteorder_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace bdlde {
                        // ----------------------
                        // struct ByteOrder
                        // ----------------------

// CLASS METHODS
bsl::ostream& ByteOrder::print(bsl::ostream&  stream,
                              ByteOrder::Enum value,
                              int             level,
                              int             spacesPerLevel)
{
    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << ByteOrder::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *ByteOrder::toAscii(ByteOrder::Enum value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(LITTLE_ENDIAN)
      CASE(BIG_ENDIAN)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
