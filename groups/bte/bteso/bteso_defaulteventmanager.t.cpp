// bteso_defaulteventmanager.t.cpp      -*-C++-*-

#include <bteso_defaulteventmanager.h>

#include <bteso_platform.h>
#include <bteso_timemetrics.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bsl_iostream.h>
#include <bsl_c_stdio.h>
#include <bsl_c_stdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//  We just need to verify that the typedefs are properly hooked up and
// an instance of default event manager can be created.  We also need to
// verify that an instance of specialized event manager can be created.
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] bteso_EventMgr::TYPE
//-----------------------------------------------------------------------------
// [ 1] USAGE EXAMPLE
// [ 1] BREATHING TEST
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int          globalVerbose = 0,
         globalVeryVerbose = 0,
     globalVeryVeryVerbose = 0;

//==========================================================================
//                      MAIN PROGRAM
//--------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;                 globalVerbose = verbose;
    int veryVerbose = argc > 3;         globalVeryVerbose = veryVerbose;
    int veryVeryVerbose = argc > 4; globalVeryVeryVerbose = veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // -----------------------------------------------------------------
        // BREATHING TEST
        //    Verify that
        //      o  a default instance can be created
        //      o  a specialized instance can be created
        //
        // Testing:
        //   Create an object of this event manager under test.
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (veryVerbose) cout << "\tCreating a default instance." << endl;
        {
            bteso_TimeMetrics metrics(
                                   bteso_TimeMetrics::BTESO_MIN_NUM_CATEGORIES,
                                   bteso_TimeMetrics::BTESO_CPU_BOUND,
                                   &testAllocator);
            bteso_EventMgr::TYPE mX(&metrics, &testAllocator);
        }

        if (veryVerbose) cout << "\tCreating an instance explicitly."
                              << endl;
        {
            bteso_DefaultEventManager<bteso_Platform::SELECT>
                 mX((bteso_TimeMetrics*)0, &testAllocator);
        }
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
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
