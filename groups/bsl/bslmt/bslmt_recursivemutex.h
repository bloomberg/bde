// bslmt_recursivemutex.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_RECURSIVEMUTEX
#define INCLUDED_BSLMT_RECURSIVEMUTEX

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a platform-independent recursive mutex.
//
//@CLASSES:
//  bslmt::RecursiveMutex: platform-independent recursive mutex
//
//@SEE_ALSO: bslmt_mutex
//
//@DESCRIPTION: This component provides a mutually exclusive lock primitive
// ("mutex") that is "recursive" - a given thread can lock a recursive mutex
// multiple times, and then release it by unlocking it the same number of
// times.  The 'bslmt::RecursiveMutex' class provides the following operations:
// 'lock', 'tryLock', and 'unlock'.
//
// The non-recursive mutex 'bslmt::Mutex' has substantially lower overhead than
// 'bslmt::RecursiveMutex', and should be used instead if at all possible.  In
// particular, it is rare to need a recursive mutex.
//
// The behavior is undefined if 'unlock' is invoked on a
// 'bslmt::RecursiveMutex' object from a thread that does not currently own the
// lock.
//
///Usage
///-----
// As the name implies, 'bslmt::RecursiveMutex' supports multiple calls to
// 'lock', which *must* be balanced by a corresponding number of calls to
// 'unlock'.  Suppose that we are using a 'bslmt::RecursiveMutex' object to
// guarantee exclusive access to some object.  The following sketches the
// "recursive" nature of 'bslmt::RecursiveMutex':
//..
//  bslmt::RecursiveMutex recMutex;
//..
// Assume that we do not have exclusive access to the object.
//..
//  recMutex.lock();    // first level of locking
//..
// We have exclusive access here.
//..
//  recMutex.lock();    // second level of locking
//..
// We still have exclusive access.
//..
//  recMutex.unlock();  // release second level lock -- mutex stays locked
//..
// We *still* have exclusive access.
//..
//  recMutex.unlock();  // release first level lock -- mutex is unlocked
//..
// The two calls to 'unlock' have balanced the two earlier calls to 'lock'.
// Consequently, we no longer have exclusive access.
//
// Note that 'bslmt::RecursiveMutex' has substantially more overhead than does
// 'bslmt::Mutex'.  Consequently, the latter should be used unless recursive
// locking is truly warranted.

#include <bslscm_version.h>

#include <bslmt_recursivemuteximpl_pthread.h>
#include <bslmt_recursivemuteximpl_win32.h>
#include <bslmt_platform.h>

namespace BloombergLP {
namespace bslmt {

template <class THREAD_POLICY>
class RecursivemutexImpl;

                           // ====================
                           // class RecursiveMutex
                           // ====================

class RecursiveMutex {
    // This 'class' implements a recursive mutex (i.e., a mutex that can be
    // locked any number of times by a thread, and then released by unlocking
    // the mutex the same number of times).  If there is an efficient native
    // recursive mutex, this class wraps it.  Otherwise, a reasonably efficient
    // proprietary implementation is used.  Note that 'Mutex' should be
    // preferred if at all possible.

    // DATA
    RecursiveMutexImpl<Platform::ThreadPolicy> d_imp;  // platform-specific
                                                       // implementation

    // NOT IMPLEMENTED
    RecursiveMutex(const RecursiveMutex&);
    RecursiveMutex& operator=(const RecursiveMutex&);

  public:
    // CREATORS
    RecursiveMutex();
        // Create a recursive mutex object in the unlocked state.  This method
        // does not return normally unless there are sufficient system
        // resources to construct the object.

    ~RecursiveMutex();
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

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                           // --------------------
                           // class RecursiveMutex
                           // --------------------

// CREATORS
inline
bslmt::RecursiveMutex::RecursiveMutex()
{
}

inline
bslmt::RecursiveMutex::~RecursiveMutex()
{
}

// MANIPULATORS
inline
void bslmt::RecursiveMutex::lock()
{
    d_imp.lock();
}

inline
int bslmt::RecursiveMutex::tryLock()
{
    return d_imp.tryLock();
}

inline
void bslmt::RecursiveMutex::unlock()
{
    d_imp.unlock();
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
