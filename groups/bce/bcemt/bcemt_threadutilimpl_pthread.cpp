// bcemt_threadutilimpl_pthread.cpp                                   -*-C++-*-
#include <bcemt_threadutilimpl_pthread.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_threadutilimpl_pthread_cpp,"$Id$ $CSID$")

#include <bcemt_default.h>
#include <bcemt_threadattributes.h>

#include <bdet_timeinterval.h>
#include <bsls_platform.h>

#ifdef BCES_PLATFORM__POSIX_THREADS

#include <bsl_ctime.h>

namespace BloombergLP {

static int initPthreadAttribute(pthread_attr_t                *dest,
                                const bcemt_ThreadAttributes&  src)
    // Initialize the specified pthreads attribute type 'dest', configuring it
    // with information from the specified thread attributes object 'src'.
    // Note that it is assumed that 'pthread_attr_init' has not already been
    // call on 'dest'.
{
    typedef bcemt_ThreadAttributes Attr;

    int rc = 0;

    rc |= pthread_attr_init(dest);
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

    if (0 == src.inheritSchedule()) {
        rc |= pthread_attr_setinheritsched(dest, PTHREAD_EXPLICIT_SCHED);

        int policy = src.schedulingPolicy();
        policy = Attr::BCEMT_SCHED_FIFO == policy
               ? SCHED_FIFO
               : Attr::BCEMT_SCHED_RR == policy ? SCHED_RR : SCHED_OTHER;
        rc |= pthread_attr_setschedpolicy(dest, policy);

        struct sched_param sched;
        rc |= pthread_attr_getschedparam(dest, &sched);
        sched.sched_priority = src.schedulingPriority();
        rc |= pthread_attr_setschedparam(dest, &sched);
    }

    int stackSize = src.stackSize();
    if (stackSize < 0) {
        stackSize = bcemt_Default::defaultThreadStackSize();
    }
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
