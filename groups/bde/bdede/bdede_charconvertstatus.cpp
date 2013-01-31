// bdede_charconvertstatus.cpp                                        -*-C++-*-
#include <bdede_charconvertstatus.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdede_charconvertstatus,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                     // ------------------------------
                     // struct bdede_CharConvertStatus
                     // ------------------------------

// CLASS METHODS
bsl::ostream& bdede_CharConvertStatus::print(
                                  bsl::ostream&                 stream,
                                  bdede_CharConvertStatus::Enum value,
                                  int                           level,
                                  int                           spacesPerLevel)
{
    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << bdede_CharConvertStatus::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *bdede_CharConvertStatus::toAscii(
                                           bdede_CharConvertStatus::Enum value)
{
    switch (value) {
      case BDEDE_INVALID_CHARS_BIT: {
        return  "INVALID_CHARS_BIT";                                  // RETURN
      } break;
      case BDEDE_OUT_OF_SPACE_BIT: {
        return  "OUT_OF_SPACE_BIT";                                   // RETURN
      } break;
    }

    return "(* UNKNOWN *)";
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
