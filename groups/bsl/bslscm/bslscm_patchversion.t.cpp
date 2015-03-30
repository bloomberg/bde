// bslscm_patchversion.t.cpp                                          -*-C++-*-

#include <bslscm_patchversion.h>

#include <stdio.h>   // print, fprintf
#include <stdlib.h>  // atoi


//==========================================================================
//                  STANDARD BSL ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

static int testStatus = 0;
static bool verbose = false;
static bool veryVerbose = false;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        fprintf(stderr, "Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s: %d\n", #I, I); \
                aSsErT(1, #X, __LINE__); } }

//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    (void)aSsErT;  // suppress warning

    verbose = (argc > 2);
    veryVerbose = (argc > 3);

    printf("TEST %s CASE %d\n", __FILE__, test);

    switch (test) { case 0:
      case 2: {
        //--------------------------------------------------------------------
        // TEST USAGE EXAMPLE
        //
        // Concern:
        //   That the usage example in the user documentation compiles and
        //   runs as expected.
        //
        // Plan:
        //   Use the exact text of the usage example from the user
        //   documentation, but guard any 'printf' statements with 'verbose'
        //   checks.
        //
        // Testing:
        //   USAGE EXAMPLE
        //--------------------------------------------------------------------

        if (verbose) printf("\nTEST USAGE EXAMPLE"
                            "\n==================\n");

// At compile time, the 'patch' version of BSL can be examined.
//..
        #if BSLSCM_PATCHVERSION_PATCH > 0
            if (verbose) printf("This is a patch release\n");
        #else
            if (verbose) printf("This is a \".0\" initial release\n");
        #endif
//..
      } break;

      case 1: {
        //--------------------------------------------------------------------
        // TEST SYMBOL EXISTENCE
        //
        // Concerns:
        //   That BSLSCM_PATCHVERSION_PATCH is defined.
        //
        // Plan:
        //   Make sure that the 'BSLSCM_PATCHVERSION_PATCH' symbol is defined
        //   by the header.
        //
        // Testing:
        //   BSLSCM_PATCHVERSION_PATCH
        //--------------------------------------------------------------------

        if (verbose) printf("\nTEST SYMBOL EXISTENCE"
                            "\n=====================\n");

        #if !defined(BSLSCM_PATCHVERSION_PATCH)
        #error BSLSCM_PATCHVERSION_PATCH is not defined.
        #endif
      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n",
                     testStatus);
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
