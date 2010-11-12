// bdem_choicearrayitem.cpp                  -*-C++-*-
#include <bdem_choicearrayitem.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_choicearrayitem_cpp,"$Id$ $CSID$")


#include <bsls_assert.h>

namespace BloombergLP {

                        // --------------------------
                        // class bdem_ChoiceArrayItem
                        // --------------------------

// ACCESSORS
void bdem_ChoiceArrayItem::selectionTypes(
                                bsl::vector<bdem_ElemType::Type> *result) const
{
    BSLS_ASSERT(result);

    const int len = numSelections();
    result->resize(len);
    for (int i = 0; i < len; ++i) {
        (*result)[i] = selectionType(i);
    }
}

bsl::ostream& bdem_ChoiceArrayItem::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    return d_header.print(stream, level, spacesPerLevel);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
