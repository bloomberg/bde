// bslalg_rbtreeanchor.t.cpp                                          -*-C++-*-
#include <bslalg_rbtreeanchor.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <algorithm>

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bslalg;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a class that is similar to an in-core
// simply constrained (value-semantic) attribute class, except that it does
// not supply equality-comparison, copy-construction, and copy-assignment
// operations.  The class provides access to the address of root, first, and
// sentinel nodes of a tree along with the count of the number of nodes.  The
// the address of root node, the first node, and the node count can be
// modified and accessed through the attribute getters and setters, while the
// address of sentinel node is fixed.
//
// The attribute getters and setters are the primary manipulators and basic
// accessors of this class and follows our standard attribute-type naming
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'setFirstNode'
//: o 'setRootNode'
//: o 'setNumNodes'
//
// Basic Accessors:
//: o 'firstNode'
//: o 'rootNode'
//: o 'numNodes'
//: o 'sentinel'
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  In
// addition, the standard tests cases associated with equality-comparison,
// copy-construction, copy-assignment, and bdex-externalization are not
// included because these operations are not implemented by this class.  We
// will therefore implement only the first 4 test-cases (breathing test,
// primary manipulators, primary tests generator function, and basic accessor)
// that are relevant to this class from the standard 10-case approach to
// testing value-semantic types, and we will test the value constructor in lieu
// of the generator function in case 3.
//
// Global Concerns:
//: o No memory is ever allocated.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] RbTreeAnchor();
// [ 3] RbTreeAnchor(RbTreeNode *root, RbTreeNode *first, int numNodes);
// [ 2] ~RbTreeAnchor();
//
// MANIPULATORS
// [ 3] void reset(RbTreeNode *root, RbTreeNode *first, int numNodes);
// [ 2] void setFirstNode(RbTreeNode *value);
// [ 2] void setRootNode(RbTreeNode *value);
// [ 2] void setNumNodes(int value);
// [ 6] void incrementNumNodes();
// [ 6] void decrementNumNodes();
//
// ACCESSORS
// [ 4] RbTreeNode *firstNode();
// [ 4] RbTreeNode *rootNode();
// [ 4] const RbTreeNode *firstNode() const;
// [ 4] const RbTreeNode *rootNode() const;
// [ 4] int numNodes() const;
// [ 5] const RbTreeNode *sentinel() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//-----------------------------------------------------------------------------
//=============================================================================

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

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

// Fundamental-type-specific print functions.
namespace BloombergLP {
namespace bslalg {

inline void debugprint(const RbTreeAnchor& val) {
    printf("RbTreeAnchor (rootNode: %p, firstNode: %p, numNodes: %d)\n",
           val.rootNode(), val.firstNode(), val.numNodes());
    fflush(stdout);
}
inline void debugprint(const RbTreeNode *val) {
    printf("%p", val);
    fflush(stdout);
}

}  // close package namespace
}  // close enterprise namespace


//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef RbTreeAnchor Obj;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
    struct IntTreeNode : public RbTreeNode {
        // A red-black tree node containing an integer data-value.

        int d_value;  // "payload" value represented by the node
    };
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
struct IntTreeNodeComparator {
    // This class defines a comparator providing a comparison operation
    // between two 'IntTreeNode' objects.

    bool operator()(const RbTreeNode& lhs, const RbTreeNode& rhs)  const
    {
        return static_cast<const IntTreeNode&>(lhs).d_value <
               static_cast<const IntTreeNode&>(rhs).d_value;
    }
};
//..
// Then, we declare the signature of a function 'insertNode', which takes
// three arguments: (1) the anchor of the tree in which to insert the node (2)
// the new node to insert into the tree, and (3) a comparator, which is used
// to compare the payload values of the tree nodes.  Note that the
// parameterized comparator is needed because a node's value is not accessible
// through the supplied 'RbTreeNode'.
//..
    template <class NODE_COMPARATOR>
    void insertNode(RbTreeAnchor           *searchTree,
                    RbTreeNode             *newNode,
                    const NODE_COMPARATOR&  comparator)
        // Insert into the specified 'searchTree', ordered according to the
        // specified 'comparator', the specified 'newNode'.  If there are
        // multiple nodes having the same value as 'newNode', insert 'newNode'
        // in the last position according to an infix traversal of the tree.
        // The behavior is undefined unless the 'comparator' provides a
        // strict-weak ordering on the nodes in the tree.
    {
//..
// Next, we find the location where 'newNode' can be inserted into 'searchTree'
// without violating the ordering imposed by 'comparator', and then updates
// 'searchTree' with a potentially updated root node and first node.
//..
        RbTreeNode *parent = searchTree->sentinel();
        RbTreeNode *node   = searchTree->rootNode();
        bool        isLeftChild;

        newNode->setLeftChild(0);
        newNode->setRightChild(0);

        if (!node) {
//..
// If the root node of 'searchTree' is 0, we use the 'reset' function set the
// root node and the first node of 'searchTree' to 'newNode' and set the number
// of nodes to 1:
//..
            searchTree->reset(newNode, newNode, 1);
            newNode->setParent(parent);
            return;                                                   // RETURN
        }

        // Find the leaf node that would be a valid parent of 'newNode'.

        do {
            parent = node;
            isLeftChild = comparator(*newNode, *node);
            if (isLeftChild) {
                node = node->leftChild();
            }
            else {
                node = node->rightChild();
            }
        } while (node);

        // Insert 'newNode' into 'searchTree' and the location that's been
        // found.
//..
// Then, we insert 'newNode' into the appropriate position by setting it as a
// child of 'parent':
//..
        if (isLeftChild) {
            // If 'newNode' is a left-child, it may be the new first node, but
            // cannot be the new last node.

            parent->setLeftChild(newNode);
            newNode->setParent(parent);
            if (parent == searchTree->firstNode()) {
                searchTree->setFirstNode(newNode);
            }
        }
        else {
            parent->setRightChild(newNode);
            newNode->setParent(parent);
        }
//..
// Next, we complete the insert function by incrementing the number of nodes in
// the tree:
//..
        searchTree->incrementNumNodes();
    }
//..

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: No memory is ever allocated.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------


        if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");

// Then, we define 'main' for our test, and create three nodes that we'll use
// to construct a tree:
//..
//    int main(int argc, const char *argv[])
        {
            IntTreeNode A, B, C;
//..
// Next, we create an 'RbTreeAnchor', 'myTree', which will hold the addresses
// of the root node and the first node of our tree along with a count of nodes,
// and then verify the attribute values of the default constructed object:
//..
            RbTreeAnchor myTree;
            ASSERT(0                 == myTree.rootNode());
            ASSERT(myTree.sentinel() == myTree.firstNode());
            ASSERT(0                 == myTree.numNodes());
//..
// Next, we describe the structure of the tree we wish to construct.
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
            A.d_value = 2;
            A.makeBlack();
            A.setParent(myTree.sentinel());
            A.setLeftChild(&B);
            A.setRightChild(&C);

            B.d_value = 1;
            B.makeRed();
            B.setParent(&A);
            B.setLeftChild(0);
            B.setRightChild(0);

            C.d_value = 3;
            C.makeRed();
            C.setParent(&A);
            C.setLeftChild(0);
            C.setRightChild(0);
//..
// Now, we assign the address of 'A' and 'B' as the root node and the first
// node of 'myTree' respectively and set the number of nodes to 3:
//..
            myTree.reset(&A, &B, 3);
//..
// Finally, we verify the attributes of 'myTree':
//..
            ASSERT(&A == myTree.rootNode());
            ASSERT(&B == myTree.firstNode());
            ASSERT(3  == myTree.numNodes());
//..
        }
        {
//..
// Now, we create 5 'IntTreeNode' objects and insert them into a tree using the
// 'insertNode' function.
//..
            IntTreeNode nodes[5];

            nodes[0].d_value = 3;
            nodes[1].d_value = 1;
            nodes[2].d_value = 5;
            nodes[3].d_value = 2;
            nodes[4].d_value = 0;

            IntTreeNodeComparator comparator;

            RbTreeAnchor anchor;
            for (int i = 0; i < 5; ++i) {
                insertNode(&anchor, nodes + i, comparator);
            }
//..
// Finally, we verify that the 'RbTreeAnchor' refers to the correct 'TreeNode'
// with its 'firstNode' and 'rootNode' attributes.
//..
            ASSERT(
                0 == static_cast<IntTreeNode *>(anchor.firstNode())->d_value);
            ASSERT(
                3 == static_cast<IntTreeNode *>(anchor.rootNode())->d_value);
        }
//..
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // MANIPULATORS 'incrementNumNodes' & 'decrementNumNodes'
        //   Ensure that 'incrementNumNodes' and 'decrementNumNodes' behave
        //   according to their contracts.
        //
        // Concerns:
        //:  1 Calling 'incrementNumNodes' increases 'numNodes' by 1.
        //:
        //:  2 Calling 'decrementNumNodes' decreases 'numNodes' by 1.
        //
        // Plan:
        //:  1 Default construct an object and verify that 'numNodes' is 0.
        //:
        //:  2 Call 'incrementNumNodes' a few times and verify that 'numNodes'
        //:    is correct.  (C-1)
        //:
        //:  3 Call 'decrementNumNodes' a few times and verify that 'numNodes'
        //:    is correct.  (C-2)
        //
        // Testing:
        //   void incrementNumNodes();
        //   void decrementNumNodes();
        // --------------------------------------------------------------------

        if (verbose) printf("\nMANIPULATORS 'incrementNumNodes' & "
                            "'decrementNumNodes'"
                            "\n==================================="
                            "===================\n");

        Obj mX; const Obj& X = mX;

        ASSERTV(0, X.numNodes(), 0 == X.numNodes());

        mX.incrementNumNodes();

        ASSERTV(1, X.numNodes(), 1 == X.numNodes());

        mX.incrementNumNodes();

        ASSERTV(2, X.numNodes(), 2 == X.numNodes());

        mX.decrementNumNodes();

        ASSERTV(1, X.numNodes(), 1 == X.numNodes());

        mX.decrementNumNodes();

        ASSERTV(0, X.numNodes(), 0 == X.numNodes());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // ACCESSOR 'sentinel':
        //   Ensure that invariant of the sentinel node always hold.
        //
        // Concerns:
        //:  1 'leftChild' of the node returned by the 'sentinel' member
        //:    function refers to the same 'RbTreeNode' as 'rootNode'.
        //:
        //:  2 'rightChild' of the node returned by the 'sentinel' member
        //:    function refers to the same 'RbTreeNode' as 'firstNode'.
        //:
        //:  3 Two function overloads exist for the 'sentinel' member
        //:    function: a 'const' function returning a non-modifiable
        //:    reference, and a non-'const' function returning a modifiable
        //:    reference.
        //
        // Plan:
        //:  1 Default construct an object and verify that C-1 and C-2 holds
        //:    with both the 'const' and the non-'const' 'sentinel' member
        //:    function.  (C-1..3)
        //:
        //:  2 Change the 'rootNode' attribute using the attribute setter,
        //:    verify that C-1 and C-2 still holds.  (C-1..3)
        //:
        //:  3 Change the 'firstNode' attribute using the attribute setter,
        //:    verify that C-1 and C-2 still holds.  (C-1..3)
        //
        // Testing:
        //   const RbTreeNode *sentinel() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nACCESSOR 'sentinel'"
                            "\n===================\n");

        Obj mX; const Obj& X = mX;

        ASSERTV(X.sentinel()->leftChild(), X.rootNode(),
                X.sentinel()->leftChild() == X.rootNode());
        ASSERTV(mX.sentinel()->leftChild(), mX.rootNode(),
                mX.sentinel()->leftChild() == mX.rootNode());
        ASSERTV(X.sentinel()->rightChild(), X.firstNode(),
                X.sentinel()->rightChild() == X.firstNode());
        ASSERTV(mX.sentinel()->rightChild(), mX.firstNode(),
                mX.sentinel()->rightChild() == mX.firstNode());

        RbTreeNode n1, n2;

        mX.setRootNode(&n1);

        ASSERTV(X.sentinel()->leftChild(), X.rootNode(),
                X.sentinel()->leftChild() == X.rootNode());
        ASSERTV(mX.sentinel()->leftChild(), mX.rootNode(),
                mX.sentinel()->leftChild() == mX.rootNode());
        ASSERTV(X.sentinel()->rightChild(), X.firstNode(),
                X.sentinel()->rightChild() == X.firstNode());
        ASSERTV(mX.sentinel()->rightChild(), mX.firstNode(),
                mX.sentinel()->rightChild() == mX.firstNode());

        mX.setFirstNode(&n2);

        ASSERTV(X.sentinel()->leftChild(), X.rootNode(),
                X.sentinel()->leftChild() == X.rootNode());
        ASSERTV(mX.sentinel()->leftChild(), mX.rootNode(),
                mX.sentinel()->leftChild() == mX.rootNode());
        ASSERTV(X.sentinel()->rightChild(), X.firstNode(),
                X.sentinel()->rightChild() == X.firstNode());
        ASSERTV(mX.sentinel()->rightChild(), mX.firstNode(),
                mX.sentinel()->rightChild() == mX.firstNode());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //:  1 Each accessor returns the value of the corresponding attribute
        //:    of the object.
        //:
        //:  2 Two function overloads exist for each accessor returning a
        //:    pointer to 'RbTreeNode': a 'const' function returning a
        //:    non-modifiable reference, and a non-'const' function returning
        //:    a modifiable reference.
        //
        // Plan:
        //: 1 Use the default constructor, create an object having default
        //:   attribute values.
        //:
        //: 2 Verify that each basic accessor, invoked on either a reference
        //:   providing non-modifiable access or a reference providing
        //:   modifiable access to the object created in P1, returns the
        //:   expected value.  (C-2)
        //:
        //: 3 For each salient attribute (contributing to value):  (C-1)
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //
        // Testing:
        //   RbTreeNode *firstNode();
        //   RbTreeNode *rootNode();
        //   const RbTreeNode *firstNode() const;
        //   const RbTreeNode *rootNode() const;
        //   int numNodes() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        RbTreeNode n1, n2;

        // 'D' values: These are the default-constructed values.

        RbTreeNode       *mD1 = 0;
        const RbTreeNode *D1  = mD1;
        const int         D3  = 0;

        // 'A' values

        RbTreeNode       *mA1 = &n1;
        RbTreeNode       *mA2 = &n2;
        const RbTreeNode *A1  = mA1;
        const RbTreeNode *A2  = mA2;
        const int         A3  = 10;

        // default constructed object

        if (verbose)
            printf("\nCreate an object with the default constructor\n");
        Obj mX; const Obj& X = mX;

        const RbTreeNode* D2  = X.sentinel();
        RbTreeNode*       mD2 = const_cast<RbTreeNode*>(D2);

        if (verbose)
            printf("\nVerify all basic accessors report expected values.");
        {
            // non-modifiable attribute accessors
            ASSERTV(D1, X.rootNode(), D1 == X.rootNode());
            ASSERTV(D2, X.rootNode(), D2 == X.firstNode());
            ASSERTV(D3, X.rootNode(), D3 == X.numNodes());

            // modifiable attribute accessors
            ASSERTV(mD1, mX.rootNode(), mD1 == mX.rootNode());
            ASSERTV(mD2, mX.rootNode(), mD2 == mX.firstNode());
        }

        if (verbose)
            printf("\nApply primary manipulators "
                   "and verify expected values.\n");

        if (veryVerbose) { T_ Q(rootNode) }
        {
            mX.setRootNode(mA1);

            ASSERTV(A1, X.rootNode(), A1 == X.rootNode());
            ASSERTV(mA1, mX.rootNode(), mA1 == mX.rootNode());
        }

        if (veryVerbose) { T_ Q(firstNode) }
        {
            mX.setFirstNode(mA2);

            ASSERTV(A2, X.firstNode(), A2 == X.firstNode());
            ASSERTV(mA2, mX.firstNode(), mA2 == mX.firstNode());
        }

        if (veryVerbose) { T_ Q(numNodes) }
        {
            mX.setNumNodes(A3);

            ASSERTV(A3, X.numNodes(), A3 == X.numNodes());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CTOR & MANIPULATOR 'reset'
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing by either constructing the object with the
        //   value constructor or default constructing the object and changing
        //   its state using the 'reset' member function.
        //
        // Concerns:
        //: 1 The value constructor can create an object having any value that
        //:   does not violate the documented constraints.
        //:
        //: 2 The 'reset' member function can change an object to have any
        //:   value that does not violate the the documented constraints.
        //:
        //: 3 'numNodes' can be const.
        //
        // Plan:
        //
        //: 1 Using the table-driven technique, specify a set of (unique)
        //:   valid object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.  Note that the
        //:   default attributes have the same values as a default constructed
        //:   object, except for the 'rootNode' attribute, which is set to '0'
        //:   instead of the address of the sentinel node in a default
        //:   constructed object.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:
        //:
        //:   1 Use the value constructor to dynamically create an object 'w'
        //:     having the value 'V', supplying all the arguments as 'const'.
        //:     (C-3).
        //:
        //:   2 Use the (as yet unproven) salient attribute accessors to
        //:     verify that all of the attributes of the object 'w' have their
        //:     expected values.  (C-1)
        //:
        //:   3 Use the default constructor to create another object
        //:     'x'.  Then, call 'reset' on that object, supplying 'numNodes'
        //:     as 'const'.  (C-3).
        //:
        //:   4 Use the (as yet unproven) salient attribute accessors to
        //:     verify that all of the attributes of the object 'x' have their
        //:     expected values.  (C-2)
        //
        //
        // Testing:
        //   RbTreeAnchor(RbTreeNode *root, RbTreeNode *first, int numNodes);
        //   void reset(RbTreeNode *root, RbTreeNode *first, int numNodes);
        // --------------------------------------------------------------------


        if (verbose) printf("\nVALUE CTOR & MANIPULATOR 'reset'"
                            "\n================================\n");

        static RbTreeNode n1, n2, n3, n4;

        static const struct {
            int         d_line;              // source line number
            RbTreeNode *d_rootNode;
            RbTreeNode *d_firstNode;
            int         d_numNodes;
        } DATA[] = {
            //LINE  ROOT  FIRST  NUMNODES
            //----  ----  -----  --------

            // default
            { L_,      0,     0,       0 },

            // 'rootNode'
            { L_,    &n1,     0,       0 },
            { L_,    &n2,     0,       0 },

            // 'firstNode'
            { L_,      0,   &n3,       0 },
            { L_,      0,   &n4,       0 },

            // 'numNodes'
            { L_,      0,     0,      10 },
            { L_,      0,     0, INT_MAX },
        };


        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti)
        {
            const int   LINE     = DATA[ti].d_line;
            RbTreeNode* ROOT     = DATA[ti].d_rootNode;
            RbTreeNode* FIRST    = DATA[ti].d_firstNode;
            const int   NUMNODES = DATA[ti].d_numNodes;

            if (veryVerbose) { T_ P_(LINE) P_(ROOT) P_(FIRST) P(NUMNODES) }

            Obj mW(ROOT, FIRST, NUMNODES); Obj& W = mW;

            ASSERTV(ROOT, W.rootNode(), ROOT == W.rootNode());
            ASSERTV(FIRST, W.firstNode(), FIRST == W.firstNode());
            ASSERTV(NUMNODES, W.numNodes(), NUMNODES == W.numNodes());

            Obj mX; Obj& X = mX;
            mX.reset(ROOT, FIRST, NUMNODES);

            ASSERTV(ROOT, X.rootNode(), ROOT == X.rootNode());
            ASSERTV(FIRST, X.firstNode(), FIRST == X.firstNode());
            ASSERTV(NUMNODES, X.numNodes(), NUMNODES == X.numNodes());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR & PRIMARY MANIPULATORS
        //   Ensure that we can use default constructor to create an object
        //   (having the default-constructed value).  Also ensure that we can
        //   primary manipulators to put that object into any state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 Each attribute is modifiable independently.
        //:
        //: 3 The argument to 'setNumNodes' can be const.
        //:
        //: 4 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: ('D')
        //:   values corresponding to the default-constructed object, ('A')
        //:   and ('B') values corresponding to the boundary values where
        //:   possible.
        //:
        //: 2 Default-construct an object and use the individual (as yet
        //:   unproven) salient attribute accessors to verify the
        //:   default-constructed value.  (C-1)
        //:
        //: 3 For each attribute 'i', in turn, create a local block.  Then
        //:   inside the block, using brute force, set that attribute's value,
        //:   passing a 'const' argument representing each of the three test
        //:   values, in turn (see P-1), first to 'Ai', then to 'Bi', and
        //:   finally back to 'Di'.  After each transition, use the (as yet
        //:   unproven) basic accessors to verify that only the intended
        //:   attribute value changed.  (C-3..4)
        //:
        //: 4 Corroborate that attributes are modifiable independently by
        //:   first setting all of the attributes to their 'A' values.  Then
        //:   incrementally set each attribute to it's corresponding  'B'
        //:   value and verify after each manipulation that only that
        //:   attribute's value changed.  (C-2)
        //
        // Testing:
        //   RbTreeAnchor();
        //   ~RbTreeAnchor();
        //   void setFirstNode(RbTreeNode *value);
        //   void setRootNode(RbTreeNode *value);
        //   void setNumNodes(int value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nDEFAULT CTOR & PRIMARY MANIPULATORS"
                            "\n===================================\n");

        if (verbose) printf("\nEstablish suitable attribute values.\n");


        RbTreeNode n1, n2, n3, n4;

        // 'D' values: These are the default-constructed values.

        RbTreeNode       *mD1 = 0;
        const RbTreeNode *D1  = mD1;
        const int         D3  = 0;

        // 'A' values

        RbTreeNode       *mA1 = &n1;
        RbTreeNode       *mA2 = &n2;
        const RbTreeNode *A1  = mA1;
        const RbTreeNode *A2  = mA2;
        const int         A3  = 10;

        // 'B' values

        RbTreeNode       *mB1 = &n3;
        RbTreeNode       *mB2 = &n4;
        const RbTreeNode *B1  = mB1;
        const RbTreeNode *B2  = mB2;
        const int         B3  = INT_MAX;

        Obj mX;  const Obj& X = mX;

        const RbTreeNode* D2  = X.sentinel();
        RbTreeNode*       mD2 = const_cast<RbTreeNode*>(D2);

        ASSERTV(D1, X.rootNode(), D1 == X.rootNode());
        ASSERTV(D2, X.rootNode(), D2 == X.firstNode());
        ASSERTV(D3, X.rootNode(), D3 == X.numNodes());

        // 'rootNode'
        {
            mX.setRootNode(mA1);
            ASSERTV(A1, X.rootNode(), A1 == X.rootNode());
            ASSERTV(D2, X.firstNode(), D2 == X.firstNode());
            ASSERTV(D3, X.numNodes(), D3 == X.numNodes());

            mX.setRootNode(mB1);
            ASSERTV(B1, X.rootNode(), B1 == X.rootNode());
            ASSERTV(D2, X.firstNode(), D2 == X.firstNode());
            ASSERTV(D3, X.numNodes(), D3 == X.numNodes());

            mX.setRootNode(mD1);
            ASSERTV(D1, X.rootNode(), D1 == X.rootNode());
            ASSERTV(D2, X.firstNode(), D2 == X.firstNode());
            ASSERTV(D3, X.numNodes(), D3 == X.numNodes());
        }

        // 'firstNode'
        {
            mX.setFirstNode(mA2);
            ASSERTV(D1, X.rootNode(), D1 == X.rootNode());
            ASSERTV(A2, X.firstNode(), A2 == X.firstNode());
            ASSERTV(D3, X.numNodes(), D3 == X.numNodes());

            mX.setFirstNode(mB2);
            ASSERTV(D1, X.rootNode(), D1 == X.rootNode());
            ASSERTV(B2, X.firstNode(), B2 == X.firstNode());
            ASSERTV(D3, X.numNodes(), D3 == X.numNodes());

            mX.setFirstNode(mD2);
            ASSERTV(D1, X.rootNode(), D1 == X.rootNode());
            ASSERTV(D2, X.firstNode(), D2 == X.firstNode());
            ASSERTV(D3, X.numNodes(), D3 == X.numNodes());
        }

        // 'numNodes'
        {
            mX.setNumNodes(A3);
            ASSERTV(D1, X.rootNode(), D1 == X.rootNode());
            ASSERTV(D2, X.firstNode(), D2 == X.firstNode());
            ASSERTV(A3, X.numNodes(), A3 == X.numNodes());

            mX.setNumNodes(B3);
            ASSERTV(D1, X.rootNode(), D1 == X.rootNode());
            ASSERTV(D2, X.firstNode(), D2 == X.firstNode());
            ASSERTV(B3, X.numNodes(), B3 == X.numNodes());

            mX.setNumNodes(D3);
            ASSERTV(D1, X.rootNode(), D1 == X.rootNode());
            ASSERTV(D2, X.firstNode(), D2 == X.firstNode());
            ASSERTV(D3, X.numNodes(), D3 == X.numNodes());
        }

        // Corroborate attribute independence.
        {
            // Set all attributes to their 'A' values.

            mX.setRootNode(mA1);
            mX.setFirstNode(mA2);
            mX.setNumNodes(A3);

            ASSERTV(A1, X.rootNode(), A1 == X.rootNode());
            ASSERTV(A2, X.firstNode(), A2 == X.firstNode());
            ASSERTV(A3, X.numNodes(), A3 == X.numNodes());

            // Set all attributes to their 'B' values.

            mX.setRootNode(mB1);
            ASSERTV(B1, X.rootNode(), B1 == X.rootNode());
            ASSERTV(A2, X.firstNode(), A2 == X.firstNode());
            ASSERTV(A3, X.numNodes(), A3 == X.numNodes());

            mX.setFirstNode(mB2);
            ASSERTV(B1, X.rootNode(), B1 == X.rootNode());
            ASSERTV(B2, X.firstNode(), B2 == X.firstNode());
            ASSERTV(A3, X.numNodes(), A3 == X.numNodes());

            mX.setNumNodes(B3);
            ASSERTV(B1, X.rootNode(), B1 == X.rootNode());
            ASSERTV(B2, X.firstNode(), B2 == X.firstNode());
            ASSERTV(B3, X.numNodes(), B3 == X.numNodes());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an object 'w' (default ctor).       { w:D     }
        //: 2 Set 'w' to 'A' (value distinct from 'D').  { w:A     }
        //: 3 Create an object 'x' (init. to 'A').       { w:A x:A }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
        // Nodes to use
        RbTreeNode n1, n2;

        // Attribute Types

        typedef RbTreeNode* T1; // 'rootNode'
        typedef RbTreeNode* T2; // 'firstNode'
        typedef int         T3; // 'numNodes'

        // Attribute 1 Values: 'rootNode'

        const T1 D1 = 0;
        const T1 A1 = &n1;

        // Attribute 2 Values: 'firstNode'

        // D2 is the address of the sentinel node
        const T2 A2 = &n2;

        // Attribute 3 Values: 'numNodes'

        const T3 D3 = 0;
        const T3 A3 = 2;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n 1. Create an object 'w' (default ctor)."
                            "\t\t{ w:D     }\n");

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) printf("\ta. Check initial value of 'w'.\n");
        if (veryVeryVerbose) { T_ T_ P(W) }

        const RbTreeNode* D2 = W.sentinel();

        ASSERT(D1 == W.rootNode());
        ASSERT(D2 == W.firstNode());
        ASSERT(D3 == W.numNodes());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n 2. Set 'w' to 'A' (value distinct from 'D')."
                            "\t\t{ w:A     }\n");

        mW.setRootNode(A1);
        mW.setFirstNode(A2);
        mW.setNumNodes(A3);

        if (veryVerbose) printf("\ta. Check initial value of 'w'.\n");
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(A1 == W.rootNode());
        ASSERT(A2 == W.firstNode());
        ASSERT(A3 == W.numNodes());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n 1. Create an object 'x' (init. to 'A')."
                            "\t\t{ w:A x:A }\n");

        Obj mX(A1, A2, A3);  const Obj& X = mX;

        if (veryVerbose) printf("\ta. Check initial value of 'x'.\n");
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.rootNode());
        ASSERT(A2 == X.firstNode());
        ASSERT(A3 == X.numNodes());

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: No memory is ever allocated.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());


    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
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
