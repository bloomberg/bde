// bcemt_semaphoreimpl_countedpthread.cpp                             -*-C++-*-
#include <bcemt_semaphoreimpl_countedpthread.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_semaphoreimpl_countedpthread_cpp,"$Id$ $CSID$")

#ifdef BCES_PLATFORM_POSIX_THREADS

#include <bcemt_lockguard.h>    // for testing only
#include <bcemt_mutex.h>        // for testing only
#include <bcemt_threadutil.h>   // for testing only

#include <bsl_c_errno.h>

namespace BloombergLP {

         // ---------------------------------------------------------------
         // class bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore>
         // ---------------------------------------------------------------

// MANIPULATORS
int bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore>::tryWait()
{
    for (int i = d_resources; i > 0; i = d_resources) {
        if (i == d_resources.testAndSwap(i, i - 1)) {
            return 0;
        }
    }

    return -1;
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM_POSIX_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
