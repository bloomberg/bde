// bbldc_calendarbus252.t.cpp                                         -*-C++-*-
#include <bbldc_calendarbus252.h>

#include <bdlt_calendar.h>
#include <bdlt_date.h>
#include <bdlt_dayofweek.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of static member functions that compute
// the day and year difference between two dates.  The standard table-based
// test case implementation is used to verify the functionality of these
// methods.
//-----------------------------------------------------------------------------
// [ 1] int daysDiff(beginDate, endDate, calendar);
// [ 2] double yearsDiff(beginDate, endDate, calendar);
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bbldc::CalendarBus252 Util;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test        = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose     = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // Define and populate calendars used in testing.

    bdlt::Calendar mCA;  const bdlt::Calendar& CA = mCA;
    {
        mCA.setValidRange(bdlt::Date(2015, 6, 1), bdlt::Date(2015, 6, 30));
    }

    bdlt::Calendar mCB;  const bdlt::Calendar& CB = mCB;
    {
        mCB.setValidRange(bdlt::Date(2015, 6, 1), bdlt::Date(2015, 6, 30));
        mCB.addWeekendDay(bdlt::DayOfWeek::e_SUN);
        mCB.addWeekendDay(bdlt::DayOfWeek::e_SAT);
    }

    switch (test) { case 0:
      case 3: {
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
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing Day Count and Year Fraction
///- - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use 'bbldc::CalendarBus252'
// methods.  First, create two 'bdlt::Date' variables, 'd1' and 'd2':
//..
    const bdlt::Date d1(2003, 10, 19);
    const bdlt::Date d2(2003, 12, 31);
//..
// Then, create a 'calendar' with a valid range spanning 2003 and typical
// weekend days:
//..
    bdlt::Calendar calendar;
    calendar.setValidRange(bdlt::Date(2003, 1, 1), bdlt::Date(2003, 12, 31));
    calendar.addWeekendDay(bdlt::DayOfWeek::e_SUN);
    calendar.addWeekendDay(bdlt::DayOfWeek::e_SAT);
//..
// Next, compute the day count between 'd1' and 'd2':
//..
    const int daysDiff = bbldc::CalendarBus252::daysDiff(d1, d2, calendar);
    ASSERT(52 == daysDiff);
//..
// Finally, compute the year fraction between the two dates:
//..
    const double yearsDiff = bbldc::CalendarBus252::yearsDiff(d1,
                                                              d2,
                                                              calendar);
    // Need fuzzy comparison since 'yearsDiff' is a 'double'.
    ASSERT(yearsDiff > 0.2063 && yearsDiff < 0.2064);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'yearsDiff'
        //   Verify the method correctly computes the number of years between
        //   two dates.
        //
        // Concerns:
        //: 1 The 'yearsDiff' method produces the correct results.
        //:
        //: 2 Reversing the date parameters negates the returned value.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify a set S of {pairs of dates (d1, d2), a schedule of
        //:   periods P, and the difference in years D}.  For the method under
        //:   test, in a loop over the elements of S, apply the method to dates
        //:   having the values d1 and d2 with the periods P and 1.0 for the
        //:   'periodYearDiff' and confirm the result using the value D with a
        //:   fuzzy comparison (since the return value is a floating-point
        //:   number).   (C-1)
        //:
        //: 2 Also verify the result is negated when the date parameters are
        //:   reversed.  (C-2)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //  double yearsDiff(beginDate, endDate, calendar);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'yearsDiff'" << endl
                          << "===================" << endl;

        {
            static const struct {
                int d_lineNum;       // source line number
                int d_year1;         // beginDate year
                int d_month1;        // beginDate month
                int d_day1;          // beginDate day
                int d_year2;         // endDate year
                int d_month2;        // endDate month
                int d_day2;          // endDate day
                double d_numYearsA;  // result # of years for calendar 'CA'
                double d_numYearsB;  // result # of years for calendar 'CB'
            } DATA[] = {
                //      - - - first- -  - - second - -
                //line  year  mon  day  year  mon  day    YA      YB
                //----  ----  ---  ---  ----  ---  ---  ------  ------
                { L_,   2015,   6,   1, 2015,   6,   1, 0.0000, 0.0000 },
                { L_,   2015,   6,   1, 2015,   6,   8, 0.0278, 0.0198 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int    LINE        = DATA[di].d_lineNum;
                const double NUM_YEARS_A = DATA[di].d_numYearsA;
                const double NUM_YEARS_B = DATA[di].d_numYearsB;

                const bdlt::Date X(DATA[di].d_year1,
                                   DATA[di].d_month1,
                                   DATA[di].d_day1);
                const bdlt::Date Y(DATA[di].d_year2,
                                   DATA[di].d_month2,
                                   DATA[di].d_day2);

                if (veryVerbose) {
                    T_;  P_(X);  P_(Y);  P_(NUM_YEARS_A);  P_(NUM_YEARS_B);
                }

                const double RESULT_A = Util::yearsDiff(X, Y, CA);
                const double RESULT_B = Util::yearsDiff(X, Y, CB);

                if (veryVerbose) { P_(RESULT_A); P(RESULT_B); }
                const double diff_A = NUM_YEARS_A - RESULT_A;
                const double diff_B = NUM_YEARS_B - RESULT_B;
                LOOP_ASSERT(LINE, -0.00005 <= diff_A && diff_A <= 0.00005);
                LOOP_ASSERT(LINE, -0.00005 <= diff_B && diff_B <= 0.00005);

                // Verify the result is negated when the dates are reversed.

                const double NRESULT_A = Util::yearsDiff(Y, X, CA);
                const double NRESULT_B = Util::yearsDiff(Y, X, CB);
                const double sum_A     = RESULT_A + NRESULT_A;
                const double sum_B     = RESULT_B + NRESULT_B;
                LOOP_ASSERT(LINE, -1.0e-15 <= sum_A && sum_A <= 1.0e-15);
                LOOP_ASSERT(LINE, -1.0e-15 <= sum_B && sum_B <= 1.0e-15);
            }
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_PASS(Util::yearsDiff(bdlt::Date(2015, 6,  1),
                                             bdlt::Date(2015, 6, 30),
                                             CA));

            ASSERT_SAFE_FAIL(Util::yearsDiff(bdlt::Date(2015, 5, 31),
                                             bdlt::Date(2015, 6, 30),
                                             CA));

            ASSERT_SAFE_FAIL(Util::yearsDiff(bdlt::Date(2015, 6,  1),
                                             bdlt::Date(2015, 7,  1),
                                             CA));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'daysDiff'
        //   Verify the method correctly computes the number of days between
        //   two dates.
        //
        // Concerns:
        //: 1 The 'daysDiff' method produces the correct results.
        //:
        //: 2 Reversing the date parameters negates the returned value.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Define two calendars, 'A' and 'B', such that 'A != B'.  Specify a
        //:   set 'S' of {pairs of dates ('d1', 'd2') and their difference in
        //:   days 'DA' for calendar 'A' and their difference in days 'DB' for
        //:   calendar 'B'}.  For the method under test, in a loop over the
        //:   elements of 'S', apply the method to dates having the values 'd1'
        //:   and 'd2' and confirm the result using the value 'DA' when the
        //:   method is invoked with 'A' and the value 'DB' when the method is
        //:   invoked with 'B'.  (C-1)
        //:
        //: 2 Also verify the result is negated when the date parameters are
        //:   reversed.  (C-2)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   int daysDiff(beginDate, endDate, calendar);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'daysDiff'" << endl
                          << "==================" << endl;

        if (verbose) cout << "\nTesting: 'daysDiff(beginDate, endDate)'"
                          << "\n======================================="
                          << endl;

        {
            static const struct {
                int d_lineNum;   // source line number
                int d_year1;     // beginDate year
                int d_month1;    // beginDate month
                int d_day1;      // beginDate day
                int d_year2;     // endDate year
                int d_month2;    // endDate month
                int d_day2;      // endDate day
                int d_numDaysA;  // result number of days for calendar 'CA'
                int d_numDaysB;  // result number of days for calendar 'CB'
            } DATA[] = {
                //      - - - first- -  - - second - -
                //line  year  mon  day  year  mon  day  DA  DB
                //----  ----  ---  ---  ----  ---  ---  --  --
                { L_,   2015,   6,   1, 2015,   6,   1,  0,  0 },
                { L_,   2015,   6,   1, 2015,   6,   8,  7,  5 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE       = DATA[di].d_lineNum;
                const int NUM_DAYS_A = DATA[di].d_numDaysA;
                const int NUM_DAYS_B = DATA[di].d_numDaysB;

                const bdlt::Date X(DATA[di].d_year1,
                                   DATA[di].d_month1,
                                   DATA[di].d_day1);
                const bdlt::Date Y(DATA[di].d_year2,
                                   DATA[di].d_month2,
                                   DATA[di].d_day2);

                if (veryVerbose) {
                    T_;  P_(X);  P_(Y);  P_(NUM_DAYS_A); P_(NUM_DAYS_B);
                }

                const int RESULT_A = Util::daysDiff(X, Y, CA);
                const int RESULT_B = Util::daysDiff(X, Y, CB);

                if (veryVerbose) { P_(RESULT_A) P(RESULT_B); }
                LOOP_ASSERT(LINE, NUM_DAYS_A == RESULT_A);
                LOOP_ASSERT(LINE, NUM_DAYS_B == RESULT_B);

                // Verify the result is negated when the dates are reversed.

                const int NRESULT_A = Util::daysDiff(Y, X, CA);
                const int NRESULT_B = Util::daysDiff(Y, X, CB);
                LOOP_ASSERT(LINE, NRESULT_A == -RESULT_A);
                LOOP_ASSERT(LINE, NRESULT_B == -RESULT_B);
            }
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_PASS(Util::daysDiff(bdlt::Date(2015, 6,  1),
                                            bdlt::Date(2015, 6, 30),
                                            CA));

            ASSERT_SAFE_FAIL(Util::daysDiff(bdlt::Date(2015, 5, 31),
                                            bdlt::Date(2015, 6, 30),
                                            CA));

            ASSERT_SAFE_FAIL(Util::daysDiff(bdlt::Date(2015, 6,  1),
                                            bdlt::Date(2015, 7,  1),
                                            CA));
        }
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
