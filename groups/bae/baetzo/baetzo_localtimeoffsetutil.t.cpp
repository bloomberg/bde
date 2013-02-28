// baetzo_localtimeoffsetutil.t.cpp                                   -*-C++-*-
#include <baetzo_localtimeoffsetutil.h>

#include <bael_administration.h>
#include <bael_defaultobserver.h>
#include <bael_loggermanager.h> 
#include <bael_loggermanagerconfiguration.h> 
#include <bael_severity.h>

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
// The component under test is a utility; however, as the functions provided
// all serve to set or report on local time offset information (consisting of
// value semnatic types), the test strategy of this component more closely
// resembles that of a VST than of a typcial utility.  In particular, we will
// designate methods as "Primary Manipulators" and "Basic Accessors"
// validate them, and then use them in tests for other methods.
//
// Primary Manipulators:
//: o 'setTimezone' (the overlaod with two  parameters)
//
// Basic Accessors:
//: o 'timezone'
//: o 'localTimePeriod'
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ X] int loadLocalTimeOffset(int *result, const bdet_Datetime& utc);
// [ 2] const baetzo_LocalTimePeriod& localTimePeriod();
// [ 4] bdetu_SystemTime::LLTOC setLoadLocalTimeOffsetCallback();
// [ 3] int setTimezone();
// [ 3] int setTimezone(const char *timezone);
// [ 2] int setTimezone(const char *timezone, const bdet_Datetime& utc);
// [ 2] const char *timezone();
// [ X] int updateCount();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE
// [ X] CONCERN: This component uses the default global allocator.
// [ X] CONCERN: The static members have the expected initial values.
// [ X] CONCERN: The public methods of this component are *thread-safe*.

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

typedef baetzo_LocalTimeOffsetUtil Util;

const char *NY  = "America/New_York";
const char *BE  = "Europe/Berlin";
const char *RY  = "Asia/Riyadh";
const char *SA  = "Asia/Saigon";
const char *GMT = "Etc/GMT";
const char *GP1 = "Etc/GMT+1";
const char *GM1 = "Etc/GMT-1";

//=============================================================================
//                                 GLOBAL TEST DATA 
//-----------------------------------------------------------------------------

const char *DEFAULT_TZ_ARRAY[]   = { NY, BE, RY, SA, GMT, GP1, GM1 };
const int   DEFAULT_NUM_TZ_ARRAY = sizeof  DEFAULT_TZ_ARRAY
                                 / sizeof *DEFAULT_TZ_ARRAY;

const bdet_Datetime DEFAULT_DT_ARRAY[] = { bdet_Datetime(2012,  1,  1),
                                           bdet_Datetime(2013,  6, 20),
                                           bdet_Datetime(2014, 12, 31)
                                          };
const int DEFAULT_NUM_DT_ARRAY = sizeof  DEFAULT_DT_ARRAY
                               / sizeof *DEFAULT_DT_ARRAY;

// ============================================================================
//                  GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

struct LogVerbosityGuard {
    // The Logger verbosity guard disables logging on construction, and
    // re-enables logging, based on the prior default pass-through level, when
    // it goes out of scope and is destroyed.  It is intended to suppress
    // logged output for intentional errors when the test driver is run in
    // non-verbose mode.

    bool d_verbose;             // verbose mode does not disable logging
    int  d_defaultPassthrough;  // default passthrough log level

    LogVerbosityGuard(bool verbose = false)
        // If the specified 'verbose' is 'false' disable logging util this
        // guard is destroyed.
    {
        d_verbose = verbose;
        if (!d_verbose) {
            d_defaultPassthrough =
                   bael_LoggerManager::singleton().defaultPassThresholdLevel();

            bael_Administration::setDefaultThresholdLevels(
                                              bael_Severity::BAEL_OFF,
                                              bael_Severity::BAEL_OFF,
                                              bael_Severity::BAEL_OFF,
                                              bael_Severity::BAEL_OFF);
            bael_Administration::setThresholdLevels(
                                              "*",
                                              bael_Severity::BAEL_OFF,
                                              bael_Severity::BAEL_OFF,
                                              bael_Severity::BAEL_OFF,
                                              bael_Severity::BAEL_OFF);

        }
    }

   ~LogVerbosityGuard()
        // Set the logging verbosity back to its default state.
    {
        if (!d_verbose) {
            bael_Administration::setDefaultThresholdLevels(
                                              bael_Severity::BAEL_OFF,
                                              d_defaultPassthrough,
                                              bael_Severity::BAEL_OFF,
                                              bael_Severity::BAEL_OFF);
            bael_Administration::setThresholdLevels(
                                              "*",
                                              bael_Severity::BAEL_OFF,
                                              d_defaultPassthrough,
                                              bael_Severity::BAEL_OFF,
                                              bael_Severity::BAEL_OFF);
        }
    }
};

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

    bael_DefaultObserver            observer(&bsl::cout);
    bael_LoggerManagerConfiguration configuration;
    bael_LoggerManager&             manager =
                   bael_LoggerManager::initSingleton(&observer, configuration);

    switch (test) { case 0:
      case 7: {
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
      case 6: {
        // --------------------------------------------------------------------
        // CALLBACK FUNCTION
        //
        // Concerns:
        //: 1 The callback function loads the correct offset and returns 0 for
        //:   arbitrary Zoneinfo timezones for arbitrary UTC datetimes.
        //:
        //: 2 For a given timezone, the callback function updates the cached
        //:   local time offset information on transitions into and out of
        //:   daylight saving time.
        //
        // Plan:
        //: 1 Using the array-driven approach, compare the results of calling
        //:   to those obtained from an independent source.
        //:
        //: 2 Using the array-driven approach, for a fixed timezone, invoke
        //:   the call back method on both sides of the boundaries of daylight
        //:   savings time for a year.  Confirm that the expected values are
        //:   returned throughout, and that the static members are updated when
        //:   times transition into and out of daylight saving time.  Also 
        //:   confirm the same behavior when the series of daytetimes are used
        //:   in reverse order.
        //
        // Testing:
        //  int loadLocalTimeOffset(int *result, const bdet_Datetime& utc);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CALLBACK FUNCTION" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nCheck for arbirary timezones and UTCs" << endl;
        {
            for (int i = 0; i < DEFAULT_NUM_TZ_ARRAY; ++i) {
                const char *TIMEZONE = DEFAULT_TZ_ARRAY[i];
    
                if (veryVerbose) { P(TIMEZONE) }
    
                for (int j = 0; j < DEFAULT_NUM_DT_ARRAY; ++j) {
                    const bdet_Datetime& UTC = DEFAULT_DT_ARRAY[j];
                    int                  status;
    
                    if (veryVerbose) { T_ P(UTC) }

                    baetzo_LocalTimePeriod expected;
                    status = baetzo_TimeZoneUtil::loadLocalTimePeriodForUtc(
                                                                    &expected,
                                                                    TIMEZONE,
                                                                    UTC);
                    ASSERT(0 == status);
    
                    status = Util::setTimezone(TIMEZONE, UTC);
                    ASSERT(0 == status);

                    int observedOffset;
                    status = Util::loadLocalTimeOffset(&observedOffset, UTC);
                    ASSERT(0 == status);
                    ASSERT(expected.descriptor().utcOffsetInSeconds()
                        == observedOffset);
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // STATIC INITIALIZATION
        //
        // Concerns:
        //: 1 The static members of 'baetzo_LocalTimeOffsetUtil' are
        //:   statically initialized to their expected default values.
        //:
        //: 2 The static members of 'baetzo_LocalTimeOffsetUtil' are
        //:   use the global default allocator.
        //:
        // Plan:
        //: 1 Before any other use of 'baetzo_LocalTimeOffsetUtil', compare
        //:   the values of the static members to their expected values.  (C-1)
        //:
        //: 2 Before any other use of 'baetzo_LocalTimeOffsetUtil', examine the
        //:   allocator used by those static members that take allocators.
        //:   (C-2)
        //
        // Testing:
        //  CONCERN: This component uses the default global allocator.
        //  CONCERN: The static members have the expected initial values.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "STATIC INITIALIZATION" << endl
                          << "=====================" << endl;

        ASSERT(0                        == Util::timezone());
        ASSERT(baetzo_LocalTimePeriod() == Util::localTimePeriod());
        ASSERT(bslma::Default::globalAllocator() 
            == Util::localTimePeriod().allocator());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // SETTING THE LOCAL TIME OFFSET CALLBACK
        //
        // Concerns:
        //: 1 The 'setLoadLocalTimeOffsetCallback' method sets
        //:   the 'loadLocalTimeOffset' method as the current local time offset
        //:   callback used by 'bdetu_SystemTime'.
        //:
        //: 2 The 'setLoadLocalTimeOffsetCallback' method returns the
        //:   installed previously callback.
        //
        // Plan:
        //: 1 Use the 'bdetu_SystemTime::currentLocalTimeOffsetCallback' method
        //:   to check the current local time offset callback before and after
        //:   calls to 'setLoadLocalTimeOffsetCallback'.  (C-1)
        //:
        //: 2 Compare the value returned by the initial call to the
        //:   'bdetu_SystemTime::currentLocalTimeOffsetCallback' method with
        //:   that returned by the 'setLoadLocalTimeOffsetCallback' method.
        //:   (C-2)
        //
        // Testing:
        //   bdetu_SystemTime::LLTOC setLoadLocalTimeOffsetCallback();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SETTING THE LOCAL TIME OFFSET CALLBACK" << endl
                          << "======================================" << endl;

        const bdetu_SystemTime::LoadLocalTimeOffsetCallback initialCallback =
                        bdetu_SystemTime::currentLoadLocalTimeOffsetCallback();
        ASSERT(&Util::loadLocalTimeOffset != initialCallback);


        const bdetu_SystemTime::LoadLocalTimeOffsetCallback previousCallback =
                                        Util::setLoadLocalTimeOffsetCallback();
        ASSERT(initialCallback == previousCallback);
        ASSERT(&Util::loadLocalTimeOffset
            == bdetu_SystemTime::currentLoadLocalTimeOffsetCallback());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // OTHER MANIPULATORS
        //
        // Concerns:
        //: 1 The other (non-primary) manipulators apply their expected
        //:   default values.
        //:
        //: 2 The manipulators return a non-zero value when an invalid
        //:   timezone is specified, and (QoI) there is no change in the static
        //:   members.
        //
        // Plan:
        //: 1 Invoke the non-primary manipulators and compare the results with
        //:   obtained from the primary manipulator using the expected default
        //:   values.  (C-1)
        //:
        //:   o One default value is the current UTC time which, of course,
        //:     changes between the invocation of the two manipulators;
        //:     however, this is only a problem in the unlikely event that the
        //:     local time zone information changes in that short interval.
        //:
        //: 2 Save the values of the static members and call each 'setTime'
        //:   method with an unknown timezone.  After each 'setTime' call,
        //:   check the return value, and compare the current values of the
        //:   static members to the saved values.  (C-2)
        //
        // Testing:
        //   int setTimezone();
        //   int setTimezone(const char *timezone);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "OTHER MANIPULATORS" << endl
                          << "==================" << endl;

        if (verbose) cout << "\nTesting 'setTimezone(const char *timezone)'"
                          << endl;
        {
            for (int i = 0; i < DEFAULT_NUM_TZ_ARRAY; ++i) {
                const char *TIMEZONE = DEFAULT_TZ_ARRAY[i];

                if (veryVerbose) { P(TIMEZONE) }

                int status = Util::setTimezone(TIMEZONE);
                ASSERT(0 == status);
                ASSERT(0 == strcmp(TIMEZONE, Util::timezone()));

                // Race: utcDatetime is changing.

                const baetzo_LocalTimePeriod observedLocalTimePeriod(
                                                      Util::localTimePeriod());

                bdet_Datetime now = bdetu_SystemTime::nowAsDatetimeUtc();
                status = Util::setTimezone(TIMEZONE, now);
                ASSERT(0 == status);
                ASSERT(Util::localTimePeriod() == observedLocalTimePeriod);
            }
        }

        if (verbose) cout << "\nTesting 'setTimezone()' for valid timezones"
                          << endl;
        {
            static char tzEquals[] = "TZ=";

            for (int i = 0; i < DEFAULT_NUM_TZ_ARRAY; ++i) {
                const char *TIMEZONE = DEFAULT_TZ_ARRAY[i];

                if (veryVerbose) { P(TIMEZONE) }

                char buffer[80];
                ASSERT(sizeof(buffer) >
                                  sizeof(tzEquals) -1 + strlen(TIMEZONE) + 1);
                snprintf(buffer, sizeof(buffer), "%s%s", tzEquals, TIMEZONE);

                if (veryVeryVerbose) { T_ P(buffer) }

                int status = putenv(buffer);
                ASSERT(0 == status);
                ASSERT(0 == strcmp(TIMEZONE, getenv("TZ")));

                status = Util::setTimezone();
                ASSERT(0 == status);
                ASSERT(0 == strcmp(TIMEZONE, Util::timezone()));

                // Race: The UTC datetime  is changing.

                const baetzo_LocalTimePeriod observedLocalTimePeriod(
                                                      Util::localTimePeriod());

                bdet_Datetime now = bdetu_SystemTime::nowAsDatetimeUtc();
                status = Util::setTimezone(TIMEZONE, now);
                ASSERT(0 == status);
                ASSERT(Util::localTimePeriod() == observedLocalTimePeriod);
            }
        }

        if (verbose) cout << "\nTesting Manipulators with invalid timezone"
                          << endl;
        {
            int         status;
            const char *invalidTimezone =    "FOO";
            char buffer[]               = "TZ=FOO";

            const char *timezone0                               =
                                                              Util::timezone();
            const       baetzo_LocalTimePeriod localTimePeriod0 =
                                                       Util::localTimePeriod();

            if (veryVeryVerbose) { T_ P(invalidTimezone) }

            {
                LogVerbosityGuard guard;
                status = Util::setTimezone(invalidTimezone);
            }

            ASSERT(0                != status);
            ASSERT(timezone0        == Util::timezone());
            ASSERT(localTimePeriod0 == Util::localTimePeriod());


            if (veryVeryVerbose) { T_ P(buffer) }

            status = putenv(buffer);
            ASSERT(0 == status);
            ASSERT(0 == strcmp(invalidTimezone, getenv("TZ")));

            {
                LogVerbosityGuard guard;
                status = Util::setTimezone();
            }

            ASSERT(0                != status);
            ASSERT(timezone0        == Util::timezone());
            ASSERT(localTimePeriod0 == Util::localTimePeriod());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATOR and BASIC ACCESSORS
        //
        // Concerns:
        //: 1 The primary manipulator can set the cached local time offset
        //:   information for any entry in the Zoneinfo database for arbitary
        //:   UTC datetime.
        //:
        //: 2 The basic accessors report the currently cached local time offset
        //:   information.
        //:
        //: 3 The primary manipulator returns a non-zero value when an invalid
        //:   timezone is specified, and (QoI) there is no change in the static
        //:   members.
        //
        // Plan:
        //: 1 Using the array-driven approach, generate test cases for several
        //:   entries representing several categories of Zoneinfo entries
        //:   (i.e., timezone before and after "Etc/GMT" and "Etc/GMT" itself)
        //:   and for several categories of UTC datetimes (i.e., before,
        //:   during, and after daylight saving time, if defined in a
        //:   timezone).  For each entry confirm that 'setTimezone' returns
        //:   successfully, and use the accessors to confirm that the cached
        //:   information matches the expected results.  The expected results
        //:   for the local time period is obtained using the
        //:  'baetzo_TimeZoneUtil::loadLocalTimePeriodForUtc' method.  (C-1..2)
        //:
        //: 2 Save the values of the static members, call 'setTime' with an
        //:   unknown timezone, check the return value, and compare the current
        //:   values of the static members to the saved values.  
        //
        // Testing:
        //   const baetzo_LocalTimePeriod& localTimePeriod();
        //   int setTimezone(const char *timezone, const bdet_Datetime& utc);
        //   const char *timezone();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATOR and BASIC ACCESSORS" << endl
                          << "=======================================" << endl;

        if (verbose) cout << "\nCheck valid timezones" << endl;
        {
            for (int i = 0; i < DEFAULT_NUM_TZ_ARRAY; ++i) {
                const char *TIMEZONE = DEFAULT_TZ_ARRAY[i];
    
                if (veryVerbose) { P(TIMEZONE) }
    
                for (int j = 0; j < DEFAULT_NUM_DT_ARRAY; ++j) {
                    const bdet_Datetime& UTC = DEFAULT_DT_ARRAY[j];
    
                    if (veryVerbose) { T_ P(UTC) }
    
                    int status = Util::setTimezone(TIMEZONE, UTC);
                    ASSERT(0 == status);
                    ASSERT(0 == strcmp(TIMEZONE, Util::timezone()));
    
    
                    baetzo_LocalTimePeriod expectedLocalTimePeriod;
    
                    status = baetzo_TimeZoneUtil::loadLocalTimePeriodForUtc(
                                                      &expectedLocalTimePeriod,
                                                      TIMEZONE,
                                                      UTC);
                    ASSERT(0 == status);
                    ASSERT(expectedLocalTimePeriod == Util::localTimePeriod());
                }
            }
        }

        if (verbose) cout << "\nCheck invalid timezone" << endl;
        {
            const char *timezone0                               =
                                                              Util::timezone();
            const       baetzo_LocalTimePeriod localTimePeriod0 =
                                                       Util::localTimePeriod();

            int status;

            {
                LogVerbosityGuard guard;
                status = Util::setTimezone("hello, world", bdet_Datetime(
                                                                        2013,
                                                                           2,
                                                                          28));
            }
            ASSERT(0                != status);
            ASSERT(timezone0        == Util::timezone());
            ASSERT(localTimePeriod0 == Util::localTimePeriod());
        }
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

        int status1 = Util::setTimezone(NY);
        ASSERT(0 == status1);

        if (verbose) {
            cout << Util::localTimePeriod() << endl;
        }

        bdet_Datetime startOfDst(2013, 3, 10, 7);
        int status2 = Util::setTimezone(NY, startOfDst);
        ASSERT(0 == status2);

        if (verbose) {
            cout << Util::localTimePeriod() << endl;
        }

        bdet_Datetime resumeOfStd(2013, 11, 3, 6);
        int status3 = Util::setTimezone(NY, resumeOfStd);
        ASSERT(0 == status3);

        if (verbose) {
            cout << Util::localTimePeriod() << endl;
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
