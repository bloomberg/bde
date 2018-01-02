// bslalg_rbtreenode.h                                                -*-C++-*-
#ifndef INCLUDED_BSLALG_RBTREENODE
#define INCLUDED_BSLALG_RBTREENODE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a base class for a red-black binary tree node.
//
//@CLASSES:
//  bslalg::RbTreeNode: base class for a red-black binary tree node
//
//@SEE_ALSO: bslalg_rbtreeutil
//
//@DESCRIPTION: This component provides a single POD-like class, 'RbTreeNode',
// used to represent a node in a red-black binary search tree.  An 'RbTreeNode'
// provides the address to its parent, left-child, and right-child nodes, as
// well as providing a "color" (red or black).  'RbTreeNode' does not, however,
// contain "payload" data (e.g., a value), as it is intended to work with
// generalized tree operations (see 'bslalg_rbtreenodeutil').  Clients creating
// a red-black binary search tree must define their own node type that
// incorporates 'RbTreeNode' (generally via inheritance), and that maintains
// the "key" value and any associated data.
//
///Storing Color Information
///-------------------------
// To reduce the memory footprint of the 'RbTreeNode', the color information is
// stored at the least-significant-bit (LSB) of the parent node.  The address
// of the parent node and the color can be accessed through bit-wise
// operations.  This is possible because all memory addresses are at least
// 4-bytes aligned, therefore, the 2 LSB of any pointer are always 0.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating a Function to Print a Red Black Tree
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates creating a function that prints, to a 'FILE', a
// tree of 'RbTreeNode' objects.
//
// First, we define the signature of a function, 'printTree', that accepts, in
// addition to an output file and root node, a function pointer argument
// (supplied by clients) used to print each node's value, note that a node's
// value is not accessible through 'RbTreeNode':
//..
//  void printTree(FILE             *output,
//                 const RbTreeNode *rootNode,
//                 void (*printNodeValueCallback)(FILE *, const RbTreeNode *))
//  {
//..
// Now, we define the body of 'printTree', which is a recursive function that
// performs a prefix traversal of the supplied binary tree, printing the value
// and color of 'rootNode' before recursively printing its left and then right
// sub-trees.
//..
//      if (0 == rootNode) {
//          return;                                                   // RETURN
//      }
//      fprintf(output, " [ ");
//
//      // Print the value and color of 'rootNode'.
//
//      printNodeValueCallback(output, rootNode);
//      fprintf(output,
//              ": %s",
//              rootNode->color() == RbTreeNode::BSLALG_RED ? "RED" : "BLACK");
//
//      // Recursively call 'printTree' on the left and right sub-trees.
//
//      printTree(output, rootNode->leftChild(), printNodeValueCallback);
//      printTree(output, rootNode->rightChild(), printNodeValueCallback);
//      fprintf(output, " ]");
//  }
//..
// Notice that we use 'FILE' in the context of this usage example to avoid a
// dependency of standard library streams.  Finally, we will use 'printTree' to
// print a description of a tree in the next example.
//
///Example 2: Creating a Simple Red-Black Tree
///- - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates creating a simple tree of integer values using
// 'RbTreeNode'.  Note that, in practice, clients should use associated
// utilities to manage such a tree (see 'bslalg_rbtreenodeutil').
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
// Then, we define a function 'printIntNodeValue' to print the value of an
// integer node.  Note that this function's signature matches that
// required by 'printTree' (defined in the preceding example):
//..
//  void printIntTreeNodeValue(FILE *output, const RbTreeNode *node)
//  {
//     BSLS_ASSERT(0 != node);
//
//     fprintf(output, "%d", static_cast<const IntTreeNode*>(node)->d_value);
//  }
//..
// Next, we define 'main' for our test, and create three nodes that we'll use
// to construct a tree:
//..
//  int main(int argc, const char *argv[])
//  {
//      IntTreeNode A, B, C;
//..
// Then, we describe the structure of the tree we wish to construct.
//..
//
//                A (value: 2, BLACK)
//              /       \.
//             /         \.
//  B (value: 1, RED)   C ( value: 3, RED )
//..
// Now, we set the properties for the nodes 'A', 'B', and 'C' to form a valid
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
// Finally, we use the 'printTree' function with the 'printIntTreeNodeValue'
// function to print the structure of our tree to 'stdout':
//..
//      printTree(stdout, &A, printIntTreeNodeValue);
//  }
//..
// Resulting in a single line of console output:
//..
//  [ 2: BLACK [ 1: RED ] [ 3: RED ] ]
//..
//
///Example 3: Creating a Function To Validate a Red-Black Tree
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates creating a function to validate the properties of
// a red-black tree.
//
// First, we declare the signature of a function 'validateRbTree', which takes
// two arguments: (1) the address to the root node of a tree, and (2) a
// comparator function, which is used to compare the payload values of the tree
// nodes.  Note that the parameterized comparator is needed because a node's
// value is not accessible through the supplied 'RbTreeNode'.
//..
//  template <class NODE_COMPARATOR>
//  int validateRbTree(const RbTreeNode       *rootNode,
//                     const NODE_COMPARATOR&  comparator);
//      // Return the uniform number of black nodes between every leaf node in
//      // the tree and the specified 'rootNode', 0 if 'rootNode' is 0, and a
//      // negative value if 'rootNode' does not refer to a valid red-black
//      // binary-search tree that is ordered according to the specified
//      // 'comparator'.  'rootNode' is considered a valid red-black binary
//      // search-tree if it obeys the following rules:
//      //
//      //: 1 All nodes in the left sub-tree of 'rootNode' are ordered at or
//      //:   before 'rootNode' (as determined by 'comparator'), and all nodes
//      //:   in the right sub-tree are ordered at or after 'rootNode'.
//      //:
//      //: 2 Both children of 'rootNode' refer to 'rootNode' as a parent.
//      //:
//      //: 3 If 'rootNode' is red, its children are either black or 0.
//      //:
//      //: 4 Every path from 'rootNode' to a leaf contains the same number of
//      //:   black nodes (the uniform number of black nodes in every path is
//      //:   returned by this function if valid).
//      //:
//      //: 5 Rules (1-4) are obeyed, recursively, by the left and right
//      //:   sub-trees of 'rootNode'.
//      //
//      // Note that this particular specification of the constraints of a
//      // red-black tree does not require the presense of black-colored NIL
//      // leaf-nodes; instead NULL children are implicitly assumed to be NIL
//      // leaf-nodes (as is typically the case for C/C++ implementations).
//      // This specification also does not require the root node to be
//      // colored black, as there's no practical benefit to enforcing that
//      // constraint.
//..
// Then, we declare the signature for an auxiliary function,
// 'validateRbTreeRaw', that accepts, additionally, the address of minimum
// and maximum value nodes, and is needed to recursively apply rule 1:
//..
//  template <class NODE_COMPARATOR>
//  int validateRbTreeRaw(const RbTreeNode *rootNode,
//                        const RbTreeNode *minNodeValue,
//                        const RbTreeNode *maxNodeValue,
//                        NODE_COMPARATOR   comparator);
//
//      // Return the uniform number of black nodes between every leaf node in
//      // the tree and the specified 'rootNode', 0 if 'rootNode' is 0, and a
//      // negative value if (1) 'rootNode' does not refer to a valid red-black
//      // binary search tree that is ordered according to the specified
//      // 'comparator', (2) the specified 'minNodeValue' is not 0 and there is
//      // at least 1 node in the tree ordered before 'minNodeValue', or (3)
//      // the specified 'maxNodeValue' is not 0 and there is at least 1 node
//      // in the tree ordered after 'maxNodeValue'.
//..
// Now, we define the implementation of 'validateRbTree', which simply
// delegates to 'validateRbTreeRaw'.
//..
//  template <class NODE_COMPARATOR>
//  int validateRbTree(const RbTreeNode *rootNode,
//                     NODE_COMPARATOR   comparator)
//  {
//      return validateRbTreeRaw(rootNode, 0, 0, comparator);
//  }
//..
// Finally, we define the implementation of 'validateRbTreeRaw', which tests if
// 'rootNode' violates any of the rules defined in the 'validateRbTree' method
// documentation, and then recursively calls 'validateRbTreeRaw' on the left
// and right sub-trees or 'rootNode':
//..
//  template <class NODE_COMPARATOR>
//  int validateRbTreeRaw(const RbTreeNode *rootNode,
//                        const RbTreeNode *minNodeValue,
//                        const RbTreeNode *maxNodeValue,
//                        NODE_COMPARATOR   comparator)
//  {
//      enum { INVALID_RBTREE = -1 };
//
//      // The black-height of a empty tree is considered 0.
//
//      if (0 == rootNode) {
//          return 0;                                                 // RETURN
//      }
//
//      // Rule 1.
//
//      if ((minNodeValue && comparator(*rootNode, *minNodeValue)) ||
//          (maxNodeValue && comparator(*maxNodeValue, *rootNode))) {
//          return INVALID_RBTREE;                                    // RETURN
//      }
//
//      // Rule 2.
//
//      const RbTreeNode *left  = rootNode->leftChild();
//      const RbTreeNode *right = rootNode->rightChild();
//      if ((left  && left->parent()  != rootNode) ||
//          (right && right->parent() != rootNode)) {
//          return INVALID_RBTREE;                                    // RETURN
//      }
//
//      // Rule 3.
//
//      if (RbTreeNode::BSLALG_RED == rootNode->color()) {
//          if ((left  && left->color()  != RbTreeNode::BSLALG_BLACK) ||
//              (right && right->color() != RbTreeNode::BSLALG_BLACK)) {
//              return INVALID_RBTREE;                                // RETURN
//          }
//      }
//
//      // Recursively validate the left and right sub-tree's and obtain their
//      // black-height in order to apply rule 5.
//
//      int leftDepth  = validateRbTreeRaw(rootNode->leftChild(),
//                                         minNodeValue,
//                                         rootNode,
//                                         comparator);
//
//      int rightDepth = validateRbTreeRaw(rootNode->rightChild(),
//                                         rootNode,
//                                         maxNodeValue,
//                                         comparator);
//
//      if (leftDepth < 0 || rightDepth < 0) {
//          return INVALID_RBTREE;                                    // RETURN
//      }
//
//      // Rule 4.
//
//      if (leftDepth != rightDepth) {
//          return INVALID_RBTREE;                                    // RETURN
//      }
//
//      return (rootNode->color() == RbTreeNode::BSLALG_BLACK)
//          ? leftDepth + 1
//          : leftDepth;
//  }
//..


#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bslalg {

                        // ================
                        // class RbTreeNode
                        // ================

class RbTreeNode {
    // This POD-like 'class' describes a node suitable for use in a red-black
    // binary search tree, holding the addresses of the parent, left-child, and
    // right-child nodes (any of which may be 0), as well as a "color" (red or
    // black).  This class is a "POD-like" to facilitate efficient allocation
    // and use in the context of a container implementation.  In order to meet
    // the essential requirements of a POD type, this 'class' does not define a
    // constructor or destructor.  However its data members are private.  Since
    // this class will be aligned to a word boundary, a pointer type will be a
    // multiple of 4.  This class use this property to reduce its size by
    // storing the color information in the least significant bit of the parent
    // pointer.  Note that this type does not contain any "payload" member
    // data: Clients creating a red-black binary search tree must define an
    // appropriate node type that incorporates 'RbTreeNode' (generally via
    // inheritance), and that holds the "key" value and any associated data.

  public:
    // TYPES
    enum Color{
        BSLALG_RED   = 0,
        BSLALG_BLACK = 1
    };

  private:
    // DATA
    RbTreeNode *d_parentWithColor_p;  // parent of this node (may be 0) with
                                      // the color information stored in the
                                      // least significant bit

    RbTreeNode *d_left_p;             // left-child of this node (may be 0)

    RbTreeNode *d_right_p;            // right-child of this node (may be 0)


  private:
    // PRIVATE CLASS METHODS
    static bsls::Types::UintPtr toInt(RbTreeNode *value);
        // Return the specified 'value' as an 'unsigned int'.

    static RbTreeNode *toNode(bsls::Types::UintPtr value);
        // Return the specified 'value' as 'RbTreeNode *'.

  public:
    //! RbTreeNode() = default;
        // Create a 'RbTreeNode' object having uninitialized values.

    //! RbTreeNode(const RbTreeNode& original) = default;
        // Create a 'RbTreeNode' object having the same value as the specified
        // 'original' object.

    //! ~RbTreeNode() = default;
        // Destroy this object.

    // MANIPULATORS
    //! RbTreeNode& operator= (const RbTreeNode& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void makeBlack();
        // Set the color of this node to black.  Note that this operation is
        // at least as fast as (and potentially faster than) 'setColor'.

    void makeRed();
        // Set the color of this node to red.    Note that this operation is
        // at least as fast as (and potentially faster than) 'setColor'.

    void setParent(RbTreeNode *address);
        // Set the parent of this node to the specified 'address'.  If
        // 'address' is 0, then this node will have not have a parent node
        // (i.e., it will be the root node).  The behavior is undefined unless
        // 'address' is aligned to at least two bytes.

    void setLeftChild(RbTreeNode *address);
        // Set the left child of this node to the specified 'address'.  If
        // 'address' is 0, then this node will not have a left child.

    void setRightChild(RbTreeNode *address);
        // Set the right child of this node to the specified 'address'.  If
        // 'address' is 0, then this node will not have a right child.

    void setColor(Color value);
        // Set the color of this node to the specified 'value'.

    void toggleColor();
        // Set the color of this node to the alternative color.  If this
        // node's color is red, set it to black, and set it to red otherwise.
        // Note that this operation is at least as fast as (and potentially
        // faster than) 'setColor'.

    void reset(RbTreeNode *parent,
               RbTreeNode *leftChild,
               RbTreeNode *rightChild,
               Color       color);
        // Reset this object to have the specified 'parent', 'leftChild',
        // 'rightChild', and 'color' property values.

    RbTreeNode *parent();
        // Return the address of the (modifiable) parent of this node if one
        // exists, and 0 otherwise.

    RbTreeNode *leftChild();
        // Return the address of the (modifiable) left child of this node if
        // one exists, and 0 otherwise.

    RbTreeNode *rightChild();
        // Return the address of the (modifiable) right child of this node if
        // one exists, and 0 otherwise.

    // ACCESSORS
    const RbTreeNode *parent() const;
        // Return the address of the parent of this node if one exists, and 0
        // otherwise.

    bool isBlack() const;
        // Return 'true' if this node is black.

    bool isRed() const;
        // Return 'true' if this node is red.

    const RbTreeNode *leftChild() const;
        // Return the address of the left child of this node if one exists,
        // and 0 otherwise.

    const RbTreeNode *rightChild() const;
        // Return the address of the right child of this node if one exists,
        // and 0 otherwise.

    Color color() const;
        // Return the color of this node.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// PRIVATE METHODS
inline
bsls::Types::UintPtr RbTreeNode::toInt(RbTreeNode *value)
{
    return reinterpret_cast<bsls::Types::UintPtr>(value);
}

inline
RbTreeNode *RbTreeNode::toNode(bsls::Types::UintPtr value)
{
    return reinterpret_cast<RbTreeNode *>(value);
}

// MANIPULATORS
inline
void RbTreeNode::makeBlack()
{
    d_parentWithColor_p = toNode(toInt(d_parentWithColor_p) | 0x01);
}

inline
void RbTreeNode::makeRed()
{
    d_parentWithColor_p = toNode(toInt(d_parentWithColor_p) & ~0x01);
}

inline
void RbTreeNode::setParent(RbTreeNode *address)
{
    BSLS_ASSERT_SAFE(0 == (toInt(address) & 0x01));

    d_parentWithColor_p =
                  toNode(toInt(address) | (toInt(d_parentWithColor_p) & 0x01));
}

inline
void RbTreeNode::setLeftChild(RbTreeNode *address)
{
    d_left_p = address;
}

inline
void RbTreeNode::setRightChild(RbTreeNode *address)
{
    d_right_p = address;
}

inline
void RbTreeNode::setColor(Color value)
{
    d_parentWithColor_p = toNode((toInt(d_parentWithColor_p) & ~0x01) | value);
}

inline
void RbTreeNode::toggleColor()
{
    BSLMF_ASSERT(0 == BSLALG_RED);
    BSLMF_ASSERT(1 == BSLALG_BLACK);

    d_parentWithColor_p = toNode(toInt(d_parentWithColor_p) ^ 0x01);
}

inline
void RbTreeNode::reset(RbTreeNode *parent,
                       RbTreeNode *leftChild,
                       RbTreeNode *rightChild,
                       Color       color)
{
    BSLS_ASSERT_SAFE(0 == (toInt(parent) & 0x01));

    d_parentWithColor_p = toNode(toInt(parent) | color);
    d_left_p = leftChild;
    d_right_p = rightChild;
}

inline
RbTreeNode *RbTreeNode::parent()
{
    return toNode(toInt(d_parentWithColor_p) & ~0x01);
}

inline
RbTreeNode *RbTreeNode::leftChild()
{
    return d_left_p;
}

inline
RbTreeNode *RbTreeNode::rightChild()
{
    return d_right_p;
}

// ACCESSORS
inline
const RbTreeNode *RbTreeNode::parent() const
{
    return toNode(toInt(d_parentWithColor_p) & ~0x01);
}

inline
bool RbTreeNode::isBlack() const
{
    return toInt(d_parentWithColor_p) & 0x01;
}

inline
bool RbTreeNode::isRed() const
{
    return !isBlack();
}

inline
const RbTreeNode *RbTreeNode::leftChild() const
{
    return d_left_p;
}

inline
const RbTreeNode *RbTreeNode::rightChild() const
{
    return d_right_p;
}

inline
RbTreeNode::Color RbTreeNode::color() const
{
    return static_cast<Color>(toInt(d_parentWithColor_p) & 0x01);
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
