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
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

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
