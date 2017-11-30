// bdlmt_throttle.t.cpp                                               -*-C++-*-

#include <bdlmt_throttle.h>

#include <bdlsb_fixedmemoutstreambuf.h>

#include <bslim_testutil.h>

#include <bslmt_barrier.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>
#include <bslmt_mutex.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_atomic.h>
#include <bsls_stopwatch.h>
#include <bsls_systemtime.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>      // 'atoi'
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// ----------------------------------------------------------------------------
// MANIPULATORS
// [13] bool requestPermission();
// [13] bool requestPermission(int);
// [13] bool requestPermission(const bsls::TimeInterval&);
// [13] bool requestPermission(int, const bsls::TimeInterval&);
// [12] bool requestPermission(int);
// [12] bool requestPermission(const bsls::TimeInterval&);
// [12] bool requestPermission(int, const bsls::TimeInterval&);
// [11] bool requestPermission();
// [11] bool requestPermission(int);
// [11] bool requestPermission(const bsls::TimeInterval&);
// [11] bool requestPermission(int, const bsls::TimeInterval&);
// [ 4] bool requestPermission(); -- high contention
// [ 3] bool requestPermission(); -- low contention
// [ 2] void initialize(int, Int64, SystemClockType::Enum);
// [ 2] int requestPermissionIfValid(bool *, int);
// [ 2] int requestPermissionIfValid(bool *, int, const TimeInterval&);
//
// ACCESSORS
// [14] int nextPermit(bsls::TimeInterval *, int) const;
// [ 2] bsls::SystemClockType::Enum clockType() const;
// [ 2] int maxSimultaneousActions() const;
// [ 2] Int64 nanosecondsPerAction() const;
//
// MACROS
// [10] BDLMT_THROTTLE_INIT_ALLOW_NONE
// [ 9] BDLMT_THROTTLE_IF_ALLOW_NONE
// [ 8] BDLMT_THROTTLE_INIT_ALLOW_ALL
// [ 7] BDLMT_THROTTLE_IF_ALLOW_ALL
// [ 6] BDLMT_THROTTLE_IF -- high contention
// [ 6] BDLMT_THROTTLE_IF_REALTIME -- high contention
// [ 5] BDLMT_THROTTLE_IF -- low contention
// [ 5] BDLMT_THROTTLE_IF_REALTIME -- low contention
// [ 2] BDLMT_THROTTLE_INIT(int, Int64)
// [ 2] BDLMT_THROTTLE_INIT_REALTIME(int, Int64)
// ----------------------------------------------------------------------------
// [15] USAGE EXAMPLE
// [-1] EVENTS DROPPED TEST
// [ 1] BREATHING TEST
// ----------------------------------------------------------------------------

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

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)

// ============================================================================
//                GLOBAL TYPEDEFS/CONSTANTS/VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlmt::Throttle     Obj;
typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

int                 test;
bool             verbose;
bool         veryVerbose;
bool     veryVeryVerbose;
bool veryVeryVeryVerbose;

namespace {
namespace u {

typedef bsls::AtomicOperations AtomicOps;

enum { k_MICROSECOND = 1000,
       k_MILLISECOND = 1000 * 1000,
       k_SECOND      = 1000 * 1000 * 1000 };

bslma::TestAllocator ta("test", veryVeryVeryVerbose); // test allocator

const Uint64 epsilon = 100;         // 100 nanoseconds

const double maxOver = 0.2;         // 'u::sleep' can take quite a bbit more
                                    // time than was requested.
const double minSleep = 0.00001;

const int numThreads = 40;

Uint64 start;

bslmt::Mutex outputMutex;

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                 HELPER CLASSES AND FUNCTIONS  FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

typedef void (*VoidFunc)();

inline
bsls::TimeInterval clockTi(bsls::SystemClockType::Enum clockType =
                                            bsls::SystemClockType::e_MONOTONIC)
    // Return a 'TimeInterval' indicating the current time, using the clock
    // indicated by the optionally specified 'clockType'.
{
    return bsls::SystemTime::now(clockType);
}

Int64 get(bsls::AtomicOperations::AtomicTypes::Int64 *x_p)
    // Return the specified atomic '*x_p'.
{
    return bsls::AtomicOperations::getInt64(x_p);
}

inline
Uint64 nanoClock(bsls::SystemClockType::Enum clockType =
                                            bsls::SystemClockType::e_MONOTONIC)
    // Return the current time, in nanoseconds, according to the clock
    // indicated by the optionally specified 'clockType'.
{
    return clockTi(clockType).totalNanoseconds();
}

void sleep(double timeInSeconds)
    // Sleep for the specified 'timeInSeconds' seconds, where 10 * 1000
    // microseconds is minimum sleep, at least on some platforms.
{
    ASSERT(minSleep <= timeInSeconds);

    ASSERT(timeInSeconds <= 2.0);       // Don't want this test driver to take
                                        // too long.

    bslmt::ThreadUtil::microSleep(static_cast<int>(timeInSeconds * 1e6));
}

void testCase(u::VoidFunc     f,
              bslmt::Barrier *barrier,
              Int64           expElapsed)
    // Run the specified function 'f' on multiple threads, use the specified
    // '*barrier' to coordinate the threads, join the threads after they've all
    // waited on the barrier twice, and verify that the threads took at least
    // the specified 'expElapsed' time to finish.
{
    bslmt::ThreadGroup tg(&ta);

    tg.addThreads(f, u::numThreads);

    sleep(0.1);    // get everybody waiting on the barrier

    u::start = u::nanoClock();
    barrier->wait();

    barrier->wait();
    const Int64 elapsed = u::nanoClock() - u::start;

    tg.joinAll();

    ASSERTV(elapsed, expElapsed, elapsed - expElapsed, elapsed >= expElapsed);

    if (veryVerbose) { P_(elapsed);    P(elapsed - expElapsed); }
}

}  // close namespace u
}  // close unnamed namespace

bsl::ostream& operator<<(bsl::ostream& stream, const bsl::vector<Int64>& v)
    // Output the specified 'v', one element per line, to the specified
    // 'stream'.
{
    typedef bsl::vector<Int64>::const_iterator It;

    stream << endl;

    for (It it = v.begin(); v.end() != it; ++it) {
        stream << *it << endl;
    }

    return stream;
}

                                  // ----------
                                  // Case_Usage
                                  // ----------

namespace Case_Usage {

///Usage
///-----
// In this section we show intended usage of this component.
//
// Suppose we have an error reporting function 'reportError', that prints an
// error message.  There is a possibility that 'reportError' will be called
// very frequently and spew, so we want to throttle the error messages.  If
// many happen in a short time, we want to see the first 10, but we don't want
// them spewing at a sustained rate of more than one message every five
// seconds.
//
// First, we begin our routine:
//..
    void reportError(bsl::ostream& stream)
        // Report an error to the specified 'stream'.
    {
//..
// Then, we define the maximum number of traces that can happen in a short
// time, provided that there have been no preceding traces for a long time:
//..
        static const int                maxSimultaneousTraces = 10;
//..
// Next, we define the minimum nanoseconds per trace if sustained traces are
// being attempted to print as five seconds.  Note that since this is in
// nanoseconds, a 64-bit value is going to be needed to represent it:
//..
        static const bsls::Types::Int64 nanosecondsPerSustainedTrace =
                            5 * bdlt::TimeUnitRatio::k_NANOSECONDS_PER_SECOND;
//..
// Then, we declare our 'throttle' object and use the 'BDLMT_THROTTLE_INIT'
// macro to initialize it, using the two above constants.  Note that the two
// above constants *MUST* be calculated at compile-time, which means, among
// other things, that they can't contain any floating point sub-expressions:
//..
        static bdlmt::Throttle throttle = BDLMT_THROTTLE_INIT(
                          maxSimultaneousTraces, nanosecondsPerSustainedTrace);
//..
// Next, we call 'requestPermission' at run-time to determine whether we've
// been spewing too much to allow the next trace:
//..
        if (throttle.requestPermission()) {
//..
// Then, we do the error message controlled by the throttle:
//..
            stream << "Help!  I'm being held prisoner in a microprocessor!\n";
        }
    }
//..

}  // close namespace Case_Usage



                                // --------------
                                // Case_Allow_Few
                                // --------------

namespace Case_Allow_Few {

const int         burst           = 10;
const Uint64      leakPeriod      = 100 * 1000 * 1000 / burst;
const Uint64      timeLimit       = leakPeriod * (burst / 2);
const double      shortSleepTime  = 0.0001;
const int         trials          = 5;
bsls::AtomicInt64 start(0);
bsls::AtomicInt64 eventsSoFar;
bsls::AtomicInt64 rejectedSoFar;
int               initMode;
const int         numModes        = 8;
bool              lastDone = false;
bsls::AtomicInt   atomicBarrier(-1);
bslmt::Barrier    barrier(u::numThreads + 1);

Obj throttleDefault   = BDLMT_THROTTLE_INIT(         burst, leakPeriod);
Obj throttleRealtime  = BDLMT_THROTTLE_INIT_REALTIME(burst, leakPeriod);
Obj throttles[numModes];

void threadJob()
    // Request permission many times, expecting most requests to be rejected,
    // and keep track of the acceptance/rejection rate.
    //
    // 'initMode' will be set prior to this function being called, and will
    // drive which of 8 methods will be used to request permission.  Even
    // values of 'initMode' will use the monotonic clock, odd values will use
    // the realtime clock.
{
    const bsls::SystemClockType::Enum clockType =
                                          initMode % 2
                                          ? bsls::SystemClockType::e_REALTIME
                                          : bsls::SystemClockType::e_MONOTONIC;
    Obj& throttle = throttles[initMode];

    for (int ii = 0; ii < trials; ++ii) {
        barrier.wait();
        while (atomicBarrier < 0) {}

        for (Uint64 jj = 0; true; ++jj) {
            const Uint64 time = u::nanoClock() - start;
            if (time > timeLimit) {
                break;
            }

            bool permitted = false;
            switch (initMode) {
              case 0:
              case 1: {
                permitted = throttle.requestPermission();
              } break;
              case 2:
              case 3: {
                permitted = throttle.requestPermission(1);
              } break;
              case 4:
              case 5: {
                permitted = throttle.requestPermission(
                                                     1, u::clockTi(clockType));
              } break;
              case 6: {
                BDLMT_THROTTLE_IF(burst, leakPeriod) {
                  permitted = true;
                }
              } break;
              case 7: {
                lastDone = true;
                BDLMT_THROTTLE_IF_REALTIME(burst, leakPeriod) {
                  permitted = true;
                }
              } break;
              default: {
                BSLS_ASSERT_OPT(0);
              }
            }

            if (permitted) {
                ++eventsSoFar;
            }
            else {
                ++rejectedSoFar;
            }
        }
        barrier.wait();
    }
}

}  // close namespace Case_Allow_Few

                    // ----------------------------------------
                    // Case_Allow_Virtually_All_Multiple_Action
                    // ----------------------------------------

namespace Case_Allow_Virtually_All_Multiple_Action {

const int         burst           = 10 * 1000 * 100;
const Int64       leakPeriod      = 1;
const double      shortSleepTime  = 0.0001;
const int         trials          = 10;
bsls::AtomicInt64 eventsSoFar;
bsls::AtomicInt64 rejectedSoFar;
bsls::AtomicInt   atomicBarrier;
int               initMode;
bslmt::Barrier    barrier(u::numThreads + 1);

Obj throttleDefault   = BDLMT_THROTTLE_INIT(         burst, leakPeriod);
Obj throttleRealtime  = BDLMT_THROTTLE_INIT_REALTIME(burst, leakPeriod);

void threadJob()
    // Run a test where we request permission in multiple ways, sometimes
    // requesting multiple actions, where 'allow all' is NOT specified, but
    // where we expect virtually all of the requests to be permitted.  This
    // function is called twice, with 'initMode == 0' and 'initMode == 1',
    // using the monotonic and realtime clocks, respectively.
{
    Obj&                              throttle = 0 == initMode % 2
                                               ? throttleDefault
                                               : throttleRealtime;
    const bsls::SystemClockType::Enum clockType =
                                           &throttle == &throttleDefault
                                           ? bsls::SystemClockType::e_MONOTONIC
                                           : bsls::SystemClockType::e_REALTIME;

    for (int ii = 0; ii < trials; ++ii) {
        barrier.wait();
        while (atomicBarrier < 0) {}

        int kk = 0;
        while (atomicBarrier < 1) {
            // Repeat the contents of the loop 16 times to minimize testing on
            // 'atomicBarrier'.

            bool lastDone = false;
            for (int jj = 0; jj < 16; ++jj, ++kk) {
                bool permitted = false;
                switch (kk % 8) {
                  case 0: {
                    permitted = throttle.requestPermission(1);
                  } break;
                  case 1: {
                    permitted = throttle.requestPermission(10);
                  } break;
                  case 2:
                  case 3: {
                    permitted = throttle.requestPermission(100);
                  } break;
                  case 4: {
                    permitted = throttle.requestPermission(
                                                     1, u::clockTi(clockType));
                  } break;
                  case 5: {
                    permitted = throttle.requestPermission(
                                                    10, u::clockTi(clockType));
                  } break;
                  case 6:
                  case 7: {
                    lastDone = true;
                    permitted = throttle.requestPermission(
                                                   100, u::clockTi(clockType));
                  } break;
                  default: {
                    BSLS_ASSERT_OPT(0);
                  }
                }

                if (permitted) {
                    ++eventsSoFar;
                }
                else {
                    ++rejectedSoFar;
                }
            }
            ASSERT(lastDone);

        }

        barrier.wait();
    }
}

}  // close namespace Case_Allow_Virtually_All_Multiple_Action


                      // --------------------------------------
                      // Case_Allow_Virtually_All_Single_Action
                      // --------------------------------------

namespace Case_Allow_Virtually_All_Single_Action {

const int         burst           = 2 * 1000 * 1000;
const Uint64      leakPeriod      = 500 * 1000 * 1000 / burst;
const double      shortSleepTime  = 0.0001;
const int         trials          = 10;
bsls::AtomicInt64 eventsSoFar;
bsls::AtomicInt64 rejectedSoFar;
bsls::AtomicInt   atomicBarrier;
int               initMode;
const int         numModes = 8;
bool              lastDone = false;
bslmt::Barrier    barrier(u::numThreads + 1);

Obj throttleDefault   = BDLMT_THROTTLE_INIT(          burst, leakPeriod);
Obj throttleRealtime  = BDLMT_THROTTLE_INIT_REALTIME( burst, leakPeriod);

void threadJob()
    // Run a test where a large number of requests are made, all requests being
    // for a single action, under circumstances where we expect virtually all
    // of the actions to be permitted.  This function is called multiple times
    // with values of 'initMode' spanning the range '[ 1 .. 8 ]', for
    // requestPermission to be called via different interfaces.  Note that
    // since we are only requesting single actions at a time, we also use if
    // '..._IF*' macros.
{
    Obj&                              throttle = 0 == initMode % 2
                                               ? throttleDefault
                                               : throttleRealtime;
    const bsls::SystemClockType::Enum clockType =
                                           &throttle == &throttleDefault
                                           ? bsls::SystemClockType::e_MONOTONIC
                                           : bsls::SystemClockType::e_REALTIME;

    for (int ii = 0; ii < trials; ++ii) {
        barrier.wait();
        while (atomicBarrier < 0) {}

        while (atomicBarrier < 1) {
            // Repeat the contents of the loop 16 times to minimize testing on
            // 'atomicBarrier'.

            for (int jj = 0; jj < 16; ++jj) {
                bool permitted = false;
                switch (initMode) {
                  case 0:
                  case 1: {
                    permitted = throttle.requestPermission();
                  } break;
                  case 2:
                  case 3: {
                    permitted = throttle.requestPermission(1);
                  } break;
                  case 4:
                  case 5: {
                    permitted = throttle.requestPermission(
                                                     1, u::clockTi(clockType));
                  } break;
                  case 6: {
                    BDLMT_THROTTLE_IF(burst, leakPeriod) {
                      permitted = true;
                    }
                  } break;
                  case 7: {
                    lastDone = true;
                    BDLMT_THROTTLE_IF_REALTIME(burst, leakPeriod) {
                      permitted = true;
                    }
                  } break;
                  default: {
                    BSLS_ASSERT_OPT(0);
                  }
                }

                if (permitted) {
                    ++eventsSoFar;
                }
                else {
                    ++rejectedSoFar;

                    u::outputMutex.lock();
                    cout << "Rejected: ";    P_(initMode);    P_(ii);    P(jj);
                    u::outputMutex.unlock();
                }
            }
        }
        barrier.wait();
    }
}

}  // close namespace Case_Allow_Virtually_All_Single_Action

                                // ---------------
                                // Case_Allow_None
                                // ---------------

namespace Case_Allow_None {

bsls::AtomicInt64 eventsSoFar(0);
bsls::AtomicInt   atomicBarrier(-1);

void threadJobIf()
    // Make a large number of requests via '..._IF_ALLOW_NONE' and verify that
    // none of the actions and permitted and all of them are refused.
{
    enum { k_MILLION = 1024 * 1024 };

    while (atomicBarrier < 0) {}

    while (atomicBarrier < 1) {
        int delta = 0;
        for (int ii = 0; ii < k_MILLION; ++ii) {
            BDLMT_THROTTLE_IF_ALLOW_NONE {
                ASSERT(0 && "Action rejected\n");
            }
            else {
                ++delta;
            }
        }
        ASSERTV(delta, k_MILLION == delta);
        eventsSoFar += delta;
    }
}

void threadJobInit()
    // Make a large number of requests of varying numbers of actions on a
    // throttle configured with '..._ALLOW_NONE' and verify that none of the
    // are permitted and all of them are refused.
{
    enum { k_MILLION = 1024 * 1024,
           k_BILLION = 1000 * 1000 * 1000 };
    static bdlmt::Throttle throttle = BDLMT_THROTTLE_INIT_ALLOW_NONE;
    bool lastDone = false;

    while (atomicBarrier < 0) {}

    while (atomicBarrier < 1) {
        int delta = 0;
        for (int ii = 0; ii < k_MILLION; ++ii) {
            bool permitted = false;
            int todo = ii % 8;
            switch (todo) {
              case 0: {
                permitted = throttle.requestPermission();
              } break;
              case 1: {
                permitted = throttle.requestPermission(1);
              } break;
              case 2: {
                permitted = throttle.requestPermission(100);
              } break;
              case 3: {
                permitted = throttle.requestPermission(k_BILLION);
              } break;
              case 4: {
                permitted = throttle.requestPermission(u::clockTi());
              } break;
              case 5: {
                permitted = throttle.requestPermission(1, u::clockTi());
              } break;
              case 6: {
                permitted = throttle.requestPermission(100, u::clockTi());
              } break;
              case 7: {
                lastDone = true;
                permitted = throttle.requestPermission(k_BILLION,
                                                       u::clockTi());
              } break;
              default: {
                BSLS_ASSERT_OPT(0);
              }
            }

            if (permitted) {
                ASSERTV(todo, 0 && "Action accepted\n");
            }
            else {
                ++delta;
            }
        }
        ASSERT(lastDone);
        ASSERTV(delta, k_MILLION == delta);
        eventsSoFar += delta;
    }
}

}  // close namespace Case_Allow_None

                                // --------------
                                // Case_Allow_All
                                // --------------

namespace Case_Allow_All {

bsls::AtomicInt64 eventsSoFar(0);
bsls::AtomicInt   atomicBarrier(-1);

void threadJobIf()
    // Make a large number of requests for permission using '..._IF_ALLOW_ALL'
    // and observe that actions are always allowed.
{
    enum { k_MILLION = 1024 * 1024 };

    while (atomicBarrier < 0) {}

    while (atomicBarrier < 1) {
        int delta = 0;
        for (int ii = 0; ii < k_MILLION; ++ii) {
            BDLMT_THROTTLE_IF_ALLOW_ALL {
                ++delta;
            }
            else {
                ASSERT(0 && "Action rejected\n");
            }
        }
        ASSERTV(delta, k_MILLION == delta);
        eventsSoFar += delta;
    }
}

void threadJobInit()
    // Make a large number of requests for permission for varying numbers of
    // actions via varying overloads of the 'requestPermission' method and
    // observe that actions are always allowed.
{
    enum { k_MILLION = 1024 * 1024,
           k_BILLION = 1000 * 1000 * 1000 };

    static bdlmt::Throttle throttle = BDLMT_THROTTLE_INIT_ALLOW_ALL;

    bool lastDone = false;
    while (atomicBarrier < 0) {}

    while (atomicBarrier < 1) {
        int delta = 0;
        for (int ii = 0; ii < k_MILLION; ++ii) {
            bool permitted = false;
            switch (ii % 8) {
              case 0: {
                permitted = throttle.requestPermission();
              } break;
              case 1: {
                permitted = throttle.requestPermission(1);
              } break;
              case 2: {
                permitted = throttle.requestPermission(100);
              } break;
              case 3: {
                permitted = throttle.requestPermission(k_BILLION);
              } break;
              case 4: {
                permitted = throttle.requestPermission(u::clockTi());
              } break;
              case 5: {
                permitted = throttle.requestPermission(1, u::clockTi());
              } break;
              case 6: {
                permitted = throttle.requestPermission(100, u::clockTi());
              } break;
              case 7: {
                lastDone = true;
                permitted = throttle.requestPermission(k_BILLION,
                                                       u::clockTi());
              } break;
              default: {
                BSLS_ASSERT_OPT(0);
              }
            }

            if (permitted) {
                ++delta;
            }
            else {
                ASSERT(0 && "Action rejected\n");
            }
        }
        ASSERT(lastDone);
        ASSERTV(delta, k_MILLION == delta);
        eventsSoFar += delta;
    }
}

}  // close namespace Case_Allow_All

                              // ----------------
                              // Case_THROTTLE_IF
                              // ----------------

namespace Case_THROTTLE_IF {

const Int64     leakPeriod      = 10 * u::k_MILLISECOND;
const int       burst           = 10;
const int       totalEvents     = 5 * burst;
const double    sleepPeriod     = 0.00001;
const Uint64    expElapsed      = (totalEvents - burst) * leakPeriod -
                                                                    u::epsilon;
enum InitMode { e_MONOTONIC_LOW,
                e_REALTIME_LOW,
                e_MONOTONIC_HIGH,
                e_REALTIME_HIGH } initMode;
bsls::AtomicInt eventsSoFar(0);
bsls::AtomicInt eventsMissed(0);
bslmt::Barrier  barrier(u::numThreads + 1);

void threadJob()
    // Request permission for many actions using the '_IF*' macros, with the
    // type of clock and the level of contention driven by the value of the
    // 'initMode' variable.
{
    barrier.wait();

    switch (initMode) {
      case e_MONOTONIC_LOW: {
        while (eventsSoFar < totalEvents) {
            u::sleep(sleepPeriod);
            BDLMT_THROTTLE_IF(burst, leakPeriod) {
                ++eventsSoFar;
            }
            else {
                ++eventsMissed;
            }
        }
      } break;
      case e_REALTIME_LOW: {
        while (eventsSoFar < totalEvents) {
            u::sleep(sleepPeriod);
            BDLMT_THROTTLE_IF_REALTIME(burst, leakPeriod) {
                ++eventsSoFar;
            }
            else {
                ++eventsMissed;
            }
        }
      } break;
      case e_MONOTONIC_HIGH: {
        while (eventsSoFar < totalEvents) {
            BDLMT_THROTTLE_IF(burst, leakPeriod) {
                ++eventsSoFar;
            }
            else {
                ++eventsMissed;
            }
        }
      } break;
      case e_REALTIME_HIGH: {
        while (eventsSoFar < totalEvents) {
            BDLMT_THROTTLE_IF_REALTIME(burst, leakPeriod) {
                ++eventsSoFar;
            }
            else {
                ++eventsMissed;
            }
        }
      } break;
      default: {
        BSLS_ASSERT_OPT(0);
      }
    }

    barrier.wait();
}

}  // close namespace Case_THROTTLE_IF

                              // ------------------
                              // Case_Throttle_INIT
                              // ------------------

namespace Case_Throttle_INIT {

const Uint64    leakPeriod      = 10 * u::k_MILLISECOND;
const int       burstSize       = 10;
const int       totalEvents     = burstSize * 5;
const double    sleepPeriod     = 100e-6;
const Uint64    expElapsed      = (totalEvents - burstSize) * leakPeriod - 100;
enum InitMode { e_MONOTONIC_LOW,
                e_REALTIME_LOW,
                e_MONOTONIC_HIGH,
                e_REALTIME_HIGH } initMode;
bsls::AtomicInt eventsSoFar(0);
bslmt::Barrier  barrier(u::numThreads + 1);

Obj throttleDefault   = BDLMT_THROTTLE_INIT(          burstSize, leakPeriod);
Obj throttleRealtime  = BDLMT_THROTTLE_INIT_REALTIME( burstSize, leakPeriod);

void threadJob()
    // Request permission for many actions using 'Throttle' objections
    // initialialized with the '..._INIT*' macros, with the type of clock and
    // the level of contention driven by the value of the 'initMode' variable.
{
    barrier.wait();

    Obj& throttle = e_MONOTONIC_LOW == initMode || e_MONOTONIC_HIGH == initMode
                  ? throttleDefault
                  : throttleRealtime;

    switch (initMode) {
      case e_MONOTONIC_HIGH:
      case e_REALTIME_HIGH: {
        while (eventsSoFar < totalEvents) {
            if (throttle.requestPermission()) {
                ++eventsSoFar;
            }
        }
      } break;
      case e_MONOTONIC_LOW:
      case e_REALTIME_LOW: {
        while (eventsSoFar < totalEvents) {
            u::sleep(sleepPeriod);
            if (throttle.requestPermission()) {
                ++eventsSoFar;
            }
        }
      } break;
      default: {
        BSLS_ASSERT(0);
      }
    }

    barrier.wait();
};

}  // close namespace Case_Throttle_INIT

                            // ---------------------------
                            // Case_Minus_1_Events_Dropped
                            // ---------------------------

namespace Case_Minus_1_Events_Dropped {

const Int64     period          = 100 * u::k_MILLISECOND;
const double    sleepTime       = 2.0 * period / u::k_SECOND;
const double    shortSleepTime  = 0.0001;
const int       eventsPerPeriodPerThread = 10;
const int       eventsPerPeriod = eventsPerPeriodPerThread * u::numThreads;
bsls::AtomicInt eventsSoFar(0);
bsls::AtomicInt atomicBarrier(-1);
bslmt::Barrier  barrier(u::numThreads + 1);

void threadJob()
    // Request permission, many times under very high contention, and see if
    // any actions are refused.
{
    barrier.wait();
    while (0 != atomicBarrier) {}

    for (int jj = 0; jj < eventsPerPeriodPerThread; ++jj) {
        BDLMT_THROTTLE_IF(eventsPerPeriod, period) {
            ++eventsSoFar;
        }
    }
}

}  // close namespace Case_Minus_1_Events_Dropped

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    test                = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 15: {
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

    if (verbose) cout << "USAGE EXAMPLE\n"
                         "=============\n";

    using namespace Case_Usage;

// Next, in 'main', we create an output 'bsl::ostream' object that writes to a
// RAM buffer:
//..
    char                        buffer[10 * 1024];
    bdlsb::FixedMemOutStreamBuf streamBuf(buffer, sizeof(buffer));
    bsl::ostream                ostr(&streamBuf);
//..
// Then, we create a stopwatch and start it running:
//..
    bsls::Stopwatch stopwatch;
    stopwatch.start();
//..
// Now, we cycle for seven seconds, calling the above-defined 'reportError'
// every hundredth of a second.  This should result in ten traces being logged
// in the first tenth of a second, and one more trace being logged five seconds
// later, with all other requests for permission from the throttle being
// refused:
//..
    while (stopwatch.accumulatedWallTime() < 7.0) {
        reportError(ostr);
        bslmt::ThreadUtil::microSleep(10 * 1000);
    }
//..
// Finally, we count the number of traces that were logged and verify that the
// number is eleven, as anticipated:
//..
    const bsl::size_t numLines = bsl::count(streamBuf.data(),
                                            streamBuf.data() +
                                                            streamBuf.length(),
                                            '\n');
    ASSERT(11 == numLines);
//..
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING NEXTPERMIT
        //
        // Concerns:
        //: 1 That 'nextPermit' can predict times in the future when permission
        //:   will be granted.
        //:
        //: 2 That 'nextPermit' can predict times in the past when permission
        //:   will be granted.
        //
        // Plan
        //: 1 Do tests calling 'nextPermit' without calling 'requestPermission'
        //:   first.  We do not anticipate clients really doing this, and the
        //:   test requires an intimate understanding of the component, but
        //:   it's good to test.
        //:
        //: 2 Do tests where 'maxSimultaneousActions' have already been
        //:   requested in a single request at a specific time, and request
        //:   varying numbers of actions and observe the result.
        //:
        //: 3 Do tests where half of 'maxSimultaneousActions' have already been
        //:   requested in a single request at a specific time, and observe the
        //:   results, which for low values of actions requested, will be
        //:   *BEFORE* the time of the request.
        //
        // Testing:
        //   int nextPermit(bsls::TimeInterval *, int) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING NEXTPERMIT\n"
                             "==================\n";

        typedef bdlt::TimeUnitRatio TUR;

        if (verbose) cout << "burst size == 1, not pre-consumed\n";
        {
            Obj throttle = BDLMT_THROTTLE_INIT(
                                        1, TUR::k_NANOSECONDS_PER_MILLISECOND);

            const bsls::TimeInterval start(
                -Obj::k_TEN_YEARS_NANOSECONDS / TUR::k_NANOSECONDS_PER_SECOND);

            bsls::TimeInterval ti;
            throttle.nextPermit(&ti, 1);
            const Int64 microSecs = (ti - start).totalMicroseconds();
            ASSERTV(microSecs, 1000 == microSecs);
        }

        if (verbose) cout << "burst size == 1, 1 pre-consumed\n";
        {
            Obj throttle = BDLMT_THROTTLE_INIT(
                                        1, TUR::k_NANOSECONDS_PER_MILLISECOND);

            const bsls::TimeInterval start = u::clockTi();
            ASSERT(throttle.requestPermission(1, start));

            bsls::TimeInterval ti;
            throttle.nextPermit(&ti, 1);
            const Int64 microSecs = (ti - start).totalMicroseconds();
            ASSERTV(microSecs, 1000 == microSecs);
        }

        if (verbose) cout << "burst size == 10, not pre-consumed\n";
        {
            Obj throttle = BDLMT_THROTTLE_INIT(
                                       10, TUR::k_NANOSECONDS_PER_MILLISECOND);

            const bsls::TimeInterval start(
                -Obj::k_TEN_YEARS_NANOSECONDS / TUR::k_NANOSECONDS_PER_SECOND);

            for (int ii = 1; ii <= 10; ++ii) {
                bsls::TimeInterval ti;
                throttle.nextPermit(&ti, ii);
                const Int64 microSecs = (ti - start).totalMicroseconds();
                ASSERTV(microSecs, ii * 1000 == microSecs);
            }
            for (int ii = 10; 1 <= ii; --ii) {
                bsls::TimeInterval ti;
                throttle.nextPermit(&ti, ii);
                const Int64 microSecs = (ti - start).totalMicroseconds();
                ASSERTV(microSecs, ii * 1000 == microSecs);
            }
        }

        if (verbose) cout << "burst size == 10, 10 pre-consumed\n";
        {
            Obj throttle = BDLMT_THROTTLE_INIT(
                                       10, TUR::k_NANOSECONDS_PER_MILLISECOND);

            const bsls::TimeInterval start = u::clockTi();
            ASSERT(throttle.requestPermission(10, start));

            for (int ii = 1; ii <= 10; ++ii) {
                bsls::TimeInterval ti;
                throttle.nextPermit(&ti, ii);
                const Int64 microSecs = (ti - start).totalMicroseconds();
                ASSERTV(microSecs, ii * 1000 == microSecs);
            }
            for (int ii = 10; 1 <= ii; --ii) {
                bsls::TimeInterval ti;
                throttle.nextPermit(&ti, ii);
                const Int64 microSecs = (ti - start).totalMicroseconds();
                ASSERTV(microSecs, ii * 1000 == microSecs);
            }
        }

        if (verbose) cout << "burst size == 10, 5 pre-consumed\n";
        {
            Obj throttle = BDLMT_THROTTLE_INIT(
                                       10, TUR::k_NANOSECONDS_PER_MILLISECOND);

            const bsls::TimeInterval start = u::clockTi();
            ASSERT(throttle.requestPermission(5, start));

            for (int ii = 1; ii <= 10; ++ii) {
                bsls::TimeInterval ti;
                throttle.nextPermit(&ti, ii);
                const Int64 microSecs = (ti - start).totalMicroseconds();
                ASSERTV(ii, microSecs, (-5 + ii) * 1000 == microSecs);
            }
            for (int ii = 10; 1 <= ii; --ii) {
                bsls::TimeInterval ti;
                throttle.nextPermit(&ti, ii);
                const Int64 microSecs = (ti - start).totalMicroseconds();
                ASSERTV(ii, microSecs, (-5 + ii) * 1000 == microSecs);
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // SPEED TEST: FEW ALLOWED
        //
        // Concerns:
        //: 1 Measure how fast the throttle is able to grant permission under
        //:   heavy contention (and *NOT* using 'BDLMT_THROTTLE_INIT_ALLOW_ALL'
        //:   or 'BDLMT_THROTTLE_INIT_ALLOW_NONE').
        //:
        //: 2 That the service grants permission exactly 10 times in the
        //:   period.  The period is 0.3 seconds, but the test only runs 0.1
        //:   seconds per trial, with 0.25 seconds of waiting between trials.
        //
        // Plan:
        //: 1 Have threads do a high-frequency request for permission with
        //:   only 10 events permitted in an hour and run for much less than
        //:   an hour, and observe that exactly 10 events are permitted.
        //
        // Testing:
        //   bool requestPermission();
        //   bool requestPermission(int);
        //   bool requestPermission(const bsls::TimeInterval&);
        //   bool requestPermission(int, const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << "SPEED TEST: FEW ALLOWED\n"
                             "=======================\n";

        namespace TC = Case_Allow_Few;

        double totalEvents   = 0;
        double totalRejected = 0;
        double totalTime     = 0;

        for (int jj = 0; jj < TC::numModes; ++jj) {
            int kk = jj % 2;
            bsl::memcpy(&TC::throttles[jj],
                        &(  0 == kk
                          ? TC::throttleDefault
                          : TC::throttleRealtime),
                        sizeof(bdlmt::Throttle));
        }

        bslmt::ThreadGroup tg(&u::ta);

        for (TC::initMode = 0; TC::initMode < TC::numModes; ++TC::initMode) {
            tg.addThreads(&TC::threadJob, u::numThreads);

            for (int ii = 0; ii < TC::trials; ++ii) {
                TC::eventsSoFar = 0;
                TC::rejectedSoFar = 0;
                TC::atomicBarrier = -1;

                TC::barrier.wait();
                u::sleep(0.001);
                TC::start = u::nanoClock();
                TC::atomicBarrier = 0;

                TC::barrier.wait();
                const double thisTime =
                        1e-9 * static_cast<double>(u::nanoClock() - TC::start);
                totalTime += thisTime;

                ASSERTV(ii, TC::eventsSoFar, 0 == TC::eventsSoFar ||
                                             TC::eventsSoFar == TC::burst ||
                                             (TC::eventsSoFar > TC::burst &&
                       thisTime > 1e-9 * static_cast<double>(TC::leakPeriod)));

                totalEvents   += static_cast<double>(TC::eventsSoFar);
                totalRejected += static_cast<double>(TC::rejectedSoFar);

                u::sleep(0.25);
            }

            tg.joinAll();
        }
        ASSERT(TC::lastDone);

        if (verbose) {
            cout << "Events / sec   = " << (totalEvents   / totalTime) << endl;
            cout << "Rejected / sec = " << (totalRejected / totalTime) << endl;
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // SPEED TEST: PERMISSION GRANTED: MULTIPLE ACTION
        //
        // Concerns:
        //: 1 Measure how fast the throttle is able to grant permission under
        //:   heavy contention (and *NOT* using 'BDLMT_ALLOW_ALL').
        //:
        //: 2 That the service always grants permission and never rejects any
        //:   events.
        //
        // Plan:
        //: 1 Have threads do a high-frequency request for permission with
        //:   everything granted for a short period of time, and observe how
        //:   many events are permitted.
        //
        // Testing:
        //   bool requestPermission(int);
        //   bool requestPermission(const bsls::TimeInterval&);
        //   bool requestPermission(int, const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                           "SPEED TEST: PERMISSION GRANTED: MULTIPLE ACTION\n"
                           "===============================================\n";

        namespace TC = Case_Allow_Virtually_All_Multiple_Action;

        double totalEvents   = 0;
        double totalRejected = 0;
        double totalTime     = 0;

        bslmt::ThreadGroup tg(&u::ta);

        for (TC::initMode = 0; TC::initMode < 2; ++TC::initMode) {
            tg.addThreads(&TC::threadJob, u::numThreads);


            for (int ii = 0; ii < TC::trials; ++ii) {
                TC::eventsSoFar = 0;
                TC::rejectedSoFar = 0;
                TC::atomicBarrier = -1;

                TC::barrier.wait();
                u::sleep(TC::shortSleepTime);

                const Uint64 start = u::nanoClock();
                TC::atomicBarrier = 0;

                u::sleep(0.1);
                TC::atomicBarrier = 1;

                TC::barrier.wait();
                totalTime += 1e-9 *
                                   static_cast<double>(u::nanoClock() - start);

                ASSERTV(TC::initMode, ii, TC::rejectedSoFar, TC::eventsSoFar,
                                      TC::rejectedSoFar * 5 < TC::eventsSoFar);

                totalEvents   += static_cast<double>(TC::eventsSoFar);
                totalRejected += static_cast<double>(TC::rejectedSoFar);
            }

            tg.joinAll();
        }

        ASSERTV(totalEvents, totalRejected, totalEvents / totalRejected > 8);

        if (verbose) {
            cout << "Events / sec   = " << (totalEvents   / totalTime) << endl;
            cout << "Rejected / sec = " << (totalRejected / totalTime) << endl;
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // SPEED TEST: PERMISSION GRANTED: SINGLE ACTION
        //
        // Concerns:
        //: 1 Measure how fast the throttle is able to grant permission under
        //:   heavy contention (and *NOT* using 'BDLMT_ALLOW_ALL').
        //:
        //: 2 That the service always grants permission and never rejects any
        //:   events.
        //
        // Plan:
        //: 1 Have threads do a high-frequency request for permission with
        //:   everything granted for a short period of time, and observe how
        //:   many events are permitted.
        //
        // Testing:
        //   bool requestPermission();
        //   bool requestPermission(int);
        //   bool requestPermission(const bsls::TimeInterval&);
        //   bool requestPermission(int, const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << "SPEED TEST: PERMISSION GRANTED: SINGLE ACTION\n"
                             "=============================================\n";

        namespace TC = Case_Allow_Virtually_All_Single_Action;

        double totalEvents   = 0;
        double totalRejected = 0;
        double totalTime     = 0;

        bslmt::ThreadGroup tg(&u::ta);

        for (TC::initMode = 0; TC::initMode < TC::numModes; ++TC::initMode) {
            tg.addThreads(&TC::threadJob, u::numThreads);


            for (int ii = 0; ii < TC::trials; ++ii) {
                TC::eventsSoFar = 0;
                TC::rejectedSoFar = 0;
                TC::atomicBarrier = -1;

                TC::barrier.wait();
                u::sleep(TC::shortSleepTime);

                const Uint64 start = u::nanoClock();
                TC::atomicBarrier = 0;

                u::sleep(0.1);
                TC::atomicBarrier = 1;

                TC::barrier.wait();
                totalTime += 1e-9 *
                                   static_cast<double>(u::nanoClock() - start);

                ASSERTV(TC::initMode, ii, TC::rejectedSoFar,
                                                       0 == TC::rejectedSoFar);

                totalEvents   += static_cast<double>(TC::eventsSoFar);
                totalRejected += static_cast<double>(TC::rejectedSoFar);
            }

            tg.joinAll();
        }

        ASSERT(TC::lastDone);
        ASSERTV(totalRejected, 0 == totalRejected);

        if (verbose) {
            cout << "Events / sec   = " << (totalEvents   / totalTime) << endl;
            cout << "Rejected / sec = " << (totalRejected / totalTime) << endl;
        }
      } break;
      case 9:
      case 10: {
        // --------------------------------------------------------------------
        // ALLOW_NONE TEST
        //
        // Concerns:
        //: 1 That the 'BDLMT_THROTTLE_IF_ALLOW_NONE' and
        //:   'BDLMT_THROTTLE_INIT_ALLOW_NONE' macros permit no events, and
        //:   appropriately control an 'else' block as well.
        //
        // Plan:
        //: 1 Have 40 threads in a tight loop calling the 'allow no' macro
        //:   controlling a 'then' clause that we confirm was never taken, and
        //:   an 'else' clause that we confirm was always taken.
        //: 2 Measure the speed with which events are approved.
        //
        // Testing:
        //   BDLMT_THROTTLE_INIT_ALLOW_NONE
        //   BDLMT_THROTTLE_IF_ALLOW_NONE
        // --------------------------------------------------------------------

        if (verbose) cout << "ALLOW_NONE\n"
                             "==========\n";

        if (verbose) {
            if (9 == test) cout << "BDLMT_THROTTLE_IF_ALLOW_NONE\n"
                                   "============================\n";
            if (10 == test) cout << "BDLMT_THROTTLE_INIT_ALLOW_NONE\n"
                                    "==============================\n";
        }

        namespace TC = Case_Allow_None;

        bslmt::ThreadGroup tg(&u::ta);
        tg.addThreads((9 == test ? &TC::threadJobIf
                                 : &TC::threadJobInit),
                      u::numThreads);

        u::sleep(0.01);
        const Uint64 start   = u::nanoClock();
        TC::atomicBarrier = 0;
        u::sleep(0.1);
        TC::atomicBarrier = 1;
        const double elapsed         = 1e-9 * static_cast<double>(
                                                       u::nanoClock() - start);
        const double events          = static_cast<double>(TC::eventsSoFar);
        const double eventsPerSecond = events / elapsed / u::numThreads;

        tg.joinAll();

        if (verbose) cout << "Events per sec: " << eventsPerSecond << endl;
        if (verbose) P(TC::eventsSoFar);
      } break;
      case 7:
      case 8: {
        // --------------------------------------------------------------------
        // ALLOW_ALL TEST
        //
        // Concerns:
        //: 1 That the 'BDLMT_THROTTLE_IF_ALLOW_ALL' and
        //:   'BDLMT_THROTTLE_INIT_ALLOW_ALL' macros permit all events, and
        //:   appropriately control an 'else' block as well.
        //
        // Plan:
        //: 1 Have 40 threads in a tight loop calling the 'allow all' macro
        //:   controlling a 'then' clause that we confirm was always taken, and
        //:   an 'else' clause that we confirm was never taken.
        //: 2 Measure the speed with which events are approved.
        //
        // Testing:
        //   BDLMT_THROTTLE_INIT_ALLOW_ALL
        //   BDLMT_THROTTLE_IF_ALLOW_ALL
        // --------------------------------------------------------------------

        if (verbose) cout << "ALLOW_ALL TEST\n"
                             "==============\n";

        if (verbose) {
            if (7 == test) cout << "BDLMT_THROTTLE_IF_ALLOW_ALL\n"
                                   "===========================\n";
            if (8 == test) cout << "BDLMT_THROTTLE_INIT_ALLOW_ALL\n"
                                   "=============================\n";
        }

        namespace TC = Case_Allow_All;

        bslmt::ThreadGroup tg(&u::ta);
        tg.addThreads((7 == test ? &TC::threadJobIf
                                 : &TC::threadJobInit),
                      u::numThreads);

        u::sleep(0.01);
        const Int64 start   = u::nanoClock();
        TC::atomicBarrier = 0;
        u::sleep(0.1);
        TC::atomicBarrier = 1;
        const double elapsed         = 1e-9 * static_cast<double>(
                                                       u::nanoClock() - start);
        const double events          = static_cast<double>(TC::eventsSoFar);
        const double eventsPerSecond = events / elapsed / u::numThreads;

        tg.joinAll();

        if (verbose) cout << "Events per sec: " << eventsPerSecond << endl;
        if (verbose) P(TC::eventsSoFar);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // MULTITHREADED TEST -- BDLMT_THROTTLE_IF -- HIGH CONTENTION
        //
        // Concerns:
        //: 1 That the type under test functions properly when configured to
        //:   allow all events.
        //:
        //: 2 When BDLMT_ALLOW_ALL is specified, 100% of events are permitted
        //:   and NONE are refused.
        //
        // Plan:
        //: 1 Repeat the first test in the breathing test, only in a
        //:   multithreaded context.
        //
        // Testing:
        //   BDLMT_THROTTLE_IF -- high contention
        //   BDLMT_THROTTLE_IF_REALTIME -- high contention
        // --------------------------------------------------------------------

        if (verbose) cout <<
                "MULTITHREADED TEST -- BDLMT_THROTTLE_IF -- HIGH CONTENTION\n"
                "==========================================================\n";

        namespace TC = Case_THROTTLE_IF;

        for (TC::initMode = TC::e_MONOTONIC_HIGH; true;
                                          TC::initMode = TC::e_REALTIME_HIGH) {
            if (veryVerbose) P(TC::initMode);

            TC::eventsSoFar = 0;
            u::testCase(&TC::threadJob,
                        &TC::barrier,
                        TC::expElapsed);

            if (TC::e_REALTIME_HIGH == TC::initMode) {
                break;
            }
        }

        if (verbose) P(TC::eventsMissed);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // MULTITHREADED TEST -- BDLMT_THROTTLE_IF -- LOW CONTENTION
        //
        // Concerns:
        //: 1 That the type under test functions properly under light
        //:   multithreaded contention.
        //
        // Plan:
        //: 1 Repeat the first test in the breathing test, only in a
        //:   multithreaded context.
        //
        // Testing:
        //   BDLMT_THROTTLE_IF -- low contention
        //   BDLMT_THROTTLE_IF_REALTIME -- low contention
        // --------------------------------------------------------------------

        if (verbose) cout <<
                "MULTITHREADED TEST -- BDLMT_THROTTLE_IF -- LOW CONTENTION\n"
                "=========================================================\n";

        namespace TC = Case_THROTTLE_IF;

        for (TC::initMode = TC::e_MONOTONIC_LOW; true;
                                           TC::initMode = TC::e_REALTIME_LOW) {
            if (veryVerbose) P(TC::initMode);

            TC::eventsSoFar = 0;
            u::testCase(&TC::threadJob,
                        &TC::barrier,
                        TC::expElapsed);

            if (TC::e_REALTIME_LOW == TC::initMode) {
                break;
            }
        }

        if (verbose) P(TC::eventsMissed);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // MULTITHREADED TEST -- HIGH CONTENTION
        //
        // Concers:
        //: 1 That the type under test functions properly under light
        //:   multithreaded contention.
        //
        // Plan:
        //: 1 Repeat the first test in the breathing test, only in a
        //:   multithreaded context.
        //
        // Testing:
        //   bool requestPermission(); -- high contention
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTITHREADED TEST -- HIGH CONTENTION\n"
                             "=====================================\n";

        namespace TC = Case_Throttle_INIT;

        for (TC::initMode = TC::e_MONOTONIC_HIGH; true;
                                          TC::initMode = TC::e_REALTIME_HIGH) {
            if (veryVerbose) P(TC::initMode);

            TC::eventsSoFar = 0;
            u::testCase(&TC::threadJob,
                        &TC::barrier,
                        TC::expElapsed);

            if (TC::e_REALTIME_HIGH == TC::initMode) {
                break;
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // MULTITHREADED TEST -- LOW CONTENTION
        //
        // Concerns:
        //: 1 That the type under test functions properly under light
        //:   multithreaded contention.
        //
        // Plan:
        //: 1 Repeat the first test in the breathing test, only in a
        //:   multithreaded context.
        //
        // Testing:
        //   bool requestPermission(); -- low contention
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTITHREADED TEST -- LOW CONTENTION\n"
                             "====================================\n";

        namespace TC = Case_Throttle_INIT;

        for (TC::initMode = TC::e_MONOTONIC_LOW; true;
                                           TC::initMode = TC::e_REALTIME_LOW) {
            if (veryVerbose) P(TC::initMode);

            TC::eventsSoFar = 0;
            u::testCase(&TC::threadJob,
                        &TC::barrier,
                        TC::expElapsed);

            if (TC::e_REALTIME_LOW == TC::initMode) {
                break;
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'initialize', ACCESSORS, AND 'requestPermissionIfValid'
        //
        // Concerns:
        //: 1 That after an object is initialized, either using 'initialize'
        //:   or the '...INIT*' macros, that the accessors verify the state of
        //:   the object is as expected.
        //:
        //: 2 Verify that 'requestPermissionIfValid' fails on various forms of
        //:   invalid input.
        //:
        //: 3 Verify that the state of the object is identical to the state
        //:   of an object created with the '...INIT*' macro passed the same
        //:   args.
        //:
        //
        // Plan:
        //: 1 Initialize a variety of objects using the '...INIT*' macros,
        //:   and set up a table to drive identical initialization using the
        //:   'initialize' function.
        //:
        //: 2 Use 'memcmp' to verify that objects initialized both ways are
        //:   identical.
        //:
        //: 3 Use the 'maxSimultaneousActions' and 'nanosecondsPerAction'
        //:   accessors to verify the state of the object.
        //:
        //: 4 Call 'requestPermissionIfValid' several times with invalid input,
        //:   observed that non-zero values are returned.
        //
        // Testing:
        //   BDLMT_THROTTLE_INIT(int, Int64)
        //   BDLMT_THROTTLE_INIT_REALTIME(int, Int64)
        //   void initialize(int, Int64, SystemClockType::Enum);
        //   bsls::SystemClockType::Enum clockType() const;
        //   int maxSimultaneousActions() const;
        //   Int64 nanosecondsPerAction() const;
        //   int requestPermissionIfValid(bool *, int);
        //   int requestPermissionIfValid(bool *, int, const TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout <<
           "TESTING 'initialize', ACCESSORS, AND 'requestPermissionIfValid'\n"
           "---------------------------------------------------------------\n";

        typedef bdlt::TimeUnitRatio TUR;

        static Obj throttle00 = BDLMT_THROTTLE_INIT(0, 1);
        static Obj throttle01 = BDLMT_THROTTLE_INIT_REALTIME(0, 1);
        static Obj throttle02 = BDLMT_THROTTLE_INIT(1, 0);
        static Obj throttle03 = BDLMT_THROTTLE_INIT_REALTIME(1, 0);
        static Obj throttle04 = BDLMT_THROTTLE_INIT(1, u::k_MILLISECOND);
        static Obj throttle05 = BDLMT_THROTTLE_INIT_REALTIME(
                                                    1, u::k_MILLISECOND);
        static Obj throttle06 = BDLMT_THROTTLE_INIT(5, u::k_MILLISECOND);
        static Obj throttle07 = BDLMT_THROTTLE_INIT_REALTIME(
                                                    5, u::k_MILLISECOND);
        static Obj throttle08 = BDLMT_THROTTLE_INIT(1000, u::k_SECOND);
        static Obj throttle09 = BDLMT_THROTTLE_INIT_REALTIME(
                                                    1000, u::k_SECOND);
        static Obj throttle10 = BDLMT_THROTTLE_INIT(
                                         10, 10 * TUR::k_NANOSECONDS_PER_HOUR);
        static Obj throttle11 = BDLMT_THROTTLE_INIT_REALTIME(
                                         10, 10 * TUR::k_NANOSECONDS_PER_HOUR);

        static const struct Data {
            int              d_line;
            int              d_maxSimultaneousActions;
            Int64            d_nanosecondsPerAction;
            Obj *d_staticThrottle_p;
        } DATA[] = {
            { L_,    0, 1,                                &throttle00 },
            { L_,    0, 1,                                &throttle01 },
            { L_,    1, 0,                                &throttle02 },
            { L_,    1, 0,                                &throttle03 },
            { L_,    1, u::k_MILLISECOND,                 &throttle04 },
            { L_,    1, u::k_MILLISECOND,                 &throttle05 },
            { L_,    5, u::k_MILLISECOND,                 &throttle06 },
            { L_,    5, u::k_MILLISECOND,                 &throttle07 },
            { L_, 1000, u::k_SECOND,                      &throttle08 },
            { L_, 1000, u::k_SECOND,                      &throttle09 },
            { L_,   10, 10 * TUR::k_NANOSECONDS_PER_HOUR, &throttle10 },
            { L_,   10, 10 * TUR::k_NANOSECONDS_PER_HOUR, &throttle11 }
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&  data                   = DATA[ti];
            const int    LINE                   = data.d_line;
            const int    maxSimultaneousActions =
                                                 data.d_maxSimultaneousActions;
            const Int64  nanosecondsPerAction   = data.d_nanosecondsPerAction;
            const Obj   *pStaticThrottle        = data.d_staticThrottle_p;

            const bsls::SystemClockType::Enum clockType = 0 == (ti & 1)
                                           ? bsls::SystemClockType::e_MONOTONIC
                                           : bsls::SystemClockType::e_REALTIME;
            ASSERT(clockType == pStaticThrottle->clockType());

            Obj monoThrottle;
            monoThrottle.initialize(maxSimultaneousActions,
                                    nanosecondsPerAction);
            Obj mX;    const Obj& X = mX;
            mX.initialize(maxSimultaneousActions,
                          nanosecondsPerAction,
                          clockType);

            ASSERTV(LINE, 0 == bsl::memcmp(&mX,
                                           data.d_staticThrottle_p,
                                           sizeof(Obj)));
            if (bsls::SystemClockType::e_MONOTONIC == clockType) {
                ASSERTV(LINE, 0 == bsl::memcmp(&monoThrottle,
                                               &mX,
                                               sizeof(Obj)));
            }

            const int   expMaxSimultaneousActions = 0 == nanosecondsPerAction
                                                  ? INT_MAX
                                                  : maxSimultaneousActions;
            const Int64 expNanosecondsPerAction   = 0 == maxSimultaneousActions
                                                  ? LLONG_MAX
                                                  : 0 == nanosecondsPerAction
                                                  ? LLONG_MIN
                                                  : nanosecondsPerAction;

            ASSERTV(ti, X.clockType(), clockType, X.clockType() == clockType);

            ASSERTV(expMaxSimultaneousActions, maxSimultaneousActions,
                    X.maxSimultaneousActions(),
                    expMaxSimultaneousActions == X.maxSimultaneousActions());

            ASSERTV(expNanosecondsPerAction, nanosecondsPerAction,
                    X.nanosecondsPerAction(),
                    expNanosecondsPerAction == X.nanosecondsPerAction());

            const bsls::TimeInterval now = u::clockTi(clockType);

            bool ret;
            ASSERT(0 != mX.requestPermissionIfValid(&ret, -1));
            ASSERT(0 != mX.requestPermissionIfValid(&ret, -1, now));
            if (0 < maxSimultaneousActions) {
                if (0 < nanosecondsPerAction) {
                    ASSERT(0 != mX.requestPermissionIfValid(
                                                  &ret,
                                                  maxSimultaneousActions + 1));
                    ASSERT(0 != mX.requestPermissionIfValid(
                                                   &ret,
                                                   maxSimultaneousActions + 1,
                                                   now));
                }

                ret = false;
                ASSERT(0 == mX.requestPermissionIfValid(&ret, 1, now));
                ASSERT(ret);
                if (2 <= maxSimultaneousActions) {
                    ret = false;
                    ASSERT(0 == mX.requestPermissionIfValid(
                                                    &ret,
                                                    maxSimultaneousActions - 1,
                                                    now));
                    ASSERT(ret);
                }
                else {
                    ASSERT(1 == maxSimultaneousActions);
                }

                if (0 < nanosecondsPerAction) {
                    for (int ii = 1; ii <= maxSimultaneousActions; ++ii) {
                        ret = true;
                        ASSERT(0 == mX.requestPermissionIfValid(&ret,
                                                                ii,
                                                                now));
                        ASSERT(!ret);
                    }
                }
                else {
                    // allow all

                    for (int ii = 1; ii <= 100 * 100; ii += 50) {
                        ret = false;
                        ASSERT(0 == mX.requestPermissionIfValid(&ret,
                                                                ii,
                                                                now));
                        ASSERT(ret);
                    }
                }
            }
            else {
                // allow none

                for (int ii = 1; ii <= 100 * 100; ii += 50) {
                    ret = true;
                    ASSERT(0 == mX.requestPermissionIfValid(&ret, ii, now));
                    ASSERT(!ret);
                }
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
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        int saveTestStatus = 0;

        for (int kk = 0; kk < 3 * 3; ++kk) {
            int jj = kk / 3;
            int mm = kk % 3;

            int                   leakPeriods[3] = { 100, 10, 1 };
            static const Int64    leakPeriod      = leakPeriods[jj] *
                                                              u::k_MILLISECOND;

            if (veryVerbose) cout << leakPeriods[jj] <<
                                 " milliseconds, 20 action test, burst = 10\n";

            static const unsigned burst           = 10;
            const unsigned        numLeakPeriods  = 20;

            bsls::SystemClockType::Enum clockType;

            Obj mX;
            switch (mm) {
              case 0: {
                mX.initialize(burst,
                              leakPeriod);
                clockType = bsls::SystemClockType::e_MONOTONIC;
              } break;
              case 1: {
                mX.initialize(burst,
                              leakPeriod,
                              bsls::SystemClockType::e_MONOTONIC);
                clockType = bsls::SystemClockType::e_MONOTONIC;
              } break;
              case 2: {
                mX.initialize(burst,
                              leakPeriod,
                              bsls::SystemClockType::e_REALTIME);
                clockType = bsls::SystemClockType::e_REALTIME;
              } break;
              default: {
                ASSERTV(mm, 0);
              }
            }
            if (veryVerbose) {
                P_(burst);    P(leakPeriod);
                P(u::nanoClock(clockType));    P(u::get(&mX.d_prevLeakTime));
                P_(mX.d_nanosecondsPerAction);
                P(mX.d_nanosecondsPerTotalReset);
            }

            bsl::vector<Int64> results(&u::ta);
            results.reserve(numLeakPeriods + 1);

            Int64 startTime = u::nanoClock(clockType);
            for (unsigned ii = 0; ii < numLeakPeriods; ) {
                if (mX.requestPermission()) {
                    if (++ii > burst) {
                        Int64 t = u::nanoClock(clockType);
                        const Int64 diff = (t - startTime) -
                                                     (ii - burst) * leakPeriod;
                        if (diff < 0) {
                            results.push_back(diff);
                        }
                    }
                }
            }

            const Int64 elapsed = u::nanoClock(clockType) - startTime;
            const Int64 expElapsed = leakPeriod * (numLeakPeriods - burst) -
                                                                    u::epsilon;

            ASSERTV(elapsed - expElapsed, results, leakPeriod,
                                                        elapsed >= expElapsed);

            if (veryVerbose) {
                P_(elapsed);    P_(elapsed - expElapsed);    P(results);
            }

            saveTestStatus += testStatus;
            testStatus = 0;
        }

        testStatus = saveTestStatus;

        if (verbose) cout << "1 millsecond leak time, burst 10, 20 periods\n";
        for (int mm = 0; mm < 2; ++mm) {
            static const Int64    leakPeriod      = 1 * u::k_MILLISECOND;
            static const unsigned burst           = 10;
            const unsigned        numLeakPeriods  = 20;

            bsls::SystemClockType::Enum clockType;

            Obj *pMx;
            switch (mm) {
              case 0: {
                static Obj mX = BDLMT_THROTTLE_INIT(burst,
                                                    leakPeriod);
                pMx = &mX;
                clockType = bsls::SystemClockType::e_MONOTONIC;
              } break;
              case 1: {
                static Obj mX = BDLMT_THROTTLE_INIT_REALTIME(burst,
                                                             leakPeriod);
                pMx = &mX;
                clockType = bsls::SystemClockType::e_REALTIME;
              } break;
              default: {
                ASSERTV(mm, 0);
              }
            }
            if (veryVerbose) {
                P_(burst);    P(leakPeriod);
                P(u::nanoClock(clockType));    P(u::get(&pMx->d_prevLeakTime));
                P_(pMx->d_nanosecondsPerAction);
                P(pMx->d_nanosecondsPerTotalReset);
            }

            bsl::vector<Int64> results(&u::ta);
            results.reserve(numLeakPeriods + 1);

            Int64 startTime = u::nanoClock(clockType);
            for (unsigned ii = 0; ii < numLeakPeriods; ) {
                if (pMx->requestPermission()) {
                    if (++ii > burst) {
                        const Int64 t = u::nanoClock(clockType);
                        const Int64 diff = (t - startTime) -
                                                     (ii - burst) * leakPeriod;
                        if (diff < 0) {
                            results.push_back(diff);
                        }
                    }
                }
            }

            const Int64 elapsed = u::nanoClock(clockType) - startTime;
            const Int64 expElapsed = leakPeriod * (numLeakPeriods - burst) -
                                                                    u::epsilon;

            ASSERTV(elapsed - expElapsed, results, leakPeriod,
                                                        elapsed >= expElapsed);

            if (veryVerbose) {
                P_(elapsed);    P_(elapsed - expElapsed);    P(results);
            }

            if (veryVerbose) {
                P_(elapsed);    P_(elapsed - expElapsed); P(results);
            }
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // EVENTS DROPPED TEST
        //
        // Concerns:
        //: 1 That under high contention under circumstances where multiple
        //:   threads are likely to update the clock at the same time, that
        //:   events are not lost.
        //
        // Plan:
        //: 1 Control everything by a double barrier -- first, a
        //:   'bslmt::Barrier' for long waits, then spinning on the atomic
        //:   'atomicBarrier' for short waits.  This guarantees that all
        //:   threads will be released from the second barrier at very close to
        //:   exactly the same time, without wasting too many cycles doing long
        //:   spins on the atomic.
        //:
        //: 2 After being released from the double barrier, all subthreads will
        //:   attempt to get permission for a number of events.  There will be
        //:   enough events allowed in the period for ALL attempted events to
        //:   acquire permission.
        //:
        //: 3 At the end, check the numer of events that were permitted and
        //:   verify that none were refused.
        //
        // Testing:
        //   EVENTS DROPPED TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "EVENTS DROPPED TEST\n"
                             "===================\n";

        namespace TC = Case_Minus_1_Events_Dropped;

        bslmt::ThreadGroup tg(&u::ta);

        tg.addThreads(&TC::threadJob, u::numThreads);

        TC::barrier.wait();
        u::sleep(TC::shortSleepTime);
        ++TC::atomicBarrier;
        ASSERT(TC::atomicBarrier == 0);

        tg.joinAll();

        ASSERTV(TC::eventsPerPeriod, TC::eventsSoFar,
                                       TC::eventsPerPeriod == TC::eventsSoFar);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global or default
    // allocators.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                0 == defaultAllocator.numBlocksTotal());

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
