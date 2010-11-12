// bcemt_mutex.h                                                      -*-C++-*-
#ifndef INCLUDED_BCEMT_MUTEX
#define INCLUDED_BCEMT_MUTEX

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a platform-independent mutex.
//
//@CLASSES:
//  bcemt_Mutex: platform-independent mutex
//
//@SEE_ALSO: bcemt_recursivemutex, bcemt_mutex
//
//@DESCRIPTION: This component provides a mutually exclusive lock primitive
// ("mutex") by wrapping a suitable platform-specific mechanism.  The
// 'bcemt_Mutex' class provides the following operations: 'lock', 'tryLock',
// and 'unlock'.
//
// The behavior is undefined if 'unlock' is invoked on a 'bcemt_Mutex' object
// from a thread that did not successfully acquire the lock, or if 'lock' is
// called twice in a thread without calling 'unlock' in between (i.e.,
// 'bcemt_Mutex' is non-recursive).  In particular, 'lock' *may* or *may* *not*
// deadlock if the current thread holds the lock.
//
///Usage
///-----
// The following snippets of code illustrate the use of 'bcemt_Mutex' to write
// a thread-safe class, 'my_SafeAccount', given a thread-unsafe class,
// 'my_Account'.  The simple 'my_Account' class is defined as follows:
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
//          // Create an account with zero balance.
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
//          // and return a reference to this modifiable account.
//
//      void deposit(double amount);
//          // Deposit the specified 'amount' of money into this account.
//
//      void withdraw(double amount);
//          // Withdraw the specified 'amount' of money from this account.
//
//      // ACCESSORS
//      double balance() const;
//          // Return the amount of money that is available for withdrawal
//          // from this account.
//  };
//..
// The implementation of this class is straightforward and omitted here for
// brevity.
//
// Next, we use a 'bcemt_Mutex' object to render atomic the function calls of a
// new thread-safe class that uses the thread-unsafe class in its
// implementation.  Note the typical use of 'mutable' for the lock:
//..
//  class my_SafeAccountHandle {
//      // This 'class' provides a thread-safe handle to an account (held,
//      // not owned) passed at construction.
//
//      // DATA
//      my_Account          *d_account_p;  // held, not owned
//      mutable bcemt_Mutex  d_lock;       // guard access to 'd_account_p'
//
//      // NOT IMPLEMENTED
//      my_SafeAccountHandle(const my_SafeAccountHandle&);
//      my_SafeAccountHandle& operator=(const my_SafeAccountHandle&);
//
//    public:
//      // CREATORS
//      my_SafeAccountHandle(my_Account *account);
//          // Create a thread-safe handle to the specified 'account'.
//
//      ~my_SafeAccountHandle();
//          // Destroy this handle.  Note that the held account is unaffected
//          // by this operation.
//
//      // MANIPULATORS
//      void deposit(double amount);
//          // Atomically deposit the specified 'amount' of money into the
//          // account held by this handle.  Note that this operation is
//          // thread-safe; no 'lock' is needed.
//
//      void lock();
//          // Provide exclusive access to the underlying account held by this
//          // object.
//
//      void unlock();
//          // Release exclusivity of the access to the underlying account held
//          // by this object.
//
//      void withdraw(double amount);
//          // Atomically withdraw the specified 'amount' of money from the
//          // account held by this handle.  Note that this operation is
//          // thread-safe; no 'lock' is needed.
//
//      // ACCESSORS
//      my_Account *account() const;
//          // Return the address of the modifiable account held by this
//          // handle.
//
//      double balance() const;
//          // Atomically return the amount of money that is available for
//          // withdrawal from the account held by this handle.
//  };
//..
// The implementation show-casing the use of 'bcemt_Mutex' follows:
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
//  void my_SafeAccountHandle::deposit(double amount)
//  {
//..
// Where appropriate, clients should use a lock-guard to ensure that an
// acquired mutex is always properly released, even if an exception is thrown.
// See 'bcemt_lockguard' for more information:
//..
//      d_lock.lock();  // consider using 'bcemt_LockGuard'
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
//
//  void my_SafeAccountHandle::withdraw(double amount)
//  {
//      d_lock.lock();  // consider using 'bcemt_LockGuard'
//      d_account_p->withdraw(amount);
//      d_lock.unlock();
//  }
//
//  // ACCESSORS
//  my_Account *my_SafeAccountHandle::account() const
//  {
//      return d_account_p;
//  }
//
//  double my_SafeAccountHandle::balance() const
//  {
//      d_lock.lock();  // consider using 'bcemt_LockGuard'
//      const double res = d_account_p->balance();
//      d_lock.unlock();
//      return res;
//  }
//..
// The handle's atomic methods are used just as the corresponding methods in
// 'my_Account':
//..
//  Account account;
//  account.deposit(100.50);
//  double paycheck = 50.25;
//  my_SafeAccountHandle handle(&account);
//
//                             assert(100.50 == handle.balance());
//  handle.deposit(paycheck);  assert(150.75 == handle.balance());
//..
// We can also use the handle's 'lock' and 'unlock' methods to implement
// non-primitive atomic transactions on the account:
//..
//  double check[5] = { 25.0, 100.0, 99.95, 75.0, 50.0 };
//
//  handle.lock();  // consider using 'bcemt_LockGuard'
//
//  double originalBalance = handle.account()->balance();
//  for (int i = 0; i < 5; ++i) {
//      handle.account()->deposit(check[i]);
//  }
//  assert(originalBalance + 349.95 == handle.account()->balance());
//  handle.unlock();
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEXIMPL_PTHREAD
#include <bcemt_muteximpl_pthread.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEXIMPL_WIN32
#include <bcemt_muteximpl_win32.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

namespace BloombergLP {

template <typename THREAD_POLICY>
class bcemt_MutexImpl;

                             // =================
                             // class btemt_Mutex
                             // =================

class bcemt_Mutex {
    // This 'class' implements a lightweight, portable wrapper of an OS-level
    // mutex lock to support intra-process synchronization.  The behavior is
    // undefined if the 'lock' method of this class is invoked more than once
    // on the same mutex object in the same thread without an intervening call
    // to 'unLock'.

    // DATA
    bcemt_MutexImpl<bces_Platform::ThreadPolicy>
                                    d_imp;  // platform-specific implementation

    // NOT IMPLEMENTED
    bcemt_Mutex(const bcemt_Mutex&);
    bcemt_Mutex& operator=(const bcemt_Mutex&);

  public:
    // PUBLIC TYPES
    typedef bcemt_MutexImpl<bces_Platform::ThreadPolicy>::NativeType
                                                                    NativeType;
        // 'NativeType' is an alias for the underlying OS-level mutex type.  It
        // is exposed so that other 'bcemt' components can operate directly on
        // this mutex.

    // CREATORS
    bcemt_Mutex();
        // Create a mutex object in the unlocked state.

    ~bcemt_Mutex();
        // Destroy this mutex object.  The behavior is undefined if the mutex
        // is in a locked state.

    // MANIPULATORS
    void lock();
        // Acquire a lock on this mutex object.  If this object is currently
        // locked by a different thread, then suspend execution of the current
        // thread until a lock can be acquired.  The behavior is undefined if
        // the calling thread already owns the lock on this mutex, and may
        // result in deadlock.

    NativeType& nativeMutex();
        // Return a reference to the modifiable OS-level mutex underlying this
        // object.  This method is intended only to support other 'bcemt'
        // components that must operate directly on this mutex.

    int tryLock();
        // Attempt to acquire a lock on this mutex object.  Return 0 on
        // success, and a non-zero value if this object is already locked by
        // a different thread.  The behavior is undefined if the calling thread
        // already owns the lock on this mutex, and may result in deadlock.

    void unlock();
        // Release a lock on this mutex that was previously acquired through a
        // call to 'lock', or a successful call to 'tryLock', enabling another
        // thread to acquire a lock on this mutex.  The behavior is undefined
        // unless the calling thread currently owns the lock on this mutex.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                             // -----------------
                             // class bcemt_Mutex
                             // -----------------

// CREATORS
inline
bcemt_Mutex::bcemt_Mutex()
{
}

inline
bcemt_Mutex::~bcemt_Mutex()
{
}

// MANIPULATORS
inline
void bcemt_Mutex::lock()
{
    d_imp.lock();
}

inline
bcemt_Mutex::NativeType& bcemt_Mutex::nativeMutex()
{
    return d_imp.nativeMutex();
}

inline
int bcemt_Mutex::tryLock()
{
    return d_imp.tryLock();
}

inline
void bcemt_Mutex::unlock()
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
