// baetzo_timezoneidutil.t.cpp                                        -*-C++-*-
#include <baetzo_timezoneidutil.h>

#include <bsl_iostream.h>
#include <bsls_asserttest.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provide a utility for mapping time zone identifiers
//
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 1] getTimeZoneIdFromWindowsTimeZoneId(const char **, const char *);
// [ 2] getWindowsTimeZoneIdFromTimeZoneId(const char **, const char *);
// ============================================================================
// [ 3] USAGE EXAMPLE
//
// ============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
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

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_  cout << "\t" << flush;          // Print a tab (w/o newline)
#define L_ __LINE__                           // current Line number

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

typedef baetzo_TimeZoneIdUtil      Obj;

//=============================================================================
//                      GLOBAL CONSTANTS
//-----------------------------------------------------------------------------

// ============================================================================
//                  GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

//=============================================================================
//                                 HELPER FUNCTIONS
//-----------------------------------------------------------------------------

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
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

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Converting Between Windows and IANA (Olson) Time Zone Identifiers
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, given a Windows time zone identifer, one can convert it to the
// designated equivilant IANA (Olson) identifier using the
// 'getTimeZoneIdFromWindowsTimeZoneId' method.
//..
    int         rc;
    const char *timeZoneId;
    rc = baetzo_TimeZoneIdUtil::getTimeZoneIdFromWindowsTimeZoneId(
                                             &timeZoneId,
                                             "Central Standard Time (Mexico)");
    ASSERT(0 == rc);
    ASSERT(0 == strcmp("America/Mexico_City", timeZoneId));
//..
// Next, one can use the 'getWindowsTimeZoneIdFromTimeZoneId' method, to
// perform the inverse mapping.
//..
    const char *windowsTimeZoneId;
    rc = baetzo_TimeZoneIdUtil::getWindowsTimeZoneIdFromTimeZoneId(
                                                        &windowsTimeZoneId,
                                                        "America/Mexico_City");
    ASSERT(0 == rc);
    ASSERT(0 == strcmp("Central Standard Time (Mexico)", windowsTimeZoneId));
//..
// Finally, neither method provides a mapping if given an unknown time zone
// identifier.
//..
    timeZoneId = (const char *)0xdeadbeef;
    rc = baetzo_TimeZoneIdUtil::getTimeZoneIdFromWindowsTimeZoneId(&timeZoneId,
                                                                   "ABCZ");
    ASSERT(0                        != rc);
    ASSERT((const char *)0xdeadbeef == timeZoneId);
  
    windowsTimeZoneId = (const char*)0xcafed00d;
    rc = baetzo_TimeZoneIdUtil::getWindowsTimeZoneIdFromTimeZoneId(
                                                           &windowsTimeZoneId,
                                                           "XYZA");
    ASSERT(0                        != rc);
    ASSERT((const char *)0xcafed00d == windowsTimeZoneId);
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'getWindowsTimeZoneIdFromTimeZoneId'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   getWindowsTimeZoneIdFromTimeZoneId(const char **, const char *);
        // --------------------------------------------------------------------

        if (verbose) cout
                << endl
                << "CLASS METHOD 'getWindowsTimeZoneIdFromTimeZoneId'" << endl
                << "=================================================" << endl;

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'getTimeZoneIdFromWindowsTimeZoneId'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   getTimeZoneIdFromWindowsTimeZoneId(const char **, const char *);
        // --------------------------------------------------------------------

        if (verbose) cout
                << endl
                << "CLASS METHOD 'getTimeZoneIdFromWindowsTimeZoneId'" << endl
                << "=================================================" << endl;

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
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
