// bslstl_treeiterator.t.cpp                                          -*-C++-*-
#include <bslstl_treeiterator.h>

#include <bslalg_rbtreeutil.h>
#include <bslalg_rbtreenode.h>
#include <bslalg_rbtreeanchor.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#include <algorithm>
#include <cstddef>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
#include <random>
#endif

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace BloombergLP;
using namespace std;
using namespace bslstl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a class template, 'bslstl::TreeIterator',
// that is an in-core value-semantic type.  This class provides an
// STL-conforming bidirectional iterator over the ordered 'bslalg::RbTreeNode'
// objects in a binary tree (see section [24.2.6 bidirectional.iterators] of
// the C++11 standard).
//
// The primary manipulators of this class is the value constructor, which is
// sufficient to allow an object to attain any achievable state.  The basic
// accessor of this tree iterator is the 'node' method, which returns the
// address of the tree node at which the iterator is positioned.
//
// Primary Manipulators:
//: o explicit TreeIterator(const bslalg::RbTreeNode *node)
//
// Basic Accessors:
//: o const bslalg::RbTreeNode *node()
//
// Since the state of an object will only be meaningful if the object refers to
// a valid tree node.  The facilities provided by 'bslalg::RbTreeUtil' will be
// used to create and access red-black trees to which objects of this class can
// refer.  We will assume the correctness of the operations in
// 'bslalg::RbTreeUtil', because it is a dependency of this component.  This
// particular class provides a value constructor capable of creating an object
// in any state, obviating the primitive generator function, 'gg', which is
// normally used for this purpose.
//
// We will follow our standard 10-case approach to testing value-semantic types
// with some exceptions:
//: o We do not need to test anything in case 3 and 8, because the value
//:   constructor will have been already tested as the primary manipulator in
//:   case 2.
//: o We do not need to test anything in case 5, because this class does not
//:   provide a 'print' method or 'operator<<'.
//: o We do not need to test anything in case 10, because this class does not
//:   provide bdex stream operators.
//: o We will test 'operator*' and 'operator->' together with the 'node' method
//:   in test case 2, because these operator simply provide different ways to
//:   access to the value of the node.
//
// Global Concerns:
//: o No memory is ever allocated.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] TreeIterator();
// [ 2] explicit TreeIterator(const bslalg::RbTreeNode *node);
// [ 7] TreeIterator(const NcIter& original);
// [ 7] TreeIterator(const TreeIterator& original);
// [ 2] ~TreeIterator();
//
// MANIPULATORS
// [ 9] TreeIterator& operator=(const TreeIterator& rhs);
// [11] TreeIterator operator++();
// [12] TreeIterator operator--();
//
// ACCESSORS
// [ 3] reference operator*() const;
// [ 3] pointer operator->() const;
// [ 3] const bslalg::RbTreeNode *node() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const TreeIterator& lhs, const TreeIterator& rhs);
// [ 6] bool operator!=(const TreeIterator& lhs, const TreeIterator& rhs);
// [13] TreeIterator operator++(iter, int);
// [14] TreeIterator operator--(iter, int);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [15] USAGE EXAMPLE
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: No memory is ever allocated from this class.
// [16] CONCERN: The type is trivially copyable.
// [17] CONCERN: There are no surprising constructor overloads.

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslalg::RbTreeUtil   Op;
typedef bslalg::RbTreeNode   Node;
typedef bslalg::RbTreeAnchor Anchor;

static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

class MyWrapper
{

    // DATA
    int d_data;

  public:
    // MANIPULATORS
    int& data() { return d_data; }

    // ACCESSORS
    int data() const { return d_data; }
};

bool operator==(const MyWrapper& lhs, const MyWrapper& rhs)
{
    return lhs.data() == rhs.data();
}


class WrapperNode : public bslalg::RbTreeNode
{

    // DATA
    MyWrapper d_value;

  public:
    // MANIPULATORS
    MyWrapper& value() { return d_value; }
        // Return a reference providing modifiable access to the 'value' of
        // this object.

    // ACCESSORS
    const MyWrapper& value() const {return d_value; }
        // Return a reference providing non-modifiable access to the 'value' of
        // this object.
};

typedef TreeIterator<MyWrapper, WrapperNode, std::ptrdiff_t> Obj;

struct WrapperNodeComparator {
    typedef WrapperNode NodeType;
    bool operator()(const bslalg::RbTreeNode& lhs,
                    const bslalg::RbTreeNode& rhs) const {
        return static_cast<const WrapperNode&>(lhs).value().data()
            < static_cast<const WrapperNode&>(rhs).value().data();
    }
};

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace UsageExample {

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Navigating a Tree Using 'TreeIterator'.
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we create a simple tree and then use a
// 'TreeIterator' to navigate its elements.
//
// First, we define a type 'IntNode' for the nodes of our tree.  'IntNode'
// inherits from 'bslalg::RbTreeNode' (allowing it to be operated on by
// 'bslalg::RbTreeUtil'), and supplies an integer payload:
//..
    class IntNode : public bslalg::RbTreeNode {
        // DATA
        int d_value;  // actual value represented by the node

      public:
        // MANIPULATORS
        int& value() { return d_value; }

        // ACCESSORS
        const int& value() const {return d_value; }
    };
//..
// Then, we define a comparison function for 'IntNode' objects.  This type is
// designed to be supplied to functions in 'bslalg::RbTreeUtil':
//..
    class IntNodeComparator {
      public:
        // CREATORS
        IntNodeComparator() {}
            // Create a node-value comparator.

        // ACCESSORS
        bool operator()(const bslalg::RbTreeNode& lhs,
                        const bslalg::RbTreeNode& rhs) const
        {
            return static_cast<const IntNode&>(lhs).value() <
                   static_cast<const IntNode&>(rhs).value();
        }
    };
//..
// Next, we define the signature of the example function where we will
// navigate a tree using a 'TreeIterator':
//..
    void exampleTreeNavigationFunction()
    {
//..
// Then, we define a 'bslalg::RbTreeAnchor' object to hold our tree, and a
// series of nodes that we will use to populate the tree:
//..
        enum { NUM_NODES = 5 };

        bslalg::RbTreeAnchor tree;
        IntNode              nodes[NUM_NODES];
//..
// Next, we assign values to each of the 'nodes' and insert them into 'tree'
// using 'bslalg::RbTreeUtil', suppling the 'insert' function an instance of
// the comparator we defined earlier:
//..
        for (int i = 0; i < NUM_NODES; ++i) {
            nodes[i].value() = i;
            bslalg::RbTreeUtil::insert(&tree, IntNodeComparator(), &nodes[i]);
        }

        ASSERT(5 == tree.numNodes());
//..
// Then, we create a type alias for a 'TreeIterator' providing non-modifiable
// access to elements in the tree.  Note that in this instance, the iterator
// must provide non-modifiable access as modifying the key value for a node
// would invalidate ordering of the binary search tree:
//..
        typedef TreeIterator<const int, IntNode, std::ptrdiff_t>
                                                             ConstTreeIterator;
//..
// Now, we create an instance of the 'TreeIterator' and use it to navigate the
// elements of 'tree', printing their values to the console:
//..
        ConstTreeIterator iterator(tree.firstNode());
        ConstTreeIterator endIterator(tree.sentinel());
        for (; iterator != endIterator; ++iterator) {
            if (verbose) {
            printf("Node value: %d\n", *iterator);
            }
        }
    }
//..
// Finally, we observe the following console output:
//..
//  Node value: 0
//  Node value: 1
//  Node value: 2
//  Node value: 3
//  Node value: 4
//..

}  // close namespace UsageExample

                            // =============
                            // class BasedOn
                            // =============

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
class BasedOn : public TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>
    // This is a minimal class that has a public base of a 'TreeIterator',
    // therefore it implicitly converts to a 'TreeIterator'
{
  public:
    // CREATORS
    explicit BasedOn(const bslalg::RbTreeNode *node);
        // Create an 'BasedOn' object at the specified 'position'.  The
        // behavior is undefined unless 'node' is of the parameterized 'NODE',
        // which is derived from 'bslalg::RbTreeNode.

};

                            // -------------
                            // class BasedOn
                            // -------------

// CREATORS
template <class VALUE, class NODE, class DIFFERENCE_TYPE>
inline
BasedOn<VALUE, NODE, DIFFERENCE_TYPE>::
BasedOn(const bslalg::RbTreeNode *node)
: TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>(node)
{
}

                          // ================
                          // class ConvertsTo
                          // ================

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
class ConvertsTo
    // This is a minimal class that has a public base of a 'TreeIterator',
    // therefore it implicitly converts to a 'TreeIterator'
{
  private:
    // DATA
    TreeIterator<VALUE, NODE, DIFFERENCE_TYPE> d_node;

  public:
    // CREATORS
    explicit ConvertsTo(const bslalg::RbTreeNode *node);
        // Create an 'BasedOn' object at the specified 'position'.  The
        // behavior is undefined unless 'node' is of the parameterized 'NODE',
        // which is derived from 'bslalg::RbTreeNode.

    operator const TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>&() const;
        // Convert to a tree iterator.
};

                          // ----------------
                          // class ConvertsTo
                          // ----------------

// CREATORS
template <class VALUE, class NODE, class DIFFERENCE_TYPE>
inline
ConvertsTo<VALUE, NODE, DIFFERENCE_TYPE>::
ConvertsTo(const bslalg::RbTreeNode *node)
: d_node(node)
{
}

template <class VALUE, class NODE, class DIFFERENCE_TYPE>
inline
ConvertsTo<VALUE, NODE, DIFFERENCE_TYPE>::
operator const TreeIterator<VALUE, NODE, DIFFERENCE_TYPE>&() const
{
    return d_node;
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: No memory is ever allocated.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:
      case 17: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR OVERLOADS
        //   Ensure that the iterator types (const and non-'const') both have
        //   only a copy constructor, a conversion constructor from the
        //   'const'-version, a default constructor, and no other, surprising
        //   overloads.
        //
        // Concerns:
        //:  1 A non-'const' 'iterator' is copy constructible from a type that
        //:    converts to a non-'const' 'iterator'.
        //:
        //:  2 A 'const_iterator' is copy constructible from a type that
        //:    converts to a 'const_iterator'.
        //:
        //:  3 A non-'const' 'iterator' is copy constructible from a type that
        //:    converts to a 'const_iterator'.
        //
        // Plan:
        //: 1 Create two ('const'/non-'const') iterator types.
        //:
        //: 2 Create a templated type 'BasedOn' that publicly inherits from an
        //:   'iterator'.
        //:
        //: 3 Create a templated type 'ConvertsTo' that implicitly converts to
        //:   an 'iterator'.
        //:
        //: 4 Construct 'BasedOn' and a 'ConvertsTo' with known values; some
        //:   with 'const', the others with non-'const' value type.
        //:
        //: 5 Construct 'iterator's from the objects with mutable value types.
        //:   Verify that the values were copied properly.
        //:
        //: 6 Construct 'const_iterator's from all objects.  Verify that the
        //:   values were copied properly.
        //
        // Testing
        //   CONCERN: There are no surprising constructor overloads.
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONSTRUCTOR OVERLOADS"
                            "\n=====================\n");

        typedef TreeIterator<MyWrapper, WrapperNode, std::ptrdiff_t> iterator;
        typedef TreeIterator<const MyWrapper, WrapperNode, std::ptrdiff_t>
                                                                const_iterator;

        typedef BasedOn<MyWrapper, WrapperNode, std::ptrdiff_t> based_on;
        typedef BasedOn<const MyWrapper, WrapperNode, std::ptrdiff_t>
                                                                const_based_on;

        typedef ConvertsTo<MyWrapper, WrapperNode, std::ptrdiff_t> converts_to;
        typedef ConvertsTo<const MyWrapper, WrapperNode, std::ptrdiff_t>
                                                             const_converts_to;

        WrapperNode node1;
        node1.value().data() = 1;

        WrapperNode node2;
        node2.value().data() = 2;

        if (verbose) printf("\nVerify inheritance-conversion (to base).\n");
        {
            based_on basedOn1(&node1);
            based_on basedOn2(&node2);

            iterator it1(basedOn1);
            iterator it2(basedOn2);

            ASSERT(it1->data() == 1);
            ASSERT(it2->data() == 2);
        }

        {
            const_based_on cBasedOn1(&node1);
            const_based_on cBasedOn2(&node2);

            const_iterator cit11(cBasedOn1);
            const_iterator cit12(cBasedOn2);

            ASSERT(cit11->data() == 1);
            ASSERT(cit12->data() == 2);

            based_on basedOn1(&node1);
            based_on basedOn2(&node2);

            iterator it1(basedOn1);
            iterator it2(basedOn2);

            const_iterator cit21(it1);
            const_iterator cit22(it2);

            ASSERT(cit21->data() == 1);
            ASSERT(cit22->data() == 2);
        }

        if (verbose) printf("\nVerify conversion-operator.\n");
        {
            converts_to convertsTo1(&node1);
            converts_to convertsTo2(&node2);

            iterator it1(convertsTo1);
            iterator it2(convertsTo2);

            ASSERT(it1->data() == 1);
            ASSERT(it2->data() == 2);
        }

        {
            const_converts_to cConvertsTo1(&node1);
            const_converts_to cConvertsTo2(&node2);

            const_iterator cit11(cConvertsTo1);
            const_iterator cit12(cConvertsTo2);

            ASSERT(cit11->data() == 1);
            ASSERT(cit12->data() == 2);

            converts_to convertsTo1(&node1);
            converts_to convertsTo2(&node2);

            iterator it1(convertsTo1);
            iterator it2(convertsTo2);

            const_iterator cit21(it1);
            const_iterator cit22(it2);

            ASSERT(cit21->data() == 1);
            ASSERT(cit22->data() == 2);
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TYPE TRAITS
        //   Ensure that the iterator types ('const' and non-'const') both have
        //   the proper type traits.
        //
        // Concerns:
        //:  1 The types are trivially copyable.
        //
        // Plan:
        //: 1 Create two ('const'/non-'const') iterator types.
        //:
        //: 2 Verify that their type traits say they are trivially copyable.
        //
        // Testing
        //   CONCERN: The type is trivially copyable.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTYPE TRAITS"
                            "\n===========\n");

#ifndef BSLS_PLATFORM_CMP_SUN
        typedef TreeIterator<MyWrapper, WrapperNode, std::ptrdiff_t> iterator;
        typedef TreeIterator<const MyWrapper, WrapperNode, std::ptrdiff_t>
                                                                const_iterator;

        ASSERT(bsl::is_trivially_copyable<iterator>::value == true);

        ASSERT(bsl::is_trivially_copyable<const_iterator>::value == true);
#endif
      } break;
      case 15: {
          if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");

          UsageExample::exampleTreeNavigationFunction();

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // POST-DECREMENT OPERATOR
        //   Ensure that 'operator--(iter, int)' behaves according to its
        //   contract.
        //
        // Concerns:
        //:  1 The post-decrement operator changes the value of the object to
        //:    refer to the next element in the tree.
        //:
        //:  2 The signature and return type are standard.
        //:
        //:  3 The value returned is the value of the object prior to the
        //:    operator call.
        //:
        //:  4 Post-decrementing an object referring to the past-the-end
        //:    address moves the object to point to the rightmost element in
        //:    the tree.
        //
        // Plan:
        //: 1 Use the address of 'operator--(iter, int)' to initialize a
        //:   member-function pointer having the appropriate signature and
        //:   return type for the post-decrement operator defined in this
        //:   component.  (C-2)
        //:
        //: 2 Create a tree with N nodes.  (C-1,3,4)
        //:
        //: 3 For each node 'N1' (including the sentinel node) that is not the
        //:   leftmost node in the tree of P-2:
        //:
        //:   1 Create a modifiable 'Obj', 'mX', and a 'const' 'Obj', 'Y', both
        //:     pointing to 'N1'.
        //:
        //:   2 Create a 'const' 'Obj', 'Z', pointing to the node to the left
        //:     of 'N1'.
        //:
        //:   2 Invoke the post-decrement operator on 'mX'.
        //:
        //:   3 Verify that value returned compare equals to that of 'Y'.
        //:     (C-3)
        //:
        //:   4 Verify using the equality-comparison operator that 'mX' has the
        //:     same value as that of 'Z'.  (C-1, 4)
        //
        // Testing
        //   TreeIterator operator--(iter, int);
        // --------------------------------------------------------------------

        if (verbose) printf("\nPOST-DECREMENT OPERATOR"
                            "\n=======================\n");

        if (verbose)
            printf("\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj (*operatorPtr)(Obj&, int);

            // Verify that the signature and return type are standard.

            operatorPtr operatorPredecrement = &operator--;

            (void) operatorPredecrement;  // quash potential compiler warning
        }

        if (verbose) printf("\nCreate a tree with N nodes.\n");

        const int N = 10;
        Anchor tree;

        WrapperNodeComparator nodeComparator;

        WrapperNode nodes[N];
        for (int i = 0; i < N; ++i) {
            nodes[i].value().data()  = i;
            Op::insert(&tree, nodeComparator, &nodes[i]);
        }

        if (verbose)
            printf("\nTest the post-decrement operator.\n");

        for (int ti = 1; ti <= N; ++ti) {

            const bslalg::RbTreeNode *address;
            if (ti == N) {
                address = tree.sentinel();
            }
            else {
                address = &nodes[ti];
            }

            Obj mX(address);
            const Obj Y(address);

            const Obj Z(&nodes[ti - 1]);

            const Obj mR = mX--;

            if (veryVerbose) {
                T_ T_ P_(ti) P_(mR->data()) P_(mX->data()) P(Z->data()) }

            ASSERTV(ti, mR->data(), Y->data(), mR == Y);
            ASSERTV(ti, Z->data(), mX->data(), Z == mX);
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // POST-INCREMENT OPERATOR
        //   Ensure that 'operator++(iter, int)' behaves according to its
        //   contract.
        //
        // Concerns:
        //:  1 The post-increment operator changes the value of the object to
        //:    refer to the next element in the tree.
        //:
        //:  2 The signature and return type are standard.
        //:
        //:  3 The value returned is the value of the object prior to the
        //:    operator call.
        //:
        //:  4 Post-incrementing an object referring to the last element in the
        //:    tree moves the object to point to the sentinel node.
        //
        // Plan:
        //: 1 Use the address of 'operator++(iter, int)' to initialize a
        //:   member-function pointer having the appropriate signature and
        //:   return type for the post-increment operator defined in this
        //:   component.  (C-2)
        //:
        //: 2 Create a tree with N nodes.  (C-1,3,4)
        //:
        //: 3 For each node 'N1' node in the tree of P-2:
        //:
        //:   1 Create a modifiable 'Obj', 'mX', and a 'const' 'Obj', 'Y', both
        //:     pointing to 'N1'.
        //:
        //:   2 Create a 'const' 'Obj', 'Z', pointing the node to the right of
        //:     'N1'.
        //:
        //:   2 Invoke the post-increment operator on 'mX'.
        //:
        //:   3 Verify that value returned compare equals to that of 'Y'.
        //:     (C-3)
        //:
        //:   4 Verify using the equality-comparison operator that 'mX' has the
        //:     same value as that of 'Z'.  (C-1, 4)
        //
        // Testing:
        //   TreeIterator operator++(iter, int);
        // --------------------------------------------------------------------

        if (verbose) printf("\nPost-Increment OPERATOR"
                            "\n=======================\n");

        if (verbose)
            printf("\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj (*operatorPtr)(Obj&, int);

            // Verify that the signature and return type are standard.

            operatorPtr operatorPostincrement = &operator++;

            (void) operatorPostincrement;  // quash potential compiler warning
        }

        if (verbose) printf("\nCreate a tree with N nodes.\n");

        const int N = 10;
        Anchor tree;

        WrapperNodeComparator nodeComparator;

        WrapperNode nodes[N];
        for (int i = 0; i < N; ++i) {
            nodes[i].value().data()  = i;
            Op::insert(&tree, nodeComparator, &nodes[i]);
        }

        if (verbose)
            printf("\nTest the post-increment operator.\n");

        for (int ti = 1; ti < N; ++ti) {

            Obj mX(&nodes[ti]);
            const Obj Y(&nodes[ti]);

            const bslalg::RbTreeNode *address;
            if (ti == N - 1) {
                address = tree.sentinel();
            }
            else {
                address = &nodes[ti + 1];
            }
            const Obj Z(address);

            const Obj mR = mX++;

            if (veryVerbose) {
                T_ T_ P_(ti) P_(mR->data()) P_(mX->data()) P(Z->data()) }

            ASSERTV(ti, mR->data(), Y->data(), mR == Y);
            ASSERTV(ti, Z->data(), mX->data(), Z == mX);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // PRE-DECREMENT OPERATOR
        //   Ensure that 'operator--' behaves according to its contract.
        //
        // Concerns:
        //:  1 The pre-decrement operator changes the value of the object to
        //:    refer to the previous element in the tree.
        //:
        //:  2 The signature and return type are standard.
        //:
        //:  3 The reference returned is to the object on which the operator
        //:    was invoked.
        //:
        //:  4 Pre-decrementing an object referring to the past-the-end address
        //:    moves the object to point to the rightmost element in the tree.
        //
        // Plan:
        //: 1 Use the address of 'operator--' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   pre-decrement operator defined in this component.  (C-2)
        //:
        //: 2 Create a tree with N nodes.  (C-1,3,4)
        //:
        //: 3 For each node 'N1' (including the sentinel node) that is not the
        //:   leftmost node in the tree of P-2:
        //:
        //:   1 Create a modifiable 'Obj', 'mX', pointing to 'N1'.
        //:
        //:   2 Create a 'const' 'Obj', 'Z', pointing to the node to the left
        //:     of 'N1'.
        //:
        //:   2 Invoke the pre-decrement operator on 'mX'.
        //:
        //:   3 Verify that the address of the return value is the same as
        //:     that of 'mX'.  (C-3)
        //:
        //:   4 Verify using the equality-comparison operator that 'mX' has the
        //:     same value as that of 'Z'.  (C-1, 4)
        //
        // Testing:
        //   TreeIterator operator--();
        // --------------------------------------------------------------------

        if (verbose) printf("\nPRE-DECREMENT OPERATOR"
                            "\n======================\n");

        if (verbose)
            printf("\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj& (Obj::*operatorPtr)();

            // Verify that the signature and return type are standard.

            operatorPtr operatorPredecrement = &Obj::operator--;

            (void) operatorPredecrement;  // quash potential compiler warning
        }

        if (verbose) printf("\nCreate a tree with N nodes.\n");

        const int N = 10;
        Anchor tree;

        WrapperNodeComparator nodeComparator;

        WrapperNode nodes[N];
        for (int i = 0; i < N; ++i) {
            nodes[i].value().data()  = i;
            Op::insert(&tree, nodeComparator, &nodes[i]);
        }

        if (verbose)
            printf("\nTest the pre-decrement operator.\n");

        for (int ti = 1; ti <= N; ++ti) {

            const bslalg::RbTreeNode *address;
            if (ti == N) {
                address = tree.sentinel();
            }
            else {
                address = &nodes[ti];
            }

            Obj mX(address);

            const Obj Z(&nodes[ti - 1]);

            Obj *mR = &(--mX);

            if (veryVerbose) {
                T_ T_ P_(ti) P_(mR) P_(mX->data()) P(Z->data()) }

            ASSERTV(ti, mR, &mX, mR == &mX);

            ASSERTV(ti, Z->data(), mX->data(), Z == mX);
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // PRE-INCREMENT OPERATOR
        //   Ensure that 'operator++' behaves according to its contract.
        //
        // Concerns:
        //:  1 The pre-increment operator changes the value of the object to
        //:    refer to the next element in the tree.
        //:
        //:  2 The signature and return type are standard.
        //:
        //:  3 The reference returned is to the object on which the operator
        //:    was invoked.
        //:
        //:  4 Pre-increment an object referring to the rightmost element in
        //:    the tree moves the object to point to the past-the-end address.
        //
        // Plan:
        //: 1 Use the address of 'operator++' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   pre-decrement operator defined in this component.  (C-2)
        //:
        //: 2 Create a tree with N nodes.  (C-1,3,4)
        //:
        //: 3 For each node 'N1' (including the sentinel node) that is not the
        //:   leftmost node in the tree of P-2:
        //:
        //:   1 Create a modifiable 'Obj', 'mX', pointing to 'N1'.
        //:
        //:   2 Create a 'const' 'Obj', 'Z', pointing to the node to the right
        //:     of 'N1'.
        //:
        //:   2 Invoke the pre-increment operator on 'mX'.
        //:
        //:   3 Verify that the address of the return value is the same as
        //:     that of 'mX'.  (C-3)
        //:
        //:   4 Verify using the equality-comparison operator that 'mX' has the
        //:     same value as that of 'Z'.  (C-1, 4)
        //
        // Testing:
        //   TreeIterator operator++();
        // --------------------------------------------------------------------

        if (verbose) printf("\nPRE-INCREMENT OPERATOR"
                            "\n======================\n");

        if (verbose)
            printf("\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj& (Obj::*operatorPtr)();

            // Verify that the signature and return type are standard.

            operatorPtr operatorPredecrement = &Obj::operator++;

            (void) operatorPredecrement;  // quash potential compiler warning
        }

        if (verbose) printf("\nCreate a tree with N nodes.\n");

        const int N = 10;
        Anchor tree;

        WrapperNodeComparator nodeComparator;

        WrapperNode nodes[N];
        for (int i = 0; i < N; ++i) {
            nodes[i].value().data()  = i;
            Op::insert(&tree, nodeComparator, &nodes[i]);
        }

        if (verbose)
            printf("\nTest the pre-increment operator.\n");

        for (int ti = 0; ti < N; ++ti) {

            Obj mX(&nodes[ti]);
            const bslalg::RbTreeNode *address;
            if (ti == N - 1) {
                address = tree.sentinel();
            }
            else {
                address = &nodes[ti + 1];
            }
            const Obj Z(address);

            Obj *mR = &(++mX);

            if (veryVerbose) {
                T_ T_ P_(ti) P_(mR) P_(mX->data()) P(Z->data()) }

            ASSERTV(ti, mR, &mX, mR == &mX);

            ASSERTV(ti, Z->data(), mX->data(), Z == mX);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX STREAMING
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified.
        //:
        //: 5 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-2)
        //:
        //: 2 Create a tree with N nodes.
        //:
        //: 3 For each node 'N1' in the tree of P-2:  (C-1,3..4)
        //:
        //:   1 Create two 'const' 'Obj', 'Z' and 'ZZ', pointing to 'N1'.
        //:
        //:   2 For each node 'N2' in the tree of P-2:  (C-1,3..4)
        //:
        //:     1 Create a modifiable 'Obj', 'mX', pointing to 'N2'.
        //:
        //:     2 Assign 'mX' from 'Z'.  (C-1)
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:     4 Use the equality-comparison operator to verify that:
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 4 For each node 'N1' in tree of P-2:  (C-3, 5)
        //:
        //:   1 Create a modifiable 'Obj', 'mX', pointing to 'N1'.
        //:
        //:   1 Create a 'const' 'Obj', 'ZZ', pointing to 'N1'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:   5 Use the equal-comparison operator to verify that 'mX' has the
        //:     same value as 'ZZ'.  (C-5)
        //
        // Testing:
        //   TreeIterator& operator=(const TreeIterator& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR"
                            "\n========================\n");

        if (verbose)
            printf("\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void) operatorAssignment;  // quash potential compiler warning
        }


        if (verbose) printf("\nCreate a tree with N nodes.\n");

        const int N = 10;
        Anchor tree;

        WrapperNodeComparator nodeComparator;

        WrapperNode nodes[N];
        for (int i = 0; i < N; ++i) {
            nodes[i].value().data()  = i;
            Op::insert(&tree, nodeComparator, &nodes[i]);
        }

        if (verbose)
            printf("\nTest the assignment operator on the cross product\n"
                   "\nof the iterators pointing to nodes in the tree.\n");

        for (int ti = 0; ti < N; ++ti) {

            const Obj Z(&nodes[ti]);
            const Obj ZZ(&nodes[ti]);

            for (int tj = 0; tj < N; ++tj) {

                Obj mX(&nodes[tj]);

                Obj *mR = &(mX = Z);
                ASSERTV(ti, tj, mR, &mX, mR == &mX);

                ASSERTV(ti, tj, Z->data(), mX->data(), Z == mX);
                ASSERTV(ti, tj, Z->data(), ZZ->data(), Z == ZZ);
            }
        }

        if (verbose)
            printf("\nTest for aliasing.\n");

        for (int ti = 0; ti < N; ++ti) {

            Obj mX(&nodes[ti]);
            const Obj ZZ(&nodes[ti]);

            const Obj& Z = mX;
            Obj *mR = &(mX = Z);
            ASSERTV(ti, mR, &mX, mR == &mX);

            ASSERTV(ti, Z->data(), mX->data(), Z == mX);
            ASSERTV(ti, Z->data(), ZZ->data(), Z == ZZ);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTORS
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //:
        //: 4 An object having the parameterized 'VALUE_TYPE' declared 'const'
        //:   can be used to create an object having a the same 'VALUE_TYPE'
        //:   declared as non-'const'.
        //
        // Plan:
        //: 1 Create a tree with N nodes.
        //:
        //: 2 For each node 'N1' in the tree of P-1:  (C-1..4)
        //:
        //:   1 Create two 'const' 'Obj' 'Z', and 'ZZ' both pointing to 'N1'.
        //:
        //:   2 Create a 'const' object having the parameterized 'VALUE_TYPE'
        //:     declared 'const', 'Y', pointing to 'N1'.
        //:
        //:   3 Use the copy constructor to create an object 'X1', supplying it
        //:     the 'const' object 'Z'.  (C-2)
        //:
        //:   4 Verify that the newly constructed object 'X1', has the same
        //:     value as that of 'Z'.  Verify that 'Z' still has the same value
        //:     as that of 'ZZ'.  (C-1,3)
        //:
        //:   5 Use the copy constructor to create an object having the
        //:     parameterized 'VALUE_TYPE' declared 'const' ,'X2', supplying it
        //:     the 'const' object 'Z'.  (C-2, 4)
        //:
        //:   6 Verify that the newly constructed object 'X2', has the same
        //:     value as that of 'Y'.  Verify that 'Z' still has the same value
        //:     as that of 'ZZ'.  (C-1,3)
        //
        // Testing:
        //   TreeIterator(const NcIter& original);
        //   TreeIterator(const TreeIterator& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTORS"
                            "\n=================\n");

        if (verbose) printf("\nCreate a tree with N nodes.\n");

        const int N = 10;
        Anchor tree;

        WrapperNodeComparator nodeComparator;

        WrapperNode nodes[N];
        for (int i = 0; i < N; ++i) {
            nodes[i].value().data()  = i;
            Op::insert(&tree, nodeComparator, &nodes[i]);
        }

        if (verbose)
            printf("\nTest using the copy constructor on every value\n");

        for (int ti = 0; ti < N; ++ti) {

            const Obj Z(&nodes[ti]);
            const Obj ZZ(&nodes[ti]);

            typedef TreeIterator<const MyWrapper, WrapperNode, std::ptrdiff_t>
                                                             ConstValueTypeObj;
            const ConstValueTypeObj Y(&nodes[ti]);

            const Obj X1(Z);

            if (veryVerbose) { T_ T_ P_(ti) P(X1->data()) }

            ASSERTV(Z->data(), X1->data(), Z == X1);
            ASSERTV(Z->data(), ZZ->data(), Z == ZZ);

            const ConstValueTypeObj X2(Z);

            ASSERTV(Y->data(), X2->data(), Y == X2);
            ASSERTV(Z->data(), ZZ->data(), Z == ZZ);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if they point
        //:   to the same node in the same tree.
        //:
        //: 2 'true  == (X == X)'  (i.e., identity)
        //:
        //: 3 'false == (X != X)'  (i.e., identity)
        //:
        //: 4 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 5 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'
        //:
        //: 7 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 8 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 9 The equality operator's signature and return type are standard.
        //:
        //:10 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-7..10)
        //:
        //: 2 Create a tree with N nodes.
        //:
        //: 3 For each node 'N1' in the tree of P-2:  (C-1..6)
        //:
        //:   1 Create a single object pointing to 'N1', and use it to verity
        //:     the reflexive (anti-reflexive) property of equality
        //:     (inequality) in the presence of aliasing.  (C-2..3)
        //:
        //:   2 For each node 'N2' in the tree of P-2:  (C-1, 4..6)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'N1' and 'N2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 Create an object 'X' having the value of 'R1'.  Create
        //:       another object 'Y' having the value of 'R2'.
        //:
        //:     3 Verify the commutativity property and the expected return
        //:       value for both '==' and '!='.  (C-1, 4..6)
        //
        // Testing:
        //   bool operator==(const TreeIterator& lhs, const TreeIterator& rhs);
        //   bool operator!=(const TreeIterator& lhs, const TreeIterator& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                            "\n=============================\n");

        if (verbose)
            printf("\nAssign the address of each operator to a variable.\n");
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = bslstl::operator==;
                                        // See {DRQS 131792157} for 'bslstl::'.
            (void) operatorEq;  // quash potential compiler warnings

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
            (void) [](const Obj& lhs, const Obj& rhs) -> bool {
                return lhs != rhs;
            };
#else
            operatorPtr operatorNe = bslstl::operator!=;
                                        // See {DRQS 131792157} for 'bslstl::'.
            (void) operatorNe;
#endif
        }

        if (verbose) printf("\nCreate a tree with N nodes.\n");

        const int N = 10;
        Anchor tree;
        if (verbose) P((void*) tree.sentinel());

        WrapperNodeComparator nodeComparator;

        WrapperNode nodes[N];
        for (int i = 0; i < N; ++i) {
            nodes[i].value().data()  = i;
            Op::insert(&tree, nodeComparator, &nodes[i]);
        }

        if (verbose) printf("\nCompare every value with every value.\n");

        for (int ti = 0; ti < N; ++ti) {

            Obj mX(&nodes[ti]); const Obj& X = mX;

            // Ensure an object compares correctly with itself (alias test).
            ASSERTV(X->data(),   X == X);
            ASSERTV(X->data(), !(X != X));

            for (int tj = 0; tj < N; ++tj) {

                bool EXP = ti == tj;

                Obj mY(tree.firstNode()); const Obj& Y = mY;
                for (int ty = 0; ty < tj; ++ty) { ++mY; }

                if (veryVerbose) {
                    T_ T_ P_(EXP) P_(X->data()) P(Y->data()) }

                // Verify value, commutativity

                ASSERTV(X->data(), Y->data(),  EXP == (X == Y));
                ASSERTV(X->data(), Y->data(),  EXP == (Y == X));

                ASSERTV(X->data(), Y->data(), !EXP == (X != Y));
                ASSERTV(X->data(), Y->data(), !EXP == (Y != X));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS, 'operator*', and 'operator->'
        //   Ensure that the basic accessor 'node' as well as 'operator*' and
        //   'operator->' properly interprets object state.
        //
        // Concerns:
        //: 1 The 'node' method returns the address of the node to which this
        //:   object refers.
        //:
        //: 2 The 'operator*' returns the reference to the value of the element
        //:   to which this object refers.
        //:
        //: 3 The 'operator->' returns the address to the value of the element
        //:   to which this object refers.
        //:
        //: 4 Each of the three methods are declared 'const'.
        //:
        //: 5 The signature and return type are standard.
        //
        // Plan:
        //: 1 Use the addresses of 'operator*' and 'operator->' to initialize
        //:   member-function pointers having the appropriate signatures and
        //:   return types for the operators defined in this component.  (C-5)
        //:
        //: 2 Create a tree with N nodes.
        //:
        //: 3 Use the primary manipulators to create an object, 'mX', and
        //:   iterator through the nodes of the tree.  For each iteration do
        //:   the following: (C-1..4)
        //:
        //:   1 Create a const reference to the object 'X'.
        //:
        //:   2 Invoke the method 'node' on 'X' and verify that it returns the
        //:     expected value.  (C-1,4)
        //:
        //:   3 Invoke 'operator*' on 'X' and verify that it returns the
        //:     expected value.  (C-2,4)
        //:
        //:   4 Invoke 'operator->' on 'X' and verify that it returns the
        //:     expected value.  (C-3,4)
        //
        // Testing:
        //   reference operator*() const;
        //   pointer operator->() const;
        //   const bslalg::RbTreeNode *node() const;
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBASIC ACCESSORS, 'operator*', and 'operator->'"
                   "\n==============================================\n");

        if (verbose)
            printf("\nAssign the address of the dereference operator to a "
                   "variable.\n");
        {
            typedef MyWrapper& (Obj::*operatorPtr)() const;

            // Verify that the signature and return type are standard.

            operatorPtr operatorDereference = &Obj::operator*;

            (void) operatorDereference;  // quash potential compiler warning
        }

        if (verbose)
            printf("\nAssign the address of the arrow operator to a "
                   "variable.\n");
        {
            typedef MyWrapper* (Obj::*operatorPtr)() const;

            // Verify that the signature and return type are standard.

            operatorPtr operatorArrow = &Obj::operator->;

            (void) operatorArrow;  // quash potential compiler warning
        }

        if (verbose) printf("\nCreate a tree with N nodes.\n");

        const int N = 10;
        Anchor tree;
        if (verbose) P((void*) tree.sentinel());

        WrapperNodeComparator nodeComparator;

        WrapperNode nodes[N];
        for (int i = 0; i < N; ++i) {
            nodes[i].value().data()  = i;
            Op::insert(&tree, nodeComparator, &nodes[i]);
        }

        if (verbose) printf("\nTest the accessors.\n");

        Obj mX(tree.firstNode()); const Obj& X = mX;

        for (int i = 0; X.node() != tree.sentinel(); ++i, ++mX) {

            if (veryVerbose) {
                T_ P_(i) P_((const void*) X.node()) P_((*X).data()) P(X->data()) }

            ASSERTV(i, X.node() == &nodes[i]);
            ASSERTV(i, *X == nodes[i].value());
            ASSERTV(i, X.operator->() == &nodes[i].value());
            ASSERTV(i, X->data() == nodes[i].value().data());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING (PRIMITIVE) GENERATORS
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR & PRIMARY MANIPULATORS
        //   Ensure that we can use the default constructor to create an object
        //   (having the default constructed value).  Also ensure that we can
        //   use the primary manipulators to put that object into any state
        //   relevant for thorough testing.
        //
        // Concerns:
        //:  1 An object can be created with the default constructor.  Two
        //:    objects created with the default constructor has the same value.
        //:
        //:  2 An object created with the value constructor have the
        //:    contractually specified value.
        //:
        //:  3 An object initialized using a value constructor to a valid
        //:    'RbTreeNode' can be post-incremented using the (as yet unproven)
        //:    'operator++'.
        //:
        //:  4 An object can be used to traverse the all nodes in a tree in
        //:    order using the value constructor and the (as yet unproven)
        //:    operator++.
        //
        // Plan:
        //:  1 Construct two objects using the default constructor.  Verify
        //:    that the two objects refers to the same tree node by using the
        //:    (as yet unproven) salient attribute accessor.  (C-1)
        //:
        //:  2 Create a tree with a N nodes.
        //:
        //:  3 For each node in the tree, use the value constructor to create
        //:    an object to point to that node.  Verify the state of the object
        //:    with the (as yet unproven) salient attribute accessor.  (C-2)
        //:
        //:  4 Instantiate an object with the value constructor, passing in a
        //:    pointer to the first node of the tree.  Iterate over the nodes
        //:    of the tree with the pre-increment.  After construction and each
        //:    iteration, verify the state of the object with the (as yet
        //:    unproven) salient attribute accessor.  (C-2..4)
        //
        // Testing:
        //   TreeIterator();
        //   explicit TreeIterator(const bslalg::RbTreeNode *node);
        //   TreeIterator operator++(int);
        //   ~TreeIterator();
        // --------------------------------------------------------------------

        if (verbose) printf("\nDEFAULT CTOR & PRIMARY MANIPULATORS"
                            "\n===================================\n");

        if (verbose) printf("\nTest default constructor.\n");

        Obj D1, D2;
        ASSERTV(D1.node(), D2.node(), D1.node() == D2.node());

        if (verbose) printf("\nCreate a tree with N nodes.\n");

        const int N = 10;
        Anchor tree;
        if (verbose) P((void*) tree.sentinel());

        WrapperNodeComparator nodeComparator;

        WrapperNode nodes[N];
        for (int i = 0; i < N; ++i) {
            nodes[i].value().data() = i;
            Op::insert(&tree, nodeComparator, &nodes[i]);
        }

        if (verbose)
            printf("\nTest value constructing object pointing every node.\n");

        for (int ti = 0; ti < N; ++ti) {
            const Obj X(&nodes[ti]);
            ASSERTV(X.node(), &nodes[ti], X.node() == &nodes[ti]);
        }

        if (verbose)
            printf("\nTest value constructor and iterating over tree.\n");

        Obj iter(tree.firstNode());
        ASSERTV(iter.node(), &nodes[0], iter.node() == &nodes[0]);

        int i = 1;
        ++iter;
        while (iter.node() != tree.sentinel()) {

            if (veryVerbose) {
                T_ P_(i);
                P_((const void*) iter.node());
                P((const void*) &nodes[i]);
            }

            ASSERTV(i, iter.node(), &nodes[i], iter.node() == &nodes[i]);
            ++iter; ++i;
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef TreeIterator<MyWrapper, WrapperNode, std::ptrdiff_t> iterator;
        typedef TreeIterator<const MyWrapper, WrapperNode, std::ptrdiff_t>
                                                                const_iterator;

        {
            iterator itA;

            ASSERT(  itA == itA);
            ASSERT(!(itA != itA));

            iterator itB(itA);

            ASSERT(  itA == itB);
            ASSERT(!(itA != itB));

            const_iterator itC(itA);

            ASSERT(  itA == itC);
            ASSERT(!(itA != itC));

            const_iterator itD(itC);
            (void) itD;

            ASSERT(  itA == itC);
            ASSERT(!(itA != itC));

        }
        {
            WrapperNodeComparator nodeComparator;
            enum {
                NUM_VALUES = 10
            };
            int  VALUES[NUM_VALUES];
            for (int i = 0; i < NUM_VALUES; ++i) {
                VALUES[i] = i;
            }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
            std::shuffle(VALUES,
                         VALUES + NUM_VALUES,
                         std::default_random_engine());
#else  // fall-back for C++03, potentially unsupported in C++17
            std::random_shuffle(VALUES, VALUES + NUM_VALUES);
#endif

            Anchor tree;

            WrapperNode nodes[NUM_VALUES];
            for (int i = 0; i < NUM_VALUES; ++i) {
                nodes[i].setLeftChild(0);
                nodes[i].setRightChild(0);
                nodes[i].setParent(0);
                nodes[i].value().data() = VALUES[i];

                Op::insert(&tree, nodeComparator, &nodes[i]);
            }
            ASSERT(Op::validateRbTree(tree.rootNode(), nodeComparator));

            WrapperNode *node =
                     static_cast<WrapperNode *>(Op::leftmost(tree.rootNode()));
            do {
                WrapperNode *nextNode = 0;
                WrapperNode *prevNode = 0;

                nextNode = static_cast<WrapperNode *>(Op::next(node));

                if (node != Op::leftmost(tree.rootNode()))
                {
                    prevNode = static_cast<WrapperNode *>(Op::previous(node));
                }

                iterator it(node); const iterator& IT = it;
                const_iterator cit(node); const const_iterator& CIT = cit;

                ASSERT( node->value() == *IT);
                ASSERT( node->value() == *CIT);
                ASSERT(&node->value() == IT.operator->());
                ASSERT(&node->value() == CIT.operator->());
                ASSERT(IT == CIT);
                ASSERT(!(IT != CIT));

                // prefix increment
                {
                    iterator x(node); const iterator& X = x;
                    const_iterator cx(node); const const_iterator& CX = cx;

                    iterator y(++x); const iterator& Y = y;
                    const_iterator cy(++cx); const const_iterator& CY = cy;

                    ASSERT(X  != IT);
                    ASSERT(CX != CIT);

                    ASSERT(X  == Y);
                    ASSERT(CX == CY);

                    if (nextNode) {
                        ASSERT( nextNode->value() == *X);
                        ASSERT( nextNode->value() == *CX);
                    }
                }

                // postfix increment
                {
                    iterator x(node); const iterator& X = x;
                    const_iterator cx(node); const const_iterator& CX = cx;

                    iterator y(x++); const iterator& Y = y;
                    const_iterator cy(cx++); const const_iterator& CY = cy;

                    ASSERT(X  != IT);
                    ASSERT(CX != CIT);

                    ASSERT(X  != Y);
                    ASSERT(CX != CY);

                    ASSERT(Y  == IT);
                    ASSERT(CY == CIT);

                    if (nextNode) {
                        ASSERT( nextNode->value() == *X);
                        ASSERT( nextNode->value() == *CX);
                    }
                }

                // prefix decrement
                {
                    if (prevNode)
                    {
                        iterator x(node); const iterator& X = x;
                        const_iterator cx(node); const const_iterator& CX = cx;

                        iterator y(--x); const iterator& Y = y;
                        const_iterator cy(--cx); const const_iterator& CY = cy;

                        ASSERT(X  != IT);
                        ASSERT(CX != CIT);

                        ASSERT(X  == Y);
                        ASSERT(CX == CY);

                        if (prevNode) {
                            ASSERT( prevNode->value() == *X);
                            ASSERT( prevNode->value() == *CX);
                        }
                    }
                }
                // postfix decrement
                {
                    if (prevNode)
                    {
                        iterator x(node); const iterator& X = x;
                        const_iterator cx(node); const const_iterator& CX = cx;

                        iterator y(x--); const iterator& Y = y;
                        const_iterator cy(cx--); const const_iterator& CY = cy;


                        ASSERT(X  != IT);
                        ASSERT(CX != CIT);

                        ASSERT(X  != Y);
                        ASSERT(CX != CY);

                        ASSERT(Y  == IT);
                        ASSERT(CY == CIT);

                        if (prevNode) {
                            ASSERT( prevNode->value() == *X);
                            ASSERT( prevNode->value() == *CX);
                        }
                    }
                }

                node = nextNode;
            } while (node != tree.sentinel());
        }
       } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    // CONCERN: No memory is ever allocated.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

    return testStatus;
}

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
