// bslmt_lockguard.h                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_LOCKGUARD
#define INCLUDED_BSLMT_LOCKGUARD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a generic proctor for synchronization objects.
//
//@CLASSES:
//  bslmt::LockGuard: automatic mutex locking-unlocking
//  bslmt::LockGuardUnlock: automatic mutex unlocking-locking
//  bslmt::LockGuardTryLock: automatic non-blocking mutex locking-unlocking
//  bslmt::UnLockGuard: DEPRECATED
//  bslmt::TryLockGuard: DEPRECATED
//
//@SEE_ALSO: bslmt_readlockguard, bslmt_writelockguard
//
//@DESCRIPTION: This component provides generic proctors, 'bslmt::LockGuard',
// 'bslmt::LockGuardUnlock', 'bslmt::LockGuardTryLock', 'bslmt::UnLockGuard',
// and 'bslmt::TryLockGuard', to automatically lock and unlock an external
// synchronization object.  The synchronization object can be any type (e.g.,
// 'bslmt::Mutex' or 'bslmt::RecursiveMutex') that provides the following
// methods:
//..
//  void lock();
//  void unlock();
//..
// Both 'bslmt::LockGuard' and 'bslmt::LockGuardUnlock' implement the
// "construction is acquisition, destruction is release" idiom.  During
// construction, 'bslmt::LockGuard' automatically calls 'lock' on the
// user-supplied object, and 'unlock' when it is destroyed (unless released).
// 'bslmt::LockGuardUnlock' does the opposite -- it invokes the 'unlock' method
// when constructed and the 'lock' method when it is destroyed.
//
// A third type of guard, 'bslmt::LockGuardTryLock', attempts to acquire a
// lock, and if acquisition succeeds, releases it upon destruction.  Since the
// acquisition is done at construction time, it is not possible to return a
// value to indicate success.  Rather, the 'bslmt::LockGuardTryLock' contains a
// pointer to the synchronization object if 'tryLock' succeeds, and is null
// otherwise.  The synchronization object can be any type (e.g., 'bslmt::Mutex'
// or 'bslmt::RecursiveMutex') that provides the following methods:
//..
//  void tryLock();
//  void unlock();
//..
// Note that none of these guard types assumes ownership of the external
// synchronization object.  Also note that objects of all of the guard types
// may be constructed with a null 'lock' whereby the constructed guard objects
// proctor no lock.  The destructor of each of the guard types has no effect if
// no lock is under management.
//
///Behavior of the 'release' Method
///--------------------------------
// Like all BDE proctor classes, each of the three 'bslmt::LockGuard*' classes
// provides a 'release' method that terminates the proctor's management of any
// lock object that the proctor holds.  The 'release' method has *no* *effect*
// on the state of the lock object.
//
// In particular, 'bslmt::ReadLockGuard::release' does not unlock the lock
// object under management.  If a user wants to release the lock object *and*
// unlock the lock object (because the lock is no longer required before the
// guard goes out of scope), the following idiom can be used:
//..
//  // 'guard' is an existing guard of type 'bslmt::LockGuard<my_Lock>',
//  // created in a scope that we do not control.
//
//  {
//      // ... Do work that requires the lock.
//
//      // We know that the lock is no longer needed.
//
//      my_Lock *lock = guard.release();
//
//      // 'lock' is no longer managed, but is *still* *locked*.
//
//      lock->unlock();
//
//      // ... Do work that does not require the lock.
//  }
//..
//
///Usage
///-----
// Use this component to ensure that in the event of an exception or exit from
// any point in a given scope, the synchronization object will be properly
// unlocked.  The following function, 'errorProneFunc', is overly complex, not
// exception safe, and contains a bug.
//..
//  static void errorProneFunc(my_Object *obj, my_Mutex *mutex)
//  {
//      mutex->lock();
//      if (someCondition) {
//          obj->someMethod();
//          mutex->unlock();
//          return;                                                   // RETURN
//      } else if (someOtherCondition) {
//          obj->someOtherMethod();
//          // MISTAKE! forgot to unlock mutex
//          return;                                                   // RETURN
//      }
//      obj->defaultMethod();
//      mutex->unlock();
//      return;
//  }
//..
// The function can be rewritten with a cleaner and safer implementation using
// a guard object.  The 'safeFunc' function is simpler than 'errorProneFunc',
// is exception safe, and avoids the multiple calls to unlock that can be a
// source of errors.
//..
//  static void safeFunc(my_Object *obj, my_Mutex *mutex)
//  {
//      bslmt::LockGuard<my_Mutex> guard(mutex);
//      if (someCondition) {
//          obj->someMethod();
//          return;                                                   // RETURN
//      } else if (someOtherCondition) {
//          obj->someOtherMethod();
//          // OK, mutex is automatically unlocked
//          return;                                                   // RETURN
//      }
//      obj->defaultMethod();
//      return;
//  }
//..
// When blocking while acquiring the lock is not desirable, one may instead use
// a 'bslmt::LockGuardTryLock' in the typical following fashion:
//..
//  static int safeButNonBlockingFunc(my_Object *obj, my_Mutex *mutex)
//      // Perform task and return positive value if locking succeeds.  Return
//      // 0 if locking fails.
//  {
//      const int RETRIES = 1; // use higher values for higher success rate
//      bslmt::LockGuardTryLock<my_Mutex> guard(mutex, RETRIES);
//      if (guard.ptr()) { // mutex is locked
//          if (someCondition) {
//              obj->someMethod();
//              return 2;                                             // RETURN
//          } else if (someOtherCondition) {
//              obj->someOtherMethod();
//              return 3;                                             // RETURN
//          }
//          obj->defaultMethod();
//          return 1;                                                 // RETURN
//      }
//      return 0;
//  }
//..
// Instantiations of 'bslmt::LockGuardUnlock' can be interleaved with
// instantiations of 'bslmt::LockGuard' to create both critical sections and
// regions where the lock is released.
//..
//  void f(my_Mutex *mutex)
//  {
//      bslmt::LockGuard<my_Mutex> guard(mutex);
//
//      // critical section here
//
//      {
//          bslmt::LockGuardUnlock<my_Mutex> guard(mutex);
//
//          // mutex is unlocked here
//
//      } // mutex is locked again here
//
//      // critical section here
//
//  } // mutex is unlocked here
//..
// Care must be taken so as not to interleave guard objects in such a way as to
// cause an illegal sequence of calls on a lock (two sequential lock calls or
// two sequential unlock calls on a non-recursive mutex).

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {
namespace bslmt {

                             // ===============
                             // class LockGuard
                             // ===============

template <class T>
class LockGuard {
    // This class template implements a proctor for acquisition and release of
    // synchronization resources (i.e., locks).

    // DATA
    T *d_lock_p;  // lock proctored by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    LockGuard(const LockGuard<T>&);
    LockGuard<T>& operator=(const LockGuard<T>&);

  public:
    // CREATORS
    explicit LockGuard(T *lock);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero), and invokes the 'lock' method on 'lock'.  Note
        // that 'lock' must remain valid throughout the lifetime of this
        // proctor, or until 'release' is called.

    LockGuard(T *lock, int preLockedFlag);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero) and, unless the specified 'preLockedFlag' is
        // non-zero, invokes the 'lock' method on 'lock'.  Note that 'lock'
        // must remain valid throughout the lifetime of this proctor, or until
        // 'release' is called.

    ~LockGuard();
        // Destroy this proctor object and invoke the 'unlock' method on the
        // lock object under management by this proctor, if any.  If no lock is
        // currently being managed, this method has no effect.

    // MANIPULATORS
    T *release();
        // Return the address of the modifiable lock object under management by
        // this proctor, and release the lock from further management by this
        // proctor.  If no lock is currently being managed, return 0 with no
        // other effect.  Note that this operation does *not* unlock the lock
        // object (if any) that was under management.

    // ACCESSORS
    T *ptr() const;
        // Return the address of the modifiable lock object under management by
        // this proctor, or 0 if no lock is currently being managed.
};

                          // =====================
                          // class LockGuardUnlock
                          // =====================

template <class T>
class LockGuardUnlock {
    // This class template implements a proctor for release and reacquisition
    // of synchronization resources (i.e., locks).

    // DATA
    T *d_lock_p;  // lock proctored by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    LockGuardUnlock(const LockGuardUnlock<T>&);
    LockGuardUnlock<T>& operator=(const LockGuardUnlock<T>&);

  public:
    // CREATORS
    explicit LockGuardUnlock(T *lock);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero), and invokes the 'unlock' method on 'lock'.
        // Note that 'lock' must remain valid throughout the lifetime of this
        // proctor, or until 'release' is called.

    LockGuardUnlock(T *lock, int preUnlockedFlag);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero) and, unless the specified 'preUnlockedFlag' is
        // non-zero, invokes the 'unlock' method on 'lock'.  Note that 'lock'
        // must remain valid throughout the lifetime of this proctor, or until
        // 'release' is called.

    ~LockGuardUnlock();
        // Destroy this proctor object and invoke the 'lock' method on the lock
        // object under management by this proctor, if any.  If no lock is
        // currently being managed, this method has no effect.

    // MANIPULATORS
    T *release();
        // Return the address of the modifiable lock object under management by
        // this proctor, and release the lock from further management by this
        // proctor.  If no lock is currently being managed, return 0 with no
        // other effect.  Note that this operation does *not* lock the lock
        // object (if any) that was under management.

    // ACCESSORS
    T *ptr() const;
        // Return the address of the modifiable lock object under management by
        // this proctor, or 0 if no lock is currently being managed.
};

                            // =================
                            // class UnLockGuard
                            // =================

template <class T>
class UnLockGuard : public LockGuardUnlock<T> {
    // This class is DEPRECATED.  Use 'LockGuardUnlock' instead.

  private:
    // NOT IMPLEMENTED
    UnLockGuard(const UnLockGuard<T>&);
    UnLockGuard<T>& operator=(const UnLockGuard<T>&);

  public:
    // CREATORS
    explicit UnLockGuard(T *lock);
        // DEPRECATED: Use 'LockGuardUnlock' instead.

    UnLockGuard(T *lock, int preUnlockedFlag);
        // DEPRECATED: Use 'LockGuardUnlock' instead.
};

                          // ======================
                          // class LockGuardTryLock
                          // ======================

template <class T>
class LockGuardTryLock {
    // This class template implements a proctor for tentative acquisition and
    // release of synchronization resources (i.e., locks).

    // DATA
    T *d_lock_p;  // lock proctored by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    LockGuardTryLock(const LockGuardTryLock<T>&);
    LockGuardTryLock<T>& operator=(const LockGuardTryLock<T>&);

  public:
    // CREATORS
    explicit LockGuardTryLock(T *lock, int attempts = 1);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero), and invokes the 'tryLock' method on 'lock'
        // until the lock is acquired, or until up to the optionally specified
        // 'attempts' have been made to acquire the lock.  The behavior is
        // undefined unless '0 < attempts'.  Note that 'lock' must remain valid
        // throughout the lifetime of this proctor, or until 'release' is
        // called.

    ~LockGuardTryLock();
        // Destroy this proctor object and invoke the 'unlock' method on the
        // lock object under management by this proctor, if any.  If no lock is
        // currently being managed, this method has no effect.

    // MANIPULATORS
    T *release();
        // Return the address of the modifiable lock object under management by
        // this proctor, and release the lock from further management by this
        // proctor.  If no lock is currently being managed, return 0 with no
        // other effect.  Note that this operation does *not* unlock the lock
        // object (if any) that was under management.

    // ACCESSORS
    T *ptr() const;
        // Return the address of the modifiable lock object under management by
        // this proctor, or 0 if no lock is currently being managed.
};

                            // ==================
                            // class TryLockGuard
                            // ==================

template <class T>
class TryLockGuard : public LockGuardTryLock<T> {
    // This class is DEPRECATED.  Use 'LockGuardTryLock' instead.

  private:
    // NOT IMPLEMENTED
    TryLockGuard(const TryLockGuard<T>&);
    TryLockGuard<T>& operator=(const TryLockGuard<T>&);

  public:
    // CREATORS
    explicit TryLockGuard(T *lock, int attempts = 1);
        // DEPRECATED: Use 'LockGuardTryLock' instead.
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                             // ---------------
                             // class LockGuard
                             // ---------------

// CREATORS
template <class T>
inline
bslmt::LockGuard<T>::LockGuard(T *lock)
: d_lock_p(lock)
{
    if (d_lock_p) {
        d_lock_p->lock();
    }
}

template <class T>
inline
bslmt::LockGuard<T>::LockGuard(T *lock, int preLockedFlag)
: d_lock_p(lock)
{
    if (d_lock_p && !preLockedFlag) {
        d_lock_p->lock();
    }
}

template <class T>
inline
bslmt::LockGuard<T>::~LockGuard()
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

// MANIPULATORS
template <class T>
inline
T *bslmt::LockGuard<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bslmt::LockGuard<T>::ptr() const
{
    return d_lock_p;
}

                            // -----------------
                            // class UnLockGuard
                            // -----------------

// CREATORS
template <class T>
inline
bslmt::UnLockGuard<T>::UnLockGuard(T *lock)
: LockGuardUnlock<T>(lock)
{
}

template <class T>
inline
bslmt::UnLockGuard<T>::UnLockGuard(T *lock, int preUnlockedFlag)
: LockGuardUnlock<T>(lock, preUnlockedFlag)
{
}

                            // ------------------
                            // class TryLockGuard
                            // ------------------

template <class T>
inline
bslmt::TryLockGuard<T>::TryLockGuard(T *lock, int attempts)
: LockGuardTryLock<T>(lock, attempts)
{
}

                          // ---------------------
                          // class LockGuardUnlock
                          // ---------------------

// CREATORS
template <class T>
inline
bslmt::LockGuardUnlock<T>::LockGuardUnlock(T *lock)
: d_lock_p(lock)
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

template <class T>
inline
bslmt::LockGuardUnlock<T>::LockGuardUnlock(T *lock, int preUnlockedFlag)
: d_lock_p(lock)
{
    if (d_lock_p && !preUnlockedFlag) {
        d_lock_p->unlock();
    }
}

template <class T>
inline
bslmt::LockGuardUnlock<T>::~LockGuardUnlock()
{
    if (d_lock_p) {
        d_lock_p->lock();
    }
}

// MANIPULATORS
template <class T>
inline
T *bslmt::LockGuardUnlock<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bslmt::LockGuardUnlock<T>::ptr() const
{
    return d_lock_p;
}

                          // ----------------------
                          // class LockGuardTryLock
                          // ----------------------

// CREATORS
template <class T>
bslmt::LockGuardTryLock<T>::LockGuardTryLock(T *lock, int attempts)
: d_lock_p(0)
{
    if (lock) {
        while (attempts--) {
            if (!lock->tryLock()) {
                d_lock_p = lock;
                break;
            }
        }
    }
}

template <class T>
inline
bslmt::LockGuardTryLock<T>::~LockGuardTryLock()
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

// MANIPULATORS
template <class T>
inline
T *bslmt::LockGuardTryLock<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bslmt::LockGuardTryLock<T>::ptr() const
{
    return d_lock_p;
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
