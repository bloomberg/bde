// bsls_linkcoercion.t.cpp                                            -*-C++-*-

#include <bsls_linkcoercion.h>
#include <bsls_linkcoercion.h>
#include <stdio.h>  // printf
#include <stdlib.h> // atoi


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This component does not have any easy way to do runtime checks.
//
//-----------------------------------------------------------------------------
// [ 1] Breathing Test (nothing)
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error %s (%d): %s    (failed)\n", __FILE__, i, s);

        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                        TESTING USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Example 1 - Force a Link-Time Dependency on 's_coerce'
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, declare a variable 's_coerce' of type 'const char *':
//..
    extern const char *s_coerce;
//..
// Then, use 'BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY' to add a dependency on
// this symbol into any object which includes the header:
//..
    BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(const char *,
                                              bsls_coerceexample_coerce,
                                              s_coerce)
//..
// Finally, in the corresponding '.cpp' file, the 's_coerce' symbol needs to be
// defined:
//..
    const char *s_coerce = "Link me in!";
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
//    int veryVerbose = argc > 3;
//    int veryVeryVerbose = argc > 4;

    printf("TEST %s CASE %d\n", __FILE__, test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns: That there is no real runtime test for this component.
        //
        // Plan:
        //   Do nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        ASSERT(true);  // Reference assert implementation

        if (verbose)
            printf("\nThere is no runtime test for this component\n");

      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d.\n", testStatus);
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
