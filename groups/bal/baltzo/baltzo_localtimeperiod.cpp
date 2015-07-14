// baltzo_localtimeperiod.cpp                                         -*-C++-*-
#include <baltzo_localtimeperiod.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_localtimeperiod_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace baltzo {
                        // ----------------------------
                        // class LocalTimePeriod
                        // ----------------------------

                        // Aspects

// ACCESSORS
bsl::ostream& LocalTimePeriod::print(bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("descriptor",   d_descriptor);
    printer.printAttribute("utcStartTime", d_utcStartTime);
    printer.printAttribute("utcEndTime",   d_utcEndTime);
    printer.end();

    return stream;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
