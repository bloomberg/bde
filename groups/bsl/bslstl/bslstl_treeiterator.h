// bslstl_treeiterator.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_TREEITERATOR
#define INCLUDED_BSLSTL_TREEITERATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL compliant iterator for a tree of 'TreeNode' objects.
//
//@CLASSES:
//   bslstl::TreeIterator: an STL compliant bidirectional binary tree iterator
//
//@SEE_ALSO: bslstl_treenode, bslalg_rbtreeutil, bslstl_map
//
//@DESCRIPTION: This component provides an STL-compliant bidirectional iterator
// over a binary tree of 'bslstl::TreeNode' objects.  The requirements of a
// bidirectional STL iterator are outlined in the C++11 standard in section
// [24.2.6] under the tag [bidirectional.iterators].  A 'TreeIterator' object
// is parameterized on 'VALUE', 'NODE', and 'DIFFERENCE_TYPE'.  The
// parameterized 'VALUE' indicates the type of the value value to which this
// iterator provides a references, and may be const-qualified for
// const-iterators.  The parameterized 'NODE' indicates the type of the node
// objects in this tree.  Note that 'NODE' is not necessarily 'TreeNode<VALUE>'
// as 'VALUE' may be const-qualified.  Finally, the parameterized
// 'DIFFERENCE_TYPE' determines the, standard required, 'difference_type' for
// the iterator.  'NODE' must derives from 'bslalg::RbTreeNode', and contains a
// 'value' method that returns a reference providing modifiable access to a
// type that is convertible to the parameterized 'VALUE' (e.g., a
// 'bslstl::TreeNode' object).
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Navigating a Tree Using 'TreeIterator'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we create a simple tree and then use a
// 'TreeIterator' to navigate its elements.
//
// First, we define a type 'IntNode' for the nodes of our tree.  'IntNode'
// inherits from 'bslalg::RbTreeNode' (allowing it to be operated on by
// 'bslalg::RbTreeUtil'), and supplies an integer payload:
//..
//  class IntNode : public bslalg::RbTreeNode {
//      // DATA
//      int d_value;  // actual value represented by the node
//
//    public:
//      // MANIPULATORS
//  int& value() { return d_value; }
//      // Return a reference providing modifiable access to the 'value' of
//      // this object.
//
//  // ACCESSORS
//  const int& value() const { return d_value; }
//      // Return a reference providing non-modifiable access to the
//      // 'value' of this object.
//  };
//..
// Then, we define a comparison function for 'IntNode' objects.  This type is
// designed to be supplied to functions in 'bslalg::RbTreeUtil':
//..
//  class IntNodeComparator {
//    public:
//      // CREATORS
//      IntNodeComparator() {}
//          // Create a node-value comparator.
//
//      // ACCESSORS
//      bool operator()(const bslalg::RbTreeNode& node, int value) const {
//          return static_cast<const IntNode&>(node).value() < value;
//      }
//
//      bool operator()(int value, const bslalg::RbTreeNode& node) const {
//         return value < static_cast<const IntNode&>(node).value();
//      }
//  };
//..
// Next, we define the signature of the example function where we will
// navigate a tree using a 'TreeIterator':
//..
//  void exampleTreeNavigationFunction()
//  {
//..
// Then, we define a 'bslalg::RbTreeAnchor' object to hold our tree, and a
// series of nodes that we will use to populate the tree:
//..
//      enum { NUM_NODES = 5 };
//
//      bslalg::RbTreeAnchor tree;
//      IntNode              nodes[NUM_NODES];
//..
// Next, we assign values to each of the 'nodes' and insert them into 'tree'
// using 'bslalg::RbTreeUtil', supplying the 'insert' function an instance of
// the comparator we defined earlier:
//..
//      for (int i = 0; i < NUM_NODES; ++i) {
//          nodes[i].value() = i;
//          bslalg::RbTreeUtil::insert(&tree, IntNodeComparator(), &nodes[i]);
//      }
//
//      assert(5 == tree.numNodes());
//..
// Then, we create a type alias for a 'TreeIterator' providing non-modifiable
// access to elements in the tree.  Note that in this instance, the iterator
// must provide non-modifiable access as modifying the key value for a node
// would invalidate ordering of the binary search tree:
//..
//      typedef TreeIterator<const int, IntNode, std::ptrdiff_t>
//                                                           ConstTreeIterator;
//..
// Now, we create an instance of the 'TreeIterator' and use it to navigate the
// elements of 'tree', printing their values to the console:
//..
//      ConstTreeIterator iterator(tree.firstNode());
//      ConstTreeIterator endIterator(tree.sentinel());
//      for (; iterator != endIterator; ++iterator) {
//          printf("Node value: %d\n", *iterator);
//      }
//  }
//..
// Finally, we observe the following console output:
//..
//  Node value: 0
//  Node value: 1
//  Node value: 2
//  Node value: 3
//  Node value: 4
//..

#include <bslscm_version.h>

#include <bslstl_iterator.h>

#include <bslalg_rbtreenode.h>
#include <bslalg_rbtreeutil.h>

#include <bslmf_enableif.h>
#include <bslmf_isconvertible.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_removecv.h>

#include <bsls_assert.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>
#include <bsls_util.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_removecvq.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDE

namespace BloombergLP {
namespace bslstl {

                     // ==================
                     // class TreeIterator
                     // ==================

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
class TreeIterator
#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
    : public std::iterator<std::bidirectional_iterator_tag, VALUE>
// On Solaris just to keep studio12-v4 happy, since algorithms takes only
// iterators inheriting from 'std::iterator'.
#endif
{
    // This class provides an STL-conforming bidirectional iterator over the
    // ordered 'bslalg::RbTreeNode' objects in a binary tree (see section
    // [24.2.6 bidirectional.iterators] of the C++11 standard).  A
    // 'TreeIterator' provides access to values of the parameterized 'VALUE',
    // over a binary tree composed of nodes of the parameterized 'NODE' (which
    // must derive from 'bslalg::RbTreeNode').  The parameterized
    // 'DIFFERENCE_TYPE' determines the standard required 'difference_type' of
    // the iterator, without requiring access to the allocator-traits for the
    // node.  The behavior of the 'operator*' method is undefined unless the
    // iterator is at a valid position in the tree (i.e., not the 'end') and
    // the referenced element has not been removed since the iterator was
    // constructed.  'NODE' must derives from 'bslalg::RbTreeNode', and
    // contains a 'value' method that returns a reference providing modifiable
    // access to a type that is convertible to the parameterized 'VALUE' (e.g.,
    // a 'bslstl::TreeNode' object).

    // PRIVATE TYPES
    typedef typename bsl::remove_cv<VALUE>::type        NcType;
    typedef TreeIterator<NcType, NODE, DIFFERENCE_TYPE> NcIter;

    // DATA
    bslalg::RbTreeNode *d_node_p;  // current iterator position

  private:
    // FRIENDS
    template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
    friend bool operator==(const TreeIterator<VALUE1, NODEPTR, DIFF>&,
                           const TreeIterator<VALUE2, NODEPTR, DIFF>&);
#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
    friend bool operator!=(const TreeIterator<VALUE1, NODEPTR, DIFF>&,
                           const TreeIterator<VALUE2, NODEPTR, DIFF>&);
#endif

    template <class OTHER_VALUE, class OTHER_NODE, class OTHER_DIFFERENCE_TYPE>
    friend class TreeIterator;

  public:
    // PUBLIC TYPES
    typedef bsl::bidirectional_iterator_tag iterator_category;
    typedef NcType                          value_type;
    typedef DIFFERENCE_TYPE                 difference_type;
    typedef VALUE*                          pointer;
    typedef VALUE&                          reference;
        // Standard iterator defined types [24.4.2].

    // CREATORS
    TreeIterator();
        // Create an uninitialized iterator.

    explicit TreeIterator(const bslalg::RbTreeNode *node);
        // Create an iterator at the specified 'position'.  The behavior is
        // undefined unless 'node' is of the parameterized 'NODE', which is
        // derived from 'bslalg::RbTreeNode.  Note that this constructor is an
        // implementation detail and is not part of the C++ standard.

#ifndef BSLS_PLATFORM_CMP_SUN
    template <class NON_CONST_ITERATOR>
    TreeIterator(
           const NON_CONST_ITERATOR& original,
           typename bsl::enable_if<bsl::is_convertible<NON_CONST_ITERATOR,
                                                       NcIter>::value,
                                   int>::type = 0)
        // Create an iterator at the same position as the specified 'original'
        // iterator.  Note that this constructor enables converting from
        // modifiable to const iterator types.
        : d_node_p(static_cast<const NcIter&>(original).d_node_p)
        {
            // This constructor template must be defined inline inside the
            // class definition, as Microsoft Visual C++ does not recognize the
            // definition as matching this signature when placed out-of-line.
        }
#else
    TreeIterator(const NcIter& original)
        // Create an iterator at the same position as the specified 'original'
        // iterator.  Note that this constructor enables converting from
        // modifiable to const iterator types.
        : d_node_p(original.d_node_p)
        {
        }

#endif

    //! TreeIterator(const TreeIterator& original) = default;
        // Create an iterator having the same value as the specified
        // 'original'.  Note that this operation is either defined by the
        // constructor taking 'NcIter' (if 'NcType' is the same as 'VALUE'), or
        // generated automatically by the compiler.  Also note that this
        // construct cannot be defined explicitly (without using
        // 'bsls_enableif') to avoid a duplicate declaration when 'NcType' is
        // the same as 'VALUE'.

    //! ~TreeIterator() = default;
        // Destroy this object.

    // MANIPULATORS
    //! TreeIterator& operator=(const TreeIterator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // a return a reference providing modifiable access to this object.

    TreeIterator& operator++();
        // Move this iterator to the next element in the tree and return a
        // reference providing modifiable access to this iterator.  The
        // behavior is undefined unless the iterator refers to an element in
        // the tree.

    TreeIterator& operator--();
        // Move this iterator to the previous element in the tree and return a
        // reference providing modifiable access to this iterator.  The
        // behavior is undefined unless the iterator refers to the past-the-end
        // address or the non-leftmost element in the tree.

    // ACCESSORS
    reference operator*() const;
        // Return a reference providing modifiable access to the value (of the
        // parameterized 'VALUE') of the element at which this iterator is
        // positioned.  The behavior is undefined unless this iterator is at a
        // valid position in the tree.

    pointer operator->() const;
        // Return the address of the value (of the parameterized 'VALUE') of
        // the element at which this iterator is positioned.  The behavior is
        // undefined unless this iterator is at a valid position in the tree.

    const bslalg::RbTreeNode *node() const;
        // Return the address of the non-modifiable tree node at which this
        // iterator is positioned, or 0 if this iterator is not at a valid
        // position in the tree.  Note that this method is an implementation
        // detail and is not part of the C++ standard.
};

// FREE OPERATORS
template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
bool operator==(const TreeIterator<VALUE1, NODEPTR, DIFF>& lhs,
                const TreeIterator<VALUE2, NODEPTR, DIFF>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // have the same value and 'false' otherwise.  Two iterators have the same
    // value if they refer to the same position in the same tree, or if both
    // iterators are at an invalid position in the tree (i.e., the 'end' of the
    // tree, or the default constructed value).

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
bool operator!=(const TreeIterator<VALUE1, NODEPTR, DIFF>& lhs,
                const TreeIterator<VALUE2, NODEPTR, DIFF>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // do not have the same value and 'false' otherwise.  Two iterators do not
    // have the same value if they differ in either the tree to which they
    // refer or the position in that tree.
#endif

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>
operator++(TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>& iter, int);
    // Move the specified 'iter' to the next element in the tree and return the
    // value of 'iter' prior to this call.  The behavior is undefined unless
    // the iterator refers to an element in the tree.

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>
operator--(TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>& iter, int);
    // Move the specified 'iter' to the previous element in the tree and return
    // the value of 'iter' prior to this call.  The behavior is undefined
    // unless the iterator refers to the past-the-end or the non-leftmost
    // element in the tree.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // ------------------
                     // class TreeIterator
                     // ------------------

// CREATORS
template <class VALUE, class NODE, class DIFFERENCE_TYPE>
inline
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>::TreeIterator()
: d_node_p(0)
{
}

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
inline
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>::
TreeIterator(const bslalg::RbTreeNode *node)
: d_node_p(const_cast<bslalg::RbTreeNode *>(node))
{
}

// MANIPULATORS
template <class VALUE, class NODE, class DIFFERENCE_TYPE>
inline
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>&
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>::operator++()
{
    d_node_p = bslalg::RbTreeUtil::next(d_node_p);
    return *this;
}

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
inline
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>&
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>::operator--()
{
    d_node_p = bslalg::RbTreeUtil::previous(d_node_p);
    return *this;
}

// ACCESSORS
template <class VALUE, class NODE, class DIFFERENCE_TYPE>
inline
typename TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>::reference
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>::operator*() const
{
    BSLS_ASSERT_SAFE(d_node_p);

    return static_cast<NODE *>(d_node_p)->value();
}

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
inline
typename TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>::pointer
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>::operator->() const
{
    BSLS_ASSERT_SAFE(d_node_p);

    return bsls::Util::addressOf(static_cast<NODE *>(d_node_p)->value());
}

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
inline
const bslalg::RbTreeNode*
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>::node() const
{
    return d_node_p;
}

// FREE OPERATORS
template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
inline
bool operator==(const TreeIterator<VALUE1, NODEPTR, DIFF>& lhs,
                const TreeIterator<VALUE2, NODEPTR, DIFF>& rhs)
{
    return lhs.d_node_p == rhs.d_node_p;
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
inline
bool operator!=(const TreeIterator<VALUE1, NODEPTR, DIFF>& lhs,
                const TreeIterator<VALUE2, NODEPTR, DIFF>& rhs)
{
    return lhs.d_node_p != rhs.d_node_p;
}
#endif

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
inline
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>
operator++(TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>& iter, int)
{
    TreeIterator<VALUE, NODE, DIFFERENCE_TYPE> temp = iter;
    ++iter;
    return temp;
}

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
inline
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>
operator--(TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>& iter, int)
{
    TreeIterator<VALUE, NODE, DIFFERENCE_TYPE> temp = iter;
    --iter;
    return temp;
}

}  // close package namespace
}  // close enterprise namespace

#ifndef BSLS_PLATFORM_CMP_SUN
# ifndef BSLMF_ISTRIVIALLYCOPYABLE_NATIVE_IMPLEMENTATION
namespace bsl {

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
struct is_trivially_copyable<
              BloombergLP::bslstl::TreeIterator<VALUE, NODE, DIFFERENCE_TYPE> >
    : bsl::true_type {
};

}  // close namespace bsl
# endif // BSLMF_ISTRIVIALLYCOPYABLE_NATIVE_IMPLEMENTATION
#endif  // BSLS_PLATFORM_CMP_SUN

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
