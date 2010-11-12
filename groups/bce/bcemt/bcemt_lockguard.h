// bcemt_lockguard.h                                                  -*-C++-*-
#ifndef INCLUDED_BCEMT_LOCKGUARD
#define INCLUDED_BCEMT_LOCKGUARD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a generic proctor for synchronization objects.
//
//@CLASSES:
//  bcemt_LockGuard: automatic locking-unlocking of mutexes
//  bcemt_LockGuardUnlock: automatic unlocking-locking of mutexes
//  bcemt_LockGuardTryLock: automatic non-blocking locking-unlocking of mutexes
//  bcemt_UnLockGuard: DEPRECATED
//  bcemt_TryLockGuard: DEPRECATED
//
//@SEE_ALSO: bcemt_thread, bcemt_readlockguard, bcemt_writelockguard
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides generic proctors to automatically
// lock and unlock an external synchronization object.  The synchronization
// object can be any type (e.g., 'bcemt_Mutex' or 'bcemt_RecursiveMutex')
// that provides the following methods:
//..
//  void lock();
//  void unlock();
//..
// Both 'bcemt_LockGuard' and 'bcemt_LockGuardUnlock' implement the
// "construction is acquisition, destruction is release" idiom.  During
// construction, 'bcemt_LockGuard' automatically calls 'lock' on the
// user-supplied object, and 'unlock' when it is destroyed (unless released).
// 'bcemt_LockGuardUnlock' does the opposite -- it invokes the 'unlock' method
// when constructed and the 'lock' method when it is destroyed.
//
// A third type of guard, 'bcemt_LockGuardTryLock', attempts to acquire a
// lock, and if acquisition succeeds, releases it upon destruction.  Since the
// acquisition is done at construction time, it is not possible to return a
// value to indicate success.  Rather, the 'bcemt_LockGuardTryLock' contains a
// pointer to the synchronization object if 'tryLock' succeeds, and is null
// otherwise.  The synchronization object can be any type (e.g., 'bcemt_Mutex'
// or 'bcemt_RecursiveMutex') that provides the following methods:
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
//          return;
//      } else if (someOtherCondition) {
//          obj->someOtherMethod();
//          return;                       // MISTAKE! forgot to unlock mutex
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
//      bcemt_LockGuard<my_Mutex> guard(mutex);
//      if (someCondition) {
//          obj->someMethod();
//          return;
//      } else if (someOtherCondition) {
//          obj->someOtherMethod();
//          return;                   // OK, mutex is automatically unlocked
//      }
//      obj->defaultMethod();
//      return;
//  }
//..
// When blocking while acquiring the lock is not desirable, one may instead use
// a 'bcemt_LockGuardTryLock' in the typical following fashion:
//..
//  static int safeButNonBlockingFunc(my_Object *obj, my_Mutex *mutex)
//      // Perform task and return positive value if locking succeeds.
//      // Return 0 if locking fails.
//  {
//      const int RETRIES = 1; // use higher values for higher success rate
//      bcemt_LockGuardTryLock<my_Mutex> guard(mutex, RETRIES);
//      if (guard.ptr()) { // mutex is locked
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
// Instantiations of 'bcemt_LockGuardUnlock' can be interleaved with
// instantiations of 'bcemt_LockGuard' to create both critical sections
// and regions where the lock is released.
//..
//  void f(my_Mutex *mutex)
//  {
//      bcemt_LockGuard<my_Mutex> guard(mutex);
//
//      // critical section here
//
//      {
//           bcemt_LockGuardUnlock<my_Mutex> guard(mutex);
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

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

namespace BloombergLP {

                           // =====================
                           // class bcemt_LockGuard
                           // =====================

template <class T>
class bcemt_LockGuard {
    // This class template implements a proctor for acquisition and release of
    // synchronization resources (i.e., locks).

    // DATA
    T *d_lock_p;  // lock proctored by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    bcemt_LockGuard(const bcemt_LockGuard<T>&);
    bcemt_LockGuard<T>& operator=(const bcemt_LockGuard<T>&);

  public:
    // CREATORS
    explicit bcemt_LockGuard(T *lock);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero), and invokes the 'lock' method on 'lock'.  Note
        // that 'lock' must remain valid throughout the lifetime of this
        // proctor, or until 'release' is called.

    bcemt_LockGuard(T *lock, int preLockedFlag);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero) and, unless the specified 'preLockedFlag' is
        // non-zero, invokes the 'lock' method on 'lock'.  Note that 'lock'
        // must remain valid throughout the lifetime of this proctor, or until
        // 'release' is called.

    ~bcemt_LockGuard();
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

                         // ===========================
                         // class bcemt_LockGuardUnlock
                         // ===========================

template <class T>
class bcemt_LockGuardUnlock {
    // This class template implements a proctor for release and reacquisition
    // of synchronization resources (i.e., locks).

    // DATA
    T *d_lock_p;  // lock proctored by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    bcemt_LockGuardUnlock(const bcemt_LockGuardUnlock<T>&);
    bcemt_LockGuardUnlock<T>& operator=(const bcemt_LockGuardUnlock<T>&);

  public:
    // CREATORS
    explicit bcemt_LockGuardUnlock(T *lock);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero), and invokes the 'unlock' method on 'lock'.
        // Note that 'lock' must remain valid throughout the lifetime of this
        // proctor, or until 'release' is called.

    bcemt_LockGuardUnlock(T *lock, int preUnlockedFlag);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero) and, unless the specified 'preUnlockedFlag' is
        // non-zero, invokes the 'unlock' method on 'lock'.  Note that 'lock'
        // must remain valid throughout the lifetime of this proctor, or until
        // 'release' is called.

    ~bcemt_LockGuardUnlock();
        // Destroy this proctor object and invoke the 'lock' method on the lock
        // object under management by this proctor, if any.  If no lock is
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

                          // =======================
                          // class bcemt_UnLockGuard
                          // =======================

template <class T>
class bcemt_UnLockGuard : public bcemt_LockGuardUnlock<T> {
    // This class is DEPRECATED.  Use 'bcemt_LockGuardUnlock' instead.

  private:
    // NOT IMPLEMENTED
    bcemt_UnLockGuard(const bcemt_UnLockGuard<T>&);
    bcemt_UnLockGuard<T>& operator=(const bcemt_UnLockGuard<T>&);

  public:
    // CREATORS
    explicit bcemt_UnLockGuard(T *lock);
        // DEPRECATED: Use 'bcemt_LockGuardUnlock' instead.

    bcemt_UnLockGuard(T *lock, int preUnlockedFlag);
        // DEPRECATED: Use 'bcemt_LockGuardUnlock' instead.
};

                        // ============================
                        // class bcemt_LockGuardTryLock
                        // ============================

template <class T>
class bcemt_LockGuardTryLock {
    // This class template implements a proctor for tentative acquisition and
    // release of synchronization resources (i.e., locks).

    // DATA
    T *d_lock_p;  // lock proctored by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    bcemt_LockGuardTryLock(const bcemt_LockGuardTryLock<T>&);
    bcemt_LockGuardTryLock<T>& operator=(const bcemt_LockGuardTryLock<T>&);

  public:
    // CREATORS
    explicit bcemt_LockGuardTryLock(T *lock, int attempts = 1);
        // Create a proctor object that conditionally manages the specified
        // 'lock' (if non-zero), and invokes the 'tryLock' method on 'lock'
        // until the lock is acquired, or until up to the specified 'attempts'
        // have been made to acquire the lock.  The behavior is undefined
        // unless '0 < attempts'.  Note that 'lock' must remain valid
        // throughout the lifetime of this proctor, or until 'release' is
        // called.

    ~bcemt_LockGuardTryLock();
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

                          // ========================
                          // class bcemt_TryLockGuard
                          // ========================

template <class T>
class bcemt_TryLockGuard : public bcemt_LockGuardTryLock<T> {
    // This class is DEPRECATED.  Use 'bcemt_LockGuardTryLock' instead.

  private:
    // NOT IMPLEMENTED
    bcemt_TryLockGuard(const bcemt_TryLockGuard<T>&);
    bcemt_TryLockGuard<T>& operator=(const bcemt_TryLockGuard<T>&);

  public:
    // CREATORS
    explicit bcemt_TryLockGuard(T *lock, int attempts = 1);
        // DEPRECATED: Use 'bcemt_LockGuardTryLock' instead.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // ---------------------
                          // class bcemt_LockGuard
                          // ---------------------

// CREATORS
template <class T>
inline
bcemt_LockGuard<T>::bcemt_LockGuard(T *lock)
: d_lock_p(lock)
{
    if (d_lock_p) {
        d_lock_p->lock();
    }
}

template <class T>
inline
bcemt_LockGuard<T>::bcemt_LockGuard(T *lock, int preLocked)
: d_lock_p(lock)
{
    if (d_lock_p && !preLocked) {
        d_lock_p->lock();
    }
}

template <class T>
inline
bcemt_LockGuard<T>::~bcemt_LockGuard()
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

// MANIPULATORS
template <class T>
inline
T *bcemt_LockGuard<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bcemt_LockGuard<T>::ptr() const
{
    return d_lock_p;
}

                          // -----------------------
                          // class bcemt_UnLockGuard
                          // -----------------------

// CREATORS
template <class T>
inline
bcemt_UnLockGuard<T>::bcemt_UnLockGuard(T *lock)
: bcemt_LockGuardUnlock<T>(lock)
{
}

template <class T>
inline
bcemt_UnLockGuard<T>::bcemt_UnLockGuard(T *lock, int preUnlockedFlag)
: bcemt_LockGuardUnlock<T>(lock, preUnlockedFlag)
{
}

                          // ------------------------
                          // class bcemt_TryLockGuard
                          // ------------------------

template <class T>
inline
bcemt_TryLockGuard<T>::bcemt_TryLockGuard(T *lock, int attempts)
: bcemt_LockGuardTryLock<T>(lock, attempts)
{
}

                         // ---------------------------
                         // class bcemt_LockGuardUnlock
                         // ---------------------------

// CREATORS
template <class T>
inline
bcemt_LockGuardUnlock<T>::bcemt_LockGuardUnlock(T *lock)
: d_lock_p(lock)
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

template <class T>
inline
bcemt_LockGuardUnlock<T>::bcemt_LockGuardUnlock(T *lock, int preUnlocked)
: d_lock_p(lock)
{
    if (d_lock_p && !preUnlocked) {
        d_lock_p->unlock();
    }
}

template <class T>
inline
bcemt_LockGuardUnlock<T>::~bcemt_LockGuardUnlock()
{
    if (d_lock_p) {
        d_lock_p->lock();
    }
}

// MANIPULATORS
template <class T>
inline
T *bcemt_LockGuardUnlock<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bcemt_LockGuardUnlock<T>::ptr() const
{
    return d_lock_p;
}

                          // ----------------------------
                          // class bcemt_LockGuardTryLock
                          // ----------------------------

// CREATORS
template <class T>
bcemt_LockGuardTryLock<T>::bcemt_LockGuardTryLock(T *lock, int attempts)
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
bcemt_LockGuardTryLock<T>::~bcemt_LockGuardTryLock()
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

// MANIPULATORS
template <class T>
inline
T *bcemt_LockGuardTryLock<T>::release()
{
    T *lock  = d_lock_p;
    d_lock_p = 0;
    return lock;
}

// ACCESSORS
template <class T>
inline
T *bcemt_LockGuardTryLock<T>::ptr() const
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
