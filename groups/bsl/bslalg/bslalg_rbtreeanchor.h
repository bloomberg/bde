// bslalg_rbtreeanchor.h                                              -*-C++-*-
#ifndef INCLUDED_BSLALG_RBTREEANCHOR
#define INCLUDED_BSLALG_RBTREEANCHOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Encapsulate root, first, and last nodes of a tree with a count.
//
//@CLASSES:
//  bslalg::RbTreeAnchor: (in-core) node-addresses and node count
//
//@SEE_ALSO: bslalg_rbtreenode, bslalg_rbtreeutil
//
//@DESCRIPTION: This component defines a single class, 'RbTreeAnchor',
// providing access to the addresses of the root, first, and sentinel nodes of
// a tree, as well as the count of the number of nodes.  A sentinel node is a
// value-less node, owned by the 'RbTreeAnchor' for the tree, that is used as
// the end-point for iteration over the nodes in a tree.  'RbTreeAnchor'
// provides modifiers for the 'firstNode', 'rootNode', and 'numNodes'
// properties, however the sentinel node for a tree is located at a fixed
// address and cannot be modified.  An 'RbTreeAnchor' is similar to an in-core
// unconstrained attribute class, except that it does not supply
// equality-comparison, copy-construction, and copy-assignment operations.
//
///Sentinel Node
///-------------
// The sentinel node is an 'RbTreeNode' object that does not have a value, and
// provides a fixed end-point for navigation over the tree.  However, a
// sentinel node's attributes have different interpretations than those of
// other 'RbTreeNode' objects.  Specifically, a sentinel node's 'leftChild'
// refers to the root of the tree, and its 'rightChild' refers to the first
// node of the tree.  The following diagram shows the composition of a tree
// with 'RbTreeAnchor' and 'RbTreeNode':
//..
//                        .------------------------.
//                 .------|      RbTreeAnchor      |-------.
//                 |      |                        |       |
//       firstNode |      |    .--------------.    |       | rootNode
//                 |      |    |  RbTreeNode  |    |       |
//                 |      |    |  (sentinel)  |    |       |
//                 |      |    `--------------'    |       |
//                 |      |      /    ^      \     |       |
//                 |      `-----/-----|-------\----'       |
//                 V           /      |        \           V
//                  __________/       |         \__________
//                 |                  |                    |
//                 |                  |                    |
//        sentinel |             root |                    | sentinel
//   *right*-child |       parentNode |                    | *left*-child
//                 |                  |                    |
//                 |           .--------------.            |
//                 |           |  RbTreeNode  | <----------'
//                 |           |    (root)    |
//                 |           `--------------'
//                 |                /   \.
//                 |     .------------. .------------.
//                 |     | RbTreeNode | | RbTreeNode |
//                 |     `------------' `------------'
//                 |         /                \.
//                 |    .-----------------------------.
//                 |    |                             |
//                 |    |    [Tree of RbTreeNodes]    |
//                 |    |                             |
//                 |    |_____________________________|
//                 V   /                               \.
//        .------------.                                .------------.
//        | RbTreeNode |                                | RbTreeNode |
//        |   (first)  |                                |   (last)   |
//        `------------'                                `------------'
//..
// Notice that, counter-intuitively, the sentinel's right-child refers to the
// left-most (first) node of the tree.  Also notice that 'RbTreeAnchor'
// doesn't hold a direct reference to the last (i.e., the right-most) node of
// the tree.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating a Simple Tree
///- - - - - - - - - - - - - - - - -
// This example demonstrates creating a simple tree of integer values using
// 'RbTreeAnchor'.  Note that, in practice, clients should use associated
// utilities to manage such a tree (see 'bslalg_rbtreeutil').
//
// First, we define a node-type, 'IntTreeNode', that inherits from
// 'RbTreeNode':
//..
//  struct IntTreeNode : public RbTreeNode {
//      // A red-black tree node containing an integer data-value.
//
//      int d_value;  // "payload" value represented by the node
//  };
//..
// Then, we define 'main' for our example, and create three nodes that we'll
// use to construct a tree:
//..
//  int main(int argc, const char *argv[])
//  {
//      IntTreeNode A, B, C;
//..
// Next, we create an 'RbTreeAnchor', 'myTree', which will hold the addresses
// of the root node and the first node of our tree along with a count of nodes,
// and then verify the attribute values of the default constructed object:
//..
//      RbTreeAnchor myTree;
//      assert(0                 == myTree.rootNode());
//      assert(myTree.sentinel() == myTree.firstNode());
//      assert(0                 == myTree.numNodes());
//..
// Then, we describe the structure of the tree we wish to construct.
//..
//
//          A (value: 2, BLACK)
//              /       \.
//             /         \.
//  B (value: 1, RED)   C ( value: 5, RED )
//..
// Next, we set the properties for the nodes 'A', 'B', and 'C' to form a valid
// tree whose structure matches that description:
//..
//      A.d_value = 2;
//      A.makeBlack();
//      A.setParent(myTree.sentinel());
//      A.setLeftChild(&B);
//      A.setRightChild(&C);
//
//      B.d_value = 1;
//      B.makeRed();
//      B.setParent(&A);
//      B.setLeftChild(0);
//      B.setRightChild(0);
//
//      C.d_value = 3;
//      C.makeRed();
//      C.setParent(&A);
//      C.setLeftChild(0);
//      C.setRightChild(0);
//..
// Now, we assign the address of 'A' and 'B' as the root node and the first
// node of 'myTree' respectively and set the number of nodes to 3:
//..
//      myTree.reset(&A, &B, 3);
//..
// Finally, we verify the attributes of 'myTree':
//..
//      assert(&A == myTree.rootNode());
//      assert(&B == myTree.firstNode());
//      assert(3  == myTree.numNodes());
//..
//
///Example 2: Creating an Insert Function for a Binary Tree
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates creating a function that inserts elements into a
// binary search tree.  Note that, for simplicity, this function does *not*
// create a balanced red-black tree (see 'bslalg_rbtreeutil').
//
// First, we define a comparison functor for 'IntTreeNode' objects used by the
// insertion function:
//..
//  struct IntTreeNodeComparator {
//      // This class defines a comparator providing a comparison operation
//      // between two 'IntTreeNode' objects.
//
//      bool operator()(const RbTreeNode& lhs, const RbTreeNode& rhs)  const
//      {
//          return static_cast<const IntTreeNode&>(lhs).d_value <
//                 static_cast<const IntTreeNode&>(rhs).d_value;
//      }
//  };
//..
// Then, we declare the signature of a function 'insertNode', which takes
// three arguments: (1) the anchor of the tree in which to insert the node (2)
// the new node to insert into the tree, and (3) a comparator, which is used to
// compare the payload values of the tree nodes.  Note that the parameterized
// comparator is needed because a node's value is not accessible through the
// supplied 'RbTreeNode'.
//..
//  template <class NODE_COMPARATOR>
//  void insertNode(RbTreeAnchor           *searchTree,
//                  RbTreeNode             *newNode,
//                  const NODE_COMPARATOR&  comparator)
//      // Insert into the specified 'searchTree', ordered according to the
//      // specified 'comparator', the specified 'newNode'.  If there are
//      // multiple nodes having the same value as 'newNode', insert 'newNode'
//      // in the last position according to an infix traversal of the tree.
//      // The behavior is undefined unless the 'comparator' provides a
//      // strict weak ordering on the nodes in the tree.
//  {
//..
// Next, we find the location where 'newNode' can be inserted into 'searchTree'
// without violating the ordering imposed by 'comparator', and then updates
// 'searchTree' with a potentially updated root node and first node.
//..
//      RbTreeNode *parent = searchTree->sentinel();
//      RbTreeNode *node   = searchTree->rootNode();
//      bool        isLeftChild;
//
//      newNode->setLeftChild(0);
//      newNode->setRightChild(0);
//
//      if (!node) {
//..
// If the root node of 'searchTree' is 0, we use the 'reset' function set the
// root node and the first node of 'searchTree' to 'newNode' and set the number
// of nodes to 1:
//..
//          searchTree->reset(newNode, newNode, 1);
//          newNode->setParent(parent);
//          return;                                                   // RETURN
//      }
//
//      // Find the leaf node that would be a valid parent of 'newNode'.
//
//      do {
//          parent = node;
//          isLeftChild = comparator(*newNode, *node);
//          if (isLeftChild) {
//              node = parent->leftChild();
//          }
//          else {
//              node = parent->rightChild();
//          }
//      } while (node);
//
//      // Insert 'newNode' into 'searchTree' and the location that's been
//      // found.
//..
// Then, we insert 'newNode' into the appropriate position by setting it as a
// child of 'parent':
//..
//      if (isLeftChild) {
//          // If 'newNode' is a left-child, it may be the new first node, but
//          // cannot be the new last node.
//
//          parent->setLeftChild(newNode);
//          newNode->setParent(parent);
//          if (parent == searchTree->firstNode()) {
//              searchTree->setFirstNode(newNode);
//          }
//      }
//      else {
//          parent->setRightChild(newNode);
//          newNode->setParent(parent);
//      }
//..
// Next, we complete the insert function by incrementing the number of nodes in
// the tree:
//..
//      searchTree->incrementNumNodes();
//  }
//..
// Now, we create 5 'IntTreeNode' objects and insert them into a tree using the
// 'insertNode' function.
//..
//  IntTreeNode nodes[5];
//
//  nodes[0].d_value = 3;
//  nodes[1].d_value = 1;
//  nodes[2].d_value = 5;
//  nodes[3].d_value = 2;
//  nodes[4].d_value = 0;
//
//  IntTreeNodeComparator comparator;
//
//  RbTreeAnchor anchor;
//  for (int i = 0; i < 5; ++i) {
//      insertNode(&anchor, nodes + i, comparator);
//  }
//..
// Finally, we verify that the 'RbTreeAnchor' refers to the correct 'TreeNode'
// with its 'firstNode' and 'rootNode' attributes.
//..
//  assert(0 == static_cast<IntTreeNode *>(anchor.firstNode())->d_value);
//  assert(3 == static_cast<IntTreeNode *>(anchor.rootNode())->d_value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_RBTREENODE
#include <bslalg_rbtreenode.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bslalg {

                        // ==================
                        // class RbTreeAnchor
                        // ==================

class RbTreeAnchor {
    // An 'RbTreeAnchor' provides the addresses of the first and root nodes of
    // a binary search tree.  An 'RbTreeAnchor' is similar to an in-core
    // simply constrained (value-semantic) attribute class, except that it
    // does not supply equality-comparison, copy-construction, and
    // copy-assignment operations.  Note that a node may not be copied because
    // 'sentinel' returns an address unique to each 'RbTreeAnchor' object.
    //
    // This class:
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    RbTreeNode  d_sentinel;  // sentinel for the tree, holding the root,
                             // first and last tree nodes

    int         d_numNodes;  // number of nodes

  private:
    // NOT IMPLEMENTED
    RbTreeAnchor(const RbTreeAnchor&);
    RbTreeAnchor& operator=(const RbTreeAnchor&);

  public:
    // CREATORS
    RbTreeAnchor();
        // Create a 'RbTree' object having the (default) attribute values:
        //..
        //  rootNode()  == 0
        //  firstNode() == sentinel()
        //  numNodes()  == 0
        //..

    RbTreeAnchor(RbTreeNode *rootNode,
                 RbTreeNode *firstNode,
                 int         numNodes);
        // Create a 'RbTreeAnchor' object having the specified 'rootNode',
        // 'firstNode', and 'numNodes' attribute values.

    ~RbTreeAnchor();
        // Destroy this object.

    // MANIPULATORS
    void reset(RbTreeNode *rootNode,
               RbTreeNode *firstNode,
               int         numNodes);
        // Set the 'rootNode', 'firstNode', and 'numNodes'
        // attributes to the specified 'rootNodeValue', 'firstNodeValue',
        // and 'numNodes' respectively.

    void setFirstNode(RbTreeNode *value);
        // Set the 'firstNode' attribute of this object to the specified
        // 'value'.

    void setRootNode(RbTreeNode *value);
        // Set the 'rootNode' attribute of this object to the specified
        // 'value'.

    void setNumNodes(int value);
        // Set the 'numNodes' attribute of this object to the specified
        // 'value'.  The behavior is undefined unless '0 <= value'.

    void incrementNumNodes();
        // Increment, by 1, the 'numNodes' attribute of this object.  The
        // behavior is undefined unless 'numNodes <= INT_MAX - 1'.

    void decrementNumNodes();
        // Decrement, by 1, the 'numNodes' attribute of this object.  The
        // behavior is undefined unless '1 <= numNodes'.

    RbTreeNode *rootNode();
        // Return the address of the (modifiable) node referred to by the
        // 'rootNode' attribute of this object.

    RbTreeNode *firstNode();
        // Return the address of the (modifiable) node referred to by the
        // 'firstNode' attribute of this object.

    RbTreeNode *sentinel();
        // Return the address of the (modifiable) node referred to by the
        // 'sentinel' node for this tree.

    // ACCESSORS
    const RbTreeNode *firstNode() const;
        // Return the address referred to by the 'firstNode' attribute of this
        // object.

    const RbTreeNode *rootNode() const;
        // Return the address referred to by the 'rootNode' attribute of this
        // object.

    const RbTreeNode *sentinel() const;
        // Return the address referred to by the 'sentinel' node for this tree.

    int numNodes() const;
        // Return the 'numNodes' attribute of this object.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------
                        // class RbTreeAnchor
                        // ------------------

// CREATORS
inline
RbTreeAnchor::RbTreeAnchor()
: d_numNodes(0)
{
    d_sentinel.setRightChild(&d_sentinel);
    d_sentinel.setLeftChild(0);
}

inline
RbTreeAnchor::RbTreeAnchor(RbTreeNode *rootNode,
                           RbTreeNode *firstNode,
                           int         numNodes)
: d_numNodes(numNodes)
{
    d_sentinel.setRightChild(firstNode);
    d_sentinel.setLeftChild(rootNode);
}

inline
RbTreeAnchor::~RbTreeAnchor()
{
    BSLS_ASSERT_SAFE(sentinel()->leftChild() == rootNode());
    BSLS_ASSERT_SAFE(sentinel()->rightChild() == firstNode());
}

// MANIPULATORS
inline
void RbTreeAnchor::reset(RbTreeNode *rootNode,
                         RbTreeNode *firstNode,
                         int         numNodes)
{
    d_sentinel.setLeftChild(rootNode);
    d_sentinel.setRightChild(firstNode);
    d_numNodes = numNodes;
}

inline
void RbTreeAnchor::setFirstNode(RbTreeNode *value)
{
    d_sentinel.setRightChild(value);
}

inline
void RbTreeAnchor::setRootNode(RbTreeNode *value)
{
    d_sentinel.setLeftChild(value);
}

inline
void RbTreeAnchor::setNumNodes(int value)
{
    BSLS_ASSERT_SAFE(0 <= value);

    d_numNodes = value;
}

inline
void RbTreeAnchor::incrementNumNodes()
{
    ++d_numNodes;
}

inline
void RbTreeAnchor::decrementNumNodes()
{
    --d_numNodes;
}

inline
RbTreeNode *RbTreeAnchor::firstNode()
{
    return d_sentinel.rightChild();
}

inline
RbTreeNode *RbTreeAnchor::rootNode()
{
    return d_sentinel.leftChild();
}

inline
RbTreeNode *RbTreeAnchor::sentinel()
{
    return &d_sentinel;
}

// ACCESSORS
inline
const RbTreeNode *RbTreeAnchor::firstNode() const
{
    return d_sentinel.rightChild();
}

inline
const RbTreeNode *RbTreeAnchor::rootNode() const
{
    return d_sentinel.leftChild();
}

inline
int RbTreeAnchor::numNodes() const
{
    return d_numNodes;
}

inline
const RbTreeNode *RbTreeAnchor::sentinel() const
{
    return &d_sentinel;
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
