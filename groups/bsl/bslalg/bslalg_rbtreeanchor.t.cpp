//bslag_rbtreeanchor.t.cpp                                      -*-C++-*-
#include <bslalg_rbtreeanchor.h>

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
//
        int d_value;  // "payload" value represented by the node
    };

//
///Example 2: Creating an Insert Function For A Binary Tree
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
// Now, we implement the search node function, which finds a location where
// 'newNode' can be inserted into 'searchTree' without violating the ordering
// imposed by 'comparator', and then updates 'searchTree' with a potentially
// updated root, first, and last node.
//..
        RbTreeNode *parent = 0;
        RbTreeNode *node   = searchTree->rootNode();
        bool        isLeftChild;
//
        newNode->setLeftChild(0);
        newNode->setRightChild(0);
//
        if (!node) {
            // 'searchTree' is empty, so 'newNode' will be the root, first,
            // and last most nodes in the tree.
//
            newNode->setParent(0);
            searchTree->reset(newNode, newNode, newNode);
            return;                                                   // RETURN
        }
//
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
//
        // Insert 'newNode' into 'searchTree' and the location that's been
        // found. 
//
        if (isLeftChild) {
            parent->setLeftChild(newNode);
            newNode->setParent(parent);
            if (parent == searchTree->firstNode()) {
                searchTree->setFirstNode(newNode);
            }
        }
        else {
            parent->setRightChild(newNode);
            newNode->setParent(parent);
            if (parent == searchTree->lastNode()) {
                searchTree->setLastNode(newNode);
            }
        }
    }
//..


struct IntTreeNodeComparator {
    bool operator()(const RbTreeNode& lhs, const RbTreeNode& rhs)  const
    {
        return static_cast<const IntTreeNode&>(lhs).d_value <
               static_cast<const IntTreeNode&>(rhs).d_value;
    }
};

void printTree(FILE             *output,
               const RbTreeNode *rootNode,
               void (*printNodeValueCallback)(FILE *, const RbTreeNode *))
{
    if (0 == rootNode) {
        return;                                                   // RETURN
    }
    fprintf(output, " [ ");
    // Print the value and color of 'rootNode'.
    printNodeValueCallback(output, rootNode);
    fprintf(output,
            ": %s",
            rootNode->color() == RbTreeNode::BSLALG_RED ? "RED" : "BLACK");
    printTree(output, rootNode->leftChild(), printNodeValueCallback);
    printTree(output, rootNode->rightChild(), printNodeValueCallback);
    fprintf(output, " ]");
}

void printIntTreeNodeValue(FILE *output, const RbTreeNode *node)
{
    BSLS_ASSERT(0 != node);
    fprintf(output, "%d", static_cast<const IntTreeNode*>(node)->d_value);
}

 
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
// Next, we define 'main' for our test, and create three nodes that we'll use
// to construct a tree:
//..
//    int main(int argc, const char *argv[])
      {
        IntTreeNode A, B, C;
//..
// Then we create an 'RbTreeAnchor', 'myTree', while will hold the addresses
// for the root, first, and last nodes of our tree, and verify its default
// attribute values:
//..
        RbTreeAnchor myTree;
        ASSERT(0 == myTree.rootNode());
        ASSERT(0 == myTree.firstNode());
        ASSERT(0 == myTree.lastNode());
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
// Now, we assign the address of 'A', 'B', and 'C' as the root, first, and
// last nodes of 'myTree', respectively:
//..
        myTree.reset(&A, &B, &C);
//..
// Finally, we verify the attributes of 'myTree':
//..
        ASSERT(&A == myTree.rootNode());
        ASSERT(&B == myTree.firstNode());
        ASSERT(&C == myTree.lastNode());
//..


      }
      {
          
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

          ASSERT(0 == static_cast<IntTreeNode *>(anchor.firstNode())->d_value);
          ASSERT(5 == static_cast<IntTreeNode *>(anchor.lastNode())->d_value);
          ASSERT(3 == static_cast<IntTreeNode *>(anchor.rootNode())->d_value);

          if (veryVerbose) {
              printTree(stdout, anchor.rootNode(), printIntTreeNodeValue);
              printf("\n");
          }

      }

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
