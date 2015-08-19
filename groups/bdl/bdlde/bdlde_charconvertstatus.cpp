// bdlde_charconvertstatus.cpp                                        -*-C++-*-
#include <bdlde_charconvertstatus.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_charconvertstatus,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlde {

                     // ------------------------
                     // struct CharConvertStatus
                     // ------------------------

// CLASS METHODS
bsl::ostream& CharConvertStatus::print(bsl::ostream&           stream,
                                       CharConvertStatus::Enum value,
                                       int                     level,
                                       int                     spacesPerLevel)
{
    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << CharConvertStatus::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *CharConvertStatus::toAscii(CharConvertStatus::Enum value)
{
    switch (value) {
      case k_INVALID_CHARS_BIT: {
        return  "INVALID_CHARS_BIT";                                  // RETURN
      } break;
      case k_OUT_OF_SPACE_BIT: {
        return  "OUT_OF_SPACE_BIT";                                   // RETURN
      } break;
    }

    return "(* UNKNOWN *)";
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
