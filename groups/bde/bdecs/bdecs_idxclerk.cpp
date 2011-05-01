// bdecs_idxclerk.cpp            -*-C++-*-
#include <bdecs_idxclerk.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdecs_idxclerk_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bslma_default.h>        // for testing only
#include <bslma_testallocator.h>  // for testing only
#include <bsls_performancehint.h>

#include <bsl_ostream.h>
#include <bsl_vector.h>

namespace BloombergLP {

                        // --------------------
                        // class bdecs_IdxClerk
                        // --------------------

// PRIVATE ACCESSORS
bool
bdecs_IdxClerk::areInvariantsPreserved(const bsl::vector<int>& unusedStack,
                                       int                     nextNewIndex)
{
    int indicesInvalid   = 0;
    int indicesNotUnique = 0;
    int size             = unusedStack.size();

    bsl::vector<char> bin(nextNewIndex, 0);

    // Optimizing for the valid case.

    for (int i = 0; i < size; ++i) {
        indicesInvalid |= ((unsigned int) unusedStack[i] >=
                                                  (unsigned int) nextNewIndex);

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(!indicesInvalid)) {
            indicesNotUnique |= ++bin[unusedStack[i]];
        }
    }

    if (indicesInvalid || (indicesNotUnique & 0x2)) {
        return false;                                                 // RETURN
    }

    return true;
}

// ACCESSORS
bool bdecs_IdxClerk::isInUse(int index) const
{
    BSLS_ASSERT((unsigned int) index < (unsigned int)d_nextNewIndex);

    for (unsigned int i = 0; i < d_unusedStack.size(); ++i) {
        if (index == d_unusedStack[i]) {
            return false;
        }
    }
    return true;
}

bsl::ostream& bdecs_IdxClerk::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    int nestedLevel, nestedSpacesPerLevel;

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '[';

    if (level < 0) {
        level = -level;
    }

    if (0 <= spacesPerLevel) {
        stream << "\n";
        nestedLevel = level + 1;
        nestedSpacesPerLevel = spacesPerLevel;
    }
    else {
        // If 'spacesPerLevel' is negative, just put one space between fields
        // and suppress newlines when formatting each field.

        nestedLevel = 1;
        nestedSpacesPerLevel = -1;
    }

    for (bdecs_IdxClerkIter it = begin(); it != end(); ++it) {
        bdeu_Print::indent(stream, nestedLevel, nestedSpacesPerLevel);
        stream << *it;
    }

    bdeu_Print::indent(stream, nestedLevel, nestedSpacesPerLevel);
    stream << "(" << d_nextNewIndex << ")";

    if (0 <= spacesPerLevel) {
        stream << "\n";
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        stream << " ]";
    }

    return stream << bsl::flush;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
