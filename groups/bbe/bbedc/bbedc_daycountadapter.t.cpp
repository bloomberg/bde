// bbedc_daycountadapter.t.cpp     -*-C++-*-

#include <bbedc_daycountadapter.h>
#include <bbedc_isdaactualactual.h>
#include <bbedc_isma30360.h>
#include <bdet_date.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script




//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// The component under test consists of two forwarding methods to a templatized
// class containing static member functions (pure procedures) that compute the
// day count and term between two dates.  The general plan is that the methods
// are tested with two different templatizations to ensure the methods forward
// correctly.
//-----------------------------------------------------------------------------
// [ 1] int daysDiff(const bdet_Date& d1, const bdet_Date& d2) const;
// [ 1] double yearsDiff(const bdet_Date& d1, const bdet_Date& d2) const;
//-----------------------------------------------------------------------------
// [ 2] USAGE TEST - Make sure main usage example compiles and works properly.
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
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

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
      case 2: {
       // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // Concerns:
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

        if (verbose) cout << "\nTesting Usage Examples"
                          << "\n======================" << endl;

        bbedc_DayCountAdapter<bbedc_Isma30360> myDcc;
        bbedc_DayCountInterface& dcc = myDcc;

        const bdet_Date d1(2003, 10, 18);
        const bdet_Date d2(2003, 12, 31);

        const int daysDiff = dcc.daysDiff(d1, d2);
        ASSERT(72 == daysDiff);

        const double yearsDiff = dcc.yearsDiff(d1, d2);
        // Need fuzzy comparison since 'yearsDiff' is a double.  Expect
        // 0.2 == 'yearsDiff'.
        ASSERT(0.1999 < yearsDiff);
        ASSERT(0.2001 > yearsDiff);

        if (veryVerbose) {
            P_(daysDiff);
            P(yearsDiff);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // INHERITANCE MECHANISM TEST:
        // Concerns:
        //   We must ensure that (1) the adaptation of a day-count convention
        //   class compiles and links (all virtual functions are defined), and
        //   (2) the functions are in fact virtual and accessible from the
        //   'bbedc_DayCountInterface'.
        //
        // Plan:
        //   Construct an adapted object of a class (which is derived from
        //   'bbedc_DayCountInterface') and bind a 'bbedc_DayCountInterface'
        //   reference to the object.  Using the base class reference, invoke
        //   the 'daysDiff' and 'yearsDiff' methods.  Verify that the correct
        //   implementations of the methods are called.
        //
        // Testing:
        //   int daysDiff(const bdet_Date& d1, const bdet_Date& d2) const;
        //   double yearsDiff(const bdet_Date& d1,const bdet_Date& d2) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        bdet_Date DATE1(1993, 12, 15);
        bdet_Date DATE2(1993, 12, 31);
        bdet_Date DATE3(2003,  2, 28);
        bdet_Date DATE4(2004,  2, 29);

        if (verbose) cout << "\nTesting 'daysDiff(date1, date2)'" << endl;
        {
            {
                bbedc_DayCountAdapter<bbedc_IsdaActualActual> mX;
                const bbedc_DayCountInterface& protocol = mX;

                ASSERT( 16 == protocol.daysDiff(DATE1, DATE2));
                ASSERT(366 == protocol.daysDiff(DATE3, DATE4));
            }

            {
                bbedc_DayCountAdapter<bbedc_Isma30360> mX;
                const bbedc_DayCountInterface& protocol = mX;

                ASSERT( 15 == protocol.daysDiff(DATE1, DATE2));
                ASSERT(361 == protocol.daysDiff(DATE3, DATE4));
            }
        }

        if (verbose) cout << "\nTesting 'yearsDiff(date1, date2)'" << endl;
        {
            {
                bbedc_DayCountAdapter<bbedc_IsdaActualActual> mX;
                bbedc_DayCountInterface& protocol = mX;

                double diff1 = 0.0438 - protocol.yearsDiff(DATE1, DATE2);
                ASSERT(-0.00005 <= diff1 && diff1 <= 0.00005);

                double diff2 = 1.0023 - protocol.yearsDiff(DATE3, DATE4);
                ASSERT(-0.00005 <= diff2 && diff2 <= 0.00005);
            }

            {
                bbedc_DayCountAdapter<bbedc_Isma30360> mX;
                bbedc_DayCountInterface& protocol = mX;

                double diff1 = 0.0417 - protocol.yearsDiff(DATE1, DATE2);
                ASSERT(-0.00005 <= diff1 && diff1 <= 0.00005);

                double diff2 = 1.0028 - protocol.yearsDiff(DATE3, DATE4);
                ASSERT(-0.00005 <= diff2 && diff2 <= 0.00005);
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
