// bslmt_mutex.h                                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_MUTEX
#define INCLUDED_BSLMT_MUTEX

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a platform-independent mutex.
//
//@CLASSES:
//  bslmt::Mutex: platform-independent mutex
//
//@SEE_ALSO: bslmt_recursivemutex, bslmt_mutex
//
//@DESCRIPTION: This component provides a mutually exclusive lock primitive
// ("mutex") by wrapping a suitable platform-specific mechanism.  The
// 'bslmt::Mutex' class provides the following operations: 'lock', 'tryLock',
// and 'unlock'.
//
// The behavior is undefined if 'unlock' is invoked on a 'bslmt::Mutex' object
// from a thread that did not successfully acquire the lock, or if 'lock' is
// called twice in a thread without calling 'unlock' in between (i.e.,
// 'bslmt::Mutex' is non-recursive).  In particular, 'lock' *may* or *may*
// *not* deadlock if the current thread holds the lock.
//
///Usage
///-----
// The following snippets of code illustrate the use of 'bslmt::Mutex' to write
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
//
//  // CREATORS
//  my_Account::my_Account()
//  : d_money(0.0)
//  {
//  }
//
//  my_Account::my_Account(const my_Account& original)
//  : d_money(original.d_money)
//  {
//  }
//
//  my_Account::~my_Account()
//  {
//  }
//
//  // MANIPULATORS
//  my_Account& my_Account::operator=(const my_Account& rhs)
//  {
//      d_money = rhs.d_money;
//      return *this;
//  }
//
//  void my_Account::deposit(double amount)
//  {
//      d_money += amount;
//  }
//
//  void my_Account::withdraw(double amount)
//  {
//      d_money -= amount;
//  }
//
//  // ACCESSORS
//  double my_Account::balance() const
//  {
//      return d_money;
//  }
//..
// Next, we use a 'bslmt::Mutex' object to render atomic the function calls of
// a new thread-safe class that uses the thread-unsafe class in its
// implementation.  Note the typical use of 'mutable' for the lock:
//..
//  class my_SafeAccountHandle {
//      // This 'class' provides a thread-safe handle to an account (held, not
//      // owned) passed at construction.
//
//      // DATA
//      my_Account          *d_account_p;  // held, not owned
//      mutable bslmt::Mutex  d_lock;       // guard access to 'd_account_p'
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
// The implementation show-casing the use of 'bslmt::Mutex' follows:
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
// See 'bslmt_lockguard' for more information:
//..
//      d_lock.lock();  // consider using 'bslmt::LockGuard'
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
//      d_lock.lock();  // consider using 'bslmt::LockGuard'
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
//      d_lock.lock();  // consider using 'bslmt::LockGuard'
//      const double res = d_account_p->balance();
//      d_lock.unlock();
//      return res;
//  }
//..
// The handle's atomic methods are used just as the corresponding methods in
// 'my_Account':
//..
//  my_Account account;
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
//  handle.lock();  // consider using 'bslmt::LockGuard'
//
//  double originalBalance = handle.account()->balance();
//  for (int i = 0; i < 5; ++i) {
//      handle.account()->deposit(check[i]);
//  }
//  assert(originalBalance + 349.95 == handle.account()->balance());
//  handle.unlock();
//..

#include <bslscm_version.h>

#include <bslmt_muteximpl_pthread.h>
#include <bslmt_muteximpl_win32.h>
#include <bslmt_platform.h>

namespace BloombergLP {
namespace bslmt {

template <class THREAD_POLICY>
class MutexImpl;

                               // ===========
                               // class Mutex
                               // ===========

class Mutex {
    // This 'class' implements a lightweight, portable wrapper of an OS-level
    // mutex lock to support intra-process synchronization.  The behavior is
    // undefined if the 'lock' method of this class is invoked more than once
    // on the same mutex object in the same thread without an intervening call
    // to 'unLock'.

    // DATA
    MutexImpl<Platform::ThreadPolicy> d_imp;  // platform-specific
                                              // implementation

    // NOT IMPLEMENTED
    Mutex(const Mutex&);
    Mutex& operator=(const Mutex&);

  public:
    // PUBLIC TYPES
    typedef MutexImpl<Platform::ThreadPolicy>::NativeType NativeType;
        // 'NativeType' is an alias for the underlying OS-level mutex type.  It
        // is exposed so that other 'bslmt' components can operate directly on
        // this mutex.

    // CREATORS
    Mutex();
        // Create a mutex object in the unlocked state.  This method does not
        // return normally unless there are sufficient system resources to
        // construct the object.

    ~Mutex();
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
        // object.  This method is intended only to support other 'bslmt'
        // components that must operate directly on this mutex.

    int tryLock();
        // Attempt to acquire a lock on this mutex object.  Return 0 on
        // success, and a non-zero value if this object is already locked by a
        // different thread.  The behavior is undefined if the calling thread
        // already owns the lock on this mutex, and may result in deadlock.

    void unlock();
        // Release a lock on this mutex that was previously acquired through a
        // call to 'lock', or a successful call to 'tryLock', enabling another
        // thread to acquire a lock on this mutex.  The behavior is undefined
        // unless the calling thread currently owns the lock on this mutex.
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                               // -----------
                               // class Mutex
                               // -----------

// CREATORS
inline
bslmt::Mutex::Mutex()
{
}

inline
bslmt::Mutex::~Mutex()
{
}

// MANIPULATORS
inline
void bslmt::Mutex::lock()
{
    d_imp.lock();
}

inline
bslmt::Mutex::NativeType& bslmt::Mutex::nativeMutex()
{
    return d_imp.nativeMutex();
}

inline
int bslmt::Mutex::tryLock()
{
    return d_imp.tryLock();
}

inline
void bslmt::Mutex::unlock()
{
    d_imp.unlock();
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
