// ball_attributecontainerlist.cpp                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_attributecontainerlist.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_attributecontainerlist_cpp,"$Id$ $CSID$")

#include <ball_attribute.h>           // for testing
#include <ball_attributecontainer.h>

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace ball {
                    // ---------------------------------
                    // class AttributeContainerList
                    // ---------------------------------

// CREATORS
AttributeContainerList::AttributeContainerList(
                                 const AttributeContainerList&  original,
                                 bslma::Allocator              *basicAllocator)
: d_head_p(0)
, d_free_p(0)
, d_length(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    Node **prevNextAddr = &d_head_p;
    for (iterator it = original.begin(); it != original.end(); ++it) {

        // Incrementing 'd_length' and assigning 'd_next_p' to 0 in each
        // iteration ensures this object is left in a valid state if an
        // exception is thrown creating a new element.
        Node *node = new (*d_allocator_p) Node();
        node->d_next_p = 0;
        node->d_value_p = *it;
        node->d_prevNextAddr_p = prevNextAddr;
        *prevNextAddr = node;
        prevNextAddr  = &node->d_next_p;
        ++d_length;
    }
}

// MANIPULATORS
AttributeContainerList& AttributeContainerList::operator=(
                                        const AttributeContainerList& rhs)
{
    if (&rhs != this) {
        // This could be made more efficient (by not using 'removeAll()' but
        // this is easier - this operation should not be called in practice.

        // Move all the elements to the free list.
        removeAll();

        // Append the 'rhs' elements to this list.
        Node **prevNextAddr = &d_head_p;
        for (iterator it = rhs.begin(); it != rhs.end(); ++it) {
            Node *node;
            if (d_free_p) {
                node = d_free_p;
                d_free_p = d_free_p->d_next_p;
            }
            else {
                node = new (*d_allocator_p) Node();
            }
            node->d_value_p = *it;
            node->d_next_p = 0;
            node->d_prevNextAddr_p = prevNextAddr;
            *prevNextAddr = node;
            prevNextAddr = &node->d_next_p;
            ++d_length;
        }


    }
    return *this;
}

AttributeContainerList::iterator
AttributeContainerList::pushFront(const AttributeContainer *container)
{
    Node *node;
    if (d_free_p) {
        node = d_free_p;
        d_free_p = d_free_p->d_next_p;
    }
    else {
        node = new (*d_allocator_p) Node();
    }
    node->d_value_p        = container;
    node->d_next_p         = d_head_p;
    node->d_prevNextAddr_p = &d_head_p;
    if (d_head_p) {
        d_head_p->d_prevNextAddr_p = &node->d_next_p;
    }
    d_head_p = node;
    ++d_length;
    return iterator(node);
}

void AttributeContainerList::remove(const iterator& element)
{
    Node *node = element.d_node_p;
    *node->d_prevNextAddr_p = node->d_next_p;
    if (node->d_next_p) {
        node->d_next_p->d_prevNextAddr_p = node->d_prevNextAddr_p;
    }
    node->d_next_p  = d_free_p;
    d_free_p = node;
    --d_length;
}

void AttributeContainerList::removeAll()
{
    // This operation does not release any memory, it prepends the list of
    // elements to the free list.

    // If there are no elements, simply return.
    if (!d_head_p) {
        return;                                                       // RETURN
    }

    // Find the tail of the element list.
    Node *node = d_head_p;
    while (node->d_next_p) {
        node = node->d_next_p;
    }

    // Append the free list to the tail of the element list.
    if (d_free_p) {
        d_free_p->d_prevNextAddr_p  = &node->d_next_p;
        node->d_next_p              = d_free_p;
    }

    // Set the element list to the free list.
    d_free_p = d_head_p;
    d_head_p = 0;
    d_length = 0;
}


void AttributeContainerList::removeAllAndRelease()
{
    while (d_head_p) {
        Node *node = d_head_p;
        d_head_p = d_head_p->d_next_p;
        d_allocator_p->deleteObjectRaw(node);
    }
    while (d_free_p) {
        Node *node = d_free_p;
        d_free_p = d_free_p->d_next_p;
        d_allocator_p->deleteObjectRaw(node);
    }
    d_length = 0;
}


// ACCESSORS
bool AttributeContainerList::hasValue(const Attribute& value) const
{
    Node *node = d_head_p;
    while (node) {
        if (node->d_value_p->hasValue(value)) {
            return true;                                              // RETURN
        }
        node = node->d_next_p;
    }
    return false;
}

bsl::ostream&
AttributeContainerList::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    char EL = (spacesPerLevel < 0) ? ' ' : '\n';

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[" << EL;

    Node *node = d_head_p;
    while (node) {
        node->d_value_p->print(stream, level+1, spacesPerLevel);
        node = node->d_next_p;
    }
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "]" << EL;
    return stream << bsl::flush;
}
}  // close package namespace

// FREE OPERATORS
bool ball::operator==(const AttributeContainerList& lhs,
                      const AttributeContainerList& rhs)
{
    if (lhs.numContainers() != rhs.numContainers()) {
        return false;                                                 // RETURN
    }
    AttributeContainerList::iterator lIt = lhs.begin();
    AttributeContainerList::iterator rIt = rhs.begin();
    for (; lIt != lhs.end(); ++lIt, ++rIt) {
        if (*lIt != *rIt) {
            return false;                                             // RETURN
        }
    }
    return true;
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
