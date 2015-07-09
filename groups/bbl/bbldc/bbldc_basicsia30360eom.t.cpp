// bbldc_basicsia30360eom.t.cpp                                       -*-C++-*-
#include <bbldc_basicsia30360eom.h>

#include <bdls_testutil.h>

#include <bdlt_date.h>

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
// the day and year difference between two dates.  The standard table-based
// test case implementation is used to verify the functionality of these
// methods.
//-----------------------------------------------------------------------------
// [ 1] int daysDiff(const bdlt::Date& bD, const bdlt::Date& eD);
// [ 2] double yearsDiff(const bdlt::Date& bD, const bdlt::Date& eD);
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
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bbldc::BasicSia30360Eom Util;

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
// 'bbldc::BasicSia30360Eom' methods.  First, create two 'bdlt::Date'
// variables, 'd1' and 'd2':
//..
    const bdlt::Date d1(2004,  9, 30);
    const bdlt::Date d2(2004, 12, 31);
//..
// Then, compute the day count between the two dates:
//..
    const int daysDiff = bbldc::BasicSia30360Eom::daysDiff(d1, d2);
    ASSERT(90 == daysDiff);
//..
// Finally, compute the year fraction between the two dates:
//..
    const double yearsDiff = bbldc::BasicSia30360Eom::yearsDiff(d1, d2);
    ASSERT(0.25 == yearsDiff);
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
        //
        // Plan:
        //: 1 Specify a set S of {pairs of dates (d1, d2) and their difference
        //:   in years D}.  For the method under test, in a loop over the
        //:   elements of S, apply the method to dates having the values d1
        //:   and d2 and confirm the result using the value D with a fuzzy
        //:   comparison (since the return value is a floating-point number).
        //:   (C-1)
        //:
        //: 2 Also verify the result is negated when the date parameters are
        //:   reversed.  (C-2)
        //
        // Testing:
        //   double yearsDiff(const bdlt::Date& bD, const bdlt::Date& eD);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'yearsDiff'" << endl
                          << "===================" << endl;

        {
            static const struct {
                int    d_lineNum;   // source line number
                int    d_year1;     // date1 year
                int    d_month1;    // date1 month
                int    d_day1;      // date1 day
                int    d_year2;     // date2 year
                int    d_month2;    // date2 month
                int    d_day2;      // date2 day
                double d_numYears;  // result number of years
            } DATA[] = {
                //       - - - -first- - - -   - - - second- - - -
                //line   year   month   day    year   month   day    numYears
                //----   -----  -----  -----   -----  -----  -----   --------
                { L_,     1993,     2,     1,   1993,     3,     1,   0.0833 },
                { L_,     1996,     1,    15,   1996,     5,    31,   0.3778 },
                { L_,     2000,     2,    27,   2000,     2,    27,   0.0000 },
                { L_,     2000,     2,    27,   2000,     2,    28,   0.0028 },
                { L_,     2000,     2,    27,   2000,     2,    29,   0.0056 },
                { L_,     2000,     2,    28,   2000,     2,    27,  -0.0028 },
                { L_,     2000,     2,    28,   2000,     2,    28,   0.0000 },
                { L_,     2000,     2,    28,   2000,     2,    29,   0.0028 },
                { L_,     2000,     2,    29,   2000,     2,    27,  -0.0056 },
                { L_,     2000,     2,    29,   2000,     2,    28,  -0.0028 },
                { L_,     2000,     2,    29,   2000,     2,    29,   0.0000 },
                { L_,     2003,     2,    28,   2004,     2,    29,   1.0000 },

                { L_,     2000,     1,    29,   2000,     1,    30,   0.0028 },
                { L_,     2000,     1,    29,   2000,     1,    31,   0.0056 },
                { L_,     2000,     1,    29,   2000,     3,    29,   0.1667 },
                { L_,     2000,     1,    29,   2000,     3,    30,   0.1694 },
                { L_,     2000,     1,    29,   2000,     3,    31,   0.1722 },
                { L_,     2000,     1,    30,   2000,     1,    31,        0 },
                { L_,     2000,     1,    30,   2000,     2,    27,   0.0750 },
                { L_,     2000,     1,    30,   2000,     2,    28,   0.0778 },
                { L_,     2000,     1,    30,   2000,     2,    29,   0.0806 },
                { L_,     2000,     1,    30,   2000,     3,    29,   0.1639 },
                { L_,     2000,     1,    30,   2000,     3,    30,   0.1667 },
                { L_,     2000,     1,    30,   2000,     3,    31,   0.1667 },
                { L_,     2000,     1,    31,   2000,     3,    29,   0.1639 },
                { L_,     2000,     1,    31,   2000,     3,    30,   0.1667 },
                { L_,     2000,     1,    31,   2000,     3,    31,   0.1667 },

                { L_,     2000,     1,    29,   2004,     1,    30,   4.0028 },
                { L_,     2000,     1,    29,   2004,     1,    31,   4.0056 },
                { L_,     2000,     1,    29,   2004,     3,    29,   4.1667 },
                { L_,     2000,     1,    29,   2004,     3,    30,   4.1694 },
                { L_,     2000,     1,    29,   2004,     3,    31,   4.1722 },
                { L_,     2000,     1,    30,   2004,     1,    31,   4.0000 },
                { L_,     2000,     1,    30,   2004,     2,    27,   4.0750 },
                { L_,     2000,     1,    30,   2004,     2,    28,   4.0778 },
                { L_,     2000,     1,    30,   2004,     2,    29,   4.0806 },
                { L_,     2000,     1,    30,   2004,     3,    29,   4.1639 },
                { L_,     2000,     1,    30,   2004,     3,    30,   4.1667 },
                { L_,     2000,     1,    30,   2004,     3,    31,   4.1667 },
                { L_,     2000,     1,    31,   2004,     3,    29,   4.1639 },
                { L_,     2000,     1,    31,   2004,     3,    30,   4.1667 },
                { L_,     2000,     1,    31,   2004,     3,    31,   4.1667 },

                { L_,     2004,     1,    29,   2000,     1,    30,  -3.9972 },
                { L_,     2004,     1,    29,   2000,     1,    31,  -3.9972 },
                { L_,     2004,     1,    29,   2000,     3,    29,  -3.8333 },
                { L_,     2004,     1,    29,   2000,     3,    30,  -3.8306 },
                { L_,     2004,     1,    29,   2000,     3,    31,  -3.8306 },
                { L_,     2004,     1,    30,   2000,     1,    31,  -4.0000 },
                { L_,     2004,     1,    30,   2000,     2,    27,  -3.9250 },
                { L_,     2004,     1,    30,   2000,     2,    28,  -3.9222 },
                { L_,     2004,     1,    30,   2000,     2,    29,  -3.9167 },
                { L_,     2004,     1,    30,   2000,     3,    29,  -3.8361 },
                { L_,     2004,     1,    30,   2000,     3,    30,  -3.8333 },
                { L_,     2004,     1,    30,   2000,     3,    31,  -3.8333 },
                { L_,     2004,     1,    31,   2000,     3,    29,  -3.8389 },
                { L_,     2004,     1,    31,   2000,     3,    30,  -3.8333 },
                { L_,     2004,     1,    31,   2000,     3,    31,  -3.8333 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (verbose) cout <<
                "\nTesting: 'yearsDiff(beginDate, endDate)'" << endl;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int    LINE      = DATA[di].d_lineNum;
                const double NUM_YEARS = DATA[di].d_numYears;

                const bdlt::Date X(DATA[di].d_year1,
                                   DATA[di].d_month1,
                                   DATA[di].d_day1);
                const bdlt::Date Y(DATA[di].d_year2,
                                   DATA[di].d_month2,
                                   DATA[di].d_day2);

                if (veryVerbose) { T_;  P_(X);  P_(Y);  P_(NUM_YEARS); }
                const double RESULT = Util::yearsDiff(X, Y);

                if (veryVerbose) { P(RESULT); }
                const double diff = NUM_YEARS - RESULT;
                LOOP_ASSERT(LINE, -0.00005 <= diff && diff <= 0.00005);

                // Verify the result is negated when the dates are reversed.

                const double NRESULT = Util::yearsDiff(Y, X);
                const double sum     = RESULT + NRESULT;
                LOOP_ASSERT(LINE, -1.0e-15 <= sum && sum <= 1.0e-15);
            }
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
        {
            static const struct {
                int d_lineNum;   // source line number
                int d_year1;     // date1 year
                int d_month1;    // date1 month
                int d_day1;      // date1 day
                int d_year2;     // date2 year
                int d_month2;    // date2 month
                int d_day2;      // date2 day
                int d_numDays;   // result number of days
            } DATA[] = {
                //       - - - -first- - - -   - - - second- - - -
                //line   year   month   day    year   month   day    numDays
                //----   -----  -----  -----   -----  -----  -----   -------
                { L_,     1992,     2,     1,   1992,     3,     1,       30 },

                { L_,     1993,     1,     1,   1993,     2,    21,       50 },
                { L_,     1993,     1,     1,   1994,     1,     1,      360 },
                { L_,     1993,     1,    15,   1993,     2,     1,       16 },

                { L_,     1993,     2,     1,   1993,     3,     1,       30 },
                { L_,     1993,     2,    15,   1993,     4,     1,       46 },

                { L_,     1993,     3,    15,   1993,     6,    15,       90 },
                { L_,     1993,     3,    31,   1993,     4,     1,        1 },
                { L_,     1993,     3,    31,   1993,     4,    30,       30 },
                { L_,     1993,     3,    31,   1993,    12,    31,      270 },

                { L_,     1993,     7,    15,   1993,     9,    15,       60 },

                { L_,     1993,     8,    21,   1994,     4,    11,      230 },

                { L_,     1993,    11,     1,   1994,     3,     1,      120 },

                { L_,     1993,    12,    15,   1993,    12,    30,       15 },
                { L_,     1993,    12,    15,   1993,    12,    31,       16 },
                { L_,     1993,    12,    31,   1994,     2,     1,       31 },

                { L_,     1996,     1,    15,   1996,     5,    31,      136 },

                { L_,     1998,     2,    27,   1998,     3,    27,       30 },
                { L_,     1998,     2,    28,   1998,     3,    27,       27 },

                { L_,     1999,     1,     1,   1999,     1,    29,       28 },
                { L_,     1999,     1,    29,   1999,     1,    30,        1 },
                { L_,     1999,     1,    29,   1999,     1,    31,        2 },
                { L_,     1999,     1,    29,   1999,     3,    29,       60 },
                { L_,     1999,     1,    29,   1999,     3,    30,       61 },
                { L_,     1999,     1,    29,   1999,     3,    31,       62 },
                { L_,     1999,     1,    30,   1999,     1,    31,        0 },
                { L_,     1999,     1,    30,   1999,     2,    27,       27 },
                { L_,     1999,     1,    30,   1999,     2,    28,       28 },
                { L_,     1999,     1,    30,   1999,     3,    29,       59 },
                { L_,     1999,     1,    30,   1999,     3,    30,       60 },
                { L_,     1999,     1,    30,   1999,     3,    31,       60 },
                { L_,     1999,     1,    31,   1999,     3,    29,       59 },
                { L_,     1999,     1,    31,   1999,     3,    30,       60 },
                { L_,     1999,     1,    31,   1999,     3,    31,       60 },

                { L_,     1999,     2,    27,   1999,     2,    27,        0 },
                { L_,     1999,     2,    27,   1999,     2,    28,        1 },
                { L_,     1999,     2,    28,   1999,     2,    27,       -1 },
                { L_,     1999,     2,    28,   1999,     2,    28,        0 },

                { L_,     2000,     1,    29,   2000,     1,    30,        1 },
                { L_,     2000,     1,    29,   2000,     1,    31,        2 },
                { L_,     2000,     1,    29,   2000,     3,    29,       60 },
                { L_,     2000,     1,    29,   2000,     3,    30,       61 },
                { L_,     2000,     1,    29,   2000,     3,    31,       62 },
                { L_,     2000,     1,    30,   2000,     1,    31,        0 },
                { L_,     2000,     1,    30,   2000,     2,    27,       27 },
                { L_,     2000,     1,    30,   2000,     2,    28,       28 },
                { L_,     2000,     1,    30,   2000,     2,    29,       29 },
                { L_,     2000,     1,    30,   2000,     3,    29,       59 },
                { L_,     2000,     1,    30,   2000,     3,    30,       60 },
                { L_,     2000,     1,    30,   2000,     3,    31,       60 },
                { L_,     2000,     1,    31,   2000,     3,    29,       59 },
                { L_,     2000,     1,    31,   2000,     3,    30,       60 },
                { L_,     2000,     1,    31,   2000,     3,    31,       60 },

                { L_,     2000,     1,    29,   2004,     1,    30,     1441 },
                { L_,     2000,     1,    29,   2004,     1,    31,     1442 },
                { L_,     2000,     1,    29,   2004,     3,    29,     1500 },
                { L_,     2000,     1,    29,   2004,     3,    30,     1501 },
                { L_,     2000,     1,    29,   2004,     3,    31,     1502 },
                { L_,     2000,     1,    30,   2004,     1,    31,     1440 },
                { L_,     2000,     1,    30,   2004,     2,    27,     1467 },
                { L_,     2000,     1,    30,   2004,     2,    28,     1468 },
                { L_,     2000,     1,    30,   2004,     2,    29,     1469 },
                { L_,     2000,     1,    30,   2004,     3,    29,     1499 },
                { L_,     2000,     1,    30,   2004,     3,    30,     1500 },
                { L_,     2000,     1,    30,   2004,     3,    31,     1500 },
                { L_,     2000,     1,    31,   2004,     3,    29,     1499 },
                { L_,     2000,     1,    31,   2004,     3,    30,     1500 },
                { L_,     2000,     1,    31,   2004,     3,    31,     1500 },

                { L_,     2004,     1,    29,   2000,     1,    30,    -1439 },
                { L_,     2004,     1,    29,   2000,     1,    31,    -1439 },
                { L_,     2004,     1,    29,   2000,     3,    29,    -1380 },
                { L_,     2004,     1,    29,   2000,     3,    30,    -1379 },
                { L_,     2004,     1,    29,   2000,     3,    31,    -1379 },
                { L_,     2004,     1,    30,   2000,     1,    31,    -1440 },
                { L_,     2004,     1,    30,   2000,     2,    27,    -1413 },
                { L_,     2004,     1,    30,   2000,     2,    28,    -1412 },
                { L_,     2004,     1,    30,   2000,     2,    29,    -1410 },
                { L_,     2004,     1,    30,   2000,     3,    29,    -1381 },
                { L_,     2004,     1,    30,   2000,     3,    30,    -1380 },
                { L_,     2004,     1,    30,   2000,     3,    31,    -1380 },
                { L_,     2004,     1,    31,   2000,     3,    29,    -1382 },
                { L_,     2004,     1,    31,   2000,     3,    30,    -1380 },
                { L_,     2004,     1,    31,   2000,     3,    31,    -1380 },

                { L_,     2000,     2,    27,   2000,     2,    27,        0 },
                { L_,     2000,     2,    27,   2000,     2,    28,        1 },
                { L_,     2000,     2,    27,   2000,     2,    29,        2 },
                { L_,     2000,     2,    28,   2000,     2,    27,       -1 },
                { L_,     2000,     2,    28,   2000,     2,    28,        0 },
                { L_,     2000,     2,    28,   2000,     2,    29,        1 },
                { L_,     2000,     2,    29,   2000,     2,    27,       -2 },
                { L_,     2000,     2,    29,   2000,     2,    28,       -1 },
                { L_,     2000,     2,    29,   2000,     2,    29,        0 },

                { L_,     2000,     7,    29,   2000,     8,    31,       32 },
                { L_,     2000,     7,    29,   2000,     9,     1,       32 },
                { L_,     2000,     7,    30,   2000,     8,    31,       30 },
                { L_,     2000,     7,    30,   2000,     9,     1,       31 },
                { L_,     2000,     7,    31,   2000,     8,    31,       30 },
                { L_,     2000,     7,    31,   2000,     9,     1,       31 },

                { L_,     2000,     8,     1,   2000,     8,    31,       30 },
                { L_,     2000,     8,     1,   2000,     9,     1,       30 },

                { L_,     2003,     2,    28,   2004,     2,    29,      360 },
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
