// bslstl_hashtableiterator.t.cpp                                     -*-C++-*-
#include <bslstl_hashtableiterator.h>

#include <bslstl_allocator.h>
#include <bslstl_bidirectionalnodepool.h>

#include <bslma_testallocator.h>
#include <bslma_sequentialallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//
// ----------------------------------------------------------------------------
// [  ] ...
// ----------------------------------------------------------------------------
// [  ] BREATHING TEST
// [  ] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

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

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

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
//             GLOBAL TYPEDEFS, FUNCTIONS AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

typedef bslstl::HashTableIterator<int, bsl::allocator<int> > Obj;
typedef bslalg::BidirectionalNode<int>                       Node;

struct NodePool {
  private:
    // DATA
    bslma::SequentialAllocator                               d_seqAlloc;
    bslstl::BidirectionalNodePool<int, bsl::allocator<int> > d_subPool;

  public:
    // CREATORS
    explicit
    NodePool(bslma::Allocator *alloc)
    : d_seqAlloc(alloc)
    , d_subPool(&d_seqAlloc)
    {}

    // MANIPULATORS
    Node *createNode(int value, Node *prev, Node *next)
    {
        Node *result = (Node *) d_subPool.createNode(value);
        result->setPreviousLink(prev);
        result->setNextLink(    next);

        return result;
    }
};

Node *makeSequence(NodePool *pool, int from, const int to, int by)
{
    Node *result = 0, *prev = 0;
    for (int i = from; to != i; i += by) {
        Node *current = pool->createNode(i, prev, 0);
        if (0 == result) {
            result = current;
        }
        else {
            prev->setNextLink(current);
        }
        prev = current;
    }

    return result;
}

struct MyPair {
    int d_first;
    int d_second;

    // CREATORS
    MyPair() {}
    MyPair(int first, int second) : d_first(first), d_second(second) {}
};

typedef bslstl::HashTableIterator<MyPair, bsl::allocator<int> > PairObj;
typedef bslalg::BidirectionalNode<MyPair>                       PairNode;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose             = argc > 2;
    bool veryVerbose         = argc > 3;
    bool veryVeryVerbose     = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator ta;

    switch (test) { case 0:
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

        {
            NodePool pool(&ta);

            Node *list = makeSequence(&pool, 0, 10, 1);

            int i = 0;
            for (Obj it(list); it.node(); ++it, ++i) {
                ASSERT(i == *it);
            }
            ASSERT(10 == i);

            i = 0;
            for (Obj it(list); it.node(); ++i) {
                ASSERT(i == *it++);
            }
            ASSERT(10 == i);
        }

        {
            PairNode *pn = (PairNode *) ta.allocate(sizeof(PairNode));
            pn->value() = MyPair(3, 5);

            PairObj it(pn);

            ASSERT(3 == it->d_first);
            ASSERT(5 == it->d_second);

            ta.deallocate(pn);
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
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
