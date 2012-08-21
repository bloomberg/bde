// bsls_timeutil.cpp                                                  -*-C++-*-
#include <bsls_timeutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_platform.h>     // BSLS_PLATFORM__OS_UNIX, etc.
#include <bsls_atomicoperations.h>

#if defined BSLS_PLATFORM__OS_UNIX
    #include <time.h>      // NOTE: <ctime> conflicts with <sys/time.h>
    #include <sys/time.h>  // gethrtime()
    #include <sys/times.h> // struct tms, times()
    #include <unistd.h>    // sysconf(), _SC_CLK_TCK
#elif defined BSLS_PLATFORM__OS_WINDOWS
    #include <windows.h>
    #include <winbase.h>   // QueryPerformanceCounter(), GetProcessTimes()
    #include <sys/timeb.h> // ftime(struct timeb *)
#else
    #error "Don't know how to get nanosecond time for this platform"
#endif

namespace BloombergLP {

namespace {

#ifdef BSLS_PLATFORM__OS_UNIX
struct UnixTimerUtil {
    // Provides access to UNIX process user and system timers.

  private:
    // CLASS DATA
    static bsls::AtomicOperations::AtomicTypes::Int64 s_ticksPerSecond;
    static const bsls::Types::Int64                   s_nsecsPerSecond;

  private:
    // PRIVATE CLASS METHODS
    static void systemProcessTimers(clock_t *systemTimer, clock_t *userTimer);
        // Helper routine to be used by the public methods below: call the OS
        // APIs and handle errors.

  public:
    // CLASS METHODS
    static void initialize();
        // Ensure all the fields are initialized (currently only the
        // 's_ticksPerSecond' value above).

    static bsls::Types::Int64 systemTimer();
        // Return converted to nanoseconds current value of system time as
        // returned by times() if the call succeeds, and zero otherwise.

    static bsls::Types::Int64 userTimer();
        // Return converted to nanoseconds current value of user time as
        // returned by times() if the call succeeds, and zero otherwise.

    static void processTimers(bsls::Types::Int64 *systemTimer,
                              bsls::Types::Int64 *userTimer);
        // Return converted to nanoseconds current values of system and user
        // times as returned by times() if the call succeeds, and zero values
        // otherwise
};

bsls::AtomicOperations::AtomicTypes::Int64
                                        UnixTimerUtil::s_ticksPerSecond = {-1};
const bsls::Types::Int64 UnixTimerUtil::s_nsecsPerSecond = 1000 * 1000 * 1000;

inline
void UnixTimerUtil::systemProcessTimers(clock_t *systemTimer,
                                        clock_t *userTimer)
{
    struct tms processTimes;
    if (-1 == ::times(&processTimes)) {
        *systemTimer = 0;
        *userTimer   = 0;
        return;                                                       // RETURN
    }

    *systemTimer = processTimes.tms_stime;
    *userTimer   = processTimes.tms_utime;
}

inline
void UnixTimerUtil::initialize()
{
    if (-1 == bsls::AtomicOperations::getInt64Relaxed(&s_ticksPerSecond)) {
        long ticksPerSecond = ::sysconf(_SC_CLK_TCK);

        bsls::AtomicOperations::setInt64Relaxed(
                &s_ticksPerSecond,
                ticksPerSecond != -1 ? ticksPerSecond : CLOCKS_PER_SEC);
    }
}

inline
bsls::Types::Int64 UnixTimerUtil::systemTimer()
{
    initialize();

    clock_t sTimer, dummy;
    systemProcessTimers(&sTimer, &dummy);

    return static_cast<bsls::Types::Int64>(sTimer) * s_nsecsPerSecond
        / bsls::AtomicOperations::getInt64Relaxed(&s_ticksPerSecond);
}

inline
bsls::Types::Int64 UnixTimerUtil::userTimer()
{
    initialize();

    clock_t dummy, uTimer;
    systemProcessTimers(&dummy, &uTimer);

    return static_cast<bsls::Types::Int64>(uTimer) * s_nsecsPerSecond
        / bsls::AtomicOperations::getInt64Relaxed(&s_ticksPerSecond);
}

inline
void UnixTimerUtil::processTimers(bsls::Types::Int64 *systemTimer,
                                  bsls::Types::Int64 *userTimer)
{
    initialize();

    clock_t sTimer, uTimer;
    systemProcessTimers(&sTimer, &uTimer);

    bsls::Types::Int64 ticksPerSecond
        = bsls::AtomicOperations::getInt64Relaxed(&s_ticksPerSecond);
    *systemTimer = static_cast<bsls::Types::Int64>(sTimer)
                   * s_nsecsPerSecond / ticksPerSecond;
    *userTimer   = static_cast<bsls::Types::Int64>(uTimer)
                   * s_nsecsPerSecond / ticksPerSecond;
}
#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS
struct WindowsTimerUtil {
    // Provides access to Windows process user and system timers.

  private:
    // CLASS DATA
    static bsls::AtomicOperations::AtomicTypes::Int   s_initRequired;

    static bsls::AtomicOperations::AtomicTypes::Int64 s_initialTime;
                                                // initial time for the
                                                // Windows hardware
                                                // timer

    static bsls::AtomicOperations::AtomicTypes::Int64 s_timerFrequency;
                                                // frequency of the
                                                // Windows hardware
                                                // timer

    static bsls::AtomicOperations::AtomicTypes::Int64 s_highPartDivisionFactor;
                                                // cached, frequency-dependent
                                                // factor for calculating the
                                                // high part's contribution
                                                // to a nanosecond time

    static bsls::AtomicOperations::AtomicTypes::Int64
                                                     s_highPartRemainderFactor;
                                                // cached, frequency-dependent
                                                // factor for calculating the
                                                // high part's remainder's
                                                // contribution to a
                                                // nanosecond time

    static const bsls::Types::Int64 s_nsecsPerUnit;
                                                // size in nanoseconds
                                                // of one time unit used
                                                // by GetProcessTimes()

  private:
    // PRIVATE CLASS METHODS
    static void systemProcessTimers(PULARGE_INTEGER systemTimer,
                                    PULARGE_INTEGER userTimer);
        // Helper routine to be used by the public methods below: call the OS
        // APIs and handle errors.

  public:
    // CLASS METHODS
    static void initialize();
        // Ensure all the fields are initialized (currently the 's_initialTime'
        // and the 's_timerFrequency' values above).

    static bsls::Types::Int64 systemTimer();
        // Return converted to nanoseconds current value of kernel (system)
        // time as returned by GetProcessTimes() if the call succeeds, and zero
        // otherwise.

    static bsls::Types::Int64 userTimer();
        // Return converted to nanoseconds current value of user time as
        // returned by GetProcessTimes() if the call succeeds, and zero
        // otherwise.

    static void processTimers(bsls::Types::Int64 *systemTimer,
                              bsls::Types::Int64 *userTimer);
        // Return converted to nanoseconds current value of kernel (system) and
        // user times as returned by GetProcessTimes() if the call succeeds,
        // and zero otherwise.

    static bsls::Types::Int64 wallTimer();
        // Return converted to nanoseconds current value of wall time as per
        // Windows hardware timer, if available, uses ::ftime otherwise.

    static bsls::Types::Int64 getTimerRaw();
        // Return a machine-dependent value representing the current time.
        // 'timeValue' must be converted by the 'convertRawTime' method to
        // conventional units (nanoseconds).  This method is intended to
        // facilitate accurate timing of small segments of code, and care must
        // be used in interpreting the results.  Note that this method is
        // thread-safe only if 'initialize' has been called before.

    static bsls::Types::Int64 convertRawTime(bsls::Types::Int64 rawTime);
        // Convert the specified 'rawTime' to a value in nanoseconds,
        // referenced to an arbitrary but fixed origin, and return the result
        // of the conversion.  Note that this method is thread-safe only if
        // 'initialize' has been called before.
};

bsls::AtomicOperations::AtomicTypes::Int
                                     WindowsTimerUtil::s_initRequired   = {1};
bsls::AtomicOperations::AtomicTypes::Int64
                                     WindowsTimerUtil::s_initialTime    = {-1};
bsls::AtomicOperations::AtomicTypes::Int64
                                     WindowsTimerUtil::s_timerFrequency = {-1};
bsls::AtomicOperations::AtomicTypes::Int64
                            WindowsTimerUtil::s_highPartDivisionFactor  = {-1};
bsls::AtomicOperations::AtomicTypes::Int64
                            WindowsTimerUtil::s_highPartRemainderFactor = {-1};
const bsls::Types::Int64             WindowsTimerUtil::s_nsecsPerUnit   = 100;

inline
void WindowsTimerUtil::systemProcessTimers(PULARGE_INTEGER systemTimer,
                                           PULARGE_INTEGER userTimer)
{
    FILETIME crtnTm, exitTm, krnlTm, userTm;

    if (!::GetProcessTimes(::GetCurrentProcess(),
                           &crtnTm,
                           &exitTm,
                           &krnlTm,
                           &userTm)) {
        systemTimer->QuadPart = 0;
        userTimer->QuadPart   = 0;
        return;                                                       // RETURN
    }

    systemTimer->LowPart  = krnlTm.dwLowDateTime;
    systemTimer->HighPart = krnlTm.dwHighDateTime;
    userTimer->LowPart    = userTm.dwLowDateTime;
    userTimer->HighPart   = userTm.dwHighDateTime;
}

inline
void WindowsTimerUtil::initialize()
{
    const bsls::Types::Int64 K = 1000;
    const bsls::Types::Int64 G = K * K * K;
    const bsls::Types::Int64 HIGH_DWORD_MULTIPLIER = G * (1LL << 32);

    if (bsls::AtomicOperations::getIntRelaxed(&s_initRequired)) {
        bsls::AtomicOperations::setIntRelaxed(&s_initRequired, 0);

        LARGE_INTEGER t;
        bsls::AtomicOperations::setInt64Relaxed(
                &s_initialTime,
                ::QueryPerformanceCounter(&t) ? t.QuadPart : 0);

        LARGE_INTEGER f;
        if (::QueryPerformanceFrequency(&f)) {
            bsls::AtomicOperations::setInt64Relaxed(&s_timerFrequency,
                                                    f.QuadPart);

            bsls::AtomicOperations::setInt64Relaxed(
                &s_highPartDivisionFactor, HIGH_DWORD_MULTIPLIER / f.QuadPart);

            bsls::AtomicOperations::setInt64Relaxed(
               &s_highPartRemainderFactor, HIGH_DWORD_MULTIPLIER % f.QuadPart);
        }
        else {
            bsls::AtomicOperations::setInt64Relaxed(&s_timerFrequency, 0);

            bsls::AtomicOperations::setInt64Relaxed(&s_highPartDivisionFactor,
                                                    0);

            bsls::AtomicOperations::setInt64Relaxed(&s_highPartRemainderFactor,
                                                    0);
    }
}
}

inline
bsls::Types::Int64 WindowsTimerUtil::systemTimer()
{
    ULARGE_INTEGER sTimer, dummy;
    systemProcessTimers(&sTimer, &dummy);

    return sTimer.QuadPart * s_nsecsPerUnit;
}

inline
bsls::Types::Int64 WindowsTimerUtil::userTimer()
{
    ULARGE_INTEGER dummy, uTimer;
    systemProcessTimers(&dummy, &uTimer);

    return uTimer.QuadPart * s_nsecsPerUnit;
}

inline
void WindowsTimerUtil::processTimers(bsls::Types::Int64 *systemTimer,
                                     bsls::Types::Int64 *userTimer)
{
    ULARGE_INTEGER sTimer, uTimer;
    systemProcessTimers(&sTimer, &uTimer);

    *systemTimer = static_cast<bsls::Types::Int64>(sTimer.QuadPart)
                                                              * s_nsecsPerUnit;
    *userTimer = static_cast<bsls::Types::Int64>(uTimer.QuadPart)
                                                              * s_nsecsPerUnit;
}

inline
bsls::Types::Int64 WindowsTimerUtil::wallTimer()
{
    return convertRawTime(getTimerRaw());
}

inline
bsls::Types::Int64 WindowsTimerUtil::convertRawTime(bsls::Types::Int64 rawTime)
{
    initialize();

    const bsls::Types::Int64 K = 1000;
    const bsls::Types::Int64 M = K * K;
    const bsls::Types::Int64 G = K * K * K;
    const bsls::Types::Uint64 LOW_MASK = 0x00000000ffffffff;

    bsls::Types::Int64 initialTime
        = bsls::AtomicOperations::getInt64Relaxed(&s_initialTime);

    if (0 != initialTime) {
        bsls::Types::Int64 timerFrequency
            = bsls::AtomicOperations::getInt64Relaxed(&s_timerFrequency);
        // Not implemented: Assert that rawTime - initialTime will fit in an 
        // Int64, when expressed as nanoseconds (~292 days).
        //
        // N.B. This assert is not implemented because:
        // A) Cannot use BSLS_ASSERT because bsls_assert has an indirect
        // (and testing only) dependency on bsls_timeutil.
        // B) Cannot use std::numeric_limits.
        //
        // If it were implemented, it would look like the following:
        // BSLS_ASSERT(((rawTime - initialTime) / timerFrequency) 
        //             < std::numeric_limits<bsls::Types::Int64>::max() / G)

        bsls::Types::Int64 highPartDivisionFactor
            = bsls::AtomicOperations::getInt64Relaxed(
                                                    &s_highPartDivisionFactor);
        bsls::Types::Int64 highPartRemainderFactor
            = bsls::AtomicOperations::getInt64Relaxed(
                                                   &s_highPartRemainderFactor);

        rawTime -= initialTime;

        // Original outline for improved-precision nanosecond calculation
        // with integer arithmetic, provided to make the final implementation
        // easier to understand:

        // Treat the high-dword and low-dword contributions to the counter as
        // separate 64-bit values.  Since all known timerFrequency values fit
        // inside 32 unsigned bits, the high-dword contribution will retain 32
        // significant bits after being divided by the timerFrequency.
        // Therefore the calculation can be done on the high-dword contribution
        // by dividing first by the frequency and then multiplying by one
        // billion.  Similarly, one billion fits inside signed 32 bits, so the
        // low-dword contribution can be multiplied by one billion without
        // overflowing.  Therefore, the calculation can be done on the
        // low-dword contribution by multiplying first by one billion, and then
        // dividing by the frequency.

        // const bsls::Types::Uint64 HIGH_MASK = 0xffffffff00000000;
        // return (
        //         (
        //          static_cast<bsls::Types::Int64>(t.QuadPart & HIGH_MASK)
        //              / timerFrequency
        //         ) * G
        //         +
        //         (
        //          static_cast<bsls::Types::Uint64>(t.LowPart)
        //              * G
        //         ) / timerFrequency
        //        );                                                  // RETURN

        // If the high-dword contribution is small, this approach will lose
        // most of the significant bits from the high-dword contribution during
        // the initial division.  It can be improved by merging the division
        // with the multiplication by one billion:

        return (
                // Divide high part by frequency
                static_cast<bsls::Types::Int64>(rawTime >> 32)
                    * highPartDivisionFactor
                +
                (
                 // Restore remainder of high part division
                 static_cast<bsls::Types::Int64>(rawTime >> 32)
                    * highPartRemainderFactor
                 +
                 // Calculate low part contribution
                 static_cast<bsls::Types::Uint64>(rawTime & LOW_MASK)
                    * G
                )
                    / timerFrequency
                );                                                    // RETURN

        // Note that by caching the highPart factors, this code runs as fast as
        // the original implementation.  It works for counters representing
        // time values up to 292 years (the upper limit for representing
        // nanoseconds in 64 bits), and for any frequency that fits in 32 bits.
        // The original implementation broke down on counter values over
        // ~9x10^12, which could be as little as 50 minutes with a 3GHz clock.

        // Another alternative is to use floating-point arithmetic.  This is
        // just as fast as the integer arithmetic on my development machine,
        // but might be very slow on systems with pre-Pentium CPUs, unless they
        // have a separate floating-point processor.

        // return static_cast<bsls::Types::Int64>(
        //     static_cast<double>(t.QuadPart) * G) /
        //     static_cast<double>(timerFrequency));
    }
    else {
        return rawTime * M;                                           // RETURN
    }
}

inline
bsls::Types::Int64 WindowsTimerUtil::getTimerRaw()
{
    const bsls::Types::Int64 K = 1000;

    initialize();

    bsls::Types::Int64 initialTime
        = bsls::AtomicOperations::getInt64Relaxed(&s_initialTime);

    if (0 != initialTime) {
        LARGE_INTEGER counter;
        ::QueryPerformanceCounter(&counter);
        return counter.QuadPart;                                      // RETURN
    } else {
        timeb t;
        ::ftime(&t);

        return static_cast<bsls::Types::Int64>(t.time)                // RETURN
            * K + t.millitm;
    }
}

#endif

}  // close unnamed namespace

namespace bsls {

                            // ---------------
                            // struct TimeUtil
                            // ---------------

// CLASS METHODS
void TimeUtil::initialize()
{
#if defined BSLS_PLATFORM__OS_UNIX
    UnixTimerUtil::initialize();
#elif defined BSLS_PLATFORM__OS_WINDOWS
    WindowsTimerUtil::initialize();
#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

Types::Int64
TimeUtil::convertRawTime(TimeUtil::OpaqueNativeTime rawTime)
{
#if defined BSLS_PLATFORM__OS_SOLARIS

    return rawTime.d_opaque;

#elif defined BSLS_PLATFORM__OS_AIX

    // Imp Note:
    // 'time_base_to_time' takes much more time (~1.2 usec) than actually
    // collecting the raw time in the first place (<100 nsec).

    const Types::Int64 G = 1000000000;
    time_base_to_time(&rawTime, TIMEBASE_SZ);
    return (Types::Int64) rawTime.tb_high * G + rawTime.tb_low;

#elif defined BSLS_PLATFORM__OS_HPUX

    return rawTime.d_opaque;

#elif defined BSLS_PLATFORM__OS_LINUX

    const Types::Int64 G = 1000000000;
    return ((Types::Int64) rawTime.tv_sec * G + rawTime.tv_nsec);

#elif defined BSLS_PLATFORM__OS_UNIX

    const Types::Int64 K = 1000;
    const Types::Int64 M = 1000000;
    return ((Types::Int64) rawTime.tv_sec * M + rawTime.tv_usec) * K;

#elif defined BSLS_PLATFORM__OS_WINDOWS

    return WindowsTimerUtil::convertRawTime(rawTime.d_opaque);

#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

Types::Int64 TimeUtil::getTimer()
{
#if defined BSLS_PLATFORM__OS_SOLARIS

    // Call the platform-specific function directly, since it is reliable and
    // requires no conversion.

    return gethrtime();

#elif defined BSLS_PLATFORM__OS_AIX   || \
      defined BSLS_PLATFORM__OS_HPUX  || \
      defined BSLS_PLATFORM__OS_LINUX || \
      defined BSLS_PLATFORM__OS_UNIX

    TimeUtil::OpaqueNativeTime rawTime;
    getTimerRaw(&rawTime);
    return convertRawTime(rawTime);

    // Other generic BSLS_PLATFORM__OS_UNIX implementations are left in place
    // as comments for historical reference.

    // The below imp would cause bsls_stopwatch to profile at ~1.45 usec on AIX
    // when compiled with /bb/util/version10-062009/usr/vacpp/bin/xlC_r
    //..
    //  const Types::Int64 G = 1000000000;
    //  timespec ts;
    //  clock_gettime(CLOCK_REALTIME, &ts);
    //  return = (Types::Int64) ts.tv_sec * G + ts.tv_nsec;
    //..

    // Historic workaround for non-monotonic clock behavior.
    //..
    //  const Types::Int64 K = 1000;
    //  const Types::Int64 M = 1000000;
    //  timeval t;
    //  Types::Int64 t0, t1, t2;
    //  gettimeofday(&t, 0);
    //  t0 = ((Types::Int64) t.tv_sec * M + t.tv_usec) * K;
    //  do {
    //      gettimeofday(&t, 0);
    //      t1 = ((Types::Int64) t.tv_sec * M + t.tv_usec) * K;
    //  } while (t1 == t0);
    //  do {
    //      gettimeofday(&t, 0);
    //      t2 = ((Types::Int64) t.tv_sec * M + t.tv_usec) * K;
    //  } while (t2 == t1);
    //  return t2 < t1 || t2 < t1 + 10 * M ? t2 : t1;
    //..

#elif defined BSLS_PLATFORM__OS_WINDOWS

    return WindowsTimerUtil::wallTimer();

#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

void TimeUtil::getTimerRaw(TimeUtil::OpaqueNativeTime *timeValue)
{
    // Historical Note: Older Sun machines (e.g., sundev2 circa 2003) exhibited
    // intermittent non-compliant (i.e., non-monotonic) behavior for function
    // 'gethrtime'.  As of July 2004, no non-monotonic behavior has been seen
    // on any Sun machine.  However, hp2 does exhibit a *different*
    // non-monotonic behavior.  An Imp Note within the 'OS_HPUX' block
    // describes the new problem.  The imp note in the next paragraph is no
    // longer valid, but is retained for archival purposes, and as a historical
    // caution.
    //
    // Archival Imp Note:
    // This method is implemented with a workaround for the clock-sync-induced
    // problem that, occasionally, calls to system-time functions will return
    // a value that is grossly too large.  Subsequent return values are
    // presumed correct, and it is also assumed that the probability of two
    // successive bad return values is vanishingly small.  On the other hand,
    // two successive *calls* may yield *the same* return value, and so a new
    // return value may need to be explicitly confirmed.

#if defined BSLS_PLATFORM__OS_SOLARIS

    // 'gethrtime' has not been observed to fail on any 'sundev' machine.

    timeValue->d_opaque = gethrtime();

#elif defined BSLS_PLATFORM__OS_AIX

    // Imp Note:
    // 'read_wall_time' is very fast (<100 nsec), and guaranteed monotonic per
    // http://publib.boulder.ibm.com/infocenter/systems doc.  (It has not been
    // observed to be non-monotonic when tested to better than 3 parts in 10^10
    // on ibm2.)  Converting the time to nanoseconds is much slower
    // (~1.2 usec).

    read_wall_time(timeValue, TIMEBASE_SZ);

#elif defined BSLS_PLATFORM__OS_HPUX

    // The following Imp Note applies to behavior observed on 'hp2' in late
    // July and early August of 2004.
    //
    // The call to 'gethrtime' takes about 100 nsecs (the mode difference
    // between two successive return values).  About once in every 10^7 call
    // pairs, the difference is negative; the distribution of negative values
    // is bimodal, with (approximately equal) peaks at 0 and -90 nsec.  The
    // maximum observed negative difference in 10^9 call pairs was -144 nsec.
    //
    // The algorithm implemented here defends against this behavior by
    // returning the maximum of two successive calls to 'gethrtime'.

    Types::Int64 t1 = (Types::Int64) gethrtime();
    Types::Int64 t2 = (Types::Int64) gethrtime();
    timeValue->d_opaque = t2 > t1 ? t2 : t1;

#elif defined BSLS_PLATFORM__OS_LINUX

    // The call to 'clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts)' has never
    // been observed to be non-monotonic when tested at better than 1 parts in
    // 10^9 on linxdev5, in a tight loop.  However, not all users will own the
    // process, and for multi-processor systems the value returned may be
    // meaningless.  Therefore, for the standard timer, the CLOCK_MONOTONIC
    // clock is preferred.
    //..
    //         Clock ID            Performance (arbitrary but consistent test)
    // ------------------------    -------------------------------------------
    // CLOCK_PROCESS_CPUTIME_ID         ~0.8  usec
    // CLOCK_THREAD_CPUTIME_ID          ~0.85 usec
    // CLOCK_REALTIME                   ~1.8  usec
    // CLOCK_MONOTONIC                  ~1.8  usec
    //..

    clock_gettime(CLOCK_MONOTONIC, timeValue);         // significantly slower
                                                       // ~1.8 usec

#elif defined BSLS_PLATFORM__OS_UNIX

    // A generic implementation having microsecond resolution is used.  There
    // is no attempt to defend against possible non-monotonic
    // behavior. Together with the arithmetic to convert 'timeValue' to
    // nanoseconds, the imp would cause bsls_stopwatch to profile at ~1.40 usec
    // on AIX when compiled with /bb/util/version10-062009/usr/vacpp/bin/xlC_r.

    gettimeofday(timeValue, 0);

#elif defined BSLS_PLATFORM__OS_WINDOWS

    timeValue->d_opaque = WindowsTimerUtil::getTimerRaw();

#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

Types::Int64 TimeUtil::getProcessSystemTimer()
{
#if defined BSLS_PLATFORM__OS_UNIX
    return UnixTimerUtil::systemTimer();
#elif defined BSLS_PLATFORM__OS_WINDOWS
    return WindowsTimerUtil::systemTimer();
#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

Types::Int64 TimeUtil::getProcessUserTimer()
{
#if defined BSLS_PLATFORM__OS_UNIX
    return UnixTimerUtil::userTimer();
#elif defined BSLS_PLATFORM__OS_WINDOWS
    return WindowsTimerUtil::userTimer();
#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

void TimeUtil::getProcessTimers(Types::Int64 *systemTimer,
                                Types::Int64 *userTimer)
{
#if defined BSLS_PLATFORM__OS_UNIX
    UnixTimerUtil::processTimers(systemTimer, userTimer);
#elif defined BSLS_PLATFORM__OS_WINDOWS
    WindowsTimerUtil::processTimers(systemTimer, userTimer);
#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

}  // close package namespace

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
