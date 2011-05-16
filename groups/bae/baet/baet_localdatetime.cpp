// baet_localdatetime.cpp                                             -*-C++-*-
#include <baet_localdatetime.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baet_localdatetime_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

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

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.print(d_datetimeTz,         "datetimeTz");
    printer.print(d_timeZoneId.c_str(), "timeZoneId");
    printer.end();

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
