// bslalg_bidirectionalnode.h                                 -*-C++-*-
#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINKLISTNODE
#define INCLUDED_BSLALG_BIDIRECTIONALLINKLISTNODE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a node holding a value in a doubly-linked list.
//
//@CLASSES:
//   bslalg::BidirectionalNode : Node holding a value in a linked list
//
//@SEE_ALSO: bslalg_bidirectionallistutil, bslalg_hashtableimputil
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides a single POD-like class,
// 'bslalg::BidirectionalNode', used to represent a node in a doubly-linked
// (bidirectional) list holding a value of a parameterized type.  A
// 'bslalg::BidirectionalNode' publicly derives from
// 'bslalg::BidirectionalLink', so it may be used with
// 'bslalg::BidirectionalListUtil' functions, and adds an attribute 'value' of
// the parameterized 'VALUE_TYPE'.  The following inheritance hierarchy diagram
// shows the classes involved and their methods:
//..
//    ,---------------------------------.
//   ( bslalg::BidirectionalNode         )
//    `---------------------------------'
//                  |      value
//                  V
//     ,-------------------------.
//    ( bslalg::BidirectionalLink )
//     `-------------------------'
//                         ctor
//                         dtor
//                         setNextLink()
//                         setPrev
//                         nextLink()
//                         prev
//..
// This class is "POD-like" to facilitate efficient allocation and use in the
// context of container implementations.  In order to meet the essential
// requirements of a POD type, neither this 'class' nor
// 'bslalg::BidirectionalLink' define a constructor or destructor.  The
// manipulator, 'value', returns a modifiable reference to the stored object
// that may be constructed in-place, for example, by the appropriate
// 'bsl::allocator_traits' methods.  While not strictly a POD, this class is a
// *standard-layout* type according to the C++11 standard.
///-----------------------------------------------------------------------------
//..
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

namespace BloombergLP
{
namespace bslalg
{

                        // ===============================
                        // class BidirectionalNode
                        // ===============================

template <class VALUE_TYPE>
class BidirectionalNode: public bslalg::BidirectionalLink {
    // This POD-like 'class' describes a node suitable for use in a red-black
    // binary search tree of values of the parameterized 'VALUE'.  This class
    // is a "POD-like" to facilitate efficient allocation and use in the
    // context of a container implementation.  In order to meet the essential
    // requirements of a POD type, this 'class' does not define a constructor
    // or destructor.  The manipulator, 'value', returns a modifiable reference
    // to 'd_value' so that it may be constructed in-place by the appropriate
    // 'bsl::allocator_traits' object.

    // DATA
    VALUE_TYPE d_value;  // payload value

  private:
    // The following functions are not defined because a
    // 'BidirectionalNode' should never be constructed, destructed, or
    // assigned.  The 'd_value' member should be separately constructed and
    // destroyed using an appropriate 'bsl::allocator_traits' object.

    BidirectionalNode();                            // Declared but not defined
    BidirectionalNode(const BidirectionalNode&);             // Declared but not defined
    BidirectionalNode& operator=(const BidirectionalNode&);  // Declared but not defined
    ~BidirectionalNode();                           // Declared but not defined

  public:
    typedef VALUE_TYPE    ValueType;  // 

    // MANIPULATORS
    ValueType& value();
        // Return a reference providing modifiable access to the 'value' held
        // by this object.

    // ACCESSORS
    const ValueType& value() const;
        // Return a reference providing non-modifiable access to the 'value'
        // held by this object.
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -------------------------------
                        // class BidirectionalNode
                        // -------------------------------

template <class VALUE_TYPE>
inline
VALUE_TYPE& BidirectionalNode<VALUE_TYPE>::value()
{
    return d_value;
}

template <class VALUE_TYPE>
inline
const VALUE_TYPE& BidirectionalNode<VALUE_TYPE>::value() const
{
    return d_value;
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
