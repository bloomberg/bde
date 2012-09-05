// bslalg_bidirectionallink.h                                         -*-C++-*-
#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#define INCLUDED_BSLALG_BIDIRECTIONALLINK

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a basic link type for building doubly-linked lists.
//
//@CLASSES:
//   bslalg::BidirectionalLink : A node in a doubly-linked list
//
//@SEE_ALSO: bslalg_bidirectionallinklistutil, bslalg_hashtableutil
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides a single POD-like class,
// 'BidirectionalLink', used to represent a node in a doubly-linked list.  A
// 'BidirectionalLink' provides the address to its preceding node, and the
// address of it successor node.  A null-pointer value for either address
// signifies the end of the list.  'BidirectionalLink' does not, however,
// contain "payload" data (e.g., a value), as it is intended to work with
// generalized list operations (see 'bslalg_bidirectionallinklistutil').
// Clients creating a doubly-linked list must define their own node type that
// incorporates 'BidirectionalLink' (generally via inheritance), and that
// maintains the "value" stored in that node.
//
//-----------------------------------------------------------------------------
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: ...
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates ...
//..
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP
{
namespace bslalg
{

                          // =======================
                          // class BidirectionalLink
                          // =======================

class BidirectionalLink {
    // This POD-like 'class' describes a node suitable for use in a doubly-
    // linked (bidirectional) list, holding the addresses of the preceding and
    // succeeding nodes, either or both of which may be 0.  This class is
    // "POD-like" to facilitate efficient allocation and use in the context of
    // a container implementations.  In order to meet the essential
    // requirements of a POD type, this 'class' does not declare a constructor
    // or destructor.  However its data members are private.  It satisfies the
    // requirements of a *trivial* type and a *standard* *layout* type defined
    // by the C++11 standard.  Note that this type does not contain any
    // "payload" member data: Clients creating a doubly-linked list of data
    // must define an appropriate node type that incorporates
    // 'BidirectionalLink' (generally via inheritance), and that holds the
    // "value" of any data stored in that node.

  private:
    // DATA
    BidirectionalLink *d_next_p;  // The nextLink() node in a list traversal
    BidirectionalLink *d_prev_p;  // The preceding node in a list traversal

  public:
    // CREATORS
    //! BidirectionalLink() = default;
        // Create a 'BidirectionalLink' object having uninitialized values,
        // or zero-initialized values if value-initialized.

    //! BidirectionalLink(const BidirectionalLink& original) = default;
        // Create a 'BidirectionalLink' object having the same data member
        // values as the specified 'original' object.

    //! ~BidirectionalLink() = default;
        // Destroy this object.

    // MANIPULATORS
    //! BidirectionalLink& operator= (const BidirectionalLink& rhs) = default;
        // Assign to the data members of this object the values of the data
        // members of the specified 'rhs' object, and return a reference
        // providing modifiable access to this object.

    void setNextLink(BidirectionalLink *next);
        // Set the successor of this node to be the specified 'nextLink()' link.

    void setPreviousLink(BidirectionalLink *previous);
        // Set the predecessor of this node to be the specified 'prev' link.

    void reset();
        // Set the 'nextLink' and 'previousLink' attributes of this value to 0.

    // ACCESSORS
    BidirectionalLink *nextLink() const;
        // Return the address of the nextLink() node linked from this node.

    BidirectionalLink *previousLink() const;
        // Return the address of the preceding node linked from this node.

};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        //------------------------
                        // class BidirectionalLink
                        //------------------------

// MANIPULATORS
inline
void BidirectionalLink::setNextLink(BidirectionalLink *next)
{
    d_next_p = next;
}

inline
void BidirectionalLink::setPreviousLink(BidirectionalLink *previous)
{
    d_prev_p = previous;
}

inline
void BidirectionalLink::reset()
{
    d_prev_p = 0;
    d_next_p = 0;
}

// ACCESSORS
inline
BidirectionalLink *BidirectionalLink::nextLink() const
{
    return d_next_p;
}

inline
BidirectionalLink *BidirectionalLink::previousLink() const
{
    return d_prev_p;
}

}  // close namespace BloombergLP::bslalg
}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
