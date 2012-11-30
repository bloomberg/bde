// bslmf_isintegral.t.cpp                                             -*-C++-*-
#include <bslmf_isintegral.h>

#include <bsls_bsltestutil.h>

#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace bsl;
using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test implements a meta-functions, 'bsl::is_integral',
// that determine whether a template parameter type is a integral type.  Thus,
// we need to ensure that the values returned by the meta-function is correct
// for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_integral::value
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
   // argument for the template parameter 'TYPE' of 'bsl::is_integral'.
};

}  // close unnamed namespace

#define TYPE_ASSERT_CVQ(metaFunc, member, type, result)                       \
    ASSERT(result == metaFunc<type>::member);                                 \
    ASSERT(result == metaFunc<type const>::member);                           \
    ASSERT(result == metaFunc<type volatile>::member);                        \
    ASSERT(result == metaFunc<type const volatile>::member);

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

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
///Example 1: Verify Integral Types
///- - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a integral type.
//
// First, we create two 'typedef's -- a integral type and a non-integral type:
//..
        typedef void MyType;
        typedef int  MyIntegralType;
//..
// Now, we instantiate the 'bsl::is_integral' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
        ASSERT(false == bsl::is_integral<MyType>::value);
        ASSERT(true == bsl::is_integral<MyIntegralType>::value);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_integral::value'
        //   Ensure that 'bsl::is_integral' returns the correct values for a
        //   variety of a variety of template parameter types.
        //
        // Concerns:
        //: 1 'is_integral::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) non-integral primitve type.
        //
        //: 2 'is_integral::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'is_integral::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) integral type.
        //
        // Plan:
        //   Verify that 'bsl::is_integral::value' has the correct value for
        //   each concern.
        //
        // Testing:
        //   bsl::is_integral::value
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::is_integral::value\n"
                            "\n========================\n");

        // C-1
        TYPE_ASSERT_CVQ(bsl::is_integral, value, double, false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, long double, false);

        // C-2
        TYPE_ASSERT_CVQ(bsl::is_integral, value, TestType, false);

        // C-3
        TYPE_ASSERT_CVQ(bsl::is_integral, value, bool, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, char, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, wchar_t, true);

        TYPE_ASSERT_CVQ(bsl::is_integral, value, signed char, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, short, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, int, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, long int, true);

        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned char, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned short, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned int, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned long int, true);

        TYPE_ASSERT_CVQ(bsl::is_integral, value, bsls::Types::Int64, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, bsls::Types::Uint64, true);
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
