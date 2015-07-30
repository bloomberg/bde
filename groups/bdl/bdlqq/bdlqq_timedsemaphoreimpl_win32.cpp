// bdlqq_timedsemaphoreimpl_win32.cpp                                 -*-C++-*-
#include <bdlqq_timedsemaphoreimpl_win32.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_timedsemaphoreimpl_win32_cpp,"$Id$ $CSID$")

#ifdef BDLQQ_PLATFORM_WIN32_THREADS

#include <bcemt_SaturatedTimeConversionimputil.h>

#include <bdlqq_lockguard.h>     // for testing only
#include <bdlqq_mutex.h>         // for testing only
#include <bdlqq_threadutil.h>    // for testing only

namespace BloombergLP {

namespace bdlqq {
           // ---------------------------------------------------
           // class TimedSemaphoreImpl<Win32TimedSemaphore>
           // ---------------------------------------------------

// MANIPULATORS
int
TimedSemaphoreImpl<bdlqq::Platform::Win32TimedSemaphore>::timedWait(
                                              const bsls::TimeInterval& timeout)
{
    DWORD milliTimeout = 0;
    bsls::TimeInterval now = bdlt::CurrentTime::now(d_clockType);
    if (timeout > now) {
        bsls::TimeInterval reltime = timeout - now;
        SaturatedTimeConversionImpUtil::toMillisec(&milliTimeout,
                                                         reltime);
    }

    return WaitForSingleObject(d_handle, milliTimeout);
}
}  // close package namespace

}  // close namespace BloombergLP

#endif  // BDLQQ_PLATFORM_WIN32_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
