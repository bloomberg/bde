// bsls_ident.t.cpp                                                   -*-C++-*-



// *** The format of this component test driver is non-standard. ***

// #include <bsls_ident.h>      // included below in usage example.
#include <cstdlib>              // 'atoi'
#include <cstdio>

// using namespace BloombergLP;

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
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                        TESTING USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// The following example shows how we first include 'bsls_ident.h' and then
// use the 'BSLS_IDENT' macro.  For header ('.h') files this macro should be
// placed directly after the include guards, e.g., 'bsls_somefile.h':
//..
    // bsls_somefile.h            -*-C++-*-
    #ifndef INCLUDED_BSLS_SOMEFILE
    #define INCLUDED_BSLS_SOMEFILE

    #include <bsls_ident.h>
    BSLS_IDENT("$Id: $")

    // ...

    #endif // INCLUDED_BSLS_SOMEFILE
//..
//  (For BDE-style headers, which make use of redundant includes guards,
//  see below.)
//
// For implementation ('.cpp') files the 'BSLS_IDENT' macro should be placed
// directly after the comment line for the file name and the language, e.g.,
// 'bsls_somefile.cpp':
//..
    // bsls_somefile.cpp          -*-C++-*-

    #include <bsls_ident.h>
    BSLS_IDENT("$Id: $")
//..
// Note that BDE-style components use *redundant* '#include' guards (see
// "HERE" below) in their header files, e.g., 'bsls_somefile.h':
//..
    // bsls_somefile.h            -*-C++-*-
    #ifndef INCLUDED_BSLS_SOMEFILE
    #define INCLUDED_BSLS_SOMEFILE

    #ifndef INCLUDED_BSLS_IDENT                         // <- HERE
    #include <bsls_ident.h>
    #endif                                              // <- HERE
    BSLS_IDENT("$Id: $")

    // ...

    #endif // INCLUDED_BSLS_SOMEFILE
//..


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;
//    int veryVerbose = argc > 3;
//    int veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

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

        if (verbose) printf( "\nBREATHING TEST"
                             "\n==============");

        ASSERT(true);  // Reference assert implementation

        printf("\nThere is no real runtime test for this component");


      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
      fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus );
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
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
