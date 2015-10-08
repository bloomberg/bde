// bslmt_timedsemaphoreimpl_posixadv.cpp                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_timedsemaphoreimpl_posixadv.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_timedsemaphoreimpl_posixadv_cpp,"$Id$ $CSID$")

#ifdef BSLMT_PLATFORM_POSIXADV_TIMEDSEMAPHORE

#include <bslmt_muteximpl_pthread.h>   // for testing only
#include <bslmt_saturatedtimeconversionimputil.h>
#include <bslmt_threadutil.h>          // for testing only

#include <bsls_timeinterval.h>
#include <bsls_systemclocktype.h>
#include <bsls_systemtime.h>

#include <bsl_ctime.h>
#include <bsl_c_errno.h>

namespace BloombergLP {

             // ================================================
             // class TimedSemaphoreImpl<PosixAdvTimedSemaphore>
             // ================================================

// MANIPULATORS
void bslmt::TimedSemaphoreImpl<bslmt::Platform::PosixAdvTimedSemaphore>::post(
                                                                    int number)
{
    for (int i = 0; i < number; i++) {
        ::sem_post(&d_sem);
    }
}

int bslmt::TimedSemaphoreImpl<bslmt::Platform::PosixAdvTimedSemaphore>::
    timedWait(const bsls::TimeInterval& timeout)
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
bslmt::TimedSemaphoreImpl<bslmt::Platform::PosixAdvTimedSemaphore>::wait()
{
    while (::sem_wait(&d_sem) != 0) {
        if (EINTR != errno) {
            return;                                                   // RETURN
        }
    }
}

}  // close enterprise namespace

#endif  // BSLMT_PLATFORM_POSIXADV_TIMEDSEMAPHORE

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
