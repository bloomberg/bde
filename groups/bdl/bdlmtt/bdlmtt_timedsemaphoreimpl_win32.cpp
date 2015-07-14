// bdlmtt_timedsemaphoreimpl_win32.cpp                                 -*-C++-*-
#include <bdlmtt_timedsemaphoreimpl_win32.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmtt_timedsemaphoreimpl_win32_cpp,"$Id$ $CSID$")

#ifdef BDLMTT_PLATFORM_WIN32_THREADS

#include <bcemt_SaturatedTimeConversionimputil.h>

#include <bdlmtt_lockguard.h>     // for testing only
#include <bdlmtt_mutex.h>         // for testing only
#include <bdlmtt_threadutil.h>    // for testing only

namespace BloombergLP {

namespace bdlmtt {
           // ---------------------------------------------------
           // class TimedSemaphoreImpl<Win32TimedSemaphore>
           // ---------------------------------------------------

// MANIPULATORS
int
TimedSemaphoreImpl<bdlmtt::Platform::Win32TimedSemaphore>::timedWait(
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

#endif  // BDLMTT_PLATFORM_WIN32_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
