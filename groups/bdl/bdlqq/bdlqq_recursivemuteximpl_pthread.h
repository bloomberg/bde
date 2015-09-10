// bdlqq_recursivemuteximpl_pthread.h                                 -*-C++-*-
#ifndef INCLUDED_BDLQQ_RECURSIVEMUTEXIMPL_PTHREAD
#define INCLUDED_BDLQQ_RECURSIVEMUTEXIMPL_PTHREAD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a POSIX implementation of 'bdlqq::RecursiveMutex'.
//
//@CLASSES:
//  bdlqq::RecursiveMutexImpl<PosixThreads>: POSIX specialization
//
//@SEE_ALSO: bdlqq_recursivemutex
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of
// 'bdlqq::RecursiveMutex' for POSIX threads ("pthreads") via the template
// specialization:
//..
//  bdlqq::RecursiveMutexImpl<Platform::PosixThreads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bdlqq::RecursiveMutex'.
//
///Usage
///-----
// This component is an implementation detail of 'bcemt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLQQ_PLATFORM
#include <bdlqq_platform.h>
#endif

#ifdef BDLQQ_PLATFORM_POSIX_THREADS

// Platform-specific implementation starts here.

#ifndef INCLUDED_PTHREAD
#include <pthread.h>
#define INCLUDED_PTHREAD
#endif

#ifndef PTHREAD_MUTEX_RECURSIVE
#ifndef INCLUDED_BSLS_SPINLOCK
#include <bsls_spinlock.h>
#endif
#endif

namespace BloombergLP {
namespace bdlqq {

template <class THREAD_POLICY>
class RecursiveMutexImpl;

             // ================================================
             // class RecursiveMutexImpl<Platform::PosixThreads>
             // ================================================

template <>
class RecursiveMutexImpl<Platform::PosixThreads> {
    // This class provides a full specialization of 'RecursiveMutexImpl' for
    // pthreads.  If the pthreads implementation supports the "recursive"
    // attribute, then the native implementation is used, otherwise, a
    // portable, efficient implementation is provided.

    // DATA
    pthread_mutex_t d_lock;       // TBD doc

#ifndef PTHREAD_MUTEX_RECURSIVE
    bsls::SpinLock  d_spin;       // spin lock controlling access to this
                                  // object

    pthread_t       d_owner;      // thread id of thread currently owning this
                                  // lock object

    int             d_lockCount;  // current lock recursion level
#endif

    // NOT IMPLEMENTED
    RecursiveMutexImpl(const RecursiveMutexImpl&);
    RecursiveMutexImpl& operator=(const RecursiveMutexImpl&);

  public:
    // CREATORS
    RecursiveMutexImpl();
        // Create a recursive mutex initialized to an unlocked state.

    ~RecursiveMutexImpl();
        // Destroy this recursive mutex object.

    // MANIPULATORS
    void lock();
        // Acquire a lock on this mutex object.  If this object is currently
        // locked by a different thread, then suspend execution of the current
        // thread until a lock can be acquired.  Otherwise, if it unlocked, or
        // locked by the calling thread, then grant ownership of the lock
        // immediately and return.  Note that when this object is recursively
        // locked by a thread, 'unlock' must be called an equal number of times
        // before the lock is actually released.

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
        // successful call to 'lock', or 'tryLock'.  The behavior is undefined,
        // unless the calling thread currently owns the lock on this mutex.
        // Note that when this object is recursively locked by a thread,
        // 'unlock' must be called an equal number of times before the lock is
        // actually released.
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

             // ------------------------------------------------
             // class RecursiveMutexImpl<Platform::PosixThreads>
             // ------------------------------------------------

// CREATORS
inline
bdlqq::RecursiveMutexImpl<bdlqq::Platform::PosixThreads>::~RecursiveMutexImpl()
{
    pthread_mutex_destroy(&d_lock);
}

}  // close enterprise namespace

#endif // BDLQQ_PLATFORM_POSIX_THREADS

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
