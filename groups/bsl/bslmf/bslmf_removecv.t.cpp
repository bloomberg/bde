// bslmf_removecv.t.cpp                                               -*-C++-*-
#include <bslmf_removecv.h>

#include <bslmf_issame.h>

#include <cstdlib>
#include <cstdio>

using namespace bsl;
using namespace BloombergLP;

using std::printf;
using std::fprintf;
using std::atoi;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-functions, 'bsl::remove_cv', that
// removes any top-level cv-qualifiers from a template parameter type.  Thus,
// we need to ensure that the values returned by the meta-function is correct
// for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::remove_cv::type
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
///Example 1: Removing the CV-Qualifiers of a Type
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove the cv-qualifiers from a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified and
// 'volatile'-qualified type ('MyCvType') and the same type without the
// cv-qualifier ('MyType'):
//..
        typedef int                MyType;
        typedef const volatile int MyCvType;
//..
// Now, we remove the cv-qualifiers from 'MyCvType' using 'bsl::remove_cv' and
// verify that the resulting type is the same as 'MyType':
//..
        ASSERT(true == (bsl::is_same<bsl::remove_cv<MyCvType>::type,
                                                              MyType>::value));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::remove_cv::type'
        //   Ensure that the 'typedef' 'type' of 'bsl::remove_cv'
        //   instantiations has the same type as the template parameter type
        //   except has any top-level cv-qualifier removed.
        //
        // Concerns:
        //: 1 'bsl::remove_cv' leaves types that are not 'const'-qualified nor
        //:   'volatile'-qualified as-is.
        //:
        //: 2 'bsl::remove_const' remove any top-level cv-qualifiers.
        //
        // Plan:
        //   Verify that 'bsl::remove_cv::type' has the correct type for each
        //   concern.
        //
        // Testing:
        //   bsl::remove_cv::type
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::remove_cv::type'\n"
                            "\n======================\n");

        // C-1
        ASSERT((is_same<remove_cv<int>::type, int>::value));
        ASSERT((is_same<remove_cv<int *>::type, int *>::value));
        ASSERT((is_same<remove_cv<TestType>::type, TestType>::value));
        ASSERT((is_same<remove_volatile<int const volatile *>::type,
                                                int const volatile *>::value));
        // C-2
        ASSERT((is_same<remove_cv<int const>::type, int>::value));
        ASSERT((is_same<remove_cv<int * const>::type, int *>::value));
        ASSERT((is_same<remove_cv<TestType const>::type, TestType>::value));

        ASSERT((is_same<remove_cv<int volatile>::type, int>::value));
        ASSERT((is_same<remove_cv<int * volatile>::type, int *>::value));
        ASSERT((is_same<remove_cv<TestType volatile>::type, TestType>::value));

        ASSERT((is_same<remove_cv<int const volatile>::type, int>::value));
        ASSERT((is_same<remove_cv<int * const volatile>::type, int *>::value));
        ASSERT((is_same<remove_cv<TestType const volatile>::type,
                                                            TestType>::value));
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
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
