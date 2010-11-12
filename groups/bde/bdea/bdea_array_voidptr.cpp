// bdea_array_voidptr.cpp     -*-C++-*-

#include <bdea_array_voidptr.h>

#include <bdes_platformutil.h>                  // for testing only
#include <bdeu_print.h>
#include <bdeu_print.h>

#include <cassert>
#include <ostream>

namespace BloombergLP {

                        // ---------
                        // ACCESSORS
                        // ---------

std::ostream& bdea_Array<void *>::print(std::ostream& stream,
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
    void * const *array = data();

    for (int i = 0; i < len; ++i) {
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        bdeu_Print::printPtr(stream, array[i]);
        stream << '\n';
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    return stream << "]\n";
}

                        // --------------
                        // FREE OPERATORS
                        // --------------

std::ostream& operator<<(std::ostream& stream, const bdea_Array<void *>& rhs)
{
    int len = rhs.length();
    void * const *array = rhs.data();

    stream << '[';
    for (int i = 0; i < len; ++i) {
        stream << ' ';
        bdeu_Print::printPtr(stream, array[i]);
    }
    return stream << " ]" << std::flush;
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
