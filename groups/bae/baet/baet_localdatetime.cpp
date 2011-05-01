// baet_localdatetime.cpp                                             -*-C++-*-
#include <baet_localdatetime.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baet_localdatetime_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                           // ------------------------
                           // class baet_LocalDatetime
                           // ------------------------

// ACCESSORS
bsl::ostream& baet_LocalDatetime::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << "[ ";
    d_datetimeTz.print(stream, level, spacesPerLevel);
    stream << ", "
           << d_timeZoneId
           << " ]";

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
