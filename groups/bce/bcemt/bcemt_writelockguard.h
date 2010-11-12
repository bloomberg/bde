// bcemt_writelockguard.h                                             -*-C++-*-
#ifndef INCLUDED_BCEMT_WRITELOCKGUARD
#define INCLUDED_BCEMT_WRITELOCKGUARD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a generic proctor for write synchronization objects.
//
//@CLASSES:
//  bcemt_WriteLockGuard: automatic locking-unlocking for write access
//  bcemt_WriteLockGuardUnlock: automatic unlocking-locking for write access
//  bcemt_WriteLockGuardTryLock: automatic non-blocking locking-unlocking for
//                               write access
// bcemt_LockWriteGuard: DEPRECATED
//
//@SEE_ALSO: bcemt_lockguard, bcemt_readlockguard, bcemt_rwmutex
//
//@AUTHOR: Herve Bronnimann (hbronnimann)
//
//@DESCRIPTION: This component provides generic proctors to automatically lock
// and unlock an external synchronization object for writing.  The
// synchronization object can be any type (e.g., 'bcemt_ReaderWriterLock') that
// provides the following methods:
//..
//  void lockWrite();
//  void unlock();
//..
// Both 'bcemt_WriteLockGuard' and 'bcemt_WriteLockGuardUnlock' implement the
// "construction is acquisition, destruction is release" idiom.  During
// construction, 'bcemt_WriteLockGuard' automatically calls 'lockWrite' on the
// user-supplied object, and 'unlock' when it is destroyed (unless released).
// 'bcemt_WriteLockGuardUnlock' does the opposite -- it invokes the 'unlock'
// method when constructed and the 'lockWrite' method when destroyed.
//
// A third type of guard, 'bcemt_WriteLockGuardTryLock', attempts to acquire a
// lock, and if acquisition succeeds, releases it upon destruction.  Since the
// acquisition is done at construction time, it is not possible to return a
// value to indicate success.  Rather, the 'bcemt_WriteLockGuardTryLock'
// contains a pointer to the synchronization object if 'tryLock' succeeds, and
// is null otherwise.  The synchronization object can be any type (e.g.,
// 'bcemt_Mutex' or 'bcemt_RecursiveMutex') that provides the following
// methods:
//..
//  int tryLockWrite();
//  void unlock();
//..
// Note that objects of neither guard type assumes ownership of the
// synchronization object passed at construction.  Also note that objects of
// all of the guard types may be constructed with a null 'lock' whereby the
// constructed guard objects proctor no lock.  The destructor of each of the
// guard types has no effect if no lock is under management.
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
//          return;
//      } else if (someOtherUpgradeCondition) {
//          obj->someOtherUpgradeMethod();
//          return;                      // MISTAKE! forgot to unlock rwlock
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
//      bcemt_WriteLockGuard<my_RWLock> guard(rwlock);
//      if (someCondition) {
//          obj->someUpgradeMethod();
//          return;
//      } else if (someOtherUpgradeCondition) {
//          obj->someOtherUpgradeMethod();
//          return;                  // OK, rwlock is automatically unlocked
//      }
//      obj->defaultUpgradeMethod();
//      return;
//  }
//..
// When blocking while acquiring the lock is not desirable, one may instead use
// a 'bcemt_WriteLockGuardTryLock' in the typical following fashion:
//..
//  static int safeButNonBlockingFunc(my_Object *obj, my_RWLock *rwlock)
//      // Perform upgrade and return positive value if locking succeeds.
//      // Return 0 if locking fails.
//  {
//      const int RETRIES = 1; // use higher values for higher success rate
//      bcemt_WriteLockGuardTryLock<my_RWLock> guard(rwlock, RETRIES);
//      if (guard.ptr()) { // rwlock is locked
//          if (someUpgradeCondition) {
//              obj->someUpgradeMethod();
//              return 2;
//          } else if (someOtherUpgradeCondition) {
//              obj->someOtherUpgradeMethod();
//              return 3;
//          }
//          obj->defaultUpgradeMethod();
//          return 1;
//      }
//      return 0;
//  }
//..
// If the underlying lock object provides an upgrade from a lock for read to a
// lock for write (as does 'bcemt_ReaderWriterLock' with the
// 'upgradeToWriteLock' function, for example), and the lock is already guarded
// by a 'bcemt_LockReadGuard', then it is not necessary to transfer the guard
// to a 'bcemt_WriteLockGuard'.  In fact, a combination of
// 'bcemt_LockReadGuard' and 'bcemt_WriteLockGuard' guarding a common lock
// object should probably never be needed.
//
// Care must be taken so as not to interleave guard objects in such a way as to
// cause an illegal sequence of calls on a lock (two sequential lock calls or
// two sequential unlock calls on a non-recursive read/write lock).

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

namespace BloombergLP {

                           // ==========================
                           // class bcemt_WriteLockGuard
                           // ==========================

template <class T>
class bcemt_WriteLockGuard {
    // This class template implements a proctor for acquisition and release of
    // write synchronization resources (i.e., writer locks).

    // DATA
    T *d_lock_p;  // lock proctored by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    bcemt_WriteLockGuard(const bcemt_WriteLockGuard<T>&);
    bcemt_WriteLockGuard<T>& operator=(const bcemt_WriteLockGuard<T>&);

  public:
    // CREATORS
    explicit bcemt_WriteLockGuard(T *lock);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero), and invokes the 'lockWrite' method on 'lock'.
        // Note that 'lock' must remain valid throughout the lifetime of this
        // proctor, or until 'release' is called.

    bcemt_WriteLockGuard(T *lock, int preLockedFlag);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero) and, unless the specified 'preLockedFlag' is
        // non-zero, invokes the 'lockWrite' method on 'lock'.  Note that
        // 'lock' must remain valid throughout the lifetime of this proctor, or
        // until 'release' is called.

    ~bcemt_WriteLockGuard();
        // Destroy this proctor object and invoke the 'unlock' method on the
        // lock object under management by this proctor, if any.  If no lock is
        // currently being managed, this method has no effect.

    // MANIPULATORS
    T *release();
        // Return the address of the modifiable lock object under management by
        // this proctor, and release the lock from further management by this
        // proctor.  If no lock is currently being managed, return 0 with no
        // other effect.

    // ACCESSORS
    T *ptr() const;
        // Return the address of the modifiable lock object under management by
        // this proctor, or 0 if no lock is currently being managed.
};

                           // ==========================
                           // class bcemt_LockWriteGuard
                           // ==========================

template <class T>
class bcemt_LockWriteGuard : public bcemt_WriteLockGuard<T> {
    // This class is DEPRECATED.  Use 'bcemt_WriteLockGuard' instead.

  private:
    // NOT IMPLEMENTED
    bcemt_LockWriteGuard(const bcemt_LockWriteGuard<T>&);
    bcemt_LockWriteGuard<T>& operator=(const bcemt_LockWriteGuard<T>&);

  public:
    // CREATORS
    explicit bcemt_LockWriteGuard(T *lock);
        // DEPRECATED: Use 'bcemt_WriteLockGuard' instead.

    bcemt_LockWriteGuard(T *lock, int preLockedFlag);
        // DEPRECATED: Use 'bcemt_WriteLockGuard' instead.

};

                      // ================================
                      // class bcemt_WriteLockGuardUnlock
                      // ================================

template <class T>
class bcemt_WriteLockGuardUnlock {
    // This class template implements a proctor for release and reacquisition
    // of write synchronization resources (i.e., writer locks).

    // DATA
    T *d_lock_p;  // lock proctored by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    bcemt_WriteLockGuardUnlock(const bcemt_WriteLockGuardUnlock<T>&);
    bcemt_WriteLockGuardUnlock<T>& operator=(
                               const bcemt_WriteLockGuardUnlock<T>&);

  public:
    // CREATORS
    explicit bcemt_WriteLockGuardUnlock(T *lock);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero), and invokes the 'unlock' method on 'lock'.
        // Note that 'lock' must remain valid throughout the lifetime of this
        // proctor, or until 'release' is called.

    bcemt_WriteLockGuardUnlock(T *lock, int preUnlockedFlag);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero) and, unless the specified 'preUnlockedFlag' is
        // non-zero, invokes the 'unlock' method on 'lock'.  Note that 'lock'
        // must remain valid throughout the lifetime of this proctor, or until
        // 'release' is called.

    ~bcemt_WriteLockGuardUnlock();
        // Destroy this proctor object and invoke the 'lockWrite' method on the
        // lock object under management by this proctor, if any.  If no lock is
        // currently being managed, this method has no effect.

    // MANIPULATORS
    T *release();
        // Return the address of the modifiable lock object under management by
        // this proctor, and release the lock from further management by this
        // proctor.  If no lock is currently being managed, return 0 with no
        // other effect.

    // ACCESSORS
    T *ptr() const;
        // Return the address of the modifiable lock object under management by
        // this proctor, or 0 if no lock is currently being managed.
};

                      // =================================
                      // class bcemt_WriteLockGuardTryLock
                      // =================================

template <class T>
class bcemt_WriteLockGuardTryLock {
    // This class template implements a proctor for tentative acquisition and
    // release of write synchronization resources (i.e., writer locks).

    // DATA
    T *d_lock_p;  // lock proctored by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    bcemt_WriteLockGuardTryLock(const bcemt_WriteLockGuardTryLock<T>&);
    bcemt_WriteLockGuardTryLock<T>& operator=(
                                const bcemt_WriteLockGuardTryLock<T>&);

  public:
    // CREATORS
    explicit bcemt_WriteLockGuardTryLock(T *lock, int attempts = 1);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero), and invokes the 'tryLockWrite' method on
        // 'lock' until the lock is acquired, or until up to the specified
        // 'attempts' have been made to acquire the lock.  The behavior is
        // undefined unless '0 < attempts'.  Note that 'lock' must remain valid
        // throughout the lifetime of this proctor, or until 'release' is
        // called.

    ~bcemt_WriteLockGuardTryLock();
        // Destroy this proctor object and invoke the 'unlock' method on the
        // lock object under management by this proctor, if any.  If no lock is
        // currently being managed, this method has no effect.

    // MANIPULATORS
    T *release();
        // Return the address of the modifiable lock object under management by
        // this proctor, and release the lock from further management by this
        // proctor.  If no lock is currently being managed, return 0 with no
        // other effect.

    // ACCESSORS
    T *ptr() const;
        // Return the address of the modifiable lock object under management by
        // this proctor, or 0 if no lock is currently being managed.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // --------------------------
                          // class bcemt_WriteLockGuard
                          // --------------------------

// CREATORS
template <class T>
inline
bcemt_WriteLockGuard<T>::bcemt_WriteLockGuard(T *lock)
: d_lock_p(lock)
{
    if (d_lock_p) {
        d_lock_p->lockWrite();
    }
}

template <class T>
inline
bcemt_WriteLockGuard<T>::bcemt_WriteLockGuard(T *lock, int preLocked)
: d_lock_p(lock)
{
    if (d_lock_p && !preLocked) {
        d_lock_p->lockWrite();
    }
}

template <class T>
inline
bcemt_WriteLockGuard<T>::~bcemt_WriteLockGuard()
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

// MANIPULATORS

template <class T>
inline
T *bcemt_WriteLockGuard<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bcemt_WriteLockGuard<T>::ptr() const
{
    return d_lock_p;
}

                      // --------------------------------
                      // class bcemt_WriteLockGuardUnlock
                      // --------------------------------

// CREATORS
template <class T>
inline
bcemt_WriteLockGuardUnlock<T>::bcemt_WriteLockGuardUnlock(T *lock)
: d_lock_p(lock)
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

template <class T>
inline
bcemt_WriteLockGuardUnlock<T>::bcemt_WriteLockGuardUnlock(T   *lock,
                                                          int  preUnlocked)
: d_lock_p(lock)
{
    if (d_lock_p && !preUnlocked) {
        d_lock_p->unlock();
    }
}

template <class T>
inline
bcemt_WriteLockGuardUnlock<T>::~bcemt_WriteLockGuardUnlock()
{
    if (d_lock_p) {
        d_lock_p->lockWrite();
    }
}

// MANIPULATORS
template <class T>
inline
T *bcemt_WriteLockGuardUnlock<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bcemt_WriteLockGuardUnlock<T>::ptr() const
{
    return d_lock_p;
}

                      // ---------------------------------
                      // class bcemt_WriteLockGuardTryLock
                      // ---------------------------------

// CREATORS
template <class T>
bcemt_WriteLockGuardTryLock<T>::bcemt_WriteLockGuardTryLock(T   *lock,
                                                            int  attempts)
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
bcemt_WriteLockGuardTryLock<T>::~bcemt_WriteLockGuardTryLock()
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

// MANIPULATORS
template <class T>
inline
T *bcemt_WriteLockGuardTryLock<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bcemt_WriteLockGuardTryLock<T>::ptr() const
{
    return d_lock_p;
}

                         // --------------------------
                         // class bcemt_LockWriteGuard
                         // --------------------------

// CREATORS
template <class T>
inline
bcemt_LockWriteGuard<T>::bcemt_LockWriteGuard(T *lock)
: bcemt_WriteLockGuard<T>(lock)
{
}

template <class T>
inline
bcemt_LockWriteGuard<T>::bcemt_LockWriteGuard(T *lock, int preLocked)
: bcemt_WriteLockGuard<T>(lock, preLocked)
{
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
