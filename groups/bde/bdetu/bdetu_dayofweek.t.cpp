// bdetu_dayofweek.t.cpp        -*-C++-*-

#include <bdetu_dayofweek.h>

#include <bdet_date.h>
#include <bdet_dayofweek.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test is a utility for day-of-week calculations.  In this
// test driver, each function is tested in a separate test case.  Cross-product
// partitioning is used as the data selection method.  The component implements
// all date calculations using 'bdet_Date', which already compensates for
// calendar irregularities.  A white-box test approach is employed because of
// the implementation's simplicity.  All the data that would be used in a
// black-box category-partitioning scheme will also fall under the white-box
// partitions.
//
// The following abbreviations are used throughout this documentation:
//     c     = const
//     b_DOW = bdet_DayOfWeek::Day
//     b_D   = bdet_Date
//-----------------------------------------------------------------------------
// FUNCTIONS
// [12] int numDaysInRange(b_DOW dayOfWeek, c b_D& date1, c b_D& date2);
// [ 3] b_D nextDay(b_DOW dayOfWeek, c b_D& date);
// [ 5] b_D ceilDay(b_DOW dayOfWeek, c b_D& date);
// [ 4] b_D previousDay(b_DOW dayOfWeek, c b_D& date);
// [ 6] b_D floorDay(b_DOW dayOfWeek, c b_D& date);
// [ 7] b_D adjustDay(b_DOW dayOfWeek, c b_D& initialDate, int count);
// [ 8] b_D ceilAdjustDay(b_DOW dayOfWeek, c b_D& initialDate, int count);
// [ 9] b_D floorAdjustDay(b_DOW dayOfWeek, c b_D& initialDate, int count);
// [10] b_D ceilOrPreviousDayInMonth(b_DOW dayOfWeek, c b_D& date);
// [11] b_D floorOrNextDayInMonth(b_DOW dayOfWeek, c b_D& date);
// [13] int weekdayInMonth(int year, int month, b_DOW dayOfWeek, int ordinal);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [14] USAGE EXAMPLE
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
typedef bdetu_DayOfWeek Util;

const bdet_DayOfWeek::Day SUN = bdet_DayOfWeek::BDET_SUN;
const bdet_DayOfWeek::Day MON = bdet_DayOfWeek::BDET_MON;
const bdet_DayOfWeek::Day TUE = bdet_DayOfWeek::BDET_TUE;
const bdet_DayOfWeek::Day WED = bdet_DayOfWeek::BDET_WED;
const bdet_DayOfWeek::Day THU = bdet_DayOfWeek::BDET_THU;
const bdet_DayOfWeek::Day FRI = bdet_DayOfWeek::BDET_FRI;
const bdet_DayOfWeek::Day SAT = bdet_DayOfWeek::BDET_SAT;

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
      case 14: {
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
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'numDaysInRange' FUNCTION
        //
        // Concerns:
        //   We are concerned that this function works not only for March,
        //   April, and October, but for all months including February on a
        //   leap year.  We will use the Category Partitioning method applied
        //   via a simple table-based implementation.
        //
        // Testing:
        //   int weekdayInMonth(int yr, int mo, b_DOW day, int ordinal);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'numDaysInRange' Function"
                          << "\n=================================" << endl;

        {
                    if (verbose) cout << "\nweekdaysInMonth" << endl;

            enum { SU = 1, MO, TU, WE, TH, FR, SA };

            enum { FAILURE = 1 };

            static const struct {
                int d_lineNum;          // source line number

                int d_year;             // input [1996 .. 2038]
                int d_month;            // input [1 .. 12]
                int d_dayOfWeek;        // input [1 .. 7]
                int d_weekdayOffset;    // input [-5 .. -1, 1 .. 5]

                int d_expected;         // value returned [0 .. 31]

            } DATA[] = {
                   // <---- input --->         ,- expected day of month
                //lin year mon dow off  exp <-'
                //--- ---- --- --- ---  ---

                // *** testing offset in JAN 2002 (1st is a TUESDAY) ***
                //lin year mon dow off  exp
                //--- ---- --- --- ---  ---
                { L_, 2002,  1, MO, -5,   0  }, // weekday before first DOM
                { L_, 2002,  1, MO, -4,   7  },
                { L_, 2002,  1, MO, -3,  14  },
                { L_, 2002,  1, MO, -2,  21  },
                { L_, 2002,  1, MO, -1,  28  },
                { L_, 2002,  1, MO,  1,   7  },
                { L_, 2002,  1, MO,  2,  14  },
                { L_, 2002,  1, MO,  3,  21  },
                { L_, 2002,  1, MO,  4,  28  },
                { L_, 2002,  1, MO,  5,   0  },

                { L_, 2002,  1, TU, -5,   1  }, // weekday first day of month
                { L_, 2002,  1, TU, -4,   8  },
                { L_, 2002,  1, TU, -3,  15  },
                { L_, 2002,  1, TU, -2,  22  },
                { L_, 2002,  1, TU, -1,  29  },
                { L_, 2002,  1, TU,  1,   1  },
                { L_, 2002,  1, TU,  2,   8  },
                { L_, 2002,  1, TU,  3,  15  },
                { L_, 2002,  1, TU,  4,  22  },
                { L_, 2002,  1, TU,  5,  29  },

                { L_, 2002,  1, TH, -5,   3  }, // weekday last day of month
                { L_, 2002,  1, TH, -4,  10  },
                { L_, 2002,  1, TH, -3,  17  },
                { L_, 2002,  1, TH, -2,  24  },
                { L_, 2002,  1, TH, -1,  31  },
                { L_, 2002,  1, TH,  1,   3  },
                { L_, 2002,  1, TH,  2,  10  },
                { L_, 2002,  1, TH,  3,  17  },
                { L_, 2002,  1, TH,  4,  24  },
                { L_, 2002,  1, TH,  5,  31  },

                { L_, 2002,  1, FR, -5,   0  }, // weekday after last DOM
                { L_, 2002,  1, FR, -4,   4  },
                { L_, 2002,  1, FR, -3,  11  },
                { L_, 2002,  1, FR, -2,  18  },
                { L_, 2002,  1, FR, -1,  25  },
                { L_, 2002,  1, FR,  1,   4  },
                { L_, 2002,  1, FR,  2,  11  },
                { L_, 2002,  1, FR,  3,  18  },
                { L_, 2002,  1, FR,  4,  25  },
                { L_, 2002,  1, FR,  5,   0  },

                // *** testing offset in FEB 2003 (1st is a SATURDAY) ***
                //lin year mon dow off  exp
                //--- ---- --- --- ---  ---
                { L_, 2003,  2, FR, -5,   0  }, // weekday before first DOM
                { L_, 2003,  2, FR, -4,   7  }, // and also last day of month
                { L_, 2003,  2, FR, -3,  14  },
                { L_, 2003,  2, FR, -2,  21  },
                { L_, 2003,  2, FR, -1,  28  },
                { L_, 2003,  2, FR,  1,   7  },
                { L_, 2003,  2, FR,  2,  14  },
                { L_, 2003,  2, FR,  3,  21  },
                { L_, 2003,  2, FR,  4,  28  },
                { L_, 2003,  2, FR,  5,   0  },

                { L_, 2003,  2, SA, -5,   0  }, // weekday first day of month
                { L_, 2003,  2, SA, -4,   1  }, // and also after last DOM
                { L_, 2003,  2, SA, -3,   8  },
                { L_, 2003,  2, SA, -2,  15  },
                { L_, 2003,  2, SA, -1,  22  },
                { L_, 2003,  2, SA,  1,   1  },
                { L_, 2003,  2, SA,  2,   8  },
                { L_, 2003,  2, SA,  3,  15  },
                { L_, 2003,  2, SA,  4,  22  },
                { L_, 2003,  2, SA,  5,   0  },

                // *** testing offset in FEB 2004 (1st is a SUNDAY) ***
                //lin year mon dow off  exp
                { L_, 2004,  2, SA, -5,   0  }, // weekday before first DOM
                { L_, 2004,  2, SA, -4,   7  },
                { L_, 2004,  2, SA, -3,  14  },
                { L_, 2004,  2, SA, -2,  21  },
                { L_, 2004,  2, SA, -1,  28  },
                { L_, 2004,  2, SA,  1,   7  },
                { L_, 2004,  2, SA,  2,  14  },
                { L_, 2004,  2, SA,  3,  21  },
                { L_, 2004,  2, SA,  4,  28  },
                { L_, 2004,  2, SA,  5,   0  },

                { L_, 2004,  2, SU, -5,   1  }, // weekday first day of mon
                { L_, 2004,  2, SU, -4,   8  }, // and also last day of mon
                { L_, 2004,  2, SU, -3,  15  },
                { L_, 2004,  2, SU, -2,  22  },
                { L_, 2004,  2, SU, -1,  29  },
                { L_, 2004,  2, SU,  1,   1  },
                { L_, 2004,  2, SU,  2,   8  },
                { L_, 2004,  2, SU,  3,  15  },
                { L_, 2004,  2, SU,  4,  22  },
                { L_, 2004,  2, SU,  5,  29  },

                { L_, 2004,  2, MO, -5,   0  }, // weekday after last DOM
                { L_, 2004,  2, MO, -4,   2  },
                { L_, 2004,  2, MO, -3,   9  },
                { L_, 2004,  2, MO, -2,  16  },
                { L_, 2004,  2, MO, -1,  23  },
                { L_, 2004,  2, MO,  1,   2  },
                { L_, 2004,  2, MO,  2,   9  },
                { L_, 2004,  2, MO,  3,  16  },
                { L_, 2004,  2, MO,  4,  23  },
                { L_, 2004,  2, MO,  5,   0  },

                // *** testing offset in DEC 2004 (1st is a WEDNESDAY) ***
                //lin year mon dow off  exp
                //--- ---- --- --- ---  ---
                { L_, 2004, 12, TU, -5,   0  }, // weekday before first DOM
                { L_, 2004, 12, TU, -4,   7  },
                { L_, 2004, 12, TU, -3,  14  },
                { L_, 2004, 12, TU, -2,  21  },
                { L_, 2004, 12, TU, -1,  28  },
                { L_, 2004, 12, TU,  1,   7  },
                { L_, 2004, 12, TU,  2,  14  },
                { L_, 2004, 12, TU,  3,  21  },
                { L_, 2004, 12, TU,  4,  28  },
                { L_, 2004, 12, TU,  5,   0  },

                { L_, 2004, 12, WE, -5,   1  }, // weekday first day of month
                { L_, 2004, 12, WE, -4,   8  },
                { L_, 2004, 12, WE, -3,  15  },
                { L_, 2004, 12, WE, -2,  22  },
                { L_, 2004, 12, WE, -1,  29  },
                { L_, 2004, 12, WE,  1,   1  },
                { L_, 2004, 12, WE,  2,   8  },
                { L_, 2004, 12, WE,  3,  15  },
                { L_, 2004, 12, WE,  4,  22  },
                { L_, 2004, 12, WE,  5,  29  },

                { L_, 2004, 12, FR, -5,   3  }, // weekday last day of month
                { L_, 2004, 12, FR, -4,  10  },
                { L_, 2004, 12, FR, -3,  17  },
                { L_, 2004, 12, FR, -2,  24  },
                { L_, 2004, 12, FR, -1,  31  },
                { L_, 2004, 12, FR,  1,   3  },
                { L_, 2004, 12, FR,  2,  10  },
                { L_, 2004, 12, FR,  3,  17  },
                { L_, 2004, 12, FR,  4,  24  },
                { L_, 2004, 12, FR,  5,  31  },

                { L_, 2004, 12, SA, -5,   0  }, // weekday after last dom
                { L_, 2004, 12, SA, -4,   4  },
                { L_, 2004, 12, SA, -3,  11  },
                { L_, 2004, 12, SA, -2,  18  },
                { L_, 2004, 12, SA, -1,  25  },
                { L_, 2004, 12, SA,  1,   4  },
                { L_, 2004, 12, SA,  2,  11  },
                { L_, 2004, 12, SA,  3,  18  },
                { L_, 2004, 12, SA,  4,  25  },
                { L_, 2004, 12, SA,  5,   0  },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int LINE = DATA[ti].d_lineNum;
                const int YEAR = DATA[ti].d_year;
                const int  MON = DATA[ti].d_month;
                const bdet_DayOfWeek::Day
                           DOW = bdet_DayOfWeek::Day(
                                    DATA[ti].d_dayOfWeek);
                const int  OFF = DATA[ti].d_weekdayOffset;
                const int  EXP = DATA[ti].d_expected;

                if (veryVerbose) {
                    cout << "\n--------------------------------------" << endl;
                    P_(LINE) P_(YEAR) P_(MON) P_(DOW) P_(OFF) P(EXP)
                }

                int result = Util::weekdayInMonth(YEAR, MON, DOW, OFF);

                LOOP3_ASSERT(LINE, EXP, result, EXP == result);

            } // end for ti
        } // end block

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'numDaysInRange' FUNCTION
        //   This will test the 'numDaysInRange' function.
        //
        // Concerns:
        //   1. When 'date1' is later than 'date2', the function must return 0,
        //      as stated in the function-level documentation.
        //   2. The following boundary conditions must be tested:
        //       o date1.dayOfWeek() == dayOfWeek.
        //       o date2.dayOfWeek() == dayOfWeek.
        //       o date1.dayOfWeek() == date2.dayOfWeek() != dayOfWeek.
        //       o date1.dayOfWeek() == date2.dayOfWeek() == dayOfWeek.
        //   3. The number of weeks between 'date1' and 'date2' is included in
        //      the returned result.
        //
        //   Profound fact: Calendar irregularities are already compensated for
        //                  by 'bdet_Date' and are therefore not a concern in
        //                  this test.
        //
        // Plan:
        //   For a set of test data, exercise the 'numDaysInRange' function and
        //   check that the returned value is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //   dayOfWeek     isLater      dowEqual     dateSameDow    numWeeks
        //   ---------     -------      --------     -----------    --------
        //
        //    | SUN |
        //    | MON |                                                 | 0 |
        //    | TUE |     | true  |     | true  |     | true  |       | 1 |
        //    | WED |  X  |       |  X  |       |  X  |       |   X   |   |
        //    | THU |     | false |     | false |     | false |       | 2 |
        //    | FRI |                                                 | 3 |
        //    | SAT |
        //
        //   where:
        //       o isLater     : date1 > date2.
        //       o dowEqual    : dayOfWeek == date1.dayOfWeek().
        //       o dateSameDow : date1.dayOfWeek() == date2.dayOfWeek().
        //       o numWeeks    : number of weeks between date1 and
        //                       floorDay(date1.dayOfWeek(), date2).
        //
        //   Note that the values of 'date1' and 'date2' can be any arbitrary
        //   dates that conform to the scenario being tested.
        //
        //   Also note that if 'isLater' is true, then 'numWeeks' must be zero.
        //
        // Testing:
        //   int numDaysInRange(b_DOW dayOfWeek, c b_D& date1, c b_D& date2);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'numDaysInRange' Function"
                          << "\n=================================" << endl;

        static const struct {
            int                  d_lineNum;     // source line number
            bdet_DayOfWeek::Day  d_dayOfWeek;   // dayOfWeek
            const char          *d_date1;       // date1
            const char          *d_date2;       // date2
            int                  d_result;      // expected result
        } DATA[] = {
            //    January 2000
            //  S  M Tu  W Th  F  S
            //                    1
            //  2  3  4  5  6  7  8
            //  9 10 11 12 13 14 15
            // 16 17 18 19 20 21 22
            // 23 24 25 26 27 28 29
            // 30 31
            //
            //line    dayOfWeek    date1            date2            result
            //----    ---------    -----            -----            ------

            // isLater=true, dowEqual=true, dateSameDow=true, numWeeks=0
            { L_,     SUN,         "2000,01,09",    "2000,01,02",    0     },
            { L_,     MON,         "2000,01,10",    "2000,01,03",    0     },
            { L_,     TUE,         "2000,01,11",    "2000,01,04",    0     },
            { L_,     WED,         "2000,01,12",    "2000,01,05",    0     },
            { L_,     THU,         "2000,01,13",    "2000,01,06",    0     },
            { L_,     FRI,         "2000,01,14",    "2000,01,07",    0     },
            { L_,     SAT,         "2000,01,15",    "2000,01,08",    0     },

            // isLater=true, dowEqual=false, dateSameDow=true, numWeeks=0
            { L_,     SUN,         "2000,01,15",    "2000,01,08",    0     },
            { L_,     MON,         "2000,01,15",    "2000,01,08",    0     },
            { L_,     TUE,         "2000,01,15",    "2000,01,08",    0     },
            { L_,     WED,         "2000,01,15",    "2000,01,08",    0     },
            { L_,     THU,         "2000,01,15",    "2000,01,08",    0     },
            { L_,     FRI,         "2000,01,15",    "2000,01,08",    0     },
            { L_,     SAT,         "2000,01,09",    "2000,01,02",    0     },

            // isLater=true, dowEqual=true, dateSameDow=false, numWeeks=0
            { L_,     SUN,         "2000,01,09",    "2000,01,03",    0     },
            { L_,     MON,         "2000,01,10",    "2000,01,02",    0     },
            { L_,     TUE,         "2000,01,11",    "2000,01,02",    0     },
            { L_,     WED,         "2000,01,12",    "2000,01,02",    0     },
            { L_,     THU,         "2000,01,13",    "2000,01,02",    0     },
            { L_,     FRI,         "2000,01,14",    "2000,01,02",    0     },
            { L_,     SAT,         "2000,01,15",    "2000,01,02",    0     },

            // isLater=true, dowEqual=false, dateSameDow=false, numWeeks=0
            { L_,     SUN,         "2000,01,15",    "2000,01,07",    0     },
            { L_,     MON,         "2000,01,15",    "2000,01,07",    0     },
            { L_,     TUE,         "2000,01,15",    "2000,01,07",    0     },
            { L_,     WED,         "2000,01,15",    "2000,01,07",    0     },
            { L_,     THU,         "2000,01,15",    "2000,01,07",    0     },
            { L_,     FRI,         "2000,01,15",    "2000,01,07",    0     },
            { L_,     SAT,         "2000,01,09",    "2000,01,08",    0     },

            // isLater=false, dowEqual=true, dateSameDow=true, numWeeks=0
            { L_,     SUN,         "2000,01,02",    "2000,01,02",    1     },
            { L_,     MON,         "2000,01,03",    "2000,01,03",    1     },
            { L_,     TUE,         "2000,01,04",    "2000,01,04",    1     },
            { L_,     WED,         "2000,01,05",    "2000,01,05",    1     },
            { L_,     THU,         "2000,01,06",    "2000,01,06",    1     },
            { L_,     FRI,         "2000,01,07",    "2000,01,07",    1     },
            { L_,     SAT,         "2000,01,08",    "2000,01,08",    1     },

            // isLater=false, dowEqual=false, dateSameDow=true, numWeeks=0
            { L_,     SUN,         "2000,01,03",    "2000,01,03",    0     },
            { L_,     MON,         "2000,01,02",    "2000,01,02",    0     },
            { L_,     TUE,         "2000,01,02",    "2000,01,02",    0     },
            { L_,     WED,         "2000,01,02",    "2000,01,02",    0     },
            { L_,     THU,         "2000,01,02",    "2000,01,02",    0     },
            { L_,     FRI,         "2000,01,02",    "2000,01,02",    0     },
            { L_,     SAT,         "2000,01,02",    "2000,01,02",    0     },

            // isLater=false, dowEqual=true, dateSameDow=false, numWeeks=0
            { L_,     SUN,         "2000,01,02",    "2000,01,08",    1     },
            { L_,     MON,         "2000,01,03",    "2000,01,08",    1     },
            { L_,     TUE,         "2000,01,04",    "2000,01,08",    1     },
            { L_,     WED,         "2000,01,05",    "2000,01,08",    1     },
            { L_,     THU,         "2000,01,06",    "2000,01,08",    1     },
            { L_,     FRI,         "2000,01,07",    "2000,01,08",    1     },
            { L_,     SAT,         "2000,01,08",    "2000,01,09",    1     },

            // isLater=false, dowEqual=false, dateSameDow=false, numWeeks=0
            { L_,     SUN,         "2000,01,03",    "2000,01,04",    0     },
            { L_,     MON,         "2000,01,02",    "2000,01,03",    1     },
            { L_,     TUE,         "2000,01,02",    "2000,01,03",    0     },
            { L_,     WED,         "2000,01,02",    "2000,01,03",    0     },
            { L_,     THU,         "2000,01,02",    "2000,01,03",    0     },
            { L_,     FRI,         "2000,01,02",    "2000,01,03",    0     },
            { L_,     SAT,         "2000,01,02",    "2000,01,03",    0     },

            // isLater=false, dowEqual=true, dateSameDow=true, numWeeks=1
            { L_,     SUN,         "2000,01,02",    "2000,01,09",    2     },
            { L_,     MON,         "2000,01,03",    "2000,01,10",    2     },
            { L_,     TUE,         "2000,01,04",    "2000,01,11",    2     },
            { L_,     WED,         "2000,01,05",    "2000,01,12",    2     },
            { L_,     THU,         "2000,01,06",    "2000,01,13",    2     },
            { L_,     FRI,         "2000,01,07",    "2000,01,14",    2     },
            { L_,     SAT,         "2000,01,08",    "2000,01,15",    2     },

            // isLater=false, dowEqual=false, dateSameDow=true, numWeeks=1
            { L_,     SUN,         "2000,01,03",    "2000,01,10",    1     },
            { L_,     MON,         "2000,01,02",    "2000,01,09",    1     },
            { L_,     TUE,         "2000,01,02",    "2000,01,09",    1     },
            { L_,     WED,         "2000,01,02",    "2000,01,09",    1     },
            { L_,     THU,         "2000,01,02",    "2000,01,09",    1     },
            { L_,     FRI,         "2000,01,02",    "2000,01,09",    1     },
            { L_,     SAT,         "2000,01,02",    "2000,01,09",    1     },

            // isLater=false, dowEqual=true, dateSameDow=false, numWeeks=1
            { L_,     SUN,         "2000,01,02",    "2000,01,15",    2     },
            { L_,     MON,         "2000,01,03",    "2000,01,15",    2     },
            { L_,     TUE,         "2000,01,04",    "2000,01,15",    2     },
            { L_,     WED,         "2000,01,05",    "2000,01,15",    2     },
            { L_,     THU,         "2000,01,06",    "2000,01,15",    2     },
            { L_,     FRI,         "2000,01,07",    "2000,01,15",    2     },
            { L_,     SAT,         "2000,01,08",    "2000,01,16",    2     },

            // isLater=false, dowEqual=false, dateSameDow=false, numWeeks=1
            { L_,     SUN,         "2000,01,03",    "2000,01,11",    1     },
            { L_,     MON,         "2000,01,02",    "2000,01,10",    2     },
            { L_,     TUE,         "2000,01,02",    "2000,01,10",    1     },
            { L_,     WED,         "2000,01,02",    "2000,01,10",    1     },
            { L_,     THU,         "2000,01,02",    "2000,01,10",    1     },
            { L_,     FRI,         "2000,01,02",    "2000,01,10",    1     },
            { L_,     SAT,         "2000,01,02",    "2000,01,10",    1     },

            // isLater=false, dowEqual=true, dateSameDow=true, numWeeks=2
            { L_,     SUN,         "2000,01,02",    "2000,01,16",    3     },
            { L_,     MON,         "2000,01,03",    "2000,01,17",    3     },
            { L_,     TUE,         "2000,01,04",    "2000,01,18",    3     },
            { L_,     WED,         "2000,01,05",    "2000,01,19",    3     },
            { L_,     THU,         "2000,01,06",    "2000,01,20",    3     },
            { L_,     FRI,         "2000,01,07",    "2000,01,21",    3     },
            { L_,     SAT,         "2000,01,08",    "2000,01,22",    3     },

            // isLater=false, dowEqual=false, dateSameDow=true, numWeeks=2
            { L_,     SUN,         "2000,01,03",    "2000,01,17",    2     },
            { L_,     MON,         "2000,01,02",    "2000,01,16",    2     },
            { L_,     TUE,         "2000,01,02",    "2000,01,16",    2     },
            { L_,     WED,         "2000,01,02",    "2000,01,16",    2     },
            { L_,     THU,         "2000,01,02",    "2000,01,16",    2     },
            { L_,     FRI,         "2000,01,02",    "2000,01,16",    2     },
            { L_,     SAT,         "2000,01,02",    "2000,01,16",    2     },

            // isLater=false, dowEqual=true, dateSameDow=false, numWeeks=2
            { L_,     SUN,         "2000,01,02",    "2000,01,22",    3     },
            { L_,     MON,         "2000,01,03",    "2000,01,22",    3     },
            { L_,     TUE,         "2000,01,04",    "2000,01,22",    3     },
            { L_,     WED,         "2000,01,05",    "2000,01,22",    3     },
            { L_,     THU,         "2000,01,06",    "2000,01,22",    3     },
            { L_,     FRI,         "2000,01,07",    "2000,01,22",    3     },
            { L_,     SAT,         "2000,01,08",    "2000,01,23",    3     },

            // isLater=false, dowEqual=false, dateSameDow=false, numWeeks=2
            { L_,     SUN,         "2000,01,03",    "2000,01,18",    2     },
            { L_,     MON,         "2000,01,02",    "2000,01,17",    3     },
            { L_,     TUE,         "2000,01,02",    "2000,01,17",    2     },
            { L_,     WED,         "2000,01,02",    "2000,01,17",    2     },
            { L_,     THU,         "2000,01,02",    "2000,01,17",    2     },
            { L_,     FRI,         "2000,01,02",    "2000,01,17",    2     },
            { L_,     SAT,         "2000,01,02",    "2000,01,17",    2     },

            // isLater=false, dowEqual=true, dateSameDow=true, numWeeks=3
            { L_,     SUN,         "2000,01,02",    "2000,01,23",    4     },
            { L_,     MON,         "2000,01,03",    "2000,01,24",    4     },
            { L_,     TUE,         "2000,01,04",    "2000,01,25",    4     },
            { L_,     WED,         "2000,01,05",    "2000,01,26",    4     },
            { L_,     THU,         "2000,01,06",    "2000,01,27",    4     },
            { L_,     FRI,         "2000,01,07",    "2000,01,28",    4     },
            { L_,     SAT,         "2000,01,08",    "2000,01,29",    4     },

            // isLater=false, dowEqual=false, dateSameDow=true, numWeeks=3
            { L_,     SUN,         "2000,01,03",    "2000,01,24",    3     },
            { L_,     MON,         "2000,01,02",    "2000,01,23",    3     },
            { L_,     TUE,         "2000,01,02",    "2000,01,23",    3     },
            { L_,     WED,         "2000,01,02",    "2000,01,23",    3     },
            { L_,     THU,         "2000,01,02",    "2000,01,23",    3     },
            { L_,     FRI,         "2000,01,02",    "2000,01,23",    3     },
            { L_,     SAT,         "2000,01,02",    "2000,01,23",    3     },

            // isLater=false, dowEqual=true, dateSameDow=false, numWeeks=3
            { L_,     SUN,         "2000,01,02",    "2000,01,29",    4     },
            { L_,     MON,         "2000,01,03",    "2000,01,29",    4     },
            { L_,     TUE,         "2000,01,04",    "2000,01,29",    4     },
            { L_,     WED,         "2000,01,05",    "2000,01,29",    4     },
            { L_,     THU,         "2000,01,06",    "2000,01,29",    4     },
            { L_,     FRI,         "2000,01,07",    "2000,01,29",    4     },
            { L_,     SAT,         "2000,01,08",    "2000,01,30",    4     },

            // isLater=false, dowEqual=false, dateSameDow=false, numWeeks=3
            { L_,     SUN,         "2000,01,03",    "2000,01,25",    3     },
            { L_,     MON,         "2000,01,02",    "2000,01,24",    4     },
            { L_,     TUE,         "2000,01,02",    "2000,01,24",    3     },
            { L_,     WED,         "2000,01,02",    "2000,01,24",    3     },
            { L_,     THU,         "2000,01,02",    "2000,01,24",    3     },
            { L_,     FRI,         "2000,01,02",    "2000,01,24",    3     },
            { L_,     SAT,         "2000,01,02",    "2000,01,24",    3     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                 LINE        = DATA[i].d_lineNum;
            const bdet_DayOfWeek::Day DAY_OF_WEEK = DATA[i].d_dayOfWeek;
            const bdet_Date           DATE1       = parseDate(DATA[i].d_date1);
            const bdet_Date           DATE2       = parseDate(DATA[i].d_date2);
            const int                 RESULT      = DATA[i].d_result;

            if (veryVerbose) {
                T_
                P_(LINE) P_(DAY_OF_WEEK) P_(DATE1) P_(DATE2) P(RESULT);
            }

            int result = Util::numDaysInRange(DAY_OF_WEEK, DATE1, DATE2);
            LOOP2_ASSERT(LINE, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'numDaysInRange' Function Test."
                          << endl;
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'floorOrNextDayInMonth' FUNCTION
        //   This will test the 'floorOrNextDayInMonth' function.
        //
        // Concerns:
        //   1. If 'floorDay' falls in the same month as 'date', then
        //      'floorDay' must be returned.
        //   2. If 'floorDay' falls on the following month, then 'nextDay' must
        //      be returned.
        //
        //   Profound fact: Calendar irregularities are already compensated for
        //                  by 'bdet_Date' and are therefore not a concern in
        //                  this test.
        //
        // Plan:
        //   For a set of test data, exercise the 'floorOrNextDayInMonth'
        //   function and check that the returned value is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //      date.dayOfWeek       dayOfWeek     floorDay.month == date.month
        //      --------------       ---------     ----------------------------
        //
        //          | SUN |           | SUN |
        //          | MON |           | MON |
        //          | TUE |           | TUE |            | true  |
        //          | WED |     X     | WED |      X     |       |
        //          | THU |           | THU |            | false |
        //          | FRI |           | FRI |
        //          | SAT |           | SAT |
        //
        //   Note that the value of 'date' can be any arbitrary date that
        //   conforms to the scenario being tested.
        //
        //   Also note that if date.dayOfWeek == dayOfWeek, then
        //   floorDay.month == date.month cannot be false.
        //
        // Testing:
        //   b_D floorOrNextDayInMonth(b_DOW dayOfWeek, c b_D& date);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'floorOrNextDayInMonth' Function"
                          << "\n========================================"
                          << endl;

        static const struct {
            int                  d_lineNum;      // source line number
            bdet_DayOfWeek::Day  d_dayOfWeek;    // dayOfWeek
            const char          *d_date;         // date
            const char          *d_result;       // expected result
        } DATA[] = {
            //line    dayOfWeek    date             result   // floorDay.month
            //----    ---------    ----             ------   //   == date.month
                                                             // ---------------

            //    October 2000          - begins on SUN
            //  S  M Tu  W Th  F  S
            //  1  2  3  4  5  6  7
            //  8  9 10 11 12 13 14
            // 15 16 17 18 19 20 21
            // 22 23 24 25 26 27 28
            // 29 30 31
            //
            // date.dayOfWeek() == SUN
            { L_,     SUN,         "2000,10,08",    "2000,10,08" },    // true
            { L_,     MON,         "2000,10,08",    "2000,10,02" },    // true
            { L_,     TUE,         "2000,10,08",    "2000,10,03" },    // true
            { L_,     WED,         "2000,10,08",    "2000,10,04" },    // true
            { L_,     THU,         "2000,10,08",    "2000,10,05" },    // true
            { L_,     FRI,         "2000,10,08",    "2000,10,06" },    // true
            { L_,     SAT,         "2000,10,08",    "2000,10,07" },    // true

            { L_,     SUN,         "2000,10,01",    "2000,10,01" },    // true
            { L_,     MON,         "2000,10,01",    "2000,10,02" },    // false
            { L_,     TUE,         "2000,10,01",    "2000,10,03" },    // false
            { L_,     WED,         "2000,10,01",    "2000,10,04" },    // false
            { L_,     THU,         "2000,10,01",    "2000,10,05" },    // false
            { L_,     FRI,         "2000,10,01",    "2000,10,06" },    // false
            { L_,     SAT,         "2000,10,01",    "2000,10,07" },    // false

            //    May 2000              - begins on MON
            //  S  M Tu  W Th  F  S
            //     1  2  3  4  5  6
            //  7  8  9 10 11 12 13
            // 14 15 16 17 18 19 20
            // 21 22 23 24 25 26 27
            // 28 29 30 31
            //
            // date.dayOfWeek() == MON
            { L_,     SUN,         "2000,05,08",    "2000,05,07" },    // true
            { L_,     MON,         "2000,05,08",    "2000,05,08" },    // true
            { L_,     TUE,         "2000,05,08",    "2000,05,02" },    // true
            { L_,     WED,         "2000,05,08",    "2000,05,03" },    // true
            { L_,     THU,         "2000,05,08",    "2000,05,04" },    // true
            { L_,     FRI,         "2000,05,08",    "2000,05,05" },    // true
            { L_,     SAT,         "2000,05,08",    "2000,05,06" },    // true

            { L_,     SUN,         "2000,05,01",    "2000,05,07" },    // false
            { L_,     MON,         "2000,05,01",    "2000,05,01" },    // true
            { L_,     TUE,         "2000,05,01",    "2000,05,02" },    // false
            { L_,     WED,         "2000,05,01",    "2000,05,03" },    // false
            { L_,     THU,         "2000,05,01",    "2000,05,04" },    // false
            { L_,     FRI,         "2000,05,01",    "2000,05,05" },    // false
            { L_,     SAT,         "2000,05,01",    "2000,05,06" },    // false

            //    February 2000         - begins on TUE
            //  S  M Tu  W Th  F  S
            //        1  2  3  4  5
            //  6  7  8  9 10 11 12
            // 13 14 15 16 17 18 19
            // 20 21 22 23 24 25 26
            // 27 28 29
            //
            // date.dayOfWeek() == TUE
            { L_,     SUN,         "2000,02,08",    "2000,02,06" },    // true
            { L_,     MON,         "2000,02,08",    "2000,02,07" },    // true
            { L_,     TUE,         "2000,02,08",    "2000,02,08" },    // true
            { L_,     WED,         "2000,02,08",    "2000,02,02" },    // true
            { L_,     THU,         "2000,02,08",    "2000,02,03" },    // true
            { L_,     FRI,         "2000,02,08",    "2000,02,04" },    // true
            { L_,     SAT,         "2000,02,08",    "2000,02,05" },    // true

            { L_,     SUN,         "2000,02,01",    "2000,02,06" },    // false
            { L_,     MON,         "2000,02,01",    "2000,02,07" },    // false
            { L_,     TUE,         "2000,02,01",    "2000,02,01" },    // true
            { L_,     WED,         "2000,02,01",    "2000,02,02" },    // false
            { L_,     THU,         "2000,02,01",    "2000,02,03" },    // false
            { L_,     FRI,         "2000,02,01",    "2000,02,04" },    // false
            { L_,     SAT,         "2000,02,01",    "2000,02,05" },    // false

            //    March 2000            - begins on WED
            //  S  M Tu  W Th  F  S
            //           1  2  3  4
            //  5  6  7  8  9 10 11
            // 12 13 14 15 16 17 18
            // 19 20 21 22 23 24 25
            // 26 27 28 29 30 31
            //
            // date.dayOfWeek() == WED
            { L_,     SUN,         "2000,03,08",    "2000,03,05" },    // true
            { L_,     MON,         "2000,03,08",    "2000,03,06" },    // true
            { L_,     TUE,         "2000,03,08",    "2000,03,07" },    // true
            { L_,     WED,         "2000,03,08",    "2000,03,08" },    // true
            { L_,     THU,         "2000,03,08",    "2000,03,02" },    // true
            { L_,     FRI,         "2000,03,08",    "2000,03,03" },    // true
            { L_,     SAT,         "2000,03,08",    "2000,03,04" },    // true

            { L_,     SUN,         "2000,03,01",    "2000,03,05" },    // false
            { L_,     MON,         "2000,03,01",    "2000,03,06" },    // false
            { L_,     TUE,         "2000,03,01",    "2000,03,07" },    // false
            { L_,     WED,         "2000,03,01",    "2000,03,01" },    // true
            { L_,     THU,         "2000,03,01",    "2000,03,02" },    // false
            { L_,     FRI,         "2000,03,01",    "2000,03,03" },    // false
            { L_,     SAT,         "2000,03,01",    "2000,03,04" },    // false

            //    June 2000             - begins on THU
            //  S  M Tu  W Th  F  S
            //              1  2  3
            //  4  5  6  7  8  9 10
            // 11 12 13 14 15 16 17
            // 18 19 20 21 22 23 24
            // 25 26 27 28 29 30
            //
            // date.dayOfWeek() == THU
            { L_,     SUN,         "2000,06,08",    "2000,06,04" },    // true
            { L_,     MON,         "2000,06,08",    "2000,06,05" },    // true
            { L_,     TUE,         "2000,06,08",    "2000,06,06" },    // true
            { L_,     WED,         "2000,06,08",    "2000,06,07" },    // true
            { L_,     THU,         "2000,06,08",    "2000,06,08" },    // true
            { L_,     FRI,         "2000,06,08",    "2000,06,02" },    // true
            { L_,     SAT,         "2000,06,08",    "2000,06,03" },    // true

            { L_,     SUN,         "2000,06,01",    "2000,06,04" },    // false
            { L_,     MON,         "2000,06,01",    "2000,06,05" },    // false
            { L_,     TUE,         "2000,06,01",    "2000,06,06" },    // false
            { L_,     WED,         "2000,06,01",    "2000,06,07" },    // false
            { L_,     THU,         "2000,06,01",    "2000,06,01" },    // true
            { L_,     FRI,         "2000,06,01",    "2000,06,02" },    // false
            { L_,     SAT,         "2000,06,01",    "2000,06,03" },    // false

            //    September 2000        - begins on FRI
            //  S  M Tu  W Th  F  S
            //                 1  2
            //  3  4  5  6  7  8  9
            // 10 11 12 13 14 15 16
            // 17 18 19 20 21 22 23
            // 24 25 26 27 28 29 30
            //
            // date.dayOfWeek() == FRI
            { L_,     SUN,         "2000,09,08",    "2000,09,03" },    // true
            { L_,     MON,         "2000,09,08",    "2000,09,04" },    // true
            { L_,     TUE,         "2000,09,08",    "2000,09,05" },    // true
            { L_,     WED,         "2000,09,08",    "2000,09,06" },    // true
            { L_,     THU,         "2000,09,08",    "2000,09,07" },    // true
            { L_,     FRI,         "2000,09,08",    "2000,09,08" },    // true
            { L_,     SAT,         "2000,09,08",    "2000,09,02" },    // true

            { L_,     SUN,         "2000,09,01",    "2000,09,03" },    // false
            { L_,     MON,         "2000,09,01",    "2000,09,04" },    // false
            { L_,     TUE,         "2000,09,01",    "2000,09,05" },    // false
            { L_,     WED,         "2000,09,01",    "2000,09,06" },    // false
            { L_,     THU,         "2000,09,01",    "2000,09,07" },    // false
            { L_,     FRI,         "2000,09,01",    "2000,09,01" },    // true
            { L_,     SAT,         "2000,09,01",    "2000,09,02" },    // false

            //    January 2000          - begins on SAT
            //  S  M Tu  W Th  F  S
            //                    1
            //  2  3  4  5  6  7  8
            //  9 10 11 12 13 14 15
            // 16 17 18 19 20 21 22
            // 23 24 25 26 27 28 29
            // 30 31
            //
            // date.dayOfWeek() == SAT
            { L_,     SUN,         "2000,01,08",    "2000,01,02" },    // true
            { L_,     MON,         "2000,01,08",    "2000,01,03" },    // true
            { L_,     TUE,         "2000,01,08",    "2000,01,04" },    // true
            { L_,     WED,         "2000,01,08",    "2000,01,05" },    // true
            { L_,     THU,         "2000,01,08",    "2000,01,06" },    // true
            { L_,     FRI,         "2000,01,08",    "2000,01,07" },    // true
            { L_,     SAT,         "2000,01,08",    "2000,01,08" },    // true

            { L_,     SUN,         "2000,01,01",    "2000,01,02" },    // false
            { L_,     MON,         "2000,01,01",    "2000,01,03" },    // false
            { L_,     TUE,         "2000,01,01",    "2000,01,04" },    // false
            { L_,     WED,         "2000,01,01",    "2000,01,05" },    // false
            { L_,     THU,         "2000,01,01",    "2000,01,06" },    // false
            { L_,     FRI,         "2000,01,01",    "2000,01,07" },    // false
            { L_,     SAT,         "2000,01,01",    "2000,01,01" },    // true
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                 LINE        = DATA[i].d_lineNum;
            const bdet_DayOfWeek::Day DAY_OF_WEEK = DATA[i].d_dayOfWeek;
            const bdet_Date           DATE        = parseDate(DATA[i].d_date);
            const bdet_Date           RESULT      = parseDate(
                                                             DATA[i].d_result);

            if (veryVerbose) {
                T_
                P_(LINE) P_(DAY_OF_WEEK) P_(DATE) P(RESULT);
            }

            // Check 'RESULT' using oracle.
            {
                const bdet_Date FLOOR_DAY = Util::floorDay(DAY_OF_WEEK, DATE);

                if (FLOOR_DAY.month() == DATE.month()) {
                    LOOP3_ASSERT(LINE, RESULT, FLOOR_DAY, RESULT == FLOOR_DAY);
                }
                else {
                    const bdet_Date NEXT_DAY = Util::nextDay(DAY_OF_WEEK,
                                                             DATE);
                    LOOP3_ASSERT(LINE, RESULT, NEXT_DAY, RESULT == NEXT_DAY);
                }
            }

            bdet_Date result = Util::floorOrNextDayInMonth(DAY_OF_WEEK, DATE);
            LOOP2_ASSERT(LINE, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'floorOrNextDayInMonth' Function "
                          << "Test." << endl;
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'ceilOrPreviousDayInMonth' FUNCTION
        //   This will test the 'ceilOrPreviousDayInMonth' function.
        //
        // Concerns:
        //   1. If 'ceilDay' falls in the same month as 'date', then 'ceilDay'
        //      must be returned.
        //   2. If 'ceilDay' falls on the following month, then 'previousDay'
        //      must be returned.
        //
        //   Profound fact: Calendar irregularities are already compensated for
        //                  by 'bdet_Date' and are therefore not a concern in
        //                  this test.
        //
        // Plan:
        //   For a set of test data, exercise the 'ceilOrPreviousDayInMonth'
        //   function and check that the returned value is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //      date.dayOfWeek       dayOfWeek      ceilDay.month == date.month
        //      --------------       ---------      ---------------------------
        //
        //          | SUN |           | SUN |
        //          | MON |           | MON |
        //          | TUE |           | TUE |            | true  |
        //          | WED |     X     | WED |      X     |       |
        //          | THU |           | THU |            | false |
        //          | FRI |           | FRI |
        //          | SAT |           | SAT |
        //
        //   Note that the value of 'date' can be any arbitrary date that
        //   conforms to the scenario being tested.
        //
        //   Also note that if date.dayOfWeek == dayOfWeek, then
        //   ceilDay.month == date.month cannot be false.
        //
        // Testing:
        //   b_D ceilOrPreviousDayInMonth(b_DOW dayOfWeek, c b_D& date);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'ceilOrPreviousDayInMonth' Function"
                          << "\n==========================================="
                          << endl;

        static const struct {
            int                  d_lineNum;      // source line number
            bdet_DayOfWeek::Day  d_dayOfWeek;    // dayOfWeek
            const char          *d_date;         // date
            const char          *d_result;       // expected result
        } DATA[] = {
            //line    dayOfWeek    date             result   // ceilDay.month
            //----    ---------    ----             ------   //   == date.month
                                                             // ---------------

            //    April 2000            - ends on SUN
            //  S  M Tu  W Th  F  S
            //                    1
            //  2  3  4  5  6  7  8
            //  9 10 11 12 13 14 15
            // 16 17 18 19 20 21 22
            // 23 24 25 26 27 28 29
            // 30
            //
            // date.dayOfWeek() == SUN
            { L_,     SUN,         "2000,04,23",    "2000,04,23" },    // true
            { L_,     MON,         "2000,04,23",    "2000,04,24" },    // true
            { L_,     TUE,         "2000,04,23",    "2000,04,25" },    // true
            { L_,     WED,         "2000,04,23",    "2000,04,26" },    // true
            { L_,     THU,         "2000,04,23",    "2000,04,27" },    // true
            { L_,     FRI,         "2000,04,23",    "2000,04,28" },    // true
            { L_,     SAT,         "2000,04,23",    "2000,04,29" },    // true

            { L_,     SUN,         "2000,04,30",    "2000,04,30" },    // true
            { L_,     MON,         "2000,04,30",    "2000,04,24" },    // false
            { L_,     TUE,         "2000,04,30",    "2000,04,25" },    // false
            { L_,     WED,         "2000,04,30",    "2000,04,26" },    // false
            { L_,     THU,         "2000,04,30",    "2000,04,27" },    // false
            { L_,     FRI,         "2000,04,30",    "2000,04,28" },    // false
            { L_,     SAT,         "2000,04,30",    "2000,04,29" },    // false

            //    January 2000          - ends on MON
            //  S  M Tu  W Th  F  S
            //                    1
            //  2  3  4  5  6  7  8
            //  9 10 11 12 13 14 15
            // 16 17 18 19 20 21 22
            // 23 24 25 26 27 28 29
            // 30 31
            //
            // date.dayOfWeek() == MON
            { L_,     SUN,         "2000,01,24",    "2000,01,30" },    // true
            { L_,     MON,         "2000,01,24",    "2000,01,24" },    // true
            { L_,     TUE,         "2000,01,24",    "2000,01,25" },    // true
            { L_,     WED,         "2000,01,24",    "2000,01,26" },    // true
            { L_,     THU,         "2000,01,24",    "2000,01,27" },    // true
            { L_,     FRI,         "2000,01,24",    "2000,01,28" },    // true
            { L_,     SAT,         "2000,01,24",    "2000,01,29" },    // true

            { L_,     SUN,         "2000,01,31",    "2000,01,30" },    // false
            { L_,     MON,         "2000,01,31",    "2000,01,31" },    // true
            { L_,     TUE,         "2000,01,31",    "2000,01,25" },    // false
            { L_,     WED,         "2000,01,31",    "2000,01,26" },    // false
            { L_,     THU,         "2000,01,31",    "2000,01,27" },    // false
            { L_,     FRI,         "2000,01,31",    "2000,01,28" },    // false
            { L_,     SAT,         "2000,01,31",    "2000,01,29" },    // false

            //    February 2000         - ends on TUE
            //  S  M Tu  W Th  F  S
            //        1  2  3  4  5
            //  6  7  8  9 10 11 12
            // 13 14 15 16 17 18 19
            // 20 21 22 23 24 25 26
            // 27 28 29
            //
            // date.dayOfWeek() == TUE
            { L_,     SUN,         "2000,02,22",    "2000,02,27" },    // true
            { L_,     MON,         "2000,02,22",    "2000,02,28" },    // true
            { L_,     TUE,         "2000,02,22",    "2000,02,22" },    // true
            { L_,     WED,         "2000,02,22",    "2000,02,23" },    // true
            { L_,     THU,         "2000,02,22",    "2000,02,24" },    // true
            { L_,     FRI,         "2000,02,22",    "2000,02,25" },    // true
            { L_,     SAT,         "2000,02,22",    "2000,02,26" },    // true

            { L_,     SUN,         "2000,02,29",    "2000,02,27" },    // false
            { L_,     MON,         "2000,02,29",    "2000,02,28" },    // false
            { L_,     TUE,         "2000,02,29",    "2000,02,29" },    // true
            { L_,     WED,         "2000,02,29",    "2000,02,23" },    // false
            { L_,     THU,         "2000,02,29",    "2000,02,24" },    // false
            { L_,     FRI,         "2000,02,29",    "2000,02,25" },    // false
            { L_,     SAT,         "2000,02,29",    "2000,02,26" },    // false

            //    May 2000              - ends on WED
            //  S  M Tu  W Th  F  S
            //     1  2  3  4  5  6
            //  7  8  9 10 11 12 13
            // 14 15 16 17 18 19 20
            // 21 22 23 24 25 26 27
            // 28 29 30 31
            //
            // date.dayOfWeek() == WED
            { L_,     SUN,         "2000,05,24",    "2000,05,28" },    // true
            { L_,     MON,         "2000,05,24",    "2000,05,29" },    // true
            { L_,     TUE,         "2000,05,24",    "2000,05,30" },    // true
            { L_,     WED,         "2000,05,24",    "2000,05,24" },    // true
            { L_,     THU,         "2000,05,24",    "2000,05,25" },    // true
            { L_,     FRI,         "2000,05,24",    "2000,05,26" },    // true
            { L_,     SAT,         "2000,05,24",    "2000,05,27" },    // true

            { L_,     SUN,         "2000,05,31",    "2000,05,28" },    // false
            { L_,     MON,         "2000,05,31",    "2000,05,29" },    // false
            { L_,     TUE,         "2000,05,31",    "2000,05,30" },    // false
            { L_,     WED,         "2000,05,31",    "2000,05,31" },    // true
            { L_,     THU,         "2000,05,31",    "2000,05,25" },    // false
            { L_,     FRI,         "2000,05,31",    "2000,05,26" },    // false
            { L_,     SAT,         "2000,05,31",    "2000,05,27" },    // false

            //    August 2000           - ends on THU
            //  S  M Tu  W Th  F  S
            //        1  2  3  4  5
            //  6  7  8  9 10 11 12
            // 13 14 15 16 17 18 19
            // 20 21 22 23 24 25 26
            // 27 28 29 30 31
            //
            // date.dayOfWeek() == THU
            { L_,     SUN,         "2000,08,24",    "2000,08,27" },    // true
            { L_,     MON,         "2000,08,24",    "2000,08,28" },    // true
            { L_,     TUE,         "2000,08,24",    "2000,08,29" },    // true
            { L_,     WED,         "2000,08,24",    "2000,08,30" },    // true
            { L_,     THU,         "2000,08,24",    "2000,08,24" },    // true
            { L_,     FRI,         "2000,08,24",    "2000,08,25" },    // true
            { L_,     SAT,         "2000,08,24",    "2000,08,26" },    // true

            { L_,     SUN,         "2000,08,31",    "2000,08,27" },    // false
            { L_,     MON,         "2000,08,31",    "2000,08,28" },    // false
            { L_,     TUE,         "2000,08,31",    "2000,08,29" },    // false
            { L_,     WED,         "2000,08,31",    "2000,08,30" },    // false
            { L_,     THU,         "2000,08,31",    "2000,08,31" },    // true
            { L_,     FRI,         "2000,08,31",    "2000,08,25" },    // false
            { L_,     SAT,         "2000,08,31",    "2000,08,26" },    // false

            //    June 2000             - ends on FRI
            //  S  M Tu  W Th  F  S
            //              1  2  3
            //  4  5  6  7  8  9 10
            // 11 12 13 14 15 16 17
            // 18 19 20 21 22 23 24
            // 25 26 27 28 29 30
            //
            // date.dayOfWeek() == FRI
            { L_,     SUN,         "2000,06,23",    "2000,06,25" },    // true
            { L_,     MON,         "2000,06,23",    "2000,06,26" },    // true
            { L_,     TUE,         "2000,06,23",    "2000,06,27" },    // true
            { L_,     WED,         "2000,06,23",    "2000,06,28" },    // true
            { L_,     THU,         "2000,06,23",    "2000,06,29" },    // true
            { L_,     FRI,         "2000,06,23",    "2000,06,23" },    // true
            { L_,     SAT,         "2000,06,23",    "2000,06,24" },    // true

            { L_,     SUN,         "2000,06,30",    "2000,06,25" },    // false
            { L_,     MON,         "2000,06,30",    "2000,06,26" },    // false
            { L_,     TUE,         "2000,06,30",    "2000,06,27" },    // false
            { L_,     WED,         "2000,06,30",    "2000,06,28" },    // false
            { L_,     THU,         "2000,06,30",    "2000,06,29" },    // false
            { L_,     FRI,         "2000,06,30",    "2000,06,30" },    // true
            { L_,     SAT,         "2000,06,30",    "2000,06,24" },    // false

            //    September 2000        - ends on SAT
            //  S  M Tu  W Th  F  S
            //                 1  2
            //  3  4  5  6  7  8  9
            // 10 11 12 13 14 15 16
            // 17 18 19 20 21 22 23
            // 24 25 26 27 28 29 30
            //
            // date.dayOfWeek() == SAT
            { L_,     SUN,         "2000,09,23",    "2000,09,24" },    // true
            { L_,     MON,         "2000,09,23",    "2000,09,25" },    // true
            { L_,     TUE,         "2000,09,23",    "2000,09,26" },    // true
            { L_,     WED,         "2000,09,23",    "2000,09,27" },    // true
            { L_,     THU,         "2000,09,23",    "2000,09,28" },    // true
            { L_,     FRI,         "2000,09,23",    "2000,09,29" },    // true
            { L_,     SAT,         "2000,09,23",    "2000,09,23" },    // true

            { L_,     SUN,         "2000,09,30",    "2000,09,24" },    // false
            { L_,     MON,         "2000,09,30",    "2000,09,25" },    // false
            { L_,     TUE,         "2000,09,30",    "2000,09,26" },    // false
            { L_,     WED,         "2000,09,30",    "2000,09,27" },    // false
            { L_,     THU,         "2000,09,30",    "2000,09,28" },    // false
            { L_,     FRI,         "2000,09,30",    "2000,09,29" },    // false
            { L_,     SAT,         "2000,09,30",    "2000,09,30" },    // true
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                 LINE        = DATA[i].d_lineNum;
            const bdet_DayOfWeek::Day DAY_OF_WEEK = DATA[i].d_dayOfWeek;
            const bdet_Date           DATE        = parseDate(DATA[i].d_date);
            const bdet_Date           RESULT      = parseDate(
                                                             DATA[i].d_result);

            if (veryVerbose) {
                T_
                P_(LINE) P_(DAY_OF_WEEK) P_(DATE) P(RESULT);
            }

            // Check 'RESULT' using oracle.
            {
                const bdet_Date CEIL_DAY = Util::ceilDay(DAY_OF_WEEK, DATE);

                if (CEIL_DAY.month() == DATE.month()) {
                    LOOP3_ASSERT(LINE, RESULT, CEIL_DAY, RESULT == CEIL_DAY);
                }
                else {
                    const bdet_Date PREVIOUS_DAY = Util::previousDay(
                                                                   DAY_OF_WEEK,
                                                                   DATE);
                    LOOP3_ASSERT(LINE, RESULT,   PREVIOUS_DAY,
                                       RESULT == PREVIOUS_DAY);
                }
            }

            bdet_Date result = Util::ceilOrPreviousDayInMonth(DAY_OF_WEEK,
                                                              DATE);
            LOOP2_ASSERT(LINE, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'ceilOrPreviousDayInMonth' Function "
                          << "Test." << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'floorAdjustDay' FUNCTION
        //   This will test the 'floorAdjustDay' function.
        //
        // Concerns:
        //   1. If 'count' is zero, the function must return the same value as
        //      'floorDay'.
        //   2. If 'count' is not zero, the function must return the date that
        //      is 'count' weeks from 'floorDay'.
        //
        //   Profound fact: Calendar irregularities are already compensated for
        //                  by 'bdet_Date' and are therefore not a concern in
        //                  this test.
        //
        // Plan:
        //   For a set of test data, exercise the 'floorAdjustDay' function and
        //   check that the returned value is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //      initialDate.dayOfWeek       dayOfWeek      count
        //      ---------------------       ---------      -----
        //
        //             | SUN |               | SUN |
        //             | MON |               | MON |       | -2 |
        //             | TUE |               | TUE |       | -1 |
        //             | WED |       X       | WED |   X   |  0 |
        //             | THU |               | THU |       |  1 |
        //             | FRI |               | FRI |       |  2 |
        //             | SAT |               | SAT |
        //
        //   Note that the value of 'initialDate' can be any arbitrary date
        //   that conforms to the scenario being tested.
        //
        // Testing:
        //   b_D floorAdjustDay(b_DOW dayOfWeek, c b_D& initialDate,int count);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'floorAdjustDay' Function"
                          << "\n=================================" << endl;

        static const struct {
            int                  d_lineNum;      // source line number
            bdet_DayOfWeek::Day  d_dayOfWeek;    // dayOfWeek
            const char          *d_initialDate;  // initialDate
            int                  d_count;        // count
            const char          *d_result;       // expected result
        } DATA[] = {
            //    December 1999
            //  S  M Tu  W Th  F  S
            //           1  2  3  4
            //  5  6  7  8  9 10 11
            // 12 13 14 15 16 17 18
            // 19 20 21 22 23 24 25
            // 26 27 28 29 30 31
            //
            //    January 2000
            //  S  M Tu  W Th  F  S
            //                    1
            //  2  3  4  5  6  7  8
            //  9 10 11 12 13 14 15
            // 16 17 18 19 20 21 22
            // 23 24 25 26 27 28 29
            // 30 31
            //
            //line    dayOfWeek    initialDate      count    result
            //----    ---------    -----------      -----    ------

            // initialDate.dayOfWeek() == SUN
            { L_,     SUN,         "2000,01,09",    -2,      "1999,12,26" },
            { L_,     MON,         "2000,01,09",    -2,      "1999,12,20" },
            { L_,     TUE,         "2000,01,09",    -2,      "1999,12,21" },
            { L_,     WED,         "2000,01,09",    -2,      "1999,12,22" },
            { L_,     THU,         "2000,01,09",    -2,      "1999,12,23" },
            { L_,     FRI,         "2000,01,09",    -2,      "1999,12,24" },
            { L_,     SAT,         "2000,01,09",    -2,      "1999,12,25" },

            { L_,     SUN,         "2000,01,09",    -1,      "2000,01,02" },
            { L_,     MON,         "2000,01,09",    -1,      "1999,12,27" },
            { L_,     TUE,         "2000,01,09",    -1,      "1999,12,28" },
            { L_,     WED,         "2000,01,09",    -1,      "1999,12,29" },
            { L_,     THU,         "2000,01,09",    -1,      "1999,12,30" },
            { L_,     FRI,         "2000,01,09",    -1,      "1999,12,31" },
            { L_,     SAT,         "2000,01,09",    -1,      "2000,01,01" },

            { L_,     SUN,         "2000,01,09",    0,       "2000,01,09" },
            { L_,     MON,         "2000,01,09",    0,       "2000,01,03" },
            { L_,     TUE,         "2000,01,09",    0,       "2000,01,04" },
            { L_,     WED,         "2000,01,09",    0,       "2000,01,05" },
            { L_,     THU,         "2000,01,09",    0,       "2000,01,06" },
            { L_,     FRI,         "2000,01,09",    0,       "2000,01,07" },
            { L_,     SAT,         "2000,01,09",    0,       "2000,01,08" },

            { L_,     SUN,         "2000,01,09",    1,       "2000,01,16" },
            { L_,     MON,         "2000,01,09",    1,       "2000,01,10" },
            { L_,     TUE,         "2000,01,09",    1,       "2000,01,11" },
            { L_,     WED,         "2000,01,09",    1,       "2000,01,12" },
            { L_,     THU,         "2000,01,09",    1,       "2000,01,13" },
            { L_,     FRI,         "2000,01,09",    1,       "2000,01,14" },
            { L_,     SAT,         "2000,01,09",    1,       "2000,01,15" },

            { L_,     SUN,         "2000,01,09",    2,       "2000,01,23" },
            { L_,     MON,         "2000,01,09",    2,       "2000,01,17" },
            { L_,     TUE,         "2000,01,09",    2,       "2000,01,18" },
            { L_,     WED,         "2000,01,09",    2,       "2000,01,19" },
            { L_,     THU,         "2000,01,09",    2,       "2000,01,20" },
            { L_,     FRI,         "2000,01,09",    2,       "2000,01,21" },
            { L_,     SAT,         "2000,01,09",    2,       "2000,01,22" },

            // initialDate.dayOfWeek() == MON
            { L_,     SUN,         "2000,01,10",    -2,      "1999,12,26" },
            { L_,     MON,         "2000,01,10",    -2,      "1999,12,27" },
            { L_,     TUE,         "2000,01,10",    -2,      "1999,12,21" },
            { L_,     WED,         "2000,01,10",    -2,      "1999,12,22" },
            { L_,     THU,         "2000,01,10",    -2,      "1999,12,23" },
            { L_,     FRI,         "2000,01,10",    -2,      "1999,12,24" },
            { L_,     SAT,         "2000,01,10",    -2,      "1999,12,25" },

            { L_,     SUN,         "2000,01,10",    -1,      "2000,01,02" },
            { L_,     MON,         "2000,01,10",    -1,      "2000,01,03" },
            { L_,     TUE,         "2000,01,10",    -1,      "1999,12,28" },
            { L_,     WED,         "2000,01,10",    -1,      "1999,12,29" },
            { L_,     THU,         "2000,01,10",    -1,      "1999,12,30" },
            { L_,     FRI,         "2000,01,10",    -1,      "1999,12,31" },
            { L_,     SAT,         "2000,01,10",    -1,      "2000,01,01" },

            { L_,     SUN,         "2000,01,10",    0,       "2000,01,09" },
            { L_,     MON,         "2000,01,10",    0,       "2000,01,10" },
            { L_,     TUE,         "2000,01,10",    0,       "2000,01,04" },
            { L_,     WED,         "2000,01,10",    0,       "2000,01,05" },
            { L_,     THU,         "2000,01,10",    0,       "2000,01,06" },
            { L_,     FRI,         "2000,01,10",    0,       "2000,01,07" },
            { L_,     SAT,         "2000,01,10",    0,       "2000,01,08" },

            { L_,     SUN,         "2000,01,10",    1,       "2000,01,16" },
            { L_,     MON,         "2000,01,10",    1,       "2000,01,17" },
            { L_,     TUE,         "2000,01,10",    1,       "2000,01,11" },
            { L_,     WED,         "2000,01,10",    1,       "2000,01,12" },
            { L_,     THU,         "2000,01,10",    1,       "2000,01,13" },
            { L_,     FRI,         "2000,01,10",    1,       "2000,01,14" },
            { L_,     SAT,         "2000,01,10",    1,       "2000,01,15" },

            { L_,     SUN,         "2000,01,10",    2,       "2000,01,23" },
            { L_,     MON,         "2000,01,10",    2,       "2000,01,24" },
            { L_,     TUE,         "2000,01,10",    2,       "2000,01,18" },
            { L_,     WED,         "2000,01,10",    2,       "2000,01,19" },
            { L_,     THU,         "2000,01,10",    2,       "2000,01,20" },
            { L_,     FRI,         "2000,01,10",    2,       "2000,01,21" },
            { L_,     SAT,         "2000,01,10",    2,       "2000,01,22" },

            // initialDate.dayOfWeek() == TUE
            { L_,     SUN,         "2000,01,11",    -2,      "1999,12,26" },
            { L_,     MON,         "2000,01,11",    -2,      "1999,12,27" },
            { L_,     TUE,         "2000,01,11",    -2,      "1999,12,28" },
            { L_,     WED,         "2000,01,11",    -2,      "1999,12,22" },
            { L_,     THU,         "2000,01,11",    -2,      "1999,12,23" },
            { L_,     FRI,         "2000,01,11",    -2,      "1999,12,24" },
            { L_,     SAT,         "2000,01,11",    -2,      "1999,12,25" },

            { L_,     SUN,         "2000,01,11",    -1,      "2000,01,02" },
            { L_,     MON,         "2000,01,11",    -1,      "2000,01,03" },
            { L_,     TUE,         "2000,01,11",    -1,      "2000,01,04" },
            { L_,     WED,         "2000,01,11",    -1,      "1999,12,29" },
            { L_,     THU,         "2000,01,11",    -1,      "1999,12,30" },
            { L_,     FRI,         "2000,01,11",    -1,      "1999,12,31" },
            { L_,     SAT,         "2000,01,11",    -1,      "2000,01,01" },

            { L_,     SUN,         "2000,01,11",    0,       "2000,01,09" },
            { L_,     MON,         "2000,01,11",    0,       "2000,01,10" },
            { L_,     TUE,         "2000,01,11",    0,       "2000,01,11" },
            { L_,     WED,         "2000,01,11",    0,       "2000,01,05" },
            { L_,     THU,         "2000,01,11",    0,       "2000,01,06" },
            { L_,     FRI,         "2000,01,11",    0,       "2000,01,07" },
            { L_,     SAT,         "2000,01,11",    0,       "2000,01,08" },

            { L_,     SUN,         "2000,01,11",    1,       "2000,01,16" },
            { L_,     MON,         "2000,01,11",    1,       "2000,01,17" },
            { L_,     TUE,         "2000,01,11",    1,       "2000,01,18" },
            { L_,     WED,         "2000,01,11",    1,       "2000,01,12" },
            { L_,     THU,         "2000,01,11",    1,       "2000,01,13" },
            { L_,     FRI,         "2000,01,11",    1,       "2000,01,14" },
            { L_,     SAT,         "2000,01,11",    1,       "2000,01,15" },

            { L_,     SUN,         "2000,01,11",    2,       "2000,01,23" },
            { L_,     MON,         "2000,01,11",    2,       "2000,01,24" },
            { L_,     TUE,         "2000,01,11",    2,       "2000,01,25" },
            { L_,     WED,         "2000,01,11",    2,       "2000,01,19" },
            { L_,     THU,         "2000,01,11",    2,       "2000,01,20" },
            { L_,     FRI,         "2000,01,11",    2,       "2000,01,21" },
            { L_,     SAT,         "2000,01,11",    2,       "2000,01,22" },

            // initialDate.dayOfWeek() == WED
            { L_,     SUN,         "2000,01,12",    -2,      "1999,12,26" },
            { L_,     MON,         "2000,01,12",    -2,      "1999,12,27" },
            { L_,     TUE,         "2000,01,12",    -2,      "1999,12,28" },
            { L_,     WED,         "2000,01,12",    -2,      "1999,12,29" },
            { L_,     THU,         "2000,01,12",    -2,      "1999,12,23" },
            { L_,     FRI,         "2000,01,12",    -2,      "1999,12,24" },
            { L_,     SAT,         "2000,01,12",    -2,      "1999,12,25" },

            { L_,     SUN,         "2000,01,12",    -1,      "2000,01,02" },
            { L_,     MON,         "2000,01,12",    -1,      "2000,01,03" },
            { L_,     TUE,         "2000,01,12",    -1,      "2000,01,04" },
            { L_,     WED,         "2000,01,12",    -1,      "2000,01,05" },
            { L_,     THU,         "2000,01,12",    -1,      "1999,12,30" },
            { L_,     FRI,         "2000,01,12",    -1,      "1999,12,31" },
            { L_,     SAT,         "2000,01,12",    -1,      "2000,01,01" },

            { L_,     SUN,         "2000,01,12",    0,       "2000,01,09" },
            { L_,     MON,         "2000,01,12",    0,       "2000,01,10" },
            { L_,     TUE,         "2000,01,12",    0,       "2000,01,11" },
            { L_,     WED,         "2000,01,12",    0,       "2000,01,12" },
            { L_,     THU,         "2000,01,12",    0,       "2000,01,06" },
            { L_,     FRI,         "2000,01,12",    0,       "2000,01,07" },
            { L_,     SAT,         "2000,01,12",    0,       "2000,01,08" },

            { L_,     SUN,         "2000,01,12",    1,       "2000,01,16" },
            { L_,     MON,         "2000,01,12",    1,       "2000,01,17" },
            { L_,     TUE,         "2000,01,12",    1,       "2000,01,18" },
            { L_,     WED,         "2000,01,12",    1,       "2000,01,19" },
            { L_,     THU,         "2000,01,12",    1,       "2000,01,13" },
            { L_,     FRI,         "2000,01,12",    1,       "2000,01,14" },
            { L_,     SAT,         "2000,01,12",    1,       "2000,01,15" },

            { L_,     SUN,         "2000,01,12",    2,       "2000,01,23" },
            { L_,     MON,         "2000,01,12",    2,       "2000,01,24" },
            { L_,     TUE,         "2000,01,12",    2,       "2000,01,25" },
            { L_,     WED,         "2000,01,12",    2,       "2000,01,26" },
            { L_,     THU,         "2000,01,12",    2,       "2000,01,20" },
            { L_,     FRI,         "2000,01,12",    2,       "2000,01,21" },
            { L_,     SAT,         "2000,01,12",    2,       "2000,01,22" },

            // initialDate.dayOfWeek() == THU
            { L_,     SUN,         "2000,01,13",    -2,      "1999,12,26" },
            { L_,     MON,         "2000,01,13",    -2,      "1999,12,27" },
            { L_,     TUE,         "2000,01,13",    -2,      "1999,12,28" },
            { L_,     WED,         "2000,01,13",    -2,      "1999,12,29" },
            { L_,     THU,         "2000,01,13",    -2,      "1999,12,30" },
            { L_,     FRI,         "2000,01,13",    -2,      "1999,12,24" },
            { L_,     SAT,         "2000,01,13",    -2,      "1999,12,25" },

            { L_,     SUN,         "2000,01,13",    -1,      "2000,01,02" },
            { L_,     MON,         "2000,01,13",    -1,      "2000,01,03" },
            { L_,     TUE,         "2000,01,13",    -1,      "2000,01,04" },
            { L_,     WED,         "2000,01,13",    -1,      "2000,01,05" },
            { L_,     THU,         "2000,01,13",    -1,      "2000,01,06" },
            { L_,     FRI,         "2000,01,13",    -1,      "1999,12,31" },
            { L_,     SAT,         "2000,01,13",    -1,      "2000,01,01" },

            { L_,     SUN,         "2000,01,13",    0,       "2000,01,09" },
            { L_,     MON,         "2000,01,13",    0,       "2000,01,10" },
            { L_,     TUE,         "2000,01,13",    0,       "2000,01,11" },
            { L_,     WED,         "2000,01,13",    0,       "2000,01,12" },
            { L_,     THU,         "2000,01,13",    0,       "2000,01,13" },
            { L_,     FRI,         "2000,01,13",    0,       "2000,01,07" },
            { L_,     SAT,         "2000,01,13",    0,       "2000,01,08" },

            { L_,     SUN,         "2000,01,13",    1,       "2000,01,16" },
            { L_,     MON,         "2000,01,13",    1,       "2000,01,17" },
            { L_,     TUE,         "2000,01,13",    1,       "2000,01,18" },
            { L_,     WED,         "2000,01,13",    1,       "2000,01,19" },
            { L_,     THU,         "2000,01,13",    1,       "2000,01,20" },
            { L_,     FRI,         "2000,01,13",    1,       "2000,01,14" },
            { L_,     SAT,         "2000,01,13",    1,       "2000,01,15" },

            { L_,     SUN,         "2000,01,13",    2,       "2000,01,23" },
            { L_,     MON,         "2000,01,13",    2,       "2000,01,24" },
            { L_,     TUE,         "2000,01,13",    2,       "2000,01,25" },
            { L_,     WED,         "2000,01,13",    2,       "2000,01,26" },
            { L_,     THU,         "2000,01,13",    2,       "2000,01,27" },
            { L_,     FRI,         "2000,01,13",    2,       "2000,01,21" },
            { L_,     SAT,         "2000,01,13",    2,       "2000,01,22" },

            // initialDate.dayOfWeek() == FRI
            { L_,     SUN,         "2000,01,14",    -2,      "1999,12,26" },
            { L_,     MON,         "2000,01,14",    -2,      "1999,12,27" },
            { L_,     TUE,         "2000,01,14",    -2,      "1999,12,28" },
            { L_,     WED,         "2000,01,14",    -2,      "1999,12,29" },
            { L_,     THU,         "2000,01,14",    -2,      "1999,12,30" },
            { L_,     FRI,         "2000,01,14",    -2,      "1999,12,31" },
            { L_,     SAT,         "2000,01,14",    -2,      "1999,12,25" },

            { L_,     SUN,         "2000,01,14",    -1,      "2000,01,02" },
            { L_,     MON,         "2000,01,14",    -1,      "2000,01,03" },
            { L_,     TUE,         "2000,01,14",    -1,      "2000,01,04" },
            { L_,     WED,         "2000,01,14",    -1,      "2000,01,05" },
            { L_,     THU,         "2000,01,14",    -1,      "2000,01,06" },
            { L_,     FRI,         "2000,01,14",    -1,      "2000,01,07" },
            { L_,     SAT,         "2000,01,14",    -1,      "2000,01,01" },

            { L_,     SUN,         "2000,01,14",    0,       "2000,01,09" },
            { L_,     MON,         "2000,01,14",    0,       "2000,01,10" },
            { L_,     TUE,         "2000,01,14",    0,       "2000,01,11" },
            { L_,     WED,         "2000,01,14",    0,       "2000,01,12" },
            { L_,     THU,         "2000,01,14",    0,       "2000,01,13" },
            { L_,     FRI,         "2000,01,14",    0,       "2000,01,14" },
            { L_,     SAT,         "2000,01,14",    0,       "2000,01,08" },

            { L_,     SUN,         "2000,01,14",    1,       "2000,01,16" },
            { L_,     MON,         "2000,01,14",    1,       "2000,01,17" },
            { L_,     TUE,         "2000,01,14",    1,       "2000,01,18" },
            { L_,     WED,         "2000,01,14",    1,       "2000,01,19" },
            { L_,     THU,         "2000,01,14",    1,       "2000,01,20" },
            { L_,     FRI,         "2000,01,14",    1,       "2000,01,21" },
            { L_,     SAT,         "2000,01,14",    1,       "2000,01,15" },

            { L_,     SUN,         "2000,01,14",    2,       "2000,01,23" },
            { L_,     MON,         "2000,01,14",    2,       "2000,01,24" },
            { L_,     TUE,         "2000,01,14",    2,       "2000,01,25" },
            { L_,     WED,         "2000,01,14",    2,       "2000,01,26" },
            { L_,     THU,         "2000,01,14",    2,       "2000,01,27" },
            { L_,     FRI,         "2000,01,14",    2,       "2000,01,28" },
            { L_,     SAT,         "2000,01,14",    2,       "2000,01,22" },

            // initialDate.dayOfWeek() == SAT
            { L_,     SUN,         "2000,01,15",    -2,      "1999,12,26" },
            { L_,     MON,         "2000,01,15",    -2,      "1999,12,27" },
            { L_,     TUE,         "2000,01,15",    -2,      "1999,12,28" },
            { L_,     WED,         "2000,01,15",    -2,      "1999,12,29" },
            { L_,     THU,         "2000,01,15",    -2,      "1999,12,30" },
            { L_,     FRI,         "2000,01,15",    -2,      "1999,12,31" },
            { L_,     SAT,         "2000,01,15",    -2,      "2000,01,01" },

            { L_,     SUN,         "2000,01,15",    -1,      "2000,01,02" },
            { L_,     MON,         "2000,01,15",    -1,      "2000,01,03" },
            { L_,     TUE,         "2000,01,15",    -1,      "2000,01,04" },
            { L_,     WED,         "2000,01,15",    -1,      "2000,01,05" },
            { L_,     THU,         "2000,01,15",    -1,      "2000,01,06" },
            { L_,     FRI,         "2000,01,15",    -1,      "2000,01,07" },
            { L_,     SAT,         "2000,01,15",    -1,      "2000,01,08" },

            { L_,     SUN,         "2000,01,15",    0,       "2000,01,09" },
            { L_,     MON,         "2000,01,15",    0,       "2000,01,10" },
            { L_,     TUE,         "2000,01,15",    0,       "2000,01,11" },
            { L_,     WED,         "2000,01,15",    0,       "2000,01,12" },
            { L_,     THU,         "2000,01,15",    0,       "2000,01,13" },
            { L_,     FRI,         "2000,01,15",    0,       "2000,01,14" },
            { L_,     SAT,         "2000,01,15",    0,       "2000,01,15" },

            { L_,     SUN,         "2000,01,15",    1,       "2000,01,16" },
            { L_,     MON,         "2000,01,15",    1,       "2000,01,17" },
            { L_,     TUE,         "2000,01,15",    1,       "2000,01,18" },
            { L_,     WED,         "2000,01,15",    1,       "2000,01,19" },
            { L_,     THU,         "2000,01,15",    1,       "2000,01,20" },
            { L_,     FRI,         "2000,01,15",    1,       "2000,01,21" },
            { L_,     SAT,         "2000,01,15",    1,       "2000,01,22" },

            { L_,     SUN,         "2000,01,15",    2,       "2000,01,23" },
            { L_,     MON,         "2000,01,15",    2,       "2000,01,24" },
            { L_,     TUE,         "2000,01,15",    2,       "2000,01,25" },
            { L_,     WED,         "2000,01,15",    2,       "2000,01,26" },
            { L_,     THU,         "2000,01,15",    2,       "2000,01,27" },
            { L_,     FRI,         "2000,01,15",    2,       "2000,01,28" },
            { L_,     SAT,         "2000,01,15",    2,       "2000,01,29" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                 LINE         = DATA[i].d_lineNum;
            const bdet_DayOfWeek::Day DAY_OF_WEEK  = DATA[i].d_dayOfWeek;
            const bdet_Date           INITIAL_DATE = parseDate(
                                                        DATA[i].d_initialDate);
            const int                 COUNT        = DATA[i].d_count;
            const bdet_Date           RESULT       = parseDate(
                                                             DATA[i].d_result);

            if (veryVerbose) {
                T_
                P_(LINE) P_(DAY_OF_WEEK) P_(INITIAL_DATE) P_(COUNT) P(RESULT);
            }

            // Check 'RESULT' with oracle.
            {
                const bdet_Date FLOOR_DAY = Util::floorDay(DAY_OF_WEEK,
                                                           INITIAL_DATE);
                if (0 == COUNT) {
                    LOOP3_ASSERT(LINE, RESULT, FLOOR_DAY, RESULT == FLOOR_DAY);
                }
                else {
                    const bdet_Date ADJUST_DAY = Util::adjustDay(DAY_OF_WEEK,
                                                                 FLOOR_DAY,
                                                                 COUNT);
                    LOOP3_ASSERT(LINE, RESULT,   ADJUST_DAY,
                                       RESULT == ADJUST_DAY);
                }
            }

            bdet_Date result = Util::floorAdjustDay(DAY_OF_WEEK,
                                                    INITIAL_DATE,
                                                    COUNT);
            LOOP2_ASSERT(LINE, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'floorAdjustDay' Function Test."
                          << endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'ceilAdjustDay' FUNCTION
        //   This will test the 'ceilAdjustDay' function.
        //
        // Concerns:
        //   1. If 'count' is zero, the function must return the same value as
        //      'ceilDay'.
        //   2. If 'count' is not zero, the function must return the date that
        //      is 'count' weeks from 'ceilDay'.
        //
        //   Profound fact: Calendar irregularities are already compensated for
        //                  by 'bdet_Date' and are therefore not a concern in
        //                  this test.
        //
        // Plan:
        //   For a set of test data, exercise the 'ceilAdjustDay' function and
        //   check that the returned value is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //      initialDate.dayOfWeek       dayOfWeek      count
        //      ---------------------       ---------      -----
        //
        //             | SUN |               | SUN |
        //             | MON |               | MON |       | -2 |
        //             | TUE |               | TUE |       | -1 |
        //             | WED |       X       | WED |   X   |  0 |
        //             | THU |               | THU |       |  1 |
        //             | FRI |               | FRI |       |  2 |
        //             | SAT |               | SAT |
        //
        //   Note that the value of 'initialDate' can be any arbitrary date
        //   that conforms to the scenario being tested.
        //
        // Testing:
        //   b_D ceilAdjustDay(b_DOW dayOfWeek, c b_D& initialDate, int count);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'ceilAdjustDay' Function"
                          << "\n================================" << endl;

        static const struct {
            int                  d_lineNum;      // source line number
            bdet_DayOfWeek::Day  d_dayOfWeek;    // dayOfWeek
            const char          *d_initialDate;  // initialDate
            int                  d_count;        // count
            const char          *d_result;       // expected result
        } DATA[] = {
            //    December 1999
            //  S  M Tu  W Th  F  S
            //           1  2  3  4
            //  5  6  7  8  9 10 11
            // 12 13 14 15 16 17 18
            // 19 20 21 22 23 24 25
            // 26 27 28 29 30 31
            //
            //    January 2000
            //  S  M Tu  W Th  F  S
            //                    1
            //  2  3  4  5  6  7  8
            //  9 10 11 12 13 14 15
            // 16 17 18 19 20 21 22
            // 23 24 25 26 27 28 29
            // 30 31
            //
            //    February 2000
            //  S  M Tu  W Th  F  S
            //        1  2  3  4  5
            //  6  7  8  9 10 11 12
            // 13 14 15 16 17 18 19
            // 20 21 22 23 24 25 26
            // 27 28 29
            //
            //line    dayOfWeek    initialDate      count    result
            //----    ---------    -----------      -----    ------

            // initialDate.dayOfWeek() == SUN
            { L_,     SUN,         "2000,01,09",    -2,      "1999,12,26" },
            { L_,     MON,         "2000,01,09",    -2,      "1999,12,27" },
            { L_,     TUE,         "2000,01,09",    -2,      "1999,12,28" },
            { L_,     WED,         "2000,01,09",    -2,      "1999,12,29" },
            { L_,     THU,         "2000,01,09",    -2,      "1999,12,30" },
            { L_,     FRI,         "2000,01,09",    -2,      "1999,12,31" },
            { L_,     SAT,         "2000,01,09",    -2,      "2000,01,01" },

            { L_,     SUN,         "2000,01,09",    -1,      "2000,01,02" },
            { L_,     MON,         "2000,01,09",    -1,      "2000,01,03" },
            { L_,     TUE,         "2000,01,09",    -1,      "2000,01,04" },
            { L_,     WED,         "2000,01,09",    -1,      "2000,01,05" },
            { L_,     THU,         "2000,01,09",    -1,      "2000,01,06" },
            { L_,     FRI,         "2000,01,09",    -1,      "2000,01,07" },
            { L_,     SAT,         "2000,01,09",    -1,      "2000,01,08" },

            { L_,     SUN,         "2000,01,09",    0,       "2000,01,09" },
            { L_,     MON,         "2000,01,09",    0,       "2000,01,10" },
            { L_,     TUE,         "2000,01,09",    0,       "2000,01,11" },
            { L_,     WED,         "2000,01,09",    0,       "2000,01,12" },
            { L_,     THU,         "2000,01,09",    0,       "2000,01,13" },
            { L_,     FRI,         "2000,01,09",    0,       "2000,01,14" },
            { L_,     SAT,         "2000,01,09",    0,       "2000,01,15" },

            { L_,     SUN,         "2000,01,09",    1,       "2000,01,16" },
            { L_,     MON,         "2000,01,09",    1,       "2000,01,17" },
            { L_,     TUE,         "2000,01,09",    1,       "2000,01,18" },
            { L_,     WED,         "2000,01,09",    1,       "2000,01,19" },
            { L_,     THU,         "2000,01,09",    1,       "2000,01,20" },
            { L_,     FRI,         "2000,01,09",    1,       "2000,01,21" },
            { L_,     SAT,         "2000,01,09",    1,       "2000,01,22" },

            { L_,     SUN,         "2000,01,09",    2,       "2000,01,23" },
            { L_,     MON,         "2000,01,09",    2,       "2000,01,24" },
            { L_,     TUE,         "2000,01,09",    2,       "2000,01,25" },
            { L_,     WED,         "2000,01,09",    2,       "2000,01,26" },
            { L_,     THU,         "2000,01,09",    2,       "2000,01,27" },
            { L_,     FRI,         "2000,01,09",    2,       "2000,01,28" },
            { L_,     SAT,         "2000,01,09",    2,       "2000,01,29" },

            // initialDate.dayOfWeek() == MON
            { L_,     SUN,         "2000,01,10",    -2,      "2000,01,02" },
            { L_,     MON,         "2000,01,10",    -2,      "1999,12,27" },
            { L_,     TUE,         "2000,01,10",    -2,      "1999,12,28" },
            { L_,     WED,         "2000,01,10",    -2,      "1999,12,29" },
            { L_,     THU,         "2000,01,10",    -2,      "1999,12,30" },
            { L_,     FRI,         "2000,01,10",    -2,      "1999,12,31" },
            { L_,     SAT,         "2000,01,10",    -2,      "2000,01,01" },

            { L_,     SUN,         "2000,01,10",    -1,      "2000,01,09" },
            { L_,     MON,         "2000,01,10",    -1,      "2000,01,03" },
            { L_,     TUE,         "2000,01,10",    -1,      "2000,01,04" },
            { L_,     WED,         "2000,01,10",    -1,      "2000,01,05" },
            { L_,     THU,         "2000,01,10",    -1,      "2000,01,06" },
            { L_,     FRI,         "2000,01,10",    -1,      "2000,01,07" },
            { L_,     SAT,         "2000,01,10",    -1,      "2000,01,08" },

            { L_,     SUN,         "2000,01,10",    0,       "2000,01,16" },
            { L_,     MON,         "2000,01,10",    0,       "2000,01,10" },
            { L_,     TUE,         "2000,01,10",    0,       "2000,01,11" },
            { L_,     WED,         "2000,01,10",    0,       "2000,01,12" },
            { L_,     THU,         "2000,01,10",    0,       "2000,01,13" },
            { L_,     FRI,         "2000,01,10",    0,       "2000,01,14" },
            { L_,     SAT,         "2000,01,10",    0,       "2000,01,15" },

            { L_,     SUN,         "2000,01,10",    1,       "2000,01,23" },
            { L_,     MON,         "2000,01,10",    1,       "2000,01,17" },
            { L_,     TUE,         "2000,01,10",    1,       "2000,01,18" },
            { L_,     WED,         "2000,01,10",    1,       "2000,01,19" },
            { L_,     THU,         "2000,01,10",    1,       "2000,01,20" },
            { L_,     FRI,         "2000,01,10",    1,       "2000,01,21" },
            { L_,     SAT,         "2000,01,10",    1,       "2000,01,22" },

            { L_,     SUN,         "2000,01,10",    2,       "2000,01,30" },
            { L_,     MON,         "2000,01,10",    2,       "2000,01,24" },
            { L_,     TUE,         "2000,01,10",    2,       "2000,01,25" },
            { L_,     WED,         "2000,01,10",    2,       "2000,01,26" },
            { L_,     THU,         "2000,01,10",    2,       "2000,01,27" },
            { L_,     FRI,         "2000,01,10",    2,       "2000,01,28" },
            { L_,     SAT,         "2000,01,10",    2,       "2000,01,29" },

            // initialDate.dayOfWeek() == TUE
            { L_,     SUN,         "2000,01,11",    -2,      "2000,01,02" },
            { L_,     MON,         "2000,01,11",    -2,      "2000,01,03" },
            { L_,     TUE,         "2000,01,11",    -2,      "1999,12,28" },
            { L_,     WED,         "2000,01,11",    -2,      "1999,12,29" },
            { L_,     THU,         "2000,01,11",    -2,      "1999,12,30" },
            { L_,     FRI,         "2000,01,11",    -2,      "1999,12,31" },
            { L_,     SAT,         "2000,01,11",    -2,      "2000,01,01" },

            { L_,     SUN,         "2000,01,11",    -1,      "2000,01,09" },
            { L_,     MON,         "2000,01,11",    -1,      "2000,01,10" },
            { L_,     TUE,         "2000,01,11",    -1,      "2000,01,04" },
            { L_,     WED,         "2000,01,11",    -1,      "2000,01,05" },
            { L_,     THU,         "2000,01,11",    -1,      "2000,01,06" },
            { L_,     FRI,         "2000,01,11",    -1,      "2000,01,07" },
            { L_,     SAT,         "2000,01,11",    -1,      "2000,01,08" },

            { L_,     SUN,         "2000,01,11",    0,       "2000,01,16" },
            { L_,     MON,         "2000,01,11",    0,       "2000,01,17" },
            { L_,     TUE,         "2000,01,11",    0,       "2000,01,11" },
            { L_,     WED,         "2000,01,11",    0,       "2000,01,12" },
            { L_,     THU,         "2000,01,11",    0,       "2000,01,13" },
            { L_,     FRI,         "2000,01,11",    0,       "2000,01,14" },
            { L_,     SAT,         "2000,01,11",    0,       "2000,01,15" },

            { L_,     SUN,         "2000,01,11",    1,       "2000,01,23" },
            { L_,     MON,         "2000,01,11",    1,       "2000,01,24" },
            { L_,     TUE,         "2000,01,11",    1,       "2000,01,18" },
            { L_,     WED,         "2000,01,11",    1,       "2000,01,19" },
            { L_,     THU,         "2000,01,11",    1,       "2000,01,20" },
            { L_,     FRI,         "2000,01,11",    1,       "2000,01,21" },
            { L_,     SAT,         "2000,01,11",    1,       "2000,01,22" },

            { L_,     SUN,         "2000,01,11",    2,       "2000,01,30" },
            { L_,     MON,         "2000,01,11",    2,       "2000,01,31" },
            { L_,     TUE,         "2000,01,11",    2,       "2000,01,25" },
            { L_,     WED,         "2000,01,11",    2,       "2000,01,26" },
            { L_,     THU,         "2000,01,11",    2,       "2000,01,27" },
            { L_,     FRI,         "2000,01,11",    2,       "2000,01,28" },
            { L_,     SAT,         "2000,01,11",    2,       "2000,01,29" },

            // initialDate.dayOfWeek() == WED
            { L_,     SUN,         "2000,01,12",    -2,      "2000,01,02" },
            { L_,     MON,         "2000,01,12",    -2,      "2000,01,03" },
            { L_,     TUE,         "2000,01,12",    -2,      "2000,01,04" },
            { L_,     WED,         "2000,01,12",    -2,      "1999,12,29" },
            { L_,     THU,         "2000,01,12",    -2,      "1999,12,30" },
            { L_,     FRI,         "2000,01,12",    -2,      "1999,12,31" },
            { L_,     SAT,         "2000,01,12",    -2,      "2000,01,01" },

            { L_,     SUN,         "2000,01,12",    -1,      "2000,01,09" },
            { L_,     MON,         "2000,01,12",    -1,      "2000,01,10" },
            { L_,     TUE,         "2000,01,12",    -1,      "2000,01,11" },
            { L_,     WED,         "2000,01,12",    -1,      "2000,01,05" },
            { L_,     THU,         "2000,01,12",    -1,      "2000,01,06" },
            { L_,     FRI,         "2000,01,12",    -1,      "2000,01,07" },
            { L_,     SAT,         "2000,01,12",    -1,      "2000,01,08" },

            { L_,     SUN,         "2000,01,12",    0,       "2000,01,16" },
            { L_,     MON,         "2000,01,12",    0,       "2000,01,17" },
            { L_,     TUE,         "2000,01,12",    0,       "2000,01,18" },
            { L_,     WED,         "2000,01,12",    0,       "2000,01,12" },
            { L_,     THU,         "2000,01,12",    0,       "2000,01,13" },
            { L_,     FRI,         "2000,01,12",    0,       "2000,01,14" },
            { L_,     SAT,         "2000,01,12",    0,       "2000,01,15" },

            { L_,     SUN,         "2000,01,12",    1,       "2000,01,23" },
            { L_,     MON,         "2000,01,12",    1,       "2000,01,24" },
            { L_,     TUE,         "2000,01,12",    1,       "2000,01,25" },
            { L_,     WED,         "2000,01,12",    1,       "2000,01,19" },
            { L_,     THU,         "2000,01,12",    1,       "2000,01,20" },
            { L_,     FRI,         "2000,01,12",    1,       "2000,01,21" },
            { L_,     SAT,         "2000,01,12",    1,       "2000,01,22" },

            { L_,     SUN,         "2000,01,12",    2,       "2000,01,30" },
            { L_,     MON,         "2000,01,12",    2,       "2000,01,31" },
            { L_,     TUE,         "2000,01,12",    2,       "2000,02,01" },
            { L_,     WED,         "2000,01,12",    2,       "2000,01,26" },
            { L_,     THU,         "2000,01,12",    2,       "2000,01,27" },
            { L_,     FRI,         "2000,01,12",    2,       "2000,01,28" },
            { L_,     SAT,         "2000,01,12",    2,       "2000,01,29" },

            // initialDate.dayOfWeek() == THU
            { L_,     SUN,         "2000,01,13",    -2,      "2000,01,02" },
            { L_,     MON,         "2000,01,13",    -2,      "2000,01,03" },
            { L_,     TUE,         "2000,01,13",    -2,      "2000,01,04" },
            { L_,     WED,         "2000,01,13",    -2,      "2000,01,05" },
            { L_,     THU,         "2000,01,13",    -2,      "1999,12,30" },
            { L_,     FRI,         "2000,01,13",    -2,      "1999,12,31" },
            { L_,     SAT,         "2000,01,13",    -2,      "2000,01,01" },

            { L_,     SUN,         "2000,01,13",    -1,      "2000,01,09" },
            { L_,     MON,         "2000,01,13",    -1,      "2000,01,10" },
            { L_,     TUE,         "2000,01,13",    -1,      "2000,01,11" },
            { L_,     WED,         "2000,01,13",    -1,      "2000,01,12" },
            { L_,     THU,         "2000,01,13",    -1,      "2000,01,06" },
            { L_,     FRI,         "2000,01,13",    -1,      "2000,01,07" },
            { L_,     SAT,         "2000,01,13",    -1,      "2000,01,08" },

            { L_,     SUN,         "2000,01,13",    0,       "2000,01,16" },
            { L_,     MON,         "2000,01,13",    0,       "2000,01,17" },
            { L_,     TUE,         "2000,01,13",    0,       "2000,01,18" },
            { L_,     WED,         "2000,01,13",    0,       "2000,01,19" },
            { L_,     THU,         "2000,01,13",    0,       "2000,01,13" },
            { L_,     FRI,         "2000,01,13",    0,       "2000,01,14" },
            { L_,     SAT,         "2000,01,13",    0,       "2000,01,15" },

            { L_,     SUN,         "2000,01,13",    1,       "2000,01,23" },
            { L_,     MON,         "2000,01,13",    1,       "2000,01,24" },
            { L_,     TUE,         "2000,01,13",    1,       "2000,01,25" },
            { L_,     WED,         "2000,01,13",    1,       "2000,01,26" },
            { L_,     THU,         "2000,01,13",    1,       "2000,01,20" },
            { L_,     FRI,         "2000,01,13",    1,       "2000,01,21" },
            { L_,     SAT,         "2000,01,13",    1,       "2000,01,22" },

            { L_,     SUN,         "2000,01,13",    2,       "2000,01,30" },
            { L_,     MON,         "2000,01,13",    2,       "2000,01,31" },
            { L_,     TUE,         "2000,01,13",    2,       "2000,02,01" },
            { L_,     WED,         "2000,01,13",    2,       "2000,02,02" },
            { L_,     THU,         "2000,01,13",    2,       "2000,01,27" },
            { L_,     FRI,         "2000,01,13",    2,       "2000,01,28" },
            { L_,     SAT,         "2000,01,13",    2,       "2000,01,29" },

            // initialDate.dayOfWeek() == FRI
            { L_,     SUN,         "2000,01,14",    -2,      "2000,01,02" },
            { L_,     MON,         "2000,01,14",    -2,      "2000,01,03" },
            { L_,     TUE,         "2000,01,14",    -2,      "2000,01,04" },
            { L_,     WED,         "2000,01,14",    -2,      "2000,01,05" },
            { L_,     THU,         "2000,01,14",    -2,      "2000,01,06" },
            { L_,     FRI,         "2000,01,14",    -2,      "1999,12,31" },
            { L_,     SAT,         "2000,01,14",    -2,      "2000,01,01" },

            { L_,     SUN,         "2000,01,14",    -1,      "2000,01,09" },
            { L_,     MON,         "2000,01,14",    -1,      "2000,01,10" },
            { L_,     TUE,         "2000,01,14",    -1,      "2000,01,11" },
            { L_,     WED,         "2000,01,14",    -1,      "2000,01,12" },
            { L_,     THU,         "2000,01,14",    -1,      "2000,01,13" },
            { L_,     FRI,         "2000,01,14",    -1,      "2000,01,07" },
            { L_,     SAT,         "2000,01,14",    -1,      "2000,01,08" },

            { L_,     SUN,         "2000,01,14",    0,       "2000,01,16" },
            { L_,     MON,         "2000,01,14",    0,       "2000,01,17" },
            { L_,     TUE,         "2000,01,14",    0,       "2000,01,18" },
            { L_,     WED,         "2000,01,14",    0,       "2000,01,19" },
            { L_,     THU,         "2000,01,14",    0,       "2000,01,20" },
            { L_,     FRI,         "2000,01,14",    0,       "2000,01,14" },
            { L_,     SAT,         "2000,01,14",    0,       "2000,01,15" },

            { L_,     SUN,         "2000,01,14",    1,       "2000,01,23" },
            { L_,     MON,         "2000,01,14",    1,       "2000,01,24" },
            { L_,     TUE,         "2000,01,14",    1,       "2000,01,25" },
            { L_,     WED,         "2000,01,14",    1,       "2000,01,26" },
            { L_,     THU,         "2000,01,14",    1,       "2000,01,27" },
            { L_,     FRI,         "2000,01,14",    1,       "2000,01,21" },
            { L_,     SAT,         "2000,01,14",    1,       "2000,01,22" },

            { L_,     SUN,         "2000,01,14",    2,       "2000,01,30" },
            { L_,     MON,         "2000,01,14",    2,       "2000,01,31" },
            { L_,     TUE,         "2000,01,14",    2,       "2000,02,01" },
            { L_,     WED,         "2000,01,14",    2,       "2000,02,02" },
            { L_,     THU,         "2000,01,14",    2,       "2000,02,03" },
            { L_,     FRI,         "2000,01,14",    2,       "2000,01,28" },
            { L_,     SAT,         "2000,01,14",    2,       "2000,01,29" },

            // initialDate.dayOfWeek() == SAT
            { L_,     SUN,         "2000,01,15",    -2,      "2000,01,02" },
            { L_,     MON,         "2000,01,15",    -2,      "2000,01,03" },
            { L_,     TUE,         "2000,01,15",    -2,      "2000,01,04" },
            { L_,     WED,         "2000,01,15",    -2,      "2000,01,05" },
            { L_,     THU,         "2000,01,15",    -2,      "2000,01,06" },
            { L_,     FRI,         "2000,01,15",    -2,      "2000,01,07" },
            { L_,     SAT,         "2000,01,15",    -2,      "2000,01,01" },

            { L_,     SUN,         "2000,01,15",    -1,      "2000,01,09" },
            { L_,     MON,         "2000,01,15",    -1,      "2000,01,10" },
            { L_,     TUE,         "2000,01,15",    -1,      "2000,01,11" },
            { L_,     WED,         "2000,01,15",    -1,      "2000,01,12" },
            { L_,     THU,         "2000,01,15",    -1,      "2000,01,13" },
            { L_,     FRI,         "2000,01,15",    -1,      "2000,01,14" },
            { L_,     SAT,         "2000,01,15",    -1,      "2000,01,08" },

            { L_,     SUN,         "2000,01,15",    0,       "2000,01,16" },
            { L_,     MON,         "2000,01,15",    0,       "2000,01,17" },
            { L_,     TUE,         "2000,01,15",    0,       "2000,01,18" },
            { L_,     WED,         "2000,01,15",    0,       "2000,01,19" },
            { L_,     THU,         "2000,01,15",    0,       "2000,01,20" },
            { L_,     FRI,         "2000,01,15",    0,       "2000,01,21" },
            { L_,     SAT,         "2000,01,15",    0,       "2000,01,15" },

            { L_,     SUN,         "2000,01,15",    1,       "2000,01,23" },
            { L_,     MON,         "2000,01,15",    1,       "2000,01,24" },
            { L_,     TUE,         "2000,01,15",    1,       "2000,01,25" },
            { L_,     WED,         "2000,01,15",    1,       "2000,01,26" },
            { L_,     THU,         "2000,01,15",    1,       "2000,01,27" },
            { L_,     FRI,         "2000,01,15",    1,       "2000,01,28" },
            { L_,     SAT,         "2000,01,15",    1,       "2000,01,22" },

            { L_,     SUN,         "2000,01,15",    2,       "2000,01,30" },
            { L_,     MON,         "2000,01,15",    2,       "2000,01,31" },
            { L_,     TUE,         "2000,01,15",    2,       "2000,02,01" },
            { L_,     WED,         "2000,01,15",    2,       "2000,02,02" },
            { L_,     THU,         "2000,01,15",    2,       "2000,02,03" },
            { L_,     FRI,         "2000,01,15",    2,       "2000,02,04" },
            { L_,     SAT,         "2000,01,15",    2,       "2000,01,29" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                 LINE         = DATA[i].d_lineNum;
            const bdet_DayOfWeek::Day DAY_OF_WEEK  = DATA[i].d_dayOfWeek;
            const bdet_Date           INITIAL_DATE = parseDate(
                                                        DATA[i].d_initialDate);
            const int                 COUNT        = DATA[i].d_count;
            const bdet_Date           RESULT       = parseDate(
                                                             DATA[i].d_result);

            if (veryVerbose) {
                T_
                P_(LINE) P_(DAY_OF_WEEK) P_(INITIAL_DATE) P_(COUNT) P(RESULT);
            }

            // Check 'RESULT' with oracle.
            {
                const bdet_Date CEIL_DAY = Util::ceilDay(DAY_OF_WEEK,
                                                         INITIAL_DATE);
                if (0 == COUNT) {
                    LOOP3_ASSERT(LINE, RESULT, CEIL_DAY, RESULT == CEIL_DAY);
                }
                else {
                    const bdet_Date ADJUST_DAY = Util::adjustDay(DAY_OF_WEEK,
                                                                 CEIL_DAY,
                                                                 COUNT);
                    LOOP3_ASSERT(LINE, RESULT,   ADJUST_DAY,
                                       RESULT == ADJUST_DAY);
                }
            }

            bdet_Date result = Util::ceilAdjustDay(DAY_OF_WEEK,
                                                   INITIAL_DATE,
                                                   COUNT);
            LOOP2_ASSERT(LINE, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'ceilAdjustDay' Function Test." << endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'adjustDay' FUNCTION
        //   This will test the 'adjustDay' function.
        //
        // Concerns:
        //   1. Both negative and positive 'count' values can be used.
        //   2. The correct branch is executed based on the polarity of the
        //      'count' value.
        //   3. If count is positive, the returned date must be count *minus* 1
        //      weeks *after* nextDay(dayOfWeek, initialDate).
        //   4. If count is negative, the returned date must be count *plus* 1
        //      weeks *before* previousDay(dayOfWeek, initialDate).
        //
        //   Profound fact: Calendar irregularities are already compensated for
        //                  by 'bdet_Date' and are therefore not a concern in
        //                  this test.
        //
        // Plan:
        //   For a set of test data, exercise the 'adjustDay' function and
        //   check that the returned value is as expected.  Data is selected
        //   based on the following scenarios:
        //      o When initialDate.dayOfWeek() is equal to dayOfWeek.
        //      o When initialDate.dayOfWeek() is one day before dayOfWeek.
        //      o When initialDate.dayOfWeek() is one day after dayOfWeek.
        //
        //   For each scenario, exercise the following cross-product:
        //
        //          dayOfWeek      count
        //          ---------      -----
        //
        //           | SUN |
        //           | MON |       | -2 |
        //           | TUE |       | -1 |
        //           | WED |   X   |    |
        //           | THU |       |  1 |
        //           | FRI |       |  2 |
        //           | SAT |
        //
        //   Note that the value of 'initialDate' can be any arbitrary date
        //   that conforms to the scenario being tested.
        //
        // Testing:
        //   b_D adjustDay(b_DOW dayOfWeek, c b_D& initialDate, int count);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'adjustDay' Function"
                          << "\n============================" << endl;

        static const struct {
            int                  d_lineNum;      // source line number
            bdet_DayOfWeek::Day  d_dayOfWeek;    // dayOfWeek
            const char          *d_initialDate;  // initialDate
            int                  d_count;        // count
            const char          *d_result;       // expected result
        } DATA[] = {
            //    December 1999
            //  S  M Tu  W Th  F  S
            //           1  2  3  4
            //  5  6  7  8  9 10 11
            // 12 13 14 15 16 17 18
            // 19 20 21 22 23 24 25
            // 26 27 28 29 30 31
            //
            //    January 2000
            //  S  M Tu  W Th  F  S
            //                    1
            //  2  3  4  5  6  7  8
            //  9 10 11 12 13 14 15
            // 16 17 18 19 20 21 22
            // 23 24 25 26 27 28 29
            // 30 31
            //
            //line    dayOfWeek    initialDate      count    result
            //----    ---------    -----------      -----    ------

            // initialDate.dayOfWeek() == dayOfWeek
            { L_,     SUN,         "2000,01,09",    -2,      "1999,12,26" },
            { L_,     MON,         "2000,01,10",    -2,      "1999,12,27" },
            { L_,     TUE,         "2000,01,11",    -2,      "1999,12,28" },
            { L_,     WED,         "2000,01,12",    -2,      "1999,12,29" },
            { L_,     THU,         "2000,01,13",    -2,      "1999,12,30" },
            { L_,     FRI,         "2000,01,14",    -2,      "1999,12,31" },
            { L_,     SAT,         "2000,01,15",    -2,      "2000,01,01" },

            { L_,     SUN,         "2000,01,09",    -1,      "2000,01,02" },
            { L_,     MON,         "2000,01,10",    -1,      "2000,01,03" },
            { L_,     TUE,         "2000,01,11",    -1,      "2000,01,04" },
            { L_,     WED,         "2000,01,12",    -1,      "2000,01,05" },
            { L_,     THU,         "2000,01,13",    -1,      "2000,01,06" },
            { L_,     FRI,         "2000,01,14",    -1,      "2000,01,07" },
            { L_,     SAT,         "2000,01,15",    -1,      "2000,01,08" },

            { L_,     SUN,         "2000,01,09",    1,       "2000,01,16" },
            { L_,     MON,         "2000,01,10",    1,       "2000,01,17" },
            { L_,     TUE,         "2000,01,11",    1,       "2000,01,18" },
            { L_,     WED,         "2000,01,12",    1,       "2000,01,19" },
            { L_,     THU,         "2000,01,13",    1,       "2000,01,20" },
            { L_,     FRI,         "2000,01,14",    1,       "2000,01,21" },
            { L_,     SAT,         "2000,01,15",    1,       "2000,01,22" },

            { L_,     SUN,         "2000,01,09",    2,       "2000,01,23" },
            { L_,     MON,         "2000,01,10",    2,       "2000,01,24" },
            { L_,     TUE,         "2000,01,11",    2,       "2000,01,25" },
            { L_,     WED,         "2000,01,12",    2,       "2000,01,26" },
            { L_,     THU,         "2000,01,13",    2,       "2000,01,27" },
            { L_,     FRI,         "2000,01,14",    2,       "2000,01,28" },
            { L_,     SAT,         "2000,01,15",    2,       "2000,01,29" },

            // initialDate.dayOfWeek() == dayOfWeek - 1
            { L_,     SUN,         "2000,01,08",    -2,      "1999,12,26" },
            { L_,     MON,         "2000,01,09",    -2,      "1999,12,27" },
            { L_,     TUE,         "2000,01,10",    -2,      "1999,12,28" },
            { L_,     WED,         "2000,01,11",    -2,      "1999,12,29" },
            { L_,     THU,         "2000,01,12",    -2,      "1999,12,30" },
            { L_,     FRI,         "2000,01,13",    -2,      "1999,12,31" },
            { L_,     SAT,         "2000,01,14",    -2,      "2000,01,01" },

            { L_,     SUN,         "2000,01,08",    -1,      "2000,01,02" },
            { L_,     MON,         "2000,01,09",    -1,      "2000,01,03" },
            { L_,     TUE,         "2000,01,10",    -1,      "2000,01,04" },
            { L_,     WED,         "2000,01,11",    -1,      "2000,01,05" },
            { L_,     THU,         "2000,01,12",    -1,      "2000,01,06" },
            { L_,     FRI,         "2000,01,13",    -1,      "2000,01,07" },
            { L_,     SAT,         "2000,01,14",    -1,      "2000,01,08" },

            { L_,     SUN,         "2000,01,08",    1,       "2000,01,09" },
            { L_,     MON,         "2000,01,09",    1,       "2000,01,10" },
            { L_,     TUE,         "2000,01,10",    1,       "2000,01,11" },
            { L_,     WED,         "2000,01,11",    1,       "2000,01,12" },
            { L_,     THU,         "2000,01,12",    1,       "2000,01,13" },
            { L_,     FRI,         "2000,01,13",    1,       "2000,01,14" },
            { L_,     SAT,         "2000,01,14",    1,       "2000,01,15" },

            { L_,     SUN,         "2000,01,08",    2,       "2000,01,16" },
            { L_,     MON,         "2000,01,09",    2,       "2000,01,17" },
            { L_,     TUE,         "2000,01,10",    2,       "2000,01,18" },
            { L_,     WED,         "2000,01,11",    2,       "2000,01,19" },
            { L_,     THU,         "2000,01,12",    2,       "2000,01,20" },
            { L_,     FRI,         "2000,01,13",    2,       "2000,01,21" },
            { L_,     SAT,         "2000,01,14",    2,       "2000,01,22" },

            // initialDate.dayOfWeek() == dayOfWeek + 1
            { L_,     SUN,         "2000,01,10",    -2,      "2000,01,02" },
            { L_,     MON,         "2000,01,11",    -2,      "2000,01,03" },
            { L_,     TUE,         "2000,01,12",    -2,      "2000,01,04" },
            { L_,     WED,         "2000,01,13",    -2,      "2000,01,05" },
            { L_,     THU,         "2000,01,14",    -2,      "2000,01,06" },
            { L_,     FRI,         "2000,01,15",    -2,      "2000,01,07" },
            { L_,     SAT,         "2000,01,16",    -2,      "2000,01,08" },

            { L_,     SUN,         "2000,01,10",    -1,      "2000,01,09" },
            { L_,     MON,         "2000,01,11",    -1,      "2000,01,10" },
            { L_,     TUE,         "2000,01,12",    -1,      "2000,01,11" },
            { L_,     WED,         "2000,01,13",    -1,      "2000,01,12" },
            { L_,     THU,         "2000,01,14",    -1,      "2000,01,13" },
            { L_,     FRI,         "2000,01,15",    -1,      "2000,01,14" },
            { L_,     SAT,         "2000,01,16",    -1,      "2000,01,15" },

            { L_,     SUN,         "2000,01,10",    1,       "2000,01,16" },
            { L_,     MON,         "2000,01,11",    1,       "2000,01,17" },
            { L_,     TUE,         "2000,01,12",    1,       "2000,01,18" },
            { L_,     WED,         "2000,01,13",    1,       "2000,01,19" },
            { L_,     THU,         "2000,01,14",    1,       "2000,01,20" },
            { L_,     FRI,         "2000,01,15",    1,       "2000,01,21" },
            { L_,     SAT,         "2000,01,16",    1,       "2000,01,22" },

            { L_,     SUN,         "2000,01,10",    2,       "2000,01,23" },
            { L_,     MON,         "2000,01,11",    2,       "2000,01,24" },
            { L_,     TUE,         "2000,01,12",    2,       "2000,01,25" },
            { L_,     WED,         "2000,01,13",    2,       "2000,01,26" },
            { L_,     THU,         "2000,01,14",    2,       "2000,01,27" },
            { L_,     FRI,         "2000,01,15",    2,       "2000,01,28" },
            { L_,     SAT,         "2000,01,16",    2,       "2000,01,29" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                 LINE         = DATA[i].d_lineNum;
            const bdet_DayOfWeek::Day DAY_OF_WEEK  = DATA[i].d_dayOfWeek;
            const bdet_Date           INITIAL_DATE = parseDate(
                                                        DATA[i].d_initialDate);
            const int                 COUNT        = DATA[i].d_count;
            const bdet_Date           RESULT       = parseDate(
                                                             DATA[i].d_result);

            if (veryVerbose) {
                T_
                P_(LINE) P_(DAY_OF_WEEK) P_(INITIAL_DATE) P_(COUNT) P(RESULT);
            }

            bdet_Date result = Util::adjustDay(DAY_OF_WEEK,
                                               INITIAL_DATE,
                                               COUNT);
            LOOP2_ASSERT(LINE, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'adjustDay' Function Test." << endl;
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'floorDay' FUNCTION
        //   This will test the 'floorDay' function.
        //
        // Concerns:
        //   1. If dayOfWeek is equal to date.dayOfWeek(), date is returned,
        //      *not* one week before date.
        //
        //   Profound fact: Calendar irregularities are already compensated for
        //                  by 'bdet_Date' and are therefore not a concern in
        //                  this test.
        //
        // Plan:
        //   For a set of test data, exercise the 'floorDay' function and check
        //   that the returned value is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //           date.dayOfWeek        dayOfWeek
        //           --------------        ---------
        //
        //               | SUN |            | SUN |
        //               | MON |            | MON |
        //               | TUE |            | TUE |
        //               | WED |      X     | WED |
        //               | THU |            | THU |
        //               | FRI |            | FRI |
        //               | SAT |            | SAT |
        //
        //   Note that the value of 'date' can be any arbitrary date that
        //   conforms to the scenario being tested.
        //
        // Testing:
        //   b_D floorDay(b_DOW dayOfWeek, c b_D& date);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'floorDay' Function"
                          << "\n===========================" << endl;

        static const struct {
            int                  d_lineNum;      // source line number
            bdet_DayOfWeek::Day  d_dayOfWeek;    // dayOfWeek
            const char          *d_date;         // date
            const char          *d_result;       // expected result
        } DATA[] = {
            //    January 2000
            //  S  M Tu  W Th  F  S
            //                    1
            //  2  3  4  5  6  7  8
            //  9 10 11 12 13 14 15
            // 16 17 18 19 20 21 22
            // 23 24 25 26 27 28 29
            // 30 31
            //
            //line    dayOfWeek    date             result
            //----    ---------    ----             ------

            // date.dayOfWeek() == SUN
            { L_,     SUN,         "2000,01,09",    "2000,01,09" },
            { L_,     MON,         "2000,01,09",    "2000,01,03" },
            { L_,     TUE,         "2000,01,09",    "2000,01,04" },
            { L_,     WED,         "2000,01,09",    "2000,01,05" },
            { L_,     THU,         "2000,01,09",    "2000,01,06" },
            { L_,     FRI,         "2000,01,09",    "2000,01,07" },
            { L_,     SAT,         "2000,01,09",    "2000,01,08" },

            // date.dayOfWeek() == MON
            { L_,     SUN,         "2000,01,10",    "2000,01,09" },
            { L_,     MON,         "2000,01,10",    "2000,01,10" },
            { L_,     TUE,         "2000,01,10",    "2000,01,04" },
            { L_,     WED,         "2000,01,10",    "2000,01,05" },
            { L_,     THU,         "2000,01,10",    "2000,01,06" },
            { L_,     FRI,         "2000,01,10",    "2000,01,07" },
            { L_,     SAT,         "2000,01,10",    "2000,01,08" },

            // date.dayOfWeek() == TUE
            { L_,     SUN,         "2000,01,11",    "2000,01,09" },
            { L_,     MON,         "2000,01,11",    "2000,01,10" },
            { L_,     TUE,         "2000,01,11",    "2000,01,11" },
            { L_,     WED,         "2000,01,11",    "2000,01,05" },
            { L_,     THU,         "2000,01,11",    "2000,01,06" },
            { L_,     FRI,         "2000,01,11",    "2000,01,07" },
            { L_,     SAT,         "2000,01,11",    "2000,01,08" },

            // date.dayOfWeek() == WED
            { L_,     SUN,         "2000,01,12",    "2000,01,09" },
            { L_,     MON,         "2000,01,12",    "2000,01,10" },
            { L_,     TUE,         "2000,01,12",    "2000,01,11" },
            { L_,     WED,         "2000,01,12",    "2000,01,12" },
            { L_,     THU,         "2000,01,12",    "2000,01,06" },
            { L_,     FRI,         "2000,01,12",    "2000,01,07" },
            { L_,     SAT,         "2000,01,12",    "2000,01,08" },

            // date.dayOfWeek() == THU
            { L_,     SUN,         "2000,01,13",    "2000,01,09" },
            { L_,     MON,         "2000,01,13",    "2000,01,10" },
            { L_,     TUE,         "2000,01,13",    "2000,01,11" },
            { L_,     WED,         "2000,01,13",    "2000,01,12" },
            { L_,     THU,         "2000,01,13",    "2000,01,13" },
            { L_,     FRI,         "2000,01,13",    "2000,01,07" },
            { L_,     SAT,         "2000,01,13",    "2000,01,08" },

            // date.dayOfWeek() == FRI
            { L_,     SUN,         "2000,01,14",    "2000,01,09" },
            { L_,     MON,         "2000,01,14",    "2000,01,10" },
            { L_,     TUE,         "2000,01,14",    "2000,01,11" },
            { L_,     WED,         "2000,01,14",    "2000,01,12" },
            { L_,     THU,         "2000,01,14",    "2000,01,13" },
            { L_,     FRI,         "2000,01,14",    "2000,01,14" },
            { L_,     SAT,         "2000,01,14",    "2000,01,08" },

            // date.dayOfWeek() == SAT
            { L_,     SUN,         "2000,01,15",    "2000,01,09" },
            { L_,     MON,         "2000,01,15",    "2000,01,10" },
            { L_,     TUE,         "2000,01,15",    "2000,01,11" },
            { L_,     WED,         "2000,01,15",    "2000,01,12" },
            { L_,     THU,         "2000,01,15",    "2000,01,13" },
            { L_,     FRI,         "2000,01,15",    "2000,01,14" },
            { L_,     SAT,         "2000,01,15",    "2000,01,15" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                 LINE        = DATA[i].d_lineNum;
            const bdet_DayOfWeek::Day DAY_OF_WEEK = DATA[i].d_dayOfWeek;
            const bdet_Date           DATE        = parseDate(DATA[i].d_date);
            const bdet_Date           RESULT      = parseDate(
                                                             DATA[i].d_result);

            if (veryVerbose) {
                T_
                P_(LINE) P_(DAY_OF_WEEK) P_(DATE) P(RESULT);
            }

            bdet_Date result = Util::floorDay(DAY_OF_WEEK, DATE);
            LOOP2_ASSERT(LINE, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'floorDay' Function Test." << endl;
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'ceilDay' FUNCTION
        //   This will test the 'ceilDay' function.
        //
        // Concerns:
        //   1. If dayOfWeek is equal to date.dayOfWeek(), date is returned,
        //      *not* one week from date.
        //
        //   Profound fact: Calendar irregularities are already compensated for
        //                  by 'bdet_Date' and are therefore not a concern in
        //                  this test.
        //
        // Plan:
        //   For a set of test data, exercise the 'ceilDay' function and check
        //   that the returned value is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //           date.dayOfWeek        dayOfWeek
        //           --------------        ---------
        //
        //               | SUN |            | SUN |
        //               | MON |            | MON |
        //               | TUE |            | TUE |
        //               | WED |      X     | WED |
        //               | THU |            | THU |
        //               | FRI |            | FRI |
        //               | SAT |            | SAT |
        //
        //   Note that the value of 'date' can be any arbitrary date that
        //   conforms to the scenario being tested.
        //
        // Testing:
        //   b_D ceilDay(b_DOW dayOfWeek, c b_D& date);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'ceilDay' Function"
                          << "\n==========================" << endl;

        static const struct {
            int                  d_lineNum;      // source line number
            bdet_DayOfWeek::Day  d_dayOfWeek;    // dayOfWeek
            const char          *d_date;         // date
            const char          *d_result;       // expected result
        } DATA[] = {
            //    January 2000
            //  S  M Tu  W Th  F  S
            //                    1
            //  2  3  4  5  6  7  8
            //  9 10 11 12 13 14 15
            // 16 17 18 19 20 21 22
            // 23 24 25 26 27 28 29
            // 30 31
            //
            //line    dayOfWeek    date             result
            //----    ---------    ----             ------

            // date.dayOfWeek() == SUN
            { L_,     SUN,         "2000,01,09",    "2000,01,09" },
            { L_,     MON,         "2000,01,09",    "2000,01,10" },
            { L_,     TUE,         "2000,01,09",    "2000,01,11" },
            { L_,     WED,         "2000,01,09",    "2000,01,12" },
            { L_,     THU,         "2000,01,09",    "2000,01,13" },
            { L_,     FRI,         "2000,01,09",    "2000,01,14" },
            { L_,     SAT,         "2000,01,09",    "2000,01,15" },

            // date.dayOfWeek() == MON
            { L_,     SUN,         "2000,01,10",    "2000,01,16" },
            { L_,     MON,         "2000,01,10",    "2000,01,10" },
            { L_,     TUE,         "2000,01,10",    "2000,01,11" },
            { L_,     WED,         "2000,01,10",    "2000,01,12" },
            { L_,     THU,         "2000,01,10",    "2000,01,13" },
            { L_,     FRI,         "2000,01,10",    "2000,01,14" },
            { L_,     SAT,         "2000,01,10",    "2000,01,15" },

            // date.dayOfWeek() == TUE
            { L_,     SUN,         "2000,01,11",    "2000,01,16" },
            { L_,     MON,         "2000,01,11",    "2000,01,17" },
            { L_,     TUE,         "2000,01,11",    "2000,01,11" },
            { L_,     WED,         "2000,01,11",    "2000,01,12" },
            { L_,     THU,         "2000,01,11",    "2000,01,13" },
            { L_,     FRI,         "2000,01,11",    "2000,01,14" },
            { L_,     SAT,         "2000,01,11",    "2000,01,15" },

            // date.dayOfWeek() == WED
            { L_,     SUN,         "2000,01,12",    "2000,01,16" },
            { L_,     MON,         "2000,01,12",    "2000,01,17" },
            { L_,     TUE,         "2000,01,12",    "2000,01,18" },
            { L_,     WED,         "2000,01,12",    "2000,01,12" },
            { L_,     THU,         "2000,01,12",    "2000,01,13" },
            { L_,     FRI,         "2000,01,12",    "2000,01,14" },
            { L_,     SAT,         "2000,01,12",    "2000,01,15" },

            // date.dayOfWeek() == THU
            { L_,     SUN,         "2000,01,13",    "2000,01,16" },
            { L_,     MON,         "2000,01,13",    "2000,01,17" },
            { L_,     TUE,         "2000,01,13",    "2000,01,18" },
            { L_,     WED,         "2000,01,13",    "2000,01,19" },
            { L_,     THU,         "2000,01,13",    "2000,01,13" },
            { L_,     FRI,         "2000,01,13",    "2000,01,14" },
            { L_,     SAT,         "2000,01,13",    "2000,01,15" },

            // date.dayOfWeek() == FRI
            { L_,     SUN,         "2000,01,14",    "2000,01,16" },
            { L_,     MON,         "2000,01,14",    "2000,01,17" },
            { L_,     TUE,         "2000,01,14",    "2000,01,18" },
            { L_,     WED,         "2000,01,14",    "2000,01,19" },
            { L_,     THU,         "2000,01,14",    "2000,01,20" },
            { L_,     FRI,         "2000,01,14",    "2000,01,14" },
            { L_,     SAT,         "2000,01,14",    "2000,01,15" },

            // date.dayOfWeek() == SAT
            { L_,     SUN,         "2000,01,15",    "2000,01,16" },
            { L_,     MON,         "2000,01,15",    "2000,01,17" },
            { L_,     TUE,         "2000,01,15",    "2000,01,18" },
            { L_,     WED,         "2000,01,15",    "2000,01,19" },
            { L_,     THU,         "2000,01,15",    "2000,01,20" },
            { L_,     FRI,         "2000,01,15",    "2000,01,21" },
            { L_,     SAT,         "2000,01,15",    "2000,01,15" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                 LINE        = DATA[i].d_lineNum;
            const bdet_DayOfWeek::Day DAY_OF_WEEK = DATA[i].d_dayOfWeek;
            const bdet_Date           DATE        = parseDate(DATA[i].d_date);
            const bdet_Date           RESULT      = parseDate(
                                                             DATA[i].d_result);

            if (veryVerbose) {
                T_
                P_(LINE) P_(DAY_OF_WEEK) P_(DATE) P(RESULT);
            }

            bdet_Date result = Util::ceilDay(DAY_OF_WEEK, DATE);
            LOOP2_ASSERT(LINE, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'ceilDay' Function Test." << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'previousDay' FUNCTION
        //   This will test the 'previousDay' function.
        //
        // Concerns:
        //   1. If dayOfWeek is equal to date.dayOfWeek(), one week before date
        //      is returned, *not* date.
        //
        //   Profound fact: Calendar irregularities are already compensated for
        //                  by 'bdet_Date' and are therefore not a concern in
        //                  this test.
        //
        // Plan:
        //   For a set of test data, exercise the 'previousDay' function and
        //   check that the returned value is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //          date.dayOfWeek        dayOfWeek
        //          --------------        ---------
        //
        //              | SUN |            | SUN |
        //              | MON |            | MON |
        //              | TUE |            | TUE |
        //              | WED |      X     | WED |
        //              | THU |            | THU |
        //              | FRI |            | FRI |
        //              | SAT |            | SAT |
        //
        //   Note that the value of 'date' can be any arbitrary date that
        //   conforms to the scenario being tested.
        //
        // Testing:
        //   b_D previousDay(b_DOW dayOfWeek, c b_D& date);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'previousDay' Function"
                          << "\n==============================" << endl;

        static const struct {
            int                  d_lineNum;      // source line number
            bdet_DayOfWeek::Day  d_dayOfWeek;    // dayOfWeek
            const char          *d_date;         // date
            const char          *d_result;       // expected result
        } DATA[] = {
            //    January 2000
            //  S  M Tu  W Th  F  S
            //                    1
            //  2  3  4  5  6  7  8
            //  9 10 11 12 13 14 15
            // 16 17 18 19 20 21 22
            // 23 24 25 26 27 28 29
            // 30 31
            //
            //line    dayOfWeek    date             result
            //----    ---------    ----             ------

            // date.dayOfWeek() == SUN
            { L_,     SUN,         "2000,01,09",    "2000,01,02" },
            { L_,     MON,         "2000,01,09",    "2000,01,03" },
            { L_,     TUE,         "2000,01,09",    "2000,01,04" },
            { L_,     WED,         "2000,01,09",    "2000,01,05" },
            { L_,     THU,         "2000,01,09",    "2000,01,06" },
            { L_,     FRI,         "2000,01,09",    "2000,01,07" },
            { L_,     SAT,         "2000,01,09",    "2000,01,08" },

            // date.dayOfWeek() == MON
            { L_,     SUN,         "2000,01,10",    "2000,01,09" },
            { L_,     MON,         "2000,01,10",    "2000,01,03" },
            { L_,     TUE,         "2000,01,10",    "2000,01,04" },
            { L_,     WED,         "2000,01,10",    "2000,01,05" },
            { L_,     THU,         "2000,01,10",    "2000,01,06" },
            { L_,     FRI,         "2000,01,10",    "2000,01,07" },
            { L_,     SAT,         "2000,01,10",    "2000,01,08" },

            // date.dayOfWeek() == TUE
            { L_,     SUN,         "2000,01,11",    "2000,01,09" },
            { L_,     MON,         "2000,01,11",    "2000,01,10" },
            { L_,     TUE,         "2000,01,11",    "2000,01,04" },
            { L_,     WED,         "2000,01,11",    "2000,01,05" },
            { L_,     THU,         "2000,01,11",    "2000,01,06" },
            { L_,     FRI,         "2000,01,11",    "2000,01,07" },
            { L_,     SAT,         "2000,01,11",    "2000,01,08" },

            // date.dayOfWeek() == WED
            { L_,     SUN,         "2000,01,12",    "2000,01,09" },
            { L_,     MON,         "2000,01,12",    "2000,01,10" },
            { L_,     TUE,         "2000,01,12",    "2000,01,11" },
            { L_,     WED,         "2000,01,12",    "2000,01,05" },
            { L_,     THU,         "2000,01,12",    "2000,01,06" },
            { L_,     FRI,         "2000,01,12",    "2000,01,07" },
            { L_,     SAT,         "2000,01,12",    "2000,01,08" },

            // date.dayOfWeek() == THU
            { L_,     SUN,         "2000,01,13",    "2000,01,09" },
            { L_,     MON,         "2000,01,13",    "2000,01,10" },
            { L_,     TUE,         "2000,01,13",    "2000,01,11" },
            { L_,     WED,         "2000,01,13",    "2000,01,12" },
            { L_,     THU,         "2000,01,13",    "2000,01,06" },
            { L_,     FRI,         "2000,01,13",    "2000,01,07" },
            { L_,     SAT,         "2000,01,13",    "2000,01,08" },

            // date.dayOfWeek() == FRI
            { L_,     SUN,         "2000,01,14",    "2000,01,09" },
            { L_,     MON,         "2000,01,14",    "2000,01,10" },
            { L_,     TUE,         "2000,01,14",    "2000,01,11" },
            { L_,     WED,         "2000,01,14",    "2000,01,12" },
            { L_,     THU,         "2000,01,14",    "2000,01,13" },
            { L_,     FRI,         "2000,01,14",    "2000,01,07" },
            { L_,     SAT,         "2000,01,14",    "2000,01,08" },

            // date.dayOfWeek() == SAT
            { L_,     SUN,         "2000,01,15",    "2000,01,09" },
            { L_,     MON,         "2000,01,15",    "2000,01,10" },
            { L_,     TUE,         "2000,01,15",    "2000,01,11" },
            { L_,     WED,         "2000,01,15",    "2000,01,12" },
            { L_,     THU,         "2000,01,15",    "2000,01,13" },
            { L_,     FRI,         "2000,01,15",    "2000,01,14" },
            { L_,     SAT,         "2000,01,15",    "2000,01,08" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                 LINE        = DATA[i].d_lineNum;
            const bdet_DayOfWeek::Day DAY_OF_WEEK = DATA[i].d_dayOfWeek;
            const bdet_Date           DATE        = parseDate(DATA[i].d_date);
            const bdet_Date           RESULT      = parseDate(
                                                             DATA[i].d_result);

            if (veryVerbose) {
                T_
                P_(LINE) P_(DAY_OF_WEEK) P_(DATE) P(RESULT);
            }

            bdet_Date result = Util::previousDay(DAY_OF_WEEK, DATE);
            LOOP2_ASSERT(LINE, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'previousDay' Function Test." << endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'nextDay' FUNCTION
        //   This will test the 'nextDay' function.
        //
        // Concerns:
        //   1. If dayOfWeek is equal to date.dayOfWeek(), one week from date
        //      is returned, *not* date.
        //
        //   Profound fact: Calendar irregularities are already compensated for
        //                  by 'bdet_Date' and are therefore not a concern in
        //                  this test.
        //
        // Plan:
        //   For a set of test data, exercise the 'nextDay' function and check
        //   that the returned value is as expected.
        //
        //   Data is selected based on the following cross-product:
        //
        //          date.dayOfWeek        dayOfWeek
        //          --------------        ---------
        //
        //              | SUN |            | SUN |
        //              | MON |            | MON |
        //              | TUE |            | TUE |
        //              | WED |      X     | WED |
        //              | THU |            | THU |
        //              | FRI |            | FRI |
        //              | SAT |            | SAT |
        //
        //   Note that the value of 'date' can be any arbitrary date that
        //   conforms to the scenario being tested.
        //
        // Testing:
        //   b_D nextDay(b_DOW dayOfWeek, c b_D& date);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'nextDay' Function"
                          << "\n==========================" << endl;

        static const struct {
            int                  d_lineNum;      // source line number
            bdet_DayOfWeek::Day  d_dayOfWeek;    // dayOfWeek
            const char          *d_date;         // date
            const char          *d_result;       // expected result
        } DATA[] = {
            //    January 2000
            //  S  M Tu  W Th  F  S
            //                    1
            //  2  3  4  5  6  7  8
            //  9 10 11 12 13 14 15
            // 16 17 18 19 20 21 22
            // 23 24 25 26 27 28 29
            // 30 31
            //
            //line    dayOfWeek    date             result
            //----    ---------    ----             ------

            // date.dayOfWeek() == SUN
            { L_,     SUN,         "2000,01,09",    "2000,01,16" },
            { L_,     MON,         "2000,01,09",    "2000,01,10" },
            { L_,     TUE,         "2000,01,09",    "2000,01,11" },
            { L_,     WED,         "2000,01,09",    "2000,01,12" },
            { L_,     THU,         "2000,01,09",    "2000,01,13" },
            { L_,     FRI,         "2000,01,09",    "2000,01,14" },
            { L_,     SAT,         "2000,01,09",    "2000,01,15" },

            // date.dayOfWeek() == MON
            { L_,     SUN,         "2000,01,10",    "2000,01,16" },
            { L_,     MON,         "2000,01,10",    "2000,01,17" },
            { L_,     TUE,         "2000,01,10",    "2000,01,11" },
            { L_,     WED,         "2000,01,10",    "2000,01,12" },
            { L_,     THU,         "2000,01,10",    "2000,01,13" },
            { L_,     FRI,         "2000,01,10",    "2000,01,14" },
            { L_,     SAT,         "2000,01,10",    "2000,01,15" },

            // date.dayOfWeek() == TUE
            { L_,     SUN,         "2000,01,11",    "2000,01,16" },
            { L_,     MON,         "2000,01,11",    "2000,01,17" },
            { L_,     TUE,         "2000,01,11",    "2000,01,18" },
            { L_,     WED,         "2000,01,11",    "2000,01,12" },
            { L_,     THU,         "2000,01,11",    "2000,01,13" },
            { L_,     FRI,         "2000,01,11",    "2000,01,14" },
            { L_,     SAT,         "2000,01,11",    "2000,01,15" },

            // date.dayOfWeek() == WED
            { L_,     SUN,         "2000,01,12",    "2000,01,16" },
            { L_,     MON,         "2000,01,12",    "2000,01,17" },
            { L_,     TUE,         "2000,01,12",    "2000,01,18" },
            { L_,     WED,         "2000,01,12",    "2000,01,19" },
            { L_,     THU,         "2000,01,12",    "2000,01,13" },
            { L_,     FRI,         "2000,01,12",    "2000,01,14" },
            { L_,     SAT,         "2000,01,12",    "2000,01,15" },

            // date.dayOfWeek() == THU
            { L_,     SUN,         "2000,01,13",    "2000,01,16" },
            { L_,     MON,         "2000,01,13",    "2000,01,17" },
            { L_,     TUE,         "2000,01,13",    "2000,01,18" },
            { L_,     WED,         "2000,01,13",    "2000,01,19" },
            { L_,     THU,         "2000,01,13",    "2000,01,20" },
            { L_,     FRI,         "2000,01,13",    "2000,01,14" },
            { L_,     SAT,         "2000,01,13",    "2000,01,15" },

            // date.dayOfWeek() == FRI
            { L_,     SUN,         "2000,01,14",    "2000,01,16" },
            { L_,     MON,         "2000,01,14",    "2000,01,17" },
            { L_,     TUE,         "2000,01,14",    "2000,01,18" },
            { L_,     WED,         "2000,01,14",    "2000,01,19" },
            { L_,     THU,         "2000,01,14",    "2000,01,20" },
            { L_,     FRI,         "2000,01,14",    "2000,01,21" },
            { L_,     SAT,         "2000,01,14",    "2000,01,15" },

            // date.dayOfWeek() == SAT
            { L_,     SUN,         "2000,01,15",    "2000,01,16" },
            { L_,     MON,         "2000,01,15",    "2000,01,17" },
            { L_,     TUE,         "2000,01,15",    "2000,01,18" },
            { L_,     WED,         "2000,01,15",    "2000,01,19" },
            { L_,     THU,         "2000,01,15",    "2000,01,20" },
            { L_,     FRI,         "2000,01,15",    "2000,01,21" },
            { L_,     SAT,         "2000,01,15",    "2000,01,22" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                 LINE        = DATA[i].d_lineNum;
            const bdet_DayOfWeek::Day DAY_OF_WEEK = DATA[i].d_dayOfWeek;
            const bdet_Date           DATE        = parseDate(DATA[i].d_date);
            const bdet_Date           RESULT      = parseDate(
                                                             DATA[i].d_result);

            if (veryVerbose) {
                T_
                P_(LINE) P_(DAY_OF_WEEK) P_(DATE) P(RESULT);
            }

            bdet_Date result = Util::nextDay(DAY_OF_WEEK, DATE);
            LOOP2_ASSERT(LINE, result, RESULT == result);
        }

        if (verbose) cout << "\nEnd of 'nextDay' Function Test." << endl;
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
