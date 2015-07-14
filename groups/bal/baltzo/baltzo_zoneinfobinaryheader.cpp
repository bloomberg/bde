// baltzo_zoneinfobinaryheader.cpp                                    -*-C++-*-
#include <baltzo_zoneinfobinaryheader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_zoneinfobinaryheader_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

#include <bslim_printer.h>

namespace BloombergLP {

namespace baltzo {
                        // ---------------------------------
                        // class ZoneinfoBinaryHeader
                        // ---------------------------------

// ACCESSORS

                        // Aspects

bsl::ostream& ZoneinfoBinaryHeader::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("version",           static_cast<int>(d_version));
    printer.printAttribute("numIsGmt",          d_numIsGmt);
    printer.printAttribute("numIsStd",          d_numIsStd);
    printer.printAttribute("numLeaps",          d_numLeaps);
    printer.printAttribute("numTransitions",    d_numTransitions);
    printer.printAttribute("numLocalTimeTypes", d_numLocalTimeTypes);
    printer.printAttribute("abbrevDataSize",    d_abbrevDataSize);
    printer.end();

    return stream;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
