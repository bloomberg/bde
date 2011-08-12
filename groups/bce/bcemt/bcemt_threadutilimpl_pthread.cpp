// bcemt_threadutilimpl_pthread.cpp                                   -*-C++-*-
#include <bcemt_threadutilimpl_pthread.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_threadutilimpl_pthread_cpp,"$Id$ $CSID$")

#include <bcemt_default.h>
#include <bcemt_threadattributes.h>

#include <bces_platform.h>

#ifdef BCES_PLATFORM__POSIX_THREADS

#include <bdet_timeinterval.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_ctime.h>
#include <bsl_limits.h>

#include <pthread.h>

#if defined(BSLS_PLATFORM__OS_AIX)
# include <sys/types.h>    // geteuid
# include <unistd.h>       // geteuid
#endif

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
#if defined(BSLS_PLATFORM__OS_HPUX)
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
        return rc;
    }
    rc |= pthread_attr_setdetachstate(
                             dest,
                             Attr::BCEMT_CREATE_DETACHED == src.detachedState()
                                                    ? PTHREAD_CREATE_DETACHED
                                                    : PTHREAD_CREATE_JOINABLE);

    int guardSize = src.guardSize();
    if (guardSize < 0) {
        guardSize = bcemt_Default::nativeDefaultThreadGuardSize();
    }
    rc |= pthread_attr_setguardsize(dest, guardSize);

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

    enum { STACK_FUDGE = 8192 };
        // Before 'STACK_FUDGE' was added, in bcemt_threadutil.t.cpp cases -2
        // and -4, Linux was crashing about 4K away from the end of the stack
        // in 32 & 64 bit.  All other unix platforms were running past the
        // end of the stack without crashing.

    int stackSize = src.stackSize();
    if (stackSize < 0) {
        stackSize = bcemt_Default::defaultThreadStackSize();
    }
    stackSize += STACK_FUDGE;
#if defined(BSLS_PLATFORM__OS_HPUX)
    // The Itanium divides the stack into two sections: a variable stack and a
    // control stack.  To make 'stackSize' have the same meaning across
    // platforms, we must double it on this platform.

    stackSize *= 2;
#endif
    rc |= pthread_attr_setstacksize(dest, stackSize);

    return rc;
}

            // -------------------------------------------------------
            // class bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>
            // -------------------------------------------------------

// CLASS DATA
const pthread_t
bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::INVALID_HANDLE =
                                                    static_cast<pthread_t>(-1);

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
                                                getMinSchedPriority(int policy)
{
    typedef bcemt_ThreadAttributes Attr;

    switch (policy) {
      case -1: {
        policy = sched_getscheduler(0);
      }  break;
      case Attr::BCEMT_SCHED_FIFO: {
        policy = SCHED_FIFO;
      }  break;
      case Attr::BCEMT_SCHED_RR: {
        policy = SCHED_RR;
      }  break;
      case Attr::BCEMT_SCHED_OTHER: {
        policy = SCHED_OTHER;
      }  break;
      case Attr::BCEMT_SCHED_DEFAULT: {
#if defined(BSLS_PLATFORM__OS_HPUX)
        policy = SCHED_HPUX;
#else
        policy = SCHED_OTHER;
#endif
      }  break;
      default: {
        return bsl::numeric_limits<int>::min();                       // RETURN
      }
    }

    return sched_get_priority_min(policy);
}

int bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::
                                                getMaxSchedPriority(int policy)
{
    typedef bcemt_ThreadAttributes Attr;

    switch (policy) {
      case -1: {
        policy = sched_getscheduler(0);
      }  break;
      case Attr::BCEMT_SCHED_FIFO: {
        policy = SCHED_FIFO;
      }  break;
      case Attr::BCEMT_SCHED_RR: {
        policy = SCHED_RR;
      }  break;
      case Attr::BCEMT_SCHED_OTHER: {
        policy = SCHED_OTHER;
      }  break;
      case Attr::BCEMT_SCHED_DEFAULT: {
#if defined(BSLS_PLATFORM__OS_HPUX)
        policy = SCHED_HPUX;
#else
        policy = SCHED_OTHER;
#endif
      }  break;
      default: {
        return bsl::numeric_limits<int>::min();                       // RETURN
      }
    }

    int pri = sched_get_priority_max(policy);

# if defined(BSLS_PLATFORM__OS_AIX)
    // Note max prirority returned is 127 regardless of policy on AIX

    enum { MAX_NON_SUPERUSER_PRIORITY = 60 };

    if (pri > MAX_NON_SUPERUSER_PRIORITY && 0 != geteuid()) {
        pri = MAX_NON_SUPERUSER_PRIORITY;
    }

# endif

    return pri;
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
