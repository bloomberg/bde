// bael_defaultattributecontainer.cpp                 -*-C++-*-
#include <bael_defaultattributecontainer.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_defaultattributecontainer_cpp,"$Id$ $CSID$")

namespace BloombergLP {

int bael_DefaultAttributeContainer::s_initialSize = 8;

int bael_DefaultAttributeContainer::AttributeHash::s_hashtableSize = INT_MAX;

// MANIPULATORS
bael_DefaultAttributeContainer&
bael_DefaultAttributeContainer::operator=(
    const bael_DefaultAttributeContainer& rhs)
{
    if (this != &rhs) {
        removeAllAttributes();

        for (const_iterator iter = rhs.begin(); iter != rhs.end(); ++iter) {
            addAttribute(*iter);
        }
    }

    return *this;
}

// ACCESSORS
bool bael_DefaultAttributeContainer::hasValue(
                                             const bael_Attribute& value) const
{
    return d_attributeSet.find(value) != d_attributeSet.end();
}


bsl::ostream& bael_DefaultAttributeContainer::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    const char NL = spacesPerLevel >= 0 ? '\n' : ' ';
    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << '{' <<  NL;

    for (const_iterator iter = begin(); iter != end(); ++iter) {
        iter->print(stream, level + 1, spacesPerLevel);
        stream << NL;
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '}' << NL;

    return stream;
}

// FREE OPERATORS
bool operator==(const bael_DefaultAttributeContainer& lhs,
                const bael_DefaultAttributeContainer& rhs)
{
    if (lhs.numAttributes() != rhs.numAttributes()) {
        return false;                                                 // RETURN
    }

    for (bael_DefaultAttributeContainer::const_iterator iter = lhs.begin();
         iter != lhs.end();
         ++iter) {
        if (!rhs.hasValue(*iter)) {
            return false;                                             // RETURN
        }
    }
    return true;
}

bool operator!=(const bael_DefaultAttributeContainer& lhs,
                const bael_DefaultAttributeContainer& rhs)
{
    return !(lhs == rhs);
}

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
