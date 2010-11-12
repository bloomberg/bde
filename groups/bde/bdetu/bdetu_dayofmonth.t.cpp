// bdetu_dayofmonth.t.cpp    -*-C++-*-

#include <bdetu_dayofmonth.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This component provides a suite of utility functions that perform
// day of month manipulations on 'bdet_Date' objects.  Category partitioning is
// used as the data selection mechanism.  Additionally, a white box test
// approach is taken for independent functions and a black box for functions
// that simply forward to the independent functions.  The independent functions
// are tested first followed by the dependent functions.  Each function
// provided by this component is tested in a separate test case.
//-----------------------------------------------------------------------------
// FUNCTIONS
// [ 2] int numDaysInRange(dayOfMonth, date1, date2);
// [ 3] bdetDate nextDay(dayOfMonth, initialDate);
// [ 6] bdetDate ceilDay(dayOfMonth, initialDate);
// [ 4] bdetDate previousDay(dayOfMonth, initialDate);
// [ 7] bdetDate floorDay(dayOfMonth, initialDate);
// [ 5] bdetDate adjustDay(dayOfMonth, initialDate, count);
// [11] bdetDate ceilAdjustDay(dayOfMonth, initialDate, count);
// [10] bdetDate floorAdjustDay(dayOfMonth, initialDate, count);
// [ 9] bdetDate ceilOrPreviousDayInMonth(dayOfMonth, initialDate);
// [ 8] bdetDate floorOrNextDayInMonth(dayOfMonth, initialDate);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
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
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bdetu_DayOfMonth  Util;

typedef bdet_Date         Date;

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage example provided in the component header
        //   file.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //
        // Testing:
        //   Usage example.
        // --------------------------------------------------------------------



        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;
        // TBD
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'ceilAdjustDay' FUNCTION
        //
        // Concerns:
        //   1. When count = 0, the result date is same as 'ceilDay'.
        //   2. When initialDate.day() <= 'dayOfMonth', the counting should
        //      start from 'dayOfMonth' in initialDate.month().
        //   3. When initialDate.day() > 'dayOfMonth', the counting should
        //      start from 'dayOfMonth' in initialDate.month() + 1.
        //
        // Plan:
        //   Use a table based approach to store the test inputs and expected
        //   outputs.
        //
        //   Select data using category partitioning from the following
        //   data categories:
        //
        //   dayOfMonth           date
        //   ----------           ----
        //                  Year  Month  Day
        //                  ----  -----  ---
        //
        //      1           0001  1 - 12   1
        //     27           2000          27
        //     28           9999          28
        //     29                         29
        //     30                         30
        //     31                         31
        //
        //   Specifically test for the following conditions:
        //     1. count == 0.
        //     2. For count is +ve, test for
        //        a. 'dayOfMonth' <  initialDate.day()
        //        b. 'dayOfMonth' == initialDate.day()
        //        c. 'dayOfMonth' >  initialDate.day()
        //     3. For count is -ve, test for
        //        a. 'dayOfMonth' <  initialDate.day()
        //        b. 'dayOfMonth' == initialDate.day()
        //        c. 'dayOfMonth' >  initialDate.day()
        //
        // Testing:
        //   bdet_Date ceilAdjustDay(dayOfMonth, initialDate, count);
        // --------------------------------------------------------------------

        if (verbose) { cout << "\nTesting ceilAdjustDay"
                            << "\n=====================" << endl; }
        {
            static const struct {
                int d_lineNum;     // source line number

                int d_dom;         // day of month

                int d_year;        // input year
                int d_month;       // input month
                int d_day;         // input day

                int d_count;       // count

                int d_expYear;     // expected year
                int d_expMonth;    // expected month
                int d_expDay;      // expected day
            } DATA [] = {
                // <--------- input -----------> <----- expected ---->
                //lin dom  year  mon  day  count ExpYear ExpMon ExpDay
                //--- ---  ----  ---  ---  ----- ------  ------ ------
                { L_,  1,    1,   1,   1,    0,     1,      1,      1},
                { L_,  2,    1,   1,   1,    0,     1,      1,      2},
                { L_,  1,    1,   1,   2,    0,     1,      2,      1},

                { L_, 31, 9999,  12,  31,    0,  9999,     12,     31},
                { L_, 31, 9999,  12,  30,    0,  9999,     12,     31},
                { L_, 29, 9999,  11,  30,    0,  9999,     12,     29},

                { L_, 29, 2000,   2,  29,    0,  2000,      2,     29},
                { L_, 29, 2000,   2,  28,    0,  2000,      2,     29},
                { L_, 28, 2000,   2,  29,    0,  2000,      3,     28},

                { L_,  1, 9999,   1,   1,    1,  9999,      2,      1},
                { L_,  1, 9999,   1,   1,    2,  9999,      3,      1},
                { L_,  1, 9998,   1,   1,   12,  9999,      1,      1},

                { L_,  2, 9999,   1,   1,    1,  9999,      2,      2},
                { L_,  2, 9999,   1,   1,    2,  9999,      3,      2},
                { L_,  2, 9998,   1,   1,   12,  9999,      1,      2},

                { L_,  1, 9999,   1,   2,    1,  9999,      3,      1},
                { L_,  1, 9999,   1,   2,    2,  9999,      4,      1},
                { L_,  1, 9998,   1,   2,   11,  9999,      1,      1},

                { L_, 29, 2000,   1,  29,    1,  2000,      2,     29},
                { L_, 29, 2000,   1,  29,    2,  2000,      3,     29},
                { L_, 29, 2000,   1,  29,   12,  2001,      1,     29},

                { L_, 29, 2000,   1,  28,    1,  2000,      2,     29},
                { L_, 29, 2000,   1,  28,    2,  2000,      3,     29},
                { L_, 29, 2000,   1,  28,   12,  2001,      1,     29},

                { L_, 29, 2000,   1,  30,    1,  2000,      3,     29},
                { L_, 29, 2000,   1,  30,    2,  2000,      4,     29},
                { L_, 29, 2000,   1,  30,   11,  2001,      1,     29},

                { L_, 31, 9999,  12,  31,   -1,  9999,     10,     31},
                { L_, 31, 9999,  12,  31,   -2,  9999,      8,     31},
                { L_, 31, 9999,  12,  31,   -7,  9998,     12,     31},

                { L_, 31, 9999,  12,  30,   -1,  9999,     10,     31},
                { L_, 31, 9999,  12,  30,   -2,  9999,      8,     31},
                { L_, 31, 9999,  12,  30,   -7,  9998,     12,     31},

#if !defined(BDE_BUILD_TARGET_SAFE)
                { L_, 30, 9999,  12,  31,   -1,  9999,     12,     30},
                { L_, 30, 9999,  12,  31,   -2,  9999,     11,     30},
                { L_, 30, 9999,  12,  31,  -12,  9998,     12,     30},
#endif

                { L_, 29, 2000,   3,  29,   -1,  2000,      2,     29},
                { L_, 29, 2000,   3,  29,   -2,  2000,      1,     29},
                { L_, 29, 2001,   3,  29,   -3,  2000,     11,     29},

                { L_, 28, 2000,   2,  29,   -1,  2000,      2,     28},
                { L_, 28, 2000,   2,  29,   -2,  2000,      1,     28},
                { L_, 27, 2001,   2,  28,   -3,  2000,     12,     27},

                { L_, 30, 2000,   2,  29,   -1,  2000,      1,     30},
                { L_, 30, 2001,   2,  28,   -2,  2000,     12,     30}
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int  LINE   = DATA[ti].d_lineNum;
                const int  DOM    = DATA[ti].d_dom;

                const int  YEAR   = DATA[ti].d_year;
                const int  MONTH  = DATA[ti].d_month;
                const int  DAY    = DATA[ti].d_day;

                const int  COUNT  = DATA[ti].d_count;

                const int  EXP_YEAR  = DATA[ti].d_expYear;
                const int  EXP_MONTH = DATA[ti].d_expMonth;
                const int  EXP_DAY   = DATA[ti].d_expDay;

                if (veryVerbose) { P(LINE) }

                const Date INPUT(YEAR, MONTH, DAY);
                const Date EXP_DATE(EXP_YEAR, EXP_MONTH, EXP_DAY);
                const Date CEIL      = Util::ceilDay(DOM, INPUT);

                Date       result     = Util::ceilAdjustDay(DOM, INPUT, COUNT);
                LOOP3_ASSERT(LINE, result, EXP_DATE, EXP_DATE == result);

                if (!(9999 == YEAR && 12 == MONTH && DAY > DOM)) {
                    Date   resultCeil = Util::ceilAdjustDay(DOM, INPUT, 0);
                    LOOP3_ASSERT(LINE, resultCeil, CEIL, CEIL == resultCeil);
                }
            }
        }
     } break;

      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'floorAdjustDay' FUNCTION
        //
        // Concerns:
        //   1. When count = 0, the result date is same as 'floorDay'.
        //   2. When initialDate.day() >= 'dayOfMonth', the counting should
        //      start from 'dayOfMonth' in initialDate.month().
        //   3. When initialDate.day() < 'dayOfMonth', the counting should
        //      start from 'dayOfMonth' in initialDate.month() - 1.
        //
        // Plan:
        //   Use a table based approach to store the test inputs and expected
        //   outputs.
        //
        //   Select data using category partitioning from the following
        //   data categories:
        //
        //   dayOfMonth           date
        //   ----------           ----
        //                  Year  Month  Day
        //                  ----  -----  ---
        //
        //      1           0001  1 - 12   1
        //     27           2000          27
        //     28           9999          28
        //     29                         29
        //     30                         30
        //     31                         31
        //
        //   Specifically test for the following conditions:
        //     1. count == 0.
        //     2. For count is +ve, test for
        //        a. 'dayOfMonth' <  initialDate.day()
        //        b. 'dayOfMonth' == initialDate.day()
        //        c. 'dayOfMonth' >  initialDate.day()
        //     3. For count is -ve, test for
        //        a. 'dayOfMonth' <  initialDate.day()
        //        b. 'dayOfMonth' == initialDate.day()
        //        c. 'dayOfMonth' >  initialDate.day()
        //
        // Testing:
        //   bdet_Date floorAdjustDay(dayOfMonth, initialDate, count);
        // --------------------------------------------------------------------

        if (verbose) { cout << "\nTesting floorAdjustDay"
                            << "\n======================" << endl; }
        {
            static const struct {
                int d_lineNum;     // source line number

                int d_dom;         // day of month

                int d_year;        // input year
                int d_month;       // input month
                int d_day;         // input day

                int d_count;       // count

                int d_expYear;     // expected year
                int d_expMonth;    // expected month
                int d_expDay;      // expected day
            } DATA [] = {
                // <--------- input -----------> <----- expected ---->
                //lin dom  year  mon  day  count ExpYear ExpMon ExpDay
                //--- ---  ----  ---  ---  ----- ------  ------ ------
                { L_,  1,    1,   1,   1,    0,     1,      1,      1},
                { L_,  2,    1,   2,   1,    0,     1,      1,      2},
                { L_,  1,    1,   1,   2,    0,     1,      1,      1},

                { L_, 31, 9999,  12,  31,    0,  9999,     12,     31},
                { L_, 31, 9999,  12,  30,    0,  9999,     10,     31},
                { L_, 30, 9999,  12,  31,    0,  9999,     12,     30},

                { L_, 29, 2000,   2,  29,    0,  2000,      2,     29},
                { L_, 29, 2000,   3,  28,    0,  2000,      2,     29},
                { L_, 28, 2000,   2,  29,    0,  2000,      2,     28},

                { L_,  1, 9999,   1,   1,    1,  9999,      2,      1},
                { L_,  1, 9999,   1,   1,    2,  9999,      3,      1},
                { L_,  1, 9998,   1,   1,   12,  9999,      1,      1},

                { L_,  2, 9999,   2,   1,    1,  9999,      2,      2},
                { L_,  2, 9999,   2,   1,    2,  9999,      3,      2},
                { L_,  2, 9998,   2,   1,   12,  9999,      1,      2},

                { L_,  1, 9999,   1,   2,    1,  9999,      2,      1},
                { L_,  1, 9999,   1,   2,    2,  9999,      3,      1},
                { L_,  1, 9998,   1,   2,   12,  9999,      1,      1},

                { L_, 29, 2000,   1,  29,    1,  2000,      2,     29},
                { L_, 29, 2000,   1,  29,    2,  2000,      3,     29},
                { L_, 29, 2000,   1,  29,   12,  2001,      1,     29},

                { L_, 29, 2000,   2,  28,    1,  2000,      2,     29},
                { L_, 29, 2000,   2,  28,    2,  2000,      3,     29},
                { L_, 29, 2000,   2,  28,   12,  2001,      1,     29},

                { L_, 29, 2000,   1,  30,    1,  2000,      2,     29},
                { L_, 29, 2000,   1,  30,    2,  2000,      3,     29},
                { L_, 29, 2000,   1,  30,   12,  2001,      1,     29},

                { L_, 31, 9999,  12,  31,   -1,  9999,     10,     31},
                { L_, 31, 9999,  12,  31,   -2,  9999,      8,     31},
                { L_, 31, 9999,  12,  31,   -7,  9998,     12,     31},

                { L_, 31, 9999,  12,  30,   -1,  9999,      8,     31},
                { L_, 31, 9999,  12,  30,   -2,  9999,      7,     31},
                { L_, 31, 9999,  12,  30,   -6,  9998,     12,     31},

                { L_, 30, 9999,  12,  31,   -1,  9999,     11,     30},
                { L_, 30, 9999,  12,  31,   -2,  9999,     10,     30},
                { L_, 30, 9999,  12,  31,  -11,  9998,     12,     30},

                { L_, 29, 2000,   3,  29,   -1,  2000,      2,     29},
                { L_, 29, 2000,   3,  29,   -2,  2000,      1,     29},
                { L_, 29, 2001,   3,  29,   -3,  2000,     11,     29},

                { L_, 28, 2000,   3,  29,   -1,  2000,      2,     28},
                { L_, 28, 2000,   3,  29,   -2,  2000,      1,     28},
                { L_, 28, 2001,   3,  29,   -3,  2000,     12,     28},

                { L_, 30, 2000,   4,  29,   -1,  2000,      1,     30},
                { L_, 30, 2001,   4,  29,   -2,  2000,     12,     30}
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int  LINE   = DATA[ti].d_lineNum;
                const int  DOM    = DATA[ti].d_dom;

                const int  YEAR   = DATA[ti].d_year;
                const int  MONTH  = DATA[ti].d_month;
                const int  DAY    = DATA[ti].d_day;

                const int  COUNT  = DATA[ti].d_count;

                const int  EXP_YEAR  = DATA[ti].d_expYear;
                const int  EXP_MONTH = DATA[ti].d_expMonth;
                const int  EXP_DAY   = DATA[ti].d_expDay;

                const Date INPUT(YEAR, MONTH, DAY);
                const Date EXP_DATE(EXP_YEAR, EXP_MONTH, EXP_DAY);
                const Date FLOOR     = Util::floorDay(DOM, INPUT);

                Date     result      = Util::floorAdjustDay(DOM, INPUT, COUNT);
                Date     resultFloor = Util::floorAdjustDay(DOM, INPUT, 0);

                LOOP3_ASSERT(LINE, result, EXP_DATE, EXP_DATE == result);
                LOOP3_ASSERT(LINE, resultFloor, FLOOR, FLOOR  == resultFloor);
            }
        }
      } break;

      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'ceilOrPreviousDayInMonth' FUNCTION
        //
        // Concerns:
        //   1. If 'dayOfMonth' is equal to initialDate.dayOfMonth(),
        //      initialDate is returned.
        //   2. Correct date is returned when result date is not in same month.
        //
        // Plan:
        //   Use a table based approach to store the test inputs and expected
        //   outputs.
        //
        //   Select data using category partitioning from the following
        //   data categories:
        //
        //   dayOfMonth           date
        //   ----------           ----
        //                  Year  Month  Day
        //                  ----  -----  ---
        //
        //      1           0001  1 - 12   1
        //     27           2000          27
        //     28           9999          28
        //     29                         29
        //     30                         30
        //     31                         31
        //
        //   Specifically test for the following conditions:
        //     0. When initialDate.day() == 'dayOfMonth'
        //     1. When 'dayOfMonth' < initialDate.day()
        //     2. When 'dayOfMonth' > initialDate.day()
        //     3. When result date lies in initialDate.month() - 1.
        //
        // Testing:
        //   bdet_Date ceilOrPreviousDayInMonth(dayOfMonth,initialDate);
        // --------------------------------------------------------------------

        if (verbose) { cout << "\nTesting ceilOrPreviousDayInMonth"
                            << "\n================================" << endl; }
        {
            static const struct {
                int d_lineNum;     // source line number

                int d_dom;         // day of month

                int d_year;        // input year
                int d_month;       // input month
                int d_day;         // input day

                int d_expYear;     // expected year
                int d_expMonth;    // expected month
                int d_expDay;      // expected day
            } DATA [] = {
                // <------- input ------->    <-expected->
                //lin dom year   mon  day  ExpYear ExpMon ExpDay
                //--- --- ----   ---  ---  ------  ------ ------
                { L_,  1,    1,    1,   1,     1,     1,      1},
                { L_, 31, 9999,   12,  31,  9999,    12,     31},

                { L_, 28, 2000,    3,  28,  2000,     3,     28},
                { L_, 29, 2000,    3,  29,  2000,     3,     29},

                // day < dom
                { L_, 30,    1,    1,   1,     1,     1,     30},
                { L_, 31,    1,    1,   1,     1,     1,     31},

                { L_, 28,    1,    2,   1,     1,     2,     28},
                { L_, 29,    1,    2,   1,     1,     1,     29},
                { L_, 30,    1,    2,   1,     1,     1,     30},
                { L_, 31,    1,    2,   1,     1,     1,     31},

                { L_, 30,    1,    4,   1,     1,     4,     30},
                { L_, 31,    1,    4,   1,     1,     3,     31},

                { L_, 28, 2000,    2,   1,  2000,     2,     28},
                { L_, 29, 2000,    2,   1,  2000,     2,     29},
                { L_, 30, 2000,    2,   1,  2000,     1,     30},
                { L_, 31, 2000,    2,   1,  2000,     1,     31},

                // day >= dom
                { L_, 31, 9999,    1,  31,  9999,     1,     31},
                { L_, 30, 9999,    1,  31,  9999,     1,     30},

                { L_, 28, 9999,    2,  28,  9999,     2,     28},
                { L_, 27, 9999,    2,  28,  9999,     2,     27},

                { L_, 30, 9999,    4,  30,  9999,     4,     30},
                { L_, 29, 9999,    4,  30,  9999,     4,     29},

                { L_, 31, 9999,   12,  31,  9999,    12,     31},
                { L_, 30, 9999,   12,  31,  9999,    12,     30},

                { L_, 28, 2000,    2,  28,  2000,     2,     28},
                { L_, 28, 2000,    2,  29,  2000,     2,     28},

                { L_, 29, 2000,    3,  29,  2000,     3,     29},
                { L_, 28, 2000,    3,  29,  2000,     3,     28}
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int  LINE   = DATA[ti].d_lineNum;
                const int  DOM    = DATA[ti].d_dom;

                const int  YEAR   = DATA[ti].d_year;
                const int  MONTH  = DATA[ti].d_month;
                const int  DAY    = DATA[ti].d_day;

                const int  EXP_YEAR  = DATA[ti].d_expYear;
                const int  EXP_MONTH = DATA[ti].d_expMonth;
                const int  EXP_DAY   = DATA[ti].d_expDay;

                const Date INPUT(YEAR, MONTH, DAY);
                const Date EXP_DATE(EXP_YEAR, EXP_MONTH, EXP_DAY);

                Date       result = Util::ceilOrPreviousDayInMonth(DOM, INPUT);

                LOOP3_ASSERT(LINE, result, EXP_DATE, EXP_DATE == result);
            }
        }
     } break;

      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'floorOrNextDayInMonth' FUNCTION
        //
        // Concerns:
        //   1. If 'dayOfMonth' is equal to initialDate.dayOfMonth(),
        //      initialDate is returned.
        //   2. Correct date is returned when result date is not in same month.
        //
        // Plan:
        //   Use a table based approach to store the test inputs and expected
        //   outputs.
        //
        //   Select data using category partitioning from the following
        //   data categories:
        //
        //   dayOfMonth           date
        //   ----------           ----
        //                  Year  Month  Day
        //                  ----  -----  ---
        //
        //      1           0001  1 - 12   1
        //     27           2000          27
        //     28           9999          28
        //     29                         29
        //     30                         30
        //     31                         31
        //
        //   Specifically test for the following conditions:
        //     0. When initialDate.day() == 'dayOfMonth'
        //     1. When 'dayOfMonth' < initialDate.day()
        //     2. When 'dayOfMonth' > initialDate.day()
        //     3. When result date lies in initialDate.month() + 1.
        //
        // Testing:
        //   bdet_Date floorOrNextDayInMonth(dayOfMonth, initialDate);
        // --------------------------------------------------------------------

        if (verbose) { cout << "\nTesting floorOrNextDayInMonth"
                            << "\n=============================" << endl; }
        {
            static const struct {
                int d_lineNum;     // source line number

                int d_dom;         // day of month

                int d_year;        // input year
                int d_month;       // input month
                int d_day;         // input day

                int d_expYear;     // expected year
                int d_expMonth;    // expected month
                int d_expDay;      // expected day
            } DATA [] = {
                // <------- input -------> <---- expected ----->
                //lin dom year   mon  day  ExpYear ExpMon ExpDay
                //--- --- ----   ---  ---  ------  ------ ------
                { L_,  1,    1,    1,   1,     1,     1,      1},
                { L_, 31, 9999,   12,  31,  9999,    12,     31},

                { L_, 28, 2000,    3,  28,  2000,     3,     28},
                { L_, 29, 2000,    3,  29,  2000,     3,     29},

                // day > dom
                { L_,  1,    1,    1,   2,     1,     1,      1},
                { L_,  1,    1,    1,  31,     1,     1,      1},

                { L_,  1,    1,    2,   2,     1,     2,      1},
                { L_,  1,    1,    2,  28,     1,     2,      1},

                { L_,  1,    1,    4,   2,     1,     4,      1},
                { L_,  1,    1,    4,  30,     1,     4,      1},

                { L_,  1, 2000,    2,   2,  2000,     2,      1},
                { L_, 27, 2000,    2,  28,  2000,     2,     27},
                { L_, 28, 2000,    2,  29,  2000,     2,     28},

                // day <= dom
                { L_, 30, 9999,   12,  30,  9999,    12,     30},
                { L_, 31, 9999,   12,  31,  9999,    12,     31},
                { L_, 30, 9999,   12,   1,  9999,    12,     30},
                { L_, 31, 9999,   12,   1,  9999,    12,     31},

                { L_, 28, 9999,    2,  28,  9999,     2,     28},
                { L_, 29, 9999,    2,  28,  9999,     3,     29},
                { L_, 30, 9999,    2,  28,  9999,     3,     30},
                { L_, 31, 9999,    2,  28,  9999,     3,     31},

                { L_, 30, 9999,    4,  30,  9999,     4,     30},
                { L_, 31, 9999,    4,   1,  9999,     5,     31},

                { L_, 28, 2000,    2,  28,  2000,     2,     28},
                { L_, 29, 2000,    2,  28,  2000,     2,     29},
                { L_, 30, 2000,    2,  29,  2000,     3,     30},
                { L_, 31, 2000,    2,  29,  2000,     3,     31}
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int  LINE   = DATA[ti].d_lineNum;
                const int  DOM    = DATA[ti].d_dom;

                const int  YEAR   = DATA[ti].d_year;
                const int  MONTH  = DATA[ti].d_month;
                const int  DAY    = DATA[ti].d_day;

                const int  EXP_YEAR  = DATA[ti].d_expYear;
                const int  EXP_MONTH = DATA[ti].d_expMonth;
                const int  EXP_DAY   = DATA[ti].d_expDay;

                const Date INPUT(YEAR, MONTH, DAY);
                const Date EXP_DATE(EXP_YEAR, EXP_MONTH, EXP_DAY);

                Date       result = Util::floorOrNextDayInMonth(DOM, INPUT);

                LOOP3_ASSERT(LINE, result, EXP_DATE, EXP_DATE == result);
            }
        }
      } break;

      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'floorDay' FUNCTION
        //
        // Concerns:
        //   1. If 'dayOfMonth' is equal to initialDate.dayOfMonth(),
        //      initialDate is returned.
        //   2. Correct date is returned when result date is not in same month.
        //
        // Plan:
        //   Use a table based approach to store the test inputs and expected
        //   outputs.
        //
        //   Select data using category partitioning from the following
        //   data categories:
        //
        //   dayOfMonth           date
        //   ----------           ----
        //                  Year  Month  Day
        //                  ----  -----  ---
        //
        //      1           0001  1 - 12   1
        //     27           2000          27
        //     28           9999          28
        //     29                         29
        //     30                         30
        //     31                         31
        //
        //   Specifically test for the following conditions:
        //     0. When initialDate.day() == 'dayOfMonth'
        //     1. When result date lies in the same month as
        //        initialDate.month().
        //     2. When result date lies in initialDate.month() - 1.
        //     3. When result date lies in initialDate.month() - 2.
        //     4. When result date lies in initialDate.year() - 1.
        //
        // Testing:
        //   bdet_Date floorDay(dayOfMonth, initialDate);
        // --------------------------------------------------------------------

        if (verbose) { cout << "\nTesting floorDay"
                            << "\n================" << endl; }
        {
            static const struct {
                int d_lineNum;     // source line number

                int d_dom;         // day of month

                int d_year;        // input year
                int d_month;       // input month
                int d_day;         // input day

                int d_expYear;     // expected year
                int d_expMonth;    // expected month
                int d_expDay;      // expected day
            } DATA [] = {
                // <------- input ------->    <-expected->
                //lin dom year   mon  day  ExpYear ExpMon ExpDay
                //--- --- ----   ---  ---  ------  ------ ------
                { L_,  1,    1,    1,   1,     1,     1,      1},
                { L_,  1,    1,    2,   1,     1,     2,      1},

                { L_, 30, 9999,   12,  30,  9999,    12,     30},
                { L_, 31, 9999,   12,  31,  9999,    12,     31},

                { L_, 28, 2000,    2,  28,  2000,     2,     28},
                { L_, 29, 2000,    2,  29,  2000,     2,     29},

                // dom < day
                { L_,  1,    1,    1,   2,     1,     1,      1},
                { L_,  1,    1,    1,  31,     1,     1,      1},

                { L_,  1,    1,    2,   2,     1,     2,      1},
                { L_, 27,    1,    2,  28,     1,     2,     27},

                { L_,  1,    1,    4,  30,     1,     4,      1},

                { L_,  1, 2000,    2,   2,  2000,     2,      1},
                { L_, 27, 2000,    2,  28,  2000,     2,     27},
                { L_, 28, 2000,    2,  29,  2000,     2,     28},

                // dom >= day, testing adjacent months
                { L_, 30, 9999,   12,  30,  9999,    12,     30},
                { L_, 30, 9999,   12,  29,  9999,    11,     30},

                { L_, 28, 9999,    2,  28,  9999,     2,     28},
                { L_, 29, 9999,    2,  28,  9999,     1,     29},
                { L_, 30, 9999,    2,  28,  9999,     1,     30},
                { L_, 31, 9999,    2,  28,  9999,     1,     31},

                { L_, 28, 9999,    3,  28,  9999,     3,     28},
                { L_, 28, 9999,    3,  27,  9999,     2,     28},

                { L_, 30, 9999,    4,  30,  9999,     4,     30},
                { L_, 31, 9999,    4,  30,  9999,     3,     31},

                { L_, 28, 2000,    2,  28,  2000,     2,     28},
                { L_, 29, 2000,    2,  28,  2000,     1,     29},
                { L_, 30, 2000,    2,  29,  2000,     1,     30},
                { L_, 31, 2000,    2,  29,  2000,     1,     31},

                { L_, 29, 2000,    3,  29,  2000,     3,     29},
                { L_, 29, 2000,    3,  28,  2000,     2,     29},

                // year change
                { L_,  2, 9999,    1,   1,  9998,    12,      2},
                { L_, 30, 9999,    1,   1,  9998,    12,     30},
                { L_, 31, 9999,    1,   1,  9998,    12,     31},

                // dom > day, testing multiple months
                { L_, 31, 9999,   12,  30,  9999,    10,     31},
                { L_, 31, 9999,   10,  30,  9999,     8,     31},
                { L_, 31, 9999,    7,  30,  9999,     5,     31},
                { L_, 31, 9999,    5,  30,  9999,     3,     31},
                { L_, 29, 9999,    3,  28,  9999,     1,     29},
                { L_, 30, 9999,    3,  29,  9999,     1,     30},
                { L_, 31, 9999,    3,  30,  9999,     1,     31},

                { L_, 31, 2000,    3,  30,  2000,     1,     31}
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int  LINE   = DATA[ti].d_lineNum;
                const int  DOM    = DATA[ti].d_dom;

                const int  YEAR   = DATA[ti].d_year;
                const int  MONTH  = DATA[ti].d_month;
                const int  DAY    = DATA[ti].d_day;

                const int  EXP_YEAR  = DATA[ti].d_expYear;
                const int  EXP_MONTH = DATA[ti].d_expMonth;
                const int  EXP_DAY   = DATA[ti].d_expDay;

                const Date INPUT(YEAR, MONTH, DAY);
                const Date EXP_DATE(EXP_YEAR, EXP_MONTH, EXP_DAY);

                Date result = Util::floorDay(DOM, INPUT);

                LOOP3_ASSERT(LINE, result, EXP_DATE, EXP_DATE == result);
            }
        }
     } break;

     case 6: {
        // --------------------------------------------------------------------
        // TESTING 'ceilDay' FUNCTION
        //
        // Concerns:
        //   1. If 'dayOfMonth' is equal to initialDate.dayOfMonth(),
        //      initialDate is returned.
        //   2. Correct date is returned when result date is not in same month.
        //
        // Plan:
        //   Use a table based approach to store the test inputs and expected
        //   outputs.
        //
        //   Select data using category partitioning from the following
        //   data categories:
        //
        //   dayOfMonth           date
        //   ----------           ----
        //                  Year  Month  Day
        //                  ----  -----  ---
        //
        //      1           0001  1 - 12   1
        //     27           2000          27
        //     28           9999          28
        //     29                         29
        //     30                         30
        //     31                         31
        //
        //   Specifically test for the following conditions:
        //     0. When initialDate.day() == 'dayOfMonth'
        //     1. When result date lies in the same month as
        //        initialDate.month().
        //     2. When result date lies in initialDate.month() + 1.
        //     3. When result date lies in initialDate.month() + 2.
        //     4. When result date lies in initialDate.year() + 1.
        //
        // Testing:
        //   bdet_Date ceilDay(dayOfMonth, initialDate);
        // --------------------------------------------------------------------

        if (verbose) { cout << "\nTesting ceilDay"
                            << "\n===============" << endl; }
        {
            static const struct {
                int d_lineNum;     // source line number

                int d_dom;         // day of month

                int d_year;        // input year
                int d_month;       // input month
                int d_day;         // input day

                int d_expYear;     // expected year
                int d_expMonth;    // expected month
                int d_expDay;      // expected day
            } DATA [] = {
                // <------- input ------->    <-expected->
                //lin dom year   mon  day  ExpYear ExpMon ExpDay
                //--- --- ----   ---  ---  ------  ------ ------
                { L_, 30, 9999,   12,  30,  9999,    12,     30},
                { L_, 31, 9999,   12,  31,  9999,    12,     31},

                { L_,  1,    1,    1,   1,     1,     1,      1},
                { L_, 31,    1,   12,  31,     1,    12,     31},

                { L_, 28, 2000,    2,  28,  2000,     2,     28},
                { L_, 29, 2000,    2,  29,  2000,     2,     29},

                // day < dom
                { L_,  2,    1,    1,   1,     1,     1,      2},
                { L_, 30,    1,    1,   1,     1,     1,     30},
                { L_, 31,    1,    1,   1,     1,     1,     31},

                { L_, 27,    1,    2,   1,     1,     2,     27},
                { L_, 28,    1,    2,   1,     1,     2,     28},
                { L_, 29,    1,    2,   1,     1,     3,     29},
                { L_, 30,    1,    2,   1,     1,     3,     30},
                { L_, 31,    1,    2,   1,     1,     3,     31},

                { L_, 30,    1,    4,   1,     1,     4,     30},
                { L_, 31,    1,    4,   1,     1,     5,     31},

                { L_, 28, 2000,    2,   1,  2000,     2,     28},
                { L_, 29, 2000,    2,   1,  2000,     2,     29},
                { L_, 30, 2000,    2,   1,  2000,     3,     30},
                { L_, 31, 2000,    2,   1,  2000,     3,     31},

                // day >= dom
                { L_,  1,    1,    1,   2,     1,     2,      1},
                { L_, 28,    1,    1,  29,     1,     2,     28},
                { L_, 28,    1,    1,  30,     1,     2,     28},
                { L_, 28,    1,    1,  31,     1,     2,     28},
                { L_, 31,    1,    1,  31,     1,     1,     31},

                { L_, 27,    1,    2,  28,     1,     3,     27},
                { L_, 28,    1,    2,  28,     1,     2,     28},
                { L_, 29,    1,    2,  28,     1,     3,     29},
                { L_, 30,    1,    2,  28,     1,     3,     30},
                { L_, 31,    1,    2,  28,     1,     3,     31},

                { L_, 30,    1,    3,  31,     1,     4,     30},
                { L_, 31,    1,    3,  31,     1,     3,     31},

                { L_, 29,    1,    4,  30,     1,     5,     29},
                { L_, 30,    1,    4,  30,     1,     4,     30},

                { L_, 27, 2000,    1,  28,  2000,     2,     27},
                { L_, 28, 2000,    1,  29,  2000,     2,     28},
                { L_, 29, 2000,    1,  30,  2000,     2,     29},
                { L_, 30, 2000,    1,  31,  2000,     3,     30},

                { L_, 27, 2000,    2,  28,  2000,     3,     27},
                { L_, 28, 2000,    2,  29,  2000,     3,     28},

                // year change, alternate month
                { L_,  1, 9998,   12,   2,  9999,     1,      1},
                { L_, 30, 9998,   12,  31,  9999,     1,     30},

                // Testing multiple months (two months)

                { L_, 29, 9999,    1,  30,  9999,     3,     29},
                { L_, 29, 9999,    1,  31,  9999,     3,     29},

                { L_, 29, 2000,    1,  30,  2000,     2,     29},
                { L_, 30, 2000,    1,  31,  2000,     3,     30}
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int  LINE   = DATA[ti].d_lineNum;
                const int  DOM    = DATA[ti].d_dom;

                const int  YEAR   = DATA[ti].d_year;
                const int  MONTH  = DATA[ti].d_month;
                const int  DAY    = DATA[ti].d_day;

                const int  EXP_YEAR  = DATA[ti].d_expYear;
                const int  EXP_MONTH = DATA[ti].d_expMonth;
                const int  EXP_DAY   = DATA[ti].d_expDay;

                const Date INPUT(YEAR, MONTH, DAY);
                const Date EXP_DATE(EXP_YEAR, EXP_MONTH, EXP_DAY);

                Date result = Util::ceilDay(DOM, INPUT);

                LOOP3_ASSERT(LINE, result, EXP_DATE, EXP_DATE == result);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'adjustDay' FUNCTION
        //
        // Concerns:
        //   1. When count = -1, then result date is same as previousDay(...).
        //   2. When count =  1, then result date is same as nextDay(...).
        //   3. Date arithmetic is correct when result date does not lie in
        //      the same month as initialDate.month().
        //   4. Month arithmetic is handled correctly when the result date is
        //      in a different year from initialDate.year().
        //   5. Cases when 'dayOfMonth' <= 28, and 29 <= 'dayOfMonth' <= 31 is
        //      handled correctly.
        //   6. Static arrays used in the function are correctly initialized.
        //
        // Plan:
        //   Use a table based approach to store the test inputs and expected
        //   outputs.
        //
        //   Select data using category partitioning from the following
        //   data categories:
        //
        //   dayOfMonth           date            count
        //   ----------           ----            -----
        //                  Year  Month  Day
        //                  ----  -----  ---
        //
        //      1           0001  1 - 12   1      A collection of positive and
        //     27           2000          27      negative numbers.
        //     28           9999          28
        //     29                         29
        //     30                         30
        //     31                         31
        //
        //   Specifically test for the following conditions:
        //     1. When count is 1 then result date matches that returned by
        //        'nextDay' function.
        //     2. When count is -1 then result date matches that returned by
        //        'previousDay' function.
        //     3. When result date lies in the same month as
        //        initialDate.month().
        //     4. For each of the cases when 'dayOfMonth' <= 28 and when
        //        'dayOfMonth' is in range 29 - 31, test the following:
        //          a. When result date lies in initialDate.year().
        //          b. When result date lies in initialDate.year() + 1.
        //          c. When result date lies in initialDate.year() + 2.
        //          d. When result date lies in initialDate.year() - 1.
        //          e. When result date lies in initialDate.year() - 2.
        //        Especially check that the year transitions are handled
        //        correctly.  In each case test with multiple months.
        //     5. Test that static arrays contain correct data.
        //     6. Ensure that the boundary limits imposed by 'bdet_Date' are
        //        handled correctly.
        //
        // Testing:
        //   bdet_Date adjustDay(dayOfMonth, initialDate, count);
        // --------------------------------------------------------------------

        if (verbose) { cout << "\nTesting adjustDay"
                            << "\n=================" << endl; }
        {
            static const struct {
                int d_lineNum;     // source line number

                int d_dom;         // day of month

                int d_year;        // input year
                int d_month;       // input month
                int d_day;         // input day

                int d_count;       // count

                int d_expYear;     // expected year
                int d_expMonth;    // expected month
                int d_expDay;      // expected day
            } DATA [] = {
                // <--------- input -----------> <----- expected ---->
                //lin dom  year  mon  day  count ExpYear ExpMon ExpDay
                //--- ---  ----  ---  ---  ----- ------  ------ ------
                { L_,  1,    1,   1,   2,    -1,     1,      1,      1},
                { L_, 31, 9999,  12,  30,     1,  9999,     12,     31},

                { L_,  1,    1,   2,   1,     2,     1,      4,      1},
                { L_,  2,    1,   2,   1,     3,     1,      4,      2},
                { L_,  2,    1,   2,   3,     3,     1,      5,      2},

                { L_, 27,    1,   2,  27,     2,     1,      4,     27},
                { L_, 27,    1,   2,  26,     2,     1,      3,     27},
                { L_, 27,    1,   2,  28,     2,     1,      4,     27},

                { L_,  1,    1,   3,   1,     2,     1,      5,      1},
                { L_, 30,    1,   3,  29,     2,     1,      4,     30},
                { L_, 30,    1,   3,  31,     2,     1,      5,     30},

                // dom <= 28
                // February.
                { L_,  1, 9999,   2,   1,     9,  9999,     11,      1},
                { L_, 27, 9999,   2,   1,     9,  9999,     10,     27},
                { L_, 28, 9999,   2,   1,     9,  9999,     10,     28},

                { L_,  1, 9999,   2,   1,    10,  9999,     12,      1},
                { L_, 27, 9999,   2,   1,    10,  9999,     11,     27},
                { L_, 28, 9999,   2,   1,    10,  9999,     11,     28},

                { L_,  1, 9998,   2,   1,    11,  9999,      1,      1},
                { L_, 27, 9998,   2,   1,    11,  9998,     12,     27},
                { L_, 28, 9998,   2,   1,    11,  9998,     12,     28},

                { L_,  1, 9998,   2,   1,    12,  9999,      2,      1},
                { L_, 27, 9998,   2,   1,    12,  9999,      1,     27},
                { L_, 28, 9998,   2,   1,    12,  9999,      1,     28},

                { L_,  1, 9997,   2,   1,    22,  9998,     12,      1},
                { L_, 27, 9997,   2,   1,    22,  9998,     11,     27},
                { L_, 28, 9997,   2,   1,    22,  9998,     11,     28},

                { L_,  1, 9997,   2,   1,    23,  9999,      1,      1},
                { L_, 27, 9997,   2,   1,    23,  9998,     12,     27},
                { L_, 28, 9997,   2,   1,    23,  9998,     12,     28},

                { L_,  1, 9997,   2,   1,    24,  9999,      2,      1},
                { L_, 27, 9997,   2,   1,    24,  9999,      1,     27},
                { L_, 28, 9997,   2,   1,    24,  9999,      1,     28},

                // June.
                { L_,  1, 9999,   6,   1,     5,  9999,     11,      1},
                { L_, 27, 9999,   6,   1,     5,  9999,     10,     27},
                { L_, 28, 9999,   6,   1,     5,  9999,     10,     28},

                { L_,  1, 9999,   6,   1,     6,  9999,     12,      1},
                { L_, 27, 9999,   6,   1,     6,  9999,     11,     27},
                { L_, 28, 9999,   6,   1,     6,  9999,     11,     28},

                { L_,  1, 9998,   6,   1,     7,  9999,      1,      1},
                { L_, 27, 9998,   6,   1,     7,  9998,     12,     27},
                { L_, 28, 9998,   6,   1,     7,  9998,     12,     28},

                { L_,  1, 9998,   6,   1,     8,  9999,      2,      1},
                { L_, 27, 9998,   6,   1,     8,  9999,      1,     27},
                { L_, 28, 9998,   6,   1,     8,  9999,      1,     28},

                { L_,  1, 9998,   6,   1,     9,  9999,      3,      1},
                { L_, 27, 9998,   6,   1,     9,  9999,      2,     27},
                { L_, 28, 9998,   6,   1,     9,  9999,      2,     28},

                { L_,  1, 9997,   6,   1,    18,  9998,     12,      1},
                { L_, 27, 9997,   6,   1,    18,  9998,     11,     27},
                { L_, 28, 9997,   6,   1,    18,  9998,     11,     28},

                { L_,  1, 9997,   6,   1,    19,  9999,      1,      1},
                { L_, 27, 9997,   6,   1,    19,  9998,     12,     27},
                { L_, 28, 9997,   6,   1,    19,  9998,     12,     28},

                { L_,  1, 9997,   6,   1,    20,  9999,      2,      1},
                { L_, 27, 9997,   6,   1,    20,  9999,      1,     27},
                { L_, 28, 9997,   6,   1,    20,  9999,      1,     28},

                { L_,  1, 9997,   6,   1,    21,  9999,      3,      1},
                { L_, 27, 9997,   6,   1,    21,  9999,      2,     27},
                { L_, 28, 9997,   6,   1,    21,  9999,      2,     28},

                // December.
                { L_,  1, 9998,  12,   1,     1,  9999,      1,      1},
                { L_, 27, 9998,  12,   1,     1,  9998,     12,     27},
                { L_, 28, 9998,  12,   1,     1,  9998,     12,     28},

                { L_,  1, 9998,  12,   1,     2,  9999,      2,      1},
                { L_, 27, 9998,  12,   1,     2,  9999,      1,     27},
                { L_, 28, 9998,  12,   1,     2,  9999,      1,     28},

                { L_,  1, 9998,  12,   1,     3,  9999,      3,      1},
                { L_, 27, 9998,  12,   1,     3,  9999,      2,     27},
                { L_, 28, 9998,  12,   1,     3,  9999,      2,     28},

                { L_,  1, 9997,  12,   1,    12,  9998,     12,      1},
                { L_, 27, 9997,  12,   1,    12,  9998,     11,     27},
                { L_, 28, 9997,  12,   1,    12,  9998,     11,     28},

                { L_,  1, 9997,  12,   1,    13,  9999,      1,      1},
                { L_, 27, 9997,  12,   1,    13,  9998,     12,     27},
                { L_, 28, 9997,  12,   1,    13,  9998,     12,     28},

                { L_,  1, 9997,  12,   1,    14,  9999,      2,      1},
                { L_, 27, 9997,  12,   1,    14,  9999,      1,     27},
                { L_, 28, 9997,  12,   1,    14,  9999,      1,     28},

                { L_,  1, 9997,  12,   1,    15,  9999,      3,      1},
                { L_, 27, 9997,  12,   1,    15,  9999,      2,     27},
                { L_, 28, 9997,  12,   1,    15,  9999,      2,     28},

                // test dom == 29
                // January.
                { L_, 29, 1998,   1,   1,     3,  1998,      4,     29},
                { L_, 29, 1998,   1,   1,     6,  1998,      7,     29},
                { L_, 29, 1998,   1,   1,    10,  1998,     11,     29},
                { L_, 29, 1998,   1,   1,    11,  1998,     12,     29},

                { L_, 29, 1998,   1,   1,    12,  1999,      1,     29},
                { L_, 29, 1998,   1,   1,    13,  1999,      3,     29},
                { L_, 29, 1998,   1,   1,    14,  1999,      4,     29},
                { L_, 29, 1998,   1,   1,    22,  1999,     12,     29},

                { L_, 29, 1998,   1,   1,    23,  2000,      1,     29},
                { L_, 29, 1998,   1,   1,    24,  2000,      2,     29},
                { L_, 29, 1998,   1,   1,    25,  2000,      3,     29},

                // March.
                { L_, 29, 1998,   3,   1,     3,  1998,      5,     29},
                { L_, 29, 1998,   3,   1,     9,  1998,     11,     29},
                { L_, 29, 1998,   3,   1,    10,  1998,     12,     29},

                { L_, 29, 1998,   3,   1,    11,  1999,      1,     29},
                { L_, 29, 1998,   3,   1,    12,  1999,      3,     29},
                { L_, 29, 1998,   3,   1,    13,  1999,      4,     29},
                { L_, 29, 1998,   3,   1,    21,  1999,     12,     29},

                { L_, 29, 1998,   3,   1,    22,  2000,      1,     29},
                { L_, 29, 1998,   3,   1,    23,  2000,      2,     29},
                { L_, 29, 1998,   3,   1,    24,  2000,      3,     29},

                // June.
                { L_, 29, 1998,   6,   1,     3,  1998,      8,     29},
                { L_, 29, 1998,   6,   1,     6,  1998,     11,     29},
                { L_, 29, 1998,   6,   1,     7,  1998,     12,     29},

                { L_, 29, 1998,   6,   1,     8,  1999,      1,     29},
                { L_, 29, 1998,   6,   1,     9,  1999,      3,     29},
                { L_, 29, 1998,   6,   1,    10,  1999,      4,     29},
                { L_, 29, 1998,   6,   1,    18,  1999,     12,     29},

                { L_, 29, 1998,   6,   1,    19,  2000,      1,     29},
                { L_, 29, 1998,   6,   1,    20,  2000,      2,     29},
                { L_, 29, 1998,   6,   1,    21,  2000,      3,     29},

                // December.
                { L_, 29, 1998,  12,   1,     1,  1998,     12,     29},

                { L_, 29, 1998,  12,   1,     2,  1999,      1,     29},
                { L_, 29, 1998,  12,   1,     3,  1999,      3,     29},
                { L_, 29, 1998,  12,   1,     4,  1999,      4,     29},
                { L_, 29, 1998,  12,   1,    12,  1999,     12,     29},

                { L_, 29, 1998,  12,   1,    13,  2000,      1,     29},
                { L_, 29, 1998,  12,   1,    14,  2000,      2,     29},
                { L_, 29, 1998,  12,   1,    15,  2000,      3,     29},

                // test dom == 30
                // January.
                { L_, 30, 1998,   1,   1,     3,  1998,      4,     30},
                { L_, 30, 1998,   1,   1,     6,  1998,      7,     30},
                { L_, 30, 1998,   1,   1,    10,  1998,     11,     30},
                { L_, 30, 1998,   1,   1,    11,  1998,     12,     30},

                { L_, 30, 1998,   1,   1,    12,  1999,      1,     30},
                { L_, 30, 1998,   1,   1,    13,  1999,      3,     30},
                { L_, 30, 1998,   1,   1,    14,  1999,      4,     30},
                { L_, 30, 1998,   1,   1,    22,  1999,     12,     30},

                { L_, 30, 1998,   1,   1,    23,  2000,      1,     30},
                { L_, 30, 1998,   1,   1,    24,  2000,      3,     30},
                { L_, 30, 1998,   1,   1,    25,  2000,      4,     30},

                // March.
                { L_, 30, 1998,   3,   1,     3,  1998,      5,     30},
                { L_, 30, 1998,   3,   1,     9,  1998,     11,     30},
                { L_, 30, 1998,   3,   1,    10,  1998,     12,     30},

                { L_, 30, 1998,   3,   1,    11,  1999,      1,     30},
                { L_, 30, 1998,   3,   1,    12,  1999,      3,     30},
                { L_, 30, 1998,   3,   1,    13,  1999,      4,     30},
                { L_, 30, 1998,   3,   1,    21,  1999,     12,     30},

                { L_, 30, 1998,   3,   1,    22,  2000,      1,     30},
                { L_, 30, 1998,   3,   1,    23,  2000,      3,     30},
                { L_, 30, 1998,   3,   1,    24,  2000,      4,     30},

                // June.
                { L_, 30, 1998,   6,   1,     3,  1998,      8,     30},
                { L_, 30, 1998,   6,   1,     6,  1998,     11,     30},
                { L_, 30, 1998,   6,   1,     7,  1998,     12,     30},

                { L_, 30, 1998,   6,   1,     8,  1999,      1,     30},
                { L_, 30, 1998,   6,   1,     9,  1999,      3,     30},
                { L_, 30, 1998,   6,   1,    10,  1999,      4,     30},
                { L_, 30, 1998,   6,   1,    18,  1999,     12,     30},

                { L_, 30, 1998,   6,   1,    19,  2000,      1,     30},
                { L_, 30, 1998,   6,   1,    20,  2000,      3,     30},
                { L_, 30, 1998,   6,   1,    21,  2000,      4,     30},

                // December.
                { L_, 30, 1998,  12,   1,     1,  1998,     12,     30},

                { L_, 30, 1998,  12,   1,     2,  1999,      1,     30},
                { L_, 30, 1998,  12,   1,     3,  1999,      3,     30},
                { L_, 30, 1998,  12,   1,     4,  1999,      4,     30},
                { L_, 30, 1998,  12,   1,    12,  1999,     12,     30},

                { L_, 30, 1998,  12,   1,    13,  2000,      1,     30},
                { L_, 30, 1998,  12,   1,    14,  2000,      3,     30},
                { L_, 30, 1998,  12,   1,    15,  2000,      4,     30},

                // test dom == 31
                // January.
                { L_, 31, 1998,   1,   1,     3,  1998,      5,     31},
                { L_, 31, 1998,   1,   1,     6,  1998,     10,     31},
                { L_, 31, 1998,   1,   1,     7,  1998,     12,     31},

                { L_, 31, 1998,   1,   1,     8,  1999,      1,     31},
                { L_, 31, 1998,   1,   1,     9,  1999,      3,     31},
                { L_, 31, 1998,   1,   1,    14,  1999,     12,     31},

                { L_, 31, 1998,   1,   1,    15,  2000,      1,     31},
                { L_, 31, 1998,   1,   1,    16,  2000,      3,     31},
                { L_, 31, 1998,   1,   1,    17,  2000,      5,     31},

                // March.
                { L_, 31, 1998,   3,   1,     3,  1998,      7,     31},
                { L_, 31, 1998,   3,   1,     6,  1998,     12,     31},

                { L_, 31, 1998,   3,   1,     7,  1999,      1,     31},
                { L_, 31, 1998,   3,   1,     8,  1999,      3,     31},
                { L_, 31, 1998,   3,   1,    13,  1999,     12,     31},

                { L_, 31, 1998,   3,   1,    14,  2000,      1,     31},
                { L_, 31, 1998,   3,   1,    15,  2000,      3,     31},
                { L_, 31, 1998,   3,   1,    16,  2000,      5,     31},

                // June.
                { L_, 31, 1998,   6,   1,     3,  1998,     10,     31},
                { L_, 31, 1998,   6,   1,     4,  1998,     12,     31},

                { L_, 31, 1998,   6,   1,     5,  1999,      1,     31},
                { L_, 31, 1998,   6,   1,     6,  1999,      3,     31},
                { L_, 31, 1998,   6,   1,    11,  1999,     12,     31},

                { L_, 31, 1998,   6,   1,    12,  2000,      1,     31},
                { L_, 31, 1998,   6,   1,    13,  2000,      3,     31},
                { L_, 31, 1998,   6,   1,    14,  2000,      5,     31},

                // December.
                { L_, 31, 1998,  12,   1,     1,  1998,     12,     31},

                { L_, 31, 1998,  12,   1,     2,  1999,      1,     31},
                { L_, 31, 1998,  12,   1,     3,  1999,      3,     31},
                { L_, 31, 1998,  12,   1,     8,  1999,     12,     31},

                { L_, 31, 1998,  12,   1,     9,  2000,      1,     31},
                { L_, 31, 1998,  12,   1,    10,  2000,      3,     31},
                { L_, 31, 1998,  12,   1,    11,  2000,      5,     31},

                // count < 0
                { L_,  1,    1,   4,   1,    -2,     1,      2,      1},
                { L_,  1,    1,   4,   2,    -3,     1,      2,      1},
                { L_,  3,    1,   4,   2,    -3,     1,      1,      3},
                { L_, 29,    1,   4,  28,    -2,     1,      1,     29},
                { L_, 29,    1,   4,  30,    -2,     1,      3,     29},

                { L_,  1,    1,   3,   1,    -2,     1,      1,      1},
                { L_,  1,    1,   3,   2,    -2,     1,      2,      1},
                { L_,  3,    1,   3,   2,    -2,     1,      1,      3},
                { L_, 30,    1,   4,  29,    -2,     1,      1,     30},
                { L_, 30,    1,   5,   1,    -2,     1,      3,     30},

                // test dom <= 28
                // December.
                { L_,  1, 2001,  12,   1,   -10,  2001,      2,      1},
                { L_, 27, 2001,  12,   1,   -10,  2001,      2,     27},
                { L_, 28, 2001,  12,   1,   -10,  2001,      2,     28},

                { L_,  1, 2001,  12,   1,   -11,  2001,      1,      1},
                { L_, 27, 2001,  12,   1,   -11,  2001,      1,     27},
                { L_, 28, 2001,  12,   1,   -11,  2001,      1,     28},

                { L_,  1, 2001,  12,   1,   -12,  2000,     12,      1},
                { L_, 27, 2001,  12,   1,   -12,  2000,     12,     27},
                { L_, 28, 2001,  12,   1,   -12,  2000,     12,     28},

                { L_,  1, 2001,  12,   1,   -13,  2000,     11,      1},
                { L_, 27, 2001,  12,   1,   -13,  2000,     11,     27},
                { L_, 28, 2001,  12,   1,   -13,  2000,     11,     28},

                { L_,  1, 2001,  12,   1,   -23,  2000,      1,      1},
                { L_, 27, 2001,  12,   1,   -23,  2000,      1,     27},
                { L_, 28, 2001,  12,   1,   -23,  2000,      1,     28},

                { L_,  1, 2001,  12,   1,   -24,  1999,     12,      1},
                { L_, 27, 2001,  12,   1,   -24,  1999,     12,     27},
                { L_, 28, 2001,  12,   1,   -24,  1999,     12,     28},

                { L_,  1, 2001,  12,   1,   -25,  1999,     11,      1},
                { L_, 27, 2001,  12,   1,   -25,  1999,     11,     27},
                { L_, 28, 2001,  12,   1,   -25,  1999,     11,     28},

                // June.
                { L_,  1, 2001,   6,   1,    -5,  2001,      1,      1},
                { L_, 27, 2001,   6,   1,    -5,  2001,      1,     27},
                { L_, 28, 2001,   6,   1,    -5,  2001,      1,     28},

                { L_,  1, 2001,   6,   1,    -6,  2000,     12,      1},
                { L_, 27, 2001,   6,   1,    -6,  2000,     12,     27},
                { L_, 28, 2001,   6,   1,    -6,  2000,     12,     28},

                { L_,  1, 2001,   6,   1,    -7,  2000,     11,      1},
                { L_, 27, 2001,   6,   1,    -7,  2000,     11,     27},
                { L_, 28, 2001,   6,   1,    -7,  2000,     11,     28},

                { L_,  1, 2001,   6,   1,   -17,  2000,      1,      1},
                { L_, 27, 2001,   6,   1,   -17,  2000,      1,     27},
                { L_, 28, 2001,   6,   1,   -17,  2000,      1,     28},

                { L_,  1, 2001,   6,   1,   -18,  1999,     12,      1},
                { L_, 27, 2001,   6,   1,   -18,  1999,     12,     27},
                { L_, 28, 2001,   6,   1,   -18,  1999,     12,     28},

                { L_,  1, 2001,   6,   1,   -19,  1999,     11,      1},
                { L_, 27, 2001,   6,   1,   -19,  1999,     11,     27},
                { L_, 28, 2001,   6,   1,   -19,  1999,     11,     28},

                // January.
                { L_,  1, 2001,   1,   1,    -1,  2000,     12,      1},
                { L_, 27, 2001,   1,   1,    -1,  2000,     12,     27},
                { L_, 28, 2001,   1,   1,    -1,  2000,     12,     28},

                { L_,  1, 2001,   1,   1,    -2,  2000,     11,      1},
                { L_, 27, 2001,   1,   1,    -2,  2000,     11,     27},
                { L_, 28, 2001,   1,   1,    -2,  2000,     11,     28},

                { L_,  1, 2001,   1,   1,   -12,  2000,      1,      1},
                { L_, 27, 2001,   1,   1,   -12,  2000,      1,     27},
                { L_, 28, 2001,   1,   1,   -12,  2000,      1,     28},

                { L_,  1, 2001,   1,   1,   -13,  1999,     12,      1},
                { L_, 27, 2001,   1,   1,   -13,  1999,     12,     27},
                { L_, 28, 2001,   1,   1,   -13,  1999,     12,     28},

                { L_,  1, 2001,   1,   1,   -14,  1999,     11,      1},
                { L_, 27, 2001,   1,   1,   -14,  1999,     11,     27},
                { L_, 28, 2001,   1,   1,   -14,  1999,     11,     28},

                // test dom == 29
                // December.
                { L_, 29, 2001,  12,   1,    -3,  2001,      9,     29},
                { L_, 29, 2001,  12,   1,    -6,  2001,      6,     29},
                { L_, 29, 2001,  12,   1,    -9,  2001,      3,     29},
                { L_, 29, 2001,  12,   1,   -10,  2001,      1,     29},

                { L_, 29, 2001,  12,   1,   -11,  2000,     12,     29},
                { L_, 29, 2001,  12,   1,   -12,  2000,     11,     29},
                { L_, 29, 2001,  12,   1,   -21,  2000,      2,     29},
                { L_, 29, 2001,  12,   1,   -22,  2000,      1,     29},

                { L_, 29, 2001,  12,   1,   -23,  1999,     12,     29},
                { L_, 29, 2001,  12,   1,   -24,  1999,     11,     29},
                { L_, 29, 2001,  12,   1,   -32,  1999,      3,     29},
                { L_, 29, 2001,  12,   1,   -33,  1999,      1,     29},

                // June.
                { L_, 29, 2001,   6,   1,    -2,  2001,      4,     29},
                { L_, 29, 2001,   6,   1,    -3,  2001,      3,     29},
                { L_, 29, 2001,   6,   1,    -4,  2001,      1,     29},

                { L_, 29, 2001,   6,   1,    -5,  2000,     12,     29},
                { L_, 29, 2001,   6,   1,    -6,  2000,     11,     29},
                { L_, 29, 2001,   6,   1,   -15,  2000,      2,     29},
                { L_, 29, 2001,   6,   1,   -16,  2000,      1,     29},

                { L_, 29, 2001,   6,   1,   -17,  1999,     12,     29},
                { L_, 29, 2001,   6,   1,   -18,  1999,     11,     29},
                { L_, 29, 2001,   6,   1,   -26,  1999,      3,     29},
                { L_, 29, 2001,   6,   1,   -27,  1999,      1,     29},

                // March.
                { L_, 29, 2001,   3,   1,    -1,  2001,      1,     29},

                { L_, 29, 2001,   3,   1,    -2,  2000,     12,     29},
                { L_, 29, 2001,   3,   1,    -3,  2000,     11,     29},
                { L_, 29, 2001,   3,   1,   -12,  2000,      2,     29},
                { L_, 29, 2001,   3,   1,   -13,  2000,      1,     29},

                { L_, 29, 2001,   3,   1,   -14,  1999,     12,     29},
                { L_, 29, 2001,   3,   1,   -15,  1999,     11,     29},
                { L_, 29, 2001,   3,   1,   -23,  1999,      3,     29},
                { L_, 29, 2001,   3,   1,   -24,  1999,      1,     29},

                // January.
                { L_, 29, 2001,   1,   1,    -1,  2000,     12,     29},
                { L_, 29, 2001,   1,   1,    -2,  2000,     11,     29},
                { L_, 29, 2001,   1,   1,   -11,  2000,      2,     29},
                { L_, 29, 2001,   1,   1,   -12,  2000,      1,     29},

                { L_, 29, 2001,   1,   1,   -13,  1999,     12,     29},
                { L_, 29, 2001,   1,   1,   -14,  1999,     11,     29},
                { L_, 29, 2001,   1,   1,   -22,  1999,      3,     29},
                { L_, 29, 2001,   1,   1,   -23,  1999,      1,     29},

                // test dom == 30
                // December.
                { L_, 30, 2001,  12,   1,    -3,  2001,      9,     30},
                { L_, 30, 2001,  12,   1,    -6,  2001,      6,     30},
                { L_, 30, 2001,  12,   1,    -9,  2001,      3,     30},
                { L_, 30, 2001,  12,   1,   -10,  2001,      1,     30},

                { L_, 30, 2001,  12,   1,   -11,  2000,     12,     30},
                { L_, 30, 2001,  12,   1,   -12,  2000,     11,     30},
                { L_, 30, 2001,  12,   1,   -20,  2000,      3,     30},
                { L_, 30, 2001,  12,   1,   -21,  2000,      1,     30},

                { L_, 30, 2001,  12,   1,   -22,  1999,     12,     30},
                { L_, 30, 2001,  12,   1,   -23,  1999,     11,     30},
                { L_, 30, 2001,  12,   1,   -31,  1999,      3,     30},
                { L_, 30, 2001,  12,   1,   -32,  1999,      1,     30},

                // June.
                { L_, 30, 2001,   6,   1,    -2,  2001,      4,     30},
                { L_, 30, 2001,   6,   1,    -3,  2001,      3,     30},
                { L_, 30, 2001,   6,   1,    -4,  2001,      1,     30},

                { L_, 30, 2001,   6,   1,    -5,  2000,     12,     30},
                { L_, 30, 2001,   6,   1,    -6,  2000,     11,     30},
                { L_, 30, 2001,   6,   1,   -14,  2000,      3,     30},
                { L_, 30, 2001,   6,   1,   -15,  2000,      1,     30},

                { L_, 30, 2001,   6,   1,   -16,  1999,     12,     30},
                { L_, 30, 2001,   6,   1,   -17,  1999,     11,     30},
                { L_, 30, 2001,   6,   1,   -25,  1999,      3,     30},
                { L_, 30, 2001,   6,   1,   -26,  1999,      1,     30},

                // March.
                { L_, 30, 2001,   3,   1,    -1,  2001,      1,     30},

                { L_, 30, 2001,   3,   1,    -2,  2000,     12,     30},
                { L_, 30, 2001,   3,   1,    -3,  2000,     11,     30},
                { L_, 30, 2001,   3,   1,   -11,  2000,      3,     30},
                { L_, 30, 2001,   3,   1,   -12,  2000,      1,     30},

                { L_, 30, 2001,   3,   1,   -13,  1999,     12,     30},
                { L_, 30, 2001,   3,   1,   -14,  1999,     11,     30},
                { L_, 30, 2001,   3,   1,   -22,  1999,      3,     30},
                { L_, 30, 2001,   3,   1,   -23,  1999,      1,     30},

                // January.
                { L_, 30, 2001,   1,   1,    -1,  2000,     12,     30},
                { L_, 30, 2001,   1,   1,    -2,  2000,     11,     30},
                { L_, 30, 2001,   1,   1,   -10,  2000,      3,     30},
                { L_, 30, 2001,   1,   1,   -11,  2000,      1,     30},

                { L_, 30, 2001,   1,   1,   -12,  1999,     12,     30},
                { L_, 30, 2001,   1,   1,   -13,  1999,     11,     30},
                { L_, 30, 2001,   1,   1,   -21,  1999,      3,     30},
                { L_, 30, 2001,   1,   1,   -22,  1999,      1,     30},

                // test dom == 31
                // December.
                { L_, 31, 2001,  12,   1,    -3,  2001,      7,     31},
                { L_, 31, 2001,  12,   1,    -5,  2001,      3,     31},
                { L_, 31, 2001,  12,   1,    -6,  2001,      1,     31},

                { L_, 31, 2001,  12,   1,    -7,  2000,     12,     31},
                { L_, 31, 2001,  12,   1,    -8,  2000,     10,     31},
                { L_, 31, 2001,  12,   1,   -12,  2000,      3,     31},
                { L_, 31, 2001,  12,   1,   -13,  2000,      1,     31},

                { L_, 31, 2001,  12,   1,   -14,  1999,     12,     31},
                { L_, 31, 2001,  12,   1,   -15,  1999,     10,     31},
                { L_, 31, 2001,  12,   1,   -19,  1999,      3,     31},
                { L_, 31, 2001,  12,   1,   -20,  1999,      1,     31},

                // June.
                { L_, 31, 2001,   6,   1,    -2,  2001,      3,     31},
                { L_, 31, 2001,   6,   1,    -3,  2001,      1,     31},

                { L_, 31, 2001,   6,   1,    -4,  2000,     12,     31},
                { L_, 31, 2001,   6,   1,    -5,  2000,     10,     31},
                { L_, 31, 2001,   6,   1,    -9,  2000,      3,     31},
                { L_, 31, 2001,   6,   1,   -10,  2000,      1,     31},

                { L_, 31, 2001,   6,   1,   -11,  1999,     12,     31},
                { L_, 31, 2001,   6,   1,   -12,  1999,     10,     31},
                { L_, 31, 2001,   6,   1,   -16,  1999,      3,     31},
                { L_, 31, 2001,   6,   1,   -17,  1999,      1,     31},

                // March.
                { L_, 31, 2001,   3,   1,    -1,  2001,      1,     31},

                { L_, 31, 2001,   3,   1,    -2,  2000,     12,     31},
                { L_, 31, 2001,   3,   1,    -3,  2000,     10,     31},
                { L_, 31, 2001,   3,   1,    -7,  2000,      3,     31},
                { L_, 31, 2001,   3,   1,    -8,  2000,      1,     31},

                { L_, 31, 2001,   3,   1,    -9,  1999,     12,     31},
                { L_, 31, 2001,   3,   1,   -10,  1999,     10,     31},
                { L_, 31, 2001,   3,   1,   -14,  1999,      3,     31},
                { L_, 31, 2001,   3,   1,   -15,  1999,      1,     31},

                // January.
                { L_, 31, 2001,   1,   1,    -1,  2000,     12,     31},
                { L_, 31, 2001,   1,   1,    -2,  2000,     10,     31},
                { L_, 31, 2001,   1,   1,    -6,  2000,      3,     31},
                { L_, 31, 2001,   1,   1,    -7,  2000,      1,     31},

                { L_, 31, 2001,   1,   1,    -8,  1999,     12,     31},
                { L_, 31, 2001,   1,   1,    -9,  1999,     10,     31},
                { L_, 31, 2001,   1,   1,   -13,  1999,      3,     31},
                { L_, 31, 2001,   1,   1,   -14,  1999,      1,     31},

                // testing which30MonthFromStart
                { L_, 30, 2000,   1,  30,     1,  2000,      3,     30},
                { L_, 30, 2000,   1,  30,     2,  2000,      4,     30},
                { L_, 30, 2000,   1,  30,     3,  2000,      5,     30},
                { L_, 30, 2000,   1,  30,     4,  2000,      6,     30},
                { L_, 30, 2000,   1,  30,     5,  2000,      7,     30},
                { L_, 30, 2000,   1,  30,     6,  2000,      8,     30},
                { L_, 30, 2000,   1,  30,     7,  2000,      9,     30},
                { L_, 30, 2000,   1,  30,     8,  2000,     10,     30},
                { L_, 30, 2000,   1,  30,     9,  2000,     11,     30},
                { L_, 30, 2000,   1,  30,    10,  2000,     12,     30},

                // testing which31MonthFromStart
                { L_, 31, 2000,   1,  31,     1,  2000,      3,     31},
                { L_, 31, 2000,   1,  31,     2,  2000,      5,     31},
                { L_, 31, 2000,   1,  31,     3,  2000,      7,     31},
                { L_, 31, 2000,   1,  31,     4,  2000,      8,     31},
                { L_, 31, 2000,   1,  31,     5,  2000,     10,     31},
                { L_, 31, 2000,   1,  31,     6,  2000,     12,     31},

                // testing which30MonthFromEnd
                { L_, 30, 2000,  12,  30,    -1,  2000,     11,     30},
                { L_, 30, 2000,  12,  30,    -2,  2000,     10,     30},
                { L_, 30, 2000,  12,  30,    -3,  2000,      9,     30},
                { L_, 30, 2000,  12,  30,    -4,  2000,      8,     30},
                { L_, 30, 2000,  12,  30,    -5,  2000,      7,     30},
                { L_, 30, 2000,  12,  30,    -6,  2000,      6,     30},
                { L_, 30, 2000,  12,  30,    -7,  2000,      5,     30},
                { L_, 30, 2000,  12,  30,    -8,  2000,      4,     30},
                { L_, 30, 2000,  12,  30,    -9,  2000,      3,     30},
                { L_, 30, 2000,  12,  30,   -10,  2000,      1,     30},

                // testing which31MonthFromEnd
                { L_, 31, 2000,  12,  31,    -1,  2000,     10,     31},
                { L_, 31, 2000,  12,  31,    -2,  2000,      8,     31},
                { L_, 31, 2000,  12,  31,    -3,  2000,      7,     31},
                { L_, 31, 2000,  12,  31,    -4,  2000,      5,     31},
                { L_, 31, 2000,  12,  31,    -5,  2000,      3,     31},
                { L_, 31, 2000,  12,  31,    -6,  2000,      1,     31},

                // testing num30MonthAbove
                { L_, 29, 2000,   1,  29,     1,  2000,      2,     29},
                { L_, 29, 2000,   2,  29,     1,  2000,      3,     29},
                { L_, 29, 2000,   3,  29,     1,  2000,      4,     29},
                { L_, 29, 2000,   4,  29,     1,  2000,      5,     29},
                { L_, 29, 2000,   5,  29,     1,  2000,      6,     29},
                { L_, 29, 2000,   6,  29,     1,  2000,      7,     29},
                { L_, 29, 2000,   7,  29,     1,  2000,      8,     29},
                { L_, 29, 2000,   8,  29,     1,  2000,      9,     29},
                { L_, 29, 2000,   9,  29,     1,  2000,     10,     29},
                { L_, 29, 2000,  10,  29,     1,  2000,     11,     29},
                { L_, 29, 2000,  11,  29,     1,  2000,     12,     29},
                { L_, 29, 2000,  12,  29,     1,  2001,      1,     29},

                // testing num31MonthAbove
                { L_, 31, 2000,   1,  31,     1,  2000,      3,     31},
                { L_, 31, 2000,   2,  29,     1,  2000,      3,     31},
                { L_, 31, 2000,   3,  31,     1,  2000,      5,     31},
                { L_, 31, 2000,   4,  30,     1,  2000,      5,     31},
                { L_, 31, 2000,   5,  31,     1,  2000,      7,     31},
                { L_, 31, 2000,   6,  30,     1,  2000,      7,     31},
                { L_, 31, 2000,   7,  31,     1,  2000,      8,     31},
                { L_, 31, 2000,   8,  31,     1,  2000,     10,     31},
                { L_, 31, 2000,   9,  30,     1,  2000,     10,     31},
                { L_, 31, 2000,  10,  31,     1,  2000,     12,     31},
                { L_, 31, 2000,  11,  30,     1,  2000,     12,     31},
                { L_, 31, 2000,  12,  31,     1,  2001,      1,     31},

                // testing num30MonthBelow
                { L_, 29, 2000,  12,  29,    -1,  2000,     11,     29},
                { L_, 29, 2000,  11,  29,    -1,  2000,     10,     29},
                { L_, 29, 2000,  10,  29,    -1,  2000,      9,     29},
                { L_, 29, 2000,   9,  29,    -1,  2000,      8,     29},
                { L_, 29, 2000,   8,  29,    -1,  2000,      7,     29},
                { L_, 29, 2000,   7,  29,    -1,  2000,      6,     29},
                { L_, 29, 2000,   6,  29,    -1,  2000,      5,     29},
                { L_, 29, 2000,   5,  29,    -1,  2000,      4,     29},
                { L_, 29, 2000,   4,  29,    -1,  2000,      3,     29},
                { L_, 29, 2000,   3,  29,    -1,  2000,      2,     29},
                { L_, 29, 2000,   2,  29,    -1,  2000,      1,     29},
                { L_, 29, 2000,   1,  29,    -1,  1999,     12,     29},

                // testing num31MonthBelow
                { L_, 31, 2000,  12,  31,    -1,  2000,     10,     31},
                { L_, 31, 2000,  11,  30,    -1,  2000,     10,     31},
                { L_, 31, 2000,  10,  31,    -1,  2000,      8,     31},
                { L_, 31, 2000,   9,  30,    -1,  2000,      8,     31},
                { L_, 31, 2000,   8,  31,    -1,  2000,      7,     31},
                { L_, 31, 2000,   7,  31,    -1,  2000,      5,     31},
                { L_, 31, 2000,   6,  30,    -1,  2000,      5,     31},
                { L_, 31, 2000,   5,  31,    -1,  2000,      3,     31},
                { L_, 31, 2000,   4,  30,    -1,  2000,      3,     31},
                { L_, 31, 2000,   3,  31,    -1,  2000,      1,     31},
                { L_, 31, 2000,   2,  29,    -1,  2000,      1,     31},
                { L_, 31, 2000,   1,  31,    -1,  1999,     12,     31}
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int  LINE   = DATA[ti].d_lineNum;
                const int  DOM    = DATA[ti].d_dom;

                const int  YEAR   = DATA[ti].d_year;
                const int  MONTH  = DATA[ti].d_month;
                const int  DAY    = DATA[ti].d_day;

                const int  COUNT  = DATA[ti].d_count;

                const int  EXP_YEAR  = DATA[ti].d_expYear;
                const int  EXP_MONTH = DATA[ti].d_expMonth;
                const int  EXP_DAY   = DATA[ti].d_expDay;

                const Date INPUT(YEAR, MONTH, DAY);
                const Date EXP_DATE(EXP_YEAR, EXP_MONTH, EXP_DAY);

                const Date NEXT      = Util::nextDay(DOM, INPUT);
                const Date PREVIOUS  = Util::previousDay(DOM, INPUT);

                Date       result     = Util::adjustDay(DOM, INPUT, COUNT);
                Date       resultNext = Util::adjustDay(DOM, INPUT, 1);
                Date       resultPrev = Util::adjustDay(DOM, INPUT, -1);

                LOOP3_ASSERT(LINE, resultNext, NEXT, NEXT     == resultNext);
                LOOP3_ASSERT(LINE, resultPrev, PREVIOUS,
                                                     PREVIOUS == resultPrev);

                LOOP3_ASSERT(LINE, result, EXP_DATE, EXP_DATE == result);
            }
        }

        // Additional special cases
        ASSERT(Date(   1,  2,  1) == Util::adjustDay( 1, Date( 1, 1, 1), 1));
        ASSERT(Date(9999, 10, 31) ==
                                  Util::adjustDay(31, Date(9999, 12, 31), -1));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'previousDay' FUNCTION
        //
        // Concerns:
        //   1. If 'dayOfMonth' is equal to initialDate.dayOfMonth(), previous
        //      'dayOfMonth' date is returned.
        //   2. Correct date is returned when result date is not in same month.
        //
        // Plan:
        //   Use a table based approach to store the test inputs and expected
        //   outputs.
        //
        //   Select data using category partitioning from the following
        //   data categories:
        //
        //   dayOfMonth           date
        //   ----------           ----
        //                  Year  Month  Day
        //                  ----  -----  ---
        //
        //      1           0001  1 - 12   1
        //     27           2000          27
        //     28           9999          28
        //     29                         29
        //     30                         30
        //     31                         31
        //
        //   Specifically test for the following conditions:
        //     0. When initialDate.day() == 'dayOfMonth'
        //     1. When result date lies in the same month as
        //        initialDate.month().
        //     2. When result date lies in initialDate.month() - 1.
        //     3. When result date lies in initialDate.month() - 2.
        //     4. When result date lies in initialDate.year() - 1.
        //
        // Testing:
        //   bdet_Date previousDay(dayOfMonth, initialDate);
        // --------------------------------------------------------------------

        if (verbose) { cout << "\nTesting previousDay"
                            << "\n===================" << endl; }
        {
            static const struct {
                int d_lineNum;     // source line number

                int d_dom;         // day of month

                int d_year;        // input year
                int d_month;       // input month
                int d_day;         // input day

                int d_expYear;     // expected year
                int d_expMonth;    // expected month
                int d_expDay;      // expected day
            } DATA [] = {
                // <------- input ------->    <-expected->
                //lin dom year   mon  day  ExpYear ExpMon ExpDay
                //--- --- ----   ---  ---  ------  ------ ------
                { L_,  1,    1,    1,   2,     1,     1,      1},
                { L_,  1,    1,    2,   1,     1,     1,      1},

                { L_, 30, 9999,   12,  31,  9999,    12,     30},
                { L_, 31, 9999,   12,  31,  9999,    10,     31},

                { L_, 28, 2000,    3,  28,  2000,     2,     28},
                { L_, 29, 2000,    3,  29,  2000,     2,     29},

                // dom < day
                { L_,  1,    1,    1,   2,     1,     1,      1},
                { L_,  1,    1,    1,  31,     1,     1,      1},

                { L_,  1,    1,    2,   2,     1,     2,      1},
                { L_, 27,    1,    2,  28,     1,     2,     27},

                { L_,  1,    1,    4,  30,     1,     4,      1},

                { L_,  1, 2000,    2,   2,  2000,     2,      1},
                { L_, 27, 2000,    2,  28,  2000,     2,     27},
                { L_, 28, 2000,    2,  29,  2000,     2,     28},

                // dom >= day, testing adjacent months
                { L_, 30, 9999,   12,  30,  9999,    11,     30},
                { L_, 30, 9999,   12,  29,  9999,    11,     30},

                { L_, 28, 9999,    2,  28,  9999,     1,     28},
                { L_, 29, 9999,    2,  28,  9999,     1,     29},
                { L_, 30, 9999,    2,  28,  9999,     1,     30},
                { L_, 31, 9999,    2,  28,  9999,     1,     31},

                { L_, 28, 9999,    3,  28,  9999,     2,     28},
                { L_, 28, 9999,    3,  27,  9999,     2,     28},

                { L_, 30, 9999,    4,  30,  9999,     3,     30},
                { L_, 31, 9999,    4,  30,  9999,     3,     31},

                { L_, 28, 2000,    2,  28,  2000,     1,     28},
                { L_, 29, 2000,    2,  28,  2000,     1,     29},
                { L_, 30, 2000,    2,  29,  2000,     1,     30},
                { L_, 31, 2000,    2,  29,  2000,     1,     31},

                { L_, 29, 2000,    3,  29,  2000,     2,     29},
                { L_, 29, 2000,    3,  28,  2000,     2,     29},

                // year change
                { L_,  2, 9999,    1,   1,  9998,    12,      2},
                { L_, 30, 9999,    1,   1,  9998,    12,     30},
                { L_, 31, 9999,    1,   1,  9998,    12,     31},

                // dom >= day, testing multiple months
                { L_, 31, 9999,   12,  30,  9999,    10,     31},
                { L_, 31, 9999,   12,  31,  9999,    10,     31},
                { L_, 31, 9999,   10,  30,  9999,     8,     31},
                { L_, 31, 9999,   10,  31,  9999,     8,     31},
                { L_, 31, 9999,    7,  30,  9999,     5,     31},
                { L_, 31, 9999,    7,  31,  9999,     5,     31},
                { L_, 31, 9999,    5,  30,  9999,     3,     31},
                { L_, 31, 9999,    5,  31,  9999,     3,     31},
                { L_, 29, 9999,    3,  28,  9999,     1,     29},
                { L_, 30, 9999,    3,  29,  9999,     1,     30},
                { L_, 31, 9999,    3,  30,  9999,     1,     31},
                { L_, 31, 9999,    3,  31,  9999,     1,     31},

                { L_, 31, 2000,    3,  30,  2000,     1,     31},
                { L_, 31, 2000,    3,  31,  2000,     1,     31}
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int  LINE   = DATA[ti].d_lineNum;
                const int  DOM    = DATA[ti].d_dom;

                const int  YEAR   = DATA[ti].d_year;
                const int  MONTH  = DATA[ti].d_month;
                const int  DAY    = DATA[ti].d_day;

                const int  EXP_YEAR  = DATA[ti].d_expYear;
                const int  EXP_MONTH = DATA[ti].d_expMonth;
                const int  EXP_DAY   = DATA[ti].d_expDay;

                const Date INPUT(YEAR, MONTH, DAY);
                const Date EXP_DATE(EXP_YEAR, EXP_MONTH, EXP_DAY);

                Date result = Util::previousDay(DOM, INPUT);

                LOOP3_ASSERT(LINE, result, EXP_DATE, EXP_DATE == result);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'nextDay' FUNCTION
        //
        // Concerns:
        //   1. If 'dayOfMonth' is equal to initialDate.dayOfMonth(), next
        //      'dayOfMonth' date is returned.
        //   2. Correct date is returned when result date is not in same month.
        //
        // Plan:
        //   Use a table based approach to store the test inputs and expected
        //   outputs.
        //
        //   Select data using category partitioning from the following
        //   data categories:
        //
        //   dayOfMonth           date
        //   ----------           ----
        //                  Year  Month  Day
        //                  ----  -----  ---
        //
        //      1           0001  1 - 12   1
        //     27           2000          27
        //     28           9999          28
        //     29                         29
        //     30                         30
        //     31                         31
        //
        //   Specifically test for the following conditions:
        //     0. When initialDate.day() == 'dayOfMonth'
        //     1. When result date lies in the same month as
        //        initialDate.month().
        //     2. When result date lies in initialDate.month() + 1.
        //     3. When result date lies in initialDate.month() + 2.
        //     4. When result date lies in initialDate.year() + 1.
        //
        // Testing:
        //   bdet_Date nextDay(dayOfMonth, initialDate);
        // --------------------------------------------------------------------

        if (verbose) { cout << "\nTesting nextDay"
                            << "\n===============" << endl; }
        {
            static const struct {
                int d_lineNum;     // source line number

                int d_dom;         // day of month

                int d_year;        // input year
                int d_month;       // input month
                int d_day;         // input day

                int d_expYear;     // expected year
                int d_expMonth;    // expected month
                int d_expDay;      // expected day
            } DATA [] = {
                // <------- input -------> <--- expected ------>
                //lin dom year   mon  day  ExpYear ExpMon ExpDay
                //--- --- ----   ---  ---  ------  ------ ------
                { L_, 30, 9999,   12,  29,  9999,    12,     30},
                { L_, 31, 9999,   12,  30,  9999,    12,     31},

                { L_,  1,    1,    1,   1,     1,     2,      1},
                { L_, 31,    1,   12,  31,     2,     1,     31},

                { L_, 28, 9999,    1,  28,  9999,     2,     28},
                { L_, 29, 9999,    1,  29,  9999,     3,     29},

                { L_, 28, 2000,    1,  28,  2000,     2,     28},
                { L_, 29, 2000,    1,  29,  2000,     2,     29},

                // day < dom
                { L_,  2,    1,    1,   1,     1,     1,      2},
                { L_, 30,    1,    1,   1,     1,     1,     30},
                { L_, 31,    1,    1,   1,     1,     1,     31},

                { L_, 27,    1,    2,   1,     1,     2,     27},
                { L_, 28,    1,    2,   1,     1,     2,     28},
                { L_, 29,    1,    2,   1,     1,     3,     29},
                { L_, 30,    1,    2,   1,     1,     3,     30},
                { L_, 31,    1,    2,   1,     1,     3,     31},

                { L_, 30,    1,    4,   1,     1,     4,     30},
                { L_, 31,    1,    4,   1,     1,     5,     31},

                { L_, 28, 2000,    2,   1,  2000,     2,     28},
                { L_, 29, 2000,    2,   1,  2000,     2,     29},
                { L_, 30, 2000,    2,   1,  2000,     3,     30},
                { L_, 31, 2000,    2,   1,  2000,     3,     31},

                // day >= dom
                { L_,  1,    1,    1,   2,     1,     2,      1},
                { L_, 28,    1,    1,  29,     1,     2,     28},
                { L_, 28,    1,    1,  30,     1,     2,     28},
                { L_, 28,    1,    1,  31,     1,     2,     28},

                { L_, 27,    1,    2,  28,     1,     3,     27},
                { L_, 28,    1,    2,  28,     1,     3,     28},
                { L_, 29,    1,    2,  28,     1,     3,     29},
                { L_, 30,    1,    2,  28,     1,     3,     30},
                { L_, 31,    1,    2,  28,     1,     3,     31},

                { L_, 30,    1,    3,  31,     1,     4,     30},
                { L_, 31,    1,    3,  31,     1,     5,     31},

                { L_, 29,    1,    4,  30,     1,     5,     29},
                { L_, 30,    1,    4,  30,     1,     5,     30},
                { L_, 31,    1,    4,  30,     1,     5,     31},

                { L_, 27, 2000,    1,  28,  2000,     2,     27},
                { L_, 28, 2000,    1,  29,  2000,     2,     28},
                { L_, 29, 2000,    1,  30,  2000,     2,     29},
                { L_, 30, 2000,    1,  31,  2000,     3,     30},

                { L_, 27, 2000,    2,  28,  2000,     3,     27},
                { L_, 28, 2000,    2,  29,  2000,     3,     28},

                // year change, alternate month
                { L_,  1, 9998,   12,   2,  9999,     1,      1},
                { L_, 30, 9998,   12,  31,  9999,     1,     30},

                // Testing multiple months (two months)

                { L_, 29, 9999,    1,  30,  9999,     3,     29},
                { L_, 29, 9999,    1,  31,  9999,     3,     29},

                { L_, 29, 2000,    1,  30,  2000,     2,     29},
                { L_, 30, 2000,    1,  31,  2000,     3,     30}
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int  LINE   = DATA[ti].d_lineNum;
                const int  DOM    = DATA[ti].d_dom;

                const int  YEAR   = DATA[ti].d_year;
                const int  MONTH  = DATA[ti].d_month;
                const int  DAY    = DATA[ti].d_day;

                const int  EXP_YEAR  = DATA[ti].d_expYear;
                const int  EXP_MONTH = DATA[ti].d_expMonth;
                const int  EXP_DAY   = DATA[ti].d_expDay;

                const Date INPUT(YEAR, MONTH, DAY);
                const Date EXP_DATE(EXP_YEAR, EXP_MONTH, EXP_DAY);

                Date       result = Util::nextDay(DOM, INPUT);

                LOOP3_ASSERT(LINE, result, EXP_DATE, EXP_DATE == result);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'numDaysInRange' FUNCTION
        //
        // Concerns:
        //   1. Function returns 0 when 'date1' > 'date2'
        //   2. All the static arrays used in the function contain correct
        //      values.
        //   3. The special cases when 28 < 'dayOfMonth' <= 31 is handled
        //      correctly.
        //
        // Plan:
        //   Use a table based approach to store the test inputs and expected
        //   outputs.
        //
        //   Select data using category partitioning from the following
        //   data categories:
        //
        //   dayOfMonth           date
        //   ----------           ----
        //                  Year  Month  Day
        //                  ----  -----  ---
        //
        //      1           0001  1 - 12   1
        //     27           1999          27
        //     28           2000          28
        //     29           2001          29
        //     30           9999          30
        //     31                         31
        //
        //   Specifically test for the following conditions:
        //     0. When 'date1' > 'date2'
        //     1. When there are no days in the specified range.
        //     2. When both 'date1' and 'date2' lie in the same year and month.
        //     3. Check when 'dayOfMonth' <= 28 and 28 < 'dayOfMonth' <= 31.
        //     4. Verify year arithmetic by repeating 3. when 'date1' and
        //        'date2' have different years.
        //     6. Leap years.
        //
        // Testing:
        //   int numDaysInRange(dayOfMonth, date1, date2);
        // --------------------------------------------------------------------

        if (verbose) { cout << "\nTesting numDaysInRange"
                            << "\n======================" << endl; }
        {
            static const struct {
                int d_lineNum;      // source line number

                int d_dom;          // day of month

                int d_year1;        // input year1
                int d_month1;       // input month1
                int d_day1;         // input day1

                int d_year2;        // input year2
                int d_month2;       // input month2
                int d_day2;         // input day2

                int d_expected;     // expected return value
            } DATA [] = {
                // <-------------- input ---------------> <-expected->
                //lin dom year1 mon1 day1 year2 mon2 day2 result
                //--- --- ----- ---- ---- ----- ---- ---- ------

                { L_,  2,  2000,   5,   3,  2000,   5,   2,       0},
                { L_,  9,  0001,   9,   9,  0001,   9,   8,       0},

                { L_,  28, 2000,   9,   29, 2000,   9,   29,      0},
                { L_,  29, 2000,   9,   29, 2000,   9,   29,      1},
                { L_,  30, 2000,   9,   29, 2000,   9,   29,      0},

                { L_,  27, 2000,   9,   28, 2000,   9,   30,      0},
                { L_,  28, 2000,   9,   28, 2000,   9,   30,      1},
                { L_,  29, 2000,   9,   28, 2000,   9,   30,      1},
                { L_,  30, 2000,   9,   28, 2000,   9,   30,      1},
                { L_,  31, 2000,   9,   28, 2000,   9,   30,      0},

                { L_,  26, 2000,   9,   25, 2000,   10,  01,      1},
                { L_,  26, 2000,   9,   26, 2000,   10,  01,      1},
                { L_,  26, 2000,   9,   27, 2000,   10,  01,      0},
                { L_,  28, 2000,   9,   27, 2000,   10,  28,      2},
                { L_,  28, 2000,   9,   27, 2000,   10,  29,      2},
                { L_,  28, 2000,   2,   29, 2000,    3,  27,      0},

                { L_,  26, 2000,   1,   25, 2000,   10,  01,      9},
                { L_,  26, 2000,   1,   26, 2000,   10,  01,      9},
                { L_,  26, 2000,   1,   27, 2000,   10,  01,      8},
                { L_,  28, 2000,   1,   27, 2000,   10,  28,      10},
                { L_,  28, 2000,   1,   27, 2000,   10,  28,      10},

                // checking num31down[]
                { L_,  31, 2000,   1,   1, 2000,   12,  31,      7},
                { L_,  31, 2000,   2,   1, 2000,   12,  31,      6},
                { L_,  31, 2000,   3,   1, 2000,   12,  31,      6},
                { L_,  31, 2000,   4,   1, 2000,   12,  31,      5},
                { L_,  31, 2000,   5,   1, 2000,   12,  31,      5},
                { L_,  31, 2000,   6,   1, 2000,   12,  31,      4},
                { L_,  31, 2000,   7,   1, 2000,   12,  31,      4},
                { L_,  31, 2000,   8,   1, 2000,   12,  31,      3},
                { L_,  31, 2000,   9,   1, 2000,   12,  31,      2},
                { L_,  31, 2000,  10,   1, 2000,   12,  31,      2},
                { L_,  31, 2000,  11,   1, 2000,   12,  31,      1},
                { L_,  31, 2000,  12,   1, 2000,   12,  31,      1},

                // checking num30down[]
                { L_,  30, 2000,   1,   1, 2000,   12,  30,     11},
                { L_,  30, 2000,   2,   1, 2000,   12,  30,     10},
                { L_,  30, 2000,   3,   1, 2000,   12,  30,     10},
                { L_,  30, 2000,   4,   1, 2000,   12,  30,      9},
                { L_,  30, 2000,   5,   1, 2000,   12,  30,      8},
                { L_,  30, 2000,   6,   1, 2000,   12,  30,      7},
                { L_,  30, 2000,   7,   1, 2000,   12,  30,      6},
                { L_,  30, 2000,   8,   1, 2000,   12,  30,      5},
                { L_,  30, 2000,   9,   1, 2000,   12,  30,      4},
                { L_,  30, 2000,  10,   1, 2000,   12,  30,      3},
                { L_,  30, 2000,  11,   1, 2000,   12,  30,      2},
                { L_,  30, 2000,  12,   1, 2000,   12,  30,      1},

                { L_,  29, 2000,   1,   1, 2000,    3,   1,      2},
                { L_,  29, 1999,   1,   1, 1999,    3,   1,      1},

                { L_,  26, 1999,   1,   25, 2000,   10,  01,     21},
                { L_,  26, 1999,   1,   26, 2000,   10,  01,     21},
                { L_,  26, 1999,   1,   27, 2000,   10,  01,     20},
                { L_,  28, 1999,   1,   27, 2000,   10,  28,     22},
                { L_,  28, 1999,   1,   27, 2000,   10,  28,     22},

                { L_,  26, 1999,   1,   25, 2000,   10,  01,     21},
                { L_,  26, 1999,   1,   26, 2000,   10,  01,     21},
                { L_,  26, 1999,   1,   27, 2000,   10,  01,     20},
                { L_,  28, 1999,   1,   27, 2000,   10,  28,     22},
                { L_,  28, 1999,   1,   27, 2000,   10,  28,     22},

                { L_,  31, 1999,   1,   1,  2001,   1,   1,     14},
                { L_,  31, 1999,   2,   1,  2001,   2,   1,     14},
                { L_,  31, 1999,   3,   1,  2001,   3,   1,     14},
                { L_,  31, 1999,   4,   1,  2001,   4,   1,     14},
                { L_,  31, 1999,   5,   1,  2001,   5,   1,     14},
                { L_,  31, 1999,   6,   1,  2001,   6,   1,     14},
                { L_,  31, 1999,   7,   1,  2001,   7,   1,     14},
                { L_,  31, 1999,   8,   1,  2001,   8,   1,     14},
                { L_,  31, 1999,   9,   1,  2001,   9,   1,     14},
                { L_,  31, 1999,  10,   1,  2001,  10,   1,     14},
                { L_,  31, 1999,  11,   1,  2001,  11,   1,     14},
                { L_,  31, 1999,  12,   1,  2001,  12,   1,     14},

                { L_,  31, 1999,   1,   1,  2001,  12,  31,     21},
                { L_,  31, 1999,   2,   1,  2001,  12,  31,     20},
                { L_,  31, 1999,   3,   1,  2001,  12,  31,     20},
                { L_,  31, 1999,   4,   1,  2001,  12,  31,     19},
                { L_,  31, 1999,   5,   1,  2001,  12,  31,     19},
                { L_,  31, 1999,   6,   1,  2001,  12,  31,     18},
                { L_,  31, 1999,   7,   1,  2001,  12,  31,     18},
                { L_,  31, 1999,   8,   1,  2001,  12,  31,     17},
                { L_,  31, 1999,   9,   1,  2001,  12,  31,     16},
                { L_,  31, 1999,  10,   1,  2001,  12,  31,     16},
                { L_,  31, 1999,  11,   1,  2001,  12,  31,     15},
                { L_,  31, 1999,  12,   1,  2001,  12,  31,     15},

                { L_,  31, 1999,   1,   1,  2001,  12,   1,     20},
                { L_,  31, 1999,   1,   1,  2001,  11,   1,     20},
                { L_,  31, 1999,   1,   1,  2001,  10,   1,     19},
                { L_,  31, 1999,   1,   1,  2001,   9,   1,     19},
                { L_,  31, 1999,   1,   1,  2001,   8,   1,     18},
                { L_,  31, 1999,   1,   1,  2001,   7,   1,     17},
                { L_,  31, 1999,   1,   1,  2001,   6,   1,     17},
                { L_,  31, 1999,   1,   1,  2001,   5,   1,     16},
                { L_,  31, 1999,   1,   1,  2001,   4,   1,     16},
                { L_,  31, 1999,   1,   1,  2001,   3,   1,     15},
                { L_,  31, 1999,   1,   1,  2001,   2,   1,     15},
                { L_,  31, 1999,   1,   1,  2001,   1,   1,     14},

                { L_,  30, 1999,   1,   1,  2001,   1,   1,     22},
                { L_,  30, 1999,   2,   1,  2001,   2,   1,     22},
                { L_,  30, 1999,   3,   1,  2001,   3,   1,     22},
                { L_,  30, 1999,   4,   1,  2001,   4,   1,     22},
                { L_,  30, 1999,   5,   1,  2001,   5,   1,     22},
                { L_,  30, 1999,   6,   1,  2001,   6,   1,     22},
                { L_,  30, 1999,   7,   1,  2001,   7,   1,     22},
                { L_,  30, 1999,   8,   1,  2001,   8,   1,     22},
                { L_,  30, 1999,   9,   1,  2001,   9,   1,     22},
                { L_,  30, 1999,  10,   1,  2001,  10,   1,     22},
                { L_,  30, 1999,  11,   1,  2001,  11,   1,     22},
                { L_,  30, 1999,  12,   1,  2001,  12,   1,     22},

                { L_,  30, 1999,   1,   1,  2001,  12,  31,     33},
                { L_,  30, 1999,   2,   1,  2001,  12,  31,     32},
                { L_,  30, 1999,   3,   1,  2001,  12,  31,     32},
                { L_,  30, 1999,   4,   1,  2001,  12,  31,     31},
                { L_,  30, 1999,   5,   1,  2001,  12,  31,     30},
                { L_,  30, 1999,   6,   1,  2001,  12,  31,     29},
                { L_,  30, 1999,   7,   1,  2001,  12,  31,     28},
                { L_,  30, 1999,   8,   1,  2001,  12,  31,     27},
                { L_,  30, 1999,   9,   1,  2001,  12,  31,     26},
                { L_,  30, 1999,  10,   1,  2001,  12,  31,     25},
                { L_,  30, 1999,  11,   1,  2001,  12,  31,     24},
                { L_,  30, 1999,  12,   1,  2001,  12,  31,     23},

                { L_,  30, 1999,   1,   1,  2001,  12,   1,     32},
                { L_,  30, 1999,   1,   1,  2001,  11,   1,     31},
                { L_,  30, 1999,   1,   1,  2001,  10,   1,     30},
                { L_,  30, 1999,   1,   1,  2001,   9,   1,     29},
                { L_,  30, 1999,   1,   1,  2001,   8,   1,     28},
                { L_,  30, 1999,   1,   1,  2001,   7,   1,     27},
                { L_,  30, 1999,   1,   1,  2001,   6,   1,     26},
                { L_,  30, 1999,   1,   1,  2001,   5,   1,     25},
                { L_,  30, 1999,   1,   1,  2001,   4,   1,     24},
                { L_,  30, 1999,   1,   1,  2001,   3,   1,     23},
                { L_,  30, 1999,   1,   1,  2001,   2,   1,     23},
                { L_,  30, 1999,   1,   1,  2001,   1,   1,     22},

                { L_,  29, 1999,   1,   1, 2000,    3,   1,     13},
                { L_,  29, 1998,   1,   1, 1999,    3,   1,     12},
                { L_,  29, 2000,   1,   1, 2001,    3,   1,     13}
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int  LINE   = DATA[ti].d_lineNum;
                const int  DOM    = DATA[ti].d_dom;

                const int  YEAR1  = DATA[ti].d_year1;
                const int  MONTH1 = DATA[ti].d_month1;
                const int  DAY1   = DATA[ti].d_day1;

                const int  YEAR2  = DATA[ti].d_year2;
                const int  MONTH2 = DATA[ti].d_month2;
                const int  DAY2   = DATA[ti].d_day2;

                const int  EXP    = DATA[ti].d_expected;

                const Date DATE1(YEAR1, MONTH1, DAY1);
                const Date DATE2(YEAR2, MONTH2, DAY2);

                int        result = Util::numDaysInRange(DOM, DATE1, DATE2);

                LOOP3_ASSERT(LINE, result, EXP, EXP == result);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   This test case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;
        // TBD
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
