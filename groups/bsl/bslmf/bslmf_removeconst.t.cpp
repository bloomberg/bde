// bslmf_removeconst.t.cpp                                            -*-C++-*-
#include <bslmf_removeconst.h>

#include <bslmf_issame.h>

#include <cstdlib>
#include <cstdio>

using namespace bsl;
using namespace BloombergLP;

using std::atoi;
using std::printf;
using std::fprintf;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-functions, 'bsl::remove_const', that
// removes any top-level 'const'-qualifier from a template parameter type.
// Thus, we need to ensure that the values returned by the meta-function is
// correct for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::remove_const::type
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
   // argument for the template parameter 'TYPE' of 'bsl::remove_const'.
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
///Example 1: Removing The 'const'-qualifier of A Type
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove any top-level 'const'-qualifier from a
// particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified type ('MyConstType')
// and the same type without the 'const'-qualifier ('MyType'):
//..
        typedef int       MyType;
        typedef const int MyConstType;
//..
// Now, we remove the 'const'-qualifier from 'MyConstType' using
// 'bsl::remove_const' and verify that the resulting type is the same as
// 'MyType':
//..
        ASSERT(true == (bsl::is_same<bsl::remove_const<MyConstType>::type,
                                                              MyType>::value));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::remove_const::type'
        //   Ensure that the 'typedef' 'type' of 'bsl::remove_const' has the
        //   correct type for a variety of template parameter types.
        //
        // Concerns:
        //: 1 'bsl::remove_const' leaves types that are not 'const'-qualified
        //:   at the top-level as-is.
        //:
        //: 2 'bsl::remove_const' remove any top-level 'const'-qualifier.
        //
        // Plan:
        //   Verify that 'bsl::remove_const::type' has the correct type for
        //   each concern.
        //
        // Testing:
        //   bsl::remove_const::type
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::remove_const::type'\n"
                            "\n=========================\n");

        // C-1
        ASSERT((is_same<remove_const<int>::type, int>::value));
        ASSERT((is_same<remove_const<int *>::type, int *>::value));
        ASSERT((is_same<remove_const<TestType>::type, TestType>::value));
        ASSERT((is_same<remove_const<int const *>::type, int const *>::value));
        ASSERT((is_same<remove_const<int const &>::type, int const &>::value));
        ASSERT((is_same<remove_const<int const()>::type, int const()>::value));
        ASSERT((is_same<remove_const<int[5]>::type, int[5]>::value));
        ASSERT((is_same<remove_const<int[5][2]>::type, int[5][2]>::value));
        ASSERT((is_same<remove_const<int[5][2][3]>::type,
                                     int[5][2][3]>::value));
        ASSERT((is_same<remove_const<int[]>::type, int[]>::value));
        ASSERT((is_same<remove_const<int[][2]>::type, int[][2]>::value));
        ASSERT((is_same<remove_const<int[][2][3]>::type, int[][2][3]>::value));

        ASSERT((is_same<remove_const<void>::type, void>::value));
        ASSERT((is_same<remove_const<volatile void>::type,
                                     volatile void>::value));

        // C-2
        ASSERT((is_same<remove_const<int const>::type, int>::value));
        ASSERT((is_same<remove_const<int * const>::type, int *>::value));
        ASSERT((is_same<remove_const<TestType const>::type, TestType>::value));

        ASSERT((is_same<remove_const<int const volatile>::type,
                                                        int volatile>::value));
        ASSERT((is_same<remove_const<int * const volatile>::type,
                                                      int * volatile>::value));
        ASSERT((is_same<remove_const<TestType const volatile>::type,
                                                   TestType volatile>::value));

        ASSERT((is_same<remove_const<const int[5]>::type,
                                           int[5]>::value));
        ASSERT((is_same<remove_const<const int[5][2]>::type,
                                           int[5][2]>::value));
        ASSERT((is_same<remove_const<const int[5][2][3]>::type,
                                           int[5][2][3]>::value));

        ASSERT((is_same<remove_const<const int[]>::type,
                                           int[]>::value));
        ASSERT((is_same<remove_const<const int[][2]>::type,
                                           int[][2]>::value));
        ASSERT((is_same<remove_const<const int[][2][3]>::type,
                                           int[][2][3]>::value));

        ASSERT((is_same<remove_const<const void>::type,
                                           void>::value));
        ASSERT((is_same<remove_const<const volatile void>::type,
                                           volatile void>::value));
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
