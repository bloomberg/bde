// bslalg_rbtreeanchor.h                                              -*-C++-*-
#ifndef INCLUDED_BSLALG_RBTREEANCHOR
#define INCLUDED_BSLALG_RBTREEANCHOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Encapuslate addresses of the root, first, and last nodes of a tree.
//
//@CLASSES:
//  bslalg::RbTreeAnchor: (in-core) node-address attribute class
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@SEE_ALSO: bslalg_rbtreenode, bslalg_rbtreeprimitives
//
//@DESCRIPTION: This component provides a single, in-core, unconstrained
// (value-semantic) attribute class, 'RbTreeAnchor', that holds the addresses
// of the root, first, and last nodes of an (ordered) binary search tree.
//
///Attributes
///----------
//..
//  Name       Type          Default
//  ---------  ------------  -------
//  firstNode  RbTreeNode *  0
//  lastNode   RbTreeNode *  0
//  rootNode   RbTreeNode *  0
//
//: o 'firstNode': the first, or left-most, node of the tree
//:
//: o 'lastNode': the last, or right-most, node of the tree
//:
//: o 'rootNode': the root node of the tree
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating a Simple Tree
///- - - - - - - - - - - - - - - - -
// This example demonstrates creating a simple tree of integer values using
// 'RbTreeAnchor'.  Note that, in practice, clients should use associated
// utilities to manage such a tree (see 'bslalg_rbtreeprimitives').
//
// First, we define a node-type, 'IntTreeNode', that inherits from
//'RbTreeNode':
//..
//  struct IntTreeNode : public RbTreeNode {
//      // A red-black tree node containing an integer data-value.
//
//      int d_value;  // "payload" value represented by the node
//  };
//..
// Next, we define 'main' for our test, and create three nodes that we'll use
// to construct a tree:
//..
//    int main(int argc, const char *argv[])
//    {
//      IntTreeNode A, B, C;
//..
// Then we create an 'RbTreeAnchor', 'myTree', which will hold the addresses
// of the root, first, and last nodes of our tree, and the verify the
// attribute values of the default constructed object:
//..
//      RbTreeAnchor myTree;
//      assert(0 == myTree.rootNode());
//      assert(0 == myTree.firstNode());
//      assert(0 == myTree.lastNode());
//..
// Next, we describe the structure of the tree we wish to construct.
//..
//
//                A (value: 2, BLACK)
//              /       \
//             /         \
//  B (value: 1, RED)   C ( value: 5, RED )
//..
// Then, we set the properties for the nodes 'A', 'B', and 'C' to form a valid
// tree whose structure matches that description:
//..
//      A.d_value = 2;
//      A.setColor(RbTreeNode::BSLALG_BLACK);
//      A.setParent(0);
//      A.setLeftChild(&B);
//      A.setRightChild(&C);
//
//      B.d_value = 1;
//      B.setColor(RbTreeNode::BSLALG_RED);
//      B.setParent(&A);
//      B.setLeftChild(0);
//      B.setRightChild(0);
//
//      C.d_value = 3;
//      C.setColor(RbTreeNode::BSLALG_RED);
//      C.setParent(&A);
//      C.setLeftChild(0);
//      C.setRightChild(0);
//..
// Now, we assign the address of 'A', 'B', and 'C' as the root, first, and
// last nodes of 'myTree', respectively:
//..
//      myTree.reset(&A, &B, &C);
//..
// Finally, we verify the attributes of 'myTree':
//..
//      assert(&A == myTree.rootNode());
//      assert(&B == myTree.firstNode());
//      assert(&C == myTree.lastNode());
//..
//
///Example 2: Creating an Insert Function for a Binary Tree
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates creating a function that inserts elements into a
// binary search tree.  Note that, for simplicity, this function does *not*
// create a balanced red-black tree (see 'bslalg_rbtreeprimitives').
//
// First, we declare the signature of a function 'insertNode', which takes
// three arguments: (1) the anchor of the tree in which to insert the node (2)
// the new node to insert into the tree, and (3) a comparator function, which
// is used to compare the payload values of the tree nodes.  Note that the
// parameterized comparator is needed because a node's value is not accessible
// through the supplied 'RbTreeNode'.
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
//      // strict-weak ordering on the nodes in the tree.
//  {
//..
// Now, we implement the search node function, which finds a location where
// 'newNode' can be inserted into 'searchTree' without violating the ordering
// imposed by 'comparator', and then updates 'searchTree' with a potentially
// updated root, first, and last node.
//..
//      RbTreeNode *parent = 0;
//      RbTreeNode *node   = searchTree->rootNode();
//      bool        isLeftChild;
//
//      newNode->setLeftChild(0);
//      newNode->setRightChild(0);
//
//      if (!node) {
//..
// If the root node of 'searchTree' is 0, we use the 'reset' function set the
// root, first, and last nodes of 'searchTree' to 'newNode':
//..
//          searchTree->reset(newNode, newNode, newNode);
//          newNode->setParent(0);
//          return;                                                   // RETURN
//      }
//
//      // Find the leaf node that would be a valid parent of 'newNode'.
//
//      do {
//          parent = node;
//          isLeftChild = comparator(*newNode, *node);
//          if (isLeftChild) {
//              node = node->leftChild();
//          }
//          else {
//              node = node->rightChild();
//          }
//      } while (node);
//
//      // Insert 'newNode' into 'searchTree' and the location that's been
//      // found.
//
//..
// Finally, if 'newNode' is either the new first node or the new last node, we
// use 'setFirstNode' and 'setLastNode' to set the first and last nodes of
// 'searchTree', respectively:
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
//          // If 'newNode' is a right-child, it may be the new last node, but
//          // cannot be the new first node.
//
//          parent->setRightChild(newNode);
//          newNode->setParent(parent);
//          if (parent == searchTree->lastNode()) {
//              searchTree->setLastNode(newNode);
//          }
//      }
//  }
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

namespace BloombergLP {
namespace bslalg {

                        // ==================
                        // class RbTreeAnchor
                        // ==================

class RbTreeAnchor {
    // This in-core unconstrained attribute type that provides the addresses
    // of the first, last, and root nodes of a binary search tree.  See the
    // Attributes section under @DESCRIPTION in the component-level
    // documentation for information on the class attributes.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    RbTreeNode *d_root_p;   // root node of the tree
    RbTreeNode *d_first_p;  // first node of the tree
    RbTreeNode *d_last_p;   // last node of the tree

  public:

    // CREATORS
    RbTreeAnchor();
        // Create a 'RbTree' object having the (default) attribute values:
        //..
        //  rootNode()  == 0
        //  firstNode() == 0
        //  lastNode()  == 0
        //..

    RbTreeAnchor(RbTreeNode *rootNode,
                 RbTreeNode *firstNode,
                 RbTreeNode *lastNode);
        // Create a 'RbTreeAnchor' object having the specified 'rootNode',
        // 'firstNode', and 'lastNode' attribute values.

    RbTreeAnchor(const RbTreeAnchor& original);
        // Create a 'RbTreeAnchor' object having the same value as the
        // specified 'original' object.

//! ~RbTreeAnchor() = default;
        // Destroy this object.

    // MANIPULATORS
    RbTreeAnchor& operator=(const RbTreeAnchor& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void reset(RbTreeNode *rootNodeValue,
               RbTreeNode *firstNodeValue,
               RbTreeNode *lastNodeValue);
        // Set the 'rootNode', 'firstNode', and 'lastNode' attributes to the
        // specified 'rootNodeValue', 'firstNodeValue', and 'lastNodeValue',
        // respectively.

    void setFirstNode(RbTreeNode *value);
        // Set the 'firstNode' attribute of this object to the specified
        // 'value'.

    void setLastNode(RbTreeNode *value);
        // Set the 'lastNode' attribute of this object to the specified
        // 'value'.

    void setRootNode(RbTreeNode *value);
        // Set the 'rootNode' attribute of this object to the specified
        // 'value'.

    RbTreeNode *firstNode();
        // Return an address of the (modifiable) 'firstNode' attribute of
        // this object.

    RbTreeNode *lastNode();
        // Return an address of the (modifiable) 'lastNode' attribute of
        // this object.

    RbTreeNode *rootNode();
        // Return an address of the (modifiable) 'rootNode' attribute of
        // this object.

    // ACCESSORS
    const RbTreeNode *firstNode() const;
        // Return an address of the 'firstNode' attribute of this object.

    const RbTreeNode *lastNode() const;
        // Return an address of the 'lastNode' attribute of this object.

    const RbTreeNode *rootNode() const;
        // Return an address of the 'rootNode' attribute of  this object.
};

// FREE OPERATORS
bool operator==(const RbTreeAnchor& lhs, const RbTreeAnchor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'RbTreeAnchor' objects have the same
    // value if all of the corresponding values of their 'rootAnchor',
    // 'firstAnchor', and 'lastAnchor' attributes are the same.

bool operator!=(const RbTreeAnchor& lhs, const RbTreeAnchor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'RbTreeNode' objects do not
    // have the same value if any of the corresponding values of their
    // 'rootNode', 'firstNode', or 'lastNode' attributes are not the same.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------
                        // class RbTreeAnchor
                        // ------------------

// CREATORS
inline
RbTreeAnchor::RbTreeAnchor()
: d_root_p(0)
, d_first_p(0)
, d_last_p(0)
{
}

inline
RbTreeAnchor::RbTreeAnchor(RbTreeNode *rootNode,
                           RbTreeNode *firstNode,
                           RbTreeNode *lastNode)
: d_root_p(rootNode)
, d_first_p(firstNode)
, d_last_p(lastNode)
{
}

inline
RbTreeAnchor::RbTreeAnchor(const RbTreeAnchor& original)
: d_root_p(original.d_root_p)
, d_first_p(original.d_first_p)
, d_last_p(original.d_last_p)
{
}

// MANIPULATORS
inline
RbTreeAnchor& RbTreeAnchor::operator=(const RbTreeAnchor& rhs)
{
    d_root_p  = rhs.d_root_p;
    d_first_p = rhs.d_first_p;
    d_last_p  = rhs.d_last_p;
    return *this;
}

inline
void RbTreeAnchor::reset(RbTreeNode *rootNodeValue,
                         RbTreeNode *firstNodeValue,
                         RbTreeNode *lastNodeValue)
{
    d_root_p  = rootNodeValue;
    d_first_p = firstNodeValue;
    d_last_p  = lastNodeValue;
}

inline
void RbTreeAnchor::setFirstNode(RbTreeNode *value)
{
    d_first_p = value;
}

inline
void RbTreeAnchor::setLastNode(RbTreeNode *value)
{
    d_last_p = value;
}

inline
void RbTreeAnchor::setRootNode(RbTreeNode *value)
{
    d_root_p = value;
}
inline
RbTreeNode *RbTreeAnchor::firstNode()
{
    return d_first_p;
}

inline
RbTreeNode *RbTreeAnchor::lastNode()
{
    return d_last_p;
}

inline
RbTreeNode *RbTreeAnchor::rootNode()
{
    return d_root_p;
}

// ACCESSORS
inline
const RbTreeNode *RbTreeAnchor::firstNode() const
{
    return d_first_p;
}

inline
const RbTreeNode *RbTreeAnchor::lastNode() const
{
    return d_last_p;
}

inline
const RbTreeNode *RbTreeAnchor::rootNode() const
{
    return d_root_p;
}

}  // close namespace bslalg

// FREE OPERATORS
inline
bool bslalg::operator==(const bslalg::RbTreeAnchor& lhs,
                        const bslalg::RbTreeAnchor& rhs)
{
    return lhs.firstNode() == rhs.firstNode()
        && lhs.lastNode()  == rhs.lastNode()
        && lhs.rootNode()  == rhs.rootNode();
}

inline
bool bslalg::operator!=(const bslalg::RbTreeAnchor& lhs,
                        const bslalg::RbTreeAnchor& rhs)
{
    return lhs.firstNode() != rhs.firstNode()
        || lhs.lastNode()  != rhs.lastNode()
        || lhs.rootNode()  != rhs.rootNode();
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
