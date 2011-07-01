// baetzo_localtimeperiod.cpp                                         -*-C++-*-
#include <baetzo_localtimeperiod.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_localtimeperiod_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                        // ----------------------------
                        // class baetzo_LocalTimePeriod
                        // ----------------------------

                        // Aspects

// ACCESSORS
bsl::ostream& baetzo_LocalTimePeriod::print(bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.print(d_descriptor,   "descriptor");
    printer.print(d_utcStartTime, "utcStartTime");
    printer.print(d_utcEndTime,   "utcEndTime");
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
