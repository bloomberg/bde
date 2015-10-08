// ball_defaultattributecontainer.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_defaultattributecontainer.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_defaultattributecontainer_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_climits.h>


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
bool DefaultAttributeContainer::hasValue(const Attribute& value) const
{
    return d_attributeSet.find(value) != d_attributeSet.end();
}


bsl::ostream& DefaultAttributeContainer::print(
                                            bsl::ostream& stream,
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
