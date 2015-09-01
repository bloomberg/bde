// bdlt_currenttime.t.cpp                                             -*-C++-*-
#include <bdlt_currenttime.h>

#include <bdlt_datetimeinterval.h>
#include <bdlt_epochutil.h>
#include <bdlt_intervalconversionutil.h>
#include <bdlt_localtimeoffset.h>
#include <bdlt_timeunitratio.h>

#include <bslim_testutil.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>

#include <bsls_systemtime.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <bsl_ctime.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// Testing of 'bdlt::CurrentTime' consists of verifying that the various time
// methods return non-decreasing values, including when called many times in
// tight loops.  Additionally, the tests verify that the current-time callback
// can be set, retrieved, and invoked correctly.
//
// ----------------------------------------------------------------------------
// [ 6] DatetimeTz asDatetimeTz()
// [ 5] Datetime local()
// [ 4] bsls::TimeInterval now()
// [ 4] Datetime utc()
// [ 2] CurrentTimeCallback currentTimeCallback()
// [ 2] CurrentTimeCallback setCurrentTimeCallback(CurrentTimeCallback)
// [ 1] bsls::TimeInterval currentTimeDefault()
// ----------------------------------------------------------------------------
// [10] USAGE EXAMPLE
// [ 9] Datetime local() stress test
// [ 7] bsls::TimeInterval now() stress test
// [ 8] Datetime utc() stress test
// [ 3] static int inOrder(INTERVAL_L lhs, INTERVAL_R rhs)

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlt::CurrentTime Util;

// ============================================================================
//                      HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

bsls::TimeInterval IncrementInterval()
    // Return a 'bsls::TimeInterval' constructed from an internal value which
    // is incremented on each call.
{
    static int value;

    bsls::TimeInterval result(value, value);

    ++value;

    return result;
}

bsls::TimeInterval getClientTime()
    // Return a fixed 'bsls::TimeInterval' value.
{
    return bsls::TimeInterval(1, 1);
}

bsls::TimeInterval f1()
    // Return a fixed 'bsls::TimeInterval' value.
{
    return bsls::TimeInterval(1, 1);
}

bsls::TimeInterval f2()
    // Return a fixed 'bsls::TimeInterval' value.
{
    return bsls::TimeInterval(1, 1);
}

bsls::TimeInterval getClientOffset(const bdlt::Datetime&)
     // Return a fixed 'bsls::TimeInterval' for a local time offset.
{
    return bsls::TimeInterval(60 * 60, 0);
}


bool within1Sec(bdlt::Datetime a, bdlt::Datetime b)
   // Return 'true' if 'a' and 'b' are within one second of each other.
{
    return (a - b).totalSeconds() == 0;
}


template <class INTERVAL_L, class INTERVAL_R>
static int inOrder(const INTERVAL_L& lhs, const INTERVAL_R& rhs)
    // Return 1, 0, -1 accordingly as the specified 'lhs' is less than, equal
    // to, or greater than, the specified 'rhs'.
{
    bsls::Types::Int64 tml = lhs.totalMilliseconds();
    bsls::Types::Int64 tmr = rhs.totalMilliseconds();
    return tml < tmr ? 1 : tml == tmr ? 0 : -1;
}

static void putAxis(unsigned width)
    // Output an axis of the specified 'width' for a progress bar.
{
    const char P1[] = "0%";
    const char P2[] = "50%";
    const char P3[] = "100%";

    unsigned hl = width / 2;

    cout << P1;
    for (unsigned i = sizeof(P1) + sizeof(P2); i < hl + 4; ++i) {
        cout << "-";
    }
    cout << P2;
    for (unsigned i = sizeof(P3); i < hl; ++i) {
        cout << "-";
    }
    cout << P3 << endl;
}

typedef void *(*ThreadFunction)(void *);  // type of a thread function

#ifdef BSLS_PLATFORM_OS_WINDOWS

typedef HANDLE ThreadId;

static ThreadId createThread(ThreadFunction function, void *argument)
    // Create a thread, invoking the specified 'function' with the specified
    // 'argument' and return the id of the thread.
{
  return CreateThread(0, 0,
                      reinterpret_cast<LPTHREAD_START_ROUTINE>(function),
                      argument,
                      0, 0);
}

static void joinThread(ThreadId id)
    // Wait until the thread with the specified 'id' is finished.
{
    WaitForSingleObject(id, INFINITE);
    CloseHandle(id);
}

#else

typedef pthread_t ThreadId;

static ThreadId createThread(ThreadFunction function, void *argument)
    // Create a thread, invoking the specified 'function' with the specified
    // 'argument' and return the id of the thread.
{
    ThreadId id;
    pthread_create(&id, 0, function, argument);
    return id;
}

static void joinThread(ThreadId id)
    // Wait until the thread with the specified 'id' is finished.
{
     pthread_join(id, 0);
}

#endif

struct ThreadInfo {
    // Data passed to thread function.
    Util::CurrentTimeCallback set;      // Set this as the callback.
    Util::CurrentTimeCallback other;    // Expect this as the callback too.
    int                       count;    // Set callback this many times.
    bool                      verbose;  // Print message on each iteration.
    int                       index;    // Index for this info.
};

static bsls::AtomicOperations::AtomicTypes::Int go = { 0 };

static void *threadFunction(void *argument)
    // Using the 'ThreadInfo' data specified by 'argument', invoke the set and
    // get callback methods of 'bdlt::CurrentTime'.
{
    while (!bsls::AtomicOperations::getInt(&go)) {
        // Wait for the starting gun.
    }

    ThreadInfo *ti = static_cast<ThreadInfo *>(argument);

    Util::setCurrentTimeCallback(ti->set);

    for (int i = 0; i < ti->count; ++i) {
        if (ti->verbose) { P_(ti->index) P(i) }
        Util::CurrentTimeCallback old = Util::setCurrentTimeCallback(ti->set);
        Util::CurrentTimeCallback current = Util::currentTimeCallback();
        ASSERT(ti->set == old     || ti->other == old);
        ASSERT(ti->set == current || ti->other == current);
    }

    return argument;
}

// ============================================================================
//                             USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Repeatable Tests Involving Current Time
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are writing an application which involves dealing with the
// current time (for example, a clock displaying it).  In order to test the
// application, we would like to be able to control the time it sees.  We can
// use 'bdlt::CurrentTime' for this purpose.
//
// First, we create a sample application.  For this example, we simply have it
// retrieve the current time in several formats:
//..
void sampleApplication(bdlt::Datetime     *local,
                       bdlt::Datetime     *utc,
                       bsls::TimeInterval *now)
    // Retrieve versions of the current time into the specified 'local',
    // 'utc', and 'now' parameters.
{
    *local = bdlt::CurrentTime::local();
    *utc   = bdlt::CurrentTime::utc();
    *now   = bdlt::CurrentTime::now();
}
//..
// Then, we create a method to test whether the application is producing the
// expected results:
//..
bool checkApplication(bdlt::Datetime     expectedLocal,
                      bdlt::Datetime     expectedUtc,
                      bsls::TimeInterval expectedNow)
    // Return 'true' if 'sampleApplication' returns values matching the
    // specified expected values 'expectedLocal', 'expectedUtc', and
    // 'expectedNow'.
{
    bdlt::Datetime     local;
    bdlt::Datetime     utc;
    bsls::TimeInterval now;

    sampleApplication(&local, &utc, &now);
    return expectedLocal == local &&
           expectedUtc   == utc   &&
           expectedNow   == now;
}
//..
// Next, we create a class allowing us to set the current time which will be
// seen by the application:
//..
struct TestCurrentTimeGuard
    // Maintain and return a "current time" value.
{
    TestCurrentTimeGuard();
        // Create an object of this type, installing the handler.

    ~TestCurrentTimeGuard();
        // Destroy an object of this type, restoring the handler.

    static bsls::TimeInterval time();
        // Return 's_time'.

    bdlt::CurrentTime::CurrentTimeCallback d_prev;  // old callback
    static bsls::TimeInterval s_time;               // the "current time"
};

bsls::TimeInterval TestCurrentTimeGuard::s_time;

TestCurrentTimeGuard::TestCurrentTimeGuard()
: d_prev(bdlt::CurrentTime::setCurrentTimeCallback(time))
{
}

TestCurrentTimeGuard::~TestCurrentTimeGuard()
{
    bdlt::CurrentTime::setCurrentTimeCallback(d_prev);
}

bsls::TimeInterval TestCurrentTimeGuard::time()
{
    return s_time;
}
//..
// Finally, we write a method that tests that our application is functioning
// correctly:
//..
void testApplication()
    // Test the application.
{
    TestCurrentTimeGuard tct;
    TestCurrentTimeGuard::s_time = bdlt::EpochUtil::convertToTimeInterval(
                                                 bdlt::EpochUtil::epoch());

    bdlt::Datetime     local;
    bdlt::Datetime     utc;
    bsls::TimeInterval now;

    sampleApplication(&local, &utc, &now);

    TestCurrentTimeGuard::s_time += 1E6;
    local.addSeconds(1000000);
    utc.addSeconds(1000000);
    now += 1E6;

    ASSERT(checkApplication(local, utc, now));
}
//..

// ============================================================================
//                          MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)     veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        testApplication();
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'local' METHOD - STRESS TESTING FOR MONOTONICITY
        //
        // Concerns:
        //: 1 'local' returns non-decreasing values when run in a loop a large
        //:   number of times.
        //
        // Plan:
        //: 1 Run 'local' in a loop a large, configurable, number of times, and
        //:   verify that its result is non-decreasing.
        //
        // Testing:
        //   Datetime local() stress test
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'local' METHOD - STRESS TESTING FOR MONOTONICITY"
                 << endl
                 << "========================================================"
                 << endl;
        {
            bdlt::DatetimeInterval TOLERANCE(0, 0, 0, 0, 1); // 1ms

            enum {
                NUM_ITERATIONS = 120,
                NUM_TEST_PER_ITERATION = 1000,
                OUTPUT_WIDTH = 60,
                OUTPUT_FREQ = NUM_ITERATIONS / OUTPUT_WIDTH
            };

            int iterations = verbose ? atoi(argv[2]) : NUM_ITERATIONS;
            int output_freq = iterations < OUTPUT_WIDTH ? 1
                            : iterations / OUTPUT_WIDTH;
            int testsPerIteration = NUM_TEST_PER_ITERATION;

            if (veryVerbose) {
                testsPerIteration = atoi(argv[3]);
                putAxis(OUTPUT_WIDTH);
            }

            for (int i = 0; i < iterations; ++i) {
                bdlt::Datetime prev = Util::local();
                for (int j = 0; j < testsPerIteration; ++j) {
                    bdlt::Datetime now = Util::local();
                    if (veryVeryVerbose) { P_(prev) P(now) }
                    if (prev > now) {
                        cout << "*** Warning: system time is not "
                             << "reliably monotonic on this platform\n."
                             << "*** Allowing a tolerance of 1ms "
                             << "in test driver.\n";
                        LOOP4_ASSERT(i, j, prev, now, prev - TOLERANCE <= now);
                    }
                    LOOP4_ASSERT(i, j, prev, now, prev <= now);
                    prev = now;
                }
                if (veryVerbose && 0 == i % output_freq) {
                    cout << "+|"[0 == i % (OUTPUT_WIDTH / 4 * output_freq)]
                         << flush;
                }
            }
            if (veryVerbose) cout << "|" << endl;
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'utc' METHOD - STRESS TESTING FOR MONOTONICITY
        //
        // Concerns:
        //: 1 'utc' returns non-decreasing values when run in a loop a large
        //:   number of times.
        //
        // Plan:
        //: 1 Run 'utc' in a loop a large, configurable, number of times, and
        //:   verify that its result is non-decreasing.
        //
        // Testing:
        //   Datetime utc() stress test
        // --------------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING 'utc' METHOD - STRESS TESTING FOR MONOTONICITY"
                 << endl
                 << "======================================================"
                 << endl;
        {
            bdlt::DatetimeInterval TOLERANCE(0, 0, 0, 0, 1); // 1ms

            enum {
                NUM_ITERATIONS = 120,
                NUM_TEST_PER_ITERATION = 1000,
                OUTPUT_WIDTH = 60,
                OUTPUT_FREQ = NUM_ITERATIONS / OUTPUT_WIDTH
            };

            int iterations = verbose ? atoi(argv[2]) : NUM_ITERATIONS;
            int output_freq = iterations < OUTPUT_WIDTH ? 1
                            : iterations / OUTPUT_WIDTH;
            int testsPerIteration = NUM_TEST_PER_ITERATION;

            if (veryVerbose) {
                testsPerIteration = atoi(argv[3]);
                putAxis(OUTPUT_WIDTH);
            }

            for (int i = 0; i < iterations; ++i) {
                bdlt::Datetime prev = Util::utc();
                for (int j = 0; j < testsPerIteration; ++j) {
                    bdlt::Datetime now = Util::utc();
                    if (veryVeryVerbose) { P_(prev) P(now) }
                    if (prev > now) {
                        cout << "*** Warning: system time is not "
                             << "reliably monotonic on this platform\n."
                             << "*** Allowing a tolerance of 1ms "
                             << "in test driver.\n";
                        LOOP4_ASSERT(i, j, prev, now, prev - TOLERANCE <= now);
                    }
                    LOOP4_ASSERT(i, j, prev, now, prev <= now);
                    prev = now;
                }
                if (veryVerbose && 0 == i % output_freq) {
                    cout << "+|"[0 == i % (OUTPUT_WIDTH / 4 * output_freq)]
                         << flush;
                }
            }
            if (veryVerbose) cout << "|" << endl;
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'now' METHOD - STRESS TESTING FOR MONOTONICITY
        //
        // Concerns:
        //: 1 'now' returns non-decreasing values when run in a loop a large
        //:   number of times.
        //
        // Plan:
        //: 1 Run 'now' in a loop a large, configurable, number of times, and
        //:   verify that its result is non-decreasing.
        //
        // Testing:
        //   bsls::TimeInterval now() stress test
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'now' METHOD - STRESS TESTING FOR MONOTONICITY"
                 << endl
                 << "======================================================"
                 << endl;
        {
            bsls::TimeInterval TOLERANCE(0.001); // 1ms

            enum {
                NUM_ITERATIONS = 120,
                NUM_TEST_PER_ITERATION = 1000,
                OUTPUT_WIDTH = 60,
                OUTPUT_FREQ = NUM_ITERATIONS / OUTPUT_WIDTH
            };

            int iterations = verbose ? atoi(argv[2]) : NUM_ITERATIONS;
            int output_freq = iterations < OUTPUT_WIDTH ? 1
                            : iterations / OUTPUT_WIDTH;
            int testsPerIteration = NUM_TEST_PER_ITERATION;

            if (veryVerbose) {
                testsPerIteration = atoi(argv[3]);
                putAxis(OUTPUT_WIDTH);
            }
            for (int i = 0; i < iterations; ++i) {
                bsls::TimeInterval prev = Util::now();
                for (int j = 0; j < testsPerIteration; ++j) {
                    bsls::TimeInterval now = Util::now();
                    if (veryVeryVerbose) { P_(prev) P(now) }
                    if (prev > now) {
                        cout << "*** Warning: system time is not "
                             << "reliably monotonic on this platform\n."
                             << "*** Allowing a tolerance of 1ms "
                             << "in test driver.\n";
                        LOOP4_ASSERT(i, j, prev, now, prev - TOLERANCE <= now);
                    }
                    LOOP4_ASSERT(i, j, prev, now, prev <= now);
                    prev = now;
                }
                if (veryVerbose && 0 == i % output_freq) {
                    cout << "+|"[0 == i % (OUTPUT_WIDTH / 4 * output_freq)]
                         << flush;
                }
            }
            if (veryVerbose) cout << "|" << endl;
        }
      } break;
      case 6: {
        // -------------------------------------------------------------------
        // TESTING 'asDatetimeTz' METHOD
        //   Test the results of 'asDatetimeTz' correspond to the (already
        //   tested) results for 'utc' and 'local'.
        //
        // Concerns:
        //: 1 'asDatetimeTz' returns a value that corresponds to the current
        //:   'utc' time.
        //:
        //: 2 'asDatetimeTz' returns a value that corresponds to the current
        //:   'local' time.
        //
        // Plan:
        //: 1 Using the default real-time clock, verify 'asDatetimeTz'
        //:   returns datetime value whose local time matches 'Util::local'
        //:   and whose UTC time matches 'Util::now'
        //:
        //: 2 Install a callback which returns a specific value, and verify
        //:   that 'asDatetimeTz' returns datetime value whose local time
        //:   matches 'Util::local' and whose UTC time matches 'Util::now'
        //
        // Testing:
        //   DatetimeTz asDatetimeTz()
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'asDatetimeTz' METHOD" << endl
                          << "=============================" << endl;

        if (verbose) { cout << "Test with a real-time clock value" << endl; }
        {
            bdlt::Datetime   utc        = Util::utc();
            bdlt::Datetime   local      = Util::local();
            bdlt::DatetimeTz datetimeTz = Util::asDatetimeTz();

            ASSERT(within1Sec(utc, datetimeTz.utcDatetime()));
            ASSERT(within1Sec(local, datetimeTz.localDatetime()));
        }
        if (verbose) { cout << "Test with  a test clock value" << endl; }
        {
            Util::setCurrentTimeCallback(getClientTime);
            bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(getClientOffset);

            bdlt::Datetime   utc        = Util::utc();
            bdlt::Datetime   local      = Util::local();
            bdlt::DatetimeTz datetimeTz = Util::asDatetimeTz();

            ASSERTV(utc,   datetimeTz, utc   == datetimeTz.utcDatetime());
            ASSERTV(local, datetimeTz, local == datetimeTz.localDatetime());
        }
      } break;

      case 5: {
        // -------------------------------------------------------------------
        // TESTING 'local' METHOD
        //
        // Concerns:
        //: 1 'local' returns a non-decreasing value.
        //: 2 'local' is correct for a given 'utc'.
        //
        // Plan:
        //: 1 First create two Datetime objects and then load the local time.
        //:   Then verify that the local time is non-decreasing by invoking
        //:   'local' several times.
        //:
        //: 2 Install a callback which returns a specific value, and verify
        //:   that the difference between 'utc' and 'local' corresponds to the
        //:   offset reported by 'LocalTimeOffset' for that date.
        //
        // Testing:
        //   Datetime local()
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'local' METHOD" << endl
                          << "======================" << endl;

        if (verbose) { cout << "Testing monotonicity" << endl; }
        bdlt::Datetime dt1 = Util::local();
        bdlt::Datetime dt2 = Util::local();
        ASSERT (dt1 <= dt2);

        for (int i = 0; i < 10; ++i) {
            dt1 = dt2;
            dt2 = Util::local();
            if (veryVerbose) { P_(dt1) P(dt2) }
            ASSERT (dt1 <= dt2);
        }

        if (verbose) { cout << "Comparing 'utc' and 'local'" << endl; }
        Util::setCurrentTimeCallback(getClientTime);
        dt1 = Util::local();
        dt2 = Util::utc();
        ASSERT(bdlt::LocalTimeOffset::localTimeOffset(dt2) ==
               bdlt::IntervalConversionUtil::convertToTimeInterval(dt1 - dt2));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'now' AND 'utc' METHODS
        //
        // Concerns:
        //: 1 'now' and 'utc' return non-decreasing values, including with
        //:   respect to each other.
        //:
        //: 2 'now' and 'utc' correctly return values from user-specified
        //:   callback methods.
        //
        // Plan:
        //: 1 Call 'now' and then call 'utc', recording its offset from the
        //:   epoch, and verify that the results are properly ordered.
        //:
        //: 2 Call 'now' several times and verify that the results are
        //:   non-decreasing.
        //:
        //: 3 Install a callback function returning known time values and
        //:   verify that 'now' and 'utc' return the expected values.
        //
        // Testing:
        //   bsls::TimeInterval now()
        //   Datetime utc()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'now' AND 'utc' METHODS" << endl
                          << "===============================" << endl;

        if (verbose) {
            cout << "Testing 'now' and 'utc' (default implementation)" << endl;
        }
        {
            bsls::TimeInterval i1 = Util::now();
            ASSERT( 0 != i1 );

            bdlt::Datetime         dt1 = Util::utc();
            bdlt::DatetimeInterval dti = dt1 - bdlt::EpochUtil::epoch();
            ASSERT(0 <= inOrder(i1, dti) );  // i1 <= dt1

            bsls::TimeInterval i2 = Util::now();
            ASSERT( 0 != i2 );
            ASSERT( 0 <= inOrder(dti, i2) );  // dt1 <= i2
            ASSERT( i2 >= i1 );

            bsls::TimeInterval i3 = Util::now();
            ASSERT( 0 != i3 );

            bsls::TimeInterval i4 = Util::now();
            ASSERT( 0 != i4 );
            ASSERT( i4 >= i3 );

            bsls::TimeInterval i5 = Util::now();
            ASSERT( 0 != i5 );
            ASSERT( i5 >= i4 );
        }

        if (verbose) {
            cout << "Testing 'now' and 'utc' with user defined functions"
                 << endl;
        }
        {
            Util::setCurrentTimeCallback(getClientTime);
            ASSERT(getClientTime == Util::currentTimeCallback());

            bsls::TimeInterval i1 = Util::now();
            ASSERT( 1 == i1.seconds() && 1 == i1.nanoseconds() );

            bdlt::Datetime dt1 = Util::utc();
            ASSERT( 1970 == dt1.date().year());
            ASSERT( 1 == dt1.date().month());
            ASSERT( 1 == dt1.date().day());
            ASSERT( 0 == dt1.time().hour());
            ASSERT( 0 == dt1.time().minute());
            ASSERT( 1 == dt1.time().second());
            ASSERT( 0 == dt1.time().millisecond());

            bdlt::DatetimeInterval dti = dt1 - bdlt::EpochUtil::epoch();
            ASSERT( 0 == inOrder(i1, dti));
                                          // 'i1' == 'dti' == 1 second interval

            Util::setCurrentTimeCallback(IncrementInterval);
            ASSERT(IncrementInterval == Util::currentTimeCallback());

            bsls::TimeInterval i2 = Util::now();
            ASSERT( 0 == i2 );

            bdlt::Datetime dt2 = Util::utc();
            ASSERT( 1970 == dt2.date().year());
            ASSERT( 1 == dt2.date().month());
            ASSERT( 1 == dt2.date().day());
            ASSERT( 0 == dt2.time().hour());
            ASSERT( 0 == dt2.time().minute());
            ASSERT( 1 == dt2.time().second());
            ASSERT( 0 == dt2.time().millisecond());

            bdlt::DatetimeInterval dti2 = dt2 - bdlt::EpochUtil::epoch();
            ASSERT( 1 == inOrder(i2, dti2) );  // i2 < dt2-epoch()

            bdlt::Datetime dt3 = Util::utc();
            ASSERT( 1970 == dt3.date().year());
            ASSERT( 1 == dt3.date().month());
            ASSERT( 1 == dt3.date().day());
            ASSERT( 0 == dt3.time().hour());
            ASSERT( 0 == dt3.time().minute());
            ASSERT( 2 == dt3.time().second());
            ASSERT( 0 == dt3.time().millisecond());
            ASSERT( dt2 < dt3);

            bsls::TimeInterval     i3 = Util::now();
            bdlt::DatetimeInterval dti3 = dt3 - bdlt::EpochUtil::epoch();
            ASSERT( 1 == inOrder(dti3, i3) );  // dt3 < i2
            ASSERT( i3 >= i2 );

            bsls::TimeInterval i4 = Util::now();
            ASSERT( i4 >= i3 );

            bsls::TimeInterval i5 = Util::now();
            ASSERT( i5 >= i4 );

            bsls::TimeInterval i6 = Util::now();
            ASSERT( i6 >= i5 );

            Util::setCurrentTimeCallback(getClientTime);
            ASSERT(getClientTime == Util::currentTimeCallback());

            bsls::TimeInterval i7 = Util::now();
            ASSERT( 1 == i7.seconds() && 1 == i7.nanoseconds() );
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'inOrder(lhs, rhs)'
        //
        // Concerns:
        //: 1 The 'inOrder' helper method correctly determines the order of its
        //:   parameters.
        //
        // Plan:
        //: 1 Apply the function to a set of carefully chosen challenge
        //:   vectors, checking that the function returns the expected results.
        //
        // Testing:
        //   static int inOrder(INTERVAL_L lhs, INTERVAL_R rhs)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'inOrder(lhs, rhs)'" << endl
                          << "===========================" << endl;
        {
            typedef bdlt::TimeUnitRatio R;

            static const struct DtiTi {
                bdlt::DatetimeInterval dti;      // sample DatetimeInterval
                bsls::TimeInterval     ti;       // sample TimeInterval
                bsls::Types::Int64     diff;     // 'ti - dti' in milliseconds
                int                    inorder;  // predicted value
            } dtDiff[] = {
                {
                    bdlt::DatetimeInterval(),
                    bsls::TimeInterval(0, 0),
                    0,
                    0
                },
                {
                    bdlt::DatetimeInterval(0, 0, 0, 0, 1),
                    bsls::TimeInterval(0, 0),
                    -1,
                    -1
                },
                {
                    bdlt::DatetimeInterval(),
                    bsls::TimeInterval(0, R::k_NANOSECONDS_PER_MILLISECOND),
                    1,
                    1
                },
                {
                    bdlt::DatetimeInterval(25000),
                    bsls::TimeInterval(25000 * R::k_SECONDS_PER_DAY,
                                       R::k_NANOSECONDS_PER_MILLISECOND),
                    1,
                    1
                },
                {
                    bdlt::DatetimeInterval(-25000),
                    bsls::TimeInterval(-25000 * R::k_SECONDS_PER_DAY,
                                       -R::k_NANOSECONDS_PER_MILLISECOND),
                    -1,
                    -1
                },
                {
                    bdlt::DatetimeInterval(10000),
                    bsls::TimeInterval(10000 * R::k_SECONDS_PER_DAY,
                                       R::k_NANOSECONDS_PER_MILLISECOND),
                    1,
                    1
                },
                {
                    bdlt::DatetimeInterval(1),
                    bsls::TimeInterval(R::k_SECONDS_PER_DAY, 0),
                    0,
                    0
                },
                {
                    bdlt::DatetimeInterval(),
                    bsls::TimeInterval(0, 1),
                    0,
                    0
                },
                {
                    bdlt::DatetimeInterval(),
                    bsls::TimeInterval(0, -1),
                    0,
                    0
                },
                {
                    bdlt::DatetimeInterval(0, 0, 0, 0, 1),
                    bsls::TimeInterval(0, R::k_NANOSECONDS_PER_MILLISECOND),
                    0,
                    0
                },
            };

            const int NUM_TESTS = sizeof(dtDiff) / sizeof(DtiTi);

            for (int i=0; i < NUM_TESTS; ++i) {
                ASSERT(inOrder(dtDiff[i].dti, dtDiff[i].ti) ==
                                                            dtDiff[i].inorder);
                ASSERT(inOrder(dtDiff[i].ti, dtDiff[i].dti) ==
                                                           -dtDiff[i].inorder);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CALLBACK RELATED FUNCTIONS
        //
        // Concerns:
        //: 1 The callback function can be set through the
        //:   'setCurrentTimeCallback' function.
        //:
        //: 2 The callback function can be retrieved through the
        //:   'currentTimeCallback' function
        //:
        //: 3 The 'setCurrentTimeCallback' function returns the previous value
        //:   with which it was called.
        //:
        //: 4 These methods are thread-safe.
        //
        // Plan:
        //: 1 Set and retrieve callback functions using the two methods.
        //:   Verify that 'currentTimeCallback' returns the most recently set
        //:   value.  Verify that 'setCurrentTimeCallback' returns the value
        //:   with which it was called previously.
        //:
        //: 2 Start multiple threads which repeatedly install one of two
        //:   callback functions and verify that the only those functions are
        //:   ever returned by the methods.
        //
        // Testing:
        //   CurrentTimeCallback currentTimeCallback()
        //   CurrentTimeCallback setCurrentTimeCallback(CurrentTimeCallback)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CALLBACK RELATED FUNCTIONS" << endl
                          << "==================================" << endl;
        {
            const Util::CurrentTimeCallback callback_default_p1 =
                                                      Util::currentTimeDefault;
            Util::setCurrentTimeCallback(callback_default_p1);

            const Util::CurrentTimeCallback callback_default_p2 =
                                                   Util::currentTimeCallback();
            ASSERT(callback_default_p2 == callback_default_p1);

            const Util::CurrentTimeCallback callback_user_p1 = getClientTime;
            const Util::CurrentTimeCallback callback_return_p1 =
                                Util::setCurrentTimeCallback(callback_user_p1);
            ASSERT(callback_user_p1   == Util::currentTimeCallback());
            ASSERT(callback_return_p1 == callback_default_p2);
        }

        if (verbose) {
             cout << "Testing 'setCurrentTimeCallback' with some generic user "
                     "defined functions"
                  << endl;
        }
        {
            const Util::CurrentTimeCallback user_p1 = f1;
            Util::setCurrentTimeCallback(user_p1);
            ASSERT(user_p1 == Util::currentTimeCallback());

            const Util::CurrentTimeCallback user_p2 = f2;
            const Util::CurrentTimeCallback return_p1 =
                                         Util::setCurrentTimeCallback(user_p2);
            ASSERT(user_p2 == Util::currentTimeCallback());
            ASSERT(user_p1 == return_p1);
        }

        if (verbose) {
            cout << "Testing callback setting in multiple threads" << endl;
        }
        {
            int iterations = verbose     ? atoi(argv[2]) : 1000;
            int threads    = veryVerbose ? atoi(argv[3]) :   10;

            const int MAX_THREADS = 100;

            ThreadInfo DATA[MAX_THREADS];
            ThreadId   IDS [MAX_THREADS];

            bsls::AtomicOperations::setInt(&go, 0);
            for (int i = 0; i < MAX_THREADS && i < threads; ++i) {
                ThreadInfo& ti = DATA[i];
                ti.set = i & 1 ? f1 : f2;
                ti.other = i & 1 ? f2 : f1;
                ti.count = iterations;
                ti.verbose = veryVeryVerbose;
                ti.index = i;
                IDS[i] = createThread(threadFunction, &ti);
            }
            bsls::AtomicOperations::setInt(&go, 1);
            for (int i = 0; i < MAX_THREADS && i < threads; ++i) {
                joinThread(IDS[i]);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'currentTimeDefault'
        //
        // Concerns:
        //: 1 The default current-time callback function returns non-decreasing
        //:   values.
        //:
        //: 2 The default current-time callback function returns values close
        //:   to other current-time functions.
        //
        // Plan:
        //: 1 Repeatedly invoke the method and verify that returned values are
        //:   non-decreasing.
        //:
        //: 2 Compare the result of 'currentTimeDefault' against 'bsl::time'
        //:   and 'bsls::SystemTime::now'.
        //
        // Testing:
        //   bsls::TimeInterval currentTimeDefault()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'currentTimeDefault'" << endl
                          << "============================" << endl;

        if (verbose) { cout << "Testing monotonicity" << endl; }
        {
            bsls::TimeInterval i1 = Util::currentTimeDefault();
            ASSERT( 0 != i1);
            bsls::TimeInterval i2 = Util::currentTimeDefault();
            ASSERT( 0 != i2);
            ASSERT( i2 >= i1 );
            bsls::TimeInterval i3 = Util::currentTimeDefault();
            ASSERT( 0 != i3);
            ASSERT( i3 >= i2 );
            bsls::TimeInterval i4 = Util::currentTimeDefault();
            ASSERT( 0 != i4);
            ASSERT( i4 >= i3 );
            bsls::TimeInterval i5 = Util::currentTimeDefault();
            ASSERT( 0 != i5);
            ASSERT( i5 >= i4 );
            if (veryVerbose) { P_(i1); P_(i2); P(i3); P_(i4); P(i5); }
        }

        if (verbose) { cout << "Comparing to other time sources" << endl; }
        {
            bsls::TimeInterval ti1 = Util::currentTimeDefault();
            bsls::Types::Int64 s1 = ti1.totalSeconds();
            bsls::TimeInterval ti2 = bsls::SystemTime::nowRealtimeClock();
            bsls::Types::Int64 s2 = ti2.totalSeconds();
            bsls::Types::Int64 s3 = bsls::Types::Int64(bsl::time(0));
            ASSERT(s1 <= s2 && s2 <= s1 + 2);
            ASSERT(s1 <= s3 && s2 <= s3 + 2);
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // DETERMINE RESOLUTION OF 'now()'
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DETERMINE RESOLUTION OF 'now()'" << endl
                          << "===============================" << endl;

        for (int i = 0; i < 10; ++i) {
            bsls::TimeInterval ti1, ti2;

            ti1 = Util::now();
            do {
                ti2 = Util::now();
            } while (ti2 <= ti1);

            cout << "Resolution: " << (ti2 - ti1).totalSecondsAsDouble()
                 << " seconds\n";
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

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
