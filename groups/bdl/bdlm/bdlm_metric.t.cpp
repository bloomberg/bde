// bdlm_metric.t.cpp                                                  -*-C++-*-
#include <bdlm_metric.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::atoi;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test wraps a variant.  The primary concern is that the
// supplied methods correctly forward to the variant.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] Metric();
// [ 3] Metric(const Gauge& value);
// [ 2] ~Metric();
//
// MANIPULATORS
// [ 3] Metric& operator=(const Gauge& value);
//
// ACCESSORS
// [ 3] bool isGauge() const;
// [ 3] const Gauge& theGauge() const;
//
// FREE OPERATORS
// [ 4] bool operator==(const Metric& lhs, rhs);
// [ 4] bool operator!=(const Metric& lhs, rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

static int                test;
static int             verbose;
static int         veryVerbose;
static int     veryVeryVerbose;
static int veryVeryVeryVerbose;

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlm::Metric Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
                   test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;  // always the last

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator ta(veryVeryVeryVerbose);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 'true  == (X == X)'  (i.e., identity)
        //:
        //: 4 'false == (X != X)'  (i.e., identity)
        //:
        //: 5 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if '!(X == Y)'
        //:
        //: 8 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 9 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:10 The equality operator's signature and return type are standard.
        //:
        //:11 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-8..11)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual salient
        //:   attributes such that for each salient attribute, there exists a
        //:   pair of rows that differ (slightly) in only the column
        //:   corresponding to that attribute.
        //:
        //: 3 For each row 'R1' in the table of P-3:  (C-1..7)
        //:
        //:   1 Create a single object, and use it to verify the reflexive
        //:     (anti-reflexive) property of equality (inequality) in the
        //:     presence of aliasing.  (C-3..4)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1..2, 5..7)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 Create an object 'X' having the value 'R1'.
        //:
        //:     3 Create an object 'Y' having the value 'R2'.
        //:
        //:     4 Verify the commutative property and expected return value for
        //:       both '==' and '!='.  (C-1..2, 5..7)
        //
        // Testing:
        //   bool operator==(const Metric& lhs, rhs);
        //   bool operator!=(const Metric& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING EQUALITY-COMPARISON OPERATORS" << endl
                          << "=====================================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace bdlm;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static const struct {
            int         d_line;     // source line number
            int         d_isGauge;  // flag indicating value is a guage
            double      d_gauge;    // value of metric if the metric is a guage
        } DATA[] = {
            // The first row of the table below represents an object value
            // consisting of "baseline" attribute values.  Each subsequent row
            // differs (slightly) from the first in exactly one attribute
            // value.

            //LN  ISG   G
            //--  ---  ---
            { L_,   0, 0.0 },  // baseline

            { L_,   1, 1.0 },
            { L_,   1, 2.0 },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1     = DATA[ti].d_line;
            const int         IS_GUAGE1 = DATA[ti].d_isGauge;
            const Obj::Gauge  GUAGE1    = Obj::Gauge(DATA[ti].d_gauge);

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mX;  const Obj& X = mX;

                if (IS_GUAGE1) {
                    mX = GUAGE1;
                }

                LOOP1_ASSERT(LINE1,   X == X);
                LOOP1_ASSERT(LINE1, !(X != X));
            }
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2     = DATA[tj].d_line;
                const int         IS_GUAGE2 = DATA[tj].d_isGauge;
                const Obj::Gauge  GUAGE2    = Obj::Gauge(DATA[tj].d_gauge);

                const bool EXP = ti == tj;  // expected for equality comparison

                Obj mX;  const Obj& X = mX;

                if (IS_GUAGE1) {
                    mX = GUAGE1;
                }

                Obj mY;  const Obj& Y = mY;

                if (IS_GUAGE2) {
                    mY = GUAGE2;
                }

                // Verify value, and commutative.

                LOOP2_ASSERT(LINE1, LINE2,  EXP == (X == Y));
                LOOP2_ASSERT(LINE1, LINE2,  EXP == (Y == X));

                LOOP2_ASSERT(LINE1, LINE2, !EXP == (X != Y));
                LOOP2_ASSERT(LINE1, LINE2, !EXP == (Y != X));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GAUGE METHODS
        //
        // Concerns:
        //: 1 The gauge value constructor creates the correct initial value.
        //:
        //: 2 The gauge value assignment operator works as expected.
        //:
        //: 3 The gauge value accessors, 'isGauge' and 'theGauge', work as
        //:   expected.
        //
        // Plan:
        //: 1 Create objects using the value constructor and verify the value
        //:   using the accessors.  (C-1)
        //:
        //: 2 Modify the objects using the assignment operator and verify the
        //:   value using the accessors.  (C-2,3)
        //
        // Testing:
        //   Metric(const Gauge& value);
        //   Metric& operator=(const Gauge& value);
        //   bool isGauge() const;
        //   const Gauge& theGauge() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GAUGE METHODS" << endl
                          << "=============" << endl;

        {
            Obj mX(Obj::Gauge(3.0));  const Obj& X = mX;

            ASSERT(true            == X.isGauge());
            ASSERT(Obj::Gauge(3.0) == X.theGauge());
            ASSERT(Obj::Gauge(5.0) != X.theGauge());

            Obj& mY = (mX = Obj::Gauge(5.0));  const Obj& Y = mY;

            ASSERT(true            == Y.isGauge());
            ASSERT(Obj::Gauge(3.0) != Y.theGauge());
            ASSERT(Obj::Gauge(5.0) == Y.theGauge());
        }
        {
            Obj mX(Obj::Gauge(4.0));  const Obj& X = mX;

            ASSERT(true            == X.isGauge());
            ASSERT(Obj::Gauge(4.0) == X.theGauge());
            ASSERT(Obj::Gauge(6.0) != X.theGauge());

            Obj& mY = (mX = Obj::Gauge(6.0));  const Obj& Y = mY;

            ASSERT(true            == Y.isGauge());
            ASSERT(Obj::Gauge(4.0) != Y.theGauge());
            ASSERT(Obj::Gauge(6.0) == Y.theGauge());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CREATORS
        //   The basic concern is that the default constructor and the
        //   destructor operate as expected.
        //
        // Concerns:
        //: 1 The default constructor creates the correct initial value.
        //
        //: 2 The destructor works as expected.
        //
        // Plan:
        //: 1 Create an object using the default constructor and verify the
        //:   value (empty).  (C-1)
        //:
        //: 2 Allow the created object to go out-of-scope.  (C-2)
        //
        // Testing:
        //   Metric();
        //   ~Metric();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CREATORS" << endl
                          << "========" << endl;

        Obj mX;  const Obj& X = mX;

        ASSERT(false == X.isGauge());
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
        //: 1 Create an object 'X'.
        //: 2 Get access to each attribute.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj mX; const Obj& X = mX;

        ASSERT(false == X.isGauge());

        mX = Obj::Gauge(3.0);

        ASSERT(true            == X.isGauge());
        ASSERT(Obj::Gauge(3.0) == X.theGauge());
        ASSERT(Obj::Gauge(5.0) != X.theGauge());

        mX = Obj::Gauge(5.0);

        ASSERT(true            == X.isGauge());
        ASSERT(Obj::Gauge(3.0) != X.theGauge());
        ASSERT(Obj::Gauge(5.0) == X.theGauge());
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: No memory came from the global or default allocator.

    LOOP2_ASSERT(test,
                 globalAllocator.numBlocksTotal(),
                 0 == globalAllocator.numBlocksTotal());
    LOOP2_ASSERT(test,
                 defaultAllocator.numBlocksTotal(),
                 0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
