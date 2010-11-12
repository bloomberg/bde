// bael_attributecontainerlist.cpp         -*-C++-*-
#include <bael_attributecontainerlist.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_attributecontainerlist_cpp,"$Id$ $CSID$")

#include <bael_attribute.h>           // for testing
#include <bael_attributecontainer.h>

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                    // ---------------------------------
                    // class bael_AttributeContainerList
                    // ---------------------------------

// CREATORS
bael_AttributeContainerList::bael_AttributeContainerList(
                           const bael_AttributeContainerList&  original,
                           bslma_Allocator                    *basicAllocator)
: d_head_p(0)
, d_free_p(0)
, d_length(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
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
bael_AttributeContainerList& bael_AttributeContainerList::operator=(
                                        const bael_AttributeContainerList& rhs)
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

bael_AttributeContainerList::iterator
bael_AttributeContainerList::pushFront(
                                      const bael_AttributeContainer *container)
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

void bael_AttributeContainerList::remove(const iterator& element)
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

void bael_AttributeContainerList::removeAll()
{
    // This operation does not release any memory, it prepends the list of
    // elements to the free list.

    // If there are no elements, simply return.
    if (!d_head_p) {
        return;                                                      // RETURN
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


void bael_AttributeContainerList::removeAllAndRelease()
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
bool bael_AttributeContainerList::hasValue(const bael_Attribute& value) const
{
    Node *node = d_head_p;
    while (node) {
        if (node->d_value_p->hasValue(value)) {
            return true;
        }
        node = node->d_next_p;
    }
    return false;
}

bsl::ostream&
bael_AttributeContainerList::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    char EL = (spacesPerLevel < 0) ? ' ' : '\n';

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[" << EL;

    Node *node = d_head_p;
    while (node) {
        node->d_value_p->print(stream, level+1, spacesPerLevel);
        node = node->d_next_p;
    }
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "]" << EL;
    return stream << bsl::flush;
}

// FREE OPERATORS
bool operator==(const bael_AttributeContainerList& lhs,
                const bael_AttributeContainerList& rhs)
{
    if (lhs.numContainers() != rhs.numContainers()) {
        return false;                                                 // RETURN
    }
    bael_AttributeContainerList::iterator lIt = lhs.begin();
    bael_AttributeContainerList::iterator rIt = rhs.begin();
    for (; lIt != lhs.end(); ++lIt, ++rIt) {
        if (*lIt != *rIt) {
            return false;
        }
    }
    return true;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
