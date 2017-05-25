// bdlb_testinputiterator.t.cpp                                       -*-C++-*-
#include <bdlb_testinputiterator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>

#include <bslim_testutil.h>

#include <bslmf_issame.h> // for testing only

#include <bsls_asserttest.h>

using namespace BloombergLP;
using namespace bsl;

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
// [ 2] ~TestInputIterator();
//
// MANIPULATORS
// [ 2] TestInputIterator& operator=(const TestInputIterator&);
// [ 3] TestInputIterator& operator++();
// [ 3] TestInputIterator& operator++(int);
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

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

class MyClass {
    // A simple test class that can instantiated and copied to help test the
    // instantiation of 'bdlb::TestInputIterator' on a user-defined type.

  public:
    // CREATORS
    MyClass(int){ }
        // Construct a MyClass object.

    MyClass(const MyClass&){ }
        // Construct a copy of an object.

    ~MyClass() { }
        // Destroy an object.

    int value() const { return 0; }
        // Member function, required for dereference operator test.

  private:
    // MANIPULATORS
    MyClass& operator=(const MyClass& rhs);
        // The type used to instantiate 'bdlb::TestInputIterator' does not need
        // to be assignable, although it must be copy-constructible.  We
        // disable the assignment operator to ensure that we don't depend on
        // it.


};

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
        typename bsl::iterator_traits<IN_ITER>::value_type total = 0;
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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

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
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

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

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (veryVerbose) cout << "\tTesting different objects" << endl;
        {
            bdlb::TestInputIterator<MyClass> nc1;
            bdlb::TestInputIterator<MyClass> nc2;
            ASSERT(nc2 == nc1);
            ASSERT(nc1 == nc2);
            ASSERT(!(nc2 != nc1));
            ASSERT(!(nc1 != nc2));
        }

        if (veryVerbose) cout << "\tTesting copies" << endl;
        {
            bdlb::TestInputIterator<MyClass>        nc;
            const bdlb::TestInputIterator<MyClass>& NC(nc);
            ASSERT(NC == nc);
            ASSERT(nc == NC);
            ASSERT(!(NC != nc));
            ASSERT(!(nc != NC));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // UNSUPPORTED OPERATIONS
        //   Ensure that calls of the four implicitly declared and defined
        //   special member functions (increment operators, indirection
        //   operator and dereference operator) lead to assertion, but can be
        //   compiled.
        //
        // Concerns:
        //: 1 QoI: asserted forbidden function calls are detected when enabled.
        //:
        //: 2 Expressions with this operators are able to be compiled.
        //
        // Plan:
        //: 1 Manually call different methods of the object.  (C-1..2)
        //:
        //: 2 Verify that defensive checks are triggered for unacceptable
        //:   operator calls. (C-1)
        //
        // Testing:
        //   TestInputIterator& operator++();
        //   TestInputIterator& operator++(int);
        //   TYPE *operator->() const;
        //   TYPE operator*() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "UNSUPPORTED OPERATIONS" << endl
                          << "======================" << endl;

        if (verbose) cout <<
                            "\nNegative Testing of direct invocation." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            bdlb::TestInputIterator<int> ni;
            ASSERT_SAFE_FAIL(++ni);
            ASSERT_SAFE_FAIL(ni++);
            ASSERT_SAFE_FAIL(*ni);

            bdlb::TestInputIterator<MyClass> nc;
            ASSERT_SAFE_FAIL(++nc);
            ASSERT_SAFE_FAIL(nc++);
            ASSERT_SAFE_FAIL(*nc);
            ASSERT_SAFE_FAIL(nc->value());
        }

        if (verbose) cout << "\nNegative Testing of expressions" << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            bdlb::TestInputIterator<int> ni;
            ASSERT_SAFE_FAIL(bdlb::TestInputIterator<int>(++ni));
            ASSERT_SAFE_FAIL(bdlb::TestInputIterator<int>(ni++));
            ASSERT_SAFE_FAIL(MyClass nmci(*ni));

            bdlb::TestInputIterator<MyClass> nc;
            ASSERT_SAFE_FAIL(bdlb::TestInputIterator<MyClass> nc2(++nc));
            ASSERT_SAFE_FAIL(bdlb::TestInputIterator<MyClass> nc3(nc++));
            ASSERT_SAFE_FAIL(MyClass nmc(*nc));
            ASSERT_SAFE_FAIL(MyClass(nc->value()));
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
        //:   by default-constructing a 'const bdlb::TestInputIterator' object.
        //:  (C-1)
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
        //:   'bdlb::TestInputIterator' object to leave scope and be destroyed.
        //:   (C-6)
        //
        // Testing:
        //   TestInputIterator();
        //   TestInputIterator(const TestInputIterator&);
        //   ~TestInputIterator();
        //   TestInputIterator& operator=(const TestInputIterator&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;
        if (veryVerbose) cout << "\tTesting basic type" << endl;
        {
            typedef bdlb::TestInputIterator<int> Obj;

            const Obj NI1;
            Obj       ni2(NI1);
            ni2 = NI1;
            ni2 = ni2 = NI1;

            typedef bsl::iterator_traits<Obj> ObjTraits;
            BSLMF_ASSERT((bsl::is_same<ObjTraits::iterator_category,
                                       bsl::input_iterator_tag>::value));
            BSLMF_ASSERT((bsl::is_same<ObjTraits::value_type, int>::value));
        }

        if (veryVerbose) cout << "\tTesting user-defined type" << endl;
        {
            typedef bdlb::TestInputIterator<MyClass> Obj;

            const Obj NC1;
            Obj       nc2(NC1);
            nc2 = NC1;
            nc2 = nc2 = NC1;

            typedef bsl::iterator_traits<Obj> ObjTraits;
            BSLMF_ASSERT((bsl::is_same<ObjTraits::iterator_category,
                                       bsl::input_iterator_tag>::value));
            BSLMF_ASSERT((bsl::is_same<ObjTraits::value_type,
                                       MyClass>::value));
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

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
