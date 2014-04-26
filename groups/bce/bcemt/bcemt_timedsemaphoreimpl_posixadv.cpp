// bcemt_timedsemaphoreimpl_posixadv.cpp                              -*-C++-*-
#include <bcemt_timedsemaphoreimpl_posixadv.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_timedsemaphoreimpl_posixadv_cpp,"$Id$ $CSID$")

#include <bcemt_muteximpl_pthread.h>   // for testing only
#include <bcemt_threadutil.h>          // for testing only

#ifdef BCES_PLATFORM_POSIXADV_TIMEDSEMAPHORE

#include <bcemt_saturatedtimeconversionimputil.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemclocktype.h>
#include <bdetu_systemtime.h>

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
    bdet_TimeInterval realTimeout(timeout);

    if (d_clockType != bdetu_SystemClockType::e_REALTIME) {
        // since sem_timedwait operates only with the realtime clock, adjust
        // the timeout value to make it consistent with the realtime clock
        realTimeout += bdetu_SystemTime::nowRealtimeClock()
                       - bdetu_SystemTime::now(d_clockType);
    }

    timespec ts;
    bcemt_SaturatedTimeConversionImpUtil::toTimeSpec(&ts, realTimeout);

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
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
