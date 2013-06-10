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

// CREATORS
bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::bcemt_SemaphoreImpl(
                                                                     int count)
{
    int result = ::sem_init(&d_sem, 0, count);

    (void) result;
    BSLS_ASSERT(result == 0);
}

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
    sem_t * sem_p = &d_sem;
    int result = 0;

    do {
        result = ::sem_wait(sem_p);
    } while (result != 0 && errno == EINTR);

    BSLS_ASSERT(result == 0);
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
