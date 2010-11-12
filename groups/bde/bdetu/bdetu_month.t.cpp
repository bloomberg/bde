// bdetu_month.t.cpp        -*-C++-*-

#include <bdetu_month.h>

#include <bdet_month.h>
#include <bdet_date.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test is a utility for month calculations.  In this test
// driver, each function is tested in a separate test case.  Cross-product
// partitioning is used as the data selection method.  A white-box test
// approach is employed because of the implementation's simplicity.  All the
// data that would be used in a black-box category-partitioning scheme will
// also fall under the white-box partitions.
//
// The following abbreviations are used throughout this documentation:
//     c   = const
//     b_M = bdet_Month::Month
//     b_D = bdet_Date
//-----------------------------------------------------------------------------
// FUNCTIONS
// [14] int numMonthsInRange(b_M month, c b_D& date1, c b_D& date2);
// [ 3] b_D nextMonth(b_M month, c b_D& date);
// [ 5] b_D ceilMonth(b_M month, c b_D& date);
// [ 4] b_D previousMonth(b_M month, c b_D& date);
// [ 6] b_D floorMonth(b_M month, c b_D& date);
// [ 7] b_D adjustMonth(b_M month, c b_D& date, int count);
// [ 8] b_D ceilAdjustMonth(b_M month, c b_D& date, int count);
// [ 9] b_D ceilAdjustMonth2(b_M month, c b_D& date, int count);
// [10] b_D floorAdjustMonth(b_M month, c b_D& date, int count);
// [11] b_D floorAdjustMonth2(b_M month, c b_D& date, int count);
// [12] b_D ceilOrPreviousMonthInYear(b_M month, c b_D& date);
// [13] b_D floorOrNextMonthInYear(b_M month, c b_D& date);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [15] USAGE EXAMPLE
// [ 2] b_D parseDate(const char *str);
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_  cout << "\t" << flush;            // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bdetu_Month       Util;
typedef bdet_Month::Month Month;
typedef bdet_Date         Date;

const Month JAN = bdet_Month::BDET_JAN;
const Month FEB = bdet_Month::BDET_FEB;
const Month MAR = bdet_Month::BDET_MAR;
const Month APR = bdet_Month::BDET_APR;
const Month MAY = bdet_Month::BDET_MAY;
const Month JUN = bdet_Month::BDET_JUN;
const Month JUL = bdet_Month::BDET_JUL;
const Month AUG = bdet_Month::BDET_AUG;
const Month SEP = bdet_Month::BDET_SEP;
const Month OCT = bdet_Month::BDET_OCT;
const Month NOV = bdet_Month::BDET_NOV;
const Month DEC = bdet_Month::BDET_DEC;

//=============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static bdet_Date parseDate(const char *str)
    // Parse the specified 'str' and return a 'bdet_Date' corresponding to
    // 'str'.  The behavior is undefined unless 'str' represents a valid date.
    // Note that 'str' must be in the format 'YYYY,MM,DD' where 'YYYY' is any
    // value in the range [0001..9999], 'MM' is any value in the range
    // [01..12], and 'DD' is any value in the range [01..31].
{
    int year, month, day;

    // Parse year.

    ASSERT('0' <= *str && *str <= '9');
    year = (*str-'0') * 1000;
    ++str;
    ASSERT('0' <= *str && *str <= '9');
    year += (*str-'0') * 100;
    ++str;
    ASSERT('0' <= *str && *str <= '9');
    year += (*str-'0') * 10;
    ++str;
    ASSERT('0' <= *str && *str <= '9');
    year += (*str-'0');
    ++str;

    ASSERT(',' == *str);
    ++str;

    // Parse month.

    ASSERT('0' <= *str && *str <= '9');
    month = (*str-'0') * 10;
    ++str;
    ASSERT('0' <= *str && *str <= '9');
    month += (*str-'0');
    ++str;

    ASSERT(',' == *str);
    ++str;

    // Parse day.

    ASSERT('0' <= *str && *str <= '9');
    day = (*str-'0') * 10;
    ++str;
    ASSERT('0' <= *str && *str <= '9');
    day += (*str-'0');
    ++str;

    ASSERT(0 == *str);
    ASSERT(bdet_Date::isValid(year, month, day));

    return bdet_Date(year, month, day);
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    static int test = argc > 1 ? atoi(argv[1]) : 0;
    static int verbose = argc > 2;
    static int veryVerbose = argc > 3;
    // static int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 15: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage example from the component's header file.
        //
        // Concerns:
        //   We want to make sure that the usage example from the component's
        //   header file compiles and runs correctly.
        //
        // Plan:
        //   Copy and paste the usage example from the component's header file,
        //   changing 'assert' to 'ASSERT'.
        //
        // Testing:
        //   Usage Example.
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;
        // TBD
        if (verbose) cout << "\nEnd of Usage Example Test." << endl;
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'numMonthsInRange' FUNCTION
        //   This will test the 'numMonthsInRange' function.
        //
        // Concerns:
        //   1. When date1 is later than date2, the function must return 0, as
        //      stated in the function-level documentation.
        //   2. The following boundary conditions must be tested:
        //       o date1.month() == month.
        //       o date2.month() == month.
        //       o date1.month() == date2.month() != month.
        //       o date1.month() == date2.month() == month.
        //   3. The number of years between 'date1' and 'date2' is included in
        //      the returned result.
        //
        // Plan:
        //   For a set of test data, exercise the 'numMonthsInRange' function
        //   and check that the returned value is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //      date1.day  date2.day   monthEqual   dateSameMonth  numYears
        //      ---------  ---------   ----------   -------------  --------
        //
        //                                                          | -1 |
        //        |  1 |     |  1 |     | true  |     | true  |     |  0 |
        //        | 15 |  X  | 15 |  X  |       |  X  |       |  X  |  1 |
        //        | 28 |     | 28 |     | false |     | false |     |  2 |
        //                                                          |  3 |
        //
        //   where:
        //       o monthEqual    : month == date1.month().
        //       o dateSameMonth : date1.month() == date2.month().
        //       o numYears      : number of years between date1 and
        //                         floorMonth(date1.month(), date2).
        //
        //   The three different values for each date's day value checks that,
        //   when date1.month() == date2.month() and
        //   date1.year() == date2.year(), if date1.day() > date2.day(), the
        //   function still returns 0.
        //
        //   The 'dateSameMonth' and 'monthEqual' boolean flags will check that
        //   the 'bdet_Month::Month(m2) == month' expressions are correctly
        //   added to the returned result.
        //
        //   The 'numYears' value will check that the 'y2 - y1' expression is
        //   correctly added to the returned result.
        //
        //   A '-1' value is included in 'numYears' to test the case where
        //   [date1.year(), date1.month()] < [date2.year, date2.month].
        //
        //   Note that the values of 'month', 'date1.month()', 'date1.year()',
        //   'date2.month()', and 'date2.year()' can be any arbitrary values
        //   that conform to the scenario being tested.
        //
        // Testing:
        //   int numMonthsInRange(b_M month, c b_D& date1, c b_D& date2);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'numMonthsInRange' Function"
                          << "\n===================================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Month       d_month;    // month
            const char *d_date1;    // date1
            const char *d_date2;    // date2
            int         d_result;   // expected result
        } DATA[] = {
            //line    month    date1            date2            result
            //----    -----    -----            -----            ------

            // monthEqual = true, dateSameMonth = true, numYears = -1
            { L_,     JAN,     "2001,01,01",    "2000,01,01",    0       },
            { L_,     JAN,     "2001,01,01",    "2000,01,15",    0       },
            { L_,     JAN,     "2001,01,01",    "2000,01,28",    0       },
            { L_,     FEB,     "2001,02,15",    "2000,02,01",    0       },
            { L_,     FEB,     "2001,02,15",    "2000,02,15",    0       },
            { L_,     FEB,     "2001,02,15",    "2000,02,28",    0       },
            { L_,     MAR,     "2001,03,28",    "2000,03,01",    0       },
            { L_,     MAR,     "2001,03,28",    "2000,03,15",    0       },
            { L_,     MAR,     "2001,03,28",    "2000,03,28",    0       },

            // monthEqual = false, dateSameMonth = true, numYears = -1
            { L_,     APR,     "2001,01,01",    "2000,01,01",    0       },
            { L_,     APR,     "2001,01,01",    "2000,01,15",    0       },
            { L_,     APR,     "2001,01,01",    "2000,01,28",    0       },
            { L_,     MAY,     "2001,02,15",    "2000,02,01",    0       },
            { L_,     MAY,     "2001,02,15",    "2000,02,15",    0       },
            { L_,     MAY,     "2001,02,15",    "2000,02,28",    0       },
            { L_,     JUN,     "2001,03,28",    "2000,03,01",    0       },
            { L_,     JUN,     "2001,03,28",    "2000,03,15",    0       },
            { L_,     JUN,     "2001,03,28",    "2000,03,28",    0       },

            // monthEqual = true, dateSameMonth = false, numYears = -1
            { L_,     JUL,     "2001,07,01",    "2000,10,01",    0       },
            { L_,     JUL,     "2001,07,01",    "2000,10,15",    0       },
            { L_,     JUL,     "2001,07,01",    "2000,10,28",    0       },
            { L_,     AUG,     "2001,08,15",    "2000,10,01",    0       },
            { L_,     AUG,     "2001,08,15",    "2000,10,15",    0       },
            { L_,     AUG,     "2001,08,15",    "2000,10,28",    0       },
            { L_,     SEP,     "2001,09,28",    "2000,10,01",    0       },
            { L_,     SEP,     "2001,09,28",    "2000,10,15",    0       },
            { L_,     SEP,     "2001,09,28",    "2000,10,28",    0       },

            // monthEqual = false, dateSameMonth = false, numYears = -1
            { L_,     OCT,     "2001,07,01",    "2000,10,01",    0       },
            { L_,     OCT,     "2001,07,01",    "2000,10,15",    0       },
            { L_,     OCT,     "2001,07,01",    "2000,10,28",    0       },
            { L_,     NOV,     "2001,08,15",    "2000,10,01",    0       },
            { L_,     NOV,     "2001,08,15",    "2000,10,15",    0       },
            { L_,     NOV,     "2001,08,15",    "2000,10,28",    0       },
            { L_,     DEC,     "2001,09,28",    "2000,10,01",    0       },
            { L_,     DEC,     "2001,09,28",    "2000,10,15",    0       },
            { L_,     DEC,     "2001,09,28",    "2000,10,28",    0       },

            // monthEqual = true, dateSameMonth = true, numYears = 0
            { L_,     JAN,     "2001,01,01",    "2001,01,01",    1       },
            { L_,     JAN,     "2001,01,01",    "2001,01,15",    1       },
            { L_,     JAN,     "2001,01,01",    "2001,01,28",    1       },
            { L_,     FEB,     "2001,02,15",    "2001,02,01",    0       },
            { L_,     FEB,     "2001,02,15",    "2001,02,15",    1       },
            { L_,     FEB,     "2001,02,15",    "2001,02,28",    1       },
            { L_,     MAR,     "2001,03,28",    "2001,03,01",    0       },
            { L_,     MAR,     "2001,03,28",    "2001,03,15",    0       },
            { L_,     MAR,     "2001,03,28",    "2001,03,28",    1       },

            // monthEqual = false, dateSameMonth = true, numYears = 0
            { L_,     APR,     "2001,01,01",    "2001,01,01",    0       },
            { L_,     APR,     "2001,01,01",    "2001,01,15",    0       },
            { L_,     APR,     "2001,01,01",    "2001,01,28",    0       },
            { L_,     MAY,     "2001,02,15",    "2001,02,01",    0       },
            { L_,     MAY,     "2001,02,15",    "2001,02,15",    0       },
            { L_,     MAY,     "2001,02,15",    "2001,02,28",    0       },
            { L_,     JUN,     "2001,03,28",    "2001,03,01",    0       },
            { L_,     JUN,     "2001,03,28",    "2001,03,15",    0       },
            { L_,     JUN,     "2001,03,28",    "2001,03,28",    0       },

            // monthEqual = true, dateSameMonth = false, numYears = 0
            { L_,     JUL,     "2001,07,01",    "2001,10,01",    1       },
            { L_,     JUL,     "2001,07,01",    "2001,10,15",    1       },
            { L_,     JUL,     "2001,07,01",    "2001,10,28",    1       },
            { L_,     AUG,     "2001,08,15",    "2001,10,01",    1       },
            { L_,     AUG,     "2001,08,15",    "2001,10,15",    1       },
            { L_,     AUG,     "2001,08,15",    "2001,10,28",    1       },
            { L_,     SEP,     "2001,09,28",    "2001,10,01",    1       },
            { L_,     SEP,     "2001,09,28",    "2001,10,15",    1       },
            { L_,     SEP,     "2001,09,28",    "2001,10,28",    1       },

            // monthEqual = false, dateSameMonth = false, numYears = 0
            { L_,     OCT,     "2001,07,01",    "2001,10,01",    1       },
            { L_,     OCT,     "2001,07,01",    "2001,10,15",    1       },
            { L_,     OCT,     "2001,07,01",    "2001,10,28",    1       },
            { L_,     NOV,     "2001,08,15",    "2001,10,01",    0       },
            { L_,     NOV,     "2001,08,15",    "2001,10,15",    0       },
            { L_,     NOV,     "2001,08,15",    "2001,10,28",    0       },
            { L_,     DEC,     "2001,09,28",    "2001,10,01",    0       },
            { L_,     DEC,     "2001,09,28",    "2001,10,15",    0       },
            { L_,     DEC,     "2001,09,28",    "2001,10,28",    0       },

            // monthEqual = true, dateSameMonth = true, numYears = 1
            { L_,     JAN,     "2001,01,01",    "2002,01,01",    2       },
            { L_,     JAN,     "2001,01,01",    "2002,01,15",    2       },
            { L_,     JAN,     "2001,01,01",    "2002,01,28",    2       },
            { L_,     FEB,     "2001,02,15",    "2002,02,01",    2       },
            { L_,     FEB,     "2001,02,15",    "2002,02,15",    2       },
            { L_,     FEB,     "2001,02,15",    "2002,02,28",    2       },
            { L_,     MAR,     "2001,03,28",    "2002,03,01",    2       },
            { L_,     MAR,     "2001,03,28",    "2002,03,15",    2       },
            { L_,     MAR,     "2001,03,28",    "2002,03,28",    2       },

            // monthEqual = false, dateSameMonth = true, numYears = 1
            { L_,     APR,     "2001,01,01",    "2002,01,01",    1       },
            { L_,     APR,     "2001,01,01",    "2002,01,15",    1       },
            { L_,     APR,     "2001,01,01",    "2002,01,28",    1       },
            { L_,     MAY,     "2001,02,15",    "2002,02,01",    1       },
            { L_,     MAY,     "2001,02,15",    "2002,02,15",    1       },
            { L_,     MAY,     "2001,02,15",    "2002,02,28",    1       },
            { L_,     JUN,     "2001,03,28",    "2002,03,01",    1       },
            { L_,     JUN,     "2001,03,28",    "2002,03,15",    1       },
            { L_,     JUN,     "2001,03,28",    "2002,03,28",    1       },

            // monthEqual = true, dateSameMonth = false, numYears = 1
            { L_,     JUL,     "2001,07,01",    "2002,10,01",    2       },
            { L_,     JUL,     "2001,07,01",    "2002,10,15",    2       },
            { L_,     JUL,     "2001,07,01",    "2002,10,28",    2       },
            { L_,     AUG,     "2001,08,15",    "2002,10,01",    2       },
            { L_,     AUG,     "2001,08,15",    "2002,10,15",    2       },
            { L_,     AUG,     "2001,08,15",    "2002,10,28",    2       },
            { L_,     SEP,     "2001,09,28",    "2002,10,01",    2       },
            { L_,     SEP,     "2001,09,28",    "2002,10,15",    2       },
            { L_,     SEP,     "2001,09,28",    "2002,10,28",    2       },

            // monthEqual = false, dateSameMonth = false, numYears = 1
            { L_,     OCT,     "2001,07,01",    "2002,10,01",    2       },
            { L_,     OCT,     "2001,07,01",    "2002,10,15",    2       },
            { L_,     OCT,     "2001,07,01",    "2002,10,28",    2       },
            { L_,     NOV,     "2001,08,15",    "2002,10,01",    1       },
            { L_,     NOV,     "2001,08,15",    "2002,10,15",    1       },
            { L_,     NOV,     "2001,08,15",    "2002,10,28",    1       },
            { L_,     DEC,     "2001,09,28",    "2002,10,01",    1       },
            { L_,     DEC,     "2001,09,28",    "2002,10,15",    1       },
            { L_,     DEC,     "2001,09,28",    "2002,10,28",    1       },

            // monthEqual = true, dateSameMonth = true, numYears = 2
            { L_,     JAN,     "2001,01,01",    "2003,01,01",    3       },
            { L_,     JAN,     "2001,01,01",    "2003,01,15",    3       },
            { L_,     JAN,     "2001,01,01",    "2003,01,28",    3       },
            { L_,     FEB,     "2001,02,15",    "2003,02,01",    3       },
            { L_,     FEB,     "2001,02,15",    "2003,02,15",    3       },
            { L_,     FEB,     "2001,02,15",    "2003,02,28",    3       },
            { L_,     MAR,     "2001,03,28",    "2003,03,01",    3       },
            { L_,     MAR,     "2001,03,28",    "2003,03,15",    3       },
            { L_,     MAR,     "2001,03,28",    "2003,03,28",    3       },

            // monthEqual = false, dateSameMonth = true, numYears = 2
            { L_,     APR,     "2001,01,01",    "2003,01,01",    2       },
            { L_,     APR,     "2001,01,01",    "2003,01,15",    2       },
            { L_,     APR,     "2001,01,01",    "2003,01,28",    2       },
            { L_,     MAY,     "2001,02,15",    "2003,02,01",    2       },
            { L_,     MAY,     "2001,02,15",    "2003,02,15",    2       },
            { L_,     MAY,     "2001,02,15",    "2003,02,28",    2       },
            { L_,     JUN,     "2001,03,28",    "2003,03,01",    2       },
            { L_,     JUN,     "2001,03,28",    "2003,03,15",    2       },
            { L_,     JUN,     "2001,03,28",    "2003,03,28",    2       },

            // monthEqual = true, dateSameMonth = false, numYears = 2
            { L_,     JUL,     "2001,07,01",    "2003,10,01",    3       },
            { L_,     JUL,     "2001,07,01",    "2003,10,15",    3       },
            { L_,     JUL,     "2001,07,01",    "2003,10,28",    3       },
            { L_,     AUG,     "2001,08,15",    "2003,10,01",    3       },
            { L_,     AUG,     "2001,08,15",    "2003,10,15",    3       },
            { L_,     AUG,     "2001,08,15",    "2003,10,28",    3       },
            { L_,     SEP,     "2001,09,28",    "2003,10,01",    3       },
            { L_,     SEP,     "2001,09,28",    "2003,10,15",    3       },
            { L_,     SEP,     "2001,09,28",    "2003,10,28",    3       },

            // monthEqual = false, dateSameMonth = false, numYears = 2
            { L_,     OCT,     "2001,07,01",    "2003,10,01",    3       },
            { L_,     OCT,     "2001,07,01",    "2003,10,15",    3       },
            { L_,     OCT,     "2001,07,01",    "2003,10,28",    3       },
            { L_,     NOV,     "2001,08,15",    "2003,10,01",    2       },
            { L_,     NOV,     "2001,08,15",    "2003,10,15",    2       },
            { L_,     NOV,     "2001,08,15",    "2003,10,28",    2       },
            { L_,     DEC,     "2001,09,28",    "2003,10,01",    2       },
            { L_,     DEC,     "2001,09,28",    "2003,10,15",    2       },
            { L_,     DEC,     "2001,09,28",    "2003,10,28",    2       },

            // monthEqual = true, dateSameMonth = true, numYears = 3
            { L_,     JAN,     "2001,01,01",    "2004,01,01",    4       },
            { L_,     JAN,     "2001,01,01",    "2004,01,15",    4       },
            { L_,     JAN,     "2001,01,01",    "2004,01,28",    4       },
            { L_,     FEB,     "2001,02,15",    "2004,02,01",    4       },
            { L_,     FEB,     "2001,02,15",    "2004,02,15",    4       },
            { L_,     FEB,     "2001,02,15",    "2004,02,28",    4       },
            { L_,     MAR,     "2001,03,28",    "2004,03,01",    4       },
            { L_,     MAR,     "2001,03,28",    "2004,03,15",    4       },
            { L_,     MAR,     "2001,03,28",    "2004,03,28",    4       },

            // monthEqual = false, dateSameMonth = true, numYears = 3
            { L_,     APR,     "2001,01,01",    "2004,01,01",    3       },
            { L_,     APR,     "2001,01,01",    "2004,01,15",    3       },
            { L_,     APR,     "2001,01,01",    "2004,01,28",    3       },
            { L_,     MAY,     "2001,02,15",    "2004,02,01",    3       },
            { L_,     MAY,     "2001,02,15",    "2004,02,15",    3       },
            { L_,     MAY,     "2001,02,15",    "2004,02,28",    3       },
            { L_,     JUN,     "2001,03,28",    "2004,03,01",    3       },
            { L_,     JUN,     "2001,03,28",    "2004,03,15",    3       },
            { L_,     JUN,     "2001,03,28",    "2004,03,28",    3       },

            // monthEqual = true, dateSameMonth = false, numYears = 3
            { L_,     JUL,     "2001,07,01",    "2004,10,01",    4       },
            { L_,     JUL,     "2001,07,01",    "2004,10,15",    4       },
            { L_,     JUL,     "2001,07,01",    "2004,10,28",    4       },
            { L_,     AUG,     "2001,08,15",    "2004,10,01",    4       },
            { L_,     AUG,     "2001,08,15",    "2004,10,15",    4       },
            { L_,     AUG,     "2001,08,15",    "2004,10,28",    4       },
            { L_,     SEP,     "2001,09,28",    "2004,10,01",    4       },
            { L_,     SEP,     "2001,09,28",    "2004,10,15",    4       },
            { L_,     SEP,     "2001,09,28",    "2004,10,28",    4       },

            // monthEqual = false, dateSameMonth = false, numYears = 3
            { L_,     OCT,     "2001,07,01",    "2004,10,01",    4       },
            { L_,     OCT,     "2001,07,01",    "2004,10,15",    4       },
            { L_,     OCT,     "2001,07,01",    "2004,10,28",    4       },
            { L_,     NOV,     "2001,08,15",    "2004,10,01",    3       },
            { L_,     NOV,     "2001,08,15",    "2004,10,15",    3       },
            { L_,     NOV,     "2001,08,15",    "2004,10,28",    3       },
            { L_,     DEC,     "2001,09,28",    "2004,10,01",    3       },
            { L_,     DEC,     "2001,09,28",    "2004,10,15",    3       },
            { L_,     DEC,     "2001,09,28",    "2004,10,28",    3       },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const Month MONTH  = DATA[i].d_month;
            const Date  DATE1  = parseDate(DATA[i].d_date1);
            const Date  DATE2  = parseDate(DATA[i].d_date2);
            const int   RESULT = DATA[i].d_result;

            if (veryVerbose) {
                T_
                P_(LINE) P_(MONTH) P_(DATE1) P_(DATE2) P(RESULT);
            }

            int result = Util::numMonthsInRange(MONTH, DATE1, DATE2);
            LOOP2_ASSERT(LINE, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'numMonthsInRange' Function Test."
                          << endl;
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'floorOrNextMonthInYear' FUNCTION
        //   This will test the 'floorOrNextMonthInYear' function.
        //
        // Concerns:
        //   1. If 'floorMonth' falls in the same year as 'date', then
        //      'floorMonth' must be returned.
        //   2. If 'floorMonth' falls in the following year, then 'nextMonth'
        //      must be returned.
        //   3. The returned date's day is correctly capped by the number of
        //      days in the returned date's month and year.
        //
        // Plan:
        //   For a set of test data, exercise the 'floorOrNextMonthInYear'
        //   function and check that the returned date is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //            month      date.day      date.month
        //            -----      --------      ----------
        //
        //           | JAN |
        //           | FEB |     |    1 |     | month-1 |
        //           |     |  X  |   15 |  X  | month   |
        //           | JUN |     | last |     | month+1 |
        //           | DEC |
        //
        //   Note that not all months are included in the cross-product because
        //   this would drastically increase the size of the test vector,
        //   without contributing anything new to the test.  The selected
        //   months are chosen for the following reasons:
        //
        //       o JAN, DEC : first and last month (boundary check), both
        //                    contain 31 days.
        //       o FEB      : interesting because of leap year.
        //       o JUN      : contains 30 days.
        //
        //   The first and last days of each month are chosen for boundary
        //   checks.  The 15th of each month is also used as an additional
        //   check.  The last day is especially important in order to check
        //   that the returned date's day is capped correctly.
        //
        //   The values for 'date.month' are chosen to be one less than, equal
        //   to, and one more than the value of 'month'.  This scheme allows
        //   the 'd.year() == date.year()' condition to be true and false in
        //   different cases.  This also matches boundary conditions in
        //   'floorMonth' and 'nextMonth'.
        //
        //   Note that if 'month' is JAN, then 'date.month' cannot be
        //   'month-1'.  Likewise, if 'month' is DEC, then 'date.month' cannot
        //   be 'month+1'.
        //
        //   Also note that 'date.year' can be any arbitrary value that does
        //   not make this operation undefined.  A leap year is *preferred*,
        //   though not necessary, in order to check that the returned date's
        //   day is capped correctly.
        //
        // Testing:
        //   b_D floorOrNextMonthInYear(b_M month, c b_D& date);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'floorOrNextMonthInYear' Function"
                          << "\n========================================="
                          << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Month       d_month;    // month
            const char *d_date;     // date
            const char *d_result;   // expected result
        } DATA[] = {
            //line   month   date            result
            //----   -----   ----            ------

            // date.month = month-1   (will return nextMonth)
            { L_,    FEB,    "1996,01,01",   "1996,02,01"  },
            { L_,    FEB,    "1996,01,15",   "1996,02,15"  },
            { L_,    FEB,    "1996,01,31",   "1996,02,29"  },
            { L_,    JUN,    "1996,05,01",   "1996,06,01"  },
            { L_,    JUN,    "1996,05,15",   "1996,06,15"  },
            { L_,    JUN,    "1996,05,31",   "1996,06,30"  },
            { L_,    DEC,    "1996,11,01",   "1996,12,01"  },
            { L_,    DEC,    "1996,11,15",   "1996,12,15"  },
            { L_,    DEC,    "1996,11,30",   "1996,12,30"  },

            // date.month = month     (will return floorMonth)
            { L_,    JAN,    "1996,01,01",   "1996,01,01"  },
            { L_,    JAN,    "1996,01,15",   "1996,01,15"  },
            { L_,    JAN,    "1996,01,31",   "1996,01,31"  },
            { L_,    FEB,    "1996,02,01",   "1996,02,01"  },
            { L_,    FEB,    "1996,02,15",   "1996,02,15"  },
            { L_,    FEB,    "1996,02,29",   "1996,02,29"  },
            { L_,    JUN,    "1996,06,01",   "1996,06,01"  },
            { L_,    JUN,    "1996,06,15",   "1996,06,15"  },
            { L_,    JUN,    "1996,06,30",   "1996,06,30"  },
            { L_,    DEC,    "1996,12,01",   "1996,12,01"  },
            { L_,    DEC,    "1996,12,15",   "1996,12,15"  },
            { L_,    DEC,    "1996,12,31",   "1996,12,31"  },

            // date.month = month+1   (will return floorMonth)
            { L_,    JAN,    "1996,02,01",   "1996,01,01"  },
            { L_,    JAN,    "1996,02,15",   "1996,01,15"  },
            { L_,    JAN,    "1996,02,29",   "1996,01,29"  },
            { L_,    FEB,    "1996,03,01",   "1996,02,01"  },
            { L_,    FEB,    "1996,03,15",   "1996,02,15"  },
            { L_,    FEB,    "1996,03,31",   "1996,02,29"  },
            { L_,    JUN,    "1996,07,01",   "1996,06,01"  },
            { L_,    JUN,    "1996,07,15",   "1996,06,15"  },
            { L_,    JUN,    "1996,07,30",   "1996,06,30"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const Month MONTH  = DATA[i].d_month;
            const Date  DATE   = parseDate(DATA[i].d_date);
            const Date  RESULT = parseDate(DATA[i].d_result);

            if (veryVerbose) {
                T_ P_(LINE) P_(MONTH) P_(DATE) P(RESULT)
            }

            Date result = Util::floorOrNextMonthInYear(MONTH, DATE);
            LOOP3_ASSERT(LINE, RESULT, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'floorOrNextMonthInYear' Function Test."
                          << endl;
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'ceilOrPreviousMonthInYear' FUNCTION
        //   This will test the 'ceilOrPreviousMonthInYear' function.
        //
        // Concerns:
        //   1. If 'ceilMonth' falls in the same year as 'date', then
        //      'ceilMonth' must be returned.
        //   2. If 'ceilMonth' falls in the following year, then
        //      'previousMonth' must be returned.
        //   3. The returned date's day is correctly capped by the number of
        //      days in the returned date's month and year.
        //
        // Plan:
        //   For a set of test data, exercise the 'ceilOrPreviousMonthInYear'
        //   function and check that the returned date is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //            month      date.day      date.month
        //            -----      --------      ----------
        //
        //           | JAN |
        //           | FEB |     |    1 |     | month-1 |
        //           |     |  X  |   15 |  X  | month   |
        //           | JUN |     | last |     | month+1 |
        //           | DEC |
        //
        //   Note that not all months are included in the cross-product because
        //   this would drastically increase the size of the test vector,
        //   without contributing anything new to the test.  The selected
        //   months are chosen for the following reasons:
        //
        //       o JAN, DEC : first and last month (boundary check), both
        //                    contain 31 days.
        //       o FEB      : interesting because of leap year.
        //       o JUN      : contains 30 days.
        //
        //   The first and last days of each month are chosen for boundary
        //   checks.  The 15th of each month is also used as an additional
        //   check.  The last day is especially important in order to check
        //   that the returned date's day is capped correctly.
        //
        //   The values for 'date.month' are chosen to be one less than, equal
        //   to, and one more than the value of 'month'.  This scheme allows
        //   the 'd.year() == date.year()' condition to be true and false in
        //   different cases.  This also matches boundary conditions in
        //   'ceilMonth' and 'previousMonth'.
        //
        //   Note that if 'month' is JAN, then 'date.month' cannot be
        //   'month-1'.  Likewise, if 'month' is DEC, then 'date.month' cannot
        //   be 'month+1'.
        //
        //   Also note that 'date.year' can be any arbitrary value that does
        //   not make this operation undefined.  A leap year is *preferred*,
        //   though not necessary, in order to check that the returned date's
        //   day is capped correctly.
        //
        // Testing:
        //   b_D ceilOrPreviousMonthInYear(b_M month, c b_D& date);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'ceilOrPreviousMonthInYear' Function"
                          << "\n============================================"
                          << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Month       d_month;    // month
            const char *d_date;     // date
            const char *d_result;   // expected result
        } DATA[] = {
            //line   month   date            result
            //----   -----   ----            ------

            // date.month = month-1   (will return ceilMonth)
            { L_,    FEB,    "1996,01,01",   "1996,02,01"  },
            { L_,    FEB,    "1996,01,15",   "1996,02,15"  },
            { L_,    FEB,    "1996,01,31",   "1996,02,29"  },
            { L_,    JUN,    "1996,05,01",   "1996,06,01"  },
            { L_,    JUN,    "1996,05,15",   "1996,06,15"  },
            { L_,    JUN,    "1996,05,31",   "1996,06,30"  },
            { L_,    DEC,    "1996,11,01",   "1996,12,01"  },
            { L_,    DEC,    "1996,11,15",   "1996,12,15"  },
            { L_,    DEC,    "1996,11,30",   "1996,12,30"  },

            // date.month = month     (will return ceilMonth)
            { L_,    JAN,    "1996,01,01",   "1996,01,01"  },
            { L_,    JAN,    "1996,01,15",   "1996,01,15"  },
            { L_,    JAN,    "1996,01,31",   "1996,01,31"  },
            { L_,    FEB,    "1996,02,01",   "1996,02,01"  },
            { L_,    FEB,    "1996,02,15",   "1996,02,15"  },
            { L_,    FEB,    "1996,02,29",   "1996,02,29"  },
            { L_,    JUN,    "1996,06,01",   "1996,06,01"  },
            { L_,    JUN,    "1996,06,15",   "1996,06,15"  },
            { L_,    JUN,    "1996,06,30",   "1996,06,30"  },
            { L_,    DEC,    "1996,12,01",   "1996,12,01"  },
            { L_,    DEC,    "1996,12,15",   "1996,12,15"  },
            { L_,    DEC,    "1996,12,31",   "1996,12,31"  },

            // date.month = month+1   (will return previousMonth)
            { L_,    JAN,    "1996,02,01",   "1996,01,01"  },
            { L_,    JAN,    "1996,02,15",   "1996,01,15"  },
            { L_,    JAN,    "1996,02,29",   "1996,01,29"  },
            { L_,    FEB,    "1996,03,01",   "1996,02,01"  },
            { L_,    FEB,    "1996,03,15",   "1996,02,15"  },
            { L_,    FEB,    "1996,03,31",   "1996,02,29"  },
            { L_,    JUN,    "1996,07,01",   "1996,06,01"  },
            { L_,    JUN,    "1996,07,15",   "1996,06,15"  },
            { L_,    JUN,    "1996,07,30",   "1996,06,30"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const Month MONTH  = DATA[i].d_month;
            const Date  DATE   = parseDate(DATA[i].d_date);
            const Date  RESULT = parseDate(DATA[i].d_result);

            if (veryVerbose) {
                T_ P_(LINE) P_(MONTH) P_(DATE) P(RESULT)
            }

            Date result = Util::ceilOrPreviousMonthInYear(MONTH, DATE);
            LOOP3_ASSERT(LINE, RESULT, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'ceilOrPreviousMonthInYear' Function "
                          << "Test." << endl;
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'floorAdjustMonth2' FUNCTION
        //   This will test the 'floorAdjustMonth2' function.
        //
        // Concerns:
        //   1. When 'count' is zero, the function must return 'floorMonth'.
        //   2. When 'count' is not zero, the function must return the date
        //      that is 'count' years from 'floorMonth'.
        //   3. The returned date's day is correctly capped by the number of
        //      days in the returned date's month and year *and* the number of
        //      days in 'floorMonth's month and year.
        //
        // Plan:
        //   For a set of test data, exercise the 'floorAdjustMonth2' function
        //   and check that the returned date is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //        month      date.day      date.month     count
        //        -----      --------      ----------     -----
        //
        //       | JAN |                                  | -2 |
        //       | FEB |     |    1 |     | month-1 |     | -1 |
        //       |     |  X  |   15 |  X  | month   |  X  |  0 |
        //       | JUN |     | last |     | month+1 |     |  1 |
        //       | DEC |                                  |  2 |
        //
        //   Note that not all months are included in the cross-product because
        //   this would drastically increase the size of the test vector,
        //   without contributing anything new to the test.  The selected
        //   months are chosen for the following reasons:
        //
        //       o JAN, DEC : first and last month (boundary check), both
        //                    contain 31 days.
        //       o FEB      : interesting because of leap year.
        //       o JUN      : contains 30 days.
        //
        //   The first and last days of each month are chosen for boundary
        //   checks.  The 15th of each month is also used as an additional
        //   check.  The last day is especially important in order to check
        //   that the returned date's day is capped correctly.
        //
        //   The values for 'date.month' are chosen to be one less than, equal
        //   to, and one more than the value of 'month'.  This is to match
        //   boundary conditions in 'floorMonth' and 'adjustMonth'.
        //
        //   The values for 'count' are all valid 'count' values between -2 and
        //   +2.  Other values are not included because they do not contribute
        //   anything new to the test.
        //
        //   Note that if 'month' is JAN, then 'date.month' cannot be
        //   'month-1'.  Likewise, if 'month' is DEC, then 'date.month' cannot
        //   be 'month+1'.
        //
        //   Also note that 'date.year' can be any arbitrary value that does
        //   not make this operation undefined.  A leap year is *preferred*,
        //   though not necessary, in order to check that the returned date's
        //   day is capped correctly.
        //
        // Testing:
        //   b_D floorAdjustMonth2(b_M month, c b_D& date, int count);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'floorAdjustMonth2' Function"
                          << "\n====================================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Month       d_month;    // month
            const char *d_date;     // date
            int         d_count;    // count
            const char *d_result;   // expected result
        } DATA[] = {
            //line   month   date            count    result
            //----   -----   ----            -----    ------

            // date.month = month-1
            { L_,    FEB,    "1996,01,01",   -2,      "1993,02,01"  },
            { L_,    FEB,    "1996,01,15",   -2,      "1993,02,15"  },
            { L_,    FEB,    "1996,01,31",   -2,      "1993,02,28"  },
            { L_,    JUN,    "1996,05,01",   -2,      "1993,06,01"  },
            { L_,    JUN,    "1996,05,15",   -2,      "1993,06,15"  },
            { L_,    JUN,    "1996,05,31",   -2,      "1993,06,30"  },
            { L_,    DEC,    "1996,11,01",   -2,      "1993,12,01"  },
            { L_,    DEC,    "1996,11,15",   -2,      "1993,12,15"  },
            { L_,    DEC,    "1996,11,30",   -2,      "1993,12,30"  },

            { L_,    FEB,    "1996,01,01",   -1,      "1994,02,01"  },
            { L_,    FEB,    "1996,01,15",   -1,      "1994,02,15"  },
            { L_,    FEB,    "1996,01,31",   -1,      "1994,02,28"  },
            { L_,    JUN,    "1996,05,01",   -1,      "1994,06,01"  },
            { L_,    JUN,    "1996,05,15",   -1,      "1994,06,15"  },
            { L_,    JUN,    "1996,05,31",   -1,      "1994,06,30"  },
            { L_,    DEC,    "1996,11,01",   -1,      "1994,12,01"  },
            { L_,    DEC,    "1996,11,15",   -1,      "1994,12,15"  },
            { L_,    DEC,    "1996,11,30",   -1,      "1994,12,30"  },

            { L_,    FEB,    "1996,01,01",   0,       "1995,02,01"  },
            { L_,    FEB,    "1996,01,15",   0,       "1995,02,15"  },
            { L_,    FEB,    "1996,01,31",   0,       "1995,02,28"  },
            { L_,    JUN,    "1996,05,01",   0,       "1995,06,01"  },
            { L_,    JUN,    "1996,05,15",   0,       "1995,06,15"  },
            { L_,    JUN,    "1996,05,31",   0,       "1995,06,30"  },
            { L_,    DEC,    "1996,11,01",   0,       "1995,12,01"  },
            { L_,    DEC,    "1996,11,15",   0,       "1995,12,15"  },
            { L_,    DEC,    "1996,11,30",   0,       "1995,12,30"  },

            { L_,    FEB,    "1996,01,01",   1,       "1996,02,01"  },
            { L_,    FEB,    "1996,01,15",   1,       "1996,02,15"  },
            { L_,    FEB,    "1996,01,31",   1,       "1996,02,28"  },
            { L_,    JUN,    "1996,05,01",   1,       "1996,06,01"  },
            { L_,    JUN,    "1996,05,15",   1,       "1996,06,15"  },
            { L_,    JUN,    "1996,05,31",   1,       "1996,06,30"  },
            { L_,    DEC,    "1996,11,01",   1,       "1996,12,01"  },
            { L_,    DEC,    "1996,11,15",   1,       "1996,12,15"  },
            { L_,    DEC,    "1996,11,30",   1,       "1996,12,30"  },

            { L_,    FEB,    "1996,01,01",   2,       "1997,02,01"  },
            { L_,    FEB,    "1996,01,15",   2,       "1997,02,15"  },
            { L_,    FEB,    "1996,01,31",   2,       "1997,02,28"  },
            { L_,    JUN,    "1996,05,01",   2,       "1997,06,01"  },
            { L_,    JUN,    "1996,05,15",   2,       "1997,06,15"  },
            { L_,    JUN,    "1996,05,31",   2,       "1997,06,30"  },
            { L_,    DEC,    "1996,11,01",   2,       "1997,12,01"  },
            { L_,    DEC,    "1996,11,15",   2,       "1997,12,15"  },
            { L_,    DEC,    "1996,11,30",   2,       "1997,12,30"  },

            // date.month = month
            { L_,    JAN,    "1996,01,01",   -2,      "1994,01,01"  },
            { L_,    JAN,    "1996,01,15",   -2,      "1994,01,15"  },
            { L_,    JAN,    "1996,01,31",   -2,      "1994,01,31"  },
            { L_,    FEB,    "1996,02,01",   -2,      "1994,02,01"  },
            { L_,    FEB,    "1996,02,15",   -2,      "1994,02,15"  },
            { L_,    FEB,    "1996,02,29",   -2,      "1994,02,28"  },
            { L_,    JUN,    "1996,06,01",   -2,      "1994,06,01"  },
            { L_,    JUN,    "1996,06,15",   -2,      "1994,06,15"  },
            { L_,    JUN,    "1996,06,30",   -2,      "1994,06,30"  },
            { L_,    DEC,    "1996,12,01",   -2,      "1994,12,01"  },
            { L_,    DEC,    "1996,12,15",   -2,      "1994,12,15"  },
            { L_,    DEC,    "1996,12,31",   -2,      "1994,12,31"  },

            { L_,    JAN,    "1996,01,01",   -1,      "1995,01,01"  },
            { L_,    JAN,    "1996,01,15",   -1,      "1995,01,15"  },
            { L_,    JAN,    "1996,01,31",   -1,      "1995,01,31"  },
            { L_,    FEB,    "1996,02,01",   -1,      "1995,02,01"  },
            { L_,    FEB,    "1996,02,15",   -1,      "1995,02,15"  },
            { L_,    FEB,    "1996,02,29",   -1,      "1995,02,28"  },
            { L_,    JUN,    "1996,06,01",   -1,      "1995,06,01"  },
            { L_,    JUN,    "1996,06,15",   -1,      "1995,06,15"  },
            { L_,    JUN,    "1996,06,30",   -1,      "1995,06,30"  },
            { L_,    DEC,    "1996,12,01",   -1,      "1995,12,01"  },
            { L_,    DEC,    "1996,12,15",   -1,      "1995,12,15"  },
            { L_,    DEC,    "1996,12,31",   -1,      "1995,12,31"  },

            { L_,    JAN,    "1996,01,01",   0,       "1996,01,01"  },
            { L_,    JAN,    "1996,01,15",   0,       "1996,01,15"  },
            { L_,    JAN,    "1996,01,31",   0,       "1996,01,31"  },
            { L_,    FEB,    "1996,02,01",   0,       "1996,02,01"  },
            { L_,    FEB,    "1996,02,15",   0,       "1996,02,15"  },
            { L_,    FEB,    "1996,02,29",   0,       "1996,02,29"  },
            { L_,    JUN,    "1996,06,01",   0,       "1996,06,01"  },
            { L_,    JUN,    "1996,06,15",   0,       "1996,06,15"  },
            { L_,    JUN,    "1996,06,30",   0,       "1996,06,30"  },
            { L_,    DEC,    "1996,12,01",   0,       "1996,12,01"  },
            { L_,    DEC,    "1996,12,15",   0,       "1996,12,15"  },
            { L_,    DEC,    "1996,12,31",   0,       "1996,12,31"  },

            { L_,    JAN,    "1996,01,01",   1,       "1997,01,01"  },
            { L_,    JAN,    "1996,01,15",   1,       "1997,01,15"  },
            { L_,    JAN,    "1996,01,31",   1,       "1997,01,31"  },
            { L_,    FEB,    "1996,02,01",   1,       "1997,02,01"  },
            { L_,    FEB,    "1996,02,15",   1,       "1997,02,15"  },
            { L_,    FEB,    "1996,02,29",   1,       "1997,02,28"  },
            { L_,    JUN,    "1996,06,01",   1,       "1997,06,01"  },
            { L_,    JUN,    "1996,06,15",   1,       "1997,06,15"  },
            { L_,    JUN,    "1996,06,30",   1,       "1997,06,30"  },
            { L_,    DEC,    "1996,12,01",   1,       "1997,12,01"  },
            { L_,    DEC,    "1996,12,15",   1,       "1997,12,15"  },
            { L_,    DEC,    "1996,12,31",   1,       "1997,12,31"  },

            { L_,    JAN,    "1996,01,01",   2,       "1998,01,01"  },
            { L_,    JAN,    "1996,01,15",   2,       "1998,01,15"  },
            { L_,    JAN,    "1996,01,31",   2,       "1998,01,31"  },
            { L_,    FEB,    "1996,02,01",   2,       "1998,02,01"  },
            { L_,    FEB,    "1996,02,15",   2,       "1998,02,15"  },
            { L_,    FEB,    "1996,02,29",   2,       "1998,02,28"  },
            { L_,    JUN,    "1996,06,01",   2,       "1998,06,01"  },
            { L_,    JUN,    "1996,06,15",   2,       "1998,06,15"  },
            { L_,    JUN,    "1996,06,30",   2,       "1998,06,30"  },
            { L_,    DEC,    "1996,12,01",   2,       "1998,12,01"  },
            { L_,    DEC,    "1996,12,15",   2,       "1998,12,15"  },
            { L_,    DEC,    "1996,12,31",   2,       "1998,12,31"  },

            // date.month = month+1
            { L_,    JAN,    "1996,02,01",   -2,      "1994,01,01"  },
            { L_,    JAN,    "1996,02,15",   -2,      "1994,01,15"  },
            { L_,    JAN,    "1996,02,29",   -2,      "1994,01,29"  },
            { L_,    FEB,    "1996,03,01",   -2,      "1994,02,01"  },
            { L_,    FEB,    "1996,03,15",   -2,      "1994,02,15"  },
            { L_,    FEB,    "1996,03,31",   -2,      "1994,02,28"  },
            { L_,    JUN,    "1996,07,01",   -2,      "1994,06,01"  },
            { L_,    JUN,    "1996,07,15",   -2,      "1994,06,15"  },
            { L_,    JUN,    "1996,07,30",   -2,      "1994,06,30"  },

            { L_,    JAN,    "1996,02,01",   -1,      "1995,01,01"  },
            { L_,    JAN,    "1996,02,15",   -1,      "1995,01,15"  },
            { L_,    JAN,    "1996,02,29",   -1,      "1995,01,29"  },
            { L_,    FEB,    "1996,03,01",   -1,      "1995,02,01"  },
            { L_,    FEB,    "1996,03,15",   -1,      "1995,02,15"  },
            { L_,    FEB,    "1996,03,31",   -1,      "1995,02,28"  },
            { L_,    JUN,    "1996,07,01",   -1,      "1995,06,01"  },
            { L_,    JUN,    "1996,07,15",   -1,      "1995,06,15"  },
            { L_,    JUN,    "1996,07,30",   -1,      "1995,06,30"  },

            { L_,    JAN,    "1996,02,01",   0,       "1996,01,01"  },
            { L_,    JAN,    "1996,02,15",   0,       "1996,01,15"  },
            { L_,    JAN,    "1996,02,29",   0,       "1996,01,29"  },
            { L_,    FEB,    "1996,03,01",   0,       "1996,02,01"  },
            { L_,    FEB,    "1996,03,15",   0,       "1996,02,15"  },
            { L_,    FEB,    "1996,03,31",   0,       "1996,02,29"  },
            { L_,    JUN,    "1996,07,01",   0,       "1996,06,01"  },
            { L_,    JUN,    "1996,07,15",   0,       "1996,06,15"  },
            { L_,    JUN,    "1996,07,30",   0,       "1996,06,30"  },

            { L_,    JAN,    "1996,02,01",   1,       "1997,01,01"  },
            { L_,    JAN,    "1996,02,15",   1,       "1997,01,15"  },
            { L_,    JAN,    "1996,02,29",   1,       "1997,01,29"  },
            { L_,    FEB,    "1996,03,01",   1,       "1997,02,01"  },
            { L_,    FEB,    "1996,03,15",   1,       "1997,02,15"  },
            { L_,    FEB,    "1996,03,31",   1,       "1997,02,28"  },
            { L_,    JUN,    "1996,07,01",   1,       "1997,06,01"  },
            { L_,    JUN,    "1996,07,15",   1,       "1997,06,15"  },
            { L_,    JUN,    "1996,07,30",   1,       "1997,06,30"  },

            { L_,    JAN,    "1996,02,01",   2,       "1998,01,01"  },
            { L_,    JAN,    "1996,02,15",   2,       "1998,01,15"  },
            { L_,    JAN,    "1996,02,29",   2,       "1998,01,29"  },
            { L_,    FEB,    "1996,03,01",   2,       "1998,02,01"  },
            { L_,    FEB,    "1996,03,15",   2,       "1998,02,15"  },
            { L_,    FEB,    "1996,03,31",   2,       "1998,02,28"  },
            { L_,    JUN,    "1996,07,01",   2,       "1998,06,01"  },
            { L_,    JUN,    "1996,07,15",   2,       "1998,06,15"  },
            { L_,    JUN,    "1996,07,30",   2,       "1998,06,30"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const Month MONTH  = DATA[i].d_month;
            const Date  DATE   = parseDate(DATA[i].d_date);
            const int   COUNT  = DATA[i].d_count;
            const Date  RESULT = parseDate(DATA[i].d_result);

            if (veryVerbose) {
                T_ P_(LINE) P_(MONTH) P_(DATE) P_(COUNT) P(RESULT)
            }

            Date result = Util::floorAdjustMonth2(MONTH, DATE, COUNT);
            LOOP3_ASSERT(LINE, RESULT, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'floorAdjustMonth2' Function Test."
                          << endl;
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'floorAdjustMonth' FUNCTION
        //   This will test the 'floorAdjustMonth' function.
        //
        // Concerns:
        //   1. When 'count' is zero, the function must return 'floorMonth'.
        //   2. When 'count' is not zero, the function must return the date
        //      that is 'count' years from 'floorMonth'.
        //   3. The returned date's day is correctly capped by the number of
        //      days in the returned date's month and year.
        //
        // Plan:
        //   For a set of test data, exercise the 'floorAdjustMonth' function
        //   and check that the returned date is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //        month      date.day      date.month     count
        //        -----      --------      ----------     -----
        //
        //       | JAN |                                  | -2 |
        //       | FEB |     |    1 |     | month-1 |     | -1 |
        //       |     |  X  |   15 |  X  | month   |  X  |  0 |
        //       | JUN |     | last |     | month+1 |     |  1 |
        //       | DEC |                                  |  2 |
        //
        //   Note that not all months are included in the cross-product because
        //   this would drastically increase the size of the test vector,
        //   without contributing anything new to the test.  The selected
        //   months are chosen for the following reasons:
        //
        //       o JAN, DEC : first and last month (boundary check), both
        //                    contain 31 days.
        //       o FEB      : interesting because of leap year.
        //       o JUN      : contains 30 days.
        //
        //   The first and last days of each month are chosen for boundary
        //   checks.  The 15th of each month is also used as an additional
        //   check.  The last day is especially important in order to check
        //   that the returned date's day is capped correctly.
        //
        //   The values for 'date.month' are chosen to be one less than, equal
        //   to, and one more than the value of 'month'.  This is to match
        //   boundary conditions in 'floorMonth' and 'adjustMonth'.
        //
        //   The values for 'count' are all valid 'count' values between -2 and
        //   +2.  Other values are not included because they do not contribute
        //   anything new to the test.
        //
        //   Note that if 'month' is JAN, then 'date.month' cannot be
        //   'month-1'.  Likewise, if 'month' is DEC, then 'date.month' cannot
        //   be 'month+1'.
        //
        //   Also note that 'date.year' can be any arbitrary value that does
        //   not make this operation undefined.  A leap year is *preferred*,
        //   though not necessary, in order to test the usage of
        //   'lastDayOfMonth'.
        //
        // Testing:
        //   b_D floorAdjustMonth(b_M month, c b_D& date, int count);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'floorAdjustMonth' Function"
                          << "\n===================================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Month       d_month;    // month
            const char *d_date;     // date
            int         d_count;    // count
            const char *d_result;   // expected result
        } DATA[] = {
            //line   month   date            count    result
            //----   -----   ----            -----    ------

            // date.month = month-1
            { L_,    FEB,    "1996,01,01",   -2,      "1993,02,01"  },
            { L_,    FEB,    "1996,01,15",   -2,      "1993,02,15"  },
            { L_,    FEB,    "1996,01,31",   -2,      "1993,02,28"  },
            { L_,    JUN,    "1996,05,01",   -2,      "1993,06,01"  },
            { L_,    JUN,    "1996,05,15",   -2,      "1993,06,15"  },
            { L_,    JUN,    "1996,05,31",   -2,      "1993,06,30"  },
            { L_,    DEC,    "1996,11,01",   -2,      "1993,12,01"  },
            { L_,    DEC,    "1996,11,15",   -2,      "1993,12,15"  },
            { L_,    DEC,    "1996,11,30",   -2,      "1993,12,30"  },

            { L_,    FEB,    "1996,01,01",   -1,      "1994,02,01"  },
            { L_,    FEB,    "1996,01,15",   -1,      "1994,02,15"  },
            { L_,    FEB,    "1996,01,31",   -1,      "1994,02,28"  },
            { L_,    JUN,    "1996,05,01",   -1,      "1994,06,01"  },
            { L_,    JUN,    "1996,05,15",   -1,      "1994,06,15"  },
            { L_,    JUN,    "1996,05,31",   -1,      "1994,06,30"  },
            { L_,    DEC,    "1996,11,01",   -1,      "1994,12,01"  },
            { L_,    DEC,    "1996,11,15",   -1,      "1994,12,15"  },
            { L_,    DEC,    "1996,11,30",   -1,      "1994,12,30"  },

            { L_,    FEB,    "1996,01,01",   0,       "1995,02,01"  },
            { L_,    FEB,    "1996,01,15",   0,       "1995,02,15"  },
            { L_,    FEB,    "1996,01,31",   0,       "1995,02,28"  },
            { L_,    JUN,    "1996,05,01",   0,       "1995,06,01"  },
            { L_,    JUN,    "1996,05,15",   0,       "1995,06,15"  },
            { L_,    JUN,    "1996,05,31",   0,       "1995,06,30"  },
            { L_,    DEC,    "1996,11,01",   0,       "1995,12,01"  },
            { L_,    DEC,    "1996,11,15",   0,       "1995,12,15"  },
            { L_,    DEC,    "1996,11,30",   0,       "1995,12,30"  },

            { L_,    FEB,    "1996,01,01",   1,       "1996,02,01"  },
            { L_,    FEB,    "1996,01,15",   1,       "1996,02,15"  },
            { L_,    FEB,    "1996,01,31",   1,       "1996,02,29"  },
            { L_,    JUN,    "1996,05,01",   1,       "1996,06,01"  },
            { L_,    JUN,    "1996,05,15",   1,       "1996,06,15"  },
            { L_,    JUN,    "1996,05,31",   1,       "1996,06,30"  },
            { L_,    DEC,    "1996,11,01",   1,       "1996,12,01"  },
            { L_,    DEC,    "1996,11,15",   1,       "1996,12,15"  },
            { L_,    DEC,    "1996,11,30",   1,       "1996,12,30"  },

            { L_,    FEB,    "1996,01,01",   2,       "1997,02,01"  },
            { L_,    FEB,    "1996,01,15",   2,       "1997,02,15"  },
            { L_,    FEB,    "1996,01,31",   2,       "1997,02,28"  },
            { L_,    JUN,    "1996,05,01",   2,       "1997,06,01"  },
            { L_,    JUN,    "1996,05,15",   2,       "1997,06,15"  },
            { L_,    JUN,    "1996,05,31",   2,       "1997,06,30"  },
            { L_,    DEC,    "1996,11,01",   2,       "1997,12,01"  },
            { L_,    DEC,    "1996,11,15",   2,       "1997,12,15"  },
            { L_,    DEC,    "1996,11,30",   2,       "1997,12,30"  },

            // date.month = month
            { L_,    JAN,    "1996,01,01",   -2,      "1994,01,01"  },
            { L_,    JAN,    "1996,01,15",   -2,      "1994,01,15"  },
            { L_,    JAN,    "1996,01,31",   -2,      "1994,01,31"  },
            { L_,    FEB,    "1996,02,01",   -2,      "1994,02,01"  },
            { L_,    FEB,    "1996,02,15",   -2,      "1994,02,15"  },
            { L_,    FEB,    "1996,02,29",   -2,      "1994,02,28"  },
            { L_,    JUN,    "1996,06,01",   -2,      "1994,06,01"  },
            { L_,    JUN,    "1996,06,15",   -2,      "1994,06,15"  },
            { L_,    JUN,    "1996,06,30",   -2,      "1994,06,30"  },
            { L_,    DEC,    "1996,12,01",   -2,      "1994,12,01"  },
            { L_,    DEC,    "1996,12,15",   -2,      "1994,12,15"  },
            { L_,    DEC,    "1996,12,31",   -2,      "1994,12,31"  },

            { L_,    JAN,    "1996,01,01",   -1,      "1995,01,01"  },
            { L_,    JAN,    "1996,01,15",   -1,      "1995,01,15"  },
            { L_,    JAN,    "1996,01,31",   -1,      "1995,01,31"  },
            { L_,    FEB,    "1996,02,01",   -1,      "1995,02,01"  },
            { L_,    FEB,    "1996,02,15",   -1,      "1995,02,15"  },
            { L_,    FEB,    "1996,02,29",   -1,      "1995,02,28"  },
            { L_,    JUN,    "1996,06,01",   -1,      "1995,06,01"  },
            { L_,    JUN,    "1996,06,15",   -1,      "1995,06,15"  },
            { L_,    JUN,    "1996,06,30",   -1,      "1995,06,30"  },
            { L_,    DEC,    "1996,12,01",   -1,      "1995,12,01"  },
            { L_,    DEC,    "1996,12,15",   -1,      "1995,12,15"  },
            { L_,    DEC,    "1996,12,31",   -1,      "1995,12,31"  },

            { L_,    JAN,    "1996,01,01",   0,       "1996,01,01"  },
            { L_,    JAN,    "1996,01,15",   0,       "1996,01,15"  },
            { L_,    JAN,    "1996,01,31",   0,       "1996,01,31"  },
            { L_,    FEB,    "1996,02,01",   0,       "1996,02,01"  },
            { L_,    FEB,    "1996,02,15",   0,       "1996,02,15"  },
            { L_,    FEB,    "1996,02,29",   0,       "1996,02,29"  },
            { L_,    JUN,    "1996,06,01",   0,       "1996,06,01"  },
            { L_,    JUN,    "1996,06,15",   0,       "1996,06,15"  },
            { L_,    JUN,    "1996,06,30",   0,       "1996,06,30"  },
            { L_,    DEC,    "1996,12,01",   0,       "1996,12,01"  },
            { L_,    DEC,    "1996,12,15",   0,       "1996,12,15"  },
            { L_,    DEC,    "1996,12,31",   0,       "1996,12,31"  },

            { L_,    JAN,    "1996,01,01",   1,       "1997,01,01"  },
            { L_,    JAN,    "1996,01,15",   1,       "1997,01,15"  },
            { L_,    JAN,    "1996,01,31",   1,       "1997,01,31"  },
            { L_,    FEB,    "1996,02,01",   1,       "1997,02,01"  },
            { L_,    FEB,    "1996,02,15",   1,       "1997,02,15"  },
            { L_,    FEB,    "1996,02,29",   1,       "1997,02,28"  },
            { L_,    JUN,    "1996,06,01",   1,       "1997,06,01"  },
            { L_,    JUN,    "1996,06,15",   1,       "1997,06,15"  },
            { L_,    JUN,    "1996,06,30",   1,       "1997,06,30"  },
            { L_,    DEC,    "1996,12,01",   1,       "1997,12,01"  },
            { L_,    DEC,    "1996,12,15",   1,       "1997,12,15"  },
            { L_,    DEC,    "1996,12,31",   1,       "1997,12,31"  },

            { L_,    JAN,    "1996,01,01",   2,       "1998,01,01"  },
            { L_,    JAN,    "1996,01,15",   2,       "1998,01,15"  },
            { L_,    JAN,    "1996,01,31",   2,       "1998,01,31"  },
            { L_,    FEB,    "1996,02,01",   2,       "1998,02,01"  },
            { L_,    FEB,    "1996,02,15",   2,       "1998,02,15"  },
            { L_,    FEB,    "1996,02,29",   2,       "1998,02,28"  },
            { L_,    JUN,    "1996,06,01",   2,       "1998,06,01"  },
            { L_,    JUN,    "1996,06,15",   2,       "1998,06,15"  },
            { L_,    JUN,    "1996,06,30",   2,       "1998,06,30"  },
            { L_,    DEC,    "1996,12,01",   2,       "1998,12,01"  },
            { L_,    DEC,    "1996,12,15",   2,       "1998,12,15"  },
            { L_,    DEC,    "1996,12,31",   2,       "1998,12,31"  },

            // date.month = month+1
            { L_,    JAN,    "1996,02,01",   -2,      "1994,01,01"  },
            { L_,    JAN,    "1996,02,15",   -2,      "1994,01,15"  },
            { L_,    JAN,    "1996,02,29",   -2,      "1994,01,29"  },
            { L_,    FEB,    "1996,03,01",   -2,      "1994,02,01"  },
            { L_,    FEB,    "1996,03,15",   -2,      "1994,02,15"  },
            { L_,    FEB,    "1996,03,31",   -2,      "1994,02,28"  },
            { L_,    JUN,    "1996,07,01",   -2,      "1994,06,01"  },
            { L_,    JUN,    "1996,07,15",   -2,      "1994,06,15"  },
            { L_,    JUN,    "1996,07,30",   -2,      "1994,06,30"  },

            { L_,    JAN,    "1996,02,01",   -1,      "1995,01,01"  },
            { L_,    JAN,    "1996,02,15",   -1,      "1995,01,15"  },
            { L_,    JAN,    "1996,02,29",   -1,      "1995,01,29"  },
            { L_,    FEB,    "1996,03,01",   -1,      "1995,02,01"  },
            { L_,    FEB,    "1996,03,15",   -1,      "1995,02,15"  },
            { L_,    FEB,    "1996,03,31",   -1,      "1995,02,28"  },
            { L_,    JUN,    "1996,07,01",   -1,      "1995,06,01"  },
            { L_,    JUN,    "1996,07,15",   -1,      "1995,06,15"  },
            { L_,    JUN,    "1996,07,30",   -1,      "1995,06,30"  },

            { L_,    JAN,    "1996,02,01",   0,       "1996,01,01"  },
            { L_,    JAN,    "1996,02,15",   0,       "1996,01,15"  },
            { L_,    JAN,    "1996,02,29",   0,       "1996,01,29"  },
            { L_,    FEB,    "1996,03,01",   0,       "1996,02,01"  },
            { L_,    FEB,    "1996,03,15",   0,       "1996,02,15"  },
            { L_,    FEB,    "1996,03,31",   0,       "1996,02,29"  },
            { L_,    JUN,    "1996,07,01",   0,       "1996,06,01"  },
            { L_,    JUN,    "1996,07,15",   0,       "1996,06,15"  },
            { L_,    JUN,    "1996,07,30",   0,       "1996,06,30"  },

            { L_,    JAN,    "1996,02,01",   1,       "1997,01,01"  },
            { L_,    JAN,    "1996,02,15",   1,       "1997,01,15"  },
            { L_,    JAN,    "1996,02,29",   1,       "1997,01,29"  },
            { L_,    FEB,    "1996,03,01",   1,       "1997,02,01"  },
            { L_,    FEB,    "1996,03,15",   1,       "1997,02,15"  },
            { L_,    FEB,    "1996,03,31",   1,       "1997,02,28"  },
            { L_,    JUN,    "1996,07,01",   1,       "1997,06,01"  },
            { L_,    JUN,    "1996,07,15",   1,       "1997,06,15"  },
            { L_,    JUN,    "1996,07,30",   1,       "1997,06,30"  },

            { L_,    JAN,    "1996,02,01",   2,       "1998,01,01"  },
            { L_,    JAN,    "1996,02,15",   2,       "1998,01,15"  },
            { L_,    JAN,    "1996,02,29",   2,       "1998,01,29"  },
            { L_,    FEB,    "1996,03,01",   2,       "1998,02,01"  },
            { L_,    FEB,    "1996,03,15",   2,       "1998,02,15"  },
            { L_,    FEB,    "1996,03,31",   2,       "1998,02,28"  },
            { L_,    JUN,    "1996,07,01",   2,       "1998,06,01"  },
            { L_,    JUN,    "1996,07,15",   2,       "1998,06,15"  },
            { L_,    JUN,    "1996,07,30",   2,       "1998,06,30"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const Month MONTH  = DATA[i].d_month;
            const Date  DATE   = parseDate(DATA[i].d_date);
            const int   COUNT  = DATA[i].d_count;
            const Date  RESULT = parseDate(DATA[i].d_result);

            if (veryVerbose) {
                T_ P_(LINE) P_(MONTH) P_(DATE) P_(COUNT) P(RESULT)
            }

            Date result = Util::floorAdjustMonth(MONTH, DATE, COUNT);
            LOOP3_ASSERT(LINE, RESULT, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'floorAdjustMonth' Function Test."
                          << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'ceilAdjustMonth2' FUNCTION
        //   This will test the 'ceilAdjustMonth2' function.
        //
        // Concerns:
        //   1. When 'count' is zero, the function must return 'ceilMonth'.
        //   2. When 'count' is not zero, the function must return the date
        //      that is 'count' years from 'ceilMonth'.
        //   3. The returned date's day is correctly capped by the number of
        //      days in the returned date's month and year *and* the number of
        //      days in 'ceilMonth's month and year.
        //
        // Plan:
        //   For a set of test data, exercise the 'ceilAdjustMonth2' function
        //   and check that the returned value is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //        month      date.day      date.month     count
        //        -----      --------      ----------     -----
        //
        //       | JAN |                                  | -2 |
        //       | FEB |     |    1 |     | month-1 |     | -1 |
        //       |     |  X  |   15 |  X  | month   |  X  |  0 |
        //       | JUN |     | last |     | month+1 |     |  1 |
        //       | DEC |                                  |  2 |
        //
        //   Note that not all months are included in the cross-product because
        //   this would drastically increase the size of the test vector,
        //   without contributing anything new to the test.  The selected
        //   months are chosen for the following reasons:
        //
        //       o JAN, DEC : first and last month (boundary check), both
        //                    contain 31 days.
        //       o FEB      : interesting because of leap year.
        //       o JUN      : contains 30 days.
        //
        //   The first and last days of each month are chosen for boundary
        //   checks.  The 15th of each month is also used as an additional
        //   check.  The last day is especially important in order to check
        //   that the returned date's day is capped correctly.
        //
        //   The values for 'date.month' are chosen to be one less than, equal
        //   to, and one more than the value of 'month'.  This is to match
        //   boundary conditions in 'ceilMonth' and 'adjustMonth'.
        //
        //   The values for 'count' are all valid 'count' values between -2 and
        //   +2.  Other values are not included because they do not contribute
        //   anything new to the test.
        //
        //   Note that if 'month' is JAN, then 'date.month' cannot be
        //   'month-1'.  Likewise, if 'month' is DEC, then 'date.month' cannot
        //   be 'month+1'.
        //
        //   Also note that 'date.year' can be any arbitrary value that does
        //   not make this operation undefined.  A leap year is *preferred*,
        //   though not necessary, in order to check that the returned date's
        //   day is capped correctly.
        //
        // Testing:
        //   b_D ceilAdjustMonth2(b_M month, c b_D& date, int count);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'ceilAdjustMonth2' Function"
                          << "\n===================================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Month       d_month;    // month
            const char *d_date;     // date
            int         d_count;    // count
            const char *d_result;   // expected result
        } DATA[] = {
            //line   month   date            count    result
            //----   -----   ----            -----    ------

            // date.month = month-1
            { L_,    FEB,    "1996,01,01",   -2,      "1994,02,01"  },
            { L_,    FEB,    "1996,01,15",   -2,      "1994,02,15"  },
            { L_,    FEB,    "1996,01,31",   -2,      "1994,02,28"  },
            { L_,    JUN,    "1996,05,01",   -2,      "1994,06,01"  },
            { L_,    JUN,    "1996,05,15",   -2,      "1994,06,15"  },
            { L_,    JUN,    "1996,05,31",   -2,      "1994,06,30"  },
            { L_,    DEC,    "1996,11,01",   -2,      "1994,12,01"  },
            { L_,    DEC,    "1996,11,15",   -2,      "1994,12,15"  },
            { L_,    DEC,    "1996,11,30",   -2,      "1994,12,30"  },

            { L_,    FEB,    "1996,01,01",   -1,      "1995,02,01"  },
            { L_,    FEB,    "1996,01,15",   -1,      "1995,02,15"  },
            { L_,    FEB,    "1996,01,31",   -1,      "1995,02,28"  },
            { L_,    JUN,    "1996,05,01",   -1,      "1995,06,01"  },
            { L_,    JUN,    "1996,05,15",   -1,      "1995,06,15"  },
            { L_,    JUN,    "1996,05,31",   -1,      "1995,06,30"  },
            { L_,    DEC,    "1996,11,01",   -1,      "1995,12,01"  },
            { L_,    DEC,    "1996,11,15",   -1,      "1995,12,15"  },
            { L_,    DEC,    "1996,11,30",   -1,      "1995,12,30"  },

            { L_,    FEB,    "1996,01,01",   0,       "1996,02,01"  },
            { L_,    FEB,    "1996,01,15",   0,       "1996,02,15"  },
            { L_,    FEB,    "1996,01,31",   0,       "1996,02,29"  },
            { L_,    JUN,    "1996,05,01",   0,       "1996,06,01"  },
            { L_,    JUN,    "1996,05,15",   0,       "1996,06,15"  },
            { L_,    JUN,    "1996,05,31",   0,       "1996,06,30"  },
            { L_,    DEC,    "1996,11,01",   0,       "1996,12,01"  },
            { L_,    DEC,    "1996,11,15",   0,       "1996,12,15"  },
            { L_,    DEC,    "1996,11,30",   0,       "1996,12,30"  },

            { L_,    FEB,    "1996,01,01",   1,       "1997,02,01"  },
            { L_,    FEB,    "1996,01,15",   1,       "1997,02,15"  },
            { L_,    FEB,    "1996,01,31",   1,       "1997,02,28"  },
            { L_,    JUN,    "1996,05,01",   1,       "1997,06,01"  },
            { L_,    JUN,    "1996,05,15",   1,       "1997,06,15"  },
            { L_,    JUN,    "1996,05,31",   1,       "1997,06,30"  },
            { L_,    DEC,    "1996,11,01",   1,       "1997,12,01"  },
            { L_,    DEC,    "1996,11,15",   1,       "1997,12,15"  },
            { L_,    DEC,    "1996,11,30",   1,       "1997,12,30"  },

            { L_,    FEB,    "1996,01,01",   2,       "1998,02,01"  },
            { L_,    FEB,    "1996,01,15",   2,       "1998,02,15"  },
            { L_,    FEB,    "1996,01,31",   2,       "1998,02,28"  },
            { L_,    JUN,    "1996,05,01",   2,       "1998,06,01"  },
            { L_,    JUN,    "1996,05,15",   2,       "1998,06,15"  },
            { L_,    JUN,    "1996,05,31",   2,       "1998,06,30"  },
            { L_,    DEC,    "1996,11,01",   2,       "1998,12,01"  },
            { L_,    DEC,    "1996,11,15",   2,       "1998,12,15"  },
            { L_,    DEC,    "1996,11,30",   2,       "1998,12,30"  },

            // date.month = month
            { L_,    JAN,    "1996,01,01",   -2,      "1994,01,01"  },
            { L_,    JAN,    "1996,01,15",   -2,      "1994,01,15"  },
            { L_,    JAN,    "1996,01,31",   -2,      "1994,01,31"  },
            { L_,    FEB,    "1996,02,01",   -2,      "1994,02,01"  },
            { L_,    FEB,    "1996,02,15",   -2,      "1994,02,15"  },
            { L_,    FEB,    "1996,02,29",   -2,      "1994,02,28"  },
            { L_,    JUN,    "1996,06,01",   -2,      "1994,06,01"  },
            { L_,    JUN,    "1996,06,15",   -2,      "1994,06,15"  },
            { L_,    JUN,    "1996,06,30",   -2,      "1994,06,30"  },
            { L_,    DEC,    "1996,12,01",   -2,      "1994,12,01"  },
            { L_,    DEC,    "1996,12,15",   -2,      "1994,12,15"  },
            { L_,    DEC,    "1996,12,31",   -2,      "1994,12,31"  },

            { L_,    JAN,    "1996,01,01",   -1,      "1995,01,01"  },
            { L_,    JAN,    "1996,01,15",   -1,      "1995,01,15"  },
            { L_,    JAN,    "1996,01,31",   -1,      "1995,01,31"  },
            { L_,    FEB,    "1996,02,01",   -1,      "1995,02,01"  },
            { L_,    FEB,    "1996,02,15",   -1,      "1995,02,15"  },
            { L_,    FEB,    "1996,02,29",   -1,      "1995,02,28"  },
            { L_,    JUN,    "1996,06,01",   -1,      "1995,06,01"  },
            { L_,    JUN,    "1996,06,15",   -1,      "1995,06,15"  },
            { L_,    JUN,    "1996,06,30",   -1,      "1995,06,30"  },
            { L_,    DEC,    "1996,12,01",   -1,      "1995,12,01"  },
            { L_,    DEC,    "1996,12,15",   -1,      "1995,12,15"  },
            { L_,    DEC,    "1996,12,31",   -1,      "1995,12,31"  },

            { L_,    JAN,    "1996,01,01",   0,       "1996,01,01"  },
            { L_,    JAN,    "1996,01,15",   0,       "1996,01,15"  },
            { L_,    JAN,    "1996,01,31",   0,       "1996,01,31"  },
            { L_,    FEB,    "1996,02,01",   0,       "1996,02,01"  },
            { L_,    FEB,    "1996,02,15",   0,       "1996,02,15"  },
            { L_,    FEB,    "1996,02,29",   0,       "1996,02,29"  },
            { L_,    JUN,    "1996,06,01",   0,       "1996,06,01"  },
            { L_,    JUN,    "1996,06,15",   0,       "1996,06,15"  },
            { L_,    JUN,    "1996,06,30",   0,       "1996,06,30"  },
            { L_,    DEC,    "1996,12,01",   0,       "1996,12,01"  },
            { L_,    DEC,    "1996,12,15",   0,       "1996,12,15"  },
            { L_,    DEC,    "1996,12,31",   0,       "1996,12,31"  },

            { L_,    JAN,    "1996,01,01",   1,       "1997,01,01"  },
            { L_,    JAN,    "1996,01,15",   1,       "1997,01,15"  },
            { L_,    JAN,    "1996,01,31",   1,       "1997,01,31"  },
            { L_,    FEB,    "1996,02,01",   1,       "1997,02,01"  },
            { L_,    FEB,    "1996,02,15",   1,       "1997,02,15"  },
            { L_,    FEB,    "1996,02,29",   1,       "1997,02,28"  },
            { L_,    JUN,    "1996,06,01",   1,       "1997,06,01"  },
            { L_,    JUN,    "1996,06,15",   1,       "1997,06,15"  },
            { L_,    JUN,    "1996,06,30",   1,       "1997,06,30"  },
            { L_,    DEC,    "1996,12,01",   1,       "1997,12,01"  },
            { L_,    DEC,    "1996,12,15",   1,       "1997,12,15"  },
            { L_,    DEC,    "1996,12,31",   1,       "1997,12,31"  },

            { L_,    JAN,    "1996,01,01",   2,       "1998,01,01"  },
            { L_,    JAN,    "1996,01,15",   2,       "1998,01,15"  },
            { L_,    JAN,    "1996,01,31",   2,       "1998,01,31"  },
            { L_,    FEB,    "1996,02,01",   2,       "1998,02,01"  },
            { L_,    FEB,    "1996,02,15",   2,       "1998,02,15"  },
            { L_,    FEB,    "1996,02,29",   2,       "1998,02,28"  },
            { L_,    JUN,    "1996,06,01",   2,       "1998,06,01"  },
            { L_,    JUN,    "1996,06,15",   2,       "1998,06,15"  },
            { L_,    JUN,    "1996,06,30",   2,       "1998,06,30"  },
            { L_,    DEC,    "1996,12,01",   2,       "1998,12,01"  },
            { L_,    DEC,    "1996,12,15",   2,       "1998,12,15"  },
            { L_,    DEC,    "1996,12,31",   2,       "1998,12,31"  },

            // date.month = month+1
            { L_,    JAN,    "1996,02,01",   -2,      "1995,01,01"  },
            { L_,    JAN,    "1996,02,15",   -2,      "1995,01,15"  },
            { L_,    JAN,    "1996,02,29",   -2,      "1995,01,29"  },
            { L_,    FEB,    "1996,03,01",   -2,      "1995,02,01"  },
            { L_,    FEB,    "1996,03,15",   -2,      "1995,02,15"  },
            { L_,    FEB,    "1996,03,31",   -2,      "1995,02,28"  },
            { L_,    JUN,    "1996,07,01",   -2,      "1995,06,01"  },
            { L_,    JUN,    "1996,07,15",   -2,      "1995,06,15"  },
            { L_,    JUN,    "1996,07,30",   -2,      "1995,06,30"  },

            { L_,    JAN,    "1996,02,01",   -1,      "1996,01,01"  },
            { L_,    JAN,    "1996,02,15",   -1,      "1996,01,15"  },
            { L_,    JAN,    "1996,02,29",   -1,      "1996,01,29"  },
            { L_,    FEB,    "1996,03,01",   -1,      "1996,02,01"  },
            { L_,    FEB,    "1996,03,15",   -1,      "1996,02,15"  },
            { L_,    FEB,    "1996,03,31",   -1,      "1996,02,28"  },
            { L_,    JUN,    "1996,07,01",   -1,      "1996,06,01"  },
            { L_,    JUN,    "1996,07,15",   -1,      "1996,06,15"  },
            { L_,    JUN,    "1996,07,30",   -1,      "1996,06,30"  },

            { L_,    JAN,    "1996,02,01",   0,       "1997,01,01"  },
            { L_,    JAN,    "1996,02,15",   0,       "1997,01,15"  },
            { L_,    JAN,    "1996,02,29",   0,       "1997,01,29"  },
            { L_,    FEB,    "1996,03,01",   0,       "1997,02,01"  },
            { L_,    FEB,    "1996,03,15",   0,       "1997,02,15"  },
            { L_,    FEB,    "1996,03,31",   0,       "1997,02,28"  },
            { L_,    JUN,    "1996,07,01",   0,       "1997,06,01"  },
            { L_,    JUN,    "1996,07,15",   0,       "1997,06,15"  },
            { L_,    JUN,    "1996,07,30",   0,       "1997,06,30"  },

            { L_,    JAN,    "1996,02,01",   1,       "1998,01,01"  },
            { L_,    JAN,    "1996,02,15",   1,       "1998,01,15"  },
            { L_,    JAN,    "1996,02,29",   1,       "1998,01,29"  },
            { L_,    FEB,    "1996,03,01",   1,       "1998,02,01"  },
            { L_,    FEB,    "1996,03,15",   1,       "1998,02,15"  },
            { L_,    FEB,    "1996,03,31",   1,       "1998,02,28"  },
            { L_,    JUN,    "1996,07,01",   1,       "1998,06,01"  },
            { L_,    JUN,    "1996,07,15",   1,       "1998,06,15"  },
            { L_,    JUN,    "1996,07,30",   1,       "1998,06,30"  },

            { L_,    JAN,    "1996,02,01",   2,       "1999,01,01"  },
            { L_,    JAN,    "1996,02,15",   2,       "1999,01,15"  },
            { L_,    JAN,    "1996,02,29",   2,       "1999,01,29"  },
            { L_,    FEB,    "1996,03,01",   2,       "1999,02,01"  },
            { L_,    FEB,    "1996,03,15",   2,       "1999,02,15"  },
            { L_,    FEB,    "1996,03,31",   2,       "1999,02,28"  },
            { L_,    JUN,    "1996,07,01",   2,       "1999,06,01"  },
            { L_,    JUN,    "1996,07,15",   2,       "1999,06,15"  },
            { L_,    JUN,    "1996,07,30",   2,       "1999,06,30"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const Month MONTH  = DATA[i].d_month;
            const Date  DATE   = parseDate(DATA[i].d_date);
            const int   COUNT  = DATA[i].d_count;
            const Date  RESULT = parseDate(DATA[i].d_result);

            if (veryVerbose) {
                T_ P_(LINE) P_(MONTH) P_(DATE) P_(COUNT) P(RESULT)
            }

            Date result = Util::ceilAdjustMonth2(MONTH, DATE, COUNT);
            LOOP3_ASSERT(LINE, RESULT, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'ceilAdjustMonth2' Function Test."
                          << endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'ceilAdjustMonth' FUNCTION
        //   This will test the 'ceilAdjustMonth' function.
        //
        // Concerns:
        //   1. When 'count' is zero, the function must return 'ceilMonth'.
        //   2. When 'count' is not zero, the function must return the date
        //      that is 'count' years from 'ceilMonth'.
        //   3. The returned date's day is correctly capped by the number of
        //      days in the returned date's month and year.
        //
        // Plan:
        //   For a set of test data, exercise the 'ceilAdjustMonth' function
        //   and check that the returned value is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //        month      date.day      date.month     count
        //        -----      --------      ----------     -----
        //
        //       | JAN |                                  | -2 |
        //       | FEB |     |    1 |     | month-1 |     | -1 |
        //       |     |  X  |   15 |  X  | month   |  X  |  0 |
        //       | JUN |     | last |     | month+1 |     |  1 |
        //       | DEC |                                  |  2 |
        //
        //   Note that not all months are included in the cross-product because
        //   this would drastically increase the size of the test vector,
        //   without contributing anything new to the test.  The selected
        //   months are chosen for the following reasons:
        //
        //       o JAN, DEC : first and last month (boundary check), both
        //                    contain 31 days.
        //       o FEB      : interesting because of leap year.
        //       o JUN      : contains 30 days.
        //
        //   The first and last days of each month are chosen for boundary
        //   checks.  The 15th of each month is also used as an additional
        //   check.  The last day is especially important in order to check
        //   that the returned date's day is capped correctly.
        //
        //   The values for 'date.month' are chosen to be one less than, equal
        //   to, and one more than the value of 'month'.  This is to match
        //   boundary conditions in 'ceilMonth' and 'adjustMonth'.
        //
        //   The values for 'count' are all valid 'count' values between -2 and
        //   +2.  Other values are not included because they do not contribute
        //   anything new to the test.
        //
        //   Note that if 'month' is JAN, then 'date.month' cannot be
        //   'month-1'.  Likewise, if 'month' is DEC, then 'date.month' cannot
        //   be 'month+1'.
        //
        //   Also note that 'date.year' can be any arbitrary value that does
        //   not make this operation undefined.  A leap year is *preferred*,
        //   though not necessary, in order to test the usage of
        //   'lastDayOfMonth'.
        //
        // Testing:
        //   b_D ceilAdjustMonth(b_M month, c b_D& date, int count);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'ceilAdjustMonth' Function"
                          << "\n==================================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Month       d_month;    // month
            const char *d_date;     // date
            int         d_count;    // count
            const char *d_result;   // expected result
        } DATA[] = {
            //line   month   date            count    result
            //----   -----   ----            -----    ------

            // date.month = month-1
            { L_,    FEB,    "1996,01,01",   -2,      "1994,02,01"  },
            { L_,    FEB,    "1996,01,15",   -2,      "1994,02,15"  },
            { L_,    FEB,    "1996,01,31",   -2,      "1994,02,28"  },
            { L_,    JUN,    "1996,05,01",   -2,      "1994,06,01"  },
            { L_,    JUN,    "1996,05,15",   -2,      "1994,06,15"  },
            { L_,    JUN,    "1996,05,31",   -2,      "1994,06,30"  },
            { L_,    DEC,    "1996,11,01",   -2,      "1994,12,01"  },
            { L_,    DEC,    "1996,11,15",   -2,      "1994,12,15"  },
            { L_,    DEC,    "1996,11,30",   -2,      "1994,12,30"  },

            { L_,    FEB,    "1996,01,01",   -1,      "1995,02,01"  },
            { L_,    FEB,    "1996,01,15",   -1,      "1995,02,15"  },
            { L_,    FEB,    "1996,01,31",   -1,      "1995,02,28"  },
            { L_,    JUN,    "1996,05,01",   -1,      "1995,06,01"  },
            { L_,    JUN,    "1996,05,15",   -1,      "1995,06,15"  },
            { L_,    JUN,    "1996,05,31",   -1,      "1995,06,30"  },
            { L_,    DEC,    "1996,11,01",   -1,      "1995,12,01"  },
            { L_,    DEC,    "1996,11,15",   -1,      "1995,12,15"  },
            { L_,    DEC,    "1996,11,30",   -1,      "1995,12,30"  },

            { L_,    FEB,    "1996,01,01",   0,       "1996,02,01"  },
            { L_,    FEB,    "1996,01,15",   0,       "1996,02,15"  },
            { L_,    FEB,    "1996,01,31",   0,       "1996,02,29"  },
            { L_,    JUN,    "1996,05,01",   0,       "1996,06,01"  },
            { L_,    JUN,    "1996,05,15",   0,       "1996,06,15"  },
            { L_,    JUN,    "1996,05,31",   0,       "1996,06,30"  },
            { L_,    DEC,    "1996,11,01",   0,       "1996,12,01"  },
            { L_,    DEC,    "1996,11,15",   0,       "1996,12,15"  },
            { L_,    DEC,    "1996,11,30",   0,       "1996,12,30"  },

            { L_,    FEB,    "1996,01,01",   1,       "1997,02,01"  },
            { L_,    FEB,    "1996,01,15",   1,       "1997,02,15"  },
            { L_,    FEB,    "1996,01,31",   1,       "1997,02,28"  },
            { L_,    JUN,    "1996,05,01",   1,       "1997,06,01"  },
            { L_,    JUN,    "1996,05,15",   1,       "1997,06,15"  },
            { L_,    JUN,    "1996,05,31",   1,       "1997,06,30"  },
            { L_,    DEC,    "1996,11,01",   1,       "1997,12,01"  },
            { L_,    DEC,    "1996,11,15",   1,       "1997,12,15"  },
            { L_,    DEC,    "1996,11,30",   1,       "1997,12,30"  },

            { L_,    FEB,    "1996,01,01",   2,       "1998,02,01"  },
            { L_,    FEB,    "1996,01,15",   2,       "1998,02,15"  },
            { L_,    FEB,    "1996,01,31",   2,       "1998,02,28"  },
            { L_,    JUN,    "1996,05,01",   2,       "1998,06,01"  },
            { L_,    JUN,    "1996,05,15",   2,       "1998,06,15"  },
            { L_,    JUN,    "1996,05,31",   2,       "1998,06,30"  },
            { L_,    DEC,    "1996,11,01",   2,       "1998,12,01"  },
            { L_,    DEC,    "1996,11,15",   2,       "1998,12,15"  },
            { L_,    DEC,    "1996,11,30",   2,       "1998,12,30"  },

            // date.month = month
            { L_,    JAN,    "1996,01,01",   -2,      "1994,01,01"  },
            { L_,    JAN,    "1996,01,15",   -2,      "1994,01,15"  },
            { L_,    JAN,    "1996,01,31",   -2,      "1994,01,31"  },
            { L_,    FEB,    "1996,02,01",   -2,      "1994,02,01"  },
            { L_,    FEB,    "1996,02,15",   -2,      "1994,02,15"  },
            { L_,    FEB,    "1996,02,29",   -2,      "1994,02,28"  },
            { L_,    JUN,    "1996,06,01",   -2,      "1994,06,01"  },
            { L_,    JUN,    "1996,06,15",   -2,      "1994,06,15"  },
            { L_,    JUN,    "1996,06,30",   -2,      "1994,06,30"  },
            { L_,    DEC,    "1996,12,01",   -2,      "1994,12,01"  },
            { L_,    DEC,    "1996,12,15",   -2,      "1994,12,15"  },
            { L_,    DEC,    "1996,12,31",   -2,      "1994,12,31"  },

            { L_,    JAN,    "1996,01,01",   -1,      "1995,01,01"  },
            { L_,    JAN,    "1996,01,15",   -1,      "1995,01,15"  },
            { L_,    JAN,    "1996,01,31",   -1,      "1995,01,31"  },
            { L_,    FEB,    "1996,02,01",   -1,      "1995,02,01"  },
            { L_,    FEB,    "1996,02,15",   -1,      "1995,02,15"  },
            { L_,    FEB,    "1996,02,29",   -1,      "1995,02,28"  },
            { L_,    JUN,    "1996,06,01",   -1,      "1995,06,01"  },
            { L_,    JUN,    "1996,06,15",   -1,      "1995,06,15"  },
            { L_,    JUN,    "1996,06,30",   -1,      "1995,06,30"  },
            { L_,    DEC,    "1996,12,01",   -1,      "1995,12,01"  },
            { L_,    DEC,    "1996,12,15",   -1,      "1995,12,15"  },
            { L_,    DEC,    "1996,12,31",   -1,      "1995,12,31"  },

            { L_,    JAN,    "1996,01,01",   0,       "1996,01,01"  },
            { L_,    JAN,    "1996,01,15",   0,       "1996,01,15"  },
            { L_,    JAN,    "1996,01,31",   0,       "1996,01,31"  },
            { L_,    FEB,    "1996,02,01",   0,       "1996,02,01"  },
            { L_,    FEB,    "1996,02,15",   0,       "1996,02,15"  },
            { L_,    FEB,    "1996,02,29",   0,       "1996,02,29"  },
            { L_,    JUN,    "1996,06,01",   0,       "1996,06,01"  },
            { L_,    JUN,    "1996,06,15",   0,       "1996,06,15"  },
            { L_,    JUN,    "1996,06,30",   0,       "1996,06,30"  },
            { L_,    DEC,    "1996,12,01",   0,       "1996,12,01"  },
            { L_,    DEC,    "1996,12,15",   0,       "1996,12,15"  },
            { L_,    DEC,    "1996,12,31",   0,       "1996,12,31"  },

            { L_,    JAN,    "1996,01,01",   1,       "1997,01,01"  },
            { L_,    JAN,    "1996,01,15",   1,       "1997,01,15"  },
            { L_,    JAN,    "1996,01,31",   1,       "1997,01,31"  },
            { L_,    FEB,    "1996,02,01",   1,       "1997,02,01"  },
            { L_,    FEB,    "1996,02,15",   1,       "1997,02,15"  },
            { L_,    FEB,    "1996,02,29",   1,       "1997,02,28"  },
            { L_,    JUN,    "1996,06,01",   1,       "1997,06,01"  },
            { L_,    JUN,    "1996,06,15",   1,       "1997,06,15"  },
            { L_,    JUN,    "1996,06,30",   1,       "1997,06,30"  },
            { L_,    DEC,    "1996,12,01",   1,       "1997,12,01"  },
            { L_,    DEC,    "1996,12,15",   1,       "1997,12,15"  },
            { L_,    DEC,    "1996,12,31",   1,       "1997,12,31"  },

            { L_,    JAN,    "1996,01,01",   2,       "1998,01,01"  },
            { L_,    JAN,    "1996,01,15",   2,       "1998,01,15"  },
            { L_,    JAN,    "1996,01,31",   2,       "1998,01,31"  },
            { L_,    FEB,    "1996,02,01",   2,       "1998,02,01"  },
            { L_,    FEB,    "1996,02,15",   2,       "1998,02,15"  },
            { L_,    FEB,    "1996,02,29",   2,       "1998,02,28"  },
            { L_,    JUN,    "1996,06,01",   2,       "1998,06,01"  },
            { L_,    JUN,    "1996,06,15",   2,       "1998,06,15"  },
            { L_,    JUN,    "1996,06,30",   2,       "1998,06,30"  },
            { L_,    DEC,    "1996,12,01",   2,       "1998,12,01"  },
            { L_,    DEC,    "1996,12,15",   2,       "1998,12,15"  },
            { L_,    DEC,    "1996,12,31",   2,       "1998,12,31"  },

            // date.month = month+1
            { L_,    JAN,    "1996,02,01",   -2,      "1995,01,01"  },
            { L_,    JAN,    "1996,02,15",   -2,      "1995,01,15"  },
            { L_,    JAN,    "1996,02,29",   -2,      "1995,01,29"  },
            { L_,    FEB,    "1996,03,01",   -2,      "1995,02,01"  },
            { L_,    FEB,    "1996,03,15",   -2,      "1995,02,15"  },
            { L_,    FEB,    "1996,03,31",   -2,      "1995,02,28"  },
            { L_,    JUN,    "1996,07,01",   -2,      "1995,06,01"  },
            { L_,    JUN,    "1996,07,15",   -2,      "1995,06,15"  },
            { L_,    JUN,    "1996,07,30",   -2,      "1995,06,30"  },

            { L_,    JAN,    "1996,02,01",   -1,      "1996,01,01"  },
            { L_,    JAN,    "1996,02,15",   -1,      "1996,01,15"  },
            { L_,    JAN,    "1996,02,29",   -1,      "1996,01,29"  },
            { L_,    FEB,    "1996,03,01",   -1,      "1996,02,01"  },
            { L_,    FEB,    "1996,03,15",   -1,      "1996,02,15"  },
            { L_,    FEB,    "1996,03,31",   -1,      "1996,02,29"  },
            { L_,    JUN,    "1996,07,01",   -1,      "1996,06,01"  },
            { L_,    JUN,    "1996,07,15",   -1,      "1996,06,15"  },
            { L_,    JUN,    "1996,07,30",   -1,      "1996,06,30"  },

            { L_,    JAN,    "1996,02,01",   0,       "1997,01,01"  },
            { L_,    JAN,    "1996,02,15",   0,       "1997,01,15"  },
            { L_,    JAN,    "1996,02,29",   0,       "1997,01,29"  },
            { L_,    FEB,    "1996,03,01",   0,       "1997,02,01"  },
            { L_,    FEB,    "1996,03,15",   0,       "1997,02,15"  },
            { L_,    FEB,    "1996,03,31",   0,       "1997,02,28"  },
            { L_,    JUN,    "1996,07,01",   0,       "1997,06,01"  },
            { L_,    JUN,    "1996,07,15",   0,       "1997,06,15"  },
            { L_,    JUN,    "1996,07,30",   0,       "1997,06,30"  },

            { L_,    JAN,    "1996,02,01",   1,       "1998,01,01"  },
            { L_,    JAN,    "1996,02,15",   1,       "1998,01,15"  },
            { L_,    JAN,    "1996,02,29",   1,       "1998,01,29"  },
            { L_,    FEB,    "1996,03,01",   1,       "1998,02,01"  },
            { L_,    FEB,    "1996,03,15",   1,       "1998,02,15"  },
            { L_,    FEB,    "1996,03,31",   1,       "1998,02,28"  },
            { L_,    JUN,    "1996,07,01",   1,       "1998,06,01"  },
            { L_,    JUN,    "1996,07,15",   1,       "1998,06,15"  },
            { L_,    JUN,    "1996,07,30",   1,       "1998,06,30"  },

            { L_,    JAN,    "1996,02,01",   2,       "1999,01,01"  },
            { L_,    JAN,    "1996,02,15",   2,       "1999,01,15"  },
            { L_,    JAN,    "1996,02,29",   2,       "1999,01,29"  },
            { L_,    FEB,    "1996,03,01",   2,       "1999,02,01"  },
            { L_,    FEB,    "1996,03,15",   2,       "1999,02,15"  },
            { L_,    FEB,    "1996,03,31",   2,       "1999,02,28"  },
            { L_,    JUN,    "1996,07,01",   2,       "1999,06,01"  },
            { L_,    JUN,    "1996,07,15",   2,       "1999,06,15"  },
            { L_,    JUN,    "1996,07,30",   2,       "1999,06,30"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const Month MONTH  = DATA[i].d_month;
            const Date  DATE   = parseDate(DATA[i].d_date);
            const int   COUNT  = DATA[i].d_count;
            const Date  RESULT = parseDate(DATA[i].d_result);

            if (veryVerbose) {
                T_ P_(LINE) P_(MONTH) P_(DATE) P_(COUNT) P(RESULT)
            }

            Date result = Util::ceilAdjustMonth(MONTH, DATE, COUNT);
            LOOP3_ASSERT(LINE, RESULT, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'ceilAdjustMonth' Function Test."
                          << endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'adjustMonth' FUNCTION
        //   This will test the 'adjustMonth' function.
        //
        // Concerns:
        //   1. Both positive and negative 'count' values can be used.
        //   2. The correct branch is executed based on the polarity of the
        //      'count' values.
        //   3. If count is positive, the returned date must be count *minus* 1
        //      years *after* nextMonth(month, date).
        //   4. If count is negative, the returned date must be count *plus* 1
        //      years *before* previousMonth(month, date).
        //   5. The returned date's day is correctly capped by the number of
        //      days in the returned date's month and year.
        //
        // Plan:
        //   For a set of test data, exercise the 'adjustMonth' function and
        //   check that the returned date is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //        month      date.day      date.month     count
        //        -----      --------      ----------     -----
        //
        //       | JAN |                                  | -2 |
        //       | FEB |     |    1 |     | month-1 |     | -1 |
        //       |     |  X  |   15 |  X  | month   |  X  |    |
        //       | JUN |     | last |     | month+1 |     |  1 |
        //       | DEC |                                  |  2 |
        //
        //   Note that not all months are included in the cross-product because
        //   this would drastically increase the size of the test vector,
        //   without contributing anything new to the test.  The selected
        //   months are chosen for the following reasons:
        //
        //       o JAN, DEC : first and last month (boundary check), both
        //                    contain 31 days.
        //       o FEB      : interesting because of leap year.
        //       o JUN      : contains 30 days.
        //
        //   The first and last days of each month are chosen for boundary
        //   checks.  The 15th of each month is also used as an additional
        //   check.  The last day is especially important in order to check
        //   that the returned date's day is capped correctly.
        //
        //   The values for 'date.month' are chosen to be one less than, equal
        //   to, and one more than the value of 'month'.  This is to test the
        //   '<' and '>' operations that compare these two values.
        //
        //   The values for 'count' are all valid 'count' values between -2 and
        //   +2.  Other values are not included because they do not contribute
        //   anything new to the test.
        //
        //   Note that if 'month' is JAN, then 'date.month' cannot be
        //   'month-1'.  Likewise, if 'month' is DEC, then 'date.month' cannot
        //   be 'month+1'.
        //
        //   Also note that 'date.year' can be any arbitrary value that does
        //   not make this operation undefined.  A leap year is *preferred*,
        //   though not necessary, in order to test the usage of
        //   'lastDayOfMonth'.
        //
        // Testing:
        //   b_D adjustMonth(b_M month, c b_D& date, int count);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'adjustMonth' Function"
                          << "\n==============================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Month       d_month;    // month
            const char *d_date;     // date
            int         d_count;    // count
            const char *d_result;   // expected result
        } DATA[] = {
            //line   month   date            count    result
            //----   -----   ----            -----    ------

            // date.month = month-1
            { L_,    FEB,    "1996,01,01",   -2,      "1994,02,01"  },
            { L_,    FEB,    "1996,01,15",   -2,      "1994,02,15"  },
            { L_,    FEB,    "1996,01,31",   -2,      "1994,02,28"  },
            { L_,    JUN,    "1996,05,01",   -2,      "1994,06,01"  },
            { L_,    JUN,    "1996,05,15",   -2,      "1994,06,15"  },
            { L_,    JUN,    "1996,05,31",   -2,      "1994,06,30"  },
            { L_,    DEC,    "1996,11,01",   -2,      "1994,12,01"  },
            { L_,    DEC,    "1996,11,15",   -2,      "1994,12,15"  },
            { L_,    DEC,    "1996,11,30",   -2,      "1994,12,30"  },

            { L_,    FEB,    "1996,01,01",   -1,      "1995,02,01"  },
            { L_,    FEB,    "1996,01,15",   -1,      "1995,02,15"  },
            { L_,    FEB,    "1996,01,31",   -1,      "1995,02,28"  },
            { L_,    JUN,    "1996,05,01",   -1,      "1995,06,01"  },
            { L_,    JUN,    "1996,05,15",   -1,      "1995,06,15"  },
            { L_,    JUN,    "1996,05,31",   -1,      "1995,06,30"  },
            { L_,    DEC,    "1996,11,01",   -1,      "1995,12,01"  },
            { L_,    DEC,    "1996,11,15",   -1,      "1995,12,15"  },
            { L_,    DEC,    "1996,11,30",   -1,      "1995,12,30"  },

            { L_,    FEB,    "1996,01,01",   1,       "1996,02,01"  },
            { L_,    FEB,    "1996,01,15",   1,       "1996,02,15"  },
            { L_,    FEB,    "1996,01,31",   1,       "1996,02,29"  },
            { L_,    JUN,    "1996,05,01",   1,       "1996,06,01"  },
            { L_,    JUN,    "1996,05,15",   1,       "1996,06,15"  },
            { L_,    JUN,    "1996,05,31",   1,       "1996,06,30"  },
            { L_,    DEC,    "1996,11,01",   1,       "1996,12,01"  },
            { L_,    DEC,    "1996,11,15",   1,       "1996,12,15"  },
            { L_,    DEC,    "1996,11,30",   1,       "1996,12,30"  },

            { L_,    FEB,    "1996,01,01",   2,       "1997,02,01"  },
            { L_,    FEB,    "1996,01,15",   2,       "1997,02,15"  },
            { L_,    FEB,    "1996,01,31",   2,       "1997,02,28"  },
            { L_,    JUN,    "1996,05,01",   2,       "1997,06,01"  },
            { L_,    JUN,    "1996,05,15",   2,       "1997,06,15"  },
            { L_,    JUN,    "1996,05,31",   2,       "1997,06,30"  },
            { L_,    DEC,    "1996,11,01",   2,       "1997,12,01"  },
            { L_,    DEC,    "1996,11,15",   2,       "1997,12,15"  },
            { L_,    DEC,    "1996,11,30",   2,       "1997,12,30"  },

            // date.month = month
            { L_,    JAN,    "1996,01,01",   -2,      "1994,01,01"  },
            { L_,    JAN,    "1996,01,15",   -2,      "1994,01,15"  },
            { L_,    JAN,    "1996,01,31",   -2,      "1994,01,31"  },
            { L_,    FEB,    "1996,02,01",   -2,      "1994,02,01"  },
            { L_,    FEB,    "1996,02,15",   -2,      "1994,02,15"  },
            { L_,    FEB,    "1996,02,29",   -2,      "1994,02,28"  },
            { L_,    JUN,    "1996,06,01",   -2,      "1994,06,01"  },
            { L_,    JUN,    "1996,06,15",   -2,      "1994,06,15"  },
            { L_,    JUN,    "1996,06,30",   -2,      "1994,06,30"  },
            { L_,    DEC,    "1996,12,01",   -2,      "1994,12,01"  },
            { L_,    DEC,    "1996,12,15",   -2,      "1994,12,15"  },
            { L_,    DEC,    "1996,12,31",   -2,      "1994,12,31"  },

            { L_,    JAN,    "1996,01,01",   -1,      "1995,01,01"  },
            { L_,    JAN,    "1996,01,15",   -1,      "1995,01,15"  },
            { L_,    JAN,    "1996,01,31",   -1,      "1995,01,31"  },
            { L_,    FEB,    "1996,02,01",   -1,      "1995,02,01"  },
            { L_,    FEB,    "1996,02,15",   -1,      "1995,02,15"  },
            { L_,    FEB,    "1996,02,29",   -1,      "1995,02,28"  },
            { L_,    JUN,    "1996,06,01",   -1,      "1995,06,01"  },
            { L_,    JUN,    "1996,06,15",   -1,      "1995,06,15"  },
            { L_,    JUN,    "1996,06,30",   -1,      "1995,06,30"  },
            { L_,    DEC,    "1996,12,01",   -1,      "1995,12,01"  },
            { L_,    DEC,    "1996,12,15",   -1,      "1995,12,15"  },
            { L_,    DEC,    "1996,12,31",   -1,      "1995,12,31"  },

            { L_,    JAN,    "1996,01,01",   1,       "1997,01,01"  },
            { L_,    JAN,    "1996,01,15",   1,       "1997,01,15"  },
            { L_,    JAN,    "1996,01,31",   1,       "1997,01,31"  },
            { L_,    FEB,    "1996,02,01",   1,       "1997,02,01"  },
            { L_,    FEB,    "1996,02,15",   1,       "1997,02,15"  },
            { L_,    FEB,    "1996,02,29",   1,       "1997,02,28"  },
            { L_,    JUN,    "1996,06,01",   1,       "1997,06,01"  },
            { L_,    JUN,    "1996,06,15",   1,       "1997,06,15"  },
            { L_,    JUN,    "1996,06,30",   1,       "1997,06,30"  },
            { L_,    DEC,    "1996,12,01",   1,       "1997,12,01"  },
            { L_,    DEC,    "1996,12,15",   1,       "1997,12,15"  },
            { L_,    DEC,    "1996,12,31",   1,       "1997,12,31"  },

            { L_,    JAN,    "1996,01,01",   2,       "1998,01,01"  },
            { L_,    JAN,    "1996,01,15",   2,       "1998,01,15"  },
            { L_,    JAN,    "1996,01,31",   2,       "1998,01,31"  },
            { L_,    FEB,    "1996,02,01",   2,       "1998,02,01"  },
            { L_,    FEB,    "1996,02,15",   2,       "1998,02,15"  },
            { L_,    FEB,    "1996,02,29",   2,       "1998,02,28"  },
            { L_,    JUN,    "1996,06,01",   2,       "1998,06,01"  },
            { L_,    JUN,    "1996,06,15",   2,       "1998,06,15"  },
            { L_,    JUN,    "1996,06,30",   2,       "1998,06,30"  },
            { L_,    DEC,    "1996,12,01",   2,       "1998,12,01"  },
            { L_,    DEC,    "1996,12,15",   2,       "1998,12,15"  },
            { L_,    DEC,    "1996,12,31",   2,       "1998,12,31"  },

            // date.month = month+1
            { L_,    JAN,    "1996,02,01",   -2,      "1995,01,01"  },
            { L_,    JAN,    "1996,02,15",   -2,      "1995,01,15"  },
            { L_,    JAN,    "1996,02,29",   -2,      "1995,01,29"  },
            { L_,    FEB,    "1996,03,01",   -2,      "1995,02,01"  },
            { L_,    FEB,    "1996,03,15",   -2,      "1995,02,15"  },
            { L_,    FEB,    "1996,03,31",   -2,      "1995,02,28"  },
            { L_,    JUN,    "1996,07,01",   -2,      "1995,06,01"  },
            { L_,    JUN,    "1996,07,15",   -2,      "1995,06,15"  },
            { L_,    JUN,    "1996,07,30",   -2,      "1995,06,30"  },

            { L_,    JAN,    "1996,02,01",   -1,      "1996,01,01"  },
            { L_,    JAN,    "1996,02,15",   -1,      "1996,01,15"  },
            { L_,    JAN,    "1996,02,29",   -1,      "1996,01,29"  },
            { L_,    FEB,    "1996,03,01",   -1,      "1996,02,01"  },
            { L_,    FEB,    "1996,03,15",   -1,      "1996,02,15"  },
            { L_,    FEB,    "1996,03,31",   -1,      "1996,02,29"  },
            { L_,    JUN,    "1996,07,01",   -1,      "1996,06,01"  },
            { L_,    JUN,    "1996,07,15",   -1,      "1996,06,15"  },
            { L_,    JUN,    "1996,07,30",   -1,      "1996,06,30"  },

            { L_,    JAN,    "1996,02,01",   1,       "1997,01,01"  },
            { L_,    JAN,    "1996,02,15",   1,       "1997,01,15"  },
            { L_,    JAN,    "1996,02,29",   1,       "1997,01,29"  },
            { L_,    FEB,    "1996,03,01",   1,       "1997,02,01"  },
            { L_,    FEB,    "1996,03,15",   1,       "1997,02,15"  },
            { L_,    FEB,    "1996,03,31",   1,       "1997,02,28"  },
            { L_,    JUN,    "1996,07,01",   1,       "1997,06,01"  },
            { L_,    JUN,    "1996,07,15",   1,       "1997,06,15"  },
            { L_,    JUN,    "1996,07,30",   1,       "1997,06,30"  },

            { L_,    JAN,    "1996,02,01",   2,       "1998,01,01"  },
            { L_,    JAN,    "1996,02,15",   2,       "1998,01,15"  },
            { L_,    JAN,    "1996,02,29",   2,       "1998,01,29"  },
            { L_,    FEB,    "1996,03,01",   2,       "1998,02,01"  },
            { L_,    FEB,    "1996,03,15",   2,       "1998,02,15"  },
            { L_,    FEB,    "1996,03,31",   2,       "1998,02,28"  },
            { L_,    JUN,    "1996,07,01",   2,       "1998,06,01"  },
            { L_,    JUN,    "1996,07,15",   2,       "1998,06,15"  },
            { L_,    JUN,    "1996,07,30",   2,       "1998,06,30"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const Month MONTH  = DATA[i].d_month;
            const Date  DATE   = parseDate(DATA[i].d_date);
            const int   COUNT  = DATA[i].d_count;
            const Date  RESULT = parseDate(DATA[i].d_result);

            if (veryVerbose) {
                T_ P_(LINE) P_(MONTH) P_(DATE) P_(COUNT) P(RESULT)
            }

            Date result = Util::adjustMonth(MONTH, DATE, COUNT);
            LOOP3_ASSERT(LINE, RESULT, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'adjustMonth' Function Test." << endl;
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'floorMonth' FUNCTION
        //   This will test the 'floorMonth' function.
        //
        // Concerns:
        //   1. The year must be decremented if 'date.month()' is less than
        //      'month'.
        //   2. The returned date's day is correctly capped by the number of
        //      days in the returned date's month and year.
        //
        // Plan:
        //   For a set of test data, exercise the 'floorMonth' function and
        //   check that the returned date is correct.
        //
        //   Data is selected based on the following cross-product:
        //
        //        month      date.day    date.month    date.year
        //        -----      --------    ----------    ---------
        //
        //       | JAN |                  | JAN |     |    1 |
        //       | FEB |     |    1 |     | FEB |     | 1996 |
        //       |     |  X  |   15 |  X  |     |  X  |      |
        //       | JUN |     | last |     | JUN |     | 1997 |
        //       | DEC |                  | DEC |     | 9999 |
        //
        //   Note that not all months are included in the cross-product because
        //   this would drastically increase the size of the test vector,
        //   without contributing anything new to the test.  The selected
        //   months are chosen for the following reasons:
        //
        //       o JAN, DEC : first and last month (boundary check), both
        //                    contain 31 days.
        //       o FEB      : interesting because of leap year.  Note that FEB
        //                    is also directly next to JAN, so this will also
        //                    test the '==' operation.
        //       o JUN      : contains 30 days.
        //
        //   The first and last days of each month are chosen for boundary
        //   checks.  The 15th of each month is also used as an additional
        //   check.  The last day is especially important in order to check
        //   that the returned date's day is capped correctly.
        //
        //   Years 1 and 9999 are chosen for boundary checks.  Years 1996 and
        //   1997 are used to test changes from non-leap-years to leap-years
        //   and vice-versa.
        //
        //   Also note that the following { month, date.month, year }
        //   combinations are undefined and are therefore not tested:
        //
        //       o { FEB, JAN, 1 }
        //       o { JUN, JAN, 1 }
        //       o { DEC, JAN, 1 }
        //       o { JUN, FEB, 1 }
        //       o { DEC, FEB, 1 }
        //       o { DEC, JUN, 1 }
        //
        // Testing:
        //   b_D floorMonth(b_M month, c b_D& date);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'floorMonth' Function"
                          << "\n=============================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Month       d_month;    // month
            const char *d_date;     // date
            const char *d_result;   // expected result
        } DATA[] = {
            //line      month      date           result
            //----      -----      ----           ------

            // date.month = JAN, date.year = 1
            { L_,       JAN,       "0001,01,01",  "0001,01,01"  },
            { L_,       JAN,       "0001,01,15",  "0001,01,15"  },
            { L_,       JAN,       "0001,01,31",  "0001,01,31"  },

            // date.month = FEB, date.year = 1
            { L_,       JAN,       "0001,02,01",  "0001,01,01"  },
            { L_,       JAN,       "0001,02,15",  "0001,01,15"  },
            { L_,       JAN,       "0001,02,28",  "0001,01,28"  },
            { L_,       FEB,       "0001,02,01",  "0001,02,01"  },
            { L_,       FEB,       "0001,02,15",  "0001,02,15"  },
            { L_,       FEB,       "0001,02,28",  "0001,02,28"  },

            // date.month = JUN, date.year = 1
            { L_,       JAN,       "0001,06,01",  "0001,01,01"  },
            { L_,       JAN,       "0001,06,15",  "0001,01,15"  },
            { L_,       JAN,       "0001,06,30",  "0001,01,30"  },
            { L_,       FEB,       "0001,06,01",  "0001,02,01"  },
            { L_,       FEB,       "0001,06,15",  "0001,02,15"  },
            { L_,       FEB,       "0001,06,30",  "0001,02,28"  },
            { L_,       JUN,       "0001,06,01",  "0001,06,01"  },
            { L_,       JUN,       "0001,06,15",  "0001,06,15"  },
            { L_,       JUN,       "0001,06,30",  "0001,06,30"  },

            // date.month = DEC, date.year = 1
            { L_,       JAN,       "0001,12,01",  "0001,01,01"  },
            { L_,       JAN,       "0001,12,15",  "0001,01,15"  },
            { L_,       JAN,       "0001,12,31",  "0001,01,31"  },
            { L_,       FEB,       "0001,12,01",  "0001,02,01"  },
            { L_,       FEB,       "0001,12,15",  "0001,02,15"  },
            { L_,       FEB,       "0001,12,31",  "0001,02,28"  },
            { L_,       JUN,       "0001,12,01",  "0001,06,01"  },
            { L_,       JUN,       "0001,12,15",  "0001,06,15"  },
            { L_,       JUN,       "0001,12,31",  "0001,06,30"  },
            { L_,       DEC,       "0001,12,01",  "0001,12,01"  },
            { L_,       DEC,       "0001,12,15",  "0001,12,15"  },
            { L_,       DEC,       "0001,12,31",  "0001,12,31"  },

            // date.month = JAN, date.year = 1996
            { L_,       JAN,       "1996,01,01",  "1996,01,01"  },
            { L_,       JAN,       "1996,01,15",  "1996,01,15"  },
            { L_,       JAN,       "1996,01,31",  "1996,01,31"  },
            { L_,       FEB,       "1996,01,01",  "1995,02,01"  },
            { L_,       FEB,       "1996,01,15",  "1995,02,15"  },
            { L_,       FEB,       "1996,01,31",  "1995,02,28"  },
            { L_,       JUN,       "1996,01,01",  "1995,06,01"  },
            { L_,       JUN,       "1996,01,15",  "1995,06,15"  },
            { L_,       JUN,       "1996,01,31",  "1995,06,30"  },
            { L_,       DEC,       "1996,01,01",  "1995,12,01"  },
            { L_,       DEC,       "1996,01,15",  "1995,12,15"  },
            { L_,       DEC,       "1996,01,31",  "1995,12,31"  },

            // date.month = FEB, date.year = 1996
            { L_,       JAN,       "1996,02,01",  "1996,01,01"  },
            { L_,       JAN,       "1996,02,15",  "1996,01,15"  },
            { L_,       JAN,       "1996,02,29",  "1996,01,29"  },
            { L_,       FEB,       "1996,02,01",  "1996,02,01"  },
            { L_,       FEB,       "1996,02,15",  "1996,02,15"  },
            { L_,       FEB,       "1996,02,29",  "1996,02,29"  },
            { L_,       JUN,       "1996,02,01",  "1995,06,01"  },
            { L_,       JUN,       "1996,02,15",  "1995,06,15"  },
            { L_,       JUN,       "1996,02,29",  "1995,06,29"  },
            { L_,       DEC,       "1996,02,01",  "1995,12,01"  },
            { L_,       DEC,       "1996,02,15",  "1995,12,15"  },
            { L_,       DEC,       "1996,02,29",  "1995,12,29"  },

            // date.month = JUN, date.year = 1996
            { L_,       JAN,       "1996,06,01",  "1996,01,01"  },
            { L_,       JAN,       "1996,06,15",  "1996,01,15"  },
            { L_,       JAN,       "1996,06,30",  "1996,01,30"  },
            { L_,       FEB,       "1996,06,01",  "1996,02,01"  },
            { L_,       FEB,       "1996,06,15",  "1996,02,15"  },
            { L_,       FEB,       "1996,06,30",  "1996,02,29"  },
            { L_,       JUN,       "1996,06,01",  "1996,06,01"  },
            { L_,       JUN,       "1996,06,15",  "1996,06,15"  },
            { L_,       JUN,       "1996,06,30",  "1996,06,30"  },
            { L_,       DEC,       "1996,06,01",  "1995,12,01"  },
            { L_,       DEC,       "1996,06,15",  "1995,12,15"  },
            { L_,       DEC,       "1996,06,30",  "1995,12,30"  },

            // date.month = DEC, date.year = 1996
            { L_,       JAN,       "1996,12,01",  "1996,01,01"  },
            { L_,       JAN,       "1996,12,15",  "1996,01,15"  },
            { L_,       JAN,       "1996,12,31",  "1996,01,31"  },
            { L_,       FEB,       "1996,12,01",  "1996,02,01"  },
            { L_,       FEB,       "1996,12,15",  "1996,02,15"  },
            { L_,       FEB,       "1996,12,31",  "1996,02,29"  },
            { L_,       JUN,       "1996,12,01",  "1996,06,01"  },
            { L_,       JUN,       "1996,12,15",  "1996,06,15"  },
            { L_,       JUN,       "1996,12,31",  "1996,06,30"  },
            { L_,       DEC,       "1996,12,01",  "1996,12,01"  },
            { L_,       DEC,       "1996,12,15",  "1996,12,15"  },
            { L_,       DEC,       "1996,12,31",  "1996,12,31"  },

            // date.month = JAN, date.year = 1997
            { L_,       JAN,       "1997,01,01",  "1997,01,01"  },
            { L_,       JAN,       "1997,01,15",  "1997,01,15"  },
            { L_,       JAN,       "1997,01,31",  "1997,01,31"  },
            { L_,       FEB,       "1997,01,01",  "1996,02,01"  },
            { L_,       FEB,       "1997,01,15",  "1996,02,15"  },
            { L_,       FEB,       "1997,01,31",  "1996,02,29"  },
            { L_,       JUN,       "1997,01,01",  "1996,06,01"  },
            { L_,       JUN,       "1997,01,15",  "1996,06,15"  },
            { L_,       JUN,       "1997,01,31",  "1996,06,30"  },
            { L_,       DEC,       "1997,01,01",  "1996,12,01"  },
            { L_,       DEC,       "1997,01,15",  "1996,12,15"  },
            { L_,       DEC,       "1997,01,31",  "1996,12,31"  },

            // date.month = FEB, date.year = 1997
            { L_,       JAN,       "1997,02,01",  "1997,01,01"  },
            { L_,       JAN,       "1997,02,15",  "1997,01,15"  },
            { L_,       JAN,       "1997,02,28",  "1997,01,28"  },
            { L_,       FEB,       "1997,02,01",  "1997,02,01"  },
            { L_,       FEB,       "1997,02,15",  "1997,02,15"  },
            { L_,       FEB,       "1997,02,28",  "1997,02,28"  },
            { L_,       JUN,       "1997,02,01",  "1996,06,01"  },
            { L_,       JUN,       "1997,02,15",  "1996,06,15"  },
            { L_,       JUN,       "1997,02,28",  "1996,06,28"  },
            { L_,       DEC,       "1997,02,01",  "1996,12,01"  },
            { L_,       DEC,       "1997,02,15",  "1996,12,15"  },
            { L_,       DEC,       "1997,02,28",  "1996,12,28"  },

            // date.month = JUN, date.year = 1997
            { L_,       JAN,       "1997,06,01",  "1997,01,01"  },
            { L_,       JAN,       "1997,06,15",  "1997,01,15"  },
            { L_,       JAN,       "1997,06,30",  "1997,01,30"  },
            { L_,       FEB,       "1997,06,01",  "1997,02,01"  },
            { L_,       FEB,       "1997,06,15",  "1997,02,15"  },
            { L_,       FEB,       "1997,06,30",  "1997,02,28"  },
            { L_,       JUN,       "1997,06,01",  "1997,06,01"  },
            { L_,       JUN,       "1997,06,15",  "1997,06,15"  },
            { L_,       JUN,       "1997,06,30",  "1997,06,30"  },
            { L_,       DEC,       "1997,06,01",  "1996,12,01"  },
            { L_,       DEC,       "1997,06,15",  "1996,12,15"  },
            { L_,       DEC,       "1997,06,30",  "1996,12,30"  },

            // date.month = DEC, date.year = 1997
            { L_,       JAN,       "1997,12,01",  "1997,01,01"  },
            { L_,       JAN,       "1997,12,15",  "1997,01,15"  },
            { L_,       JAN,       "1997,12,31",  "1997,01,31"  },
            { L_,       FEB,       "1997,12,01",  "1997,02,01"  },
            { L_,       FEB,       "1997,12,15",  "1997,02,15"  },
            { L_,       FEB,       "1997,12,31",  "1997,02,28"  },
            { L_,       JUN,       "1997,12,01",  "1997,06,01"  },
            { L_,       JUN,       "1997,12,15",  "1997,06,15"  },
            { L_,       JUN,       "1997,12,31",  "1997,06,30"  },
            { L_,       DEC,       "1997,12,01",  "1997,12,01"  },
            { L_,       DEC,       "1997,12,15",  "1997,12,15"  },
            { L_,       DEC,       "1997,12,31",  "1997,12,31"  },

            // date.month = JAN, date.year = 9999
            { L_,       JAN,       "9999,01,01",  "9999,01,01"  },
            { L_,       JAN,       "9999,01,15",  "9999,01,15"  },
            { L_,       JAN,       "9999,01,31",  "9999,01,31"  },
            { L_,       FEB,       "9999,01,01",  "9998,02,01"  },
            { L_,       FEB,       "9999,01,15",  "9998,02,15"  },
            { L_,       FEB,       "9999,01,31",  "9998,02,28"  },
            { L_,       JUN,       "9999,01,01",  "9998,06,01"  },
            { L_,       JUN,       "9999,01,15",  "9998,06,15"  },
            { L_,       JUN,       "9999,01,31",  "9998,06,30"  },
            { L_,       DEC,       "9999,01,01",  "9998,12,01"  },
            { L_,       DEC,       "9999,01,15",  "9998,12,15"  },
            { L_,       DEC,       "9999,01,31",  "9998,12,31"  },

            // date.month = FEB, date.year = 9999
            { L_,       JAN,       "9999,02,01",  "9999,01,01"  },
            { L_,       JAN,       "9999,02,15",  "9999,01,15"  },
            { L_,       JAN,       "9999,02,28",  "9999,01,28"  },
            { L_,       FEB,       "9999,02,01",  "9999,02,01"  },
            { L_,       FEB,       "9999,02,15",  "9999,02,15"  },
            { L_,       FEB,       "9999,02,28",  "9999,02,28"  },
            { L_,       JUN,       "9999,02,01",  "9998,06,01"  },
            { L_,       JUN,       "9999,02,15",  "9998,06,15"  },
            { L_,       JUN,       "9999,02,28",  "9998,06,28"  },
            { L_,       DEC,       "9999,02,01",  "9998,12,01"  },
            { L_,       DEC,       "9999,02,15",  "9998,12,15"  },
            { L_,       DEC,       "9999,02,28",  "9998,12,28"  },

            // date.month = JUN, date.year = 9999
            { L_,       JAN,       "9999,06,01",  "9999,01,01"  },
            { L_,       JAN,       "9999,06,15",  "9999,01,15"  },
            { L_,       JAN,       "9999,06,30",  "9999,01,30"  },
            { L_,       FEB,       "9999,06,01",  "9999,02,01"  },
            { L_,       FEB,       "9999,06,15",  "9999,02,15"  },
            { L_,       FEB,       "9999,06,30",  "9999,02,28"  },
            { L_,       JUN,       "9999,06,01",  "9999,06,01"  },
            { L_,       JUN,       "9999,06,15",  "9999,06,15"  },
            { L_,       JUN,       "9999,06,30",  "9999,06,30"  },
            { L_,       DEC,       "9999,06,01",  "9998,12,01"  },
            { L_,       DEC,       "9999,06,15",  "9998,12,15"  },
            { L_,       DEC,       "9999,06,30",  "9998,12,30"  },

            // date.month = DEC, date.year = 9999
            { L_,       JAN,       "9999,12,01",  "9999,01,01"  },
            { L_,       JAN,       "9999,12,15",  "9999,01,15"  },
            { L_,       JAN,       "9999,12,31",  "9999,01,31"  },
            { L_,       FEB,       "9999,12,01",  "9999,02,01"  },
            { L_,       FEB,       "9999,12,15",  "9999,02,15"  },
            { L_,       FEB,       "9999,12,31",  "9999,02,28"  },
            { L_,       JUN,       "9999,12,01",  "9999,06,01"  },
            { L_,       JUN,       "9999,12,15",  "9999,06,15"  },
            { L_,       JUN,       "9999,12,31",  "9999,06,30"  },
            { L_,       DEC,       "9999,12,01",  "9999,12,01"  },
            { L_,       DEC,       "9999,12,15",  "9999,12,15"  },
            { L_,       DEC,       "9999,12,31",  "9999,12,31"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const Month MONTH  = DATA[i].d_month;
            const Date  DATE   = parseDate(DATA[i].d_date);
            const Date  RESULT = parseDate(DATA[i].d_result);

            if (veryVerbose) {
                T_ P_(LINE) P_(MONTH) P_(DATE) P(RESULT)
            }

            Date result = Util::floorMonth(MONTH, DATE);
            LOOP3_ASSERT(LINE, RESULT, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'floorMonth' Function Test." << endl;
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'ceilMonth' FUNCTION
        //   This will test the 'ceilMonth' function.
        //
        // Concerns:
        //   1. The year must be incremented if 'date.month()' is greater than
        //      'month'.
        //   2. The returned date's day is correctly capped by the number of
        //      days in the returned date's month and year.
        //
        // Plan:
        //   For a set of test data, exercise the 'ceilMonth' function and
        //   check that the returned date is correct.
        //
        //   Data is selected based on the following cross-product:
        //
        //        month      date.day    date.month    date.year
        //        -----      --------    ----------    ---------
        //
        //       | JAN |                  | JAN |     |    1 |
        //       | FEB |     |    1 |     | FEB |     | 1995 |
        //       |     |  X  |   15 |  X  |     |  X  |      |
        //       | JUN |     | last |     | JUN |     | 1996 |
        //       | DEC |                  | DEC |     | 9999 |
        //
        //   Note that not all months are included in the cross-product because
        //   this would drastically increase the size of the test vector,
        //   without contributing anything new to the test.  The selected
        //   months are chosen for the following reasons:
        //
        //       o JAN, DEC : first and last month (boundary check), both
        //                    contain 31 days.
        //       o FEB      : interesting because of leap year.  Note that FEB
        //                    is also directly next to JAN, so this will also
        //                    test the '==' operation.
        //       o JUN      : contains 30 days.
        //
        //   The first and last days of each month are chosen for boundary
        //   checks.  The 15th of each month is also used as an additional
        //   check.  The last day is especially important in order to check
        //   that the returned date's day is capped correctly.
        //
        //   Years 1 and 9999 are chosen for boundary checks.  Years 1995 and
        //   1996 are used to test changes from non-leap-years to leap-years
        //   and vice-versa.
        //
        //   Also note that the following { month, date.month, year }
        //   combinations are undefined and are therefore not tested:
        //
        //       o { JAN, FEB, 9999 }
        //       o { JAN, JUN, 9999 }
        //       o { FEB, JUN, 9999 }
        //       o { JAN, DEC, 9999 }
        //       o { FEB, DEC, 9999 }
        //       o { JUN, DEC, 9999 }
        //
        // Testing:
        //   b_D ceilMonth(b_M month, c b_D& date);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'ceilMonth' Function"
                          << "\n============================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Month       d_month;    // month
            const char *d_date;     // date
            const char *d_result;   // expected result
        } DATA[] = {
            //line      month      date           result
            //----      -----      ----           ------

            // date.month = JAN, date.year = 1
            { L_,       JAN,       "0001,01,01",  "0001,01,01"  },
            { L_,       JAN,       "0001,01,15",  "0001,01,15"  },
            { L_,       JAN,       "0001,01,31",  "0001,01,31"  },
            { L_,       FEB,       "0001,01,01",  "0001,02,01"  },
            { L_,       FEB,       "0001,01,15",  "0001,02,15"  },
            { L_,       FEB,       "0001,01,31",  "0001,02,28"  },
            { L_,       JUN,       "0001,01,01",  "0001,06,01"  },
            { L_,       JUN,       "0001,01,15",  "0001,06,15"  },
            { L_,       JUN,       "0001,01,31",  "0001,06,30"  },
            { L_,       DEC,       "0001,01,01",  "0001,12,01"  },
            { L_,       DEC,       "0001,01,15",  "0001,12,15"  },
            { L_,       DEC,       "0001,01,31",  "0001,12,31"  },

            // date.month = FEB, date.year = 1
            { L_,       JAN,       "0001,02,01",  "0002,01,01"  },
            { L_,       JAN,       "0001,02,15",  "0002,01,15"  },
            { L_,       JAN,       "0001,02,28",  "0002,01,28"  },
            { L_,       FEB,       "0001,02,01",  "0001,02,01"  },
            { L_,       FEB,       "0001,02,15",  "0001,02,15"  },
            { L_,       FEB,       "0001,02,28",  "0001,02,28"  },
            { L_,       JUN,       "0001,02,01",  "0001,06,01"  },
            { L_,       JUN,       "0001,02,15",  "0001,06,15"  },
            { L_,       JUN,       "0001,02,28",  "0001,06,28"  },
            { L_,       DEC,       "0001,02,01",  "0001,12,01"  },
            { L_,       DEC,       "0001,02,15",  "0001,12,15"  },
            { L_,       DEC,       "0001,02,28",  "0001,12,28"  },

            // date.month = JUN, date.year = 1
            { L_,       JAN,       "0001,06,01",  "0002,01,01"  },
            { L_,       JAN,       "0001,06,15",  "0002,01,15"  },
            { L_,       JAN,       "0001,06,30",  "0002,01,30"  },
            { L_,       FEB,       "0001,06,01",  "0002,02,01"  },
            { L_,       FEB,       "0001,06,15",  "0002,02,15"  },
            { L_,       FEB,       "0001,06,30",  "0002,02,28"  },
            { L_,       JUN,       "0001,06,01",  "0001,06,01"  },
            { L_,       JUN,       "0001,06,15",  "0001,06,15"  },
            { L_,       JUN,       "0001,06,30",  "0001,06,30"  },
            { L_,       DEC,       "0001,06,01",  "0001,12,01"  },
            { L_,       DEC,       "0001,06,15",  "0001,12,15"  },
            { L_,       DEC,       "0001,06,30",  "0001,12,30"  },

            // date.month = DEC, date.year = 1
            { L_,       JAN,       "0001,12,01",  "0002,01,01"  },
            { L_,       JAN,       "0001,12,15",  "0002,01,15"  },
            { L_,       JAN,       "0001,12,31",  "0002,01,31"  },
            { L_,       FEB,       "0001,12,01",  "0002,02,01"  },
            { L_,       FEB,       "0001,12,15",  "0002,02,15"  },
            { L_,       FEB,       "0001,12,31",  "0002,02,28"  },
            { L_,       JUN,       "0001,12,01",  "0002,06,01"  },
            { L_,       JUN,       "0001,12,15",  "0002,06,15"  },
            { L_,       JUN,       "0001,12,31",  "0002,06,30"  },
            { L_,       DEC,       "0001,12,01",  "0001,12,01"  },
            { L_,       DEC,       "0001,12,15",  "0001,12,15"  },
            { L_,       DEC,       "0001,12,31",  "0001,12,31"  },

            // date.month = JAN, date.year = 1995
            { L_,       JAN,       "1995,01,01",  "1995,01,01"  },
            { L_,       JAN,       "1995,01,15",  "1995,01,15"  },
            { L_,       JAN,       "1995,01,31",  "1995,01,31"  },
            { L_,       FEB,       "1995,01,01",  "1995,02,01"  },
            { L_,       FEB,       "1995,01,15",  "1995,02,15"  },
            { L_,       FEB,       "1995,01,31",  "1995,02,28"  },
            { L_,       JUN,       "1995,01,01",  "1995,06,01"  },
            { L_,       JUN,       "1995,01,15",  "1995,06,15"  },
            { L_,       JUN,       "1995,01,31",  "1995,06,30"  },
            { L_,       DEC,       "1995,01,01",  "1995,12,01"  },
            { L_,       DEC,       "1995,01,15",  "1995,12,15"  },
            { L_,       DEC,       "1995,01,31",  "1995,12,31"  },

            // date.month = FEB, date.year = 1995
            { L_,       JAN,       "1995,02,01",  "1996,01,01"  },
            { L_,       JAN,       "1995,02,15",  "1996,01,15"  },
            { L_,       JAN,       "1995,02,28",  "1996,01,28"  },
            { L_,       FEB,       "1995,02,01",  "1995,02,01"  },
            { L_,       FEB,       "1995,02,15",  "1995,02,15"  },
            { L_,       FEB,       "1995,02,28",  "1995,02,28"  },
            { L_,       JUN,       "1995,02,01",  "1995,06,01"  },
            { L_,       JUN,       "1995,02,15",  "1995,06,15"  },
            { L_,       JUN,       "1995,02,28",  "1995,06,28"  },
            { L_,       DEC,       "1995,02,01",  "1995,12,01"  },
            { L_,       DEC,       "1995,02,15",  "1995,12,15"  },
            { L_,       DEC,       "1995,02,28",  "1995,12,28"  },

            // date.month = JUN, date.year = 1995
            { L_,       JAN,       "1995,06,01",  "1996,01,01"  },
            { L_,       JAN,       "1995,06,15",  "1996,01,15"  },
            { L_,       JAN,       "1995,06,30",  "1996,01,30"  },
            { L_,       FEB,       "1995,06,01",  "1996,02,01"  },
            { L_,       FEB,       "1995,06,15",  "1996,02,15"  },
            { L_,       FEB,       "1995,06,30",  "1996,02,29"  },
            { L_,       JUN,       "1995,06,01",  "1995,06,01"  },
            { L_,       JUN,       "1995,06,15",  "1995,06,15"  },
            { L_,       JUN,       "1995,06,30",  "1995,06,30"  },
            { L_,       DEC,       "1995,06,01",  "1995,12,01"  },
            { L_,       DEC,       "1995,06,15",  "1995,12,15"  },
            { L_,       DEC,       "1995,06,30",  "1995,12,30"  },

            // date.month = DEC, date.year = 1995
            { L_,       JAN,       "1995,12,01",  "1996,01,01"  },
            { L_,       JAN,       "1995,12,15",  "1996,01,15"  },
            { L_,       JAN,       "1995,12,31",  "1996,01,31"  },
            { L_,       FEB,       "1995,12,01",  "1996,02,01"  },
            { L_,       FEB,       "1995,12,15",  "1996,02,15"  },
            { L_,       FEB,       "1995,12,31",  "1996,02,29"  },
            { L_,       JUN,       "1995,12,01",  "1996,06,01"  },
            { L_,       JUN,       "1995,12,15",  "1996,06,15"  },
            { L_,       JUN,       "1995,12,31",  "1996,06,30"  },
            { L_,       DEC,       "1995,12,01",  "1995,12,01"  },
            { L_,       DEC,       "1995,12,15",  "1995,12,15"  },
            { L_,       DEC,       "1995,12,31",  "1995,12,31"  },

            // date.month = JAN, date.year = 1996
            { L_,       JAN,       "1996,01,01",  "1996,01,01"  },
            { L_,       JAN,       "1996,01,15",  "1996,01,15"  },
            { L_,       JAN,       "1996,01,31",  "1996,01,31"  },
            { L_,       FEB,       "1996,01,01",  "1996,02,01"  },
            { L_,       FEB,       "1996,01,15",  "1996,02,15"  },
            { L_,       FEB,       "1996,01,31",  "1996,02,29"  },
            { L_,       JUN,       "1996,01,01",  "1996,06,01"  },
            { L_,       JUN,       "1996,01,15",  "1996,06,15"  },
            { L_,       JUN,       "1996,01,31",  "1996,06,30"  },
            { L_,       DEC,       "1996,01,01",  "1996,12,01"  },
            { L_,       DEC,       "1996,01,15",  "1996,12,15"  },
            { L_,       DEC,       "1996,01,31",  "1996,12,31"  },

            // date.month = FEB, date.year = 1996
            { L_,       JAN,       "1996,02,01",  "1997,01,01"  },
            { L_,       JAN,       "1996,02,15",  "1997,01,15"  },
            { L_,       JAN,       "1996,02,29",  "1997,01,29"  },
            { L_,       FEB,       "1996,02,01",  "1996,02,01"  },
            { L_,       FEB,       "1996,02,15",  "1996,02,15"  },
            { L_,       FEB,       "1996,02,29",  "1996,02,29"  },
            { L_,       JUN,       "1996,02,01",  "1996,06,01"  },
            { L_,       JUN,       "1996,02,15",  "1996,06,15"  },
            { L_,       JUN,       "1996,02,29",  "1996,06,29"  },
            { L_,       DEC,       "1996,02,01",  "1996,12,01"  },
            { L_,       DEC,       "1996,02,15",  "1996,12,15"  },
            { L_,       DEC,       "1996,02,29",  "1996,12,29"  },

            // date.month = JUN, date.year = 1996
            { L_,       JAN,       "1996,06,01",  "1997,01,01"  },
            { L_,       JAN,       "1996,06,15",  "1997,01,15"  },
            { L_,       JAN,       "1996,06,30",  "1997,01,30"  },
            { L_,       FEB,       "1996,06,01",  "1997,02,01"  },
            { L_,       FEB,       "1996,06,15",  "1997,02,15"  },
            { L_,       FEB,       "1996,06,30",  "1997,02,28"  },
            { L_,       JUN,       "1996,06,01",  "1996,06,01"  },
            { L_,       JUN,       "1996,06,15",  "1996,06,15"  },
            { L_,       JUN,       "1996,06,30",  "1996,06,30"  },
            { L_,       DEC,       "1996,06,01",  "1996,12,01"  },
            { L_,       DEC,       "1996,06,15",  "1996,12,15"  },
            { L_,       DEC,       "1996,06,30",  "1996,12,30"  },

            // date.month = DEC, date.year = 1996
            { L_,       JAN,       "1996,12,01",  "1997,01,01"  },
            { L_,       JAN,       "1996,12,15",  "1997,01,15"  },
            { L_,       JAN,       "1996,12,31",  "1997,01,31"  },
            { L_,       FEB,       "1996,12,01",  "1997,02,01"  },
            { L_,       FEB,       "1996,12,15",  "1997,02,15"  },
            { L_,       FEB,       "1996,12,31",  "1997,02,28"  },
            { L_,       JUN,       "1996,12,01",  "1997,06,01"  },
            { L_,       JUN,       "1996,12,15",  "1997,06,15"  },
            { L_,       JUN,       "1996,12,31",  "1997,06,30"  },
            { L_,       DEC,       "1996,12,01",  "1996,12,01"  },
            { L_,       DEC,       "1996,12,15",  "1996,12,15"  },
            { L_,       DEC,       "1996,12,31",  "1996,12,31"  },

            // date.month = JAN, date.year = 9999
            { L_,       JAN,       "9999,01,01",  "9999,01,01"  },
            { L_,       JAN,       "9999,01,15",  "9999,01,15"  },
            { L_,       JAN,       "9999,01,31",  "9999,01,31"  },
            { L_,       FEB,       "9999,01,01",  "9999,02,01"  },
            { L_,       FEB,       "9999,01,15",  "9999,02,15"  },
            { L_,       FEB,       "9999,01,31",  "9999,02,28"  },
            { L_,       JUN,       "9999,01,01",  "9999,06,01"  },
            { L_,       JUN,       "9999,01,15",  "9999,06,15"  },
            { L_,       JUN,       "9999,01,31",  "9999,06,30"  },
            { L_,       DEC,       "9999,01,01",  "9999,12,01"  },
            { L_,       DEC,       "9999,01,15",  "9999,12,15"  },
            { L_,       DEC,       "9999,01,31",  "9999,12,31"  },

            // date.month = FEB, date.year = 9999
            { L_,       FEB,       "9999,02,01",  "9999,02,01"  },
            { L_,       FEB,       "9999,02,15",  "9999,02,15"  },
            { L_,       FEB,       "9999,02,28",  "9999,02,28"  },
            { L_,       JUN,       "9999,02,01",  "9999,06,01"  },
            { L_,       JUN,       "9999,02,15",  "9999,06,15"  },
            { L_,       JUN,       "9999,02,28",  "9999,06,28"  },
            { L_,       DEC,       "9999,02,01",  "9999,12,01"  },
            { L_,       DEC,       "9999,02,15",  "9999,12,15"  },
            { L_,       DEC,       "9999,02,28",  "9999,12,28"  },

            // date.month = JUN, date.year = 9999
            { L_,       JUN,       "9999,06,01",  "9999,06,01"  },
            { L_,       JUN,       "9999,06,15",  "9999,06,15"  },
            { L_,       JUN,       "9999,06,30",  "9999,06,30"  },
            { L_,       DEC,       "9999,06,01",  "9999,12,01"  },
            { L_,       DEC,       "9999,06,15",  "9999,12,15"  },
            { L_,       DEC,       "9999,06,30",  "9999,12,30"  },

            // date.month = DEC, date.year = 9999
            { L_,       DEC,       "9999,12,01",  "9999,12,01"  },
            { L_,       DEC,       "9999,12,15",  "9999,12,15"  },
            { L_,       DEC,       "9999,12,31",  "9999,12,31"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const Month MONTH  = DATA[i].d_month;
            const Date  DATE   = parseDate(DATA[i].d_date);
            const Date  RESULT = parseDate(DATA[i].d_result);

            if (veryVerbose) {
                T_ P_(LINE) P_(MONTH) P_(DATE) P(RESULT)
            }

            Date result = Util::ceilMonth(MONTH, DATE);
            LOOP3_ASSERT(LINE, RESULT, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'ceilMonth' Function Test." << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'previousMonth' FUNCTION
        //   This will test the 'previousMonth' function.
        //
        // Concerns:
        //   1. The year must be decremented if 'date.month()' is less than or
        //      equal to 'month'.
        //   2. The returned date's day is correctly capped by the number of
        //      days in the returned date's month and year.
        //
        // Plan:
        //   For a set of test data, exercise the 'previousMonth' function and
        //   check that the returned date is correct.
        //
        //   Data is selected based on the following cross-product:
        //
        //        month      date.day    date.month    date.year
        //        -----      --------    ----------    ---------
        //
        //       | JAN |                  | JAN |     |    1 |
        //       | FEB |     |    1 |     | FEB |     | 1996 |
        //       |     |  X  |   15 |  X  |     |  X  |      |
        //       | JUN |     | last |     | JUN |     | 1997 |
        //       | DEC |                  | DEC |     | 9999 |
        //
        //   Note that not all months are included in the cross-product because
        //   this would drastically increase the size of the test vector,
        //   without contributing anything new to the test.  The selected
        //   months are chosen for the following reasons:
        //
        //       o JAN, DEC : first and last month (boundary check), both
        //                    contain 31 days.
        //       o FEB      : interesting because of leap year.  Note that FEB
        //                    is also directly next to JAN, so this will also
        //                    test the '<=' operation.
        //       o JUN      : contains 30 days.
        //
        //   The first and last days of each month are chosen for boundary
        //   checks.  The 15th of each month is also used as an additional
        //   check.  The last day is especially important in order to check
        //   that the returned date's day is capped correctly.
        //
        //   Years 1 and 9999 are chosen for boundary checks.  Years 1996 and
        //   1997 are used to test changes from non-leap-years to leap-years
        //   and vice-versa.
        //
        //   Also note that the following { month, date.month, year }
        //   combinations are undefined and are therefore not tested:
        //
        //       o { JAN, JAN, 1 }
        //       o { FEB, JAN, 1 }
        //       o { JUN, JAN, 1 }
        //       o { DEC, JAN, 1 }
        //       o { FEB, FEB, 1 }
        //       o { JUN, FEB, 1 }
        //       o { DEC, FEB, 1 }
        //       o { JUN, JUN, 1 }
        //       o { DEC, JUN, 1 }
        //       o { DEC, DEC, 1 }
        //
        // Testing:
        //   b_D previousMonth(b_M month, c b_D& date);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'previousMonth' Function"
                          << "\n================================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Month       d_month;    // month
            const char *d_date;     // date
            const char *d_result;   // expected result
        } DATA[] = {
            //line      month      date           result
            //----      -----      ----           ------

            // date.month = FEB, date.year = 1
            { L_,       JAN,       "0001,02,01",  "0001,01,01"  },
            { L_,       JAN,       "0001,02,15",  "0001,01,15"  },
            { L_,       JAN,       "0001,02,28",  "0001,01,28"  },

            // date.month = JUN, date.year = 1
            { L_,       JAN,       "0001,06,01",  "0001,01,01"  },
            { L_,       JAN,       "0001,06,15",  "0001,01,15"  },
            { L_,       JAN,       "0001,06,30",  "0001,01,30"  },
            { L_,       FEB,       "0001,06,01",  "0001,02,01"  },
            { L_,       FEB,       "0001,06,15",  "0001,02,15"  },
            { L_,       FEB,       "0001,06,30",  "0001,02,28"  },

            // date.month = DEC, date.year = 1
            { L_,       JAN,       "0001,12,01",  "0001,01,01"  },
            { L_,       JAN,       "0001,12,15",  "0001,01,15"  },
            { L_,       JAN,       "0001,12,31",  "0001,01,31"  },
            { L_,       FEB,       "0001,12,01",  "0001,02,01"  },
            { L_,       FEB,       "0001,12,15",  "0001,02,15"  },
            { L_,       FEB,       "0001,12,31",  "0001,02,28"  },
            { L_,       JUN,       "0001,12,01",  "0001,06,01"  },
            { L_,       JUN,       "0001,12,15",  "0001,06,15"  },
            { L_,       JUN,       "0001,12,31",  "0001,06,30"  },

            // date.month = JAN, date.year = 1996
            { L_,       JAN,       "1996,01,01",  "1995,01,01"  },
            { L_,       JAN,       "1996,01,15",  "1995,01,15"  },
            { L_,       JAN,       "1996,01,31",  "1995,01,31"  },
            { L_,       FEB,       "1996,01,01",  "1995,02,01"  },
            { L_,       FEB,       "1996,01,15",  "1995,02,15"  },
            { L_,       FEB,       "1996,01,31",  "1995,02,28"  },
            { L_,       JUN,       "1996,01,01",  "1995,06,01"  },
            { L_,       JUN,       "1996,01,15",  "1995,06,15"  },
            { L_,       JUN,       "1996,01,31",  "1995,06,30"  },
            { L_,       DEC,       "1996,01,01",  "1995,12,01"  },
            { L_,       DEC,       "1996,01,15",  "1995,12,15"  },
            { L_,       DEC,       "1996,01,31",  "1995,12,31"  },

            // date.month = FEB, date.year = 1996
            { L_,       JAN,       "1996,02,01",  "1996,01,01"  },
            { L_,       JAN,       "1996,02,15",  "1996,01,15"  },
            { L_,       JAN,       "1996,02,29",  "1996,01,29"  },
            { L_,       FEB,       "1996,02,01",  "1995,02,01"  },
            { L_,       FEB,       "1996,02,15",  "1995,02,15"  },
            { L_,       FEB,       "1996,02,29",  "1995,02,28"  },
            { L_,       JUN,       "1996,02,01",  "1995,06,01"  },
            { L_,       JUN,       "1996,02,15",  "1995,06,15"  },
            { L_,       JUN,       "1996,02,29",  "1995,06,29"  },
            { L_,       DEC,       "1996,02,01",  "1995,12,01"  },
            { L_,       DEC,       "1996,02,15",  "1995,12,15"  },
            { L_,       DEC,       "1996,02,29",  "1995,12,29"  },

            // date.month = JUN, date.year = 1996
            { L_,       JAN,       "1996,06,01",  "1996,01,01"  },
            { L_,       JAN,       "1996,06,15",  "1996,01,15"  },
            { L_,       JAN,       "1996,06,30",  "1996,01,30"  },
            { L_,       FEB,       "1996,06,01",  "1996,02,01"  },
            { L_,       FEB,       "1996,06,15",  "1996,02,15"  },
            { L_,       FEB,       "1996,06,30",  "1996,02,29"  },
            { L_,       JUN,       "1996,06,01",  "1995,06,01"  },
            { L_,       JUN,       "1996,06,15",  "1995,06,15"  },
            { L_,       JUN,       "1996,06,30",  "1995,06,30"  },
            { L_,       DEC,       "1996,06,01",  "1995,12,01"  },
            { L_,       DEC,       "1996,06,15",  "1995,12,15"  },
            { L_,       DEC,       "1996,06,30",  "1995,12,30"  },

            // date.month = DEC, date.year = 1996
            { L_,       JAN,       "1996,12,01",  "1996,01,01"  },
            { L_,       JAN,       "1996,12,15",  "1996,01,15"  },
            { L_,       JAN,       "1996,12,31",  "1996,01,31"  },
            { L_,       FEB,       "1996,12,01",  "1996,02,01"  },
            { L_,       FEB,       "1996,12,15",  "1996,02,15"  },
            { L_,       FEB,       "1996,12,31",  "1996,02,29"  },
            { L_,       JUN,       "1996,12,01",  "1996,06,01"  },
            { L_,       JUN,       "1996,12,15",  "1996,06,15"  },
            { L_,       JUN,       "1996,12,31",  "1996,06,30"  },
            { L_,       DEC,       "1996,12,01",  "1995,12,01"  },
            { L_,       DEC,       "1996,12,15",  "1995,12,15"  },
            { L_,       DEC,       "1996,12,31",  "1995,12,31"  },

            // date.month = JAN, date.year = 1997
            { L_,       JAN,       "1997,01,01",  "1996,01,01"  },
            { L_,       JAN,       "1997,01,15",  "1996,01,15"  },
            { L_,       JAN,       "1997,01,31",  "1996,01,31"  },
            { L_,       FEB,       "1997,01,01",  "1996,02,01"  },
            { L_,       FEB,       "1997,01,15",  "1996,02,15"  },
            { L_,       FEB,       "1997,01,31",  "1996,02,29"  },
            { L_,       JUN,       "1997,01,01",  "1996,06,01"  },
            { L_,       JUN,       "1997,01,15",  "1996,06,15"  },
            { L_,       JUN,       "1997,01,31",  "1996,06,30"  },
            { L_,       DEC,       "1997,01,01",  "1996,12,01"  },
            { L_,       DEC,       "1997,01,15",  "1996,12,15"  },
            { L_,       DEC,       "1997,01,31",  "1996,12,31"  },

            // date.month = FEB, date.year = 1997
            { L_,       JAN,       "1997,02,01",  "1997,01,01"  },
            { L_,       JAN,       "1997,02,15",  "1997,01,15"  },
            { L_,       JAN,       "1997,02,28",  "1997,01,28"  },
            { L_,       FEB,       "1997,02,01",  "1996,02,01"  },
            { L_,       FEB,       "1997,02,15",  "1996,02,15"  },
            { L_,       FEB,       "1997,02,28",  "1996,02,28"  },
            { L_,       JUN,       "1997,02,01",  "1996,06,01"  },
            { L_,       JUN,       "1997,02,15",  "1996,06,15"  },
            { L_,       JUN,       "1997,02,28",  "1996,06,28"  },
            { L_,       DEC,       "1997,02,01",  "1996,12,01"  },
            { L_,       DEC,       "1997,02,15",  "1996,12,15"  },
            { L_,       DEC,       "1997,02,28",  "1996,12,28"  },

            // date.month = JUN, date.year = 1997
            { L_,       JAN,       "1997,06,01",  "1997,01,01"  },
            { L_,       JAN,       "1997,06,15",  "1997,01,15"  },
            { L_,       JAN,       "1997,06,30",  "1997,01,30"  },
            { L_,       FEB,       "1997,06,01",  "1997,02,01"  },
            { L_,       FEB,       "1997,06,15",  "1997,02,15"  },
            { L_,       FEB,       "1997,06,30",  "1997,02,28"  },
            { L_,       JUN,       "1997,06,01",  "1996,06,01"  },
            { L_,       JUN,       "1997,06,15",  "1996,06,15"  },
            { L_,       JUN,       "1997,06,30",  "1996,06,30"  },
            { L_,       DEC,       "1997,06,01",  "1996,12,01"  },
            { L_,       DEC,       "1997,06,15",  "1996,12,15"  },
            { L_,       DEC,       "1997,06,30",  "1996,12,30"  },

            // date.month = DEC, date.year = 1997
            { L_,       JAN,       "1997,12,01",  "1997,01,01"  },
            { L_,       JAN,       "1997,12,15",  "1997,01,15"  },
            { L_,       JAN,       "1997,12,31",  "1997,01,31"  },
            { L_,       FEB,       "1997,12,01",  "1997,02,01"  },
            { L_,       FEB,       "1997,12,15",  "1997,02,15"  },
            { L_,       FEB,       "1997,12,31",  "1997,02,28"  },
            { L_,       JUN,       "1997,12,01",  "1997,06,01"  },
            { L_,       JUN,       "1997,12,15",  "1997,06,15"  },
            { L_,       JUN,       "1997,12,31",  "1997,06,30"  },
            { L_,       DEC,       "1997,12,01",  "1996,12,01"  },
            { L_,       DEC,       "1997,12,15",  "1996,12,15"  },
            { L_,       DEC,       "1997,12,31",  "1996,12,31"  },

            // date.month = JAN, date.year = 9999
            { L_,       JAN,       "9999,01,01",  "9998,01,01"  },
            { L_,       JAN,       "9999,01,15",  "9998,01,15"  },
            { L_,       JAN,       "9999,01,31",  "9998,01,31"  },
            { L_,       FEB,       "9999,01,01",  "9998,02,01"  },
            { L_,       FEB,       "9999,01,15",  "9998,02,15"  },
            { L_,       FEB,       "9999,01,31",  "9998,02,28"  },
            { L_,       JUN,       "9999,01,01",  "9998,06,01"  },
            { L_,       JUN,       "9999,01,15",  "9998,06,15"  },
            { L_,       JUN,       "9999,01,31",  "9998,06,30"  },
            { L_,       DEC,       "9999,01,01",  "9998,12,01"  },
            { L_,       DEC,       "9999,01,15",  "9998,12,15"  },
            { L_,       DEC,       "9999,01,31",  "9998,12,31"  },

            // date.month = FEB, date.year = 9999
            { L_,       JAN,       "9999,02,01",  "9999,01,01"  },
            { L_,       JAN,       "9999,02,15",  "9999,01,15"  },
            { L_,       JAN,       "9999,02,28",  "9999,01,28"  },
            { L_,       FEB,       "9999,02,01",  "9998,02,01"  },
            { L_,       FEB,       "9999,02,15",  "9998,02,15"  },
            { L_,       FEB,       "9999,02,28",  "9998,02,28"  },
            { L_,       JUN,       "9999,02,01",  "9998,06,01"  },
            { L_,       JUN,       "9999,02,15",  "9998,06,15"  },
            { L_,       JUN,       "9999,02,28",  "9998,06,28"  },
            { L_,       DEC,       "9999,02,01",  "9998,12,01"  },
            { L_,       DEC,       "9999,02,15",  "9998,12,15"  },
            { L_,       DEC,       "9999,02,28",  "9998,12,28"  },

            // date.month = JUN, date.year = 9999
            { L_,       JAN,       "9999,06,01",  "9999,01,01"  },
            { L_,       JAN,       "9999,06,15",  "9999,01,15"  },
            { L_,       JAN,       "9999,06,30",  "9999,01,30"  },
            { L_,       FEB,       "9999,06,01",  "9999,02,01"  },
            { L_,       FEB,       "9999,06,15",  "9999,02,15"  },
            { L_,       FEB,       "9999,06,30",  "9999,02,28"  },
            { L_,       JUN,       "9999,06,01",  "9998,06,01"  },
            { L_,       JUN,       "9999,06,15",  "9998,06,15"  },
            { L_,       JUN,       "9999,06,30",  "9998,06,30"  },
            { L_,       DEC,       "9999,06,01",  "9998,12,01"  },
            { L_,       DEC,       "9999,06,15",  "9998,12,15"  },
            { L_,       DEC,       "9999,06,30",  "9998,12,30"  },

            // date.month = DEC, date.year = 9999
            { L_,       JAN,       "9999,12,01",  "9999,01,01"  },
            { L_,       JAN,       "9999,12,15",  "9999,01,15"  },
            { L_,       JAN,       "9999,12,31",  "9999,01,31"  },
            { L_,       FEB,       "9999,12,01",  "9999,02,01"  },
            { L_,       FEB,       "9999,12,15",  "9999,02,15"  },
            { L_,       FEB,       "9999,12,31",  "9999,02,28"  },
            { L_,       JUN,       "9999,12,01",  "9999,06,01"  },
            { L_,       JUN,       "9999,12,15",  "9999,06,15"  },
            { L_,       JUN,       "9999,12,31",  "9999,06,30"  },
            { L_,       DEC,       "9999,12,01",  "9998,12,01"  },
            { L_,       DEC,       "9999,12,15",  "9998,12,15"  },
            { L_,       DEC,       "9999,12,31",  "9998,12,31"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const Month MONTH  = DATA[i].d_month;
            const Date  DATE   = parseDate(DATA[i].d_date);
            const Date  RESULT = parseDate(DATA[i].d_result);

            if (veryVerbose) {
                T_ P_(LINE) P_(MONTH) P_(DATE) P(RESULT)
            }

            Date result = Util::previousMonth(MONTH, DATE);
            LOOP3_ASSERT(LINE, RESULT, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'previousMonth' Function Test." << endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'nextMonth' FUNCTION
        //   This will test the 'nextMonth' function.
        //
        // Concerns:
        //   1. The year must be incremented if 'date.month()' is greater than
        //      or equal to 'month'.
        //   2. The returned date's day is correctly capped by the number of
        //      days in the returned date's month and year.
        //
        // Plan:
        //   For a set of test data, exercise the 'nextMonth' function and
        //   check that the returned date is correct.
        //
        //   Data is selected based on the following cross-product:
        //
        //        month      date.day    date.month    date.year
        //        -----      --------    ----------    ---------
        //
        //       | JAN |                  | JAN |     |    1 |
        //       | FEB |     |    1 |     | FEB |     | 1995 |
        //       |     |  X  |   15 |  X  |     |  X  |      |
        //       | JUN |     | last |     | JUN |     | 1996 |
        //       | DEC |                  | DEC |     | 9999 |
        //
        //   Note that not all months are included in the cross-product because
        //   this would drastically increase the size of the test vector,
        //   without contributing anything new to the test.  The selected
        //   months are chosen for the following reasons:
        //
        //       o JAN, DEC : first and last month (boundary check), both
        //                    contain 31 days.
        //       o FEB      : interesting because of leap year.  Note that FEB
        //                    is also directly next to JAN, so this will also
        //                    test the '>=' operation.
        //       o JUN      : contains 30 days.
        //
        //   The first and last days of each month are chosen for boundary
        //   checks.  The 15th of each month is also used as an additional
        //   check.  The last day is especially important in order to check
        //   that the returned date's day is capped correctly.
        //
        //   Years 1 and 9999 are chosen for boundary checks.  Years 1995 and
        //   1996 are used to test changes from non-leap-years to leap-years
        //   and vice-versa.
        //
        //   Also note that the following { month, date.month, year }
        //   combinations are undefined and are therefore not tested:
        //
        //       o { JAN, JAN, 9999 }
        //       o { JAN, FEB, 9999 }
        //       o { FEB, FEB, 9999 }
        //       o { JAN, JUN, 9999 }
        //       o { FEB, JUN, 9999 }
        //       o { JUN, JUN, 9999 }
        //       o { JAN, DEC, 9999 }
        //       o { FEB, DEC, 9999 }
        //       o { JUN, DEC, 9999 }
        //       o { DEC, DEC, 9999 }
        //
        // Testing:
        //   b_D nextMonth(b_M month, c b_D& date);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'nextMonth' Function"
                          << "\n============================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Month       d_month;    // month
            const char *d_date;     // date
            const char *d_result;   // expected result
        } DATA[] = {
            //line      month      date           result
            //----      -----      ----           ------

            // date.month = JAN, date.year = 1
            { L_,       JAN,       "0001,01,01",  "0002,01,01"  },
            { L_,       JAN,       "0001,01,15",  "0002,01,15"  },
            { L_,       JAN,       "0001,01,31",  "0002,01,31"  },
            { L_,       FEB,       "0001,01,01",  "0001,02,01"  },
            { L_,       FEB,       "0001,01,15",  "0001,02,15"  },
            { L_,       FEB,       "0001,01,31",  "0001,02,28"  },
            { L_,       JUN,       "0001,01,01",  "0001,06,01"  },
            { L_,       JUN,       "0001,01,15",  "0001,06,15"  },
            { L_,       JUN,       "0001,01,31",  "0001,06,30"  },
            { L_,       DEC,       "0001,01,01",  "0001,12,01"  },
            { L_,       DEC,       "0001,01,15",  "0001,12,15"  },
            { L_,       DEC,       "0001,01,31",  "0001,12,31"  },

            // date.month = FEB, date.year = 1
            { L_,       JAN,       "0001,02,01",  "0002,01,01"  },
            { L_,       JAN,       "0001,02,15",  "0002,01,15"  },
            { L_,       JAN,       "0001,02,28",  "0002,01,28"  },
            { L_,       FEB,       "0001,02,01",  "0002,02,01"  },
            { L_,       FEB,       "0001,02,15",  "0002,02,15"  },
            { L_,       FEB,       "0001,02,28",  "0002,02,28"  },
            { L_,       JUN,       "0001,02,01",  "0001,06,01"  },
            { L_,       JUN,       "0001,02,15",  "0001,06,15"  },
            { L_,       JUN,       "0001,02,28",  "0001,06,28"  },
            { L_,       DEC,       "0001,02,01",  "0001,12,01"  },
            { L_,       DEC,       "0001,02,15",  "0001,12,15"  },
            { L_,       DEC,       "0001,02,28",  "0001,12,28"  },

            // date.month = JUN, date.year = 1
            { L_,       JAN,       "0001,06,01",  "0002,01,01"  },
            { L_,       JAN,       "0001,06,15",  "0002,01,15"  },
            { L_,       JAN,       "0001,06,30",  "0002,01,30"  },
            { L_,       FEB,       "0001,06,01",  "0002,02,01"  },
            { L_,       FEB,       "0001,06,15",  "0002,02,15"  },
            { L_,       FEB,       "0001,06,30",  "0002,02,28"  },
            { L_,       JUN,       "0001,06,01",  "0002,06,01"  },
            { L_,       JUN,       "0001,06,15",  "0002,06,15"  },
            { L_,       JUN,       "0001,06,30",  "0002,06,30"  },
            { L_,       DEC,       "0001,06,01",  "0001,12,01"  },
            { L_,       DEC,       "0001,06,15",  "0001,12,15"  },
            { L_,       DEC,       "0001,06,30",  "0001,12,30"  },

            // date.month = DEC, date.year = 1
            { L_,       JAN,       "0001,12,01",  "0002,01,01"  },
            { L_,       JAN,       "0001,12,15",  "0002,01,15"  },
            { L_,       JAN,       "0001,12,31",  "0002,01,31"  },
            { L_,       FEB,       "0001,12,01",  "0002,02,01"  },
            { L_,       FEB,       "0001,12,15",  "0002,02,15"  },
            { L_,       FEB,       "0001,12,31",  "0002,02,28"  },
            { L_,       JUN,       "0001,12,01",  "0002,06,01"  },
            { L_,       JUN,       "0001,12,15",  "0002,06,15"  },
            { L_,       JUN,       "0001,12,31",  "0002,06,30"  },
            { L_,       DEC,       "0001,12,01",  "0002,12,01"  },
            { L_,       DEC,       "0001,12,15",  "0002,12,15"  },
            { L_,       DEC,       "0001,12,31",  "0002,12,31"  },

            // date.month = JAN, date.year = 1995
            { L_,       JAN,       "1995,01,01",  "1996,01,01"  },
            { L_,       JAN,       "1995,01,15",  "1996,01,15"  },
            { L_,       JAN,       "1995,01,31",  "1996,01,31"  },
            { L_,       FEB,       "1995,01,01",  "1995,02,01"  },
            { L_,       FEB,       "1995,01,15",  "1995,02,15"  },
            { L_,       FEB,       "1995,01,31",  "1995,02,28"  },
            { L_,       JUN,       "1995,01,01",  "1995,06,01"  },
            { L_,       JUN,       "1995,01,15",  "1995,06,15"  },
            { L_,       JUN,       "1995,01,31",  "1995,06,30"  },
            { L_,       DEC,       "1995,01,01",  "1995,12,01"  },
            { L_,       DEC,       "1995,01,15",  "1995,12,15"  },
            { L_,       DEC,       "1995,01,31",  "1995,12,31"  },

            // date.month = FEB, date.year = 1995
            { L_,       JAN,       "1995,02,01",  "1996,01,01"  },
            { L_,       JAN,       "1995,02,15",  "1996,01,15"  },
            { L_,       JAN,       "1995,02,28",  "1996,01,28"  },
            { L_,       FEB,       "1995,02,01",  "1996,02,01"  },
            { L_,       FEB,       "1995,02,15",  "1996,02,15"  },
            { L_,       FEB,       "1995,02,28",  "1996,02,28"  },
            { L_,       JUN,       "1995,02,01",  "1995,06,01"  },
            { L_,       JUN,       "1995,02,15",  "1995,06,15"  },
            { L_,       JUN,       "1995,02,28",  "1995,06,28"  },
            { L_,       DEC,       "1995,02,01",  "1995,12,01"  },
            { L_,       DEC,       "1995,02,15",  "1995,12,15"  },
            { L_,       DEC,       "1995,02,28",  "1995,12,28"  },

            // date.month = JUN, date.year = 1995
            { L_,       JAN,       "1995,06,01",  "1996,01,01"  },
            { L_,       JAN,       "1995,06,15",  "1996,01,15"  },
            { L_,       JAN,       "1995,06,30",  "1996,01,30"  },
            { L_,       FEB,       "1995,06,01",  "1996,02,01"  },
            { L_,       FEB,       "1995,06,15",  "1996,02,15"  },
            { L_,       FEB,       "1995,06,30",  "1996,02,29"  },
            { L_,       JUN,       "1995,06,01",  "1996,06,01"  },
            { L_,       JUN,       "1995,06,15",  "1996,06,15"  },
            { L_,       JUN,       "1995,06,30",  "1996,06,30"  },
            { L_,       DEC,       "1995,06,01",  "1995,12,01"  },
            { L_,       DEC,       "1995,06,15",  "1995,12,15"  },
            { L_,       DEC,       "1995,06,30",  "1995,12,30"  },

            // date.month = DEC, date.year = 1995
            { L_,       JAN,       "1995,12,01",  "1996,01,01"  },
            { L_,       JAN,       "1995,12,15",  "1996,01,15"  },
            { L_,       JAN,       "1995,12,31",  "1996,01,31"  },
            { L_,       FEB,       "1995,12,01",  "1996,02,01"  },
            { L_,       FEB,       "1995,12,15",  "1996,02,15"  },
            { L_,       FEB,       "1995,12,31",  "1996,02,29"  },
            { L_,       JUN,       "1995,12,01",  "1996,06,01"  },
            { L_,       JUN,       "1995,12,15",  "1996,06,15"  },
            { L_,       JUN,       "1995,12,31",  "1996,06,30"  },
            { L_,       DEC,       "1995,12,01",  "1996,12,01"  },
            { L_,       DEC,       "1995,12,15",  "1996,12,15"  },
            { L_,       DEC,       "1995,12,31",  "1996,12,31"  },

            // date.month = JAN, date.year = 1996
            { L_,       JAN,       "1996,01,01",  "1997,01,01"  },
            { L_,       JAN,       "1996,01,15",  "1997,01,15"  },
            { L_,       JAN,       "1996,01,31",  "1997,01,31"  },
            { L_,       FEB,       "1996,01,01",  "1996,02,01"  },
            { L_,       FEB,       "1996,01,15",  "1996,02,15"  },
            { L_,       FEB,       "1996,01,31",  "1996,02,29"  },
            { L_,       JUN,       "1996,01,01",  "1996,06,01"  },
            { L_,       JUN,       "1996,01,15",  "1996,06,15"  },
            { L_,       JUN,       "1996,01,31",  "1996,06,30"  },
            { L_,       DEC,       "1996,01,01",  "1996,12,01"  },
            { L_,       DEC,       "1996,01,15",  "1996,12,15"  },
            { L_,       DEC,       "1996,01,31",  "1996,12,31"  },

            // date.month = FEB, date.year = 1996
            { L_,       JAN,       "1996,02,01",  "1997,01,01"  },
            { L_,       JAN,       "1996,02,15",  "1997,01,15"  },
            { L_,       JAN,       "1996,02,29",  "1997,01,29"  },
            { L_,       FEB,       "1996,02,01",  "1997,02,01"  },
            { L_,       FEB,       "1996,02,15",  "1997,02,15"  },
            { L_,       FEB,       "1996,02,29",  "1997,02,28"  },
            { L_,       JUN,       "1996,02,01",  "1996,06,01"  },
            { L_,       JUN,       "1996,02,15",  "1996,06,15"  },
            { L_,       JUN,       "1996,02,29",  "1996,06,29"  },
            { L_,       DEC,       "1996,02,01",  "1996,12,01"  },
            { L_,       DEC,       "1996,02,15",  "1996,12,15"  },
            { L_,       DEC,       "1996,02,29",  "1996,12,29"  },

            // date.month = JUN, date.year = 1996
            { L_,       JAN,       "1996,06,01",  "1997,01,01"  },
            { L_,       JAN,       "1996,06,15",  "1997,01,15"  },
            { L_,       JAN,       "1996,06,30",  "1997,01,30"  },
            { L_,       FEB,       "1996,06,01",  "1997,02,01"  },
            { L_,       FEB,       "1996,06,15",  "1997,02,15"  },
            { L_,       FEB,       "1996,06,30",  "1997,02,28"  },
            { L_,       JUN,       "1996,06,01",  "1997,06,01"  },
            { L_,       JUN,       "1996,06,15",  "1997,06,15"  },
            { L_,       JUN,       "1996,06,30",  "1997,06,30"  },
            { L_,       DEC,       "1996,06,01",  "1996,12,01"  },
            { L_,       DEC,       "1996,06,15",  "1996,12,15"  },
            { L_,       DEC,       "1996,06,30",  "1996,12,30"  },

            // date.month = DEC, date.year = 1996
            { L_,       JAN,       "1996,12,01",  "1997,01,01"  },
            { L_,       JAN,       "1996,12,15",  "1997,01,15"  },
            { L_,       JAN,       "1996,12,31",  "1997,01,31"  },
            { L_,       FEB,       "1996,12,01",  "1997,02,01"  },
            { L_,       FEB,       "1996,12,15",  "1997,02,15"  },
            { L_,       FEB,       "1996,12,31",  "1997,02,28"  },
            { L_,       JUN,       "1996,12,01",  "1997,06,01"  },
            { L_,       JUN,       "1996,12,15",  "1997,06,15"  },
            { L_,       JUN,       "1996,12,31",  "1997,06,30"  },
            { L_,       DEC,       "1996,12,01",  "1997,12,01"  },
            { L_,       DEC,       "1996,12,15",  "1997,12,15"  },
            { L_,       DEC,       "1996,12,31",  "1997,12,31"  },

            // date.month = JAN, date.year = 9999
            { L_,       FEB,       "9999,01,01",  "9999,02,01"  },
            { L_,       FEB,       "9999,01,15",  "9999,02,15"  },
            { L_,       FEB,       "9999,01,31",  "9999,02,28"  },
            { L_,       JUN,       "9999,01,01",  "9999,06,01"  },
            { L_,       JUN,       "9999,01,15",  "9999,06,15"  },
            { L_,       JUN,       "9999,01,31",  "9999,06,30"  },
            { L_,       DEC,       "9999,01,01",  "9999,12,01"  },
            { L_,       DEC,       "9999,01,15",  "9999,12,15"  },
            { L_,       DEC,       "9999,01,31",  "9999,12,31"  },

            // date.month = FEB, date.year = 9999
            { L_,       JUN,       "9999,02,01",  "9999,06,01"  },
            { L_,       JUN,       "9999,02,15",  "9999,06,15"  },
            { L_,       JUN,       "9999,02,28",  "9999,06,28"  },
            { L_,       DEC,       "9999,02,01",  "9999,12,01"  },
            { L_,       DEC,       "9999,02,15",  "9999,12,15"  },
            { L_,       DEC,       "9999,02,28",  "9999,12,28"  },

            // date.month = JUN, date.year = 9999
            { L_,       DEC,       "9999,06,01",  "9999,12,01"  },
            { L_,       DEC,       "9999,06,15",  "9999,12,15"  },
            { L_,       DEC,       "9999,06,30",  "9999,12,30"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_lineNum;
            const Month MONTH  = DATA[i].d_month;
            const Date  DATE   = parseDate(DATA[i].d_date);
            const Date  RESULT = parseDate(DATA[i].d_result);

            if (veryVerbose) {
                T_ P_(LINE) P_(MONTH) P_(DATE) P(RESULT)
            }

            Date result = Util::nextMonth(MONTH, DATE);
            LOOP3_ASSERT(LINE, RESULT, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'nextMonth' Function Test." << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'parseDate' GLOBAL HELPER FUNCTION
        //   This will test the 'parseDate' global helper function.
        //
        // Concerns:
        //   1. All valid date ranges must be supported.
        //
        // Plan:
        //   For a set of test data, exercise the 'parseDate' global helper
        //   function and verify that the returned 'bdet_Date' object
        //   corresponds to the input string.
        //
        //   The dataset includes the first and last day of each month for the
        //   years 0001 and 9999.
        //
        // Testing:
        //   b_D parseDate(const char *str);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'parseDate' Global Helper Function"
                          << "\n=========================================="
                          << endl;

        static const struct {
            int         d_lineNum;
            const char *d_date;
            int         d_year;
            int         d_month;
            int         d_day;
        } DATA[] = {
            //line   date           year    month    day
            //----   ----           ----    -----    ---
            { L_,    "0001,01,01",  1,      1,       1    },
            { L_,    "0001,01,31",  1,      1,       31   },
            { L_,    "0001,02,01",  1,      2,       1    },
            { L_,    "0001,02,28",  1,      2,       28   },
            { L_,    "0001,03,01",  1,      3,       1    },
            { L_,    "0001,03,31",  1,      3,       31   },
            { L_,    "0001,04,01",  1,      4,       1    },
            { L_,    "0001,04,30",  1,      4,       30   },
            { L_,    "0001,05,01",  1,      5,       1    },
            { L_,    "0001,05,31",  1,      5,       31   },
            { L_,    "0001,06,01",  1,      6,       1    },
            { L_,    "0001,06,30",  1,      6,       30   },
            { L_,    "0001,07,01",  1,      7,       1    },
            { L_,    "0001,07,31",  1,      7,       31   },
            { L_,    "0001,08,01",  1,      8,       1    },
            { L_,    "0001,08,31",  1,      8,       31   },
            { L_,    "0001,09,01",  1,      9,       1    },
            { L_,    "0001,09,30",  1,      9,       30   },
            { L_,    "0001,10,01",  1,      10,      1    },
            { L_,    "0001,10,31",  1,      10,      31   },
            { L_,    "0001,11,01",  1,      11,      1    },
            { L_,    "0001,11,30",  1,      11,      30   },
            { L_,    "0001,12,01",  1,      12,      1    },
            { L_,    "0001,12,31",  1,      12,      31   },

            { L_,    "9999,01,01",  9999,   1,       1    },
            { L_,    "9999,01,31",  9999,   1,       31   },
            { L_,    "9999,02,01",  9999,   2,       1    },
            { L_,    "9999,02,28",  9999,   2,       28   },
            { L_,    "9999,03,01",  9999,   3,       1    },
            { L_,    "9999,03,31",  9999,   3,       31   },
            { L_,    "9999,04,01",  9999,   4,       1    },
            { L_,    "9999,04,30",  9999,   4,       30   },
            { L_,    "9999,05,01",  9999,   5,       1    },
            { L_,    "9999,05,31",  9999,   5,       31   },
            { L_,    "9999,06,01",  9999,   6,       1    },
            { L_,    "9999,06,30",  9999,   6,       30   },
            { L_,    "9999,07,01",  9999,   7,       1    },
            { L_,    "9999,07,31",  9999,   7,       31   },
            { L_,    "9999,08,01",  9999,   8,       1    },
            { L_,    "9999,08,31",  9999,   8,       31   },
            { L_,    "9999,09,01",  9999,   9,       1    },
            { L_,    "9999,09,30",  9999,   9,       30   },
            { L_,    "9999,10,01",  9999,   10,      1    },
            { L_,    "9999,10,31",  9999,   10,      31   },
            { L_,    "9999,11,01",  9999,   11,      1    },
            { L_,    "9999,11,30",  9999,   11,      30   },
            { L_,    "9999,12,01",  9999,   12,      1    },
            { L_,    "9999,12,31",  9999,   12,      31   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_lineNum;
            const char *DATE  = DATA[i].d_date;
            const int   YEAR  = DATA[i].d_year;
            const int   MONTH = DATA[i].d_month;
            const int   DAY   = DATA[i].d_day;

            if (veryVerbose) {
                T_ P_(LINE) P_(DATE) P_(YEAR) P_(MONTH) P(DAY)
            }

            bdet_Date date = parseDate(DATE);

            int year  = date.year();
            int month = date.month();
            int day   = date.day();

            LOOP3_ASSERT(LINE, YEAR,  year,  YEAR  == year);
            LOOP3_ASSERT(LINE, MONTH, month, MONTH == month);
            LOOP3_ASSERT(LINE, DAY,   day,   DAY   == day);
        }

        if (verbose) cout << "\nEnd of 'parseDate' Global Helper Function "
                          << "Test." << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This is a breathing test that exercises basic functionality, but
        //   tests nothing.
        //
        // Concerns:
        //   The component must behave as expected.
        //
        // Plan:
        //   Exercise each function in the component using any arbitrary
        //   argument values and check that the returned value is as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) cout << "\nBreathing Test"
                          << "\n==============" << endl;
        // TBD
        if (verbose) cout << "\nEnd of Breathing Test." << endl;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
