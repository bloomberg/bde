// bslalg_rbtreenode.t.cpp                                            -*-C++-*-

#include <bslalg_rbtreenode.h>

#include <bslalg_arrayprimitives.h>
#include <bslalg_arraydestructionprimitives.h>
#include <bslalg_swaputil.h>
#include <bslma_allocator.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <algorithm>

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bslalg;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single, simply constrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o setColor
//: o setParent
//: o setLeftChild
//: o setRightChild
//
// Basic Accessors:
//: o color
//: o parent
//: o leftChild
//: o rightChild
//
// Certain standard value-semantic-type test cases are omitted:
//: o [ 8] -- 'swap' is not implemented for this class.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o Precondition violations are detected in appropriate build modes.
//: o No memory is allocated from any allocator.
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] bslalg::RbTreeNode
// [ 6] bslalg::RbTreeNode(const bslalg::RbTreeNode& original);
// [ 2] ~bslalg::RbTreeNode();
//
// MANIPULATORS
// [ 7] RbTreeNode& operator=(const bslalg::RbTreeNode& rhs);
// [ 2] void setColor(Color color);
// [ 2] void setParent(bslalg::RbTreeNode *parent);
// [ 2] void setLeftChild(bslalg::RbTreeNode *leftChild);
// [ 2] void setRightChild(bslalg::RbTreeNode *rightChild);
// [ 5] void makeBlack();
// [ 5] void makeRed();
// [ 5] void toggleColor();
// [ 9] void reset();
// [ 8] RbTreeNode *parent();
// [ 8] RbTreeNode *leftChild();
// [ 8] RbTreeNode *rightChild();
//
// ACCESSORS
// [ 4] Color color() const;
// [ 4] const RbTreeNode *parent() const;
// [ 4] const RbTreeNode *leftChild() const;
// [ 4] const RbTreeNode *rightChild() const;
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
// [ 3] 'gg' FUNCTION
// [ 2] CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
// [ 4] CONCERN: All accessor methods are declared 'const'.
// [ 2] CONCERN: Precondition violations are detected when enabled.

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

#define ASSERT_FAIL(expr)      BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr)      BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef RbTreeNode        Obj;
typedef RbTreeNode::Color Color;

const RbTreeNode::Color RED   = RbTreeNode::BSLALG_RED;
const RbTreeNode::Color BLACK = RbTreeNode::BSLALG_BLACK;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
namespace BloombergLP {
namespace bslalg {

void debugprint(const Obj& val) {
    printf("color=%d, parent=0x%p, left=0x%p, right=0x%p",
            val.color(), val.parent(), val.leftChild(), val.rightChild());
}

}  // close package namespace
}  // close enterprise namespace

static Obj& gg(Obj *result, Color color, Obj *parent, Obj *left, Obj *right)
    // Initialize the specified 'result' with the specified 'color', 'parent',
    // 'left', and 'right'.
{
    result->setColor(color);
    result->setParent(parent);
    result->setLeftChild(left);
    result->setRightChild(right);
    return *result;
}

static bool operator==(const Obj& lhs, const Obj& rhs)
    // Convenience function to verify all attributes of the specified 'lhs' has
    // the same value as the attributes of the specified 'rhs'.
{
    return (lhs.color()      == rhs.color()
         && lhs.parent()     == rhs.parent()
         && lhs.leftChild()  == rhs.leftChild()
         && lhs.rightChild() == rhs.rightChild());
}

static bool operator!=(const Obj& lhs, const Obj& rhs)
    // Convenience function to verify at least one attribute of the specified
    // 'lhs' has different value from the attributes of the specified 'rhs'.
{
    return !(lhs == rhs);
}

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA (potentially) used by test cases 3, 7, (8), 9, and (10)

struct DefaultDataRow {
    int    d_line;           // source line number
    Color  d_color;
    Obj *  d_parent;
    Obj *  d_leftChild;
    Obj *  d_rightChild;
};

Obj *const PTR1 = (Obj *)0xbaadf00ddeadc0deULL;
Obj *const PTR2 = (Obj *)0xbaadf00ddeadbeefULL;

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE  COLOR      PARENT        LEFT       RIGHT
    //----  -----      ------        ----       -----

    { L_,   RED,            0,          0,          0 },

    // 'color'
    { L_,   BLACK,          0,          0,          0 },

    // 'parent'
    { L_,   RED,     (Obj *)4,          0,          0 },
    { L_,   RED,         PTR1,          0,          0 },

    // 'leftNode'
    { L_,   RED,            0,   (Obj *)4,          0 },
    { L_,   RED,            0,       PTR2,          0 },

    // 'rightNode'
    { L_,   RED,            0,          0,   (Obj *)4 },
    { L_,   RED,            0,          0,       PTR2 },

    // other
    { L_,   BLACK,   (Obj *)4,   (Obj *)4,   (Obj *)4 },
    { L_,   BLACK,       PTR1,       PTR2,       PTR2 },
};
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating a Function to Print a Red Black Tree
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates creating a function that prints, to a 'FILE', a
// tree of 'RbTreeNode' objects.
//
// First, we define the signature to a function, 'printTree', that accepts, in
// addition to an output file and root node, a function pointer argument
// (supplied by clients) used to print each node's value.  Note that a node's
// value is not accessible through 'RbTreeNode':
//..
    void printTree(FILE             *output,
                   const RbTreeNode *rootNode,
                   void (*printNodeValueCallback)(FILE *, const RbTreeNode *))
    {
//..
// Now, we define the body of 'printTree', which is a recursive function that
// performs a prefix traversal of the supplied binary tree, printing the value
// and color of 'rootNode' before recursively printing its left and then the
// right sub-trees.
//..
        if (0 == rootNode) {
            return;                                                   // RETURN
        }
        fprintf(output, " [ ");
//
        // Print the value and color of 'rootNode'.
//
        printNodeValueCallback(output, rootNode);
        fprintf(output,
                ": %s",
                rootNode->color() == RbTreeNode::BSLALG_RED ? "RED" : "BLACK");
//
        // Recursively call 'printTree' on the left and right sub-trees.
//
        printTree(output, rootNode->leftChild(), printNodeValueCallback);
        printTree(output, rootNode->rightChild(), printNodeValueCallback);
        fprintf(output, " ]");
    }
//..
// Finally, notice that we use 'FILE' in the context of this usage example to
// avoid a dependency of standard library streams.  We will use 'printTree' to
// print a description of a tree in the next example.
//
///Example 2: Creating a Simple Red-Black Tree
///- - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates creating a simple tree of integer values using
// 'RbTreeNode'.  Note that, in practice, clients should use associated
// utilities to manage such a tree (see 'bslalg_rbtreenodeprimitives').
//
// First, we define a node-type 'IntTreeNode' that inherits from 'RbTreeNode':
//..
    struct IntTreeNode : public RbTreeNode {
        // A red-black tree node containing an integer data-value.
//
        int d_value;  // "payload" value represented by the node
    };
//..
// Then, we define a function 'printIntNodeValue' to print the value of an
// integer node.  Note that this function's signature matches that
// required by 'printTree' (defined in the preceding example):
//..
    void printIntTreeNodeValue(FILE *output, const RbTreeNode *node)
    {
       ASSERT(0 != node);
//
       fprintf(output, "%d", static_cast<const IntTreeNode*>(node)->d_value);
    }
//..

///Example 3: Creating a Function To Validate a Red-Black Tree
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates creating a function to validate the properties of
// a red-black tree.
//
// First, we declare the signature of a function 'validateRbTree', which takes
// two arguments, the address to the root node of a tree, and a comparator
// function, which is used to compare the payload values of the tree nodes.
// Note that the parameterized comparator is needed because a node's value is
// not accessible through the supplied 'RbTreeNode', and that the functor is,
// by convention, supplied by value, which may be more efficient in the case
// of state-less function objects.
//..
    template <class NODE_COMPARATOR>
    int validateRbTree(const RbTreeNode *rootNode,
                       NODE_COMPARATOR   comparator);
        // Return the number of black nodes between 'rootNode' and each (and
        // every) leaf in the tree rooted at 'rootNode', and a negative number
        // if 'rootNode' does not refer to a valid red-black binary-search tree
        // that is ordered according to the specified 'comparator'.  If
        // 'rootNode' is 0, then return 0.  'rootNode' is considered a valid
        // red-black binary search-tree if it obeys the following rules:
        //
        //: 1 All nodes in the left sub-tree of 'rootNode' are ordered at or
        //:   before 'rootNode', and all nodes in right sub-tree of
        //:   'rootNode', are ordered at or after node, as determined by
        //:   'comparator'.
        //:
        //: 2 Both children of 'rootNode' refer to 'rootNode' as a parent.
        //:
        //: 3 If 'rootNode' is red, its children are either black or 0.
        //:
        //: 4 Every path from 'rootNode' to a leaf contains the same number of
        //:   black nodes.
        //:
        //: 5 Rules (1-4) are obeyed, recursively, by the left and right
        //:   sub-trees of 'rootNode'.
        //
        // Note that this particular specification of a the contraints on a
        // red-black tree does not require the presense of, black colored, NIL
        // leaf nodes, instead NULL children are implictly assumed to be NIL
        // leaf nodes (as typically is the case for C/C++ implementations).
        // This specification also does not require the root node to be
        // colored black, as there's no practical benefit to enforcing that
        // constraint.
//..
// Then, we declare the signature for an auxiliary function.  The auxiliary
// function accepts an additional minimum node value and a maximum node value,
// and is needed to recursively apply rule 1.
//..
    template <class NODE_COMPARATOR>
    int validateRbTreeRaw(const RbTreeNode *rootNode,
                          const RbTreeNode *minNodeValue,
                          const RbTreeNode *maxNodeValue,
                          NODE_COMPARATOR   comparator);
        // Return the number of black nodes between 'rootNode' and each (and
        // every) leaf in the tree rooted at 'rootNode', and a negative number
        // if either: (1) 'rootNode' does not refer to a valid red-black
        // binary search tree that is ordered according to the specified
        // 'comparator', (2) if 'minNodeValue' is not 0 and all nodes in the
        // tree are at or after 'minNodeValue', and (3) if 'maxNodeValue' is
        // not 0 and  all nodes in the tree are at or before 'maxNodeValue'.
//..
// Next, we define the implementation of 'validateRbTree', which simply
// delegates to 'validateRbTreeRaw'.
//..
    template <class NODE_COMPARATOR>
    int validateRbTree(const RbTreeNode *rootNode,
                       NODE_COMPARATOR   comparator)
    {
        return validateRbTreeRaw(rootNode, 0, 0, comparator);
    }
//..
// Now, we define the implementation of 'validateRbTreeRaw', which tests if
// 'rootNode' (and its immediate descendent) violate any of rules, and then
// recursively calls 'validateRbTreeAux' on its left and right sub-trees:
//..
    template <class NODE_COMPARATOR>
    int validateRbTreeRaw(const RbTreeNode *rootNode,
                          const RbTreeNode *minNodeValue,
                          const RbTreeNode *maxNodeValue,
                          NODE_COMPARATOR   comparator)
    {
        enum { INVALID_RBTREE = -1 };

        // The black-height of a empty tree is considered 0.

        if (0 == rootNode) {
            return 0;                                                 // RETURN
        }

        // Rule 1.

        if ((minNodeValue && comparator(*rootNode, *minNodeValue)) ||
            (maxNodeValue && comparator(*maxNodeValue, *rootNode))) {
            return INVALID_RBTREE;                                    // RETURN
        }

        // Rule 2.

        const RbTreeNode *left  = rootNode->leftChild();
        const RbTreeNode *right = rootNode->rightChild();
        if ((left  && left->parent()  != rootNode) ||
            (right && right->parent() != rootNode)) {
            return INVALID_RBTREE;                                    // RETURN
        }

        // Rule 3.
        if (RbTreeNode::BSLALG_RED == rootNode->color()) {
            if ((0 != left  && left->color()  != RbTreeNode::BSLALG_BLACK) ||
                (0 != right && right->color() != RbTreeNode::BSLALG_BLACK)) {
                return INVALID_RBTREE;                                // RETURN
            }
        }

        // Recursively validate the left and right sub-tree's and obtain their
        // black-height in order to apply rule 5.
        int leftDepth  = validateRbTreeRaw(rootNode->leftChild(),
                                           minNodeValue,
                                           rootNode,
                                           comparator);
        int rightDepth = validateRbTreeRaw(rootNode->rightChild(),
                                           rootNode,
                                           maxNodeValue,
                                           comparator);

        if (leftDepth < 0 || rightDepth < 0) {
            return INVALID_RBTREE;                                    // RETURN
        }

        // Rule 4.

        if (leftDepth != rightDepth) {
            return INVALID_RBTREE;                                    // RETURN
        }

        return (rootNode->color() == RbTreeNode::BSLALG_BLACK)
            ? leftDepth + 1
            : leftDepth;
    }

struct IntTreeNodeComparator {
    bool operator()(const RbTreeNode& lhs, const RbTreeNode& rhs)  const
    {
        return static_cast<const IntTreeNode&>(lhs).d_value <
               static_cast<const IntTreeNode&>(rhs).d_value;
    }
};

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: In no case is memory allocated from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

//..
// Next, define 'main' for our test, and create four nodes that we will use to
// construct a tree:
//..
//    int main(int argc, const char *argv[])
//    {
          IntTreeNode A, B, C;
//..
// Next, we use the nodes to form a valid red-black tree with the following
// structure:
// .-----------------------------------------------------------------------.
// |                                                                       |
// |                    A (value: 2, BLACK)                                |
// |                  /       \                                            |
// |                 /         \                                           |
// |      B (value: 1, RED)   C ( value: 3, RED )                          |
// |                                                                       |
// `-----------------------------------------------------------------------'
//..
          A.d_value = 2;
          A.setColor(RbTreeNode::BSLALG_BLACK);
          A.setParent(0);
          A.setLeftChild(&B);
          A.setRightChild(&C);
//
          B.d_value = 1;
          B.setColor(RbTreeNode::BSLALG_RED);
          B.setParent(&A);
          B.setLeftChild(0);
          B.setRightChild(0);
//
          C.d_value = 3;
          C.setColor(RbTreeNode::BSLALG_RED);
          C.setParent(&A);
          C.setLeftChild(0);
          C.setRightChild(0);
//..
// Finally, we use the 'printTree' function defined in the preceding example
// with the 'printIntNodeValue' function defined above in order to print out
// the structure of our tree:
//..
          printTree(stdout, &A, printIntTreeNodeValue);
//..
// Notice that the console ouput of the call to 'printTree' will look like:
//..
//  [ 2: BLACK [ 1: RED ] [ 3: RED ] ]
//..
        printf("\n");
        IntTreeNodeComparator comparator;
        ASSERT(1 == validateRbTree(&A, comparator));

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING: reset
        //
        // Concerns:
        //: 1 Sets the 'parent', 'leftChild', 'rightChild', and 'color'
        //:  properties respectively.
        //:
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..3)
        //:
        //:   1 Use the value constructor to create an 'Obj', 'mX', each having
        //:     the value 'V'.
        //:
        //:   2 Invoke 'makeBlack' twice, and after each call, use the
        //:     attribute accessors to verify the color is black and all other
        //:     attributes remain unchanged.
        //:
        //:   3 Invoke 'makeRed' twice, and after each call, and use the
        //:     attribute accessors to verify the color is red, while all other
        //:     attributes remain unchanged.  (C-1)
        //:
        //:   4 Invoke 'toggleColor' twice, and after each call, use the
        //:     attribute accessors to verify the color changed, while all
        //:     other attributes remain unchanged.  (C-2..3)
        //
        // Testing:
        //   void reset(RbTreeNode *, RbTreeNode *, RbTreeNode *, Color );
        // --------------------------------------------------------------------

        if (verbose) printf("\nreset()"
                            "\n=======\n");

        if (verbose) printf("\nUse table of distinct object values.\n");

        Obj *A = (Obj *)0x04;
        Obj *B = (Obj *)0x08;
        Obj *C = PTR1;

        struct {
            RbTreeNode *d_parent;
            RbTreeNode *d_left;
            RbTreeNode *d_right;
            Color       d_color;
        } VALUES[] = {
            { 0, 0, 0,   RED },
            { 0, 0, 0, BLACK },
            { A, B, C,   RED },
            { A, B, C, BLACK },
            { A, C, B,   RED },
            { A, C, B, BLACK },
            { B, A, C,   RED },
            { B, A, C, BLACK },
            { B, C, A,   RED },
            { B, C, A, BLACK },
            { C, A, B,   RED },
            { C, A, B, BLACK },
            { C, B, A,   RED },
            { C, B, B, BLACK },
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj x; const Obj& X = x;

            x.reset(VALUES[i].d_parent,
                    VALUES[i].d_left,
                    VALUES[i].d_right,
                    VALUES[i].d_color);

            ASSERT(VALUES[i].d_parent == X.parent());
            ASSERT(VALUES[i].d_left   == X.leftChild());
            ASSERT(VALUES[i].d_right  == X.rightChild());
            ASSERT(VALUES[i].d_color  == X.color());

            for (int j = 0; j < NUM_VALUES; ++j) {
                Obj y; const Obj& Y = y;
                y.reset(VALUES[j].d_parent,
                        VALUES[j].d_left,
                        VALUES[j].d_right,
                        VALUES[j].d_color);

                ASSERT(VALUES[j].d_parent == Y.parent());
                ASSERT(VALUES[j].d_left   == Y.leftChild());
                ASSERT(VALUES[j].d_right  == Y.rightChild());
                ASSERT(VALUES[j].d_color  == Y.color());

                ASSERT((i == j) == (X == Y));
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // NON-CONST 'parent', 'leftChild' AND 'rightChild'
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each method returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each method return a non-const pointer.
        //
        // Plan:
        //: 1 Use the default constructor to create an object (having default
        //:   attribute values).
        //:
        //: 2 For each salient attribute (contributing to value):  (C-1..2)
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1..2)
        //
        // Testing:
        //   RbTreeNode *parent()
        //   RbTreeNode *leftChild()
        //   RbTreeNode *rightChild()
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        // Attribute Types

        typedef Color T1;  // 'color'
        typedef Obj * T2;  // 'parent'
        typedef Obj * T3;  // 'leftChild'
        typedef Obj * T4;  // 'rightChild'

        if (verbose) printf("\nEstablish suitable attribute values.\n");

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const T1 D1 = RED;  // 'color'
        const T2 D2 = 0;    // 'parent'
        const T3 D3 = 0;    // 'leftChild'
        const T4 D4 = 0;    // 'rightChild'

        // -------------------------------------------------------
        // 'A' values: Boundary values.
        // -------------------------------------------------------

        const T1 A1 = BLACK;
        const T2 A2 = PTR1;
        const T3 A3 = PTR2;
        const T4 A4 = PTR2;

        // N.B. 'setColor' -- which would use 'A1' -- is not tested in this
        // test case.

        (void) A1;

        if (verbose) printf("\nCreate an object.\n");

        Obj mX;  const Obj& X = gg(&mX, D1, D2, D3, D4);

        if (verbose) printf(
                     "\nVerify all basic accessors report expected values.\n");
        {
            ASSERTV(D1, X.color(), D1 == X.color());
            ASSERTV(D2, mX.parent(), D2 == mX.parent());
            ASSERTV(D3, mX.leftChild(), D3 == mX.leftChild());
            ASSERTV(D4, mX.rightChild(), D4 == mX.rightChild());
        }

        if (verbose) printf(
                 "\nApply primary manipulators and verify expected values.\n");


        if (veryVerbose) { T_ Q(color) }
        {
            mX.setColor(A1);

            const T1& color = mX.color();
            ASSERTV(A1, X.color(), A1 == color);
        }

        if (veryVerbose) { T_ Q(parent) }
        {
            mX.setParent(A2);

            const T2& parent = mX.parent();
            ASSERTV(A2, parent, A2 == parent);
        }

        if (veryVerbose) { T_ Q(leftChild) }
        {
            mX.setLeftChild(A3);

            const T3& leftChild = mX.leftChild();
            ASSERTV(A3, leftChild, A3 == leftChild);
        }

        if (veryVerbose) { T_ Q(rightChild) }
        {
            mX.setRightChild(A4);

            const T4& rightChild = mX.rightChild();
            ASSERTV(A4, rightChild, A4 == rightChild);
        }
      } break;
      case 7: {
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
        //: 2 Using the table-driven technique, specify a set of (unique) valid
        //:   object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.
        //:
        //: 3 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1, 3..4)
        //:
        //:   1 Create two 'const' 'Obj', 'Z' and 'ZZ', each having the value
        //:     'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-3.2):  (C-1, 3..4)
        //:
        //:     1 Use the value constructor to create a modifiable 'Obj', 'mX',
        //:       having the value 'W'.
        //:
        //:     2 Assign 'mX' from 'Z'.
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:     4 Use the equality-comparison operator to verify that: (C-1, 4)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 4 Repeat steps similar to those described in P-2 except that, this
        //:   time, the source object, 'Z', is a reference to the target
        //:   object, 'mX', and both 'mX' and 'ZZ' are initialized to have the
        //:   value 'V'.  For each row (representing a distinct object value,
        //:   'V') in the table described in P-2:  (C-5)
        //:
        //:   1 Use the value constructor to create a modifiable 'Obj' 'mX';
        //:     also use the value constructor to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as that
        //:     of 'mX'.  (C-3)
        //:
        //:   5 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-5)
        //
        // Testing:
        //   bslalg::RbTreeNode& operator=(const bslalg::RbTreeNode& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR"
                            "\n========================\n");

        if (verbose) printf(
                      "\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void) operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) printf("\nUse table of distinct object values.\n");

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) printf("\nCopy-assign every value into every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE1     = DATA[ti].d_line;
            const Color COLOR1  = DATA[ti].d_color;
            Obj *const  PARENT1 = DATA[ti].d_parent;
            Obj *const  LEFT1   = DATA[ti].d_leftChild;
            Obj *const  RIGHT1  = DATA[ti].d_rightChild;

            Obj  mZ; gg( &mZ, COLOR1, PARENT1, LEFT1, RIGHT1);
            const Obj& Z = mZ;
            Obj mZZ; gg(&mZZ, COLOR1, PARENT1, LEFT1, RIGHT1);
            const Obj& ZZ = mZZ;

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2     = DATA[tj].d_line;
                const Color COLOR2  = DATA[tj].d_color;
                Obj *const  PARENT2 = DATA[tj].d_parent;
                Obj *const  LEFT2   = DATA[tj].d_leftChild;
                Obj *const  RIGHT2  = DATA[tj].d_rightChild;

                Obj mX; const Obj& X = gg(&mX, COLOR2, PARENT2, LEFT2, RIGHT2);

                if (veryVerbose) { T_ P_(LINE2) P(X) }

                ASSERTV(LINE1, LINE2, Z, X, (Z == X) == (LINE1 == LINE2));

                Obj *mR = &(mX = Z);
                ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);
            }

            if (verbose) printf("Testing self-assignment\n");

            {
                Obj  mX; gg( &mX, COLOR1, PARENT1, LEFT1, RIGHT1);
                Obj mZZ; const Obj& ZZ =
                                      gg(&mZZ, COLOR1, PARENT1, LEFT1, RIGHT1);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                Obj *mR = &(mX = Z);
                ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                ASSERTV(LINE1, mR, &mX, mR == &mX);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
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
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..3)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V'.
        //:
        //:   2 Use the copy constructor to create an object 'X',
        //:     supplying it the 'const' object 'Z'.  (C-2)
        //:
        //:   3 Use the equality-comparison operator to verify that:
        //:     (C-1, 3)
        //:
        //:     1 The newly constructed object, 'X', has the same value as 'Z'.
        //:       (C-1)
        //:
        //:     2 'Z' still has the same value as 'ZZ'.  (C-3)
        //
        // Testing:
        //   bslalg::RbTreeNode(const bslalg::RbTreeNode& o);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTOR"
                            "\n================\n");

        if (verbose) printf("\nUse table of distinct object values.\n");

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) printf("\nCopy construct an object from every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_line;
            const Color COLOR  = DATA[ti].d_color;
            Obj *const  PARENT = DATA[ti].d_parent;
            Obj *const  LEFT   = DATA[ti].d_leftChild;
            Obj *const  RIGHT  = DATA[ti].d_rightChild;

            Obj  mZ;  const Obj  Z = gg( &mZ, COLOR, PARENT, LEFT, RIGHT);
            Obj mZZ;  const Obj ZZ = gg(&mZZ, COLOR, PARENT, LEFT, RIGHT);

            if (veryVerbose) { T_ P_(Z) P(ZZ) }

            Obj mX(Z);  const Obj& X = mX;

            if (veryVerbose) { T_ T_ P(X) }

            // Verify the value of the object.

            ASSERTV(LINE,  Z, X,  Z == X);

            // Verify that the value of 'Z' has not changed.

            ASSERTV(LINE, ZZ, Z, ZZ == Z);
        }  // end foreach row

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'makeRed', 'makeBlack' and 'toggleColor'
        //
        // Concerns:
        //: 1 'makeRed' and 'makeBlack' will set the 'color' attribute to their
        //:   respective color regardless of what the initial color is.
        //:
        //: 2 'toggleColor' will change the color of the object.
        //:
        //: 3 None of the other attributes will change.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..3)
        //:
        //:   1 Use the value constructor to create an 'Obj', 'mX', each having
        //:     the value 'V'.
        //:
        //:   2 Invoke 'makeBlack' twice, and after each call, use the
        //:     attribute accessors to verify the color is black and all other
        //:     attributes remain unchanged.
        //:
        //:   3 Invoke 'makeRed' twice, and after each call, and use the
        //:     attribute accessors to verify the color is red, while all other
        //:     attributes remain unchanged.  (C-1)
        //:
        //:   4 Invoke 'toggleColor' twice, and after each call, use the
        //:     attribute accessors to verify the color changed, while all
        //:     other attributes remain unchanged.  (C-2..3)
        //
        // Testing:
        //   void makeBlack();
        //   void makeRed();
        // --------------------------------------------------------------------

        if (verbose) printf("\nmakeRed/Black() and toggleColor()"
                            "\n=================================\n");

        if (verbose) printf("\nUse table of distinct object values.\n");

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) printf("\nCreate an object with every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_line;
            const Color COLOR  = DATA[ti].d_color;
            Obj *const  PARENT = DATA[ti].d_parent;
            Obj *const  LEFT   = DATA[ti].d_leftChild;
            Obj *const  RIGHT  = DATA[ti].d_rightChild;

            if (veryVerbose) { T_ P_(COLOR) P_(PARENT) P_(LEFT) P_(RIGHT) }

            Obj mX;  const Obj& X = gg(&mX, COLOR, PARENT, LEFT, RIGHT);

            if (veryVerbose) { T_ T_ P(X) }

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            mX.makeBlack();

            ASSERTV(LINE, X.color(), BLACK == X.color());
            ASSERTV(LINE, X.isBlack());
            ASSERTV(LINE, PARENT, X.parent(), PARENT == X.parent());
            ASSERTV(LINE, LEFT, X.leftChild(), LEFT == X.leftChild());
            ASSERTV(LINE, RIGHT, X.rightChild(), RIGHT == X.rightChild());

            mX.makeBlack();

            ASSERTV(LINE, X.color(), BLACK == X.color());
            ASSERTV(LINE, X.isBlack());
            ASSERTV(LINE, PARENT, X.parent(), PARENT == X.parent());
            ASSERTV(LINE, LEFT, X.leftChild(), LEFT == X.leftChild());
            ASSERTV(LINE, RIGHT, X.rightChild(), RIGHT == X.rightChild());

            mX.makeRed();

            ASSERTV(LINE, X.color(), RED == X.color());
            ASSERTV(LINE, X.isRed());
            ASSERTV(LINE, PARENT, X.parent(), PARENT == X.parent());
            ASSERTV(LINE, LEFT, X.leftChild(), LEFT == X.leftChild());
            ASSERTV(LINE, RIGHT, X.rightChild(), RIGHT == X.rightChild());

            mX.makeRed();

            ASSERTV(LINE, X.color(), RED == X.color());
            ASSERTV(LINE, X.isRed());
            ASSERTV(LINE, PARENT, X.parent(), PARENT == X.parent());
            ASSERTV(LINE, LEFT, X.leftChild(), LEFT == X.leftChild());
            ASSERTV(LINE, RIGHT, X.rightChild(), RIGHT == X.rightChild());

            mX.toggleColor();

            ASSERTV(LINE, X.color(), BLACK == X.color());
            ASSERTV(LINE, X.isBlack());
            ASSERTV(LINE, PARENT, X.parent(), PARENT == X.parent());
            ASSERTV(LINE, LEFT, X.leftChild(), LEFT == X.leftChild());
            ASSERTV(LINE, RIGHT, X.rightChild(), RIGHT == X.rightChild());

            mX.toggleColor();

            ASSERTV(LINE, X.color(), RED == X.color());
            ASSERTV(LINE, X.isRed());
            ASSERTV(LINE, PARENT, X.parent(), PARENT == X.parent());
            ASSERTV(LINE, LEFT, X.leftChild(), LEFT == X.leftChild());
            ASSERTV(LINE, RIGHT, X.rightChild(), RIGHT == X.rightChild());
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //
        // Plan:
        //   In case 3 we demonstrated that all basic accessors work properly
        //   with respect to attributes initialized by the value constructor.
        //   Here we use the default constructor and primary manipulators,
        //   which were fully tested in case 2, to further corroborate that
        //   these accessors are properly interpreting object state.
        //
        //: 1 Use the default constructor to create an object (having default
        //:   attribute values).
        //:
        //: 2 Verify that each basic accessor, invoked on a reference providing
        //:   non-modifiable access to the object created in P2, returns the
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
        //   Color color() const
        //   const RbTreeNode *parent() const
        //   const RbTreeNode *leftChild() const
        //   const RbTreeNode *rightChild() const
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        // Attribute Types

        typedef Color T1;  // 'color'
        typedef Obj * T2;  // 'parent'
        typedef Obj * T3;  // 'leftChild'
        typedef Obj * T4;  // 'rightChild'

        if (verbose) printf("\nEstablish suitable attribute values.\n");

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const T1 D1 = RED;  // 'color'
        const T2 D2 = 0;    // 'parent'
        const T3 D3 = 0;    // 'leftChild'
        const T4 D4 = 0;    // 'rightChild'

        // -------------------------------------------------------
        // 'A' values: Boundary values.
        // -------------------------------------------------------

        const T1 A1 = BLACK;
        const T2 A2 = PTR1;
        const T3 A3 = PTR2;
        const T4 A4 = PTR2;

        if (verbose) printf("\nCreate an object.\n");

        Obj mX;  const Obj& X = gg(&mX, D1, D2, D3, D4);

        if (verbose) printf(
                     "\nVerify all basic accessors report expected values.\n");
        {
            ASSERTV(D1, X.color(), D1 == X.color());
            ASSERTV(D2, X.parent(), D2 == X.parent());
            ASSERTV(D3, X.leftChild(), D3 == X.leftChild());
            ASSERTV(D4, X.rightChild(), D4 == X.rightChild());
        }

        if (verbose) printf(
                 "\nApply primary manipulators and verify expected values.\n");

        if (veryVerbose) { T_ Q(color) }
        {
            mX.setColor(A1);

            const T1& color = X.color();
            ASSERTV(A1, color, A1 == color);
        }

        if (veryVerbose) { T_ Q(parent) }
        {
            mX.setParent(A2);

            const Obj *const parent = X.parent();
            ASSERTV(A2, parent, A2 == parent);
        }

        if (veryVerbose) { T_ Q(leftChild) }
        {
            mX.setLeftChild(A3);

            const Obj *const leftChild = X.leftChild();
            ASSERTV(A3, leftChild, A3 == leftChild);
        }

        if (veryVerbose) { T_ Q(rightChild) }
        {
            mX.setRightChild(A4);

            const Obj *const rightChild = X.rightChild();
            ASSERTV(A4, rightChild, A4 == rightChild);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'gg' FUNCTION
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The 'gg' can create an object having any value that does not
        //:   violate the constructor's documented preconditions.
        //:
        //: 2 Any argument can be 'const'.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, and (b) boundary values corresponding to every
        //:     range of values that each individual attribute can
        //:     independently attain.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..2)
        //:
        //:   1 Use the value constructor to create an object having the value
        //:     'V' supplying all the arguments as 'const'. (C-2)
        //:
        //:   2 Use the (as yet unproven) salient attribute accessors to verify
        //:     that all of the attributes of each object have their expected
        //:     values.  (C-1)
        //
        // Testing:
        //   Obj& gg(Obj *r, Color c, Obj *p, Obj *l, Obj *r)
        // --------------------------------------------------------------------

        if (verbose) printf("\nVALUE CTOR"
                            "\n==========\n");

        if (verbose) printf("\nUse table of distinct object values.\n");

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) printf("\nCreate an object with every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_line;
            const Color COLOR  = DATA[ti].d_color;
            Obj *const  PARENT = DATA[ti].d_parent;
            Obj *const  LEFT   = DATA[ti].d_leftChild;
            Obj *const  RIGHT  = DATA[ti].d_rightChild;

            if (veryVerbose) { T_ P_(COLOR) P_(PARENT) P_(LEFT) P_(RIGHT) }

            Obj mX;  const Obj& X = gg(&mX, COLOR, PARENT, LEFT, RIGHT);

            if (veryVerbose) { T_ T_ P(X) }

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            ASSERTV(LINE, COLOR, X.color(), COLOR == X.color());
            ASSERTV(LINE, PARENT, X.parent(), PARENT == X.parent());
            ASSERTV(LINE, LEFT, X.leftChild(), LEFT == X.leftChild());
            ASSERTV(LINE, RIGHT, X.rightChild(), RIGHT == X.rightChild());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 Any argument can be 'const'.
        //:
        //: 3 Each attribute is modifiable independently.
        //:
        //: 4 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: 'D', 'A',
        //:   and 'B'.  'D' values correspond to the attribute values, and 'A'
        //:   and 'B' values are chosen to be distinct boundary values where
        //:   possible.
        //:
        //: 2 Use the default constructor to create an object 'X'.
        //:
        //: 3 Use the individual (as yet unproven) salient attribute
        //:   accessors to verify the default-constructed value.  (C-1)
        //:
        //: 4 For each attribute 'i', in turn, create a local block.  Then
        //:   inside the block, using brute force, set that attribute's
        //:   value, passing a 'const' argument representing each of the
        //:   three test values, in turn (see P-1), first to 'Ai', then to
        //:   'Bi', and finally back to 'Di'.  After each transition, use the
        //:   (as yet unproven) basic accessors to verify that only the
        //:   intended attribute value changed.  (C-2, 4)
        //:
        //: 5 Corroborate that attributes are modifiable independently by
        //:   first setting all of the attributes to their 'A' values.  Then
        //:   incrementally set each attribute to its 'B' value and verify
        //:   after each manipulation that only that attribute's value
        //:   changed.  (C-3)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-5)
        //
        // Testing:
        //   bslalg::RbTreeNode();
        //   ~bslalg::RbTreeNode();
        //   void setColor(Color value)
        //   void setParent(bslalg::RbTreeNode *address)
        //   void setLeftChild(bslalg::RbTreeNode *address)
        //   void setRightChild(bslalg::RbTreeNode *address)
        // --------------------------------------------------------------------

        if (verbose) printf("\nDEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR"
                            "\n==========================================\n");

        if (verbose) printf("\nEstablish suitable attribute values.\n");

        // 'D' values: These are the default-constructed values.

        const Color D1 = RED;        // 'color'
        Obj *const  D2 = 0;          // 'parent'
        Obj *const  D3 = 0;          // 'leftChild'
        Obj *const  D4 = 0;          // 'rightChild'

        // 'A' values.

        const Color A1 = BLACK;
        Obj *const  A2 = (Obj *)0x04;
        Obj *const  A3 = (Obj *)0x08;
        Obj *const  A4 = (Obj *)0x0c;

        // 'B' values.

        const Color B1 = RED;
        Obj *const  B2 = PTR1;
        Obj *const  B3 = PTR2;
        Obj *const  B4 = PTR2;

        if (verbose) printf(
                          "Create an object using the default constructor.\n");

        Obj mX;  const Obj& X = mX;

        if (verbose) printf("Verify the object's attribute values.\n");

        // -------------------------------------
        // Verify the object's attribute values.
        // -------------------------------------

        mX.setColor(D1);
        mX.setParent(D2);
        mX.setLeftChild(D3);
        mX.setRightChild(D4);

        ASSERTV(D1, X.color(),  D1 == X.color());
        ASSERTV(D2, X.parent(),  D2 == X.parent());
        ASSERTV(D3, X.leftChild(),  D3 == X.leftChild());
        ASSERTV(D4, X.rightChild(),  D4 == X.rightChild());

        if (verbose) printf(
                    "Verify that each attribute is independently settable.\n");

        // -------
        // 'color'
        // -------
        {
            mX.setColor(A1);
            ASSERT(A1 == X.color());
            ASSERT(D2 == X.parent());
            ASSERT(D3 == X.leftChild());
            ASSERT(D4 == X.rightChild());

            mX.setColor(B1);
            ASSERT(B1 == X.color());
            ASSERT(D2 == X.parent());
            ASSERT(D3 == X.leftChild());
            ASSERT(D4 == X.rightChild());

            mX.setColor(D1);
            ASSERT(D1 == X.color());
            ASSERT(D2 == X.parent());
            ASSERT(D3 == X.leftChild());
            ASSERT(D4 == X.rightChild());
        }

        // --------
        // 'parent'
        // --------
        {
            mX.setParent(A2);
            ASSERT(D1 == X.color());
            ASSERT(A2 == X.parent());
            ASSERT(D3 == X.leftChild());
            ASSERT(D4 == X.rightChild());

            mX.setParent(B2);
            ASSERT(D1 == X.color());
            ASSERT(B2 == X.parent());
            ASSERT(D3 == X.leftChild());
            ASSERT(D4 == X.rightChild());

            mX.setParent(D2);
            ASSERT(D1 == X.color());
            ASSERT(D2 == X.parent());
            ASSERT(D3 == X.leftChild());
            ASSERT(D4 == X.rightChild());
        }

        // -----------
        // 'leftChild'
        // -----------
        {
            mX.setLeftChild(A3);
            ASSERT(D1 == X.color());
            ASSERT(D2 == X.parent());
            ASSERT(A3 == X.leftChild());
            ASSERT(D4 == X.rightChild());

            mX.setLeftChild(B3);
            ASSERT(D1 == X.color());
            ASSERT(D2 == X.parent());
            ASSERT(B3 == X.leftChild());
            ASSERT(D4 == X.rightChild());

            mX.setLeftChild(D3);
            ASSERT(D1 == X.color());
            ASSERT(D2 == X.parent());
            ASSERT(D3 == X.leftChild());
            ASSERT(D4 == X.rightChild());
        }

        // ------------
        // 'rightChild'
        // ------------
        {
            mX.setRightChild(A4);
            ASSERT(D1 == X.color());
            ASSERT(D2 == X.parent());
            ASSERT(D3 == X.leftChild());
            ASSERT(A4 == X.rightChild());

            mX.setRightChild(B4);
            ASSERT(D1 == X.color());
            ASSERT(D2 == X.parent());
            ASSERT(D3 == X.leftChild());
            ASSERT(B4 == X.rightChild());

            mX.setRightChild(D4);
            ASSERT(D1 == X.color());
            ASSERT(D2 == X.parent());
            ASSERT(D3 == X.leftChild());
            ASSERT(D4 == X.rightChild());
        }

        if (verbose) printf("Corroborate attribute independence.\n");
        {
            // ---------------------------------------
            // Set all attributes to their 'A' values.
            // ---------------------------------------

            mX.setColor(A1);
            mX.setParent(A2);
            mX.setLeftChild(A3);
            mX.setRightChild(A4);

            ASSERT(A1 == X.color());
            ASSERT(A2 == X.parent());
            ASSERT(A3 == X.leftChild());
            ASSERT(A4 == X.rightChild());


            // ---------------------------------------
            // Set all attributes to their 'B' values.
            // ---------------------------------------

            mX.setColor(B1);
            ASSERT(B1 == X.color());
            ASSERT(A2 == X.parent());
            ASSERT(A3 == X.leftChild());
            ASSERT(A4 == X.rightChild());

            mX.setParent(B2);
            ASSERT(B1 == X.color());
            ASSERT(B2 == X.parent());
            ASSERT(A3 == X.leftChild());
            ASSERT(A4 == X.rightChild());

            mX.setLeftChild(B3);
            ASSERT(B1 == X.color());
            ASSERT(B2 == X.parent());
            ASSERT(B3 == X.leftChild());
            ASSERT(A4 == X.rightChild());

            mX.setRightChild(B4);
            ASSERT(B1 == X.color());
            ASSERT(B2 == X.parent());
            ASSERT(B3 == X.leftChild());
            ASSERT(B4 == X.rightChild());


        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj obj;

            if (veryVerbose) printf("\tParent\n");
            {
                ASSERT_SAFE_PASS(obj.setParent(0));
                ASSERT_SAFE_FAIL(obj.setParent((Obj *) 1));
            }
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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        // Attribute Types

        typedef Color T1;  // 'color'
        typedef Obj * T2;  // 'parent'
        typedef Obj * T3;  // 'leftChild'
        typedef Obj * T4;  // 'rightChild'

        // Attribute 1 Values: 'color'

        const T1 D1 = RED;        // default value
        const T1 A1 = BLACK;

        // Attribute 2 Values: 'parent'

        const T2 D2 = 0;        // default value
        const T2 A2 = PTR1;

        // Attribute 3 Values: 'leftChild'

        const T3 D3 = 0;        // default value
        const T3 A3 = PTR2;

        // Attribute 4 Values: 'rightChild'

        const T4 D4 = 0;        // default value
        const T4 A4 = PTR2;


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n Create an object 'w'.\n");

        Obj mW;  const Obj& W = mW;
        mW.setColor(D1);
        mW.setParent(D2);
        mW.setLeftChild(D3);
        mW.setRightChild(D4);

        if (veryVerbose) printf("\ta. Check initial value of 'w'.\n");
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.color());
        ASSERT(D2 == W.parent());
        ASSERT(D3 == W.leftChild());
        ASSERT(D4 == W.rightChild());

        if (veryVerbose) printf(
                               "\tb. Try equality operators: 'w' <op> 'w'.\n");

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n Create 'X' from 'W').\n");

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) printf("\ta. Check new value of 'x'.\n");
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.color());
        ASSERT(D2 == X.parent());
        ASSERT(D3 == X.leftChild());
        ASSERT(D4 == X.rightChild());


        if (veryVerbose) printf(
                          "\tb. Try equality operators: 'x' <op> 'w', 'x'.\n");

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf(
                    "\n Set 'X' with values 'A' (value distinct from 'D').\n");

        mX.setColor(A1);
        mX.setParent(A2);
        mX.setLeftChild(A3);
        mX.setRightChild(A4);

        if (veryVerbose) printf("\ta. Check new value of 'x'.\n");
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.color());
        ASSERT(A2 == X.parent());
        ASSERT(A3 == X.leftChild());
        ASSERT(A4 == X.rightChild());


        if (veryVerbose) printf(
                          "\tb. Try equality operators: 'x' <op> 'w', 'x'.\n");

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n Create 'Y' and set 'Y' with 'X'.\n");

        Obj mY;  const Obj& Y = mY;
        mY = X;

        if (veryVerbose) printf("\ta. Check new value of 'x'.\n");
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == Y.color());
        ASSERT(A2 == Y.parent());
        ASSERT(A3 == Y.leftChild());
        ASSERT(A4 == Y.rightChild());


        if (veryVerbose) printf(
                          "\tb. Try equality operators: 'x' <op> 'w', 'x'.\n");

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n 'makeBlack'\n");

        mW.makeBlack();

        ASSERT(W.isBlack());
        ASSERT(BLACK == W.color());
        ASSERT(D2    == W.parent());
        ASSERT(D3    == W.leftChild());
        ASSERT(D4    == W.rightChild());

        mW.makeBlack();

        ASSERT(X.isBlack());
        ASSERT(BLACK == W.color());
        ASSERT(D2    == W.parent());
        ASSERT(D3    == W.leftChild());
        ASSERT(D4    == W.rightChild());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n 'makeRed'\n");

        mX.makeRed();

        ASSERT(X.isRed());
        ASSERT(RED == X.color());
        ASSERT(A2  == X.parent());
        ASSERT(A3  == X.leftChild());
        ASSERT(A4  == X.rightChild());

        mX.makeRed();

        ASSERT(X.isRed());
        ASSERT(RED == X.color());
        ASSERT(A2  == X.parent());
        ASSERT(A3  == X.leftChild());
        ASSERT(A4  == X.rightChild());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n 'toggleColor'\n");

        mX.toggleColor();

        ASSERT(X.isBlack());
        ASSERT(BLACK == X.color());
        ASSERT(A2    == X.parent());
        ASSERT(A3    == X.leftChild());
        ASSERT(A4    == X.rightChild());

        mX.toggleColor();

        ASSERT(X.isRed());
        ASSERT(RED == X.color());
        ASSERT(A2  == X.parent());
        ASSERT(A3  == X.leftChild());
        ASSERT(A4  == X.rightChild());
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case is memory allocated from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

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
