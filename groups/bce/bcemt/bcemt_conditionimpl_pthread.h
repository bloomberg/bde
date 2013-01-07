// bcemt_conditionimpl_pthread.h                                      -*-C++-*-
#ifndef INCLUDED_BCEMT_CONDITIONIMPL_PTHREAD
#define INCLUDED_BCEMT_CONDITIONIMPL_PTHREAD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a POSIX implementation of 'bcemt_Condition'.
//
//@CLASSES:
//  bcemt_ConditionImpl<PosixThreads>: POSIX specialization
//
//@SEE_ALSO: bcemt_condition
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of 'bcemt_Condition'
// for POSIX threads ("pthreads") via the template specialization:
//..
//  bcemt_ConditionImpl<bces_Platform::PosixThreads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_Condition'.
//
///Usage
///-----
// This component is an implementation detail of 'bcemt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEX
#include <bcemt_mutex.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

#ifdef BCES_PLATFORM_POSIX_THREADS

// Platform-specific implementation starts here.

#ifndef INCLUDED_BSL_CTIME
#include <bsl_ctime.h>
#endif

#ifndef INCLUDED_BSL_C_ERRNO
#include <bsl_c_errno.h>
#endif

#ifndef INCLUDED_PTHREAD
#include <pthread.h>
#define INCLUDED_PTHREAD
#endif

namespace BloombergLP {

template <typename THREAD_POLICY>
class bcemt_ConditionImpl;

class bdet_TimeInterval;

             // ======================================================
             // class bcemt_ConditionImpl<bces_Platform::PosixThreads>
             // ======================================================

template <>
class bcemt_ConditionImpl<bces_Platform::PosixThreads> {
    // This class provides a full specialization of 'bcemt_Condition'
    // for pthreads.  The implementation provided here defines an efficient
    // proxy for the 'pthread_cond_t' pthread type, and related operations.

    // DATA
    pthread_cond_t d_cond;  // TBD doc

    // NOT IMPLEMENTED
    bcemt_ConditionImpl(const bcemt_ConditionImpl&);
    bcemt_ConditionImpl& operator=(const bcemt_ConditionImpl&);

  public:
    // CREATORS
    bcemt_ConditionImpl();
        // Create a condition variable.

    ~bcemt_ConditionImpl();
        // Destroy condition variable this object.

    // MANIPULATORS
    void broadcast();
        // Signal this condition object; wake up all threads that are currently
        // waiting on this condition.

    void signal();
        // Signal this condition object; wake up a single thread that is
        // currently waiting on this condition.

    int timedWait(bcemt_Mutex *mutex, const bdet_TimeInterval& timeout);
        // Atomically unlock the specified 'mutex' and suspend execution of
        // current thread until this condition object is "signaled"('signal',
        // 'broadcast') or until the specified 'timeout' (expressed as the
        // absolute time from 00:00:00 UTC, January 1, 1970), then re-acquire
        // the lock on the specified 'mutex', and return 0 upon success and
        // non-zero if an error or timeout occurred.  Note that the behavior is
        // undefined unless specified 'mutex' is locked by the calling thread
        // prior to calling this method.

    int wait(bcemt_Mutex *mutex);
        // Atomically unlock the specified 'mutex' and suspend execution of
        // current thread until this condition object is "signaled"('signal',
        // 'broadcast') then re-acquire the lock on the specified 'mutex', and
        // return 0 upon success and non-zero otherwise.  Note that the
        // behavior is undefined unless specified 'mutex' is locked by the
        // calling thread prior to calling this method.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

             // ------------------------------------------------------
             // class bcemt_ConditionImpl<bces_Platform::PosixThreads>
             // ------------------------------------------------------

// CREATORS
inline
bcemt_ConditionImpl<bces_Platform::PosixThreads>::bcemt_ConditionImpl()
{
    pthread_cond_init(&d_cond, 0);
}

inline
bcemt_ConditionImpl<bces_Platform::PosixThreads>::~bcemt_ConditionImpl()
{
    pthread_cond_destroy(&d_cond);
}

// MANIPULATORS
inline
void bcemt_ConditionImpl<bces_Platform::PosixThreads>::broadcast()
{
    pthread_cond_broadcast(&d_cond);
}

inline
void bcemt_ConditionImpl<bces_Platform::PosixThreads>::signal()
{
    pthread_cond_signal(&d_cond);
}

inline
int bcemt_ConditionImpl<bces_Platform::PosixThreads>::wait(bcemt_Mutex *mutex)
{
    return pthread_cond_wait(&d_cond, &mutex->nativeMutex());
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
