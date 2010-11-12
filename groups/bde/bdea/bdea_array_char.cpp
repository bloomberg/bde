// bdea_array_char.cpp     -*-C++-*-

#include <bdea_array_char.h>

#include <bdes_platformutil.h>                  // for testing only
#include <bdeu_print.h>

#include <cassert>
#include <ostream>

namespace BloombergLP {

                        // ---------
                        // ACCESSORS
                        // ---------

std::ostream& bdea_Array<char>::print(std::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '[';

    if (level < 0) {
        level = -level;
    }

    // Nested items are indented an additional level.
    ++level;

    for (int i = 0; i < d_array.length(); ++i) {
        bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << (char) d_array[i];
    }

    --level;
    bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << ']';

    if (0 <= spacesPerLevel) {
        stream << '\n';
    }

    return stream;
}

std::ostream& bdea_Array<char>::streamOut(std::ostream& stream) const
{
    // Print out on a single line.
    print(stream, 0, -1);
    return stream;
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
