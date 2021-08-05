// bdlb_testinputiterator.t.cpp                                       -*-C++-*-
#include <bdlb_testinputiterator.h>

#include <bslmf_assert.h>      // for testing only
#include <bslmf_issame.h>      // for testing only
#include <bslmf_removecv.h>    // for testing only

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <vector>

#include <cstddef>

#include <stdio.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// bdlb::TestInputIterator has no state and thus very little to test.  We need
// to verify that objects can be created and destroyed, that all forbidden
// operations calls lead to assert, and that that comparison operators return
// valid results.  The usage example completes the test by proving that it
// works in idiomatic use.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] TestInputIterator();
// [ 2] TestInputIterator(const TestInputIterator&);
// [ 3] TestInputIterator(TYPE *);
// [ 3] TestInputIterator(CONTIGUOUS_ITERATOR);
// [ 2] ~TestInputIterator();
//
// MANIPULATORS
// [ 2] TestInputIterator& operator=(const TestInputIterator&);
// [ 3] TestInputIterator& operator++();
// [ 3] TestInputIterator operator++(int);
//
// ACCESSORS
// [ 3] TYPE *operator->() const;
// [ 3] TYPE operator*() const;
//
// FREE OPERATORS
// [ 4] bool operator==(TestInputIterator&, TestInputIterator&);
// [ 4] bool operator!=(TestInputIterator&, TestInputIterator&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
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

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {
namespace u {

class MyClass {
    // A simple test class that can instantiated and copied to help test the
    // instantiation of 'bdlb::TestInputIterator' on a user-defined type.

    int d_value;

  public:
    // CREATORS
    explicit
    MyClass(int value) : d_value(value) {}
        // Construct a MyClass object.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    MyClass(const MyClass& original) = default;
        // Construct a copy of an object.

    ~MyClass() = default;
        // Destroy an object.

    // MANIPULATORS
    MyClass& operator=(const MyClass&) = default;
        // The type used to instantiate 'bdlb::TestInputIterator' does not
        // need to be assignable, although it must be copy-constructible.  We
        // disable the assignment operator to ensure that we don't depend on
        // it.
#endif

    // ACCESSORS
    int value() const { return d_value; }
        // Member function, required for dereference operator test.
};

template <class TYPE>
bool isConst(TYPE&)
    // Return 'true' if passed a 'const' object and 'false' otherwise.
{
    return false;
}

template <class TYPE>
bool isConst(const TYPE&)
    // Return 'true' if passed a 'const' object and 'false' otherwise.
{
    return true;
}

}  // close namespace u
}  // close unnamed namespace

//=============================================================================
//                           USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlb::TestInputIterator'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we use a 'bdlb::TestInputIterator' to test that an
// aggregation function compiles when instantiated with a pure input iterator.
//
// First, we define a function 'sum' that accepts two input iterators and
// returns the sum of all elements in range specified by them.
//..
    template <class IN_ITER>
    typename bsl::iterator_traits<IN_ITER>::value_type
    sum(IN_ITER first, IN_ITER last)
    {
        typedef typename bsl::iterator_traits<IN_ITER>::value_type CValueType;
        typedef typename bsl::remove_cv<CValueType>::type ValueType;

        ValueType total = 0;
        while (first != last) {
            total += *first++;
        }
        return total;
    }
//..
// Now, we define a function 'testSum' that first verifies that 'sum' correctly
// accumulates a sum, and then verifies, using 'bdlb::TestInputIterator', that
// 'sum' can be instantiated on an iterator that strictly matches the
// requirements of an empty input iterator:
//..
    int testSum()
    {
        static const int myArray[6] = { 2, 3, 5, 7, 11, 0 };

        // Verify that 'sum' correctly computes the sum using random access
        // iterators (pointers).
        int r1 = sum(&myArray[0], &myArray[5]);
        ASSERT(28 == r1);

        // Verify that 'sum' can be instantiated using a pure input iterator.
        typedef bdlb::TestInputIterator<unsigned> iterType;
        unsigned r2 = sum(iterType(), iterType());
        ASSERT(0 == r2);

        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int         test = argc > 1 ? atoi(argv[1]) : 0;
    const bool     verbose = argc > 2;
    const bool veryVerbose = argc > 3;

    printf("TEST" __FILE__ "CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        testSum();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 Two objects of 'bdlb::TestInputIterator' class, 'X' and 'Y',
        //:   always compare equal.
        //:
        //: 2 Two objects of 'bdlb::TestInputIterator' class, 'X' and 'Y',
        //:   never compare unequal.
        //:
        //: 3 Comparison is symmetric.
        //:
        //: 4 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //
        // Plan:
        //: 1 Create two different objects and verify the correctness of
        //:   'operator==' and 'operator!=' using them.  (C-1..4)
        //:
        //: 2 Create modifiable object and const reference pointing to it.
        //:   Verify the correctness of 'operator==' and 'operator!=' using
        //:   them.  (C-1..4)
        //
        // Testing:
        //   bool operator==(TestInputIterator&, TestInputIterator&);
        //   bool operator!=(TestInputIterator&, TestInputIterator&);
        // --------------------------------------------------------------------

        if (verbose) printf("EQUALITY-COMPARISON OPERATORS\n"
                            "=============================\n");

        if (veryVerbose) printf("\tTesting different objects\n");
        {
            bdlb::TestInputIterator<u::MyClass> nc1;
            bdlb::TestInputIterator<u::MyClass> nc2;
            ASSERT(nc2 == nc1);
            ASSERT(nc1 == nc2);
            ASSERT(!(nc2 != nc1));
            ASSERT(!(nc1 != nc2));
        }

        if (veryVerbose) printf("\tTesting copies\n");
        {
            bdlb::TestInputIterator<u::MyClass>        nc;
            const bdlb::TestInputIterator<u::MyClass>& NC(nc);
            ASSERT(NC == nc);
            ASSERT(nc == NC);
            ASSERT(!(NC != nc));
            ASSERT(!(nc != NC));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ITERATING OVER A RANGE
        //
        // Concerns:
        //: 1 That the class under test can iterate over a contiguous range.
        //:
        //: 2 That the class under test can be initialized with a pointer, or
        //:   with a contiguous iterator.
        //
        // Plan:
        //: 1 Use a 'vector' to create a range of 'int's.
        //:   o Create an iterator pair to the start and end of the vector
        //:     using the 'CONTIGUOUS_ITERATOR' c'tor.
        //:
        //:   o Traverse the range, examining the values with 'operator++()'
        //:     and 'operator*()'.
        //:
        //:   o Traverse the range again, examining the values with
        //:     'operator++(int)' and 'operator*()'.
        //:
        //:   o Create an iterator pair to the start and end of the vector
        //:     using the pointer c'tor.
        //:
        //:   o Traverse the range, examining the values with 'operator++()'
        //:     and 'operator*()'.
        //:
        //:   o Traverse the range again, examining the values with
        //:     'operator++(int)' and 'operator*()'.
        //:
        //: 2 Use a 'vector' to create a range of 'u::MyClass' objects.
        //:   o Create an iterator pair to the start and end of the vector
        //:     using the 'CONTIGUOUS_ITERATOR' c'tor.
        //:
        //:   o Traverse the range, examining the values with 'operator++()'
        //:     and 'operator->()'.
        //:
        //:   o Traverse the range again, examining the values with
        //:     'operator++(int)' and 'operator->()'.
        //:
        //:   o Create an iterator pair to the start and end of the vector
        //:     using the pointer c'tor.
        //:
        //:   o Traverse the range, examining the values with 'operator++()'
        //:     and 'operator->()'.
        //:
        //:   o Traverse the range again, examining the values with
        //:     'operator++(int)' and 'operator->()'.
        //
        // Testing:
        //   TestInputIterator(TYPE *);
        //   TestInputIterator(CONTIGUOUS_ITERATOR);
        //   TestInputIterator& operator++();
        //   TestInputIterator operator++(int);
        //   TYPE *operator->() const;
        //   TYPE operator*() const;
        // --------------------------------------------------------------------

        if (verbose) printf("ITERATING OVER A RANGE\n"
                            "======================\n");

        if (verbose) printf("Iterating over a range of 'int'\n");
        {
            std::vector<int> v;
            for (int ii = 0; ii < 100; ++ii) {
                v.push_back(ii);
            }

            typedef bdlb::TestInputIterator<int> Iter;
            int jj = 0;
            {
                const Iter begin(v.begin()), end(v.end());
                for (Iter it = begin; end != it; ++it) {
                    ASSERT(jj++ == *it);
                }
                ASSERT(100 == jj);

                jj = 0;
                for (Iter it = begin; end != it; ) {
                    ASSERT(jj++ == *it++);
                }
                ASSERT(100 == jj);
            }

            {
                const Iter begin(&v[0]), end(&v[0] + 100);
                jj = 0;
                for (Iter it = begin; end != it; ++it) {
                    ASSERT(jj++ == *it);
                }
                ASSERT(100 == jj);

                jj = 0;
                for (Iter it = begin; end != it; ) {
                    ASSERT(jj++ == *it++);
                }
                ASSERT(100 == jj);
            }
        }

        if (verbose) printf("Iterating over a range of 'u::MyClass'\n");
        {
            std::vector<u::MyClass> mv;
            for (int ii = 0; ii < 100; ++ii) {
                mv.push_back(u::MyClass(ii));
            }

            typedef bdlb::TestInputIterator<u::MyClass> Iter;
            int jj = 0;
            {
                const Iter begin(mv.begin()), end(mv.end());
                for (Iter it = begin; end != it; ++it) {
                    ASSERT(jj++ == it->value());
                }
                ASSERT(100 == jj);

                jj = 0;
                for (Iter it = begin; end != it; ) {
                    ASSERT(jj++ == (it++)->value());
                }
                ASSERT(100 == jj);
            }

            {
                const Iter begin(&mv[0]), end(&mv[0] + 100);
                jj = 0;
                for (Iter it = begin; end != it; ++it) {
                    ASSERT(jj++ == it->value());
                }
                ASSERT(100 == jj);

                jj = 0;
                for (Iter it = begin; end != it; ) {
                    ASSERT(jj++ == (it++)->value());
                }
                ASSERT(100 == jj);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //   Ensure that the four implicitly declared and defined special
        //   member functions (constructors, destructor and assignment
        //   operator) are publicly callable.  As there is no observable state
        //   to inspect, there is little to verify other than that the expected
        //   expressions all compile.
        //
        // Concerns:
        //: 1 Objects can be created using the default constructor.
        //:
        //: 2 Objects can be created using the copy constructor.
        //:
        //: 3 The copy constructor is not declared as explicit.
        //:
        //: 4 Objects can be assigned to from constant objects.
        //:
        //: 5 Assignments operations can be chained.
        //:
        //: 6 Objects can be destroyed.
        //
        // Plan:
        //: 1 Verify the default constructor exists and is publicly accessible
        //:   by default-constructing a 'const bdlb::TestInputIterator'
        //:   object.  (C-1)
        //:
        //: 2 Verify the copy constructor is publicly accessible and not
        //:   'explicit' by using the copy-initialization syntax to create a
        //:   second 'bdlb::TestInputIterator' from the first. (C-2..3)
        //:
        //: 3 Assign the value of the first ('const') object to the second.
        //:   (C-4)
        //:
        //: 4 Chain the assignment of the value of the first ('const') object
        //:   to the second, into a self-assignment of the second object to
        //:   itself. (C-5)
        //:
        //: 5 Verify the destructor is publicly accessible by allowing the two
        //:   'bdlb::TestInputIterator' object to leave scope and be
        //:   destroyed.  (C-6)
        //
        // Testing:
        //   TestInputIterator();
        //   TestInputIterator(const TestInputIterator&);
        //   ~TestInputIterator();
        //   TestInputIterator& operator=(const TestInputIterator&);
        // --------------------------------------------------------------------

        if (verbose) printf("PRIMARY MANIPULATORS\n"
                            "====================\n");

        if (veryVerbose) printf("\tTesting basic type\n");
        {
            typedef bdlb::TestInputIterator<int> Obj;

            int ii(3);
            const Obj NI1(&ii);
            Obj       ni2(NI1), ni3;
            ni2 = NI1;
            ni3 = ni2 = NI1;

            BSLMF_ASSERT((bsl::is_same<Obj::iterator_category,
                                       std::input_iterator_tag>::value));
            BSLMF_ASSERT((bsl::is_same<Obj::value_type, const int>::value));

            ASSERT(u::isConst(*ni3));
            ASSERT(! u::isConst(ni3));
        }

        if (veryVerbose) printf("\tTesting user-defined type\n");
        {
            typedef bdlb::TestInputIterator<u::MyClass> Obj;

            u::MyClass mc(5);
            const Obj NC1(&mc);
            Obj       nc2(NC1), nc3;
            nc2 = NC1;
            nc3 = (nc2 = NC1);

            BSLMF_ASSERT((bsl::is_same<Obj::iterator_category,
                                       std::input_iterator_tag>::value));
            BSLMF_ASSERT((bsl::is_same<Obj::value_type,
                                       const u::MyClass>::value));

            ASSERT(u::isConst(nc3->value()));
            ASSERT(! u::isConst(nc3));
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
        //: 1 Developer test sandbox. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");
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
// Copyright 2021 Bloomberg Finance L.P.
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
