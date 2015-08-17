// ball_rule.cpp                                                      -*-C++-*-
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

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
