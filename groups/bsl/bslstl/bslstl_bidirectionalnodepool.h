// bslstl_bidirectionalnodepool.h                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_BIDIRECTIONALNODEPOOL
#define INCLUDED_BSLSTL_BIDIRECTIONALNODEPOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide efficient creation of nodes used in node-based container.
//
//@CLASSES:
//   bslstl:: memory manager to allocate hash table nodes
//
//@AUTHOR: Stefano Pacifico (spacifico1)
//
//@SEE_ALSO: bslstl_simplepool
//
//@DESCRIPTION: This component implements a mechanism that creates and deletes
// 'bslalg::BidirectionalListNode' objects for the parameterized 'VALUE' type
// for use in hash-table-based containers.
// A 'bslstl::BidirectionalNodePool' contains a memory pool provided by the
// 'bslstl_simplepool' component to provide memory for the nodes (see
// 'bslstl_simplepool').  When the pool is empty, a number of memory blocks is
// allocated and added to the pool, where each block is large enough to contain
// a 'bslstl::BidirectionalNodePool'  The first allocation contains one
// memory block.  Subsequent allocations double the number of memory blocks of
// the previous allocation up to an implementation defined maximum number of
// blocks.
//
///Usage
///-----
// This section illustrates intended use of this component.


#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#include <bslstl_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLSTL_SIMPLEPOOL
#include <bslstl_simplepool.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINKLISTNODE
#include <bslalg_bidirectionalnode.h>
#endif

#ifndef INCLUDED_BSLMA_DEALLOCATORPROCTOR
#include <bslma_deallocatorproctor.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {
namespace bslstl {

                       // ===========================
                       // class BidirectionalNodePool
                       // ===========================

template <class VALUE, class ALLOCATOR>
class BidirectionalNodePool {
    // This class provides methods for creating and deleting nodes using the
    // appropriate allocator-traits of the parameterized 'ALLOCATOR'.  This
    // type is intended to be used as a private base-class for a node-based
    // container, in order to take advantage of the empty-base-class
    // optimization in the case where the base-class has 0 size (as may the
    // case if the parameterized 'ALLOCATOR' is not a 'bslma::Allocator').

    typedef SimplePool<bslalg::BidirectionalNode<VALUE>, ALLOCATOR>       Pool;
        // Alias for the memory pool allocator.

    typedef typename Pool::AllocatorTraits AllocatorTraits;
        // Alias for the allocator traits defined by 'SimplePool'.

    // DATA
    Pool d_pool;  // pool for allocating memory

  private:
    // NOT IMPLEMENTED
    BidirectionalNodePool& operator=(const BidirectionalNodePool&);
    BidirectionalNodePool(const BidirectionalNodePool&);

  public:
    // PUBLIC TYPE
    typedef typename Pool::AllocatorType AllocatorType;
        // Alias for the allocator type defined by 'SimplePool'.

  public:
    // CREATORS
    explicit BidirectionalNodePool(const ALLOCATOR& allocator);
        // Create a node-allocator that will use the specified 'allocator' to
        // supply memory for allocated node objects.

    // MANIPULATORS
    AllocatorType& allocator();
        // Return a reference providing modifiable access to the rebound
        // allocator traits for the node-type.  Note that this operation
        // returns a base-class ('NodeAlloc') reference to this object.

    bslalg::BidirectionalLink *createNode();
        // Allocate a node object having a default constructed 'VALUE', and
        // return its address.  Note that the 'next' and 'prev' attributes of
        // the returned node will be uninitialized.

    template <class SOURCE>
    bslalg::BidirectionalLink *createNode(const SOURCE& value);
        // Allocate a node object and return its address.  This operation will
        // construct a 'VALUE' into the returned node passing the specified
        // 'value' to the constructor call.  Note that the 'next' and 'prev'
        // attributes of the returned node will be uninitialized.

    template <class FIRST_ARG, class SECOND_ARG>
    bslalg::BidirectionalLink *createNode(const FIRST_ARG&  first,
                                          const SECOND_ARG& second);
        // Allocate a node object and return its address.  This operation will
        // construct a 'VALUE' into the returned node passing the specified
        // 'first' and 'second' arguments to the constructor call.  Note that
        // the 'next' and 'prev' attributes of the returned node will be
        // uninitialized.

    bslalg::BidirectionalLink *cloneNode(
                                    const bslalg::BidirectionalLink& original);
        // Allocate a node object having a copy-constructed 'VALUE' of
        // 'value()' of the specified 'original', and return its address.  The
        // behavior is undefined unless 'original' refers to a
        // 'bslalg::BidirectionalNode<VALUE>'.  Note that the 'next' and 'prev'
        // attributes of the returned node will be uninitialized.
    
    void deleteNode(bslalg::BidirectionalLink *linkNode);
        // Destroy the 'VALUE' value of the specified 'node' and return the
        // memory footprint of 'node' to this pool for potential reuse.  The
        // behavior is undefined unless 'node' refers to a
        // 'bslalg::BidirectionalNode<VALUE>' that was allocated by this pool.

    void reserveNodes(native_std::size_t numNodes);
        // Reserve memory from this pool to satisfy memory requests for at
        // least the specified 'numNodes' before the pool replenishes.  The
        // behavior is undefined unless '0 < numBlocks'.

    void swap(BidirectionalNodePool& other);
        // Efficiently exchange the management of nodes of this object and
        // the specified 'other' object.  The behavior is undefined unless the
        // underlying mechanisms of 'allocator()' unless 
        // 'allocator() == other.allocator()'.

    // ACCESSORS
    const AllocatorType& allocator() const;
        // Return a reference providing non-modifiable access to the rebound
        // allocator for the node-type.  Note that this operation returns a
        // base-class ('NodeAlloc') reference to this object.
};

// FREE FUNCTIONS
template <class VALUE, class ALLOCATOR>
void swap(BidirectionalNodePool<VALUE, ALLOCATOR>& a,
          BidirectionalNodePool<VALUE, ALLOCATOR>& b);
    // Swap... TBD.

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CREATORS
template <class VALUE, class ALLOCATOR>
inline
BidirectionalNodePool<VALUE, ALLOCATOR>::BidirectionalNodePool(
                                                    const ALLOCATOR& allocator)
: d_pool(allocator)
{
}

// MANIPULATORS
template <class VALUE, class ALLOCATOR>
inline
typename SimplePool<bslalg::BidirectionalNode<VALUE>, ALLOCATOR>::
                                                                 AllocatorType&
BidirectionalNodePool<VALUE, ALLOCATOR>::allocator()
{
    return d_pool.allocator();
}

template <class VALUE, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::createNode()
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               BSLS_UTIL_ADDRESSOF(node->value()));

    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
template <class SOURCE>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::createNode(const SOURCE& value)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               BSLS_UTIL_ADDRESSOF(node->value()),
                               value);
    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
template <class FIRST_ARG, class SECOND_ARG>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::createNode(const FIRST_ARG&  first,
                                                    const SECOND_ARG& second)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               BSLS_UTIL_ADDRESSOF(node->value()),
                               first,
                               second);
    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::cloneNode(
                                     const bslalg::BidirectionalLink& original)
{
    return createNode(static_cast<const bslalg::BidirectionalNode<VALUE>&>
                                                           (original).value());
}

template <class VALUE, class ALLOCATOR>
inline
void BidirectionalNodePool<VALUE, ALLOCATOR>::deleteNode(
                                           bslalg::BidirectionalLink *linkNode)
{
    BSLS_ASSERT(linkNode);

    bslalg::BidirectionalNode<VALUE> *node =
                     static_cast<bslalg::BidirectionalNode<VALUE> *>(linkNode);
    AllocatorTraits::destroy(allocator(),
                             BSLS_UTIL_ADDRESSOF(node->value()));
    d_pool.deallocate(node);
}

template <class VALUE, class ALLOCATOR>
inline
void BidirectionalNodePool<VALUE, ALLOCATOR>::reserveNodes(
                                                   native_std::size_t numNodes)
{
    BSLS_ASSERT_SAFE(0 < numNodes);

    d_pool.reserve(numNodes);
}

template <class VALUE, class ALLOCATOR>
inline
void BidirectionalNodePool<VALUE, ALLOCATOR>::swap(
                                BidirectionalNodePool<VALUE, ALLOCATOR>& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    d_pool.swap(other.d_pool);
}

// ACCESSORS
template <class VALUE, class ALLOCATOR>
inline
const typename
              SimplePool<bslalg::BidirectionalNode<VALUE>, ALLOCATOR>::
                                                                 AllocatorType&
BidirectionalNodePool<VALUE, ALLOCATOR>::allocator() const
{
    return d_pool.allocator();
}

}  // close namespace bslstl

template <class VALUE, class ALLOCATOR>
inline
void bslstl::swap(bslstl::BidirectionalNodePool<VALUE, ALLOCATOR>& a,
                  bslstl::BidirectionalNodePool<VALUE, ALLOCATOR>& b)
{
    a.swap(b);
}

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
