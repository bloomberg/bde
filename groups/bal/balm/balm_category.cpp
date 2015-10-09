// balm_category.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_category.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_category_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

namespace balm {
                               // --------------
                               // class Category
                               // --------------

// CREATORS
Category::~Category()
{
    // Reset the linked-list of category holders registered with this object.
    while (d_holders_p) {
        CategoryHolder *next = d_holders_p->next();
        d_holders_p->reset();
        d_holders_p = next;
    }
}

// MANIPULATORS
void Category::setEnabled(bool enabledFlag)
{
    if (d_enabled != enabledFlag) {

        // Update the linked-list of category holder's registered with this
        // category.
        CategoryHolder *holder = d_holders_p;
        while (holder) {
            holder->setEnabled(enabledFlag);
            holder = holder->next();
        }
        d_enabled = enabledFlag;
    }
}

void Category::registerCategoryHolder(CategoryHolder *holder)
{
    holder->setEnabled(d_enabled);
    holder->setCategory(this);
    holder->setNext(d_holders_p);
    d_holders_p = holder;
}

// ACCESSORS
bsl::ostream& Category::print(bsl::ostream& stream) const
{
    stream << "[ " << d_name_p << (d_enabled ? " ENABLED ]" : " DISABLED ]");
    return stream;
}

                            // --------------------
                            // class CategoryHolder
                            // --------------------

// MANIPULATORS
void CategoryHolder::reset()
{
    d_enabled    = false;
    d_category_p = 0;
    d_next_p     = 0;
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
