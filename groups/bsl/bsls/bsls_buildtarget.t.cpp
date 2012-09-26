// bsls_buildtarget.t.cpp                                             -*-C++-*-

#include <bsls_buildtarget.h>
#include <bsls_platform.h>

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
// There is nothing to test here except that the constants are defined
// properly and according to the build-target macros.
//-----------------------------------------------------------------------------
// [ 1] bsls::ExcBuildTarget::d_isExcBuildTarget
// [ 1] bsls::MtBuildTarget::d_isMtBuildTarget
// [ 1] bsls_64BitBuildTarget::d_is64BitBuildTarget
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
#define L_ __LINE__                     // current Line number
#define T_ printf("\t");                // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#ifdef BDE_BUILD_TARGET_EXC
        ASSERT(1 == bsls::ExcBuildTarget::d_isExcBuildTarget);
#else
        ASSERT(0 == bsls::ExcBuildTarget::d_isExcBuildTarget);
#endif

#ifdef BDE_BUILD_TARGET_MT
        ASSERT(1 == bsls::MtBuildTarget::d_isMtBuildTarget);
#else
        ASSERT(0 == bsls::MtBuildTarget::d_isMtBuildTarget);
#endif

#ifdef BSLS_PLATFORM_CPU_64_BIT
        ASSERT(1 == bsls_64BitBuildTarget::d_is64BitBuildTarget);
#else
        ASSERT(0 == bsls_64BitBuildTarget::d_is64BitBuildTarget);
#endif

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
