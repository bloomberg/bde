// bslmt_readerwritermuteximpl.h                                      -*-C++-*-

#ifndef INCLUDED_BSLMT_READERWRITERMUTEXIMPL
#define INCLUDED_BSLMT_READERWRITERMUTEXIMPL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a multi-reader/single-writer lock.
//
//@CLASSES:
//   bslmt::ReaderWriterMutexImpl: multi-reader/single-writer lock class
//
//@SEE_ALSO: bslmt_readerwriterlock
//
//@DESCRIPTION: This component defines an efficient multi-reader/single-writer
// lock mechanism, 'bslmt::ReaderWriterMutexImpl'.  It is designed to allow
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
///Usage
///-----
// There is no usage example for this component since it is not meant for
// direct client use.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bslmt {

                       // ===========================
                       // class ReaderWriterMutexImpl
                       // ===========================

template <class ATOMIC_OP, class MUTEX, class SEMAPHORE>
class ReaderWriterMutexImpl {
    // This class provides a multi-reader/single-writer lock mechanism.

    // CLASS DATA
    static const bsls::Types::Int64 k_READER_MASK       = 0x00000000ffffffffLL;
    static const bsls::Types::Int64 k_READER_INC        = 0x0000000000000001LL;
    static const bsls::Types::Int64 k_PENDING_WRITER_MASK
                                                        = 0x0fffffff00000000LL;
    static const bsls::Types::Int64 k_PENDING_WRITER_INC
                                                        = 0x0000000100000000LL;
    static const bsls::Types::Int64 k_WRITER            = 0x1000000000000000LL;

    // DATA
    bsls::AtomicOperations::AtomicTypes::Int64 d_state;      // atomic value
                                                             // used to track
                                                             // the state of
                                                             // this mutex

    MUTEX                                      d_mutex;      // primary access
                                                             // control

    SEMAPHORE                                  d_semaphore;  // used to capture
                                                             // writers
                                                             // released from
                                                             // 'd_mutex' but
                                                             // must wait for
                                                             // readers to
                                                             // finish

    // NOT IMPLEMENTED
    ReaderWriterMutexImpl(const ReaderWriterMutexImpl&);
    ReaderWriterMutexImpl& operator=(const ReaderWriterMutexImpl&);

  public:
    // CREATORS
    ReaderWriterMutexImpl();
        // Construct a reader/writer lock initialized to an unlocked state.

    //! ~ReaderWriterMutexImpl();
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

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                       // ---------------------------
                       // class ReaderWriterMutexImpl
                       // ---------------------------

// CREATORS
template <class ATOMIC_OP, class MUTEX, class SEMAPHORE>
inline
ReaderWriterMutexImpl<ATOMIC_OP, MUTEX, SEMAPHORE>::ReaderWriterMutexImpl()
{
    ATOMIC_OP::initInt64(&d_state);
}

// MANIPULATORS
template <class ATOMIC_OP, class MUTEX, class SEMAPHORE>
inline
void ReaderWriterMutexImpl<ATOMIC_OP, MUTEX, SEMAPHORE>::lockRead()
{
    bsls::Types::Int64 state = ATOMIC_OP::getInt64(&d_state);
    bsls::Types::Int64 expState;

    do {
        // If there are no actual or pending writers, the lock can be obtained
        // by simply incrementing the reader count.  This results, typically,
        // in a substantial performance benefit when there are very few writers
        // in the system and no noticible degredation in other scenarios.

        if (state & (k_WRITER | k_PENDING_WRITER_MASK)) {
            d_mutex.lock();
            ATOMIC_OP::addInt64AcqRel(&d_state, k_READER_INC);
            d_mutex.unlock();
            return;                                                   // RETURN
        }

        expState = state;
        state    = ATOMIC_OP::testAndSwapInt64AcqRel(&d_state,
                                                     state,
                                                     state + k_READER_INC);
    } while (state != expState);
}

template <class ATOMIC_OP, class MUTEX, class SEMAPHORE>
inline
void ReaderWriterMutexImpl<ATOMIC_OP, MUTEX, SEMAPHORE>::lockWrite()
{
    // The presence of a pending writer must be noted before attempting the
    // 'mutex.lock' in case this thread blocks on the mutex lock operation.

    ATOMIC_OP::addInt64AcqRel(&d_state, k_PENDING_WRITER_INC);
    d_mutex.lock();
    if (ATOMIC_OP::addInt64NvAcqRel(&d_state,
                                    k_WRITER - k_PENDING_WRITER_INC)
                                                             & k_READER_MASK) {
        // There must be no readers present to obtain the write lock.  By
        // obtaining the mutex, there can be no new readers obtaining a read
        // lock (ensuring this lock is not reader biased).  If there are
        // currently readers present, the last reader to release its read lock
        // will 'post' to 'd_semaphore'.  Note that, since the locking
        // primitive is a semaphore, the timing of the 'wait' and the 'post' is
        // irrelevant.

        d_semaphore.wait();
    }
}

template <class ATOMIC_OP, class MUTEX, class SEMAPHORE>
inline
int ReaderWriterMutexImpl<ATOMIC_OP, MUTEX, SEMAPHORE>::tryLockRead()
{
    bsls::Types::Int64 state = ATOMIC_OP::getInt64(&d_state);

    // If there are no actual or pending writers, the lock can be obtained by
    // simply incrementing the reader count.  Since this method must return
    // "immediately" if the lock is not obtained, only one attempt will be
    // performed.

    if (0 == (state & (k_WRITER | k_PENDING_WRITER_MASK))) {
        if (state == ATOMIC_OP::testAndSwapInt64AcqRel(&d_state,
                                                       state,
                                                       state + k_READER_INC)) {
            return 0;                                                 // RETURN
        }
    }

    // To accomodate the possibility of mutex re-acquisition being important
    // for the performance characteristics of this lock, the mutex acquisition
    // must be attempted.

    if (0 == d_mutex.tryLock()) {
        ATOMIC_OP::addInt64AcqRel(&d_state, k_READER_INC);
        d_mutex.unlock();
        return 0;                                                     // RETURN
    }
    return 1;
}

template <class ATOMIC_OP, class MUTEX, class SEMAPHORE>
inline
int ReaderWriterMutexImpl<ATOMIC_OP, MUTEX, SEMAPHORE>::tryLockWrite()
{
    // To obtain a write lock, 'd_mutex' must be obtained *and* there must be
    // no readers.

    if (0 == d_mutex.tryLock()) {
        bsls::Types::Int64 state = ATOMIC_OP::getInt64Acquire(&d_state);

        if (0 == (state & k_READER_MASK)) {
            // Since the mutex is obtained and there are no readers (and none
            // can enter while the mutex is held), the lock has been obtained.

            ATOMIC_OP::addInt64AcqRel(&d_state, k_WRITER);

            return 0;                                                 // RETURN
        }
        d_mutex.unlock();
    }
    return 1;
}

template <class ATOMIC_OP, class MUTEX, class SEMAPHORE>
inline
void ReaderWriterMutexImpl<ATOMIC_OP, MUTEX, SEMAPHORE>::unlock()
{
    // A caller of 'unlock', by contract, is either the owner of a read lock or
    // the owner of the write lock.  If the caller is the owner of the write
    // lock, there are no readers and there can not be readers until after the
    // 'unlockWrite' completes.  If the caller owns a read lock, 'd_state' must
    // reflect at least one reader until the 'unlockRead' completes.  In either
    // case, the chosen branch of the following 'if' is correct.

    if (ATOMIC_OP::getInt64Acquire(&d_state) & k_READER_MASK) {
        unlockRead();
    }
    else {
        unlockWrite();
    }
}

template <class ATOMIC_OP, class MUTEX, class SEMAPHORE>
inline
void ReaderWriterMutexImpl<ATOMIC_OP, MUTEX, SEMAPHORE>::unlockRead()
{
    BSLS_ASSERT_SAFE(0 < (ATOMIC_OP::getInt64Acquire(&d_state)
                                                             & k_READER_MASK));

    bsls::Types::Int64 state = ATOMIC_OP::addInt64Nv(&d_state, -k_READER_INC);

    // If this is the last reader and there is a pending writer who obtained
    // 'd_mutex' (and hence will be calling 'wait' on 'd_semaphore'), 'post' to
    // 'd_semaphore' to allow the pending writer to complete obtaining the
    // write lock.

    if (0 == (state & k_READER_MASK) && (state & k_WRITER)) {
        d_semaphore.post();
    }
}

template <class ATOMIC_OP, class MUTEX, class SEMAPHORE>
inline
void ReaderWriterMutexImpl<ATOMIC_OP, MUTEX, SEMAPHORE>::unlockWrite()
{
    BSLS_ASSERT_SAFE(k_WRITER == (ATOMIC_OP::getInt64Acquire(&d_state)
                                                                  & k_WRITER));

    ATOMIC_OP::addInt64AcqRel(&d_state, -k_WRITER);
    d_mutex.unlock();
}

}  // close package namespace
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
