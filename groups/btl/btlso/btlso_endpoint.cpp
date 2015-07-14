// btlso_endpoint.cpp                                                 -*-C++-*-
#include <btlso_endpoint.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_endpoint_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace btlso {
                            // --------------------
                            // class Endpoint
                            // --------------------

// ACCESSORS

                                  // Aspects

bsl::ostream& Endpoint::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("hostname", d_hostname);
    printer.printAttribute("port",     d_port);
    printer.end();

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bsl::ostream& btlso::operator<<(bsl::ostream&         stream,
                         const Endpoint& object)
{
    stream << object.hostname() << ':' << object.port();
    return stream;
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
