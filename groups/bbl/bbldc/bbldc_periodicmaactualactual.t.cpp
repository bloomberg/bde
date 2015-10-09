// bbldc_periodicmaactualactual.t.cpp                                 -*-C++-*-
#include <bbldc_periodicmaactualactual.h>

#include <bdls_testutil.h>

#include <bdlt_date.h>

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
// [ 1] int daysDiff(const bdlt::Date& bD, const bdlt::Date& eD);
// [ 2] double yearsDiff(bD, eD, pD, pYD);
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

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bbldc::PeriodIcmaActualActual Util;

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
// The following snippets of code illustrate how to use
// 'bbldc::PeriodIcmaActualActual' methods.  First, create two 'bdlt::Date'
// variables, 'd1' and 'd2':
//..
    const bdlt::Date d1(2003, 10, 19);
    const bdlt::Date d2(2003, 12, 31);
//..
// Then, create a schedule of period dates, 'sched', corresponding to a
// quarterly payment ('periodYearDiff == 0.25'):
//..
    bsl::vector<bdlt::Date> sched;
    sched.push_back(bdlt::Date(2003, 10, 1));
    sched.push_back(bdlt::Date(2004,  1, 1));
//..
// Next, compute the day count between 'd1' and 'd2':
//..
    const int daysDiff = bbldc::PeriodIcmaActualActual::daysDiff(d1, d2);
    ASSERT(73 == daysDiff);
//..
// Finally, compute the year fraction between the two dates:
//..
    const double yearsDiff = bbldc::PeriodIcmaActualActual::yearsDiff(d1,
                                                                      d2,
                                                                      sched,
                                                                      0.25);
    // Need fuzzy comparison since 'yearsDiff' is a 'double'.
    ASSERT(yearsDiff > 0.1983 && yearsDiff < 0.1985);
//..
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
        //: 3 The 'periodYearDiff' argument acts as a simple multiplier of the
        //:   result.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
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
        //: 3 Also verify the result is multiplied by the 'periodYearDiff' when
        //:   alternate values are supplied.  (C-3)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   double yearsDiff(bD, eD, pD, pYD);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'yearsDiff'" << endl
                          << "===================" << endl;

        {
            bsl::vector<bdlt::Date>        mA00;
            const bsl::vector<bdlt::Date>& A00 = mA00;
            {
                mA00.push_back(bdlt::Date(2015, 3, 1));
                mA00.push_back(bdlt::Date(2015, 4, 1));
            }

            bsl::vector<bdlt::Date>        mA01;
            const bsl::vector<bdlt::Date>& A01 = mA01;
            {
                mA01.push_back(bdlt::Date(2015, 3, 1));
                mA01.push_back(bdlt::Date(2015, 4, 1));
                mA01.push_back(bdlt::Date(2015, 5, 1));
            }

            bsl::vector<bdlt::Date>        mA10;
            const bsl::vector<bdlt::Date>& A10 = mA10;
            {
                mA10.push_back(bdlt::Date(2015, 2, 1));
                mA10.push_back(bdlt::Date(2015, 3, 1));
                mA10.push_back(bdlt::Date(2015, 4, 1));
            }

            bsl::vector<bdlt::Date>        mA11;
            const bsl::vector<bdlt::Date>& A11 = mA11;
            {
                mA11.push_back(bdlt::Date(2015, 2, 1));
                mA11.push_back(bdlt::Date(2015, 3, 1));
                mA11.push_back(bdlt::Date(2015, 4, 1));
                mA11.push_back(bdlt::Date(2015, 5, 1));
            }

            bsl::vector<bdlt::Date>        mB;
            const bsl::vector<bdlt::Date>& B = mB;
            {
                mB.push_back(bdlt::Date(2015, 1, 10));
                mB.push_back(bdlt::Date(2015, 2, 10));
                mB.push_back(bdlt::Date(2015, 3, 10));
            }

            bsl::vector<bdlt::Date>        mC;
            const bsl::vector<bdlt::Date>& C = mC;
            {
                mC.push_back(bdlt::Date(2015, 1, 15));
                mC.push_back(bdlt::Date(2015, 2, 15));
                mC.push_back(bdlt::Date(2015, 3, 15));
                mC.push_back(bdlt::Date(2015, 4, 15));
            }

            bsl::vector<bdlt::Date>        mD;
            const bsl::vector<bdlt::Date>& D = mD;
            {
                mD.push_back(bdlt::Date(2015, 1, 5));
                mD.push_back(bdlt::Date(2015, 2, 5));
                mD.push_back(bdlt::Date(2015, 3, 5));
                mD.push_back(bdlt::Date(2015, 4, 5));
                mD.push_back(bdlt::Date(2015, 5, 5));
            }

            bsl::vector<bdlt::Date> mY;  const bsl::vector<bdlt::Date>& Y = mY;
            {
                for (unsigned year = 1990; year <= 2006; ++year) {
                    mY.push_back(bdlt::Date(year, 1, 1));
                }
            }

            bsl::vector<bdlt::Date>        mS1;
            const bsl::vector<bdlt::Date>& S1 = mS1;
            {
                mS1.push_back(bdlt::Date(1998, 7, 1));
                mS1.push_back(bdlt::Date(1999, 7, 1));
                mS1.push_back(bdlt::Date(2000, 7, 1));
            }

            bsl::vector<bdlt::Date>        mS2;
            const bsl::vector<bdlt::Date>& S2 = mS2;
            {
                mS2.push_back(bdlt::Date(1999, 1, 1));
                mS2.push_back(bdlt::Date(1999, 7, 1));
                mS2.push_back(bdlt::Date(2000, 1, 1));
            }

            static const struct {
                int    d_lineNum;   // source line number
                int    d_year1;     // beginDate year
                int    d_month1;    // beginDate month
                int    d_day1;      // beginDate day
                int    d_year2;     // endDate year
                int    d_month2;    // endDate month
                int    d_day2;      // endDate day

                const bsl::vector<bdlt::Date> *d_sched;
                                    // period dates

                double d_numYears;  // result number of years
            } DATA[] = {
                //      - - - first- -  - - second - -
                //line  year  mon  day  year  mon  day  schedule  numYears
                //----  ----  ---  ---  ----  ---  ---  --------  --------
                { L_,   2015,   3,   1, 2015,   3,   1,     &A00,   0.0000 },
                { L_,   2015,   3,   1, 2015,   3,   2,     &A00,   0.0323 },
                { L_,   2015,   3,   2, 2015,   3,   3,     &A00,   0.0323 },
                { L_,   2015,   3,  30, 2015,   3,  31,     &A00,   0.0323 },
                { L_,   2015,   3,  31, 2015,   4,   1,     &A00,   0.0323 },
                { L_,   2015,   4,   1, 2015,   4,   1,     &A00,   0.0000 },
                { L_,   2015,   3,   1, 2015,   4,   1,     &A00,   1.0000 },
                { L_,   2015,   3,   2, 2015,   4,   1,     &A00,   0.9677 },
                { L_,   2015,   3,   1, 2015,   3,  31,     &A00,   0.9677 },
                { L_,   2015,   3,   2, 2015,   3,  31,     &A00,   0.9355 },

                { L_,   2015,   3,   1, 2015,   3,   1,     &A01,   0.0000 },
                { L_,   2015,   3,   1, 2015,   3,   2,     &A01,   0.0323 },
                { L_,   2015,   3,   2, 2015,   3,   3,     &A01,   0.0323 },
                { L_,   2015,   3,  30, 2015,   3,  31,     &A01,   0.0323 },
                { L_,   2015,   3,  31, 2015,   4,   1,     &A01,   0.0323 },
                { L_,   2015,   4,   1, 2015,   4,   1,     &A01,   0.0000 },
                { L_,   2015,   3,   1, 2015,   4,   1,     &A01,   1.0000 },
                { L_,   2015,   3,   2, 2015,   4,   1,     &A01,   0.9677 },
                { L_,   2015,   3,   1, 2015,   3,  31,     &A01,   0.9677 },
                { L_,   2015,   3,   2, 2015,   3,  31,     &A01,   0.9355 },

                { L_,   2015,   3,   1, 2015,   3,   1,     &A10,   0.0000 },
                { L_,   2015,   3,   1, 2015,   3,   2,     &A10,   0.0323 },
                { L_,   2015,   3,   2, 2015,   3,   3,     &A10,   0.0323 },
                { L_,   2015,   3,  30, 2015,   3,  31,     &A10,   0.0323 },
                { L_,   2015,   3,  31, 2015,   4,   1,     &A10,   0.0323 },
                { L_,   2015,   4,   1, 2015,   4,   1,     &A10,   0.0000 },
                { L_,   2015,   3,   1, 2015,   4,   1,     &A10,   1.0000 },
                { L_,   2015,   3,   2, 2015,   4,   1,     &A10,   0.9677 },
                { L_,   2015,   3,   1, 2015,   3,  31,     &A10,   0.9677 },
                { L_,   2015,   3,   2, 2015,   3,  31,     &A10,   0.9355 },

                { L_,   2015,   3,   1, 2015,   3,   1,     &A11,   0.0000 },
                { L_,   2015,   3,   1, 2015,   3,   2,     &A11,   0.0323 },
                { L_,   2015,   3,   2, 2015,   3,   3,     &A11,   0.0323 },
                { L_,   2015,   3,  30, 2015,   3,  31,     &A11,   0.0323 },
                { L_,   2015,   3,  31, 2015,   4,   1,     &A11,   0.0323 },
                { L_,   2015,   4,   1, 2015,   4,   1,     &A11,   0.0000 },
                { L_,   2015,   3,   1, 2015,   4,   1,     &A11,   1.0000 },
                { L_,   2015,   3,   2, 2015,   4,   1,     &A11,   0.9677 },
                { L_,   2015,   3,   1, 2015,   3,  31,     &A11,   0.9677 },
                { L_,   2015,   3,   2, 2015,   3,  31,     &A11,   0.9355 },

                { L_,   2015,   1,  10, 2015,   3,  10,       &B,   2.0000 },
                { L_,   2015,   1,  10, 2015,   2,  10,       &B,   1.0000 },
                { L_,   2015,   2,  10, 2015,   3,  10,       &B,   1.0000 },
                { L_,   2015,   1,  10, 2015,   1,  11,       &B,   0.0323 },
                { L_,   2015,   2,  10, 2015,   2,  11,       &B,   0.0357 },
                { L_,   2015,   2,   7, 2015,   2,  12,       &B,   0.1682 },
                { L_,   2015,   2,  10, 2015,   2,  10,       &B,   0.0000 },

                { L_,   2015,   1,  15, 2015,   4,  15,       &C,   3.0000 },
                { L_,   2015,   1,  15, 2015,   3,  15,       &C,   2.0000 },
                { L_,   2015,   2,  15, 2015,   4,  15,       &C,   2.0000 },
                { L_,   2015,   1,  15, 2015,   2,  16,       &C,   1.0357 },
                { L_,   2015,   2,  15, 2015,   3,  16,       &C,   1.0323 },
                { L_,   2015,   2,  12, 2015,   3,  17,       &C,   1.1613 },
                { L_,   2015,   2,  15, 2015,   3,  15,       &C,   1.0000 },

                { L_,   2015,   1,   5, 2015,   5,   5,       &D,   4.0000 },
                { L_,   2015,   1,   5, 2015,   4,   5,       &D,   3.0000 },
                { L_,   2015,   2,   5, 2015,   5,   5,       &D,   3.0000 },
                { L_,   2015,   1,   5, 2015,   3,   6,       &D,   2.0323 },
                { L_,   2015,   2,   5, 2015,   4,   6,       &D,   2.0333 },
                { L_,   2015,   2,   2, 2015,   4,   7,       &D,   2.1634 },
                { L_,   2015,   2,   5, 2015,   4,   5,       &D,   2.0000 },

                { L_,   1992,   2,   1, 1992,   3,   1,       &Y,   0.0792 },
                { L_,   1992,   2,   1, 1993,   3,   1,       &Y,   1.0769 },
                { L_,   1992,   3,   1, 1992,   2,   1,       &Y,  -0.0792 },
                { L_,   1993,   2,   1, 1993,   3,   1,       &Y,   0.0767 },
                { L_,   1993,   2,   1, 1996,   2,   1,       &Y,   2.9998 },
                { L_,   1993,   3,   1, 1992,   2,   1,       &Y,  -1.0769 },
                { L_,   1993,   3,   1, 1993,   2,   1,       &Y,  -0.0767 },
                { L_,   1996,   1,  15, 1996,   5,  31,       &Y,   0.3743 },
                { L_,   1996,   2,   1, 1993,   2,   1,       &Y,  -2.9998 },
                { L_,   2000,   2,  27, 2000,   2,  27,       &Y,   0.0000 },
                { L_,   2000,   2,  27, 2000,   2,  28,       &Y,   0.0027 },
                { L_,   2000,   2,  27, 2000,   2,  29,       &Y,   0.0055 },
                { L_,   2000,   2,  28, 2000,   2,  27,       &Y,  -0.0027 },
                { L_,   2000,   2,  28, 2000,   2,  28,       &Y,   0.0000 },
                { L_,   2000,   2,  28, 2000,   2,  29,       &Y,   0.0027 },
                { L_,   2000,   2,  29, 2000,   2,  27,       &Y,  -0.0055 },
                { L_,   2000,   2,  29, 2000,   2,  28,       &Y,  -0.0027 },
                { L_,   2000,   2,  29, 2000,   2,  29,       &Y,   0.0000 },
                { L_,   2001,   1,   1, 2003,   1,   1,       &Y,   2.0000 },
                { L_,   2003,   1,   1, 2001,   1,   1,       &Y,  -2.0000 },
                { L_,   2003,   2,  28, 2004,   2,  29,       &Y,   1.0023 },
                { L_,   2004,   2,  29, 2003,   2,  28,       &Y,  -1.0023 },

                { L_,   1999,   2,   1, 2000,   7,   1,      &S1,   1.4110 },
                { L_,   1999,   2,   1, 2000,   1,   1,      &S2,   1.8287 }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            double PYD[] = { 0.5, 0.25 };  // periodYearDiff
            double NUM_PYD = sizeof PYD / sizeof *PYD;

            if (verbose) cout <<
                "\nTesting: 'yearsDiff(beginDate, endDate)'" << endl;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int    LINE      = DATA[di].d_lineNum;
                const double NUM_YEARS = DATA[di].d_numYears;

                const bsl::vector<bdlt::Date>& SCHED = *DATA[di].d_sched;

                const bdlt::Date X(DATA[di].d_year1,
                                   DATA[di].d_month1,
                                   DATA[di].d_day1);
                const bdlt::Date Y(DATA[di].d_year2,
                                   DATA[di].d_month2,
                                   DATA[di].d_day2);

                if (veryVerbose) { T_;  P_(X);  P_(Y);  P_(NUM_YEARS); }
                const double RESULT = Util::yearsDiff(X, Y, SCHED, 1.0);

                if (veryVerbose) { P(RESULT); }
                const double diff = NUM_YEARS - RESULT;
                LOOP_ASSERT(LINE, -0.00005 <= diff && diff <= 0.00005);

                // Verify the result is negated when the dates are reversed.

                const double NRESULT = Util::yearsDiff(Y, X, SCHED, 1.0);
                const double sum     = RESULT + NRESULT;
                LOOP_ASSERT(LINE, -1.0e-15 <= sum && sum <= 1.0e-15);

                // Verify modifying the 'periodYearFraction' value works as
                // expected.

                for (int dj = 0; dj < NUM_PYD; ++dj) {
                    const double pyd = PYD[dj];

                    const double RESULT2 = Util::yearsDiff(X, Y, SCHED, pyd);
                    const double diff2 = NUM_YEARS * pyd - RESULT2;

                    LOOP2_ASSERT(LINE,
                                 dj,
                                 -0.00005 <= diff2 && diff2 <= 0.00005);
                }

            }
        }

        { // negative testing
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            // 'periodDate' with no errors.

            bsl::vector<bdlt::Date>        mA;
            const bsl::vector<bdlt::Date>& A = mA;
            {
                mA.push_back(bdlt::Date(2015, 1, 5));
                mA.push_back(bdlt::Date(2015, 2, 5));
                mA.push_back(bdlt::Date(2015, 3, 5));
                mA.push_back(bdlt::Date(2015, 4, 5));
                mA.push_back(bdlt::Date(2015, 5, 5));
            }

            // 'periodDate' with non-sorted values.

            bsl::vector<bdlt::Date>        mE1;
            const bsl::vector<bdlt::Date>& E1 = mE1;
            {
                mE1.push_back(bdlt::Date(2015, 1, 5));
                mE1.push_back(bdlt::Date(2015, 3, 5));
                mE1.push_back(bdlt::Date(2015, 2, 5));
                mE1.push_back(bdlt::Date(2015, 4, 5));
                mE1.push_back(bdlt::Date(2015, 5, 5));
            }

            // 'periodDate' with non-unique values.

            bsl::vector<bdlt::Date>        mE2;
            const bsl::vector<bdlt::Date>& E2 = mE2;
            {
                mE2.push_back(bdlt::Date(2015, 1, 5));
                mE2.push_back(bdlt::Date(2015, 2, 5));
                mE2.push_back(bdlt::Date(2015, 3, 5));
                mE2.push_back(bdlt::Date(2015, 3, 5));
                mE2.push_back(bdlt::Date(2015, 4, 5));
                mE2.push_back(bdlt::Date(2015, 5, 5));
            }

            // 'periodDate' with only one value.

            bsl::vector<bdlt::Date>        mE3;
            const bsl::vector<bdlt::Date>& E3 = mE3;
            {
                mE3.push_back(bdlt::Date(2015, 1, 5));
            }

            // 'periodDate' with no values.

            bsl::vector<bdlt::Date>        mE4;
            const bsl::vector<bdlt::Date>& E4 = mE4;

            ASSERT_PASS(Util::yearsDiff(bdlt::Date(2015, 1, 5),
                                        bdlt::Date(2015, 5, 5),
                                        A,
                                        1.0));

            ASSERT_SAFE_FAIL(Util::yearsDiff(bdlt::Date(2015, 1, 5),
                                             bdlt::Date(2015, 5, 5),
                                             E1,
                                             1.0));

            ASSERT_SAFE_FAIL(Util::yearsDiff(bdlt::Date(2015, 1, 5),
                                             bdlt::Date(2015, 5, 5),
                                             E2,
                                             1.0));

            ASSERT_FAIL(Util::yearsDiff(bdlt::Date(2015, 1, 5),
                                        bdlt::Date(2015, 1, 5),
                                        E3,
                                        1.0));

            ASSERT_FAIL(Util::yearsDiff(bdlt::Date(2015, 1, 5),
                                        bdlt::Date(2015, 1, 5),
                                        E4,
                                        1.0));

            ASSERT_FAIL(Util::yearsDiff(bdlt::Date(2015, 1, 4),
                                        bdlt::Date(2015, 1, 5),
                                        A,
                                        1.0));

            ASSERT_FAIL(Util::yearsDiff(bdlt::Date(2015, 1, 5),
                                        bdlt::Date(2015, 5, 6),
                                        A,
                                        1.0));
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
        //
        // Plan:
        //: 1 Specify a set S of {pairs of dates (d1, d2) and their difference
        //:   in days D}.  For the method under test, in a loop over the
        //:   elements of S, apply the method to dates having the values d1
        //:   and d2 and confirm the result using the value D.  (C-1)
        //:
        //: 2 Also verify the result is negated when the date parameters are
        //:   reversed.  (C-2)
        //
        // Testing:
        //   int daysDiff(const bdlt::Date& bD, const bdlt::Date& eD);
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
                int d_numDays;   // result number of days
            } DATA[] = {
                //       - - - -begin- - - -   - - - end - - - - -
                //line   year   month   day    year   month   day    numDays
                //----   -----  -----  -----   -----  -----  -----   -------
                { L_,     1992,     2,     1,   1992,     3,     1,       29 },
                { L_,     1992,     2,     1,   1993,     3,     1,      394 },

                { L_,     1993,     1,     1,   1993,     2,    21,       51 },
                { L_,     1993,     1,     1,   1994,     1,     1,      365 },
                { L_,     1993,     1,    15,   1993,     2,     1,       17 },
                { L_,     1993,     2,     1,   1993,     3,     1,       28 },
                { L_,     1993,     2,     1,   1996,     2,     1,     1095 },

                { L_,     1993,     2,     1,   1993,     3,     1,       28 },
                { L_,     1993,     2,    15,   1993,     4,     1,       45 },

                { L_,     1993,     3,    15,   1993,     6,    15,       92 },
                { L_,     1993,     3,    31,   1993,     4,     1,        1 },
                { L_,     1993,     3,    31,   1993,     4,    30,       30 },
                { L_,     1993,     3,    31,   1993,    12,    31,      275 },

                { L_,     1993,     7,    15,   1993,     9,    15,       62 },

                { L_,     1993,     8,    21,   1994,     4,    11,      233 },

                { L_,     1993,    11,     1,   1994,     3,     1,      120 },

                { L_,     1993,    12,    15,   1993,    12,    30,       15 },
                { L_,     1993,    12,    15,   1993,    12,    31,       16 },
                { L_,     1993,    12,    31,   1994,     2,     1,       32 },

                { L_,     1996,     1,    15,   1996,     5,    31,      137 },

                { L_,     1998,     2,    27,   1998,     3,    27,       28 },
                { L_,     1998,     2,    28,   1998,     3,    27,       27 },

                { L_,     1999,     1,     1,   1999,     1,    29,       28 },
                { L_,     1999,     1,    29,   1999,     1,    30,        1 },
                { L_,     1999,     1,    29,   1999,     1,    31,        2 },
                { L_,     1999,     1,    29,   1999,     3,    29,       59 },
                { L_,     1999,     1,    29,   1999,     3,    30,       60 },
                { L_,     1999,     1,    29,   1999,     3,    31,       61 },
                { L_,     1999,     1,    30,   1999,     1,    31,        1 },
                { L_,     1999,     1,    30,   1999,     2,    27,       28 },
                { L_,     1999,     1,    30,   1999,     2,    28,       29 },
                { L_,     1999,     1,    30,   1999,     3,    29,       58 },
                { L_,     1999,     1,    30,   1999,     3,    30,       59 },
                { L_,     1999,     1,    30,   1999,     3,    31,       60 },
                { L_,     1999,     1,    31,   1999,     3,    29,       57 },
                { L_,     1999,     1,    31,   1999,     3,    30,       58 },
                { L_,     1999,     1,    31,   1999,     3,    31,       59 },

                { L_,     1999,     2,    27,   1999,     2,    27,        0 },
                { L_,     1999,     2,    27,   1999,     2,    28,        1 },
                { L_,     1999,     2,    28,   1999,     2,    27,       -1 },
                { L_,     1999,     2,    28,   1999,     2,    28,        0 },

                { L_,     2000,     1,    29,   2000,     1,    30,        1 },
                { L_,     2000,     1,    29,   2000,     1,    31,        2 },
                { L_,     2000,     1,    29,   2000,     3,    29,       60 },
                { L_,     2000,     1,    29,   2000,     3,    30,       61 },
                { L_,     2000,     1,    29,   2000,     3,    31,       62 },
                { L_,     2000,     1,    30,   2000,     1,    31,        1 },
                { L_,     2000,     1,    30,   2000,     2,    27,       28 },
                { L_,     2000,     1,    30,   2000,     2,    28,       29 },
                { L_,     2000,     1,    30,   2000,     2,    29,       30 },
                { L_,     2000,     1,    30,   2000,     3,    29,       59 },
                { L_,     2000,     1,    30,   2000,     3,    30,       60 },
                { L_,     2000,     1,    30,   2000,     3,    31,       61 },
                { L_,     2000,     1,    31,   2000,     3,    29,       58 },
                { L_,     2000,     1,    31,   2000,     3,    30,       59 },
                { L_,     2000,     1,    31,   2000,     3,    31,       60 },

                { L_,     2000,     2,    27,   2000,     2,    27,        0 },
                { L_,     2000,     2,    27,   2000,     2,    28,        1 },
                { L_,     2000,     2,    27,   2000,     2,    29,        2 },
                { L_,     2000,     2,    28,   2000,     2,    27,       -1 },
                { L_,     2000,     2,    28,   2000,     2,    28,        0 },
                { L_,     2000,     2,    28,   2000,     2,    29,        1 },
                { L_,     2000,     2,    29,   2000,     2,    27,       -2 },
                { L_,     2000,     2,    29,   2000,     2,    28,       -1 },
                { L_,     2000,     2,    29,   2000,     2,    29,        0 },

                { L_,     2000,     7,    29,   2000,     8,    31,       33 },
                { L_,     2000,     7,    29,   2000,     9,     1,       34 },
                { L_,     2000,     7,    30,   2000,     8,    31,       32 },
                { L_,     2000,     7,    30,   2000,     9,     1,       33 },
                { L_,     2000,     7,    31,   2000,     8,    31,       31 },
                { L_,     2000,     7,    31,   2000,     9,     1,       32 },

                { L_,     2000,     8,     1,   2000,     8,    31,       30 },
                { L_,     2000,     8,     1,   2000,     9,     1,       31 },

                { L_,     2003,     2,    28,   2004,     2,    29,      366 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (verbose) cout <<
                "\nTesting: 'daysDiff(beginDate, endDate)'" << endl;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE     = DATA[di].d_lineNum;
                const int NUM_DAYS = DATA[di].d_numDays;

                const bdlt::Date X(DATA[di].d_year1,
                                   DATA[di].d_month1,
                                   DATA[di].d_day1);
                const bdlt::Date Y(DATA[di].d_year2,
                                   DATA[di].d_month2,
                                   DATA[di].d_day2);

                if (veryVerbose) { T_;  P_(X);  P_(Y);  P_(NUM_DAYS); }
                const int RESULT = Util::daysDiff(X, Y);

                if (veryVerbose) { P(RESULT); }
                LOOP_ASSERT(LINE, NUM_DAYS == RESULT);

                // Verify the result is negated when the dates are reversed.

                const int NRESULT = Util::daysDiff(Y, X);
                LOOP_ASSERT(LINE, NRESULT == -RESULT);
            }
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
