// ball_predicateset.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_predicateset.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_predicateset_cpp,"$Id$ $CSID$")

#include <ball_attributecontainerlist.h>      // for testing only
#include <ball_defaultattributecontainer.h>   // for testing only

#include <bslim_printer.h>

#include <bsl_climits.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                   // ------------------------
                   // class ball::PredicateSet
                   // ------------------------

// CLASS MEMBER
int ball::PredicateSet::s_initialSize = 8;

int ball::PredicateSet::PredicateHash::s_hashtableSize = INT_MAX;

namespace ball {
// CREATE METHOD
int PredicateSet::hash(const PredicateSet& set, int size)
{
    const_iterator iter;
    int hashValue = 0;
    for (iter = set.begin(); iter != set.end(); ++iter) {
        hashValue += Predicate::hash(*iter, size);
    }
    hashValue &= INT_MAX; // clear bit sign
    hashValue %= size;
    return hashValue;
}

// MANIPULATORS
PredicateSet& PredicateSet::operator=(const PredicateSet& rhs)
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
bool PredicateSet::evaluate(const AttributeContainerList& containerList) const
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
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    for (const_iterator iter = begin(); iter != end(); ++iter) {
        printer.printValue(*iter);
    }
    printer.end();
    return stream;
}

}  // close package namespace

// FREE OPERATORS
bool ball::operator==(const PredicateSet& lhs, const PredicateSet& rhs)
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

bool ball::operator!=(const PredicateSet& lhs, const PredicateSet& rhs)
{
    return !(lhs == rhs);
}

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
