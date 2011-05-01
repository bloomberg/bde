// baetzo_zoneinfobinaryheader.cpp                                    -*-C++-*-
#include <baetzo_zoneinfobinaryheader.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_zoneinfobinaryheader_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

#include <bslim_printer.h>

namespace BloombergLP {

                        // ---------------------------------
                        // class baetzo_ZoneinfoBinaryHeader
                        // ---------------------------------

// ACCESSORS

                        // Aspects

bsl::ostream& baetzo_ZoneinfoBinaryHeader::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.print(static_cast<int>(d_version), "version");
    printer.print(d_numIsGmt,                  "numIsGmt");
    printer.print(d_numIsStd,                  "numIsStd");
    printer.print(d_numLeaps,                  "numLeaps");
    printer.print(d_numTransitions,            "numTransitions");
    printer.print(d_numLocalTimeTypes,         "numLocalTimeTypes");
    printer.print(d_abbrevDataSize,            "abbrevDataSize");
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
