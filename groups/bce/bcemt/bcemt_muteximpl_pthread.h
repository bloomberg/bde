// bcemt_muteximpl_pthread.h                                          -*-C++-*-
#ifndef INCLUDED_BCEMT_MUTEXIMPL_PTHREAD
#define INCLUDED_BCEMT_MUTEXIMPL_PTHREAD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a POSIX implementation of 'bcemt_Mutex'.
//
//@CLASSES:
//  bcemt_MutexImpl<bces_Platform::PosixThreads>: POSIX specialization
//
//@SEE_ALSO: bcemt_mutex
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides an implementation of 'bcemt_Mutex' for
// POSIX threads ("pthreads") via the template specialization:
//..
//  bcemt_MutexImpl<bces_Platform::PosixThreads>
//..
// This template class should not be used (directly) by client code.  Clients
// should instead use 'bcemt_Mutex'.
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

template <typename THREAD_POLICY>
class bcemt_MutexImpl;

               // ==================================================
               // class bcemt_MutexImpl<bces_Platform::PosixThreads>
               // ==================================================

template <>
class bcemt_MutexImpl<bces_Platform::PosixThreads> {
    // This class provides a full specialization of 'bcemt_MutexImpl' for
    // pthreads.  It provides a efficient proxy for the 'pthread_mutex_t'
    // pthreads type, and related operations.  Note that the mutex implemented
    // in this class is *not* error checking, and is non-recursive.

    // DATA
    pthread_mutex_t d_lock;  // TBD doc

    // NOT IMPLEMENTED
    bcemt_MutexImpl(const bcemt_MutexImpl&);
    bcemt_MutexImpl& operator=(const bcemt_MutexImpl&);

  public:
    // PUBLIC TYPES
    typedef pthread_mutex_t NativeType;
       // The underlying OS-level type.  Exposed so that other bcemt components
       // can operate directly on this mutex.

    // CREATORS
    bcemt_MutexImpl();
        // Create a mutex initialized to an unlocked state.

    ~bcemt_MutexImpl();
        // Destroy this mutex object.  The behavior is undefined in the mutex
        // is in a locked state.

    // MANIPULATORS
    void lock();
        // Acquire a lock on this mutex object.  If this object is currently
        // locked, then suspend execution of the current thread until a
        // lock can be acquired.  Note that the behavior is undefined if the
        // calling thread already owns the lock on this mutex, and will likely
        // result in a deadlock.

    NativeType& nativeMutex();
        // Return a reference to the modifiable OS-level mutex underlying this
        // object.  This method is intended only to support other bcemt
        // components that must operate directly on this mutex.

    int tryLock();
        // Attempt to acquire a lock on this mutex object.  Return 0 on
        // success, and a non-zero value of this object is already locked, or
        // if an error occurs.

    void unlock();
        // Release a lock on this mutex that was previously acquired through a
        // successful call to 'lock', or 'tryLock'.  The behavior is
        // undefined, unless the calling thread currently owns the lock on this
        // mutex.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

               // --------------------------------------------------
               // class bcemt_MutexImpl<bces_Platform::PosixThreads>
               // --------------------------------------------------

// CREATORS
inline
bcemt_MutexImpl<bces_Platform::PosixThreads>::bcemt_MutexImpl()
{
    const int status = pthread_mutex_init(&d_lock, 0);
    (void) status;
    BSLS_ASSERT_SAFE(0 == status);
}

// MANIPULATORS
inline
void bcemt_MutexImpl<bces_Platform::PosixThreads>::lock()
{
    BSLS_ASSERT_SAFE(0xdeadbeef !=
                                 *reinterpret_cast<const unsigned *>(&d_lock));

    const int status = pthread_mutex_lock(&d_lock);
    (void) status;
    BSLS_ASSERT_SAFE(0 == status);
}

inline
int bcemt_MutexImpl<bces_Platform::PosixThreads>::tryLock()
{
    BSLS_ASSERT_SAFE(0xdeadbeef !=
                                 *reinterpret_cast<const unsigned *>(&d_lock));

    return pthread_mutex_trylock(&d_lock);
}

inline
void bcemt_MutexImpl<bces_Platform::PosixThreads>::unlock()
{
    BSLS_ASSERT_SAFE(0xdeadbeef !=
                                 *reinterpret_cast<const unsigned *>(&d_lock));

    const int status = pthread_mutex_unlock(&d_lock);
    (void) status;
    BSLS_ASSERT_SAFE(0 == status);
}

inline
bcemt_MutexImpl<bces_Platform::PosixThreads>::NativeType&
bcemt_MutexImpl<bces_Platform::PosixThreads>::nativeMutex()
{
    return d_lock;
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
