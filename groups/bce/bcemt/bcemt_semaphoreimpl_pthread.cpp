// bcemt_semaphoreimpl_pthread.cpp                                    -*-C++-*-
#include <bcemt_semaphoreimpl_pthread.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_semaphoreimpl_pthread_cpp,"$Id$ $CSID$")

#ifdef BCES_PLATFORM_POSIX_THREADS

#include <bcemt_lockguard.h>    // for testing only
#include <bcemt_mutex.h>        // for testing only
#include <bcemt_threadutil.h>   // for testing only

#include <bsl_c_errno.h>

namespace BloombergLP {

             // --------------------------------------------------------
             // class bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>
             // --------------------------------------------------------

// MANIPULATORS
void bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::post(int number)
{
    for (int i = 0; i < number; i++) {
        ::sem_post(&d_sem);
    }
}

void
bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::wait()
{
    while (::sem_wait(&d_sem) != 0 && EINTR == errno) {
        ;
    }
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
