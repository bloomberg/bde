// bslalg_rbtreeutil.h                                                -*-C++-*-
#ifndef INCLUDED_BSLALG_RBTREEUTIL
#define INCLUDED_BSLALG_RBTREEUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a suite of primitive algorithms on red-black trees.
//
//@CLASSES:
//  bslalg::RbTreeUtil: namespace for red-black tree functions
//  bslalg::RbTreeUtilTreeProctor: proctor to manage all nodes in a tree
//
//@SEE_ALSO: bslalg_rbtreenode
//
//@DESCRIPTION: This component provides a variety of algorithms that operate
// on nodes forming a red-black binary search tree.
//
// This implementation is adapted from Cormen, Leiserson, Rivest,
// "Introduction to Algorithms" [MIT Press, 1997].
//
///Summary
///-------
// The following section provides a short synopsis describing observable
// behavior of functions supplied in this component.  See the full
// function-level contract for detailed description.
//
///Navigation
/// - - - - -
// The following algorithms search a tree for a value, or iterate over the
// nodes in a tree:
//..
//  leftmost            Return the leftmost node.
//
//  rightmost           Return the rightmost node.
//
//  next                Return the next node in an in-order traversal.
//
//  previous            Return the previous node in an in-order traversal.
//
//  find                Find the node with the supplied value.
//
//  lowerBound          Find the first node not less-than the supplied value.
//
//  upperBound          Find the first node greater than the supplied value.
//..
//
///Modification
/// - - - - - -
// The following algorithms are used in the process of manipulating the
// structure of a tree:
//..
//  copyTree            Return a deep-copy of the supplied tree.
//
//  deleteTree          Delete all the nodes of the supplied tree.
//
//  findInsertLocation  Find the location where a value may be inserted.
//
//  findUniqueInsertLocation
//                      Find the location where a unique value may be inserted.
//
//  insert              Insert the supplied node into the tree.
//
//  insertAt            Insert the supplied node at the indicated position.
//
//  remove              Remove the supplied node from the tree.
//
//  swap                Swap the contents of two trees.
//..
//
///Utility
///- - - -
// The following algorithms are typically used when implementing higher-level
// algorithms (and are not generally used by clients):
//..
//  isLeftChild         Return 'true' if the supplied node is a left child.
//
//  isRightChild        Return 'true' if the supplied node is a right child.
//
//  rotateLeft          Perform a counter-clockwise rotation on a node.
//
//  rotateRight         Perform a clockwise rotation on a node.
//..
//
///Testing
///- - - -
// The following algorithms are used for testing and debugging, and
// generally should not be used in production code:
//..
//  printTreeStructure  Print, to a file, the structure of the supplied tree.
//
//  validateRbTree      Indicate if a tree is a valid red-black tree.
//
//  isWellFormed        Indicate if the 'RbTreeAnchor' object is well-formed.
//..
//
///Well-Formed 'RbTreeAnchor' Objects
///----------------------------------
// Many of the algorithms defined in this component operate over a complete
// tree of nodes, rather than a (possible) subtree referred to through a
// pointer to a node.  These operations refer to a complete tree through a
// 'RbTreeAnchor' object, which maintains references to the first, root, and
// sentinel nodes for the tree, as well as a count of the number of nodes in
// the tree.  'RbTreeAnchor' objects supplied to 'RbTreeUtil' are frequently
// required to meet a series of constraints that are not enforced by the
// 'RbTreeAnchor' type itself.  An 'RbTreeAnchor' object meeting these
// constraints is said to be "well-formed", and 'RbTreeUtil::isWellFormed'
// will return 'true' for such an object.  A 'RbTreeAnchor' object is
// considered well-formed if all of the following are true:
//
//: 1 The root node refers to a valid red-black tree (see 'validateRbTree').
//:
//: 2 The first node refers to the leftmost node in the tree, or the sentinel
//:   node if the tree is empty.
//:
//: 3 The node count is the number of nodes in the tree (not counting the
//:   sentinel node).
//:
//: 4 The sentinel node refers to the root node as its left child, and the
//:   root node refers to the sentinel as its parent.
//:
//: 5 The root node is either 0 or is colored black.
//
// The manipulation functions of 'RbTreeUtil' guarantee that these properties
// are maintained for any supplied tree.  Note that 'RbTreeUtil::isWellFormed'
// has linear complexity with respect to the number of nodes in the tree, and
// is typically used for debugging and testing purposes only.  Note also that
// the final condition, that the root node be either 0 or colored black, is
// not a canonical requirement of a red-black tree but an additional invariant
// enforced by the methods of 'RbTreeUtil' to simplify the implementations.
//
///The Sentinel Node
///- - - - - - - - -
// The sentinel node is 'RbTreeNode' object (unique to an 'RbTreeAnchor'
// instance) which does not have a value, and provides a fixed end-point for
// navigation over the tree (which is distinct from the 'rightmost' node of
// that tree).  The sentinel node will be returned by 'next' if the supplied
// node is the rightmost node in the tree, as well as by search operations
// when no nodes meet the supplied search-criteria.  In addition, the sentinel
// node may be supplied as a 'hint' to 'findInsertLocation' and
// 'findUniqueInsertLocation', as well as supplied to 'previous' to obtain the
// rightmost node of a (non-empty) tree.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and Using a Tree with 'RbTreeUtil'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create a tree of integers using
// 'RbTreeUtil'.
//
// First, we define a type 'SimpleIntNode' that will represent a nodes in our
// tree of integer values.  'SimpleIntNode' contains an 'int' payload, and
// inherits from 'RbTreeNode', allowing it to be operated on by
// 'RbTreeUtil'.
//..
//  struct SimpleIntNode : public RbTreeNode {
//      int d_value;
//  };
//..
// Then, we define a comparison function for 'SimpleIntNode' objects (note
// that we static-cast 'RBTreeNode' objects to the actual node type,
// 'SimpleIntNode', for comparison purposes):
//..
//  struct SimpleIntNodeValueComparator {
//      // This class defines a comparator providing comparison operations
//      // between 'SimpleIntNode' objects, and 'int' values.
//
//      bool operator()(const RbTreeNode& lhs, int rhs) const
//      {
//          return static_cast<const SimpleIntNode&>(lhs).d_value < rhs;
//      }
//
//      bool operator()(int lhs, const RbTreeNode& rhs) const
//      {
//          return lhs < static_cast<const SimpleIntNode&>(rhs).d_value;
//      }
//  };
//
//..
// Next, we begin to define the example function that will build a tree of
// nodes holding integer values:
//..
//  void createTestTreeExample()
//  {
//..
// Then, within this function, we define a 'RbTreeAnchor' object that will
// hold the root, first, last, and sentinel nodes of tree, as well a count of
// the number of nodes in the tree:
//..
//      RbTreeAnchor tree;
//..
// Next, we define an array of 5 'SimpleIntNode' objects that we will insert
// into the tree; in practice, nodes are more often allocated on the heap (see
// example 2):
//..
//      const int NUM_NODES = 5;
//      SimpleIntNode nodes[NUM_NODES];
//..
// Then, we assign unique values to each of the 'nodes':
//..
//      for (int i = 0; i < NUM_NODES; ++i) {
//          nodes[i].d_value = i;
//      }
//..
// Now, for each node in the tree, we use 'RbTreeUtil' to first find the
// location at which the node should be inserted, and then insert that node
// into the tree:
//..
//      for (int i = 0; i < NUM_NODES; ++i) {
//          int comparisonResult;
//          SimpleIntNodeValueComparator comparator;
//          RbTreeNode *insertLocation = RbTreeUtil::findUniqueInsertLocation(
//                                              &comparisonResult,
//                                              &tree,
//                                              comparator,
//                                              nodes[i].d_value);
//          BSLS_ASSERT(comparisonResult);
//          RbTreeUtil::insertAt(&tree,
//                               insertLocation,
//                               comparisonResult < 0,
//                               &nodes[i]);
//      }
//..
// And verify the resulting 'tree' holds 5 nodes, and the first node has
// the value 0:
//..
//      assert(5 == tree.numNodes());
//      assert(0 == static_cast<SimpleIntNode *>(tree.firstNode())->d_value);
//..
// Finally, we use 'RbTreeUtil' to iterate through the nodes of 'tree', and
// write the value of each node to the console:
//..
//      const RbTreeNode *nodeIterator = tree.firstNode();
//      while (tree.sentinel() != nodeIterator) {
//          printf("Node value: %d\n",
//                 static_cast<const SimpleIntNode *>(nodeIterator)->d_value);
//          nodeIterator = RbTreeUtil::next(nodeIterator);
//      }
//  }
//..
// Notice that each of the 'RbTreeNode' objects must be 'static_cast' to the
// derived type, 'SimpleIntNode', in order to access their values.
//
// The resulting output is displayed on the console:
//..
//  Node value: 0
//  Node value: 1
//  Node value: 2
//  Node value: 3
//  Node value: 4
//..
//
///Example 2: Implementing a Set of Integers
///- - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to use 'RbTreeUtil' to implement a simple
// container holding a set of (unique) integer values as a red-black binary
// search tree.
//
// Before defining the 'IntSet' class, we need to define a series of
// associated helper types:
//: 1 The node-type, for the nodes in the tree.
//: 2 An iterator, for iterating over nodes in the tree.
//: 3 A comparison functor for comparing nodes and values.
//: 4 A factory for creating and destroying nodes.
//
// First, we define a type, 'IntSet_Node', that will represent the nodes in our
// tree of integer values; it contains an 'int' payload, and inherits from
// 'RbTreeNode', allowing it to be operated on by 'RbTreeUtil' (note that the
// underscore "_" indicates that this type is a private implementation type of
// 'IntSet', and not for use by clients of 'IntSet'):
//..
//  class IntSet_Node : public RbTreeNode {
//      // A red-black tree node containing an integer data-value.
//
//      // DATA
//      int d_value;  // actual value represented by the node
//
//    public:
//      // MANIPULATORS
//      int& value() { return d_value; }
//          // Return a reference providing modifiable access to the 'value' of
//          // this object.
//
//      // ACCESSORS
//      const int& value() const { return d_value; }
//          // Return a reference providing non-modifiable access to the
//          // 'value' of this object.
//  };
//..
// Then, we define a iterator over 'IntSet_Node' objects.  We use the 'next'
// function of 'RbTreeUtil' to increment the iterator (note that, for
// simplicity, this iterator is *not* a fully STL compliant iterator
// implementation):
//..
//  class IntSetConstIterator {
//      // This class defines an STL-style iterator over a non-modifiable tree
//      // of 'IntSet_Node' objects.
//
//      // DATA
//      const RbTreeNode *d_node_p;  // current location of this iterator
//
//    public:
//      IntSetConstIterator() : d_node_p(0) {}
//          // Create an iterator that does not refer to a node.
//
//      IntSetConstIterator(const RbTreeNode *node) : d_node_p(node) {}
//          // Create an iterator referring to the specified 'node'.
//
//  //  IntSetConstIterator(const IntSetConstIterator&) = default;
//
//      // MANIPULATOR
//  //  IntSetConstIterator& operator=(const IntSetConstIterator&) = default;
//
//..
// Here, we implement the prefix-increment operator using the 'next' function
// of 'RbTreeUtil:
//..
//      IntSetConstIterator& operator++()
//         // Advance this iterator to the subsequent value it the 'IntSet',
//         // and return a reference providing modifiable access to this
//         // iterator.   The behavior is undefined unless this iterator
//         // refers to a element in an 'IntSet'.
//      {
//          d_node_p = RbTreeUtil::next(d_node_p);
//          return *this;
//      }
//
//      // ACCESSORS
//      const int& operator*() const
//          // Return a reference providing non-modifiable access to the value
//          // referred to by this iterator.
//      {
//          return static_cast<const IntSet_Node *>(d_node_p)->value();
//      }
//
//      const int *operator->() const
//          // Return an address providing non-modifiable access to the value
//          // referred to by this iterator.
//      {
//          return &(static_cast<const IntSet_Node *>(d_node_p)->value());
//      }
//
//      const IntSet_Node *nodePtr() const
//          // Return the address of the non-modifiable int-set node referred
//          // to by this iterator
//      {
//          return static_cast<const IntSet_Node *>(d_node_p);
//      }
//  };
//
//  // FREE OPERATORS
//  bool operator==(const IntSetConstIterator &lhs,
//                  const IntSetConstIterator &rhs)
//      // Return 'true' if the 'lhs' and 'rhs' objects have the same value,
//      // and 'false' otherwise.  Two 'IntSetConstIterator' objects have the
//      // same value if they refer to the same node.
//  {
//      return lhs.nodePtr() == rhs.nodePtr();
//  }
//
//  bool operator!=(const IntSetConstIterator &lhs,
//                  const IntSetConstIterator &rhs)
//      // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
//      // the same value, and 'false' otherwise.  Two 'IntSetConstIterator'
//      // objects do not have the same value if they refer to different nodes.
//  {
//      return lhs.nodePtr() != rhs.nodePtr();
//  }
//..
// Next, we define a comparison functor for 'IntSet_Node' objects, which will
// be supplied to 'RbTreeUtil' functions that must compare nodes with values
// -- i.e., those with a 'NODE_VALUE_COMPARATOR' template parameter (e.g.,
// 'find' and 'findInsertLocation'):
//..
//  struct IntSet_NodeValueComparator {
//      // This class defines a comparator providing comparison operations
//      // between 'IntSet_Node' objects, and 'int' values.
//
//      bool operator()(const RbTreeNode& lhs, int rhs) const
//      {
//          return static_cast<const IntSet_Node&>(lhs).value() < rhs;
//      }
//
//      bool operator()(int lhs, const RbTreeNode& rhs) const
//      {
//          return lhs < static_cast<const IntSet_Node&>(rhs).value();
//      }
//  };
//..
// Notice that we static-cast 'RbTreeNode' objects to the actual node type,
// 'IntSet_Node' for comparison.
//
// Next, we define a factory for creating and destroying 'IntSet_Node'
// objects.  This factory provides the operations 'createNode' and
// 'deleteNode'.  These operations will be used directly by our container
// implementation, and they are also required by 'RbTreeUtil' functions taking
// a 'FACTORY' template parameter (e.g., 'copyTree' and 'deleteTree'):
//..
//  class IntSet_NodeFactory {
//      // This class defines a creator object, that when invoked, creates a
//      // new 'IntSet_Node' (either from a int value, or an existing
//      // 'IntSet_Node' object) using the allocator supplied at construction.
//
//      bslma::Allocator *d_allocator_p;  // allocator, (held, not owned)
//
//    public:
//
//      IntSet_NodeFactory(bslma::Allocator *allocator)
//      : d_allocator_p(allocator)
//      {
//          BSLS_ASSERT_SAFE(allocator);
//      }
//
//      RbTreeNode *createNode(int value) const
//      {
//          IntSet_Node *newNode = new (*d_allocator_p) IntSet_Node;
//          newNode->value() = value;
//          return newNode;
//      }
//
//      RbTreeNode *createNode(const RbTreeNode& node) const
//      {
//          IntSet_Node *newNode = new (*d_allocator_p) IntSet_Node;
//          newNode->value() = static_cast<const IntSet_Node&>(node).value();
//          return newNode;
//      }
//      void deleteNode(RbTreeNode *node) const
//      {
//          d_allocator_p->deleteObject(static_cast<IntSet_Node *>(node));
//      }
//
//      bslma::Allocator *allocator() const
//      {
//          return d_allocator_p;
//      }
//  };
//..
// Then, having defined the requisite helper types, we define the public
// interface for our 'IntSet' type.  Note that for the purposes of
// illustrating the use of 'RbTreeUtil' a number of simplifications have been
// made.  For example, this implementation provides only a minimal set of
// critical operations, and it does not use the empty base-class optimization
// for the comparator, etc.  We define the interface of 'IntSet' as follows:
//..
//  class IntSet {
//      // This class implements a set of (unique) 'int' values.
//
//      // DATA
//      RbTreeAnchor           d_tree;         // root, first, and last tree
//                                             // nodes
//
//      IntSet_NodeValueComparator
//                             d_comparator;   // comparison functor for ints
//
//      IntSet_NodeFactory     d_nodeFactory;  // factory for creating and
//                                             // destroying nodes
//
//      // FRIENDS
//      friend bool operator==(const IntSet& lhs, const IntSet& rhs);
//
//    public:
//      // PUBLIC TYPES
//      typedef IntSetConstIterator const_iterator;
//
//      // CREATORS
//      IntSet(bslma::Allocator *basicAllocator = 0);
//          // Create a empty 'IntSet'.  Optionally specify a 'basicAllocator'
//          // used to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      IntSet(const IntSet& original, bslma::Allocator *basicAllocator = 0);
//          // Create a 'IntSet' object having the same value as the specified
//          // 'original' object.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      ~IntSet();
//          // Destroy this object.
//
//      // MANIPULATORS
//      IntSet& operator=(const IntSet& rhs);
//          // Assign to this object the value of the specified 'rhs' object,
//          // and return a reference providing modifiable access to this
//          // object.
//
//      const_iterator insert(int value);
//          // If the specified 'value' is not already a member of this set,
//          // insert it into this set, returning an iterator referring to the
//          // newly added value, and return an iterator referring to the
//          // existing instance of 'value' in this set, with no other effect,
//          // otherwise.
//
//      const_iterator erase(const_iterator iterator);
//          // Remove the value referred to by the specified 'iterator' from
//          // this set, and return an iterator referring to the value
//          // subsequent to 'iterator' (prior to its removal).  The behavior
//          // is undefined unless 'iterator' refers to a valid value in this
//          // set.
//
//      void clear();
//          // Remove all the elements from this set.
//
//      void swap(IntSet& other);
//          // Efficiently exchange the value of this object with the value of
//          // the specified 'other' object.
//
//      // ACCESSORS
//      const_iterator begin() const;
//          // Return an iterator referring leftmost node value in this set, or
//          // 'end()' if this set is empty.
//
//      const_iterator end() const;
//          // Return an iterator referring to the value one past the
//          // rightmost value in this set.
//
//      const_iterator find(int value) const;
//          // Return a iterator referring to the specified 'value' in this
//          // set, or 'end()' if 'value' is not a member of this set.
//
//      int size() const;
//          // Return the number of elements in this set.
//  };
//
//  // FREE OPERATORS
//  bool operator==(const IntSet& lhs, const IntSet& rhs);
//      // Return 'true' if the 'lhs' and 'rhs' objects have the same value,
//      // and 'false' otherwise.  Two 'IntSet' objects have the same value if
//      // they contain the same number of elements, and if for each element
//      // in 'lhs' there is a corresponding element in 'rhs' with the same
//      // value.
//
//  bool operator!=(const IntSet& lhs, const IntSet& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
//      // the same value, and 'false' otherwise.  Two 'IntSet' objects do not
//      // have the same value if they differ in the number of elements they
//      // contain, or if for any element in 'lhs' there is not a
//      // corresponding element in 'rhs' with the same value.
//..
// Now, we implement the methods of 'IntSet' using 'RbTreeUtil' and the
// helper types we defined earlier:
//..
//  // CREATORS
//  IntSet::IntSet(bslma::Allocator *basicAllocator)
//  : d_tree()
//  , d_comparator()
//  , d_nodeFactory(bslma::Default::allocator(basicAllocator))
//  {
//  }
//
//  IntSet::IntSet(const IntSet& original, bslma::Allocator *basicAllocator)
//  : d_tree()
//  , d_comparator()
//  , d_nodeFactory(bslma::Default::allocator(basicAllocator))
//  {
//      if (original.d_tree.rootNode()) {
//          RbTreeUtil::copyTree(&d_tree, original.d_tree, &d_nodeFactory);
//      }
//  }
//
//  IntSet::~IntSet()
//  {
//      clear();
//  }
//
//  // MANIPULATORS
//  IntSet& IntSet::operator=(const IntSet& rhs)
//  {
//      IntSet temp(rhs, d_nodeFactory.allocator());
//      swap(temp);
//      return *this;
//  }
//
//..
// Here, we implement 'insert' by using the 'RbTreeUtil' algorithms
// 'findUniqueInsertLocation' and 'insertAt':
//..
//  IntSet::const_iterator IntSet::insert(int value)
//  {
//      // To insert a value into the tree, we first find the location where
//      // the node would be added, and whether 'value' is unique.  If 'value'
//      // is not unique we do not want to incur the expense of allocating
//      // memory for a node.
//
//      int comparisonResult;
//      RbTreeNode *insertLocation =
//                    RbTreeUtil::findUniqueInsertLocation(&comparisonResult,
//                                                         &d_tree,
//                                                         d_comparator,
//                                                         value);
//      if (0 == comparisonResult) {
//          // 'value' already exists in 'd_tree'.
//
//          return const_iterator(insertLocation);                    // RETURN
//      }
//
//      // If 'value' is unique, we create a new node and supply it to
//      // 'insertAt', along with the tree location returned by
//      // 'findUniqueInsertLocation'.
//
//      RbTreeNode *newNode = d_nodeFactory.createNode(value);
//      RbTreeUtil::insertAt(&d_tree,
//                           insertLocation,
//                           comparisonResult < 0,
//                           newNode);
//      return const_iterator(newNode);
//  }
//
//  IntSet::const_iterator IntSet::erase(const_iterator iterator)
//  {
//      BSLS_ASSERT(iterator.nodePtr());
//      IntSet_Node *node = const_cast<IntSet_Node *>(iterator.nodePtr());
//
//      // Before removing the node, we first find the subsequent node to which
//      // we will return an iterator.
//
//      RbTreeNode *next = RbTreeUtil::next(node);
//      RbTreeUtil::remove(&d_tree, node);
//      d_nodeFactory.deleteNode(node);
//      return const_iterator(next);
//  }
//
//  void IntSet::clear()
//  {
//      if (d_tree.rootNode()) {
//          RbTreeUtil::deleteTree(&d_tree, &d_nodeFactory);
//      }
//  }
//
//  void IntSet::swap(IntSet& other) {
//      BSLS_ASSERT(d_nodeFactory.allocator() ==
//                  other.d_nodeFactory.allocator());
//      RbTreeUtil::swap(&d_tree, &other.d_tree);
//  }
//
//  // ACCESSORS
//  IntSet::const_iterator IntSet::begin() const
//  {
//      return const_iterator(d_tree.firstNode());
//  }
//
//  IntSet::const_iterator IntSet::end() const
//  {
//      return const_iterator(d_tree.sentinel());
//  }
//
//  IntSet::const_iterator IntSet::find(int value) const
//  {
//      return const_iterator(RbTreeUtil::find(d_tree, d_comparator, value));
//  }
//
//  int IntSet::size() const
//  {
//      return d_tree.numNodes();
//  }
//..
// Finally, we implement the free operators on 'IntSet':
//..
//  // FREE OPERATORS
//  bool operator==(const IntSet& lhs, const IntSet& rhs)
//  {
//      return bslalg::RangeCompare::equal(lhs.begin(),
//                                        lhs.end(),
//                                        lhs.size(),
//                                        rhs.begin(),
//                                        rhs.end(),
//                                        rhs.size());
//  }
//
//  bool operator!=(const IntSet& lhs, const IntSet& rhs)
//  {
//      return !(lhs == rhs);
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_RBTREEANCHOR
#include <bslalg_rbtreeanchor.h>
#endif

#ifndef INCLUDED_BSLALG_RBTREENODE
#include <bslalg_rbtreenode.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_STDIO_H
#include <stdio.h>
#define INCLUDED_STDIO_H
#endif

namespace BloombergLP {
namespace bslalg {

                        // ================
                        // class RbTreeUtil
                        // ================

struct RbTreeUtil {
    // This 'struct' provides a namespace for a suite of utility functions that
    // operate on elements of type 'RbTreeNode'.
    //
    // Each method of this class, other than 'copyTree', provides the
    // *no-throw* exception guarantee if the the client-supplied comparator
    // provides the no-throw guarantee, and provides the *strong* guarantee
    // otherwise (see 'bsldoc_glossary').  'copyTree' provides the *strong*
    // guarantee.

    // CLASS METHODS
                                 // Navigation

    static const RbTreeNode *leftmost(const RbTreeNode *subtree);
    static       RbTreeNode *leftmost(      RbTreeNode *subtree);
        // Return the address of the leftmost node in the specified 'subtree',
        // and 'subtree' if 'subtree' has no left child.  The behavior is
        // undefined unless '0 != subtree', and 'subtree' refers to a valid
        // binary tree.  Note that the value held by the returned node will not
        // compare greater than that of any other node in 'subtree' (as
        // determined by the comparator used to organize the red-black subtree
        // data).

    static const RbTreeNode *rightmost(const RbTreeNode *subtree);
    static       RbTreeNode *rightmost(      RbTreeNode *subtree);
        // Return the address of the rightmost node in the specified
        // 'subtree', and 'subtree' if 'subtree' has no right child.  The
        // behavior is undefined unless '0 != subtree' and 'subtree' refers to
        // a valid binary tree.  Note that the value held by the returned node
        // will not compare less than that of any other node in 'subtree' (as
        // determined by the comparator used to organize the red-black subtree
        // data).

    static const RbTreeNode *next(const RbTreeNode *node);
    static       RbTreeNode *next(      RbTreeNode *node);
        // Return the address of the node that follows the specified 'node' in
        // an in-order traversal of the binary tree to which 'node' belongs, or
        // the tree's sentinel node if 'node' is the rightmost node in the
        // tree.  The behavior is undefined unless 'node' is a member of a
        // valid binary tree, and is not a sentinel node.  Note that if the
        // tree does not contain duplicate values, then the returned node will
        // have the smallest value greater than that of 'node'.

    static const RbTreeNode *previous(const RbTreeNode *node);
    static       RbTreeNode *previous(      RbTreeNode *node);
        // Return the address of the node that precedes the specified 'node' in
        // an in-order traversal of the binary tree to which 'node' belongs, or
        // the tree's rightmost node if 'node' is the sentinel node of the
        // tree.  The behavior is undefined unless or 'node' is a non-leftmost
        // member of a valid binary tree or is a sentinel 'node'.  Note that if
        // the tree does not contain duplicate values, then the returned node
        // will have the largest value less than that of 'node'.

                                 // Search

    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static const RbTreeNode *find(const RbTreeAnchor&    tree,
                                  NODE_VALUE_COMPARATOR& comparator,
                                  const VALUE&           value);
    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static       RbTreeNode *find(RbTreeAnchor&          tree,
                                  NODE_VALUE_COMPARATOR& comparator,
                                  const VALUE&           value);
        // Return the address of the leftmost node holding the specified
        // 'value' in the specified 'tree' (organized according to the
        // specified 'comparator) if found, and return 'tree.sentinel()'
        // otherwise.  'COMPARATOR' shall be a functor providing two methods
        // that can be called as if they had the following signatures:
        //..
        //  bool operator()(const RbTreeNode&, const VALUE&) const;
        //  bool operator()(const VALUE&, const RbTreeNode&) const;
        //..
        // The behavior is undefined unless 'comparator' provides a strict
        // weak ordering on objects of type 'VALUE', and 'tree' is well-formed
        // (see 'isWellFormed').

    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static const RbTreeNode *lowerBound(const RbTreeAnchor&    tree,
                                        NODE_VALUE_COMPARATOR& comparator,
                                        const VALUE&           value);
    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static       RbTreeNode *lowerBound(RbTreeAnchor&         tree,
                                        NODE_VALUE_COMPARATOR& comparator,
                                        const VALUE&           value);
        // Return the address of the leftmost node holding the smallest
        // value greater-than or equal-to 'value' in the specified 'tree'
        // (organized according to the specified 'comparator) if found, and
        // return 'tree.sentinel()' if 'value' is greater-than the rightmost
        // node in 'tree'.  'COMPARATOR' shall be a functor providing two
        // methods that can be called as if they had the following signatures:
        //..
        //  bool operator()(const RbTreeNode&, const VALUE&) const;
        //  bool operator()(const VALUE&, const RbTreeNode&) const;
        //..
        // The behavior is undefined unless 'comparator' provides a strict
        // weak ordering on objects of type 'VALUE', and 'tree' is well-formed
        // ('isWellFormed').   Note that this function returns the *first*
        // position before which 'value' could be inserted into 'tree' while
        // preserving its ordering.

    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static const RbTreeNode *upperBound(const RbTreeAnchor&    tree,
                                        NODE_VALUE_COMPARATOR& comparator,
                                        const VALUE&           value);
    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static       RbTreeNode *upperBound(RbTreeAnchor&          tree,
                                        NODE_VALUE_COMPARATOR& comparator,
                                        const VALUE&           value);
        // Return the address of the leftmost node holding the smallest
        // value greater-than 'value' in the specified 'tree' (organized
        // according to the specified 'comparator) if found, and return
        // 'tree.sentinel()' if 'value' is greater-than or equal-to the
        // rightmost node in 'tree'.  'COMPARATOR' shall be a functor
        // providing two methods that can be called as if they had the
        // following signatures:
        //..
        //  bool operator()(const RbTreeNode&, const VALUE&) const;
        //  bool operator()(const VALUE&, const RbTreeNode&) const;
        //..
        // The behavior is undefined unless 'comparator' provides a strict
        // weak ordering on objects of type 'VALUE', and 'tree' is well-formed
        // ('isWellFormed').   Note that this function returns the *last*
        // position before which 'value' could be inserted into 'tree' while
        // preserving its ordering.

                                 // Modification

    template <class FACTORY>
    static void copyTree(RbTreeAnchor        *result,
                         const RbTreeAnchor&  original,
                         FACTORY             *nodeFactory);
        // Load, into the specified 'result', a collection of newly created
        // nodes having the same red-black tree structure as that of the
        // specified 'original' tree, where each node in the returned tree is
        // created by invoking 'nodeFactory->createNode' on the corresponding
        // 'original' node; if an exception occurs, use
        // 'nodeFactory->deleteNode' to destroy any newly created nodes, and
        // propagate the exception to the caller (i.e., this operation provides
        // the *strong* exception guarantee).  'FACTORY' shall be a class
        // providing two methods that can be called as if they had the
        // following signatures:
        //..
        //  RbTreeNode *createNode(const RbTreeNode&);
        //  void deleteNode(RbTreeNode *);
        //..
        // The behavior is undefined unless 'result' is an empty tree,
        // 'original' is a well-formed (see 'isWellFormed'), and
        // 'nodeFactory->deleteNode' does not throw.

    template <class FACTORY>
    static void deleteTree(RbTreeAnchor *tree, FACTORY *nodeFactory);
        // Call 'nodeFactory->deleteNode' on each node in 'tree' and reset
        // 'tree' to an empty state.  'FACTORY' shall be a class providing a
        // method that can be called as if it has the following signature:
        //..
        //  void deleteNode(RbTreeNode *);
        //..
        // The behavior is undefined unless 'tree' is a valid binary tree, and
        // 'nodeFactory->deleteNode' does not throw.

    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static RbTreeNode *findInsertLocation(
                                 bool                   *insertAsLeftChildFlag,
                                 RbTreeAnchor           *tree,
                                 NODE_VALUE_COMPARATOR&  comparator,
                                 const VALUE&            value);
    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static RbTreeNode *findInsertLocation(
                                 bool                   *insertAsLeftChildFlag,
                                 RbTreeAnchor           *tree,
                                 NODE_VALUE_COMPARATOR&  comparator,
                                 const VALUE&            value,
                                 RbTreeNode             *hint);
        // Return the address of the node that would be the parent a node
        // holding the specified 'value', if it were to be inserted into the
        // specified 'tree' (organized according to the specified
        // 'comparator'), and load, into the specified 'insertAsLeftChildFlag',
        // 'true' if 'value' would be held as the returned node's left child,
        // and 'false' if 'value' would be held in its right child, unless
        // 'tree' is empty, in which case return 'tree->sentinel()' and load
        // 'true' into 'insertAsLeftChildFlag'.  Optionally specify a 'hint',
        // suggesting a node in 'tree' that might be the immediate successor of
        // a node holding 'value' if it were to be inserted into 'tree'.  If
        // the supplied 'hint' is the successor, this operation will take
        // amortized constant time; otherwise, it will take O(log(N))
        // operations, where N is the number of nodes in the tree.  If a node
        // holding 'value' is inserted as suggested by this method, the
        // resulting tree will be an ordered binary tree, but may require
        // rebalancing (and re-coloring) to again be a valid red-black tree.
        // 'COMPARATOR' shall be a functor providing two methods that can be
        // called as if they have the following signatures:
        //..
        //  bool operator()(const RbTreeNode&, const VALUE&) const;
        //  bool operator()(const VALUE&, const RbTreeNode&) const;
        //..
        // The behavior is undefined unless 'comparator' provides a strict
        // weak ordering on objects of type 'VALUE', 'tree' is well-formed
        // (see 'isWellFormed'), and 'hint', if supplied, is a node in 'tree'.
        // Note that this operation is intended to be used in conjunction with
        // the 'insertAt' method.

    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static RbTreeNode *findUniqueInsertLocation(
                                      int                    *comparisonResult,
                                      RbTreeAnchor           *tree,
                                      NODE_VALUE_COMPARATOR&  comparator,
                                      const VALUE&            value);
    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static RbTreeNode *findUniqueInsertLocation(
                                      int                    *comparisonResult,
                                      RbTreeAnchor           *tree,
                                      NODE_VALUE_COMPARATOR&  comparator,
                                      const VALUE&            value,
                                      RbTreeNode             *hint);
        // Return the address of the node holding the specified 'value' in the
        // specified 'tree' (organized according to the specified 'comparator')
        // if found, and the address of the node that would be the parent for
        // 'value' otherwise; load, into the specified 'comparisonResult', 0 if
        // 'value' is found, a negative number if 'value' would be held in its
        // left child, and a positive number if 'value' would be held in its
        // right child, unless 'tree' is empty, in which case load a negative
        // number into 'comparisonResult' and return 'tree->sentinel()'.
        // Optionally specify a 'hint', suggesting a node in 'tree' that might
        // be the immediate successor of a node holding 'value' if it were to
        // be inserted into 'tree'.  If the supplied 'hint' is the successor,
        // this operation will take amortized constant time; otherwise, it will
        // take O(log(N)) operations, where N is the number of nodes in the
        // tree.  If a node holding 'value' is inserted as suggested by this
        // method, the resulting tree will be an ordered binary tree, but may
        // require rebalancing (and re-coloring) to again be a valid red-black
        // tree.  'COMPARATOR' shall be a functor providing two methods that
        // can be called as if they have the following signatures:
        //..
        //  bool operator()(const RbTreeNode&, const VALUE&) const;
        //  bool operator()(const VALUE&, const RbTreeNode&) const;
        //..
        // The behavior is undefined unless 'comparator' provides a strict
        // weak ordering on objects of type 'VALUE', 'tree' is well-formed
        // (see 'isWellFormed'), and 'hint', if supplied, is a node in 'tree'.
        // Note that this operation is intended to be used in conjunction with
        // the 'insertAt' method.

    template <class NODE_COMPARATOR>
    static void insert(RbTreeAnchor           *tree,
                       const NODE_COMPARATOR&  comparator,
                       RbTreeNode             *newNode);
        // Insert the specified 'newNode' into the specified 'tree', organized
        // according to the specified 'comparator'.  The resulting tree will
        // be well-formed (see 'isWellFormed').  'NODE_COMPARATOR' shall be a
        // functor providing a method that can be called as if it had the
        // following signatures:
        //..
        //  bool operator()(const RbTreeNode&, const RbTreeNode&) const;
        //..
        // The behavior is undefined unless 'comparator' provides a strict
        // weak ordering on objects of type 'VALUE', and 'tree' is well-formed
        // (see 'isWellFormed').

    static void insertAt(RbTreeAnchor *tree,
                         RbTreeNode   *parentNode,
                         bool          leftChildFlag,
                         RbTreeNode   *newNode);
        // Insert the specified 'newNode' into the specified 'tree' as either
        // the left or right child of the specified 'parentNode', as indicated
        // by the specified 'leftChildFlag', and then rebalance the tree so
        // that it is a valid red-black tree (see 'validateRbTree').  The
        // behavior is undefined unless 'tree' is well-formed (see
        // 'isWellFormed'), and, if 'tree' is empty, 'parentNode' is
        // 'tree->sentinel()' and 'leftChildFlag' is 'true', or, if 'tree' is
        // not empty, 'parentNode' is a node in 'tree' whose left or right
        // child (as indicated by 'leftChildFlag') is 0 where if 'newNode' were
        // attached as that child (without rebalancing) 'tree' would still
        // form an ordered binary tree (though not necessarily a valid
        // red-black tree).  Note that this operation is intended to be used in
        // conjunction with the 'findInsertLocation' or
        // 'findUniqueInsertLocation' methods.

    static void remove(RbTreeAnchor *tree, RbTreeNode *node);
        // Remove the specified 'node' from the specified 'tree', and then
        // rebalance 'tree' so that it again forms a valid red-black tree (see
        // 'validateRbTree').  The behavior is undefined unless 'tree' is
        // well-formed (see 'isWellFormed').

    static void swap(RbTreeAnchor *a, RbTreeAnchor *b);
        // Efficiently exchange the nodes in the specified 'a' tree with the
        // nodes in the specified 'b' tree.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless 'a'
        // and 'b' are well-formed (see 'isWellFormed').

                                 // Utility

    static bool isLeftChild(const RbTreeNode *node);
        // Return 'true' if the specified 'node' is the left child of its
        // parent, and 'false' otherwise.  The behavior is undefined unless
        // '0 != node->parent()'.

    static bool isRightChild(const RbTreeNode *node);
        // Return 'true' if the specified 'node' is the left child of its
        // parent, and 'false' otherwise.  The behavior is undefined unless
        // '0 != node->parent()'.

    static void rotateLeft(RbTreeNode *node);
        // Perform counter-clockwise rotation on the specified 'node': Rotate
        // the node's right child (the pivot) to be the node's parent, and
        // attach the pivot's left child as the node's right child.
        //..
        //     (node)              (pivot)
        //     /    \              /     \.
        //    a   (pivot)  --->  (node)   c
        //         /   \         /    \.
        //        b     c       a      b
        //..
        // The behavior is undefined unless 'node->rightChild()' is not 0,
        // 'node->parent()' is not 0, and node's parent refers to 'node' as one
        // of its children.  Note that this operation maintains the ordering
        // of the subtree rooted at 'node'.  Also note this operation will
        // successfully rotate the root node of an unbalanced, but otherwise
        // well-formed, tree referred to by a 'RbTreeAnchor' object (see
        // 'isWellFormed') because the parent of the root node is the tree's
        // sentinel node (i.e., not 0), which refers to the root node as its
        // left child, and an 'RbTreeAnchor' object returns the left child of
        // the sentinel node as the root of the tree.

    static void rotateRight(RbTreeNode *node);
        // Perform clockwise rotation on the specified 'node': Rotate the
        // node's left child (the pivot) to be the node's parent, and attach
        // the pivot's right child as the node's left child.
        //..
        //       (node)            (pivot)
        //       /    \            /     \.
        //   (pivot)   c   --->   a     (node)
        //    /  \                      /    \.
        //   a    b                    b      c
        //..
        // The behavior is undefined unless 'node->leftChild()' is not 0,
        // 'node->parent()' is not 0, and node's parent refers to 'node' as one
        // of its children.  Note that this operation maintains the ordering
        // of the subtree rooted at 'node'.  Also note this operation will
        // successfully rotate the root node of an unbalanced, but otherwise
        // well-formed, tree referred to by a 'RbTreeAnchor' object (see
        // 'isWellFormed') because the parent of the root node is the tree's
        // sentinel node (i.e., not 0), which refers to the root node as its
        // left child, and an 'RbTreeAnchor' object returns the left child of
        // the sentinel node as the root of the tree.

                                 // Testing

    static void printTreeStructure(
                    FILE                   *file,
                    const RbTreeNode       *subtree,
                    void (*printNodeValueCallback)(FILE *, const RbTreeNode *),
                    int                     level = 0,
                    int                     spacesPerLevel = 4);
        // Write a description of the structure of the specified 'subtree' to
        // the specified output 'file' in a human-readable format, using the
        // specified 'printValueCallback' to render the value of each node.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  The behavior
        // is undefined unless 'node' is 0, or the root of a valid binary
        // tree.  Note that the implementation of this function is recursive
        // and expensive to perform, it is intended for debugging purposes
        // only.  Also note that the format is not fully specified, and can
        // change without notice.

    template <class NODE_COMPARATOR>
    static int validateRbTree(const RbTreeNode       *rootNode,
                              const NODE_COMPARATOR&  comparator);
    template <class NODE_COMPARATOR>
    static int validateRbTree(const RbTreeNode       **errorNode,
                              const char             **errorDescription,
                              const RbTreeNode        *rootNode,
                              const NODE_COMPARATOR&   comparator);
        // Return the (common) number of black nodes on each path from the
        // specified 'rootNode' to a leaf in the tree, 0 if 'rootNode' is 0,
        // and a negative number if 'rootNode' does not refer to a valid
        // red-black binary search tree, ordered according to the specified
        // 'comparator'.  Optionally specify 'errorNode' and 'errorDescription'
        // in which to load the address of a node violating a red-black tree
        // constraint and a description of that violation, respectively.  The
        // behavior is undefined unless 'rootNode' is 0, or refers to a valid
        // binary tree.
        //
        // Each node of a red-black tree is colored either red or black; null
        // nodes are considered black.  Four requirements must be satisfied
        // for 'rootNode' to refer to a valid red-black binary search tree:
        //
        //: 1 For each node in the tree, no descendents to the left of that
        //:   node would order after that node (according to the 'comparator'),
        //:   and no descendents to the right of that node would order before
        //:   it.
        //:
        //: 2 For each node in the tree, each non-null child of that node
        //:   refers to that node as its parent.
        //:
        //: 3 If a node in the tree is colored red, all its children are
        //:   colored black or are null (which is considered black).
        //:
        //: 4 For each node in the tree, every path from that node to a leaf
        //:   contains the same number of black nodes, where null children are
        //:   considered black leaf nodes.
        //
        // The behavior is undefined unless 'rootNode' is 0, or refers to a
        // valid binary tree.  Note that the implementation of this function
        // is recursive and has linear complexity with respect to the the
        // number of nodes in 'tree', it is intended for debugging purposes
        // only.

    template <class NODE_COMPARATOR>
    static bool isWellFormed(const RbTreeAnchor&    tree,
                             const NODE_COMPARATOR& comparator);
        // Return 'true' if the specified 'tree' is well-formed and refers to
        // a valid red-black tree, and 'false' otherwise.  For a
        // 'RbTreeAnchor' to be considered well-formed *all* of the following
        // must be true:
        //
        //: 1 'tree.rootNode()' must refer to a valid red-black tree, whose
        //:   nodes are organized according to 'comparator' (see
        //:   'validateRbTree').
        //:
        //: 2 'tree.firstNode()' must refer to 'tree.sentinel()' if
        //:   'tree.rootNode()' is 0, and leftmost(tree.rootNode())' otherwise.
        //:
        //: 3 'tree.nodeCount()' must be the count of nodes in 'tree' (not
        //:   including the sentinel node).
        //:
        //: 4 'tree.sentinel()->leftchild()' is 'tree.rootNode()', and (if
        //:   'tree.rootNode()' is not 0) 'tree.rootNode()->parent()' is
        //:   'tree.sentinel()'.
        //:
        //: 5 'tree.rootNode()' is 0 or 'tree.rootNode().isBlack()' is 'true'
        //
        // The behavior is undefined unless 'tree.rootNode()' is 0 or refers
        // to a valid binary tree.  Note that the implementation of this
        // function is recursive and has linear complexity with respect to the
        // number of nodes in 'tree', it is intended for debugging purposes
        // only.  Note also that the final condition, that the root node be
        // either 0 or colored black, is not a canonical requirement of a
        // red-black tree but an additional invariant enforced by the methods
        // of 'RbTreeUtil' to simplify the implementations.
};

                      // ==========================
                      // class RbTreeUtil_Validator
                      // ==========================

struct RbTreeUtil_Validator {
    // This 'struct' provides a namespace for auxiliary functions used to
    // validate a red-black binary search tree.

    // CLASS METHODS
    template <class NODE_COMPARATOR>
    static int validateRbTree(const RbTreeNode       **errorNode,
                              const char             **errorDescription,
                              const RbTreeNode        *rootNode,
                              const RbTreeNode        *minNodeValue,
                              const RbTreeNode        *maxNodeValue,
                              const NODE_COMPARATOR&   comparator);
        // Return the (common) number of black nodes on each path from the
        // specified 'rootNode' to a leaf in the tree, 0 if 'rootNode' is 0,
        // and a negative number if 'rootNode' does not refer to a valid
        // red-black binary search tree (ordered according to the specified
        // 'comparator') that contains no nodes whose value is less than the
        // specified 'minNodeValue' (if not 0) or greater-than the specified
        // 'maxNodeValue' (if not 0).  If 'rootNode' does not refer to a valid
        // red-black tree containing nodes whose values are between the
        // specified 'minNodeValue' and 'maxNodeValue' (inclusively) then load
        // 'errorNode' and 'errorDescription' with the address of a node
        // violating a red-black tree constraint and a description of that
        // violation, respectively.  The behavior is undefined unless
        // 'rootNode' is 0, or refers to a valid binary tree.

    static bool isWellFormedAnchor(const RbTreeAnchor& tree);
        // Return 'true' if the specified 'tree' is well-formed, without
        // confirming that it refers to a valid-red-black tree, and
        // 'false' otherwise.  This method will return 'true' if *all* of the
        // following are true:
        //
        //: 1 'tree.firstNode()' must refer to 'tree.sentinel()' if
        //:   'tree.rootNode()' is 0, and leftmost(tree.rootNode())' otherwise.
        //:
        //: 2 'tree.nodeCount()' must be the count of nodes in 'tree' (not
        //:   including the sentinel node).
        //:
        //: 3 'tree.sentinel()->leftchild()' is 'tree.rootNode()', and (if
        //:   'tree.rootNode()' is not 0), 'tree.rootNode()->parent()' is
        //:   'tree.sentinel().
        //:
        //: 4 'tree.rootNode()' is 0 or 'tree.rootNode().isBlack()' is 'true'
        //
        // The behavior is undefined unless 'tree.rootNode()' is 0, or refers
        // to a valid binary tree.  Note that this function provides a
        // non-templatized implementation for several criteria of a
        // well-formed tree (but not the complete set verified by
        // 'RbTreeUtil::isWellFormed').
};

                        // ============================
                        // struct RbTreeUtilTreeProctor
                        // ============================

template <class DELETER>
class RbTreeUtilTreeProctor {
    // This class implements a proctor that, unless 'release' is called,
    // invokes the parameterized 'DELETER' on each node in the tree supplied at
    // construction.

    // DATA
    RbTreeAnchor *d_tree_p;     // address of root node (held, not owned)

    DELETER      *d_deleter_p;  // address of deleter used to destroy each
                                // node (held, not owned)

  public:
    // CREATORS
    RbTreeUtilTreeProctor(RbTreeAnchor *tree, DELETER *deleter);
        // Create a proctor object that, unless 'release' is called, will,
        // on destruction, invoke the specified 'deleter' on each node in
        // 'tree'.

    ~RbTreeUtilTreeProctor();
        // Unless 'release' has been called, invoke the deleter supplied at
        // construction on each node in the tree supplied at construction.

    // MANIPULATORS
    void release();
        // Release from management the tree supplied at construction.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------
                        // class RbTreeUtil
                        // ----------------

// CLASS METHODS
inline
RbTreeNode *RbTreeUtil::leftmost(RbTreeNode *subtree)
{
    return const_cast<RbTreeNode *>(
                            leftmost(const_cast<const RbTreeNode *>(subtree)));
}

inline
RbTreeNode *RbTreeUtil::rightmost(RbTreeNode *subtree)
{
    return const_cast<RbTreeNode *>(
                           rightmost(const_cast<const RbTreeNode *>(subtree)));
}

inline
RbTreeNode *RbTreeUtil::next(RbTreeNode *node)
{
    return const_cast<RbTreeNode *>(
                                   next(const_cast<const RbTreeNode *>(node)));
}

inline
RbTreeNode *RbTreeUtil::previous(RbTreeNode *node)
{
    return const_cast<RbTreeNode *>(
                               previous(const_cast<const RbTreeNode *>(node)));
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
inline
RbTreeNode *RbTreeUtil::find(RbTreeAnchor&          tree,
                             NODE_VALUE_COMPARATOR& comparator,
                             const VALUE&           value)
{
    return const_cast<RbTreeNode *>(
               find(const_cast<const RbTreeAnchor&>(tree), comparator, value));
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
inline
const RbTreeNode *RbTreeUtil::find(const RbTreeAnchor&    tree,
                                   NODE_VALUE_COMPARATOR& comparator,
                                   const VALUE&           value)
{
    const RbTreeNode *lowBound = lowerBound(tree, comparator, value);

    // Note that a Solaris compiler bug prevents using a ternary ('?:')
    // operator here in certain contexts.

    if (lowBound != tree.sentinel() && !comparator(value, *lowBound)) {
        return lowBound;                                              // RETURN
    }
    return tree.sentinel();
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
RbTreeNode *RbTreeUtil::lowerBound(RbTreeAnchor&          tree,
                                   NODE_VALUE_COMPARATOR& comparator,
                                   const VALUE&           value)
{
    return const_cast<RbTreeNode *>(
         lowerBound(const_cast<const RbTreeAnchor&>(tree), comparator, value));
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
inline
const RbTreeNode *RbTreeUtil::lowerBound(const RbTreeAnchor&    tree,
                                         NODE_VALUE_COMPARATOR& comparator,
                                         const VALUE&           value)
{
    const RbTreeNode *nextLargestNode = tree.sentinel();
    const RbTreeNode *node            = tree.rootNode();
    while (node) {
        if (comparator(*node, value)) {
            node = node->rightChild();
        }
        else {
            nextLargestNode = node;
            node = node->leftChild();
        }
    }
    return nextLargestNode;
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
inline
RbTreeNode *RbTreeUtil::upperBound(RbTreeAnchor&          tree,
                                   NODE_VALUE_COMPARATOR& comparator,
                                   const VALUE&           value)
{
    return const_cast<RbTreeNode *>(
        upperBound(const_cast<const RbTreeAnchor&>(tree), comparator, value));
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
inline
const RbTreeNode *RbTreeUtil::upperBound(const RbTreeAnchor&    tree,
                                         NODE_VALUE_COMPARATOR& comparator,
                                         const VALUE&           value)
{
    const RbTreeNode *nextLargestNode = tree.sentinel();
    const RbTreeNode *node            = tree.rootNode();
    while (node) {
        if (comparator(value, *node)) {
            nextLargestNode = node;
            node = node->leftChild();
        }
        else {
            node = node->rightChild();
        }
    }
    return nextLargestNode;
}

template <class FACTORY>
void RbTreeUtil::copyTree(RbTreeAnchor        *result,
                          const RbTreeAnchor&  original,
                          FACTORY             *nodeFactory)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(0 == result->rootNode());
    BSLS_ASSERT_SAFE(nodeFactory);

    if (!original.rootNode()) {
        result->reset(0, result->sentinel(), 0);
        return;                                                       // RETURN
    }

    // Perform an pre-order traversal of the nodes of the tree, invoking
    // 'nodeFactory->createNode()' on each node.

    const RbTreeNode *originalNode = original.rootNode();

    RbTreeNode   *copiedRoot = nodeFactory->createNode(*originalNode);
    RbTreeAnchor  tree(copiedRoot, 0, 1);

    RbTreeUtilTreeProctor<FACTORY> proctor(&tree, nodeFactory);

    RbTreeNode *copiedNode = copiedRoot;

    copiedNode->setColor(originalNode->color());
    copiedNode->setParent(result->sentinel());
    copiedNode->setLeftChild(0);
    copiedNode->setRightChild(0);
    do {
        if (0 != originalNode->leftChild() && 0 == copiedNode->leftChild()) {
            originalNode = originalNode->leftChild();
            RbTreeNode *newNode = nodeFactory->createNode(*originalNode);
            copiedNode->setLeftChild(newNode);
            newNode->setColor(originalNode->color());
            newNode->setParent(copiedNode);
            newNode->setLeftChild(0);
            newNode->setRightChild(0);

            copiedNode = newNode;
        }
        else if (0 != originalNode->rightChild() &&
                 0 == copiedNode->rightChild()) {
            originalNode = originalNode->rightChild();
            RbTreeNode *newNode = nodeFactory->createNode(*originalNode);
            copiedNode->setRightChild(newNode);
            newNode->setColor(originalNode->color());
            newNode->setParent(copiedNode);
            newNode->setLeftChild(0);
            newNode->setRightChild(0);

            copiedNode = newNode;
        }
        else {
            originalNode = originalNode->parent();
            copiedNode   = copiedNode->parent();
        }
    } while (original.sentinel() != originalNode);

    proctor.release();

    result->reset(copiedRoot,
                  leftmost(copiedRoot),
                  original.numNodes());
}

template <class FACTORY>
void RbTreeUtil::deleteTree(RbTreeAnchor *tree, FACTORY *nodeFactory)
{
    BSLS_ASSERT_SAFE(tree);
    BSLS_ASSERT_SAFE(nodeFactory);

    if (0 == tree->rootNode()) {
        BSLS_ASSERT_SAFE(tree->sentinel() == tree->firstNode());
        return;                                                       // RETURN
    }

    // Perform a post-order traversal of the nodes of the tree, invoking
    // 'nodeFactory->deleteNode()' on each node.

    RbTreeNode *node = tree->firstNode();
    do {
        // At each iteration through this loop, we are at a leftmost (i.e.,
        // minimum) node of some sub-tree.  Note that 'node->leftChild()' may
        // not be 0 if we've simply deleted the left sub-tree of this node.

        if (node->rightChild()) {
            // If this node has a right child, then navigate to the first node
            // in the subtree to remove, and set 'node->rightChild()' to 0 so
            // we know this sub-tree has been removed when we iterate back up
            // parent pointers of the tree to this node.

            RbTreeNode *rightChild = node->rightChild();
            node->setRightChild(0);
            node = leftmost(rightChild);
        }
        else {
            RbTreeNode *parent = node->parent();
            nodeFactory->deleteNode(node);
            node = parent;
        }
    } while (tree->sentinel() != node);
    tree->reset(0, tree->sentinel(), 0);
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
RbTreeNode *RbTreeUtil::findInsertLocation(
                                 bool                   *insertAsLeftChildFlag,
                                 RbTreeAnchor           *tree,
                                 NODE_VALUE_COMPARATOR&  comparator,
                                 const VALUE&            value)
{
    BSLS_ASSERT_SAFE(insertAsLeftChildFlag);
    BSLS_ASSERT_SAFE(tree);

    RbTreeNode *parent = tree->sentinel();
    RbTreeNode *node   = tree->rootNode();
    *insertAsLeftChildFlag = true;
    while (node) {
        // Find the leaf node that would be the parent of 'newNode'.

        parent = node;
        *insertAsLeftChildFlag = comparator(value, *node);
        if (*insertAsLeftChildFlag) {
            node = node->leftChild();
        }
        else {
            node = node->rightChild();
        }
    }
    return parent;
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
RbTreeNode *RbTreeUtil::findInsertLocation(
                                 bool                   *insertAsLeftChildFlag,
                                 RbTreeAnchor           *tree,
                                 NODE_VALUE_COMPARATOR&  comparator,
                                 const VALUE&            value,
                                 RbTreeNode             *hint)
{
    BSLS_ASSERT_SAFE(insertAsLeftChildFlag);
    BSLS_ASSERT_SAFE(tree);
    BSLS_ASSERT_SAFE(hint);

    // 'hint' is valid if it is equal to, or the smallest value greater than,
    // 'value'.

    if (tree->sentinel() == hint || !comparator(*hint, value)) {
        // 'hint' is greater-than or equal-to 'value', if the previous node,
        // 'prev' is less-than or equal-to 'value', then we have a valid hint.

        RbTreeNode *prev = (tree->firstNode() == hint) ? hint : previous(hint);
        if (tree->firstNode() == hint || !comparator(value, *prev)) {
            // There will be an empty position for a child node between every
            // two consecutive nodes (in an in-order traversal) of a binary
            // tree.  Determine whether that empty position is the left child
            // of 'hint' or the right child of 'prev'.

            if (0 == hint->leftChild()) {
                *insertAsLeftChildFlag = true;
                return hint;                                          // RETURN
            }
            BSLS_ASSERT_SAFE(prev);
            *insertAsLeftChildFlag = false;
            return prev;                                              // RETURN
        }
        // 'prev' is greater than 'value', so this is not a valid hint.

    }
    // If 'hint' was less than 'value', it is not a valid hint.

    // The 'hint' is not valid, fall-back and search the entire tree.

    return findInsertLocation(insertAsLeftChildFlag, tree, comparator, value);
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
RbTreeNode *RbTreeUtil::findUniqueInsertLocation(
                                      int                    *comparisonResult,
                                      RbTreeAnchor           *tree,
                                      NODE_VALUE_COMPARATOR&  comparator,
                                      const VALUE&            value)
{
    BSLS_ASSERT_SAFE(comparisonResult);
    BSLS_ASSERT_SAFE(tree);

    // Note that 'nextSmallestNode' is used, rather than 'nextLargestNode' (as
    // seen in 'upperBound' and 'lowerBound') to avoid an unnecessary
    // negation.

    RbTreeNode *parent           = tree->sentinel();
    RbTreeNode *nextSmallestNode = 0;
    RbTreeNode *node             = tree->rootNode();

    bool leftChild = true;
    while (node) {
        // Find the leaf node that would be the parent of 'newNode'.

        parent = node;
        leftChild = comparator(value, *node);
        if (leftChild) {
            node = node->leftChild();
        }
        else {
            nextSmallestNode = node;
            node = node->rightChild();
        }
    }

    if (nextSmallestNode && !comparator(*nextSmallestNode, value)) {
        *comparisonResult = 0;
        return nextSmallestNode;                                      // RETURN
    }
    *comparisonResult = leftChild ? -1 : 1;
    return parent;
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
RbTreeNode *RbTreeUtil::findUniqueInsertLocation(
                                      int                    *comparisonResult,
                                      RbTreeAnchor           *tree,
                                      NODE_VALUE_COMPARATOR&  comparator,
                                      const VALUE&            value,
                                      RbTreeNode             *hint)
{
    BSLS_ASSERT_SAFE(comparisonResult);
    BSLS_ASSERT_SAFE(tree);
    BSLS_ASSERT_SAFE(hint);

    enum { LEFT_CHILD = -1, NODE_FOUND = 0, RIGHT_CHILD = 1 };

    // 'hint' is valid if it is first value greater than 'value' in the tree.

    if (tree->sentinel() == hint || comparator(value, *hint)) {
        // 'hint' is greater than 'value'.  If the previous node, 'prev' is
        // less than the value, then we have a valid hint.

        RbTreeNode *prev = (tree->firstNode() == hint) ? hint : previous(hint);
        if (tree->firstNode() == hint || comparator(*prev, value)) {
            // There will be an empty position for a child node between every
            // two consecutive nodes (in an in-order traversal) of a binary
            // tree.  Determine whether that empty position is the left child
            // of 'hint' or the right child of 'prev'.

            if (0 == hint->leftChild()) {
                *comparisonResult = LEFT_CHILD;
                return hint;                                          // RETURN
            }

            BSLS_ASSERT_SAFE(prev);
            *comparisonResult = RIGHT_CHILD;
            return prev;                                              // RETURN
        }
        // 'value' is ordered before 'hint', but 'prev' is not ordered before
        // 'value', so either: (1) 'value' is equal to 'prev' or (2) 'hint' is
        // not a valid hint.  Optimize for (1), by handling it as a special
        // case, as this may reasonably occur using 'upperBound' on 'value' to
        // determine a hint.

        if (!comparator(value, *prev)) {
            *comparisonResult = NODE_FOUND;
            return prev;                                              // RETURN
        }

        // 'prev' is greater than 'value', so this is not a valid hint.
    }
    // If 'value' is not ordered before 'hint', either: (1) 'value' is equal
    // to 'hint', or (2) 'hint' is not a valid hint.  Optimize for (1).

    else if (tree->sentinel() != hint && !comparator(*hint, value)) {
        *comparisonResult = NODE_FOUND;
        return hint;                                                  // RETURN
    }
    // The 'hint' is not valid, fall-back and search the entire tree.

    return findUniqueInsertLocation(comparisonResult,
                                    tree,
                                    comparator,
                                    value);
}

template <class NODE_COMPARATOR>
void RbTreeUtil::insert(RbTreeAnchor           *tree,
                        const NODE_COMPARATOR&  comparator,
                        RbTreeNode             *newNode)
{
    BSLS_ASSERT_SAFE(tree);
    BSLS_ASSERT_SAFE(newNode);

    // Note that the following logic is the same as 'findInsertLocation'
    // except that the comparator required for this operation compares two
    // nodes, rather than comparing a value to a node.

    RbTreeNode *parent        = tree->sentinel();
    RbTreeNode *node          = tree->rootNode();
    bool        leftChildFlag = true;
    while (node) {
        // Find the leaf node that would be the parent of 'newNode'.

        parent = node;
        leftChildFlag = comparator(*newNode, *node);
        if (leftChildFlag) {
            node = node->leftChild();
        }
        else {
            node = node->rightChild();
        }
    }
    return insertAt(tree, parent, leftChildFlag, newNode);
}

inline
bool RbTreeUtil::isLeftChild(const RbTreeNode *node)
{
    BSLS_ASSERT_SAFE(node);
    BSLS_ASSERT_SAFE(node->parent());

    return node->parent()->leftChild() == node;
}

inline
bool RbTreeUtil::isRightChild(const RbTreeNode *node)
{
    BSLS_ASSERT_SAFE(node);
    BSLS_ASSERT_SAFE(node->parent());

    return node->parent()->rightChild() == node;
}

template <class NODE_COMPARATOR>
inline
int RbTreeUtil::validateRbTree(const RbTreeNode       *rootNode,
                               const NODE_COMPARATOR&  comparator)
{
    const RbTreeNode *errorNode;
    const char       *errorDescription;
    return validateRbTree(&errorNode, &errorDescription, rootNode, comparator);
}

template <class NODE_COMPARATOR>
int RbTreeUtil::validateRbTree(const RbTreeNode       **errorNode,
                               const char             **errorDescription,
                               const RbTreeNode        *rootNode,
                               const NODE_COMPARATOR&   comparator)
{
    BSLS_ASSERT(errorNode);
    BSLS_ASSERT(errorDescription);

    return RbTreeUtil_Validator::validateRbTree(errorNode,
                                                errorDescription,
                                                rootNode,
                                                0,
                                                0,
                                                comparator);
}

template <class NODE_COMPARATOR>
inline
bool RbTreeUtil::isWellFormed(const RbTreeAnchor&    tree,
                              const NODE_COMPARATOR& comparator)
{
    if (!RbTreeUtil_Validator::isWellFormedAnchor(tree)) {
        return false;                                                 // RETURN
    }
    return 0 <= validateRbTree(tree.rootNode(), comparator);
}

                      // --------------------------
                      // class RbTreeUtil_Validator
                      // --------------------------

// CLASS METHODS
template <class NODE_COMPARATOR>
int RbTreeUtil_Validator::validateRbTree(
                                     const RbTreeNode       **errorNode,
                                     const char             **errorDescription,
                                     const RbTreeNode        *rootNode,
                                     const RbTreeNode        *minNodeValue,
                                     const RbTreeNode        *maxNodeValue,
                                     const NODE_COMPARATOR&   comparator)
{
    BSLS_ASSERT_SAFE(errorNode);
    BSLS_ASSERT_SAFE(errorDescription);

    //: 1 All the descendents to the left of each node are are ordered
    //:   that at or before that node, and all descendents to the right of
    //:   each node are ordered at or after that node, as determined by
    //:   'comparator'.
    //:
    //: 2 Both children of every node refer to 'node' as a parent.
    //:
    //: 3 If a node in the tree has no children, it is black.
    //:
    //: 4 If a node in the tree is red, its children are either black or 0.
    //:
    //: 5 For each node in the tree, every path from that node to a leaf
    //:   contains the same number of black nodes.

    enum { INVALID_RBTREE = -1};

    // 'NIL' nodes are considered black

    if (!rootNode) {
        return 0;                                                     // RETURN
    }

    // Rule 1.

    if ((minNodeValue && comparator(*rootNode, *minNodeValue)) ||
        (maxNodeValue && comparator(*maxNodeValue, *rootNode))) {
        *errorNode        = rootNode;
        *errorDescription = "Invalid binary search tree.";
        return INVALID_RBTREE;                                        // RETURN
    }

    const RbTreeNode *left  = rootNode->leftChild();
    const RbTreeNode *right = rootNode->rightChild();
    if ((left != 0 || right != 0) && left == right) {
        *errorNode        = rootNode;
        *errorDescription = "Invalid children";
        return INVALID_RBTREE;                                        // RETURN
    }

    // Rule 2.

    if ((left  && left->parent()  != rootNode) ||
        (right && right->parent() != rootNode)) {
        *errorNode        = rootNode;
        *errorDescription = "Invalid parent pointers for children";
        return INVALID_RBTREE;                                        // RETURN
    }

    // Rule 4.

    if (RbTreeNode::BSLALG_RED == rootNode->color()) {
        if ((left  && left->color()  != RbTreeNode::BSLALG_BLACK) ||
            (right && right->color() != RbTreeNode::BSLALG_BLACK)) {
            *errorNode        = rootNode;
            *errorDescription = "Red node with a red child.";
            return INVALID_RBTREE;                                    // RETURN
        }
    }

    int leftDepth  = validateRbTree(errorNode,
                                    errorDescription,
                                    rootNode->leftChild(),
                                    minNodeValue,
                                    rootNode,
                                    comparator);
    int rightDepth = validateRbTree(errorNode,
                                    errorDescription,
                                    rootNode->rightChild(),
                                    rootNode,
                                    maxNodeValue,
                                    comparator);

    if (leftDepth < 0 || rightDepth < 0) {
        return INVALID_RBTREE;                                        // RETURN
    }

    // Rule 5.

    if (leftDepth != rightDepth) {
        *errorNode        = rootNode;
        *errorDescription =
            "Black violation (unequal black depth from node to leaves).";
        return INVALID_RBTREE;                                        // RETURN
    }

    return (rootNode->color() == RbTreeNode::BSLALG_BLACK)
          ? leftDepth + 1
          : leftDepth;
}

                        // ----------------------------
                        // struct RbTreeUtilTreeProctor
                        // ----------------------------

template <class DELETER>
inline
RbTreeUtilTreeProctor<DELETER>::RbTreeUtilTreeProctor(RbTreeAnchor *tree,
                                                      DELETER      *deleter)
: d_tree_p(tree)
, d_deleter_p(deleter)
{
    BSLS_ASSERT_SAFE(deleter);
}

template <class DELETER>
inline
RbTreeUtilTreeProctor<DELETER>::~RbTreeUtilTreeProctor()
{
    if (d_tree_p && d_tree_p->rootNode()) {
        d_tree_p->rootNode()->setParent(d_tree_p->sentinel());

        d_tree_p->setFirstNode(RbTreeUtil::leftmost(d_tree_p->rootNode()));
        RbTreeUtil::deleteTree(d_tree_p, d_deleter_p);
    }
}

template <class DELETER>
inline
void RbTreeUtilTreeProctor<DELETER>::release()
{
    d_tree_p = 0;
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
