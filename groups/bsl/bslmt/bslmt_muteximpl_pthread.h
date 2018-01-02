// bslmt_muteximpl_pthread.h                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_MUTEXIMPL_PTHREAD
#define INCLUDED_BSLMT_MUTEXIMPL_PTHREAD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a POSIX implementation of 'bslmt::Mutex'.
//
//@CLASSES:
//  bslmt::MutexImpl<Platform::PosixThreads>: POSIX specialization
//
//@SEE_ALSO: bslmt_mutex
//
//@DESCRIPTION: This component provides an implementation of 'bslmt::Mutex' for
// POSIX threads ("pthreads"), 'bslmt::MutexImpl<Platform::PosixThreads>', via
// the template specialization:
//..
//  bslmt::MutexImpl<Platform::PosixThreads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bslmt::Mutex'.
//
///Usage
///-----
// This component is an implementation detail of 'bslmt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_PLATFORM
#include <bslmt_platform.h>
#endif

#ifdef BSLMT_PLATFORM_POSIX_THREADS

// Platform-specific implementation starts here.

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_PTHREAD
#include <pthread.h>
#define INCLUDED_PTHREAD
#endif

namespace BloombergLP {
namespace bslmt {

template <class THREAD_POLICY>
class MutexImpl;

                 // =======================================
                 // class MutexImpl<Platform::PosixThreads>
                 // =======================================

template <>
class MutexImpl<Platform::PosixThreads> {
    // This class provides a full specialization of 'MutexImpl' for pthreads.
    // It provides a efficient proxy for the 'pthread_mutex_t' pthreads type,
    // and related operations.  Note that the mutex implemented in this class
    // is *not* error checking, and is non-recursive.

    // DATA
    pthread_mutex_t d_lock;  // TBD doc

    // NOT IMPLEMENTED
    MutexImpl(const MutexImpl&);
    MutexImpl& operator=(const MutexImpl&);

  public:
    // PUBLIC TYPES
    typedef pthread_mutex_t NativeType;
       // The underlying OS-level type.  Exposed so that other 'bslmt'
       // components can operate directly on this mutex.

    // CREATORS
    MutexImpl();
        // Create a mutex initialized to an unlocked state.

    ~MutexImpl();
        // Destroy this mutex object.  The behavior is undefined in the mutex
        // is in a locked state.

    // MANIPULATORS
    void lock();
        // Acquire a lock on this mutex object.  If this object is currently
        // locked, then suspend execution of the current thread until a lock
        // can be acquired.  Note that the behavior is undefined if the calling
        // thread already owns the lock on this mutex, and will likely result
        // in a deadlock.

    NativeType& nativeMutex();
        // Return a reference to the modifiable OS-level mutex underlying this
        // object.  This method is intended only to support other 'bslmt'
        // components that must operate directly on this mutex.

    int tryLock();
        // Attempt to acquire a lock on this mutex object.  Return 0 on
        // success, and a non-zero value of this object is already locked, or
        // if an error occurs.

    void unlock();
        // Release a lock on this mutex that was previously acquired through a
        // successful call to 'lock', or 'tryLock'.  The behavior is undefined,
        // unless the calling thread currently owns the lock on this mutex.
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                 // ---------------------------------------
                 // class MutexImpl<Platform::PosixThreads>
                 // ---------------------------------------

// CREATORS
inline
bslmt::MutexImpl<bslmt::Platform::PosixThreads>::MutexImpl()
{
    const int status = pthread_mutex_init(&d_lock, 0);
    (void) status;
    BSLS_ASSERT_SAFE(0 == status);
}

// MANIPULATORS
inline
void bslmt::MutexImpl<bslmt::Platform::PosixThreads>::lock()
{
    BSLS_ASSERT_SAFE(0xdeadbeef !=
                                 *reinterpret_cast<const unsigned *>(&d_lock));

    const int status = pthread_mutex_lock(&d_lock);
    (void) status;
    BSLS_ASSERT_SAFE(0 == status);
}

inline
int bslmt::MutexImpl<bslmt::Platform::PosixThreads>::tryLock()
{
    BSLS_ASSERT_SAFE(0xdeadbeef !=
                                 *reinterpret_cast<const unsigned *>(&d_lock));

    return pthread_mutex_trylock(&d_lock);
}

inline
void bslmt::MutexImpl<bslmt::Platform::PosixThreads>::unlock()
{
    BSLS_ASSERT_SAFE(0xdeadbeef !=
                                 *reinterpret_cast<const unsigned *>(&d_lock));

    const int status = pthread_mutex_unlock(&d_lock);
    (void) status;
    BSLS_ASSERT_SAFE(0 == status);
}

inline
bslmt::MutexImpl<bslmt::Platform::PosixThreads>::NativeType&
bslmt::MutexImpl<bslmt::Platform::PosixThreads>::nativeMutex()
{
    return d_lock;
}

}  // close enterprise namespace

#endif  // BSLMT_PLATFORM_POSIX_THREADS

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
