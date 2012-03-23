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
                        // Aspects

bsl::ostream& baet_LocalDatetime::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("datetimeTz", d_datetimeTz);
    printer.printAttribute("timeZoneId", d_timeZoneId.c_str());
    printer.end();

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const baet_LocalDatetime& localDatetime)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(localDatetime.datetimeTz());
    printer.printValue(localDatetime.timeZoneId().c_str());
    printer.end();

    return stream;
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
