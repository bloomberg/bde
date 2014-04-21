// bdetu_systemtime.cpp                                               -*-C++-*-
#include <bdetu_systemtime.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdetu_systemtime_cpp,"$Id$ $CSID$")

#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bslmf_assert.h>

// Before we include the unix-specific header, '<sys/time.h>', we include the
// standard header, '<time.h>', to ensure that things like 'time_t' end up in
// both the 'std' namespace and the global namespace. (Critical for Sunpro.)
#include <bsl_c_time.h>

#include <bsl_c_limits.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    #include <bsls_timeutil.h>
    #include <windows.h>
#else
    #include <bsl_c_sys_time.h>
    #if defined(BSLS_PLATFORM_OS_AIX)
        #include <sys/systemcfg.h>
    #elif defined(BSLS_PLATFORM_OS_DARWIN)
        #include <mach/clock.h>
        #include <mach/mach.h>
    #endif
#endif

namespace BloombergLP {
                            // ----------------------
                            // class bdetu_SystemTime
                            // ----------------------

// DATA
bdetu_SystemTime::SystemTimeCallback
bdetu_SystemTime::s_systime_callback_p =
                                       bdetu_SystemTime::loadSystemTimeDefault;

bdetu_SystemTime::LoadLocalTimeOffsetCallback
bdetu_SystemTime::s_loadLocalTimeOffsetCallback_p =
                                  bdetu_SystemTime::loadLocalTimeOffsetDefault;

// CLASS METHODS

                        // ** now methods **

#if defined(BSLS_PLATFORM_OS_DARWIN)

static const int k_UNINITIALIZED_CLOCK = 0;
static bsls::AtomicOperations::AtomicTypes::Int g_calendarClock =
                                                     { k_UNINITIALIZED_CLOCK };
static bsls::AtomicOperations::AtomicTypes::Int g_realtimeClock =
                                                     { k_UNINITIALIZED_CLOCK };

class MachClockGuard {
    // A guard that deallocates a Darwin (mach kernel) 'clock_serv_t' on its
    // destruction.

    // DATA
    bsls::AtomicOperations::AtomicTypes::Int& d_clock;  // clock identifier

  private:
    // NOT IMPLEMENTED
    MachClockGuard(const MachClockGuard&);
    MachClockGuard operator=(const MachClockGuard&);
  public:

    // CREATORS
    explicit MachClockGuard(bsls::AtomicOperations::AtomicTypes::Int& clock)
        : d_clock(clock) {}

    ~MachClockGuard()  
    {
        int clock = d_clock.swap(k_UNINITIALIZED_CLOCK);
        if (clock != k_UNINITIALIZED_CLOCK) {
            mach_port_deallocate(mach_task_self(),
                                 static_cast<clock_serv_t>(clock));
        }
    }
};

static MachClockGuard g_calendarClockGuard(g_calendarClock);
static MachClockGuard g_realtimeClockGuard(g_realtimeClock);

static
clock_serv_t getClockService(clock_id_t       clockId,
                             bsls::AtomicInt& atomicClockStore)
    // Return a MACH clock service handle for the specified 'clockId'.  Use the
    // specified 'atomicClockStore' for single-time initialization of the
    // clock service handle.
{
    BSLMF_ASSERT(sizeof(clock_serv_t) == sizeof(int));

    // One-time initialization (note: the sucessfully initialized clock service
    // is released at the task's destruction).

    if (k_UNINITIALIZED_CLOCK == atomicClockStore.loadAcquire()) {
        clock_serv_t clockServ;
        kern_return_t rc = host_get_clock_service(mach_host_self(),
                                                  clockId,
                                                  &clockServ);
        (void) rc; BSLS_ASSERT_OPT(KERN_SUCCESS == rc);

        if (atomicClockStore.testAndSwap(k_UNINITIALIZED_CLOCK,
                                         static_cast<int>(clockServ))
            != k_UNINITIALIZED_CLOCK)
        {
            // atomicClockStore was already initialized by another thread,
            // release clockServ.

            rc = mach_port_deallocate(mach_task_self(), clockServ);
            (void) rc; BSLS_ASSERT_OPT(KERN_SUCCESS == rc);
        }
    }

    return static_cast<clock_serv_t>(atomicClockStore.loadRelaxed());
}

static inline
bdet_TimeInterval getNowTime(clock_id_t       clockId,
                             bsls::AtomicInt& atomicClockStore)
    // Return the current time for the specified 'clockId' using the specified
    // 'atomicClockStore' as a storage for an appropriate MACH clock service
    // handle.
{
    mach_timespec_t mts;
    clock_serv_t clockServ = getClockService(clockId, atomicClockStore);
    clock_get_time(clockServ, &mts);
    return bdet_TimeInterval(mts.tv_sec, mts.tv_nsec);
}

bdet_TimeInterval bdetu_SystemTime::nowRealtimeClock()
    // Return the current time for the realtime clock.
{
    return getNowTime(CALENDAR_CLOCK, g_calendarClock);
}

bdet_TimeInterval bdetu_SystemTime::nowMonotonicClock()
    // Return the current time for the monotonic clock.
{
    return getNowTime(REALTIME_CLOCK, g_realtimeClock);
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

const unsigned int k_TicksPerSecond = 10000000;
static const bsls::Types::Uint64 k_TicksToWindowsEpoch
                                            = 11644473600LL * k_TicksPerSecond;
                                        // Windows epoch starts from Jan 1 1601
                                        // the offset is 11644473600 seconds
const unsigned int k_NanosecondsPerTick = 100;
const unsigned int k_NanosecondsPerSecond = 1000000000;

static bsls::AtomicOperations::AtomicTypes::Int64 g_frequency = { 0 };

static inline
bsls::Types::Int64 getFrequency()
    // Return the frequency of the Windows monotonic performance counter.
{
    // We can use relaxed atomic operations throughout, it only matters that
    // g_frequency is updated atomically.

    if (0 == bsls::AtomicOperations::getInt64Relaxed(&g_frequency)) {
        LARGE_INTEGER freq;

        BOOL rc = QueryPerformanceFrequency(&freq);
        (void) rc; BSLS_ASSERT(rc);

        bsls::AtomicOperations::setInt64Relaxed(&g_frequency, freq.QuadPart);
    }

    return bsls::AtomicOperations::getInt64Relaxed(&g_frequency);
}

bdet_TimeInterval bdetu_SystemTime::nowRealtimeClock()
    // Return the current time for the realtime clock.
{
    ULARGE_INTEGER fileTime;
    GetSystemTimeAsFileTime(reinterpret_cast<FILETIME *>(&fileTime));
    bsls::Types::Uint64 ticksSinceEpoch
                                   = fileTime.QuadPart - k_TicksToWindowsEpoch;
    return bdet_TimeInterval(ticksSinceEpoch / k_TicksPerSecond,
                             ticksSinceEpoch % k_TicksPerSecond
                                                       * k_NanosecondsPerTick);
}

bdet_TimeInterval bdetu_SystemTime::nowMonotonicClock()
    // Return the current time for the monotonic clock.
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    bsls::Types::Int64 freq = getFrequency();

    bsls::Types::Int64 nanosec =
        (bsls::Types::Int64) (double(counter.QuadPart) /
                                                freq * k_NanosecondsPerSecond);
    return bdet_TimeInterval(nanosec / k_NanosecondsPerSecond, 
                             nanosec % k_NanosecondsPerSecond);
}

#else

static inline
bdet_TimeInterval getNowTime(clockid_t clockId)
    // Return the current time for the specified 'clockId'.
{
    timespec tp = {0};
    int rc = clock_gettime(clockId, &tp);
    (void) rc; BSLS_ASSERT(0 == rc);

    return bdet_TimeInterval(tp.tv_sec, tp.tv_nsec);
}

bdet_TimeInterval bdetu_SystemTime::nowMonotonicClock()
    // Return the current time for the monotonic clock.
{
    return getNowTime(CLOCK_MONOTONIC);
}

bdet_TimeInterval bdetu_SystemTime::nowRealtimeClock()
    // Return the current time for the realtime clock.
{
    return getNowTime(CLOCK_REALTIME);
}

#endif

bdet_Datetime bdetu_SystemTime::nowAsDatetimeLocal()
{
    int           offsetInSeconds;
    bdet_Datetime now = nowAsDatetimeUtc();

    (*s_loadLocalTimeOffsetCallback_p)(&offsetInSeconds, now);

    now.addSeconds(offsetInSeconds);

    return now;
}

                       // ** default callbacks **

void bdetu_SystemTime::loadLocalTimeOffsetDefault(
                                             int                  *result,
                                             const bdet_Datetime&  utcDatetime)
{
    BSLS_ASSERT(result);

    bsl::time_t currentTime;
    int         status = bdetu_Epoch::convertToTimeT(&currentTime,
                                                     utcDatetime);
    BSLS_ASSERT(0 == status);

    struct tm localTm;
    struct tm   gmtTm;
#if defined(BSLS_PLATFORM_OS_WINDOWS) || ! defined(BDE_BUILD_TARGET_MT)
    localTm = *localtime(&currentTime);
      gmtTm =    *gmtime(&currentTime);
#else
    localtime_r(&currentTime, &localTm);
       gmtime_r(&currentTime,   &gmtTm);
#endif

    bdet_Datetime localDatetime;
    bdet_Datetime   gmtDatetime;
    bdetu_Datetime::convertFromTm(&localDatetime, localTm);
    bdetu_Datetime::convertFromTm(  &gmtDatetime,   gmtTm);

    bdet_DatetimeInterval offset = localDatetime - gmtDatetime;

    BSLS_ASSERT(offset.totalSeconds() < INT_MAX);
    *result = offset.totalSeconds();
}

void bdetu_SystemTime::loadSystemTimeDefault(bdet_TimeInterval *result)
{
    BSLS_ASSERT(result);

#ifdef BSLS_PLATFORM_OS_WINDOWS
    bsls::Types::Uint64 t;
    GetSystemTimeAsFileTime((FILETIME*)&t);
    t -= k_TicksToWindowsEpoch; // windows epoch -> unix epoch
    result->setInterval(t / k_TicksPerSecond,
                        (t % k_TicksPerSecond) * k_NanosecondsPerTick);
#elif BSLS_PLATFORM_OS_LINUX
    struct timespec tp;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                         clock_gettime(CLOCK_REALTIME, &tp))) {
          result->setInterval(0, 0);
          return;
    }
    result->setInterval(tp.tv_sec, tp.tv_nsec);
#elif BSLS_PLATFORM_OS_AIX
    timebasestruct_t tb;
    read_real_time(&tb, TIMEBASE_SZ);
    time_base_to_time(&tb, TIMEBASE_SZ);
    result->setInterval(tb.tb_high, tb.tb_low);
#else
    timeval time_val;
    if (gettimeofday(&time_val, NULL) != 0) {
         result->setInterval(0, 0);
    }
    else {
        result->setInterval(time_val.tv_sec, time_val.tv_usec * 1000);
    }
#endif
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
