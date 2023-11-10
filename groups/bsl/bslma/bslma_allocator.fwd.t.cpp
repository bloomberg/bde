// bslma_allocator.fwd.t.cpp                                          -*-C++-*-
#include <bslma_allocator.fwd.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test forward declares memory-allocation mechanisms.
// ----------------------------------------------------------------------------
// TYPES
//: o 'Allocator'
//
// ----------------------------------------------------------------------------
// [ 1] FORWARD DECLARATIONS
// ----------------------------------------------------------------------------


// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace


// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//               STANDARD FORWARDING HEADER TESTING UTILITIES
// ----------------------------------------------------------------------------

namespace u {
                    // ================================
                    // 'isIncomplete' function template
                    // ================================

template <class TYPE> bool isIncomplete(int(*)[sizeof(TYPE)]);
template <class TYPE> bool isIncomplete(...);
    // Return 'true' when invoked with literal '0' argument if 'TYPE' is
    // incomplete, 'false' if 'TYPE' is complete.

                    // --------------------------------
                    // 'isIncomplete' function template
                    // --------------------------------

template <class TYPE>
bool isIncomplete(int(*)[sizeof(TYPE)])
{
    return false;
}

template <class TYPE>
bool isIncomplete(...)
{
    return true;
}

}  // close namespace u

// ============================================================================
//                         NAME COMPLETION DECLARATION
// ----------------------------------------------------------------------------

static void testTypeCompleteness ();
    // Verify that the names declared in this forwarding header are complete
    // when the primary header is included.  Note that the definition of this
    // function is at the end of this file after 'main' and the inclusion of
    // the primary header.

//-----------------------------------------------------------------------------
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;  (void)veryVerbose;
    bool veryVeryVerbose = argc > 4;  (void)veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // FORWARD DECLARATIONS
        //
        // Concerns:
        //: 1 Each type that should be available has been declared by having
        //: included the forwarding header for this component.
        //:
        //: 2 The types are incomplete.
        //:
        //: 3 The types are completed by including the primary header.
        //
        // Plan:
        //: 1 Verify that each expected name is declared and is incomplete
        //: using 'isIncomplete'.
        //:
        //: 2 Invoke 'testTypeCompleteness', defined below after the inclusion
        //: of the primary header, to verify that the forward names are
        //: completed by including the primary header.
        //
        // Testing:
        //   FORWARD DECLARATIONS
        // --------------------------------------------------------------------
        if (verbose) printf("\nFORWARD DECLARATIONS"
                            "\n====================\n");

        ASSERT(u::isIncomplete<bslma::Allocator>(0));

        testTypeCompleteness ();
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

// Verify that the primary header can be included *after* the forwarding
// header.

#include <bslma_allocator.h>

// Verify that the primary header defined the types only declared by '.fwd.h'.

static void testTypeCompleteness ()
{
    ASSERT(!u::isIncomplete<bslma::Allocator>(0));
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
