// baetzo_localtimeoffsetutil.t.cpp                                   -*-C++-*-
#include <baetzo_localtimeoffsetutil.h>

#include <bsl_iostream.h>
#include <bsls_types.h>

#include <bsls_asserttest.h>
#include <bsl_cstring.h>     // 'strcmp'

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides a utility for ...
//
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ x] int loadLocalTimeOffset(int *result, const bdet_Datetime& utc);
// [ 2] const baetzo_LocalTimePeriod& localTimePeriod();
// [ x] bdetu_SystemTime::LLTOC setLoadLocalTimeOffsetCallback();
// [ 2] int setTimezone();
// [ 2] int setTimezone(const char *timezone);
// [ 2] int setTimezone(const char *timezone, const bdet_Datetime& utc);
// [ 2] const char *timezone();
// [ x] int updateCount();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE

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

bool         g_verbose;
bool     g_veryVerbose;
bool g_veryVeryVerbose;

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

typedef baetzo_LocalTimeOffsetUtil Obj;

//=============================================================================
//                      GLOBAL CONSTANTS
//-----------------------------------------------------------------------------

const char *NY  = "America/New_York";
const char *RY  = "Asia/Riyadh";
const char *SA  = "Asia/Saigon";
const char *GMT = "Etc/GMT";
const char *GP1 = "Etc/GMT+1";
const char *GM1 = "Etc/GMT-1";


// ============================================================================
//                  GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------


//=============================================================================
//                                 HELPER FUNCTIONS
//-----------------------------------------------------------------------------

// ============================================================================
//                                 USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace UsageExample1 {
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'loadLocalTimeOffset' as the Local Time Offset Callback
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we must quickly generate time stamp values in local time (e.g., on
// records for a high frequency logger) and the default performance of the
// relevant methods of 'bdetu_SystemTime' is inadequate.  Further suppose that
// we must do so arbitrary time values and time zones.  Those requirements can
// be met by installing the 'loadLocalTimeOffset' method of
// 'baetzo_LocalTimeOffsetUtil' as the local time callback used by
// 'bdetu_SystemTime'.
//
// First, specify the time zone to be used by the callback and a UTC date time
// for the initial offset information in the cache.
//..
void main1()
{
    ASSERT(0 == baetzo_LocalTimeOffsetUtil::updateCount());

    int status = baetzo_LocalTimeOffsetUtil::setTimezone("America/New_York",
                                                          bdet_Datetime(2013,
                                                                           2,
                                                                          26));
    ASSERT(0 == status);
    ASSERT(1 == baetzo_LocalTimeOffsetUtil::updateCount());
    ASSERT(0 == strcmp("America/New_York",
                       baetzo_LocalTimeOffsetUtil::timezone()));
//..
// Notice that the value returned by the 'updateCount' method is increased
// by one after then time zone information has been set.
//
// Then, use the 'setLoadLocalTimeOffsetCallback' method to set the
// 'loadLocalTimeOffset' of 'baetzo_LocalTimeOffsetUtil' as the local time
// offset callback used in 'bdetu_SystemTime'.
//..
    bdetu_SystemTime::LoadLocalTimeOffsetCallback previousCallback =
                  baetzo_LocalTimeOffsetUtil::setLoadLocalTimeOffsetCallback();

    ASSERT(&baetzo_LocalTimeOffsetUtil::loadLocalTimeOffset
        == bdetu_SystemTime::currentLoadLocalTimeOffsetCallback());
//..
// Notice that previously installed callback was saved so we can restore it,
// if needed.
//
// Now, calls to 'bdetu_SystemTime' methods will use the method we installed.
// For example, we can check the time offset in New York for three dates
// of interest:
//..
    int offsetInSeconds;

    status = bdetu_SystemTime::loadLocalTimeOffset(&offsetInSeconds,
                                                   bdet_Datetime(2013,
                                                                    2,
                                                                   26));
    ASSERT(        0 == status);
    ASSERT(-5 * 3600 == offsetInSeconds);
    ASSERT(        1 == baetzo_LocalTimeOffsetUtil::updateCount());
    ASSERT(        0 == strcmp("America/New_York",
                                baetzo_LocalTimeOffsetUtil::timezone()));
    
    status = bdetu_SystemTime::loadLocalTimeOffset(&offsetInSeconds,
                                                   bdet_Datetime(2013,
                                                                    7,
                                                                    4));
    ASSERT(        0 == status);
    ASSERT(-4 * 3600 == offsetInSeconds);
    ASSERT(        2 == baetzo_LocalTimeOffsetUtil::updateCount());
    ASSERT(        0 == strcmp("America/New_York",
                                baetzo_LocalTimeOffsetUtil::timezone()));
   
    status = bdetu_SystemTime::loadLocalTimeOffset(&offsetInSeconds,
                                                   bdet_Datetime(2013,
                                                                   12,
                                                                   21));
    ASSERT(        0 == status);
    ASSERT(-5 * 3600 == offsetInSeconds);
    ASSERT(        3 == baetzo_LocalTimeOffsetUtil::updateCount());
    ASSERT(        0 == strcmp("America/New_York",
                                baetzo_LocalTimeOffsetUtil::timezone()));
//..
// Notice that the value returned by 'updateCount()' is unchanged by our first
// request, but incremented by the second and third request, which transitions
// into and then out of daylight saving time.  Also notice that the updates
// change the offset information but do not change the timezone.
//
// Finally, we restore the original local time callback.
//..
    previousCallback = bdetu_SystemTime::setLoadLocalTimeOffsetCallback(
                                                             previousCallback);
    ASSERT(previousCallback
        == &baetzo_LocalTimeOffsetUtil::loadLocalTimeOffset);
//..
}
}  // end usage example namespace

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;         g_verbose =         verbose;
    bool     veryVerbose = argc > 3;     g_veryVerbose =     veryVerbose;
    bool veryVeryVerbose = argc > 4; g_veryVeryVerbose = veryVeryVerbose;

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

        UsageExample1::main1();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'setTimezone', 'timezone', and 'localTimePeriod'
        //
        // Concerns:
        //: 1 TBD 
        //:   
        //
        // Plan:
        //: 1 TBD
        //
        // Testing:
        //   const baetzo_LocalTimePeriod& localTimePeriod();
        //   int setTimezone();
        //   int setTimezone(const char *timezone);
        //   int setTimezone(const char *timezone, const bdet_Datetime& utc);
        //   const char *timezone();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'setTimezone', 'timezone', and 'localTimePeriod'"
                          << "================================================"
                          << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Set the time zone to "America/New_York" for several dates in
        //:   succession, and examine the local time period information at each
        //:   stage.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        int status1 = Obj::setTimezone(NY);
        ASSERT(0 == status1);
 
        if (verbose) {
            cout << Obj::localTimePeriod() << endl;
        }

        bdet_Datetime startOfDst(2013, 3, 10, 7);
        int status2 = Obj::setTimezone(NY, startOfDst);
        ASSERT(0 == status2);

        if (verbose) {
            cout << Obj::localTimePeriod() << endl;
        }

        bdet_Datetime resumeOfStd(2013, 11, 3, 6);
        int status3 = Obj::setTimezone(NY, resumeOfStd);
        ASSERT(0 == status3);

        if (verbose) {
            cout << Obj::localTimePeriod() << endl;
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
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
