// bcemt_readerwriterlock.cpp      -*-C++-*-
#include <bcemt_readerwriterlock.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_readerwriterlock_cpp,"$Id$ $CSID$")

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

#include <bcemt_barrier.h>    // for testing only
#include <bces_atomictypes.h> // for testing only

#include <bsl_map.h>          // for testing only

namespace BloombergLP {

#ifdef BSLS_PLATFORM__OS_AIX // IBM does not properly inline these consts
const long long bcemt_ReaderWriterLock::WRITER_MASK;
const long long bcemt_ReaderWriterLock::READER_MASK;
const long long bcemt_ReaderWriterLock::READER_INC;
const long long bcemt_ReaderWriterLock::BLOCKED_READER_MASK;
const long long bcemt_ReaderWriterLock::BLOCKED_READER_INC;
const long long bcemt_ReaderWriterLock::READ_OK;
const long long bcemt_ReaderWriterLock::UPGRADE_PENDING;
const long long bcemt_ReaderWriterLock::RESERVATION_PENDING;
const long long bcemt_ReaderWriterLock::READ_BCAST_MASK;
const long long bcemt_ReaderWriterLock::READ_BCAST_INC;

#endif

// MANIPULATORS
void bcemt_ReaderWriterLock::lockRead()
{
    int wait;
    bsls_PlatformUtil::Int64 rwcount;
    bsls_PlatformUtil::Int64 newrwcount=bces_AtomicUtil::getInt64(d_rwCount);

    do {
        rwcount = newrwcount;
        if (newrwcount & READ_OK) {
            wait = 0;
            newrwcount += READER_INC;
        } else {
            wait = 1;
            newrwcount += BLOCKED_READER_INC;
        }
        newrwcount = bces_AtomicUtil::testAndSwapInt64(&d_rwCount,
                                                       rwcount,
                                                       newrwcount);
    } while (newrwcount != rwcount);

    if (wait) {
        d_mutex.lock();
        do {
            rwcount = bces_AtomicUtil::getInt64(d_rwCount);
            if ((rwcount & READ_BCAST_MASK) != (newrwcount & READ_BCAST_MASK))
                break;
            d_readCond.wait(&d_mutex);
        } while (1);
        d_mutex.unlock();
    }
}

void bcemt_ReaderWriterLock::lockReadReserveWrite()
{
    int wait;
    bsls_PlatformUtil::Int64 rwcount;
    bsls_PlatformUtil::Int64 newrwcount=bces_AtomicUtil::getInt64(d_rwCount);

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
        newrwcount = bces_AtomicUtil::testAndSwapInt64( &d_rwCount,
                                                        rwcount,
                                                        newrwcount);
    } while(newrwcount != rwcount);

    if (wait) {
        d_mutex.lock();
        while (d_signalState != BCEMT_WRITE_SIGNALED) {
            d_writeCond.wait(&d_mutex);
        }
        d_signalState = BCEMT_NOT_SIGNALED;

        newrwcount = bces_AtomicUtil::getInt64(d_rwCount);
        do {
            rwcount = newrwcount;
            newrwcount = ((rwcount >> 16 ) & READER_MASK)
                             + ((rwcount | READ_OK | RESERVATION_PENDING)
                             & ~BLOCKED_READER_MASK) + READER_INC;
            if (newrwcount & READER_MASK) newrwcount += READ_BCAST_INC;
        } while ((newrwcount = bces_AtomicUtil::testAndSwapInt64(
                                                     &d_rwCount,
                                                     rwcount,
                                                     newrwcount)) != rwcount );
        d_mutex.unlock();
        if (newrwcount & BLOCKED_READER_MASK) d_readCond.broadcast();
    }

    if (d_owned) {
        d_owned = 0;
    }

    d_owner = bcemt_ThreadUtil::selfId();
    d_owned = 1;
}

void bcemt_ReaderWriterLock::lockWrite()
{
    bsls_PlatformUtil::Int64 rwcount;
    bsls_PlatformUtil::Int64 newrwcount=bces_AtomicUtil::getInt64(d_rwCount);
    int wait;

    do {
        rwcount = newrwcount;
        if (!(newrwcount & (WRITER_MASK| READER_MASK)))
            wait = 0;
        else wait = 1;
        newrwcount &= ~READ_OK;
        newrwcount++;
    } while ((newrwcount = bces_AtomicUtil::testAndSwapInt64(
                                                     &d_rwCount,
                                                     rwcount,
                                                     newrwcount)) != rwcount );

    if (wait) {
        d_mutex.lock();
        while (d_signalState != BCEMT_WRITE_SIGNALED) {
            d_writeCond.wait(&d_mutex);
        }
        d_signalState = BCEMT_NOT_SIGNALED;
        d_mutex.unlock();
    }
    d_owner = bcemt_ThreadUtil::selfId();
    d_owned = 1;
}

int bcemt_ReaderWriterLock::tryLockRead()
{
    bsls_PlatformUtil::Int64 rwcount;
    bsls_PlatformUtil::Int64 newrwcount=bces_AtomicUtil::getInt64(d_rwCount);
    do {
        rwcount = newrwcount;

        if (!(rwcount & READ_OK)) {
            return 1;
        }
        newrwcount += READER_INC;
        newrwcount = bces_AtomicUtil::testAndSwapInt64(&d_rwCount,
                                                       rwcount,
                                                       newrwcount);
    } while(rwcount != newrwcount);
    return 0;
}

int bcemt_ReaderWriterLock::tryLockWrite()
{
    bsls_PlatformUtil::Int64 rwcount;
    bsls_PlatformUtil::Int64 newrwcount;
    newrwcount = bces_AtomicUtil::getInt64(d_rwCount);
    do {
        rwcount = newrwcount;
        if (rwcount & (READER_MASK|WRITER_MASK)) {
            return 1;
        }
        newrwcount = 1;
    } while ((newrwcount = bces_AtomicUtil::testAndSwapInt64(&d_rwCount,
                                                                rwcount,
                                                             newrwcount))
             != rwcount );

    d_owner = bcemt_ThreadUtil::selfId();
    d_owned = 1;
    return 0;
}

int bcemt_ReaderWriterLock::upgradeToWriteLock()
{
    int atomic;
    int wait;
    bsls_PlatformUtil::Int64 rwcount;
    bsls_PlatformUtil::Int64 newrwcount=bces_AtomicUtil::getInt64(d_rwCount);
    bcemt_ThreadUtil::Id me = bcemt_ThreadUtil::selfId();
    bool mine = d_owned && bcemt_ThreadUtil::isEqualId(d_owner, me);

    do {
        rwcount = newrwcount;
        if (!(newrwcount & READER_MASK)) {
            // Invalid lock state for this operation
            return -1;
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
    } while ((newrwcount = bces_AtomicUtil::testAndSwapInt64(
                                                     &d_rwCount,
                                                     rwcount,
                                                     newrwcount)) != rwcount );
    if (wait) {
        d_mutex.lock();
        if (wait > 1) {
            // If we were the last reader and we could not transition
            // atomically, then there must be another trying
            // to upgrade atomically so signal it.
            d_signalState = BCEMT_UPGRADE_SIGNALED;
            d_upgradeCond.signal();
        }

        if (atomic) {
            while (d_signalState != BCEMT_UPGRADE_SIGNALED) {
                d_upgradeCond.wait(&d_mutex);
            }
            d_signalState = BCEMT_NOT_SIGNALED;
        }
        else {
            while (d_signalState != BCEMT_WRITE_SIGNALED) {
                d_writeCond.wait(&d_mutex);
            }
            d_signalState = BCEMT_NOT_SIGNALED;
        }
        d_mutex.unlock();
    }

    if (!mine) {
        d_owner = me;
        d_owned = 1;
    }

    return atomic ? 0 : 1;
}

int bcemt_ReaderWriterLock::tryUpgradeToWriteLock()
{
    int wait;
    bsls_PlatformUtil::Int64 rwcount;
    bsls_PlatformUtil::Int64 newrwcount=bces_AtomicUtil::getInt64(d_rwCount);
    bcemt_ThreadUtil::Id me = bcemt_ThreadUtil::selfId();
    bool mine = d_owned && bcemt_ThreadUtil::isEqualId(d_owner, me);

    do {
        rwcount = newrwcount;
        if (!(newrwcount & READER_MASK)) {
            // Invalid lock state for this operation
            return -1;
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
            } else return 1;
        }
        else {
            newrwcount = ((rwcount - READER_INC) + 1) & ~READ_OK;
            if (newrwcount & READER_MASK) {
                if (newrwcount & UPGRADE_PENDING) {
                    return 1;
                }
                newrwcount |= UPGRADE_PENDING;
                wait = 1;
            } else {
                if (newrwcount & UPGRADE_PENDING) {
                    return 1;
                }
                else wait = 0;
            }
        }
    } while ((newrwcount = bces_AtomicUtil::testAndSwapInt64(
                                                      &d_rwCount,
                                                      rwcount,
                                                      newrwcount)) != rwcount);
    if (wait) {
        d_mutex.lock();
        while (d_signalState != BCEMT_UPGRADE_SIGNALED) {
            d_upgradeCond.wait(&d_mutex);
        }
        d_signalState = BCEMT_NOT_SIGNALED;
        d_mutex.unlock();
    }

    if (!mine) {
        d_owner = me;
        d_owned = 1;
    }

    return 0;
}

void bcemt_ReaderWriterLock::unlock()
{
    enum {SIG_NONE = 0, SIG_READ = 1, SIG_WRITE=2, SIG_UPGRADE=3 };

    bsls_PlatformUtil::Int64 rwcount;
    bsls_PlatformUtil::Int64 newrwcount=bces_AtomicUtil::getInt64(d_rwCount);
    bool mine = d_owned && bcemt_ThreadUtil::isEqualId(d_owner,
                                                   bcemt_ThreadUtil::selfId());
    int sigType;

    if (mine) d_owned = 0;

    do {
        sigType=SIG_NONE;
        rwcount = newrwcount;

        if (newrwcount & READER_MASK) {
            // Reader mask is set, either we have a read lock, or a
            // read lock with reservation that was never cashed in.
            newrwcount -= READER_INC;

            if (mine && (newrwcount & RESERVATION_PENDING))
                newrwcount -= (RESERVATION_PENDING | 1);

            if (!(newrwcount & READER_MASK) && (newrwcount & WRITER_MASK)) {
                sigType = (newrwcount & UPGRADE_PENDING) ?
                          SIG_UPGRADE : SIG_WRITE;
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
                    sigType = SIG_READ;
                }
            }
            else sigType = SIG_WRITE;
        }
        else break;
    } while ((newrwcount = bces_AtomicUtil::testAndSwapInt64(
                                                      &d_rwCount,
                                                      rwcount,
                                                      newrwcount)) != rwcount);

    if (sigType) {
        d_mutex.lock();
        switch(sigType) {
          case SIG_WRITE: {
            d_signalState = BCEMT_WRITE_SIGNALED;
            d_writeCond.signal();
          } break;
          case SIG_READ: {
            d_readCond.broadcast();
          } break;
          case SIG_UPGRADE: {
            d_signalState = BCEMT_UPGRADE_SIGNALED;
            d_upgradeCond.signal();
          } break;
        }
        d_mutex.unlock();
    }
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
