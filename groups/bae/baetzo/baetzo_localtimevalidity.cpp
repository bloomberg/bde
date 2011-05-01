// baetzo_localtimevalidity.cpp                                       -*-C++-*-
#include <baetzo_localtimevalidity.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_localtimevalidity_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                     // -------------------------------
                     // struct baetzo_LocalTimeValidity
                     // -------------------------------

// CLASS METHODS
bsl::ostream& baetzo_LocalTimeValidity::print(
                                 bsl::ostream&                  stream,
                                 baetzo_LocalTimeValidity::Enum value,
                                 int                            level,
                                 int                            spacesPerLevel)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << baetzo_LocalTimeValidity::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *baetzo_LocalTimeValidity::toAscii(
                                          baetzo_LocalTimeValidity::Enum value)
{
#define CASE(X) case(BAETZO_ ## X): return #X;

    switch (value) {
      CASE(VALID_UNIQUE)
      CASE(VALID_AMBIGUOUS)
      CASE(INVALID)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
