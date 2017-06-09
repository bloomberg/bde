// bdlstat_moment.t.cpp                                               -*-C++-*-
#include <bdlstat_moment.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test consists of several static member functions that
// provide a way to generate an infinite series of dates and compute a subset
// of it.  The general plan is that the methods are tested against a set of
// tabulated test vectors, and negative tests for preconditions are conducted.
// The test vectors cover the permutations of interest of relations of the type
// 'earliest <= example <= latest' as well as edge cases for the resulting
// schedule.
// ----------------------------------------------------------------------------
// [ 2] generateFromDayInterval(s, e, l, example, interval);
// [ 3] generateFromDayOfMonth(s, e, l, eY, eM, i, tDOM, tDOF);
// [ 4] generateFromBusinessDayOfMonth(s, e, l, c, eY, eM, i, tBDOM);
// [ 5] generateFromDayOfWeekAfterDayOfMonth(s, e, l, d, eY, eM, i, DOM);
// [ 6] generateFromDayOfWeekInMonth(s, e, l, d, eY, eM, i, oW);
// ----------------------------------------------------------------------------
// [ 7] USAGE EXAMPLE
// [ 1] toString(output, date)
// ----------------------------------------------------------------------------

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
//             NON-STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define DAY(X) bdlt::DayOfWeek::e_##X       // Shorten qualified name

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlstat::Moment<bdlstat::MEAN> ObjM;
typedef bdlstat::Moment<bdlstat::VARIANCE> ObjV;
typedef bdlstat::Moment<bdlstat::KURTOSIS> ObjK;

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int         test = argc > 1 ? atoi(argv[1]) : 0;
    bool     verbose = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Generating a Schedule
///- - - - - - - - - - - - - - - -
// Suppose that we want to determine the sequence of dates that are:
//   * integral multiples of 9 months away from July 2007,
//   * on the 23rd day of the month,
//   * and within the closed interval '[02/01/2012, 02/28/2015]'.
//
// First, we define the inputs and output to the schedule generation function:
//..
/*    bdlt::Date earliest(2012, 2,  1);
    bdlt::Date   latest(2015, 2, 28);
    bdlt::Date  example(2007, 7, 23);

    bsl::vector<bdlt::Date> schedule;
//..
// Now, we invoke the 'generateFromDayOfMonth' routine to obtain the subset of
// dates:
//..
    bblb::ScheduleGenerationUtil::generateFromDayOfMonth(
                                                    &schedule,
                                                    earliest,
                                                    latest,
                                                    example.year(),
                                                    example.month(),
                                                    9,    // 'intervalInMonths'
                                                    23);  // 'targetDayOfMonth'
//..
// Finally, we assert that the generated schedule is what we expect:
//..
    ASSERT(4 == schedule.size());
    ASSERT(bdlt::Date(2012, 10, 23) == schedule[0]);
    ASSERT(bdlt::Date(2013,  7, 23) == schedule[1]);
    ASSERT(bdlt::Date(2014,  4, 23) == schedule[2]);
    ASSERT(bdlt::Date(2015,  1, 23) == schedule[3]);*/
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'generateFromDayInterval'
        //
        // Concerns:
        //: 1 The method produces the expected result values.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test permutations of interest for the method's arguments.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values.  (C-2)
        //
        // Testing:
        //   generateFromDayInterval(s, e, l, example, interval);
        // --------------------------------------------------------------------

/*        if (verbose) cout << endl
                          << "TESTING 'generateFromDayInterval'" << endl
                          << "=================================" << endl;

        static const struct {
            int         d_lineNum;
            int         d_earliestYYYYMMDD;
            int         d_latestYYYYMMDD;
            int         d_exampleYYYYMMDD;
            int         d_interval;
            const char *d_expectedOutputString_p;
        } INPUT[] = {
            //LN  earliest   latest   example    I   EXP
            //--  --------  --------  --------  ---  -----------------------
            { L_, 20140505, 20140512, 20010201,   1,
  "20140505,20140506,20140507,20140508,20140509,20140510,20140511,20140512," },
            { L_, 20150123, 20150203, 20190205,   7, "20150127,20150203,"    },
            { L_, 20150123, 20150304, 20190223,  14,
                                               "20150131,20150214,20150228," },
            { L_, 20150123, 20150703, 19680607,  30,
                    "20150201,20150303,20150402,20150502,20150601,20150701," },
            { L_, 20010123, 20070203, 20520201, 365,
                    "20010213,20020213,20030213,20040213,20050212,20060212," },
            { L_, 20150123, 20150125, 20150122,   3, "20150125,"             },
            { L_, 20150123, 20150123, 20150124,   2, ""                      },
        };

        const size_t NUM_DATA = sizeof INPUT / sizeof *INPUT;

        bsl::vector<bdlt::Date> schedule;

        for (size_t di = 0; di < NUM_DATA; ++di) {
            const int  LINE     = INPUT[di].d_lineNum;
            bdlt::Date earliest = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                 INPUT[di].d_earliestYYYYMMDD);
            bdlt::Date latest   = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                   INPUT[di].d_latestYYYYMMDD);
            bdlt::Date example = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                  INPUT[di].d_exampleYYYYMMDD);

            const int   interval       = INPUT[di].d_interval;
            const char* expectedOutput = INPUT[di].d_expectedOutputString_p;

            Obj::generateFromDayInterval(
                               &schedule, earliest, latest, example, interval);

            bsl::ostringstream output;
            toString(&output, schedule);

            if (veryVerbose) { P_(LINE) P_(output.str()) P(expectedOutput); }

            LOOP3_ASSERT(di,
                         output.str(),
                         expectedOutput,
                         output.str() == expectedOutput);
        }

        // negative tests

        if (veryVerbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard g(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Date d(2015, 1, 23);

            // 'earliest <= latest'

            ASSERT_FAIL(Obj::generateFromDayInterval(
                                                   &schedule, d, d - 1, d, 2));
            ASSERT_PASS(Obj::generateFromDayInterval(&schedule, d, d, d, 2));

            // 'interval'

            ASSERT_FAIL(Obj::generateFromDayInterval(&schedule, d, d, d, 0));
            ASSERT_PASS(Obj::generateFromDayInterval(&schedule, d, d, d, 1));
        }*/
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'toString'
        //
        // Concerns:
        //: 1 The method produces the expected result values.
        //
        // Plan:
        //: 1 Test permutations of interest for the method's arguments.  (C-1)
        //
        // Testing:
        //   toString(output, date)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'toString'" << endl
                          << "==================" << endl;

        double input[] = {1.0, 2.0, 4.0, 5.0};
        bdlstat::Moment<bdlstat::MEAN> m1;
        for(int i = 0; i < 4; ++i)
        	m1.add(input[i]);
        ASSERT(4 == m1.getCount());
        ASSERT(3.0 == m1.getMean());
        //ASSERT(fabs(3.33333  - m2.getVariance()) < 1e-5); // Does not compile, as it should

        bdlstat::Moment<bdlstat::VARIANCE> m2;
        for(int i = 0; i < 4; ++i)
        	m2.add(input[i]);
        ASSERT(4 == m2.getCount());
        ASSERT(3.0 == m2.getMean());
        ASSERT(fabs(3.33333  - m2.getVariance()) < 1e-5);

        bdlstat::Moment<bdlstat::KURTOSIS> m4;
        for(int i = 0; i < 4; ++i)
        	m4.add(input[i]);
        ASSERT(4 == m4.getCount());
        ASSERT(3.0 == m4.getMean());
        ASSERT(fabs(3.33333  - m4.getVariance()) < 1e-5);
        ASSERT(fabs(-1.38086 - m4.getSkew())     < 1e-5);
        ASSERT(fabs(644.185  - m4.getKurtosis()) < 1e-3);
        //cout << "variance=" << m4.getVariance() <<  ",skew=" << m4.getSkew() <<  ",kurtosis=" << m4.getKurtosis() <<  "\n";
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
