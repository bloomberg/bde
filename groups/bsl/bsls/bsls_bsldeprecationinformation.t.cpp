// bsls_bsldeprecationinformation.t.cpp                               -*-C++-*-
#include <bsls_bsldeprecationinformation.h>

#include <bsls_deprecate.h>

#include <stdio.h>
#include <stdlib.h>  // 'atoi'
#include <string.h>  // 'strcmp'

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component component under test defines a family of macros that control
// whether or not deprecations introduced in any given version of 'bsl' are
// active.  The component has no user interface as such, so the only test cases
// are ad-hoc and appear "below-the-line".
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// [ 1] CONFIRM VERSIONS WITH ACTIVE DEPRECATIONS

// BDE_VERIFY pragma: -TP19

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                              TEST APPARATUS
// ----------------------------------------------------------------------------

#define EMPTY_STRING ""

#define MACRO_TO_STRING(...)  MACRO_TO_STRING_A((__VA_ARGS__, EMPTY_STRING))
#define MACRO_TO_STRING_A(T)  MACRO_TO_STRING_B T
#define MACRO_TO_STRING_B(M, ...) #M

// ============================================================================
//                       GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#if defined(BSLS_DEPRECATE_ATTRIBUTE)
const char *deprecationAttributeString =
                                     MACRO_TO_STRING(BSLS_DEPRECATE_ATTRIBUTE);
#else
const char *deprecationAttributeString = EMPTY_STRING;
#endif

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void)     veryVerbose;  // Suppress unused-variable warnings
    (void) veryVeryVerbose;  // Suppress unused-variable warnings

    fprintf(stderr, "TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // CONFIRM VERSIONS WITH ACTIVE DEPRECATIONS
        //   Appropriate 'BSL_VERSION_DEPRECATE_<M>_<N>' macros are defined for
        //   all versions with active deprecations.
        //
        // Concerns:
        //: 1 Desired macros are defined.
        //:
        //: 2 Desired macros are correctly rendered.
        //:
        //: 2 Un-desired macros are not defined.
        //
        // Plan:
        //: 1 Compare the expansion of 'BSLS_DEPRECATE_IS_ACTIVE' for each
        //:   version of BSL known to have active deprecation with the
        //:   expansion of 'BSLS_DEPRECATE_ATTRIBUTE'.  (C-1,2)
        //:
        //: 2 Compare the expansion of 'BSLS_DEPRECATE_IS_ACTIVE' for nearby
        //:   version of BSL known to *not* have active deprecation with the
        //:   expansion of an empty macro definition.  (C-3)
        //
        // Testing:
        //   CONFIRM VERSIONS WITH ACTIVE DEPRECATIONS
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONFIRM VERSIONS WITH ACTIVE DEPRECATIONS"
                            "\n=========================================\n");

#if BSLS_DEPRECATE_IS_ACTIVE(BSL, 2, 27)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(BSL, 3,  0)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(BSL, 3,  1)
        ASSERT(true);
#else
        ASSERT(false);
#endif

#if BSLS_DEPRECATE_IS_ACTIVE(BSL, 3,  2)
        ASSERT(false);
#else
        ASSERT(true);
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

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
