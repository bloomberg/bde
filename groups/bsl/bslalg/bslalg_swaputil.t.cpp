// bslalg_swaputil.t.cpp                                              -*-C++-*-

#include <bslalg_swaputil.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
//-----------------------------------------------------------------------------
// bslalg::SwapUtil public interface:
// [ 2] void swap(T *a, T *b);
//-----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 1] BREATHING TEST
//-----------------------------------------------------------------------------

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

//=============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
//-----------------------------------------------------------------------------
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/TYPES FOR TESTING
//-----------------------------------------------------------------------------

namespace BloombergLP {

struct SwapTester {
    bool swapped;

    SwapTester()
    : swapped(false)
    {}
};

inline
void swap(SwapTester& a, SwapTester& b)
{
    a.swapped = b.swapped = true;
}

struct SwapContainer
{
    SwapTester m_t;

    void swap(SwapContainer & other)
    {
        bslalg::SwapUtil::swap(&m_t, &other.m_t);
    }
};

inline
void swap(SwapContainer & a, SwapContainer & b)
{
    a.swap(b);
}

}  // close enterprise namespace

using namespace BloombergLP;

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: using 'bslalg::SwapUtil::swap'
/// - - - - - - - - - - - - - - - - - - - -
// In this example we define a type 'Container' and use 'bslalg::SwapUtil' to
// both implement a user-defined 'swap' for 'Container', and swap two container
// objects.
//
// We start by defining a class 'Container' in the 'xyz' namespace.  Further we
// assume that 'Container' has some expensive-to-copy data, so we provide a
// custom 'swap' algorithm to efficiently swap the data between a two objects
// this class by defining a 'swap' method and a 'swap' free function.
//..
namespace xyz {

class Container {
  private:
    int d_expensiveData;

  public:
    void swap(Container& other);
        // Swap the value of 'this' object with the value of the specified
        // 'other' object.  This method provides the no-throw
        // exception-safety guarantee.
};

void swap(Container& a, Container& b);
    // Swap the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee.
//..
// Note that the free function 'swap' is overloaded in the namespace of the
// class 'Container', which is 'xyz'.
//
// Next, we implemente the 'swap' method using the 'bslalg::SwapUtil::swap' to
// swap the individual data elements:
//..
inline
void Container::swap(Container& other)
{
    bslalg::SwapUtil::swap(&d_expensiveData, &other.d_expensiveData);

    // Equivalent to:
    // using bsl::swap;
    // bsl::swap(d_expensiveData, other.d_expensiveData);
}
//..
// Notice that calling 'bslalg::SwapUtil::swap' is equivalent to making the
// 'bsl::swap' available in the current scope by doing 'using bsl::swap' and
// making a subsequent call to an unqualified 'swap' function.
//
// Then, we implement the 'swap' free function:
//..
inline
void swap(Container& a, Container& b)
{
    a.swap(b);
}

}  // close namespace xyz
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// Finally we can use 'bslalg::SwapUtil::swap' to swap two objects of class
// 'xyz::Container':
//..
xyz::Container c1, c2;

bslalg::SwapUtil::swap(&c1, &c2);
//..
// The above code correctly calls the 'xyz::swap' overload for the 'Container'
// class.

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING swap
        //
        // Concerns:
        //: 1 'bslalg::SwapUtil::swap' calls the 'swap' found by ADL in the
        //:   namespace of a class for the class that provides its own 'swap'
        //:   overload.
        //: 2 'bslald_SwapUtil::swap' calls the generic 'bsl::swap' for a class
        //:   that doesn't provide its own 'swap' overload.
        //
        // Plan:
        //: 1 Create two objects of type 'SwapTester' which provides its own
        //:   overload of 'swap'.  Call the 'bslalg::SwapUtil::swap' on them
        //:   and verify that the overloaded 'swap' has been called.
        //: 2 Call 'bslalg::SwapUtil::swap' on two ints.  Since 'int' doesn't
        //:   have its own 'swap' overload, the generic 'swap' has to be found.
        //
        // Testing:
        //   void swap(T *a, T *b);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'swap'\n");

        SwapTester a;
        SwapTester b;

        ASSERT(!a.swapped);
        ASSERT(!b.swapped);

        bslalg::SwapUtil::swap(&a, &b);

        ASSERT(a.swapped);
        ASSERT(b.swapped);

        int c = 10;
        int d = 20;

        bslalg::SwapUtil::swap(&c, &d);

        ASSERT(c == 20);
        ASSERT(d == 10);

        if (verbose) printf("\nTesting 'swap' for a 'container' class\n");

        SwapContainer ca;
        SwapContainer cb;
        ASSERT(!ca.m_t.swapped);
        ASSERT(!cb.m_t.swapped);

        bslalg::SwapUtil::swap(&ca, &cb);

        ASSERT(ca.m_t.swapped);
        ASSERT(cb.m_t.swapped);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   The 'swap' utility function works as intended.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        int c = 10;
        int d = 20;

        bslalg::SwapUtil::swap(&c, &d);

        ASSERT(c == 20);
        ASSERT(d == 10);

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
