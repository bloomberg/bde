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

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_ctime.h>
#include <bsl_c_limits.h>

#include <pthread.h>

#if   defined(BSLS_PLATFORM_OS_AIX)
# include <sys/types.h>    // geteuid
# include <unistd.h>       // geteuid
#elif defined(BSLS_PLATFORM_OS_DARWIN)
# include <unistd.h>       // sysconf
# include <mach/mach.h>    // clock_sleep
# include <mach/clock.h>   // clock_sleep
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
# include <sys/utsname.h>
#endif

#include <errno.h>         // constant 'EINTR'

namespace BloombergLP {

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
#if defined(BSLS_PLATFORM_OS_HPUX)
      case Attr::e_SCHED_DEFAULT:
      default:                        return SCHED_HPUX;              // RETURN
#else
      case Attr::e_SCHED_DEFAULT:
      default:                        return SCHED_OTHER;             // RETURN
#endif
    }

    BSLS_ASSERT_OPT(0);
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

        int pthreadPolicy = localPthreadsPolicy(src.schedulingPolicy());
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

#if defined(BSLS_PLATFORM_OS_HPUX)
        // The Itanium divides the stack into two sections: a variable stack
        // and a control stack.  To make 'stackSize' have the same meaning
        // across platforms, we must double it on this platform.

        stackSize *= 2;
#endif

#if defined(PTHREAD_STACK_MIN)
        // Note sometimes PTHREAD_STACK_MIN is a function so cache the call to
        // a variable.

        const int pthreadStackMin = PTHREAD_STACK_MIN;
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

namespace {

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

}  // close unnamed namespace

#endif  // defined(BSLS_PLATFORM_OS_DARWIN)

               // --------------------------------------------
               // class ThreadUtilImpl<Platform::PosixThreads>
               // --------------------------------------------

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

    rc = initPthreadAttribute(&pthreadAttr, attributes);
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
    ++rcDestroy;                           // suppress unused variable warnings

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
    // Darwin does not provide that function,and provides the alternative
    // 'clock_sleep'.

#if defined(BSLS_PLATFORM_OS_DARWIN)
    // According 'mach.h' ('/user/include/mach/') the 'clock_sleep' signature
    // is:
    //..
    //  kern_return_t clock_sleep(
    //        mach_port_t, int, mach_timespec_t, mach_timespec_t *);
    //..
    // According to 'mach_interface.h' mach_timespec_t is a simple struct that
    // is equivalent to 'timespec' on other UNIX platforms.  Many identifier
    // types used in the mach interface are aliases to 'mach_port_t', including
    // 'clock_serv_t' which is returned by 'host_get_clock_service'.  The
    // signature for 'host_get_clock_service' is in 'mach_host.h':
    //..
    //  kern_return_t host_get_clock_service(host_t, clock_id_t,clock_serv_t *)
    //..
    // There is little official documentation of these APIs.  Some information
    // can be found:
    //: o http://felinemenace.org/~nemo/mach/manpages/
    //: o http://boredzo.org/blog/archives/2006-11-26/how-to-use-mach-clocks/
    //: o Mac OS X Interals: A Systems Approach (On Safari-Online)

    // This implementation is very sensitive to the 'clockType'.  For safety,
    // we will assert the value is one of the two currently expected values.

    BSLS_ASSERT(bsls::SystemClockType::e_REALTIME ==  clockType ||
                bsls::SystemClockType::e_MONOTONIC == clockType);

    bsls::TimeInterval sleepUntilTime(absoluteTime);
    if (clockType != bsls::SystemClockType::e_MONOTONIC) {
        // since we will be operating with the monotonic clock, adjust the
        // timeout value to make it consistent with the monotonic clock

        sleepUntilTime += bsls::SystemTime::nowMonotonicClock() -
                          bsls::SystemTime::now(clockType);
    }

    clock_serv_t clock;

    // Unfortunately the 'CALENDAR_CLOCK', which is based on unix-epoch time
    // does not provide 'clock_sleep'.  'REALTIME_CLOCK' is guaranteed to
    // support 'clock_sleep', but uses the system boot-time as the unix epoch.

    kern_return_t status = host_get_clock_service(mach_host_self(),
                                                  REALTIME_CLOCK,
                                                  &clock);
    MachClockGuard clockGuard(clock);

    if (0 != status) {
        return status;                                                // RETURN
    }

    if (sleepUntilTime <= bsls::TimeInterval()) {
        return 0;                                                     // RETURN
    }

    mach_timespec_t clockTime;
    mach_timespec_t resultTime;

    SaturatedTimeConversionImpUtil::toTimeSpec(&clockTime,
                                                     sleepUntilTime);

    status = clock_sleep(clock, TIME_ABSOLUTE, clockTime, &resultTime);

    return KERN_ABORTED == status ? 0 : status;

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
