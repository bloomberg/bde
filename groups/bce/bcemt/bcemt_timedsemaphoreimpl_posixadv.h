// bcemt_timedsemaphoreimpl_posixadv.h                                -*-C++-*-
#ifndef INCLUDED_BCEMT_TIMEDSEMAPHOREIMPL_POSIXADV
#define INCLUDED_BCEMT_TIMEDSEMAPHOREIMPL_POSIXADV

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide "advanced" POSIX implementation of 'bcemt_TimedSemaphore'.
//
//@CLASSES:
//  bcemt_TimedSemaphoreImpl<PosixAdvTimedSemaphore>: POSIXa specialization
//
//@SEE_ALSO: bcemt_timedsemaphore
//
//@AUTHOR: David Schumann (dschumann1)
//
//@DESCRIPTION: This component provides an implementation of
// 'bcemt_TimedSemaphore' for conforming POSIX platforms via the template
// specialization:
//..
//  bcemt_TimedSemaphoreImpl<bces_Platform::PosixAdvTimedSemaphore>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_TimedSemaphore'.
//
// This implementation of 'bcemt_TimedSemaphore' is preferred over that
// defined in 'bcemt_timedsemaphoreimpl_pthread' on platforms that support
// advanced realtime POSIX extensions (e.g., 'sem_timedwait').
//
///Usage
///-----
// This component is an implementation detail of 'bcemt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

#ifdef BCES_PLATFORM_POSIXADV_TIMEDSEMAPHORE

// Platform-specific implementation starts here.

#ifndef INCLUDED_SEMAPHORE
#include <semaphore.h>
#define INCLUDED_SEMAPHORE
#endif

namespace BloombergLP {

template <class TIMED_SEMAPHORE_POLICY>
class bcemt_TimedSemaphoreImpl;

class bdet_TimeInterval;

           // ======================================================
           // class bcemt_TimedSemaphoreImpl<PosixAdvTimedSemaphore>
           // ======================================================

template <>
class bcemt_TimedSemaphoreImpl<bces_Platform::PosixAdvTimedSemaphore> {
    // This class implements a timed semaphore in terms of POSIX operations.
    // Note that only certain platforms provide 'sem_timedwait'; on those that
    // do not, 'bcemt_TimedSemaphoreImpl<PthreadTimedSemaphore>' is used.

    // DATA
    sem_t d_sem;  // TBD doc

    // NOT IMPLEMENTED
    bcemt_TimedSemaphoreImpl(const bcemt_TimedSemaphoreImpl&);
    bcemt_TimedSemaphoreImpl& operator=(const bcemt_TimedSemaphoreImpl&);

  public:
    // CREATORS
    bcemt_TimedSemaphoreImpl();
        // Create a timed semaphore object with a count of 0.

    explicit
    bcemt_TimedSemaphoreImpl(int count);
        // Create a timed semaphore object having the specified 'count'.

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

           // ------------------------------------------------------
           // class bcemt_TimedSemaphoreImpl<PosixAdvTimedSemaphore>
           // ------------------------------------------------------

// CREATORS
inline
bcemt_TimedSemaphoreImpl<bces_Platform::PosixAdvTimedSemaphore>::
                                                     bcemt_TimedSemaphoreImpl()
{
    ::sem_init(&d_sem, 0, 0);
}

inline
bcemt_TimedSemaphoreImpl<bces_Platform::PosixAdvTimedSemaphore>::
                                            bcemt_TimedSemaphoreImpl(int count)
{
    ::sem_init(&d_sem, 0, count);
}

inline
bcemt_TimedSemaphoreImpl<bces_Platform::PosixAdvTimedSemaphore>::
                                                    ~bcemt_TimedSemaphoreImpl()
{
    ::sem_destroy(&d_sem);
}

// MANIPULATORS
inline
void bcemt_TimedSemaphoreImpl<bces_Platform::PosixAdvTimedSemaphore>::post()
{
    ::sem_post(&d_sem);
}

inline
int bcemt_TimedSemaphoreImpl<bces_Platform::PosixAdvTimedSemaphore>::tryWait()
{
    return ::sem_trywait(&d_sem);
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
