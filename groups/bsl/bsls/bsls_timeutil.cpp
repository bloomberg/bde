// bsls_timeutil.cpp                                                  -*-C++-*-

// Required to expose `RUSAGE_THREAD` from `<sys/resource.h>` on Linux.  Must
// be defined before including any system header.
#if defined(__linux__) && !defined(_GNU_SOURCE)
    #define _GNU_SOURCE
#endif

#include <bsls_timeutil.h>

#include <bsls_assert.h>
#include <bsls_bslonce.h>
#include <bsls_bsltestutil.h>  // for testing only

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_platform.h>     // BSLS_PLATFORM_OS_UNIX, etc.
#include <bsls_atomicoperations.h>

#if defined BSLS_PLATFORM_OS_UNIX
    #include <time.h>          // NOTE: <ctime> conflicts with <sys/time.h>
    #include <sys/resource.h>  // struct rusage
    #include <unistd.h>        // sysconf(), _SC_CLK_TCK
#elif defined BSLS_PLATFORM_OS_WINDOWS
    #include <windows.h>
    #include <winbase.h>   // QueryPerformanceCounter(), GetProcessTimes()
    #include <sys/timeb.h> // ftime(struct timeb *)
#else
    #error "Don't know how to get nanosecond time for this platform"
#endif

#if defined(BSLS_PLATFORM_OS_SOLARIS)
    #include <sys/time.h>           // gethrtime()
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    #include <stdlib.h>             // abort()
    #include <limits.h>             // LLONG_MIN
    #include <mach/mach.h>          // mach_thread_self(), thread_info()
    #include <mach/mach_init.h>     // mach_thread_self()
    #include <mach/thread_act.h>    // thread_info(), THREAD_BASIC_INFO
#endif

namespace BloombergLP {

namespace {


#ifdef BSLS_PLATFORM_OS_UNIX

/// Provides access to UNIX process user and system timers.
struct UnixTimerUtil {

  private:
    // CLASS DATA
    static const bsls::Types::Int64 s_nsecsPerSecond;
    static const bsls::Types::Int64 s_nsecsPerMicrosecond;
  private:
    // PRIVATE CLASS METHODS

    /// Convert the specified `seconds` and `microseconds` to nanoseconds
    /// and return the result.  The behavior is undefined unless
    /// `seconds >= 0` and `0 <= microseconds < 1000000`.
    template <class t_SECONDS, class t_MICROSECONDS>
    static bsls::Types::Int64 toNanoseconds(t_SECONDS      seconds,
                                            t_MICROSECONDS microseconds);

    /// Helper routine to be used by the public methods below: call the OS APIs
    /// and handle errors.
    static void systemProcessTimers(bsls::Types::Int64 *systemTimer,
                                    bsls::Types::Int64 *userTimer);

    /// Helper routine to be used by the public thread-timer methods below:
    /// call the OS APIs and handle errors.  On platforms where per-thread
    /// CPU times are unavailable, load -1 into both timers.
    static void systemThreadTimers(bsls::Types::Int64 *systemTimer,
                                   bsls::Types::Int64 *userTimer);

  public:
    // CLASS METHODS

    /// Return converted to nanoseconds current value of system time as
    /// returned by `getrusage()` if the call succeeds, and -1 otherwise.
    static bsls::Types::Int64 systemTimer();

    /// Return converted to nanoseconds current value of user time as
    /// returned by `getrusage()` if the call succeeds, and -1 otherwise.
    static bsls::Types::Int64 userTimer();

    /// Return converted to nanoseconds current values of system and user times
    /// as returned by `getrusage()` if the call succeeds, and -1 otherwise.
    static void processTimers(bsls::Types::Int64 *systemTimer,
                              bsls::Types::Int64 *userTimer);

    /// Return converted to nanoseconds the current calling thread's system CPU
    /// time as reported by the platform's per-thread timing API, and -1 if the
    /// call fails or per-thread CPU times are unavailable on this platform.
    static bsls::Types::Int64 threadSystemTimer();

    /// Return converted to nanoseconds the current calling thread's user CPU
    /// time as reported by the platform's per-thread timing API, and -1 if the
    /// call fails or per-thread CPU times are unavailable on this platform.
    static bsls::Types::Int64 threadUserTimer();

    /// Load into the specified `systemTimer` and `userTimer` the current
    /// calling thread's system and user CPU times in nanoseconds as reported
    /// by the platform's per-thread timing API, and -1 values if the call
    /// fails or per-thread CPU times are unavailable on this platform.
    static void threadTimers(bsls::Types::Int64 *systemTimer,
                             bsls::Types::Int64 *userTimer);
};

const bsls::Types::Int64 UnixTimerUtil::s_nsecsPerSecond = 1000 * 1000 * 1000;
const bsls::Types::Int64 UnixTimerUtil::s_nsecsPerMicrosecond = 1000;

template <class t_SECONDS, class t_MICROSECONDS>
inline
bsls::Types::Int64 UnixTimerUtil::toNanoseconds(t_SECONDS      seconds,
                                                t_MICROSECONDS microseconds)
{
    BSLS_ASSERT_SAFE(0 <= seconds);
    BSLS_ASSERT_SAFE(0 <= microseconds && microseconds < 1000000);

    typedef bsls::Types::Int64 Int64;
    return static_cast<Int64>(seconds)      * s_nsecsPerSecond +
           static_cast<Int64>(microseconds) * s_nsecsPerMicrosecond;
}

inline
void UnixTimerUtil::systemProcessTimers(bsls::Types::Int64 *systemTimer,
                                        bsls::Types::Int64 *userTimer)
{
    struct rusage usage;

    int rc = getrusage(RUSAGE_SELF, &usage);
    (void) rc;                   // silence unused variable warning

    BSLS_ASSERT_SAFE(-1 != rc);  // Sanity check for validity of `RUSAGE_SELF`
                                 // and `&usage`.  Possible errors all require
                                 // invalid input to `getrusage`.

    *systemTimer = toNanoseconds(usage.ru_stime.tv_sec,
                                 usage.ru_stime.tv_usec);

    *userTimer = toNanoseconds(usage.ru_utime.tv_sec,
                               usage.ru_utime.tv_usec);
}

inline
void UnixTimerUtil::systemThreadTimers(bsls::Types::Int64 *systemTimer,
                                       bsls::Types::Int64 *userTimer)
{
#if defined(BSLS_PLATFORM_OS_DARWIN)
    // On Darwin, `getrusage` does not support per-thread queries; use the
    // Mach kernel's `thread_info` instead.

    mach_port_t thread = mach_thread_self();
    thread_basic_info_data_t info;
    mach_msg_type_number_t   count = THREAD_BASIC_INFO_COUNT;

    kern_return_t kr = thread_info(thread,
                                   THREAD_BASIC_INFO,
                                   reinterpret_cast<thread_info_t>(&info),
                                   &count);

    // `mach_thread_self` returns a send right which must be released.
    (void)mach_port_deallocate(mach_task_self(), thread);

    if (KERN_SUCCESS != kr) {
        *systemTimer = -1;
        *userTimer   = -1;
        return;                                                       // RETURN
    }

    *systemTimer = toNanoseconds(info.system_time.seconds,
                                 info.system_time.microseconds);

    *userTimer = toNanoseconds(info.user_time.seconds,
                               info.user_time.microseconds);

#elif defined(RUSAGE_THREAD)
    // Linux (since 2.6.26), FreeBSD (since 8.1), and other UNIXes that
    // define `RUSAGE_THREAD` (NetBSD, OpenBSD).

    struct rusage usage;

    int rc = getrusage(RUSAGE_THREAD, &usage);
    if (0 != rc) {
        *systemTimer = -1;
        *userTimer   = -1;
        return;                                                       // RETURN
    }

    *systemTimer = toNanoseconds(usage.ru_stime.tv_sec,
                                 usage.ru_stime.tv_usec);

    *userTimer = toNanoseconds(usage.ru_utime.tv_sec,
                               usage.ru_utime.tv_usec);

#elif defined(RUSAGE_LWP)
    // Solaris exposes per-LWP (lightweight process, i.e., kernel thread)
    // resource usage via `RUSAGE_LWP`.

    struct rusage usage;

    int rc = getrusage(RUSAGE_LWP, &usage);
    if (0 != rc) {
        *systemTimer = -1;
        *userTimer   = -1;
        return;                                                       // RETURN
    }

    *systemTimer = toNanoseconds(usage.ru_stime.tv_sec,
                                 usage.ru_stime.tv_usec);

    *userTimer = toNanoseconds(usage.ru_utime.tv_sec,
                               usage.ru_utime.tv_usec);

#else
    // No per-thread CPU-time API known on this UNIX (e.g., AIX, HP-UX,
    // Cygwin).  Report -1 so callers can detect the situation.

    *systemTimer = -1;
    *userTimer   = -1;
#endif
}

inline
bsls::Types::Int64 UnixTimerUtil::systemTimer()
{
    bsls::Types::Int64 sTimer;
    bsls::Types::Int64 dummy;

    systemProcessTimers(&sTimer, &dummy);

    return sTimer;
}

inline
bsls::Types::Int64 UnixTimerUtil::userTimer()
{
    bsls::Types::Int64 uTimer;
    bsls::Types::Int64 dummy;

    systemProcessTimers(&dummy, &uTimer);

    return uTimer;
}

inline
void UnixTimerUtil::processTimers(bsls::Types::Int64 *systemTimer,
                                  bsls::Types::Int64 *userTimer)
{
    systemProcessTimers(systemTimer, userTimer);
}

inline
bsls::Types::Int64 UnixTimerUtil::threadSystemTimer()
{
    bsls::Types::Int64 sTimer;
    bsls::Types::Int64 dummy;

    systemThreadTimers(&sTimer, &dummy);

    return sTimer;
}

inline
bsls::Types::Int64 UnixTimerUtil::threadUserTimer()
{
    bsls::Types::Int64 uTimer;
    bsls::Types::Int64 dummy;

    systemThreadTimers(&dummy, &uTimer);

    return uTimer;
}

inline
void UnixTimerUtil::threadTimers(bsls::Types::Int64 *systemTimer,
                                 bsls::Types::Int64 *userTimer)
{
    systemThreadTimers(systemTimer, userTimer);
}
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
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
                                                // by `GetProcessTimes()`

  private:
    // PRIVATE CLASS METHODS

    /// Helper routine to be used by the public methods below: call the OS
    /// APIs and handle errors by returning a non-zero value in case of an
    /// error.
    static int systemProcessTimers(PULARGE_INTEGER systemTimer,
                                   PULARGE_INTEGER userTimer);

    /// Helper routine to be used by the public thread-timer methods below:
    /// call `GetThreadTimes` on the current thread.  Report errors by
    /// returning a non-zero value.
    static int systemThreadTimers(PULARGE_INTEGER systemTimer,
                                  PULARGE_INTEGER userTimer);

  public:
    // CLASS METHODS

    /// Initialize the static data used by `WindowsTimeUtil` (currently the
    /// `s_initialTime` and the `s_timerFrequency` values).
    static void initialize();

    /// Return converted to nanoseconds current value of kernel (system) time
    /// as returned by `GetProcessTimes()` if the call succeeds, and -1
    /// otherwise. The behavior is undefined unless `initialize` has been
    /// called.
    static bsls::Types::Int64 systemTimer();

    /// Return converted to nanoseconds current value of user time as returned
    /// by `GetProcessTimes()` if the call succeeds, and -1 otherwise.  The
    /// behavior is undefined unless `initialize` has been called.
    static bsls::Types::Int64 userTimer();

    /// Return converted to nanoseconds current value of kernel (system) and
    /// user times as returned by `GetProcessTimes()` if the call succeeds, and
    /// -1 otherwise.  The behavior is undefined unless `initialize` has been
    /// called.
    static void processTimers(bsls::Types::Int64 *systemTimer,
                              bsls::Types::Int64 *userTimer);

    /// Return converted to nanoseconds current value of kernel (system) time
    /// as returned by `GetThreadTimes()` on the current thread if the call
    /// succeeds, and -1 otherwise.
    static bsls::Types::Int64 threadSystemTimer();

    /// Return converted to nanoseconds current value of user time as returned
    /// by `GetThreadTimes()` on the current thread if the call succeeds, and
    /// -1 otherwise.
    static bsls::Types::Int64 threadUserTimer();

    /// Return converted to nanoseconds current value of kernel (system) and
    /// user times as returned by `GetThreadTimes()` on the current thread if
    /// the call succeeds, and -1 otherwise.
    static void threadTimers(bsls::Types::Int64 *systemTimer,
                             bsls::Types::Int64 *userTimer);

    /// Return converted to nanoseconds current value of wall time as per
    /// Windows hardware timer, if available, uses `::ftime` otherwise.  The
    /// behavior is undefined unless `initialize` has been called.
    static bsls::Types::Int64 wallTimer();

    /// Return a machine-dependent value representing the current time.
    /// `timeValue` must be converted by the `convertRawTime` method to
    /// conventional units (nanoseconds).  This method is intended to
    /// facilitate accurate timing of small segments of code, and care must be
    /// used in interpreting the results.  The behavior is undefined unless
    /// `initialize` has been called.  Note that this method is thread-safe
    /// only if `initialize` has been called before.
    static bsls::Types::Int64 getTimerRaw();

    /// Convert the specified `rawTime` to a value in nanoseconds, referenced
    /// to an arbitrary but fixed origin, and return the result of the
    /// conversion. The behavior is undefined unless `initialize` has been
    /// called.  Note that this method is thread-safe only if `initialize` has
    /// been called before.
    static bsls::Types::Int64 convertRawTime(bsls::Types::Int64 rawTime);
};

bsls::AtomicOperations::AtomicTypes::Int
                                     WindowsTimerUtil::s_initRequired   = { 1};
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
int WindowsTimerUtil::systemProcessTimers(PULARGE_INTEGER systemTimer,
                                          PULARGE_INTEGER userTimer)
{
    FILETIME creationTime, exitTime, kernelTime, userTime;

    if (!::GetProcessTimes(::GetCurrentProcess(),
                           &creationTime,
                           &exitTime,
                           &kernelTime,
                           &userTime)) {
        return -1;                                                    // RETURN
    }

    systemTimer->LowPart  = kernelTime.dwLowDateTime;
    systemTimer->HighPart = kernelTime.dwHighDateTime;
    userTimer->LowPart    = userTime.dwLowDateTime;
    userTimer->HighPart   = userTime.dwHighDateTime;

    return 0;
}

inline
void WindowsTimerUtil::initialize()
{
    const bsls::Types::Int64 K = 1000;
    const bsls::Types::Int64 G = K * K * K;
    const bsls::Types::Int64 HIGH_DWORD_MULTIPLIER = G * (1LL << 32);

    if (bsls::AtomicOperations::getIntAcquire(&s_initRequired)) {

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

        bsls::AtomicOperations::setIntRelease(&s_initRequired, 0);
    }
}

inline
bsls::Types::Int64 WindowsTimerUtil::systemTimer()
{
    ULARGE_INTEGER sTimer, dummy;
    if (0 != systemProcessTimers(&sTimer, &dummy)) {
        return -1;                                                    // RETURN
    }

    return sTimer.QuadPart * s_nsecsPerUnit;
}

inline
bsls::Types::Int64 WindowsTimerUtil::userTimer()
{
    ULARGE_INTEGER dummy, uTimer;
    if (0 != systemProcessTimers(&dummy, &uTimer)) {
        return -1;                                                    // RETURN
    }

    return uTimer.QuadPart * s_nsecsPerUnit;
}

inline
void WindowsTimerUtil::processTimers(bsls::Types::Int64 *systemTimer,
                                     bsls::Types::Int64 *userTimer)
{
    ULARGE_INTEGER sTimer, uTimer;
    if (0 != systemProcessTimers(&sTimer, &uTimer)) {
        *systemTimer = -1;
        *userTimer   = -1;
        return;                                                       // RETURN
    }

    *systemTimer = static_cast<bsls::Types::Int64>(sTimer.QuadPart)
                                                              * s_nsecsPerUnit;
    *userTimer = static_cast<bsls::Types::Int64>(uTimer.QuadPart)
                                                              * s_nsecsPerUnit;
}

inline
int WindowsTimerUtil::systemThreadTimers(PULARGE_INTEGER systemTimer,
                                         PULARGE_INTEGER userTimer)
{
    FILETIME creationTime, exitTime, kernelTime, userTime;

    if (!::GetThreadTimes(::GetCurrentThread(),
                          &creationTime,
                          &exitTime,
                          &kernelTime,
                          &userTime)) {
        return -1;                                                    // RETURN
    }

    systemTimer->LowPart  = kernelTime.dwLowDateTime;
    systemTimer->HighPart = kernelTime.dwHighDateTime;
    userTimer->LowPart    = userTime.dwLowDateTime;
    userTimer->HighPart   = userTime.dwHighDateTime;

    return 0;
}

inline
bsls::Types::Int64 WindowsTimerUtil::threadSystemTimer()
{
    bsls::Types::Int64 sTimer, uTimer;
    threadTimers(&sTimer, &uTimer);
    return sTimer;
}

inline
bsls::Types::Int64 WindowsTimerUtil::threadUserTimer()
{
    bsls::Types::Int64 sTimer, uTimer;
    threadTimers(&sTimer, &uTimer);
    return uTimer;
}

inline
void WindowsTimerUtil::threadTimers(bsls::Types::Int64 *systemTimer,
                                    bsls::Types::Int64 *userTimer)
{
    ULARGE_INTEGER sTimer, uTimer;
    if (0 != systemThreadTimers(&sTimer, &uTimer)) {
        *systemTimer = -1;
        *userTimer = -1;
        return;                                                       // RETURN
    }

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
        // Not implemented: Assert that rawTime - initialTime will fit in an
        // Int64, when expressed as nanoseconds (~292 days).
        //
        // N.B. This assert is not implemented because we cannot use
        // std::numeric_limits here.
        //
        // If it were implemented, it would look like the following:
        // BSLS_ASSERT(((rawTime - initialTime) / timerFrequency)
        //             < std::numeric_limits<bsls::Types::Int64>::max() / G)

        bsls::Types::Int64 timerFrequency
            = bsls::AtomicOperations::getInt64Relaxed(&s_timerFrequency);

        bsls::Types::Int64 highPartDivisionFactor
            = bsls::AtomicOperations::getInt64Relaxed(
                                                    &s_highPartDivisionFactor);
        bsls::Types::Int64 highPartRemainderFactor
            = bsls::AtomicOperations::getInt64Relaxed(
                                                   &s_highPartRemainderFactor);

        rawTime -= initialTime;

        // Outline for improved-precision nanosecond calculation with integer
        // arithmetic:

        // Treat the high-dword and low-dword contributions to the counter as
        // separate 64-bit values.  Since all known timerFrequency values fit
        // inside 32 unsigned bits, the high-dword contribution will retain 32
        // significant bits after being divided by the timerFrequency.
        // Therefore the calculation can be done on the high-dword contribution
        // by dividing first by the frequency and then multiplying by one
        // billion.  Then the remainder of the division by frequency can be
        // calculated and added back to the low part of the result, for
        // complete accuracy down to the nanosecond.  Similarly, one billion
        // fits inside signed 32 bits, so the low-dword contribution can be
        // multiplied by one billion without overflowing.  Therefore, the
        // calculation can be done on the low-dword contribution by multiplying
        // first by one billion, and then dividing by the frequency.  Finally,
        // the whole calculation can be sped up by pre-calculating the parts of
        // the calculation that involve frequency and constants, and caching
        // the results at initialization time.

        const bsls::Types::Int64 high32Bits =
            static_cast<bsls::Types::Int64>(rawTime >> 32);
        const bsls::Types::Uint64 low32Bits  =
            static_cast<bsls::Types::Uint64> (rawTime & LOW_MASK);

        return high32Bits * highPartDivisionFactor +
              (high32Bits * highPartRemainderFactor) / timerFrequency +
              (low32Bits * G) / timerFrequency;                       // RETURN

        // Note that this code runs as fast as the original implementation.  It
        // works for counters representing time values up to 292 years (the
        // upper limit for representing nanoseconds in 64 bits), and for any
        // frequency that fits in 32 bits.  The original implementation broke
        // down on counter values over ~9x10^12, which could be as little as 50
        // minutes with a 3GHz clock.

        // Another alternative is to use floating-point arithmetic.  This is
        // just as fast as the integer arithmetic on my development machine.
        // On the other hand, it is accurate to only 15 decimal places, which
        // will affect our resolution on timers that have been running for more
        // than 11 days.

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

#ifdef BSLS_PLATFORM_OS_DARWIN

struct DarwinTimerUtil {
    // Provides access to high-resolution Mach kernel timer

  private:
    // CLASS DATA
    static bsls::Types::Uint64 s_initialTime;  // initial time for the Mach
                                               // hardware timer

  public:
    // CLASS METHODS

    /// Return converted to nanoseconds the current uptime as reported by
    /// `clock_gettime_nsec_np`.
    static bsls::Types::Uint64 getNanosecondsUptime();

    /// Initialize the static data used by `DarwinTimerUtil` (currently the
    /// `s_initialTime` value).
    static void initialize();

    /// Return a machine-dependent value representing the current time.
    static bsls::Types::Uint64 getTimerRaw();
};

bsls::Types::Uint64 DarwinTimerUtil::s_initialTime    = {0};

inline
bsls::Types::Uint64 DarwinTimerUtil::getNanosecondsUptime()
{
    // An earlier implementation used `mach_absolute_time()`, which is
    // considered deprecated (since it requires scaling on Apple Silicon).
    //
    // * https://developer.apple.com/documentation/kernel/1462446-mach_absolute_time

    return static_cast<bsls::Types::Uint64>(
            clock_gettime_nsec_np(CLOCK_UPTIME_RAW));
}

inline
void DarwinTimerUtil::initialize()
{
    static bsls::BslOnce once = BSLS_BSLONCE_INITIALIZER;

    bsls::BslOnceGuard onceGuard;
    if (onceGuard.enter(&once)) {

        s_initialTime = getNanosecondsUptime();
    }
}

inline
bsls::Types::Uint64 DarwinTimerUtil::getTimerRaw()
{
    initialize();

    return static_cast<bsls::Types::Uint64>(
                                       getNanosecondsUptime() - s_initialTime);
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
#if defined BSLS_PLATFORM_OS_UNIX
#  if defined BSLS_PLATFORM_OS_DARWIN
    DarwinTimerUtil::initialize();
#  endif
#elif defined BSLS_PLATFORM_OS_WINDOWS
    WindowsTimerUtil::initialize();
#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

Types::Int64
TimeUtil::convertRawTime(TimeUtil::OpaqueNativeTime rawTime)
{
#if defined BSLS_PLATFORM_OS_SOLARIS

    return rawTime.d_opaque;

#elif defined BSLS_PLATFORM_OS_AIX

    // Imp Note:
    // `time_base_to_time` takes much more time (~1.2 usec) than actually
    // collecting the raw time in the first place (<100 nsec).

    const Types::Int64 G = 1000000000;
    time_base_to_time(&rawTime, TIMEBASE_SZ);
    return (Types::Int64) rawTime.tb_high * G + rawTime.tb_low;

#elif defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_CYGWIN)     \
   || defined(BSLS_PLATFORM_OS_EMSCRIPTEN)

    const Types::Int64 G = 1000000000;
    return ((Types::Int64) rawTime.tv_sec * G + rawTime.tv_nsec);

#elif defined BSLS_PLATFORM_OS_DARWIN

    return rawTime.d_opaque;

#elif defined BSLS_PLATFORM_OS_UNIX

    const Types::Int64 K = 1000;
    const Types::Int64 M = 1000000;
    return ((Types::Int64) rawTime.tv_sec * M + rawTime.tv_usec) * K;

#elif defined BSLS_PLATFORM_OS_WINDOWS

    return WindowsTimerUtil::convertRawTime(rawTime.d_opaque);

#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

Types::Int64 TimeUtil::getTimer()
{
#if defined BSLS_PLATFORM_OS_SOLARIS

    // Call the platform-specific function directly, since it is reliable and
    // requires no conversion.

    return gethrtime();

#elif defined BSLS_PLATFORM_OS_AIX    || \
      defined BSLS_PLATFORM_OS_LINUX  || \
      defined BSLS_PLATFORM_OS_DARWIN || \
      defined BSLS_PLATFORM_OS_UNIX

    TimeUtil::OpaqueNativeTime rawTime;
    getTimerRaw(&rawTime);
    return convertRawTime(rawTime);

#elif defined BSLS_PLATFORM_OS_WINDOWS

    return WindowsTimerUtil::wallTimer();

#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

void TimeUtil::getTimerRaw(TimeUtil::OpaqueNativeTime *timeValue)
{
    // Historical Note: Older Sun machines (e.g., sundev2 circa 2003) exhibited
    // intermittent non-compliant (i.e., non-monotonic) behavior for function
    // `gethrtime`.  As of July 2004, no non-monotonic behavior has been seen
    // on any Sun machine.  The imp note in the next paragraph is no longer
    // valid, but is retained for archival purposes, and as a historical
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

#if defined BSLS_PLATFORM_OS_SOLARIS

    // `gethrtime` has not been observed to fail on any `sundev` machine.

    timeValue->d_opaque = gethrtime();

#elif defined BSLS_PLATFORM_OS_AIX

    // Imp Note:
    // `read_wall_time` is very fast (<100 nsec), and guaranteed monotonic per
    // http://publib.boulder.ibm.com/infocenter/systems doc.  It has not been
    // observed to be non-monotonic when tested to better than 3 parts in 10^10
    // on an ibm dev host (the deprecated ibmTwo).  Converting the time to
    // nanoseconds is much slower (~1.2 usec).

    read_wall_time(timeValue, TIMEBASE_SZ);

#elif defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_CYGWIN)     \
   || defined(BSLS_PLATFORM_OS_EMSCRIPTEN)

    // The call to `clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts)` has never
    // been observed to be non-monotonic when tested at better than 1 parts in
    // 10^9 on a linux dev host (the deprecated linxdevFive), in a tight loop.
    // However, not all users will own the process, and for multi-processor
    // systems the value returned may be meaningless.  Therefore, for the
    // standard timer, the `CLOCK_MONOTONIC` clock is preferred.
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

#elif defined BSLS_PLATFORM_OS_DARWIN

    timeValue->d_opaque = DarwinTimerUtil::getTimerRaw();

#elif defined BSLS_PLATFORM_OS_UNIX

    // A generic implementation having microsecond resolution is used.  There
    // is no attempt to defend against possible non-monotonic
    // behavior. Together with the arithmetic to convert `timeValue` to
    // nanoseconds, the imp would cause `bsls_stopwatch` to profile at
    // ~1.40 usec on AIX when compiled with
    // /bb/util/version10-062009/usr/vacpp/bin/xlC_r.

    gettimeofday(timeValue, 0);

#elif defined BSLS_PLATFORM_OS_WINDOWS

    timeValue->d_opaque = WindowsTimerUtil::getTimerRaw();

#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

Types::Int64 TimeUtil::getProcessSystemTimer()
{
#if defined BSLS_PLATFORM_OS_UNIX
    return UnixTimerUtil::systemTimer();
#elif defined BSLS_PLATFORM_OS_WINDOWS
    return WindowsTimerUtil::systemTimer();
#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

Types::Int64 TimeUtil::getProcessUserTimer()
{
#if defined BSLS_PLATFORM_OS_UNIX
    return UnixTimerUtil::userTimer();
#elif defined BSLS_PLATFORM_OS_WINDOWS
    return WindowsTimerUtil::userTimer();
#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

void TimeUtil::getProcessTimers(Types::Int64 *systemTimer,
                                Types::Int64 *userTimer)
{
#if defined BSLS_PLATFORM_OS_UNIX
    UnixTimerUtil::processTimers(systemTimer, userTimer);
#elif defined BSLS_PLATFORM_OS_WINDOWS
    WindowsTimerUtil::processTimers(systemTimer, userTimer);
#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

Types::Int64 TimeUtil::getThreadSystemTimer()
{
#if defined BSLS_PLATFORM_OS_UNIX
    return UnixTimerUtil::threadSystemTimer();
#elif defined BSLS_PLATFORM_OS_WINDOWS
    return WindowsTimerUtil::threadSystemTimer();
#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

Types::Int64 TimeUtil::getThreadUserTimer()
{
#if defined BSLS_PLATFORM_OS_UNIX
    return UnixTimerUtil::threadUserTimer();
#elif defined BSLS_PLATFORM_OS_WINDOWS
    return WindowsTimerUtil::threadUserTimer();
#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

void TimeUtil::getThreadTimers(Types::Int64 *systemTimer,
                               Types::Int64 *userTimer)
{
#if defined BSLS_PLATFORM_OS_UNIX
    UnixTimerUtil::threadTimers(systemTimer, userTimer);
#elif defined BSLS_PLATFORM_OS_WINDOWS
    WindowsTimerUtil::threadTimers(systemTimer, userTimer);
#else
    #error "Don't know how to get nanosecond time for this platform"
#endif
}

}  // close package namespace

}  // close enterprise namespace

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
