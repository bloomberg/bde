// bdlmxxx_choice.cpp                                                    -*-C++-*-
#include <bdlmxxx_choice.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_choice_cpp,"$Id$ $CSID$")

#include <bdlmxxx_choicearrayitem.h>            // for testing only
#include <bdlmxxx_properties.h>                 // for testing only
#include <bdlmxxx_row.h>                        // for testing only

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace bdlmxxx {
                        // -----------------
                        // class Choice
                        // -----------------

// MANIPULATORS
Choice& Choice::operator=(const ChoiceArrayItem& rhs)
{
    const ChoiceHeader& rhsHeader = (const ChoiceHeader&)rhs;
    ChoiceHeader&       lhsHeader = d_choiceImp.choiceHeader();
    if (&lhsHeader != &rhsHeader) {
        ChoiceHeader::DescriptorCatalog *catalog =
       const_cast<ChoiceHeader::DescriptorCatalog *>(lhsHeader.catalog());

        lhsHeader.reset();

        *catalog  = *rhsHeader.catalog();
        lhsHeader = rhsHeader;
    }
    return *this;
}

// ACCESSORS
void
Choice::selectionTypes(bsl::vector<ElemType::Type> *result) const
{
    BSLS_ASSERT(result);

    int len = numSelections();
    result->resize(len);
    for (int i = 0; i < len; ++i) {
        (*result)[i] = selectionType(i);
    }
}

bsl::ostream& Choice::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    return d_choiceImp.print(stream,
                             level,
                             spacesPerLevel);
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
