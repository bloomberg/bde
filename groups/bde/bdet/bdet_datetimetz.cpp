// bdet_datetimetz.cpp                                                -*-C++-*-
#include <bdet_datetimetz.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdet_datetimetz_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_cstdio.h>    // 'sprintf'
#include <bsl_ostream.h>
#include <bsl_sstream.h>

namespace BloombergLP {

                             // ---------------------
                             // class bdet_DatetimeTz
                             // ---------------------

// ACCESSORS
bsl::ostream& bdet_DatetimeTz::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    bsl::ostringstream tmp;

    // Set the width of 'tmp' to 0, so that the original width of 'stream' is
    // preserved when streaming 'tmp' into 'stream'.

    tmp.copyfmt(stream);
    tmp.width(0);

    tmp << d_datetime;

    const char sign    = d_offset < 0 ? '-' : '+';
    const int  minutes = '-' == sign ? -d_offset : d_offset;
    const int  hours   = minutes / 60;

    // space usage: +-  hh  mm  nil
    const int SIZE = 1 + 2 + 2 + 1;
    char buf[SIZE];

    // Use only 2 digits for 'hours' (DRQS 12693813).
    if (hours < 100) {
        bsl::sprintf(buf, "%c%02d%02d", sign, hours, minutes % 60);
    }
    else {
        bsl::sprintf(buf, "%cXX%02d", sign, minutes % 60);
    }

    tmp << buf;

    return stream << tmp.str() << bsl::flush;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const bdet_DatetimeTz& rhs)
{
    return rhs.print(stream, 0, -1);
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
