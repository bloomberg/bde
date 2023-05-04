// ball_logthrottle.t.cpp                                             -*-C++-*-
#include <ball_logthrottle.h>

#include <ball_administration.h>
#include <ball_attribute.h>
#include <ball_attributecontainer.h>
#include <ball_attributecontainerlist.h>
#include <ball_attributecontext.h>
#include <ball_defaultattributecontainer.h>
#include <ball_log.h>
#include <ball_loggermanager.h>
#include <ball_loggermanagerconfiguration.h>
#include <ball_record.h>
#include <ball_streamobserver.h>
#include <ball_testobserver.h>

#include <bdlt_currenttime.h>
#include <bdlt_timeunitratio.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_default.h>
#include <bslma_managedptr.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bslmt_barrier.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>
#include <bsls_systemtime.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cmath.h>      // floor()
#include <bsl_cstddef.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>    // atoi()
#include <bsl_cstring.h>    // strlen(), strcmp(), memset(), memcpy(), memcmp()
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
// Undefine some awkwardly named Windows macros that interfere with this cpp
// file, but only after the last #include.
# undef ERROR
#endif

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a large number of preprocessor macros.
//
// Only macros are tested by this test driver -- no 'class'es, 'struct's, or
// functions are tested directly.  The 'class'es, 'struct's, and functions
// accessed by the macros are furnished by other components, particularly
// 'ball_log' and 'bdlmt_throttle'.
// ----------------------------------------------------------------------------
// [ 3] BALL_LOGTHROTTLE_TRACE_BLOCK
// [ 3] BALL_LOGTHROTTLE_DEBUG_BLOCK
// [ 3] BALL_LOGTHROTTLE_INFO_BLOCK
// [ 3] BALL_LOGTHROTTLE_WARN_BLOCK
// [ 3] BALL_LOGTHROTTLE_ERROR_BLOCK
// [ 3] BALL_LOGTHROTTLE_FATAL_BLOCK
// [ 3] BALL_LOGTHROTTLE_BLOCK
// [ 2] BALL_LOGTHROTTLE_STREAM
// [ 2] BALL_LOGTHROTTLE_TRACE
// [ 2] BALL_LOGTHROTTLE_DEBUG
// [ 2] BALL_LOGTHROTTLE_INFO
// [ 2] BALL_LOGTHROTTLE_WARN
// [ 2] BALL_LOGTHROTTLE_ERROR
// [ 2] BALL_LOGTHROTTLE_FATAL
// [ 1] BALL_LOGTHROTTLEVA
// [ 1] BALL_LOGTHROTTLEVA_TRACE
// [ 1] BALL_LOGTHROTTLEVA_DEBUG
// [ 1] BALL_LOGTHROTTLEVA_INFO
// [ 1] BALL_LOGTHROTTLEVA_WARN
// [ 1] BALL_LOGTHROTTLEVA_ERROR
// [ 1] BALL_LOGTHROTTLEVA_FATAL
// ----------------------------------------------------------------------------
// [10] PRINTF-BASED USAGE EXAMPLE
// [ 9] BLOCK-BASED USAGE EXAMPLE
// [ 8] STREAM-BASED USAGE EXAMPLE
// [ 7] MULTI PERIOD STREAM TEST
// [ 6] MULTI PERIOD PRINTF TEST
// [ 5] MULTITHREADED STREAM
// [ 4] MULTITHREADED PRINTF
// ----------------------------------------------------------------------------

// Please do not include the entire 'BloombergLP' or 'bsl' namespaces into
// this component, because we want to make sure that the macros under test
// work properly without them.

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

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

typedef BloombergLP::ball::Category           Cat;
typedef BloombergLP::ball::CategoryHolder     Holder;
typedef BloombergLP::ball::CategoryManager    CategoryManager;
typedef BloombergLP::ball::LoggerManager      LoggerManager;
typedef BloombergLP::ball::Severity           Sev;
typedef Sev::Level                            Level;
typedef BloombergLP::ball::ThresholdAggregate Thresholds;

typedef BloombergLP::bslma::TestAllocator     TestAllocator;

typedef BloombergLP::bsls::Types::IntPtr      IntPtr;
typedef BloombergLP::bsls::Types::Int64       Int64;

const int TRACE = Sev::e_TRACE;
const int DEBUG = Sev::e_DEBUG;
const int INFO  = Sev::e_INFO;
const int WARN  = Sev::e_WARN;
const int ERROR = Sev::e_ERROR;
const int FATAL = Sev::e_FATAL;
const int OFF   = Sev::e_OFF;

static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

static const bool k_HAS_MULTILINE_OFFSET =
    // 'k_HAS_MULTILINE_OFFSET' is 'true' if the '__LINE__' macro is
    // substituted by the line number of the last line of a macro invocation
    // split on several lines; and it is 'false' if the first line is reported.
#if defined(BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST)
    false;
#else
    true;
#endif

// ============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

                            // -----------------
                            // unnamed namespace
                            // -----------------

namespace {

namespace u {

const double epsilon = 2e-9;        // 2 nanoseconds (the timer used by the
                                    // 'Throttle' is accurate to the
                                    // microsecond).

const double minSleep = 0.00001 - epsilon;

inline
double doubleClock()
    // Return the time in seconds since New Year, 1970 GMT, according to a
    // monotonic clock.
{
    enum { k_BILLION = 1000 * 1000 * 1000 };

    return static_cast<double>(
       BloombergLP::bsls::SystemTime::nowMonotonicClock().totalNanoseconds()) /
                                                                     k_BILLION;
}

void doubleSleep(double timeInSeconds)
    // Pause for the specified 'timeInSeconds'.
{
    ASSERT(minSleep < timeInSeconds);   // 10 * 1000 microseconds is minimum
                                        // sleep, at least on some platforms.

    ASSERT(timeInSeconds <= 2.0);       // Don't want this test driver to take
                                        // too long.

    BloombergLP::bslmt::ThreadUtil::microSleep(static_cast<int>(
                                                         timeInSeconds * 1e6));
}

BloombergLP::ball::Severity::Level nextSev()
    // Return a different severity from the 'enum' 'ball::Severity::Level'
    // every time this is called, rotating through the defined values.
{
    static const Level severities[] = {Sev::e_TRACE,
                                       Sev::e_DEBUG,
                                       Sev::e_INFO,
                                       Sev::e_WARN,
                                       Sev::e_ERROR,
                                       Sev::e_FATAL };
    enum { k_NUM_SEVERITIES = sizeof severities / sizeof *severities };
    static int idx = -1;

    idx = (idx + 1) % k_NUM_SEVERITIES;

    return severities[idx];
}

enum TestType {
    e_PRINTF_STYLE = 1,
    e_STREAM_STYLE };

}  // close namespace u
}  // close unnamed namespace

bsl::ostream& operator<<(bsl::ostream& stream, u::TestType testType)
    // Stream the specified 'testType' to the specified 'stream', and return
    // 'stream'.
{
    switch (testType) {
      case u::e_PRINTF_STYLE: stream << "e_PRINTF_STYLE"; break;
      case u::e_STREAM_STYLE: stream << "e_STREAM_STYLE"; break;
      default: {
        BSLS_ASSERT_OPT(0 && "invalid testType");
      }
    }

    return stream;
}

static bool isRecordOkay(const BloombergLP::ball::TestObserver&  observer,
                         const BloombergLP::ball::Category      *category,
                         int                                     severity,
                         const char                             *fileName,
                         int                                     lineNumber,
                         const char                             *message)
    // Return 'true' if the last record published to the specified 'observer'
    // includes the name of the specified 'category' and the specified
    // 'severity', 'fileName', 'lineNumber', and 'message', and 'false'
    // otherwise.
{
    const BloombergLP::ball::RecordAttributes& attributes =
                                  observer.lastPublishedRecord().fixedFields();

    const char * const w = "isRecordOkay";
    const int saveStatus = testStatus;

    ASSERTV(w, category->categoryName(), attributes.category(),
                !bsl::strcmp(category->categoryName(), attributes.category()));
    ASSERTV(w, attributes.severity(), severity,
                                            attributes.severity() == severity);
    ASSERTV(w, fileName, attributes.fileName(),
                                !bsl::strcmp(fileName, attributes.fileName()));
    ASSERTV(w, lineNumber, attributes.lineNumber(),
                                        lineNumber == attributes.lineNumber());
    ASSERTV(w, message, attributes.message(),
                                  !bsl::strcmp(message, attributes.message()));

    return saveStatus == testStatus;
}

namespace Usage {

struct Data {
    double d_deltaTime;
    double d_reading;
};

struct RadiationMeterReceiver {
    // DATA
    double                    d_lastDetection;
    Data                     *d_data_p;
    static       Data         s_data[];
    static const bsl::size_t  s_numData;

    RadiationMeterReceiver()
    : d_lastDetection(u::doubleClock())
    , d_data_p(&s_data[0])
        // Default-construct a 'RadiationMeterReceiver'.
    {}

    double yield();
        // Yield a radiation level.
};

Data RadiationMeterReceiver::s_data[] = {
    { 0.3, 12.3 }, { 0.2, 10.5 }, { 0.1, 33.1 }, { 0.1, 11.9 }, { 0.1, 53.7 },
    { 0.3, 46.1 }, { 0.1, 14.7 }, { 0.1, 67.4 }, { 0.1, 43.9 }, { 0.1, 53.3 },
    { 0.1, 98.2 }, { 0.2, 22.3 }, { 0.1, 77.3 }, { 0.1, 36.2 }, { 0.2, 17.7 },
    { 0.1, 52.5 }, { 0.2, 43.2 }, { 0.2, 72.9 }, { 0.2, 51.9 }, { 0.1, 71.2 },
    { 0.1, 92.4 }, { 0.3, 11.8 }, { 0.1, 33.1 }, { 0.2, 47.2 }, { 0.1, 15.5 },
    { 0.1, 35.7 }, { 0.2, 22.3 }, { 0.1, 17.6 }, { 0.2, 52.7 }, { 0.2, 22.1 }};
const bsl::size_t RadiationMeterReceiver::s_numData =
                                        sizeof  RadiationMeterReceiver::s_data
                                      / sizeof *RadiationMeterReceiver::s_data;

double RadiationMeterReceiver::yield()
{
    if (s_data + s_numData <= d_data_p) {
        return -1.0;                                                  // RETURN
    }

    const double nextReading = d_lastDetection + d_data_p->d_deltaTime;
    const double tick        = u::doubleClock() + 0.101;
    const double waitUntil   = bsl::min(nextReading, tick);

    double t;
    while ((t = u::doubleClock()) < waitUntil) {
        u::doubleSleep(0.01);
    }

    if (nextReading <= t) {
        d_lastDetection = t;
        return (d_data_p++)->d_reading;                               // RETURN
    }
    else {
        // No radiation detected.

        return 0.0;                                                   // RETURN
    }
}

//
///Usage
///-----
// This section illustrates the intended use of this component.
//
// The following constants are used throughout the usage examples:
//..
    enum {
        k_NUM_INFO  = 20,       // max # of info messages in a very short time
        k_NUM_DEBUG =  5,       // max # of debug messages in a very short time
        k_NUM_TRACE =  1        // max # of trace messages in a very short time
    };
//
    const Int64 k_NS_PER_HOUR =
                      BloombergLP::bdlt::TimeUnitRatio::k_NANOSECONDS_PER_HOUR;
//
    const Int64 k_NS_PER_INFO  = k_NS_PER_HOUR / k_NUM_INFO;
                   // long-term minimum nanoseconds per info message permitted
    const Int64 k_NS_PER_DEBUG = k_NS_PER_HOUR / k_NUM_DEBUG;
                   // long-term minimum nanoseconds per debug message permitted
    const Int64 k_NS_PER_TRACE = k_NS_PER_HOUR / k_NUM_TRACE;
                   // long-term minimum nanoseconds per trace message permitted
//..
//
///Example 1: C++ Stream-Style Throttling Macro Usage
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose a computer is reading 'double' values from a radio receiver, ten per
// second, which represent readings of radiation detected by a Geiger counter
// on a spacecraft, and is transmitting them to a ground control at Jet
// Propulsion Laboratories in California.
//
// The readings are returned by the 'double yield()' manipulator of a
// 'RadiationMeterReceiver' object (the implementation of which is omitted).
// The 'yield' method blocks until it obtains a reading to return.  If called
// in a tight loop, 'yield' returns ten readings per second.
//
// Readings range from 0 to 100.
//: o Readings above 10 but not greater than 30 are a concern, but are not very
//:   serious.  We will report those with an 'e_TRACE' severity, and at most
//:   one per hour (i.e., messages will be throttled).
//: o Readings above 30 but not greater than 60 are more of a worry.  We will
//:   report those with an 'e_DEBUG' severity, and at most five per hour.
//: o Readings above 60 but not greater than 90 are very serious.  They will be
//:   reported with an 'e_INFO' severity, and at most twenty per hour.
//: o Readings above 90 are potentially catastrophic, and will be reported with
//:   an 'e_WARN' severity, with no limit on the number of readings reported
//:   (i.e., no throttling).
//
// We are to write a daemon process, which will loop gathering readings.  A
// reading of an impossible value of -1.0 will indicate termination.
//..
    void radiationMonitorStreamDaemon()
        // Daemon to run the radiation monitor.
    {
        BALL_LOG_SET_CATEGORY("RADIATION.MONITOR");
//
        RadiationMeterReceiver receiver;
//
        BALL_LOG_DEBUG << "Start gathering data.";

        double reading;
        while (-1.0 != (reading = receiver.yield())) {
//..
// First, we deal with 'e_WARN' readings:
//..
            if      (90 < reading) {
                BALL_LOG_WARN << "Serious Radiation reading of " << reading;
            }
//..
// Next, we deal with 'e_INFO' readings, which aren't as severe as 'e_WARN':
//..
            else if (60 < reading) {
                BALL_LOGTHROTTLE_INFO(k_NUM_INFO, k_NS_PER_INFO) <<
                                            "Radiation reading of " << reading;
            }
//..
// Now, we deal with 'e_DEBUG' messages less severe than 'e_INFO' readings:
//..
            else if (30 < reading) {
                BALL_LOGTHROTTLE_DEBUG(k_NUM_DEBUG, k_NS_PER_DEBUG) <<
                                            "Radiation reading of " << reading;
            }
//..
// Finally, we deal with 'e_TRACE' messages less severe than 'e_DEBUG'
// readings:
//..
            else if (10 < reading) {
                BALL_LOGTHROTTLE_TRACE(k_NUM_TRACE, k_NS_PER_TRACE) <<
                                            "Radiation reading of " << reading;
            }
        }

        BALL_LOG_DEBUG << "Finished gathering data.";
    }
//..
// The values returned by 'receiver.yield()' are:
//..
//  0 0 12.3 0 10.5 33.1 11.9 53.7 0 0 46.1 14.7 67.4 43.9 53.3 98.2 0 22.3
//  77.3 36.2 0 17.7 52.5 0 43.2 0 72.9 0 51.9 71.2 92.4 0 0 11.8 33.1 0 47.2
//  15.5 35.7 0 22.3 17.6 0 52.7 0 22.1 -1
//..
// Where:
//: o 13 readings of 0.0, which don't produce output, occurred.
//: o 9 readings in the range '(10.0 .. 30.0]', which correspond to 'e_TRACE'
//:   level messages, occurred.
//: o 13 readings in the range '(30.0 .. 60.0]', which correspond to 'e_DEBUG'
//:   level messages, occurred.
//: o 5 readings in the range '(60.0 .. 90.0]', which correspond to 'e_INFO'
//:   level messages, occurred.
//: o 2 readings greater than 90.0, which correspond to 'e_WARN' level
//:   messages, occurred.
//
// Note that only 1 'e_TRACE' message and 5 'e_DEBUG' messages are permitted by
// the throttle within the (very long) time period of one hour, so the other
// messages at those levels will be suppressed.
//
// 'radiationMonitorPrintfDaemon' produces output like:
//..
//  24APR2018_16:36:22.791 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  460 RADIATION.MONITOR Start gathering data.
//
//  24APR2018_16:36:23.094 61260 139907579877152 TRACE ball_logthrottle.t.cpp
//  488 RADIATION.MONITOR Radiation reading of 12.3
//
//  24APR2018_16:36:23.396 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  481 RADIATION.MONITOR Radiation reading of 33.1
//
//  24APR2018_16:36:23.597 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  481 RADIATION.MONITOR Radiation reading of 53.7
//
//  24APR2018_16:36:23.901 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  481 RADIATION.MONITOR Radiation reading of 46.1
//
//  24APR2018_16:36:24.102 61260 139907579877152 INFO ball_logthrottle.t.cpp
//  474 RADIATION.MONITOR Radiation reading of 67.4
//
//  24APR2018_16:36:24.203 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  481 RADIATION.MONITOR Radiation reading of 43.9
//
//  24APR2018_16:36:24.304 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  481 RADIATION.MONITOR Radiation reading of 53.3
//
//  24APR2018_16:36:24.404 61260 139907579877152 WARN ball_logthrottle.t.cpp
//  468 RADIATION.MONITOR Serious Radiation reading of 98.2
//
//  24APR2018_16:36:24.706 61260 139907579877152 INFO ball_logthrottle.t.cpp
//  474 RADIATION.MONITOR Radiation reading of 77.3
//
//  24APR2018_16:36:25.513 61260 139907579877152 INFO ball_logthrottle.t.cpp
//  474 RADIATION.MONITOR Radiation reading of 72.9
//
//  24APR2018_16:36:25.816 61260 139907579877152 INFO ball_logthrottle.t.cpp
//  474 RADIATION.MONITOR Radiation reading of 71.2
//
//  24APR2018_16:36:25.918 61260 139907579877152 WARN ball_logthrottle.t.cpp
//  468 RADIATION.MONITOR Serious Radiation reading of 92.4
//
//  24APR2018_16:36:27.429 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  493 RADIATION.MONITOR Finished gathering data.
//..
// Note that 8 'e_TRACE' messages and 8 'e_DEBUG' messages were suppressed by
// the throttling.
//
///Example 2: BLOCK-Style Throttling Macro Usage
///- - - - - - - - - - - - - - - - - - - - - - -
// Here, we just repeat exactly the same code, using the BLOCK-style throttling
// macros instead of the stream-style throttling macros:
//..
    void radiationMonitorBlockDaemon()
        // Daemon to run the radiation monitor.
    {
        BALL_LOG_SET_CATEGORY("RADIATION.MONITOR");
//
        RadiationMeterReceiver receiver;
//
        BALL_LOGVA_DEBUG("Start gathering data.");

        double reading;
        while (-1.0 != (reading = receiver.yield())) {
//..
// First, we deal with 'e_WARN' messages:
//..
            if      (90 < reading) {
                BALL_LOG_WARN_BLOCK {
                    BALL_LOG_OUTPUT_STREAM <<
                                    "Serious radiation reading of " << reading;
                }
            }
//..
// Next, we deal with 'e_INFO' messages that aren't as severe as 'e_WARN':
//..
            else if (60 < reading) {
                BALL_LOGTHROTTLE_INFO_BLOCK(k_NUM_INFO, k_NS_PER_INFO) {
                    BALL_LOG_OUTPUT_STREAM <<
                                            "Radiation reading of " << reading;
                }
            }
//..
// Now, we deal with 'e_DEBUG' messages less severe than 'e_INFO' readings:
//..
            else if (30 < reading) {
                BALL_LOGTHROTTLE_DEBUG_BLOCK(k_NUM_DEBUG, k_NS_PER_DEBUG) {
                    BALL_LOG_OUTPUT_STREAM <<
                                            "Radiation reading of " << reading;
                }
            }
//..
// Finally, we deal with 'e_TRACE' messages less severe than 'e_DEBUG'
// readings.
//..
            else if (10 < reading) {
                BALL_LOGTHROTTLE_TRACE_BLOCK(k_NUM_TRACE, k_NS_PER_TRACE) {
                    BALL_LOG_OUTPUT_STREAM << "Radiation reading of "
                                                                    << reading;
                }
            }
        }

        BALL_LOG_DEBUG << "Finished gathering data.";
    }
//..
// If the values returned by 'receiver.yield()' match those from Usage Example
// 1, then the output will be identical to that example.
//..
///Example 3: 'printf'-Style Throttling Macro Usage
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Here, we again repeat exactly the same code, using the 'printf'-style
// throttling macros:
//..
    void radiationMonitorPrintfDaemon()
        // Daemon to run the radiation monitor.
    {
        BALL_LOG_SET_CATEGORY("RADIATION.MONITOR");
//
        RadiationMeterReceiver receiver;
//
        BALL_LOGVA_DEBUG("Start gathering data.");

        double reading;
        while (-1.0 != (reading = receiver.yield())) {
//..
// First, we deal with 'e_WARN' messages:
//..
            if      (90 < reading) {
                BALL_LOGVA_WARN("Serious radiation reading of %g", reading);
            }
//..
// Next, we deal with 'e_INFO' messages that aren't as severe as 'e_WARN':
//..
            else if (60 < reading) {
                BALL_LOGTHROTTLEVA_INFO(
                                     k_NUM_INFO,
                                     k_NS_PER_INFO,
                                     "Radiation reading of %g", reading);
            }
//..
// Now, we deal with 'e_DEBUG' messages less severe than 'e_INFO' readings:
//..
            else if (30 < reading) {
                BALL_LOGTHROTTLEVA_DEBUG(
                                   k_NUM_DEBUG,
                                   k_NS_PER_DEBUG,
                                   "Radiation reading of %g", reading);
            }
//..
// Finally, we deal with 'e_TRACE' messages less severe than 'e_DEBUG'
// readings:
//..
            else if (10 < reading) {
                BALL_LOGTHROTTLEVA_TRACE(
                                      k_NUM_TRACE,
                                      k_NS_PER_TRACE,
                                      "Radiation reading of %g", reading);
            }
        }

        BALL_LOGVA_DEBUG("Finished gathering data.");
    }
//..
// If the values returned by 'receiver.yield()' match those from Usage Example
// 1, then the output will be identical to that example.

}  // close namespace Usage

                                // ---------------
                                // MultiPeriodTest
                                // ---------------

namespace MultiPeriodTest {

void testMain(
             const bsl::shared_ptr<BloombergLP::ball::TestObserver>&  TO,
             u::TestType                                              testType,
             TestAllocator                                           *alloc)
    // This function runs the multi period test, one 'period' being the
    // 'NANOSECONDS_PER_MESSAGE' passed to the throttle.  Use the specified
    // observer '*TO', and the specified 'alloc' for memory allocation.  The
    // specified 'testType' indicates whether we are to perform a stream-style
    // or printf-style test.
    //
    // Note that this test, described in test cases 5 and 6 where it is called,
    // is single threaded.
{
    BloombergLP::ball::LoggerManagerConfiguration lmc;
    BloombergLP::ball::LoggerManagerScopedGuard lmg(lmc, alloc);
    BloombergLP::ball::LoggerManager::singleton().registerObserver(TO, "TO");

    BloombergLP::ball::Administration::addCategory(
                                      "sieve",
                                      BloombergLP::ball::Severity::e_DEBUG,
                                      BloombergLP::ball::Severity::e_DEBUG,
                                      0,
                                      0);
    BALL_LOG_SET_CATEGORY("sieve");

    enum { k_BURST_SIZE              = 10,
           k_NANOSECONDS_PER_MESSAGE = 400 * 1000 * 1000,
           k_II_LOOPS                = 10,
           k_JJ_LOOPS                = k_BURST_SIZE * 5 };

    const double secondsPerMessage = k_NANOSECONDS_PER_MESSAGE * 1e-9;
    const double sleepTime         = secondsPerMessage * 0.51;

    int retries = 0, expTotal = 0;
    double start = u::doubleClock(), lastEpoch = start;
    for (int ii = 0; ii < k_II_LOOPS; ++ii) {
        const double rawEpoch = (u::doubleClock() - lastEpoch) /
                                                             secondsPerMessage;
        const double epochs   = bsl::floor(rawEpoch);
        ASSERTV(ii, rawEpoch, epochs, 0 <= rawEpoch && 0 <= epochs);

        if (2.0 <= epochs) {
            ++retries;

            cout << "Excessive sleep: epoch: " << epochs << ", retries: " <<
                                                               retries << endl;

            ASSERTV(retries, retries < 5);
            if (5 <= retries) {
                return;                                               // RETURN
            }

            u::doubleSleep(k_BURST_SIZE * secondsPerMessage * 1.1);
            ii = -1;
            start = u::doubleClock(), lastEpoch = start;
            continue;
        }
        ASSERTV(ii, epochs, !ii || !(ii & 1) == (1.0 == epochs));

        ASSERTV(epochs, epochs < 2.0);

        const int iiExp = 0 == ii
                        ? 10
                        : 0.0 == epochs
                        ? 0
                        : 1;
        if (ii && 0 < iiExp) {
            ASSERT(1.0 == epochs);

            lastEpoch += secondsPerMessage;
        }

        for (int jj = 1; jj <= k_JJ_LOOPS; ++jj) {
            switch (testType) {
              case u::e_PRINTF_STYLE: {
                // Threshold is 'e_DEBUG', so trace shouldn't be published, but
                // info should.

                BALL_LOGTHROTTLEVA_INFO(
                                      k_BURST_SIZE,
                                      k_NANOSECONDS_PER_MESSAGE,
                                      "Info:  Printf: ii: %d, jj: %d", ii, jj);

                BALL_LOGTHROTTLEVA_TRACE(
                                      k_BURST_SIZE,
                                      k_NANOSECONDS_PER_MESSAGE,
                                      "Trace: Printf: ii: %d, jj: %d", ii, jj);
              } break;
              case u::e_STREAM_STYLE: {
                // Threshold is 'e_DEBUG', so trace shouldn't be published, but
                // info should.

                BALL_LOGTHROTTLE_INFO( k_BURST_SIZE,
                                       k_NANOSECONDS_PER_MESSAGE) <<
                                 "Info:  Stream: ii: " << ii << ", jj: " << jj;

                BALL_LOGTHROTTLE_TRACE(k_BURST_SIZE,
                                       k_NANOSECONDS_PER_MESSAGE) <<
                                 "Trace: Stream: ii: " << ii << ", jj: " << jj;
              } break;
              default: {
                BSLS_ASSERT_OPT(0 && "invalid testType");
              }
            }

            const int jjExp = jj < iiExp ? jj : iiExp;
            ASSERTV(expTotal, jjExp, iiExp, TO->numPublishedRecords(), epochs,
                                expTotal + jjExp == TO->numPublishedRecords());
        }

        expTotal += iiExp;
        ASSERTV(ii, iiExp, expTotal, TO->numPublishedRecords(), epochs,
                                        expTotal == TO->numPublishedRecords());

        if (veryVerbose) {
            P_(ii);    P_(expTotal);    P(TO->numPublishedRecords());
        }

        const double toSleep = sleepTime * (ii+1) - (u::doubleClock() - start);
        u::doubleSleep(toSleep);
    }

    const double elapsed = u::doubleClock() - start;

    const BloombergLP::ball::RecordAttributes& attributes =
                                       TO->lastPublishedRecord().fixedFields();

    const int    numPublished = TO->numPublishedRecords();
    const char  *lastTrace    = attributes.message();
    const Level  severity     = static_cast<Level>(attributes.severity());

    ASSERTV(expTotal, numPublished, expTotal == numPublished);
    ASSERTV(severity, INFO == severity);
    const char * const expTrace = u::e_PRINTF_STYLE == testType
                                ? "Info:  Printf: ii: 8, jj: 1"
                                : "Info:  Stream: ii: 8, jj: 1";

    ASSERTV(testType, lastTrace, expTrace, !strcmp(lastTrace, expTrace));

    if (verbose) {
        P_(numPublished);    P_(severity);    P_(lastTrace);    P(elapsed);
    }
}

}  // close namespace MultiPeriodTest

                            // ------------------
                            // MultiThreadedTests
                            // ------------------

namespace MultiThreadedTests {

enum { k_NUM_THREADS             = 40,
       k_TERMINATE               = 2 * k_NUM_THREADS + 1,
       k_PERMITTED               = 400,
       k_NANOSECONDS_PER_MESSAGE = 1000 * 1000 * 1000,
       k_ATTEMPTS_PER_THREAD     = k_PERMITTED * 100 / k_NUM_THREADS };

BloombergLP::bslmt::Barrier  barrier(k_NUM_THREADS + 1);
BloombergLP::bsls::AtomicInt atomicBarrier(0);
BloombergLP::bsls::AtomicInt idKey(0);
BloombergLP::bsls::AtomicInt numAttempts(0);

class Func {
    // This 'class' is a functor for multithreaded testing.

    BALL_LOG_SET_CLASS_CATEGORY("sieve");

    // DATA
    u::TestType d_testType;

  public:
    // CREATOR
    explicit
    Func(u::TestType testType)
    : d_testType(testType)
        // Create a functor object based upon the specified 'testType'.
    {}

    // Func(const Func&) = default;
    // Func& operator=(const Func&) = default;

    // ACCESSOR
    void operator()() const
        // Run one thread of the test.  Use 'barrier' and 'atomicBarrier' to
        // coordinate with other subthreads and the main thread.  Attempt
        // 'k_ATTEMPTS_PER_THREAD' traces at 'debug' and 'trace' level (we
        // expect some of the 'debug' messages and none of the 'trace' messages
        // to go through'.  Keep a total of the attempts in the atomic
        // 'numAttempts'.
    {
        const int id = idKey++;

        barrier.wait();
        ++atomicBarrier;
        while (atomicBarrier <= k_NUM_THREADS) ;

        switch (d_testType) {
          case u::e_PRINTF_STYLE: {
            for (int ii = 0; ii < k_ATTEMPTS_PER_THREAD; ++ii) {
                // Threshold is 'e_DEBUG', so trace shouldn't be published, but
                // debug should.

                BALL_LOGTHROTTLEVA_DEBUG(k_PERMITTED,k_NANOSECONDS_PER_MESSAGE,
                                     "Logva: Debug: Id: %d, iter: %d", id, ii);
                BALL_LOGTHROTTLEVA_TRACE(k_PERMITTED,k_NANOSECONDS_PER_MESSAGE,
                                     "Logva: Trace: Id: %d, iter: %d", id, ii);
            }
          } break;
          case u::e_STREAM_STYLE: {
            for (int ii = 0; ii < k_ATTEMPTS_PER_THREAD; ++ii) {
                // Threshold is 'e_DEBUG', so trace shouldn't be published, but
                // debug should.

                BALL_LOGTHROTTLE_DEBUG(k_PERMITTED,k_NANOSECONDS_PER_MESSAGE)<<
                                "Stream: Debug: Id: " << id << ", iter:" << ii;
                BALL_LOGTHROTTLE_TRACE(k_PERMITTED,k_NANOSECONDS_PER_MESSAGE)<<
                                "Stream: Trace: Id: " << id << ", iter:" << ii;
            }
          } break;
          default: {
            BSLS_ASSERT_OPT(0 && "invalid testType");
          }
        }

        numAttempts += k_ATTEMPTS_PER_THREAD;
        ++atomicBarrier;
    }
};

void testMain(
             const bsl::shared_ptr<BloombergLP::ball::TestObserver>&  TO,
             u::TestType                                              testType,
             TestAllocator                                           *alloc)
    // This test case is called from cases 3 and 4, and the test plan is
    // described in detail there.  Use the specified observer '*TO', with the
    // specified 'testType' indicating whether the test is 'printf'-style or
    // stream style.  Use the specified 'alloc' for memory allocation.
{
    BloombergLP::ball::LoggerManagerConfiguration lmc;
    BloombergLP::ball::LoggerManagerScopedGuard lmg(lmc, alloc);
    BloombergLP::ball::LoggerManager::singleton().registerObserver(TO, "TO");

    // Re "sieve" category: (1) if recorded, then also published;
    // (2) never triggered.

    BloombergLP::ball::Administration::addCategory(
                                      "sieve",
                                      BloombergLP::ball::Severity::e_DEBUG,
                                      BloombergLP::ball::Severity::e_DEBUG,
                                      0,
                                      0);

    BloombergLP::bslmt::ThreadGroup tg(alloc);

    tg.addThreads(Func(testType), k_NUM_THREADS);

    barrier.wait();

    const double start = u::doubleClock();
    ++atomicBarrier;

    while (atomicBarrier < k_TERMINATE) ;
    const double  elapsed      = u::doubleClock() - start;

    tg.joinAll();

    const int     numPublished = TO->numPublishedRecords();
    const BloombergLP::ball::RecordAttributes& attributes =
                                   TO->lastPublishedRecord().fixedFields();
    const char   *lastTrace = attributes.message();
    const int     severity  = attributes.severity();
    const double  minElapsed = 0.9 * 1e-9 * k_NANOSECONDS_PER_MESSAGE;

    ASSERTV(k_TERMINATE == atomicBarrier);
    ASSERTV(elapsed, minElapsed, elapsed < minElapsed);
    ASSERTV(k_PERMITTED,numPublished, k_PERMITTED == numPublished);
    ASSERTV(k_ATTEMPTS_PER_THREAD * k_NUM_THREADS == numAttempts);
    ASSERTV(DEBUG == severity);

    if (verbose) {
        P_(elapsed);    P_(numPublished);    P(numAttempts);
        P(lastTrace);
    }
}

}  // close namespace MultiThreadedTests

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;


    bsl::shared_ptr<BloombergLP::ball::TestObserver> TO =
              bsl::make_shared<BloombergLP::ball::TestObserver>(&cout);

    TestAllocator ta("test", veryVeryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // PRINTF-BASED USAGE EXAMPLE
        //
        // Concerns:
        //: 1 Demonstrate the usage of the printf-based macros.
        //
        // Plan:
        //: 1 Compile and execute Example 2 from the component doc.
        //
        // Testing:
        //   PRINTF-BASED USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "PRINTF-BASED USAGE EXAMPLE\n"
                             "==========================\n";

        bsl::ostringstream oss;
        bsl::shared_ptr<BloombergLP::ball::StreamObserver> sobs =
                     bsl::make_shared<BloombergLP::ball::StreamObserver>(&oss);

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard   lmg(lmc, &ta);
        BloombergLP::ball::LoggerManager::singleton().registerObserver(sobs,
                                                                       "SO");

        // Re "sieve" category: (1) if recorded, then also published;
        // (2) never triggered.

        BloombergLP::ball::Administration::addCategory(
                                "RADIATION.MONITOR",
                                BloombergLP::ball::Severity::e_TRACE,
                                BloombergLP::ball::Severity::e_TRACE,
                                0,
                                0);

        Usage::radiationMonitorPrintfDaemon();

        if (veryVerbose) cout << oss.str();
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // BLOCK-BASED USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the usage of the block-based macros.
        //
        // Plan:
        //: 2 Demonstrate and execute
        //
        // Testing:
        //   BLOCK-BASED USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "BLOCK-BASED USAGE EXAMPLE\n"
                             "==========================\n";

        bsl::ostringstream oss;
        bsl::shared_ptr<BloombergLP::ball::StreamObserver> sobs =
                     bsl::make_shared<BloombergLP::ball::StreamObserver>(&oss);

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard lmg(lmc, &ta);
        BloombergLP::ball::LoggerManager::singleton().registerObserver(sobs,
                                                                       "SO");

        // Re "sieve" category: (1) if recorded, then also published;
        // (2) never triggered.

        BloombergLP::ball::Administration::addCategory(
                                "RADIATION.MONITOR",
                                BloombergLP::ball::Severity::e_TRACE,
                                BloombergLP::ball::Severity::e_TRACE,
                                0,
                                0);

        Usage::radiationMonitorBlockDaemon();

        if (veryVerbose) cout << oss.str();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // STREAM-BASED USAGE EXAMPLE
        //
        // Concerns:
        //: 1 Demonstrate the usage of the stream-based macros.
        //
        // Plan:
        //: 1 Compile and execute Example 1 from the component doc.
        //:
        //: 2 If 'veryVeryVerbose' is set, create a second radiation monitor
        //:   and dump out the raw results returned by it before doing the
        //:   usage example.
        //
        // Testing:
        //   STREAM-BASED USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "STREAM-BASED USAGE EXAMPLE\n"
                             "==========================\n";

        if (veryVeryVerbose) {
            Usage::RadiationMeterReceiver receiver;

            cout << "Raw radiation readings:\n";

            const double start = u::doubleClock();
            double value;
            do {
                value = receiver.yield();
                cout << ' ' << value;
            } while (0 <= value);
            cout << endl;
            cout << "In " << (u::doubleClock() - start) << " seconds\n";
        }

        bsl::ostringstream oss;
        bsl::shared_ptr<BloombergLP::ball::StreamObserver> sobs =
                     bsl::make_shared<BloombergLP::ball::StreamObserver>(&oss);

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard   lmg(lmc, &ta);
        BloombergLP::ball::LoggerManager::singleton().registerObserver(sobs,
                                                                       "SO");

        // Re "sieve" category: (1) if recorded, then also published;
        // (2) never triggered.

        BloombergLP::ball::Administration::addCategory(
                                "RADIATION.MONITOR",
                                BloombergLP::ball::Severity::e_TRACE,
                                BloombergLP::ball::Severity::e_TRACE,
                                0,
                                0);

        Usage::radiationMonitorStreamDaemon();

        if (veryVerbose) cout << oss.str();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // MULTI PERIOD STREAM TEST
        //
        // Concerns:
        //: 1 That the stream-style macros will produce the correct number of
        //:   traces over a timespan encompassing several
        //:   'NANOSECONDS_PER_MESSAGE' periods.
        //
        // Plan:
        //: 1 Do a single-threaded test, where attempts to are made to issue
        //:   bursts of traces.
        //:
        //: 2 The length of each attempted burst will greatly exceed the number
        //:   that the throttle is configured to permit.
        //:
        //: 3 Each attempted burst will be completed in less time than the
        //:   'NANOSECONDS_PER_MESSAGE' period length specified to the
        //:   throttle.
        //:
        //: 4 Between bursts, sleep barely over half a period.
        //:
        //: 5 It will thus be possible, at any point, to calculate the exact
        //:   number of traces that will have been published.  Each iteration,
        //:   do so, and query the observer to verify that that is what
        //:   happened.
        //:
        //: 6 Confirm that the message in the last trace is exactly as
        //:   predicted.
        //
        // Testing:
        //   MULTI PERIOD STREAM TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTI PERIOD STREAM TEST\n"
                             "========================\n";

        namespace TC = MultiPeriodTest;

        TC::testMain(TO, u::e_STREAM_STYLE, &ta);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // MULTI PERIOD PRINTF TEST
        //
        // Concerns:
        //: 1 That the printf-style macros will produce the correct number of
        //:   traces over a timespan encompassing several
        //:   'NANOSECONDS_PER_MESSAGE' periods.
        //
        // Plan:
        //: 1 Do a single-threaded test, where attempts to are made to issue
        //:   bursts of traces.
        //:
        //: 2 The length of each attempted burst will greatly exceed the number
        //:   that the throttle is configured to permit.
        //:
        //: 3 Each attempted burst will be completed in less time than the
        //:   'NANOSECONDS_PER_MESSAGE' period length specified to the
        //:   throttle.
        //:
        //: 4 Between bursts, sleep barely over half a period.
        //:
        //: 5 It will thus be possible, at any point, to calculate the exact
        //:   number of traces that will have been published.  Each iteration,
        //:   do so, and query the observer to verify that that is what
        //:   happened.
        //:
        //: 6 Confirm that the message in the last trace is exactly as
        //:   predicted.
        //
        // Testing:
        //   MULTI PERIOD PRINTF TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTI PERIOD PRINTF TEST\n"
                             "========================\n";

        namespace TC = MultiPeriodTest;

        TC::testMain(TO, u::e_PRINTF_STYLE, &ta);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // MULTITHREADED STREAM TEST
        //
        // Concerns:
        //: 1 That the 'BALL_LOGTHROTTLEVA_*' macros operate correctly under
        //:   heavy multithreaded contention.
        //:
        //: 2 That the number of traces permitted exactly equals the number
        //:   specified to the macro.
        //:
        //: 3 That only traces with sufficient severity get logged.
        //
        // Plan:
        //: 1 Start many threads, coordinate the start by spinning on the
        //:   atomic variable 'atomicBarrier' so they all start at very nearly
        //:   exactly the same time.
        //:
        //: 2 Have the threads attempt to log traces many times, so that they
        //:   attempt 100 times as many traces as are to be permitted.
        //:
        //: 3 Observe that the elapsed time during which all logging took place
        //:   is less than one period.
        //:
        //: 4 Sum up the total number of attempted traces and verify that it is
        //:   as expected.
        //:
        //: 5 Query the observer and verify that exactly the number of traces
        //:   permitted occurred.
        //:
        //: 6 Observe that the last published record was of sufficient severity
        //:   to be logged.
        //
        // Testing
        //   MULTITHREADED STREAM
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTITHREADED STREAM TEST\n"
                             "=========================\n";

        namespace TC = MultiThreadedTests;

        TC::testMain(TO, u::e_STREAM_STYLE, &ta);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // MULTITHREADED PRINTF TEST
        //
        // Concerns:
        //: 1 That the 'BALL_LOGTHROTTLEVA_*' macros operate correctly under
        //:   heavy multithreaded contention.
        //:
        //: 2 That the number of traces permitted exactly equals the number
        //:   specified to the macro.
        //:
        //: 3 That only traces with sufficient severity get logged.
        //
        // Plan:
        //: 1 Start many threads, coordinate the start by spinning on the
        //:   atomic variable 'atomicBarrier' so they all start at very nearly
        //:   exactly the same time.
        //:
        //: 2 Have the threads attempt to log traces many times, so that they
        //:   attempt 100 times as many traces as are to be permitted.
        //:
        //: 3 Observe that the elapsed time during which all logging took place
        //:   is less than one period.
        //:
        //: 4 Sum up the total number of attempted traces and verify that it is
        //:   as expected.
        //:
        //: 5 Query the observer and verify that exactly the number of traces
        //:   permitted occurred.
        //:
        //: 6 Observe that the last published record was of sufficient severity
        //:   to be logged.
        //
        // Testing
        //   MULTITHREADED PRINTF
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTITHREADED PRINTF TEST\n"
                             "=========================\n";

        namespace TC = MultiThreadedTests;

        TC::testMain(TO, u::e_PRINTF_STYLE, &ta);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BLOCK MACRO TEST
        //
        // Concerns:
        //: 1 That the block macros permit the expected number of events for
        //:   the severity levels applied.
        //
        // Plan:
        //: 1 Create the function 'oneTest', which will call the block macros a
        //:   large number of times in less than the 'NANOSECONDS_PER_MESSAGE'
        //:   arg of time with the category at a specified level and the
        //:   attempted severity at another specified level.
        //:
        //: 2 Call 'oneTest' with every combination of threshold vs attempt
        //:   severities, waiting
        //:   '2 * MAX_SIMULTANEOUS_ACTIONS * NANOSECONDS_PER_MESSAGE' between
        //:   calls.
        //:
        //: 3 Put the macros without '{}'s in the statement controlled by an
        //:   'if' with an 'else' to verify that the macros expand to a single
        //:   C++ statement.
        //
        // Testing:
        //   BALL_LOGTHROTTLE_TRACE_BLOCK
        //   BALL_LOGTHROTTLE_DEBUG_BLOCK
        //   BALL_LOGTHROTTLE_INFO_BLOCK
        //   BALL_LOGTHROTTLE_WARN_BLOCK
        //   BALL_LOGTHROTTLE_ERROR_BLOCK
        //   BALL_LOGTHROTTLE_FATAL_BLOCK
        //   BALL_LOGTHROTTLE_BLOCK
        // --------------------------------------------------------------------

        if (verbose) cout << "BLOCK MACRO TEST\n"
                             "================\n";

        static
        const Level levels[] = { Sev::e_OFF,  Sev::e_FATAL, Sev::e_ERROR,
                                 Sev::e_WARN, Sev::e_INFO, Sev::e_DEBUG,
                                 Sev::e_TRACE };
        enum { k_NUM_LEVELS = sizeof levels / sizeof *levels };

        enum { k_MSM = 5,                   // max simultaneous messages
               k_NPM = 10 * 1000 * 1000 };  // nanoseconds per message

        const double periodTime        = 1e-9 * k_NPM;
        const double attemptTime       = 0.5         * periodTime;
        const double betweenTrialsTime = 1.5 * k_MSM * periodTime;

        for (int ii = 0; ii < k_NUM_LEVELS; ++ii) {
            const Level categoryThreshold = levels[ii];

            if (veryVerbose) P(categoryThreshold);

            bsl::ostringstream oss(&ta);
            bsl::shared_ptr<BloombergLP::ball::StreamObserver> sobs =
                     bsl::make_shared<BloombergLP::ball::StreamObserver>(&oss);

            BloombergLP::ball::LoggerManagerConfiguration lmc;
            BloombergLP::ball::LoggerManagerScopedGuard   lmg(lmc, &ta);
            BloombergLP::ball::LoggerManager::singleton().registerObserver(
                                                                         sobs,
                                                                         "SO");

            // Re "sieve" category: (1) if recorded, then also published;
            // (2) never triggered.

            BloombergLP::ball::Administration::addCategory(
                                        "sieve",
                                        categoryThreshold,
                                        categoryThreshold,
                                        0,
                                        0);

            BALL_LOG_SET_CATEGORY("sieve")

            for (int jj = 0; jj < k_NUM_LEVELS; ++jj) {
                const Level attemptSev = levels[jj];

                if (Sev::e_OFF == attemptSev) {
                    continue;
                }

                int numStreamPermitted, numConstPermitted;
                bool wentOver;
                Int64 numAttempts;
                do {
                    numStreamPermitted = 0, numConstPermitted = 0;

                    const double startTime = u::doubleClock();
                    const double endTime   = startTime + attemptTime;
                    for (numAttempts = 0; u::doubleClock() < endTime;
                                                               ++numAttempts) {
                        if (true)
                            BALL_LOGTHROTTLE_BLOCK(
                                attemptSev, k_MSM, k_NPM) ++numStreamPermitted;
                        else ASSERT(0 && "else happened");

                        switch (attemptSev) {
                          case Sev::e_FATAL: {
                            if (true)
                                BALL_LOGTHROTTLE_FATAL_BLOCK(
                                             k_MSM, k_NPM) ++numConstPermitted;
                            else ASSERT(0 && "else happened");
                          } break;
                          case Sev::e_ERROR: {
                            if (true)
                                BALL_LOGTHROTTLE_ERROR_BLOCK(
                                             k_MSM, k_NPM) ++numConstPermitted;
                            else ASSERT(0 && "else happened");
                          } break;
                          case Sev::e_WARN: {
                            if (true)
                                BALL_LOGTHROTTLE_WARN_BLOCK(
                                             k_MSM, k_NPM) ++numConstPermitted;
                            else ASSERT(0 && "else happened");
                          } break;
                          case Sev::e_INFO: {
                            if (true)
                                BALL_LOGTHROTTLE_INFO_BLOCK(
                                             k_MSM, k_NPM) ++numConstPermitted;
                            else ASSERT(0 && "else happened");
                          } break;
                          case Sev::e_DEBUG: {
                            if (true)
                                BALL_LOGTHROTTLE_DEBUG_BLOCK(
                                             k_MSM, k_NPM) ++numConstPermitted;
                            else ASSERT(0 && "else happened");
                          } break;
                          case Sev::e_TRACE: {
                            if (true)
                                BALL_LOGTHROTTLE_TRACE_BLOCK(
                                             k_MSM, k_NPM) ++numConstPermitted;
                            else ASSERT(0 && "else happened");
                          } break;
                          default: {
                            ASSERTV(attemptSev, 0 && "invalid attemptLevel");
                          }
                        }
                    }

                    wentOver = u::doubleClock() > startTime + periodTime;

                    // Wait to clear any time debt left over from test.

                    u::doubleSleep(betweenTrialsTime);
                } while (wentOver);

                const Int64 expNum = attemptSev <= categoryThreshold
                                   ? bsl::min<Int64>(k_MSM, numAttempts)
                                   : 0;

                if (veryVeryVerbose) { T_;    P_(expNum);    P(attemptSev); }

                ASSERTV(categoryThreshold, attemptSev, expNum,
                                               numStreamPermitted, numAttempts,
                                                 expNum == numStreamPermitted);
                ASSERTV(categoryThreshold, attemptSev, expNum,
                                                numConstPermitted, numAttempts,
                                                  expNum == numConstPermitted);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING OSTREAM MACROS
        //
        // Concerns:
        //: 1 That the 'BALL_LOGTHROTTLE_STREAM' macro works as documented.
        //:   o The 'severity' argument is able to be passed a non-'const'
        //:     variable.
        //:
        //:   o If the 'severity' is less severe than the threshold, no trace
        //:     occurs.
        //:
        //:   o That only the specified # of traces is allowed to occur within
        //:     the specified time period.
        //:
        //: 2 That the severity-hard-coded 'BALL_LOGTHROTTLE_*' macros all work
        //:   as documented.
        //:   o If the 'severity' is less severe than the threshold, no trace
        //:     occurs.
        //:
        //:   o That only the specified # of traces is allowed to occur within
        //:     the specified time period.
        //:
        //: 3 That in buffer overflow, the full, untruncated trace appears.
        //:
        //: 4 That all of the macros, with a semicolon following the call, add
        //:   up to a single C++ statement.
        //
        // Plan:
        //: 1 In all cases, the time period chosen is one second.  The whole
        //:   test should take much less than that, which we enforce by
        //:   measuring the elapsed time and asserting it is less than 0.9
        //:   seconds.
        //:
        //: 2 Each trace is called with 'numMessagesPerPeriod = 1', and they
        //:   are called 10 times in a tight loop, and we assert that only
        //:   the first trace happened.
        //:
        //: 3 In all of the tight loops there are two traces -- in both cases,
        //:   the throttling is one trace per second, but the first trace is
        //:   with the severity less severe than the category threshold, while
        //:   the second trace is with the severity sufficiently severe to for
        //:   the trace to occur.  We verify that the first trace was
        //:   suppressed, that the second trace was not, and that the second
        //:   trace happened only on the first iteration.
        //:
        //: 4 Call all the macros to print a very long string, observe that the
        //:   string is truncated exactly as expected.
        //:
        //: 5 Call all the macros guarded by a satisfied 'if' followed by an
        //:   'else', observe that it compiles and that 'else' is not executed.
        //:   Can't put 'ASSERT(0);' in the else clause because our test driver
        //:   macros suck and don't reduce to a single statement if followed by
        //:   a semicolon.
        //
        // Testing:
        //   BALL_LOGTHROTTLE_STREAM
        //   BALL_LOGTHROTTLE_TRACE
        //   BALL_LOGTHROTTLE_DEBUG
        //   BALL_LOGTHROTTLE_INFO
        //   BALL_LOGTHROTTLE_WARN
        //   BALL_LOGTHROTTLE_ERROR
        //   BALL_LOGTHROTTLE_FATAL
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING OSTREAM MACROS\n"
                             "======================\n";

        const double start = u::doubleClock();

        const char *MESSAGE = "message:1:2:3";
        const char  SEP     = ':';
        const int   ARGS[]  = { 1, 2, 3 };

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard lmg(lmc, &ta);
        BloombergLP::ball::LoggerManager::singleton().registerObserver(TO,
                                                                       "TO");

        // Re "sieve" category: (1) if recorded, then also published;
        // (2) never triggered.

        BloombergLP::ball::Administration::addCategory(
                                "sieve",
                                BloombergLP::ball::Severity::e_TRACE,
                                BloombergLP::ball::Severity::e_TRACE,
                                0,
                                0);
        BALL_LOG_SET_CATEGORY("sieve")

        BALL_LOG_TRACE << "This will load the category";

        const Cat  *CAT  = BALL_LOG_CATEGORY;
        const char *FILE = __FILE__;

        const int TRACE = BloombergLP::ball::Severity::e_TRACE;
        const int DEBUG = BloombergLP::ball::Severity::e_DEBUG;
        const int INFO  = BloombergLP::ball::Severity::e_INFO;
        const int WARN  = BloombergLP::ball::Severity::e_WARN;
        const int ERROR = BloombergLP::ball::Severity::e_ERROR;
        const int FATAL = BloombergLP::ball::Severity::e_FATAL;

        BloombergLP::ball::Administration::addCategory(
                                     "noTRACE",
                                     BloombergLP::ball::Severity::e_TRACE - 1,
                                     BloombergLP::ball::Severity::e_TRACE - 1,
                                     BloombergLP::ball::Severity::e_TRACE - 1,
                                     BloombergLP::ball::Severity::e_TRACE - 1);

        BloombergLP::ball::Administration::addCategory(
                                     "noDEBUG",
                                     BloombergLP::ball::Severity::e_DEBUG - 1,
                                     BloombergLP::ball::Severity::e_DEBUG - 1,
                                     BloombergLP::ball::Severity::e_DEBUG - 1,
                                     BloombergLP::ball::Severity::e_DEBUG - 1);

        BloombergLP::ball::Administration::addCategory(
                                      "noINFO",
                                      BloombergLP::ball::Severity::e_INFO - 1,
                                      BloombergLP::ball::Severity::e_INFO - 1,
                                      BloombergLP::ball::Severity::e_INFO - 1,
                                      BloombergLP::ball::Severity::e_INFO - 1);

        BloombergLP::ball::Administration::addCategory(
                                      "noWARN",
                                      BloombergLP::ball::Severity::e_WARN - 1,
                                      BloombergLP::ball::Severity::e_WARN - 1,
                                      BloombergLP::ball::Severity::e_WARN - 1,
                                      BloombergLP::ball::Severity::e_WARN - 1);

        BloombergLP::ball::Administration::addCategory(
                                     "noERROR",
                                     BloombergLP::ball::Severity::e_ERROR - 1,
                                     BloombergLP::ball::Severity::e_ERROR - 1,
                                     BloombergLP::ball::Severity::e_ERROR - 1,
                                     BloombergLP::ball::Severity::e_ERROR - 1);

        BloombergLP::ball::Administration::addCategory(
                                     "noFATAL",
                                     BloombergLP::ball::Severity::e_FATAL - 1,
                                     BloombergLP::ball::Severity::e_FATAL - 1,
                                     BloombergLP::ball::Severity::e_FATAL - 1,
                                     BloombergLP::ball::Severity::e_FATAL - 1);

        Level sev = u::nextSev();

        for (int ti = 0; ti < 2; ++ti) {
            const bool  doTraces    = ti;
            int         elseCounter = 0;
            int         np;
            const Int64 SECOND = 1000 * 1000 * 1000;

            if (veryVerbose) cout << "\tTesting 'BALL_LOG_TRACE'\n";
            for (int ii = 0; ii < 10; ++ii) {
                ASSERT(TRACE == sev);
                np = TO->numPublishedRecords();

                {
                    BALL_LOG_SET_CATEGORY("noTRACE");

                    BALL_LOGTHROTTLE_STREAM(sev, 10, SECOND) << "message";
                    ASSERT(np == TO->numPublishedRecords());
                }

                const int LINE = L_ + 2;
                if (doTraces)        // deliberately no '{}'s
                    BALL_LOGTHROTTLE_STREAM(sev, 1, SECOND) <<
                                                   "message" << SEP << ARGS[0]
                                                             << SEP << ARGS[1]
                                                             << SEP << ARGS[2];
                else
                    ++elseCounter;

                ASSERTV(doTraces, np, ii, TO->numPublishedRecords(),
                      (doTraces ? np + !ii : np) == TO->numPublishedRecords());
                ASSERT(!doTraces ||
                             isRecordOkay(*TO, CAT, sev, FILE, LINE, MESSAGE));
            }

            if (veryVerbose) cout << "\tTesting 'BALL_LOG_TRACE'\n";
            for (int ii = 0; ii < 10; ++ii) {
                np = TO->numPublishedRecords();

                {
                    BALL_LOG_SET_CATEGORY("noTRACE");

                    BALL_LOGTHROTTLE_TRACE(10, SECOND) << "message";
                    ASSERT(np == TO->numPublishedRecords());
                }

                const int LINE = L_ + 2;
                if (doTraces)        // deliberately no '{}'s
                    BALL_LOGTHROTTLE_TRACE(1, SECOND) <<
                                                    "message"<< SEP << ARGS[0]
                                                             << SEP << ARGS[1]
                                                             << SEP << ARGS[2];
                else
                    ++elseCounter;

                ASSERTV(doTraces, np, ii, TO->numPublishedRecords(),
                      (doTraces ? np + !ii : np) == TO->numPublishedRecords());
                ASSERT(!doTraces ||
                           isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE));
            }

            if (veryVerbose) cout << "\tTesting 'BALL_LOG_DEBUG'\n";
            for (int ii = 0; ii < 10; ++ii) {
                np = TO->numPublishedRecords();

                {
                    BALL_LOG_SET_CATEGORY("noDEBUG");

                    BALL_LOGTHROTTLE_DEBUG(10, SECOND) << "message";
                    ASSERT(np == TO->numPublishedRecords());
                }

                const int LINE = L_ + 2;
                if (doTraces)        // deliberately no '{}'s
                    BALL_LOGTHROTTLE_DEBUG(1, SECOND)
                                                 << "message"<< SEP << ARGS[0]
                                                             << SEP << ARGS[1]
                                                             << SEP << ARGS[2];
                else
                    ++elseCounter;

                ASSERTV(doTraces, np, ii, TO->numPublishedRecords(),
                      (doTraces ? np + !ii : np) == TO->numPublishedRecords());
                ASSERT(!doTraces ||
                           isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE));
            }

            if (veryVerbose) cout << "\tTesting 'BALL_LOG_INFO'\n";
            for (int ii = 0; ii < 10; ++ii) {
                np = TO->numPublishedRecords();

                {
                    BALL_LOG_SET_CATEGORY("noINFO");

                    BALL_LOGTHROTTLE_INFO(10, SECOND) << "message";
                    ASSERT(np == TO->numPublishedRecords());
                }

                const int LINE = L_ + 2;
                if (doTraces)        // deliberately no '{}'s
                    BALL_LOGTHROTTLE_INFO( 1, SECOND) <<
                                                    "message"<< SEP << ARGS[0]
                                                             << SEP << ARGS[1]
                                                             << SEP << ARGS[2];
                else
                    ++elseCounter;

                ASSERTV(doTraces, np, ii, TO->numPublishedRecords(),
                      (doTraces ? np + !ii : np) == TO->numPublishedRecords());
                ASSERT(!doTraces ||
                            isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE));
            }

            if (veryVerbose) cout << "\tTesting 'BALL_LOG_WARN'\n";
            for (int ii = 0; ii < 10; ++ii) {
                np = TO->numPublishedRecords();

                {
                    BALL_LOG_SET_CATEGORY("noWARN")

                    BALL_LOGTHROTTLE_WARN(10, SECOND) << "message";
                    ASSERT(np == TO->numPublishedRecords());
                }

                const int LINE = L_ + 2;
                if (doTraces)        // deliberately no '{}'s
                    BALL_LOGTHROTTLE_WARN( 1, SECOND) <<
                                                    "message"<< SEP << ARGS[0]
                                                             << SEP << ARGS[1]
                                                             << SEP << ARGS[2];
                else
                    ++elseCounter;

                ASSERTV(doTraces, np, ii, TO->numPublishedRecords(),
                      (doTraces ? np + !ii : np) == TO->numPublishedRecords());
                ASSERT(!doTraces ||
                            isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE));
            }

            if (veryVerbose) cout << "\tTesting 'BALL_LOG_ERROR'" << endl;
            for (int ii = 0; ii < 10; ++ii) {
                np = TO->numPublishedRecords();

                {
                    BALL_LOG_SET_CATEGORY("noERROR")

                    BALL_LOGTHROTTLE_ERROR(10, SECOND) << "message";
                    ASSERT(np == TO->numPublishedRecords());
                }

                const int LINE = L_ + 2;
                if (doTraces)        // deliberately no '{}'s
                    BALL_LOGTHROTTLE_ERROR(1, SECOND) <<
                                                    "message"<< SEP << ARGS[0]
                                                             << SEP << ARGS[1]
                                                             << SEP << ARGS[2];
                else
                    ++elseCounter;

                ASSERTV(doTraces, np, ii, TO->numPublishedRecords(),
                      (doTraces ? np + !ii : np) == TO->numPublishedRecords());
                ASSERT(!doTraces ||
                           isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE));
            }

            if (veryVerbose) cout << "\tTesting 'BALL_LOG_FATAL'\n";
            for (int ii = 0; ii < 10; ++ii) {
                np = TO->numPublishedRecords();

                {
                    BALL_LOG_SET_CATEGORY("noFATAL")

                    BALL_LOGTHROTTLE_FATAL(10, SECOND) << "message";
                    ASSERT(np == TO->numPublishedRecords());
                }

                const int LINE = L_ + 2;
                if (doTraces)        // deliberately no '{}'s
                    BALL_LOGTHROTTLE_FATAL(1, SECOND) <<
                                                    "message"<< SEP << ARGS[0]
                                                             << SEP << ARGS[1]
                                                             << SEP << ARGS[2];
                else
                    ++elseCounter;

                ASSERTV(doTraces, np, ii, TO->numPublishedRecords(),
                      (doTraces ? np + !ii : np) == TO->numPublishedRecords());
                ASSERT(!doTraces ||
                           isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE));
            }

            {
                const int expElseCounter = doTraces ? 0 : 70;
                ASSERTV(expElseCounter, elseCounter,
                                                expElseCounter == elseCounter);
            }

            np = TO->numPublishedRecords();
            elseCounter = 0;

            if (veryVerbose) cout <<
                      "\tTesting Buffer Overflow with 'ostream' Macro" << endl;
            {
                const int                BUFLEN     =
                    LoggerManager::singleton().getLogger().messageBufferSize();
                const int                EXCESS     = 128;
                const int                NN         = BUFLEN + EXCESS;
                char                    *longString = (char *) ta.allocate(NN);
                char                    *cpyString  = (char *) ta.allocate(NN);

                BloombergLP::bslma::ManagedPtr<char>  managedLongString(
                                                                    longString,
                                                                    &ta);
                BloombergLP::bslma::ManagedPtr<char>  managedCpyString(
                                                                     cpyString,
                                                                     &ta);

                bsl::fill(longString + 0, longString + NN, 'x');
                longString[NN - 1] = '\0';
                bsl::strcpy(cpyString, longString);

                np += doTraces;
                for (int ii = 0; ii < 10; ++ii) {
                    ASSERT('x' == longString[BUFLEN - 1]);

                    const int LINE = L_ + 2;
                    if (doTraces)                    // deliberately no '{}'
                        BALL_LOGTHROTTLE_TRACE(1, SECOND) << longString;
                    else
                        ++elseCounter;

                    ASSERT(0 == bsl::strcmp(cpyString, longString));
                    ASSERTV(doTraces, ii, np, TO->numPublishedRecords(),
                                              np == TO->numPublishedRecords());
                    ASSERT(!doTraces ||
                        isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
                }

                np += doTraces;
                for (int ii = 0; ii < 10; ++ii) {
                    ASSERT('x' == longString[BUFLEN - 1]);

                    const int LINE = L_ + 2;
                    if (doTraces)                    // deliberately no '{}'
                        BALL_LOGTHROTTLE_DEBUG(1, SECOND) << longString;
                    else
                        ++elseCounter;

                    ASSERT(0 == bsl::strcmp(cpyString, longString));
                    ASSERTV(doTraces, ii, np, TO->numPublishedRecords(),
                                              np == TO->numPublishedRecords());
                    ASSERT(!doTraces ||
                        isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
                }

                np += doTraces;
                for (int ii = 0; ii < 10; ++ii) {
                    ASSERT('x' == longString[BUFLEN - 1]);

                    const int LINE = L_ + 2;
                    if (doTraces)                    // deliberately no '{}'
                        BALL_LOGTHROTTLE_INFO(1, SECOND) << longString;
                    else
                        ++elseCounter;

                    ASSERT(0 == bsl::strcmp(cpyString, longString));
                    ASSERTV(doTraces, ii, np, TO->numPublishedRecords(),
                                              np == TO->numPublishedRecords());
                    ASSERT(!doTraces ||
                         isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
                }

                np += doTraces;
                for (int ii = 0; ii < 10; ++ii) {
                    ASSERT('x' == longString[BUFLEN - 1]);

                    const int LINE = L_ + 2;
                    if (doTraces)                    // deliberately no '{}'
                        BALL_LOGTHROTTLE_WARN(1, SECOND) << longString;
                    else
                        ++elseCounter;

                    ASSERT(0 == bsl::strcmp(cpyString, longString));
                    ASSERTV(doTraces, ii, np, TO->numPublishedRecords(),
                                              np == TO->numPublishedRecords());
                    ASSERT(!doTraces ||
                         isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
                }

                np += doTraces;
                for (int ii = 0; ii < 10; ++ii) {
                    ASSERT('x' == longString[BUFLEN - 1]);

                    const int LINE = L_ + 2;
                    if (doTraces)                    // deliberately no '{}'
                        BALL_LOGTHROTTLE_ERROR(1, SECOND) << longString;
                    else
                        ++elseCounter;

                    ASSERT(0 == bsl::strcmp(cpyString, longString));
                    ASSERTV(doTraces, ii, np, TO->numPublishedRecords(),
                                              np == TO->numPublishedRecords());
                    ASSERT(!doTraces ||
                        isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
                }

                np += doTraces;
                for (int ii = 0; ii < 10; ++ii) {
                    ASSERT('x' == longString[BUFLEN - 1]);

                    const int LINE = L_ + 2;
                    if (doTraces)                    // deliberately no '{}'
                        BALL_LOGTHROTTLE_FATAL(1, SECOND) << longString;
                    else
                        ++elseCounter;

                    ASSERT(0 == bsl::strcmp(cpyString, longString));
                    ASSERTV(doTraces, ii, np, TO->numPublishedRecords(),
                                              np == TO->numPublishedRecords());
                    ASSERT(!doTraces ||
                        isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
                }

                {
                    const int expElseCounter = doTraces ? 0 : 60;
                    ASSERTV(expElseCounter, elseCounter,
                                                expElseCounter == elseCounter);
                }
            }
        }

        const double elapsed = u::doubleClock() - start;
        ASSERTV(elapsed, elapsed < 0.2);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING PRINTF-STYLE MACROS
        //
        // Concerns:
        //: 1 That the 'BALL_LOGTHROTTLEVA' macro works as documented.
        //:   o The 'severity' argument is able to be passed a non-'const'
        //:     variable.
        //:
        //:   o If the 'severity' is less severe than the threshold, no trace
        //:     occurs.
        //:
        //:   o That only the specified # of traces is allowed to occur within
        //:     the specified time period.
        //:
        //: 2 That the severity-hard-coded 'BALL_LOGTHROTTLEVA_*' macros all
        //:   work as documented.
        //:   o If the 'severity' is less severe than the threshold, no trace
        //:     occurs.
        //:
        //:   o That only the specified # of traces is allowed to occur within
        //:     the specified time period.
        //:
        //: 3 That in buffer overflow, the trace is truncated as expected.
        //:
        //: 4 That all of the macros, with a semicolon following the call, add
        //:   up to a single C++ statement.
        //
        // Plan:
        //: 1 In all cases, the time period chosen is one second.  The whole
        //:   test should take much less than that, which we enforce by
        //:   measuring the elapsed time and asserting it is less than 0.9
        //:   seconds.
        //:
        //: 2 Each trace is called with 'numMessagesPerPeriod = 1', and they
        //:   are called 10 times in a tight loop, and we assert that only
        //:   the first trace happened.
        //:
        //: 3 In all of the tight loops there are two traces -- in both cases,
        //:   the throttling is one trace per second, but the first trace is
        //:   with the severity less severe than the category threshold, while
        //:   the second trace is with the severity sufficiently severe to for
        //:   the trace to occur.  We verify that the first trace was
        //:   suppressed, that the second trace was not, and that the second
        //:   trace happened only on the first iteration.
        //:
        //: 4 Call all the macros to print a very long string, observe that the
        //:   string is truncated exactly as expected.
        //:
        //: 5 Call all the macros guarded by a satisfied 'if' followed by an
        //:   'else', observe that it compiles and that 'else' is not executed.
        //:   Can't put 'ASSERT(0);' in the else clause because our test driver
        //:   macros suck and don't reduce to a single statement if followed by
        //:   a semicolon.
        //
        // Testing:
        //   BALL_LOGTHROTTLEVA
        //   BALL_LOGTHROTTLEVA_TRACE
        //   BALL_LOGTHROTTLEVA_DEBUG
        //   BALL_LOGTHROTTLEVA_INFO
        //   BALL_LOGTHROTTLEVA_WARN
        //   BALL_LOGTHROTTLEVA_ERROR
        //   BALL_LOGTHROTTLEVA_FATAL
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING PRINTF-STYLE MACROS\n"
                             "===========================\n";

        const double start = u::doubleClock();

        BloombergLP::bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        BloombergLP::bslma::DefaultAllocatorGuard taGuard(&testAllocator);

        const int MAX_ARGS = 9;

        #define FORMAT_SPEC_0_ARGS "message"
        #define FORMAT_SPEC_1_ARGS "message:%d"
        #define FORMAT_SPEC_2_ARGS "message:%d:%d"
        #define FORMAT_SPEC_3_ARGS "message:%d:%d:%d"
        #define FORMAT_SPEC_4_ARGS "message:%d:%d:%d:%d"
        #define FORMAT_SPEC_5_ARGS "message:%d:%d:%d:%d:%d"
        #define FORMAT_SPEC_6_ARGS "message:%d:%d:%d:%d:%d:%d"
        #define FORMAT_SPEC_7_ARGS "message:%d:%d:%d:%d:%d:%d:%d"
        #define FORMAT_SPEC_8_ARGS "message:%d:%d:%d:%d:%d:%d:%d:%d"
        #define FORMAT_SPEC_9_ARGS "message:%d:%d:%d:%d:%d:%d:%d:%d:%d"

        const char *MESSAGE[] = {
            "message",
            "message:1",
            "message:1:2",
            "message:1:2:3",
            "message:1:2:3:4",
            "message:1:2:3:4:5",
            "message:1:2:3:4:5:6",
            "message:1:2:3:4:5:6:7",
            "message:1:2:3:4:5:6:7:8",
            "message:1:2:3:4:5:6:7:8:9"
        };
        ASSERT(MAX_ARGS + 1 == sizeof MESSAGE / sizeof *MESSAGE);

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard lmg(lmc, &ta);
        BloombergLP::ball::LoggerManager::singleton().registerObserver(TO,
                                                                       "TO");

        const int NONE  = BloombergLP::ball::Severity::e_TRACE + 1;
        const int TRACE = BloombergLP::ball::Severity::e_TRACE;
        const int DEBUG = BloombergLP::ball::Severity::e_DEBUG;
        const int INFO  = BloombergLP::ball::Severity::e_INFO;
        const int WARN  = BloombergLP::ball::Severity::e_WARN;
        const int ERROR = BloombergLP::ball::Severity::e_ERROR;
        const int FATAL = BloombergLP::ball::Severity::e_FATAL;

        BloombergLP::ball::Administration::addCategory("pass", 0, TRACE, 0, 0);

        // Re "sieve" category: (1) if recorded, then also published;
        // (2) never triggered.

        BloombergLP::ball::Administration::addCategory("sieve",
                                                       TRACE,
                                                       TRACE,
                                                       0,
                                                       0);

        BloombergLP::ball::Administration::addCategory(
                                   "noTRACE",
                                   BloombergLP::ball::Severity::e_TRACE - 1,
                                   BloombergLP::ball::Severity::e_TRACE - 1,
                                   BloombergLP::ball::Severity::e_TRACE - 1,
                                   BloombergLP::ball::Severity::e_TRACE - 1);

        BloombergLP::ball::Administration::addCategory(
                                  "noDEBUG",
                                  BloombergLP::ball::Severity::e_DEBUG - 1,
                                  BloombergLP::ball::Severity::e_DEBUG - 1,
                                  BloombergLP::ball::Severity::e_DEBUG - 1,
                                  BloombergLP::ball::Severity::e_DEBUG - 1);

        BloombergLP::ball::Administration::addCategory(
                                  "noINFO",
                                  BloombergLP::ball::Severity::e_INFO - 1,
                                  BloombergLP::ball::Severity::e_INFO - 1,
                                  BloombergLP::ball::Severity::e_INFO - 1,
                                  BloombergLP::ball::Severity::e_INFO - 1);

        BloombergLP::ball::Administration::addCategory(
                                   "noWARN",
                                   BloombergLP::ball::Severity::e_WARN - 1,
                                   BloombergLP::ball::Severity::e_WARN - 1,
                                   BloombergLP::ball::Severity::e_WARN - 1,
                                   BloombergLP::ball::Severity::e_WARN - 1);

        BloombergLP::ball::Administration::addCategory(
                                  "noERROR",
                                  BloombergLP::ball::Severity::e_ERROR - 1,
                                  BloombergLP::ball::Severity::e_ERROR - 1,
                                  BloombergLP::ball::Severity::e_ERROR - 1,
                                  BloombergLP::ball::Severity::e_ERROR - 1);

        BloombergLP::ball::Administration::addCategory(
                                   "noFATAL",
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1);

        const int   n = 1;                     // numEventsPerPeriod
        const Int64 p = 1000 * 1000 * 1000;    // NANOSECONDS_PER_MESSAGE
        Level       sev;

        BALL_LOG_SET_CATEGORY("sieve")

        BALL_LOG_TRACE << "This will load the category";

        const Cat  *CAT  = BALL_LOG_CATEGORY;
        const char *FILE = __FILE__;

        if (verbose) cout <<
                           "Now test the variadic '*_LOGTHROTTLEVA' macros"
                           " with non-'const' severity & varying arguments.\n";

        ASSERT(!bsl::strcmp("sieve", CAT->categoryName()));

        int jj = 0, np = TO->numPublishedRecords() + 1; sev = u::nextSev();
        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA(" <<
                                                   sev << ") - " << jj << endl;
        for (int ii = 0; ii < 10; ++ii) {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            BALL_LOGTHROTTLEVA(NONE, n, p, FORMAT_SPEC_0_ARGS);
            ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA(sev, n, p, FORMAT_SPEC_0_ARGS);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, sev, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        ++jj; ++np; sev = u::nextSev();
        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA(" <<
                                                   sev << ") - " << jj << endl;
        for (int ii = 0; ii < 10; ++ii) {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            BALL_LOGTHROTTLEVA(NONE, n, p, FORMAT_SPEC_1_ARGS, 1);
            ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA(sev, n, p, FORMAT_SPEC_1_ARGS, 1);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, sev, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        ++jj; ++np; sev = u::nextSev();
        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA(" <<
                                                   sev << ") - " << jj << endl;
        for (int ii = 0; ii < 10; ++ii) {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            BALL_LOGTHROTTLEVA(NONE, n, p, FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA(sev, n, p, FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, sev, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        ++jj; ++np; sev = u::nextSev();
        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA(" <<
                                                   sev << ") - " << jj << endl;
        for (int ii = 0; ii < 10; ++ii) {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            BALL_LOGTHROTTLEVA(NONE, n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA(sev, n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, sev, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        ++jj; ++np; sev = u::nextSev();
        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA(" <<
                                                   sev << ") - " << jj << endl;
        for (int ii = 0; ii < 10; ++ii) {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            BALL_LOGTHROTTLEVA(NONE, n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA(sev, n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, sev, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        ++jj; ++np; sev = u::nextSev();
        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA(" <<
                                                   sev << ") - " << jj << endl;
        for (int ii = 0; ii < 10; ++ii) {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            BALL_LOGTHROTTLEVA(NONE, n, p, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA(sev, n, p, FORMAT_SPEC_5_ARGS, 1,2,3,4,5);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, sev, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        ++jj; ++np; sev = u::nextSev();
        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA(" <<
                                                   sev << ") - " << jj << endl;
        for (int ii = 0; ii < 10; ++ii) {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            BALL_LOGTHROTTLEVA(NONE, n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5,
                                                                            6);
            ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA(sev, n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5,
                                                                            6);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, sev, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        ++jj; ++np; sev = u::nextSev();
        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA(" <<
                                                   sev << ") - " << jj << endl;
        for (int ii = 0; ii < 10; ++ii) {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            BALL_LOGTHROTTLEVA(NONE, n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5,
                                                                         6, 7);
            ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA(sev, n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6,
                                                                            7);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, sev, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        ++jj; ++np; sev = u::nextSev();
        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA(" <<
                                                   sev << ") - " << jj << endl;
        for (int ii = 0; ii < 10; ++ii) {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            BALL_LOGTHROTTLEVA(NONE, n, p, FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5,
                                                                      6, 7, 8);
            ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA(sev, n, p, FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6,
                                                                         7, 8);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, sev, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        ++jj; ++np; sev = u::nextSev();
        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA(" <<
                                                   sev << ") - " << jj << endl;
        for (int ii = 0; ii < 10; ++ii) {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            BALL_LOGTHROTTLEVA(NONE, n, p, FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5,
                                                                      6,7,8,9);
            ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA(sev, n, p, FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5,
                                                                      6,7,8,9);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, sev, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (verbose) cout << "Now test the variadic"
              " '*_LOGTHROTTLEVA_<SEVERITY>' macros with varying arguments.\n";

        jj = -1;

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_TRACE - 0\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_0_ARGS);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_0_ARGS);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_TRACE - 1\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_1_ARGS, 1);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_1_ARGS, 1);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_TRACE - 2\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_TRACE - 3\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_TRACE - 4\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_TRACE - 5\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4,
                                                                            5);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_TRACE - 6\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4,
                                                                         5, 6);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5,
                                                                            6);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_TRACE - 7\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4,
                                                                       5, 6,7);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5,
                                                                         6, 7);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_TRACE - 8\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_8_ARGS, 1,2,3,4,5,6,
                                                                          7,8);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_8_ARGS, 1,2,3,4,5,6,7,
                                                                            8);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_TRACE - 9\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_9_ARGS, 1,2,3,4,5,6,
                                                                        7,8,9);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_9_ARGS, 1,2,3,4,5,6,7,8,
                                                                            9);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        jj = -1;

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_DEBUG - 0\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_0_ARGS);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_0_ARGS);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_DEBUG - 1\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_1_ARGS, 1);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_1_ARGS, 1);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_DEBUG - 2\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_DEBUG - 3\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_DEBUG - 4\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_DEBUG - 5\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4,
                                                                            5);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_DEBUG - 6\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4,
                                                                         5, 6);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }


            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5,
                                                                            6);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_DEBUG - 7\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4,
                                                                       5, 6,7);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5,
                                                                         6, 7);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_DEBUG - 8\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_8_ARGS, 1,2,3,4,5,6,
                                                                          7,8);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_8_ARGS, 1,2,3,4,5,6,7,
                                                                            8);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_DEBUG - 9\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_9_ARGS, 1,2,3,4,5,6,
                                                                        7,8,9);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_9_ARGS, 1,2,3,4,5,6,7,8,
                                                                            9);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        jj = -1;

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_INFO - 0\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_0_ARGS);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_0_ARGS);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_INFO - 1\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_1_ARGS, 1);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_1_ARGS, 1);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_INFO - 2\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_INFO - 3\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_INFO - 4\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_INFO - 5\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4,
                                                                            5);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_INFO - 6\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4,
                                                                         5, 6);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5,
                                                                            6);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_INFO - 7\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4,
                                                                       5, 6,7);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6,
                                                                            7);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_INFO - 8\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_8_ARGS, 1,2,3,4,5,6,
                                                                          7,8);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_8_ARGS, 1,2,3,4,5,6,7,8);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_INFO - 9\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_9_ARGS, 1,2,3,4,5,6,
                                                                        7,8,9);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_9_ARGS, 1,2,3,4,5,6,7,8,
                                                                            9);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        jj = -1;

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_WARN - 0\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_0_ARGS);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_0_ARGS);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_WARN - 1\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_1_ARGS, 1);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_1_ARGS, 1);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_WARN - 2\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_WARN - 3\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_WARN - 4\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_WARN - 5\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4,
                                                                            5);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_WARN - 6\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4,
                                                                         5, 6);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5,
                                                                            6);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_WARN - 7\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4,
                                                                       5, 6,7);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6,
                                                                            7);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_WARN - 8\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_8_ARGS, 1,2,3,4,5,6,
                                                                          7,8);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_8_ARGS, 1,2,3,4,5,6,7,8);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_WARN - 9\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_9_ARGS, 1,2,3,4,5,6,
                                                                        7,8,9);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_9_ARGS, 1,2,3,4,5,6,7,8,
                                                                            9);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        jj = -1;

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_ERROR - 0\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_0_ARGS);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_0_ARGS);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_ERROR - 1\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_1_ARGS, 1);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_1_ARGS, 1);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_ERROR - 2\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_ERROR - 3\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_ERROR - 4\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_ERROR - 5\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4,
                                                                            5);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_ERROR - 6\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4,
                                                                         5, 6);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5,
                                                                            6);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_ERROR - 7\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4,
                                                                       5, 6,7);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5,
                                                                         6, 7);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_ERROR - 8\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_8_ARGS, 1,2,3,4,5,6,
                                                                          7,8);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_8_ARGS, 1,2,3,4,5,6,7,
                                                                            8);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_ERROR - 9\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_9_ARGS, 1,2,3,4,5,6,
                                                                        7,8,9);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_9_ARGS, 1,2,3,4,5,6,7,8,
                                                                            9);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        jj = -1;

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_FATAL - 0\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_0_ARGS);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_0_ARGS);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_FATAL - 1\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_1_ARGS, 1);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_1_ARGS, 1);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_FATAL - 2\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_FATAL - 3\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_FATAL - 4\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_FATAL - 5\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4,
                                                                            5);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + 1;
            BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_FATAL - 6\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4,
                                                                         5, 6);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5,
                                                                            6);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_FATAL - 7\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4,
                                                                       5, 6,7);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5,
                                                                         6, 7);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_FATAL - 8\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_8_ARGS, 1,2,3,4,5,6,
                                                                          7,8);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_8_ARGS, 1,2,3,4,5,6,7,
                                                                            8);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout << "BALL_LOGTHROTTLEVA_FATAL - 9\n";
        ++jj; ++np;
        for (int ii = 0; ii < 10; ++ii) {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
                BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_9_ARGS, 1,2,3,4,5,6,
                                                                        7,8,9);
                ASSERT(PREVIOUS_RECORD == TO->lastPublishedRecord());
            }

            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                                     TO->lastPublishedRecord();
            const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
            BALL_LOGTHROTTLEVA_FATAL(n, p, FORMAT_SPEC_9_ARGS, 1,2,3,4,5,6,7,8,
                                                                            9);
            ASSERTV(ii,
                    !!ii == (PREVIOUS_RECORD == TO->lastPublishedRecord()));
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[jj]));
            ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
        }

        if (veryVerbose) cout <<
                         "Truncation on buffer overflow with 'printf' macro\n";
        {
            const int                BUFLEN     =
                    LoggerManager::singleton().getLogger().messageBufferSize();
            const int                EXCESS     = 128;
            const int                NN         = BUFLEN + EXCESS;
            char                    *longString = (char *) ta.allocate(NN);

            BloombergLP::bslma::ManagedPtr<char> managedString(longString,
                                                               &ta);

            bsl::fill(longString + 0, longString + NN, 'x');
            longString[NN - 1] = '\0';

            // Severity passed as arg

            int severity = DEBUG;
            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'a';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA(severity, n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            severity = INFO;
            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'b';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA(severity, n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            severity = WARN;
            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'c';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA(severity, n, p, "%s %d", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            severity = ERROR;
            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'j';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
                BALL_LOGTHROTTLEVA(severity, n, p,"%s %d %d %d %d %d %d %d %d",
                                           longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            // printf-style macros with severity hard coded

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'k';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA_TRACE(n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'l';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA_TRACE(n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'j';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
                BALL_LOGTHROTTLEVA_TRACE(n, p, "%s %d %d %d %d %d %d %d %d",
                                           longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'k';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA_DEBUG(n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'l';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA_DEBUG(n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'j';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
                BALL_LOGTHROTTLEVA_DEBUG(n, p, "%s %d %d %d %d %d %d %d %d",
                                           longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'k';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA_INFO(n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'l';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA_INFO(n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'j';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
                BALL_LOGTHROTTLEVA_INFO(n, p, "%s %d %d %d %d %d %d %d %d",
                                           longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'k';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA_WARN(n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'l';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA_WARN(n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'j';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
                BALL_LOGTHROTTLEVA_WARN(n, p, "%s %d %d %d %d %d %d %d %d",
                                           longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'k';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA_ERROR(n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'l';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA_ERROR(n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'j';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
                BALL_LOGTHROTTLEVA_ERROR(n, p, "%s %d %d %d %d %d %d %d %d",
                                           longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'k';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA_FATAL(n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'l';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGTHROTTLEVA_FATAL(n, p, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }

            ++np;
            for (int ii = 0; ii < 10; ++ii) {
                longString[BUFLEN - 2] = 'j';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + (k_HAS_MULTILINE_OFFSET? 2 : 1);
                BALL_LOGTHROTTLEVA_FATAL(n, p, "%s %d %d %d %d %d %d %d %d",
                                           longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
                ASSERTV(np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            }
        }

        // Note that the following is expressly meant to test the modifications
        // made to the 'printf'-style macros to use the 'do { ... } while(0)'
        // idiom (DRQS 13261698).  In particular, the 'if' statements are
        // *INTENTIONALLY* *NOT* fully bracketed ('{}'-enclosed), contrary to
        // the BDE coding standard.

        if (veryVerbose)
            cout << "\tTesting macros in unbracketed context."
                 << endl;
        for (int ti = 0; ti < 2; ++ti) {
            const bool doTraces = ti;
            int        numElses = 0;

            {
                np += doTraces;
                const int LINE = L_ + 2;
                if (doTraces)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGTHROTTLEVA_TRACE(n, p, FORMAT_SPEC_0_ARGS);
                else
                    ++numElses;

                ASSERT(!doTraces ||
                        isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[0]));
            }

            {
                np += doTraces;
                const int LINE = L_ + 2;
                if (doTraces)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGTHROTTLEVA_DEBUG(n, p, FORMAT_SPEC_1_ARGS, 1);
                else
                    ++numElses;

                ASSERT(!doTraces ||
                        isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[1]));
            }

            {
                np += doTraces;
                const int LINE = L_ + 2;
                if (doTraces)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGTHROTTLEVA_INFO(n, p, FORMAT_SPEC_2_ARGS, 1, 2);
                else
                    ++numElses;

                ASSERT(!doTraces ||
                         isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[2]));
            }

            {
                np += doTraces;
                const int LINE = L_ + 2;
                if (doTraces)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGTHROTTLEVA_WARN(n, p, FORMAT_SPEC_3_ARGS, 1, 2, 3);
                else
                    ++numElses;

                ASSERT(!doTraces ||
                         isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[3]));
            }

            {
                np += doTraces;
                const int LINE = L_ + 1 + (k_HAS_MULTILINE_OFFSET? 2 : 1);
                if (doTraces)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGTHROTTLEVA_ERROR(n, p, FORMAT_SPEC_4_ARGS, 1, 2, 3,
                                                                            4);
                else
                    ++numElses;

                ASSERT(!doTraces ||
                        isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[4]));
            }

            {
                np += doTraces;
                const int LINE = L_ + 1 + (k_HAS_MULTILINE_OFFSET? 2 : 1);
                if (doTraces)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGTHROTTLEVA_FATAL(n, p,  FORMAT_SPEC_5_ARGS, 1, 2,
                                                                      3, 4, 5);
                else
                    ++numElses;

                ASSERT(!doTraces ||
                        isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[5]));
            }

            ASSERTV(doTraces, np, TO->numPublishedRecords(),
                                              TO->numPublishedRecords() == np);
            const int expNumElses = doTraces ? 0 : 6;
            ASSERTV(expNumElses, numElses, expNumElses == numElses);
        }

        const double elapsed = u::doubleClock() - start;
        ASSERTV(elapsed, elapsed < 0.9);
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // YIELDING RESULTS OF RADIATION METER
        //
        // Concerns:
        //: 1 Unit-test the radiation meter used in both usage examples.
        //
        // Plan:
        //: 2 Run the meter and print out the results.
        // --------------------------------------------------------------------

        if (verbose) cout << "YIELDING RESULTS OF RADIATION METER\n"
                             "===================================\n";

        Usage::RadiationMeterReceiver receiver;
        bool firstTime = true;
        double value;
        while (-1.0 != (value = receiver.yield())) {
            if (firstTime) {
                firstTime = false;
            }
            else {
                cout << ' ';
            }
            cout << value;
        }
        cout << endl;
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

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
