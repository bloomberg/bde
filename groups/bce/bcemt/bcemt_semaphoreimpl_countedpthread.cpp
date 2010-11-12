// bcemt_semaphoreimpl_countedpthread.cpp                             -*-C++-*-
#include <bcemt_semaphoreimpl_countedpthread.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_semaphoreimpl_countedpthread_cpp,"$Id$ $CSID$")

#ifdef BCES_PLATFORM__POSIX_THREADS

#include <bcemt_lockguard.h>    // for testing only
#include <bcemt_mutex.h>        // for testing only
#include <bcemt_threadutil.h>   // for testing only

#include <bsl_c_errno.h>

namespace BloombergLP {

         // ---------------------------------------------------------------
         // class bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore>
         // ---------------------------------------------------------------

// MANIPULATORS
void
bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore>::post(int number)
{
    for (int i = 0; i < number; ++i) {
        post();
    }
}

int bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore>::tryWait()
{
    for (int i = d_resources; i > 0; i = d_resources) {
        if (i == d_resources.testAndSwap(i, i - 1)) {
            return 0;
        }
    }
    return -1;
}

void bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore>::wait()
{
    if (--d_resources >= 0) {
        return;
    }

    while (::sem_wait(&d_sem) != 0 && EINTR == errno) {
        ;
    }
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM__POSIX_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
