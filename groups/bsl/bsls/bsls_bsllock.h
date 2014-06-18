// bsls_bsllock.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLS_BSLLOCK
#define INCLUDED_BSLS_BSLLOCK

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a platform-independent mutex for use below 'bslmt'.
//
//@CLASSES:
//  bsls::BslLock: platform-independent mutex
//  bsls::BslLockGuard: RAII mechanism for locking/unlocking a 'BslLock'
//
//@SEE_ALSO: bslmt_mutex
//
//@DESCRIPTION: This component provides a mutually exclusive lock primitive
// ("mutex") by wrapping a suitable platform-specific mechanism.  The
// 'bsls::BslLock' class provides 'lock' and 'unlock' operations.  Note that
// 'bsls::BslLock' is not intended for direct client use; see 'bslmt_mutex'
// instead.  Also note that 'bsls::BslLock' is not recursive.
//
// This component also provides the 'bsls::BslLockGuard' class, a mechanism
// that follows the RAII idiom for automatically acquiring and releasing the
// lock on an associated 'bsls::BslLock' object.  To ensure exception safety,
// client code should make use of a 'bsls::BslLockGuard' object wherever
// appropriate rather than calling the methods on the associated
// 'bsls::BslLock' object directly.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Using 'bsls::BslLock' to Make a 'class' Thread-Safe
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate the use of 'bsls::BslLock' and
// 'bsls::BslLockGuard' to write a thread-safe class.
//
// First, we provide an elided definition of the 'my_Account' class.  Note the
// 'd_lock' data member of type 'bsls::BslLock':
//..
//  class my_Account {
//      // This 'class' implements a very simplistic bank account.  It is meant
//      // for illustrative purposes only.
//
//      // DATA
//      double                d_money;  // amount of money in the account
//      mutable bsls::BslLock d_lock;   // ensure exclusive access to 'd_money'
//
//    // ...
//
//    public:
//
//      // ...
//
//      // MANIPULATORS
//      void deposit(double amount);
//          // Atomically deposit the specified 'amount' of money into this
//          // account.  The behavior is undefined unless 'amount >= 0.0'.
//
//      int withdraw(double amount);
//          // Atomically withdraw the specified 'amount' of money from this
//          // account.  Return 0 on success, and a non-zero value otherwise.
//          // The behavior is undefined unless 'amount >= 0.0'.
//
//      // ...
//  };
//..
// Next, we show the implementation of the two 'my_Account' manipulators
// show-casing the use of 'bsls::BslLock' and 'bsls::BslLockGuard':
//..
//  // MANIPULATORS
//  void my_Account::deposit(double amount)
//  {
//..
// Here, we use the interface of 'bsls::BslLock' directly.  However, wherever
// appropriate, a 'bsls::BslLockGuard' object should be used instead to ensure
// that an acquired lock is always properly released, even if an exception is
// thrown:
//..
//      BSLS_ASSERT(amount >= 0.0);
//
//      d_lock.lock();  // consider using 'bsls::BslLockGuard' (see 'withdraw')
//      d_money += amount;
//      d_lock.unlock();
//  }
//..
// In contrast, 'withdraw' uses a 'bsls::BslLockGuard' to automatically acquire
// and release the lock.  The lock is acquired as a side-effect of the
// construction of 'guard', and released when 'guard' is destroyed upon
// returning from the function:
//..
//  int my_Account::withdraw(double amount)
//  {
//      BSLS_ASSERT(amount >= 0.0);
//
//      bsls::BslLockGuard guard(&d_lock);  // a very good practice
//
//      if (amount <= d_money) {
//          d_money -= amount;
//          return 0;
//      }
//      else {
//          return -1;
//      }
//  }
//..

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS

#ifndef INCLUDED_WTYPES
#include <wtypes.h>
#define INCLUDED_WTYPES
#endif

#ifndef INCLUDED_WINBASE
#include <winbase.h>
#define INCLUDED_WINBASE
#endif

#else

#ifndef INCLUDED_PTHREAD
#include <pthread.h>
#define INCLUDED_PTHREAD
#endif

#endif

namespace BloombergLP {
namespace bsls {

                             // =============
                             // class BslLock
                             // =============

class BslLock {
    // This 'class' implements a light-weight, portable wrapper of an OS-level
    // mutex to support intra-process synchronization.  The mutex implemented
    // by this class is *non*-recursive.  Note that 'BslLock' is *not* intended
    // for direct use by client code; it is meant for internal use only.

    // DATA
#ifdef BSLS_PLATFORM_OS_WINDOWS
    CRITICAL_SECTION d_lock;  // Windows critical section
#else
    pthread_mutex_t  d_lock;  // pthreads mutex object
#endif

  private:
    // NOT IMPLEMENTED
    BslLock(const BslLock&);             // = delete
    BslLock& operator=(const BslLock&);  // = delete

  public:
    // CREATORS
    BslLock();
        // Create a lock object initialized to the unlocked state.

    ~BslLock();
        // Destroy this lock object.  The behavior is undefined unless this
        // object is in the unlocked state.

    // MANIPULATORS
    void lock();
        // Acquire the lock on this object.  If the lock on this object is
        // currently held by another thread, then suspend execution of the
        // calling thread until the lock can be acquired.  The behavior is
        // undefined unless the calling thread does not already hold the lock
        // on this object.  Note that deadlock may result if this method is
        // invoked while the calling thread holds the lock on the object.

    void unlock();
        // Release the lock on this object that was previously acquired
        // through a call to 'lock', enabling another thread to acquire the
        // lock.  The behavior is undefined unless the calling thread holds the
        // lock on this object.
};

                             // ==================
                             // class BslLockGuard
                             // ==================

class BslLockGuard {
    // This 'class' implements a guard for automatically acquiring and
    // releasing the lock on an associated 'bsls::BslLock' object.  This
    // mechanism follows the RAII idiom whereby the lock on the 'BslLock'
    // associated with a guard object is acquired upon construction and
    // released upon destruction.

    // DATA
    BslLock *d_lock_p;  // lock guarded by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    BslLockGuard(const BslLockGuard&);             // = delete
    BslLockGuard& operator=(const BslLockGuard&);  // = delete

  public:
    // CREATORS
    explicit BslLockGuard(BslLock *lock);
        // Create a guard object that conditionally manages the specified
        // 'lock', and acquires the lock on 'lock' by invoking its 'lock'
        // method.  The behavior is undefined unless the calling thread does
        // not already hold the lock on 'lock'.  Note that deadlock may result
        // if a guard is created for 'lock' while the calling thread holds the
        // lock on 'lock'.  Also note that 'lock' must remain valid throughout
        // the lifetime of this guard, or until 'release' is called.

    ~BslLockGuard();
        // Destroy this guard object and release the lock on the object it
        // manages (if any) by invoking the 'unlock' method of the object that
        // was supplied at construction of this guard.  If no lock is currently
        // being managed, this method has no effect.  Note that if this guard
        // object currently manages a lock, this method assumes the behavior
        // of 'BslLock::unlock'.

    // MANIPULATORS
    void release();
        // Release from management, with no effect, the object currently
        // managed by this guard, if any.  Note that 'unlock' is *not* called
        // on the managed object upon its release.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // -------------
                             // class BslLock
                             // -------------

// CREATORS
inline
BslLock::BslLock()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    enum {
        // A Windows critical section has a configurable spin count.  A lock
        // operation spins this many iterations (on, presumably, some atomic
        // integer) before sleeping on the underlying primitive.

        BSLS_SPIN_COUNT = 30
    };

    InitializeCriticalSectionAndSpinCount(&d_lock, BSLS_SPIN_COUNT);
#else
    const int status = pthread_mutex_init(&d_lock, 0);
    (void)status;
    BSLS_ASSERT_SAFE(0 == status);
#endif
}

inline
BslLock::~BslLock()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    DeleteCriticalSection(&d_lock);
#else
    const int status = pthread_mutex_destroy(&d_lock);
    (void)status;
    BSLS_ASSERT_SAFE(0 == status);
#endif
}

// MANIPULATORS
inline
void BslLock::lock()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    EnterCriticalSection(&d_lock);
#else
    const int status = pthread_mutex_lock(&d_lock);
    (void)status;
    BSLS_ASSERT_SAFE(0 == status);
#endif
}

inline
void BslLock::unlock()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    LeaveCriticalSection(&d_lock);
#else
    const int status = pthread_mutex_unlock(&d_lock);
    (void)status;
    BSLS_ASSERT_SAFE(0 == status);
#endif
}

                             // ------------------
                             // class BslLockGuard
                             // ------------------

// CREATORS
inline
BslLockGuard::BslLockGuard(BslLock *lock)
: d_lock_p(lock)
{
    BSLS_ASSERT_SAFE(lock);

    d_lock_p->lock();
}

inline
BslLockGuard::~BslLockGuard()
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

// MANIPULATORS
inline
void BslLockGuard::release()
{
    d_lock_p = 0;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
