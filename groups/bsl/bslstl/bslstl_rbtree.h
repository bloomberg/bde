// bslstl_rbtree.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLSTL_RBTREE
#define INCLUDED_BSLSTL_RBTREE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant map class.
//
//@CLASSES:
//   bslstl::RbTree: STL-compatible map template
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

#ifndef INCLUDED_BSLSTL_RBTREE_NODECOMPARATOR
#include <bslstl_rbtree_nodecomparator.h>
#endif

#ifndef INCLUDED_BSLSTL_RBTREE_NODEALLOCATOR
#include <bslstl_rbtree_nodeallocator.h>
#endif

#ifndef INCLUDED_BSLSTL_RBTREE_ITERATOR
#include <bslstl_rbtree_iterator.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#warning TBD: FOR TESTING REMOVE
#include <iostream>

namespace BloombergLP {
namespace bslstl {

template <class VALUE_TYPE, class ALLOCATOR_TYPE> class RbTree_Cloner;
template <class VALUE_TYPE, class ALLOCATOR_TYPE> class RbTree_Disposer;


                       // ============
                       // class RbTree
                       // ============

template <class VALUE_TYPE,
          class COMPARATOR_TYPE = std::less<VALUE_TYPE>,
          class ALLOCATOR_TYPE  = bsl::allocator<VALUE_TYPE>
         >
class RbTree : private RbTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE>,
               private RbTree_NodeComparator<VALUE_TYPE, COMPARATOR_TYPE> {
    // This class provides a (value-semantic) red-black tree
    // implementation based on the boost-intrusive library.
    //
    // This type extents 'RbTree_Base' adding methods that require memory
    // allocation, including the standard set of value-semantic operations
    // (copy-construction, assignment, equality comparison).

    // PRIVATE TYPES
    typedef RbTree_Node<VALUE_TYPE>                            Node;
    typedef RbTree_NodeComparator<VALUE_TYPE, COMPARATOR_TYPE> NodeComparator;
    typedef RbTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE>   NodeAllocator;
    typedef RbTree_Cloner<VALUE_TYPE, ALLOCATOR_TYPE>          Cloner;
    typedef RbTree_Disposer<VALUE_TYPE, ALLOCATOR_TYPE>        Disposer;
  public:

    // PUBLIC TYPES
    typedef ALLOCATOR_TYPE                                 allocator_type;
    typedef typename bsl::allocator_traits<allocator_type> AllocatorTraits;
    typedef typename AllocatorTraits::size_type            size_type;
    typedef typename AllocatorTraits::difference_type      difference_type;

    typedef RbTree_Iterator<VALUE_TYPE, Node, difference_type> iterator;
    typedef RbTree_Iterator<const VALUE_TYPE, Node, difference_type> 
                                                                const_iterator;
  private:

    // DATA
    bslalg::RbTreeNode *d_root_p;
    bslalg::RbTreeNode *d_first_p; 
    bslalg::RbTreeNode *d_last_p;
    size_type           d_size;

  private:

    // PRIVATE MANIPULATORS
    NodeAllocator& nodeAllocator(); 
        // Return a reference providing modifiable access to the
        // node-allocator for this tree.  Note that this class inherits from
        // (rather than contains) a node-allocator to take advantage of the
        // empty-base optimization, and this operation returns a base-class
        // reference to this object.

    NodeComparator& nodeComparator();
        // Return a reference providing modifiable access to the
        // node-comparator for this tree.  Note that this class inherits from
        // (rather than contains) a node-comparator to take advantage of the
        // empty-base optimization, and this operation returns a base-class
        // reference to this object.

    // PRIVATE ACCESSORS
    const NodeAllocator& nodeAllocator() const;
        // Return a reference providing non-modifiable access to the
        // node-allocator for this tree.  Note that this class inherits from
        // (rather than contains) a node-allocator to take advantage of the
        // empty-base optimization, and this operation returns a base-class
        // reference to this object.

    const NodeComparator& nodeComparator() const;
        // Return a reference providing non-modifiable access to the
        // node-comparator for this tree.  Note that this class inherits from
        // (rather than contains) a node-comparator to take advantage of the
        // empty-base optimization, and this operation returns a base-class
        // reference to this object.

       
  public:
    // CREATORS
    explicit RbTree(const COMPARATOR_TYPE& comparator = COMPARATOR_TYPE(),
                    const ALLOCATOR_TYPE&  allocator  = ALLOCATOR_TYPE());
        // Create an empty red-black 'RbTree' object.  Optionally specify a
        // 'comparator' that provides a strict weak ordering on values of the
        // parameterized 'VALUE_TYPE'.  If 'comparator' is not supplied, a
        // default constructed 'COMPARATOR_TYPE' object is used.  Optionally
        // specify a 'allocator' used to supply memory'.  If 'allocator' is
        // not specified, a default constructed 'ALLOCATOR_TYPE' object is
        // used. 

    explicit RbTree(const ALLOCATOR_TYPE&  allocator  = ALLOCATOR_TYPE());
        // Create an empty red-black 'RbTree' object, using a
        // default-constructed 'COMPARATOR_TYPE' object to provide a strict
        // weak ordering on values of the parameterized 'VALUE_TYPE'.
        // Optionally specify a 'allocator' used to supply memory'.  If
        // 'allocator' is not specified, a default constructed
        // 'ALLOCATOR_TYPE' object is used.  

    RbTree(const RbTree&         original,
           const ALLOCATOR_TYPE& allocator = ALLOCATOR_TYPE());
        // Create a 'RbTree' object having the same value as the specified
        // 'original' object.  Optionally specify a 'allocator' used to supply
        // memory'.  If 'allocator' is not specified, a default constructed
        // 'ALLOCATOR_TYPE' object is used.  
               
    ~RbTree();
        // Destroy this object.

    // MANIPULATORS
    RbTree& operator=(const RbTree& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    iterator begin();
        // Return an iterator providing modifiable access to the first
        // element in the ordered sequence of elements maintained by this
        // object.  Note that if 'begin() == end()' then this object does not
        // contain any elements.

    iterator end();
        // Return an iterator providing modifiable access to the one-past
        // the last element in the ordered sequence of elements that is
        // associated with this object.  Note that if 'begin() == end()' then
        // this object does not contain any elements.


    iterator lowerBound(const VALUE_TYPE& value);
    iterator upperBound(const VALUE_TYPE& value);
    iterator find(const VALUE_TYPE& value);

    void clear();
        // Remove all the elements from this 'RbTree' object, leaving the list
        // empty.

    iterator erase(const_iterator position);        
    size_type erase(const VALUE_TYPE& value);    
    iterator insert(const VALUE_TYPE& value);
    std::pair<iterator, bool> insertUnique(const VALUE_TYPE& value);

    void swap(RbTree& other);
        // Exchange the value of this object with the value of the specified
        // 'other' object.  If this object and 'other' were created using the
        // same 'allocator', then this method is both efficient and provides
        // the no-throw exception-safety guarantee.  If this object and
        // 'other' were not created with same allocator, then this operation
        // requires a copy of both this object and the 'other' object, and
        // provides a strong exception guarantee. 

    // ACCESSORS
    const_iterator begin() const;
        // Return an iterator providing non-modifiable access to the first
        // element in the ordered sequence of elements maintained by this
        // object.  Note that if 'begin() == end()' then this object does not
        // contain any elements.

    const_iterator end() const;
        // Return an iterator providing non-modifiable access to the one-past
        // the last element in the ordered sequence of elements that is
        // associated with this object.  Note that if 'begin() == end()' then
        // this object does not contain any elements.

    const_iterator lowerBound(const VALUE_TYPE& value) const;
    const_iterator upperBound(const VALUE_TYPE& value) const;
    const_iterator find(const VALUE_TYPE& value) const;

    size_type size() const;
        // Return the number of elements maintained by this object.

    allocator_type allocator() const;
        // Return the allocator used by this object to supply memory.
};

// FREE OPERATORS
template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
bool operator==(const RbTree<VALUE_TYPE,COMPARATOR_TYPE,ALLOCATOR_TYPE>& lhs,
                const RbTree<VALUE_TYPE,COMPARATOR_TYPE,ALLOCATOR_TYPE>& rhs);

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
bool operator!=(const RbTree<VALUE_TYPE,COMPARATOR_TYPE,ALLOCATOR_TYPE>& lhs,
                const RbTree<VALUE_TYPE,COMPARATOR_TYPE,ALLOCATOR_TYPE>& rhs);

  
                        // ===================
                        // class RbTree_Cloner
                        // ===================

template <class VALUE_TYPE, class ALLOCATOR_TYPE>
class RbTree_Cloner {
    // This class defines a functor for cloning node object values, and is
    // compatible with 'bslalg::RbTreePrimitives' operations taking a
    // 'NODE_COPY_FUNCTION' parameter.  Note that this class adapts a
    // 'RbTree_NodeAllocator::createNode' to a functor.

    // PRIVATE TYPES
    typedef RbTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE> NodeAllocator;

    // DATA
    NodeAllocator& d_allocator;  // allocator for node objects

  public:

    // PUBLIC TYPES
    typedef RbTree_Node<VALUE_TYPE> NodeType;    
        // An alias (required by 'RbTreePrimitives') for the node-type of on
        // which comparisons are performed. 

    // CREATOR
    RbTree_Cloner(NodeAllocator& allocator) : d_allocator(allocator) {}

    RbTree_Node<VALUE_TYPE> *operator()(const RbTree_Node<VALUE_TYPE>& node) 
                                                                          const
    {
        return d_allocator.createNode(node.d_value);
    }
};
  
                        // =====================
                        // class RbTree_Disposer
                        // =====================

template <class VALUE_TYPE, class ALLOCATOR_TYPE>
class RbTree_Disposer {
    // This class defines a functor for disposing of node object values, and is
    // compatible with 'bslalg::RbTreePrimitives' operations taking a
    // 'NODE_DELTE_FUNCTION' parameter.  Note that this class adapts a
    // 'RbTree_NodeAllocator::releaseNode' to a functor.

    // PRIVATE TYPES
    typedef RbTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE> NodeAllocator;

    // DATA
     NodeAllocator& d_allocator;  // allocator for node objects

  public:            
    // PUBLIC TYPES
    typedef RbTree_Node<VALUE_TYPE> NodeType;    
        // An alias (required by 'RbTreePrimitives') for the node-type of on
        // which comparisons are performed. 

    RbTree_Disposer(NodeAllocator& allocator) : d_allocator(allocator) { }

    void operator()(RbTree_Node<VALUE_TYPE> *node) const {
        d_allocator.releaseNode(node);
    }
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================


                        // ------------
                        // class RbTree
                        // ------------

// PRIVATE MANIPULATORS
template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::NodeAllocator&
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::nodeAllocator() 
{ 
    return *this; 
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::NodeComparator&
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::nodeComparator() 
{
    return *this; 
}

// PRIVATE ACCESOSRS
template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
const typename 
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::NodeAllocator&
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::nodeAllocator() const
{ 
    return *this; 
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
const typename 
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::NodeComparator&
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::nodeComparator() const
{
    return *this; 
}

// CREATORS
template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::
RbTree(const COMPARATOR_TYPE& comparator, const ALLOCATOR_TYPE& allocator)
: NodeAllocator(allocator)
, NodeComparator(comparator)
, d_root_p(0)
, d_first_p(0)
, d_last_p(0)
, d_size(0)
{
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::
RbTree(const ALLOCATOR_TYPE& allocator)
: NodeAllocator(allocator)
, NodeComparator()
, d_root_p(0)
, d_first_p(0)
, d_last_p(0)
, d_size(0)
{
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::
RbTree(const RbTree& original, const ALLOCATOR_TYPE& allocator)
: NodeAllocator(allocator)
, NodeComparator(original.nodeComparator())
, d_root_p(0)
, d_first_p(0)
, d_last_p(0)
, d_size(0)
{
    if (original.d_root_p) {
        d_root_p  = bslalg::RbTreePrimitives::copyTree(
                                                    original.d_root_p,
                                                    Cloner(nodeAllocator()),
                                                    Disposer(nodeAllocator()));
        d_first_p = bslalg::RbTreePrimitives::minimum(d_root_p);
        d_last_p  = bslalg::RbTreePrimitives::maximum(d_root_p);
        d_size    = original.d_size;
    }
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::~RbTree()
{
    clear();
}

// MANIPULATORS
template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>&
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::operator=(
                const RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>& rhs)
{
    RbTree otherCopy(rhs, allocator());
    this->swap(otherCopy);
    return *this;
}


template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
void RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::clear()
{
    if (0 != d_root_p) {
        BSLS_ASSERT_SAFE(0 < d_size);
        BSLS_ASSERT_SAFE(0 != d_first_p);
        BSLS_ASSERT_SAFE(0 != d_last_p);

        bslalg::RbTreePrimitives::deleteTree(d_root_p, 
                                             Disposer(nodeAllocator()));
        d_root_p  = 0;
        d_first_p = 0;
        d_last_p  = 0;
        d_size    = 0;
    }
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::iterator
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::erase(
                                                      const_iterator position)
{    
    Node *node = const_cast<Node *>(position.node());
    bslalg::RbTreeNode *result = bslalg::RbTreePrimitives::next(node);

    if (d_first_p == node) {
        d_first_p = result;
    }
    if (d_last_p == node) {
        d_last_p = bslalg::RbTreePrimitives::previous(node);
    }
    d_root_p = bslalg::RbTreePrimitives::remove(d_root_p, node);

    nodeAllocator().releaseNode(node);
    --d_size;

    return iterator(result);
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::iterator
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::insert(
                                                      const VALUE_TYPE& value)
{
    RbTree_Node<VALUE_TYPE> *node = nodeAllocator().createNode(value);
    bool leftChild; 
    bslalg::RbTreeNode *insertLocation =
                 bslalg::RbTreePrimitives::findInsertLocation(
                                                        &leftChild,
                                                        d_root_p,
                                                        this->nodeComparator(),
                                                        value);
    d_root_p = bslalg::RbTreePrimitives::insertAt(d_root_p,
                                                  insertLocation,
                                                  leftChild,
                                                  node);
    if (!insertLocation || (leftChild && insertLocation == d_first_p)) {
        d_first_p = node;
    }
    if (!insertLocation || (!leftChild && insertLocation == d_last_p)) {
        d_last_p = node;
    }
    ++d_size;
    return iterator(node);
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
std::pair<
    typename RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::iterator,
    bool>
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::insertUnique(
                                                      const VALUE_TYPE& value)
{
    bool leftChild, isUnique;
    bslalg::RbTreeNode *insertLocation =
        bslalg::RbTreePrimitives::findUniqueInsertLocation(
                                                        &isUnique,
                                                        &leftChild,
                                                        d_root_p,
                                                        this->nodeComparator(),
                                                        value);
    if (!isUnique) {
        return std::pair<iterator, bool>(iterator(insertLocation), false); 
                                                                      // RETURN
    }
    RbTree_Node<VALUE_TYPE> *node = nodeAllocator().createNode(value);
    d_root_p = bslalg::RbTreePrimitives::insertAt(d_root_p,
                                                  insertLocation,
                                                  leftChild,
                                                  node);
    if (!insertLocation || (leftChild && insertLocation == d_first_p)) {
        d_first_p = node;
    }
    if (!insertLocation || (!leftChild && insertLocation == d_last_p)) {
        d_last_p = node;
    }
    ++d_size;
    return std::pair<iterator, bool>(iterator(node), true);
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
void RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::swap(
                    RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>& other)
{    
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
            allocator().mechanism() == other.allocator().mechanism())) {
        bslalg_SwapUtil::swap(&d_root_p, &other.d_root_p);
        bslalg_SwapUtil::swap(&d_first_p, &other.d_first_p);
        bslalg_SwapUtil::swap(&d_last_p, &other.d_last_p);
        bslalg_SwapUtil::swap(&d_size, &other.d_size);
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        RbTree thisCopy(*this, other.allocator());
        RbTree otherCopy(other, allocator());

        this->swap(otherCopy);
        other.swap(thisCopy);
    }
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename 
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::iterator
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::begin() 
{
    return iterator(d_first_p);
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename 
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::iterator
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::end()    
{
    return iterator(0);
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::iterator
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::find(
                                                       const VALUE_TYPE& value)
{
    return iterator(bslalg::RbTreePrimitives::find(d_root_p, 
                                                   this->nodeComparator(), 
                                                   value));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::iterator
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::lowerBound(
                                                       const VALUE_TYPE& value)
{
    return iterator(bslalg::RbTreePrimitives::lowerBound(
                                                        d_root_p, 
                                                        this->nodeComparator(),
                                                        value));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::iterator
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::upperBound(
                                                       const VALUE_TYPE& value)
{
    return iterator(bslalg::RbTreePrimitives::upperBound(
                                                        d_root_p, 
                                                        this->nodeComparator(),
                                                        value));
}

// ACCESSORS
template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename 
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::const_iterator
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::begin() const
{
    return const_iterator(d_first_p);
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename 
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::const_iterator
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::end() const
{
    return const_iterator(0);
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::const_iterator
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::find(
                                                 const VALUE_TYPE& value) const
{
    return const_iterator(bslalg::RbTreePrimitives::find(d_root_p, 
                                                   this->nodeComparator(), 
                                                   value));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::const_iterator
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::lowerBound(
                                                 const VALUE_TYPE& value) const
{
    return const_iterator(bslalg::RbTreePrimitives::lowerBound(
                                                        d_root_p, 
                                                        this->nodeComparator(),
                                                        value));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::const_iterator
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::upperBound(
                                                 const VALUE_TYPE& value) const
{
    return const_iterator(bslalg::RbTreePrimitives::upperBound(
                                                        d_root_p, 
                                                        this->nodeComparator(),
                                                        value));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::size_type
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::size() const   
{
    return d_size;
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::allocator_type
RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::allocator() const
{    
    return nodeAllocator().allocator();
}

// FREE OPERATORS
template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
bool operator==(const RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>& lhs,
                const RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>& rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(),
                                                  lhs.end(),
                                                  rhs.begin());
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
bool operator!=(const RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>& lhs,
                const RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>& rhs)
{
    return !(lhs == rhs);
}

#warning TBD:for dev testing. remove
        template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE, class OSTREAM>
inline
OSTREAM& operator<<(
            OSTREAM&                                                   stream, 
            const RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>& tree)
{
    stream << "[ ";
    typename RbTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::const_iterator it = 
                                                                  tree.begin();

    for (; it != tree.end(); ++it) {
        stream << *it << " ";
    }
    stream << "]";
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
