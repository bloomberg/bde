// bslalg_bidirectionallinklistutil.cpp                               -*-C++-*-
#include <bslalg_bidirectionallinklistutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslalg_bidirectionallink.h>

#include <bsls_assert.h>
#include <bsls_types.h>

namespace BloombergLP
{

namespace bslalg
{

void BidirectionalLinkListUtil::insertLinkBeforeTail(BidirectionalLink *newNode,
                                                     BidirectionalLink *tail)
       // Insert the specified 'newNode' into the doubly-linked list before the
       // specified 'tail' node.
{
//    BSLS_ASSERT(tail);
    BSLS_ASSERT(newNode);
    // newNode must be in an unlinked state
    BSLS_ASSERT(!newNode->next());
    BSLS_ASSERT(!newNode->prev());

    if (!tail) {  // Prepending before an empty list is *explicitly* *allowed*
//        newNode->setPrev(0);  // redundant with pre-condition
//        newNode->setNext(0);
    }
    else if (BidirectionalLink *prev = tail->prev()) {  // decode this value only once
        newNode->setPrev(prev);
        prev->setNext(newNode);
        newNode->setNext(tail);
        tail->setPrev(newNode);
    }
    else {
        // newNode CANNOT be end-of-bucket, as 'tail' is in the same bucket
//        newNode->setPrev(0);  // asserted precondition
        newNode->setNext(tail);
        tail->setPrev(newNode);
    }
}

void BidirectionalLinkListUtil::spliceListBeforeLink(BidirectionalLink *first,
                                                     BidirectionalLink *last,
                                                     BidirectionalLink *before)
    // Splice the segment of a doubly-linked list specified by the closed range
    // '[first, last]' into the doubly-linked target list before the specified
    // 'before' node.
{
    BSLS_ASSERT(first);
    BSLS_ASSERT(last);

    // unlink from existing list
    if (BidirectionalLink* prev = first->prev()) {
        prev->setNext(last->next());
    }
    if (BidirectionalLink* next = last->next()) {
        next->setPrev(first->prev());
    }

    // update into spliced location:
    if (!before) {
        // Prepending before an empty list is *explicitly* *allowed*
        // The "spliced" segment is still extracted from the original list
        first->setPrev(0);  // redundant with pre-condition
        last->setNext(0);
    }
    else {
        if (BidirectionalLink *prev = before->prev()) {
            first->setPrev(prev);
            prev->setNext(first);
        }
        else {
            first->setPrev(0);
        }

        last->setNext(before);
        before->setPrev(last);
    }
}


void BidirectionalLinkListUtil::unlink(BidirectionalLink *node)
        // Unlink the specified 'node' from a linked list.
        // Note that this method does *not* change
        // the 'next' and 'prev' attributes of 'node' itself.
{
    BSLS_ASSERT_SAFE(node);

    if (BidirectionalLink *prev = node->prev()) {
        if (BidirectionalLink *next = node->next()) {
            next->setPrev(prev);
            prev->setNext(next);
        }
        else {
            prev->setNext(0);
        }
    }
    else if(BidirectionalLink *next = node->next()) {
        next->setPrev(0);
    }
}

}  // close namespace BloobmergLP::bslalg
}  // close namespace BloombergLP
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
