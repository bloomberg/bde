// bsls_performancehint.t.cpp                                         -*-C++-*-
#include <bsls_performancehint.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>
#include <bsls_types.h> // 'BloombergLP::bsls::Types::Int64'
#include <bsls_unspecifiedbool.h>

#include <algorithm>   // 'std::sort'
#include <cassert>
#include <cstdlib>     // 'std::rand', 'std::srand'
#include <vector>

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>      // 'GetSystemTimeAsFileTime', 'Sleep'
#include <winbase.h>      // 'GetProcessTimes'
#else
#include <unistd.h>       // 'sleep'
#include <sys/time.h>     // 'gettimeofday'
#include <sys/resource.h> // 'struct rusage'
#endif

#ifdef BSLS_PLATFORM_CMP_CLANG
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#endif // BSLS_PLATFORM_CMP_CLANG

// ============================================================================
//                                 TEST  PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This component provides MACROs to facilitate more intelligent code
// generation by the compiler with regards to branch prediction and
// prefetching.  It is difficult to ensure the compiler behaves exactly as
// *hinted*.  This is because the hint is only taken under optimized build and
// various other optimizations are performed simultaneously by the compiler.
// Therefore, only the small usage example will be verified.
//
// In addition, macro safety is tested in all test cases.  The common
// 'using namespace BloombergLP' statement is intentionally ommitted to confirm
// that these macros are validly used outside of namespace 'BloombergLP'.
//
// The test cases of this test driver fall into three categories:
//: o Verification of usage examples (compile correctness).
//: o Tests of the helper classes.
//: o Manually run (negatively case numbered) measures of performance.
//
// A definitive test of this component's macros requires an examination of
// generated assembly language to confirm that the expected specialized
// instructions appear where expected.  Doing that is impractical from the test
// driver framework.  Attempts measure the time needed to complete a given work
// load with and without the preformance hint presents challenges since, in
// many cases, the performance improvement is only a percent or two, a value
// that difficult to reliably confirm in test runs that complete in reasonable
// time -- especially when running on heavily build machines where overall
// system load can (as has been observed) quickly change between measurements.
// Accordingly, timed measurements appear as manually run tests where human
// judgement can be applied.  The sole exception to this categorization is the
// test for 'BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE'.  That is placed
// "above the line" because the performance differs by an order of magnitude.
//
// The validity of the measurments of the manually-run test cases is further
// improved by:
//
//: o Running each test scenario multiple types and then reporting the
//:   minimum, median, and maximum of for the series of runs.
//:
//: o Where appropriate, judge performance by comparing accrued "user" time,
//:   not "wall" elapsed time.  User time for a fixed task shows less variance
//:   than "wall" time and makes sense when the expected change affects code
//:   execution within user space only.
//
// Note that the relatively low position of this component in the hierarchy
// forces us to avoid many of our conventional testing facilities.  Notably,
// this component uses a local definition of a 'Stopwatch' class to avoid a
// cycle that would be introduced if 'bsls_performancehint' depended on
// 'bsls_stopwatch'.  Similarly, 'BSLS_ASSERT*' macros (and their attendant
// negative testing) are eschewed in the implementation helper class
// 'Stopwatch' and helper functions lest a cycle be introduced -- the classic
// 'assert' macro is used instead.
// ----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE 3
// [ 4] USAGE EXAMPLE 2
// [ 3] USAGE EXAMPLE 1
// [ 2] BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE
// [ 1] TEST-MACHINERY: 'Stopwatch'
// ----------------------------------------------------------------------------
// [-1] CONCERN: STOPWATCH ACCURACY
// [-2] PERFORMANCE: _PREDICT_LIKELY, _PREDICT_UNLIKELY, _UNLIKELY_HINT
// [-3] PERFORMANCE: prefetchForReading, prefecthForWriting
// [ 6] CONCERN: 'bool' Coercion

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_OS_WINDOWS)
void sleep(unsigned int seconds)
{
    Sleep(seconds * 1000);
}
#endif

int64_t getTimer()
   // Return a 64-bit signed integer values indicating current time as an
   // offset in nanoseconds from some fixed (but unspecified) point in time.
   // Note that this value can be used to determine the number of nanoseconds
   // between two calls to 'getTimer'.  Note also that this deliberately
   // simple implementation is provided to avoid a dependency on
   // 'bsls_timeutil'.
{
    int64_t result;

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    const unsigned int k_NANOSECONDS_PER_TICK = 100;

    ULARGE_INTEGER fileTime;
    GetSystemTimeAsFileTime(reinterpret_cast<FILETIME *>(&fileTime));
    result = static_cast<int64_t>(fileTime.QuadPart * k_NANOSECONDS_PER_TICK);
#else
    timeval rawTime;
    gettimeofday(&rawTime, 0);

    const int64_t K = 1000;
    const int64_t M = 1000000;
    result = (static_cast<int64_t>(rawTime.tv_sec) * M + rawTime.tv_usec) * K;
#endif

    return result;
}

int64_t getTimerUser()
{
#if defined(BSLS_PLATFORM_OS_WINDOWS)
    static const BloombergLP::bsls::Types::Int64 s_nsecsPerUnit = 100;

    FILETIME crtnTm, exitTm, krnlTm, userTm;
    ULARGE_INTEGER userTimer;

    if (!::GetProcessTimes(::GetCurrentProcess(),
                           &crtnTm,
                           &exitTm,
                           &krnlTm,
                           &userTm)) {
        userTimer.QuadPart = 0;
    }

    userTimer.LowPart  = userTm.dwLowDateTime;
    userTimer.HighPart = userTm.dwHighDateTime;

    return userTimer.QuadPart * s_nsecsPerUnit;
#else

    static BloombergLP::bsls::Types::Int64 s_nsecsPerSecond      = 1000
                                                                 * 1000
                                                                 * 1000;
    static BloombergLP::bsls::Types::Int64 s_nsecsPerMicrosecond = 1000;

    struct rusage usage;

    int rc = getrusage(RUSAGE_SELF, &usage); (void) rc;

    assert(-1 != rc);  // Sanity check for validity of 'RUSAGE_SELF' and
                       // '&usage'.  Possible errors all require invalid input
                       // to 'getrusage'.

    BloombergLP::bsls::Types::Int64 timeSec  = 0;
    BloombergLP::bsls::Types::Int64 timeUsec = 0;

    timeSec  = static_cast<BloombergLP::bsls::Types::Int64>(
                                                        usage.ru_utime.tv_sec);
    timeUsec = static_cast<BloombergLP::bsls::Types::Int64>(
                                                       usage.ru_utime.tv_usec);
    return timeSec * s_nsecsPerSecond + timeUsec * s_nsecsPerMicrosecond;
#endif
}

class Stopwatch {
    // The 'class' provides an accumulator for the system time of the current
    // process.  A stopwatch can be in either the STOPPED (initial) state or
    // the RUNNING state.  The accumulated times can be accessed at any time
    // and in either state (RUNNING or STOPPED).

    // DATA
    int64_t d_startTime;
    int64_t d_accumulatedTime;

    bool    d_isRunning;

    int64_t d_startTimeUser;
    int64_t d_accumulatedTimeUser;

  public:
    Stopwatch()
    : d_startTime(0)
    , d_accumulatedTime(0)
    , d_isRunning(false)
    , d_startTimeUser(0)
    , d_accumulatedTimeUser(0) {}
        // Create a stopwatch in the STOPPED state having total accumulated
        // system, user, and wall times all equal to 0.0.

     //! ~Stopwatch();
        // Destroy this stopwatch.  Note that this method's definition is
        // compiler generated.

    // MANIPULATORS
    void reset()
        // Place this stopwatch in the STOPPED state, unconditionally stopping
        // the accumulation of elapsed times, and set the quiescent elapsed
        // times to 0.0.
    {
        d_isRunning           = false;
        d_accumulatedTime     = 0;
        d_startTime           = 0;
        d_accumulatedTimeUser = 0;
        d_startTimeUser       = 0;
    }

    void start()
        // Place this stopwatch in the RUNNING state and begin accumulating
        // elapsed times if this object was in the STOPPED state.
    {
        d_isRunning     = true;
        d_startTime     = getTimer();
        d_startTimeUser = getTimerUser();
    }

    void stop()
        // Place this stopwatch in the STOPPED state, unconditionally stopping
        // the accumulation of elapsed times.  Note that the quiescent
        // accumulated elapsed times are available while in the STOPPED state.
    {
        d_isRunning           = false;
        d_accumulatedTime     = getTimer()     - d_startTime;
        d_accumulatedTimeUser = getTimerUser() - d_startTimeUser;
    }

    // ACCESSORS
    double elapsedTime() const
        // Return the total (instantaneous and quiescent) elapsed wall time (in
        // seconds) accumulated by this stopwatch.  Note that this method is
        // equivalent to 'accumulatedWallTime'.
    {

        const double k_NanosecondsPerSecond = 1.0E9;

        int64_t elapsedTime = (d_isRunning)
                            ? getTimer() - d_startTime
                            : d_accumulatedTime;
        return (double)elapsedTime / k_NanosecondsPerSecond;
    }

    double userTime() const
        // Return the total (instantanous and quiescent) elapsed user time (in
        // seconds) accumulated by this stopwatch.  Note that this method is
        // equivalent to 'accumulatedUserTime'.
    {

        const double k_NanosecondsPerSecond = 1.0E9;

        int64_t userTime = (d_isRunning)
                         ? getTimerUser() - d_startTimeUser
                         : d_accumulatedTimeUser;
        return (double)userTime / k_NanosecondsPerSecond;
    }

    bool isRunning() const { return d_isRunning; }
        // Return 'true' if this stopwatch is accumulating time, and 'false'
        // otherwise.
};

// ============================================================================
//                              GLOBAL TEST CASES
// ----------------------------------------------------------------------------

namespace UsageExample1 {

///Usage
///-----
// The following series of examples illustrates use of the macros and functions
// provided by this component.
//
///Example 1: Using the Branch Prediction Macros
///- - - - - - - - - - - - - - - - - - - - - - -
// The following demonstrates the use of 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY'
// and 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY' to generate more efficient
// assembly instructions.  Note the use of 'BSLS_PERFORMANCEHINT_UNLIKELY_HINT'
// inside the 'if' branch for maximum portability.
//..
    volatile int global;

    void foo()
    {
        global = 1;
    }

    void bar()
    {
        global = 2;
    }

    int main(int argc, char **argv)
    {
        argc = std::atoi(argv[1]);

        for (int x = 0; x < argc; ++x) {
            int y = std::rand() % 10;

            // Correct usage of 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' since
            // there are nine of ten chance that this branch is taken.

            if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(8 != y)) {
                foo();
            }
            else {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                bar();
            }
        }
        return 0;
    }
//..
// An excerpt of the assembly code generated using 'xlC' Version 10 on AIX from
// this small program is:
//..
//  b8:   2c 00 00 08     cmpwi   r0,8
//  bc:   41 82 00 38     beq-    f4 <.main+0xb4>
//                         ^
//                         Note that if register r0 (y) equals 8, branch to
//                         instruction f4 (a jump).  The '-' after 'beq'
//                         indicates that the branch is unlikely to be taken.
//                         The predicted code path continues the 'if'
//                         statement, which calls 'foo' below.
//
//  c0:   4b ff ff 41     bl      0 <.foo__Fv>
//  ...
//  f4:   4b ff ff 2d     bl      20 <.bar__Fv>
//..
// Now, if 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' is changed to
// 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY', and the
// 'BSLS_PERFORMANCEHINT_UNLIKELY_HINT' is moved to the first branch, the
// following assembly code will be generated:
//..
//  b8:   2c 00 00 08     cmpwi   r0,8
//  bc:   40 c2 00 38     bne-    f4 <.main+0xb4>
//                         ^
//                         Note that the test became a "branch not equal"
//                         test.  The predicted code path now continues to the
//                         'else' statement, which calls 'bar' below.
//
//  c0:   4b ff ff 61     bl      20 <.bar__Fv>
//  ...
//  f4:   4b ff ff 0d     bl      0 <.foo__Fv>
//..
// A timing analysis shows that effective use of branch prediction can have a
// material effect on code efficiency:
//..
//  $time ./unlikely.out 100000000
//
//  real    0m2.022s
//  user    0m2.010s
//  sys     0m0.013s
//
//  $time ./likely.out 100000000
//
//  real    0m2.159s
//  user    0m2.149s
//  sys     0m0.005s
//..
}  // close namespace UsageExample1

namespace UsageExample3 {

// Note that the local 'Stopwatch' class is used below in lieu of
// 'BloombergLP::bsls::Stopwatch' to avoid a cyclic depenency between
// components.

///Example 3: Cache Line Prefetching
///- - - - - - - - - - - - - - - - -
// The following demonstrates use of 'prefetchForReading' and
// 'prefetchForWriting' to prefetch data cache lines:
//..
    const int SIZE = 10 * 1024 * 1024;

    void add(int *arrayA, int *arrayB)
    {
        for (int i = 0; i < SIZE / 8; ++i){
            *arrayA += *arrayB; ++arrayA; ++arrayB;
            *arrayA += *arrayB; ++arrayA; ++arrayB;
            *arrayA += *arrayB; ++arrayA; ++arrayB;
            *arrayA += *arrayB; ++arrayA; ++arrayB;

            *arrayA += *arrayB; ++arrayA; ++arrayB;
            *arrayA += *arrayB; ++arrayA; ++arrayB;
            *arrayA += *arrayB; ++arrayA; ++arrayB;
            *arrayA += *arrayB; ++arrayA; ++arrayB;
        }
    }

#if defined(BSLS_PLATFORM_CMP_GNU)                                            \
 && defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlarger-than=" // clang doesn't recognize this
#endif
    int array1[SIZE];
    int array2[SIZE];
#if defined(BSLS_PLATFORM_CMP_GNU)                                            \
 && defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#pragma GCC diagnostic pop
#endif

    int main()
    {
     // BloombergLP::bsls::Stopwatch timer;
        Stopwatch timer;
        timer.start();
        for (int i = 0; i < 10; ++i) {
            add(array1, array2);
        }
        printf("time: %f\n", timer.elapsedTime());
        return 0;
    }
//..
// The above code simply adds two arrays together multiple times.  Using
// 'bsls::Stopwatch', we recorded the running time and printed it to 'stdout':
//..
//  $./prefetch.sundev1.tsk
//  time: 8.446806
//..
// Now, we can observe that in the 'add' function, 'arrayA' and 'arrayB' are
// accessed sequentially for the majority of the program.  'arrayA' is used for
// writing and 'arrayB' is used for reading.  Making use of prefetch, we add
// calls to 'prefetchForReading' and 'prefetchForWriting':
//..
    void add2(int *arrayA, int *arrayB)
    {
        for (int i = 0; i < SIZE / 8; ++i){
            using namespace BloombergLP; // Generally avoid 'using' in this TD.
            bsls::PerformanceHint::prefetchForWriting((int *) arrayA + 16);
            bsls::PerformanceHint::prefetchForReading((int *) arrayB + 16);

            *arrayA += *arrayB; ++arrayA; ++arrayB;
            *arrayA += *arrayB; ++arrayA; ++arrayB;
            *arrayA += *arrayB; ++arrayA; ++arrayB;
            *arrayA += *arrayB; ++arrayA; ++arrayB;

            *arrayA += *arrayB; ++arrayA; ++arrayB;
            *arrayA += *arrayB; ++arrayA; ++arrayB;
            *arrayA += *arrayB; ++arrayA; ++arrayB;
            *arrayA += *arrayB; ++arrayA; ++arrayB;
        }
    }
//..
// Adding the prefetch improves the program's efficiency:
//..
//  $./prefetch.sundev1.tsk
//  time: 6.442100
//..
// Note that we prefetch the address '16 * sizeof(int)' bytes away from
// 'arrayA'.  This is such that the prefetch instruction has sufficient time to
// finish before the data is actually accessed.  To see the difference, if we
// changed '+ 16' to '+ 4':
//..
//  $./prefetch.sundev1.tsk
//  time: 6.835928
//..
// And we get less of an improvement in speed.  Similarly, if we prefetch too
// far away from the data use, the data might be removed from the cache before
// it is looked at and the prefetch is wasted.

}  // close namespace UsageExample3

namespace PerformanceLikelyUnlikely {

const int    TESTSIZE = 10 * 1000 * 1000;  // test size used for timing
volatile int global;

volatile int count1 = 0;
volatile int count2 = 0;

void foo()
    // Dummy function that sets the global variable.  Used to prevent the
    // compiler from optimizing the code too much.
{
    global = std::rand();
    count1 = std::rand();
    count1 = std::rand();
    count1 = std::rand();
    count1 = std::rand();

    count1 = std::rand();
    count1 = std::rand();
    count1 = std::rand();
    count1 = std::rand();
}

void bar()
    // Dummy function that sets the global variable.  Used to prevent the
    // compiler from optimizing the code too much.
{
    global = std::rand();
    count2 = std::rand();
    count2 = std::rand();
    count2 = std::rand();
    count2 = std::rand();

    count2 = std::rand();
    count2 = std::rand();
    count2 = std::rand();
    count2 = std::rand();
}

void measureLikelyUnlikely(int     argc,
                           double *outLikelyTime,
                           double *outUnlikelyTime)
{
    assert(  outLikelyTime);
    assert(outUnlikelyTime);

    int         verbose = argc > 2; (void) verbose;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    Stopwatch timer;

    timer.reset();

    if (veryVerbose) {
        printf("Misuse 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY'\n");
    }

    timer.start();

    for (int x = 0; x < TESTSIZE; ++x) {
        int y = std::rand() % 100;

        // Incorrect usage of 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' since there
        // is only a one in 100 chance that this branch is taken.

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(y == 8)) {
            foo();
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            bar();
        }
        if (veryVeryVerbose) { P(global) } // Deter optimization.
                                           // Do not set 'veryVeryVerbose' on
                                           // timed runs.
    }

    timer.stop();
    double likelyTime = timer.userTime();

    if (veryVerbose) { P(likelyTime) }

    if (veryVerbose) {
        printf("Use 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY' appropriately\n");
    }

    timer.reset();
    timer.start();

    for (int x = 0; x < TESTSIZE; ++x) {
        int y = std::rand() % 100;

        // Correct usage of 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY' since there
        // is only a one in 100 chance that this branch is taken.

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(y == 8)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            foo();
        }
        else {
            bar();
        }
        if (veryVeryVerbose) { P(global) }
    }
    timer.stop();
    double unlikelyTime = timer.userTime();

    if (veryVerbose) { P(unlikelyTime) }

      *outLikelyTime =   likelyTime;
    *outUnlikelyTime = unlikelyTime;
}

}  // close namespace PerformanceLikelyUnlikely

namespace PerformancePrefetch {

const int SIZE = 10 * 1024 * 1024;  // big enough so not all data sits in cache

#if defined(BSLS_PLATFORM_CMP_SUN)
    // For some reason the sun machine is A LOT slower than the other
    // platforms, even in optimized mode.
const int TESTSIZE =  10;
#else
const int TESTSIZE = 100;
#endif

#if defined(BSLS_PLATFORM_CMP_GNU)                                            \
 && defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlarger-than=" // clang doesn't recognize this
#endif

int arraySansW[SIZE]; // for 'addWithoutPrefetch (write)
int arraySansR[SIZE]; // for 'addWithoutPrefetch (read)

int arrayAvecW[SIZE]; // for 'addWithPrefetch'  (write)
int arrayAvecR[SIZE]; // for 'addWithPrefetch'  (read)

#if defined(BSLS_PLATFORM_CMP_GNU)                                            \
 && defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#pragma GCC diagnostic pop
#endif

void init(int argc, int *arrayA, int *arrayB)
{
#if defined(BSLS_PLATFORM_CMP_IBM)
    // Available since xlc 10.

    for (int i = 0; i < SIZE; ++i){
        __dcbf((const void *)(arrayA++));
        __dcbf((const void *)(arrayB++));
    }
#else
    std::srand(argc);
    for (int i = 0; i < SIZE; ++i) {
        *arrayA++ = std::rand();
        *arrayB++ = std::rand();
    }
#endif
}

void addWithoutPrefetch(int *arrayW, int *arrayR)
    // Performs some form of addition on the specified 'arrayW' and 'arrayR'
    // without using prefetch.
{
    for (int i = 0; i < SIZE/8; ++i){

        // Omit prefetch

    //  BloombergLP::bsls::PerformanceHint::prefetchForWriting(
    //                                         const_cast<int *>(arrayW + 16));
    //  BloombergLP::bsls::PerformanceHint::prefetchForReading(
    //                                         const_cast<int *>(arrayR + 16));
        *arrayW += *(arrayR++);
        ++arrayW;
        *arrayW += *(arrayR++);
        ++arrayW;
        *arrayW += *(arrayR++);
        ++arrayW;
        *arrayW += *(arrayR++);
        ++arrayW;

        *arrayW += *(arrayR++);
        ++arrayW;
        *arrayW += *(arrayR++);
        ++arrayW;
        *arrayW += *(arrayR++);
        ++arrayW;
        *arrayW += *(arrayR++);
        ++arrayW;
    }
}

void addWithPrefetch(int *arrayW, int *arrayR)
    // Performs some form of addition on the specified 'arrayW' and 'arrayR'
    // using prefetch.
{
    for (int i = 0; i < SIZE/8; ++i){

        // cast away the volatile qualifiers when calling 'prefetch*':

        BloombergLP::bsls::PerformanceHint::prefetchForWriting(
                                               const_cast<int *>(arrayW + 16));
        BloombergLP::bsls::PerformanceHint::prefetchForReading(
                                               const_cast<int *>(arrayR + 16));

        *arrayW += *(arrayR++);
        ++arrayW;
        *arrayW += *(arrayR++);
        ++arrayW;
        *arrayW += *(arrayR++);
        ++arrayW;
        *arrayW += *(arrayR++);
        ++arrayW;

        *arrayW += *(arrayR++);
        ++arrayW;
        *arrayW += *(arrayR++);
        ++arrayW;
        *arrayW += *(arrayR++);
        ++arrayW;
        *arrayW += *(arrayR++);
        ++arrayW;
    }
}

void measureWithWithoutPrefetch(int     argc,
                                double *outWithoutPrefetch,
                                double *outWithPrefetch)
{
    assert(outWithoutPrefetch);
    assert(   outWithPrefetch);

    int         verbose = argc > 2; (void)         verbose;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4; (void) veryVeryVerbose;

    if (veryVerbose) {
        printf("Adding without prefetch\n");
    }

    if (veryVerbose) {
        printf("initialize arrays\n");
    }

    init(argc, arraySansW, arraySansR);

    Stopwatch timer;
    timer.start();

    for(int i = 0; i < TESTSIZE; ++i) {
        addWithoutPrefetch(arraySansW, arraySansR);
    }

    timer.stop();
    double withoutPrefetch = timer.userTime();

    if (veryVerbose) {
        P(withoutPrefetch);
    }

    if (veryVerbose) {
        printf("Adding with prefetch\n");
    }

    if (veryVerbose) {
        printf("initialize arrays\n");
    }

    init(argc, arrayAvecW, arrayAvecR);

    timer.reset();
    timer.start();

    for(int i = 0; i < TESTSIZE; ++i) {
        addWithPrefetch(arrayAvecW, arrayAvecR);
    }

    timer.stop();
    double withPrefetch = timer.userTime();

    if (veryVerbose) {
        P(withPrefetch);
    }

    *outWithoutPrefetch = withoutPrefetch;
       *outWithPrefetch =    withPrefetch;
}

}  // close namespace PerformancePrefetch

namespace ReorderingFence {

void measureWithWithoutFence(int    argc,
                             double *outRateWithFence,
                             double *outRateWithoutFence)
{
    assert(outRateWithFence);
    assert(outRateWithoutFence);

    int         verbose = argc > 2;         (void) verbose;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4; (void) veryVeryVerbose;

    enum {
        CHUNK_SIZE = 1000
    };

    double       rateWithFence;
    double       rateWithoutFence;
    const double timeLimit = 0.2;  // elapse ("wall") time

    if (veryVerbose) {
        printf("With BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE\n");
    }

    {
        Stopwatch timer;
        timer.start();

        double elapsedTime = 0;
        int    iterations  = 0;
        int    numChunks   = 0;

        while (elapsedTime <= timeLimit) {
            for (int i = 0; i < CHUNK_SIZE; ++i) {
                iterations++;
                BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE;
            }
            ++numChunks;
            elapsedTime = timer.elapsedTime();
        }

        double userTime = timer.userTime();
        rateWithFence = numChunks/userTime;
    }

    if (veryVerbose) {
        printf("\trate = %f\n", rateWithFence);
    }

    if (veryVerbose) {
        printf("Without BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE\n");
    }
    {
        Stopwatch timer;
        timer.start();

        double elapsedTime = 0;
        int    iterations  = 0;
        int    numChunks   = 0;

        while (elapsedTime <= timeLimit) {
            for (int i = 0; i < CHUNK_SIZE; ++i) {
                iterations++;
            //  BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE;  // Omit the hint.
            }
            ++numChunks;
            elapsedTime = timer.elapsedTime();
        }

        double userTime = timer.userTime();
        rateWithoutFence = numChunks/userTime;
    }
    if (veryVerbose) {
        printf("\trate = %f\n", rateWithoutFence);
    }

    *outRateWithFence    = rateWithFence;
    *outRateWithoutFence = rateWithoutFence;
}

}  // close namespace ReorderingFence

namespace BoolCoercion {

class TestSmartPtr {
    // This class performs the same 'bool' conversion logic as
    // 'bsl::shared_ptr<T>' and 'bslma::ManagedPtr<T>'.

    typedef BloombergLP::bsls::UnspecifiedBool<TestSmartPtr>::
                                                             BoolType BoolType;

  public:
    operator BoolType() const {
        return BloombergLP::bsls::UnspecifiedBool<TestSmartPtr>::trueValue();
    }
};

class TestFunction {
    // This class performs the same 'bool' conversion logic as
    // 'bsl::function<PROTOTYPE>'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
  public:
    explicit operator bool() const {
        return true;
    }
#else
  private:
    typedef BloombergLP::bsls::UnspecifiedBool<TestFunction>
                                          UnspecifiedBoolUtil;
    typedef UnspecifiedBoolUtil::BoolType UnspecifiedBool;

  public:
    operator UnspecifiedBool() const {
        return UnspecifiedBoolUtil::makeValue(true);
    }
#endif
};

}  // close namespace BoolCoercion

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? std::atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void) veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'bool' Coercion
        //
        // Concerns:
        //   'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' and
        //   'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY' both take a boolean
        //   expression.  Any expresion that's convertible to 'bool', even
        //   explicitly, should be acceptable.
        //
        // Plan:
        // Try calling the two macros with various different expressions which
        // should all be convertible to 'bool'.  Successful compilation
        // indicates the conversions are supported.  Run-time tests confirm
        // that the expected value is produced and, in a boolean context, leads
        // to the execution of the expected branch.
        //
        // Testing:
        //   CONCERN: 'bool' Coercion
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\n" "TESTING 'bool' COERCION\n"
                        "=======================\n");
        }

        void                       *ptr = 0;
        BoolCoercion::TestFunction  fn;
        BoolCoercion::TestSmartPtr  sptr;

#define EXPECT(boolValue)                                                     \
        { aSsErT(!(boolValue), "unexpected branch", __LINE__); } else         \
        { aSsErT( (boolValue), "unexpected branch", __LINE__); }

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(false))   EXPECT(false)
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0))       EXPECT(false)
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(1))       EXPECT(true)
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(ptr))     EXPECT(false)
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(fn))      EXPECT(true)
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(sptr))    EXPECT(true)

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(static_cast<bool>(1.0)))
                                                          EXPECT(true)
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(static_cast<bool>(0.0)))
                                                          EXPECT(false)

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(true))  EXPECT(true)
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(false)) EXPECT(false)
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0))     EXPECT(false)
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1))     EXPECT(true)
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(ptr))   EXPECT(false)
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(fn))    EXPECT(true)
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(sptr))  EXPECT(true)

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(static_cast<bool>(1.0)))
                                                          EXPECT(true)
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(static_cast<bool>(0.0)))
                                                          EXPECT(false)

#undef EXPECT
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 3
        //   This will test the usage example 3 provided in the component
        //   header file for 'prefetchForReading' and 'prefetchForWriting'.
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
        //   USAGE EXAMPLE 3
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\n" "USAGE EXAMPLE 3\n"
                        "===============\n");
        }

        int rc = UsageExample3::main();
        ASSERT(0 == rc);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        //   This will test the usage example 2 provided in the component
        //   header file for 'BSLS_PERFORMANCEHINT_PREDICT_EXPECT'.
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
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE 2"
                            "\n===============\n");

///Example 2: Using 'BSLS_PERFORMANCEHINT_PREDICT_EXPECT'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This macro is essentially the same as the '__builtin_expect(expr, value)'
// macro that is provided by some compilers.  This macro allows the user to
// define more complex hints to the compiler, such as the optimization of
// 'switch' statements.  For example, given:
//..
    int x = std::rand() % 4;
//..
// the following is incorrect usage of 'BSLS_PERFORMANCEHINT_PREDICT_EXPECT',
// since the probability of getting a 3 is equivalent to the other
// possibilities ( 0, 1, 2 ):
//..
    switch (BSLS_PERFORMANCEHINT_PREDICT_EXPECT(x, 3)) {
      case 1: //..
              break;
      case 2: //..
              break;
      case 3: //..
              break;
      default: break;
    }
//..
// However, this is sufficient to illustrate the intent of this macro.

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        // Extracted from component header file.  This usage example uses:
        //:  o 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' and
        //:  o 'BSLS_PERFORMANCEHINT_UNLIKELY_HINT'
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
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE 1"
                            "\n===============\n");

        char  programName[] = "programName";
        char  numRuns[]     = "100000000";
        char *argv[]        = { programName, numRuns, 0 };
        int   argc          = sizeof argc / sizeof *argv;

        int rc = UsageExample1::main(argc, argv);
        ASSERT(0 == rc); (void) rc;

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING: BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE
        //
        // Concerns:
        //: 1 'OPTIMIZATION_FENCE' compiles on all platforms.
        //:
        //: 2 On platforms on which it is reasonably implemented,
        //:   'OPTIMIZATION_FENCE' impedes compiler optimizations on optimized
        //:   builds.
        //
        // Plan:
        //: 1 Perform a simple loop incrementing a counter for a set period of
        //:   time, both with and without the use of the 'OPTMIZATION_FENCE'.
        //:   Use a timer to verify the rate of increments using the
        //:   'OPTIMIZATION_FENCE' is slower.  Note that in practice, on
        //:   platforms on which the fence is reasonably implemented (*not*
        //:   older versions of Sun CC), the iteration with the
        //:   'OPTIMIZATION_FENCE" is very noticeably slower (several times
        //:   slower).
        //
        // Testing:
        //   BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE
        // --------------------------------------------------------------------

        if (verbose) printf(
            "\nTESTING: BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE"
            "\n================================================\n");

        double rateWithFence;
        double rateWithoutFence;

#if defined(BDE_BUILD_TARGET_OPT)
    // Perform this test only for optimized builds.

        const size_t NUM_SAMPLES = 21;

        std::vector<double>    statsRateWithFence;
        std::vector<double> statsRateWithoutFence;

        for (size_t run = 0; run < NUM_SAMPLES; ++run) {

            if (veryVerbose) {
                P(run);
            }

            ReorderingFence::measureWithWithoutFence(argc,
                                                     &rateWithFence,
                                                     &rateWithoutFence);

               statsRateWithFence.push_back(   rateWithFence);
            statsRateWithoutFence.push_back(rateWithoutFence);
        }

        std::sort(   statsRateWithFence.begin(),    statsRateWithFence.end());
        std::sort(statsRateWithoutFence.begin(), statsRateWithoutFence.end());

        assert(0                            <  statsRateWithFence.size());
        assert(1                            == statsRateWithFence.size() % 2);
        assert(statsRateWithoutFence.size() == statsRateWithFence.size());

        const size_t medianIndex = statsRateWithFence.size() / 2;

        double    rateWithFenceMedian =    statsRateWithFence[medianIndex];
        double rateWithoutFenceMedian = statsRateWithoutFence[medianIndex];

        if (veryVerbose) {
            double    rateWithFenceMin =    statsRateWithFence.front();
            double    rateWithFenceMax =    statsRateWithFence.back();
            double rateWithoutFenceMin = statsRateWithoutFence.front();
            double rateWithoutFenceMax = statsRateWithoutFence.back();

            P_(rateWithFenceMin)
            P_(rateWithFenceMedian)
            P_(rateWithFenceMax)
            P(NUM_SAMPLES)

            P_(rateWithoutFenceMin)
            P_(rateWithoutFenceMedian)
            P_(rateWithoutFenceMax)
            P(NUM_SAMPLES)
        }

        ASSERTV(rateWithFenceMedian,  rateWithoutFenceMedian, NUM_SAMPLES,
                rateWithFenceMedian < rateWithoutFenceMedian);
#else
        // Run once.
        ReorderingFence::measureWithWithoutFence(argc,
                                                 &rateWithFence,
                                                 &rateWithoutFence);
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TEST-MACHINERY: 'Stopwatch'
        //
        // Concerns:
        //: 1 That 'Stopwatch' is created in a STOPPED state with an elapsed
        //:   time of 0.
        //:
        //: 2 That 'start' puts the stopwatch in a RUNNING state
        //:
        //: 3 That 'stop' puts the soptwatch in a STOPPED state and recurds
        //:   the accumuted time.
        //:
        //: 4 That 'elaspsedTime' returns the correct elapsed time in
        //:   nanoseconds.
        //:
        //: 5 That 'reset' resets the 'Stopwatch' to its defualt constructed
        //:   state.
        //
        // Plan:
        //: 1 Construct a 'Stopwatch' and verify 'isRunning' is 'false' and
        //:   'elapsedTime' is 0. (C-1)
        //:
        //: 2 Construct a 'Stopwatch', call 'start', sleep for ~1 second and
        //:   verify 'isRunning' it 'true' and 'elapsedTime' is ~1 second.
        //:
        //: 2 Construct a 'Stopwatch', call 'start', sleep for ~1 second and
        //:   then stop the 'Stopwatch'.  Sleep another second. Verify
        //:   'isRunning' it 'false', 'elapsedTime' is ~1, and that the elapsed
        //:   time has not changed since the stopwatch was stopped.
        //
        // Testing:
        //   TEST-MACHINERY: 'Stopwatch'
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\n" "TEST-MACHINERY: 'Stopwatch'\n"
                        "===========================\n");
        }

        const double TOLERANCE = 0.5;

        if (veryVerbose) printf("Compare constructed 'Stopwatch'\n");
        {
            Stopwatch mX; const Stopwatch& X = mX;

            ASSERT(false == X.isRunning());
            ASSERT(0     == X.elapsedTime());
        }

        if (veryVerbose) printf("Test starting a stopwatch\n");
        {
            Stopwatch mX; const Stopwatch& X = mX;

            ASSERT(false == X.isRunning());
            ASSERT(0     == X.elapsedTime());

            mX.start();
            sleep(1);

            ASSERT(true  == X.isRunning());
            ASSERT(1 - TOLERANCE <  X.elapsedTime());
            ASSERT(1 + TOLERANCE >  X.elapsedTime());
        }

        if (veryVerbose) printf("Test stop and elapsedTime\n");
        {
            Stopwatch mX; const Stopwatch& X = mX;

            ASSERT(false == X.isRunning());
            ASSERT(0     == X.elapsedTime());

            mX.start();
            sleep(1);

            ASSERT(true  == X.isRunning());
            ASSERT(0     <  X.elapsedTime());

            mX.stop();

            double EXPECTED = X.elapsedTime();

            sleep(1);

            double ACTUAL   = X.elapsedTime();
            ASSERTV(EXPECTED, ACTUAL,
                    EXPECTED - EXPECTED * .00001 < ACTUAL &&
                    EXPECTED + EXPECTED * .00001 > ACTUAL);
            ASSERTV(X.elapsedTime(), 1 - TOLERANCE <  X.elapsedTime());
            ASSERTV(X.elapsedTime(), 1 + TOLERANCE >  X.elapsedTime());

            mX.reset();

            ASSERT(false == X.isRunning());
            ASSERT(0     == X.elapsedTime());
        }

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // CONCERN: STOPWATCH ACCURACY
        //   This test is concerned with the accuracy of 'Stopwatch'. It is a
        //   negative test case because it takes ~1 minute to run.
        //
        // Concerns:
        //: 1 That the 'elapsedTime' reported by 'Stopwatch' is accurate.
        //:
        //: 2 That the 'userTime' reported by 'Stopwatch' is (roughly)
        //:   proportional to the work done in user mode.
        //
        // Plan:
        //: 1 Perform a loop-based tested, sleeping over a series of different
        //:   delay periods and comparing the results of
        //:   'Stopwatch::elapsedTime' to 'time'.  (C-1)
        //:
        //: 2 Perform a series of progressively larger tasks that require
        //:   intensive, user-mode activity.  Confirm that the measured
        //:   user-time for those those tasks have the same ordering.  (C-2)
        //
        // Testing:
        //   CONCERN: STOPWATCH ACCURACY
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "CONCERN: STOPWATCH ACCURACY\n"
                                 "===========================\n");

        if (veryVerbose) printf("\tCompare results w/ 'time'\n");

        for (int i = 1; i < 7; ++i) {
            const int DELAY = i;

            time_t    startTime = time(0);
            Stopwatch mX;
            mX.start();

            sleep(DELAY);

            mX.stop();
            time_t expectedElapsedTime = time(0) - startTime;

            if (veryVerbose) {
                P_(DELAY); P_(mX.elapsedTime()); P(expectedElapsedTime);
            }

            double expectedHigh = static_cast<double>(expectedElapsedTime + 1);
            double expectedLow  = static_cast<double>(expectedElapsedTime - 1);

            ASSERTV(mX.elapsedTime(),  expectedHigh,
                    mX.elapsedTime() < expectedHigh);

            ASSERTV(mX.elapsedTime(),  expectedLow,
                    mX.elapsedTime() > expectedLow);
        }

#ifdef BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE
        if (veryVerbose) printf("\tTest userTime\n");
        {
            static volatile int s_value = 0;

            struct Task {
                enum  { CHUNK_SIZE = 1000 * 1000 };

                static void doit(volatile int *valuePtr) {
                    assert(valuePtr);

                    for (int i = 0; i < CHUNK_SIZE; ++i) {
                        BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE;
                        *valuePtr = i;
                    }
                }
            };

            Stopwatch mX; const Stopwatch& X = mX;
            mX.start();
            Task::doit(&s_value);    // 1
            mX.stop();
            double timeX = X.userTime();

            Stopwatch mY; const Stopwatch& Y = mY;
            mY.start();
            Task::doit(&s_value);    // 1
            Task::doit(&s_value);    // 2
            Task::doit(&s_value);    // 3
            Task::doit(&s_value);    // 4
            mY.stop();
            double timeY = Y.userTime();

            Stopwatch mZ; const Stopwatch& Z = mZ;
            mZ.start();
            Task::doit(&s_value);    // 01
            Task::doit(&s_value);    // 02
            Task::doit(&s_value);    // 03
            Task::doit(&s_value);    // 04
            Task::doit(&s_value);    // 05
            Task::doit(&s_value);    // 06
            Task::doit(&s_value);    // 07
            Task::doit(&s_value);    // 08
            Task::doit(&s_value);    // 09
            Task::doit(&s_value);    // 10
            Task::doit(&s_value);    // 11
            Task::doit(&s_value);    // 12
            Task::doit(&s_value);    // 13
            Task::doit(&s_value);    // 14
            Task::doit(&s_value);    // 15
            Task::doit(&s_value);    // 16
            mZ.stop();
            double timeZ = Z.userTime();

            if (veryVeryVerbose) {
                P_(timeX) P_(timeY) P(timeZ)
            }

            ASSERTV(timeX,   timeY,            timeZ,
                    timeX <= timeY && timeY <= timeZ);

        }
#else
        if (veryVerbose)    printf("\t" "Test 'userTime': SKIPPED \n");
        if (verVeryVerbose) printf(
             "\t" "'BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE' not supported\n"):
#endif
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // PERFORMANCE: _PREDICT_LIKELY, _PREDICT_UNLIKELY, _UNLIKELY_HINT
        //   Test the improvement of performance in using several, related
        //
        // Concerns:
        //   The performance of using the performance hints in Example 1 should
        //   be faster than not using them.  These test support evaluation of
        //   that performance gain on platforms where these macros are set.
        //
        // Plan:
        //: 1 Define 'measureLikelyUnlikely', a test function that iteratively
        //:   tests an expression that is *unlikely* to be 'true' wrapped by
        //:   the macros:
        //:   o 'BSLS_PERFORMANCEHINT_LIKELY' (i.e., misuse that macro). and
        //:     again wrapped by
        //:   o 'BSLS_PERFORMANCEHINT_UNLIKELY' (i.e., correctly using that
        //:     macro).
        //:   o Note that the structure of 'measureLikelyUnlikely' is inspired
        //:     by (but not identical to) Usage Example 1 (see TC 4).
        //:
        //: 2 The time measured for the misused macro is expected to be greater
        //:   than that for the correctly used macro.
        //:
        //: 3 Repeat the above test multiple times to account for timing
        //:   variations on the (time-shared) test machine.
        //:
        //: 4 Evaluate by "user" time, which shows less variance than "wall"
        //:   time.
        //
        // Testing:
        //   PERFORMANCE: _PREDICT_LIKELY, _PREDICT_UNLIKELY, _UNLIKELY_HINT
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\n"
          "PERFORMANCE: _PREDICT_LIKELY, _PREDICT_UNLIKELY, _UNLIKELY_HINT\n"
          "===============================================================\n");
        }

#if defined(BDE_BUILD_TARGET_OPT) // Check only under optimized build.

#if defined(BSLS_PLATFORM_CMP_CLANG)                                          \
 || defined(BSLS_PLATFORM_CMP_GNU)                                            \
 || defined(BSLS_PLATFORM_CMP_SUN)                                            \
 || defined(BSLS_PLATFORM_CMP_IBM)
    // Check only when 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' and
    // 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY' expands into something
    // meaningful.

        const size_t NUM_RUNS = 21;

        std::vector<double>   statsLikelyTime;
        std::vector<double> statsUnlikelyTime;

        for (size_t run = 0; run < NUM_RUNS; ++run) {
            if (veryVerbose) {
                P(run);
            }
            double   likelyTime;
            double unlikelyTime;

            PerformanceLikelyUnlikely::measureLikelyUnlikely(argc,
                                                             &likelyTime,
                                                             &unlikelyTime);

              statsLikelyTime.push_back(  likelyTime);
            statsUnlikelyTime.push_back(unlikelyTime);
        }

        std::sort(   statsLikelyTime.begin(),    statsLikelyTime.end());
        std::sort(statsUnlikelyTime.begin(), statsUnlikelyTime.end());

        assert(0                        <  statsLikelyTime.size());
        assert(1                        == statsLikelyTime.size() % 2);
        assert(statsUnlikelyTime.size() == statsLikelyTime.size());

        const size_t medianIndex = statsLikelyTime.size() / 2;

        double   likelyTimeMedian =   statsLikelyTime[medianIndex];
        double unlikelyTimeMedian = statsUnlikelyTime[medianIndex];

        if (veryVerbose) {
            double   likelyTimeMin =   statsLikelyTime.front();
            double   likelyTimeMax =   statsLikelyTime.back();
            double unlikelyTimeMin = statsUnlikelyTime.front();
            double unlikelyTimeMax = statsUnlikelyTime.back();

            P_(likelyTimeMin)
            P_(likelyTimeMedian)
            P_(likelyTimeMax)
            P(NUM_RUNS)

            P_(unlikelyTimeMin)
            P_(unlikelyTimeMedian)
            P_(unlikelyTimeMax)
            P(NUM_RUNS)
        }

        ASSERTV(likelyTimeMedian,  unlikelyTimeMedian, NUM_RUNS,
                likelyTimeMedian > unlikelyTimeMedian);

#else // PLATFORM
        if (veryVerbose) {
            printf("SKIP: optimized build but non-supported platform\n");
        }
#endif // PLATFORM
#else // BDE_BUILD_TARGET_OPT
        if (veryVerbose) {
            printf("SKIP: non-optimized build\n");
        }
#endif // BDE_BUILD_TARGET_OPT
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // PERFORMANCE: prefetchForReading, prefecthForWriting
        //   Test the improvement of performance in using performance hints for
        //   prefetching memory values.
        //
        // Concerns:
        //   The performance of using the performance hints in Example 3 should
        //   be faster than not using them.  These test support evaluation of
        //   that performance gain on platforms where these macros are set.
        //
        // Plan:
        //: 1 Define 'measureWithWithoutPrefetch', a test function that
        //:   iteratively copies values from one array to another:
        //:   o First without calling the 'prefetch*' functions.
        //:   o Then with calling the 'prefetch*' functions.
        //:   o Note that the structure of 'measureWithWithoutPrefetch' is
        //:     inspired by (but not identical to) Example 3 (see TC 5).
        //:
        //: 2 The time measured for the task without prefetch is expected to
        //:   exceed that when the prefetch functions are used.
        //:
        //: 3 Repeat the above test multiple times to account for timing
        //:   variations on the (time-shared) test machine.
        //:
        // Testing:
        //   PERFORMANCE: prefetchForReading, prefecthForWriting
        // --------------------------------------------------------------------

        if (verbose) {
            printf(
                 "\n" "PERFORMANCE: prefetchForReading, prefecthForWriting\n"
                      "===================================================\n");
        }

        double withoutPrefetch = 0.0;
        double    withPrefetch = 0.0;

#if defined(BDE_BUILD_TARGET_OPT)
    // Check only under optimized build.

#if defined(BSLS_PLATFORM_CMP_CLANG)                                          \
 || defined(BSLS_PLATFORM_CMP_GNU)                                            \
 || defined(BSLS_PLATFORM_CMP_SUN)                                            \
 || defined(BSLS_PLATFORM_CMP_IBM)                                            \
 || defined(BSLS_PLATFORM_OS_WINDOWS)
    // Check only when 'prefetchForReading' or 'prefetchForWriting' expands
    // expands into something meaningful.

    // Note that when compiling using 'bde_build.pl -t opt_exc_mt', the
    // optimization flag is set to '-O'.  Whereas, the optimization flag used
    // by 'IS_OPTIMIZED=1 pcomp' is set to '-xO2'.  Therefore, the improvement
    // in efficiency is much less than that described in the usage example when
    // compiled using 'bde_build'.

        const size_t MAX_NUM_SAMPLES = 1001;
        const size_t NUM_RUNS        =   11;

        if (veryVerbose) {
            P_(MAX_NUM_SAMPLES) P(NUM_RUNS)
        }

        std::vector<double> statsWithoutPrefetch;
        std::vector<double>    statsWithPrefetch;

        for (size_t run = 0; run < NUM_RUNS; ++run) {

            if (veryVerbose) {
                P(run);
            }

            PerformancePrefetch::measureWithWithoutPrefetch(argc,
                                                            &withoutPrefetch,
                                                            &withPrefetch);

            statsWithoutPrefetch.push_back(withoutPrefetch);
               statsWithPrefetch.push_back(   withPrefetch);
        }

        std::sort(   statsWithoutPrefetch.begin(),    statsWithoutPrefetch.end());
        std::sort(statsWithPrefetch.begin(), statsWithPrefetch.end());

        assert(0                        <  statsWithoutPrefetch.size());
        assert(1                        == statsWithoutPrefetch.size() % 2);
        assert(statsWithPrefetch.size() == statsWithoutPrefetch.size());

        const size_t medianIndex = statsWithoutPrefetch.size() / 2;

        double withoutPrefetchMedian = statsWithoutPrefetch[medianIndex];
        double    withPrefetchMedian =    statsWithPrefetch[medianIndex];

        if (veryVerbose) {
            double withoutPrefetchMin = statsWithoutPrefetch.front();
            double withoutPrefetchMax = statsWithoutPrefetch.back();
            double    withPrefetchMin =    statsWithPrefetch.front();
            double    withPrefetchMax =    statsWithPrefetch.back();

            P_(withoutPrefetchMin)
            P_(withoutPrefetchMedian)
            P_(withoutPrefetchMax)
            P(NUM_RUNS)

            P_(withPrefetchMin)
            P_(withPrefetchMedian)
            P_(withPrefetchMax)
            P(NUM_RUNS)
        }

        ASSERTV(withoutPrefetchMedian,  withPrefetchMedian, NUM_RUNS,
                withoutPrefetchMedian > withPrefetchMedian);
#else // PLATFORM
        (void) withoutPrefetch;
        (void)    withPrefetch;

        if (veryVerbose) {
            printf("SKIP: optimized build but non-supported platform\n");
        }

#endif // PLATFORM
#else // BDE_BUILD_TARGET_OPT
        (void) withoutPrefetch;
        (void)    withPrefetch;

        if (veryVerbose) {
            printf("SKIP: non-optimized build\n");
        }
#endif // BDE_BUILD_TARGET_OPT
      } break;
       default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
