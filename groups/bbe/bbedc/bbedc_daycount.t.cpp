// bbedc_daycount.t.cpp     -*-C++-*-

#include <bbedc_daycount.h>
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
// [ 1] int daysDiff(const bdet_Date& beginDate,
//                   const bdet_Date& endDate,
//                   bbedc_DayCountConvention::Type type);
// [ 2] double yearsDiff(const bdet_Date& beginDate,
//                       const bdet_Date& endDate,
//                       bbedc_DayCountConvention::Type type);
// -------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
//--------------------------------------------------------------------------

// =========================================================================
//                  STANDARD BDE ASSERT TEST MACROS
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

typedef bbedc_DayCount Obj;

const bbedc_DayCountConvention::Type ACTUAL_360 =
                                    bbedc_DayCountConvention::BBEDC_ACTUAL_360;
const bbedc_DayCountConvention::Type ACTUAL_365_FIXED =
                              bbedc_DayCountConvention::BBEDC_ACTUAL_365_FIXED;
const bbedc_DayCountConvention::Type ICMA_ACTUAL_ACTUAL =
                            bbedc_DayCountConvention::BBEDC_ICMA_ACTUAL_ACTUAL;
const bbedc_DayCountConvention::Type ISDA_ACTUAL_ACTUAL =
                            bbedc_DayCountConvention::BBEDC_ISDA_ACTUAL_ACTUAL;
const bbedc_DayCountConvention::Type ISMA_ACTUAL_ACTUAL =
                            bbedc_DayCountConvention::BBEDC_ISMA_ACTUAL_ACTUAL;
const bbedc_DayCountConvention::Type ISMA_30_360 =
                                   bbedc_DayCountConvention::BBEDC_ISMA_30_360;
const bbedc_DayCountConvention::Type PSA_30_360_EOM =
                                bbedc_DayCountConvention::BBEDC_PSA_30_360_EOM;
const bbedc_DayCountConvention::Type SIA_30_360_EOM =
                                bbedc_DayCountConvention::BBEDC_SIA_30_360_EOM;
const bbedc_DayCountConvention::Type SIA_30_360_NEOM =
                               bbedc_DayCountConvention::BBEDC_SIA_30_360_NEOM;

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

        const int daysDiff = bbedc_DayCount::
                daysDiff(d1, d2,
                         bbedc_DayCountConvention::BBEDC_ISDA_ACTUAL_ACTUAL);
        ASSERT(73 == daysDiff);

        const double yearsDiff = bbedc_DayCount::
               yearsDiff(d1, d2,
                         bbedc_DayCountConvention::BBEDC_ISDA_ACTUAL_ACTUAL);
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
        //   Each function must return the expected value.  Use of white-box
        //   testing reduces the number of needed cases to only those needed to
        //   differentiate the conventions.
        //
        // Plan:
        //   Specify a set S of {pairs of dates (d1, d2) and their difference
        //   in years D}.  For each of the operators under test, in a loop over
        //   the elements of S, apply the method to dates having the values d1
        //   and d2 and confirm the results using the value D.
        //
        // Testing
        //   double yearsDiff(const bdet_Date& beginDate,
        //                    const bdet_Date& endDate,
        //                    bbedc_DayCountConvention::Type type);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting: 'yearsDiff(beginDate, endDate, type)'"
                          << "\n=============================================="
                          << endl;

        {
            static const struct {
                int    d_lineNum;                      // source line number
                bbedc_DayCountConvention::Type d_type; // convention to use
                int    d_year1;                        // beginDate year
                int    d_month1;                       // beginDate month
                int    d_day1;                         // beginDate day
                int    d_year2;                        // endDate year
                int    d_month2;                       // endDate month
                int    d_day2;                         // endDate day
                double d_numYears;                     // result # of years
            } DATA[] = {

    //                          - - -first - - -  - - - second - -
    //line  type                year  mnth  day  year  mnth  day  numYears
    //----  ----------------    ----  ----  ---  ----  ----  ---  --------
    { L_,   ACTUAL_360,         1993,   12,  15, 1993,   12,  31, 0.0444444 },
    { L_,   ACTUAL_360,         2003,    2,  28, 2004,    2,  29, 1.01667 },

    { L_,   ACTUAL_365_FIXED,   1993,   12,  15, 1993,   12,  31, 0.0438356 },
    { L_,   ACTUAL_365_FIXED,   2003,    2,  28, 2004,    2,  29, 1.00274 },

    { L_,   ICMA_ACTUAL_ACTUAL, 1993,   12,  15, 1993,   12,  31, 0.0438356 },
    { L_,   ICMA_ACTUAL_ACTUAL, 2003,    2,  28, 2004,    2,  29, 1.0000000 },

    { L_,   ISDA_ACTUAL_ACTUAL, 1993,   12,  15, 1993,   12,  31, 0.0438356 },
    { L_,   ISDA_ACTUAL_ACTUAL, 2003,    2,  28, 2004,    2,  29, 1.0023 },

    { L_,   ISMA_30_360,        1993,   12,  15, 1993,   12,  31, 0.0416667 },
    { L_,   ISMA_30_360,        2003,    2,  28, 2004,    2,  29, 1.00278 },

    { L_,   PSA_30_360_EOM,     1993,   12,  15, 1993,   12,  31, 0.0444444 },
    { L_,   PSA_30_360_EOM,     2003,    2,  28, 2004,    2,  29, 0.997222 },

    { L_,   SIA_30_360_EOM,     1993,   12,  15, 1993,   12,  31, 0.0444444 },
    { L_,   SIA_30_360_EOM,     2003,    2,  28, 2004,    2,  29, 1.0000 },

    { L_,   SIA_30_360_NEOM,    1993,   12,  15, 1993,   12,  31, 0.0444444 },
    { L_,   SIA_30_360_NEOM,    2003,    2,  28, 2004,    2,  29, 1.00278 },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (verbose) cout <<
                "\nTesting: 'yearsDiff(beginDate, endDate, type)'" << endl;

            // Ensure the test data differentiates each convention type.
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const bbedc_DayCountConvention::Type CONV1 =
                                                                DATA[i].d_type;
                    for (int j = i + 1; j < NUM_DATA; ++j) {
                        const bbedc_DayCountConvention::Type CONV2 =
                                                                DATA[j].d_type;
                        if (CONV1 != CONV2) {
                            int hasUnique = 0;
                            for (int ii = 0; ii < NUM_DATA; ++ii) {
                                if (CONV1 == DATA[ii].d_type) {
                                  for (int jj = 0; jj < NUM_DATA; ++jj) {
                                      if (CONV2 == DATA[jj].d_type) {
                        //---------------^
                        if (DATA[ii].d_year1    == DATA[jj].d_year1 &&
                            DATA[ii].d_month1   == DATA[jj].d_month1 &&
                            DATA[ii].d_day1     == DATA[jj].d_day1 &&
                            DATA[ii].d_year2    == DATA[jj].d_year2 &&
                            DATA[ii].d_month2   == DATA[jj].d_month2 &&
                            DATA[ii].d_day2     == DATA[jj].d_day2 &&
                            DATA[ii].d_numYears != DATA[jj].d_numYears) {
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
                const int    LINE      = DATA[di].d_lineNum;
                const double NUM_YEARS = DATA[di].d_numYears;
                const bbedc_DayCountConvention::Type CONV = DATA[di].d_type;
                bdet_Date x(DATA[di].d_year1,
                            DATA[di].d_month1,
                            DATA[di].d_day1);
                const bdet_Date& X = x;
                bdet_Date y(DATA[di].d_year2,
                            DATA[di].d_month2,
                            DATA[di].d_day2);
                const bdet_Date& Y = y;
                if (veryVerbose) {
                    T_ P_(X) P_(Y) P(CONV);
                    T_ T_ T_ T_ T_ T_ T_ P(NUM_YEARS);
                    T_ T_ T_ T_ T_ T_ T_;
                }
                const double RESULT = Obj::yearsDiff(X, Y, CONV);

                if (veryVerbose) { P(RESULT); }
                const double diff = NUM_YEARS - RESULT;
                LOOP3_ASSERT(LINE, NUM_YEARS, RESULT,
                                        -0.000005 <= diff && diff <= 0.000005);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING STATIC daysDiff FUNCTION:
        //
        // Concerns:
        //   Each function must return the expected value.  Use of white-box
        //   testing reduces the number of needed cases to only those needed to
        //   differentiate the conventions.
        //
        // Plan:
        //   Specify a set S of {pairs of dates (d1, d2) and their difference
        //   in days D}.  For each of the operators under test, in a loop over
        //   the elements of S, apply the method to dates having the values d1
        //   and d2 and confirm the results using the value D.
        //
        // Testing
        //   int daysDiff(const bdet_Date& beginDate,
        //                const bdet_Date& endDate,
        //                bbedc_DayCountConvention::Type type);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting: 'daysDiff(beginDate, endDate, type)'"
                          << "\n============================================="
                          << endl;

        {
            static const struct {
                int d_lineNum;                         // source line number
                bbedc_DayCountConvention::Type d_type; // convention to use
                int d_year1;                           // beginDate year
                int d_month1;                          // beginDate month
                int d_day1;                            // beginDate day
                int d_year2;                           // endDate year
                int d_month2;                          // endDate month
                int d_day2;                            // endDate day
                int d_numDays;                         // result number of days
            } DATA[] = {

    //                        - - -first - - -  - - - second - -
    //line  type              year  month  day  year  month  day  numDays
    //----  ----------------  ----  -----  ---  ----  -----  ---  -------

    { L_,   ISDA_ACTUAL_ACTUAL, 1993,    12,  15, 1993,    12,  31,      16 },
    { L_,   ISDA_ACTUAL_ACTUAL, 2003,     2,  28, 2004,     2,  29,     366 },

    { L_,   ACTUAL_360,         1993,    12,  15, 1993,    12,  31,      16 },
    { L_,   ACTUAL_360,         2003,     2,  28, 2004,     2,  29,     366 },

    { L_,   ACTUAL_365_FIXED,   1993,    12,  15, 1993,    12,  31,      16 },
    { L_,   ACTUAL_365_FIXED,   2003,     2,  28, 2004,     2,  29,     366 },

//  { L_,   ISMA_ACTUAL_ACTUAL, 1993,    12,  15, 1993,    12,  31,      16 },
//  { L_,   ISMA_ACTUAL_ACTUAL, 2003,     2,  28, 2004,     2,  29,     366 },

    { L_,   ISMA_30_360,        1993,    12,  15, 1993,    12,  31,      15 },
    { L_,   ISMA_30_360,        2003,     2,  28, 2004,     2,  29,     361 },

    { L_,   SIA_30_360_EOM,     1993,    12,  15, 1993,    12,  31,      16 },
    { L_,   SIA_30_360_EOM,     2003,     2,  28, 2004,     2,  29,     360 },

    { L_,   SIA_30_360_NEOM,    1993,    12,  15, 1993,    12,  31,      16 },
    { L_,   SIA_30_360_NEOM,    2003,     2,  28, 2004,     2,  29,     361 },

    { L_,   PSA_30_360_EOM,     1993,    12,  15, 1993,    12,  31,      16 },
    { L_,   PSA_30_360_EOM,     2003,     2,  28, 2004,     2,  29,     359 },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (verbose) cout <<
                "\nTesting: 'daysDiff(beginDate, endDate, type)'" << endl;

            // Ensure the test data differentiates each convention type.
            // Note: this part of the test is getting inappropriate as we
            // add more conventions, some of which count days identically.
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    const bbedc_DayCountConvention::Type CONV1 =
                                                                DATA[i].d_type;
                    for (int j = i + 1; j < NUM_DATA; ++j) {
                        const bbedc_DayCountConvention::Type CONV2 =
                                                                DATA[j].d_type;
                        if (CONV1 != CONV2) {
                            if (CONV1 == ISMA_ACTUAL_ACTUAL ||
                                CONV1 == ACTUAL_360 ||
                                CONV1 == ACTUAL_365_FIXED ||
                                CONV2 == ISMA_ACTUAL_ACTUAL ||
                                CONV2 == ACTUAL_360 ||
                                CONV2 == ACTUAL_365_FIXED) {
                                continue;
                            }

                            int hasUnique = 0;
                            for (int ii = 0; ii < NUM_DATA; ++ii) {
                                if (CONV1 == DATA[ii].d_type) {
                                  for (int jj = 0; jj < NUM_DATA; ++jj) {
                                      if (CONV2 == DATA[jj].d_type) {
                        //---------------^
                        if (DATA[ii].d_year1   == DATA[jj].d_year1 &&
                            DATA[ii].d_month1  == DATA[jj].d_month1 &&
                            DATA[ii].d_day1    == DATA[jj].d_day1 &&
                            DATA[ii].d_year2   == DATA[jj].d_year2 &&
                            DATA[ii].d_month2  == DATA[jj].d_month2 &&
                            DATA[ii].d_day2    == DATA[jj].d_day2 &&
                            DATA[ii].d_numDays != DATA[jj].d_numDays) {
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
                const int LINE                            = DATA[di].d_lineNum;
                const int NUM_DAYS                        = DATA[di].d_numDays;
                const bbedc_DayCountConvention::Type CONV = DATA[di].d_type;
                bdet_Date x(DATA[di].d_year1,
                            DATA[di].d_month1,
                            DATA[di].d_day1);
                const bdet_Date& X = x;
                bdet_Date y(DATA[di].d_year2,
                            DATA[di].d_month2,
                            DATA[di].d_day2);
                const bdet_Date& Y = y;
                if (veryVerbose) {
                    T_ P_(X) P_(Y) P(CONV);
                    T_ T_ T_ T_ T_ T_ T_ P(NUM_DAYS);
                    T_ T_ T_ T_ T_ T_ T_;
                }
                const int RESULT = Obj::daysDiff(X, Y, CONV);

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
