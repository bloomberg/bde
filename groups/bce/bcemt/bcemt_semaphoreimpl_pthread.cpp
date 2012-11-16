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

const char *
bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::s_semaphoreName
    = "bcemt_semaphore_object";

// MANIPULATORS
void bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::post(int number)
{
    for (int i = 0; i < number; i++) {
        post();
    }
}

void
bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::wait()
{
#if defined(BSLS_PLATFORM_OS_DARWIN)
    sem_t * sem_p = d_sem_p;
#else
    sem_t * sem_p = &d_sem;
#endif

    while (::sem_wait(sem_p) != 0 && errno == EINTR) {
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
