// bael_rule.cpp                 -*-C++-*-
#include <bael_rule.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_rule_cpp,"$Id$ $CSID$")

#include <bael_attributecontainerlist.h>        // for testing only
#include <bael_defaultattributecontainer.h>     // for testing only

#include <bdeu_hashutil.h>

#include <bsl_iostream.h>

namespace BloombergLP {

                         // ---------------
                         // class bael_Rule
                         // ---------------

// CLASS METHODS
int bael_Rule::hash(const bael_Rule& rule, int size)
{
    if (rule.d_hashValue < 0 || rule.d_hashSize != size) {
        rule.d_hashValue = bael_PredicateSet::hash(rule.d_predicateSet, size)
                      + bael_ThresholdAggregate::hash(rule.d_thresholds, size)
                      + bdeu_HashUtil::hash0(rule.d_pattern.c_str(), size);
        rule.d_hashValue %= size;
        rule.d_hashValue &= ~(1 << (sizeof(int) * 8 - 1));  // clear sign bit
        rule.d_hashSize = size;
    }
    return rule.d_hashValue;
}

// MANIPULATORS
bael_Rule& bael_Rule::operator=(const bael_Rule& rhs)
{
    d_pattern = rhs.d_pattern,
    d_thresholds = rhs.d_thresholds;
    d_predicateSet = rhs.d_predicateSet;
    d_hashValue = rhs.d_hashValue;
    d_hashSize  = rhs.d_hashSize;
    return *this;
}

// ACCESSORS
bsl::ostream& bael_Rule::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    const char NL = spacesPerLevel >= 0 ? '\n' : ' ';

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '{' <<  NL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << "pattern = " << d_pattern << NL;

    d_thresholds.print(stream, level + 1, spacesPerLevel);

    d_predicateSet.print(stream, level + 1, spacesPerLevel);

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '}' << NL;

    return stream;
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
