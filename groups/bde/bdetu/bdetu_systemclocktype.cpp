// bdetu_systemclocktype.cpp                                          -*-C++-*-

#include <bdetu_systemclocktype.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdetu_systemclocktype_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                     // ---------------------------
                     // strut bdetu_SystemClockType
                     // ---------------------------

// CLASS METHODS
bsl::ostream& bdetu_SystemClockType::print(
                                    bsl::ostream&               stream,
                                    bdetu_SystemClockType::Enum value,
                                    int                         level,
                                    int                         spacesPerLevel)
{
    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << bdetu_SystemClockType::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *bdetu_SystemClockType::toAscii(bdetu_SystemClockType::Enum value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(REALTIME)
      CASE(MONOTONIC)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
