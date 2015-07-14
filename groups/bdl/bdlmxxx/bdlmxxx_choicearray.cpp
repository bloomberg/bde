// bdlmxxx_choicearray.cpp                  -*-C++-*-
#include <bdlmxxx_choicearray.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_choicearray_cpp,"$Id$ $CSID$")


#include <bdlmxxx_choice.h>            // testing only
#include <bdlmxxx_choiceimp.h>         // testing only
#include <bdlmxxx_listimp.h>           // testing only
#include <bdlmxxx_properties.h>        // testing only
#include <bdlmxxx_tableimp.h>          // testing only

#include <bsls_assert.h>

namespace BloombergLP {

namespace bdlmxxx {
                        // ----------------------
                        // class ChoiceArray
                        // ----------------------

// MANIPULATORS
void ChoiceArray::appendItem(const Choice& src)
{
    d_arrayImp.insertItem(length(),
                          ((const ChoiceImp&)src).choiceHeader());
}

void ChoiceArray::insertItem(int dstIndex, const Choice& src)
{
    d_arrayImp.insertItem(dstIndex,
                          ((const ChoiceImp&)src).choiceHeader());
}

// ACCESSORS
void ChoiceArray::selectionTypes(
                                bsl::vector<ElemType::Type> *result) const
{
    BSLS_ASSERT(result);

    const int len = numSelections();
    result->resize(len);
    for (int i = 0; i < len; ++i) {
        (*result)[i] = selectionType(i);
    }
}

bool ChoiceArray::isAnyNull() const
{
    const int len = length();
    for (int i = 0; i < len; ++i) {
        if (d_arrayImp.theItem(i).isSelectionNull()) {
            return true;
        }
    }
    return false;
}

bool ChoiceArray::isAnyNonNull() const
{
    const int len = length();
    for (int i = 0; i < len; ++i) {
        if (!d_arrayImp.theItem(i).isSelectionNull()) {
            return true;
        }
    }
    return false;
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
