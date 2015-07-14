// ball_predicateset.cpp                                              -*-C++-*-
#include <ball_predicateset.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_predicateset_cpp,"$Id$ $CSID$")

#include <ball_attributecontainerlist.h>      // for testing only
#include <ball_defaultattributecontainer.h>   // for testing only

#include <bsl_iostream.h>

namespace BloombergLP {

                   // -----------------------
                   // class ball::PredicateSet
                   // -----------------------

// CLASS MEMBER
int ball::PredicateSet::s_initialSize = 8;

int ball::PredicateSet::PredicateHash::s_hashtableSize = INT_MAX;

namespace ball {
// CREATE METHOD
int PredicateSet::hash(const PredicateSet& predicateSet, int size)
{
    const_iterator iter;
    int hashValue = 0;
    for (iter = predicateSet.begin(); iter != predicateSet.end(); ++iter) {
        hashValue += Predicate::hash(*iter, size);
    }
    hashValue &= INT_MAX; // clear bit sign
    hashValue %= size;
    return hashValue;
}

// MANIPULATORS
PredicateSet&
PredicateSet::operator=(const PredicateSet& rhs)
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
bool PredicateSet::evaluate(
                        const AttributeContainerList& containerList) const
{
    for (const_iterator iter = begin(); iter != end(); ++iter) {
        if (!containerList.hasValue(iter->attribute())) {
            return false;                                             // RETURN
        }
    }
    return true;
}

bsl::ostream& PredicateSet::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    const char NL = spacesPerLevel >= 0 ? '\n' : ' ';

    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << '{' <<  NL;

    for (const_iterator iter = begin(); iter != end(); ++iter) {
        bdlb::Print::indent(stream, level + 1, spacesPerLevel);
        stream << *iter << NL;
    }

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << '}' << NL;

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bool ball::operator==(const PredicateSet& lhs,
                const PredicateSet& rhs)
{
    if (lhs.numPredicates() != rhs.numPredicates()) {
        return false;                                                 // RETURN
    }

    for (PredicateSet::const_iterator iter = lhs.begin();
         iter != lhs.end();
         ++iter) {
        if (!rhs.isMember(*iter)) {
            return false;                                             // RETURN
        }
    }
    return true;
}

bool ball::operator!=(const PredicateSet& lhs,
                const PredicateSet& rhs)
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
