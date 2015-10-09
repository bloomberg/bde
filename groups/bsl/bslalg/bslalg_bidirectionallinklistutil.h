// bslalg_bidirectionallinklistutil.h                                 -*-C++-*-
#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINKLISTUTIL
#define INCLUDED_BSLALG_BIDIRECTIONALLINKLISTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities to maintain bidirectional list data structures.
//
//@CLASSES:
//  bslalg::BidirectionalLinkListUtil: utilities to maintain linked lists
//
//@SEE_ALSO: bslalg_bidirectionallink, bslalg_hashtableimputil
//
//@DESCRIPTION: This component provides a namespace,
// 'bslalg::BidirectionalLinkListUtil', containing utility functions for
// operating on doubly linked lists with nodes of type
// 'bslalg::BidirectionalLink'.  The operations assume that the linked lists
// are either 0 terminated (on both ends) or terminate with sentinel (valid)
// nodes.  The main operations include insertion and removal of a node from a
// list of nodes, and the *splicing* of ranges from one list into another one.
// Splicing is the operation of moving a sub-list, or range, of elements from
// one linked list and into a second list, at a specified position.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating and Using a List Template Class
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// First, since 'Link' neither has a constructor nor is a POD (which would make
// aggregate initialization possible), we create a function 'makeLink' to
// assemble a link from two pointers:
//..
//  Link makeLink(Link *prev, Link *next)
//  {
//      Link result;
//      result.setPreviousLink(prev);
//      result.setNextLink(    next);
//
//      return result;
//  }
//..
// Then, we create a function that will, passed two links that are endpoints
// of a linked list from the specified 'first' to 'last' though the 'nextLink'
// pointers, count the number of nodes in the list including both endpoints.
//..
//  int length(Link *first, Link *last)
//  {
//      int result = 0;
//      Link *p = first;
//      while (p && ++result && last != p) {
//          p = p->nextLink();
//      }
//
//      return result;
//  }
//..
// Next, in our 'main', we declare a 'typedef' for the component name and a
// a constant 'invalid' garbage pointer we use when we want data to be garbage.
//..
//  typedef BidirectionalLinkListUtil Util;
//  Link * const invalid = (Link *) 0XBADDEED5;
//..
// Then, we create a linked list of links and use 'isWellFormed' to verify
// that it is well formed, and call the 'length' method we just created to
// verify its length.
//..
//  Link usageData[] = {
//      makeLink(0,             &usageData[1]),
//      makeLink(&usageData[0], &usageData[2]),
//      makeLink(&usageData[1], &usageData[3]),
//      makeLink(&usageData[2], 0            )  };
//
//  assert(Util::isWellFormed(      &usageData[0], &usageData[3]));
//  assert(4 == length(&usageData[0], &usageData[3]));
//..
// Next, we create two new links 'front' and 'back', and initialize them with
// garbage:
//..
//  Link front = makeLink(invalid, invalid);
//  Link back  = makeLink(invalid, invalid);
//..
// Then, we use our component's 'insertLinkBeforeTarget' and
// 'insertLinkAfterTarget' to concatenate 'front' to the front of the list and
// 'back' to its rear:
//..
//  Util::insertLinkBeforeTarget(&front, &usageData[0]);
//  Util::insertLinkAfterTarget( &back,  &usageData[3]);
//..
// Next, We examine the new list and verify we now have a well-formed list, 2
// longer than the old list:
//..
//  assert(0 == front.previousLink());
//  assert(0 == back .nextLink());
//  assert(Util::isWellFormed(          &front, &back));
//  assert(6 == length(&front, &back));
//..
// Then, we use our component's 'unlink' method to remove two nodes from our
// list.  Note that the state of the removed nodes is undefined:
//..
//  Util::unlink(&usageData[1]);
//  Util::unlink(&usageData[3]);
//..
// Next, we verify that the new list is well formed and 2 elements shorter than
// it was before we removed those two nodes:
//..
//  assert(Util::isWellFormed(&front, &back));
//  assert(4 == length(&front, &back));
//..
// Then, we weave the two discarded nodes into a new, second list of two nodes,
// and use 'isWellFormed' and 'length' to verify it is as we expect:
//..
//  usageData[1] = makeLink(0, &usageData[3]);
//  usageData[3] = makeLink(&usageData[1], 0);
//  assert(Util::isWellFormed(&usageData[1], &usageData[3]));
//  assert(2 ==        length(&usageData[1], &usageData[3]));
//..
// Next, we use our component's 'spliceListBeforeTarget' method to remove the
// middle nodes from the longer list and append them to the end of shorter
// list.  Note that the splicing function not only adds the sequence to the new
// list, it also splices the list the sequence is removed from so that both are
// well-formed lists:
//..
//  Util::spliceListBeforeTarget(&usageData[0],
//                               &usageData[2],
//                               &usageData[3]);
//..
// Then, we use 'isWellFormed' and 'length' to verify the state of our two
// lists:
//..
//  assert(Util::isWellFormed(&usageData[1], &usageData[3]));
//  assert(4 ==        length(&usageData[1], &usageData[3]));
//
//  assert(Util::isWellFormed(&front, &back));
//  assert(2 ==        length(&front, &back));
//..
// Next, we call 'spliceListBeforeTarget' again to join our two lists into one:
//..
//  Util::spliceListBeforeTarget(&usageData[1],
//                               &usageData[3],
//                               &back);
//..
// Now, we use 'isWellFormed' and 'length' to verify the state of our one
// remaining list:
//..
//  assert(Util::isWellFormed(&front, &back));
//  assert(6 ==        length(&front, &back));
//  assert(0 == front.previousLink());
//  assert(0 == back .nextLink());
//..
// Finally, we traverse our list in both directions, examining each node to
// verify all the links and that the sequence is as expected:
//..
//  Link *p = &front;
//  assert(0 == p->previousLink());
//  p = p->nextLink();
//  assert(&usageData[1] == p);
//  p = p->nextLink();
//  assert(&usageData[0] == p);
//  p = p->nextLink();
//  assert(&usageData[2] == p);
//  p = p->nextLink();
//  assert(&usageData[3] == p);
//  p = p->nextLink();
//  assert(&back         == p);
//  assert(0 == p->nextLink());
//
//  p = p->previousLink();
//  assert(&usageData[3] == p);
//  p = p->previousLink();
//  assert(&usageData[2] == p);
//  p = p->previousLink();
//  assert(&usageData[0] == p);
//  p = p->previousLink();
//  assert(&usageData[1] == p);
//  p = p->previousLink();
//  assert(&front        == p);
//  assert(0 == p->previousLink());
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP
{

namespace bslalg
{

class BidirectionalLink;

                        // ========================================
                        // struct bslalg::BidirectionalLinkListUtil
                        // ========================================

struct BidirectionalLinkListUtil {
    // This 'struct' provides a namespace for utility functions that manipulate
    // linked lists based on 'bslalg::BidirectionalLink' nodes, including
    // insertion, removal, and *splicing*.

    // CLASS METHODS
    static
    void insertLinkBeforeTarget(BidirectionalLink *newNode,
                                BidirectionalLink *target);
        // Insert the specified 'newNode' before the specified 'target' node in
        // the linked list that contains 'target'.  If 'target' is 0, then the
        // value of the attributes 'nextLink' and 'previousLink' of 'newNode'
        // is set to 0.  After successful execution of this function the values
        // of the 'previousLink' and 'nextLink' attributes of all the links in
        // the list appropriately reflect the operation.  The behavior is
        // undefined unless '0 == target->previousLink()' is true or
        // 'isWellFormed(target->previousLink(), target)' is true.

    static
    void insertLinkAfterTarget(BidirectionalLink *newNode,
                               BidirectionalLink *target);
        // Insert the specified 'newNode' after the specified 'target' node in
        // the linked list that contains 'target'.  If the node following
        // 'target' is 0, then set the 'nextLink' attribute of 'newNode' to 0.
        // After successful execution of this function the values of the
        // 'previousLink' and 'nextLink' attributes of all the links in the
        // list appropriately reflect the operation.  The behavior is undefined
        // unless '0 != newNode' and '0 != target'.  The behavior is also
        // undefined unless '0 == target->nextLink()' is true or
        // 'isWellFormed(target, target->nextLink())' are true.

    static
    bool isWellFormed(BidirectionalLink *head, BidirectionalLink *tail);
        // Return true if the bidirectional list starting from the specified
        // 'head', and ending with the specified 'tail' is well formed.  A
        // bidirectional list is well formed if 'tail == head' (0 values are
        // allowed) or all of the following conditions are met (note that
        // 'head' is renamed to 'h' and 'tail' to 't' for brevity):
        //
        //: 1 'h' and 't' are valid addresses.
        //:
        //: 2 'h->nextLink()->previousLink() == h' is true.
        //:
        //: 3 '!h->previousLink() || h->previousLink()->nextLink() == h'
        //:    is true.
        //:
        //: 4 't->previousLink()->nextLink() == t' is true.
        //:
        //: 5 '!t->nextLink() || t->nextLink()->previousLink() == t'
        //:    is true.
        //:
        //: 6 For each 'link' in the list different than 'h' and 't' both
        //:   'link->nextLink()->previousLink() == link' and
        //:   'link->previousLink()->nextLink() == link' are true.
        //
        // The behavior is undefined unless 'tail' can be reached from 'head'
        // following the chain of 'nextLink' attributes of all the nodes in the
        // open range '[head, tail)'.

    static
    void spliceListBeforeTarget(BidirectionalLink *first,
                                BidirectionalLink *last,
                                BidirectionalLink *target);
        // Unlink and move (splice) the elements of a doubly-linked list
        // included in the closed range '[first, last]' out of their original
        // list and into another doubly-linked list before the specified
        // 'target' node.  If 'target' is 0, then the the elements are
        // extracted and form a new list such that '0 == first->previousLink()'
        // and '0 == last->nextLink()' .  After successful execution of this
        // function the values of the 'previousLink' and 'nextLink' attributes
        // of all the links in the origin and destination lists appropriately
        // reflect the operation.  The behavior is undefined unless both
        // 'first' and 'last' are non-zero members of the same linked list;
        // 'first' precedes 'last' in the list, or 'first == last'; 'target' is
        // not a node contained in the closed range '[first, last]'; and
        // 'isWellFormed(first, last)' is true.

    static
    void unlink(BidirectionalLink *node);
        // Unlink the specified 'node' from the linked list of which it is a
        // member.  After successful execution of this function the values of
        // the 'previousLink' and 'nextLink' attributes of all the links in the
        // origin and destination lists appropriately reflect the operation
        // Note that this method does *not* change the value for the 'nextLink'
        // and 'previousLink' attributes of 'node'.  The behavior is
        // undefined unless '!node->previousLink()', '!node->nextLink()', or
        // 'isWellFormed(node->previousLink(), node->nextLink())' are true.
};

}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
