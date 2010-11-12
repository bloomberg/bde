// bdem_choice.cpp                                                    -*-C++-*-
#include <bdem_choice.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_choice_cpp,"$Id$ $CSID$")

#include <bdem_choicearrayitem.h>            // for testing only
#include <bdem_properties.h>                 // for testing only
#include <bdem_row.h>                        // for testing only

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                        // -----------------
                        // class bdem_Choice
                        // -----------------

// MANIPULATORS
bdem_Choice& bdem_Choice::operator=(const bdem_ChoiceArrayItem& rhs)
{
    const bdem_ChoiceHeader& rhsHeader = (const bdem_ChoiceHeader&)rhs;
    bdem_ChoiceHeader&       lhsHeader = d_choiceImp.choiceHeader();
    if (&lhsHeader != &rhsHeader) {
        bdem_ChoiceHeader::DescriptorCatalog *catalog =
       const_cast<bdem_ChoiceHeader::DescriptorCatalog *>(lhsHeader.catalog());

        lhsHeader.reset();

        *catalog  = *rhsHeader.catalog();
        lhsHeader = rhsHeader;
    }
    return *this;
}

// ACCESSORS
void
bdem_Choice::selectionTypes(bsl::vector<bdem_ElemType::Type> *result) const
{
    BSLS_ASSERT(result);

    int len = numSelections();
    result->resize(len);
    for (int i = 0; i < len; ++i) {
        (*result)[i] = selectionType(i);
    }
}

bsl::ostream& bdem_Choice::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return d_choiceImp.print(stream,
                             level,
                             spacesPerLevel);
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
