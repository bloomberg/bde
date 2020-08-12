// bslmt_writelockguard.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_WRITELOCKGUARD
#define INCLUDED_BSLMT_WRITELOCKGUARD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide generic scoped guards for write synchronization objects.
//
//@CLASSES:
//  bslmt::WriteLockGuard: automatic locking-unlocking for write access
//  bslmt::WriteLockGuardUnlock: automatic unlocking-locking for write access
//  bslmt::WriteLockGuardTryLock: automatic non-blocking locking-unlocking
//  bslmt::LockWriteGuard: DEPRECATED
//
//@SEE_ALSO: bslmt_lockguard, bslmt_readlockguard
//
//@DESCRIPTION: This component provides generic guards,
// 'bslmt::WriteLockGuard', 'bslmt::WriteLockGuardUnlock', and
// 'bslmt::WriteLockGuardTryLock', to automatically lock and unlock an external
// synchronization object for writing.  The synchronization object can be any
// type (e.g., 'bslmt::ReaderWriterLock') that provides the following methods:
//..
//  void lockWrite();
//  void unlock();
//..
// Both 'bslmt::WriteLockGuard' and 'bslmt::WriteLockGuardUnlock' implement the
// "construction is acquisition, destruction is release" idiom.  During
// construction, 'bslmt::WriteLockGuard' automatically calls 'lockWrite' on the
// user-supplied object, and 'unlock' when it is destroyed (unless released).
// 'bslmt::WriteLockGuardUnlock' does the opposite -- it invokes the 'unlock'
// method when constructed and the 'lockWrite' method when destroyed.
//
// A third type of guard, 'bslmt::WriteLockGuardTryLock', attempts to acquire a
// lock, and if acquisition succeeds, releases it upon destruction.  Since the
// acquisition is done at construction time, it is not possible to return a
// value to indicate success.  Rather, the 'bslmt::WriteLockGuardTryLock'
// contains a pointer to the synchronization object if 'tryLock' succeeds, and
// is null otherwise.  The synchronization object can be any type (e.g.,
// 'bslmt::Mutex' or 'bslmt::RecursiveMutex') that provides the following
// methods:
//..
//  int tryLockWrite();
//  void unlock();
//..
// Note that objects of neither guard type assumes ownership of the
// synchronization object passed at construction.  Also note that objects of
// all of the guard types may be constructed with a null 'lock' whereby the
// constructed guard objects guard no lock.  The destructor of each of the
// guard types has no effect if no lock is under management.
//
///Behavior of the 'release' Method
///--------------------------------
// Like all BDE guard classes, each of the three 'bslmt::WriteLockGuard*'
// classes provides a 'release' method that terminates the guard's management
// of any lock object that the guard holds.  The 'release' method has *no*
// *effect* on the state of the lock object.
//
// In particular, 'bslmt::WriteLockGuard::release' does not unlock the lock
// object under management.  If a user wants to release the lock object *and*
// unlock the lock object (because the lock is no longer required before the
// guard goes out of scope), the following idiom can be used:
//..
//  // 'guard' is an existing guard of type 'bslmt::WriteLockGuard<my_RLock>',
//  // created in a scope that we do not control.
//
//  {
//      // ... Do work that requires the lock.
//
//      // We know that the lock is no longer needed.
//
//      my_RLock *rlock = guard.release();
//
//      // 'rlock' is no longer managed, but is *still* *locked*.
//
//      rlock->unlock();
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
//  static void errorProneFunc(my_Object *obj, my_RWLock *rwlock)
//  {
//      rwlock->lockWrite();
//      if (someUpgradeCondition) {
//          obj->someUpgradeMethod();
//          rwlock->unlock();
//          return;                                                   // RETURN
//      } else if (someOtherUpgradeCondition) {
//          obj->someOtherUpgradeMethod();
//          // MISTAKE! forgot to unlock rwlock
//          return;                                                   // RETURN
//      }
//      obj->defaultUpgradeMethod();
//      rwlock->unlock();
//      return;
//  }
//..
// The function can be rewritten with a cleaner and safer implementation using
// a guard object.  The 'safeFunc' function is simpler than 'errorProneFunc',
// is exception safe, and avoids the multiple calls to unlock that can be a
// source of errors.
//..
//  static void safeFunc(my_Object *obj, my_RWLock *rwlock)
//  {
//      bslmt::WriteLockGuard<my_RWLock> guard(rwlock);
//      if (someUpgradeCondition) {
//          obj->someUpgradeMethod();
//          return;                                                   // RETURN
//      } else if (someOtherUpgradeCondition) {
//          obj->someOtherUpgradeMethod();
//          // OK, rwlock is automatically unlocked
//          return;                                                   // RETURN
//      }
//      obj->defaultUpgradeMethod();
//      return;
//  }
//..
// When blocking while acquiring the lock is not desirable, one may instead use
// a 'bslmt::WriteLockGuardTryLock' in the typical following fashion:
//..
//  static int safeButNonBlockingFunc(my_Object *obj, my_RWLock *rwlock)
//      // Perform upgrade and return positive value if locking succeeds.
//      // Return 0 if locking fails.
//  {
//      const int RETRIES = 1; // use higher values for higher success rate
//      bslmt::WriteLockGuardTryLock<my_RWLock> guard(rwlock, RETRIES);
//      if (guard.ptr()) { // rwlock is locked
//          if (someUpgradeCondition) {
//              obj->someUpgradeMethod();
//              return 2;                                             // RETURN
//          } else if (someOtherUpgradeCondition) {
//              obj->someOtherUpgradeMethod();
//              return 3;                                             // RETURN
//          }
//          obj->defaultUpgradeMethod();
//          return 1;                                                 // RETURN
//      }
//      return 0;
//  }
//..
// If the underlying lock object provides an upgrade from a lock for read to a
// lock for write (as does 'bslmt::ReaderWriterLock' with the
// 'upgradeToWriteLock' function, for example), and the lock is already guarded
// by a 'bslmt::ReadLockGuard', then it is not necessary to transfer the guard
// to a 'bslmt::WriteLockGuard'.  In fact, a combination of
// 'bslmt::ReadLockGuard' and 'bslmt::WriteLockGuard' guarding a common lock
// object should probably never be needed.
//
// Care must be taken so as not to interleave guard objects in such a way as to
// cause an illegal sequence of calls on a lock (two sequential lock calls or
// two sequential unlock calls on a non-recursive read/write lock).

#include <bslscm_version.h>

namespace BloombergLP {
namespace bslmt {

                           // ====================
                           // class WriteLockGuard
                           // ====================

template <class T>
class WriteLockGuard {
    // This class template implements a guard for acquisition and release of
    // write synchronization resources (i.e., writer locks).

    // DATA
    T *d_lock_p;  // lock guarded by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    WriteLockGuard(const WriteLockGuard<T>&);
    WriteLockGuard<T>& operator=(const WriteLockGuard<T>&);

  public:
    // CREATORS
    explicit WriteLockGuard(T *lock);
        // Create a scoped guard that conditionally manages the specified
        // 'lock' (if non-null) and invokes 'lock->lockWrite()'.  Supplying a
        // null 'lock' has no effect.  The behavior is undefined unless 'lock'
        // (if non-null) is not already locked by this thread.  Note that
        // 'lock' must remain valid throughout the lifetime of this guard, or
        // until 'release' is called.

    WriteLockGuard(T *lock, bool alreadyLockedFlag);
        // Create a scoped guard that conditionally manages the specified
        // 'lock' (if non-null) and invokes 'lock->lockWrite()' if the
        // specified 'alreadyLockedFlag' is 'false'.  Supplying a null 'lock'
        // has no effect.  The behavior is undefined unless the state of 'lock'
        // (if non-null) is consistent with 'alreadyLockedFlag'.  Note that
        // 'alreadyLockedFlag' is used to indicate whether 'lock' is in an
        // already-locked state when passed, so if 'alreadyLockedFlag' is
        // 'true' the 'lock' method will *not* be called on the supplied
        // 'lock'.  Also note that 'lock' must remain valid throughout the
        // lifetime of this guard, or until 'release' is called.

    ~WriteLockGuard();
        // Destroy this scoped guard and invoke the 'unlock' method on the
        // lock object under management by this guard, if any.  If no lock is
        // currently being managed, this method has no effect.

    // MANIPULATORS
    T *release();
        // Return the address of the modifiable lock object under management by
        // this guard, and release the lock from further management by this
        // guard.  If no lock is currently being managed, return 0 with no
        // other effect.  Note that this operation does *not* unlock the lock
        // object (if any) that was under management.

    // ACCESSORS
    T *ptr() const;
        // Return the address of the modifiable lock object under management by
        // this guard, or 0 if no lock is currently being managed.
};

                           // ====================
                           // class LockWriteGuard
                           // ====================

template <class T>
class LockWriteGuard : public WriteLockGuard<T> {
    // This class is DEPRECATED.  Use 'WriteLockGuard' instead.

  private:
    // NOT IMPLEMENTED
    LockWriteGuard(const LockWriteGuard<T>&);
    LockWriteGuard<T>& operator=(const LockWriteGuard<T>&);

  public:
    // CREATORS
    explicit LockWriteGuard(T *lock);
        // DEPRECATED: Use 'WriteLockGuard' instead.

    LockWriteGuard(T *lock, bool alreadyLockedFlag);
        // DEPRECATED: Use 'WriteLockGuard' instead.

};

                        // ==========================
                        // class WriteLockGuardUnlock
                        // ==========================

template <class T>
class WriteLockGuardUnlock {
    // This class template implements a guard for release and reacquisition
    // of write synchronization resources (i.e., writer locks).

    // DATA
    T *d_lock_p;  // lock guarded by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    WriteLockGuardUnlock(const WriteLockGuardUnlock<T>&);
    WriteLockGuardUnlock<T>& operator=(const WriteLockGuardUnlock<T>&);

  public:
    // CREATORS
    explicit WriteLockGuardUnlock(T *lock);
        // Create a scoped guard that conditionally manages the specified
        // 'lock' (if non-null) and invokes 'lock->unlock()'.  Supplying a null
        // 'lock' has no effect.  The behavior is undefined unless 'lock' (if
        // non-null) is locked by this thread.  Note that 'lock' must remain
        // valid throughout the lifetime of this guard, or until 'release' is
        // called.

    WriteLockGuardUnlock(T *lock, bool alreadyUnlockedFlag);
        // Create a scoped guard that conditionally manages the specified
        // 'lock' (if non-null) and invokes 'lock->unlock()' if the specified
        // 'alreadyUnlockedFlag' is 'false'.  Supplying a null 'lock' has no
        // effect.  The behavior is undefined unless the state of 'lock' (if
        // non-null) is consistent with 'alreadyUnlockedFlag'.  Note that
        // 'alreadyUnlockedFlag' is used to indicate whether 'lock' is in an
        // already-unlocked state when passed, so if 'alreadyUnlockedFlag' is
        // 'true' the 'unlock' method will *not* be called on the supplied
        // 'lock'.  Also note that 'lock' must remain valid throughout the
        // lifetime of this guard, or until 'release' is called.

    ~WriteLockGuardUnlock();
        // Destroy this scoped guard and invoke the 'lockWrite' method on the
        // lock object under management by this guard, if any.  If no lock is
        // currently being managed, this method has no effect.

    // MANIPULATORS
    T *release();
        // Return the address of the modifiable lock object under management by
        // this guard, and release the lock from further management by this
        // guard.  If no lock is currently being managed, return 0 with no
        // other effect.  Note that this operation does *not* lock the lock
        // object (if any) that was under management.

    // ACCESSORS
    T *ptr() const;
        // Return the address of the modifiable lock object under management by
        // this guard, or 0 if no lock is currently being managed.
};

                       // ===========================
                       // class WriteLockGuardTryLock
                       // ===========================

template <class T>
class WriteLockGuardTryLock {
    // This class template implements a guard for tentative acquisition and
    // release of write synchronization resources (i.e., writer locks).

    // DATA
    T *d_lock_p;  // lock guarded by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    WriteLockGuardTryLock(const WriteLockGuardTryLock<T>&);
    WriteLockGuardTryLock<T>& operator=(const WriteLockGuardTryLock<T>&);

  public:
    // CREATORS
    explicit WriteLockGuardTryLock(T *lock, int attempts = 1);
        // Create a scoped guard that conditionally manages the specified
        // 'lock' (if non-null) and invokes 'lock->tryLockWrite()' until the
        // lock is acquired for writing, or until the optionally specified
        // 'attempts' have been made to acquire the lock.  If 'attempts' is not
        // specified only one attempt is made to acquire the lock.  Supplying a
        // null 'lock' has no effect.  The behavior is undefined unless 'lock'
        // (if non-null) is not already locked by this thread and
        // '0 < attempts'.  Note that 'lock' must remain valid throughout the
        // lifetime of this guard, or until 'release' is called.

    ~WriteLockGuardTryLock();
        // Destroy this scoped guard and invoke the 'unlock' method on the
        // lock object under management by this guard, if any.  If no lock is
        // currently being managed, this method has no effect.

    // MANIPULATORS
    T *release();
        // Return the address of the modifiable lock object under management by
        // this guard, and release the lock from further management by this
        // guard.  If no lock is currently being managed, return 0 with no
        // other effect.  Note that this operation does *not* unlock the lock
        // object (if any) that was under management.

    // ACCESSORS
    T *ptr() const;
        // Return the address of the modifiable lock object under management by
        // this guard, or 0 if no lock is currently being managed.
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                           // --------------------
                           // class WriteLockGuard
                           // --------------------

// CREATORS
template <class T>
inline
bslmt::WriteLockGuard<T>::WriteLockGuard(T *lock)
: d_lock_p(lock)
{
    if (d_lock_p) {
        d_lock_p->lockWrite();
    }
}

template <class T>
inline
bslmt::WriteLockGuard<T>::WriteLockGuard(T *lock, bool alreadyLockedFlag)
: d_lock_p(lock)
{
    if (d_lock_p && !alreadyLockedFlag) {
        d_lock_p->lockWrite();
    }
}

template <class T>
inline
bslmt::WriteLockGuard<T>::~WriteLockGuard()
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

// MANIPULATORS

template <class T>
inline
T *bslmt::WriteLockGuard<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bslmt::WriteLockGuard<T>::ptr() const
{
    return d_lock_p;
}

                        // --------------------------
                        // class WriteLockGuardUnlock
                        // --------------------------

// CREATORS
template <class T>
inline
bslmt::WriteLockGuardUnlock<T>::WriteLockGuardUnlock(T *lock)
: d_lock_p(lock)
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

template <class T>
inline
bslmt::WriteLockGuardUnlock<T>::WriteLockGuardUnlock(T    *lock,
                                                     bool  alreadyUnlockedFlag)
: d_lock_p(lock)
{
    if (d_lock_p && !alreadyUnlockedFlag) {
        d_lock_p->unlock();
    }
}

template <class T>
inline
bslmt::WriteLockGuardUnlock<T>::~WriteLockGuardUnlock()
{
    if (d_lock_p) {
        d_lock_p->lockWrite();
    }
}

// MANIPULATORS
template <class T>
inline
T *bslmt::WriteLockGuardUnlock<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bslmt::WriteLockGuardUnlock<T>::ptr() const
{
    return d_lock_p;
}

                       // ---------------------------
                       // class WriteLockGuardTryLock
                       // ---------------------------

// CREATORS
template <class T>
bslmt::WriteLockGuardTryLock<T>::WriteLockGuardTryLock(T *lock, int attempts)
: d_lock_p(0)
{
    if (lock) {
        while (attempts--) {
            if (!lock->tryLockWrite()) {
                d_lock_p = lock;
                break;
            }
        }
    }
}

template <class T>
inline
bslmt::WriteLockGuardTryLock<T>::~WriteLockGuardTryLock()
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

// MANIPULATORS
template <class T>
inline
T *bslmt::WriteLockGuardTryLock<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bslmt::WriteLockGuardTryLock<T>::ptr() const
{
    return d_lock_p;
}

                           // --------------------
                           // class LockWriteGuard
                           // --------------------

// CREATORS
template <class T>
inline
bslmt::LockWriteGuard<T>::LockWriteGuard(T *lock)
: WriteLockGuard<T>(lock)
{
}

template <class T>
inline
bslmt::LockWriteGuard<T>::LockWriteGuard(T *lock, bool alreadyLockedFlag)
: WriteLockGuard<T>(lock, alreadyLockedFlag)
{
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
