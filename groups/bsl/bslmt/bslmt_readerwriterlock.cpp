// bslmt_readerwriterlock.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_readerwriterlock.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_readerwriterlock_cpp,"$Id$ $CSID$")

///Implementation Notes
///--------------------
// The private read/write atomic counter has the following structure:
//..
// [51-61][50][49][48][32-47][16-31][0-15]
//  |      |   |    |     |      |     +- Active/Waiting writers
//  |      |   |    |     |      +------- Active readers
//  |      |   |    |     +-------------- Waiting readers
//  |      |   |    +-------------------- Read OK
//  |      |   +------------------------- Upgrade Pending
//  |      +----------------------------- Reservation Pending
//  +------------------------------------ Read broadcast count
//..

#include <bslmt_barrier.h>    // for testing only
#include <bsls_atomic.h> // for testing only

#include <bsls_types.h>

#include <bsl_map.h>          // for testing only

namespace BloombergLP {

#ifdef BSLS_PLATFORM_OS_AIX // IBM does not properly inline these constants
const long long bslmt::ReaderWriterLock::WRITER_MASK;
const long long bslmt::ReaderWriterLock::READER_MASK;
const long long bslmt::ReaderWriterLock::READER_INC;
const long long bslmt::ReaderWriterLock::BLOCKED_READER_MASK;
const long long bslmt::ReaderWriterLock::BLOCKED_READER_INC;
const long long bslmt::ReaderWriterLock::READ_OK;
const long long bslmt::ReaderWriterLock::UPGRADE_PENDING;
const long long bslmt::ReaderWriterLock::RESERVATION_PENDING;
const long long bslmt::ReaderWriterLock::READ_BCAST_MASK;
const long long bslmt::ReaderWriterLock::READ_BCAST_INC;

#endif

// MANIPULATORS
void bslmt::ReaderWriterLock::lockRead()
{
    int wait;
    bsls::Types::Int64 rwcount;
    bsls::Types::Int64 newrwcount=bsls::AtomicOperations::getInt64(&d_rwCount);

    do {
        rwcount = newrwcount;
        if (newrwcount & READ_OK) {
            wait = 0;
            newrwcount += READER_INC;
        } else {
            wait = 1;
            newrwcount += BLOCKED_READER_INC;
        }
        newrwcount = bsls::AtomicOperations::testAndSwapInt64(&d_rwCount,
                                                       rwcount,
                                                       newrwcount);
    } while (newrwcount != rwcount);

    if (wait) {
        d_mutex.lock();
        do {
            rwcount = bsls::AtomicOperations::getInt64(&d_rwCount);
            if ((rwcount & READ_BCAST_MASK) != (newrwcount & READ_BCAST_MASK))
                break;
            d_readCond.wait(&d_mutex);
        } while (1);
        d_mutex.unlock();
    }
}

void bslmt::ReaderWriterLock::lockReadReserveWrite()
{
    int wait;
    bsls::Types::Int64 rwcount;
    bsls::Types::Int64 newrwcount=bsls::AtomicOperations::getInt64(&d_rwCount);

    do {
        rwcount = newrwcount;
        if (newrwcount & WRITER_MASK) {
            wait = 1;
            ++newrwcount;
        }
        else {
            wait = 0;
            newrwcount += READER_INC + 1;
            newrwcount |= RESERVATION_PENDING;
        }
        if ((newrwcount & WRITER_MASK) > 16) {
            wait = 1;
        }
        newrwcount = bsls::AtomicOperations::testAndSwapInt64( &d_rwCount,
                                                        rwcount,
                                                        newrwcount);
    } while(newrwcount != rwcount);

    if (wait) {
        d_mutex.lock();
        while (d_signalState != e_WRITE_SIGNALED) {
            d_writeCond.wait(&d_mutex);
        }
        d_signalState = e_NOT_SIGNALED;

        newrwcount = bsls::AtomicOperations::getInt64(&d_rwCount);
        do {
            rwcount = newrwcount;
            newrwcount = ((rwcount >> 16 ) & READER_MASK)
                             + ((rwcount | READ_OK | RESERVATION_PENDING)
                             & ~BLOCKED_READER_MASK) + READER_INC;
            if (newrwcount & READER_MASK) newrwcount += READ_BCAST_INC;
        } while ((newrwcount = bsls::AtomicOperations::testAndSwapInt64(
                                                     &d_rwCount,
                                                     rwcount,
                                                     newrwcount)) != rwcount );
        d_mutex.unlock();
        if (newrwcount & BLOCKED_READER_MASK) d_readCond.broadcast();
    }

    if (d_owned) {
        d_owned = 0;
    }

    d_owner = ThreadUtil::selfId();
    d_owned = 1;
}

void bslmt::ReaderWriterLock::lockWrite()
{
    bsls::Types::Int64 rwcount;
    bsls::Types::Int64 newrwcount=bsls::AtomicOperations::getInt64(&d_rwCount);
    int wait;

    do {
        rwcount = newrwcount;
        if (!(newrwcount & (WRITER_MASK| READER_MASK)))
            wait = 0;
        else wait = 1;
        newrwcount &= ~READ_OK;
        newrwcount++;
    } while ((newrwcount = bsls::AtomicOperations::testAndSwapInt64(
                                                     &d_rwCount,
                                                     rwcount,
                                                     newrwcount)) != rwcount );

    if (wait) {
        d_mutex.lock();
        while (d_signalState != e_WRITE_SIGNALED) {
            d_writeCond.wait(&d_mutex);
        }
        d_signalState = e_NOT_SIGNALED;
        d_mutex.unlock();
    }
    d_owner = ThreadUtil::selfId();
    d_owned = 1;
}

int bslmt::ReaderWriterLock::tryLockRead()
{
    bsls::Types::Int64 rwcount;
    bsls::Types::Int64 newrwcount=bsls::AtomicOperations::getInt64(&d_rwCount);
    do {
        rwcount = newrwcount;

        if (!(rwcount & READ_OK)) {
            return 1;                                                 // RETURN
        }
        newrwcount += READER_INC;
        newrwcount = bsls::AtomicOperations::testAndSwapInt64(&d_rwCount,
                                                       rwcount,
                                                       newrwcount);
    } while(rwcount != newrwcount);
    return 0;
}

int bslmt::ReaderWriterLock::tryLockWrite()
{
    bsls::Types::Int64 rwcount;
    bsls::Types::Int64 newrwcount;
    newrwcount = bsls::AtomicOperations::getInt64(&d_rwCount);
    do {
        rwcount = newrwcount;
        if (rwcount & (READER_MASK|WRITER_MASK)) {
            return 1;                                                 // RETURN
        }
        newrwcount = 1;
    } while ((newrwcount = bsls::AtomicOperations::testAndSwapInt64(&d_rwCount,
                                                                rwcount,
                                                             newrwcount))
             != rwcount );

    d_owner = ThreadUtil::selfId();
    d_owned = 1;
    return 0;
}

int bslmt::ReaderWriterLock::upgradeToWriteLock()
{
    int atomic;
    int wait;
    bsls::Types::Int64 rwcount;
    bsls::Types::Int64 newrwcount=bsls::AtomicOperations::getInt64(&d_rwCount);
    ThreadUtil::Id me = ThreadUtil::selfId();
    bool mine = d_owned && ThreadUtil::isEqualId(d_owner, me);

    do {
        rwcount = newrwcount;
        if (!(newrwcount & READER_MASK)) {
            // Invalid lock state for this operation
            return -1;                                                // RETURN
        }

        if (newrwcount & RESERVATION_PENDING) {
            if (mine) {
                // I own a reservation on this lock
                newrwcount = (rwcount - READER_INC) &
                    ~(READ_OK|RESERVATION_PENDING);
                atomic = 1;
                if (newrwcount & READER_MASK) {
                    newrwcount |= UPGRADE_PENDING;
                    wait = 1;
                } else wait = 0;
            } else {
                newrwcount = ((rwcount - READER_INC) + 1) & ~READ_OK;
                atomic = 0;
                wait = 1;
            }
        }
        else {
            newrwcount = ((rwcount - READER_INC) + 1) & ~READ_OK;
            if (newrwcount & READER_MASK) {
                if (!(newrwcount & UPGRADE_PENDING)){
                    atomic = 1;
                    newrwcount |= UPGRADE_PENDING;
                }
                else {
                    atomic = 0;
                }
                wait = 1;
            } else {
                if (newrwcount & UPGRADE_PENDING) {
                    newrwcount ^= UPGRADE_PENDING;
                    wait = 2;
                    atomic = 0;
                }else {
                    wait = 0;
                    atomic = 1;
                }
            }
        }
    } while ((newrwcount = bsls::AtomicOperations::testAndSwapInt64(
                                                     &d_rwCount,
                                                     rwcount,
                                                     newrwcount)) != rwcount );
    if (wait) {
        d_mutex.lock();
        if (wait > 1) {
            // If we were the last reader and we could not transition
            // atomically, then there must be another trying to upgrade
            // atomically so signal it.
            d_signalState = e_UPGRADE_SIGNALED;
            d_upgradeCond.signal();
        }

        if (atomic) {
            while (d_signalState != e_UPGRADE_SIGNALED) {
                d_upgradeCond.wait(&d_mutex);
            }
            d_signalState = e_NOT_SIGNALED;
        }
        else {
            while (d_signalState != e_WRITE_SIGNALED) {
                d_writeCond.wait(&d_mutex);
            }
            d_signalState = e_NOT_SIGNALED;
        }
        d_mutex.unlock();
    }

    if (!mine) {
        d_owner = me;
        d_owned = 1;
    }

    return atomic ? 0 : 1;
}

int bslmt::ReaderWriterLock::tryUpgradeToWriteLock()
{
    int wait;
    bsls::Types::Int64 rwcount;
    bsls::Types::Int64 newrwcount=bsls::AtomicOperations::getInt64(&d_rwCount);
    ThreadUtil::Id me = ThreadUtil::selfId();
    bool mine = d_owned && ThreadUtil::isEqualId(d_owner, me);

    do {
        rwcount = newrwcount;
        if (!(newrwcount & READER_MASK)) {
            // Invalid lock state for this operation
            return -1;                                                // RETURN
        }

        if (newrwcount & RESERVATION_PENDING) {
            if (mine) {
                // I own a reservation on this lock
                newrwcount = (rwcount - READER_INC) &
                    ~(READ_OK|RESERVATION_PENDING);
                if (newrwcount & READER_MASK) {
                    newrwcount |= UPGRADE_PENDING;
                    wait = 1;
                } else wait = 0;
            } else return 1;                                          // RETURN
        }
        else {
            newrwcount = ((rwcount - READER_INC) + 1) & ~READ_OK;
            if (newrwcount & READER_MASK) {
                if (newrwcount & UPGRADE_PENDING) {
                    return 1;                                         // RETURN
                }
                newrwcount |= UPGRADE_PENDING;
                wait = 1;
            } else {
                if (newrwcount & UPGRADE_PENDING) {
                    return 1;                                         // RETURN
                }
                else wait = 0;
            }
        }
    } while ((newrwcount = bsls::AtomicOperations::testAndSwapInt64(
                                                      &d_rwCount,
                                                      rwcount,
                                                      newrwcount)) != rwcount);
    if (wait) {
        d_mutex.lock();
        while (d_signalState != e_UPGRADE_SIGNALED) {
            d_upgradeCond.wait(&d_mutex);
        }
        d_signalState = e_NOT_SIGNALED;
        d_mutex.unlock();
    }

    if (!mine) {
        d_owner = me;
        d_owned = 1;
    }

    return 0;
}

void bslmt::ReaderWriterLock::unlock()
{
    enum {  e_SIG_NONE = 0, e_SIG_READ = 1, e_SIG_WRITE=2, e_SIG_UPGRADE=3  };

    bsls::Types::Int64 rwcount;
    bsls::Types::Int64 newrwcount=bsls::AtomicOperations::getInt64(&d_rwCount);
    bool mine = d_owned && ThreadUtil::isEqualId(d_owner,
                                                   ThreadUtil::selfId());
    int sigType;

    if (mine) d_owned = 0;

    do {
        sigType=e_SIG_NONE;
        rwcount = newrwcount;

        if (newrwcount & READER_MASK) {
            // Reader mask is set, either we have a read lock, or a read lock
            // with reservation that was never cashed in.
            newrwcount -= READER_INC;

            if (mine && (newrwcount & RESERVATION_PENDING))
                newrwcount -= (RESERVATION_PENDING | 1);

            if (!(newrwcount & READER_MASK) && (newrwcount & WRITER_MASK)) {
                sigType = (newrwcount & UPGRADE_PENDING) ?
                          e_SIG_UPGRADE : e_SIG_WRITE;
                newrwcount &= ~(UPGRADE_PENDING|READ_OK);
            }
        }
        else if (mine && (rwcount & WRITER_MASK)) {
            --newrwcount;
            if (!(newrwcount & WRITER_MASK)) {
                newrwcount = ((newrwcount & BLOCKED_READER_MASK) >> 16) +
                    ((newrwcount | READ_OK) & ~BLOCKED_READER_MASK);
                if (newrwcount & READER_MASK) {
                    newrwcount += READ_BCAST_INC;
                    sigType = e_SIG_READ;
                }
            }
            else sigType = e_SIG_WRITE;
        }
        else break;
    } while ((newrwcount = bsls::AtomicOperations::testAndSwapInt64(
                                                      &d_rwCount,
                                                      rwcount,
                                                      newrwcount)) != rwcount);

    if (sigType) {
        d_mutex.lock();
        switch(sigType) {
          case e_SIG_WRITE: {
            d_signalState = e_WRITE_SIGNALED;
            d_writeCond.signal();
          } break;
          case e_SIG_READ: {
            d_readCond.broadcast();
          } break;
          case e_SIG_UPGRADE: {
            d_signalState = e_UPGRADE_SIGNALED;
            d_upgradeCond.signal();
          } break;
        }
        d_mutex.unlock();
    }
}

}  // close enterprise namespace

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
