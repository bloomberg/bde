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
                        // ----------------------------------------
                        // struct bslalg::BidirectionalLinkListUtil
                        // ----------------------------------------

void BidirectionalLinkListUtil::insertLinkBeforeTarget(
                                                   BidirectionalLink  *newNode,
                                                   BidirectionalLink  *target)
{
    BSLS_ASSERT(newNode);
#ifdef BDE_BUILD_TARGET_SAFE_2
    BSLS_ASSERT_SAFE(isWellFormed(target, target));
#endif

    // Prepending before an empty list is *explicitly* *allowed*

    if (!target) {
        newNode->reset();
    }
    else if (BidirectionalLink *prev = target->previousLink()) {
        newNode->setPreviousLink(prev);
        prev->setNextLink(newNode);
        newNode->setNextLink(target);
        target->setPreviousLink(newNode);
    }
    else {
        newNode->setPreviousLink(0);  // asserted precondition
        newNode->setNextLink(target);
        target->setPreviousLink(newNode);
    }
}

void BidirectionalLinkListUtil::insertLinkAfterTarget(
                                                    BidirectionalLink *newNode,
                                                    BidirectionalLink *target)
{
    BSLS_ASSERT_SAFE(newNode);
    BSLS_ASSERT_SAFE(target);

    BidirectionalLink *next = target->nextLink();

#ifdef BDE_BUILD_TARGET_SAFE_2
    BSLS_ASSERT_SAFE(!next || isWellFormed(target, next));
#endif

    target->setNextLink(newNode);
    if (next) {
        next->setPreviousLink(newNode);
    }

    newNode->setPreviousLink(target);
    newNode->setNextLink(next);
}

bool BidirectionalLinkListUtil::isWellFormed(BidirectionalLink *head,
                                             BidirectionalLink *tail)
{
    if (!head != !tail) {
        return false;                                                 // RETURN
    }

    if (!head) {          // empty list
        return true;                                                  // RETURN
    }

    // Check that the nodes adjacent to the ends point back at us.

    if   ((head->previousLink() && head->previousLink()->nextLink() != head)
       || (tail->nextLink()     && tail->nextLink()->previousLink() != tail)) {
        return false;                                                 // RETURN
    }

    if (head == tail) {    // single element list
        return true;                                                  // RETURN
    }

    for (const BidirectionalLink *prev = head, *cursor = head->nextLink();
                               cursor;
                                  prev = cursor, cursor = cursor->nextLink()) {
        if (cursor->previousLink() != prev) {
            return false;                                             // RETURN
        }

        if (tail == cursor) {
            return true;                                              // RETURN
        }
    }

    // We ran off the end of the list without reaching 'tail'.

    return false;
}

void BidirectionalLinkListUtil::spliceListBeforeTarget
                                                   (BidirectionalLink *first,
                                                    BidirectionalLink *last,
                                                    BidirectionalLink *target)
{
    BSLS_ASSERT_SAFE(first);
    BSLS_ASSERT_SAFE(last);
#ifdef BDE_BUILD_TARGET_SAFE_2
    // Test to avoid creating loops is O(N) expensive, so check only in SAFE_2

    BidirectionalLink *cursor = first;
    while(cursor != last->nextLink()) {
        BSLS_ASSERT_SAFE(cursor != target);
        cursor = cursor->nextLink();
    }
    BSLS_ASSERT_SAFE(isWellFormed(first, last));
#endif

    // unlink from existing list

    if (BidirectionalLink* prev = first->previousLink()) {
        prev->setNextLink(last->nextLink());
    }
    if (BidirectionalLink* next = last->nextLink()) {
        next->setPreviousLink(first->previousLink());
    }

    // update into spliced location:

    if (!target) {
        // Prepending target an empty list is *explicitly* *allowed*
        // The "spliced" segment is still extracted from the original list

        first->setPreviousLink(0);  // redundant with pre-condition
        last->setNextLink(0);
    }
    else {
        if (BidirectionalLink *prev = target->previousLink()) {
            first->setPreviousLink(prev);
            prev->setNextLink(first);
        }
        else {
            first->setPreviousLink(0);
        }

        last->setNextLink(target);
        target->setPreviousLink(last);
    }
}

void BidirectionalLinkListUtil::unlink(BidirectionalLink *node)
{
    BSLS_ASSERT_SAFE(node);

    BidirectionalLink *prev = node->previousLink(), *next = node->nextLink();

    if (prev) {
        if (next) {
            BSLS_ASSERT_SAFE(isWellFormed(prev, next));

            next->setPreviousLink(prev);
            prev->setNextLink(next);
        }
        else {
            prev->setNextLink(0);
        }
    }
    else if (next) {
        next->setPreviousLink(0);
    }
}

}  // close namespace BloombergLP::bslalg
}  // close namespace BloombergLP
// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
