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
///Supported Clock-Types
///-------------------------
// The component 'bdetu_SystemClockType' supplies the enumeration indicating
// the system clock on which timeouts supplied to other methods should be
// based.  If the clock type indicated at construction is
// 'bdetu_SystemClockType::e_REALTIME', the timeout should be expressed as an
// absolute offset since 00:00:00 UTC, January 1, 1970 (which matches the epoch
// used in 'bdetu_SystemTime::now(bdetu_SystemClockType::e_REALTIME)'.  If the
// clock type indicated at construction is
// 'bdetu_SystemClockType::e_MONOTONIC', the timeout should be expressed as an
// absolute offset since the epoch of this clock (which matches the epoch used
// in 'bdetu_SystemTime::now(bdetu_SystemClockType::e_MONOTONIC)'.
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

#ifdef BCES_PLATFORM_POSIX_THREADS

// Platform specific implementation starts here.

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BDETU_SYSTEMCLOCKTYPE
#include <bdetu_systemclocktype.h>
#endif

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

#ifdef BSLS_PLATFORM_OS_DARWIN
    bdetu_SystemClockType::Type d_clockType;
#endif

    // NOT IMPLEMENTED
    bcemt_TimedSemaphoreImpl(const bcemt_TimedSemaphoreImpl&);
    bcemt_TimedSemaphoreImpl& operator=(const bcemt_TimedSemaphoreImpl&);

    // PRIVATE MANIPULATORS
    int timedWaitImp(const bdet_TimeInterval& timeout);
        // Block until the count of this semaphore is potentially a positive
        // value, or until the specified 'timeout' expires.  The 'timeout' is
        // an absolute time represented as an interval from some epoch, which
        // is detemined by the clock indicated at construction (see
        // {Supported Clock-Types} in the component documentation).  Return 0
        // if the 'timeout' did not expire, -1 if a timeout occurred, and -2 on
        // error.

  public:
    // CREATORS
    explicit
    bcemt_TimedSemaphoreImpl(bdetu_SystemClockType::Type clockType
                                          = bdetu_SystemClockType::e_REALTIME);
        // Create a timed semaphore initially having a count of 0.  Optionally
        // specify a 'clockType' indicating the type of the system clock
        // against which the 'bdet_TimeInterval' timeouts passed to the
        // 'timedWait' method are to be interpreted.  If 'clockType' is not
        // specified then the realtime system clock is used.

    explicit
    bcemt_TimedSemaphoreImpl(int                         count,
                             bdetu_SystemClockType::Type clockType
                                          = bdetu_SystemClockType::e_REALTIME);
        // Create a timed semaphore initially having the specified 'count'.
        // Optionally specify a 'clockType' indicating the type of the system
        // clock against which the 'bdet_TimeInterval' timeouts passed to the
        // 'timedWait' method are to be interpreted.  If 'clockType' is not
        // specified then the realtime system clock is used.

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
        // until the specified 'timeout' expires.  The 'timeout' is an absolute
        // time represented as an interval from some epoch, which is detemined
        // by the clock indicated at construction (see {Supported Clock-Types}
        // in the component documentation).  If the 'timeout' did not expire
        // before the count attained a positive value, atomically decrement the
        // count and return 0; otherwise, return a non-zero value with no
        // effect on the count.

    int tryWait();
        // Decrement the count of this semaphore if it is positive and return
        // 0.  Return a non-zero value otherwise.

    void wait();
        // Block until the count is a positive value and atomically decrement
        // it.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM_POSIX_THREADS

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
