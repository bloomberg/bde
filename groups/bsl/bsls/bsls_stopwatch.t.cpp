// bsls_stopwatch.t.cpp                                               -*-C++-*-
#include <bsls_stopwatch.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>
#include <bsls_timeutil.h>   // getTimer() (used in delay generation)

#include <stdio.h>           // printf(), puts()
#include <stdlib.h>          // atoi()

// The following OS-dependent includes are for the 'osSystemCall' helper
// function below.

#if defined BSLS_PLATFORM_OS_UNIX
    #include <sys/times.h>
    #include <unistd.h>
#elif defined BSLS_PLATFORM_OS_WINDOWS
    #include <windows.h>
#endif

using namespace BloombergLP;


//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test has state, but not value.  State transitions must
// be confirmed explicitly.  The timing functions can be verified to a
// reasonable degree using the 'bsls::TimeUtil::get<...>Timer' functions
// (with which the stopwatch class is implemented) to confirm "reasonable"
// behavior.
//-----------------------------------------------------------------------------
// [ 2] bsls::Stopwatch();
// [ 2] bsls::Stopwatch(const bsls::Stopwatch& other);
// [ 2] ~bsls::Stopwatch();
// [ 3] void start();
// [ 3] void stop();
// [ 3] void reset();
// [ 2] bool isRunning() const;
// [ 4] double accumulatedSystemTime() const;
// [ 4] double accumulatedUserTime() const;
// [ 4] double accumulatedWallTime() const;
// [ 5] void accumulatedTimes(double*, double*, double*) const;
// [ 4] double elapsedTime() const;
//-----------------------------------------------------------------------------
// [ 1] Breathing Test
// [ 2] State Transitions
// [ 7] USAGE Example
// [ 6] Reproduce bug from test case
//-----------------------------------------------------------------------------

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

namespace {

bool isEqual(double a, double b)
    // Return 'true' if the specified 'a' equals the specified 'b', and 'false'
    // otherwise.  Note that comparing two floating-point values on GCC x86
    // platforms due to extended precision of x87 unit may lead to some strange
    // result when two the same floating-point values are not equal.  We can
    // workaround this problem by always explicitly storing the values to
    // memory to force the round-down using 'volatile' intermediate variables.
    // See https://gcc.gnu.org/wiki/x87note for details.
{
    volatile double va = a;
    volatile double vb = b;
    return va == vb;
}

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

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
//                     LOCAL HELPER MACROS FOR READABLE CODE
//-----------------------------------------------------------------------------

#define u_DBGPRINT(lead, expr, trail)                                         \
    bsls::BslTestUtil::callDebugprint(expr, lead, trail "\n")
    // Print the specified 'expr' value with the specified 'lead' string
    // literal as prefix, and the specified 'trail' string literal with an
    // appended newline character as postfix using
    // 'bsls::BslTestUtil::callDebugprint'.

#define u_VDBGPRINT(lead, expr, trail)                                        \
    do { if (verbose) { u_DBGPRINT(lead, expr, trail); } } while (false)
    // If 'verbose' print the specified 'expr' value with the specified 'lead'
    // string literal as prefix, and the specified 'trail' string literal with
    // an appended newline character as postfix using
    // 'bsls::BslTestUtil::callDebugprint'.

#define u_VVDBGPREFPRINT(lead, expr) do { if (veryVerbose) {                  \
    bsls::BslTestUtil::callDebugprint(expr, lead, "\n"); } } while (false)
    // If 'veryVerbose' print the specified 'expr' value with the specified
    // 'lead' string literal as prefix, and a newline character as postfix
    // using 'bsls::BslTestUtil::callDebugprint'.

//=============================================================================
//                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls::Stopwatch    Obj;

typedef bsls::TimeUtil     TU;
typedef bsls::Types::Int64 Int64;

typedef double (Obj::*AccumulatedTimeMethod)() const;

typedef Int64 (*RawTimerFunction)();
typedef double (*ResourceBurningFunction)(double optionalUserData);
typedef Int64 (*LoopBodyFunction)(RawTimerFunction timerFn);

//=============================================================================
//            VOLATILE GLOBALS AGAINST OPTIMIZATION OF BENCHMARKS
//-----------------------------------------------------------------------------

static volatile unsigned busyFunctionSink = 0;
    // See 'busyFunction()' below.

static volatile double dblSink = 0.0;
    // Used in 'main' benchmark cases.

#if defined(BSLS_PLATFORM_OS_UNIX)
static volatile pid_t   pidSink;
static volatile clock_t clockSink;
#endif


//=============================================================================
//                             HELPER FUNCTIONS
//-----------------------------------------------------------------------------

Int64 emptyFunction(RawTimerFunction timerFn)
{
    return timerFn();
}

void busyFunction()
{
    for (unsigned i = 0; i < 10; ++i) {
        for (unsigned j  = 0;j < 10; ++j) {
            busyFunctionSink = busyFunctionSink + i*j+ j*i;
        }
    }
}

void exchangeInts(int *a, int *b)
    // Exchange the value of the 'int's stored in the specified addresses 'a'
    // and 'b, in a manner that minimizes the chance of the operation being
    // optimized away by a smart compiler.
{
    volatile int temp = *a;
    *a = *b;
    *b = temp;
}

// Do some work here, as we need to consume user time.  The more we do the
// better, so we are intentionally inefficient.

int dataArray[17] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
};

Int64 osUserCall(RawTimerFunction timerFn)
{
    for (int k = 0; k < 100; ++k) {
        for (int i = 0; i < 17-1; ++i) {
            for (int j = i+1; j < 17; ++j) {
                if (   (k%2 == 0 && dataArray[i] < dataArray[j]++)
                    || (k%2 != 0 && dataArray[i] > dataArray[j]++)) {
                    exchangeInts(&dataArray[i], &dataArray[j]);
                }
            }
        }
        fflush(stdout);
    }
    return timerFn();
}

Int64 osSystemCall(RawTimerFunction timerFn)
{
#if defined BSLS_PLATFORM_OS_UNIX
    pidSink = getpid();
    struct tms tt;
    clockSink = times(&tt);
#elif defined BSLS_PLATFORM_OS_WINDOWS
    HANDLE ph = ::GetCurrentProcess();
    FILETIME crtnTm, exitTm, krnlTm, userTm;
    ::GetProcessTimes(ph, &crtnTm, &exitTm, &krnlTm, &userTm);
#endif
    return timerFn();
}

static void shortDelay(double                  delayTime,
                       RawTimerFunction        rawTimerFunction,
                       ResourceBurningFunction burnFunction,
                       bool                    veryVeryVeryVerbose)
    // Repeatedly call the specified 'burnFunction' until the specified
    // 'delayTime' nanoseconds of the resource measured by the specified
    // 'rawTimerFunction' have been consumed, and print the sum of the return
    // values of 'burnFunction' if the specified 'veryVeryVeryVerbose' flag
    // evaluates to 'true'.  Note that 'burnFunction' should burn only a tiny
    // bit of the same resource measured by 'rawTimerFunction', so that
    // 'shortDelay' can exit as soon as possible after the 'delayTime' target
    // has been achieved.  When choosing or designing 'burnFunction', clients
    // should keep in mind that some system time will be consumed just by
    // calling 'rawTimerFunction' to measure the time resource consumption.
{
    const Int64 t0 = rawTimerFunction();
    const Int64 tEnd = t0 + static_cast<Int64>(delayTime * 1e9);

    volatile double x = 0;

    while ((*rawTimerFunction)() < tEnd) {
        // Not using 'x += ...;' because:
        // WARNING: compound assignment to object of volatile-qualified type
        // 'volatile double' is deprecated
        x = x + (*burnFunction)(delayTime);
    }

    if (veryVeryVeryVerbose) {
        // Optionally print out the cumulative result of all calls to
        // 'burnFunction'.  The value of 'veryVeryVeryVerbose' depends on the
        // command line with which the test driver is invoked, and therefore
        // the optimizer cannot assume that 'shortDelay' and 'burnFunction'
        // have no side effects.  In practice, we will never enter this branch
        // on actual test driver runs.

        P(x)
    }
}

static double burnMinimalSystemTime(double)
    // Do nothing.  Note that this function is designed to be used with
    // 'shortDelay', and need only use enough system time to ensure that system
    // time is not completely dominated by user time during a call to
    // 'shortDelay'.  Since 'shortDelay' already uses a small amount of system
    // time to measure a time resource, this function does not need to do
    // anything at all.
{
    return 0.0;
}

static double burnMinimalUserTime(double delayTime)
    // Perform some calculations that use an arbitrary, but small amount of
    // user time.  Note that this function is designed to be used with
    // 'shortDelay', and need only use up enough user time to ensure that user
    // time is not completely dominated by system time during a call to
    // 'shortDelay'.
{
    const double frac = delayTime * 4.7;
    volatile double x = delayTime;

    for (int i = 0; i < 100000; ++i) {
        // Not using 'x += ...;' because:
        // WARNING: compound assignment to object of volatile-qualified type
        // 'volatile double' is deprecated
        x = x + delayTime / frac;    // expensive operation
    }

    return x;
}

static Int64 delay(double           delayTime,
                   RawTimerFunction rawTimerFunction,
                   LoopBodyFunction loopBodyFunction)
{
    const Int64 t0 = rawTimerFunction();
    const Int64 t1 = t0 + static_cast<Int64>(delayTime * 1.0e9) + 100;
          Int64 t  = rawTimerFunction();

    while (t < t1) {
        t = loopBodyFunction(rawTimerFunction);
    }

    return t - t0;
}

static inline Int64 delayWall(double delayTime)
{
    return delay(delayTime, &TU::getTimer, &emptyFunction);
}

static inline Int64 delayUser(double delayTime)
{
    return delay(delayTime, &TU::getProcessUserTimer, &osUserCall);
}

static inline Int64 delaySystem(double delayTime)
{
    return delay(delayTime, &TU::getProcessSystemTimer, &osSystemCall);
}

// ============================================================================
//                               TEST DATA TYPE
// ----------------------------------------------------------------------------

struct TimeMethods {

    // PUBLIC DATA
    AccumulatedTimeMethod   d_timeMethod;
    Int64                 (*d_delayFunction_p)(double);
    const char             *d_timeMethodName_p;

    // PUBLIC ACCESSORS
    double accumulated(const Obj& stopWatch) const
        // Return the type of accumulated time of this object from the
        // specified 'stopWatch'.
    {
        return (stopWatch.*(d_timeMethod))();
    }

    Int64 delay(double delayTime) const
        // Call the delay function of this object with the specified
        // 'delayTime' and return its result.
    {
        return (*d_delayFunction_p)(delayTime);
    }

    bool isWallTime() const
        // Return 'true' if this object's time method collects wall time,
        // otherwise, if it collects CPU time, return 'false'.
    {
        return (d_timeMethod == &Obj::accumulatedWallTime);
    }

    const char *timeMethodName() const
        // Return the name of the accumulated time method of this object.
    {
        return d_timeMethodName_p;
    }

};


// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;


    //=====================================================================
    //                              TEST DATA
    //---------------------------------------------------------------------

    // This used to be a top-level static array above main, but that caused
    // an internal compiler failure in aCC
    const TimeMethods TIME_METHODS[] = {
        { &Obj::accumulatedSystemTime, &delaySystem, "accumulatedSystemTime" },
        { &Obj::accumulatedUserTime,   &delayUser,   "accumulatedUserTime"   },
        { &Obj::accumulatedWallTime,   &delayWall,   "accumulatedWallTime"   }
    };

    const size_t NUM_TIME_METHODS = sizeof TIME_METHODS / sizeof *TIME_METHODS;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms.
        //
        // Plan:
        //   Copy the usage example exactly, substituting 'ASSERT' for 'assert'
        //   and confirm that the build does not break and that the test case
        //   runs and exits normally.
        //
        // Testing:
        //   USAGE
        // --------------------------------------------------------------------

        if (verbose) puts("\nTesting Usage Example"
                          "\n=====================");

        bsls::Stopwatch s;
        const double t0s = s.accumulatedSystemTime();  ASSERT(0.0 == t0s);
        const double t0u = s.accumulatedUserTime();    ASSERT(0.0 == t0u);
        const double t0w = s.accumulatedWallTime();    ASSERT(0.0 == t0w);

        s.start();
        const double t1s = s.accumulatedSystemTime();  ASSERT(0.0 == t1s);
        const double t1u = s.accumulatedUserTime();    ASSERT(0.0 == t1u);
        const double t1w = s.accumulatedWallTime();    ASSERT(0.0 <= t1w);

        s.stop();
        const double t2s = s.accumulatedSystemTime();ASSERT(isEqual(t1s, t2s));
        const double t2u = s.accumulatedUserTime();  ASSERT(isEqual(t1u, t2u));
        const double t2w = s.accumulatedWallTime();  ASSERT(t1w <= t2w);

        s.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
        const double t3s = s.accumulatedSystemTime();  ASSERT(t2s <= t3s);
        const double t3u = s.accumulatedUserTime();    ASSERT(t2u <= t3u);
        const double t3w = s.accumulatedWallTime();    ASSERT(t2w <= t3w);

        s.reset();
        const double t4s = s.accumulatedSystemTime();  ASSERT(0.0 == t4s);
        const double t4u = s.accumulatedUserTime();    ASSERT(0.0 == t4u);
        const double t4w = s.accumulatedWallTime();    ASSERT(0.0 == t4w);
        const double t5s = s.accumulatedSystemTime();  ASSERT(0.0 == t5s);
        const double t5u = s.accumulatedUserTime();    ASSERT(0.0 == t5u);
        const double t5w = s.accumulatedWallTime();    ASSERT(0.0 == t5w);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // ATTEMPT TO REPRODUCE BUG PRODUCING NEGATIVE TIMES
        //
        // Concern:
        //   bsls::Stopwatch is returning negative times in the field, attempt
        //   to reproduce here.
        //
        // Plan:
        //   Call bsls::Stopwatch many times and see if it ever returns a
        //   negative time.
        // --------------------------------------------------------------------

        if (verbose) puts("\nAttempt to reproduce negative times bug"
                          "\n=======================================");

        bsls::Stopwatch mX;    const bsls::Stopwatch& X = mX;

        const double delayTime = 1e-6;

        double totalSTime = 0;
        double totalUTime = 0;
        double totalWTime = 0;

        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 3; ++j) {
                mX.reset();
                mX.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);

                // Use up a tiny bit of one of the three time resources.  The
                // idea here is that we want to keep the accumulated times as
                // small as possible, but guarantee that at least one of them
                // has had an opportunity to be non-zero.

                switch (j) {
                  case 0: {
                    shortDelay(delayTime,
                               &TU::getProcessSystemTimer,
                               &burnMinimalSystemTime,
                               veryVeryVeryVerbose);
                  } break;
                  case 1: {
                    shortDelay(delayTime,
                               &TU::getProcessUserTimer,
                               &burnMinimalUserTime,
                               veryVeryVeryVerbose);
                  } break;
                  case 2: {
                    shortDelay(delayTime,
                               &TU::getTimer,
                               &burnMinimalUserTime,
                               veryVeryVeryVerbose);
                  }
                }
                mX.stop();

                double stime = X.accumulatedSystemTime();
                double utime = X.accumulatedUserTime();
                double wtime = X.accumulatedWallTime();
                totalSTime += stime;
                totalUTime += utime;
                totalWTime += wtime;

                LOOP2_ASSERT(stime, j, stime >= 0);
                LOOP2_ASSERT(stime, j, stime < 10);
                if (0 == j) LOOP2_ASSERT(stime, j, stime >= delayTime);

                LOOP2_ASSERT(utime, j, utime >= 0);
                LOOP2_ASSERT(utime, j, utime < 10);
                if (1 == j) LOOP2_ASSERT(utime, j, utime >= delayTime);

                LOOP2_ASSERT(wtime, j, wtime >= 0);
                LOOP2_ASSERT(wtime, j, wtime < 10);

                if (2 == j) LOOP2_ASSERT(wtime, j, wtime >= delayTime);

                if (veryVeryVerbose) {
                    P_(j) P_(stime);    P_(utime);    P(wtime);
                }
            } // j
        } // i

        if (verbose) {
            P_(totalSTime); P_(totalUTime); P(totalWTime);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING
        //   void accumulatedTimes(double*, double*, double*) const;
        //
        //   Values loaded by the method should be close to those returned by
        //   the corresponding single-value method ('accumulatedSystemTime',
        //   'accumulatedUserTime' or 'accumulatedWallTime').
        //
        // Plan:
        //   Check everything is zero, start the timer, do some delay, check
        //   that the values returned by accumulatedTimes are close to those
        //   obtained from the above single-valued methods, stop the timer,
        //   check that now the values are exactly equal.
        //
        // Testing:
        //   void accumulatedTimes(double*, double*, double*) const;
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING 'accumulatedTimes'"
                          "\n==========================");

        double const delayTime     = 2;    //  2 seconds
        double const precision     = 1e-3; //  1 milliseconds
        double const finePrecision = 1e-5; // 10 microseconds

        // Note that the 'finePrecision' value is a heuristic meant to be
        // greater than the expected cumulative cost of: 'Stopwatch::start',
        // 'delayWall', and (notably) 'Stopwatch::accumulatedTimes'.  On some
        // older, heavily loaded systems the cost can be well over 1us (I'm
        // looking at you, SunOS).
        //
        // If the value of 'finePrecision' is less than the finest *practical*
        // resolution of the timers provided by 'bsls::TimeUtil', then the test
        // below effectively asserts that the cumulative cost of those
        // operations is too small to be observed.  Such an assertion is
        // vulnerable to improvements in the resolution of 'bsls::TimeUtil'.
        //
        // Earlier versions of this test driver used a 'finePrecision' that was
        // far too small, but the low resolution of 'bsls::TimeUtil' masked
        // that defect.  Now that 'bsls::TimeUtil' can actually detect
        // microsecond-level resource usages, 'finePrecision' has been
        // increased to express the expected cost of the operations.  Even so,
        // this test is fundamentally brittle.

        bsls::Stopwatch pt;

        ASSERT(0 == pt.accumulatedSystemTime());
        ASSERT(0 == pt.accumulatedUserTime());
        ASSERT(0 == pt.accumulatedWallTime());
        double st, ut, wt; pt.accumulatedTimes(&st, &ut, &wt);
        ASSERT(0 == st && 0 == ut && 0 == wt);

        pt.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
        delayWall(delayTime);

        double st1, ut1, wt1;
        pt.accumulatedTimes(&st1, &ut1, &wt1);
        double st2 = pt.accumulatedSystemTime();
        double ut2 = pt.accumulatedUserTime();
        double wt2 = pt.accumulatedWallTime();

        ASSERTV(st2 - st1, st1 + finePrecision > st2);
        ASSERTV(ut2 - ut1, ut1 + finePrecision > ut2);
        ASSERTV(wt1 +     precision > wt2);

        if (verbose) {
            P_(st1) P_(ut1) P(wt1)
            P_(st2) P_(ut2) P(wt2);
            P_(st2 - st1) P(ut2 - ut1)
        }

        pt.stop();

        double st3, ut3, wt3;
        pt.accumulatedTimes(&st3, &ut3, &wt3);
        double st4 = pt.accumulatedSystemTime();
        double ut4 = pt.accumulatedUserTime();
        double wt4 = pt.accumulatedWallTime();

        ASSERT(isEqual(st3, st4));
        ASSERT(isEqual(ut3, ut4));
        ASSERT(isEqual(wt3, wt4));

        if (verbose) {
            P_(st3) P_(ut3) P(wt3)
            P_(st4) P_(ut4) P(wt4);
            P_(st4 - st3) P(ut4 - ut3)
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING
        //   double accumulatedSystemTime() const;
        //   double accumulatedUserTime() const;
        //   double accumulatedWallTime() const;
        //
        //   Each of the above methods should return a plausible value when
        //   called from either the RUNNING or the STOPPED state.
        //
        // Plan:
        //   For a sequence of programmatically-generated delays (using timers
        //   from the tested 'bsls::TimeUtil' class), confirm that each of the
        //   above methods returns an expected value when invoked after the
        //   'start', 'stop', and' reset' methods.
        //
        // Testing:
        //   double accumulatedSystemTime() const;
        //   double accumulatedUserTime() const;
        //   double accumulatedWallTime() const;
        //   double elapsedTime() const;
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING accumulated time functions"
                          "\n==================================");

        for (size_t t = 0; t < NUM_TIME_METHODS; ++t) {
            const TimeMethods& METHODS     = TIME_METHODS[t];
            const char * const METHOD_NAME = METHODS.timeMethodName();

            u_VDBGPRINT("\nTesting '", METHOD_NAME, "'\n"
                                                    "=======================");

            u_VDBGPRINT("\tConfirm the value returned by '", METHOD_NAME,"'.");
            {
                const double TIME_STEP = 0.125;
                const int    NUM_STEPS = 5;

                Obj x;  const Obj& X = x;

                if (veryVerbose) puts("\tFrom the RUNNING state:");
                for (int i = 1; i <= NUM_STEPS; ++i) {
                    x.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
                    METHODS.delay(TIME_STEP);
                    const double elapsedTime = METHODS.accumulated(X);
                    x.stop();

                    if (veryVerbose) {
                        T_; T_; P_(t); P_(i * TIME_STEP); P(elapsedTime);
                    }

                    LOOP2_ASSERT(t, i, (i - 1) * TIME_STEP <  elapsedTime);
                    LOOP2_ASSERT(t, i, (i - 0) * TIME_STEP <= elapsedTime);

                    if (t != 2) {
                        LOOP2_ASSERT(t, i, (i + 1) * TIME_STEP >  elapsedTime);
                    }
                    // Not sure if the above check should be restored for wall
                    // time (t==2) even in any modified form: any given process
                    // can take longer than expected due to scheduling
                    // conditions.
                }

                if (veryVerbose) puts("\tAfter 'reset':");
                x.reset();
                if (veryVerbose) { T_  T_  P(METHODS.accumulated(X)); }
                ASSERT(0.0 == METHODS.accumulated(X));

                if (veryVerbose) puts("\tFrom the STOPPED state:");
                x.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
                for (int j = 1; j <= NUM_STEPS; ++j) {
                    METHODS.delay(TIME_STEP);
                    x.stop();
                    const double elapsedTime = METHODS.accumulated(X);

                    if (veryVerbose) {
                        T_;  T_;  P_(j * TIME_STEP);  P(elapsedTime);
                    }

                    LOOP2_ASSERT(t, j, (j - 1) * TIME_STEP <  elapsedTime);
                    LOOP2_ASSERT(t, j, (j - 0) * TIME_STEP <= elapsedTime);

                    if (t != 2) {
                        LOOP2_ASSERT(t, j, (j + 1) * TIME_STEP >  elapsedTime);
                    }
                    // Not sure if the above check should be restored for wall
                    // time (t==2) even in any modified form: any given process
                    // can take longer than expected due to scheduling
                    // conditions.

                    x.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'start', 'stop', and 'reset':
        //   Each of the 'accumulatedSystemTime', 'accumulatedUserTime',
        //   and 'accumulatedWallTime' methods should return increasing
        //   values when called repeatedly in the RUNNING state, constant
        //   values when called repeatedly in the STOPPED state, and 0.0 when
        //   called repeatedly after 'reset'.  Note that this tests the
        //   correct functioning of the 'start', 'stop', and 'reset'
        //   methods.
        //
        // Plan:
        //   For each of the manipulators 'start', 'stop', and 'reset',
        //   and for each of the accessors 'accumulatedSystemTime',
        //   'accumulatedUserTime', and 'accumulatedWallTime', call
        //   a manipulator, then, with a fixed delay before each call, call an
        //   accessor and confirm that the return value is consistent with the
        //   expected behavior of the manipulator under test.
        //
        // Testing:
        //   void start();
        //   void stop();
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) puts("\nTesting 'start', 'stop', and 'reset'"
                          "\n====================================");

        static const double DELAY_TIME = 0.05;

        if (verbose) puts("\nConfirm the behavior of the 'start' method");
        {
            for (size_t t = 0; t < NUM_TIME_METHODS; ++t) {
                const TimeMethods& METHODS     = TIME_METHODS[t];
                const char * const METHOD_NAME = METHODS.timeMethodName();

                Obj x;  const Obj& X = x;

                if (veryVerbose) { T_;  P(METHOD_NAME); }

                x.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
                METHODS.delay(DELAY_TIME);
                const double t1 = METHODS.accumulated(X);
                METHODS.delay(DELAY_TIME);
                const double t2 = METHODS.accumulated(X);
                METHODS.delay(DELAY_TIME);
                const double t3 = METHODS.accumulated(X);

                ASSERTV(METHOD_NAME, t1,     0.0 < t1);
                ASSERTV(METHOD_NAME, t1, t2,  t1 < t2);
                ASSERTV(METHOD_NAME, t2, t3,  t2 < t3);
            }
        }

        if (verbose) puts("\nConfirm behavior of the 'start(bool)' argument");
        {

            // Default does not collect user & system, 'false' is the default
            for (size_t t = 0; t < NUM_TIME_METHODS; ++t) {
                const TimeMethods& METHODS     = TIME_METHODS[t];
                const char * const METHOD_NAME = METHODS.timeMethodName();

                Obj x;  const Obj& X = x;  // 'start()'
                Obj y;  const Obj& Y = y;  // 'start(false)'

                if (veryVerbose) { T_;  P(METHOD_NAME); }

                x.start();  y.start(false);
                METHODS.delay(DELAY_TIME);
                const double tx1 = METHODS.accumulated(X);
                const double ty1 = METHODS.accumulated(Y);
                METHODS.delay(DELAY_TIME);
                const double tx2 = METHODS.accumulated(X);
                const double ty2 = METHODS.accumulated(Y);
                METHODS.delay(DELAY_TIME);
                const double tx3 = METHODS.accumulated(X);
                const double ty3 = METHODS.accumulated(Y);

                if (METHODS.isWallTime()) {  // Wall is time collected
                    ASSERTV(METHOD_NAME,      tx1, 0.0 < tx1);
                    ASSERTV(METHOD_NAME, tx1, tx2, tx1 < tx2);
                    ASSERTV(METHOD_NAME, tx2, tx3, tx2 < tx3);

                    ASSERTV(METHOD_NAME,      ty1, 0.0 < ty1);
                    ASSERTV(METHOD_NAME, ty1, ty2, ty1 < ty2);
                    ASSERTV(METHOD_NAME, ty2, ty3, ty2 < ty3);
                }
                else {                           // CPU times are not collected
                    ASSERTV(METHOD_NAME, tx1, 0.0 == tx1);
                    ASSERTV(METHOD_NAME, tx2, 0.0 == tx2);
                    ASSERTV(METHOD_NAME, tx3, 0.0 == tx3);

                    ASSERTV(METHOD_NAME, ty1, 0.0 == ty1);
                    ASSERTV(METHOD_NAME, ty2, 0.0 == ty2);
                    ASSERTV(METHOD_NAME, ty3, 0.0 == ty3);
                }
            }

            // 'k_COLLECT_WALL_TIME_ONLY' is identical to 'false'.
            ASSERT(false == Obj::k_COLLECT_WALL_TIME_ONLY);

            // 'start(true)' has been tested here earlier to collect all times.

            // 'k_COLLECT_WALL_AND_CPU_TIMES ' is identical to 'true'.
            ASSERT(true == Obj::k_COLLECT_WALL_AND_CPU_TIMES);
        }

        if (verbose) puts("\nConfirm the behavior of the 'stop' method");
        {
            for (size_t t = 0; t < NUM_TIME_METHODS; ++t) {
                const TimeMethods& METHODS     = TIME_METHODS[t];
                const char * const METHOD_NAME = METHODS.timeMethodName();

                Obj x;  const Obj& X = x;

                if (veryVerbose) { T_;  P(METHOD_NAME); }

                x.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
                METHODS.delay(DELAY_TIME);
                x.stop();
                const double t1 = METHODS.accumulated(X);
                METHODS.delay(DELAY_TIME);
                const double t2 = METHODS.accumulated(X);
                METHODS.delay(DELAY_TIME);
                const double t3 = METHODS.accumulated(X);

                ASSERTV(METHOD_NAME, t1,            0.0 < t1);
                ASSERTV(METHOD_NAME, t1, t2, isEqual(t1, t2));
                ASSERTV(METHOD_NAME, t2, t3, isEqual(t2, t3));
            }
        }

        if (verbose) puts("\nConfirm the behavior of the 'reset' method");
        {
            for (size_t t = 0; t < NUM_TIME_METHODS; ++t) {
                const TimeMethods& METHODS     = TIME_METHODS[t];
                const char * const METHOD_NAME = METHODS.timeMethodName();

                Obj x;  const Obj& X = x;

                if (veryVerbose) { T_;  P(METHOD_NAME); }

                x.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
                METHODS.delay(DELAY_TIME);
                x.reset();
                const double t1 = METHODS.accumulated(X);
                METHODS.delay(DELAY_TIME);
                const double t2 = METHODS.accumulated(X);
                METHODS.delay(DELAY_TIME);
                const double t3 = METHODS.accumulated(X);

                ASSERTV(METHOD_NAME, t1, 0.0 == t1);
                ASSERTV(METHOD_NAME, t2, 0.0 == t2);
                ASSERTV(METHOD_NAME, t3, 0.0 == t3);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // STATE TRANSITION TESTS FOR 'start', 'stop', and 'reset':
        //   The object should report the correct state after every possible
        //   transition as induced by 'start', 'stop', and 'reset'.
        //
        // Plan:
        //   Create a sequence of objects and systematically invoke the
        //   'start', 'stop', and 'reset' methods in all possible one-
        //   and two-step sequences, confirming the state with 'isRunning'.
        //
        // Testing:
        //   state transitions of:
        //     void start();
        //     void stop();
        //     void reset();
        //   bool isRunning();
        // --------------------------------------------------------------------

        if (verbose) puts("\nState Transition Tests"
                          "\n======================");

        if (verbose) puts("\nConfirm the object state after"
                           " 'start', 'stop', and 'reset'.");

        {
            Obj x;  const Obj& X = x;  ASSERT(false == X.isRunning());
            x.start();                 ASSERT(true  == X.isRunning());

            Obj y(x);  const Obj& Y = y;
            ASSERT(true == X.isRunning());
            ASSERT(true == Y.isRunning());

            y.stop();
            ASSERT(true  == X.isRunning());
            ASSERT(false == Y.isRunning());
        }

        {
            Obj x;  const Obj& X = x;  ASSERT(false == X.isRunning());
            x.stop();                  ASSERT(false == X.isRunning());

            x.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
            delayWall(0.2);
            delayUser(0.2);
            delaySystem(0.2);
            x.stop();

            Obj y(x);  const Obj& Y = y;
            ASSERT(false == X.isRunning());
            ASSERT(false == Y.isRunning());

            double v;

            v = X.accumulatedWallTime() - Y.accumulatedWallTime();
            ASSERT(-1.0e-15 <= v && v <= 1.0e-15);

            v = X.accumulatedUserTime() - Y.accumulatedUserTime();
            ASSERT(-1.0e-15 <= v && v <= 1.0e-15);

            v = X.accumulatedSystemTime() - Y.accumulatedSystemTime();
            ASSERT(-1.0e-15 <= v && v <= 1.0e-15);
        }

        {
            Obj x;  const Obj& X = x;  ASSERT(false == X.isRunning());
            x.reset();                 ASSERT(false == X.isRunning());
        }

        {
            Obj x;  const Obj& X = x;  ASSERT(false == X.isRunning());
            x.start();  x.start();     ASSERT(true  == X.isRunning());
            x.start();  x.stop();      ASSERT(false == X.isRunning());
            x.start();  x.reset();     ASSERT(false == X.isRunning());
        }

        {
            Obj x;  const Obj& X = x;  ASSERT(false == X.isRunning());
            x.stop();   x.start();     ASSERT(true  == X.isRunning());
            x.stop();   x.stop();      ASSERT(false == X.isRunning());
            x.stop();   x.reset();     ASSERT(false == X.isRunning());
        }

        {
            Obj x;  const Obj& X = x;  ASSERT(false == X.isRunning());
            x.reset();  x.start();     ASSERT(true  == X.isRunning());
            x.reset();  x.stop();      ASSERT(false == X.isRunning());
            x.reset();  x.reset();     ASSERT(false == X.isRunning());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   We want to exercise all of the basic functionality without being
        //   too systematic yet.
        //
        // Plan:
        //   Create objects using default constructors and exercise them using
        //   various methods.  Display object values frequently in verbose
        //   mode.
        //
        // Testing:
        //   This test *exercises* basic functionality.
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

        if (verbose) puts("\nCreate and exercise three objects.");

        Obj x1;  const Obj& X1 = x1;
        Obj x2;  const Obj& X2 = x2;
        Obj x3;  const Obj& X3 = x3;
        double t1, t2, t3;

        for (int i = 1; i <= 5; ++i) {
            x1.start();
            for (int j = 0; j < 2 * i; ++j) {
                x2.start();
                x3.start();
                i = i + 1;
                i = i - 1;
                x3.stop();
                x2.stop();
            }
            LOOP_ASSERT(i, true  == X1.isRunning());
            LOOP_ASSERT(i, false == X2.isRunning());
            LOOP_ASSERT(i, false == X3.isRunning());

            x1.stop();

            for (size_t t = 0; t < NUM_TIME_METHODS; ++t) {
                t1 = TIME_METHODS[t].accumulated(X1);
                t2 = TIME_METHODS[t].accumulated(X2);
                t3 = TIME_METHODS[t].accumulated(X3);

                if (verbose) {
                    T_;  P(i);  T_;  P_(t1);  P_(t2); P(t3);
                    T_;  P_(t1 - t2);  P(t2 - t3);
                }
            }
        }
        x1.reset();
        x2.reset();
        x3.reset();

        for (size_t t = 0; t < NUM_TIME_METHODS; ++t) {
            LOOP_ASSERT(t, 0.0 == TIME_METHODS[t].accumulated(X1));
            LOOP_ASSERT(t, 0.0 == TIME_METHODS[t].accumulated(X2));
            LOOP_ASSERT(t, 0.0 == TIME_METHODS[t].accumulated(X3));
        }

        ASSERT(false == X1.isRunning());
        ASSERT(false == X2.isRunning());
        ASSERT(false == X3.isRunning());

        Obj x4(X1);  const Obj& X4 = x4;

        ASSERT(false == X1.isRunning());
        ASSERT(false == X4.isRunning());

        double v;

        v = X1.accumulatedWallTime() - X4.accumulatedWallTime();
        ASSERT(-1.0e-15 <= v && v <= 1.0e-15);
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST I: USING WALL TIME TO MEASURE PERFORMANCE
        //
        // Concerns: The various system calls used by 'bsls::Stopwatch' can be
        //           expensive, so their costs should be measured.
        //
        // Plan:
        //   1)  Use a 'bsls::Stopwatch' (in wall time mode) to time a
        //       reference loop that calls a non-trivial function
        //       'busyFunction'.  The number of loop iterations is governed by
        //       'verbose'.
        //
        //   2a) Use a 'bsls::Stopwatch' to time the reference loop
        //       instrumented with a separate 'bsls::Stopwatch' instance
        //       started with the (less expensive) 'start' method and calling
        //       'elapsedTime'.
        //
        //
        //   2b) Use a 'bsls::Stopwatch' to time the reference loop
        //       instrumented with a separate 'bsls::Stopwatch' instance
        //       started with the (more expensive) 'start(true)' method and
        //       calling 'accumulatedTimes(double *, double *, double *)'.
        //
        //   3)  Subtract the result obtained in (1) from each of the results
        //       obtained in (2a) and (2b), and normalize by the number of
        //       loop iterations to obtain the per-loop cost of the
        //       instrumentation.
        //
        //   NOTE: Because the OS can suspend the loops at any point, the above
        //         tests are subject to an intrinsic uncertainty.  However, the
        //         wall-time mode converges *much* more quickly to a reasonable
        //         value than does the "correct" cpu-time mode.
        //
        // Testing:
        //   Performance of the two main usage modes of 'bsls::Stopwatch',
        //   using the wall-time mode to test.
        //
        // --------------------------------------------------------------------

        if (verbose) puts("\nProfiling 'start(false)': wall time only mode");

        const int    numTrials     = verbose ? atoi(argv[2]) : 10000;
        const double toNanoseconds = 1.0e9 / (double)numTrials;

        // Reference (baseline) loop -- using wall-time mode for timing
        bsls::Stopwatch baselineWatch;
        baselineWatch.start(Obj::k_COLLECT_WALL_TIME_ONLY);
        for (int i = 0; i < numTrials; ++i) {
            busyFunction();
        }
        baselineWatch.stop();
        const double baselineTime = baselineWatch.elapsedTime();

        {
            if (verbose) puts("\tProfiling the sequence\n"
                              "\t  1. 'start(true)'\n"
                              "\t  2. {delay}\n"
                              "\t  3. 'stop()'\n"
                              "\t  4. 'elapsedTime()'");
            bsls::Stopwatch watch;
            watch.start(Obj::k_COLLECT_WALL_TIME_ONLY);
            for (int i = 0; i < numTrials; ++i) {
                bsls::Stopwatch w;
                w.start(Obj::k_COLLECT_WALL_TIME_ONLY);
                busyFunction();
                w.stop();
                dblSink = dblSink + w.elapsedTime();
            }
            watch.stop();
            const double testTime = watch.elapsedTime();
            u_VDBGPRINT("\t  + Net cost per loop iteration is ",
                           (testTime - baselineTime) * toNanoseconds, " nsec");
        }

        {
            if (verbose) puts("\tProfiling the sequence\n"
                              "\t  1. 'start(true)'\n"
                              "\t  2. {delay}\n"
                              "\t  3. 'stop()'\n"
                              "\t  4. 'accumulatedTimes()'");
            bsls::Stopwatch watch;
            watch.start(Obj::k_COLLECT_WALL_TIME_ONLY);
            bsls::Stopwatch w;
            for (int i = 0; i < numTrials; ++i) {
                w.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
                busyFunction();
                w.stop();
                double d, u, s;
                w.accumulatedTimes(&d,&u,&s);
            }
            watch.stop();
            const double testTime = watch.elapsedTime();
            u_VDBGPRINT("\t  + Net cost per loop iteration is ",
                           (testTime - baselineTime) * toNanoseconds, " nsec");
        }
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST II: USING CPU + SYSTEM TIME TO MEASURE PERFORMANCE
        //
        // Concerns: The various system calls used by bsls::Stopwatch can be
        //           expensive, so their costs should be measured.
        //
        // Plan:
        //   1)  Use a 'bsls::Stopwatch' (in CPU time mode) to time a reference
        //       loop that calls a non-trivial function 'busyFunction'.  The
        //       number of loop iterations is governed by 'verbose'.
        //
        //   2a) Use a 'bsls::Stopwatch' to time the reference loop
        //       instrumented with a separate 'bsls::Stopwatch' instance
        //       started with the (less expensive) 'start' method and calling
        //       'elapsedTime'.
        //
        //   2b) Use a 'bsls::Stopwatch' to time the reference loop
        //       instrumented with a separate 'bsls::Stopwatch' instance
        //       started with the (more expensive) 'start(true)' method and
        //       calling 'accumulatedTimes(double *, double *, double *)'.
        //
        //   3)  Subtract the result obtained in (1) from each of the results
        //       obtained in (2a) and (2b), and normalize by the number of
        //       loop iterations to obtain the per-loop cost of the
        //       instrumentation.
        //
        //   NOTE: Because of the lower precision of the CPU-time mode, this
        //         benchmark requires a much larger number of loop iterations
        //         to converge to an accurate answer than does the wall-time
        //         mode.
        //
        // Testing:
        //   Performance of the two main usage modes of 'bsls::Stopwatch',
        //   using the CPU-time mode to test.
        //
        // --------------------------------------------------------------------

        if (verbose) puts("\nProfiling 'start(true)': wall & CPU time mode");

        const int    numTrials     = verbose ? atoi(argv[2]) : 10000;
        const double toNanoseconds = 1.0e9 / (double)numTrials;

        // Reference (baseline) loop --  using CPU-time mode for timing
        bsls::Stopwatch baselineWatch;
        baselineWatch.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
        for (int i = 0; i < numTrials; ++i) {
            busyFunction();
        }
        baselineWatch.stop();
        const double baselineTimeSys  = baselineWatch.accumulatedSystemTime();
        const double baselineTimeUser = baselineWatch.accumulatedUserTime();
        const double baselineTime     = baselineTimeSys + baselineTimeUser;

        u_VVDBGPREFPRINT("\tTotal reference time = ", baselineTime);

        {
            puts("\tProfiling the sequence\n"
                 "\t  1. 'start(true)'\n"
                 "\t  2. {delay}\n"
                 "\t  3. 'stop()'\n"
                 "\t  4. 'elapsedTime()'");
            bsls::Stopwatch testWatch;
            testWatch.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
            for (int i = 0; i < numTrials; ++i) {
                bsls::Stopwatch w;
                w.start();
                busyFunction();
                w.stop();
                dblSink = dblSink + w.elapsedTime();
            }
            testWatch.stop();
            const double testTimeSys  = testWatch.accumulatedSystemTime();
            const double testTimeUser = testWatch.accumulatedUserTime();
            const double testTime     = testTimeSys + testTimeUser;

            u_VVDBGPREFPRINT("\tTotal test time = ", testTime);
            u_DBGPRINT("\t  + Net cost per loop iteration is ",
                           (testTime - baselineTime) * toNanoseconds, " nsec");
        }
        {
            puts("\tProfiling the sequence\n"
                 "\t  1. 'start(true)'\n"
                 "\t  2. {delay}\n"
                 "\t  3. 'stop()'\n"
                 "\t  4. 'accumulatedTimes()'");
            bsls::Stopwatch testWatch;
            testWatch.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
            bsls::Stopwatch w;
            for (int i = 0; i < numTrials; ++i) {
                w.start(Obj::k_COLLECT_WALL_AND_CPU_TIMES);
                busyFunction();
                w.stop();
                double d, u, s;
                w.accumulatedTimes(&d,&u,&s);
            }
            testWatch.stop();
            const double testTimeSys  = testWatch.accumulatedSystemTime();
            const double testTimeUser = testWatch.accumulatedUserTime();
            const double testTime     = testTimeSys + testTimeUser;
            u_VVDBGPREFPRINT("\tTotal test time = ", testTime);
            u_DBGPRINT("\t  + Net cost per loop iteration is ",
                           (testTime - baselineTime) * toNanoseconds, " nsec");
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
// Copyright 2016 Bloomberg Finance L.P.
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
