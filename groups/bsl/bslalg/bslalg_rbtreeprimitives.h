// bslalg_rbtreeprimitives.h                                          -*-C++-*-
#ifndef INCLUDED_BSLALG_RBTREEPRIMITIVES
#define INCLUDED_BSLALG_RBTREEPRIMITIVES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a set of primitive operations on a red-black tree.
//
//@CLASSES:
//  bslalg::RbTreePrimitives: a namespace providing red-black tree operations
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@SEE_ALSO: bslalg_rbtreenode
//
//@DESCRIPTION: This component provides utilities that operate on nodes
// forming a red-black binary search tree.
//
// A number of algorithms are provided by this component, and the following
// sections provide a short synopsis describing their observable behavior.
// See the full function-level contract for detailed description.
//..
//  Algorithm           Description
//  ----------------    -------------------------------------------------------
//                      [NAVIGATION]
//
//  minimum             Return the node holding the minimum value in the
//                      supplied tree.  This is always the left-most
//                      descendent of the root of the tree. 
//
//  maximum             Return the node holding the maximum value in the
//                      supplied tree.  This is always the right-most
//                      descendent of the root of the tree. 
//
//  next                Return the node that follows the supplied node in a
//                      in-order traversal of the binary tree.  This is the
//                      node holding the smallest value greater-than or
//                      equal-to that of the supplied node.
//
//  previous            Return the node that preceds the supplied node in a
//                      in-order traversal of the binary tree.  This is the
//                      node holding the greatest value less-than or equal-to
//                      that of the supplied node.
//
//                      [SEARCH]
//
//  find                Return the node holding the supplied value, or 0 if
//                      no such node exists in the tree.
//
//  lowerBound          Return the node holding the smallest value greater
//                      than the supplied value in the tree, or 0 if there are
//                      no nodes whose values are greater than that value.  If 
//                      there are multiple nodes of equal value meeting that
//                      description, return the node ordered *first* according
//                      to an in-order traversal.
//
//  upperBound          Return the node holding the smallest value greater
//                      than the specifed value in the tree, or 0 if there are
//                      no nodes whose values are greater than that value.  If
//                      there are multiple nodes of equal value meeting that
//                      description, return the node ordered *last* according
//                      to an in-order traversal.
//
//                      [MODIFICATION]
//
//  copyTree            Copy the supplied tree, using the supplied
//                      node-copy function to copy individual node objects,
//                      and the supplied node-deletion function to ensure
//                      exception safety.
//
//  deleteTree          Invoke the specified node-deletion function on
//                      every node in the tree.
//
//  findInsertLocation  Return a location in the tree such that inserting a
//                      node with the supplied value at that location would
//                      result in a valid binary search tree (though not
//                      necessisarily a valid red-black tree, without
//                      re-balancing the nodes in the tree).  Note that this
//                      function is intended to be used with 'insertAt'.
//
//  findUniqueInsertLocation
//                      Return a location in the tree such that, if the
//                      supplied value is unique, then inserting a node with
//                      the value at the returned location would result in a
//                      valid binary search tree (though not necessisarily a
//                      valid red-black tree, without re-balancing the nodes
//                      in the tree); otherwise return an indication that the
//                      value is not unique and the address of a node with an
//                      equivalent value.  Note that this function is
//                      intended to be used with 'insertAt'.
//
//  insert              Insert the supplied node into the tree.
//
//  insertAt            Insert the supplied node at the indicated location in
//                      the tree, and then rebalance the tree so that it
//                      is a valid red-black tree.
//
//  remove              Remove the specified node from the tree.
//
//  rotateLeft          Perform a counter-clockwise rotation on the supplied
//                      node -- rotating the node's right child (the pivot) to
//                      be the node's parent, and attaching the pivot's left
//                      child as the node's right child.
//
//  rotateRight         Perform a clockwise rotation on the supplied node --
//                      rotating the node's left child (the pivot) to be the
//                      node's parent, and attaching the pivot's right child
//                      as the node's left child.
//
//                      [UTILITY]
//
//  isLeftChild         Return 'true' if the supplied node is the left child
//                      of its parent.
//
//  isRightChild        Return 'true' if the supplied node is the right child
//                      of its parent.
//
//  printTree           Print a description of the supplied tree to the
//                      supplied file, using the user-defined node printing 
//                      function to print the values of each individual node. 
//
//  validateRbTree      Validate the supplied tree meets the criteria for a
//                      red-black binary search tree; return the black height
//                      of the tree for a valid tree, and 0 if the tree is not
//                      a valid red-black binary search tree.  This operation
//                      is for debugging purposes only.
//..
// This implementation is adapted from Cormen, Leiserson, Rivest,
// "Introduction to Algorithms" [MIT Press, 1997].
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating and Using a Tree with 'RbTreePrimitives'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TBD: A straight forward example creating a tree using 'RbTreePrimitives'.
//
///Example 2: Implementing a Set of Integers
///- - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to implement a simple container holding a set
// of (unique) integer values using a red-black binary search tree through
// using 'RbTreePrimitives'.
//
// Before defining the 'IntSet' class itself, we need to define a series of
// associated helper types:
//: 1 The node-type, for the nodes in the tree.
//: 2 An iterator, for iterating over nodes in the tree.
//: 3 A comparison functor for comparing nodes and values.
//: 4 A functor for creating nodes.
//: 5 A functor for destroying and deallocating nodes.
//
// First, we define a type 'IntSet_Node' that will represent the nodes in our
// tree of integer values.  'IntSet_Node' contains an 'int' payload, and
// inherits from 'RbTreeNode' allowing it to be operated on by
// 'RbTreePrimitives'.  Note that the underscore ("_") indicates that
// 'IntSet_Node' is private implementation type of 'IntSet', and not for use
// by clients of 'IntSet':
//..
//  struct IntSet_Node : public RbTreeNode {
//      // A red-black tree node containing an integer data-value.
//
//      int d_value;  // actual value represented by the node
//  };
//..
// Then, we define a iterator over 'IntSet_Node' objects.  We use the 'next'
// function of 'RbTreePrimitives' to increment the iterator.  Note that, for
// simplicity, this iterator is *not* a fully STL compliant iterator
// implementation:
//..
//  class IntSetConstIterator {
//      // This class defines an STL-style iterator over a non-modifiable tree
//      // of 'IntSet_Node' objects.
//
//      // DATA
//      const IntSet_Node *d_node_p;  // current location of this iterator
//
//    public:
//
//      IntSetConstIterator() : d_node_p(0) {}
//          // Create an iterator that does not refer to a node, and will
//          // compare equal to 'end' iterator of a 'IntSet'.
//
//      IntSetConstIterator(const IntSet_Node *node) : d_node_p(node) {}
//          // Create an iterator refering to the specified 'node'.
//
//  //  IntSetConstIterator(const IntSetConstIterator&) = default;
//
//      // MANIPULATOR
//  //  IntSetConstIterator& operator=(const IntSetConstIterator&) = default;
//
//..
// We implement the prefix-increment operator using the 'next' function of
// 'RbTreePrimitives: 
//..
//      IntSetConstIterator& operator++()
//         // Advance this iterator to the subsequent value it the 'IntSet',
//         // and return a reference providing modifiable access to this
//         // iterator.   The behavior is undefined unless this iterator
//         // refers to a element in an 'IntSet'.
//      {
//          d_node_p = static_cast<const IntSet_Node *>(
//                                           RbTreePrimitives::next(d_node_p));
//          return *this;
//      }
//
//      // ACCESSORS
//      const int& operator*() const { return d_node_p->d_value; }
//          // Return a reference providing non-modifiable access to the value
//          // refered to by this iterator.
//
//      const int *operator->() const { return &d_node_p->d_value; }
//          // Return an address providing non-modifiable access to the value
//          // refered to by this iterator.
//
//      const IntSet_Node *nodePtr() const { return d_node_p; }
//          // Return the address of the non-modifiable int-set node refered
//          // to by this iterator
//  };
//
//  // FREE OPERATORS
//  bool operator==(const IntSetConstIterator &lhs,
//                  const IntSetConstIterator &rhs)
//      // Return 'true' if the 'lhs and 'rhs' objects have the same value, and
//      // 'faluse' otherwise.  Two 'IntSetConstIterator' objects have the
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
// Next, we define a comparison functor for 'IntSet_Node' objects.  This
// comparator is supplied to 'RbTreePrimitives' functions that must compare
// nodes with values -- i.e., those with a 'NODE_VALUE_COMPARATOR' template
// parameter (e.g., 'find' and 'findInsertLocation'):
//..
//  struct IntSet_NodeValueComparator {
//      // This class defines a comparator providing comparison operations
//      // between 'IntSet_Node' objects, and 'int' values.
//
//      typedef IntSet_Node NodeType;  // Alias for the type of node (required
//                                     // by RbTreeNodePrimitives)
//..
// Notice that we define a type 'NodeType' for our comparison function.  The
// 'RbTreePrimitives' operations taking a template parameter
// 'NODE_VALUE_COMPARATOR'  static-cast 'RBTReeNode' objects to
// 'NODE_VALUE_COMPARATOR::NodeType' to supply them to the comparison
// operations.
//..
//      bool operator()(const IntSet_Node& lhs, int rhs) const
//      {
//          return lhs.d_value < rhs;
//      }
//
//      bool operator()(int lhs, const IntSet_Node& rhs) const
//      {
//          return lhs < rhs.d_value;
//      }
//  };
//..
// Next, we define a functor for creating 'IntSet_Node' objects.  This functor
// provides two 'operator()' overloads.  The first 'operator()' overload,
// which copies an 'IntSet_Node' is required by 'RbTreePrimitives' operations
// with a 'NODE_COPY_FUNCTION' template parameter (e.g., 'copyTree').  The
// second 'operator()' overload isn't used by 'RbTreePrimitives', but is
// provided here to centralize all node creation logic for 'IntSet' in
// 'IntSet_NodeCreator'.
//..
//  class IntSet_NodeCreator {
//      // This class defines a creator object, that when invoked, creates a
//      // new 'IntSet_Node' (either from a int value, or an existing
//      // 'IntSet_Node' object) using the allocator supplied at construction.
//
//      bslma_Allocator *d_allocator_p;  // allocator, (held, not owned)
//
//    public:
//      typedef IntSet_Node NodeType;
//
//      IntSet_NodeCreator(bslma_Allocator *allocator)
//      : d_allocator_p(allocator)
//      {
//          BSLS_ASSERT_SAFE(allocator);
//      }
//
//
//      IntSet_Node *operator()(const IntSet_Node& node) const
//      {
//          IntSet_Node *newNode = new (*d_allocator_p) IntSet_Node;
//          newNode->d_value = node.d_value;
//          return newNode;
//      }
//
//      IntSet_Node *operator()(int value) const
//      {
//          IntSet_Node *newNode = new (*d_allocator_p) IntSet_Node;
//          newNode->d_value = value;
//          return newNode;
//      }
//  };
//..
// Then, we define a functor for destroying 'IntSet_Node' objects, and
// deallocating their memory footprint.  This functor is required by
// 'RbTreePrimitives' operations with a 'NODE_DELETE_FUNCTION' template
// parameter (e.g., 'deleteTree' and 'copyTree'):
//..
//  class IntSet_NodeDeleter {
//      // This class defines a deleter object that, when invoked, destroys
//      // the supplied 'IntSet_Node' and deallocates its footprint using the
//      // allocator supplied at construction.
//
//      // DATA
//      bslma_Allocator *d_allocator_p;  // allocator, (held, not owned)
//
//    public:
//
//      // PUBLIC TYPES
//      typedef IntSet_Node NodeType;
//
//      IntSet_NodeDeleter(bslma_Allocator *allocator)
//      : d_allocator_p(allocator)
//      {
//          BSLS_ASSERT_SAFE(allocator);
//      }
//
//      void operator()(IntSet_Node *node) const
//      {
//          d_allocator_p->deleteObject(node);
//      }
//  };
//..
// Next, having defined the requesite helper types, we define the public
// interface for our 'IntSet' type.
//
// Note that for the purposes of illustrating the use of 'RbTreePrimitives'
// a number of simplifications have been made.  For example, this
// implementation provides only a minimal set of criticial operations, it does
// not maintain a pointer to the last node in the tree (as it also does not
// expose a reverse iterator), and it does not use the empty base-class
// optimization for the comparator, etc.
//..
//  class IntSet {
//      // This class implements a set of (unique) 'int' values.
//
//      // DATA
//      IntSet_Node           *d_root_p;       // root of the tree
//
//      IntSet_Node           *d_first_p;      // left-most node in the tree
//
//      IntSet_NodeValueComparator  
//                             d_comparator;   // comparison functor for ints
//
//      int                    d_numElements;  // size of the set
//
//      bslma_Allocator       *d_allocator_p;  // allocator (held, not owned)
//
//      // FRIENDS
//      friend bool operator==(const IntSet& lhs, const IntSet& rhs);
//
//    private:
//
//      // PRIVATE CLASS METHODS
//      static IntSet_Node *toNode(RbTreeNode *node);
//      static const IntSet_Node *toNode(const RbTreeNode *node);
//          // Return the 'IntSet_Node' refered to by the specified node.  The
//          // behavior is undefined unless 'node' refers to a 'IntSet_Node'
//          // object.  Note that this a convenience function for applying a
//          // 'static_cast' to the 'RbTreeNode' return value for
//          // 'RbTreePrimitives' operation.
//
//    public:
//
//      // PUBLIC TYPES
//      typedef IntSetConstIterator const_iterator;
//
//      // CREATORS
//      IntSet(bslma_Allocator *basicAllocator = 0);
//          // Create a empty 'IntSet'.  Optionally specify a 'basicAllocator'
//          // used to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      IntSet(const IntSet& original, bslma_Allocator *basicAllocator = 0);
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
//          // insert it into this set, returning an iterator refering to the
//          // newly added value, and return an iterator refering to the
//          // existing instance of 'value' in this set otherwise.
//
//      const_iterator erase(const_iterator iterator);
//          // Remove the value refered to by the specified 'iterator' from
//          // this set, and return an iterator refering to the value
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
//          // Return an iterator referring lowest value in this set, or
//          // 'end()' if this set is empty. 
//
//      const_iterator end() const;
//          // Return an iterator refering to the value one past the highest
//          // value in this set.
//
//      const_iterator find(int value) const;
//          // Return a iterator refering to the specified 'value' in this
//          // set, or 'end()' if 'value' is not a member of this set.
//
//      int size() const;
//          // Return the number of elements in this set.
//  };
//
//  // FREE OPERATORS
//  bool operator==(const IntSet& lhs, const IntSet& rhs);
//      // Return 'true' if the 'lhs and 'rhs' objects have the same value, and
//      // 'false' otherwise.  Two 'IntSet' objects have the same value if
//      // they contain the same number of elements, and if for each element
//      // in 'lhs' there is a corresponding element in 'rhs' with the same
//      // value.
//
//  bool operator!=(const IntSet& lhs, const IntSet& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
//      // the same value, and 'false' otherwise.  Two 'IntSet' objects do not
//      // have the same value if they differ in the number of elements they
//      // contain, of if for any element in 'lhs' there is not a
//      // corresponding element in 'rhs' with the same value.
//..
// Now, we implement the methods of 'IntSet' using 'RbTreePrimitives' and the
// helper types we defined earlier:
//..
//  // PRIVATE CLASS METHODS
//  IntSet_Node *IntSet::toNode(RbTreeNode *node)
//  {
//      return static_cast<IntSet_Node *>(node);
//  }
//
//  const IntSet_Node *IntSet::toNode(const RbTreeNode *node) {
//      return static_cast<const IntSet_Node *>(node);
//  }
//
//  // CREATORS
//  IntSet::IntSet(bslma_Allocator *basicAllocator)
//  : d_root_p(0)
//  , d_first_p(0)
//  , d_comparator()
//  , d_numElements(0)
//  , d_allocator_p(bslma_Default::allocator(basicAllocator))
//  {
//  }
//
//  IntSet::IntSet(const IntSet& original, bslma_Allocator *basicAllocator)
//  : d_root_p(0)
//  , d_first_p(0)
//  , d_comparator()
//  , d_numElements(0)
//  , d_allocator_p(bslma_Default::allocator(basicAllocator))
//  {
//      if (original.d_root_p) {
//          // In order to copy the tree, we create an instance of our
//          // node-creation functor, which we supply to 'copyTree'.
//
//          IntSet_NodeCreator creator(d_allocator_p);
//          IntSet_NodeDeleter deleter(d_allocator_p);
//          d_root_p  = toNode(RbTreePrimitives::copyTree(original.d_root_p,
//                                                        creator,
//                                                        deleter));
//          d_first_p = toNode(RbTreePrimitives::minimum(d_root_p));
//          d_numElements = original.d_numElements;
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
//      IntSet temp(rhs, d_allocator_p);
//      swap(temp);
//  }
//
//  IntSet::const_iterator IntSet::insert(int value)
//  {
//..
// To insert a value into the tree, we first find the location where the node
// would be added, and whether 'value' is unique.  If 'value' is not unique
// we do not want to incur the expense of allocating memory for a node:
//..
//      bool isUnique, isLeftChild;
//      RbTreeNode *insertLocation =
//                     RbTreePrimitives::findUniqueInsertLocation(&isUnique,
//                                                                &isLeftChild,
//                                                                d_root_p,
//                                                                d_comparator,
//                                                                value);
//      if (!isUnique) {
//          return const_iterator(toNode(insertLocation));            // RETURN
//      }
//..
// If 'value' is unique, we create a new node, and supply it to 'insertAt'
// along with the tree location returned by 'findUniqueInsertLocation.
//..
//
//      IntSet_NodeCreator creator(d_allocator_p);
//      IntSet_Node *newNode = creator(value);
//      d_root_p = toNode(RbTreePrimitives::insertAt(d_root_p,
//                                                   insertLocation,
//                                                   isLeftChild,
//                                                   newNode));
//      // We may also need to update 'd_first_p', if the new node was
//      // inserted as the first node in the tree.
//
//      if (!insertLocation || d_first_p == insertLocation && isLeftChild) {
//          BSLS_ASSERT(newNode == RbTreePrimitives::minimum(d_root_p));
//          d_first_p = newNode;
//      }
//      ++d_numElements;
//      return const_iterator(toNode(newNode));
//  }
//
//  IntSet::const_iterator IntSet::erase(const_iterator iterator)
//  {
//      BSLS_ASSERT(0 != iterator.nodePtr());
//      IntSet_Node *node = const_cast<IntSet_Node *>(iterator.nodePtr());
//
//      // Before removing the node, we first find the subsequent node to which
//      // we will return an iterator.
//
//      IntSet_Node *next = toNode(RbTreePrimitives::next(node));
//      if (d_first_p == node) {
//          d_first_p = next;
//      }
//      d_root_p = toNode(RbTreePrimitives::remove(d_root_p, node));
//      IntSet_NodeDeleter deleter(d_allocator_p);
//      deleter(node);
//      --d_numElements;
//      return const_iterator(next);
//  }
//
//  void IntSet::clear()
//  {
//      // Create an instance of the node-deletion functor, which we will
//      // supply to 'deleteTree'.
//
//      if (0 != d_root_p) {
//          IntSet_NodeDeleter deleter(d_allocator_p);
//          RbTreePrimitives::deleteTree(d_root_p, deleter);
//
//          d_root_p      = 0;
//          d_first_p     = 0;
//          d_numElements = 0;
//      }
//  }
//
//  void IntSet::swap(IntSet& other) {
//      BSLS_ASSERT(d_allocator_p == other.d_allocator_p);
//      bslalg_SwapUtil::swap(d_root_p, other.d_root_p);
//      bslalg_SwapUtil::swap(d_root_p, other.d_first_p);
//  }
//
//  // ACCESSORS
//  IntSet::const_iterator IntSet::begin() const
//  {
//      return const_iterator(d_first_p);
//  }
//
//  IntSet::const_iterator IntSet::end() const
//  {
//      return const_iterator(0);
//  }
//
//  IntSet::const_iterator IntSet::find(int value) const
//  {
//      const RbTreeNode *node = RbTreePrimitives::find(d_root_p,
//                                                      d_comparator,
//                                                      value);
//      return const_iterator(toNode(node));
//  }
//
//  int IntSet::size() const
//  {
//      return d_numElements;
//  }
//..
// Finally, we implement the free operators on 'IntSet':
//..
//  // FREE OPERATORS
//  bool operator==(const IntSet& lhs, const IntSet& rhs)
//  {
//      if (lhs.size() != rhs.size()) {
//          return false;                                             // RETURN
//      }
//      IntSet::const_iterator lIt = lhs.begin();
//      IntSet::const_iterator rIt = rhs.begin();
//      for (; lIt != lhs.end(); ++lIt, ++rIt) {
//          if (*lIt != *rIt) {
//              return false;                                         // RETURN
//          }
//      }
//      return true;
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

#ifndef INCLUDED_BSLALG_RBTREENODE
#include <bslalg_rbtreenode.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_STDIO
#include <stdio.h>
#define INCLUDED_STDIO
#endif

namespace BloombergLP {
namespace bslalg {

                        // ======================
                        // class RbTreePrimitives
                        // ======================

struct RbTreePrimitives {
    // This 'struct' provides a namespace for a suite of utility functions that
    // operate on elements of type 'RbTreeNode'.  

    typedef void (*printNodeValueCallback)(FILE *, const RbTreeNode *);
        // Alias for a pointer to a function that prints a 'RbTreeNode'
        // object's value to a 'FILE' descriptor.

                                 // Navigation

    static const RbTreeNode *minimum(const RbTreeNode *node);
    static       RbTreeNode *minimum(      RbTreeNode *node);
        // Return the address of the node holding the minimum value in the
        // subtree rooted at the specified 'node' (as determined by the
        // comparator used to organize the red-black tree data).  This is
        // always the left-most descendent of 'node'.  The behavior is
        // undefined unless 'node' is a valid 'RbTreeNode' object referring to
        // a valid binary tree.

    static const RbTreeNode *maximum(const RbTreeNode *node);
    static       RbTreeNode *maximum(      RbTreeNode *node);
        // Return the address of the node holding the maximum value in the
        // subtree rooted at the specified 'node' (as determined by the
        // comparator used to organize the red-black tree data).  This is
        // always the right-most descendent 'node'.  The behavior is
        // undefined unless 'node' is a valid 'RbTreeNode' object referring to
        // a valid binary tree.

    static const RbTreeNode *next(const RbTreeNode *node);
    static       RbTreeNode *next(      RbTreeNode *node);
        // Return the address of the node holding the smallest value greater
        // than that of the specified 'node' (as determined by the comparator
        // used to organize the red-black tree data).  This is the next node
        // according to an in-order traversal of the tree.  The behavior is
        // undefined unless 'node' is a valid 'RbTreeNode' object referring to
        // a valid node in a binary tree.

    static const RbTreeNode *previous(const RbTreeNode *node);
    static       RbTreeNode *previous(      RbTreeNode *node);
        // Return the address of the node holding the greatest value smaller
        // than that of the specified 'node' (as determined by the comparator
        // used to organize the red-black tree data).  This is the previous
        // node according to an in-order traversal of the tree.  The behavior
        // is undefined unless 'node' is a valid 'RbTreeNode' object referring
        // to a valid node in a binary tree.

                                 // Search

    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static const RbTreeNode *find(const RbTreeNode            *node,
                                  const NODE_VALUE_COMPARATOR&  comparator,
                                  const VALUE&                 value);
    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static       RbTreeNode *find(RbTreeNode                  *node,
                                  const NODE_VALUE_COMPARATOR&  comparator,
                                  const VALUE&                 value);
        // Return the address of a node holding the the specified 'value', in
        // the tree rooted at the specified 'node', which has been organized
        // using a comparator providing an equivalent ordering to the
        // specified 'comparator'.  Compilation will fail
        // unless 'NODE_VALUE_COMPARATOR' provides a type
        // 'NODE_VALUE_COMPARATOR::NodeType', to which 'RbTreeNode' can be
        // 'static_cast', that provides two methods compatible with the
        // following two signatures, respectively:
        //..
        //  bool operator(const NodeType&, const VALUE&) const;
        //  bool operator(const VALUE&, const NodeType&) const;
        //..
        // The behavior is undefined unless the 'comparator' provides a
        // strict-weak ordering on 'VALUE' and
        // 'NODE_VALUE_COMPARATOR::NodeType' objects, and 'rootNode' is a
        // valid binary search tree whose elements have an equivalent ordering
        // to that supplied by 'comparator'.

    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static const RbTreeNode *lowerBound(
                                      const RbTreeNode             *node,
                                      const NODE_VALUE_COMPARATOR&  comparator,
                                      const VALUE&                  value);
    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static       RbTreeNode *lowerBound(
                                      RbTreeNode                   *node,
                                      const NODE_VALUE_COMPARATOR&  comparator,
                                      const VALUE&                  value);
        // Return the address of the node holding the smallest value greater
        // than the specified 'value', in the tree rooted at the specified
        // 'rootNode', using the specified 'comparator' to compare 'value' to
        // the value of nodes in the tree.  If there are multiple nodes having
        // the equal values meeting that criteria, return the node ordered
        // *first* in an in-order traversal of the tree.  Compilation will fail
        // unless 'NODE_VALUE_COMPARATOR' provides a type
        // 'NODE_VALUE_COMPARATOR::NodeType', to which 'RbTreeNode' can be
        // 'static_cast', that provides two methods compatible with the
        // following two signatures, respectively:
        //..
        //  bool operator(const NodeType&, const VALUE&) const;
        //  bool operator(const VALUE&, const NodeType&) const;
        //..
        // The behavior is undefined unless the 'comparator' provides a
        // strict-weak ordering on 'VALUE' and
        // 'NODE_VALUE_COMPARATOR::NodeType' objects, and 'rootNode' is a
        // valid binary search tree whose elements have an equivalent ordering
        // to that supplied by 'comparator'.

    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static const RbTreeNode *upperBound(
                                      const RbTreeNode             *rootNode,
                                      const NODE_VALUE_COMPARATOR&  comparator,
                                      const VALUE&                  value);
    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static       RbTreeNode *upperBound(
                                      RbTreeNode                   *rootNode,
                                      const NODE_VALUE_COMPARATOR&  comparator,
                                      const VALUE&                  value);
        // Return the address of the node holding the smallest value greater
        // than the specified 'value', in the tree rooted at the specified
        // 'rootNode', using the specified 'comparator' to compare 'value' to
        // the value of nodes in the tree.  If there are multiple nodes having
        // the equal values meeting that criteria, return the node ordered
        // *last* in an in-order traversal of the tree.  Compilation will fail
        // unless 'NODE_VALUE_COMPARATOR' provides a type
        // 'NODE_VALUE_COMPARATOR::NodeType', to which 'RbTreeNode' can be
        // 'static_cast', that provides two methods compatible with the
        // following two signatures, respectively:
        //..
        //  bool operator(const NodeType&, const VALUE&) const;
        //  bool operator(const VALUE&, const NodeType&) const;
        //..
        // The behavior is undefined unless the 'comparator' provides a
        // strict-weak ordering on 'VALUE' and
        // 'NODE_VALUE_COMPARATOR::NodeType' objects, and 'rootNode' is a valid
        // binary search tree whose elements have an equivalent ordering to
        // that supplied by 'comparator'.

                                 // Modification

    template <class NODE_COPY_FUNCTION, class NODE_DELETE_FUNCTION>
    static RbTreeNode *copyTree(
                              const RbTreeNode            *original,
                              const NODE_COPY_FUNCTION&    nodeCopyFunction,
                              const NODE_DELETE_FUNCTION&  nodeDeleteFunction);
        // Return the root of a newly created tree having the same structure
        // as the specified 'original', where each node in 'original' has a
        // corresponding node in the returned tree created by calling
        // 'nodeCopyFunction' on the 'original' node; if an exception occurs
        // duing the operation the specified 'nodeDeleteFunction' will be
        // used to destroy any created nodes, and the exception will be
        // propagated to the caller.  Compilation will fail unless
        // 'NODE_COPY_FUNCTION' provides a type'NODE_COPY_FUNCTION::NodeType',
        // to which 'RbTreeNode' can be 'static_cast', that provides a method
        // accepting an address providing non-modifiable access to an
        // 'NODE_COPY_FUNCTION::NodeType' and returns a type convertable to 
        // 'RbTreeNode *' -- for example:
        //..
        //  NodeType *operator(const NodeType&) const;
        //..
        // Compilation will also fail unless 'NODE_DELETE_FUNCTION' provides a
        // type 'NODE_DELETE_FUNCTION::NodeType', to which 'RbTreeNode' can be
        // 'static_cast', that provides a method accepting an address
        // providing modifiable access to an 'NODE_DELETE_FUNCTION::NodeType'
        // -- for example:
        //..
        //  void operator(NodeType *) const;
        //..
        // The behavior is undefined unless 'original' refers to a valid
        // binary tree and 'nodeCopyFunction' returns a 'RbTreeNode' that is
        // *not* an alias to any node in the 'original' tree.

    template <class NODE_DELETE_FUNCTION>
    static void deleteTree(RbTreeNode                  *rootNode,
                           const NODE_DELETE_FUNCTION&  nodeDeleteFunction);
        // Destroy the tree rooted at the the specified 'rootNode', and call
        // 'nodeDeleterFunction' on every node in that tree.   Compilation
        // will fail unless 'NODE_DELETE_FUNCTION' provides a type
        // 'NODE_DELETE_FUNCTION::NodeType', to which 'RbTreeNode' can be
        // 'static_cast', that provides a method accepting an address
        // providing modifiable access to an 'NODE_DELETE_FUNCTION::NodeType'
        // -- for example:
        //..
        //  void operator(NodeType *) const;
        //..
        // The behavior is undefined unless 'original' refers to a valid
        // binary tree.

    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static RbTreeNode *findInsertLocation(
                                    bool                         *isLeftChild,
                                    RbTreeNode                   *rootNode,
                                    const NODE_VALUE_COMPARATOR&  comparator,
                                    const VALUE&                  value);
        // Return a node in the binary search-tree rooted at the specified
        // 'rootNode', ordered according to the specified 'comparator', that
        // would be a valid parent for a node holding the specified 'value',
        // and load 'isLeftChild' with 'true' if 'value' would be the left
        // child of the returned node, and 'false' otherwise.  If 'rootNode' is
        // 0, return 0 with no effect on 'isLeftChild'.  Assuming the returned
        // node is not 0, then indicated child (as determined by
        // 'isLeftChild') of that node will be 0 and attaching a new node
        // holding 'value' as that child would result in a valid binary search
        // tree -- though *not* necessarily a valid red-black tree, without
        // re-balancing the nodes in the tree.  Compilation will fail unless
        // 'NODE_VALUE_COMPARATOR' provides a type
        // 'NODE_VALUE_COMPARATOR::NodeType', to which 'RbTreeNode' can be
        // 'static_cast', that provides two methods compatible with the
        // following two signatures, respectively:
        //..
        //  bool operator(const NodeType&, const VALUE&) const;
        //  bool operator(const VALUE&, const NodeType&) const;
        //..
        // The behavior is undefined unless the 'comparator' provides a
        // strict-weak ordering on 'VALUE' and
        // 'NODE_VALUE_COMPARATOR::NodeType' objects, and 'rootNode' is either
        // 0, or a valid binary search tree whose elements have an equivalent
        // ordering to that supplied by 'comparator'.


    template <class NODE_VALUE_COMPARATOR, class VALUE>
    static RbTreeNode *findUniqueInsertLocation(
                                   bool                         *isUniqueValue,
                                   bool                         *isLeftChild,
                                   RbTreeNode                   *rootNode,
                                   const NODE_VALUE_COMPARATOR&  comparator,
                                   const VALUE&                  value);
        // If the specified 'value' is unique, according to the specified
        // 'comparator', in the binary search-tree rooted at the specified
        // 'rootNode', then load 'true' into 'isUniqueValue' and return a node
        // that would be a valid parent of 'value' and load 'isLeftChild' with
        // 'true' if 'value' would be its left child, and 'false' if 'value'
        // would be its right child; otherwise, if 'value' is not unique, load
        // 'false' into 'isUniqueValue' and return a node containing 'value'.
        // If 'rootNode' is 0, load 'isUniqueValue' with 'true' and return 0
        // with no effect on 'isLeftChild'.   Assuming 'isUniqueValue' is
        // 'true' and the returned node is not 0, then indicated child (as
        // determined by  'isLeftChild') of that node will be 0 and attaching
        // a new node holding 'value' as that child would result in a valid
        // binary search tree -- though *not* necessarily a valid red-black
        // tree, without re-balancing the nodes in the tree.  Compilation will
        // fail unless 'NODE_VALUE_COMPARATOR' provides a type
        // 'NODE_VALUE_COMPARATOR::NodeType', to which 'RbTreeNode' can be
        // 'static_cast', that provides two methods compatible with the
        // following two signatures, respectively:
        //..
        //  bool operator(const NodeType&, const VALUE&) const;
        //  bool operator(const VALUE&, const NodeType&) const;
        //..
        // The behavior is undefined unless the 'comparator' provides a
        // strict-weak ordering on 'VALUE' and
        // 'NODE_VALUE_COMPARATOR::NodeType' objects, and 'rootNode' is either
        // 0, or a valid binary search tree whose elements have an equivalent
        // ordering to that supplied by 'comparator'.

    template <class NODE_COMPARATOR>
    static RbTreeNode *insert(RbTreeNode             *rootNode,
                              const NODE_COMPARATOR&  comparator,
                              RbTreeNode             *newNode);
        // Return the address of the (potentially new) root of a red-back tree
        // composed by inserting the specified 'newNode' into the red-back
        // tree currently rooted at the specified 'rootNode' (which may be 0),
        // using the specified 'comparator' to determine the relative ordering
        // of nodes in the tree.  The resulting tree will be a valid red-black
        // binary search tree ('validateRbTree' will return a value greater
        // than 0).  Compilation will fail unless 'NODE_COMPARATOR' provides a
        // type 'NODE_COMPARATOR::NodeType', to which 'RbTreeNode' can be
        // 'static_cast', that provides 'operator()', returning a type
        // convertable to bool, that accepts two 'NODE_COMPARATOR::NodeType'
        // objects (see the usage example).  The behavior is undefined unless
        // the 'comparator' provides a strict-weak ordering on
        // 'NODE_COMPARATOR::NodeType' objects, and 'rootNode' is either 0, or
        // a valid binary search tree whose elements have an equivalent
        // ordering to that supplied by 'comparator'.

    static RbTreeNode *insertAt(RbTreeNode *rootNode,
                                RbTreeNode *parentNode,
                                bool        leftChild,
                                RbTreeNode *newNode);
        // Insert the specified 'newNode' into the red-black binary search
        // tree rooted at the specified 'rootNode', as the child of the
        // specified 'parentNode' -- the left-child of 'parentNode' if
        // 'leftChild' is 'true', and the right-child of 'parentNode'
        // otherwise -- and then rebalance the tree so that it is a valid
        // red-black tree (i.e., validateRbTree' will return a value greater
        // than 0).  Return the (potentially new) root node of the resulting
        // red-black binary search tree.  'rootNode' may be 0, in which case
        // 'parentNode' and 'leftChild' are ignored.  The behavior is
        // undefined unless 'rootNode' is either 0, or the root
        // node of valid red-black binary search tree, and 'parentNode' is a
        // node in that binary search tree whose left or right child (as
        // indicated by 'leftChild') is 0, where, if 'newNode' were simply
        // attached as that child, 'rootNode' would still form a valid binary
        // search tree (though not necessarily a valid red-black tree).

    static RbTreeNode *remove(RbTreeNode *rootNode, RbTreeNode *node);
        // Return the address of the (potentially new) root of a red-back tree
        // composed by removing the specified 'node' from the red-back
        // tree currently rooted at the specified 'rootNode'.  The resulting
        // tree will be a valid red-black binary search tree ('validateRbTree'
        // will return a value greater than 0).  The behavior is undefined
        // unless 'rootNode' refers to a valid red-black tree that contains
        // 'node'.

                                 // Utility

    static bool isLeftChild(const RbTreeNode *node);
        // Return 'true' if the specified 'node' is the left child of its
        // parent, and 'false' otherwise.  The behavior is undefined if the
        // parent of 'node' is 0.

    static bool isRightChild(const RbTreeNode *node);
        // Return 'true' if the specified 'node' is the left child of its
        // parent, and 'false' otherwise.  The behavior is undefined if the
        // parent of 'node' is 0.

    static void printTree(FILE                   *file,
                          const RbTreeNode       *node,
                          printNodeValueCallback  printValueCallback,
                          int                     level = 0,
                          int                     spacesPerLevel = 4);
        // Write the tree rooted at the specified 'rootNode' to the specified
        // output 'file' in a human-readable format, using the
        // 'printValueCallback' to render the value of each node.  Optionally
        // specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute
        // value indicates the number of spaces per indentation level for this
        // and all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  Note that the
        // format is not fully specified, and can change without notice.
        // The behavior is undefined unless 'node' is a valid node in a binary
        // search tree.

    static RbTreeNode *rotateRight(RbTreeNode *rootNode, RbTreeNode *node);
        // Perform clockwise rotation on the specified 'node', in the tree
        // roted at the specified 'rootNode', such that the resulting tree
        // tree is still a valid binary search.  Return the (potenially new)
        // root node of the resulting tree.  For example:
        //..
        //     (node)             (X)
        //     /    \            /   \
        //   (X)     c   --->   a    (node)
        //   / \                     /    \
        //  a   b                   b      c
        //..
        // The behavior is undefined if 'node' or its left child are 0.

   static RbTreeNode *rotateLeft(RbTreeNode *rootNode, RbTreeNode *node);
        // Perform counter-clockwise rotation on the specified 'node', in the
        // tree roted at the specified 'rootNode', such that the resulting tree
        // tree is still a valid binary search.  Return the (potenially new)
        // root node of the resulting tree.  For example:
        //..
        //     (node)               (Y)
        //     /    \              /   \
        //    a     (Y)  --->  (node)    c
        //          / \        /    \
        //         b   c      a      b
        //..
        // The behavior is undefined if 'node' or its right child are 0.

    template <class NODE_COMPARATOR>
    static int validateRbTree(const RbTreeNode        *node,
                              const NODE_COMPARATOR&   comparator);
    template <class NODE_COMPARATOR>
    static int validateRbTree(const RbTreeNode       **errorNode,
                              const char             **errorDescription,
                              const RbTreeNode        *node,
                              const NODE_COMPARATOR&   comparator);
        // Return the number of black nodes between the specified 'node' and
        // each leaf in the tree rooted at 'node', or 0 if 'node' does not
        // refer to a valid red-black binary search tree that is ordered
        // according to the specified 'comparator'.   If 'node' is 0, this
        // method will return 1.  Optionally specify 'errorNode' and
        // 'errorDescription' in which to load the address of a node violating
        // a red-black tree contraint and a description of that violation,
        // respectively.  'node' refers to a valid red-black binary search
        // tree if:
        //
        //: 1 All nodes to the left of 'node' are ordered at or before 'node',
        //:   and all nodes to the right of 'node' are ordered at or after
        //:   'node', as determined by the specified 'comparator'.
        //:
        //: 2 Both children of 'node' refer to 'node' as a parent.
        //:
        //: 3 If 'node' has no children, it is black.
        //:
        //: 4 If 'node' is red, that its children are either black or 0.
        //:
        //: 5 Every path from 'node' to a leaf contains the same number of
        //:   black nodes.
        //:
        //: 6 Rules (1-5) are true of the left and right sub-trees of 'node'.
        //
        // Note that this operations uses recursion and is expensive to
        // perform, it is intended for debugging purposes only.
};

                      // ================================
                      // class RbTreePrimitives_Validator
                      // ================================

struct RbTreePrimitives_Validator {
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
};

// TBD: Development purposes.  Remove.
void debugPrintNodeValue(FILE *file, const RbTreeNode *node);

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------
                        // class RbTreePrimitives
                        // ----------------------

// CLASS METHODS
inline
RbTreeNode *RbTreePrimitives::minimum(RbTreeNode *node)
{
    return const_cast<RbTreeNode *>(
        minimum(const_cast<const RbTreeNode *>(node)));
}

inline
RbTreeNode *RbTreePrimitives::maximum(RbTreeNode *node)
{
    return const_cast<RbTreeNode *>(
        maximum(const_cast<const RbTreeNode *>(node)));
}

inline
RbTreeNode *RbTreePrimitives::next(RbTreeNode *node)
{
    return const_cast<RbTreeNode *>(
        next(const_cast<const RbTreeNode *>(node)));
}

inline
RbTreeNode *RbTreePrimitives::previous(RbTreeNode *node)
{
    return const_cast<RbTreeNode *>(
        previous(const_cast<const RbTreeNode *>(node)));
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
inline
RbTreeNode *RbTreePrimitives::find(RbTreeNode                   *node,
                                   const NODE_VALUE_COMPARATOR&  comparator,
                                   const VALUE&                  value)
{
    return const_cast<RbTreeNode *>(
        find(const_cast<const RbTreeNode *>(node), comparator, value));
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
inline
const RbTreeNode *RbTreePrimitives::find(
                                      const RbTreeNode             *node,
                                      const NODE_VALUE_COMPARATOR&  comparator,
                                      const VALUE&                  value)
{
    typedef const typename NODE_VALUE_COMPARATOR::NodeType NodeType;

    const RbTreeNode *lowBound = lowerBound(node, comparator, value);
    return (lowBound && !comparator(value, static_cast<NodeType&>(*lowBound)))
          ? lowBound
          : 0;
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
inline
RbTreeNode *RbTreePrimitives::lowerBound(
                                      RbTreeNode                   *node,
                                      const NODE_VALUE_COMPARATOR&  comparator,
                                      const VALUE&                  value)
{
    return const_cast<RbTreeNode *>(
        lowerBound(const_cast<const RbTreeNode *>(node), comparator, value));
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
inline
const RbTreeNode *RbTreePrimitives::lowerBound(
                                      const RbTreeNode             *node,
                                      const NODE_VALUE_COMPARATOR&  comparator,
                                      const VALUE&                  value)
{
    typedef const typename NODE_VALUE_COMPARATOR::NodeType NodeType;

    const RbTreeNode *nextLargestNode = 0;
    while (node) {
        if (comparator(static_cast<NodeType&>(*node), value)) {
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
RbTreeNode *RbTreePrimitives::upperBound(
                                      RbTreeNode                   *node,
                                      const NODE_VALUE_COMPARATOR&  comparator,
                                      const VALUE&                  value)
{
    return const_cast<RbTreeNode *>(
        upperBound(const_cast<const RbTreeNode *>(node), comparator, value));
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
inline
const RbTreeNode *RbTreePrimitives::upperBound(
                                      const RbTreeNode             *node,
                                      const NODE_VALUE_COMPARATOR&  comparator,
                                      const VALUE&                  value)
{
    typedef const typename NODE_VALUE_COMPARATOR::NodeType NodeType;

    const RbTreeNode *nextLargestNode = 0;
    while (node) {
        if (comparator(value, static_cast<NodeType&>(*node))) {
            nextLargestNode = node;
            node = node->leftChild();
        }
        else {
            node = node->rightChild();
        }
    }
    return nextLargestNode;
}

template <typename NODE_COPY_FUNCTION, typename NODE_DELETE_FUNCTION>
RbTreeNode *RbTreePrimitives::copyTree(
                               const RbTreeNode            *original,
                               const NODE_COPY_FUNCTION&    nodeCopyFunction,
                               const NODE_DELETE_FUNCTION&  nodeDeleteFunction)
{
#warning TBD: Use a proctor to ensure exception safety.
    BSLS_ASSERT_OPT(0 != &nodeDeleteFunction); // suppress warning, TBD: remove

    if (0 == original) {
        return 0;                                                     // RETURN
    }

    typedef typename NODE_COPY_FUNCTION::NodeType NodeType;

    // Perform an pre-order traversal of the nodes of the tree, invoking the
    // 'nodeCopyFunction' on each node.

    const RbTreeNode *originalNode = original;

    RbTreeNode *copiedRoot = nodeCopyFunction(
                                  static_cast<const NodeType&>(*originalNode));
    RbTreeNode *copiedNode = copiedRoot;

    copiedNode->setColor(originalNode->color());
    copiedNode->setParent(0);
    copiedNode->setLeftChild(0);
    copiedNode->setRightChild(0);
    do {
        if (0 != originalNode->leftChild() && 0 == copiedNode->leftChild()) {
            originalNode = originalNode->leftChild();
            RbTreeNode *newNode = nodeCopyFunction(
                                  static_cast<const NodeType&>(*originalNode));
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
            RbTreeNode *newNode = nodeCopyFunction(
                                  static_cast<const NodeType&>(*originalNode));
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
    } while (originalNode);
    return copiedRoot;
}

template <class NODE_DELETE_FUNCTION>
void RbTreePrimitives::deleteTree(
                               RbTreeNode                  *rootNode,
                               const NODE_DELETE_FUNCTION&  nodeDeleteFunction)
{
    if (0 == rootNode) {
        return;                                                       // RETURN
    }

    typedef typename NODE_DELETE_FUNCTION::NodeType NodeType;

    // Perform a post-order traversal of the nodes of the tree, invoking the
    // 'nodeDeleteFunction' on each node.

    RbTreeNode *node = minimum(rootNode);
    do {
        // At each iteration through this loop, we are at a mimimum (i.e.,
        // left most) node of some sub-tree.  Note that 'node->leftChild()'
        // may not be 0 if we've simply deleted the left sub-tree of this node.

        if (node->rightChild()) {
            // If this node has a right child, then navigate to the first node
            // in the subtree to remove, and set 'node->rightChild()' to 0 so
            // we know this sub-tree has been removed when we iterate back up
            // parent pointers of the tree to this node.

            RbTreeNode *rightChild = node->rightChild();
            node->setRightChild(0);
            node = minimum(rightChild);
        }
        else {
            RbTreeNode *parent = node->parent();
            nodeDeleteFunction(static_cast<NodeType *>(node));
            node = parent;
        }
    } while (node);
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
RbTreeNode *RbTreePrimitives::findInsertLocation(
                                     bool                         *isLeftChild,
                                     RbTreeNode                   *rootNode,
                                     const NODE_VALUE_COMPARATOR&  comparator,
                                     const VALUE&                  value)
{
    typedef typename NODE_VALUE_COMPARATOR::NodeType NodeType;

    RbTreeNode *parent = 0;
    RbTreeNode *node   = rootNode;
    while (node) {
        // Find the leaf node that would be the parent of 'newNode'.

        parent = node;
        *isLeftChild = comparator(value, static_cast<NodeType&>(*node));
        if (*isLeftChild) {
            node = node->leftChild();
        }
        else {
            node = node->rightChild();
        }
    }
    return parent;
}

template <class NODE_VALUE_COMPARATOR, class VALUE>
RbTreeNode *RbTreePrimitives::findUniqueInsertLocation(
                                   bool                         *isUniqueValue,
                                   bool                         *isLeftChild,
                                   RbTreeNode                   *rootNode,
                                   const NODE_VALUE_COMPARATOR&  comparator,
                                   const VALUE&                  value)
{
    typedef typename NODE_VALUE_COMPARATOR::NodeType NodeType;

    // Seperate out the de-generate case to reduce the number of comparisons
    // required when providing return values.

    if (!rootNode) {
        *isUniqueValue = true;
        return 0;                                                     // RETURN
    }

    // Note that 'nextSmallestNode' is used, rather than 'nextLargestNode' (as
    // seen in 'upperBound' and 'lowerBound') to avoid an unneccesary
    // negation.  Using 'nextLargestNode' would require either the comparison
    // '*isLeftChild = !comparator(*node, value)') or a changing 'isLeftNode' 
    // to be 'isRightNode'.

    RbTreeNode *parent           = 0;
    RbTreeNode *nextSmallestNode = 0;
    RbTreeNode *node             = rootNode;
    do {
        // Find the leaf node that would be the parent of 'newNode'.

        parent = node;
        *isLeftChild = comparator(value, static_cast<NodeType&>(*node));
        if (*isLeftChild) {
            node = node->leftChild();
        }
        else {
            nextSmallestNode = node;
            node = node->rightChild();
        }
    } while (node);

    *isUniqueValue =
        !nextSmallestNode ||
        comparator(static_cast<NodeType&>(*nextSmallestNode), value);

    return *isUniqueValue ? parent : nextSmallestNode;
}


template <class NODE_COMPARATOR>
RbTreeNode *RbTreePrimitives::insert(RbTreeNode             *rootNode,
                                     const NODE_COMPARATOR&  comparator,
                                     RbTreeNode             *newNode)
{
    typedef typename NODE_COMPARATOR::NodeType NodeType;

    // Note that the following logic is the same as 'findInsertLocation'
    // except that the comparator required for this operation compares two
    // nodes, rather than comparing a value to a node.

    RbTreeNode *parent = 0;
    RbTreeNode *node   = rootNode;
    bool        isLeftChild;
    while (node) {
        // Find the leaf node that would be the parent of 'newNode'.

        parent = node;
        isLeftChild = comparator(static_cast<NodeType&>(*newNode),
                                 static_cast<NodeType&>(*node));
        if (isLeftChild) {
            node = node->leftChild();
        }
        else {
            node = node->rightChild();
        }
    }
    return insertAt(rootNode, parent, isLeftChild, newNode);
}

inline
bool RbTreePrimitives::isLeftChild(const RbTreeNode *node)
{
    BSLS_ASSERT_SAFE(node);
    BSLS_ASSERT_SAFE(node->parent());

    return node->parent()->leftChild() == node;
}

inline
bool RbTreePrimitives::isRightChild(const RbTreeNode *node)
{
    BSLS_ASSERT_SAFE(node);
    BSLS_ASSERT_SAFE(node->parent());

    return node->parent()->rightChild() == node;
}

template <class NODE_COMPARATOR>
inline
int RbTreePrimitives::validateRbTree(const RbTreeNode       *node,
                                     const NODE_COMPARATOR&  comparator)
{
    const RbTreeNode *errorNode;
    const char       *errorDescription;
    return validateRbTree(&errorNode, &errorDescription, node, comparator);
}

template <class NODE_COMPARATOR>
int RbTreePrimitives::validateRbTree(const RbTreeNode       **errorNode,
                                     const char             **errorDescription,
                                     const RbTreeNode        *node,
                                     const NODE_COMPARATOR&   comparator)
{
    return RbTreePrimitives_Validator::validateRbTree(errorNode,
                                                      errorDescription,
                                                      node,
                                                      0,
                                                      0,
                                                      comparator);
}

                      // --------------------------------
                      // class RbTreePrimitives_Validator
                      // --------------------------------

// CLASS METHODS
template <class NODE_COMPARATOR>
int RbTreePrimitives_Validator::validateRbTree(
                                     const RbTreeNode       **errorNode,
                                     const char             **errorDescription,
                                     const RbTreeNode        *rootNode,
                                     const RbTreeNode        *minNodeValue,
                                     const RbTreeNode        *maxNodeValue,
                                     const NODE_COMPARATOR&   comparator)
{
    typedef typename NODE_COMPARATOR::NodeType NodeType;

    enum { INVALID_RBTREE = -1};

    // 'NIL' nodes are considered black

    if (0 == rootNode) {
        return 0;                                                     // RETURN
    }

    // Rule 1.

    if ((minNodeValue &&
              comparator(static_cast<const NodeType&>(*rootNode), 
                         static_cast<const NodeType&>(*minNodeValue))) ||
        (maxNodeValue && 
              comparator(static_cast<const NodeType&>(*maxNodeValue), 
                         static_cast<const NodeType&>(*rootNode)))) {
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

}  // close namespace bslalg
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
