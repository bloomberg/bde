// bslstl_treenodepool.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_TREENODEPOOL
#define INCLUDED_BSLSTL_TREENODEPOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide efficient creation of nodes used in tree-based container.
//
//@CLASSES:
//   bslstl::TreeNodePool: memory manager to allocate tree nodes
//
//@SEE_ALSO: bslstl_simplepool
//
//@DESCRIPTION: This component implements a mechanism that creates and deletes
// 'bslstl::TreeNode' objects for the parameterized 'VALUE' type for use in a
// tree-based container.
//
// A 'bslstl::TreeNodePool' contains a memory pool provided by the
// 'bslstl_simplepool' component to provide memory for the nodes (see
// 'bslstl_simplepool').  When the pool is empty, a number of memory blocks is
// allocated and added to the pool, where each block is large enough to contain
// a 'bslstl::TreeNode'.  The first allocation contains one memory block.
// Subsequent allocations double the number of memory blocks of the previous
// allocation up to an implementation defined maximum number of blocks.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a 'IntSet' Container
/// - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create a container type, 'IntSet' using
// 'bslalg::RbTreeUtil'.
//
// First, we define a comparison functor for comparing a
// 'bslstl::RbTreeNode<int>' object and an 'int' value.  This functor conforms
// to the requirements of 'bslalg::RbTreeUtil':
//..
//  struct IntNodeComparator {
//      // This class defines a comparator providing comparison operations
//      // between 'bslstl::TreeNode<int>' objects, and 'int' values.
//
//    private:
//      // PRIVATE TYPES
//      typedef bslstl::TreeNode<int> Node;
//          // Alias for a node type containing an 'int' value.
//
//    public:
//      // CLASS METHODS
//      bool operator()(const bslalg::RbTreeNode& lhs, int rhs) const
//      {
//          return static_cast<const Node&>(lhs).value() < rhs;
//      }
//
//      bool operator()(int lhs, const bslalg::RbTreeNode& rhs) const
//      {
//          return lhs < static_cast<const Node&>(rhs).value();
//      }
//  };
//..
// Then, we define the public interface of 'IntSet'.  Note that it contains a
// 'TreeNodePool' that will be used by 'bslalg::RbTreeUtil' as a 'FACTORY' to
// create and delete nodes.  Also note that a number of simplifications have
// been made for the purpose of illustration.  For example, this implementation
// provides only a minimal set of critical operations, and it does not use the
// empty base-class optimization for the comparator.
//..
//  template <class ALLOCATOR = bsl::allocator<int> >
//  class IntSet {
//      // This class implements a set of (unique) 'int' values.
//
//      // PRIVATE TYPES
//      typedef bslstl::TreeNodePool<int, ALLOCATOR> TreeNodePool;
//
//      // DATA
//      bslalg::RbTreeAnchor d_tree;      // tree of node objects
//      TreeNodePool         d_nodePool;  // allocator for node objects
//
//      // NOT IMPLEMENTED
//      IntSet(const IntSet&);
//      IntSet& operator=(const IntSet&);
//
//    public:
//      // CREATORS
//      IntSet(const ALLOCATOR& allocator = ALLOCATOR());
//          // Create an empty set.  Optionally specify an 'allocator' used to
//          // supply memory.  If 'allocator' is not specified, a default
//          // constructed 'ALLOCATOR' object is used.
//
//      //! ~IntSet() = 0;
//          // Destroy this object.
//
//      // MANIPULATORS
//      void insert(int value);
//          // Insert the specified 'value' into this set.
//
//      bool remove(int value);
//          // If 'value' is a member of this set, then remove it from the set
//          // and return 'true'.  Otherwise, return 'false' with no effect.
//
//      // ACCESSORS
//      bool isElement(int value) const;
//          // Return 'true' if the specified 'value' is a member of this set,
//          // and 'false' otherwise.
//
//      int numElements() const;
//          // Return the number of elements in this set.
//  };
//..
// Now, we implement the methods of 'IntSet' using 'RbTreeUtil'.
//..
//  // CREATORS
//  template <class ALLOCATOR>
//  inline
//  IntSet<ALLOCATOR>::IntSet(const ALLOCATOR& allocator)
//  : d_tree()
//  , d_nodePool(allocator)
//  {
//  }
//
//  // MANIPULATORS
//  template <class ALLOCATOR>
//  void IntSet<ALLOCATOR>::insert(int value)
//  {
//      int comparisonResult;
//      bslalg::RbTreeNode *parent =
//          bslalg::RbTreeUtil::findUniqueInsertLocation(&comparisonResult,
//                                                       &d_tree,
//                                                       IntNodeComparator(),
//                                                       value);
//..
// Here we use the 'TreeNodePool' object, 'd_nodePool', to create the node that
// was inserted into the set.
//..
//      if (0 != comparisonResult) {
//          bslalg::RbTreeNode *node = d_nodePool.createNode(value);
//          bslalg::RbTreeUtil::insertAt(&d_tree,
//                                       parent,
//                                       comparisonResult < 0,
//                                       node);
//      }
//  }
//
//  template <class ALLOCATOR>
//  bool IntSet<ALLOCATOR>::remove(int value)
//  {
//      IntNodeComparator comparator;
//      bslalg::RbTreeNode *node =
//                bslalg::RbTreeUtil::find(d_tree, comparator, value);
//..
// Here we use the 'TreeNodePool' object, 'd_nodePool', to delete a node that
// was removed from the set.
//..
//      if (node) {
//          bslalg::RbTreeUtil::remove(&d_tree, node);
//          d_nodePool.deleteNode(node);
//      }
//      return node;
//  }
//
//  // ACCESSORS
//  template <class ALLOCATOR>
//  inline
//  bool IntSet<ALLOCATOR>::isElement(int value) const
//  {
//      return bslalg::RbTreeUtil::find(d_tree, IntNodeComparator(), value);
//  }
//
//  template <class ALLOCATOR>
//  inline
//  int IntSet<ALLOCATOR>::numElements() const
//  {
//      return d_tree.numNodes();
//  }
//..
// Finally, we create a sample 'IntSet' object and insert 3 values into the
// 'IntSet'.  We verify the attributes of the 'Set' before and after each
// insertion.
//..
//  bslma::TestAllocator defaultAllocator("defaultAllocator");
//  bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);
//
//  bslma::TestAllocator objectAllocator("objectAllocator");
//
//  IntSet<bsl::allocator<int> > set(&objectAllocator);
//  assert(0 == defaultAllocator.numBytesInUse());
//  assert(0 == objectAllocator.numBytesInUse());
//  assert(0 == set.numElements());
//
//  set.insert(1);
//  assert(set.isElement(1));
//  assert(1 == set.numElements());
//
//  set.insert(1);
//  assert(set.isElement(1));
//  assert(1 == set.numElements());
//
//  set.insert(2);
//  assert(set.isElement(1));
//  assert(set.isElement(2));
//  assert(2 == set.numElements());
//
//  assert(0 == defaultAllocator.numBytesInUse());
//  assert(0 <  objectAllocator.numBytesInUse());
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_treenodepool.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#include <bslstl_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLSTL_SIMPLEPOOL
#include <bslstl_simplepool.h>
#endif

#ifndef INCLUDED_BSLSTL_TREENODE
#include <bslstl_treenode.h>
#endif

#ifndef INCLUDED_BSLALG_RBTREENODE
#include <bslalg_rbtreenode.h>
#endif

#ifndef INCLUDED_BSLMA_DEALLOCATORPROCTOR
#include <bslma_deallocatorproctor.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

namespace BloombergLP {
namespace bslstl {

                       // ==================
                       // class TreeNodePool
                       // ==================

template <class VALUE, class ALLOCATOR>
class TreeNodePool {
    // This class provides methods for creating and deleting nodes using the
    // appropriate allocator-traits of the parameterized 'ALLOCATOR'.  This
    // type is intended to be used as a private base-class for a node-based
    // container, in order to take advantage of the empty-base-class
    // optimization in the case where the base-class has 0 size (as may the
    // case if the parameterized 'ALLOCATOR' is not a 'bslma::Allocator').

    typedef SimplePool<TreeNode<VALUE>, ALLOCATOR> Pool;
        // Alias for the memory pool allocator.

    typedef typename Pool::AllocatorTraits         AllocatorTraits;
        // Alias for the allocator traits defined by 'SimplePool'.

    // DATA
    Pool d_pool;  // pool for allocating memory

  private:
    // NOT IMPLEMENTED
    TreeNodePool& operator=(const TreeNodePool&);
    TreeNodePool(const TreeNodePool&);

  public:
    // PUBLIC TYPE
    typedef typename Pool::AllocatorType AllocatorType;
        // Alias for the allocator type defined by 'SimplePool'.

    typedef typename AllocatorTraits::size_type size_type;
        // Alias for the 'size_type' of the allocator defined by 'SimplePool'.

  public:
    // CREATORS
    explicit TreeNodePool(const ALLOCATOR& allocator);
        // Create a node-allocator that will use the specified 'allocator' to
        // supply memory for allocated node objects.

    // MANIPULATORS
    AllocatorType& allocator();
        // Return a reference providing modifiable access to the rebound
        // allocator traits for the node-type.  Note that this operation
        // returns a base-class ('NodeAlloc') reference to this object.

    bslalg::RbTreeNode *createNode();
        // Allocate a node object having a default constructed 'VALUE'.

    bslalg::RbTreeNode *createNode(const bslalg::RbTreeNode& original);
        // Allocate a node object having a copy-constructed 'VALUE' of
        // 'value()' of the specified 'original'.  The behavior is undefined
        // unless 'original' refers to a 'TreeNode<VALUE>'.

    bslalg::RbTreeNode *createNode(const VALUE& value);
        // Allocate a node object having the specified 'value'.  This operation
        // will copy-construct 'value' into the value of the returned node.

    void deleteNode(bslalg::RbTreeNode *node);
        // Destroy the 'VALUE' value of the specified 'node' and return the
        // memory footprint of 'node' to this pool for potential reuse.  The
        // behavior is undefined unless 'node' refers to a 'TreeNode<VALUE>'.

    void reserveNodes(size_type numNodes);
        // Reserve memory from this pool to satisfy memory requests for at
        // least the specified 'numNodes' before the pool replenishes.  The
        // behavior is undefined unless '0 < numNodes'.

    void swap(TreeNodePool<VALUE, ALLOCATOR>& other);
        // Efficiently exchange the management of nodes of this object and
        // the specified 'other' object.  The behavior is undefined unless the
        // underlying mechanisms of 'allocator()' refers to the same allocator.

    // ACCESSORS
    const AllocatorType& allocator() const;
        // Return a reference providing non-modifiable access to the rebound
        // allocator traits for the node-type.  Note that this operation
        // returns a base-class ('NodeAlloc') reference to this object.

};

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
template <class VALUE, class ALLOCATOR>
inline
TreeNodePool<VALUE, ALLOCATOR>::TreeNodePool(const ALLOCATOR& allocator)
: d_pool(allocator)
{
}

// MANIPULATORS
template <class VALUE, class ALLOCATOR>
inline
typename SimplePool<TreeNode<VALUE>, ALLOCATOR>::AllocatorType&
TreeNodePool<VALUE, ALLOCATOR>::allocator()
{
    return d_pool.allocator();
}

template <class VALUE, class ALLOCATOR>
inline
bslalg::RbTreeNode *TreeNodePool<VALUE, ALLOCATOR>::createNode()
{
    TreeNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               BSLS_UTIL_ADDRESSOF(node->value()));

    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
inline
bslalg::RbTreeNode *TreeNodePool<VALUE, ALLOCATOR>::createNode(
                                                            const VALUE& value)
{
    TreeNode<VALUE> *node = d_pool.allocate();
    bslma::DeallocatorProctor<Pool> proctor(node, &d_pool);

    AllocatorTraits::construct(allocator(),
                               BSLS_UTIL_ADDRESSOF(node->value()),
                               value);
    proctor.release();
    return node;
}

template <class VALUE, class ALLOCATOR>
inline
bslalg::RbTreeNode *TreeNodePool<VALUE, ALLOCATOR>::createNode(
                                            const bslalg::RbTreeNode& original)
{
    return createNode(static_cast<const TreeNode<VALUE>&>(original).value());
}

template <class VALUE, class ALLOCATOR>
inline
void TreeNodePool<VALUE, ALLOCATOR>::deleteNode(bslalg::RbTreeNode *node)
{
    BSLS_ASSERT(node);

    TreeNode<VALUE> *treeNode = static_cast<TreeNode<VALUE> *>(node);
    AllocatorTraits::destroy(allocator(),
                             BSLS_UTIL_ADDRESSOF(treeNode->value()));
    d_pool.deallocate(treeNode);
}

template <class VALUE, class ALLOCATOR>
inline
void TreeNodePool<VALUE, ALLOCATOR>::reserveNodes(size_type numNodes)
{
    BSLS_ASSERT_SAFE(0 < numNodes);

    d_pool.reserve(numNodes);
}

template <class VALUE, class ALLOCATOR>
inline
void TreeNodePool<VALUE, ALLOCATOR>::swap(
                                         TreeNodePool<VALUE, ALLOCATOR>& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    d_pool.swap(other.d_pool);
}

// ACCESSORS
template <class VALUE, class ALLOCATOR>
inline
const typename SimplePool<TreeNode<VALUE>, ALLOCATOR>::AllocatorType&
TreeNodePool<VALUE, ALLOCATOR>::allocator() const
{
    return d_pool.allocator();
}

}  // close package namespace
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
