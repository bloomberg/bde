// bslmf_isconst.t.cpp                                                -*-C++-*-
#include <bslmf_isconst.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace bsl;
using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, 'bsl::is_const', that
// determines whether a template parameter type is a 'const'-qualified type.
// Thus, we need to ensure that the value returned by the meta-function is
// correct for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_const::value
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

namespace {

struct TestType {
   // This user-defined type is intended to be used for testing.
};

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void)veryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                            "\n=============\n");

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify 'Const' Types
///- - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is
// 'const'-qualified.
//
// First, we create two 'typedef's -- a 'const'-qualified type and an
// unqualified type:
//..
    typedef int        MyType;
    typedef const int  MyConstType;
//..
// Now, we instantiate the 'bsl::is_const' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
    ASSERT(false == bsl::is_const<MyType>::value);
    ASSERT(true  == bsl::is_const<MyConstType>::value);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_const::value'
        //   Ensure that 'bsl::is_const' returns the correct values for a
        //   variety of template parameter types.
        //
        // Concerns:
        //: 1 'is_const::value' is 'false' when 'TYPE' is a (possibly
        //:   'volatile'-qualified) type.
        //:
        //: 2 'is_const::value' is 'true' when 'TYPE' is a 'const'-qualified or
        //:    cv-qualified type.
        //
        // Plan:
        //   Verify that 'bsl::is_const::value' has the correct value for
        //   each concern.
        //
        // Testing:
        //   bsl::is_const::value
        // --------------------------------------------------------------------

        if (verbose) printf("\nbsl::is_const::value\n"
                            "\n====================\n");

        // C-1
        ASSERT(false == is_const<int>::value);
        ASSERT(false == is_const<int volatile>::value);

        ASSERT(false == is_const<TestType>::value);
        ASSERT(false == is_const<TestType volatile>::value);

        // C-2
        ASSERT(true == is_const<int const>::value);
        ASSERT(true == is_const<int const volatile>::value);

        ASSERT(true == is_const<TestType const>::value);
        ASSERT(true == is_const<TestType const volatile>::value);
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
// Copyright (C) 2013 Bloomberg L.P.
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
