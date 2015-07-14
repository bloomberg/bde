// bsls_spinlock.h                                               -*-C++-*-
#ifndef INCLUDED_BSLS_SPINLOCK
#define INCLUDED_BSLS_SPINLOCK

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$: $")

//@PURPOSE: Provide a spin lock.
//
//@CLASSES:
//       SpinLock: A mutex using "busy waiting" atomic operations.
//  SpinLockGuard: Automatic locking-unlocking of SpinLock
//
//@AUTHOR: David Schumann (dschumann1)
//
//@DESCRIPTION: This component provides a "busy wait" mutual exclusion lock
// primitive ("mutex"). This object is small and statically initializable, but
// because it "spins" in a tight loop rather than using system operations to
// block the thread of execution, it is unsuited for use cases involving high
// contention or long critical regions. Additionally, this component does not
// provide any guarantee of fairness when multiple threads are contending for
// the same lock.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Executing a custom functor type
///- - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to determine the maximum number of threads executing
// a block of code concurrently. Note that such a use case naturally calls
// for a statically initialized lock and the critical region involves
// a few integer operations; spinlock may be suitable.
//
// First, we define a type to manage the count within a scope:
//..
// class MaxConcurrencyCounter {
//     // This type manages a count and high-water-mark within a scope.
//     // It decrements the count in its destructor upon leaving the scope.
//
//     // DATA
//     int            *d_count_p;
//     bsls::SpinLock *d_lock_p;
//
//   public:
//     // CREATORS
//     MaxConcurrencyCounter(int *count, int *max, bsls::SpinLock *lock);
//         // Acquire the specified 'lock' and increment the specified 'count'.
//         // If the resulting value is larger than the specified 'max',
//         // load it into 'max'. Release 'lock' and create a scoped guard to
//         // decrement 'count' on destruction.
//
//     ~MaxConcurrencyCounter();
//         // Acquire the lock specified at construction, decrement the count
//         // variable, and release the lock.
//   };
//
//   MaxConcurrencyCounter::MaxConcurrencyCounter(int            *count,
//                                                int            *max,
//                                                bsls::SpinLock *lock)
//   : d_count_p(count)
//   , d_lock_p(lock) {
//      bsls::SpinLockGuard guard(lock);
//      int result = ++(*count);
//      if (result > *max) {
//         *max = result;
//      }
//   }
//
//   MaxConcurrencyCounter::~MaxConcurrencyCounter() {
//      bsls::SpinLockGuard guard(d_lock_p);
//      --(*d_count_p);
//   }
//..
// Next, we declare static variables to track the call count and a SpinLock to
// guard them. 'SpinLock' may be statically initialized using the
// 'BSLS_SPINLOCK_UNLOCKED' constant:
//..
//   {
//      static int            threadCount = 0;
//      static int            maxThreads = 0;
//      static bsls::SpinLock threadLock = BSLS_SPINLOCK_UNLOCKED;
//..
// Next, by creating a 'MaxConcurrencyCounter' object, each thread entering the
// block of code uses the 'SpinLock' to synchronize manipulation of the static
// count variables:
//..
//      MaxConcurrencyCounter counter(&threadCount, &maxThreads, &threadLock);
//..
// Finally, closing the block synchronizes on the 'SpinLock' again to decrement
// the thread count. Any intervening code can run in parallel.
//..
//   }
//..

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#define BSLS_SPINLOCK_UNLOCKED  { {0} }
    // Use this macro as the value for initializing an object of type
    // 'SpinLock'.  For example:
    //..
    //  SpinLock lock = BSLS_SPINLOCK_UNLOCKED;
    //..

namespace BloombergLP {
namespace bsls {

                             // ==============
                             // class SpinLock
                             // ==============
struct SpinLock {
    // A statically-initializable synchronization primitive that "spins"
    // (i.e., executes user instructions in a tight loop) rather than
    // blocking waiting threads using system calls. The following idiom is
    // used to initialize 'SpinLock' objects:
    //..
    //  SpinLock lock = BSLS_SPINLOCK_UNLOCKED;
    //..
    
  private:
    // NOT IMPLEMENTED
    SpinLock& operator=(const SpinLock&);
    
    // We would like to prohibit copy construction, but then this class
    // would not be a POD and could not be initialized statically:
    // SpinLock(const SpinLock&);

    // PRIVATE TYPES
    enum {
        BSLS_UNLOCKED = 0, // unlocked state value
        BSLS_LOCKED = 1    // locked state value
    };
    
  public:
    // DATA
    AtomicOperations::AtomicTypes::Int d_state;
        // Public to allow this type to be a statically-initializable POD.
        // Do not use directly.
    
    // MANIPULATORS
    void lock();
        // Spin (repeat a loop continuously without using the system to
        // pause or reschedule the thread) until this object is unlocked,
        // then atomically acquire the lock.
    
    int tryLock(int numRetries = 0);
        // Attempt to acquire the lock; if this object is already locked,
        // attempt again up to the specified 'numRetries' times. Return 0
        // on success, and a non-zero value if the lock was not successfully
        // acquired.  The behavior is undefined unless '0 <= numRetries'.

    void unlock();
        // Release the lock. The behavior is undefined unless the current
        // thread holds the lock.
};

                         // ===================
                         // class SpinLockGuard
                         // ===================
class SpinLockGuard {
    // This type implements a scoped guard for 'SpinLock'.

    // DATA
    SpinLock *d_lock_p; // lock proctored by this object

  private:
    // NOT IMPLEMENTED
    SpinLockGuard(const SpinLockGuard&);
    SpinLockGuard& operator=(const SpinLockGuard&);

  public:

    // CREATORS
    explicit SpinLockGuard(SpinLock *lock);
       // Create a proctor object that manages the specified 'lock'. Invoke
       // 'lock->lock()'.

    ~SpinLockGuard();
       // Destroy this proctor object and invoke 'unlock()' on the lock managed
       // by this object.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                             // --------------
                             // class SpinLock
                             // --------------
inline
void SpinLock::lock() {
    do {
        if (BSLS_UNLOCKED == AtomicOperations::getIntAcquire(&d_state)) {
            if (BSLS_UNLOCKED == AtomicOperations::swapIntAcqRel(&d_state,
                                                                 BSLS_LOCKED))
            {
                break;
            }
        }
    } while(1);
}

inline
int SpinLock::tryLock(int numRetries) {
    do {
        if (BSLS_UNLOCKED == AtomicOperations::getIntAcquire(&d_state)) {
            if (BSLS_UNLOCKED == AtomicOperations::swapIntAcqRel(&d_state,
                                                                 BSLS_LOCKED))
            {
                return 0;
            }
        }
    } while(numRetries--);
    return -1;
}

inline
void SpinLock::unlock() {
    BSLS_ASSERT_SAFE(BSLS_LOCKED == AtomicOperations::getInt(&d_state));
    
    AtomicOperations::setIntRelease(&d_state, BSLS_UNLOCKED);
}
        
                          // -------------------
                          // class SpinLockGuard
                          // -------------------
inline
SpinLockGuard::SpinLockGuard(SpinLock *lock)
: d_lock_p(lock) {
    lock->lock();
}

inline
SpinLockGuard::~SpinLockGuard()
{
    d_lock_p->unlock();
}
    
}  // close namespace bsls
}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
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
// ----------------------------- END-OF-FILE ---------------------------------

    

    
