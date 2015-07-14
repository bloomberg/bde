// ball_defaultattributecontainer.cpp                 -*-C++-*-
#include <ball_defaultattributecontainer.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_defaultattributecontainer_cpp,"$Id$ $CSID$")

namespace BloombergLP {

int ball::DefaultAttributeContainer::s_initialSize = 8;

int ball::DefaultAttributeContainer::AttributeHash::s_hashtableSize = INT_MAX;

namespace ball {
// MANIPULATORS
DefaultAttributeContainer&
DefaultAttributeContainer::operator=(
    const DefaultAttributeContainer& rhs)
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
bool DefaultAttributeContainer::hasValue(
                                             const Attribute& value) const
{
    return d_attributeSet.find(value) != d_attributeSet.end();
}


bsl::ostream& DefaultAttributeContainer::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    const char NL = spacesPerLevel >= 0 ? '\n' : ' ';
    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << '{' <<  NL;

    for (const_iterator iter = begin(); iter != end(); ++iter) {
        iter->print(stream, level + 1, spacesPerLevel);
        stream << NL;
    }

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << '}' << NL;

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bool ball::operator==(const DefaultAttributeContainer& lhs,
                const DefaultAttributeContainer& rhs)
{
    if (lhs.numAttributes() != rhs.numAttributes()) {
        return false;                                                 // RETURN
    }

    for (DefaultAttributeContainer::const_iterator iter = lhs.begin();
         iter != lhs.end();
         ++iter) {
        if (!rhs.hasValue(*iter)) {
            return false;                                             // RETURN
        }
    }
    return true;
}

bool ball::operator!=(const DefaultAttributeContainer& lhs,
                const DefaultAttributeContainer& rhs)
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
