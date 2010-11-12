// bcemt_recursivemutex.h                                             -*-C++-*-
#ifndef INCLUDED_BCEMT_RECURSIVEMUTEX
#define INCLUDED_BCEMT_RECURSIVEMUTEX

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a platform-independent recursive mutex.
//
//@CLASSES:
//  bcemt_RecursiveMutex: platform-independent recursive mutex
//
//@SEE_ALSO: bcemt_mutex
//
//@DESCRIPTION: This component provides a mutually exclusive lock primitive
// ("mutex") that is "recursive" - a given thread can lock a recursive mutex
// multiple times, and then release it by unlocking it the same number of
// times.  The 'bcemt_RecursiveMutex' class provides the following operations:
// 'lock', 'tryLock', and 'unlock'.
//
// The non-recursive mutex 'bcemt_Mutex' has substantially lower overhead than
// 'bcemt_RecursiveMutex', and should be used instead if at all possible.  In
// particular, it is rare to need a recursive mutex.
//
// The behavior is undefined if 'unlock' is invoked on a 'bcemt_RecursiveMutex'
// object from a thread that does not currently own the lock.
//
///Usage
///-----
// As the name implies, 'bcemt_RecursiveMutex' supports multiple calls to
// 'lock', which *must* be balanced by a corresponding number of calls to
// 'unlock'.  Suppose that we are using a 'bcemt_RecursiveMutex' object to
// guarantee exclusive access to some object.  The following sketches the
// "recursive" nature of 'bcemt_RecursiveMutex':
//..
//  void foo(bcemt_RecursiveMutex *recMutex)
//  {
//..
// Assume that we do not have exclusive access upon entry to 'foo'.
//..
//      recMutex->lock();    // first level of locking
//..
// We have exclusive access here.
//..
//      recMutex->lock();    // second level of locking
//..
// We still have exclusive access.
//..
//      recMutex->unlock();  // release second level lock -- mutex stays locked
//..
// We *still* have exclusive access.
//..
//      recMutex->unlock();  // release first level lock -- mutex is unlocked
//..
// The two calls to 'unlock' have balanced the two earlier calls to 'lock'.
// Consequently, we no longer have exclusive access.
//..
//  }
//..
// Note that 'bcemt_RecursiveMutex' has substantially more overhead than does
// 'bcemt_Mutex'.  Consequently, the latter should be used unless recursive
// locking is truly warranted.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_RECURSIVEMUTEXIMPL_PTHREAD
#include <bcemt_recursivemuteximpl_pthread.h>
#endif

#ifndef INCLUDED_BCEMT_RECURSIVEMUTEXIMPL_WIN32
#include <bcemt_recursivemuteximpl_win32.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

namespace BloombergLP {

template <typename THREAD_POLICY>
class bcemt_RecursivemutexImpl;

                     // ==========================
                     // class btemt_RecursiveMutex
                     // ==========================

class bcemt_RecursiveMutex {
    // This 'class' implements a recursive mutex (i.e., a mutex that can be
    // locked any number of times by a thread, and then released by unlocking
    // the mutex the same number of times).  If there is an efficient native
    // recursive mutex, this class wraps it.  Otherwise, a reasonably efficient
    // proprietary implementation is used.  Note that 'bcemt_Mutex' should be
    // preferred if at all possible.

    // DATA
    bcemt_RecursiveMutexImpl<bces_Platform::ThreadPolicy>
                                    d_imp;  // platform-specific implementation

    // NOT IMPLEMENTED
    bcemt_RecursiveMutex(const bcemt_RecursiveMutex&);
    bcemt_RecursiveMutex& operator=(const bcemt_RecursiveMutex&);

  public:
    // CREATORS
    bcemt_RecursiveMutex();
        // Create a recursive mutex object in the unlocked state.

    ~bcemt_RecursiveMutex();
        // Destroy this recursive mutex object.

    // MANIPULATORS
    void lock();
        // Acquire a lock on this object.  If this object is currently locked,
        // then suspend execution of the current thread until a lock can be
        // acquired.  Succeed immediately if this thread already holds the
        // lock.

    int tryLock();
        // Attempt to acquire a lock on this object.  Return 0 on success, and
        // a non-zero value if this object is already locked by another thread,
        // or if an error occurs.  Succeed immediately if this thread already
        // holds the lock.

    void unlock();
        // Release a lock on this object that was previously acquired through a
        // call to 'lock', or a successful call to 'tryLock'.  To fully release
        // the lock, a thread must invoke 'unlock' the same number of times it
        // invoked 'lock' and 'tryLock'.  The behavior is undefined unless the
        // calling thread currently owns the lock on this recursive mutex.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------------
                        // class bcemt_RecursiveMutex
                        // --------------------------

// CREATORS
inline
bcemt_RecursiveMutex::bcemt_RecursiveMutex()
{
}

inline
bcemt_RecursiveMutex::~bcemt_RecursiveMutex()
{
}

// MANIPULATORS
inline
void bcemt_RecursiveMutex::lock()
{
    d_imp.lock();
}

inline
int bcemt_RecursiveMutex::tryLock()
{
    return d_imp.tryLock();
}

inline
void bcemt_RecursiveMutex::unlock()
{
    d_imp.unlock();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
