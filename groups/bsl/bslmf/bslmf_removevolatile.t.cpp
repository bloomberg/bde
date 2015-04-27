// bslmf_removevolatile.t.cpp                                         -*-C++-*-
#include <bslmf_removevolatile.h>

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
// The component under test defines a meta-function, 'bsl::remove_volatile',
// that removes any top-level 'volatile'-qualifier from a template parameter
// type.  Thus, we need to ensure that the values returned by the meta-function
// are correct for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::remove_volatile::type
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct TestType {
   // This user-defined type is intended to be used during testing as an
   // argument for the template parameter 'TYPE' of 'bsl::remove_volatile'.
};

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void) veryVerbose;

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
///Example 1: Removing the 'volatile'-qualifier of a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove any 'volatile'-qualifier from a particular
// type.
//
// First, we create two 'typedef's -- a 'volatile'-qualified type
// ('MyVolatileType') and the same type without the 'volatile'-qualifier
// ('MyType'):
//..
        typedef int          MyType;
        typedef volatile int MyVolatileType;
//..
// Now, we remove the 'volatile'-qualifier from 'MyVolatileType' using
// 'bsl::remove_volatile' and verify that the resulting type is the same as
// 'MyType':
//..
        ASSERT(true ==
               (bsl::is_same<bsl::remove_volatile<MyVolatileType>::type,
                                                              MyType>::value));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::remove_volatile::type'
        //   Ensure that the 'typedef' 'type' of 'bsl::remove_volatile' has the
        //   correct type for a variety of template parameter types.
        //
        // Concerns:
        //: 1 'bsl::remove_volatile' leaves types that are not
        //:   'volatile'-qualified at the top-level as-is.
        //:
        //: 2 'bsl::remove_volatile' removes any top-level
        //:   'volatile'-qualifier.
        //
        // Plan:
        //   Verify that 'bsl::remove_volatile::type' has the correct type for
        //   each concern.
        //
        // Testing:
        //   bsl::remove_volatile::type
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::remove_volatile::type'\n"
                            "\n============================\n");

        // C-1
        ASSERT((is_same<remove_volatile<int>::type, int>::value));
        ASSERT((is_same<remove_volatile<int *>::type, int *>::value));
        ASSERT((is_same<remove_volatile<TestType>::type, TestType>::value));
        ASSERT((is_same<remove_volatile<int volatile *>::type,
                                        int volatile *>::value));
        ASSERT((is_same<remove_volatile<int volatile &>::type,
                                        int volatile &>::value));
        ASSERT((is_same<remove_volatile<int volatile()>::type,
                                        int volatile()>::value));
        ASSERT((is_same<remove_volatile<int[5]>::type,
                                        int[5]>::value));
        ASSERT((is_same<remove_volatile<int[5][2]>::type,
                                        int[5][2]>::value));
        ASSERT((is_same<remove_volatile<int[5][2][3]>::type,
                                        int[5][2][3]>::value));
        ASSERT((is_same<remove_volatile<int[]>::type,
                                        int[]>::value));
        ASSERT((is_same<remove_volatile<int[][2]>::type,
                                        int[][2]>::value));
        ASSERT((is_same<remove_volatile<int[][2][3]>::type,
                                        int[][2][3]>::value));

        ASSERT((is_same<remove_volatile<void>::type, void>::value));
        ASSERT((is_same<remove_volatile<const void>::type,
                                        const void>::value));

        // C-2
        ASSERT((is_same<remove_volatile<int volatile>::type, int>::value));
        ASSERT((is_same<remove_volatile<int * volatile>::type, int *>::value));
        ASSERT((is_same<remove_volatile<TestType volatile>::type,
                                                            TestType>::value));

        ASSERT((is_same<remove_volatile<int const volatile>::type,
                                                           int const>::value));
        ASSERT((is_same<remove_volatile<int * const volatile>::type,
                                                         int * const>::value));
        ASSERT((is_same<remove_volatile<TestType const volatile>::type,
                                                      TestType const>::value));

        ASSERT((is_same<remove_volatile<volatile int[5]>::type,
                                                 int[5]>::value));
        ASSERT((is_same<remove_volatile<volatile int[5][2]>::type,
                                                 int[5][2]>::value));
        ASSERT((is_same<remove_volatile<volatile int[5][2][3]>::type,
                                                 int[5][2][3]>::value));

        ASSERT((is_same<remove_volatile<volatile int[]>::type,
                                                 int[]>::value));
        ASSERT((is_same<remove_volatile<volatile int[][2]>::type,
                                                 int[][2]>::value));
        ASSERT((is_same<remove_volatile<volatile int[][2][3]>::type,
                                                 int[][2][3]>::value));

        ASSERT((is_same<remove_volatile<volatile void>::type,
                                                 void>::value));
        ASSERT((is_same<remove_volatile<const volatile void>::type,
                                        const          void>::value));
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
