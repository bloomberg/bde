// bdetu_datetimeinterval.t.cpp         -*-C++-*-

#include <bdetu_datetimeinterval.h>
#include <bdet_datetimeinterval.h>
#include <bdet_timeinterval.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
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
// [1] void convertToDatetimeInterval();
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
            bdet_TimeInterval ti(10801, 14031416);  // 10801.014031416 seconds
            bdet_DatetimeInterval dti;

            bdetu_DatetimeInterval::convertToDatetimeInterval(&dti, ti);
            if (verbose) {
                cout << dti << endl;    // 3 hours 1.014 minutes
            }
        }
    } break;
    case 1: {
        // --------------------------------------------------------------------
        // TESTING 'convertToDatetimeInterval' METHOD
        //  The 'convertToDatetimeInterval' function converts an input
        //  'bdet_TimeInterval' into a 'bdet_DatetimeInterval'.  Since the
        //  'bdet_TimeInterval' has greater precision, down to nanoseconds, we
        //  expect lose a little information, rounding the nanoseconds towards
        //  0 into milliseconds.  'bdet_TimeInterval' also has greater range
        //  than 'bdet_DatetimeInterval' so that large positive or large
        //  negative values may overflow.
        //
        // Concerns:
        //  That the conversion is done correctly.
        //  That there is no overflow for "reasonably" large time intervals.
        //  That the loss of precision is limited to the order of .0001 second.
        //
        // Plan:
        //   First create several 'bdet_TimeInterval' objects from an Int64 for
        //   seconds and an int for nanoseconds.  Then, for each one, use the
        //   'convertToDatetimeInterval' method to convert each one into a
        //   'bdet_DatetimeInterval' and compare it's actual to it's expected
        //   output format.
        //
        // Testing:
        //   convertToDatetimeInterval(bdet_DatetimeInterval    *result,
        //                             const bdet_TimeInterval&  timeinterval);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'convertToDatetimeInterval'"
                             "\n===================================" << endl;

        {
            static const struct {
                int                       d_lineNum;  // source line number
                bsls_PlatformUtil::Int64  d_seconds;  // second field value
                int                       d_nsecs;    // nanosecond field value
                const char               *d_fmt_p;    // expected output format
            } DATA[] = {
        //------^
        //line             secs       nsecs      expected output format
        //---- ----------------   ---------   -------------------------
        {L_,                0LL,          0,          "+0_00:00:00.000"},
        {L_,                0LL,        999,          "+0_00:00:00.000"},
        {L_,                0LL,       -999,          "+0_00:00:00.000"},
        {L_,               10LL,  666666666,          "+0_00:00:10.666"},
        {L_,             3600LL,    1999999,          "+0_01:00:00.001"},
        {L_,            -5000LL, -123456789,          "-0_01:23:20.123"},
        {L_,             6000LL,  876543210,          "+0_01:40:00.876"},
        {L_,          -864300LL,  -88888888,         "-10_00:05:00.088"},
        {L_,       3000000000LL,  999999999,      "+34722_05:20:00.999"},
        {L_,      -3000000000LL, -999999999,      "-34722_05:20:00.999"},
        {L_,   99999999999999LL,  999999999, "+1157407407_09:46:39.999"},
        {L_,  180000000000000LL,  999999999, "+2083333333_08:00:00.999"},
        {L_, -180000000000000LL, -999999999, "-2083333333_08:00:00.999"},
        {L_,  185535000000000LL,     999999, "+2147395833_08:00:00.000"},
        {L_,  185535000000000LL,    9999999, "+2147395833_08:00:00.009"},
        {L_, -185540000000000LL, -979999999, "-2147453703_16:53:20.979"},
        //------v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 128;
            char buf[SIZE];

            for (int i = 0; i < NUM_DATA; ++i) {
                int                       LINE  = DATA[i].d_lineNum;
                bsls_PlatformUtil::Int64  SECS  = DATA[i].d_seconds;
                int                       NSECS = DATA[i].d_nsecs;
                const char               *FMT   = DATA[i].d_fmt_p;

                bdet_DatetimeInterval dti;
                bdet_TimeInterval ti(SECS, NSECS);
                bdetu_DatetimeInterval::convertToDatetimeInterval(&dti, ti);

                if (veryVerbose) {
                    P_(LINE)  P_(ti)  P(dti)
                }

                bsl::memset(buf, 0, SIZE);

                bsl::ostrstream out(buf, SIZE);  out << dti << bsl::ends;
                LOOP_ASSERT(LINE, 0 == bsl::strcmp(buf, FMT));

                if (veryVerbose) {
                    P_(buf)  P(FMT)
                }
            }
        }
    } break;
    default:
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
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
