// bslmf_addpointer.t.cpp                                             -*-C++-*-
#include <bslmf_addpointer.h>

#include <bslmf_issame.h>  // for testing only

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace bsl;
using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, 'bsl::add_pointer', that
// transforms a type to a pointer type to that type.  We need to ensure that
// the values returned by the meta-function are correct for each possible
// category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC TYPE
// [ 1] bsl::add_pointer::type
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
   // argument for a template parameter.
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
///Example 1: Transform Type to Pointer Type to that Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform a type to a pointer type to that type.
//
// First, we create two 'typedef's -- a pointer type ('MyPtrType') and the type
// pointed to by the pointer type ('MyType'):
//..
    typedef int   MyType;
    typedef int * MyPtrType;
//..
// Now, we transform 'MyType' to a pointer type to 'MyType' using
// 'bsl::add_pointer' and verify that the resulting type is the same as
// 'MyPtrType':
//..
    ASSERT((bsl::is_same<bsl::add_pointer<MyType>::type,
                         MyPtrType>::value));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::add_pointer::type'
        //   Ensure that the 'typedef' 'type' of 'bsl::add_pointer' has the
        //   correct type for a variety of template parameter types.
        //
        // Concerns:
        //: 1 'bsl::add_pointer' transforms a non-reference type to a pointer
        //:   type pointing to the original type.
        //:
        //: 2 'bsl::add_pointer' transforms a reference type to a pointer
        //:   type pointing to the type referred to by the reference type.
        //
        // Plan:
        //   Verify that 'bsl::add_pointer::type' has the correct type for
        //   each concern.
        //
        // Testing:
        //   bsl::add_pointer::type
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::add_pointer::type'\n"
                            "\n========================\n");

        // C-1
        ASSERT((is_same<add_pointer<int>::type, int *>::value));
        ASSERT((is_same<add_pointer<int const>::type, int const *>::value));
        ASSERT((is_same<add_pointer<int *>::type, int **>::value));
        ASSERT((is_same<add_pointer<TestType>::type, TestType *>::value));

        // C-2
        ASSERT((is_same<add_pointer<int &>::type, int *>::value));
        ASSERT((is_same<add_pointer<int const &>::type, int const *>::value));
        ASSERT((is_same<add_pointer<TestType &>::type, TestType *>::value));

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT((is_same<add_pointer<int &&>::type, int *>::value));
        ASSERT((is_same<add_pointer<int const &&>::type, int const *>::value));
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
