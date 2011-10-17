// bslstl_rbtree_iterator.h                                           -*-C++-*-
#ifndef INCLUDED_BSLSTL_RBTREE_ITERATOR
#define INCLUDED_BSLSTL_RBTREE_ITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: 
//
//@CLASSES:
//   bslstl::RbTree_Iterator: 
//
//@SEE_ALSO:
//
//@AUTHOR: 
//
//@DESCRIPTION: 
//
///gUsage
///-----

#ifndef INCLUDED_BSLSTL_RBTREE_NODE
#include <bslstl_rbtree_node.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>
#endif

#ifndef INCLUDED_BSLALG_RBTREENODE
#include <bslalg_rbtreenode.h
#endif

#ifndef INCLUDED_BSLALG_RBTREEPRIMITIVES
#include <bslalg_rbtreeprimitives.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

namespace BloombergLP {
namespace bslstl {

                     // =====================
                     // class RbTree_Iterator
                     // =====================

template <class VALUE_TYPE, class NODE_TYPE, class DIFF_TYPE>
class RbTree_Iterator {
    // For use only by 'bslstl::RbTree' implementation.  This class provides
    // an STL-conforming bidirectional iterator [24.2] over the ordered
    // elements in a red-black tree.  An 'RbTree_Iterator' provides access
    // to values of the parameterized 'VALUE_TYPE', over a red-black tree
    // composed of nodes of the parameterized 'NODE_TYPE'.  The paramterized
    // 'DIFF_TYPE' determines the 'difference_type' of the iterator (without
    // requiring access to the allocator-traits for the node) -- it is
    // required by the standard, but provides little practical value.   The
    // behavior of the 'operator*' method is undefined unless the iterator is
    // at a valid position in the tree (i.e., not the "end") and the
    // referenced element has not been removed since the iterator was
    // constructed.  
        
    // PRIVATE TYPES   
    typedef typename BloombergLP::bslmf_RemoveCvq<VALUE_TYPE>::Type NcType;
    typedef RbTree_Iterator<NcType, NODE_TYPE, DIFF_TYPE>           NcIter;

    // DATA
    bslalg::RbTreeNode *d_node_p;  // current position

  private:
    // FRIENDS
    template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
    friend bool operator==(const RbTree_Iterator<VALUE1, NODEPTR, DIFF>& lhs,
                           const RbTree_Iterator<VALUE2, NODEPTR, DIFF>& rhs);
    template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
    friend bool operator!=(const RbTree_Iterator<VALUE1, NODEPTR, DIFF>& lhs,
                           const RbTree_Iterator<VALUE2, NODEPTR, DIFF>& rhs);
    
    template <class VT, class NT, class DT>
    friend class RbTree_Iterator;

  public:
    // PUBLIC TYPES
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef NcType                          value_type;
    typedef DIFF_TYPE                       difference_type;
    typedef VALUE_TYPE*                     pointer;
    typedef VALUE_TYPE&                     reference;
        // Standard iterator defined types [24.4.2].

    // CREATORS       
    RbTree_Iterator();
        // Create an uninitialized iterator.

    RbTree_Iterator(const bslalg::RbTreeNode *node);
        // Create an iterator at the specified 'position'.  The behavior is
        // undefined unless 'node' is of the parameterized 'NODE_TYPE',
        // which is an instance of the type 'bslalg_RbTreeNode

    RbTree_Iterator(const NcIter& original);
        // Create an iterator at the same position as the specified 'original'
        // iterator.  Note that this constructor enables converting from
        // modifiable to const iterator types.

    // RbTree_Iterator(const RbTree_Iterator& original ) = default;
        // Create an iterator having the same value as the specified
        // 'original'.  Note that this operation is either defined by the
        // constructor taking 'NcIter' (if 'NcType' is the same as
        // 'VALUE_TYPE'), or generated automatically by the compiler.  Also
        // note that this construct cannot be defined explicitly (without
        // using 'bsls_enableif') to avoid a duplicate declaration when
        // 'NcType' is the same as 'VALUE_TYPE'.

    // ~RbTree_Iterator() = default;
        // Destroy this object.

    // RbTree_Iterator& operator=(const RbTree_Iterator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // a return a reference providing modifiable access to this object.

    // MANIPULATORS
    RbTree_Iterator operator++();
        // Move this iterator to the next element in the red-black tree and
        // return the value of the iterator prior to this method call.  The
        // behavior is undefined unless the iterator is at a valid position in
        // the red-black tree. 

    RbTree_Iterator operator--();
        // Move this iterator to the previous element in the red-black tree and
        // return the value of the iterator prior to this method call.  The
        // behavior is undefined unless the iterator is at a valid position in
        // the red-black tree. 

    RbTree_Iterator operator++(int);
        // Move this iterator to the next element in the red-black tree 
        // and return the value of this iterator.  The behavior is undefined
        // unless the iterator is at a valid position in the red-black tree.

    RbTree_Iterator operator--(int);
        // Move this iterator to the previous element in the red-black tree 
        // and return the value of this iterator.  The behavior is undefined
        // unless the iterator is at a valid position in the red-black tree.


    // ACCESSORS
    reference operator*() const;
        // Return a reference to the value (of the parameterized 'VALUE_TYPE')
        // of the element at which this iterator is positioned.  The behavior
        // is undefined unless this iterator is at a valid position in the
        // red-black tree.

    pointer operator->() const;      
        // Return the address of the value (of the parameterized 'VALUE_TYPE')
        // of the element at which this iterator is positioned.  The behavior
        // is undefined unless this iterator is at a valid position in the
        // red-black tree.

    const NODE_TYPE *node() const;
        // Return the address of the non-modifiable tree node at which this
        // iterator is positioned, or 0 if this iterator is not at a valid
        // position in the tree.  Note that this operations is used by
        // 'RbTree' to access the node of the iterator.
};

template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
bool operator==(const RbTree_Iterator<VALUE1, NODEPTR, DIFF>& lhs,
                const RbTree_Iterator<VALUE2, NODEPTR, DIFF>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // have the same value and 'false' otherwise.  Two iterators have
    // the same value if they refer to the same position in the same red-black
    // tree, or if both iterators are at an invalid position in the tree
    // (i.e., the "end" of the tree, or the default constructed value).

template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
bool operator!=(const RbTree_Iterator<VALUE1, NODEPTR, DIFF>& lhs,
                const RbTree_Iterator<VALUE2, NODEPTR, DIFF>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // do not have the same value and 'false' otherwise.  Two iterators do not
    // have the same value if they differ in either the red-black tree to
    // which they refer or the position in that tree.

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================


                     // ---------------------
                     // class RbTree_Iterator
                     // ---------------------

// CREATORS
template <class VALUE_TYPE, class NODE_TYPE, class DIFF_TYPE>
inline
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>::RbTree_Iterator() 
: d_node_p(0) 
{
}

template <class VALUE_TYPE, class NODE_TYPE, class DIFF_TYPE>
inline
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>::
RbTree_Iterator(const bslalg::RbTreeNode *node) 
: d_node_p(const_cast<bslalg::RbTreeNode *>(node))
{
}

template <class VALUE_TYPE, class NODE_TYPE, class DIFF_TYPE>
inline
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>::
RbTree_Iterator(const NcIter& original)
: d_node_p(original.d_node_p)
{
}

// MANIPULATORS
template <class VALUE_TYPE, class NODE_TYPE, class DIFF_TYPE>
inline
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>::operator++()
{ 
    d_node_p = bslalg::RbTreePrimitives::next(d_node_p);
    return *this;
}

template <class VALUE_TYPE, class NODE_TYPE, class DIFF_TYPE>
inline
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>::operator--()
{ 
    d_node_p = bslalg::RbTreePrimitives::previous(d_node_p);
    return *this;
}

template <class VALUE_TYPE, class NODE_TYPE, class DIFF_TYPE>
inline
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>::operator++(int)
{
    RbTree_Iterator temp = *this;
    d_node_p = bslalg::RbTreePrimitives::next(d_node_p);
    return temp;
}

template <class VALUE_TYPE, class NODE_TYPE, class DIFF_TYPE>
inline
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>::operator--(int)
{
    RbTree_Iterator temp = *this;
    d_node_p = bslalg::RbTreePrimitives::previous(d_node_p);
    return temp;
}

// ACCESSORS
template <class VALUE_TYPE, class NODE_TYPE, class DIFF_TYPE>
inline
typename RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>::reference
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>::operator*() const
{ 
    return static_cast<NODE_TYPE *>(d_node_p)->d_value;
}

template <class VALUE_TYPE, class NODE_TYPE, class DIFF_TYPE>
inline
typename RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>::pointer 
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>::operator->() const  
{
    return bsls_Util::addressOf(static_cast<NODE_TYPE *>(d_node_p)->d_value);
}

template <class VALUE_TYPE, class NODE_TYPE, class DIFF_TYPE>
inline
const NODE_TYPE *
RbTree_Iterator<VALUE_TYPE, NODE_TYPE, DIFF_TYPE>::node() const  
{
    return static_cast<const NODE_TYPE *>(d_node_p);
}

// FREE OPERATORS
template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
inline
bool operator==(const RbTree_Iterator<VALUE1, NODEPTR, DIFF>& lhs,
                const RbTree_Iterator<VALUE2, NODEPTR, DIFF>& rhs)
{
    return lhs.d_node_p == rhs.d_node_p;
}

template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
inline
bool operator!=(const RbTree_Iterator<VALUE1, NODEPTR, DIFF>& lhs,
                const RbTree_Iterator<VALUE2, NODEPTR, DIFF>& rhs)
{
    return lhs.d_node_p != rhs.d_node_p;
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
