// bsls_performancehint.t.cpp                                         -*-C++-*-
#include <bsls_performancehint.h>

#include <bsls_bsltestutil.h>


#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>   // GetSystemTimeAsFileTime, Sleep
#else
#include <unistd.h>    // sleep
#include <sys/time.h>  // gettimeofday
#include <stdint.h>    // int64_t
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1600
// stdint.h is only available starting is VS2010.
typedef unsigned long long int64_t;
#else
#include <stdint.h>
#endif

using namespace BloombergLP;
using namespace bsls;

//=============================================================================
//                                 TEST  PLAN
//-----------------------------------------------------------------------------
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
// 'using namespace BloombergLP' statement is intentionally commented out to
// test that these macros function outside namespace 'BloombergLP'.
// ----------------------------------------------------------------------------
// [ 1] Usage Example: Using 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' and
//                     'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY'
// [ 2] Usage Example: Using 'BSLS_PERFORMANCEHINT_PREDICT_EXPECT'
// [ 3] Usage Example: Using 'prefetchForReading' and 'prefetchForWriting'
//-----------------------------------------------------------------------------
// [-1] Performance Test: Verifies the performance of test 1, 2, 3
//-----------------------------------------------------------------------------

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace UsageExample1Case {

const int TESTSIZE = 10000000;  // test size used for timing
int global;                     // uninitialized on purpose to prevent compiler
                                // optimization

}  // close namespace UsageExample1Case

namespace UsageExample3Case {

const int SIZE = 10 * 1024 * 1024;  // big enough so not all data sits in cache

#if defined(BSLS_PLATFORM_CMP_SUN)
    // For some reason the sun machine is A LOT slower than the other
    // platforms, even in optimized mode.
const int TESTSIZE = 10;
#else
const int TESTSIZE = 100;
#endif

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlarger-than="
#endif

volatile int array1[SIZE]; // for 'addWithPrefetch'
volatile int array2[SIZE]; // for 'addWithPrefetch'

volatile int array3[SIZE]; // for 'addWithoutPrefetch
volatile int array4[SIZE]; // for 'addWithoutPrefetch

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

}  // close namespace UsageExample3Case

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------


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

class Stopwatch {
    // The 'class' provides an accumulator for the system time of the current
    // process.  A stopwatch can be in either the STOPPED (initial) state or
    // the RUNNING state.  The accumulated times can be accessed at any time
    // and in either state (RUNNING or STOPPED).

    // DATA
    int64_t d_startTime;
    int64_t d_accumulatedTime;
    bool    d_isRunning;

  public:
    Stopwatch() : d_startTime(0), d_accumulatedTime(0), d_isRunning(false) {}
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
        d_isRunning       = false;
        d_accumulatedTime = 0;
        d_startTime       = 0;
    }

    void start()
        // Place this stopwatch in the RUNNING state and begin accumulating
        // elapsed times if this object was in the STOPPED state.
    {
        d_isRunning = true;
        d_startTime = getTimer();
    }

    void stop()
        // Place this stopwatch in the STOPPED state, unconditionally stopping
        // the accumulation of elapsed times.  Note that the quiescent
        // accumulated elapsed times are available while in the STOPPED state.
    {
        d_isRunning = false;
        d_accumulatedTime = getTimer() - d_startTime;
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

    bool isRunning() const { return d_isRunning; }
        // Return 'true' if this stopwatch is accumulating time, and 'false'
        // otherwise.

};


//=============================================================================
//                              GLOBAL TEST CASES
//-----------------------------------------------------------------------------

namespace UsageExample1Case {

volatile int count1 = 0;
volatile int count2 = 0;

void foo()
    // Dummy function that sets the global variable.  Used to prevent the
    // compiler from optimizing the code too much.
{
    global = 1;
    count1++;
    count1++;
    count1++;
    count1++;

    count1++;
    count1++;
    count1++;
    count1++;
}

void bar()
    // Dummy function that sets the global variable.  Used to prevent the
    // compiler from optimizing the code too much.
{
    global = 2;
    count2++;
    count2++;
    count2++;
    count2++;

    count2++;
    count2++;
    count2++;
    count2++;
}

void testUsageExample1(int argc, bool assert)
{
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    double tolerance = 0.02;

    (void) assert;
    (void) verbose;
    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) tolerance;

    Stopwatch timer;

    timer.reset();

    if (veryVerbose) {
        printf("BSLS_PERFORMANCEHINT_PREDICT_LIKELY\n");
    }

    timer.start();

    for (int x = 0; x < TESTSIZE; ++x) {
        int y = rand() % 10;

        // Incorrect usage of 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' since there
        // is only a one in ten chance that this branch is taken.

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(y == 8)) {
            foo();
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            bar();
        }
    }

    timer.stop();
    double likelyTime = timer.elapsedTime();

    if (veryVerbose) {
        P(likelyTime);
    }


    if (veryVerbose) {
        printf("BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY\n");
    }

    timer.reset();
    timer.start();

    for (int x = 0; x < TESTSIZE; ++x) {
        int y = rand() % 10;

        // Correct usage of 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY' since there
        // is only a one in ten chance that this branch is taken.

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(y == 8)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            foo();
        }
        else {
            bar();
        }
    }
    timer.stop();
    double unlikelyTime = timer.elapsedTime();

    if (veryVerbose) {
        P(unlikelyTime);
    }

#if defined(BDE_BUILD_TARGET_OPT)
    // Only check under optimized build.

#if defined(BSLS_PLATFORM_CMP_CLANG)                                          \
 || defined(BSLS_PLATFORM_CMP_GNU)                                            \
 || defined(BSLS_PLATFORM_CMP_SUN)                                            \
 || (defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VERSION >= 0x0900)
    // Only check when 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' and
    // 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY' expands into something
    // meaningful.

    if (assert) {
        LOOP2_ASSERT(likelyTime, unlikelyTime,
                     likelyTime + tolerance > unlikelyTime);
    }

#endif

#endif

}

}  // close namespace UsageExample1Case

namespace UsageExample3Case {

void init(volatile int *arrayA, volatile int *arrayB)
{
#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VERSION >= 0x0900
    // Only available under xlc 10.

    for (int i = 0; i < SIZE; ++i){
        __dcbf((const void *)(arrayA++));
        __dcbf((const void *)(arrayB++));
    }
#else
    // suppress 'unused parameter' compiler warnings:
    (void) arrayA;
    (void) arrayB;
#endif
}

void addWithoutPrefetch(volatile int *arrayA, volatile int *arrayB)
    // Performs some form of addition on the specified 'arrayA' and 'arrayB'
    // without using prefetch.
{
    for (int i = 0; i < SIZE/8; ++i){
        *arrayA += *(arrayB++);
        ++arrayA;
        *arrayA += *(arrayB++);
        ++arrayA;
        *arrayA += *(arrayB++);
        ++arrayA;
        *arrayA += *(arrayB++);
        ++arrayA;

        *arrayA += *(arrayB++);
        ++arrayA;
        *arrayA += *(arrayB++);
        ++arrayA;
        *arrayA += *(arrayB++);
        ++arrayA;
        *arrayA += *(arrayB++);
        ++arrayA;
    }
}

void addWithPrefetch(volatile int *arrayA, volatile int *arrayB)
    // Performs some form of addition on the specified 'arrayA' and 'arrayB'
    // using prefetch.
{
    for (int i = 0; i < SIZE/8; ++i){

        // cast away the volatile qualifiers when calling 'prefetch*':

        BloombergLP::bsls::PerformanceHint::prefetchForWriting(
                                               const_cast<int *>(arrayA + 16));
        BloombergLP::bsls::PerformanceHint::prefetchForReading(
                                               const_cast<int *>(arrayB + 16));

        *arrayA += *(arrayB++);
        ++arrayA;
        *arrayA += *(arrayB++);
        ++arrayA;
        *arrayA += *(arrayB++);
        ++arrayA;
        *arrayA += *(arrayB++);
        ++arrayA;

        *arrayA += *(arrayB++);
        ++arrayA;
        *arrayA += *(arrayB++);
        ++arrayA;
        *arrayA += *(arrayB++);
        ++arrayA;
        *arrayA += *(arrayB++);
        ++arrayA;
    }
}

void testUsageExample3(int argc, bool assert)
{
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    // suppress 'unused parameter' compiler warnings:
    (void) assert;
    (void) verbose;
    (void) veryVeryVerbose;

    if (veryVerbose) {
        printf("Adding without prefetch\n");
    }

    UsageExample3Case::init(UsageExample3Case::array1,
                            UsageExample3Case::array2);

    Stopwatch timer;
    timer.start();

    for(int i = 0; i < TESTSIZE; ++i) {
        UsageExample3Case::addWithoutPrefetch(UsageExample3Case::array1,
                                              UsageExample3Case::array2);
    }

    timer.stop();
    double withoutPrefetch = timer.elapsedTime();

    if (veryVerbose) {
        P(withoutPrefetch);
    }

    if (veryVerbose) {
        printf("Adding with prefetch\n");
    }

    UsageExample3Case::init(UsageExample3Case::array3,
                            UsageExample3Case::array4);

    timer.reset();
    timer.start();

    for(int i = 0; i < UsageExample3Case::TESTSIZE; ++i) {
        addWithPrefetch(array3, array4);
    }

    timer.stop();
    double withPrefetch = timer.elapsedTime();

    if (veryVerbose) {
        P(withPrefetch);
    }

#if defined(BDE_BUILD_TARGET_OPT)
    // Only check under optimized build.

#if defined(BSLS_PLATFORM_CMP_CLANG)                                          \
 || defined(BSLS_PLATFORM_CMP_GNU)                                            \
 || defined(BSLS_PLATFORM_CMP_SUN)                                            \
 || defined(BSLS_PLATFORM_CMP_IBM)                                            \
 || defined(BSLS_PLATFORM_OS_WINDOWS)
    // Only check when 'prefetchForReading' or 'prefetchForWriting' expands
    // expands into something meaningful.

    double tolerance = 0.02;

    // Note that when compiling using 'bde_build.pl -t opt_exc_mt', the
    // optimization flag is set to '-O'.  Whereas, the optimization flag used
    // by 'IS_OPTIMIZED=1 pcomp' is set to '-xO2'.  Therefore, the improvement
    // in efficiency is much less than what's described in the usage example
    // when compiled using bde_build.

    if (assert) {
        // Only assert in performance test case.
        LOOP2_ASSERT(withoutPrefetch, withPrefetch,
                     withoutPrefetch + tolerance > withPrefetch);
    }

#endif

#endif

}

}  // close namespace UsageExample3Case

namespace ReorderingFenceTestCase {

void testReorderingFence(int argc)
{
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    // suppress 'unused parameter' compiler warnings:
    (void) verbose;
    (void) veryVeryVerbose;

    enum {
        CHUNK_SIZE = 1000
    };
    double    rateWithFence;
    double    rateWithoutFence;

    if (veryVerbose) {
        printf("With BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE\n");
    }
    {
        Stopwatch timer;
        timer.start();

        double elapsedTime = 0;
        int    iterations  = 0;
        int    numChunks   = 0;
        while (elapsedTime <= .4) {
            for (int i = 0; i < CHUNK_SIZE; ++i) {
                iterations++;
                BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE;
            }
            ++numChunks;
            elapsedTime = timer.elapsedTime();
        }

        rateWithFence = numChunks/elapsedTime;
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
        while (elapsedTime <= .4) {
            for (int i = 0; i < CHUNK_SIZE; ++i) {
                iterations++;
            }
            ++numChunks;
            elapsedTime = timer.elapsedTime();
        }

        rateWithoutFence = numChunks/elapsedTime;
    }
    if (veryVerbose) {
        printf("\trate = %f\n", rateWithoutFence);
    }

#if defined(BDE_BUILD_TARGET_OPT) &&                                          \
  !(defined(BSLS_PLATFORM_CMP_SUN) && (BSLS_PLATFORM_CMP_VERSION < 0x5110))

    // Perform this test only for optimized builds.  Also older Sun compilers
    // do not support this fence.

    LOOP2_ASSERT(rateWithFence, rateWithoutFence,
                 rateWithFence < rateWithoutFence);
#endif
}

}  // close namespace ReorderingFenceTestCase


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void) veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.


      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 3
        //   This will test the usage example 3 provided in the component
        //   header file for 'prefetchForReading' and 'prefetchForWriting'.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example using 'prefetchForReading' and
        //   'prefetchForWriting'.
        //
        // Testing:
        //   prefetchForReading
        //   prefetchForWriting
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING USAGE EXAMPLE 3"
                            "\n=======================\n");

        UsageExample3Case::testUsageExample3(argc, false);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
        //   This will test the usage example 2 provided in the component
        //   header file for 'BSLS_PERFORMANCEHINT_PREDICT_EXPECT'.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Ensure the usage example compiles.
        //
        // Testing:
        //   BSLS_PERFORMANCEHINT_PREDICT_EXPECT
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING USAGE EXAMPLE 2"
                            "\n=======================\n");

        int x = rand() % 4;

        // Incorrect usage of 'BSLS_PERFORMANCEHINT_PREDICT_EXPECT', since the
        // probability of getting a 3 is equivalent to other numbers (0, 1, 2).
        // However, this is sufficient to illustrate the intent of this macro.

        switch(BSLS_PERFORMANCEHINT_PREDICT_EXPECT(x, 3)) {
          case 1: //..
                  break;
          case 2: //..
                  break;
          case 3: //..
                  break;
          default: break;
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //   This will test the usage example 1 provided in the component
        //   header file for 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' and
        //   'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY'.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example using 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY'
        //   and 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY'.
        //
        // Testing:
        //   BSLS_PERFORMANCEHINT_PREDICT_LIKELY,
        //   BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING USAGE EXAMPLE 1"
                            "\n=======================\n");


        ASSERT(true);
        UsageExample1Case::testUsageExample1(argc, true);

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

        ReorderingFenceTestCase::testReorderingFence(argc);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING TEST-MACHINERY: 'Stopwatch'
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
        //   TESTING TEST-MACHINERY: 'Stopwatch'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST-MACHINERY: 'Stopwatch'"
                            "\n===================================\n");

        const double TOLERANCE = .5;

        if (veryVerbose) printf("\tCompare constructed 'Stopwatch'\n");
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
        // PERFORMANCE TEST
        //   Test the improvement of performance in using various performance
        //   hints.
        //
        // Concerns:
        //   The performance of using the performance hints in the various
        //   usage examples must be faster than not using them.
        //
        // Plan:
        //   Using 'Stopwatch', compare the time it takes to run the test
        //   using the performance hints and not using the hints.  Then compare
        //   and assert the time difference.  The test driver must observe an
        //   improvement in performance.  To minimize the effect of caching
        //   biasing the result, run the version that uses the proper hint
        //   first.
        //
        // Testing:
        //   PERFORMANCE TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nPERFORMANCE TEST"
                            "\n================\n");

        if (veryVerbose) {
            printf("Usage Example 1:\n");
        }

        UsageExample1Case::testUsageExample1(argc, true);

        if (veryVerbose) {
            printf("Usage Example 3:\n");
        }

        UsageExample3Case::testUsageExample3(argc, true);

      } break;
      case -2: {
        // --------------------------------------------------------------------
        // CONCERN: STOPWATCH ACCURACY
        //   This test is concerned with the accuracy of 'Stopwatch'. It is a
        //   negative test case because it takes ~1 minute to run.

        // Concerns:
        //: 1 That the 'elapsedTime' reported by 'Stopwatch' is accurate.
        //
        // Plan:
        //: 1 Perform a loop-based tested, sleeping over a series of different
        //:   delay periods and comparing the results of
        //:   'Stopwatch::elapsedTime' to 'time'
        //
        // Testing:
        //   CONCERN: STOPWATCH ACCURACY
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONCERN: STOPWATCH ACCURACY"
                            "\n===========================\n");

        if (veryVerbose) printf("\tCompare results w/ 'time'\n");

        for (int i = 1; i < 7; ++i){
            const int DELAY = i;

            time_t    startTime = time(0);
            Stopwatch mX;
            mX.start();

            sleep(DELAY);

            mX.stop();
            time_t expectedElaspedTime = time(0) - startTime;

            if (veryVerbose) {
                P_(DELAY); P_(mX.elapsedTime()); P(expectedElaspedTime);
            }
            ASSERT(mX.elapsedTime() < expectedElaspedTime + 1 &&
                   mX.elapsedTime() > expectedElaspedTime - 1);
        }
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
