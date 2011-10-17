// bslstl_rbtree_nodecomparator.h                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_RBTREE_NODECOMPARATOR
#define INCLUDED_BSLSTL_RBTREE_NODECOMPARATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: 
//
//@CLASSES:
//   bslstl::RbTree_NodeComparator: 
//
//@SEE_ALSO:
//
//@AUTHOR: 
//
//@DESCRIPTION: 
//
///Usage
///-----

#ifndef INCLUDED_BSLSTL_RBTREE_NODE
#include <bslstl_rbtree_node.h>
#endif

namespace BloombergLP {
namespace bslstl {

                    // ===========================
                    // class RbTree_NodeComparator
                    // ===========================

template <typename VALUE_TYPE, typename COMPARATOR_TYPE>
class RbTree_NodeComparator : private COMPARATOR_TYPE {
    // For use only by 'bslstl::RbTree' implementation.  This class provides
    // comparison operations for a 'RBTRee_Node<VALUE_TYPE>' using the
    // paramterized 'COMPARATOR_TYPE' to compare the values held by a node.
    // This type supports comparing two nodes, as well as comparing a 
    // node with a 'VALUE_TYPE' object, and can be used as a functor for
    // 'bslalg::RbTreePrimitives' operations requiring a comparator.  It is 
    // intended to be used as a private base-class to take advantage of the
    // empty-base-class optimization in the case where the paramterized
    // 'COMPARATOR_TYPE' has 0 size. 

    typedef COMPARATOR_TYPE value_comparator;
        // Alias for the parameterized 'VALUE_TYPE' comparison operation.

    const value_comparator& valueComparator() const;
        // Return a reference providing non-modifiable access to the
        // 'VALUE_TYPE' comparison functor.  Note that this operation returns
        // a base-class ('COMPARATOR_TYPE') reference to this object.

  public:
    // PUBLIC TYPES
    typedef RbTree_Node<VALUE_TYPE> NodeType;    
        // An alias (required by 'RbTreePrimitives') for the node-type of on
        // which comparisons are performed. 

    // CREATORS
    RbTree_NodeComparator();
        // Create a node-comparator that will use a default constructed
        // 'COMPRATOR_TYPE' for comparing 'VALUE_TYPE' objects.

    RbTree_NodeComparator(const COMPARATOR_TYPE& valueComparator);
        // Create a node-comparator that will use the specified
        // 'valueComparator' for comparing 'VALUE_TYPE' objects.

    // ACCESSORS
    bool operator()(const RbTree_Node<VALUE_TYPE>& lhs, 
                    const RbTree_Node<VALUE_TYPE>& rhs) const;
    bool operator()(const VALUE_TYPE&              lhs, 
                    const RbTree_Node<VALUE_TYPE>& rhs) const;
    bool operator()(const RbTree_Node<VALUE_TYPE>& lhs, 
                    const VALUE_TYPE&              rhs) const;
        // Return 'true' if the specified 'lhs' is less than (ordered before)
        // the specified 'rhs', and 'false' otherwise.  A node is compared
        // with other nodes and value by their 'd_value' data member.  A value,
        // of the parameterized 'VALUE_TYPE', compares less than another value
        // if the parameterized 'COMPARATOR_TYPE' supplied at construction
        // returns 'true'.  
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                    // ---------------------------
                    // class RbTree_NodeComparator
                    // ---------------------------


// PRIVATE ACCESSORS
template<typename VALUE_TYPE, typename COMPARATOR_TYPE>
inline
const COMPARATOR_TYPE& 
RbTree_NodeComparator<VALUE_TYPE, COMPARATOR_TYPE>::valueComparator() const
{
    return *this; // Return a reference to the base class
}

// CREATORS
template<typename VALUE_TYPE, typename COMPARATOR_TYPE>
inline
RbTree_NodeComparator<VALUE_TYPE, COMPARATOR_TYPE>::
RbTree_NodeComparator()
: value_comparator(COMPARATOR_TYPE())
{
}

template<typename VALUE_TYPE, typename COMPARATOR_TYPE>
inline
RbTree_NodeComparator<VALUE_TYPE, COMPARATOR_TYPE>::
RbTree_NodeComparator(const COMPARATOR_TYPE& valueComparator)
: value_comparator(valueComparator)
{
}

// ACCESSORS
template<typename VALUE_TYPE, typename COMPARATOR_TYPE>
inline 
bool RbTree_NodeComparator<VALUE_TYPE, COMPARATOR_TYPE>::operator()(
                                      const RbTree_Node<VALUE_TYPE>& lhs, 
                                      const RbTree_Node<VALUE_TYPE>& rhs) const
{
    return valueComparator()(lhs.d_value, rhs.d_value);
}

template<typename VALUE_TYPE, typename COMPARATOR_TYPE>
inline 
bool RbTree_NodeComparator<VALUE_TYPE, COMPARATOR_TYPE>::operator()(
                                      const VALUE_TYPE&              lhs, 
                                      const RbTree_Node<VALUE_TYPE>& rhs) const
{
    return valueComparator()(lhs, rhs.d_value);
}

template<typename VALUE_TYPE, typename COMPARATOR_TYPE>
inline 
bool RbTree_NodeComparator<VALUE_TYPE, COMPARATOR_TYPE>::operator()(
                                      const RbTree_Node<VALUE_TYPE>& lhs, 
                                      const VALUE_TYPE&              rhs) const
{
    return valueComparator()(lhs.d_value, rhs);
}

}  // close namespace bslstl
}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
