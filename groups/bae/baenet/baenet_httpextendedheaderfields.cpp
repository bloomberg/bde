// baenet_httpextendedheaderfields.cpp   -*-C++-*-
#include <baenet_httpextendedheaderfields.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpextendedheaderfields_cpp,"$Id$ $CSID$")

namespace BloombergLP {

bool operator==(const baenet_HttpExtendedHeaderFields& lhs,
                const baenet_HttpExtendedHeaderFields& rhs)
{
    typedef baenet_HttpExtendedHeaderFields::ConstFieldIterator
                                                       ConstFieldIterator;
    typedef baenet_HttpExtendedHeaderFields::ConstFieldValueIterator
                                                       ConstFieldValueIterator;

    if (lhs.numFields() != rhs.numFields()) {
        return false;
    }

    ConstFieldIterator lhsIter, rhsIter;

    for (lhsIter  = lhs.beginFields(), rhsIter  = rhs.beginFields();
         lhsIter != lhs.endFields() && rhsIter != rhs.endFields();
         ++lhsIter, ++rhsIter) {
        if (lhsIter->first         != rhsIter->first
         || lhsIter->second.size() != rhsIter->second.size()) {
            return false;
        }

        ConstFieldValueIterator lhsValueIter, rhsValueIter;

        for (lhsValueIter  = lhsIter->second.begin(),
                                       rhsValueIter  = rhsIter->second.begin();
             lhsValueIter != lhsIter->second.end()
                                    && rhsValueIter != rhsIter->second.end();
             ++lhsValueIter, ++rhsValueIter) {
            if (*lhsValueIter != *rhsValueIter) {
                return false;
            }
        }
    }

    return true;
}

}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
