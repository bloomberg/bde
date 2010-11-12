// bdea_array_datetime.cpp     -*-C++-*-

#include <bdea_array_datetime.h>

#include <bdeu_print.h>

#include <cassert>
#include <ostream>

namespace BloombergLP {

                        // -------------
                        // CLASS METHODS
                        // -------------

int bdea_Array<bdet_Datetime>::maxSupportedBdexVersion()
{
    return 1;
}

int bdea_Array<bdet_Datetime>::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

                        // ---------
                        // ACCESSORS
                        // ---------

std::ostream& bdea_Array<bdet_Datetime>::
                                      print(std::ostream& stream,
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
    const bdet_Datetime *array = data();

    for (int i = 0; i < len; ++i) {
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << array[i] << '\n';
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    return stream << "]\n";
}

std::ostream& bdea_Array<bdet_Datetime>::streamOut(std::ostream& stream) const
{
    int len = length();
    const bdet_Datetime *array = data();

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
