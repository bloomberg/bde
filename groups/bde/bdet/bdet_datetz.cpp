// bdet_datetz.cpp                                                    -*-C++-*-
#include <bdet_datetz.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdet_datetz_cpp,"$Id$ $CSID$")

#include <bdesb_fixedmemoutstreambuf.h>

#include <bdeu_print.h>

#include <bsl_cstdio.h>    // 'sprintf'
#include <bsl_ostream.h>
#include <bsl_sstream.h>

namespace BloombergLP {

                             // -----------------
                             // class bdet_DateTz
                             // -----------------

// ACCESSORS
bsl::ostream& bdet_DateTz::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    // Write to a temporary stream having width 0 in case the caller has done
    // something like:
    //..
    //  os << bsl::setw(20) << myDateTz;
    //..
    // The user-specified width will be effective when 'buffer' is written to
    // 'stream' (below).

    const int SIZE = 128;  // Size the buffer to be able to hold a *bad* date.
    char buffer[SIZE];

    bdesb_FixedMemOutStreamBuf sb(buffer, SIZE);
    bsl::ostream os(&sb);

    os << localDate();

    const char sign    = d_offset < 0 ? '-' : '+';
    const int  minutes = '-' == sign ? -d_offset : d_offset;
    const int  hours   = minutes / 60;

    //         space usage: +-  hh  mm  nil
    const int OFFSET_SIZE = 1 + 2 + 2 + 1;
    char offsetBuffer[OFFSET_SIZE];

    // Use only 2 digits for 'hours' (DRQS 12693813).
    if (hours < 100) {
        bsl::sprintf(offsetBuffer, "%c%02d%02d", sign, hours, minutes % 60);
    }
    else {
        bsl::sprintf(offsetBuffer, "%cXX%02d", sign, minutes % 60);
    }

    os << offsetBuffer;

    buffer[sb.length()] = '\0';

    stream << buffer;

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
