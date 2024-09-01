// bslmt_semaphore.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLMT_SEMAPHORE
#define INCLUDED_BSLMT_SEMAPHORE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a semaphore class.
//
//@CLASSES:
//  bslmt::Semaphore: semaphore class
//
//@SEE_ALSO: bslmt_timedsemaphore
//
//@DESCRIPTION: This component defines a portable and efficient thread
// synchronization primitive.  In particular, `bslmt::Semaphore` is an
// efficient synchronization primitive that enables sharing of a counted number
// of resources or exclusive access.  The usage model of this facility is
// modeled on POSIX semaphores and Windows semaphores.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// This example illustrates a very simple queue where potential clients can
// push integers to a queue, and later retrieve the integer values from the
// queue in FIFO order.  It illustrates two potential uses of semaphores: to
// enforce exclusive access, and to allow resource sharing.
// ```
// /// FIFO queue of integer values.
// class IntQueue {
//
//     // DATA
//     bsl::deque<int> d_queue;         // underlying queue
//     bslmt::Semaphore d_mutexSem;     // mutual-access semaphore
//     bslmt::Semaphore d_resourceSem;  // resource-availability semaphore
//
//     // NOT IMPLEMENTED
//     IntQueue(const IntQueue&);
//     IntQueue& operator=(const IntQueue&);
//
//   public:
//     // CREATORS
//
//     /// Create an `IntQueue` object.  Optionally specified a
//     /// `basicAllocator` used to supply memory.  If `basicAllocator` is
//     /// 0, the currently installed default allocator is used.
//     explicit IntQueue(bslma::Allocator *basicAllocator = 0);
//
//     /// Destroy this `IntQueue` object.
//     ~IntQueue();
//
//     // MANIPULATORS
//
//     /// Retrieve an integer from this `IntQueue` object.  Integer values
//     /// are obtained from the queue in FIFO order.
//     int getInt();
//
//     /// Push the specified `value` to this `IntQueue` object.
//     void pushInt(int value);
// };
// ```
// Note that the `IntQueue` constructor increments the count of the semaphore
// to 1 so that values can be pushed into the queue immediately following
// construction:
// ```
// // CREATORS
// IntQueue::IntQueue(bslma::Allocator *basicAllocator)
// : d_queue(basicAllocator)
// {
//     d_mutexSem.post();
// }
//
// IntQueue::~IntQueue()
// {
//     d_mutexSem.wait();  // Wait for potential modifier.
// }
//
// // MANIPULATORS
// int IntQueue::getInt()
// {
//     // Waiting for resources.
//     d_resourceSem.wait();
//
//     // `d_mutexSem` is used for exclusive access.
//     d_mutexSem.wait();        // lock
//     const int ret = d_queue.back();
//     d_queue.pop_back();
//     d_mutexSem.post();        // unlock
//
//     return ret;
// }
//
// void IntQueue::pushInt(int value)
// {
//     d_mutexSem.wait();
//     d_queue.push_front(value);
//     d_mutexSem.post();
//
//     d_resourceSem.post();  // Signal we have resources available.
// }
// ```

#include <bslscm_version.h>

#include <bslmt_platform.h>
#include <bslmt_semaphoreimpl_counted.h>
#include <bslmt_semaphoreimpl_pthread.h>
#include <bslmt_semaphoreimpl_win32.h>

namespace BloombergLP {
namespace bslmt {

template <class SEMAPHORE_POLICY>
class SemaphoreImpl;

                             // ===============
                             // class Semaphore
                             // ===============

/// This class implements a portable semaphore type for thread
/// synchronization.  It forwards all requests to an appropriate
/// platform-specific implementation.
class Semaphore {

    // DATA
    SemaphoreImpl<Platform::SemaphorePolicy> d_impl;  // platform-specific
                                                      // implementation

    // NOT IMPLEMENTED
    Semaphore(const Semaphore&);
    Semaphore& operator=(const Semaphore&);

  public:
    // CREATORS

    /// Create a semaphore initially having a count of 0.  This
    /// method does not return normally unless there are sufficient system
    /// resources to construct the object.
    Semaphore();

    /// Create a semaphore initially having the specified `count`.  This
    /// method does not return normally unless there are sufficient system
    /// resources to construct the object.
    explicit
    Semaphore(int count);

    /// Destroy this semaphore.
    ~Semaphore();

    // MANIPULATORS

    /// Atomically increment the count of this semaphore.
    void post();

    /// Atomically increase the count of this semaphore by the specified
    /// `value`.  The behavior is undefined unless `value > 0`.
    void post(int value);

    /// If the count of this semaphore is positive, atomically decrement the
    /// count and return 0; otherwise, return a non-zero value with no
    /// effect on the count.
    int tryWait();

    /// Block until the count of this semaphore is a positive value, then
    /// atomically decrement the count and return.
    void wait();

    // ACCESSORS

    /// Return the value of the current count of this semaphore.
    int getValue() const;
};

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                             // ---------------
                             // class Semaphore
                             // ---------------

// CREATORS
inline
bslmt::Semaphore::Semaphore()
: d_impl(0)
{
}

inline
bslmt::Semaphore::Semaphore(int count)
: d_impl(count)
{
}

inline
bslmt::Semaphore::~Semaphore()
{
}

inline
void bslmt::Semaphore::post()
{
    d_impl.post();
}

inline
void bslmt::Semaphore::post(int value)
{
    d_impl.post(value);
}

inline
int bslmt::Semaphore::tryWait()
{
    return d_impl.tryWait();
}

inline
void bslmt::Semaphore::wait()
{
    d_impl.wait();
}

// ACCESSORS
inline
int bslmt::Semaphore::getValue() const
{
    return d_impl.getValue();
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
