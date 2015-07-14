// ball_record.cpp         -*-C++-*-
#include <ball_record.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_record_cpp,"$Id$ $CSID$")

#ifdef BDE_FOR_TESTING_ONLY
#include <ball_severity.h>       // for testing only
#endif

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace ball {
                           // -----------------
                           // class Record
                           // -----------------

// ACCESSORS
bsl::ostream& Record::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    if (0 <= spacesPerLevel) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "[\n";
    }
    else {
        // No newlines if 0 > spacesPerLevel.
        stream << '[';
    }

    if (level < 0) {
        level = -level;
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    d_fixedFields.print(stream, levelPlus1, spacesPerLevel);

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
    }
    else {
        stream << ' ';
    }
    d_userFields.print(stream, levelPlus1, spacesPerLevel);

    if (0 <= spacesPerLevel) {
        stream << '\n';
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        stream << " ]";
    }

    return stream << bsl::flush;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
