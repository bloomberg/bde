// bslmt_readerwritermuteximpl.cpp                                    -*-C++-*-

#include <bslmt_readerwritermuteximpl.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_readerwriterwmuteximpl_cpp,"$Id$ $CSID$")

///Implementation Note
///===================
// The core of the implementation is a direct interpretation of the
// requirements of a reader-writer lock: the writer has exclusive access to
// this lock and the readers may share access to this lock only with other
// readers.  The implementation -- ignoring an optimization -- requires all
// threads to first acquire a mutex.
//
// For writer threads, acquiring the mutex prevents other threads from
// obtaining read locks and ensures only this thread can proceed to obtaining a
// write lock.  Now, this thread waits for the outstanding read locks to be
// returned.  Then, this thread is granted the write lock.  To efficiently wait
// for the outstanding read locks to be returned, a count of outstanding read
// locks is maintained and a semaphore is used to wait when necessary.
//
// For reader threads, acquiring the mutex ensures there is no outstanding
// write lock.  Now, this thread updates the reader count.  Then, this thread
// releases the mutex.  The presence of a non-zero reader count prevents a
// write lock from being obtained while this read lock is outstanding.  Note
// that another reader thread may obtain a read lock at this point, assuming no
// writer threads have intervened.
//
// Releasing a write lock is equivalant, at the moment, to unlocking the mutex.
// Releasing a read lock is more complicated.  The count of readers must be
// decremented and this must be done with appropriate memory barriers to allow
// the write lock procedure to work correctly.  Furthermore, the last returned
// read lock must also post to the semaphore if, and only if, there is a writer
// that is, or will be, blocked on the semphore.  This implies the need for an
// indicator that there is a writer and the need to access this indicator and
// update the outstanding read lock count atomically.
//
// At this point, the implementation requires a mutex, a semaphore, and atomic
// state that indicates the presence of a writer and the count of the number of
// outstanding read locks.
//
// Note that, if there are no writers, obtaining a read lock currently requires
// locking and unlocking a mutex and this implies substantial overhead and
// possible poor performance if a reader thread is swapped out while holding
// the mutex.  To optimize this procedure, while preventing readers from being
// able to starve writers, a count of the pending writers (those blocked on the
// mutex lock) is maintained and, when this count of pending writers and the
// original writer indicator are zero, a reader thread will first attempt to
// update the atomic state to indicate an additional outstanding read lock.  If
// successful, the lock has been obtained.  Otherwise, the original algorithm
// of obtaining the mutex is used.

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
