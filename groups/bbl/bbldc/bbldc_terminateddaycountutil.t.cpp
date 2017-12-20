// bbldc_terminateddaycountutil.t.cpp                                 -*-C++-*-

#include <bbldc_terminateddaycountutil.h>

#include <bdlt_date.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of static member functions that compute
// the day and year difference between two dates for a specified convention.
// The standard table-based test case implementation is used to verify the
// functionality of these methods.
// ----------------------------------------------------------------------------
// [ 2] int daysDiff(beginDate, endDate, terminationDate, convention);
// [ 1] bool isSupported(convention);
// [ 3] double yearsDiff(beginDate, endDate, terminationDate, convention);
// ----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bbldc::TerminatedDayCountUtil   Util;
typedef bbldc::DayCountConvention::Enum Enum;

const Enum ISDA_30_360_EOM = bbldc::DayCountConvention::e_ISDA_30_360_EOM;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test        = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose     = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
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
// The following snippets of code illustrate how to use
// 'bbldc::TerminatedDayCountUtil' methods.  First, create three 'bdlt::Date'
// variables, 'd1', 'd2', and 'dt':
//..
    const bdlt::Date d1(2003, 10, 18);
    const bdlt::Date d2(2003, 12, 31);
    const bdlt::Date dt(2004,  2, 29);
//..
// Then, compute the day count between 'd1' and 'd2' according to the ISDA
// 30/360 EOM day-count convention with termination date 'dt':
//..
    const int daysDiff = bbldc::TerminatedDayCountUtil::daysDiff(
                                 d1,
                                 d2,
                                 dt,
                                 bbldc::DayCountConvention::e_ISDA_30_360_EOM);
    ASSERT(72 == daysDiff);
//..
// Finally, compute the year fraction between the two dates according to the
// ISDA 30/360 EOM day-count convention with termination date 'dt':
//..
    const double yearsDiff = bbldc::TerminatedDayCountUtil::yearsDiff(
                                 d1,
                                 d2,
                                 dt,
                                 bbldc::DayCountConvention::e_ISDA_30_360_EOM);

    // Need fuzzy comparison since 'yearsDiff' is a 'double'.
    ASSERT(0.1999 < yearsDiff && 0.2001 > yearsDiff);
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'yearsDiff'
        //   Verify the method correctly computes the number of years between
        //   two dates for the provided convention.
        //
        // Concerns:
        //: 1 The 'yearsDiff' method produces the correct results for the
        //:   provided convention.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify two termination dates, TA and TB, and a set S of
        //:   {convention C, pairs of dates (d1, d2), their difference in years
        //:   YA for TA, and their difference in years YB for TB}.  For the
        //:   method under test, in a loop over the elements of S, apply the
        //:   method to dates having the values d1 and d2 using convention C
        //:   and confirm, with a fuzzy comparison (since the return value is a
        //:   floating-point number), the method's results using the value YA
        //:   for termination date TA and YB for termination date TB.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   double yearsDiff(beginDate, endDate, terminationDate, convention);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'yearsDiff'" << endl
                          << "===================" << endl;

        const bdlt::Date TA(2016, 2, 29);
        const bdlt::Date TB(2016, 3, 31);

        {
            static const struct {
                int    d_lineNum;    // source line number
                Enum   d_type;       // convention to use
                int    d_year1;      // beginDate year
                int    d_month1;     // beginDate month
                int    d_day1;       // beginDate day
                int    d_year2;      // endDate year
                int    d_month2;     // endDate month
                int    d_day2;       // endDate day
                double d_numYearsA;  // result # of years for 'TA'
                double d_numYearsB;  // result # of years for 'TB'
            } DATA[] = {
//                          - - first - -   - - second - -
//line  type                year  mon  day  year  mon  day    YA      YB
//----  ------------------  ----  ---  ---  ----  ---  ---  ------  ------
{ L_,   ISDA_30_360_EOM,   2015,   6,   1, 2015,   6,   1, 0.0000, 0.0000 },
{ L_,   ISDA_30_360_EOM,   2015,   6,   1, 2015,   6,   8, 0.0194, 0.0194 },
{ L_,   ISDA_30_360_EOM,   2016,   1,   1, 2016,   2,  29, 0.1611, 0.1639 },
{ L_,   ISDA_30_360_EOM,   2016,   1,   1, 2016,   3,  31, 0.2472, 0.2472 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // Ensure the test data differentiates each convention type.

            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const Enum CONV1 = DATA[i].d_type;
                    for (int j = i + 1; j < NUM_DATA; ++j) {
                        const Enum CONV2 = DATA[j].d_type;
                        if (CONV1 != CONV2) {
                            int hasUnique = 0;
                            for (int ii = 0; ii < NUM_DATA; ++ii) {
                                if (CONV1 == DATA[ii].d_type) {
                                  for (int jj = 0; jj < NUM_DATA; ++jj) {
                                      if (CONV2 == DATA[jj].d_type) {
                        //---------------^
                        if (DATA[ii].d_year1      == DATA[jj].d_year1
                         && DATA[ii].d_month1     == DATA[jj].d_month1
                         && DATA[ii].d_day1       == DATA[jj].d_day1
                         && DATA[ii].d_year2      == DATA[jj].d_year2
                         && DATA[ii].d_month2     == DATA[jj].d_month2
                         && DATA[ii].d_day2       == DATA[jj].d_day2
                         && (DATA[ii].d_numYearsA != DATA[jj].d_numYearsA
                          || DATA[ii].d_numYearsB != DATA[jj].d_numYearsB)) {
                            hasUnique = 1;
                        }
                        //---------------v
                                      }
                                  }
                                }
                            }
                            if (!hasUnique && veryVerbose) {
                                bsl::cout << CONV1
                                          << " is not distinguished from "
                                          << CONV2 << bsl::endl;
                            }
                            LOOP2_ASSERT(CONV1, CONV2, hasUnique);
                        }
                    }
                }
            }

            int di;
            for (di = 0; di < NUM_DATA; ++di) {
                const int    LINE        = DATA[di].d_lineNum;
                const double NUM_YEARS_A = DATA[di].d_numYearsA;
                const double NUM_YEARS_B = DATA[di].d_numYearsB;
                const Enum   CONV        = DATA[di].d_type;

                const bdlt::Date X(DATA[di].d_year1,
                                   DATA[di].d_month1,
                                   DATA[di].d_day1);

                const bdlt::Date Y(DATA[di].d_year2,
                                   DATA[di].d_month2,
                                   DATA[di].d_day2);

                if (veryVerbose) {
                    T_ P_(X) P_(Y) P(CONV);
                    T_ T_ T_ T_ T_ T_ T_ P_(NUM_YEARS_A) P(NUM_YEARS_B);
                    T_ T_ T_ T_ T_ T_ T_;
                }

                const double RESULT_A = Util::yearsDiff(X, Y, TA, CONV);
                const double RESULT_B = Util::yearsDiff(X, Y, TB, CONV);

                if (veryVerbose) { P(RESULT_A); P(RESULT_B); }

                const double diff_A = NUM_YEARS_A - RESULT_A;
                LOOP3_ASSERT(LINE,
                             NUM_YEARS_A,
                             RESULT_A,
                             -0.00005 <= diff_A && diff_A <= 0.00005);

                const double diff_B = NUM_YEARS_B - RESULT_B;
                LOOP3_ASSERT(LINE,
                             NUM_YEARS_B,
                             RESULT_B,
                             -0.00005 <= diff_B && diff_B <= 0.00005);
            }
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            ASSERT_OPT_PASS(Util::yearsDiff(bdlt::Date(2015, 6,  1),
                                            bdlt::Date(2015, 6, 30),
                                            TA,
                                            ISDA_30_360_EOM));

            ASSERT_OPT_FAIL(Util::yearsDiff(
                             bdlt::Date(2015, 1, 5),
                             bdlt::Date(2015, 5, 6),
                             TA,
                             bbldc::DayCountConvention::e_ISDA_ACTUAL_ACTUAL));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'daysDiff'
        //   Verify the method correctly computes the number of days between
        //   two dates for the provided convention.
        //
        // Concerns:
        //: 1 The 'daysDiff' method produces the correct results for the
        //:   provided convention.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify two termination dates, TA and TB, and a set S of
        //:   {convention C, pairs of dates (d1, d2), their difference in days
        //:   DA for TA, and their difference in days DB for TB}.  For the
        //:   method under test, in a loop over the elements of S, apply the
        //:   method to dates having the values d1 and d2 using convention C
        //:   and confirm the method's results using the value DA for
        //:   termination date TA and DB for termination date TB.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   int daysDiff(beginDate, endDate, terminationDate, convention);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'daysDiff'" << endl
                          << "==================" << endl;

        const bdlt::Date TA(2016, 2, 29);
        const bdlt::Date TB(2016, 3, 31);

        {
            static const struct {
                int  d_lineNum;   // source line number
                Enum d_type;      // convention to use
                int  d_year1;     // beginDate year
                int  d_month1;    // beginDate month
                int  d_day1;      // beginDate day
                int  d_year2;     // endDate year
                int  d_month2;    // endDate month
                int  d_day2;      // endDate day
                int  d_numDaysA;  // result # of years for 'TA'
                int  d_numDaysB;  // result # of years for 'TB'
            } DATA[] = {
//                          - - first - -   - - second - -
//line  type                year  mon  day  year  mon  day  DA  DB
//----  ------------------  ----  ---  ---  ----  ---  ---  --  --
{ L_,   ISDA_30_360_EOM,   2015,   6,   1, 2015,   6,   1,   0,  0 },
{ L_,   ISDA_30_360_EOM,   2015,   6,   1, 2015,   6,   8,   7,  7 },
{ L_,   ISDA_30_360_EOM,   2016,   1,   1, 2016,   2,  29,  58, 59 },
{ L_,   ISDA_30_360_EOM,   2016,   1,   1, 2016,   3,  31,  89, 89 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // Ensure the test data differentiates each convention type.  Note:
            // this part of the test is getting inappropriate as we add more
            // conventions, some of which count days identically.

            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const Enum CONV1 = DATA[i].d_type;
                    for (int j = i + 1; j < NUM_DATA; ++j) {
                        const Enum CONV2 = DATA[j].d_type;
                        if (CONV1 != CONV2) {
                            int hasUnique = 0;
                            for (int ii = 0; ii < NUM_DATA; ++ii) {
                                if (CONV1 == DATA[ii].d_type) {
                                  for (int jj = 0; jj < NUM_DATA; ++jj) {
                                      if (CONV2 == DATA[jj].d_type) {
                        //---------------^
                        if (DATA[ii].d_year1     == DATA[jj].d_year1
                         && DATA[ii].d_month1    == DATA[jj].d_month1
                         && DATA[ii].d_day1      == DATA[jj].d_day1
                         && DATA[ii].d_year2     == DATA[jj].d_year2
                         && DATA[ii].d_month2    == DATA[jj].d_month2
                         && DATA[ii].d_day2      == DATA[jj].d_day2
                         && (DATA[ii].d_numDaysA != DATA[jj].d_numDaysA
                          || DATA[ii].d_numDaysB != DATA[jj].d_numDaysB)) {
                            hasUnique = 1;
                        }
                        //---------------v
                                      }
                                  }
                                }
                            }
                            if (!hasUnique && veryVerbose) {
                                bsl::cout << CONV1
                                          << " is not distinguished from "
                                          << CONV2 << bsl::endl;
                            }
                            LOOP2_ASSERT(CONV1, CONV2, hasUnique);
                        }
                    }
                }
            }

            int di;
            for (di = 0; di < NUM_DATA ; ++di) {
                const int  LINE       = DATA[di].d_lineNum;
                const int  NUM_DAYS_A = DATA[di].d_numDaysA;
                const int  NUM_DAYS_B = DATA[di].d_numDaysB;
                const Enum CONV       = DATA[di].d_type;

                const bdlt::Date X(DATA[di].d_year1,
                                   DATA[di].d_month1,
                                   DATA[di].d_day1);

                const bdlt::Date Y(DATA[di].d_year2,
                                   DATA[di].d_month2,
                                   DATA[di].d_day2);

                if (veryVerbose) {
                    T_ P_(X) P_(Y) P(CONV);
                    T_ T_ T_ T_ T_ T_ T_ P_(NUM_DAYS_A) P(NUM_DAYS_B);
                    T_ T_ T_ T_ T_ T_ T_;
                }

                const int RESULT_A = Util::daysDiff(X, Y, TA, CONV);
                const int RESULT_B = Util::daysDiff(X, Y, TB, CONV);

                if (veryVerbose) { P_(RESULT_A); P(RESULT_B); }
                LOOP_ASSERT(LINE, NUM_DAYS_A == RESULT_A);
                LOOP_ASSERT(LINE, NUM_DAYS_B == RESULT_B);
            }
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            ASSERT_OPT_PASS(Util::daysDiff(bdlt::Date(2015, 6,  1),
                                           bdlt::Date(2015, 6, 30),
                                           TA,
                                           ISDA_30_360_EOM));

            ASSERT_OPT_FAIL(Util::daysDiff(
                             bdlt::Date(2015, 1, 5),
                             bdlt::Date(2015, 5, 6),
                             TA,
                             bbldc::DayCountConvention::e_ISDA_ACTUAL_ACTUAL));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'isSupported'
        //   Verify the method correctly indicates whether or not the provided
        //   convention is supported by this component.
        //
        // Concerns:
        //: 1 The 'isSupported' method produces the correct results for the
        //:   provided convention.
        //
        // Plan:
        //: 1 Directly test the return value of the method.  (C-1)
        //
        // Testing:
        //   bool isSupported(convention);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'isSupported'" << endl
                          << "=====================" << endl;

        for (int i = 0; i < 1000; ++i) {
            const Enum convention = static_cast<Enum>(i);
            ASSERT(   (ISDA_30_360_EOM == convention)
                   == Util::isSupported(convention));
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT == FOUND." << endl;
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
