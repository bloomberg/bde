// bcemt_semaphoreimpl_pthread.h                -*-C++-*-
#ifndef INCLUDED_BCEMT_SEMAPHOREIMPL_PTHREAD
#define INCLUDED_BCEMT_SEMAPHOREIMPL_PTHREAD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a POSIX implementation of 'bcemt_Semaphore'.
//
//@CLASSES:
//  bcemt_SemaphoreImpl<PosixSemaphore>: POSIX specialization
//
//@SEE_ALSO: bcemt_semaphore
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of 'bcemt_Semaphore'
// for POSIX threads ("pthreads") via the template specialization:
//..
//  bcemt_SemaphoreImpl<bces_Platform::PosixThreads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_Semaphore'.
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

// Platform-specific implementation starts here.

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_SEMAPHORE
#include <semaphore.h>
#define INCLUDED_SEMAPHORE
#endif

namespace BloombergLP {

template <typename SEMAPHORE_POLICY>
class bcemt_SemaphoreImpl;

             // ========================================================
             // class bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>
             // ========================================================

template <>
class bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore> {
    // This class provides a full specialization of 'bcemt_SemaphoreImpl'
    // for pthreads.  The implementation provided here defines an efficient
    // proxy for the 'sem_t' pthread type, and related operations.

    // DATA
    sem_t d_sem;  // TBD doc

    // NOT IMPLEMENTED
    bcemt_SemaphoreImpl(const bcemt_SemaphoreImpl&);
    bcemt_SemaphoreImpl& operator=(const bcemt_SemaphoreImpl&);

  public:
    // CREATORS
    bcemt_SemaphoreImpl(int count);
        // Create a semaphore initialized to the specified 'count'.

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

             // --------------------------------------------------------
             // class bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>
             // --------------------------------------------------------

// CREATORS
inline
bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::bcemt_SemaphoreImpl(
                                                                     int count)
{
    ::sem_init(&d_sem, 0, count);
}

inline
bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::~bcemt_SemaphoreImpl()
{
    ::sem_destroy(&d_sem);
}

// MANIPULATORS
inline
void bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::post()
{
    ::sem_post(&d_sem);
}

inline
int bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::tryWait()
{
    return ::sem_trywait(&d_sem);
}

// ACCESSORS
inline
int bcemt_SemaphoreImpl<bces_Platform::PosixSemaphore>::getValue() const
{
    int value;
    const int rc = ::sem_getvalue(const_cast<sem_t *>(&d_sem), &value);
    (void) rc;
    BSLS_ASSERT_SAFE(0 == rc);
    return value;
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
