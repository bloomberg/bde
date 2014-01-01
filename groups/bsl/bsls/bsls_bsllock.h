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
// instead.
//
// The behavior is undefined if 'lock' is called twice on a 'bsls::BslLock'
// object from a thread without an intervening call to 'unlock' (i.e.,
// 'bsls::BslLock' is non-recursive), or if 'unlock' is invoked from a thread
// that does not hold the lock.  In particular, a call to 'lock' *may* deadlock
// if the calling thread already holds the lock.
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
// The following snippets of code illustrate the use of 'bsls::BslLock' and
// 'bsls::BslLockGuard' to write a thread-safe class, 'my_SafeAccount', given
// a thread-unsafe class, 'my_Account'.  The simple 'my_Account' class is
// defined as follows:
//..
//  class my_Account {
//      // This 'class' represents a bank account with a single balance.  It
//      // is not thread-safe.
//
//      // DATA
//      double d_money;  // amount of money in the account
//
//    public:
//      // CREATORS
//      my_Account();
//          // Create an account having a zero balance.
//
//      my_Account(const my_Account& original);
//          // Create an account having the value of the specified 'original'
//          // account.
//
//      ~my_Account();
//          // Destroy this account.
//
//      // MANIPULATORS
//      my_Account& operator=(const my_Account& rhs);
//          // Assign to this account the value of the specified 'rhs' account,
//          // and return a reference providing modifiable access to this
//          // account.
//
//      void deposit(double amount);
//          // Deposit the specified 'amount' of money into this account.
//          // The behavior is undefined unless 'amount >= 0.0'.
//
//      void withdraw(double amount);
//          // Withdraw the specified 'amount' of money from this account.
//          // The behavior is undefined unless 'amount >= 0.0'.
//
//      // ACCESSORS
//      double balance() const;
//          // Return the amount of money that is available for withdrawal
//          // from this account.
//  };
//..
// The implementation of 'my_Account' is straightforward and omitted here for
// brevity.
//
// Next, we use a 'bsls::BslLock' data member to render atomic the function
// calls of a new thread-safe class that uses the thread-unsafe class in its
// implementation.  Note the typical use of 'mutable' for declaring the lock:
//..
//  class my_SafeAccountHandle {
//      // This 'class' provides a thread-safe handle to the thread-unsafe
//      // account that is supplied at construction.
//
//      // DATA
//      my_Account            *d_account_p;  // held, not owned
//      mutable bsls::BslLock  d_lock;       // guard access to 'd_account_p'
//
//    private:
//      // NOT IMPLEMENTED
//      my_SafeAccountHandle(const my_SafeAccountHandle&);
//      my_SafeAccountHandle& operator=(const my_SafeAccountHandle&);
//
//    public:
//      // CREATORS
//      my_SafeAccountHandle(my_Account *account);
//          // Create a thread-safe handle to the specified modifiable
//          // 'account'.
//
//      ~my_SafeAccountHandle();
//          // Destroy this handle.  Note that the held account is unaffected
//          // by this operation.
//
//      // MANIPULATORS
//      my_Account *account();
//          // Return an address providing modifiable access to the account
//          // held by this handle.
//
//      void deposit(double amount);
//          // Atomically deposit the specified 'amount' of money into the
//          // account held by this handle.  The behavior is undefined unless
//          // the calling thread does not hold the lock on this handle and
//          // 'amount >= 0.0'.  Note that this operation is thread-safe.
//
//      void lock();
//          // Acquire the lock that provides exclusive access to the
//          // underlying account held by this object.  The behavior is
//          // undefined unless the calling thread does not already hold the
//          // lock on this handle.
//
//      void unlock();
//          // Release the lock that provides exclusive access to the
//          // underlying account held by this object.  The behavior is
//          // undefined unless the calling thread holds the lock on this
//          // handle.
//
//      void withdraw(double amount);
//          // Atomically withdraw the specified 'amount' of money from the
//          // account held by this handle.  The behavior is undefined unless
//          // the calling thread does not hold the lock on this handle and
//          // 'amount >= 0.0'.  Note that this operation is thread-safe.
//
//      // ACCESSORS
//      const my_Account *account() const;
//          // Return an address providing non-modifiable access to the account
//          // held by this handle.
//
//      double balance() const;
//          // Atomically return the amount of money that is available for
//          // withdrawal from the account held by this handle.  The behavior
//          // is undefined unless the calling thread does not hold the lock
//          // on this handle.
//  };
//..
// The implementation of 'my_SafeAccountHandle' show-casing the use of
// 'bsls::BslLock', 'bsls::BslLockGuard', and 'my_Account' follows:
//..
//  // CREATORS
//  my_SafeAccountHandle::my_SafeAccountHandle(my_Account *account)
//  : d_account_p(account)
//  {
//  }
//
//  my_SafeAccountHandle::~my_SafeAccountHandle()
//  {
//  }
//
//  // MANIPULATORS
//  my_Account *my_SafeAccountHandle::account()
//  {
//      return d_account_p;
//  }
//
//  void my_SafeAccountHandle::deposit(double amount)
//  {
//..
// In this method we make direct use of the interface of 'bsls::BslLock'.
// However, wherever appropriate, clients should use a 'bsls::BslLockGuard'
// object to ensure that an acquired mutex is always properly released, even if
// an exception is thrown:
//..
//      d_lock.lock();  // consider using 'bsls::BslLockGuard'
//      d_account_p->deposit(amount);
//      d_lock.unlock();
//  }
//
//  void my_SafeAccountHandle::lock()
//  {
//      d_lock.lock();
//  }
//
//  void my_SafeAccountHandle::unlock()
//  {
//      d_lock.unlock();
//  }
//..
// In the implementation of 'withdraw' we make use of the 'lock' and 'unlock'
// methods provided by 'my_SafeAccountHandle':
//..
//  void my_SafeAccountHandle::withdraw(double amount)
//  {
//      lock();         // consider using 'bsls::BslLockGuard'
//      d_account_p->withdraw(amount);
//      unlock();
//  }
//
//  // ACCESSORS
//  const my_Account *my_SafeAccountHandle::account() const
//  {
//      return d_account_p;
//  }
//..
// The implementation of 'balance' uses a 'bsls::BslLockGuard' to automatically
// acquire and release the lock.  The lock is acquired as a side-effect of the
// construction of 'guard', and released when the guard object is destroyed
// upon returning from the function:
//..
//  double my_SafeAccountHandle::balance() const
//  {
//      bsls::BslLockGuard guard(&d_lock);  // a very good practice
//
//      return d_account_p->balance();
//  }
//..
// The handle's atomic methods are used just as the corresponding methods in
// 'my_Account':
//..
//  my_Account account;
//  account.deposit(100.50);
//  const double paycheck = 50.25;
//  my_SafeAccountHandle handle(&account);
//
//                             assert(100.50 == handle.balance());
//  handle.deposit(paycheck);  assert(150.75 == handle.balance());
//..
// Client code can also directly use the handle's 'lock' and 'unlock' methods
// to effect non-primitive atomic transactions on the account:
//..
//  const double check[5] = { 25.0, 100.0, 99.95, 75.0, 50.0 };
//
//  handle.lock();
//
//  const double originalBalance = handle.account()->balance();
//  for (int i = 0; i < 5; ++i) {
//      handle.account()->deposit(check[i]);
//  }
//  assert(originalBalance + 349.95 == handle.account()->balance());
//
//  handle.unlock();
//..

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS

// Rather than setting 'WINVER' or 'NTDDI_VERSION', just forward-declare the
// Windows 2000 functions that are used.

struct _RTL_CRITICAL_SECTION;

typedef struct _RTL_CRITICAL_SECTION *LPCRITICAL_SECTION;

typedef int           BOOL;
typedef unsigned long DWORD;

extern "C" {
    __declspec(dllimport) BOOL __stdcall InitializeCriticalSectionAndSpinCount(
                                         LPCRITICAL_SECTION lpCriticalSection,
                                         DWORD              dwSpinCount);

    __declspec(dllimport) void __stdcall DeleteCriticalSection(
                                         LPCRITICAL_SECTION lpCriticalSection);

    __declspec(dllimport) void __stdcall EnterCriticalSection(
                                         LPCRITICAL_SECTION lpCriticalSection);

    __declspec(dllimport) void __stdcall LeaveCriticalSection(
                                         LPCRITICAL_SECTION lpCriticalSection);

}  // extern "C"

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
    // by this class is *not* error checking, and is *non*-recursive.  Note
    // that 'BslLock' is *not* intended for direct use by client code; it is
    // meant for internal use only.

#ifdef BSLS_PLATFORM_OS_WINDOWS
  private:
    // PRIVATE TYPES
    enum {
#ifdef BSLS_PLATFORM_CPU_64_BIT
        CRITICAL_SECTION_BUFFER_SIZE = 5 * sizeof(void *)
#else
        CRITICAL_SECTION_BUFFER_SIZE = 6 * sizeof(void *)
#endif
    };
#endif

    // DATA
#ifdef BSLS_PLATFORM_OS_WINDOWS
    void *d_lock[CRITICAL_SECTION_BUFFER_SIZE];  // 'CriticalSection' buffer
#else
    pthread_mutex_t d_lock;                      // pthreads mutex object
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
        // being managed, this method has no effect.  The behavior is undefined
        // unless the calling thread holds the lock on the object managed by
        // this guard (if any).

    // MANIPULATORS
    void release();
        // Release from management the object currently managed by this guard,
        // if any.  If no object is currently being managed, this method has no
        // effect.
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

    InitializeCriticalSectionAndSpinCount(
                          reinterpret_cast<_RTL_CRITICAL_SECTION *>(d_lock),
                          BSLS_SPIN_COUNT);
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
    DeleteCriticalSection(reinterpret_cast<_RTL_CRITICAL_SECTION *>(d_lock));
#else
    const int status = pthread_mutex_destroy(&d_lock);
    BSLS_ASSERT(0 == status);
#endif
}

// MANIPULATORS
inline
void BslLock::lock()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    EnterCriticalSection( reinterpret_cast<_RTL_CRITICAL_SECTION *>(d_lock));
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
    LeaveCriticalSection( reinterpret_cast<_RTL_CRITICAL_SECTION *>(d_lock));
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
