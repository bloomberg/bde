// baejsn_encoderoptions.cpp                                          -*-C++-*-
#include <baejsn_encoderoptions.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_encoderoptions_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                      // ---------------------------
                      // class baejsn_EncoderOptions
                      // ---------------------------

// ACCESSORS

                                  // Aspects

bsl::ostream& baejsn_EncoderOptions::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("initialIndentLevel", d_initialIndentLevel);
    printer.printAttribute("spacesPerLevel",     d_spacesPerLevel);
    printer.printAttribute("encodingStyle",      d_encodingStyle);
    printer.end();

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baejsn_EncoderOptions& object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.initialIndentLevel());
    printer.printValue(object.spacesPerLevel());
    printer.printValue(object.encodingStyle());
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
