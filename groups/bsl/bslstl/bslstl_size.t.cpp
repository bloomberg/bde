// bslstl_size.t.cpp                                                  -*-C++-*-
#include <bslstl_size.h>

#include <bslstl_array.h>
#include <bslstl_deque.h>
#include <bslstl_vector.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>     // 'sprintf', 'snprintf' [NOT '<cstdio>', which does not
                       // include 'snprintf']
#include <stdlib.h>    // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// [ 2] size_t size(const CONTAINER&);
// [ 2] ptrdiff_t ssize(const CONTAINER&);
// [ 3] size_t size(const TYPE (&)[DIMENSION]);
// [ 3] ptrdiff_t ssize(const TYPE (&)[DIMENSION]);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

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

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

// ============================================================================
//                         OTHER MACROS
// ----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace {
namespace u {

class MyContainer {
    // DATA
    std::size_t d_size;

  public:
    // CREATOR
    explicit
    MyContainer(std::size_t value)
    : d_size(value)
    {}

    // ACCESSOR
    std::size_t size() const
    {
        return d_size;
    }
};

}  // close namespace u
}  // close unnamed namespace

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bsl::size' to Implement a 'Stack' Class Template
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to create a 'Stack' class template that contains a
// fixed-length container whose elements can be accessed via 'operator[]'.  The
// underlying container could be a 'bsl::vector', 'bsl::deque', 'bsl::array',
// or a raw array.
//
// For 'bsl::vector', 'bsl::deque', and 'bsl::array', we can call
// '<container>.size()' to get the capacity of the fixed-length container, but
// that won't work in the case of a raw array, so we use 'bsl::size'.
//
// First, we we declare the 'Stack' class templte:
//..
    template <class CONTAINER, class ELEMENT = typename CONTAINER::value_type>
    class Stack {
      public:
        // PUBLIC TYPES
        typedef ELEMENT value_type;

      private:
        // DATA
        CONTAINER      d_container;
        std::size_t    d_numElements;

      public:
        // CREATORS
        Stack();
            // Create a 'Stack'.  Use this constructor when the 'CONTAINER'
            // type default-constructs to the desired length.

        explicit
        Stack(std::size_t initialSize);
            // Create a 'Stack'.  Use this constructor when the 'CONTAINER'
            // type default constructs to 0 length, and has a single-argument
            // constructor that takes a 'size_t' to grow it to the specified
            // 'initialSize'.

        // Stack(const Stack&) = default;
        // ~Stack() = default;

        // MANIPULATORS
        void push(const value_type& value);
            // Push the specified 'value' onto this stack.  The behavior is
            // undefined if the stack is full.

        value_type pop();
            // Pop the element from the top of this stack and return it.  The
            // behavior is undefined if this stack is empty.

        // ACCESSOR
        std::size_t size() const;
            // Return the number of elements stored in the stack.

        const value_type& top() const;
            // Return a 'const' reference to the element at the top of this
            // stack.  The behavior is undefined if this 'Stack' is empty.

        BSLS_KEYWORD_CONSTEXPR std::size_t capacity() const;
            // Return the capacity of the stack.
    };
//..
// Next, we define all the methods other than 'capacity()':
//..
    // CREATORS
    template <class CONTAINER, class ELEMENT>
    Stack<CONTAINER, ELEMENT>::Stack()
    : d_numElements(0)
    {}

    template <class CONTAINER, class ELEMENT>
    Stack<CONTAINER, ELEMENT>::Stack(std::size_t initialSize)
    : d_container(initialSize)
    , d_numElements(0)
    {}

    // MANIPULATORS
    template <class CONTAINER, class ELEMENT>
    void Stack<CONTAINER, ELEMENT>::push(const value_type& value)
    {
        ASSERT(d_numElements < this->capacity());

        d_container[d_numElements++] = value;
    }

    template <class CONTAINER, class ELEMENT>
    typename Stack<CONTAINER, ELEMENT>::value_type
    Stack<CONTAINER, ELEMENT>::pop()
    {
        ASSERT(0 < d_numElements);

        return d_container[--d_numElements];
    }

    // ACCESSORS
    template <class CONTAINER, class ELEMENT>
    std::size_t Stack<CONTAINER, ELEMENT>::size() const
    {
        return d_numElements;
    }

    template <class CONTAINER, class ELEMENT>
    const typename Stack<CONTAINER, ELEMENT>::value_type&
    Stack<CONTAINER, ELEMENT>::top() const
    {
        ASSERT(0 < d_numElements);

        return d_container[d_numElements - 1];
    }
//..
// Now, we define 'capacity' and use 'bsl::size' to get the size of the
// underlying 'd_container'.
//
// Note that the underlying container object never grows or shrinks after
// construction, it has a fixed size throughout the lifetime of the 'Stack'.
// If it has a 'capacity' greater than its size, it never makes a difference
// here.
//
// So if the 'CONTAINER' is 'bsl::vector', 'stack.capacity()' returns
// 'd_container.size()', not 'd_container.capacity()'.
//..
    template <class CONTAINER, class ELEMENT>
    inline
    BSLS_KEYWORD_CONSTEXPR std::size_t
    Stack<CONTAINER, ELEMENT>::capacity() const
    {
        return bsl::size(d_container);
    }
//..
// Finally, in 'main', we use our new class based on several types of owned
// 'CONTAINER's: raw array, 'bsl::vector', 'bsl::deque', and 'bsl::array'.
//..
void usage()
{
    typedef int Array[10];
    Stack<Array, int> aStack;

    ASSERT(10 == aStack.capacity());
    ASSERT( 0 == aStack.size());

    aStack.push(5);
    aStack.push(7);
    aStack.push(2);

    ASSERT(3 == aStack.size());

    ASSERT(2 == aStack.top());
    ASSERT(2 == aStack.pop());

    Stack<bsl::vector<int> > vStack(10);

    ASSERT(10 == vStack.capacity());
    ASSERT( 0 == vStack.size());

    vStack.push(5);
    vStack.push(7);
    vStack.push(2);

    ASSERT(vStack.size() == 3);

    ASSERT(2 == vStack.top());
    ASSERT(2 == vStack.pop());

    Stack<bsl::deque<int> > dStack(10);

    ASSERT(10 == dStack.capacity());
    ASSERT( 0 == dStack.size());

    dStack.push(5);
    dStack.push(7);
    dStack.push(2);

    ASSERT(3 == dStack.size());

    ASSERT(2 == dStack.top());
    ASSERT(2 == dStack.pop());

    Stack<bsl::array<int, 10> > baStack;

    ASSERT(10 == baStack.capacity());
    ASSERT( 0 == baStack.size());

    baStack.push(5);
    baStack.push(7);
    baStack.push(2);

    ASSERT(3 == baStack.size());

    ASSERT(2 == baStack.top());
    ASSERT(2 == baStack.pop());
//..
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TEST USAGE EXAMPLE
        //
        // Concerns:
        //: 1 Provide a usage example.
        //:
        //: 2 Ensure that it is correct.
        //
        // Plan:
        //: 1 Compile and run the usage example.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("TEST USAGE EXAMPLE\n"
                            "==================\n");

        usage();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST ARRAY SIZE CALLS
        //
        // Concern:
        //: 1 That the functions under test can return the dimension of a
        //:   one-dimensional fixed-length array.
        //
        // Plan:
        //: 1 Create a number of fixed-length arrays of different lengths.
        //:
        //: 2 Call the functions under test on them.
        //
        // Testing
        //   size_t size(const TYPE (&)[DIMENSION]);
        //   ptrdiff_t ssize(const TYPE (&)[DIMENSION]);
        // --------------------------------------------------------------------

        if (verbose) printf("TEST ARRAY SIZE CALLS\n"
                            "=====================\n");

#define U_LENGTH(array)    (sizeof(array) / sizeof(array[0]))

        int ii;
        unsigned uu;

        if (verbose) printf("Array of 5 'doubles'\n");
        {
            double d[5];    ii = uu = 5;

            ASSERT(uu == bsl::size(d));
            ASSERT(ii == bsl::ssize(d));

            ASSERT(U_LENGTH(d) == bsl::size(d));
        }

        if (verbose) printf("Array of 500 'int's\n");
        {
            int array[500];    ii = uu = 500;

            ASSERT(uu == bsl::size(array));
            ASSERT(ii == bsl::ssize(array));

            ASSERT(U_LENGTH(array) == bsl::size(array));
        }

        if (verbose) printf("40 char quoted string\n");
        {
            ii = uu = 40;

            ASSERT(uu == bsl::size("forty char string                      "));
            ASSERT(ii == bsl::ssize(
                                   "forty char string                      "));

            ASSERT(U_LENGTH("forty char string                      ") ==
                         bsl::size("forty char string                      "));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST CONTAINER SIZE CALLS
        //
        // Concern:
        //: 1 That the functions under test return the number of elements in a
        //:   container that provides a 'size' accessor.
        //
        // Plan:
        //: 1 Create a type, 'MyContainer', in the unnamed namespace.
        //:
        //: 2 Create an object of type 'MyContainer'.
        //:
        //: 3 Call the functions under test on that object.
        //
        // Testing:
        //   size_t size(const CONTAINER&);
        //   ptrdiff_t ssize(const CONTAINER&);
        // --------------------------------------------------------------------

        if (verbose) printf("TEST CONTAINER SIZE CALLS\n"
                            "=========================\n");

        for (unsigned uu = 0; uu <= 10 * 1000; uu += 100) {
            u::MyContainer c(uu);

            ASSERT(bsl::size(c) == uu);
            ASSERT(bsl::size(c) == c.size());

            const int ii = uu;

            ASSERT(bsl::ssize(c) == ii);
            ASSERT(bsl::ssize(c) == static_cast<int>(c.size()));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 Demonstrate the basic functionality.
        //
        // Plan:
        //: 1 Demonstrate by taking the sizes of arrays.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

        if (verbose) printf("Test length of array\n");
        {
            int x[5];

            ASSERT(5 == bsl::size(x));
            ASSERT(5 == bsl::ssize(x));
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-Zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
