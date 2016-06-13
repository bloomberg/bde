// bslmf_decay.t.cpp                  -*-C++-*-

#include "bslmf_decay.h"

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
// The component under test is a simple metafunction with a well-define set of
// input and output types.  This test driver consists of applying a sequence
// of simple test input types and verifying the correct output types.
//
//-----------------------------------------------------------------------------
// [1] COMPLETE TEST
// [2] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
//-----------------------------------------------------------------------------
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                                VERBOSITY
//-----------------------------------------------------------------------------

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0; // For test allocators

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

struct MyClass
{
    long m_value;
};

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //    The usage example from the component documentation compiles and
        //    runs correctly.
        //
        // Plan:
        //    Copy the usage example from the component documentation,
        //    replacing 'assert' with 'ASSERT'.
        //
        // Testing:
        //     Usage example
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        /// Usage Example 1
        /// - - - - - - - -
        // When an array is passed as a function argument, it decays to a
        // pointer to its element type. The result of 'bsl::decay' applied
        // to array types is, therefore, a pointer to the element type of the
        // array:
        //..
        typedef const int A1[];
        typedef double A2[3][4];
        ASSERT((bsl::is_same<const int*,   bsl::decay<A1>::type>::value));
        ASSERT((bsl::is_same<double(*)[4], bsl::decay<A2>::type>::value));
        //..
        //
        /// Usage Example 2
        /// - - - - - - - -
        // When an function is passed as a function argument, it decays to a
        // pointer to the function type. This transformation is reflected in
        // result of 'bsl::decay', when applied to function types
        //..
        typedef void F1(int);
        typedef int (&F2)();
        ASSERT((bsl::is_same<void (*)(int), bsl::decay<F1>::type>::value));
        ASSERT((bsl::is_same<int (*)(),     bsl::decay<F2>::type>::value));
        //..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // COMPLETE TEST
        //
        // Concerns:
        //: 1 If 'TYPE' is a scalar, class, or pointer type
        //:   'bsl::decay<TYPE>::type' is 'TYPE' except with top-level
        //:   cv-qualifiers removed.
        //: 2 If 'TYPE' is a one-dimentional array of unknown bound, 'U[]',
        //:   then 'bsl::decay<TYPE>::type' is 'U*'
        //: 3 If 'TYPE' is a one-dimentional array of known bound, 'U[N]',
        //:   then 'bsl::decay<TYPE>::type' is 'U*'
        //: 4 If 'TYPE' is a multi-dimentional array of unknown bound,
        //:   'U[][M]', then 'bsl::decay<TYPE>::type' is 'U(*)[M]'
        //: 5 If 'TYPE' is a multi-dimentional array of known bound,
        //:   'U[N][M]', then 'bsl::decay<TYPE>::type' is 'U(*)[M]'
        //: 6 Cv-qualification on array elements is preserved.
        //: 7 If 'TYPE' is a function type 'F', then 'bsl::decay<TYPE>::type'
        //:   is 'F*'.
        //: 8 if 'TYPE' is a pointer-to-function type or pointer-to-array type
        //:   then 'bsl::decay<TYPE>::type' is 'TYPE' except with top-level
        //:   cv-qualifiers removed (i.e., 'TYPE' is treated as a normal
        //:   pointer).
        //: 9 If 'TYPE&' is a reference type 'U&', then
        //:   'bsl::decay<TYPE>::type' is the same as 'bsl::decay<U>::type'.
        //
        // Plan:
        //    For each of the above concerns, instantiate
        //    'bsl::remove_extent<TYPE>' with an appropriate 'TYPE'. Use
        //    'bsl::is_same' to verify that 'bsl::remove_extent<TYPE>::type'
        //    is as expected.
	//
        // Testing:
        //     bsl::decay<TYPE>::type
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOMPLETE TEST"
                            "\n=============\n");

#define TEST(a,b) ASSERT((bsl::is_same<bsl::decay<a>::type, b>::value))

        //   TYPE                     decay<TYPE>::type
        //   =======================  ====================
        TEST(int                    , int                 );
        TEST(MyClass                , MyClass             );
        TEST(MyClass *              , MyClass *           );
        TEST(const int              , int                 );
        TEST(const MyClass *        , const MyClass *     );
        TEST(MyClass *volatile      , MyClass *           );
        TEST(short[]                , short *             );
        TEST(const MyClass[10]      , const MyClass *     );
        TEST(volatile char[][20]    , volatile char(*)[20]);
        TEST(long[10][30]           , long(*)[30]         );
        TEST(void (double)          , void (*)(double)    );
        TEST(short (* const)(int)   , short (*)(int)      );

        TEST(int&                   , int                 );
        TEST(MyClass&               , MyClass             );
        TEST(MyClass *&             , MyClass *           );
        TEST(const int&             , int                 );
        TEST(const MyClass *&       , const MyClass *     );
        TEST(MyClass *volatile &    , MyClass *           );
        TEST(short(&)[]             , short *             );
        TEST(const MyClass(&)[10]   , const MyClass *     );
        TEST(volatile char(&)[][20] , volatile char(*)[20]);
        TEST(long(&)[10][30]        , long(*)[30]         );
        TEST(void (&)(double)       , void (*)(double)    );
        TEST(short (* const&)(int)  , short (*)(int)      );

#undef TEST
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
// Copyright 2016 Bloomberg Finance L.P.
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
