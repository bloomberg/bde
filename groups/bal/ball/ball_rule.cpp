// ball_rule.cpp                 -*-C++-*-
#include <ball_rule.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_rule_cpp,"$Id$ $CSID$")

#include <ball_attributecontainerlist.h>        // for testing only
#include <ball_defaultattributecontainer.h>     // for testing only

#include <bdlb_hashutil.h>

#include <bsl_iostream.h>

namespace BloombergLP {

namespace ball {
                         // ---------------
                         // class Rule
                         // ---------------

// CLASS METHODS
int Rule::hash(const Rule& rule, int size)
{
    if (rule.d_hashValue < 0 || rule.d_hashSize != size) {
        rule.d_hashValue = PredicateSet::hash(rule.d_predicateSet, size)
                      + ThresholdAggregate::hash(rule.d_thresholds, size)
                      + bdlb::HashUtil::hash0(rule.d_pattern.c_str(), size);
        rule.d_hashValue %= size;
        rule.d_hashValue &= ~(1 << (sizeof(int) * 8 - 1));  // clear sign bit
        rule.d_hashSize = size;
    }
    return rule.d_hashValue;
}

// MANIPULATORS
Rule& Rule::operator=(const Rule& rhs)
{
    d_pattern = rhs.d_pattern,
    d_thresholds = rhs.d_thresholds;
    d_predicateSet = rhs.d_predicateSet;
    d_hashValue = rhs.d_hashValue;
    d_hashSize  = rhs.d_hashSize;
    return *this;
}

// ACCESSORS
bsl::ostream& Rule::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    const char NL = spacesPerLevel >= 0 ? '\n' : ' ';

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << '{' <<  NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << "pattern = " << d_pattern << NL;

    d_thresholds.print(stream, level + 1, spacesPerLevel);

    d_predicateSet.print(stream, level + 1, spacesPerLevel);

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << '}' << NL;

    return stream;
}
}  // close package namespace

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
