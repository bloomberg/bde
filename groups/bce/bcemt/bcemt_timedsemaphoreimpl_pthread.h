// bcemt_timedsemaphoreimpl_pthread.h                                 -*-C++-*-
#ifndef INCLUDED_BCEMT_TIMEDSEMAPHOREIMPL_PTHREAD
#define INCLUDED_BCEMT_TIMEDSEMAPHOREIMPL_PTHREAD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a POSIX implementation of 'bcemt_TimedSemaphore'.
//
//@CLASSES:
//  bcemt_TimedSemaphoreImpl<PthreadTimedSemaphore>: POSIX specialization
//
//@SEE_ALSO: bcemt_timedsemaphore
//
//@AUTHOR: Guillaume Morin (gmorin1)
//
//@DESCRIPTION: This component provides an implementation of
// 'bcemt_TimedSemaphore' for POSIX threads ("pthreads") via the template
// specialization:
//..
//  bcemt_TimedSemaphoreImpl<bces_Platform::PosixThreads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_TimedSemaphore'.
//
///Usage
///-----
// This component is an implementation detail of 'bcemt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

#ifdef BCES_PLATFORM_POSIX_THREADS

// Platform specific implementation starts here.

#ifndef INCLUDED_PTHREAD
#include <pthread.h>
#define INCLUDED_PTHREAD
#endif

namespace BloombergLP {

template <class TIMED_SEMAPHORE_POLICY>
class bcemt_TimedSemaphoreImpl;

class bdet_TimeInterval;

           // =====================================================
           // class bcemt_TimedSemaphoreImpl<PthreadTimedSemaphore>
           // =====================================================

template<>
class bcemt_TimedSemaphoreImpl<bces_Platform::PthreadTimedSemaphore> {
    // This class implements a portable semaphore type for thread
    // synchronization.

    // DATA
    bces_AtomicInt  d_resources;  // resources count
    bces_AtomicInt  d_waiters;    // number of threads waiting
    pthread_mutex_t d_lock;       // lock
    pthread_cond_t  d_condition;  // condition

    // NOT IMPLEMENTED
    bcemt_TimedSemaphoreImpl(const bcemt_TimedSemaphoreImpl&);
    bcemt_TimedSemaphoreImpl& operator=(const bcemt_TimedSemaphoreImpl&);

  public:
    // CREATORS
    bcemt_TimedSemaphoreImpl();
        // Create a new semaphore object with a count of 0.

    explicit
    bcemt_TimedSemaphoreImpl(int count);
        // Create a new semaphore object having the specified 'count'.

    ~bcemt_TimedSemaphoreImpl();
        // Destroy this semaphore object.

    // MANIPULATORS
    void post();
        // Atomically increment the count of the semaphore.

    void post(int number);
        // Atomically increment the count by the specified 'number' of the
        // semaphore.  The behavior is undefined unless 'number' is a positive
        // value.

    int timedWait(const bdet_TimeInterval& timeout);
        // Block until the count of this semaphore is a positive value, or
        // until the specified 'timeout' (expressed as the !ABSOLUTE! time from
        // 00:00:00 UTC, January 1, 1970) expires.  Atomically decrement the
        // count and return 0 on success, and a non-zero value otherwise.

    int tryWait();
        // Decrement the count of this semaphore if it is positive and return
        // 0.  Return a non-zero value otherwise.

    void wait();
        // Block until the count is a positive value and atomically decrement
        // it.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

           // =====================================================
           // class bcemt_TimedSemaphoreImpl<PthreadTimedSemaphore>
           // =====================================================

// CREATORS
inline
bcemt_TimedSemaphoreImpl<bces_Platform::PthreadTimedSemaphore>::
                                            bcemt_TimedSemaphoreImpl()
: d_resources(0)
, d_waiters(0)
{
    pthread_mutex_init(&d_lock, 0);
    pthread_cond_init(&d_condition, 0);
}

inline
bcemt_TimedSemaphoreImpl<bces_Platform::PthreadTimedSemaphore>::
                                            bcemt_TimedSemaphoreImpl(int count)
: d_resources(count)
, d_waiters(0)
{
    pthread_mutex_init(&d_lock, 0);
    pthread_cond_init(&d_condition, 0);
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM_POSIX_THREADS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
