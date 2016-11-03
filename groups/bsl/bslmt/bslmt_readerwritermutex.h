// bslmt_readerwritermutex.h                                          -*-C++-*-

#ifndef INCLUDED_BSLMT_READERWRITERMUTEX
#define INCLUDED_BSLMT_READERWRITERMUTEX

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a multi-reader/single-writer lock.
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn4)
//
//@CLASSES:
//   bslmt::ReaderWriterMutex: multi-reader/single-writer lock class
//
//@SEE_ALSO: bslmt_readerwriterlock
//
//@DESCRIPTION: This component defines an efficient multi-reader/single-writer
// lock mechanism, 'bslmt::ReaderWriterMutex'.  It is designed to allow
// concurrent *read* access to a shared resource while still controlling
// *write* access.
//
// Reader-writer locks are generally used for resources that are frequently
// read and less frequently updated.  Unlike other lock mechanisms (e.g.,
// "mutexes"), reader-writer locks provide two distinct but mutually exclusive
// lock states: a *read* *lock* state, and a *write* *lock* state.
//
// This implementation is writer biased but, to the extent the implementation's
// underlying mutex prevents a thread from starving, readers can not be starved
// by writers.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Maintaining an Account Balance
///- - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate the use of
// 'bslmt::ReaderWriterMutex' to write a thread-safe class, 'my_Account'.  Note
// the typical use of 'mutable' for the lock:
//..
//  class my_Account {
//      // This 'class' represents a bank account with a single balance.
//
//      // DATA
//      double                            d_money;  // amount of money in the
//                                                  // account
//
//      mutable bslmt::ReaderWriterMutex  d_lock;   // guard access to
//                                                  // 'd_account_p'
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
//          // Atomically assign to this account the value of the specified
//          // 'rhs' account, and return a reference to this modifiable
//          // account.  Note that this operation is thread-safe; no 'lock' is
//          // needed.
//
//      void deposit(double amount);
//          // Atomically deposit the specified 'amount' of money into this
//          // account.  Note that this operation is thread-safe; no 'lock' is
//          // needed.
//
//      void withdraw(double amount);
//          // Atomically withdraw the specified 'amount' of money from this
//          // account.  Note that this operation is thread-safe; no 'lock' is
//          // needed.
//
//      // ACCESSORS
//      double balance() const;
//          // Atomically return the amount of money that is available for
//          // withdrawal from this account.
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
//..
// Where appropriate, clients should use a lock-guard to ensure that an
// acquired mutex is always properly released, even if an exception is thrown.
//..
//      d_lock.lockWrite();
//      d_money = rhs.d_money;
//      d_lock.unlockWrite();
//      return *this;
//  }
//
//  void my_Account::deposit(double amount)
//  {
//      d_lock.lockWrite();
//      d_money += amount;
//      d_lock.unlockWrite();
//  }
//
//  void my_Account::withdraw(double amount)
//  {
//      d_lock.lockWrite();
//      d_money -= amount;
//      d_lock.unlockWrite();
//  }
//
//  // ACCESSORS
//  double my_Account::balance() const
//  {
//      d_lock.lockRead();
//      double rv = d_money;
//      d_lock.unlockRead();
//      return rv;
//  }
//..
// The atomic 'my_Account' methods are used as expected:
//..
//  my_Account account;
//
//  account.deposit(100.50);
//  assert(100.50 == account.balance());
//
//  double paycheck = 50.25;
//
//  account.deposit(paycheck);
//  assert(150.75 == account.balance());
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_READERWRITERMUTEXIMPL
#include <bslmt_readerwritermuteximpl.h>
#endif

#ifndef INCLUDED_BSLMT_SEMAPHORE
#include <bslmt_semaphore.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

namespace BloombergLP {
namespace bslmt {

                         // =======================
                         // class ReaderWriterMutex
                         // =======================

class ReaderWriterMutex {
    // This class provides a multi-reader/single-writer lock mechanism.

    // DATA
    ReaderWriterMutexImpl<bsls::AtomicOperations, Mutex, Semaphore> d_impl;

    // NOT IMPLEMENTED
    ReaderWriterMutex(const ReaderWriterMutex&);
    ReaderWriterMutex& operator=(const ReaderWriterMutex&);

  public:
    // CREATORS
    ReaderWriterMutex();
        // Construct a reader/writer lock initialized to an unlocked state.

    //! ~ReaderWriterMutex();
        // Destroy this object

    // MANIPULATORS
    void lockRead();
        // Lock this reader-writer mutex for reading.  If there are no active
        // or pending write locks, lock this mutex for reading and return
        // immediately.  Otherwise, block until the read lock on this mutex is
        // acquired.  Use 'unlockRead' or 'unlock' to release the lock on this
        // mutex.  The behavior is undefined if this method is called from a
        // thread that already has a lock on this mutex.

    void lockWrite();
        // Lock this reader-writer mutex for writing.  If there are no active
        // or pending locks on this mutex, lock this mutex for writing and
        // return immediately.  Otherwise, block until the write lock on this
        // mutex is acquired.  Use 'unlockWrite' or 'unlock' to release the
        // lock on this mutex.  The behavior is undefined if this method is
        // called from a thread that already has a lock on this mutex.

    int tryLockRead();
        // Attempt to lock this reader-writer mutex for reading.  Immediately
        // return 0 on success, and a non-zero value if there are active or
        // pending writers.  If successful, 'unlockRead' or 'unlock' must be
        // used to release the lock on this mutex.  The behavior is undefined
        // if this method is called from a thread that already has a lock on
        // this mutex.

    int tryLockWrite();
        // Attempt to lock this reader-writer mutex for writing.  Immediately
        // return 0 on success, and a non-zero value if there are active or
        // pending locks on this mutex.  If successful, 'unlockWrite' or
        // 'unlock' must be used to release the lock on this mutex.  The
        // behavior is undefined if this method is called from a thread that
        // already has a lock on this mutex.

    void unlock();
        // Release the lock that the calling thread holds on this reader-writer
        // mutex.  The behavior is undefined unless the calling thread
        // currently has a lock on this mutex.

    void unlockRead();
        // Release the read lock that the calling thread holds on this
        // reader-writer mutex.  The behavior is undefined unless the calling
        // thread currently has a read lock on this mutex.

    void unlockWrite();
        // Release the write lock that the calling thread holds on this
        // reader-writer mutex.  The behavior is undefined unless the calling
        // thread currently has a write lock on this mutex.
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                         // -----------------------
                         // class ReaderWriterMutex
                         // -----------------------

// CREATORS
inline
bslmt::ReaderWriterMutex::ReaderWriterMutex()
{
}

// MANIPULATORS
inline
void bslmt::ReaderWriterMutex::lockRead()
{
    d_impl.lockRead();
}

inline
void bslmt::ReaderWriterMutex::lockWrite()
{
    d_impl.lockWrite();
}

inline
int bslmt::ReaderWriterMutex::tryLockRead()
{
    return d_impl.tryLockRead();
}

inline
int bslmt::ReaderWriterMutex::tryLockWrite()
{
    return d_impl.tryLockWrite();
}

inline
void bslmt::ReaderWriterMutex::unlock()
{
    d_impl.unlock();
}

inline
void bslmt::ReaderWriterMutex::unlockRead()
{
    d_impl.unlockRead();
}

inline
void bslmt::ReaderWriterMutex::unlockWrite()
{
    d_impl.unlockWrite();
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
