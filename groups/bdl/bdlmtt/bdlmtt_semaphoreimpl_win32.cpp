// bdlmtt_semaphoreimpl_win32.cpp                                      -*-C++-*-
#include <bdlmtt_semaphoreimpl_win32.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmtt_semaphoreimpl_win32_cpp,"$Id$ $CSID$")

#include <bdlmtt_lockguard.h>    // for testing only
#include <bdlmtt_mutex.h>        // for testing only
#include <bdlmtt_threadutil.h>   // for testing only

#include <bdlmtt_platform.h>

#ifdef BDLMTT_PLATFORM_WIN32_THREADS

namespace BloombergLP {

namespace bdlmtt {
           // --------------------------------------------------------
           // class SemaphoreImpl<bdlmtt::Platform::Win32Semaphore>
           // --------------------------------------------------------

// MANIPULATORS
void SemaphoreImpl<bdlmtt::Platform::Win32Semaphore>::post(int number)
{
    for (int i = 0; i < number; ++i) {
        post();
    }
}

int SemaphoreImpl<bdlmtt::Platform::Win32Semaphore>::tryWait()
{
    for (int i = d_resources; i > 0; i = d_resources) {
        if (i == d_resources.testAndSwap(i, i - 1)) {
            return 0;
        }
    }
    return -1;
}
}  // close package namespace

}  // close namespace BloombergLP

#endif  // BDLMTT_PLATFORM_WIN32_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
