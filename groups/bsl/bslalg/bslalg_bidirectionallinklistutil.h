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
//  bslalg::BidirectionalLinkListUtil: utilities to maintain bidirectional list data structures
//
//@SEE_ALSO: bslalg_bidirectionallink, bslalg_hashtableutil
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

                          // ===================
                          // class HashTableUtil
                          // ===================

struct BidirectionalLinkListUtil {
    static
    void insertLinkBeforeTail(BidirectionalLink *newNode,
                              BidirectionalLink *tail);
       // Insert the specified 'newNode' into the doubly-linked list before the
       // specified 'tail' node.

    static
    void spliceListBeforeLink(BidirectionalLink *first,
                              BidirectionalLink *last,
                              BidirectionalLink *before);
    // Splice the segment of a doubly-linked list specified by the closed range
    // '[first, last]' into the doubly-linked target list before the specified
    // 'before' node.

    static
    void unlink(BidirectionalLink *node);
        // Unlink the specified 'node' from a linked list.
        // Note that this method does *not* change
        // the 'next' and 'prev' attributes of 'node' itself.
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        //--------------------
                        // class HashTableUtil
                        //--------------------

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
