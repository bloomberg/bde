// bbescm_versiontag.t.cpp    -*-C++-*-

#include <bbescm_versiontag.h>

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstdio.h>

using namespace BloombergLP;

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;
static bool verbose = false;
static bool veryVerbose = false;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        std::fprintf(stderr, "Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { std::printf("%s: %d\n", #I, I); \
                aSsErT(1, #X, __LINE__); } }

//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
 #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) std::printf("<| " #X " |>\n");  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ std::printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  USAGE EXAMPLE HELPER FUNCTIONS
//-----------------------------------------------------------------------------

int newFunction()
    // Return 1
{
    return 1;
}

// int oldFunction()
// Not defined and never called due to conditional compilation

//=============================================================================
//                  MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);

    std::printf("TEST %s CASE %d\n", __FILE__, test);

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
        //   documentation, but change uses of 'assert' to 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        //--------------------------------------------------------------------

        if (verbose) std::printf("\nTEST USAGE EXAMPLE"
                                 "\n==================\n");

    #if BBE_VERSION > BDE_MAKE_VERSION(1, 2)
        // Call 'newFunction' for BBE version 1.2 and later:
        int result = newFunction();
    #else
        // Call 'oldFunction' for BBE older than version 1.2:
        int result = oldFunction();
    #endif

        ASSERT(result);
      } break;

      case 1: {
        //--------------------------------------------------------------------
        // TEST VERSION CONSISTENCY
        //
        // Concerns:
        //   That BBE_VERSION corresponds to the three components,
        //   BBE_VERSION_MAJOR, BBE_VERSION_MINOR, and BBE_VERSION_PATCH.
        //
        // Plan:
        //   Decompose BBE_VERSION into its three components and verify
        //   that they correspond to the defined macros.
        //
        // Testing:
        //   BBE_VERSION
        //   BBE_VERSION_MAJOR
        //   BBE_VERSION_MINOR
        //   BBE_VERSION_PATCH
        //--------------------------------------------------------------------

        if (verbose) std::printf("\nTEST VERSION CONSISTENCY"
                                 "\n========================\n");

        int major = (BBE_VERSION / 10000) % 100;
        int minor = (BBE_VERSION / 100) % 100;
        int patch = BBE_VERSION % 100;

        ASSERT(BBE_VERSION_MAJOR == major);
        ASSERT(BBE_VERSION_MINOR == minor);
        ASSERT(0 == patch);
      } break;

      default: {
        std::fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::fprintf(stderr, "Error, non-zero test status = %d.\n",
                     testStatus);
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
