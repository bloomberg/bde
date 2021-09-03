// bsltf_inputiterator.t.cpp                                          -*-C++-*-
#include <bsltf_inputiterator.h>

#include <bslmf_assert.h> // for testing only
#include <bslmf_issame.h> // for testing only

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <vector>

#include <cstddef>
#include <cstdlib>
#include <cstring>

#include <stdio.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// bsltf::InputIterator has no state and thus very little to test.  We need
// to verify that objects can be created and destroyed, that all forbidden
// operations calls lead to assert, and that that comparison operators return
// valid results.  The usage example completes the test by proving that it
// works in idiomatic use.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] InputIterator();
// [ 2] InputIterator(const InputIterator&);
// [ 3] InputIterator(TYPE *);
// [ 2] ~InputIterator();
//
// MANIPULATORS
// [ 2] InputIterator& operator=(const InputIterator&);
// [ 3] InputIterator& operator++();
// [ 3] InputIterator operator++(int);
//
// ACCESSORS
// [ 3] TYPE *operator->() const;
// [ 3] TYPE operator*() const;
//
// FREE OPERATORS
// [ 4] bool operator==(InputIterator&, InputIterator&);
// [ 4] bool operator!=(InputIterator&, InputIterator&);
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
    // instantiation of 'bsltf::InputIterator' on a user-defined type.

    int d_value;

  public:
    // CREATORS
    MyClass() : d_value(0)
        // Create a 'MyClass' object with value 0.
    {}

    explicit MyClass(int value)
    : d_value(value)
        // Construct a MyClass object from the specified 'value'.
    {}

    // MyClass(const MyClass& original) = default;
        // Construct a copy of an object.

    // ~MyClass() = default;
        // Destroy an object.

    // MANIPULATORS
    //  MyClass& operator=(const MyClass& rhs) = default;
        // Assign the specified 'rhs' to this object and return a reference to
        // this object.

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
///Example 1: Basic Use of 'bsltf::InputIterator':
/// - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we use a 'bsltf::InputIterator' to test that an
// aggregation function compiles and works when instantiated with a pure input
// iterator.
//
// First, we define a function 'sum' that accepts two input iterators and
// returns the sum of all elements in range specified by them:
//..
    template <class IN_ITER>
    double sum(IN_ITER first, IN_ITER last)
        // Return the sum of the 'double's in the specified range
        // '[ first, last )'.
    {
        double total = 0;
        while (first != last) {
            total += *first++;
        }
        return total;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int         test = argc > 1 ? std::atoi(argv[1]) : 0;
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

// Then, in 'main', we define an array of 'double's and define 'InputIterators'
// pointing to the beginning and ending of it, initializing the iterators with
// pointers:
//..
    static const double myArray[] = { 2.5, 3, 5, 7, 11.5, 5 };
    enum { k_MY_ARRAY_LEN = sizeof myArray / sizeof *myArray };

    typedef bsltf::InputIterator<const double> Iter;

    Iter begin(myArray + 0), end(myArray + k_MY_ARRAY_LEN);
//..
// Next, we call 'sum' with the two iterators, and observe that its yields the
// expected result, and because it compiles, we know that 'sum' did not attempt
// any operations on the iterators other than those defined for the most basic
// input iterator:
//..
    const double x = sum(begin, end);
    ASSERT(34.0 == x);
//..
// Then, we illustrate that we can just make 'begin' and 'end' iterators from
// the array directly with the 'begin' and 'end' class methods of the
// 'InputIteratorUtil' class.
//..
    typedef bsltf::InputIteratorUtil Util;

    const double y = sum(Util::begin(myArray), Util::end(myArray));
    ASSERT(34.0 == y);
//..
// Now, we make an 'std::vector' containing the elements of 'myArray':
//..
    const std::vector<double> v(myArray + 0, myArray + k_MY_ARRAY_LEN);
//..
// Finally, we call 'sum' using, again, the 'begin' and 'end' class methods to
// create iterators for it directly from our 'vector':
//..
    const double z = sum(Util::begin(v), Util::end(v));
    ASSERT(34.0 == z);
//..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 Two objects of 'bsltf::InputIterator' class, 'X' and 'Y',
        //:   compare equal if and only if they have the same value.
        //:
        //: 2 Two objects of 'bsltf::InputIterator' class, 'X' and 'Y',
        //:   compare unequal if and only if they have different values.
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
        //   bool operator==(InputIterator&, InputIterator&);
        //   bool operator!=(InputIterator&, InputIterator&);
        // --------------------------------------------------------------------

        if (verbose) printf("EQUALITY-COMPARISON OPERATORS\n"
                            "=============================\n");

        typedef bsltf::InputIterator<u::MyClass> Iter;

        u::MyClass mc[2] = { u::MyClass(0), u::MyClass(0) };

        Iter       nc1(&mc[0]), nc2(&mc[0]), nc3(&mc[1]);
        const Iter NC1(&mc[0]), NC2(&mc[0]), NC3(&mc[1]);


        if (veryVerbose) printf("\tTesting different objects\n");
        {                                                                     \
            ASSERT(nc2 == nc1);
            ASSERT(nc1 == nc2);
            ASSERT(nc1 != nc3);
            ASSERT(nc3 != nc1);

            ASSERT(!(nc2 != nc1));
            ASSERT(!(nc1 != nc2));
            ASSERT(!(nc3 == nc1));
            ASSERT(!(nc1 == nc3));
        }

        if (veryVerbose) printf("\tTesting const copies\n");
        {
            ASSERT(nc1 == NC1);
            ASSERT(NC1 == nc1);
            ASSERT(!(nc1 != NC1));
            ASSERT(!(NC1 != nc1));

            ASSERT(nc2 == NC2);
            ASSERT(NC2 == nc2);
            ASSERT(!(nc2 != NC2));
            ASSERT(!(NC2 != nc2));

            ASSERT(nc3 == NC3);
            ASSERT(NC3 == nc3);
            ASSERT(!(nc3 != NC3));
            ASSERT(!(NC3 != nc3));


            ASSERT(NC2 == NC1);
            ASSERT(NC1 == NC2);
            ASSERT(NC1 != NC3);
            ASSERT(NC3 != NC1);

            ASSERT(!(NC2 != NC1));
            ASSERT(!(NC1 != NC2));
            ASSERT(!(NC3 == NC1));
            ASSERT(!(NC1 == NC3));


            ASSERT(NC2 == nc1);
            ASSERT(NC1 == nc2);
            ASSERT(NC1 != nc3);
            ASSERT(NC3 != nc1);

            ASSERT(!(NC2 != nc1));
            ASSERT(!(NC1 != nc2));
            ASSERT(!(NC3 == nc1));
            ASSERT(!(NC1 == nc3));

            ASSERT(nc2 == NC1);
            ASSERT(nc1 == NC2);
            ASSERT(nc1 != NC3);
            ASSERT(nc3 != NC1);

            ASSERT(!(nc2 != NC1));
            ASSERT(!(nc1 != NC2));
            ASSERT(!(nc3 == NC1));
            ASSERT(!(nc1 == NC3));
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
        //   InputIterator(TYPE *);
        //   InputIterator& operator++();
        //   InputIterator operator++(int);
        //   InputIteratorUtil::begin(vector);
        //   InputIteratorUtil::end(vector);
        //   TYPE *operator->() const;
        //   TYPE operator*() const;
        // --------------------------------------------------------------------

        if (verbose) printf("ITERATING OVER A RANGE\n"
                            "======================\n");

        typedef bsltf::InputIteratorUtil Util;

        if (verbose) printf("Iterating over a range of 'int'\n");
        {
            std::vector<int> v;
            for (int ii = 0; ii < 100; ++ii) {
                v.push_back(ii);
            }

            typedef bsltf::InputIterator<int> Iter;
            int jj = 0;
            {
                const Iter end = Util::end(v);
                for (Iter it = Util::begin(v); end != it; ++it) {
                    ASSERT(jj++ == *it);
                }
                ASSERT(100 == jj);

                jj = 0;
                for (Iter it = Util::begin(v); end != it; ) {
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

            int array[100];
            std::memcpy(array, &v[0], 100 * sizeof(int));

            jj = 0;
            {
                const Iter end = Util::end(array);
                for (Iter it = Util::begin(array); end != it; ++it) {
                    ASSERT(jj++ == *it);
                }
                ASSERT(100 == jj);

                jj = 0;
                for (Iter it = Util::begin(array); end != it; ) {
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

            typedef bsltf::InputIterator<u::MyClass> Iter;
            int jj = 0;
            {
                const Iter end = Util::end(mv);
                for (Iter it = Util::begin(mv); end != it; ++it) {
                    ASSERT(jj++ == it->value());
                }
                ASSERT(100 == jj);

                jj = 0;
                for (Iter it = Util::begin(mv); end != it; ) {
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

            u::MyClass array[100];
            std::memcpy(array, &mv[0], 100 * sizeof(int));

            jj = 0;
            {
                const Iter end = Util::end(array);
                for (Iter it = Util::begin(array); end != it; ++it) {
                    ASSERT(jj++ == it->value());
                }
                ASSERT(100 == jj);

                jj = 0;
                for (Iter it = Util::begin(array); end != it; ) {
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
        //:   by default-constructing a 'const bsltf::InputIterator' object.
        //:   (C-1)
        //:
        //: 2 Verify the copy constructor is publicly accessible and not
        //:   'explicit' by using the copy-initialization syntax to create a
        //:   second 'bsltf::InputIterator' from the first.  (C-2..3)
        //:
        //: 3 Assign the value of the first ('const') object to the second.
        //:   (C-4)
        //:
        //: 4 Chain the assignment of the value of the first ('const') object
        //:   to the second, into a self-assignment of the second object to
        //:   itself.  (C-5)
        //:
        //: 5 Verify the destructor is publicly accessible by allowing the two
        //:   'bsltf::InputIterator' object to leave scope and be destroyed.
        //:   (C-6)
        //
        // Testing:
        //   InputIterator();
        //   InputIterator(const InputIterator&);
        //   ~InputIterator();
        //   InputIterator& operator=(const InputIterator&);
        // --------------------------------------------------------------------

        if (verbose) printf("PRIMARY MANIPULATORS\n"
                            "====================\n");

        if (veryVerbose) printf("\tTesting basic type\n");
        {
            typedef bsltf::InputIterator<int> Obj;

            int ii(3);
            const Obj NI1(&ii);
            Obj       ni2(NI1), ni3;
            ni2 = NI1;
            ni3 = ni2 = NI1;

            BSLMF_ASSERT((bsl::is_same<Obj::iterator_category,
                                       std::input_iterator_tag>::value));
            BSLMF_ASSERT((bsl::is_same<Obj::value_type, int>::value));

            ASSERT(! u::isConst(*ni3));
            ASSERT(! u::isConst(ni3));
        }

        if (veryVerbose) printf("\tTesting user-defined type\n");
        {
            typedef bsltf::InputIterator<u::MyClass> Obj;

            u::MyClass mc(5);
            const Obj NC1(&mc);
            Obj       nc2(NC1), nc3;
            nc2 = NC1;
            nc3 = (nc2 = NC1);

            BSLMF_ASSERT((bsl::is_same<Obj::iterator_category,
                                       std::input_iterator_tag>::value));
            BSLMF_ASSERT((bsl::is_same<Obj::value_type,
                                       u::MyClass>::value));

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
