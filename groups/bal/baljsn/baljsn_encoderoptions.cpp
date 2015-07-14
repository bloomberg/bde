// baljsn_encoderoptions.cpp                                          -*-C++-*-
#include <baljsn_encoderoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_encoderoptions_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {

namespace baljsn {
                      // ---------------------------
                      // class EncoderOptions
                      // ---------------------------

// ACCESSORS

                                  // Aspects

bsl::ostream& EncoderOptions::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("initialIndentLevel", d_initialIndentLevel);
    printer.printAttribute("spacesPerLevel",     d_spacesPerLevel);
    printer.printAttribute("encodingStyle",      d_encodingStyle);
    printer.printAttribute("encodeEmptyArrays",  d_encodeEmptyArrays);
    printer.printAttribute("encodeNullElements", d_encodeNullElements);
    printer.end();

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bsl::ostream& baljsn::operator<<(bsl::ostream&                stream,
                         const EncoderOptions& object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.initialIndentLevel());
    printer.printValue(object.spacesPerLevel());
    printer.printValue(object.encodingStyle());
    printer.printValue(object.encodeEmptyArrays());
    printer.printValue(object.encodeNullElements());
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
