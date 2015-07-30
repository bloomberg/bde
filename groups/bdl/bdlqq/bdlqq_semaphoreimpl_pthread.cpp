// bdlqq_semaphoreimpl_pthread.cpp                                    -*-C++-*-
#include <bdlqq_semaphoreimpl_pthread.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_semaphoreimpl_pthread_cpp,"$Id$ $CSID$")

#ifdef BDLQQ_PLATFORM_POSIX_SEMAPHORE

#include <bdlqq_lockguard.h>    // for testing only
#include <bdlqq_mutex.h>        // for testing only
#include <bdlqq_threadutil.h>   // for testing only

#include <bsl_c_errno.h>

namespace BloombergLP {

namespace bdlqq {
             // --------------------------------------------------------
             // class SemaphoreImpl<bdlqq::Platform::PosixSemaphore>
             // --------------------------------------------------------

// CREATORS
SemaphoreImpl<bdlqq::Platform::PosixSemaphore>::SemaphoreImpl(
                                                                     int count)
{
    int result = ::sem_init(&d_sem, 0, count);

    (void) result;
    BSLS_ASSERT(result == 0);
}

// MANIPULATORS
void SemaphoreImpl<bdlqq::Platform::PosixSemaphore>::post(int number)
{
    for (int i = 0; i < number; i++) {
        post();
    }
}

void
SemaphoreImpl<bdlqq::Platform::PosixSemaphore>::wait()
{
    sem_t * sem_p = &d_sem;
    int result = 0;

    do {
        result = ::sem_wait(sem_p);
    } while (result != 0 && errno == EINTR);

    BSLS_ASSERT(result == 0);
}
}  // close package namespace

}  // close namespace BloombergLP

#endif  // BDLQQ_PLATFORM_POSIX_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
