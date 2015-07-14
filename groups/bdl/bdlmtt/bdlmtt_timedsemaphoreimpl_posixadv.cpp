// bdlmtt_timedsemaphoreimpl_posixadv.cpp                              -*-C++-*-
#include <bdlmtt_timedsemaphoreimpl_posixadv.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmtt_timedsemaphoreimpl_posixadv_cpp,"$Id$ $CSID$")

#include <bdlmtt_muteximpl_pthread.h>   // for testing only
#include <bdlmtt_threadutil.h>          // for testing only

#ifdef BDLMTT_PLATFORM_POSIXADV_TIMEDSEMAPHORE

#include <bdlmtt_saturatedtimeconversionimputil.h>

#include <bsls_timeinterval.h>
#include <bsls_systemclocktype.h>
#include <bsls_systemtime.h>

#include <bsl_ctime.h>
#include <bsl_c_errno.h>

namespace BloombergLP {

namespace bdlmtt {
           // ======================================================
           // class TimedSemaphoreImpl<PosixAdvTimedSemaphore>
           // ======================================================

// MANIPULATORS
void TimedSemaphoreImpl<bdlmtt::Platform::PosixAdvTimedSemaphore>::post(
                                                                    int number)
{
    for (int i = 0; i < number; i++) {
        ::sem_post(&d_sem);
    }
}

int TimedSemaphoreImpl<bdlmtt::Platform::PosixAdvTimedSemaphore>::timedWait(
                                              const bsls::TimeInterval& timeout)
{
    bsls::TimeInterval realTimeout(timeout);

    if (d_clockType != bsls::SystemClockType::e_REALTIME) {
        // since sem_timedwait operates only with the realtime clock, adjust
        // the timeout value to make it consistent with the realtime clock
        realTimeout += bsls::SystemTime::nowRealtimeClock() -
                       bsls::SystemTime::now(d_clockType);
    }

    timespec ts;
    SaturatedTimeConversionImpUtil::toTimeSpec(&ts, realTimeout);

    while (0 != ::sem_timedwait(&d_sem, &ts)) {
        if (EINTR != errno) {
            return -1;                                                // RETURN
        }
    }

    return 0;
}

void
TimedSemaphoreImpl<bdlmtt::Platform::PosixAdvTimedSemaphore>::wait()
{
    while (::sem_wait(&d_sem) != 0) {
        if (EINTR != errno) {
            return;                                                   // RETURN
        }
    }
}
}  // close package namespace

}  // close namespace BloombergLP

#endif  // BDLMTT_PLATFORM_POSIXADV_TIMEDSEMAPHORE

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
