// baetzo_dstpolicy.cpp                                               -*-C++-*-

#include <baetzo_dstpolicy.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_dstpolicy_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                     // -----------------------
                     // struct baetzo_DstPolicy
                     // -----------------------

// CLASS METHODS
bsl::ostream& baetzo_DstPolicy::print(bsl::ostream&          stream,
                                      baetzo_DstPolicy::Enum value,
                                      int                    level,
                                      int                    spacesPerLevel)
{
    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << baetzo_DstPolicy::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *baetzo_DstPolicy::toAscii(baetzo_DstPolicy::Enum value)
{
#define CASE(X) case(BAETZO_ ## X): return #X;

    switch (value) {
      CASE(DST)
      CASE(STANDARD)
      CASE(UNSPECIFIED)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
