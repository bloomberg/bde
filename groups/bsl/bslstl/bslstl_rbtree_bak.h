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
//   bslstl::RBTree: STL-compatible map template
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
#warning replace with bslstl_allocator when moved to bsl
#include <bsl_memory.h>
//#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#include <iostream>

namespace BloombergLP {
namespace bslstl {

                       // =================
                       // class RBTree_Base
                       // =================

template <class VALUE_TYPE, class COMPARATOR_TYPE>
class RBTree_Base : 
                   private RBTree_NodeComparator<VALUE_TYPE, COMPARATOR_TYPE> {
    // This class provides an partial implementation of a red-black tree,
    // providing the basic manipulators and accessors that do *not* require
    // memory allocation.

  public:
    // PUBLIC TYPES
    typedef RBTree_Node<VALUE_TYPE>                            Node;
    typedef RBTree_NodeComparator<VALUE_TYPE, COMPARATOR_TYPE> NodeComparator;

    // DATA
    Node        *d_root_p;
    Node        *d_first_p;
    Node        *d_last_p;
    bsl::size_t  d_size;

    // NOT IMPLEMENTED
    RBTree_Base(const RBTree_Base&); 
    RBTree_Base& operator=(const RBTree_Base&);

  protected:
    const NodeComparator& comparator() const 
    { return *this; } // return ref to base-class 

  public:
    // PUBLIC TYPES
#warning ptrdiff_t
    typedef RBTree_Iterator<VALUE_TYPE, Node, std::ptrdiff_t> iterator;
    typedef RBTree_Iterator<const VALUE_TYPE, Node, std::ptrdiff_t> 
                                                               const_iterator;

    // CREATORS
    RBTree_Base();
    RBTree_Base(const COMPARATOR_TYPE& comparator);
    ~RBTree_Base();

    // MANIPULATORS
    iterator begin();
    iterator end();
    template <typename SEARCH_TYPE>
    iterator lowerBound(const SEARCH_TYPE& value);

    template <typename SEARCH_TYPE>
    iterator upperBound(const SEARCH_TYPE& value);

    template <typename SEARCH_TYPE>
    iterator find(const SEARCH_TYPE& value);

    // ACCESSORS
    const_iterator begin() const;
    const_iterator end() const;
    template <typename SEARCH_TYPE>
    const_iterator lowerBound(const SEARCH_TYPE& value) const;

    template <typename SEARCH_TYPE>
    const_iterator upperBound(const SEARCH_TYPE& value) const;

    template <typename SEARCH_TYPE>
    const_iterator find(const SEARCH_TYPE& value) const;

    size_type size() const;
};

                       // ============
                       // class RBTree
                       // ============

template <class VALUE_TYPE,
          class COMPARATOR_TYPE = std::less<VALUE_TYPE>,
          class ALLOCATOR_TYPE  = bsl::allocator<VALUE_TYPE>
         >
class RBTree : public  RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>,
               private RBTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE> {
    // This class provides a (value-semantic) red-black tree
    // implementation based on the boost-intrusive library.
    //
    // This type extents 'RBTree_Base' adding methods that require memory
    // allocation, including the standard set of value-semantic operations
    // (copy-construction, assignment, equality comparison).
    
    // PRIVATE TYPES
    typedef RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE> TreeBase;



    typedef RBTree_NodeAllocator<VALUE_TYPE, ALLOCATOR_TYPE> NodeAllocator;
    
    NodeAllocator& nodeAllocator() { return *this; }
    const NodeAllocator& nodeAllocator() const { return *this; }

    // FRIENDS
    template <class VALUE_TYPE2, class COMPARATOR_TYPE2, class ALLOCATOR_TYPE2>
    friend bool operator==(
           const RBTree<VALUE_TYPE2,COMPARATOR_TYPE2,ALLOCATOR_TYPE2>& lhs,
           const RBTree<VALUE_TYPE2,COMPARATOR_TYPE2,ALLOCATOR_TYPE2>& rhs);
    template <class VALUE_TYPE2, class COMPARATOR_TYPE2, class ALLOCATOR_TYPE2>
    friend bool operator!=(
           const RBTree<VALUE_TYPE2,COMPARATOR_TYPE2,ALLOCATOR_TYPE2>& lhs,
           const RBTree<VALUE_TYPE2,COMPARATOR_TYPE2,ALLOCATOR_TYPE2>& rhs);
        
  public:
    typedef typename TreeBase::iterator       iterator;
    typedef typename TreeBase::const_iterator const_iterator;
    typedef ALLOCATOR_TYPE                    allocator_type;

    typedef typename bsl::allocator_traits<ALLOCATOR_TYPE>::size_type 
                                                               size_type;

    struct Cloner {
        // This type serves as a functor for operations that clone nodes
        // (e.g., copy-construction and assignment), it is compatible with
        // boost-instrusive "clone" functions. 

        NodeAllocator& d_allocator;
            
        Cloner(NodeAllocator& allocator) 
        : d_allocator(allocator) {
        }

        RBTree_Node<VALUE_TYPE> *operator()(
            const RBTree_Node<VALUE_TYPE>& node) {
            return d_allocator.createNode(node.d_value);
        }
    };

    struct Disposer {
        // This type serves as a functor for operations that destroy nodes
        // (e.g., 'erase', destruction), it is compatible with
        // boost-instrusive "dispose" functions.

        NodeAllocator& d_allocator;
            
        Disposer(NodeAllocator& allocator) 
        : d_allocator(allocator) {
        }

        void operator()(RBTree_Node<VALUE_TYPE> *node) {
            d_allocator.releaseNode(node);
        }
    };

    // CREATORS
    explicit RBTree(const COMPARATOR_TYPE& comp      = COMPARATOR_TYPE(),
                    const ALLOCATOR_TYPE&  allocator = ALLOCATOR_TYPE());

    explicit RBTree(const ALLOCATOR_TYPE& allocator);

    RBTree(const RBTree&         original,
           const ALLOCATOR_TYPE& allocator = ALLOCATOR_TYPE());
               
    ~RBTree();

    // MANIPULATORS
    RBTree& operator=(const RBTree& rhs);

    void clear();
    iterator erase(const_iterator position);

    template <typename SEARCH_TYPE>
    size_type erase(const SEARCH_TYPE& value);
    
    iterator insertEqual(const VALUE_TYPE& value);
    std::pair<iterator, bool> insertUnique(const VALUE_TYPE& value);
    void swap(RBTree& other);

    // ACCESSORS
    allocator_type allocator() const;
};

// FREE OPERATORS
template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
bool operator==(const RBTree<VALUE_TYPE,COMPARATOR_TYPE,ALLOCATOR_TYPE>& lhs,
                const RBTree<VALUE_TYPE,COMPARATOR_TYPE,ALLOCATOR_TYPE>& rhs);

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
bool operator!=(const RBTree<VALUE_TYPE,COMPARATOR_TYPE,ALLOCATOR_TYPE>& lhs,
                const RBTree<VALUE_TYPE,COMPARATOR_TYPE,ALLOCATOR_TYPE>& rhs);

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // -----------------
                       // class RBTree_Base
                       // -----------------

// CREATORS
template <class VALUE_TYPE, class COMPARATOR_TYPE>
inline
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::RBTree_Base()
: NodeComparator(COMPARATOR_TYPE())
, d_imp()
{        
}

template <class VALUE_TYPE, class COMPARATOR_TYPE>
inline
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::
RBTree_Base(const COMPARATOR_TYPE& comparator)
: NodeComparator(comparator)
, d_imp()
{        
}

template <class VALUE_TYPE, class COMPARATOR_TYPE>
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::~RBTree_Base()
{
}

// MANIPULATORS
template <class VALUE_TYPE, class COMPARATOR_TYPE>
inline
typename
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::iterator
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::begin() 
{
    return iterator(d_imp.begin());
}

template <class VALUE_TYPE, class COMPARATOR_TYPE>
inline
typename
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::iterator
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::end()    
{
    return iterator(d_imp.end());
}

template <class VALUE_TYPE, class COMPARATOR_TYPE>
template <typename SEARCH_TYPE>
inline
typename
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::iterator
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::find(const SEARCH_TYPE& node)
{
    return iterator(d_imp.find(node, comparator()));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE>
template <typename SEARCH_TYPE>
inline
typename
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::iterator
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::lowerBound(const SEARCH_TYPE& node)
{
    return iterator(d_imp.lower_bound(node, comparator()));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE>
template <typename SEARCH_TYPE>
inline
typename
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::iterator
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::upperBound(const SEARCH_TYPE& node)
{
    return iterator(d_imp.upper_bound(node, comparator()));
}

// ACCESSORS
template <class VALUE_TYPE, class COMPARATOR_TYPE>
inline
typename
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::const_iterator
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::begin() const
{
    return const_iterator(d_imp.begin());
}

template <class VALUE_TYPE, class COMPARATOR_TYPE>
inline
typename
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::const_iterator
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::end() const   
{
    return const_iterator(d_imp.end());
}

template <class VALUE_TYPE, class COMPARATOR_TYPE>
template <typename SEARCH_TYPE>
inline
typename
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::const_iterator
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::find(const SEARCH_TYPE& node) const
{
    return const_iterator(d_imp.find(node, comparator()));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE>
template <typename SEARCH_TYPE>
inline
typename
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::const_iterator
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::lowerBound(
                                                 const SEARCH_TYPE& node) const
{
    return const_iterator(d_imp.lower_bound(node, comparator()));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE>
template <typename SEARCH_TYPE>
inline
typename
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::const_iterator
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::upperBound(
                                                 const SEARCH_TYPE& node) const
{
    return const_iterator(d_imp.upper_bound(node, comparator()));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE>
inline
typename RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::size_type
RBTree_Base<VALUE_TYPE, COMPARATOR_TYPE>::size() const   
{
    return d_imp.size();
}


                          // ------------
                          // class RBTree
                          // ------------

// CREATORS
template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::
RBTree(const COMPARATOR_TYPE& comparator, const ALLOCATOR_TYPE& allocator)
: TreeBase(comparator)
, NodeAllocator(allocator)
{
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::
RBTree(const ALLOCATOR_TYPE& allocator)
: TreeBase()
, NodeAllocator(allocator)
{
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::
RBTree(const RBTree& original, const ALLOCATOR_TYPE& allocator)
: TreeBase()
, NodeAllocator(allocator)
{
    TreeBase::imp().clone_from(original.imp(), 
                               Cloner(nodeAllocator()),
                               Disposer(nodeAllocator()));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::
~RBTree()
{
    clear();
}

// MANIPULATORS
template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>&
RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::operator=(
                const RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>& rhs)
{
    RBTree otherCopy(rhs, allocator());
    this->swap(otherCopy);
    return *this;
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
void RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::clear()
{
    TreeBase::imp().clear_and_dispose(Disposer(nodeAllocator()));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::iterator
RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::erase(
                                                      const_iterator position)
{    
    return iterator(TreeBase::imp().erase_and_dispose(
                                                   position.imp(),
                                                   Disposer(nodeAllocator())));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
template <typename SEARCH_TYPE>
inline
typename RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::size_type
RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::erase(
                                                    const SEARCH_TYPE& value)
{    
    return TreeBase::imp().erase_and_dispose(value,
                                             TreeBase::comparator(), 
                                             Disposer(nodeAllocator()));
}


template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::iterator
RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::insertEqual(
                                                      const VALUE_TYPE& value)
{
    RBTree_Node<VALUE_TYPE> *node = nodeAllocator().createNode(value);
    return iterator(TreeBase::imp().insert_equal(*node));
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
std::pair<
    typename RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::iterator,
    bool>
RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::insertUnique(
                                                      const VALUE_TYPE& value)
{
    typename TreeBase::TreeImp::insert_commit_data insertData;

    std::pair<typename TreeBase::TreeImp::iterator, bool> ret = 
                    TreeBase::imp().insert_unique_check(value, 
                                                        TreeBase::comparator(),
                                                        insertData);

    if (ret.second) {
        // Commit the insertion, if the check was successful.
        
        RBTree_Node<VALUE_TYPE> *node = nodeAllocator().createNode(value);
        typename TreeBase::TreeImp::iterator it = 
                       TreeBase::imp().insert_unique_commit(*node, insertData);

        return std::pair<iterator, bool>(iterator(it), true);         // RETURN
    }

    return std::pair<iterator, bool>(iterator(ret.first), false);
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
void
RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::swap(
                    RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>& other)
{    
    using std::swap;

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
            allocator().mechanism() == other.allocator().mechanism())) {
        TreeBase::imp().swap(other.imp());
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        RBTree thisCopy(*this, other.allocator());
        RBTree otherCopy(other, allocator());
        
        TreeBase::imp().swap(otherCopy.imp());
        other.imp().swap(thisCopy.imp());
    }
}


template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
typename RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::allocator_type
RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>::allocator() const
{    
    return nodeAllocator().allocator();
}

// FREE OPERATORS
template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
bool operator==(const RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>& lhs,
                const RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>& rhs)
{
    return lhs.d_imp == rhs.d_imp;
}

template <class VALUE_TYPE, class COMPARATOR_TYPE, class ALLOCATOR_TYPE>
inline
bool operator!=(const RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>& lhs,
                const RBTree<VALUE_TYPE, COMPARATOR_TYPE, ALLOCATOR_TYPE>& rhs)
{
    return lhs.d_imp != rhs.d_imp;
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
