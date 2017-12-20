// bslmf_addvolatile.t.cpp                                            -*-C++-*-
#include <bslmf_addvolatile.h>

#include <bslmf_issame.h>  // for testing only

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace bsl;
using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, 'bsl::add_volatile', that
// adds a top-level 'volatile'-qualifier to a template parameter type.  Thus,
// we need to ensure that the values returned by the meta-function are correct
// for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC TYPES
// [ 1] bsl::add_volatile::type
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

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

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct TestType {
   // This user-defined type is intended to be used during testing as an
   // argument for the template parameter 'TYPE' of 'bsl::add_volatile'.
};

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                            "\n=============\n");

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Adding a 'volatile'-Qualifier to a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to add a 'volatile'-qualifier to a particular type.
//
// First, we create two 'typedef's -- a 'volatile'-qualified type
// ('MyVolatileType') and the same type without the 'volatile'-qualifier
// ('MyType'):
//..
        typedef int          MyType;
        typedef volatile int MyVolatileType;
//..
// Now, we add a 'volatile'-qualifier to 'MyType' using 'bsl::add_volatile' and
// verify that the resulting type is the same as 'MyVolatileType':
//..
        ASSERT(true == (bsl::is_same<bsl::add_volatile<MyType>::type,
                                                      MyVolatileType>::value));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::add_volatile'
        //   Ensure that the 'typedef' 'type' of 'bsl::add_volatile'
        //   has the correct type for a variety of template parameter types.
        //
        // Concerns:
        //: 1 'bsl::add_volatile' adds a top-level 'volatile'-qualifier only to
        //:   regular types (primitive, pointers, and user-defined types).
        //:
        //: 2 'bsl::add_volatile' does not add a 'volatile'-qualifier to
        //:   reference types, function types, or types that are already
        //:   'volatile'-qualified.
        //
        // Plan:
        //   Verify that 'bsl::add_volatile::type' has the correct type for
        //   each concern.
        //
        // Testing:
        //   bsl::add_volatile::type
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::add_volatile'\n"
                            "\n===================\n");

        // C-1
        ASSERT((is_same<add_volatile<int>::type, int volatile>::value));
        ASSERT((is_same<add_volatile<int *>::type, int * volatile>::value));
        ASSERT((is_same<add_volatile<int volatile *>::type,
                                             int volatile * volatile>::value));
        ASSERT((is_same<add_volatile<TestType>::type,
                                                   TestType volatile>::value));

        // C-2
        ASSERT((is_same<add_volatile<int &>::type, int &>::value));
        ASSERT((is_same<add_volatile<TestType &>::type, TestType &>::value));
        ASSERT((is_same<add_volatile<int (int)>::type, int (int)>::value));
        ASSERT((is_same<add_volatile<int volatile>::type,
                                                        int volatile>::value));
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
// Copyright 2013 Bloomberg Finance L.P.
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
