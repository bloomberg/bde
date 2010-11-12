// bcemt_readlockguard.h                                              -*-C++-*-
#ifndef INCLUDED_BCEMT_READLOCKGUARD
#define INCLUDED_BCEMT_READLOCKGUARD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a generic proctor for read synchronization objects.
//
//@CLASSES:
//  bcemt_ReadLockGuard: automatic locking-unlocking for read access
//  bcemt_ReadLockGuardUnlock: automatic unlocking-locking for read access
//  bcemt_ReadLockGuardTryLock: automatic non-blocking locking-unlocking for
//                              read access
//  bcemt_LockReadGuard: DEPRECATED
//
//@SEE_ALSO: bcemt_lockguard, bcemt_writelockguard, bcemt_rwmutex
//
//@AUTHOR: Herve Bronnimann (hbronnimann)
//
//@DESCRIPTION: This component provides generic proctors to automatically lock
// and unlock an external synchronization object for reading.  The
// synchronization object can be any type (e.g., 'bcemt_ReaderWriterLock')
// that provides the following methods:
//..
//  void lockRead();
//  void unlock();
//..
// Both 'bcemt_ReadLockGuard' and 'bcemt_ReadLockGuardUnlock' implement the
// "construction is acquisition, destruction is release" idiom.  During
// construction, 'bcemt_ReadLockGuard' automatically calls 'lockRead' on the
// user-supplied object, and 'unlock' when it is destroyed (unless released).
// 'bcemt_ReadLockGuardUnlock' does the opposite -- it invokes the 'unlock'
// method when constructed and the 'lockRead' method when destroyed.
//
// A third type of guard, 'bcemt_ReadLockGuardTryLock', attempts to acquire a
// lock, and if acquisition succeeds, releases it upon destruction.  Since the
// acquisition is done at construction time, it is not possible to return a
// value to indicate success.  Rather, the 'bcemt_ReadLockGuardTryLock'
// contains a pointer to the synchronization object if 'tryLock' succeeds, and
// is null otherwise.  The synchronization object can be any type (e.g.,
// 'bcemt_Mutex' or 'bcemt_RecursiveMutex') that provides the following
// methods:
//..
//  int tryLockRead();
//  void unlock();
//..
// Note that objects of none of these guard types assumes ownership of the
// synchronization object provided at construction.  Also note that objects of
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
//  static void errorProneFunc(const my_Object *obj, my_RWLock *rwlock)
//  {
//      rwlock->lockRead();
//      if (someCondition) {
//          obj->someMethod();
//          rwlock->unlock();
//          return;
//      } else if (someOtherCondition) {
//          obj->someOtherMethod();
//          return;                      // MISTAKE! forgot to unlock rwlock
//      }
//      obj->defaultMethod();
//      rwlock->unlock();
//      return;
//  }
//..
// The function can be rewritten with a cleaner and safer implementation using
// a guard object.  The 'safeFunc' function is simpler than 'errorProneFunc',
// is exception safe, and avoids the multiple calls to unlock that can be a
// source of errors.
//..
//  static void safeFunc(const my_Object *obj, my_RWLock *rwlock)
//  {
//      bcemt_ReadLockGuard<my_RWLock> guard(rwlock);
//      if (someCondition) {
//          obj->someMethod();
//          return;
//      } else if (someOtherCondition) {
//          obj->someOtherMethod();
//          return;                  // OK, rwlock is automatically unlocked
//      }
//      obj->defaultMethod();
//      return;
//  }
//..
// When blocking while acquiring the lock is not desirable, one may instead use
// a 'bcemt_ReadLockGuardTryLock' in the typical following fashion:
//..
//  static int safeButNonBlockingFunc(const my_Object *obj, my_RWLock *rwlock)
//      // Perform task and return positive value if locking succeeds.
//      // Return 0 if locking fails.
//  {
//      const int RETRIES = 1; // use higher values for higher success rate
//      bcemt_ReadLockGuardTryLock<my_RWLock> guard(rwlock, RETRIES);
//      if (guard.ptr()) { // rwlock is locked
//          if (someCondition) {
//              obj->someMethod();
//              return 2;
//          } else if (someOtherCondition) {
//              obj->someOtherMethod();
//              return 3;
//          }
//          obj->defaultMethod();
//          return 1;
//      }
//      return 0;
//  }
//..
// If the underlying lock object provides an upgrade to a lock for write (as
// does 'bcemt_ReaderWriterLock' with the 'upgradeToWriteLock' function, for
// example), this can be safely used in conjunction with 'bcemt_ReadLockGuard',
// as long as the same 'unlock' method is used to release both kinds of locks.
// The following method illustrates this usage:
//..
//  static void safeUpdateFunc(my_Object *obj, my_RWLock *rwlock)
//  {
//      const my_Object *constObj = obj;
//      bcemt_ReadLockGuard<my_RWLock> guard(rwlock);
//      if (someUpgradeCondition) {
//          rwlock->upgradeToWriteLock();
//          obj->someUpgradeMethod();
//          return;
//      } else if (someOtherCondition) {
//          constObj->someOtherMethod();
//          return;
//      }
//      constObj->defaultMethod();
//      return;
//  }
//..
// In the above code, the call to 'upgradeToWriteLock' is not necessarily
// atomic, as the upgrade may release the lock for read and be interrupted
// before getting a lock for write.  It is possible to guarantee atomicity (as
// does 'bcemt_ReaderWriterLock' if the 'lockReadReserveWrite' function is used
// instead of 'lockRead', for example), but the standard constructor should
// not be used.  Instead, the 'lockReadReserveWrite' lock function should be
// used explicitly, and the guard constructed with an object which is already
// locked.  The following method illustrates this usage:
//..
//  static void safeAtomicUpdateFunc(my_Object *obj, my_RWLock *rwlock)
//  {
//      const my_Object *constObj = obj;
//      rwlock->lockReadReserveWrite();
//      const int PRELOCKED = 1;
//      bcemt_ReadLockGuard<my_RWLock> guard(rwlock, PRELOCKED);
//      if (someUpgradeCondition) {
//          rwlock->upgradeToWriteLock();
//          obj->someUpgradeMethod();
//          return;
//      } else if (someOtherCondition) {
//          constObj->someOtherMethod();
//          return;
//      }
//      constObj->defaultMethod();
//      return;
//  }
//..
// Note that in the code above, the function 'rwlock->lockRead()' is never
// called, but is nevertheless required for the code to compile.
//
// Instantiations of 'bcemt_ReadLockGuardUnlock' can be interleaved with
// instantiations of 'bcemt_ReadLockGuard' to create both critical sections and
// regions where the lock is released.
//..
//  void f(my_RWLock *lock)
//  {
//      bcemt_ReadLockGuard<my_RWLock> guard(lock);
//
//      // critical section here
//
//      {
//           bcemt_ReadLockGuardUnlock<my_RWLock> guard(lock);
//
//          // mutex is unlocked here
//
//      } // lock reacquired upon destruction
//
//      // critical section here
//
//  } // lock is unlocked here
//..
// Care must be taken so as not to interleave guard objects in such a way as to
// cause an illegal sequence of calls on a lock (two sequential lock calls or
// two sequential unlock calls on a non-recursive read/write lock).

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

namespace BloombergLP {

                           // =========================
                           // class bcemt_ReadLockGuard
                           // =========================

template <class T>
class bcemt_ReadLockGuard {
    // This class template implements a proctor for acquisition and release of
    // read synchronization resources (i.e., reader locks).

    // DATA
    T *d_lock_p;  // lock proctored by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    bcemt_ReadLockGuard(const bcemt_ReadLockGuard<T>&);
    bcemt_ReadLockGuard<T>& operator=(const bcemt_ReadLockGuard<T>&);

  public:
    // CREATORS
    explicit bcemt_ReadLockGuard(T *lock);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero), and invokes the 'lockRead' method on 'lock'.
        // Note that 'lock' must remain valid throughout the lifetime of this
        // proctor, or until 'release' is called.

    bcemt_ReadLockGuard(T *lock, int preLockedFlag);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero) and, unless the specified 'preLockedFlag' is
        // non-zero, invokes the 'lockRead' method on 'lock'.  Note that 'lock'
        // must remain valid throughout the lifetime of this proctor, or until
        // 'release' is called.

    ~bcemt_ReadLockGuard();
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

                           // =========================
                           // class bcemt_LockReadGuard
                           // =========================

template <class T>
class bcemt_LockReadGuard : public bcemt_ReadLockGuard<T> {
    // This class is DEPRECATED.  Use 'bcemt_ReadLockGuard' instead.

  private:
    // NOT IMPLEMENTED
    bcemt_LockReadGuard(const bcemt_LockReadGuard<T>&);
    bcemt_LockReadGuard<T>& operator=(const bcemt_LockReadGuard<T>&);

  public:
    // CREATORS
    explicit bcemt_LockReadGuard(T *lock);
        // DEPRECATED: Use 'bcemt_ReadLockGuard' instead.

    bcemt_LockReadGuard(T *lock, int preLockedFlag);
        // DEPRECATED: Use 'bcemt_ReadLockGuard' instead.
};

                       // ===============================
                       // class bcemt_ReadLockGuardUnlock
                       // ===============================

template <class T>
class bcemt_ReadLockGuardUnlock {
    // This class template implements a proctor for release and reacquisition
    // of read synchronization resources (i.e., reader locks).

    // DATA
    T *d_lock_p;  // lock proctored by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    bcemt_ReadLockGuardUnlock(const bcemt_ReadLockGuardUnlock<T>&);
    bcemt_ReadLockGuardUnlock<T>& operator=
                              (const bcemt_ReadLockGuardUnlock<T>&);

  public:
    // CREATORS
    explicit bcemt_ReadLockGuardUnlock(T *lock);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero), and invokes the 'unlock' method on 'lock'.
        // Note that 'lock' must remain valid throughout the lifetime of this
        // proctor, or until 'release' is called.

    bcemt_ReadLockGuardUnlock(T *lock, int preUnlockedFlag);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero) and, unless the specified 'preUnlockedFlag' is
        // non-zero, invokes the 'unlock' method on 'lock'.  Note that 'lock'
        // must remain valid throughout the lifetime of this proctor, or until
        // 'release' is called.

    ~bcemt_ReadLockGuardUnlock();
        // Destroy this proctor object and invoke the 'lockRead' method on the
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

                       // ================================
                       // class bcemt_ReadLockGuardTryLock
                       // ================================

template <class T>
class bcemt_ReadLockGuardTryLock {
    // This class template implements a proctor for tentative acquisition and
    // release of read synchronization resources (i.e., reader locks).

    // DATA
    T *d_lock_p;  // lock proctored by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    bcemt_ReadLockGuardTryLock(const bcemt_ReadLockGuardTryLock<T>&);
    bcemt_ReadLockGuardTryLock<T>& operator=(
                               const bcemt_ReadLockGuardTryLock<T>&);

  public:
    // CREATORS
    explicit bcemt_ReadLockGuardTryLock(T *lock, int attempts = 1);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero), and invokes the 'tryLockRead' method on 'lock'
        // until the lock is acquired, or until up to the specified 'attempts'
        // have been made to acquire the lock.  The behavior is undefined
        // unless '0 < attempts'.  Note that 'lock' must remain valid
        // throughout the lifetime of this proctor, or until 'release' is
        // called.

    ~bcemt_ReadLockGuardTryLock();
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

                          // -------------------------
                          // class bcemt_ReadLockGuard
                          // -------------------------

// CREATORS
template <class T>
inline
bcemt_ReadLockGuard<T>::bcemt_ReadLockGuard(T *lock)
: d_lock_p(lock)
{
    if (d_lock_p) {
        d_lock_p->lockRead();
    }
}

template <class T>
inline
bcemt_ReadLockGuard<T>::bcemt_ReadLockGuard(T *lock, int preLocked)
: d_lock_p(lock)
{
    if (d_lock_p && !preLocked) {
        d_lock_p->lockRead();
    }
}

template <class T>
inline
bcemt_ReadLockGuard<T>::~bcemt_ReadLockGuard()
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

// MANIPULATORS
template <class T>
inline
T *bcemt_ReadLockGuard<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bcemt_ReadLockGuard<T>::ptr() const
{
    return d_lock_p;
}

                          // -------------------------
                          // class bcemt_LockReadGuard
                          // -------------------------

// CREATORS
template <class T>
inline
bcemt_LockReadGuard<T>::bcemt_LockReadGuard(T *lock)
: bcemt_ReadLockGuard<T>(lock)
{
}

template <class T>
inline
bcemt_LockReadGuard<T>::bcemt_LockReadGuard(T *lock, int preLocked)
: bcemt_ReadLockGuard<T>(lock, preLocked)
{
}

                       // -------------------------------
                       // class bcemt_ReadLockGuardUnlock
                       // -------------------------------

// CREATORS
template <class T>
inline
bcemt_ReadLockGuardUnlock<T>::bcemt_ReadLockGuardUnlock(T *lock)
: d_lock_p(lock)
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

template <class T>
inline
bcemt_ReadLockGuardUnlock<T>::bcemt_ReadLockGuardUnlock(T   *lock,
                                                        int  preUnlocked)
: d_lock_p(lock)
{
    if (d_lock_p && !preUnlocked) {
        d_lock_p->unlock();
    }
}

template <class T>
inline
bcemt_ReadLockGuardUnlock<T>::~bcemt_ReadLockGuardUnlock()
{
    if (d_lock_p) {
        d_lock_p->lockRead();
    }
}

// MANIPULATORS
template <class T>
inline
T *bcemt_ReadLockGuardUnlock<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bcemt_ReadLockGuardUnlock<T>::ptr() const
{
    return d_lock_p;
}

                          // --------------------------------
                          // class bcemt_ReadLockGuardTryLock
                          // --------------------------------

// CREATORS
template <class T>
bcemt_ReadLockGuardTryLock<T>::bcemt_ReadLockGuardTryLock(T *lock, int retries)
: d_lock_p(0)
{
    if (lock) {
        while (retries--) {
            if (!lock->tryLockRead()) {
                d_lock_p = lock;
                break;
            }
        }
    }
}

template <class T>
inline
bcemt_ReadLockGuardTryLock<T>::~bcemt_ReadLockGuardTryLock()
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

// MANIPULATORS
template <class T>
inline
T *bcemt_ReadLockGuardTryLock<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bcemt_ReadLockGuardTryLock<T>::ptr() const
{
    return d_lock_p;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
