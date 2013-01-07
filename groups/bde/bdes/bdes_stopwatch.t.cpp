// bdes_stopwatch.t.cpp         -*-C++-*-

#include <bdes_stopwatch.h>

#include <bdes_timeutil.h>   // getTimer() (used in delay generation)
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>            // printf()
#include <bsl_cstdlib.h>           // atoi()
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

// The following OS-dependent includes are for the 'osSystemCall' helper
// function below.

#if defined BSLS_PLATFORM_OS_UNIX
    #include <sys/times.h>
    #include <unistd.h>
#elif defined BSLS_PLATFORM_OS_WINDOWS
    #include <windows.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test has state, but not value.  State transitions must
// be confirmed explicitly.  The timing functions can be verified to a
// reasonable degree using the 'bdes_TimeUtil::get<...>Timer()' functions
// (with which the stopwatch class is implemented) to confirm "reasonable"
// behavior.
//-----------------------------------------------------------------------------
// [ 2] bdes_Stopwatch();
// [ 2] ~bdes_Stopwatch();
// [ 3] void start();
// [ 3] void stop();
// [ 3] void reset();
// [ 2] int isRunning() const;
// [ 4] double accumulatedSystemTime() const;
// [ 4] double accumulatedUserTime() const;
// [ 4] double accumulatedWallTime() const;
// [ 5] void accumulatedTimes(double*, double*, double*) const;
// [ 4] double elapsedTime() const;
//-----------------------------------------------------------------------------
// [ 1] Breathing Test
// [ 2] State Transitions
// [ 6] USAGE Example
//-----------------------------------------------------------------------------

//=============================================================================
//                         STANDARD BDE ASSERT TEST MACRO
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

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);} }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdes_Stopwatch    Obj;

typedef bdes_TimeUtil     TU;
typedef bsls_Types::Int64 Int64;

typedef double (Obj::*AccumulatedTimeMethod)() const;

typedef Int64 (*RawTimerFunction)();
typedef Int64 (*LoopBodyFunction)(RawTimerFunction timerFn);

//=============================================================================
//                             HELPER FUNCTIONS
//-----------------------------------------------------------------------------

Int64 emptyFunction(RawTimerFunction timerFn)
{
    return timerFn();
}

void busyFunction()
{
    for (int i = 0; i < 10; ++i) {
        for (int j  = 0;j < 10; ++j) {
            int k = i*j+ j*i;
        }
    }
}

// Do some work here, as we need to consume user time.  The more we do the
// better, so we are intentionally inefficient.

int array[17] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
};

Int64 osUserCall(RawTimerFunction timerFn)
{
    for (int k = 0; k < 100; ++k) {
        for (int i = 0; i < 17-1; ++i) {
            for (int j = i+1; j < 17; ++j) {
                if (   k%2 == 0 && array[i] < array[j]++
                    || k%2 != 0 && array[i] > array[j]++) {
                    bsl::swap(array[i], array[j]);
                }
            }
        }
        bsl::cout.flush();
    }
    return timerFn();
}

Int64 osSystemCall(RawTimerFunction timerFn)
{
#if defined BSLS_PLATFORM_OS_UNIX
    pid_t p = getpid();
    struct tms tt;
    clock_t c = times(&tt);
#elif defined BSLS_PLATFORM_OS_WINDOWS
    HANDLE ph = ::GetCurrentProcess();
    FILETIME crtnTm, exitTm, krnlTm, userTm;
    ::GetProcessTimes(ph, &crtnTm, &exitTm, &krnlTm, &userTm);
#endif
    return timerFn();
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

//=============================================================================
//                              TEST DATA
//-----------------------------------------------------------------------------

// Converted from struct to object initialization to work around an aCC bug
// on HP/UX -- mgiroux, 20091207.
static struct TimeMethod {
    AccumulatedTimeMethod d_method;
    Int64 (*d_delayFunction)(double);
    bsl::string d_methodName;

    TimeMethod(AccumulatedTimeMethod a,
               Int64(*b)(double),
               const char *c)
         : d_method(a)
         , d_delayFunction(b)
         , d_methodName(c)
    {
    }
} const TimeMethods[] = {
    TimeMethod(&Obj::accumulatedSystemTime,
               &delaySystem,
               "accumulatedSystemTime"),
    TimeMethod(&Obj::accumulatedUserTime,
               &delayUser,
               "accumulatedUserTime"  ),
    TimeMethod(&Obj::accumulatedWallTime,
               &delayWall,
               "accumulatedWallTime"  )
};

static size_t const TimeMethodsCount =
                                      sizeof TimeMethods / sizeof *TimeMethods;

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
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

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        bdes_Stopwatch s;
        const double t0s = s.accumulatedSystemTime();  ASSERT(0.0 == t0s);
        const double t0u = s.accumulatedUserTime();    ASSERT(0.0 == t0u);
        const double t0w = s.accumulatedWallTime();    ASSERT(0.0 == t0w);

        s.start();
        const double t1s = s.accumulatedSystemTime();  ASSERT(0.0 == t1s);
        const double t1u = s.accumulatedUserTime();    ASSERT(0.0 == t1u);
        const double t1w = s.accumulatedWallTime();    ASSERT(0.0 <= t1w);

        s.stop();
        const double t2s = s.accumulatedSystemTime();  ASSERT(t1s == t2s);
        const double t2u = s.accumulatedUserTime();    ASSERT(t1u == t2u);
        const double t2w = s.accumulatedWallTime();    ASSERT(t1w <= t2w);

        s.start(true);
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
      case 5: {
        // --------------------------------------------------------------------
        // TESTING
        //   void accumulatedTimes(double*, double*, double*) const;
        //
        //   Values loaded by the method should be close to those returned by
        //   the corresponding single-value method ('accumulatedSystemTime()',
        //   'accumulatedUserTime()' or 'accumulatedWallTime()').
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

        double const delayTime = 2; // seconds
        double const precision = 1e-3; // 1 msec
        double const finePrecision = 1e-8; // 10 nsecs

        bdes_Stopwatch pt;

        ASSERT(0 == pt.accumulatedSystemTime());
        ASSERT(0 == pt.accumulatedUserTime());
        ASSERT(0 == pt.accumulatedWallTime());
        double st, ut, wt; pt.accumulatedTimes(&st, &ut, &wt);
        ASSERT(0 == st && 0 == ut && 0 == wt);

        pt.start(true);
        delayWall(delayTime);

        double st1, ut1, wt1;
        pt.accumulatedTimes(&st1, &ut1, &wt1);
        double st2 = pt.accumulatedSystemTime();
        double ut2 = pt.accumulatedUserTime();
        double wt2 = pt.accumulatedWallTime();

        ASSERT(st1 + finePrecision > st2);
        ASSERT(ut1 + finePrecision > ut2);
        ASSERT(wt1 +     precision > wt2);

        if (verbose) cout << st1 << ", " << ut1 << ", " << wt1 << bsl::endl
                          << st2 << ", " << ut2 << ", " << wt2 << bsl::endl;

        pt.stop();

        double st3, ut3, wt3;
        pt.accumulatedTimes(&st3, &ut3, &wt3);
        double st4 = pt.accumulatedSystemTime();
        double ut4 = pt.accumulatedUserTime();
        double wt4 = pt.accumulatedWallTime();

        ASSERT(st3 == st4);
        ASSERT(ut3 == ut4);
        ASSERT(wt3 == wt4);

        if (verbose) cout << st3 << ", " << ut3 << ", " << wt3 << bsl::endl
                          << st4 << ", " << ut4 << ", " << wt4 << bsl::endl;

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
        //   from the tested 'bdes_TimeUtil' class), confirm that each of the
        //   above methods returns an expected value when invoked after the
        //   'start()', 'stop()', and' reset()' methods.
        //
        // Testing:
        //   double accumulatedSystemTime() const;
        //   double accumulatedUserTime() const;
        //   double accumulatedWallTime() const;
        //   double elapsedTime() const;
        // --------------------------------------------------------------------

        for (size_t t = 0; t < TimeMethodsCount; ++t) {
            if (verbose) cout << "\nTesting '"
                              << TimeMethods[t].d_methodName
                              << "'"
                              << "\n=======================" << endl;

            if (verbose) cout << "\nConfirm the value returned by"
                              << "'"
                              << TimeMethods[t].d_methodName
                              << "'." << endl;
            {
                const double TIME_STEP = 0.20;
                const int    NUM_STEPS = 5;

                Obj x;  const Obj& X = x;

                if (verbose) cout << "\tFrom the RUNNING state:" << endl;
                for (int i = 1; i <= NUM_STEPS; ++i) {
                    x.start(true);
                    TimeMethods[t].d_delayFunction(TIME_STEP);
                    const double elapsedTime =
                        (X.*(TimeMethods[t].d_method))();
                    x.stop();

                    if (veryVerbose) {
                        T_(); T_(); P_(t); P_(i * TIME_STEP); P(elapsedTime);
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

                if (verbose) cout << "\tAfter 'reset()':" << endl;
                x.reset();
                if (veryVerbose)
                    { T_();  T_();  P((X.*(TimeMethods[t].d_method))()); }
                ASSERT(0.0 == (X.*(TimeMethods[t].d_method))());

                if (verbose) cout << "\tFrom the STOPPED state:" << endl;
                x.start(true);
                for (int j = 1; j <= NUM_STEPS; ++j) {
                    TimeMethods[t].d_delayFunction(TIME_STEP);
                    x.stop();
                    const double elapsedTime =
                        (X.*(TimeMethods[t].d_method))();

                    if (veryVerbose) {
                        T_();  T_();  P_(j * TIME_STEP);  P(elapsedTime);
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

                    x.start(true);
                }
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'start()', 'stop()', and 'reset()':
        //   Each of the 'accumulatedSystemTime()', 'accumulatedUserTime()',
        //   and 'accumulatedWallTime()' methods should return increasing
        //   values when called repeatedly in the RUNNING state, constant
        //   values when called repeatedly in the STOPPED state, and 0.0 when
        //   called repeatedly after 'reset()'.  Note that this tests the
        //   correct functioning of the 'start()', 'stop()', and 'reset()'
        //   methods.
        //
        // Plan:
        //   For each of the manipulators 'start()', 'stop()', and 'reset()',
        //   and for each of the accessors 'accumulatedSystemTime()',
        //   'accumulatedUserTime()', and 'accumulatedWallTime()', call
        //   a manipulator, then, with a fixed delay before each call, call an
        //   accessor and confirm that the return value is consistent with the
        //   expected behavior of the manipulator under test.
        //
        // Testing:
        //   void start();
        //   void stop();
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'start()', 'stop()', and 'reset()'"
                          << "\n=========================================="
                          << endl;

        const double DELAY_TIME = 0.05;

        if (verbose) cout << "\nConfirm the behavior of 'start()" << endl;
        {
            for (size_t t = 0; t < TimeMethodsCount; ++t) {
                Obj x;  const Obj& X = x;

                x.start(true);
                TimeMethods[t].d_delayFunction(DELAY_TIME);
                const double t1 = (X.*(TimeMethods[t].d_method))();
                TimeMethods[t].d_delayFunction(DELAY_TIME);
                const double t2 = (X.*(TimeMethods[t].d_method))();
                TimeMethods[t].d_delayFunction(DELAY_TIME);
                const double t3 = (X.*(TimeMethods[t].d_method))();

                if (veryVerbose) { T_();  P_(t1);  P_(t2);  P(t3); }
                LOOP_ASSERT(t,  0 < t1);
                LOOP_ASSERT(t, t1 < t2);
                LOOP_ASSERT(t, t2 < t3);
            }
        }

        if (verbose) cout << "\nConfirm the behavior of 'stop()" << endl;
        {
            for (size_t t = 0; t < TimeMethodsCount; ++t) {
                Obj x;  const Obj& X = x;

                x.start(true);
                TimeMethods[t].d_delayFunction(DELAY_TIME);
                x.stop();
                const double t1 = (X.*(TimeMethods[t].d_method))();
                TimeMethods[t].d_delayFunction(DELAY_TIME);
                const double t2 = (X.*(TimeMethods[t].d_method))();
                TimeMethods[t].d_delayFunction(DELAY_TIME);
                const double t3 = (X.*(TimeMethods[t].d_method))();

                if (veryVerbose) { T_();  P_(t1);  P_(t2);  P(t3); }
                LOOP_ASSERT(t,  0 <  t1);
                LOOP_ASSERT(t, t1 == t2);
                LOOP_ASSERT(t, t2 == t3);
            }
        }

        if (verbose) cout << "\nConfirm the behavior of 'reset()" << endl;
        {
            for (size_t t = 0; t < TimeMethodsCount; ++t) {
                Obj x;  const Obj& X = x;

                x.start(true);
                TimeMethods[t].d_delayFunction(DELAY_TIME);
                x.reset();
                const double t1 = (X.*(TimeMethods[t].d_method))();
                TimeMethods[t].d_delayFunction(DELAY_TIME);
                const double t2 = (X.*(TimeMethods[t].d_method))();
                TimeMethods[t].d_delayFunction(DELAY_TIME);
                const double t3 = (X.*(TimeMethods[t].d_method))();

                if (veryVerbose) { T_();  P_(t1);  P_(t2);  P(t3); }
                LOOP_ASSERT(t, 0 == t1);
                LOOP_ASSERT(t, 0 == t2);
                LOOP_ASSERT(t, 0 == t3);
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // STATE TRANSITION TESTS FOR 'start()', 'stop()', and 'reset()':
        //   The object should report the correct state after every possible
        //   transition as induced by 'start()', 'stop()', and 'reset()'.
        //
        // Plan:
        //   Create a sequence of objects and systematically invoke the
        //   'start()', 'stop()', and 'reset()' methods in all possible one-
        //   and two-step sequences, confirming the state with 'isRunning()'.
        //
        // Testing:
        //   state transitions of:
        //     void start();
        //     void stop();
        //     void reset();
        //   int isRunning();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nState Transition Tests"
                          << "\n======================" << endl;

        if (verbose) cout << "\nConfirm the object state after"
                             "'start()', 'stop()', and 'reset()'." << endl;
        {
            Obj x;  const Obj& X = x;  ASSERT(false == X.isRunning());
            x.start();                 ASSERT(true  == X.isRunning());
        }

        {
            Obj x;  const Obj& X = x;  ASSERT(false == X.isRunning());
            x.stop();                  ASSERT(false == X.isRunning());
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

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        if (verbose) cout << "\nCreate and exercise three objects." << endl;

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

            for (size_t t = 0; t < TimeMethodsCount; ++t) {
                t1 = (X1.*(TimeMethods[t].d_method))();
                t2 = (X2.*(TimeMethods[t].d_method))();
                t3 = (X3.*(TimeMethods[t].d_method))();

                if (verbose) {
                    T_();  P(i);  T_();  P_(t1);  P_(t2); P(t3);
                    T_();  P_(t1 - t2);  P(t2 - t3);
                }
            }
        }
        x1.reset();
        x2.reset();
        x3.reset();

        for (size_t t = 0; t < TimeMethodsCount; ++t) {
            LOOP_ASSERT(t, 0.0 == (X1.*(TimeMethods[t].d_method))());
            LOOP_ASSERT(t, 0.0 == (X2.*(TimeMethods[t].d_method))());
            LOOP_ASSERT(t, 0.0 == (X3.*(TimeMethods[t].d_method))());
        }

        ASSERT(false == X1.isRunning());
        ASSERT(false == X2.isRunning());
        ASSERT(false == X3.isRunning());

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        enum { NUM_MEASUREMENTS = 10000000 };
        {
            cout << "Measuring using processTimer: ";
            bdes_Stopwatch watch;
            watch.start();
            for (int i = 0; i < NUM_MEASUREMENTS; ++i) {
                bdes_Stopwatch w;
                w.start();
                busyFunction();
                w.stop();
                double d = w.elapsedTime();
            }
            watch.stop();
            cout << watch.elapsedTime() << bsl::endl;
        }
        {
            cout << "Measuring using processTimer with cpu times: ";
            bdes_Stopwatch watch;
            watch.start();
            for (int i = 0; i < NUM_MEASUREMENTS; ++i) {
                bdes_Stopwatch w;
                w.start(true);
                busyFunction();
                w.stop();
                double d,u,s;
                w.accumulatedTimes(&d,&u,&s);
            }
            watch.stop();
            cout << watch.elapsedTime() << bsl::endl;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
