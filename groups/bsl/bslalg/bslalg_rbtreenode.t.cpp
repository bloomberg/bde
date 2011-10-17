// bslag_rbtreenode.t.cpp                                             -*-C++-*-
#include <bslalg_rbtreenode.h>

#include <bslalg_arrayprimitives.h>
#include <bslalg_arraydestructionprimitives.h>
#include <bslalg_swaputil.h>
#include <bslma_allocator.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>

#include <algorithm>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace BloombergLP;
using namespace std;
using namespace bslalg;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// Global Concerns:
//: o Pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated.
//: o Precondition violations are detected in appropriate build modes.
//-----------------------------------------------------------------------------
// CLASS METHODS
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//-----------------------------------------------------------------------------
//=============================================================================

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

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
# define LOOP_ASSERT(I,X) { \
    if (!(X)) { P_(I); aSsErT(!(X), #X, __LINE__); } }

# define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { P(I) P_(J);   \
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { P(I) P(J) P_(K) \
                aSsErT(!(X), #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");      // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n");  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", "); // P(X) without '\n'
#define L_ __LINE__                         // current Line number
#define T_ putchar('\t');                   // Print a tab (w/o newline)

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
inline void dbg_print(bool b) { printf(b ? "true" : "false"); fflush(stdout); }
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%d", (int)val); fflush(stdout); }
inline void dbg_print(unsigned short val) {
    printf("%d", (int)val); fflush(stdout);
}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%ld", val); fflush(stdout); }
inline void dbg_print(unsigned long val) {
    printf("%lu", val); fflush(stdout);
}
inline void dbg_print(long long val) { printf("%lld", val); fflush(stdout); }
inline void dbg_print(unsigned long long val) {
    printf("%llu", val); fflush(stdout);
}
inline void dbg_print(float val) {
    printf("'%f'", (double)val); fflush(stdout);
}
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(long double val) {
    printf("'%Lf'", val); fflush(stdout);
}
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(void* p) { printf("%p", p); fflush(stdout); }

// Generic debug print function (3-arguments).
template <typename T>
void dbg_print(const char* s, const T& val, const char* nl) {
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef RbTreeNode Obj;

const RbTreeNode::Color RED   = RbTreeNode::BSLALG_RED;
const RbTreeNode::Color BLACK = RbTreeNode::BSLALG_BLACK;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
       BSLS_ASSERT(0 != node);
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
 
//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {

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
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");


       } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
