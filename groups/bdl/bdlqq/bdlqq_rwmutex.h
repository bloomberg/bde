// bdlqq_rwmutex.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLQQ_RWMUTEX
#define INCLUDED_BDLQQ_RWMUTEX

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a platform-independent RW mutex class.
//
//@AUTHOR: David Schumann (dschumann1)
//
//@CLASSES:
//   bdlqq::RWMutex: platform-independent wrapper of an RW mutex
//
//@SEE_ALSO: bdlqq_readerwriterlock
//
//@DESCRIPTION: This component provides a class, 'bdlqq::RWMutex', that defines
// a platform-independent RW mutex.  An RW mutex provides for a shared "read"
// lock that may be held simultaneously by any number of threads, and a "write"
// lock that is exclusive (i.e., it may be held by only one thread at a time).
// The "write" lock is also exclusive with the "read" lock, so that no threads
// may hold a "read" lock while the "write" lock is held, and vice versa.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLQQ_PLATFORM
#include <bdlqq_platform.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#if defined(BDLQQ_PLATFORM_WIN32_THREADS) || defined(BSLS_PLATFORM_OS_AIX)
#ifndef INCLUDED_BDLQQ_READERWRITERLOCK
#include <bdlqq_readerwriterlock.h>
#endif
#endif

#ifdef BDLQQ_PLATFORM_POSIX_THREADS
#ifndef INCLUDED_PTHREAD
#include <pthread.h>
#define INCLUDED_PTHREAD
#endif
#endif

namespace BloombergLP {
namespace bdlqq {

template <class THREAD_POLICY>
struct RWMutexImpl;

}  // close package namespace

#ifdef BDLQQ_PLATFORM_POSIX_THREADS

namespace bdlqq {
                    // ================================
                    // struct RWMutexImpl<PosixThreads>
                    // ================================

template <>
struct RWMutexImpl<Platform::PosixThreads> {
    // This is a platform-specific implementation detail that is not intended
    // for use outside of this component.  Use the 'RWMutex' class
    // instead.  This structure is a wrapper around a POSIX RW lock on Sun (on
    // AIX the POSIX RW lock has poor performance and no writer guarantees).

  private:
    // DATA
    pthread_rwlock_t d_lock;

  public:
    // CREATORS
    RWMutexImpl();
    ~RWMutexImpl();

    // MANIPULATORS
    void lockRead();
    void lockWrite();
    int tryLockRead();
    int tryLockWrite();
    void unlock();
};

}  // close package namespace

#endif  // BDLQQ_PLATFORM_POSIX_THREADS

namespace bdlqq {
                            // =============
                            // class RWMutex
                            // =============

class RWMutex {
    // This class is a platform-independent interface to a reader-writer lock
    // ("RW mutex").  Multiple readers can safely hold the lock simultaneously,
    // whereas only one writer is allowed to hold the lock at a time.  This
    // class uses the most efficient RW mutex implementation available for the
    // current platform that provides a "writer bias."  A "writer bias" is a
    // guarantee that writers will not be starved by having reader threads
    // continuously acquiring the shared lock.

    // DATA
#if defined(BSLS_PLATFORM_OS_AIX) || defined(BDLQQ_PLATFORM_WIN32_THREADS)
    ReaderWriterLock d_impl;
#else
    RWMutexImpl<Platform::ThreadPolicy> d_impl;
#endif

    // NOT IMPLEMENTED
    RWMutex(const RWMutex&);
    RWMutex& operator=(const RWMutex&);

  public:
    // CREATORS
    RWMutex();
        // Create an RW mutex initialized to an unlocked state.

    ~RWMutex();
        // Destroy this RW mutex.

    // MANIPULATORS
    void lockRead();
        // Lock this RW mutex for reading.  If there are no pending or active
        // write locks, this method will return immediately; otherwise, it will
        // block until all write locks have been released.  'unlock' must be
        // called to release the lock.  The behavior is undefined if this
        // method is called from a thread that already has a lock on this RW
        // mutex.

    void lockWrite();
        // Lock this RW mutex for writing.  This method will block until all
        // active read locks, and all pending or active write locks, have been
        // released.  When this RW mutex is locked for writing, all lock
        // attempts will either fail ('tryLockRead', 'tryLockWrite') or block
        // ('lockRead', 'lockWrite') until the lock is released.  'unlock' must
        // be called to release the lock.  The behavior is undefined if this
        // method is called from a thread that already has a lock on this RW
        // mutex.

    int tryLockRead();
        // Attempt to lock this RW mutex for reading.  Return 0 on success, and
        // a non-zero value if this RW mutex is currently locked for writing,
        // or if there are writers waiting for this lock.  If successful,
        // 'unlock' must be called to release the lock.

    int tryLockWrite();
        // Attempt to lock this RW mutex for writing.  Return 0 on success, and
        // a non-zero value if this RW mutex is already locked.  If successful,
        // 'unlock' must be called to release the lock.

    void unlock();
        // Release the lock that the calling thread holds on this RW mutex.
        // The behavior is undefined unless the calling thread currently has a
        // lock on this RW mutex.
};

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

#ifdef BDLQQ_PLATFORM_POSIX_THREADS

                            // ------------------
                            // struct RWMutexImpl
                            // ------------------

// CREATORS
inline
bdlqq::RWMutexImpl<bdlqq::Platform::PosixThreads>::RWMutexImpl()
{
    const int rc = pthread_rwlock_init(&d_lock, NULL);

    // pthread_rwlock_init should not return a failure code.

    BSLS_ASSERT_SAFE(0 == rc);
    (void) rc;    // suppress 'unused variable' warnings
}

inline
bdlqq::RWMutexImpl<bdlqq::Platform::PosixThreads>::~RWMutexImpl()
{
    const int rc = pthread_rwlock_destroy(&d_lock);

    // pthread_rwlock_destroy should not return a failure code.

    BSLS_ASSERT_SAFE(0 == rc);
    (void) rc;    // suppress 'unused variable' warnings
}

// MANIPULATORS
inline
void
bdlqq::RWMutexImpl<bdlqq::Platform::PosixThreads>::lockRead()
{
    const int rc = pthread_rwlock_rdlock(&d_lock);

    // pthread_rwlock_rdlock should not return a failure code.

    BSLS_ASSERT_SAFE(0 == rc);
    (void) rc;    // suppress 'unused variable' warnings
}

inline
void
bdlqq::RWMutexImpl<bdlqq::Platform::PosixThreads>::lockWrite()
{
    const int rc = pthread_rwlock_wrlock(&d_lock);

    // pthread_rwlock_wrlock should not return a failure code.

    BSLS_ASSERT_SAFE(0 == rc);
    (void) rc;    // suppress 'unused variable' warnings
}

inline
int
bdlqq::RWMutexImpl<bdlqq::Platform::PosixThreads>::tryLockRead()
{
    return pthread_rwlock_tryrdlock(&d_lock) ? 1 : 0;
}

inline
int
bdlqq::RWMutexImpl<bdlqq::Platform::PosixThreads>::tryLockWrite()
{
    return pthread_rwlock_trywrlock(&d_lock) ? 1 : 0;
}

inline
void
bdlqq::RWMutexImpl<bdlqq::Platform::PosixThreads>::unlock()
{
    pthread_rwlock_unlock(&d_lock);
}

#endif  // BDLQQ_PLATFORM_POSIX_THREADS

                            // -------------
                            // class RWMutex
                            // -------------

// CREATORS
inline
bdlqq::RWMutex::RWMutex()
{
}

inline
bdlqq::RWMutex::~RWMutex()
{
}

// MANIPULATORS
inline
void bdlqq::RWMutex::lockRead()
{
    d_impl.lockRead();
}

inline
void bdlqq::RWMutex::lockWrite()
{
    d_impl.lockWrite();
}

inline
int bdlqq::RWMutex::tryLockRead()
{
    return d_impl.tryLockRead();
}

inline
int bdlqq::RWMutex::tryLockWrite()
{
    return d_impl.tryLockWrite();
}

inline
void bdlqq::RWMutex::unlock()
{
    d_impl.unlock();
}

}  // close enterprise namespace

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
