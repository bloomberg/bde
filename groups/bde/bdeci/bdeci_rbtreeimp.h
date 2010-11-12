// bdeci_rbtreeimp.h                -*-C++-*-
#ifndef INCLUDED_BDECI_RBTREEIMP
#define INCLUDED_BDECI_RBTREEIMP

//@PURPOSE: Provide allocation & red-black tree access of uniform sized blocks.
//
//@CLASSES:
//           bdeci_RbTreeImp: red-black tree of managed memory
//       bdeci_RbTreeImpIter: read-only accessor of red-black tree nodes
//      bdeci_RbTreeImpManip: sequential selective inserter/remover of nodes
// bdeci_RbTreeImpBatchManip: sequential selective inserter/remover of nodes
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@SEE_ALSO: bdeci_rbtree
//
//@DESCRIPTION: This component implements an ordered memory pool that allocates
// blocks of some uniform size specified at construction and organizes the
// allocated blocks into a red-black tree.  A red-black tree is a binary tree
// with the following properties:
//..
//      (1) Every node in the tree is colored either *red* or *black*.
//      (2) The root node is black.
//      (3) Leaf (nil) nodes are black.
//      (4) If a node is red, then both of its children are black.
//      (5) For each node, the path to each descendent leaf node contains the
//          same number of black nodes.
//..
// An especially attractive property of red-black trees, which may be derived
// from those above, is that they are balanced -- a red-black tree with 'N'
// internal nodes has height at most 2*log(N+1).  Consequently, red-black trees
// support very efficient insertion, retrieval, and removal operations.  See
// [Introduction to Algorithms; Cormen, Leiserson, Rivest, and Stein] for a
// detailed treatment of red-black trees.
//
// The 'bdeci_RbTreeImp' class manages the allocation and deallocation of
// memory used by the nodes of a red-black tree and organizes the outstanding
// nodes into a valid red-black tree (i.e., one that has the five properties
// above).  A node is colored when it is first allocated and inserted into a
// red-black tree.  Furthermore, when a node is inserted into, or removed from,
// a red-black tree, the 'bdeci_RbTreeImp' class performs any requisite
// rebalancing of the tree.  During rebalancing, one or more nodes may have
// their color toggled (from red to black or vice versa).  Any rebalancing
// and/or recoloration performed by 'bdeci_RbTreeImp' is transparent to clients
// of the class.  Note that the coloration of the nodes in a red-black tree is
// used by the rebalancing algorithms to achieve the desired result.
//
// The 'bdeci_RbTreeImp' class is concerned with maintaining only the
// structural properties of red-black trees and the management of the memory
// used for the nodes.  It is the responsibility of client code to assign
// values to nodes (which are raw memory as far as 'bdeci_RbTreeImp' is
// concerned), to insert nodes into their appropriate positions in a red-black
// tree, and to invoke destructors on the nodes, as necessary, when nodes are
// removed.
//
// The ordering of the nodes of a red-black tree is determined by the order in
// which the 'insertAsFirst' and 'insertAfter' methods are called (the 'remove'
// method has no effect on the relative order of the remaining nodes).  For a
// given value 'V' stored in a red-black tree node, a typical ordering will be
// such that the values in all nodes in the left subtree are "equal to" or
// "less than" 'V' and the values in all nodes in the right subtree are "equal
// to" or "greater than" 'V'.  Note that the precise meaning, or lack thereof,
// of the "less than", "equal to" and "greater than" relations depends on the
// values (and their type) that client code stores in the nodes of a red-black
// tree and the order in which they are inserted into the tree.
//
// The 'bdeci_RbTreeImpIter' class provides *read-only* access to the nodes of
// its associated red-black tree.  Instances of this iterator class may access
// these nodes sequentially (as in a linked-list traversal) or recursively (as
// in a binary-tree traversal).  An iteration may begin at any node in a
// red-black tree.  For sequential traversals, reachable nodes are the
// iterator's initial node and any successors of the initial node (all
// successor node are "equal to" or "greater than" the intial node).  For
// recursive traversals, reachable nodes are the initial node of the iterator
// and any nodes in the left and right subtrees of the initial node.
//
// The 'bdeci_RbTreeImpManip' and 'bdeci_RbTreeImpBatchManip' classes provide
// *modifiable* access to the nodes of an associated red-black tree.  Instances
// of these manipulator classes access the nodes of an associated red-black
// tree sequentially and may selectively insert new nodes and remove existing
// nodes as the nodes of the tree are visited.  These two manipulators
// differ in their performance characteristics, but their interfaces are
// identical.  See the documentation of the individual classes for details.
//
///PERFORMANCE
///-----------
// The following characterizes the performance of representative operations
// using big-oh notation, O[f(N,M)], where the object names 'N' and 'M' also
// refer to the number of respective elements in each container (i.e., its
// 'length()').  'K' refers to a positive integer.
//..
//      Operation                  Worst Case
//      ---------                  ----------
//      DEFAULT CTOR               O[1]
//      N.DTOR()                   O[1]
//
//      N.insertAfter(node)        O[log(N)]
//      N.insertAsFirst()          O[log(N)]
//      N.remove(node, previous)   O[log(N)]
//
//      N.length()                 O[1]
//
//      K x N.insertAfter(node)    O[K * log(N)] N is bdeci_RbTreeImpManip
//      K x N.insertAfter(node)    O[K + N]      N is bdeci_RbTreeImpBatchManip
//..
//
///USAGE
///-----
// The following sample code illustrates how to use 'bdeci_RbTreeImp', as
// well as its friends, 'bdeci_RbTreeImpIter', 'bdeci_RbTreeImpManip' and
// 'bdeci_RbTreeImpBatchManip'.  First we declare a class, 'my_SortedInts',
// that has a 'bdeci_RbTreeImp' as its only data member.  A 'my_SortedInts'
// object manages a sorted collection of 'int' values using the contained
// red-black tree to hold the values.
//..
//      // sortedints.h
//
//      class my_SortedInts {
//          // This class is a container for a sorted collection of 'int'
//          // values.  The values are sorted in ascending order.  Duplicates
//          // are allowed.
//
//          bdeci_RbTreeImp d_tree;   // Red-black tree container of values.
//
//          // PRIVATE METHODS
//          void printTreeImp(ostream&                   stream,
//                            const bdeci_RbTreeImpIter& iterator,
//                            int                        depth) const;
//              // Recursively output the sorted collection of integers in the
//              // sub-tree associated with the specified 'iterator' to the
//              // specified 'stream'.  Output one value per line with the
//              // value indented by the specified 'depth' number of spaces to
//              // indicate the depth of the value in the containing red-black
//              // tree.  Each line is prefixed with an initial "T" to
//              // indicate that the collection is output in tree format.
//
//        public:
//          // CREATORS
//          my_SortedInts();
//              // Create a sorted collection of integers that is initially
//              // empty.
//
//          ~my_SortedInts();
//              // Destroy this sorted collection of integers.
//
//          // MANIPULATORS
//          void insert(int value);
//              // Insert into this sorted collection of integers the specified
//              // 'value' in O[log(N)]-time.
//
//          void insert(const int *sortedArray, int numElements);
//              // Insert into this sorted collection of integers the specified
//              // 'numElements' values from the specified 'sortedArray'.  The
//              // behavior is undefined unless 0 <= numElements and
//              // 'sortedArray' is the address of an array containing
//              // 'numElements' values sorted in ascending order.
//
//          void removeEven();
//              // Remove the even values from this sorted collection of
//              // integers.
//
//          void removeOdd();
//              // Remove the odd values from this sorted collection of
//              // integers.
//
//          // ACCESSORS
//          int length();
//              // Return the number of elements in this collection.
//
//          void printList(ostream& stream) const;
//              // Output this sorted collection of integers to the specified
//              // 'stream'.  Output all values in ascending order on a single
//              // line.  Prefix the line with an initial "L" to indicate that
//              // the values are output in list format.
//
//          void printTree(ostream& stream) const;
//              // Output this sorted collection of integers to the specified
//              // 'stream'.  Output one value per line with the value indented
//              // by an amount proportional to the value's depth in the
//              // containing red-black tree.  Prefix each line with an initial
//              // "T" to indicate that the collection is output in tree
//              // format.
//      };
//
//      // FREE OPERATORS
//      ostream& operator<<(ostream& stream, const my_SortedInts& rhs);
//          // Write the specified 'rhs' sorted collection of integers to the
//          // specified output 'stream' and return a reference to the
//          // modifiable 'stream'.  Output the values in the collection twice.
//          // First output the collection linearly (in list format); then
//          // output it hierarchically (in tree format).
//..
// The implementation of 'my_SortedInts' follows.
//..
//      // sortedints.cpp
//
//      // PRIVATE METHODS
//      void my_SortedInts::printTreeImp(
//                                      ostream&                   stream,
//                                      const bdeci_RbTreeImpIter& iterator,
//                                      int                        depth) const
//      {
//          enum { INDENT = 2 };
//          if (iterator) {
//              stream << 'T';
//              int n = depth;
//              while (n-- > 0) {
//                  stream << ' ';
//              }
//              stream << *(int *)iterator() << endl;
//
//              if (iterator.hasLeft()) {
//                  bdeci_RbTreeImpIter left(iterator());
//                  left.moveLeft();
//                  printTreeImp(stream, left, depth + INDENT);
//              }
//              if (iterator.hasRight()) {
//                  bdeci_RbTreeImpIter right(iterator());
//                  right.moveRight();
//                  printTreeImp(stream, right, depth + INDENT);
//              }
//          }
//      }
//
//      // CREATORS
//      my_SortedInts::my_SortedInts() : d_tree(sizeof(int)) {}
//
//      my_SortedInts::~my_SortedInts() {}
//
//      // MANIPULATORS
//      void my_SortedInts::insert(int value)
//      {
//          bdeci_RbTreeImpIter it(d_tree);
//          bdeci_RbTreeImpIter prev;       // Created in "unassociated" state.
//
//          while (it) {          // Locate position to insert 'value'.
//              const int x = *(int *)it();
//              if (x > value) {
//                  it.moveLeft();
//              }
//              else {
//                  prev = it;
//                  if (x == value) {
//                      break;    // allow duplicates
//                  }
//                  else {
//                      it.moveRight();
//                  }
//              }
//          }
//
//          int *node;
//          if (!prev) {
//              node = (int *)d_tree.insertAsFirst();
//          }
//          else {
//              node = (int *)d_tree.insertAfter(prev());
//          }
//          *node = value;
//      }
//
//      void my_SortedInts::insert(const int *sortedArray, int numElements)
//      {
//          bdeci_RbTreeImpBatchManip manip(&d_tree);   // "batch" manipulator
//
//          for (int i = 0; i < numElements; ++i) {
//              const int value = sortedArray[i];
//              while (manip) {    // Locate position to insert 'value'.
//                  const int x = *(int *)manip();
//                  if (x >= value) {
//                      break;
//                  }
//                  else {
//                      manip.advance();
//                  }
//              }
//
//              int *node = (int *)manip.insertBefore();
//              *node = value;
//          }
//      }
//
//      void my_SortedInts::removeOdd()
//      {
//          bdeci_RbTreeImpManip manip(&d_tree);    // Non-"batch" manipulator.
//
//          while (manip) {
//              const int x = *(int *)manip();
//              if (x % 2) {
//                  manip.remove();
//              }
//              else {
//                  manip.advance();
//              }
//          }
//      }
//
//      // ACCESSORS
//      int my_SortedInts::length()
//      {
//          return d_tree.length();
//      }
//
//      void my_SortedInts::printList(ostream& stream) const
//      {
//          bdeci_RbTreeImpIter it(d_tree.first());
//
//          stream << "L";
//          while (it) {
//              stream << *(int *)it() << ' ';
//              it.moveNext();
//          }
//          stream << endl;
//      }
//
//      void my_SortedInts::printTree(ostream& stream) const
//      {
//          printTreeImp(stream, bdeci_RbTreeImpIter(d_tree.root()), 0);
//      }
//
//      // FREE OPERATORS
//      ostream& operator<<(ostream& stream, const my_SortedInts& rhs)
//      {
//          rhs.printList(stream);
//          rhs.printTree(stream);
//
//          return stream;
//      }
//..
// In the following, we declare a 'my_SortedInts' object 'x' and confirm that
// it is initially empty.  The 'my_SortedInts' constructor initializes the
// 'bdeci_RbTreeImp' data member to manage blocks of size 'sizeof(int)' bytes.
//  Our sample class uses the default memory allocator.
//..
//      my_SortedInts x;          assert(0 == x.length());
//..
// Next, the one-argument 'insert' method is used to populate 'x' with some
// values.  The 'insert' method employs an iterator to locate the position in
// the red-black tree in which to place the new value.  If the tree is empty
// or if the value being inserted is less than all values currently stored in
// the tree, then 'insertAsFirst' does the insertion; otherwise, 'insertAfter'
// does the job.
//..
//      x.insert(4);           assert(1 == x.length());
//      x.insert(6);           assert(2 == x.length());
//      x.insert(6);           assert(3 == x.length());
//      x.insert(3);           assert(4 == x.length());
//      x.insert(2);           assert(5 == x.length());
//      x.insert(17);          assert(6 == x.length());
//      x.insert(5);           assert(7 == x.length());
//..
// When we print 'x':
//..
//      cout << x;
//..
// the following is written to 'stdout':
//..
//      L2 3 4 5 6 6 17
//      T6
//      T  3
//      T    2
//      T    4
//      T      5
//      T  6
//      T    17
//..
// The 'printList' method uses an iterator to visit each of the values in our
// collection sequentially via the 'moveNext' method.  In contrast, the
// 'printTree' method uses an iterator to traverse the red-black tree
// recursively via the 'moveLeft' and 'moveRight' methods.
//
// The two-argument 'insert' method illustrates use of a
// 'bdeci_RbTreeImpBatchManip' manipulator.  We'll add more values to our
// collection using this 'insert' method and print the collection again.  As in
// the other 'insert' method, the correct position in which to place the new
// value must be determined.  Note that each of the two manipulator classes
// iterate over the nodes in our red-black tree sequentially via the 'advance'
// method.
//..
//      const int V[] = { 3, 8, 9, 12 };
//      x.insert(V, sizeof V / sizeof *V);       assert(11 == x.length());
//      cout << x;
//..
// The following is written to 'stdout':
//..
//      L2 3 3 4 5 6 6 8 9 12 17
//      T4
//      T  3
//      T    2
//      T    3
//      T  8
//      T    6
//      T      5
//      T      6
//      T    12
//      T      9
//      T      17
//..
// Finally we remove the odd values from 'x' and print the collection one last
// time.  The 'removeOdd' method illustrates a very simple application of the
// 'bdeci_RbTreeImpManip' manipulator.
//..
//      x.removeOdd();           assert(6 == x.length());
//      cout << x;
//..
// We now get the following output:
//..
//      L2 4 6 6 8 12
//      T8
//      T  4
//      T    2
//      T    6
//      T      6
//      T  12
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_POOL
#include <bdema_pool.h>
#endif

#ifndef INCLUDED_BDES_ALIGNMENT
#include <bdes_alignment.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif



namespace BloombergLP {



class bdema_Allocator;
class bdeci_RbTreeImpIter;
class bdeci_RbTreeImpManip;
class bdeci_RbTreeImpBatchManip;



                        // ===========================
                        // struct bdeci_RbTreeImp_Node
                        // ===========================

struct bdeci_RbTreeImp_Node {
    // This struct implements a node of a red-black tree.

    enum Color { BDECI_RED = 0, BDECI_BLACK = 1 };

    bdeci_RbTreeImp_Node  *d_parent_p;  // pointer to parent, or 0 if root
    bdeci_RbTreeImp_Node  *d_left_p;    // left child
    bdeci_RbTreeImp_Node  *d_right_p;   // right child
    bdeci_RbTreeImp_Node  *d_next_p;    // next Node in sequence
    Color                  d_color;     // BDECI_RED or BDECI_BLACK
    bdes_Alignment::MaxAlignedType d_memory; // Force alignment; client's
                                             // memory starts here.
};

                        // =====================
                        // class bdeci_RbTreeImp
                        // =====================

class bdeci_RbTreeImp {
    // This class implements an ordered memory pool that allocates and manages
    // memory blocks of some uniform size specified at construction.  These
    // blocks are accessible as nodes of a red-black tree and as elements of
    // a singly-linked list.
    //
    // This container is *exception* *neutral* with no guarantee of rollback:
    // if an exception is thrown during the invocation of a method on a
    // pre-existing instance, the class is left in a valid state, but its
    // value is undefined.  In no event is memory leaked.  Finally, *aliasing*
    // (e.g., using all or part of an object as both source and destination) is
    // supported in all cases.

    bdeci_RbTreeImp_Node *d_root_p;       // pointer to root of tree
    bdeci_RbTreeImp_Node *d_first_p;      // pointer to first node
    int                   d_numElements;  // number of nodes in tree
    bdema_Pool            d_pool;         // pool of preallocated nodes

    // FRIEND CLASSES
    friend class bdeci_RbTreeImpIter;
    friend class bdeci_RbTreeImpManip;
    friend class bdeci_RbTreeImpBatchManip;

    // NOT IMPLEMENTED
    bdeci_RbTreeImp(const bdeci_RbTreeImp&);
    bdeci_RbTreeImp& operator=(const bdeci_RbTreeImp&);

  private:
    // PRIVATE MANIPULATORS
    void listToTree();
        // Transform the sequence of ordered memory blocks managed by this
        // object from a singly-linked list into a valid red-black tree.  The
        // tree nodes remain accessible as a singly-linked list.  Note that
        // this method is an implementation detail of
        // 'bdeci_RbTreeImpBatchManip'.

    bdeci_RbTreeImp_Node *insertAfter(bdeci_RbTreeImp_Node *node);
        // Allocate a memory block located immediately after the specified
        // 'node' of this red-black tree and return the address of the new
        // modifiable block.

    void remove(bdeci_RbTreeImp_Node *node);
        // Remove the specified 'node' from the sequence of memory blocks
        // managed by this red-black tree and deallocate its memory, making it
        // available for future use.

    void remove(bdeci_RbTreeImp_Node *node, bdeci_RbTreeImp_Node *previous);
        // Remove the specified 'node', whose predecessor is the specified
        // 'previous' node, from the sequence of memory blocks managed by this
        // red-black tree and deallocate its memory, making it available for
        // future use.  The behavior is undefined unless either 'node' is first
        // in this red-black tree and 'previous' is 0 or 'previous' is the
        // predecessor of 'node'.  Note that given a node N having predecessor
        // P (possibly null), the following:
        //..
        //      remove(N, P);
        //..
        // is equivalent to:
        //..
        //      remove(N);
        //..
        // but the former is more efficient.

  public:
    // TYPES
    struct InitialCapacity { int d_i; InitialCapacity(int i) : d_i(i) { }
                                     ~InitialCapacity() { } };
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in nodes).  For example:
        //..
        //      bdeci_RbTreeImp x(bdeci_RbTreeImp::InitialCapacity(8));
        //..
        // defines an instance, 'x', with an initial capacity of 8 nodes, but
        // with 0 logical number of nodes.

    // CREATORS
    explicit bdeci_RbTreeImp(int              numBytes,
                             bdema_Allocator *basicAllocator = 0);
        // Create an ordered memory pool that returns memory of the specified
        // 'numBytes' for each 'insertAsFirst' and 'insertAfter' method
        // invocation.  Optionally specify 'basicAllocator' to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  The behavior is undefined unless 0 < numBytes.  Note that
        // when a node is inserted and no free nodes are available, this pool
        // automatically increases its capacity according to an implementation-
        // defined strategy.

    bdeci_RbTreeImp(int                     numBytes,
                    const InitialCapacity&  numElements,
                    bdema_Allocator        *basicAllocator = 0);
        // Create an ordered memory pool that returns memory of the specified
        // 'numBytes' for each 'insertAsFirst' and 'insertAfter' method
        // invocation and has sufficient initial capacity to accommodate up to
        // the specified 'numElements' values without subsequent reallocation.
        // Optionally specify 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 0 < numBytes.  Note that
        // when a node is inserted and no free nodes are available, this pool
        // automatically increases its capacity according to an implementation-
        // defined strategy.

    ~bdeci_RbTreeImp();
        // Destroy this object and release all memory allocated through this
        // object.

    // MANIPULATORS
    void *insertAfter(const void *node);
        // Allocate a memory block stored immediately after the specified
        // 'node' of this red-black tree and return the address of the new
        // block.

    void *insertAsFirst();
        // Allocate a memory block stored as the first node of this red-black
        // tree and return the address of the new block.

    void remove(const void *node);
        // Remove the specified 'node' from the sequence of memory blocks
        // managed by this red-black tree and deallocate its memory, making it
        // available for future use.  The behavior is undefined unless 'node'
        // is in this tree.

    void remove(const void *node, const void *previous);
        // Remove the specified 'node', whose predecessor is the specified
        // 'previous' node, from the sequence of memory blocks managed by this
        // red-black tree and deallocate its memory, making it available for
        // future use.  The behavior is undefined unless either 'node' is first
        // in this tree and 'previous' is 0 or 'previous' is the predecessor of
        // 'node'.  Note that given a node N having predecessor P (possibly
        // null), the following:
        //..
        //      remove(N, P);
        //..
        // is equivalent to:
        //..
        //      remove(N);
        //..
        // but the former is more efficient.

    void removeAll();
        // Release all memory currently allocated through this object.

    void reserveCapacity(int numElements);
        // Reserve sufficient internal capacity to accommodate up to the
        // specified 'numElements' memory blocks without reallocation.  The
        // behavior is undefined unless 0 <= numElements.

    // ACCESSORS
    const void *first() const;
        // Return the address of the memory block associated with the first
        // node of this red-black tree or 0 if the tree is empty.

    const void *root() const;
        // Return the address of the memory block associated with the root
        // node of this red-black tree or 0 if the tree is empty.

    int length() const;
        // Return the number of nodes in this red-black tree.

    std::ostream& print(std::ostream&  stream,
                        int            level,
                        int            spacesPerLevel) const;
        // Format this red-black tree to the specified output 'stream' at the
        // (absolute value of the) specified indentation 'level' using the
        // specified 'spacesPerLevel' of indentation and return a reference to
        // the modifiable 'stream'.  Making 'level' negative suppresses
        // indentation for the first line only.  The behavior is undefined
        // unless 0 <= spacesPerLevel.  For example, with level = 1 and
        // spacesPerLevel = 1, a red-black tree consisting of a single node is
        // output as follows:
        //..
        //      b
        //        nil
        //        nil
        //..
        // Note that "r" signifies a red node, "b" signifies a black, non-leaf
        // node, and "nil" signifies a leaf (nil) node.
};

// FREE OPERATORS
std::ostream& operator<<(std::ostream& stream, const bdeci_RbTreeImp& rhs);
    // Write the specified 'rhs' red-black tree to the specified output
    // 'stream' in some reasonable (single-line) format, and return a
    // reference to the modifiable 'stream'.  Parentheses reflect the structure
    // of the underlying binary tree with "r" signifying a red node and "b"
    // signifying a black, non-leaf node.  For example, a red-black tree
    // consisting of a single node is output as follows:
    //..
    //      (b () ())
    //..
    // Note that "()" signifies a leaf (nil) node.

                        // =========================
                        // class bdeci_RbTreeImpIter
                        // =========================

class bdeci_RbTreeImpIter {
    // Provide read-only access to the nodes of a red-black tree.  The nodes
    // in the tree may be accessed sequentially as for a linked list
    // ('moveNext') or by the usual (recursive) traversal of a binary tree
    // ('moveLeft' and 'moveRight').  The iterator becomes invalid if any
    // manipulator method of the associated red-black tree, or of any
    // 'bdeci_RbTreeImpManip' or 'bdeci_RbTreeImpBatchManip' associated with
    // the same tree, is used.

    bdeci_RbTreeImp_Node *d_node_p;  // Current red-black tree node.

    // FRIENDS
    friend int operator==(const bdeci_RbTreeImpIter& lhs,
                          const bdeci_RbTreeImpIter& rhs);

  public:
    // CREATORS
    bdeci_RbTreeImpIter();
        // Create an iterator for an unspecified red-black tree.  Note that
        // this iterator is invalid at construction, but may subsequently be
        // assigned a valid position in some tree.

    explicit bdeci_RbTreeImpIter(const bdeci_RbTreeImp& tree);
        // Create an iterator for the specified red-black 'tree' initialized
        // to the root of 'tree'.

    explicit bdeci_RbTreeImpIter(const void *node);
        // Create an iterator for a red-black tree initialized to the specified
        // tree 'node'.  The behavior is undefined unless 'node' is the address
        // of a node in some 'bdeci_RbTreeImp' object.

    bdeci_RbTreeImpIter(const bdeci_RbTreeImpIter& original);
        // Create an iterator initialized to the value of the specified
        // 'original' iterator.

    ~bdeci_RbTreeImpIter();
        // Destroy this iterator.

    // MANIPULATORS
    bdeci_RbTreeImpIter& operator=(const bdeci_RbTreeImpIter& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator;
        // this iterator now accesses the same node in the same tree as 'rhs'.

    void moveLeft();
        // Advance this iterator to the left child of the current node in the
        // associated red-black tree.  The behavior is undefined unless the
        // current tree node is valid.  Note that if the left child is null,
        // then the current tree node for this iterator becomes invalid.

    void moveRight();
        // Advance this iterator to the right child of the current node in the
        // associated red-black tree.  The behavior is undefined unless the
        // current tree node is valid.  Note that if the right child is null,
        // then the current tree node for this iterator becomes invalid.

    void moveNext();
        // Advance this iterator to the successor of the current node in the
        // associated red-black tree.  The behavior is undefined unless the
        // current tree node is valid.  Note that if the successor is null,
        // then the current tree node for this iterator becomes invalid.

    // ACCESSORS
    operator const void *() const;
        // Return non-zero if the current node of the red-black tree
        // associated with this iterator is valid, and 0 otherwise.

    const void *operator()() const;
        // Return the address of the current node of the red-black tree
        // associated with this iterator.  The behavior is undefined unless
        // the current tree node is valid.

    int hasLeft() const;
        // Return 1 if the current node of the red-black tree associated with
        // this iterator has a left child, and 0 otherwise.  The behavior is
        // undefined unless the current tree node is valid.

    int hasRight() const;
        // Return 1 if the current node of the red-black tree associated with
        // this iterator has a right child, and 0 otherwise.  The behavior is
        // undefined unless the current tree node is valid.

    int isBlack() const;
        // Return 1 if the current node of the red-black tree associated with
        // this iterator is colored black, and 0 (red) otherwise.  The behavior
        // is undefined unless the current tree node is valid.

    int isRed() const;
        // Return 1 if the current node of the red-black tree associated with
        // this iterator is colored red, and 0 (black) otherwise.  The behavior
        // is undefined unless the current tree node is valid.
};

// FREE OPERATORS
inline
int operator==(const bdeci_RbTreeImpIter& lhs, const bdeci_RbTreeImpIter& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' iterators have the same value,
    // and 0 otherwise.  Two iterators have the same value if (1) both are
    // valid and access the same node of the same red-black tree or, (2) both
    // are invalid.

inline
int operator!=(const bdeci_RbTreeImpIter& lhs, const bdeci_RbTreeImpIter& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' iterators do not have the same
    // value, and 0 otherwise.  Two iterators do not have the same value if
    // (1) both are valid and access different red-black trees or access
    // different nodes of the same red-black tree or, (2) one is valid and the
    // other is invalid.

                       // ==========================
                       // class bdeci_RbTreeImpManip
                       // ==========================

class bdeci_RbTreeImpManip {
    // Provide sequential access, with selective insertion and removal
    // (filtering) capability, to nodes of a red-black tree.  This
    // manipulator becomes invalid if any manipulator method of the associated
    // red-black tree, or of any other 'bdeci_RbTreeImpManip' or
    // 'bdeci_RbTreeImpBatchManip' manipulator associated with the same tree,
    // is used during its lifetime.  Note that K insertions into a tree of size
    // N occur in O[K * log(N)]-time and the associated tree is maintained as a
    // valid red-black tree throughout the lifetime of the manipulator.  (See
    // 'bdeci_RbTreeImpBatchManip' for an alternative manipulator.)

    bdeci_RbTreeImp      *d_tree_p;  // ptr. to tree being manipulated
    bdeci_RbTreeImp_Node *d_node_p;  // ptr. to current  (initially = first)
    bdeci_RbTreeImp_Node *d_prev_p;  // ptr. to previous (initially = 0)

    // NOT IMPLEMENTED
    bdeci_RbTreeImpManip(const bdeci_RbTreeImpManip&);
    bdeci_RbTreeImpManip& operator=(const bdeci_RbTreeImpManip&);

  public:
    // CREATORS
    explicit bdeci_RbTreeImpManip(bdeci_RbTreeImp *tree);
        // Create a manipulator for the specified red-black 'tree' referencing
        // the first node of 'tree' if 'tree' is non-empty.  If 'tree' is
        // empty, there is no valid node and only 'insertBefore' manipulation
        // is initially possible.

    ~bdeci_RbTreeImpManip();
        // Destroy this manipulator.

    // MANIPULATORS
    void *operator()();
        // Return the address of the current node in the associated red-black
        // tree.  The behavior is undefined unless the current tree node for
        // this manipulator is valid.

    void advance();
        // Advance this manipulator to the successor of the current node in
        // the associated red-black tree.  The behavior is undefined unless the
        // current tree node for this manipulator is valid.  Note that if the
        // successor is null, then the current tree node for this manipulator
        // becomes invalid.

    void *insertAfter();
        // Allocate a memory block stored as the successor of the current
        // node in the associated red-black tree and return the address of the
        // new block.  The new node becomes the current tree node for this
        // manipulator.  The behavior is undefined unless the current tree node
        // is valid.

    void *insertBefore();
        // Allocate a memory block stored as the predecessor of the current
        // node in the associated red-black tree if the current node is valid,
        // and otherwise as the last node; return the address of the new block.
        // The new node becomes the current tree node for this manipulator.
        // Note that this method serves both (1) to insert a node into an empty
        // tree (in which case the last node is also the first node), and (2)
        // to append a node to the end of the ordered sequence of nodes
        // comprising the associated red-black tree.

    void remove();
        // Remove and deallocate the memory for the current node of this
        // manipulator from the sequence of memory blocks managed by the
        // associated red-black tree.  The successor of the removed node
        // becomes the current node for this manipulator.  The behavior is
        // undefined unless the current tree node is valid.

    // ACCESSORS
    operator const void *() const;
        // Return non-zero if the current node of the red-black tree
        // associated with this manipulator is valid, and 0 otherwise.

    const void *operator()() const;
        // Return the address of the current node of the red-black tree
        // associated with this manipulator.  The behavior is undefined unless
        // the current tree node for this manipulator is valid.
};

                      // ===============================
                      // class bdeci_RbTreeImpBatchManip
                      // ===============================

class bdeci_RbTreeImpBatchManip {
    // Provide sequential access, with selective insertion and removal
    // (filtering) capability, to nodes of a red-black tree.  This batch
    // manipulator becomes invalid if any manipulator method of the associated
    // red-black tree, or of any other 'bdeci_RbTreeImpManip' or
    // 'bdeci_RbTreeImpBatchManip' manipulator associated with the same tree,
    // is used during its lifetime.  Note that K insertions into a tree of size
    // N occur in O[K + N]-time, but the associated tree is *not* maintained as
    // a valid red-black tree during the lifetime of the manipulator.  When
    // the manipulator is destroyed, the associated tree is transformed back
    // into a valid red-black tree.  (See 'bdeci_RbTreeImpManip' for an
    // alternative manipulator.)

    bdeci_RbTreeImp      *d_tree_p;  // ptr. to tree being manipulated
    bdeci_RbTreeImp_Node *d_node_p;  // ptr. to current  (initially = first)
    bdeci_RbTreeImp_Node *d_prev_p;  // ptr. to previous (initially = 0)

    // NOT IMPLEMENTED
    bdeci_RbTreeImpBatchManip(const bdeci_RbTreeImpBatchManip&);
    bdeci_RbTreeImpBatchManip& operator=(const bdeci_RbTreeImpBatchManip&);

  public:
    // CREATORS
    explicit bdeci_RbTreeImpBatchManip(bdeci_RbTreeImp *tree);
        // Create a batch manipulator for the specified red-black 'tree'
        // referencing the first node of 'tree' if 'tree' is non-empty.  If
        // 'tree' is empty, there is no valid node and only 'insertBefore'
        // manipulation is initially possible.

    ~bdeci_RbTreeImpBatchManip();
        // Destroy this batch manipulator.  As a side-effect, the tree
        // associated with this batch manipulator is transformed back into a
        // valid red-black tree.  (See the 'listToTree' method of
        // 'bdeci_RbTreeImp'.)

    // MANIPULATORS
    void *operator()();
        // Return the address of the current node in the associated red-black
        // tree.  The behavior is undefined unless the current tree node for
        // this batch manipulator is valid.

    void advance();
        // Advance this batch manipulator to the successor of the current
        // node in the associated red-black tree.  The behavior is undefined
        // unless the current tree node for this batch manipulator is valid.
        // Note that if the successor of the current tree node is null, then
        // the current tree node for this batch manipulator becomes invalid.

    void *insertAfter();
        // Allocate a memory block stored as the successor of the current node
        // in the associated red-black tree and return the address of the new
        // block.  The new node becomes the current tree node for this batch
        // manipulator.  The behavior is undefined unless the current tree node
        // is valid.

    void *insertBefore();
        // Allocate a memory block stored as the predecessor of the current
        // node in the associated red-black tree if the current node is valid,
        // and otherwise as the last node; return the address of the new block.
        // The new node becomes the current tree node for this batch
        // manipulator.  Note that this method serves both (1) to insert a node
        // into an empty tree (in which case the last node is also the first
        // node) and, (2) to append a node to the end of the ordered sequence
        // of nodes comprising the associated red-black tree.

    void remove();
        // Remove and deallocate the memory for the current node of this batch
        // manipulator from the sequence of memory blocks managed by the
        // associated red-black tree.  The successor of the removed node
        // becomes the current node for this batch manipulator.  The behavior
        // is undefined unless the current tree node is valid.

    // ACCESSORS
    operator const void *() const;
        // Return non-zero if the current node of the red-black tree
        // associated with this batch manipulator is valid, and 0 otherwise.

    const void *operator()() const;
        // Return the address of the current node of the red-black tree
        // associated with this batch manipulator.  The behavior is undefined
        // unless the current tree node for this batch manipulator is valid.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                           // ---------------
                           // bdeci_RbTreeImp
                           // ---------------

// CREATORS

inline
bdeci_RbTreeImp::bdeci_RbTreeImp(int              numBytes,
                                 bdema_Allocator *basicAllocator)
: d_root_p(0)
, d_first_p(0)
, d_numElements(0)
, d_pool(numBytes < bdes_Alignment::MAX_ALIGNMENT
                 ? sizeof(bdeci_RbTreeImp_Node)
                 : sizeof(bdeci_RbTreeImp_Node) - bdes_Alignment::MAX_ALIGNMENT
                                                + numBytes, basicAllocator)
{
}

inline
bdeci_RbTreeImp::bdeci_RbTreeImp(int                     numBytes,
                                 const InitialCapacity&  numElements,
                                 bdema_Allocator        *basicAllocator)
: d_root_p(0)
, d_first_p(0)
, d_numElements(0)
, d_pool(numBytes < bdes_Alignment::MAX_ALIGNMENT
                 ? sizeof(bdeci_RbTreeImp_Node)
                 : sizeof(bdeci_RbTreeImp_Node) - bdes_Alignment::MAX_ALIGNMENT
                                                + numBytes, basicAllocator)
{
    d_pool.reserveCapacity(numElements.d_i);
}

inline
bdeci_RbTreeImp::~bdeci_RbTreeImp()
{
}

// MANIPULATORS

inline
void bdeci_RbTreeImp::remove(const void *node)
{
    remove((bdeci_RbTreeImp_Node *)((char *)node
              - sizeof(bdeci_RbTreeImp_Node) + bdes_Alignment::MAX_ALIGNMENT));
}

inline
void bdeci_RbTreeImp::remove(const void *node, const void *previous)
{
    remove((bdeci_RbTreeImp_Node *)((char *)node
               - sizeof(bdeci_RbTreeImp_Node) + bdes_Alignment::MAX_ALIGNMENT),
            previous ? (bdeci_RbTreeImp_Node *)((char *)previous
                - sizeof(bdeci_RbTreeImp_Node) + bdes_Alignment::MAX_ALIGNMENT)
                     : 0);
}

inline
void bdeci_RbTreeImp::removeAll()
{
    d_pool.release();
    d_root_p = 0;
    d_first_p = 0;
    d_numElements = 0;
}

inline
void bdeci_RbTreeImp::reserveCapacity(int numElements)
{
    if (numElements > d_numElements) {
        d_pool.reserveCapacity(numElements - d_numElements);
    }
}

// ACCESSORS

inline
const void *bdeci_RbTreeImp::first() const
{
    return d_first_p ? &d_first_p->d_memory : 0;
}

inline
int bdeci_RbTreeImp::length() const
{
    return d_numElements;
}

inline
const void *bdeci_RbTreeImp::root() const
{
    return d_root_p ? &d_root_p->d_memory : 0;
}

                           // -------------------
                           // bdeci_RbTreeImpIter
                           // -------------------

// CREATORS

inline
bdeci_RbTreeImpIter::bdeci_RbTreeImpIter()
: d_node_p(0)
{
}

inline
bdeci_RbTreeImpIter::bdeci_RbTreeImpIter(const bdeci_RbTreeImp& tree)
: d_node_p(tree.d_root_p)
{
}

inline
bdeci_RbTreeImpIter::bdeci_RbTreeImpIter(const void *node)
: d_node_p(node ? (bdeci_RbTreeImp_Node *)((char *)node
                - sizeof(bdeci_RbTreeImp_Node) + bdes_Alignment::MAX_ALIGNMENT)
                : 0)
{
}

inline
bdeci_RbTreeImpIter::bdeci_RbTreeImpIter(const bdeci_RbTreeImpIter& iter)
: d_node_p(iter.d_node_p)
{
}

inline
bdeci_RbTreeImpIter::~bdeci_RbTreeImpIter()
{
}

// MANIPULATORS

inline
bdeci_RbTreeImpIter& bdeci_RbTreeImpIter::operator=(
                                                const bdeci_RbTreeImpIter& rhs)
{
    d_node_p = rhs.d_node_p;
    return *this;
}

inline
void bdeci_RbTreeImpIter::moveLeft()
{
    d_node_p = d_node_p->d_left_p;
}

inline
void bdeci_RbTreeImpIter::moveRight()
{
    d_node_p = d_node_p->d_right_p;
}

inline
void bdeci_RbTreeImpIter::moveNext()
{
    d_node_p = d_node_p->d_next_p;
}

// ACCESSORS

inline
bdeci_RbTreeImpIter::operator const void *() const
{
    return d_node_p;
}

inline
const void *bdeci_RbTreeImpIter::operator()() const
{
    return &d_node_p->d_memory;
}

inline
int bdeci_RbTreeImpIter::hasLeft() const
{
    return 0 != d_node_p->d_left_p;
}

inline
int bdeci_RbTreeImpIter::hasRight() const
{
    return 0 != d_node_p->d_right_p;
}

inline
int bdeci_RbTreeImpIter::isBlack() const
{
    return bdeci_RbTreeImp_Node::BDECI_BLACK == d_node_p->d_color;
}

inline
int bdeci_RbTreeImpIter::isRed() const
{
    return bdeci_RbTreeImp_Node::BDECI_RED == d_node_p->d_color;
}

// FREE OPERATORS

inline
int operator==(const bdeci_RbTreeImpIter& lhs, const bdeci_RbTreeImpIter& rhs)
{
    return lhs.d_node_p == rhs.d_node_p;
}

inline
int operator!=(const bdeci_RbTreeImpIter& lhs, const bdeci_RbTreeImpIter& rhs)
{
    return !(lhs == rhs);
}

                           // --------------------
                           // bdeci_RbTreeImpManip
                           // --------------------

// CREATORS

inline
bdeci_RbTreeImpManip::bdeci_RbTreeImpManip(bdeci_RbTreeImp *tree)
: d_tree_p(tree)
, d_node_p(tree->d_first_p)
, d_prev_p(0)
{
}

inline
bdeci_RbTreeImpManip::~bdeci_RbTreeImpManip()
{
}

// MANIPULATORS

inline
void *bdeci_RbTreeImpManip::operator()()
{
    return &d_node_p->d_memory;
}

inline
void bdeci_RbTreeImpManip::advance()
{
    d_prev_p = d_node_p;
    d_node_p = d_node_p->d_next_p;
}

inline
void *bdeci_RbTreeImpManip::insertAfter()
{
    bdeci_RbTreeImp_Node *tmp = d_tree_p->insertAfter(d_node_p);
    d_prev_p = d_node_p;
    d_node_p = d_node_p->d_next_p;

    return &tmp->d_memory;
}

inline
void *bdeci_RbTreeImpManip::insertBefore()
{
    if (d_prev_p) {
        bdeci_RbTreeImp_Node *tmp = d_tree_p->insertAfter(d_prev_p);
        d_node_p = d_prev_p->d_next_p;

        return &tmp->d_memory;
    }
    else {
        void *tmp = d_tree_p->insertAsFirst();
        d_node_p = d_tree_p->d_first_p;

        return tmp;
    }
}

inline
void bdeci_RbTreeImpManip::remove()
{
    bdeci_RbTreeImp_Node *p = d_node_p;
    d_node_p = d_node_p->d_next_p;
    d_tree_p->remove(p, d_prev_p);
}

// ACCESSORS

inline
bdeci_RbTreeImpManip::operator const void *() const
{
    return d_node_p;
}

inline
const void *bdeci_RbTreeImpManip::operator()() const
{
    return &d_node_p->d_memory;
}

                           // -------------------------
                           // bdeci_RbTreeImpBatchManip
                           // -------------------------

// CREATORS

inline
bdeci_RbTreeImpBatchManip::bdeci_RbTreeImpBatchManip(bdeci_RbTreeImp *tree)
: d_tree_p(tree)
, d_node_p(tree->d_first_p)
, d_prev_p(0)
{
}

inline
bdeci_RbTreeImpBatchManip::~bdeci_RbTreeImpBatchManip()
{
    d_tree_p->listToTree();
}

// MANIPULATORS

inline
void *bdeci_RbTreeImpBatchManip::operator()()
{
    return &d_node_p->d_memory;
}

inline
void bdeci_RbTreeImpBatchManip::advance()
{
    d_prev_p = d_node_p;
    d_node_p = d_node_p->d_next_p;
}

inline
void *bdeci_RbTreeImpBatchManip::insertAfter()
{
    bdeci_RbTreeImp_Node *tmp =
                           (bdeci_RbTreeImp_Node *)d_tree_p->d_pool.allocate();
    tmp->d_next_p = d_node_p->d_next_p;
    d_node_p->d_next_p = tmp;
    d_prev_p = d_node_p;
    d_node_p = tmp;
    ++d_tree_p->d_numElements;

    return &tmp->d_memory;
}

inline
void *bdeci_RbTreeImpBatchManip::insertBefore()
{
    bdeci_RbTreeImp_Node *tmp =
                           (bdeci_RbTreeImp_Node *)d_tree_p->d_pool.allocate();
    tmp->d_next_p = d_node_p;
    if (d_prev_p) {
        d_prev_p->d_next_p = tmp;
    }
    else {
        d_tree_p->d_first_p = tmp;
    }
    d_node_p = tmp;
    ++d_tree_p->d_numElements;

    return &tmp->d_memory;
}

inline
void bdeci_RbTreeImpBatchManip::remove()
{
    bdeci_RbTreeImp_Node *p = d_node_p;
    d_node_p = d_node_p->d_next_p;
    if (d_prev_p) {
        d_prev_p->d_next_p = d_node_p;
    }
    else {
        d_tree_p->d_first_p = d_node_p;
    }
    d_tree_p->d_pool.deallocate(p);
    --d_tree_p->d_numElements;
}

// ACCESSORS

inline
bdeci_RbTreeImpBatchManip::operator const void *() const
{
    return d_node_p;
}

inline
const void *bdeci_RbTreeImpBatchManip::operator()() const
{
    return &d_node_p->d_memory;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
