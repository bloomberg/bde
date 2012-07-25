// bcem_aggregateerror.cpp                                            -*-C++-*-

#include <bcem_aggregateerror.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcem_aggregateerror_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {

bsl::ostream& bcem_AggregateError::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("description", d_description.c_str());
    printer.printAttribute("code", d_code);

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
