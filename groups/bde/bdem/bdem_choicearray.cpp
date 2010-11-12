// bdem_choicearray.cpp                  -*-C++-*-
#include <bdem_choicearray.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_choicearray_cpp,"$Id$ $CSID$")


#include <bdem_choice.h>            // testing only
#include <bdem_choiceimp.h>         // testing only
#include <bdem_listimp.h>           // testing only
#include <bdem_properties.h>        // testing only
#include <bdem_tableimp.h>          // testing only

#include <bsls_assert.h>

namespace BloombergLP {

                        // ----------------------
                        // class bdem_ChoiceArray
                        // ----------------------

// MANIPULATORS
void bdem_ChoiceArray::appendItem(const bdem_Choice& src)
{
    d_arrayImp.insertItem(length(),
                          ((const bdem_ChoiceImp&)src).choiceHeader());
}

void bdem_ChoiceArray::insertItem(int dstIndex, const bdem_Choice& src)
{
    d_arrayImp.insertItem(dstIndex,
                          ((const bdem_ChoiceImp&)src).choiceHeader());
}

// ACCESSORS
void bdem_ChoiceArray::selectionTypes(
                                bsl::vector<bdem_ElemType::Type> *result) const
{
    BSLS_ASSERT(result);

    const int len = numSelections();
    result->resize(len);
    for (int i = 0; i < len; ++i) {
        (*result)[i] = selectionType(i);
    }
}

bool bdem_ChoiceArray::isAnyNull() const
{
    const int len = length();
    for (int i = 0; i < len; ++i) {
        if (d_arrayImp.theItem(i).isSelectionNull()) {
            return true;
        }
    }
    return false;
}

bool bdem_ChoiceArray::isAnyNonNull() const
{
    const int len = length();
    for (int i = 0; i < len; ++i) {
        if (!d_arrayImp.theItem(i).isSelectionNull()) {
            return true;
        }
    }
    return false;
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
