// bcemt_threadutilimpl_pthread.cpp                                   -*-C++-*-
#include <bcemt_threadutilimpl_pthread.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_threadutilimpl_pthread_cpp,"$Id$ $CSID$")

#include <bcemt_configuration.h>
#include <bcemt_threadattributes.h>
#include <bcemt_saturatedtimeconversionimputil.h>

#include <bces_platform.h>

#ifdef BCES_PLATFORM_POSIX_THREADS

#include <bdet_timeinterval.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_ctime.h>
#include <bsl_c_limits.h>

#include <pthread.h>

#if defined(BSLS_PLATFORM_OS_AIX)
# include <sys/types.h>    // geteuid
# include <unistd.h>       // geteuid
#endif

#if defined(BSLS_PLATFORM_OS_DARWIN)
# include <unistd.h>       // sysconf
# include <mach/mach.h>    // clock_sleep
# include <mach/clock.h>   // clock_sleep
#endif

#include <errno.h>         // constant EINTR

namespace BloombergLP {

static inline
int localPthreadsPolicy(int policy)
    // Return the native pthreads scheduling policy corresponding to the
    // specified 'policy' which is of type
    // 'bcemt_ThreadAttributes::SchedulingPolicy'.
{
    typedef bcemt_ThreadAttributes Attr;

    switch (policy) {
      case Attr::BCEMT_SCHED_OTHER:   return SCHED_OTHER;             // RETURN
      case Attr::BCEMT_SCHED_FIFO:    return SCHED_FIFO;              // RETURN
      case Attr::BCEMT_SCHED_RR:      return SCHED_RR;                // RETURN
#if defined(BSLS_PLATFORM_OS_HPUX)
      case Attr::BCEMT_SCHED_DEFAULT:
      default:                        return SCHED_HPUX;              // RETURN
#else
      case Attr::BCEMT_SCHED_DEFAULT:
      default:                        return SCHED_OTHER;             // RETURN
#endif
    }

    BSLS_ASSERT_OPT(0);
}

static int initPthreadAttribute(pthread_attr_t                *dest,
                                const bcemt_ThreadAttributes&  src)
    // Initialize the specified pthreads attribute type 'dest', configuring it
    // with information from the specified thread attributes object 'src'.
    // Note that it is assumed that 'dest' is uninitialized and
    // 'pthread_attr_init' has not already been call on it.
{
    typedef bcemt_ThreadAttributes Attr;

    int rc = pthread_attr_init(dest);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    // From now on, instead of doing 'if (0 != rc) return rc;' after each call
    // we just bitwise-or the return values, all of which we expect to be 0,
    // together, and then return that value at the end, to achieve higher speed
    // by avoiding confusing the pipeline with branches.

    rc |= pthread_attr_setdetachstate(
                             dest,
                             Attr::BCEMT_CREATE_DETACHED == src.detachedState()
                                                    ? PTHREAD_CREATE_DETACHED
                                                    : PTHREAD_CREATE_JOINABLE);

    int guardSize = src.guardSize();
    if (Attr::BCEMT_UNSET_GUARD_SIZE != guardSize) {
        rc |= pthread_attr_setguardsize(dest, guardSize);
    }

    if (!src.inheritSchedule()) {
        rc |= pthread_attr_setinheritsched(dest, PTHREAD_EXPLICIT_SCHED);

        int pthreadPolicy = localPthreadsPolicy(src.schedulingPolicy());
        rc |= pthread_attr_setschedpolicy(dest, pthreadPolicy);

        int priority = src.schedulingPriority();
        if (Attr::BCEMT_UNSET_PRIORITY != priority) {
            sched_param param;
            rc |= pthread_attr_getschedparam(dest, &param);
            param.sched_priority = priority;
            rc |= pthread_attr_setschedparam(dest, &param);
        }
    }

    int stackSize = src.stackSize();
    if (Attr::BCEMT_UNSET_STACK_SIZE == stackSize) {
        stackSize = bcemt_Configuration::defaultThreadStackSize();
    }

    if (Attr::BCEMT_UNSET_STACK_SIZE != stackSize) {
        // Note that if 'stackSize' is still unset, we just leave the '*dest'
        // to its default, initialized state.

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
        long pageSize = sysconf(_SC_PAGESIZE);

        // Page size is always a power of 2.
        BSLS_ASSERT_SAFE(pageSize & (pageSize - 1) == 0);

        stackSize = (stackSize & ~(pageSize - 1)) + pageSize;
#endif

        rc |= pthread_attr_setstacksize(dest, stackSize);
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

            // -------------------------------------------------------
            // class bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>
            // -------------------------------------------------------

// CLASS DATA
const pthread_t
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::INVALID_HANDLE =
                                                                (pthread_t) -1;

// CLASS METHODS
int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::create(
          bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::Handle *handle,
          const bcemt_ThreadAttributes&                              attribute,
          bcemt_ThreadFunction                                       function,
          void                                                      *userData)
{
    int rc;
    pthread_attr_t pthreadAttr;

    rc = initPthreadAttribute(&pthreadAttr, attribute);
    if (rc) {
        return -1;                                                    // RETURN
    }

    rc = pthread_create(handle,
                        &pthreadAttr,
                        function,
                        userData);

    // If attr destruction fails, don't want to return a bad status if thread
    // creation succeeded and thread potentially needs to be joined.

    int rcDestroy = pthread_attr_destroy(&pthreadAttr);
    BSLS_ASSERT_SAFE(0 == rcDestroy);
    ++rcDestroy;                           // suppress unused variable warnings

    return rc;
}

int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::
                        getMinSchedulingPriority(
                               bcemt_ThreadAttributes::SchedulingPolicy policy)
{
    typedef bcemt_ThreadAttributes Attr;

    int pPolicy;

    switch (policy) {
      case Attr::BCEMT_SCHED_FIFO: {
        pPolicy = SCHED_FIFO;
      }  break;
      case Attr::BCEMT_SCHED_RR: {
        pPolicy = SCHED_RR;
      }  break;
      case Attr::BCEMT_SCHED_OTHER: {
        pPolicy = SCHED_OTHER;
      }  break;
      case Attr::BCEMT_SCHED_DEFAULT: {
#if defined(BSLS_PLATFORM_OS_HPUX)
        pPolicy = SCHED_HPUX;
#else
        pPolicy = SCHED_OTHER;
#endif
      }  break;
      default: {
        BSLS_ASSERT_OPT(0);
      }
    }

    int priority = sched_get_priority_min(pPolicy);

# if defined(BSLS_PLATFORM_OS_AIX)
    // Note that on AIX all priorities below 40 are equivalent to a priority of
    // 40.  See AIX doc "http://publib.boulder.ibm.com/infocenter/aix/v6r1/
    // index.jsp?topic=%2Fcom.ibm.aix.basetechref%2Fdoc%2Fbasetrf1%2F
    // pthread_setschedparam.htm" (shortens to "http://bit.ly/pcOgwl").

    enum { MIN_AIX_PRIORITY = 40 };

    if (priority < MIN_AIX_PRIORITY) {
        priority = MIN_AIX_PRIORITY;
    }

# endif

    return priority;
}

int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::
                        getMaxSchedulingPriority(
                               bcemt_ThreadAttributes::SchedulingPolicy policy)
{
    typedef bcemt_ThreadAttributes Attr;

    int pPolicy;

    switch (policy) {
      case Attr::BCEMT_SCHED_FIFO: {
        pPolicy = SCHED_FIFO;
      }  break;
      case Attr::BCEMT_SCHED_RR: {
        pPolicy = SCHED_RR;
      }  break;
      case Attr::BCEMT_SCHED_OTHER: {
        pPolicy = SCHED_OTHER;
      }  break;
      case Attr::BCEMT_SCHED_DEFAULT: {
#if defined(BSLS_PLATFORM_OS_HPUX)
        pPolicy = SCHED_HPUX;
#else
        pPolicy = SCHED_OTHER;
#endif
      }  break;
      default: {
        BSLS_ASSERT_OPT(0);
      }
    }

    int priority = sched_get_priority_max(pPolicy);

# if defined(BSLS_PLATFORM_OS_AIX)
    // Note that the max priority returned above is 127 regardless of policy
    // on AIX, yet for non-superusers, thread creation fails if
    // 'priority > 60'.  See AIX doc "http://publib.boulder.ibm.com/
    // infocenter/aix/v6r1/index.jsp?topic=%2Fcom.ibm.aix.basetechref%2F
    // doc%2Fbasetrf1%2Fpthread_setschedparam.htm" (shortens to
    // "http://bit.ly/pcOgwl").

    enum { MAX_AIX_NON_ROOT_PRIORITY = 60,
           MAX_AIX_PRIORITY          = 80 };

    if (0 == geteuid()) {
        // privileged user

        // On AIX 5.3 and above, all priorities above 80 are equivalent to 80.

        if (priority > MAX_AIX_PRIORITY) {
            priority = MAX_AIX_PRIORITY;
        }
    }
    else {
        // non-privileged user

        if (priority > MAX_AIX_NON_ROOT_PRIORITY) {
            priority = MAX_AIX_NON_ROOT_PRIORITY;
        }
    }

# endif

    return priority;
}

int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::sleep(
                                        const bdet_TimeInterval&  sleepTime,
                                        bdet_TimeInterval        *unsleeptTime)
{
    timespec naptime;
    timespec unslept_time;
    bcemt_SaturatedTimeConversionImpUtil::toTimeSpec(&naptime, sleepTime);

    const int result = nanosleep(&naptime, unsleeptTime ? &unslept_time : 0);
    if (result && unsleeptTime) {
        unsleeptTime->setInterval(unslept_time.tv_sec,
                                  static_cast<int>(unslept_time.tv_nsec));
    }
    return result;
}

int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::microSleep(
                                                int                microsecs,
                                                int                seconds,
                                                bdet_TimeInterval *unsleptTime)
{
    enum { MILLION = 1000 * 1000 };

    bdet_TimeInterval sleepTime((microsecs / MILLION) + seconds,
                                (microsecs % MILLION) * 1000);
    timespec naptime;
    timespec unslept;
    bcemt_SaturatedTimeConversionImpUtil::toTimeSpec(&naptime, sleepTime);

    const int result = nanosleep(&naptime, unsleptTime ? &unslept : 0);
    if (result && unsleptTime) {
        unsleptTime->setInterval(unslept.tv_sec,
                                 static_cast<int>(unslept.tv_nsec));
    }
    return result;
}

int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::sleepUntil(
                             const bdet_TimeInterval&    absoluteTime,
                             bool                        retryOnSignalInterupt,
                             bdetu_SystemClockType::Enum clockType)
{
    // ASSERT that the interval is between January 1, 1970 00:00.000 and
    // the end of December 31, 9999 (i.e., less than January 1, 10000).

    BSLS_ASSERT(absoluteTime >= bdet_TimeInterval(0, 0));
    BSLS_ASSERT(absoluteTime <  bdet_TimeInterval(253402300800LL, 0));

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

    // This implementation is very sensitive to the 'clockType'.  For
    // safety, we will assert the value is one of the two currently expected
    // values.
    BSLS_ASSERT(bdetu_SystemClockType::e_REALTIME ==  clockType ||
                bdetu_SystemClockType::e_MONOTONIC == clockType);

    if (clockType != bdetu_SystemClockType::e_MONOTONIC) {
        // since we will be operating with the monotonic clock, adjust
        // the timeout value to make it consistent with the monotonic clock
        absoluteTime += bdetu_SystemTime::nowMonotonicClock()
                                            - bdetu_SystemTime::now(clockType);
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

    if (absoluteTime <= bdet_TimeInterval()) {
        return 0;                                                     // RETURN
    }

    mach_timespec_t clockTime;
    mach_timespec_t resultTime;

    bcemt_SaturatedTimeConversionImpUtil::toTimeSpec(&clockTime, absoluteTime);

    status = clock_sleep(clock, TIME_ABSOLUTE, clockTime, &resultTime);

    return KERN_ABORTED == status ? 0 : status;

#else
    timespec clockTime;
    bcemt_SaturatedTimeConversionImpUtil::toTimeSpec(&clockTime, absoluteTime);

    int pthreadClockType = (clockType == bdetu_SystemClockType::e_MONOTONIC
                            ? CLOCK_MONOTONIC
                            : CLOCK_REALTIME);
    int result;
    do {
        result = clock_nanosleep(pthreadClockType,
                                 TIMER_ABSTIME,
                                 &clockTime,
                                 0);
    } while (EINTR == result && retryOnSignalInterupt);

    // An signal interrupt is not considered an error.

    return result == EINTR ? 0 : result;
#endif
}


}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
