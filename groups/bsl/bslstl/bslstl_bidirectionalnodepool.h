// bslstl_bidirectionalnodepool.h                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_BIDIRECTIONALNODEPOOL
#define INCLUDED_BSLSTL_BIDIRECTIONALNODEPOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide efficient creation of nodes used in a node-based container.
//
//@CLASSES:
//   bslstl::BidirectionalNodePool: memory manager to allocate hash table nodes
//
//@SEE_ALSO: bslstl_simplepool
//
//@DESCRIPTION: This component implements a mechanism, 'BidirectionalNodePool',
// that creates and destroys 'bslalg::BidirectionalListNode' objects holding
// objects of a (template parameter) type 'VALUE' for use in hash-table-based
// containers.
//
// A 'BidirectionalNodePool' uses a memory pool provided by the
// 'bslstl_simplepool' component in its implementation to provide memory for
// the nodes (see 'bslstl_simplepool').
//
///Memory Allocation
///-----------------
// 'BidirectionalNodePool' uses an allocator of the (template parameter) type
// 'ALLOCATOR' specified at construction to allocate memory.
// 'BidirectionalNodePool' supports allocators meeting the requirements of the
// C++ standard allocator requirements ([allocator.requirements], C++11
// 17.6.3.5).
//
// If 'ALLOCATOR' is 'bsl::allocator' and the (template parameter) type 'VALUE'
// defines the 'bslma::UsesBslmaAllocator' trait, then the 'bslma::Allocator'
// object specified at construction will be supplied to constructors of the
// (template parameter) type 'VALUE' in the 'cloneNode' method and
// 'emplaceIntoNewNode' method overloads.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Linked List Container
///- - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to define a bidirectional linked list that can hold
// elements of a template parameter type.  'bslstl::BidirectionalNodePool' can
// be used to create and destroy nodes that make up a linked list.
//
// First, we create an elided definition of the class template 'MyList':
//..
// #include <bslalg_bidirectionallinklistutil.h>
//
//  template <class VALUE, class ALLOCATOR>
//  class MyList {
//      // This class template implements a bidirectional linked list of
//      // element of the (template parameter) type 'VALUE'.  The memory used
//      // will be allocated from an allocator of the (template parameter) type
//      // 'ALLOCATOR' specified at construction.
//
//    public:
//      // TYPES
//      typedef bslalg::BidirectionalNode<VALUE> Node;
//          // This 'typedef' is an alias to the type of the linked list node.
//
//    private:
//      // TYPES
//      typedef bslstl::BidirectionalNodePool<VALUE, ALLOCATOR> Pool;
//          // This 'typedef' is an alias to the type of the memory pool.
//
//      typedef bslalg::BidirectionalLinkListUtil               Util;
//          // This 'typedef' is an alias to the utility 'struct' providing
//          // functions for constructing and manipulating linked lists.
//
//      typedef bslalg::BidirectionalLink                       Link;
//          // This 'typedef' is an alis to the type of the linked list link.
//
//      // DATA
//      Node *d_head_p;  // pointer to the head of the linked list
//      Node *d_tail_p;  // pointer to the tail of the linked list
//      Pool  d_pool;    // memory pool used to allocate memory
//
//
//    public:
//      // CREATORS
//      MyList(const ALLOCATOR& allocator = ALLOCATOR());
//          // Create an empty linked list that allocate memory using the
//          // specified 'allocator'.
//
//      ~MyList();
//          // Destroy this linked list by calling destructor for each element
//          // and deallocate all allocated storage.
//
//      // MANIPULATORS
//      void pushFront(const VALUE& value);
//          // Insert the specified 'value' at the front of this linked list.
//
//      void pushBack(const VALUE& value);
//          // Insert the specified 'value' at the end of this linked list.
//
//      //...
//  };
//..
// Now, we define the methods of 'MyMatrix':
//..
// CREATORS
//  template <class VALUE, class ALLOCATOR>
//  MyList<VALUE, ALLOCATOR>::MyList(const ALLOCATOR& allocator)
//  : d_head_p(0)
//  , d_tail_p(0)
//  , d_pool(allocator)
//  {
//  }
//
//  template <class VALUE, class ALLOCATOR>
//  MyList<VALUE, ALLOCATOR>::~MyList()
//  {
//      Link *link = d_head_p;
//      while (link) {
//          Link *next = link->nextLink();
//..
// Here, we call the memory pool's 'deleteNode' method to destroy the 'value'
// attribute of the node and return its memory footprint back to the pool:
//..
//          d_pool.deleteNode(static_cast<Node*>(link));
//          link = next;
//      }
//  }
//
// MANIPULATORS
//  template <class VALUE, class ALLOCATOR>
//  void
//  MyList<VALUE, ALLOCATOR>::pushFront(const VALUE& value)
//  {
//..
// Here, we call the memory pool's 'emplaceIntoNewNode' method to allocate a
// node and copy-construct the specified 'value' at the 'value' attribute of
// the node:
//..
//      Node *node = static_cast<Node *>(d_pool.emplaceIntoNewNode(value));
//..
// Note that the memory pool will allocate the footprint of the node using the
// allocator specified at construction.  If the (template parameter) type
// 'ALLOCATOR' is an instance of 'bsl::allocator' and the (template parameter)
// type 'VALUE' has the 'bslma::UsesBslmaAllocator' trait, then the allocator
// specified at construction will also be supplied to the copy-constructor of
// 'VALUE'.
//..
//      if (!d_head_p) {
//          d_tail_p = node;
//          node->setNextLink(0);
//          node->setPreviousLink(0);
//      }
//      else {
//          Util::insertLinkBeforeTarget(node, d_head_p);
//      }
//      d_head_p = node;
//  }
//
//  template <class VALUE, class ALLOCATOR>
//  void
//  MyList<VALUE, ALLOCATOR>::pushBack(const VALUE& value)
//  {
//..
// Here, just like how we implemented the 'pushFront' method, we call the
// pool's 'emplaceIntoNewNode' method to allocate a node and copy-construct the
// specified 'value' at the 'value' attribute of the node:
//..
//      Node *node = static_cast<Node *>(d_pool.emplaceIntoNewNode(value));
//      if (!d_head_p) {
//          d_head_p = node;
//          node->setNextLink(0);
//          node->setPreviousLink(0);
//      }
//      else {
//          Util::insertLinkAfterTarget(node, d_tail_p);
//      }
//      d_tail_p = node;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATORTRAITS
#include <bslma_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLSTL_SIMPLEPOOL
#include <bslstl_simplepool.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALNODE
#include <bslalg_bidirectionalnode.h>
#endif

#ifndef INCLUDED_BSLMA_DEALLOCATORPROCTOR
#include <bslma_deallocatorproctor.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_MOVABLEREF
#include <bslmf_movableref.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

namespace BloombergLP {
namespace bslstl {

                       // ===========================
                       // class BidirectionalNodePool
                       // ===========================

template <class VALUE, class ALLOCATOR>
class BidirectionalNodePool {
    // This class provides methods for creating and destroying nodes using the
    // appropriate allocator-traits of the (template parameter) type
    // 'ALLOCATOR'.

    // PRIVATE TYPES
    typedef SimplePool<bslalg::BidirectionalNode<VALUE>, ALLOCATOR> Pool;
        // This 'typedef' is an alias for the memory pool allocator.

    typedef typename Pool::AllocatorTraits                     AllocatorTraits;
        // This 'typedef' is an alias for the allocator traits defined by
        // 'SimplePool'.

    typedef bslmf::MovableRefUtil                              MoveUtil;
        // This typedef is a convenient alias for the utility associated with
        // movable references.

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

    typedef typename AllocatorTraits::size_type size_type;
        // Alias for the 'size_type' of the allocator defined by 'SimplePool'.

  public:
    // CREATORS
    explicit BidirectionalNodePool(const ALLOCATOR& allocator);
        // Create a 'BidirectionalNodePool' object that will use the specified
        // 'allocator' to supply memory for allocated node objects.  If the
        // (template parameter) 'ALLOCATOR' is 'bsl::allocator', then
        // 'allocator' shall be convertible to 'bslma::Allocator *'.

    BidirectionalNodePool(bslmf::MovableRef<BidirectionalNodePool> original);
        // Create a bidirectional node-pool, adopting all outstanding memory
        // allocations associated with the specified 'original' node-pool, that
        // will use the allocator associated with 'original' to supply memory
        // for allocated node objects.  'original' is left in a valid but
        // unspecified state.

    // ~BidirectionalNodePool() = default;
        // Destroy the memory pool maintained by this object, releasing all
        // memory used by the nodes of the type 'BidirectionalNode<VALUE>' in
        // the pool.  Any memory allocated for the nodes' 'value' attribute of
        // the (template parameter) type 'VALUE' will be leaked unless the
        // nodes are explicitly destroyed via the 'destroyNode' method.

    // MANIPULATORS
    void adopt(bslmf::MovableRef<BidirectionalNodePool> pool);
        // Adopt all outstanding memory allocations associated with the
        // specified node 'pool'.  The behavior is undefined unless this pool
        // uses the same allocator as that associated with 'pool'.  The
        // behavior is also undefined unless this pool is in the
        // default-constructed state.

    AllocatorType& allocator();
        // Return a reference providing modifiable access to the allocator
        // supplying memory for the memory pool maintained by this object.  The
        // behavior is undefined if the allocator used by this object is
        // changed with this method.  Note that this method provides modifiable
        // access to enable a client to call non-'const' methods on the
        // allocator.

    bslalg::BidirectionalLink *cloneNode(
                                    const bslalg::BidirectionalLink& original);
        // Allocate a node of the type 'BidirectionalNode<VALUE>', and
        // copy-construct an object of the (template parameter) type 'VALUE'
        // having the same value as the specified 'original' at the 'value'
        // attribute of the node.  Return the address of the node.  Note that
        // the 'next' and 'prev' attributes of the returned node will be
        // uninitialized.

    void deleteNode(bslalg::BidirectionalLink *linkNode);
        // Destroy the 'VALUE' attribute of the specified 'linkNode' and return
        // the memory footprint of 'linkNode' to this pool for potential reuse.
        // The behavior is undefined unless 'node' refers to a
        // 'bslalg::BidirectionalNode<VALUE>' that was allocated by this pool.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    bslalg::BidirectionalLink *emplaceIntoNewNode(Args&&... arguments);
        // Allocate a node of the type 'BidirectionalNode<VALUE>', and
        // construct in-place an object of the (template parameter) type
        // 'VALUE' with the specified constructor 'arguments'.  Return the
        // address of the node.  Note that the 'next' and 'prev' attributes of
        // the returned node will be uninitialized.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_bidirectionalnodepool.h
    bslalg::BidirectionalLink *emplaceIntoNewNode(
                         );

    template <class Args_01>
    bslalg::BidirectionalLink *emplaceIntoNewNode(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class Args_01,
              class Args_02>
    bslalg::BidirectionalLink *emplaceIntoNewNode(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class Args_01,
              class Args_02,
              class Args_03>
    bslalg::BidirectionalLink *emplaceIntoNewNode(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04>
    bslalg::BidirectionalLink *emplaceIntoNewNode(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05>
    bslalg::BidirectionalLink *emplaceIntoNewNode(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06>
    bslalg::BidirectionalLink *emplaceIntoNewNode(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07>
    bslalg::BidirectionalLink *emplaceIntoNewNode(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08>
    bslalg::BidirectionalLink *emplaceIntoNewNode(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09>
    bslalg::BidirectionalLink *emplaceIntoNewNode(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09,
              class Args_10>
    bslalg::BidirectionalLink *emplaceIntoNewNode(
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... Args>
    bslalg::BidirectionalLink *emplaceIntoNewNode(
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
// }}} END GENERATED CODE
#endif

    bslalg::BidirectionalLink *moveIntoNewNode(
                                          bslalg::BidirectionalLink *original);
        // Allocate a node of the type 'BidirectionalNode<VALUE>', and
        // move-construct an object of the (template parameter) type 'VALUE'
        // with the (explicitly moved) value indicated by the 'value' attribute
        // of the specified 'original' link.  Return the address of the node.
        // Note that the 'next' and 'prev' attributes of the returned node will
        // be uninitialized.  Also note that the 'value' attribute of
        // 'original' is left in a valid but unspecified state.

    void release();
        // Relinquish all memory currently allocated with the memory pool
        // maintained by this object.

    void reserveNodes(size_type numNodes);
        // Reserve memory from this pool to satisfy memory requests for at
        // least the specified 'numNodes' before the pool replenishes.  The
        // behavior is undefined unless '0 < numNodes'.

    void swapRetainAllocators(BidirectionalNodePool& other);
        // Efficiently exchange the nodes of this object with those of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless
        // 'allocator() == other.allocator()'.

    void swapExchangeAllocators(BidirectionalNodePool& other);
        // Efficiently exchange the nodes and the allocator of this object with
        // those of the specified 'other' object.  This method provides the
        // no-throw exception-safety guarantee.

    // ACCESSORS
    const AllocatorType& allocator() const;
        // Return a reference providing non-modifiable access to the allocator
        // supplying memory for the memory pool maintained by this object.
};

// FREE FUNCTIONS
template <class VALUE, class ALLOCATOR>
void swap(BidirectionalNodePool<VALUE, ALLOCATOR>& a,
          BidirectionalNodePool<VALUE, ALLOCATOR>& b);
    // Efficiently exchange the nodes of the specified 'a' object with those of
    // the specified 'b' object.  This method provides the no-throw
    // exception-safety guarantee.  The behavior is undefined unless
    // 'a.allocator() == b.allocator()'.

}  // close package namespace


// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for HashTable:
//: o A HashTable is bitwise moveable if the allocator is bitwise moveable.

namespace bslmf {

template <class VALUE, class ALLOCATOR>
struct IsBitwiseMoveable<bslstl::BidirectionalNodePool<VALUE, ALLOCATOR> >
: bsl::integral_constant<bool, bslmf::IsBitwiseMoveable<ALLOCATOR>::value>
{};

}  // close namespace bslmf

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace bslstl {

// CREATORS
template <class VALUE, class ALLOCATOR>
inline
BidirectionalNodePool<VALUE, ALLOCATOR>::BidirectionalNodePool(
                                                    const ALLOCATOR& allocator)
: d_pool(allocator)
{
}

template <class VALUE, class ALLOCATOR>
inline
BidirectionalNodePool<VALUE, ALLOCATOR>::BidirectionalNodePool(
                             bslmf::MovableRef<BidirectionalNodePool> original)
: d_pool(MoveUtil::move(MoveUtil::access(original).d_pool))
{
}

// MANIPULATORS
template <class VALUE, class ALLOCATOR>
inline
void BidirectionalNodePool<VALUE, ALLOCATOR>::adopt(
                                 bslmf::MovableRef<BidirectionalNodePool> pool)
{
    BidirectionalNodePool& lvalue = pool;
    d_pool.adopt(MoveUtil::move(lvalue.d_pool));
}

template <class VALUE, class ALLOCATOR>
inline
typename
SimplePool<bslalg::BidirectionalNode<VALUE>, ALLOCATOR>::AllocatorType&
BidirectionalNodePool<VALUE, ALLOCATOR>::allocator()
{
    return d_pool.allocator();
}

template <class VALUE, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::cloneNode(
                                     const bslalg::BidirectionalLink& original)
{
    return emplaceIntoNewNode(
       static_cast<const bslalg::BidirectionalNode<VALUE>&>(original).value());
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class VALUE, class ALLOCATOR>
template <class... Args>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::emplaceIntoNewNode(
                                                           Args&&... arguments)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(
                             allocator(),
                             bsls::Util::addressOf(node->value()),
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
    proctor.release();
    return node;
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_bidirectionalnodepool.h
template <class VALUE, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::emplaceIntoNewNode(
                               )
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               bsls::Util::addressOf(node->value()));
    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
template <class Args_01>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::emplaceIntoNewNode(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               bsls::Util::addressOf(node->value()),
                               BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01));
    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
template <class Args_01,
          class Args_02>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::emplaceIntoNewNode(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               bsls::Util::addressOf(node->value()),
                               BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                               BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02));
    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::emplaceIntoNewNode(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               bsls::Util::addressOf(node->value()),
                               BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                               BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                               BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03));
    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::emplaceIntoNewNode(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               bsls::Util::addressOf(node->value()),
                               BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                               BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                               BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                               BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04));
    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::emplaceIntoNewNode(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               bsls::Util::addressOf(node->value()),
                               BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                               BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                               BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                               BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04),
                               BSLS_COMPILERFEATURES_FORWARD(Args_05,args_05));
    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::emplaceIntoNewNode(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               bsls::Util::addressOf(node->value()),
                               BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                               BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                               BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                               BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04),
                               BSLS_COMPILERFEATURES_FORWARD(Args_05,args_05),
                               BSLS_COMPILERFEATURES_FORWARD(Args_06,args_06));
    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::emplaceIntoNewNode(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               bsls::Util::addressOf(node->value()),
                               BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                               BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                               BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                               BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04),
                               BSLS_COMPILERFEATURES_FORWARD(Args_05,args_05),
                               BSLS_COMPILERFEATURES_FORWARD(Args_06,args_06),
                               BSLS_COMPILERFEATURES_FORWARD(Args_07,args_07));
    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::emplaceIntoNewNode(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) args_08)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               bsls::Util::addressOf(node->value()),
                               BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                               BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                               BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                               BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04),
                               BSLS_COMPILERFEATURES_FORWARD(Args_05,args_05),
                               BSLS_COMPILERFEATURES_FORWARD(Args_06,args_06),
                               BSLS_COMPILERFEATURES_FORWARD(Args_07,args_07),
                               BSLS_COMPILERFEATURES_FORWARD(Args_08,args_08));
    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08,
          class Args_09>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::emplaceIntoNewNode(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) args_08,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) args_09)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               bsls::Util::addressOf(node->value()),
                               BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                               BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                               BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                               BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04),
                               BSLS_COMPILERFEATURES_FORWARD(Args_05,args_05),
                               BSLS_COMPILERFEATURES_FORWARD(Args_06,args_06),
                               BSLS_COMPILERFEATURES_FORWARD(Args_07,args_07),
                               BSLS_COMPILERFEATURES_FORWARD(Args_08,args_08),
                               BSLS_COMPILERFEATURES_FORWARD(Args_09,args_09));
    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08,
          class Args_09,
          class Args_10>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::emplaceIntoNewNode(
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) args_08,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) args_09,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) args_10)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               bsls::Util::addressOf(node->value()),
                               BSLS_COMPILERFEATURES_FORWARD(Args_01,args_01),
                               BSLS_COMPILERFEATURES_FORWARD(Args_02,args_02),
                               BSLS_COMPILERFEATURES_FORWARD(Args_03,args_03),
                               BSLS_COMPILERFEATURES_FORWARD(Args_04,args_04),
                               BSLS_COMPILERFEATURES_FORWARD(Args_05,args_05),
                               BSLS_COMPILERFEATURES_FORWARD(Args_06,args_06),
                               BSLS_COMPILERFEATURES_FORWARD(Args_07,args_07),
                               BSLS_COMPILERFEATURES_FORWARD(Args_08,args_08),
                               BSLS_COMPILERFEATURES_FORWARD(Args_09,args_09),
                               BSLS_COMPILERFEATURES_FORWARD(Args_10,args_10));
    proctor.release();
    return node;
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class VALUE, class ALLOCATOR>
template <class... Args>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::emplaceIntoNewNode(
                               BSLS_COMPILERFEATURES_FORWARD_REF(Args)... args)
{
    bslalg::BidirectionalNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               bsls::Util::addressOf(node->value()),
                               BSLS_COMPILERFEATURES_FORWARD(Args,args)...);
    proctor.release();
    return node;
}
// }}} END GENERATED CODE
#endif

template <class VALUE, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
BidirectionalNodePool<VALUE, ALLOCATOR>::moveIntoNewNode(
                                           bslalg::BidirectionalLink *original)
{
    return emplaceIntoNewNode(MoveUtil::move(
        static_cast<bslalg::BidirectionalNode<VALUE> *>(original)->value()));
}

template <class VALUE, class ALLOCATOR>
void BidirectionalNodePool<VALUE, ALLOCATOR>::deleteNode(
                                           bslalg::BidirectionalLink *linkNode)
{
    BSLS_ASSERT(linkNode);

    bslalg::BidirectionalNode<VALUE> *node =
                     static_cast<bslalg::BidirectionalNode<VALUE> *>(linkNode);
    AllocatorTraits::destroy(allocator(),
                             bsls::Util::addressOf(node->value()));
    d_pool.deallocate(node);
}

template <class VALUE, class ALLOCATOR>
inline
void BidirectionalNodePool<VALUE, ALLOCATOR>::release()
{
    d_pool.release();
}

template <class VALUE, class ALLOCATOR>
inline
void BidirectionalNodePool<VALUE, ALLOCATOR>::reserveNodes(size_type numNodes)
{
    BSLS_ASSERT_SAFE(0 < numNodes);

    d_pool.reserve(numNodes);
}

template <class VALUE, class ALLOCATOR>
inline
void BidirectionalNodePool<VALUE, ALLOCATOR>::swapRetainAllocators(
                                BidirectionalNodePool<VALUE, ALLOCATOR>& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    d_pool.quickSwapRetainAllocators(other.d_pool);
}

template <class VALUE, class ALLOCATOR>
inline
void BidirectionalNodePool<VALUE, ALLOCATOR>::swapExchangeAllocators(
                                BidirectionalNodePool<VALUE, ALLOCATOR>& other)
{
    d_pool.quickSwapExchangeAllocators(other.d_pool);
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

}  // close package namespace

template <class VALUE, class ALLOCATOR>
inline
void bslstl::swap(bslstl::BidirectionalNodePool<VALUE, ALLOCATOR>& a,
                  bslstl::BidirectionalNodePool<VALUE, ALLOCATOR>& b)
{
    a.swapRetainAllocators(b);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
