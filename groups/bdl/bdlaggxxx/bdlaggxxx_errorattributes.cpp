// bdlaggxxx_errorattributes.cpp                                           -*-C++-*-

#include <bdlaggxxx_errorattributes.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlaggxxx_errorattributes_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace bdlaggxxx {
                     // --------------------------
                     // class ErrorAttributes
                     // --------------------------

// ACCESSORS
bsl::ostream& ErrorAttributes::print(bsl::ostream& stream,
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
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
