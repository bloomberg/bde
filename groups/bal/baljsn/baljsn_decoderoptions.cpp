// baljsn_decoderoptions.cpp                                          -*-C++-*-
#include <baljsn_decoderoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_decoderoptions_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {

namespace baljsn {
                      // ---------------------------
                      // class DecoderOptions
                      // ---------------------------

// ACCESSORS

                                  // Aspects

bsl::ostream& DecoderOptions::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("maxDepth",            d_maxDepth);
    printer.printAttribute("skipUnknownElements", d_skipUnknownElements);
    printer.end();

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bsl::ostream& baljsn::operator<<(bsl::ostream&                stream,
                         const DecoderOptions& object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.maxDepth());
    printer.printValue(object.skipUnknownElements());
    printer.end();

    return stream;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
