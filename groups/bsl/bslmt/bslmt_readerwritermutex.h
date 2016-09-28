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
// This implementation does not give preference to readers or writers.  When
// multiple threads are waiting to obtain the lock, the successful thread is
// chosen by an underlying locking mechanism (i.e., a mutex).
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_SEMAPHORE
#include <bslmt_semaphore.h>
#endif

#ifndef INCLUDED_BSLMT_THREADUTIL
#include <bslmt_threadutil.h>
#endif

namespace BloombergLP {
namespace bslmt {

                         // =======================
                         // class ReaderWriterMutex
                         // =======================

class ReaderWriterMutex {
    // This class provides a multi-reader/single-writer lock mechanism.

    // CLASS DATA
    static const bsls::Types::Int64 k_READER_MASK       = 0x000000000fffffffLL;
    static const bsls::Types::Int64 k_READER_INC        = 0x0000000000000001LL;
    static const bsls::Types::Int64 k_PENDING_WRITER_MASK
                                                        = 0x0ffffffff0000000LL;
    static const bsls::Types::Int64 k_PENDING_WRITER_INC
                                                        = 0x0000000010000000LL;
    static const bsls::Types::Int64 k_WRITER            = 0x1000000000000000LL;

    // DATA
    bsls::AtomicOperations::AtomicTypes::Int64 d_state;      // atomic value
                                                             // used to track
                                                             // the state of
                                                             // this mutex

    Mutex                                      d_mutex;      // primary access
                                                             // control

    Semaphore                                  d_semaphore;  // used to capture
                                                             // writers
                                                             // released from
                                                             // 'd_mutex' but
                                                             // must wait for
                                                             // readers to
                                                             // finish

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
    bsls::AtomicOperations::initInt64(&d_state);
}

// MANIPULATORS
inline
void bslmt::ReaderWriterMutex::lockRead()
{
    bsls::Types::Int64 state = bsls::AtomicOperations::getInt64(&d_state);
    bsls::Types::Int64 expState;

    do {
        if (state & (k_WRITER | k_PENDING_WRITER_MASK)) {
            d_mutex.lock();
            bsls::AtomicOperations::addInt64AcqRel(&d_state, k_READER_INC);
            d_mutex.unlock();
            return;                                                   // RETURN
        }

        expState = state;
        state    = bsls::AtomicOperations::testAndSwapInt64AcqRel(
                                                         &d_state,
                                                         state,
                                                         state + k_READER_INC);
    } while (state != expState);
}

inline
void bslmt::ReaderWriterMutex::lockWrite()
{
    bsls::AtomicOperations::addInt64NvAcqRel(&d_state, k_PENDING_WRITER_INC);
    d_mutex.lock();
    if (bsls::AtomicOperations::addInt64NvAcqRel(&d_state,
                                                 k_WRITER
                                                        - k_PENDING_WRITER_INC)
                                                             & k_READER_MASK) {
        d_semaphore.wait();
    }
}

inline
int bslmt::ReaderWriterMutex::tryLockRead()
{
    if (0 == d_mutex.tryLock()) {
        bsls::AtomicOperations::addInt64AcqRel(&d_state, k_READER_INC);
        d_mutex.unlock();
        return 0;                                                     // RETURN
    }
    return 1;
}

inline
int bslmt::ReaderWriterMutex::tryLockWrite()
{
    if (0 == d_mutex.tryLock()) {
        if (0 == bsls::AtomicOperations::testAndSwapInt64(&d_state,
                                                          0,
                                                          k_WRITER)) {
            return 0;                                                 // RETURN
        }
        d_mutex.unlock();
    }
    return 1;
}

inline
void bslmt::ReaderWriterMutex::unlock()
{
    if (bsls::AtomicOperations::getInt64Acquire(&d_state) & k_READER_MASK) {
        unlockRead();
    }
    else {
        unlockWrite();
    }
}

inline
void bslmt::ReaderWriterMutex::unlockRead()
{
    bsls::Types::Int64 state = bsls::AtomicOperations::addInt64Nv(
                                                                &d_state,
                                                                -k_READER_INC);

    if (0 == (state & k_READER_MASK) && (state & k_WRITER)) {
        d_semaphore.post();
    }
}

inline
void bslmt::ReaderWriterMutex::unlockWrite()
{
    bsls::AtomicOperations::addInt64AcqRel(&d_state, -k_WRITER);
    d_mutex.unlock();
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
