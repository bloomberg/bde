// bslmt_threadutilimpl_pthread.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_threadutilimpl_pthread.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_threadutilimpl_pthread_cpp,"$Id$ $CSID$")

#ifdef BSLMT_PLATFORM_POSIX_THREADS

#include <bslmt_configuration.h>
#include <bslmt_saturatedtimeconversionimputil.h>
#include <bslmt_threadattributes.h>

#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>

#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>   // 'bsl::min'
#include <bsl_cstdlib.h>
#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_ctime.h>
#include <bsl_c_limits.h>

#include <pthread.h>
#include <unistd.h>        // sysconf, geteuid

#if   defined(BSLS_PLATFORM_OS_AIX)
# include <sys/types.h>    // geteuid
#elif defined(BSLS_PLATFORM_OS_DARWIN)
# include <mach/mach.h>    // clock_sleep
# include <mach/clock.h>   // clock_sleep
# include <sys/sysctl.h>   // sysctl
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
# include <sys/utsname.h>
#elif defined(BSLS_PLATFORM_OS_LINUX)
# include <sys/prctl.h>
#endif

#include <errno.h>         // constant 'EINTR'

namespace {
namespace u {

using namespace BloombergLP;

// 'man (2) prctl' from Linux 2.6.9 says the thread name buffer must
// accommodate at least 16 bytes, for both setting and getting.

// http://linux.die.net/man/3/pthread_getname_np says 'pthread_getname_np'
// requires a buffer at least 16 bytes long (and says that 'pthread_setname_np'
// can't handle a string longer than 16 bytes, including the terminating '\0').

// 'man pthread_setname_np' on Solaris says the max thread name length is 31
// characters.  It diescussed no limit for 'pthread_getname_np', but the buffer
// length is passed to that call.

enum {
    k_THREAD_NAME_BUF_SIZE =
# if defined(BSLS_PLATFORM_OS_SOLARIS)
                              32
# else
                              16
# endif
};

static inline
int localPthreadsPolicy(int policy)
    // Return the native pthreads scheduling policy corresponding to the
    // specified 'policy' which is of type
    // 'bslmt::ThreadAttributes::SchedulingPolicy'.
{
    typedef bslmt::ThreadAttributes Attr;

    switch (policy) {
      case Attr::e_SCHED_OTHER:   return SCHED_OTHER;                 // RETURN
      case Attr::e_SCHED_FIFO:    return SCHED_FIFO;                  // RETURN
      case Attr::e_SCHED_RR:      return SCHED_RR;                    // RETURN
      case Attr::e_SCHED_DEFAULT:
      default:                    return SCHED_OTHER;                 // RETURN
    }

    BSLS_ASSERT_OPT(0);
    return SCHED_OTHER;
}

static int initPthreadAttribute(pthread_attr_t                 *destination,
                                const bslmt::ThreadAttributes&  src)
    // Initialize the specified pthreads attribute type 'destination',
    // configuring it with information from the specified thread attributes
    // object 'src'.  Note that it is assumed that 'destination' is
    // uninitialized and 'pthread_attr_init' has not already been call on it.
{
    typedef bslmt::ThreadAttributes Attr;

    int rc = pthread_attr_init(destination);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    // From now on, instead of doing 'if (0 != rc) return rc;' after each call
    // we just bitwise-or the return values, all of which we expect to be 0,
    // together, and then return that value at the end, to achieve higher speed
    // by avoiding confusing the pipeline with branches.

    rc |= pthread_attr_setdetachstate(
                             destination,
                             Attr::e_CREATE_DETACHED == src.detachedState()
                                                    ? PTHREAD_CREATE_DETACHED
                                                    : PTHREAD_CREATE_JOINABLE);

    int guardSize = src.guardSize();
    if (Attr::e_UNSET_GUARD_SIZE != guardSize) {
        rc |= pthread_attr_setguardsize(destination, guardSize);
    }

    if (!src.inheritSchedule()) {
        rc |= pthread_attr_setinheritsched(destination,
                                           PTHREAD_EXPLICIT_SCHED);

        int pthreadPolicy = u::localPthreadsPolicy(src.schedulingPolicy());
        rc |= pthread_attr_setschedpolicy(destination, pthreadPolicy);

        int priority = src.schedulingPriority();
        if (Attr::e_UNSET_PRIORITY != priority) {
            sched_param param;
            rc |= pthread_attr_getschedparam(destination, &param);
            param.sched_priority = priority;
            rc |= pthread_attr_setschedparam(destination, &param);
        }
    }

    int stackSize = src.stackSize();
    if (Attr::e_UNSET_STACK_SIZE == stackSize) {
        stackSize = bslmt::Configuration::defaultThreadStackSize();
    }

    if (Attr::e_UNSET_STACK_SIZE != stackSize) {
        // Note that if 'stackSize' is still unset, we just leave the
        // '*destination' to its default, initialized state.

        BSLS_ASSERT_OPT(stackSize > 0);

#if defined(PTHREAD_STACK_MIN)
        // Note sometimes PTHREAD_STACK_MIN is a function so cache the call to
        // a variable.

        const int pthreadStackMin = static_cast<int>(PTHREAD_STACK_MIN);
        if (stackSize < pthreadStackMin) {
            stackSize = pthreadStackMin;
        }
#endif

#if defined(BSLS_PLATFORM_OS_DARWIN)
        // Stack size needs to be a multiple of the system page size.

        static int pageSize = -1;
        if (-1 == pageSize) {
            pageSize = getpagesize();

            // Page size is always a power of 2.

            BSLS_ASSERT_SAFE((pageSize & (pageSize - 1)) == 0);
        }

        stackSize = (stackSize + pageSize - 1) & ~(pageSize - 1);
#endif

        rc |= pthread_attr_setstacksize(destination, stackSize);
    }

    return rc;
}

#if defined(BSLS_PLATFORM_OS_DARWIN)

class MachClockGuard {
    // A guard that deallocates a Darwin (mach kernel) 'clock_serv_t' on its
    // destruction.

    // DATA
    clock_serv_t d_clock;  // guarded clock identifier

  private:
    // NOT IMPLEMENTED
    MachClockGuard(const MachClockGuard&);
    MachClockGuard operator=(const MachClockGuard&);
  public:

    // CREATORS
    explicit MachClockGuard(clock_serv_t clock) : d_clock(clock) {}

    ~MachClockGuard()
    {
        mach_port_deallocate(mach_task_self(), d_clock);
    }
};

#endif  // defined(BSLS_PLATFORM_OS_DARWIN)

}  // close namespace u
}  // close unnamed namespace

               // --------------------------------------------
               // class ThreadUtilImpl<Platform::PosixThreads>
               // --------------------------------------------

namespace BloombergLP {

// CLASS DATA
const pthread_t
bslmt::ThreadUtilImpl<bslmt::Platform::PosixThreads>::INVALID_HANDLE =
                                                                (pthread_t) -1;

// CLASS METHODS
int bslmt::ThreadUtilImpl<bslmt::Platform::PosixThreads>::create(
                                         Handle                  *threadHandle,
                                         const ThreadAttributes&  attributes,
                                         bslmt_ThreadFunction     function,
                                         void                    *userData)
{
    int rc;
    pthread_attr_t pthreadAttr;

    rc = u::initPthreadAttribute(&pthreadAttr, attributes);
    if (rc) {
        return -1;                                                    // RETURN
    }

    rc = pthread_create(threadHandle,
                        &pthreadAttr,
                        function,
                        userData);

    // If 'attr' destruction fails, don't want to return a bad status if thread
    // creation succeeded and thread potentially needs to be joined.

    int rcDestroy = pthread_attr_destroy(&pthreadAttr);
    BSLS_ASSERT_SAFE(0 == rcDestroy);
    (void) rcDestroy;                      // suppress unused variable warnings

    return rc;
}

                // *** Thread Priorities ***

int bslmt::ThreadUtilImpl<bslmt::Platform::PosixThreads>::
      getMaxSchedulingPriority(ThreadAttributes::SchedulingPolicy policy)
{
    typedef ThreadAttributes Attributes;

    int pPolicy;

    switch (policy) {
      case Attributes::e_SCHED_FIFO: {
        pPolicy = SCHED_FIFO;
      }  break;
      case Attributes::e_SCHED_RR: {
        pPolicy = SCHED_RR;
      }  break;
      case Attributes::e_SCHED_OTHER: {
        pPolicy = SCHED_OTHER;
      }  break;
      case Attributes::e_SCHED_DEFAULT: {
        pPolicy = SCHED_OTHER;
      }  break;
      default: {
        pPolicy = SCHED_OTHER;
        BSLS_ASSERT_OPT(0);
      }
    }

    int priority = sched_get_priority_max(pPolicy);

#  if defined(BSLS_PLATFORM_OS_AIX)
    // Note max priority returned above is 127 regardless of policy on AIX, yet
    // for non-superusers, thread creation fails if 'priority > 60'.  See AIX
    // doc "http://publib.boulder.ibm.com/infocenter/aix/v6r1/index.jsp?
    // topic=%2Fcom.ibm.aix.basetechref%2Fdoc%2Fbasetrf1%2F
    // pthread_setschedparam.htm"

    enum { k_MAX_AIX_NON_ROOT_PRIORITY = 60,
           k_MAX_AIX_PRIORITY          = 80 };

    if (0 == geteuid()) {
        // privileged user

        // On AIX 5.3 and above, all priorities above 80 are equivalent to 80.

        if (priority > k_MAX_AIX_PRIORITY) {
            priority = k_MAX_AIX_PRIORITY;
        }
    }
    else {
        // non-privileged user

        if (priority > k_MAX_AIX_NON_ROOT_PRIORITY) {
            priority = k_MAX_AIX_NON_ROOT_PRIORITY;
        }
    }
#elif defined (BSLS_PLATFORM_OS_SOLARIS)

    // Solaris

    static bsls::AtomicOperations::AtomicTypes::Int isPost_5_10 = { -1 };

    int cacheIsPost_5_10 = bsls::AtomicOperations::getInt(&isPost_5_10);
    if (-1 == cacheIsPost_5_10) {
        struct utsname utsName;
        uname(&utsName);

        int front, tail = 0;
        char *dot = bsl::strchr(utsName.release, '.');
        if (dot) {
            *dot = 0;
            tail = bsl::atoi(dot + 1);
        }
        front = bsl::atoi(utsName.release);
        cacheIsPost_5_10 = front > 5 || (5 == front && tail > 10);

        bsls::AtomicOperations::setInt(&isPost_5_10, cacheIsPost_5_10);
    }

    if (cacheIsPost_5_10) {
        enum { k_MAX_SOLARIS_5_11_PRIORITY = 0 };

        // On Solaris 5.11, privileged clients can set threads to high
        // priorities that will make them real-time priority threads.  If done
        // wrong, this could destabilize the whole box for other users, and
        // there is no way to do it right through the pthreads interface.  So
        // we are preventing thread priorities above 0 on 5.11.
        //
        // Note that thread creation will fail for priorities above 0 for
        // non-privileged clients on 5.11.

        priority = k_MAX_SOLARIS_5_11_PRIORITY;
    }
#endif
    return priority;

}

int bslmt::ThreadUtilImpl<bslmt::Platform::PosixThreads>::
      getMinSchedulingPriority(ThreadAttributes::SchedulingPolicy policy)
{
    int pPolicy;

    typedef ThreadAttributes Attributes;
    switch (policy) {
      case Attributes::e_SCHED_FIFO: {
        pPolicy = SCHED_FIFO;
      }  break;
      case Attributes::e_SCHED_RR: {
        pPolicy = SCHED_RR;
      }  break;
      case Attributes::e_SCHED_OTHER: {
        pPolicy = SCHED_OTHER;
      }  break;
      case Attributes::e_SCHED_DEFAULT: {
        pPolicy = SCHED_OTHER;
      }  break;
      default: {
        pPolicy = SCHED_OTHER;
        BSLS_ASSERT_OPT(0);
      }
    }

    int priority = sched_get_priority_min(pPolicy);
# if defined(BSLS_PLATFORM_OS_AIX)
    // Note on AIX all priorities below 40 are equivalent to a priority of 40.
    // See AIX doc "http://publib.boulder.ibm.com/infocenter/aix/v6r1/
    // index.jsp?topic=%2Fcom.ibm.aix.basetechref%2Fdoc%2Fbasetrf1%2F
    // pthread_setschedparam.htm"

    enum { k_MIN_AIX_PRIORITY = 40 };

    if (priority < k_MIN_AIX_PRIORITY) {
        priority = k_MIN_AIX_PRIORITY;
    }

# endif
    return priority;
}

int bslmt::ThreadUtilImpl<bslmt::Platform::PosixThreads>::sleep(
                                        const bsls::TimeInterval&  sleepTime,
                                        bsls::TimeInterval        *unsleptTime)
{
    timespec naptime;
    timespec unslept_time;
    SaturatedTimeConversionImpUtil::toTimeSpec(&naptime, sleepTime);

    const int result = nanosleep(&naptime, unsleptTime ? &unslept_time : 0);
    if (result && unsleptTime) {
        unsleptTime->setInterval(unslept_time.tv_sec,
                                  static_cast<int>(unslept_time.tv_nsec));
    }
    return result;
}

void bslmt::ThreadUtilImpl<bslmt::Platform::PosixThreads>::getThreadName(
                                                       bsl::string *threadName)
{
    BSLS_ASSERT(threadName);

#if defined(BSLS_PLATFORM_OS_LINUX) ||  defined(BSLS_PLATFORM_OS_DARWIN) ||   \
    defined(BSLS_PLATFORM_OS_SOLARIS)

    char localBuf[u::k_THREAD_NAME_BUF_SIZE];

# if defined(BSLS_PLATFORM_OS_LINUX)
    const int rc = prctl(PR_GET_NAME, localBuf, 0, 0, 0);
# elif defined(BSLS_PLATFORM_OS_DARWIN) || defined(BSLS_PLATFORM_OS_SOLARIS)
    const int rc = pthread_getname_np(pthread_self(),
                                      localBuf,
                                      u::k_THREAD_NAME_BUF_SIZE);
# endif

    localBuf[u::k_THREAD_NAME_BUF_SIZE - 1] = 0;
    BSLS_ASSERT(0 == rc);        (void)rc;    // suppress unused warnings

    *threadName = localBuf;

#else

    // Thread names are not implemented on other platforms, but the passed
    // 'bsl::string' is cleared.

    threadName->clear();

#endif
}

int bslmt::ThreadUtilImpl<bslmt::Platform::PosixThreads>::microSleep(
                                              int                 microseconds,
                                              int                 seconds,
                                              bsls::TimeInterval *unsleptTime)
{
    enum { k_MILLION = 1000 * 1000 };

    bsls::TimeInterval sleptTime((microseconds / k_MILLION) + seconds,
                                 (microseconds % k_MILLION) * 1000);
    timespec naptime;
    timespec unslept;
    SaturatedTimeConversionImpUtil::toTimeSpec(&naptime, sleptTime);

    const int result = nanosleep(&naptime, unsleptTime ? &unslept : 0);
    if (result && unsleptTime) {
        unsleptTime->setInterval(unslept.tv_sec,
                                 static_cast<int>(unslept.tv_nsec));
    }
    return result;
}

void bslmt::ThreadUtilImpl<bslmt::Platform::PosixThreads>::setThreadName(
                                           const bslstl::StringRef& threadName)
{
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN) ||    \
    defined(BSLS_PLATFORM_OS_SOLARIS)

    char buffer[u::k_THREAD_NAME_BUF_SIZE];
    const bsl::size_t len = bsl::min(sizeof(buffer) - 1, threadName.length());

    bsl::strncpy(buffer, threadName.data(), len);
    buffer[len] = 0;

# if   defined(BSLS_PLATFORM_OS_LINUX)
    const int rc = prctl(PR_SET_NAME, buffer, 0, 0, 0);
# elif defined(BSLS_PLATFORM_OS_DARWIN)
    const int rc = pthread_setname_np(buffer);
# elif defined(BSLS_PLATFORM_OS_SOLARIS)
    const int rc = pthread_setname_np(pthread_self(), buffer);
# endif

    BSLS_ASSERT(0 == rc);        (void)rc;    // suppress unused warnings
#else
    // This function has no effect on other platforms.

    (void)threadName;
#endif
}

int bslmt::ThreadUtilImpl<bslmt::Platform::PosixThreads>::sleepUntil(
                            const bsls::TimeInterval&   absoluteTime,
                            bool                        retryOnSignalInterrupt,
                            bsls::SystemClockType::Enum clockType)
{
    // ASSERT that the interval is between January 1, 1970 00:00.000 and the
    // end of December 31, 9999 (i.e., less than January 1, 10000).

    BSLS_ASSERT(absoluteTime >= bsls::TimeInterval(0, 0));
    BSLS_ASSERT(absoluteTime <  bsls::TimeInterval(253402300800LL, 0));

    // POSIX defines 'clock_nanosleep' which is used for most UNIX platforms,
    // Darwin does not provide that function.  Originally, we implemented this
    // using the kernel funciton clock_sleep, but have moved to a simpler
    // implementation.

#if defined(BSLS_PLATFORM_OS_DARWIN)
    (void) retryOnSignalInterrupt;
    bsls::TimeInterval relativeTime =
        absoluteTime - bsls::SystemTime::now(clockType);

    while (relativeTime > bsls::TimeInterval()) {
        int rc = sleep(relativeTime);
        if (0 != rc) {
            return rc;                                                 // RETURN
        }
        relativeTime = absoluteTime - bsls::SystemTime::now(clockType);
    }
    return 0;
#else
    timespec clockTime;
    SaturatedTimeConversionImpUtil::toTimeSpec(&clockTime, absoluteTime);

    int pthreadClockType = (clockType == bsls::SystemClockType::e_MONOTONIC
                            ? CLOCK_MONOTONIC
                            : CLOCK_REALTIME);
    int result;
    do {
        result = clock_nanosleep(pthreadClockType,
                                 TIMER_ABSTIME,
                                 &clockTime,
                                 0);
    } while (EINTR == result && retryOnSignalInterrupt);

    // An signal interrupt is not considered an error.

    return result == EINTR ? 0 : result;
#endif
}

unsigned int
bslmt::ThreadUtilImpl<bslmt::Platform::PosixThreads>::hardwareConcurrency()
{
    int result = 0;
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_AIX)          \
 || defined(BSLS_PLATFORM_OS_SOLARIS) || defined(BSLS_PLATFORM_OS_CYGWIN)

    result = static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));

#elif defined(BSLS_PLATFORM_OS_FREEBSD) || defined(BSLS_PLATFORM_OS_DARWIN)

    int         mib[2];
    bsl::size_t len = sizeof(result);

    // set the mib for hw.ncpu.

    mib[0] = CTL_HW;
    mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU

    // Get the number of CPUs from the system.

    sysctl(mib, 2, &result, &len, NULL, 0);

    if (result < 1)
    {
        mib[1] = HW_NCPU;
        sysctl(mib, 2, &result, &len, NULL, 0);
    }

#else

    BSLMF_ASSERT(!"Unsupported platform");

#endif

    return 0 > result ? 0 : static_cast<unsigned int>(result);
}

}  // close enterprise namespace

#endif

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
