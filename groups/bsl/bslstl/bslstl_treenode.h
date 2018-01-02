// bslstl_treenode.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLSTL_TREENODE
#define INCLUDED_BSLSTL_TREENODE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a POD-like tree node type holding a parameterized value.
//
//@CLASSES:
//   bslstl::TreeNode: a tree node holding a parameterized value
//
//@SEE_ALSO: bslstl_treenodefactory, bslstl_set, bslstl_map
//
//@DESCRIPTION: This component provides a single POD-like class, 'TreeNode',
// used to represent a node in a red-black binary search tree holding a value
// of a parameterized type.  A 'TreeNode' inherits from 'bslalg::RbTreeNode',
// so it may be used with 'bslalg::RbTreeUtil' functions, and adds an attribute
// 'value' of the parameterized 'VALUE'.  The following inheritance hierarchy
// diagram shows the classes involved and their methods:
//..
//    ,----------------.
//   ( bslstl::TreeNode )
//    `----------------'
//             |       value
//             V
//   ,------------------.
//  ( bslalg::RbTreeNode )
//   `------------------'
//                   ctor
//                   dtor
//                   makeBlack
//                   makeRed
//                   setParent
//                   setLeftChild
//                   setRightChild
//                   setColor
//                   toggleColor
//                   parent
//                   leftChild
//                   rightChild
//                   isBlack
//                   isRed
//                   color
//..
// This class is "POD-like" to facilitate efficient allocation and use in the
// context of a container implementation.  In order to meet the essential
// requirements of a POD type, both this 'class' and 'bslalg::RbTreeNode' do
// not define a constructor or destructor.  The manipulator, 'value', returns a
// modifiable reference to the object that may be constructed in-place by the
// appropriate 'bsl::allocator_traits' object.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Allocating and Deallocating 'TreeNode' Objects.
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we define a factory class for allocating and
// destroying 'TreeNode' objects.
//
// First, we define the interface for the class 'NodeFactory':
//..
//  template <class VALUE, class ALLOCATOR>
//  class NodeFactory {
//..
// The parameterized 'ALLOCATOR' is intended to allocate objects of the
// parameterized 'VALUE', so to use it to allocate objects of 'TreeNode<VALUE>'
// we must rebind it to the tree node type.  Note that in general, we use
// 'allocator_traits' to perform actions using an allocator (including the
// rebind below):
//..
//      // PRIVATE TYPES
//      typedef typename bsl::allocator_traits<ALLOCATOR>::template
//                             rebind_traits<TreeNode<VALUE> > AllocatorTraits;
//      typedef typename AllocatorTraits::allocator_type       NodeAllocator;
//
//      // DATA
//      NodeAllocator d_allocator;  // rebound tree-node allocator
//
//      // NOT IMPLEMENTED
//      NodeFactory(const NodeFactory&);
//      NodeFactory& operator=(const NodeFactory&);
//
//    public:
//      // CREATORS
//      NodeFactory(const ALLOCATOR& allocator);
//          // Create a tree node-factory that will use the specified
//          // 'allocator' to supply memory.
//
//      // MANIPULATORS
//      TreeNode<VALUE> *createNode(const VALUE& value);
//          // Create a new 'TreeNode' object holding the specified 'value'.
//
//      void deleteNode(bslalg::RbTreeNode *node);
//          // Destroy and deallocate the specified 'node'.  The behavior is
//          // undefined unless 'node' is the address of a
//          // 'TreeNode<VALUE>' object.
//  };
//..
// Now, we implement the 'NodeFactory' type:
//..
//  template <class VALUE, class ALLOCATOR>
//  inline
//  NodeFactory<VALUE, ALLOCATOR>::NodeFactory(const ALLOCATOR& allocator)
//  : d_allocator(allocator)
//  {
//  }
//..
// We implement the 'createNode' function by using the rebound
// 'allocator_traits' for our allocator to in-place copy-construct the
// supplied 'value' into the 'value' data member of our 'result' node
// object.  Note that 'TreeNode' is a POD-like type, without a constructor, so
// we do not need to call its constructor here:
//..
//  template <class VALUE, class ALLOCATOR>
//  inline
//  TreeNode<VALUE> *
//  NodeFactory<VALUE, ALLOCATOR>::createNode(const VALUE& value)
//  {
//      TreeNode<VALUE> *result = AllocatorTraits::allocate(d_allocator, 1);
//      AllocatorTraits::construct(d_allocator,
//                                 bsls::Util::addressOf(result->value()),
//                                 value);
//      return result;
//  }
//..
// Finally, we implement the function 'deleteNode'.  Again, we use the
// rebound 'allocator_traits' for our tree node type, this time to destroy the
// 'value' date member of node, and then to deallocate its footprint.  Note
// that 'TreeNode' is a POD-like type, so we do not need to call its destructor
// here:
//..
//  template <class VALUE, class ALLOCATOR>
//  inline
//  void NodeFactory<VALUE, ALLOCATOR>::deleteNode(bslalg::RbTreeNode *node)
//  {
//      TreeNode<VALUE> *treeNode = static_cast<TreeNode<VALUE> *>(node);
//      AllocatorTraits::destroy(d_allocator,
//                               bsls::Util::addressOf(treeNode->value()));
//      AllocatorTraits::deallocate(d_allocator, treeNode, 1);
//  }
//..
//
///Example 2: Creating a Simple Tree of 'TreeNode' Objects.
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we create a container-type 'Set' for
// holding a set of values of a parameterized 'VALUE'.
//
// First, we define a comparator for 'VALUE' of 'TreeNode<VALUE>' objects.
// This type is designed to be supplied to functions in 'bslalg::RbTreeUtil'.
// Note that, for simplicity, this type uses 'operator<' to compare values,
// rather than a client defined comparator type.
//..
//  template <class VALUE>
//  class Comparator {
//    public:
//      // CREATORS
//      Comparator() {}
//          // Create a node-value comparator.
//
//      // ACCESSORS
//      bool operator()(const VALUE&              lhs,
//                      const bslalg::RbTreeNode& rhs) const;
//      bool operator()(const bslalg::RbTreeNode& lhs,
//                      const VALUE&              rhs) const;
//          // Return 'true' if the specified 'lhs' is less than (ordered
//          // before) the specified 'rhs', and 'false' otherwise.  The
//          // behavior is undefined unless the supplied 'bslalg::RbTreeNode'
//          // object is of the derived 'TreeNode<VALUE>' type.
//  };
//..
// Then, we implement the comparison methods of 'Comparator'.  Note that the
// supplied 'RbTreeNode' objects must be 'static_cast' to
// 'TreeNode<VALUE>' to access their value:
//..
//  template <class VALUE>
//  inline
//  bool Comparator<VALUE>::operator()(const VALUE&              lhs,
//                                     const bslalg::RbTreeNode& rhs) const
//  {
//      return lhs < static_cast<const TreeNode<VALUE>& >(rhs).value();
//  }
//
//  template <class VALUE>
//  inline
//  bool Comparator<VALUE>::operator()(const bslalg::RbTreeNode& lhs,
//                                     const VALUE&              rhs) const
//  {
//      return static_cast<const TreeNode<VALUE>& >(lhs).value() < rhs;
//  }
//..
// Now, having defined the requisite helper types, we define the public
// interface for 'Set'.  Note that for the purposes of illustrating the use of
// 'TreeNode' a number of simplifications have been made.  For example, this
// implementation provides only 'insert', 'remove', 'isMember', and
// 'numMembers' operations:
//..
//  template <class VALUE,
//            class ALLOCATOR = bsl::allocator<VALUE> >
//  class Set {
//      // PRIVATE TYPES
//      typedef Comparator<VALUE>             ValueComparator;
//      typedef NodeFactory<VALUE, ALLOCATOR> Factory;
//
//      // DATA
//      bslalg::RbTreeAnchor d_tree;     // tree of node objects
//      Factory              d_factory;  // allocator for node objects
//
//      // NOT IMPLEMENTED
//      Set(const Set&);
//      Set& operator=(const Set&);
//
//    public:
//      // CREATORS
//      Set(const ALLOCATOR& allocator = ALLOCATOR());
//          // Create an empty set.  Optionally specify a 'allocator' used to
//          // supply memory.  If 'allocator' is not specified, a default
//          // constructed 'ALLOCATOR' object is used.
//
//      ~Set();
//          // Destroy this set.
//
//      // MANIPULATORS
//      void insert(const VALUE& value);
//          // Insert the specified value into this set.
//
//      bool remove(const VALUE& value);
//          // If 'value' is a member of this set, then remove it and return
//          // 'true', and return 'false' otherwise.
//
//      // ACCESSORS
//      bool isElement(const VALUE& value) const;
//          // Return 'true' if the specified 'value' is a member of this set,
//          // and 'false' otherwise.
//
//      int numElements() const;
//          // Return the number of elements in this set.
//  };
//..
// Now, we define the implementation of 'Set':
//..
//  // CREATORS
//  template <class VALUE, class ALLOCATOR>
//  inline
//  Set<VALUE, ALLOCATOR>::Set(const ALLOCATOR& allocator)
//  : d_tree()
//  , d_factory(allocator)
//  {
//  }
//
//  template <class VALUE, class ALLOCATOR>
//  inline
//  Set<VALUE, ALLOCATOR>::~Set()
//  {
//      bslalg::RbTreeUtil::deleteTree(&d_tree, &d_factory);
//  }
//
//  // MANIPULATORS
//  template <class VALUE, class ALLOCATOR>
//  void Set<VALUE, ALLOCATOR>::insert(const VALUE& value)
//  {
//      int comparisonResult;
//      ValueComparator comparator;
//      bslalg::RbTreeNode *parent =
//          bslalg::RbTreeUtil::findUniqueInsertLocation(&comparisonResult,
//                                                       &d_tree,
//                                                       comparator,
//                                                       value);
//      if (0 != comparisonResult) {
//          bslalg::RbTreeNode *node = d_factory.createNode(value);
//          bslalg::RbTreeUtil::insertAt(&d_tree,
//                                       parent,
//                                       comparisonResult < 0,
//                                       node);
//      }
//  }
//
//  template <class VALUE, class ALLOCATOR>
//  bool Set<VALUE, ALLOCATOR>::remove(const VALUE& value)
//  {
//      bslalg::RbTreeNode *node =
//                  bslalg::RbTreeUtil::find(d_tree, ValueComparator(), value);
//      if (node) {
//          bslalg::RbTreeUtil::remove(&d_tree, node);
//          d_factory.deleteNode(node);
//      }
//      return node;
//  }
//
//  // ACCESSORS
//  template <class VALUE, class ALLOCATOR>
//  inline
//  bool Set<VALUE, ALLOCATOR>::isElement(const VALUE& value) const
//  {
//      ValueComparator comparator;
//      return bslalg::RbTreeUtil::find(d_tree, comparator, value);
//  }
//
//  template <class VALUE, class ALLOCATOR>
//  inline
//  int Set<VALUE, ALLOCATOR>::numElements() const
//  {
//      return d_tree.numNodes();
//  }
//..
// Notice that the definition and implementation of 'Set' never directly
// uses the 'TreeNode' type, but instead use it indirectly through
// 'Comparator', and 'NodeFactory', and uses it via its base-class
// 'bslalg::RbTreeNode'.
//
// Finally, we test our 'Set'.
//..
//  Set<int> set;
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
//..

#ifndef INCLUDED_BSLALG_RBTREENODE
#include <bslalg_rbtreenode.h>
#endif

namespace BloombergLP {
namespace bslstl {

                        // ==============
                        // class TreeNode
                        // ==============

template <class VALUE>
class TreeNode : public bslalg::RbTreeNode {
    // This POD-like 'class' describes a node suitable for use in a red-black
    // binary search tree of values of the parameterized 'VALUE'.  This class
    // is a "POD-like" to facilitate efficient allocation and use in the
    // context of a container implementation.  In order to meet the essential
    // requirements of a POD type, this 'class' does not define a constructor
    // or destructor.  The manipulator, 'value', returns a modifiable reference
    // to 'd_value' so that it may be constructed in-place by the appropriate
    // 'bsl::allocator_traits' object.

    // DATA
    VALUE d_value;  // payload value

  private:
    // The following functions are not defined because a 'TreeNode' should
    // never be constructed, destructed, or assigned.  The 'd_value' member
    // should be separately constructed and destroyed using an appropriate
    // 'bsl::allocator_traits' object.

    TreeNode();                            // Declared but not defined
    TreeNode(const TreeNode&);             // Declared but not defined
    TreeNode& operator=(const TreeNode&);  // Declared but not defined
    ~TreeNode();                           // Declared but not defined

  public:
    // MANIPULATORS
    VALUE& value();
        // Return a reference providing modifiable access to the 'value' of
        // this object.

    // ACCESSORS
    const VALUE& value() const;
        // Return a reference providing non-modifiable access to the 'value' of
        // this object.
};

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

template <class VALUE>
inline
VALUE& TreeNode<VALUE>::value()
{
    return d_value;
}

template <class VALUE>
inline
const VALUE& TreeNode<VALUE>::value() const
{
    return d_value;
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
