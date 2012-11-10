// bslstl_hashtablebucketiterator.t.cpp                               -*-C++-*-
#include <bslstl_hashtablebucketiterator.h>

#include <bslstl_allocator.h>
#include <bslstl_bidirectionalnodepool.h>

#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stddef.h>
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
typedef bslstl::HashTableBucketIterator<int, ptrdiff_t > Obj;
typedef bslalg::BidirectionalLink                        Link;

struct NodePoolInt {
  private:
    // DATA
    bslstl::BidirectionalNodePool<int, bsl::allocator<int> > d_subPool;

  public:
    // CREATORS
    explicit
    NodePoolInt(bslma::Allocator *allocator)
    : d_subPool(allocator)
    {}

    // MANIPULATORS
    Link *createNode(int value, Link *prev, Link *next)
    {
        Link *result = d_subPool.createNode(value);
        result->setPreviousLink(prev);
        result->setNextLink(    next);

        return result;
    }
};

Link *makeSequence(NodePoolInt *pool, int from, const int to, int by)
{
    Link *result = 0, *prev = 0;
    for (int i = from; to != i; i += by) {
        Link *current = pool->createNode(i, prev, 0);
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


struct MyStruct {
    int d_data;

    // CREATORS
    MyStruct() {}
    explicit MyStruct(int data) : d_data(data) {}
};

typedef bslstl::HashTableBucketIterator<MyStruct, ptrdiff_t > PairObj;

struct NodePoolMyStruct {
  private:
    // DATA
    bslstl::BidirectionalNodePool<MyStruct, bsl::allocator<MyStruct> >
                                                                     d_subPool;

  public:
    // CREATORS
    explicit
    NodePoolMyStruct(bslma::Allocator *allocator)
    : d_subPool(allocator)
    {}

    // MANIPULATORS
    Link *createNode(int value, Link *prev, Link *next)
    {
        Link *result = d_subPool.createNode(value);
        result->setPreviousLink(prev);
        result->setNextLink(    next);

        return result;
    }
};

Link *makeSequence(NodePoolMyStruct *pool, int from, const int to, int by)
{
    Link *result = 0, *prev = 0;
    for (int i = from; to != i; i += by) {
        Link *current = pool->createNode(i, prev, 0);
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

        bslma::TestAllocator ta;

        const Obj X;
        ASSERT(  X == X );
        ASSERT(!(X != X));

        {
            NodePoolInt pool(&ta);

            Link *list = makeSequence(&pool, 0, 10, 1);

            // Create a bucket of 3 elements at the start of the list
            bslalg::HashTableBucket bucket = { list,
                                               list->nextLink()->nextLink() };

            const Obj bucketBegin(&bucket);
            ASSERT(bucketBegin == bucketBegin);
            // ASSERT(bucketBegin != X);  // This should be a test for an
                                          // assertion, comparing iterators to
                                          // different buckets.

            Obj cursor = bucketBegin;
            ASSERT(bucketBegin == cursor);

            *cursor = 1;

            const Obj bucketSecond(list->nextLink(), &bucket);
            ASSERT(bucketSecond != cursor);
            *bucketSecond = 2;

            // Check post-increment
            const Obj original = cursor++;
            ASSERT(bucketSecond == cursor);
            ASSERT(original == bucketBegin);
            ASSERT(2 == *cursor);

            // Check pre-increment
            *++cursor = 3;
            ASSERT(3 == *cursor);
        }

        // need to test a struct to validate operator->
        {
            NodePoolMyStruct pool(&ta);

            Link *list = makeSequence(&pool, 0, 10, 1);

            // Create a bucket of 3 elements at the start of the list
            bslalg::HashTableBucket bucket = { list,
                                               list->nextLink()->nextLink() };

            const PairObj bucketBegin(&bucket);
            PairObj cursor = bucketBegin;
            ASSERT(bucketBegin == cursor);

            cursor->d_data = 1;

            const PairObj bucketSecond(list->nextLink(), &bucket);
            ASSERT(bucketSecond != cursor);
            bucketSecond->d_data = 2;

            // Check post-increment
            const PairObj original = cursor++;
            ASSERT(bucketSecond == cursor);
            ASSERT(original == bucketBegin);
            ASSERT(2 == cursor->d_data);

            // Check pre-increment
            (++cursor)->d_data = 3;
            ASSERT(3 == cursor->d_data);
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

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
