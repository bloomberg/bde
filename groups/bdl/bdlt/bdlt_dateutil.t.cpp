// bdlt_dateutil.t.cpp                                                -*-C++-*-
#include <bdlt_dateutil.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>
#include <bsls_stopwatch.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bdlt::DateUtil' provides a suite of functions for manipulating dates
// without the use of a calendar.  This test driver tests each implemented
// utility function independently.
//
//-----------------------------------------------------------------------------
// CLASS METHODS
// [13] Date addMonths(original, numMonths, eomFlag);
// [12] Date addMonthsEom(original, numMonths);
// [11] Date addMonthsNoEom(original, numMonths);
// [16] Date addYears(original, numYears, eomFlag);
// [14] Date addYearsEom(original, numYears);
// [15] Date addYearsNoEom(original, numYears);
// [ 3] int convertFromYYYYMMDD(Date *result, int yyyymmddValue);
// [ 2] Date convertFromYYYYMMDDRaw(int yyyymmddValue);
// [ 4] int convertToYYYYMMDD(const Date& date);
// [ 1] bool isValidYYYYMMDD(int yyyymmddValue);
// [10] Date lastDayOfWeekInMonth(year, month, dayOfWeek);
// [ 5] Date nextDayOfWeek(dayOfWeek, date);
// [ 6] Date nextDayOfWeekInclusive(dayOfWeek, date);
// [ 9] Date nthDayOfWeekInMonth(year, month, dayOfWeek, n);
// [ 7] Date previousDayOfWeek(dayOfWeek, date);
// [ 8] Date previousDayOfWeekInclusive(dayOfWeek, date);
// ----------------------------------------------------------------------------
// [17] USAGE EXAMPLE

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
typedef bdlt::DateUtil Util;

const bdlt::DayOfWeek::Enum e_SUN = bdlt::DayOfWeek::e_SUN;
const bdlt::DayOfWeek::Enum e_MON = bdlt::DayOfWeek::e_MON;
const bdlt::DayOfWeek::Enum e_TUE = bdlt::DayOfWeek::e_TUE;
const bdlt::DayOfWeek::Enum e_WED = bdlt::DayOfWeek::e_WED;
const bdlt::DayOfWeek::Enum e_THU = bdlt::DayOfWeek::e_THU;
const bdlt::DayOfWeek::Enum e_FRI = bdlt::DayOfWeek::e_FRI;
const bdlt::DayOfWeek::Enum e_SAT = bdlt::DayOfWeek::e_SAT;

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)     veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:
      case 17: {
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

///Example 1: Schedule Generation
/// - - - - - - - - - - - - - - -
// Suppose that given a starting date in the 'YYYYMMDD' format, we want to
// generate a schedule for an event that occurs on the same day of the month
// for 24 months.
//
// First, we use the 'bdlt::DateUtil::convertFromYYYYMMDD' function to convert
// the integer into a 'bdlt::Date':
//..
    const int startingDateYYYYMMDD = 20130430;

    bdlt::Date date;
    int rc = bdlt::DateUtil::convertFromYYYYMMDD(&date, startingDateYYYYMMDD);
    ASSERT(0 == rc);
//..
// Now, we use the 'addMonthsEom' function to generate the schedule.  Note
// that 'addMonthsEom' adjusts the resulting date to be the last day of the
// month if the original date is the last day of the month, while
// 'addMonthsNoEom' does not make this adjustment.
//..
    bsl::vector<bdlt::Date> schedule;
    schedule.push_back(date);

    for (int i = 1; i < 12; ++i) {
        schedule.push_back(bdlt::DateUtil::addMonthsEom(date, i));
    }
//..
// Finally, we print the generated schedule to the console and observe the
// output:
//..
    if (veryVerbose) {
        bsl::copy(schedule.begin(),
                  schedule.end(),
                  bsl::ostream_iterator<bdlt::Date>(bsl::cout, "\n"));
    }

    // Expected output on the console:
    //
    //   30APR2013
    //   31MAY2013
    //   30JUN2013
    //   31JUL2013
    //   31AUG2013
    //   30SEP2013
    //   31OCT2013
    //   30NOV2013
    //   31DEC2013
    //   31JAN2014
    //   28FEB2014
    //   31MAR2014
//..
// Notice that the dates have been adjusted to the end of the month.  If we had
// used 'addMonthsNoEom' instead of 'addMonthsEom', this adjustment would not
// have occurred.
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'addYears'
        //
        // Concerns:
        //: 1 The function correctly delegates its operations either to
        //:   'addYearsEom' if 'eomFlag' is 'true', or 'addYearsNoEom'
        //:   otherwise.
        //:
        // Plan:
        //: 1 Using the brute-force approach, for some inputs for which
        //:   'addYearsEom' and 'addYearsNoEom' return different results,
        //:   call 'addYears' with 'eomFlag' set to 'true' and 'false'.
        //:   Verify that the function returns the correct values.  (C-1)
        //
        // Testing:
        //   Date addYears(original, numYears, eomFlag);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'addYears'" << endl
                          << "==================" << endl;

        {
            const bdlt::Date original(2003, 2, 28);
            const bdlt::Date expEom(  2004, 2, 29);
            const bdlt::Date expNEom( 2004, 2, 28);

            const bdlt::Date resultEom  = Util::addYears(original, 1, true);
            const bdlt::Date resultNEom = Util::addYears(original, 1, false);

            ASSERTV( expEom,  resultEom,  expEom == resultEom);
            ASSERTV(expNEom, resultNEom, expNEom == resultNEom);
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'addYearsNoEom'
        //
        // Concerns:
        //: 1 If the day of the month of 'original' is the last day of the
        //:   month, the resulting date is *not* moved to the end of the month.
        //:
        //: 2 If the day of the month of 'original' does not exist in the
        //:   result month, the resulting date *is* moved to the end of the
        //:   month.
        //:
        //: 3 That the month of the resulting date is the same as the original
        //:   date.
        //:
        //: 4 That the year of the resulting date is the appropriately
        //:   adjusted year.
        //:
        //: 5 Adding a negative number of years works as expected.
        //:
        //: 6 The entire range of valid argument and return values are
        //:   supported.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven approach, define a representative set of
        //:   valid inputs.  Verify that the function returns the correct
        //:   value.  (C-1..6)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-7)
        //
        // Testing:
        //   Date addYearsNoEom(original, numYears);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'addYearsNoEom'" << endl
                          << "=======================" << endl;

        static const struct {
            int        d_line;      // source line number
            bdlt::Date d_original;  // original date
            int        d_numYears;  // number of months to add
            bdlt::Date d_exp;       // expected result
        } DATA[] = {
            //LINE ORIGINAL                     N   EXP
            //---- --------                     -   ---
            // Add 1 year to every month
            { L_,  bdlt::Date(2014,  1, 31),    1,  bdlt::Date(2015,  1, 31) },
            { L_,  bdlt::Date(2014,  2, 28),    1,  bdlt::Date(2015,  2, 28) },
            { L_,  bdlt::Date(2014,  3, 31),    1,  bdlt::Date(2015,  3, 31) },
            { L_,  bdlt::Date(2014,  4, 30),    1,  bdlt::Date(2015,  4, 30) },
            { L_,  bdlt::Date(2014,  5, 31),    1,  bdlt::Date(2015,  5, 31) },
            { L_,  bdlt::Date(2014,  6, 30),    1,  bdlt::Date(2015,  6, 30) },
            { L_,  bdlt::Date(2014,  7, 31),    1,  bdlt::Date(2015,  7, 31) },
            { L_,  bdlt::Date(2014,  8, 31),    1,  bdlt::Date(2015,  8, 31) },
            { L_,  bdlt::Date(2014,  9, 30),    1,  bdlt::Date(2015,  9, 30) },
            { L_,  bdlt::Date(2014, 10, 31),    1,  bdlt::Date(2015, 10, 31) },
            { L_,  bdlt::Date(2014, 11, 30),    1,  bdlt::Date(2015, 11, 30) },
            { L_,  bdlt::Date(2014, 12, 31),    1,  bdlt::Date(2015, 12, 31) },

            // Not End of Month (Feb 28)
            { L_,  bdlt::Date(2000,  2, 28),    0,  bdlt::Date(2000,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 28),    1,  bdlt::Date(2001,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 28),    2,  bdlt::Date(2002,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 28),    3,  bdlt::Date(2003,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 28),    4,  bdlt::Date(2004,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 28),    5,  bdlt::Date(2005,  2, 28) },

            // End of Month (Feb 28)
            { L_,  bdlt::Date(2001,  2, 28),    0,  bdlt::Date(2001,  2, 28) },
            { L_,  bdlt::Date(2001,  2, 28),    1,  bdlt::Date(2002,  2, 28) },
            { L_,  bdlt::Date(2001,  2, 28),    2,  bdlt::Date(2003,  2, 28) },
            { L_,  bdlt::Date(2001,  2, 28),    3,  bdlt::Date(2004,  2, 28) },
            { L_,  bdlt::Date(2001,  2, 28),    4,  bdlt::Date(2005,  2, 28) },
            { L_,  bdlt::Date(2001,  2, 28),    5,  bdlt::Date(2006,  2, 28) },

            // End of Month (Feb 29)
            { L_,  bdlt::Date(2000,  2, 29),    0,  bdlt::Date(2000,  2, 29) },
            { L_,  bdlt::Date(2000,  2, 29),    1,  bdlt::Date(2001,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 29),    2,  bdlt::Date(2002,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 29),    3,  bdlt::Date(2003,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 29),    4,  bdlt::Date(2004,  2, 29) },
            { L_,  bdlt::Date(2000,  2, 29),    5,  bdlt::Date(2005,  2, 28) },

            // Add differing values of years
            { L_,  bdlt::Date(2014,  1, 31),  100,  bdlt::Date(2114,  1, 31) },
            { L_,  bdlt::Date(2014,  2, 28),  200,  bdlt::Date(2214,  2, 28) },
            { L_,  bdlt::Date(2014,  3, 31), 1000,  bdlt::Date(3014,  3, 31) },
            { L_,  bdlt::Date(2014,  4, 30), -100,  bdlt::Date(1914,  4, 30) },
            { L_,  bdlt::Date(2014,  5, 31), -200,  bdlt::Date(1814,  5, 31) },
            { L_,  bdlt::Date(2014,  6, 30), -999,  bdlt::Date(1015,  6, 30) },

            // Boundary Conditions
            { L_,  bdlt::Date(   2,  1,  1),   -1,  bdlt::Date(   1,  1,  1) },
            { L_,  bdlt::Date(9998, 12, 31),    1,  bdlt::Date(9999, 12, 31) },
            { L_,  bdlt::Date(   1, 12, 31), 9998,  bdlt::Date(9999, 12, 31) },
            { L_,  bdlt::Date(9999,  1,  1),-9998,  bdlt::Date(   1,  1,  1) },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int        LINE      = DATA[i].d_line;
            const bdlt::Date ORIGINAL  = DATA[i].d_original;
            const int        NUM_YEARS = DATA[i].d_numYears;
            const bdlt::Date EXP       = DATA[i].d_exp;

            if (veryVerbose) {
                T_ P_(LINE) P_(ORIGINAL) P_(NUM_YEARS) P(EXP);
            }

            const bdlt::Date result = Util::addYearsNoEom(ORIGINAL, NUM_YEARS);
            ASSERTV(LINE, EXP, result, EXP == result);
        }
        {
            // Test assertions
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_PASS(
                            Util::addYearsNoEom(bdlt::Date(  2,   1,  1),  2));
            ASSERT_SAFE_FAIL(
                            Util::addYearsNoEom(bdlt::Date(  2,   1,  1), -2));

            ASSERT_SAFE_PASS(
                            Util::addYearsNoEom(bdlt::Date(9998, 12, 31),  1));
            ASSERT_SAFE_FAIL(
                            Util::addYearsNoEom(bdlt::Date(9998, 12, 31),  2));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'addYearsEom'
        //
        // Concerns:
        //: 1 If the day of the month of 'original' is the last day of the
        //:   month, the resulting date is moved to the end of the month.
        //:
        //: 2 If the day of the month of 'original' does not exist in the
        //:   result month, the resulting date is moved to the end of the
        //:   month.
        //:
        //: 3 That the month of the resulting date is the same as the original
        //:   date.
        //:
        //: 4 That the year of the resulting date is the appropriately
        //:   adjusted year.
        //:
        //: 5 Adding a negative number of years works as expected.
        //:
        //: 6 The entire range of valid argument and return values are
        //:   supported.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven approach, define a representative set of
        //:   valid inputs.  Verify that the function returns the correct
        //:   value.  (C-1..6)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-7)
        //
        // Testing:
        //   Date addYearsEom(original, numYears);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'addYearsEom'" << endl
                          << "=====================" << endl;

        static const struct {
            int        d_line;      // source line number
            bdlt::Date d_original;  // original date
            int        d_numYears;  // number of months to add
            bdlt::Date d_exp;       // expected result
        } DATA[] = {
            //LINE ORIGINAL                     N   EXP
            //---- --------                     -   ---
            // Add 1 year to every month
            { L_,  bdlt::Date(2014,  1, 31),    1,  bdlt::Date(2015,  1, 31) },
            { L_,  bdlt::Date(2014,  2, 28),    1,  bdlt::Date(2015,  2, 28) },
            { L_,  bdlt::Date(2014,  3, 31),    1,  bdlt::Date(2015,  3, 31) },
            { L_,  bdlt::Date(2014,  4, 30),    1,  bdlt::Date(2015,  4, 30) },
            { L_,  bdlt::Date(2014,  5, 31),    1,  bdlt::Date(2015,  5, 31) },
            { L_,  bdlt::Date(2014,  6, 30),    1,  bdlt::Date(2015,  6, 30) },
            { L_,  bdlt::Date(2014,  7, 31),    1,  bdlt::Date(2015,  7, 31) },
            { L_,  bdlt::Date(2014,  8, 31),    1,  bdlt::Date(2015,  8, 31) },
            { L_,  bdlt::Date(2014,  9, 30),    1,  bdlt::Date(2015,  9, 30) },
            { L_,  bdlt::Date(2014, 10, 31),    1,  bdlt::Date(2015, 10, 31) },
            { L_,  bdlt::Date(2014, 11, 30),    1,  bdlt::Date(2015, 11, 30) },
            { L_,  bdlt::Date(2014, 12, 31),    1,  bdlt::Date(2015, 12, 31) },

            // Not End of Month (Feb 28)
            { L_,  bdlt::Date(2000,  2, 28),    0,  bdlt::Date(2000,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 28),    1,  bdlt::Date(2001,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 28),    2,  bdlt::Date(2002,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 28),    3,  bdlt::Date(2003,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 28),    4,  bdlt::Date(2004,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 28),    5,  bdlt::Date(2005,  2, 28) },

            // End of Month (Feb 28)
            { L_,  bdlt::Date(2001,  2, 28),    0,  bdlt::Date(2001,  2, 28) },
            { L_,  bdlt::Date(2001,  2, 28),    1,  bdlt::Date(2002,  2, 28) },
            { L_,  bdlt::Date(2001,  2, 28),    2,  bdlt::Date(2003,  2, 28) },
            { L_,  bdlt::Date(2001,  2, 28),    3,  bdlt::Date(2004,  2, 29) },
            { L_,  bdlt::Date(2001,  2, 28),    4,  bdlt::Date(2005,  2, 28) },
            { L_,  bdlt::Date(2001,  2, 28),    5,  bdlt::Date(2006,  2, 28) },

            // End of Month (Feb 29)
            { L_,  bdlt::Date(2000,  2, 29),    0,  bdlt::Date(2000,  2, 29) },
            { L_,  bdlt::Date(2000,  2, 29),    1,  bdlt::Date(2001,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 29),    2,  bdlt::Date(2002,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 29),    3,  bdlt::Date(2003,  2, 28) },
            { L_,  bdlt::Date(2000,  2, 29),    4,  bdlt::Date(2004,  2, 29) },
            { L_,  bdlt::Date(2000,  2, 29),    5,  bdlt::Date(2005,  2, 28) },

            // Add differing values of years
            { L_,  bdlt::Date(2014,  1, 31),  100,  bdlt::Date(2114,  1, 31) },
            { L_,  bdlt::Date(2014,  2, 28),  200,  bdlt::Date(2214,  2, 28) },
            { L_,  bdlt::Date(2014,  3, 31), 1000,  bdlt::Date(3014,  3, 31) },
            { L_,  bdlt::Date(2014,  4, 30), -100,  bdlt::Date(1914,  4, 30) },
            { L_,  bdlt::Date(2014,  5, 31), -200,  bdlt::Date(1814,  5, 31) },
            { L_,  bdlt::Date(2014,  6, 30), -999,  bdlt::Date(1015,  6, 30) },

            // Boundary Conditions
            { L_,  bdlt::Date(   2,  1,  1),   -1,  bdlt::Date(   1,  1,  1) },
            { L_,  bdlt::Date(9998, 12, 31),    1,  bdlt::Date(9999, 12, 31) },
            { L_,  bdlt::Date(   1, 12, 31), 9998,  bdlt::Date(9999, 12, 31) },
            { L_,  bdlt::Date(9999,  1,  1),-9998,  bdlt::Date(   1,  1,  1) },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int        LINE      = DATA[i].d_line;
            const bdlt::Date ORIGINAL  = DATA[i].d_original;
            const int        NUM_YEARS = DATA[i].d_numYears;
            const bdlt::Date EXP       = DATA[i].d_exp;

            if (veryVerbose) {
                T_ P_(LINE) P_(ORIGINAL) P_(NUM_YEARS) P(EXP);
            }

            const bdlt::Date result = Util::addYearsEom(ORIGINAL, NUM_YEARS);
            ASSERTV(LINE, EXP, result, EXP == result);
        }
        {
            // Test assertions
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_PASS(Util::addYearsEom(bdlt::Date(  2,   1,  1),  2));
            ASSERT_SAFE_FAIL(Util::addYearsEom(bdlt::Date(  2,   1,  1), -2));

            ASSERT_SAFE_PASS(Util::addYearsEom(bdlt::Date(9998, 12, 31),  1));
            ASSERT_SAFE_FAIL(Util::addYearsEom(bdlt::Date(9998, 12, 31),  2));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'addMonths'
        //
        // Concerns:
        //: 1 The function correctly delegates its operations either to
        //:   'addMonthsEom' if 'eomFlag' is 'true', or 'addMonthsNoEom'
        //:   otherwise.
        //:
        // Plan:
        //: 1 Using the brute-force approach, for some inputs for which
        //:   'addMonthsEom' and 'addMonthsNoEom' return different results,
        //:   call 'addMonths' with 'eomFlag' set to 'true' and 'false'.
        //:   Verify that the function returns the correct values.  (C-1)
        //
        // Testing:
        //   Date addMonths(original, numMonths, eomFlag);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'addMonths'" << endl
                          << "===================" << endl;

        {
            const bdlt::Date original(2000, 2, 29);

            const int numMonths = 2;

            const bdlt::Date expEom(2000, 4, 30);
            const bdlt::Date expNEom(2000, 4, 29);

            const bdlt::Date resultEom =
                                     Util::addMonths(original,numMonths, true);
            const bdlt::Date resultNEom =
                                   Util::addMonths(original, numMonths, false);

            ASSERTV( expEom,  resultEom,  expEom == resultEom);
            ASSERTV(expNEom, resultNEom, expNEom == resultNEom);
        }

        {
            const bdlt::Date original(2001, 4, 30);
            const int numMonths = 1;

            const bdlt::Date expEom(2001, 5, 31);
            const bdlt::Date expNEom(2001, 5, 30);

            const bdlt::Date resultEom =
                                    Util::addMonths(original, numMonths, true);
            const bdlt::Date resultNEom =
                                   Util::addMonths(original, numMonths, false);

            ASSERTV( expEom,  resultEom,   expEom == resultEom);
            ASSERTV(expNEom,  resultNEom, expNEom == resultNEom);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'addMonthsEom'
        //
        // Concerns:
        //: 1 If the day of the month of 'original' is the last day of the
        //:   month, the resulting date is moved to the end of the month.
        //:
        //: 2 If the day of the month of 'original' does not exist in the
        //:   result month, the resulting date is moved to the end of the
        //:   month.
        //:
        //: 3 If adding 'numMonths' to 'original' cannot be represented in the
        //:   year of 'original', the resulting date correctly rolls over to
        //:   the subsequent year.
        //:
        //: 4 Adding a negative number of months works as expected.
        //:
        //: 5 The entire range of valid argument and return values are
        //:   supported.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven approach, define a representative set of
        //:   valid inputs.  Verify that the function returns the correct
        //:   value.  (C-1..5)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-6)
        //
        // Testing:
        //   Date addMonthsEom(original, numMonths);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'addMonthsEom'" << endl
                          << "======================" << endl;

        static const struct {
            int        d_line;       // source line number
            bdlt::Date d_original;   // original date
            int        d_numMonths;  // number of months to add
            bdlt::Date d_exp;        // expected result
        } DATA[] = {
         //LINE ORIGINAL                        N   EXP
         //---- --------                        -   ---
         // NEOM - 28
         { L_,  bdlt::Date(2000,  1, 28),       0,  bdlt::Date(2000,  1, 28) },
         { L_,  bdlt::Date(2000,  1, 28),      13,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2000,  1, 28),       1,  bdlt::Date(2000,  2, 28) },
         { L_,  bdlt::Date(2000,  1, 28),       3,  bdlt::Date(2000,  4, 28) },
         { L_,  bdlt::Date(2000,  1, 28),       2,  bdlt::Date(2000,  3, 28) },
         { L_,  bdlt::Date(2000,  1, 28),      12,  bdlt::Date(2001,  1, 28) },
         { L_,  bdlt::Date(2000,  1, 28),      50,  bdlt::Date(2004,  3, 28) },

         // NEOM - 29
         { L_,  bdlt::Date(2000,  1, 29),       0,  bdlt::Date(2000,  1, 29) },
         { L_,  bdlt::Date(2000,  1, 29),      13,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2000,  1, 29),       1,  bdlt::Date(2000,  2, 29) },
         { L_,  bdlt::Date(2000,  1, 29),       3,  bdlt::Date(2000,  4, 29) },
         { L_,  bdlt::Date(2000,  1, 29),       2,  bdlt::Date(2000,  3, 29) },
         { L_,  bdlt::Date(2000,  1, 29),      12,  bdlt::Date(2001,  1, 29) },
         { L_,  bdlt::Date(2000,  1, 29),      50,  bdlt::Date(2004,  3, 29) },

         // NEOM - 30
         { L_,  bdlt::Date(2000,  1, 30),       0,  bdlt::Date(2000,  1, 30) },
         { L_,  bdlt::Date(2000,  1, 30),      13,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2000,  1, 30),       1,  bdlt::Date(2000,  2, 29) },
         { L_,  bdlt::Date(2000,  1, 30),       3,  bdlt::Date(2000,  4, 30) },
         { L_,  bdlt::Date(2000,  1, 30),       2,  bdlt::Date(2000,  3, 30) },
         { L_,  bdlt::Date(2000,  1, 30),      12,  bdlt::Date(2001,  1, 30) },
         { L_,  bdlt::Date(2000,  1, 30),      50,  bdlt::Date(2004,  3, 30) },

         // EOM - 28
         { L_,  bdlt::Date(2001,  2, 28),       0,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2001,  2, 28),      12,  bdlt::Date(2002,  2, 28) },
         { L_,  bdlt::Date(2001,  2, 28),      36,  bdlt::Date(2004,  2, 29) },
         { L_,  bdlt::Date(2001,  2, 28),       2,  bdlt::Date(2001,  4, 30) },
         { L_,  bdlt::Date(2001,  2, 28),       1,  bdlt::Date(2001,  3, 31) },
         { L_,  bdlt::Date(2001,  2, 28),      24,  bdlt::Date(2003,  2, 28) },
         { L_,  bdlt::Date(2001,  2, 28),      50,  bdlt::Date(2005,  4, 30) },

         // EOM - 29
         { L_,  bdlt::Date(2000,  2, 29),       0,  bdlt::Date(2000,  2, 29) },
         { L_,  bdlt::Date(2000,  2, 29),      12,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2000,  2, 29),      48,  bdlt::Date(2004,  2, 29) },
         { L_,  bdlt::Date(2000,  2, 29),       2,  bdlt::Date(2000,  4, 30) },
         { L_,  bdlt::Date(2000,  2, 29),       1,  bdlt::Date(2000,  3, 31) },
         { L_,  bdlt::Date(2000,  2, 29),      24,  bdlt::Date(2002,  2, 28) },
         { L_,  bdlt::Date(2000,  2, 29),      50,  bdlt::Date(2004,  4, 30) },

         // EOM - 30
         { L_,  bdlt::Date(2000,  4, 30),       0,  bdlt::Date(2000,  4, 30) },
         { L_,  bdlt::Date(2000,  4, 30),      10,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2000,  4, 30),      46,  bdlt::Date(2004,  2, 29) },
         { L_,  bdlt::Date(2000,  4, 30),       2,  bdlt::Date(2000,  6, 30) },
         { L_,  bdlt::Date(2000,  4, 30),       1,  bdlt::Date(2000,  5, 31) },
         { L_,  bdlt::Date(2000,  4, 30),      12,  bdlt::Date(2001,  4, 30) },
         { L_,  bdlt::Date(2000,  4, 30),      50,  bdlt::Date(2004,  6, 30) },

         // EOM - 31
         { L_,  bdlt::Date(2000,  1, 31),       0,  bdlt::Date(2000,  1, 31) },
         { L_,  bdlt::Date(2000,  1, 31),      13,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2000,  1, 31),       1,  bdlt::Date(2000,  2, 29) },
         { L_,  bdlt::Date(2000,  1, 31),       3,  bdlt::Date(2000,  4, 30) },
         { L_,  bdlt::Date(2000,  1, 31),       2,  bdlt::Date(2000,  3, 31) },
         { L_,  bdlt::Date(2000,  1, 31),      12,  bdlt::Date(2001,  1, 31) },
         { L_,  bdlt::Date(2000,  1, 31),      50,  bdlt::Date(2004,  3, 31) },

         // NORMAL
         { L_,  bdlt::Date(2000,  3,  1),       0,  bdlt::Date(2000,  3,  1) },
         { L_,  bdlt::Date(2000,  3,  1),       1,  bdlt::Date(2000,  4,  1) },
         { L_,  bdlt::Date(2000,  3, 15),       1,  bdlt::Date(2000,  4, 15) },
         { L_,  bdlt::Date(2000,  3, 15),      15,  bdlt::Date(2001,  6, 15) },
         { L_,  bdlt::Date(2000,  3, 15),     200,  bdlt::Date(2016, 11, 15) },

         // ADD NEGATIVE MONTHS
         { L_,  bdlt::Date(2000,  3,  1),      -1,  bdlt::Date(2000,  2,  1) },
         { L_,  bdlt::Date(2000,  3, 15),      -1,  bdlt::Date(2000,  2, 15) },
         { L_,  bdlt::Date(2000,  3, 15),      -2,  bdlt::Date(2000,  1, 15) },
         { L_,  bdlt::Date(2000,  3, 15),      -3,  bdlt::Date(1999, 12, 15) },
         { L_,  bdlt::Date(2000,  3, 15),     -15,  bdlt::Date(1998, 12, 15) },
         { L_,  bdlt::Date(2000,  3, 15),    -200,  bdlt::Date(1983,  7, 15) },

         // BOUNDARY CONDITIONS
         { L_,  bdlt::Date(   2,  3,  1),     -14,  bdlt::Date(   1,  1,  1) },
         { L_,  bdlt::Date(9998,  3, 31),      21,  bdlt::Date(9999, 12, 31) },
         { L_,  bdlt::Date(   1,  1, 31),  119987,  bdlt::Date(9999, 12, 31) },
         { L_,  bdlt::Date(9999, 12,  1), -119987,  bdlt::Date(   1,  1,  1) },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int        LINE       = DATA[i].d_line;
            const bdlt::Date ORIGINAL   = DATA[i].d_original;
            const int        NUM_MONTHS = DATA[i].d_numMonths;
            const bdlt::Date EXP        = DATA[i].d_exp;

            if (veryVerbose) {
                T_ P_(LINE) P_(ORIGINAL) P_(NUM_MONTHS) P(EXP);
            }

            const bdlt::Date result = Util::addMonthsEom(ORIGINAL, NUM_MONTHS);
            ASSERTV(LINE, EXP, result, EXP == result);
        }

        {
            // Test assertions
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_PASS(Util::addMonthsEom(bdlt::Date(  1,   2,  1), -1));
            ASSERT_SAFE_FAIL(Util::addMonthsEom(bdlt::Date(  1,   2,  1), -2));

            ASSERT_SAFE_PASS(Util::addMonthsEom(bdlt::Date(9999, 10, 31),  2));
            ASSERT_SAFE_FAIL(Util::addMonthsEom(bdlt::Date(9999, 10, 31),  3));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'addMonthsNoEom'
        //
        // Concerns:
        //: 1 If the day of the month of 'original' is the last day of the
        //:   month, the resulting date is *not* moved to the end of the month.
        //:
        //: 2 If the day of the month of 'original' does not exist in the
        //:   result month, the resulting date *is* moved to the end of the
        //:   month.
        //:
        //: 3 If adding 'numMonths' to 'original' cannot be represented in the
        //:   year of 'original', the resulting date correctly rolls over to
        //:   the subsequent year.
        //:
        //: 4 Adding a negative number of months works as expected.
        //:
        //: 5 The entire range of valid argument and return values are
        //:   supported.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven approach, define a representative set of
        //:   valid inputs.  Verify that the function returns the correct
        //:   value.  (C-1..5)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-6)
        //
        // Testing:
        //   Date addMonthsNoEom(original, numMonths);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'addMonthsNoEom'" << endl
                          << "========================" << endl;


        static const struct {
            int        d_line;       // source line number
            bdlt::Date d_original;   // original date
            int        d_numMonths;  // number of months to add
            bdlt::Date d_exp;        // expected result
        } DATA[] = {
         //LINE ORIGINAL                        N   EXP
         //---- --------                        -   ---
         // NEOM - 28
         { L_,  bdlt::Date(2000,  1, 28),       0,  bdlt::Date(2000,  1, 28) },
         { L_,  bdlt::Date(2000,  1, 28),      13,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2000,  1, 28),       1,  bdlt::Date(2000,  2, 28) },
         { L_,  bdlt::Date(2000,  1, 28),       3,  bdlt::Date(2000,  4, 28) },
         { L_,  bdlt::Date(2000,  1, 28),       2,  bdlt::Date(2000,  3, 28) },
         { L_,  bdlt::Date(2000,  1, 28),      12,  bdlt::Date(2001,  1, 28) },
         { L_,  bdlt::Date(2000,  1, 28),      50,  bdlt::Date(2004,  3, 28) },

         // NEOM - 29
         { L_,  bdlt::Date(2000,  1, 29),       0,  bdlt::Date(2000,  1, 29) },
         { L_,  bdlt::Date(2000,  1, 29),      13,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2000,  1, 29),       1,  bdlt::Date(2000,  2, 29) },
         { L_,  bdlt::Date(2000,  1, 29),       3,  bdlt::Date(2000,  4, 29) },
         { L_,  bdlt::Date(2000,  1, 29),       2,  bdlt::Date(2000,  3, 29) },
         { L_,  bdlt::Date(2000,  1, 29),      12,  bdlt::Date(2001,  1, 29) },
         { L_,  bdlt::Date(2000,  1, 29),      50,  bdlt::Date(2004,  3, 29) },

         // NEOM - 30
         { L_,  bdlt::Date(2000,  1, 30),       0,  bdlt::Date(2000,  1, 30) },
         { L_,  bdlt::Date(2000,  1, 30),      13,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2000,  1, 30),       1,  bdlt::Date(2000,  2, 29) },
         { L_,  bdlt::Date(2000,  1, 30),       3,  bdlt::Date(2000,  4, 30) },
         { L_,  bdlt::Date(2000,  1, 30),       2,  bdlt::Date(2000,  3, 30) },
         { L_,  bdlt::Date(2000,  1, 30),      12,  bdlt::Date(2001,  1, 30) },
         { L_,  bdlt::Date(2000,  1, 30),      50,  bdlt::Date(2004,  3, 30) },

         // EOM - 28
         { L_,  bdlt::Date(2001,  2, 28),       0,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2001,  2, 28),      12,  bdlt::Date(2002,  2, 28) },
         { L_,  bdlt::Date(2001,  2, 28),      36,  bdlt::Date(2004,  2, 28) },
         { L_,  bdlt::Date(2001,  2, 28),       2,  bdlt::Date(2001,  4, 28) },
         { L_,  bdlt::Date(2001,  2, 28),       1,  bdlt::Date(2001,  3, 28) },
         { L_,  bdlt::Date(2001,  2, 28),      24,  bdlt::Date(2003,  2, 28) },
         { L_,  bdlt::Date(2001,  2, 28),      50,  bdlt::Date(2005,  4, 28) },

         // EOM - 29
         { L_,  bdlt::Date(2000,  2, 29),       0,  bdlt::Date(2000,  2, 29) },
         { L_,  bdlt::Date(2000,  2, 29),      12,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2000,  2, 29),      48,  bdlt::Date(2004,  2, 29) },
         { L_,  bdlt::Date(2000,  2, 29),       2,  bdlt::Date(2000,  4, 29) },
         { L_,  bdlt::Date(2000,  2, 29),       1,  bdlt::Date(2000,  3, 29) },
         { L_,  bdlt::Date(2000,  2, 29),      24,  bdlt::Date(2002,  2, 28) },
         { L_,  bdlt::Date(2000,  2, 29),      50,  bdlt::Date(2004,  4, 29) },

         // EOM - 30
         { L_,  bdlt::Date(2000,  4, 30),       0,  bdlt::Date(2000,  4, 30) },
         { L_,  bdlt::Date(2000,  4, 30),      10,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2000,  4, 30),      46,  bdlt::Date(2004,  2, 29) },
         { L_,  bdlt::Date(2000,  4, 30),       2,  bdlt::Date(2000,  6, 30) },
         { L_,  bdlt::Date(2000,  4, 30),       1,  bdlt::Date(2000,  5, 30) },
         { L_,  bdlt::Date(2000,  4, 30),      12,  bdlt::Date(2001,  4, 30) },
         { L_,  bdlt::Date(2000,  4, 30),      50,  bdlt::Date(2004,  6, 30) },

         // EOM - 31
         { L_,  bdlt::Date(2000,  1, 31),       0,  bdlt::Date(2000,  1, 31) },
         { L_,  bdlt::Date(2000,  1, 31),      13,  bdlt::Date(2001,  2, 28) },
         { L_,  bdlt::Date(2000,  1, 31),       1,  bdlt::Date(2000,  2, 29) },
         { L_,  bdlt::Date(2000,  1, 31),       3,  bdlt::Date(2000,  4, 30) },
         { L_,  bdlt::Date(2000,  1, 31),       2,  bdlt::Date(2000,  3, 31) },
         { L_,  bdlt::Date(2000,  1, 31),      12,  bdlt::Date(2001,  1, 31) },
         { L_,  bdlt::Date(2000,  1, 31),      50,  bdlt::Date(2004,  3, 31) },

         // NORMAL
         { L_,  bdlt::Date(2000,  3,  1),       0,  bdlt::Date(2000,  3,  1) },
         { L_,  bdlt::Date(2000,  3,  1),       1,  bdlt::Date(2000,  4,  1) },
         { L_,  bdlt::Date(2000,  3, 15),       1,  bdlt::Date(2000,  4, 15) },
         { L_,  bdlt::Date(2000,  3, 15),      15,  bdlt::Date(2001,  6, 15) },
         { L_,  bdlt::Date(2000,  3, 15),     200,  bdlt::Date(2016, 11, 15) },

         // ADD NEGATIVE MONTHS
         { L_,  bdlt::Date(2000,  3,  1),      -1,  bdlt::Date(2000,  2,  1) },
         { L_,  bdlt::Date(2000,  3, 15),      -1,  bdlt::Date(2000,  2, 15) },
         { L_,  bdlt::Date(2000,  3, 15),      -2,  bdlt::Date(2000,  1, 15) },
         { L_,  bdlt::Date(2000,  3, 15),      -3,  bdlt::Date(1999, 12, 15) },
         { L_,  bdlt::Date(2000,  3, 15),     -15,  bdlt::Date(1998, 12, 15) },
         { L_,  bdlt::Date(2000,  3, 15),    -200,  bdlt::Date(1983,  7, 15) },

         // BOUNDARY CONDITIONS
         { L_,  bdlt::Date(   2,  3,  1),     -14,  bdlt::Date(   1,  1,  1) },
         { L_,  bdlt::Date(9998,  3, 31),      21,  bdlt::Date(9999, 12, 31) },
         { L_,  bdlt::Date(   1,  1, 31),  119987,  bdlt::Date(9999, 12, 31) },
         { L_,  bdlt::Date(9999, 12,  1), -119987,  bdlt::Date(   1,  1,  1) },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int        LINE       = DATA[i].d_line;
            const bdlt::Date ORIGINAL   = DATA[i].d_original;
            const int        NUM_MONTHS = DATA[i].d_numMonths;
            const bdlt::Date EXP        = DATA[i].d_exp;

            if (veryVerbose) {
                T_ P_(LINE) P_(ORIGINAL) P_(NUM_MONTHS) P(EXP);
            }

            const bdlt::Date result = Util::addMonthsNoEom(ORIGINAL,
                                                           NUM_MONTHS);
            ASSERTV(LINE, EXP, result, EXP == result);
        }

        {
            // Test assertions
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_PASS(
                           Util::addMonthsNoEom(bdlt::Date(  1,   2,  1), -1));
            ASSERT_SAFE_FAIL(
                           Util::addMonthsNoEom(bdlt::Date(  1,   2,  1), -2));

            ASSERT_SAFE_PASS(
                           Util::addMonthsNoEom(bdlt::Date(9999, 10, 31),  2));
            ASSERT_SAFE_FAIL(
                           Util::addMonthsNoEom(bdlt::Date(9999, 10, 31),  3));
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'lastDayOfWeekInMonth'
        //
        // Concerns:
        //: 1 The function returns the last 'dayOfWeek' in 'month' and 'year'
        //:   for all possible 'dayOfWeek' values.
        //:
        //: 2 The function works correctly if the last day of 'month' in 'year'
        //:   falls on 'dayOfWeek'.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven approach, define a representative set of
        //:   valid inputs.  Verify that the function returns the correct
        //:   value.  Note that we do not need to do a very exhaustive test,
        //:   because most of the work is handled by the already tested
        //:   function 'previousDayOfWeekInclusive'.  (C-1..2)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-3)
        //
        // Testing:
        //   Date lastDayOfWeekInMonth(year, month, dayOfWeek);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'lastDayOfWeekInMonth'" << endl
                          << "==============================" << endl;

        static const struct {
            int                   d_line;   // source line number
            int                   d_year;   // year
            int                   d_month;  // month
            bdlt::DayOfWeek::Enum d_dow;    // day of week
            bdlt::Date            d_exp;    // expected result
        } DATA[] = {
            //LINE  YEAR  MONTH  DOW     EXP
            //----  ----  -----  ---     ---
            { L_,  2000,     1,  e_SUN,  bdlt::Date(2000, 1, 30) },
            { L_,  2000,     1,  e_MON,  bdlt::Date(2000, 1, 31) },
            { L_,  2000,     1,  e_TUE,  bdlt::Date(2000, 1, 25) },
            { L_,  2000,     1,  e_WED,  bdlt::Date(2000, 1, 26) },
            { L_,  2000,     1,  e_THU,  bdlt::Date(2000, 1, 27) },
            { L_,  2000,     1,  e_FRI,  bdlt::Date(2000, 1, 28) },
            { L_,  2000,     1,  e_SAT,  bdlt::Date(2000, 1, 29) }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                   LINE  = DATA[i].d_line;
            const int                   YEAR  = DATA[i].d_year;
            const int                   MONTH = DATA[i].d_month;
            const bdlt::DayOfWeek::Enum DOW   = DATA[i].d_dow;
            const bdlt::Date            EXP   = DATA[i].d_exp;

            if (veryVerbose) {
                T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DOW) P(EXP);
            }

            const bdlt::Date result = Util::lastDayOfWeekInMonth(YEAR,
                                                                 MONTH,
                                                                 DOW);
            ASSERTV(LINE, EXP, result, EXP == result);
        }

        {
            // Test assertions
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_PASS(Util::lastDayOfWeekInMonth(    1,  1, e_SUN));
            ASSERT_SAFE_PASS(Util::lastDayOfWeekInMonth( 9999,  1, e_SUN));
            ASSERT_SAFE_FAIL(Util::lastDayOfWeekInMonth(    0,  1, e_SUN));
            ASSERT_SAFE_FAIL(Util::lastDayOfWeekInMonth(10000,  1, e_SUN));

            ASSERT_SAFE_PASS(Util::lastDayOfWeekInMonth(    1, 12, e_SUN));
            ASSERT_SAFE_FAIL(Util::lastDayOfWeekInMonth(    1,  0, e_SUN));
            ASSERT_SAFE_FAIL(Util::lastDayOfWeekInMonth(    1, 13, e_SUN));
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'nthDayOfWeekInMonth'
        //   This function first uses 'nextDayOfWeekInclusive' to find the
        //   first occurrence of 'dayOfWeek' in 'month', and then adds the
        //   required offset to get the 'n'th occurrence.  We need to verify
        //   that 'nextDayOfWeekInclusive' is correctly called and that the
        //   correct offset is added.
        //
        // Concerns:
        //: 1 The function correctly returns the 1st - 4th occurrence of the
        //:   'dayOfWeek' in 'month' and 'year'.
        //:
        //: 2 If a 5th occurrence of the 'dayOfWeek' doesn't exist within
        //:   'month' and 'year', the first 'dayOfWeek' of the next month is
        //:   returned.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven approach, define a representative set of
        //:   valid inputs, including 'n' from 1 to 5.  Note that we do not
        //:   need to do a very exhaustive test, because most of the work is
        //:   handled by the already tested function 'nextDayOfWeekInclusive'.
        //:   (C-1..2)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-3)
        //
        // Testing:
        //   Date nthDayOfWeekInMonth(year, month, dayOfWeek, n);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'nthDayOfWeekInMonth'" << endl
                          << "=============================" << endl;

        static const struct {
            int                   d_line;   // source line number
            int                   d_year;   // year
            int                   d_month;  // month
            bdlt::DayOfWeek::Enum d_dow;    // day of week
            int                   d_n;      // n'th occurrence
            bdlt::Date            d_exp;    // expected result
        } DATA[] = {
            //LINE   YEAR  MONTH  DOW      N  EXP
            //----   ----  -----  ---      -  ---
            { L_,    2000,     1,  e_SUN,  1,  bdlt::Date(2000, 1,  2) },
            { L_,    2000,     1,  e_SUN,  2,  bdlt::Date(2000, 1,  9) },
            { L_,    2000,     1,  e_SUN,  3,  bdlt::Date(2000, 1, 16) },
            { L_,    2000,     1,  e_SUN,  4,  bdlt::Date(2000, 1, 23) },
            { L_,    2000,     1,  e_SUN,  5,  bdlt::Date(2000, 1, 30) },

            { L_,    2000,     1,  e_MON,  1,  bdlt::Date(2000, 1,  3) },
            { L_,    2000,     1,  e_MON,  2,  bdlt::Date(2000, 1, 10) },
            { L_,    2000,     1,  e_MON,  3,  bdlt::Date(2000, 1, 17) },
            { L_,    2000,     1,  e_MON,  4,  bdlt::Date(2000, 1, 24) },
            { L_,    2000,     1,  e_MON,  5,  bdlt::Date(2000, 1, 31) },

            { L_,    2000,     1,  e_TUE,  1,  bdlt::Date(2000, 1,  4) },
            { L_,    2000,     1,  e_TUE,  2,  bdlt::Date(2000, 1, 11) },
            { L_,    2000,     1,  e_TUE,  3,  bdlt::Date(2000, 1, 18) },
            { L_,    2000,     1,  e_TUE,  4,  bdlt::Date(2000, 1, 25) },
            { L_,    2000,     1,  e_TUE,  5,  bdlt::Date(2000, 2,  1) },

            { L_,    2000,     1,  e_WED,  1,  bdlt::Date(2000, 1,  5) },
            { L_,    2000,     1,  e_WED,  2,  bdlt::Date(2000, 1, 12) },
            { L_,    2000,     1,  e_WED,  3,  bdlt::Date(2000, 1, 19) },
            { L_,    2000,     1,  e_WED,  4,  bdlt::Date(2000, 1, 26) },
            { L_,    2000,     1,  e_WED,  5,  bdlt::Date(2000, 2,  2) },

            { L_,    2000,     1,  e_THU,  1,  bdlt::Date(2000, 1,  6) },
            { L_,    2000,     1,  e_THU,  2,  bdlt::Date(2000, 1, 13) },
            { L_,    2000,     1,  e_THU,  3,  bdlt::Date(2000, 1, 20) },
            { L_,    2000,     1,  e_THU,  4,  bdlt::Date(2000, 1, 27) },
            { L_,    2000,     1,  e_THU,  5,  bdlt::Date(2000, 2,  3) },

            { L_,    2000,     1,  e_FRI,  1,  bdlt::Date(2000, 1,  7) },
            { L_,    2000,     1,  e_FRI,  2,  bdlt::Date(2000, 1, 14) },
            { L_,    2000,     1,  e_FRI,  3,  bdlt::Date(2000, 1, 21) },
            { L_,    2000,     1,  e_FRI,  4,  bdlt::Date(2000, 1, 28) },
            { L_,    2000,     1,  e_FRI,  5,  bdlt::Date(2000, 2,  4) },

            { L_,    2000,     1,  e_SAT,  1,  bdlt::Date(2000, 1,  1) },
            { L_,    2000,     1,  e_SAT,  2,  bdlt::Date(2000, 1,  8) },
            { L_,    2000,     1,  e_SAT,  3,  bdlt::Date(2000, 1, 15) },
            { L_,    2000,     1,  e_SAT,  4,  bdlt::Date(2000, 1, 22) },
            { L_,    2000,     1,  e_SAT,  5,  bdlt::Date(2000, 1, 29) }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                   LINE  = DATA[i].d_line;
            const int                   YEAR  = DATA[i].d_year;
            const int                   MONTH = DATA[i].d_month;
            const bdlt::DayOfWeek::Enum DOW   = DATA[i].d_dow;
            const int                   N     = DATA[i].d_n;
            const bdlt::Date            EXP   = DATA[i].d_exp;

            if (veryVerbose) {
                T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DOW) P_(N) P(EXP);
            }

            const bdlt::Date result =
                                Util::nthDayOfWeekInMonth(YEAR, MONTH, DOW, N);
            ASSERTV(LINE, EXP, result, EXP == result);
        }

        {
            // Test assertions
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_PASS(Util::nthDayOfWeekInMonth(    1,  1, e_SUN, 1));
            ASSERT_SAFE_PASS(Util::nthDayOfWeekInMonth( 9999,  1, e_SUN, 1));
            ASSERT_SAFE_FAIL(Util::nthDayOfWeekInMonth(    0,  1, e_SUN, 1));
            ASSERT_SAFE_FAIL(Util::nthDayOfWeekInMonth(10000,  1, e_SUN, 1));

            ASSERT_SAFE_PASS(Util::nthDayOfWeekInMonth(    1, 12, e_SUN, 1));
            ASSERT_SAFE_FAIL(Util::nthDayOfWeekInMonth(    1,  0, e_SUN, 1));
            ASSERT_SAFE_FAIL(Util::nthDayOfWeekInMonth(    1, 13, e_SUN, 1));

            ASSERT_SAFE_PASS(Util::nthDayOfWeekInMonth(    1,  1, e_SUN, 5));
            ASSERT_SAFE_FAIL(Util::nthDayOfWeekInMonth(    1,  1, e_SUN, 0));
            ASSERT_SAFE_FAIL(Util::nthDayOfWeekInMonth(    1,  1, e_SUN, 6));
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'previousDayOfWeekInclusive'
        //   Verify that the function returns the correct date.  Note that
        //   calendar irregularities are already compensated for by 'Date'
        //   and therefore are not a concern in this test.
        //
        // Concerns:
        //: 1 The date of the last 'dayOfWeek' on or before 'date' is returned.
        //:
        //: 2 If 'dayOfWeek' is equal to 'date.dayOfWeek()', 'date' is
        //:   returned.
        //
        // Plan:
        //: 1 Using the table-driven approach, select a range of inputs and
        //:   expected outputs covering all possible permutations of days of
        //:   the week (SUN-SAT x SUN-SAT).  Verify that the function returns
        //:   the correct value.  (C-1..2)
        //
        // Testing:
        //   Date previousDayOfWeekInclusive(dayOfWeek, date);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'previousDayOfWeekInclusive'" << endl
                          << "====================================" << endl;

        static const struct {
            int                   d_lineNum;    // source line number
            bdlt::DayOfWeek::Enum d_dayOfWeek;  // dayOfWeek
            bdlt::Date            d_date;       // date
            bdlt::Date            d_exp;        // expected result
        } DATA[] = {
          //    January 2000
          //  S  M Tu  W Th  F  S
          //                    1
          //  2  3  4  5  6  7  8
          //  9 10 11 12 13 14 15
          // 16 17 18 19 20 21 22
          // 23 24 25 26 27 28 29
          // 30 31

          //LINE DOW     DATE                       EXP
          //---- ---     ----                       ---
          // SUN
          { L_,  e_SUN,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  3) },
          { L_,  e_TUE,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  4) },
          { L_,  e_WED,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  5) },
          { L_,  e_THU,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  6) },
          { L_,  e_FRI,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  7) },
          { L_,  e_SAT,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  8) },

          // MON
          { L_,  e_SUN,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1,  4) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1,  5) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1,  6) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1,  7) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1,  8) },

          // TUE
          { L_,  e_SUN,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1,  5) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1,  6) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1,  7) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1,  8) },

          // WED
          { L_,  e_SUN,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1,  6) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1,  7) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1,  8) },

          // THU
          { L_,  e_SUN,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1,  7) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1,  8) },

          // FRI
          { L_,  e_SUN,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1,  8) },

          // SAT
          { L_,  e_SUN,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 15) },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                   LINE        = DATA[i].d_lineNum;
            const bdlt::DayOfWeek::Enum DAY_OF_WEEK = DATA[i].d_dayOfWeek;
            const bdlt::Date            DATE        = DATA[i].d_date;
            const bdlt::Date            EXP         = DATA[i].d_exp;

            if (veryVerbose) {
                T_ P_(LINE) P_(DAY_OF_WEEK) P_(DATE) P(EXP);
            }

            const bdlt::Date result = Util::previousDayOfWeekInclusive(
                                                                  DAY_OF_WEEK,
                                                                  DATE);
            ASSERTV(LINE, EXP, result, EXP == result);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'previousDayOfWeek'
        //   Verify that the function returns the correct date.  Note that
        //   calendar irregularities are already compensated for by 'Date'
        //   and therefore are not a concern in this test.
        //
        // Concerns:
        //: 1 The date of the last 'dayOfWeek' before 'date' is returned.
        //:
        //: 2 If 'dayOfWeek' is equal to 'date.dayOfWeek()', the date one week
        //:   before 'date' is returned, *not* 'date'.
        //
        // Plan:
        //: 1 Using the table-driven approach, select a range of inputs and
        //:   expected outputs covering all possible permutations of days of
        //:   the week (SUN-SAT x SUN-SAT).  Verify that the function returns
        //:   the correct value.  (C-1..2)
        //
        // Testing:
        //   Date previousDayOfWeek(dayOfWeek, date);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'previousDayOfWeek'" << endl
                          << "===========================" << endl;

        static const struct {
            int                   d_lineNum;    // source line number
            bdlt::DayOfWeek::Enum d_dayOfWeek;  // dayOfWeek
            bdlt::Date            d_date;       // date
            bdlt::Date            d_exp;        // expected result
        } DATA[] = {
          //    January 2000
          //  S  M Tu  W Th  F  S
          //                    1
          //  2  3  4  5  6  7  8
          //  9 10 11 12 13 14 15
          // 16 17 18 19 20 21 22
          // 23 24 25 26 27 28 29
          // 30 31

          //LINE DOW     DATE                       EXP
          //---- ---     ----                       ---
          // SUN
          { L_,  e_SUN,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  2) },
          { L_,  e_MON,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  3) },
          { L_,  e_TUE,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  4) },
          { L_,  e_WED,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  5) },
          { L_,  e_THU,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  6) },
          { L_,  e_FRI,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  7) },
          { L_,  e_SAT,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  8) },

          // MON
          { L_,  e_SUN,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1,  3) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1,  4) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1,  5) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1,  6) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1,  7) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1,  8) },

          // TUE
          { L_,  e_SUN,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1,  4) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1,  5) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1,  6) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1,  7) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1,  8) },

          // WED
          { L_,  e_SUN,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1,  5) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1,  6) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1,  7) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1,  8) },

          // THU
          { L_,  e_SUN,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1,  6) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1,  7) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1,  8) },

          // FRI
          { L_,  e_SUN,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1,  7) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1,  8) },

          // SAT
          { L_,  e_SUN,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1,  8) },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                   LINE        = DATA[i].d_lineNum;
            const bdlt::DayOfWeek::Enum DAY_OF_WEEK = DATA[i].d_dayOfWeek;
            const bdlt::Date            DATE        = DATA[i].d_date;
            const bdlt::Date            EXP         = DATA[i].d_exp;

            if (veryVerbose) {
                T_ P_(LINE) P_(DAY_OF_WEEK) P_(DATE) P(EXP);
            }

            const bdlt::Date result = Util::previousDayOfWeek(DAY_OF_WEEK,
                                                              DATE);
            ASSERTV(LINE, EXP, result, EXP == result);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'nextDayOfWeekInclusive'
        //   Verify that the function returns the correct date.  Note that
        //   calendar irregularities are already compensated for by 'Date'
        //   and therefore are not a concern in this test.
        //
        // Concerns:
        //: 1 The date of the first 'dayOfWeek' on or after 'date' is returned.
        //:
        //: 2 If 'dayOfWeek' is equal to 'date.dayOfWeek()', 'date' is
        //:   returned.
        //
        // Plan:
        //: 1 Using the table-driven approach, select a range of inputs and
        //:   expected outputs covering all possible permutations of days of
        //:   the week (SUN-SAT x SUN-SAT).  Verify that the function returns
        //:   the correct value.  (C-1..2)
        //
        // Testing:
        //   Date nextDayOfWeekInclusive(dayOfWeek, date);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'nextDayOfWeekInclusive'" << endl
                          << "================================" << endl;

        static const struct {
            int                   d_lineNum;    // source line number
            bdlt::DayOfWeek::Enum d_dayOfWeek;  // dayOfWeek
            bdlt::Date            d_date;       // date
            bdlt::Date            d_exp;        // expected result
        } DATA[] = {
          //    January 2000
          //  S  M Tu  W Th  F  S
          //                    1
          //  2  3  4  5  6  7  8
          //  9 10 11 12 13 14 15
          // 16 17 18 19 20 21 22
          // 23 24 25 26 27 28 29
          // 30 31

          //LINE DOW     DATE                       EXP
          //---- ---     ----                       ---
          // SUN
          { L_,  e_SUN,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1,  9) },
          { L_,  e_MON,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1, 15) },

          // MON
          { L_,  e_SUN,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 16) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 15) },

          // TUE
          { L_,  e_SUN,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 16) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 17) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 15) },

          // WED
          { L_,  e_SUN,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 16) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 17) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 18) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 15) },

          // THU
          { L_,  e_SUN,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 16) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 17) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 18) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 19) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 15) },

          // FRI
          { L_,  e_SUN,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 16) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 17) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 18) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 19) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 20) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 15) },

          // SAT
          { L_,  e_SUN,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 16) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 17) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 18) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 19) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 20) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 21) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 15) },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                   LINE        = DATA[i].d_lineNum;
            const bdlt::DayOfWeek::Enum DAY_OF_WEEK = DATA[i].d_dayOfWeek;
            const bdlt::Date            DATE        = DATA[i].d_date;
            const bdlt::Date            EXP         = DATA[i].d_exp;

            if (veryVerbose) {
                T_ P_(LINE) P_(DAY_OF_WEEK) P_(DATE) P(EXP);
            }

            const bdlt::Date result =
                               Util::nextDayOfWeekInclusive(DAY_OF_WEEK, DATE);
            ASSERTV(LINE, EXP, result, EXP == result);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'nextDayOfWeek'
        //   Verify that the function returns the correct date.  Note that
        //   calendar irregularities are already compensated for by 'Date'
        //   and therefore are not a concern in this test.
        //
        // Concerns:
        //: 1 The date of the first 'dayOfWeek' after 'date' is returned.
        //:
        //: 2 If 'dayOfWeek' is equal to 'date.dayOfWeek()', the date one week
        //:   from 'date' is returned, *not* 'date'.
        //
        // Plan:
        //: 1 Using the table-driven approach, select a range of inputs and
        //:   expected outputs covering all possible permutations of days of
        //:   the week (SUN-SAT x SUN-SAT).  Verify that the function returns
        //:   the correct value.  (C-1..2)
        //
        // Testing:
        //   Date nextDayOfWeek(dayOfWeek, date);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'nextDayOfWeek'" << endl
                          << "=======================" << endl;

        static const struct {
            int                   d_lineNum;    // source line number
            bdlt::DayOfWeek::Enum d_dayOfWeek;  // dayOfWeek
            bdlt::Date            d_date;       // date
            bdlt::Date            d_exp;        // expected result
        } DATA[] = {
          //    January 2000
          //  S  M Tu  W Th  F  S
          //                    1
          //  2  3  4  5  6  7  8
          //  9 10 11 12 13 14 15
          // 16 17 18 19 20 21 22
          // 23 24 25 26 27 28 29
          // 30 31

          //LINE DOW     DATE                       EXP
          //---- ---     ----                       ---
          // SUN
          { L_,  e_SUN,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1, 16) },
          { L_,  e_MON,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1, 10) },
          { L_,  e_TUE,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1,  9),  bdlt::Date(2000,  1, 15) },

          // MON
          { L_,  e_SUN,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 16) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 17) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 11) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 10),  bdlt::Date(2000,  1, 15) },

          // TUE
          { L_,  e_SUN,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 16) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 17) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 18) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 12) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 11),  bdlt::Date(2000,  1, 15) },

          // WED
          { L_,  e_SUN,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 16) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 17) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 18) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 19) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 13) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 12),  bdlt::Date(2000,  1, 15) },

          // THU
          { L_,  e_SUN,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 16) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 17) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 18) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 19) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 20) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 14) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 13),  bdlt::Date(2000,  1, 15) },

          // FRI
          { L_,  e_SUN,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 16) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 17) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 18) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 19) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 20) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 21) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 14),  bdlt::Date(2000,  1, 15) },

          // SAT
          { L_,  e_SUN,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 16) },
          { L_,  e_MON,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 17) },
          { L_,  e_TUE,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 18) },
          { L_,  e_WED,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 19) },
          { L_,  e_THU,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 20) },
          { L_,  e_FRI,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 21) },
          { L_,  e_SAT,  bdlt::Date(2000,  1, 15),  bdlt::Date(2000,  1, 22) },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                   LINE        = DATA[i].d_lineNum;
            const bdlt::DayOfWeek::Enum DAY_OF_WEEK = DATA[i].d_dayOfWeek;
            const bdlt::Date            DATE        = DATA[i].d_date;
            const bdlt::Date            EXP         = DATA[i].d_exp;

            if (veryVerbose) {
                T_ P_(LINE) P_(DAY_OF_WEEK) P_(DATE) P(EXP);
            }

            const bdlt::Date result = Util::nextDayOfWeek(DAY_OF_WEEK, DATE);
            ASSERTV(LINE, EXP, result, EXP == result);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'convertToYYYYMMDD'
        //
        // Concerns:
        //: 1 The function applies the correct formula to convert a 'Date'
        //:   to an integral value in the 'YYYYMMDD' format.
        //
        // Plan:
        //: 1 Use the table-driven approach to define a representative set of
        //:   dates:
        //:
        //:   1 Verify that calling 'convertToYYYYMMDD' on each date in the set
        //:     returns the expected result.
        //:
        //:   2 Further corroborate the result by verifying that calling
        //:     'convertFromYYYYMMDDRaw' on the result of P-1.1 returns the
        //:     original date.  (C-1)
        //
        // Testing:
        //   int convertToYYYYMMDD(const Date& date);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'convertToYYYYMMDD'" << endl
                          << "===========================" << endl;


        static const struct {
            int        d_lineNum;   // source line number
            bdlt::Date d_date;      // date value
            int        d_expected;  // expected result ('YYYYMMDD' format)
        } DATA[] = {
            //LINE DATE                       EXP
            //---- ----                       ---
            { L_,  bdlt::Date(   1,  1,  1),     10101 },
            { L_,  bdlt::Date(   1,  1,  2),     10102 },
            { L_,  bdlt::Date(   1,  2,  1),     10201 },
            { L_,  bdlt::Date(   2,  1,  1),     20101 },
            { L_,  bdlt::Date(  88,  5, 31),    880531 },
            { L_,  bdlt::Date( 123,  2, 10),   1230210 },
            { L_,  bdlt::Date(1954,  9, 17),  19540917 },
            { L_,  bdlt::Date(9998,  1,  1),  99980101 },
            { L_,  bdlt::Date(9998, 12, 31),  99981231 },
            { L_,  bdlt::Date(9999, 11, 30),  99991130 },
            { L_,  bdlt::Date(9999, 12, 30),  99991230 },
            { L_,  bdlt::Date(9999, 12, 31),  99991231 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int        LINE = DATA[ti].d_lineNum;
            const bdlt::Date DATE = DATA[ti].d_date;
            const int        EXP  = DATA[ti].d_expected;

            if (veryVerbose) { P_(LINE); P_(DATE); P(EXP); }

            int result = Util::convertToYYYYMMDD(DATE);
            ASSERTV(LINE, EXP, result, EXP == result);
            ASSERTV(LINE, DATE == Util::convertFromYYYYMMDDRaw(EXP));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'convertFromYYYYMMDD'
        //
        // Concerns:
        //: 1 The function correctly delegates validation to 'isValidYYYYMMDD'.
        //:
        //: 2 The function correctly delegates its operations to
        //:   'convertFromYYYYMMDDRaw' if the input value is valid.
        //:
        //: 3 If the input value is invalid, '*result' is unmodified.
        //:
        //: 4 If the input value is valid, 0 is returned; otherwise a non-zero
        //:   value is returned.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Call the function passing in an invalid 'YYYYMMDD' value; ensure
        //:   that a non-zero value is returned and that the '*result' is
        //:   unmodified.  (C-1, 3..4)
        //:
        //: 2 Call the function passing in a valid 'YYYYMMDD' value; ensure
        //:   that the return value is 0 and that '*result' is set to the
        //:   correct value.  (C-2, 4)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-5)
        //
        // Testing:
        //   int convertFromYYYYMMDD(Date *result, int yyyymmddValue);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'convertFromYYYYMMDD'" << endl
                          << "=============================" << endl;

        {
            // Invalid Input
            bdlt::Date result(1999, 2, 3);
            const bdlt::Date ZZ(result);

            int rc = Util::convertFromYYYYMMDD(&result, 1);
            ASSERTV(rc, 0 != rc);
            ASSERTV(result, ZZ == result);
        }

        {
            // Valid Input
            bdlt::Date result(1999, 2, 3);
            const bdlt::Date EXP(2000, 10, 30);

            int rc = Util::convertFromYYYYMMDD(&result, 20001030);
            ASSERTV(rc, 0 == rc);
            ASSERTV(result, EXP == result);
        }

        {
            // Test assertions
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Date result(1999, 2, 3);

            ASSERT_SAFE_PASS(Util::convertFromYYYYMMDD(&result, 20001030));
            ASSERT_SAFE_FAIL(Util::convertFromYYYYMMDD(      0, 20001030));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'convertFromYYYYMMDDRaw'
        //
        // Concerns:
        //: 1 The function works for valid inputs.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the table-driven approach to test representative valid
        //:   inputs, and verify that the function returns the expected result.
        //:   (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-2)
        //
        // Testing:
        //   Date convertFromYYYYMMDDRaw(int yyyymmddValue);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'convertFromYYYYMMDDRaw'" << endl
                          << "================================" << endl;

        static const struct {
            int        d_lineNum;        // source line number
            int        d_yyyymmddValue;  // date value ('YYYYMMDD' format)
            bdlt::Date d_exp;            // expected result

        } DATA[] = {
            //LINE  INPUT VALUE   EXP
            //----  -----------   ---
            { L_,        10101,   bdlt::Date(   1,  1,  1) },
            { L_,        10102,   bdlt::Date(   1,  1,  2) },
            { L_,        10201,   bdlt::Date(   1,  2,  1) },
            { L_,        20101,   bdlt::Date(   2,  1,  1) },
            { L_,       880531,   bdlt::Date(  88,  5, 31) },
            { L_,      1230210,   bdlt::Date( 123,  2, 10) },
            { L_,     19540917,   bdlt::Date(1954,  9, 17) },
            { L_,     99980101,   bdlt::Date(9998,  1,  1) },
            { L_,     99981231,   bdlt::Date(9998, 12, 31) },
            { L_,     99991130,   bdlt::Date(9999, 11, 30) },
            { L_,     99991230,   bdlt::Date(9999, 12, 30) },
            { L_,     99991231,   bdlt::Date(9999, 12, 31) },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int        LINE     = DATA[ti].d_lineNum;
            const int        YYYYMMDD = DATA[ti].d_yyyymmddValue;
            const bdlt::Date EXP      = DATA[ti].d_exp;

            if (veryVerbose) { P_(YYYYMMDD);  P(EXP); }

            const bdlt::Date result = Util::convertFromYYYYMMDDRaw(YYYYMMDD);

            ASSERTV(LINE, EXP, result, EXP == result);
        }

        {
            // Test assertions
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_FAIL(Util::convertFromYYYYMMDDRaw(1));
            ASSERT_SAFE_PASS(Util::convertFromYYYYMMDDRaw(20001210));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'isValidYYYYMMDD'
        //   The function correctly discriminates between valid and invalid
        //   dates in the "YYYYMMDD" format.
        //
        // Concerns:
        //: 1 The function works for valid and invalid inputs.
        //
        // Plan:
        //: 1 Use the table-driven approach to test representative valid and
        //:   invalid inputs, and verify the function returns the expected
        //:   result.  (C-1)
        //
        // Testing:
        //   bool isValidYYYYMMDD(int yyyymmddValue);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'isValidYYYYMMDD'" << endl
                          << "=========================" << endl;

        static const struct {
            int  d_lineNum;        // source line number
            int  d_yyyymmddValue;  // date value ('YYYYMMDD' format)
            bool d_exp;            // expected result

        } DATA[] = {
            //LINE                        INPUT VALUE   EXP
            //----                        -----------   ---
            { L_,                              10101,   true  },
            { L_,                              10102,   true  },
            { L_,                              10201,   true  },
            { L_,                              20101,   true  },
            { L_,                             880531,   true  },
            { L_,                            1230210,   true  },
            { L_,                           19540917,   true  },
            { L_,                           20000229,   true  },
            { L_,                           99980101,   true  },
            { L_,                           99981231,   true  },
            { L_,                           99991130,   true  },
            { L_,                           99991230,   true  },
            { L_,                           99991231,   true  },

            { L_,                          -19500101,   false },
            { L_,                                 -1,   false },
            { L_,                                  0,   false },
            { L_,                                  1,   false },
            { L_,                               1231,   false },
            { L_,                           19540017,   false },
            { L_,                           19541317,   false },
            { L_,                           20010229,   false },
            { L_,                           19540900,   false },
            { L_,                           19540931,   false },
            { L_,                           99991232,   false },
            { L_,                           99991331,   false },
            { L_,                          100000917,   false },
            { L_,                          100000917,   false },
            { L_,    bsl::numeric_limits<int>::max(),   false },
            { L_,    bsl::numeric_limits<int>::min(),   false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE     = DATA[ti].d_lineNum;
            const int  YYYYMMDD = DATA[ti].d_yyyymmddValue;
            const bool EXP      = DATA[ti].d_exp;

            if (veryVerbose) { P_(YYYYMMDD);  P(EXP); }

            ASSERTV(LINE, EXP == Util::isValidYYYYMMDD(YYYYMMDD));
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
