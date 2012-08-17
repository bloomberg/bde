// bslstl_hashtablenodefactory.h                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_HASHTABLENODEFACTORY
#define INCLUDED_BSLSTL_HASHTABLENODEFACTORY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for hash functions
//
//@CLASSES:
//   bslimp::HashTableNodeFactory : factory for unorded container nodes
//
//@SEE_ALSO: bsl+stdhdrs
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides an implementation of the container,
// 'unordered_map', specified by the C++11 standard.
//
// This implementation will be the 'monkey-skin' approach that minimizes the
// size of the nodes, by using the preceding element in the list when storing
// references to nodes, e.g., in iterators or buckets.  The main container is
// a singly-linked list, indexed by a vector of buckets.  The overhead is:
//   o one pointer in each node
//   o one pointer per bucket (no. buckets determined by max_load_factor)
//   o one additional (empty) sentinel node
// As we do not cache the hashed value, if any hash function throws we will
// either do nothing and allow the exception to propogate, or, if some change
// of state has already been made, clear the whole container to provide the
// basic exception guarantee.  There are similar concerns for the 'equal_to'
// predicate.
//
//-----------------------------------------------------------------------------
//..
//
///Usage
///-----

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_hashtablenodefactory.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#include <bslstl_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif


namespace BloombergLP
{
namespace bslstl
{

                          // ==========================
                          // class HashTableNodeFactory
                          // ==========================

template <class ALLOCATOR>
class HashTableNodeFactory {

    // PRIVATE TYPES
    typedef ALLOCATOR NodeAllocator;
    typedef ::bsl::allocator_traits<NodeAllocator> AllocatorTraits;
    typedef typename AllocatorTraits::value_type NodeType;
    //typedef typename NodeType::ValueType ValueType;

  private:
    // DATA
    NodeAllocator d_allocator;  // rebound node allocator

    // NOT IMPLEMENTED
    HashTableNodeFactory(const HashTableNodeFactory&);
    HashTableNodeFactory& operator=(const HashTableNodeFactory&);

  public:
    // CREATORS
    HashTableNodeFactory(const ALLOCATOR& allocator);
        // Create a tree node-factory that will use the specified
        // 'allocator' to supply memory.


    // MANIPULATORS
    template <class SOURCE_TYPE>
    void constructValueInNode(NodeType *node, const SOURCE_TYPE& value);
        // Create an object of the parametized 'ValueType' in the specified
        // 'node' having the specified 'value'.
        // 'SOURCE_TYPE' is not 'ValueType' to avoid conversion in maps from
        // 'pair<K,V>' to 'pair<const K, V>', which would create a temporary
        // potentially using the default allocator for the pair elements, rather
        // than the allocator provided by the factory.
        // Note that we need to be sure to check 'bslma' traits to propagate
        // the factory allocator if the 'ValueType' stored in the node when
        // appropriate.

    NodeType *createEmptyNode();
        // Create a new 'HashTableNode' object that does not link to any other
        // node, and having an un-initialized 'value'.

    NodeType *cloneNode(const bslalg::BidirectionalLink *original);
        // Create a new 'HashTableNode' object that does not link to any other
        // node, holding a value that is the same as the value in the specified
        // 'original' node.  Behavior is undefined unless 'original' holds the
        // address of a 'NodeType' object.

    void disposeNode(NodeType *node);
        // Destroy and deallocate the specified 'node'.  The behavior is
        // undefined unless 'node' is the address of a
        // 'TreeNode<VALUE>' object.

    void reclaimNode(NodeType *node);
        // Blah...

    NodeAllocator allocator() const;
        // Return the allocator held by this object.
};


// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        //--------------------------------
                        // class HashTableNodeFactory
                        //--------------------------------

// CREATORS
template <class ALLOCATOR_TYPE>
inline
HashTableNodeFactory<ALLOCATOR_TYPE>
::HashTableNodeFactory(const ALLOCATOR_TYPE& allocator)
: d_allocator(allocator)
{
}


// MANIPULATORS
template <class ALLOCATOR_TYPE>
inline
typename HashTableNodeFactory<ALLOCATOR_TYPE>::NodeAllocator
HashTableNodeFactory<ALLOCATOR_TYPE>::allocator() const
{
    return d_allocator;
}

template <class ALLOCATOR_TYPE>
template <class SOURCE_TYPE>
inline
void
HashTableNodeFactory<ALLOCATOR_TYPE>::constructValueInNode(
                                                     NodeType           *node,
                                                     const SOURCE_TYPE&  value)
{
    BSLS_ASSERT_SAFE(node);
    using BloombergLP::bsls_Util;
    AllocatorTraits::construct(d_allocator,
                               bsls::Util::addressOf(node->value()),
                               value);
}

template <class ALLOCATOR_TYPE>
inline
typename HashTableNodeFactory<ALLOCATOR_TYPE>::NodeType *
HashTableNodeFactory<ALLOCATOR_TYPE>::createEmptyNode()
{
    NodeType *result  = AllocatorTraits::allocate(d_allocator, 1);
    AllocatorTraits::template construct<bslalg::BidirectionalLink>(d_allocator,
                                                                   result);
    return result;
}

template <class ALLOCATOR_TYPE>
inline
typename HashTableNodeFactory<ALLOCATOR_TYPE>::NodeType *
HashTableNodeFactory<ALLOCATOR_TYPE>::cloneNode(
                                     const bslalg::BidirectionalLink *original)
{
    BSLS_ASSERT_SAFE(original);

    NodeType *result = createEmptyNode();
    try {
        constructValueInNode(result,
                             static_cast<const NodeType *>(original)->value());
    }
    catch(...) {
        reclaimNode(result);
        throw;
    }
    return result;
}

template <class ALLOCATOR_TYPE>
inline
void
HashTableNodeFactory<ALLOCATOR_TYPE>::disposeNode(NodeType *node)
{
    BSLS_ASSERT_SAFE(node);

    using BloombergLP::bsls_Util;

    AllocatorTraits::destroy(d_allocator,
                             bsls::Util::addressOf(node->value()));
    reclaimNode(node);
}

template <class ALLOCATOR_TYPE>
inline
void
HashTableNodeFactory<ALLOCATOR_TYPE>::reclaimNode(NodeType *node)
{
    BSLS_ASSERT_SAFE(node);

    AllocatorTraits::deallocate(d_allocator, node, 1);
}

}

}

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
