// bcemt_timedsemaphoreimpl_pthread.cpp                               -*-C++-*-
#include <bcemt_timedsemaphoreimpl_pthread.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_timedsemaphoreimpl_pthread_cpp,"$Id$ $CSID$")

#include <bcemt_muteximpl_pthread.h>   // for testing only
#include <bcemt_threadutil.h>

#include <bdet_timeinterval.h>

#include <bsls_assert.h>

#ifdef BCES_PLATFORM_POSIX_THREADS

// Platform-specific implementation starts here.

#include <bsl_ctime.h>
#include <bsl_c_errno.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS
static
int decrementIfPositive(bces_AtomicInt *a)
    // Try to decrement the specified atomic integer 'a' if positive.  Return
    // 0 on success and a non-zero value otherwise.
{
    int i = *a;

    while (i > 0) {
        if (i == a->testAndSwap(i, i - 1)) {
            return 0;                                                 // RETURN
        }
        i = *a;
    }
    return -1;
}

static
int pthreadTimedWait(pthread_cond_t           *cond,
                     pthread_mutex_t          *lock,
                     const bdet_TimeInterval&  timeout)
{
    timespec ts;
    ts.tv_sec  = static_cast<bsl::time_t>(timeout.seconds());
    ts.tv_nsec = timeout.nanoseconds();
    int status = pthread_cond_timedwait(cond, lock, &ts);

    return 0 == status ? 0 : (ETIMEDOUT == status ? -1 : -2);
}

           // -----------------------------------------------------
           // class bcemt_TimedSemaphoreImpl<PthreadTimedSemaphore>
           // -----------------------------------------------------

// CREATORS
bcemt_TimedSemaphoreImpl<bces_Platform::PthreadTimedSemaphore>::
                                                    ~bcemt_TimedSemaphoreImpl()
{
    pthread_mutex_lock(&d_lock);
    pthread_mutex_destroy(&d_lock);

    pthread_cond_destroy(&d_condition);

    BSLS_ASSERT(d_resources >= 0);
    BSLS_ASSERT(d_waiters >= 0);
}

// MANIPULATORS
void bcemt_TimedSemaphoreImpl<bces_Platform::PthreadTimedSemaphore>::post()
{
    ++d_resources;
    // barrier
    if (d_waiters > 0) {
        pthread_mutex_lock(&d_lock);
        pthread_cond_signal(&d_condition);
        pthread_mutex_unlock(&d_lock);
    }
}

void
bcemt_TimedSemaphoreImpl<bces_Platform::PthreadTimedSemaphore>::post(int n)
{
    BSLS_ASSERT(n > 0);

    d_resources += n;
    // barrier
    if (d_waiters > 0) {
        pthread_mutex_lock(&d_lock);
        pthread_cond_broadcast(&d_condition);
        pthread_mutex_unlock(&d_lock);
    }
}

int bcemt_TimedSemaphoreImpl<bces_Platform::PthreadTimedSemaphore>::timedWait(
                                              const bdet_TimeInterval& timeout)
{
    if (0 == decrementIfPositive(&d_resources)) {
        return 0;                                                     // RETURN
    }

    int ret = 0;
    pthread_mutex_lock(&d_lock);
    ++d_waiters;
    while (0 != decrementIfPositive(&d_resources)) {
        const int status = pthreadTimedWait(&d_condition, &d_lock, timeout);
        if (0 != status) {
            BSLS_ASSERT(-1 == status);  // It is a timeout and not an error.
            ret = 1;
            break;
        }
    }
    --d_waiters;
    pthread_mutex_unlock(&d_lock);
    return ret;
}

int bcemt_TimedSemaphoreImpl<bces_Platform::PthreadTimedSemaphore>::tryWait()
{
    const int newValue = decrementIfPositive(&d_resources);
    if (newValue >= 0) {
        if (newValue != 0) {
            pthread_cond_signal(&d_condition);
        }
        return 0;                                                     // RETURN
    }
    return 1;
}

void bcemt_TimedSemaphoreImpl<bces_Platform::PthreadTimedSemaphore>::wait()
{
    if (0 == decrementIfPositive(&d_resources)) {
        return;                                                       // RETURN
    }

    pthread_mutex_lock(&d_lock);
    ++d_waiters;
    while (0 != decrementIfPositive(&d_resources)) {
        pthread_cond_wait(&d_condition, &d_lock);
    }
    --d_waiters;
    pthread_mutex_unlock(&d_lock);
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
