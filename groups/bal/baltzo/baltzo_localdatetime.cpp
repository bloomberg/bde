// baltzo_localdatetime.cpp                                             -*-C++-*-
#include <baltzo_localdatetime.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_localdatetime_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace baltzo {
                           // ------------------------
                           // class LocalDatetime
                           // ------------------------

// ACCESSORS
                        // Aspects

bsl::ostream& LocalDatetime::print(bsl::ostream& stream,
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
}  // close package namespace

// FREE OPERATORS
bsl::ostream& baltzo::operator<<(bsl::ostream&             stream,
                         const LocalDatetime& localDatetime)
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
