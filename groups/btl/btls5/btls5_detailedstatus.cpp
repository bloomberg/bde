// btls5_detailedstatus.cpp                                           -*-C++-*-
#include <btls5_detailedstatus.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btls5_detailedstatus_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace btls5 {

                        // --------------------
                        // class DetailedStatus
                        // --------------------

// ACCESSORS

                                  // Aspects

bsl::ostream& DetailedStatus::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("description", d_description);
    if (address().port()) {
        printer.printAttribute("address", d_address);
    }
    printer.end();

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& btls5::operator<<(bsl::ostream&         stream,
                                const DetailedStatus& object)
{
    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.printValue(object.description());
    if (object.address().port()) {
        printer.printValue(object.address());
    }
    printer.end();

    return stream;
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE
//      Copyright (C) Bloomberg L.P., 2015
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
