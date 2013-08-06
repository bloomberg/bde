// bslma_managedptr_pairproxy.t.cpp                                   -*-C++-*-
#include <bslma_managedptr_pairproxy.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bsls_bsltestutil.h>

#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'memcpy'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//                             ---------
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
int testStatus = 0;

namespace {

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }

}

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
//                               TEST APPARATUS
// ----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                         HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct Dummy {};

}  // close unnamed namespace

//=============================================================================
//                  TEST PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&da);

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1. That the functions exist with the documented signatures.
        //   2. That the basic functionality works as documented.
        //
        // Plan:
        //   Exercise each function in turn and devise an elementary test
        //   sequence to ensure that the basic functionality is as documented.
        //
        // Testing:
        //   This test exercises basic functionality but *tests* *nothing*.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n--------------\n");

        // Construct an object using a simple instantiation of the template.
        bslma::ManagedPtr_PairProxy<Dummy, int> x = {0, 0};
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
