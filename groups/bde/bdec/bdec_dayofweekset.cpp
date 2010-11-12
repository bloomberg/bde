// bdec_dayofweekset.cpp              -*-C++-*-
#include <bdec_dayofweekset.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdec_dayofweekset_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

// STATIC DATA
static const bdet_DayOfWeek::Day DAY_OF_WEEK_ARRAY[8] = {
    static_cast<bdet_DayOfWeek::Day>(-1), // place holder, never used
    bdet_DayOfWeek::BDET_SUNDAY,
    bdet_DayOfWeek::BDET_MONDAY,
    bdet_DayOfWeek::BDET_TUESDAY,
    bdet_DayOfWeek::BDET_WEDNESDAY,
    bdet_DayOfWeek::BDET_THURSDAY,
    bdet_DayOfWeek::BDET_FRIDAY,
    bdet_DayOfWeek::BDET_SATURDAY
};

                         // ----------------------------
                         // class bdec_DayOfWeekSet_Iter
                         // ----------------------------

// CLASS DATA
const bdet_DayOfWeek::Day *bdec_DayOfWeekSet_Iter::s_dayOfWeekArray_p =
                                                             DAY_OF_WEEK_ARRAY;

// CREATORS
bdec_DayOfWeekSet_Iter::bdec_DayOfWeekSet_Iter(int data, int index)
: d_data((char)data)
, d_index((char)index)
{
    while (d_index < 8 && !((1 << d_index) & d_data)) {
        ++d_index;
    }
}

// MANIPULATORS
bdec_DayOfWeekSet_Iter& bdec_DayOfWeekSet_Iter::operator++()
{
    do {
        ++d_index;
    } while (d_index < 8 && !((1 << d_index) & d_data));
    return *this;
}

bdec_DayOfWeekSet_Iter& bdec_DayOfWeekSet_Iter::operator--()
{
    do {
        --d_index;
    } while (d_index > 0 && !((1 << d_index) & d_data));
    return *this;
}

                         // -----------------------
                         // class bdec_DayOfWeekSet
                         // -----------------------

// ACCESSORS
bsl::ostream& bdec_DayOfWeekSet::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;
    }

    // one-line format, no indentation, if 'spacesPerLevel' is negative
    const char NL          = spacesPerLevel >=0 ? '\n' : ' ';
    const int  hasElements = 0 != length();

    if (level >= 0) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    else {
        level = -level;
    }

    stream << '[' << NL;

    if (spacesPerLevel >= 0 && hasElements) {
        bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    }

    for (iterator iter = begin(); iter != end(); ++iter) {
        if (iter != begin()) {
            stream << ' ';
        }
        stream << *iter;
    }

    if (hasElements) {
        stream << NL;
    }

    if (spacesPerLevel < 0) {
        stream << ']';
        return stream;
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << ']' << NL;

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
