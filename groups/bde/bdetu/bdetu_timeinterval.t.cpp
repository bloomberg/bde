// bdetu_timeinterval.t.cpp         -*-C++-*-

#include <bdetu_timeinterval.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


// ==========================================================================
//                          TEST PLAN
// --------------------------------------------------------------------------
//                          Overview
//                          --------
//
//
//---------------------------------------------------------------------------
// [1] void convertToTimeInterval();
//---------------------------------------------------------------------------
// [2] USAGE example
//=============================================================================
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

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t"                             \
                    << #J << ": " << J << "\t" << #K << ": " << K << "\t"  \
                    << #L << ": " << L << "\t" << #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define F_(X)  cout << X << flush;            // Print X, usually space filler
#define NL()  cout << endl;                   // Print newline
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------



// ==========================================================================
//                      HELPER FUNCTIONS FOR TESTING
// --------------------------------------------------------------------------


// =========================================================================
//                          MAIN PROGRAM
// -------------------------------------------------------------------------


int main(int argc, char *argv[])
{

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
    case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
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

        if (verbose) cout << "\nTesting Usage Example"
                             "\n=====================" << endl;
        {
            bdet_DatetimeInterval dti(0, 8, 15, 14, 416);
                                   // 8 hr. 15 min. 14.416 sec.
            bdet_TimeInterval ti;

            bdetu_TimeInterval::convertToTimeInterval(&ti, dti);
            if (verbose) {
                bsl::cout << ti << bsl::endl;
            }
        }
    } break;
    case 1: {
        // --------------------------------------------------------------------
        // TESTING 'convertToTimeInterval' METHOD
        //   The 'convertToTimeInterval' function converts a
        //   'bdet_DatetimeInterval' into a 'bdet_TimeInterval'.
        //
        // Concerns:
        //  That the conversion is done correctly.
        //
        // Plan:
        //   First create several 'bdet_DatetimeInterval' objects, then convert
        //   them to 'bdet_TimeInterval' objects and check the correctness of
        //   the conversion using the available accessor methods of both
        //   classes.
        //
        // Testing:
        //   static void
        //   convertToTimeInterval(bdet_TimeInterval            *result,
        //                         const bdet_DatetimeInterval&  timeInterval);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'convertToTimeInterval' method"
                 << "\n======================================" << endl;
        {
            static const struct {
                int d_line;
                int d_days;
                int d_hours;
                int d_minutes;
                int d_seconds;
                int d_msecs;
            } VALUES[] = {
                {L_,      0,   0,   0,   0,    0},
                {L_,     -1,  -1,  -1,  -1,   -1},
                {L_,      0,   0,   0,   0,  999},
                {L_,      0,   0,   0,   0, -999},
                {L_,      0,   0,   0,  59,    0},
                {L_,      0,   0,   0, -59,    0},
                {L_,      0,   0,  59,   0,    0},
                {L_,      0,   0, -59,   0,    0},
                {L_,      0,  23,   0,   0,    0},
                {L_,      0, -23,   0,   0,    0},
                {L_,     45,   0,   0,   0,    0},
                {L_,    -45,   0,   0,   0,    0},
                {L_,     45,  23,  59,  59,  999},
                {L_,    -45, -23, -59, -59, -999},
                {L_,     45,  23,  22,  21,  206},
                {L_,     45,  23,  22,  21,  207},
                {L_,     45,  23,  22,  21,  208},
                {L_,  10000,  23,  22,  21,  206},
                {L_, -10000, -23, -22, -21, -206},
           };

           const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

           for (int i = 0; i < NUM_VALUES; ++i) {
               const int LINE      = VALUES[i].d_line;
               const int DAYS      = VALUES[i].d_days;
               const int HOURS     = VALUES[i].d_hours;
               const int MINS      = VALUES[i].d_minutes;
               const int SECS      = VALUES[i].d_seconds;
               const int MSECS     = VALUES[i].d_msecs;

               bdet_DatetimeInterval dti(DAYS, HOURS, MINS, SECS, MSECS);

               if (veryVerbose) {
                   T_() P_(LINE) P_(i) T_() P(dti)
                   T_() P_(DAYS) P_(HOURS) P_(MINS) P_(SECS) P(MSECS)
               }

               bdet_TimeInterval ti;
               bdetu_TimeInterval::convertToTimeInterval(&ti, dti);
               LOOP_ASSERT(LINE,
                           ti.totalMilliseconds() == dti.totalMilliseconds());
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
