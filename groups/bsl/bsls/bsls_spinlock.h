// bsls_spinlock.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLS_SPINLOCK
#define INCLUDED_BSLS_SPINLOCK

#include <bsls_ident.h>
BSLS_IDENT("$: $")

//@PURPOSE: Provide a spin lock.
//
//@CLASSES:
//  bsls::SpinLock: A mutex using "busy waiting" atomic operations
//  bsls::SpinLockGuard: Automatic locking-unlocking of SpinLock
//
//@DESCRIPTION: This component provides a "busy wait" mutual exclusion lock
// primitive ("mutex").  A 'SpinLock' is small and statically-initializable,
// but because it "spins" in a tight loop rather than using system operations
// to block the thread of execution, it is unsuited for use cases involving
// high contention or long critical regions.  Additionally, this component does
// not provide any guarantee of fairness when multiple threads are contending
// for the same lock.  Use 'SpinLockGuard' for automatic locking-unlocking in a
// scope.
//
// *WARNING*: A 'bsls::SpinLock' *must* be aggregate initialized to
// 'BSLS_SPINLOCK_UNLOCKED'.  For example:
//..
//  bsls::SpinLock lock = BSLS_SPINLOCK_UNLOCKED;
//..
// Note that 'SpinLock' is a struct requiring aggregate initialization to allow
// lock variables to be statically initialized when using a C++03 compiler
// (i.e., without using 'constexpr').
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Maintaining Static Count/Max Values
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to determine the maximum number of threads executing a
// block of code concurrently.  Note that such a use case naturally calls for a
// statically initialized lock and the critical region involves a few integer
// operations; SpinLock may be suitable.
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
// guard them.  'SpinLock' may be statically initialized using the
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
// the thread count.  Any intervening code can run in parallel.
//..
//   }
//..
//
///Example 2: Fine-Grained Locking
///- - - - - - - - - - - - - - - -
// Suppose that we have a large array of objects to be manipulated concurrently
// by multiple threads, but the size of the array itself does not change.
// (This might be because it represents an inherently fixed number of objects
// or because changes to the array size are infrequent and controlled by some
// other synchronization mechanism like a "reader-writer" lock).  Thus one
// thread can manipulate a particular object in the array concurrently with a
// different thread manipulating another.  If the manipulations are short and
// contention is likely to be low, SpinLock might be suitable due to its small
// size.
//
// In particular, imagine we want a threadsafe "multi-queue".  In this case, we
// would have an array of queues, each with a SpinLock member for fine-grained
// locking.  First, we define the type to be held in the array.
//..
//  template<typename TYPE>
//  class LightweightThreadsafeQueue {
//     // This type implements a threadsafe queue with a small memory
//     // footprint and low initialization costs. It is designed for
//     // low-contention use only.
//
//     // TYPES
//     struct Node {
//          TYPE  d_item;
//          Node *d_next_p;
//
//          Node(const TYPE& item) : d_item(item), d_next_p(0) {}
//      };
//
//     // DATA
//     Node           *d_front_p; // Front of queue, or 0 if empty
//     Node           *d_back_p; // Back of queue, or 0 if empty
//     bsls::SpinLock  d_lock;
//
//   public:
//     // CREATORS
//     LightweightThreadsafeQueue();
//       // Create an empty queue.
//
//     ~LightweightThreadsafeQueue();
//       // Destroy this object.
//
//     // MANIPULATORS
//     int dequeue(TYPE* value);
//        // Remove the element at the front of the queue and load it into the
//        // specified 'value'. Return '0' on success, or a nonzero value if
//        // the queue is empty.
//
//     void enqueue(const TYPE& value);
//        // Add the specified 'value' to the back of the queue.
//   };
//..
// Next, we implement the creators.  Note that a different idiom is used to
// initialize member variables of 'SpinLock' type than is used for static
// variables:
//..
//  template<typename TYPE>
//  LightweightThreadsafeQueue<TYPE>::LightweightThreadsafeQueue()
//  : d_front_p(0)
//  , d_back_p(0)
//  , d_lock(bsls::SpinLock::s_unlocked)
//  {}
//
//  template<typename TYPE>
//  LightweightThreadsafeQueue<TYPE>::~LightweightThreadsafeQueue() {
//     for (Node *node = d_front_p; 0 != node; ) {
//         Node *next = node->d_next_p;
//         delete node;
//         node = next;
//     }
//  }
//..
// Then we implement the manipulator functions using 'SpinLockGuard' to ensure
// thread safety.  Note that we do memory allocation and deallocation outside
// the scope of the lock, as these may involve system calls that should be
// avoided in the scope of a SpinLock.
//..
//  template<typename TYPE>
//  int LightweightThreadsafeQueue<TYPE>::dequeue(TYPE* value) {
//     Node *front;
//     {
//        bsls::SpinLockGuard guard(&d_lock);
//        front = d_front_p;
//        if (0 == front) {
//          return 1;
//        }
//
//        *value = front->d_item;
//
//        if (d_back_p == front) {
//           d_front_p = d_back_p = 0;
//        } else {
//           d_front_p = front->d_next_p;
//        }
//     }
//     delete front;
//     return 0;
//  }
//
//  template<typename TYPE>
//  void LightweightThreadsafeQueue<TYPE>::enqueue(const TYPE& value) {
//     Node *node = new Node(value);
//     bsls::SpinLockGuard guard(&d_lock);
//     if (0 == d_front_p && 0 == d_back_p) {
//        d_front_p = d_back_p = node;
//     } else {
//        d_back_p->d_next_p = node;
//        d_back_p = node;
//     }
//  }
//..
//  To illustrate fine-grained locking with this queue, we create a thread
//  function that will manipulate queues out of a large array at random.
//  Since each element in the array is locked independently, these threads
//  will rarely contend for the same queue and can run largely in parallel.
//..
// const int NUM_QUEUES = 10000;
// const int NUM_ITERATIONS = 20000;
//
// struct QueueElement {
//    int d_threadId;
//    int d_value;
// };
//
// struct ThreadParam {
//    LightweightThreadsafeQueue<QueueElement> *d_queues_p;
//    int                                       d_threadId;
// };
//
// void *addToRandomQueues(void *paramAddr) {
//    ThreadParam *param = (ThreadParam*)paramAddr;
//    LightweightThreadsafeQueue<QueueElement> *queues = param->d_queues_p;
//    int threadId = param->d_threadId;
//    unsigned seed = threadId;
//    for (int i = 0; i < NUM_ITERATIONS; ++i) {
//       int queueIndex = rand_r(&seed) % NUM_QUEUES;
//       LightweightThreadsafeQueue<QueueElement> *queue = queues + queueIndex;
//       QueueElement value = { threadId, i };
//       queue->enqueue(value);
//    }
//    return 0;
// }
//..
// Finally, we create the "multi-queue" and several of these threads to
// manipulate it.  We assume the existence of a createThread() function that
// starts a new thread of execution with a parameter, and we omit details of
// "joining" these threads.
//..
// enum { NUM_THREADS = 3};
// LightweightThreadsafeQueue<QueueElement> multiQueue[NUM_QUEUES];
// ThreadParam threadParams[NUM_THREADS];
// for (int i = 0; i < NUM_THREADS; ++i) {
//   threadParams[i].d_queues_p = multiQueue;
//   threadParams[i].d_threadId = i + 1;
//   createThread(addToRandomQueues, threadParams + i);
// }
//..
//

#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>
#include <bsls_performancehint.h>

#define BSLS_SPINLOCK_UNLOCKED  { {0} }
    // Use this macro as the value for initializing an object of type
    // 'SpinLock'.  For example:
    //..
    //  SpinLock lock = BSLS_SPINLOCK_UNLOCKED;
    //..

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef unsigned long DWORD;
typedef int BOOL;

extern "C" {
    __declspec(dllimport) void __stdcall Sleep(
            DWORD dwMilliseconds);

    __declspec(dllimport) DWORD __stdcall SleepEx(
            DWORD dwMilliseconds,
            BOOL  bAlertable);
};
#else
#include <pthread.h>
#include <sched.h>
#include <time.h>
#endif

namespace BloombergLP {
namespace bsls {

                             // ==============
                             // class SpinLock
                             // ==============
struct SpinLock {
    // A statically-initializable synchronization primitive that "spins" (i.e.,
    // executes user instructions in a tight loop) rather than blocking waiting
    // threads using system calls.  The following idiom is used to initialize
    // 'SpinLock' variables:
    //..
    //  SpinLock lock = BSLS_SPINLOCK_UNLOCKED;
    //..
    // A class member 'd_lock' of type 'SpinLock' may be initialized using the
    // following idiom:
    //..
    //  , d_lock(SpinLock::s_unlocked)
    //..

  private:
    // NOT IMPLEMENTED
    SpinLock& operator=(const SpinLock&) BSLS_KEYWORD_DELETED;

    // We would like to prohibit copy construction, but then this class would
    // not be a POD and could not be initialized statically:
    //
    // SpinLock(const SpinLock&);

    // PRIVATE TYPES
    enum {
        e_UNLOCKED = 0, // unlocked state value
        e_LOCKED = 1    // locked state value
    };

    // PRIVATE CLASS METHODS
    static void doBackoff(int *count);
        // This function provides a backoff mechanism for 'lock' and 'tryLock',
        // using the specified 'count' as the backoff counter.  'count' is
        // incremented within this method.

    static void pause();
        // If available, invoke a pause operation (e.g., Intel's 'pause'
        // instruction); otherwise do nothing.  (i.e., this method is a no-op).

    static void sleepMillis(int milliseconds);
        // Sleep the specified 'milliseconds'.  Note that this partially
        // reimplements 'bslmt::ThreadUtil::microSleep' locally, as 'bslmt' is
        // above 'bsls'.

    static void yield();
        // Move the current thread to the end of the scheduler's queue and
        // schedule another thread to run.  Note that this allows cooperating
        // threads of the same priority to share CPU resources equally.  Also
        // note that this reimplements 'bslmt::ThreadUtil::yield()' locally, as
        // 'bslmt' is above 'bsls'.

  public:
    // PUBLIC CLASS DATA
    static const SpinLock s_unlocked;
        // This constant SpinLock is always unlocked.  It is suitable for use
        // initializing class members of SpinLock type.

    // PUBLIC DATA
    AtomicOperations::AtomicTypes::Int d_state;
        // Public to allow this type to be a statically-initializable POD.  Do
        // not use directly.

    // MANIPULATORS
    void lock();
        // Spin (repeat a loop continuously without using the system to pause
        // or reschedule the thread) until this object is unlocked, then
        // atomically acquire the lock.

    void lockWithBackoff();
        // Repeat a loop continuously, potentially using the system to pause or
        // reschedule the thread, until this object is unlocked, then
        // atomically acquire the lock.  The spinning has backoff logic.  Note
        // that this method is recommended when system calls are permissible,
        // significant contention is expected, and no better mutual exclusion
        // primitive is available.

    void lockWithoutBackoff();
        // Spin (repeat a loop continuously without using the system to pause
        // or reschedule the thread) until this object is unlocked, then
        // atomically acquire the lock.  The spinning does not perform a
        // backoff.

    int tryLock(int numRetries = 0);
        // Attempt to acquire the lock; optionally specify the 'numRetries'
        // times to attempt again if this object is already locked.  Return 0
        // on success, and a non-zero value if the lock was not successfully
        // acquired.  The behavior is undefined unless '0 <= numRetries'.

    void unlock();
        // Release the lock.  The behavior is undefined unless the current
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
        // Create a proctor object that manages the specified 'lock'.  Invoke
        // 'lock->lock()'.

    ~SpinLockGuard();
        // Destroy this proctor object and invoke 'unlock()' on the lock
        // managed by this object.

    // MANIPULATORS
    SpinLock *release();
        // Return the lock pointer that was provided at construction and stop
        // managing it.  (Subsequent calls to 'release()' will return null and
        // the destruction of this object will not affect the lock.)  The lock
        // status is not changed by this call.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                             // --------------
                             // class SpinLock
                             // --------------
// PRIVATE CLASS METHODS
inline
void SpinLock::doBackoff(int *count) {
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(*count < 10)) {
        pause();
    } else if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(*count < 25)) {
        yield();
    } else {
        sleepMillis(10);
    }
    ++(*count);
}


inline
void SpinLock::sleepMillis(int milliseconds)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    ::Sleep(milliseconds);
#else
    timespec naptime;
    naptime.tv_sec = 0;
    naptime.tv_nsec = 1000 * 1000 * milliseconds;
    nanosleep(&naptime, 0);
#endif
}

inline
void SpinLock::yield() {
#ifdef BSLS_PLATFORM_OS_WINDOWS
    ::SleepEx(0, 0);
#else
    sched_yield();
#endif
}

// MANIPULATORS
inline
void SpinLock::lock() {
    lockWithoutBackoff();
}

inline
void SpinLock::lockWithBackoff() {
    int count = 0;
    do {
        // Implementation note: the outer 'if' block is not logically necessary
        // but may reduce memory barrier costs when spinning.
        if (e_UNLOCKED == AtomicOperations::getIntAcquire(&d_state)) {
            if (e_UNLOCKED == AtomicOperations::swapIntAcqRel(&d_state,
                                                              e_LOCKED))
            {
                break;
            }
        }
        doBackoff(&count);
    } while (1);
}

inline
void SpinLock::lockWithoutBackoff() {
    do {
        // Implementation note: the outer 'if' block is not logically necessary
        // but may reduce memory barrier costs when spinning.
        if (e_UNLOCKED == AtomicOperations::getIntAcquire(&d_state)) {
            if (e_UNLOCKED == AtomicOperations::swapIntAcqRel(&d_state,
                                                              e_LOCKED))
            {
                break;
            }
        }
    } while (1);
}

inline
int SpinLock::tryLock(int numRetries) {
    int count = 0;
    do {
        // See lock() for implementation note.
        if (e_UNLOCKED == AtomicOperations::getIntAcquire(&d_state)) {
            if (e_UNLOCKED == AtomicOperations::swapIntAcqRel(&d_state,
                                                              e_LOCKED))
            {
                return 0;                                             // RETURN
            }
        }
        doBackoff(&count);
    } while (numRetries--);
    return -1;
}

inline
void SpinLock::unlock() {
    BSLS_ASSERT_SAFE(e_LOCKED == AtomicOperations::getInt(&d_state));

    AtomicOperations::setIntRelease(&d_state, e_UNLOCKED);
}

                          // -------------------
                          // class SpinLockGuard
                          // -------------------
inline
SpinLockGuard::SpinLockGuard(SpinLock *lock)
: d_lock_p(lock) {
    BSLS_ASSERT_SAFE(0 != lock);
    lock->lock();
}

inline
SpinLockGuard::~SpinLockGuard()
{
    if (0 != d_lock_p) {
        d_lock_p->unlock();
    }
}

// MANIPULATORS
inline
SpinLock *SpinLockGuard::release() {
    SpinLock *lock = d_lock_p;
    d_lock_p = 0;
    return lock;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
