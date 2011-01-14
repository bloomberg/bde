// bsls_timeutil.t.cpp                                                -*-C++-*-

#include <bsls_timeutil.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#ifdef BSLS_PLATFORM__OS_UNIX
    #include <time.h>      // NOTE: <ctime> conflicts with <sys/time.h>
    #include <sys/time.h>  // 'gethrtime()'
    #include <unistd.h>    // 'sleep'
#ifdef BSLS_PLATFORM__OS_SOLARIS   // Solaris OR late SunOS!
    #include <limits.h>    // 'CLK_TCK', for Sun (on FreeBSD, in <sys/time.h>)
#endif
#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS
typedef unsigned long DWORD;

extern "C" {
    __declspec(dllimport) void __stdcall Sleep(DWORD dwMilliseconds);
};
#endif

#include <cstdio>                   // printf()
#include <cstdlib>                  // atoi()
#include <iostream>
#include <iomanip>
#include <string>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This utility component has at present only four static functions (one being
// just an alias to one of the other three), which are a timer functions based
// on a platform-dependent system clocks.
// It is not possible to completely test a system-dependent function, but we
// address basic concerns to probe both our own code for consistent behavior
// and the system results for plausible correct behavior.
//-----------------------------------------------------------------------------
// [11] bsls_Types::Int64 convertRawTime(OpaqueNativeTime rawTime);
// [ 1] bsls_Types::Int64 bsls_TimeUtil::getProcessSystemTimer();
// [ 1] void bsls_TimeUtil::getProcessTimers(bsls_Types::Int64);
// [ 1] bsls_Types::Int64 bsls_TimeUtil::getTimer();
// [ 1] bsls_Types::Int64 bsls_TimeUtil::getProcessUserTimer();
// [11] OpaqueNativeTime getTimerRaw();
//-----------------------------------------------------------------------------
// [XX] Breathing Test -- NOT IMPLEMENTED
// [ 2] USAGE
// [ 3] Performance Test
// [ 4] Test for unique, monotonically increasing return values (statistical)
// [ 5] Test correct hooking of methods to underlying OS APIs (approximately)
// [ 6] Test of 'gethrtime()' (Sun and HP only -- statistical)
// [ 7] Initialization test: getProcessSystemTimer (UNIX only)
// [ 8] Initialization test: getProcessUserTimer (UNIX only)
// [ 9] Initialization test: getProcessTimers (UNIX only)
// [10] Initialization test: getTimer (Windows only)
//-----------------------------------------------------------------------------

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
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

//=============================================================================

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                 GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls_TimeUtil     TU;
typedef bsls_Types::Int64 Int64;

typedef Int64 (*TimerMethod) ();

//=============================================================================
//                              HELPER FUNCTIONS
//-----------------------------------------------------------------------------

static Int64 callGetProcessTimersRetSystem() {
    Int64 cTimer;
    Int64 uTimer;
    TU::getProcessTimers(&cTimer, &uTimer);
    return cTimer;
}

static Int64 callGetProcessTimersRetUser() {
    Int64 cTimer;
    Int64 uTimer;
    TU::getProcessTimers(&cTimer, &uTimer);
    return uTimer;
}

static void osSleep(unsigned seconds) {
#if defined(BSLS_PLATFORM__OS_UNIX)
    for(;;) {
        int t = sleep(seconds);
        if (t <= 0) {
            break;
        }
        seconds = t;
    }
#elif defined(BSLS_PLATFORM__OS_WINDOWS)
    Sleep(seconds*1000);  // milliseconds
#else
    #error "Do not know how to sleep on this platform"
#endif
}

//=============================================================================
//                             USAGE EXAMPLE CODE
//-----------------------------------------------------------------------------

class my_Timer {
    // This class implements a simple interval timer that is created in
    // the "running" state, and may be queried for its cumulative
    // interval (as a 'double', in seconds) but never stopped or reset.

    bsls_Types::Int64 d_startWallTime; // time at creation (nsec)
    bsls_Types::Int64 d_startUserTime; // time at creation (nsec)
    bsls_Types::Int64 d_startSystemTime; // time at creation (nsec)

  public:
    // CREATORS
    my_Timer() {
        d_startWallTime = bsls_TimeUtil::getTimer();
        d_startUserTime = bsls_TimeUtil::getProcessUserTimer();
        d_startSystemTime = bsls_TimeUtil::getProcessSystemTimer();
    }
        // Create a timer object initialized with the times at creation.
        // All values returned by subsequent calls to 'elapsed<...>Time()'
        // are with respect to this creation time.

    ~my_Timer() {};

    // ACCESSORS
    double elapsedWallTime();
        // Return the total elapsed time in seconds since the creation of
        // this timer object.
    double elapsedUserTime();
        // Return the elapsed user time in seconds since the creation of
        // this timer object.
    double elapsedSystemTime();
        // Return the elapsed system time in seconds since the creation of
        // this timer object.
};

inline
double my_Timer::elapsedWallTime()
{
    return (double) (bsls_TimeUtil::getTimer() - d_startWallTime) * 1.0E-9;
}

inline
double my_Timer::elapsedUserTime()
{
    return (double) (bsls_TimeUtil::getProcessUserTimer() - d_startUserTime)
                                                                      * 1.0E-9;
}

inline
double my_Timer::elapsedSystemTime()
{
    return (double) (bsls_TimeUtil::getProcessSystemTimer()
                                                 - d_startSystemTime) * 1.0E-9;
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING getTimerRaw() and convertRawTime()
        //
        // Concerns:
        //   The return values from two successive calls to 'getTimerRaw()'
        //   separated by zero or more simple operations, converted to
        //   nanoseconds and subtracted, should produce non-negative results.
        //   The concern of "reasonable" results is difficult to program and
        //   for now will be observed manually.
        //
        // Plan:
        //   Construct blocks containing two calls to 'getTimerRaw()'
        //   bracketing various non-trivial (i.e., non-removable by an
        //   optimizer) statements.  ASSERT that the differences of the
        //   converted return values are always non-negative.  In verbose mode,
        //   print the elapsed times so that they can be observed to be
        //   "reasonable"
        //
        // Testing:
        //   bsls_TimeUtil::getTimerRaw()
        //   bsls_TimeUtil::convertRawTime(bsls_TimeUtil::OpaqueNativeTime)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Raw methods"
                          << "\n===================" << endl;

        {
            // ---------------------------------------------------------------
            // This "warmup" section added to explore strange initial results
            // on Sun in particular, where the first block, timing one
            // division, was reproducibly slower than the same block but with
            // two divisions.
            TU::OpaqueNativeTime idle;
            TU::getTimerRaw(&idle);
            double               primeThePump = 12345/67 / 8.901;
            if (veryVeryVerbose)
                std::cout << "Warmup: " << primeThePump << std::endl;
            // ---------------------------------------------------------------
        }

        {
            TU::OpaqueNativeTime t1, t2;
            Int64                dT;

            TU::getTimerRaw(&t1);
            TU::getTimerRaw(&t2);
            dT = TU::convertRawTime(t2) - TU::convertRawTime(t1);
            ASSERT(dT >= 0);

            if (verbose) std::cout << "0: Elapsed time under test: " << dT
                                   << " nsec" << std::endl;
        }

        {
            TU::OpaqueNativeTime t1, t2;
            Int64                dT;
            double               x;

            TU::getTimerRaw(&t1);
            x  = argc * 355.0 / 113.0;
            TU::getTimerRaw(&t2);
            dT = TU::convertRawTime(t2) - TU::convertRawTime(t1);
            ASSERT(dT >= 0);

            if (verbose) std::cout << "1: Elapsed time under test: " << dT
                                   << " nsec" << std::endl;
            if (veryVerbose) std::cout << "Computed Values:"
                                       << x << std::endl;
        }

        {
            TU::OpaqueNativeTime t1, t2;
            Int64                dT;
            double               x, y;

            TU::getTimerRaw(&t1);
            x = argc * 355.2 / 113.3;
            y = argc * x / 12.0;
            TU::getTimerRaw(&t2);
            dT = TU::convertRawTime(t2) - TU::convertRawTime(t1);
            ASSERT(dT >= 0);

            if (verbose) std::cout << "2: Elapsed time under test: " << dT
                                   << " nsec" << std::endl;
            if (veryVerbose) std::cout << "Computed Values:"
                                       << x << "  " << y << std::endl;
        }

        {
            TU::OpaqueNativeTime t1, t2;
            Int64                dT;
            double               x, y, z;

            TU::getTimerRaw(&t1);
            x = argc * 355.1 / 113.7;
            y = (x * x) / (argc * 12.2);
            z = (x + 3) / (x * y + 4) + 2.0 * x;
            TU::getTimerRaw(&t2);

            dT = TU::convertRawTime(t2) - TU::convertRawTime(t1);
            ASSERT(dT >= 0);

            if (verbose) std::cout << "3: Elapsed time under test: " << dT
                                   << " nsec" << std::endl;
            if (veryVerbose) std::cout << "Computed Values:" << x << "  "
                                       << y << "  " << z << std::endl;
        }

        {
            TU::OpaqueNativeTime t1, t2;
            Int64                dT;
            double               x, y, z;

            TU::getTimerRaw(&t1);
            x = argc * 355.1 / 113.7;
            for (int i = 0; i < 10; ++i) {
                y = (x * 1.2) / 12.2;
                z = (x + 3) / (x * y + 4) + 2.0 * x;
                x = y + z;
            }
            TU::getTimerRaw(&t2);
            dT = TU::convertRawTime(t2) - TU::convertRawTime(t1);
            ASSERT(dT >= 0);

            if (verbose) std::cout << "10: Elapsed time under test: " << dT
                                   << " nsec" << std::endl;
            if (veryVerbose) std::cout << "Computed Values:" << x << "  "
                                       << y << "  " << z << std::endl;
        }

        {
            TU::OpaqueNativeTime t1, t2;
            Int64                dT;
            double               x, y, z;

            TU::getTimerRaw(&t1);
            x = argc * 355.1 / 113.7;
            for (int i = 0; i < 100; ++i) {
                y = (x * 1.2) / 12.2;
                z = (x + 3) / (x * y + 4) + 2.0 * x;
                x = y + z;
            }
            TU::getTimerRaw(&t2);
            dT = TU::convertRawTime(t2) - TU::convertRawTime(t1);
            ASSERT(dT >= 0);

            if (verbose) std::cout << "100: Elapsed time under test: " << dT
                                   << " nsec" << std::endl;
            if (veryVerbose) std::cout << "Computed Values:" << x << "  "
                                       << y << "  " << z << std::endl;
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // INITIALIZATION TEST
        //   *** Windows only ***: bsls_TimeUtil::getTimer()
        //
        // Plan:
        //   Call the method and check that the return value is not negative
        //   (the uninitialized values for s_initialTime and s_timerFrequency
        //   are -1).
        //
        // Testing:
        //   bsls_TimeUtil::getTimer()
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_WINDOWS
        Int64 t = TU::getTimer();
        if (verbose) { T_(); P64_(t); }
        ASSERT(t >= 0);
#endif

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // INITIALIZATION TEST
        //   *** UNIX only ***: bsls_TimeUtil::getProcessTimers()
        //
        // Plan:
        //   Call the method and check that the values returned are not
        //   negative (the uninitialized factor value is -1).
        //
        // Testing:
        //   bsls_TimeUtil::getProcessTimers()
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_UNIX
        Int64 ts, tu; TU::getProcessTimers(&ts, &tu);
        if (verbose) { T_(); P64_(ts); P64_(tu); }
        ASSERT(ts >= 0); ASSERT(tu >= 0);
#endif

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // INITIALIZATION TEST
        //   *** UNIX only ***: bsls_TimeUtil::getProcessUserTimer()
        //
        // Plan:
        //   Call the method and check that the return value is not negative
        //   (the uninitialized factor value is -1).
        //
        // Testing:
        //   bsls_TimeUtil::getProcessUserTimer()
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_UNIX
        Int64 t = TU::getProcessUserTimer();
        if (verbose) { T_(); P64_(t); }
        ASSERT(t >= 0);
#endif

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // INITIALIZATION TEST
        //   *** UNIX only ***: bsls_TimeUtil::getProcessSystemTimer()
        //
        // Plan:
        //   Call the method and check that the return value is not negative
        //   (the uninitialized factor value is -1).
        //
        // Testing:
        //   bsls_TimeUtil::getProcessSystemTimer()
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_UNIX
        Int64 t = TU::getProcessSystemTimer();
        if (verbose) { T_(); P64_(t); }
        ASSERT(t >= 0);
#endif

      } break;
      case 6: {
#if defined (BSLS_PLATFORM__OS_SOLARIS) || defined (BSLS_PLATFORM__OS_HPUX)
        // --------------------------------------------------------------------
        // PERFORMANCE TEST 'gethrtime()' *** Sun and HP ONLY ***
        //   Test whether successive calls ever return non-increasing values.
        //
        // Plan:
        //   Call 'gethrtime()' two times within a loop, and compare the return
        //   values each time.
        //
        // Testing:
        //   'gethrtime()'
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting 'gethrtime()' statistical correctness"
                 << "\n=============================================" << endl;

        if (verbose)
            cout << "\nCall 'gethrtime()' twice in a large loop."
                 << endl;
        {
            const Int64 NUM_TESTS = verbose ? 100LL * atoi(argv[2]) : 100;

            const Int64 t0 = TU::getTimer();
            Int64       t1, t2;
            int         numSame  = 0;
            int         numWrong = 0;

            for (Int64 i = 1; i < NUM_TESTS; ++i) {
                t1 = (Int64) gethrtime();
                t2 = (Int64) gethrtime();
                // Int64 dt = t2 - t1;  // these lines retained for
                // P64(dt);             //  non-production testing.
                if (t2 <= t1) {  // Optimize for CORRECT behavior
                    if (t2 == t1) {
                        ++numSame;
                        if (verbose) { T_();  P_(i);  P(numSame); }
                    }
                    else {
                        ++numWrong;
                        if (verbose) {
                            T_();      P64_(i);   P_(numWrong);
                            P64_(t1);  P64_(t2);  P64(t2 - t1);
                        }
                    }
                }
            }
            double elapsedTime = (double) (TU::getTimer() - t0) * 1.0e-9;
            double numCalls    = ((double) NUM_TESTS) * 1.0e-6;
            double timePerCall = elapsedTime / (numCalls * 2.0); //(microsec.)
            if (verbose) {
                cout << "\telapsed time   = " << elapsedTime << " (sec)\n"
                     << "\tnum tests     = " << numCalls << " x 10^6\n"
                     << "\ttime per call  = " << timePerCall << " (nsec)\n"
                     << "\n\tnum non-monotonic pairs   = " << numWrong
                     << "\n\tnum pairs with same value = " << numSame << endl;
            }
        }

#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // HOOKING TEST
        // Concerns:
        //   That the wall time, user time, and system time calls are making
        //   the correct calls into the OS.
        //
        // Plan:
        //   1. Take all the values, then sleep, then check how they changed.
        //   Verify that the values returned to us are compatible with the
        //   OS timers that the calls should be returning.
        //   2. Make some repeated time calls; check how they changed.  Verify
        //   that the values returned to us are compatible with the OS timers
        //   that the calls should be returning.
        //   3. Burn some CPU time, then check how they changed.  Verify that
        //   the values returned to us are compatible with the OS timers that
        //   the calls should be returning.
        //   (These are imperfect tests; we can sleep longer than we expect,
        //   and interrupt and scheduling activities can add to the user and
        //   system times.)
        //
        // Testing:
        //   bsls_Types::Int64 bsls_TimeUtil::getTimer();
        //   bsls_Types::Int64 bsls_TimeUtil::getProcessSystemTimer();
        //   bsls_Types::Int64 bsls_TimeUtil::getProcessUserTimer();
        // --------------------------------------------------------------------

        const Int64 nsecsPerSec = 1000LL * 1000LL * 1000LL;

        //  The initialization of 'timeQuantum' varies from OS to OS.
        //  'timeQuantum' is to be the minimum increment visible in a
        //  timer, expressed as a number of nanoseconds.  POSIX wants it
        //  to be 100 milliseconds.
#if defined BSLS_PLATFORM__OS_SOLARIS || defined BSLS_PLATFORM__OS_FREEBSD
        const Int64 timeQuantum = nsecsPerSec / CLK_TCK;
#elif defined BSLS_PLATFORM__OS_LINUX || defined BSLS_PLATFORM__OS_AIX \
   || defined BSLS_PLATFORM__OS_HPUX
        const Int64 timeQuantum = nsecsPerSec / sysconf(_SC_CLK_TCK);
                                        // On our local flavor of Linux, old
                                        // POSIX requirements and immoderate
                                        // file system cleverness combine to
                                        // make the symbol that is available
                                        // wrong (CLOCKS_PER_SEC == 1000000)
                                        // and the symbol that is correct
                                        // (CLK_TCK) unavailable.
                                        // (AIX just walks its own path.)
#elif defined BSLS_PLATFORM__OS_WINDOWS
        const Int64 timeQuantum = 100;  // Hard-coded from Windows
                                        // documentation.  We need to test
                                        // (not pre-accept)
                                        // 'UnixTimerUtil::s_nsecsPerSecond'.

        const Int64 windowsFudge = 15 * 1000000LL;
                                        // This interval (15 milliseconds) is
                                        // an experimentally determined
                                        // allowance for lack of
                                        // synchronization between the Windows
                                        // wall-time timer and the Windows
                                        // processor time counts.
#else
# error Need platform info to find time quantum!
        const Int64 timeQuantum[0];     // There is an error here, but
                                        // the '#error' directive is not
                                        // available everywhere.
#endif

        // First subtest: Get values, sleep, get values, check that
        // they make sense for the OS counters they are supposed to be.
        {

            if (verbose)
                cout << "\nHooking Test: System time requests across sleep."
                     << "\n================================================"
                     << endl;

            if (veryVerbose)
                cout << "timeQuantum = " << timeQuantum << endl;

            const unsigned shortSleep = 2;
            Int64 wt1 = TU::getTimer();
            Int64 ut1 = TU::getProcessUserTimer();
            Int64 st1 = TU::getProcessSystemTimer();

            osSleep(shortSleep);

            Int64 wt2 = TU::getTimer();
            Int64 ut2 = TU::getProcessUserTimer();
            Int64 st2 = TU::getProcessSystemTimer();

            if (veryVerbose)
                cout
                    << "wt1: " << wt1 << " "
                    << "ut1: " << ut1 << " "
                    << "st1: " << st1 << std::endl
                    << "wt2: " << wt2 << " "
                    << "ut2: " << ut2 << " "
                    << "st2: " << st2 << std::endl
                    << "wt2 - wt1: " << wt2 - wt1 << std::endl;

            // System and user time differences must be zero mod quantum, or
            // quantum is wrong.

            ASSERT(0 == (ut2 - ut1) % timeQuantum);
            ASSERT(0 == (st2 - st1) % timeQuantum);

            // Comparisons on times must be written in such a way that rollover
            // can never corrupt the result.  (With 'long long' values, this is
            // unlikely, but not eventually impossible.)  This means that
            // the times must be grouped on one side of the inequality and
            // their differences taken, and those differences compared to
            // whatever constants are involved.

#if !defined(BSLS_PLATFORM__OS_WINDOWS)
            ASSERT(wt2 - wt1 >= shortSleep * nsecsPerSec);
#else
            ASSERT(wt2 - wt1 + windowsFudge >= shortSleep * nsecsPerSec);
#endif
                                               // Did we sleep long enough?

            ASSERT(ut2 - ut1 >= 0);            // User time must not go
                                               // backwards ...
            ASSERT(ut2 - ut1 <= timeQuantum);  // ... nor forwards by too much.
            ASSERT(st2 - st1 >= 0);            // System time must not go
                                               // backwards ...
            ASSERT(st2 - st1 <= timeQuantum);  // ... nor forwards by too much.

        }

        // Second subtest: Make repeated time calls; check for consistency.
        {
            if (verbose)
                cout << "\nHooking Test: System time requests repeated."
                     << "\n============================================"
                     << endl;

            enum { NUM_INTERVALS = 10,
                   NUM_SAMPLES = NUM_INTERVALS + 1 };
            struct sample {
                Int64 d_wt;  // wall time
                Int64 d_ut;  // user time
                Int64 d_st;  // system time
            } samples[NUM_SAMPLES];

            samples[0].d_wt = TU::getTimer();
            samples[0].d_ut = TU::getProcessUserTimer();
            samples[0].d_st = TU::getProcessSystemTimer();

            // Time differences must remain sane over repeated calls.

            Int64 uQuantsSpent = 0;
            Int64 sQuantsSpent = 0;
            for (int i = 0; i < NUM_INTERVALS; ++i) {
                sample& s0 = samples[i];
                sample& s1 = samples[i+1];

                s1.d_wt = TU::getTimer();
                s1.d_ut = TU::getProcessUserTimer();
                s1.d_st = TU::getProcessSystemTimer();

                // Sys and user differences must be zero mod quantum, or
                // quantum is wrong.

                ASSERT(0 == (s1.d_ut - s0.d_ut) % timeQuantum);
                ASSERT(0 == (s1.d_st - s0.d_st) % timeQuantum);

                uQuantsSpent += (s1.d_ut - s0.d_ut) / timeQuantum;
                sQuantsSpent += (s1.d_st - s0.d_st) / timeQuantum;

                // We should not have spent much time in those, except possibly
                // for wall time: we might have been descheduled.  In any case,
                // time must not go backward.

                ASSERT(s1.d_wt - s0.d_wt >= 0);
                ASSERT(s1.d_ut - s0.d_ut >= 0
                    && s1.d_ut - s0.d_ut <= timeQuantum);
                ASSERT(s1.d_st - s0.d_st >= 0
                    && s1.d_st - s0.d_st <= timeQuantum);
            }

            // We should not have spent much time getting the time ten times.

            if (veryVerbose)
                cout << "uQuantsSpent: " << uQuantsSpent << "    "
                     << "sQuantsSpent: " << sQuantsSpent << endl;

            ASSERT(uQuantsSpent <= 1);
            ASSERT(sQuantsSpent <= 1);

            if (veryVerbose) {
                cout << " 0: " << "wt: " << samples[0].d_wt << endl
                     << " 0: " << "ut: " << samples[0].d_ut << endl
                     << " 0: " << "st: " << samples[0].d_st << endl;

                for (int i = 1; i < NUM_SAMPLES; ++i) {
                    sample& s0 = samples[i - 1];
                    sample& s1 = samples[i];

                    cout << std::setw(2) << i << ": "
                                             << "wt: " << s1.d_wt
                                             << " - "  << s0.d_wt
                                             << " = "  << s1.d_wt - s0.d_wt
                                                                    << endl;
                    cout << std::setw(2) << i << ": "
                                            << "ut: " << s1.d_ut
                                            << " - "  << s0.d_ut
                                            << " = "  << s1.d_ut - s0.d_ut
                                                                    << endl;
                    cout << std::setw(2) << i << ": "
                                            << "st: " << s1.d_st
                                            << " - "  << s0.d_st
                                            << " = "  << s1.d_st - s0.d_st
                                                                    << endl;
                }
            }

        }

        // Third subtest: Consume time; verify that it appears in the counts.
        {
            const unsigned longLoop = 8 * 1000L * 1000L;

            // Burn a little processor time--at least one timeQuantum.
            // The 'volatile' should force every access to memory and slow
            // things up a little, or a lot.  It may make the test less
            // dependent on ever-increasing processor speeds.

            if (verbose)
                cout << "\nHooking Test: System time requests over long loop."
                     << "\n=================================================="
                     << endl;

            Int64 wt1 = TU::getTimer();
            Int64 ut1 = TU::getProcessUserTimer();
            Int64 st1 = TU::getProcessSystemTimer();

            for (volatile unsigned u = 0; u < longLoop; ++u) {
                ++u; --u;
            }

            Int64 wt2 = TU::getTimer();
            Int64 ut2 = TU::getProcessUserTimer();
            Int64 st2 = TU::getProcessSystemTimer();

            if (veryVerbose)
                cout << "wt2: " << wt2       << " -  "
                     << "wt1: " << wt1       << " = "
                                << wt2 - wt1 << std::endl
                     << "ut2: " << ut2       << " -  "
                     << "ut1: " << ut1       << " = "
                                << ut2 - ut1 << std::endl
                     << "st2: " << st2       << " -  "
                     << "st1: " << st1       << " = "
                                << st2 - st1 << std::endl << endl;

            ASSERT(wt2 - wt1 >= timeQuantum);
                                        // Our wall time is over a timeQuantum.

            ASSERT(ut2 - ut1 >= timeQuantum);
                                        // We ran at least a timeQuantum.

            ASSERT(st2 - st1 >= 0);     // And system time did not go backward.

#if !defined(BSLS_PLATFORM__OS_WINDOWS)
            ASSERT((wt2 - wt1) - (ut2 - ut1) - (st2 - st1) +
                                                         2 * timeQuantum >= 0);
                                        // And our wall time was greater than
                                        // our user and system time together,
                                        // allowing for quantization error
                                        // (in both user and system time).
#else
            ASSERT((wt2 - wt1) - (ut2 - ut1) - (st2 - st1)
                                  + 10 * windowsFudge + 2 * timeQuantum >= 0);
#endif

        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //   Test whether successive calls ever return the same value.
        //
        // Plan:
        //   Call each method two times within a loop, and compare the return
        //   values each time.
        //
        // Testing:
        //   bsls_Types::Int64 bsls_TimeUtil::getTimer();
        //   bsls_Types::Int64 bsls_TimeUtil::getProcessSystemTimer();
        //   bsls_Types::Int64 bsls_TimeUtil::getProcessUserTimer();
        //   void bsls_TimeUtil::getProcessTimers(bsls_Types::Int64,
        //                                        bsls_Types::Int64);
        // --------------------------------------------------------------------

        struct {
            TimerMethod d_method;
            std::string d_methodName;
            bool        d_reportSame;
                // don't report same values for user and system timers
        }
        TimerMethods[] = {
            { TU::getTimer,                 "getTimer",                true  },
            { TU::getProcessSystemTimer,    "getProcessSystemTimer",   false },
            { TU::getProcessUserTimer,      "getProcessUserTimer",     false },
            { callGetProcessTimersRetSystem,"getProcessTimers(system)",false },
            { callGetProcessTimersRetUser,  "getProcessTimers(user)",  false }
        };
        size_t const TimerMethodsCount =
                                  sizeof TimerMethods / sizeof TimerMethods[0];

        for (size_t t = 0; t < TimerMethodsCount; ++t) {
            if (verbose)
                cout << "\nTesting '"
                    << TimerMethods[t].d_methodName
                    << "()' statistical correctness"
                    << "\n===========================================" << endl;

            if (verbose)
                cout << "\nCall 'bsls_TimeUtil::"
                    << TimerMethods[t].d_methodName
                    << "()' twice in a large loop."
                    << endl;
            {
                const Int64 NUM_TESTS  = verbose ? 100LL * atoi(argv[2]) : 100;

                const Int64 t0 = TU::getTimer(); //measure the tool with itself
                Int64       t1, t2;
                int         numSame  = 0;
                int         numWrong = 0;

                for (Int64 i = 1; i < NUM_TESTS; ++i) {
                    t1 = TimerMethods[t].d_method();
                    t2 = TimerMethods[t].d_method();

                    //t1 = (Int64) gethrtime();  // These are saved for future
                    //t2 = (Int64) gethrtime();  // "paranoid" tests.

                    if (t2 <= t1) {  // Optimize for CORRECT behavior
                        if (t2 == t1) {
                            if (TimerMethods[t].d_reportSame) {
                                ++numSame;
                                if (verbose) { T_();  P_(i);  P(numSame); }
                            }
                        }
                        else {
                            ++numWrong;
                            if (verbose) {
                                T_();      P64_(i);   P_(numWrong);
                                P64_(t1);  P64_(t2);  P64(t2 - t1);
                            }
                        }
                    }
                }
                LOOP_ASSERT(t, 0 == numWrong);

                double elapsedTime = (double) (TU::getTimer() - t0) * 1.0e-9;
                double numCalls    = ((double) NUM_TESTS) * 1.0e-6;
                double timePerCall = elapsedTime / (numCalls * 2.0);//microsec
                if (verbose) {
                    cout << "\telapsed time   = " << elapsedTime << " (sec)\n"
                        << "\tnum tests     = " << numCalls << " x 10^6\n"
                        << "\ttime per call  = " << timePerCall << " (nsec)\n"
                        << "\n\tnum non-monotonic pairs   = " << numWrong
                        << "\n\tnum pairs with same value = " << numSame
                        << endl;
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //   Time the (platform-dependent) call.
        //
        // Plan:
        //   Call each method in a loop.
        //
        // Testing:
        //   bsls_Types::Int64 bsls_TimeUtil::getTimer();
        //   bsls_Types::Int64 bsls_TimeUtil::getProcessSystemTimer();
        //   bsls_Types::Int64 bsls_TimeUtil::getProcessUserTimer();
        //   void bsls_TimeUtil::getProcessTimers(bsls_Types::Int64,
        //                                        bsls_Types::Int64);
        // --------------------------------------------------------------------

        struct {
            TimerMethod d_method;
            std::string d_methodName;
        }
        TimerMethods[] = {
            { TU::getTimer,                  "getTimer"                 },
            { TU::getProcessSystemTimer,     "getProcessSystemTimer"    },
            { TU::getProcessUserTimer,       "getProcessUserTimer"      },
            { callGetProcessTimersRetSystem, "getProcessTimers(system)" },
            { callGetProcessTimersRetUser,   "getProcessTimers(user)"   }
        };
        size_t const TimerMethodsCount =
                                  sizeof TimerMethods / sizeof TimerMethods[0];

        for (size_t t = 0; t < TimerMethodsCount; ++t) {
            if (verbose)
                cout << "\nTesting 'bsls_TimeUtil::"
                    << TimerMethods[t].d_methodName
                    << " Performance()'"
                    << "\n==============================================="
                    << endl;

            if (verbose)
                cout << "\nCall 'bsls_TimeUtil::"
                    << TimerMethods[t].d_methodName
                    << "()' in a large loop."
                    << endl;
            {
                const Int64 NUM_STEPS  = verbose ? 10LL * atoi(argv[2]) : 10;
                const Int64 t0 = TU::getTimer(); //measure the tool with itself

                for (Int64 i = 1; i < NUM_STEPS; ++i) {
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                    TimerMethods[t].d_method();
                }
                double elapsedTime = (double) (TU::getTimer() - t0) * 1.0e-9;
                double numCalls    = ((double) NUM_STEPS * 10.0) * 1.0e-6;
                double timePerCall = elapsedTime / numCalls; // (microsec.)
                if (verbose) {
                    cout << "\telapsed time  = " << elapsedTime << " (sec)\n"
                        << "\tnum calls    = " << numCalls << " x 10^6\n"
                        << "\ttime per call = " << timePerCall << " (usec)"
                        << endl;
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header must build and
        //   run with a normal exit status.
        //
        // Plan:
        //   Copy the implementation portion of the Usage Example to the
        //   reserved space above, and copy the executable portion below,
        //   adding any needed supporting code.
        //
        // Testing:
        //   USAGE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        {
            my_Timer tw;
            for (int i = 0; i < 1000000; ++i) {
                // ...
            }
            double dTw = tw.elapsedWallTime();
            my_Timer tu;
            for (int i = 0; i < 1000000; ++i) {
                // ...
            }
            double dTu = tu.elapsedUserTime();
            my_Timer ts;
            for (int i = 0; i < 1000000; ++i) {
                // ...
            }
            double dTs = ts.elapsedSystemTime();
            if (verbose)
                std::cout
                    << "elapsed wall time: " << dTw << std::endl
                    << "elapsed user time: " << dTu << std::endl
                    << "elapsed system time: " << dTs << std::endl;
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC FUNCTIONALITY
        //   The methods should return non-decreasing values of type
        //   'bsls_Types::Int64'.
        //
        // Plan:
        //   Verify that the *temporary* returned by each method is at least 8
        //   bytes long.  Then invoke the method several times in sequence,
        //   separated by delay loops of increasing duration, and assert that
        //   the return value does not decrease.  Print results and differences
        //   in 'veryVerbose' mode.
        //
        // Testing:
        //   bsls_Types::Int64 bsls_TimeUtil::getTimer();
        //   bsls_Types::Int64 bsls_TimeUtil::getProcessSystemTimer();
        //   bsls_Types::Int64 bsls_TimeUtil::getProcessUserTimer();
        //   void bsls_TimeUtil::getProcessTimers(bsls_Types::Int64,
        //                                        bsls_Types::Int64);
        // --------------------------------------------------------------------

        struct {
            TimerMethod d_method;
            std::string d_methodName;
        }
        TimerMethods[] = {
            { TU::getTimer,                  "getTimer"                 },
            { TU::getProcessSystemTimer,     "getProcessSystemTimer"    },
            { TU::getProcessUserTimer,       "getProcessUserTimer"      },
            { callGetProcessTimersRetSystem, "getProcessTimers(system)" },
            { callGetProcessTimersRetUser,   "getProcessTimers(user)"   }
        };
        size_t const TimerMethodsCount =
                                  sizeof TimerMethods / sizeof TimerMethods[0];

        for (size_t t = 0; t < TimerMethodsCount; ++t) {
            if (verbose)
                cout << "\nTesting '"
                     << TimerMethods[t].d_methodName
                     << "()'"
                     << "\n====================" << endl;

            if (verbose)
                cout << "\nConfirm sizeof ("
                     << TimerMethods[t].d_methodName
                     << "())."
                     << endl;
            {
                int numBytes = sizeof(TimerMethods[t].d_method());
                if (veryVerbose) { T_(); P(numBytes); }
                LOOP_ASSERT(t, 8 <= numBytes);
            }

            if (verbose)
                cout << "\nExercise '"
                     << TimerMethods[t].d_methodName
                     << "()'." << endl;
            {
                const int NUM_CALLS   = 10;
                const int DELAY_COUNT = 1000000;
                Int64     result[NUM_CALLS];
                double    totalElapsedTime = 0.0;

                result[0] = TimerMethods[t].d_method();
                if (veryVerbose) { T_();  P64(result[0]); }
                for (int i = 1; i < NUM_CALLS; ++i) {
                    for (int j = 0; j < DELAY_COUNT * i; ++j) {
                        i = i + 1;
                        i = i - 1;
                    }
                    result[i] = TimerMethods[t].d_method();
                    Int64 difference = result[i] - result[i - 1];
                    double seconds = (double) difference / 1.0e9;
                    totalElapsedTime += seconds;
                    if (veryVerbose) {
                        T_();  P64_(result[i]);  P64_(difference);  P(seconds);
                    }
                    LOOP2_ASSERT(t, i, (Int64) 0 <= difference);//cast--dg bug
                }
                if (veryVerbose) { cout << endl;  T_();  P(totalElapsedTime); }
            }
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
//      Copyright (C) Bloomberg L.P., 2008, 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
