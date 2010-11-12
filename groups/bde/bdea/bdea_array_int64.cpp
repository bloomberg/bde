// bdea_array_int64.cpp     -*-C++-*-

#include <bdea_array_int64.h>

#include <bdeu_print.h>

#include <cassert>
#include <ostream>

namespace BloombergLP {

typedef bdes_PlatformUtil::Int64 Int64;

                         // -------------
                         // CLASS METHODS
                         // -------------

int bdea_Array<bdes_PlatformUtil::Int64>::maxSupportedBdexVersion()
{
    return 1;
}

int bdea_Array<bdes_PlatformUtil::Int64>::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

                        // ---------
                        // ACCESSORS
                        // ---------

std::ostream& bdea_Array<Int64>::print(std::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    assert(0 <= spacesPerLevel);

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[\n";

    if (level < 0) {
        level = -level;
    }

    int levelPlus1 = level + 1;
    int len = length();
    const Int64 *array = data();

    for (int i = 0; i < len; ++i) {
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << array[i] << '\n';
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    return stream << "]\n";
}

std::ostream& bdea_Array<bdes_PlatformUtil::Int64>::
                                          streamOut(std::ostream& stream) const
{
    int len = length();
    const bdes_PlatformUtil::Int64 *array = data();

    stream << '[';
    for (int i = 0; i < len; ++i) {
        stream << ' ' << array[i];
    }
    return stream << " ]";
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
