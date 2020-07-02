// bslstl_size.t.cpp                                                  -*-C++-*-
#include <bslstl_size.h>

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
// The component under test implements a version of the ratio operations
// introduced into the standard library in C++11 (and 14).  We test to make
// sure that the operations exist and give sane results.  The tests are not
// exhaustive.
//
// Groups of operations tested:
// * Basic ratio functionality - numerator/denominator/nested 'type'
// * SI prefixes - kilo, mega, milli, etc.
// * Ratio comparisons - less/greater/equal, etc.
// ** This includes the '_v' inline variables
// * Ratio operations - add/subtract/multiply/divide, etc.
// ** There are no '_t' versions of these operations.
// ----------------------------------------------------------------------------
// [ 1] '<ratio>' STL header

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
    native_std::size_t d_size;

  public:
    // CREATOR
    explicit
    MyContainer(native_std::size_t value)
    : d_size(value)
    {}

    // ACCESSOR
    native_std::size_t size() const
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
        //: 1 That the functions under test can access the 'size' accessor of
        //:   a container.
        //
        // Plan:
        //: 1 Create a type, 'MyContainer' in the unnamed namespace.
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
