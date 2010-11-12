// bael_predicateset.cpp                                              -*-C++-*-
#include <bael_predicateset.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_predicateset_cpp,"$Id$ $CSID$")

#include <bael_attributecontainerlist.h>      // for testing only
#include <bael_defaultattributecontainer.h>   // for testing only

#include <bsl_iostream.h>

namespace BloombergLP {

                   // -----------------------
                   // class bael_PredicateSet
                   // -----------------------

// CLASS MEMBER
int bael_PredicateSet::s_initialSize = 8;

int bael_PredicateSet::PredicateHash::s_hashtableSize = INT_MAX;

// CREATE METHOD
int bael_PredicateSet::hash(const bael_PredicateSet& predicateSet, int size)
{
    const_iterator iter;
    int hashValue = 0;
    for (iter = predicateSet.begin(); iter != predicateSet.end(); ++iter) {
        hashValue += bael_Predicate::hash(*iter, size);
    }
    hashValue &= INT_MAX; // clear bit sign
    hashValue %= size;
    return hashValue;
}

// MANIPULATORS
bael_PredicateSet&
bael_PredicateSet::operator=(const bael_PredicateSet& rhs)
{
    if (this != &rhs) {

        d_predicateSet.clear();

        for (const_iterator iter = rhs.begin(); iter != rhs.end(); ++iter) {
            addPredicate(*iter);
        }
    }

    return *this;
}

// ACCESSORS
bool bael_PredicateSet::evaluate(
                        const bael_AttributeContainerList& containerList) const
{
    for (const_iterator iter = begin(); iter != end(); ++iter) {
        if (!containerList.hasValue(iter->attribute())) {
            return false;                                             // RETURN
        }
    }
    return true;
}

bsl::ostream& bael_PredicateSet::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    const char NL = spacesPerLevel >= 0 ? '\n' : ' ';

    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << '{' <<  NL;

    for (const_iterator iter = begin(); iter != end(); ++iter) {
        bdeu_Print::indent(stream, level + 1, spacesPerLevel);
        stream << *iter << NL;
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '}' << NL;

    return stream;
}

// FREE OPERATORS
bool operator==(const bael_PredicateSet& lhs,
                const bael_PredicateSet& rhs)
{
    if (lhs.numPredicates() != rhs.numPredicates()) {
        return false;                                                 // RETURN
    }

    for (bael_PredicateSet::const_iterator iter = lhs.begin();
         iter != lhs.end();
         ++iter) {
        if (!rhs.isMember(*iter)) {
            return false;                                             // RETURN
        }
    }
    return true;
}

bool operator!=(const bael_PredicateSet& lhs,
                const bael_PredicateSet& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
