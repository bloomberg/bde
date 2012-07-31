// bsltf_enumeratedtesttype.t.cpp                                     -*-C++-*-
#include <bsltf_enumeratedtesttype.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_default.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace BloombergLP::bsltf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a single enumeration supporting
// convertion from integral values (0 to 127) to and from the enumeration.
//
//-----------------------------------------------------------------------------
// TYPES
// [ 1] enum Enum { ... };
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    int verbose         = argc > 2;
    int veryVerbose     = argc > 3;
    int veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
          if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using The Enumeration
/// - - - - - - - - - - - - - - - -
// First, we create an 'EnumeratedTestType::Enum' value and initialize it to
// the first possible value:
//..
          EnumeratedTestType::Enum first = EnumeratedTestType::FIRST;
          ASSERT(static_cast<int>(first) == 0);
//..
// Finally, we create an 'EnumeratedTestType::Enum' value and initilize it to
// the last possible value:
//..
          EnumeratedTestType::Enum last = EnumeratedTestType::LAST;
          ASSERT(static_cast<int>(last) == 127);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'enum'
        //
        // Concerns:
        //: 1 The 'enum' support convertion to and from integral values 0 to
        //:   127.
        //
        // Plan:
        //: 1 Use a loop-based approach, verify that integral values from 0 to
        //:   127 can be converted to the enumeration values.
        //
        // Testing:
        //   enum Enum { ... };
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'enum'"
                            "\n==============\n");

        for (int ti = 0; ti < 127; ++ti) {
            EnumeratedTestType::Enum val =
                                 static_cast<EnumeratedTestType::Enum>(ti);
            ASSERT(val == ti);
        }

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
// NOTICE: Copyright (C) Bloomberg L.P., 2012 All Rights Reserved.  Property of
// Bloomberg L.P. (BLP) This software is made available solely pursuant to the
// terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
