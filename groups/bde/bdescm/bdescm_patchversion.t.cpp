// bdescm_patchversion.t.cpp                                          -*-C++-*-

#include <bdescm_patchversion.h>

#include <bsl_cstdio.h>  // bsl::print, bsl::fprintf
#include <bsl_cstdlib.h> // bsl::atoi

using namespace BloombergLP;

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;
static bool verbose = false;
static bool veryVerbose = false;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::fprintf(stderr, "Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::printf("%s: %d\n", #I, I); \
                aSsErT(1, #X, __LINE__); } }

//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) bsl::printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ bsl::printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

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

        if (verbose) bsl::printf("\nTEST USAGE EXAMPLE"
                                 "\n==================\n");

// At compile time, the 'patch' version of BDE can be examined.
//..
        #if BDESCM_PATCHVERSION_PATCH > 0
            if (verbose) bsl::printf("This is a patch release\n");
        #else
            if (verbose) bsl::printf("This is a \".0\" initial release\n");
        #endif
//..
      } break;

      case 1: {
        //--------------------------------------------------------------------
        // TEST SYMBOL EXISTENCE
        //
        // Concerns:
        //   That BDESCM_PATCHVERSION_PATCH is defined.
        //
        // Plan:
        //   Make sure that the 'BDESCM_PATCHVERSION_PATCH' symbol is defined
        //   by the header.
        //
        // Testing:
        //   BDESCM_PATCHVERSION_PATCH
        //--------------------------------------------------------------------

        if (verbose) bsl::printf("\nTEST SYMBOL EXISTENCE"
                                 "\n=====================\n");

        #if !defined(BDESCM_PATCHVERSION_PATCH)
        #error BDESCM_PATCHVERSION_PATCH is not defined.
        #endif
      } break;

      default: {
        bsl::fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::fprintf(stderr, "Error, non-zero test status = %d.\n",
                     testStatus);
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
