// bcemt_threadutilimpl_pthread.cpp                                   -*-C++-*-
#include <bcemt_threadutilimpl_pthread.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_threadutilimpl_pthread_cpp,"$Id$ $CSID$")

#include <bcemt_configuration.h>
#include <bcemt_threadattributes.h>

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
# include <mach/mach.h>    // clock_sleep
# include <mach/clock.h>   // clock_sleep TBD
#endif

#include <errno.h>         // constant EINTR

#include <bdetu_datetimeinterval.h> // TBD
#include <bdetu_systemtime.h>
#include <bsl_iostream.h> // TBD

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
        rc |= pthread_attr_setstacksize(dest, stackSize);
    }

    return rc;
}

#if defined(BSLS_PLATFORM_OS_DARWIN)
namespace {

class PthreadMutexGuard {
    // A guard for unlocking and *destroying* a 'pthread_mutex_t'

    // DATA
    pthread_mutex_t *d_lock_p;

  private:
    // NOT IMPLEMENTED
    PthreadMutexGuard(const PthreadMutexGuard&);
    PthreadMutexGuard operator=(const PthreadMutexGuard&);
  public:

    // CREATORS
    PthreadMutexGuard(pthread_mutex_t *lock) : d_lock_p(lock) {}
    ~PthreadMutexGuard()
    {
      if (0 != pthread_mutex_unlock(d_lock_p)) {
	BSLS_ASSERT_OPT(false);
      }
      if (0 != pthread_mutex_destroy(d_lock_p)) {
	BSLS_ASSERT_OPT(false);
      }
    }
};

}

static bdet_TimeInterval getDarwinSystemBootTime()
    // Return the system-start time as a time interval from the UNIX epoch 
    // time, January 1, 1970
{

  // TBD: We currently obtain the system boot time by computing the
  // the difference between the 'REALTIME_CLOCK' and 'CALENDAR_CLOCK'.
  // This has the potential to be inaccurate, but after a day of investigation
  // is the best alternative I've found.  The one alternative I've found
  // using ('sysctl' to obtain the 'KERN_BOOTTIME'), was, on testing, 
  // significantly less accurate.


  static bsls::AtomicOperations::AtomicTypes::Int64 bootSecs     = { 0 };
  static bsls::AtomicOperations::AtomicTypes::Int   bootNanoSecs = { 0 };
 
  if (!bsls::AtomicOperations::getInt64Acquire(&bootSecs)) {
 
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    if (0 != pthread_mutex_lock(&mutex)) {
      BSLS_ASSERT_OPT(false);
    }

    PthreadMutexGuard guard(&mutex);
    
    clock_serv_t calendarClock, realtimeClock;

    kern_return_t status1 = host_get_clock_service(mach_host_self(),
						  REALTIME_CLOCK, 
						  &realtimeClock);

    kern_return_t status2 = host_get_clock_service(mach_host_self(),
						   CALENDAR_CLOCK, 
						   &calendarClock);
    BSLS_ASSERT_OPT(0 == status1);
    BSLS_ASSERT_OPT(0 == status2);


    mach_timespec_t nowCalendar, nowRealtime;

    clock_get_time(realtimeClock, &nowRealtime);
    clock_get_time(calendarClock, &nowCalendar);
    bdet_TimeInterval adjustment =
               bdet_TimeInterval(nowCalendar.tv_sec, nowCalendar.tv_nsec) -
               bdet_TimeInterval(nowRealtime.tv_sec, nowRealtime.tv_nsec);
    
    bsls::AtomicOperations::setInt64Release(&bootSecs, adjustment.seconds());
    bsls::AtomicOperations::setIntRelease(&bootNanoSecs, 
					  adjustment.nanoseconds());
  }
    
  return bdet_TimeInterval(
			  bsls::AtomicOperations::getInt64Relaxed(&bootSecs),
			  bsls::AtomicOperations::getIntRelaxed(&bootNanoSecs));
}
#endif

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
    // Note that the max prirority returned above is 127 regardless of policy
    // on AIX, yet for non-superusers, thread creation fails if
    // 'priority > 60'.  See AIX doc "http://publib.boulder.ibm.com/
    // infocenter/aix/v6r1/index.jsp?topic=%2Fcom.ibm.aix.basetechref%2F
    // doc%2Fbasetrf1%2Fpthread_setschedparam.htm" (shortens to
    // "http://bit.ly/pcOgwl").

    enum { MAX_AIX_NON_ROOT_PRIORITY = 60,
           MAX_AIX_PRIORITY          = 80 };

    if (0 == geteuid()) {
        // priviledged user

        // On AIX 5.3 and above, all priorities above 80 are equivalent to 80.

        if (priority > MAX_AIX_PRIORITY) {
            priority = MAX_AIX_PRIORITY;
        }
    }
    else {
        // non-priviledged user

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
    naptime.tv_sec  = static_cast<bsl::time_t>(sleepTime.seconds());
    naptime.tv_nsec = sleepTime.nanoseconds();

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
    timespec naptime;
    timespec unslept;
    naptime.tv_sec  = static_cast<bsl::time_t>(microsecs / 1000000 + seconds);
    naptime.tv_nsec = (microsecs % 1000000) * 1000;

    const int result = nanosleep(&naptime, unsleptTime ? &unslept : 0);
    if (result && unsleptTime) {
        unsleptTime->setInterval(unslept.tv_sec,
                                 static_cast<int>(unslept.tv_nsec));
    }
    return result;
}

int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::sleepUntil(
                               const bdet_TimeInterval& absoluteTime,
                               bool                     retryOnSignalInterupt)
{
    // ASSERT that the interval is between January 1, 1970 00:00.000 and
    // the end of December 31, 9999 (i.e., less than January 1, 10000).

    BSLS_ASSERT(absoluteTime >= bdet_TimeInterval(0, 0));
    BSLS_ASSERT(absoluteTime <  bdet_TimeInterval(253402300800LL, 0));

    // POSIX defines 'clock_nanosleep' which is used for most UNIX platforms,
    // Darwin does not provide that function,and provides the alternative
    // 'clock_sleep'.

#if defined(BSLS_PLATFORM_OS_DARWIN)
    // The online documentation for 'clock_sleep' (e.g., 
    // http://felinemenace.org/~nemo/mach/manpages/) is not very clear, and is
    // sometimes incorrect or out-of-date.  According 
    // 'mach.h' ('/user/include/mach/') the 'clock_sleep' signature is:
    //..
    //  kern_return_t clock_sleep(
    //        mach_port_t, int, mach_timespec_t, mach_timespec_t *);
    //..
    // According to 'mach_interface.h' mach_timespec_t is a simple struct that
    // is equivalent to 'timespec' on other UNIX platforms.  Many identifier 
    // types used in the mach interface are aliases to 'mach_port_t', including
    // 'clock_serv_t' which is returned by 'host_get_clock_service'.  The 
    // signature for 'host_get_clock_service' also differs from what is found
    // online.  The signature in 'mach_host.h':
    //..
    //  kern_return_t host_get_clock_service(host_t, clock_id_t, clock_serv_t *)
    //..

    //    TimeInterval systemBootTime = systemBootTime();

    clock_serv_t clock;

    // Unforutunately the 'CALNEDAR_CLOCK', which is based on unix-epoch time
    // does not provide 'clock_sleep'.  'REALTIME_CLOCK' is guaranteed to
    // support 'clock_sleep', but uses the system boot-time as the unix epoch.

    kern_return_t status = host_get_clock_service(mach_host_self(),
						  REALTIME_CLOCK, 
						  &clock);
    if (0 != status) {
      return status;
    }

    bdet_TimeInterval systemTime = absoluteTime - getDarwinSystemBootTime();

    if (systemTime <= bdet_TimeInterval()) {
      return 0;                                                        // RETURN
    }

    mach_timespec_t clockTime, resultTime;

    clockTime.tv_sec  = static_cast<bsl::time_t>(systemTime.seconds());
    clockTime.tv_nsec = static_cast<long>(systemTime.nanoseconds());

    status = clock_sleep(clock, TIME_ABSOLUTE, clockTime, &resultTime);

    return status;

#else
    timespec clockTime;
    clockTime.tv_sec  = static_cast<bsl::time_t>(absoluteTime.seconds());
    clockTime.tv_nsec = static_cast<long>(absoluteTime.nanoseconds());

    int result; 
    do {
        result = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &clockTime, 0);
       
    } while (EINTR == result && retryOnSignalInterupt);

    // An signal interrupt is not considered an error.

    return result == EINTR ? 0 : result;
#endif
}


}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
