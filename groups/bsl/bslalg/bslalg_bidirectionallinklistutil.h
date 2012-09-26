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
//@AUTHOR: Alisdair Meredith (ameredith1) Stefano Pacifico (spacifico1)
//
//@DESCRIPTION: This component provides a namespace,
// 'bslalg::BidirectionalLinkListUtil', containing utility functions for
// operating on doubly linked lists with nodes of type
// 'bslalg::BidirectionalLink'.  The operations assume that the linked lists
// are either 0 terminated (on both ends) or terminate with sentinel (valid)
// nodes.  The main operations include insertion and removal of a node from a
// list of nodes, and the *splicing* of ranges from one list into another one.
// Splicing is the operation of moving a sub-list, or range, of elements from
// one linked list and into a second list, at a
// specified position.
//
///Usage
///-----

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
       // value of the attributes 'nextLink' and 'previousLink' of 'newNode' is
       // set to 0.  After successful execution of this function the values of
       // the 'previousLink' and 'nextLink' attributes of all the links in the
       // list appropriately reflect the operation.  The behavior is undefined
       // unless '0 == target->previousLink()' is true or
       // 'isWellFormed(target->previousLink(), target)' is true.

    static
    void insertLinkAfterTarget(BidirectionalLink *newNode,
                               BidirectionalLink *target);
       // Insert the specified 'newNode' after the specified 'target' node in
       // the linked list that contains 'target'.  If the node following
       // 'target' is 0, then set the 'nextLink' attribute of 'newNode' to 0.
       // After successful execution of this function the values of the
       // 'previousLink' and 'nextLink' attributes of all the links in the list
       // appropriately reflect the operation.  The behavior is undefined
       // unless 'isWellFormed(target, target->nextLink())' is true.

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
        // 'first' and 'last' are members of the same linked list; 'first'
        // precedes 'last' in the list, or 'first == last'; 'target' is not a
        // node contained in the closed range '[first, last]'; and
        // 'isWellFormed(first, last)' is true.

    static
    void unlink(BidirectionalLink *node);
        // Unlink the specified 'node' from the linked list of which it is a
        // member.  After successful execution of this function the values of
        // the 'previousLink' and 'nextLink' attributes of all the links in the
        // origin and destination lists appropriately reflect the operation
        // Note that this method does *not* change the value for the 'nextLink'
        // and 'previousLink' attributes of 'node'.  The behavior is
        // undefined unless
        // 'isWellFormed(node->previousLink(), node->nextLink())' is true.
};

} // namespace BloombergLP::bslalg

} // namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
