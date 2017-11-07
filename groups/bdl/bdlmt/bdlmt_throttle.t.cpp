// bdlmt_throttle.t.cpp                                               -*-C++-*-

#include <bdlmt_throttle.h>

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
#include <bsls_systemtime.h>

#include <bsl_cstdlib.h>     // 'atoi'
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
// [11] bool requestPermission();
// [11] bool requestPermission(int);
// [11] bool requestPermission(const bsls::TimeInterval&);
// [11] bool requestPermission(int, const bsls::TimeInterval&);
// [10] bool requestPermission();
// [10] bool requestPermission(int);
// [10] bool requestPermission(const bsls::TimeInterval&);
// [10] bool requestPermission(int, const bsls::TimeInterval&);
// [ 3] bool requestPermission(); -- high contention
// [ 2] bool requestPermission(); -- low contention
//
// MACROS
// [ 9] BDLMT_THROTTLE_INIT_ALLOW_NONE
// [ 8] BDLMT_THROTTLE_IF_ALLOW_NONE
// [ 7] BDLMT_THROTTLE_INIT_ALLOW_ALL
// [ 6] BDLMT_THROTTLE_IF_ALLOW_ALL
// [ 5] BDLMT_THROTTLE_IF -- high contention
// [ 5] BDLMT_THROTTLE_IF_MONOTONIC -- high contention
// [ 5] BDLMT_THROTTLE_IF_REALTIME -- high contention
// [ 4] BDLMT_THROTTLE_IF -- low contention
// [ 4] BDLMT_THROTTLE_IF_MONOTONIC -- low contention
// [ 4] BDLMT_THROTTLE_IF_REALTIME -- low contention
// ----------------------------------------------------------------------------
// [-1] SPEED TEST: PERMISSION GRANTED
// [-2] EVENTS DROPPED TEST
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
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

bool isHighContention = false;

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

Int64 get(bsls::AtomicOperations::AtomicTypes::Int64 *x_p)
{
    return bsls::AtomicOperations::getInt64(x_p);
}

inline
bsls::TimeInterval tiClock(bsls::SystemClockType::Enum clockType =
                                            bsls::SystemClockType::e_MONOTONIC)
{
    return bsls::SystemTime::now(clockType);
}

inline
Uint64 nanoClock(bsls::SystemClockType::Enum clockType =
                                            bsls::SystemClockType::e_MONOTONIC)
{
    return tiClock(clockType).totalNanoseconds();
}

void sleep(double timeInSeconds)
{
    ASSERT(minSleep <= timeInSeconds);   // 10 * 1000 microseconds is minimum
                                        // sleep, at least on some platforms.

    ASSERT(timeInSeconds <= 2.0);       // Don't want this test driver to take
                                        // too long.

    bslmt::ThreadUtil::microSleep(static_cast<int>(timeInSeconds * 1e6));
}

void testCase(u::VoidFunc     f,
              bslmt::Barrier *barrier,
              Int64           expElapsed)
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
{
    typedef bsl::vector<Int64>::const_iterator It;

    stream << endl;

    for (It it = v.begin(); v.end() != it; ++it) {
        stream << *it << endl;
    }

    return stream;
}

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
const int         numModes        = 12;
bool              lastDone = false;
bsls::AtomicInt   atomicBarrier(-1);
bslmt::Barrier    barrier(u::numThreads + 1);

Obj throttleDefault   = BDLMT_THROTTLE_INIT(          burst, leakPeriod);
Obj throttleMonotonic = BDLMT_THROTTLE_INIT_MONOTONIC(burst, leakPeriod);
Obj throttleRealtime  = BDLMT_THROTTLE_INIT_REALTIME( burst, leakPeriod);
Obj throttles[numModes];

void threadJob()
{
    const bsls::SystemClockType::Enum clockType =
                                          2 == initMode % 3
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
              case 1:
              case 2: {
                permitted = throttle.requestPermission();
              } break;
              case 3:
              case 4:
              case 5: {
                permitted = throttle.requestPermission(1);
              } break;
              case 6:
              case 7:
              case 8: {
                permitted = throttle.requestPermission(
                                                     1, u::tiClock(clockType));
              } break;
              case 9: {
                BDLMT_THROTTLE_IF(burst, leakPeriod) {
                  permitted = true;
                }
              } break;
              case 10: {
                BDLMT_THROTTLE_IF_MONOTONIC(burst, leakPeriod) {
                  permitted = true;
                }
              } break;
              case 11: {
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

Obj throttleDefault   = BDLMT_THROTTLE_INIT(          burst, leakPeriod);
Obj throttleMonotonic = BDLMT_THROTTLE_INIT_MONOTONIC(burst, leakPeriod);
Obj throttleRealtime  = BDLMT_THROTTLE_INIT_REALTIME( burst, leakPeriod);

void threadJob()
{
    const int                         ti = initMode % 3;
    const bsls::SystemClockType::Enum clockType =
                                          2 == ti
                                          ? bsls::SystemClockType::e_REALTIME
                                          : bsls::SystemClockType::e_MONOTONIC;
    Obj&                              throttle = 0 == ti
                                               ? throttleDefault
                                               : 1 == ti
                                               ? throttleMonotonic
                                               : throttleRealtime;

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
                                                     1, u::tiClock(clockType));
                  } break;
                  case 5: {
                    permitted = throttle.requestPermission(
                                                    10, u::tiClock(clockType));
                  } break;
                  case 6:
                  case 7: {
                    lastDone = true;
                    permitted = throttle.requestPermission(
                                                   100, u::tiClock(clockType));
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
bool              lastDone = false;
bslmt::Barrier    barrier(u::numThreads + 1);

Obj throttleDefault   = BDLMT_THROTTLE_INIT(          burst, leakPeriod);
Obj throttleMonotonic = BDLMT_THROTTLE_INIT_MONOTONIC(burst, leakPeriod);
Obj throttleRealtime  = BDLMT_THROTTLE_INIT_REALTIME( burst, leakPeriod);

void threadJob()
{
    const int                         ti = initMode % 3;
    const bsls::SystemClockType::Enum clockType =
                                          2 == ti
                                          ? bsls::SystemClockType::e_REALTIME
                                          : bsls::SystemClockType::e_MONOTONIC;
    Obj&                              throttle = 0 == ti
                                               ? throttleDefault
                                               : 1 == ti
                                               ? throttleMonotonic
                                               : throttleRealtime;

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
                  case 1:
                  case 2: {
                    permitted = throttle.requestPermission();
                  } break;
                  case 3:
                  case 4:
                  case 5: {
                    permitted = throttle.requestPermission(1);
                  } break;
                  case 6:
                  case 7:
                  case 8: {
                    permitted = throttle.requestPermission(
                                                     1, u::tiClock(clockType));
                  } break;
                  case 9: {
                    BDLMT_THROTTLE_IF(burst, leakPeriod) {
                      permitted = true;
                    }
                  } break;
                  case 10: {
                    BDLMT_THROTTLE_IF_MONOTONIC(burst, leakPeriod) {
                      permitted = true;
                    }
                  } break;
                  case 11: {
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
                permitted = throttle.requestPermission(u::tiClock());
              } break;
              case 5: {
                permitted = throttle.requestPermission(1, u::tiClock());
              } break;
              case 6: {
                permitted = throttle.requestPermission(100, u::tiClock());
              } break;
              case 7: {
                lastDone = true;
                permitted = throttle.requestPermission(k_BILLION,
                                                       u::tiClock());
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
                permitted = throttle.requestPermission(u::tiClock());
              } break;
              case 5: {
                permitted = throttle.requestPermission(1, u::tiClock());
              } break;
              case 6: {
                permitted = throttle.requestPermission(100, u::tiClock());
              } break;
              case 7: {
                lastDone = true;
                permitted = throttle.requestPermission(k_BILLION,
                                                       u::tiClock());
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

                                // ------------
                                // Case 4_and_5
                                // ------------

namespace Case_4_and_5 {

const Int64     leakPeriod      = 10 * u::k_MILLISECOND;
const int       burst           = 10;
const int       totalEvents     = 5 * burst;
const double    sleepPeriod     = 0.00001;
const Uint64    expElapsed      = (totalEvents - burst) * leakPeriod -
                                                                    u::epsilon;
int             initMode;
bsls::AtomicInt eventsSoFar(0);
bsls::AtomicInt eventsMissed(0);
bslmt::Barrier  barrier(u::numThreads + 1);

void threadJob()
{
    barrier.wait();

    switch (initMode + 3 * u::isHighContention) {
      case 0: {
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
      case 1: {
        while (eventsSoFar < totalEvents) {
            u::sleep(sleepPeriod);
            BDLMT_THROTTLE_IF_MONOTONIC(burst, leakPeriod) {
                ++eventsSoFar;
            }
            else {
                ++eventsMissed;
            }
        }
      } break;
      case 2: {
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
      case 3: {
        while (eventsSoFar < totalEvents) {
            BDLMT_THROTTLE_IF(burst, leakPeriod) {
                ++eventsSoFar;
            }
            else {
                ++eventsMissed;
            }
        }
      } break;
      case 4: {
        while (eventsSoFar < totalEvents) {
            BDLMT_THROTTLE_IF_MONOTONIC(burst, leakPeriod) {
                ++eventsSoFar;
            }
            else {
                ++eventsMissed;
            }
        }
      } break;
      case 5: {
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

}  // close namespace Case_4_and_5

                                // ------------
                                // Case 2_and_3
                                // ------------

namespace Case_2_and_3 {

const Uint64    leakPeriod      = 10 * u::k_MILLISECOND;
const int       burstSize       = 10;
const int       totalEvents     = burstSize * 5;
const double    sleepPeriod     = 100e-6;
const Uint64    expElapsed      = (totalEvents - burstSize) * leakPeriod - 100;
int             initMode;
bsls::AtomicInt eventsSoFar(0);
bslmt::Barrier  barrier(u::numThreads + 1);

Obj throttleDefault   = BDLMT_THROTTLE_INIT(          burstSize, leakPeriod);
Obj throttleMonotonic = BDLMT_THROTTLE_INIT_MONOTONIC(burstSize, leakPeriod);
Obj throttleRealtime  = BDLMT_THROTTLE_INIT_REALTIME( burstSize, leakPeriod);

void threadJob()
{
    barrier.wait();

    Obj& throttle = 0 == initMode
                  ? throttleDefault
                  : 1 == initMode
                  ? throttleMonotonic
                  : throttleRealtime;

    if (u::isHighContention) {
        while (eventsSoFar < totalEvents) {
            if (throttle.requestPermission()) {
                ++eventsSoFar;
            }
        }
    }
    else {
        while (eventsSoFar < totalEvents) {
            u::sleep(sleepPeriod);
            if (throttle.requestPermission()) {
                ++eventsSoFar;
            }
        }
    }

    barrier.wait();
};

}  // close namespace Case_2_and_3

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
      case 13: {
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
// In this section we show intended usage of this component.
//
///Example 1:
///- - - - - - - - - - - - - - - - - - - - - - - - -
      } break;
      case 12: {
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
            int kk = jj % 3;
            bsl::memcpy(&TC::throttles[jj],
                        &(  0 == kk
                          ? TC::throttleDefault
                          : 1 == kk
                          ? TC::throttleMonotonic
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
      case 11: {
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

        for (TC::initMode = 0; TC::initMode < 3; ++TC::initMode) {
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
      case 10: {
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

        for (TC::initMode = 0; TC::initMode < 12; ++TC::initMode) {
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
      case 8:
      case 9: {
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
        //:   controlling a 'then' clause which we confirm was never taken,
        //:   and an 'else' clause which we confirm was always taken.
        //: 2 Measure the speed with which events are approved.
        //
        // Testing:
        //   BDLMT_THROTTLE_INIT_ALLOW_NONE
        //   BDLMT_THROTTLE_IF_ALLOW_NONE
        // --------------------------------------------------------------------

        if (verbose && 8 == test) cout <<"BDLMT_THROTTLE_IF_ALLOW_NONE TEST\n"
                                         "=================================\n";
        if (verbose && 9 == test) cout <<
                                       "BDLMT_THROTTLE_INIT_ALLOW_NONE TEST\n"
                                       "===================================\n";

        namespace TC = Case_Allow_None;

        bslmt::ThreadGroup tg(&u::ta);
        tg.addThreads((8 == test ? &TC::threadJobIf
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
      case 6:
      case 7: {
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
        //:   controlling a 'then' clause which we confirm was always taken,
        //:   and an 'else' clause which we confirm was never taken.
        //: 2 Measure the speed with which events are approved.
        //
        // Testing:
        //   BDLMT_THROTTLE_INIT_ALLOW_ALL
        //   BDLMT_THROTTLE_IF_ALLOW_ALL
        // --------------------------------------------------------------------

        if (verbose && 6 == test) cout << "BDLMT_THROTTLE_IF_ALLOW_ALL TEST\n"
                                          "================================\n";
        if (verbose && 7 == test) cout <<
                                        "BDLMT_THROTTLE_INIT_ALLOW_ALL TEST\n"
                                        "==================================\n";

        namespace TC = Case_Allow_All;

        bslmt::ThreadGroup tg(&u::ta);
        tg.addThreads((6 == test ? &TC::threadJobIf
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
      case 5: {
        // --------------------------------------------------------------------
        // MULTITHREADED TEST -- BDLMT_THROTTLE_IF -- HIGH CONTENTION
        //
        // Concers:
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
        //   BDLMT_THROTTLE_IF_MONOTONIC -- high contention
        //   BDLMT_THROTTLE_IF_REALTIME -- high contention
        // --------------------------------------------------------------------

        if (verbose) cout <<
                "MULTITHREADED TEST -- BDLMT_THROTTLE_IF -- HIGH CONTENTION\n"
                "==========================================================\n";

        namespace TC = Case_4_and_5;

        u::isHighContention = true;

        for (TC::initMode = 0; TC::initMode < 3; ++TC::initMode) {
            if (veryVerbose) P(TC::initMode);

            TC::eventsSoFar = 0;
            u::testCase(&TC::threadJob,
                        &TC::barrier,
                        TC::expElapsed);
        }

        if (verbose) P(TC::eventsMissed);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // MULTITHREADED TEST -- BDLMT_THROTTLE_IF -- HIGH CONTENTION
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
        //   BDLMT_THROTTLE_IF -- low contention
        //   BDLMT_THROTTLE_IF_MONOTONIC -- low contention
        //   BDLMT_THROTTLE_IF_REALTIME -- low contention
        // --------------------------------------------------------------------

        if (verbose) cout <<
                "MULTITHREADED TEST -- BDLMT_THROTTLE_IF -- LOW CONTENTION\n"
                "=========================================================\n";

        namespace TC = Case_4_and_5;

        u::isHighContention = false;

        for (TC::initMode = 0; TC::initMode < 3; ++TC::initMode) {
            if (veryVerbose) P(TC::initMode);

            TC::eventsSoFar = 0;
            u::testCase(&TC::threadJob,
                        &TC::barrier,
                        TC::expElapsed);
        }

        if (verbose) P(TC::eventsMissed);
      } break;
      case 3: {
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

        namespace TC = Case_2_and_3;

        u::isHighContention = true;

        for (TC::initMode = 0; TC::initMode < 3; ++TC::initMode) {
            if (veryVerbose) P(TC::initMode);

            TC::eventsSoFar = 0;
            u::testCase(&TC::threadJob,
                        &TC::barrier,
                        TC::expElapsed);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // MULTITHREADED TEST -- LOW CONTENTION
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
        //   bool requestPermission(); -- low contention
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTITHREADED TEST -- LOW CONTENTION\n"
                             "====================================\n";

        namespace TC = Case_2_and_3;

        u::isHighContention = false;

        for (TC::initMode = 0; TC::initMode < 3; ++TC::initMode) {
            if (veryVerbose) P(TC::initMode);

            TC::eventsSoFar = 0;
            u::testCase(&TC::threadJob,
                        &TC::barrier,
                        TC::expElapsed);
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

            if (verbose) cout << leakPeriods[jj] <<
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
            if (verbose) {
                P_(burst);    P(leakPeriod);
                P(u::nanoClock(clockType));    P(u::get(&mX.d_prevLeakTime));
                P_(mX.d_nanosecondsPerActionLeak);
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

        if (verbose) "1 millsecond leak time, burst 10, 20 periods\n";
        for (int mm = 0; mm < 3; ++mm) {
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
                static Obj mX = BDLMT_THROTTLE_INIT_MONOTONIC(burst,
                                                              leakPeriod);
                pMx = &mX;
                clockType = bsls::SystemClockType::e_MONOTONIC;
              } break;
              case 2: {
                static Obj mX = BDLMT_THROTTLE_INIT_REALTIME(burst,
                                                             leakPeriod);
                pMx = &mX;
                clockType = bsls::SystemClockType::e_REALTIME;
              } break;
              default: {
                ASSERTV(mm, 0);
              }
            }
            if (verbose) {
                P_(burst);    P(leakPeriod);
                P(u::nanoClock(clockType));    P(u::get(&pMx->d_prevLeakTime));
                P_(pMx->d_nanosecondsPerActionLeak);
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
        //   bool requestPermission();
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
