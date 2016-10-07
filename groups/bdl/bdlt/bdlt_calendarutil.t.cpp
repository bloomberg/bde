// bdlt_calendarutil.t.cpp                                            -*-C++-*-
#include <bdlt_calendarutil.h>

#include <bdlt_calendar.h>
#include <bdlt_date.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_set.h>

using namespace BloombergLP;
using namespace BloombergLP::bdlt;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'CalendarUtil' provided a suite of functions for manipulating on dates with
// the use of calendars.  These test must verify that each of the functions
// behaves as documented, and works correctly on the full range of data that a
// 'CalendarUtil' may be used.
//-----------------------------------------------------------------------------
// [ 9] int addBusinessDaysIfValid(bdlt::Date *result, orig, cdr, num);
// [ 8] int nthBusinessDayOfMonthOrMaxIfValid(res, cal, year, month, n);
// [ 7] shiftIfValid(bdlt::Date *result, orig, calendar, convention)
// [ 7] shiftIfValid(res, orig, cdr, conv, specDay, extSpecDay, specConv)
// [ 3] shiftModifiedPrecedingIfValid(bdlt::Date *result, orig, calendar)
// [ 4] shiftModifiedFollowingIfValid(bdlt::Date *result, orig, calendar)
// [ 5] shiftFollowingIfValid(bdlt::Date *result, orig, calendar)
// [ 6] shiftPrecedingIfValid(bdlt::Date *result, orig, calendar)
// [ 9] int subtractBusinessDaysIfValid(bdlt::Date *result, orig, cdr, num);
//-----------------------------------------------------------------------------
// [10] USAGE EXAMPLE
// [ 1] parseCalendar(const char *, const bdlt::Date&)
// [ 2] getStartDate(const char *)
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
//                     NON-STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------
#define D_(X) bdlt::Date((X/10000),                     \
                        ((X%10000)/100),                \
                        ((X%10000)%100))    // bdlt::Date from valid YYYYMMDD

#define LOOP7_ASSERT(I,J,K,L,M,N,O,X)                   \
    if (!(X)) { cout << #I << ": " << I << "\t"         \
                     << #J << ": " << J << "\t"         \
                     << #K << ": " << K << "\t"         \
                     << #L << ": " << L << "\t"         \
                     << #M << ": " << M << "\t"         \
                     << #N << ": " << N << "\t"         \
                     << #O << ": " << O << "\t";        \
                aSsErT(1, #X, __LINE__); }

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
typedef CalendarUtil Util;

const int NV = -5; // value that is loaded to the 'result' before prior to call

// ============================================================================
//                                TEST FUNCTIONS
// ----------------------------------------------------------------------------

bdlt::Calendar parseCalendar(const char *input, const bdlt::Date& startDate)
    // Return the calendar value indicated by the specified 'input' having the
    // specified 'startDate', and containing a sequence of characters with
    // symbol 'n' representing non-business day, symbol 'B' representing
    // business day, symbol '.' representing date outside of calendar range and
    // symbol '|' representing month boundaries.
{
    BSLS_ASSERT_SAFE(input);

    int inputLen = static_cast<int>(strlen(input));

    bdlt::Calendar result = (  inputLen
                             ? bdlt::Calendar(startDate,
                                              startDate + inputLen - 1)
                             : bdlt::Calendar());

    int numIgnored = 0; // for adjusting the length of the calendar

    bdlt::Date currentDate = startDate;

    int i = 0;

    // first skip dates outside of range of calendar

    for (; i < inputLen && ((input[i] == '.') || (input[i] == '|')); ++i) {
        ++numIgnored;
    }

    for (; i < inputLen; ++i) {
        BSLS_ASSERT_SAFE(   input[i] == 'n'
                         || input[i] == 'B'
                         || input[i] == '.'
                         || input[i] == '|');

        if (input[i] == '.') {
            ++numIgnored;
            continue;
        }
        else if (input[i] == 'n') {
            result.addHoliday(currentDate);
        }
        else if (input[i] == '|') {
            ++numIgnored;
            continue; // month Pipe
        }
        ++currentDate;
    }

    if (numIgnored >= inputLen) {
        result.removeAll();
    }
    else if (numIgnored) {
        result.setValidRange(startDate,
                             startDate + inputLen
                                       - 1
                                       - numIgnored);
    }
    return result;
}

int getStartDate(const char *input)
    // Return the distance in characters (positive or negative) from the first
    // occurrence in the specified 'input' of 'n' or 'B' (holiday or business
    // day) to the first occurrence of '|' (start of month).  If this distance
    // happens to be negative (i.e calendar starts before the month), 1 is
    // added to the returned value.  If there is no occurrence of 'n' or 'B',
    // return 999.
{
    BSLS_ASSERT_SAFE(input);
    int inputLen = static_cast<int>(strlen(input));

    int daysBeforeMonth = 0;
    for (int i = 0; i < inputLen; ++i) {
        if (input[i] == '|') {
            daysBeforeMonth = -i; // invert
            break;
        }
    }

    for (int i = 0; i < inputLen; ++i) {
        if ((input[i] != '.') && (input[i] != '|')) {
            if ( (i + daysBeforeMonth) < 0) {
                // Adjust to account for non-intuitive convention.
                return i + daysBeforeMonth + 1;                       // RETURN
            }
            return i + daysBeforeMonth;                               // RETURN
        }
    }

    return 999;
}

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) {
      case 0:
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
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
///Example 1: Manipulating Dates with 'CalendarUtil'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to determine the actual interest payment date in
// January 2014 from a US bond that pays on the 20th of each month and uses the
// modified-following date-shifting convention.
//
// We create a calendar, 'calUS', that has the calendar information populated
// for the US in 2014.  We then use the 'shiftIfValid' function, provided by
// 'CalendarUtil', to compute the payment date.
//
// First, we create a date for January 1, 2014 that corresponds to the nominal
// payment date (which happens to be holiday) and a calendar with valid range
// from April 20, 2012 through April 20, 2014, typical weekend days, and the
// holiday:
//..
    const bdlt::Date unadjustedDate(2014, 1, 20);

    const bdlt::Date startDate(2012, 4, 20);
    const bdlt::Date endDate(2014, 4, 20);

    bdlt::Calendar calUS(startDate, endDate);
    calUS.addWeekendDay(bdlt::DayOfWeek::e_SAT);
    calUS.addWeekendDay(bdlt::DayOfWeek::e_SUN);
    calUS.addHoliday(unadjustedDate);
//..
// Now, we determine the actual payment date by invoking the 'shiftIfValid'
// function:
//..
    bdlt::Date result;
    int        status = CalendarUtil::shiftIfValid(
                                           &result,
                                           unadjustedDate,
                                           calUS,
                                           CalendarUtil::e_MODIFIED_FOLLOWING);
//..
// Notice that, 'e_MODIFIED_FOLLOWING' is specified as an argument to
// 'shiftIfValid' to indicate that we want to use the modified-following
// date-shifting convention.
//
// Finally, we verify that the resulting date is correct:
//..
    const bdlt::Date expected(2014, 1, 21);

    ASSERT(0 == status);
    ASSERT(expected == result);
//..
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING '(add|subtract)BusinessDaysIfValid'
        //   Ensure that this function either loads the expected result and
        //   indicates a successful return value or the expected error code
        //   with an un-modified result.
        //
        // Concerns:
        //: 1 If either the 'original' or resulting date is out of the valid
        //:   range of the calendar, an error code is returned leaving the
        //:   result unchanged, otherwise the expected value is loaded into the
        //:   result and success indicated in return value.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the table-driven approach, define a representative set of
        //:   valid and invalid inputs to address the above concerns. (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values.  (C-2)
        //
        // Testing:
        //   int addBusinessDays(bdlt::Date *result, orig, cdr, num);
        //   int subtractBusinessDays(bdlt::Date *result, orig, cdr, num);
        // --------------------------------------------------------------------

        bslma::TestAllocator         defaultAllocator;
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        if (verbose) {
            cout << endl
                 << "TESTING '(add|subtract)BusinessDaysIfValid'" << endl
                 << "================================" << endl;
        }

        static const struct {
            int         d_line;        // source line
            const char *d_input_p;     // input values
            int         d_numBusDays;  // number of business days
            int         d_original;    // original date
            int         d_ret;         // expected return value
            int         d_result;      // expected result
        } DATA[] = {
            // Both original and desired value out of range.

            //LN INPUT                                    NUM  ORIG RET RESULT
            //-- -----                                    ---  ---- --- ------
            {L_, "..|...............................|..",  5,  -2,   1,  NV},
            {L_, "..|...............................|..",  1,  -2,   1,  NV},
            {L_, "..|...............................|..",  0,  -2,   1,  NV},
            {L_, "..|...............................|..", -1,  -2,   1,  NV},
            {L_, "..|...............................|..", -5,  -2,   1,  NV},

            // Original is in range, desired value is out of range.

            //LN INPUT                                    NUM  ORIG RET RESULT
            //-- -----                                    ---  ---- --- ------
            {L_, "n.|...............................|..",  5,  -2,   1,  NV},
            {L_, "n.|...............................|..",  1,  -2,   1,  NV},
            {L_, "n.|...............................|..",  0,  -2,   1,  NV},
            {L_, "n.|...............................|..", -1,  -2,   1,  NV},
            {L_, "n.|...............................|..", -5,  -2,   1,  NV},

            // Original is out of range, desired value is in range.

            //LN INPUT                                    NUM  ORIG RET RESULT
            //-- -----                                    ---  ---- --- ------
            {L_, "..|..BBBBB........................|..",  5,  -2,   1,  NV},
            {L_, ".B|...............................|..",  1,  -2,   1,  NV},
            {L_, ".B|...............................|..",  0,  -2,   1,  NV},
            {L_, "B.|...............................|..", -1,  -1,   1,  NV},
            {L_, "BB|BBB............................|..", -5,   8,   1,  NV},

            // Both original and desired value are in range, original is a
            // business-day.

            //LN INPUT                                    NUM  ORIG RET RESULT
            //-- -----                                    ---  ---- --- ------
            {L_, "BB|nnBBBB.........................|..",  5,  -1,   0,   6},
            {L_, "BB|...............................|..",  1,  -1,   0,   0},
            {L_, "B.|...............................|..",  0,  -1,   0,  -1},
            {L_, "BB|...............................|..", -1,   0,   0,  -1},
            {L_, "nn|BBBBnBnnBn.....................|..", -5,   9,   0,   1},

            // Both original and desired value are in range, original is *not*
            // a business-day.

            //LN INPUT                                    NUM  ORIG RET RESULT
            //-- -----                                    ---  ---- --- ------
            {L_, "nB|nnBBBB.........................|..",  5,  -1,   0,   6},
            {L_, "nB|...............................|..",  1,  -1,   0,   0},
            {L_, "nB|...............................|..",  0,  -1,   0,   0},
            {L_, "Bn|...............................|..", -1,   0,   0,  -1},
            {L_, "nn|BBBBnBnnnn.....................|..", -5,   9,   0,   1}
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        const bdlt::Date ORIGIN = bdlt::Date(1999, 12, 31);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE     = DATA[i].d_line;
            const char       *INPUT    = DATA[i].d_input_p;
            const bdlt::Date  START    = ORIGIN + getStartDate(INPUT);
            const bdlt::Date  ORIGINAL = ORIGIN + DATA[i].d_original;
            const int         RET      = DATA[i].d_ret;
            const bdlt::Date  RESULT   = ORIGIN + DATA[i].d_result;
            const int         NUMDAYS  = DATA[i].d_numBusDays;

            bdlt::Calendar calendar = parseCalendar(INPUT, START);

            bdlt::Date LOADED1  = ORIGIN - 5;
            int       rStatus1 = Util::addBusinessDaysIfValid(&LOADED1,
                                                              ORIGINAL,
                                                              calendar,
                                                              NUMDAYS);

            bdlt::Date LOADED2  = ORIGIN - 5;
            int       rStatus2 = Util::subtractBusinessDaysIfValid(&LOADED2,
                                                                   ORIGINAL,
                                                                   calendar,
                                                                   -NUMDAYS);

            if (veryVerbose) {
                T_ P_(LINE) P(NUMDAYS) P_(ORIGINAL) P_(RESULT) P_(LOADED1);
            }

            // Check status
            LOOP7_ASSERT(LINE,
                         INPUT,
                         NUMDAYS,
                         START,
                         ORIGINAL,
                         RET,
                         rStatus1,
                         RET == rStatus1);

            // Check loaded
            LOOP7_ASSERT(LINE,
                         INPUT,
                         NUMDAYS,
                         START,
                         ORIGINAL,
                         RESULT,
                         LOADED1,
                         RESULT == LOADED1);

            if (0 != NUMDAYS) {
                // Check status
                LOOP7_ASSERT(LINE,
                             INPUT,
                             NUMDAYS,
                             START,
                             ORIGINAL,
                             RET,
                             rStatus2,
                             RET == rStatus2);

                // Check loaded
                LOOP7_ASSERT(LINE,
                             INPUT,
                             NUMDAYS,
                             START,
                             ORIGINAL,
                             RESULT,
                             LOADED2,
                             RESULT == LOADED2);
            }
            else {
                bdlt::Date LOADED_P  = ORIGIN - 5;
                const int RET_P = Util::shiftPrecedingIfValid(&LOADED_P,
                                                              ORIGINAL,
                                                              calendar);
                // Check status
                LOOP7_ASSERT(LINE,
                             INPUT,
                             NUMDAYS,
                             START,
                             ORIGINAL,
                             RET_P,
                             rStatus2,
                             (0 == RET_P) == (0 == rStatus2));

                // Check loaded
                LOOP7_ASSERT(LINE,
                             INPUT,
                             NUMDAYS,
                             START,
                             ORIGINAL,
                             LOADED_P,
                             LOADED2,
                             LOADED_P == LOADED2);
            }
        }

        // negative tests
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Calendar cdr;
            bdlt::Date     date;
            bdlt::Date     result;

            ASSERT_PASS(Util::addBusinessDaysIfValid(&result,
                                                     date,
                                                     cdr,
                                                     0));
            ASSERT_FAIL(Util::addBusinessDaysIfValid(0,
                                                     date,
                                                     cdr,
                                                     0));

            ASSERT_PASS(Util::subtractBusinessDaysIfValid(&result,
                                                          date,
                                                          cdr,
                                                          0));
            ASSERT_FAIL(Util::subtractBusinessDaysIfValid(0,
                                                          date,
                                                          cdr,
                                                          0));
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'nthBusinessDayOfMonthOrMaxIfValid'
        //   Ensure that this function correctly counts the number of business
        //   days starting either at the beginning or the end of the month.
        //   Also ensure that defensive checks are triggered for
        //   out-of-contract inputs.
        //
        // Concerns:
        //: 1 If there are more than 'n' business days in the specified month,
        //:   the 'n'th business day is returned counting forward or backward.
        //:
        //: 2 If there are fewer than 'n' business days in the specified month,
        //:   the business day with maximum count is returned counting forward
        //:   or backward.
        //:
        //: 3 The method is unsuccessful as expected.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the table-driven approach, define a representative set of
        //:   valid inputs to cover calendars with fewer, more, or exactly the
        //:   specified number of business days in both forward and backward
        //:   counting directions (C-1), (C-2)
        //:
        //: 2 Explicitely test situations were the method should be
        //:   unsuccessful.  (C-3)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values.  (C-4)
        //
        // Testing:
        //   int nthBusinessDayOfMonthOrMaxIfValid(res, cal, year, month, n);
        // --------------------------------------------------------------------

        bslma::TestAllocator         defaultAllocator;
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        if (verbose) {
            cout << endl
                 << "TESTING 'nthBusinessDayOfMonthOrMaxIfValid'" << endl
                 << "===========================================" << endl;
        }

        static const struct {
            int         d_line;     // source line
            const char *d_input_p;  // input values
            int         d_month;    // month
            int         d_leap;     // is it a leap year
            int         d_n;        // 'n'
            int         d_result;   // expected result
        } DATA[] = {
            // Different length months and full range with no holidays

            //LN INPUT                                    MON LEAP N  RESULT
            //-- -----                                    --- ---- -  ------
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",   1, 0,  1,      1},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",   1, 0,  2,      2},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",   1, 0, 15,     15},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",   1, 0, 30,     30},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",   1, 0, 31,     31},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",   1, 0, 40,     31},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",   1, 0, -1,     31},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",   1, 0, -2,     30},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",   1, 0,-15,     17},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",   1, 0,-30,      2},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",   1, 0,-31,      1},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",   1, 0,-40,      1},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBB",   4, 0,  1,      1},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBB",   4, 0,  2,      2},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBB",   4, 0, 15,     15},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBB",   4, 0, 29,     29},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBB",   4, 0, 30,     30},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBB",   4, 0, 40,     30},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBB",   4, 0, -1,     30},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBB",   4, 0, -2,     29},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBB",   4, 0,-15,     16},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBB",   4, 0,-29,      2},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBB",   4, 0,-30,      1},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBB",   4, 0,-40,      1},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBB",   2, 1,  1,      1},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBB",   2, 1,  2,      2},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBB",   2, 1, 15,     15},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBB",   2, 1, 28,     28},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBB",   2, 1, 29,     29},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBB",   2, 1, 40,     29},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBB",   2, 1, -1,     29},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBB",   2, 1, -2,     28},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBB",   2, 1,-15,     15},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBB",   2, 1,-28,      2},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBB",   2, 1,-29,      1},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBB",   2, 1,-40,      1},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBBB",   2, 0,  1,      1},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBBB",   2, 0,  2,      2},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBBB",   2, 0, 15,     15},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBBB",   2, 0, 27,     27},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBBB",   2, 0, 28,     28},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBBB",   2, 0, 40,     28},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBBB",   2, 0, -1,     28},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBBB",   2, 0, -2,     27},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBBB",   2, 0,-15,     14},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBBB",   2, 0,-27,      2},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBBB",   2, 0,-28,      1},
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBB|BBBBB",   2, 0,-40,      1},

            // Exactly the same number of days as requested

            //LN INPUT                                    MON LEAP N  RESULT
            //-- -----                                    --- ---- -  ------
            {L_, "..|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|..",   1, 0,  1,      1},
            {L_, "..|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|..",   1, 0, -1,      1},
            {L_, "..|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|..",   1, 0,  1,     31},
            {L_, "..|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|..",   1, 0, -1,     31},
            {L_, "..|nBnnnnBnnnnnnnnnnnBnnnnnnnnnnnn|..",   1, 0,  3,     19},
            {L_, "..|nBnnnnBnnnnnnnnnnnBnnnnnnnnnnnn|..",   1, 0, -3,      2},
            {L_, "..|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|..",   1, 0, 31,     31},
            {L_, "..|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|..",   1, 0,-31,      1},

            // Fewer business days than the number requested
            //LN INPUT                                    MON LEAP N  RESULT
            //-- -----                                    --- ---- -  ------
            {L_, "..|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|..",   1, 0,  2,      1},
            {L_, "..|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|..",   1, 0, -2,      1},
            {L_, "..|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|..",   1, 0,  2,     31},
            {L_, "..|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|..",   1, 0, -2,     31},
            {L_, "..|nBnnnnBnnnnnnnnnnnBnnnnnnnnnnnn|..",   1, 0,  4,     19},
            {L_, "..|nBnnnnBnnnnnnnnnnnBnnnnnnnnnnnn|..",   1, 0, -4,      2},
            {L_, "..|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|..",   1, 0, 32,     31},
            {L_, "..|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|..",   1, 0,-32,      1},

            // More business days than the number requested

            //LN INPUT                                    MON LEAP N  RESULT
            //-- -----                                    --- ---- -  ------
            {L_, "..|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|..",   1, 0,  1,      1},
            {L_, "..|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|..",   1, 0, -1,      2},
            {L_, "..|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|..",   1, 0,  1,     30},
            {L_, "..|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|..",   1, 0, -1,     31},
            {L_, "..|nBnnnnBnnnnnnnnnnnBnnnnnnnnnnnn|..",   1, 0,  2,      7},
            {L_, "..|nBnnnnBnnnnnnnnnnnBnnnnnnnnnnnn|..",   1, 0, -2,      7},
            {L_, "..|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|..",   1, 0, 30,     30},
            {L_, "..|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|..",   1, 0,-30,      2}
        };

        const int NON_LEAP_YEAR = 2014;
        const int LEAP_YEAR = 2016;

        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);
        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE   = DATA[i].d_line;
            const char       *INPUT  = DATA[i].d_input_p;
            const int         YEAR   = (  DATA[i].d_leap
                                        ? LEAP_YEAR
                                        : NON_LEAP_YEAR);
            const int         MONTH  = DATA[i].d_month;
            const bdlt::Date  ORIGIN = bdlt::Date(YEAR, MONTH, 1) - 1;
            const bdlt::Date  START  = ORIGIN + getStartDate(INPUT);
            const bdlt::Date  RESULT = ORIGIN + DATA[i].d_result;
            const int         N      = DATA[i].d_n;

            bdlt::Calendar calendar = parseCalendar(INPUT, START);

            bdlt::Date OUTPUT;
            int        RV = Util::nthBusinessDayOfMonthOrMaxIfValid(&OUTPUT,
                                                                    calendar,
                                                                    YEAR,
                                                                    MONTH,
                                                                    N);

            // Check return value.

            LOOP7_ASSERT(LINE,
                         INPUT,
                         YEAR,
                         MONTH,
                         N,
                         OUTPUT,
                         RESULT,
                         0 == RV);

            if (veryVerbose) {
                T_ P_(LINE) P_(YEAR) P_(MONTH) P_(N) P_(OUTPUT) P_(RESULT);
            }

            // Check RESULT

            LOOP7_ASSERT(LINE,
                         INPUT,
                         YEAR,
                         MONTH,
                         N,
                         OUTPUT,
                         RESULT,
                         RESULT == OUTPUT);
        }

        // unsuccessful method invocations
        {

            bdlt::Calendar cdr;
            bdlt::Date     OUTPUT;

            // invalid calendar
            ASSERT(0 != Util::nthBusinessDayOfMonthOrMaxIfValid(&OUTPUT,
                                                                cdr,
                                                                2000,
                                                                1,
                                                                1));
            ASSERT(bdlt::Date() == OUTPUT);

            cdr.setValidRange(bdlt::Date(2000, 1, 1), bdlt::Date(2000, 1, 31));

            // not in valid range of 'cdr'
            ASSERT(0 != Util::nthBusinessDayOfMonthOrMaxIfValid(&OUTPUT,
                                                                cdr,
                                                                2000,
                                                                2,
                                                                1));
            ASSERT(bdlt::Date() == OUTPUT);

            // no business days
            cdr.addWeekendDay(bdlt::DayOfWeek::e_SUN);
            cdr.addWeekendDay(bdlt::DayOfWeek::e_MON);
            cdr.addWeekendDay(bdlt::DayOfWeek::e_TUE);
            cdr.addWeekendDay(bdlt::DayOfWeek::e_WED);
            cdr.addWeekendDay(bdlt::DayOfWeek::e_THU);
            cdr.addWeekendDay(bdlt::DayOfWeek::e_FRI);
            cdr.addWeekendDay(bdlt::DayOfWeek::e_SAT);

            ASSERT(0 != Util::nthBusinessDayOfMonthOrMaxIfValid(&OUTPUT,
                                                                cdr,
                                                                2000,
                                                                1,
                                                                1));
            ASSERT(bdlt::Date() == OUTPUT);
        }

        // negative tests
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Calendar cdr;
            bdlt::Date     OUTPUT;

            cdr.setValidRange(bdlt::Date(2000, 1, 1), bdlt::Date(2000, 1, 31));

            // invalid result
            ASSERT_FAIL(Util::nthBusinessDayOfMonthOrMaxIfValid(
                                                         0, cdr, 2000,  1, 1));

            // invalid year
            ASSERT_FAIL(Util::nthBusinessDayOfMonthOrMaxIfValid(
                                                   &OUTPUT, cdr,   -1,  1, 1));

            // valid year
            ASSERT_PASS(Util::nthBusinessDayOfMonthOrMaxIfValid(
                                                   &OUTPUT, cdr, 2000,  1, 1));

            // invalid month
            ASSERT_FAIL(Util::nthBusinessDayOfMonthOrMaxIfValid(
                                                   &OUTPUT, cdr, 2000, 13, 1));

            // valid month
            ASSERT_PASS(Util::nthBusinessDayOfMonthOrMaxIfValid(
                                                   &OUTPUT, cdr, 2000,  1, 1));

            // invalid 'n'
            ASSERT_FAIL(Util::nthBusinessDayOfMonthOrMaxIfValid(
                                                   &OUTPUT, cdr, 2000,  1, 0));

            // valid 'n'
            ASSERT_PASS(Util::nthBusinessDayOfMonthOrMaxIfValid(
                                                   &OUTPUT, cdr, 2000,  1, 1));
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'shiftIfValid'
        //
        // Concerns:
        //: 1 This method calls appropriate functions depending on the
        //:   specified convention(s).
        //:
        //: 2 This method loads 'original' date to the 'result' if the used
        //:   convention 'e_UNADJUSTED'.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test that, when convention is used content of the 'result' and
        //:   the indication of success is the same as when calling
        //:   corresponding function. (C-1)
        //:
        //: 2 Verify that, when 'e_UNADJUSTED' is used as the convention, the
        //:   'result' contains the value of 'original' date. (C-2)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values.  (C-3)
        //
        // Testing:
        //  shiftIfValid(bdlt::Date *result, orig, calendar, convention)
        //  shiftIfValid(res, orig, cdr, conv, specDay, extSpecDay, specConv)
        // --------------------------------------------------------------------

        const bdlt::Date startDate(2012, 4, 20);
        const bdlt::Date endDate(2014, 4, 20);

        bdlt::Calendar calendar(startDate, endDate);
        calendar.addWeekendDay(bdlt::DayOfWeek::e_SAT);
        calendar.addWeekendDay(bdlt::DayOfWeek::e_SUN);

        // Add holidays that are will not affect special processing in latter
        // tests.

        calendar.addHoliday(bdlt::Date(2012, 7,  4));
        calendar.addHoliday(bdlt::Date(2013, 2, 12));
        calendar.addHoliday(bdlt::Date(2014, 1, 23));

        if (verbose) cout << endl
                          << "CLASS METHOD 'shiftIfValid'" << endl
                          << "===========================" << endl;

        if (verbose) {
            cout << endl
                 << "'shiftIfValid (r, o, c, c)'" << endl
                 << "===========================" << endl;
        }

        for (bdlt::Date date = startDate - 2; date <= endDate + 2; ++date) {
            {
                bdlt::Date result;
                int        status = CalendarUtil::shiftIfValid(
                                                   &result,
                                                   date,
                                                   calendar,
                                                   CalendarUtil::e_UNADJUSTED);
                bdlt::Date EXP_RESULT = date;
                int        EXP_STATUS = 0;

                ASSERT(result == EXP_RESULT);
                ASSERT((0 == status) == (0 == EXP_STATUS));
            }
            {
                bdlt::Date result;
                int        status = CalendarUtil::shiftIfValid(
                                                    &result,
                                                    date,
                                                    calendar,
                                                    CalendarUtil::e_FOLLOWING);
                bdlt::Date EXP_RESULT;
                int        EXP_STATUS = CalendarUtil::shiftFollowingIfValid(
                                                    &EXP_RESULT,
                                                    date,
                                                    calendar);

                ASSERT(result == EXP_RESULT);
                ASSERT((0 == status) == (0 == EXP_STATUS));
            }
            {
                bdlt::Date result;
                int        status = CalendarUtil::shiftIfValid(
                                                    &result,
                                                    date,
                                                    calendar,
                                                    CalendarUtil::e_PRECEDING);
                bdlt::Date EXP_RESULT;
                int        EXP_STATUS = CalendarUtil::shiftPrecedingIfValid(
                                                    &EXP_RESULT,
                                                    date,
                                                    calendar);

                ASSERT(result == EXP_RESULT);
                ASSERT((0 == status) == (0 == EXP_STATUS));
            }
        }

        if (verbose) {
            cout << endl
                 << "'shiftIfValid(r, o, c, c, s, e, s)'" << endl
                 << "===================================" << endl;
        }

        // Add holidays that are will affect special processing and store these
        // dates in 'specialDays'.

        bsl::set<bdlt::Date> specialDays;
        {
            specialDays.insert(bdlt::Date(2012, 7, 19));
            specialDays.insert(bdlt::Date(2012, 7, 20));
            specialDays.insert(bdlt::Date(2013, 2, 22));
            specialDays.insert(bdlt::Date(2014, 1,  5));
            specialDays.insert(bdlt::Date(2014, 1,  6));
            specialDays.insert(bdlt::Date(2014, 1,  7));
            specialDays.insert(bdlt::Date(2014, 1,  8));
            specialDays.insert(bdlt::Date(2014, 1,  9));
            specialDays.insert(bdlt::Date(2014, 1, 10));
        }
        {
            bsl::set<bdlt::Date>::iterator iter = specialDays.begin();
            while (iter != specialDays.end()) {
                calendar.addHoliday(*iter);
                ++iter;
            }
        }

        static const CalendarUtil::ShiftConvention CONVENTION[] = {
            CalendarUtil::e_UNADJUSTED,
            CalendarUtil::e_FOLLOWING,
            CalendarUtil::e_PRECEDING
        };

        static int NUM = static_cast<int>(  sizeof CONVENTION
                                          / sizeof *CONVENTION);

        for (int ci = 0; ci < NUM; ++ci) {
            for (int cj = 0; cj < NUM; ++cj) {
                for (bdlt::Date date = startDate - 2;
                     date <= endDate + 2;
                     ++date) {

                    if (veryVerbose) {
                        T_ P_(ci) P_(cj) P(date);
                    }

                    {
                        bdlt::Date result;
                        int        status = CalendarUtil::shiftIfValid(
                                                   &result,
                                                   date,
                                                   calendar,
                                                   CONVENTION[ci],
                                                   bdlt::DayOfWeek::e_SAT,
                                                   false,
                                                   CONVENTION[cj]);

                        bdlt::Date EXP_RESULT;
                        int        EXP_STATUS;

                        if (bdlt::DayOfWeek::e_SAT != date.dayOfWeek()) {
                            EXP_STATUS = CalendarUtil::shiftIfValid(
                                                               &EXP_RESULT,
                                                               date,
                                                               calendar,
                                                               CONVENTION[ci]);
                        }
                        else {
                            EXP_STATUS = CalendarUtil::shiftIfValid(
                                                               &EXP_RESULT,
                                                               date,
                                                               calendar,
                                                               CONVENTION[cj]);
                        }

                        LOOP5_ASSERT(ci,
                                     cj,
                                     date,
                                     EXP_RESULT,
                                     result,
                                     result == EXP_RESULT);
                        LOOP5_ASSERT(ci,
                                     cj,
                                     date,
                                     EXP_STATUS,
                                     status,
                                     (0 == status) == (0 == EXP_STATUS));
                    }
                    {
                        bdlt::Date result;
                        int        status = CalendarUtil::shiftIfValid(
                                                  &result,
                                                   date,
                                                   calendar,
                                                   CONVENTION[ci],
                                                   bdlt::DayOfWeek::e_SAT,
                                                   true,
                                                   CONVENTION[cj]);

                        bdlt::Date EXP_RESULT;
                        int        EXP_STATUS;

                        if (date >= startDate && date < endDate) {
                            if (   bdlt::DayOfWeek::e_SAT != date.dayOfWeek()
                                && specialDays.find(date) ==
                                                           specialDays.end()) {
                                EXP_STATUS = CalendarUtil::shiftIfValid(
                                                               &EXP_RESULT,
                                                               date,
                                                               calendar,
                                                               CONVENTION[ci]);
                            }
                            else {
                                EXP_STATUS = CalendarUtil::shiftIfValid(
                                                               &EXP_RESULT,
                                                               date,
                                                               calendar,
                                                               CONVENTION[cj]);
                            }
                        }
                        else {
                            EXP_STATUS = 1;
                        }

                        LOOP5_ASSERT(ci,
                                     cj,
                                     date,
                                     EXP_RESULT,
                                     result,
                                     result == EXP_RESULT);
                        LOOP5_ASSERT(ci,
                                     cj,
                                     date,
                                     EXP_STATUS,
                                     status,
                                     (0 == status) == (0 == EXP_STATUS));
                    }
                }
            }
        }

        // negative tests
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Calendar cdr;
            bdlt::Date     date;
            bdlt::Date     result;

            ASSERT_PASS(Util::shiftIfValid(&result,
                                           date,
                                           cdr,
                                           Util::e_FOLLOWING));
            ASSERT_FAIL(Util::shiftIfValid(0,
                                           date,
                                           cdr,
                                           Util::e_FOLLOWING));

            ASSERT_PASS(Util::shiftIfValid(&result,
                                           date,
                                           cdr,
                                           Util::e_FOLLOWING,
                                           bdlt::DayOfWeek::e_FRI,
                                           true,
                                           Util::e_PRECEDING));
            ASSERT_FAIL(Util::shiftIfValid(0,
                                           date,
                                           cdr,
                                           Util::e_FOLLOWING,
                                           bdlt::DayOfWeek::e_FRI,
                                           true,
                                           Util::e_PRECEDING));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        //  TESTING 'shiftPrecedingIfValid'
        //    Ensure 'shiftPrecedingIfValid' loads proper business day or
        //    returns appropriate status in case of failure.
        //
        // Concerns:
        //: 1 This method returns status with value '1' if 'original' date is
        //:   out of range of a specified calendar, and returns status with
        //:   value '2' if while looking for a business day withing the month,
        //:   boundaries of the valid range of a 'calendar' were exceeded.
        //:
        //: 2 If 'original' date itself is a business day inside of the valid
        //:   range, this method loads it to the 'result' and returns zero.
        //:
        //: 3 If 'original' date is not a business day, this methods loads
        //:   the date of the chronologically latest business day preceding
        //:   'original' date within the valid range of a 'calendar'.
        //:
        //: 4 If non-zero value is returned, result remains unchanged.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the table-driven approach, define a representative set of
        //:   valid inputs.  Verify that the function returns the correct
        //:   value.  (C-1..4)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values.  (C-5)
        //
        // Testing:
        //  shiftPrecedingIfValid(bdlt::Date *result, orig, calendar)
        // --------------------------------------------------------------------

        bslma::TestAllocator         defaultAllocator;
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        if (verbose) cout << endl
                          << "TESTING 'shiftPrecedingIfValid'" << endl
                          << "===============================" << endl;

        static const struct {
            int         d_line;      // source line
            const char *d_input_p;   // input values
            int         d_original;  // original date
            int         d_status;    // expected status
            int         d_result;    // expected result
        } DATA[] = {
            // All days represented as an offset from December 31, 1999.

            //Depth: 0
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, ".|...............................|.",  0,       1,      NV},
            {L_, ".|...............................|.",  1,       1,      NV},
            {L_, ".|...............................|.",  2,       1,      NV},

            //Depth: 1
            //Calendar with 1 NON business day.
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, ".|n..............................|.",   0,      1,      NV},
            {L_, ".|n..............................|.",   1,      2,      NV},
            {L_, ".|n..............................|.",   2,      1,      NV},
            {L_, ".|.n.............................|.",   1,      1,      NV},
            {L_, ".|.n.............................|.",   2,      2,      NV},
            {L_, ".|.n.............................|.",   3,      1,      NV},
            {L_, ".|..............n................|.",  14,      1,      NV},
            {L_, ".|..............n................|.",  15,      2,      NV},
            {L_, ".|..............n................|.",  16,      1,      NV},
            {L_, ".|.............................n.|.",  29,      1,      NV},
            {L_, ".|.............................n.|.",  30,      2,      NV},
            {L_, ".|.............................n.|.",  31,      1,      NV},
            {L_, ".|..............................n|.",  30,      1,      NV},
            {L_, ".|..............................n|.",  31,      2,      NV},
            {L_, ".|..............................n|.",  32,      1,      NV},

            //Calendar with 1 business day.
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, ".|B..............................|.",   0,      1,     NV },
            {L_, ".|B..............................|.",   1,      0,      1 },
            {L_, ".|B..............................|.",   2,      1,     NV },
            {L_, ".|.B.............................|.",   1,      1,     NV },
            {L_, ".|.B.............................|.",   2,      0,      2 },
            {L_, ".|.B.............................|.",   3,      1,     NV },
            {L_, ".|..............B................|.",  14,      1,     NV },
            {L_, ".|..............B................|.",  15,      0,     15 },
            {L_, ".|..............B................|.",  16,      1,     NV },
            {L_, ".|.............................B.|.",  29,      1,     NV },
            {L_, ".|.............................B.|.",  30,      0,     30 },
            {L_, ".|.............................B.|.",  31,      1,     NV },
            {L_, ".|..............................B|.",  30,      1,     NV },
            {L_, ".|..............................B|.",  31,      0,     31 },
            {L_, ".|..............................B|.",  32,      1,     NV },

            //Depth: 2
            // Calendar with 2 NON business days.
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "n|n..............................|.",  -1,       1,     NV},
            {L_, "n|n..............................|.",   0,       2,     NV},
            {L_, "n|n..............................|.",   1,       2,     NV},
            {L_, "n|n..............................|.",   2,       1,     NV},
            {L_, ".|nn.............................|.",   0,       1,     NV},
            {L_, ".|nn.............................|.",   1,       2,     NV},
            {L_, ".|nn.............................|.",   2,       2,     NV},
            {L_, ".|nn.............................|.",   3,       1,     NV},
            {L_, ".|.nn............................|.",   1,       1,     NV},
            {L_, ".|.nn............................|.",   2,       2,     NV},
            {L_, ".|.nn............................|.",   3,       2,     NV},
            {L_, ".|.nn............................|.",   4,       1,     NV},
            {L_, ".|.............nn................|.",  13,       1,     NV},
            {L_, ".|.............nn................|.",  14,       2,     NV},
            {L_, ".|.............nn................|.",  15,       2,     NV},
            {L_, ".|.............nn................|.",  16,       1,     NV},
            {L_, ".|............................nn.|.",  28,       1,     NV},
            {L_, ".|............................nn.|.",  29,       2,     NV},
            {L_, ".|............................nn.|.",  30,       2,     NV},
            {L_, ".|............................nn.|.",  31,       1,     NV},
            {L_, ".|.............................nn|.",  29,       1,     NV},
            {L_, ".|.............................nn|.",  30,       2,     NV},
            {L_, ".|.............................nn|.",  31,       2,     NV},
            {L_, ".|.............................nn|.",  32,       1,     NV},
            {L_, ".|..............................n|n",  30,       1,     NV},
            {L_, ".|..............................n|n",  31,       2,     NV},
            {L_, ".|..............................n|n",  32,       2,     NV},
            {L_, ".|..............................n|n",  33,       1,     NV},

            //Calendar with 1 NON Business day, 1 Business day
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "n|B..............................|.",  -1,       1,     NV},
            {L_, "n|B..............................|.",   0,       2,     NV},
            {L_, "n|B..............................|.",   1,       0,      1},
            {L_, "n|B..............................|.",   2,       1,     NV},
            {L_, ".|nB.............................|.",   0,       1,     NV},
            {L_, ".|nB.............................|.",   1,       2,     NV},
            {L_, ".|nB.............................|.",   2,       0,      2},
            {L_, ".|nB.............................|.",   3,       1,     NV},
            {L_, ".|.nB............................|.",   1,       1,     NV},
            {L_, ".|.nB............................|.",   2,       2,     NV},
            {L_, ".|.nB............................|.",   3,       0,      3},
            {L_, ".|.nB............................|.",   4,       1,     NV},
            {L_, ".|.............nB................|.",  13,       1,     NV},
            {L_, ".|.............nB................|.",  14,       2,     NV},
            {L_, ".|.............nB................|.",  15,       0,     15},
            {L_, ".|.............nB................|.",  16,       1,     NV},
            {L_, ".|............................nB.|.",  28,       1,     NV},
            {L_, ".|............................nB.|.",  29,       2,     NV},
            {L_, ".|............................nB.|.",  30,       0,     30},
            {L_, ".|............................nB.|.",  31,       1,     NV},
            {L_, ".|.............................nB|.",  29,       1,     NV},
            {L_, ".|.............................nB|.",  30,       2,     NV},
            {L_, ".|.............................nB|.",  31,       0,     31},
            {L_, ".|.............................nB|.",  32,       1,     NV},
            {L_, ".|..............................n|B",  30,       1,     NV},
            {L_, ".|..............................n|B",  31,       2,     NV},
            {L_, ".|..............................n|B",  32,       0,     32},
            {L_, ".|..............................n|B",  33,       1,     NV},

            //Calendar with 1 Business day, 1 NON Business day
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "B|n..............................|.",  -1,       1,     NV},
            {L_, "B|n..............................|.",   0,       0,      0},
            {L_, "B|n..............................|.",   1,       0,      0},
            {L_, "B|n..............................|.",   2,       1,     NV},
            {L_, ".|Bn.............................|.",   0,       1,     NV},
            {L_, ".|Bn.............................|.",   1,       0,      1},
            {L_, ".|Bn.............................|.",   2,       0,      1},
            {L_, ".|Bn.............................|.",   3,       1,     NV},
            {L_, ".|.Bn............................|.",   1,       1,     NV},
            {L_, ".|.Bn............................|.",   2,       0,      2},
            {L_, ".|.Bn............................|.",   3,       0,      2},
            {L_, ".|.Bn............................|.",   4,       1,     NV},
            {L_, ".|.............Bn................|.",  13,       1,     NV},
            {L_, ".|.............Bn................|.",  14,       0,     14},
            {L_, ".|.............Bn................|.",  15,       0,     14},
            {L_, ".|.............Bn................|.",  16,       1,     NV},
            {L_, ".|............................Bn.|.",  28,       1,     NV},
            {L_, ".|............................Bn.|.",  29,       0,     29},
            {L_, ".|............................Bn.|.",  30,       0,     29},
            {L_, ".|............................Bn.|.",  31,       1,     NV},
            {L_, ".|.............................Bn|.",  29,       1,     NV},
            {L_, ".|.............................Bn|.",  30,       0,     30},
            {L_, ".|.............................Bn|.",  31,       0,     30},
            {L_, ".|.............................Bn|.",  32,       1,     NV},
            {L_, ".|..............................B|n",  30,       1,     NV},
            {L_, ".|..............................B|n",  31,       0,     31},
            {L_, ".|..............................B|n",  32,       0,     31},
            {L_, ".|..............................B|n",  33,       1,     NV},

            //Calendar with 2 Business Days
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "B|B..............................|.",  -1,       1,     NV},
            {L_, "B|B..............................|.",   0,       0,      0},
            {L_, "B|B..............................|.",   1,       0,      1},
            {L_, "B|B..............................|.",   2,       1,     NV},
            {L_, ".|BB.............................|.",   0,       1,     NV},
            {L_, ".|BB.............................|.",   1,       0,      1},
            {L_, ".|BB.............................|.",   2,       0,      2},
            {L_, ".|BB.............................|.",   3,       1,     NV},
            {L_, ".|.BB............................|.",   1,       1,     NV},
            {L_, ".|.BB............................|.",   2,       0,      2},
            {L_, ".|.BB............................|.",   3,       0,      3},
            {L_, ".|.BB............................|.",   4,       1,     NV},
            {L_, ".|.............BB................|.",  13,       1,     NV},
            {L_, ".|.............BB................|.",  14,       0,     14},
            {L_, ".|.............BB................|.",  15,       0,     15},
            {L_, ".|.............BB................|.",  16,       1,     NV},
            {L_, ".|............................BB.|.",  28,       1,     NV},
            {L_, ".|............................BB.|.",  29,       0,     29},
            {L_, ".|............................BB.|.",  30,       0,     30},
            {L_, ".|............................BB.|.",  31,       1,     NV},
            {L_, ".|.............................BB|.",  29,       1,     NV},
            {L_, ".|.............................BB|.",  30,       0,     30},
            {L_, ".|.............................BB|.",  31,       0,     31},
            {L_, ".|.............................BB|.",  32,       1,     NV},
            {L_, ".|..............................B|B",  30,       1,     NV},
            {L_, ".|..............................B|B",  31,       0,     31},
            {L_, ".|..............................B|B",  32,       0,     32},
            {L_, ".|..............................B|B",  33,       1,     NV},

            //Depth: 3
            //"NNN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|n..............................|..",  -2,     1,     NV},
            {L_, "nn|n..............................|..",  -1,     2,     NV},
            {L_, "nn|n..............................|..",   0,     2,     NV},
            {L_, "nn|n..............................|..",   1,     2,     NV},
            {L_, "nn|n..............................|..",   2,     1,     NV},
            {L_, ".n|nn.............................|..",  -1,     1,     NV},
            {L_, ".n|nn.............................|..",   0,     2,     NV},
            {L_, ".n|nn.............................|..",   1,     2,     NV},
            {L_, ".n|nn.............................|..",   2,     2,     NV},
            {L_, ".n|nn.............................|..",   3,     1,     NV},
            {L_, "..|nnn............................|..",   0,     1,     NV},
            {L_, "..|nnn............................|..",   1,     2,     NV},
            {L_, "..|nnn............................|..",   2,     2,     NV},
            {L_, "..|nnn............................|..",   3,     2,     NV},
            {L_, "..|nnn............................|..",   4,     1,     NV},
            {L_, "..|.nnn...........................|..",   1,     1,     NV},
            {L_, "..|.nnn...........................|..",   2,     2,     NV},
            {L_, "..|.nnn...........................|..",   3,     2,     NV},
            {L_, "..|.nnn...........................|..",   4,     2,     NV},
            {L_, "..|.nnn...........................|..",   5,     1,     NV},
            {L_, "..|.............nnn...............|..",  13,     1,     NV},
            {L_, "..|.............nnn...............|..",  14,     2,     NV},
            {L_, "..|.............nnn...............|..",  15,     2,     NV},
            {L_, "..|.............nnn...............|..",  16,     2,     NV},
            {L_, "..|.............nnn...............|..",  17,     1,     NV},
            {L_, "..|...........................nnn.|..",  27,     1,     NV},
            {L_, "..|...........................nnn.|..",  28,     2,     NV},
            {L_, "..|...........................nnn.|..",  29,     2,     NV},
            {L_, "..|...........................nnn.|..",  30,     2,     NV},
            {L_, "..|...........................nnn.|..",  31,     1,     NV},
            {L_, "..|............................nnn|..",  28,     1,     NV},
            {L_, "..|............................nnn|..",  29,     2,     NV},
            {L_, "..|............................nnn|..",  30,     2,     NV},
            {L_, "..|............................nnn|..",  31,     2,     NV},
            {L_, "..|............................nnn|..",  32,     1,     NV},
            {L_, "..|.............................nn|n.",  29,     1,     NV},
            {L_, "..|.............................nn|n.",  30,     2,     NV},
            {L_, "..|.............................nn|n.",  31,     2,     NV},
            {L_, "..|.............................nn|n.",  32,     2,     NV},
            {L_, "..|.............................nn|n.",  33,     1,     NV},
            {L_, "..|..............................n|nn",  30,     1,     NV},
            {L_, "..|..............................n|nn",  31,     2,     NV},
            {L_, "..|..............................n|nn",  32,     2,     NV},
            {L_, "..|..............................n|nn",  33,     2,     NV},
            {L_, "..|..............................n|nn",  34,     1,     NV},

            //"NNB".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|B..............................|..",  -2,     1,     NV},
            {L_, "nn|B..............................|..",  -1,     2,     NV},
            {L_, "nn|B..............................|..",   0,     2,     NV},
            {L_, "nn|B..............................|..",   1,     0,      1},
            {L_, "nn|B..............................|..",   2,     1,     NV},
            {L_, ".n|nB.............................|..",  -1,     1,     NV},
            {L_, ".n|nB.............................|..",   0,     2,     NV},
            {L_, ".n|nB.............................|..",   1,     2,     NV},
            {L_, ".n|nB.............................|..",   2,     0,      2},
            {L_, ".n|nB.............................|..",   3,     1,     NV},
            {L_, "..|nnB............................|..",   0,     1,     NV},
            {L_, "..|nnB............................|..",   1,     2,     NV},
            {L_, "..|nnB............................|..",   2,     2,     NV},
            {L_, "..|nnB............................|..",   3,     0,      3},
            {L_, "..|nnB............................|..",   4,     1,     NV},
            {L_, "..|.nnB...........................|..",   1,     1,     NV},
            {L_, "..|.nnB...........................|..",   2,     2,     NV},
            {L_, "..|.nnB...........................|..",   3,     2,     NV},
            {L_, "..|.nnB...........................|..",   4,     0,      4},
            {L_, "..|.nnB...........................|..",   5,     1,     NV},
            {L_, "..|.............nnB...............|..",  13,     1,     NV},
            {L_, "..|.............nnB...............|..",  14,     2,     NV},
            {L_, "..|.............nnB...............|..",  15,     2,     NV},
            {L_, "..|.............nnB...............|..",  16,     0,     16},
            {L_, "..|.............nnB...............|..",  17,     1,     NV},
            {L_, "..|...........................nnB.|..",  27,     1,     NV},
            {L_, "..|...........................nnB.|..",  28,     2,     NV},
            {L_, "..|...........................nnB.|..",  29,     2,     NV},
            {L_, "..|...........................nnB.|..",  30,     0,     30},
            {L_, "..|...........................nnB.|..",  31,     1,     NV},
            {L_, "..|............................nnB|..",  28,     1,     NV},
            {L_, "..|............................nnB|..",  29,     2,     NV},
            {L_, "..|............................nnB|..",  30,     2,     NV},
            {L_, "..|............................nnB|..",  31,     0,     31},
            {L_, "..|............................nnB|..",  32,     1,     NV},
            {L_, "..|.............................nn|B.",  29,     1,     NV},
            {L_, "..|.............................nn|B.",  30,     2,     NV},
            {L_, "..|.............................nn|B.",  31,     2,     NV},
            {L_, "..|.............................nn|B.",  32,     0,     32},
            {L_, "..|.............................nn|B.",  33,     1,     NV},
            {L_, "..|..............................n|nB",  30,     1,     NV},
            {L_, "..|..............................n|nB",  31,     2,     NV},
            {L_, "..|..............................n|nB",  32,     2,     NV},
            {L_, "..|..............................n|nB",  33,     0,     33},
            {L_, "..|..............................n|nB",  34,     1,     NV},

            //"NBN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nB|n..............................|..",  -2,     1,     NV},
            {L_, "nB|n..............................|..",  -1,     2,     NV},
            {L_, "nB|n..............................|..",   0,     0,      0},
            {L_, "nB|n..............................|..",   1,     0,      0},
            {L_, "nB|n..............................|..",   2,     1,     NV},
            {L_, ".n|Bn.............................|..",  -1,     1,     NV},
            {L_, ".n|Bn.............................|..",   0,     2,     NV},
            {L_, ".n|Bn.............................|..",   1,     0,      1},
            {L_, ".n|Bn.............................|..",   2,     0,      1},
            {L_, ".n|Bn.............................|..",   3,     1,     NV},
            {L_, "..|nBn............................|..",   0,     1,     NV},
            {L_, "..|nBn............................|..",   1,     2,     NV},
            {L_, "..|nBn............................|..",   2,     0,      2},
            {L_, "..|nBn............................|..",   3,     0,      2},
            {L_, "..|nBn............................|..",   4,     1,     NV},
            {L_, "..|.nBn...........................|..",   1,     1,     NV},
            {L_, "..|.nBn...........................|..",   2,     2,     NV},
            {L_, "..|.nBn...........................|..",   3,     0,      3},
            {L_, "..|.nBn...........................|..",   4,     0,      3},
            {L_, "..|.nBn...........................|..",   5,     1,     NV},
            {L_, "..|.............nBn...............|..",  13,     1,     NV},
            {L_, "..|.............nBn...............|..",  14,     2,     NV},
            {L_, "..|.............nBn...............|..",  15,     0,     15},
            {L_, "..|.............nBn...............|..",  16,     0,     15},
            {L_, "..|.............nBn...............|..",  17,     1,     NV},
            {L_, "..|...........................nBn.|..",  27,     1,     NV},
            {L_, "..|...........................nBn.|..",  28,     2,     NV},
            {L_, "..|...........................nBn.|..",  29,     0,     29},
            {L_, "..|...........................nBn.|..",  30,     0,     29},
            {L_, "..|...........................nBn.|..",  31,     1,     NV},
            {L_, "..|............................nBn|..",  28,     1,     NV},
            {L_, "..|............................nBn|..",  29,     2,     NV},
            {L_, "..|............................nBn|..",  30,     0,     30},
            {L_, "..|............................nBn|..",  31,     0,     30},
            {L_, "..|............................nBn|..",  32,     1,     NV},
            {L_, "..|.............................nB|n.",  29,     1,     NV},
            {L_, "..|.............................nB|n.",  30,     2,     NV},
            {L_, "..|.............................nB|n.",  31,     0,     31},
            {L_, "..|.............................nB|n.",  32,     0,     31},
            {L_, "..|.............................nB|n.",  33,     1,     NV},
            {L_, "..|..............................n|Bn",  30,     1,     NV},
            {L_, "..|..............................n|Bn",  31,     2,     NV},
            {L_, "..|..............................n|Bn",  32,     0,     32},
            {L_, "..|..............................n|Bn",  33,     0,     32},
            {L_, "..|..............................n|Bn",  34,     1,     NV},

            //"NBB".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nB|B..............................|..",  -2,     1,     NV},
            {L_, "nB|B..............................|..",  -1,     2,     NV},
            {L_, "nB|B..............................|..",   0,     0,      0},
            {L_, "nB|B..............................|..",   1,     0,      1},
            {L_, "nB|B..............................|..",   2,     1,     NV},
            {L_, ".n|BB.............................|..",  -1,     1,     NV},
            {L_, ".n|BB.............................|..",   0,     2,     NV},
            {L_, ".n|BB.............................|..",   1,     0,      1},
            {L_, ".n|BB.............................|..",   2,     0,      2},
            {L_, ".n|BB.............................|..",   3,     1,     NV},
            {L_, "..|nBB............................|..",   0,     1,     NV},
            {L_, "..|nBB............................|..",   1,     2,     NV},
            {L_, "..|nBB............................|..",   2,     0,      2},
            {L_, "..|nBB............................|..",   3,     0,      3},
            {L_, "..|nBB............................|..",   4,     1,     NV},
            {L_, "..|.nBB...........................|..",   1,     1,     NV},
            {L_, "..|.nBB...........................|..",   2,     2,     NV},
            {L_, "..|.nBB...........................|..",   3,     0,      3},
            {L_, "..|.nBB...........................|..",   4,     0,      4},
            {L_, "..|.nBB...........................|..",   5,     1,     NV},
            {L_, "..|.............nBB...............|..",  13,     1,     NV},
            {L_, "..|.............nBB...............|..",  14,     2,     NV},
            {L_, "..|.............nBB...............|..",  15,     0,     15},
            {L_, "..|.............nBB...............|..",  16,     0,     16},
            {L_, "..|.............nBB...............|..",  17,     1,     NV},
            {L_, "..|...........................nBB.|..",  27,     1,     NV},
            {L_, "..|...........................nBB.|..",  28,     2,     NV},
            {L_, "..|...........................nBB.|..",  29,     0,     29},
            {L_, "..|...........................nBB.|..",  30,     0,     30},
            {L_, "..|...........................nBB.|..",  31,     1,     NV},
            {L_, "..|............................nBB|..",  28,     1,     NV},
            {L_, "..|............................nBB|..",  29,     2,     NV},
            {L_, "..|............................nBB|..",  30,     0,     30},
            {L_, "..|............................nBB|..",  31,     0,     31},
            {L_, "..|............................nBB|..",  32,     1,     NV},
            {L_, "..|.............................nB|B.",  29,     1,     NV},
            {L_, "..|.............................nB|B.",  30,     2,     NV},
            {L_, "..|.............................nB|B.",  31,     0,     31},
            {L_, "..|.............................nB|B.",  32,     0,     32},
            {L_, "..|.............................nB|B.",  33,     1,     NV},
            {L_, "..|..............................n|BB",  30,     1,     NV},

            //"BNN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "Bn|n..............................|..",  -2,     1,     NV},
            {L_, "Bn|n..............................|..",  -1,     0,     -1},
            {L_, "Bn|n..............................|..",   0,     0,     -1},
            {L_, "Bn|n..............................|..",   1,     0,     -1},
            {L_, "Bn|n..............................|..",   2,     1,     NV},
            {L_, ".B|nn.............................|..",  -1,     1,     NV},
            {L_, ".B|nn.............................|..",   0,     0,      0},
            {L_, ".B|nn.............................|..",   1,     0,      0},
            {L_, ".B|nn.............................|..",   2,     0,      0},
            {L_, ".B|nn.............................|..",   3,     1,     NV},
            {L_, "..|Bnn............................|..",   0,     1,     NV},
            {L_, "..|Bnn............................|..",   1,     0,      1},
            {L_, "..|Bnn............................|..",   2,     0,      1},
            {L_, "..|Bnn............................|..",   3,     0,      1},
            {L_, "..|Bnn............................|..",   4,     1,     NV},
            {L_, "..|.Bnn...........................|..",   1,     1,     NV},
            {L_, "..|.Bnn...........................|..",   2,     0,      2},
            {L_, "..|.Bnn...........................|..",   3,     0,      2},
            {L_, "..|.Bnn...........................|..",   4,     0,      2},
            {L_, "..|.Bnn...........................|..",   5,     1,     NV},
            {L_, "..|.............Bnn...............|..",  13,     1,     NV},
            {L_, "..|.............Bnn...............|..",  14,     0,     14},
            {L_, "..|.............Bnn...............|..",  15,     0,     14},
            {L_, "..|.............Bnn...............|..",  16,     0,     14},
            {L_, "..|.............Bnn...............|..",  17,     1,     NV},
            {L_, "..|...........................Bnn.|..",  27,     1,     NV},
            {L_, "..|...........................Bnn.|..",  28,     0,     28},
            {L_, "..|...........................Bnn.|..",  29,     0,     28},
            {L_, "..|...........................Bnn.|..",  30,     0,     28},
            {L_, "..|...........................Bnn.|..",  31,     1,     NV},
            {L_, "..|............................Bnn|..",  28,     1,     NV},
            {L_, "..|............................Bnn|..",  29,     0,     29},
            {L_, "..|............................Bnn|..",  30,     0,     29},
            {L_, "..|............................Bnn|..",  31,     0,     29},
            {L_, "..|............................Bnn|..",  32,     1,     NV},
            {L_, "..|.............................Bn|n.",  29,     1,     NV},
            {L_, "..|.............................Bn|n.",  30,     0,     30},
            {L_, "..|.............................Bn|n.",  31,     0,     30},
            {L_, "..|.............................Bn|n.",  32,     0,     30},
            {L_, "..|.............................Bn|n.",  33,     1,     NV},
            {L_, "..|..............................B|nn",  30,     1,     NV},
            {L_, "..|..............................B|nn",  31,     0,     31},
            {L_, "..|..............................B|nn",  32,     0,     31},
            {L_, "..|..............................B|nn",  33,     0,     31},
            {L_, "..|..............................B|nn",  34,     1,     NV},

            //"BNB".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "Bn|B..............................|..",  -2,     1,     NV},
            {L_, "Bn|B..............................|..",  -1,     0,     -1},
            {L_, "Bn|B..............................|..",   0,     0,     -1},
            {L_, "Bn|B..............................|..",   1,     0,      1},
            {L_, "Bn|B..............................|..",   2,     1,     NV},
            {L_, ".B|nB.............................|..",  -1,     1,     NV},
            {L_, ".B|nB.............................|..",   0,     0,      0},
            {L_, ".B|nB.............................|..",   1,     0,      0},
            {L_, ".B|nB.............................|..",   2,     0,      2},
            {L_, ".B|nB.............................|..",   3,     1,     NV},
            {L_, "..|BnB............................|..",   0,     1,     NV},
            {L_, "..|BnB............................|..",   1,     0,      1},
            {L_, "..|BnB............................|..",   2,     0,      1},
            {L_, "..|BnB............................|..",   3,     0,      3},
            {L_, "..|BnB............................|..",   4,     1,     NV},
            {L_, "..|.BnB...........................|..",   1,     1,     NV},
            {L_, "..|.BnB...........................|..",   2,     0,      2},
            {L_, "..|.BnB...........................|..",   3,     0,      2},
            {L_, "..|.BnB...........................|..",   4,     0,      4},
            {L_, "..|.BnB...........................|..",   5,     1,     NV},
            {L_, "..|.............BnB...............|..",  13,     1,     NV},
            {L_, "..|.............BnB...............|..",  14,     0,     14},
            {L_, "..|.............BnB...............|..",  15,     0,     14},
            {L_, "..|.............BnB...............|..",  16,     0,     16},
            {L_, "..|.............BnB...............|..",  17,     1,     NV},
            {L_, "..|...........................BnB.|..",  27,     1,     NV},
            {L_, "..|...........................BnB.|..",  28,     0,     28},
            {L_, "..|...........................BnB.|..",  29,     0,     28},
            {L_, "..|...........................BnB.|..",  30,     0,     30},
            {L_, "..|...........................BnB.|..",  31,     1,     NV},
            {L_, "..|............................BnB|..",  28,     1,     NV},
            {L_, "..|............................BnB|..",  29,     0,     29},
            {L_, "..|............................BnB|..",  30,     0,     29},
            {L_, "..|............................BnB|..",  31,     0,     31},
            {L_, "..|............................BnB|..",  32,     1,     NV},
            {L_, "..|.............................Bn|B.",  29,     1,     NV},
            {L_, "..|.............................Bn|B.",  30,     0,     30},
            {L_, "..|.............................Bn|B.",  31,     0,     30},
            {L_, "..|.............................Bn|B.",  32,     0,     32},
            {L_, "..|.............................Bn|B.",  33,     1,     NV},
            {L_, "..|..............................B|nB",  30,     1,     NV},
            {L_, "..|..............................B|nB",  31,     0,     31},
            {L_, "..|..............................B|nB",  32,     0,     31},
            {L_, "..|..............................B|nB",  33,     0,     33},
            {L_, "..|..............................B|nB",  34,     1,     NV},

            //"BBN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "BB|n..............................|..",  -2,     1,     NV},
            {L_, "BB|n..............................|..",  -1,     0,     -1},
            {L_, "BB|n..............................|..",   0,     0,      0},
            {L_, "BB|n..............................|..",   1,     0,      0},
            {L_, "BB|n..............................|..",   2,     1,     NV},
            {L_, ".B|Bn.............................|..",  -1,     1,     NV},
            {L_, ".B|Bn.............................|..",   0,     0,      0},
            {L_, ".B|Bn.............................|..",   1,     0,      1},
            {L_, ".B|Bn.............................|..",   2,     0,      1},
            {L_, ".B|Bn.............................|..",   3,     1,     NV},
            {L_, "..|BBn............................|..",   0,     1,     NV},
            {L_, "..|BBn............................|..",   1,     0,      1},
            {L_, "..|BBn............................|..",   2,     0,      2},
            {L_, "..|BBn............................|..",   3,     0,      2},
            {L_, "..|BBn............................|..",   4,     1,     NV},
            {L_, "..|.BBn...........................|..",   1,     1,     NV},
            {L_, "..|.BBn...........................|..",   2,     0,      2},
            {L_, "..|.BBn...........................|..",   3,     0,      3},
            {L_, "..|.BBn...........................|..",   4,     0,      3},
            {L_, "..|.BBn...........................|..",   5,     1,     NV},
            {L_, "..|.............BBn...............|..",  13,     1,     NV},
            {L_, "..|.............BBn...............|..",  14,     0,     14},
            {L_, "..|.............BBn...............|..",  15,     0,     15},
            {L_, "..|.............BBn...............|..",  16,     0,     15},
            {L_, "..|.............BBn...............|..",  17,     1,     NV},
            {L_, "..|...........................BBn.|..",  27,     1,     NV},
            {L_, "..|...........................BBn.|..",  28,     0,     28},
            {L_, "..|...........................BBn.|..",  29,     0,     29},
            {L_, "..|...........................BBn.|..",  30,     0,     29},
            {L_, "..|...........................BBn.|..",  31,     1,     NV},
            {L_, "..|............................BBn|..",  28,     1,     NV},
            {L_, "..|............................BBn|..",  29,     0,     29},
            {L_, "..|............................BBn|..",  30,     0,     30},
            {L_, "..|............................BBn|..",  31,     0,     30},
            {L_, "..|............................BBn|..",  32,     1,     NV},
            {L_, "..|.............................BB|n.",  29,     1,     NV},
            {L_, "..|.............................BB|n.",  30,     0,     30},
            {L_, "..|.............................BB|n.",  31,     0,     31},
            {L_, "..|.............................BB|n.",  32,     0,     31},
            {L_, "..|.............................BB|n.",  33,     1,     NV},
            {L_, "..|..............................B|Bn",  30,     1,     NV},
            {L_, "..|..............................B|Bn",  31,     0,     31},
            {L_, "..|..............................B|Bn",  32,     0,     32},
            {L_, "..|..............................B|Bn",  33,     0,     32},
            {L_, "..|..............................B|Bn",  34,     1,     NV},

            //"BBB".
            //LINE   INPUT                            ORIGINAL  STATUS  RESULT
            //----   -----                            --------  ------  ------
            {L_, "BB|B..............................|..",  -2,     1,     NV},
            {L_, "BB|B..............................|..",  -1,     0,     -1},
            {L_, "BB|B..............................|..",   0,     0,      0},
            {L_, "BB|B..............................|..",   1,     0,      1},
            {L_, "BB|B..............................|..",   2,     1,     NV},
            {L_, ".B|BB.............................|..",  -1,     1,     NV},
            {L_, ".B|BB.............................|..",   0,     0,      0},
            {L_, ".B|BB.............................|..",   1,     0,      1},
            {L_, ".B|BB.............................|..",   2,     0,      2},
            {L_, ".B|BB.............................|..",   3,     1,     NV},
            {L_, "..|BBB............................|..",   0,     1,     NV},
            {L_, "..|BBB............................|..",   1,     0,      1},
            {L_, "..|BBB............................|..",   2,     0,      2},
            {L_, "..|BBB............................|..",   3,     0,      3},
            {L_, "..|BBB............................|..",   4,     1,     NV},
            {L_, "..|.BBB...........................|..",   1,     1,     NV},
            {L_, "..|.BBB...........................|..",   2,     0,      2},
            {L_, "..|.BBB...........................|..",   3,     0,      3},
            {L_, "..|.BBB...........................|..",   4,     0,      4},
            {L_, "..|.BBB...........................|..",   5,     1,     NV},
            {L_, "..|.............BBB...............|..",  13,     1,     NV},
            {L_, "..|.............BBB...............|..",  14,     0,     14},
            {L_, "..|.............BBB...............|..",  15,     0,     15},
            {L_, "..|.............BBB...............|..",  16,     0,     16},
            {L_, "..|.............BBB...............|..",  17,     1,     NV},
            {L_, "..|...........................BBB.|..",  27,     1,     NV},
            {L_, "..|...........................BBB.|..",  28,     0,     28},
            {L_, "..|...........................BBB.|..",  29,     0,     29},
            {L_, "..|...........................BBB.|..",  30,     0,     30},
            {L_, "..|...........................BBB.|..",  31,     1,     NV},
            {L_, "..|............................BBB|..",  28,     1,     NV},
            {L_, "..|............................BBB|..",  29,     0,     29},
            {L_, "..|............................BBB|..",  30,     0,     30},
            {L_, "..|............................BBB|..",  31,     0,     31},
            {L_, "..|............................BBB|..",  32,     1,     NV},
            {L_, "..|.............................BB|B.",  29,     1,     NV},
            {L_, "..|.............................BB|B.",  30,     0,     30},
            {L_, "..|.............................BB|B.",  31,     0,     31},
            {L_, "..|.............................BB|B.",  32,     0,     32},
            {L_, "..|.............................BB|B.",  33,     1,     NV},
            {L_, "..|..............................B|BB",  30,     1,     NV},
            {L_, "..|..............................B|BB",  31,     0,     31},
            {L_, "..|..............................B|BB",  32,     0,     32},
            {L_, "..|..............................B|BB",  33,     0,     33},
            {L_, "..|..............................B|BB",  34,     1,     NV},

            //Calendar that covers whole month (Calendar.length() == 35)
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",  15,     0,     15},
            {L_, "BB|BBBBBBBBBBBBBBnBBBBBBBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnBBBBBBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnBBBBBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnBBBBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnBBBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnBBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnn|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnn|nB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnn|nn",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBnnnnnnnnnnnnnnnnnn|nn",  15,     0,     13},
            {L_, "BB|BBBBBBBBBBBBnnnnnnnnnnnnnnnnnnn|nn",  15,     0,     12},
            {L_, "BB|BBBBBBBBBBBnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,     11},
            {L_, "BB|BBBBBBBBBBnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,     10},
            {L_, "BB|BBBBBBBBBnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      9},
            {L_, "BB|BBBBBBBBnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      8},
            {L_, "BB|BBBBBBBnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      7},
            {L_, "BB|BBBBBBnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      6},
            {L_, "BB|BBBBBnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      5},
            {L_, "BB|BBBBnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      4},
            {L_, "BB|BBBnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      3},
            {L_, "BB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "BB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      1},
            {L_, "BB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      0},
            {L_, "Bn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,     -1},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},

            //Corner cases
            //Origin: 15
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,      2},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,      2},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,      2},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,      2},

            //Origin: 32, it first date of the next month
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},

            //Origin 31
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},

            //Origin 30
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},

            //Origin 2
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},

            //Origin 1
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      0},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},

            //Origin  0, i.e last day of the previous m
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0}
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        const bdlt::Date ORIGIN = bdlt::Date(1999, 12, 31);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE     = DATA[i].d_line;
            const char       *INPUT    = DATA[i].d_input_p;
            const bdlt::Date  START    = ORIGIN + getStartDate(INPUT);
            const bdlt::Date  ORIGINAL = ORIGIN + DATA[i].d_original;
            bdlt::Date        LOADED   = ORIGIN - 5;
            const int         STATUS   = DATA[i].d_status;
            const bdlt::Date  RESULT   = ORIGIN + DATA[i].d_result;

            bdlt::Calendar calendar = parseCalendar(INPUT, START);
            int            rStatus = Util::shiftPrecedingIfValid(&LOADED,
                                                                 ORIGINAL,
                                                                 calendar);

            if (veryVerbose) {
                T_ P_(LINE) P_(ORIGINAL) P_(RESULT) P_(LOADED);
            }

            // Check status
            ASSERTV(LINE,
                    INPUT,
                    START,
                    ORIGINAL,
                    STATUS,
                    rStatus,
                    rStatus == STATUS);

            // Check loaded
            ASSERTV(LINE,
                    INPUT,
                    START,
                    ORIGINAL,
                    LOADED,
                    RESULT,
                    RESULT == LOADED);
        }

        // negative tests
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Calendar cdr;
            bdlt::Date     date;
            bdlt::Date     result;

            ASSERT_SAFE_PASS(Util::shiftPrecedingIfValid(&result, date, cdr));
            ASSERT_SAFE_FAIL(Util::shiftPrecedingIfValid(      0, date, cdr));
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        //  TESTING 'shiftFollowingIfValid'
        //    Ensure 'shiftFollowingIfValid' loads proper business day or
        //    returns appropriate status in case of failure.
        //
        // Concerns:
        //: 1 This method returns status with value '1' if 'original' date is
        //:   out of range of a specified calendar, and returns status with
        //:   value '2' if while looking for a business day withing the month,
        //:   boundaries of the valid range of a 'calendar' were exceeded.
        //:
        //: 2 If 'original' date itself is a business day inside of the valid
        //:   range, this method loads it to the 'result' and returns zero.
        //:
        //: 3 If 'original' date is not a business day, this methods loads
        //:   the date of the chronologically earliest business day following
        //:   'original' date within the valid range of a 'calendar'.
        //:
        //: 4 If non-zero value is returned, result remains unchanged.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the table-driven approach, define a representative set of
        //:   valid inputs.  Verify that the function returns the correct
        //:   value.  (C-1..4)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values.  (C-5)
        //
        // Testing:
        //  shiftFollowingIfValid(bdlt::Date *result, orig, calendar)
        // --------------------------------------------------------------------

        bslma::TestAllocator         defaultAllocator;
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        if (verbose) cout << endl
                          << "TESTING 'shiftFollowingIfValid'" << endl
                          << "===============================" << endl;

        static const struct {
            int         d_line;      // source line
            const char *d_input_p;   // input values
            int         d_original;  // original date
            int         d_status;    // expected status
            int         d_result;    // expected result
        } DATA[] = {
            // All days represented as an offset from December 31, 1999.

            //Depth: 0
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, ".|...............................|.",  0,       1,      NV},
            {L_, ".|...............................|.",  1,       1,      NV},
            {L_, ".|...............................|.",  2,       1,      NV},

            //Depth: 1
            //Calendar with 1 NON business day.
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, ".|n..............................|.",   0,      1,      NV},
            {L_, ".|n..............................|.",   1,      2,      NV},
            {L_, ".|n..............................|.",   2,      1,      NV},
            {L_, ".|.n.............................|.",   1,      1,      NV},
            {L_, ".|.n.............................|.",   2,      2,      NV},
            {L_, ".|.n.............................|.",   3,      1,      NV},
            {L_, ".|..............n................|.",  14,      1,      NV},
            {L_, ".|..............n................|.",  15,      2,      NV},
            {L_, ".|..............n................|.",  16,      1,      NV},
            {L_, ".|.............................n.|.",  29,      1,      NV},
            {L_, ".|.............................n.|.",  30,      2,      NV},
            {L_, ".|.............................n.|.",  31,      1,      NV},
            {L_, ".|..............................n|.",  30,      1,      NV},
            {L_, ".|..............................n|.",  31,      2,      NV},
            {L_, ".|..............................n|.",  32,      1,      NV},

            //Calendar with 1 business day.
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, ".|B..............................|.",   0,      1,     NV },
            {L_, ".|B..............................|.",   1,      0,      1 },
            {L_, ".|B..............................|.",   2,      1,     NV },
            {L_, ".|.B.............................|.",   1,      1,     NV },
            {L_, ".|.B.............................|.",   2,      0,      2 },
            {L_, ".|.B.............................|.",   3,      1,     NV },
            {L_, ".|..............B................|.",  14,      1,     NV },
            {L_, ".|..............B................|.",  15,      0,     15 },
            {L_, ".|..............B................|.",  16,      1,     NV },
            {L_, ".|.............................B.|.",  29,      1,     NV },
            {L_, ".|.............................B.|.",  30,      0,     30 },
            {L_, ".|.............................B.|.",  31,      1,     NV },
            {L_, ".|..............................B|.",  30,      1,     NV },
            {L_, ".|..............................B|.",  31,      0,     31 },
            {L_, ".|..............................B|.",  32,      1,     NV },

            //Depth: 2
            // Calendar with 2 NON business days.
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "n|n..............................|.",  -1,       1,     NV},
            {L_, "n|n..............................|.",   0,       2,     NV},
            {L_, "n|n..............................|.",   1,       2,     NV},
            {L_, "n|n..............................|.",   2,       1,     NV},
            {L_, ".|nn.............................|.",   0,       1,     NV},
            {L_, ".|nn.............................|.",   1,       2,     NV},
            {L_, ".|nn.............................|.",   2,       2,     NV},
            {L_, ".|nn.............................|.",   3,       1,     NV},
            {L_, ".|.nn............................|.",   1,       1,     NV},
            {L_, ".|.nn............................|.",   2,       2,     NV},
            {L_, ".|.nn............................|.",   3,       2,     NV},
            {L_, ".|.nn............................|.",   4,       1,     NV},
            {L_, ".|.............nn................|.",  13,       1,     NV},
            {L_, ".|.............nn................|.",  14,       2,     NV},
            {L_, ".|.............nn................|.",  15,       2,     NV},
            {L_, ".|.............nn................|.",  16,       1,     NV},
            {L_, ".|............................nn.|.",  28,       1,     NV},
            {L_, ".|............................nn.|.",  29,       2,     NV},
            {L_, ".|............................nn.|.",  30,       2,     NV},
            {L_, ".|............................nn.|.",  31,       1,     NV},
            {L_, ".|.............................nn|.",  29,       1,     NV},
            {L_, ".|.............................nn|.",  30,       2,     NV},
            {L_, ".|.............................nn|.",  31,       2,     NV},
            {L_, ".|.............................nn|.",  32,       1,     NV},
            {L_, ".|..............................n|n",  30,       1,     NV},
            {L_, ".|..............................n|n",  31,       2,     NV},
            {L_, ".|..............................n|n",  32,       2,     NV},
            {L_, ".|..............................n|n",  33,       1,     NV},

            //Calendar with 1 NON Business day, 1 Business day
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "n|B..............................|.",  -1,       1,     NV},
            {L_, "n|B..............................|.",   0,       0,      1},
            {L_, "n|B..............................|.",   1,       0,      1},
            {L_, "n|B..............................|.",   2,       1,     NV},
            {L_, ".|nB.............................|.",   0,       1,     NV},
            {L_, ".|nB.............................|.",   1,       0,      2},
            {L_, ".|nB.............................|.",   2,       0,      2},
            {L_, ".|nB.............................|.",   3,       1,     NV},
            {L_, ".|.nB............................|.",   1,       1,     NV},
            {L_, ".|.nB............................|.",   2,       0,      3},
            {L_, ".|.nB............................|.",   3,       0,      3},
            {L_, ".|.nB............................|.",   4,       1,     NV},
            {L_, ".|.............nB................|.",  13,       1,     NV},
            {L_, ".|.............nB................|.",  14,       0,     15},
            {L_, ".|.............nB................|.",  15,       0,     15},
            {L_, ".|.............nB................|.",  16,       1,     NV},
            {L_, ".|............................nB.|.",  28,       1,     NV},
            {L_, ".|............................nB.|.",  29,       0,     30},
            {L_, ".|............................nB.|.",  30,       0,     30},
            {L_, ".|............................nB.|.",  31,       1,     NV},
            {L_, ".|.............................nB|.",  29,       1,     NV},
            {L_, ".|.............................nB|.",  30,       0,     31},
            {L_, ".|.............................nB|.",  31,       0,     31},
            {L_, ".|.............................nB|.",  32,       1,     NV},
            {L_, ".|..............................n|B",  30,       1,     NV},
            {L_, ".|..............................n|B",  31,       0,     32},
            {L_, ".|..............................n|B",  32,       0,     32},
            {L_, ".|..............................n|B",  33,       1,     NV},

            //Calendar with 1 Business day, 1 NON Business day
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "B|n..............................|.",  -1,       1,     NV},
            {L_, "B|n..............................|.",   0,       0,      0},
            {L_, "B|n..............................|.",   1,       2,     NV},
            {L_, "B|n..............................|.",   2,       1,     NV},
            {L_, ".|Bn.............................|.",   0,       1,     NV},
            {L_, ".|Bn.............................|.",   1,       0,      1},
            {L_, ".|Bn.............................|.",   2,       2,     NV},
            {L_, ".|Bn.............................|.",   3,       1,     NV},
            {L_, ".|.Bn............................|.",   1,       1,     NV},
            {L_, ".|.Bn............................|.",   2,       0,      2},
            {L_, ".|.Bn............................|.",   3,       2,     NV},
            {L_, ".|.Bn............................|.",   4,       1,     NV},
            {L_, ".|.............Bn................|.",  13,       1,     NV},
            {L_, ".|.............Bn................|.",  14,       0,     14},
            {L_, ".|.............Bn................|.",  15,       2,     NV},
            {L_, ".|.............Bn................|.",  16,       1,     NV},
            {L_, ".|............................Bn.|.",  28,       1,     NV},
            {L_, ".|............................Bn.|.",  29,       0,     29},
            {L_, ".|............................Bn.|.",  30,       2,     NV},
            {L_, ".|............................Bn.|.",  31,       1,     NV},
            {L_, ".|.............................Bn|.",  29,       1,     NV},
            {L_, ".|.............................Bn|.",  30,       0,     30},
            {L_, ".|.............................Bn|.",  31,       2,     NV},
            {L_, ".|.............................Bn|.",  32,       1,     NV},
            {L_, ".|..............................B|n",  30,       1,     NV},
            {L_, ".|..............................B|n",  31,       0,     31},
            {L_, ".|..............................B|n",  32,       2,     NV},
            {L_, ".|..............................B|n",  33,       1,     NV},

            //Calendar with 2 Business Days
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "B|B..............................|.",  -1,       1,     NV},
            {L_, "B|B..............................|.",   0,       0,      0},
            {L_, "B|B..............................|.",   1,       0,      1},
            {L_, "B|B..............................|.",   2,       1,     NV},
            {L_, ".|BB.............................|.",   0,       1,     NV},
            {L_, ".|BB.............................|.",   1,       0,      1},
            {L_, ".|BB.............................|.",   2,       0,      2},
            {L_, ".|BB.............................|.",   3,       1,     NV},
            {L_, ".|.BB............................|.",   1,       1,     NV},
            {L_, ".|.BB............................|.",   2,       0,      2},
            {L_, ".|.BB............................|.",   3,       0,      3},
            {L_, ".|.BB............................|.",   4,       1,     NV},
            {L_, ".|.............BB................|.",  13,       1,     NV},
            {L_, ".|.............BB................|.",  14,       0,     14},
            {L_, ".|.............BB................|.",  15,       0,     15},
            {L_, ".|.............BB................|.",  16,       1,     NV},
            {L_, ".|............................BB.|.",  28,       1,     NV},
            {L_, ".|............................BB.|.",  29,       0,     29},
            {L_, ".|............................BB.|.",  30,       0,     30},
            {L_, ".|............................BB.|.",  31,       1,     NV},
            {L_, ".|.............................BB|.",  29,       1,     NV},
            {L_, ".|.............................BB|.",  30,       0,     30},
            {L_, ".|.............................BB|.",  31,       0,     31},
            {L_, ".|.............................BB|.",  32,       1,     NV},
            {L_, ".|..............................B|B",  30,       1,     NV},
            {L_, ".|..............................B|B",  31,       0,     31},
            {L_, ".|..............................B|B",  32,       0,     32},
            {L_, ".|..............................B|B",  33,       1,     NV},

            //Depth: 3
            //"NNN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|n..............................|..",  -2,     1,     NV},
            {L_, "nn|n..............................|..",  -1,     2,     NV},
            {L_, "nn|n..............................|..",   0,     2,     NV},
            {L_, "nn|n..............................|..",   1,     2,     NV},
            {L_, "nn|n..............................|..",   2,     1,     NV},
            {L_, ".n|nn.............................|..",  -1,     1,     NV},
            {L_, ".n|nn.............................|..",   0,     2,     NV},
            {L_, ".n|nn.............................|..",   1,     2,     NV},
            {L_, ".n|nn.............................|..",   2,     2,     NV},
            {L_, ".n|nn.............................|..",   3,     1,     NV},
            {L_, "..|nnn............................|..",   0,     1,     NV},
            {L_, "..|nnn............................|..",   1,     2,     NV},
            {L_, "..|nnn............................|..",   2,     2,     NV},
            {L_, "..|nnn............................|..",   3,     2,     NV},
            {L_, "..|nnn............................|..",   4,     1,     NV},
            {L_, "..|.nnn...........................|..",   1,     1,     NV},
            {L_, "..|.nnn...........................|..",   2,     2,     NV},
            {L_, "..|.nnn...........................|..",   3,     2,     NV},
            {L_, "..|.nnn...........................|..",   4,     2,     NV},
            {L_, "..|.nnn...........................|..",   5,     1,     NV},
            {L_, "..|.............nnn...............|..",  13,     1,     NV},
            {L_, "..|.............nnn...............|..",  14,     2,     NV},
            {L_, "..|.............nnn...............|..",  15,     2,     NV},
            {L_, "..|.............nnn...............|..",  16,     2,     NV},
            {L_, "..|.............nnn...............|..",  17,     1,     NV},
            {L_, "..|...........................nnn.|..",  27,     1,     NV},
            {L_, "..|...........................nnn.|..",  28,     2,     NV},
            {L_, "..|...........................nnn.|..",  29,     2,     NV},
            {L_, "..|...........................nnn.|..",  30,     2,     NV},
            {L_, "..|...........................nnn.|..",  31,     1,     NV},
            {L_, "..|............................nnn|..",  28,     1,     NV},
            {L_, "..|............................nnn|..",  29,     2,     NV},
            {L_, "..|............................nnn|..",  30,     2,     NV},
            {L_, "..|............................nnn|..",  31,     2,     NV},
            {L_, "..|............................nnn|..",  32,     1,     NV},
            {L_, "..|.............................nn|n.",  29,     1,     NV},
            {L_, "..|.............................nn|n.",  30,     2,     NV},
            {L_, "..|.............................nn|n.",  31,     2,     NV},
            {L_, "..|.............................nn|n.",  32,     2,     NV},
            {L_, "..|.............................nn|n.",  33,     1,     NV},
            {L_, "..|..............................n|nn",  30,     1,     NV},
            {L_, "..|..............................n|nn",  31,     2,     NV},
            {L_, "..|..............................n|nn",  32,     2,     NV},
            {L_, "..|..............................n|nn",  33,     2,     NV},
            {L_, "..|..............................n|nn",  34,     1,     NV},

            //"NNB".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|B..............................|..",  -2,     1,     NV},
            {L_, "nn|B..............................|..",  -1,     0,      1},
            {L_, "nn|B..............................|..",   0,     0,      1},
            {L_, "nn|B..............................|..",   1,     0,      1},
            {L_, "nn|B..............................|..",   2,     1,     NV},
            {L_, ".n|nB.............................|..",  -1,     1,     NV},
            {L_, ".n|nB.............................|..",   0,     0,      2},
            {L_, ".n|nB.............................|..",   1,     0,      2},
            {L_, ".n|nB.............................|..",   2,     0,      2},
            {L_, ".n|nB.............................|..",   3,     1,     NV},
            {L_, "..|nnB............................|..",   0,     1,     NV},
            {L_, "..|nnB............................|..",   1,     0,      3},
            {L_, "..|nnB............................|..",   2,     0,      3},
            {L_, "..|nnB............................|..",   3,     0,      3},
            {L_, "..|nnB............................|..",   4,     1,     NV},
            {L_, "..|.nnB...........................|..",   1,     1,     NV},
            {L_, "..|.nnB...........................|..",   2,     0,      4},
            {L_, "..|.nnB...........................|..",   3,     0,      4},
            {L_, "..|.nnB...........................|..",   4,     0,      4},
            {L_, "..|.nnB...........................|..",   5,     1,     NV},
            {L_, "..|.............nnB...............|..",  13,     1,     NV},
            {L_, "..|.............nnB...............|..",  14,     0,     16},
            {L_, "..|.............nnB...............|..",  15,     0,     16},
            {L_, "..|.............nnB...............|..",  16,     0,     16},
            {L_, "..|.............nnB...............|..",  17,     1,     NV},
            {L_, "..|...........................nnB.|..",  27,     1,     NV},
            {L_, "..|...........................nnB.|..",  28,     0,     30},
            {L_, "..|...........................nnB.|..",  29,     0,     30},
            {L_, "..|...........................nnB.|..",  30,     0,     30},
            {L_, "..|...........................nnB.|..",  31,     1,     NV},
            {L_, "..|............................nnB|..",  28,     1,     NV},
            {L_, "..|............................nnB|..",  29,     0,     31},
            {L_, "..|............................nnB|..",  30,     0,     31},
            {L_, "..|............................nnB|..",  31,     0,     31},
            {L_, "..|............................nnB|..",  32,     1,     NV},
            {L_, "..|.............................nn|B.",  29,     1,     NV},
            {L_, "..|.............................nn|B.",  30,     0,     32},
            {L_, "..|.............................nn|B.",  31,     0,     32},
            {L_, "..|.............................nn|B.",  32,     0,     32},
            {L_, "..|.............................nn|B.",  33,     1,     NV},
            {L_, "..|..............................n|nB",  30,     1,     NV},
            {L_, "..|..............................n|nB",  31,     0,     33},
            {L_, "..|..............................n|nB",  32,     0,     33},
            {L_, "..|..............................n|nB",  33,     0,     33},
            {L_, "..|..............................n|nB",  34,     1,     NV},

            //"NBN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nB|n..............................|..",  -2,     1,     NV},
            {L_, "nB|n..............................|..",  -1,     0,      0},
            {L_, "nB|n..............................|..",   0,     0,      0},
            {L_, "nB|n..............................|..",   1,     2,     NV},
            {L_, "nB|n..............................|..",   2,     1,     NV},
            {L_, ".n|Bn.............................|..",  -1,     1,     NV},
            {L_, ".n|Bn.............................|..",   0,     0,      1},
            {L_, ".n|Bn.............................|..",   1,     0,      1},
            {L_, ".n|Bn.............................|..",   2,     2,     NV},
            {L_, ".n|Bn.............................|..",   3,     1,     NV},
            {L_, "..|nBn............................|..",   0,     1,     NV},
            {L_, "..|nBn............................|..",   1,     0,      2},
            {L_, "..|nBn............................|..",   2,     0,      2},
            {L_, "..|nBn............................|..",   3,     2,     NV},
            {L_, "..|nBn............................|..",   4,     1,     NV},
            {L_, "..|.nBn...........................|..",   1,     1,     NV},
            {L_, "..|.nBn...........................|..",   2,     0,      3},
            {L_, "..|.nBn...........................|..",   3,     0,      3},
            {L_, "..|.nBn...........................|..",   4,     2,     NV},
            {L_, "..|.nBn...........................|..",   5,     1,     NV},
            {L_, "..|.............nBn...............|..",  13,     1,     NV},
            {L_, "..|.............nBn...............|..",  14,     0,     15},
            {L_, "..|.............nBn...............|..",  15,     0,     15},
            {L_, "..|.............nBn...............|..",  16,     2,     NV},
            {L_, "..|.............nBn...............|..",  17,     1,     NV},
            {L_, "..|...........................nBn.|..",  27,     1,     NV},
            {L_, "..|...........................nBn.|..",  28,     0,     29},
            {L_, "..|...........................nBn.|..",  29,     0,     29},
            {L_, "..|...........................nBn.|..",  30,     2,     NV},
            {L_, "..|...........................nBn.|..",  31,     1,     NV},
            {L_, "..|............................nBn|..",  28,     1,     NV},
            {L_, "..|............................nBn|..",  29,     0,     30},
            {L_, "..|............................nBn|..",  30,     0,     30},
            {L_, "..|............................nBn|..",  31,     2,     NV},
            {L_, "..|............................nBn|..",  32,     1,     NV},
            {L_, "..|.............................nB|n.",  29,     1,     NV},
            {L_, "..|.............................nB|n.",  30,     0,     31},
            {L_, "..|.............................nB|n.",  31,     0,     31},
            {L_, "..|.............................nB|n.",  32,     2,     NV},
            {L_, "..|.............................nB|n.",  33,     1,     NV},
            {L_, "..|..............................n|Bn",  30,     1,     NV},
            {L_, "..|..............................n|Bn",  31,     0,     32},
            {L_, "..|..............................n|Bn",  32,     0,     32},
            {L_, "..|..............................n|Bn",  33,     2,     NV},
            {L_, "..|..............................n|Bn",  34,     1,     NV},

            //"NBB".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nB|B..............................|..",  -2,     1,     NV},
            {L_, "nB|B..............................|..",  -1,     0,      0},
            {L_, "nB|B..............................|..",   0,     0,      0},
            {L_, "nB|B..............................|..",   1,     0,      1},
            {L_, "nB|B..............................|..",   2,     1,     NV},
            {L_, ".n|BB.............................|..",  -1,     1,     NV},
            {L_, ".n|BB.............................|..",   0,     0,      1},
            {L_, ".n|BB.............................|..",   1,     0,      1},
            {L_, ".n|BB.............................|..",   2,     0,      2},
            {L_, ".n|BB.............................|..",   3,     1,     NV},
            {L_, "..|nBB............................|..",   0,     1,     NV},
            {L_, "..|nBB............................|..",   1,     0,      2},
            {L_, "..|nBB............................|..",   2,     0,      2},
            {L_, "..|nBB............................|..",   3,     0,      3},
            {L_, "..|nBB............................|..",   4,     1,     NV},
            {L_, "..|.nBB...........................|..",   1,     1,     NV},
            {L_, "..|.nBB...........................|..",   2,     0,      3},
            {L_, "..|.nBB...........................|..",   3,     0,      3},
            {L_, "..|.nBB...........................|..",   4,     0,      4},
            {L_, "..|.nBB...........................|..",   5,     1,     NV},
            {L_, "..|.............nBB...............|..",  13,     1,     NV},
            {L_, "..|.............nBB...............|..",  14,     0,     15},
            {L_, "..|.............nBB...............|..",  15,     0,     15},
            {L_, "..|.............nBB...............|..",  16,     0,     16},
            {L_, "..|.............nBB...............|..",  17,     1,     NV},
            {L_, "..|...........................nBB.|..",  27,     1,     NV},
            {L_, "..|...........................nBB.|..",  28,     0,     29},
            {L_, "..|...........................nBB.|..",  29,     0,     29},
            {L_, "..|...........................nBB.|..",  30,     0,     30},
            {L_, "..|...........................nBB.|..",  31,     1,     NV},
            {L_, "..|............................nBB|..",  28,     1,     NV},
            {L_, "..|............................nBB|..",  29,     0,     30},
            {L_, "..|............................nBB|..",  30,     0,     30},
            {L_, "..|............................nBB|..",  31,     0,     31},
            {L_, "..|............................nBB|..",  32,     1,     NV},
            {L_, "..|.............................nB|B.",  29,     1,     NV},
            {L_, "..|.............................nB|B.",  30,     0,     31},
            {L_, "..|.............................nB|B.",  31,     0,     31},
            {L_, "..|.............................nB|B.",  32,     0,     32},
            {L_, "..|.............................nB|B.",  33,     1,     NV},
            {L_, "..|..............................n|BB",  30,     1,     NV},

            //"BNN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "Bn|n..............................|..",  -2,     1,     NV},
            {L_, "Bn|n..............................|..",  -1,     0,     -1},
            {L_, "Bn|n..............................|..",   0,     2,     NV},
            {L_, "Bn|n..............................|..",   1,     2,     NV},
            {L_, "Bn|n..............................|..",   2,     1,     NV},
            {L_, ".B|nn.............................|..",  -1,     1,     NV},
            {L_, ".B|nn.............................|..",   0,     0,      0},
            {L_, ".B|nn.............................|..",   1,     2,     NV},
            {L_, ".B|nn.............................|..",   2,     2,     NV},
            {L_, ".B|nn.............................|..",   3,     1,     NV},
            {L_, "..|Bnn............................|..",   0,     1,     NV},
            {L_, "..|Bnn............................|..",   1,     0,      1},
            {L_, "..|Bnn............................|..",   2,     2,     NV},
            {L_, "..|Bnn............................|..",   3,     2,     NV},
            {L_, "..|Bnn............................|..",   4,     1,     NV},
            {L_, "..|.Bnn...........................|..",   1,     1,     NV},
            {L_, "..|.Bnn...........................|..",   2,     0,      2},
            {L_, "..|.Bnn...........................|..",   3,     2,     NV},
            {L_, "..|.Bnn...........................|..",   4,     2,     NV},
            {L_, "..|.Bnn...........................|..",   5,     1,     NV},
            {L_, "..|.............Bnn...............|..",  13,     1,     NV},
            {L_, "..|.............Bnn...............|..",  14,     0,     14},
            {L_, "..|.............Bnn...............|..",  15,     2,     NV},
            {L_, "..|.............Bnn...............|..",  16,     2,     NV},
            {L_, "..|.............Bnn...............|..",  17,     1,     NV},
            {L_, "..|...........................Bnn.|..",  27,     1,     NV},
            {L_, "..|...........................Bnn.|..",  28,     0,     28},
            {L_, "..|...........................Bnn.|..",  29,     2,     NV},
            {L_, "..|...........................Bnn.|..",  30,     2,     NV},
            {L_, "..|...........................Bnn.|..",  31,     1,     NV},
            {L_, "..|............................Bnn|..",  28,     1,     NV},
            {L_, "..|............................Bnn|..",  29,     0,     29},
            {L_, "..|............................Bnn|..",  30,     2,     NV},
            {L_, "..|............................Bnn|..",  31,     2,     NV},
            {L_, "..|............................Bnn|..",  32,     1,     NV},
            {L_, "..|.............................Bn|n.",  29,     1,     NV},
            {L_, "..|.............................Bn|n.",  30,     0,     30},
            {L_, "..|.............................Bn|n.",  31,     2,     NV},
            {L_, "..|.............................Bn|n.",  32,     2,     NV},
            {L_, "..|.............................Bn|n.",  33,     1,     NV},
            {L_, "..|..............................B|nn",  30,     1,     NV},
            {L_, "..|..............................B|nn",  31,     0,     31},
            {L_, "..|..............................B|nn",  32,     2,     NV},
            {L_, "..|..............................B|nn",  33,     2,     NV},
            {L_, "..|..............................B|nn",  34,     1,     NV},

            //"BNB".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "Bn|B..............................|..",  -2,     1,     NV},
            {L_, "Bn|B..............................|..",  -1,     0,     -1},
            {L_, "Bn|B..............................|..",   0,     0,      1},
            {L_, "Bn|B..............................|..",   1,     0,      1},
            {L_, "Bn|B..............................|..",   2,     1,     NV},
            {L_, ".B|nB.............................|..",  -1,     1,     NV},
            {L_, ".B|nB.............................|..",   0,     0,      0},
            {L_, ".B|nB.............................|..",   1,     0,      2},
            {L_, ".B|nB.............................|..",   2,     0,      2},
            {L_, ".B|nB.............................|..",   3,     1,     NV},
            {L_, "..|BnB............................|..",   0,     1,     NV},
            {L_, "..|BnB............................|..",   1,     0,      1},
            {L_, "..|BnB............................|..",   2,     0,      3},
            {L_, "..|BnB............................|..",   3,     0,      3},
            {L_, "..|BnB............................|..",   4,     1,     NV},
            {L_, "..|.BnB...........................|..",   1,     1,     NV},
            {L_, "..|.BnB...........................|..",   2,     0,      2},
            {L_, "..|.BnB...........................|..",   3,     0,      4},
            {L_, "..|.BnB...........................|..",   4,     0,      4},
            {L_, "..|.BnB...........................|..",   5,     1,     NV},
            {L_, "..|.............BnB...............|..",  13,     1,     NV},
            {L_, "..|.............BnB...............|..",  14,     0,     14},
            {L_, "..|.............BnB...............|..",  15,     0,     16},
            {L_, "..|.............BnB...............|..",  16,     0,     16},
            {L_, "..|.............BnB...............|..",  17,     1,     NV},
            {L_, "..|...........................BnB.|..",  27,     1,     NV},
            {L_, "..|...........................BnB.|..",  28,     0,     28},
            {L_, "..|...........................BnB.|..",  29,     0,     30},
            {L_, "..|...........................BnB.|..",  30,     0,     30},
            {L_, "..|...........................BnB.|..",  31,     1,     NV},
            {L_, "..|............................BnB|..",  28,     1,     NV},
            {L_, "..|............................BnB|..",  29,     0,     29},
            {L_, "..|............................BnB|..",  30,     0,     31},
            {L_, "..|............................BnB|..",  31,     0,     31},
            {L_, "..|............................BnB|..",  32,     1,     NV},
            {L_, "..|.............................Bn|B.",  29,     1,     NV},
            {L_, "..|.............................Bn|B.",  30,     0,     30},
            {L_, "..|.............................Bn|B.",  31,     0,     32},
            {L_, "..|.............................Bn|B.",  32,     0,     32},
            {L_, "..|.............................Bn|B.",  33,     1,     NV},
            {L_, "..|..............................B|nB",  30,     1,     NV},
            {L_, "..|..............................B|nB",  31,     0,     31},
            {L_, "..|..............................B|nB",  32,     0,     33},
            {L_, "..|..............................B|nB",  33,     0,     33},
            {L_, "..|..............................B|nB",  34,     1,     NV},

            //"BBN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "BB|n..............................|..",  -2,     1,     NV},
            {L_, "BB|n..............................|..",  -1,     0,     -1},
            {L_, "BB|n..............................|..",   0,     0,      0},
            {L_, "BB|n..............................|..",   1,     2,     NV},
            {L_, "BB|n..............................|..",   2,     1,     NV},
            {L_, ".B|Bn.............................|..",  -1,     1,     NV},
            {L_, ".B|Bn.............................|..",   0,     0,      0},
            {L_, ".B|Bn.............................|..",   1,     0,      1},
            {L_, ".B|Bn.............................|..",   2,     2,     NV},
            {L_, ".B|Bn.............................|..",   3,     1,     NV},
            {L_, "..|BBn............................|..",   0,     1,     NV},
            {L_, "..|BBn............................|..",   1,     0,      1},
            {L_, "..|BBn............................|..",   2,     0,      2},
            {L_, "..|BBn............................|..",   3,     2,     NV},
            {L_, "..|BBn............................|..",   4,     1,     NV},
            {L_, "..|.BBn...........................|..",   1,     1,     NV},
            {L_, "..|.BBn...........................|..",   2,     0,      2},
            {L_, "..|.BBn...........................|..",   3,     0,      3},
            {L_, "..|.BBn...........................|..",   4,     2,     NV},
            {L_, "..|.BBn...........................|..",   5,     1,     NV},
            {L_, "..|.............BBn...............|..",  13,     1,     NV},
            {L_, "..|.............BBn...............|..",  14,     0,     14},
            {L_, "..|.............BBn...............|..",  15,     0,     15},
            {L_, "..|.............BBn...............|..",  16,     2,     NV},
            {L_, "..|.............BBn...............|..",  17,     1,     NV},
            {L_, "..|...........................BBn.|..",  27,     1,     NV},
            {L_, "..|...........................BBn.|..",  28,     0,     28},
            {L_, "..|...........................BBn.|..",  29,     0,     29},
            {L_, "..|...........................BBn.|..",  30,     2,     NV},
            {L_, "..|...........................BBn.|..",  31,     1,     NV},
            {L_, "..|............................BBn|..",  28,     1,     NV},
            {L_, "..|............................BBn|..",  29,     0,     29},
            {L_, "..|............................BBn|..",  30,     0,     30},
            {L_, "..|............................BBn|..",  31,     2,     NV},
            {L_, "..|............................BBn|..",  32,     1,     NV},
            {L_, "..|.............................BB|n.",  29,     1,     NV},
            {L_, "..|.............................BB|n.",  30,     0,     30},
            {L_, "..|.............................BB|n.",  31,     0,     31},
            {L_, "..|.............................BB|n.",  32,     2,     NV},
            {L_, "..|.............................BB|n.",  33,     1,     NV},
            {L_, "..|..............................B|Bn",  30,     1,     NV},
            {L_, "..|..............................B|Bn",  31,     0,     31},
            {L_, "..|..............................B|Bn",  32,     0,     32},
            {L_, "..|..............................B|Bn",  33,     2,     NV},
            {L_, "..|..............................B|Bn",  34,     1,     NV},

            //"BBB".
            //LINE   INPUT                            ORIGINAL  STATUS  RESULT
            //----   -----                            --------  ------  ------
            {L_, "BB|B..............................|..",  -2,     1,     NV},
            {L_, "BB|B..............................|..",  -1,     0,     -1},
            {L_, "BB|B..............................|..",   0,     0,      0},
            {L_, "BB|B..............................|..",   1,     0,      1},
            {L_, "BB|B..............................|..",   2,     1,     NV},
            {L_, ".B|BB.............................|..",  -1,     1,     NV},
            {L_, ".B|BB.............................|..",   0,     0,      0},
            {L_, ".B|BB.............................|..",   1,     0,      1},
            {L_, ".B|BB.............................|..",   2,     0,      2},
            {L_, ".B|BB.............................|..",   3,     1,     NV},
            {L_, "..|BBB............................|..",   0,     1,     NV},
            {L_, "..|BBB............................|..",   1,     0,      1},
            {L_, "..|BBB............................|..",   2,     0,      2},
            {L_, "..|BBB............................|..",   3,     0,      3},
            {L_, "..|BBB............................|..",   4,     1,     NV},
            {L_, "..|.BBB...........................|..",   1,     1,     NV},
            {L_, "..|.BBB...........................|..",   2,     0,      2},
            {L_, "..|.BBB...........................|..",   3,     0,      3},
            {L_, "..|.BBB...........................|..",   4,     0,      4},
            {L_, "..|.BBB...........................|..",   5,     1,     NV},
            {L_, "..|.............BBB...............|..",  13,     1,     NV},
            {L_, "..|.............BBB...............|..",  14,     0,     14},
            {L_, "..|.............BBB...............|..",  15,     0,     15},
            {L_, "..|.............BBB...............|..",  16,     0,     16},
            {L_, "..|.............BBB...............|..",  17,     1,     NV},
            {L_, "..|...........................BBB.|..",  27,     1,     NV},
            {L_, "..|...........................BBB.|..",  28,     0,     28},
            {L_, "..|...........................BBB.|..",  29,     0,     29},
            {L_, "..|...........................BBB.|..",  30,     0,     30},
            {L_, "..|...........................BBB.|..",  31,     1,     NV},
            {L_, "..|............................BBB|..",  28,     1,     NV},
            {L_, "..|............................BBB|..",  29,     0,     29},
            {L_, "..|............................BBB|..",  30,     0,     30},
            {L_, "..|............................BBB|..",  31,     0,     31},
            {L_, "..|............................BBB|..",  32,     1,     NV},
            {L_, "..|.............................BB|B.",  29,     1,     NV},
            {L_, "..|.............................BB|B.",  30,     0,     30},
            {L_, "..|.............................BB|B.",  31,     0,     31},
            {L_, "..|.............................BB|B.",  32,     0,     32},
            {L_, "..|.............................BB|B.",  33,     1,     NV},
            {L_, "..|..............................B|BB",  30,     1,     NV},
            {L_, "..|..............................B|BB",  31,     0,     31},
            {L_, "..|..............................B|BB",  32,     0,     32},
            {L_, "..|..............................B|BB",  33,     0,     33},
            {L_, "..|..............................B|BB",  34,     1,     NV},

            //Calendar that covers whole month (Calendar.length() == 35)
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",  15,     0,     15},
            {L_, "BB|BBBBBBBBBBBBBBnBBBBBBBBBBBBBBBB|BB",  15,     0,     16},
            {L_, "BB|BBBBBBBBBBBBBBnnBBBBBBBBBBBBBBB|BB",  15,     0,     17},
            {L_, "BB|BBBBBBBBBBBBBBnnnBBBBBBBBBBBBBB|BB",  15,     0,     18},
            {L_, "BB|BBBBBBBBBBBBBBnnnnBBBBBBBBBBBBB|BB",  15,     0,     19},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnBBBBBBBBBBBB|BB",  15,     0,     20},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnBBBBBBBBBBB|BB",  15,     0,     21},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnBBBBBBBBBB|BB",  15,     0,     22},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnBBBBBBBBB|BB",  15,     0,     23},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnBBBBBBBB|BB",  15,     0,     24},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnBBBBBBB|BB",  15,     0,     25},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnBBBBBB|BB",  15,     0,     26},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnBBBBB|BB",  15,     0,     27},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnBBBB|BB",  15,     0,     28},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnBBB|BB",  15,     0,     29},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnBB|BB",  15,     0,     30},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnB|BB",  15,     0,     31},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnn|BB",  15,     0,     32},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnn|nB",  15,     0,     33},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|BBBBBBBBBBBBBnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|BBBBBBBBBBBBnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|BBBBBBBBBBBnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|BBBBBBBBBBnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|BBBBBBBBBnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|BBBBBBBBnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|BBBBBBBnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|BBBBBBnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|BBBBBnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|BBBBnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|BBBnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "BB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "Bn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},

            //Corner cases
            //Origin: 15
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},

            //Origin: 32, it first date of the next month
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},

            //Origin 31
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     2,     NV},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     2,     NV},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},

            //Origin 30
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     2,     NV},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},

            //Origin 2
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     2,     NV},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},

            //Origin 1
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      2},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      2},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},

            //Origin  0, i.e last day of the previous m
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      2},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      1},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0}
        };
        const bdlt::Date ORIGIN = bdlt::Date(1999, 12, 31);

        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);
        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE     = DATA[i].d_line;
            const char       *INPUT    = DATA[i].d_input_p;
            const bdlt::Date  START    = ORIGIN + getStartDate(INPUT);
            const bdlt::Date  ORIGINAL = ORIGIN + DATA[i].d_original;
            bdlt::Date        LOADED   = ORIGIN - 5;
            const int         STATUS   = DATA[i].d_status;
            const bdlt::Date  RESULT   = ORIGIN + DATA[i].d_result;

            bdlt::Calendar calendar = parseCalendar(INPUT, START);

            int rStatus = Util::shiftFollowingIfValid(&LOADED,
                                                      ORIGINAL,
                                                      calendar);

            if (veryVerbose) {
                T_ P_(LINE) P_(ORIGINAL) P_(RESULT) P(LOADED);
            }

            // Check status
            ASSERTV(LINE,
                    INPUT,
                    START,
                    ORIGINAL,
                    STATUS,
                    rStatus,
                    rStatus == STATUS);

            // Check loaded
            ASSERTV(LINE,
                    INPUT,
                    START,
                    ORIGINAL,
                    LOADED,
                    RESULT,
                    RESULT == LOADED);
        }

        // negative tests
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Calendar cdr;
            bdlt::Date     date;
            bdlt::Date     result;

            ASSERT_SAFE_PASS(Util::shiftFollowingIfValid(&result, date, cdr));
            ASSERT_SAFE_FAIL(Util::shiftFollowingIfValid(      0, date, cdr));
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        //  TESTING 'shiftModifiedFollowingIfValid'
        //    Ensure 'shiftModifiedFollowingIfValid' loads proper business day
        //    or returns appropriate status in case of failure.
        //
        // Concerns:
        //: 1 This method returns status with value '1' if 'original' date is
        //:   out of range of a specified calendar, and returns status with
        //:   value '2' if while looking for a business day withing the month,
        //:   boundaries of the valid range of a 'calendar' were exceeded.
        //:
        //: 2 If 'original' date itself is a business day inside of the valid
        //:   range, this method loads it to the 'result' and returns zero.
        //:
        //: 3 If 'original' date is not a business day, this methods loads
        //:   the date of the chronologically earliest business day following
        //:   'original' date if it can be found within the current month.
        //:
        //: 4 If 'original' date is not a business day and there is no business
        //:   days withing this month that follow 'original' date, this method
        //:   loads chronologically latest business day preceding 'original'
        //:   date.
        //:
        //: 5 If 'original' date is not a business day and no business day were
        //:   found within the month of an 'original' date, status with value
        //:   '3' is returned.
        //:
        //: 6 If non-zero value is returned, result remains unchanged.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the table-driven approach, define a representative set of
        //:   valid inputs.  Verify that the function returns the correct
        //:   value.  (C-1..6)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values.  (C-7)
        //
        // Testing:
        //  shiftModifiedFollowingIfValid(bdlt::Date *result, orig, calendar)
        // --------------------------------------------------------------------

        bslma::TestAllocator         defaultAllocator;
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        if (verbose) cout << endl
                          << "TESTING 'shiftModifiedFollowingIfValid'" << endl
                          << "=======================================" << endl;

        static const struct {
            int         d_line;      // source line
            const char *d_input_p;   // input values
            int         d_original;  // original date
            int         d_status;    // expected status
            int         d_result;    // expected result
        } DATA[] = {
            // All days represented as an offset from December 31, 1999.

            //Depth: 0
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, ".|...............................|.",  0,       1,      NV},
            {L_, ".|...............................|.",  1,       1,      NV},
            {L_, ".|...............................|.",  2,       1,      NV},

            //Depth: 1
            //Calendar with 1 NON business day.
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, ".|n..............................|.",   0,      1,      NV},
            {L_, ".|n..............................|.",   1,      2,      NV},
            {L_, ".|n..............................|.",   2,      1,      NV},
            {L_, ".|.n.............................|.",   1,      1,      NV},
            {L_, ".|.n.............................|.",   2,      2,      NV},
            {L_, ".|.n.............................|.",   3,      1,      NV},
            {L_, ".|..............n................|.",  14,      1,      NV},
            {L_, ".|..............n................|.",  15,      2,      NV},
            {L_, ".|..............n................|.",  16,      1,      NV},
            {L_, ".|.............................n.|.",  29,      1,      NV},
            {L_, ".|.............................n.|.",  30,      2,      NV},
            {L_, ".|.............................n.|.",  31,      1,      NV},
            {L_, ".|..............................n|.",  30,      1,      NV},
            {L_, ".|..............................n|.",  31,      2,      NV},
            {L_, ".|..............................n|.",  32,      1,      NV},

            //Calendar with 1 business day.
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, ".|B..............................|.",   0,      1,     NV },
            {L_, ".|B..............................|.",   1,      0,      1 },
            {L_, ".|B..............................|.",   2,      1,     NV },
            {L_, ".|.B.............................|.",   1,      1,     NV },
            {L_, ".|.B.............................|.",   2,      0,      2 },
            {L_, ".|.B.............................|.",   3,      1,     NV },
            {L_, ".|..............B................|.",  14,      1,     NV },
            {L_, ".|..............B................|.",  15,      0,     15 },
            {L_, ".|..............B................|.",  16,      1,     NV },
            {L_, ".|.............................B.|.",  29,      1,     NV },
            {L_, ".|.............................B.|.",  30,      0,     30 },
            {L_, ".|.............................B.|.",  31,      1,     NV },
            {L_, ".|..............................B|.",  30,      1,     NV },
            {L_, ".|..............................B|.",  31,      0,     31 },
            {L_, ".|..............................B|.",  32,      1,     NV },

            //Depth: 2
            // Calendar with 2 NON business days.
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "n|n..............................|.",  -1,       1,     NV},
            {L_, "n|n..............................|.",   0,       2,     NV},
            {L_, "n|n..............................|.",   1,       2,     NV},
            {L_, "n|n..............................|.",   2,       1,     NV},
            {L_, ".|nn.............................|.",   0,       1,     NV},
            {L_, ".|nn.............................|.",   1,       2,     NV},
            {L_, ".|nn.............................|.",   2,       2,     NV},
            {L_, ".|nn.............................|.",   3,       1,     NV},
            {L_, ".|.nn............................|.",   1,       1,     NV},
            {L_, ".|.nn............................|.",   2,       2,     NV},
            {L_, ".|.nn............................|.",   3,       2,     NV},
            {L_, ".|.nn............................|.",   4,       1,     NV},
            {L_, ".|.............nn................|.",  13,       1,     NV},
            {L_, ".|.............nn................|.",  14,       2,     NV},
            {L_, ".|.............nn................|.",  15,       2,     NV},
            {L_, ".|.............nn................|.",  16,       1,     NV},
            {L_, ".|............................nn.|.",  28,       1,     NV},
            {L_, ".|............................nn.|.",  29,       2,     NV},
            {L_, ".|............................nn.|.",  30,       2,     NV},
            {L_, ".|............................nn.|.",  31,       1,     NV},
            {L_, ".|.............................nn|.",  29,       1,     NV},
            {L_, ".|.............................nn|.",  30,       2,     NV},
            {L_, ".|.............................nn|.",  31,       2,     NV},
            {L_, ".|.............................nn|.",  32,       1,     NV},
            {L_, ".|..............................n|n",  30,       1,     NV},
            {L_, ".|..............................n|n",  31,       2,     NV},
            {L_, ".|..............................n|n",  32,       2,     NV},
            {L_, ".|..............................n|n",  33,       1,     NV},

            //Calendar with 1 NON Business day, 1 Business day
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "n|B..............................|.",  -1,       1,     NV},
            {L_, "n|B..............................|.",   0,       2,     NV},
            {L_, "n|B..............................|.",   1,       0,      1},
            {L_, "n|B..............................|.",   2,       1,     NV},
            {L_, ".|nB.............................|.",   0,       1,     NV},
            {L_, ".|nB.............................|.",   1,       0,      2},
            {L_, ".|nB.............................|.",   2,       0,      2},
            {L_, ".|nB.............................|.",   3,       1,     NV},
            {L_, ".|.nB............................|.",   1,       1,     NV},
            {L_, ".|.nB............................|.",   2,       0,      3},
            {L_, ".|.nB............................|.",   3,       0,      3},
            {L_, ".|.nB............................|.",   4,       1,     NV},
            {L_, ".|.............nB................|.",  13,       1,     NV},
            {L_, ".|.............nB................|.",  14,       0,     15},
            {L_, ".|.............nB................|.",  15,       0,     15},
            {L_, ".|.............nB................|.",  16,       1,     NV},
            {L_, ".|............................nB.|.",  28,       1,     NV},
            {L_, ".|............................nB.|.",  29,       0,     30},
            {L_, ".|............................nB.|.",  30,       0,     30},
            {L_, ".|............................nB.|.",  31,       1,     NV},
            {L_, ".|.............................nB|.",  29,       1,     NV},
            {L_, ".|.............................nB|.",  30,       0,     31},
            {L_, ".|.............................nB|.",  31,       0,     31},
            {L_, ".|.............................nB|.",  32,       1,     NV},
            {L_, ".|..............................n|B",  30,       1,     NV},
            {L_, ".|..............................n|B",  31,       2,     NV},
            {L_, ".|..............................n|B",  32,       0,     32},
            {L_, ".|..............................n|B",  33,       1,     NV},

            //Calendar with 1 Business day, 1 NON Business day
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "B|n..............................|.",  -1,       1,     NV},
            {L_, "B|n..............................|.",   0,       0,      0},
            {L_, "B|n..............................|.",   1,       2,     NV},
            {L_, "B|n..............................|.",   2,       1,     NV},
            {L_, ".|Bn.............................|.",   0,       1,     NV},
            {L_, ".|Bn.............................|.",   1,       0,      1},
            {L_, ".|Bn.............................|.",   2,       2,     NV},
            {L_, ".|Bn.............................|.",   3,       1,     NV},
            {L_, ".|.Bn............................|.",   1,       1,     NV},
            {L_, ".|.Bn............................|.",   2,       0,      2},
            {L_, ".|.Bn............................|.",   3,       2,     NV},
            {L_, ".|.Bn............................|.",   4,       1,     NV},
            {L_, ".|.............Bn................|.",  13,       1,     NV},
            {L_, ".|.............Bn................|.",  14,       0,     14},
            {L_, ".|.............Bn................|.",  15,       2,     NV},
            {L_, ".|.............Bn................|.",  16,       1,     NV},
            {L_, ".|............................Bn.|.",  28,       1,     NV},
            {L_, ".|............................Bn.|.",  29,       0,     29},
            {L_, ".|............................Bn.|.",  30,       2,     NV},
            {L_, ".|............................Bn.|.",  31,       1,     NV},
            {L_, ".|.............................Bn|.",  29,       1,     NV},
            {L_, ".|.............................Bn|.",  30,       0,     30},
            {L_, ".|.............................Bn|.",  31,       0,     30},
            {L_, ".|.............................Bn|.",  32,       1,     NV},
            {L_, ".|..............................B|n",  30,       1,     NV},
            {L_, ".|..............................B|n",  31,       0,     31},
            {L_, ".|..............................B|n",  32,       2,     NV},
            {L_, ".|..............................B|n",  33,       1,     NV},

            //Calendar with 2 Business Days
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "B|B..............................|.",  -1,       1,     NV},
            {L_, "B|B..............................|.",   0,       0,      0},
            {L_, "B|B..............................|.",   1,       0,      1},
            {L_, "B|B..............................|.",   2,       1,     NV},
            {L_, ".|BB.............................|.",   0,       1,     NV},
            {L_, ".|BB.............................|.",   1,       0,      1},
            {L_, ".|BB.............................|.",   2,       0,      2},
            {L_, ".|BB.............................|.",   3,       1,     NV},
            {L_, ".|.BB............................|.",   1,       1,     NV},
            {L_, ".|.BB............................|.",   2,       0,      2},
            {L_, ".|.BB............................|.",   3,       0,      3},
            {L_, ".|.BB............................|.",   4,       1,     NV},
            {L_, ".|.............BB................|.",  13,       1,     NV},
            {L_, ".|.............BB................|.",  14,       0,     14},
            {L_, ".|.............BB................|.",  15,       0,     15},
            {L_, ".|.............BB................|.",  16,       1,     NV},
            {L_, ".|............................BB.|.",  28,       1,     NV},
            {L_, ".|............................BB.|.",  29,       0,     29},
            {L_, ".|............................BB.|.",  30,       0,     30},
            {L_, ".|............................BB.|.",  31,       1,     NV},
            {L_, ".|.............................BB|.",  29,       1,     NV},
            {L_, ".|.............................BB|.",  30,       0,     30},
            {L_, ".|.............................BB|.",  31,       0,     31},
            {L_, ".|.............................BB|.",  32,       1,     NV},
            {L_, ".|..............................B|B",  30,       1,     NV},
            {L_, ".|..............................B|B",  31,       0,     31},
            {L_, ".|..............................B|B",  32,       0,     32},
            {L_, ".|..............................B|B",  33,       1,     NV},

            //Depth: 3
            //"NNN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|n..............................|..",  -2,     1,     NV},
            {L_, "nn|n..............................|..",  -1,     2,     NV},
            {L_, "nn|n..............................|..",   0,     2,     NV},
            {L_, "nn|n..............................|..",   1,     2,     NV},
            {L_, "nn|n..............................|..",   2,     1,     NV},
            {L_, ".n|nn.............................|..",  -1,     1,     NV},
            {L_, ".n|nn.............................|..",   0,     2,     NV},
            {L_, ".n|nn.............................|..",   1,     2,     NV},
            {L_, ".n|nn.............................|..",   2,     2,     NV},
            {L_, ".n|nn.............................|..",   3,     1,     NV},
            {L_, "..|nnn............................|..",   0,     1,     NV},
            {L_, "..|nnn............................|..",   1,     2,     NV},
            {L_, "..|nnn............................|..",   2,     2,     NV},
            {L_, "..|nnn............................|..",   3,     2,     NV},
            {L_, "..|nnn............................|..",   4,     1,     NV},
            {L_, "..|.nnn...........................|..",   1,     1,     NV},
            {L_, "..|.nnn...........................|..",   2,     2,     NV},
            {L_, "..|.nnn...........................|..",   3,     2,     NV},
            {L_, "..|.nnn...........................|..",   4,     2,     NV},
            {L_, "..|.nnn...........................|..",   5,     1,     NV},
            {L_, "..|.............nnn...............|..",  13,     1,     NV},
            {L_, "..|.............nnn...............|..",  14,     2,     NV},
            {L_, "..|.............nnn...............|..",  15,     2,     NV},
            {L_, "..|.............nnn...............|..",  16,     2,     NV},
            {L_, "..|.............nnn...............|..",  17,     1,     NV},
            {L_, "..|...........................nnn.|..",  27,     1,     NV},
            {L_, "..|...........................nnn.|..",  28,     2,     NV},
            {L_, "..|...........................nnn.|..",  29,     2,     NV},
            {L_, "..|...........................nnn.|..",  30,     2,     NV},
            {L_, "..|...........................nnn.|..",  31,     1,     NV},
            {L_, "..|............................nnn|..",  28,     1,     NV},
            {L_, "..|............................nnn|..",  29,     2,     NV},
            {L_, "..|............................nnn|..",  30,     2,     NV},
            {L_, "..|............................nnn|..",  31,     2,     NV},
            {L_, "..|............................nnn|..",  32,     1,     NV},
            {L_, "..|.............................nn|n.",  29,     1,     NV},
            {L_, "..|.............................nn|n.",  30,     2,     NV},
            {L_, "..|.............................nn|n.",  31,     2,     NV},
            {L_, "..|.............................nn|n.",  32,     2,     NV},
            {L_, "..|.............................nn|n.",  33,     1,     NV},
            {L_, "..|..............................n|nn",  30,     1,     NV},
            {L_, "..|..............................n|nn",  31,     2,     NV},
            {L_, "..|..............................n|nn",  32,     2,     NV},
            {L_, "..|..............................n|nn",  33,     2,     NV},
            {L_, "..|..............................n|nn",  34,     1,     NV},

            //"NNB".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|B..............................|..",  -2,     1,     NV},
            {L_, "nn|B..............................|..",  -1,     2,     NV},
            {L_, "nn|B..............................|..",   0,     2,     NV},
            {L_, "nn|B..............................|..",   1,     0,      1},
            {L_, "nn|B..............................|..",   2,     1,     NV},
            {L_, ".n|nB.............................|..",  -1,     1,     NV},
            {L_, ".n|nB.............................|..",   0,     2,     NV},
            {L_, ".n|nB.............................|..",   1,     0,      2},
            {L_, ".n|nB.............................|..",   2,     0,      2},
            {L_, ".n|nB.............................|..",   3,     1,     NV},
            {L_, "..|nnB............................|..",   0,     1,     NV},
            {L_, "..|nnB............................|..",   1,     0,      3},
            {L_, "..|nnB............................|..",   2,     0,      3},
            {L_, "..|nnB............................|..",   3,     0,      3},
            {L_, "..|nnB............................|..",   4,     1,     NV},
            {L_, "..|.nnB...........................|..",   1,     1,     NV},
            {L_, "..|.nnB...........................|..",   2,     0,      4},
            {L_, "..|.nnB...........................|..",   3,     0,      4},
            {L_, "..|.nnB...........................|..",   4,     0,      4},
            {L_, "..|.nnB...........................|..",   5,     1,     NV},
            {L_, "..|.............nnB...............|..",  13,     1,     NV},
            {L_, "..|.............nnB...............|..",  14,     0,     16},
            {L_, "..|.............nnB...............|..",  15,     0,     16},
            {L_, "..|.............nnB...............|..",  16,     0,     16},
            {L_, "..|.............nnB...............|..",  17,     1,     NV},
            {L_, "..|...........................nnB.|..",  27,     1,     NV},
            {L_, "..|...........................nnB.|..",  28,     0,     30},
            {L_, "..|...........................nnB.|..",  29,     0,     30},
            {L_, "..|...........................nnB.|..",  30,     0,     30},
            {L_, "..|...........................nnB.|..",  31,     1,     NV},
            {L_, "..|............................nnB|..",  28,     1,     NV},
            {L_, "..|............................nnB|..",  29,     0,     31},
            {L_, "..|............................nnB|..",  30,     0,     31},
            {L_, "..|............................nnB|..",  31,     0,     31},
            {L_, "..|............................nnB|..",  32,     1,     NV},
            {L_, "..|.............................nn|B.",  29,     1,     NV},
            {L_, "..|.............................nn|B.",  30,     2,     NV},
            {L_, "..|.............................nn|B.",  31,     2,     NV},
            {L_, "..|.............................nn|B.",  32,     0,     32},
            {L_, "..|.............................nn|B.",  33,     1,     NV},
            {L_, "..|..............................n|nB",  30,     1,     NV},
            {L_, "..|..............................n|nB",  31,     2,     NV},
            {L_, "..|..............................n|nB",  32,     0,     33},
            {L_, "..|..............................n|nB",  33,     0,     33},
            {L_, "..|..............................n|nB",  34,     1,     NV},

            //"NBN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nB|n..............................|..",  -2,     1,     NV},
            {L_, "nB|n..............................|..",  -1,     0,      0},
            {L_, "nB|n..............................|..",   0,     0,      0},
            {L_, "nB|n..............................|..",   1,     2,     NV},
            {L_, "nB|n..............................|..",   2,     1,     NV},
            {L_, ".n|Bn.............................|..",  -1,     1,     NV},
            {L_, ".n|Bn.............................|..",   0,     2,     NV},
            {L_, ".n|Bn.............................|..",   1,     0,      1},
            {L_, ".n|Bn.............................|..",   2,     2,     NV},
            {L_, ".n|Bn.............................|..",   3,     1,     NV},
            {L_, "..|nBn............................|..",   0,     1,     NV},
            {L_, "..|nBn............................|..",   1,     0,      2},
            {L_, "..|nBn............................|..",   2,     0,      2},
            {L_, "..|nBn............................|..",   3,     2,     NV},
            {L_, "..|nBn............................|..",   4,     1,     NV},
            {L_, "..|.nBn...........................|..",   1,     1,     NV},
            {L_, "..|.nBn...........................|..",   2,     0,      3},
            {L_, "..|.nBn...........................|..",   3,     0,      3},
            {L_, "..|.nBn...........................|..",   4,     2,     NV},
            {L_, "..|.nBn...........................|..",   5,     1,     NV},
            {L_, "..|.............nBn...............|..",  13,     1,     NV},
            {L_, "..|.............nBn...............|..",  14,     0,     15},
            {L_, "..|.............nBn...............|..",  15,     0,     15},
            {L_, "..|.............nBn...............|..",  16,     2,     NV},
            {L_, "..|.............nBn...............|..",  17,     1,     NV},
            {L_, "..|...........................nBn.|..",  27,     1,     NV},
            {L_, "..|...........................nBn.|..",  28,     0,     29},
            {L_, "..|...........................nBn.|..",  29,     0,     29},
            {L_, "..|...........................nBn.|..",  30,     2,     NV},
            {L_, "..|...........................nBn.|..",  31,     1,     NV},
            {L_, "..|............................nBn|..",  28,     1,     NV},
            {L_, "..|............................nBn|..",  29,     0,     30},
            {L_, "..|............................nBn|..",  30,     0,     30},
            {L_, "..|............................nBn|..",  31,     0,     30},
            {L_, "..|............................nBn|..",  32,     1,     NV},
            {L_, "..|.............................nB|n.",  29,     1,     NV},
            {L_, "..|.............................nB|n.",  30,     0,     31},
            {L_, "..|.............................nB|n.",  31,     0,     31},
            {L_, "..|.............................nB|n.",  32,     2,     NV},
            {L_, "..|.............................nB|n.",  33,     1,     NV},
            {L_, "..|..............................n|Bn",  30,     1,     NV},
            {L_, "..|..............................n|Bn",  31,     2,     NV},
            {L_, "..|..............................n|Bn",  32,     0,     32},
            {L_, "..|..............................n|Bn",  33,     2,     NV},
            {L_, "..|..............................n|Bn",  34,     1,     NV},

            //"NBB".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nB|B..............................|..",  -2,     1,     NV},
            {L_, "nB|B..............................|..",  -1,     0,      0},
            {L_, "nB|B..............................|..",   0,     0,      0},
            {L_, "nB|B..............................|..",   1,     0,      1},
            {L_, "nB|B..............................|..",   2,     1,     NV},
            {L_, ".n|BB.............................|..",  -1,     1,     NV},
            {L_, ".n|BB.............................|..",   0,     2,     NV},
            {L_, ".n|BB.............................|..",   1,     0,      1},
            {L_, ".n|BB.............................|..",   2,     0,      2},
            {L_, ".n|BB.............................|..",   3,     1,     NV},
            {L_, "..|nBB............................|..",   0,     1,     NV},
            {L_, "..|nBB............................|..",   1,     0,      2},
            {L_, "..|nBB............................|..",   2,     0,      2},
            {L_, "..|nBB............................|..",   3,     0,      3},
            {L_, "..|nBB............................|..",   4,     1,     NV},
            {L_, "..|.nBB...........................|..",   1,     1,     NV},
            {L_, "..|.nBB...........................|..",   2,     0,      3},
            {L_, "..|.nBB...........................|..",   3,     0,      3},
            {L_, "..|.nBB...........................|..",   4,     0,      4},
            {L_, "..|.nBB...........................|..",   5,     1,     NV},
            {L_, "..|.............nBB...............|..",  13,     1,     NV},
            {L_, "..|.............nBB...............|..",  14,     0,     15},
            {L_, "..|.............nBB...............|..",  15,     0,     15},
            {L_, "..|.............nBB...............|..",  16,     0,     16},
            {L_, "..|.............nBB...............|..",  17,     1,     NV},
            {L_, "..|...........................nBB.|..",  27,     1,     NV},
            {L_, "..|...........................nBB.|..",  28,     0,     29},
            {L_, "..|...........................nBB.|..",  29,     0,     29},
            {L_, "..|...........................nBB.|..",  30,     0,     30},
            {L_, "..|...........................nBB.|..",  31,     1,     NV},
            {L_, "..|............................nBB|..",  28,     1,     NV},
            {L_, "..|............................nBB|..",  29,     0,     30},
            {L_, "..|............................nBB|..",  30,     0,     30},
            {L_, "..|............................nBB|..",  31,     0,     31},
            {L_, "..|............................nBB|..",  32,     1,     NV},
            {L_, "..|.............................nB|B.",  29,     1,     NV},
            {L_, "..|.............................nB|B.",  30,     0,     31},
            {L_, "..|.............................nB|B.",  31,     0,     31},
            {L_, "..|.............................nB|B.",  32,     0,     32},
            {L_, "..|.............................nB|B.",  33,     1,     NV},
            {L_, "..|..............................n|BB",  30,     1,     NV},

            //"BNN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "Bn|n..............................|..",  -2,     1,     NV},
            {L_, "Bn|n..............................|..",  -1,     0,     -1},
            {L_, "Bn|n..............................|..",   0,     0,     -1},
            {L_, "Bn|n..............................|..",   1,     2,     NV},
            {L_, "Bn|n..............................|..",   2,     1,     NV},
            {L_, ".B|nn.............................|..",  -1,     1,     NV},
            {L_, ".B|nn.............................|..",   0,     0,      0},
            {L_, ".B|nn.............................|..",   1,     2,     NV},
            {L_, ".B|nn.............................|..",   2,     2,     NV},
            {L_, ".B|nn.............................|..",   3,     1,     NV},
            {L_, "..|Bnn............................|..",   0,     1,     NV},
            {L_, "..|Bnn............................|..",   1,     0,      1},
            {L_, "..|Bnn............................|..",   2,     2,     NV},
            {L_, "..|Bnn............................|..",   3,     2,     NV},
            {L_, "..|Bnn............................|..",   4,     1,     NV},
            {L_, "..|.Bnn...........................|..",   1,     1,     NV},
            {L_, "..|.Bnn...........................|..",   2,     0,      2},
            {L_, "..|.Bnn...........................|..",   3,     2,     NV},
            {L_, "..|.Bnn...........................|..",   4,     2,     NV},
            {L_, "..|.Bnn...........................|..",   5,     1,     NV},
            {L_, "..|.............Bnn...............|..",  13,     1,     NV},
            {L_, "..|.............Bnn...............|..",  14,     0,     14},
            {L_, "..|.............Bnn...............|..",  15,     2,     NV},
            {L_, "..|.............Bnn...............|..",  16,     2,     NV},
            {L_, "..|.............Bnn...............|..",  17,     1,     NV},
            {L_, "..|...........................Bnn.|..",  27,     1,     NV},
            {L_, "..|...........................Bnn.|..",  28,     0,     28},
            {L_, "..|...........................Bnn.|..",  29,     2,     NV},
            {L_, "..|...........................Bnn.|..",  30,     2,     NV},
            {L_, "..|...........................Bnn.|..",  31,     1,     NV},
            {L_, "..|............................Bnn|..",  28,     1,     NV},
            {L_, "..|............................Bnn|..",  29,     0,     29},
            {L_, "..|............................Bnn|..",  30,     0,     29},
            {L_, "..|............................Bnn|..",  31,     0,     29},
            {L_, "..|............................Bnn|..",  32,     1,     NV},
            {L_, "..|.............................Bn|n.",  29,     1,     NV},
            {L_, "..|.............................Bn|n.",  30,     0,     30},
            {L_, "..|.............................Bn|n.",  31,     0,     30},
            {L_, "..|.............................Bn|n.",  32,     2,     NV},
            {L_, "..|.............................Bn|n.",  33,     1,     NV},
            {L_, "..|..............................B|nn",  30,     1,     NV},
            {L_, "..|..............................B|nn",  31,     0,     31},
            {L_, "..|..............................B|nn",  32,     2,     NV},
            {L_, "..|..............................B|nn",  33,     2,     NV},
            {L_, "..|..............................B|nn",  34,     1,     NV},

            //"BNB".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "Bn|B..............................|..",  -2,     1,     NV},
            {L_, "Bn|B..............................|..",  -1,     0,     -1},
            {L_, "Bn|B..............................|..",   0,     0,     -1},
            {L_, "Bn|B..............................|..",   1,     0,      1},
            {L_, "Bn|B..............................|..",   2,     1,     NV},
            {L_, ".B|nB.............................|..",  -1,     1,     NV},
            {L_, ".B|nB.............................|..",   0,     0,      0},
            {L_, ".B|nB.............................|..",   1,     0,      2},
            {L_, ".B|nB.............................|..",   2,     0,      2},
            {L_, ".B|nB.............................|..",   3,     1,     NV},
            {L_, "..|BnB............................|..",   0,     1,     NV},
            {L_, "..|BnB............................|..",   1,     0,      1},
            {L_, "..|BnB............................|..",   2,     0,      3},
            {L_, "..|BnB............................|..",   3,     0,      3},
            {L_, "..|BnB............................|..",   4,     1,     NV},
            {L_, "..|.BnB...........................|..",   1,     1,     NV},
            {L_, "..|.BnB...........................|..",   2,     0,      2},
            {L_, "..|.BnB...........................|..",   3,     0,      4},
            {L_, "..|.BnB...........................|..",   4,     0,      4},
            {L_, "..|.BnB...........................|..",   5,     1,     NV},
            {L_, "..|.............BnB...............|..",  13,     1,     NV},
            {L_, "..|.............BnB...............|..",  14,     0,     14},
            {L_, "..|.............BnB...............|..",  15,     0,     16},
            {L_, "..|.............BnB...............|..",  16,     0,     16},
            {L_, "..|.............BnB...............|..",  17,     1,     NV},
            {L_, "..|...........................BnB.|..",  27,     1,     NV},
            {L_, "..|...........................BnB.|..",  28,     0,     28},
            {L_, "..|...........................BnB.|..",  29,     0,     30},
            {L_, "..|...........................BnB.|..",  30,     0,     30},
            {L_, "..|...........................BnB.|..",  31,     1,     NV},
            {L_, "..|............................BnB|..",  28,     1,     NV},
            {L_, "..|............................BnB|..",  29,     0,     29},
            {L_, "..|............................BnB|..",  30,     0,     31},
            {L_, "..|............................BnB|..",  31,     0,     31},
            {L_, "..|............................BnB|..",  32,     1,     NV},
            {L_, "..|.............................Bn|B.",  29,     1,     NV},
            {L_, "..|.............................Bn|B.",  30,     0,     30},
            {L_, "..|.............................Bn|B.",  31,     0,     30},
            {L_, "..|.............................Bn|B.",  32,     0,     32},
            {L_, "..|.............................Bn|B.",  33,     1,     NV},
            {L_, "..|..............................B|nB",  30,     1,     NV},
            {L_, "..|..............................B|nB",  31,     0,     31},
            {L_, "..|..............................B|nB",  32,     0,     33},
            {L_, "..|..............................B|nB",  33,     0,     33},
            {L_, "..|..............................B|nB",  34,     1,     NV},

            //"BBN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "BB|n..............................|..",  -2,     1,     NV},
            {L_, "BB|n..............................|..",  -1,     0,     -1},
            {L_, "BB|n..............................|..",   0,     0,      0},
            {L_, "BB|n..............................|..",   1,     2,     NV},
            {L_, "BB|n..............................|..",   2,     1,     NV},
            {L_, ".B|Bn.............................|..",  -1,     1,     NV},
            {L_, ".B|Bn.............................|..",   0,     0,      0},
            {L_, ".B|Bn.............................|..",   1,     0,      1},
            {L_, ".B|Bn.............................|..",   2,     2,     NV},
            {L_, ".B|Bn.............................|..",   3,     1,     NV},
            {L_, "..|BBn............................|..",   0,     1,     NV},
            {L_, "..|BBn............................|..",   1,     0,      1},
            {L_, "..|BBn............................|..",   2,     0,      2},
            {L_, "..|BBn............................|..",   3,     2,     NV},
            {L_, "..|BBn............................|..",   4,     1,     NV},
            {L_, "..|.BBn...........................|..",   1,     1,     NV},
            {L_, "..|.BBn...........................|..",   2,     0,      2},
            {L_, "..|.BBn...........................|..",   3,     0,      3},
            {L_, "..|.BBn...........................|..",   4,     2,     NV},
            {L_, "..|.BBn...........................|..",   5,     1,     NV},
            {L_, "..|.............BBn...............|..",  13,     1,     NV},
            {L_, "..|.............BBn...............|..",  14,     0,     14},
            {L_, "..|.............BBn...............|..",  15,     0,     15},
            {L_, "..|.............BBn...............|..",  16,     2,     NV},
            {L_, "..|.............BBn...............|..",  17,     1,     NV},
            {L_, "..|...........................BBn.|..",  27,     1,     NV},
            {L_, "..|...........................BBn.|..",  28,     0,     28},
            {L_, "..|...........................BBn.|..",  29,     0,     29},
            {L_, "..|...........................BBn.|..",  30,     2,     NV},
            {L_, "..|...........................BBn.|..",  31,     1,     NV},
            {L_, "..|............................BBn|..",  28,     1,     NV},
            {L_, "..|............................BBn|..",  29,     0,     29},
            {L_, "..|............................BBn|..",  30,     0,     30},
            {L_, "..|............................BBn|..",  31,     0,     30},
            {L_, "..|............................BBn|..",  32,     1,     NV},
            {L_, "..|.............................BB|n.",  29,     1,     NV},
            {L_, "..|.............................BB|n.",  30,     0,     30},
            {L_, "..|.............................BB|n.",  31,     0,     31},
            {L_, "..|.............................BB|n.",  32,     2,     NV},
            {L_, "..|.............................BB|n.",  33,     1,     NV},
            {L_, "..|..............................B|Bn",  30,     1,     NV},
            {L_, "..|..............................B|Bn",  31,     0,     31},
            {L_, "..|..............................B|Bn",  32,     0,     32},
            {L_, "..|..............................B|Bn",  33,     2,     NV},
            {L_, "..|..............................B|Bn",  34,     1,     NV},

            //"BBB".
            //LINE   INPUT                            ORIGINAL  STATUS  RESULT
            //----   -----                            --------  ------  ------
            {L_, "BB|B..............................|..",  -2,     1,     NV},
            {L_, "BB|B..............................|..",  -1,     0,     -1},
            {L_, "BB|B..............................|..",   0,     0,      0},
            {L_, "BB|B..............................|..",   1,     0,      1},
            {L_, "BB|B..............................|..",   2,     1,     NV},
            {L_, ".B|BB.............................|..",  -1,     1,     NV},
            {L_, ".B|BB.............................|..",   0,     0,      0},
            {L_, ".B|BB.............................|..",   1,     0,      1},
            {L_, ".B|BB.............................|..",   2,     0,      2},
            {L_, ".B|BB.............................|..",   3,     1,     NV},
            {L_, "..|BBB............................|..",   0,     1,     NV},
            {L_, "..|BBB............................|..",   1,     0,      1},
            {L_, "..|BBB............................|..",   2,     0,      2},
            {L_, "..|BBB............................|..",   3,     0,      3},
            {L_, "..|BBB............................|..",   4,     1,     NV},
            {L_, "..|.BBB...........................|..",   1,     1,     NV},
            {L_, "..|.BBB...........................|..",   2,     0,      2},
            {L_, "..|.BBB...........................|..",   3,     0,      3},
            {L_, "..|.BBB...........................|..",   4,     0,      4},
            {L_, "..|.BBB...........................|..",   5,     1,     NV},
            {L_, "..|.............BBB...............|..",  13,     1,     NV},
            {L_, "..|.............BBB...............|..",  14,     0,     14},
            {L_, "..|.............BBB...............|..",  15,     0,     15},
            {L_, "..|.............BBB...............|..",  16,     0,     16},
            {L_, "..|.............BBB...............|..",  17,     1,     NV},
            {L_, "..|...........................BBB.|..",  27,     1,     NV},
            {L_, "..|...........................BBB.|..",  28,     0,     28},
            {L_, "..|...........................BBB.|..",  29,     0,     29},
            {L_, "..|...........................BBB.|..",  30,     0,     30},
            {L_, "..|...........................BBB.|..",  31,     1,     NV},
            {L_, "..|............................BBB|..",  28,     1,     NV},
            {L_, "..|............................BBB|..",  29,     0,     29},
            {L_, "..|............................BBB|..",  30,     0,     30},
            {L_, "..|............................BBB|..",  31,     0,     31},
            {L_, "..|............................BBB|..",  32,     1,     NV},
            {L_, "..|.............................BB|B.",  29,     1,     NV},
            {L_, "..|.............................BB|B.",  30,     0,     30},
            {L_, "..|.............................BB|B.",  31,     0,     31},
            {L_, "..|.............................BB|B.",  32,     0,     32},
            {L_, "..|.............................BB|B.",  33,     1,     NV},
            {L_, "..|..............................B|BB",  30,     1,     NV},
            {L_, "..|..............................B|BB",  31,     0,     31},
            {L_, "..|..............................B|BB",  32,     0,     32},
            {L_, "..|..............................B|BB",  33,     0,     33},
            {L_, "..|..............................B|BB",  34,     1,     NV},

            //Calendar that covers whole month (Calendar.length() == 35)
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",  15,     0,     15},
            {L_, "BB|BBBBBBBBBBBBBBnBBBBBBBBBBBBBBBB|BB",  15,     0,     16},
            {L_, "BB|BBBBBBBBBBBBBBnnBBBBBBBBBBBBBBB|BB",  15,     0,     17},
            {L_, "BB|BBBBBBBBBBBBBBnnnBBBBBBBBBBBBBB|BB",  15,     0,     18},
            {L_, "BB|BBBBBBBBBBBBBBnnnnBBBBBBBBBBBBB|BB",  15,     0,     19},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnBBBBBBBBBBBB|BB",  15,     0,     20},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnBBBBBBBBBBB|BB",  15,     0,     21},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnBBBBBBBBBB|BB",  15,     0,     22},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnBBBBBBBBB|BB",  15,     0,     23},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnBBBBBBBB|BB",  15,     0,     24},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnBBBBBBB|BB",  15,     0,     25},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnBBBBBB|BB",  15,     0,     26},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnBBBBB|BB",  15,     0,     27},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnBBBB|BB",  15,     0,     28},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnBBB|BB",  15,     0,     29},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnBB|BB",  15,     0,     30},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnB|BB",  15,     0,     31},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnn|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnn|nB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnn|nn",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBnnnnnnnnnnnnnnnnnn|nn",  15,     0,     13},
            {L_, "BB|BBBBBBBBBBBBnnnnnnnnnnnnnnnnnnn|nn",  15,     0,     12},
            {L_, "BB|BBBBBBBBBBBnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,     11},
            {L_, "BB|BBBBBBBBBBnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,     10},
            {L_, "BB|BBBBBBBBBnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      9},
            {L_, "BB|BBBBBBBBnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      8},
            {L_, "BB|BBBBBBBnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      7},
            {L_, "BB|BBBBBBnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      6},
            {L_, "BB|BBBBBnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      5},
            {L_, "BB|BBBBnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      4},
            {L_, "BB|BBBnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      3},
            {L_, "BB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "BB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      1},
            {L_, "BB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     3,     NV},
            {L_, "Bn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     3,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     3,     NV},

            //Corner cases
            //Origin: 15
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     3,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},

            //Origin: 32, it first date of the next month
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},

            //Origin 31
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     3,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},

            //Origin 30
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     3,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},

            //Origin 2
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     3,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},

            //Origin 1
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     3,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      2},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      2},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},

            //Origin  0, i.e last day of the previous m
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0}
        };
        const bdlt::Date ORIGIN = bdlt::Date(1999, 12, 31);

        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);
        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE     = DATA[i].d_line;
            const char       *INPUT    = DATA[i].d_input_p;
            const bdlt::Date  START    = ORIGIN + getStartDate(INPUT);
            const bdlt::Date  ORIGINAL = ORIGIN + DATA[i].d_original;
            bdlt::Date        LOADED   = ORIGIN - 5;
            const int         STATUS   = DATA[i].d_status;
            const bdlt::Date  RESULT   = ORIGIN + DATA[i].d_result;

            bdlt::Calendar calendar = parseCalendar(INPUT, START);
            int            rStatus = Util::shiftModifiedFollowingIfValid(
                                                                     &LOADED,
                                                                     ORIGINAL,
                                                                     calendar);

            if (veryVerbose) {
                T_ P_(LINE) P_(ORIGINAL) P_(RESULT) P_(LOADED);
            }

            // Check status
            ASSERTV(LINE,
                    INPUT,
                    START,
                    ORIGINAL,
                     STATUS,
                    rStatus,
                    rStatus == STATUS);

            // Check loaded
            ASSERTV(LINE,
                    INPUT,
                    START,
                    ORIGINAL,
                    LOADED,
                    RESULT,
                    RESULT == LOADED);
        }

        // negative tests
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Calendar cdr;
            bdlt::Date     date;
            bdlt::Date     result;

            ASSERT_PASS(Util::shiftModifiedFollowingIfValid(&result,
                                                            date,
                                                            cdr));
            ASSERT_FAIL(Util::shiftModifiedFollowingIfValid(0,
                                                            date,
                                                            cdr));
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        //  TESTING 'shiftModifiedPrecedingIfValid'
        //    Ensure 'shiftModifiedPrecedingIfValid' loads proper business day
        //    or returns appropriate status in case of failure.
        //
        // Concerns:
        //: 1 This method returns status with value '1' if 'original' date is
        //:   out of range of a specified calendar, and returns status with
        //:   value '2' if while looking for a business day withing the month,
        //:   boundaries of the valid range of a 'calendar' were exceeded.
        //:
        //: 2 If 'original' date itself is a business day inside of the valid
        //:   range, this method loads it to the 'result' and returns zero.
        //:
        //: 3 If 'original' date is not a business day, this methods loads
        //:   the date of the chronologically latest business day preceding
        //:   'original' date if it can be found within the current month.
        //:
        //: 4 If 'original' date is not a business day and there is no business
        //:   days withing this month that precede 'original' date, this method
        //:   loads chronologically earliest business day following 'original'
        //:   date.
        //:
        //: 5 If 'original' date is not a business day and no business day were
        //:   found within the month of an 'original' date, status with value
        //:   '3' is returned.
        //:
        //: 6 If non-zero value is returned, result remains unchanged.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the table-driven approach, define a representative set of
        //:   valid inputs.  Verify that the function returns the correct
        //:   value.  (C-1..6)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for argument values.  (C-7)
        //
        // Testing:
        //   shiftModifiedPrecedingIfValid(bdlt::Date *result, orig, calendar)
        // --------------------------------------------------------------------

        bslma::TestAllocator         defaultAllocator;
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        if (verbose) cout << endl
                          << "TESTING 'shiftModifiedPrecedingIfValid'" << endl
                          << "=======================================" << endl;

        static const struct {
            int         d_line;      // source line
            const char *d_input_p;   // input values
            int         d_original;  // original date
            int         d_status;    // expected status
            int         d_result;    // expected result
        } DATA[] = {
            // All days represented as an offset from December 31, 1999.

            //Depth: 0
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, ".|...............................|.",  0,       1,      NV},
            {L_, ".|...............................|.",  1,       1,      NV},
            {L_, ".|...............................|.",  2,       1,      NV},

            //Depth: 1
            //Calendar with 1 NON business day.
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, ".|n..............................|.",   0,      1,      NV},
            {L_, ".|n..............................|.",   1,      2,      NV},
            {L_, ".|n..............................|.",   2,      1,      NV},
            {L_, ".|.n.............................|.",   1,      1,      NV},
            {L_, ".|.n.............................|.",   2,      2,      NV},
            {L_, ".|.n.............................|.",   3,      1,      NV},
            {L_, ".|..............n................|.",  14,      1,      NV},
            {L_, ".|..............n................|.",  15,      2,      NV},
            {L_, ".|..............n................|.",  16,      1,      NV},
            {L_, ".|.............................n.|.",  29,      1,      NV},
            {L_, ".|.............................n.|.",  30,      2,      NV},
            {L_, ".|.............................n.|.",  31,      1,      NV},
            {L_, ".|..............................n|.",  30,      1,      NV},
            {L_, ".|..............................n|.",  31,      2,      NV},
            {L_, ".|..............................n|.",  32,      1,      NV},

            //Calendar with 1 business day.
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, ".|B..............................|.",   0,      1,     NV },
            {L_, ".|B..............................|.",   1,      0,      1 },
            {L_, ".|B..............................|.",   2,      1,     NV },
            {L_, ".|.B.............................|.",   1,      1,     NV },
            {L_, ".|.B.............................|.",   2,      0,      2 },
            {L_, ".|.B.............................|.",   3,      1,     NV },
            {L_, ".|..............B................|.",  14,      1,     NV },
            {L_, ".|..............B................|.",  15,      0,     15 },
            {L_, ".|..............B................|.",  16,      1,     NV },
            {L_, ".|.............................B.|.",  29,      1,     NV },
            {L_, ".|.............................B.|.",  30,      0,     30 },
            {L_, ".|.............................B.|.",  31,      1,     NV },
            {L_, ".|..............................B|.",  30,      1,     NV },
            {L_, ".|..............................B|.",  31,      0,     31 },
            {L_, ".|..............................B|.",  32,      1,     NV },

            //Depth: 2
            // Calendar with 2 NON business days.
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "n|n..............................|.",  -1,       1,     NV},
            {L_, "n|n..............................|.",   0,       2,     NV},
            {L_, "n|n..............................|.",   1,       2,     NV},
            {L_, "n|n..............................|.",   2,       1,     NV},
            {L_, ".|nn.............................|.",   0,       1,     NV},
            {L_, ".|nn.............................|.",   1,       2,     NV},
            {L_, ".|nn.............................|.",   2,       2,     NV},
            {L_, ".|nn.............................|.",   3,       1,     NV},
            {L_, ".|.nn............................|.",   1,       1,     NV},
            {L_, ".|.nn............................|.",   2,       2,     NV},
            {L_, ".|.nn............................|.",   3,       2,     NV},
            {L_, ".|.nn............................|.",   4,       1,     NV},
            {L_, ".|.............nn................|.",  13,       1,     NV},
            {L_, ".|.............nn................|.",  14,       2,     NV},
            {L_, ".|.............nn................|.",  15,       2,     NV},
            {L_, ".|.............nn................|.",  16,       1,     NV},
            {L_, ".|............................nn.|.",  28,       1,     NV},
            {L_, ".|............................nn.|.",  29,       2,     NV},
            {L_, ".|............................nn.|.",  30,       2,     NV},
            {L_, ".|............................nn.|.",  31,       1,     NV},
            {L_, ".|.............................nn|.",  29,       1,     NV},
            {L_, ".|.............................nn|.",  30,       2,     NV},
            {L_, ".|.............................nn|.",  31,       2,     NV},
            {L_, ".|.............................nn|.",  32,       1,     NV},
            {L_, ".|..............................n|n",  30,       1,     NV},
            {L_, ".|..............................n|n",  31,       2,     NV},
            {L_, ".|..............................n|n",  32,       2,     NV},
            {L_, ".|..............................n|n",  33,       1,     NV},

            //Calendar with 1 NON Business day, 1 Business day
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "n|B..............................|.",  -1,       1,     NV},
            {L_, "n|B..............................|.",   0,       2,     NV},
            {L_, "n|B..............................|.",   1,       0,      1},
            {L_, "n|B..............................|.",   2,       1,     NV},
            {L_, ".|nB.............................|.",   0,       1,     NV},
            {L_, ".|nB.............................|.",   1,       0,      2},
            {L_, ".|nB.............................|.",   2,       0,      2},
            {L_, ".|nB.............................|.",   3,       1,     NV},
            {L_, ".|.nB............................|.",   1,       1,     NV},
            {L_, ".|.nB............................|.",   2,       2,     NV},
            {L_, ".|.nB............................|.",   3,       0,      3},
            {L_, ".|.nB............................|.",   4,       1,     NV},
            {L_, ".|.............nB................|.",  13,       1,     NV},
            {L_, ".|.............nB................|.",  14,       2,     NV},
            {L_, ".|.............nB................|.",  15,       0,     15},
            {L_, ".|.............nB................|.",  16,       1,     NV},
            {L_, ".|............................nB.|.",  28,       1,     NV},
            {L_, ".|............................nB.|.",  29,       2,     NV},
            {L_, ".|............................nB.|.",  30,       0,     30},
            {L_, ".|............................nB.|.",  31,       1,     NV},
            {L_, ".|.............................nB|.",  29,       1,     NV},
            {L_, ".|.............................nB|.",  30,       2,     NV},
            {L_, ".|.............................nB|.",  31,       0,     31},
            {L_, ".|.............................nB|.",  32,       1,     NV},
            {L_, ".|..............................n|B",  30,       1,     NV},
            {L_, ".|..............................n|B",  31,       2,     NV},
            {L_, ".|..............................n|B",  32,       0,     32},
            {L_, ".|..............................n|B",  33,       1,     NV},

            //Calendar with 1 Business day, 1 NON Business day
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "B|n..............................|.",  -1,       1,     NV},
            {L_, "B|n..............................|.",   0,       0,      0},
            {L_, "B|n..............................|.",   1,       2,     NV},
            {L_, "B|n..............................|.",   2,       1,     NV},
            {L_, ".|Bn.............................|.",   0,       1,     NV},
            {L_, ".|Bn.............................|.",   1,       0,      1},
            {L_, ".|Bn.............................|.",   2,       0,      1},
            {L_, ".|Bn.............................|.",   3,       1,     NV},
            {L_, ".|.Bn............................|.",   1,       1,     NV},
            {L_, ".|.Bn............................|.",   2,       0,      2},
            {L_, ".|.Bn............................|.",   3,       0,      2},
            {L_, ".|.Bn............................|.",   4,       1,     NV},
            {L_, ".|.............Bn................|.",  13,       1,     NV},
            {L_, ".|.............Bn................|.",  14,       0,     14},
            {L_, ".|.............Bn................|.",  15,       0,     14},
            {L_, ".|.............Bn................|.",  16,       1,     NV},
            {L_, ".|............................Bn.|.",  28,       1,     NV},
            {L_, ".|............................Bn.|.",  29,       0,     29},
            {L_, ".|............................Bn.|.",  30,       0,     29},
            {L_, ".|............................Bn.|.",  31,       1,     NV},
            {L_, ".|.............................Bn|.",  29,       1,     NV},
            {L_, ".|.............................Bn|.",  30,       0,     30},
            {L_, ".|.............................Bn|.",  31,       0,     30},
            {L_, ".|.............................Bn|.",  32,       1,     NV},
            {L_, ".|..............................B|n",  30,       1,     NV},
            {L_, ".|..............................B|n",  31,       0,     31},
            {L_, ".|..............................B|n",  32,       2,     NV},
            {L_, ".|..............................B|n",  33,       1,     NV},

            //Calendar with 2 Business Days
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "B|B..............................|.",  -1,       1,     NV},
            {L_, "B|B..............................|.",   0,       0,      0},
            {L_, "B|B..............................|.",   1,       0,      1},
            {L_, "B|B..............................|.",   2,       1,     NV},
            {L_, ".|BB.............................|.",   0,       1,     NV},
            {L_, ".|BB.............................|.",   1,       0,      1},
            {L_, ".|BB.............................|.",   2,       0,      2},
            {L_, ".|BB.............................|.",   3,       1,     NV},
            {L_, ".|.BB............................|.",   1,       1,     NV},
            {L_, ".|.BB............................|.",   2,       0,      2},
            {L_, ".|.BB............................|.",   3,       0,      3},
            {L_, ".|.BB............................|.",   4,       1,     NV},
            {L_, ".|.............BB................|.",  13,       1,     NV},
            {L_, ".|.............BB................|.",  14,       0,     14},
            {L_, ".|.............BB................|.",  15,       0,     15},
            {L_, ".|.............BB................|.",  16,       1,     NV},
            {L_, ".|............................BB.|.",  28,       1,     NV},
            {L_, ".|............................BB.|.",  29,       0,     29},
            {L_, ".|............................BB.|.",  30,       0,     30},
            {L_, ".|............................BB.|.",  31,       1,     NV},
            {L_, ".|.............................BB|.",  29,       1,     NV},
            {L_, ".|.............................BB|.",  30,       0,     30},
            {L_, ".|.............................BB|.",  31,       0,     31},
            {L_, ".|.............................BB|.",  32,       1,     NV},
            {L_, ".|..............................B|B",  30,       1,     NV},
            {L_, ".|..............................B|B",  31,       0,     31},
            {L_, ".|..............................B|B",  32,       0,     32},
            {L_, ".|..............................B|B",  33,       1,     NV},

            //Depth: 3
            //"NNN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|n..............................|..",  -2,     1,     NV},
            {L_, "nn|n..............................|..",  -1,     2,     NV},
            {L_, "nn|n..............................|..",   0,     2,     NV},
            {L_, "nn|n..............................|..",   1,     2,     NV},
            {L_, "nn|n..............................|..",   2,     1,     NV},
            {L_, ".n|nn.............................|..",  -1,     1,     NV},
            {L_, ".n|nn.............................|..",   0,     2,     NV},
            {L_, ".n|nn.............................|..",   1,     2,     NV},
            {L_, ".n|nn.............................|..",   2,     2,     NV},
            {L_, ".n|nn.............................|..",   3,     1,     NV},
            {L_, "..|nnn............................|..",   0,     1,     NV},
            {L_, "..|nnn............................|..",   1,     2,     NV},
            {L_, "..|nnn............................|..",   2,     2,     NV},
            {L_, "..|nnn............................|..",   3,     2,     NV},
            {L_, "..|nnn............................|..",   4,     1,     NV},
            {L_, "..|.nnn...........................|..",   1,     1,     NV},
            {L_, "..|.nnn...........................|..",   2,     2,     NV},
            {L_, "..|.nnn...........................|..",   3,     2,     NV},
            {L_, "..|.nnn...........................|..",   4,     2,     NV},
            {L_, "..|.nnn...........................|..",   5,     1,     NV},
            {L_, "..|.............nnn...............|..",  13,     1,     NV},
            {L_, "..|.............nnn...............|..",  14,     2,     NV},
            {L_, "..|.............nnn...............|..",  15,     2,     NV},
            {L_, "..|.............nnn...............|..",  16,     2,     NV},
            {L_, "..|.............nnn...............|..",  17,     1,     NV},
            {L_, "..|...........................nnn.|..",  27,     1,     NV},
            {L_, "..|...........................nnn.|..",  28,     2,     NV},
            {L_, "..|...........................nnn.|..",  29,     2,     NV},
            {L_, "..|...........................nnn.|..",  30,     2,     NV},
            {L_, "..|...........................nnn.|..",  31,     1,     NV},
            {L_, "..|............................nnn|..",  28,     1,     NV},
            {L_, "..|............................nnn|..",  29,     2,     NV},
            {L_, "..|............................nnn|..",  30,     2,     NV},
            {L_, "..|............................nnn|..",  31,     2,     NV},
            {L_, "..|............................nnn|..",  32,     1,     NV},
            {L_, "..|.............................nn|n.",  29,     1,     NV},
            {L_, "..|.............................nn|n.",  30,     2,     NV},
            {L_, "..|.............................nn|n.",  31,     2,     NV},
            {L_, "..|.............................nn|n.",  32,     2,     NV},
            {L_, "..|.............................nn|n.",  33,     1,     NV},
            {L_, "..|..............................n|nn",  30,     1,     NV},
            {L_, "..|..............................n|nn",  31,     2,     NV},
            {L_, "..|..............................n|nn",  32,     2,     NV},
            {L_, "..|..............................n|nn",  33,     2,     NV},
            {L_, "..|..............................n|nn",  34,     1,     NV},

            //"NNB".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|B..............................|..",  -2,     1,     NV},
            {L_, "nn|B..............................|..",  -1,     2,     NV},
            {L_, "nn|B..............................|..",   0,     2,     NV},
            {L_, "nn|B..............................|..",   1,     0,      1},
            {L_, "nn|B..............................|..",   2,     1,     NV},
            {L_, ".n|nB.............................|..",  -1,     1,     NV},
            {L_, ".n|nB.............................|..",   0,     2,     NV},
            {L_, ".n|nB.............................|..",   1,     0,      2},
            {L_, ".n|nB.............................|..",   2,     0,      2},
            {L_, ".n|nB.............................|..",   3,     1,     NV},
            {L_, "..|nnB............................|..",   0,     1,     NV},
            {L_, "..|nnB............................|..",   1,     0,      3},
            {L_, "..|nnB............................|..",   2,     0,      3},
            {L_, "..|nnB............................|..",   3,     0,      3},
            {L_, "..|nnB............................|..",   4,     1,     NV},
            {L_, "..|.nnB...........................|..",   1,     1,     NV},
            {L_, "..|.nnB...........................|..",   2,     2,     NV},
            {L_, "..|.nnB...........................|..",   3,     2,     NV},
            {L_, "..|.nnB...........................|..",   4,     0,      4},
            {L_, "..|.nnB...........................|..",   5,     1,     NV},
            {L_, "..|.............nnB...............|..",  13,     1,     NV},
            {L_, "..|.............nnB...............|..",  14,     2,     NV},
            {L_, "..|.............nnB...............|..",  15,     2,     NV},
            {L_, "..|.............nnB...............|..",  16,     0,     16},
            {L_, "..|.............nnB...............|..",  17,     1,     NV},
            {L_, "..|...........................nnB.|..",  27,     1,     NV},
            {L_, "..|...........................nnB.|..",  28,     2,     NV},
            {L_, "..|...........................nnB.|..",  29,     2,     NV},
            {L_, "..|...........................nnB.|..",  30,     0,     30},
            {L_, "..|...........................nnB.|..",  31,     1,     NV},
            {L_, "..|............................nnB|..",  28,     1,     NV},
            {L_, "..|............................nnB|..",  29,     2,     NV},
            {L_, "..|............................nnB|..",  30,     2,     NV},
            {L_, "..|............................nnB|..",  31,     0,     31},
            {L_, "..|............................nnB|..",  32,     1,     NV},
            {L_, "..|.............................nn|B.",  29,     1,     NV},
            {L_, "..|.............................nn|B.",  30,     2,     NV},
            {L_, "..|.............................nn|B.",  31,     2,     NV},
            {L_, "..|.............................nn|B.",  32,     0,     32},
            {L_, "..|.............................nn|B.",  33,     1,     NV},
            {L_, "..|..............................n|nB",  30,     1,     NV},
            {L_, "..|..............................n|nB",  31,     2,     NV},
            {L_, "..|..............................n|nB",  32,     0,     33},
            {L_, "..|..............................n|nB",  33,     0,     33},
            {L_, "..|..............................n|nB",  34,     1,     NV},

            //"NBN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nB|n..............................|..",  -2,     1,     NV},
            {L_, "nB|n..............................|..",  -1,     2,     NV},
            {L_, "nB|n..............................|..",   0,     0,      0},
            {L_, "nB|n..............................|..",   1,     2,     NV},
            {L_, "nB|n..............................|..",   2,     1,     NV},
            {L_, ".n|Bn.............................|..",  -1,     1,     NV},
            {L_, ".n|Bn.............................|..",   0,     2,     NV},
            {L_, ".n|Bn.............................|..",   1,     0,      1},
            {L_, ".n|Bn.............................|..",   2,     0,      1},
            {L_, ".n|Bn.............................|..",   3,     1,     NV},
            {L_, "..|nBn............................|..",   0,     1,     NV},
            {L_, "..|nBn............................|..",   1,     0,      2},
            {L_, "..|nBn............................|..",   2,     0,      2},
            {L_, "..|nBn............................|..",   3,     0,      2},
            {L_, "..|nBn............................|..",   4,     1,     NV},
            {L_, "..|.nBn...........................|..",   1,     1,     NV},
            {L_, "..|.nBn...........................|..",   2,     2,     NV},
            {L_, "..|.nBn...........................|..",   3,     0,      3},
            {L_, "..|.nBn...........................|..",   4,     0,      3},
            {L_, "..|.nBn...........................|..",   5,     1,     NV},
            {L_, "..|.............nBn...............|..",  13,     1,     NV},
            {L_, "..|.............nBn...............|..",  14,     2,     NV},
            {L_, "..|.............nBn...............|..",  15,     0,     15},
            {L_, "..|.............nBn...............|..",  16,     0,     15},
            {L_, "..|.............nBn...............|..",  17,     1,     NV},
            {L_, "..|...........................nBn.|..",  27,     1,     NV},
            {L_, "..|...........................nBn.|..",  28,     2,     NV},
            {L_, "..|...........................nBn.|..",  29,     0,     29},
            {L_, "..|...........................nBn.|..",  30,     0,     29},
            {L_, "..|...........................nBn.|..",  31,     1,     NV},
            {L_, "..|............................nBn|..",  28,     1,     NV},
            {L_, "..|............................nBn|..",  29,     2,     NV},
            {L_, "..|............................nBn|..",  30,     0,     30},
            {L_, "..|............................nBn|..",  31,     0,     30},
            {L_, "..|............................nBn|..",  32,     1,     NV},
            {L_, "..|.............................nB|n.",  29,     1,     NV},
            {L_, "..|.............................nB|n.",  30,     2,     NV},
            {L_, "..|.............................nB|n.",  31,     0,     31},
            {L_, "..|.............................nB|n.",  32,     2,     NV},
            {L_, "..|.............................nB|n.",  33,     1,     NV},
            {L_, "..|..............................n|Bn",  30,     1,     NV},
            {L_, "..|..............................n|Bn",  31,     2,     NV},
            {L_, "..|..............................n|Bn",  32,     0,     32},
            {L_, "..|..............................n|Bn",  33,     0,     32},
            {L_, "..|..............................n|Bn",  34,     1,     NV},

            //"NBB".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nB|B..............................|..",  -2,     1,     NV},
            {L_, "nB|B..............................|..",  -1,     2,     NV},
            {L_, "nB|B..............................|..",   0,     0,      0},
            {L_, "nB|B..............................|..",   1,     0,      1},
            {L_, "nB|B..............................|..",   2,     1,     NV},
            {L_, ".n|BB.............................|..",  -1,     1,     NV},
            {L_, ".n|BB.............................|..",   0,     2,     NV},
            {L_, ".n|BB.............................|..",   1,     0,      1},
            {L_, ".n|BB.............................|..",   2,     0,      2},
            {L_, ".n|BB.............................|..",   3,     1,     NV},
            {L_, "..|nBB............................|..",   0,     1,     NV},
            {L_, "..|nBB............................|..",   1,     0,      2},
            {L_, "..|nBB............................|..",   2,     0,      2},
            {L_, "..|nBB............................|..",   3,     0,      3},
            {L_, "..|nBB............................|..",   4,     1,     NV},
            {L_, "..|.nBB...........................|..",   1,     1,     NV},
            {L_, "..|.nBB...........................|..",   2,     2,     NV},
            {L_, "..|.nBB...........................|..",   3,     0,      3},
            {L_, "..|.nBB...........................|..",   4,     0,      4},
            {L_, "..|.nBB...........................|..",   5,     1,     NV},
            {L_, "..|.............nBB...............|..",  13,     1,     NV},
            {L_, "..|.............nBB...............|..",  14,     2,     NV},
            {L_, "..|.............nBB...............|..",  15,     0,     15},
            {L_, "..|.............nBB...............|..",  16,     0,     16},
            {L_, "..|.............nBB...............|..",  17,     1,     NV},
            {L_, "..|...........................nBB.|..",  27,     1,     NV},
            {L_, "..|...........................nBB.|..",  28,     2,     NV},
            {L_, "..|...........................nBB.|..",  29,     0,     29},
            {L_, "..|...........................nBB.|..",  30,     0,     30},
            {L_, "..|...........................nBB.|..",  31,     1,     NV},
            {L_, "..|............................nBB|..",  28,     1,     NV},
            {L_, "..|............................nBB|..",  29,     2,     NV},
            {L_, "..|............................nBB|..",  30,     0,     30},
            {L_, "..|............................nBB|..",  31,     0,     31},
            {L_, "..|............................nBB|..",  32,     1,     NV},
            {L_, "..|.............................nB|B.",  29,     1,     NV},
            {L_, "..|.............................nB|B.",  30,     2,     NV},
            {L_, "..|.............................nB|B.",  31,     0,     31},
            {L_, "..|.............................nB|B.",  32,     0,     32},
            {L_, "..|.............................nB|B.",  33,     1,     NV},
            {L_, "..|..............................n|BB",  30,     1,     NV},

            //"BNN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "Bn|n..............................|..",  -2,     1,     NV},
            {L_, "Bn|n..............................|..",  -1,     0,     -1},
            {L_, "Bn|n..............................|..",   0,     0,     -1},
            {L_, "Bn|n..............................|..",   1,     2,     NV},
            {L_, "Bn|n..............................|..",   2,     1,     NV},
            {L_, ".B|nn.............................|..",  -1,     1,     NV},
            {L_, ".B|nn.............................|..",   0,     0,      0},
            {L_, ".B|nn.............................|..",   1,     2,     NV},
            {L_, ".B|nn.............................|..",   2,     2,     NV},
            {L_, ".B|nn.............................|..",   3,     1,     NV},
            {L_, "..|Bnn............................|..",   0,     1,     NV},
            {L_, "..|Bnn............................|..",   1,     0,      1},
            {L_, "..|Bnn............................|..",   2,     0,      1},
            {L_, "..|Bnn............................|..",   3,     0,      1},
            {L_, "..|Bnn............................|..",   4,     1,     NV},
            {L_, "..|.Bnn...........................|..",   1,     1,     NV},
            {L_, "..|.Bnn...........................|..",   2,     0,      2},
            {L_, "..|.Bnn...........................|..",   3,     0,      2},
            {L_, "..|.Bnn...........................|..",   4,     0,      2},
            {L_, "..|.Bnn...........................|..",   5,     1,     NV},
            {L_, "..|.............Bnn...............|..",  13,     1,     NV},
            {L_, "..|.............Bnn...............|..",  14,     0,     14},
            {L_, "..|.............Bnn...............|..",  15,     0,     14},
            {L_, "..|.............Bnn...............|..",  16,     0,     14},
            {L_, "..|.............Bnn...............|..",  17,     1,     NV},
            {L_, "..|...........................Bnn.|..",  27,     1,     NV},
            {L_, "..|...........................Bnn.|..",  28,     0,     28},
            {L_, "..|...........................Bnn.|..",  29,     0,     28},
            {L_, "..|...........................Bnn.|..",  30,     0,     28},
            {L_, "..|...........................Bnn.|..",  31,     1,     NV},
            {L_, "..|............................Bnn|..",  28,     1,     NV},
            {L_, "..|............................Bnn|..",  29,     0,     29},
            {L_, "..|............................Bnn|..",  30,     0,     29},
            {L_, "..|............................Bnn|..",  31,     0,     29},
            {L_, "..|............................Bnn|..",  32,     1,     NV},
            {L_, "..|.............................Bn|n.",  29,     1,     NV},
            {L_, "..|.............................Bn|n.",  30,     0,     30},
            {L_, "..|.............................Bn|n.",  31,     0,     30},
            {L_, "..|.............................Bn|n.",  32,     2,     NV},
            {L_, "..|.............................Bn|n.",  33,     1,     NV},
            {L_, "..|..............................B|nn",  30,     1,     NV},
            {L_, "..|..............................B|nn",  31,     0,     31},
            {L_, "..|..............................B|nn",  32,     2,     NV},
            {L_, "..|..............................B|nn",  33,     2,     NV},
            {L_, "..|..............................B|nn",  34,     1,     NV},

            //"BNB".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "Bn|B..............................|..",  -2,     1,     NV},
            {L_, "Bn|B..............................|..",  -1,     0,     -1},
            {L_, "Bn|B..............................|..",   0,     0,     -1},
            {L_, "Bn|B..............................|..",   1,     0,      1},
            {L_, "Bn|B..............................|..",   2,     1,     NV},
            {L_, ".B|nB.............................|..",  -1,     1,     NV},
            {L_, ".B|nB.............................|..",   0,     0,      0},
            {L_, ".B|nB.............................|..",   1,     0,      2},
            {L_, ".B|nB.............................|..",   2,     0,      2},
            {L_, ".B|nB.............................|..",   3,     1,     NV},
            {L_, "..|BnB............................|..",   0,     1,     NV},
            {L_, "..|BnB............................|..",   1,     0,      1},
            {L_, "..|BnB............................|..",   2,     0,      1},
            {L_, "..|BnB............................|..",   3,     0,      3},
            {L_, "..|BnB............................|..",   4,     1,     NV},
            {L_, "..|.BnB...........................|..",   1,     1,     NV},
            {L_, "..|.BnB...........................|..",   2,     0,      2},
            {L_, "..|.BnB...........................|..",   3,     0,      2},
            {L_, "..|.BnB...........................|..",   4,     0,      4},
            {L_, "..|.BnB...........................|..",   5,     1,     NV},
            {L_, "..|.............BnB...............|..",  13,     1,     NV},
            {L_, "..|.............BnB...............|..",  14,     0,     14},
            {L_, "..|.............BnB...............|..",  15,     0,     14},
            {L_, "..|.............BnB...............|..",  16,     0,     16},
            {L_, "..|.............BnB...............|..",  17,     1,     NV},
            {L_, "..|...........................BnB.|..",  27,     1,     NV},
            {L_, "..|...........................BnB.|..",  28,     0,     28},
            {L_, "..|...........................BnB.|..",  29,     0,     28},
            {L_, "..|...........................BnB.|..",  30,     0,     30},
            {L_, "..|...........................BnB.|..",  31,     1,     NV},
            {L_, "..|............................BnB|..",  28,     1,     NV},
            {L_, "..|............................BnB|..",  29,     0,     29},
            {L_, "..|............................BnB|..",  30,     0,     29},
            {L_, "..|............................BnB|..",  31,     0,     31},
            {L_, "..|............................BnB|..",  32,     1,     NV},
            {L_, "..|.............................Bn|B.",  29,     1,     NV},
            {L_, "..|.............................Bn|B.",  30,     0,     30},
            {L_, "..|.............................Bn|B.",  31,     0,     30},
            {L_, "..|.............................Bn|B.",  32,     0,     32},
            {L_, "..|.............................Bn|B.",  33,     1,     NV},
            {L_, "..|..............................B|nB",  30,     1,     NV},
            {L_, "..|..............................B|nB",  31,     0,     31},
            {L_, "..|..............................B|nB",  32,     0,     33},
            {L_, "..|..............................B|nB",  33,     0,     33},
            {L_, "..|..............................B|nB",  34,     1,     NV},

            //"BBN".
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "BB|n..............................|..",  -2,     1,     NV},
            {L_, "BB|n..............................|..",  -1,     0,     -1},
            {L_, "BB|n..............................|..",   0,     0,      0},
            {L_, "BB|n..............................|..",   1,     2,     NV},
            {L_, "BB|n..............................|..",   2,     1,     NV},
            {L_, ".B|Bn.............................|..",  -1,     1,     NV},
            {L_, ".B|Bn.............................|..",   0,     0,      0},
            {L_, ".B|Bn.............................|..",   1,     0,      1},
            {L_, ".B|Bn.............................|..",   2,     0,      1},
            {L_, ".B|Bn.............................|..",   3,     1,     NV},
            {L_, "..|BBn............................|..",   0,     1,     NV},
            {L_, "..|BBn............................|..",   1,     0,      1},
            {L_, "..|BBn............................|..",   2,     0,      2},
            {L_, "..|BBn............................|..",   3,     0,      2},
            {L_, "..|BBn............................|..",   4,     1,     NV},
            {L_, "..|.BBn...........................|..",   1,     1,     NV},
            {L_, "..|.BBn...........................|..",   2,     0,      2},
            {L_, "..|.BBn...........................|..",   3,     0,      3},
            {L_, "..|.BBn...........................|..",   4,     0,      3},
            {L_, "..|.BBn...........................|..",   5,     1,     NV},
            {L_, "..|.............BBn...............|..",  13,     1,     NV},
            {L_, "..|.............BBn...............|..",  14,     0,     14},
            {L_, "..|.............BBn...............|..",  15,     0,     15},
            {L_, "..|.............BBn...............|..",  16,     0,     15},
            {L_, "..|.............BBn...............|..",  17,     1,     NV},
            {L_, "..|...........................BBn.|..",  27,     1,     NV},
            {L_, "..|...........................BBn.|..",  28,     0,     28},
            {L_, "..|...........................BBn.|..",  29,     0,     29},
            {L_, "..|...........................BBn.|..",  30,     0,     29},
            {L_, "..|...........................BBn.|..",  31,     1,     NV},
            {L_, "..|............................BBn|..",  28,     1,     NV},
            {L_, "..|............................BBn|..",  29,     0,     29},
            {L_, "..|............................BBn|..",  30,     0,     30},
            {L_, "..|............................BBn|..",  31,     0,     30},
            {L_, "..|............................BBn|..",  32,     1,     NV},
            {L_, "..|.............................BB|n.",  29,     1,     NV},
            {L_, "..|.............................BB|n.",  30,     0,     30},
            {L_, "..|.............................BB|n.",  31,     0,     31},
            {L_, "..|.............................BB|n.",  32,     2,     NV},
            {L_, "..|.............................BB|n.",  33,     1,     NV},
            {L_, "..|..............................B|Bn",  30,     1,     NV},
            {L_, "..|..............................B|Bn",  31,     0,     31},
            {L_, "..|..............................B|Bn",  32,     0,     32},
            {L_, "..|..............................B|Bn",  33,     0,     32},
            {L_, "..|..............................B|Bn",  34,     1,     NV},

            //"BBB".
            //LINE   INPUT                            ORIGINAL  STATUS  RESULT
            //----   -----                            --------  ------  ------
            {L_, "BB|B..............................|..",  -2,     1,     NV},
            {L_, "BB|B..............................|..",  -1,     0,     -1},
            {L_, "BB|B..............................|..",   0,     0,      0},
            {L_, "BB|B..............................|..",   1,     0,      1},
            {L_, "BB|B..............................|..",   2,     1,     NV},
            {L_, ".B|BB.............................|..",  -1,     1,     NV},
            {L_, ".B|BB.............................|..",   0,     0,      0},
            {L_, ".B|BB.............................|..",   1,     0,      1},
            {L_, ".B|BB.............................|..",   2,     0,      2},
            {L_, ".B|BB.............................|..",   3,     1,     NV},
            {L_, "..|BBB............................|..",   0,     1,     NV},
            {L_, "..|BBB............................|..",   1,     0,      1},
            {L_, "..|BBB............................|..",   2,     0,      2},
            {L_, "..|BBB............................|..",   3,     0,      3},
            {L_, "..|BBB............................|..",   4,     1,     NV},
            {L_, "..|.BBB...........................|..",   1,     1,     NV},
            {L_, "..|.BBB...........................|..",   2,     0,      2},
            {L_, "..|.BBB...........................|..",   3,     0,      3},
            {L_, "..|.BBB...........................|..",   4,     0,      4},
            {L_, "..|.BBB...........................|..",   5,     1,     NV},
            {L_, "..|.............BBB...............|..",  13,     1,     NV},
            {L_, "..|.............BBB...............|..",  14,     0,     14},
            {L_, "..|.............BBB...............|..",  15,     0,     15},
            {L_, "..|.............BBB...............|..",  16,     0,     16},
            {L_, "..|.............BBB...............|..",  17,     1,     NV},
            {L_, "..|...........................BBB.|..",  27,     1,     NV},
            {L_, "..|...........................BBB.|..",  28,     0,     28},
            {L_, "..|...........................BBB.|..",  29,     0,     29},
            {L_, "..|...........................BBB.|..",  30,     0,     30},
            {L_, "..|...........................BBB.|..",  31,     1,     NV},
            {L_, "..|............................BBB|..",  28,     1,     NV},
            {L_, "..|............................BBB|..",  29,     0,     29},
            {L_, "..|............................BBB|..",  30,     0,     30},
            {L_, "..|............................BBB|..",  31,     0,     31},
            {L_, "..|............................BBB|..",  32,     1,     NV},
            {L_, "..|.............................BB|B.",  29,     1,     NV},
            {L_, "..|.............................BB|B.",  30,     0,     30},
            {L_, "..|.............................BB|B.",  31,     0,     31},
            {L_, "..|.............................BB|B.",  32,     0,     32},
            {L_, "..|.............................BB|B.",  33,     1,     NV},
            {L_, "..|..............................B|BB",  30,     1,     NV},
            {L_, "..|..............................B|BB",  31,     0,     31},
            {L_, "..|..............................B|BB",  32,     0,     32},
            {L_, "..|..............................B|BB",  33,     0,     33},
            {L_, "..|..............................B|BB",  34,     1,     NV},

            //Calendar that covers whole month (Calendar.length() == 35)
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "BB|BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB|BB",  15,     0,     15},
            {L_, "BB|BBBBBBBBBBBBBBnBBBBBBBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnBBBBBBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnBBBBBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnBBBBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnBBBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnBBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnBBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnBBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnBBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnBBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnBBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnBBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnBBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnBBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnBB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnB|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnn|BB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnn|nB",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBBnnnnnnnnnnnnnnnnn|nn",  15,     0,     14},
            {L_, "BB|BBBBBBBBBBBBBnnnnnnnnnnnnnnnnnn|nn",  15,     0,     13},
            {L_, "BB|BBBBBBBBBBBBnnnnnnnnnnnnnnnnnnn|nn",  15,     0,     12},
            {L_, "BB|BBBBBBBBBBBnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,     11},
            {L_, "BB|BBBBBBBBBBnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,     10},
            {L_, "BB|BBBBBBBBBnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      9},
            {L_, "BB|BBBBBBBBnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      8},
            {L_, "BB|BBBBBBBnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      7},
            {L_, "BB|BBBBBBnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      6},
            {L_, "BB|BBBBBnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      5},
            {L_, "BB|BBBBnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      4},
            {L_, "BB|BBBnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      3},
            {L_, "BB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "BB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      1},
            {L_, "BB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     3,     NV},
            {L_, "Bn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     3,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     3,     NV},

            //Corner cases
            //Origin: 15
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     3,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,      2},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,      2},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,      2},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  15,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  15,     0,      2},

            //Origin: 32, it first date of the next month
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  32,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  32,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  32,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  32,     0,     32},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  32,     2,     NV},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  32,     0,     32},

            //Origin 31
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     3,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  31,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  31,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  31,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  31,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  31,     0,     31},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  31,     0,     31},

            //Origin 30
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     3,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",  30,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",  30,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",  30,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",  30,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",  30,     0,     30},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",  30,     0,     30},

            //Origin 2
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     3,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   2,     0,      2},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   2,     0,      2},

            //Origin 1
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     3,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,     31},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,     30},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,     30},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      2},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      2},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     3,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,     31},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,     30},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,     30},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      2},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      2},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   1,     0,      1},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   1,     0,      1},

            //Origin  0, i.e last day of the previous m
            //LN INPUT                                ORIGINAL  STATUS  RESULT
            //-- -----                                --------  ------  ------
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     2,     NV},
            {L_, "nn|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     2,     NV},
            {L_, "nn|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     2,     NV},
            {L_, "nn|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     2,     NV},
            {L_, "nn|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     2,     NV},
            {L_, "nn|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     2,     NV},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|nnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|nBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|Bnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|BnnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnn|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnnB|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBn|Bn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|nn",   0,     0,      0},
            {L_, "nB|BBnnnnnnnnnnnnnnnnnnnnnnnnnnnBB|Bn",   0,     0,      0}
        };
        const bdlt::Date ORIGIN = bdlt::Date(1999, 12, 31);

        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);
        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE     = DATA[i].d_line;
            const char       *INPUT    = DATA[i].d_input_p;
            const bdlt::Date  START    = ORIGIN + getStartDate(INPUT);
            const bdlt::Date  ORIGINAL = ORIGIN + DATA[i].d_original;
            bdlt::Date        LOADED   = ORIGIN - 5;
            const int         STATUS   = DATA[i].d_status;
            const bdlt::Date  RESULT   = ORIGIN + DATA[i].d_result;

            bdlt::Calendar calendar = parseCalendar(INPUT, START);

            int rStatus = Util::shiftModifiedPrecedingIfValid(&LOADED,
                                                              ORIGINAL,
                                                              calendar);

            if (veryVerbose) {
                T_ P_(LINE) P_(ORIGINAL) P_(RESULT) P_(LOADED);
            }

            // Check status
            ASSERTV(LINE,
                    INPUT,
                    START,
                    ORIGINAL,
                    STATUS,
                    rStatus,
                    rStatus == STATUS);

            // Check loaded
            ASSERTV(LINE,
                    INPUT,
                    START,
                    ORIGINAL,
                    LOADED,
                    RESULT,
                    RESULT == LOADED);
        }

        // negative tests
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            bdlt::Calendar cdr;
            bdlt::Date     date;
            bdlt::Date     result;

            ASSERT_PASS(Util::shiftModifiedPrecedingIfValid(&result,
                                                            date,
                                                            cdr));
            ASSERT_FAIL(Util::shiftModifiedPrecedingIfValid(0,
                                                            date,
                                                            cdr));
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS 'getStartDate'
        //   Ensure that 'getStartDate' reads any arbitrary sequence of
        //   characters that represent a calendar and returns the start date of
        //   the calendar relative to the beginning of the month.
        //
        // Concerns:
        //: 1 Function behaves correctly regardless of wether there are
        //:   business days or non-business days before the month.
        //:
        //: 2 Function returns '999' when the sequence that get passed
        //:    is empty or does not contain any valid days.
        //
        // Plan:
        //: 1 Use the table-driven approach, define a representative set of
        //:   valid inputs.  Verify that the function returns the correct
        //:   value.  (C-1, C-2)
        //
        // Testing:
        //  getStartDate(const char *)
        // --------------------------------------------------------------------

        bslma::TestAllocator         defaultAllocator;
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        if (verbose) cout << endl
                          << "TEST APPARATUS 'getStartDate'" << endl
                          << "=============================" << endl;

        static const struct {
            int         d_line;     // source line
            const char *d_input_p;  // input values
            int         d_result;   // expected result
        } DATA[] = {
            // All days represented as an offset from December 31, 1999.

            //Depth: 0
            //LN INPUT                                 RESULT
            //-- -----                                 ------
            {L_, "..|...............................|.", 999  },
            {L_, "n.|...............................|.",  -1  },
            {L_, "B.|...............................|.",  -1  },
            {L_, "nB|...............................|.",  -1  },
            {L_, "BB|...............................|.",  -1  },
            {L_, "BB|n..............................|.",  -1  },
            {L_, "BB|B..............................|.",  -1  },
            {L_, "BB|nB.............................|.",  -1  },
            {L_, "BB|BB.............................|.",  -1  },
            {L_, ".B|BB.............................|.",   0  },
            {L_, ".n|BB.............................|.",   0  },
            {L_, "..|BB.............................|.",   1  },
            {L_, "..|nB.............................|.",   1  },
            {L_, "..|.B.............................|.",   2  },
            {L_, "..|..............................n|.",  31  },
            {L_, "..|..............................B|.",  31  },
            {L_, "..|..............................B|n",  31  },
            {L_, "..|..............................n|n",  31  },
            {L_, "..|..............................n|B",  31  },
            {L_, "..|..............................n|n",  31  }
        };

        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_line;
            const char *INPUT  = DATA[i].d_input_p;
            const int   RESULT = DATA[i].d_result;

            const int rDate = getStartDate(INPUT);

            if (veryVerbose) {
                T_ P_(rDate) P_(INPUT) P_(RESULT);
            }

            // Check date
            ASSERTV(LINE, INPUT, rDate, rDate == RESULT);
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TEST APPARATUS 'parseCalendar'
        //   Ensure that 'parseCalendar' parses the any arbitrary sequence of
        //   characters that represent a calendar.
        //
        // Concerns:
        //: 1 Function parses input correctly regardless of a size and a
        //:    content of the input sequence.
        //
        // Plan:
        //: 1 Use the table-driven approach, define a representative set of
        //:   valid inputs.  Verify that the function returns the correct
        //:   value.  (C-1, C-2)
        //
        // Testing:
        //   parseCalendar(const char *, const bdlt::Date&)
        // --------------------------------------------------------------------
        if (verbose) {
            cout << endl
                 << "TEST APPARATUS 'parseCalendar'" << endl
                 << "==============================" << endl;
        }

        static const struct {
            int         d_line;          // source line
            const char *d_input_p;       // input values
            int         d_lenght;        // length of calendar
            bdlt::Date  d_start;         // start date
            bdlt::Date  d_end;           // end date
            int         d_businessDays;  // number of business days
        } DATA[] = {
            {L_, "",                                   0,
                  bdlt::Date(9999, 12, 31), bdlt::Date(1, 1, 1), 0},
            {L_, "n",                                  1,
                  bdlt::Date(2014, 4, 1),   bdlt::Date(2014, 4, 1), 0},
            {L_, "nnnnnnnnnnnnnnnnnnnnnnnnnnnnnn",    30,
                  bdlt::Date(2014, 4, 1),   bdlt::Date(2014, 4, 30), 0},
            {L_, "nnnnnnnnnnnnnnnnBnnnnnnnnnnnnn",    30,
                  bdlt::Date(2014, 4, 1),   bdlt::Date(2014, 4, 30), 1},
            {L_, "BnnnnnnnnnnnnnnnnnnnnnnnnnnnnB",    30,
                  bdlt::Date(2014, 4, 1),   bdlt::Date(2014, 4, 30), 2},
            {L_, "BnnnnnnnnnnnnnnnnnnnnnnnnnnB",      28,
                  bdlt::Date(2014, 4, 3),   bdlt::Date(2014, 4, 30), 2},
            {L_, "BBnnnnnnnnnnnnnnnnnnnnnnnnnB",      28,
                  bdlt::Date(2014, 4, 3),   bdlt::Date(2014, 4, 30), 3},
            {L_, "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",    30,
                  bdlt::Date(2014, 4, 1),   bdlt::Date(2014, 4, 30), 30},
            {L_, "BBBBBBBBBBBBBnBBBBBBBBBBBBBBBB",    30,
                  bdlt::Date(2014, 4, 1),   bdlt::Date(2014, 4, 30), 29},
            {L_, "nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn",  32,
                  bdlt::Date(2014, 3, 30),  bdlt::Date(2014, 4, 30), 0},
            {L_, "nnnnnnnnnnnnnnnnBnnnnnnnnnnnnnnnn", 33,
                  bdlt::Date(2014, 4, 1),   bdlt::Date(2014, 5, 3), 1},
        };

        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);
        for (int i = 0; i < NUM_DATA; ++i) {
            const int       LINE   = DATA[i].d_line;
            const char     *INPUT  = DATA[i].d_input_p;
            const int       LENGTH = DATA[i].d_lenght;
            bdlt::Date      START  = DATA[i].d_start;
            bdlt::Date      END    = DATA[i].d_end;
            const int       BDAYS  = DATA[i].d_businessDays;
            bdlt::Calendar  rval   = parseCalendar(INPUT, START);

            if (veryVerbose) {
                T_ P_(LINE) P_(BDAYS) P_(rval);
            }

            // check if business days match
            ASSERTV(LINE,
                    INPUT,
                    LENGTH,
                    BDAYS,
                    rval,
                    rval.numBusinessDays(),
                    rval.numBusinessDays() == BDAYS);

            if (LENGTH) {
                // check if start date match
                ASSERTV(LINE,
                        INPUT,
                        LENGTH,
                        BDAYS,
                        rval,
                        rval.firstDate(),
                        rval.firstDate() == START);

                // check if last date match
                ASSERTV(LINE,
                        INPUT,
                        LENGTH,
                        BDAYS,
                        rval,
                        rval.lastDate(),
                        rval.lastDate() == END);
            }

            // check if length match
            ASSERTV(LINE,
                    INPUT,
                    LENGTH,
                    BDAYS,
                    rval,
                    rval.length(),
                    rval.length() == LENGTH);
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      } break;
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus
                  << "." << bsl::endl;
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
