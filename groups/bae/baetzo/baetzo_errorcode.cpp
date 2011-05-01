// baetzo_errorcode.cpp                                               -*-C++-*-

#include <baetzo_errorcode.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_errorcode_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                     // -----------------------
                     // struct baetzo_ErrorCode
                     // -----------------------

// CLASS METHODS
bsl::ostream& baetzo_ErrorCode::print(bsl::ostream&          stream,
                                      baetzo_ErrorCode::Enum value,
                                      int                    level,
                                      int                    spacesPerLevel)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << baetzo_ErrorCode::toAscii(value);

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

const char *baetzo_ErrorCode::toAscii(baetzo_ErrorCode::Enum value)
{
#define CASE(X) case(BAETZO_ ## X): return #X;

    switch (value) {
      CASE(UNSUPPORTED_ID)
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
