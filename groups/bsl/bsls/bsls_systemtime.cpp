// bsls_systemtime.cpp                                                -*-C++-*-
#include <bsls_systemtime.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_atomicoperations.h>
#include <bsls_bsltestutil.h>        // for testing only
#include <bsls_platform.h>
#include <bsls_types.h>

// Before we include the Unix-specific header, '<sys/time.h>', we include the
// standard header, '<time.h>', to ensure that things like 'time_t' end up in
// both the 'std' namespace and the global namespace. (Critical for Sunpro.)
#include <time.h>

#include <limits.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    #include <bsls_timeutil.h>
    #include <windows.h>
#else
    #include <sys/time.h>
    #if defined(BSLS_PLATFORM_OS_AIX)
        #include <sys/systemcfg.h>
    #elif defined(BSLS_PLATFORM_OS_DARWIN)
        #include <mach/clock.h>
        #include <mach/mach.h>
    #endif
#endif

namespace BloombergLP {
namespace bsls {

                            // -----------------
                            // struct SystemTime
                            // -----------------

// CLASS METHODS

#if defined(BSLS_PLATFORM_OS_DARWIN)

                            // - - - - - - - - - - -
                            // Darwin Implementation
                            // - - - - - - - - - - -

typedef bsls::AtomicOperations::AtomicTypes::Int AtomicOpInt;

static const int k_UNINITIALIZED_CLOCK = 0;
static AtomicOpInt g_calendarClock = { k_UNINITIALIZED_CLOCK };
static AtomicOpInt g_realtimeClock = { k_UNINITIALIZED_CLOCK };

class MachClockGuard {
    // A guard that deallocates a Darwin (Mach kernel) 'clock_serv_t' on its
    // destruction.

    // DATA
    AtomicOpInt *d_clock_p;  // clock identifier

  private:
    // NOT IMPLEMENTED
    MachClockGuard(const MachClockGuard&);
    MachClockGuard operator=(const MachClockGuard&);

  public:

    // CREATORS
    explicit MachClockGuard(AtomicOpInt *clock)
        : d_clock_p(clock) {}

    ~MachClockGuard()
    {
        int clock = bsls::AtomicOperations::swapInt(d_clock_p,
                                                    k_UNINITIALIZED_CLOCK);
        if (clock != k_UNINITIALIZED_CLOCK) {
            mach_port_deallocate(mach_task_self(),
                                 static_cast<clock_serv_t>(clock));
        }
    }
};

#define BSLS_SYSTEMTIME_ASSERT(x) extern int ____dummy[(x) ? 1 : -1];
    // A simple compile-time assert (since we are below 'bslmf').

static
clock_serv_t getClockService(clock_id_t clockId, AtomicOpInt *atomicClockStore)
    // Return a Mach clock service handle for the specified 'clockId'.  Use the
    // specified 'atomicClockStore' for single-time initialization of the
    // clock service handle.
{
    BSLS_SYSTEMTIME_ASSERT(sizeof(clock_serv_t) == sizeof(int));

    // One-time initialization (note: the successfully initialized clock
    // service is released at the task's destruction).

    typedef bsls::AtomicOperations AtomicOp;

    if (k_UNINITIALIZED_CLOCK == AtomicOp::getIntAcquire(atomicClockStore)) {
        static MachClockGuard s_calendarClockGuard(&g_calendarClock);
        static MachClockGuard s_realtimeClockGuard(&g_realtimeClock);

        clock_serv_t clockServ;
        kern_return_t rc = host_get_clock_service(mach_host_self(),
                                                  clockId,
                                                  &clockServ);
        (void) rc; BSLS_ASSERT_OPT(KERN_SUCCESS == rc);

        if (AtomicOp::testAndSwapInt(atomicClockStore,
                                     k_UNINITIALIZED_CLOCK,
                                     static_cast<int>(clockServ))
            != k_UNINITIALIZED_CLOCK)
        {
            // 'atomicClockStore' was already initialized by another thread;
            // release 'clockServ'.

            rc = mach_port_deallocate(mach_task_self(), clockServ);
            (void)rc; BSLS_ASSERT_OPT(KERN_SUCCESS == rc);
        }
    }

    return static_cast<clock_serv_t>(
                                    AtomicOp::getIntRelaxed(atomicClockStore));
}

static inline
TimeInterval getNowTime(clock_id_t clockId, AtomicOpInt *atomicClockStore)
    // Return the current time for the specified 'clockId' using the specified
    // 'atomicClockStore' as storage for an appropriate Mach clock service
    // handle.
{
    mach_timespec_t mts;
    clock_serv_t clockServ = getClockService(clockId, atomicClockStore);
    clock_get_time(clockServ, &mts);
    return TimeInterval(mts.tv_sec, mts.tv_nsec);
}

TimeInterval SystemTime::nowRealtimeClock()
{
    return getNowTime(CALENDAR_CLOCK, &g_calendarClock);
}

TimeInterval SystemTime::nowMonotonicClock()
{
    return getNowTime(SYSTEM_CLOCK, &g_realtimeClock);
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

                            //- - - - - - - - - - - -
                            // Windows Implementation
                            //- - - - - - - - - - - -

const unsigned int k_TicksPerSecond       = 10000000;
static const bsls::Types::Uint64 k_TicksToWindowsEpoch
                                          = 11644473600LL * k_TicksPerSecond;
                                        // Windows epoch starts from Jan 1 1601
                                        // the offset is 11644473600 seconds
const unsigned int k_NanosecondsPerTick   = 100;
const unsigned int k_NanosecondsPerSecond = 1000000000;

static bsls::AtomicOperations::AtomicTypes::Int64 g_frequency = { 0 };

static inline
bsls::Types::Int64 getFrequency()
    // Return the frequency of the Windows monotonic performance counter.
{
    // We can use relaxed atomic operations throughout; it only matters that
    // 'g_frequency' is updated atomically.

    if (0 == bsls::AtomicOperations::getInt64Relaxed(&g_frequency)) {
        LARGE_INTEGER freq;

        BOOL rc = QueryPerformanceFrequency(&freq);
        (void)rc; BSLS_ASSERT(rc);

        bsls::AtomicOperations::setInt64Relaxed(&g_frequency, freq.QuadPart);
    }

    return bsls::AtomicOperations::getInt64Relaxed(&g_frequency);
}

// CLASS METHODS
TimeInterval SystemTime::nowRealtimeClock()
{
    ULARGE_INTEGER fileTime;
    GetSystemTimeAsFileTime(reinterpret_cast<FILETIME *>(&fileTime));
    bsls::Types::Uint64 ticksSinceEpoch
                                   = fileTime.QuadPart - k_TicksToWindowsEpoch;
    return TimeInterval(ticksSinceEpoch / k_TicksPerSecond,
                        ticksSinceEpoch % k_TicksPerSecond
                                                       * k_NanosecondsPerTick);
}

TimeInterval SystemTime::nowMonotonicClock()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    bsls::Types::Int64 freq = getFrequency();

    bsls::Types::Int64 nanosec = static_cast<Types::Int64>(
        static_cast<double>(counter.QuadPart) / freq * k_NanosecondsPerSecond);

    return TimeInterval(nanosec / k_NanosecondsPerSecond,
                        nanosec % k_NanosecondsPerSecond);
}

#else

                            //- - - - - - - - - - - -
                            // Default Implementation
                            //- - - - - - - - - - - -

static inline
TimeInterval getNowTime(clockid_t clockId)
    // Return the current time for the specified 'clockId'.
{
    timespec tp = { 0, 0 };
    int      rc = clock_gettime(clockId, &tp);
    (void)rc; BSLS_ASSERT(0 == rc);

    // The 'static_cast' is required as on some platforms 'tv_nsec' is a 64 bit
    // field for 64 bit builds.
    return TimeInterval(tp.tv_sec, static_cast<int>(tp.tv_nsec));
}


// CLASS METHODS
TimeInterval SystemTime::nowMonotonicClock()
{
    return getNowTime(CLOCK_MONOTONIC);
}

TimeInterval SystemTime::nowRealtimeClock()
{
    return getNowTime(CLOCK_REALTIME);
}

#endif

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
