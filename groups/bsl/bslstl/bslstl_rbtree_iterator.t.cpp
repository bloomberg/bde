// bslstl_rbtree_iterator.t.cpp                                       -*-C++-*-
#include <bslstl_rbtree_iterator.h>

#include <bslalg_rbtreeprimitives.h>
#include <bslalg_rbtreenode.h>

#include <bslma_allocator.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>

#include <algorithm>
#include <cstddef>
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

typedef bslalg::RbTreePrimitives Op;
typedef bslalg::RbTreeNode   Node;

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
struct IntNode : bslalg::RbTreeNode
{
    int d_value;
};

void printIntNodeValue(FILE *file, const bslalg::RbTreeNode *node) 
{
    fprintf(file, "%d", static_cast<const IntNode *>(node)->d_value);
}

struct IntNodeComparator {
    typedef IntNode NodeType;
    bool operator()(const IntNode& lhs, const IntNode& rhs) const {
        return lhs.d_value < rhs.d_value;
    }
};

struct IntNodeValueComparator {
    typedef IntNode NodeType;

    bool operator()(const IntNode& node, int value) const {
        return node.d_value < value;
    }

    bool operator()(int value, const IntNode& node) const {
       return value < node.d_value;
    }
};

void assignIntNodeValue(IntNode *node, int value)
{
    node->d_value = value;
}

bool intLess(int lhs, int rhs)
{
    return lhs < rhs;
}

inline
IntNode *testNodeAtIndex(IntNode *nodes, int nodeIndex)
   // Return the node at the specified 'index' in the specified array of
   // 'nodes' in 'nodeIndex >= 0', and 0 otherwise.
{
    return nodeIndex >= 0 ? &nodes[nodeIndex] : 0;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
 
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

        typedef RbTree_Iterator<int, IntNode, std::ptrdiff_t> iterator;
        typedef RbTree_Iterator<const int, IntNode, std::ptrdiff_t> 
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

            ASSERT(  itA == itC);
            ASSERT(!(itA != itC));

        }
        {
            IntNodeComparator nodeComparator;
            enum {
                NUM_VALUES = 10
            };
            int  VALUES[NUM_VALUES];
            for (int i = 0; i < NUM_VALUES; ++i) {
                VALUES[i] = i;
            }            

            std::random_shuffle(VALUES, VALUES + NUM_VALUES);    
            Node *root = 0;

            IntNode nodes[NUM_VALUES];
            for (int i = 0; i < NUM_VALUES; ++i) {
                nodes[i].setLeftChild(0);
                nodes[i].setRightChild(0);
                nodes[i].setParent(0);
                nodes[i].d_value  = VALUES[i];

                root = Op::insert(root, nodeComparator, &nodes[i]);
            }
            ASSERT(Op::validateRbTree(root, nodeComparator));
                        
            IntNode *node = static_cast<IntNode *>(Op::minimum(root));
            do {
                IntNode *nextNode = static_cast<IntNode *>(Op::next(node));
                IntNode *prevNode = static_cast<IntNode *>(Op::previous(node));

                iterator it(node); const iterator& IT = it;
                const_iterator cit(node); const const_iterator& CIT = cit;

                ASSERT( node->d_value == *IT);
                ASSERT( node->d_value == *CIT);
                ASSERT(&node->d_value == IT.operator->());
                ASSERT(&node->d_value == CIT.operator->());
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
                        ASSERT( nextNode->d_value == *X);
                        ASSERT( nextNode->d_value == *CX);
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
                        ASSERT( nextNode->d_value == *X);
                        ASSERT( nextNode->d_value == *CX);
                    }
                }

                // prefix decrement
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
                        ASSERT( prevNode->d_value == *X);
                        ASSERT( prevNode->d_value == *CX);
                    }
                }
                // postfix decrement
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
                        ASSERT( prevNode->d_value == *X);
                        ASSERT( prevNode->d_value == *CX);
                    }
                }

                node = nextNode;
            } while (node);
            
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
