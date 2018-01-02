// bslmt_readerwriterlock.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_READERWRITERLOCK
#define INCLUDED_BSLMT_READERWRITERLOCK

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a multi-reader/single-writer lock.
//
//@CLASSES:
//   bslmt::ReaderWriterLock: multi-reader/single-writer lock class
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines an efficient multi-reader/single-writer
// lock (RW-Lock) mechanism, 'bslmt::ReaderWriterLock'.  It is designed to
// allow concurrent *read* access to a shared resource while still controlling
// *write* access.
//
// RW-Locks are generally used for resources which are frequently read and less
// frequently updated.  Unlike other lock mechanisms (e.g.,"Mutexes"), RW-Locks
// provide two distinct but mutually exclusive lock states: a *read* *lock*
// state, and a *write* *lock* state.  Multiple callers can simultaneously
// acquire a *read* *lock*, but only one *write* *lock* may be active at any
// given time.
//
// This implementation gives preference to writers, which can lead to reader
// "starvation" in applications with continuous writes.
//
///Read and Write Locks
///--------------------
// If a *read* *lock* is attempted while another *read* *lock* is already
// active and there are no pending *write* *locks*, the lock will be
// immediately granted.  If there are pending or active *write* *locks*, the
// reader will block until all *write* *locks* (including those acquired after
// the reader) are released.
//
// 'bslmt::ReaderWriterLock' also supports atomic conversion from *read* to
// *write* *locks*.  This feature allows callers to first acquire a *read*
// *lock*, determine if a write operation needs to be performed, and
// conditionally upgrade to a *write* *lock* without possibility of another
// writer having changed the state of the resource.
//
// The component supports both optimistic and pessimistic lock conversions.
//
///Optimistic Lock Conversions
///- - - - - - - - - - - - - -
// Any basic *read* *lock* can be converted to a *write* *lock*, but the
// conversion is not guaranteed to be atomic.  If the conversion cannot be
// performed atomically, which means the lock was first released, then a lock
// for write was acquired again (possibly after other threads have obtained and
// released a write lock themselves), the state of the resource must be
// re-evaluated, since the resource may have been changed by another thread.
//
///Pessimistic Lock Conversions
/// - - - - - - - - - - - - - -
// For conditions with high probably for write contention, or where the cost of
// re-evaluating the update condition is too high, clients may choose to
// acquire a *read* *lock* that is guaranteed to upgrade atomically, that is
// without the possibility of another thread acquiring a read or write lock in
// the meantime.  The 'lockReadReserveWrite' method allows a caller to acquire
// a *read* *lock* and simultaneously reserve a *write* *lock*.  The *read*
// *lock* can then be atomically upgraded to the reserved *write* *lock* by
// calling the 'upgradeToWriteLock' method.
//
///Usage
///-----
// The following snippet of code demonstrates a typical use of a reader/writer
// lock.  The sample implements a simple cache mechanism for user information.
// We expect that the information is read very frequently, but only modified
// when a user "badges" in or out, which should be relatively infrequent.
//..
//  struct UserInfo{
//      long               d_UserId;
//      char               d_UserName[MAX_USER_NAME];
//      char               d_badge_location[MAX_BADGE_LOCATION];
//      int                d_inOutStatus;
//      bsls::TimeInterval d_badgeTime;
//  };
//
//  class UserInfoCache {
//      typedef bsl::map<int, UserInfo> InfoMap;
//
//      bslmt::ReaderWriterLock d_lock;
//      InfoMap                 d_infoMap;
//
//    public:
//      UserInfoCache();
//      ~UserInfoCache();
//
//      int getUserInfo(int userId, UserInfo *userInfo);
//      int updateUserInfo(int userId, UserInfo *userInfo);
//      int addUserInfo(int userId, UserInfo *userInfo);
//      void removeUser(int userId);
//  };
//
//  inline
//  UserInfoCache::UserInfoCache()
//  {
//  }
//
//  inline
//  UserInfoCache::~UserInfoCache()
//  {
//  }
//
//  inline
//  int UserInfoCache::getUserInfo(int userId, UserInfo *userInfo)
//  {
//      int ret = 1;
//..
// Getting the user info does not require any write access.  We do, however,
// need read access to 'd_infoMap', which is controlled by 'd_lock'.  (Note
// that writers *will* block until this *read* *lock* is released, but
// concurrent reads are allowed.)  The user info is copied into the
// caller-owned location 'userInfo'.
//..
//      d_lock.lockRead();
//      InfoMap::iterator it = d_infoMap.find(userId);
//      if (d_infoMap.end() != it) {
//          *userInfo = it->second;
//          ret = 0;
//      }
//      d_lock.unlock();
//      return ret;
//  }
//
//  inline
//  int UserInfoCache::updateUserInfo(int userId, UserInfo *userInfo)
//  {
//      int ret = 1;
//..
// Although we intend to update the information, we first acquire a *read*
// *lock* to locate the item.  This allows other threads to read the list while
// we find the item.  If we do not locate the item we can simply release the
// *read* *lock* and return an error without causing any other *reading* thread
// to block.  (Again, other writers *will* block until this *read* *lock* is
// released.)
//..
//      d_lock.lockRead();
//      InfoMap::iterator it = d_infoMap.find(userId);
//      if (d_infoMap.end() != it) {
//..
// Since 'it != end()', we found the item.  Now we need to upgrade to a *write*
// *lock*.  If we can't do this atomically, then we need to locate the item
// again.  This is because another thread may have changed 'd_infoMap' during
// the time between our *read* and *write* locks.
//..
//          if (d_lock.upgradeToWriteLock()) {
//              it = d_infoMap.find(userId);
//          }
//..
// This is a little more costly, but since we don't expect many concurrent
// writes, it should not happen often.  In the (likely) event that we do
// upgrade to a *write* *lock* atomically, then the second lookup above is not
// performed.  In any case, we can now update the information and release the
// lock, since we already have a pointer to the item and we know that the list
// could not have been changed by anyone else.
//..
//          if (d_infoMap.end() != it) {
//              it->second = *userInfo;
//              ret = 0;
//          }
//          d_lock.unlock();
//      }
//      else {
//          d_lock.unlock();
//      }
//      return ret;
//  }
//
//  inline
//  int UserInfoCache::addUserInfo(int userId, UserInfo *userInfo)
//  {
//      d_lock.lockRead();
//      bool found = !! d_infoMap.count(userId);
//      if (! found) {
//          if (d_lock.upgradeToWriteLock()) {
//              found = !! d_infoMap.count(userId);
//          }
//          if (! found) {
//              d_infoMap[userId] = *userInfo;
//          }
//          d_lock.unlock();
//      }
//      else {
//          d_lock.unlock();
//      }
//      return found ? 1 : 0;
//  }
//
//  inline
//  void UserInfoCache::removeUser(int userId)
//  {
//      d_lock.lockWrite();
//      d_infoMap.erase(userId);
//      d_lock.unlock();
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSLMT_CONDITION
#include <bslmt_condition.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_THREADUTIL
#include <bslmt_threadutil.h>
#endif

namespace BloombergLP {
namespace bslmt {

                          // ======================
                          // class ReaderWriterLock
                          // ======================

class ReaderWriterLock {
    // This class provides a multi-reader/single-writer lock mechanism.

    // PRIVATE TYPES
    enum SignalState {
        e_NOT_SIGNALED     = 0,
        e_WRITE_SIGNALED   = 1,
        e_UPGRADE_SIGNALED = 2
    };

    // CLASS DATA
    static const long long WRITER_MASK         = 0x000000000000FFFFLL;
        // mask for writer portion of the counter

    static const long long READER_MASK         = 0x00000000FFFF0000LL;
        // mask for active reader potion of the counter

    static const long long READER_INC          = 0x0000000000010000LL;
        // value used to increment and decrement the active reader count by 1

    static const long long BLOCKED_READER_MASK = 0x0000FFFF00000000LL;
        // mask for waiting reader portion of the counter

    static const long long BLOCKED_READER_INC = 0x00000000100000000LL;
        // value used to increment and decrement the blocked reader count by 1

    static const long long READ_OK             = 0x0001000000000000LL;
        // mask for read ok flag which indicates that readers can acquire a
        // lock without blocking.

    static const long long UPGRADE_PENDING     = 0x0002000000000000LL;
        // mask for upgrade pending flag which indicates that a reader thread
        // is waiting to upgrade to a writer

    static const long long RESERVATION_PENDING = 0x0004000000000000LL;
        // mask for reservation pending flag

    static const long long READ_BCAST_MASK     = 0xFFF0000000000000LL;
        // mask for the read broadcast state portion of counter

    static const long long READ_BCAST_INC      = 0x0010000000000000LL;
        // value used to increment the read broadcast count by 1

    // INSTANCE DATA
    bsls::AtomicOperations::AtomicTypes::Int64 d_rwCount;
                                     // atomic counter used to track active and
                                     // waiting read/write lock requests

    Mutex            d_mutex;        // used for access control

    Condition        d_readCond;     // used to signal waiting readers

    Condition        d_writeCond;    // used to signal waiting writers

    Condition        d_upgradeCond;  // used to signal upgraders

    ThreadUtil::Id   d_owner;        // id of thread that currently owns this
                                     // lock if it is in the write lock state,
                                     // or the id of the thread that holds the
                                     // write reservation if one exists

    volatile SignalState   d_signalState;

    volatile char          d_owned;

    // NOT IMPLEMENTED
    ReaderWriterLock(const ReaderWriterLock&);
    ReaderWriterLock& operator=(const ReaderWriterLock&);

  public:
    // CREATORS
    ReaderWriterLock();
        // Construct a reader/writer lock initialized to an unlocked state.

    ~ReaderWriterLock();
        // Destroy this reader/writer lock.

    // MANIPULATORS
    void lockRead();
        // Lock this reader/writer lock for read.  If there are no pending or
        // active write locks, the call will return immediately, otherwise it
        // block until all write locks have been released.  Use 'unlock' to
        // release the lock.

    void lockReadReserveWrite();
        // Lock this reader/writer lock for read and reserve a write lock so
        // that a call to 'upgradeToWriteLock' is guaranteed to upgrade
        // atomically.  If there are no pending or active write locks, the call
        // will return immediately, otherwise it will block until it all active
        // and pending writes have completed.  The lock may then be atomically
        // converted to a write lock by calling 'upgradeToWriteLock'.  Use
        // 'unlock' to release the lock.

    void lockWrite();
        // Lock the reader/writer lock for write.  The call will block until
        // all active read locks or active/pending write locks are released.
        // When the reader/writer lock is locked for write, all read/write lock
        // attempts will either fail or block until the lock is released.  Use
        // 'unlock' to release the lock.

    int tryLockRead();
        // Attempt to lock this reader/writer lock for read.  Return 0 on
        // success, and a non-zero value if the lock is currently locked for
        // write or if there are writers waiting for this lock.  If successful,
        // 'unlock' must be used to release this lock.

    int tryLockWrite();
        // Attempt to lock this reader/writer lock for write.  Return 0 on
        // success, and a non-zero value if the lock already locked for read or
        // write.  If successful, 'unlock' must be called to release the lock.

    int upgradeToReservedWriteLock();
        // *DEPRECATED* Use 'upgradeToWriteLock' instead.  Note that calls to
        // this function are simply forwarded to 'upgradeToWriteLock'.

    int upgradeToWriteLock();
        // Convert a read lock (acquired by a successful call to 'lockRead',
        // 'lockReadReserveWrite', or 'tryLockRead') to a write lock.  Return 0
        // if the upgrade operation was atomic, and a non-zero value otherwise.
        // If there are other active read locks, the call will block until all
        // current read locks are released.  Note that locks that were acquired
        // through 'lockReadReserveWrite' are guaranteed to upgrade atomically.
        // Use 'unlock' to release this lock.

    int tryUpgradeToWriteLock();
        // Attempt to atomically convert a read lock (acquired by a successful
        // call to 'lockRead', 'lockReadReserveWrite', or 'tryLockRead') to a
        // write lock.  Return 0 on success, and a non-zero value otherwise.
        // If a write lock request is already pending, a non-zero value is
        // immediately returned.  If there are other active read locks, the
        // call will block until all current read locks are released.  Note
        // that locks that were acquired through 'lockReadReserveWrite' are
        // guaranteed to succeed.

    void unlockRead();
        // *DEPRECATED* Use 'unlock' instead.  Note that calls to this function
        // are simply forwarded to 'unlock'.

    void unlockReadUnreserveWrite();
        // *DEPRECATED* Use 'unlock' instead.  Note that calls to this function
        // are simply forwarded to 'unlock'.

    void unlockWrite();
        // *DEPRECATED* Use 'unlock' instead.  Note that calls to this function
        // are simply forwarded to 'unlock'.

    void unlock();
        // Release a read lock that was previously acquired from a successful
        // call to 'lockRead', 'lockReadReserveWrite', or 'tryLockRead', or a
        // call to write lock which was previously acquired by a successful
        // call to 'lockWrite', 'tryLockWrite', or 'upgradeToWriteLock'.  Note
        // that the behavior is undefined unless the calling thread currently
        // owns this read/write lock.
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                          // ----------------------
                          // class ReaderWriterLock
                          // ----------------------

// CREATORS
inline
bslmt::ReaderWriterLock::ReaderWriterLock()
: d_signalState(e_NOT_SIGNALED)
, d_owned(0)
{
    bsls::AtomicOperations::initInt64(&d_rwCount, READ_OK);
}

inline
bslmt::ReaderWriterLock::~ReaderWriterLock()
{
}

// MANIPULATORS
inline
int bslmt::ReaderWriterLock::upgradeToReservedWriteLock()
{
    return upgradeToWriteLock();
}

inline
void bslmt::ReaderWriterLock::unlockRead()
{
    unlock();
}

inline
void bslmt::ReaderWriterLock::unlockReadUnreserveWrite()
{
    unlock();
}

inline
void bslmt::ReaderWriterLock::unlockWrite()
{
    unlock();
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
