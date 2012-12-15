// bsls_ident.t.cpp                                                   -*-C++-*-



// *** The format of this component test driver is non-standard. ***

// #include <bsls_ident.h>      // included below in usage example.
#include <cstdlib>              // 'atoi'
#include <iostream>

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
        std::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << std::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { std::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { std::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { std::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { std::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { std::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { std::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) std::cout << #X " = " << (X) << std::endl; // Print ID and value.
#define Q(X) std::cout << "<| " #X " |>" << std::endl;  // Quote ID literally.
#define P_(X) std::cout << #X " = " << (X) << ", " << flush; // P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ std::cout << "\t" << flush;             // Print a tab (w/o newline)

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

    std::cout << "TEST " << __FILE__ << " CASE " << test << std::endl;;

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

        if (verbose) std::cout << "\nBREATHING TEST"
                               << "\n==============" << std::endl;

        ASSERT(true);  // Reference assert implementation

        std::cout << "\nThere is no real runtime test for this component"
                  << std::endl;

      } break;
      default: {
        std::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << std::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::cerr << "Error, non-zero test status = " << testStatus << "."
                  << std::endl;
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
