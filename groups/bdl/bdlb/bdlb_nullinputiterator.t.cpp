// bdlb_nullinputiterator.t.cpp                                       -*-C++-*-

#include <bdlb_nullinputiterator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// bdlb::NullInputIterator has no state and thus very little to test.  One
// test case is sufficient to test that every method can be instantiated.  The
// usage example completes the test by proving that it works in idiomatic use.
// ----------------------------------------------------------------------------
// CREATORS
// [1] NullInputIterator();
// [1] NullInputIterator(const NullInputIterator& rhs);
// [1] ~NullInputIterator();
//
// MANIPULATORS
// [1] NullInputIterator& operator=(const NullInputIterator& rhs);
// [1] NullInputIterator& operator++();
// [1] NullInputIterator& operator++(int);
//
// ACCESSORS
// [1] TYPE* operator->() const;
// [1] TYPE operator*() const;
//
// FREE OPERATORS
// [1] bool operator==(NullInputIterator&, NullInputIterator&);
// [1] bool operator!=(NullInputIterator&, NullInputIterator&);
// ----------------------------------------------------------------------------
// [2] USAGE EXAMPLE

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
    // instantiation of 'bdlb::NullInputIterator' on a user-defined type.

  public:
    MyClass(int v = 0) : d_value(v) { }
    MyClass(const MyClass& rhs) : d_value(rhs.d_value) { }
    ~MyClass() { }

    int value() const { return d_value; }

  private:
    // The type used to instantiate 'bdlb::NullInputIterator' does not need to
    // be assignable, although it must be copy-constructible.  We disable the
    // assignment operator to ensure that we don't depend on it.
    MyClass& operator=(const MyClass& rhs);

    int d_value;
};

//=============================================================================
//                           USAGE EXAMPLE
//-----------------------------------------------------------------------------
//
// In the following example we use a 'bdlb::NullInputIterator' to test that
// function compiles when instantiated with a pure input iterator.
//
// First, we define a function 'sum' that accepts two input iterators and
// returns sum of all elements in range specified by them.
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
// accumulates a sum, and then verifies, using 'bdlb::NullInputIterator', that
// 'sum' can be instantiated on an iterator that strictly matches the
// requirements of a null input iterator:
//..
  int testSum()
  {
      static const int myArray[6] = { 2, 3, 5, 7, 11, 0 };

      // Verify that 'sum' correctly computes the sum using random access
      // iterators (pointers).
      int r1 = sum(&myArray[0], &myArray[5]);
      ASSERT(28 == r1);

      // Verify that 'sum' can be instantiated using a pure input iterator.
      typedef bdlb::NullInputIterator<unsigned> iterType;
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
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
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
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

        testSum();

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // BASIC TEST
        //   This case exercises all methods
        //
        // Concerns:
        //: 1 The default constructor can create an object.
        //:
        //: 2 The copy constructor can create a copy of object.
        //:
        //: 3 Assignment assigns the value of the original object.
        //:
        //: 4 The comparison operator 'operator==' returns true for two
        //:   different objects
        //:
        //: 5 The comparison operator 'operator!=' returns false for two
        //:   different objects
        //:
        //: 6 The template can be instantiated both for basic types and
        //:   user-defined types.
        //:
        //: 7 The template correctly defines the input iterator traits.
        //:
        //: 8 Calls of increment operators, dereference operator and member
        //:   access operator are unacceptable.
        //
        // Plan:
        //: 1 Manually call different methods of the object.  (C-1..7)
        //:
        //: 2 Verify that defensive checks are triggered for unacceptable
        //:   operator calls. (C-8)
        //
        // Testing:
        //   NullInputIterator();
        //   NullInputIterator(const NullInputIterator& rhs);
        //   ~NullInputIterator();
        //   NullInputIterator& operator=(const NullInputIterator& rhs);
        //   NullInputIterator& operator++();
        //   NullInputIterator& operator++(int);
        //   TYPE* operator->() const;
        //   TYPE operator*() const;
        //   bool operator==(NullInputIterator&, NullInputIterator&);
        //   bool operator!=(NullInputIterator&, NullInputIterator&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBASIC TEST"
                          << "\n==============" << endl;

        const bdlb::NullInputIterator<int> NI1;
        bdlb::NullInputIterator<int>       ni2(NI1);
        ni2 = NI1;
        ASSERT(NI1 == ni2);
        ASSERT(! (NI1 != ni2));
        ASSERT(NI1 == bdlb::NullInputIterator<int>())
        bsl::iterator_traits<bdlb::NullInputIterator<int> > TI1;
        (void)TI1;

        const bdlb::NullInputIterator<MyClass> NC1;
        bdlb::NullInputIterator<MyClass>       nc2(NC1);
        nc2 = NC1;
        ASSERT(NC1 == nc2);
        ASSERT(! (NC1 != nc2));
        ASSERT(NC1 == bdlb::NullInputIterator<MyClass>())
        bsl::iterator_traits<bdlb::NullInputIterator<MyClass> > TC1;
        (void)TC1;

        if (verbose) cout << "\nNegative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);
            ASSERT_SAFE_FAIL(++ni2);
            ASSERT_SAFE_FAIL(ni2++);
            ASSERT_SAFE_FAIL((void)*ni2);

            ASSERT_SAFE_FAIL(MyClass c = *nc2);
            ASSERT_SAFE_FAIL(++nc2);
            ASSERT_SAFE_FAIL(nc2++);
            ASSERT_SAFE_FAIL((void)nc2->value());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

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
