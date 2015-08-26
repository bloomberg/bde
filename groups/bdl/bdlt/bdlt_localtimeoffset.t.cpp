// bdlt_localtimeoffset.t.cpp                                         -*-C++-*-
#include <bdlt_localtimeoffset.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>
#include <bdlt_epochutil.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_systemtime.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>     // 'atoi', 'getenv', 'putenv'
#include <bsl_cstring.h>     // 'strncat'
#include <bsl_iostream.h>
#include <bsl_string.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#endif

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// 'bdlt_localtimeoffset' defines functions for setting, retrieving, and using
// a global callback for computing the local time offset as well as a default
// implementation for this global callback.  The operations are verified by
// directly invoking the methods and inspecting the results.
// ----------------------------------------------------------------------------
// [ 1] bsls::TimeInterval localTimeOffset(const Datetime& utcDatetime);
// [ 2] bsls::TimeInterval localTimeOffsetDefault(const Datetime& utcDT);
// [ 1] LTOC setLocalTimeOffsetCallback(LTOC callback);
// [ 1] LocalTimeOffsetCallback localTimeOffsetCallback();
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlt::LocalTimeOffset Obj;

// ============================================================================
//                      HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

class MyLocalTimeOffset {

    // DATA
    static const int *s_offset_p;  // address of offset returned by callback
    static bool      *s_callbackInvoked_p;
                                   // indication of callback invocation

  public:
    // CLASS METHODS
    static bsls::TimeInterval localTimeOffset(
                                            const bdlt::Datetime& utcDatetime);
        // Return a 'bsls::TimeInterval' value representing the value at the
        // address specified by the 'offset' argument of the last invocation of
        // the 'setExternals' method and set to 'true' the value at the address
        // specified by the 'callbackInvoked' argument of last invocation of
        // the 'setExternals' method.  The specified 'utcDatetime' is ignored.
        // The behavior is undefined unless the value at 'callbackInvoked' is
        // initially 'false'.

    static void setExternals(const int *offset, bool *callbackInvoked);
        // Set the the specified address 'offset' as the source of the value
        // returned by the 'localLocalTimeOffset' method and set the specified
        // address 'callbackInvoked' as the indicator of whether the callback
        // was invoked.
};

const int *MyLocalTimeOffset::s_offset_p          = 0;
bool      *MyLocalTimeOffset::s_callbackInvoked_p = 0;

bsls::TimeInterval MyLocalTimeOffset::localTimeOffset(const bdlt::Datetime&)
{
    ASSERT(!*s_callbackInvoked_p);

    *s_callbackInvoked_p = true;
    return bsls::TimeInterval(*s_offset_p, 0);
}

void MyLocalTimeOffset::setExternals(const int *offset, bool *callbackInvoked)
{
    ASSERT(offset);
    ASSERT(callbackInvoked);

    s_offset_p          = offset;
    s_callbackInvoked_p = callbackInvoked;
}

// ============================================================================
//                      HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

bsls::TimeInterval f1(const bdlt::Datetime& /* utcDatetime */)
    // Return a fixed 'bsls::TimeInterval' value.
{
    return bsls::TimeInterval(17, 1);
}

bsls::TimeInterval f2(const bdlt::Datetime& /* utcDatetime */)
    // Return a fixed 'bsls::TimeInterval' value.
{
    return bsls::TimeInterval(17, 2);
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
    Obj::LocalTimeOffsetCallback set;      // Set this as the callback.
    Obj::LocalTimeOffsetCallback other;    // Expect this as the callback too.
    int                          count;    // Set callback this many times.
    bool                         verbose;  // Print message on each iteration.
    int                          index;    // Index for this info.
};

static bsls::AtomicOperations::AtomicTypes::Int threadStart = { 0 };

static void *threadFunction(void *argument)
    // Using the 'ThreadInfo' data specified by 'argument', invoke the set and
    // get callback methods of 'bdlt::CurrentTime'.
{
    while (!bsls::AtomicOperations::getInt(&threadStart)) {
        // Wait for the starting gun.
    }

    ThreadInfo *ti = static_cast<ThreadInfo *>(argument);

    Obj::setLocalTimeOffsetCallback(ti->set);

    for (int i = 0; i < ti->count; ++i) {
        if (ti->verbose) { P_(ti->index) P(i) }
        Obj::LocalTimeOffsetCallback old =
                                      Obj::setLocalTimeOffsetCallback(ti->set);
        Obj::LocalTimeOffsetCallback current = Obj::localTimeOffsetCallback();

        bsls::TimeInterval offset = Obj::localTimeOffset(bdlt::Datetime());

        ASSERT(ti->set == old     || ti->other == old);
        ASSERT(ti->set == current || ti->other == current);

        ASSERT(   bsls::TimeInterval(17, 1) == offset
               || bsls::TimeInterval(17, 2) == offset);
    }

    return argument;
}

// ============================================================================
//                             USAGE EXAMPLE
// ----------------------------------------------------------------------------

//..
    struct MyLocalTimeOffsetUtilNewYork2013 {

      private:
        // DATA
        static int            s_useCount;
        static bdlt::Datetime s_startOfDaylightSavingTime;  // UTC Datetime
        static bdlt::Datetime s_resumptionOfStandardTime;   // UTC Datetime

      public:
        // CLASS METHODS
        static bsls::TimeInterval localTimeOffset(
                                            const bdlt::Datetime& utcDatetime);
            // Return a 'bsls::TimeInterval' value representing the difference
            // between the local time for the "America/New_York" timezone and
            // UTC time at the specified 'utcDatetime'.  The behavior is
            // undefined unless '2013 == utcDatetime.date().year()'.

        static int useCount();
            // Return the number of invocations of the 'localTimeOffset' since
            // the start of the process.
    };

    // DATA
    int MyLocalTimeOffsetUtilNewYork2013::s_useCount = 0;

    bdlt::Datetime
    MyLocalTimeOffsetUtilNewYork2013::s_startOfDaylightSavingTime(2013,
                                                                     3,
                                                                    10,
                                                                     7);
    bdlt::Datetime
    MyLocalTimeOffsetUtilNewYork2013::s_resumptionOfStandardTime(2013,
                                                                   11,
                                                                    3,
                                                                    6);

    // CLASS METHODS
    bsls::TimeInterval MyLocalTimeOffsetUtilNewYork2013::localTimeOffset(
                                             const bdlt::Datetime& utcDatetime)
    {
        ASSERT(2013 == utcDatetime.date().year());

        ++s_useCount;
        int seconds = utcDatetime < s_startOfDaylightSavingTime ? -18000 :
                      utcDatetime < s_resumptionOfStandardTime  ? -14400 :
                                                                  -18000;
        return bsls::TimeInterval(seconds, 0);
    }

    int MyLocalTimeOffsetUtilNewYork2013::useCount()
    {
        return s_useCount;
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
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Example 1: Basic 'bdlt::LocalTimeOffset' Usage
///- - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to use 'bdlt::LocalTimeOffset'.
//
// First, obtain the current UTC time - ignoring milliseconds - using
// 'bsls::SystemTime' and 'bdlt::EpochUtil' (note that clients may prefer
// 'bdlt_currenttime', which is not shown here for dependency reasons):
//..
    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();

    bdlt::Datetime utc = bdlt::EpochUtil::epoch() +
                                bdlt::DatetimeInterval(0, 0, 0, now.seconds());
//..
// Then, obtain the local time offset:
//..
    bsls::TimeInterval localOffset =
                                   bdlt::LocalTimeOffset::localTimeOffset(utc);
//..
// Next, add the offset to the UTC time to obtain the local time:
//..
    bdlt::Datetime local = utc;
    local.addSeconds(localOffset.seconds());
//..
// Finally, stream the two time values to 'stdout':
//..
    bsl::cout << "utc   = " << utc << bsl::endl;
    bsl::cout << "local = " << local << bsl::endl;
//..
// The streaming operator produces output in the following format on 'stdout':
//..
//  utc   = ddMONyyyy_hh:mm::ss.000
//  local = ddMONyyyy_hh:mm::ss.000
//..
//
///Example 2: Using the Local Time Offset Callback
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose one has to provide time stamp values that always reflect local time
// for a given location, even when local time transitions into and out of
// daylight saving time.  Further suppose that one must do this quite often
// (e.g., for every record in a high frequency log), so the performance of the
// default method for calculating local time offset is not adequate.  Creation
// and installation of a specialized user-defined callback for local time
// offset allows one to solve this problem.
//
// First, create a utility class that provides a method of type
// 'bdlt::LocalTimeOffset::LocalTimeOffsetCallback' that is valid for the
// location of interest (New York) for the period of interest (the year 2013).
//..
//..
// Note that the transition times into and out of daylight saving for New York
// are given in UTC.  Also notice that we do not attempt to make the
// 'localTimeOffset' method 'inline', since we must take its address to install
// it as the callback.
//
// Then, we install this 'localTimeOffset' as the local time offset callback.
//..
    bdlt::LocalTimeOffset::LocalTimeOffsetCallback defaultCallback =
                             bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(
                                            &MyLocalTimeOffsetUtilNewYork2013::
                                                              localTimeOffset);

    ASSERT(bdlt::LocalTimeOffset::localTimeOffsetDefault == defaultCallback);
    ASSERT(&MyLocalTimeOffsetUtilNewYork2013::localTimeOffset
                          == bdlt::LocalTimeOffset::localTimeOffsetCallback());
//..
// Now, we can use the 'bdlt::LocalTimeOffset::localTimeOffset' method to
// obtain the local time offsets in New York on several dates of interest.  The
// increasing values from our 'useCount' method assures us that the callback we
// defined is indeed being used.
//..
    ASSERT(0 == MyLocalTimeOffsetUtilNewYork2013::useCount());

    bsls::Types::Int64 offset;
    bdlt::Datetime     newYearsDay(2013,  1,  1);
    bdlt::Datetime     independenceDay(2013,  7,  4);
    bdlt::Datetime     newYearsEve(2013, 12, 31);

    offset = bdlt::LocalTimeOffset::localTimeOffset(newYearsDay).seconds();
    ASSERT(-5 * 3600 == offset);
    ASSERT(        1 == MyLocalTimeOffsetUtilNewYork2013::useCount());

    offset = bdlt::LocalTimeOffset::localTimeOffset(independenceDay).seconds();
    ASSERT(-4 * 3600 == offset);
    ASSERT(        2 == MyLocalTimeOffsetUtilNewYork2013::useCount());

    offset = bdlt::LocalTimeOffset::localTimeOffset(newYearsEve).seconds();
    ASSERT(-5 * 3600 == offset);
    ASSERT(        3 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//..
// Finally, to be neat, we restore the local time offset callback to the
// default callback:
//..
    bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(defaultCallback);
    ASSERT(&bdlt::LocalTimeOffset::localTimeOffsetDefault
                          == bdlt::LocalTimeOffset::localTimeOffsetCallback());
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'localTimeOffsetDefault'
        //  Verify the 'localTimeOffsetDefault' function returns the difference
        //  between local time and UTC time by comparing to an oracle function.
        //
        // Concerns:
        //: 1 'localTimeOffsetDefault' returns the correct value.
        //
        // Plan:
        //: 1 Compare the result of 'localTimeOffsetDefault' with an oracle
        //:   function.  (C-1)
        //
        // Testing:
        //   bsls::TimeInterval localTimeOffsetDefault(const Datetime& utcDT);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'localTimeOffsetDefault'" << endl
                          << "================================" << endl;

        if (verbose) cout << "\nCompare 'localTimeOffsetDefault' "
                             "to Alternate Implementation" << endl;
        {
            static const char *INPUT[]   = {  "America/New_York",
                                              "Utc/GMT",
                                              "Europe/Berlin"
                                           };
            static const int   NUM_INPUT = static_cast<int>(sizeof(INPUT)
                                                            / sizeof(*INPUT));

            static const string TZ_EQUALS = "TZ=";

            // On some systems, 'putenv' requires a mutable buffer that will be
            // incorporated into the environment *by* *address*, so it must
            // have static storage.

            const size_t ENV_BUFFER_SIZE = 80;
            static char  tzSetting[ENV_BUFFER_SIZE] = {};

            for (int i = 0; i < NUM_INPUT; ++i) {
                const string TZ = INPUT[i];

                if (veryVerbose) { T_; P(TZ) }

                tzSetting[0] = '\0';
                strncat(tzSetting, (TZ_EQUALS + TZ).c_str(),
                                                          ENV_BUFFER_SIZE - 1);

                if (veryVeryVerbose) { T_; P(tzSetting) }

                int status = putenv(tzSetting);
                ASSERT(0 == status);

                const string RETRIEVED_TZ = getenv("TZ");
                ASSERTV(RETRIEVED_TZ, TZ, RETRIEVED_TZ == TZ);

                tzset();

                // Compute current time by adding interval to epoch.
                bdlt::Datetime now = bdlt::Datetime(1970, 1, 1);
                now.addMilliseconds(
                     bsls::SystemTime::nowRealtimeClock().totalMilliseconds());

                bsls::TimeInterval offset =
                            bdlt::LocalTimeOffset::localTimeOffsetDefault(now);

                if (veryVeryVerbose) { T_; P_(now) P(offset) }

                time_t currentTime;
                bdlt::EpochUtil::convertToTimeT(&currentTime, now);

                struct tm gmtTM =    *gmtime(&currentTime);
                struct tm lclTM = *localtime(&currentTime);

                bdlt::Datetime gmtDatetime(gmtTM.tm_year + 1900,
                                           gmtTM.tm_mon  +    1,
                                           gmtTM.tm_mday,
                                           gmtTM.tm_hour,
                                           gmtTM.tm_min,
                                           gmtTM.tm_sec);

                bdlt::Datetime lclDatetime(lclTM.tm_year + 1900,
                                           lclTM.tm_mon  +    1,
                                           lclTM.tm_mday,
                                           lclTM.tm_hour,
                                           lclTM.tm_min,
                                           lclTM.tm_sec);

                bdlt::DatetimeInterval diffInterval =
                                                     lclDatetime - gmtDatetime;

                if (veryVeryVerbose) {
                    P(gmtDatetime)
                    P(lclDatetime)
                    P(diffInterval);
                }

                ASSERT(bsls::TimeInterval(diffInterval.totalSeconds(), 0)
                                                                    == offset);
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // LOCAL TIME OFFSET CALLBACK FACILITY
        //   Verify the methods for setting, obtaining, and invoking the
        //   global callback function.
        //
        // Concerns:
        //: 1 The 'localTimeOffsetDefault' method is installed as the default
        //:   callback.
        //:
        //: 2 The 'localTimeOffsetCallback' method shows the currently
        //:   installed callback.
        //:
        //: 3 The 'setLocalTimeOffsetCallback' returns the previously installed
        //:   callback, and sets the specified callback.
        //:
        //: 4 The installed callback is called by 'localTimeOffset' and that
        //:   the value generated by the callback is reflected in the value
        //:   returned by this method.
        //:
        //: 5 The user-defined callback used in this test operates correctly.
        //:
        //: 6 These methods are thread-safe.
        //:
        //: 7 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the 'setLocalTimeOffsetCallback' and
        //:   'localTimeOffsetCallback' methods to check, change to a
        //:   user-defined callback, and restore the initial callback.  Confirm
        //:   that the expected values are returned by each method at each
        //:   stage.  (C-1..3)
        //:
        //: 2 Create a user-defined callback that provides an external
        //:   indication when called, and provides a user-specified offset to
        //:   its caller.  Using an array-driven test, demonstrate that the
        //:   callback works as designed for several offset values, and, when
        //:   the user-defined callback is installed as the local time offset
        //:   callback, demonstrate that the 'localTimeOffset' method passes
        //:   through the offset value.  (C-4, C-5).
        //:
        //: 3 Start multiple threads which repeatedly install and use one of
        //:   two callback functions and verify that only those functions are
        //:   ever returned and used by the methods.  (C-6)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-7)
        //
        // Testing:
        //   bsls::TimeInterval localTimeOffset(const Datetime& utcDatetime);
        //   LTOC setLocalTimeOffsetCallback(LTOC callback);
        //   LocalTimeOffsetCallback localTimeOffsetCallback();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "LOCAL TIME OFFSET CALLBACK FACILITY" << endl
                          << "===================================" << endl;

        if (verbose) cout << "\n'localTimeOffsetCallback' and "
                             "'setLocalTimeOffsetCallback'" << endl;
        {
            if (veryVerbose) { T_; Q(Check for Default Initially) }

            ASSERT(Obj::localTimeOffsetDefault
                   == Obj::localTimeOffsetCallback());

            if (veryVerbose) { T_; Q(Set User-Defined Callback) }

            Obj::LocalTimeOffsetCallback defaultCallback =
                           Obj::setLocalTimeOffsetCallback(&MyLocalTimeOffset::
                                                              localTimeOffset);
            ASSERT(Obj::localTimeOffsetDefault == defaultCallback);
            ASSERT(&MyLocalTimeOffset::localTimeOffset
                                            == Obj::localTimeOffsetCallback());

            if (veryVerbose) { T_; Q(Restore Default Callback) }

            Obj::LocalTimeOffsetCallback userCallback =
                 Obj::setLocalTimeOffsetCallback(&Obj::localTimeOffsetDefault);

            ASSERT(&MyLocalTimeOffset::localTimeOffset == userCallback);
            ASSERT(Obj::localTimeOffsetDefault
                                            == Obj::localTimeOffsetCallback());
        }

        if (verbose) cout << "\n'MyLocalTimeOffset' and"
                             "'localTimeOffset'" << endl;
        {
            const int INPUT[]   = { INT_MIN, -1, 0, 1, INT_MAX};
            const int NUM_INPUT = static_cast<int>(sizeof(INPUT)
                                                   / sizeof(*INPUT));

            int  expectedOffset;
            bool callbackInvoked;

            MyLocalTimeOffset::setExternals(&expectedOffset, &callbackInvoked);

            Obj::LocalTimeOffsetCallback defaultCallback =
                           Obj::setLocalTimeOffsetCallback(&MyLocalTimeOffset::
                                                              localTimeOffset);

            ASSERT(Obj::localTimeOffsetDefault == defaultCallback);
            ASSERT(&MyLocalTimeOffset::localTimeOffset
                                            == Obj::localTimeOffsetCallback());

            for (int i = 0; i < NUM_INPUT; ++i) {
                for (int j = 0; j < NUM_INPUT; ++j) {
                    expectedOffset = INPUT[j];

                    if (veryVerbose) { T_; P(expectedOffset) }

                    callbackInvoked = false;
                    ASSERT(!callbackInvoked);
                    bsls::TimeInterval resultHelper =
                          MyLocalTimeOffset::localTimeOffset(bdlt::Datetime());
                    ASSERT(callbackInvoked);
                    ASSERT(bsls::TimeInterval(expectedOffset, 0)
                                                              == resultHelper);

                    callbackInvoked = false;
                    ASSERT(!callbackInvoked);
                    bsls::TimeInterval resultMethod =
                                        Obj::localTimeOffset(bdlt::Datetime());
                    ASSERT(callbackInvoked);
                    ASSERT(bsls::TimeInterval(expectedOffset, 0)
                                                              == resultMethod);

                    ASSERT(resultHelper == resultMethod);
                }
            }

            // Restore default local time callback

            Obj::setLocalTimeOffsetCallback(defaultCallback);
            ASSERT(&Obj::localTimeOffsetDefault
                                            == Obj::localTimeOffsetCallback());
        }

        if (verbose) cout << "\nTesting thread-safety." << endl;
        {
            int iterations = verbose     ? atoi(argv[2]) : 1000;
            int threads    = veryVerbose ? atoi(argv[3]) :   10;

            const int MAX_THREADS = 100;

            ThreadInfo DATA[MAX_THREADS];
            ThreadId   IDS [MAX_THREADS];

            bsls::AtomicOperations::setInt(&threadStart, 0);
            for (int i = 0; i < MAX_THREADS && i < threads; ++i) {
                ThreadInfo& ti = DATA[i];
                ti.set = i & 1 ? f1 : f2;
                ti.other = i & 1 ? f2 : f1;
                ti.count = iterations;
                ti.verbose = veryVeryVerbose;
                ti.index = i;
                IDS[i] = createThread(threadFunction, &ti);
            }
            bsls::AtomicOperations::setInt(&threadStart, 1);
            for (int i = 0; i < MAX_THREADS && i < threads; ++i) {
                joinThread(IDS[i]);
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            ASSERT_SAFE_FAIL(Obj::setLocalTimeOffsetCallback(0));
            ASSERT_SAFE_PASS(Obj::setLocalTimeOffsetCallback(
                                         &MyLocalTimeOffset::localTimeOffset));
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
