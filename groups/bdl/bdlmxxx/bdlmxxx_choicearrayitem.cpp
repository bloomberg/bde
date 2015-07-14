// bdlmxxx_choicearrayitem.cpp                  -*-C++-*-
#include <bdlmxxx_choicearrayitem.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_choicearrayitem_cpp,"$Id$ $CSID$")


#include <bsls_assert.h>

namespace BloombergLP {

namespace bdlmxxx {
                        // --------------------------
                        // class ChoiceArrayItem
                        // --------------------------

// ACCESSORS
void ChoiceArrayItem::selectionTypes(
                                bsl::vector<ElemType::Type> *result) const
{
    BSLS_ASSERT(result);

    const int len = numSelections();
    result->resize(len);
    for (int i = 0; i < len; ++i) {
        (*result)[i] = selectionType(i);
    }
}

bsl::ostream& ChoiceArrayItem::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    return d_header.print(stream, level, spacesPerLevel);
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
