// bbedc_isdaactualactual.t.cpp     -*-C++-*-

#include <bbedc_isdaactualactual.h>
#include <bdet_date.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script




// =========================================================================
//                              TEST PLAN
// -------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of static member functions (pure
// procedures) that compute the day count and term between two dates.  The
// general plan is that the methods are tested against a set of tabulated test
// vectors.
//-----------------------------------------------------------------------------
// [ 1] int daysDiff(const bdet_Date& beginDate, const bdet_Date& endDate);
// [ 2] double yearsDiff(const bdet_Date& beginDate, const bdet_Date& endDate);
// -------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
//--------------------------------------------------------------------------



// ============================================================================
//                  STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

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


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bbedc_IsdaActualActual Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
       // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose mode, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Examples"
                          << "\n======================" << endl;

        const bdet_Date d1(2003, 10, 19);
        const bdet_Date d2(2003, 12, 31);

        const int daysDiff = bbedc_IsdaActualActual::daysDiff(d1, d2);
        ASSERT(73 == daysDiff);

        const double yearsDiff = bbedc_IsdaActualActual::yearsDiff(d1, d2);
        // Need fuzzy comparison since 'yearsDiff' is a double.  Expect
        // 0.2 == 'yearsDiff'.
        ASSERT(0.1999 < yearsDiff);
        ASSERT(0.2001 > yearsDiff);

        if (veryVerbose) {
            P_(daysDiff);
            P(yearsDiff);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING STATIC yearsDiff FUNCTION:
        //
        // Concerns:
        //   Each function must return the expected value.
        //
        // Plan:
        //   Specify a set S of {pairs of dates (d1, d2) and their difference
        //   in years D}.  For each of the operators under test, in a loop over
        //   the elements of S, apply the method to dates having the values d1
        //   and d2 and confirm the results using the value D.
        //
        // Testing
        //   double yearsDiff(const bdet_Date&, const bdet_Date&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting: 'yearsDiff(beginDate, endDate)'"
                          << "\n========================================"
                          << endl;

        {
            static const struct {
                int    d_lineNum;   // source line number
                int    d_year1;     // beginDate year
                int    d_month1;    // beginDate month
                int    d_day1;      // beginDate day
                int    d_year2;     // endDate year
                int    d_month2;    // endDate month
                int    d_day2;      // endDate day
                double d_numYears;  // result number of years
            } DATA[] = {
                //       - - - -first- - - -   - - - second- - - -
                //line   year   month   day    year   month   day    numYears
                //----   -----  -----  -----   -----  -----  -----   --------
                { L_,     1992,     2,     1,   1992,     3,     1,   0.0792 },
                { L_,     1992,     2,     1,   1993,     3,     1,   1.0769 },
                { L_,     1992,     3,     1,   1992,     2,     1,  -0.0792 },
                { L_,     1993,     2,     1,   1993,     3,     1,   0.0767 },
                { L_,     1993,     2,     1,   1996,     2,     1,   2.9998 },
                { L_,     1993,     3,     1,   1992,     2,     1,  -1.0769 },
                { L_,     1993,     3,     1,   1993,     2,     1,  -0.0767 },
                { L_,     1996,     1,    15,   1996,     5,    31,   0.3743 },
                { L_,     1996,     2,     1,   1993,     2,     1,  -2.9998 },
                { L_,     2000,     2,    27,   2000,     2,    27,   0.0000 },
                { L_,     2000,     2,    27,   2000,     2,    28,   0.0027 },
                { L_,     2000,     2,    27,   2000,     2,    29,   0.0055 },
                { L_,     2000,     2,    28,   2000,     2,    27,  -0.0027 },
                { L_,     2000,     2,    28,   2000,     2,    28,   0.0000 },
                { L_,     2000,     2,    28,   2000,     2,    29,   0.0027 },
                { L_,     2000,     2,    29,   2000,     2,    27,  -0.0055 },
                { L_,     2000,     2,    29,   2000,     2,    28,  -0.0027 },
                { L_,     2000,     2,    29,   2000,     2,    29,   0.0000 },
                { L_,     2001,     1,     1,   2003,     1,     1,   2.0000 },
                { L_,     2003,     1,     1,   2001,     1,     1,  -2.0000 },
                { L_,     2003,     2,    28,   2004,     2,    29,   1.0023 },
                { L_,     2004,     2,    29,   2003,     2,    28,  -1.0023 },

                // The following taken from ISDA - BS:9951.1.
                { L_,     1999,     2,     1,   1999,     7,     1,   0.4110 },
                { L_,     1999,     7,     1,   2000,     7,     1,   1.0014 },
                { L_,     1999,     7,    30,   2000,     1,    30,   0.5039 },
                { L_,     1999,    11,    30,   2000,     4,    30,   0.4155 },
                { L_,     2000,     1,    30,   2000,     6,    30,   0.4153 },
                { L_,     2002,     8,    15,   2003,     7,    15,   0.9151 },
                { L_,     2003,    11,     1,   2004,     5,     1,   0.4977 },

                // Test negative results of previous.
                { L_,     1999,     7,     1,   1999,     2,     1,  -0.4110 },
                { L_,     2000,     1,    30,   1999,     7,    30,  -0.5039 },
                { L_,     2000,     4,    30,   1999,    11,    30,  -0.4155 },
                { L_,     2000,     6,    30,   2000,     1,    30,  -0.4153 },
                { L_,     2000,     7,     1,   1999,     7,     1,  -1.0014 },
                { L_,     2003,     7,    15,   2002,     8,    15,  -0.9151 },
                { L_,     2004,     5,     1,   2003,    11,     1,  -0.4977 },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int di;
            if (verbose) cout <<
                "\nTesting: 'yearsDiff(beginDate, endDate)'" << endl;
            for (di = 0; di < NUM_DATA ; ++di) {
                const int    LINE      = DATA[di].d_lineNum;
                const double NUM_YEARS = DATA[di].d_numYears;
                bdet_Date x(DATA[di].d_year1,
                            DATA[di].d_month1,
                            DATA[di].d_day1);
                const bdet_Date& X = x;
                bdet_Date y(DATA[di].d_year2,
                            DATA[di].d_month2,
                            DATA[di].d_day2);
                const bdet_Date& Y = y;
                if (veryVerbose) { T_;  P_(X);  P_(Y);  P_(NUM_YEARS); }
                double result, diff;
                result = Obj::yearsDiff(X, Y);

                if (veryVerbose) { P(result); }
                diff = NUM_YEARS - result;
                LOOP_ASSERT(LINE, -0.00005 <= diff && diff <= 0.00005);

                result = Obj::yearsDiff(Y, X);

                if (veryVerbose) { P(result); }
                diff = - NUM_YEARS - result;
                LOOP_ASSERT(LINE, -0.00005 <= diff && diff <= 0.00005);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING STATIC daysDiff FUNCTION:
        //
        // Concerns:
        //   Each function must return the expected value.  Use of white-box
        //   testing reduces the number of needed negative-result cases.
        //
        // Plan:
        //   Specify a set S of {pairs of dates (d1, d2) and their difference
        //   in days D}.  For each of the operators under test, in a loop over
        //   the elements of S, apply the method to dates having the values d1
        //   and d2 and confirm the results using the value D.
        //
        // Testing
        //   int daysDiff(const bdet_Date&, const bdet_Date&);
        // --------------------------------------------------------------------

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

            int di;
            if (verbose) cout <<
                "\nTesting: 'daysDiff(beginDate, endDate)'" << endl;
            for (di = 0; di < NUM_DATA ; ++di) {
                const int LINE     = DATA[di].d_lineNum;
                const int NUM_DAYS = DATA[di].d_numDays;
                bdet_Date x(DATA[di].d_year1,
                            DATA[di].d_month1,
                            DATA[di].d_day1);
                const bdet_Date& X = x;
                bdet_Date y(DATA[di].d_year2,
                            DATA[di].d_month2,
                            DATA[di].d_day2);
                const bdet_Date& Y = y;
                if (veryVerbose) { T_;  P_(X);  P_(Y);  P_(NUM_DAYS); }
                const int RESULT = Obj::daysDiff(X, Y);

                if (veryVerbose) { P(RESULT); }
                LOOP_ASSERT(LINE, NUM_DAYS == RESULT);
            }
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
