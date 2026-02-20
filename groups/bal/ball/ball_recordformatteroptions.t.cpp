// ball_recordformatteroptions.t.cpp                                 -*-C++-*-

#include <ball_recordformatteroptions.h>

#include <ball_recordformattertimezone.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_cstddef.h>  // for 'bsl::size_t'

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single, simply constrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: `setAttributeName` and `attributeName`.
//
// Primary Manipulators:
//  - `setTimezoneDefault`
//
// Basic Accessors:
//  - `timezoneDefault`
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, `gg`, normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will test the value constructor in case 3 (in lieu of
// the generator function), with the default constructor and primary
// manipulators tested fully in case 2.
//
// Certain standard value-semantic-type test cases are omitted:
//  - [ 5] -- Print and output operator not implemented.
//  - [ 7] -- Copy constructor is compiler-generated.
//  - [ 8] -- Swap is not applicable (no allocator).
//  - [ 9] -- Copy-assignment is compiler-generated.
//  - [10] -- BSLX streaming is not implemented for this class.
//  - [11] -- Move operations are compiler-generated.
//  - [12] -- No class methods to test.
//
// Global Concerns:
//  - ACCESSOR methods are declared `const`.
//  - CREATOR & MANIPULATOR pointer/reference parameters are declared `const`.
//
// Global Assumptions:
//  - ACCESSOR methods are `const` thread-safe.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] RecordFormatterOptions();
// [ 3] RecordFormatterOptions(RecordFormatterTimezone::Enum tz);
//
// MANIPULATORS
// [ 2] setTimezoneDefault(RecordFormatterTimezone::Enum value);
//
// ACCESSORS
// [ 4] RecordFormatterTimezone::Enum timezoneDefault() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const RecordFormatterOptions& lhs, rhs);
// [ 6] bool operator!=(const RecordFormatterOptions& lhs, rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [13] USAGE EXAMPLE
// [ *] CONCERN: All accessor methods are declared `const`.
// [ 3] CONCERN: All creator/manipulator ptr./ref. parameters are `const`.

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

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

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
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef ball::RecordFormatterOptions     Obj;
typedef ball::RecordFormatterTimezone    Tz;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA used by test cases 3 and 4.

struct DefaultDataRow {
    int      d_line;                 // source line number
    Tz::Enum d_timezoneDefault;
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE  TIMEZONE
    //----  --------

    // default (must be first)
    { L_,   Tz::e_UTC   },

    // other values
    { L_,   Tz::e_LOCAL },
};
static const bsl::size_t DEFAULT_NUM_DATA =
                                    sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;  (void)veryVeryVerbose;
    bool veryVeryVeryVerbose = argc > 5;  (void)veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 13: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

///Example 1: Configuring Formatter Options
///- - - - - - - - - - - - - - - - - - - - -
// Suppose we are configuring a log record formatter and want to specify
// options for how timestamps should be rendered.  We can use
// `ball::RecordFormatterOptions` to specify these configuration values.
//
// First, we create a `RecordFormatterOptions` object with default settings,
// which uses UTC timezone:
// ```
   ball::RecordFormatterOptions options;
   ASSERT(ball::RecordFormatterTimezone::e_UTC == options.timezoneDefault());
// ```
// Then, if we want to configure the formatter to use local time instead, we
// can either create the options object with the desired timezone:
// ```
   ball::RecordFormatterOptions localOptions(
                                      ball::RecordFormatterTimezone::e_LOCAL);
   ASSERT(ball::RecordFormatterTimezone::e_LOCAL ==
                                              localOptions.timezoneDefault());
// ```
// Or we can modify an existing options object:
// ```
   options.setTimezoneDefault(ball::RecordFormatterTimezone::e_LOCAL);
   ASSERT(ball::RecordFormatterTimezone::e_LOCAL == options.timezoneDefault());
// ```
// Finally, we can pass these options to a formatter configuration function
// (such as `ball::RecordJsonFormatter::loadJsonSchemeFormatter`) to configure
// how timestamps are rendered in the log output.

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CLASS METHOD `isValidUtcOffsetInSeconds`
        //   N/A
        //
        // Testing:
        //   Reserved for testing class methods.
        // --------------------------------------------------------------------

        if (verbose) cout << "CLASS METHOD `isValidUtcOffsetInSeconds`\n"
                             "========================================\n";

        if (verbose) cout << "Not implemented for this component.\n";

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // MOVE-ASSIGNMENT OPERATOR
        //   Compiler-generated move assignment operator is sufficient.
        //
        // Testing:
        //   Reserved for move-assignment operator.
        // --------------------------------------------------------------------

        if (verbose) cout << "MOVE-ASSIGNMENT OPERATOR\n"
                             "========================\n";

        if (verbose) cout << "Compiler-generated is sufficient.\n";

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX STREAMING
        //   N/A
        //
        // Testing:
        //   Reserved for `bslx` streaming.
        // --------------------------------------------------------------------

        if (verbose) cout << "BSLX STREAMING\n"
                             "==============\n";

        if (verbose) cout << "Not implemented for this component.\n";

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Compiler-generated copy assignment operator is sufficient.
        //
        // Testing:
        //   Reserved for copy-assignment operator.
        // --------------------------------------------------------------------

        if (verbose) cout << "COPY-ASSIGNMENT OPERATOR\n"
                             "========================\n";

        if (verbose) cout << "Compiler-generated is sufficient.\n";

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   N/A - no allocator.
        //
        // Testing:
        //   Reserved for `swap` testing.
        // --------------------------------------------------------------------

        if (verbose) cout << "SWAP MEMBER AND FREE FUNCTIONS\n"
                             "==============================\n";

        if (verbose) cout << "Not applicable (no allocator).\n";

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Compiler-generated copy constructor is sufficient.
        //
        // Testing:
        //   Reserved for copy constructor.
        // --------------------------------------------------------------------

        if (verbose) cout << "COPY CONSTRUCTOR\n"
                             "================\n";

        if (verbose) cout << "Compiler-generated is sufficient.\n";

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   N/A - no print or output operator defined.
        //
        // Testing:
        //   Reserved for `print` and output operator (`<<`).
        // --------------------------------------------------------------------

        if (verbose) cout << "PRINT AND OUTPUT OPERATOR\n"
                             "=========================\n";

        if (verbose) cout << "Not implemented for this component.\n";

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        // 1. Each accessor returns the value of the corresponding attribute
        //    of the object.
        //
        // 2. Each accessor method is declared `const`.
        //
        // Plan:
        // 1. Use the value constructor to create an object having the default
        //    attribute values.  Verify that the accessor for the
        //    `timezoneDefault` attribute invoked on a reference providing
        //    non-modifiable access to the object returns the expected value.
        //    (C-1..2)
        //
        // 2. For each row 'R' in a table of diverse object values: (C-1..2)
        //    1. Use the value constructor to create a `const` object having
        //       the value from 'R'.
        //    2. Verify that the accessor returns the expected value.
        //
        // Testing:
        //   RecordFormatterTimezone::Enum timezoneDefault() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "BASIC ACCESSORS\n"
                             "===============\n";

        if (verbose)
             cout << "\tUse the value constructor to create default object.\n";
        {
            const Obj X(Tz::e_UTC);
            ASSERT(Tz::e_UTC == X.timezoneDefault());
        }

        if (verbose) cout << "\tThoroughly test the basic accessors.\n";
        {
            for (bsl::size_t ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
                const int      LINE = DEFAULT_DATA[ti].d_line;
                const Tz::Enum TZ   = DEFAULT_DATA[ti].d_timezoneDefault;

                const Obj X(TZ);

                if (veryVerbose) { T_ P_(LINE) P(TZ) }

                ASSERTV(LINE, TZ == X.timezoneDefault());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that `==` and `!=` are the operational definition of value.
        //
        // Concerns:
        // 1. Two objects, `X` and `Y`, compare equal if and only if their
        //    corresponding `timezoneDefault` attributes compare equal.
        //
        // 2. `true  == (X == X)`  (i.e., identity)
        //
        // 3. `false == (X != X)`  (i.e., identity)
        //
        // 4. `X == Y` if and only if `Y == X`  (i.e., commutativity)
        //
        // 5. `X != Y` if and only if `Y != X`  (i.e., commutativity)
        //
        // 6. `X != Y` if and only if `!(X == Y)`
        //
        // 7. Comparison is symmetric with respect to user-defined conversion
        //    (i.e., both comparison operators are free functions).
        //
        // 8. Non-modifiable objects can be compared (i.e., `const` objects and
        //    `const` references).
        //
        // 9. The equality operator's signature and return type are standard.
        //
        // 10. The inequality operator's signature and return type are standard.
        //
        // Plan:
        // 1. Use the respective addresses of `operator==` and `operator!=` to
        //    initialize function pointers having the appropriate signatures
        //    and return types for the two homogeneous, free equality-
        //    comparison operators defined in this component.  (C-7..10)
        //
        // 2. Using the table-driven technique, specify a set of distinct
        //    object values (one per row) in terms of their individual salient
        //    attributes.
        //
        // 3. For each row `R1` in the table of P-2:  (C-1..6)
        //
        //   1. Create a single object, and use it to verify the reflexive
        //      (anti-reflexive) property of equality (inequality) in the
        //      presence of aliasing.  (C-2..3)
        //
        //   2. For each row `R2` in the table of P-2:  (C-1, 4..6)
        //
        //     1. Record, in `EXP`, whether or not distinct objects created
        //        from `R1` and `R2`, respectively, are expected to have the
        //        same value.
        //
        //     2. Create an object `X` having the value `R1`.
        //
        //     3. Create an object `Y` having the value `R2`.
        //
        //     4. Verify the commutativity property and expected return value
        //        for both `==` and `!=`.  (C-1, 4..6)
        //
        // Testing:
        //   bool operator==(const RecordFormatterOptions& lhs, rhs);
        //   bool operator!=(const RecordFormatterOptions& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "EQUALITY-COMPARISON OPERATORS\n"
                             "=============================\n";

        if (verbose)
            cout << "\tAssign the address of each operator to a variable.\n";
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = ball::operator==;
            operatorPtr operatorNe = ball::operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose) cout << "\tCompare every value with every value.\n";

        for (bsl::size_t ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
            const int      LINE1 = DEFAULT_DATA[ti].d_line;
            const Tz::Enum TZ1   = DEFAULT_DATA[ti].d_timezoneDefault;

            if (veryVerbose) { T_ P_(LINE1) P(TZ1) }

            // Ensure an object compares correctly with itself (alias test).

            {
                const Obj X(TZ1);

                ASSERTV(LINE1, TZ1,   X == X);
                ASSERTV(LINE1, TZ1, !(X != X));
            }

            for (bsl::size_t tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
                const int      LINE2 = DEFAULT_DATA[tj].d_line;
                const Tz::Enum TZ2   = DEFAULT_DATA[tj].d_timezoneDefault;

                if (veryVerbose) { T_ T_ P_(LINE2) P(TZ2) }

                const bool EXP = (ti == tj);  // expected result

                const Obj X(TZ1);
                const Obj Y(TZ2);

                if (veryVerbose) { T_ T_ T_ P_(EXP) P_(X.timezoneDefault())
                                                    P(Y.timezoneDefault()); }

                // Verify value and commutativity.

                ASSERTV(LINE1, TZ1, LINE2, TZ2,  EXP == (X == Y));
                ASSERTV(LINE1, TZ1, LINE2, TZ2,  EXP == (Y == X));

                ASSERTV(LINE1, TZ1, LINE2, TZ2, !EXP == (X != Y));
                ASSERTV(LINE1, TZ1, LINE2, TZ2, !EXP == (Y != X));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        // 1. The value constructor can create an object having any value that
        //    does not violate the constructor's documented preconditions.
        //
        // 2. Any argument can be `const`.
        //
        // Plan:
        // 1. Using the table-driven technique:
        //    1. Specify a set of widely varying object values in terms of
        //       their individual attributes.
        //
        //    2. For each row 'R' in the table of P-1: (C-1..2)
        //       1. Use the value constructor to create a `const` object, `X`,
        //          having the value from 'R'.
        //       2. Use the (as yet unproven) salient attribute accessors to
        //          verify that all of the attributes of `X` have the expected
        //          values.
        //
        // Testing:
        //   RecordFormatterOptions(RecordFormatterTimezone::Enum tz);
        // --------------------------------------------------------------------

        if (verbose) cout << "VALUE CTOR\n"
                             "==========\n";

        if (verbose) cout << "\tUse table of distinct object values.\n";
        {
            for (bsl::size_t ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
                const int      LINE = DEFAULT_DATA[ti].d_line;
                const Tz::Enum TZ   = DEFAULT_DATA[ti].d_timezoneDefault;

                if (veryVerbose) { T_ P_(LINE) P(TZ) }

                const Obj X(TZ);  // test here

                ASSERTV(LINE, TZ == X.timezoneDefault());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR & PRIMARY MANIPULATORS (BOOTSTRAP)
        //   Ensure that the default constructor has the documented behavior
        //   and that the primary manipulators work as expected.
        //
        // Concerns:
        // 1. An object created with the default constructor has the
        //    contractually specified default value.
        //
        // 2. The `setTimezoneDefault` method sets the timezone default
        //    attribute to the specified value.
        //
        // Plan:
        // 1. Create an object using the default constructor.  Verify, using
        //    the basic accessors, that the object has the expected default
        //    value. (C-1)
        //
        // 2. Set the timezone default attribute to a different value and
        //    verify using the basic accessor that the value has been set.
        //    (C-2)
        //
        // Testing:
        //   RecordFormatterOptions();
        //   setTimezoneDefault(RecordFormatterTimezone::Enum value);
        // --------------------------------------------------------------------

        if (verbose) cout << "DEFAULT CTOR & PRIMARY MANIPULATORS\n"
                             "===================================\n";

        if (verbose) cout << "\tTesting default constructor.\n";
        {
            Obj mX;  const Obj& X = mX;
            ASSERT(Tz::e_UTC == X.timezoneDefault());
        }

        if (verbose) cout << "\tTesting primary manipulator.\n";
        {
            Obj mX;  const Obj& X = mX;

            mX.setTimezoneDefault(Tz::e_LOCAL);
            ASSERT(Tz::e_LOCAL == X.timezoneDefault());

            mX.setTimezoneDefault(Tz::e_UTC);
            ASSERT(Tz::e_UTC == X.timezoneDefault());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Create an object using the default constructor.
        // 2. Exercise the object using basic accessor and manipulator methods.
        // 3. Create an object using the value constructor.
        // 4. Exercise the object using basic accessor methods.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t 1. Create an object x1 (default ctor)\n";
        Obj mX1;  const Obj& X1 = mX1;
        if (veryVerbose) { T_ P(X1.timezoneDefault()); }

        if (verbose) cout << "\ta. Check initial state of x1.\n";
        ASSERT(Tz::e_UTC == X1.timezoneDefault());

        if (verbose)
            cout << "\tb. Try equality operators: x1 <op> x1.\n";
        // No equality operators defined

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t 2. Create an object x2 (copy from x1).\n";
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { T_ P(X2.timezoneDefault()); }

        if (verbose) cout << "\ta. Check initial state of x2.\n";
        ASSERT(Tz::e_UTC == X2.timezoneDefault());

        if (verbose)
            cout << "\tb. Try equality operators: x2 <op> x1, x2.\n";
        // No equality operators defined

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t 3. Set x1 to a new value.\n";
        mX1.setTimezoneDefault(Tz::e_LOCAL);
        if (veryVerbose) { T_ P(X1.timezoneDefault()); }

        if (verbose)
            cout << "\ta. Try equality operators: x1 <op> x1, x2.\n";
        // No equality operators defined

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t 4. Create an object x3 (value ctor).\n";
        Obj mX3(Tz::e_LOCAL);  const Obj& X3 = mX3;
        if (veryVerbose) { T_ P(X3.timezoneDefault()); }

        if (verbose) cout << "\ta. Check initial state of x3.\n";
        ASSERT(Tz::e_LOCAL == X3.timezoneDefault());

        if (verbose)
            cout << "\tb. Try equality operators: x3 <op> x1, x2, x3.\n";
        // No equality operators defined

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t 5. Create an object x4 (copy from x3).\n";
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (veryVerbose) { T_ P(X4.timezoneDefault()); }

        if (verbose) cout << "\ta. Check initial state of x4.\n";
        ASSERT(Tz::e_LOCAL == X4.timezoneDefault());

        if (verbose)
            cout << "\tb. Try equality operators: x4 <op> x1, x2, x3, x4.\n";
        // No equality operators defined

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t 6. Assign x2 = x1.\n";
        mX2 = X1;
        if (veryVerbose) { T_ P(X2.timezoneDefault()); }

        if (verbose)
            cout << "\ta. Try equality operators: x2 <op> x1, x2, x3, x4.\n";
        // No equality operators defined

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t 7. Assign x2 = x3.\n";
        mX2 = X3;
        if (veryVerbose) { T_ P(X2.timezoneDefault()); }

        if (verbose)
            cout << "\ta. Try equality operators: x2 <op> x1, x2, x3, x4.\n";
        // No equality operators defined

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t 8. Assign x1 = x1 (aliasing).\n";
        mX1 = X1;
        if (veryVerbose) { T_ P(X1.timezoneDefault()); }

        if (verbose)
            cout << "\ta. Try equality operators: x1 <op> x1, x2, x3, x4.\n";
        // No equality operators defined
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
