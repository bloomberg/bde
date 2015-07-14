// btlso_lingeroptions.cpp                                            -*-C++-*-
#include <btlso_lingeroptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_lingeroptions_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

#include <bsls_assert.h>


namespace BloombergLP {

namespace btlso {
                       // -------------------------
                       // class LingerOptions
                       // -------------------------

// ACCESSORS
                                  // Aspects

bsl::ostream& LingerOptions::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("timeout",    d_timeout);
    printer.printAttribute("lingerFlag", d_lingerFlag);
    printer.end();

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bsl::ostream& btlso::operator<<(bsl::ostream&              stream,
                         const LingerOptions& object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.timeout());
    printer.printValue(object.lingerFlag());
    printer.end();

    return stream;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
