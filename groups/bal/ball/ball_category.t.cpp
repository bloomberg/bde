// ball_category.t.cpp                                                -*-C++-*-
#include <ball_category.h>

#include <ball_severity.h>                                  // for testing only

#include <bdls_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// TBD
//
// Primary Manipulators:
//: o TBD
//
// Basic Accessors:
//: o TBD
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR/MANIPULATOR/OPERATOR ptr./ref. parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// CREATORS
// [  ] Category();
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT

#define Q            BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P            BDLS_TESTUTIL_P   // Print identifier and value.
#define P_           BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BDLS_TESTUTIL_L_  // current Line number

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::Category Obj;

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------



//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);

    switch (test) { case 0:
      case 2: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Example 1: Basic Use of 'ball::Category'
/// - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates creating a category and accessing its
// threshold information.
//
// Note that other components in the logging subsystem provide more user
// focused examples of using categories (see {'ball_loggermanager'},
// {'ball_administration'}, and 'ball_categorymanager'}).
//
// First we create a simple category, 'example', that has the record-level,
// trigger-level, and trigger-all thresholds set to OFF and the pass-level set
// to WARN, and verify these values:
//..
    ball::Category example("example",
                           ball::Severity::e_OFF,
                           ball::Severity::e_WARN,
                           ball::Severity::e_OFF,
                           ball::Severity::e_OFF);
//
    ASSERT(0 == bsl::strcmp("example", example.categoryName()));
    ASSERT(ball::Severity::e_OFF  == example.recordLevel());
    ASSERT(ball::Severity::e_WARN == example.passLevel());
    ASSERT(ball::Severity::e_OFF  == example.triggerLevel());
    ASSERT(ball::Severity::e_OFF  == example.triggerAllLevel());
//..
// See {'ball_loggermanager'} for more information on the use of various
// thresholds levels.
//
// Finally, we test if a the category is enabled for log record recorded with
// 'e_ERROR' severity:
//..
    if (example.isEnabled(ball::Severity::e_ERROR)) {
        // publish record
    }
//..
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
        //: 1 Create an object 'w' (default ctor).      { w:0             }
        //: 2 Create an object 'x' (copy from 'w').     { w:0 x:0         }
        //: 3 Set 'x' to 'A' (value distinct from 0).   { w:0 x:A         }
        //: 4 Create an object 'y' (init. to 'A').      { w:0 x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').     { w:0 x:A y:A z:A }
        //: 6 Set 'z' to 0 (the default value).         { w:0 x:A y:A z:0 }
        //: 7 Assign 'w' from 'x'.                      { w:A x:A y:A z:0 }
        //: 8 Assign 'w' from 'z'.                      { w:0 x:A y:A z:0 }
        //: 9 Assign 'x' from 'x' (aliasing).           { w:0 x:A y:A z:0 }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

/*
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // This test intentionally uses the default allocator.

        bslma::TestAllocator testAllocator("breathing", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&testAllocator);

        const Type::Enum        TYPEA = Type::e_STRING;
        const bsl::string       VALUEA("foo");

        const bdlt::DatetimeTz  DATE(bdlt::Datetime(1999,1,1), 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:0             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ P(W) }

        ASSERT(Type::e_VOID   == W.type());

        if (veryVerbose) cout <<
                          "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:0 x:0         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout <<
                                "\ta. Check the initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ P(X) }

        ASSERT(Type::e_VOID   == W.type());

        if (veryVerbose) cout <<
                     "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 0)."
                             "\t\t{ w:0 x:A         }" << endl;

        mX.setString(VALUEA);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ P(X) }

        ASSERT(TYPEA  == X.type());
        ASSERT(VALUEA == X.theString());

        if (veryVerbose) cout <<
                     "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' and set to 'A'."
                             "\t\t{ w:0 x:A y:A     }" << endl;

        Obj mY;  const Obj& Y = mY;
        mY.setString(VALUEA);

        if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ P(Y) }

        ASSERT(TYPEA  == Y.type());
        ASSERT(VALUEA == Y.theString());

        if (veryVerbose) cout <<
                "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'." << endl;

        ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
                             "\t\t{ w:0 x:A y:A z:A }" << endl;

        Obj mZ(Y);  const Obj& Z = mZ;

        if (veryVerbose) cout << "\ta. Check initial value of 'z'." << endl;
        if (veryVeryVerbose) { T_ P(Z) }

        ASSERT(TYPEA  == Z.type());
        ASSERT(VALUEA == Z.theString());


        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 0 (the default value)."
                             "\t\t\t{ w:0 x:A y:A z:0 }" << endl;

        mZ.reset();

        if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ P(Z) }

        ASSERT(Type::e_VOID   == Z.type());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign 'w' from 'x'."
                             "\t\t\t\t{ w:A x:A y:A z:0 }" << endl;

        mW = X;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ P(W) }

        ASSERT(TYPEA  == W.type());
        ASSERT(VALUEA == W.theString());


        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
        ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign 'w' from 'z'."
                             "\t\t\t\t{ w:0 x:A y:A z:0 }" << endl;

        mW = Z;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ P(W) }

        ASSERT(Type::e_VOID   == Z.type());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
        ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
        ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign 'x' from 'x' (aliasing)."
                             "\t\t\t{ w:0 x:A y:A z:0 }" << endl;

        mX = X;

        if (veryVerbose) cout << "\ta. Check (same) value of 'x'." << endl;
        if (veryVeryVerbose) { T_ P(X) }

        ASSERT(TYPEA  == X.type());
        ASSERT(VALUEA == X.theString());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));
*/
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the default allocator.

    LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
