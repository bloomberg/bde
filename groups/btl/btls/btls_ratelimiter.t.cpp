// btls_ratelimiter.t.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btls_ratelimiter.h>

#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bdlt_currenttime.h>

#include <bsls_asserttest.h>

#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_c_math.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a mechanism.
//
// Primary Manipulators:
//: o 'setRateLimits'
//
// Basic Accessors:
//: o 'peakRateLimit'
//: o 'sustainedRateLimit'
//: o 'peakRateWindow'
//: o 'sustainedRateWindow'
//: o 'lastUpdateTime'
//: o 'unitsReserved'
//
// This class also provides a value constructor capable of creating an object
// having any parameters.
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
// ----------------------------------------------------------------------------
//
// CREATORS
// [ 2] btls::RateLimiter(sR, sRW, pR, pRW, currentTime);
//
// MANIPULATORS
// [ 6] void setRateLimits(sR, sRW, pR, pRW, currentTime);
// [ 4] void submit(bsls::Types::Uint64 numOfUnits);
// [ 5] void reserve(bsls::Types::Uint64 numOfUnits);
// [ 8] void updateState(const bsls::TimeInterval& currentTime);
// [ 4] bool wouldExceedBandwidth(currentTime);
// [ 5] void submitReserved(bsls::Types::Unit64 numOfUnits);
// [12] void cancelReserved(bsls::Types::Unit64 numOfUnits);
// [10] void resetStatistics();
// [11] void reset(const bsls::TimeInterval& currentTime);
// [ 7] bsls::TimeInterval calculateTimeToSubmit(currentTime);
//
// ACCESSORS
// [ 3] bsls::Types::Uint64 peakRateLimit() const;
// [ 3] bsls::Types::Uint64 sustainedRateLimit() const;
// [ 3] bsls::TimeInterval peakRateWindow() const;
// [ 3] bsls::TimeInterval sustainedRateWindow() const;
// [ 5] bsls::Types::Uint64 unitsReserved() const;
// [ 3] bsls::TimeInterval lastUpdateTime() const;
// [ 9] void getStatistics(*submittedUnits, *unusedUnits) const;
// [ 3] bsls::TimeInterval statisticsCollectionStartTime() const;
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [13] USAGE EXAMPLE
// [ 3] All accessor methods are declared 'const'.
// [ *] All creator/manipulator ptr./ref. parameters are 'const'.
// ============================================================================

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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)
#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
typedef btls::RateLimiter    Obj;
typedef bsls::TimeInterval   Ti;
typedef bsls::Types::Uint64 Uint64;
typedef unsigned int        uint;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Controlling Network Traffic Generation
///-------------------------------------------------
// Suppose that we want to send data over a network interface with the load
// spike limitations explained below:
//
//: o The long term average rate of resource usage (i.e., the sustained rate)
//:   should not exceed 1024 bytes/s ('Rs').
//:
//: o The period over which to monitor the long term average rate (i.e., the
//:   sustained-rate time-window) should be 0.5s ('Wp').
//:
//: o The peak resource usage (i.e., the peak rate) should not exceed 2048
//:   bytes/s ('Rp').
//:
//: o The period over which to monitor the peak resource usage should be
//:   0.0625s (Wp).
//
// This is shown in Figure 2 below.
//..
// Fig. 2:
//
//     ^ Rate (Units per second)
//     |                             _____         .
//     |                            /  B  \        .
// 2048|---------------------------/-------\--------Rp (Maximum peak rate)
//     |           __             /         \      .
//     |          /  \           /    A2     \     .
//     |         / A1 \         /             \    .
// 1024|--------/------\ ------/---------------\----Rs (Maximum sustained rate)
//     |   __  /        \     /                 \__.
//     |__/  \/          \___/                     .
//     |                                           .
//      --------------------------------------------->
//                                         T (seconds)
//..
// Notice that we can understand the limitations imposed by the rate-limiter
// graphically as the maximum area above the respective lines, 'Rp' and 'Rs',
// that the usage curve to allowed to achieve.  In the example above:
//
//  o The area above the sustained rate 'Rs' (e.g., 'A1' or 'A2+B') should
//    contain no more than 512 bytes (Rs * Ws).
//
//  o The area above the peak rate 'Rp' should contain no more than 128 bytes
//    (Rp * Wp).
//
// Further suppose that we have a function, 'sendData', that transmits a
// specified amount of data over that network:
//..
    bool sendData(size_t dataSize)
        // Send a specified 'dataSize' amount of data over the network.  Return
        // 'true' if data was sent successfully and 'false' otherwise.
    {
        (void)(dataSize);
        // For simplicity, 'sendData' will not actually send any data and will
        // always return 'true'.
        return true;
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 13: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;

// First, we create a 'btls::RateLimiter' object having a sustained rate of
// 1024 bytes/s, a sustained-rate time-window of 0.5s (512 bytes / 1024
// bytes/s), a peak-rate of 2048 bytes/s, and a peak-rate time-window of
// 0.0625s (128 bytes / 2048 bytes/s):
//..
    bsls::Types::Uint64 sustainedRateLimit = 1024;
    bsls::TimeInterval  sustainedRateWindow(0.5);
    bsls::Types::Uint64 peakRateLimit = 2048;
    bsls::TimeInterval  peakRateWindow(0.0625);
    bsls::TimeInterval  now = bdlt::CurrentTime::now();

    btls::RateLimiter  rateLimiter(sustainedRateLimit,
                                   sustainedRateWindow,
                                   peakRateLimit,
                                   peakRateWindow,
                                   now);
//..
// Note that the rate limiter does not prevent the rate at any instant from
// exceeding either the peak-rate or the sustained rate; instead, it prevents
// the average rate over the peak-rate time-window from exceeding maximum
// peak-rate and the average rate over the sustained-rate time-window from
// exceeding the maximum sustained-rate.
//
// Then, we define the size of data to be send, the size of each data chunk,
// and a counter of data actually sent:
//..
    bsls::Types::Uint64 sizeOfData = 10 * 1024; // in bytes
    bsls::Types::Uint64 chunkSize  = 64;        // in bytes
    bsls::Types::Uint64 bytesSent  = 0;
//..
// Now, we send the chunks of data using a loop.  For each iteration, we check
// whether submitting another byte would exceed the rate limiter's bandwidth
// limits.  If not, we send an additional chunk of data and submit the number
// of bytes sent to the leaky bucket.  Note that 'submit' is invoked only after
// the data has been sent.
//..
    while (bytesSent < sizeOfData) {
        now = bdlt::CurrentTime::now();
        if (!rateLimiter.wouldExceedBandwidth(now)) {
            if (true == sendData(chunkSize)) {
                rateLimiter.submit(chunkSize);
                bytesSent += chunkSize;
            }
        }
//..
// Finally, if submitting another byte will cause the rate limiter to exceed
// its bandwidth limits, then we wait until the submission will be allowed by
// waiting for an amount time returned by the 'calculateTimeToSubmit' method:
//..
        else {
            bsls::TimeInterval timeToSubmit =
                                        rateLimiter.calculateTimeToSubmit(now);
            bsls::Types::Uint64 uS = timeToSubmit.totalMicroseconds() +
                                   (timeToSubmit.nanoseconds() % 1000) ? 1 : 0;
            bslmt::ThreadUtil::microSleep(static_cast<int>(uS));
        }
    }
//..
// Notice that we wait by putting the thread into a sleep state instead of
// using busy-waiting to better optimize for multi-threaded applications.
//..
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'cancelReserved'
        //
        // Concerns:
        //: 1 The method decrements the number of 'unitsReserved'.
        //:
        //: 2 The method does not submit units.
        //:
        //: 3 The method correctly handles the case, when 'numOfUnits' is
        //:  greater than the number of 'unitsReserved'.
        //
        // Testing:
        //   void cancelReserved(bsls::Types::Uint64 numOfUnits)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'cancelReserved'" << endl
                          << "=============================" << endl;

        const Uint64 S_RATE = 1000;
        const Ti     S_WND  = Ti(1);
        const Uint64 P_RATE = 10000;
        const Ti     P_WND  = Ti(0.01);

        struct {
            int    d_line;
            Uint64 d_unitsToReserve;
            Uint64 d_unitsToCancel;
            Uint64 d_expectedUnitsReserved;
        } DATA[] = {

        //  LINE    RESERVE       CANCEL            EXP_RES
        //  ----  ----------  -------------   --------------------
            {L_,       1000,          300,                     700},
            // C-3
            {L_,       1000,         1000,                       0},
            {L_,       1000,          700,                     300},
            {L_, ULLONG_MAX, ULLONG_MAX/2, ULLONG_MAX-ULLONG_MAX/2}
        };

        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        for(int ti=0; ti<NUM_DATA; ti++) {

            const Uint64 LINE             = DATA[ti].d_line;
            const Uint64 UNITS_TO_RESERVE = DATA[ti].d_unitsToReserve;
            const Uint64 UNITS_TO_CANCEL  = DATA[ti].d_unitsToCancel;
            const Uint64 EXPECTED_UNITS_RESERVED =
                DATA[ti].d_expectedUnitsReserved;

            Obj x(S_RATE, S_WND, P_RATE, P_WND, Ti(0));

            btls::LeakyBucket peakLb(
                           P_RATE,
                           btls::LeakyBucket::calculateCapacity(P_RATE, P_WND),
                           Ti(0));

            btls::LeakyBucket sustLb(
                           S_RATE,
                           btls::LeakyBucket::calculateCapacity(S_RATE, S_WND),
                           Ti(0));

            // C-1
            x.reserve(UNITS_TO_RESERVE);
            sustLb.reserve(UNITS_TO_RESERVE);
            peakLb.reserve(UNITS_TO_RESERVE);

            LOOP_ASSERT(LINE, UNITS_TO_RESERVE == x.unitsReserved());

            x.cancelReserved(UNITS_TO_CANCEL);
            sustLb.cancelReserved(UNITS_TO_CANCEL);
            peakLb.cancelReserved(UNITS_TO_CANCEL);

            LOOP_ASSERT(LINE,
                        EXPECTED_UNITS_RESERVED == x.unitsReserved());

            // Check that no units were submitted to the rate limiter.
            // C-2
            if (false == x.wouldExceedBandwidth(Ti(0))) {

                bsls::Types::Uint64 freeUnits = bsl::min(
                                        sustLb.capacity() - x.unitsReserved(),
                                        peakLb.capacity() - x.unitsReserved());

                x.submit(freeUnits);
                LOOP_ASSERT(LINE, true == x.wouldExceedBandwidth(Ti(0)));

                x.reset(Ti(0));
                x.reserve(UNITS_TO_RESERVE);
                x.cancelReserved(UNITS_TO_CANCEL);
                x.submit(freeUnits-1);
                LOOP_ASSERT(LINE, false == x.wouldExceedBandwidth(Ti(0)));
            }
            else {
            }
        }

        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj y(100, Ti(10), 1000, Ti(1), Ti(0));
            y.submit(100);
            y.reserve(100);
            ASSERT_PASS(y.cancelReserved(100));
            ASSERT_FAIL(y.cancelReserved(1));
            y.reserve(100);
            ASSERT_FAIL(y.cancelReserved(101));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'reset'
        //
        // Concerns:
        //: 1 The object parameters are not affected by the 'reset'
        //:    method.
        //:
        //: 2 The object state is set to initial state and 'lastUpdateTime'
        //:   is recorded correctly when 'reset' is invoked
        //:
        //: 3 'reset' method invokes 'resetStatistics' method and
        //:   'statisticsCollectionStartTime' is updated
        //
        // Testing:
        //   void reset(const bsls::TimeInterval& currentTime);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'reset'" << endl
                          << "====================" << endl;

        Uint64 usedUnits       = 0;
        Uint64 unusedUnits     = 0;
        const Uint64 S_RATE    = 1000;
        const Ti     S_WND     = Ti(10);
        const Uint64 P_RATE    = 10000;
        const Ti     P_WND     = Ti(0.1);

        struct {
            int    d_line;
            Ti     d_creationTime;
            Uint64 d_units;
            Ti     d_resetTime;
        } DATA[] = {
            //LINE  CREATION TIME  UNITS  RESET TIME
            //----  -------------  -----  ----------
            {  L_,         Ti( 0),     0,     Ti( 0) },
            {  L_,         Ti( 0),  1000,     Ti( 0) },
            {  L_,         Ti( 0),  2000,     Ti( 0) },
            {  L_,         Ti(50),     0,     Ti(60) },
            {  L_,         Ti(50),  1000,     Ti(60) },
            {  L_,         Ti(50),     0,     Ti( 0) },
            {  L_,         Ti(50),  1000,     Ti( 0) }
        };
        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        for(int ti = 0; ti < NUM_DATA; ti++) {
            const int    LINE          = DATA[ti].d_line;
            const Ti     CREATION_TIME = DATA[ti].d_creationTime;
            const Uint64 UNITS         = DATA[ti].d_units;
            const Ti     RESET_TIME    = DATA[ti].d_resetTime;

            Obj x(S_RATE, S_WND, P_RATE, P_WND, CREATION_TIME);
            x.submit(UNITS);
            x.reserve(UNITS);
            x.updateState(RESET_TIME);

            x.reset(RESET_TIME);

            // C-1
            ASSERT(S_RATE   == x.sustainedRateLimit());
            ASSERT(S_WND    == x.sustainedRateWindow());
            ASSERT(P_RATE   == x.peakRateLimit());
            ASSERT(P_WND    == x.peakRateWindow());

            // C-2
            LOOP_ASSERT(LINE, 0          == x.unitsReserved());
            LOOP_ASSERT(LINE, RESET_TIME == x.lastUpdateTime());

            // C-3
            x.getStatistics(&usedUnits, &unusedUnits);

            LOOP_ASSERT(LINE, 0          == usedUnits);
            LOOP_ASSERT(LINE, 0          == unusedUnits);
            LOOP_ASSERT(LINE, RESET_TIME == x.statisticsCollectionStartTime());
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'resetStatistics'
        //
        // Concerns:
        //: 1 'resetStatistics' resets unit statistics counter to 0.
        //:
        //: 2 'resetStatistics' updates 'statisticsCollectionStartTime' time
        //:    correctly.
        //:
        //: 3 'resetStatistics' does not alter object state except for
        //:   submitted units counter and 'lastReset' time.
        //
        // Testing:
        //   void resetStatistics();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'resetStatistics'" << endl
                          << "==========================" << endl;

        Uint64 usedUnits       = 0;
        Uint64 unusedUnits     = 0;
        const Ti CREATION_TIME = Ti(0.0);
        const Ti UPD_TIME      = Ti(0.75);
        const Uint64 UNITS     = 500;
        const Uint64 S_RATE    = 1000;
        const Ti     S_WND     = Ti(10);
        const Uint64 P_RATE    = 10000;
        const Ti     P_WND     = Ti(0.1);
        const Uint64 EXP_USED  = UNITS;

        const Uint64 EXP_UNUSED =
            (Uint64) floor((UPD_TIME - CREATION_TIME).totalSecondsAsDouble() *
                                                            S_RATE) - EXP_USED;

        Obj x(S_RATE, S_WND, P_RATE, P_WND, CREATION_TIME);
        x.submit(UNITS);
        x.reserve(UNITS*2);
        x.updateState(UPD_TIME);

        x.getStatistics(&usedUnits, &unusedUnits);
        ASSERT(EXP_USED      == usedUnits);
        ASSERT(EXP_UNUSED    == unusedUnits);
        ASSERT(CREATION_TIME == x.statisticsCollectionStartTime());

        x.submit(UNITS);
        x.resetStatistics();

        // C-1
        x.getStatistics(&usedUnits, &unusedUnits);
        ASSERT(0 == usedUnits);
        ASSERT(0 == unusedUnits);

        // C-2
        ASSERT(UPD_TIME == x.statisticsCollectionStartTime());

        // C-3
        ASSERT(S_RATE   == x.sustainedRateLimit());
        ASSERT(S_WND    == x.sustainedRateWindow());
        ASSERT(P_RATE   == x.peakRateLimit());
        ASSERT(P_WND    == x.peakRateWindow());
        ASSERT(UPD_TIME == x.lastUpdateTime());
        ASSERT(UNITS*2  == x.unitsReserved());

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'getStatistics'
        //
        // Concerns:
        //: 1 'getStatistics' returns 0 for a new object, created by value
        //:   CTOR.
        //:
        //: 2 'getStatistics' returns correct numbers of used and unused units
        //:   (the calculation is base upon the sustained rate sustained rate
        //:   after a sequence of 'submit' and 'updateState' calls.
        //:
        //: 3 Specifying invalid parameters for 'getStatistics()' causes
        //:   certain behavior in special build configuration.
        //:
        //: 4 Statistics is calculated for interval between
        //:   'statisticsCollectionStartTime' and 'lastUpdateTime'
        //
        // Testing:
        //   void getStatistics(*submittedUnits, *unusedUnits) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'getStatistics'" << endl
                          << "============================" << endl;

        Uint64 usedUnits   = 0;
        Uint64 unusedUnits = 0;

        // C-1
        if (verbose) cout << endl
                          << "Testing: statistics after value construction"
                          << endl;
        {
            Obj x(1000, Ti(1), 10000, Ti(0.01), Ti(42.4242));

            x.getStatistics(&usedUnits, &unusedUnits);
            ASSERT(0 == usedUnits);
            ASSERT(0 == unusedUnits);
        }

        // C-2
        if (verbose) cout << endl
                          << "Testing: statistics calculation"
                          << endl;
        {
            const Uint64 P_RATE = 100000;
            const Ti     P_WND  = Ti(0.01);

            struct {
                int    d_line;
                Uint64 d_sustRate;
                Ti     d_sustWnd;
                Uint64 d_units;
                Ti     d_creationTime;
                Ti     d_updateInterval;
                int    d_NumOfUpdates;
                Uint64 d_expectedUsed;
                Uint64 d_expectedUnused;
            } DATA[] = {

     //  LINE S_RT  S_WND  UNITS    TCREATE  UPDATE_INT N_UPD  USED_U  UNUSED_U
     //  ---- ----- -----  ------   -------  ---------- -----  ------  --------
         {L_, 1000, Ti(1), 1000,    Ti( 0),  Ti( 0.01),  10,   10000,       0},
         {L_, 1000, Ti(1),  100,    Ti( 0),  Ti(  0.5),   5,     500,    2000},
         {L_, 100,  Ti(1),    0,    Ti(10),  Ti(  0.1),  20,       0,     200}
            };
            const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int    LINE            = DATA[ti].d_line;
                const Uint64 S_RATE          = DATA[ti].d_sustRate;
                const Ti     S_WND           = DATA[ti].d_sustWnd;
                const Uint64 UNITS           = DATA[ti].d_units;
                const Ti     CREATION_TIME   = DATA[ti].d_creationTime;
                const Ti     UPDATE_INTERVAL = DATA[ti].d_updateInterval;
                const int    NUM_OF_UPDATES  = DATA[ti].d_NumOfUpdates;
                const Uint64 EXPECTED_USED   = DATA[ti].d_expectedUsed;
                const Uint64 EXPECTED_UNUSED = DATA[ti].d_expectedUnused;

                Obj x(S_RATE, S_WND, P_RATE, P_WND, CREATION_TIME);
                Ti currentTime(CREATION_TIME);

                for(int i = 0; i < NUM_OF_UPDATES; ++i) {
                    currentTime += UPDATE_INTERVAL;
                    x.submit(UNITS);
                    x.updateState(currentTime);
                }

                x.getStatistics(&usedUnits, &unusedUnits);
                LOOP_ASSERT(LINE, EXPECTED_USED == usedUnits);
                LOOP_ASSERT(LINE, EXPECTED_UNUSED == unusedUnits);

            }
        }

        // C-3
        if (verbose) cout << endl << "Negative testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);
            Obj x(1, Ti(10), 1, Ti(10), Ti(0));

            ASSERT_SAFE_FAIL(x.getStatistics(0,&unusedUnits));
            ASSERT_SAFE_FAIL(x.getStatistics(&usedUnits,0));
            ASSERT_SAFE_FAIL(x.getStatistics(0,0));
        }

        // C-4
        if (verbose) cout << endl
                          << "Testing statistics collection interval"
                          << endl;
        {
            Obj x(1000, Ti(1), 10000, Ti(0.01), Ti(0));
            x.submit(1000);

            x.getStatistics(&usedUnits, &unusedUnits);
            ASSERT(0 == usedUnits);
            ASSERT(0 == unusedUnits);

            x.updateState(Ti(0.1));
            x.getStatistics(&usedUnits, &unusedUnits);
            ASSERT(1000 == usedUnits);
            ASSERT(0    == unusedUnits);

            x.updateState(Ti(10));
            x.getStatistics(&usedUnits, &unusedUnits);
            ASSERT(1000 == usedUnits);
            ASSERT(9000 == unusedUnits);

            x.resetStatistics();
            x.updateState(Ti(15));
            x.getStatistics(&usedUnits, &unusedUnits);
            ASSERT(0    == usedUnits);
            ASSERT(5000 == unusedUnits);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'updateState'
        //
        // Concerns:
        //: 1 The method updates 'lastUpdateTime'.
        //:
        //: 2 The method updates 'lastUpdateTime' if the specified
        //:   'currentTime' precedes 'lastUpdateTime'.
        //:
        //: 3 The method does not affect 'statisticsCollectionStartTime' if
        //:   time does not go backwards.
        //:
        //: 4 The method updates 'statisticsCollectionStartTime', if
        //:   'lastUpdateTime' precedes 'statisticsCollectionStartTime'.
        //
        // Testing:
        //   void updateState(const bsls::TimeInterval& currentTime);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'updateState'" << endl
                          << "==========================" << endl;

        Obj x(10, Ti(10), 100, Ti(1), Ti(10));
        x.submit(30);
        x.submit(100);

        // C-1
        x.updateState(Ti(15));

        ASSERT(Ti(15) == x.lastUpdateTime());
        ASSERT(Ti(10) == x.statisticsCollectionStartTime()); // C-3

        // C-2
        x.updateState(Ti(5));
        ASSERT(Ti(5)  == x.lastUpdateTime());
        ASSERT(Ti(5) == x.statisticsCollectionStartTime()); // C-4

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'wouldExceedBandwidth', 'calculateTimeToSubmit'.
        //
        // Concerns:
        //: 1 'wouldExceedBandwidth' returns true when peak rate limit is
        //:   exceeded.
        //:
        //: 2 'wouldExceedBandwidth' returns true, when sustained rate limit is
        //:   exceeded.
        //:
        //: 3 'wouldExceedBandwidth' returns false, if load does not exceed
        //:   both limits.
        //:
        //: 4 'calculateTimeToSubmit' returns a non-zero interval when
        //:   'wouldExceedBandwidth' returns true.
        //:
        //: 5 'calculateTimeToSubmit' returns a zero interval when
        //:   'wouldExceedBandwidth' returns false.
        //:
        //: 6 After waiting for the time interval, returned by
        //:   'calculateTimeToSubmit' 'wouldExceedBandwidth' returns false.
        //
        // Testing:
        //   bsls::TimeInterval wouldExceedBandwidth(currentTime);
        //   bsls::TimeInterval calculateTimeToSubmit(currentTime);
        // --------------------------------------------------------------------

        if (verbose)
         cout << endl
              << "CLASS METHOD 'wouldExceedBandwidth', 'calculateTimeToSubmit'"
              << endl
              << "============================================================"
              << endl;

        struct {
            int    d_line;
            Uint64 d_sustRate;
            Ti     d_sustWindow;
            Uint64 d_peakRate;
            Ti     d_peakWindow;
            Ti     d_creationTime;
            Uint64 d_nSubmits;
            Uint64 d_units;
            Ti     d_submitInterval;
            bool   d_exceedBw;
        } DATA[] = {

 // LINE S_RATE   S_WND  P_RATE   P_WND  TCREATE  N_SBMT UNITS INTERVAL EXCD_BW
 // ---- ------- ------- ------ -------- -------- ------ ----- -------- -------
    {L_,   100,   Ti(10), 1000, Ti(  1),  Ti(0),   500,  100,  Ti( 0.1), true},
    // C-2
    {L_,   100,   Ti(10), 1000, Ti(  1),  Ti(0),   50,   100,  Ti( 0.1), true},
    // C-1
    {L_,   100,   Ti(10), 1000, Ti(0.1),  Ti(0),   10,   100,  Ti(0.01), true},
    // C-3
    {L_,   100,   Ti(10), 1000, Ti(0.5),  Ti(0),   10,   100,  Ti(   1), false}
        };
        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const Uint64 LINE            = DATA[ti].d_line;
            const Uint64 S_RATE          = DATA[ti].d_sustRate;
            const Ti     S_WND           = DATA[ti].d_sustWindow;
            const Uint64 P_RATE          = DATA[ti].d_peakRate;
            const Ti     P_WND           = DATA[ti].d_peakWindow;
            const Ti     CREATION_TIME   = DATA[ti].d_creationTime;
            const Uint64 N_SUBMITS       = DATA[ti].d_nSubmits;
            const Uint64 UNITS           = DATA[ti].d_units;
            const Ti     SUBMIT_INTERVAL = DATA[ti].d_submitInterval;
            const bool   EXCEED_BW       = DATA[ti].d_exceedBw;

            Obj x(S_RATE, S_WND, P_RATE, P_WND, CREATION_TIME);

            Ti curTime = CREATION_TIME;

            for(unsigned int i = 0; i < N_SUBMITS; i++) {

                curTime += SUBMIT_INTERVAL;
                x.submit(UNITS);
            }

            LOOP_ASSERT(LINE,
                        EXCEED_BW == x.wouldExceedBandwidth(curTime));

            if (true == EXCEED_BW) {
                // C-4
                LOOP_ASSERT(LINE,
                            Ti(0) < x.calculateTimeToSubmit(curTime));
            }
            else {
                // C-5
                LOOP_ASSERT(LINE,
                            Ti(0) == x.calculateTimeToSubmit(curTime));
            }

            // C-6
            if (true == EXCEED_BW) {

                const Ti TIME_TO_SUBMIT = x.calculateTimeToSubmit(curTime);

                LOOP_ASSERT(LINE, Ti(0) == x.calculateTimeToSubmit(
                                                    curTime + TIME_TO_SUBMIT));

                LOOP_ASSERT(LINE, false == x.wouldExceedBandwidth(
                                                    curTime + TIME_TO_SUBMIT));
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'setRateLimits'
        //
        // Concerns:
        //: 1 The method sets the specified time windows intervals for peak and
        //:   sustained rate.
        //:
        //: 2 The method sets the specified peak and sustained rate.
        //:
        //: 3 Specifying wrong parameters for 'setRateLimits' parameters causes
        //:   certain behavior in special build configuration.
        //:
        //: 4 The 'sustainedRateWindow' is set to the interval, it takes to
        //:   drain the rounded down number of units, that may be drained at
        //:   the 'sustainedRateLimit' during the 'sustainedRateWindow',
        //:   specified for the CTOR.
        //:
        //: 5 The 'sustainedRateWindow' is set to the interval, it takes to
        //:   drain the rounded down number of units, that may be drained at
        //:   the 'sustainedRateLimit' during the 'sustainedRateWindow',
        //:   specified for the CTOR.
        //:
        //: 6 If the 'sustainedRateWindow', specified for CTOR is less than the
        //:   interval, required to drain 1 unit at sustainedRateLimit',
        //:   'sustainedRateWindow' is set to this interval.
        //
        // Testing:
        //   void setRateLimits(sR, sRW, pR, pRW, currentTime);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: 'setRateLimits'" << endl
                          << "========================" << endl;

        const Uint64 BIG_VAL       = 0xFFFFFFFFULL * 2;
        const Uint64 MAX_RT        = ULLONG_MAX/1000000;
        const Ti     CREATION_TIME = Ti(0);

        struct {
            int    d_line;
            Uint64 d_sustRate;
            Ti     d_sustWindow;
            Uint64 d_peakRate;
            Ti     d_peakWindow;
            Ti     d_expSustWnd;
            Ti     d_expPeakWnd;
        } DATA[] = {

         //  LINE   S_RATE    S_WND    P_RATE    P_WND     EXP_S_WND EXP_P_WND
         //  ---- --------- --------- -------- ----------  --------- ---------
            {L_,      100, Ti(   1),    1000, Ti(0.001),  Ti(   1), Ti(0.001)},
            {L_,       10, Ti( 0.5),     150, Ti(  0.1),  Ti( 0.5), Ti(  0.1)},
            {L_,  BIG_VAL, Ti(1000),  MAX_RT, Ti(   50),  Ti(1000), Ti(   50)},
            {L_, MAX_RT/2, Ti( 100),  MAX_RT, Ti(   90),  Ti( 100), Ti(   90)},
            // C-4
            {L_,        5, Ti( 0.3),   1000,  Ti(  0.1),  Ti( 0.2), Ti(  0.1)},
            // C-5
            {L_,        1, Ti(  10),     10,  Ti( 1.55),  Ti(  10), Ti(  1.5)},
            // C-6
            {L_,        1, Ti( 0.1),    100,  Ti( 0.05),  Ti(   1), Ti( 0.05)}

        };

        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);
        Obj x(1, Ti(10), 1, Ti(10), Ti(0));

        for (int ti = 0; ti < NUM_DATA; ++ti) {

            const Uint64 LINE          = DATA[ti].d_line;
            const Uint64 S_RATE        = DATA[ti].d_sustRate;
            const Ti     S_WND         = DATA[ti].d_sustWindow;
            const Uint64 P_RATE        = DATA[ti].d_peakRate;
            const Ti     P_WND         = DATA[ti].d_peakWindow;
            const Ti     EXP_S_WND     = DATA[ti].d_expSustWnd;
            const Ti     EXP_P_WND     = DATA[ti].d_expPeakWnd;

            x.setRateLimits(S_RATE, S_WND, P_RATE, P_WND);
            const Obj& X = x;

            // C-1, C-2
            LOOP_ASSERT(LINE, S_RATE        == X.sustainedRateLimit());
            LOOP_ASSERT(LINE, EXP_S_WND     == X.sustainedRateWindow());
            LOOP_ASSERT(LINE, P_RATE        == X.peakRateLimit());
            LOOP_ASSERT(LINE, EXP_P_WND     == X.peakRateWindow());
            LOOP_ASSERT(LINE, CREATION_TIME == X.lastUpdateTime());

            LOOP_ASSERT(LINE,
                        CREATION_TIME == X.statisticsCollectionStartTime());
        }

        // C-3
        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            // Zero rate or zero window.
            ASSERT_FAIL(x.setRateLimits(0, Ti(15), 10, Ti(10)));
            ASSERT_FAIL(x.setRateLimits(1, Ti( 0), 10, Ti(10)));
            ASSERT_FAIL(x.setRateLimits(1, Ti(15),  0, Ti(10)));
            ASSERT_FAIL(x.setRateLimits(1, Ti(15), 10, Ti( 0)));

            // Negative window.
            ASSERT_FAIL(x.setRateLimits(1, Ti(-15), 10, Ti(10)));
            ASSERT_FAIL(x.setRateLimits(1, Ti(15), 10, Ti(-10)));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'reserve', 'unitsReserved', 'submitReserved'
        //
        // Concerns:
        //: 1 'reserve' increments 'unitsReserved'.
        //:
        //: 2 'unitsReserved' returns number of units currently reserved.
        //:
        //: 3 'submitReserved' decrements 'unitsReserved' and submits units.
        //:
        //: 4 'submitReserved' may submit more units, than there are actually
        //:   reserved.
        //:
        //: 5 Specifying wrong parameters to 'reserve' causes certain behavior
        //:   in special build configuration.
        //:
        //: 6 Specifying wrong parameters to 'submitReserved' causes certain
        //:   behavior in special build configuration.
        //
        // Testing:
        //      void reserve(bsls::Types::Uint64 numOfUnits);
        //      bsls::Types::Uint64 unitsReserved();
        //      void submitReserved(bsls::Types::Uint64 numOfUnits);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING: 'reserve', 'unitsReserved', 'submitReserved'"
                 << endl
                 << "====================================================="
                 << endl;

        const Uint64 S_RATE = 1000;
        const Ti     S_WND  = Ti(1);
        const Uint64 P_RATE = 10000;
        const Ti     P_WND  = Ti(0.01);

        struct {
            int    d_line;
            Uint64 d_unitsToReserve;
            Uint64 d_unitsToSubmit;
            Uint64 d_expectedUnitsReserved;
        } DATA[] = {

         //  LINE    RESERVE       SUBMIT            EXP_RES
         //  ----  ----------  -------------   --------------------
            {L_,       1000,          300,                     700},
            // C-4
            {L_,       1000,         1000,                       0},
            {L_,       1000,          700,                     300},
            {L_, ULLONG_MAX, ULLONG_MAX/2, ULLONG_MAX-ULLONG_MAX/2}
        };

        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        for(int ti=0; ti<NUM_DATA; ti++) {

            const Uint64 LINE             = DATA[ti].d_line;
            const Uint64 UNITS_TO_RESERVE = DATA[ti].d_unitsToReserve;
            const Uint64 UNITS_TO_SUBMIT  = DATA[ti].d_unitsToSubmit;

            const Uint64 EXPECTED_UNITS_RESERVED =
                DATA[ti].d_expectedUnitsReserved;

            btls::LeakyBucket peakLb(
                           P_RATE,
                           btls::LeakyBucket::calculateCapacity(P_RATE, P_WND),
                           Ti(0));

            btls::LeakyBucket sustLb(
                           S_RATE,
                           btls::LeakyBucket::calculateCapacity(S_RATE, S_WND),
                           Ti(0));

            Obj x(S_RATE, S_WND, P_RATE, P_WND, Ti(0));

            // C-1, C-2
            x.reserve(UNITS_TO_RESERVE);
            LOOP_ASSERT(LINE, UNITS_TO_RESERVE == x.unitsReserved());

            sustLb.reserve(UNITS_TO_RESERVE);
            peakLb.reserve(UNITS_TO_RESERVE);

            Ti t1 = x.calculateTimeToSubmit(Ti(0));

            x.submitReserved(UNITS_TO_SUBMIT);
            sustLb.submitReserved(UNITS_TO_SUBMIT);
            peakLb.submitReserved(UNITS_TO_SUBMIT);

            Ti t2 = x.calculateTimeToSubmit(Ti(0));

            // C-3
            LOOP_ASSERT(LINE,
                        EXPECTED_UNITS_RESERVED == x.unitsReserved());

            // Check, whether units were actually submitted to rate limiter.

            if (false == x.wouldExceedBandwidth(Ti(0))) {

                bsls::Types::Uint64 freeUnits = bsl::min(
                                   sustLb.capacity() - sustLb.unitsInBucket(),
                                   peakLb.capacity() - peakLb.unitsInBucket());

                x.submit(freeUnits);

                LOOP_ASSERT(LINE, true == x.wouldExceedBandwidth(Ti(0)));
            }
            else {
                if (0 != UNITS_TO_SUBMIT) {
                    LOOP_ASSERT(LINE, t2 >= t1);
                }
            }
        }

        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            // Units reserved, trying to submit & reserve.

            Obj y1(100, Ti(10), 1000, Ti(1), Ti(0));

            // C-5
            y1.reserve(ULLONG_MAX);
            ASSERT_SAFE_FAIL(y1.submit(1));
            ASSERT_SAFE_FAIL(y1.reserve(1));

            Obj y2(100, Ti(10), 1000, Ti(1), Ti(0));

            y2.reserve(ULLONG_MAX/2);
            ASSERT_SAFE_FAIL(y2.submit((ULLONG_MAX>>2)*3));
            ASSERT_SAFE_FAIL(y2.reserve((ULLONG_MAX>>2)*3));

            // C-6
            Obj y3(100, Ti(10), 1000, Ti(1), Ti(0));
            y3.submit(1);
            y3.reserve(1);
            ASSERT_FAIL(y3.submitReserved(ULLONG_MAX));

            ASSERT_FAIL(y3.submitReserved(ULLONG_MAX/2+1));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'submit', 'wouldExceedBandwidth', 'calculateTimeToSubmit'
        //
        // Concerns:
        //: 1 'wouldExceedBandwidth' has the same behavior, when compared to
        //:    the pair of 'btls::LeakyBucket' objects.
        //:
        //: 2 Specifying wrong parameters to 'submit' causes certain behavior
        //:   in special build configuration.
        //
        // Testing:
        //   void submit(unsigned int numOfUnits);
        //   bool wouldExceedBandwidth(bsls::TimeInterval currentTime);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "'submit', 'wouldExceedBandwidth', calculateTimeToSubmit'"
                 << endl
                 << "========================================================"
                 << endl;

        struct {
            int    d_line;
            Uint64 d_sustRate;
            Ti     d_sustWindow;
            Uint64 d_peakRate;
            Ti     d_peakWindow;
            Ti     d_creationTime;
            Uint64 d_nSubmits;
            Uint64 d_units;
            Ti     d_submitInterval;
        } DATA[] = {

 //  LINE S_RATE   S_WND  P_RATE   P_WND    TCREATE  SUBMITS UNITS  INTERVAL
 //  ---- ------- ------- ------- --------- -------- ------- ------ --------
     {L_,   100,   Ti(10),  1000,  Ti(  1),   Ti(0),   500,    10,  Ti( 0.1)},
     {L_,   100,   Ti(10),  1000,  Ti(  1),   Ti(0),   50,    100,  Ti( 0.1)},
     {L_,   100,   Ti(10),  1000,  Ti(0.5),   Ti(0),   10,    100,  Ti(   1)},
     {L_,   100,   Ti(10),  1000,  Ti(0.1),   Ti(0),   10,    100,  Ti(0.01)}

        };

        const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

        if (verbose) cout << endl << "Testing: 'wouldExceedBandwidth'"
                          << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {

            const Uint64 LINE            = DATA[ti].d_line;
            const Uint64 S_RATE          = DATA[ti].d_sustRate;
            const Ti     S_WND           = DATA[ti].d_sustWindow;
            const Uint64 P_RATE          = DATA[ti].d_peakRate;
            const Ti     P_WND           = DATA[ti].d_peakWindow;
            const Ti     CREATION_TIME   = DATA[ti].d_creationTime;
            const Uint64 N_SUBMITS       = DATA[ti].d_nSubmits;
            const Uint64 UNITS           = DATA[ti].d_units;
            const Ti     SUBMIT_INTERVAL = DATA[ti].d_submitInterval;

            const Uint64 S_CAP = btls::LeakyBucket::calculateCapacity(S_RATE,
                                                                     S_WND);

            const Uint64 P_CAP = btls::LeakyBucket::calculateCapacity(P_RATE,
                                                                     P_WND);

            btls::LeakyBucket peakLB(P_RATE, P_CAP, CREATION_TIME);
            btls::LeakyBucket sustLB(S_RATE, S_CAP, CREATION_TIME);

            Obj x(S_RATE, S_WND, P_RATE, P_WND, CREATION_TIME);

            Ti curTime = CREATION_TIME;

            for(unsigned int i = 0; i < N_SUBMITS; i++) {

                curTime += SUBMIT_INTERVAL;

                peakLB.submit(UNITS);
                sustLB.submit(UNITS);

                x.submit(UNITS);

                const bool LB_RESULT = peakLB.wouldOverflow(curTime) ||
                                                 sustLB.wouldOverflow(curTime);

                LOOP_ASSERT(LINE,
                            LB_RESULT == x.wouldExceedBandwidth(curTime));

                const Ti LB_TIMESTAMP = bsl::max(sustLB.lastUpdateTime(),
                                                 peakLB.lastUpdateTime());

                LOOP_ASSERT(LINE, LB_TIMESTAMP == x.lastUpdateTime());
            }
        }

        if (verbose) cout << endl
                          << "Testing: 'calculateTimeToSubmit'" << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {

            const Uint64 LINE            = DATA[ti].d_line;
            const Uint64 S_RATE          = DATA[ti].d_sustRate;
            const Ti     S_WND           = DATA[ti].d_sustWindow;
            const Uint64 P_RATE          = DATA[ti].d_peakRate;
            const Ti     P_WND           = DATA[ti].d_peakWindow;
            const Ti     CREATION_TIME   = DATA[ti].d_creationTime;
            const Uint64 N_SUBMITS       = DATA[ti].d_nSubmits;
            const Uint64 UNITS           = DATA[ti].d_units;
            const Ti     SUBMIT_INTERVAL = DATA[ti].d_submitInterval;

            const Uint64 S_CAP = btls::LeakyBucket::calculateCapacity(
                S_RATE,
                S_WND);

            const Uint64 P_CAP = btls::LeakyBucket::calculateCapacity(
                P_RATE,
                P_WND);

            btls::LeakyBucket peakLB(P_RATE, P_CAP, CREATION_TIME);
            btls::LeakyBucket sustLB(S_RATE, S_CAP, CREATION_TIME);

            Obj x(S_RATE, S_WND, P_RATE, P_WND, CREATION_TIME);

            Ti curTime = CREATION_TIME;

            for(unsigned int i = 0; i < N_SUBMITS; i++) {

                curTime += SUBMIT_INTERVAL;

                peakLB.submit(UNITS);
                sustLB.submit(UNITS);

                x.submit(UNITS);

                const Ti LB_RESULT = bsl::max(
                                        peakLB.calculateTimeToSubmit(curTime),
                                        sustLB.calculateTimeToSubmit(curTime));

                LOOP_ASSERT(LINE,
                            LB_RESULT == x.calculateTimeToSubmit(curTime));

                const Ti LB_TIMESTAMP = bsl::max(sustLB.lastUpdateTime(),
                                                 peakLB.lastUpdateTime());

                LOOP_ASSERT(LINE, LB_TIMESTAMP == x.lastUpdateTime());
            }

            const Ti CHECK_TIME = curTime + x.calculateTimeToSubmit(curTime);

            LOOP_ASSERT(LINE, false == x.wouldExceedBandwidth(CHECK_TIME));
        }

        // C-2
        if (verbose) cout << endl << "Negative Testing" << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                bsls::AssertTest::failTestDriver);

            // Units submitted, trying to submit & reserve.

            Obj x1(100, Ti(10), 1000, Ti(1), Ti(0));

            x1.submit(ULLONG_MAX);
            ASSERT_SAFE_FAIL(x1.submit(1));
            ASSERT_SAFE_FAIL(x1.reserve(1));

            Obj x2(100, Ti(10), 1000, Ti(1), Ti(0));

            x2.submit(ULLONG_MAX/2);
            ASSERT_SAFE_FAIL(x2.submit((ULLONG_MAX>>2)*3));
            ASSERT_SAFE_FAIL(x2.reserve((ULLONG_MAX>>2)*3));

        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Create an object using the value constructor.  Verify that each
        //:   basic accessor, invoked on a reference providing non-modifiable
        //:   access to the object, returns the expected value.  (C-2)
        //:
        //: 2 Create another object using the value constructor having a
        //:   different value for each attribute compared to a default
        //:   constructed object.  Verify that each basic accessor, invoked on
        //:   a reference providing non-modifiable access to the object,
        //:   returns the expected value.  (C-1)
        //
        // Testing:
        //   bsls::Types::Uint64 peakRateLimit() const;
        //   bsls::Types::Uint64 sustainedRateLimit() const;
        //   bsls::TimeInterval peakRateWindow() const;
        //   bsls::TimeInterval sustainedRateWindow() const;
        //   bsls::TimeInterval lastUpdateTime() const;
        //   bsls::TimeInterval statisticsCollectionStartTime() const;
        // ----------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        Obj mX(1, Ti(10), 10, Ti(1), Ti(0)); const Obj& X = mX;

        ASSERT(10     == X.peakRateLimit());
        ASSERT(Ti(1)  == X.peakRateWindow());
        ASSERT(1      == X.sustainedRateLimit());
        ASSERT(Ti(10) == X.sustainedRateWindow());
        ASSERT(Ti(0)  == X.lastUpdateTime());
        ASSERT(Ti(0)  == X.statisticsCollectionStartTime());
        ASSERT(0      == X.unitsReserved());

        Uint64 SR = 100;
        Ti SW(50);
        Uint64 PR = 1000;
        Ti PW(10);
        Ti CT(6);
        Uint64 RU = 22;

        Obj mY(SR, SW, PR, PW, CT); const Obj& Y = mY;
        mY.reserve(RU);
        ASSERT(PR == Y.peakRateLimit());
        ASSERT(PW == Y.peakRateWindow());
        ASSERT(SR == Y.sustainedRateLimit());
        ASSERT(SW == Y.sustainedRateWindow());
        ASSERT(CT == Y.lastUpdateTime());
        ASSERT(CT == Y.statisticsCollectionStartTime());
        ASSERT(RU == Y.unitsReserved());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor can create an object having any value that
        //:   does not violate the constructor's documented preconditions.
        //:
        //: 2 The capacity of a underlying leaky bucket is set to the
        //:   rounded-down product of 'sustainedRateWindow' and
        //:   'sustainedRateLimit'.  As a result, the 'sustainedRateLimit'
        //:   retrieved from an object's accessor may be one less than the
        //:   original specified value.
        //:
        //: 3 If less than one unit is transmitted during the specified
        //:   'sustainedRateWindow' at 'sustainedRateLimit' then
        //:   'sustainRateWindow' will be set to the time period during which 1
        //:   unit is transmitted.
        //:
        //: 4 The capacity of a underlying leaky bucket is set to the
        //:   rounded-down product of 'peakRateWindow' and 'peakRateLimit'.  As
        //:   a result, the 'peakRateLimit' retrieved from an object's accessor
        //:   may be one less than the original specified value.
        //:
        //: 5 If less than one unit is transmitted during the specified
        //:   'peakRateWindow' at 'peakRateLimit' then 'sustainRateWindow' will
        //:   be set to the time period during which 1 unit is transmitted.
        //:
        //: 6 QoI: Assert preconditions violations are detected when enabled.
        //
        // Plan:
        //: 2 Use a table driven test, for a set of varied possible attributes,
        //:   to invoke the value constructor and verify the values of the
        //:   resulting objects.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-6)
        //
        // Testing:
        //   btls::RateLimiter(sR, sRW, pR, pRW, currentTime);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CTOR" << endl
                          << "==========" << endl;

        // Testing value constructor.
        {
            const Uint64 BIG_VAL = 0xFFFFFFFFULL * 2;
            const Uint64 MAX_RT  = ULLONG_MAX/1000000;

            struct {
                int    d_line;
                Uint64 d_sustRate;
                Ti     d_sustWindow;
                Uint64 d_peakRate;
                Ti     d_peakWindow;
                Ti     d_creationTime;
                Ti     d_expSustWnd;
                Ti     d_expPeakWnd;
            } DATA[] = {

  // LINE   S_RATE    S_WND    P_RATE    P_WND    TCREATE  EXP_S_WND EXP_P_WND
  // ---- --------- --------- -------- ---------- -------- --------- ---------
    {L_,      100, Ti(   1),    1000, Ti(0.001), Ti(  0), Ti(   1), Ti(0.001)},
    {L_,       10, Ti( 0.5),     150, Ti(  0.1), Ti(  0), Ti( 0.5), Ti(  0.1)},
    {L_,  BIG_VAL, Ti(1000),  MAX_RT, Ti(   50), Ti(999), Ti(1000), Ti(   50)},
    {L_, MAX_RT/2, Ti( 100),  MAX_RT, Ti(   90), Ti(500), Ti( 100), Ti(   90)},
    {L_,        5, Ti( 0.3),   1000,  Ti(  0.1), Ti(  0), Ti( 0.2), Ti(  0.1)},
    {L_,        1, Ti(  10),     10,  Ti( 1.55), Ti(  0), Ti(  10), Ti(  1.5)},
    {L_,        1, Ti( 0.1),    100,  Ti( 0.05), Ti(  0), Ti(   1), Ti( 0.05)}
            };
            const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const Uint64 LINE          = DATA[ti].d_line;
                const Uint64 S_RATE        = DATA[ti].d_sustRate;
                const Ti     S_WND         = DATA[ti].d_sustWindow;
                const Uint64 P_RATE        = DATA[ti].d_peakRate;
                const Ti     P_WND         = DATA[ti].d_peakWindow;
                const Ti     CREATION_TIME = DATA[ti].d_creationTime;
                const Ti     EXP_S_WND     = DATA[ti].d_expSustWnd;
                const Ti     EXP_P_WND     = DATA[ti].d_expPeakWnd;

                Obj x(S_RATE, S_WND, P_RATE, P_WND, CREATION_TIME);
                const Obj& X = x;

                LOOP_ASSERT(LINE, 0             == X.unitsReserved());
                LOOP_ASSERT(LINE, S_RATE        == X.sustainedRateLimit());
                LOOP_ASSERT(LINE, P_RATE        == X.peakRateLimit());
                LOOP_ASSERT(LINE, CREATION_TIME == X.lastUpdateTime());

                LOOP_ASSERT(LINE,
                            CREATION_TIME ==
                                            X.statisticsCollectionStartTime());

                Ti delta = EXP_S_WND - X.sustainedRateWindow();
                LOOP_ASSERT(LINE,
                            0 == delta.seconds() &&
                            delta.nanoseconds() >= 0 &&
                            delta.nanoseconds() <= 1 );

                delta = EXP_P_WND - X.peakRateWindow();
                LOOP_ASSERT(LINE,
                            0 == delta.seconds() &&
                            delta.nanoseconds() >= 0 &&
                            delta.nanoseconds() <= 1 );
            }

            // Testing Coexistence
            {
                Obj y(DATA[0].d_sustRate,
                      DATA[0].d_sustWindow,
                      DATA[0].d_peakRate,
                      DATA[0].d_peakWindow,
                      DATA[0].d_creationTime);

                Obj z(DATA[1].d_sustRate,
                      DATA[1].d_sustWindow,
                      DATA[1].d_peakRate,
                      DATA[1].d_peakWindow,
                      DATA[1].d_creationTime);

                ASSERT(DATA[0].d_sustRate     == y.sustainedRateLimit());
                ASSERT(DATA[0].d_peakRate     == y.peakRateLimit());
                ASSERT(DATA[0].d_creationTime == y.lastUpdateTime());
                ASSERT(DATA[0].d_creationTime ==
                                            y.statisticsCollectionStartTime());

                Ti delta = DATA[0].d_sustWindow - y.sustainedRateWindow();
                ASSERT(0 == delta.seconds() &&
                       delta.nanoseconds() >= 0 &&
                       delta.nanoseconds() <= 1 );

                delta = DATA[0].d_peakWindow - y.peakRateWindow();
                ASSERT(0 == delta.seconds() &&
                       delta.nanoseconds() >= 0 &&
                       delta.nanoseconds() <= 1 );

                ASSERT(DATA[1].d_sustRate     == z.sustainedRateLimit());
                ASSERT(DATA[1].d_peakRate     == z.peakRateLimit());
                ASSERT(DATA[1].d_creationTime == z.lastUpdateTime());
                ASSERT(DATA[1].d_creationTime ==
                                            z.statisticsCollectionStartTime());

                delta = DATA[1].d_sustWindow - z.sustainedRateWindow();
                ASSERT(0 == delta.seconds() &&
                       delta.nanoseconds() >= 0 &&
                       delta.nanoseconds() <= 1 );

                delta = DATA[1].d_peakWindow - z.peakRateWindow();
                ASSERT(0 == delta.seconds() &&
                       delta.nanoseconds() >= 0 &&
                       delta.nanoseconds() <= 1 );
            }
        }

        // Negative Testing
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            // Zero rate or zero window.
            ASSERT_SAFE_FAIL(Obj x1(0, Ti(15), 10, Ti(10), Ti(0)));
            ASSERT_SAFE_FAIL(Obj x1(1, Ti( 0), 10, Ti(10), Ti(0)));
            ASSERT_SAFE_FAIL(Obj x1(1, Ti(15),  0, Ti(10), Ti(0)));
            ASSERT_SAFE_FAIL(Obj x1(1, Ti(15), 10, Ti( 0), Ti(0)));

            // Negative window.
            ASSERT_SAFE_FAIL(Obj x1(1, Ti(-15), 10, Ti(10), Ti(0)));
            ASSERT_SAFE_FAIL(Obj x1(1, Ti(15), 10, Ti(-10), Ti(0)));

        }
      } break;
      case 1: {
        // ----------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //  Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //  This "test" *exercises* basic functionality, but *tests*
        //  nothing.
        // ----------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj x(1, Ti(10), 1, Ti(10), Ti(0));
        ASSERT(1      == x.sustainedRateLimit());
        ASSERT(Ti(10) == x.sustainedRateWindow());
        ASSERT(1      == x.peakRateLimit());
        ASSERT(Ti(10) == x.peakRateWindow());
        ASSERT(Ti(0)  == x.lastUpdateTime());
        ASSERT(Ti(0)  == x.statisticsCollectionStartTime()); // C-10
        ASSERT(0      == x.unitsReserved());

        Ti  currentTime(1);

        x.setRateLimits(1000, Ti(1), 10000, Ti(0.1));
        x.reset(currentTime);

        ASSERT(1000   == x.sustainedRateLimit());
        ASSERT(Ti(1)  == x.sustainedRateWindow());

        x.submit(500);
        x.reserve(250);
        ASSERT(250 == x.unitsReserved());

        ASSERT(false == x.wouldExceedBandwidth(currentTime));
        ASSERT(Ti(0) == x.calculateTimeToSubmit(currentTime));
        x.submitReserved(250);
        x.submit(750);

        ASSERT(0 == x.unitsReserved());

        Ti delta = Ti(0.501) - x.calculateTimeToSubmit(currentTime);
        ASSERT(0 == delta.seconds() &&
               delta.nanoseconds() >= 0 &&
               delta.nanoseconds() <= 1 );

        currentTime.addMilliseconds(500);
        ASSERT(0 == delta.seconds() &&
               delta.nanoseconds() >= 0 &&
               delta.nanoseconds() <= 1 );

        currentTime.addMilliseconds(100);
        x.updateState(currentTime);

        x.submit(100);

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

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
