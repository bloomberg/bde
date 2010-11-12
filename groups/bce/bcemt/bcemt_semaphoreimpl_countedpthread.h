// bcemt_semaphoreimpl_countedpthread.h                               -*-C++-*-
#ifndef INCLUDED_BCEMT_SEMAPHOREIMPL_COUNTEDPTHREAD
#define INCLUDED_BCEMT_SEMAPHOREIMPL_COUNTEDPTHREAD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a POSIX implementation of 'bcemt_Semaphore' with count.
//
//@CLASSES:
//  bcemt_SemaphoreImpl<CountedPosixSemaphore>: POSIX specialization with count
//
//@SEE_ALSO: bcemt_semaphore
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of 'bcemt_Semaphore'
// for POSIX threads ("pthreads") via the template specialization:
//..
//  bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_Semaphore'.
//
// This pthreads-based implementation of 'bcemt_Semaphore' differs from that of
// 'bcemt_semaphoreimpl_pthread' for platforms where a separate count must be
// maintained.  'bcemt_Semaphore' supports large values, but the native
// semaphores provided on some platforms are restricted to a relatively small
// range of values (e.g., '[ 0 .. 32000 ]' on AIX).  To support uniform
// usage across platforms, this component maintains the count of the semaphore
// in a separate atomic integer.  'post' is only invoked on the underlying
// semaphore when it is known there are threads blocked on it.
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

#ifdef BCES_PLATFORM__POSIX_THREADS

// Platform-specific implementation starts here.

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_SEMAPHORE
#include <semaphore.h>
#define INCLUDED_SEMAPHORE
#endif

namespace BloombergLP {

template <typename SEMAPHORE_POLICY>
class bcemt_SemaphoreImpl;

         // ===============================================================
         // class bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore>
         // ===============================================================

template <>
class bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore> {
    // This class provides a full specialization of 'bcemt_SemaphoreImpl'
    // for pthreads with a separate count variable.  This implementation
    // maintains the value of the semaphore in a separate atomic integer,
    // so as to allow for large counts on platforms where pthread supports a
    // very limited range of values.

    // DATA
    bces_AtomicInt d_resources;  // if positive, number of available resources
                                 // if negative: number of waiting threads

    sem_t          d_sem;        // native semaphore

    // NOT IMPLEMENTED
    bcemt_SemaphoreImpl(const bcemt_SemaphoreImpl&);
    bcemt_SemaphoreImpl& operator=(const bcemt_SemaphoreImpl&);

  public:
    // CREATORS
    bcemt_SemaphoreImpl(int count);
        // Create a semaphore.

    ~bcemt_SemaphoreImpl();
        // Destroy a semaphore

    // MANIPULATORS
    void post();
        // Atomically increment the count of this semaphore.

    void post(int number);
        // Atomically increment the count of this semaphore by the specified
        // 'number'.  The behavior is undefined unless 'number > 0'.

    int tryWait();
        // Decrement the count of this semaphore if it is positive and return
        // 0.  Return a non-zero value otherwise.

    void wait();
        // Block until the count of this semaphore is a positive value and
        // atomically decrement it.

    // ACCESSORS
    int getValue() const;
        // Return the current value of this semaphore.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

         // ---------------------------------------------------------------
         // class bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore>
         // ---------------------------------------------------------------

// CREATORS
inline
bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore>::bcemt_SemaphoreImpl(
                                                                     int count)
: d_resources(count)
{
    // The first 0 passed to 'sem_init' indicates not shared between processes.
    // A count of 0 (i.e., the second 0) is provided to 'sem_init' because
    // 'd_resources' holds the count.

    ::sem_init(&d_sem, 0, 0);
}

inline
bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore>::
                                                         ~bcemt_SemaphoreImpl()
{
    ::sem_destroy(&d_sem);
}

// MANIPULATORS
inline
void bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore>::post()
{
    if (++d_resources <= 0) {
        ::sem_post(&d_sem);
    }
}

// ACCESSORS
inline
int bcemt_SemaphoreImpl<bces_Platform::CountedPosixSemaphore>::getValue() const
{
    const int v = d_resources;
    return v > 0 ? v : 0;
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM__POSIX_THREADS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
