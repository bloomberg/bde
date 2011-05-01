// bdet_datetime.cpp                                                  -*-C++-*-
#include <bdet_datetime.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdet_datetime_cpp,"$Id$ $CSID$")

#include <bdesb_fixedmemoutstreambuf.h>

#include <bdeu_print.h>

#include <bsl_ostream.h>
#include <bsl_sstream.h>

namespace BloombergLP {

                        // -------------------
                        // class bdet_Datetime
                        // -------------------

// ACCESSORS
bsl::ostream& bdet_Datetime::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    // Write to a temporary stream having width 0 in case the caller has done
    // something like:
    //..
    //  os << bsl::setw(20) << myDatetime;
    //..
    // The user-specified width will be effective when 'buffer' is written to
    // the 'stream' (below).

    const int SIZE = 128;  // Size the buffer to be able to hold a *bad* date.
    char buffer[SIZE];

    bdesb_FixedMemOutStreamBuf sb(buffer, SIZE);
    bsl::ostream os(&sb);

    os << date() << '_' << time();

    buffer[sb.length()] = '\0';

    stream << buffer;

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const bdet_Datetime& datetime)
{
    return datetime.print(stream, 0, -1);
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
