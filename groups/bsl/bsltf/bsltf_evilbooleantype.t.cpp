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
// CREATORS
// [ 2] EvilBooleanType(bool value);
// [ 7] EvilBooleanType(const EvilBooleanType& original);
//
// ACCESSORS
// [ 3] operator BoolResult() const;
// [ 3] EvilBooleanType operator!() const;
//
// FREE OPERATORS
// [ 5] EvilBooleanType operator==(EvilBooleanType, EvilBooleanType);
// [ 5] EvilBooleanType operator!=(EvilBooleanType, EvilBooleanType);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE
// [ 8] NOT IMPLEMENTED OPERATORS

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

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsltf::EvilBooleanType Obj;

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
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'bsltf::EvilBooleanType'.
//
// First, we create an object 'trueValue' and initialize it with the 'true'
// value:
//..
    bsltf::EvilBooleanType trueValue(true);
//..
// Now, we can use it for if-else conditions or another constructions, that
// require boolen value:
//..
    if (trueValue) {
        ASSERT(trueValue);
    }
//..
// Finally we create another object, having the opposite value, and verify it:
//..
    bsltf::EvilBooleanType falseValue = !trueValue;
    ASSERT(false == (bool)falseValue);
//..

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // NOT IMPLEMENTED OPERATORS
        //
        // Concerns:
        //: 1 The copy-assignment operator is declared private.
        //:
        //: 2 The address-of operator is declared private.
        //:
        //: 3 The comma operator is declared private.
        //
        // Plan:
        //: 1 There is no good way to test the operators access privilege in
        //:   C++03.  SFINAE doesn't support check for private-access.
        //
        // Testing:
        //   void operator=(const EvilBooleanType&);
        //   void operator&() const;
        //   void operator,(T&) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nNOT IMPLEMENTED OPERATORS"
                            "\n=========================\n");

        Obj       X(true);
        const Obj Y(false);

        // X = Y;  // This will not compile
        // &X;     // This will not compile
        // &Y;     // This will not compile
        // (X,X);  // This will not compile
        // (X,Y);  // This will not compile
        // (Y,X);  // This will not compile
        // (Y,Y);  // This will not compile
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //
        // Plan:
        //: 1 Use the value constructor to create two 'const' objects 'X' and
        //:   'Y', both having the same value.
        //:
        //: 2 Use the copy constructor to create an object 'Z' from 'X'.  (C-2)
        //:
        //: 3 Use the equality-comparison operator to verify that:
        //:
        //:   1 'Z' has the same value as that of 'X'.  (C-1)
        //:
        //:   2 'X' still has the same value as that of 'Y'.  (C-3)
        //
        // Testing:
        //   EvilBooleanType(const EvilBooleanType& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTOR"
                            "\n================\n");
        {
            const Obj X(false);
            const Obj Y(false);

            const Obj Z = X;

            ASSERT(X == Z);
            ASSERT(Y == X);
        }

        {
            const Obj X(true);
            const Obj Y(true);

            const Obj Z = X;

            ASSERT(X == Z);
            ASSERT(Y == X);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if they have
        //:   the same value.
        //:
        //: 2 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 3 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //
        // Plan:
        //: 1 Specify a set S whose elements have different values.  Compare
        //:   each pair (X, Y) in the cross product S X S and verify comparison
        //:   result.  (C-1..3)
        //
        // Testing:
        //   EvilBooleanType operator==(lhs, rhs);
        //   EvilBooleanType operator!=(lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                            "\n=============================\n");

        {
            const EvilBooleanType falseValue1(false);
            const EvilBooleanType falseValue2(false);
            const EvilBooleanType trueValue1(true);
            const EvilBooleanType trueValue2(true);


            ASSERT(falseValue1 == falseValue1);
            ASSERT(falseValue1 == falseValue2);
            ASSERT(falseValue2 == falseValue1);
            ASSERT(falseValue2 == falseValue2);

            ASSERT(trueValue1 == trueValue1);
            ASSERT(trueValue1 == trueValue2);
            ASSERT(trueValue2 == trueValue1);
            ASSERT(trueValue2 == trueValue2);

            ASSERT(!(falseValue1 == trueValue1));
            ASSERT(!(falseValue1 == trueValue2));
            ASSERT(!(falseValue2 == trueValue1));
            ASSERT(!(falseValue2 == trueValue2));

            ASSERT(!(trueValue1 == falseValue1));
            ASSERT(!(trueValue1 == falseValue2));
            ASSERT(!(trueValue2 == falseValue1));
            ASSERT(!(trueValue2 == falseValue2));

            ASSERT(falseValue1 != trueValue1);
            ASSERT(falseValue1 != trueValue2);
            ASSERT(falseValue2 != trueValue1);
            ASSERT(falseValue2 != trueValue2);

            ASSERT(trueValue1 != falseValue1);
            ASSERT(trueValue1 != falseValue2);
            ASSERT(trueValue2 != falseValue1);
            ASSERT(trueValue2 != falseValue2);

            ASSERT(!(falseValue1 != falseValue1));
            ASSERT(!(falseValue1 != falseValue2));
            ASSERT(!(falseValue2 != falseValue1));
            ASSERT(!(falseValue2 != falseValue2));

            ASSERT(!(trueValue1 != trueValue1));
            ASSERT(!(trueValue1 != trueValue2));
            ASSERT(!(trueValue2 != trueValue1));
            ASSERT(!(trueValue2 != trueValue2));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure basic accessors properly interprets object state.
        //
        // Concerns:
        //: 1 The 'operator!' returns the newly created object having the
        //:   logically negated value of the object.
        //:
        //: 2 The type conversion operator returns object's value, that can be
        //:   converted to boolean.
        //
        // Plan:
        //: 1 Create couple objects, having different values.
        //:
        //: 2 Use logical negation operator to create inverted objects.  (C-1)
        //:
        //: 3 Use type coversion operator to check values of all created
        //:   objects.  (C-2)
        //
        // Testing:
        //   EvilBooleanType operator!() const;
        //   operator BoolResult() const;
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBASIC ACCESSORS"
                   "\n===============\n");

        const Obj trueValue(true);
        const Obj notTrueValue = !trueValue;

        const Obj falseValue(false);
        const Obj notFalseValue = !falseValue;

        ASSERT(true  == (bool)trueValue);
        ASSERT(false == (bool)notTrueValue);
        ASSERT(false == (bool)falseValue);
        ASSERT(true  == (bool)notFalseValue);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //   Ensure that we can use constructor to create an object.
        //
        // Concerns:
        //: 1 An object created with the constructor has the specified value.
        //
        // Plan:
        //: 1 Using the value constructor create couple objects, having
        //:   different values.  Use the (untested) basic accessors to check
        //:   the value of the object.  (C-1)
        //
        // Testing:
        //   EvilBooleanType(bool value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nPRIMARY MANIPULATORS"
                            "\n====================\n");

        Obj       falseValue(false);
        const Obj trueValue(true);

        ASSERT(false == (bool)falseValue);
        ASSERT(true  == (bool)trueValue);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
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
