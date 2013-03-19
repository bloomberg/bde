// bslmf_assert.t.cpp                                                 -*-C++-*-
#include <bslmf_assert.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <stdio.h>   // printf
#include <stdlib.h>  // atoi

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] BSLMF_ASSERT(expr)i
//
// ----------------------------------------------------------------------------
// [  ] USAGE EXAMPLE

//=============================================================================
//
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

// namespace scope

BSLMF_ASSERT(sizeof(int) >= sizeof(char));

// un-named namespace
namespace {
    BSLMF_ASSERT(1);
#if defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT(1);
#else
    BSLMF_ASSERT(1); BSLMF_ASSERT(1); // not class scope
#endif
}

namespace {
    BSLMF_ASSERT(1);
}

namespace Bar {

    BSLMF_ASSERT(1);
#if defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT(1);
#else
    BSLMF_ASSERT(1); BSLMF_ASSERT(1); // not class scope
#endif
}

class MyType {
    // class scope

    BSLMF_ASSERT(sizeof(int) >= sizeof(char));
    BSLMF_ASSERT(sizeof(int) >= sizeof(char));

  public:
    int d_data;
    void foo();
};

void MyType::foo()
{
    // function scope

    BSLMF_ASSERT(sizeof(int) >= sizeof(char));
    BSLMF_ASSERT(1);

    using namespace BloombergLP;
    BSLMF_ASSERT(1);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    // bool veryVerbose = argc > 3;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TESTING BSLMF_ASSERT MACRO
        // We have the following concerns:
        //   1. Works for all non-zero integral values.
        //   2. Works in and out of class scope.
        //
        // Plan:
        //   Invoke the macro at namespace, class, and function scope (above)
        //   and verify that it does not cause a compiler error.  Please see
        //   the 'ttt' test package group for test cases where the macro
        //   should cause a compile-time error.
        //
        // Testing:
        //   BSLMF_ASSERT(expr)
        // --------------------------------------------------------------------

        if (verbose) printf("\nBSLMF_ASSERT Macro\n"
                            "\n==================\n");

        BSLMF_ASSERT(sizeof(int) >= sizeof(char));
        BSLMF_ASSERT(sizeof(int) >= sizeof(char));
        BSLMF_ASSERT(1);  ASSERT(145 == __LINE__);
        BSLMF_ASSERT(1);  ASSERT(146 == __LINE__);
        BSLMF_ASSERT(1 > 0 && 1);

// MSVC: __LINE__ macro breaks when /ZI is used (see Q199057 or KB199057)
// SUN:  BSLMF_ASSERT is defined the way that breaks this test
#if !defined(BSLS_PLATFORM_CMP_MSVC) &&                    \
    !defined(BSLS_PLATFORM_CMP_SUN)  &&                    \
    !defined(BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT)
        bslmf_Assert_145 t1; // test typedef name creation; matches above line
        bslmf_Assert_146 t2; // test typedef name creation; matches above line
        ASSERT(sizeof t1 == sizeof t2);  // use t1 and t2
#endif

        BSLMF_ASSERT(2);
        BSLMF_ASSERT(-1);

#if defined(BSLS_PLATFORM_CMP_SUN)
        BSLMF_ASSERT(1);
#else
        BSLMF_ASSERT(1); BSLMF_ASSERT(1); // not class scope
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
// Copyright (C) 2013 Bloomberg L.P.
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
