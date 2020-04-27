// bslmt_readerwritermutex.h                                          -*-C++-*-

#ifndef INCLUDED_BSLMT_READERWRITERMUTEX
#define INCLUDED_BSLMT_READERWRITERMUTEX

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a multi-reader/single-writer lock.
//
//@CLASSES:
//   bslmt::ReaderWriterMutex: multi-reader/single-writer lock class
//
//@SEE_ALSO: bslmt_readerwriterlock, bslmt_readlockguard,
//           bslmt_writelockguard, bslmt_readerwriterlockassert
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
// To the extent the implementation's underlying mutex prevents a thread from
// starving, readers can not be starved by writers and writers can not be
// starved by readers.  If the underlying mutex, to some extent, favors
// re-acquisition of the mutex to allowing a new thread to obtain the mutex
// (e.g., the mutex obtained on Linux), this reader-writer lock is writer
// biased since writers can re-acquire the lock in the presence of readers but
// readers will not be able to re-acquire the lock in the presence of writers.
//
///'bslmt' Read/Write Locking Components
///- - - - - - - - - - - - - - - - - - -
//: o 'bslmt::ReaderWriterMutex' (defined in this component).  Preferred for
//:   most use-cases, has been shown to be faster than
//:   'bslmt::ReaderWriterLock' under most conditions and is generally the best
//:   choice.
//:
//: o 'bslmt::ReaderWriterLock': Preferred only when very long hold times are
//:   anticipated.  It also provides 'upgrade*' methods from a locked-for-read
//:   state to a locked-for-write state, but the use of this feature is
//:   discouraged as it has performed poorly on benchmarks.
//:
//: o 'bslmt::RWMutex': Deprecated.
//
// Note that for extremely short hold times and very high concurrency, a
// 'bslmt::Mutex' might outperform all of the above.
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
//      bsls::Types::Uint64               d_pennies;  // amount of money in the
//                                                    // account
//
//      mutable bslmt::ReaderWriterMutex  d_lock;     // guard access to
//                                                    // 'd_account_p'
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
//      void deposit(bsls::Types::Uint64 pennies);
//          // Atomically deposit the specified 'pennies' into this account.
//          // Note that this operation is thread-safe; no 'lock' is needed.
//
//      int withdraw(bsls::Types::Uint64 pennies);
//          // Attempt to atomically withdraw the specified 'pennies' from this
//          // account.  Return 0 on success and update this account to reflect
//          // the withdrawal.  Otherwise, return a non-zero value and do not
//          // update the balance of this account.  Note that this operation is
//          // thread-safe; no 'lock' is needed.
//
//      // ACCESSORS
//      bsls::Types::Uint64 balanceInPennies() const;
//          // Atomically return the number of pennies that are available for
//          // withdrawal from this account.
//  };
//
//  // CREATORS
//  my_Account::my_Account()
//  : d_pennies(0)
//  {
//  }
//
//  my_Account::my_Account(const my_Account& original)
//  : d_pennies(original.balanceInPennies())
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
//      d_pennies = rhs.balanceInPennies();
//      d_lock.unlockWrite();
//      return *this;
//  }
//
//  void my_Account::deposit(bsls::Types::Uint64 pennies)
//  {
//      d_lock.lockWrite();
//      d_pennies += pennies;
//      d_lock.unlockWrite();
//  }
//
//  int my_Account::withdraw(bsls::Types::Uint64 pennies)
//  {
//      int rv = 0;
//
//      d_lock.lockWrite();
//
//      if (pennies <= d_pennies) {
//          d_pennies -= pennies;
//      }
//      else {
//          rv = 1;
//      }
//
//      d_lock.unlockWrite();
//
//      return rv;
//  }
//
//  // ACCESSORS
//  bsls::Types::Uint64 my_Account::balanceInPennies() const
//  {
//      d_lock.lockRead();
//      bsls::Types::Uint64 rv = d_pennies;
//      d_lock.unlockRead();
//      return rv;
//  }
//..
// The atomic 'my_Account' methods are used as expected:
//..
//  my_Account account;
//
//  account.deposit(10050);
//  assert(10050 == account.balanceInPennies());
//
//  bsls::Types::Uint64 paycheckInPennies = 5025;
//
//  account.deposit(paycheckInPennies);
//  assert(15075 == account.balanceInPennies());
//..

#include <bslscm_version.h>

#include <bslmt_mutex.h>
#include <bslmt_readerwritermuteximpl.h>
#include <bslmt_semaphore.h>

#include <bsls_atomicoperations.h>

namespace BloombergLP {
namespace bslmt {

                         // =======================
                         // class ReaderWriterMutex
                         // =======================

class ReaderWriterMutex {
    // This class provides a multi-reader/single-writer lock mechanism.

    // DATA
    ReaderWriterMutexImpl<bsls::AtomicOperations, Mutex, Semaphore> d_impl;

  private:
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

    // ACCESSORS
    bool isLocked() const;
        // Return 'true' if this reader-write mutex is currently read locked or
        // write locked, and 'false' otherwise.

    bool isLockedRead() const;
        // Return 'true' if this reader-write mutex is currently read locked,
        // and 'false' otherwise.

    bool isLockedWrite() const;
        // Return 'true' if this reader-write mutex is currently write locked,
        // and 'false' otherwise.
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

// ACCESSORS
inline
bool bslmt::ReaderWriterMutex::isLocked() const
{
    return d_impl.isLocked();
}

inline
bool bslmt::ReaderWriterMutex::isLockedRead() const
{
    return d_impl.isLockedRead();
}

inline
bool bslmt::ReaderWriterMutex::isLockedWrite() const
{
    return d_impl.isLockedWrite();
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
