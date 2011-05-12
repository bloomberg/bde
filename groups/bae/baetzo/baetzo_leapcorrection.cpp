// baetzo_leapcorrection.cpp                                          -*-C++-*-
#include <baetzo_leapcorrection.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_leapcorrection_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

namespace BloombergLP {

                      // ---------------------------
                      // class baetzo_LeapCorrection
                      // ---------------------------

                        // Aspects

// ACCESSORS
bsl::ostream& baetzo_LeapCorrection::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << "[ "
           << d_utcTime
           << " "
           << d_accumulatedCorrectionInSeconds
           << " ]";

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
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
