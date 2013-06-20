// bsltf_evilbooleantype.t.cpp                                        -*-C++-*-
#include <bsltf_evilbooleantype.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace BloombergLP::bsltf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a type that should be usable in contexts
// where a boolean value is expected, but offers the minimal set of supported
// operations.  There are two kinds of tests we are interested in to validate
// this class:
//: o It correctly acts like a 'bool' in a variety of contexts.
//: o It supports only a restricted interface, and attempts to use many regular
//:   operations should fail to compile.
// The second set of tests are difficult to automate, as compilation failures
// will break the test driver.  Hence, these tests will be moved below the line
// as negative test numbers, explicitly enabled and run manually.
//-----------------------------------------------------------------------------
//*[  ] EvilBooleanType(bool value);
//*[  ] operator BoolResult() const;
//*[  ] EvilBooleanType operator!() const;
//*[  ] EvilBooleanType operator==(EvilBooleanType, EvilBooleanType);
//*[  ] EvilBooleanType operator!=(EvilBooleanType, EvilBooleanType);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//*[ 2] USAGE EXAMPLE
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test            = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;
    bool veryVeryVerbose = argc > 4;

    (void)veryVerbose;
    (void)veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
          if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: (TBD)
/// - - - - - - - - - - - - - - - -
// First, we ...
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        const EvilBooleanType falseValue1(false);
        const EvilBooleanType falseValue2(false);
        const EvilBooleanType falseValue3 = falseValue2;
        const EvilBooleanType trueValue1(true);
        const EvilBooleanType trueValue2(true);
        const EvilBooleanType trueValue3 = trueValue2;

        ASSERT(!(bool)falseValue1);
        ASSERT(!(bool)falseValue2);
        ASSERT(!(bool)falseValue3);
        ASSERT((bool)trueValue1);
        ASSERT((bool)trueValue2);
        ASSERT((bool)trueValue3);

        ASSERT(!falseValue1);
        ASSERT(!falseValue2);
        ASSERT(!falseValue3);
        ASSERT(trueValue1);
        ASSERT(trueValue2);
        ASSERT(trueValue3);

        ASSERT(falseValue1 == falseValue1);
        ASSERT(falseValue1 == falseValue2);
        ASSERT(falseValue1 == falseValue3);

        ASSERT(falseValue2 == falseValue1);
        ASSERT(falseValue2 == falseValue2);
        ASSERT(falseValue2 == falseValue3);

        ASSERT(falseValue3 == falseValue1);
        ASSERT(falseValue3 == falseValue2);
        ASSERT(falseValue3 == falseValue3);

        ASSERT(trueValue1 == trueValue1);
        ASSERT(trueValue1 == trueValue2);
        ASSERT(trueValue1 == trueValue3);

        ASSERT(trueValue2 == trueValue1);
        ASSERT(trueValue2 == trueValue2);
        ASSERT(trueValue2 == trueValue3);

        ASSERT(trueValue3 == trueValue1);
        ASSERT(trueValue3 == trueValue2);
        ASSERT(trueValue3 == trueValue3);

        ASSERT(falseValue1 != trueValue1);
        ASSERT(falseValue1 != trueValue2);
        ASSERT(falseValue1 != trueValue3);

        ASSERT(falseValue2 != trueValue1);
        ASSERT(falseValue2 != trueValue2);
        ASSERT(falseValue2 != trueValue3);

        ASSERT(falseValue3 != trueValue1);
        ASSERT(falseValue3 != trueValue2);
        ASSERT(falseValue3 != trueValue3);

        ASSERT(trueValue1 != falseValue1);
        ASSERT(trueValue1 != falseValue2);
        ASSERT(trueValue1 != falseValue3);

        ASSERT(trueValue2 != falseValue1);
        ASSERT(trueValue2 != falseValue2);
        ASSERT(trueValue2 != falseValue3);

        ASSERT(trueValue3 != falseValue1);
        ASSERT(trueValue3 != falseValue2);
        ASSERT(trueValue3 != falseValue3);

        ASSERT(falseValue3 || trueValue3);
        ASSERT(trueValue3  || falseValue3);
        ASSERT(trueValue3  && trueValue3);

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
