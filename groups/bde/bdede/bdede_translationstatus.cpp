// bdede_translationstatus.cpp                                        -*-C++-*-
#include <bdede_translationstatus.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdede_translationstatus,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                     // ------------------------------
                     // struct bdede_TranslationStatus
                     // ------------------------------

// CLASS METHODS
bsl::ostream& bdede_TranslationStatus::print(
                                  bsl::ostream&                 stream,
                                  bdede_TranslationStatus::Enum value,
                                  int                           level,
                                  int                           spacesPerLevel)
{
    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << bdede_TranslationStatus::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *bdede_TranslationStatus::toAscii(
                                           bdede_TranslationStatus::Enum value)
{
    switch (value) {
      case BDEDE_INVALID_CHARS_MASK: {
        return  "INVALID_CHARS_MASK";                                 // RETURN
      } break;
      case BDEDE_OUT_OF_SPACE_MASK: {
        return  "OUT_OF_SPACE_MASK";                                  // RETURN
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
