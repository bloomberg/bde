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
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides utilities for...
//
//-----------------------------------------------------------------------------
//..
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

                          // ===============================
                          // class BidirectionalLinkListUtil
                          // ===============================

struct BidirectionalLinkListUtil {
    static
    void insertLinkBeforeTarget(BidirectionalLink *newNode,
                                BidirectionalLink *target);
       // Insert the specified 'newNode' into the doubly-linked list before the
       // specified 'tail' node.  If 'tail' is null, then 'newNode' becomes an
       // entire list, and this function has no observable effect.  The
       // behavior is undefined unless 'newNode' is not currently linked into
       // any list, such as having a null pointer for both 'nextLink()' and 'prev'
       // addresses.
       // specified 'tail' node.
    
    static
    void insertLinkAfterTarget(BidirectionalLink *newNode,
                               BidirectionalLink *target);
       // Insert the specified 'newNode' into the doubly-linked list after the
       // specified 'head' node.

    static
    void spliceListBeforeTarget(BidirectionalLink *first,
                                BidirectionalLink *last,
                                BidirectionalLink *target);
    // Splice the segment of a doubly-linked list specified by the closed range
    // '[first, last]' out of its original list and into the doubly-linked
    // target list before the specified 'before' node.  If 'before' is null,
    // then the splice is extracted and becomes a whole list in its own right,
    // so that 'first->previousLink()' and 'last->nextLink()()' will both return null
    // pointers.  The behavior is undefined unless both 'first' and 'last' are
    // members of the same linked list, and that 'first' precedes last in the
    // list, or 'first == last'.

    static
    void unlink(BidirectionalLink *node);
        // Unlink the specified 'node' from the linked list it is a member of.
        // Note that this method does *not* change the 'nextLink()' and 'prev'
        // attributes of 'node' itself, just those of the adjacent nodes in
        // the original list.  The behavior is undefined unless 'node' is a
        // member of a linked list.  Note that a list having only will element
        // would be represented by a node having 'previousLink()' == 0 == 'nextLink()()', so
        // this is well defined.
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
