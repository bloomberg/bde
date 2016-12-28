// bslstl_iteratorutil.t.cpp                                          -*-C++-*-

#include <bslstl_iteratorutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslstl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// 'bslstl::IteratorUtil' provides a namespace for a suite of utility functions
// for iterator types.  This test driver verify that each of the functions
// behaves as documented.  Note that only one function, 'insertDistance', is
// currently defined.
//-----------------------------------------------------------------------------
// [ 2] size_t insertDistance(InputIterator, InputIterator)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

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

typedef IteratorUtil Obj;

namespace {

template<class CATEGORY,
         class VALUE,
         class DISTANCE = ptrdiff_t,
         class POINTER = VALUE*,
         class REFERENCE = VALUE&>
struct TestIterator {
    // PUBLIC TYPES
    typedef VALUE     value_type;
    typedef DISTANCE  difference_type;
    typedef POINTER   pointer;
    typedef REFERENCE reference;
    typedef CATEGORY  iterator_category;

    // DATA
    pointer d_ptr;  // address of the element referred to by 'this'

    // MANIPULATORS
    TestIterator& operator++ ()
    {
        ++d_ptr;
        return *this;
    }

    TestIterator operator++ (int)
    {
        TestIterator tmp(*this);
        ++d_ptr;
        return tmp;
    }
};

template<class CATEGORY,
         class VALUE,
         class DISTANCE,
         class POINTER,
         class REFERENCE>
bool operator==(
        const TestIterator<CATEGORY, VALUE, DISTANCE, POINTER, REFERENCE>& lhs,
        const TestIterator<CATEGORY, VALUE, DISTANCE, POINTER, REFERENCE>& rhs)
{
    return lhs.d_ptr == rhs.d_ptr;
}

template<class CATEGORY,
         class VALUE,
         class DISTANCE,
         class POINTER,
         class REFERENCE>
bool operator!=(
        const TestIterator<CATEGORY, VALUE, DISTANCE, POINTER, REFERENCE>& lhs,
        const TestIterator<CATEGORY, VALUE, DISTANCE, POINTER, REFERENCE>& rhs)
{
    return !(lhs == rhs);
}


template<class CATEGORY,
         class VALUE,
         class DISTANCE,
         class POINTER,
         class REFERENCE>
DISTANCE operator-(
        const TestIterator<CATEGORY, VALUE, DISTANCE, POINTER, REFERENCE>& lhs,
        const TestIterator<CATEGORY, VALUE, DISTANCE, POINTER, REFERENCE>& rhs)
{
    return lhs.d_ptr - rhs.d_ptr;
}

}  // close unnamed namespace

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

    (void) veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 3: {
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

        if (verbose) printf("\nUsage Example"
                            "\n=============\n");

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Finding the Distance Between Two Random Access Iterators
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to find the number of elements between two random access
// iterators.
//
// First, we create an array of integer values and two pointers (which are
// considered random access iterators) referring to the beginning and end of a
// range within that array:
//..
        int values[] = { 1, 2, 3, 4, 5 };
        int *begin = &values[0];
        int *end   = &values[3];
//..
// Now, we use the 'IteratorUtil::insertDistance' class method to calculate the
// distance of the open range ['begin', 'end'):
//..
        std::size_t distance = IteratorUtil::insertDistance(begin, end);
        ASSERT(3 == distance);

//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'insertDistance'
        //
        // Concerns:
        //: 1 The function returns 0 for input iterators.
        //:
        //: 2 The function returns the distance between two iterators if the
        //:   iterators are either forward, bidirectional, or random-access
        //:   iterators.
        //
        // Plan:
        //: 1 Create a few pairs of input iterators.  Verify that the function
        //:   returns 0.  (C-1)
        //:
        //: 2 For each of the iterator types -- forward, bidirectional, and
        //:   random-access:  (C-2)
        //:
        //:   1 Verify the function returns 0 for a pair of iterators having an
        //:     empty range.  (C-2)
        //:
        //:   2 Create pairs of iterators having non empty ranges.
        //:
        //:   3 Verify that the function returns the correct distances.  (C-2)
        //
        // Testing:
        //   size_t insertDistance(InputIterator, InputIterator)
        // --------------------------------------------------------------------

        if (verbose) printf("\n'insertDistance'"
                            "\n================\n");

        int testData[] = { 42, 13, 56, 72, 39 };
        int numElements = sizeof(testData) / sizeof(int);

        // Test input iterators.
        {
            typedef TestIterator<bsl::input_iterator_tag, int>
                                                          IntTestInputIterator;

            IntTestInputIterator a; const IntTestInputIterator& A = a;
            IntTestInputIterator b; const IntTestInputIterator& B = b;

            a.d_ptr = testData;

            for (int ti = 0; ti < numElements; ++ti) {
                b.d_ptr = testData + ti;
                size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, 0 == DIST);
            }
        }

        // Test forward iterators.
        {
            typedef TestIterator<bsl::forward_iterator_tag, int>
                                                        IntTestForwardIterator;

            IntTestForwardIterator a; const IntTestForwardIterator& A = a;
            IntTestForwardIterator b; const IntTestForwardIterator& B = b;

            a.d_ptr = testData;
            b.d_ptr = testData;

            size_t DIST = Obj::insertDistance(A, B);
            ASSERTV(DIST, 0 == DIST);

            for (int ti = 1; ti < numElements; ++ti) {
                b.d_ptr = testData + ti;
                size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);
            }
        }

        // Test bidirectional iterators.
        {
            typedef TestIterator<bsl::bidirectional_iterator_tag, int>
                                                   IntTestBidrectionalIterator;

            IntTestBidrectionalIterator a;
            const IntTestBidrectionalIterator& A = a;
            IntTestBidrectionalIterator b;
            const IntTestBidrectionalIterator& B = b;

            a.d_ptr = testData;
            b.d_ptr = testData;

            size_t DIST = Obj::insertDistance(A, B);
            ASSERTV(DIST, 0 == DIST);

            for (int ti = 1; ti < numElements; ++ti) {
                b.d_ptr = testData + ti;
                size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);
            }
        }

        // Test bidirectional iterators.
        {
            typedef TestIterator<bsl::random_access_iterator_tag, int>
                                                   IntTestRandomAccessIterator;

            IntTestRandomAccessIterator a;
            const IntTestRandomAccessIterator& A = a;
            IntTestRandomAccessIterator b;
            const IntTestRandomAccessIterator& B = b;

            a.d_ptr = testData;
            b.d_ptr = testData;

            size_t DIST = Obj::insertDistance(A, B);
            ASSERTV(DIST, 0 == DIST);

            for (int ti = 1; ti < numElements; ++ti) {
                b.d_ptr = testData + ti;
                size_t DIST = Obj::insertDistance(A, B);
                ASSERTV(DIST, static_cast<size_t>(ti) == DIST);
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
        //: 1 Perform some ad-hoc tests.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");


        typedef TestIterator<bsl::input_iterator_tag,   int>
                                                          IntTestInputIterator;
        typedef TestIterator<bsl::forward_iterator_tag, int>
                                                        IntTestForwardIterator;

        if (veryVerbose) printf("\n\t\t Test uninitialized input iterators\n");
        {
            IntTestInputIterator a; const IntTestInputIterator& A = a;
            IntTestInputIterator b; const IntTestInputIterator& B = b;

            const size_t DIST = Obj::insertDistance(A, B);
            ASSERTV(DIST, 0 == DIST);
        }

        if (veryVerbose) printf("\n\t\t Test initialized input iterators\n");
        {
            IntTestInputIterator a; const IntTestInputIterator& A = a;
            IntTestInputIterator b; const IntTestInputIterator& B = b;

            a.d_ptr = 0;
            b.d_ptr = 0;

            const size_t DIST = Obj::insertDistance(A, B);
            ASSERTV(DIST, 0 == DIST);
            ASSERTV(A.d_ptr, 0 == A.d_ptr);
            ASSERTV(B.d_ptr, 0 == B.d_ptr);

            IntTestInputIterator::pointer DEADBEEF =
                                    (IntTestInputIterator::pointer) 0xdeadbeef;
            b.d_ptr = DEADBEEF;

            const size_t DIST2 = Obj::insertDistance(A, B);
            ASSERTV(DIST2, 0 == DIST2);
            ASSERTV(A.d_ptr,     0 == A.d_ptr);
            ASSERTV(B.d_ptr, DEADBEEF == B.d_ptr);
        }

        if (veryVerbose) printf("\n\t\t Test initialized forward iterators\n");
        {
            IntTestForwardIterator a; const IntTestForwardIterator& A = a;
            IntTestForwardIterator b; const IntTestForwardIterator& B = b;

            a.d_ptr = 0;
            b.d_ptr = 0;

            size_t DIST = Obj::insertDistance(A, B);
            ASSERTV(DIST, 0 == DIST);
            ASSERTV(A.d_ptr, 0 == A.d_ptr);
            ASSERTV(B.d_ptr, 0 == B.d_ptr);

            b++;

            DIST = Obj::insertDistance(A, B);
            ASSERTV(DIST, 1 == DIST);
            ASSERTV(A.d_ptr, 0 == A.d_ptr);
            ASSERTV(B.d_ptr, A.d_ptr + 1 == B.d_ptr);

            a++;

            DIST = Obj::insertDistance(A, B);
            ASSERTV(DIST, 0 == DIST);
            ASSERTV(A.d_ptr, B.d_ptr, B.d_ptr == A.d_ptr);

            // Choose 0xdeadbeec instead of 0xdeadbeef because of alignment.

            IntTestInputIterator::pointer PTR =
                                    (IntTestInputIterator::pointer) 0x10;
            a.d_ptr = 0;
            b.d_ptr = PTR;

            const size_t EXP_DIST2 = 4;

            const size_t DIST2 = Obj::insertDistance(A, B);
            ASSERTV(DIST2,   EXP_DIST2 == DIST2);
            ASSERTV(A.d_ptr,         0 == A.d_ptr);
            ASSERTV(B.d_ptr,       PTR == B.d_ptr);
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
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
