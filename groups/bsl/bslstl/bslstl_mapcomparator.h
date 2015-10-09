// bslstl_mapcomparator.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_MAPCOMPARATOR
#define INCLUDED_BSLSTL_MAPCOMPARATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a comparator for 'TreeNode' objects and a lookup key.
//
//@CLASSES:
//   bslstl::MapComparator: comparator for 'TreeNode' objects and key objects
//
//@SEE_ALSO: bslstl_map, bslstl_treenode, bslalg_rbtreeutil
//
//@DESCRIPTION: This component provides a functor adapter, 'MapComparator',
// that adapts a parameterized 'COMPARATOR' comparing objects of a
// parameterized 'KEY' type into a functor comparing a object of 'KEY' type
// with objects of 'TreeNode' type holding a 'bsl::pair<KEY, VALUE>' object.
// Note that this functor was designed to be supplied to functions in
// 'bslalg::RbTreeUtil', primarily for the purpose of implementing a 'map'
// container using the utilities defined in 'bslalg::RbTreeUtil'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Create a Simple Tree of 'TreeNode' Objects
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to create a tree of 'TreeNode' objects arranged
// according to a functor that we supply.
//
// First, we create an array of 'bslstl::TreeNode' objects, each holding a pair
// of integers:
//..
//  typedef bsl::allocator<TreeNode<bsl::pair<int, int> > > Alloc;
//
//  bslma::TestAllocator oa;
//  Alloc allocator(&oa);
//
//  enum { NUM_NODES = 5 };
//
//  bslstl::TreeNode<bsl::pair<int, int> >* nodes[NUM_NODES];
//
//  typedef bsl::allocator_traits<Alloc>  AllocTraits;
//
//  for (int i = 0; i < NUM_NODES; ++i) {
//      nodes[i] = AllocTraits::allocate(allocator, 1);
//      AllocTraits::construct(allocator, &nodes[i]->value(),
//                             i, 2*i);
//  }
//..
// Then, we define a 'MapComparator' object, 'comparator', for comparing
// 'bslstl::TreeNode<pair<int, int> >' objects with integers.
//..
//  MapComparator<int, int, std::less<int> > comparator;
//..
// Now, we can use the functions in 'bslalg::RbTreeUtil' to arrange our tree:
//..
//  bslalg::RbTreeAnchor tree;
//
//  for (int i = 0; i < NUM_NODES; ++i) {
//      int comparisonResult;
//      bslalg::RbTreeNode *insertLocation =
//          bslalg::RbTreeUtil::findUniqueInsertLocation(
//              &comparisonResult,
//              &tree,
//              comparator,
//              nodes[i]->value().first);
//
//      assert(0 != comparisonResult);
//
//      bslalg::RbTreeUtil::insertAt(&tree,
//                                   insertLocation,
//                                   comparisonResult < 0,
//                                   nodes[i]);
//  }
//
//  assert(5 == tree.numNodes());
//..
// Then, we use 'bslalg::RbTreeUtil::next()' to navigate the elements of the
// tree, printing their values:
//..
//  const bslalg::RbTreeNode *nodeIterator = tree.firstNode();
//
//  while (nodeIterator != tree.sentinel()) {
//      const TreeNode<bsl::pair<int, int> > *node =
//      static_cast<const TreeNode<bsl::pair<int, int> >*>(nodeIterator);
//      printf("Node value: (%d, %d)\n",
//             node->value().first, node->value().second);
//      nodeIterator = bslalg::RbTreeUtil::next(nodeIterator);
//  }
//..
// Next, we destroy and deallocate each of the 'bslstl::TreeNode' objects:
//..
//  for (int i = 0; i < NUM_NODES; ++i) {
//      AllocTraits::destroy(allocator, &nodes[i]->value());
//      AllocTraits::deallocate(allocator, nodes[i], 1);
//  }
//..
// Finally, we observe the console output:
//..
//  Node value: (0, 0)
//  Node value: (1, 2)
//  Node value: (2, 4)
//  Node value: (3, 6)
//  Node value: (4, 8)
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_mapcomparator.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_PAIR
#include <bslstl_pair.h>
#endif

#ifndef INCLUDED_BSLSTL_TREENODE
#include <bslstl_treenode.h>
#endif

#ifndef INCLUDED_BSLALG_FUNCTORADAPTER
#include <bslalg_functoradapter.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

namespace BloombergLP {
namespace bslstl {

                       // ===================
                       // class MapComparator
                       // ===================

template <class KEY, class VALUE, class COMPARATOR>
#ifdef BSLS_PLATFORM_CMP_MSVC
// Visual studio compiler fails to resolve the conversion operator in
// 'bslalg::FunctorAdapter_FunctionPointer' when using private inheritance.
// Below is a workaround until a more suitable way the resolve this issue can
// be found.
class MapComparator : public bslalg::FunctorAdapter<COMPARATOR>::Type {
#else
class MapComparator : private bslalg::FunctorAdapter<COMPARATOR>::Type {
#endif
    // This class overloads the function-call operator to compare a referenced
    // 'bslalg::RbTreeNode' object with a object of the parameterized 'KEY'
    // type, assuming the reference to 'bslalg::RbTreeNode' is a base of a
    // 'bslstl::TreeNode' holding a 'pair<KEY, VALUE>', using a functor of the
    // parameterized 'COMPARATOR' type.

  private:
    // This class does not support assignment.

    MapComparator& operator=(const MapComparator&);  // Declared but not
                                                     // defined

  public:
    // TYPES
    typedef bsl::pair<const KEY, VALUE>  ValueType;
        // This alias represents the type of the values held by nodes in an
        // 'bslalg::RbTree' object.

    typedef TreeNode<ValueType> NodeType;
        // This alias represents the type of node holding a 'ValueType' object.

    // CREATORS
    MapComparator();
        // Create a 'MapComparator' object that will use a default constructed
        // 'COMPARATOR'.

    explicit MapComparator(const COMPARATOR& keyComparator);
        // Create a 'MapComparator' object holding a copy of the specified
        // 'keyComparator'.

    // MapComparator(const MapComparator&) = default;
        // Create a 'MapComparator' object with the 'COMPARATOR' object of the
        // specified 'original' object.

    // ~MapComparator() = default;
        // Destroy this object.

    // MANIPULATORS
    bool operator()(const KEY&                lhs,
                    const bslalg::RbTreeNode& rhs);
        // Return 'true' if the specified 'lhs' is less than (ordered before,
        // according to the comparator held by this object) 'value().first' of
        // the specified 'rhs' after being cast to 'NodeType', and 'false'
        // otherwise.  The behavior is undefined unless 'rhs' can be safely
        // cast to 'NodeType'.

    bool operator()(const bslalg::RbTreeNode& lhs,
                    const KEY&                rhs);
        // Return 'true' if 'value().first()' of the specified 'lhs' after
        // being cast to 'NodeType' is less than (ordered before, according to
        // the comparator held by this object) the specified 'rhs', and 'false'
        // otherwise.  The behavior is undefined unless 'rhs' can be safely
        // cast to 'NodeType'.

    void swap(MapComparator& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.

    // ACCESSORS
    bool operator()(const KEY&                lhs,
                    const bslalg::RbTreeNode& rhs) const;
        // Return 'true' if the specified 'lhs' is less than (ordered before,
        // according to the comparator held by this object) 'value().first' of
        // the specified 'rhs' after being cast to 'NodeType', and 'false'
        // otherwise.  The behavior is undefined unless 'rhs' can be safely
        // cast to 'NodeType'.

    bool operator()(const bslalg::RbTreeNode& lhs,
                    const KEY&                rhs) const;
        // Return 'true' if 'value().first()' of the specified 'lhs' after
        // being cast to 'NodeType' is less than (ordered before, according to
        // the comparator held by this object) the specified 'rhs', and 'false'
        // otherwise.  The behavior is undefined unless 'rhs' can be safely
        // cast to 'NodeType'.

    COMPARATOR& keyComparator();
        // Return a reference providing modifiable access to the function
        // pointer or functor to which this comparator delegates comparison
        // operations.

    const COMPARATOR& keyComparator() const;
        // Return a reference providing non-modifiable access to the function
        // pointer or functor to which this comparator delegates comparison
        // operations.
};

// FREE FUNCTIONS

template <class KEY,  class VALUE,  class COMPARATOR>
void swap(MapComparator<KEY, VALUE, COMPARATOR>& a,
          MapComparator<KEY, VALUE, COMPARATOR>& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // -------------------
                    // class MapComparator
                    // -------------------

// CREATORS
template <class KEY, class VALUE, class COMPARATOR>
inline
MapComparator<KEY, VALUE, COMPARATOR>::MapComparator()
: bslalg::FunctorAdapter<COMPARATOR>::Type()
{
}

template <class KEY, class VALUE, class COMPARATOR>
inline
MapComparator<KEY, VALUE, COMPARATOR>::
MapComparator(const COMPARATOR& valueComparator)
: bslalg::FunctorAdapter<COMPARATOR>::Type(valueComparator)
{
}

// MANIPULATORS
template <class KEY, class VALUE, class COMPARATOR>
inline
void MapComparator<KEY, VALUE, COMPARATOR>::swap(
                                  MapComparator<KEY, VALUE, COMPARATOR>& other)
{
    bslalg::SwapUtil::swap(
      static_cast<typename bslalg::FunctorAdapter<COMPARATOR>::Type*>(this),
      static_cast<typename bslalg::FunctorAdapter<COMPARATOR>::Type*>(
                                                  BSLS_UTIL_ADDRESSOF(other)));
}

// ACCESSOR
template <class KEY, class VALUE, class COMPARATOR>
inline
bool MapComparator<KEY, VALUE, COMPARATOR>::operator()(
                                                 const KEY&                lhs,
                                                 const bslalg::RbTreeNode& rhs)
{
    return keyComparator()(lhs,
                           static_cast<const NodeType&>(rhs).value().first);
}

template <class KEY, class VALUE, class COMPARATOR>
inline
bool MapComparator<KEY, VALUE, COMPARATOR>::operator()(
                                           const KEY&                lhs,
                                           const bslalg::RbTreeNode& rhs) const
{
    return keyComparator()(lhs,
                           static_cast<const NodeType&>(rhs).value().first);
}

template <class KEY, class VALUE, class COMPARATOR>
inline
bool MapComparator<KEY, VALUE, COMPARATOR>::operator()(
                                                 const bslalg::RbTreeNode& lhs,
                                                 const KEY&                rhs)
{
    return keyComparator()(static_cast<const NodeType&>(lhs).value().first,
                           rhs);
}

template <class KEY, class VALUE, class COMPARATOR>
inline
bool MapComparator<KEY, VALUE, COMPARATOR>::operator()(
                                           const bslalg::RbTreeNode& lhs,
                                           const KEY&                rhs) const
{
    return keyComparator()(static_cast<const NodeType&>(lhs).value().first,
                           rhs);
}

template <class KEY, class VALUE, class COMPARATOR>
inline
COMPARATOR&
MapComparator<KEY, VALUE, COMPARATOR>::keyComparator()
{
    return *this;
}

template <class KEY, class VALUE, class COMPARATOR>
inline
const COMPARATOR&
MapComparator<KEY, VALUE, COMPARATOR>::keyComparator() const
{
    return *this;
}


// FREE FUNCTIONS
template <class KEY,  class VALUE,  class COMPARATOR>
void swap(MapComparator<KEY, VALUE, COMPARATOR>& a,
          MapComparator<KEY, VALUE, COMPARATOR>& b)
{
    a.swap(b);
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
