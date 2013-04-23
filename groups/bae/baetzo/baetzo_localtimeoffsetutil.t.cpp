// baetzo_localtimeoffsetutil.t.cpp                                   -*-C++-*-
#include <baetzo_localtimeoffsetutil.h>

#include <baetzo_defaultzoneinfocache.h>  // for testing only
#include <baetzo_testloader.h>            // for testing only
#include <baetzo_zoneinfocache.h>         // for testing only
#include <baetzo_zoneinfoutil.h>          // for testing only

#include <bael_administration.h>
#include <bael_defaultobserver.h>
#include <bael_loggermanager.h>
#include <bael_loggermanagerconfiguration.h>
#include <bael_severity.h>

#include <bcemt_barrier.h>       // case -1
#include <bcemt_configuration.h> // case -1
#include <bcemt_threadutil.h>    // case -1
#include <bdepu_iso8601.h>                // case 5
#include <bslma_defaultallocatorguard.h>  // case 5
#include <bslma_testallocator.h>          // case 5

#include <bsl_cstring.h>         // 'strcmp'
#include <bsl_iostream.h>
#include <bsls_asserttest.h>
#include <bsls_stopwatch.h>      // case -2, -3
#include <bsls_types.h>
#include <bsl_vector.h>          // case -1

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif

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
// resembles that of a VST than of a typical utility.  In particular, we will
// informally categorize certain methods as "Primary Manipulators" and "Basic
// Accessors" validate them, and then use them in tests for other methods.
//
// Primary Manipulators:
//: o 'configure' (the overload with two parameters)
//
// Basic Accessors:
//: o 'loadTimezone'
//: o 'loadLocalTimePeriod'
//
// ----------------------------------------------------------------------------
// CLASS METHODS
                        // *** local time offset methods ***
// [ 6] int loadLocalTimeOffset(int *result, const bdet_Datetime& utc);
// [ 4] bdetu_SystemTime::LLTOC setLoadLocalTimeOffsetCallback();

                        // *** configure methods ***
// [ 3] int configure();
// [ 3] int configure(const char *timezone);
// [ 2] int configure(const char *timezone, const bdet_Datetime& utc);

                        // *** accessor methods ***
// [ 2] void loadLocalTimePeriod(baetzo_LocalTimePeriod *localTimePeriod);
// [ 2] bsl::string loadTimezone(bsl::string *timezone);
// [ 6] int updateCount();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ 2] BOOTSTRAP1: int updateCount();
// [ 3] BOOTSTRAP2: int updateCount();
// [ 5] CONCERN: This component uses the default global allocator.
// [ 5] CONCERN: The static members have the expected initial values.
// [-1] CONCERN: The public methods of this component are *thread-safe*.
// [-2] CONCERN: Performance on cache hits.
// [-3] CONCERN: Performance on cache misses.

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

bool             g_verbose;
bool         g_veryVerbose;
bool     g_veryVeryVerbose;
bool g_veryVeryVeryVerbose;

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

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

const char *const LONG_STRING    = "a_"   SUFFICIENTLY_LONG_STRING;
const char *const LONGER_STRING  = "ab_"  SUFFICIENTLY_LONG_STRING;
const char *const LONGEST_STRING = "abc_" SUFFICIENTLY_LONG_STRING;

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

    explicit LogVerbosityGuard(bool verbose = false)
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

// ============================================================================
//                                 HELPER FUNCTIONS
// ----------------------------------------------------------------------------

struct ThreadArg {
    int            d_offset;
    bdet_Datetime  d_utcDatetime;
    bcemt_Barrier *d_barrier_p;
};

typedef bsl::vector<struct ThreadArg> ThreadArgs;

extern "C" void *workerThread(void *arg)
{
    ThreadArg *p = static_cast<ThreadArg *>(arg);

    p->d_barrier_p->wait();

    baetzo_LocalTimeOffsetUtil::loadLocalTimeOffset(&p->d_offset,
                                                    p->d_utcDatetime);
    return 0;
}

typedef bsl::vector<bcemt_ThreadUtil::Handle> Handles;

bdetu_Epoch::TimeT64 toTimeT(const bdet_Datetime& value)
    // Return the interval in seconds from UNIX epoch time of the specified
    // 'value'.  Note that this method is shorthand for
    // 'bdetu_Epoch::convertToTimeT64'.
{
    return bdetu_Epoch::convertToTimeT64(value);
}

bdet_Datetime toDatetime(const char *iso8601TimeString)
    // Return the datetime value indicated by the specified
    // 'iso8601TimeString'.  The behavior is undefined unless
    // 'iso8601TimeString' is a null-terminated C-string containing a time
    // description matching the iso8601 specification (see 'bdepu_iso8601').
{
    bdet_Datetime time;
    int rc = bdepu_Iso8601::parse(&time,
                                  iso8601TimeString,
                                  bsl::strlen(iso8601TimeString));
    BSLS_ASSERT(0 == rc);
    return time;
}

struct TransitionDescription {
    // A 'struct' describing a transitions.  Note that this type is meant to
    // be used to create data tables for use with 'addTransitions'.

    int         d_line;
    const char *d_transitionTime;
    int         d_offsetMins;
    const char *d_abbrev;
    bool        d_isDst;
};

void addTransitions(baetzo_Zoneinfo             *result,
                    const TransitionDescription *descriptions,
                    int                          numDescriptions)
    // Insert to the specified 'result' the contiguous sequence of specified
    // 'descriptions', of length 'numDescriptions'.
{
    BSLS_ASSERT(result);

    for (int i = 0; i < numDescriptions; ++i) {
        const char *TRANS = descriptions[i].d_transitionTime;
        baetzo_LocalTimeDescriptor desc(descriptions[i].d_offsetMins * 60,
                                        descriptions[i].d_isDst,
                                        descriptions[i].d_abbrev);
        result->addTransition(toTimeT(toDatetime(TRANS)), desc);
    }
}

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

    int status = baetzo_LocalTimeOffsetUtil::configure("America/New_York",
                                                        bdet_Datetime(2013,
                                                                         2,
                                                                        26));
    ASSERT(0 == status);
    ASSERT(1 == baetzo_LocalTimeOffsetUtil::updateCount());

    bsl::string timezone;

    baetzo_LocalTimeOffsetUtil::loadTimezone(&timezone);
    ASSERT(0 == strcmp("America/New_York", timezone.c_str()));
//..
// Notice that the value returned by the 'updateCount' method is increased by
// one after then time zone information has been set.
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
// Notice that previously installed callback was saved so we can restore it, if
// needed.
//
// Now, calls to 'bdetu_SystemTime' methods will use the method we installed.
// For example, we can check the time offset in New York for three dates of
// interest:
//..
    int offsetInSeconds;

    bdetu_SystemTime::loadLocalTimeOffset(&offsetInSeconds,
                                          bdet_Datetime(2013,  2, 26));
    ASSERT(        0 == status);
    ASSERT(-5 * 3600 == offsetInSeconds);
    ASSERT(        1 == baetzo_LocalTimeOffsetUtil::updateCount());

    baetzo_LocalTimeOffsetUtil::loadTimezone(&timezone);
    ASSERT(        0 == strcmp("America/New_York", timezone.c_str()));

    bdetu_SystemTime::loadLocalTimeOffset(&offsetInSeconds,
                                          bdet_Datetime(2013,  7,  4));
    ASSERT(-4 * 3600 == offsetInSeconds);
    ASSERT(        2 == baetzo_LocalTimeOffsetUtil::updateCount());
    baetzo_LocalTimeOffsetUtil::loadTimezone(&timezone);
    ASSERT(        0 == strcmp("America/New_York", timezone.c_str()));

    bdetu_SystemTime::loadLocalTimeOffset(&offsetInSeconds,
                                          bdet_Datetime(2013, 12, 21));
    ASSERT(-5 * 3600 == offsetInSeconds);
    ASSERT(        3 == baetzo_LocalTimeOffsetUtil::updateCount());
    baetzo_LocalTimeOffsetUtil::loadTimezone(&timezone);
    ASSERT(        0 == strcmp("America/New_York", timezone.c_str()));
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
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

                g_verbose =             verbose;
            g_veryVerbose =         veryVerbose;
        g_veryVeryVerbose =     veryVeryVerbose;
    g_veryVeryVeryVerbose = veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bael_DefaultObserver            observer(&bsl::cout);
    bael_LoggerManagerConfiguration configuration;
    bael_LoggerManager&             manager =
                   bael_LoggerManager::initSingleton(&observer, configuration);
    (void)manager;

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
        //:
        //: 3 The value returned by the 'updateCall' method increases by one
        //:   for each transition into and out of daylight saving time, and for
        //:   no other invocations of the callback.
        //
        // Plan:
        //: 1 Using the array-driven approach, compare the results of calling
        //:   to those obtained from an independent source.  (C-1)
        //:
        //: 2 Using the array-driven approach, for a fixed timezone, invoke the
        //:   call back method on both sides of the boundaries of daylight
        //:   savings time for a year.  Confirm that the expected values are
        //:   returned throughout, and that the static members are updated when
        //:   times transition into and out of daylight saving time.  Also
        //:   confirm the same behavior when the series of datetimes are used
        //:   in reverse order.  (C-2..3)
        //
        // Testing:
        //  int loadLocalTimeOffset(int *result, const bdet_Datetime& utc);
        //  int updateCount();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CALLBACK FUNCTION" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nCheck arbitrary timezones and UTCs" << endl;
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

                    status = Util::configure(TIMEZONE, UTC);
                    ASSERT(0 == status);

                    int reportedOffset;
                    Util::loadLocalTimeOffset(&reportedOffset, UTC);
                    ASSERT(expected.descriptor().utcOffsetInSeconds()
                        == reportedOffset);
                }
            }
        }

        if (verbose) cout << "\nCheck daylight savings transitions" << endl;
        {
            bdet_Datetime newYearsDay(2013,  1,  1, 0);
            int status = Util::configure("America/New_York", newYearsDay);
            ASSERT(0 == status);

            bdet_Datetime  startOfDst(2013,  3, 10, 7);
            bdet_Datetime resumeOfStd(2013, 11,  3, 6);

            bdet_Datetime startOfDstMinus(startOfDst);
                          startOfDstMinus.addMilliseconds(-1);

            bdet_Datetime startOfDstPlus(startOfDst);
                          startOfDstPlus.addMilliseconds(1);

            bdet_Datetime resumeOfStdMinus(resumeOfStd);
                          resumeOfStdMinus.addMilliseconds(-1);

            bdet_Datetime resumeOfStdPlus(resumeOfStd);
                          resumeOfStdPlus.addMilliseconds(1);

            baetzo_LocalTimePeriod priorLocalTimePeriod;
            Util::loadLocalTimePeriod(&priorLocalTimePeriod);
            int                    priorUpdateCount     = Util::updateCount();
            bsl::string            priorTimezone;
            Util::loadTimezone(&priorTimezone);

            const struct {
                int           d_line;
                bdet_Datetime d_utcDatetime;
                int           d_expectedOffset;
            } DATA[] = {

                //LINE UTC_DATETIME      EXP. OFFSET
                //---- ----------------  -----------
                { L_,  startOfDstMinus,    -5 * 3600 },
                { L_,  startOfDst,         -4 * 3600 },
                { L_,  startOfDstPlus,     -4 * 3600 },

                { L_,  resumeOfStdMinus,   -4 * 3600 },
                { L_,  resumeOfStd,        -5 * 3600 },
                { L_,  resumeOfStdPlus,    -5 * 3600 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE         = DATA[ti].d_line;
                const bdet_Datetime& UTC_DATETIME = DATA[ti].d_utcDatetime;
                const int            EXP_OFFSET   = DATA[ti].d_expectedOffset;

                if (veryVeryVerbose) { T_ P(ti) }
                if (veryVerbose) { T_ P_(LINE) P_(UTC_DATETIME) P(EXP_OFFSET) }

                int reportedOffset;
                Util::loadLocalTimeOffset(&reportedOffset, UTC_DATETIME);
                ASSERT(EXP_OFFSET == reportedOffset);

                bsl::string timezone; Util::loadTimezone(&timezone);
                LOOP_ASSERT(LINE, priorTimezone == timezone); // invariant

                if (ti > 0 && DATA[ti - 1].d_expectedOffset != EXP_OFFSET) {
                    baetzo_LocalTimePeriod localTimePeriod;
                    Util::loadLocalTimePeriod(&localTimePeriod);
                    LOOP_ASSERT(LINE, priorLocalTimePeriod != localTimePeriod);
                    LOOP_ASSERT(LINE, priorUpdateCount + 1 ==
                                                          Util::updateCount());

                } else {
                    baetzo_LocalTimePeriod localTimePeriod;
                    Util::loadLocalTimePeriod(&localTimePeriod);
                    LOOP_ASSERT(LINE, priorLocalTimePeriod == localTimePeriod);
                    LOOP_ASSERT(LINE, priorUpdateCount     ==
                                                          Util::updateCount());
                }

                priorUpdateCount = Util::updateCount();
                Util::loadTimezone(&priorTimezone);
                Util::loadLocalTimePeriod(&priorLocalTimePeriod);
            }

            ASSERT(0 <= NUM_DATA - 1);

            for (int ti = NUM_DATA - 1; 0 <= ti; --ti) {
                const int            LINE         = DATA[ti].d_line;
                const bdet_Datetime& UTC_DATETIME = DATA[ti].d_utcDatetime;
                const int            EXP_OFFSET   = DATA[ti].d_expectedOffset;

                if (veryVeryVerbose) { T_ P(ti) }
                if (veryVerbose) { T_ P_(LINE) P_(UTC_DATETIME) P(EXP_OFFSET) }

                int reportedOffset;
                Util::loadLocalTimeOffset(&reportedOffset, UTC_DATETIME);
                ASSERT(EXP_OFFSET == reportedOffset);

                bsl::string timezone; Util::loadTimezone(&timezone);
                LOOP_ASSERT(LINE, priorTimezone == timezone); // invariant

                if (ti < NUM_DATA - 1
                 && DATA[ti + 1].d_expectedOffset != EXP_OFFSET) {
                    baetzo_LocalTimePeriod localTimePeriod;
                    Util::loadLocalTimePeriod(&localTimePeriod);
                    LOOP_ASSERT(LINE, priorLocalTimePeriod != localTimePeriod);
                    LOOP_ASSERT(LINE, priorUpdateCount + 1 ==
                                                          Util::updateCount());

                } else {
                    baetzo_LocalTimePeriod localTimePeriod;
                    Util::loadLocalTimePeriod(&localTimePeriod);
                    LOOP_ASSERT(LINE, priorLocalTimePeriod == localTimePeriod);
                    LOOP_ASSERT(LINE, priorUpdateCount     ==
                                                          Util::updateCount());
                }

                priorUpdateCount = Util::updateCount();
                Util::loadTimezone(&priorTimezone);
                Util::loadLocalTimePeriod(&priorLocalTimePeriod);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // STATIC INITIALIZATION
        //
        // Concerns:
        //: 1 The 'updateCount' method returns 0 before configuration.
        //:
        //: 2 The static members of 'baetzo_LocalTimeOffsetUtil' that take
        //:   allocators use the default global allocator.
        //:
        // Plan:
        //: 1 Before any other use of 'baetzo_LocalTimeOffsetUtil', check the
        //:   value returned by the 'updateCount' method.
        //:
        //: 2 Install the test allocators as the default and global allocators.
        //:   Since the memory for the static members will be deleted after the
        //:   return of 'main', we use a statically initialized allocator for
        //:   the global allocator which will outlive the static members.
        //:
        //: 3 Using 'baetzo_TestLoader' and several helper functions, define a
        //:   time zone with a description and a timezone name sufficiently
        //:   long as to require memory allocation from 'bsl::string' (which
        //:   has a short string optimization).  Construct a
        //:   'baetzo_ZoneinfoCache' using the configured test loader and
        //:   install that cache as the default timezone cache.
        //:
        //: 4 Configure our mechanism to use the custom-made timezone.  Check
        //:   the block count of each allocator before and after configuration
        //:   and compare to the expected values.
        //
        // Testing:
        //  CONCERN: This component uses the global allocator.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "STATIC INITIALIZATION" << endl
                          << "=====================" << endl;

        if (verbose) cout << "\nCheck Initial Values" << endl;
        {
            ASSERT(0 == Util::updateCount());
        }

        if (verbose) cout << "\nCheck For Global Allocators" << endl;
        {
            const char LONG_DESCRIPTION[] = "Description: "
                                                      SUFFICIENTLY_LONG_STRING;
            const char LONG_TIMEZONE[]    = "Timezone: "
                                                      SUFFICIENTLY_LONG_STRING;

            const TransitionDescription TZ_DATA[] = {
              { L_, "0001-01-01T00:00:00.000",     0, "A",              false},
              { L_, "2013-01-01T12:00:00.000",  1439, LONG_DESCRIPTION, false},
              { L_, "9999-12-31T23:59:59.000",     0, "B",              false}
             };
            const int NUM_TZ_DATA = sizeof TZ_DATA / sizeof *TZ_DATA;

            baetzo_Zoneinfo testZoneInfo;
            addTransitions(&testZoneInfo, TZ_DATA, NUM_TZ_DATA);
            testZoneInfo.setIdentifier(LONG_TIMEZONE);
            ASSERT(baetzo_ZoneinfoUtil::isWellFormed(testZoneInfo));

            baetzo_TestLoader testLoader;
            testLoader.setTimeZone(testZoneInfo);

            baetzo_ZoneinfoCache testCache(&testLoader);
            baetzo_DefaultZoneinfoCache::setDefaultCache(&testCache);

            static bslma::TestAllocator staticGlobalAllocator(
                                                          "global",
                                                          veryVeryVeryVerbose);

            bslma::Default::setGlobalAllocator(&staticGlobalAllocator);

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::Default::setDefaultAllocator(&da);

            ASSERT(0 == staticGlobalAllocator.numBlocksInUse());
            ASSERT(0 == da.numBlocksInUse());

            int status = Util::configure(LONG_TIMEZONE,
                                         bdet_Datetime(2013, 1, 1, 12));

            ASSERT(2 == staticGlobalAllocator.numBlocksInUse());
            ASSERT(0 == da.numBlocksInUse());
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // SETTING THE LOCAL TIME OFFSET CALLBACK
        //
        // Concerns:
        //: 1 The 'setLoadLocalTimeOffsetCallback' method sets the
        //:   'loadLocalTimeOffset' method as the current local time offset
        //:   callback used by 'bdetu_SystemTime'.
        //:
        //: 2 The 'setLoadLocalTimeOffsetCallback' method returns the installed
        //:   previously callback.
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
        //: 1 The other (non-primary) manipulators apply their expected default
        //:   values.
        //:
        //: 2 The manipulators return a non-zero value when an invalid timezone
        //:   is specified, and (QoI) there is no change in the static members.
        //:
        //: 3 The value returned by the 'updateCall' method increases by one
        //:   for each successful call of a 'configure' method, and remains the
        //:   same after unsuccessful calls.
        //:
        //: 4 The value returned by the 'updateCall' method increases by one
        //:   for each successful call of a 'configure' method, and remains the
        //:   same after unsuccessful calls.
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
        //: 2 Save the values of the static members and call each 'configure'
        //:   method with an unknown timezone.  After each 'configure' call,
        //:   check the return value, and compare the current values of the
        //:   static members to the saved values.  (C-2..4)
        //
        // Testing:
        //   int configure();
        //   int configure(const char *timezone);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "OTHER MANIPULATORS" << endl
                          << "==================" << endl;

        int      updateCount = Util::updateCount();
        int priorUpdateCount = Util::updateCount();
        ASSERT(0 ==      updateCount);
        ASSERT(0 == priorUpdateCount);

        if (verbose) cout << "\nTesting 'configure(const char *timezone)'"
                          << endl;
        {
            for (int i = 0; i < DEFAULT_NUM_TZ_ARRAY; ++i) {
                const char *TIMEZONE = DEFAULT_TZ_ARRAY[i];

                if (veryVerbose) { P(TIMEZONE) }

                int status = Util::configure(TIMEZONE);
                ASSERT(0 == status);
                bsl::string timezone; Util::loadTimezone(&timezone);
                ASSERT(0 == strcmp(TIMEZONE, timezone.c_str()));

                updateCount = Util::updateCount();
                ASSERT(priorUpdateCount + 1 == updateCount);
                priorUpdateCount = updateCount;

                // Race: utcDatetime is changing.

                baetzo_LocalTimePeriod observedLocalTimePeriod;
                Util::loadLocalTimePeriod(&observedLocalTimePeriod);

                bdet_Datetime now = bdetu_SystemTime::nowAsDatetimeUtc();
                status = Util::configure(TIMEZONE, now);
                ASSERT(0 == status);
                baetzo_LocalTimePeriod localTimePeriod;
                Util::loadLocalTimePeriod(&localTimePeriod);
                ASSERT(localTimePeriod == observedLocalTimePeriod);

                updateCount = Util::updateCount();
                ASSERT(priorUpdateCount + 1 == updateCount);
                priorUpdateCount = updateCount;
            }
        }

        if (verbose) cout << "\nTesting 'configure()' for valid timezones"
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

                status = Util::configure();

                ASSERT(0 == status);
                bsl::string timezone; Util::loadTimezone(&timezone);
                ASSERT(0 == strcmp(TIMEZONE, timezone.c_str()));

                updateCount = Util::updateCount();
                ASSERT(priorUpdateCount + 1 == updateCount);
                priorUpdateCount = updateCount;

                // Race: The UTC datetime  is changing.

                baetzo_LocalTimePeriod observedLocalTimePeriod;
                Util::loadLocalTimePeriod(&observedLocalTimePeriod);

                bdet_Datetime now = bdetu_SystemTime::nowAsDatetimeUtc();
                status = Util::configure(TIMEZONE, now);
                ASSERT(0 == status);
                baetzo_LocalTimePeriod localTimePeriod;
                Util::loadLocalTimePeriod(&localTimePeriod);
                ASSERT(localTimePeriod == observedLocalTimePeriod);

                updateCount = Util::updateCount();
                ASSERT(priorUpdateCount + 1 == updateCount);
                priorUpdateCount = updateCount;
            }
        }

        if (verbose) cout << "\nTesting Manipulators with invalid timezone"
                          << endl;
        {
            int         status;
            const char *invalidTimezone =    "FOO";
            char buffer[]               = "TZ=FOO";

            bsl::string timezone0;
            Util::loadTimezone(&timezone0);
            baetzo_LocalTimePeriod localTimePeriod0;
            Util::loadLocalTimePeriod(&localTimePeriod0);

            if (veryVeryVerbose) { T_ P(invalidTimezone) }

            {
                LogVerbosityGuard guard;
                status = Util::configure(invalidTimezone);
            }

            ASSERT(0                != status);
            bsl::string timezone; Util::loadTimezone(&timezone);
            ASSERT(timezone0        == timezone);
            baetzo_LocalTimePeriod localTimePeriod;
            Util::loadLocalTimePeriod(&localTimePeriod);
            ASSERT(localTimePeriod0 == localTimePeriod);

            updateCount = Util::updateCount();
            ASSERT(priorUpdateCount == updateCount);

            if (veryVeryVerbose) { T_ P(buffer) }

            status = putenv(buffer);
            ASSERT(0 == status);
            ASSERT(0 == strcmp(invalidTimezone, getenv("TZ")));

            {
                LogVerbosityGuard guard;
                status = Util::configure();
            }

            ASSERT(0                != status);
            Util::loadTimezone(&timezone);
            Util::loadLocalTimePeriod(&localTimePeriod);
            ASSERT(localTimePeriod0 == localTimePeriod);

            updateCount = Util::updateCount();
            ASSERT(priorUpdateCount == updateCount);
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
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the array-driven approach, generate test cases for several
        //:   entries representing several categories of Zoneinfo entries
        //:   (i.e., timezone before and after "Etc/GMT" and "Etc/GMT" itself)
        //:   and for several categories of UTC datetimes (i.e., before,
        //:   during, and after daylight saving time, if defined in a
        //:   timezone).  For each entry confirm that 'configure' returns
        //:   successfully, and use the accessors to confirm that the cached
        //:   information matches the expected results.  The expected results
        //:   for the local time period is obtained using the
        //:   'baetzo_TimeZoneUtil::loadLocalTimePeriodForUtc' method.
        //:   (C-1..2)
        //:
        //: 2 Save the values of the static members, call 'configure' with an
        //:   unknown timezone, check the return value, and compare the current
        //:   values of the static members to the saved values.
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   int configure(const char *timezone, const bdet_Datetime& utc);
        //   void loadLocalTimePeriod(baetzo_LocalTimePeriod *localTimePeriod);
        //   bsl::string loadTimezone(bsl::string *timezone);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATOR and BASIC ACCESSORS" << endl
                          << "=======================================" << endl;

        int      updateCount = Util::updateCount();
        int priorUpdateCount = Util::updateCount();
        ASSERT(0 ==      updateCount);
        ASSERT(0 == priorUpdateCount);

        if (verbose) cout << "\nCheck valid timezones" << endl;
        {
            for (int i = 0; i < DEFAULT_NUM_TZ_ARRAY; ++i) {
                const char *TIMEZONE = DEFAULT_TZ_ARRAY[i];

                if (veryVerbose) { P(TIMEZONE) }

                for (int j = 0; j < DEFAULT_NUM_DT_ARRAY; ++j) {
                    const bdet_Datetime& UTC = DEFAULT_DT_ARRAY[j];

                    if (veryVerbose) { T_ P(UTC) }

                    int status = Util::configure(TIMEZONE, UTC);
                    ASSERT(0 == status);

                    bsl::string timezone; Util::loadTimezone(&timezone);
                    ASSERT(0 == strcmp(TIMEZONE, timezone.c_str()));

                    updateCount = Util::updateCount();
                    ASSERT(priorUpdateCount + 1 == updateCount);
                    priorUpdateCount = updateCount;

                    baetzo_LocalTimePeriod expectedLocalTimePeriod;

                    status = baetzo_TimeZoneUtil::loadLocalTimePeriodForUtc(
                                                      &expectedLocalTimePeriod,
                                                      TIMEZONE,
                                                      UTC);
                    ASSERT(0 == status);

                    baetzo_LocalTimePeriod localTimePeriod;
                    Util::loadLocalTimePeriod(&localTimePeriod);

                    ASSERT(expectedLocalTimePeriod == localTimePeriod);
                }
            }
        }

        if (verbose) cout << "\nCheck invalid timezone" << endl;
        {
            bsl::string timezone0; Util::loadTimezone(&timezone0);
            baetzo_LocalTimePeriod localTimePeriod0;
            Util::loadLocalTimePeriod(&localTimePeriod0);

            int status;

            {
                LogVerbosityGuard guard;
                status = Util::configure("hello, world", bdet_Datetime(2013,
                                                                          2,
                                                                         28));
            }
            bsl::string timezone; Util::loadTimezone(&timezone);
            ASSERT(0                != status);
            ASSERT(timezone0        == timezone);
            baetzo_LocalTimePeriod localTimePeriod;
            Util::loadLocalTimePeriod(&localTimePeriod);
            ASSERT(localTimePeriod0 == localTimePeriod);

            updateCount = Util::updateCount();
            ASSERT(priorUpdateCount == updateCount);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'loadLocalTimePeriod'" << endl;
            {
                baetzo_LocalTimePeriod ltp;
                ASSERT_SAFE_PASS(Util::loadLocalTimePeriod(&ltp));
                ASSERT_SAFE_FAIL(Util::loadLocalTimePeriod(0));
            }

            if (veryVerbose) cout << "\t'loadTimezone'" << endl;
            {
                bsl::string s;
                ASSERT_SAFE_PASS(Util::loadTimezone(&s));
                ASSERT_SAFE_FAIL(Util::loadTimezone( 0));
            }
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

        baetzo_LocalTimePeriod localTimePeriod;

        int status1 = Util::configure(NY);
        ASSERT(0 == status1);

        if (verbose) {
            Util::loadLocalTimePeriod(&localTimePeriod);
            cout << localTimePeriod << endl;
        }

        bdet_Datetime startOfDst(2013, 3, 10, 7);
        int status2 = Util::configure(NY, startOfDst);
        ASSERT(0 == status2);

        if (verbose) {
            Util::loadLocalTimePeriod(&localTimePeriod);
            cout << localTimePeriod << endl;
        }

        bdet_Datetime resumeOfStd(2013, 11, 3, 6);
        int status3 = Util::configure(NY, resumeOfStd);
        ASSERT(0 == status3);

        if (verbose) {
            Util::loadLocalTimePeriod(&localTimePeriod);
            cout << localTimePeriod << endl;
        }

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // THREAD SAFETY
        //   The primary concern is the thread-safety of the
        //   'loadLocalTimeOffset' method.  The other public methods will
        //   typically be called once near the start of a process, and before
        //   multiple threads are launched.  Since the limit on thread creation
        //   vary significantly with platform, this test is designed to be run
        //   interactively (i.e., the test has a negative test number).
        //
        // Concerns:
        //: 1 Concurrent threads calling the 'loadLocalTimeOffset' method each
        //:   receive the correct result and return value indicating success.
        //:
        //: 2 When needed, the cached local time zone information is updated
        //:   exactly once.
        //
        // Plan:
        //: 1 Set the timezone and UTC time to some day in 2013 before the
        //:   start of daylight saving time in New York).
        //:
        //: 2 Launch multiple threads (where the number is determined by a
        //:   command-line argument) that each wait at a common barrier until
        //:   all the threads have been created, and then invoke the
        //:   'loadLocalTimeOffset' method requesting the offset for the start
        //:   of daylight saving time.
        //:
        //: 3 Compare the values obtained by each thread with the expected
        //:   offset for the start of daylight saving time.
        //:
        //: 4 Confirm that the value returned by 'updateCount' increased by
        //:   exactly one.
        //:
        //: 5 Repeat the test a number of times specified by a command-line
        //:   argument.
        //
        // Testing:
        //   CONCERN: The public methods of this component are *thread-safe*.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "THREAD SAFETY" << endl
                          << "=============" << endl;

        const int numThreads    = argc > 2 ? atoi(argv[2]) : 1;
        const int numIterations = argc > 3 ? atoi(argv[3]) : 1;

        P(numThreads)
        P(numIterations)

        bcemt_Configuration::setDefaultThreadStackSize(
                     bcemt_Configuration::recommendedDefaultThreadStackSize());

        const bdet_Datetime newYearsDay(2013, 1,  1);
        const bdet_Datetime  startOfDst(2013, 3, 10, 7);
        const char          *timezone             = "America/New_York";
        int                  previousUpdateCount = Util::updateCount();

        ASSERT(0 == previousUpdateCount);

        bsls::Stopwatch stopwatch;
        stopwatch.start(true);

        for (int i = 0;  i < numIterations; ++i) {

            int status = Util::configure(timezone, newYearsDay);
            ASSERT(0 == status);

            int updateCount = Util::updateCount();
            ASSERT(previousUpdateCount + 1 == updateCount);
            previousUpdateCount = updateCount;

            Handles       handles(numThreads);
            ThreadArgs threadArgs(numThreads);

            bcemt_Barrier   barrier(numThreads);

            // Setup and launch threads.

            for (int j = 0; j < numThreads; ++j) {
                threadArgs[j].d_offset      = 0x0BADCAFE;
                threadArgs[j].d_utcDatetime = startOfDst;
                threadArgs[j].d_barrier_p   = &barrier;

                int status = bcemt_ThreadUtil::create(&handles[j],
                                                      workerThread,
                                                      &threadArgs[j]);
                ASSERT(0 == status);
            }

            // Wait for all threads to complete.


            for (Handles::iterator itr  = handles.begin(),
                                   end  = handles.end();
                                   end != itr; ++itr) {
                bcemt_ThreadUtil::join(*itr);
            }


            // Examine results.

            updateCount = Util::updateCount();
            ASSERT(previousUpdateCount + 1 == updateCount);
            previousUpdateCount = updateCount;

            for (ThreadArgs::iterator itr  = threadArgs.begin(),
                                      end  = threadArgs.end();
                                      end != itr; ++itr) {
                ASSERT(-4 * 3600 == itr->d_offset);
            }

        }

        stopwatch.stop();

        double systemTime;
        double   userTime;
        double   wallTime;

        stopwatch.accumulatedTimes(&systemTime, &userTime, &wallTime);
        P_(systemTime) P_(userTime) P(wallTime)

      } break;
      case -2: {
        // --------------------------------------------------------------------
        // PERFORMANCE: CACHE-HITS
        //
        // Concerns:
        //: 1 The time needed to deliver an offset value from the cached
        //:   information should be known.
        //
        // Plan:
        //: 1 Set the timezone and UTC time to some day in 2013 before the
        //:   start of daylight saving time in New York).
        //:
        //: 2 Use a stopwatch mechanism to measure the time needed to
        //:   iteratively request the local time offset at the UTC datetime set
        //:   in P-1.  The number of iterations is determined by a command-line
        //:   parameter.  (C-1)
        //
        // Testing:
        //   CONCERN: Performance on cache hits.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PERFORMANCE: CACHE-HITS" << endl
                          << "=======================" << endl;

        const int numIterations = argc > 2 ? atoi(argv[2]) : 0;

        P(numIterations);

        const bdet_Datetime newYearsDay(2013, 1,  1);
        const char          *timezone             = "America/New_York";

        int status = Util::configure(timezone, newYearsDay);
        ASSERT(0 == status);

        // Confirm that the callback is working

        int offset;
        Util::loadLocalTimeOffset(&offset, newYearsDay);

        bsls::Stopwatch stopwatch;
        stopwatch.start(true);

        for (int i = 0;  i < numIterations; ++i) {
            Util::loadLocalTimeOffset(&offset, newYearsDay);
        }

        double systemTime;
        double   userTime;
        double   wallTime;
        stopwatch.accumulatedTimes(&systemTime, &userTime, &wallTime);

        P_(systemTime) P_(userTime) P(wallTime);

      } break;
      case -3: {
        // --------------------------------------------------------------------
        // PERFORMANCE: CACHE MISSES
        //
        // Concerns:
        //: 1 The time needed to deliver an offset value when the information
        //:   is not in the cache information should be known.
        //
        // Plan:
        //: 1 Set the timezone and UTC time to some day in 2013 before the
        //:   start of daylight saving time in New York).
        //:
        //: 2 Use a stopwatch mechanism to measure the time needed to
        //:   iteratively request the local time offset at the UTC datetime
        //:   corresponding to the start of daylight savings time in New York,
        //:   a cache-miss, which resets the cache.  Then, request the result
        //:   for the UTC time set in P-1, another cache-miss.  The number of
        // requests is determined by a command-line parameter.
        //:   parameter.  (C-1)
        //
        // Testing:
        //   CONCERN: Performance on cache misses.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PERFORMANCE: CACHE-MISSES" << endl
                          << "=========================" << endl;

        const int numIterations = argc > 2 ? atoi(argv[2]) : 0;

        P(numIterations);

        const bdet_Datetime  newYearsDay(2013,  1,  1);
        const bdet_Datetime   startOfDst(2013,  3, 10, 7);
        const char          *timezone = "America/New_York";

        int status = Util::configure(timezone, newYearsDay);
        ASSERT(0 == status);

        // Confirm that the callback is working

        int offset;
        Util::loadLocalTimeOffset(&offset, newYearsDay);

        const int numIterations2 = numIterations/2;

        bsls::Stopwatch stopwatch;
        stopwatch.start(true);

        for (int i = 0;  i < numIterations2; ++i) {
            Util::loadLocalTimeOffset(&offset, startOfDst);
            Util::loadLocalTimeOffset(&offset, newYearsDay);
        }

        stopwatch.stop();

        double systemTime;
        double   userTime;
        double   wallTime;
        stopwatch.accumulatedTimes(&systemTime, &userTime, &wallTime);

        P_(systemTime) P_(userTime) P(wallTime);

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
