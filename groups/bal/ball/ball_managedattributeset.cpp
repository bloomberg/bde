// ball_managedattributeset.cpp                                       -*-C++-*-
#include <ball_managedattributeset.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_managedattributeset_cpp,"$Id$ $CSID$")

#include <ball_attributecontainerlist.h>
#include <ball_defaultattributecontainer.h>   // for testing only

#include <bslim_printer.h>

#include <bsls_assert.h>

#include <bsl_climits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ball {

                        // -------------------------
                        // class ManagedAttributeSet
                        // -------------------------

// CLASS DATA
int ManagedAttributeSet::AttributeHash::s_hashtableSize = INT_MAX;

int ManagedAttributeSet::s_initialSize = 8;

// CLASS METHODS
int ManagedAttributeSet::hash(const ManagedAttributeSet& set, int size)
{
    BSLS_ASSERT(0 < size);

    unsigned int hashValue = 0;
    for (const_iterator iter = set.begin(); iter != set.end(); ++iter) {
        hashValue += ManagedAttribute::hash(*iter, size);
    }
    hashValue %= size;
    return hashValue;
}

// MANIPULATORS
ManagedAttributeSet&
ManagedAttributeSet::operator=(const ManagedAttributeSet& rhs)
{
    if (this != &rhs) {
        d_attributeSet = rhs.d_attributeSet;
    }
    return *this;
}

// ACCESSORS
bool
ManagedAttributeSet::evaluate(const AttributeContainerList& containerList)
                                                                          const
{
    for (const_iterator iter = begin(); iter != end(); ++iter) {
        if (!containerList.hasValue(iter->attribute())) {
            return false;                                             // RETURN
        }
    }
    return true;
}

bsl::ostream&
ManagedAttributeSet::print(bsl::ostream& stream,
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
bool ball::operator==(const ManagedAttributeSet& lhs,
                      const ManagedAttributeSet& rhs)
{
    if (lhs.numAttributes() != rhs.numAttributes()) {
        return false;                                                 // RETURN
    }

    for (ManagedAttributeSet::const_iterator iter = lhs.begin();
         iter != lhs.end();
         ++iter) {
        if (!rhs.isMember(*iter)) {
            return false;                                             // RETURN
        }
    }
    return true;
}

bool ball::operator!=(const ManagedAttributeSet& lhs,
                      const ManagedAttributeSet& rhs)
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
