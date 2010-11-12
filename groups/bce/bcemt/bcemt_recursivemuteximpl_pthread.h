// bcemt_recursivemuteximpl_pthread.h                                 -*-C++-*-
#ifndef INCLUDED_BCEMT_RECURSIVEMUTEXIMPL_PTHREAD
#define INCLUDED_BCEMT_RECURSIVEMUTEXIMPL_PTHREAD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a POSIX implementation of 'bcemt_RecursiveMutex'.
//
//@CLASSES:
//  bcemt_RecursiveMutexImpl<PosixThreads>: POSIX specialization
//
//@SEE_ALSO: bcemt_recursivemutex
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of
// 'bcemt_RecursiveMutex' for POSIX threads ("pthreads") via the template
// specialization:
//..
//  bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_RecursiveMutex'.
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

#ifndef INCLUDED_PTHREAD
#include <pthread.h>
#define INCLUDED_PTHREAD
#endif

#ifndef PTHREAD_MUTEX_RECURSIVE
#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>  // for recursive mutex
#endif
#endif

namespace BloombergLP {

template <typename THREAD_POLICY>
class bcemt_RecursiveMutexImpl;

          // ===========================================================
          // class bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>
          // ===========================================================

template <>
class bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads> {
    // This class provides a full specialization of 'bcemt_RecursiveMutexImpl'
    // for pthreads.  If the pthreads implementation supports the "recursive"
    // attribute, then the native implementation is used, otherwise, a
    // portable, efficient implementation is provided.

    // DATA
    pthread_mutex_t d_lock;       // TBD doc

#ifndef PTHREAD_MUTEX_RECURSIVE
    bces_SpinLock   d_spin;       // spin lock controlling access to this
                                  // object

    pthread_t       d_owner;      // thread id of thread currently owning this
                                  // lock object

    int             d_lockCount;  // current lock recursion level
#endif

    // NOT IMPLEMENTED
    bcemt_RecursiveMutexImpl(const bcemt_RecursiveMutexImpl&);
    bcemt_RecursiveMutexImpl& operator=(const bcemt_RecursiveMutexImpl&);

  public:
    // CREATORS
    bcemt_RecursiveMutexImpl();
        // Create a recursive mutex initialized to an unlocked state.

    ~bcemt_RecursiveMutexImpl();
        // Destroy this recursive mutex object.

    // MANIPULATORS
    void lock();
        // Acquire a lock on this mutex object.  If this object is currently
        // locked by a different thread, then suspend execution of the current
        // thread until a lock can be acquired.  Otherwise, if it unlocked,
        // or locked by the calling thread, then grant ownership of the lock
        // immediately and return.  Note that when this object is recursively
        // locked by a thread, 'unlock' must be called an equal number of
        // times before the lock is actually released.

    int tryLock();
        // Attempt to acquire a lock on this mutex object.  If this object is
        // unlocked, or locked by the calling thread, then grant ownership of
        // the lock immediately and return 0.  Otherwise If this object is
        // currently locked by a different thread or if an error occurs, then
        // return a non-zero value.  Note that when this object is recursively
        // locked by a thread, 'unlock' must be called an equal number of times
        //  before the lock is actually released.

    void unlock();
        // Release a lock on this mutex that was previously acquired through a
        // successful call to 'lock', or 'tryLock'.  The behavior is
        // undefined, unless the calling thread currently owns the lock on this
        // mutex.  Note that when this object is recursively locked by a
        // thread, 'unlock' must be called an equal number of times before the
        // lock is actually released.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

          // -----------------------------------------------------------
          // class bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>
          // -----------------------------------------------------------

// CREATORS
inline
bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>::
                                                  ~bcemt_RecursiveMutexImpl()
{
    pthread_mutex_destroy(&d_lock);
}

}  // close namespace BloombergLP

#endif // BCES_PLATFORM__POSIX_THREADS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
