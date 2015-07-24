// bdlqq_semaphoreimpl_win32.cpp                                      -*-C++-*-
#include <bdlqq_semaphoreimpl_win32.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_semaphoreimpl_win32_cpp,"$Id$ $CSID$")

#include <bdlqq_lockguard.h>    // for testing only
#include <bdlqq_mutex.h>        // for testing only
#include <bdlqq_threadutil.h>   // for testing only

#include <bdlqq_platform.h>

#ifdef BDLQQ_PLATFORM_WIN32_THREADS

namespace BloombergLP {

namespace bdlqq {
           // --------------------------------------------------------
           // class SemaphoreImpl<bdlqq::Platform::Win32Semaphore>
           // --------------------------------------------------------

// MANIPULATORS
void SemaphoreImpl<bdlqq::Platform::Win32Semaphore>::post(int number)
{
    for (int i = 0; i < number; ++i) {
        post();
    }
}

int SemaphoreImpl<bdlqq::Platform::Win32Semaphore>::tryWait()
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

#endif  // BDLQQ_PLATFORM_WIN32_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
