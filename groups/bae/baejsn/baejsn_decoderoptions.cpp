// baejsn_decoderoptions.cpp                                          -*-C++-*-
#include <baejsn_decoderoptions.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_decoderoptions_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                      // ---------------------------
                      // class baejsn_DecoderOptions
                      // ---------------------------

// ACCESSORS

                                  // Aspects

bsl::ostream& baejsn_DecoderOptions::print(bsl::ostream& stream,
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

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baejsn_DecoderOptions& object)
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
