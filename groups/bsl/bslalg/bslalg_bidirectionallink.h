// bslalg_bidirectionallink.h                                             -*-C++-*-
#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#define INCLUDED_BSLALG_BIDIRECTIONALLINK

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a basic link type for building bidirectional lists. 
//
//@CLASSES:
//   bslalg::BidirectionalLink : Link in a bidirectional list
//
//@SEE_ALSO: bslalg_hashtableutil
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides...
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

                          // ========================
                          //     Hash table nodes
                          // ========================

class BidirectionalLink {
  private:
    // DATA
    BidirectionalLink *d_next_p;
    BidirectionalLink *d_prev_p;

  public:
    // CREATORS
    BidirectionalLink();
        // Create a link object having no next or previous node.

    // Trivial Copy operations
    // Trivial Destructor

    // MANIPULATORS

    void setNext(BidirectionalLink *next);

    void setPrev(BidirectionalLink *prev);

    // ACCESSORS

    BidirectionalLink *next() const;
        // Return the next node linked from this node.

    BidirectionalLink *prev() const;
        // Return the next node linked from this node.

};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        //--------------------
                        // class BidirectionalLink
                        //--------------------

// CREATORS
inline
BidirectionalLink::BidirectionalLink()
: d_next_p()
, d_prev_p()
{
}

// MANIPULATORS
inline
void BidirectionalLink::setNext(BidirectionalLink *next)
{
    d_next_p = next;
}

inline
void BidirectionalLink::setPrev(BidirectionalLink *prev)
{
    d_prev_p = prev;
}

// ACCESSORS
inline
BidirectionalLink *BidirectionalLink::next() const
{
    return d_next_p;
}

inline
BidirectionalLink *BidirectionalLink::prev() const
{
    return d_prev_p;
}

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
