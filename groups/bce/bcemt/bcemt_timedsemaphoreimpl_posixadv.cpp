// bcemt_timedsemaphoreimpl_posixadv.cpp                              -*-C++-*-
#include <bcemt_timedsemaphoreimpl_posixadv.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_timedsemaphoreimpl_posixadv_cpp,"$Id$ $CSID$")

#include <bcemt_muteximpl_pthread.h>   // for testing only
#include <bcemt_threadutil.h>          // for testing only

#ifdef BCES_PLATFORM_POSIXADV_TIMEDSEMAPHORE

#include <bdet_timeinterval.h>

#include <bsl_ctime.h>
#include <bsl_c_errno.h>

namespace BloombergLP {

           // ======================================================
           // class bcemt_TimedSemaphoreImpl<PosixAdvTimedSemaphore>
           // ======================================================

// MANIPULATORS
void bcemt_TimedSemaphoreImpl<bces_Platform::PosixAdvTimedSemaphore>::post(
                                                                    int number)
{
    for (int i = 0; i < number; i++) {
        ::sem_post(&d_sem);
    }
}

int bcemt_TimedSemaphoreImpl<bces_Platform::PosixAdvTimedSemaphore>::timedWait(
                                              const bdet_TimeInterval& timeout)
{
    timespec ts;
    ts.tv_sec  = static_cast<bsl::time_t>(timeout.seconds());
    ts.tv_nsec = timeout.nanoseconds();

    while (0 != ::sem_timedwait(&d_sem, &ts)) {
        if (EINTR != errno) {
            return -1;                                                // RETURN
        }
    }
    return 0;
}

void
bcemt_TimedSemaphoreImpl<bces_Platform::PosixAdvTimedSemaphore>::wait()
{
    while (::sem_wait(&d_sem) != 0) {
        if (EINTR != errno) {
            return;                                                   // RETURN
        }
    }
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM_POSIXADV_TIMEDSEMAPHORE

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
