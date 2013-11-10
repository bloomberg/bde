// bslstl_treeiterator.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_TREEITERATOR
#define INCLUDED_BSLSTL_TREEITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
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

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>
#endif

#ifndef INCLUDED_BSLALG_RBTREENODE
#include <bslalg_rbtreenode.h>
#endif

#ifndef INCLUDED_BSLALG_RBTREEUTIL
#include <bslalg_rbtreeutil.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDE_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

namespace BloombergLP {
namespace bslstl {

                     // ==================
                     // class TreeIterator
                     // ==================

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
class TreeIterator
#ifdef BSLS_PLATFORM_OS_SOLARIS
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
    typedef typename bsl::remove_cv<VALUE>::type          NcType;
    typedef TreeIterator<NcType, NODE, DIFFERENCE_TYPE> NcIter;

    // DATA
    bslalg::RbTreeNode *d_node_p;  // current iterator position

  private:
    // FRIENDS
    template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
    friend bool operator==(const TreeIterator<VALUE1, NODEPTR, DIFF>&,
                           const TreeIterator<VALUE2, NODEPTR, DIFF>&);
    template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
    friend bool operator!=(const TreeIterator<VALUE1, NODEPTR, DIFF>&,
                           const TreeIterator<VALUE2, NODEPTR, DIFF>&);

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
        // undefined unless 'node' is of the parameterized 'NODE', which
        // is derived from 'bslalg::RbTreeNode.  Note that this constructor is
        // an implementation detail and is not part of the C++ standard.

    TreeIterator(const NcIter& original);
        // Create an iterator at the same position as the specified 'original'
        // iterator.  Note that this constructor enables converting from
        // modifiable to const iterator types.

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
    // have the same value and 'false' otherwise.  Two iterators have
    // the same value if they refer to the same position in the same tree, or
    // if both iterators are at an invalid position in the tree (i.e., the
    // 'end' of the tree, or the default constructed value).

template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
bool operator!=(const TreeIterator<VALUE1, NODEPTR, DIFF>& lhs,
                const TreeIterator<VALUE2, NODEPTR, DIFF>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // do not have the same value and 'false' otherwise.  Two iterators do not
    // have the same value if they differ in either the tree to which they
    // refer or the position in that tree.


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

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

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

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
inline
TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>::
TreeIterator(const NcIter& original)
: d_node_p(original.d_node_p)
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

    return BSLS_UTIL_ADDRESSOF(static_cast<NODE *>(d_node_p)->value());
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

template <class VALUE1, class VALUE2, class NODEPTR, class DIFF>
inline
bool operator!=(const TreeIterator<VALUE1, NODEPTR, DIFF>& lhs,
                const TreeIterator<VALUE2, NODEPTR, DIFF>& rhs)
{
    return lhs.d_node_p != rhs.d_node_p;
}

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

}  // close namespace bslstl
}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
