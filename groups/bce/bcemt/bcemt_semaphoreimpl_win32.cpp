// bcemt_semaphoreimpl_win32.cpp                                      -*-C++-*-
#include <bcemt_semaphoreimpl_win32.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_semaphoreimpl_win32_cpp,"$Id$ $CSID$")

#include <bcemt_lockguard.h>    // for testing only
#include <bcemt_mutex.h>        // for testing only
#include <bcemt_threadutil.h>   // for testing only

#include <bces_platform.h>

#ifdef BCES_PLATFORM_WIN32_THREADS

namespace BloombergLP {

           // --------------------------------------------------------
           // class bcemt_SemaphoreImpl<bces_Platform::Win32Semaphore>
           // --------------------------------------------------------

// MANIPULATORS
void bcemt_SemaphoreImpl<bces_Platform::Win32Semaphore>::post(int number)
{
    for (int i = 0; i < number; ++i) {
        post();
    }
}

int bcemt_SemaphoreImpl<bces_Platform::Win32Semaphore>::tryWait()
{
    for (int i = d_resources; i > 0; i = d_resources) {
        if (i == d_resources.testAndSwap(i, i - 1)) {
            return 0;
        }
    }
    return -1;
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM_WIN32_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
