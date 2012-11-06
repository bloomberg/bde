//btes_ratelimiter.t.cpp                                              -*-C++-*-

#include <btes_ratelimiter.h>

#include <bcemt_threadutil.h>

#include <bdema_managedptr.h>

#include <bdet_timeinterval.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>

#include <bsl_c_math.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <btes_leakybucket.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
////                              Overview
//                              --------
// The component under test implements a mechanism.
//
// Primary Manipulators:
//: o 'setRateAndCapacity'
//
// Basic Accessors:
// o 'rate'
// o 'capacity'
// o 'lastUpdateTime'
// o 'unitsInBucket'
// o 'unitsReserved'
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
//-----------------------------------------------------------------------------
//
// CREATORS
//  [3] btes_RateLimiter();
//  [3] btes_RateLimiter(bsls_Types::Uint64       sustainedRateLimit,
//                       const bdet_TimeInterval& sustainedRateWindow,
//                       bsls_Types::Uint64       peakRateLimit,
//                       const bdet_TimeInterval& peakRateWindow,
//                       const bdet_TimeInterval& currentTime);
//
// MANIPULATORS
//  [ 6]     void setRateLimits(bsls_Types::Uint64        newSustainedRateLimit,
//                              const bdet_TimeInterval& newSustainedRateWindow,
//                              bsls_Types::Uint64       newPeakRateLimit,
//                              const bdet_TimeInterval& newPeakRateWindow);
//  [ 4] void submit(bsls_Types::Uint64 numOfUnits);
//  [ 5] void reserve(bsls_Types::Uint64 numOfUnits);
//  [ 8] void updateState(const bdet_TimeInterval& currentTime);
//  [ 4] bool wouldExceedBandwidth(const bdet_TimeInterval& currentTime);
//  [ 5] void submitReserved(bsls_Types::Unit64 numOfUnits);
//  [12] void cancelReserved(bsls_Types::Unit64 numOfUnits);
//  [10] void resetStatistics();
//  [11] void reset(const bdet_TimeInterval& currentTime);
//  [ 7] bdet_TimeInterval calculateTimeToSubmit(
//                                       const bdet_TimeInterval& currentTime);
// ACCESSORS
//  [ 3] bsls_Types::Uint64 peakRateLimit() const;
//  [ 3] bsls_Types::Uint64 sustainedRateLimit() const;
//  [ 3] bdet_TimeInterval peakRateWindow() const;
//  [ 3] bdet_TimeInterval sustainedRateWindow() const;
//  [ 5] bsls_Types::Uint64 unitsReserved() const;
//  [ 3] bdet_TimeInterval lastUpdateTime() const;
//  [ 9] void getStatistics(bsls_Types::Uint64* submittedUnits,
//                          bsls_Types::Uint64* unusedUnits) const;
//  [ 3] bdet_TimeInterval statisticsCollectionStartTime() const;
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [14] USAGE EXAMPLE
// [ 3] All accessor methods are declared 'const'.
// [ *] All creator/manipulator ptr./ref. parameters are 'const'.
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
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

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

//=============================================================================
//                  STANDARD BDE VARIADIC ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef btes_RateLimiter    Obj;
typedef bdet_TimeInterval   Ti;
typedef bsls_Types::Uint64  Uint64;
typedef unsigned int        uint;

// Function used for testing usage example.

bool sendData(size_t dataSize)
      // Send a specified 'dataSize' amount of data over the network
      // return 'true' if data was sent successfully and 'false' otherwise.
{
//..
// For simplicity, 'sendData' will not actually send any data and will
// always return 'true'.
//..
   return true;
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    switch (test) {

        case 14: {
            // ----------------------------------------------------------------
            // USAGE EXAMPLE
            //   The usage example provided in the component header file must
            //   compile, link, and run on all platforms as shown.
            //
            // Plan:
            //   Incorporate usage example from header into driver, remove
            //   leading comment characters and replace 'assert' with 'ASSERT'.
            //
            // Testing:
            //   USAGE EXAMPLE
            // ----------------------------------------------------------------

            if (verbose) cout << endl
                              << "TESTING USAGE EXAMPLE" << endl
                              << "=====================" << endl;

// In this example, we are going to send a fixed amount of data in chunks, an
// use a 'btes_RateLimiter' to prevent our application from overloading its
// network connection.
//..

//..
// First we define the size of data we are going to transmit and a counter
// of data that is actually sent.  We are going to send data by 64 byte
// chunks.
//..
  bsls_Types::Uint64 bytesSent  = 0;
  bsls_Types::Uint64 sizeOfData = 10 * 1024; // in bytes
  bsls_Types::Uint64 chunkSize  = 64;        // in bytes
//..
// Then, we select a sustained-rate time-window, and a peak-rate time window,
// over which the average rate will not exceed the respective maximum. Note
// that the sustained-rate is meant to long-term average rate of resource
// usage, where-as the peak-rate is meant to limit spikes in resource usage,
// so the sustained-rate time-window is typically significantly longer than the
// peak-rate time-window:
//..
  bsls_Types::Uint64 sustainedRateLimit = 1024;
  bdet_TimeInterval  sustainedRateWindow(0.5);
  bsls_Types::Uint64 peakRateLimit = 2048;
  bdet_TimeInterval  peakRateWindow(0.0625);
//..
// Next, we create a'btes_RateLimiter' object, providing the
// 'sustainedRateLimit', 'peakRate', and respective time-windows defined
// earlier. Note that we provide a starting time stamp that is an interval from
// the UNIX epoch-time, and subsequent time stamps must be from the same
// reference point:
//..
  bdet_TimeInterval now = bdetu_SystemTime::now();
  btes_RateLimiter  rateLimiter(sustainedRateLimit,
                                sustainedRateWindow,
                                peakRateLimit,
                                peakRateWindow,
                                now);
//..
// Now, we build a loop and for each iteration we check whether submitting
// another chunk of data to the rate limiter would cause exceeding the defined
// bandwidth limits.  If not, we can send the data and submit it to the rate
// limiter.  The loop terminates when all the data is sent.  Note that 'submit'
// is invoked only after a successful operation on the resource.
//..
  while (bytesSent < sizeOfData) {
      now = bdetu_SystemTime::now();
      if (!rateLimiter.wouldExceedBandwidth(now)) {
          if (true == sendData(chunkSize)) {
              rateLimiter.submit(chunkSize);
              bytesSent += chunkSize;
          }
      }
//..
// Finally, if it is not possible to submit a new chunk of data without
// exceeding bandwidth, we invoke the 'calculateTimeToSubmit' method to
// determine how much time is required to submit a new chunk without causing
// overflow. We round up the number of microseconds in time interval.
//..
      else {
          bdet_TimeInterval timeToSubmit =
                                      rateLimiter.calculateTimeToSubmit(now);
          bsls_Types::Uint64 uS = timeToSubmit.totalMicroseconds() +
                                 (timeToSubmit.nanoseconds() % 1000) ? 1 : 0;
          bcemt_ThreadUtil::microSleep(uS);
      }
  }
//..
// Notice that in a multi-threaded application it is appropriate to put the
// thread into the 'sleep' state, in order to avoid busy-waiting.
        } break;

        case 13: {
            // ----------------------------------------------------------------
            // FUNCTIONALITY
            //
            // Concerns:
            //   1 'btes_LeakyBucket' keeps specified load rate and allows
            //     deviation from the specified rate spikes not bigger than
            //     specified window size divided by test duration time.
            //
            // Plan:
            //
            // Testing:
            //   void submit(unsigned int numOfUnits);
            //
            //   bool wouldExceedBandwidth(
            //                           const bdet_TimeInterval& currentTime);
            //
            //   bdet_TimeInterval calculateTimeToSubmit(
            //                           const bdet_TimeInterval& currentTime);
            // ----------------------------------------------------------------
        } break;

        case 12: {
            // ----------------------------------------------------------------
            // CLASS METHOD 'cancelReserved'
            //
            // Concerns:
            //
            //   1 The method decrements the number of 'unitsReserved'.
            //
            //   2 The method does not submit units.
            //
            //   3 The method correctly handles the case, when 'numOfUnits' is
            //    greater than the number of 'unitsReserved'.
            //
            // Testing:
            //      void cancelReserved(bsls_Types::Uint64 numOfUnits)
            //
            // ----------------------------------------------------------------
            
            if (verbose) cout << endl << "TESTING: 'cancelReserved'" <<
                                 endl << "=========================" <<
                                 endl;

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

                btes_LeakyBucket peakLb(P_RATE, 
                                        btes_LeakyBucket::calculateCapacity(
                                                                       P_RATE,
                                                                       P_WND),
                                        Ti(0));

                btes_LeakyBucket sustLb(S_RATE, 
                                        btes_LeakyBucket::calculateCapacity(
                                                                       S_RATE,
                                                                       S_WND),
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

                // Check, that no units were submitted to the rate limiter.
                // C-2

                if (false == x.wouldExceedBandwidth(Ti(0))) {

                    bsls_Types::Uint64 freeUnits = bsl::min(
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
                bsls_AssertFailureHandlerGuard hG(
                                              bsls_AssertTest::failTestDriver);

                Obj y(100, Ti(10), 1000, Ti(1), Ti(0));
                y.submit(100);
                y.reserve(100);
                ASSERT_SAFE_PASS(y.submitReserved(100));
                ASSERT_SAFE_FAIL(y.submitReserved(1));
                y.reserve(100);
                ASSERT_SAFE_FAIL(y.submitReserved(101));
            }

        } break;

        case 11: {
            // ----------------------------------------------------------------
            // CLASS METHOD 'reset'
            //
            // Concerns:
            //   1 The object parameters are not affected by the 'reset'
            //   method.
            //
            //   2 The object state is set to initial state and 'lastUpdateTime'
            //     is recorded correctly when 'reset' is invoked
            //
            //   3 'reset' method invokes 'resetStatistics' method and
            //     'statisticsCollectionStartTime' is updated
            //
            // Plan:
            //
            // Testing:
            //   void reset(const bdet_TimeInterval& currentTime);
            // ----------------------------------------------------------------

            if (verbose) cout <<
                         endl << "TESTING: 'reset'" << endl
                              << "================" << endl;

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

                //LINE  CTIME   UNITS TRESET
                //----  ------- ----- ------
                {  L_,  Ti( 0),    0, Ti( 0) },
                {  L_,  Ti( 0), 1000, Ti( 0) },
                {  L_,  Ti( 0), 2000, Ti( 0) },
                {  L_,  Ti(50),    0, Ti(60) },
                {  L_,  Ti(50), 1000, Ti(60) },
                {  L_,  Ti(50),    0, Ti( 0) },
                {  L_,  Ti(50), 1000, Ti( 0) }
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
            // ----------------------------------------------------------------
            // CLASS METHOD 'resetStatistics'
            //
            // Concerns:
            //   1 'resetStatistics' resets unit statistics counter to 0.
            //
            //   2 'resetStatistics' updates 'statisticsCollectionStartTime' time
            //      correctly.
            //
            //   3 'resetStatistics' does not alter object state except for
            //     submitted units counter and 'lastReset' time.
            //
            // Plan:
            //
            // Testing:
            //   void resetStatistics();
            //
            // ----------------------------------------------------------------

            if (verbose) cout << endl << "TESTING: 'resetStatistics'"<<
                                 endl << "=========================="<<
                                 endl;

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
                floor((UPD_TIME - CREATION_TIME).totalSecondsAsDouble()*S_RATE)-
                EXP_USED;

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
            // ----------------------------------------------------------------
            // CLASS METHOD 'getStatistics'
            //
            // Concerns:
            //   1 'getStatistics' returns 0 for a new object, created by
            //     default CTOR.
            //
            //   2 'getStatistics' returns 0 for a new object, created by
            //     value CTOR.
            //
            //   3 'getStatistics' returns correct numbers of used and unused
            //     units (the calculation is base upon the sustained rate
            //     sustained rate after a sequence of 'submit' and
            //     'updateState' calls.
            //
            //   4 Specifying invalid parameters for 'getStatistics()' causes
            //     certain behavior in special build configuration.
            //
            //   5 Statistics is calculated for interval between
            //     'statisticsCollectionStartTime' and 'lastUpdateTime'
            //
            // Plan:
            //
            // Testing:
            //   void getStatistics(bsls_Types::Uint64* submittedUnits,
            //                      bsls_Types::Uint64* unusedUnits) const;
            //
            // ----------------------------------------------------------------

            if (verbose) cout << endl << "TESTING: 'getStatistics'"<<
                                 endl << "========================"<<
                                 endl;

            Uint64 usedUnits   = 0;
            Uint64 unusedUnits = 0;

            // C-1

            if (verbose) cout
                           << endl
                           << "Testing: statistics after default construction"
                           << endl;
            {
                Obj x;

                x.getStatistics(&usedUnits, &unusedUnits);
                ASSERT(0 == usedUnits);
                ASSERT(0 == unusedUnits);
            }

            // C-2

            if (verbose) cout
                           << endl
                           << "Testing: statistics after value construction"
                           << endl;
            {
                Obj x(1000, Ti(1), 10000, Ti(0.01), Ti(42.4242));

                x.getStatistics(&usedUnits, &unusedUnits);
                ASSERT(0 == usedUnits);
                ASSERT(0 == unusedUnits);
            }

            // C-3

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

            // C-4

            if (verbose) cout << endl << "Negative testing" << endl;
            {
                bsls_AssertFailureHandlerGuard hG(
                                              bsls_AssertTest::failTestDriver);
                Obj x;

                ASSERT_SAFE_FAIL(x.getStatistics(0,&unusedUnits));
                ASSERT_SAFE_FAIL(x.getStatistics(&usedUnits,0));
                ASSERT_SAFE_FAIL(x.getStatistics(0,0));
            }

            // C-5

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
            // ----------------------------------------------------------------
            // CLASS METHOD 'updateState'
            //
            // Concerns:
            //
            //   1 The method updates 'lastUpdateTime'.
            //
            //   2 The method udpdates 'lastUpdateTime' if the specified
            //     'currentTime' precedes 'lastUpdateTime'.
            //
            //   3 The method does not affect 'statisticsCollectionStartTime' if time
            //     does not go backwards.
            //
            //   4 The method updates 'statisticsCollectionStartTime', if 'lastUpdateTime'
            //     precedes 'statisticsCollectionStartTime'.
            //
            // Plan:
            //
            // Testing:
            //   void updateState(const bdet_TimeInterval& currentTime);
            // ----------------------------------------------------------------

            if (verbose) cout << endl << "TESTING: 'updateState'"<<
                                 endl << "======================"<<
                                 endl;

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
            // ----------------------------------------------------------------
            // CLASS METHOD 'wouldExceedBandwidth', 'calculateTimeToSubmit'.
            //
            // Concerns:
            //
            //   1 'wouldExceedBandwidth' returns true when peak rate limit is
            //     exceeded.
            //
            //   2 'wouldExceedBandwidth' returns true, when sustained rate
            //     limit is exceeded.
            //
            //   3 'wouldExceedBandwidth' returns false, if load does not
            //     exceed both limits.
            //
            //   4 'calculateTimeToSubmit' returns a non-zero interval when
            //     'wouldExceedBandwidth' returns true.
            //
            //   5 'calculateTimeToSubmit' returns a zero interval when
            //     'wouldExceedBandwidth' returns false.
            //
            //   6 After waiting for the time interval, returned by
            //     'calculateTimeToSubmit' 'wouldExceedBandwidth' returns
            //     false.
            //
            // Plan:
            //
            // Testing:
            //   bdet_TimeInterval wouldExceedBandwidth(
            //                           const bdet_TimeInterval& currentTime);
            //   bdet_TimeInterval calculateTimeToSubmit(
            //                           const bdet_TimeInterval& currentTime);
            // ----------------------------------------------------------------

            if (verbose) cout <<
                         endl << "TESTING: 'wouldExceedBandwidth'"<<
                         endl << "         'calculateTimeToSubmit', blackbox"<<
                         endl << "=========================================="<<
                         endl;

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

//  LINE S_RATE   S_WND  P_RATE   P_WND  TCREATE  N_SBMT UNITS INTERVAL EXCD_BW
//  ---- ------- ------- ------ -------- -------- ------ ----- -------- -------

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

                for(int i = 0; i < N_SUBMITS; i++) {

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
            // ----------------------------------------------------------------
            // CLASS METHOD 'setRateLimits'
            //
            // Concerns:
            //
            //   1 The method sets the specified time windows intervals for
            //     peak and sustained rate.
            //
            //   2 The method sets the specified peak and sustained rate.
            //
            //   3 Specifying wrong parameters for 'setRateLimits'
            //     parameters causes certain behavior in special build
            //     configuration.
            //
            //   4 The 'sustainedRateWindow' is set to the interval, it
            //     takes to drain the rounded down number of units, that may
            //     be drained at the 'sustainedRateLimit' during the
            //     'sustainedRateWindow', specified for the CTOR.
            //
            //   5 The 'sustainedRateWindow' is set to the interval, it
            //     takes to drain the rounded down number of units, that may
            //     be drained at the 'sustainedRateLimit' during the
            //     'sustainedRateWindow', specified for the CTOR.
            //
            //   6 If the 'sustainedRateWindow', specified for CTOR is less
            //     than the interval, required to drain 1 unit at
            //     sustainedRateLimit', 'sustainedRateWindow' is set to this
            //     interval.
            //
            // Plan:
            //
            // Testing:
            //   void setRateLimits(bsls_Types::Uint64     sustainedRateLimit,
            //                    const bdet_TimeInterval& sustainedRateWindow,
            //                    bsls_Types::Uint64       peakRateLimit,
            //                    const bdet_TimeInterval& peakRateWindow);
            //
            // ----------------------------------------------------------------

            if (verbose) cout << endl << "TESTING: 'setRateLimits'" << endl
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
            Obj x;

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
                bsls_AssertFailureHandlerGuard hG(
                                              bsls_AssertTest::failTestDriver);

                // Zero rate or zero window.

                ASSERT_SAFE_FAIL(x.setRateLimits(0, Ti(15), 10, Ti(10)));
                ASSERT_SAFE_FAIL(x.setRateLimits(1, Ti( 0), 10, Ti(10)));
                ASSERT_SAFE_FAIL(x.setRateLimits(1, Ti(15),  0, Ti(10)));
                ASSERT_SAFE_FAIL(x.setRateLimits(1, Ti(15), 10, Ti( 0)));

                // Negative window.

                ASSERT_SAFE_FAIL(x.setRateLimits(1, Ti(-15), 10, Ti(10)));
                ASSERT_SAFE_FAIL(x.setRateLimits(1, Ti(15), 10, Ti(-10)));

            }

        } break;

        case 5: {
            // ----------------------------------------------------------------
            // CLASS METHOD 'reserve', 'unitsReserved', 'submitReserved'
            //
            // Concerns:
            //
            //   1 'reserve' increments 'unitsReserved'.
            //
            //   2 'unitsReserved' returns number of units currently reserved.
            //
            //   3 'submitReserved' decrements 'unitsReserved' and submits 
            //     units.
            //
            //   4 'submitReserved' may submit more units, than there are
            //     actually reserved.
            //
            //   5 Specifying wrong parameters to 'reserve' causes certain
            //     behavior in special build configuration.
            //
            //   6 Specifying wrong parameters to 'submitReserved' causes
            //     certain behavior in special build configuration.
            //
            // Testing:
            //      void reserve(bsls_Types::Uint64 numOfUnits);
            //      bsls_Types::Uint64 unitsReserved();
            //      void submitReserved(bsls_Types::Uint64 numOfUnits);
            //
            //-----------------------------------------------------------------

            if (verbose) cout <<
                         endl << "TESTING: 'reserve', 'unitsReserved'," <<
                                 "'submitReserved'" <<
                         endl << "====================================" <<
                                 "================";

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
                
                btes_LeakyBucket peakLb(P_RATE, 
                                        btes_LeakyBucket::calculateCapacity(
                                                                       P_RATE,
                                                                       P_WND),
                                        Ti(0));

                btes_LeakyBucket sustLb(S_RATE, 
                                        btes_LeakyBucket::calculateCapacity(
                                                                       S_RATE,
                                                                       S_WND),
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

                // Check, whether units were actually submited to rate limiter.

                if (false == x.wouldExceedBandwidth(Ti(0))) {

                    bsls_Types::Uint64 freeUnits = bsl::min(
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
                bsls_AssertFailureHandlerGuard hG(
                                              bsls_AssertTest::failTestDriver);

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
                ASSERT_SAFE_FAIL(y3.submitReserved(ULLONG_MAX));

                ASSERT_SAFE_FAIL(y3.submitReserved(ULLONG_MAX/2+1));
            }

        } break;

        case 4: {
            // ----------------------------------------------------------------
            // 'submit', 'wouldExceedBandwidth', 'calculateTimeToSubmit'
            //
            // Concerns:
            //
            //   1 'wouldExceedBandwidth' has the same behavior, when compared
            //      to the pair of 'btes_LeakyBucket' objects.
            //
            //   2 Specifying wrong parameters to 'submit' causes certain
            //     behavior in special build configuration.
            //
            // Plan:
            //
            // Testing:
            //   void submit(unsigned int numOfUnits);
            //   bool wouldExceedBandwidth(bdet_TimeInterval currentTime);
            // ----------------------------------------------------------------

            if (verbose) cout <<
                         endl << "TESTING: 'submit', 'wouldExceedBandwidth',"<<
                         endl << "         'calculateTimeToSubmit', whitebox"<<
                         endl << "=========================================="<<
                         endl;

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

//  LINE S_RATE   S_WND  P_RATE   P_WND    TCREATE  NSUBMITS UNITS  INTERVAL
//  ---- ------- ------- ------- --------- -------- -------- ------ --------

    {L_,   100,   Ti(10),  1000,  Ti(  1),   Ti(0),   500,     10,  Ti( 0.1)},
    {L_,   100,   Ti(10),  1000,  Ti(  1),   Ti(0),   50,     100,  Ti( 0.1)},
    {L_,   100,   Ti(10),  1000,  Ti(0.5),   Ti(0),   10,     100,  Ti(   1)},
    {L_,   100,   Ti(10),  1000,  Ti(0.1),   Ti(0),   10,     100,  Ti(0.01)}

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

                const Uint64 S_CAP = btes_LeakyBucket::calculateCapacity(
                                                                        S_RATE,
                                                                        S_WND);

                const Uint64 P_CAP = btes_LeakyBucket::calculateCapacity(
                                                                        P_RATE,
                                                                        P_WND);

                btes_LeakyBucket peakLB(P_RATE, P_CAP, CREATION_TIME);
                btes_LeakyBucket sustLB(S_RATE, S_CAP, CREATION_TIME);

                Obj x(S_RATE, S_WND, P_RATE, P_WND, CREATION_TIME);

                Ti curTime = CREATION_TIME;

                for(int i = 0; i < N_SUBMITS; i++) {

                    curTime += SUBMIT_INTERVAL;

                    peakLB.submit(UNITS);
                    sustLB.submit(UNITS);

                    x.submit(UNITS);

                    const bool LB_RESULT = peakLB.wouldOverflow(1,curTime) ||
                                           sustLB.wouldOverflow(1,curTime);

                    LOOP_ASSERT(LINE,
                                LB_RESULT == x.wouldExceedBandwidth(curTime));

                    const Ti LB_TIMESTAMP = bsl::max(sustLB.lastUpdateTime(),
                                                     peakLB.lastUpdateTime());

                    LOOP_ASSERT(LINE, LB_TIMESTAMP == x.lastUpdateTime());
                }
            }

            if (verbose) cout << endl << "Testing: 'calculateTimeToSubmit'" << endl;

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

                const Uint64 S_CAP = btes_LeakyBucket::calculateCapacity(
                                                                        S_RATE,
                                                                        S_WND);

                const Uint64 P_CAP = btes_LeakyBucket::calculateCapacity(
                                                                        P_RATE,
                                                                        P_WND);

                btes_LeakyBucket peakLB(P_RATE, P_CAP, CREATION_TIME);
                btes_LeakyBucket sustLB(S_RATE, S_CAP, CREATION_TIME);

                Obj x(S_RATE, S_WND, P_RATE, P_WND, CREATION_TIME);

                Ti curTime = CREATION_TIME;

                for(int i = 0; i < N_SUBMITS; i++) {

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
                bsls_AssertFailureHandlerGuard hG(
                                              bsls_AssertTest::failTestDriver);

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
            // ----------------------------------------------------------------
            // 'sustainedRateLimit', 'sustainedRateWindow', 'peakRateLimit',
            // 'peakRateWindow', 'lastUpdateTime',
            // 'statisticsCollectionStartTime'.
            //
            // Concerns:
            //
            //   1 Default CTOR constructs object with default parameters.
            //
            //   2 Value CTOR construst the object with specified parameters.
            //
            //   3 The objects with different parameters can coexist.
            //
            //   4 Specifying wrong CTOR parameters causes certain behavior
            //      In special build configuration.
            //
            //   5 'sustainedRateLimit' returns the value provided to the
            //     'sustainedRateLimit' constructor argument.
            //
            //   6 'sustainedRateWindow' returns the value provided
            //     'sustainedRateWindow' constructor argument.
            //
            //   7 'peakRateLimit' returns the value provided to the
            //     'peakRateLimit' constructor argument.
            //
            //   8 'peakRateWindow' returns the value provided
            //     'peakRateWindow' constructor argument.
            //
            //   9 'lastUpdateTime' returns the value provided by the
            //     'currentTime' constructor argument.
            //
            //   10 'statisticsCollectionStartTime' is set to 0 during construction
            //      by default CTOR.
            //
            //   11 'statisticsCollectionStartTime' is set to the value provided
            //      by the 'currentTime' CTOR argument.
            //
            //   12 The 'sustainedRateWindow' is set to the interval, it
            //      takes to drain the rounded down number of units, that may
            //      be drained at the 'sustainedRateLimit' during the
            //      'sustainedRateWindow', specified for the CTOR.
            //
            //   13 The 'sustainedRateWindow' is set to the interval, it
            //      takes to drain the rounded down number of units, that may
            //      be drained at the 'sustainedRateLimit' during the
            //      'sustainedRateWindow', specified for the CTOR.
            //
            //   14 If the 'sustainedRateWindow', specified for CTOR is less
            //      than the interval, required to drain 1 unit at
            //      'sustainedRateLimit', 'sustainedRateWindow' is set to this
            //      interval.
            //
            // Plan:
            //
            // Testing:
            //   btes_RateLimiter();
            //
            //   btes_RateLimiter(bsls_Types::Uint64       sustainedRateLimit,
            //                    const bdet_TimeInterval& sustainedRateWindow,
            //                    bsls_Types::Uint64       peakRateLimit,
            //                    const bdet_TimeInterval& peakRateWindow,
            //                    const bdet_TimeInterval& currentTime);
            //
            //   bsls_Types::Uint64 peakRateLimit() const;
            //   bsls_Types::Uint64 sustainedRateLimit() const;
            //   bdet_TimeInterval peakRateWindow() const;
            //   bdet_TimeInterval sustainedRateWindow() const;
            //
            //   void getStatistics(bsls_Types::Uint64* submittedUnits,
            //                      bsls_Types::Uint64* unusedUnits) const;
            //
            //   bdet_TimeInterval lastUpdateTime() const;
            //   bdet_TimeInterval statisticsCollectionStartTime() const;
            //-----------------------------------------------------------------

            if (verbose) cout << endl << "BOOTSTRAP 'btes_RateLimiter'" << endl
                                      << "============================" << endl;

            // C-1

            Obj x;

            ASSERT(10     == x.peakRateLimit());
            ASSERT(Ti(1)  == x.peakRateWindow());
            ASSERT(1      == x.sustainedRateLimit());
            ASSERT(Ti(10) == x.sustainedRateWindow());
            ASSERT(Ti(0)  == x.lastUpdateTime());
            ASSERT(Ti(0)  == x.statisticsCollectionStartTime()); // C-10
            ASSERT(0      == x.unitsReserved());

            // C-2

            if (verbose) cout << endl << "Testing value CTOR & accessors" <<endl;
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

//  LINE   S_RATE    S_WND    P_RATE    P_WND    TCREATE  EXP_S_WND EXP_P_WND
//  ---- --------- --------- -------- ---------- -------- --------- ---------

    {L_,      100, Ti(   1),    1000, Ti(0.001), Ti(  0), Ti(   1), Ti(0.001)},
    {L_,       10, Ti( 0.5),     150, Ti(  0.1), Ti(  0), Ti( 0.5), Ti(  0.1)},
    {L_,  BIG_VAL, Ti(1000),  MAX_RT, Ti(   50), Ti(999), Ti(1000), Ti(   50)},
    {L_, MAX_RT/2, Ti( 100),  MAX_RT, Ti(   90), Ti(500), Ti( 100), Ti(   90)},

    // C-12

    {L_,        5, Ti( 0.3),   1000,  Ti(  0.1), Ti(  0), Ti( 0.2), Ti(  0.1)},

    // C-13

    {L_,        1, Ti(  10),     10,  Ti( 1.55), Ti(  0), Ti(  10), Ti(  1.5)},

    // C-14

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

                    // C-5, C-6, C-7, C-8, C-9, C-11

                    LOOP_ASSERT(LINE, 0             == x.unitsReserved());
                    LOOP_ASSERT(LINE, S_RATE        == X.sustainedRateLimit());
                    LOOP_ASSERT(LINE, P_RATE        == X.peakRateLimit());
                    LOOP_ASSERT(LINE, CREATION_TIME == X.lastUpdateTime());

                    LOOP_ASSERT(LINE,
                                CREATION_TIME == X.statisticsCollectionStartTime());

                    Ti delta = EXP_S_WND - X.sustainedRateWindow();
                    LOOP_ASSERT(LINE, 0 == delta.seconds() &&
                                      delta.nanoseconds() >= 0 &&
                                      delta.nanoseconds() <= 1 );

                    delta = EXP_P_WND - X.peakRateWindow();
                    LOOP_ASSERT(LINE, 0 == delta.seconds() &&
                                      delta.nanoseconds() >= 0 &&
                                      delta.nanoseconds() <= 1 );
                }

                // C-3

                if (verbose) cout<<endl<<"Testing Coexistence"<<endl;
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
                    ASSERT(DATA[0].d_creationTime == y.statisticsCollectionStartTime());

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
                    ASSERT(DATA[1].d_creationTime == z.statisticsCollectionStartTime());

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

            // C-4

            if (verbose) cout << endl << "Negative Testing" << endl;
            {
                bsls_AssertFailureHandlerGuard hG(
                                              bsls_AssertTest::failTestDriver);

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

            if (verbose) cout << endl << "BREATHING TEST" << endl
                                      << "==============" << endl;

            Obj x;
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

            ASSERT(0         == x.unitsReserved());

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
