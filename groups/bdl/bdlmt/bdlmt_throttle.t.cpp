// bdlmt_throttle.t.cpp                                               -*-C++-*-

#include <bdlmt_throttle.h>

#include <bdlsb_fixedmemoutstreambuf.h>

#include <bslim_testutil.h>

#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
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
#include <bsls_bsltestutil.h>

#include <bsl_algorithm.h>
#include <bsl_c_ctype.h>      // 'isdigit'
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>      // 'atoi'
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
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
// [ 6] int requestPermissionIfValid(bool*,int);
// [ 6] int requestPermissionIfValid(bool*,int,const bsls::TimeInterval&);
// [ 5] bool requestPermission();
// [ 5] bool requestPermission(int);
// [ 4] requestPermission(int, const bsls::TimeInterval&);
// [ 4] requestPermission(const bsls::TimeInterval&);
// [ 2] void initialize(int, Int64, SystemClockType::Enum);
//
// ACCESSORS
// [ 7] int nextPermit(bsls::TimeInterval *, int) const;
// [ 2] bsls::SystemClockType::Enum clockType() const;
// [ 2] int maxSimultaneousActions() const;
// [ 2] Int64 nanosecondsPerAction() const;
//
// MACROS
// [ 8] BDLMT_THROTTLE_INIT(int, Int64)
// [ 8] BDLMT_THROTTLE_INIT_REALTIME(int, Int64)
// [ 9] BDLMT_THROTTLE_IF(int, Int64)
// [ 9] BDLMT_THROTTLE_IF_REALTIME(int, Int64)
// [ 9] BDLMT_THROTTLE_IF_ALLOW_ALL
// [ 9] BDLMT_THROTTLE_IF_ALLOW_NONE
// ----------------------------------------------------------------------------
// [16] USAGE EXAMPLE
// [15] CONCERN: BDLMT_THROTTLE_INIT_ALLOW_NONE stress test
// [15] CONCERN: BDLMT_THROTTLE_IF_ALLOW_NONE stress test
// [14] CONCERN: BDLMT_THROTTLE_INIT_ALLOW_ALL stress test
// [14] CONCERN: BDLMT_THROTTLE_IF_ALLOW_ALL stress test
// [13] CONCERN: Multithreaded, high contention, macro if
// [12] CONCERN: Multithreaded, low contention, macro if
// [11] CONCERN: Multithreaded, high contention, macro init
// [10] CONCERN: Multithreaded, low contention, macro init
// [ 3] TEST APPARATUS
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

typedef bdlmt::Throttle         Obj;
typedef bsls::SystemClockType   CT;
typedef CT::Enum                ClockType;
typedef bsls::Types::Int64      Int64;
typedef bsls::Types::Uint64     Uint64;
typedef bsls::TimeInterval      TimeInterval;


int                 test;
bool             verbose;
bool         veryVerbose;
bool     veryVeryVerbose;
bool veryVeryVeryVerbose;

namespace {
namespace u {

typedef bsls::AtomicOperations AtomicOps;

enum { k_MICRO  = 1000,
       k_MILLI  = 1000 * 1000,
       k_SECOND = 1000 * 1000 * 1000 };

bslma::TestAllocator ta("test", veryVeryVeryVerbose); // test allocator

const Uint64 epsilon = 100;         // 100 nanoseconds

const double maxOver = 0.2;         // 'u::sleep' can take quite a bit more
                                    // time than was requested.
const double minSleep = 0.000001;   // 10 microseconds

const int numThreads = 40;

TimeInterval start, end;

bslmt::Mutex outputMutex;

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                 HELPER CLASSES AND FUNCTIONS  FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

typedef void (*VoidFunc)();

const char *b(bool x)
    // Return the specified 'x' in string form.
{
    return x ? "true" : "false";
}

inline
bsls::TimeInterval clockTi(ClockType clockType = CT::e_MONOTONIC)
    // Return a 'TimeInterval' indicating the current time, using the clock
    // indicated by the optionally specified 'clockType'.
{
    return bsls::SystemTime::now(clockType);
}

double doubleClock(ClockType clockType = CT::e_MONOTONIC)
    // Return the time in seconds, as a floating point value, using the
    // specified optionally specified 'clockType'.
{
    return u::clockTi(clockType).totalSecondsAsDouble();
}

Int64 get(bsls::AtomicOperations::AtomicTypes::Int64 *x_p)
    // Return the specified atomic '*x_p'.
{
    return bsls::AtomicOperations::getInt64(x_p);
}

inline
Int64 nanoClock(ClockType clockType = CT::e_MONOTONIC)
    // Return the current time, in nanoseconds, according to the clock
    // indicated by the optionally specified 'clockType'.
{
    return clockTi(clockType).totalNanoseconds();
}

bsls::TimeInterval toTime(const char *);    // forward declaration

double secondsPrevLeakTime(Obj *throttle_p)
{
    const Int64 lag = nanoClock(throttle_p->d_clockType) -
                                              get(&throttle_p->d_prevLeakTime);
    return static_cast<double>(lag) * 1e-9;
}

inline
void sleep(double timeInSeconds)
    // Sleep for the specified 'timeInSeconds' seconds, where 10 * 1000
    // microseconds is minimum sleep, at least on some platforms.
{
    bslmt::ThreadUtil::microSleep(static_cast<int>(timeInSeconds * 1e6));
}

inline
double checkedSleep(double    timeInSeconds,
                    ClockType clockType = CT::e_MONOTONIC)
    // Sleep for at least the specified 'timeInSeconds', using the system clock
    // specified by 'clockType'.  Return 0 if the amount of time slept was at
    // least 'timeInSeconds', and not more than 1.1 times 'timeInSeconds',
    // return a positive value if we slept too long, and a negative value if we
    // slept too short.
{
    const double start = doubleClock(clockType);
    sleep(timeInSeconds);
    const double elapsed = doubleClock(clockType) - start;

    if (elapsed < timeInSeconds || timeInSeconds * 1.1 < elapsed) {
        return elapsed - timeInSeconds;                               // RETURN
    }

    return 0;
}

bsls::TimeInterval toTime(const char *timeStr)
    // Return a time interval converted from the specified 'timeStr' of the
    // form "<minutes>:<seconds>.<frac>:<nano>", where:
    //: o <minutes> and <seconds> can be any number of digits.
    //: o <frac> is the fractional part of a second following the '.'.
    //: o <nano> is nanoseconds and can be any number of digits
    // Note that both ':'s and the '.' are optional.  If no '.' is present, the
    // second ':' may not be present.  If the first ':' is not present, the
    // first field is taken to be seconds.  The behavior is undefined if
    // 'bsl::strlen(timeStr) > 128', if any extra '.'s or ':''s are in the
    // 'timeStr', if 'timeStr' contains any character other than digits, '.',
    // and ':', or if <minutes>, <seconds>, or <nano> are too long to be
    // parsed into an 'int'.
{
    char buf[128 + 1];
    const bsl::size_t len = bsl::strlen(timeStr);
    BSLS_ASSERT(len < sizeof(buf));

    bsl::strcpy(buf, timeStr);

    char *minutesStr = 0;
    char *secondsStr = bsl::strchr(buf, ':');
    char *fracStr    = bsl::strchr(buf, '.');
    char *nanoStr    = bsl::strchr(buf, 'n');
    char *maxStr     = buf;    (void) maxStr;

    if (!secondsStr) {
        secondsStr = buf;
    }
    else {
        *secondsStr = 0;
        ++secondsStr;
        minutesStr = buf;
    }
    BSLS_ASSERT(secondsStr);
    if (fracStr) {
        *fracStr = 0;
        ++fracStr;
    }
    if (nanoStr) {
        *nanoStr = 0;
        ++nanoStr;
    }
    BSLS_ASSERT(secondsStr);

    BSLS_ASSERT(!minutesStr || maxStr == minutesStr);
    BSLS_ASSERT(maxStr <= secondsStr);
    maxStr = secondsStr;
    if (fracStr) {
        BSLS_ASSERT(maxStr <= fracStr);
        maxStr = fracStr;
    }
    if (nanoStr) {
        BSLS_ASSERT(maxStr <= nanoStr);
    }

    for (const char *pc = buf; pc < buf + len; ++pc) {
        BSLS_ASSERT(!*pc || isdigit(*pc));
    }

    bsls::TimeInterval ret;
    if (minutesStr) {
        ret.addMinutes(bsl::atoi(minutesStr));
    }
    if (*secondsStr) {
        ret.addSeconds(bsl::atoi(secondsStr));
    }
    if (fracStr) {
        enum { k_FRAC_DIGITS = 9 };
        char fracBuf[k_FRAC_DIGITS + 1];
        bsl::fill(fracBuf + 0, fracBuf + sizeof(fracBuf), 0);
        bsl::strncpy(fracBuf, fracStr, k_FRAC_DIGITS);
        bsl::fill(fracBuf + bsl::strlen(fracBuf), fracBuf+k_FRAC_DIGITS, '0');
        ret.addNanoseconds(bsl::atoi(fracBuf));
    }
    if (nanoStr) {
        ret.addNanoseconds(bsl::atoi(nanoStr));
    }

    return ret;
}

inline
bsls::TimeInterval tiAdapt(const char *timeStr)
    // This overloaded function enables 'TimeInterval's to be indicated in
    // tables by either the specified 'timeStr', which this function forwards
    // to 'toTime', or by specifying an actual 'TimeInterval', which is
    // specified to the 'orig' argument of the other overload.
{
    return toTime(timeStr);
}

inline
bsls::TimeInterval tiAdapt(const bsls::TimeInterval& orig)
{
    return orig;
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

bsl::ostream& operator<<(bsl::ostream&               stream,
                         bsls::SystemClockType::Enum clockType)
    // Output the specified 'clockType' to the specified 'stream'.
{
    stream << bsls::SystemClockType::toAscii(clockType);

    return stream;
}

#if 0
bsl::ostream& operator<<(bsl::ostream&      stream,
                         bsls::TimeInterval timeInterval)
    // Output the specified 'clockType' to the specified 'stream'.
{
    stream << timeInterval.seconds();;
    char nanoseconds[10];
    bsl::sprintf(nanoseconds, "%09d", timeInterval.nanoseconds());
    char *pc = nanoseconds + 8;
    for (; nanoseconds <= pc && '0' == *pc; --pc) {
        *pc = 0;
    }
    if (nanoseconds <= pc) {
        stream << '.' <<nanoseconds;
    }

    return stream;
}
#endif

                                  // ----------
                                  // Case_Usage
                                  // ----------

namespace Case_Usage {

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Error Reporting
/// - - - - - - - - - - - - -
// Suppose we have an error reporting function 'reportError', that prints an
// error message to a log stream.  There is a possibility that 'reportError'
// will be called very frequently, and that reports of this error will
// overwhelm the other contents of the log, so we want to throttle the number
// of times this error will be reported.  For our application we decide that we
// want to see at most 10 reports of the error at any given time, and that if
// the error is occurring continuously, that we want a maximum sustained rate
// of one error report every five seconds.
//
// First, we declare the signature of our 'reportError' function:
//..
    void reportError(bsl::ostream& stream)
        // Report an error to the specified 'stream'.
    {
//..
// Then, we define the maximum number of traces that can happen at a time to be
// 10:
//..
        static const int maxSimultaneousTraces = 10;
//..
// Next, we define the minimum interval between subsequent reported errors, if
// errors are being continuously reported to be one report every 5 seconds.
// Note that the units are nanoseconds, which must be represented using a 64
// bit integral value:
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
// Now, we call 'requestPermission' at run-time to determine whether to report
// the next error to the log:
//..
        if (throttle.requestPermission()) {
//..
// Finally, we write the message to the log:
//..
            stream << "Help!  I'm being held prisoner in a microprocessor!\n";
        }
    }
//..

}  // close namespace Case_Usage

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
    enum { k_TIGHT_ITERATIONS = 1024 };

    while (atomicBarrier < 0) {}

    while (atomicBarrier < 1) {
        int delta = 0;
        for (int ii = 0; ii < k_TIGHT_ITERATIONS; ++ii) {
            BDLMT_THROTTLE_IF_ALLOW_NONE {
                ASSERT(0 && "Action rejected\n");
            }
            else {
                ++delta;
            }
        }
        ASSERTV(delta, k_TIGHT_ITERATIONS == delta);
        eventsSoFar += delta;
    }
}

void threadJobInit()
    // Make a large number of requests of varying numbers of actions on a
    // throttle configured with '..._ALLOW_NONE' and verify that none of the
    // are permitted and all of them are refused.
{
    enum { k_BILLION          = 1000 * 1000 * 1000,
           k_TIGHT_ITERATIONS = 1024 };

    static bdlmt::Throttle throttle = BDLMT_THROTTLE_INIT_ALLOW_NONE;
    bool lastDone = false;

    while (atomicBarrier < 0) {}

    while (atomicBarrier < 1) {
        int delta = 0;
        for (int ii = 0; ii < k_TIGHT_ITERATIONS; ++ii) {
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
        ASSERTV(delta, k_TIGHT_ITERATIONS == delta);
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
    enum { k_TIGHT_ITERATIONS = 1024 };

    while (atomicBarrier < 0) {}

    while (atomicBarrier < 1) {
        int delta = 0;
        for (int ii = 0; ii < k_TIGHT_ITERATIONS; ++ii) {
            BDLMT_THROTTLE_IF_ALLOW_ALL {
                ++delta;
            }
            else {
                ASSERT(0 && "Action rejected\n");
            }
        }
        ASSERTV(delta, k_TIGHT_ITERATIONS == delta);
        eventsSoFar += delta;
    }
}

void threadJobInit()
    // Make a large number of requests for permission for varying numbers of
    // actions via varying overloads of the 'requestPermission' method and
    // observe that actions are always allowed.
{
    enum { k_BILLION          = 1000 * 1000 * 1000,
           k_TIGHT_ITERATIONS = 1024 };

    static bdlmt::Throttle throttle = BDLMT_THROTTLE_INIT_ALLOW_ALL;

    bool lastDone = false;
    while (atomicBarrier < 0) {}

    while (atomicBarrier < 1) {
        int delta = 0;
        for (int ii = 0; ii < k_TIGHT_ITERATIONS; ++ii) {
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
        ASSERTV(delta, k_TIGHT_ITERATIONS == delta);
        eventsSoFar += delta;
    }
}

}  // close namespace Case_Allow_All

                              // ---------------------------
                              // Case_Throttle_MULTITHREADED
                              // ---------------------------

namespace Case_Throttle_MULTITHREADED {

const Int64     leakPeriod      = 300 * u::k_MILLI;
const double    leakPeriodSecs  = static_cast<double>(leakPeriod) * 1e-9;
const double    balanceFactor   = 1e-9 / leakPeriodSecs;
const int       burstSize       = 10;
const int       periods         = 3;
const double    duration        = leakPeriodSecs *(periods - 0.5);
const double    sleepPeriod     = 100e-6;
const int       expEvents       = burstSize + (periods - 1);
bslmt::Mutex    mutex;
enum TestMode { e_INIT_LOW, e_INIT_HIGH, e_IF_LOW, e_IF_HIGH } testMode;
ClockType       clockType;
bsls::AtomicInt eventsSoFar(0);
bsls::AtomicInt rejectedSoFar(0);
bsls::AtomicInt ii(0);
bslmt::Barrier  barrier(u::numThreads + 1);

Obj throttleMonotonic = BDLMT_THROTTLE_INIT(          burstSize, leakPeriod);
Obj throttleRealtime  = BDLMT_THROTTLE_INIT_REALTIME( burstSize, leakPeriod);

inline
double calculateBalance(Obj *throttle, const bsls::TimeInterval& now)
{
    const Int64 diff = now.totalNanoseconds() -
                                             u::get(&throttle->d_prevLeakTime);
    return static_cast<double>(diff) * balanceFactor;
}

void threadJob()
    // Request permission for many actions using 'Throttle' objections
    // initialialized with the '..._INIT*' macros, with the type of clock
    // driven by 'clockType' and the contention driven by 'contention'.
{
    Obj& throttle = CT::e_MONOTONIC == clockType
                  ? throttleMonotonic
                  : throttleRealtime;
    ASSERTV(throttle.clockType() == clockType);

    barrier.wait();

    bsls::TimeInterval now;
    switch (testMode) {
      case e_INIT_HIGH: {
        while ((now = u::clockTi(clockType)) < u::end) {
            if (veryVerbose && ++ii < 20) {
                const double balance = calculateBalance(&throttle, now);
                bslmt::LockGuard<bslmt::Mutex> guard(&mutex);
                P(balance);
            }
            if (throttle.requestPermission(now)) {
                ++eventsSoFar;
            }
            else {
                ++rejectedSoFar;
            }
        }
      } break;
      case e_INIT_LOW: {
        while ((now = u::clockTi(clockType)) < u::end) {
            u::sleep(sleepPeriod);
            if (veryVerbose && ++ii < 20) {
                const double balance = calculateBalance(&throttle, now);
                bslmt::LockGuard<bslmt::Mutex> guard(&mutex);
                P(balance);
            }
            if (throttle.requestPermission(now)) {
                ++eventsSoFar;
            }
            else {
                ++rejectedSoFar;
            }
        }
      } break;
      case e_IF_HIGH: {
        while ((now = u::clockTi(clockType)) < u::end) {
            if (CT::e_MONOTONIC == clockType) {
                BDLMT_THROTTLE_IF(burstSize, leakPeriod) {
                    ++eventsSoFar;
                }
                else {
                    ++rejectedSoFar;
                }
            }
            else {
                BDLMT_THROTTLE_IF_REALTIME(burstSize, leakPeriod) {
                    ++eventsSoFar;
                }
                else {
                    ++rejectedSoFar;
                }
            }
        }
      } break;
      case e_IF_LOW: {
        while ((now = u::clockTi(clockType)) < u::end) {
            u::sleep(sleepPeriod);
            if (CT::e_MONOTONIC == clockType) {
                BDLMT_THROTTLE_IF(burstSize, leakPeriod) {
                    ++eventsSoFar;
                }
                else {
                    ++rejectedSoFar;
                }
            }
            else {
                BDLMT_THROTTLE_IF_REALTIME(burstSize, leakPeriod) {
                    ++eventsSoFar;
                }
                else {
                    ++rejectedSoFar;
                }
            }
        }
      } break;
      default: {
        BSLS_ASSERT(0);
      }
    }

    barrier.wait();
};

void multiThreadedMain(TestMode mode)
{
    testMode = mode;

    bslmt::ThreadGroup tg(&u::ta);

    int retries = 0;
    int testStatusSum = 0;
    for (int ti = 0; ti < 2; ++ti) {
        clockType      = ti ? CT::e_REALTIME : CT::e_MONOTONIC;
        eventsSoFar    = 0;
        rejectedSoFar  = 0;
        ii             = 0;

        if (verbose) cout << "Pass " << ti << ' ' << clockType << endl;

        tg.addThreads(&threadJob, u::numThreads);
        u::sleep(0.1);    // get everybody waiting on the barrier

        u::end = (u::start = u::clockTi(clockType)) + duration;
        barrier.wait();    // all start
        TimeInterval afterBarrier = u::clockTi(clockType);

        barrier.wait();    // all finish
        const double elapsed = (u::clockTi(clockType) - u::start).
                                                        totalSecondsAsDouble();
        const bool tookTooLong  = 0.4 * leakPeriodSecs < elapsed - duration;
        const bool tooFewEvents = eventsSoFar < expEvents;

        ASSERTV(clockType, expEvents, eventsSoFar, tookTooLong ||
                                                     eventsSoFar <= expEvents);

        if (tookTooLong || tooFewEvents) {
            ++ retries;
            if (7 < retries) {
                ASSERTV(clockType, duration, elapsed, rejectedSoFar,
                                                                 retries <= 7);
                break;
            }

            cout << "Retry necessary:" << clockType <<
                                       (tookTooLong ? " tookTooLong: " : "") <<
                                       (tooFewEvents ? " tooFewEvents: " : "");
            if (tookTooLong) { P_(duration);    P_(elapsed); }
            if (tooFewEvents) { P_(eventsSoFar); cout << " <"; P_(expEvents); }
            P(retries);

            u::sleep((burstSize + 1) * leakPeriodSecs);
            --ti;
        }
        else {
            ASSERTV(clockType, expEvents, eventsSoFar,
                                                     expEvents == eventsSoFar);
        }

        if (veryVerbose || testStatus) {
            const double scheduled = (u::end - u::start).
                                                        totalSecondsAsDouble();
            const double barrier = (afterBarrier - u::start).
                                                        totalSecondsAsDouble();
            P_(elapsed);   P_(scheduled);   P_(barrier);    P(eventsSoFar);
            P_(rejectedSoFar);    P(duration);
        }

        tg.joinAll();

        testStatusSum += testStatus;
        testStatus = 0;
    }

    testStatus = testStatusSum;
}

}  // close namespace Case_Throttle_MULTITHREADED

                            // ---------------------------
                            // Case_Minus_1_Events_Dropped
                            // ---------------------------

namespace Case_Minus_1_Events_Dropped {

const Int64     period          = 100 * u::k_MILLI;
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
      case 16: {
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

        char                        buffer[10 * 1024];
        bdlsb::FixedMemOutStreamBuf streamBuf(buffer, sizeof(buffer));
        bsl::ostream                ostr(&streamBuf);

        bsls::Stopwatch stopwatch;
        stopwatch.start();

        while (stopwatch.accumulatedWallTime() < 7.0) {
            reportError(ostr);
            bslmt::ThreadUtil::microSleep(10 * 1000);
        }

        const bsl::size_t numLines = bsl::count(streamBuf.data(),
                                                streamBuf.data() +
                                                            streamBuf.length(),
                                                '\n');
        ASSERT(11 == numLines);
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // ALLOW_NONE STRESS TEST
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
        //   CONCERN: BDLMT_THROTTLE_INIT_ALLOW_NONE stress test
        //   CONCERN: BDLMT_THROTTLE_IF_ALLOW_NONE stress test
        // --------------------------------------------------------------------

        if (verbose) cout << "ALLOW_NONE STRESS TEST\n"
                             "======================\n";

        if (verbose) cout << "BDLMT_THROTTLE_IF_ALLOW_NONE\n"
                             "============================\n";

        namespace TC = Case_Allow_None;

        bslmt::ThreadGroup tg(&u::ta);
        tg.addThreads(&TC::threadJobIf, u::numThreads);

        {
            u::sleep(0.01);
            const Uint64 start = u::nanoClock();
            TC::atomicBarrier = 0;
            u::sleep(0.1);
            TC::atomicBarrier = 1;
            const double elapsed     = 1e-9 * static_cast<double>(
                                                       u::nanoClock() - start);
            const double events      = static_cast<double>(TC::eventsSoFar);
            const double eventsPerSecond = events / elapsed / u::numThreads;

            tg.joinAll();

            if (verbose) cout << "Events per sec: " << eventsPerSecond << endl;
        }
        if (verbose) P(TC::eventsSoFar);

        if (verbose) cout << "BDLMT_THROTTLE_INIT_ALLOW_NONE\n"
                             "==============================\n";
        TC::eventsSoFar = 0;
        TC::atomicBarrier = -1;

        tg.addThreads(&TC::threadJobInit, u::numThreads);

        u::sleep(0.01);
        const Uint64 start = u::nanoClock();
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
      case 14: {
        // --------------------------------------------------------------------
        // ALLOW_ALL STRESS TEST
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
        //   CONCERN: BDLMT_THROTTLE_INIT_ALLOW_ALL stress test
        //   CONCERN: BDLMT_THROTTLE_IF_ALLOW_ALL stress test
        // --------------------------------------------------------------------

        if (verbose) cout << "ALLOW_ALL STRESS TEST\n"
                             "=====================\n";

        if (verbose) cout << "BDLMT_THROTTLE_IF_ALLOW_ALL\n"
                             "===========================\n";

        namespace TC = Case_Allow_All;

        bslmt::ThreadGroup tg(&u::ta);
        tg.addThreads(&TC::threadJobIf, u::numThreads);

        {
            u::sleep(0.01);
            const Int64 start   = u::nanoClock();
            TC::atomicBarrier = 0;
            u::sleep(0.1);
            TC::atomicBarrier = 1;
            const double elapsed     = 1e-9 * static_cast<double>(
                                                       u::nanoClock() - start);
            const double events      = static_cast<double>(TC::eventsSoFar);
            const double eventsPerSecond = events / elapsed / u::numThreads;

            tg.joinAll();

            if (verbose) cout << "Events per sec: " << eventsPerSecond << endl;
        }

        if (verbose) P(TC::eventsSoFar);

        if (verbose) cout << "BDLMT_THROTTLE_INIT_ALLOW_ALL\n"
                             "=============================\n";
        TC::eventsSoFar = 0;
        TC::atomicBarrier = -1;

        tg.addThreads(&TC::threadJobInit, u::numThreads);

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
      case 13: {
        // --------------------------------------------------------------------
        // MULTITHREADED TEST -- HIGH CONTENTION, IF
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
        //   CONCERN: Multithreaded, high contention, macro if
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTITHREADED TEST -- HIGH CONTENTION, IF\n"
                             "=========================================\n";

        namespace TC = Case_Throttle_MULTITHREADED;

        TC::multiThreadedMain(TC::e_IF_HIGH);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // MULTITHREADED TEST -- LOW CONTENTION, IF
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
        //   CONCERN: Multithreaded, low contention, macro if
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTITHREADED TEST -- LOW CONTENTION, IF\n"
                             "========================================\n";

        namespace TC = Case_Throttle_MULTITHREADED;

        bslmt::ThreadGroup tg(&u::ta);

        TC::multiThreadedMain(TC::e_IF_LOW);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // MULTITHREADED TEST -- HIGH CONTENTION, INIT
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
        //   CONCERN: Multithreaded, high contention, macro init
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTITHREADED TEST -- HIGH CONTENTION, INIT\n"
                             "===========================================\n";

        namespace TC = Case_Throttle_MULTITHREADED;

        TC::multiThreadedMain(TC::e_INIT_HIGH);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // MULTITHREADED TEST -- LOW CONTENTION, INIT
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
        //   CONCERN: Multithreaded, low contention, macro init
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTITHREADED TEST -- LOW CONTENTION, INIT\n"
                             "==========================================\n";

        namespace TC = Case_Throttle_MULTITHREADED;

        TC::multiThreadedMain(TC::e_INIT_LOW);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING BDLMT_THROTTLE_IF MACROS
        //
        // Concerns:
        //: 1 That all the 'BDLMT_THROTTLE_IF*' macros work correctly.
        //
        // Plan:
        //: 1 Black box testing 1: Iterate a number of times, calling a
        //:   'BDLMT_THROTTLE_IF' with counters in both if 'if' clause and the
        //:   'else' clause controlled by the macro, enough times to exhaust
        //:   'maxSimultaneousActions' but not enough to run long enough for
        //:   'nanosecondsPerAction' to expire.  This makes the tallies of
        //:   counters very predictable, and confirm them.
        //:
        //: 2 Black box testing 2: Iterate a number of times calling a
        //:   'BDLMT_THROTTLE_IF' with a fairly short 'nanosecondsPerAction',
        //:   with counters in both clauses, and a sleep for 1/4 of the
        //:   'nanospecondsPerAction', and the outer loop timed to finish after
        //:   '2.5 * nanospecondsPerAction' has expired.  This should result in
        //:   'maxSimultaneousActions + 2' events being approved, and a roughly
        //:   predictable number of events rejected.
        //:
        //: 3: Repeat both black box tests for 'BDLMT_THROTTLE_IF_REALTIME'.
        //:
        //: 4 White box test:
        //:   o Define a macro taking 'maxSimultaneousActions' and
        //:     'nanoSecondsPerAction' arguments, which expands into a code
        //:     block calling 'BDLMT_THROTTLE_IF' 'maxSimultaneousActions'
        //:     times, with separate bools being set in the 'if' and 'else'
        //:     clauses, verifying that the 'if' was always set and the 'else'
        //:     bool never was.
        //:   o In the 'if' clause, take a reference to the static throttle
        //:     instantiated by the macro, and use the accessors to confirm
        //:     that the state is as expected.
        //:   o Call the macro with a variety of inputs.
        //:
        //: 5 Repeat the white box test for 'BDLMT_THROTTLE_IF_REALTIME'.
        //:
        //: 6 Adapt the white box test to 'BDLMT_THROTTLE_ALLOW_ALL'.  Note
        //:   that there is no need for all the code to be in a macro, since
        //:   there are no args to be varied.  This test requires very
        //:   intimate white-box knowledge of the component, because the
        //:   the state of the throttle is set to strange values in that case.
        //:
        //: 6 Adapt the white box test to 'BDLMT_THROTTLE_ALLOW_NONE'.  This
        //:   is very similar to the 'BDLMT_THROTTLE_ALLOW_ALL' case except
        //:   that the throttle defined in the conditional has to be
        //:   accessed from the 'else' clause, since the 'if' clause is never
        //:   executed.
        //
        // Testing:
        //   BDLMT_THROTTLE_IF(int, Int64)
        //   BDLMT_THROTTLE_IF_REALTIME(int, Int64)
        //   BDLMT_THROTTLE_IF_ALLOW_ALL
        //   BDLMT_THROTTLE_IF_ALLOW_NONE
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING BDLMT_THROTTLE_IF MACROS\n"
                             "================================\n";

        if (verbose) cout << "BDLMT_THROTTLE_IF(4, u::k_SECOND)\n";
        {
            int ii, jj, kk;
            for (ii = 0, jj = 0, kk = 0; kk < 10; ++ii) {
                ASSERTV(ii, jj, kk, jj + kk == ii);
                BDLMT_THROTTLE_IF(4, u::k_SECOND) {
                    ASSERTV(ii, jj, kk, 0 == kk);
                    ++jj;
                }
                else {
                    ASSERTV(ii, jj, kk, 4 == jj);
                    ++kk;
                }
            }
            ASSERTV(ii, jj, kk, 14 == ii);
            ASSERTV(ii, jj, kk,  4 == jj);
            ASSERTV(ii, jj, kk, 10 == kk);
        }

        if (verbose) cout << "BDLMT_THROTTLE_IF(4, 80 * u::k_MILLI)\n";
        {
            const TimeInterval start = u::clockTi();
            const TimeInterval end   = start + 0.2;

            int ii, jj, kk;
            for (ii = 0, jj = 0, kk = 0; u::clockTi() < end; ++ii) {
                const double elapsed =
                                 (u::clockTi() - start).totalSecondsAsDouble();

                ASSERTV(ii, jj, kk, jj + kk == ii);
                BDLMT_THROTTLE_IF(4, 80 * u::k_MILLI) {
                    if (veryVerbose) {
                        P_(jj);    P(elapsed);
                    }
                    ++jj;
                }
                else {
                    ASSERT(4 <= jj);
                    if (veryVerbose) {
                        P_(kk);    P(elapsed);
                    }
                    u::checkedSleep(0.02);
                    ++kk;
                }
            }
            ASSERTV(ii, jj, kk, jj + kk == ii);
            ASSERTV(ii, jj, kk, jj <= 4 + 1 + 1);
            ASSERTV(ii, jj, kk, kk < 12);
        }

        if (verbose) cout << "BDLMT_THROTTLE_IF_REALTIME(4, u::k_SECOND)\n";
        {
            int ii, jj, kk;
            for (ii = 0, jj = 0, kk = 0; kk < 10; ++ii) {
                ASSERTV(ii, jj, kk, jj + kk == ii);
                BDLMT_THROTTLE_IF_REALTIME(4, u::k_SECOND) {
                    ASSERTV(ii, jj, kk, 0 == kk);
                    ++jj;
                }
                else {
                    ASSERTV(ii, jj, kk, 4 == jj);
                    ++kk;
                }
            }
            ASSERTV(ii, jj, kk, 14 == ii);
            ASSERTV(ii, jj, kk,  4 == jj);
            ASSERTV(ii, jj, kk, 10 == kk);
        }

        if (verbose) cout <<"BDLMT_THROTTLE_IF_REALTIME(4, 80 * u::k_MILLI)\n";
        {
            const TimeInterval start = u::clockTi(CT::e_REALTIME);
            const TimeInterval end   = start + 0.2;

            int ii, jj, kk;
            for (ii = 0, jj = 0, kk = 0; u::clockTi(CT::e_REALTIME) < end;
                                                                        ++ii) {
                const double elapsed =
                   (u::clockTi(CT::e_REALTIME) - start).totalSecondsAsDouble();

                ASSERTV(ii, jj, kk, jj + kk == ii);
                BDLMT_THROTTLE_IF_REALTIME(4, 80 * u::k_MILLI) {
                    if (veryVerbose) {
                        P_(jj);    P(elapsed);
                    }
                    ++jj;
                }
                else {
                    ASSERT(4 <= jj);
                    if (veryVerbose) {
                        P_(kk);    P(elapsed);
                    }
                    u::checkedSleep(0.02);
                    ++kk;
                }
            }
            ASSERTV(ii, jj, kk, jj + kk == ii);
            ASSERTV(ii, jj, kk, jj <= 4 + 1 + 1);
        }

        if (verbose) cout << "BDLMT_THROTTLE_IF -- WHITE BOX\n";

#undef  WHITE_BOX_TEST_THROTTLE_IF
#define WHITE_BOX_TEST_THROTTLE_IF(msa, npa) {                                \
            for (int ii = 0; ii < (msa); ++ii) {                              \
                BSLMF_ASSERT(0 < msa && 0 < npa);                             \
                bool found = false, elseFound = false;                        \
                BDLMT_THROTTLE_IF(msa, npa) {                                 \
                    Obj& throttle = bdlmt_throttle_iFtHrOtTlE;                \
                    ASSERTV(throttle.maxSimultaneousActions() == msa);        \
                    ASSERTV(throttle.nanosecondsPerAction()   == npa);        \
                    ASSERTV(throttle.clockType()              ==              \
                                                            CT::e_MONOTONIC); \
                    found = true;                                             \
                }                                                             \
                else {                                                        \
                    elseFound = true;                                         \
                }                                                             \
                ASSERT(found);                                                \
                ASSERT(!elseFound);                                           \
            }                                                                 \
        }

        WHITE_BOX_TEST_THROTTLE_IF( 1,           1);
        WHITE_BOX_TEST_THROTTLE_IF( 1, u::k_SECOND);
        WHITE_BOX_TEST_THROTTLE_IF(10, u::k_SECOND);
        WHITE_BOX_TEST_THROTTLE_IF( 1, u::k_MILLI);
        WHITE_BOX_TEST_THROTTLE_IF(10, u::k_MILLI);
        WHITE_BOX_TEST_THROTTLE_IF( 1, 3600LL * u::k_SECOND);
        WHITE_BOX_TEST_THROTTLE_IF(10, 3600LL * u::k_SECOND);
#undef  WHITE_BOX_TEST_THROTTLE_IF

        if (verbose) cout << "BDLMT_THROTTLE_IF_REALTIME -- WHITE BOX\n";

#undef  WHITE_BOX_TEST_THROTTLE_IF_REALTIME
#define WHITE_BOX_TEST_THROTTLE_IF_REALTIME(msa, npa) {                       \
            for (int ii = 0; ii < (msa); ++ii) {                              \
                BSLMF_ASSERT(0 < msa && 0 < npa);                             \
                bool found = false, elseFound = false;                        \
                BDLMT_THROTTLE_IF_REALTIME(msa, npa) {                        \
                    Obj& throttle = bdlmt_throttle_iFtHrOtTlE;                \
                    ASSERTV(throttle.maxSimultaneousActions() == msa);        \
                    ASSERTV(throttle.nanosecondsPerAction()   == npa);        \
                    ASSERTV(throttle.clockType()              ==              \
                                                             CT::e_REALTIME); \
                    found = true;                                             \
                }                                                             \
                else {                                                        \
                    elseFound = true;                                         \
                }                                                             \
                ASSERT(found);                                                \
                ASSERT(!elseFound);                                           \
            }                                                                 \
        }

        WHITE_BOX_TEST_THROTTLE_IF_REALTIME( 1,           1);
        WHITE_BOX_TEST_THROTTLE_IF_REALTIME( 1, u::k_SECOND);
        WHITE_BOX_TEST_THROTTLE_IF_REALTIME(10, u::k_SECOND);
        WHITE_BOX_TEST_THROTTLE_IF_REALTIME( 1, u::k_MILLI);
        WHITE_BOX_TEST_THROTTLE_IF_REALTIME(10, u::k_MILLI);
        WHITE_BOX_TEST_THROTTLE_IF_REALTIME( 1, 3600LL * u::k_SECOND);
        WHITE_BOX_TEST_THROTTLE_IF_REALTIME(10, 3600LL * u::k_SECOND);
#undef  WHITE_BOX_TEST_THROTTLE_IF_REALTIME

        if (verbose) cout << "BDLMT_THROTTLE_IF_ALLOW_ALL -- WHITE BOX\n";
        for (int ii = 0; ii < 100; ++ii) {
            bool found = false, elseFound = false;
            BDLMT_THROTTLE_IF_ALLOW_ALL {
                Obj& throttle = bdlmt_throttle_iFtHrOtTlE;
                ASSERTV(throttle.maxSimultaneousActions(), INT_MAX,
                                 throttle.maxSimultaneousActions() == INT_MAX);
                ASSERTV(throttle.nanosecondsPerAction(), LLONG_MIN,
                                 throttle.nanosecondsPerAction() == LLONG_MIN);
                ASSERTV(throttle.clockType() == CT::e_MONOTONIC);
                found = true;
            }
            else {
                elseFound = true;
            }
            ASSERT(found);
            ASSERT(!elseFound);
        }

        if (verbose) cout << "BDLMT_THROTTLE_IF_ALLOW_NONE -- WHITE BOX\n";
        for (int ii = 0; ii < 100; ++ii) {
            bool found = false, elseFound = false;
            BDLMT_THROTTLE_IF_ALLOW_NONE {
                found = true;
            }
            else {
                Obj& throttle = bdlmt_throttle_iFtHrOtTlE;
                ASSERTV(throttle.maxSimultaneousActions(),
                                       throttle.maxSimultaneousActions() == 0);
                ASSERTV(throttle.nanosecondsPerAction(), LLONG_MAX,
                                 throttle.nanosecondsPerAction() == LLONG_MAX);
                ASSERTV(throttle.clockType() == CT::e_MONOTONIC);
                elseFound = true;
            }
            ASSERT(!found);
            ASSERT(elseFound);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZATION MACROS
        //
        // Concerns:
        //: 1 That the macro initializers create throttles with the same state
        //:   as a throttle with 'initialize' called with the same arguments.
        //
        // Plan:
        //: 1 Statically initialize a set of throttles with different
        //:   arguments, and have each one pointed to by a line in a table that
        //:   contains the same arguments to be passed to 'initialize', after
        //:   which 'memcmp' is called to verify that the throttles have
        //:   identical state.
        //
        // Testing:
        //   BDLMT_THROTTLE_INIT(int, Int64)
        //   BDLMT_THROTTLE_INIT_REALTIME(int, Int64)
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING INITIALIZATION MACROS\n"
                             "=============================\n";

        typedef bdlt::TimeUnitRatio TUR;

        static Obj throttle00 = BDLMT_THROTTLE_INIT(1, u::k_MILLI);
        static Obj throttle01 = BDLMT_THROTTLE_INIT_REALTIME(1, u::k_MILLI);
        static Obj throttle02 = BDLMT_THROTTLE_INIT(5, u::k_MILLI);
        static Obj throttle03 = BDLMT_THROTTLE_INIT_REALTIME(5, u::k_MILLI);
        static Obj throttle04 = BDLMT_THROTTLE_INIT(1000, u::k_SECOND);
        static Obj throttle05 = BDLMT_THROTTLE_INIT_REALTIME(
                                                            1000, u::k_SECOND);
        static Obj throttle16 = BDLMT_THROTTLE_INIT(
                                         10, 10 * TUR::k_NANOSECONDS_PER_HOUR);
        static Obj throttle17 = BDLMT_THROTTLE_INIT_REALTIME(
                                         10, 10 * TUR::k_NANOSECONDS_PER_HOUR);
        static Obj throttle08 = BDLMT_THROTTLE_INIT(100, 2 * u::k_MILLI);
        static Obj throttle09 = BDLMT_THROTTLE_INIT_REALTIME(
                                                          100, 2 * u::k_MILLI);

        static const ClockType mono = CT::e_MONOTONIC;
        static const ClockType real = CT::e_REALTIME;

        static const struct Data {
            int        d_line;
            int        d_maxSimultaneousActions;
            Int64      d_nanosecondsPerAction;
            ClockType  d_clockType;
            Obj       *d_staticThrottle_p;
        } DATA[] = {
            { L_,    1, u::k_MILLI,                       mono, &throttle00 },
            { L_,    1, u::k_MILLI,                       real, &throttle01 },
            { L_,    5, u::k_MILLI,                       mono, &throttle02 },
            { L_,    5, u::k_MILLI,                       real, &throttle03 },
            { L_, 1000, u::k_SECOND,                      mono, &throttle04 },
            { L_, 1000, u::k_SECOND,                      real, &throttle05 },
            { L_,   10, 10 * TUR::k_NANOSECONDS_PER_HOUR, mono, &throttle16 },
            { L_,   10, 10 * TUR::k_NANOSECONDS_PER_HOUR, real, &throttle17 },
            { L_,  100, 2 * u::k_MILLI,                   mono, &throttle08 },
            { L_,  100, 2 * u::k_MILLI,                   real, &throttle09 }};
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data& data                   = DATA[ti];
            const int   LINE                   = data.d_line;
            const int   maxSimultaneousActions = data.d_maxSimultaneousActions;
            const Int64 nanosecondsPerAction   = data.d_nanosecondsPerAction;
            const ClockType clockType          = data.d_clockType;
            const Obj  *pStaticThrottle        = data.d_staticThrottle_p;

            ASSERT(clockType == pStaticThrottle->clockType());

            Obj mX;
            mX.initialize(maxSimultaneousActions,
                          nanosecondsPerAction,
                          clockType);

            ASSERTV(LINE, 0 == bsl::memcmp(&mX,
                                           data.d_staticThrottle_p,
                                           sizeof(Obj)));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'nextPermit'
        //
        // Concerns:
        //: 1 If 'nextPermit' is called with invalid input, it will return a
        //:   non-zero value without modifying the time interval passed to it.
        //:
        //: 2 If 'nextPermit' is called with valid input, it will return 0, and
        //:   set the time interval passed by pointer to it to the exact
        //:   earliest nanosecond when the specified 'numActions' would be
        //:   permitted.
        //
        // Plan:
        //: 1 Iterate through a table executing 4 commands:
        //:   o 'e_CMD_INIT': Initialize the throttle, but don't call any
        //:     manipulators that will effect its 'd_prevLeakTime' field.  This
        //:     will only be called for setting the throttle to 'allow all' or
        //:     'allow none'.
        //:   o 'e_CMD_INIT_SET_TIME': Initialize the throttle, and call
        //:     'requestPermissions' to set the 'd_prevLeakTime' field to
        //:     exactly the time specified.
        //:   o 'e_CMD_NEXT_PERMIT': Call 'nextPermit', expect it to succeed
        //:     and return 0 and set the time interval passed to the exact
        //:     nanosecond when 'numActions' actions would be permitted.  Then
        //:     follow up with 'requestPermission' to verify that, then
        //:     re-initialize the throttle to the exact state it was in before
        //:     'nextPermit' was called.
        //:   o 'e_CMD_NEXT_PERMIT_INVALID': Call 'nextPermit', expect it to
        //:     return a non-zero value without modifying the time interval
        //:     that was passed to it.
        //
        // Testing:
        //   int nextPermit(bsls::TimeInterval *, int) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'nextPermit'\n"
                             "====================\n";

        enum { k_BILLION = 1000 * 1000 * 1000 };

        enum Cmd { e_CMD_INIT,
                   e_CMD_INIT_SET_TIME,
                   e_CMD_NEXT_PERMIT,
                   e_CMD_NEXT_PERMIT_INVALID };

        const TimeInterval nullTI(-123456789, -123456789);
            // We assign the time interval to be returned from 'nextPermit' to
            // this ridiculous value before the call.  This value is never
            // expected from a valid call, while with invalid calls we confirm
            // that the time interval is unmodified.

        const TimeInterval minusTenYears(
                                  - Obj::k_TEN_YEARS_NANOSECONDS / k_BILLION,
                                  -(Obj::k_TEN_YEARS_NANOSECONDS % k_BILLION));
            // White box -- this is the time interval that will be returned by
            // 'nextPermit' if the throttle was initialized 'allow all'.

        // 'INIT_ALLOW_ALL': initialize the throttle to 'allow all'.

#undef  INIT_ALLOW_ALL
#define INIT_ALLOW_ALL                                                        \
    { L_, e_CMD_INIT, 1, 0, -1, TimeInterval() }

        // 'INIT_ALLOW_NONE': initialize the throttle to 'allow none'.

#undef  INIT_ALLOW_NONE
#define INIT_ALLOW_NONE                                                       \
    { L_, e_CMD_INIT, 0, 1, -1, TimeInterval() }

        // 'INIT_SET_TIME': initialize the throttle with the specified
        // 'maxSimultaneousActions' and 'secondsPerAction', and use
        // 'requestPermission' to set the 'd_prevLeakTime' to the specified
        // string 'setTimeSpec', which is to be parsed by 'u::toTime'.

#undef  INIT_SET_TIME
#define INIT_SET_TIME(maxSimultaneousActions, secondsPerAction, setTimeSpec)  \
    { L_, e_CMD_INIT_SET_TIME, maxSimultaneousActions, secondsPerAction,      \
                                                   -1, u::toTime(setTimeSpec) }

        // 'NEXT_PERMIT': call 'nextPermit' with the specified 'numActions' and
        // verify that it succeeds.  Compare the time returned to the specified
        // 'expectedTimeSpec' and verify that it matches.  If the throttle was
        // not initialized to 'allow all', call 'requestPermission' after the
        // 'nextPermit' call to verify that the time returned was the exact
        // nanosecond when permission would be granted, then re-initialize the
        // throttle back to the state it was in before the 'nextPermit' call.
        // Note that 'expectedTimeSpec' can be either a string to be parsed by
        // 'u::toTime' or a 'TimeInterval'.

#undef  NEXT_PERMIT
#define NEXT_PERMIT(numActions, expectedTimeSpec)                             \
    { L_, e_CMD_NEXT_PERMIT, -1, -1, numActions, u::tiAdapt(expectedTimeSpec) }

        // 'NEXT_PERMIT': call 'nextPermit' with the specified 'numActions' and
        // verify that it fails, and that the 'TimeInterval' passed to the call
        // was not modified.

#undef  NEXT_PERMIT_INVALID
#define NEXT_PERMIT_INVALID(numActions)                                       \
    { L_, e_CMD_NEXT_PERMIT_INVALID, -1, -1, numActions, TimeInterval() }

        const struct Data {
            int           d_line;
            Cmd           d_command;
            int           d_maxSimultaneousActions;
            double        d_secondsPerAction;
            int           d_numActions;
            TimeInterval  d_timeInterval;
        } DATA[] = {
            INIT_SET_TIME(4, 2, ":0"),
            NEXT_PERMIT(1, ":2"),
            NEXT_PERMIT(2, ":4"),
            NEXT_PERMIT(3, ":6"),
            NEXT_PERMIT(4, ":8"),
            NEXT_PERMIT_INVALID(5),
            NEXT_PERMIT_INVALID(0),
            NEXT_PERMIT_INVALID(-1),
            NEXT_PERMIT_INVALID(INT_MIN),
            NEXT_PERMIT_INVALID(INT_MAX),
            NEXT_PERMIT(4, ":8"),

            INIT_SET_TIME(1, 10, ":5"),
            NEXT_PERMIT(1, ":15"),
            NEXT_PERMIT_INVALID(2),
            NEXT_PERMIT_INVALID(0),
            NEXT_PERMIT_INVALID(-1),
            NEXT_PERMIT_INVALID(INT_MIN),
            NEXT_PERMIT_INVALID(INT_MAX),
            NEXT_PERMIT(1, ":15"),

            INIT_SET_TIME(10, 1, ":30"),
            NEXT_PERMIT(1, ":31"),
            NEXT_PERMIT(2, ":32"),
            NEXT_PERMIT(3, ":33"),
            NEXT_PERMIT(4, ":34"),
            NEXT_PERMIT(5, ":35"),
            NEXT_PERMIT(6, ":36"),
            NEXT_PERMIT(7, ":37"),
            NEXT_PERMIT(8, ":38"),
            NEXT_PERMIT(9, ":39"),
            NEXT_PERMIT(10, ":40"),
            NEXT_PERMIT_INVALID(11),
            NEXT_PERMIT_INVALID(0),
            NEXT_PERMIT_INVALID(-1),
            NEXT_PERMIT_INVALID(INT_MIN),
            NEXT_PERMIT_INVALID(INT_MAX),
            NEXT_PERMIT(5, ":35"),

            INIT_SET_TIME(8, (1.0 / 8), "37:27"),
            NEXT_PERMIT(1, "37:27.125"),
            NEXT_PERMIT(2, "37:27.25"),
            NEXT_PERMIT(3, "37:27.375"),
            NEXT_PERMIT(4, "37:27.5"),
            NEXT_PERMIT(5, "37:27.625"),
            NEXT_PERMIT(6, "37:27.75"),
            NEXT_PERMIT(7, "37:27.875"),
            NEXT_PERMIT(8, "37:28"),
            NEXT_PERMIT_INVALID(9),
            NEXT_PERMIT_INVALID(0),
            NEXT_PERMIT_INVALID(-1),
            NEXT_PERMIT_INVALID(INT_MIN),
            NEXT_PERMIT_INVALID(INT_MAX),
            NEXT_PERMIT(8, "37:28"),

            INIT_ALLOW_ALL,
            NEXT_PERMIT(1, minusTenYears),
            NEXT_PERMIT(2, minusTenYears),
            NEXT_PERMIT(100, minusTenYears),
            NEXT_PERMIT(1000, minusTenYears),
            NEXT_PERMIT(1000000, minusTenYears),
            NEXT_PERMIT(INT_MAX, minusTenYears),
            NEXT_PERMIT_INVALID(0),
            NEXT_PERMIT_INVALID(-1),
            NEXT_PERMIT_INVALID(INT_MIN),
            NEXT_PERMIT(200, minusTenYears),

            INIT_ALLOW_NONE,
            NEXT_PERMIT_INVALID(1),
            NEXT_PERMIT_INVALID(INT_MIN),
            NEXT_PERMIT_INVALID(-1),
            NEXT_PERMIT_INVALID(0),
            NEXT_PERMIT_INVALID(INT_MAX) };
#undef  INIT_ALLOW_ALL
#undef  INIT_ALLOW_NONE
#undef  INIT_SET_TIME
#undef  NEXT_PERMIT
#undef  NEXT_PERMIT_INVALID

        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        Obj mX;

        // We declare the following variables associated with initialization
        // outside the loop so that they can be re-used to re-initialize
        // objects in the 'e_CMD_NEXT_PERMIT == cmd' case.

        int          maxSimultaneousActions;
        double       secondsPerAction;
        TimeInterval initTimeInterval;

        bool         allowAll;      // 'e_CMD_NEXT_PERMIT == cmd' needs to know
                                    // if, on the last initialization, the
                                    // throttle was set to 'allow all'.

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&         data         = DATA[ti];
            const int           LINE         = data.d_line;
            const Cmd           cmd          = data.d_command;
            const TimeInterval& timeInterval = data.d_timeInterval;

            switch (cmd) {
              case e_CMD_INIT:
              case e_CMD_INIT_SET_TIME: {
                maxSimultaneousActions = data.d_maxSimultaneousActions;
                secondsPerAction       = data.d_secondsPerAction;
                initTimeInterval       = timeInterval;
                allowAll               = 1 == maxSimultaneousActions &&
                                                         0 == secondsPerAction;

                mX.initialize(maxSimultaneousActions,
                              static_cast<Int64>(secondsPerAction *1e9));

                if (e_CMD_INIT_SET_TIME == cmd) {
                    // Call 'requestPermission' to set the throttle's
                    // 'd_prevLeakTime' field to 'initTimeInterval'.

                    ASSERTV(LINE, mX.requestPermission(maxSimultaneousActions,
                                                       initTimeInterval));
                    ASSERTV(LINE, !mX.requestPermission(initTimeInterval));
                }
              } break;
              case e_CMD_NEXT_PERMIT: {
                const int numActions = data.d_numActions;

                TimeInterval result = nullTI;
                const int rc = mX.nextPermit(&result, numActions);
                ASSERTV(LINE, numActions, 0 == rc);
                ASSERTV(LINE, numActions, timeInterval, result,
                                                       timeInterval == result);

                if (!allowAll) {
                    // Use 'requestPermission' to verify that 'result' was the
                    // EXACT nanosecond when 'numActions' would first have been
                    // permitted.

                    result.addNanoseconds(-1);
                    ASSERTV(LINE, !mX.requestPermission(numActions, result));
                    result.addNanoseconds(1);
                    ASSERTV(LINE, mX.requestPermission(numActions, result));
                    ASSERTV(LINE, !mX.requestPermission(result));

                    // Re-initialize the throttle to the state it was in before
                    // 'nextPermit' was called.

                    mX.initialize(maxSimultaneousActions,
                                  static_cast<Int64>(secondsPerAction *1e9));
                    ASSERTV(LINE, mX.requestPermission(maxSimultaneousActions,
                                                       initTimeInterval));
                    ASSERTV(LINE, !mX.requestPermission(initTimeInterval));
                }
              } break;
              case e_CMD_NEXT_PERMIT_INVALID: {
                const int numActions = data.d_numActions;

                TimeInterval result = nullTI;
                const int rc = mX.nextPermit(&result, numActions);
                ASSERTV(LINE, numActions, 0 != rc);
                ASSERTV(LINE, numActions, nullTI, result, nullTI == result);
              } break;
              default: {
                ASSERTV(LINE, cmd, 0 && "unrecognized cmd");
              }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'requestPermissionIfValid' -- WHITE BOX
        //
        // Conerns:
        //: 1 That 'requestPermissionIfValid' returns non-zero when fed invalid
        //:   input, without modifying the value of the specified '*result'.
        //:
        //: 2 That 'requestPermissionIfValid' returns 0 when fed valid input.
        //
        // Plan:
        //: 1 Do a table-driven test to feed various invalid inputs to
        //:   'requestPermissionIfValid'.
        //:
        //: 2 Loop twice, once where '*result' is pre-set to 'false', once with
        //:   it pre-set to 'true', and observe that neither time is it
        //:   modified unless the inputs were valid.
        //:
        //: 3 Have the table call the function a few time with valid inputs,
        //:   and observe in those cases that 0 is returned a '*result' has the
        //:   expected value.
        //:
        //: 4 Note that we won't be extensively testing for correctness of the
        //:   '*result' returned when the function is fed valid input, since we
        //:   have already tested 'requestPermission'.
        //
        // Testing:
        //   int requestPermissionIfValid(bool*,int);
        //   int requestPermissionIfValid(bool*,int,const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                           "TESTING 'requestPermissionIfValid' -- WHITE BOX\n"
                           "===============================================\n";

        enum Cmd        { e_CMD_INIT, e_CMD_REQUEST };
        enum ExpOutcome { e_EXP_INVALID, e_EXP_TRUE, e_EXP_FALSE };

        const char *expOutcomeStr[] = { "exp_invalid",
                                        "exp_true",
                                        "exp_false" };

        const Int64 int64Max   = bsl::numeric_limits<Int64>::max();
        const Int64 secondsMax = int64Max / u::k_SECOND;
        const Int64 int64Min   = bsl::numeric_limits<Int64>::min();
        const Int64 secondsMin = int64Min / u::k_SECOND;

        const TimeInterval nullTI = u::toTime("1234:56.789");
            // When 'd_timeInterval' is set to this value, it indicates that
            // the time is not to be passed to 'requestPermissionIfValid' and
            // that it is to call the system time.

        // 'INIT': Initialize the throttle with the specified
        // 'maxSimultaneousActions' andb 'secondsPerAction'.

#undef  INIT
#define INIT(maxSimultaneousActions, secondsPerAction)                        \
    { L_, e_CMD_INIT, (maxSimultaneousActions),                               \
          static_cast<Int64>((secondsPerAction) * 1e9), -1, nullTI,           \
                                                                e_EXP_INVALID }

        // 'REQUEST_INVALID' -- call 'requestPermissionIfValid' with the
        // specified 'numActions'.  If 'timeSpec' does not evaluate to the same
        // time interval as 'nullTI', pass the time interval to the function is
        // well.  The function under test is expected to return a non-zero
        // value, without modifying '*result'.

#undef  REQUEST_INVALID
#define REQUEST_INVALID(numActions, timeSpec)                                 \
    { L_, e_CMD_REQUEST, -1, -1, (numActions), u::tiAdapt(timeSpec),          \
                                                                e_EXP_INVALID }

        // 'REQUEST_INVALID' -- call 'requestPermissionIfValid' with the
        // specified 'numActions'.  If 'timeSpec' does not evaluate to the same
        // time interval as 'nullTI', pass the time interval to the function is
        // well.  The function under test is expected to return 0 with
        // '*result' set to 'expResult'.

#undef  REQUEST_OK
#define REQUEST_OK(numActions, timeSpec, expResult)                           \
    { L_, e_CMD_REQUEST, -1, -1, (numActions), u::tiAdapt(timeSpec),          \
                                       (expResult ? e_EXP_TRUE : e_EXP_FALSE) }

        const struct Data {
            int           d_line;
            Cmd           d_command;
            int           d_maxSimultaneousActions;
            Int64         d_nanosecondsPerAction;
            int           d_numActions;
            TimeInterval  d_timeInterval;
            ExpOutcome    d_expOutcome;
        } DATA[] = {
            INIT(4, 0.010),
            REQUEST_INVALID(-1, ":0"),
            REQUEST_INVALID(0,  ":0"),
            REQUEST_INVALID(5,  ":0"),
            REQUEST_INVALID(-1, nullTI),
            REQUEST_INVALID(0,  nullTI),
            REQUEST_INVALID(5,  nullTI),

            REQUEST_INVALID(1, TimeInterval(int64Min, 0)),
            REQUEST_INVALID(2, TimeInterval(int64Min, 0)),
            REQUEST_INVALID(3, TimeInterval(int64Min, 0)),
            REQUEST_INVALID(4, TimeInterval(int64Min, 0)),

            REQUEST_INVALID(1, TimeInterval(secondsMin, -999999999)),
            REQUEST_INVALID(2, TimeInterval(secondsMin, -999999999)),
            REQUEST_INVALID(3, TimeInterval(secondsMin, -999999999)),
            REQUEST_INVALID(4, TimeInterval(secondsMin, -999999999)),

            REQUEST_OK(1, TimeInterval(secondsMin, 0), false),
            REQUEST_OK(2, TimeInterval(secondsMin, 0), false),
            REQUEST_OK(3, TimeInterval(secondsMin, 0), false),
            REQUEST_OK(4, TimeInterval(secondsMin, 0), false),

            REQUEST_OK(1, ":0", true),
            REQUEST_OK(2, ":0", true),
            REQUEST_OK(3, ":0", false),
            REQUEST_OK(4, ":0", false),
            REQUEST_OK(1, ":0", true),
            REQUEST_OK(1, nullTI, true),
            REQUEST_OK(2, nullTI, true),
            REQUEST_OK(3, nullTI, false),
            REQUEST_OK(4, nullTI, false),
            REQUEST_OK(1, nullTI, true),

            REQUEST_OK(1, TimeInterval(secondsMax, 0), true),
            REQUEST_OK(2, TimeInterval(secondsMax, 0), true),
            REQUEST_OK(3, TimeInterval(secondsMax, 0), false),
            REQUEST_OK(4, TimeInterval(secondsMax, 0), false),
            REQUEST_OK(1, TimeInterval(secondsMax, 0), true),

            REQUEST_INVALID(1, TimeInterval(secondsMax, 999999999)),
            REQUEST_INVALID(2, TimeInterval(secondsMax, 999999999)),
            REQUEST_INVALID(3, TimeInterval(secondsMax, 999999999)),
            REQUEST_INVALID(4, TimeInterval(secondsMax, 999999999)),

            REQUEST_INVALID(1, TimeInterval(int64Max, 0)),
            REQUEST_INVALID(2, TimeInterval(int64Max, 0)),
            REQUEST_INVALID(3, TimeInterval(int64Max, 0)),
            REQUEST_INVALID(4, TimeInterval(int64Max, 0)),

            INIT(1, 0.009),

            REQUEST_INVALID(-1, ":0"),
            REQUEST_INVALID(0,  ":0"),
            REQUEST_INVALID(2,  ":0"),
            REQUEST_INVALID(-1, nullTI),
            REQUEST_INVALID(0,  nullTI),
            REQUEST_INVALID(2,  nullTI),

            REQUEST_OK(1, ":0", true),
            REQUEST_OK(1, ":0.010", true),
            REQUEST_OK(1, ":0.015", false),
            REQUEST_OK(1, ":0.020", true),
            REQUEST_OK(1, ":0.020", false),
            REQUEST_OK(1, ":0.023", false),
            REQUEST_OK(1, ":0.023", false),
            REQUEST_OK(1, ":0.027", false),
            REQUEST_OK(1, ":0.030", true),

            // If we don't pass the time, it will pass the current time, which
            // will be >45 years after the epoch.

            REQUEST_OK(1, nullTI, true),
            REQUEST_OK(1, nullTI, false),
            REQUEST_OK(1, nullTI, false) };
#undef  INIT
#undef  REQUEST_INVALID
#undef  REQUEST_OK

        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        Obj mX;
        for (int ti = 0; ti < 2; ++ti) {
            // This outer 'ti' loop iterates through two values, 'false' and
            // 'true', to which the returned value 'ret' is set before calling
            // 'requestPermissionIfValid'.  If the parameters were not valid,
            // we verify that 'ret' is unchanged by the call.

            for (int tj = 0; tj < k_NUM_DATA; ++tj) {
                const Data& data = DATA[tj];
                const int   LINE = data.d_line;
                const Cmd   cmd  = data.d_command;

                switch (cmd) {
                  case e_CMD_INIT: {
                    const int   maxSimultaneousActions =
                                                 data.d_maxSimultaneousActions;
                    const Int64 nanosecondsPerAction =
                                                 data.d_nanosecondsPerAction;

                    if (veryVerbose) cout << "mX.initialize(" <<
                                    maxSimultaneousActions << ", " <<
                                                nanosecondsPerAction << ");\n";

                    mX.initialize(maxSimultaneousActions,
                                  nanosecondsPerAction,
                                  CT::e_REALTIME);
                  } break;
                  case e_CMD_REQUEST: {
                    const int           numActions   = data.d_numActions;
                    const TimeInterval& timeInterval = data.d_timeInterval;
                    const ExpOutcome    expOutcome   = data.d_expOutcome;

                    const bool passTheTime = nullTI != timeInterval;
                    const bool expRet      = e_EXP_INVALID == expOutcome
                                           ? ti
                                           : e_EXP_TRUE == expOutcome;

                    bool ret = ti;
                    int rc = passTheTime
                           ? mX.requestPermissionIfValid(&ret,
                                                         numActions,
                                                         timeInterval)
                           : mX.requestPermissionIfValid(&ret,
                                                         numActions);
                    ASSERTV(LINE, u::b(ti), expOutcomeStr[expOutcome], rc,
                                   (0 == rc) == (e_EXP_INVALID != expOutcome));
                    ASSERTV(LINE, u::b(ti), u::b(expRet), u::b(ret),
                                                                expRet == ret);

                    if (veryVerbose) {
                        cout << "request(" << numActions;
                        if (passTheTime) {
                            cout << ", TI(" << timeInterval.seconds() <<
                                     ", " << timeInterval.nanoseconds() << ")";
                        }
                        cout << ")    ";
                        P_(rc);    P(u::b(ret));
                    }
                  } break;
                  default: {
                    ASSERTV(cmd, 0 && "invalid cmd");
                  }
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'requestPermission' USING SYSTEM TIME
        //
        // Concerns:
        //: 1 That 'requestPermission' works when not passed a
        //:   'bsls::TimeInterval' object, in which case it calls the system
        //:   time.
        //
        // Plan:
        //: 1 Do a table-driven test, manipulating elapsed time by calling
        //:   'u::sleep'.
        //
        // Testing:
        //   bool requestPermission();
        //   bool requestPermission(int);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'requestPermission' USING SYSTEM TIME\n"
                             "=============================================\n";

        enum Cmd { e_CMD_INIT, e_CMD_SLEEP, e_CMD_REQUEST };

#undef  INIT
#define INIT(maxSimultaneousActions, secondsPerAction)                        \
    L_, e_CMD_INIT, (maxSimultaneousActions),                                 \
                          static_cast<Int64>((secondsPerAction) * 1e9), 0, 0, 0

#undef  MILLI_SLEEP
#define MILLI_SLEEP(timeInMillisecs)                                          \
    L_, e_CMD_SLEEP, -1, -1, -1, (timeInMillisecs)/1000.0, 0

#undef  REQUEST
#define REQUEST(numActions, expected)                                         \
    L_, e_CMD_REQUEST, -1, -1, (numActions), 0, (expected)

        static const struct Data {
            int         d_line;
            Cmd         d_command;
            int         d_maxSimultaneousActions;
            Int64       d_nanosecondsPerAction;
            int         d_numActions;
            double      d_sleepSeconds;
            bool        d_expPermission;
        } DATA[] = {
            INIT(1, 80e-3),
            REQUEST(1, 1),
            REQUEST(1, 0),
            REQUEST(1, 0),
            MILLI_SLEEP(40),
            REQUEST(1, 0),
            MILLI_SLEEP(80),
            REQUEST(1, 1),
            REQUEST(1, 0),

            INIT(4, 80e-3),
            REQUEST(2, 1),
            REQUEST(1, 1),
            REQUEST(1, 1),
            REQUEST(1, 0),
            REQUEST(4, 0),
            MILLI_SLEEP(40),
            REQUEST(1, 0),
            MILLI_SLEEP(80),
            REQUEST(1, 1),
            REQUEST(1, 0),
            MILLI_SLEEP(160),
            REQUEST(2, 1),
            REQUEST(4, 0),
            REQUEST(3, 0),
            REQUEST(2, 0),
            REQUEST(1, 0),

            INIT(4, 80e-3),
            REQUEST(1, 1),
            REQUEST(3, 1),
            REQUEST(1, 0),
            MILLI_SLEEP(280),
            REQUEST(1, 1),
            REQUEST(4, 0),
            MILLI_SLEEP(160),
            REQUEST(4, 1),
            REQUEST(4, 0),
            REQUEST(3, 0),
            REQUEST(2, 0),
            REQUEST(1, 0),
            MILLI_SLEEP(200),
            REQUEST(4, 0),
            REQUEST(3, 0),
            REQUEST(2, 1),
            REQUEST(1, 0) };
#undef  INIT
#undef  MILLI_SLEEP
#undef  REQUEST
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        static const struct DataTk {
            int       d_tkLine;
            bool      d_defaultToMono;
            bool      d_singleAction;
            bool      d_clockRealTime;
        } DATA_TK[] = {
            { L_, 0, 0, 0 },
            { L_, 0, 0, 1 },
            { L_, 0, 1, 0 },
            { L_, 0, 1, 1 },
            { L_, 1, 0, 0 },
            { L_, 1, 1, 0 } };
        enum { k_NUM_DATA_TK = sizeof DATA_TK / sizeof *DATA_TK };

        bool quit = false;
        Obj mX;    const Obj& X = mX;

        if (verbose) cout << "Defaulting to calling system time\n";

        for (int tk = 0; !quit && tk < k_NUM_DATA_TK; ++tk) {
            const DataTk    dataTk             = DATA_TK[tk];
            const int       TK_LINE            = dataTk.d_tkLine;
            const bool      DEFAULT_TO_MONO    = dataTk.d_defaultToMono;
            const bool      SINGLE_ACTION_ONLY = dataTk.d_singleAction;
            const ClockType CLOCK_TYPE         = dataTk.d_clockRealTime
                                               ? CT::e_REALTIME
                                               : CT::e_MONOTONIC;

            ASSERTV(TK_LINE, !DEFAULT_TO_MONO ||
                                                CT::e_MONOTONIC == CLOCK_TYPE);

            if (veryVerbose) {
                cout << endl;
                P_(TK_LINE);  P_(DEFAULT_TO_MONO); P_(SINGLE_ACTION_ONLY);
                P(CLOCK_TYPE);
            }

            Int64  nanosecondsPerAction;
            double secondsPerAction;
            double start = 0, scheduledTime = 0, actualTime = 0, overshoot = 0;
            int retries = 0;
            for (int ti = 0; !quit && ti< k_NUM_DATA; ++ti) {
                const Data  data = DATA[ti];
                const int   LINE = data.d_line;
                const Cmd   CMD  = data.d_command;

                switch (CMD) {
                  case e_CMD_INIT: {
                    const int MAX_SIMULTANEOUS_ACTIONS =
                                                 data.d_maxSimultaneousActions;

                    nanosecondsPerAction = data.d_nanosecondsPerAction;
                    secondsPerAction = 1e-9 *
                                     static_cast<double>(nanosecondsPerAction);

                    if (DEFAULT_TO_MONO) {
                        mX.initialize(MAX_SIMULTANEOUS_ACTIONS,
                                      nanosecondsPerAction);
                    }
                    else {
                        mX.initialize(MAX_SIMULTANEOUS_ACTIONS,
                                      nanosecondsPerAction,
                                      CLOCK_TYPE);
                    }

                    ASSERT(X.clockType() == CLOCK_TYPE);

                    if (veryVerbose) {
                        cout << "initialize(" << MAX_SIMULTANEOUS_ACTIONS <<
                                                      ", " << secondsPerAction;
                        if (!DEFAULT_TO_MONO) {
                            cout << ", " << CLOCK_TYPE;
                        }
                        cout << ");\n";
                    }

                    start         = u::doubleClock(CLOCK_TYPE);
                    scheduledTime = 0;
                    actualTime    = 0;
                  } break;
                  case e_CMD_SLEEP: {
                    const double SLEEP_SECONDS = data.d_sleepSeconds;

                    if (veryVerbose) {
                        cout << "sleep(" << SLEEP_SECONDS;
                        if (!DEFAULT_TO_MONO) {
                            cout << ", " << CLOCK_TYPE;
                        }
                        cout << ");\n";
                    }

                    const double timeToSleep = SLEEP_SECONDS - overshoot;
                    u::sleep(timeToSleep);
                    actualTime    =  u::doubleClock(CLOCK_TYPE) - start;
                    scheduledTime += SLEEP_SECONDS;
                    overshoot     =  actualTime - scheduledTime;
                    if (secondsPerAction < overshoot * 4) {
                        ++retries;
                        const double overshootPercent = 100 * overshoot /
                                                              secondsPerAction;
                        ASSERTV(LINE, retries, overshootPercent, retries < 10);

                        if (10 < retries || veryVerbose) {
                            cout << "Retry << " << retries << " triggered. " <<
                                 overshootPercent << "% overshoot on line: " <<
                                 LINE << " clock type: " << CLOCK_TYPE << endl;
                        }

                        if (15 < retries) {
                            quit = true;
                            break;
                        }

                        // Back up to before preceeding init and try again.

                        for (--ti; e_CMD_INIT !=
                                          DATA[ti-- % k_NUM_DATA].d_command;) {
                            ;
                        }
                    }
                  } break;
                  case e_CMD_REQUEST: {
                    const int  NUM_ACTIONS = data.d_numActions;
                    const bool EXP         = data.d_expPermission;

                    if (SINGLE_ACTION_ONLY) {
                        if (NUM_ACTIONS > 1 && !EXP) {
                            continue;
                        }

                        for (int ii = 0; ii < NUM_ACTIONS; ++ii) {
                            double spltBefore = u::secondsPrevLeakTime(&mX);
                            const bool ret = mX.requestPermission();

                            if (veryVerbose) {
                                cout << "reqestPermission()";
                                if (NUM_ACTIONS > 1) {
                                    cout << ' ' << (ii + 1);
                                }
                                cout << " == " << u::b(ret) << endl;
                            }
                            ASSERTV(LINE, EXP, ret, ii, spltBefore,EXP == ret);
                        }
                    }
                    else {
                        double spltBefore = u::secondsPrevLeakTime(&mX);
                        const bool ret = mX.requestPermission(NUM_ACTIONS);

                        if (veryVerbose) cout << "reqestPermission(" <<
                                   NUM_ACTIONS << ") == " << u::b(ret) << endl;

                        ASSERTV(LINE, EXP, ret, overshoot, spltBefore,
                                                                   EXP == ret);
                    }
                  } break;
                  default: {
                    ASSERT(0 && "invalid CMD");
                  }
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'requestPermission' WITH TIME PASSED
        //
        // Concerns:
        //: 1 That request permission grants or refuses permission :
        //:   appropriately when a throttle is configured for finite
        //:   permission.
        //:
        //: 2 That request permission always grants permission when the
        //:   throttle is configured 'allow all'.
        //:
        //: 3 That request permission never grants permission when the throttle
        //:   is configured 'allow none'.
        //:
        //: 4 Negative testing of 'requestPermission'.
        //
        // Plan:
        //: 1 Construct a table with nullable values indicating how the
        //:   throttle is to be initialized, and with a 'const char *' field to
        //:   indicate time specs, an int field to indicate
        //:   'maxSimultaneousActions', and a bool field to indicate whether
        //:   permission is expected to be granted.  Rows are to take two
        //:   forms:
        //:   o 2nd & 3rd column specify args to 'initialize'
        //:   o 2nd & 3rd columns are -1, and the remaining columns specify the
        //:     args with which 'requestPermission' is to be called and the
        //:     expected result of this call.
        //:
        //: 2 Iterate through the table, and if it's an initialization record,
        //:   call 'initalize' on the throttle, otherwise call
        //:   'requestPermission' with the specified arguments and check that
        //:   the permission matches expectations.
        //:
        //: 3 Use the same table with a similar loop to drive the single-arg
        //;   'requestPermission'.
        //:
        //: 4 Write a loop going through the same table to drive testing of
        //:   a throttle initialized for 'allow all'.
        //:
        //: 5 Write a loop going through the same table to drive testing of
        //:   'allow none'.
        //:
        //: 6 Do negative testing.
        //
        // Testing:
        //   requestPermission(int, const bsls::TimeInterval&);
        //   requestPermission(const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'requestPermission' WITH TIME PASSED\n"
                             "============================================\n";

#undef  REQUEST
#define REQUEST   -1, -1

#undef  INITIALIZE
#define INITIALIZE 0, 0, 0

        static const struct Data {
            int         d_line;
            int         d_maxSimultaneousActions;
            Int64       d_nanosecondsPerAction;
            int         d_numActions;
            const char *d_timeStr;
            bool        d_expPermission;
        } DATA[] = {
            { L_, 1, 10,     INITIALIZE },
            { L_, REQUEST,   1, "0", 1 },
            { L_, REQUEST,   1, "0", 0 },
            { L_, REQUEST,   1, "0", 0 },
            { L_, REQUEST,   1, "n5", 0 },
            { L_, REQUEST,   1, "n10", 1 },
            { L_, REQUEST,   1, "n15", 0 },
            { L_, REQUEST,   1, "n10", 0 },
            { L_, REQUEST,   1, "0", 0 },
            { L_, REQUEST,   1, ":1", 1 },
            { L_, REQUEST,   1, ":1n5", 0 },
            { L_, REQUEST,   1, ":1n10", 1 },
            { L_, REQUEST,   1, ":10n0", 1 },
            { L_, REQUEST,   1, ":10n2", 0 },
            { L_, REQUEST,   1, ":10n8", 0 },
            { L_, REQUEST,   1, ":10n10", 1 },
            { L_, REQUEST,   1, "20:n", 1 },
            { L_, REQUEST,   1, "20:n", 0 },
            { L_, REQUEST,   1, "20:n", 0 },

            { L_, 4, 10,     INITIALIZE },
            { L_, REQUEST,   1, "0", 1 },
            { L_, REQUEST,   2, "0", 1 },
            { L_, REQUEST,   2, "0", 0 },
            { L_, REQUEST,   4, "0", 0 },
            { L_, REQUEST,   1, "0", 1 },
            { L_, REQUEST,   1, "0", 0 },
            { L_, REQUEST,   4, "0", 0 },
            { L_, REQUEST,   4, ":1", 1 },
            { L_, REQUEST,   1, ":1", 0 },
            { L_, REQUEST,   4, ":2", 1 },
            { L_, REQUEST,   1, ":2n5", 0 },
            { L_, REQUEST,   1, ":2n9", 0 },
            { L_, REQUEST,   2, ":2n10", 0 },
            { L_, REQUEST,   4, ":2n10", 0 },
            { L_, REQUEST,   1, ":2n10", 1 },
            { L_, REQUEST,   1, ":2n10", 0 },
            { L_, REQUEST,   1, ":2n10", 0 },
            { L_, REQUEST,   4, ":2n49", 0 },
            { L_, REQUEST,   4, ":2n50", 1 },
            { L_, REQUEST,   1, "15:", 1 },
            { L_, REQUEST,   1, "15:", 1 },
            { L_, REQUEST,   1, "15:", 1 },
            { L_, REQUEST,   1, "15:", 1 },
            { L_, REQUEST,   1, "15:", 0 },

            { L_, 4, u::k_SECOND, INITIALIZE },
            { L_, REQUEST,   2, "0", 1 },
            { L_, REQUEST,   2, "0", 1 },
            { L_, REQUEST,   1, "0", 0 },
            { L_, REQUEST,   1, "0:.1", 0 },
            { L_, REQUEST,   1, "0:.2", 0 },
            { L_, REQUEST,   1, "0:.5", 0 },
            { L_, REQUEST,   1, "0:.5", 0 },
            { L_, REQUEST,   1, "0:.5", 0 },
            { L_, REQUEST,   4, "0:1", 0 },
            { L_, REQUEST,   3, "0:1", 0 },
            { L_, REQUEST,   2, "0:1", 0 },
            { L_, REQUEST,   1, "0:1", 1 },
            { L_, REQUEST,   4, "0:1.1", 0 },
            { L_, REQUEST,   3, "0:1.1", 0 },
            { L_, REQUEST,   2, "0:1.1", 0 },
            { L_, REQUEST,   1, "0:1.1", 0 },
            { L_, REQUEST,   4, "0:1.2", 0 },
            { L_, REQUEST,   3, "0:1.2", 0 },
            { L_, REQUEST,   2, "0:1.2", 0 },
            { L_, REQUEST,   1, "0:1.2", 0 },
            { L_, REQUEST,   4, "0:1.2", 0 },
            { L_, REQUEST,   4, "0:1.3", 0 },
            { L_, REQUEST,   3, "0:1.3", 0 },
            { L_, REQUEST,   2, "0:1.3", 0 },
            { L_, REQUEST,   1, "0:1.3", 0 },
            { L_, REQUEST,   4, "0:1.9", 0 },
            { L_, REQUEST,   3, "0:1.9", 0 },
            { L_, REQUEST,   2, "0:1.9", 0 },
            { L_, REQUEST,   1, "0:1.9", 0 },
            { L_, REQUEST,   4, "0:5", 1 },
            { L_, REQUEST,   1, "100:", 1 },
            { L_, REQUEST,   4, "100:", 0 },
            { L_, REQUEST,   4, "100:1", 1 } };
#undef REQUEST
#undef INITIALIZE
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose)     cout << "Table-Driven Testing - 2 arg\n";
        if (veryVerbose) cout << "----------------------------\n";
        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&        data       = DATA[ti];
            const int          line       = data.d_line;
            const int          msa        = data.d_maxSimultaneousActions;
            const Int64        npa        = data.d_nanosecondsPerAction;
            const int          numActions = data.d_numActions;
            const char * const timeStr    = data.d_timeStr;
            const bool         EXP        = data.d_expPermission;

            static Obj mX;

            if (-1 != msa) {
                ASSERTV(line, npa, -1 < npa);

                ASSERTV(line, numActions, 0 == numActions);
                ASSERTV(line, timeStr, 0 == timeStr);
                ASSERTV(line, EXP, !EXP);

                if (veryVerbose) cout << "'initialize(" << msa << ", " <<
                                                                 npa << ");\n";

                mX.initialize(msa, npa);
                continue;
            }
            ASSERTV(line, npa, -1 == npa);

            const TimeInterval time = u::toTime(timeStr);

            ASSERTV(line, msa, npa, -1 == msa && -1 == npa);

            const bool ret = mX.requestPermission(numActions, time);

            if (veryVerbose) cout << "'requestPermissions(int, toTime(\"" <<
                                   timeStr << "\"))' == " << u::b(ret) << endl;

            ASSERTV(line, EXP, ret, EXP == ret);
        }

        if (verbose)     cout << "Table-Driven Testing - 1 arg\n";
        if (veryVerbose) cout << "----------------------------\n";
        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&        data       = DATA[ti];
            const int          line       = data.d_line;
            const int          msa        = data.d_maxSimultaneousActions;
            const Int64        npa        = data.d_nanosecondsPerAction;
            const int          numActions = data.d_numActions;
            const char * const timeStr    = data.d_timeStr;
            const bool         EXP        = data.d_expPermission;

            static Obj mX;

            if (-1 != msa) {
                ASSERTV(line, npa, -1 < npa);

                ASSERTV(line, numActions, 0 == numActions);
                ASSERTV(line, timeStr, 0 == timeStr);
                ASSERTV(line, EXP, !EXP);

                if (veryVerbose) cout << "'initialize(" << msa << ", " <<
                                                                 npa << ");\n";

                mX.initialize(msa, npa);
                continue;
            }
            ASSERTV(line, npa, -1 == npa);

            if (1 < numActions && !EXP) {
                continue;
            }

            const TimeInterval time = u::toTime(timeStr);

            ASSERTV(line, msa, npa, -1 == msa && -1 == npa);

            for (int tj = 0; tj < numActions; ++tj) {
                const bool ret = mX.requestPermission(time);

                if (veryVerbose) cout << "'requestPermissions(toTime(\"" <<
                                   timeStr << "\"))' == " << u::b(ret) <<
                                                     " - " << (tj + 1) << endl;

                ASSERTV(line, EXP, ret, tj, EXP == ret);
            }
        }

        if (verbose)     cout << "Allow All\n";
        if (veryVerbose) cout << "---------\n";
        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&        data       = DATA[ti];
            const int          line       = data.d_line;
            const int          msa        = data.d_maxSimultaneousActions;
            const int          numActions = data.d_numActions;
            const char * const timeStr    = data.d_timeStr;

            static Obj mX;

            if (-1 != msa) {
                mX.initialize(1, 0);    // allow all
                static const Obj mXAll = BDLMT_THROTTLE_INIT_ALLOW_ALL;
                ASSERT(0 == bsl::memcmp(&mX, &mXAll, sizeof(mX)));

                continue;
            }

            const TimeInterval time = u::toTime(timeStr);

            ASSERTV(line, mX.requestPermission(numActions, time));
            ASSERTV(line, mX.requestPermission(time));
            ASSERTV(line, mX.requestPermission(1, time));
            ASSERTV(line, mX.requestPermission(INT_MAX, time));
        }

        if (verbose)     cout << "Allow None\n";
        if (veryVerbose) cout << "----------\n";
        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&        data       = DATA[ti];
            const int          line       = data.d_line;
            const int          msa        = data.d_maxSimultaneousActions;
            const int          numActions = data.d_numActions;
            const char * const timeStr    = data.d_timeStr;

            static Obj mX;

            if (-1 != msa) {
                mX.initialize(0, 1);    // allow none
                static const Obj mXNone = BDLMT_THROTTLE_INIT_ALLOW_NONE;
                ASSERT(0 == bsl::memcmp(&mX, &mXNone, sizeof(mX)));

                continue;
            }

            const TimeInterval time = u::toTime(timeStr);

            ASSERTV(line, false == mX.requestPermission(numActions, time));
            ASSERTV(line, false == mX.requestPermission(time));
            ASSERTV(line, false == mX.requestPermission(1, time));
            ASSERTV(line, false == mX.requestPermission(INT_MAX, time));
        }

        if (verbose)     cout << "Negative Testing\n";
        if (veryVerbose) cout << "----------------\n";
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX = BDLMT_THROTTLE_INIT(4, u::k_SECOND);

            typedef TimeInterval TI;
            const TI time;

            const Int64 int64Max   = bsl::numeric_limits<Int64>::max();
            const Int64 secondsMax = int64Max / u::k_SECOND;
            const Int64 int64Min   = bsl::numeric_limits<Int64>::min();
            const Int64 secondsMin = int64Min / u::k_SECOND;

            if (veryVerbose) cout << "Negative Testing, 2-Arg\n";

            ASSERT_PASS(mX.requestPermission(1, time));
            ASSERT_PASS(mX.requestPermission(2, time));
            ASSERT_PASS(mX.requestPermission(3, time));
            ASSERT_PASS(mX.requestPermission(4, time));

            ASSERT_PASS(mX.requestPermission(1, TI(secondsMax, 100)));
            ASSERT_PASS(mX.requestPermission(2, TI(secondsMin, 0)));

            ASSERT_FAIL(mX.requestPermission(1, TI(secondsMax, 999999999)));
            ASSERT_FAIL(mX.requestPermission(2, TI(secondsMin, -999999999)));

            ASSERT_FAIL(mX.requestPermission(1, TI(int64Max, 0)));
            ASSERT_FAIL(mX.requestPermission(1, TI(int64Max, 999999999)));
            ASSERT_FAIL(mX.requestPermission(1, TI(int64Min, 0)));
            ASSERT_FAIL(mX.requestPermission(1, TI(int64Min, -999999999)));

            ASSERT_FAIL(mX.requestPermission(0, time));
            ASSERT_FAIL(mX.requestPermission(5, time));

            ASSERT_FAIL(mX.requestPermission(-1, time));
            ASSERT_FAIL(mX.requestPermission(INT_MIN, time));
            ASSERT_FAIL(mX.requestPermission(INT_MAX, time));

            if (veryVerbose) cout << "Negative Testing, 1-Arg\n";

            ASSERT_PASS(mX.requestPermission(TI(secondsMax, 100)));
            ASSERT_PASS(mX.requestPermission(TI(secondsMin, 0)));

            ASSERT_FAIL(mX.requestPermission(TI(secondsMax, 999999999)));
            ASSERT_FAIL(mX.requestPermission(TI(secondsMin, -999999999)));

            ASSERT_FAIL(mX.requestPermission(TI(int64Max, 0)));
            ASSERT_FAIL(mX.requestPermission(TI(int64Max, 999999999)));
            ASSERT_FAIL(mX.requestPermission(TI(int64Min, 0)));
            ASSERT_FAIL(mX.requestPermission(TI(int64Min, -999999999)));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //
        // Concerns:
        //: 1 That 'toTime' works as specced.
        //
        // Plan:
        //: 2 Call 'toTime' with table-driven inputs and observe the output is
        //:   as expected.
        //
        // TESTING
        //   TEST APPARATUS
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING TEST APPARATUS\n"
                             "======================\n";

        static const struct Data {
            int         d_line;
            const char *d_timeStr;
            int         d_seconds;
            int         d_nanoseconds;
        } DATA[] = {
            { L_, "",                       0,         0 },
            { L_, "0",                      0,         0 },
            { L_, ":.n",                    0,         0 },
            { L_, "0:.n",                   0,         0 },
            { L_, ":0.n",                   0,         0 },
            { L_, ":.0n",                   0,         0 },
            { L_, ":.n0",                   0,         0 },
            { L_, ":n0",                    0,         0 },
            { L_, "1:.n",                  60,         0 },
            { L_, ":1.n",                   1,         0 },
            { L_, ":.1n",                   0, 100000000 },
            { L_, ":.n1",                   0,         1 },
            { L_, "n1",                     0,         1 },
            { L_, ":n1",                    0,         1 },
            { L_, ".n1",                    0,         1 },
            { L_, "12:34.100n1",          754, 100000001 },
            { L_, "2:22.543n123",         142, 543000123 },
            { L_, "431.87n90",            431, 870000090 },
            { L_, ".n12345",                0,     12345 },
            { L_, "0.n12345",               0,     12345 },
            { L_, "0.0n12345",              0,     12345 },
            { L_, "0:0.0n12345",            0,     12345 },
            { L_, "23",                    23,         0 },
            { L_, "23.4",                  23, 400000000 },
            { L_, "1:23.3",                83, 300000000 },
            { L_, "1:23n300000000",        83, 300000000 },
            { L_, ":83n300000000",         83, 300000000 },
            { L_, "1:23.987654321",        83, 987654321 },
            { L_, "1:23.9876543",          83, 987654300 },
            { L_, "1:23n987654321",        83, 987654321 },
            { L_, "1:23n987654300",        83, 987654300 },
            { L_, "45:",                 2700,         0 },
            { L_, "45:0",                2700,         0 },
            { L_, "45:0.",               2700,         0 },
            { L_, "45:0.0",              2700,         0 },
            { L_, "45:0.0n",             2700,         0 },
            { L_, "45:0.0n0",            2700,         0 },
            { L_, "45:.",                2700,         0 },
            { L_, "45:.n",               2700,         0 } };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&        data        = DATA[ti];
            const int          LINE        = data.d_line;
            const char * const TIME_STR    = data.d_timeStr;
            const int          SECONDS     = data.d_seconds;
            const int          NANOSECONDS = data.d_nanoseconds;

            const TimeInterval EXP(SECONDS, NANOSECONDS);
            const TimeInterval ret = u::toTime(TIME_STR);

            ASSERTV(LINE, EXP, ret, EXP == ret);

            const double doubleVal = SECONDS + NANOSECONDS / 1e9;
            const double retDouble = ret.totalSecondsAsDouble();

            if (veryVerbose) { P_(doubleVal);    P(retDouble); }

            ASSERTV(LINE, doubleVal, retDouble, doubleVal + 1e-10 > retDouble);
            ASSERTV(LINE, doubleVal, retDouble, doubleVal - 1e-10 < retDouble);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'initialize' and ACCESSORS
        //
        // Concerns:
        //: 1 That after an object is initialized using 'initialize' that the
        //:   accessors verify the state of the object is as expected.
        //
        // Plan:
        //: 1 Set up a table and loop over it.
        //:
        //: 2 Each iteration, create two throttles, one of which has the clock
        //:   type defaulting, and one of which has the clock type from the
        //:   table passed.
        //:
        //: 3 If the clock type is monotonic, compare the two throttles and
        //:   verify that they are binary identical.
        //:
        //: 4 Use the 3 accessors to verify that the state of the throttle
        //:   corresponds to the table driven inputs.
        //:
        //: 5 Note that we never test the 'allow all' or 'allow none' cases in
        //:   this test case, those will be covered later.
        //
        // Testing:
        //   void initialize(int, Int64, SystemClockType::Enum);
        //   bsls::SystemClockType::Enum clockType() const;
        //   int maxSimultaneousActions() const;
        //   Int64 nanosecondsPerAction() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'initialize' and ACCESSORS\n"
                             "==================================\n";

        typedef bdlt::TimeUnitRatio TUR;

        if (verbose) cout << "Table-driven testing\n";

        static const ClockType mono = CT::e_MONOTONIC;
        static const ClockType real = CT::e_REALTIME;

        static const struct Data {
            int       d_line;
            int       d_maxSimultaneousActions;
            Int64     d_nanosecondsPerAction;
            ClockType d_clockType;
        } DATA[] = {
            { L_,    1, u::k_MILLI,                       mono },
            { L_,    1, u::k_MILLI,                       real },
            { L_,    5, u::k_MILLI,                       mono },
            { L_,    5, u::k_MILLI,                       real },
            { L_, 1000, u::k_SECOND,                      mono },
            { L_, 1000, u::k_SECOND,                      real },
            { L_,   10, 10 * TUR::k_NANOSECONDS_PER_HOUR, mono },
            { L_,   10, 10 * TUR::k_NANOSECONDS_PER_HOUR, real },
            { L_,  100, 2 * u::k_MILLI,                   mono },
            { L_,  100, 2 * u::k_MILLI,                   real }
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data& data                   = DATA[ti];
            const int   LINE                   = data.d_line;
            const int   maxSimultaneousActions = data.d_maxSimultaneousActions;
            const Int64 nanosecondsPerAction   = data.d_nanosecondsPerAction;
            const ClockType clockType          = data.d_clockType;

            Obj monoThrottle;
            monoThrottle.initialize(maxSimultaneousActions,
                                    nanosecondsPerAction);
            Obj mX;    const Obj& X = mX;
            mX.initialize(maxSimultaneousActions,
                          nanosecondsPerAction,
                          clockType);

            ASSERTV(LINE, CT::e_MONOTONIC != clockType ||
                            0 == bsl::memcmp(&monoThrottle, &mX, sizeof(Obj)));

            ASSERTV(LINE, maxSimultaneousActions ==X.maxSimultaneousActions());

            ASSERTV(LINE, nanosecondsPerAction == X.nanosecondsPerAction());

            ASSERTV(LINE,
                         X.clockType(), clockType, X.clockType() == clockType);
        }

        if (verbose) cout << "Negative Testing\n";
        {
            // values for 'maxSimultaneousActions'

            const int msaBad = INT_MIN;    (void) msaBad;
            const int msaLo  = -1;         (void) msaLo;
            const int msaMin = 0;
            const int msaMax = INT_MAX;

            // values for 'nanosecondsPerAction'

            const Int64 nsaBad = bsl::numeric_limits<Int64>::min();
            const Int64 nsaLo  = -1;    (void) nsaBad;    (void) nsaLo;
            const Int64 nsaMin = 0;
            const Int64 nsaMax = bsl::numeric_limits<Int64>::max();

            // values for 'clockType'

            const ClockType ctm   = CT::e_MONOTONIC;
            const ClockType ctr   = CT::e_REALTIME;
            const ClockType ctMin = (ClockType) bsl::min(ctm, ctr);
            const ClockType ctMax = (ClockType) bsl::max(ctm, ctr);
            const ClockType ctLo  = (ClockType) (ctMin - 1);    (void) ctLo;
            const ClockType ctHi  = (ClockType) (ctMax + 1);    (void) ctHi;

            for (int ti = 0; ti < 2; ++ti) {
                const ClockType ctValid = ti ? ctm : ctr;

                bsls::AssertTestHandlerGuard hG;

                Obj mX;
                ASSERT_PASS(mX.initialize(msaMin,      1, ctValid));
                ASSERT_PASS(mX.initialize(msaMin, nsaMax, ctValid));
                ASSERT_PASS(mX.initialize(     1, nsaMin, ctValid));
                ASSERT_PASS(mX.initialize(msaMax, nsaMin, ctValid));
                ASSERT_PASS(mX.initialize(     1,      1, ctValid));

                ASSERT_FAIL(mX.initialize(msaMin, nsaMin, ctValid));

                ASSERT_FAIL(mX.initialize(msaBad,      1, ctValid));
                ASSERT_FAIL(mX.initialize(msaBad, nsaMax, ctValid));
                ASSERT_FAIL(mX.initialize( msaLo,      1, ctValid));
                ASSERT_FAIL(mX.initialize( msaLo, nsaMax, ctValid));
                ASSERT_FAIL(mX.initialize(     1, nsaBad, ctValid));
                ASSERT_FAIL(mX.initialize(msaMax, nsaBad, ctValid));
                ASSERT_FAIL(mX.initialize(     1,  nsaLo, ctValid));
                ASSERT_FAIL(mX.initialize(msaMax,  nsaLo, ctValid));

                ASSERT_FAIL(mX.initialize(msaBad,  nsaLo, ctValid));
                ASSERT_FAIL(mX.initialize( msaLo, nsaBad, ctValid));
                ASSERT_FAIL(mX.initialize( msaLo,  nsaLo, ctValid));

                ASSERT_FAIL(mX.initialize(msaMin,      1, ctLo));
                ASSERT_FAIL(mX.initialize(msaMin, nsaMax, ctLo));
                ASSERT_FAIL(mX.initialize(     1, nsaMin, ctLo));
                ASSERT_FAIL(mX.initialize(msaMax, nsaMin, ctLo));
                ASSERT_FAIL(mX.initialize(     1,      1, ctLo));

                ASSERT_FAIL(mX.initialize(msaMin,      1, ctHi));
                ASSERT_FAIL(mX.initialize(msaMin, nsaMax, ctHi));
                ASSERT_FAIL(mX.initialize(     1, nsaMin, ctHi));
                ASSERT_FAIL(mX.initialize(msaMax, nsaMin, ctHi));
                ASSERT_FAIL(mX.initialize(     1,      1, ctHi));

                ASSERT_FAIL(mX.initialize( msaLo,  nsaLo, ctHi));
                ASSERT_FAIL(mX.initialize( msaLo,  nsaLo, ctLo));
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
        //: 1 Initialize some throttles to values, and manipulators and
        //:   accessors.
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
                                                                    u::k_MILLI;

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
            static const Int64    leakPeriod      = 1 * u::k_MILLI;
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
