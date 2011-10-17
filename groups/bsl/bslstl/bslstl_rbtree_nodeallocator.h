// bslstl_rbtree_nodeallocator.h                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_RBTREE_NODEALLOCATOR
#define INCLUDED_BSLSTL_RBTREE_NODEALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: 
//
//@CLASSES:
//   bslstl::RbTree_Nodeallocator: 
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

#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#include <bslstl_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

namespace BloombergLP {
namespace bslstl {

                       // ================================
                       // class RbTree_NodeAllocator_Types
                       // ================================

template <class VALUE_TYPE, class ALLOCATOR_TYPE>
struct RbTree_NodeAllocator_Types {
    // For use only by 'bslstl::RbTree' implementation.  Provides a set of
    // types used toe define the base-class of a 'RbTree_NodeAllocator'. 
        
    typedef RbTree_Node<VALUE_TYPE> Node;
        // Alias for a node holding a value of the parameterized 'VALUE_TYPE'

    typedef typename bsl::allocator_traits<ALLOCATOR_TYPE>::template 
                                              rebind_traits<Node> AllocTraits;
        // Alias for the allocator traits for the parameterized
        // 'ALLOCATOR_TYPE' applied to an 'RbTree_Node'.

    typedef typename AllocTraits::allocator_type NodeAlloc;
        // Alias for the allocator type for an 'RbTree_Node' for the
        // parameterized 'VALUE_TYPE'.
};

                       // ==========================
                       // class RbTree_NodeAllocator
                       // ==========================

template <class VALUE_TYPE, class ALLOCATOR_TYPE>
class RbTree_NodeAllocator : 
   public RbTree_NodeAllocator_Types<VALUE_TYPE, ALLOCATOR_TYPE>::NodeAlloc {
    // For use only by 'bslstl::RbTree' implementation.  This class provides
    // methods for creating and deleting nodes using the appropriate
    // allocator-traits of the parameterized 'ALLOCATOR_TYPE'.  This type is
    // intended to be used as a private base-class for a node-based container,
    // in order to take advantage of the empty-base-class optimization in the
    // case where the base-class has 0 size (as may the case if the
    // paramterized 'ALLOCATOR_TYPE' is not a 'bslma_Allocator').

    // PRIVATE TYPES
    typedef RbTree_NodeAllocator_Types<VALUE_TYPE, ALLOCATOR_TYPE> Types;
    typedef typename Types::AllocTraits AllocTraits;
    typedef typename Types::NodeAlloc   NodeAlloc;

  private:
    // NOT IMPLEMENTED
    RbTree_NodeAllocator& operator=(const RbTree_NodeAllocator&);
    RbTree_NodeAllocator(const RbTree_NodeAllocator&);

  public:
    // CREATORS    
    RbTree_NodeAllocator(const ALLOCATOR_TYPE& allocator);
        // Create a node-allocator that will use the specified 'allocator' to
        // supply memory for allocated node objects.

    // MANIPULATORS
    NodeAlloc& allocator();
        // Return a reference providing modifiable access to the rebound
        // allocator traits for the node-type.  Note that this operation
        // returns a base-class ('NodeAlloc') reference to this object.

    RbTree_Node<VALUE_TYPE> *createDefaultValueNode();
        // Allocate a node object having a default constructed 'VALUE_TYPE'
        // value. 

    RbTree_Node<VALUE_TYPE> *createNode(const VALUE_TYPE& value);
        // Allocate a node object having a copy-constructed 'VALUE_TYPE' value
        // of the specified 'value'.
 
    void releaseNode(RbTree_Node<VALUE_TYPE> *node);
        // Destroy the 'VALUE_TYPE' value of the specified 'node' and
        // release the memory footprint of 'node'.

    // ACCESSORS
    const NodeAlloc& allocator() const;
        // Return a reference providing non-modifiable access to the rebound
        // allocator traits for the node-type.  Note that this operation
        // returns a base-class ('NodeAlloc') reference to this object.

};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CREATORS    
template <class VALUE_TYPE, class ALLOCATOR_TYPE>
inline
RbTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE>::RbTree_NodeAllocator(
                                               const ALLOCATOR_TYPE& allocator)
: NodeAlloc(allocator)
{
}

// MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR_TYPE>
inline
typename RbTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE>::NodeAlloc& 
RbTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE>::allocator()
{
    return *this;
}

template <class VALUE_TYPE, class ALLOCATOR_TYPE>
inline
RbTree_Node<VALUE_TYPE> *
RbTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE>::createDefaultValueNode()
{
    RbTree_Node<VALUE_TYPE> *node = AllocTraits::allocate(allocator(), 1);

    AllocTraits::construct(allocator(), bsls_Util::addressOf(node->d_value));

    return node;
}

template <class VALUE_TYPE, class ALLOCATOR_TYPE>
inline
RbTree_Node<VALUE_TYPE> *
RbTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE>::createNode(
                                                       const VALUE_TYPE& value)
{
    RbTree_Node<VALUE_TYPE> *node = AllocTraits::allocate(allocator(), 1);
 
    AllocTraits::construct(allocator(), 
                           bsls_Util::addressOf(node->d_value),
                           value);
    return node;
}

template <class VALUE_TYPE, class ALLOCATOR_TYPE>
inline
void RbTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE>::releaseNode(
                                                 RbTree_Node<VALUE_TYPE> *node)
{
    AllocTraits::destroy(allocator(), bsls_Util::addressOf(node->d_value));
    AllocTraits::deallocate(allocator(), node, 1);
}

// ACCESSORS
template <class VALUE_TYPE, class ALLOCATOR_TYPE>
inline
const typename RbTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE>::NodeAlloc& 
RbTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE>::allocator() const 
{
    return *this; 
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
